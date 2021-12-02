/** \file ingress_congestion_convert.c
 *
 * Ingress congestion conversion functions
 * 
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <shared/utilex/utilex_u64.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_congestion.h>

#include "bcm_int/dnx/cosq/ingress/ingress_congestion.h"
#include "ingress_congestion.h"
#include "ingress_congestion_convert.h"

#define DNX_INGRESS_CONGESTION_WRED_NORMALIZE_FACTOR  (0x28F5C29)

/**
 * \brief -
 * convert HW WRED data to BCM WRED parameters
 *
 * \see dnx_ingress_congestion_convert_wred_to_hw_get
 */
void
dnx_ingress_congestion_convert_hw_to_wred_get(
    int unit,
    dnx_ingress_congestion_wred_hw_params_t * wred_hw_params,
    dnx_ingress_congestion_wred_info_t * wred)
{
    COMPILER_UINT64 u64_1, u64_2;
    uint32 remainder;
    uint32 avrg_th_diff, two_power_c1;

    wred->wred_en = wred_hw_params->wred_en;

    /** Min average threshold */
    wred->min_avrg_th =
        DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_THRESHOLD_GET(unit,
                                                                 DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES,
                                                                 wred_hw_params->min_thresh);

    /** Max average threshold */
    wred->max_avrg_th =
        DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_THRESHOLD_GET(unit,
                                                                 DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES,
                                                                 wred_hw_params->max_thresh);

    avrg_th_diff = (wred_hw_params->max_thresh - wred_hw_params->min_thresh) *
        dnx_data_ingr_congestion.info.wred_granularity_get(unit);

    two_power_c1 = 1 << wred_hw_params->c1;
    if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported))
    {
        two_power_c1 += ((1 << wred_hw_params->c12) + (1 << wred_hw_params->c13));
    }

    COMPILER_64_SET(u64_1, 0, two_power_c1);
    COMPILER_64_UMUL_32(u64_1, avrg_th_diff);

    COMPILER_64_COPY(u64_2, u64_1);
    UTILEX_64_UDIV_32_WITH_REMAINDER(u64_2, DNX_INGRESS_CONGESTION_WRED_NORMALIZE_FACTOR, remainder);
    if (COMPILER_64_HI(u64_2) != 0)
    {
        wred->max_probability = 0xFFFFFFFF;
    }
    else
    {
        wred->max_probability = COMPILER_64_LO(u64_2);
    }
    assert(COMPILER_64_HI(u64_2) == 0);

    if (remainder > (DNX_INGRESS_CONGESTION_WRED_NORMALIZE_FACTOR / 2))
    {
        wred->max_probability++;
    }

    if (wred->max_probability > 100)
    {
        wred->max_probability = 100;
    }

    /*
     * Get continuous drop probability mode
     */
    if (dnx_data_ingr_congestion.
        info.feature_get(unit, dnx_data_ingr_congestion_info_wred_continuous_probabilty_supported))
    {
        wred->continuous_probability = wred_hw_params->continuous_probability;
    }
    else
    {
        wred->continuous_probability = 0;
    }
}

/**
 * \brief -
 * convert BCM WRED parameters to HW WRED data
 *
 * \note -
 * User specifies thresholds in bytes.
 * The thresholds are configured to HW in 16 words resolution (256 bytes)
 * Coefficients are computed from thresholds in words (16 bytes) -- the HW reads the thresholds and multiply them by 16
 */
