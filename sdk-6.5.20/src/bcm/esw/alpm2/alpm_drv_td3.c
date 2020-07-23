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

#if defined(BCM_TOMAHAWK_SUPPORT)

#define TD3_ALPM_BKT_BITS       13

#define TH_ENT_VALID_GET(u)                                     \
    soc_mem_field32_get(u, (mem), (ent),                        \
        (mem) == alpm_tbl[alpmTblPvtCmodeL1P128] ? VALID_LWRf : VALIDf)
#define TH_ENT_HIT_GET(u)                                       \
    soc_mem_field32_get(u, (mem), (ent),                        \
        (mem) == alpm_tbl[alpmTblPvtCmodeL1P128] ? HITf :       \
        x ? HIT1f : HIT0f)
#define TH_ENT_RPE_GET(u)                                       \
    soc_mem_field32_get(u, (mem), (ent),                        \
        (mem) == alpm_tbl[alpmTblPvtCmodeL1P128] ? RPEf :       \
        x ? RPE1f : RPE0f)
#define TH_ENT_DATA_TYPE_GET(u)                                 \
    soc_mem_field32_get(u, (mem), (ent),                        \
        (mem) == alpm_tbl[alpmTblPvtCmodeL1P128] ? DATA_TYPEf : \
        x ? DATA_TYPE1f : DATA_TYPE0f)
#define TH_ENT_ENTRY_VIEW_GET(u)                                 \
    soc_mem_field32_get(u, (mem), (ent),                        \
        (mem) == alpm_tbl[alpmTblPvtCmodeL1P128] ? ENTRY_VIEWf : \
        x ? ENTRY_VIEW1f : ENTRY_VIEW0f)
#define TH_ENT_DST_DISCARD_GET(u)                               \
    soc_mem_field32_get(u, (mem), (ent),                        \
        (mem) == alpm_tbl[alpmTblPvtCmodeL1P128] ? DST_DISCARDf :\
        x ? DST_DISCARD1f : DST_DISCARD0f)
#define TH_ENT_PRI_GET(u)                                       \
    soc_mem_field32_get(u, (mem), (ent),                        \
        (mem) == alpm_tbl[alpmTblPvtCmodeL1P128] ? PRIf :       \
        x ? PRI1f : PRI0f)
#define TH_ENT_CLASS_ID_GET(u)                                  \
    soc_mem_field32_get(u, (mem), (ent),                        \
        (mem) == alpm_tbl[alpmTblPvtCmodeL1P128] ? CLASS_IDf :  \
        x ? CLASS_ID1f : CLASS_ID0f)
#define TH_ENT_ACL_CLASS_ID_GET(u)                              \
    soc_mem_field32_get(u, (mem), (ent),                        \
        (mem) == alpm_tbl[alpmTblPvtCmodeL1P128] ? ACL_CLASS_IDf :  \
        x ? ACL_CLASS_ID1f : ACL_CLASS_ID0f)

/* Key & Mask */
#define TH_ENT_KEY_IPADDR_GET(u, key)                       \
    do {                                                    \
        int i;                                              \
        if (mem == alpm_tbl[alpmTblPvtCmodeL1P128]) {       \
            soc_field_t _key_fld_p[] =                      \
                {IP_ADDR0_LWRf, IP_ADDR1_LWRf,              \
                IP_ADDR0_UPRf, IP_ADDR1_UPRf};              \
            for (i = 0; i < 4; i++) {                       \
                key[i] = soc_mem_field32_get(u, mem, ent,   \
                    _key_fld_p[i]);                         \
            }                                               \
        } else {                                            \
            soc_field_t _key_fld[] = {IP_ADDR0f, IP_ADDR1f};\
            for (i = 0; i < 2-x; i++) {                     \
                if (pkm == ALPM_PKM_32B &&                  \
                    x == 0 && i == 1) {                     \
                    continue;                               \
                }                                           \
                key[i] = soc_mem_field32_get(u, mem, ent,   \
                    _key_fld[(i+x)%2]);                     \
            }                                               \
        }                                                   \
    } while (0)

#define TD3_IP6MC_KEY_IPADDR_GET(u, key)                    \
    do {                                                    \
        if (mem == alpm_tbl[alpmTblPvtCmodeL1P128]) {       \
            uint32 ip6_word[2];                                   \
            if (SOC_IS_FIREBOLT6(u)) {                            \
               soc_mem_field_get(u, mem, ent,                     \
                        IPMC_V6_ADDR_45_0f, ip6_word);            \
               key[0] = ip6_word[0];                              \
               key[1] = ip6_word[1] & 0x3fff;                     \
               soc_mem_field_get(u, mem, ent,                     \
                       IPMC_V6_ADDR_91_46f, ip6_word);            \
               key[1] |= ((ip6_word[0] & 0x3ffff) << 14);         \
               key[2] = ip6_word[0] >> 18;                        \
               key[2] |= ((ip6_word[1] & 0x3fff) << 14);          \
               soc_mem_field_get(u, mem, ent,                     \
                       IPMC_V6_ADDR_127_92f, ip6_word);           \
               key[2] |= (ip6_word[0] & 0xf) << 28;               \
               key[3] = (((ip6_word[1] & 0xf) << 28) |            \
                            (ip6_word[0] >> 4));                  \
            } else {                                              \
                soc_mem_field_get(u, mem, ent,                    \
                        IPMC_V6_ADDR_43_0f, ip6_word);            \
                    key[0] = ip6_word[0];                         \
                    key[1] = ip6_word[1] & 0xfff;                 \
                    soc_mem_field_get(u, mem, ent,                \
                            IPMC_V6_ADDR_87_44f, ip6_word);       \
                    key[1] |= ((ip6_word[0] & 0xfffff) << 12);    \
                    key[2] = ip6_word[0] >> 20;                   \
                    key[2] |= ((ip6_word[1] & 0xfff) << 12);      \
                    soc_mem_field_get(u, mem, ent,                \
                            IPMC_V6_ADDR_127_88f, ip6_word);      \
                    key[2] |= (ip6_word[0] & 0xff) << 24;         \
                    key[3] = (((ip6_word[1] & 0xff) << 24) |      \
                               (ip6_word[0] >> 8));               \
            }                                                     \
        } else {                                                  \
            uint32 ip6_word;                                      \
            if (SOC_IS_FIREBOLT6(u)) {                            \
                soc_mem_field_get(u, mem, ent,                    \
                   IPMC_V6_ADDR_109_64f, key);                    \
                ip6_word = soc_mem_field32_get(u, mem,            \
                   ent, IPMC_V6_ADDR_127_110f);                   \
                key[1] |= ((ip6_word & 0x3ffff) << 14);           \
            } else {                                              \
                soc_mem_field_get(u, mem, ent,                    \
                   IPMC_V6_ADDR_107_64f, key);                    \
                ip6_word = soc_mem_field32_get(u, mem,            \
                   ent, IPMC_V6_ADDR_127_108f);                   \
                   key[1] |= ((ip6_word & 0xfffff) << 12);        \
            }                                                     \
        }                                                         \
    } while (0)

#define TD3_IP6MC_MASK_IPADDR_GET(u, key)                         \
    do {                                                          \
        if (mem == alpm_tbl[alpmTblPvtCmodeL1P128]) {             \
            uint32 ip6_word[2];                                   \
                        if (SOC_IS_FIREBOLT6(u)) {                \
               soc_mem_field_get(u, mem, ent,                     \
                        IPMC_V6_ADDR_45_0_MASKf, ip6_word);       \
               key[0] = ip6_word[0];                              \
               key[1] = ip6_word[1] & 0x3fff;                     \
               soc_mem_field_get(u, mem, ent,                     \
                       IPMC_V6_ADDR_91_46_MASKf, ip6_word);       \
               key[1] |= ((ip6_word[0] & 0x3ffff) << 14);         \
               key[2] = ip6_word[0] >> 18;                        \
               key[2] |= ((ip6_word[1] & 0x3fff) << 14);          \
               soc_mem_field_get(u, mem, ent,                     \
                       IPMC_V6_ADDR_127_92_MASKf, ip6_word);      \
               key[2] |= (ip6_word[0] & 0xf) << 28;               \
               key[3] = (((ip6_word[1] & 0xf) << 28) |            \
                            (ip6_word[0] >> 4));                  \
            } else {                                              \
                soc_mem_field_get(u, mem, ent,                    \
                        IPMC_V6_ADDR_43_0_MASKf, ip6_word);       \
                    key[0] = ip6_word[0];                         \
                    key[1] = ip6_word[1] & 0xfff;                 \
                    soc_mem_field_get(u, mem, ent,                \
                            IPMC_V6_ADDR_87_44_MASKf, ip6_word);  \
                    key[1] |= ((ip6_word[0] & 0xfffff) << 12);    \
                    key[2] = ip6_word[0] >> 20;                   \
                    key[2] |= ((ip6_word[1] & 0xfff) << 12);      \
                    soc_mem_field_get(u, mem, ent,                \
                            IPMC_V6_ADDR_127_88_MASKf, ip6_word); \
                    key[2] |= (ip6_word[0] & 0xff) << 24;         \
                    key[3] = (((ip6_word[1] & 0xff) << 24) |      \
                               (ip6_word[0] >> 8));               \
            }                                                     \
        } else {                                                  \
            uint32 ip6_word;                                      \
            if (SOC_IS_FIREBOLT6(u)) {                            \
                soc_mem_field_get(u, mem, ent,                    \
                   IPMC_V6_ADDR_109_64_MASKf, key);               \
                ip6_word = soc_mem_field32_get(u, mem,            \
                   ent, IPMC_V6_ADDR_127_110_MASKf);              \
                key[1] |= ((ip6_word & 0x3ffff) << 14);           \
            } else {                                              \
                soc_mem_field_get(u, mem, ent,                    \
                   IPMC_V6_ADDR_107_64_MASKf, key);               \
                ip6_word = soc_mem_field32_get(u, mem,            \
                   ent, IPMC_V6_ADDR_127_108_MASKf);              \
                   key[1] |= ((ip6_word & 0xfffff) << 12);        \
            }                                                     \
        }                                                         \
    } while (0)

#define TH_ENT_KEY_VRF_ID_GET(u, vrf_id)                    \
    do {                                                    \
        int i;                                              \
        uint32 key[3] = {0};                                \
        if (mem == alpm_tbl[alpmTblPvtCmodeL1P128]) {       \
            soc_field_t _key_fld_p[] =                      \
                {GLOBAL_HIGHf, GLOBAL_ROUTEf,               \
                VRF_ID_0_LWRf};                             \
            for (i = 0; i < 3; i++) {                       \
                key[i] = soc_mem_field32_get(u, mem, ent,   \
                    _key_fld_p[i]);                         \
            }                                               \
        } else {                                            \
            soc_field_t _key_fld_0[] =                      \
                {GLOBAL_HIGH0f, GLOBAL_ROUTE0f,             \
                VRF_ID_0f};                                 \
            soc_field_t _key_fld_1[] =                      \
                {GLOBAL_HIGH1f, GLOBAL_ROUTE1f,             \
                VRF_ID_1f};                                 \
            for (i = 0; i < 3; i++) {                       \
                key[i] = soc_mem_field32_get(u, mem, ent,   \
                    x ? _key_fld_1[i] : _key_fld_0[i]);     \
            }                                               \
        }                                                   \
        if (key[1]) {                                       \
            (vrf_id) = key[0] ? ALPM_VRF_ID_GHI(u) :        \
                        ALPM_VRF_ID_GLO(u);                 \
        } else {                                            \
            (vrf_id) = key[2];                              \
        }                                                   \
    } while (0)

#define TD3_IP6MC_KEY_VRF_ID_GET(u, vrf_id)                 \
    do {                                                    \
        int ghi = 0;                                        \
        int grt = 0;                                        \
        if (mem == alpm_tbl[alpmTblPvtCmodeL1P128]) {       \
           ghi = soc_mem_field32_get(u, mem, ent,           \
               GLOBAL_HIGHf);                               \
           grt = soc_mem_field32_get(u, mem, ent,           \
               GLOBAL_ROUTEf);                              \
           if (ghi || grt) {                                \
               vrf_id = ghi ? ALPM_VRF_ID_GHI(u) :          \
                        ALPM_VRF_ID_GLO(u);                 \
           } else {                                         \
               if (SOC_IS_FIREBOLT6(u)) {                   \
                  vrf_id = soc_mem_field32_get(u, mem, ent,    \
                      IPMC_V6_VRF_ID_9_0f);                    \
                  vrf_id |= (soc_mem_field32_get(u, mem, ent,  \
                      IPMC_V6_VRF_ID_12_10f) << 10);           \
               } else {                                        \
                   vrf_id = soc_mem_field32_get(u, mem, ent,   \
                       IPMC_V6_VRF_ID_3_0f);                   \
                   vrf_id |= (soc_mem_field32_get(u, mem, ent, \
                       IPMC_V6_VRF_ID_11_4f) << 4);            \
               }                                               \
           }                                                \
        } else {                                            \
           ghi = soc_mem_field32_get(u, mem, ent,           \
               GLOBAL_HIGH0f);                              \
           grt = soc_mem_field32_get(u, mem, ent,           \
               GLOBAL_ROUTE0f);                             \
           if (ghi || grt) {                                \
               vrf_id = ghi ? ALPM_VRF_ID_GHI(u) :          \
                        ALPM_VRF_ID_GLO(u);                 \
           } else {                                         \
               vrf_id = soc_mem_field32_get(u, mem, ent,    \
                   IPMC_V6_VRF_IDf);                        \
           }                                                \
        }                                                   \
    } while (0)


#define TH_ENT_KEY_KT_GET(u)                                        \
    soc_mem_field32_get(u, (mem), (ent),                            \
        (mem) == alpm_tbl[alpmTblPvtCmodeL1P128] ? KEY_TYPE0_LWRf : \
        x ? KEY_TYPE1f : KEY_TYPE0f)
#define TH_ENT_KEY_KM_GET(u)                                        \
    soc_mem_field32_get(u, (mem), (ent),                            \
        (mem) == alpm_tbl[alpmTblPvtCmodeL1P128] ? KEY_MODE0_LWRf : \
        x ? KEY_MODE1f : KEY_MODE0f)

#define TH_ENT_MASK_IPADDR_GET(u, mask)                     \
    do {                                                    \
        int i;                                              \
        if (mem == alpm_tbl[alpmTblPvtCmodeL1P128]) {       \
            soc_field_t _msk_fld_p[] =                      \
                {IP_ADDR_MASK0_LWRf, IP_ADDR_MASK1_LWRf,    \
                IP_ADDR_MASK0_UPRf, IP_ADDR_MASK1_UPRf};    \
            for (i = 0; i < 4; i++) {                       \
                mask[i] = soc_mem_field32_get(u, mem, ent,  \
                    _msk_fld_p[i]);                         \
            }                                               \
        } else {                                            \
            soc_field_t _msk_fld[] =                        \
                {IP_ADDR_MASK0f, IP_ADDR_MASK1f};           \
            for (i = 0; i < 2-x; i++) {                     \
                if (pkm == ALPM_PKM_32B &&                  \
                    x == 0 && i == 1) {                     \
                    continue;                               \
                }                                           \
                mask[i] = soc_mem_field32_get(u, mem, ent,  \
                    _msk_fld[(i+x)%2]);                     \
            }                                               \
        }                                                   \
    } while (0)

/* Destination */
#define TH_ENT_DEST_GET(u, ecmp, ipmc, nh_idx)                          \
    do {                                                                \
        soc_format_t fmt = DESTINATION_FORMATfmt;                       \
        uint32 dt0, dt1, dv;                                            \
        if ((mem) == alpm_tbl[alpmTblPvtCmodeL1P128]) {                 \
            dv = soc_mem_field32_get(u, (mem), (ent), DESTINATIONf);    \
        } else {                                                        \
            dv = soc_mem_field32_get(u, (mem), (ent),                   \
                x ? DESTINATION1f : DESTINATION0f);                     \
        }                                                               \
        dt0 = soc_format_field32_get(u, fmt, &dv, DEST_TYPE0f);         \
        dt1 = soc_format_field32_get(u, fmt, &dv, DEST_TYPE1f);         \
        if (dt0 == 3) {   /* Should check dt0 first */                  \
            ecmp = 0;                                                   \
            ipmc = 0;                                                   \
            nh_idx = soc_format_field32_get(u, fmt, &dv, NEXT_HOP_INDEXf);\
        } else if (dt1 == 1) {                                          \
            ecmp = 1;                                                   \
            ipmc = 0;                                                   \
            nh_idx = soc_format_field32_get(u, fmt, &dv, ECMP_GROUPf);  \
        } else if (dt1 == 3) {                                          \
            ecmp = 0;                                                   \
            ipmc = 1;                                                   \
            nh_idx = soc_format_field32_get(u, fmt, &dv, IPMC_GROUPf);  \
        }                                                               \
    } while (0)

/* Bucket idx */
#define TH_ENT_BKT_PTR_GET(u)                                           \
    soc_mem_field32_get(u, (mem), (ent),                                \
        (mem) == alpm_tbl[alpmTblPvtCmodeL1P128] ? ALG_BKT_PTRf :       \
        x ? ALG_BKT_PTR1f : ALG_BKT_PTR0f)
#define TH_ENT_SUB_BKT_PTR_GET(u)                                       \
    soc_mem_field32_get(u, (mem), (ent),                                \
        (mem) == alpm_tbl[alpmTblPvtCmodeL1P128] ? ALG_SUB_BKT_PTRf :   \
        x ? ALG_SUB_BKT_PTR1f : ALG_SUB_BKT_PTR0f)
#define TH_ENT_DEFAULT_MISS_GET(u)                                      \
    soc_mem_field32_get(u, (mem), (ent),                                \
        (mem) == alpm_tbl[alpmTblPvtCmodeL1P128] ? DEFAULT_MISSf :      \
        x ? DEFAULT_MISS1f : DEFAULT_MISS0f)

/* Flex counter */
#define TH_ENT_FC_BASE_ID_GET(u)                            \
    soc_mem_field32_get(u, (mem), (ent),                    \
        (mem) == alpm_tbl[alpmTblPvtCmodeL1P128] ? FLEX_CTR_BASE_COUNTER_IDXf : \
        x ? FLEX_CTR_BASE_COUNTER_IDX1f : FLEX_CTR_BASE_COUNTER_IDX0f)
#define TH_ENT_FC_MODE_GET(u)                               \
    soc_mem_field32_get(u, (mem), (ent),                    \
        (mem) == alpm_tbl[alpmTblPvtCmodeL1P128] ? FLEX_CTR_OFFSET_MODEf :  \
        x ? FLEX_CTR_OFFSET_MODE1f : FLEX_CTR_OFFSET_MODE0f)
#define TH_ENT_FC_POOL_GET(u)                               \
    soc_mem_field32_get(u, (mem), (ent),                    \
        (mem) == alpm_tbl[alpmTblPvtCmodeL1P128] ? FLEX_CTR_POOL_NUMBERf :  \
        x ? FLEX_CTR_POOL_NUMBER1f : FLEX_CTR_POOL_NUMBER0f)

/* IPMC */
#define TH_ENT_MC_CPU_GET(u)                                \
    soc_mem_field32_get(u, (mem), (ent),                    \
        (mem) == alpm_tbl[alpmTblPvtCmodeL1P128] ? IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPUf :  \
        x ? IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU1f : IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU0f)
#define TH_ENT_MC_DROP_GET(u)                               \
    soc_mem_field32_get(u, (mem), (ent),                    \
        (mem) == alpm_tbl[alpmTblPvtCmodeL1P128] ? IPMC_EXPECTED_L3_IIF_MISMATCH_DROPf :  \
        x ? IPMC_EXPECTED_L3_IIF_MISMATCH_DROP1f : IPMC_EXPECTED_L3_IIF_MISMATCH_DROP0f)
#define TH_ENT_MC_IIF_GET(u)                                \
    soc_mem_field32_get(u, (mem), (ent),                    \
        (mem) == alpm_tbl[alpmTblPvtCmodeL1P128] ? EXPECTED_L3_IIFf :  \
        x ? EXPECTED_L3_IIF1f : EXPECTED_L3_IIF0f)

/* ALPM bucket entry get */
#define TH_BKT_VALID_GET(u)                                \
    soc_format_field32_get(u, (fmt), (ent), VALIDf)
#define TH_BKT_LENGTH_GET(u)                               \
    soc_format_field32_get(u, (fmt), (ent), LENGTHf)
#define TH_BKT_KEY_GET(u, key)                             \
    soc_format_field_get(u, (fmt), (ent), KEYf, key)
#define TH_BKT_SUB_BKT_PTR_GET(u)                          \
    soc_format_field32_get(u, (fmt), (ent), SUB_BKT_PTRf)
#define TH_BKT_CLASS_ID_GET(u)                             \
    soc_format_field32_get(u, (fmt), (ent), CLASS_IDf)
#define TH_BKT_DST_DISCARD_GET(u)                          \
    soc_format_field32_get(u, (fmt), (ent), DST_DISCARDf)
#define TH_BKT_RPE_GET(u)                                  \
    soc_format_field32_get(u, (fmt), (ent), RPEf)
#define TH_BKT_PRI_GET(u)                                  \
    soc_format_field32_get(u, (fmt), (ent), PRIf)
#define TH_BKT_ACL_CLASS_ID_GET(u)                         \
    soc_format_field32_get(u, (fmt), (ent), ACL_CLASS_IDf)

#define TH_BKT_DEST_GET(u, ecmp, ipmc, nh_idx)                          \
    do {                                                                \
        soc_format_t dfmt = DESTINATION_FORMATfmt;                      \
        uint32 dt0, dt1, dv;                                            \
        dv = soc_format_field32_get(u, (fmt), (ent), DESTINATIONf);     \
        dt0 = soc_format_field32_get(u, dfmt, &dv, DEST_TYPE0f);        \
        dt1 = soc_format_field32_get(u, dfmt, &dv, DEST_TYPE1f);        \
        if (dt0 == 3) {   /* Should check dt0 first */                  \
            ecmp = 0;                                                   \
            ipmc = 0;                                                   \
            nh_idx = soc_format_field32_get(u, dfmt, &dv, NEXT_HOP_INDEXf);\
        } else if (dt1 == 1) {                                          \
            ecmp = 1;                                                   \
            ipmc = 0;                                                   \
            nh_idx = soc_format_field32_get(u, dfmt, &dv, ECMP_GROUPf); \
        } else if (dt1 == 3) {                                          \
            ecmp = 0;                                                   \
            ipmc = 1;                                                   \
            nh_idx = soc_format_field32_get(u, dfmt, &dv, IPMC_GROUPf); \
        }                                                               \
    } while (0)

