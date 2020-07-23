/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Memory Test Kernel
 *
 * Streamlined module designed for inclusion in the SOC driver for
 * performing power-on memory tests.
 *
 * This module is also used by the main SOC diagnostics memory tests,
 * fronted by user interface code.
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>

#include <soc/cm.h>
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/drv.h>
#ifdef BCM_BRADLEY_SUPPORT
#include <soc/bradley.h>
#endif
#if defined(BCM_TRIUMPH2_SUPPORT)
#include <soc/triumph2.h>
#endif /* BCM_TRIUMPH2_SUPPORT */
#if defined(BCM_TRIUMPH3_SUPPORT)
#include <soc/triumph3.h>
#endif /* BCM_TRIUMPH3_SUPPORT */
#if defined(BCM_TRIDENT_SUPPORT)
#include <soc/trident.h>
#endif /* BCM_TRIDENT_SUPPORT */
#if defined(BCM_MONTEREY_SUPPORT)
#include <soc/monterey.h>
#endif /* BCM_MONTEREY_SUPPORT */
#if defined(BCM_APACHE_SUPPORT)
#include <soc/apache.h>
#endif /* BCM_APACHE_SUPPORT */
#if defined(BCM_TRIDENT2_SUPPORT)
#include <soc/trident2.h>
#endif /* BCM_TRIDENT_SUPPORT */
#if defined(BCM_ENDURO_SUPPORT)
#include <soc/enduro.h>
#endif /* BCM_ENDURO_SUPPORT */
#if defined(BCM_HURRICANE_SUPPORT)
#include <soc/hurricane.h>
#endif /* BCM_HURRICANE_SUPPORT */
#if defined(BCM_HURRICANE2_SUPPORT)
#include <soc/hurricane2.h>
#endif /* BCM_HURRICANE2_SUPPORT */
#if defined(BCM_HURRICANE3_SUPPORT)
#include <soc/hurricane3.h>
#endif /* BCM_HURRICANE3_SUPPORT */
#if defined(BCM_GREYHOUND_SUPPORT)
#include <soc/greyhound.h>
#endif /* BCM_GREYHOUND_SUPPORT */
#if defined(BCM_GREYHOUND2_SUPPORT)
#include <soc/greyhound2.h>
#endif /* BCM_GREYHOUND2_SUPPORT */
#if defined(BCM_FIRELIGHT_SUPPORT)
#include <soc/firelight.h>
#endif /* BCM_FIRELIGHT_SUPPORT */
#if defined(BCM_KATANA_SUPPORT)
#include <soc/katana.h>
#endif /* BCM_KATANA_SUPPORT */
#if defined(BCM_KATANA2_SUPPORT)
#include <soc/katana2.h>
#endif /* BCM_KATANA2_SUPPORT */
#if defined(BCM_SABER2_SUPPORT)
#include <soc/saber2.h>
#endif /* BCM_SABER2_SUPPORT */
#if defined(BCM_PETRA_SUPPORT)
#include <soc/dpp/drv.h>
#endif /* BCM_PETRA_SUPPORT */
#if defined(BCM_DFE_SUPPORT)
#include <soc/dfe/cmn/dfe_drv.h>
#endif /* BCM_DFE_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
#include <soc/tomahawk.h>
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TOMAHAWK3_SUPPORT
#include <soc/tomahawk3.h>
#endif /* BCM_TOMAHAWK3_SUPPORT */
#if defined(BCM_TRIDENT3_SUPPORT)
#include <soc/trident3.h>
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_TOMAHAWK2_SUPPORT
#include <soc/tomahawk2.h>
#endif /* BCM_TOMAHAWK2_SUPPORT */
#if defined(BCM_MAVERICK2_SUPPORT)
#include <soc/maverick2.h>
#endif /* BCM_MAVERICK2_SUPPORT */
#if defined(BCM_HELIX5_SUPPORT)
#include <soc/helix5.h>
#endif /* BCM_HELIX5_SUPPORT */
#if defined(BCM_FIREBOLT6_SUPPORT)
#include <soc/firebolt6.h>
#endif /* BCM_FIREBOLT6_SUPPORT */
#if defined(BCM_HURRICANE4_SUPPORT)
#include <soc/hurricane4.h>
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_DNX_SUPPORT
#include <bcm/types.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#endif


#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT) 
/*
 * Table memory test
 */

#define PAT_PLAIN    0    /* Keep writing same word */
#define PAT_XOR      1    /* XOR 0xffffffff between entries */
#define PAT_RANDOM   2    /* Add 0xdeadbeef between words */
#define PAT_INCR     3    /* Add 1 between words */

STATIC void
fillpat(uint32 *seed, uint32 *mask, uint32 *buf, int pat, int dw)
{
    int            i;

    switch (pat) {
    case PAT_PLAIN:
    default:
    for (i = 0; i < dw; i++) {
        buf[i] = (*seed) & mask[i];
    }
    break;
    case PAT_XOR:
    for (i = 0; i < dw; i++) {
        buf[i] = (*seed) & mask[i];
    }
    (*seed) ^= 0xffffffff;
    break;
    case PAT_RANDOM:
    for (i = 0; i < dw; i++) {
        buf[i] = (*seed) & mask[i];
        (*seed) += 0xdeadbeef;
    }
    break;
    case PAT_INCR:
    for (i = 0; i < dw; i++) {
        buf[i] = (*seed) & mask[i];
    }
    (*seed) += 1;
    break;
    }
}

/*
 * Function:     soc_mem_datamask_memtest
 * Purpose:      Patch a bit mask for a memory entry under test
 * Returns:      SOC_E_xxx
 */
void
soc_mem_datamask_memtest(int unit, soc_mem_t mem, uint32 *buf)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    uint16 dev_id;
    uint8  rev_id;
    uint64 zero64;
    COMPILER_64_ZERO(zero64);
#endif

#ifdef BCM_SCORPION_SUPPORT
    if (SOC_IS_SC_CQ(unit)) {
        switch (mem) {
        case FP_TCAM_Xm:
            soc_mem_field32_set(unit, FP_TCAM_Xm, buf, IPBMf, 0x1fff);
            soc_mem_field32_set(unit, FP_TCAM_Xm, buf, IPBM_MASKf, 0x1fff);
            break;
        case FP_TCAM_Ym:
            soc_mem_field32_set(unit, FP_TCAM_Ym, buf, IPBMf, 0x1fffe000);
            soc_mem_field32_set(unit, FP_TCAM_Ym, buf, IPBM_MASKf, 0x1fffe000);
            break;
        default:
            break;
        }
    }
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
    soc_cm_get_id(unit, &dev_id, &rev_id);
    if (dev_id == BCM56983_DEVICE_ID) {
        switch(mem) {
        case ALLOWED_PORT_BITMAP_PROFILEm:
            soc_mem_field32_set(unit, ALLOWED_PORT_BITMAP_PROFILEm, buf,
                                    ING_PORT_BITMAP_P2f, 0x0);
            soc_mem_field32_set(unit, ALLOWED_PORT_BITMAP_PROFILEm, buf,
                                    ING_PORT_BITMAP_P3f, 0x0);
            soc_mem_field32_set(unit, ALLOWED_PORT_BITMAP_PROFILEm, buf,
                                    ING_PORT_BITMAP_P4f, 0x0);
            soc_mem_field32_set(unit, ALLOWED_PORT_BITMAP_PROFILEm, buf,
                                    ING_PORT_BITMAP_P5f, 0x0);
            break;
        case ETRAP_LKUP_EN_ING_PORTm:
            soc_mem_field32_set(unit, ETRAP_LKUP_EN_ING_PORTm, buf,
                                    ING_PORT_BMP_EN_P2f, 0x0);
            soc_mem_field32_set(unit, ETRAP_LKUP_EN_ING_PORTm, buf,
                                    ING_PORT_BMP_EN_P3f, 0x0);
            soc_mem_field32_set(unit, ETRAP_LKUP_EN_ING_PORTm, buf,
                                    ING_PORT_BMP_EN_P4f, 0x0);
            soc_mem_field32_set(unit, ETRAP_LKUP_EN_ING_PORTm, buf,
                                    ING_PORT_BMP_EN_P5f, 0x0);
            break;
        case STG_TABm:
            soc_mem_field64_set(unit, STG_TABm, buf, SPLIT_DATA_P2f, zero64);
            soc_mem_field64_set(unit, STG_TABm, buf, SPLIT_DATA_P3f, zero64);
            soc_mem_field64_set(unit, STG_TABm, buf, SPLIT_DATA_P4f, zero64);
            soc_mem_field64_set(unit, STG_TABm, buf, SPLIT_DATA_P5f, zero64);
            soc_mem_field32_set(unit, STG_TABm, buf, PARITY_P2f, 0x0);
            soc_mem_field32_set(unit, STG_TABm, buf, ECC_P2f, 0x0);
            soc_mem_field32_set(unit, STG_TABm, buf, ECCP_P2f, 0x0);
            soc_mem_field32_set(unit, STG_TABm, buf, PARITY_P3f, 0x0);
            soc_mem_field32_set(unit, STG_TABm, buf, ECC_P3f, 0x0);
            soc_mem_field32_set(unit, STG_TABm, buf, ECCP_P3f, 0x0);
            soc_mem_field32_set(unit, STG_TABm, buf, PARITY_P4f, 0x0);
            soc_mem_field32_set(unit, STG_TABm, buf, ECC_P4f, 0x0);
            soc_mem_field32_set(unit, STG_TABm, buf, ECCP_P4f, 0x0);
            soc_mem_field32_set(unit, STG_TABm, buf, PARITY_P5f, 0x0);
            soc_mem_field32_set(unit, STG_TABm, buf, ECC_P5f, 0x0);
            soc_mem_field32_set(unit, STG_TABm, buf, ECCP_P5f, 0x0);
            break;
        case EGR_VLANm:
            soc_mem_field32_set(unit, EGR_VLANm, buf, UT_PORT_BITMAP_P2f, 0x0);
            soc_mem_field32_set(unit, EGR_VLANm, buf, PORT_BITMAP_P2f, 0x0);
            soc_mem_field32_set(unit, EGR_VLANm, buf, PARITY_P2f, 0x0);
            soc_mem_field32_set(unit, EGR_VLANm, buf, ECC_P2f, 0x0);
            soc_mem_field32_set(unit, EGR_VLANm, buf, ECCP_P2f, 0x0);
            soc_mem_field32_set(unit, EGR_VLANm, buf, UT_PORT_BITMAP_P3f, 0x0);
            soc_mem_field32_set(unit, EGR_VLANm, buf, PORT_BITMAP_P3f, 0x0);
            soc_mem_field32_set(unit, EGR_VLANm, buf, PARITY_P3f, 0x0);
            soc_mem_field32_set(unit, EGR_VLANm, buf, ECC_P3f, 0x0);
            soc_mem_field32_set(unit, EGR_VLANm, buf, ECCP_P3f, 0x0);
            soc_mem_field32_set(unit, EGR_VLANm, buf, UT_PORT_BITMAP_P4f, 0x0);
            soc_mem_field32_set(unit, EGR_VLANm, buf, PORT_BITMAP_P4f, 0x0);
            soc_mem_field32_set(unit, EGR_VLANm, buf, PARITY_P4f, 0x0);
            soc_mem_field32_set(unit, EGR_VLANm, buf, ECC_P4f, 0x0);
            soc_mem_field32_set(unit, EGR_VLANm, buf, ECCP_P4f, 0x0);
            soc_mem_field32_set(unit, EGR_VLANm, buf, UT_PORT_BITMAP_P5f, 0x0);
            soc_mem_field32_set(unit, EGR_VLANm, buf, PORT_BITMAP_P5f, 0x0);
            soc_mem_field32_set(unit, EGR_VLANm, buf, PARITY_P5f, 0x0);
            soc_mem_field32_set(unit, EGR_VLANm, buf, ECC_P5f, 0x0);
            soc_mem_field32_set(unit, EGR_VLANm, buf, ECCP_P5f, 0x0);
            break;
        case EGR_VLAN_STGm:
            soc_mem_field64_set(unit, EGR_VLAN_STGm, buf, SPLIT_DATA_P2f, zero64);
            soc_mem_field64_set(unit, EGR_VLAN_STGm, buf, SPLIT_DATA_P3f, zero64);
            soc_mem_field64_set(unit, EGR_VLAN_STGm, buf, SPLIT_DATA_P4f, zero64);
            soc_mem_field64_set(unit, EGR_VLAN_STGm, buf, SPLIT_DATA_P5f, zero64);
        default:
            break;
        }
    }
    if (SOC_IS_TOMAHAWK3(unit)) {
        switch(mem) {
        case CDMIB_MEMm:
            buf[0] = 0xffffffff;
            buf[1] = 0x000000ff;
            buf[2] = 0xffffffff;
            buf[3] = 0x000000ff;
            buf[4] = 0xffffffff;
            buf[5] = 0x000000ff;
            buf[6] = 0xffffffff;
            buf[7] = 0x000000ff;
            buf[8] = 0xffffffff;
            buf[9] = 0x000000ff;
            buf[10] = 0xffffffff;
            buf[11] = 0x000000ff;
            buf[12] = 0xffffffff;
            buf[13] = 0x000000ff;
            buf[14] = 0xffffffff;
            buf[15] = 0x000000ff;
            break;
        default:
            break;
        }
    }
#endif
#if defined(BCM_HURRICANE4_SUPPORT) || defined(BCM_FIREBOLT6_SUPPORT)
    if (SOC_IS_HURRICANE4(unit) || (SOC_IS_FIREBOLT6(unit))) {
        switch(mem) {
            case BSK_FTFP_TCAMm:
            case BSK_FTFP2_TCAMm:
            case BSK_FTFP_COMBO_TCAM_POLICYm:
            case BSK_FTFP2_COMBO_TCAM_POLICYm:
                soc_mem_field32_set(unit, mem, buf, KEY_MODEf, 0x0);
            break;
            case BSC_DT_SBUS_ACCESS_CONTROLm:
                soc_mem_field32_set(unit, mem, buf, DONEf, 0x0);
                soc_mem_field32_set(unit, mem, buf, ERRORf, 0x0);
                soc_mem_field32_set(unit, mem, buf, VALIDf, 0x0);
            break;
            case BSC_AG_AGE_TABLEm:
                 soc_mem_field32_set(unit, mem, buf, AGE_OUT_IMMEDIATELYf, 0x0);
            break;
        default:
            break;
        }
    }
#endif
}

/*
 * memtest_fill
 *
 *   parm:         Test parameters
 *   mem:       soc_mem_t of memory to test
 *   copyno:       which copy of memory to test
 *   seed:         test pattern start value
 *   pat:          test pattern function
 *
 *   Returns 0 on success, -1 on error.
 */

#define FOREACH_FRAG \
  for (frag = frag_start; \
       frag_start <= frag_end ? frag <= frag_end : frag >= frag_end;    \
       frag += frag_start <= frag_end ? 1 : -1)

#define FOREACH_INDEX \
  for (index = index_start; \
    index_start <= index_end ? index <= index_end : index >= index_end; \
    index += index_step)

STATIC int
memtest_fill(int unit, soc_mem_test_t *parm, unsigned array_index, int copyno,
                 uint32 *seed, int pat)
{
    uint32               buf[SOC_MAX_MEM_WORDS];
    uint32               mask[SOC_MAX_MEM_WORDS];
    uint32               tcammask[SOC_MAX_MEM_WORDS];
    uint32               eccmask[SOC_MAX_MEM_WORDS];
    uint32               forcemask[SOC_MAX_MEM_WORDS];
    uint32               forcedata[SOC_MAX_MEM_WORDS];
    uint32               accum_tcammask, accum_forcemask;
    soc_mem_t            mem = parm->mem;
    int                  index, dw, rv, i;
    int                  index_start = parm->index_start;
    int                  index_end = parm->index_end;
    int                  index_step;
    int                  frag;
    int                  frag_start;
    int                  frag_end;

    index_step = parm->index_step;



    dw = soc_mem_entry_words(unit, mem);
    soc_mem_datamask_get(unit, mem, mask);
    soc_mem_tcammask_get(unit, mem, tcammask);
    soc_mem_eccmask_get(unit, mem, eccmask);
    soc_mem_forcedata_get(unit, mem, forcemask, forcedata);
    accum_tcammask = 0;
    for (i = 0; i < dw; i++) {
        accum_tcammask |= tcammask[i];
    }
    accum_forcemask = 0;
    for (i = 0; i < dw; i++) {
        accum_forcemask |= forcemask[i];
    }
    if (!parm->ecc_as_data) {
        for (i = 0; i < dw; i++) {
            mask[i] &= ~eccmask[i];
        }
    }
    soc_mem_datamask_memtest(unit, mem, mask);

#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
    if (parm->array_index_start != 0 || parm->array_index_end != parm->array_index_start) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "  FILL %s[%u-%u].%s[%d-%d]\n"),
                     SOC_MEM_UFNAME(unit, mem),
                     parm->array_index_start, parm->array_index_end,
                     SOC_BLOCK_NAME(unit, copyno),
                     index_start, index_end));
    } else 
#endif /* BCM_ESW_SUPPORT || BCM_SAND_SUPPORT */
    {
        {
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "  FILL %s.%s[%d-%d]\n"),
                         SOC_MEM_UFNAME(unit, mem),
                         SOC_BLOCK_NAME(unit, copyno),
                         index_start, index_end));
#endif /* BCM_ESW_SUPPORT || BCM_SAND_SUPPORT */
        }
    }

    if (bsl_check(bslLayerSoc, bslSourceSocmem, bslSeverityNormal, unit)) {
    LOG_CLI((BSL_META_U(unit,
                        "   MASK")));
    for (i = 0; i < dw; i++) {
        LOG_CLI((BSL_META_U(unit,
                            " 0x%08x"), mask[i]));
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));
        if (accum_tcammask) {
            LOG_CLI((BSL_META_U(unit,
                                "   TCAM MASK")));
            for (i = 0; i < dw; i++) {
                LOG_CLI((BSL_META_U(unit,
                                    " 0x%08x"), tcammask[i]));
            }
            LOG_CLI((BSL_META_U(unit,
                                "\n")));
        }
    }

    if (parm->index_start < parm->index_end) {
        frag_start = 0;
        frag_end = parm->frag_count > 0 ?
                   (parm->frag_count - 1) : parm->frag_count;
    } else if (parm->index_start > parm->index_end) {
        frag_start = parm->frag_count > 0 ?
                     (parm->frag_count - 1) : parm->frag_count;
        frag_end = 0;
    } else {
        frag_start = frag_end = 0;
    }
    FOREACH_FRAG {
        index_start = parm->frag_index_start[frag];
        index_end = parm->frag_index_end[frag];

    FOREACH_INDEX {
    if (soc_mem_test_skip(unit, parm->mem, index)) {
        continue;
    }
    fillpat(seed, mask, buf, pat, dw);
        if (accum_tcammask) {
            /* data read back has dependency on mask */
            if ((SOC_BLOCK_TYPE(unit, copyno) == SOC_BLK_ESM) ||
                 (SOC_BLOCK_TYPE(unit, copyno) == SOC_BLK_ETU)) {
                for (i = 0; i < dw; i++) {
                    buf[i] &= ~tcammask[i];
                }
            } else if (soc_feature(unit, soc_feature_xy_tcam)) {
                for (i = 0; i < dw; i++) {
                    buf[i] |= tcammask[i];
                }
            }
        }
        if (accum_forcemask) {
            for (i = 0; i < dw; i++) {
                buf[i] &= ~forcemask[i];
                buf[i] |= forcedata[i];
            }
        }
    if ((rv = (*parm->write_cb)(parm, array_index, copyno, index, buf)) < 0) {
        return rv;
    }
    } /* FOREACH_INDEX */
    } /* FOREACH_FRAG */

    return SOC_E_NONE;
}

/*
 * memtest_verify
 *
 *   parm:         Test parameters
 *   mem:       soc_mem_t of memory to test
 *   copyno:       which copy of memory to test
 *   seed:         test pattern start value
 *   incr:         test pattern increment value
 *
 *   Returns 0 on success, -1 on error.
 */

STATIC int
memtest_verify(int unit, soc_mem_test_t *parm, unsigned array_index, int copyno,
               uint32 *seed, int pat)
{
    uint32        buf[SOC_MAX_MEM_WORDS];
    uint32        mask[SOC_MAX_MEM_WORDS];
    uint32        tcammask[SOC_MAX_MEM_WORDS];
    uint32        eccmask[SOC_MAX_MEM_WORDS];
    uint32        forcemask[SOC_MAX_MEM_WORDS];
    uint32        forcedata[SOC_MAX_MEM_WORDS];
    uint32        accum_tcammask, accum_forcemask;
    uint32        cmp[SOC_MAX_MEM_WORDS];
    soc_mem_t     mem = parm->mem;
    int           index, dw, rv, i;
    int           index_start = parm->index_start;
    int           index_end = parm->index_end;
    int           index_step;
    int                 frag;
    int                 frag_start;
    int                 frag_end;
    int           read_cnt;

    index_step = parm->index_step;
    for (i=0;i<SOC_MAX_MEM_WORDS;i++) {
        mask[i] = 0;
        tcammask[i] = 0;
        eccmask[i] = 0;
    }
    dw = soc_mem_entry_words(unit, mem);
#if defined(BCM_SAND_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
    if(SOC_IS_SAND(unit) || SOC_IS_GREYHOUND2(unit)) {
        soc_mem_datamask_rw_get(unit, mem, mask);
    } else 
#endif 
    {
        soc_mem_datamask_get(unit, mem, mask);
    }
    soc_mem_tcammask_get(unit, mem, tcammask);
    soc_mem_eccmask_get(unit, mem, eccmask);
    soc_mem_forcedata_get(unit, mem, forcemask, forcedata);
    accum_tcammask = 0;
    for (i = 0; i < dw; i++) {
        accum_tcammask |= tcammask[i];
    }
    accum_forcemask = 0;
    for (i = 0; i < dw; i++) {
        accum_forcemask |= forcemask[i];
    }
    if (!parm->ecc_as_data) {
        for (i = 0; i < dw; i++) {
            mask[i] &= ~eccmask[i];
        }
    }
    soc_mem_datamask_memtest(unit, mem, mask);

#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
    if (parm->array_index_start != 0 || parm->array_index_end != parm->array_index_start) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "  VERIFY %s[%u-%u].%s[%d-%d] Reading %d times\n"),
                     SOC_MEM_UFNAME(unit, mem),
                     parm->array_index_start, parm->array_index_end,
                     SOC_BLOCK_NAME(unit, copyno),
                     index_start, index_end,
                     parm->read_count));
    } else 
#endif /* BCM_ESW_SUPPORT || BCM_SAND_SUPPORT */
    {
        {
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "  VERIFY %s.%s[%d-%d] Reading %d times\n"),
                         SOC_MEM_UFNAME(unit, mem),
                         SOC_BLOCK_NAME(unit, copyno),
                         index_start, index_end,
                         parm->read_count));
#endif /* BCM_ESW_SUPPORT || BCM_SAND_SUPPORT */
        }
    }

    if (parm->index_start < parm->index_end) {
        frag_start = 0;
        frag_end = parm->frag_count > 0 ?
                   (parm->frag_count - 1) : parm->frag_count;
    } else if (parm->index_start > parm->index_end) {
        frag_start = parm->frag_count > 0 ?
                     (parm->frag_count - 1) : parm->frag_count;
        frag_end = 0;
    } else {
        frag_start = frag_end = 0;
    }
    FOREACH_FRAG {
        index_start = parm->frag_index_start[frag];
        index_end = parm->frag_index_end[frag];

    FOREACH_INDEX {
    if (soc_mem_test_skip(unit, parm->mem, index)) {
        continue;
    }
    fillpat(seed, mask, cmp, pat, dw);
        if (accum_tcammask) {
            /* data read back has dependency on mask */
            if ((SOC_BLOCK_TYPE(unit, copyno) == SOC_BLK_ESM) ||
               (SOC_BLOCK_TYPE(unit, copyno) == SOC_BLK_ETU)) {
                for (i = 0; i < dw; i++) {
                    cmp[i] &= ~tcammask[i];
                }
            } else if (soc_feature(unit, soc_feature_xy_tcam)) {
                for (i = 0; i < dw; i++) {
                    cmp[i] |= tcammask[i];
                }
            }
        }
        if (accum_forcemask) {
            for (i = 0; i < dw; i++) {
                cmp[i] &= ~forcemask[i];
                cmp[i] |= forcedata[i];
            }
        }

    for (read_cnt = 0; read_cnt < parm->read_count; read_cnt++) {
        if ((rv = (*parm->read_cb)(parm, array_index, copyno, index, buf)) < 0) {
        return rv;
        }

        for (i = 0; i < dw; i++) {
        if ((buf[i] ^ cmp[i]) & mask[i]) {
            break;
        }
        }

        if (i < dw) {
        parm->err_count++;
        if ((*parm->miscompare_cb)(parm, array_index, copyno, index, buf,
                   cmp, mask) == MT_MISCOMPARE_STOP) {
                    parm->error_count++;
                    if (!parm->continue_on_error &&
                               (parm->error_count >= parm->error_max)) {
                        return SOC_E_FAIL;
                    }
        }
        }
    }
    } /* FOREACH_INDEX */
    } /* FOREACH_FRAG */

    return SOC_E_NONE;
}

