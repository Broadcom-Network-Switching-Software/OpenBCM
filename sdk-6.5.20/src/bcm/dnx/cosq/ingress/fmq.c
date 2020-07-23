/** \file fmq.c
 * 
 *
 *  FMQ - Fabric Multicast Queue (part of IQS Module)
 *
 *  Created on: Nov 1, 2018
 *      Author: si888124
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
#include <bcm/types.h>
#include <bcm/cosq.h>

#include <bcm_int/dnx/cosq/cosq.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/cosq/ingress/ipq.h>
#include <bcm_int/dnx/cosq/scheduler/scheduler.h>
#include <bcm_int/dnx/algo/rate/algo_rates.h>

#include <shared/shrextend/shrextend_debug.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_iqs.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_ipq_alloc_mngr_access.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>

#include "fmq_dbal.h"

/*
 * }
 */

/*
 * Defines
 * {
 */
#define DNX_COSQ_FMQ_WEIGHT_WFQ_MODE   -1
#define DNX_COSQ_FMQ_WEIGHT_SP_MODE     0

#define DNX_COSQ_FMQ_SCH_MODE_BASIC     0
#define DNX_COSQ_FMQ_SCH_MODE_ENHANCED  1
/*
 * }
 */

/*
 * see .h file
 */
shr_error_e
dnx_fmq_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        /*
         * By default open top shared FMC shaper to maximum:
         * value of 1 means a credit will be sent every clock.
         * this shaper only exists when there is more than one core.
         */
        if (dnx_data_device.general.nof_cores_get(unit) > 1)
        {
            SHR_IF_ERR_EXIT(dnx_cosq_fmq_dbal_shaper_credit_rate_set(unit, DBAL_ENUM_FVAL_FMQ_SHAPER_TYPE_TOP_SHAPER,
                                                                     1));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * brief - validate gport and cosq for FMQ functions
 */
static shr_error_e
dnx_cosq_fmq_basic_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq)
{
    int core;
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "FMQ APIs are not supported on this device\n");
    }

    core = BCM_GPORT_SCHEDULER_CORE_GET(gport);
    if (core != BCM_CORE_ALL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "core must be BCM_CORE_ALL");
    }

    SHR_VAL_VERIFY(cosq, 0, _SHR_E_PARAM, "Cosq should be set to 0");

exit:
    SHR_FUNC_EXIT;
}

/*
 * brief - verify function for dnx_cosq_fmq_sched_set
 */