/* Flex counter */
#define TH_BKT_FC_BASE_ID_GET(u)                           \
    soc_format_field32_get(u, (fmt), (ent), FLEX_CTR_BASE_COUNTER_IDXf)
#define TH_BKT_FC_MODE_GET(u)                              \
    soc_format_field32_get(u, (fmt), (ent), FLEX_CTR_OFFSET_MODEf)
#define TH_BKT_FC_POOL_GET(u)                              \
    soc_format_field32_get(u, (fmt), (ent), FLEX_CTR_POOL_NUMBERf)

/* Default FMT */
#define ALPM_FMT_PKM_32B        (1)
#define ALPM_FMT_PKM_64B        (2)
#define ALPM_FMT_PKM_128        (3)
#define ALPM_FMT_PKM_FLEX_32B   (4)
#define ALPM_FMT_PKM_FLEX_64B   (5)
#define ALPM_FMT_PKM_FLEX_128   (6)

static _alpm_fmt_info_t th_fmt_info[ALPM_FMT_CNT] = {
        {
        ALPM_FMT_RTE,
        6,      /* 6 different formats(ROUTE FORMAT) */
        {       /* For fmt_ent_max */
            0,  /* FMT=0, means we cannot put entry in this format */
            6,  /* IPV4 */
            4,  /* IPV6_64 */
            2,  /* IPV6_128 */
            4,  /* IPV4_1 */
            3,  /* IPV6_64_1 */
            2,  /* IPV6_128 */
            0,
            0,
            0,
            0,
            0,
            0,
        },
        {       /* For fmt_pfx_len */
            0,
            32,  /* ROUTE FMT1 */
            64,
            128,
            32,
            64,
            128,
            0,
            0,
            0,
            0,
            0,
            0,
        },
        {       /* fmt_bnk */
            INVALIDfmt,
            L3_DEFIP_ALPM_ROUTE_FMT1_FULLfmt,
            L3_DEFIP_ALPM_ROUTE_FMT2_FULLfmt,
            L3_DEFIP_ALPM_ROUTE_FMT3_FULLfmt,
            L3_DEFIP_ALPM_ROUTE_FMT4_FULLfmt,
            L3_DEFIP_ALPM_ROUTE_FMT5_FULLfmt,
            L3_DEFIP_ALPM_ROUTE_FMT3_FULLfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt
        },
        {       /* fmt_ent */
            INVALIDfmt,
            L3_DEFIP_ALPM_ROUTE_FMT1fmt,
            L3_DEFIP_ALPM_ROUTE_FMT2fmt,
            L3_DEFIP_ALPM_ROUTE_FMT3fmt,
            L3_DEFIP_ALPM_ROUTE_FMT4fmt,
            L3_DEFIP_ALPM_ROUTE_FMT5fmt,
            L3_DEFIP_ALPM_ROUTE_FMT3fmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt
        },
        {0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0},
    },  /* ROUTE fmt info */
    {
        ALPM_FMT_RTE,
        6,      /* 6 different formats(ROUTE FORMAT) */
        {       /* For fmt_ent_max */
            0,  /* FMT=0, means we cannot put entry in this format */
            6,  /* IPV4 */
            4,  /* IPV6_64 */
            2,  /* IPV6_128 */
            4,  /* IPV4_1 */
            3,  /* IPV6_64_1 */
            2,  /* IPV6_128 */
            0,
            0,
            0,
            0,
            0,
            0,
        },
        {       /* For fmt_pfx_len */
            0,
            32,  /* ROUTE FMT1 */
            64,
            128,
            32,
            64,
            128,
            0,
            0,
            0,
            0,
            0,
            0,
        },
        {       /* fmt_bnk */
            INVALIDfmt,
            L3_DEFIP_ALPM_ROUTE_FMT1_FULLfmt,
            L3_DEFIP_ALPM_ROUTE_FMT2_FULLfmt,
            L3_DEFIP_ALPM_ROUTE_FMT3_FULLfmt,
            L3_DEFIP_ALPM_ROUTE_FMT4_FULLfmt,
            L3_DEFIP_ALPM_ROUTE_FMT5_FULLfmt,
            L3_DEFIP_ALPM_ROUTE_FMT3_FULLfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt
        },
        {       /* fmt_ent */
            INVALIDfmt,
            L3_DEFIP_ALPM_ROUTE_FMT1fmt,
            L3_DEFIP_ALPM_ROUTE_FMT2fmt,
            L3_DEFIP_ALPM_ROUTE_FMT3fmt,
            L3_DEFIP_ALPM_ROUTE_FMT4fmt,
            L3_DEFIP_ALPM_ROUTE_FMT5fmt,
            L3_DEFIP_ALPM_ROUTE_FMT3fmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt
        },
        {0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0},
    },  /* ROUTE fmt info */
};

soc_field_t th_alpm_ent_fld[] = {
    ENTRY_0f, ENTRY_1f, ENTRY_2f, ENTRY_3f,
    ENTRY_4f, ENTRY_5f, ENTRY_6f
};

static soc_mem_t th_alpm_hit_tbl_mem[] = {
    L3_DEFIP_ALPM_HIT_ONLYm
};

/* Should be the same order as _alpm_tbl_t */
static soc_mem_t th_alpm_tbl[] = {
    INVALIDm,
    L3_DEFIPm,
    L3_DEFIPm,
    L3_DEFIP_PAIR_128m,
    L3_DEFIP_PAIR_128m,
    L3_DEFIP_ALPM_RAWm,
    L3_DEFIP_ALPM_RAWm,
    L3_DEFIP_ALPM_RAWm,
    L3_DEFIP_ALPM_RAWm,
    INVALIDm,
    INVALIDm,
    INVALIDm,
    INVALIDm
};

static soc_mem_t *alpm_tbl;

/* Combined mode Level2 Table */
static _alpm_bkt_pool_conf_t th_alpm_cmode_pool_l2 = {
    {NULL, NULL},  /* bnk_info, to be allocated */
    65536,  /* 16 Banks x 4K buckets */
    16,     /* 16 Banks per logical bucket */
    alpmTblBktCmodeL2,
    0,
    1,      /* Fixed FMT */
    NULL,
    &th_fmt_info[ALPM_FMT_RTE],
};

/* Parallel mode Level2 Table pool for Global Routes */
static _alpm_bkt_pool_conf_t th_alpm_pmode_pool_l2 = {
    {NULL, NULL},  /* bnk_info, to be allocated */
    65536,  /* 8 Banks x 8K buckets */
    8,      /* 8 Banks per bucket */
    alpmTblBktPmodeL2,
    0,
    1,      /* Fixed FMT */
    NULL,
    &th_fmt_info[ALPM_FMT_RTE],
};

/* uRPF configuration */
static _alpm_bkt_pool_conf_t th_alpm_urpf_cmode_pool_l2 = {
    {NULL, NULL},  /* bnk_info, to be allocated */
    32768,  /* 8 Banks x 4K buckets */
    8,      /* 8 Banks per logical bucket */
    alpmTblBktCmodeL2,
    0,
    1,      /* Fixed FMT */
    NULL,
    &th_fmt_info[ALPM_FMT_RTE],
};

static _alpm_bkt_pool_conf_t th_alpm_urpf_pmode_pool_gbl_l2 = {
    {NULL, NULL},  /* bnk_info, to be allocated */
    16384,  /* 4 Banks x 4K buckets */
    4,      /* 4 Banks per bucket */
    alpmTblBktPmodeGlbL2,
    0,
    1,      /* Fixed FMT */
    NULL,
    &th_fmt_info[ALPM_FMT_RTE],
};

static _alpm_bkt_pool_conf_t th_alpm_urpf_pmode_pool_prt_l2 = {
    {NULL, NULL},  /* bnk_info, to be allocated */
    16384,  /* 4 Banks x 4K buckets */
    4,      /* 4 Banks per bucket */
    alpmTblBktPmodePrtL2,
    0,
    1,      /* Fixed FMT */
    NULL,
    &th_fmt_info[ALPM_FMT_RTE],
};

#if defined(BCM_HELIX5_SUPPORT)
static _alpm_bkt_pool_conf_t hx5_alpm_cmode_pool_l2 = {
    {NULL, NULL},  /* bnk_info, to be allocated */
    24576,  /* 12 Banks x 2K buckets */
    12,     /* 12 Banks per logical bucket */
    alpmTblBktCmodeL2,
    0,
    1,      /* Fixed FMT */
    NULL,
    &th_fmt_info[ALPM_FMT_RTE],
};

/* Parallel mode Level2 Table pool for Global Routes */
static _alpm_bkt_pool_conf_t hx5_alpm_pmode_pool_l2 = {
    {NULL, NULL},  /* bnk_info, to be allocated */
    24576,  /* 6 Banks x 4K buckets */
    6,      /* 6 Banks per bucket */
    alpmTblBktPmodeL2,
    0,
    1,      /* Fixed FMT */
    NULL,
    &th_fmt_info[ALPM_FMT_RTE],
};

/* uRPF configuration */
static _alpm_bkt_pool_conf_t hx5_alpm_urpf_cmode_pool_l2 = {
    {NULL, NULL},  /* bnk_info, to be allocated */
    12288,  /* 6 Banks x 2K buckets */
    6,      /* 6 Banks per logical bucket */
    alpmTblBktCmodeL2,
    0,
    1,      /* Fixed FMT */
    NULL,
    &th_fmt_info[ALPM_FMT_RTE],
};

static _alpm_bkt_pool_conf_t hx5_alpm_urpf_pmode_pool_gbl_l2 = {
    {NULL, NULL},  /* bnk_info, to be allocated */
    6144,  /* 3 Banks x 2K buckets */
    3,      /* 3 Banks per bucket */
    alpmTblBktPmodeGlbL2,
    0,
    1,      /* Fixed FMT */
    NULL,
    &th_fmt_info[ALPM_FMT_RTE],
};

static _alpm_bkt_pool_conf_t hx5_alpm_urpf_pmode_pool_prt_l2 = {
    {NULL, NULL},  /* bnk_info, to be allocated */
    6144,  /* 3 Banks x 2K buckets */
    3,      /* 3 Banks per bucket */
    alpmTblBktPmodePrtL2,
    0,
    1,      /* Fixed FMT */
    NULL,
    &th_fmt_info[ALPM_FMT_RTE],
};
#endif

#if defined(BCM_FIREBOLT6_SUPPORT)


static _alpm_cb_t fb6_app0_alpm_1cb_template = {
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
        13,     /* 8K buckets = 13 bits */
        1,      /* Double Wide */
        4,      /* 4 physical banks per bucket */
        3,      /* 3 bits */
    },
    0                   /* Block 0 */
};

static _alpm_cb_t fb6_app0_alpm_1cb_template_128b = {
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
        13,     /* 8K buckets = 13 bits */
        1,      /* Double Wide */
        4,      /* 4 physical banks per bucket */
        3,      /* 3 bits */
    },
    0                   /* Block 0 */
};

static _alpm_cb_t fb6_app1_alpm_1cb_template = {
    ALPM_APP1,          /* app */
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
        12,     /* 4K buckets = 12 bits */
        1,      /* Double Wide */
        4,      /* 4 physical banks per bucket */
        3,      /* 3 bits */
    },
    0                   /* Block 0 */
};

static _alpm_cb_t fb6_app1_alpm_1cb_template_128b = {
    ALPM_APP1,          /* app */
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
        12,     /* 4K buckets = 12 bits */
        1,      /* Double Wide */
        4,      /* 4 physical banks per bucket */
        3,      /* 3 bits */
    },
    0                   /* Block 0 */
};



/* Combined mode Level2 Table */
static _alpm_bkt_pool_conf_t fb6_acl_alpm_cmode_pool_l2 = {
    {NULL, NULL},  /* bnk_info, to be allocated */
    32768,  /* 8 Banks x 4K buckets */
    8,      /* 8 Banks per logical bucket */
    alpmTblBktCmodeL2,
    0,
    1,      /* Fixed FMT */
    NULL,
    &th_fmt_info[ALPM_FMT_RTE],
};

static _alpm_bkt_pool_conf_t fb6_alpm_pool_acl[SOC_MAX_NUM_DEVICES];
#endif

static _alpm_bkt_pool_conf_t th_alpm_pool0[SOC_MAX_NUM_DEVICES];
static _alpm_bkt_pool_conf_t th_alpm_pool1[SOC_MAX_NUM_DEVICES];

static _alpm_cb_t th_alpm_1cb_template = {
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
        13,     /* 8K buckets = 13 bits */
        1,      /* Double Wide */
        8,      /* 8 physical banks per bucket */
        3,      /* 3 bits */
    },
    0                   /* Block 0 */
};

static _alpm_cb_t th_alpm_1cb_template_128b = {
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
        13,     /* 8K buckets = 13 bits */
        1,      /* Double Wide */
        8,      /* 8 physical banks per bucket */
        3,      /* 3 bits */
    },
    0                   /* Block 0 */
};

#if defined(BCM_HELIX5_SUPPORT)
static _alpm_cb_t hx5_alpm_1cb_template = {
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
    {   /* bnk_conf */
        {NULL, NULL}, /* bnk_pool pointer */
        12,     /* 4K buckets = 12 bits */
        1,      /* Double Wide */
        6,      /* 6 physical banks per bucket */
        3,      /* 3 bits */
        },
    0                   /* Block 0 */
};

static _alpm_cb_t hx5_alpm_1cb_template_128b = {
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
    {   /* bnk_conf */
        {NULL, NULL}, /* bnk_pool pointer */
        12,     /* 4K buckets = 12 bits */
        1,      /* Double Wide */
        6,      /* 6 physical banks per bucket */
        3,      /* 3 bits */
        },
    0                   /* Block 0 */
};

#endif
int th_alpm_bkt_hit_write(int u, int vrf_id, _alpm_cb_t *acb,
                          _alpm_tbl_t tbl, int ent_idx, int hit_val);
int th_alpm_ent_hit_move(int u, int vrf_id, _alpm_cb_t *acb,
                         _alpm_tbl_t tbl, int src_ent_idx, int dst_ent_idx);
int th_alpm_bnk_hit_move(int u, int vrf_id, _alpm_cb_t *acb,
                         _alpm_bkt_info_t *bkt_info,
                         _alpm_tbl_t tbl, int src_idx, int dst_idx);
int th_alpm_bnk_hit_clear(int u, int vrf_id, _alpm_cb_t *acb,
                          _alpm_bkt_info_t *bkt_info, _alpm_tbl_t tbl, int idx);
static int
th_alpm_hit_tbl_idx_get(int u, _alpm_cb_t *acb, _alpm_tbl_t tbl, int idx, int ent);

/* Adjust config based on dynamic configuration */
static int
th_alpm_cfg_adjust(int u, _alpm_cb_t *acb)
{
    int half_banks = 0;
    int pid_count = alpm_pid_count(acb->app);
    _alpm_bkt_pool_conf_t *bp_conf;
    int pid;

    if (acb->app == APP0 &&
        (SOC_URPF_STATUS_GET(u) || ALPM_MODE_CHK(u, BCM_ALPM_MODE_PARALLEL))) {
        ACB_BKT_DW(acb) = 0;
    } else {
        ACB_BKT_DW(acb) = 1;
    }

    if (acb->app == APP0 && (SOC_URPF_STATUS_GET(u) || ACB_BKT_DW(acb))) {
        /* Adjust bucket bits for Double wide mode and uRPF */
        ACB_BKT_BITS(acb) -= 1;
    }
#if defined(BCM_HELIX5_SUPPORT)
    if (soc_feature(u, soc_feature_alpm_addr_xlate)) {
        if (soc_alpm_cmn_banks_get(u) == 4) {
            ACB_PHY_BNK_PER_BKT(acb) = ACB_PHY_BNK_PER_BKT(acb) * 2 / 3;
            ACB_BNK_BITS(acb) = ACB_BNK_BITS(acb) * 2 / 3;
            for (pid = 0; pid < pid_count; pid++) {
                bp_conf = ACB_BKT_POOL(acb, pid);

                if (pid > 0 && bp_conf == ACB_BKT_POOL(acb, pid - 1)) {
                    continue;
                }
                BPC_BNK_CNT(bp_conf) = BPC_BNK_CNT(bp_conf) * 2 / 3;
                BPC_BNK_PER_BKT(bp_conf) = BPC_BNK_PER_BKT(bp_conf) * 2 / 3;
            }
        }
        return BCM_E_NONE;
    }
#endif
    if (SOC_IS_TRIDENT3X(u) && soc_alpm_cmn_banks_get(u) == 4) {
        half_banks = 1;
    }

    /* When ACL enabled, half banks are reserved for ACL. */
    if (ALPM_ACL_EN(u) && acb->app == APP0) {
        for (pid = 0; pid < pid_count; pid++) {
            bp_conf = ACB_BKT_POOL(acb, pid);

            if (pid > 0 && bp_conf == ACB_BKT_POOL(acb, pid-1)) {
                continue;
            }
            BPC_BNK_CNT(bp_conf) >>= 1;
            BPC_BNK_PER_BKT(bp_conf) >>= 1;
        }
    }

    /* Half banks mode */
    if (half_banks) {
        /* Adjust _alpm_cb_t */
        ACB_PHY_BNK_PER_BKT(acb) >>= 1;
        ACB_BNK_BITS(acb) -= 1;

        /* Adjust _alpm_bkt_pool_conf_t */
        for (pid = 0; pid < pid_count; pid++) {
            bp_conf = ACB_BKT_POOL(acb, pid);

            if (pid > 0 && bp_conf == ACB_BKT_POOL(acb, pid-1)) {
                continue;
            }
            BPC_BNK_CNT(bp_conf) >>= 1;
            BPC_BNK_PER_BKT(bp_conf) >>= 1;
        }
        ALPMC(u)->_alpm_half_banks = 1;
    }

    return BCM_E_NONE;
}

#define HX5_ALPM_BKT_ENT_MAX (6)
#define HX5_ALPM_PHY_BKT_BITS (12)

/* Convert <bnk>|<bkt> style index to TD3 style index */
#define TH_MEM_TAB_IDX_CONVERT(u, acb, tbl, index)                  \
    do {                                                    \
        if (tbl >= alpmTblBktCmodeL2) {                     \
            int bkt, bnk;                                   \
            bkt = ALPM_IDX_TO_BKT(acb, index);              \
            bnk = ALPM_IDX_TO_BNK(acb, index);              \
            bkt = (bkt << ACB_BKT_DW(acb)) +                \
                  bnk / ACB_PHY_BNK_PER_BKT(acb);           \
            bnk = bnk % ACB_PHY_BNK_PER_BKT(acb);           \
            index = !soc_feature(u, soc_feature_alpm_addr_xlate) ?  \
                    (bkt << ACB_BNK_BITS(acb) | bnk) :              \
                    (bnk << HX5_ALPM_PHY_BKT_BITS | bkt);               \
        }                                                   \
    } while (0)

int
th_mem_phy_index_get(int u, _alpm_cb_t *acb, _alpm_tbl_t tbl, int index)
{
    int phy_index;

    /* Convert index to TD3 style as below
     * -------------------------
     * |       bkt     |  bnk  |
     * -------------------------
     */
    TH_MEM_TAB_IDX_CONVERT(u, acb, tbl, index);

    phy_index = index;
    if (SOC_URPF_STATUS_GET(u) && acb->app == APP0) {
        switch (tbl) {
            case alpmTblBktPmodeGlbL2:
            case alpmTblBktPmodeGlbL3:
                /* Private(Pid 0)/Global(Pid 1),
                 * VRF=1 means Private(Pid 0) */
                if (!soc_feature(u, soc_feature_alpm_addr_xlate)) {
                    phy_index = index + ACB_BNK_PER_BKT(acb, 1);
                } else {
                    int phy_bnk;
                    int phy_bkt;
                    phy_bnk = phy_index >> HX5_ALPM_PHY_BKT_BITS;
                    phy_bkt = phy_index & ((1 << HX5_ALPM_PHY_BKT_BITS) -1);
                    phy_bnk += ACB_BNK_PER_BKT(acb, 1);
                    phy_index = (phy_bnk << HX5_ALPM_PHY_BKT_BITS) + phy_bkt;
                }
                break;
            default:
                break;
        }
    }
    if (alpm_acl_app(acb->app)) {
        switch (tbl) {
            case alpmTblBktCmodeL2:
            case alpmTblBktPmodeL2:
                phy_index += ACB_PHY_BNK_PER_BKT(acb);
                break;
            default:
                break;
        }
    }

    return phy_index;
}