void
dnx_ingress_congestion_convert_wred_to_hw_get(
    int unit,
    dnx_ingress_congestion_wred_info_t * wred,
    dnx_ingress_congestion_wred_hw_params_t * wred_hw_params)
{
    uint32 max_prob, calc, max_val_c1, calc_c1_power_2;
    int32 max_thresh_wred_granular, min_thresh_wred_granular, avrg_th_diff_wred_granular = 0;
    COMPILER_UINT64 u64_c2;

    uint32 max_packet_size;

    COMPILER_64_SET(u64_c2, 0, 0);

    max_packet_size = dnx_data_ingr_congestion.config.wred_packet_size_get(unit);;
    wred_hw_params->ignore_packet_size = (max_packet_size == 0 ? TRUE : FALSE);

    wred_hw_params->wred_en = wred->wred_en;

    /** Min average threshold */
    wred_hw_params->min_thresh =
        DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit,
                                                                 DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES,
                                                                 wred->min_avrg_th);

    /** Max average threshold */
    wred_hw_params->max_thresh =
        DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit,
                                                                 DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES,
                                                                 wred->max_avrg_th);

    max_thresh_wred_granular = wred_hw_params->max_thresh * dnx_data_ingr_congestion.info.wred_granularity_get(unit);
    min_thresh_wred_granular = wred_hw_params->min_thresh * dnx_data_ingr_congestion.info.wred_granularity_get(unit);

    /*
     * max_packet_size
     */
    calc =
        DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_ROUND_UP_GET(unit,
                                                                          DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES,
                                                                          max_packet_size) *
        dnx_data_ingr_congestion.info.wred_granularity_get(unit);

    wred_hw_params->c3 = (max_thresh_wred_granular == 0 ? 0 : utilex_log2_round_up(calc));

    /*
     * max_probability
     */
    max_prob = (wred->max_probability);

    if (max_prob >= 100)
    {
        max_prob = 99;
    }
    calc_c1_power_2 = DNX_INGRESS_CONGESTION_WRED_NORMALIZE_FACTOR * max_prob;

    max_val_c1 = 31;    /* dnx_sand_log2_round_down(0xFFFFFFFF) */

    avrg_th_diff_wred_granular = (max_thresh_wred_granular - min_thresh_wred_granular);

    if (avrg_th_diff_wred_granular == 0)
    {
        calc_c1_power_2 = (1 << max_val_c1);
        wred_hw_params->c1 = max_val_c1;
    }
    else
    {
        calc_c1_power_2 = UTILEX_DIV_ROUND_DOWN(calc_c1_power_2, avrg_th_diff_wred_granular);
        wred_hw_params->c1 = utilex_log2_round_down(calc_c1_power_2);
    }

    if ((!dnx_data_ingr_congestion.
         info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported))
        && (wred_hw_params->c1 < max_val_c1))
    {
        /*
         * Standard Precision - C1 optimization
         */
        uint32 now = 1 << (wred_hw_params->c1);
        uint32 changed = 1 << (wred_hw_params->c1 + 1);
        uint32 diff_with_now, diff_with_change;

        diff_with_change = changed - calc_c1_power_2;
        diff_with_now = calc_c1_power_2 - now;

        /*
         * Check if a bigger C1 gives closer result of the value we add.
         */
        if (diff_with_change < diff_with_now)
        {
            wred_hw_params->c1 += 1;
        }
    }

    wred_hw_params->c1 = UTILEX_RANGE_BOUND(wred_hw_params->c1, wred_hw_params->c1, max_val_c1);

    if (min_thresh_wred_granular > 0)
    {   /* This limit from above is HW restriction */
        max_val_c1 = UTILEX_DIV_ROUND_DOWN(0xFFFFFFFF, min_thresh_wred_granular);
        max_val_c1 = utilex_log2_round_down(max_val_c1);
        wred_hw_params->c1 = UTILEX_RANGE_BOUND(wred_hw_params->c1, wred_hw_params->c1, max_val_c1);
    }

    if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported))
    {
        uint32 calc_c12_power_2, calc_c13_power_2;


        calc_c12_power_2 = calc_c1_power_2 - (1 << wred_hw_params->c1);
        if (calc_c12_power_2 == 0)
        {
            uint32 original_c1;


            original_c1 = wred_hw_params->c1;
            wred_hw_params->c1 = (original_c1 < 1) ? 0 : (original_c1 - 1);
            wred_hw_params->c12 = (original_c1 < 2) ? 0 : (original_c1 - 2);
            wred_hw_params->c13 = (original_c1 < 2) ? 0 : (original_c1 - 2);
        }
        else
        {
            wred_hw_params->c12 = utilex_log2_round_down(calc_c12_power_2);
            wred_hw_params->c12 = UTILEX_RANGE_BOUND(wred_hw_params->c12, wred_hw_params->c12, max_val_c1);


            calc_c13_power_2 = calc_c12_power_2 - (1 << wred_hw_params->c12);
            if (calc_c13_power_2 == 0)
            {
                uint32 original_c12;

                original_c12 = wred_hw_params->c12;
                wred_hw_params->c12 = (original_c12 < 1) ? 0 : (original_c12 - 1);
                wred_hw_params->c13 = (original_c12 < 1) ? 0 : (original_c12 - 1);
            }
            else
            {
                wred_hw_params->c13 = utilex_log2_round_down(calc_c13_power_2);
                wred_hw_params->c13 = UTILEX_RANGE_BOUND(wred_hw_params->c13, wred_hw_params->c13, max_val_c1);
            }
        }
    }

    COMPILER_64_SET(u64_c2, 0, min_thresh_wred_granular);
    COMPILER_64_UMUL_32(u64_c2, (((uint32) 1 << wred_hw_params->c1)));

    wred_hw_params->c2 = COMPILER_64_LO(u64_c2);

    if (COMPILER_64_HI(u64_c2))
    {
        wred_hw_params->c2 = 0xFFFFFFFF;
    }
    wred_hw_params->c2 = (max_thresh_wred_granular == 0 ? 1 : wred_hw_params->c2);

    /*
     * Get continuous drop probability mode
     */
    if (dnx_data_ingr_congestion.
        info.feature_get(unit, dnx_data_ingr_congestion_info_wred_continuous_probabilty_supported))
    {
        wred_hw_params->continuous_probability = wred->continuous_probability;
    }
    else
    {
        wred_hw_params->continuous_probability = 0;
    }

}