static shr_error_e
dnx_cosq_fmq_sched_set_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight)
{
    int max_weight;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_fmq_basic_verify(unit, gport, cosq));

    max_weight = dnx_data_iqs.credit.fmq_max_be_weight_get(unit);

    /** Validate mode and weight */
    SHR_RANGE_VERIFY(mode, DNX_COSQ_FMQ_WEIGHT_WFQ_MODE, DNX_COSQ_FMQ_WEIGHT_SP_MODE, _SHR_E_PARAM, "Invalid mode %d",
                     mode);

    if (mode == DNX_COSQ_FMQ_WEIGHT_WFQ_MODE)
    {
        SHR_RANGE_VERIFY(weight, 0, max_weight, _SHR_E_PARAM, "Invalid weight %d", weight);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_cosq_fmq_sched_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight)
{
    int is_wfq;
    int nof_be_classes;
    dbal_fields_e fmq_class_field_id;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_fmq_sched_set_verify(unit, gport, cosq, mode, weight));

    nof_be_classes = dnx_data_iqs.credit.fmq_nof_be_classes_get(unit);

    if (mode == DNX_COSQ_FMQ_WEIGHT_WFQ_MODE)
    {
        is_wfq = TRUE;

        if (BCM_COSQ_GPORT_IS_FMQ_BESTEFFORT0(gport))
        {
            fmq_class_field_id = DBAL_FIELD_CLASS_0_WEIGHT;
        }
        else if (BCM_COSQ_GPORT_IS_FMQ_BESTEFFORT1(gport))
        {
            fmq_class_field_id = DBAL_FIELD_CLASS_1_WEIGHT;
        }
        else    /* BCM_COSQ_GPORT_IS_FMQ_BESTEFFORT2(gport) */
        {
            fmq_class_field_id = DBAL_FIELD_CLASS_2_WEIGHT;
        }
        SHR_IF_ERR_EXIT(dnx_cosq_fmq_dbal_credit_weight_set(unit, fmq_class_field_id, weight));
    }
    else
    {
        /** Mode is SP only. i.e. MC2 > MC1 > MC0 */
        is_wfq = FALSE;

        for (fmq_class_field_id = DBAL_FIELD_CLASS_0_WEIGHT; fmq_class_field_id < nof_be_classes; fmq_class_field_id++)
        {
            SHR_IF_ERR_EXIT(dnx_cosq_fmq_dbal_credit_weight_set(unit, fmq_class_field_id, 0));
        }
    }

    /** set credit weight mode */
    SHR_IF_ERR_EXIT(dnx_cosq_fmq_dbal_credit_weight_mode_set(unit, is_wfq));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_cosq_fmq_sched_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *mode,
    int *weight)
{
    uint32 is_wfq, be_class_weight;
    dbal_fields_e fmq_class_field_id;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_fmq_basic_verify(unit, gport, cosq));

    /** Get credit weight mode */
    SHR_IF_ERR_EXIT(dnx_cosq_fmq_dbal_credit_weight_mode_get(unit, &is_wfq));

    if (is_wfq)
    {
        *mode = DNX_COSQ_FMQ_WEIGHT_WFQ_MODE;
        if (BCM_COSQ_GPORT_IS_FMQ_BESTEFFORT0(gport))
        {
            fmq_class_field_id = DBAL_FIELD_CLASS_0_WEIGHT;
        }
        else if (BCM_COSQ_GPORT_IS_FMQ_BESTEFFORT1(gport))
        {
            fmq_class_field_id = DBAL_FIELD_CLASS_1_WEIGHT;
        }
        else if (BCM_COSQ_GPORT_IS_FMQ_BESTEFFORT2(gport))
        {
            fmq_class_field_id = DBAL_FIELD_CLASS_2_WEIGHT;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid gport (0x%08x)", gport);
        }
        SHR_IF_ERR_EXIT(dnx_cosq_fmq_dbal_credit_weight_get(unit, fmq_class_field_id, &be_class_weight));
        *weight = be_class_weight;

    }
    else
    {
        /** Mode is SP only. i.e. MC2 > MC1 > MC0 */
        *mode = DNX_COSQ_FMQ_WEIGHT_SP_MODE;
        *weight = -1;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * brief - verify function for dnx_cosq_fmq_bandwidth_set
 */
static shr_error_e
dnx_cosq_fmq_bandwidth_set_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags)
{
    uint32 kbps_rate_max;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_fmq_basic_verify(unit, gport, cosq));

    kbps_rate_max = dnx_data_iqs.credit.fmq_shp_rate_max_get(unit);

    /** Validate kbits_sec_min */
    SHR_VAL_VERIFY(kbits_sec_min, 0, _SHR_E_PARAM, "minimum kbits per second parameter should be set to 0");

    /** Validate kbits_sec_max */
    if (kbits_sec_max != 0)
    {
        SHR_MAX_VERIFY(kbits_sec_max, kbps_rate_max, _SHR_E_PARAM,
                       "maximum kbits per second %u is out of bounds (0 - %u)", kbits_sec_max, kbps_rate_max);
    }

    /** Validate flags */
    SHR_VAL_VERIFY(flags, 0, _SHR_E_PARAM, "flags should be set to 0");

    if ((BCM_COSQ_GPORT_IS_FMQ_GUARANTEED(gport) || BCM_COSQ_GPORT_IS_FMQ_BESTEFFORT_AGR(gport)) &&
        kbits_sec_max == 0 && !dnx_data_iqs.credit.feature_get(unit, dnx_data_iqs_credit_fmq_shp_zero_rate_allowed))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "kbits_sec_max value %d is not supported on this device, please use non zero value\n",
                     kbits_sec_max);

    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_fmq_shaper_type_get(
    int unit,
    bcm_gport_t gport,
    dbal_enum_value_field_fmq_shaper_type_e * shaper_type)
{
    SHR_FUNC_INIT_VARS(unit);

    if (BCM_COSQ_GPORT_IS_FMQ_ROOT(gport))
    {
        *shaper_type = DBAL_ENUM_FVAL_FMQ_SHAPER_TYPE_ROOT_SHAPER;
    }
    else if (BCM_COSQ_GPORT_IS_FMQ_GUARANTEED(gport))
    {
        *shaper_type = DBAL_ENUM_FVAL_FMQ_SHAPER_TYPE_GUARANTEED_SHAPER;
    }
    else
    {
        /** BCM_COSQ_GPORT_IS_FMQ_BESTEFFORT_AGR(gport) */
        *shaper_type = DBAL_ENUM_FVAL_FMQ_SHAPER_TYPE_BE_SHAPER;
    }

    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_cosq_fmq_bandwidth_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags)
{
    dbal_enum_value_field_fmq_shaper_type_e shaper_type;
    uint32 credit_rate = 0, credit_size = 0;
    uint32 crdt_rate_mltp;
    uint32 crdt_rate_delta;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_fmq_bandwidth_set_verify(unit, gport, cosq, kbits_sec_min, kbits_sec_max, flags));

    crdt_rate_mltp = dnx_data_iqs.credit.fmq_shp_crdt_rate_mltp_get(unit);
    crdt_rate_delta = dnx_data_iqs.credit.fmq_shp_crdt_rate_delta_get(unit);

    if (kbits_sec_max == 0)
    {
        /** shaper is disabled */
        credit_rate = 0;
    }
    else
    {
        /** Get credit size */
        SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_group_get(unit, 0, &credit_size));

        /** Convert rate from Kbps to clock interval */
        SHR_IF_ERR_EXIT(dnx_algo_rates_kbits_per_sec_to_clocks
                        (unit, kbits_sec_max, credit_size * crdt_rate_mltp, &credit_rate));
        credit_rate -= crdt_rate_delta;

    }

    /** Get shaper type in DBAL */
    SHR_IF_ERR_EXIT(dnx_cosq_fmq_shaper_type_get(unit, gport, &shaper_type));

    /** set credit rate in HW */
    SHR_IF_ERR_EXIT(dnx_cosq_fmq_dbal_shaper_credit_rate_set(unit, shaper_type, credit_rate));

