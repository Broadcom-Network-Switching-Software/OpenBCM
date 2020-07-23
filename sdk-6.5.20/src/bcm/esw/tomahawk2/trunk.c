/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    trunk.c
 * Purpose: Tomahawk2 HGT LAG dynamic load balancing
 */

#include <soc/defs.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>
#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/profile_mem.h>

#include <bcm/error.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/trunk.h>
#include <bcm_int/esw_dispatch.h>

/* ------------------------------------------------- */
/* Bookkeeping info for HGT_LAG dynamic load balancing */
/* ------------------------------------------------- */

typedef struct _th2_hgt_lag_dlb_bookkeeping_s {
    SHR_BITDCL *hgt_lag_dlb_id_used_bitmap;
    SHR_BITDCL *hgt_lag_dlb_flowset_block_bitmap; /* Each block corresponds to
                                                 512 entries */
    int hgt_lag_dlb_sample_rate;
    int hgt_lag_dlb_tx_load_min_th;
    int hgt_lag_dlb_tx_load_max_th;
    int hgt_lag_dlb_qsize_min_th;
    int hgt_lag_dlb_qsize_max_th;
    int hgt_lag_dlb_physical_qsize_min_th;
    int hgt_lag_dlb_physical_qsize_max_th;
    uint8 *hgt_lag_dlb_load_weight;
    uint8 *hgt_lag_dlb_qsize_weight;
    soc_profile_mem_t *hgt_lag_dlb_quality_map_profile;
#ifdef BCM_WARM_BOOT_SUPPORT
    uint8 recovered_from_scache; /* Indicates if the following members of this
                                    structure have been recovered from scached:
                                    hgt_lag_dlb_sample_rate,
                                    hgt_lag_dlb_tx_load_min_th,
                                    hgt_lag_dlb_tx_load_max_th,
                                    hgt_lag_dlb_qsize_min_th,
                                    hgt_lag_dlb_qsize_max_th,
                                    hgt_lag_dlb_physical_qsize_min_th,
                                    hgt_lag_dlb_physical_qsize_max_th,
                                    hgt_lag_dlb_load_weight,
                                    hgt_lag_dlb_qsize_weight*/
#endif /* BCM_WARM_BOOT_SUPPORT */
} _th2_hgt_lag_dlb_bookkeeping_t;

static _th2_hgt_lag_dlb_bookkeeping_t *_th2_hgt_lag_dlb_bk[BCM_MAX_NUM_UNITS];

#define _BCM_HGT_LAG_DLB_CELL_BYTES 208
#define _BCM_HGT_LAG_DLB_FLOWSET_BLOCK_SIZE_SHIFT 9
#define _BCM_HGT_LAG_DLB_DEFAULT_SCALING_FACTOR        10
#define _BCM_HGT_LAG_DLB_DEFAULT_PORT_LOAD_WEIGHT      100
#define _BCM_HGT_LAG_DLB_DEFAULT_QUEUE_SIZE_WEIGHT     0

#define HGT_LAG_DLB_INFO(_unit_) (_th2_hgt_lag_dlb_bk[_unit_])

#define _BCM_HGT_LAG_DLB_ID_USED_GET(_u_, _idx_) \
    SHR_BITGET(HGT_LAG_DLB_INFO(_u_)->hgt_lag_dlb_id_used_bitmap, _idx_)
#define _BCM_HGT_LAG_DLB_ID_USED_SET(_u_, _idx_) \
    SHR_BITSET(HGT_LAG_DLB_INFO(_u_)->hgt_lag_dlb_id_used_bitmap, _idx_)
#define _BCM_HGT_LAG_DLB_ID_USED_CLR(_u_, _idx_) \
    SHR_BITCLR(HGT_LAG_DLB_INFO(_u_)->hgt_lag_dlb_id_used_bitmap, _idx_)

#define _BCM_HGT_LAG_DLB_FLOWSET_BLOCK_USED_GET(_u_, _idx_) \
    SHR_BITGET(HGT_LAG_DLB_INFO(_u_)->hgt_lag_dlb_flowset_block_bitmap, _idx_)
#define _BCM_HGT_LAG_DLB_FLOWSET_BLOCK_USED_SET(_u_, _idx_) \
    SHR_BITSET(HGT_LAG_DLB_INFO(_u_)->hgt_lag_dlb_flowset_block_bitmap, _idx_)
#define _BCM_HGT_LAG_DLB_FLOWSET_BLOCK_USED_CLR(_u_, _idx_) \
    SHR_BITCLR(HGT_LAG_DLB_INFO(_u_)->hgt_lag_dlb_flowset_block_bitmap, _idx_)
#define _BCM_HGT_LAG_DLB_FLOWSET_BLOCK_USED_SET_RANGE(_u_, _idx_, _count_) \
    SHR_BITSET_RANGE(HGT_LAG_DLB_INFO(_u_)->hgt_lag_dlb_flowset_block_bitmap, _idx_, _count_)
#define _BCM_HGT_LAG_DLB_FLOWSET_BLOCK_USED_CLR_RANGE(_u_, _idx_, _count_) \
    SHR_BITCLR_RANGE(HGT_LAG_DLB_INFO(_u_)->hgt_lag_dlb_flowset_block_bitmap, _idx_, _count_)
#define _BCM_HGT_LAG_DLB_FLOWSET_BLOCK_TEST_RANGE(_u_, _idx_, _count_, _result_) \
    SHR_BITTEST_RANGE(HGT_LAG_DLB_INFO(_u_)->hgt_lag_dlb_flowset_block_bitmap, _idx_, _count_, _result_)

/*
 * Function:
 *      _bcm_th2_hgt_lag_dlb_quality_assign
 * Purpose:
 *      Assign quality based on loading, total queue size and Physical queue size.
 * Parameters:
 *      unit - (IN)SOC unit number.
 *      tx_load_percent - (IN) Percent weight of loading in determing quality.
 *      qsize_percent   - (IN) Percent weight of total queue size in determing
 *                             quality.
 *                             The remaining percentage is the weight of Physical
 *                             queue size.
 *      entry_arr - (IN) Array of 512 quality map table entries.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_th2_hgt_lag_dlb_quality_assign(int unit, int tx_load_percent,
                                    int qsize_percent, uint32 *entry_arr)
{
    int quantized_tx_load;
    int quantized_qsize;
    int quantized_physical_qsize;
    int quality;
    int entry_index;
    int physical_qsize_percent = 100 - tx_load_percent - qsize_percent;

    if (entry_arr == NULL) {
        return BCM_E_PARAM;
    }

    for (quantized_tx_load = 0; quantized_tx_load < 8; quantized_tx_load++) {
        for (quantized_physical_qsize = 0; quantized_physical_qsize < 8; quantized_physical_qsize++) {
            for (quantized_qsize = 0; quantized_qsize < 8; quantized_qsize++) {
                quality = (quantized_tx_load * tx_load_percent +
                           quantized_qsize * qsize_percent +
                           quantized_physical_qsize * physical_qsize_percent) / 100;
                entry_index = (quantized_tx_load << 6) +
                            (quantized_physical_qsize << 3) +
                                      quantized_qsize;
                soc_DLB_HGT_LAG_PORT_QUALITY_MAPPINGm_field32_set(unit,
                    &entry_arr[entry_index], ASSIGNED_QUALITYf, quality);
            }
        }
    }

    return BCM_E_NONE;
}



/*
 * Function:
 *      _bcm_th2_hgt_lag_dlb_member_quality_map_set
 * Purpose:
 *      Set per-member HGT_LAG DLB quality mapping table.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      port - (IN) Port, not Gport.
 *      tx_load_percent - (IN) Percent weight of loading in determing quality.
 *      qsize_percent   - (IN) Percent weight of total queue size in determing
 *                             quality.
 *                             The remaining percentage is the weight of Physical
 *                             queue size.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_hgt_lag_dlb_member_quality_map_set(int unit, int port,
                                        int tx_load_percent, int qsize_percent)
{
    soc_profile_mem_t *profile;
    soc_mem_t mem;
    int entry_bytes;
    int entries_per_profile;
    int alloc_size;
    uint32 *entry_arr;
    int rv = BCM_E_NONE;
    void *entries;
    uint32 base_index;
    uint32 old_base_index;
    dlb_hgt_lag_quantize_control_entry_t quantize_control_entry;
    int num_quality_map_profiles;

    profile = _th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_quality_map_profile;
    num_quality_map_profiles = 1 << soc_mem_field_length(unit,
        DLB_HGT_LAG_QUANTIZE_CONTROLm, PORT_QUALITY_MAPPING_PROFILE_PTRf);
    entries_per_profile =
        soc_mem_index_count(unit, DLB_HGT_LAG_PORT_QUALITY_MAPPINGm)/
        num_quality_map_profiles;

    entry_bytes = sizeof(dlb_hgt_lag_port_quality_mapping_entry_t);
    alloc_size  = entries_per_profile * entry_bytes;
    entry_arr   = sal_alloc(alloc_size, "HGT_LAG DLB Quality Map entries");
    if (entry_arr == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(entry_arr, 0, alloc_size);

    /* Assign quality in the entry array */
    rv = _bcm_th2_hgt_lag_dlb_quality_assign(unit, tx_load_percent,
                                         qsize_percent, entry_arr);
    if (BCM_FAILURE(rv)) {
        sal_free(entry_arr);
        return rv;
    }

    mem = DLB_HGT_LAG_PORT_QUALITY_MAPPINGm;
    soc_mem_lock(unit, mem);

    /* Add profile */
    entries = entry_arr;
    rv = soc_profile_mem_add(unit, profile, &entries, entries_per_profile,
                             &base_index);
    sal_free(entry_arr);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, mem);
        return rv;
    }

    /* Update member profile pointer */
    rv = READ_DLB_HGT_LAG_QUANTIZE_CONTROLm(unit, MEM_BLOCK_ANY, port,
                                            &quantize_control_entry);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, mem);
        return rv;
    }
    old_base_index = entries_per_profile * soc_mem_field32_get(unit,
            DLB_HGT_LAG_QUANTIZE_CONTROLm, &quantize_control_entry,
            PORT_QUALITY_MAPPING_PROFILE_PTRf);
    soc_DLB_HGT_LAG_QUANTIZE_CONTROLm_field32_set(unit, &quantize_control_entry,
            PORT_QUALITY_MAPPING_PROFILE_PTRf, base_index / entries_per_profile);
    rv = WRITE_DLB_HGT_LAG_QUANTIZE_CONTROLm(unit, MEM_BLOCK_ALL, port,
            &quantize_control_entry);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, mem);
        return rv;
    }

    /* Delete old profile */
    rv = soc_profile_mem_delete(unit, profile, old_base_index);
    soc_mem_unlock(unit, mem);

    HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_load_weight[base_index/entries_per_profile] =
        tx_load_percent;
    HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_qsize_weight[base_index/entries_per_profile] =
        qsize_percent;

    return rv;
}


/*
 * Function:
 *      _bcm_th2_hgt_lag_dlb_scaling_factor_encode
 * Purpose:
 *      Check valid scaling factors, and encode them.
 * Parameters:
 *      unit      - (IN) SOC unit number.
 *      scaling_factor - (IN) Threshold scaling factor.
 * Returns:
 *      1: check success
 *      0: check fail
 */
STATIC int
_bcm_th2_hgt_lag_dlb_scaling_factor_encode(int unit, int scaling_factor,
                                           int *scaling_factor_hw)
{
    int bcm_factors[] = {10, 25, 40, 50, 75, 100};
    int hw_factors[]  = { 0,  1,  2,  3,  4,   5};
    int i;

    for (i = 0; i < COUNTOF(bcm_factors); i++ ) {
        if (scaling_factor == bcm_factors[i]) {
            break;
        }
    }

    if (i < COUNTOF(bcm_factors)) {
        if (scaling_factor_hw) {
            *scaling_factor_hw = hw_factors[i];
        }
        return 1;
    }
    return 0;
}



/*
 * Function:
 *      _bcm_th2_hgt_lag_dlb_scaling_factor_decode
 * Purpose:
 *      Decode scaling factors.
 * Parameters:
 *      unit      - (IN) SOC unit number.
 *      scaling_factor - (IN) Threshold scaling factor.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_hgt_lag_dlb_scaling_factor_decode(int unit, int scaling_factor_hw,
                                           int *scaling_factor)
{
    int bcm_factors[] = {10, 25, 40, 50, 75, 100};
    int hw_factors[]  = { 0,  1,  2,  3,  4,   5};
    int i;

    for (i = 0; i < COUNTOF(hw_factors); i++ ) {
        if (scaling_factor_hw == hw_factors[i]) {
            break;
        }
    }

    if (i == COUNTOF(hw_factors)) {
        return BCM_E_INTERNAL;
    }

    *scaling_factor = bcm_factors[i];
    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_th2_hgt_lag_dlb_random_seed_set
 * Purpose:
 *      Set HGT_LAG DLB random seed.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      seed - (IN) Random seed.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_hgt_lag_dlb_random_seed_set(int unit, int seed)
{

    uint32 rval;
    uint32 hw_limit;

    hw_limit = (1 << soc_reg_field_length(unit, DLB_HGT_LAG_RANDOM_SELECTION_CONTROLr,
                                          SEEDf)) - 1;
    if (seed < 0 ||
        seed > hw_limit) {
        /* Hardware limits */
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(READ_DLB_HGT_LAG_RANDOM_SELECTION_CONTROLr(unit, &rval));
    soc_reg_field_set(unit, DLB_HGT_LAG_RANDOM_SELECTION_CONTROLr, &rval, SEEDf,
                      seed);
    BCM_IF_ERROR_RETURN(WRITE_DLB_HGT_LAG_RANDOM_SELECTION_CONTROLr(unit, rval));

    return BCM_E_NONE;
}




