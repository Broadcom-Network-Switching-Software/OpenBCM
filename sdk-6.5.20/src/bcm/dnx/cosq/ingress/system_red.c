/** \file system_red.c
 * 
 *
 * System RED functionality for DNX. \n
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

/*
 * Include files.
 * {
 */
#include <bcm/fabric.h>

#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/cosq/ingress/system_red.h>
#include <bcm_int/dnx/cosq/ingress/ingress_congestion.h>

#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_integer_arithmetic.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_system_red.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_congestion.h>

#include "system_red_dbal.h"
#include "ingress_congestion.h"
/*
 * }
 */

/*
 * MACROs
 * {
 */
/*
 * }
 */

/*
 * static declarations
 * {
 */
/*
 * }
 */

/*
 * Functions
 * {
 */

/*
 * see .h file
 */
shr_error_e
dnx_system_red_init(
    int unit)
{
    int is_system_red_enabled, latency_based_addmission;
    SHR_FUNC_INIT_VARS(unit);

    /** Check if system RED soc property is enabled */
    is_system_red_enabled = dnx_data_system_red.config.enable_get(unit);

    /*
     * Check if latency based admission feature is enabled- otherwise the HW field doesn't exist. (system RED and
     * latency based admission are sharing resources) 
     */
    latency_based_addmission =
        dnx_data_system_red.info.feature_get(unit, dnx_data_system_red_info_is_share_res_with_latency_based_admission);

    if (latency_based_addmission)
    {
        SHR_IF_ERR_EXIT(dnx_system_red_dbal_enable_set(unit, is_system_red_enabled));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * brief - verify function for dnx_system_red_aging_config_set
 */
static shr_error_e
dnx_system_red_aging_config_set_verify(
    int unit,
    bcm_fabric_config_discard_t * discard)
{
    uint32 supported_flags = BCM_FABRIC_CONFIG_DISCARD_AGING_ACTION_ONLY;
    uint32 max_ing_age = dnx_data_system_red.info.max_ing_aging_period_get(unit);
    uint32 max_sch_age = dnx_data_system_red.info.max_sch_aging_period_get(unit);
    uint32 sys_red_enabled;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(discard, _SHR_E_PARAM, "discard");

    /*
     * Verify system RED soc property is enabled
     */
    sys_red_enabled = dnx_data_system_red.config.enable_get(unit);
    if (!sys_red_enabled)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "System RED Soc Property is not enabled. see soc property system_red_enable\n");
    }

    SHR_MASK_VERIFY(discard->flags, supported_flags, _SHR_E_PARAM, "Unsupported flag given");

    SHR_RANGE_VERIFY(discard->enable, 0, 1, _SHR_E_PARAM, "Enable field expected values 0 or 1");

    switch (discard->age_mode)
    {
        case bcmFabricAgeModeReset:
        case bcmFabricAgeModeDecrement:
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Age Mode %d not supported\n", discard->age_mode);
    }

    /*
     * Verify aging period against the lesser between the ingress aging period and sch aging period
     */
    SHR_RANGE_VERIFY(discard->age, 0, UTILEX_MIN(max_ing_age, max_sch_age), _SHR_E_PARAM, "Aging period out of bounds");

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_system_red_aging_config_set(
    int unit,
    bcm_fabric_config_discard_t * discard)
{
    dnx_system_red_dbal_aging_t aging_params;
    uint32 nof_tm_ports = dnx_data_port.general.nof_tm_ports_get(unit);
    uint32 factor = dnx_data_system_red.info.sch_aging_period_factor_get(unit);
    uint32 ing_aging_period, aging_period_clocks;
    dnxcmn_time_t time;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_system_red_aging_config_set_verify(unit, discard));

    aging_params.enable = discard->enable;
    aging_params.is_only_aging_dec = discard->flags & BCM_FABRIC_CONFIG_DISCARD_AGING_ACTION_ONLY;

    /*
     * convert the aging period from milliseconds to HW value
     * aging_period = nof_ports * 4 clocks * HW value
     */
    time.time_units = DNXCMN_TIME_UNIT_MILISEC;
    COMPILER_64_SET(time.time, 0, discard->age);
    SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &time, &aging_period_clocks));

    aging_params.sch_aging_period = UTILEX_DIV_ROUND_UP(aging_period_clocks, nof_tm_ports * factor);

    if (discard->age_mode == bcmFabricAgeModeReset)
    {
        aging_params.aging_mode = DBAL_ENUM_FVAL_SYSTEM_RED_AGING_MODE_RESET;
    }
    else
    {
        /** bcmFabricAgeModeDecrement */
        aging_params.aging_mode = DBAL_ENUM_FVAL_SYSTEM_RED_AGING_MODE_DECREMENT;
    }

    SHR_IF_ERR_EXIT(dnx_system_red_dbal_sch_aging_params_set(unit, &aging_params));

    /*
     * set the same aging period on ingress side.
     * here the HW value is in 16k clocks units.
     */
    ing_aging_period = UTILEX_DIV_ROUND_UP(aging_period_clocks, 16 * 1024);
    SHR_IF_ERR_EXIT(dnx_system_red_dbal_ing_params_set(unit, ing_aging_period));
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_system_red_aging_config_get(
    int unit,
    bcm_fabric_config_discard_t * discard)
{
    dnx_system_red_dbal_aging_t aging_params;
    uint32 nof_tm_ports = dnx_data_port.general.nof_tm_ports_get(unit);
    uint32 factor = dnx_data_system_red.info.sch_aging_period_factor_get(unit);
    uint32 aging_period_clocks;
    dnxcmn_time_t time;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(discard, _SHR_E_PARAM, "discard");

    SHR_IF_ERR_EXIT(dnx_system_red_dbal_sch_aging_params_get(unit, &aging_params));

    discard->enable = aging_params.enable;
    discard->flags = aging_params.is_only_aging_dec ? BCM_FABRIC_CONFIG_DISCARD_AGING_ACTION_ONLY : 0;

    /*
     * convert the aging period from HW value to milliseconds
     *  aging_period = nof_ports * 4 clocks * HW value
     */
    aging_period_clocks = aging_params.sch_aging_period * nof_tm_ports * factor;

    SHR_IF_ERR_EXIT(dnxcmn_clock_cycles_to_time_get(unit, aging_period_clocks, DNXCMN_TIME_UNIT_MILISEC, &time));
    discard->age = COMPILER_64_LO(time.time);

    if (aging_params.aging_mode == DBAL_ENUM_FVAL_SYSTEM_RED_AGING_MODE_RESET)
    {
        discard->age_mode = bcmFabricAgeModeReset;
    }
    else
    {
        /** DBAL_ENUM_FVAL_SYSTEM_RED_AGING_MODE_DECREMENT */
        discard->age_mode = bcmFabricAgeModeDecrement;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * brief - verify red-q-size used for threshold.
 */
static shr_error_e
dnx_system_red_q_size_thr_verify(
    int unit,
    int red_q_size_thr)
{
    uint32 max_red_q_size = dnx_data_system_red.info.max_red_q_size_get(unit);
    uint32 max_red_q_size_thr;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * there are 16 RED-Q-Size values controlled by 15 thresholds (indexed 0-14)
     */
    max_red_q_size_thr = max_red_q_size - 1;

    SHR_RANGE_VERIFY(red_q_size_thr, 0, max_red_q_size_thr, _SHR_E_PARAM, "Invalid color levels specified %d\n",
                     red_q_size_thr);

exit:
    SHR_FUNC_EXIT;
}

/*
 * brief - verify function for dnx_system_red_voq_max_size_set
 */
static shr_error_e
dnx_system_red_voq_max_size_set_verify(
    int unit,
    bcm_gport_t gport,
    int red_q_size,
    bcm_cosq_gport_size_t * gport_size)
{
    uint32 max_res, sys_red_enabled;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify system RED soc property is enabled
     */
    sys_red_enabled = dnx_data_system_red.config.enable_get(unit);
    if (!sys_red_enabled)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "System RED Soc Property is not enabled. see soc property system_red_enable\n");
    }

    SHR_IF_ERR_EXIT(dnx_system_red_q_size_thr_verify(unit, red_q_size));

    /*
     * Verify gport size
     */
    max_res = dnx_data_ingr_congestion.info.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES)->max;
    if (gport_size->size_max > max_res)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Gport size_max %d exceeds max resources %d", gport_size->size_max, max_res);
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_system_red_voq_max_size_set(
    int unit,
    bcm_gport_t gport,
    int red_q_size,
    bcm_cosq_gport_size_t * gport_size)
{
    uint32 rate_class;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_system_red_voq_max_size_set_verify(unit, gport, red_q_size, gport_size));

    rate_class = BCM_GPORT_PROFILE_GET(gport);
    SHR_IF_ERR_EXIT(dnx_system_red_dbal_voq_size_range_to_red_q_size_map_set
                    (unit, rate_class, red_q_size, gport_size->size_max));
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_system_red_voq_max_size_get(
    int unit,
    bcm_gport_t gport,
    int red_q_size,
    bcm_cosq_gport_size_t * gport_size)
{
    uint32 rate_class;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_system_red_q_size_thr_verify(unit, red_q_size));

    sal_memset(gport_size, 0, sizeof(bcm_cosq_gport_size_t));

    rate_class = BCM_GPORT_PROFILE_GET(gport);
    SHR_IF_ERR_EXIT(dnx_system_red_dbal_voq_size_range_to_red_q_size_map_get
                    (unit, rate_class, red_q_size, &gport_size->size_max));