exit:
    SHR_FUNC_EXIT;
}

/*
 * brief - verify function for dnx_cosq_fmq_bandwidth_get
 */
static shr_error_e
dnx_cosq_fmq_bandwidth_get_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(kbits_sec_min, _SHR_E_PARAM, "kbits_sec_min");
    SHR_NULL_CHECK(kbits_sec_max, _SHR_E_PARAM, "kbits_sec_max");
    SHR_NULL_CHECK(flags, _SHR_E_PARAM, "flags");

    SHR_IF_ERR_EXIT(dnx_cosq_fmq_basic_verify(unit, gport, cosq));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_cosq_fmq_bandwidth_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags)
{
    uint32 credit_rate, credit_size;
    dbal_enum_value_field_fmq_shaper_type_e shaper_type;
    uint32 crdt_rate_mltp;
    uint32 crdt_rate_delta;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_fmq_bandwidth_get_verify(unit, gport, cosq, kbits_sec_min, kbits_sec_max, flags));

    crdt_rate_mltp = dnx_data_iqs.credit.fmq_shp_crdt_rate_mltp_get(unit);
    crdt_rate_delta = dnx_data_iqs.credit.fmq_shp_crdt_rate_delta_get(unit);

    /** Get shaper type in DBAL */
    SHR_IF_ERR_EXIT(dnx_cosq_fmq_shaper_type_get(unit, gport, &shaper_type));

    /** get credit rate from HW */
    SHR_IF_ERR_EXIT(dnx_cosq_fmq_dbal_shaper_credit_rate_get(unit, shaper_type, &credit_rate));

    if (credit_rate == 0)
    {
        /** shaper is disabled */
        *kbits_sec_max = 0;
    }
    else
    {
        /** Get credit size */
        SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_group_get(unit, 0, &credit_size));

        /** Convert rate from clock interval to Kbps */
        credit_rate += crdt_rate_delta;
        SHR_IF_ERR_EXIT(dnx_algo_rates_clocks_to_kbits_per_sec
                        (unit, credit_rate, credit_size * crdt_rate_mltp, kbits_sec_max));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * brief - verify function for dnx_cosq_control_set for types relevant for FMQ
 */
