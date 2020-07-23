

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <shared/shrextend/shrextend_error.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <shared/utilex/utilex_u64.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_congestion.h>

#include "bcm_int/dnx/cosq/ingress/ingress_congestion.h"
#include "ingress_congestion.h"
#include "ingress_congestion_convert.h"


#define DNX_INGRESS_CONGESTION_WRED_NORMALIZE_FACTOR  (0x28F5C29)


void
dnx_ingress_congestion_convert_hw_to_wred_get(
    int unit,
    dnx_ingress_congestion_wred_hw_params_t * wred_hw_params,
    dnx_ingress_congestion_wred_info_t * wred)
{
    UTILEX_U64 u64_1, u64_2;
    uint32 remainder;
    uint32 avrg_th_diff, two_power_c1;

    wred->wred_en = wred_hw_params->wred_en;

    
    wred->min_avrg_th =
        DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_THRESHOLD_GET(unit,
                                                                 DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES,
                                                                 wred_hw_params->min_thresh);

    
    wred->max_avrg_th =
        DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_THRESHOLD_GET(unit,
                                                                 DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES,
                                                                 wred_hw_params->max_thresh);

    avrg_th_diff = (wred_hw_params->max_thresh - wred_hw_params->min_thresh) *
        dnx_data_ingr_congestion.info.wred_granularity_get(unit);

    two_power_c1 = 1 << wred_hw_params->c1;
    

    utilex_u64_multiply_longs(two_power_c1, avrg_th_diff, &u64_1);
    remainder = utilex_u64_devide_u64_long(&u64_1, DNX_INGRESS_CONGESTION_WRED_NORMALIZE_FACTOR, &u64_2);
    utilex_u64_to_long(&u64_2, &wred->max_probability);

    if (remainder > (DNX_INGRESS_CONGESTION_WRED_NORMALIZE_FACTOR / 2))
    {
        wred->max_probability++;
    }

    if (wred->max_probability > 100)
    {
        wred->max_probability = 100;
    }
}


void
dnx_ingress_congestion_convert_wred_to_hw_get(
    int unit,
    dnx_ingress_congestion_wred_info_t * wred,
    dnx_ingress_congestion_wred_hw_params_t * wred_hw_params)
{
    uint32 max_prob, calc, max_val_c1;
    int32 max_thresh_wred_granular, min_thresh_wred_granular, avrg_th_diff_wred_granular = 0;
    uint32 trunced;
    UTILEX_U64 u64_c2;

    uint32 max_packet_size;

    utilex_u64_clear(&u64_c2);
    trunced = FALSE;

    max_packet_size = dnx_data_ingr_congestion.config.wred_packet_size_get(unit);;
    wred_hw_params->ignore_packet_size = (max_packet_size == 0 ? TRUE : FALSE);

    wred_hw_params->wred_en = wred->wred_en;

    
    wred_hw_params->min_thresh =
        DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit,
                                                                 DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES,
                                                                 wred->min_avrg_th);

    
    wred_hw_params->max_thresh =
        DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit,
                                                                 DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES,
                                                                 wred->max_avrg_th);

    max_thresh_wred_granular = wred_hw_params->max_thresh * dnx_data_ingr_congestion.info.wred_granularity_get(unit);
    min_thresh_wred_granular = wred_hw_params->min_thresh * dnx_data_ingr_congestion.info.wred_granularity_get(unit);

    
    calc =
        DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_ROUND_UP_GET(unit,
                                                                          DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES,
                                                                          max_packet_size) *
        dnx_data_ingr_congestion.info.wred_granularity_get(unit);

    wred_hw_params->c3 = (max_thresh_wred_granular == 0 ? 0 : utilex_log2_round_up(calc));

    
    max_prob = (wred->max_probability);

    
    if (max_prob >= 100)
    {
        max_prob = 99;
    }
    calc = DNX_INGRESS_CONGESTION_WRED_NORMALIZE_FACTOR * max_prob;

    
    max_val_c1 = 31;    

    avrg_th_diff_wred_granular = (max_thresh_wred_granular - min_thresh_wred_granular);

    if (avrg_th_diff_wred_granular == 0)
    {
        wred_hw_params->c1 = max_val_c1;
    }
    else
    {
        calc = UTILEX_DIV_ROUND_DOWN(calc, avrg_th_diff_wred_granular);
        wred_hw_params->c1 = utilex_log2_round_down(calc);
    }

    if (wred_hw_params->c1 < max_val_c1)
    {
        
        uint32 now = 1 << (wred_hw_params->c1);
        uint32 changed = 1 << (wred_hw_params->c1 + 1);
        uint32 diff_with_now, diff_with_change;

        diff_with_change = changed - calc;
        diff_with_now = calc - now;
        if (diff_with_change < diff_with_now)
        {
            wred_hw_params->c1 += 1;
        }
    }

    wred_hw_params->c1 = UTILEX_RANGE_BOUND(wred_hw_params->c1, wred_hw_params->c1, max_val_c1);

    if (min_thresh_wred_granular > 0)
    {   
        max_val_c1 = UTILEX_DIV_ROUND_DOWN(0xFFFFFFFF, min_thresh_wred_granular);
        max_val_c1 = utilex_log2_round_down(max_val_c1);
        wred_hw_params->c1 = UTILEX_RANGE_BOUND(wred_hw_params->c1, wred_hw_params->c1, max_val_c1);
    }

    

    utilex_u64_multiply_longs(min_thresh_wred_granular, (1 << wred_hw_params->c1), &u64_c2);

    trunced = utilex_u64_to_long(&u64_c2, &wred_hw_params->c2);

    if (trunced)
    {
        wred_hw_params->c2 = 0xFFFFFFFF;
    }
    wred_hw_params->c2 = (max_thresh_wred_granular == 0 ? 1 : wred_hw_params->c2);

}


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