/*
 * memtest_test_by_entry_pattern
 *
 *   Test memories one entry at a time.
 */
STATIC int
memtest_test_by_entry_pattern(int unit, soc_mem_test_t *parm, uint32 seed0,
                          int pat, char *desc)
{
    int                 copyno;
    unsigned            array_index;
    uint32              buf[SOC_MAX_MEM_WORDS];
    uint32              mask[SOC_MAX_MEM_WORDS];
    uint32              tcammask[SOC_MAX_MEM_WORDS];
    uint32              eccmask[SOC_MAX_MEM_WORDS];
    uint32              forcemask[SOC_MAX_MEM_WORDS];
    uint32              forcedata[SOC_MAX_MEM_WORDS];
    uint32              cmp[SOC_MAX_MEM_WORDS];
    uint32              accum_tcammask, accum_forcemask;
    soc_mem_t           mem = parm->mem;
    int                 index, dw, i;
    int                 index_start = parm->index_start;
    int                 index_end = parm->index_end;
    int                 index_step = parm->index_step;
    int                 read_cnt;

    dw = soc_mem_entry_words(unit, mem);
    soc_mem_datamask_get(unit, mem, mask);
    soc_mem_tcammask_get(unit, mem, tcammask);
    soc_mem_eccmask_get(unit, mem, eccmask);
    soc_mem_forcedata_get(unit, mem, forcemask, forcedata);
    accum_tcammask = 0;
    for (i = 0; i < dw; i++) {
        accum_tcammask |= tcammask[i];
    }
    accum_forcemask = 0;
    for (i = 0; i < dw; i++) {
        accum_forcemask |= forcemask[i];
    }
    if (!parm->ecc_as_data) {
        for (i = 0; i < dw; i++) {
            mask[i] &= ~eccmask[i];
        }
    }
    soc_mem_datamask_memtest(unit, mem, mask);

    for (array_index = parm->array_index_start; array_index <= parm->array_index_end; ++array_index) {
        SOC_MEM_BLOCK_ITER(unit, mem, copyno) {
            if (parm->copyno != COPYNO_ALL && parm->copyno != copyno) {
                continue;
            }
            FOREACH_INDEX {
                if (soc_mem_test_skip(unit, parm->mem, index)) {
                    continue;
                }
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
                if (parm->array_index_start != 0 || parm->array_index_end != parm->array_index_start) {
                    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                                (BSL_META_U(unit,
                                            "  WRITE/READ %s[%u-%u].%s[%d]\n"),
                                 SOC_MEM_UFNAME(unit, mem),
                                 parm->array_index_start, parm->array_index_end,
                                 SOC_BLOCK_NAME(unit, copyno),
                                 index));
                } else 
#endif /* BCM_ESW_SUPPORT || BCM_SAND_SUPPORT */
                {
                    {
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
                        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                                    (BSL_META_U(unit,
                                                "  WRITE/READ %s.%s[%d]\n"),
                                     SOC_MEM_UFNAME(unit, mem),
                                     SOC_BLOCK_NAME(unit, copyno),
                                     index));
#endif /* BCM_ESW_SUPPORT || BCM_SAND_SUPPORT */
                    }
                }
                if (bsl_check(bslLayerSoc, bslSourceSocmem, bslSeverityNormal, unit)) {
                    LOG_CLI((BSL_META_U(unit,
                                        "   MASK")));
                    for (i = 0; i < dw; i++) {
                        LOG_CLI((BSL_META_U(unit,
                                            " 0x%08x"), mask[i]));
                    }
                    LOG_CLI((BSL_META_U(unit,
                                        "\n")));
                    if (accum_tcammask) {
                        LOG_CLI((BSL_META_U(unit,
                                            "   TCAM MASK")));
                        for (i = 0; i < dw; i++) {
                            LOG_CLI((BSL_META_U(unit,
                                                " 0x%08x"), tcammask[i]));
                        }
                    }
                    LOG_CLI((BSL_META_U(unit,
                                        "\n")));
                }
                /* First, write the data */
                fillpat(&seed0, mask, buf, pat, dw);
                if (accum_tcammask) {
                    /* data read back has dependency on mask */
                    if ((SOC_BLOCK_TYPE(unit, copyno) == SOC_BLK_ESM) ||
                       (SOC_BLOCK_TYPE(unit, copyno) == SOC_BLK_ETU)) {
                        for (i = 0; i < dw; i++) {
                            buf[i] &= ~tcammask[i];
                        }
                    } else if (soc_feature(unit, soc_feature_xy_tcam)) {
                        for (i = 0; i < dw; i++) {
                            buf[i] |= tcammask[i];
                        }
                    }
                }
                if (accum_forcemask) {
                    for (i = 0; i < dw; i++) {
                        buf[i] &= ~forcemask[i];
                        buf[i] |= forcedata[i];
                    }
                }
#ifdef BCM_TOMAHAWK3_SUPPORT
                if (parm->mem == CDMIB_MEMm && index == 12) {
                   buf[14] = 0x00000000;
                   buf[15] = 0x00000000;
                }
#endif
                for (i = 0; i < dw; i++) {
                    cmp[i] = buf[i];
                }
                if ((*parm->write_cb)(parm, array_index, copyno, index, buf) < 0) {
                    return -1;
                }
                /* Then, read the data */
                for (read_cnt = 0; read_cnt < parm->read_count; read_cnt++) {
                    if ((*parm->read_cb)(parm, array_index, copyno, index, buf) < 0) {
                        return -1;
                    }
                    for (i = 0; i < dw; i++) {
                        if ((buf[i] ^ cmp[i]) & mask[i]) {
                            break;
                        }
                    }
                    if (i < dw) {
                        parm->err_count++;
                        if ((*parm->miscompare_cb)(parm, array_index, copyno, index, buf,
                                                   cmp, mask) == MT_MISCOMPARE_STOP) {
                            parm->error_count++;
                            if (!parm->continue_on_error &&
                                       (parm->error_count >= parm->error_max)) {
                                return SOC_E_FAIL;
                            }
                        }
                    }
                }
            }
        }
    }

    return SOC_E_NONE;
}


/*
 * memtest_pattern
 *
 *   Calls memtest_fill and memtest_verify on copies of the table.
 */
STATIC int
memtest_pattern(int unit, soc_mem_test_t *parm, uint32 seed0,
        int pat, char *desc)
{
    int         copyno;
    uint32      seed;
    uint32      seed_save = 0;
    int         verify, rv, i;
    uint32      array_index;
    static char msg[80];

#ifdef BCM_TOMAHAWK3_SUPPORT
    int acc_type = SOC_MEM_ACC_TYPE(unit, parm->mem);
    uint16 dev_id;
    uint8 rev_id;

    if (SOC_IS_TOMAHAWK3(unit)) {
        soc_cm_get_id(unit, &dev_id, &rev_id);
        /* For 56983 Skip the invalid pipes from memory tests */
        if(dev_id == BCM56983_DEVICE_ID &&
           (acc_type == 2 || acc_type == 3 || acc_type == 4 || acc_type == 5)) {
            return SOC_E_NONE;
        }
        /* Since CDMIB_MEM is per CDPORT block and during the iteration of
           blocks , it would iterate from all 64 blocks and in each iteration
           it alternately write fives and as. We would verfiy only at the
           end of the iteration and there would be a mismatch as generally
           we verify after every write.
        */
        if (parm->mem == CDMIB_MEMm) {
            parm->test_by_entry = 1;
        }
    }
#endif
    if (parm->test_by_entry) {
        return memtest_test_by_entry_pattern(unit, parm, seed0, pat, desc);
    }

    /* Two passes: fill, then verify */

    for (verify = 0; verify < 2; verify++) {
        seed = seed0;

        for (array_index = parm->array_index_start; array_index <= parm->array_index_end; ++array_index) {

            SOC_MEM_BLOCK_ITER(unit, parm->mem, copyno) {
#ifdef BCM_DNX_SUPPORT
                if (SOC_IS_DNX(unit) && (SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_MIN(unit, parm->mem)) == SOC_BLK_CDMAC || SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_MIN(unit, parm->mem)) == SOC_BLK_CDPORT))
                {
                    bcm_pbmp_t valid_ports;
                    bcm_port_t valid_port;
                    int phy_port = 0;
                    int cd_block = 0;
                    if (dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF, 0, &valid_ports) < 0)   /* Get NIF logical port bitmap */
                    {
                        break;
                    }
                    BCM_PBMP_ITER(valid_ports, valid_port)
                    {
                        phy_port = SOC_INFO(unit).port_l2p_mapping[valid_port]; /* Get NIF port first phy */
                        cd_block = SOC_PORT_BLOCK(unit, phy_port);
                        if (copyno == cd_block)
                        {
                            parm->copyno = copyno;
                            break;
                        }
                    }
                    if (copyno != cd_block)
                    {
                        continue;
                    }
                }
#endif
                if (parm->copyno != COPYNO_ALL && parm->copyno != copyno) {
                    continue;
                }
                if (verify) {
                    seed_save = seed;
                    for (i = 0; i < parm->reverify_count + 1; i++) {
                        if (parm->reverify_delay > 0) {
                            sal_sleep(parm->reverify_delay);
                        }
                        if (parm->status_cb) {
                            sal_sprintf(msg, "Verifying %s", desc);
                            (*parm->status_cb)(parm, msg);
                        }
    
                        seed = seed_save;
                        if (memtest_verify(unit, parm, array_index, copyno, &seed, pat) != 0) {
                            if (!parm->continue_on_error &&
                                (parm->error_count >= parm->error_max)) {
                                return SOC_E_FAIL;
                            }
                        }
                    }
                } else {
                    if (parm->status_cb) {
                        sal_sprintf(msg, "Filling %s", desc);
                        (*parm->status_cb)(parm, msg);
                    }

                    if ((rv = memtest_fill(unit, parm, array_index, copyno,
                           &seed, pat)) < 0) {
                        return rv;
                    }
                }
            }
        }
    }

    return SOC_E_NONE;
}

int
soc_mem_test_skip(int unit, soc_mem_t mem, int index)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    uint16 dev_id;
    uint8  rev_id;
    /* FOR 56982 SKU, some of the PMs are not active in pipes.
     * The below code determines which PM does it belong to
     * based on the pipe_stage numbering of the IDB_CA/OBM
     * registers
     */
    soc_cm_get_id(unit, &dev_id, &rev_id);

    if (dev_id == BCM56982_DEVICE_ID) {
        /* Skipping the below mem as 400G is not support for
           56982 SKU
         */
        soc_mem_info_t *memp;
        if (mem == TX_LKUP_1588_MEM_400Gm) {
            return (TRUE);
        }

        memp = &(SOC_MEM_INFO(unit, mem));
        if (memp->pipe_stage) {
            int acc_type;
            soc_info_t *si;
            int pipe_stage = 0;
            int pm_num = 0, pm = 0;
            pipe_stage = memp->pipe_stage;
            acc_type = SOC_MEM_ACC_TYPE(unit, mem);
            si = &SOC_INFO(unit);
            if (pipe_stage > 14 && pipe_stage < 23) {
                if (acc_type >=0 && acc_type < 8) {
                    if (pipe_stage > 14 && pipe_stage < 19) {
                        pm_num = pipe_stage - 15;
                    }

                    if (pipe_stage > 18 && pipe_stage < 23) {
                        pm_num = pipe_stage - 19;
                    }

                    pm = pm_num + (acc_type * 4);
                }

                if (si->active_pm_map[pm] != 1) {
                    return (TRUE);
                }
            }
        }
    }
#endif
#ifdef BCM_ESW_SUPPORT
    if (mem == FP_GLOBAL_MASK_TCAMm || mem == FP_TCAMm ||
        mem == EFP_TCAMm || mem == VFP_TCAMm
        || (FP_GLOBAL_MASK_TCAM_Xm == mem)
        || (FP_GLOBAL_MASK_TCAM_Ym == mem)
        || (FP_GM_FIELDSm == mem)) {

#ifdef BCM_TRIDENT2PLUS_SUPPORT
        if (soc_feature(unit, soc_feature_field_stage_quarter_slice)
            && soc_feature(unit, soc_feature_field_quarter_slice_single_tcam)
            && soc_feature(unit, soc_feature_field_ingress_two_slice_types)) {
            if ((FP_TCAMm == mem) || (FP_GM_FIELDSm == mem)
                || (FP_GLOBAL_MASK_TCAMm == mem)
                || (FP_GLOBAL_MASK_TCAM_Xm == mem)
                || (FP_GLOBAL_MASK_TCAM_Ym == mem)) {
                if (index < (soc_mem_index_count(unit, mem) / 2)) {
                    /* Slices of 2K entries. */
                    if (((index / 1024) % 2) || ((index / 512) % 2)) {
                        return (TRUE);
                    }
                } else {
                    /* Slices of 1K entries. */
                    if (((index / 512) % 2) || ((index / 256) % 2)) {
                        return (TRUE);
                    }
                }
            }
        }
#endif /* !BCM_TRIDENT2PLUS_SUPPORT */

#ifdef BCM_APACHE_SUPPORT
    if (soc_feature(unit, soc_feature_field_stage_half_slice)
        && soc_feature(unit, soc_feature_field_half_slice_single_tcam)
        && soc_feature(unit, soc_feature_field_ingress_two_slice_types)) {
        if ((FP_TCAMm == mem) || (FP_GM_FIELDSm == mem)
            || (FP_GLOBAL_MASK_TCAMm == mem)
            || (FP_GLOBAL_MASK_TCAM_Xm == mem)
            || (FP_GLOBAL_MASK_TCAM_Ym == mem)) {
            if (index < (soc_mem_index_count(unit, mem) / 2)) {
                /* Slices of 1K entries. */
                if ((index / 512) % 2) {
                    return (TRUE);
                }
            } else {
                /* Slices of 512 entries. */
                if ((index / 256) % 2) {
                    return (TRUE);
                }
            }
        }
    }
#endif /* !BCM_APACHE_SUPPORT */
        if (soc_feature(unit, soc_feature_field_ingress_two_slice_types) &&
            soc_feature(unit, soc_feature_field_stage_ingress_512_half_slice)){
            if (mem == FP_GLOBAL_MASK_TCAMm ||  mem == FP_TCAMm) {
                if (index < (soc_mem_index_count(unit, mem) / 2)) {
                    if ((index / 256) % 2) {
                        return TRUE;
                    }
                }
            }
        }

        if (soc_feature(unit, soc_feature_field_ingress_two_slice_types) &&
            soc_feature(unit, soc_feature_field_slices8)) {
            if (mem == FP_GLOBAL_MASK_TCAMm || mem == FP_TCAMm) {
                if (index >=
                            (soc_mem_index_count(unit, mem) * 3 / 4)) {
                    return TRUE;
                }
            }
        }

        if (soc_feature(unit, soc_feature_field_stage_egress_256_half_slice)) {
            if (mem == EFP_TCAMm) {
                if ((index / 128) % 2) {
                    return TRUE;
                }
            }
        }

        if (soc_feature(unit, soc_feature_field_stage_egress_512_half_slice)) {
            if (mem == EFP_TCAMm) {
                if ((index / 256) % 2) {
                    return TRUE;
                }
            }
        }

        if (soc_feature(unit, soc_feature_field_stage_lookup_256_half_slice)) {
            if (mem == VFP_TCAMm) {
                if ((index / 64) % 2) {
                    return TRUE;
                }
            }
        }

        if (soc_feature(unit, soc_feature_field_stage_lookup_512_half_slice)) {
            if (mem == VFP_TCAMm) {
                if ((index /128) % 2) {
                    return TRUE;
                }
            }
        }
        if (soc_feature(unit, soc_feature_field_slice_size128)) {
            if ((mem == FP_GLOBAL_MASK_TCAMm) || (mem == FP_TCAMm)) {
                if ((index/64) % 2) {
                    return TRUE;
                }
            }
        }
    }

    switch ( mem ) {
        case L3_DEFIP_ALPM_HIT_ONLYm :
        case L3_DEFIP_ALPM_IPV4m :
        case L3_DEFIP_ALPM_IPV4_1m :
        case L3_DEFIP_ALPM_IPV6_64m :
        case L3_DEFIP_ALPM_IPV6_64_1m :
        case L3_DEFIP_ALPM_IPV6_128m :
        case L3_DEFIP_ALPM_RAWm :
        case L3_DEFIP_ALPM_ECCm :
            if (! (soc_mem_index_count(unit, L3_DEFIP_ALPM_RAWm) > 0)) {
                /* ALPM wasn't enabled */
                return TRUE;
            }
    }

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWK(unit) &&
        !soc_feature(unit, soc_feature_advanced_flex_counter)) {
        if ((mem >= ING_FLEX_CTR_COUNTER_TABLE_0m &&
             mem <= ING_FLEX_CTR_COUNTER_TABLE_9_PIPE3m) ||
            (mem >= EGR_FLEX_CTR_COUNTER_TABLE_0m &&
             mem <= EGR_FLEX_CTR_COUNTER_TABLE_3_Ym)) {
            return TRUE;
        }
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TOMAHAWK2_SUPPORT
    if (soc_feature(unit, soc_feature_field_stage_half_slice)) {
        if ((IFP_TCAMm == mem) ||
            (IFP_TCAM_PIPE0m == mem) ||(IFP_TCAM_PIPE1m == mem) ||
            (IFP_TCAM_PIPE2m == mem) || (IFP_TCAM_PIPE3m == mem)) {
            if ((index / 256) % 2) {
                return TRUE;
            }
        }
        if ((IFP_TCAM_WIDEm == mem) ||
            (IFP_TCAM_WIDE_PIPE0m == mem) ||(IFP_TCAM_WIDE_PIPE1m == mem) ||
            (IFP_TCAM_WIDE_PIPE2m == mem) || (IFP_TCAM_WIDE_PIPE3m == mem)) {
            if ((index / 128) % 2) {
                return TRUE;
            }
        }
    }
    if (soc_feature(unit, soc_feature_field_stage_quarter_slice)) {
        if ((IFP_TCAMm == mem) ||
            (IFP_TCAM_PIPE0m == mem) ||(IFP_TCAM_PIPE1m == mem) ||
            (IFP_TCAM_PIPE2m == mem) || (IFP_TCAM_PIPE3m == mem)) {
            if ((index / 128) % 4) {
                return TRUE;
            }
        }
        if ((IFP_TCAM_WIDEm == mem) ||
            (IFP_TCAM_WIDE_PIPE0m == mem) ||(IFP_TCAM_WIDE_PIPE1m == mem) ||
            (IFP_TCAM_WIDE_PIPE2m == mem) || (IFP_TCAM_WIDE_PIPE3m == mem)) {
            if ((index / 64) % 4) {
                return TRUE;
            }
        }
    }
#endif /* BCM_TOMAHAWK2_SUPPORT*/
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        if (CDMIB_MEMm == mem) {
            if (index > 12) {
                return TRUE;
            }
        }
    }
#endif
#endif /* BCM_ESW_SUPPORT */

    return FALSE;
}


/*
 * Memory test main entry point
 */

int
soc_mem_test(soc_mem_test_t *parm)
{
    int            rv = SOC_E_NONE;
    int            unit = parm->unit;

    parm->err_count = 0;
    
#ifdef BCM_HURRICANE4_SUPPORT
    if (_soc_hr4_mem_skip(unit, parm->mem)) {
        goto done;
    }
#endif /* BCM_HURRICANE4_SUPPORT */

#ifdef BCM_FIREBOLT6_SUPPORT
    if (_soc_fb6_mem_skip(unit, parm->mem)) {
        goto done;
    }
#endif /* BCM_FIREBOLT6_SUPPORT */

    if (parm->patterns & MT_PAT_ZEROES) {
    /* Detect bits stuck at 1 */
    if ((rv = memtest_pattern(unit, parm, 0x00000000,
                  PAT_PLAIN, "0x00000000")) < 0) {
        goto done;
    }
    }

    if (parm->patterns & MT_PAT_ONES) {
    /* Detect bits stuck at 0 */
    if ((rv = memtest_pattern(unit, parm, 0xffffffff,
                  PAT_PLAIN, "0xffffffff")) < 0) {
        goto done;
    }
    }

    if (parm->patterns & MT_PAT_FIVES) {
    if ((rv = memtest_pattern(unit, parm, 0x55555555,
                  PAT_PLAIN, "0x55555555")) < 0) {
        goto done;
    }
    }

    if (parm->patterns & MT_PAT_AS) {
    if ((rv = memtest_pattern(unit, parm, 0xaaaaaaaa,
                  PAT_PLAIN, "0xaaaaaaaa")) < 0) {
        goto done;
    }
    }

    if (parm->patterns & MT_PAT_CHECKER) {
    /*
     * Checkerboard alternates 5's and a's to generate a pattern shown
     * below.  This is useful to detecting horizontally or vertically
     * shorted data lines (in cases where the RAM layout is plain).
     *
     *     1 0 1 0 1 0 1 0 1 0 1
     *     0 1 0 1 0 1 0 1 0 1 0
     *     1 0 1 0 1 0 1 0 1 0 1
     *     0 1 0 1 0 1 0 1 0 1 0
     *     1 0 1 0 1 0 1 0 1 0 1
     */
    if ((rv = memtest_pattern(unit, parm, 0x55555555,
                  PAT_XOR, "checker-board")) < 0) {
        goto done;
    }
    }

    if (parm->patterns & MT_PAT_ICHECKER) {
    /* Same as checker-board but inverted */
    if ((rv = memtest_pattern(unit, parm, 0xaaaaaaaa,
                  PAT_XOR, "inverted checker-board")) < 0) {
        goto done;
    }
    }

    if (parm->patterns & MT_PAT_ADDR) {
    /*
     * Write a unique value in every location to make sure all
     * locations are distinct (detect shorted address bits).
     * The unique value used is the address so dumping the failed
     * memory may indicate where the bad data came from.
     */
    if ((rv = memtest_pattern(unit, parm, 0,
                  PAT_INCR, "linear increment")) < 0) {
        goto done;
    }
    }

    if (parm->patterns & MT_PAT_RANDOM) {
    /*
     * Write a unique value in every location using pseudo-random data.
     */
    if ((rv = memtest_pattern(unit, parm, 0xd246fe4b,
                  PAT_RANDOM, "pseudo-random")) < 0) {
        goto done;
    }
    }

    if (parm->patterns & MT_PAT_HEX) {
        /*
         * Write a value specified by the user.
         */
        int i, val = 0;
        for (i = 0; i < 4; i ++) {
            val |= (parm->hex_byte & 0xff) << i*8;
        }
        if ((rv = memtest_pattern(unit, parm, val,
                                  PAT_PLAIN, "hex_byte")) < 0) {
          goto done;
        }
    }

    if (parm->err_count > 0) {
    rv = SOC_E_FAIL;
    }

 done:

    return rv;
}