exit:
    SHR_FUNC_EXIT;
}

/*
 * brief - convert range flags to range id.
 * used in dnx_system_red_free_res_threshold_set/get
 */
static shr_error_e
dnx_system_red_free_res_flags_to_range_id_get(
    int unit,
    int flags,
    uint32 *range_id)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_COSQ_THRESHOLD_RANGE_0)
    {
        *range_id = 0;
    }
    else if (flags & BCM_COSQ_THRESHOLD_RANGE_1)
    {
        *range_id = 1;
    }
    else if (flags & BCM_COSQ_THRESHOLD_RANGE_2)
    {
        *range_id = 2;
    }
    else
    {
        /** BCM_COSQ_THRESHOLD_RANGE_3*/
        *range_id = 3;
    }
    SHR_FUNC_EXIT;
}

/*
 * brief - convert threshold type bcm type to system red enum resource.
 * used in dnx_system_red_free_res_threshold_set/get
 */
static shr_error_e
dnx_system_red_free_res_thr_type_to_res_type_get(
    int unit,
    bcm_cosq_threshold_type_t threshold_type,
    dnx_system_red_resource_type_e * resource)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (threshold_type)
    {
        case bcmCosqThresholdOcbPacketDescriptorBuffers:
            *resource = DNX_SYSTEM_RED_RESOURCE_SRAM_PDBS;
            break;
        case bcmCosqThresholdOcbDbuffs:
            *resource = DNX_SYSTEM_RED_RESOURCE_SRAM_BUFFERS;
            break;
        case bcmCosqThresholdBufferDescriptorBuffers:
            *resource = DNX_SYSTEM_RED_RESOURCE_DRAM_BDBS;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "threshold type %d not supported", threshold_type);
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * brief - verify function for dnx_system_red_free_res_threshold_set/get
 */