/*
 * see .h file
 */
shr_error_e
dnx_ingress_cogestion_convert_fadt_dram_threshold_to_internal_get(
    int unit,
    dnx_ingress_congestion_dram_bound_resource_type_e resource,
    bcm_cosq_fadt_threshold_t * dram_thr,
    bcm_cosq_fadt_threshold_t * internal_thr)
{
    int is_res_range = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(internal_thr, 0, sizeof(bcm_cosq_fadt_threshold_t));

    is_res_range = dnx_data_ingr_congestion.dram_bound.resource_get(unit, resource)->is_resource_range;

    internal_thr->thresh_max =
        DNX_INGRESS_CONGESTION_CONVERT_DRAM_THRESHOLD_TO_INTERNAL_GET(unit, resource, dram_thr->thresh_max);
    internal_thr->thresh_min =
        DNX_INGRESS_CONGESTION_CONVERT_DRAM_THRESHOLD_TO_INTERNAL_GET(unit, resource, dram_thr->thresh_min);
    internal_thr->alpha = dram_thr->alpha;

    if (is_res_range)
    {
        internal_thr->resource_range_max =
            DNX_INGRESS_CONGESTION_CONVERT_DRAM_THRESHOLD_TO_INTERNAL_GET(unit, resource, dram_thr->resource_range_max);
        internal_thr->resource_range_min =
            DNX_INGRESS_CONGESTION_CONVERT_DRAM_THRESHOLD_TO_INTERNAL_GET(unit, resource, dram_thr->resource_range_min);
    }

    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_ingress_cogestion_convert_fadt_internal_to_dram_threshold_get(
    int unit,
    dnx_ingress_congestion_dram_bound_resource_type_e resource,
    bcm_cosq_fadt_threshold_t * internal_thr,
    bcm_cosq_fadt_threshold_t * dram_thr)
{
    int is_res_range = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(dram_thr, 0, sizeof(bcm_cosq_fadt_threshold_t));

    is_res_range = dnx_data_ingr_congestion.dram_bound.resource_get(unit, resource)->is_resource_range;

    dram_thr->thresh_max =
        DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_DRAM_THRESHOLD_GET(unit, resource, internal_thr->thresh_max);
    dram_thr->thresh_min =
        DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_DRAM_THRESHOLD_GET(unit, resource, internal_thr->thresh_min);
    dram_thr->alpha = internal_thr->alpha;

    if (is_res_range)
    {
        dram_thr->resource_range_max =
            DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_DRAM_THRESHOLD_GET(unit, resource,
                                                                          internal_thr->resource_range_max);
        dram_thr->resource_range_min =
            DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_DRAM_THRESHOLD_GET(unit, resource,
                                                                          internal_thr->resource_range_min);
    }

    SHR_FUNC_EXIT;
}