#ifdef BCM_FIREBOLT_SUPPORT
STATIC int
_soc_fb_mem_parity_control(int unit, soc_mem_t mem, int copyno, int enable)
{
    uint32 imask, oimask, misc_cfg, omisc_cfg;
    soc_field_t parity_f = PARITY_CHECK_ENf;

    COMPILER_REFERENCE(copyno);

    switch (mem) {
        case L2Xm:
        case L2_ENTRY_ONLYm:
        if (soc_feature(unit, soc_feature_l2x_parity)) {
            SOC_IF_ERROR_RETURN(READ_L2_ENTRY_PARITY_CONTROLr(unit, &imask));
            soc_reg_field_set(unit, L2_ENTRY_PARITY_CONTROLr, &imask,
                              PARITY_ENf, (enable ? 1 : 0));
            soc_reg_field_set(unit, L2_ENTRY_PARITY_CONTROLr, &imask,
                              PARITY_IRQ_ENf, (enable ? 1 : 0));
            SOC_IF_ERROR_RETURN(WRITE_L2_ENTRY_PARITY_CONTROLr(unit, imask));
        }
        return SOC_E_NONE;

        case L3_ENTRY_IPV4_MULTICASTm:
        case L3_ENTRY_IPV4_UNICASTm:
        case L3_ENTRY_IPV6_MULTICASTm:
        case L3_ENTRY_IPV6_UNICASTm:
        case L3_ENTRY_ONLYm:
        case L3_ENTRY_VALID_ONLYm:
        if (soc_feature(unit, soc_feature_l3x_parity)) {
            SOC_IF_ERROR_RETURN(READ_L3_ENTRY_PARITY_CONTROLr(unit, &imask));
            soc_reg_field_set(unit, L3_ENTRY_PARITY_CONTROLr, &imask,
                              PARITY_ENf, (enable ? 1 : 0));
            soc_reg_field_set(unit, L3_ENTRY_PARITY_CONTROLr, &imask,
                              PARITY_IRQ_ENf, (enable ? 1 : 0));
            SOC_IF_ERROR_RETURN(WRITE_L3_ENTRY_PARITY_CONTROLr(unit, imask));
        }
        return SOC_E_NONE;

        case L3_DEFIPm:
        case L3_DEFIP_DATA_ONLYm:
        if (soc_feature(unit, soc_feature_l3defip_parity)) {
            SOC_IF_ERROR_RETURN(READ_L3_DEFIP_PARITY_CONTROLr(unit, &imask));
            soc_reg_field_set(unit, L3_DEFIP_PARITY_CONTROLr, &imask,
                              PARITY_ENf, (enable ? 1 : 0));
            soc_reg_field_set(unit, L3_DEFIP_PARITY_CONTROLr, &imask,
                              PARITY_IRQ_ENf, (enable ? 1 : 0));
            SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_PARITY_CONTROLr(unit, imask));
        }
        return SOC_E_NONE;

#ifdef BCM_RAVEN_SUPPORT
        case DSCP_TABLEm:
        if (soc_feature(unit, soc_feature_ip_ep_mem_parity)) {
            SOC_IF_ERROR_RETURN(READ_DSCP_TABLE_PARITY_CONTROLr(unit, &imask));
            soc_reg_field_set(unit, DSCP_TABLE_PARITY_CONTROLr, &imask,
                              PARITY_ENf, (enable ? 1 : 0));
            soc_reg_field_set(unit, DSCP_TABLE_PARITY_CONTROLr, &imask,
                              PARITY_IRQ_ENf, (enable ? 1 : 0));
            SOC_IF_ERROR_RETURN(WRITE_DSCP_TABLE_PARITY_CONTROLr(unit, imask));
        }
        return SOC_E_NONE;
     
        case EGR_L3_INTFm:
        if (!SOC_IS_HAWKEYE(unit)) {
            if (soc_feature(unit, soc_feature_ip_ep_mem_parity)) {
                SOC_IF_ERROR_RETURN(READ_EGR_L3_INTF_PARITY_CONTROLr(unit, &imask));
                soc_reg_field_set(unit, EGR_L3_INTF_PARITY_CONTROLr, &imask,
                                  PARITY_ENf, (enable ? 1 : 0));
                soc_reg_field_set(unit, EGR_L3_INTF_PARITY_CONTROLr, &imask,
                                  PARITY_IRQ_ENf, (enable ? 1 : 0));
                SOC_IF_ERROR_RETURN(WRITE_EGR_L3_INTF_PARITY_CONTROLr(unit, imask));
            }
        }
        return SOC_E_NONE;

        case EGR_MASKm:
        if (!SOC_IS_HAWKEYE(unit)) {
            if (soc_feature(unit, soc_feature_ip_ep_mem_parity)) {
                SOC_IF_ERROR_RETURN(READ_EGR_MASK_PARITY_CONTROLr(unit, &imask));
                soc_reg_field_set(unit, EGR_MASK_PARITY_CONTROLr, &imask,
                                  PARITY_ENf, (enable ? 1 : 0));
                soc_reg_field_set(unit, EGR_MASK_PARITY_CONTROLr, &imask,
                                  PARITY_IRQ_ENf, (enable ? 1 : 0));
                SOC_IF_ERROR_RETURN(WRITE_EGR_MASK_PARITY_CONTROLr(unit, imask));
            }
        }
        return SOC_E_NONE;

        case EGR_L3_NEXT_HOPm:
        if (!SOC_IS_HAWKEYE(unit)) {
            if (soc_feature(unit, soc_feature_ip_ep_mem_parity)) {
                SOC_IF_ERROR_RETURN(READ_EGR_NEXT_HOP_PARITY_CONTROLr(unit, &imask));
                soc_reg_field_set(unit, EGR_NEXT_HOP_PARITY_CONTROLr, &imask,
                                  PARITY_ENf, (enable ? 1 : 0));
                soc_reg_field_set(unit, EGR_NEXT_HOP_PARITY_CONTROLr, &imask,
                                  PARITY_IRQ_ENf, (enable ? 1 : 0));
                SOC_IF_ERROR_RETURN(WRITE_EGR_NEXT_HOP_PARITY_CONTROLr(unit, imask));
            }
        }
        return SOC_E_NONE;

        case EGR_VLANm:
        if (soc_feature(unit, soc_feature_ip_ep_mem_parity)) {
            SOC_IF_ERROR_RETURN(READ_EGR_VLAN_TABLE_PARITY_CONTROLr(unit, &imask));
            soc_reg_field_set(unit, EGR_VLAN_TABLE_PARITY_CONTROLr, &imask,
                              PARITY_ENf, (enable ? 1 : 0));
            soc_reg_field_set(unit, EGR_VLAN_TABLE_PARITY_CONTROLr, &imask,
                              PARITY_IRQ_ENf, (enable ? 1 : 0));
            SOC_IF_ERROR_RETURN(WRITE_EGR_VLAN_TABLE_PARITY_CONTROLr(unit, imask));
        }
        return SOC_E_NONE;

        case FP_POLICY_TABLEm:
        if (soc_feature(unit, soc_feature_ip_ep_mem_parity)) {
            SOC_IF_ERROR_RETURN(READ_FP_POLICY_PARITY_CONTROLr(unit, &imask));
            soc_reg_field_set(unit, FP_POLICY_PARITY_CONTROLr, &imask,
                              PARITY_ENf, (enable ? 1 : 0));
            soc_reg_field_set(unit, FP_POLICY_PARITY_CONTROLr, &imask,
                              PARITY_IRQ_ENf, (enable ? 1 : 0));
            SOC_IF_ERROR_RETURN(WRITE_FP_POLICY_PARITY_CONTROLr(unit, imask));
        }
        return SOC_E_NONE;

        case ING_L3_NEXT_HOPm:
        if (!SOC_IS_HAWKEYE(unit)) {
            if (soc_feature(unit, soc_feature_ip_ep_mem_parity)) {
                SOC_IF_ERROR_RETURN(READ_ING_L3_NEXT_HOP_PARITY_CONTROLr(unit, &imask));
                soc_reg_field_set(unit, ING_L3_NEXT_HOP_PARITY_CONTROLr, &imask,
                                  PARITY_ENf, (enable ? 1 : 0));
                soc_reg_field_set(unit, ING_L3_NEXT_HOP_PARITY_CONTROLr, &imask,
                                  PARITY_IRQ_ENf, (enable ? 1 : 0));
                SOC_IF_ERROR_RETURN(WRITE_ING_L3_NEXT_HOP_PARITY_CONTROLr(unit, imask));
            }
        }
        return SOC_E_NONE;

        case L2MCm:
        if (soc_feature(unit, soc_feature_ip_ep_mem_parity)) {
            SOC_IF_ERROR_RETURN(READ_L2MC_PARITY_CONTROLr(unit, &imask));
            soc_reg_field_set(unit, L2MC_PARITY_CONTROLr, &imask,
                              PARITY_ENf, (enable ? 1 : 0));
            soc_reg_field_set(unit, L2MC_PARITY_CONTROLr, &imask,
                              PARITY_IRQ_ENf, (enable ? 1 : 0));
            SOC_IF_ERROR_RETURN(WRITE_L2MC_PARITY_CONTROLr(unit, imask));
        }
        return SOC_E_NONE;

        case L3_IPMCm:
        if (!SOC_IS_HAWKEYE(unit)) {
            if (soc_feature(unit, soc_feature_ip_ep_mem_parity)) {
                SOC_IF_ERROR_RETURN(READ_L3_IPMC_PARITY_CONTROLr(unit, &imask));
                soc_reg_field_set(unit, L3_IPMC_PARITY_CONTROLr, &imask,
                                  PARITY_ENf, (enable ? 1 : 0));
                soc_reg_field_set(unit, L3_IPMC_PARITY_CONTROLr, &imask,
                                  PARITY_IRQ_ENf, (enable ? 1 : 0));
                SOC_IF_ERROR_RETURN(WRITE_L3_IPMC_PARITY_CONTROLr(unit, imask));
            }
        }
        return SOC_E_NONE;

        case VLAN_TABm:
        if (soc_feature(unit, soc_feature_ip_ep_mem_parity)) {
            SOC_IF_ERROR_RETURN(READ_VLAN_PARITY_CONTROLr(unit, &imask));
            soc_reg_field_set(unit, VLAN_PARITY_CONTROLr, &imask,
                              PARITY_ENf, (enable ? 1 : 0));
            soc_reg_field_set(unit, VLAN_PARITY_CONTROLr, &imask,
                              PARITY_IRQ_ENf, (enable ? 1 : 0));
            SOC_IF_ERROR_RETURN(WRITE_VLAN_PARITY_CONTROLr(unit, imask));
        }
        return SOC_E_NONE;
#endif /* BCM_RAVEN_SUPPORT */

        default:
            break;    /* Look at MMU memories below */
    }

    SOC_IF_ERROR_RETURN(READ_MEMFAILINTMASKr(unit, &imask));
    oimask = imask;
    /* MISCCONFIGr has PARITY_CHECK_EN & CELLCRCCHECKEN control */
    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &misc_cfg));
    omisc_cfg = misc_cfg;

    switch (mem) {
        /* Covered by parity */
        case MMU_CFAPm:
            soc_reg_field_set(unit, MEMFAILINTMASKr, &imask,
                              CFAPPARITYERRORINTMASKf, (enable ? 1 : 0));
            break;

        case MMU_CCPm:
            soc_reg_field_set(unit, MEMFAILINTMASKr, &imask,
                              CCPPARITYERRORINTMASKf, (enable ? 1 : 0));
            break;

        case MMU_CBPPKTHEADER0m:
        case MMU_CBPPKTHEADER1m:
            soc_reg_field_set(unit, MEMFAILINTMASKr, &imask,
                              CBPPKTHDRPARITYERRORINTMASKf, (enable ? 1 : 0));
            break;

        case MMU_CBPCELLHEADERm:
            soc_reg_field_set(unit, MEMFAILINTMASKr, &imask,
                              CBPCELLHDRPARITYERRORINTMASKf, (enable ? 1 : 0));
            break;

        case MMU_XQ0m:
        case MMU_XQ1m:
        case MMU_XQ2m:
        case MMU_XQ3m:
        case MMU_XQ4m:
        case MMU_XQ5m:
        case MMU_XQ6m:
        case MMU_XQ7m:
        case MMU_XQ8m:
        case MMU_XQ9m:
        case MMU_XQ10m:
        case MMU_XQ11m:
        case MMU_XQ12m:
        case MMU_XQ13m:
        case MMU_XQ14m:
        case MMU_XQ15m:
        case MMU_XQ16m:
        case MMU_XQ17m:
        case MMU_XQ18m:
        case MMU_XQ19m:
        case MMU_XQ20m:
        case MMU_XQ21m:
        case MMU_XQ22m:
        case MMU_XQ23m:
        case MMU_XQ24m:
        case MMU_XQ25m:
        case MMU_XQ26m:
        case MMU_XQ27m:
        case MMU_XQ28m:
#if defined(BCM_RAPTOR_SUPPORT)
        case MMU_XQ29m:
        case MMU_XQ30m:
        case MMU_XQ31m:
#if defined(BCM_RAPTOR1_SUPPORT)
        case MMU_XQ32m:
        case MMU_XQ33m:
        case MMU_XQ34m:
        case MMU_XQ35m:
        case MMU_XQ36m:
        case MMU_XQ37m:
        case MMU_XQ38m:
        case MMU_XQ39m:
        case MMU_XQ40m:
        case MMU_XQ41m:
        case MMU_XQ42m:
        case MMU_XQ43m:
        case MMU_XQ44m:
        case MMU_XQ45m:
        case MMU_XQ46m:
        case MMU_XQ47m:
        case MMU_XQ48m:
        case MMU_XQ49m:
        case MMU_XQ50m:
        case MMU_XQ51m:
        case MMU_XQ52m:
        case MMU_XQ53m:
#endif /* BCM_RAPTOR1_SUPPORT */
#endif /* BCM_RAPTOR_SUPPORT */
            soc_reg_field_set(unit, MEMFAILINTMASKr, &imask,
                              XQPARITYERRORINTMASKf, (enable ? 1 : 0));
            break;

        /* Covered by CRC */
        case MMU_CBPDATA0m:
        case MMU_CBPDATA1m:
        case MMU_CBPDATA2m:
        case MMU_CBPDATA3m:
        case MMU_CBPDATA4m:
        case MMU_CBPDATA5m:
        case MMU_CBPDATA6m:
        case MMU_CBPDATA7m:
        case MMU_CBPDATA8m:
        case MMU_CBPDATA9m:
        case MMU_CBPDATA10m:
        case MMU_CBPDATA11m:
        case MMU_CBPDATA12m:
        case MMU_CBPDATA13m:
        case MMU_CBPDATA14m:
        case MMU_CBPDATA15m:
            soc_reg_field_set(unit, MEMFAILINTMASKr, &imask,
                              CRCERRORINTMASKf, (enable ? 1 : 0));
            parity_f = CELLCRCCHECKENf;
            break;
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_HAWKEYE_SUPPORT)
        case MMU_MIN_BUCKET_GPORTm:
        case MMU_MAX_BUCKET_GPORTm:
            soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg,
                              METERING_CLK_ENf, (enable ? 1 : 0));
            break;
#endif /* BCM_FIREBOLT2_SUPPORT */
        default:
            return SOC_E_NONE;    /* do nothing */
    }
    if (oimask != imask) {
        SOC_IF_ERROR_RETURN(WRITE_MEMFAILINTMASKr(unit, imask));
    }
    soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg,
                      parity_f, (enable ? 1 : 0));
    if (omisc_cfg != misc_cfg) {
        SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, misc_cfg));
    }
    return SOC_E_NONE;
}
#endif /* BCM_FIREBOLT_SUPPORT */

#ifdef BCM_SHADOW_SUPPORT
STATIC int
_soc_shadow_mem_parity_control(int unit, soc_mem_t mem, int copyno, int enable)
{

    COMPILER_REFERENCE(copyno);

    switch (mem) {
        case ING_VLAN_RANGEm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IPARS_DBGCTRLr, REG_PORT_ANY, 
                                        VLR0_ENABLE_ECCf, (enable ? 1 : 0))); 
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IPARS_DBGCTRLr, REG_PORT_ANY, 
                                        VLR1_ENABLE_ECCf, (enable ? 1 : 0))); 
            break;
        case SOURCE_TRUNK_MAP_TABLEm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IPARS_DBGCTRLr, REG_PORT_ANY, 
                                        STM_ENABLE_ECCf, (enable ? 1 : 0))); 
            break;
        case UDF_OFFSETm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IPARS_DBGCTRLr, REG_PORT_ANY, 
                                        UDF_ENABLE_ECCf, (enable ? 1 : 0))); 
            break;
        case VLAN_TABm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IVLAN_DBGCTRLr, REG_PORT_ANY, 
                                        VLAN_ENABLE_ECCf, (enable ? 1 : 0))); 
            break;
        case STG_TABm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IVLAN_DBGCTRLr, REG_PORT_ANY, 
                                        VLAN_STG_ENABLE_ECCf, (enable ? 1 : 0))); 
            break;
        case VLAN_SUBNETm:
        case VLAN_SUBNET_ONLYm:
        case VLAN_SUBNET_DATA_ONLYm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit,  VXLT_DEBUG_CONTROL_0r, 
                                        REG_PORT_ANY, VLAN_SUBNET_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit,  VXLT_DEBUG_CONTROL_0r, 
                                        REG_PORT_ANY, VLAN_SUBNET_DATA_ENABLE_ECCf, 
                                        (enable ? 1 : 0)));
            break;
        case VLAN_XLATEm:
        case VLAN_MACm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, VXLT_DEBUG_CONTROL_1r, 
                                        REG_PORT_ANY, VLAN_XLATE_MEM_0_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, VXLT_DEBUG_CONTROL_1r, 
                                        REG_PORT_ANY, VLAN_XLATE_MEM_1_ENABLE_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, VXLT_DEBUG_CONTROL_1r, 
                                        REG_PORT_ANY, VLAN_XLATE_MEM_2_ENABLE_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, VXLT_DEBUG_CONTROL_1r, 
                                        REG_PORT_ANY, VLAN_XLATE_MEM_3_ENABLE_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, VXLT_DEBUG_CONTROL_1r, 
                                        REG_PORT_ANY, VLAN_XLATE_MEM_4_ENABLE_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, VXLT_DEBUG_CONTROL_1r, 
                                        REG_PORT_ANY, VLAN_XLATE_MEM_5_ENABLE_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, VXLT_DEBUG_CONTROL_1r, 
                                        REG_PORT_ANY, VLAN_XLATE_MEM_6_ENABLE_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, VXLT_DEBUG_CONTROL_1r, 
                                        REG_PORT_ANY, VLAN_XLATE_MEM_7_ENABLE_ECCf, 
                                        (enable ? 1 : 0)));
            break;
        case VLAN_PROTOCOL_DATAm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, VXLT_DEBUG_CONTROL_1r, 
                                        REG_PORT_ANY, VLAN_PROTOCOL_ENABLE_ECCf, 
                                        (enable ? 1 : 0)));
            break;
        case DSCP_TABLEm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ISW1_ECC_DEBUGr, REG_PORT_ANY, 
                                        DSCP_TABLE_ENABLE_ECCf, (enable ? 1 : 0))); 
            break;
        case UFLOW_Bm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ISW1_ECC_DEBUGr, REG_PORT_ANY, 
                                        UFLOW_B_0_ENABLE_ECCf, (enable ? 1 : 0))); 
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ISW1_ECC_DEBUGr, REG_PORT_ANY, 
                                        UFLOW_B_1_ENABLE_ECCf, (enable ? 1 : 0))); 
            break;
        case UFLOW_Am:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ISW1_ECC_DEBUGr, REG_PORT_ANY, 
                                        UFLOW_A_0_ENABLE_ECCf, (enable ? 1 : 0))); 
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ISW1_ECC_DEBUGr, REG_PORT_ANY, 
                                        UFLOW_A_1_ENABLE_ECCf, (enable ? 1 : 0))); 
            break;
        case FP_TCAMm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, FP_DEBUG_CONTROLr, REG_PORT_ANY, 
                                        TCAM_ENABLE_ECCf, (enable ? 1 : 0))); 
            break;
        case FP_METER_TABLEm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, FP_DEBUG_CONTROLr, REG_PORT_ANY, 
                                        REFRESH_ENABLEf, (enable ? 1 : 0))); 
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, FP_RAM_CONTROL_1r, REG_PORT_ANY, 
                                        METER_EVEN_SLICE_0_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, FP_RAM_CONTROL_1r, REG_PORT_ANY, 
                                        METER_EVEN_SLICE_1_ENABLE_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, FP_RAM_CONTROL_1r, REG_PORT_ANY, 
                                        METER_EVEN_SLICE_2_ENABLE_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, FP_RAM_CONTROL_1r, REG_PORT_ANY, 
                                        METER_EVEN_SLICE_3_ENABLE_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, FP_RAM_CONTROL_1r, REG_PORT_ANY, 
                                        METER_ODD_SLICE_0_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, FP_RAM_CONTROL_1r, REG_PORT_ANY, 
                                        METER_ODD_SLICE_1_ENABLE_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, FP_RAM_CONTROL_1r, REG_PORT_ANY, 
                                        METER_ODD_SLICE_2_ENABLE_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, FP_RAM_CONTROL_1r, REG_PORT_ANY, 
                                        METER_ODD_SLICE_3_ENABLE_ECCf, 
                                        (enable ? 1 : 0)));
            break;
        case FP_COUNTER_TABLEm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, FP_RAM_CONTROL_64r, REG_PORT_ANY, 
                                        COUNTER_SLICE_0_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, FP_RAM_CONTROL_64r, REG_PORT_ANY, 
                                        COUNTER_SLICE_1_ENABLE_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, FP_RAM_CONTROL_64r, REG_PORT_ANY, 
                                        COUNTER_SLICE_2_ENABLE_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, FP_RAM_CONTROL_64r, REG_PORT_ANY, 
                                        COUNTER_SLICE_3_ENABLE_ECCf, 
                                        (enable ? 1 : 0)));
            break;
        case FP_STORM_CONTROL_METERSm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, FP_RAM_CONTROL_64r, REG_PORT_ANY, 
                                        STORM_ENABLE_ECCf, (enable ? 1 : 0))); 
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, FP_DEBUG_CONTROLr, REG_PORT_ANY, 
                                        REFRESH_ENABLEf, (enable ? 1 : 0))); 
            break;
        case FP_POLICY_TABLEm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, FP_RAM_CONTROL_64r, REG_PORT_ANY, 
                                        POLICY_SLICE_0_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, FP_RAM_CONTROL_64r, REG_PORT_ANY, 
                                        POLICY_SLICE_1_ENABLE_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, FP_RAM_CONTROL_64r, REG_PORT_ANY, 
                                        POLICY_SLICE_2_ENABLE_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, FP_RAM_CONTROL_64r, REG_PORT_ANY, 
                                        POLICY_SLICE_3_ENABLE_ECCf, 
                                        (enable ? 1 : 0)));
            break;
        case CELL_BUFFER3m:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ISW2_DEBUG0r, REG_PORT_ANY, 
                                        CELL_BUFFER3_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            break;
        case CELL_BUFFER2m:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ISW2_DEBUG0r, REG_PORT_ANY, 
                                        CELL_BUFFER2_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            break;
        case CELL_BUFFER1m:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ISW2_DEBUG0r, REG_PORT_ANY, 
                                        CELL_BUFFER1_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            break;
        case CELL_BUFFER0m:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ISW2_DEBUG0r, REG_PORT_ANY, 
                                        CELL_BUFFER0_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            break;
        case DEBUG_CAPTUREm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ISW2_DEBUG0r, REG_PORT_ANY, 
                                        DEBUG_CAPTURE_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            break;
        case VLAN_PROFILE_2m:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ISW2_DEBUG0r, REG_PORT_ANY, 
                                        VLAN_PROFILE_2_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            break;
        case EGR_MASKm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ISW2_DEBUG0r, REG_PORT_ANY, 
                                        EGR_MASK_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            break;
        case TRUNK_BITMAPm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ISW2_DEBUG0r, REG_PORT_ANY, 
                                        TRUNK_BITMAP_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            break;
        case E2E_HOL_STATUSm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ISW2_DEBUG1r, REG_PORT_ANY, 
                                        E2E_HOL_STATUS3_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ISW2_DEBUG1r, REG_PORT_ANY, 
                                        E2E_HOL_STATUS2_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ISW2_DEBUG1r, REG_PORT_ANY, 
                                        E2E_HOL_STATUS1_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ISW2_DEBUG1r, REG_PORT_ANY, 
                                        E2E_HOL_STATUS0_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            break;
        case EGR_VLANm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, EGR_EVLAN_ECC_DEBUGr, REG_PORT_ANY, 
                                        EP_VLAN_ENABLE_ECCf, (enable ? 1 : 0))); 
            break;
        case EGR_VLAN_STGm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, EGR_EVLAN_ECC_DEBUGr, 
                                        REG_PORT_ANY, EP_VLAN_STG_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            break;
        case EGR_VLAN_XLATEm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, EGR_EVXLT_ECC_DEBUGr, REG_PORT_ANY, 
                                        EP_VLAN_XLATE_U_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, EGR_EVXLT_ECC_DEBUGr, REG_PORT_ANY, 
                                        EP_VLAN_XLATE_L_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            break;
        case EGR_PRI_CNG_MAPm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, EGR_EVXLT_ECC_DEBUGr, REG_PORT_ANY, 
                                        EP_PRI_CNG_MAP_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            break;
        case EGR_DSCP_TABLEm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, EGR_EVXLT_ECC_DEBUGr, REG_PORT_ANY, 
                                        EP_DSCP_ENABLE_ECCf, (enable ? 1 : 0))); 
            break;
        case EGR_SCI_TABLEm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, EGR_EHCPM_ECC_DEBUGr, REG_PORT_ANY, 
                                        EP_SCI_TABLE_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            break;
        case EGR_PERQ_XMT_COUNTERSm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, EGR_EDB_ECC_DEBUGr, REG_PORT_ANY, 
                                        EP_PERQ_XMT_COUNTER_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            break;
        case MMU_CFAP_MEMm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MMU_ECC_DEBUG0r, REG_PORT_ANY, 
                                        CFAP_ENABLE_ECCf, (enable ? 1 : 0))); 
            break;
        case MMU_PKTLINK0m:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MMU_ECC_DEBUG0r, REG_PORT_ANY, 
                                        PKTLINK_ENABLE_ECCf, (enable ? 1 : 0))); 
            break;
        case MMU_CELLLINKm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MMU_ECC_DEBUG0r, REG_PORT_ANY, 
                                        CELLLINK_ENABLE_ECCf, (enable ? 1 : 0))); 
            break;
        case MMU_CBPDATA0m:
        case MMU_CBPDATA1m:
        case MMU_CBPDATA2m:
        case MMU_CBPDATA3m:
        case MMU_CBPDATA4m:
        case MMU_CBPDATA5m:
        case MMU_CBPDATA6m:
        case MMU_CBPDATA7m:
        case MMU_CBPDATA8m:
        case MMU_CBPDATA9m:
        case MMU_CBPDATA10m:
        case MMU_CBPDATA11m:
        case MMU_CBPDATA12m:
        case MMU_CBPDATA13m:
        case MMU_CBPDATA14m:
        case MMU_CBPDATA15m:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MMU_ECC_DEBUG0r, REG_PORT_ANY, 
                                        CELLDATA_ENABLE_ECCf, (enable ? 1 : 0))); 
            break;
        case MMU_CBPPKTLENGTHm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MMU_ECC_DEBUG0r, REG_PORT_ANY, 
                                        PKTLENGTH_ENABLE_ECCf, (enable ? 1 : 0))); 
            break;
        case MMU_CBPPKTHEADER0_MEM0m:
        case MMU_CBPPKTHEADER0_MEM1m:
        case MMU_CBPPKTHEADER0_MEM2m:
        case MMU_CBPPKTHEADER0_MEM3m:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MMU_ECC_DEBUG0r, REG_PORT_ANY, 
                                        PKTHDR0_ENABLE_ECCf, (enable ? 1 : 0))); 
            break;
        case MMU_AGING_EXPm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, PKTAGINGTIMERr, REG_PORT_ANY, 
                                        DURATIONSELECTf, (enable ? 0x1000 : 0))); 
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MMU_ECC_DEBUG0r, REG_PORT_ANY, 
                                        AGING_EXP_ENABLE_ECCf, (enable ? 1 : 0))); 
            break;
        case MMU_AGING_CTRm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, PKTAGINGTIMERr, REG_PORT_ANY, 
                                        DURATIONSELECTf, (enable ? 0x1000 : 0))); 
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MMU_ECC_DEBUG0r, REG_PORT_ANY, 
                                        AGING_CTR_ENABLE_ECCf, (enable ? 1 : 0))); 
            break;
        case ES_ARB_TDM_TABLEm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MMU_ECC_DEBUG0r, REG_PORT_ANY, 
                                        ES_ARB_TDM_TABLE_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            break;
        case CTR_MEMm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY, 
                                        CTR_MEM_ENABLE_ECCf, (enable ? 1 : 0))); 
            break;
        case MMU_WRED_PORT_THD_0_CELLm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY, 
                                        WRED_PORT_THD_0_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            break;
        case MMU_WRED_PORT_THD_1_CELLm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY, 
                                        WRED_PORT_THD_1_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            break;
        case MMU_WRED_THD_0_CELLm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY, 
                                        WRED_THD_0_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            break;
        case MMU_WRED_THD_1_CELLm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY, 
                                        WRED_THD_1_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            break;
        case MMU_WRED_PORT_CFG_CELLm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY, 
                                        WRED_PORT_CFG_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            break;
        case MMU_WRED_CFG_CELLm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY, 
                                        WRED_CFG_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            break;
