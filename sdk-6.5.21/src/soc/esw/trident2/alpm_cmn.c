/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * File:    alpm_common.c
 * Purpose: ALPM common code.
 * Requires:
 */

/* Implementation notes:
 */
#include <shared/bsl.h>

#include <soc/mem.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/lpm.h>
#include <soc/trident2.h>
#include <soc/tomahawk.h>
#include <soc/trident3.h>
#include <soc/apache.h>
#include <soc/monterey.h>
#include <shared/bsl.h>

#ifdef ALPM_ENABLE
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_APACHE_SUPPORT)

#include <shared/util.h>
#include <shared/l3.h>
#include <soc/alpm.h>
#include <soc/esw/alpm_int.h>
#include <soc/esw/trie.h>

/* returns 0 for Combined Search mode and 1 for Parallel Search mode,
 * 2 for TCAM/ALPM mode, equals to L3_DEFIP_RPF_CONTROLr.LOOKUP_MODEf value
 */
int
soc_alpm_cmn_mode_get(int unit)
{
    int mode = 0;

    /* 'mode' here equals to l3_alpm_enable config property value */
#if defined(BCM_APACHE_SUPPORT)
    if (SOC_IS_APACHE(unit)) {
        mode = soc_apache_alpm_mode_get(unit);
    }
#endif
#if defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TOMAHAWKX(unit)) {
        mode = soc_tomahawk_alpm_mode_get(unit);
    }
#endif
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        mode = soc_trident3_alpm_mode_get(unit);
    }
#endif
    switch (mode) {
    case 1:     /* Parallel mode */
        return SOC_ALPM_MODE_PARALLEL;
    case 3:     /* TCAM/ALPM mode */
        return SOC_ALPM_MODE_TCAM_ALPM;
    case 2:
    default:    /* Combined mode */
        return SOC_ALPM_MODE_COMBINED;
    }

    return SOC_ALPM_MODE_COMBINED;
}

int soc_alpm_cmn_banks_get(int unit)
{
    int banks = 0;
#if defined(BCM_APACHE_SUPPORT)
    if (SOC_IS_APACHE(unit)) {
        banks = soc_apache_get_alpm_banks(unit);
    }
#endif
#if defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TOMAHAWKX(unit)) {
        banks = soc_th_get_alpm_banks(unit);
    }
#endif
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        banks = soc_trident3_get_alpm_banks(unit);
    }
#endif

    return banks;
}

#endif /* TOMAHAWK APACHE */
#endif /* ALPM_ENABLE */

void soc_alpm_cmn_bkt_view_set(int unit, int index, soc_mem_t view)
{
#if defined(BCM_APACHE_SUPPORT)
    if (SOC_IS_APACHE(unit)) {
        _soc_apache_alpm_bkt_view_set(unit, index, view);
    }
#endif
#if defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TOMAHAWKX(unit)) {
        _soc_tomahawk_alpm_bkt_view_set(unit, index, view);
    }
#endif
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit) && !SOC_IS_HELIX5(unit) &&
        !SOC_IS_FIREBOLT6(unit)) {
        _soc_trident3_alpm_bkt_view_set(unit, index, view);
    }
#endif
}

soc_mem_t soc_alpm_cmn_bkt_view_get(int unit, int index)
{
    soc_mem_t mem = INVALIDm;
#if defined(BCM_APACHE_SUPPORT)
    if (SOC_IS_APACHE(unit)) {
        mem = _soc_apache_alpm_bkt_view_get(unit, index);
    }
#endif
#if defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TOMAHAWKX(unit)) {
        mem = _soc_tomahawk_alpm_bkt_view_get(unit, index);
    }
#endif
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit) && !SOC_IS_HELIX5(unit) &&
        !SOC_IS_FIREBOLT6(unit)) {
        mem = _soc_trident3_alpm_bkt_view_get(unit, index);
    }
#endif

    return mem;
}

int
soc_alpm_l3_defip_index_map(int unit, soc_mem_t mem, int index)
{
    return soc_trident2_l3_defip_index_map(unit, mem, index);
}

int
soc_alpm_l3_defip_index_remap(int unit, soc_mem_t mem, int index)
{
    return soc_trident2_l3_defip_index_remap(unit, mem, index);
}