static shr_error_e
dnx_cosq_control_fmq_set_verify(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    int max_burst;
    int max_burst_limit;

    SHR_FUNC_INIT_VARS(unit);

    /** Gport Verify */
    SHR_IF_ERR_EXIT(dnx_cosq_fmq_basic_verify(unit, port, cosq));

    max_burst_limit = dnx_data_iqs.credit.fmq_max_burst_max_get(unit);

    switch (type)
    {
        case bcmCosqControlBandwidthBurstMax:
        {
            max_burst = arg;
            SHR_RANGE_VERIFY(max_burst, 0, max_burst_limit, _SHR_E_PARAM, "Max burst %d out of bounds\n", max_burst);
        }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type %d", type);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_cosq_control_fmq_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    uint32 max_burst;
    dbal_enum_value_field_fmq_shaper_type_e shaper_type;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_control_fmq_set_verify(unit, port, cosq, type, arg));

    /*
     * currently the only supported type is bcmCosqControlBandwidthBurstMax
     */

    /** Get shaper type in DBAL */
    SHR_IF_ERR_EXIT(dnx_cosq_fmq_shaper_type_get(unit, port, &shaper_type));

    /** set max burst in HW */
    max_burst = arg;
    SHR_IF_ERR_EXIT(dnx_cosq_fmq_dbal_shaper_max_burst_set(unit, shaper_type, max_burst));

exit:
    SHR_FUNC_EXIT;
}

/*
 * brief - verify function for dnx_cosq_control_get for types relevant for FMQ
 */