/*        case MMU_MTRO_CPU_PKT_SHAPING_WRAPPERm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY, 
                                        MTRO_CPU_PKT_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            break;
        case MMU_MTRO_SHAPE_G0_BUCKET_WRAPPERm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY, 
                                        MTRO_SHAPE_G0_BUCKET_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            break;
        case MMU_MTRO_SHAPE_G0_CONFIG_WRAPPERm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY, 
                                        MTRO_SHAPE_G0_CONFIG_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            break;
        case MMU_MTRO_SHAPE_G1_BUCKET_WRAPPERm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY, 
                                        MTRO_SHAPE_G1_BUCKET_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            break;
        case MMU_MTRO_SHAPE_G1_CONFIG_WRAPPERm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY, 
                                        MTRO_SHAPE_G1_CONFIG_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            break;
        case MMU_MTRO_SHAPE_G2_BUCKET_WRAPPERm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY, 
                                        MTRO_SHAPE_G2_BUCKET_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            break;
        case MMU_MTRO_SHAPE_G2_CONFIG_WRAPPERm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY, 
                                        MTRO_SHAPE_G2_CONFIG_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            break;
        case MMU_MTRO_SHAPE_G3_BUCKET_WRAPPERm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY, 
                                        MTRO_SHAPE_G3_BUCKET_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            break;
        case MMU_MTRO_SHAPE_G3_CONFIG_WRAPPERm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY, 
                                        MTRO_SHAPE_G3_CONFIG_ENABLE_ECCf, 
                                        (enable ? 1 : 0))); 
            break;*/
        case ESEC_PKT_HEADER_CAPTURE_BUFFERm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ESEC_ECC_DEBUGr, 1, 
                                        DEBUGFF_ENABLE_ECCf, (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ESEC_ECC_DEBUGr, 5, 
                                        DEBUGFF_ENABLE_ECCf, (enable ? 1 : 0)));
            break;
        case QL_TABLE0m:
        case QL_TABLE1m:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MISCCONFIGr, REG_PORT_ANY, 
                                        REFRESH_ENf, (enable ? 1 : 0)));
            break;
        case ESEC_SC_TABLEm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ESEC_ECC_DEBUGr, 1, 
                                        SCDB_ENABLE_ECCf, (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ESEC_ECC_DEBUGr, 5, 
                                        SCDB_ENABLE_ECCf, (enable ? 1 : 0)));
            break;
        case ESEC_SA_TABLEm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ESEC_ECC_DEBUGr, 1, 
                                        SADB_ENABLE_ECCf, (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ESEC_ECC_DEBUGr, 5, 
                                        SADB_ENABLE_ECCf, (enable ? 1 : 0)));
            break;
        case ESEC_SA_KEY_TABLEm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ESEC_ECC_DEBUGr, 1, 
                                        SAKEY_ENABLE_ECCf, (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ESEC_ECC_DEBUGr, 5, 
                                        SAKEY_ENABLE_ECCf, (enable ? 1 : 0)));
            break;
        case OUTUNCTRLUCASTPKTSm:
        case OUTUNCTRLMCASTPKTSm:
        case OUTUNCTRLBCASTPKTSm:
        case OUTCTRLMCASTPKTSm:    
        case OUTCTRLUCASTPKTSm:
        case OUTCTRLBCASTPKTSm:        
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ESEC_ECC_DEBUGr, 1, 
                                        MIB1_ENABLE_ECCf, (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ESEC_ECC_DEBUGr, 5, 
                                        MIB1_ENABLE_ECCf, (enable ? 1 : 0)));
            break;
        case TXSAPRTCPKTSm:
        case TXSACRPTPKTSm:        
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ESEC_ECC_DEBUGr, 1, 
                                        MIB2_ENABLE_ECCf, (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ESEC_ECC_DEBUGr, 5, 
                                        MIB2_ENABLE_ECCf, (enable ? 1 : 0)));
            break;
        case TXSCPRTCPKTSm:
        case TXSCCRPTPKTSm:        
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ESEC_ECC_DEBUGr, 1, 
                                        MIB3_ENABLE_ECCf, (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ESEC_ECC_DEBUGr, 5, 
                                        MIB3_ENABLE_ECCf, (enable ? 1 : 0)));
            break;
        case TXSCPRTCBYTm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ESEC_ECC_DEBUGr, 1, 
                                        MIB4_ENABLE_ECCf, (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ESEC_ECC_DEBUGr, 5, 
                                        MIB4_ENABLE_ECCf, (enable ? 1 : 0)));
            break;
        case TXSCCRPTBYTm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ESEC_ECC_DEBUGr, 1, 
                                        MIB5_ENABLE_ECCf, (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ESEC_ECC_DEBUGr, 5, 
                                        MIB5_ENABLE_ECCf, (enable ? 1 : 0)));
            break;
        case IL_STAT_MEM_0m:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_ECC_DEBUG0r, 9, 
                                        IL_RX_STAT0_ENABLE_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_ECC_DEBUG0r, 13, 
                                        IL_RX_STAT0_ENABLE_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_GLOBAL_CONFIGr, 9, 
                                        XGS_COUNTER_COMPAT_MODEf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_GLOBAL_CONFIGr, 13, 
                                        XGS_COUNTER_COMPAT_MODEf, 
                                        (enable ? 1 : 0)));
            break;
        case IL_STAT_MEM_1m:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_ECC_DEBUG0r, 9, 
                                        IL_RX_STAT1_ENABLE_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_ECC_DEBUG0r, 13, 
                                        IL_RX_STAT1_ENABLE_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_GLOBAL_CONFIGr, 9, 
                                        XGS_COUNTER_COMPAT_MODEf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_GLOBAL_CONFIGr, 13, 
                                        XGS_COUNTER_COMPAT_MODEf, 
                                        (enable ? 1 : 0)));
            break;
        case IL_STAT_MEM_2m:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_ECC_DEBUG0r, 9, 
                                        IL_RX_STAT2_ENABLE_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_ECC_DEBUG0r, 13, 
                                        IL_RX_STAT2_ENABLE_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_GLOBAL_CONFIGr, 9, 
                                        XGS_COUNTER_COMPAT_MODEf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_GLOBAL_CONFIGr, 13, 
                                        XGS_COUNTER_COMPAT_MODEf, 
                                        (enable ? 1 : 0)));
            break;
        case IL_STAT_MEM_3m:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_ECC_DEBUG0r, 9, 
                                        IL_TX_STAT0_ENABLE_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_ECC_DEBUG0r, 13, 
                                        IL_TX_STAT0_ENABLE_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_GLOBAL_CONFIGr, 9, 
                                        XGS_COUNTER_COMPAT_MODEf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_GLOBAL_CONFIGr, 13, 
                                        XGS_COUNTER_COMPAT_MODEf, 
                                        (enable ? 1 : 0)));
            break;
        case IL_STAT_MEM_4m:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_ECC_DEBUG0r, 9, 
                                        IL_TX_STAT1_ENABLE_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_ECC_DEBUG0r, 13, 
                                        IL_TX_STAT1_ENABLE_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_GLOBAL_CONFIGr, 9, 
                                        XGS_COUNTER_COMPAT_MODEf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_GLOBAL_CONFIGr, 13, 
                                        XGS_COUNTER_COMPAT_MODEf, 
                                        (enable ? 1 : 0)));
            break;
        case ISEC_SA_TABLEm:
        case ISEC_SA_KEY_TABLEm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 1, SA1_EN_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 5, SA1_EN_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 1, SA2_EN_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 5, SA2_EN_ECCf, 
                                        (enable ? 1 : 0)));
            break;
        case RXSAUNUSEDSAPKTSm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 1, 
                                        RXSASTATSUNUSEDSAPKTS_EN_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 5, 
                                        RXSASTATSUNUSEDSAPKTS_EN_ECCf, 
                                        (enable ? 1 : 0)));
            break;
        case RXSANOTUSINGSAPKTSm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 1, 
                                        RXSASTATSNOTUSINGSAPKTS_EN_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 5, 
                                        RXSASTATSNOTUSINGSAPKTS_EN_ECCf, 
                                        (enable ? 1 : 0)));
            break;
        case RXSANOTVLDPKTSm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 1, 
                                        RXSASTATSNOTVALIDPKTS_EN_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 5, 
                                        RXSASTATSNOTVALIDPKTS_EN_ECCf, 
                                        (enable ? 1 : 0)));
            break;
        case RXSAINVLDPKTSm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 1, 
                                        RXSASTATSINVALIDPKTS_EN_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 5, 
                                        RXSASTATSINVALIDPKTS_EN_ECCf, 
                                        (enable ? 1 : 0)));
            break;
        case RXSAOKPKTSm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 1, 
                                        RXSASTATSOKPKTS_EN_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 5, 
                                        RXSASTATSOKPKTS_EN_ECCf, 
                                        (enable ? 1 : 0)));
            break;
        case INCTRLUCASTPKTSm:
        case INCTRLMCASTPKTSm:
        case INCTRLBCASTPKTSm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 1, 
                                        RXPACKETTYPE_EN_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 5, 
                                        RXPACKETTYPE_EN_ECCf, 
                                        (enable ? 1 : 0)));
            break;
        case INCTRLDISCPKTSm:
        case INCTRLERRPKTSm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 1, 
                                        RXERRDSCRDSPKTS_EN_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 5, 
                                        RXERRDSCRDSPKTS_EN_ECCf, 
                                        (enable ? 1 : 0)));
            break; 
        case RXUNTAGPKTSm:
        case RXNOTAGPKTSm:
        case RXBADTAGPKTSm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 1, 
                                        RXTAGUNTAGNONEBAD_EN_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 5, 
                                        RXTAGUNTAGNONEBAD_EN_ECCf, 
                                        (enable ? 1 : 0)));
            break;  
        case RXUNKNOWNSCIPKTSm:
        case RXNOSCIPKTSm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 1, 
                                        RXSCIUNKNOWNNONEPKTS_EN_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 5, 
                                        RXSCIUNKNOWNNONEPKTS_EN_ECCf, 
                                        (enable ? 1 : 0)));
            break; 
        case RXSCUNUSEDSAPKTSm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 1,
                                        RXSCSTATSUNUSEDSAPKTS_EN_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 5, 
                                        RXSCSTATSUNUSEDSAPKTS_EN_ECCf, 
                                        (enable ? 1 : 0)));
            break;
        case RXSCNOTUSINGSAPKTSm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 1, 
                                        RXSCSTATSNOTUSINGSAPKTS_EN_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 5, 
                                        RXSCSTATSNOTUSINGSAPKTS_EN_ECCf, 
                                        (enable ? 1 : 0)));
            break;
        case RXSCLATEPKTSm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 1, 
                                        RXSCSTATSLATEPKTS_EN_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 5, 
                                        RXSCSTATSLATEPKTS_EN_ECCf, 
                                        (enable ? 1 : 0)));
            break;
        case RXSCNOTVLDPKTSm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 1, 
                                        RXSCSTATSNOTVALIDPKTS_EN_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 5, 
                                        RXSCSTATSNOTVALIDPKTS_EN_ECCf, 
                                        (enable ? 1 : 0)));
            break;
        case RXSCINVLDPKTSm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 1, 
                                        RXSCSTATSINVALIDPKTS_EN_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 5, 
                                        RXSCSTATSINVALIDPKTS_EN_ECCf, 
                                        (enable ? 1 : 0)));
            break;
        case RXSCDLYPKTSm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 1, 
                                        RXSCSTATSDELAYEDPKTS_EN_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 5, 
                                        RXSCSTATSDELAYEDPKTS_EN_ECCf, 
                                        (enable ? 1 : 0)));
            break;
        case RXSCUNCHKPKTSm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 1, 
                                        RXSCSTATSUNCHECKEDPKTS_EN_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 5, 
                                        RXSCSTATSUNCHECKEDPKTS_EN_ECCf, 
                                        (enable ? 1 : 0)));
            break;
        case RXSCOKPKTSm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 1, 
                                        RXSCSTATSOKPKTS_EN_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 5, 
                                        RXSCSTATSOKPKTS_EN_ECCf, 
                                        (enable ? 1 : 0)));
            break;
        case RXSCVLDTBYTm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 1, 
                                        RXSCSTATSOCTETSVALIDATED_EN_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 5, 
                                        RXSCSTATSOCTETSVALIDATED_EN_ECCf, 
                                        (enable ? 1 : 0)));
            break;
        case RXSCDCRPTBYTm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 1, 
                                        RXSCSTATSOCTETSDECRYPTED_EN_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 5, 
                                        RXSCSTATSOCTETSDECRYPTED_EN_ECCf, 
                                        (enable ? 1 : 0)));
            break;
        case FILTERMATCHCOUNTm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 1, 
                                        FILTERMATCHCOUNT_EN_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 5, 
                                        FILTERMATCHCOUNT_EN_ECCf, 
                                        (enable ? 1 : 0)));
            break;
        case ISEC_DEBUG_RAMm:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 1, DEBUGRAM_EN_ECCf, 
                                        (enable ? 1 : 0)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, DEBUG20r, 5, DEBUGRAM_EN_ECCf, 
                                        (enable ? 1 : 0)));
            break;
        default:
            return SOC_E_NONE;    /* do nothing */
    }
    return SOC_E_NONE;
}
#endif

#ifdef BCM_BRADLEY_SUPPORT
STATIC int
_soc_hb_mem_parity_control(int unit, soc_mem_t mem, int copyno, int enable)
{
    uint32 rval, orval, imask, oimask,
           misc_cfg, sbs_ctrl;
    soc_reg_t mctlreg = INVALIDr, sbsreg = INVALIDr;
    soc_field_t imask_f[4] = { INVALIDf, INVALIDf, INVALIDf, INVALIDf };
    int en_val = enable ? 1 : 0;
    int i;

    COMPILER_REFERENCE(copyno);

    switch (mem) {
    case VLAN_TABm:
        if (!SOC_IS_SHADOW(unit)) {
            mctlreg = VLAN_PARITY_CONTROLr;
            imask_f[0] = TOQ0_VLAN_TBL_PAR_ERR_ENf;
            imask_f[1] = TOQ1_VLAN_TBL_PAR_ERR_ENf;
            imask_f[2] = MEM1_VLAN_TBL_PAR_ERR_ENf;
            sbsreg = SBS_CONTROLr;
        }
        break;
    case L2Xm:
    case L2_ENTRY_ONLYm:
#ifdef BCM_SCORPION_SUPPORT
    case L2_ENTRY_SCRATCHm:
#endif
        mctlreg = L2_ENTRY_PARITY_CONTROLr;
        break;
    case L2MCm:
        mctlreg = L2MC_PARITY_CONTROLr;
        sbsreg = SBS_CONTROLr;
        break;
    case L3_ENTRY_ONLYm:
    case L3_ENTRY_VALID_ONLYm:
    case L3_ENTRY_IPV4_UNICASTm:
    case L3_ENTRY_IPV4_MULTICASTm:
    case L3_ENTRY_IPV6_UNICASTm:
    case L3_ENTRY_IPV6_MULTICASTm:
        mctlreg = L3_ENTRY_PARITY_CONTROLr;
        break;
    case EGR_L3_INTFm:
        mctlreg = EGR_L3_INTF_PARITY_CONTROLr;
        sbsreg = EGR_SBS_CONTROLr;
        break;
    case EGR_L3_NEXT_HOPm:
        mctlreg = EGR_L3_NEXT_HOP_PARITY_CONTROLr;
        sbsreg = EGR_SBS_CONTROLr;
        break;
    case EGR_VLANm:
        if (!SOC_IS_SHADOW(unit)) {
            mctlreg = EGR_VLAN_PARITY_CONTROLr;
            sbsreg = EGR_SBS_CONTROLr;
        }
        break;
#ifdef BCM_SCORPION_SUPPORT
    case SOURCE_TRUNK_MAP_TABLEm:
        if (SOC_IS_SC_CQ(unit) && !SOC_IS_SHADOW(unit)) {
            mctlreg = SRC_TRUNK_PARITY_CONTROLr;
            sbsreg = SBS_CONTROLr;
        } /* No parity in Bradley */
        break;
    case VLAN_MACm:
    case VLAN_XLATEm:
        if (SOC_IS_SC_CQ(unit) && !SOC_IS_SHADOW(unit)) {
            mctlreg = VLAN_MAC_OR_XLATE_PARITY_CONTROLr;
            sbsreg = SBS_CONTROLr;
        } /* No parity in Bradley */
        break;
    case VFP_POLICY_TABLEm:
        if (SOC_IS_SC_CQ(unit)) {
            mctlreg = VFP_POLICY_TABLE_PARITY_CONTROLr;
            sbsreg = SBS_CONTROLr;
        } /* No parity in Bradley */
        break;
    case L3_DEFIPm:
    case L3_DEFIP_DATA_ONLYm:
        if (SOC_IS_SC_CQ(unit)) {
            mctlreg =  L3_DEFIP_PARITY_CONTROLr;
            sbsreg = SBS_CONTROLr;
        } /* No parity in Bradley */
        break;
    case INITIAL_ING_L3_NEXT_HOPm:
        if (SOC_IS_SC_CQ(unit)) {
            mctlreg = INITIAL_ING_L3_NEXT_HOP_PARITY_CONTROLr;
            sbsreg = SBS_CONTROLr;
        } /* No parity in Bradley */
        break;
    case FP_POLICY_TABLEm:
        if (SOC_IS_SC_CQ(unit) && !SOC_IS_SHADOW(unit)) {
            mctlreg = IFP_POLICY_TABLE_PARITY_CONTROLr;
            sbsreg = SBS_CONTROLr;
        } /* No parity in Bradley */
        break;
    case ING_L3_NEXT_HOPm:
        if (SOC_IS_SC_CQ(unit)) {
            mctlreg = ING_L3_NEXT_HOP_PARITY_CONTROLr;
            sbsreg = SBS_CONTROLr;
        } /* No parity in Bradley */
        break;
    case EGR_MASKm:
        if (SOC_IS_SC_CQ(unit) && !SOC_IS_SHADOW(unit)) {
            mctlreg = EGR_MASK_PARITY_CONTROLr;
            sbsreg = SBS_CONTROLr;
        } /* No parity in Bradley */
        break;
    case SRC_MODID_BLOCKm:
        if (SOC_IS_SC_CQ(unit)) {
            mctlreg = SRC_MODID_BLOCK_PARITY_CONTROLr;
            sbsreg = SBS_CONTROLr;
        } /* No parity in Bradley */
        break;
    case MODPORT_MAPm:
    case MODPORT_MAP_SWm:
        if (SOC_IS_SC_CQ(unit)) {
            mctlreg = MODPORT_MAP_SW_PARITY_CONTROLr;
            sbsreg = SBS_CONTROLr;
        } /* No parity in Bradley */
        break;
    case MODPORT_MAP_IMm:
        if (SOC_IS_SC_CQ(unit)) {
            mctlreg = MODPORT_MAP_IM_PARITY_CONTROLr;
            sbsreg = SBS_CONTROLr;
        } /* No parity in Bradley */
        break;
    case MODPORT_MAP_EMm:
        if (SOC_IS_SC_CQ(unit)) {
            mctlreg = MODPORT_MAP_EM_PARITY_CONTROLr;
            sbsreg = SBS_CONTROLr;
        } /* No parity in Bradley */
        break;
    case EGR_VLAN_XLATEm:
        if (SOC_IS_SC_CQ(unit) && !SOC_IS_SHADOW(unit)) {
            mctlreg = EGR_VLAN_XLATE_PARITY_CONTROLr;
            sbsreg = EGR_SBS_CONTROLr;
        }
        break;
    case EGR_IP_TUNNELm:
    case EGR_IP_TUNNEL_IPV6m:
        if (SOC_IS_SC_CQ(unit)) {
            mctlreg = EGR_IP_TUNNEL_PARITY_CONTROLr;
            sbsreg = EGR_SBS_CONTROLr;
        } /* No parity in Bradley */
        break;
#endif
    case MMU_AGING_CTRm:
        imask_f[0] = AGING_CTR_PAR_ERR_ENf;
        break;
    case MMU_AGING_EXPm:
        imask_f[0] = AGING_EXP_PAR_ERR_ENf;
        break;
    case L3_IPMCm:
#ifdef BCM_SCORPION_SUPPORT
        if (SOC_IS_SC_CQ(unit)) {
            mctlreg = L3MC_PARITY_CONTROLr;
            sbsreg = SBS_CONTROLr;
        }
        /* Fall through */
#endif
    case MMU_IPMC_VLAN_TBLm:
    case MMU_IPMC_VLAN_TBL_MEM0m:
    case MMU_IPMC_VLAN_TBL_MEM1m:
    case MMU_IPMC_GROUP_TBL0m:
    case MMU_IPMC_GROUP_TBL1m:
    case MMU_IPMC_GROUP_TBL2m:
    case MMU_IPMC_GROUP_TBL3m:
#ifdef BCM_SCORPION_SUPPORT
    case MMU_IPMC_GROUP_TBL4m:
    case MMU_IPMC_GROUP_TBL5m:
    case MMU_IPMC_GROUP_TBL6m:
#endif
        imask_f[0] = TOQ0_IPMC_TBL_PAR_ERR_ENf;
        imask_f[1] = TOQ1_IPMC_TBL_PAR_ERR_ENf;
        imask_f[2] = MEM1_IPMC_TBL_PAR_ERR_ENf;
        imask_f[3] = ENQ_IPMC_TBL_PAR_ERR_ENf;
        break;
    case MMU_CFAP_MEMm :
        imask_f[0] = CFAP_PAR_ERR_ENf;
        break;
    case MMU_CCP_MEMm :
        imask_f[0] = CCP_PAR_ERR_ENf;
        break;
    case MMU_CELLLINKm:
        imask_f[0] = TOQ0_CELLLINK_PAR_ERR_ENf;
        imask_f[1] = TOQ1_CELLLINK_PAR_ERR_ENf;
        break;
    case MMU_PKTLINK0m:
    case MMU_PKTLINK1m:
    case MMU_PKTLINK2m:
    case MMU_PKTLINK3m:
    case MMU_PKTLINK4m:
    case MMU_PKTLINK5m:
    case MMU_PKTLINK6m:
    case MMU_PKTLINK7m:
    case MMU_PKTLINK8m:
    case MMU_PKTLINK9m:
    case MMU_PKTLINK10m:
    case MMU_PKTLINK11m:
    case MMU_PKTLINK12m:
    case MMU_PKTLINK13m:
    case MMU_PKTLINK14m:
    case MMU_PKTLINK15m:
    case MMU_PKTLINK16m:
    case MMU_PKTLINK17m:
    case MMU_PKTLINK18m:
    case MMU_PKTLINK19m:
    case MMU_PKTLINK20m:
#ifdef BCM_TRX_SUPPORT
    case MMU_PKTLINK21m:
    case MMU_PKTLINK22m:
    case MMU_PKTLINK23m:
    case MMU_PKTLINK24m:
    case MMU_PKTLINK25m:
    case MMU_PKTLINK26m:
    case MMU_PKTLINK27m:
    case MMU_PKTLINK28m:
#endif
        imask_f[0] = TOQ0_PKTLINK_PAR_ERR_ENf;
        imask_f[1] = TOQ1_PKTLINK_PAR_ERR_ENf;
        break;
    case MMU_CBPPKTHEADER0_MEM0m:
    case MMU_CBPPKTHEADER0_MEM1m:
    case MMU_CBPPKTHEADER0_MEM2m:
    case MMU_CBPPKTHEADER1_MEM0m:
    case MMU_CBPPKTHEADER1_MEM1m:
    case MMU_CBPPKTHEADERCPUm:
#ifdef BCM_SCORPION_SUPPORT
    case MMU_CBPPKTHEADER0_MEM3m:
#endif
        imask_f[0] = TOQ0_PKTHDR1_PAR_ERR_ENf;
        imask_f[1] = TOQ1_PKTHDR1_PAR_ERR_ENf;
        break;
    case MMU_CBPCELLHEADERm:
        imask_f[0] = TOQ0_CELLHDR_PAR_ERR_ENf;
        imask_f[1] = TOQ1_CELLHDR_PAR_ERR_ENf;
        break;
    case MMU_CBPDATA0m:
    case MMU_CBPDATA1m:
    case MMU_CBPDATA2m:
    case MMU_CBPDATA3m:
    case MMU_CBPDATA4m:
    case MMU_CBPDATA5m:
    case MMU_CBPDATA6m:
    case MMU_CBPDATA7m:
    case MMU_CBPDATA8m:
    case MMU_CBPDATA9m:
    case MMU_CBPDATA10m:
    case MMU_CBPDATA11m:
    case MMU_CBPDATA12m:
    case MMU_CBPDATA13m:
    case MMU_CBPDATA14m:
    case MMU_CBPDATA15m:
        /* Covered by CRC */
        imask_f[0] = DEQ0_CELLCRC_ERR_ENf;
        imask_f[1] = DEQ1_CELLCRC_ERR_ENf;
        break;
    case MMU_CBPPKTLENGTHm:
        /* Covered by CRC */
        imask_f[0] = DEQ0_LENGTH_PAR_ERR_ENf;
        imask_f[1] = DEQ1_LENGTH_PAR_ERR_ENf;
        break;

    case MMU_WRED_CFG_CELLm:
    case MMU_WRED_PORT_CFG_CELLm:
    case MMU_WRED_PORT_THD_0_CELLm:
    case MMU_WRED_PORT_THD_1_CELLm:
    case MMU_WRED_THD_0_CELLm:
    case MMU_WRED_THD_1_CELLm:
        imask_f[0] = WRED_PAR_ERR_ENf;
        break;
    default:
        /* Do nothing */
        return SOC_E_NONE;
    }

    /*
     * Turn off parity
     */
    if (mctlreg != INVALIDr) {
        /* Memory has a separate parity control */
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, mctlreg, REG_PORT_ANY, 0, &rval));
        orval = rval;
        soc_reg_field_set(unit, mctlreg, &rval, PARITY_ENf, en_val);
        soc_reg_field_set(unit, mctlreg, &rval, PARITY_IRQ_ENf, en_val);
        if (orval != rval) {
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, mctlreg, REG_PORT_ANY, 0, rval));
        }

        if ((sbsreg != INVALIDr) && (!SOC_IS_SHADOW(unit))) { /* Dual pipe controls needed */
            sbs_ctrl = 0;
            soc_reg_field_set(unit, sbsreg, &sbs_ctrl, PIPE_SELECTf,
                              SOC_PIPE_SELECT_Y);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, sbsreg, REG_PORT_ANY, 0, sbs_ctrl));
            
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, mctlreg, REG_PORT_ANY, 0, &rval));
            orval = rval;
            soc_reg_field_set(unit, mctlreg, &rval, PARITY_ENf, en_val);
            soc_reg_field_set(unit, mctlreg, &rval, PARITY_IRQ_ENf, en_val);
            if (orval != rval) {
                SOC_IF_ERROR_RETURN(soc_reg32_set(unit, mctlreg, REG_PORT_ANY, 0, rval));
            }

            soc_reg_field_set(unit, sbsreg, &sbs_ctrl, PIPE_SELECTf,
                              SOC_PIPE_SELECT_X);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, sbsreg, REG_PORT_ANY, 0, sbs_ctrl));
        }
    } else {
        /* Memory does not have a separate parity control */
        SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &misc_cfg));
        soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg, PARITY_STAT_CLEARf, 1);
        SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, misc_cfg));

        soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg, PARITY_STAT_CLEARf, 0);
        if (soc_reg_field_valid(unit, MISCCONFIGr, PARITY_GEN_ENf)) {
            soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg, PARITY_GEN_ENf, en_val);
            soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg, PARITY_CHK_ENf, en_val);
        }
        soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg, REFRESH_ENf, en_val);
        SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, misc_cfg));
    }

    /*
     * Disable parity and CRC interrupts
     */
    if ((imask_f[0] != INVALIDr) && SOC_REG_IS_VALID(unit, MEM_FAIL_INT_ENr)) {
        SOC_IF_ERROR_RETURN(READ_MEM_FAIL_INT_ENr(unit, &imask));
        oimask = imask;
        for (i = 0; i < COUNTOF(imask_f); i++) {
            if (imask_f[i] != INVALIDr) {
                soc_reg_field_set(unit, MEM_FAIL_INT_ENr, &imask, 
                                  imask_f[i], en_val);
            }
        }
        if (oimask != imask) {
            SOC_IF_ERROR_RETURN(WRITE_MEM_FAIL_INT_ENr(unit, imask));
        }
    }

    return SOC_E_NONE;
}
#endif /* BCM_BRADLEY_SUPPORT */