int
th_mem_read(int u, _alpm_cb_t *acb, _alpm_tbl_t tbl,
             int index, void *entry_data, int no_cache)
{
    int rv = BCM_E_NONE;
    soc_mem_t mem;

    mem = alpm_tbl[tbl];
    index = th_mem_phy_index_get(u, acb, tbl, index);

    if (no_cache) {
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

/* WAR for TD3 only */
void
th_mem_urpf_pvt_flexctr_set(int u, soc_mem_t mem, void *entry)
{
    int i;
    soc_field_t fc_flds[] = {
        FLEX_CTR_POOL_NUMBER0f,
        FLEX_CTR_OFFSET_MODE0f,
        FLEX_CTR_BASE_COUNTER_IDX0f,
        FLEX_CTR_POOL_NUMBER1f,
        FLEX_CTR_OFFSET_MODE1f,
        FLEX_CTR_BASE_COUNTER_IDX1f,
        FLEX_CTR_POOL_NUMBERf,
        FLEX_CTR_OFFSET_MODEf,
        FLEX_CTR_BASE_COUNTER_IDXf
    };
    for (i = 0; i < (sizeof(fc_flds)/sizeof(fc_flds[0])); i++) {
        if (soc_mem_field_valid(u, mem, fc_flds[i])) {
            soc_mem_field32_set(u, mem, entry, fc_flds[i], 0);
        }
    }
}

void
th_mem_urpf_pvt_dr_sd_set(int u, int app, _alpm_pvt_node_t *pnode, soc_mem_t mem, void *entry)
{
    int i, rv;
    int pkm = ALPM_PKM_32B;
    uint32 vld;
    _alpm_pvt_node_t *pvt_node = NULL;
    _alpm_bkt_node_t *bkt_node = NULL;
    int ip4mc = 0;

    soc_field_t vld_fld[] = {
        VALID0f,
        VALID1f,
        VALID_LWRf
    };

    soc_field_t key_km_fld[] = {
        KEY_MODE0f,
        KEY_MODE1f,
        KEY_MODE0_UPRf
    };

    soc_field_t dft_fld[] = {
        DEFAULTROUTE0f,
        DEFAULTROUTE1f,
        DEFAULTROUTEf
    };

    soc_field_t src_dc[] = {
        SRC_DISCARD0f,
        SRC_DISCARD1f,
        SRC_DISCARDf
    };

    soc_field_t key_kt_fld[] = {
        KEY_TYPE0f,
        KEY_TYPE1f,
        KEY_TYPE0_LWRf
    };

    for (i = 0; i < (sizeof(dft_fld)/sizeof(dft_fld[0])); i++) {
        if (soc_mem_field_valid(u, mem, vld_fld[i])) {
            vld = soc_mem_field32_get(u, mem, entry, vld_fld[i]);
            if (vld == 0) {
                continue;
            }

            /* Retrieve PKM from entry */
            pkm = soc_mem_field32_get(u, mem, entry, key_km_fld[i]);
            if (pkm == 3) { /* KEY_MODE 3 means PKM_128 */
                pkm = ALPM_PKM_128;
            }

            ip4mc = (soc_mem_field32_get(u, mem,
                     entry, key_kt_fld[i]) == ALPM_SEPARATE_IPV4MC_KEY_TYPE);

            if (ip4mc) {
                continue;
            }

            if (!(pkm == ALPM_PKM_64B && i == 1)) {
                int need_pvt_node = 0;
                if (pnode != NULL) {
                    /* pnode is specified, e.g.: bucket split adding pivot
                     * but pnode is yet to be inserted to the pvt tree
                     */
                    int sub_idx = (_tcam_pkm_uses_half_wide(u, pkm, 0) ? (PVT_IDX(pnode) & 1) : 0);
                    if (ALPM_IS_IPV6(ALPM_PKM2IPT(pkm)) || sub_idx == i) {
                        /* IPv6 or specified entry in IPv4 */
                        bkt_node = PVT_BKT_DEF(pnode);
                    } else {
                        /* Still need lookup for another half */
                        need_pvt_node = 1;
                    }
                } else {
                    /* pnode is not specified, need lookup */
                    need_pvt_node = 1;
                }

                if (need_pvt_node) {
                    rv = tcam_entry_pvt_node_get(u, app, pkm, entry, i, &pvt_node);
                    if (BCM_SUCCESS(rv)) {
                        bkt_node = PVT_BKT_DEF(pvt_node);
                    }
                }
            }
            soc_mem_field32_set(u, mem, entry, dft_fld[i],
                (bkt_node && bkt_node->key_len == 0) ? 1 : 0);
            soc_mem_field32_set(u, mem, entry, src_dc[i],
                (bkt_node && bkt_node->adata.defip_flags & BCM_L3_SRC_DISCARD) ? 1 : 0);
        }
    }

    return;
}

void
th_mem_urpf_pvt_bkt_ptr_set(int u, soc_mem_t mem, void *entry)
{
    int i;
    uint32 vld, bkt_ptr;
    uint32 urpf_offset;
    soc_field_t vld_fld[] = {
        VALID0f,
        VALID1f,
        VALID_LWRf
    };
    soc_field_t bkt_fld[] = {
        ALG_BKT_PTR0f,
        ALG_BKT_PTR1f,
        ALG_BKT_PTRf
    };
    urpf_offset = soc_feature(u, soc_feature_alpm_addr_xlate) ? 2048 : 4096;
    for (i = 0; i < (sizeof(bkt_fld)/sizeof(bkt_fld[0])); i++) {
        if (soc_mem_field_valid(u, mem, vld_fld[i])) {
            vld = soc_mem_field32_get(u, mem, entry, vld_fld[i]);
            if (vld == 0) {
                continue;
            }
            bkt_ptr = soc_mem_field32_get(u, mem, entry, bkt_fld[i]);
            if (bkt_ptr != 0) {
                soc_mem_field32_set(u, mem, entry, bkt_fld[i], bkt_ptr + urpf_offset);
            }
        }
    }

    return;
}

void
th_mem_urpf_pvt_data_type_set(int u, soc_mem_t mem, void *entry)
{
    int i;
    uint32 vld, dt_val;
    soc_field_t vld_fld[] = {
        VALID0f,
        VALID1f,
        VALID_LWRf
    };
    soc_field_t dt_fld[] = {
        DATA_TYPE0f,
        DATA_TYPE1f,
        DATA_TYPEf
    };
    for (i = 0; i < (sizeof(dt_fld)/sizeof(dt_fld[0])); i++) {
        if (soc_mem_field_valid(u, mem, vld_fld[i])) {
            vld = soc_mem_field32_get(u, mem, entry, vld_fld[i]);
            if (vld == 0) {
                continue;
            }
            dt_val = soc_mem_field32_get(u, mem, entry, dt_fld[i]);
            if (dt_val == 0) {
                soc_mem_field32_set(u, mem, entry, dt_fld[i], 1);
            }
        }
    }

    return;
}

void
th_mem_urpf_bkt_flexctr_set(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node,
                            int log_idx, void *entry)
{
    int i, eid, bnk;
    int vrf_id;
    uint8 fid;
    _alpm_bkt_info_t *bkt_info;
    soc_format_t bfmt, efmt;
    uint32 ent[17];
    soc_field_t fc_flds[] = {
        FLEX_CTR_POOL_NUMBERf,
        FLEX_CTR_OFFSET_MODEf,
        FLEX_CTR_BASE_COUNTER_IDXf
    };


    bkt_info = &PVT_BKT_INFO(pvt_node);
    vrf_id   = PVT_BKT_VRF(pvt_node);
    bnk = ALPM_IDX_TO_BNK(acb, log_idx);
    fid = bkt_info->bnk_fmt[bnk];

    bfmt = ACB_FMT_BNK(acb, vrf_id, fid);
    efmt = ACB_FMT_ENT(acb, vrf_id, fid);
    for (eid = 0; eid < ACB_FMT_ENT_MAX(acb, vrf_id, fid); eid++) {
        soc_format_field_get(u, bfmt, entry, th_alpm_ent_fld[eid], ent);
        for (i = 0; i < (sizeof(fc_flds)/sizeof(fc_flds[0])); i++) {
            if (soc_format_field_valid(u, efmt, fc_flds[i])) {
                soc_format_field32_set(u, efmt, ent, fc_flds[i], 0);
            }
        }
        /* Write entry back to bank */
        soc_format_field_set(u, bfmt, entry, th_alpm_ent_fld[eid], ent);
    }
}

void
th_mem_urpf_bkt_dr_sd_set(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node,
                          int log_idx, void *entry)
{
    int eid, bnk, rv;
    int vrf_id;
    uint8 fid;
    _alpm_bkt_info_t *bkt_info;
    alpm_lib_trie_t *bkt_trie;
    _alpm_bkt_node_t *bkt_node = NULL;
    soc_format_t bfmt, fmt;
    uint32 ent[17];

    bkt_trie = PVT_BKT_TRIE(pvt_node);
    bkt_info = &PVT_BKT_INFO(pvt_node);
    vrf_id   = PVT_BKT_VRF(pvt_node);
    bnk = ALPM_IDX_TO_BNK(acb, log_idx);
    fid = bkt_info->bnk_fmt[bnk];

    bfmt = ACB_FMT_BNK(acb, vrf_id, fid);
    fmt  = ACB_FMT_ENT(acb, vrf_id, fid);
    for (eid = 0; eid < ACB_FMT_ENT_MAX(acb, vrf_id, fid); eid++) {
        soc_format_field_get(u, bfmt, entry, th_alpm_ent_fld[eid], ent);
        if (soc_format_field32_get(u, fmt, ent, VALIDf)) {
            uint32 new_key[5], pfx[5];
            int pfx_len;
            int is_def = (soc_format_field32_get(u, fmt, ent, LENGTHf) == 0);
            soc_format_field32_set(u, fmt, ent, DEFAULTROUTEf, is_def ? 1 : 0);

            if (bkt_trie) {
                TH_BKT_KEY_GET(u, new_key);
                if (fid == ALPM_FMT_PKM_64B || fid == ALPM_FMT_PKM_FLEX_64B) {
                    new_key[3] = new_key[1];
                    new_key[2] = new_key[0];
                    new_key[1] = new_key[0] = 0;
                }
                pfx_len = TH_BKT_LENGTH_GET(u);
                alpm_trie_key_to_pfx(u, PVT_BKT_IPT(pvt_node), new_key, pfx_len, pfx);
                rv = alpm_lib_trie_search(bkt_trie, pfx, pfx_len, (alpm_lib_trie_node_t **)&bkt_node);
                if (BCM_SUCCESS(rv)) {
                    int is_srcdisc;
                    is_srcdisc = (bkt_node && bkt_node->adata.defip_flags & BCM_L3_SRC_DISCARD) ? 1 : 0;
                    soc_format_field32_set(u, fmt, ent, SRC_DISCARDf, is_srcdisc);
                }
            }
        }
        /* Write entry back to bank */
        soc_format_field_set(u, bfmt, entry, th_alpm_ent_fld[eid], ent);
    }
}

/* pvt_node is only used for writing pivot to bucket table  */
int
th_mem_write(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node,
             _alpm_tbl_t tbl, int index, void *entry_data, int update_data)
{
    int rv = BCM_E_NONE;
    int phy_index;
    soc_mem_t mem;
    void *entry = entry_data;
#if defined(BCM_FIREBOLT6_SUPPORT)
    int write_pair_data = 0;
    uint32  edata[SOC_MAX_MEM_FIELD_WORDS];
    uint32  ehit;
#endif

    mem = alpm_tbl[tbl];

    if (entry == NULL) {
        entry = soc_mem_entry_null(u, mem);
    }
    phy_index = th_mem_phy_index_get(u, acb, tbl, index);

#if defined(BCM_FIREBOLT6_SUPPORT)
    if (soc_feature(u, soc_feature_update_defip_pair_data_only) &&
        mem == L3_DEFIP_PAIR_128m) {
        if (update_data) {
            write_pair_data = 1;
        }
    }
#endif

    if (ALPMC(u)->_alpm_dbg_bmp & _ALPM_DBG_WRITE_CACHE_ONLY) {
        _soc_mem_alpm_write_cache(u, mem, MEM_BLOCK_ALL, phy_index, entry);
    } else {
#if defined(BCM_FIREBOLT6_SUPPORT)
        if (write_pair_data) {
            if (soc_mem_cache_get(u, mem, MEM_BLOCK_ALL)) {
                _soc_mem_alpm_write_cache(u, mem, MEM_BLOCK_ALL,
                                          phy_index, entry);
            }
            soc_mem_field_get(u, L3_DEFIP_PAIR_128m, entry, DATA_RANGEf, edata);
            BCM_IF_ERROR_RETURN(
                soc_mem_write(u, L3_DEFIP_PAIR_128_DATA_ONLYm, MEM_BLOCK_ALL,
                              phy_index, edata));
            soc_mem_field_get(u, L3_DEFIP_PAIR_128m, entry, HITf, &ehit);
            BCM_IF_ERROR_RETURN(
                soc_mem_write(u, L3_DEFIP_PAIR_128_HIT_ONLYm, MEM_BLOCK_ALL,
                              phy_index, &ehit));
        } else
#endif
        {
            rv = soc_mem_write(u, mem, MEM_BLOCK_ALL, phy_index, entry);
        }
#if defined (BCM_TRIDENT3_SUPPORT)
        if (tbl >= alpmTblBktCmodeL2) {
            int i;
            int8 fmt = 0;
            for (i = 0; i < ALPM_BPB_MAX; i++) {
                fmt |= pvt_node->bkt_info.bnk_fmt[i];
            }
            _soc_trident3_alpm2_bnk_fmt_set(u, phy_index,
                entry_data == NULL ? 0 : fmt);
        }
#endif
        if (SOC_URPF_STATUS_GET(u) && acb->app == APP0) {
            int urpf_index;

            /* This works for acl-en mode as well. */
            urpf_index = phy_index + (soc_mem_index_count(u, mem) >> 1);
            if (tbl < alpmTblBktCmodeL2 && ALPM_ACL_EN(u)) { /* level-1 */
                urpf_index = phy_index + (soc_mem_index_count(u, mem) >> 2);
            }
            if (soc_feature(u, soc_feature_alpm_addr_xlate)) {
                if (tbl >= alpmTblBktCmodeL2) {
                    int phy_bnk;
                    int phy_bkt;
                    phy_bnk = phy_index >> HX5_ALPM_PHY_BKT_BITS;
                    phy_bkt = phy_index & ((1 << HX5_ALPM_PHY_BKT_BITS) -1);
                    phy_bkt += 1 << (HX5_ALPM_PHY_BKT_BITS - 1);
                    urpf_index = (phy_bnk << HX5_ALPM_PHY_BKT_BITS) + phy_bkt;
                }
            }
            /* Pvt Table */
            if (entry_data != NULL) {
                if (tbl >= alpmTblBktCmodeL2) {
                    if (alpm_util_route_type_get(u, NULL, acb->app)) {
                        th_mem_urpf_bkt_flexctr_set(u, acb, pvt_node,
                                                    index, entry_data);
                    }
                    th_mem_urpf_bkt_dr_sd_set(u, acb, pvt_node, index, entry_data);
                } else {
                    if (alpm_util_route_type_get(u, NULL, acb->app)) {
                        th_mem_urpf_pvt_flexctr_set(u, mem, entry_data);
                    }
                    th_mem_urpf_pvt_dr_sd_set(u, acb->app, pvt_node, mem, entry_data);
                    th_mem_urpf_pvt_bkt_ptr_set(u, mem, entry_data);
                    th_mem_urpf_pvt_data_type_set(u, mem, entry_data);
                }
            }
#if defined(BCM_FIREBOLT6_SUPPORT)
            if (write_pair_data) {
                if (soc_mem_cache_get(u, mem, MEM_BLOCK_ALL)) {
                    _soc_mem_alpm_write_cache(u, mem, MEM_BLOCK_ALL,
                                              urpf_index, entry);
                }
                soc_mem_field_get(u, L3_DEFIP_PAIR_128m, entry, DATA_RANGEf, edata);
                BCM_IF_ERROR_RETURN(
                    soc_mem_write(u, L3_DEFIP_PAIR_128_DATA_ONLYm, MEM_BLOCK_ALL,
                                  urpf_index, edata));
                soc_mem_field_get(u, L3_DEFIP_PAIR_128m, entry, HITf, &ehit);
                BCM_IF_ERROR_RETURN(
                    soc_mem_write(u, L3_DEFIP_PAIR_128_HIT_ONLYm, MEM_BLOCK_ALL,
                                  urpf_index, &ehit));
            } else
#endif
            {
                rv = soc_mem_write(u, mem, MEM_BLOCK_ALL, urpf_index, entry);
            }
#if defined (BCM_TRIDENT3_SUPPORT)
            if (tbl >= alpmTblBktCmodeL2) {
                int i;
                int8 fmt = 0;
                for (i = 0; i < ALPM_BPB_MAX; i++) {
                    fmt |= pvt_node->bkt_info.bnk_fmt[i];
                }
                _soc_trident3_alpm2_bnk_fmt_set(u, urpf_index,
                    entry_data == NULL ? 0 : fmt);
            }
#endif
        }
    }

    if (BCM_SUCCESS(rv)) {
        acb->acb_cnt.c_mem[tbl][1]++;
        if (tbl >= alpmTblBktCmodeL2) {
            if (pvt_node != NULL) {
                ACB_PVT_PTR(acb, pvt_node, index) =
                    entry_data ? pvt_node : NULL;
            }
        }
    }

    return rv;
}

int
th_mem_bulk_write(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node,
                  _alpm_tbl_t tbl, int *index, uint32 **entry_data, int count)
{
    int i;
    int rv = BCM_E_NONE;

    for (i = 0; i < count; i++) {
        rv = th_mem_write(u, acb, pvt_node, tbl, index[i], entry_data[i], FALSE);
        ALPM_IER(rv);
    }

    return rv;
}

/*
int
th_mem_bulk_write(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node,
                  _alpm_tbl_t tbl, int *index, uint32 **entry_data, int count)
{
    int i;
    int phy_index[20];
    int copyno[20];
    soc_mem_t bulk_mem[20];
    int rv = BCM_E_NONE;

    assert(count <= 20);

    for (i = 0; i < count; i++) {
        phy_index[i] = th_mem_phy_index_get(u, acb, tbl, index[i]);
        copyno[i] = MEM_BLOCK_ALL;
        bulk_mem[i] = alpm_tbl[tbl];
    }

    if (ALPMC(u)->_alpm_dbg_bmp & _ALPM_DBG_WRITE_CACHE_ONLY) {
        for (i = 0; i < count; i++) {
            _soc_mem_alpm_write_cache(u, bulk_mem[i], copyno[i], phy_index[i],
                                      entry_data[i]);
        }
    } else {
        if (count <= ALPMC(u)->_alpm_bulk_wr_threshold) {
            for (i = 0; i < count; i++) {
                rv = soc_mem_write(u, bulk_mem[i], copyno[i], phy_index[i],
                                   entry_data[i]);
                ALPM_IER(rv);
            }

            if (SOC_URPF_STATUS_GET(u)) {
                for (i = 0; i < count; i++) {
                    int urpf_index;
                    urpf_index = phy_index[i] +
                        (soc_mem_index_count(u, bulk_mem[i]) >> 1);
                    rv = soc_mem_write(u, bulk_mem[i], copyno[i], urpf_index,
                                       entry_data[i]);
                    ALPM_IER(rv);
                }
            }

        }
        else {
            assert(0);
            rv = soc_mem_bulk_write(u, bulk_mem, phy_index,
                                    copyno, entry_data, count);
        }
    }

    if (BCM_SUCCESS(rv)) {
        for (i = 0; i < count; i++) {
            acb->acb_cnt.c_mem[tbl][1]++;
            acb->acb_cnt.c_bulk_mem[tbl][1]++;

            if (pvt_node != NULL) {
                ACB_PVT_PTR(acb, pvt_node, index[i]) = pvt_node;
            }
        }
    }

    return rv;
}
*/

/* ALPM related functions */

static int
th_alpm_cb_cnt(int u)
{
    int acb_cnt, alpm_enable;
    alpm_enable = soc_property_get(u, spn_L3_ALPM_ENABLE, 2);

    if (alpm_enable) {
        acb_cnt = 1;                    /* 1 ALPM Control Block */
    } else {
        acb_cnt = 0;
    }

    return acb_cnt;
}

void
th_alpm_ctrl_deinit(int u)
{
    int i, j, pid, ipt, acb_cnt;
    _alpm_cb_t *acb;
    int app, app_cnt = alpm_app_cnt(u);
    int pid_count;

    acb_cnt = th_alpm_cb_cnt(u);
    for (app = 0; app < app_cnt; app++) {
        for (i = 0; i < acb_cnt; i++) {
            acb = ACB(u, i, app);
            if (acb != NULL) {
                /* Free Bucket pool */
                pid_count = alpm_pid_count(app);
                for (pid = 0; pid < pid_count; pid++) {
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

    return ;
}

static void
th_alpm_hw_mem_clear(int u, soc_mem_t mem)
{
    int i;
    uint8 *mbuf;
    int size;
    soc_mem_t clr_mem[2] = {INVALIDm, INVALIDm};

    /* Clear table with ECC view to make sure
     * both data & ecc get cleared
     *
     * Clear L3_DEFIP_ALPM_RAWm as well to make sure
     * memory cache gets cleared as well.
     */
    clr_mem[0] = mem;
    if (mem == L3_DEFIP_ALPM_RAWm) {
        clr_mem[1] = L3_DEFIP_ALPM_ECCm;
    }

    for (i = 0; i < 2; i++) {
        if (clr_mem[i] == INVALIDm) {
            continue;
        }

        ALPM_INFO(("ALPM clearing mem %s\n", SOC_MEM_NAME(u, clr_mem[i])));

        size = SOC_MEM_TABLE_BYTES(u, clr_mem[i]);
        mbuf = soc_cm_salloc(u, size, "alpm_mem");
        if (mbuf == NULL) {
            ALPM_ERR(("th_alpm_hw_mem_clear alloc DMA memory failed\n"));
            return ;
        }
        sal_memset(mbuf, 0, size);

        (void)soc_mem_write_range(u, clr_mem[i], MEM_BLOCK_ALL,
                                  soc_mem_index_min(u, clr_mem[i]),
                                  soc_mem_index_max(u, clr_mem[i]), mbuf);
        soc_cm_sfree(u, mbuf);
    }

    return ;
}

static int
th_alpm_rpf_ctrl_init(int u)
{
    int         i;
    uint32      rval;
    soc_reg_t   reg = L3_DEFIP_RPF_CONTROLr;

#define FLDS_CNT    3
    soc_field_t flds[FLDS_CNT] = {
        LPM_MODEf,
        LOOKUP_MODEf,
        ENABLE_DOUBLE_WIDE_BUCKETS_FOR_IPV4f
    };

    uint32 vals[FLDS_CNT] = {
        1,
        0,
        1
    };

    /* Adjust LOOKUP_MODEf value */
    vals[1] = ALPM_MODE(u);
    SOC_IF_ERROR_RETURN(READ_L3_DEFIP_RPF_CONTROLr(u, &rval));
    for (i = 0; i < FLDS_CNT; i++) {
        if (SOC_REG_FIELD_VALID(u, reg, flds[i])) {
            soc_reg_field_set(u, reg, &rval, flds[i], vals[i]);
        }
    }
    SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_RPF_CONTROLr(u, rval));

    return BCM_E_NONE;
}

static int
th_alpm_key_sel_init(int u)
{
    int i, j, urpf_enb, pair_cnt, zoned, lpm_128b;
    uint32 *keys;
    uint32 rval = 0;
    soc_reg_t kselr = L3_DEFIP_KEY_SELr;
#define _TCAM_BLOCKS  4
    soc_field_t kself[_TCAM_BLOCKS] = {
        KEY_SEL_CAM0_1f,
        KEY_SEL_CAM2_3f,
        KEY_SEL_CAM4_5f,
        KEY_SEL_CAM6_7f
    };

    /* ATTENTION:
     * for -128 mode (Pair table enabled)
     * Combined: CAM0_1 and CAM2_3 are paired
     * Parallel/TCAM_ALPM: CAM0_1 and CAM4_5 are paired
     *
     * key_sel[7][]: Parallel mode + uRPF + 128 enable, that required 8 TCAM blocks to
     * support both single_wide and double_wide, but we only have 4, so
     * v4ipmc using single wide cannot be supported. so rollback all
     * setting to half and double wide
     */
    /*
     * 0: non-urpf + 128b
     * 1: urpf + 128b
     * 2: non-urpf + 64b
     * 3: urpf + 64b
     */
    /* 8: Zoned[bit2], URPF[bit1], 128B[bit0] */
    uint32 key_sel_ipmc[8][_TCAM_BLOCKS] = {
        {2, 2, 2, 2},   /* Combined, non-URPF, 64B */
        {0, 0, 2, 2},   /* Combined, non-URPF, 128B */
        {2, 2, 3, 3},   /* Combined, URPF, 64B */
        {0, 2, 1, 3},   /* Combined, URPF, 128B */
        {2, 2, 2, 2},   /* Parallel, non-URPF, 64B */
        {0, 2, 0, 2},   /* Parallel, non-URPF, 128B */
        {2, 2, 3, 3},   /* Parallel, URPF, 64B */
        {0, 0, 1, 1},   /* Parallel, URPF, 128B */
    };

    uint32 key_sel[8][_TCAM_BLOCKS] = {
        {2, 2, 2, 2},   /* Combined, non-URPF, 64B */
        {0, 0, 0, 0},   /* Combined, non-URPF, 128B */
        {2, 2, 3, 3},   /* Combined, URPF, 64B */
        {0, 0, 1, 1},   /* Combined, URPF, 128B */
        {2, 2, 2, 2},   /* Parallel, non-URPF, 64B */
        {0, 0, 0, 0},   /* Parallel, non-URPF, 128B */
        {2, 2, 3, 3},   /* Parallel, URPF, 64B */
        {0, 0, 1, 1},   /* Parallel, URPF, 128B */
    };

    urpf_enb = !!SOC_URPF_STATUS_GET(u);
    pair_cnt = ALPM_TCAM_PAIR_BLK_CNT(u);
    zoned    = ALPM_TCAM_ZONED(u, APP0);

    j = (zoned << 2) | (urpf_enb << 1) | (pair_cnt > 0);

    keys = soc_feature(u, soc_feature_separate_key_for_ipmc_route) ? key_sel_ipmc[j] : key_sel[j];
    for (i = 0; i < _TCAM_BLOCKS; i++) {
        soc_reg_field_set(u, kselr, &rval, kself[i], keys[i]);
    }

    if (!zoned && !urpf_enb && pair_cnt && pair_cnt != _TCAM_BLOCKS/2) {
        for (i = 0; i < _TCAM_BLOCKS; i++) {
            soc_reg_field_set(u, kselr, &rval, kself[i], keys[i < pair_cnt ? 0 : 2]);
        }
    }


    if (ALPM_ACL_EN(u)) {
        lpm_128b = !!SOC_ALPM_128B_ENABLE(u);
#define  DST_128      0
#define  SRC_128      1
#define  DST_64       2
#define  SRC_64       3

        if (lpm_128b) {
            soc_reg_field_set(u, kselr, &rval, KEY_SEL_CAM0_1f, DST_128);
            soc_reg_field_set(u, kselr, &rval, KEY_SEL_CAM2_3f,
                              urpf_enb ? SRC_128: DST_128);
            soc_reg_field_set(u, kselr, &rval, KEY_SEL_CAM4_5f, DST_128);
            soc_reg_field_set(u, kselr, &rval, KEY_SEL_CAM6_7f, SRC_128);

        } else {
            soc_reg_field_set(u, kselr, &rval, KEY_SEL_CAM0_1f, DST_64);
            soc_reg_field_set(u, kselr, &rval, KEY_SEL_CAM2_3f,
                              urpf_enb ? SRC_64: DST_64);
            soc_reg_field_set(u, kselr, &rval, KEY_SEL_CAM4_5f, DST_64);
            soc_reg_field_set(u, kselr, &rval, KEY_SEL_CAM6_7f, SRC_64);
        }
    }

    BCM_IF_ERROR_RETURN(WRITE_L3_DEFIP_KEY_SELr(u, rval));
    return BCM_E_NONE;
}

static int
th_alpm_cfg_init(int u)
{
    soc_reg_t creg = L3_DEFIP_ALPM_CFGr;
    uint32 rval = 0;
    if (ALPM_TCAM_ZONED(u, APP0)) {
        if (SOC_URPF_STATUS_GET(u)) {
            /* 4 DIPs, 4 SIPs */
            /* *DIP */
            soc_reg_field_set(u, creg, &rval, CAM2_3_SELf, 1);
            /* VRF SIP */
            soc_reg_field_set(u, creg, &rval, CAM4_5_SELf, 2);
            /* *SIP */
            soc_reg_field_set(u, creg, &rval, CAM6_7_SELf, 3);
        } else {
            soc_reg_field_set(u, creg, &rval, CAM4_5_SELf, 1);
            soc_reg_field_set(u, creg, &rval, CAM6_7_SELf, 1);
        }
    } else {
        if (SOC_URPF_STATUS_GET(u)) {
            /* 4 DIPs, 4 SIPs */
            soc_reg_field_set(u, creg, &rval, CAM4_5_SELf, 2);
            soc_reg_field_set(u, creg, &rval, CAM6_7_SELf, 2);
        }
    }

    if (ALPM_ACL_EN(u)) {
        assert(ALPM_TCAM_ZONED(u, APP0) == 0);
        if (SOC_URPF_STATUS_GET(u)) {
            soc_reg_field_set(u, creg, &rval, CAM0_1_SELf, 0);
            soc_reg_field_set(u, creg, &rval, CAM2_3_SELf, 2);
            soc_reg_field_set(u, creg, &rval, CAM4_5_SELf, 1);
            soc_reg_field_set(u, creg, &rval, CAM6_7_SELf, 3);
        } else {
            soc_reg_field_set(u, creg, &rval, CAM0_1_SELf, 0);
            soc_reg_field_set(u, creg, &rval, CAM2_3_SELf, 0);
            soc_reg_field_set(u, creg, &rval, CAM4_5_SELf, 1);
            soc_reg_field_set(u, creg, &rval, CAM6_7_SELf, 3);
        }
    }

    BCM_IF_ERROR_RETURN(WRITE_L3_DEFIP_ALPM_CFGr(u, rval));

    return BCM_E_NONE;
}

#if defined(BCM_FIREBOLT6_SUPPORT)
static int
fb6_alpm_acl_mode_init(int u)
{
    uint32 rval = 0;
    if (ALPM_ACL_EN(u)) {
        soc_reg_field_set(u, ALPM_ACL_CFGr, &rval, ALPM_ACL_MODE_ENABLEf, 1);
    }
    BCM_IF_ERROR_RETURN(WRITE_ALPM_ACL_CFGr(u, rval));
    return BCM_E_NONE;
}
#endif

static int
th_alpm_hw_init(int u)
{
    int rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN(th_alpm_rpf_ctrl_init(u));
    BCM_IF_ERROR_RETURN(th_alpm_key_sel_init(u));
    BCM_IF_ERROR_RETURN(th_alpm_cfg_init(u));
#if defined(BCM_FIREBOLT6_SUPPORT)
    if (soc_feature(u, soc_feature_lpm_prefilter)) {
        BCM_IF_ERROR_RETURN(fb6_alpm_acl_mode_init(u));
    }
#endif

    return rv;
}

int
th_alpm_ctrl_init(int u)
{
    int rv = BCM_E_NONE;
    int i, j, pid, acb_cnt, vrf_id_cnt;
    int app, app_cnt;
    int pid_count = 0;

    int alloc_sz;
#if 0
    int sf, scale_factor = 1;
#endif

    soc_mem_t mem, allmems[20] = {INVALIDm};
    int m, mem_cnt = 0;

    /*
     *  b2(p1)  b1(p0)  bit0(128b)
     * ---------------------------
     *  0       0       0       -> 2-Level (64b)
     *  0       0       1       -> 2-Level (128b)
     */
    _alpm_cb_t *acb_template[4] = {
        &th_alpm_1cb_template,
        &th_alpm_1cb_template_128b,
    };

    _alpm_bkt_pool_conf_t *acb_bp_conf[8] = {
        &th_alpm_pool0[u],
        &th_alpm_pool0[u],
        &th_alpm_pool0[u],
        &th_alpm_pool0[u],
    };

    _alpm_bkt_pool_conf_t *acb_urpf_bp_conf[8] = {
        &th_alpm_pool0[u],
        &th_alpm_pool0[u],
        &th_alpm_pool0[u],
        &th_alpm_pool1[u],
    };

    _alpm_bkt_pool_conf_t *acb_bp_conf_template[8] = {
        &th_alpm_cmode_pool_l2,
        &th_alpm_pmode_pool_l2,
        &th_alpm_cmode_pool_l2,
        &th_alpm_pmode_pool_l2
    };

    _alpm_bkt_pool_conf_t *acb_urpf_bp_conf_template[8] = {
        &th_alpm_urpf_cmode_pool_l2,
        &th_alpm_urpf_pmode_pool_prt_l2,
        &th_alpm_urpf_cmode_pool_l2,
        &th_alpm_urpf_pmode_pool_gbl_l2
    };

    /* assert ALPM_MEM_ENT_MAX for largest TCAM/ALPM memory entry */
    assert(ALPM_MEM_ENT_MAX * sizeof(uint32)
           >= sizeof(defip_pair_128_entry_t));

    if (SOC_MEM_IS_VALID(u, L3_DEFIP_ALPM_RAWm)) {
        alpm_tbl = &th_alpm_tbl[0];
    }

#if defined(BCM_HELIX5_SUPPORT)
    if (soc_feature(u, soc_feature_alpm_addr_xlate)) {
        acb_bp_conf_template[0] = &hx5_alpm_cmode_pool_l2;
        acb_bp_conf_template[1] = &hx5_alpm_pmode_pool_l2;
        acb_bp_conf_template[2] = &hx5_alpm_cmode_pool_l2;
        acb_bp_conf_template[3] = &hx5_alpm_pmode_pool_l2;

        acb_urpf_bp_conf_template[0] = &hx5_alpm_urpf_cmode_pool_l2;
        acb_urpf_bp_conf_template[1] = &hx5_alpm_urpf_pmode_pool_prt_l2;
        acb_urpf_bp_conf_template[2] = &hx5_alpm_urpf_cmode_pool_l2;
        acb_urpf_bp_conf_template[3] = &hx5_alpm_urpf_pmode_pool_gbl_l2;

        acb_template[0] = &hx5_alpm_1cb_template;
        acb_template[1] = &hx5_alpm_1cb_template_128b;
    }
#endif
    /* Load scaling factor config
     * 8 means valid factors are 1,2,4,8...128
     */
#if 0
    scale_factor = soc_property_get(u, "l3_alpm_scaling_factor", 1);
    for (sf = 0; sf < 8; sf++) {
        if (scale_factor == (1 << sf)) {
            break;
        }
    }
    if (sf >= 8) {
        scale_factor = 1;
        sf = 0;
    }
#endif


#if defined(BCM_FIREBOLT6_SUPPORT)
    if (ALPM_ACL_EN(u)) {
        acb_template[0] = &fb6_app0_alpm_1cb_template;
        acb_template[1] = &fb6_app0_alpm_1cb_template_128b;
        acb_template[2] = &fb6_app1_alpm_1cb_template;
        acb_template[3] = &fb6_app1_alpm_1cb_template_128b;
    }
#endif
    /* Only for TD3 if no ALPM_BKT_RSVD enabled */
    if (!(ALPM_128B(u) && ALPM_BKT_RSVD(u))) {
        ALPM_BKT_SHARE_THRES(u) = soc_property_get(u, spn_ALPM_BKT_SHARE_BNK_USAGE_THRES, 0);
    }

    vrf_id_cnt = ALPM_VRF_ID_CNT(u);
    acb_cnt = th_alpm_cb_cnt(u);
    assert((acb_cnt * ALPM_PKM_CNT) < 20);

    app_cnt = alpm_app_cnt(u);
    for (app = 0; app < app_cnt; app++) {
        for (i = 0; i < acb_cnt; i++) {
            int idx;
            _alpm_cb_t *acb = NULL;

            alloc_sz = sizeof(_alpm_cb_t);
            ALPM_ALLOC_EG(acb, alloc_sz, "alpm_cb");
            idx = !!app << 1 | (ALPM_128B(u) & 1);
            sal_memcpy(acb, acb_template[idx], sizeof(_alpm_cb_t));
            acb->unit = u;
            acb->app = app;

            if (app == APP0) {
                /**
                 *  -------
                 *  |2|1|0|
                 *  -------
                 *  Bit 0: Zoned: Parallel/Mixed: 1, Combined: 0
                 *  Bit 1: Private(Pid 0)/Global(Pid 1)
                 *  ---------
 */
                pid_count = alpm_pid_count(app);
                for (pid = 0; pid < pid_count; pid++) {
                    idx = pid << 1 | !!ALPM_MODE_CHK(u, BCM_ALPM_MODE_PARALLEL);
                    if (SOC_URPF_STATUS_GET(u)) {
                        ACB_BKT_POOL(acb, pid) = acb_urpf_bp_conf[idx];
                        sal_memcpy(acb_urpf_bp_conf[idx],
                                   acb_urpf_bp_conf_template[idx],
                                   sizeof(_alpm_bkt_pool_conf_t));
                    } else {
                        ACB_BKT_POOL(acb, pid) = acb_bp_conf[idx];
                        sal_memcpy(acb_bp_conf[idx],
                                   acb_bp_conf_template[idx],
                                   sizeof(_alpm_bkt_pool_conf_t));
                    }
                }
            }
#if defined(BCM_FIREBOLT6_SUPPORT)
            else if (app == ALPM_APP1) {
                sal_memcpy(&fb6_alpm_pool_acl[u],
                           &fb6_acl_alpm_cmode_pool_l2,
                           sizeof(_alpm_bkt_pool_conf_t));
                ACB_BKT_POOL(acb, 0) = &fb6_alpm_pool_acl[u];
            } else if (app == ALPM_APP2) {
                /* APP2 sharing same pool with APP1. */
                ACB_BKT_POOL(acb, 0) = &fb6_alpm_pool_acl[u];
            }
#endif

            /* Adjust ctrl bnk conf
             * BPC should NOT be adjusted because they point to
             * global storage
             */
            ALPM_IEG(th_alpm_cfg_adjust(u, acb));

#define _CHK_IF_CLEAR_MEM(mem)                  \
        do {                                        \
            for (m = 0; m < mem_cnt; m++) {         \
                if (allmems[m] == (mem)) {          \
                    break;                          \
                }                                   \
            }                                       \
            if (m == mem_cnt && mem != INVALIDm &&  \
                soc_mem_index_count(u, mem) > 0) {  \
                th_alpm_hw_mem_clear(u, (mem));    \
                allmems[mem_cnt++] = (mem);         \
            }                                       \
        } while (0)

            pid_count = alpm_pid_count(app);

            for (pid = 0; pid < pid_count; pid++) {
                _alpm_bkt_pool_conf_t *bp_conf = ACB_BKT_POOL(acb, pid);

                /* APP2 shares the same bkt pool of APP1 */
                if (app == ALPM_APP2) {
                    continue;
                }

                if (pid > 0 && ACB_BKT_POOL(acb, pid-1) == bp_conf) {
                    continue;
                }

                /* Adjust bucket count based on scaling factor */
                /* BPC_BNK_CNT(bp_conf) /= scale_factor; */
                alloc_sz = sizeof(_alpm_bnk_info_t);
                ALPM_ALLOC_EG(BPC_BNK_INFO(bp_conf, ALPM_IPT_V4),
                              alloc_sz, "bnk_bmp");
                if (ALPM_128B(u) && ALPM_BKT_RSVD(u) &&
                    ACB_IDX(acb) == acb_cnt - 1) {
                    int rsvd_v6_bnks =
                        BPC_BNK_CNT(bp_conf) * ALPM_BKT_RSVD_CNT(u) /
                        ALPM_BKT_RSVD_TOTAL;
                    ALPM_ALLOC_EG(
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

                /* Reserve bucket 0 */
                SHR_BITSET_RANGE(
                    BPC_BNK_BMP(bp_conf, ALPM_IPT_V4), 0,
                    BPC_BNK_PER_BKT(bp_conf));

                if (!SOC_WARM_BOOT(u)) {
                    mem = alpm_tbl[BPC_BKT_TBL(bp_conf)];
                    _CHK_IF_CLEAR_MEM(mem);
                }

                if (bp_conf->pvt_ptr == NULL) {
                    alloc_sz = BPC_BNK_CNT(bp_conf);
                    alloc_sz = sizeof(void *) * alloc_sz;
                    ALPM_ALLOC_EG(bp_conf->pvt_ptr, alloc_sz, "pvt_ptr");
                }
            }

            /* Adjust bucket count based on scaling factor */
            /* ALPM_IDX_BNK_SHIFT(acb) -= sf; */

            /* Per instance structure init */
            alloc_sz = vrf_id_cnt * sizeof(_alpm_pvt_ctrl_t);
            for (j = 0; j < ALPM_IPT_CNT; j++) {
                ALPM_ALLOC_EG(
                    acb->pvt_ctl[j], alloc_sz, "pvt_ctl");
            }
            for (j = 0; j < ALPM_PKM_CNT; j++) {
                if (!SOC_WARM_BOOT(u)) {
                    mem = alpm_tbl[acb->pvt_tbl[j]];
                    _CHK_IF_CLEAR_MEM(mem);
                }
            }

            ACB(u, i, app) = acb;
        }

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

    ALPM_TCAM_HB_IS_PTR(u) = TRUE;

    ACB_CNT(u) = acb_cnt;

    if (!SOC_WARM_BOOT(u)) {
        rv = th_alpm_hw_init(u);
    }

    return rv;

bad:
    th_alpm_ctrl_deinit(u);
    return rv;
}

/* ALPM ctrl cleanup (UNAVAIL for faster delete_all) */
/*
int
th_alpm_ctrl_cleanup(int u)
{
    return BCM_E_UNAVAIL;
}
*/

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

static int8
th_alpm_bnk_fmt_get(int u, _alpm_cb_t *acb,
                     _alpm_bkt_info_t *bkt_info, int ent_idx)
{
    int bnk_idx;

    bnk_idx = ALPM_IDX_TO_BNK(acb, ent_idx);
    return bkt_info->bnk_fmt[bnk_idx];
}

static void
th_alpm_ent_assemble(int u, _alpm_cb_t *acb,
                      _alpm_pvt_node_t *pvt_node, int ent_idx,
                      _bcm_defip_cfg_t *lpm_cfg, uint32 *entry)
{
    int vrf_id, ipt;
    int rmode, offset;
    int8 fmt_tp;

    uint32 key[5] = {0};
    uint32 fent[ALPM_ENT_WORDS_MAX] = {0};
    soc_format_t ent_fmt, bnk_fmt;

    vrf_id  = ALPM_LPM_VRF_ID(u, lpm_cfg);
    ipt     = ALPM_LPM_IPT(u, lpm_cfg);
    fmt_tp  = th_alpm_bnk_fmt_get(u, acb, &PVT_BKT_INFO(pvt_node), ent_idx);
    ent_fmt = ACB_FMT_ENT(acb, vrf_id, fmt_tp);

#define _FMT32_SET(field, value) \
    soc_format_field32_set(u, ent_fmt, fent, (field), (value))

    _FMT32_SET(VALIDf, 1);
    /* TD3TBD */
    _FMT32_SET(SUB_BKT_PTRf, 0);
    /* Associated-Data */
    if (soc_feature(u, soc_feature_lpm_prefilter) &&
        alpm_acl_app(acb->app)) {
        _FMT32_SET(ACL_CLASS_ID_VALIDf, 1);
        _FMT32_SET(ACL_CLASS_IDf, lpm_cfg->defip_acl_class_id);
    } else if (soc_feature(u, soc_feature_generic_dest)) {
        soc_format_t dst_fmt = DESTINATION_FORMATfmt;
        uint32 dst_fmt_val = 0;
        if (lpm_cfg->defip_flags & BCM_L3_MULTIPATH) {
            soc_format_field32_set(u, dst_fmt, &dst_fmt_val, DEST_TYPE1f, 1);
            soc_format_field32_set(u, dst_fmt, &dst_fmt_val, ECMP_GROUPf,
                                   lpm_cfg->defip_ecmp_index);
        } else if (!(lpm_cfg->defip_flags & BCM_L3_IPMC)) {
            soc_format_field32_set(u, dst_fmt, &dst_fmt_val, DEST_TYPE0f, 3);
            soc_format_field32_set(u, dst_fmt, &dst_fmt_val, NEXT_HOP_INDEXf,
                                   lpm_cfg->defip_ecmp_index);
        }
        _FMT32_SET(DESTINATIONf, dst_fmt_val);
    } else {
        if (lpm_cfg->defip_flags & BCM_L3_MULTIPATH) {
            _FMT32_SET(ECMPf, 1);
            _FMT32_SET(ECMP_PTRf, lpm_cfg->defip_ecmp_index);
        } else if (!(lpm_cfg->defip_flags & BCM_L3_IPMC)) {
            _FMT32_SET(NEXT_HOP_INDEXf, lpm_cfg->defip_ecmp_index);
        }
    }
    _FMT32_SET(PRIf, lpm_cfg->defip_prio);
    _FMT32_SET(RPEf, lpm_cfg->defip_flags & BCM_L3_RPE ? 1 : 0);
    _FMT32_SET(DST_DISCARDf, lpm_cfg->defip_flags & BCM_L3_DST_DISCARD ? 1 : 0);
    _FMT32_SET(CLASS_IDf, lpm_cfg->defip_lookup_class);
    _FMT32_SET(SUB_BKT_PTRf, PVT_SUB_BKT_IDX(pvt_node));

    /* Flex Counter */
    rmode = ACB_VRF_DB_TYPE(u, acb, vrf_id, ipt);
    if (rmode) {
        _FMT32_SET(FLEX_CTR_POOL_NUMBERf, lpm_cfg->defip_flex_ctr_pool);
        _FMT32_SET(FLEX_CTR_OFFSET_MODEf, lpm_cfg->defip_flex_ctr_mode);
        _FMT32_SET(FLEX_CTR_BASE_COUNTER_IDXf, lpm_cfg->defip_flex_ctr_base_id);
    }

    /* Key & Length */
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
    alpm_util_cfg_to_key(u, ipt, lpm_cfg, key);
    if (ALPM_IS_IPV6(ipt)) {
        if (fmt_tp == ALPM_FMT_PKM_64B || fmt_tp == ALPM_FMT_PKM_FLEX_64B) {
            key[0] = key[2];
            key[1] = key[3];
            key[2] = key[3] = 0;
        }
    }

    soc_format_field_set(u, ent_fmt, fent, KEYf, key);
    _FMT32_SET(LENGTHf, lpm_cfg->defip_sub_len);

    offset  = ALPM_IDX_TO_ENT(ent_idx);
    bnk_fmt = ACB_FMT_BNK(acb, vrf_id, fmt_tp);
    soc_format_field_set(u, bnk_fmt, entry, th_alpm_ent_fld[offset], fent);

#undef _FMT32_SET

    return ;

}

int
th_alpm_ent_pfx_len_get(int u, _alpm_cb_t *acb,
                         _alpm_pvt_node_t *pvt_node, int ent_idx)
{
    int    rv;
    int    ent_id, pfx_len = -1;
    int8   fmt_tp;
    uint32 bnkb[17];
    uint32 entb[17] = {0};
    soc_format_t bnk_fmt, ent_fmt;
    _alpm_bkt_pool_conf_t *bp_conf;

    bp_conf = ACB_BKT_VRF_POOL(acb, PVT_BKT_VRF(pvt_node));
    /* Get Bank data: bnkb */
    rv = th_mem_read(u, acb, BPC_BKT_TBL(bp_conf),
                      ALPM_TAB_IDX_GET(ent_idx), bnkb, FALSE);
    if (BCM_SUCCESS(rv)) {
        fmt_tp = th_alpm_bnk_fmt_get(u, acb, &PVT_BKT_INFO(pvt_node), ent_idx);
        bnk_fmt = BPC_FMT_BNK(bp_conf, fmt_tp);
        ent_id  = ALPM_IDX_TO_ENT(ent_idx);

        /* Get Entry data: entb */
        (void)soc_format_field_get(u, bnk_fmt, bnkb,
                                   th_alpm_ent_fld[ent_id], entb);
        /* Get PREFIXf Field data: tmpf */
        ent_fmt = BPC_FMT_ENT(bp_conf, fmt_tp);
        pfx_len = soc_format_field32_get(u, ent_fmt, entb, LENGTHf);
    }

    return pfx_len;
}

void
th_alpm_ent_copy(int u, int vrf_id, _alpm_cb_t *acb, uint32 *sb_ent, uint32 *db_ent,
                  _alpm_bkt_info_t *sbinfo, _alpm_bkt_info_t *dbinfo,
                  uint32 src_pvt_len, uint32 dst_pvt_len,
                  uint32 src_idx, uint32 dst_idx)
{
    int8 src_fid, dst_fid;
    uint32 src_eid, dst_eid;
    uint32 se_ent[17] = {0};
    uint32 de_ent[17] = {0};

    soc_format_t sb_fmt, db_fmt;

    /* Retrieve src entry */
    src_fid = th_alpm_bnk_fmt_get(u, acb, sbinfo, src_idx);
    src_eid = ALPM_IDX_TO_ENT(src_idx);
    sb_fmt  = ACB_FMT_BNK(acb, vrf_id, src_fid);
    soc_format_field_get(u, sb_fmt, sb_ent, th_alpm_ent_fld[src_eid], se_ent);

    dst_fid = th_alpm_bnk_fmt_get(u, acb, dbinfo, dst_idx);
    dst_eid = ALPM_IDX_TO_ENT(dst_idx);
    db_fmt  = ACB_FMT_BNK(acb, vrf_id, dst_fid);

    /* Copy entry */
    if (sb_fmt != db_fmt) {
        assert(0);
    } else {
        sal_memcpy(de_ent, se_ent, sizeof(de_ent));
        if (BI_SUB_BKT_IDX(sbinfo) != BI_SUB_BKT_IDX(dbinfo)) {
            soc_format_field32_set(u, ACB_FMT_ENT(acb, vrf_id, dst_fid), de_ent,
                                   SUB_BKT_PTRf, BI_SUB_BKT_IDX(dbinfo));
        }
    }

    /* Write entry back to bank */
    soc_format_field_set(u, db_fmt, db_ent, th_alpm_ent_fld[dst_eid], de_ent);

    return ;
}

int
th_alpm_bkt_bnk_write(int u, _alpm_cb_t *acb,
                       _alpm_pvt_node_t *pvt_node,
                       _bcm_defip_cfg_t *lpm_cfg, int ent_idx, uint32 write_flags)
{
    int rv, index;
    uint32 entry[ALPM_ENT_WORDS_MAX] = {0};
    _alpm_bkt_pool_conf_t *bp_conf = ACB_BKT_VRF_POOL(acb, PVT_BKT_VRF(pvt_node));

    index   = ALPM_TAB_IDX_GET(ent_idx);

    rv = th_mem_read(u, acb, BPC_BKT_TBL(bp_conf), index, entry, FALSE);
    if (BCM_SUCCESS(rv)) {
        th_alpm_ent_assemble(u, acb, pvt_node, ent_idx, lpm_cfg, entry);
        rv = th_mem_write(u, acb, pvt_node, BPC_BKT_TBL(bp_conf), index, entry, FALSE);
    }

    return rv;
}

static int
th_alpm_bkt_bnk_shrink(int u, _alpm_cb_t *acb,
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

    _alpm_bkt_pool_conf_t *bp_conf;

    vrf_id = PVT_BKT_VRF(pvt_node);
    bp_conf = ACB_BKT_VRF_POOL(acb, vrf_id);

    bkt_info = &PVT_BKT_INFO(pvt_node);
    src_fid  = bkt_info->bnk_fmt[sbnk];
    dst_fid  = bkt_info->bnk_fmt[dbnk];
    sepb     = ACB_FMT_ENT_MAX(acb, vrf_id, src_fid);
    depb     = ACB_FMT_ENT_MAX(acb, vrf_id, dst_fid);

    if (bkt_info->vet_bmp[dbnk] == 0) {
        /* Move from entire from_bnk to to_bnk */
        stab_idx = ALPM_TAB_IDX_GET_BI_BNK(acb, bkt_info, sbnk);
        dtab_idx = ALPM_TAB_IDX_GET_BI_BNK(acb, bkt_info, dbnk);

        rv = th_mem_read(u, acb, BPC_BKT_TBL(bp_conf),
                          stab_idx, srcbnke, FALSE);
        ALPM_IER(rv);

        bulk_idx[0] = dtab_idx;
        bulk_idx[1] = stab_idx;
        bulk_ent[0] = srcbnke;
        bulk_ent[1] = nullent;

        sal_memset(nullent, 0, sizeof(nullent));
        rv = th_mem_bulk_write(u, acb, pvt_node, BPC_BKT_TBL(bp_conf),
                               bulk_idx, bulk_ent, 2);
        ALPM_IER(rv);

        sal_memset(srcbnke, 0, sizeof(srcbnke));
        /* Invalidate bank from */
        rv = th_mem_write(u, acb, pvt_node, BPC_BKT_TBL(bp_conf),
                           stab_idx, srcbnke, FALSE);
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
    rv = th_mem_read(u, acb, BPC_BKT_TBL(bp_conf),
                      stab_idx, srcbnke, FALSE);
    ALPM_IER(rv);
    dtab_idx = ALPM_TAB_IDX_GET_BI_BNK(acb, bkt_info, dbnk);
    rv = th_mem_read(u, acb, BPC_BKT_TBL(bp_conf),
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
                th_alpm_ent_copy(u, vrf_id, acb, srcbnke, dstbnke,
                            &PVT_BKT_INFO(pvt_node),
                            &PVT_BKT_INFO(pvt_node),
                            PVT_KEY_LEN(pvt_node),
                            PVT_KEY_LEN(pvt_node),
                            src_idx, dst_idx);
            } else {
                sal_memset(tempent, 0, sizeof(tempent));
                (void)soc_format_field_get(u, sb_fmt, srcbnke,
                                           th_alpm_ent_fld[j], tempent);
                (void)soc_format_field_set(u, db_fmt, dstbnke,
                                           th_alpm_ent_fld[i], tempent);
            }
            /* Invalidate entry j */
            sal_memset(nullent, 0, sizeof(nullent));
            (void)soc_format_field_set(u, sb_fmt, srcbnke,
                                       th_alpm_ent_fld[j], nullent);

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
    bulk_idx[1] = stab_idx;
    bulk_ent[0] = dstbnke;
    bulk_ent[1] = srcbnke;
    rv = th_mem_bulk_write(u, acb, pvt_node, BPC_BKT_TBL(bp_conf),
                           bulk_idx, bulk_ent, 2);
    return rv;
}

/* Prefix copy from opvt_node to npvt_node if opvt_node != npvt_node.
   Prefix merge (sort) from old bucket to new_bkt
   if opvt_node == npvt_node */
int
th_alpm_bkt_pfx_copy(int u, _alpm_cb_t *acb,
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

    src_bkt = &PVT_BKT_INFO(opvt_node);
    if (npvt_node != opvt_node) {
        dst_bkt = &PVT_BKT_INFO(npvt_node);
    } else {
        dst_bkt = new_bkt;
    }
    vrf_id  = PVT_BKT_VRF(opvt_node);
    bp_conf = ACB_BKT_VRF_POOL(acb, vrf_id);

    for (i = 0; i < pfx_arr->pfx_cnt; i++) {
        sb_cnt[ALPM_IDX_TO_BNK(acb, pfx_arr->pfx[i]->ent_idx)]++;
    }

    for (i = 0; i < ALPM_BPB_MAX; i++) {
        if (sb_cnt[i] == 0) {
            continue;
        }
        rv = th_mem_read(u, acb, BPC_BKT_TBL(bp_conf),
                         ALPM_TAB_IDX_GET_BI_BNK(acb, src_bkt, i),
                         srcbnke[i], FALSE);
        ALPM_IER(rv);
    }

    def_fmt = alpm_util_bkt_def_fmt_get(u, acb, vrf_id, PVT_BKT_IPT(opvt_node));
    sal_memset(dstbnke, 0, sizeof(dstbnke));
    for (i = 0; i < pfx_arr->pfx_cnt; i++) {
        int dst_bnk, empty_bnk;
        int offset = ACB_BKT_FIXED_FMT(acb, 1) ? 0 : PVT_KEY_LEN(npvt_node);
        src_idx = pfx_arr->pfx[i]->ent_idx;
        rv = alpm_bkt_ent_get_attempt(u, acb,
                ACB_BKT_VRF_POOL(acb, vrf_id),
                dst_bkt, def_fmt, pfx_arr->pfx[i]->key_len - offset,
                &dst_idx, NULL, 0, &empty_bnk);
        /* We should not see a FULL here */
        ALPM_IER(rv);

        dst_bnk = ALPM_IDX_TO_BNK(acb, dst_idx);
        if (db_cnt[dst_bnk] == 0 && empty_bnk == FALSE) {
            rv = th_mem_read(u, acb, BPC_BKT_TBL(bp_conf),
                             ALPM_TAB_IDX_GET_BI_BNK(acb, dst_bkt, dst_bnk),
                             dstbnke[dst_bnk], FALSE);
            ALPM_IER(rv);
        }
        th_alpm_ent_copy(u, vrf_id, acb,
                         srcbnke[ALPM_IDX_TO_BNK(acb, src_idx)],
                         dstbnke[dst_bnk],
                         src_bkt, dst_bkt,
                         PVT_KEY_LEN(opvt_node),
                         PVT_KEY_LEN(npvt_node),
                         src_idx, dst_idx);

        db_cnt[dst_bnk]++;
        pfx_arr->pfx[i]->ent_idx = dst_idx;
        if (!ALPM_HIT_SKIP(u)) {
            _alpm_tbl_t tbl = alpmTblInvalid;
            ALPM_HIT_LOCK(u);
            tbl = ACB_BKT_TBL(acb, vrf_id);
            th_alpm_ent_hit_move(u, vrf_id, acb, tbl, src_idx, dst_idx);
            ALPM_HIT_UNLOCK(u);
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

    rv = th_mem_bulk_write(u, acb, npvt_node,
            BPC_BKT_TBL(bp_conf),
            bulk_idx, bulk_ent, bulk_ent_cnt);

    ALPM_IER(rv);

    /* TD3ALPMTBD: rollback@err */
    return rv;
}

int
th_alpm_bkt_pfx_clean(int u, _alpm_cb_t *acb,
                      _alpm_pvt_node_t *pvt_node,
                      int count, uint32 *ent_idx)
{
    int i, rv = BCM_E_NONE;
    int vrf_id;
    uint32 src_idx, ent_id;

    soc_format_t bnk_fmt;

    uint16 srcbnk[ALPM_BPB_MAX] = {0};
    uint32 srcbnke[ALPM_BPB_MAX][17];
    /* Bulk write */
    int     bulk_idx[ALPM_BPB_MAX];
    uint32  *bulk_ent[ALPM_BPB_MAX];
    int     bulk_ent_cnt = 0;

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
        rv = th_mem_read(u, acb, BPC_BKT_TBL(bp_conf),
                ALPM_TAB_IDX_GET_BI_BNK(acb, &PVT_BKT_INFO(pvt_node), i),
                srcbnke[i], FALSE);
        ALPM_IER(rv);
    }

    /* Clear corresponding entry from bank */
    for (i = 0; i < count; i++) {
        uint32 tmpe[17] = {0};
        int8 fmt_tp;

        src_idx = ent_idx[i];
        fmt_tp = th_alpm_bnk_fmt_get(u, acb, &PVT_BKT_INFO(pvt_node), src_idx);
        bnk_fmt = ACB_FMT_BNK(acb, vrf_id, fmt_tp);
        ent_id  = ALPM_IDX_TO_ENT(src_idx);
        (void)soc_format_field_set(u, bnk_fmt,
                    srcbnke[ALPM_IDX_TO_BNK(acb, src_idx)],
                    th_alpm_ent_fld[ent_id], tmpe);
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

    rv = th_mem_bulk_write(u, acb, pvt_node,
            BPC_BKT_TBL(bp_conf),
            bulk_idx, bulk_ent, bulk_ent_cnt);

    ALPM_IER(rv);

    for (i = 0; i < count; i++) {
        uint32 bnk_idx;

        src_idx = ent_idx[i];
        bnk_idx = ALPM_IDX_TO_BNK(acb, src_idx);
        ent_id  = ALPM_IDX_TO_ENT(src_idx);
        PVT_BKT_INFO(pvt_node).vet_bmp[bnk_idx] &= ~(1 << ent_id);
        if (!ALPM_HIT_SKIP(u)) {
            _alpm_tbl_t tbl = alpmTblInvalid;
            ALPM_HIT_LOCK(u);
            tbl = ACB_BKT_TBL(acb, vrf_id);
            th_alpm_bkt_hit_write(u, vrf_id, acb, tbl, src_idx, 0);
            ALPM_HIT_UNLOCK(u);
        }
    }

    return rv;
}

/* Only do defrag inside a bucket */
int
th_alpm_bkt_pfx_shrink(int u, _alpm_cb_t *acb,
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

    if (!ACB_BKT_WRA(acb, PVT_BKT_VRF(pvt_node)) ||
        PVT_BKT_IDX(pvt_node) & 1) {
        start = PVT_ROFS(pvt_node);
        close = PVT_ROFS(pvt_node) + bnkpb;
        step  = 1;
    } else {
        start = PVT_ROFS(pvt_node) + bnkpb - 1;
        close = PVT_ROFS(pvt_node) - 1;
        step  = -1;
    }

    for (tb = start; ABS(tb-close) > 0; tb += step) {
        tbnk = tb % bnkpb;
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
                rv = th_alpm_bkt_bnk_shrink(u, acb, pvt_node, fbnk, tbnk,
                                            pfx_arr);
                ALPM_IER(rv);
            } else if (bkt_info->bnk_fmt[tbnk] >= bkt_info->bnk_fmt[fbnk]) {
                /* Bank tb & fb are same type and both occupied */
                rv = th_alpm_bkt_bnk_shrink(u, acb, pvt_node, fbnk,
                                            tbnk, pfx_arr);
                ALPM_IER(rv);
            }
        }
    }

    return rv;
}

int
th_alpm_bkt_bnk_copy(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node,
                     _alpm_bkt_info_t *src_bkt, _alpm_bkt_info_t *dst_bkt,
                     int src_idx, int dst_idx)
{
    int rv = BCM_E_NONE;
    int vrf_id = -1;
    uint32 ent[ALPM_ENT_WORDS_MAX] = {0};

    _alpm_tbl_t tbl = alpmTblInvalid;

    if (pvt_node != NULL) {
        vrf_id = PVT_BKT_VRF(pvt_node);
        tbl = BPC_BKT_TBL(ACB_BKT_VRF_POOL(acb, vrf_id));
    }


    if (tbl != alpmTblInvalid) {
        ALPM_IER(th_mem_read(u, acb, tbl, src_idx, ent, FALSE));
        if (BI_SUB_BKT_IDX(src_bkt) != BI_SUB_BKT_IDX(dst_bkt)) {
            int eid, sbnk, dbnk, emax;
            sbnk = ALPM_IDX_TO_BNK(acb, src_idx);
            dbnk = ALPM_IDX_TO_BNK(acb, dst_idx);
            emax = ACB_FMT_ENT_MAX(acb, vrf_id, src_bkt->bnk_fmt[sbnk]);
            for (eid = 0; eid < emax; eid++) {
                if (!SHR_BITGET(&src_bkt->vet_bmp[sbnk], eid)) {
                    continue;
                }

                th_alpm_ent_copy(u, vrf_id, acb, ent, ent, src_bkt, dst_bkt, 0, 0,
                    ALPM_IDX_MAKE(acb, src_bkt, sbnk, eid),
                    ALPM_IDX_MAKE(acb, dst_bkt, dbnk, eid));
            }
        }
        ALPM_IER(th_mem_write(u, acb, pvt_node, tbl, dst_idx, ent, FALSE));

        if (!ALPM_HIT_SKIP(u)) {
            ALPM_HIT_LOCK(u);
            th_alpm_bnk_hit_move(u, vrf_id,
                                 acb, &PVT_BKT_INFO(pvt_node),
                                 tbl,
                                 src_idx, dst_idx);
            ALPM_HIT_UNLOCK(u);
        }
    }
    return rv;
}

int
th_alpm_bkt_bnk_clear(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node,
                        int tab_idx)
{
    int rv = BCM_E_NONE;
    _alpm_tbl_t tbl = alpmTblInvalid;

    if (pvt_node != NULL) {
        tbl = ACB_BKT_TBL(acb, PVT_BKT_VRF(pvt_node));
        ALPM_IER(th_mem_write(u, acb, pvt_node, tbl, tab_idx, NULL, FALSE));
        if (!ALPM_HIT_SKIP(u)) {
            th_alpm_bnk_hit_clear(u, PVT_BKT_VRF(pvt_node), acb,
                                  &PVT_BKT_INFO(pvt_node), tbl, tab_idx);
        }
    }

    return rv;
}

/* TCAM related functions */
int
th_tcam_table_sz(int u, int app, int pkm)
{
    int sz = 0;
    soc_mem_t mem;
    mem = alpm_tbl[ACB_TOP(u, app)->pvt_tbl[pkm]];

    if (mem != INVALIDm) {
        sz = soc_mem_index_count(u, mem);
        if (ALPM_ACL_EN(u)) {
            sz >>= 1;
        }

        if (SOC_URPF_STATUS_GET(u) && app == APP0) {
            sz >>= 1;
        }

        if (alpm_acl_app(app)) {
            sz >>= 1;
        }
    }
    return sz;
}

/* TCAM Hw table cleanup (UNAVAIL for fast delete_all) */
/*
int
th_tcam_hw_cleanup(int u)
{
    return BCM_E_UNAVAIL;
}
*/

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
th_tcam_entry_x_to_y(int u, int app, int pkm, void *src, void *dst,
                      int copy_hit, int x, int y)
{
    int                 i;
    uint32              val[SOC_MAX_MEM_FIELD_WORDS] = {0};
    soc_mem_t           mem;
    _alpm_cb_t    *acb = ACB_TOP(u, app);

#define _FLD_CNT    9
    soc_field_t fld[2][_FLD_CNT] =
        {
            {
                VALID0f,
                DATA_TYPE0f,
                ENTRY_VIEW0f,
                KEY0f,
                MASK0f,
                REPLACE_DATA0f,
                ALG_BKT_PTR0f,
                ALG_SUB_BKT_PTR0f,
                GLOBAL_HIGH0f
             },
            {
                VALID1f,
                DATA_TYPE1f,
                ENTRY_VIEW1f,
                KEY1f,
                MASK1f,
                REPLACE_DATA1f,
                ALG_BKT_PTR1f,
                ALG_SUB_BKT_PTR1f,
                GLOBAL_HIGH1f
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

/* db_type: flex, non-flex */
static int
th_tcam_entry_view_get(int u, _alpm_bkt_info_t *bkt_info)
{
    int i, entry_view;

    for (i = 0; i < ALPM_BPB_MAX; i++) {
        if (bkt_info->bnk_fmt[i] != 0) {
            break;
        }
    }
    assert(i < ALPM_BPB_MAX);

    switch (bkt_info->bnk_fmt[i]) {
        case ALPM_FMT_PKM_32B:
            entry_view = 4;
            break;
        case ALPM_FMT_PKM_FLEX_32B:
            entry_view = 3;
            break;
        case ALPM_FMT_PKM_64B:
            entry_view = 2;
            break;
        case ALPM_FMT_PKM_FLEX_64B:
            entry_view = 1;
            break;
        case ALPM_FMT_PKM_128:
        case ALPM_FMT_PKM_FLEX_128:
            entry_view = 0;
            break;
        default:
            entry_view = 4;
            break;
    }

    return entry_view;
}

int
td3_tcam_mc_entry_assemble(int unit, _bcm_defip_cfg_t *lpm_cfg, soc_mem_t mem, void *e)
{
#if defined (BCM_TRIDENT3_SUPPORT)
    int dw = 0;

    if (SOC_MEM_FIELD_VALID(unit, mem, VALID_LWRf)) {
        dw = 1;
    }

    if (lpm_cfg->defip_flags & BCM_L3_IP6) {
        /* IPMC v6 */
        if (dw) {
            bcm_ip6_t mask;
            /* 128b key mode and key type, vrf */
            _bcm_td3_lpm_pair128_ipv6mc_ent_init(unit, lpm_cfg, e);
            /* Create mask from prefix length. */
            bcm_ip6_mask_create(mask, lpm_cfg->defip_sub_len);
            /* 128b addr and mask */
            _bcm_defip_pair128_ip6_addr_set(unit, mem, e, lpm_cfg->defip_ip6_addr, 1);
            _bcm_defip_pair128_ip6_mask_set(unit, mem, e, mask, 1);
        } else {
            /* 64b key mode, key type, vrf */
            _bcm_td3_lpm_ipv6mc_ent_init(unit, lpm_cfg, e);
            /* 64b addr and mask */
            _bcm_td3_mem_ip6mc_defip_set(unit, e, lpm_cfg);
        }
    } else {
        /* IPMC v4 single-wide */
        bcm_ip_t ip4_mask = BCM_IP4_MASKLEN_TO_ADDR(lpm_cfg->defip_sub_len);
        soc_mem_field32_set(unit, mem, e, VALIDf, 3);
        soc_mem_field32_set(unit, mem, e, KEY_MODE0f, 1);
        soc_mem_field32_set(unit, mem, e, KEY_MODE1f, 1);
        soc_mem_field32_set(unit, mem, e, KEY_TYPE0f, ALPM_SEPARATE_IPV4MC_KEY_TYPE);
        soc_mem_field32_set(unit, mem, e, KEY_TYPE1f, ALPM_SEPARATE_IPV4MC_KEY_TYPE);
        soc_mem_field32_set(unit, mem, e, KEY_MODE_MASK0f, 0x3);
        soc_mem_field32_set(unit, mem, e, KEY_MODE_MASK1f, 0x3);
        soc_mem_field32_set(unit, mem, e, KEY_TYPE_MASK0f, 0xf);
        soc_mem_field32_set(unit, mem, e, KEY_TYPE_MASK1f, 0xf);
        soc_mem_field32_set(unit, mem, e, IP_ADDR0f, lpm_cfg->defip_ip_addr);
        soc_mem_field32_set(unit, mem, e, IP_ADDR_MASK0f, ip4_mask);
        if (lpm_cfg->defip_vrf >= 0) {
            soc_mem_field32_set(unit, mem, e, VRF_ID_0f, lpm_cfg->defip_vrf);
            if (SOC_IS_FIREBOLT6(unit)) {
                soc_mem_field32_set(unit, mem, e, VRF_ID_MASK0f, 0x1fff);
            } else {
                soc_mem_field32_set(unit, mem, e, VRF_ID_MASK0f, 0xfff);
            }
        }
    }
#endif
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
th_tcam_entry_from_cfg(int unit, int pkm, _bcm_defip_cfg_t *lpm_cfg, void *e, int x, uint32 write_flags)
{
    int             i, f, cnt = 1;
    int             ipt;
    uint32          key[4] = {0};
    uint32          msk[4] = {0};
    uint32          val;
    int app = alpm_app_get(unit, lpm_cfg->defip_inter_flags);

    soc_mem_t       mem;
    _alpm_cb_t      *acb = ACB_TOP(unit, app);
    _alpm_pvt_node_t *pvt_node;
    _alpm_bkt_node_t *bkt_node;

    /* Associated-Data */
    soc_field_t fld_hit[] = {HIT0f, HIT1f, HITf};
    soc_field_t fld_class_id[] = {CLASS_ID0f, CLASS_ID1f, CLASS_IDf};
    soc_field_t fld_dst_disc[] = {DST_DISCARD0f, DST_DISCARD1f, DST_DISCARDf};
    soc_field_t fld_pri[] = {PRI0f, PRI1f, PRIf};
    soc_field_t fld_dest[] = {DESTINATION0f, DESTINATION1f, DESTINATIONf};
    soc_field_t fld_hit_idx[] = {ALG_HIT_IDX0f, ALG_HIT_IDX1f, ALG_HIT_IDXf};
    soc_field_t fld_glb_hi[] = {GLOBAL_HIGH0f, GLOBAL_HIGH1f, GLOBAL_HIGHf};
    soc_field_t fld_glb_rt[] = {GLOBAL_ROUTE0f, GLOBAL_ROUTE1f, GLOBAL_ROUTEf};
    soc_field_t fld_df_ms[] = {DEFAULT_MISS0f, DEFAULT_MISS1f, DEFAULT_MISSf};
    soc_field_t fld_rpe[] = {RPE0f, RPE1f, RPEf};
    soc_field_t fld_dt[] = {DATA_TYPE0f, DATA_TYPE1f, DATA_TYPEf};
    soc_field_t fld_acl_class_id_valid[] =
        {ACL_CLASS_ID_VALID0f, ACL_CLASS_ID_VALID1f, ACL_CLASS_ID_VALIDf};
    soc_field_t fld_acl_class_id[] = {ACL_CLASS_ID0f, ACL_CLASS_ID1f, ACL_CLASS_IDf};

    /* Flex Counter */
    soc_field_t fld_fc_bci[] = {
        FLEX_CTR_BASE_COUNTER_IDX0f,
        FLEX_CTR_BASE_COUNTER_IDX1f,
        FLEX_CTR_BASE_COUNTER_IDXf
    };
    soc_field_t fld_fc_om[] = {
        FLEX_CTR_OFFSET_MODE0f,
        FLEX_CTR_OFFSET_MODE1f,
        FLEX_CTR_OFFSET_MODEf
    };
    soc_field_t fld_fc_pn[] = {
        FLEX_CTR_POOL_NUMBER0f,
        FLEX_CTR_POOL_NUMBER1f,
        FLEX_CTR_POOL_NUMBERf
    };

    /* IPMC */
    soc_field_t fld_mc_cpu[] = {
        IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU0f,
        IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU1f,
        IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPUf
    };
    soc_field_t fld_mc_drop[] = {
        IPMC_EXPECTED_L3_IIF_MISMATCH_DROP0f,
        IPMC_EXPECTED_L3_IIF_MISMATCH_DROP1f,
        IPMC_EXPECTED_L3_IIF_MISMATCH_DROPf
    };
    soc_field_t fld_mc_iif[] =
        {EXPECTED_L3_IIF0f, EXPECTED_L3_IIF1f, EXPECTED_L3_IIFf};

    /* VALID */
    soc_field_t fld_vld[] = {VALID0f, VALID1f, VALID_LWRf};

    /* ALPM_DATA */
    soc_field_t fld_bkt[] = {ALG_BKT_PTR0f, ALG_BKT_PTR1f, ALG_BKT_PTRf};
    soc_field_t fld_sbkt[] =
        {ALG_SUB_BKT_PTR0f, ALG_SUB_BKT_PTR1f, ALG_SUB_BKT_PTRf};

    soc_field_t fld_ent_vw[] = {ENTRY_VIEW0f, ENTRY_VIEW1f, ENTRY_VIEWf};

    /* Key & Mask */
    soc_field_t fld_key_ip[] =
        {IP_ADDR0f, IP_ADDR1f, IP_ADDR0_UPRf, IP_ADDR1_UPRf};
    soc_field_t fld_key_vf[] =
        {VRF_ID_0f, VRF_ID_1f, VRF_ID_0_UPRf, VRF_ID_1_UPRf};
    soc_field_t fld_key_kt[] =
        {KEY_TYPE0f, KEY_TYPE1f, KEY_TYPE0_UPRf, KEY_TYPE1_UPRf};
    soc_field_t fld_key_km[] =
        {KEY_MODE0f, KEY_MODE1f, KEY_MODE0_UPRf, KEY_MODE1_UPRf};
    soc_field_t fld_msk_ip[] =
        {IP_ADDR_MASK0f, IP_ADDR_MASK1f, IP_ADDR_MASK0_UPRf, IP_ADDR_MASK1_UPRf};
    soc_field_t fld_msk_vf[] =
        {VRF_ID_MASK0f, VRF_ID_MASK1f, VRF_ID_MASK0_UPRf, VRF_ID_MASK1_UPRf};
    soc_field_t fld_msk_kt[] =
        {KEY_TYPE_MASK0f, KEY_TYPE_MASK1f, KEY_TYPE_MASK0_UPRf, KEY_TYPE_MASK1_UPRf};
    soc_field_t fld_msk_km[] =
        {KEY_MODE_MASK0f, KEY_MODE_MASK1f, KEY_MODE_MASK0_UPRf, KEY_MODE_MASK1_UPRf};

    int _ent_view = 0;
    int ipmc = (lpm_cfg->defip_flags & BCM_L3_IPMC);

    /* Starting here */

    mem = alpm_tbl[acb->pvt_tbl[pkm]];
    if (SOC_MEM_FIELD_VALID(unit, mem, VALIDf)) {
        if (_tcam_pkm_uses_half_wide(unit, pkm, ipmc)) {
            f = !!x;
            fld_key_ip[!x] = fld_key_ip[!!x];
            fld_key_vf[!x] = fld_key_vf[!!x];
            fld_key_kt[!x] = fld_key_kt[!!x];
            fld_key_km[!x] = fld_key_km[!!x];
            fld_msk_ip[!x] = fld_msk_ip[!!x];
            fld_msk_vf[!x] = fld_msk_vf[!!x];
            fld_msk_kt[!x] = fld_msk_kt[!!x];
            fld_msk_km[!x] = fld_msk_km[!!x];
        } else {
            f = 0;
            cnt = 2;
        }
    } else if (SOC_MEM_FIELD_VALID(unit, mem, VALID_LWRf)) {
        fld_key_ip[0] = IP_ADDR0_LWRf;
        fld_key_ip[1] = IP_ADDR1_LWRf;
        fld_key_vf[0] = VRF_ID_0_LWRf;
        fld_key_vf[1] = VRF_ID_1_LWRf;
        fld_key_kt[0] = KEY_TYPE0_LWRf;
        fld_key_kt[1] = KEY_TYPE1_LWRf;
        fld_key_km[0] = KEY_MODE0_LWRf;
        fld_key_km[1] = KEY_MODE1_LWRf;
        fld_msk_ip[0] = IP_ADDR_MASK0_LWRf;
        fld_msk_ip[1] = IP_ADDR_MASK1_LWRf;
        fld_msk_vf[0] = VRF_ID_MASK0_LWRf;
        fld_msk_vf[1] = VRF_ID_MASK1_LWRf;
        fld_msk_kt[0] = KEY_TYPE_MASK0_LWRf;
        fld_msk_kt[1] = KEY_TYPE_MASK1_LWRf;
        fld_msk_km[0] = KEY_MODE_MASK0_LWRf;
        fld_msk_km[1] = KEY_MODE_MASK1_LWRf;
        f = 2;
    } else {
        f = 0;
    }

#define _MEM32_SET(field, value) \
    soc_mem_field32_set(unit, mem, e, (field), (value))

_conf:
    _MEM32_SET(fld_vld[f], 1);
    if (SOC_MEM_FIELD_VALID(unit, mem, VALID_LWRf)) {
        _MEM32_SET(VALID_LWRf, 3);
        _MEM32_SET(VALID_UPRf, 3);
    }
    /* HIT bits */
    _MEM32_SET(fld_hit[f], !!(lpm_cfg->defip_flags & BCM_L3_HIT));
    _MEM32_SET(fld_rpe[f], !!(lpm_cfg->defip_flags & BCM_L3_RPE));
    _MEM32_SET(fld_dst_disc[f], !!(lpm_cfg->defip_flags & BCM_L3_DST_DISCARD));
    _MEM32_SET(fld_pri[f], lpm_cfg->defip_prio);
    _MEM32_SET(fld_class_id[f], lpm_cfg->defip_lookup_class);
    _MEM32_SET(fld_fc_bci[f], lpm_cfg->defip_flex_ctr_base_id);
    _MEM32_SET(fld_fc_om[f], lpm_cfg->defip_flex_ctr_mode);
    _MEM32_SET(fld_fc_pn[f], lpm_cfg->defip_flex_ctr_pool);

    if (soc_feature(unit, soc_feature_lpm_prefilter) &&
        alpm_acl_app(acb->app)) {
        _MEM32_SET(fld_acl_class_id_valid[f], 1);
        _MEM32_SET(fld_acl_class_id[f], lpm_cfg->defip_acl_class_id);
    } else if (soc_feature(unit, soc_feature_generic_dest)) {
        soc_format_t dst_fmt = DESTINATION_FORMATfmt;
        uint32 dst_fmt_val = 0;
        if (lpm_cfg->defip_flags & BCM_L3_MULTIPATH) {
            soc_format_field32_set(unit, dst_fmt, &dst_fmt_val, DEST_TYPE1f, 1);
            soc_format_field32_set(unit, dst_fmt, &dst_fmt_val, ECMP_GROUPf,
                                   lpm_cfg->defip_ecmp_index);
        } else if (!(lpm_cfg->defip_flags & BCM_L3_IPMC)) {
            soc_format_field32_set(unit, dst_fmt, &dst_fmt_val, DEST_TYPE0f, 3);
            soc_format_field32_set(unit, dst_fmt, &dst_fmt_val, NEXT_HOP_INDEXf,
                                   lpm_cfg->defip_ecmp_index);
        }
        _MEM32_SET(fld_dest[f], dst_fmt_val);
    }

    _MEM32_SET(fld_glb_hi[f], lpm_cfg->defip_vrf == BCM_L3_VRF_OVERRIDE);
    _MEM32_SET(fld_glb_rt[f], lpm_cfg->defip_vrf == BCM_L3_VRF_OVERRIDE ||
                              lpm_cfg->defip_vrf == BCM_L3_VRF_GLOBAL);
    _MEM32_SET(fld_df_ms[f], ALPM_TCAM_ZONED(unit, app) &&
                             lpm_cfg->default_miss == 1);
    _MEM32_SET(fld_dt[f], 0);

    pvt_node = (_alpm_pvt_node_t *)lpm_cfg->pvt_node;
    if (pvt_node != NULL) {
        _ent_view = th_tcam_entry_view_get(unit, &PVT_BKT_INFO(pvt_node));
        _MEM32_SET(fld_ent_vw[f], _ent_view);

        _MEM32_SET(fld_bkt[f], PVT_BKT_IDX(pvt_node) << ACB_BKT_DW(acb));
        _MEM32_SET(fld_sbkt[f], PVT_SUB_BKT_IDX(pvt_node));

        bkt_node = PVT_BKT_DEF(pvt_node);
        if (bkt_node != NULL) {
            int ent, hw_idx = 0;
            hw_idx = bkt_node->ent_idx;
            ent = ALPM_IDX_TO_ENT(bkt_node->ent_idx);
            hw_idx = th_alpm_hit_tbl_idx_get(unit, acb, alpmTblBktCmodeL2, hw_idx, ent);
            _MEM32_SET(fld_hit_idx[f], hw_idx);
        }
    }

    if (soc_feature(unit, soc_feature_ipmc_defip) &&
        (lpm_cfg->defip_flags & BCM_L3_IPMC)) {
        _MEM32_SET(fld_dt[f], 2);
        if (lpm_cfg->defip_mc_group > 0 &&
            soc_feature(unit, soc_feature_generic_dest)) {
            soc_format_t dst_fmt = DESTINATION_FORMATfmt;
            uint32 dst_fmt_val = 0;
            soc_format_field32_set(unit, dst_fmt, &dst_fmt_val, DEST_TYPE1f, 3);
            soc_format_field32_set(unit, dst_fmt, &dst_fmt_val, IPMC_GROUPf,
                                   lpm_cfg->defip_mc_group);
            _MEM32_SET(fld_dest[f], dst_fmt_val);
        }

        if (lpm_cfg->defip_l3a_rp != BCM_IPMC_RP_ID_INVALID) {
            _MEM32_SET(fld_mc_iif[f],
                       _BCM_DEFIP_IPMC_RP_SET(lpm_cfg->defip_l3a_rp));
        } else if (lpm_cfg->defip_ipmc_flags & BCM_IPMC_POST_LOOKUP_RPF_CHECK &&
                   lpm_cfg->defip_expected_intf != 0) {
            _MEM32_SET(fld_mc_iif[f], lpm_cfg->defip_expected_intf);
            _MEM32_SET(fld_mc_drop[f],
                       !!(lpm_cfg->defip_ipmc_flags & BCM_IPMC_RPF_FAIL_DROP));
            _MEM32_SET(fld_mc_cpu[f],
                       !!(lpm_cfg->defip_ipmc_flags & BCM_IPMC_RPF_FAIL_TOCPU));
        }

    }

    if (--cnt > 0) {
        f = 1;
        goto _conf;
    }

    /* Program KEY & MASK
     *
     *           key[4] = {key[0], key[1], key[2], key[3]}
     * ipv4:     key[4] = {IP_ADDR0f, 0, 0, 0}
     * ipv6-64:  key[4] = {0, 0, IP_ADDR0f, IP_ADDR1f, }
     * ipv6-128: key[4] = {IP_ADDR0_LWRf, IP_ADDR1_LWRf, IP_ADDR0_UPRf, IP_ADDR1_UPRf}
     */
    ipt = ALPM_LPM_IPT(u, lpm_cfg);
    alpm_util_cfg_to_key(unit, ipt, lpm_cfg, key);
    alpm_util_cfg_to_msk(unit, ipt, lpm_cfg, msk);
    if (SOC_MEM_FIELD_VALID(unit, mem, VALID_LWRf)) {
        cnt = 4;
    } else if (pkm == ALPM_PKM_64B) {
        key[1] = key[3];
        key[0] = key[2];
        msk[1] = msk[3];
        msk[0] = msk[2];
        cnt = 2;
    } else {
        cnt = 1;
    }

    switch (pkm) {
        case ALPM_PKM_32B:
            val = 0;
            break;
        case ALPM_PKM_64B:
            val = 1;
            break;
        case ALPM_PKM_128:
            val = 3;
            break;
        default:
            val = 0;
            break;
    }

    if (soc_feature(unit, soc_feature_separate_key_for_ipmc_route) &&
        (lpm_cfg->defip_flags & BCM_L3_IPMC)) {
        return td3_tcam_mc_entry_assemble(unit, lpm_cfg, mem, e);
    }

    for (i = 0; i < cnt; i++) {
        /* KEY & mask */
        _MEM32_SET(fld_key_km[i], val);
        _MEM32_SET(fld_msk_km[i], 0x3);
        _MEM32_SET(fld_key_kt[i], 0);
        _MEM32_SET(fld_msk_kt[i], 0xf);
        _MEM32_SET(fld_key_ip[i], key[i]);
        _MEM32_SET(fld_msk_ip[i], msk[i]);
        if (lpm_cfg->defip_vrf >= 0) {
            _MEM32_SET(fld_key_vf[i], lpm_cfg->defip_vrf);
            if (SOC_IS_FIREBOLT6(unit)) {
                _MEM32_SET(fld_msk_vf[i], 0x1fff);
            } else {
                _MEM32_SET(fld_msk_vf[i], 0xfff);
            }
        }
    }
#undef _MEM32_SET

    return BCM_E_NONE;
}

int
th_tcam_entry_to_cfg(int unit, int pkm, void *ent, int x,
                      _bcm_defip_cfg_t *lpm_cfg)
{
    soc_mem_t       mem;
    int app = alpm_app_get(unit, lpm_cfg->defip_inter_flags);
    _alpm_cb_t      *acb = ACB_TOP(unit, app);

    x = !!x;
    mem = alpm_tbl[acb->pvt_tbl[pkm]];

    lpm_cfg->defip_flags &= BCM_L3_IP6 | BCM_L3_IPMC;
    if (TH_ENT_HIT_GET(unit)) {
        lpm_cfg->defip_flags |= BCM_L3_HIT;
    } else {
        lpm_cfg->defip_flags &= ~BCM_L3_HIT;
    }

    /* ASSOC_DATAf format */
    if (TH_ENT_RPE_GET(unit)) {
        lpm_cfg->defip_flags |= BCM_L3_RPE;
    } else {
        lpm_cfg->defip_flags &= ~BCM_L3_RPE;
    }

    if (soc_feature(unit, soc_feature_lpm_prefilter) &&
        alpm_acl_app(acb->app)) {
        lpm_cfg->defip_inter_flags |= BCM_L3_INTER_FLAGS_PRE_FILTER;
        lpm_cfg->defip_acl_class_id = TH_ENT_ACL_CLASS_ID_GET(unit);
    } else if (soc_feature(unit, soc_feature_generic_dest)) {
        int ecmp = 0, ipmc = 0;
        TH_ENT_DEST_GET(unit, ecmp, ipmc, lpm_cfg->defip_ecmp_index);
        if (ecmp) {
            lpm_cfg->defip_flags |= BCM_L3_MULTIPATH;
            lpm_cfg->defip_ecmp = 1;
        } else {
            lpm_cfg->defip_flags &= ~BCM_L3_MULTIPATH;
            lpm_cfg->defip_ecmp = 0;
            lpm_cfg->defip_ecmp_count = 0;
        }
        if (ipmc) {
            lpm_cfg->defip_flags |= BCM_L3_IPMC;
            lpm_cfg->defip_mc_group = lpm_cfg->defip_ecmp_index;
        } else {
            lpm_cfg->defip_flags &= ~BCM_L3_IPMC;
            lpm_cfg->defip_mc_group = 0;
        }
    }

    if (TH_ENT_DST_DISCARD_GET(unit)) {
        lpm_cfg->defip_flags |= BCM_L3_DST_DISCARD;
    } else {
        lpm_cfg->defip_flags &= ~BCM_L3_DST_DISCARD;
    }
    lpm_cfg->defip_prio = TH_ENT_PRI_GET(unit);
    lpm_cfg->defip_lookup_class = TH_ENT_CLASS_ID_GET(unit);
    lpm_cfg->defip_flex_ctr_base_id = TH_ENT_FC_BASE_ID_GET(unit);
    lpm_cfg->defip_flex_ctr_mode = TH_ENT_FC_MODE_GET(unit);
    lpm_cfg->defip_flex_ctr_pool = TH_ENT_FC_POOL_GET(unit);

    /* No need to parse the rest field because there are already in the
     * lpm_cfg, part of key */

    if (soc_feature(unit, soc_feature_ipmc_defip)) {
        int is_ipmc = 0;
        if (TH_ENT_DATA_TYPE_GET(unit) == 2) {
            is_ipmc = 1;
        }

        if (is_ipmc) {
            uint32 val;
            lpm_cfg->defip_flags |= BCM_L3_IPMC;
            if (TH_ENT_MC_DROP_GET(unit)) {
                lpm_cfg->defip_ipmc_flags |= BCM_IPMC_RPF_FAIL_DROP;
            }
            if (TH_ENT_MC_CPU_GET(unit)) {
                lpm_cfg->defip_ipmc_flags |= BCM_IPMC_RPF_FAIL_TOCPU;
            }
            val = TH_ENT_MC_IIF_GET(unit);
            if (_BCM_DEFIP_IPMC_RP_IS_SET(val)) {
                lpm_cfg->defip_l3a_rp = _BCM_DEFIP_IPMC_RP_GET(val);
            } else {
                lpm_cfg->defip_expected_intf = val;
                if (lpm_cfg->defip_expected_intf != 0) {
                    lpm_cfg->defip_ipmc_flags |= BCM_IPMC_POST_LOOKUP_RPF_CHECK;
                }
                lpm_cfg->defip_l3a_rp = BCM_IPMC_RP_ID_INVALID;
            }
        } else {
            lpm_cfg->defip_flags &= ~BCM_L3_IPMC;
        }
    }

    return BCM_E_NONE;
}

int
th_tcam_entry_valid(int u, int app, int pkm, void *ent, int x)
{
    _alpm_cb_t  *acb = ACB_TOP(u, app);
    soc_mem_t   mem = alpm_tbl[acb->pvt_tbl[pkm]];

    return TH_ENT_VALID_GET(u) & (1 << x) ? 1 : 0;
}

int
th_tcam_entry_valid_set(int u, int app, int pkm, void *ent, int x, int val)
{
    soc_mem_t mem;
    soc_field_t vld_fld;
    _alpm_cb_t *acb = ACB_TOP(u, app);

    mem = alpm_tbl[acb->pvt_tbl[pkm]];
    switch (pkm) {
        case ALPM_PKM_32B:
            vld_fld = x ? VALID1f : VALID0f;
            soc_mem_field32_set(u, mem, ent, vld_fld, val ? 1 : 0);
            break;
        case ALPM_PKM_64B:
            soc_mem_field32_set(u, mem, ent, VALIDf, val ? 3 : 0);
            break;
        case ALPM_PKM_128:
            soc_mem_field32_set(u, mem, ent, VALID_LWRf, val ? 3 : 0);
            soc_mem_field32_set(u, mem, ent, VALID_UPRf, val ? 3 : 0);
            break;
        default:
            break;
    }

    return BCM_E_NONE;
}

static int
th_tcam_entry_to_mask_key(int u, int app, int pkm, void *ent, int x,
                            uint32 *key, int is_mask)
{
    _alpm_cb_t *acb = ACB_TOP(u, app);
    soc_mem_t   mem = alpm_tbl[acb->pvt_tbl[pkm]];

    sal_memset(key, 0, 4 * sizeof(uint32));
    if (is_mask) {
        int key_type = TH_ENT_KEY_KT_GET(u);
        if (ALPM_IS_SEPARATE_IPV6MC_KEY_TYPE(u, key_type)) {
            TD3_IP6MC_MASK_IPADDR_GET(u, key);
        } else {
            TH_ENT_MASK_IPADDR_GET(u, key);
        }
    } else {
        int key_type = TH_ENT_KEY_KT_GET(u);
        if (ALPM_IS_SEPARATE_IPV6MC_KEY_TYPE(u, key_type)) {
            TD3_IP6MC_KEY_IPADDR_GET(u, key);
        } else {
            TH_ENT_KEY_IPADDR_GET(u, key);
        }
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
th_tcam_entry_to_key(int u, int app, int pkm, void *ent, int x, uint32 *key)
{
    return th_tcam_entry_to_mask_key(u, app, pkm, ent, x, key, FALSE);
}

int
th_tcam_entry_vrf_id_get(int u, int app, int pkm, void *ent, int x, int *vrf_id)
{
    _alpm_cb_t *acb = ACB_TOP(u, app);
    soc_mem_t mem   = alpm_tbl[acb->pvt_tbl[pkm]];

    int key_type = TH_ENT_KEY_KT_GET(u);
    if (ALPM_IS_SEPARATE_IPV6MC_KEY_TYPE(u, key_type)) {

        TD3_IP6MC_KEY_VRF_ID_GET(u, *vrf_id);
    } else {
        TH_ENT_KEY_VRF_ID_GET(u, *vrf_id);
    }

    return BCM_E_NONE;
}

int
th_tcam_entry_key_type_get(int u, int app, int pkm, void *ent, int x, int *key_type)
{
    _alpm_cb_t *acb = ACB_TOP(u, app);
    soc_mem_t mem   = alpm_tbl[acb->pvt_tbl[pkm]];

    *key_type = TH_ENT_KEY_KT_GET(u);

    return BCM_E_NONE;
}

int
th_tcam_entry_pfx_len_get(int u, int app, int pkm, void *ent, int x, int *pfx_len)
{
    int         i, rv = BCM_E_NONE;
    uint32      mask[4];

    *pfx_len = 0;

    /* Assuming each MASK field is correct,
     * Not my duty to validate MASK here */
    th_tcam_entry_to_mask_key(u, app, pkm, ent, x, mask, TRUE);
    for (i = 0; i < 4; i++) {
        *pfx_len += _shr_popcount(mask[i]);
    }

    return rv;
}

/*
 * Function:
 *      th_tcam_entry_mode_get
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
th_tcam_entry_mode_get(int u, int app, int pk, void *ent, int *step_size,
                        int *pkm, int *ipv6, int *key_mode, int x)
{
    int valid;
    int is_pair_tbl = 0;
    _alpm_cb_t *acb = ACB_TOP(u, app);
    soc_mem_t mem = alpm_tbl[acb->pvt_tbl[pk]];
    int key_type  = TH_ENT_KEY_KT_GET(u);

    if (mem == alpm_tbl[alpmTblPvtCmodeL1P128]) {
        is_pair_tbl = 1;
    }
    *step_size = is_pair_tbl ? 2 : 1;

    valid = TH_ENT_VALID_GET(u);
    valid &=  (1 << x);

    if (!valid) { /* invalid entry */
        return BCM_E_NOT_FOUND;
    }

    *key_mode = TH_ENT_KEY_KM_GET(u);

    if (ALPM_IS_SEPARATE_IPV4MC_KEY_TYPE(u, key_type) ||
       (*key_mode == 0)) {
        *ipv6 = 0;
    } else {
        *ipv6 = 1;
    }

    /* TD3TBD */
    *pkm = (mem == alpm_tbl[alpmTblPvtCmodeL1P128]) ? ALPM_PKM_128 :
                          *ipv6 ? ALPM_PKM_64B : ALPM_PKM_32B ;

    /* Wrong key_mode or key_type */
    if ((!is_pair_tbl && (*key_mode > 1)) ||
        (is_pair_tbl && (*key_mode != 3))) {
        return BCM_E_INTERNAL;
    }

   /* step_size 2 only if full-wide in un pair TCAM */
   if (!is_pair_tbl && (*key_mode == 1)) {
       *step_size = 2;
   }
   return BCM_E_NONE;
}

/* Get ASSOC_DATA from a TCAM entry */
int
th_tcam_entry_adata_get(int u, int app, int pkm, void *ent, int x,
                         _alpm_bkt_adata_t *adata)
{
    int ecmp = 0, ipmc = 0, rv = BCM_E_NONE;
    _alpm_cb_t *acb = ACB_TOP(u, app);
    soc_mem_t mem = alpm_tbl[acb->pvt_tbl[pkm]];

    if (acb->app == APP0) {
        TH_ENT_DEST_GET(u, ecmp, ipmc, adata->defip_ecmp_index);
        ipmc |= (TH_ENT_DATA_TYPE_GET(u) == 2);
        adata->defip_flags = ecmp ? BCM_L3_MULTIPATH : 0;
        adata->defip_flags |= ipmc ? BCM_L3_IPMC : 0;
    }
    adata->defip_flags |= (pkm != ALPM_PKM_32B) ? BCM_L3_IP6 : 0;
    adata->defip_flags |= TH_ENT_RPE_GET(u) ? BCM_L3_RPE : 0;
    adata->defip_flags |= TH_ENT_DST_DISCARD_GET(u) ? BCM_L3_DST_DISCARD : 0;
    adata->defip_prio = TH_ENT_PRI_GET(u);
    adata->defip_lookup_class = TH_ENT_CLASS_ID_GET(u);
    adata->defip_flex_ctr_base_id = TH_ENT_FC_BASE_ID_GET(u);
    adata->defip_flex_ctr_mode = TH_ENT_FC_MODE_GET(u);
    adata->defip_flex_ctr_pool = TH_ENT_FC_POOL_GET(u);
    if (soc_feature(u, soc_feature_lpm_prefilter) &&
        alpm_acl_app(acb->app)) {
        adata->defip_acl_class_id_valid = 1;
        adata->defip_acl_class_id = TH_ENT_ACL_CLASS_ID_GET(u);
    } else {
        adata->defip_acl_class_id_valid = 0;
        adata->defip_acl_class_id = 0;
    }

    return rv;
}

/* Get ALPM_DATA entry from a TCAM entry */
int
th_tcam_entry_bdata_get(int u, int app, int pkm, void *ent, int x, void *fent)
{
    /* return entry itself */
    sal_memcpy(fent, ent,
               sizeof(uint32) *
               soc_mem_entry_words(u, alpm_tbl[ACB_TOP(u, app)->pvt_tbl[pkm]]));

    return BCM_E_NONE;
}

/***************** Bucket entry (Bank) starts from here *****************/
int
th_alpm_ent_ent_get(int u, int vrf_id, _alpm_cb_t *acb, void *ent, uint32 fmt,
                     int eid, void *ftmp)
{
    int rv = BCM_E_NONE;

    /* get bucket level bank format entry */
    soc_format_field_get(u, ACB_FMT_BNK(acb, vrf_id, fmt), ent,
                         th_alpm_ent_fld[eid], ftmp);
    return rv;
}

/* Get ALPM_Data bucket info (logic table based):
   bkt_ptr, rofs, kshift, def_miss and bnkfmt[] */
static void
th_alpm_ent_binfo_get(int u, int vrf_id, int ipt, int pvt_pkm, _alpm_cb_t *acb,
                      void *ent, int x, _alpm_ent_info_t *info)
{
    int bnk;
    soc_mem_t mem = alpm_tbl[acb->pvt_tbl[pvt_pkm]];

    info->kshift = 0;
    BI_ROFS(&info->bkt_info) = 0;
    BI_BKT_IDX(&info->bkt_info) = TH_ENT_BKT_PTR_GET(u) >> ACB_BKT_DW(acb);
    BI_SUB_BKT_IDX(&info->bkt_info) = TH_ENT_SUB_BKT_PTR_GET(u);
    info->default_miss = TH_ENT_DEFAULT_MISS_GET(u);
    for (bnk = 0; bnk < ACB_BNK_PER_BKT(acb, vrf_id); bnk++) {
        info->bkt_info.bnk_fmt[bnk] =
            alpm_util_bkt_def_fmt_get(u, acb, vrf_id, ipt);
    }

    return;
}

/* Read and update ALPM prefix key */
int
th_alpm_bkt_pfx_get(int u, int vrf_id, _alpm_cb_t *acb, void *ent, int ent_fmt,
                    uint32 *valid, uint32 *new_key, int *new_len)
{
    int rv = BCM_E_NONE;
    soc_format_t fmt = ACB_FMT_ENT(acb, vrf_id, ent_fmt);

    /* process Bucket Level PREFIX */
    *valid = TH_BKT_VALID_GET(u);
    if (*valid) {
        TH_BKT_KEY_GET(u, new_key);
        if (ent_fmt == ALPM_FMT_PKM_64B || ent_fmt == ALPM_FMT_PKM_FLEX_64B) {
            new_key[3] = new_key[1];
            new_key[2] = new_key[0];
            new_key[1] = new_key[0] = 0;
        }
        *new_len = TH_BKT_LENGTH_GET(u);
    }

    return rv;
}

/* Read ALPM Assoc_Data */
static void
th_alpm_bkt_adata_get(int u, int vrf_id, int ipt, _alpm_cb_t *acb, void *ent,
                      int ent_fmt, _alpm_ent_info_t *info)
{
    int ecmp = 0, ipmc = 0;
    soc_format_t fmt = ACB_FMT_ENT(acb, vrf_id, ent_fmt);
     _alpm_bkt_adata_t *adata = &info->adata;

    if (acb->app == APP0) {
        TH_BKT_DEST_GET(u, ecmp, ipmc, adata->defip_ecmp_index);
        adata->defip_flags = ecmp ? BCM_L3_MULTIPATH : 0;
        adata->defip_flags |= ipmc ? BCM_L3_IPMC : 0;
    }
    adata->defip_flags |= ALPM_IS_IPV6(ipt) ? BCM_L3_IP6 : 0;
    adata->defip_flags |= TH_BKT_RPE_GET(u) ? BCM_L3_RPE : 0;
    adata->defip_flags |= TH_BKT_DST_DISCARD_GET(u) ? BCM_L3_DST_DISCARD : 0;
    adata->defip_prio = TH_BKT_PRI_GET(u);
    adata->defip_lookup_class = TH_BKT_CLASS_ID_GET(u);
    if (soc_format_field_valid(u, fmt, FLEX_CTR_BASE_COUNTER_IDXf)) {
        adata->defip_flex_ctr_base_id = TH_BKT_FC_BASE_ID_GET(u);
        adata->defip_flex_ctr_mode = TH_BKT_FC_MODE_GET(u);
        adata->defip_flex_ctr_pool = TH_BKT_FC_POOL_GET(u);
    }
    info->bkt_info.sub_bkt_idx = TH_BKT_SUB_BKT_PTR_GET(u);
    if (soc_feature(u, soc_feature_lpm_prefilter) &&
        alpm_acl_app(acb->app)) {
        adata->defip_acl_class_id_valid = 1;
        adata->defip_acl_class_id = TH_BKT_ACL_CLASS_ID_GET(u);
    } else {
        adata->defip_acl_class_id_valid = 0;
        adata->defip_acl_class_id = 0;
    }

    return ;
}

static void
th_alpm_ent_binfo_raw_get(int u, int vrf_id, _alpm_cb_t *acb, void *ent,
                          int fmt, void *binfo_raw)
{
    soc_format_t bnk_fmt = ACB_FMT_BNK(acb, vrf_id, fmt);
    soc_field_t ent_fld = th_alpm_ent_fld[0];
    uint32 ent_sz = BITS2BYTES(soc_format_field_length(u, bnk_fmt, ent_fld));
    sal_memcpy(binfo_raw, ent, ent_sz);
    return;
}

/* Used for retrieving alpm info from given entry,
 * Entry 'e' can be a TCAM entry or a entry entry, not bank entry */
int
th_alpm_ent_selective_get(int u, _alpm_cb_t *acb, void *ent, _alpm_ent_info_t *info)
{
    int rv = BCM_E_NONE;
    int ent_fmt, vrf_id, ipt, pvt_pkm;
    uint32 mask = info->action_mask;

    /* ent_fmt or sub_idx */
    ent_fmt = info->ent_fmt;
    vrf_id  = info->vrf_id;
    ipt     = info->ipt;
    pvt_pkm = info->pvt_pkm;

    if (mask & (ALPM_INFO_MASK_VALID | ALPM_INFO_MASK_KEYLEN)) {
        th_alpm_bkt_pfx_get(u, vrf_id, acb, ent, ent_fmt, &info->ent_valid,
                            info->key, &info->key_len);
    }

    if (mask & ALPM_INFO_MASK_ASSOC_DATA) {
        th_alpm_bkt_adata_get(u, vrf_id, ipt, acb, ent, ent_fmt, info);
    }

    if (mask & ALPM_INFO_MASK_ALPM_DATA) {
        /* Here ent is TCAM entry */
        th_alpm_ent_binfo_get(u, vrf_id, ipt, pvt_pkm, acb, ent, ent_fmt, info);
    }

    if (mask & ALPM_INFO_MASK_ALPM_DATA_RAW) {
        th_alpm_ent_binfo_raw_get(u, vrf_id, acb, ent, ent_fmt, info->alpm_data_raw);
    }

    if (mask & ALPM_INFO_MASK_FIXED_DATA) {
        /* For TCAM entry only */;
    }

    return rv;
}

/* Return ALPM bucket bank default (minimum) format according to DB_Type */
uint8
th_alpm_bkt_def_fmt_get(int u, _alpm_cb_t *acb, int vrf_id, int ipt)
{
    int bkt_pkm;

    static uint8 def_fmt[] = {
        ALPM_FMT_PKM_32B,
        ALPM_FMT_PKM_FLEX_32B,
        ALPM_FMT_PKM_64B,
        ALPM_FMT_PKM_FLEX_64B,
        ALPM_FMT_PKM_128,
        ALPM_FMT_PKM_FLEX_128
    };

    if (ALPM_IS_IPV6(ipt)) {
        if (ALPM_128B(u)) {
            bkt_pkm = ALPM_PKM_128;
        } else {
            bkt_pkm = ALPM_PKM_64B;
        }
    } else {
        bkt_pkm = ALPM_PKM_32B;
    }

    return def_fmt[bkt_pkm << 1 | ACB_VRF_DB_TYPE(u, acb, vrf_id, ipt)];
}

/* Return Reduced/Full format type for a given fmt */
int16
th_alpm_bkt_fmt_type_get(int u, int vrf_id, _alpm_cb_t *acb, uint8 fmt)
{
    return ACB_FMT_FULL_TYPE(acb, vrf_id, fmt);
}

/*
 * Function:
 *      th_alpm_table_dump
 * Purpose:
 *      Dump ALPM memory tables:
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
void
th_alpm_table_ent_dump(int u, int app, int vrf_id, soc_mem_t mem, void *entry,
                       int copyno, int index, int fmt, int ent_idx,
                       int flags, char *pfx)
{
    int     i, ent, ent_min, ent_max;
    int     f, entry_dw;
    int     first_print_flag = 0;
    int     vertical = 0;
    uint32  fent[ALPM_MEM_ENT_MAX];
    uint32  fval[ALPM_MEM_ENT_MAX];
    uint32  nfval[ALPM_MEM_ENT_MAX];
    char    ctmp[ALPM_MEM_ENT_MAX * 8 + 3];
    soc_field_info_t *fieldp;
    soc_format_info_t *fmtp;
    soc_format_t tfmt;
    int fmt_ent_max[] = {0, 6, 4, 2, 4, 3, 2};

    _alpm_cb_t *acb;

    if (ent_idx >= fmt_ent_max[fmt]) {
        return;
    }

    acb = ACB_TOP(u, app);
    tfmt = ACB_FMT_ENT(acb, vrf_id, fmt);

    if (ent_idx == -1) { /* all entries */
        ent_min = 0;
        ent_max = fmt_ent_max[fmt] - 1;
    } else { /* one specific entry */
        ent_min = ent_idx;
        ent_max = ent_idx;
    }

    sal_memset(nfval, 0, sizeof(nfval));

    for (ent = ent_min; ent <= ent_max; ent++) {
        soc_format_field_get(u, ACB_FMT_BNK(acb, vrf_id, fmt), entry,
                             th_alpm_ent_fld[ent], fent);
        fmtp = &SOC_FORMAT_INFO(u, tfmt);

        if (flags & ALPM_DUMP_TABLE_RAW) {
            cli_out("%s", pfx);
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
                _shr_format_long_integer(ctmp, fval, BITS2BYTES(fieldp->len));

                if (first_print_flag == 0) {
                    cli_out("%s%s", pfx, vertical ? "" : "<");
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
}

int
th_alpm_table_dump(int u, int app, int vrf_id, soc_mem_t mem, int copyno, int index,
                   int fmt, int ent_idx, int flags)
{
    int rv = BCM_E_NONE;
    uint32  entry[ALPM_MEM_ENT_MAX];
    char    mem_prefix[256] = {0};

    if ((fmt <= 0) || (fmt > 6)) {
        cli_out("invalid or disabled bank format %d\n", fmt);
        return BCM_E_INTERNAL;
    }

    if (flags & ALPM_DUMP_TABLE_NOCACHE) {
        rv = soc_mem_read_no_cache(u, 0, mem, 0, MEM_BLOCK_ANY, index, entry);
    } else {
        rv = soc_mem_read(u, mem, MEM_BLOCK_ANY, index, entry);
    }

    if (BCM_SUCCESS(rv)) {
        sal_sprintf(mem_prefix, "%s.%s[%d.%d]: ", SOC_MEM_UFNAME(u, mem),
                    SOC_BLOCK_NAME(u, copyno), index, ent_idx);
        th_alpm_table_ent_dump(u, app, vrf_id, mem, entry, copyno, index, fmt,
                               ent_idx, flags, mem_prefix);
    }

    return rv;
}

#define ALPM_PHY_BKT_CNT(u) (SOC_IS_HELIX5(u) ? 4096 : 8192)

/*
 * Function:
 *      th_alpm_cap_min
 * Purpose:
 *      Get the min capacity for ALPM routes from PKM
 * Parameters:
 *      u           - Device unit
 *      mem         - hardware memory
 */
int
th_alpm_cap_min(int u, soc_mem_t mem, int app)
{
#define UTIL_33(x) ((x) / 3)
#define UTIL_66(x) ((x) * 2 / 3)
#define UTIL_85(x) ((x) * 85 / 100)

    int min_ent = 0, tcam_ent = 0, tot_rte = 0, tot_bkt = 0;
    int pkm;
    int urpf = SOC_URPF_STATUS_GET(u);
    int tcam_alpm = 0;
    int pid, fmt, pid_count;
    _alpm_bkt_pool_conf_t *bp_conf;
    _alpm_cb_t *acb;

    acb = ACB_BTM(u, app);

    switch (ALPM_MODE(u)) {
        case BCM_ALPM_MODE_COMBINED:
        case BCM_ALPM_MODE_PARALLEL:
            break;
        case BCM_ALPM_MODE_TCAM_ALPM:
            tcam_alpm = 1;
            break;
        default:
            return 0;
    }

    switch (mem) {
    case L3_DEFIP_ALPM_IPV4m:
        fmt = 1;
        pkm = ALPM_PKM_32B;
        break;
    case L3_DEFIP_ALPM_IPV4_1m:
        fmt = 4;
        pkm = ALPM_PKM_32B;
        break;
    case L3_DEFIP_ALPM_IPV6_64m:
        fmt = 2;
        pkm = ALPM_PKM_64B;
        break;
    case L3_DEFIP_ALPM_IPV6_64_1m:
        fmt = 5;
        pkm = ALPM_PKM_64B;
        break;
    case L3_DEFIP_ALPM_IPV6_128m:
        fmt = 3;
        pkm = ALPM_PKM_128;
        break;
    default:
        fmt = 1;
        pkm = ALPM_PKM_32B;
        break;
    }

    pid_count = alpm_pid_count(app);
    /* include VRF private and global routes */
    for (pid = 0; pid < pid_count; pid++) {
        int avl_bnks;
        bp_conf = ACB_BKT_POOL(acb, pid);
        if (pid > 0 && bp_conf == ACB_BKT_POOL(acb, pid - 1)) {
            continue; /* Combined Mode uses PID=0 only */
        }

        /* Exclude reserved bucket 0 */
        avl_bnks = BPC_BNK_CNT(bp_conf) - BPC_BNK_PER_BKT(bp_conf);
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

        tot_rte += avl_bnks * BPC_FMT_ENT_MAX(bp_conf, fmt);
        tot_bkt += BPC_BNK_PER_BKT(bp_conf) ?
            (avl_bnks / BPC_BNK_PER_BKT(bp_conf)) : 0;
    }

    switch (mem) {
    case L3_DEFIP_ALPM_IPV4m:
    case L3_DEFIP_ALPM_IPV4_1m:
        tcam_ent = soc_mem_index_count(u, L3_DEFIPm) * 2;
        break;

    case L3_DEFIP_ALPM_IPV6_64m:
    case L3_DEFIP_ALPM_IPV6_64_1m:
        tcam_ent = soc_mem_index_count(u, L3_DEFIPm);
        break;

    case L3_DEFIP_ALPM_IPV6_128m:
        tcam_ent = soc_mem_index_count(u, L3_DEFIP_PAIR_128m);
        break;

    default:
        break;
    }

    if (ALPM_ACL_EN(u)) {
        tcam_ent /= 2;
    }

    if (tcam_alpm) {
        tcam_ent /= 2;
    }

    if (tcam_ent) {
        tcam_ent = tcam_ent >> urpf;

        if (tcam_ent <= tot_bkt) {
            min_ent = UTIL_33(tot_rte / tot_bkt) * tcam_ent;
        } else {
            min_ent = UTIL_33(tot_rte / tot_bkt) * tot_bkt;
        }
        if (tcam_alpm) {
            min_ent += tcam_ent;
        }
    } else {
        min_ent = 0;
    }

    return min_ent;

}

/*
 * Function:
 *      th_alpm_cap_max
 * Purpose:
 *      Get the max capacity for ALPM routes from PKM
 * Parameters:
 *      u           - Device unit
 *      mem         - hardware memory
 */
static int
th_alpm_cap_max(int u, soc_mem_t mem, int app)
{
    int pkm;
    int tcam_ent = 0, tot_rte = 0, tot_bkt = 0;
    int urpf = SOC_URPF_STATUS_GET(u);
    int tcam_alpm = 0;
    int pid, fmt, pid_count;
    _alpm_bkt_pool_conf_t *bp_conf;
    _alpm_cb_t *acb;

    acb = ACB_BTM(u, app);

    switch (ALPM_MODE(u)) {
        case BCM_ALPM_MODE_COMBINED:
        case BCM_ALPM_MODE_PARALLEL:
            break;
        case BCM_ALPM_MODE_TCAM_ALPM:
            tcam_alpm = 1;
            break;
        default:
            return 0;
    }

    switch (mem) {
    case L3_DEFIP_ALPM_IPV4m:
        pkm = ALPM_PKM_32B;
        fmt = 1;
        break;
    case L3_DEFIP_ALPM_IPV4_1m:
        pkm = ALPM_PKM_32B;
        fmt = 4;
        break;
    case L3_DEFIP_ALPM_IPV6_64m:
        pkm = ALPM_PKM_64B;
        fmt = 2;
        break;
    case L3_DEFIP_ALPM_IPV6_64_1m:
        pkm = ALPM_PKM_64B;
        fmt = 5;
        break;
    case L3_DEFIP_ALPM_IPV6_128m:
        pkm = ALPM_PKM_128;
        fmt = 3;
        break;
    default:
        pkm = ALPM_PKM_32B;
        fmt = 1;
        break;
    }

    pid_count = alpm_pid_count(app);
    /* include VRF private and global routes */
    for (pid = 0; pid < pid_count; pid++) {
        int avl_bnks;
        bp_conf = ACB_BKT_POOL(acb, pid);
        if (pid > 0 && bp_conf == ACB_BKT_POOL(acb, pid - 1)) {
            continue; /* Combined Mode uses PID=0 only */
        }

        /* Exclude reserved bucket 0 */
        avl_bnks = BPC_BNK_CNT(bp_conf) - BPC_BNK_PER_BKT(bp_conf);

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

        tot_rte += avl_bnks * BPC_FMT_ENT_MAX(bp_conf, fmt);
        tot_bkt += BPC_BNK_PER_BKT(bp_conf) ?
            (avl_bnks / BPC_BNK_PER_BKT(bp_conf)) : 0;
    }

    switch (mem) {
    case L3_DEFIP_ALPM_IPV4m:
    case L3_DEFIP_ALPM_IPV4_1m:
        tcam_ent = soc_mem_index_count(u, L3_DEFIPm) * 2;
        break;

    case L3_DEFIP_ALPM_IPV6_64m:
    case L3_DEFIP_ALPM_IPV6_64_1m:
        tcam_ent = soc_mem_index_count(u, L3_DEFIPm);
        break;

    case L3_DEFIP_ALPM_IPV6_128m:
        tcam_ent = soc_mem_index_count(u, L3_DEFIP_PAIR_128m);
        break;

    default:
        tot_rte = 0;
        break;
    }

    if (ALPM_ACL_EN(u)) {
        tcam_ent /= 2;
    }

    if (tcam_alpm) {
        tcam_ent /= 2;
    }

    if (tcam_ent) {
        tcam_ent = tcam_ent >> urpf;

        if (tcam_ent <= tot_bkt) {
            tot_rte = (tot_rte / tot_bkt) * tcam_ent;
        }
        if (tcam_alpm) {
            tot_rte += tcam_ent;
        }
    } else {
        tot_rte = 0;
    }

    return tot_rte;

}

/*
 * Function:
 *      th_alpm_cap_get
 * Purpose:
 *      Get the min or max capacity for ALPM routes from tables:
 * L3_DEFIP_ALPM_IPV4, L3_DEFIP_ALPM_IPV6_64, L3_DEFIP_ALPM_IPV6_128
 *
 * Parameters:
 *      u           - Device unit
 *      mem         - Legacy memory type
 *      max_cnt     - Maximum result returned.
 *      min_cnt     - Minimum result returned.
 */
int
th_alpm_cap_get(int u, soc_mem_t mem, int *max_cnt, int *min_cnt, int app)
{

    switch (mem) {
        case L3_DEFIP_ALPM_IPV4m:
        case L3_DEFIP_ALPM_IPV4_1m:
        case L3_DEFIP_ALPM_IPV6_64m:
        case L3_DEFIP_ALPM_IPV6_64_1m:
        case L3_DEFIP_ALPM_IPV6_128m:
            break;
        default:
            return BCM_E_UNAVAIL;
    }

    if (min_cnt != NULL) {
        *min_cnt = th_alpm_cap_min(u, mem, app);
    }

    if (max_cnt != NULL) {
        *max_cnt = th_alpm_cap_max(u, mem, app);
    }

    return BCM_E_NONE;
}


static int
th_alpm_hit_tbl_idx_get(int u, _alpm_cb_t *acb, _alpm_tbl_t tbl, int idx, int ent)
{
    int phy_idx;
    phy_idx = th_mem_phy_index_get(u, acb, tbl, idx);
#if defined(BCM_HELIX5_SUPPORT)
#define HX5_ALPM_4_BANK_MODE_BANK_BITS 2
    if (soc_feature(u, soc_feature_alpm_addr_xlate)) {
        int phy_bnk;
        int phy_bkt;
        phy_bnk = phy_idx >> HX5_ALPM_PHY_BKT_BITS;
        phy_bkt = phy_idx & ((1 << HX5_ALPM_PHY_BKT_BITS) - 1);
        if (soc_alpm_cmn_banks_get(u) == 6) {
            phy_idx = ent * HX5_ALPM_BKT_ENT_MAX + phy_bnk;
            phy_idx = (phy_idx << HX5_ALPM_PHY_BKT_BITS) | phy_bkt;
        } else {
            phy_idx = (ent << HX5_ALPM_4_BANK_MODE_BANK_BITS) | phy_bnk;
            phy_idx = (phy_idx << HX5_ALPM_PHY_BKT_BITS) | phy_bkt;
        }
    } else
#endif
    {
        phy_idx =(ent << (ACB_BNK_BITS(acb) + TD3_ALPM_BKT_BITS)) | phy_idx;
    }
    return phy_idx;
}
/*
 * Function:
 *      th_alpm_bkt_hit_get
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
th_alpm_bkt_hit_get(int u, int vrf_id, _alpm_cb_t *acb,
                    _alpm_tbl_t tbl, int idx, int ent)
{
    int rv;
    int hit_hit_offset;
    int hit_tab_idx;
    uint32 *tbl_ent; /* bucket hit cache table entry pointer */
    int i = ACB_IDX(acb);
    int pid = 0;
    soc_mem_t mem = ALPM_HTBL_MEM(u, i, pid);
    soc_field_t hit_fld[4] = {
        HIT_0f, HIT_1f, HIT_2f,  HIT_3f
    };

    ALPM_HIT_LOCK(u);

    hit_tab_idx = th_alpm_hit_tbl_idx_get(u ,acb, tbl, idx, ent);

    hit_hit_offset = hit_tab_idx % 4;
    hit_tab_idx /= 4;

    tbl_ent = soc_mem_table_idx_to_pointer(u, mem, uint32 *,
                                           ALPM_HTBL_CACHE(u, i, pid), hit_tab_idx);
    rv = soc_mem_field32_get(u, mem, tbl_ent, hit_fld[hit_hit_offset]);

    ALPM_HIT_UNLOCK(u);
    return rv;
}

/*
 * Function:
 *      th_alpm_bkt_hit_set
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
th_alpm_bkt_hit_set(int u, int vrf_id, _alpm_cb_t *acb, _alpm_tbl_t tbl,
                    int idx, int ent, int hit_val)
{
    int rv = BCM_E_NONE;
    int hit_bit_offset;
    int hit_tab_idx;
    uint32 *tbl_ent; /* bucket hit cache table entry pointer */
    int i = ACB_IDX(acb);
    int pid = 0;
    soc_mem_t mem = ALPM_HTBL_MEM(u, i, pid);
    soc_field_t hit_fld[4] = {
        HIT_0f, HIT_1f, HIT_2f,  HIT_3f
    };

    ALPM_HIT_LOCK(u);

    hit_tab_idx = th_alpm_hit_tbl_idx_get(u, acb, tbl, idx, ent);

    hit_bit_offset = hit_tab_idx % 4;
    hit_tab_idx /= 4;

    tbl_ent = soc_mem_table_idx_to_pointer(u, mem, uint32 *,
                                           ALPM_HTBL_CACHE(u, i, pid), hit_tab_idx);
    soc_mem_field32_set(u, mem, tbl_ent, hit_fld[hit_bit_offset], hit_val);

    ALPM_HIT_UNLOCK(u);
    return rv;
}

/*
 * Function:
 *      th_alpm_bkt_hit_write
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
th_alpm_bkt_hit_write(int u, int vrf_id, _alpm_cb_t *acb,
                      _alpm_tbl_t tbl, int ent_idx, int hit_val)
{
    int rv = BCM_E_NONE;
    int idx, ent;
    int hit_tab_idx;
    uint32 *dst_ent; /* bucket hit cache table entry pointer */
    int i = ACB_IDX(acb);
    int pid = 0;
    soc_mem_t mem = ALPM_HTBL_MEM(u, i, pid);
    soc_field_t hit_fld[4] = {
        HIT_0f, HIT_1f, HIT_2f,  HIT_3f
    };

    ALPM_HIT_LOCK(u);

    ent = ALPM_IDX_TO_ENT(ent_idx);
    hit_tab_idx = th_alpm_hit_tbl_idx_get(u, acb, tbl, ent_idx, ent);

    ent = hit_tab_idx % 4;
    idx = hit_tab_idx / 4;

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
th_alpm_ent_hit_move(int u, int vrf_id, _alpm_cb_t *acb, _alpm_tbl_t tbl,
                     int src_ent_idx, int dst_ent_idx)
{
    int rv = BCM_E_NONE;
    int i, hit_val, idx, ent;
    uint32 *src_ent, *dst_ent;
    int pid = 0;
    int hit_tab_idx;
    soc_mem_t mem;
    soc_field_t hit_fld[4] = {
        HIT_0f, HIT_1f, HIT_2f,  HIT_3f
    };

    ALPM_HIT_LOCK(u);

    i = ACB_IDX(acb);
    mem = ALPM_HTBL_MEM(u, i, pid);

    ent = ALPM_IDX_TO_ENT(src_ent_idx);
    hit_tab_idx = th_alpm_hit_tbl_idx_get(u, acb, tbl, src_ent_idx, ent);

    ent = hit_tab_idx % 4;
    idx = hit_tab_idx / 4;


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
    hit_tab_idx = th_alpm_hit_tbl_idx_get(u, acb, tbl, dst_ent_idx, ent);

    ent = hit_tab_idx % 4;
    idx = hit_tab_idx / 4;


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
th_alpm_bnk_hit_move(int u, int vrf_id, _alpm_cb_t *acb,
                     _alpm_bkt_info_t *bkt_info, _alpm_tbl_t tbl,
                     int src_idx, int dst_idx)
{
    int i;
    int ent_max;
    int src_ent_idx;
    int dst_ent_idx;
    int sbnk;
    int rv = BCM_E_NONE;
    sbnk = ALPM_IDX_TO_BNK(acb, src_idx);
    ent_max = ACB_FMT_ENT_MAX(acb, vrf_id, bkt_info->bnk_fmt[sbnk]);
    for (i = 0; i < ent_max; i++) {
        src_ent_idx = (i << ALPM_IDX_ENT_SHIFT) | src_idx;
        dst_ent_idx = (i << ALPM_IDX_ENT_SHIFT) | dst_idx;
        rv = th_alpm_ent_hit_move(u, vrf_id, acb, tbl, src_ent_idx, dst_ent_idx);
    }

    return rv;
}

/* Clear ALPM bucket bank hitbit cache at idx.
   Set properly move, move_mask and ent_moved flag.
 */
int
th_alpm_bnk_hit_clear(int u, int vrf_id, _alpm_cb_t *acb,
                      _alpm_bkt_info_t *bkt_info, _alpm_tbl_t tbl,
                      int idx)
{
    int i;
    int ent_max;
    int rv = BCM_E_NONE;
    int ent_idx;
    int bnk;
    bnk = ALPM_IDX_TO_BNK(acb, idx);
    ent_max = ACB_FMT_ENT_MAX(acb, vrf_id, bkt_info->bnk_fmt[bnk]);
    for (i = 0; i < ent_max; i++) {
        ent_idx = (i << ALPM_IDX_ENT_SHIFT) | idx;
        rv = th_alpm_bkt_hit_write(u, vrf_id, acb, tbl, ent_idx, 0);
    }

    return rv;
}

/* Update hitbit for entry move on level i by
 * hit_cache = (hit_cache & ~hit_move_mask) | (hit_move & hit_move_mask)
 */
int
th_alpm_hit_move_update(int u, int i, int pid)
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
th_alpm_hit_cache_sync(int u, int app, int i, int pid)
{
    soc_mem_t mem;
    int idx_max, offset = 0;
    int rv = BCM_E_NONE;

    mem = ALPM_HTBL_MEM(u, i, pid);
    idx_max = ALPM_HTBL_IDX_CNT(u, i, pid) - 1;

    rv = soc_mem_read_range(u, mem, MEM_BLOCK_ANY, offset, idx_max + offset,
                            ALPM_HTBL_CACHE(u, i, pid));

    return rv;
}

/* Write ALPM non-TCAM level (i:0/1 for level 2/3) hitbit cache to Hw */
int
th_alpm_hit_hw_write(int u, int app, int i, int pid)
{
    soc_mem_t mem;
    int idx_max, offset = 0;
    int rv = BCM_E_NONE;

    mem = ALPM_HTBL_MEM(u, i, pid);
    idx_max = ALPM_HTBL_IDX_CNT(u, i, pid) - 1;

    rv = soc_mem_write_range(u, mem, MEM_BLOCK_ALL, offset, idx_max + offset,
                             ALPM_HTBL_CACHE(u, i, pid));

    return rv;
}

/* Get hitbit status from a TCAM entry
   (it sync to Hw periodically) */
int
th_tcam_ent_hit_get(int u, int app, int pkm, void *e, int sub_idx)
{
    int         rv = BCM_E_NONE;
    soc_field_t hit_fld[2] = {HIT0f, HIT1f};
    _alpm_cb_t  *acb = ACB_TOP(u, app);
    soc_mem_t mem = alpm_tbl[acb->pvt_tbl[pkm]];

    if (SOC_MEM_FIELD_VALID(u, mem, HITf)) {
        hit_fld[0] = HITf;
        hit_fld[1] = HITf;
    }

    rv = soc_mem_field32_get(u, mem, e, hit_fld[sub_idx]);

    return rv;
}

/* Get TCAM cached hitbit status from pvt_node's tcam_idx
   (it sync to Hw periodically) */
int
th_tcam_cache_hit_get(int u, _alpm_pvt_node_t *pvt_node, int app, int pkm, int tcam_idx)
{
    int         rv = BCM_E_NONE;
    int         idx, sub_idx;
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];
    soc_field_t hit_fld[2] = {HIT0f, HIT1f};

    _alpm_cb_t *acb = ACB_TOP(u, app);
    soc_mem_t mem = alpm_tbl[acb->pvt_tbl[pkm]];

    if (SOC_MEM_FIELD_VALID(u, mem, HITf)) {
        hit_fld[0] = HITf;
        hit_fld[1] = HITf;
    }

    if (_tcam_pkm_uses_half_wide(u, pkm, PVT_IS_IPMC(pvt_node))) {
        /* tcam_idx = idx << 1 | sub_idx */
        sub_idx = tcam_idx & 1;
        idx = tcam_idx >> 1;
    } else {
        sub_idx = 0;
        idx = tcam_idx;
    }

    ALPM_IER(th_mem_read(u, acb, acb->pvt_tbl[pkm], idx, e, FALSE));
    rv = soc_mem_field32_get(u, mem, e, hit_fld[sub_idx]);
    return rv;
}

/* Sync TCAM hitbit cache tables with Hw */
int
th_tcam_hit_cache_sync(int u)
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
            tbl_sz = th_tcam_table_sz(u, app, pkm);
            if (tbl_sz) {
                rv = soc_mem_cache_set(u, mem, MEM_BLOCK_ANY, TRUE);
            }
        }
    }

    return rv;
}

void
th_alpm_hit_deinit(int u)
{
    int i, pid;
    _alpm_cb_t *acb;

    if (ALPM_HIT_SKIP(u)) {
        return;
    }

    /* Dist hitbit tables deinit */
    if (ALPMDH(u) != NULL) {
        for (i = 0; i < ACB_CNT(u); i++) {
            acb = ACB(u, i, APP0);
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

    return;
}

/* Initialize ALPM distributed hitbit */
int
th_alpm_hit_init(int u)
{
    int rv = BCM_E_NONE;
    int i, pid, index_cnt;
    int alloc_sz;
    soc_mem_t mem;
    _alpm_cb_t *acb;

    if (ALPM_HIT_SKIP(u)) {
        return rv;
    }

    /* Initialize bucket Levels hitbit table ctrl */
    for (i = 0; i < ACB_CNT(u); i++) {
        acb = ACB(u, i, APP0);
        mem = th_alpm_hit_tbl_mem[i];
        /* Current implementation is assuming that HIT table entry
           is not more than 32 bits (actaully we have as maximum 16 HITBITs).
           Otherwise code has to adjust (for multiple words per entry) */
        assert(soc_mem_entry_words(u, mem) == 1);

        if (!SOC_WARM_BOOT(u)) {
            th_alpm_hw_mem_clear(u, mem);
        }

        for (pid = 0; pid < ACB_PID_CNT(acb); pid++) {
            alloc_sz = sizeof(_alpm_hit_tbl_ctrl_t);
            ALPM_ALLOC_EG(ALPM_HTBL(u, i, pid), alloc_sz, "hit_tbl");

            /* logic table index counter (same for HIT table) */
            index_cnt = soc_mem_index_count(u, mem);
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

    return rv;

bad:
    th_alpm_hit_deinit(u);
    return rv;
}

/* ALPM hitbit cleanup (UNAVAIL for fast delete_all) */
/*
int
th_alpm_hit_cleanup(int u)
{
    return BCM_E_UNAVAIL;
}
*/

alpm_functions_t th_alpm_driver = {
    th_alpm_ctrl_init,
    th_alpm_ctrl_deinit,
    NULL, /* th_alpm_ctrl_cleanup, -> UNAVAIL */
    th_alpm_bkt_bnk_copy,
    th_alpm_bkt_bnk_clear,
    th_alpm_bkt_pfx_copy,
    th_alpm_bkt_pfx_clean,
    th_alpm_bkt_pfx_shrink,
    th_alpm_bkt_bnk_write,
    th_alpm_bkt_def_fmt_get,
    th_alpm_bkt_fmt_type_get,

    th_alpm_ent_ent_get,
    th_alpm_ent_pfx_len_get,
    th_alpm_ent_selective_get,

    /* TCAM */
    th_tcam_table_sz,
    NULL, /* th_tcam_hw_cleanup, -> UNAVAIL */
    th_tcam_entry_x_to_y,
    th_tcam_entry_from_cfg,
    /* th_tcam_entry_selective_set, */
    /* th_tcam_entry_selective_get, */
    th_tcam_entry_to_cfg,
    th_tcam_entry_valid,
    th_tcam_entry_valid_set,
    th_tcam_entry_vrf_id_get,
    th_tcam_entry_key_type_get,
    th_tcam_entry_pfx_len_get,
    th_tcam_entry_to_key,

    th_tcam_entry_mode_get,
    th_tcam_entry_adata_get,
    th_tcam_entry_bdata_get,

    /* TCAM/ALPM */
    th_mem_read,
    th_mem_write,

    /* HITBIT */
    th_alpm_hit_init,
    th_alpm_hit_deinit,
    NULL, /* th_alpm_hit_cleanup -> UNAVAIL */
    th_alpm_bkt_hit_get,
    th_alpm_bkt_hit_set,
    th_alpm_bkt_hit_write,
    th_alpm_hit_move_update,
    th_alpm_hit_cache_sync,
    th_alpm_hit_hw_write,
    th_tcam_ent_hit_get,
    th_tcam_cache_hit_get,
    th_tcam_hit_cache_sync,
    NULL, /* th_tcam_hit_write -> UNAVAIL */

    /* dump table */
    th_alpm_table_dump,
    th_alpm_cap_get,
    NULL, /* th_alpm_def_usage_get -> UNAVAIL */

    /* data op: save/restore */
    NULL,

    /* index mapping */
    th_mem_phy_index_get,
};

#endif /* BCM_TOMAHAWK_SUPPORT */
#else
typedef int bcm_esw_alpm2_alpm_drv_td3_not_empty; /* Make ISO compilers happy. */
#endif /* ALPM_ENABLE */