static shr_error_e
dnx_cosq_control_fmq_get_verify(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(arg, _SHR_E_PARAM, "arg");

    /** Gport Verify */
    SHR_IF_ERR_EXIT(dnx_cosq_fmq_basic_verify(unit, port, cosq));

    switch (type)
    {
        case bcmCosqControlBandwidthBurstMax:
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type %d", type);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_cosq_control_fmq_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    uint32 max_burst;
    dbal_enum_value_field_fmq_shaper_type_e shaper_type;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_control_fmq_get_verify(unit, port, cosq, type, arg));

    /*
     * currently the only supported type is bcmCosqControlBandwidthBurstMax
     */

    /** Get shaper type in DBAL */
    SHR_IF_ERR_EXIT(dnx_cosq_fmq_shaper_type_get(unit, port, &shaper_type));

    /** set max burst in HW */
    SHR_IF_ERR_EXIT(dnx_cosq_fmq_dbal_shaper_max_burst_get(unit, shaper_type, &max_burst));
    *arg = max_burst;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_fmq_scheduler_mode_set_verify(
    int unit,
    int mode)
{
    int is_fmq_enhance_scheduler_mode;
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "FMQ APIs are not supported on this device\n");
    }

    SHR_RANGE_VERIFY(mode, DNX_COSQ_FMQ_SCH_MODE_BASIC, DNX_COSQ_FMQ_SCH_MODE_ENHANCED, _SHR_E_PARAM,
                     "Mode %d is invalid\n", mode);

    /** check if we're in simple scheduler mode and if the FMQ range is occupied, fail in that case */
    SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.fmq.is_enhance_scheduler_mode.get(unit, &is_fmq_enhance_scheduler_mode));
    if ((is_fmq_enhance_scheduler_mode == FALSE) && (mode == DNX_COSQ_FMQ_SCH_MODE_ENHANCED))
    {
        int max_mc_queue;
        int is_allocated;
        int base_queue;
        /** min MC queue is always 0 */
        static int min_mc_queue = 0;
        SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.fmq.max_multicast_queue.get(unit, &max_mc_queue));

        /** iterate over base queues in range and check if allocated */
        for (base_queue = min_mc_queue; base_queue <= max_mc_queue;
             base_queue += dnx_data_ipq.queues.fmq_bundle_size_get(unit))
        {
            SHR_IF_ERR_EXIT(dnx_cosq_ipq_queue_is_allocated(unit, BCM_CORE_ALL, base_queue, &is_allocated));
            if (is_allocated)
            {
                break;
            }
        }

        if (is_allocated)
        {
            /** need to free FMQs before changing mode */
            SHR_ERR_EXIT(_SHR_E_RESOURCE,
                         "Need to free FMQs in order to change multicast scheduler mode. - base VOQ 0x%x is allocated",
                         base_queue);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * Purpose: Set FMQ Enhance mode
 * see .h file
 */
shr_error_e
dnx_cosq_fmq_scheduler_mode_set(
    int unit,
    int mode)
{
    uint32 hr_id_array[DNX_DATA_MAX_SCH_GENERAL_NOF_FMQ_CLASS] = { 0 };
    uint32 is_enhanced, hr_id, hr_id_is_valid;
    int nof_fmq_classes = dnx_data_sch.general.nof_fmq_class_get(unit);
    int mc_cls_idx;
    int core_id;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_fmq_scheduler_mode_set_verify(unit, mode));

    SHR_IF_ERR_EXIT(dnx_cosq_fmq_dbal_enhanced_mode_get(unit, &is_enhanced));

    if (is_enhanced && mode == DNX_COSQ_FMQ_SCH_MODE_BASIC)
    {
        /*
         * go from enhanced to basic mode:
         * 4 queues connected directly to credit generator - no HRs
         */

        /** 1. Disable enhance mode */
        is_enhanced = FALSE;
        SHR_IF_ERR_EXIT(dnx_cosq_fmq_dbal_enhanced_mode_set(unit, is_enhanced));

        /** Update sw_state */
        SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.fmq.is_enhance_scheduler_mode.set(unit, is_enhanced));

        DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
        {
            /** 2. Dealloc HRs of FMQ scheduling hierarchy */
            for (mc_cls_idx = 0; mc_cls_idx < nof_fmq_classes; mc_cls_idx++)
            {
                SHR_IF_ERR_EXIT(dnx_cosq_fmq_dbal_enhanced_mode_hr_id_get
                                (unit, core_id, mc_cls_idx, &hr_id, &hr_id_is_valid));
                hr_id_array[mc_cls_idx] = hr_id;
            }
            SHR_IF_ERR_EXIT(dnx_scheduler_fmq_hr_deallocate(unit, core_id, nof_fmq_classes, hr_id_array));

            /** 3. Disable MC ports configuration */
            hr_id_is_valid = FALSE;
            for (mc_cls_idx = 0; mc_cls_idx < nof_fmq_classes; mc_cls_idx++)
            {
                SHR_IF_ERR_EXIT(dnx_cosq_fmq_dbal_enhanced_mode_hr_id_set
                                (unit, core_id, mc_cls_idx, 0, hr_id_is_valid));
            }
        }
    }
    else if (is_enhanced == FALSE && mode == DNX_COSQ_FMQ_SCH_MODE_ENHANCED)
    {
        /*
         * go from basic to enhanced mode:
         * credit generator generates credits for 4 HRs in scheduler
         */

        /** 1. Enable enhance */
        is_enhanced = TRUE;
        SHR_IF_ERR_EXIT(dnx_cosq_fmq_dbal_enhanced_mode_set(unit, is_enhanced));

        /** Update sw_state */
        SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.fmq.is_enhance_scheduler_mode.set(unit, is_enhanced));

        DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
        {
            /** 2. Allocate HRs for FMQ scheduling hierarchy */
            SHR_IF_ERR_EXIT(dnx_scheduler_fmq_hr_allocate(unit, core_id, nof_fmq_classes, hr_id_array));

            /** 3. Set MC ports configuration */
            for (mc_cls_idx = 0; mc_cls_idx < nof_fmq_classes; mc_cls_idx++)
            {
                hr_id = hr_id_array[mc_cls_idx];
                hr_id_is_valid = TRUE;
                SHR_IF_ERR_EXIT(dnx_cosq_fmq_dbal_enhanced_mode_hr_id_set
                                (unit, core_id, mc_cls_idx, hr_id, hr_id_is_valid));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Purpose: Get FMQ Enhance mode
 * see .h file
 */
shr_error_e
dnx_cosq_fmq_scheduler_mode_get(
    int unit,
    int *mode)
{
    uint32 is_enhanced;
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "FMQ APIs are not supported on this device\n");
    }

    SHR_NULL_CHECK(mode, _SHR_E_PARAM, "mode");

    SHR_IF_ERR_EXIT(dnx_cosq_fmq_dbal_enhanced_mode_get(unit, &is_enhanced));

    *mode = (is_enhanced) ? DNX_COSQ_FMQ_SCH_MODE_ENHANCED : DNX_COSQ_FMQ_SCH_MODE_BASIC;

exit:
    SHR_FUNC_EXIT;
}