#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
/*
 * Toggle parity checks during memory tests
 */
int
soc_mem_parity_control(int unit, soc_mem_t mem, int copyno, int enable)
{
    if (mem != INVALIDm) {
        LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                                "soc_mem_parity_control: unit %d memory %s.%s %sable\n"),
                     unit,  SOC_MEM_UFNAME(unit, mem),
                     SOC_BLOCK_NAME(unit, copyno),
                     enable ? "en" : "dis"));
    }
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3(unit)) {
        return soc_trident3_mem_ser_control(unit, mem, copyno, enable);
    }
#endif
#ifdef BCM_MAVERICK2_SUPPORT
    if (SOC_IS_MAVERICK2(unit)) {
        return soc_mv2_mem_ser_control(unit, mem, copyno, enable);
    }
#endif
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        return soc_hr4_mem_ser_control(unit, mem, copyno, enable);
    }
#endif
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        return soc_fb6_mem_ser_control(unit, mem, copyno, enable);
    }
#endif
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        return soc_hx5_mem_ser_control(unit, mem, copyno, enable);
    }
#endif
#ifdef BCM_SHADOW_SUPPORT
    if (SOC_IS_SHADOW(unit)) {
        return _soc_shadow_mem_parity_control(unit, mem, copyno, enable);
    }
#endif /* BCM_SHADOW_SUPPORT */
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return soc_tomahawk3_mem_ser_control(unit, mem, copyno, enable);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TOMAHAWK2_SUPPORT)
    if (SOC_IS_TOMAHAWK(unit) || SOC_IS_TOMAHAWK2(unit)) {
        return soc_tomahawk_mem_ser_control(unit, mem, copyno, enable);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        return _soc_monterey_mem_ser_control(unit, mem, copyno, enable);
    }
#endif /* BCM_MONTEREY_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        
        return SOC_E_NONE;
    }
#endif /* BCM_MONTEREY_SUPPORT */
#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        return _soc_apache_mem_ser_control(unit, mem, copyno, enable);
    }
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        return _soc_trident2_mem_ser_control(unit, mem, copyno, enable);
    }
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit)) {
        return _soc_trident_mem_parity_control(unit, mem, copyno, enable);
    }
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return _soc_triumph3_mem_parity_control(unit, mem, copyno, enable);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit)) {
        return _soc_triumph2_mem_parity_control(unit, mem, copyno, enable);
    }
#endif /* BCM_TRIUMPH2_SUPPORT */

#ifdef BCM_ENDURO_SUPPORT
    if (SOC_IS_ENDURO(unit)) {
        return _soc_enduro_mem_parity_control(unit, mem, copyno, enable);
    }
#endif /* BCM_ENDURO_SUPPORT */
#ifdef BCM_HURRICANE1_SUPPORT
    if (SOC_IS_HURRICANE(unit)) {
        return _soc_hurricane_mem_parity_control(unit, mem, copyno, enable);
    }
#endif /* BCM_HURRICANE_SUPPORT */
#ifdef BCM_HURRICANE2_SUPPORT
    if (SOC_IS_HURRICANE2(unit)) {
        return _soc_hurricane2_mem_parity_control(unit, mem, copyno, enable);
    }
#endif /* BCM_HURRICANE2_SUPPORT */
#ifdef BCM_HURRICANE3_SUPPORT
    if (SOC_IS_HURRICANE3(unit)) {
        return _soc_hr3_mem_parity_control(unit, mem, copyno, enable);
    }
#endif /* BCM_HURRICANE3_SUPPORT */
#ifdef BCM_GREYHOUND2_SUPPORT
    if (SOC_IS_GREYHOUND2(unit)) {
#if defined(BCM_FIRELIGHT_SUPPORT)
        if (soc_feature(unit, soc_feature_fl)) {
            return _soc_fl_mem_parity_control(unit, mem, copyno, enable);
        } else
#endif
        {
            return _soc_gh2_mem_parity_control(unit, mem, copyno, enable);
        }
    }
#endif /* BCM_GREYHOUND2_SUPPORT */
#ifdef BCM_GREYHOUND_SUPPORT
    if (SOC_IS_GREYHOUND(unit)) {
        return _soc_greyhound_mem_parity_control(unit, mem, copyno, enable);
    }
#endif /* BCM_GREYHOUND_SUPPORT */

#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANA(unit)) {
        return _soc_katana_mem_parity_control(unit, mem, copyno, enable);
    }
#endif /* BCM_KATANA_SUPPORT */
#ifdef BCM_SABER2_SUPPORT
    if (SOC_IS_SABER2(unit)) {
        return soc_saber2_mem_parity_control(unit, mem, copyno, enable);
    }
#endif /* BCM_SABER2_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        return _soc_katana2_mem_parity_control(unit, mem, copyno, enable);
    }
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FB_FX_HX(unit)) {
        return _soc_fb_mem_parity_control(unit, mem, copyno, enable);
    }
#endif /* BCM_FIREBOLT_SUPPORT */

#ifdef BCM_BRADLEY_SUPPORT
    if (SOC_IS_HBX(unit)) {
        return _soc_hb_mem_parity_control(unit, mem, copyno, enable);
    }
#endif /* BCM_BRADLEY_SUPPORT */

#ifdef    BCM_HERCULES15_SUPPORT
    if (SOC_IS_HERCULES15(unit)) {
        int     copy;
        int     port;
        uint32  interrupt = 0;
        uint32  regval;

        /* Shut off parity interrupts during memory test */
        switch (mem) {
            case MEM_XQm:
                soc_reg_field_set
                (unit, MMU_INTCTRLr, &interrupt, XQ_PE_ENf, 1);
                break;
            case MEM_PPm:
                soc_reg_field_set
                (unit, MMU_INTCTRLr, &interrupt, PP_DBE_ENf, 1);
                soc_reg_field_set
                (unit, MMU_INTCTRLr, &interrupt, PP_SBE_ENf, 1);
                break;
            case MEM_LLAm:
                soc_reg_field_set
                (unit, MMU_INTCTRLr, &interrupt, LLA_PE_ENf, 1);
                break;
            case MEM_INGBUFm:
            case MEM_MCm:
            case MEM_IPMCm:
            case MEM_UCm:
            case MEM_VIDm:
            case MEM_ING_SRCMODBLKm:
            case MEM_ING_MODMAPm:
            case MEM_EGR_MODMAPm:
                soc_reg_field_set
                (unit, MMU_INTCTRLr, &interrupt, ING_PE_ENf, 1);
                break;
                break;
            default:
                return SOC_E_NONE;    /* nothing to do */
        }
        SOC_MEM_BLOCK_ITER(unit, mem, copy) {
            if (copyno != COPYNO_ALL && copyno != copy) {
                continue;
            }

            /* find the ports matching this copy/blk */
            PBMP_ITER(SOC_BLOCK_BITMAP(unit, copy), port) {
                SOC_IF_ERROR_RETURN
                (READ_MMU_INTCTRLr(unit, port, &regval));
                if (enable) {
                    regval |= interrupt;
                } else {
                    regval &= ~interrupt;
                }
                if (enable) {
                    SOC_IF_ERROR_RETURN
                    (WRITE_MMU_INTCLRr(unit, port, interrupt));
                }
                SOC_IF_ERROR_RETURN
                (WRITE_MMU_INTCTRLr(unit, port, (port == 0) ? 0:regval));
                SOC_IF_ERROR_RETURN(READ_MMU_CFGr(unit, port, &regval));
                soc_reg_field_set(unit, MMU_CFGr, &regval,
                                PARITY_DIAGf, enable ? 0 : 1);
                SOC_IF_ERROR_RETURN(WRITE_MMU_CFGr(unit, port, regval));
            }
        }
    }
#endif    /* BCM_HERCULES15_SUPPORT */

    return SOC_E_NONE;
}

/*
 * Clean up parity bits after memtest to avoid later parity errors
 */
int
soc_mem_parity_clean(int unit, soc_mem_t mem, int copyno)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(mem);
    COMPILER_REFERENCE(copyno);
    /* To avoid later parity errors */
#if defined(BCM_FIREBOLT_SUPPORT)
    switch (mem) {
        case L2Xm:
        case L2_ENTRY_ONLYm:
        if (soc_feature(unit, soc_feature_l2x_parity)) {
            if (soc_mem_clear(unit, mem, copyno, TRUE) < 0) {
                return -1;
            }
        }
        break;

        case L3_ENTRY_IPV4_MULTICASTm:
        case L3_ENTRY_IPV4_UNICASTm:
        case L3_ENTRY_IPV6_MULTICASTm:
        case L3_ENTRY_IPV6_UNICASTm:
        case L3_ENTRY_ONLYm:
        case L3_ENTRY_VALID_ONLYm:
        if (soc_feature(unit, soc_feature_l3x_parity)) {
            if (soc_mem_clear(unit, mem, copyno, TRUE) < 0) {
                return -1;
            }
        }
        break;

        case L3_DEFIPm:
        case L3_DEFIP_DATA_ONLYm:
        if (soc_feature(unit, soc_feature_l3defip_parity)) {
            if (soc_mem_clear(unit, mem, copyno, TRUE) < 0) {
                return -1;
            }
        }
        break;

#ifdef BCM_RAPTOR_SUPPORT
        case DSCP_TABLEm:
        case EGR_L3_INTFm:
        case EGR_MASKm:
        case EGR_L3_NEXT_HOPm:
        case EGR_VLANm:
        case FP_POLICY_TABLEm:
        case ING_L3_NEXT_HOPm:
        case L2MCm:
        case L3_IPMCm:
        case VLAN_TABm:
        if (soc_feature(unit, soc_feature_ip_ep_mem_parity)) {
            if (soc_mem_clear(unit, mem, copyno, TRUE) < 0) {
                return -1;
            }
        }
        break;
#endif /* BCM_RAPTOR_SUPPORT */

#ifdef BCM_TRX_SUPPORT
        case MMU_WRED_CFG_CELLm:
        case MMU_WRED_THD_0_CELLm:
        case MMU_WRED_THD_1_CELLm:
        case MMU_WRED_PORT_CFG_CELLm:
        case MMU_WRED_PORT_THD_0_CELLm:
        case MMU_WRED_PORT_THD_1_CELLm:
#ifdef BCM_TRIUMPH_SUPPORT
        case MMU_WRED_CFG_PACKETm:
        case MMU_WRED_THD_0_PACKETm:
        case MMU_WRED_THD_1_PACKETm:
        case MMU_WRED_PORT_CFG_PACKETm:
        case MMU_WRED_PORT_THD_0_PACKETm:
        case MMU_WRED_PORT_THD_1_PACKETm:
#endif /* BCM_TRIUMPH_SUPPORT */
#ifdef BCM_ENDURO_SUPPORT
        case LMEPm:
#endif /* BCM_ENDURO_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
        case SVM_METER_TABLEm:
        case MMU_INTFO_QCN_CNM_TIMER_TBLm:
        case FP_METER_TABLEm:
        case FP_STORM_CONTROL_METERSm:
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_HURRICANE2_SUPPORT
        case MMU_IPMC_VLAN_TBLm:
        case MMU_IPMC_GROUP_TBL0m:
        case MMU_IPMC_GROUP_TBL1m:
#endif /* BCM_SURRICANE2_SUPPORT */
            if (soc_mem_clear(unit, mem, copyno, TRUE) < 0) {
                return -1;
            }
            break;
#endif /* BCM_TRX_SUPPORT */

        default:
            break; /* Do nothing */
    }
#endif
     return SOC_E_NONE;
}

/* Reenable parity after testing */
int
soc_mem_parity_restore(int unit, soc_mem_t mem, int copyno)
{
    /* 
     * Some devices need memories cleared before reenabling because
     * of background HW processes which scan the tables and throw errors
     */
    if (SOC_IS_TRX(unit)) {
        SOC_IF_ERROR_RETURN
            (soc_mem_parity_clean(unit, mem, copyno));
    }

    SOC_IF_ERROR_RETURN
        (soc_mem_parity_control(unit, mem, copyno, TRUE));

    /* 
     * Other devices need memories cleared after parity is reenabled
     * so the HW generation of parity bits is correct.
     */

    if (!SOC_IS_TRX(unit)) {
        SOC_IF_ERROR_RETURN
            (soc_mem_parity_clean(unit, mem, copyno));
    }

    return 0;
}

int
soc_mem_cpu_write_control(int unit, soc_mem_t mem, int copyno, int enable,
                          int *orig_enable)
{
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        return _soc_monterey_mem_cpu_write_control(unit, mem, copyno, enable,
                                                       orig_enable);
    }
#endif /* BCM_MONTEREY_SUPPORT */
#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        return _soc_apache_mem_cpu_write_control(unit, mem, copyno, enable,
                                                       orig_enable);
    }
#endif /* BCM_APACHE_SUPPORT */

#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            return soc_tomahawk3_mem_cpu_write_control(unit, mem, copyno, enable,
                                                      orig_enable);
        } else
#endif
#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit)) {
            return soc_trident3_mem_cpu_write_control(unit, mem, copyno, enable,
                                                      orig_enable);
        } else
#endif /* TRIDENT3_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
        if (SOC_IS_TOMAHAWKX(unit)) {
            return soc_tomahawk_mem_cpu_write_control(unit, mem, copyno, enable,
                                                      orig_enable);
        } else 
#endif /* BCM_TOMAHAWK_SUPPORT */
        {
            return _soc_trident2_mem_cpu_write_control(unit, mem, copyno, enable,
                                                       orig_enable);
        }
    }
#endif /* BCM_TRIDENT2_SUPPORT */

    return SOC_E_NONE;
}


#if defined (SER_TR_TEST_SUPPORT)

#define _SER_TEST_MEM_SOC_FIELD_NAME(unit, field) \
    ((field >= 0)? SOC_FIELD_NAME((unit), (field)) : "INVALIDf")

static soc_ser_test_functions_t *ser_test_functions[SOC_MAX_NUM_DEVICES];

#if defined(BCM_TRIDENT_SUPPORT)
#define SOC_MEM_TEST_PIPE_LOCK(unit, test_data)  \
    if ((test_data->pipe_select != NULL) && \
        (test_data->acc_type == _SOC_ACC_TYPE_PIPE_Y)) { \
        IP_ARBITER_LOCK(unit); \
        (void)test_data->pipe_select(unit, TRUE, 1);  \
        (void)test_data->pipe_select(unit, FALSE, 1); \
    }
#define SOC_MEM_TEST_PIPE_UNLOCK(unit, test_data)  \
    if ((test_data->pipe_select != NULL) && \
        (test_data->acc_type == _SOC_ACC_TYPE_PIPE_Y)) { \
        (void)test_data->pipe_select(unit, TRUE, 0);  \
        (void)test_data->pipe_select(unit, FALSE, 0); \
        IP_ARBITER_UNLOCK(unit); \
    }
#else
#define SOC_MEM_TEST_PIPE_LOCK(unit, test_data) 
#define SOC_MEM_TEST_PIPE_UNLOCK(unit, test_data) 
#endif

soc_error_t
soc_ser_test_enable_field_check(int unit, soc_reg_t reg, soc_mem_t mem, soc_field_t field,
                                int fld_position, int is_mem, int *match)
{
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 copyno;
    uint32 value = 0;
    uint32 cmpvalue = 0;
    uint32 rval32 = 0;
    uint64 rval64;
    int index = 0;
    int entry_count = 0;
#ifdef BCM_TOMAHAWK3_SUPPORT
    int is_idb = 0;
#endif

    if (is_mem) {
        if (SOC_MEM_IS_VALID(unit,mem)) {
            entry_count = soc_mem_index_count(unit, mem);
            if (entry_count > 1) {
                return SOC_E_PARAM;
            }
            index = SOC_MEM_INFO(unit, mem).index_min;
            /* coverity[unsigned_compare] */
            SOC_MEM_BLOCK_ITER(unit, mem, copyno) {
                SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, copyno, index, entry));
                value = soc_mem_field32_get(unit, mem, entry, field);
                if (fld_position == -1) {
                    if (0 == value) {
                        *match = 0;
                        LOG_WARN(BSL_LS_SOC_SER,
                                 (BSL_META_U(unit,
                                          "parity control %s.%s is disabled !!\n"),
                                          SOC_MEM_NAME(unit, mem),
                                          SOC_FIELD_NAME(unit, field)));
                    } else {
                        *match = 1;
                    }
                } else {
                    if (0 == (value & (1 << fld_position))) {
                        *match = 0;
                        LOG_WARN(BSL_LS_SOC_SER,
                                 (BSL_META_U(unit,
                                          "parity control %s.%s[%d] is disabled !!\n"),
                                          SOC_MEM_NAME(unit, mem),
                                          SOC_FIELD_NAME(unit, field),
                                          fld_position));
                    } else {
                        *match = 1;
                    }
                }
            }
        }
    } else {
        if (SOC_REG_IS_VALID(unit,reg)) {
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                (void)soc_th3_idb_enable_field_check(unit, reg, field,
                                                     &is_idb, match);
                if (is_idb) {
                    return SOC_E_NONE;
                }
            }
#endif
            if (SOC_REG_IS_64(unit,reg)) {
                COMPILER_64_ZERO(rval64);
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, REG_PORT_ANY, 0, &rval64));
                value = soc_reg64_field32_get(unit, reg, rval64, field);
            } else {
                SOC_IF_ERROR_RETURN
                    (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval32));
                value = soc_reg_field_get(unit, reg, rval32, field);
            }