/*
 * Function:
 *      _bcm_th2_hgt_lag_dlb_random_seed_get
 * Purpose:
 *      Set HGT_LAG DLB random seed.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      seed - (IN) Random seed.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_hgt_lag_dlb_random_seed_get(int unit, int *seed)
{

    uint32 rval;

    BCM_IF_ERROR_RETURN(READ_DLB_HGT_LAG_RANDOM_SELECTION_CONTROLr(unit, &rval));
    *seed = soc_reg_field_get(unit, DLB_HGT_LAG_RANDOM_SELECTION_CONTROLr, rval,
                              SEEDf);
    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_th2_hgt_lag_dlb_sample_rate_thresholds_set
 * Purpose:
 *      Set HGT_LAG DLB sample period and physical link accounting thresholds.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      sample_rate - (IN) Number of samplings per second.
 *      min_th      - (IN) Minimum port load threshold, in mbps.
 *      max_th      - (IN) Maximum port load threshold, in mbps.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_hgt_lag_dlb_sample_rate_thresholds_set(int unit, int sample_rate,
        int min_th, int max_th)
{
    int num_time_units;
    uint32 measure_control_reg;
    int max_th_bytes;
    int th_increment;
    dlb_hgt_lag_glb_quantize_threshold_entry_t quantize_threshold_entry;
    int i;
    int th_bytes[7];
    uint32 hw_limit;

    if (sample_rate <= 0 || min_th < 0 || max_th < 0) {
        return BCM_E_PARAM;
    }

    if (min_th > max_th) {
        max_th = min_th;
    }

    /* Compute sampling period in units of 1us:
     *     number of 1us time units = 10^6 / sample_rate
     */
    num_time_units = 1000000 / sample_rate;
    hw_limit = (1 << soc_reg_field_length(unit, DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr,
                                          SAMPLING_PERIODf)) - 1;
    if (num_time_units < 1 ||
        num_time_units > hw_limit) {
        /* Hardware limits */
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN
        (READ_DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    soc_reg_field_set(unit, DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr,
            &measure_control_reg, SAMPLING_PERIODf, num_time_units);
    BCM_IF_ERROR_RETURN
        (WRITE_DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr(unit, measure_control_reg));
    HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_sample_rate = sample_rate;

    /* Compute threshold in bytes per sampling period:
     * bytes per 1us = (million bits per sec) * 10^6 / 8 * 10^(-6),
     * bytes per sampling period = (bytes per 1us) * (number of 1us
     *                             time units in sampling period)
     *                           = mbps * num_time_units / 8
     */
    max_th_bytes = max_th * num_time_units / 8;
    hw_limit = (1 << soc_mem_field_length(unit, DLB_HGT_LAG_GLB_QUANTIZE_THRESHOLDm,
                                          PORT_LOADING_THRESHOLDf)) - 1;
    if (max_th_bytes > hw_limit) {
        /* Hardware limits */
        return BCM_E_PARAM;
    }

    /* Use min threshold as threshold 0, and max threshold as threshold 6.
     * Thresholds 1 to 5 will be evenly spread out between min and max
     * thresholds.
     */
    th_increment = (max_th - min_th) / 6;
    for (i = 0; i < 7; i++) {
        BCM_IF_ERROR_RETURN
            (READ_DLB_HGT_LAG_GLB_QUANTIZE_THRESHOLDm(unit, MEM_BLOCK_ANY, i,
                                              &quantize_threshold_entry));
        th_bytes[i] = (min_th + i * th_increment) * num_time_units / 8;
        soc_DLB_HGT_LAG_GLB_QUANTIZE_THRESHOLDm_field32_set
            (unit, &quantize_threshold_entry, PORT_LOADING_THRESHOLDf,
             th_bytes[i]);
        BCM_IF_ERROR_RETURN
            (WRITE_DLB_HGT_LAG_GLB_QUANTIZE_THRESHOLDm(unit, MEM_BLOCK_ALL, i,
                                               &quantize_threshold_entry));
    }

    HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_tx_load_min_th = min_th;
    HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_tx_load_max_th = max_th;

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_th2_hgt_lag_dlb_sample_rate_set
 * Purpose:
 *      Set HGT_LAG dynamic load balancing sample rate.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      sample_rate - (IN) Number of samplings per second.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_hgt_lag_dlb_sample_rate_set(int unit, int sample_rate)
{
    BCM_IF_ERROR_RETURN(_bcm_th2_hgt_lag_dlb_sample_rate_thresholds_set(unit,
                sample_rate,
                HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_tx_load_min_th,
                HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_tx_load_max_th));

    return BCM_E_NONE;
}



/*
 * Function:
 *      _bcm_th2_hgt_lag_dlb_tx_load_min_th_set
 * Purpose:
 *      Set HGT_LAG DLB port load minimum threshold.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      min_th - (IN) Minimum port loading threshold.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_hgt_lag_dlb_tx_load_min_th_set(int unit, int min_th)
{
    BCM_IF_ERROR_RETURN(_bcm_th2_hgt_lag_dlb_sample_rate_thresholds_set(unit,
                HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_sample_rate,
                min_th,
                HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_tx_load_max_th));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_trident_hgt_lag_dlb_tx_load_max_th_set
 * Purpose:
 *      Set HGT_LAG DLB port load maximum threshold.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      max_th - (IN) Maximum port loading threshold.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_hgt_lag_dlb_tx_load_max_th_set(int unit, int max_th)
{
    BCM_IF_ERROR_RETURN(_bcm_th2_hgt_lag_dlb_sample_rate_thresholds_set(unit,
                HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_sample_rate,
                HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_tx_load_min_th,
                max_th));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_hgt_lag_dlb_qsize_thresholds_set
 * Purpose:
 *      Set HGT_LAG DLB queue size thresholds.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      min_th - (IN) Minimum queue size threshold, in bytes.
 *      max_th - (IN) Maximum queue size threshold, in bytes.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_hgt_lag_dlb_qsize_thresholds_set(int unit, int min_th, int max_th)
{
    int max_th_cells;
    int th_increment;
    dlb_hgt_lag_glb_quantize_threshold_entry_t quantize_threshold_entry;
    int i;
    int th_cells[7];
    uint32 hw_limit;

    if (min_th < 0 || max_th < 0) {
        return BCM_E_PARAM;
    }

    if (min_th > max_th) {
        max_th = min_th;
    }

    /* Convert threshold to number of cells */
    max_th_cells = max_th / _BCM_HGT_LAG_DLB_CELL_BYTES;
    hw_limit = (1 << soc_mem_field_length(unit, DLB_HGT_LAG_GLB_QUANTIZE_THRESHOLDm,
                                          TOTAL_PORT_QSIZE_THRESHOLDf)) - 1;
    if (max_th_cells > hw_limit) {
        /* Hardware limits */
        return BCM_E_PARAM;
    }

    /* Use min threshold as threshold 0, and max threshold as threshold 6.
     * Thresholds 1 to 5 will be evenly spread out between min and max
     * thresholds.
     */
    th_increment = (max_th - min_th) / 6;
    for (i = 0; i < 7; i++) {
        BCM_IF_ERROR_RETURN
            (READ_DLB_HGT_LAG_GLB_QUANTIZE_THRESHOLDm(unit, MEM_BLOCK_ANY, i,
                                              &quantize_threshold_entry));
        th_cells[i] = (min_th + i * th_increment) / _BCM_HGT_LAG_DLB_CELL_BYTES;
        soc_DLB_HGT_LAG_GLB_QUANTIZE_THRESHOLDm_field32_set
            (unit, &quantize_threshold_entry, TOTAL_PORT_QSIZE_THRESHOLDf,
             th_cells[i]);
        BCM_IF_ERROR_RETURN
            (WRITE_DLB_HGT_LAG_GLB_QUANTIZE_THRESHOLDm(unit, MEM_BLOCK_ALL, i,
                                               &quantize_threshold_entry));
    }

    HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_qsize_min_th = min_th;
    HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_qsize_max_th = max_th;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_hgt_lag_dlb_qsize_min_th_set
 * Purpose:
 *      Set HGT_LAG DLB queue size minimum threshold.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      min_th - (IN) Minimum queue size threshold, in bytes.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_hgt_lag_dlb_qsize_min_th_set(int unit, int min_th)
{
    BCM_IF_ERROR_RETURN(_bcm_th2_hgt_lag_dlb_qsize_thresholds_set(unit,
                min_th, HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_qsize_max_th));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_hgt_lag_dlb_qsize_max_th_set
 * Purpose:
 *      Set HGT_LAG DLB queue size maximum threshold.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      max_th - (IN) Maximum queue size threshold, in bytes.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_hgt_lag_dlb_qsize_max_th_set(int unit, int max_th)
{
    BCM_IF_ERROR_RETURN(_bcm_th2_hgt_lag_dlb_qsize_thresholds_set(unit,
                HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_qsize_min_th, max_th));

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_th2_hgt_lag_dlb_physical_qsize_thresholds_set
 * Purpose:
 *      Set HGT_LAG DLB Physical queue size thresholds.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      min_th - (IN) Minimum Physical queue size threshold, in bytes.
 *      max_th - (IN) Maximum Physical queue size threshold, in bytes.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_hgt_lag_dlb_physical_qsize_thresholds_set(int unit, int min_th, int max_th)
{
    int max_th_cells;
    int th_increment;
    dlb_hgt_lag_glb_quantize_threshold_entry_t quantize_threshold_entry;
    int i;
    int th_cells[7];
    uint32 hw_limit;

    if (min_th < 0 || max_th < 0) {
        return BCM_E_PARAM;
    }

    if (min_th > max_th) {
        max_th = min_th;
    }

    /* Convert threshold to number of cells */
    max_th_cells = max_th / _BCM_HGT_LAG_DLB_CELL_BYTES;
    hw_limit = (1 << soc_mem_field_length(unit, DLB_HGT_LAG_GLB_QUANTIZE_THRESHOLDm,
                                          XPE_PORT_QSIZE_THRESHOLDf)) - 1;
    if (max_th_cells > hw_limit) {
        /* Hardware limits */
        return BCM_E_PARAM;
    }

    /* Use min threshold as threshold 0, and max threshold as threshold 6.
     * Thresholds 1 to 5 will be evenly spread out between min and max
     * thresholds.
     */
    th_increment = (max_th - min_th) / 6;
    for (i = 0; i < 7; i++) {
        BCM_IF_ERROR_RETURN
            (READ_DLB_HGT_LAG_GLB_QUANTIZE_THRESHOLDm(unit, MEM_BLOCK_ANY, i,
                                              &quantize_threshold_entry));
        th_cells[i] = (min_th + i * th_increment) / _BCM_HGT_LAG_DLB_CELL_BYTES;
        soc_DLB_HGT_LAG_GLB_QUANTIZE_THRESHOLDm_field32_set
            (unit, &quantize_threshold_entry, XPE_PORT_QSIZE_THRESHOLDf,
             th_cells[i]);
        BCM_IF_ERROR_RETURN
            (WRITE_DLB_HGT_LAG_GLB_QUANTIZE_THRESHOLDm(unit, MEM_BLOCK_ALL, i,
                                               &quantize_threshold_entry));
    }

    HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_physical_qsize_min_th = min_th;
    HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_physical_qsize_max_th = max_th;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_hgt_lag_dlb_physical_qsize_min_th_set
 * Purpose:
 *      Set HGT_LAG DLB Physical queue size minimum threshold.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      min_th - (IN) Minimum Physical queue size threshold, in bytes.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_hgt_lag_dlb_physical_qsize_min_th_set(int unit, int min_th)
{
    BCM_IF_ERROR_RETURN(_bcm_th2_hgt_lag_dlb_physical_qsize_thresholds_set(unit,
                min_th, HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_physical_qsize_max_th));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_hgt_lag_dlb_physical_qsize_max_th_set
 * Purpose:
 *      Set HGT_LAG DLB Physical queue size maximum threshold.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      max_th - (IN) Maximum Physical queue size threshold, in bytes.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_hgt_lag_dlb_physical_qsize_max_th_set(int unit, int max_th)
{
    BCM_IF_ERROR_RETURN(_bcm_th2_hgt_lag_dlb_physical_qsize_thresholds_set(unit,
                HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_physical_qsize_min_th, max_th));

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_th2_hgt_lag_dlb_tx_load_weight_set
 * Purpose:
 *      Set HGT_LAG DLB port load weight.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      weight - (IN) Port load weight.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_hgt_lag_dlb_tx_load_weight_set(int unit, int weight)
{
    uint32 measure_control_reg;
    uint32 hw_limit;

    hw_limit = (1 << soc_reg_field_length(unit, DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr,
                                          PORT_LOADING_WEIGHTf)) - 1;
    if (weight < 0 ||
        weight > hw_limit) {
        /* Hardware limits */
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (READ_DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    soc_reg_field_set(unit, DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr,
        &measure_control_reg, PORT_LOADING_WEIGHTf, weight);
    BCM_IF_ERROR_RETURN
        (WRITE_DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr(unit, measure_control_reg));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_hgt_lag_dlb_tx_load_weight_get
 * Purpose:
 *      Get HGT_LAG DLB port load weight.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      weight - (OUT) Port load weight.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_hgt_lag_dlb_tx_load_weight_get(int unit, int *weight)
{
    uint32 measure_control_reg;

    BCM_IF_ERROR_RETURN
        (READ_DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    *weight = soc_reg_field_get(unit, DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr,
        measure_control_reg, PORT_LOADING_WEIGHTf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_hgt_lag_dlb_qsize_weight_set
 * Purpose:
 *      Set HGT_LAG DLB qsize weight.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      weight - (IN) Qsize weight.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_hgt_lag_dlb_qsize_weight_set(int unit, int weight)
{
    uint32 measure_control_reg;
    uint32 hw_limit;

    hw_limit = (1 << soc_reg_field_length(unit, DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr,
                                          TOTAL_QSIZE_WEIGHTf)) - 1;
    if (weight < 0 ||
        weight > hw_limit) {
        /* Hardware limits */
        return BCM_E_PARAM;
    }

    if (!soc_feature(unit, soc_feature_td3_dlb)) {
        BCM_IF_ERROR_RETURN
            (READ_DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
        soc_reg_field_set(unit, DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr,
                &measure_control_reg, TOTAL_QSIZE_WEIGHTf, weight);
        BCM_IF_ERROR_RETURN
            (WRITE_DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr(unit, measure_control_reg));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_hgt_lag_dlb_qsize_weight_get
 * Purpose:
 *      Get HGT_LAG DLB qsize weight.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      weight - (OUT) Qsize weight.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_hgt_lag_dlb_qsize_weight_get(int unit, int *weight)
{
    uint32 measure_control_reg;

    BCM_IF_ERROR_RETURN
        (READ_DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    *weight = soc_reg_field_get(unit, DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr,
        measure_control_reg, TOTAL_QSIZE_WEIGHTf);

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_th2_hgt_lag_dlb_physical_qsize_weight_set
 * Purpose:
 *      Set HGT_LAG DLB Physical qsize weight.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      weight - (IN) Physical Queue size weight.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_hgt_lag_dlb_physical_qsize_weight_set(int unit, int weight)
{
    uint32 measure_control_reg;
    uint32 hw_limit;

    hw_limit = (1 << soc_reg_field_length(unit, DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr,
                                          XPE_PORT_QSIZE_WEIGHTf)) - 1;

    if (weight < 0 ||
        weight > hw_limit) {
        /* Hardware limits */
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (READ_DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    soc_reg_field_set(unit, DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr,
        &measure_control_reg, XPE_PORT_QSIZE_WEIGHTf, weight);
    BCM_IF_ERROR_RETURN
        (WRITE_DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr(unit, measure_control_reg));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_hgt_lag_dlb_physical_qsize_weight_get
 * Purpose:
 *      Get HGT_LAG DLB Physical qsize weight.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      weight - (OUT) Physical Queue size weight.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_hgt_lag_dlb_physical_qsize_weight_get(int unit, int *weight)
{
    uint32 measure_control_reg;

    BCM_IF_ERROR_RETURN
        (READ_DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    *weight = soc_reg_field_get(unit, DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr,
        measure_control_reg, XPE_PORT_QSIZE_WEIGHTf);

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_th2_hgt_lag_dlb_tx_load_cap_set
 * Purpose:
 *      Set HGT_LAG DLB port load cap control.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      cap - (IN) Port load cap control.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_hgt_lag_dlb_tx_load_cap_set(int unit, int cap)
{
    uint32 measure_control_reg;

    if (cap < 0 ||
        cap > 1) {
        /* Hardware limits port load cap control to 1 bit */
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (READ_DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    soc_reg_field_set(unit, DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr,
        &measure_control_reg, CAP_PORT_LOADING_AVERAGEf, cap);
    BCM_IF_ERROR_RETURN
        (WRITE_DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr(unit, measure_control_reg));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_hgt_lag_dlb_tx_load_cap_get
 * Purpose:
 *      Get HGT_LAG DLB port load cap control.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      cap - (OUT) Cap control.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_hgt_lag_dlb_tx_load_cap_get(int unit, int *cap)
{
    uint32 measure_control_reg;

    BCM_IF_ERROR_RETURN
        (READ_DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    *cap= soc_reg_field_get(unit, DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr,
        measure_control_reg, CAP_PORT_LOADING_AVERAGEf);

    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_th2_hgt_lag_dlb_qsize_cap_set
 * Purpose:
 *      Set HGT_LAG DLB queue size cap control.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      cap - (IN) Queue size cap control.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_hgt_lag_dlb_qsize_cap_set(int unit, int cap)
{
    uint32 measure_control_reg;

    if (cap < 0 ||
        cap > 1) {
        /* Hardware limits port load cap control to 1 bit */
        return BCM_E_PARAM;
    }

    if (!soc_feature(unit, soc_feature_td3_dlb)) {
        BCM_IF_ERROR_RETURN
            (READ_DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
        soc_reg_field_set(unit, DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr,
                &measure_control_reg, CAP_TOTAL_PORT_QSIZE_AVERAGEf, cap);
        BCM_IF_ERROR_RETURN
            (WRITE_DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr(unit, measure_control_reg));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_hgt_lag_dlb_qsize_cap_get
 * Purpose:
 *      Get HGT_LAG DLB queue size cap control.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      cap - (OUT) Cap control.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_hgt_lag_dlb_qsize_cap_get(int unit, int *cap)
{
    uint32 measure_control_reg;

    BCM_IF_ERROR_RETURN
        (READ_DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    *cap= soc_reg_field_get(unit, DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr,
        measure_control_reg, CAP_TOTAL_PORT_QSIZE_AVERAGEf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_hgt_lag_dlb_physical_qsize_cap_set
 * Purpose:
 *      Set HGT_LAG DLB Physical queue size cap control.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      cap - (IN) Physical Queue size cap control.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_hgt_lag_dlb_physical_qsize_cap_set(int unit, int cap)
{
    uint32 measure_control_reg;

    if (cap < 0 ||
        cap > 1) {
        /* Hardware limits port load cap control to 1 bit */
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (READ_DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    soc_reg_field_set(unit, DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr,
        &measure_control_reg, CAP_XPE_PORT_QSIZE_AVERAGEf, cap);
    BCM_IF_ERROR_RETURN
        (WRITE_DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr(unit, measure_control_reg));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_hgt_lag_dlb_physical_qsize_cap_get
 * Purpose:
 *      Get HGT_LAG DLB Physical queue size cap control.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      cap - (OUT) Physical Queue size cap control.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_hgt_lag_dlb_physical_qsize_cap_get(int unit, int *cap)
{
    uint32 measure_control_reg;

    BCM_IF_ERROR_RETURN
        (READ_DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    *cap= soc_reg_field_get(unit, DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr,
        measure_control_reg, CAP_XPE_PORT_QSIZE_AVERAGEf);

    return BCM_E_NONE;
}



/*
 * Function:
 *      bcm_th2_hgt_lag_dlb_config_set
 * Purpose:
 *      Set per-chip HGT_LAG dynamic load balancing configuration parameters.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      type - (IN) Configuration parameter type.
 *      arg  - (IN) Configuration paramter value.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_th2_hgt_lag_dlb_config_set(int unit, bcm_switch_control_t type, int arg)
{
    switch (type) {
        case bcmSwitchTrunkDynamicSampleRate:
        case bcmSwitchFabricTrunkDynamicSampleRate:
            return _bcm_th2_hgt_lag_dlb_sample_rate_set(unit, arg);

        case bcmSwitchTrunkDynamicEgressBytesExponent:
        case bcmSwitchFabricTrunkDynamicEgressBytesExponent:
            return _bcm_th2_hgt_lag_dlb_tx_load_weight_set(unit, arg);

        case bcmSwitchTrunkDynamicQueuedBytesExponent:
        case bcmSwitchFabricTrunkDynamicQueuedBytesExponent:
            if (soc_feature(unit, soc_feature_td3_dlb)) {
                return BCM_E_UNAVAIL;
            } else {
                return _bcm_th2_hgt_lag_dlb_qsize_weight_set(unit, arg);
            }
        case bcmSwitchTrunkDynamicPhysicalQueuedBytesExponent:
        case bcmSwitchFabricTrunkDynamicPhysicalQueuedBytesExponent:
            return _bcm_th2_hgt_lag_dlb_physical_qsize_weight_set(unit, arg);

        case bcmSwitchTrunkDynamicEgressBytesDecreaseReset:
        case bcmSwitchFabricTrunkDynamicEgressBytesDecreaseReset:
            return _bcm_th2_hgt_lag_dlb_tx_load_cap_set(unit, arg);

        case bcmSwitchTrunkDynamicQueuedBytesDecreaseReset:
        case bcmSwitchFabricTrunkDynamicQueuedBytesDecreaseReset:
            if (soc_feature(unit, soc_feature_td3_dlb)) {
                return BCM_E_UNAVAIL;
            } else {
                return _bcm_th2_hgt_lag_dlb_qsize_cap_set(unit, arg);
            }
        case bcmSwitchTrunkDynamicPhysicalQueuedBytesDecreaseReset:
        case bcmSwitchFabricTrunkDynamicPhysicalQueuedBytesDecreaseReset:
            return _bcm_th2_hgt_lag_dlb_physical_qsize_cap_set(unit, arg);

        case bcmSwitchTrunkDynamicEgressBytesMinThreshold:
        case bcmSwitchFabricTrunkDynamicEgressBytesMinThreshold:
            return _bcm_th2_hgt_lag_dlb_tx_load_min_th_set(unit, arg);

        case bcmSwitchTrunkDynamicEgressBytesMaxThreshold:
        case bcmSwitchFabricTrunkDynamicEgressBytesMaxThreshold:
            return _bcm_th2_hgt_lag_dlb_tx_load_max_th_set(unit, arg);

        case bcmSwitchTrunkDynamicQueuedBytesMinThreshold:
        case bcmSwitchFabricTrunkDynamicQueuedBytesMinThreshold:
            if (soc_feature(unit, soc_feature_td3_dlb)) {
                return BCM_E_UNAVAIL;
            } else {
                return _bcm_th2_hgt_lag_dlb_qsize_min_th_set(unit, arg);
            }
        case bcmSwitchTrunkDynamicQueuedBytesMaxThreshold:
        case bcmSwitchFabricTrunkDynamicQueuedBytesMaxThreshold:
            if (soc_feature(unit, soc_feature_td3_dlb)) {
                return BCM_E_UNAVAIL;
            } else {
                return _bcm_th2_hgt_lag_dlb_qsize_max_th_set(unit, arg);
            }
        case bcmSwitchTrunkDynamicPhysicalQueuedBytesMinThreshold:
        case bcmSwitchFabricTrunkDynamicPhysicalQueuedBytesMinThreshold:
            return _bcm_th2_hgt_lag_dlb_physical_qsize_min_th_set(unit, arg);

        case bcmSwitchTrunkDynamicPhysicalQueuedBytesMaxThreshold:
        case bcmSwitchFabricTrunkDynamicPhysicalQueuedBytesMaxThreshold:
            return _bcm_th2_hgt_lag_dlb_physical_qsize_max_th_set(unit, arg);

        case bcmSwitchTrunkDynamicRandomSeed:
        case bcmSwitchFabricTrunkDynamicRandomSeed:
            return _bcm_th2_hgt_lag_dlb_random_seed_set(unit, arg);

        default:
            return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th2_hgt_lag_dlb_config_get
 * Purpose:
 *      Get per-chip HGT_LAG dynamic load balancing configuration parameters.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      type - (IN) Configuration parameter type.
 *      arg  - (OUT) Configuration paramter value.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_th2_hgt_lag_dlb_config_get(int unit, bcm_switch_control_t type, int *arg)
{
    switch (type) {
        case bcmSwitchTrunkDynamicSampleRate:
        case bcmSwitchFabricTrunkDynamicSampleRate:
            *arg = HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_sample_rate;
            break;

        case bcmSwitchTrunkDynamicEgressBytesExponent:
        case bcmSwitchFabricTrunkDynamicEgressBytesExponent:
            return _bcm_th2_hgt_lag_dlb_tx_load_weight_get(unit, arg);

        case bcmSwitchTrunkDynamicQueuedBytesExponent:
        case bcmSwitchFabricTrunkDynamicQueuedBytesExponent:
            if (soc_feature(unit, soc_feature_td3_dlb)) {
                return BCM_E_UNAVAIL;
            } else {
                return _bcm_th2_hgt_lag_dlb_qsize_weight_get(unit, arg);
            }
        case bcmSwitchTrunkDynamicPhysicalQueuedBytesExponent:
        case bcmSwitchFabricTrunkDynamicPhysicalQueuedBytesExponent:
            return _bcm_th2_hgt_lag_dlb_physical_qsize_weight_get(unit, arg);

        case bcmSwitchTrunkDynamicEgressBytesDecreaseReset:
        case bcmSwitchFabricTrunkDynamicEgressBytesDecreaseReset:
            return _bcm_th2_hgt_lag_dlb_tx_load_cap_get(unit, arg);

        case bcmSwitchTrunkDynamicQueuedBytesDecreaseReset:
        case bcmSwitchFabricTrunkDynamicQueuedBytesDecreaseReset:
            if (soc_feature(unit, soc_feature_td3_dlb)) {
                return BCM_E_UNAVAIL;
            } else {
                return _bcm_th2_hgt_lag_dlb_qsize_cap_get(unit, arg);
            }
        case bcmSwitchTrunkDynamicPhysicalQueuedBytesDecreaseReset:
        case bcmSwitchFabricTrunkDynamicPhysicalQueuedBytesDecreaseReset:
            return _bcm_th2_hgt_lag_dlb_physical_qsize_cap_get(unit, arg);

        case bcmSwitchTrunkDynamicEgressBytesMinThreshold:
        case bcmSwitchFabricTrunkDynamicEgressBytesMinThreshold:
            *arg = HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_tx_load_min_th;
            break;

        case bcmSwitchTrunkDynamicEgressBytesMaxThreshold:
        case bcmSwitchFabricTrunkDynamicEgressBytesMaxThreshold:
            *arg = HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_tx_load_max_th;
            break;

        case bcmSwitchTrunkDynamicQueuedBytesMinThreshold:
        case bcmSwitchFabricTrunkDynamicQueuedBytesMinThreshold:
            *arg = HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_qsize_min_th;
            break;

        case bcmSwitchTrunkDynamicQueuedBytesMaxThreshold:
        case bcmSwitchFabricTrunkDynamicQueuedBytesMaxThreshold:
            *arg = HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_qsize_max_th;
            break;

        case bcmSwitchTrunkDynamicPhysicalQueuedBytesMinThreshold:
        case bcmSwitchFabricTrunkDynamicPhysicalQueuedBytesMinThreshold:
            *arg = HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_physical_qsize_min_th;
            break;

        case bcmSwitchTrunkDynamicPhysicalQueuedBytesMaxThreshold:
        case bcmSwitchFabricTrunkDynamicPhysicalQueuedBytesMaxThreshold:
            *arg = HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_physical_qsize_max_th;
            break;

        case bcmSwitchTrunkDynamicRandomSeed:
        case bcmSwitchFabricTrunkDynamicRandomSeed:
            return _bcm_th2_hgt_lag_dlb_random_seed_get(unit, arg);

        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_hgt_lag_dlb_dynamic_size_encode
 * Purpose:
 *      Encode HGT_LAG dynamic load balancing flow set size.
 * Parameters:
 *      dynamic_size - (IN) Number of flow sets.
 *      encoded_value - (OUT) Encoded value.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_th2_hgt_lag_dlb_dynamic_size_encode(int dynamic_size,
        int *encoded_value)
{
    switch (dynamic_size) {
        case 512:
            *encoded_value = 1;
            break;
        case 1024:
            *encoded_value = 2;
            break;
        case 2048:
            *encoded_value = 3;
            break;
        case 4096:
            *encoded_value = 4;
            break;
        case 8192:
            *encoded_value = 5;
            break;
        case 16384:
            *encoded_value = 6;
            break;
        case 32768:
            *encoded_value = 7;
            break;
        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_hgt_lag_dlb_dynamic_size_decode
 * Purpose:
 *      Decode HGT_LAG trunk dynamic load balancing flow set size.
 * Parameters:
 *      encoded_value - (IN) Encoded value.
 *      dynamic_size - (OUT) Number of flow sets.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_hgt_lag_dlb_dynamic_size_decode(int encoded_value,
        int *dynamic_size)
{
    switch (encoded_value) {
        case 1:
            *dynamic_size = 512;
            break;
        case 2:
            *dynamic_size = 1024;
            break;
        case 3:
            *dynamic_size = 2048;
            break;
        case 4:
            *dynamic_size = 4096;
            break;
        case 5:
            *dynamic_size = 8192;
            break;
        case 6:
            *dynamic_size = 16384;
            break;
        case 7:
            *dynamic_size = 32768;
            break;
        default:
            return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_hgt_lag_dlb_id_alloc
 * Purpose:
 *      Get a free HGT_LAG DLB ID and mark it used.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      dlb_id - (OUT) Allocated DLB ID.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_hgt_lag_dlb_id_alloc(int unit, int *dlb_id)
{
    int i;

    for (i = 0; i < soc_mem_index_count(unit, DLB_HGT_LAG_GROUP_CONTROLm); i++) {
        if (!_BCM_HGT_LAG_DLB_ID_USED_GET(unit, i)) {
            _BCM_HGT_LAG_DLB_ID_USED_SET(unit, i);
            *dlb_id = i;
            return BCM_E_NONE;
        }
    }

    return BCM_E_RESOURCE;
}

/*
 * Function:
 *      _bcm_th2_hgt_lag_dlb_id_free
 * Purpose:
 *      Free a HGT_LAG DLB ID.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      dlb_id - (IN) DLB ID to be freed.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_hgt_lag_dlb_id_free(int unit, int dlb_id)
{
    if (dlb_id < 0 ||
        dlb_id > soc_mem_index_max(unit, DLB_HGT_LAG_GROUP_CONTROLm)) {
        return BCM_E_PARAM;
    }

    _BCM_HGT_LAG_DLB_ID_USED_CLR(unit, dlb_id);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th2_hgt_lag_dlb_member_attr_set
 * Purpose:
 *      Set each HGT_LAG member's attributes.
 * Parameters:
 *      unit   - (IN) SOC unit number.
 *      num_dlb_ports  - (IN) Number of DLB ports.
 *      port_array     - (IN) Array of port numbers.
 *      scaling_factor_array - (IN) Array of scaling factors.
 *      load_weight_array    - (IN) Array of load weights.
 *      qsize_weight_array    - (IN) Array of Queue size weights.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcm_th2_hgt_lag_dlb_member_attr_set(int unit,
                                    bcm_port_t port,
                                    int scaling_factor,
                                    int load_weight,
                                    int qsize_weight)
{
    int scaling_factor_hw;
    dlb_hgt_lag_quantize_control_entry_t quantize_control_entry;

    /* Set member quality mapping profile */

    BCM_IF_ERROR_RETURN(_bcm_th2_hgt_lag_dlb_member_quality_map_set(unit,
                port, load_weight, qsize_weight));

    /* Set threshold scaling factors */

    if (!_bcm_th2_hgt_lag_dlb_scaling_factor_encode(unit,
             scaling_factor, &scaling_factor_hw)) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(READ_DLB_HGT_LAG_QUANTIZE_CONTROLm(unit, MEM_BLOCK_ANY,
                port, &quantize_control_entry));
    soc_DLB_HGT_LAG_QUANTIZE_CONTROLm_field32_set(unit,
            &quantize_control_entry,
            PORT_LOADING_THRESHOLD_SCALING_FACTORf,
            scaling_factor_hw);
    if (!soc_feature(unit, soc_feature_td3_dlb)) {
        soc_DLB_HGT_LAG_QUANTIZE_CONTROLm_field32_set(unit,
                &quantize_control_entry,
                TOTAL_PORT_QSIZE_THRESHOLD_SCALING_FACTORf,
                scaling_factor_hw);
    }
    soc_DLB_HGT_LAG_QUANTIZE_CONTROLm_field32_set(unit,
            &quantize_control_entry,
            XPE_PORT_QSIZE_THRESHOLD_SCALING_FACTORf,
            scaling_factor_hw);
    BCM_IF_ERROR_RETURN(WRITE_DLB_HGT_LAG_QUANTIZE_CONTROLm(unit, MEM_BLOCK_ALL,
                port, &quantize_control_entry));

    return BCM_E_NONE;
}
/*
 * Function:
 *      bcm_th2_hgt_lag_dlb_member_attr_get
 * Purpose:
 *      Get attributes of a HGT_LAG DLB member.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      port - (IN) Port number of the member.
 *      scaling_factor - (OUT) Member's scaling factor.
 *      load_weight    - (OUT) Member's load weight.
 *      qsize_weight   - (OUT) Member's Queue size weight.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_th2_hgt_lag_dlb_member_attr_get(int unit, bcm_port_t port,
        int *scaling_factor, int *load_weight, int *qsize_weight)
{
    dlb_hgt_lag_quantize_control_entry_t quantize_control_entry;
    int profile_ptr;
    int scaling_factor_hw;

    BCM_IF_ERROR_RETURN(READ_DLB_HGT_LAG_QUANTIZE_CONTROLm(unit, MEM_BLOCK_ANY,
                port, &quantize_control_entry));

    /* Get load weight, qsize weight */
    profile_ptr = soc_DLB_HGT_LAG_QUANTIZE_CONTROLm_field32_get(unit,
            &quantize_control_entry, PORT_QUALITY_MAPPING_PROFILE_PTRf);
    *load_weight  = HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_load_weight[profile_ptr];
    *qsize_weight = HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_qsize_weight[profile_ptr];

    /* Get scaling factor */
    scaling_factor_hw = soc_DLB_HGT_LAG_QUANTIZE_CONTROLm_field32_get(unit,
            &quantize_control_entry, PORT_LOADING_THRESHOLD_SCALING_FACTORf);
    BCM_IF_ERROR_RETURN
        (_bcm_th2_hgt_lag_dlb_scaling_factor_decode(unit, scaling_factor_hw,
                                                 scaling_factor));

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_th2_hgt_lag_dlb_free_resource
 * Purpose:
 *      Free resources for a HGT_LAG dynamic load balancing group.
 * Parameters:
 *      unit   - (IN) SOC unit number.
 *      tid  - (IN) Trunk/HiGig Trunk group ID.
 *      hgt  - (IN) Is HiGig Trunk
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_th2_hgt_lag_dlb_free_resource(int unit, int tid, int hgt)
{
    int rv = BCM_E_NONE;
    int dlb_enable;
    int dlb_id;
    dlb_hgt_lag_group_control_entry_t dlb_hgt_lag_group_control_entry;
    int entry_base_ptr;
    int flow_set_size;
    int num_entries;
    int block_base_ptr;
    int membership_pointer;
    int num_blocks;
    dlb_hgt_lag_group_membership_entry_t dlb_hgt_lag_group_membership_entry;
    int port_map_width;
    int alloc_size;
    SHR_BITDCL *port_map = NULL;
    SHR_BITDCL *status_bitmap = NULL;
    SHR_BITDCL *override_bitmap = NULL;
    int port;
    hgt_dlb_control_entry_t hgt_dlb_control;
    lag_dlb_control_entry_t lag_dlb_control;
#ifdef BCM_TRIDENT3_SUPPORT
    hg_trunk_mode_entry_t   hgt_trunk_mode;
    trunk_group_entry_t     tg_entry;
#endif
    dlb_hgt_lag_link_control_entry_t link_control_entry;

    if (hgt) {
        BCM_IF_ERROR_RETURN(READ_HGT_DLB_CONTROLm(unit, MEM_BLOCK_ANY,
                tid, &hgt_dlb_control));
        dlb_enable = soc_HGT_DLB_CONTROLm_field32_get(unit, &hgt_dlb_control,
                                                      GROUP_ENABLEf);
        dlb_id = soc_HGT_DLB_CONTROLm_field32_get(unit, &hgt_dlb_control, DLB_IDf);
    } else {
        BCM_IF_ERROR_RETURN(READ_LAG_DLB_CONTROLm(unit, MEM_BLOCK_ANY,
                tid, &lag_dlb_control));
        dlb_enable = soc_LAG_DLB_CONTROLm_field32_get(unit, &lag_dlb_control,
                                                      GROUP_ENABLEf);
        dlb_id = soc_LAG_DLB_CONTROLm_field32_get(unit, &lag_dlb_control, DLB_IDf);
    }
    if (!dlb_enable) {
        return BCM_E_NONE;
    }

    /* Clear DLB fields in HGT/LAG_DLB_CONTROL table */

    if (hgt) {
        soc_HGT_DLB_CONTROLm_field32_set(unit, &hgt_dlb_control, GROUP_ENABLEf, 0);
        soc_HGT_DLB_CONTROLm_field32_set(unit, &hgt_dlb_control, DLB_IDf, 0);
        BCM_IF_ERROR_RETURN
        (WRITE_HGT_DLB_CONTROLm(unit, MEM_BLOCK_ALL, tid, &hgt_dlb_control));
#ifdef BCM_TRIDENT3_SUPPORT
        if (soc_feature(unit, soc_feature_td3_dlb)) {
            BCM_IF_ERROR_RETURN(READ_HG_TRUNK_MODEm(unit, MEM_BLOCK_ANY, tid, &hgt_trunk_mode));
            soc_HG_TRUNK_MODEm_field32_set(unit, &hgt_trunk_mode,
                    HG_TRUNK_LB_MODEf, 0x0);
            BCM_IF_ERROR_RETURN
                (WRITE_HG_TRUNK_MODEm(unit, MEM_BLOCK_ALL, tid,  &hgt_trunk_mode));
        }
#endif
    } else {
        soc_LAG_DLB_CONTROLm_field32_set(unit, &lag_dlb_control, GROUP_ENABLEf, 0);
        soc_LAG_DLB_CONTROLm_field32_set(unit, &lag_dlb_control, DLB_IDf, 0);
        BCM_IF_ERROR_RETURN
        (WRITE_LAG_DLB_CONTROLm(unit, MEM_BLOCK_ALL, tid, &lag_dlb_control));
#ifdef BCM_TRIDENT3_SUPPORT
        if (soc_feature(unit, soc_feature_td3_dlb)) {
            BCM_IF_ERROR_RETURN(READ_TRUNK_GROUPm(unit, MEM_BLOCK_ANY, tid, &tg_entry));
            soc_TRUNK_GROUPm_field32_set(unit, &tg_entry, TRUNK_MODEf, 0x0);
            BCM_IF_ERROR_RETURN
                (WRITE_TRUNK_GROUPm(unit, MEM_BLOCK_ANY, tid, &tg_entry));
        }
#endif
    }

    /* Free flow set table resources */

    BCM_IF_ERROR_RETURN
        (READ_DLB_HGT_LAG_GROUP_CONTROLm(unit, MEM_BLOCK_ANY, dlb_id,
                                         &dlb_hgt_lag_group_control_entry));
    entry_base_ptr = soc_DLB_HGT_LAG_GROUP_CONTROLm_field32_get(unit,
                        &dlb_hgt_lag_group_control_entry, FLOW_SET_BASEf);
    flow_set_size  = soc_DLB_HGT_LAG_GROUP_CONTROLm_field32_get(unit,
                        &dlb_hgt_lag_group_control_entry, FLOW_SET_SIZEf);
    BCM_IF_ERROR_RETURN
        (_bcm_th2_hgt_lag_dlb_dynamic_size_decode(flow_set_size, &num_entries));
    block_base_ptr = entry_base_ptr >> _BCM_HGT_LAG_DLB_FLOWSET_BLOCK_SIZE_SHIFT;
    num_blocks     = num_entries >> _BCM_HGT_LAG_DLB_FLOWSET_BLOCK_SIZE_SHIFT;
    _BCM_HGT_LAG_DLB_FLOWSET_BLOCK_USED_CLR_RANGE(unit, block_base_ptr, num_blocks);

    /* Clear HGT_LAG DLB group control */

    BCM_IF_ERROR_RETURN(WRITE_DLB_HGT_LAG_GROUP_CONTROLm(unit,
                MEM_BLOCK_ALL, dlb_id,
                soc_mem_entry_null(unit, DLB_HGT_LAG_GROUP_CONTROLm)));

    /* Allocate port map */
    port_map_width = soc_mem_field_length(unit,
            DLB_HGT_LAG_GROUP_MEMBERSHIPm, PORT_MAPf);
    alloc_size = SHR_BITALLOCSIZE(port_map_width);
    port_map = sal_alloc(alloc_size, "DLB HGT_LAG port map");
    if (NULL == port_map) {
        return BCM_E_MEMORY;
    }
    sal_memset(port_map, 0, alloc_size);

    membership_pointer = soc_DLB_HGT_LAG_GROUP_CONTROLm_field32_get(unit,
            &dlb_hgt_lag_group_control_entry, GROUP_MEMBERSHIP_POINTERf);
    rv = READ_DLB_HGT_LAG_GROUP_MEMBERSHIPm(unit, MEM_BLOCK_ANY, membership_pointer,
            &dlb_hgt_lag_group_membership_entry);
    if (BCM_FAILURE(rv)) {
        goto error;
    }
    soc_DLB_HGT_LAG_GROUP_MEMBERSHIPm_field_get(unit,
            &dlb_hgt_lag_group_membership_entry, PORT_MAPf, port_map);

    for (port = 0; port < port_map_width; port++) {
        if (!SHR_BITGET(port_map, port)) {
            continue;
        }

        rv = bcm_th2_hgt_lag_dlb_member_attr_set(unit, port,
                                 _BCM_HGT_LAG_DLB_DEFAULT_SCALING_FACTOR,
                                 _BCM_HGT_LAG_DLB_DEFAULT_PORT_LOAD_WEIGHT,
                                 _BCM_HGT_LAG_DLB_DEFAULT_QUEUE_SIZE_WEIGHT);
        if (BCM_FAILURE(rv)) {
            goto error;
        }
    }

    /* Clear member software state */

    rv = READ_DLB_HGT_LAG_LINK_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                        &link_control_entry);
    if (BCM_FAILURE(rv)) {
        goto error;
    }

    status_bitmap = sal_alloc(alloc_size, "DLB HGT_LAG member status bitmap");
    if (NULL == status_bitmap) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    sal_memset(status_bitmap, 0, alloc_size);
    soc_DLB_HGT_LAG_LINK_CONTROLm_field_get(unit,
            &link_control_entry, SW_PORT_STATEf, status_bitmap);
    SHR_BITREMOVE_RANGE(status_bitmap, port_map, 0, port_map_width,
            status_bitmap);
    soc_DLB_HGT_LAG_LINK_CONTROLm_field_set(unit,
            &link_control_entry, SW_PORT_STATEf, status_bitmap);

    override_bitmap = sal_alloc(alloc_size, "DLB HGT_LAG member override bitmap");
    if (NULL == override_bitmap) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    sal_memset(override_bitmap, 0, alloc_size);
    soc_DLB_HGT_LAG_LINK_CONTROLm_field_get(unit,
            &link_control_entry, SW_OVERRIDE_PORT_MAPf, override_bitmap);
    SHR_BITREMOVE_RANGE(override_bitmap, port_map, 0, port_map_width,
            override_bitmap);
    soc_DLB_HGT_LAG_LINK_CONTROLm_field_set(unit,
            &link_control_entry, SW_OVERRIDE_PORT_MAPf, override_bitmap);

    rv = WRITE_DLB_HGT_LAG_LINK_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                         &link_control_entry);
    if (BCM_FAILURE(rv)) {
        goto error;
    }

    sal_free(port_map);
    sal_free(status_bitmap);
    sal_free(override_bitmap);

    /* Clear group membership */
    BCM_IF_ERROR_RETURN
    (WRITE_DLB_HGT_LAG_GROUP_MEMBERSHIPm(unit, MEM_BLOCK_ANY, membership_pointer,
            soc_mem_entry_null(unit, DLB_HGT_LAG_GROUP_MEMBERSHIPm)));

    /* Free DLB ID */
    BCM_IF_ERROR_RETURN(_bcm_th2_hgt_lag_dlb_id_free(unit, dlb_id));

    return BCM_E_NONE;

error:
    if (NULL != port_map) {
        sal_free(port_map);
    }
    if (NULL != status_bitmap) {
        sal_free(status_bitmap);
    }
    if (NULL != override_bitmap) {
        sal_free(override_bitmap);
    }
    return rv;
}



/*
 * Function:
 *      bcm_th2_hgt_lag_dlb_set
 * Purpose:
 *      Configure a HGT_LAG dynamic load balancing group. For Lag.
 * Parameters:
 *      unit       - (IN) SOC unit number.
 *      tid        - (IN) Trunk/HiGig Trunk group ID.
 *      hgt        - (IN) Is HiGig Trunk
 *      add_info   - (IN) Pointer to trunk add info structure.
 *      num_dlb_ports  - (IN) Number of trunk member ports eligible for
 *                        HGT_LAG DLB
 *      mod_array  - (IN) Array of module IDs
 *      port_array - (IN) Array of port IDs
 *      scaling_factor_array - (IN) Array of scaling factors.
 *      load_weight_array    - (IN) Array of load weights.
 *      qsize_weight_array    - (IN) Array of Queue size weights.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_th2_hgt_lag_dlb_set(int unit, int tid, int hgt, _esw_trunk_add_info_t *add_info,
        int num_dlb_ports, int *mod_array, int *port_array,
        int *scaling_factor_array, int *load_weight_array, int *qsize_weight_array)
{
    int rv = BCM_E_NONE;
    int dlb_enable;
    int dlb_id;
    soc_mem_t flowset_mem;
    int flowset_entry_size;
    dlb_hgt_lag_flowset_entry_t *flowset_entry;
    int num_blocks;
    int total_blocks;
    int max_block_base_ptr;
    int block_base_ptr;
    int occupied;
    int entry_base_ptr;
    int num_entries_per_block;
    int port_map_width;
    int alloc_size;
    uint32 *block_ptr = NULL;
    int i, k;
    int index_min, index_max;
    int flow_set_size;
    int dlb_mode;
    SHR_BITDCL *port_map = NULL;
    SHR_BITDCL *status_bitmap = NULL;
    SHR_BITDCL *override_bitmap = NULL;
    uint32 membership_pointer;
    hgt_dlb_control_entry_t hgt_dlb_control;
    lag_dlb_control_entry_t lag_dlb_control;
#ifdef BCM_TRIDENT3_SUPPORT
    hg_trunk_mode_entry_t   hgt_trunk_mode;
    trunk_group_entry_t     tg_entry;
#endif
    dlb_hgt_lag_group_control_entry_t dlb_hgt_lag_group_control_entry;
    dlb_hgt_lag_group_membership_entry_t dlb_hgt_lag_group_membership_entry;
    dlb_hgt_lag_link_control_entry_t link_control_entry;
    int is_local;

    dlb_enable = (add_info->psc == BCM_TRUNK_PSC_DYNAMIC) ||
        (add_info->psc == BCM_TRUNK_PSC_DYNAMIC_ASSIGNED) ||
        (add_info->psc == BCM_TRUNK_PSC_DYNAMIC_OPTIMAL);

    if (!dlb_enable) {
        return BCM_E_NONE;
    }

    /* Both HiGig Trunk and LAG supports local port only,
     * For HiGig Trunk, mod_array[] are all -1.
     */
    if (soc_feature(unit, soc_feature_lag_dlb)) {
        for (i = 0; i< num_dlb_ports; i++) {
            BCM_IF_ERROR_RETURN
                (_bcm_esw_modid_is_local(unit, mod_array[i], &is_local));
            if (!is_local) {
                return BCM_E_PARAM;
            }
        }
    }

    for (i = 0; i < num_dlb_ports; i++) {
        if (!_bcm_th2_hgt_lag_dlb_scaling_factor_encode(unit,
                 scaling_factor_array[i], NULL)) {
            return BCM_E_PARAM;
        }
        if (load_weight_array[i] < 0 ||
            load_weight_array[i] > 100) {
            return BCM_E_PARAM;
        }
        if (qsize_weight_array[i] < 0 ||
            qsize_weight_array[i] > 100) {
            return BCM_E_PARAM;
        }
        if ((load_weight_array[i] + qsize_weight_array[i]) < 0 ||
            (load_weight_array[i] + qsize_weight_array[i]) > 100) {
            return BCM_E_PARAM;
        }
    }

    /* Allocate a DLB ID */
    BCM_IF_ERROR_RETURN(_bcm_th2_hgt_lag_dlb_id_alloc(unit, &dlb_id));

    for (i = 0; i < num_dlb_ports; i++) {
        BCM_IF_ERROR_RETURN
            (bcm_th2_hgt_lag_dlb_member_attr_set(unit, port_array[i],
                                                 scaling_factor_array[i],
                                                 load_weight_array[i],
                                                 qsize_weight_array[i]));
    }

    port_map_width = soc_mem_field_length(unit,
            DLB_HGT_LAG_GROUP_MEMBERSHIPm, PORT_MAPf);
    alloc_size = SHR_BITALLOCSIZE(port_map_width);
    port_map = sal_alloc(alloc_size, "DLB HGT_LAG port map");
    if (NULL == port_map) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    sal_memset(port_map, 0, alloc_size);
    for (i = 0; i < num_dlb_ports; i++) {
        SHR_BITSET(port_map, port_array[i]);
    }

    /* Set member software state */
    rv = READ_DLB_HGT_LAG_LINK_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                &link_control_entry);
    if (BCM_FAILURE(rv)) {
        goto error;
    }
    status_bitmap = sal_alloc(alloc_size, "DLB HGT_LAG member status bitmap");
    if (NULL == status_bitmap) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    sal_memset(status_bitmap, 0, alloc_size);
    soc_DLB_HGT_LAG_LINK_CONTROLm_field_get(unit,
            &link_control_entry, SW_PORT_STATEf, status_bitmap);

    SHR_BITREMOVE_RANGE(status_bitmap, port_map, 0, port_map_width,
            status_bitmap);
    soc_DLB_HGT_LAG_LINK_CONTROLm_field_set(unit,
            &link_control_entry, SW_PORT_STATEf, status_bitmap);

    override_bitmap = sal_alloc(alloc_size, "DLB HGT_LAG member override bitmap");
    if (NULL == override_bitmap) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    sal_memset(override_bitmap, 0, alloc_size);
    soc_DLB_HGT_LAG_LINK_CONTROLm_field_get(unit,
            &link_control_entry, SW_OVERRIDE_PORT_MAPf,
            override_bitmap);

    SHR_BITREMOVE_RANGE(override_bitmap, port_map, 0, port_map_width,
            override_bitmap);
    soc_DLB_HGT_LAG_LINK_CONTROLm_field_set(unit,
            &link_control_entry, SW_OVERRIDE_PORT_MAPf, override_bitmap);

    rv = WRITE_DLB_HGT_LAG_LINK_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                &link_control_entry);
    if (BCM_FAILURE(rv)) {
        goto error;
    }

    /* Set group membership table */
    sal_memset(&dlb_hgt_lag_group_membership_entry, 0,
            sizeof(dlb_hgt_lag_group_membership_entry_t));
    soc_DLB_HGT_LAG_GROUP_MEMBERSHIPm_field_set(unit,
            &dlb_hgt_lag_group_membership_entry, PORT_MAPf, port_map);
    membership_pointer = dlb_id;
    rv = WRITE_DLB_HGT_LAG_GROUP_MEMBERSHIPm(unit, MEM_BLOCK_ALL, membership_pointer,
            &dlb_hgt_lag_group_membership_entry);

    if (BCM_FAILURE(rv)) {
        goto error;
    }

    /* Set DLB flow set table */

    flowset_mem = DLB_HGT_LAG_FLOWSETm;
    flowset_entry_size = sizeof(dlb_hgt_lag_flowset_entry_t);
    num_blocks = add_info->dynamic_size >> _BCM_HGT_LAG_DLB_FLOWSET_BLOCK_SIZE_SHIFT;
    total_blocks = soc_mem_index_count(unit, flowset_mem) >> _BCM_HGT_LAG_DLB_FLOWSET_BLOCK_SIZE_SHIFT;
    max_block_base_ptr = total_blocks - num_blocks;
    for (block_base_ptr = 0;
            block_base_ptr <= max_block_base_ptr;
            block_base_ptr++) {
        /* Check if the contiguous region of flow set table from
         * block_base_ptr to (block_base_ptr + num_blocks - 1) is free.
         */
        _BCM_HGT_LAG_DLB_FLOWSET_BLOCK_TEST_RANGE(unit, block_base_ptr, num_blocks,
                occupied);
        if (!occupied) {
            break;
        }
    }
    if (block_base_ptr > max_block_base_ptr) {
        /* A contiguous region of the desired size could not be found in
         * flow set table.
         */
        rv = BCM_E_RESOURCE;
        goto error;
    }

    entry_base_ptr = block_base_ptr << _BCM_HGT_LAG_DLB_FLOWSET_BLOCK_SIZE_SHIFT;
    num_entries_per_block = 1 << _BCM_HGT_LAG_DLB_FLOWSET_BLOCK_SIZE_SHIFT;
    alloc_size = num_entries_per_block * flowset_entry_size;
    block_ptr = soc_cm_salloc(unit, alloc_size,
            "Block of DLB_HGT_LAG_FLOWSET entries");
    if (NULL == block_ptr) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    sal_memset(block_ptr, 0, alloc_size);
    for (i = 0; i < num_blocks; i++) {
        for (k = 0; k < num_entries_per_block; k++) {
            flowset_entry = soc_mem_table_idx_to_pointer(unit,
                    DLB_HGT_LAG_FLOWSETm, dlb_hgt_lag_flowset_entry_t *,
                    block_ptr, k);
            soc_DLB_HGT_LAG_FLOWSETm_field32_set(unit, flowset_entry,
                    VALIDf, 1);
            soc_DLB_HGT_LAG_FLOWSETm_field32_set(unit, flowset_entry,
                    PORT_MEMBER_ASSIGNMENTf,
                    port_array[(i * num_entries_per_block + k)
                    % num_dlb_ports]);
        }
        index_min = entry_base_ptr + i * num_entries_per_block;
        index_max = index_min + num_entries_per_block - 1;
        rv = soc_mem_write_range(unit, flowset_mem, MEM_BLOCK_ALL,
                index_min, index_max, block_ptr);
        if (BCM_FAILURE(rv)) {
            goto error;
        }
    }

    _BCM_HGT_LAG_DLB_FLOWSET_BLOCK_USED_SET_RANGE(unit, block_base_ptr, num_blocks);

    /* Set HGT_LAG DLB group control table */

    sal_memset(&dlb_hgt_lag_group_control_entry, 0,
            sizeof(dlb_hgt_lag_group_control_entry_t));
    soc_DLB_HGT_LAG_GROUP_CONTROLm_field32_set(unit, &dlb_hgt_lag_group_control_entry,
            GROUP_MEMBERSHIP_POINTERf, membership_pointer);
    soc_DLB_HGT_LAG_GROUP_CONTROLm_field32_set(unit, &dlb_hgt_lag_group_control_entry,
            ENABLE_OPTIMAL_CANDIDATE_UPDATEf, 1);
    soc_DLB_HGT_LAG_GROUP_CONTROLm_field32_set(unit, &dlb_hgt_lag_group_control_entry,
            FLOW_SET_BASEf, entry_base_ptr);

    rv = bcm_th2_hgt_lag_dlb_dynamic_size_encode(add_info->dynamic_size,
            &flow_set_size);
    if (BCM_FAILURE(rv)) {
        goto error;
    }
    soc_DLB_HGT_LAG_GROUP_CONTROLm_field32_set(unit, &dlb_hgt_lag_group_control_entry,
            FLOW_SET_SIZEf, flow_set_size);

    switch (add_info->psc) {
        case BCM_TRUNK_PSC_DYNAMIC:
            dlb_mode = 0;
            break;
        case BCM_TRUNK_PSC_DYNAMIC_ASSIGNED:
            dlb_mode = 1;
            break;
        case BCM_TRUNK_PSC_DYNAMIC_OPTIMAL:
            dlb_mode = 2;
            break;
        default:
            rv = BCM_E_PARAM;
            goto error;
    }
    soc_DLB_HGT_LAG_GROUP_CONTROLm_field32_set(unit, &dlb_hgt_lag_group_control_entry,
            PORT_ASSIGNMENT_MODEf, dlb_mode);

    soc_DLB_HGT_LAG_GROUP_CONTROLm_field32_set(unit, &dlb_hgt_lag_group_control_entry,
            INACTIVITY_DURATIONf, add_info->dynamic_age);

    rv = WRITE_DLB_HGT_LAG_GROUP_CONTROLm(unit, MEM_BLOCK_ALL, dlb_id,
            &dlb_hgt_lag_group_control_entry);
    if (BCM_FAILURE(rv)) {
        goto error;
    }

    /* Update trunk group table */
    if (hgt) {
        rv = READ_HGT_DLB_CONTROLm(unit, MEM_BLOCK_ANY, tid, &hgt_dlb_control);
        if (BCM_FAILURE(rv)) {
            goto error;
        }
        soc_HGT_DLB_CONTROLm_field32_set(unit, &hgt_dlb_control, GROUP_ENABLEf, 1);
        soc_HGT_DLB_CONTROLm_field32_set(unit, &hgt_dlb_control, DLB_IDf, dlb_id);
        rv = WRITE_HGT_DLB_CONTROLm(unit, MEM_BLOCK_ALL, tid, &hgt_dlb_control);
#ifdef BCM_TRIDENT3_SUPPORT
        if (soc_feature(unit, soc_feature_td3_dlb)) {
            rv = READ_HG_TRUNK_MODEm(unit, MEM_BLOCK_ANY, tid, &hgt_trunk_mode);
            if (BCM_FAILURE(rv)) {
                goto error;
            }
            soc_HG_TRUNK_MODEm_field32_set(unit, &hgt_trunk_mode, HG_TRUNK_LB_MODEf, 0x1);
            rv = WRITE_HG_TRUNK_MODEm(unit, MEM_BLOCK_ANY, tid, &hgt_trunk_mode);
        }
#endif
    } else {
        rv = READ_LAG_DLB_CONTROLm(unit, MEM_BLOCK_ANY, tid, &lag_dlb_control);
        if (BCM_FAILURE(rv)) {
            goto error;
        }
        soc_LAG_DLB_CONTROLm_field32_set(unit, &lag_dlb_control, GROUP_ENABLEf, 1);
        soc_LAG_DLB_CONTROLm_field32_set(unit, &lag_dlb_control, DLB_IDf, dlb_id);
        rv = WRITE_LAG_DLB_CONTROLm(unit, MEM_BLOCK_ALL, tid, &lag_dlb_control);
#ifdef BCM_TRIDENT3_SUPPORT
        if (soc_feature(unit, soc_feature_td3_dlb)) {
            rv = READ_TRUNK_GROUPm(unit, MEM_BLOCK_ANY, tid, &tg_entry);
            if (BCM_FAILURE(rv)) {
                goto error;
            }
            soc_TRUNK_GROUPm_field32_set(unit, &tg_entry, TRUNK_MODEf, 0x3);
            rv = WRITE_TRUNK_GROUPm(unit, MEM_BLOCK_ANY, tid, &tg_entry);
        }
#endif
    }
    if (BCM_FAILURE(rv)) {
        goto error;
    }

    soc_cm_sfree(unit, block_ptr);
    sal_free(port_map);
    sal_free(status_bitmap);
    sal_free(override_bitmap);

    return rv;

error:
    if (NULL != block_ptr) {
        soc_cm_sfree(unit, block_ptr);
    }
    if (NULL != port_map) {
        sal_free(port_map);
    }
    if (NULL != status_bitmap) {
        sal_free(status_bitmap);
    }
    if (NULL != override_bitmap) {
        sal_free(override_bitmap);
    }
    return rv;
}


/*
 * Function:
 *      bcm_th2_hgt_lag_dlb_member_status_set
 * Purpose:
 *      Set HGT_LAG DLB member status.
 * Parameters:
 *      unit   - (IN) SOC unit number.
 *      port   - (IN) Member port number.
 *      status - (IN) Member status.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_th2_hgt_lag_dlb_member_status_set(int unit, bcm_port_t port, int status)
{
    dlb_hgt_lag_link_control_entry_t link_control_entry;
    int status_width, alloc_size;
    SHR_BITDCL *status_bitmap = NULL;
    SHR_BITDCL *override_bitmap = NULL;

    /* Get status bitmap */
    BCM_IF_ERROR_RETURN(READ_DLB_HGT_LAG_LINK_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                &link_control_entry));
    status_width = soc_mem_field_length(unit, DLB_HGT_LAG_LINK_CONTROLm,
                                        SW_PORT_STATEf);
    alloc_size = SHR_BITALLOCSIZE(status_width);
    status_bitmap = sal_alloc(alloc_size, "DLB HGT_LAG port status bitmap");
    if (NULL == status_bitmap) {
        return BCM_E_MEMORY;
    }
    sal_memset(status_bitmap, 0, alloc_size);
    soc_DLB_HGT_LAG_LINK_CONTROLm_field_get(unit, &link_control_entry,
            SW_PORT_STATEf, status_bitmap);

    /* Get override bitmap */
    override_bitmap = sal_alloc(alloc_size, "DLB HGT_LAG port override bitmap");
    if (NULL == override_bitmap) {
        sal_free(status_bitmap);
        return BCM_E_MEMORY;
    }
    sal_memset(override_bitmap, 0, alloc_size);
    soc_DLB_HGT_LAG_LINK_CONTROLm_field_get(unit, &link_control_entry,
            SW_OVERRIDE_PORT_MAPf, override_bitmap);

    /* Update status and override bitmaps */
    switch (status) {
        case BCM_TRUNK_DYNAMIC_MEMBER_FORCE_DOWN:
            SHR_BITSET(override_bitmap, port);
            SHR_BITCLR(status_bitmap, port);
            break;
        case BCM_TRUNK_DYNAMIC_MEMBER_FORCE_UP:
            SHR_BITSET(override_bitmap, port);
            SHR_BITSET(status_bitmap, port);
            break;
        case BCM_TRUNK_DYNAMIC_MEMBER_HW:
            SHR_BITCLR(override_bitmap, port);
            SHR_BITCLR(status_bitmap, port);
            break;
        default:
            sal_free(status_bitmap);
            sal_free(override_bitmap);
            return BCM_E_PARAM;
    }

    /* Write status and override bitmaps to hardware */
    soc_DLB_HGT_LAG_LINK_CONTROLm_field_set(unit, &link_control_entry,
            SW_PORT_STATEf, status_bitmap);
    soc_DLB_HGT_LAG_LINK_CONTROLm_field_set(unit, &link_control_entry,
            SW_OVERRIDE_PORT_MAPf, override_bitmap);
    sal_free(status_bitmap);
    sal_free(override_bitmap);
    BCM_IF_ERROR_RETURN
        (WRITE_DLB_HGT_LAG_LINK_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                         &link_control_entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th2_hgt_lag_dlb_member_status_get
 * Purpose:
 *      Get HGT_LAG DLB member status.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      port   - (IN) Member port number.
 *      status - (OUT) Member status.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_th2_hgt_lag_dlb_member_status_get(int unit, bcm_port_t port, int *status)
{
    int rv = BCM_E_NONE;
    dlb_hgt_lag_link_control_entry_t link_control_entry;
    int status_width, alloc_size;
    SHR_BITDCL *status_bitmap    = NULL;
    SHR_BITDCL *override_bitmap  = NULL;
    SHR_BITDCL *hw_status_bitmap = NULL;
    dlb_hgt_lag_port_state_entry_t hw_state;

    /* Get status bitmap */
    BCM_IF_ERROR_RETURN(READ_DLB_HGT_LAG_LINK_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                &link_control_entry));
    status_width = soc_mem_field_length(unit, DLB_HGT_LAG_LINK_CONTROLm,
                                        SW_PORT_STATEf);
    alloc_size = SHR_BITALLOCSIZE(status_width);
    status_bitmap = sal_alloc(alloc_size, "DLB HGT_LAG port status bitmap");
    if (NULL == status_bitmap) {
        return BCM_E_MEMORY;
    }
    sal_memset(status_bitmap, 0, alloc_size);
    soc_DLB_HGT_LAG_LINK_CONTROLm_field_get(unit, &link_control_entry,
            SW_PORT_STATEf, status_bitmap);

    /* Get override bitmap */
    override_bitmap = sal_alloc(alloc_size, "DLB HGT_LAG port override bitmap");
    if (NULL == override_bitmap) {
        sal_free(status_bitmap);
        return BCM_E_MEMORY;
    }
    sal_memset(override_bitmap, 0, alloc_size);
    soc_DLB_HGT_LAG_LINK_CONTROLm_field_get(unit, &link_control_entry,
            SW_OVERRIDE_PORT_MAPf, override_bitmap);

    /* Derive status from software override and status bitmaps or
     * hardware state bitmap.
     */
    if (SHR_BITGET(override_bitmap, port)) {
        if (SHR_BITGET(status_bitmap, port)) {
            *status = BCM_TRUNK_DYNAMIC_MEMBER_FORCE_UP;
        } else {
            *status = BCM_TRUNK_DYNAMIC_MEMBER_FORCE_DOWN;
        }
    } else {
        /* Get hardware bitmap */
        hw_status_bitmap = sal_alloc(alloc_size, "DLB HGT_LAG member hw status bitmap");
        if (NULL == hw_status_bitmap) {
            sal_free(status_bitmap);
            sal_free(override_bitmap);
            return BCM_E_MEMORY;
        }
        sal_memset(hw_status_bitmap, 0, alloc_size);

        rv = READ_DLB_HGT_LAG_PORT_STATEm(unit, MEM_BLOCK_ANY, 0, &hw_state);
        if (BCM_FAILURE(rv)) {
            sal_free(status_bitmap);
            sal_free(override_bitmap);
            sal_free(hw_status_bitmap);
            return rv;
        }
        soc_DLB_HGT_LAG_PORT_STATEm_field_get(unit, &hw_state,
                BITMAPf, hw_status_bitmap);
        if (SHR_BITGET(hw_status_bitmap, port)) {
            *status = BCM_TRUNK_DYNAMIC_MEMBER_HW_UP;
        } else {
            *status = BCM_TRUNK_DYNAMIC_MEMBER_HW_DOWN;
        }
    }

    sal_free(status_bitmap);
    sal_free(override_bitmap);
    if (NULL != hw_status_bitmap) {
        sal_free(hw_status_bitmap);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th2_hgt_lag_dlb_ethertype_set
 * Purpose:
 *      Set the Ethertypes that are eligible or ineligible for
 *      HGT_LAG dynamic load balancing.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) BCM_TRUNK_DYNAMIC_ETHERTYPE_xxx flags.
 *      ethertype_count - (IN) Number of elements in ethertype_array.
 *      ethertype_array - (IN) Array of Ethertypes.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th2_hgt_lag_dlb_ethertype_set(int unit,
                                  uint32 flags,
                                  int ethertype_count,
                                  int *ethertype_array)
{
    int i, j;
    dlb_hgt_lag_eem_configuration_entry_t eem_entry;
    dlb_hgt_lag_ethertype_eligibility_map_entry_t ethertype_entry;
#ifdef BCM_TRIDENT3_SUPPORT
    uint32 reg_val = 0;
#endif
    /* Input check */
    if ((ethertype_count > 0) && (NULL == ethertype_array)) {
        return BCM_E_PARAM;
    }
    if (ethertype_count > soc_mem_index_count(unit,
                DLB_HGT_LAG_ETHERTYPE_ELIGIBILITY_MAPm)) {
        return BCM_E_RESOURCE;
    }

    /* Update quality measure control register */
#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_td3_dlb)) {
        BCM_IF_ERROR_RETURN(READ_DLB_HGT_LAG_ETHERTYPE_ELIGIBILITY_CONTROLr(unit,
                    &reg_val));
        soc_reg_field_set(unit, DLB_HGT_LAG_ETHERTYPE_ELIGIBILITY_CONTROLr,
                &reg_val, CONFIGURATIONf,
                flags & BCM_TRUNK_DYNAMIC_ETHERTYPE_ELIGIBLE ? 1 : 0);
        soc_reg_field_set(unit, DLB_HGT_LAG_ETHERTYPE_ELIGIBILITY_CONTROLr,
                &reg_val, INNER_OUTER_ETHERTYPE_SELECTIONf,
                flags & BCM_TRUNK_DYNAMIC_ETHERTYPE_INNER ? 1 : 0);
        BCM_IF_ERROR_RETURN(WRITE_DLB_HGT_LAG_ETHERTYPE_ELIGIBILITY_CONTROLr(unit,
                    reg_val));
    } else
#endif
    {
        BCM_IF_ERROR_RETURN(READ_DLB_HGT_LAG_EEM_CONFIGURATIONm(unit,
                    MEM_BLOCK_ANY, 0, &eem_entry));
        soc_DLB_HGT_LAG_EEM_CONFIGURATIONm_field32_set(unit,
                &eem_entry, CONFIGURATIONf,
                flags & BCM_TRUNK_DYNAMIC_ETHERTYPE_ELIGIBLE ? 1 : 0);
        soc_DLB_HGT_LAG_EEM_CONFIGURATIONm_field32_set(unit,
                &eem_entry, INNER_OUTER_ETHERTYPE_SELECTIONf,
                flags & BCM_TRUNK_DYNAMIC_ETHERTYPE_INNER ? 1 : 0);
        BCM_IF_ERROR_RETURN(WRITE_DLB_HGT_LAG_EEM_CONFIGURATIONm(unit,
                    MEM_BLOCK_ALL, 0, &eem_entry));
    }

    /* Update Ethertype eligibility map table */
    for (i = 0; i < ethertype_count; i++) {
        sal_memset(&ethertype_entry, 0,
                sizeof(dlb_hgt_lag_ethertype_eligibility_map_entry_t));
        soc_DLB_HGT_LAG_ETHERTYPE_ELIGIBILITY_MAPm_field32_set(unit,
                &ethertype_entry, VALIDf, 1);
        soc_DLB_HGT_LAG_ETHERTYPE_ELIGIBILITY_MAPm_field32_set(unit,
                &ethertype_entry, ETHERTYPEf, ethertype_array[i]);
        BCM_IF_ERROR_RETURN(WRITE_DLB_HGT_LAG_ETHERTYPE_ELIGIBILITY_MAPm(unit,
                    MEM_BLOCK_ALL, i, &ethertype_entry));
    }

    /* Zero out remaining entries of Ethertype eligibility map table */
    for (j = i; j < soc_mem_index_count(unit,
                DLB_HGT_LAG_ETHERTYPE_ELIGIBILITY_MAPm); j++) {
        BCM_IF_ERROR_RETURN(WRITE_DLB_HGT_LAG_ETHERTYPE_ELIGIBILITY_MAPm(unit,
                    MEM_BLOCK_ALL, j, soc_mem_entry_null(unit,
                        DLB_HGT_LAG_ETHERTYPE_ELIGIBILITY_MAPm)));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th2_hgt_lag_dlb_ethertype_get
 * Purpose:
 *      Get the Ethertypes that are eligible or ineligible for
 *      HGT_LAG dynamic load balancing.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (INOUT) BCM_TRUNK_DYNAMIC_ETHERTYPE_xxx flags.
 *      ethertype_max - (IN) Number of elements in ethertype_array.
 *      ethertype_array - (OUT) Array of Ethertypes.
 *      ethertype_count - (OUT) Number of elements returned in ethertype_array.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th2_hgt_lag_dlb_ethertype_get(int unit,
                                  uint32 *flags,
                                  int ethertype_max,
                                  int *ethertype_array,
                                  int *ethertype_count)
{
    int i;
    int ethertype;
    dlb_hgt_lag_ethertype_eligibility_map_entry_t ethertype_entry;
    dlb_hgt_lag_eem_configuration_entry_t eem_entry;
#ifdef BCM_TRIDENT3_SUPPORT
    uint32 reg_val = 0;
#endif
    /* Input check */
    if (NULL == flags) {
        return BCM_E_PARAM;
    }
    if ((ethertype_max > 0) && (NULL == ethertype_array)) {
        return BCM_E_PARAM;
    }
    if (NULL == ethertype_count) {
        return BCM_E_PARAM;
    }

    *ethertype_count = 0;

    /* Get flags */
#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_td3_dlb)) {
        BCM_IF_ERROR_RETURN(READ_DLB_HGT_LAG_ETHERTYPE_ELIGIBILITY_CONTROLr(unit,
                    &reg_val));
        if (soc_reg_field_get(unit, DLB_HGT_LAG_ETHERTYPE_ELIGIBILITY_CONTROLr,
                    reg_val, CONFIGURATIONf)) {
            *flags |= BCM_TRUNK_DYNAMIC_ETHERTYPE_ELIGIBLE;
        }
        if (soc_reg_field_get(unit, DLB_HGT_LAG_ETHERTYPE_ELIGIBILITY_CONTROLr,
                    reg_val, INNER_OUTER_ETHERTYPE_SELECTIONf)) {
            *flags |= BCM_TRUNK_DYNAMIC_ETHERTYPE_INNER;
        }
    } else
#endif
    {
        BCM_IF_ERROR_RETURN(READ_DLB_HGT_LAG_EEM_CONFIGURATIONm(unit,
                    MEM_BLOCK_ANY, 0, &eem_entry));
        if (soc_DLB_HGT_LAG_EEM_CONFIGURATIONm_field32_get(unit,
                    &eem_entry, CONFIGURATIONf)) {
            *flags |= BCM_TRUNK_DYNAMIC_ETHERTYPE_ELIGIBLE;
        }
        if (soc_DLB_HGT_LAG_EEM_CONFIGURATIONm_field32_get(unit,
                    &eem_entry, INNER_OUTER_ETHERTYPE_SELECTIONf)) {
            *flags |= BCM_TRUNK_DYNAMIC_ETHERTYPE_INNER;
        }
    }
    /* Get Ethertypes */
    for (i = 0; i < soc_mem_index_count(unit,
                DLB_HGT_LAG_ETHERTYPE_ELIGIBILITY_MAPm); i++) {
        BCM_IF_ERROR_RETURN(READ_DLB_HGT_LAG_ETHERTYPE_ELIGIBILITY_MAPm(unit,
                    MEM_BLOCK_ANY, i, &ethertype_entry));
        if (soc_DLB_HGT_LAG_ETHERTYPE_ELIGIBILITY_MAPm_field32_get(unit,
                    &ethertype_entry, VALIDf)) {
            ethertype = soc_DLB_HGT_LAG_ETHERTYPE_ELIGIBILITY_MAPm_field32_get(unit,
                    &ethertype_entry, ETHERTYPEf);
            if (NULL != ethertype_array) {
                ethertype_array[*ethertype_count] = ethertype;
            }
            (*ethertype_count)++;
            if ((ethertype_max > 0) && (*ethertype_count == ethertype_max)) {
                break;
            }
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_th2_hgt_lag_dlb_quality_map_profile_init
 * Purpose:
 *      Initialize HGT_LAG DLB quality mapping profile.
 * Parameters:
 *      unit - (IN)SOC unit number.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_th2_hgt_lag_dlb_quality_map_profile_init(int unit)
{
    soc_profile_mem_t *profile;
    soc_mem_t mem;
    int entry_words;
    int entries_per_profile;
    int tx_load_percent;
    int qsize_percent;
    int alloc_size;
    uint32 *entry_arr;
    int rv = BCM_E_NONE;
    void *entries;
    uint32 base_index;
    int i;
    int port;
    dlb_hgt_lag_quantize_control_entry_t quantize_control_entry;
    int num_quality_map_profiles;
    int idx_min, idx_max;
    uint8 *table_chunk = NULL;
    void *table_entry;

    if (NULL == _th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_quality_map_profile) {
        _th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_quality_map_profile =
            sal_alloc(sizeof(soc_profile_mem_t),
                      "HGT_LAG DLB Quality Map Profile Mem");
        if (NULL == _th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_quality_map_profile) {
            return BCM_E_MEMORY;
        }
    } else {
        soc_profile_mem_destroy(unit,
                _th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_quality_map_profile);
    }
    profile = _th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_quality_map_profile;
    soc_profile_mem_t_init(profile);

    mem = DLB_HGT_LAG_PORT_QUALITY_MAPPINGm;
    entry_words = sizeof(dlb_hgt_lag_port_quality_mapping_entry_t) / sizeof(uint32);
    BCM_IF_ERROR_RETURN
        (soc_profile_mem_create(unit, &mem, &entry_words, 1, profile));

    num_quality_map_profiles = 1 << soc_mem_field_length(unit,
        DLB_HGT_LAG_QUANTIZE_CONTROLm, PORT_QUALITY_MAPPING_PROFILE_PTRf);
    entries_per_profile =
        soc_mem_index_count(unit, DLB_HGT_LAG_PORT_QUALITY_MAPPINGm) /
        num_quality_map_profiles;

    if (!SOC_WARM_BOOT(unit)) {
        alloc_size = entries_per_profile * entry_words * sizeof(uint32);
        entry_arr = sal_alloc(alloc_size, "HGT_LAG DLB Quality Map entries");
        if (entry_arr == NULL) {
            return BCM_E_MEMORY;
        }
        sal_memset(entry_arr, 0, alloc_size);

        /* Assign quality in the entry array, with 100% weight for port load,
         * 0% for queue size.
         */
        tx_load_percent = 100;
        qsize_percent   = 0;
        rv = _bcm_th2_hgt_lag_dlb_quality_assign(unit, tx_load_percent,
                                             qsize_percent, entry_arr);
        if (BCM_FAILURE(rv)) {
            sal_free(entry_arr);
            return rv;
        }

        entries = entry_arr;
        rv = soc_profile_mem_add(unit, profile, &entries, entries_per_profile,
                                 &base_index);
        sal_free(entry_arr);
        if (BCM_FAILURE(rv)) {
            return rv;
        }

        mem = DLB_HGT_LAG_QUANTIZE_CONTROLm;
        idx_min = soc_mem_index_min(unit, mem);
        idx_max = soc_mem_index_max(unit, mem);
        table_chunk = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, mem),
                                    "DLB table buffer");
        if (table_chunk == NULL) {
            return BCM_E_MEMORY;
        }

        sal_memset(table_chunk, 0, SOC_MEM_TABLE_BYTES(unit, mem));
        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, idx_min, idx_max,
                                table_chunk);
        if (SOC_FAILURE(rv)) {
            soc_cm_sfree(unit, table_chunk);
            return rv;
        }
        for(port = idx_min; port <= idx_max; port++) {
            table_entry = soc_mem_table_idx_to_pointer(unit, mem, void *,
                                                       table_chunk, port);
            soc_mem_field32_set(unit, mem, table_entry,
                                PORT_QUALITY_MAPPING_PROFILE_PTRf,
                                base_index / entries_per_profile);
        }
        rv = soc_mem_write_range(unit, mem, MEM_BLOCK_ANY, idx_min, idx_max,
                                table_chunk);
        if (SOC_FAILURE(rv)) {
            soc_cm_sfree(unit, table_chunk);
            return rv;
        }
        soc_cm_sfree(unit, table_chunk);

        for (i = 0; i < entries_per_profile; i++) {
            SOC_PROFILE_MEM_REFERENCE(unit, profile, base_index + i,
                                      port - 1);
        }

        HGT_LAG_DLB_INFO(unit)->
            hgt_lag_dlb_load_weight[base_index/entries_per_profile] =
                tx_load_percent;
        HGT_LAG_DLB_INFO(unit)->
            hgt_lag_dlb_qsize_weight[base_index/entries_per_profile] =
                qsize_percent;

    } else { /* Warm boot, recover reference counts and entries_per_set */

        for (port = 0;
                port < soc_mem_index_count(unit,
                    DLB_HGT_LAG_QUANTIZE_CONTROLm);
                port++) {
            BCM_IF_ERROR_RETURN
                (READ_DLB_HGT_LAG_QUANTIZE_CONTROLm(unit, MEM_BLOCK_ANY,
                                               port,
                                               &quantize_control_entry));
            base_index = soc_DLB_HGT_LAG_QUANTIZE_CONTROLm_field32_get(unit,
                    &quantize_control_entry, PORT_QUALITY_MAPPING_PROFILE_PTRf);
            base_index *= entries_per_profile;
            for (i = 0; i < entries_per_profile; i++) {
                SOC_PROFILE_MEM_REFERENCE(unit, profile, base_index + i, 1);
                SOC_PROFILE_MEM_ENTRIES_PER_SET(unit, profile,
                        base_index + i, entries_per_profile);
            }
        }
    }

    return BCM_E_NONE;
}



/*
 * Function:
 *      bcm_th2_hgt_lag_dlb_deinit
 * Purpose:
 *      Deallocate _th2_hgt_lag_dlb_bk
 * Parameters:
 *      unit - (IN)SOC unit number.
 * Returns:
 *      None
 */
void
bcm_th2_hgt_lag_dlb_deinit(int unit)
{
    if (_th2_hgt_lag_dlb_bk[unit]) {
        if (_th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_id_used_bitmap) {
            sal_free(_th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_id_used_bitmap);
            _th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_id_used_bitmap = NULL;
        }
        if (_th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_flowset_block_bitmap) {
            sal_free(_th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_flowset_block_bitmap);
            _th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_flowset_block_bitmap = NULL;
        }
        if (_th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_load_weight) {
            sal_free(_th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_load_weight);
            _th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_load_weight = NULL;
        }
        if (_th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_qsize_weight) {
            sal_free(_th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_qsize_weight);
            _th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_qsize_weight = NULL;
        }
        if (_th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_quality_map_profile) {
            soc_profile_mem_destroy(unit,
                    _th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_quality_map_profile);
            sal_free(_th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_quality_map_profile);
            _th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_quality_map_profile = NULL;
        }

        sal_free(_th2_hgt_lag_dlb_bk[unit]);
        _th2_hgt_lag_dlb_bk[unit] = NULL;
    }

    return;
}

/*
 * Function:
 *      bcm_th2_hgt_lag_dlb_init
 * Purpose:
 *      Allocate and initialize _th2_hgt_lag_dlb_bk
 * Parameters:
 *      unit - (IN)SOC unit number.
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_th2_hgt_lag_dlb_init(int unit)
{
    int rv = BCM_E_NONE;
    int num_hgt_lag_dlb_id;
    int flowset_tbl_size;
    int total_num_blocks;
    int num_quality_map_profiles;
    int port, pipe;
    dlb_hgt_lag_eem_configuration_entry_t eem_entry;
    int sample_rate;
    int qsize_sel[4] = {1, 0, 0, 1};
    soc_reg_t reg;
    uint32 rval;
    int idx_min, idx_max;
    uint8 *table_chunk = NULL;
    void *table_entry;
    soc_mem_t mem;
#ifdef BCM_TRIDENT3_SUPPORT
    uint32 reg_val = 0;
#endif
    bcm_th2_hgt_lag_dlb_deinit(unit);

    if (_th2_hgt_lag_dlb_bk[unit] == NULL) {
        _th2_hgt_lag_dlb_bk[unit] = sal_alloc(
                sizeof(_th2_hgt_lag_dlb_bookkeeping_t), "_th2_hgt_lag_dlb_bk");
        if (_th2_hgt_lag_dlb_bk[unit] == NULL) {
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_th2_hgt_lag_dlb_bk[unit], 0, sizeof(_th2_hgt_lag_dlb_bookkeeping_t));

    num_hgt_lag_dlb_id = soc_mem_index_count(unit, DLB_HGT_LAG_GROUP_CONTROLm);
    if (_th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_id_used_bitmap == NULL) {
        _th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_id_used_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(num_hgt_lag_dlb_id),
                    "hgt_lag_dlb_id_used_bitmap");
        if (_th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_id_used_bitmap == NULL) {
            bcm_th2_hgt_lag_dlb_deinit(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_id_used_bitmap, 0,
            SHR_BITALLOCSIZE(num_hgt_lag_dlb_id));

    flowset_tbl_size = soc_mem_index_count(unit, DLB_HGT_LAG_FLOWSETm);
    /* Each bit in hgt_lag_dlb_flowset_block_bitmap corresponds to a block of 512
     * flow set table entries.
     */
    total_num_blocks = flowset_tbl_size >> _BCM_HGT_LAG_DLB_FLOWSET_BLOCK_SIZE_SHIFT;
    if (_th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_flowset_block_bitmap == NULL) {
        _th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_flowset_block_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(total_num_blocks),
                    "hgt_lag_dlb_flowset_block_bitmap");
        if (_th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_flowset_block_bitmap == NULL) {
            bcm_th2_hgt_lag_dlb_deinit(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_flowset_block_bitmap, 0,
            SHR_BITALLOCSIZE(total_num_blocks));

    num_quality_map_profiles = 1 << soc_mem_field_length(unit,
            DLB_HGT_LAG_QUANTIZE_CONTROLm, PORT_QUALITY_MAPPING_PROFILE_PTRf);
    if (_th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_load_weight == NULL) {
        _th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_load_weight =
            sal_alloc(num_quality_map_profiles * sizeof(uint8),
                    "hgt_lag_dlb_load_weight");
        if (_th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_load_weight == NULL) {
            bcm_th2_hgt_lag_dlb_deinit(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_load_weight, 0,
            num_quality_map_profiles * sizeof(uint8));

    if (_th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_qsize_weight == NULL) {
        _th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_qsize_weight =
            sal_alloc(num_quality_map_profiles * sizeof(uint8),
                    "hgt_lag_dlb_qsize_weight");
        if (_th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_qsize_weight == NULL) {
            bcm_th2_hgt_lag_dlb_deinit(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_th2_hgt_lag_dlb_bk[unit]->hgt_lag_dlb_qsize_weight, 0,
            num_quality_map_profiles * sizeof(uint8));

    /* Initialize port quality mapping profile */
    rv = _bcm_th2_hgt_lag_dlb_quality_map_profile_init(unit);
    if (BCM_FAILURE(rv)) {
        bcm_th2_hgt_lag_dlb_deinit(unit);
        return rv;
    }

    if (!SOC_WARM_BOOT(unit)) {

        /* Set DLB sampling period and thresholds */
        sample_rate = 62500;
        rv = _bcm_th2_hgt_lag_dlb_sample_rate_thresholds_set(unit, sample_rate,
                65536, (65536 * 7));
        if (BCM_FAILURE(rv)) {
            bcm_th2_hgt_lag_dlb_deinit(unit);
            return rv;
        }

        /* Configure HGT_LAG DLB load EWMA weight */
        rv = bcm_th2_hgt_lag_dlb_config_set(unit,
                bcmSwitchTrunkDynamicEgressBytesExponent, 0);
        if (BCM_FAILURE(rv)) {
            bcm_th2_hgt_lag_dlb_deinit(unit);
            return rv;
        }

        /* Configure HGT_LAG DLB queue size EWMA weight */
        if (!soc_feature(unit, soc_feature_td3_dlb)) {
            rv = bcm_th2_hgt_lag_dlb_config_set(unit,
                    bcmSwitchTrunkDynamicQueuedBytesExponent, 0);
            if (BCM_FAILURE(rv)) {
                bcm_th2_hgt_lag_dlb_deinit(unit);
                return rv;
            }
        }

        /* Configure HGT_LAG DLB Physical queue size EWMA weight */
        rv = bcm_th2_hgt_lag_dlb_config_set(unit,
                bcmSwitchTrunkDynamicPhysicalQueuedBytesExponent, 0);
        if (BCM_FAILURE(rv)) {
            bcm_th2_hgt_lag_dlb_deinit(unit);
            return rv;
        }

        /* Configure HGT_LAG DLB load cap control */
        rv = bcm_th2_hgt_lag_dlb_config_set(unit,
                bcmSwitchTrunkDynamicEgressBytesDecreaseReset, 0);
        if (BCM_FAILURE(rv)) {
            bcm_th2_hgt_lag_dlb_deinit(unit);
            return rv;
        }

        /* Configure HGT_LAG DLB queue size cap control */
        if (!soc_feature(unit, soc_feature_td3_dlb)) {
            rv = bcm_th2_hgt_lag_dlb_config_set(unit,
                    bcmSwitchTrunkDynamicQueuedBytesDecreaseReset, 0);
            if (BCM_FAILURE(rv)) {
                bcm_th2_hgt_lag_dlb_deinit(unit);
                return rv;
            }
        }
        /* Configure HGT_LAG DLB Physical queue size cap control */
        rv = bcm_th2_hgt_lag_dlb_config_set(unit,
                bcmSwitchTrunkDynamicPhysicalQueuedBytesDecreaseReset, 0);
        if (BCM_FAILURE(rv)) {
            bcm_th2_hgt_lag_dlb_deinit(unit);
            return rv;
        }

        /* Configure HGT_LAG DLB queue size thresholds */
        if (!soc_feature(unit, soc_feature_td3_dlb)) {
            rv = _bcm_th2_hgt_lag_dlb_qsize_thresholds_set(unit, 0, 0);
            if (BCM_FAILURE(rv)) {
                bcm_th2_hgt_lag_dlb_deinit(unit);
                return rv;
            }
        }

        /* Configure HGT_LAG DLB Physical queue size thresholds */
        rv = _bcm_th2_hgt_lag_dlb_physical_qsize_thresholds_set(unit, 0, 0);
        if (BCM_FAILURE(rv)) {
            bcm_th2_hgt_lag_dlb_deinit(unit);
            return rv;
        }

        /* Disable link status override by software */
        rv = soc_mem_clear(unit, DLB_HGT_LAG_LINK_CONTROLm, MEM_BLOCK_ALL, 0);
        if (BCM_FAILURE(rv)) {
            bcm_th2_hgt_lag_dlb_deinit(unit);
            return rv;
        }

        /* Clear group membership */
        rv = soc_mem_clear(unit, DLB_HGT_LAG_GROUP_MEMBERSHIPm, MEM_BLOCK_ALL, 0);
        if (BCM_FAILURE(rv)) {
            bcm_th2_hgt_lag_dlb_deinit(unit);
            return rv;
        }

        /* Configure per member quality measure update control. */
        mem = DLB_HGT_LAG_PORT_QUALITY_UPDATE_MEASURE_CONTROLm;
        idx_min = soc_mem_index_min(unit, mem);
        idx_max = soc_mem_index_max(unit, mem);
        table_chunk = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, mem),
                                    "DLB table buffer");
        if (table_chunk == NULL) {
            bcm_th2_hgt_lag_dlb_deinit(unit);
            return BCM_E_MEMORY;
        }

        sal_memset(table_chunk, 0, SOC_MEM_TABLE_BYTES(unit, mem));
        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, idx_min, idx_max,
                                table_chunk);
        if (SOC_FAILURE(rv)) {
            soc_cm_sfree(unit, table_chunk);
            bcm_th2_hgt_lag_dlb_deinit(unit);
            return rv;
        }
        for(port = idx_min; port <= idx_max; port++) {
            table_entry = soc_mem_table_idx_to_pointer(unit, mem, void *,
                                                       table_chunk, port);
            soc_mem_field32_set(unit, mem, table_entry,
                                ENABLE_MEASURE_AVERAGE_CALCULATIONf, 1);
            soc_mem_field32_set(unit, mem, table_entry,
                                ENABLE_PORT_QUALITY_UPDATEf, 1);
            soc_mem_field32_set(unit, mem, table_entry,
                                ENABLE_CREDIT_COLLECTIONf, 1);
        }
        rv = soc_mem_write_range(unit, mem, MEM_BLOCK_ANY, idx_min, idx_max,
                                 table_chunk);
        if (SOC_FAILURE(rv)) {
            soc_cm_sfree(unit, table_chunk);
            bcm_th2_hgt_lag_dlb_deinit(unit);
            return rv;
        }
        soc_cm_sfree(unit, table_chunk);

        /* Configure per member threshold scaling factor  */
        mem = DLB_HGT_LAG_QUANTIZE_CONTROLm;
        idx_min = soc_mem_index_min(unit, mem);
        idx_max = soc_mem_index_max(unit, mem);
        table_chunk = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, mem),
                                    "DLB table buffer");
        if (table_chunk == NULL) {
            bcm_th2_hgt_lag_dlb_deinit(unit);
            return BCM_E_MEMORY;
        }

        sal_memset(table_chunk, 0, SOC_MEM_TABLE_BYTES(unit, mem));
        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, idx_min, idx_max,
                                table_chunk);
        if (SOC_FAILURE(rv)) {
            soc_cm_sfree(unit, table_chunk);
            bcm_th2_hgt_lag_dlb_deinit(unit);
            return rv;
        }
        for(port = idx_min; port <= idx_max; port++) {
            table_entry = soc_mem_table_idx_to_pointer(unit, mem, void *,
                                                       table_chunk, port);
            soc_mem_field32_set(unit, mem, table_entry,
                                PORT_LOADING_THRESHOLD_SCALING_FACTORf, 2);
            soc_mem_field32_set(unit, mem, table_entry,
                                XPE_PORT_QSIZE_THRESHOLD_SCALING_FACTORf, 2);
            if (!soc_feature(unit, soc_feature_td3_dlb)) {
                soc_mem_field32_set(unit, mem, table_entry,
                                TOTAL_PORT_QSIZE_THRESHOLD_SCALING_FACTORf, 2);
            }
        }
        rv = soc_mem_write_range(unit, mem, MEM_BLOCK_ANY, idx_min, idx_max,
                                 table_chunk);
        if (SOC_FAILURE(rv)) {
            soc_cm_sfree(unit, table_chunk);
            bcm_th2_hgt_lag_dlb_deinit(unit);
            return rv;
        }
        soc_cm_sfree(unit, table_chunk);

        /* Configure Ethertype eligibility */

        rv = soc_mem_clear(unit, DLB_HGT_LAG_ETHERTYPE_ELIGIBILITY_MAPm,
                MEM_BLOCK_ALL, 0);
        if (BCM_FAILURE(rv)) {
            bcm_th2_hgt_lag_dlb_deinit(unit);
            return rv;
        }
#ifdef BCM_TRIDENT3_SUPPORT
        if (soc_feature(unit, soc_feature_td3_dlb)) {
            rv = READ_DLB_HGT_LAG_ETHERTYPE_ELIGIBILITY_CONTROLr(unit,
                    &reg_val);
            if (BCM_SUCCESS(rv)) {
                soc_reg_field_set(unit, DLB_HGT_LAG_ETHERTYPE_ELIGIBILITY_CONTROLr,
                        &reg_val, CONFIGURATIONf, 0);
                soc_reg_field_set(unit, DLB_HGT_LAG_ETHERTYPE_ELIGIBILITY_CONTROLr,
                        &reg_val, INNER_OUTER_ETHERTYPE_SELECTIONf, 0);
                rv = WRITE_DLB_HGT_LAG_ETHERTYPE_ELIGIBILITY_CONTROLr(unit,
                        reg_val);
            }
        } else
#endif
        {
            rv = READ_DLB_HGT_LAG_EEM_CONFIGURATIONm(unit,
                    MEM_BLOCK_ANY, 0, &eem_entry);
            if (BCM_SUCCESS(rv)) {
                soc_DLB_HGT_LAG_EEM_CONFIGURATIONm_field32_set(unit,
                        &eem_entry,
                        CONFIGURATIONf,
                        0);
                soc_DLB_HGT_LAG_EEM_CONFIGURATIONm_field32_set(unit,
                        &eem_entry,
                        INNER_OUTER_ETHERTYPE_SELECTIONf,
                        0);
                rv = WRITE_DLB_HGT_LAG_EEM_CONFIGURATIONm(unit,
                        MEM_BLOCK_ALL, 0, &eem_entry);
            }
        }
        if (BCM_FAILURE(rv)) {
            bcm_th2_hgt_lag_dlb_deinit(unit);
            return rv;
        }
    }

    /* Write HGT/LAG selection*/
    if (soc_feature(unit, soc_feature_hg_dlb)) {
        rv = soc_reg_field32_modify(unit,
                DLB_HGT_LAG_SELECTIONr, REG_PORT_ANY, SELECTION_HGT_LAGf, 0);
    } else {
        rv = soc_reg_field32_modify(unit,
                DLB_HGT_LAG_SELECTIONr, REG_PORT_ANY, SELECTION_HGT_LAGf, 1);
    }
    if (BCM_FAILURE(rv)) {
        bcm_th2_hgt_lag_dlb_deinit(unit);
        return rv;
    }

    /* Write XPE pipe selector, if set represents XPE A */
    if (!soc_feature(unit, soc_feature_td3_dlb)) {
        for (pipe = 0; pipe < NUM_PIPE(unit); pipe ++) {
            reg = SOC_REG_UNIQUE_ACC(unit, DLB_HGT_LAG_PORT_XPE_A_B_SELECTORr)[pipe];
            rv = soc_reg_field32_modify(unit, reg,
                    REG_PORT_ANY, CAP_SINGLE_QSIZE_SELECTORf, qsize_sel[pipe]);
            if (BCM_FAILURE(rv)) {
                bcm_th2_hgt_lag_dlb_deinit(unit);
                return rv;
            }
        }
    }

    READ_AUX_ARB_CONTROLr(unit,&rval);
    if (soc_reg_field_valid(unit, AUX_ARB_CONTROLr,
                DLB_HGT_256NS_REFRESH_ENABLEf)) {
        soc_reg_field_set(unit, AUX_ARB_CONTROLr, &rval,
                DLB_HGT_256NS_REFRESH_ENABLEf, 1);
    }
    WRITE_AUX_ARB_CONTROLr(unit,rval);

    return rv;
}


#ifdef BCM_WARM_BOOT_SUPPORT

/*
 * Function:
 *      bcm_th2_hgt_lag_dlb_wb_alloc_size_get
 * Purpose:
 *      Get level 2 warm boot scache size for HGT_LAG DLB.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      size - (OUT) Allocation size.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th2_hgt_lag_dlb_wb_alloc_size_get(int unit, int *size)
{
    int alloc_size = 0;
    int num_elements;

    /* Allocate size for the following HGT_LAG DLB parameters:
     * int hgt_lag_dlb_sample_rate;
     * int hgt_lag_dlb_tx_load_min_th;
     * int hgt_lag_dlb_tx_load_max_th;
     * int hgt_lag_dlb_qsize_min_th;
     * int hgt_lag_dlb_qsize_max_th;
     * int hgt_lag_dlb_physical_qsize_min_th;
     * int hgt_lag_dlb_physical_qsize_max_th;
     */
    alloc_size += sizeof(int) * 7;

    /* Allocate size of hgt_lag_dlb_load_weight & hgt_lag_dlb_qsize_weight array */
    num_elements = 1 << soc_mem_field_length(unit,
            DLB_HGT_LAG_QUANTIZE_CONTROLm, PORT_QUALITY_MAPPING_PROFILE_PTRf);
    alloc_size += sizeof(uint8) * 2 * num_elements;

    *size = alloc_size;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th2_hgt_lag_dlb_sync
 * Purpose:
 *      Store HGT_LAG DLB parameters into scache.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      scache_ptr - (IN/OUT) Scache pointer
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th2_hgt_lag_dlb_sync(int unit, uint8 **scache_ptr)
{
    int value;
    int num_elements;
    int i;
    uint8 load_weight;
    uint8 qsize_weight;

    /* Store the following HGT_LAG DLB parameters:
     * int hgt_lag_dlb_sample_rate;
     * int hgt_lag_dlb_tx_load_min_th;
     * int hgt_lag_dlb_tx_load_max_th;
     * int hgt_lag_dlb_qsize_min_th;
     * int hgt_lag_dlb_qsize_max_th;
     * int hgt_lag_dlb_physical_qsize_min_th;
     * int hgt_lag_dlb_physical_qsize_max_th;
     */
    value = HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_sample_rate;
    sal_memcpy((*scache_ptr), &value, sizeof(int));
    (*scache_ptr) += sizeof(int);

    value = HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_tx_load_min_th;
    sal_memcpy((*scache_ptr), &value, sizeof(int));
    (*scache_ptr) += sizeof(int);

    value = HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_tx_load_max_th;
    sal_memcpy((*scache_ptr), &value, sizeof(int));
    (*scache_ptr) += sizeof(int);

    value = HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_qsize_min_th;
    sal_memcpy((*scache_ptr), &value, sizeof(int));
    (*scache_ptr) += sizeof(int);

    value = HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_qsize_max_th;
    sal_memcpy((*scache_ptr), &value, sizeof(int));
    (*scache_ptr) += sizeof(int);

    value = HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_physical_qsize_min_th;
    sal_memcpy((*scache_ptr), &value, sizeof(int));
    (*scache_ptr) += sizeof(int);

    value = HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_physical_qsize_max_th;
    sal_memcpy((*scache_ptr), &value, sizeof(int));
    (*scache_ptr) += sizeof(int);

    /* Store hgt_lag_dlb_load_weight & hgt_lag_dlb_qsize_weight array */
    num_elements = 1 << soc_mem_field_length(unit,
            DLB_HGT_LAG_QUANTIZE_CONTROLm, PORT_QUALITY_MAPPING_PROFILE_PTRf);
    for (i = 0; i < num_elements; i++) {
        load_weight = HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_load_weight[i];
        qsize_weight = HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_qsize_weight[i];
        sal_memcpy((*scache_ptr), &load_weight, sizeof(uint8));
        (*scache_ptr) += sizeof(uint8);
        sal_memcpy((*scache_ptr), &qsize_weight, sizeof(uint8));
        (*scache_ptr) += sizeof(uint8);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th2_hgt_lag_dlb_scache_recover
 * Purpose:
 *      Recover HGT_LAG DLB parameters from scache.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      scache_ptr - (IN/OUT) Scache pointer
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th2_hgt_lag_dlb_scache_recover(int unit, uint8 **scache_ptr)
{
    int value;
    int num_elements;
    int i;
    uint8 load_weight;
    uint8 qsize_weight;

    /* Recover the following HGT_LAG DLB parameters:
     * int hgt_lag_dlb_sample_rate;
     * int hgt_lag_dlb_tx_load_min_th;
     * int hgt_lag_dlb_tx_load_max_th;
     * int hgt_lag_dlb_qsize_min_th;
     * int hgt_lag_dlb_qsize_max_th;
     * int hgt_lag_dlb_physical_qsize_min_th;
     * int hgt_lag_dlb_physical_qsize_max_th;
     */
    sal_memcpy(&value, (*scache_ptr), sizeof(int));
    HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_sample_rate = value;
    (*scache_ptr) += sizeof(int);

    sal_memcpy(&value, (*scache_ptr), sizeof(int));
    HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_tx_load_min_th = value;
    (*scache_ptr) += sizeof(int);

    sal_memcpy(&value, (*scache_ptr), sizeof(int));
    HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_tx_load_max_th = value;
    (*scache_ptr) += sizeof(int);

    sal_memcpy(&value, (*scache_ptr), sizeof(int));
    HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_qsize_min_th = value;
    (*scache_ptr) += sizeof(int);

    sal_memcpy(&value, (*scache_ptr), sizeof(int));
    HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_qsize_max_th = value;
    (*scache_ptr) += sizeof(int);

    sal_memcpy(&value, (*scache_ptr), sizeof(int));
    HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_physical_qsize_min_th = value;
    (*scache_ptr) += sizeof(int);

    sal_memcpy(&value, (*scache_ptr), sizeof(int));
    HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_physical_qsize_max_th = value;
    (*scache_ptr) += sizeof(int);

    /* Recover hgt_lag_dlb_load_weight & hgt_lag_dlb_qsize_weight array */
    num_elements = 1 << soc_mem_field_length(unit,
            DLB_HGT_LAG_QUANTIZE_CONTROLm, PORT_QUALITY_MAPPING_PROFILE_PTRf);
    for (i = 0; i < num_elements; i++) {
        sal_memcpy(&load_weight, (*scache_ptr), sizeof(uint8));
        HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_load_weight[i] = load_weight;
        (*scache_ptr) += sizeof(uint8);
        sal_memcpy(&qsize_weight, (*scache_ptr), sizeof(uint8));
        HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_qsize_weight[i] = qsize_weight;
        (*scache_ptr) += sizeof(uint8);
    }

    HGT_LAG_DLB_INFO(unit)->recovered_from_scache = TRUE;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th2_hgt_lag_dlb_group_recover
 * Purpose:
 *      Recover HGT_LAG DLB group usage and flowset table usage.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      tid  - (IN) Trunk/HiGig Trunk group ID.
 *      hgt  - (IN) Is HiGig Trunk
 *      trunk_info - (OUT) Per trunk group DLB info.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th2_hgt_lag_dlb_group_recover(int unit, bcm_trunk_t tid, int hgt,
                                  trunk_private_t *trunk_info)
{
    hgt_dlb_control_entry_t hgt_dlb_control_entry;
    lag_dlb_control_entry_t lag_dlb_control_entry;
    int dlb_enable, dlb_id;
    dlb_hgt_lag_group_control_entry_t group_control_entry;
    int dlb_mode;
    int dynamic_size_encode, dynamic_size;
    int entry_base_ptr;
    int block_base_ptr, num_blocks;

    if (hgt) {
        BCM_IF_ERROR_RETURN
            (READ_HGT_DLB_CONTROLm(unit, MEM_BLOCK_ANY,
                                  tid, &hgt_dlb_control_entry));
        dlb_enable = soc_HGT_DLB_CONTROLm_field32_get
                        (unit, &hgt_dlb_control_entry, GROUP_ENABLEf);
        dlb_id = soc_HGT_DLB_CONTROLm_field32_get
                        (unit, &hgt_dlb_control_entry, DLB_IDf);
    } else {
        BCM_IF_ERROR_RETURN
            (READ_LAG_DLB_CONTROLm(unit, MEM_BLOCK_ANY,
                                   tid, &lag_dlb_control_entry));
        dlb_enable = soc_LAG_DLB_CONTROLm_field32_get
                            (unit, &lag_dlb_control_entry, GROUP_ENABLEf);
        dlb_id = soc_LAG_DLB_CONTROLm_field32_get
                            (unit, &lag_dlb_control_entry, DLB_IDf);
    }
    if (0 == dlb_enable) {
        /* DLB not enabled */
        return BCM_E_NONE;
    }

    /* Mark DLB group ID as used */
    _BCM_HGT_LAG_DLB_ID_USED_SET(unit, dlb_id);

    BCM_IF_ERROR_RETURN(READ_DLB_HGT_LAG_GROUP_CONTROLm(unit,
                MEM_BLOCK_ANY, dlb_id, &group_control_entry));

    /* Recover DLB mode */
    dlb_mode = soc_DLB_HGT_LAG_GROUP_CONTROLm_field32_get(unit,
            &group_control_entry, PORT_ASSIGNMENT_MODEf);
    switch (dlb_mode) {
        case 0:
            trunk_info->psc = BCM_TRUNK_PSC_DYNAMIC;
            break;
        case 1:
            trunk_info->psc = BCM_TRUNK_PSC_DYNAMIC_ASSIGNED;
            break;
        case 2:
            trunk_info->psc = BCM_TRUNK_PSC_DYNAMIC_OPTIMAL;
            break;
        default:
            return BCM_E_INTERNAL;
    }

    /* Recover dynamic_size */
    dynamic_size_encode = soc_DLB_HGT_LAG_GROUP_CONTROLm_field32_get(unit,
            &group_control_entry, FLOW_SET_SIZEf);
    BCM_IF_ERROR_RETURN
        (_bcm_th2_hgt_lag_dlb_dynamic_size_decode(dynamic_size_encode,
                                              &dynamic_size));
    trunk_info->dynamic_size = dynamic_size;

    /* Recover dynamic_age */
    trunk_info->dynamic_age = soc_DLB_HGT_LAG_GROUP_CONTROLm_field32_get(unit,
            &group_control_entry, INACTIVITY_DURATIONf);

    /* Recover flow set table usage */
    entry_base_ptr = soc_DLB_HGT_LAG_GROUP_CONTROLm_field32_get(unit,
            &group_control_entry, FLOW_SET_BASEf);
    block_base_ptr = entry_base_ptr >> _BCM_HGT_LAG_DLB_FLOWSET_BLOCK_SIZE_SHIFT;
    num_blocks = dynamic_size >> _BCM_HGT_LAG_DLB_FLOWSET_BLOCK_SIZE_SHIFT;
    _BCM_HGT_LAG_DLB_FLOWSET_BLOCK_USED_SET_RANGE(unit, block_base_ptr,
            num_blocks);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th2_hgt_lag_dlb_quality_parameters_recover
 * Purpose:
 *      Recover HGT_LAG DLB parameters used to derive a member's quality.
 * Parameters:
 *      unit - (IN) SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th2_hgt_lag_dlb_quality_parameters_recover(int unit)
{
    uint32 measure_control_reg;
    int num_time_units;
    dlb_hgt_lag_glb_quantize_threshold_entry_t quantize_threshold_entry;
    int min_th_bytes, min_th_cells;
    int max_th_bytes, max_th_cells;
    int entries_per_profile;
    int profile_ptr, base_index;
    dlb_hgt_lag_port_quality_mapping_entry_t quality_mapping_entry;
    int quality;
    int num_profile;

    if (HGT_LAG_DLB_INFO(unit)->recovered_from_scache) {
        /* If already recovered from scache, do nothing. */
        return BCM_E_NONE;
    }

    /* Recover sampling rate */
    BCM_IF_ERROR_RETURN(READ_DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr(unit,
                &measure_control_reg));
    num_time_units = soc_reg_field_get(unit, DLB_HGT_LAG_QUALITY_MEASURE_CONTROLr,
            measure_control_reg, SAMPLING_PERIODf);
    if (num_time_units > 0) {
        HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_sample_rate = 1000000 / num_time_units;
    }

    /* Recover min and max load, queue size and Physical queue size thresholds:
     * Load threshold (in mbps)
     *     = (bytes per sampling period) * 8 bits per byte /
     *       (sampling period in seconds) / 10^6
     *     = (bytes per sampling period) * 8 bits per byte /
     *       (num_time_units * 1 us per time unit * 10^-6) / 10^6
     *     = (bytes per sampling period) * 8 / num_time_units
     */
    BCM_IF_ERROR_RETURN(READ_DLB_HGT_LAG_GLB_QUANTIZE_THRESHOLDm(unit,
                MEM_BLOCK_ANY, 0, &quantize_threshold_entry));
    min_th_bytes = soc_DLB_HGT_LAG_GLB_QUANTIZE_THRESHOLDm_field32_get
        (unit, &quantize_threshold_entry, PORT_LOADING_THRESHOLDf);
    HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_tx_load_min_th = (min_th_bytes << 3) /
        num_time_units;

    if (!soc_feature(unit, soc_feature_td3_dlb)) {
        min_th_cells = soc_DLB_HGT_LAG_GLB_QUANTIZE_THRESHOLDm_field32_get
            (unit, &quantize_threshold_entry, TOTAL_PORT_QSIZE_THRESHOLDf);
        HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_qsize_min_th =
            min_th_cells * _BCM_HGT_LAG_DLB_CELL_BYTES;
    }

    min_th_cells = soc_DLB_HGT_LAG_GLB_QUANTIZE_THRESHOLDm_field32_get
        (unit, &quantize_threshold_entry, XPE_PORT_QSIZE_THRESHOLDf);
    HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_physical_qsize_min_th =
        min_th_cells * _BCM_HGT_LAG_DLB_CELL_BYTES;

    BCM_IF_ERROR_RETURN(READ_DLB_HGT_LAG_GLB_QUANTIZE_THRESHOLDm(unit,
                MEM_BLOCK_ANY, soc_mem_index_max(unit,
                    DLB_HGT_LAG_GLB_QUANTIZE_THRESHOLDm),
                &quantize_threshold_entry));
    max_th_bytes = soc_DLB_HGT_LAG_GLB_QUANTIZE_THRESHOLDm_field32_get
        (unit, &quantize_threshold_entry, PORT_LOADING_THRESHOLDf);
    HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_tx_load_max_th = (max_th_bytes << 3) /
        num_time_units;

    if (!soc_feature(unit, soc_feature_td3_dlb)) {
        max_th_cells = soc_DLB_HGT_LAG_GLB_QUANTIZE_THRESHOLDm_field32_get
            (unit, &quantize_threshold_entry, TOTAL_PORT_QSIZE_THRESHOLDf);
        HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_qsize_max_th =
            max_th_cells * _BCM_HGT_LAG_DLB_CELL_BYTES;
    }

    max_th_cells = soc_DLB_HGT_LAG_GLB_QUANTIZE_THRESHOLDm_field32_get
        (unit, &quantize_threshold_entry, XPE_PORT_QSIZE_THRESHOLDf);
    HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_physical_qsize_max_th =
        max_th_cells * _BCM_HGT_LAG_DLB_CELL_BYTES;

    /* Recover load weights and qsize weights */
    num_profile = 1 << soc_mem_field_length(unit, DLB_HGT_LAG_QUANTIZE_CONTROLm,
                                            PORT_QUALITY_MAPPING_PROFILE_PTRf);
    entries_per_profile =
        soc_mem_index_count(unit, DLB_HGT_LAG_PORT_QUALITY_MAPPINGm) / num_profile;
    for (profile_ptr = 0; profile_ptr < num_profile; profile_ptr++) {
        base_index = profile_ptr * entries_per_profile;
        /* quality = (quantized_tx_load * tx_load_percent +
         *            quantized_qsize * qsize_percent +
         *            quantized_physical_qsize *
         *            (100 - tx_load_percent - qsize_percent)) / 100;
         * The index to DLB_HGT_LAG_PORT_QUALITY_MAPPING table consists of
         * base_index + quantized_tx_load * 64 +
         * quantized_qsize * 8 + quantized_physical_qsize,
         * where quantized_tx_load, quantized_qsize and quantized_physical_qsize
         * range from 0 to 7.
         *
         * With quantized_tx_load = 7, quantized_qsize = quantized_physical_qsize = 0,
         * the index to DLB_HGT_LAG_PORT_QUALITY_MAPPING table is
         * base_index + 448, and the tx_load_percent expression
         * simplifies to quality * 100 / 7;
         *
         * With quantized_qsize = 7, quantized_tx_load = quantized_physical_qsize = 0,
         * the index to DLB_HGT_LAG_PORT_QUALITY_MAPPING table is
         * base_index + 56, and the qsize_percent expression
         * simplifies to quality * 100 / 7;
         */
        BCM_IF_ERROR_RETURN(READ_DLB_HGT_LAG_PORT_QUALITY_MAPPINGm(unit,
                    MEM_BLOCK_ANY, base_index + 448,
                    &quality_mapping_entry));
        quality = soc_DLB_HGT_LAG_PORT_QUALITY_MAPPINGm_field32_get(unit,
                &quality_mapping_entry, ASSIGNED_QUALITYf);
        HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_load_weight[profile_ptr] =
            quality * 100 / 7;
        BCM_IF_ERROR_RETURN(READ_DLB_HGT_LAG_PORT_QUALITY_MAPPINGm(unit,
                    MEM_BLOCK_ANY, base_index + 56,
                    &quality_mapping_entry));
        quality = soc_DLB_HGT_LAG_PORT_QUALITY_MAPPINGm_field32_get(unit,
                &quality_mapping_entry, ASSIGNED_QUALITYf);
        HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_qsize_weight[profile_ptr] =
            quality * 100 / 7;
    }

    return BCM_E_NONE;
}

#endif /* BCM_WARM_BOOT_SUPPORT */

#ifndef BCM_SW_STATE_DUMP_DISABLE

/*
 * Function:
 *     bcm_th2_hgt_lag_dlb_sw_dump
 * Purpose:
 *     Displays HGT_LAG DLB information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
bcm_th2_hgt_lag_dlb_sw_dump(int unit)
{
    int i;
    int num_entries_per_profile;
    int num_profiles;
    int rv;
    int ref_count;

    LOG_CLI((BSL_META_U(unit,
                        "HGT_LAG DLB Info -\n")));

    /* Print HGT_LAG DLB group usage */
    LOG_CLI((BSL_META_U(unit,
                        "    HGT_LAG DLB Groups Used:")));
    for (i = 0; i < soc_mem_index_count(unit, DLB_HGT_LAG_GROUP_CONTROLm); i++) {
        if (_BCM_HGT_LAG_DLB_ID_USED_GET(unit, i)) {
            LOG_CLI((BSL_META_U(unit,
                                " %d"), i));
        }
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));

    /* Print HGT_LAG DLB flowset table usage */
    LOG_CLI((BSL_META_U(unit,
                        "    HGT_LAG DLB Flowset Table Blocks Used:")));
    for (i = 0; i < soc_mem_index_count(unit, DLB_HGT_LAG_FLOWSETm) >>
                    _BCM_HGT_LAG_DLB_FLOWSET_BLOCK_SIZE_SHIFT; i++) {
        if (_BCM_HGT_LAG_DLB_FLOWSET_BLOCK_USED_GET(unit, i)) {
            LOG_CLI((BSL_META_U(unit,
                                " %d"), i));
        }
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));

    /* Print sample rate and threshold parameters */
    LOG_CLI((BSL_META_U(unit,
                        "    Sample rate: %d per second\n"),
             HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_sample_rate));
    LOG_CLI((BSL_META_U(unit,
                        "    Tx load min threshold: %d mbps\n"),
             HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_tx_load_min_th));
    LOG_CLI((BSL_META_U(unit,
                        "    Tx load max threshold: %d mbps\n"),
             HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_tx_load_max_th));
    LOG_CLI((BSL_META_U(unit,
                        "    Queue size min threshold: %d cells\n"),
             HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_qsize_min_th));
    LOG_CLI((BSL_META_U(unit,
                        "    Queue size max threshold: %d cells\n"),
             HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_qsize_max_th));
    LOG_CLI((BSL_META_U(unit,
                        "    Physical Queue size min threshold: %d cells\n"),
             HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_physical_qsize_min_th));
    LOG_CLI((BSL_META_U(unit,
                        "    Physical Queue size max threshold: %d cells\n"),
             HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_physical_qsize_max_th));

    /* Print quality mapping profiles */
    LOG_CLI((BSL_META_U(unit,
                        "    Quality mapping profiles:\n")));

    num_profiles = 1 << soc_mem_field_length(unit, DLB_HGT_LAG_QUANTIZE_CONTROLm,
                                             PORT_QUALITY_MAPPING_PROFILE_PTRf);
    num_entries_per_profile = soc_mem_index_count(unit, DLB_HGT_LAG_PORT_QUALITY_MAPPINGm)/
                              num_profiles;
    for (i = 0; i < num_profiles; i++) {
        LOG_CLI((BSL_META_U(unit,
                            "      Profile %d: load weight %d percent, "\
                            "qsize weight %d percent, "),
                 i, HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_load_weight[i],
                 HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_qsize_weight[i]));
        rv = soc_profile_mem_ref_count_get(unit,
                HGT_LAG_DLB_INFO(unit)->hgt_lag_dlb_quality_map_profile,
                i * num_entries_per_profile, &ref_count);
        if (BCM_FAILURE(rv)) {
            LOG_CLI((BSL_META_U(unit,
                                "\n")));
            continue;
        }
        LOG_CLI((BSL_META_U(unit,
                            "ref count %d\n"), ref_count));
    }

    return;
}

#endif /* BCM_SW_STATE_DUMP_DISABLE */


#endif /* BCM_TOMAHAWK2_SUPPORT || BCM_TRIDENT3_SUPPORT */