static shr_error_e
dnx_system_red_free_res_threshold_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_threshold_t * threshold,
    int is_set)
{
    uint32 supported_flags, range_flags, is_dram_present;
    uint32 max_red_q_size = dnx_data_system_red.info.max_red_q_size_get(unit);
    uint32 max_res, sys_red_enabled;
    dnx_system_red_resource_type_e resource;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(threshold, _SHR_E_PARAM, "threshold");

    /** Verify Gport and Cosq */
    SHR_VAL_VERIFY(gport, 0, _SHR_E_PARAM, "Gport must be set to 0");
    SHR_VAL_VERIFY(cosq, 0, _SHR_E_PARAM, "Cosq must be set to 0");

    /** Verify Flags */
    range_flags =
        BCM_COSQ_THRESHOLD_RANGE_0 | BCM_COSQ_THRESHOLD_RANGE_1 | BCM_COSQ_THRESHOLD_RANGE_2 |
        BCM_COSQ_THRESHOLD_RANGE_3;
    supported_flags = range_flags | BCM_COSQ_THRESHOLD_COLOR_SYSTEM_RED;
    SHR_MASK_VERIFY(threshold->flags, supported_flags, _SHR_E_PARAM, "Invalid Flag combination");

    /** Verify exactly one range flag is set */
    SHR_NOF_SET_BITS_IN_RANGE_VERIFY(threshold->flags, range_flags, 1, 1, _SHR_E_PARAM,
                                     "Exactly one range flag should be set\n");

    /** Verify threshold type */
    switch (threshold->type)
    {
        case bcmCosqThresholdOcbPacketDescriptorBuffers:
        case bcmCosqThresholdBufferDescriptorBuffers:
        case bcmCosqThresholdOcbDbuffs:
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "threshold type %d not supported", threshold->type);
    }

    if (is_set)
    {
        /*
         * Verify system RED soc property is enabled
         */
        sys_red_enabled = dnx_data_system_red.config.enable_get(unit);
        if (!sys_red_enabled)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "System RED Soc Property is not enabled. see soc property system_red_enable\n");
        }

        /** Verify RED-Q-Size (DP)*/
        SHR_RANGE_VERIFY(threshold->dp, 0, max_red_q_size, _SHR_E_PARAM,
                         "Invalid red-q-size %d specified. Range is 0-%d\n", threshold->dp, max_red_q_size);

        /** Verify threshold value */
        SHR_IF_ERR_EXIT(dnx_system_red_free_res_thr_type_to_res_type_get(unit, threshold->type, &resource));
        max_res = dnx_data_system_red.info.resource_get(unit, resource)->max;

        if (threshold->value > max_res)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "threshold value %d is out of bounds. max resource level is %d",
                         threshold->value, max_res);
        }

        /** BDBs thresholds are irrelevant in systems without DRAM */
        is_dram_present = dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap;
        if (!is_dram_present && resource == DNX_SYSTEM_RED_RESOURCE_DRAM_BDBS)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "DRAM BDBs threshold cannot be set on a system without DRAM\n");
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_system_red_free_res_threshold_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_threshold_t * threshold)
{
    int is_set = TRUE;
    uint32 range;
    uint32 max_free_res_thr_ranges = dnx_data_system_red.info.max_free_res_thr_range_get(unit);
    dnx_system_red_resource_type_e resource;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_system_red_free_res_threshold_verify(unit, gport, cosq, threshold, is_set));

    /** get resource type */
    SHR_IF_ERR_EXIT(dnx_system_red_free_res_thr_type_to_res_type_get(unit, threshold->type, &resource));
    /** get range id */
    SHR_IF_ERR_EXIT(dnx_system_red_free_res_flags_to_range_id_get(unit, threshold->flags, &range));

    /** the last range max_value is just max_res, so we don't set threshold value for it.*/
    if (range < max_free_res_thr_ranges)
    {
        /** set threshold for the specified range */
        SHR_IF_ERR_EXIT(dnx_system_red_dbal_free_res_thr_set(unit, resource, range, threshold->value));
    }

    /** set RED-Q-Size to map to the specified range */
    SHR_IF_ERR_EXIT(dnx_system_red_dbal_free_res_range_to_red_q_size_map_set(unit, resource, range, threshold->dp));
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_system_red_free_res_threshold_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_threshold_t * threshold)
{
    int is_set = FALSE;
    uint32 range;
    uint32 max_free_res_thr_ranges = dnx_data_system_red.info.max_free_res_thr_range_get(unit);
    dnx_system_red_resource_type_e resource;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_system_red_free_res_threshold_verify(unit, gport, cosq, threshold, is_set));

    /** get resource type */
    SHR_IF_ERR_EXIT(dnx_system_red_free_res_thr_type_to_res_type_get(unit, threshold->type, &resource));
    /** get range id */
    SHR_IF_ERR_EXIT(dnx_system_red_free_res_flags_to_range_id_get(unit, threshold->flags, &range));

    /** get RED-Q-Size map to the specified range */
    SHR_IF_ERR_EXIT(dnx_system_red_dbal_free_res_range_to_red_q_size_map_get(unit, resource, range, &threshold->dp));

    if (range < max_free_res_thr_ranges)
    {
        /** get threshold for the specified range */
        SHR_IF_ERR_EXIT(dnx_system_red_dbal_free_res_thr_get(unit, resource, range, &threshold->value));
    }
    else
    {
        /** last range max value is max_res */
        threshold->value = dnx_data_system_red.info.resource_get(unit, resource)->max;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * brief - verify function for dnx_system_red_discard_set
 */
static shr_error_e
dnx_system_red_discard_set_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_gport_discard_t * discard)
{
    uint32 max_red_q_size = dnx_data_system_red.info.max_red_q_size_get(unit);
    uint32 sys_red_enabled;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify system RED soc property is enabled
     */
    sys_red_enabled = dnx_data_system_red.config.enable_get(unit);
    if (!sys_red_enabled)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "System RED Soc Property is not enabled. see soc property system_red_enable\n");
    }

    /*
     * other arguments were validated in caller function
     * required to validate only the min_threshold and max_threshold
     */
    SHR_RANGE_VERIFY(discard->min_thresh, 0, max_red_q_size, _SHR_E_PARAM,
                     "Invalid red-q-size %d specified. Range is 0-%d\n", discard->min_thresh, max_red_q_size);

    SHR_RANGE_VERIFY(discard->max_thresh, 0, max_red_q_size, _SHR_E_PARAM,
                     "Invalid red-q-size %d specified. Range is 0-%d\n", discard->max_thresh, max_red_q_size);
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_system_red_discard_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_gport_discard_t * discard)
{
    dnx_ingress_congestion_rate_class_threshold_key_t dp_info;
    dnx_system_red_dbal_discard_params_t discard_params;
    uint32 rate_class, dp;
    int dp_index;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_system_red_discard_set_verify(unit, gport, cosq, discard));

    rate_class = BCM_GPORT_PROFILE_GET(gport);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_discard_key_get(unit, gport, discard, &dp_info));

    discard_params.enable = discard->flags & BCM_COSQ_DISCARD_ENABLE;
    if (discard->flags & BCM_COSQ_DISCARD_PROBABILITY1)
    {
        /** probability low */
        discard_params.admit_thr = discard->min_thresh;
        discard_params.prob_thr = discard->max_thresh;
        discard_params.drop_prob_low = discard->drop_probability;
        for (dp_index = 0; dp_index < dp_info.nof_dp; ++dp_index)
        {
            dp = dp_info.dp[dp_index];
            SHR_IF_ERR_EXIT(dnx_system_red_dbal_discard_prob_low_set(unit, rate_class, dp, &discard_params));
        }
    }
    else
    {
        /** BCM_COSQ_DISCARD_PROBABILITY2 */
        /** probability high */
        discard_params.prob_thr = discard->min_thresh;
        discard_params.reject_thr = discard->max_thresh;
        discard_params.drop_prob_high = discard->drop_probability;
        for (dp_index = 0; dp_index < dp_info.nof_dp; ++dp_index)
        {
            dp = dp_info.dp[dp_index];
            SHR_IF_ERR_EXIT(dnx_system_red_dbal_discard_prob_high_set(unit, rate_class, dp, &discard_params));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_system_red_discard_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_gport_discard_t * discard)
{
    dnx_ingress_congestion_rate_class_threshold_key_t dp_info;
    dnx_system_red_dbal_discard_params_t discard_params;
    uint32 rate_class, dp;
    SHR_FUNC_INIT_VARS(unit);

    rate_class = BCM_GPORT_PROFILE_GET(gport);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_discard_key_get(unit, gport, discard, &dp_info));
    dp = dp_info.dp[0];

    SHR_IF_ERR_EXIT(dnx_system_red_dbal_discard_params_get(unit, rate_class, dp, &discard_params));

    if (discard->flags & BCM_COSQ_DISCARD_PROBABILITY1)
    {
        discard->min_thresh = discard_params.admit_thr;
        discard->max_thresh = discard_params.prob_thr;
        discard->drop_probability = discard_params.drop_prob_low;
    }
    else
    {
        /** BCM_COSQ_DISCARD_PROBABILITY2 */
        discard->min_thresh = discard_params.prob_thr;
        discard->max_thresh = discard_params.reject_thr;
        discard->drop_probability = discard_params.drop_prob_high;
    }

    discard->flags |= discard_params.enable ? BCM_COSQ_DISCARD_ENABLE : 0;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_system_red_sysport_verify(
    int unit,
    uint32 system_port)
{
    uint32 max_sys_port = dnx_data_device.general.invalid_system_port_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (system_port > max_sys_port)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid System port %d. valid range is 0-%d", system_port, max_sys_port);
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_system_red_sysport_red_status_get(
    int unit,
    uint32 system_port,
    uint32 *red_index)
{
    int core;
    uint32 core_red_index = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_system_red_sysport_verify(unit, system_port));

    *red_index = 0;
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        SHR_IF_ERR_EXIT(dnx_system_red_dbal_sysport_red_status_get(unit, core, system_port, &core_red_index));
        *red_index = UTILEX_MAX(*red_index, core_red_index);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_system_red_global_red_status_get(
    int unit,
    uint32 *current_red_index,
    uint32 *max_red_index)
{
    int core;
    uint32 core_curr_red_index = 0, core_max_red_index = 0;
    SHR_FUNC_INIT_VARS(unit);

    *current_red_index = 0;
    *max_red_index = 0;

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        SHR_IF_ERR_EXIT(dnx_system_red_dbal_global_red_status_get
                        (unit, core, &core_curr_red_index, &core_max_red_index));
        *current_red_index = UTILEX_MAX(*current_red_index, core_curr_red_index);
        *max_red_index = UTILEX_MAX(*max_red_index, core_max_red_index);
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * }
 */