#if defined(INCLUDE_XFLOW_MACSEC) && defined(BCM_HURRICANE4_SUPPORT)
            if (SOC_IS_HURRICANE4(unit) &&
                soc_feature(unit, soc_feature_xflow_macsec)) {
                switch (reg) {
                    case ECC_CTLSTS_EMIB1r:
                    case ECC_CTLSTS_EMIB2r:
                    case ECC_CTLSTS_ECNTRr:
                    case ECC_CTLSTS_OMGMTPKTr:
                    case ECC_CTLSTS_ESATBL0r:
                    case ECC_CTLSTS_ESATBL1r:
                    case ECC_CTLSTS_ESATBL2r:
                    case ECC_CTLSTS_ESATBL3r:
                    case ECC_CTLSTS_ESCTBLr:
                    case ECC_CTLSTS_IMGMTPKTr:
                    case ECC_CTLSTS_IMIB1r:
                    case ECC_CTLSTS_IMIB1Br:
                    case ECC_CTLSTS_IMIB2r:
                    case ECC_CTLSTS_IMIB3r:
                    case ECC_CTLSTS_ICNTRr:
                    case ECC_CTLSTS_ISATBLr:
                    case ECC_CTLSTS_SCTCAMHITr:
                    case ECC_CTLSTS_ISCTBLr:
                    case ECC_CTLSTS_SPTCAMHITr:
                    case ECC_CTLSTS_ISPLUTBLr:
                    case ECC_CTLSTS_ISPPLTBLr:
                        cmpvalue = 1;
                        break;
                    default:
                        break;
                }
            }
#endif
            if (fld_position == -1) {
                if (cmpvalue == value) {
                    *match = 0;
                    LOG_WARN(BSL_LS_SOC_SER,
                             (BSL_META_U(unit,
                                      "parity control %s.%s is disabled !!\n"),
                                      SOC_REG_NAME(unit, reg),
                                      SOC_FIELD_NAME(unit, field)));
                } else {
                    *match = 1;
                }
            } else {
                if (cmpvalue == ((value >> fld_position) & 1)) {
                    *match = 0;
                    LOG_WARN(BSL_LS_SOC_SER,
                             (BSL_META_U(unit,
                                      "parity control %s.%s[%d] is disabled !!\n"),
                                      SOC_REG_NAME(unit, reg),
                                      SOC_FIELD_NAME(unit, field),
                                      fld_position));
                } else {
                    *match = 1;
                }
            }
        }
    }

    return SOC_E_NONE;
}

soc_error_t
soc_ser_test_reg_parity_control_check(int unit, _soc_reg_ser_en_info_t *info)
{
    int i;
    int match = 0;
    int en_fld_position = -1;
    int ecc1b_en_fld_position = -1;
    soc_mem_t mem = INVALIDm;
    soc_mem_t ecc1b_mem = INVALIDm;
    soc_reg_t reg = INVALIDr;
    soc_reg_t ecc1b_reg = INVALIDr;
    soc_field_t field = INVALIDf;
    soc_field_t ecc1b_field = INVALIDf;
    uint8 flag = 0;
    uint8 ecc1b_flag = 0;
    uint32 total_count = 0;
    uint32 pass_count = 0;
    int rv = SOC_E_NONE;

    for (i = 0; ; i++) {
        if (info[i].reg == INVALIDr) {
            break;
        }
        flag = info[i].en_ctrl.flag_reg_mem;
        if (flag) {
            mem = info[i].en_ctrl.ctrl_type.en_mem;
            reg = INVALIDr;
        } else {
            reg = info[i].en_ctrl.ctrl_type.en_reg;
            mem = INVALIDm;
        }

#ifdef BCM_HURRICANE4_SUPPORT
        if (_soc_hr4_reg_skip(unit, reg) || _soc_hr4_mem_skip(unit, mem)) {
            continue;
        }
#endif
#ifdef BCM_FIREBOLT6_SUPPORT
        if (_soc_fb6_reg_skip(unit, reg) || _soc_fb6_mem_skip(unit, mem)) {
            continue;
        }
#endif

        field = info[i].en_ctrl.en_fld;
        en_fld_position = info[i].en_ctrl.en_fld_position;
        ecc1b_flag = info[i].ecc1b_ctrl.flag_reg_mem;
        if (ecc1b_flag) {
            ecc1b_mem = info[i].ecc1b_ctrl.ctrl_type.en_mem;
            ecc1b_reg = INVALIDr;
        } else {
            ecc1b_reg = info[i].ecc1b_ctrl.ctrl_type.en_reg;
            ecc1b_mem = INVALIDm;
        }
        ecc1b_field = info[i].ecc1b_ctrl.en_fld;
        ecc1b_en_fld_position = info[i].ecc1b_ctrl.en_fld_position;

        rv = soc_ser_test_enable_field_check(unit, reg, mem, field, en_fld_position,
                                             flag, &match);
        if (rv != SOC_E_NONE) {
            if ((rv == SOC_E_PARAM) && (mem != INVALIDm)) {
                LOG_ERROR(BSL_LS_SOC_SER,
                         (BSL_META_U(unit,
                                     "parity control memory(%s) is not support !!\n"),
                                     SOC_MEM_NAME(unit, mem)));
            } else {
                LOG_ERROR(BSL_LS_SOC_SER,
                         (BSL_META_U(unit,
                                     "Get parity control data failed !!\n")));
            }
        }
        total_count++;
        if (match) {
            pass_count++;
        }
        rv = soc_ser_test_enable_field_check(unit, ecc1b_reg, ecc1b_mem, ecc1b_field,
                                              ecc1b_en_fld_position, ecc1b_flag, &match);
        total_count++;
        if (match) {
            pass_count++;
        }
        if (rv != SOC_E_NONE) {
            if ((rv == SOC_E_PARAM) && (ecc1b_mem != INVALIDm)) {
                LOG_ERROR(BSL_LS_SOC_SER,
                         (BSL_META_U(unit,
                                     "parity control memory(%s) is not support !!\n"),
                                     SOC_MEM_NAME(unit, ecc1b_mem)));
            } else {
                LOG_ERROR(BSL_LS_SOC_SER,
                         (BSL_META_U(unit,
                                     "Get parity control data failed !!\n")));
            }
        }
    }

    LOG_CLI((BSL_META_U(unit, "\nRegisters parity control checked on unit %d: %d\n"),
             unit, total_count));
    LOG_CLI((BSL_META_U(unit, "Passed fields:\t%d\n"), pass_count));
    LOG_CLI((BSL_META_U(unit, "Failed fields::\t%d\n\n"), (total_count - pass_count)));

    return SOC_E_NONE;
}

soc_error_t
soc_ser_test_mem_parity_control_check(int unit, _soc_mem_ser_en_info_t *info)
{
    int i;
    int match = 0;
    int en_fld_position = -1;
    int ecc1b_en_fld_position = -1;
    soc_mem_t mem = INVALIDm;
    soc_mem_t ecc1b_mem = INVALIDm;
    soc_reg_t reg = INVALIDr;
    soc_reg_t ecc1b_reg = INVALIDr;
    soc_field_t field = INVALIDf;
    soc_field_t ecc1b_field = INVALIDf;
    uint8 flag = 0;
    uint8 ecc1b_flag = 0;
    uint32 check_ecc1b = 1;
    uint32 total_count = 0;
    uint32 pass_count = 0;
    int rv = SOC_E_NONE;

    for (i = 0; ; i++) {
        if (info[i].mem == INVALIDm) {
            break;
        }
#ifdef BCM_HURRICANE4_SUPPORT
        if (_soc_hr4_mem_skip(unit, info[i].mem)) {
            continue;
        }
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_FIREBOLT6_SUPPORT
        if (_soc_fb6_mem_skip(unit, info[i].mem)) {
            continue;
        }
#endif /* BCM_FIREBOLT6_SUPPORT */

        flag = info[i].en_ctrl.flag_reg_mem;
        if (flag) {
            mem = info[i].en_ctrl.ctrl_type.en_mem;
            reg = INVALIDr;
        } else {
            reg = info[i].en_ctrl.ctrl_type.en_reg;
            mem = INVALIDm;
        }
        field = info[i].en_ctrl.en_fld;
        en_fld_position = info[i].en_ctrl.en_fld_position;
        ecc1b_flag = info[i].ecc1b_ctrl.flag_reg_mem;
        if (ecc1b_flag) {
            ecc1b_mem = info[i].ecc1b_ctrl.ctrl_type.en_mem;
            ecc1b_reg = INVALIDr;
        } else {
            ecc1b_reg = info[i].ecc1b_ctrl.ctrl_type.en_reg;
            ecc1b_mem = INVALIDm;
        }
        ecc1b_field = info[i].ecc1b_ctrl.en_fld;
        ecc1b_en_fld_position = info[i].ecc1b_ctrl.en_fld_position;

        if(!SOC_MEM_SER_CORRECTION_TYPE(unit, info[i].mem)) {
            check_ecc1b = 0;
        } else {
            check_ecc1b = 1;
        }

        rv = soc_ser_test_enable_field_check(unit, reg, mem, field, en_fld_position,
                                             flag, &match);
        total_count++;
        if (match) {
            pass_count++;
        }
        if (rv != SOC_E_NONE) {
            if ((rv == SOC_E_PARAM) && (mem != INVALIDm)) {
                LOG_ERROR(BSL_LS_SOC_SER,
                         (BSL_META_U(unit,
                                     "parity control memory(%s) is not support !!\n"),
                                     SOC_MEM_NAME(unit, mem)));
            } else {
                LOG_ERROR(BSL_LS_SOC_SER,
                         (BSL_META_U(unit,
                                     "Get parity control data failed !!\n")));
            }
        }
        if (check_ecc1b) {
            rv = soc_ser_test_enable_field_check(unit, ecc1b_reg, ecc1b_mem, ecc1b_field,
                                                  ecc1b_en_fld_position, ecc1b_flag, &match);
            total_count++;
            if (match) {
                pass_count++;
            }
            if (rv != SOC_E_NONE) {
                if ((rv == SOC_E_PARAM) && (ecc1b_mem != INVALIDm)) {
                    LOG_ERROR(BSL_LS_SOC_SER,
                             (BSL_META_U(unit,
                                         "parity control memory(%s) is not support !!\n"),
                                         SOC_MEM_NAME(unit, ecc1b_mem)));
                } else {
                    LOG_ERROR(BSL_LS_SOC_SER,
                             (BSL_META_U(unit,
                                         "Get parity control data failed !!\n")));
                }
            }
        }
    }
    LOG_CLI((BSL_META_U(unit, "\nMemories parity control checked on unit %d: %d\n"),
             unit, total_count));
    LOG_CLI((BSL_META_U(unit, "Passed fields:\t%d\n"), pass_count));
    LOG_CLI((BSL_META_U(unit, "Failed fields::\t%d\n\n"), (total_count - pass_count)));

    return SOC_E_NONE;
}

soc_error_t
soc_ser_test_bus_parity_control_check(int unit, _soc_bus_ser_en_info_t *info)
{
    int i;
    int match = 0;
    soc_reg_t reg = INVALIDr;
    soc_field_t field = INVALIDf;
    uint32 total_count = 0;
    uint32 pass_count = 0;
    int rv = SOC_E_NONE;

    for (i = 0; ; i++) {
        if (info[i].en_reg == INVALIDr) {
            break;
        }
        reg = info[i].en_reg;
        field = info[i].en_fld;

#ifdef BCM_HURRICANE4_SUPPORT
        if (_soc_hr4_reg_skip(unit, reg)) {
            continue;
        }
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_FIREBOLT6_SUPPORT
        if (_soc_fb6_reg_skip(unit, reg)) {
            continue;
        }
#endif

        rv = soc_ser_test_enable_field_check(unit, reg, INVALIDm, field, -1, 0, &match);
        total_count++;
        if (match) {
            pass_count++;
        }
        if (rv != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_SER,
                     (BSL_META_U(unit,
                                 "Get parity control data failed !!\n")));
        }
    }
    LOG_CLI((BSL_META_U(unit, "\nBus parity control checked on unit %d: %d\n"),
             unit, total_count));
    LOG_CLI((BSL_META_U(unit, "Passed fields:\t%d\n"), pass_count));
    LOG_CLI((BSL_META_U(unit, "Failed fields::\t%d\n\n"), (total_count - pass_count)));

    return SOC_E_NONE;
}

soc_error_t
soc_ser_test_buf_parity_control_check(int unit, _soc_buffer_ser_en_info_t *info)
{
    int i;
    int match = 0;
    soc_reg_t reg = INVALIDr;
    soc_field_t field = INVALIDf;
    uint32 total_count = 0;
    uint32 pass_count = 0;
    int rv = SOC_E_NONE;

    for (i = 0; ; i++) {
        if (info[i].en_reg == INVALIDr) {
            break;
        }
#ifdef BCM_HURRICANE4_SUPPORT
        if (_soc_hr4_reg_skip(unit, info[i].en_reg)) {
            continue;
        }
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_FIREBOLT6_SUPPORT
        if (_soc_fb6_reg_skip(unit, info[i].en_reg)) {
            continue;
        }
#endif /* BCM_FIREBOLT6_SUPPORT */
        reg = info[i].en_reg;
        field = info[i].en_fld;

        rv = soc_ser_test_enable_field_check(unit, reg, INVALIDm, field, -1, 0, &match);
        total_count++;
        if (match) {
            pass_count++;
        }
        if (rv != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_SER,
                     (BSL_META_U(unit,
                                 "Get parity control data failed !!\n")));
        }
    }
    LOG_CLI((BSL_META_U(unit, "\nBuffer parity control checked on unit %d: %d\n"),
             unit, total_count));
    LOG_CLI((BSL_META_U(unit, "Passed fields:\t%d\n"), pass_count));
    LOG_CLI((BSL_META_U(unit, "Failed fields::\t%d\n\n"), (total_count - pass_count)));

    return SOC_E_NONE;
    }

soc_error_t
soc_ser_test_parity_control_check(int unit, int type, void *info)
{
    switch(type) {
        case _SOC_SER_TYPE_REG:
            soc_ser_test_reg_parity_control_check(unit, (_soc_reg_ser_en_info_t *)info);
            break;
        case _SOC_SER_TYPE_MEM:
             soc_ser_test_mem_parity_control_check(unit, (_soc_mem_ser_en_info_t *)info);
           break;
        case _SOC_SER_TYPE_BUS:
            soc_ser_test_bus_parity_control_check(unit, (_soc_bus_ser_en_info_t *)info);
            break;
        case _SOC_SER_TYPE_BUF:
            soc_ser_test_buf_parity_control_check(unit, (_soc_buffer_ser_en_info_t *)info);
            break;
        default:
            break;
    }

    return SOC_E_NONE;
}

#if defined (BCM_FIRELIGHT_SUPPORT) && defined(INCLUDE_XFLOW_MACSEC)
STATIC int
ser_xflow_macsec_write_sp_tcam (int unit, uint32 flags,
                                ser_test_data_t *test_data)
{
    soc_mem_t mem0, mem1;
    uint32 *xy_entry_0 = NULL, *xy_entry_1 = NULL;
    uint32 *entry_data_0 = NULL, *entry_data_1 = NULL;
    int alloc_size = SOC_MAX_MEM_WORDS * sizeof(uint32);
    int rv;

    mem0 = ISEC_SP_TCAM_KEYm;
    mem1 = ISEC_SP_TCAM_MASKm;

    if (!SOC_IS_FIRELIGHT(unit)) {
        rv = SOC_E_INTERNAL;
        goto exit;
    }
    xy_entry_0 = sal_alloc(alloc_size, "SP TCAM xy_entry_0 key");
    if (xy_entry_0 == NULL) {
        rv = SOC_E_MEMORY;
        goto exit;
    }
    xy_entry_1 = sal_alloc(alloc_size, "SP TCAM xy_entry_1 mask");
    if (xy_entry_1 == NULL) {
        rv = SOC_E_MEMORY;
        goto exit;
    }
    entry_data_0 = sal_alloc(alloc_size, "SP TCAM data entry_data0");
    if (entry_data_0 == NULL) {
        rv = SOC_E_MEMORY;
        goto exit;
    }
    entry_data_1 = sal_alloc(alloc_size, "SP TCAM mask entry_data1");
    if (entry_data_1 == NULL) {
        rv = SOC_E_MEMORY;
        goto exit;
    }
    sal_memcpy(entry_data_0, test_data->entry_buf, alloc_size);
    sal_memcpy(entry_data_1, test_data->entry_buf, alloc_size);
    _soc_mem_tcam_dm_to_xy_coupled(unit, mem0, mem1,
                                    entry_data_0, entry_data_1,
                                    xy_entry_0, xy_entry_1,
                                    1);
    rv = soc_mem_write_extended(unit, flags, mem0,
                                test_data->mem_block, test_data->index,
                                xy_entry_0);
    if (SOC_FAILURE(rv)) {
        goto exit;
    }

    rv = soc_mem_write_extended(unit, flags, mem1,
                                test_data->mem_block, test_data->index,
                                xy_entry_1);
exit:
    if (xy_entry_0) {
        sal_free(xy_entry_0);
    }
    if (xy_entry_1) {
        sal_free(xy_entry_1);
    }
    if (entry_data_0) {
        sal_free(entry_data_0);
    }
    if (entry_data_1) {
        sal_free(entry_data_1);
    }

    return rv;
}
#endif

/*
 * Function:
 *      ser_test_mem_write
 * Purpose:
 *      A small helper function for ser_test_mem, combining common operations required to write
 * Parameters:
 *      unit        - (IN) Device Number
 *      test_data   - (IN) contains the current state of the test.
 * Returns:
 *      An error if one is generated by the soc_mem_write, otherwise SOC_E_NONE
 */
soc_error_t
ser_test_mem_write(int unit, uint32 flags, ser_test_data_t *test_data)
{
    soc_error_t rv = SOC_E_NONE;
    uint32 write_flags = SOC_MEM_NO_FLAGS;
    soc_mem_field_set(unit, test_data->mem, test_data->entry_buf,
                      test_data->test_field, test_data->field_buf);
    SOC_MEM_TEST_PIPE_LOCK(unit, test_data);
    if (flags & SOC_INJECT_ERROR_DONT_MAP_INDEX) {
        write_flags = SOC_MEM_DONT_MAP_INDEX;
    }
#if defined (BCM_FIRELIGHT_SUPPORT) && defined(INCLUDE_XFLOW_MACSEC)
    if (test_data->mem == ISEC_SP_TCAM_KEYm ||
        test_data->mem == ISEC_SP_TCAM_MASKm) {
        rv = ser_xflow_macsec_write_sp_tcam(unit, write_flags, test_data);
    } else
#endif
    if (test_data->acc_type != _SOC_ACC_TYPE_PIPE_ANY &&
        test_data->acc_type != _SOC_ACC_TYPE_PIPE_ALL) {
        rv = soc_mem_pipe_select_write(unit, write_flags,
                      test_data->mem, test_data->mem_block, test_data->acc_type,
                      test_data->index, test_data->entry_buf);
    } else {
        rv = soc_mem_write_extended(unit, write_flags, test_data->mem,
                                    test_data->mem_block, test_data->index,
                                    test_data->entry_buf);
    }
    SOC_MEM_TEST_PIPE_UNLOCK(unit, test_data);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SER,
                  (BSL_META_U(unit,
                              "unit %d %s entry %d mem write error\n"),
                   unit, test_data->mem_name, test_data->index));
    }
    return rv;
}


/*
 * Function:
 *      ser_test_mem_read
 * Purpose:
 *      A small helper function for ser_test_mem, combining common operations required to read
 * Parameters:
 *      unit        - (IN) Device Number
 *      read_func_view
 *                  - (IN) 0 - read test_data->mem //error injection view
 *                         1 - read test_data->mem_fv //func view to induce err
 *      test_data   - (IN & OUT) contains the current state of the test.
 *                    The members entry_buf and field_buf will be modified by this function
 * Returns:
 *      An error if one is generated by the soc_mem_read, otherwise SOC_E_NONE
 */
soc_error_t
ser_test_mem_read(int unit, uint32 read_flags, ser_test_data_t *test_data)
{
    soc_error_t rv = SOC_E_NONE;
    soc_mem_t mem;
    int index;
    int use_pipe_select_read = 0;
    uint32 read_func_view = read_flags & SER_TEST_MEM_F_READ_FUNC_VIEW;
    uint32 flags = SOC_MEM_NO_FLAGS;
#ifdef INCLUDE_TCL
    /* If TCL is defined, the cache coherency flag will cause parity information
       to be DISCARDED on read. Turning this off here */
    int coherency_check = SOC_MEM_CACHE_COHERENCY_CHECK(unit);
    SOC_MEM_CACHE_COHERENCY_CHECK_SET(unit, FALSE);
#endif

    if (read_func_view) {
        mem = test_data->mem_fv;
        index = test_data->index_fv;
    } else {
        mem = test_data->mem;
        index = test_data->index;
    }

    if (soc_feature(unit, soc_feature_unique_acc_type_access)) {
       if ((test_data->acc_type >= 0) &&
           (test_data->acc_type < NUM_PIPE(unit))) {
           use_pipe_select_read = 1;
       }
    } else if (test_data->acc_type == _SOC_ACC_TYPE_PIPE_Y) {
        use_pipe_select_read = 1;
    }
    if (read_flags & SER_TEST_MEM_F_DONT_MAP_INDEX) {
        flags = SOC_MEM_DONT_MAP_INDEX;
    }
    SOC_MEM_TEST_PIPE_LOCK(unit, test_data);
    if (use_pipe_select_read) {
        rv = soc_mem_pipe_select_read(unit, flags, mem,
                                      test_data->mem_block,
                                      test_data->acc_type, index,
                                      test_data->entry_buf);
    } else {
        /*Enable NACK on read */
        flags |= SOC_MEM_SCHAN_ERR_RETURN | SOC_MEM_DONT_USE_CACHE;
        rv = soc_mem_read_extended(unit, flags,
                                   mem, 0, test_data->mem_block,
                                   index, test_data->entry_buf);
    }
    SOC_MEM_TEST_PIPE_UNLOCK(unit, test_data);
#ifdef INCLUDE_TCL
    SOC_MEM_CACHE_COHERENCY_CHECK_SET(unit, coherency_check);
#endif
    if (SOC_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LS_SOC_SER,
                  (BSL_META_U(unit,
                              "unit %d NACK received for %s entry %d:\n\t"),
                   unit, SOC_MEM_NAME(unit, mem), index));
    }

    /* For case:
     *   (read_func_view && (inject_view != func_view)
     * we MUST NOT update test_data->field_buf because
     * test_data->test_field may be ECCf, ECC_0f, etc which is NOT
     * valid for func_view.
     */
    if (!read_func_view || (test_data->mem == test_data->mem_fv)) {
        soc_mem_field_get(unit, mem, test_data->entry_buf,
                          test_data->test_field, test_data->field_buf);
    }
    return rv;
}


soc_error_t
soc_ser_test_inject_full(int unit, uint32 flags, ser_test_data_t *test_data)
{
    soc_field_t key_field, mask_field;
    uint32 write_flags = flags & SOC_INJECT_ERROR_DONT_MAP_INDEX;

    if ((flags & SOC_INJECT_ERROR_TCAM_FLAG) && test_data->tcam_parity_bit >= 0) {
        if (SOC_MEM_FIELD_VALID(unit, test_data->mem, KEYf)){
            key_field = KEYf;
            mask_field = MASKf;
        } else if (SOC_MEM_FIELD_VALID(unit, test_data->mem, KEY0f)){
            key_field = KEY0f;
            mask_field = MASK0f;
        } else if (SOC_MEM_FIELD_VALID(unit, test_data->mem, KEY1f)){
            key_field = KEY1f;
            mask_field = MASK1f;
        } else {
            return SOC_E_FAIL;
        }
        test_data->test_field = key_field;
        SOC_IF_ERROR_RETURN(ser_test_mem_read(unit, 0, test_data));
        SOC_IF_ERROR_RETURN(soc_ser_test_inject_error(unit, test_data, 0));
        test_data->test_field = mask_field;
        SOC_IF_ERROR_RETURN(ser_test_mem_read(unit, 0, test_data));
        if (test_data->field_buf[0] == 0) {
            return soc_ser_test_inject_error(unit, test_data, 0);
        } else {
            return SOC_E_NONE;
        }

    } else if (flags & SOC_INJECT_ERROR_2BIT_ECC){
        return soc_ser_test_inject_error(unit, test_data, 
                                         write_flags | SOC_INJECT_ERROR_2BIT_ECC);
    } else {
        return soc_ser_test_inject_error(unit, test_data, write_flags);
    }
}

soc_error_t
soc_ser_test_inject_error(int unit, ser_test_data_t *test_data, uint32 flags)
{
    
    if((test_data->field_buf[0] & 1) == 1) {
        test_data->field_buf[0] &= 0xFFFFFFFE;
    } else {
        test_data->field_buf[0] |= 0x00000001;
    }

    if (flags & SOC_INJECT_ERROR_2BIT_ECC) {
        if((test_data->field_buf[0] & 2) == 2) {
            test_data->field_buf[0] &= 0xFFFFFFFD;
        } else {
            test_data->field_buf[0] |= 0x00000002;
        }
    }

    test_data->badData = test_data->field_buf[0];
    /*Disable writing to cache for fudged data.*/
    SOC_MEM_TEST_SKIP_CACHE_SET(unit, TRUE);
    SOC_IF_ERROR_RETURN(ser_test_mem_write(unit, flags, test_data));
    SOC_MEM_TEST_SKIP_CACHE_SET(unit, FALSE);
    return SOC_E_NONE;
}

/*
 * Function:
 *      ser_test_mem
 * Purpose:
 *      Serform a Software Error Recovery test on passed memory mem.
 * Parameters:
 *      unit                 - (IN) Device Number
 *      parity_enable_reg    - (IN) Register which turns on and off memory protection
 *      tcam_parity_bit      - (IN) Bit in the parity_enable_reg field for TCAM parity for this memory.
 *                             This must be -1 if this memory is not protected by the TCAM
 *      hw_parity_field      - (IN) If controlled by H/W, this is the field in the enable register that
 *                             coorisponds to this memory.
 *        mem                - (IN) The memory on which to test SER
 *      test_type            - (IN) Used to determine how many indexes to test for each memory table.
 *      mem_block            - (IN) The block for this memory.  MEM_BLOCK_ANY is fine for TCAM mems
 *      port                 - (IN) The port for this memory/control reg.  REG_PORT_ANY is fine for TCAMS
 *      error_count          - (OUT)Increments if a test on any of the indexes in this memory fail.
 * Returns:
 *      The number of failed tests (This can be more than one per memory if test_type is anything
 *      other than SER_SINGLE_INDEX.)
 */


soc_error_t
ser_test_mem_pipe(int unit, soc_reg_t parity_enable_reg, int tcam_parity_bit,
                            soc_field_t hw_parity_field, soc_mem_t mem, soc_field_t test_field,
                            _soc_ser_test_t test_type, soc_block_t mem_block,
                            soc_port_t port, soc_acc_type_t acc_type, int *error_count)
{
    uint32 tmp_entry[SOC_MAX_MEM_WORDS], field_data[SOC_MAX_REG_FIELD_WORDS];
    ser_test_data_t test_data;
    soc_ser_create_test_data(unit, tmp_entry, field_data, parity_enable_reg,
                             tcam_parity_bit, hw_parity_field, mem, test_field,
                             mem_block, port, acc_type, 0, &test_data);
    return ser_test_mem(unit, 0, &test_data, test_type, error_count);
}

int
ser_test_mem_index_remap(int unit, ser_test_data_t *test_data, int *mem_ecc)
{
    int remap_status = 0;
    int mem_has_ecc = 0;

    test_data->mem = test_data->mem_fv;
    test_data->index = test_data->index_fv;

#if defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TOMAHAWKX(unit)) {
        remap_status = ser_test_th_mem_index_remap(unit, test_data,
                                                   &mem_has_ecc);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        remap_status = ser_test_td3_mem_index_remap(unit, test_data,
                                                   &mem_has_ecc);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (SOC_IS_TD2P_TT2P(unit)) {
        /* If apache gets any diff, it will have own fn */
        remap_status = ser_test_trident2p_mem_index_remap(unit, test_data,
                                                          &mem_has_ecc);
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

#if defined(BCM_MONTEREY_SUPPORT)
    if (SOC_IS_MONTEREY(unit)) {
        remap_status = ser_test_monterey_mem_index_remap(unit, test_data,
                                                         &mem_has_ecc);
    }
#endif
#if defined(BCM_APACHE_SUPPORT)
    if (SOC_IS_APACHE(unit) && !SOC_IS_MONTEREY(unit)) { 
        remap_status = ser_test_apache_mem_index_remap(unit, test_data,
                                                     &mem_has_ecc);
    }
#endif /* BCM_APACHE_SUPPORT */
#if defined(BCM_HURRICANE3_SUPPORT)
    if (SOC_IS_HURRICANE3(unit)) {
        remap_status = ser_test_hr3_mem_index_remap(unit, test_data, &mem_has_ecc);
    }
#endif /* BCM_HURRICANE3_SUPPORT */
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit)) {
#if defined(BCM_FIRELIGHT_SUPPORT)
        if (soc_feature(unit, soc_feature_fl)) {
            remap_status = ser_test_fl_mem_index_remap(unit, test_data,
                                                       &mem_has_ecc);
        } else
#endif
        {
            remap_status = ser_test_greyhound2_mem_index_remap(unit, test_data,
                                                               &mem_has_ecc);
        }
    }
#endif /* BCM_GREYHOUND2_SUPPORT */
    if (remap_status < 0) {
        LOG_ERROR(BSL_LS_SOC_SER,
                  (BSL_META_U(unit,
                              "ser_test_mem_index_remap: "
                              "FAILED for mem %s index %d\n"),
                   SOC_MEM_NAME(unit, test_data->mem_fv),
                                test_data->index_fv));
    }
    *mem_ecc = mem_has_ecc;
    return remap_status;
}

/*
 * Function:
 *      soc_ser_create_test_data
 * Purpose:
 *      Populates and performs data verification on a ser_test_data_t structure.  This was
 *      broken out into a separate function to avoid performing this work an excessive number
 *      of times.
 * Parameters:
 *      unit                 - (IN) Device Number
 *      tmp_entry            - (IN) A pointer to a uint32 array of size SOC_MEM_MAX_WORDS
 *      fieldData            - (IN) A pointer to a uint 32 array of size [SOC_MAX_REG_FIELD_WORDS]
 *      parity_enable_reg    - (IN) Register which turns on and off memory protection
 *      tcam_parity_bit      - (IN) Bit in the parity_enable_reg field for TCAM parity
                                    for this memory. This must be INVALID_TCAM_PARITY_BIT if
                                    this memory is not a TCAM protected memory.
 *      hw_parity_field      - (IN) If controlled by H/W, this is the field in the enable
                                    register that coorisponds to this memory.
 *      mem                  - (IN) The memory ID on which to test SER
 *      mem_block            - (IN) The block for this memory.
                                    MEM_BLOCK_ANY is fine for TCAM mems
 *      port                 - (IN) The port for this memory/control reg.
                                    REG_PORT_ANY is fine for TCAMS
 *      acc_type             - (IN) Access type for this memory.
 *      index                - (IN) The entry index on which to test SER
 *      test_data            - (OUT)A pointer to a data structure cotaining SER test info.
 * Returns:
 *      SOC_E_NONE if the test passes, and error if it does
 */
void
soc_ser_create_test_data(int unit, uint32 *tmp_entry, uint32 *field_data,
                         soc_reg_t parity_enable_reg, int tcam_parity_bit,
                         soc_field_t hw_parity_field, soc_mem_t mem,
                         soc_field_t test_field, soc_block_t mem_block,
                         soc_port_t port, soc_acc_type_t acc_type,
                         int index, ser_test_data_t *test_data)
{
    uint16 field_num = 0;
    static soc_field_t field_names[] = {
        ECCf,
        ECC_0f,
        EVEN_PARITYf,
        PARITYf,
        EVEN_PARITY_0f,
        EVEN_PARITY_LOWERf,
        PARITY_0f,
        ECCPf,
        EVEN_EGR_VLAN_STG_PARITY_P0f,
        LWR_ECC0f,
        DATAf,
        DATA_0f,
        INVALIDf
    };
    int i, found = 0, field_length = 1;
	size_t len;
    int mem_has_ecc = 0;

    test_data->mem_fv = mem;
    test_data->index_fv = index;
    /* compute test_data->mem, test_data->index */
    (void)ser_test_mem_index_remap(unit, test_data, &mem_has_ecc);
    test_data->parity_enable_reg = parity_enable_reg;
    test_data->parity_enable_field = hw_parity_field;
    test_data->tcam_parity_bit = tcam_parity_bit;
    test_data->mem_block = mem_block;
    test_data->port = port;
    test_data->mem_info = &SOC_MEM_INFO(unit, test_data->mem);
    test_data->test_field = test_field;

    /* An INVALIDf, 0 passed in for test_field implies we should select
     * any valid field. */
    if ((test_field == INVALIDf) || (test_field == 0) ||
        !SOC_MEM_FIELD_VALID(unit, test_data->mem, test_field)
       ) {
        /* select from preferred list */
        for (i = 0; (field_names[i] != INVALIDf); i++) {
            if (SOC_MEM_FIELD_VALID(unit, test_data->mem, field_names[i])) {
                test_data->test_field = field_names[i];
                found = 1;
                break;
            }
        }
        /* select first multi-bit field or last 1-bit field */
        if (!found && (test_data->mem_info != NULL)) {
           field_num = test_data->mem_info->nFields;
           for (i = 0; i < field_num; i++) {
               test_data->test_field = test_data->mem_info->fields[i].field;
               field_length = soc_mem_field_length(unit, test_data->mem,
                                                   test_data->test_field);
               if (field_length > 1) {
                   found = 1;
                   break;
               }
           }
        }
    }
    LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit,
                        "unit %d, soc_ser_create_test_data: error_inject_view: mem %s, test_field %s\n"),
             unit, SOC_MEM_NAME(unit, test_data->mem),
             _SER_TEST_MEM_SOC_FIELD_NAME(unit, test_data->test_field)));

    test_data->acc_type = acc_type;
    test_data->entry_buf = tmp_entry;
    test_data->field_buf = field_data;
#ifdef SOC_MEM_NAME
    /* coverity[secure_coding] */
    len = sal_strlen(SOC_MEM_NAME(unit, test_data->mem));
    if(sizeof(test_data->mem_name)- 1 < len)
            len = sizeof(test_data->mem_name)- 1;
    sal_strncpy(test_data->mem_name, SOC_MEM_NAME(unit, test_data->mem), len);
    test_data->mem_name[len] = 0;
    /* coverity[secure_coding] */
    len = sal_strlen(_SER_TEST_MEM_SOC_FIELD_NAME(unit, test_data->test_field));
    if(sizeof(test_data->field_name)- 1 < len)
      len = sizeof(test_data->field_name)- 1;
    sal_strncpy(test_data->field_name,
               _SER_TEST_MEM_SOC_FIELD_NAME(unit, test_data->test_field), len);
    test_data->field_name[len] = 0;
#else
    sprintf(test_data->mem_name, "Mem ID: %d", test_data->mem);
    sprintf(test_data->field_name, "Field ID: %d", test_data->test_field);
#endif
    test_data->badData = 0;
#ifdef BCM_TRIDENT_SUPPORT
    test_data->pipe_select = NULL;
    if (SOC_IS_TRIDENT(unit) || SOC_IS_TITAN(unit)) {
        test_data->pipe_select = soc_trident_pipe_select;
    }
#ifdef BCM_TRIDENT2_SUPPORT
    else if ((SOC_IS_TRIDENT2X(unit) || SOC_IS_TITAN2X(unit)) && \
             (!SOC_IS_APACHE(unit))) {
        test_data->pipe_select = soc_trident2_pipe_select;
    }
#endif
#endif
}

/*
 * Function:
 *      soc_ser_create_test_data_with_new_format
 * Purpose:
 *      Populates and performs data verification on a ser_test_data_t structure.  This was
 *      broken out into a separate function to avoid performing this work an excessive number
 *      of times.
 * Parameters:
 *      unit                 - (IN) Device Number
 *      tmp_entry            - (IN) A pointer to a uint32 array of size SOC_MEM_MAX_WORDS
 *      fieldData            - (IN) A pointer to a uint 32 array of size [SOC_MAX_REG_FIELD_WORDS]
 *      parity_enable_reg    - (IN) Register which turns on and off memory protection
 *      parity_enable_mem  - (IN) Memory which truns on and off memory protection
 *      tcam_parity_bit      - (IN) Bit in the parity_enable_reg field for TCAM parity
                                    for this memory. This must be INVALID_TCAM_PARITY_BIT if
                                    this memory is not a TCAM protected memory.
 *      hw_parity_field      - (IN) If controlled by H/W, this is the field in the enable
                                    register that coorisponds to this memory.
 *      parity_field_position   - (IN) Bit in hw_parity_field for memory protection
 *      mem                  - (IN) The memory ID on which to test SER
 *      mem_block            - (IN) The block for this memory.
                                    MEM_BLOCK_ANY is fine for TCAM mems
 *      port                 - (IN) The port for this memory/control reg.
                                    REG_PORT_ANY is fine for TCAMS
 *      acc_type             - (IN) Access type for this memory.
 *      index                - (IN) The entry index on which to test SER
 *      test_data            - (OUT)A pointer to a data structure cotaining SER test info.
 * Returns:
 *      SOC_E_NONE if the test passes, and error if it does
 */
void
soc_ser_create_test_data_with_new_format(int unit, uint32 *tmp_entry, uint32 *field_data,
                         soc_reg_t parity_enable_reg, soc_mem_t parity_enable_mem,
                         int tcam_parity_bit, soc_field_t hw_parity_field,
                         int parity_field_position, soc_mem_t mem,
                         soc_field_t test_field, soc_block_t mem_block,
                         soc_port_t port, soc_acc_type_t acc_type,
                         int index, ser_test_data_t *test_data)
{
    uint16 field_num = 0;
    static soc_field_t field_names[] = {
        ECCf,
        ECC_0f,
        ECC0f,
        LWR_ECC0f,
        DATAf,
        DATA_0f,
        EVEN_PARITYf,
        PARITYf,
        EVEN_PARITY_0f,
        EVEN_PARITY_LOWERf,
        PARITY_0f,
        EVEN_EGR_VLAN_STG_PARITY_P0f,
        INVALIDf
    };
    int i, found = 0, field_length = 1;
    size_t len;
    int mem_has_ecc = 0;

    test_data->mem_fv = mem;
    test_data->index_fv = index;
    /* compute test_data->mem, test_data->index */
    (void)ser_test_mem_index_remap(unit, test_data, &mem_has_ecc);
    test_data->parity_enable_reg = parity_enable_reg;
    test_data->parity_enable_mem = parity_enable_mem;
    test_data->parity_enable_field = hw_parity_field;
    test_data->parity_enable_field_position = parity_field_position;
    test_data->tcam_parity_bit = tcam_parity_bit;
    test_data->mem_block = mem_block;
    test_data->port = port;
    test_data->mem_info = &SOC_MEM_INFO(unit, test_data->mem);
    test_data->test_field = test_field;

    /* An INVALIDf, 0 passed in for test_field implies we should select
     * any valid field. */
    if ((test_field == INVALIDf) || (test_field == 0) ||
        !SOC_MEM_FIELD_VALID(unit, test_data->mem, test_field)) {
        /* select from preferred list */
        for (i = 0; (field_names[i] != INVALIDf); i++) {
            if (SOC_MEM_FIELD_VALID(unit, test_data->mem, field_names[i])) {
                test_data->test_field = field_names[i];
                found = 1;
                break;
            }
        }
        /* select first multi-bit field or last 1-bit field */
        if (!found && (test_data->mem_info != NULL)) {
           field_num = test_data->mem_info->nFields;
           for (i = 0; i < field_num; i++) {
               test_data->test_field = test_data->mem_info->fields[i].field;
               field_length = soc_mem_field_length(unit, test_data->mem,
                                                   test_data->test_field);
               if (field_length > 1) {
                   found = 1;
                   break;
               }
           }
        }
    }
    LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit,
                        "unit %d, soc_ser_create_test_data: error_inject_view: mem %s, test_field %s\n"),
             unit, SOC_MEM_NAME(unit, test_data->mem),
             _SER_TEST_MEM_SOC_FIELD_NAME(unit, test_data->test_field)));

    test_data->acc_type = acc_type;
    test_data->entry_buf = tmp_entry;
    test_data->field_buf = field_data;
#ifdef SOC_MEM_NAME
    /* coverity[secure_coding] */
    len = sal_strlen(SOC_MEM_NAME(unit, test_data->mem));
    if(sizeof(test_data->mem_name)- 1 < len) {
        len = sizeof(test_data->mem_name)- 1;
    }
    sal_strncpy(test_data->mem_name, SOC_MEM_NAME(unit, test_data->mem), len);
    test_data->mem_name[len] = 0;
    /* coverity[secure_coding] */
    len = sal_strlen(_SER_TEST_MEM_SOC_FIELD_NAME(unit, test_data->test_field));
    if(sizeof(test_data->field_name)- 1 < len) {
        len = sizeof(test_data->field_name)- 1;
    }
    sal_strncpy(test_data->field_name,
               _SER_TEST_MEM_SOC_FIELD_NAME(unit, test_data->test_field), len);
    test_data->field_name[len] = 0;
#else
    sprintf(test_data->mem_name, "Mem ID: %d", test_data->mem);
    sprintf(test_data->field_name, "Field ID: %d", test_data->test_field);
#endif
    test_data->badData = 0;

#ifdef BCM_TRIDENT_SUPPORT
    test_data->pipe_select = NULL;
#endif
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        test_data->pipe_select = soc_trident3_pipe_select;
    }
#endif
}

int soc_ser_test_long_sleep = FALSE;
int soc_ser_test_long_sleep_time_us = 100000;
/*
 * Function:
 *      ser_test_mem
 * Purpose:
 *      Serform a Software Error Recovery test on passed memory mem.
 * Parameters:
 *      unit                 - (IN) Device Number
 *      test_data            - (IN) Structure which holds the test data and some
                                    intermediate results.
 *      test_type            - (IN) Used to determine how many indexes to test for
                                    each memory table.
 *      error_count          - (OUT)Increments if a test on any of the indexes
                                    in this memory fail.
 * Returns:
 *      SOC_E_NONE if the test passes, an error if it does not
 */
#ifdef BCM_TOMAHAWK_SUPPORT
extern int _soc_th_refresh_modify(int unit, int enable);
#endif
soc_error_t
ser_test_mem(int unit, uint32 flags, ser_test_data_t *test_data,
             _soc_ser_test_t test_type, int *error_count)
{
    int numIndexesToTest, j, read_through, startErrorCount;
    soc_error_t rv = SOC_E_NONE;
    int indexesToTest[3];
    int mem_has_ecc;
    uint32 read_flags = 0;

    indexesToTest[0] = soc_mem_index_min(unit,test_data->mem);
    startErrorCount = *error_count;
    /*Check that there is a meminfo structure for this memory.*/
    if (!SOC_MEM_IS_VALID(unit, test_data->mem_fv)) {
        LOG_CLI((BSL_META_U(unit,
                            "%s is not a valid memory for this platform. Skipping.\n"),
                 SOC_MEM_NAME(unit, test_data->mem_fv)));
        return SOC_E_MEMORY;
    }
    /*Exit without testing if there is no SER flag set for this memory*/
    if (!(SOC_MEM_INFO(unit,test_data->mem_fv).flags & SOC_MEM_SER_FLAGS)) {
        /*Verbose output: Skipping memory: name*/
        LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit,
                            "SOC_MEM_SER_FLAGS is not set for %s "
                            "(flags 0x%8x). Skipping.\n"),
                 SOC_MEM_NAME(unit, test_data->mem_fv),
                              SOC_MEM_INFO(unit,test_data->mem_fv).flags));
        return SOC_E_NONE;
    }
    switch (test_type) {
    case SER_FIRST_MID_LAST_INDEX:
        indexesToTest[1] = soc_mem_index_max(unit,test_data->mem_fv);
        indexesToTest[2] = (indexesToTest[1] - indexesToTest[0])/2;
        numIndexesToTest = 3;
        break;
    case SER_ALL_INDEXES:
        numIndexesToTest = soc_mem_index_max(unit,test_data->mem_fv) + 1;
        break;
    case SER_SINGLE_INDEX:
        /*FALL THOUGH*/
    default:
        numIndexesToTest = 1;
    };
    for (j = 0; j < numIndexesToTest; j++) {
        if (test_type == SER_ALL_INDEXES){
            test_data->index_fv = j;
        } else {
            test_data->index_fv = indexesToTest[j];
        }
        if (soc_feature(unit, soc_feature_field_slice_size128)) {
            if ((test_data->mem_fv == FP_TCAMm) || 
                (test_data->mem_fv == FP_GLOBAL_MASK_TCAMm)) {
                /* Skip the entries that is not existed. */ 
                if ((test_data->index_fv/64) % 2) {
                    continue;
                }
            }
        }

        /* Revaluate test_data->index for every index being tested */
        SOC_IF_ERROR_RETURN
            (ser_test_mem_index_remap(unit, test_data, &mem_has_ecc));
        LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit,
                        "ser_mem_test: mem %s %s."
                        "field %s, index_fv %d, index %d\n"),
                        test_data->mem_name,
                        mem_has_ecc? "is ECC protected" : " ",
                        test_data->field_name, test_data->index_fv,
                        test_data->index));

        if (soc_feature(unit, soc_feature_memory_2bit_ecc_ser)) {
            if (mem_has_ecc) {
                flags |= SOC_INJECT_ERROR_2BIT_ECC;
            }
        } else
#ifdef BCM_TOMAHAWK_SUPPORT
        if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
            /* compute mem_has_ecc - need to do tnis because for some mems in
             * regsfile,  parity enable registers, etc are declared only for
             * _ECC view and not for func_view 
             * Thus when we are here with incorrect assumption that 1b error needs to be
             * injected.
             */
            /* mem_has_ecc = ser_test_mem_index_remap(unit, test_data); */

            /* Following assumes that in TH, we have injected 2b error if mem
             * was ECC protected.
             * For ECC protected mems, if we inject only 1b error, and even if 1b
             * error reporting is enabled, then we will see ser event and
             * correction but will not get NACK on 1st read
             */
            if (mem_has_ecc) {
                flags |= SOC_INJECT_ERROR_2BIT_ECC;
            }
        }
#endif /* BCM_TOMAHAWK_SUPPORT */

        /*Read through to the hardware.*/
        if (flags & SOC_INJECT_ERROR_DONT_MAP_INDEX) {
            read_flags = SER_TEST_MEM_F_DONT_MAP_INDEX;
        }
        read_through = SOC_MEM_FORCE_READ_THROUGH(unit);
        SOC_MEM_FORCE_READ_THROUGH_SET(unit, TRUE);
        rv = (ser_test_mem_read(unit, read_flags, test_data));
        if (SOC_FAILURE(rv)) {
            (*error_count)++;
            LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit,
                                "SER failed. 1st Read got NACK. mem: %s field: %s\n"),
                                test_data->mem_name,
                                test_data->field_name));
            return rv;
        }
        /*Disable Parity*/
        rv = (_ser_test_parity_control(unit, test_data, 0));
        if (SOC_FAILURE(rv)) {
            (*error_count)++;
            LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit,
                                "SER failed. Disable Parity did not work. mem: %s field: %s\n"),
                                test_data->mem_name,
                                test_data->field_name));
            return rv;
        }

        /*Inject error:*/
        if (flags & SOC_INJECT_ERROR_2BIT_ECC) {
            LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit,
                                "SER info. 2b error will be injected for mem: %s\n"),
                                test_data->mem_name));
        }
        rv = (soc_ser_test_inject_error(unit, test_data, flags));

        if (SOC_FAILURE(rv)) {
            (*error_count)++;
            LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit,
                                "SER failed. soc_ser_test_inject_error failed. mem: %s field: %s\n"),
                                test_data->mem_name,
                                test_data->field_name));
            return rv;
        }
        /*Read back write to ensure value is still fudged.*/
        rv = (ser_test_mem_read(unit, read_flags, test_data));
        if (SOC_FAILURE(rv)) {
            (*error_count)++;
            LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit,
                                "SER failed. could not corrupt the data. mem: %s field: %s\n"),
                                test_data->mem_name,
                                test_data->field_name));
            return rv;
        }
        if (test_data->badData != test_data->field_buf[0]) {
            LOG_CLI((BSL_META_U(unit,
                                "SER failed. Injection Error for mem: %s "
                                "field: %s Wrote[%d]: Read[%d]\n"),
                     test_data->mem_name,
                     test_data->field_name,
                     test_data->badData,
                     test_data->field_buf[0]));
            (*error_count)++;
            return SOC_E_FAIL;
        }
        /*Enable Parity (Even if it was not enabled before*/
        rv = (_ser_test_parity_control(unit, test_data, 1));
        if (SOC_FAILURE(rv)) {
            (*error_count)++;
            LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit,
                                "SER failed. Re-enable Parity did not work. mem: %s field: %s\n"),
                                test_data->mem_name,
                                test_data->field_name));
            return rv;
        }
        if (SAL_BOOT_QUICKTURN) {
            sal_usleep(10000); /* Add delay for QT to trigger parity error */
        }
        /*Read the memory entry with cache disabled, in order to induce the error*/
        rv = ser_test_mem_read(unit, (read_flags | SER_TEST_MEM_F_READ_FUNC_VIEW),
                               test_data); /* read func_view */
#ifdef BCM_TOMAHAWK_SUPPORT
        if (SOC_IS_TOMAHAWKX(unit)) {
            /* Following assumes that in TH, we have injected 2b error if mem
             * was ECC protected.
             * For ECC protected mems, if we inject only 1b error, and even if 1b
             * error reporting is enabled, then we will see ser event and
             * correction but will not get NACK on 1st read
             */
            if (!SOC_FAILURE(rv)) {
                (*error_count)++;
                LOG_ERROR(BSL_LS_SOC_SER,
                          (BSL_META_U(unit,
                                      "SER failed. Did not receive NACK on 1st Read "
                                      "for mem %s index %d\n"),
                           SOC_MEM_NAME(unit, test_data->mem_fv),
                                        test_data->index_fv));
                return SOC_E_FAIL;
            }
        }
#endif /* BCM_TOMAHAWK_SUPPORT */

        /* 'MMU_THDM_DB_PORTSP_BST_0m' on Apache does not get corrected
           intime on Apache, hence we will sleep a little longer */
#ifdef BCM_APACHE_SUPPORT
        if (test_data->mem == MMU_THDM_DB_PORTSP_BST_0m) {
            sal_usleep(2000000); /*wait for memory and register ops to complete.*/
        } else
#endif
        {
        sal_usleep(1500000); /*wait for memory and register ops to complete.*/
        }

        if (SAL_BOOT_QUICKTURN) {
            sal_usleep(10000000);    /* add more delay for Quickturn.*/
        } else if (soc_ser_test_long_sleep) {
            sal_usleep(soc_ser_test_long_sleep_time_us);
        }
        /*Read the memory once more to compare it to the old value*/
        rv = (ser_test_mem_read(unit, (read_flags | SER_TEST_MEM_F_READ_FUNC_VIEW),
                                test_data)); /* read func_view */
        if (SOC_FAILURE(rv)) {
            (*error_count)++;
            LOG_ERROR(BSL_LS_SOC_SER,
                      (BSL_META_U(unit,
                                  "SER failed. Received NACK on 2nd Read "
                                  "for mem %s index %d\n"),
                       SOC_MEM_NAME(unit, test_data->mem_fv),
                                    test_data->index_fv));
            return rv;
        }

        /* For case when we injected error on _ECC view,
         * we must now read _ECC view, so we can check:
         * a. whether entry got cleared for SER_ENTRY_CLEAR cases
         * b. whether field where we injected error matches badData
         */
        if (test_data->mem != test_data->mem_fv) {
            rv = (ser_test_mem_read(unit, read_flags, test_data)); /* rd error_inject view */
            if (SOC_FAILURE(rv)) {
                (*error_count)++;
                LOG_ERROR(BSL_LS_SOC_SER,
                          (BSL_META_U(unit,
                                      "SER failed. Received NACK when reading"
                                      "error_inject view after 2nd read of"
                                      "func_view. mem %s index %d\n"),
                           SOC_MEM_NAME(unit, test_data->mem),
                                        test_data->index));
                return rv;
            }
        }
        SOC_MEM_FORCE_READ_THROUGH_SET(unit, read_through);
        if ((SOC_MEM_INFO(unit, test_data->mem_fv).flags & SOC_MEM_SER_FLAGS) ==
            SOC_MEM_FLAG_SER_ENTRY_CLEAR) {
            if (0 != test_data->field_buf[0]) {
                (*error_count)++;
                LOG_ERROR(BSL_LS_SOC_SER,
                          (BSL_META_U(unit,
                                      "SER failed to clear mem %s index %d field %s \n"),
                           test_data->mem_name, test_data->index,
                           test_data->field_name));
                return SOC_E_FAIL;
            }
        } else if (test_data->badData == test_data->field_buf[0]) {
#ifdef BCM_TRIUMPH2_SUPPORT
            if ((SOC_IS_TRIUMPH3(unit) && (test_data->mem == VLAN_OR_VFI_MAC_LIMITm)) ||
                (SOC_IS_TRIUMPH2(unit) && ((test_data->mem == PORT_OR_TRUNK_MAC_LIMITm) ||
                                           (test_data->mem == VLAN_OR_VFI_MAC_LIMITm))) ||
                (SOC_IS_APOLLO(unit) && ((test_data->mem == PORT_OR_TRUNK_MAC_LIMITm) ||
                                           (test_data->mem == VLAN_OR_VFI_MAC_LIMITm))))
            {
                LOG_VERBOSE(BSL_LS_SOC_SER,
                        (BSL_META_U(unit,
                                    "SER corrected mem %s index %d\n"),
                         SOC_MEM_NAME(unit, test_data->mem_fv),
                         test_data->index_fv));
            } else
#endif /* BCM_TRIUMPH2_SUPPORT */
            {
                (*error_count)++;
                LOG_ERROR(BSL_LS_SOC_SER,
                          (BSL_META_U(unit,
                                  "SER failed to correct mem %s index %d field %s\n"),
                           test_data->mem_name, test_data->index,
                           test_data->field_name));
                return SOC_E_FAIL;
            }
        }
        else {
            LOG_VERBOSE(BSL_LS_SOC_SER,
                        (BSL_META_U(unit,
                                    "SER corrected mem %s index %d\n"),
                         SOC_MEM_NAME(unit, test_data->mem_fv),
                         test_data->index_fv));
        }
    }
    if (startErrorCount != *error_count) {
        LOG_CLI((BSL_META_U(unit,
                            "SER failed to correct memory %s acc_type: %d field: %s %d:%d\n"),
                 SOC_MEM_NAME(unit, test_data->mem_fv),
                 (int)test_data->acc_type,
                 test_data->field_name,
                 startErrorCount, *error_count));
        return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      _ser_test_parity_control_reg_set
 * Purpose:
 *      Enables/Disables parity for tcam memories using the 'newer' soc_reg_set
 *      call.  This is what is used for Trident_2 and possibly newer devices.
 * Parameters:
 *      unit       - (IN) Device Number
 *      test_data  - (IN) Contains parity registers to set.
 *      enable     - (IN) (Bool) enable parity if true, disable if false
 * Returns:
 *      SOC_E_NONE if no errors are encountered while setting parity.
 */
soc_error_t
_ser_test_parity_control_reg_set(int unit, ser_test_data_t *test_data, int enable)
{
    uint64 regData;
    if (enable) {
        /* In new devices SER_RANGE_ENABLE supports 32 ranges */
        COMPILER_64_SET(regData, 0, 0xFFFFFFFF);
    } else {
        COMPILER_64_SET(regData, 0, 0x0);
    }
    return soc_reg_set(unit, test_data->parity_enable_reg, test_data->port, 0, regData);
}


/*
 * Function:
 *      _ser_test_parity_control_pci_write
 * Purpose:
 *      Enables/Disables parity for tcam memories using the less-compatible
 *      soc_pci_write call.  Used for older devices such as Trident and older.
 * Parameters:
 *      unit       - (IN) Device Number
 *      test_data  - (IN) Contains parity registers to set.
 *      enable     - (IN) (Bool) enable parity if true, disable if false
 * Returns:
 *      SOC_E_NONE if no errors are encountered while setting parity.
 */
soc_error_t
_ser_test_parity_control_pci_write(int unit, ser_test_data_t *test_data, int enable)
{
    uint32 regDataOld;
    uint32 regAddr = soc_reg_addr(unit, test_data->parity_enable_reg, REG_PORT_ANY,
                                  test_data->tcam_parity_bit/16);
    if (enable) {
        regDataOld = 0xFFFF;
    } else {
        regDataOld = 0x0;
    }
    return soc_pci_write(unit,regAddr, regDataOld);
}


/*
 * Function:
 *      _ser_test_parity_control
 * Purpose:
 *      Disables/enables parity for the provided register.
 * Parameters:
 *      unit          - (IN) Device Number
 *      test_data     - (IN) Contains parity registers to set.
 *      enable        - (IN) (Bool) enable parity if true, disable if false
 *
 * Returns:
 *      SOC_E_NONE if no errors are encountered while setting parity.
 */
soc_error_t
_ser_test_parity_control(int unit, ser_test_data_t *test_data, int enable)
{
#ifdef INCLUDE_XFLOW_MACSEC
        int block_type = SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_ANY(unit, test_data->mem));
#endif
    soc_error_t rv = SOC_E_NONE;
    if (test_data->tcam_parity_bit >= 0) {
        /*This register controls a TCAM memory*/
        if (ser_test_functions[unit] != NULL &&
            ser_test_functions[unit]->parity_control != NULL) {
            rv = (*(ser_test_functions[unit]->parity_control))(unit, test_data,
                    enable);
        } else {
            rv = _ser_test_parity_control_reg_set(unit, test_data, enable);
        }
    } else {
#ifdef BCM_TRIUMPH3_SUPPORT
        if (SOC_IS_TRIUMPH3(unit) &&
            ser_test_functions[unit] != NULL &&
            ser_test_functions[unit]->parity_control != NULL) {
            rv = (*(ser_test_functions[unit]->parity_control))(unit, test_data,
                    enable);
        } else
#endif
        {
#ifdef BCM_TRIDENT2_SUPPORT
            if (SOC_IS_TRIDENT2(unit) || SOC_IS_TITAN2(unit)) {
                uint32 reg_data;
                /*This register is monitored by hardware*/
                SOC_MEM_TEST_PIPE_LOCK(unit, test_data);
                rv = soc_reg32_get(unit, test_data->parity_enable_reg, 
                            test_data->port, 0, &reg_data);
                soc_reg_field_set(unit, test_data->parity_enable_reg, &reg_data,
                            test_data->parity_enable_field, enable);
                rv = soc_reg32_set(unit, test_data->parity_enable_reg, 
                                test_data->port, 0, reg_data);
                SOC_MEM_TEST_PIPE_UNLOCK(unit, test_data);
            } else
#endif
            {
                SOC_MEM_TEST_PIPE_LOCK(unit, test_data);
#ifdef BCM_TOMAHAWK_SUPPORT
                if (SOC_IS_TOMAHAWKX(unit)) {
                    if ((test_data->parity_enable_reg == MTRO_ENABLE_ECCP_MEMr) ||
                     (test_data->parity_enable_reg == Q_SCHED_ENABLE_ECCP_MEMr) ||
                     (test_data->parity_enable_reg == MMU_DQS_ENABLE_ECCP_MEMr) ||
                     (test_data->parity_enable_reg == MMU_MB_TCB_ENABLE_ECCP_MEMr) ||
                       (test_data->parity_enable_reg == MMU_ENQX_ENABLE_ECCP_MEMr)) {
                    int i;
                    for (i = 0; i < NUM_PIPE(unit); i++) {
                        rv = soc_th_ser_reg_field32_modify(
                                 unit, test_data->parity_enable_reg,
                                 test_data->port, test_data->parity_enable_field,
                                 enable,
                                 0, /* index */
                                 i); /* mmu_base_index */
                    }
                    } else {
#ifdef BCM_TOMAHAWK3_SUPPORT
                        if (SOC_IS_TOMAHAWK3(unit)) {
                            rv = soc_tomahawk3_parity_bit_enable(unit, test_data->parity_enable_reg,
                                                      test_data->parity_enable_mem,
                                                      test_data->parity_enable_field,
                                                      test_data->parity_enable_field_position,
                                                      enable);
                        } else
#endif
                        {
                            rv = soc_reg_field32_modify(unit, test_data->parity_enable_reg,
                                            test_data->port,
                                            test_data->parity_enable_field, enable);
                        }
                    }

                } else
#endif
#ifdef BCM_TRIDENT3_SUPPORT
                if (SOC_IS_TRIDENT3X(unit)) {
                    if ((test_data->parity_enable_reg == MTRO_ENABLE_ECCP_MEMr) ||
                     (test_data->parity_enable_reg == Q_SCHED_ENABLE_ECCP_MEMr) ||
                     (test_data->parity_enable_reg == Q_SCHED_ENABLE_ECCP_MEM_64r) ||
                     (test_data->parity_enable_reg == MMU_DQS_ENABLE_ECCP_MEMr) ||
                     (test_data->parity_enable_reg == MMU_MB_TCB_ENABLE_ECCP_MEMr)) {
                        int i;
                        for (i = 0; i < NUM_PIPE(unit); i++) {
                            rv = soc_td3_ser_reg_field32_modify(
                                     unit, test_data->parity_enable_reg,
                                     test_data->port, test_data->parity_enable_field,
                                     enable,
                                     0, /* index */
                                     i); /* mmu_base_index */
                        }
                    } else {
#ifdef BCM_HURRICANE4_SUPPORT
                        if (SOC_IS_HURRICANE4(unit)) {
                            rv = _soc_hr4_dual_parser_parity_bit_enable(unit,
                                                                        test_data->mem,
                                                                        enable);
                            if (SOC_FAILURE(rv)) {
                                SOC_MEM_TEST_PIPE_UNLOCK(unit, test_data);
                                return rv;
                            }
                        }
#endif /* BCM_HURRICANE4_SUPPORT */
#if defined (INCLUDE_XFLOW_MACSEC) && defined (BCM_HURRICANE4_SUPPORT)
                        if (SOC_IS_HURRICANE4(unit) &&
                                (soc_feature(unit, soc_feature_xflow_macsec))) {
                            int block_type = SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_ANY(unit, test_data->mem));
                            soc_reg_t en_reg = test_data->parity_enable_reg;
                            if (test_data->mem == ESEC_SA_TABLEm) {
                                switch (test_data->index_fv % 4) {
                                case 0:
                                    en_reg = ECC_CTLSTS_ESATBL0r;
                                    break;
                                case 1:
                                    en_reg = ECC_CTLSTS_ESATBL1r;
                                    break;
                                case 2:
                                    en_reg = ECC_CTLSTS_ESATBL2r;
                                    break;
                                case 3:
                                    en_reg = ECC_CTLSTS_ESATBL3r;
                                    break;
                                default:
                                    break;
                                }
                            }
                            rv = soc_td3_parity_bit_enable(unit, en_reg,
                                    test_data->parity_enable_mem,
                                    test_data->parity_enable_field,
                                    test_data->parity_enable_field_position,
                                    (block_type != SOC_BLK_MACSEC) ? enable : !enable);
                        } else
#endif
                        {

                            rv = soc_td3_parity_bit_enable(unit, test_data->parity_enable_reg,
                                    test_data->parity_enable_mem,
                                    test_data->parity_enable_field,
                                    test_data->parity_enable_field_position,
                                    enable);
                        }
                    }
                } else
#endif
                {
#ifdef INCLUDE_XFLOW_MACSEC
                    if (soc_feature(unit, soc_feature_xflow_macsec) &&
                        !soc_feature(unit, soc_feature_xflow_macsec_inline)) {

                        rv = soc_reg_field32_modify(unit, test_data->parity_enable_reg,
                                                test_data->port,
                                                test_data->parity_enable_field, (block_type != SOC_BLK_MACSEC) ? enable : !enable);
                    } else
#endif
                    {
                        rv = soc_reg_field32_modify(unit, test_data->parity_enable_reg,
                                                test_data->port,
                                                test_data->parity_enable_field, enable);
                    }
                }
                SOC_MEM_TEST_PIPE_UNLOCK(unit, test_data);
            }
        }
    }
    return rv;
}


/*
 * Function:
 *      ser_test_cmd_generate
 * Purpose:
 *      Creates a test for a memory the user can then run from the commandline
 * Parameters:
 *      unit          - (IN) Device Number
 *      test_data     - (IN) Contains all test attributes required to print the
 *                           test commands
 */
void
ser_test_cmd_generate(int unit, ser_test_data_t *test_data)
{
    char* pipe_str;
    int ypipe = FALSE;
    if (test_data != NULL && test_data->mem_name != NULL &&
        SOC_MEM_FIELD_VALID(unit, test_data->mem, test_data->test_field)) {
        if (test_data->acc_type == _SOC_ACC_TYPE_PIPE_Y) {
            pipe_str = "pipe_y ";
            ypipe = TRUE;
        } else {
            pipe_str = "";
        }
        LOG_CLI((BSL_META_U(unit,
                            "\nCommand line test for memory %s: access_type: %d\n"),
                 test_data->mem_name, test_data->acc_type));
        if (ypipe) {
            LOG_CLI((BSL_META_U(unit,
                                "s EGR_SBS_CONTROL PIPE_SELECT=1\n")));
            LOG_CLI((BSL_META_U(unit,
                                "s SBS_CONTROL PIPE_SELECT=1\n")));
        }
            LOG_CLI((BSL_META_U(unit,
                                "wr %s%s 1 1 %s=0\n"),
                     pipe_str, test_data->mem_name, test_data->field_name));
            LOG_CLI((BSL_META_U(unit,
                                "d nocache %s%s 1 1\n"), pipe_str, test_data->mem_name));
            if (test_data->tcam_parity_bit != -1) {
                LOG_CLI((BSL_META_U(unit,
                                    "s %s %s=0\n"),
                         SOC_REG_NAME(unit,test_data->parity_enable_reg),
                         SOC_FIELD_NAME(unit,
                         test_data->parity_enable_field)));
            } else {
                LOG_CLI((BSL_META_U(unit,
                                    "s %s 0\n"),
                         SOC_REG_NAME(unit,test_data->parity_enable_reg)));
            }
            LOG_CLI((BSL_META_U(unit,
                                "wr nocache %s%s 1 1 %s=1\n"),
                     pipe_str, test_data->mem_name, test_data->field_name));
            if (test_data->tcam_parity_bit != -1) {
                LOG_CLI((BSL_META_U(unit,
                                    "s %s %s=1\n"),
                         SOC_REG_NAME(unit, test_data->parity_enable_reg),
                         SOC_FIELD_NAME(unit,
                         test_data->parity_enable_field)));
            } else {
                LOG_CLI((BSL_META_U(unit,
                                    "s %s 1\n"),
                         SOC_REG_NAME(unit,test_data->parity_enable_reg)));
            }
            LOG_CLI((BSL_META_U(unit,
                                "d nocache %s%s 1 1\n"), pipe_str, test_data->mem_name));
        if (ypipe) {
            LOG_CLI((BSL_META_U(unit,
                                "s EGR_SBS_CONTROL PIPE_SELECT=0\n")));
            LOG_CLI((BSL_META_U(unit,
                                "s SBS_CONTROL PIPE_SELECT=0\n")));
        }

    } else {
        LOG_CLI((BSL_META_U(unit,
                            "ERROR: Attempted to print a cmd from missing data.\n")));
    }
}


/*
 * Function:
 *      soc_ser_test_overlays
 * Purpose:
 *      Enables SER test functions for a unit
 * Parameters:
 *      unit        - (IN) Device Number
 *      test_type   - (IN) Sets how many memories to test
 *      overlays    - (IN) List of all overlay memories to test
 *      pipe_select - (IN) Chip-specific function used to change pipe select
 */
int
soc_ser_test_overlays(int unit, _soc_ser_test_t test_type,
                      const soc_ser_overlay_test_t *overlays,
                      int(*pipe_select)(int,int,int) )
{
    int i, mem_failed=0, rv = SOC_E_NONE;
    soc_acc_type_t acc_type;
    uint32 tmp_entry[SOC_MAX_MEM_WORDS], fieldData[SOC_MAX_REG_FIELD_WORDS];
    ser_test_data_t test_data;
    if (overlays == NULL) {
        return -1;
    }
    for (i = 0; overlays[i].mem != INVALIDm; i++) {
        int last_failed = mem_failed;
        acc_type = overlays[i].acc_type;

        if (SOC_MEM_INFO(unit, overlays[i].mem).flags & 
            SOC_MEM_FLAG_SER_PARITY_ENABLE_SKIP) {
            continue;
        }

        if (soc_mem_index_count(unit, overlays[i].mem) <= 0) {
            continue;
        }

        soc_ser_create_test_data(unit, tmp_entry, fieldData,
                                 overlays[i].parity_enable_reg,
                                 SOC_INVALID_TCAM_PARITY_BIT,
                                 overlays[i].parity_enable_field,
                                 overlays[i].mem, EVEN_PARITYf,
                                 MEM_BLOCK_ANY, REG_PORT_ANY,
                                 acc_type, 0, &test_data);

        if(NULL != pipe_select) {
            pipe_select(unit, TRUE, ((acc_type == _SOC_ACC_TYPE_PIPE_Y)? 1:0));
            pipe_select(unit, FALSE, ((acc_type == _SOC_ACC_TYPE_PIPE_Y)? 1:0));
        }
		
        rv = ser_test_mem(unit, 0, &test_data, test_type, &mem_failed);
        
        /*Clear memory so other views can be used.*/
        if (soc_mem_clear(unit,overlays[i].mem, MEM_BLOCK_ALL, TRUE) < 0) {
            return -1;
        }
        
        if(NULL != pipe_select) {
            pipe_select(unit, TRUE,0);
            pipe_select(unit, FALSE,0);
        }
        
        if ((mem_failed != last_failed) || (rv != SOC_E_NONE)) {
            LOG_CLI((BSL_META_U(unit,
                                "%s failed overlay test.\n"),
                     SOC_MEM_NAME(unit,test_data.mem)));
        }
    }
    return mem_failed;
}


/*
 * Function:
 *      soc_ser_test_functions_register
 * Purpose:
 *      Enables SER test functions for a unit
 * Parameters:
 *      unit    - (IN) Device Number
 *      fun     - (IN) An instance of soc_ser_test_functions_t with pointers to
 *                     functions required for chip-specific SER test
                       implementations.
 */
void
soc_ser_test_functions_register(int unit, soc_ser_test_functions_t *fun)
{
    if (unit < SOC_MAX_NUM_DEVICES) {
        ser_test_functions[unit] = fun;
    }
    else {
        LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                                "Invalid unit parameter %d: passed to soc_ser_test_functions_t"),
                     unit));
    }
}


/*
 * Function:
 *      soc_ser_inject_error
 * Purpose:
 *      Redirect to appropriate chip-specific function.
 *      These are found in the chip's soc/esw/X.c file
 *      and registered during misc init.
 */
soc_error_t
soc_ser_inject_error(int unit, uint32 flags, soc_mem_t mem, int pipe,
                     int blk, int index)
{
    if( ser_test_functions[unit] != NULL &&
        ser_test_functions[unit]->inject_error_f != NULL) {
        return (*(ser_test_functions[unit]->inject_error_f))(unit, flags, mem,
                    pipe, blk, index);
    }
    return SOC_E_FAIL;
}

/*
 * Function:
 *      soc_ser_inject_support
 * Purpose:
 *      Routine to determine Memory is supported by error
 *      injection interface.
 */
soc_error_t
soc_ser_inject_support(int unit, soc_mem_t mem, int pipe)
{
    if( ser_test_functions[unit] != NULL &&
        ser_test_functions[unit]->injection_support != NULL) {
        return (*(ser_test_functions[unit]->injection_support))
                    (unit, mem, pipe);
    }
    return SOC_E_FAIL;
}

/*
 * Function:
 *      soc_ser_test_mem
 * Purpose:
 *      Redirect to appropriate chip-specific function.
 *      These are found in the chip's soc/esw/X.c file
 *      and registered during misc init.
 */
soc_error_t
soc_ser_test_mem(int unit, soc_mem_t mem, _soc_ser_test_t testType, int cmd)
{
    if( ser_test_functions[unit] != NULL &&
        ser_test_functions[unit]->test_mem != NULL) {
        return (*(ser_test_functions[unit]->test_mem))(unit, mem, testType, cmd);
    }
    return SOC_E_FAIL;
}


/*
 * Function:
 *      soc_ser_test
 * Purpose:
 *      Redirect to appropriate chip-specific function.
 *      These are found in the chip's soc/esw/X.c file and registered during misc init.
 */
soc_error_t
soc_ser_test(int unit, _soc_ser_test_t testType)
{
    if( ser_test_functions[unit] != NULL &&
        ser_test_functions[unit]->test != NULL) {
        return (*(ser_test_functions[unit]->test))(unit, testType);
    }
    return SOC_E_FAIL;
}
#endif /* BCM_ESW_SUPPORT || BCM_SAND_SUPPORT */
#endif /* defined (SER_TR_TEST_SUPPORT)*/
#endif /* BCM_ESW_SUPPORT || BCM_SAND_SUPPORT */
