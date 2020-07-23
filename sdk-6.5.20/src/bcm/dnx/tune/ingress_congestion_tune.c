/** \file ingress_congestion_tune.c
 * Tuning of Ingress Congestion parameters
 * 
 * NOTE: as this code is invoked from iside SDK, 
 * API invocation is done via bcm_dnx_XXX functions.
 * When this code is taken outside SDK, 
 * these calls should be replaced by bcm_XXX functions.
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INITSEQDNX

/*
 * Include files.
 * {
 */
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm/cosq.h>
#include <bcm_int/dnx/cosq/cosq.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/tune/tune.h>
#include <bcm_int/dnx/cosq/ingress/ingress_congestion.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_congestion.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_iqs.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_system_red.h>

/*
 * }
 * Include files.
 */

/** total number of Drop Precedence levels */
#define DNX_TUNE_NOF_DP 4
/** total number of System RED indexes */
#define DNX_TUNE_NOF_SYSTEM_RED_INDEXES 15

/** Resource amount (per core) - Maximal Bytes, SRAM Buffers and SRAM Packet Descriptors */
/** dnx data and SDK internal enum are used to obtain total available resources of the device */
#define DNX_TUNE_INGRESS_CONGESTION_BYTES_MAX(unit) dnx_data_ingr_congestion.info.resource_get(unit,DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES)->max
#define DNX_TUNE_INGRESS_CONGESTION_SRAM_BUFFERS_MAX(unit) dnx_data_ingr_congestion.info.resource_get(unit,DNX_INGRESS_CONGESTION_RESOURCE_SRAM_BUFFERS)->max
#define DNX_TUNE_INGRESS_CONGESTION_SRAM_PDS_MAX(unit) dnx_data_ingr_congestion.info.resource_get(unit,DNX_INGRESS_CONGESTION_RESOURCE_SRAM_PDS)->max

/* 
 * ---------------------------------------------------------
 * ------- VOQ default rate class  -------------------------
 * ---------------------------------------------------------
 */
#define DNX_TUNE_OCB_BUFFER_SIZE 256 /** buffer size in bytes */
#define DNX_TUNE_MIN_PACKET_SIZE 64 /** in bytes */

typedef struct
{

    uint32 fadt_min;
    uint32 fadt_max;
    int fadt_alpha;

} dnx_tune_ingress_congestion_fadt_params_t;

typedef struct
{

    uint32 fadt_min;
    uint32 fadt_max;
    int fadt_alpha;
    uint32 resource_range_min;
    uint32 resource_range_max;

} dnx_tune_ingress_congestion_fadt_res_range_params_t;

/*
 * brief - get the dram bound threshold depending on the rate.
 * Recovery / recovery fail thresholds are calculated based on the dram bound threshold
 */
static shr_error_e
dnx_tune_ingress_congestion_dram_bound_threshold_get(
    int unit,
    int rate,
    int *dram_bound_th)
{
    SHR_FUNC_INIT_VARS(unit);

    *dram_bound_th = 0;

    if (rate <= 50)
    {
        *dram_bound_th = 120 * 1024;
    }
    else if (rate <= 100)
    {
        *dram_bound_th = 400 * 1024;
    }
    else if (rate <= 200)
    {
        *dram_bound_th = 500 * 1024;
    }
    else if (rate <= 300)
    {
        *dram_bound_th = 500 * 1024;
    }
    else if (rate <= 400)
    {
        *dram_bound_th = 600 * 1024;
    }
    else
    /** rate > 400G */
    {
        *dram_bound_th = 600 * 1024;
    }

    SHR_FUNC_EXIT;
}

/*
 * brief - get the dram bound recovery threshold depending on dram bound threshold.
 */
static shr_error_e
dnx_tune_ingress_congestion_dram_bound_recovery_threshold_get(
    int unit,
    int rate,
    int dram_bound_th,
    int *dram_bound_recovery_th)
{
    SHR_FUNC_INIT_VARS(unit);

    *dram_bound_recovery_th = 0;

    if (rate <= 50)
    {
        /*
         * SLOW
         */
        *dram_bound_recovery_th = (13 * dram_bound_th) / 45;
    }
    else if (rate <= 200)
    {
        /*
         * MEDIUM
         */
        *dram_bound_recovery_th = (13 * dram_bound_th) / 25;
        *dram_bound_recovery_th = (10 * (*dram_bound_recovery_th)) / 35;
    }
    else if (rate <= 400)
    {
        /*
         * FAST
         */
        *dram_bound_recovery_th = (13 * dram_bound_th) / 40;
    }
    else
    /** rate > 400G */
    {
        /*
         * EXTREAME
         */
        *dram_bound_recovery_th = (13 * dram_bound_th) / 40;
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief -  Tune global reject drop thresholds
 */
shr_error_e
dnx_tune_ingress_congestion_default_voq_rate_class_set(
    int unit,
    int rate_class_id,
    bcm_cosq_rate_class_create_info_t * create_info)
{
    bcm_gport_t gport;
    int dp, is_ocb_only, is_slow_enabled, is_multicast;
    int off_to_normal_th, dram_bound_bytes_th, dram_recovery_fail_th;
    int cosq = 0; /** cosq is not used with profile gport */
    bcm_cosq_gport_discard_t ecn_params;
    bcm_cosq_gport_discard_t wred_params;
    bcm_cosq_gport_size_t bytes_fadt_drop_params, ocb_buffers_fadt_drop_params, ocb_packet_descriptors_fadt_drop_params;
    bcm_cosq_fadt_threshold_t dram_bound_threshold, congestion_notification_threshold;
    bcm_cosq_fadt_info_t thresh_info;
    bcm_cosq_threshold_t recovery_threshold;
    dnx_tune_ingress_congestion_fadt_params_t *total_bytes_fadt_drop_ptr = NULL;
    uint32 dram_bound_flags = 0, cong_notification_flags = 0;
    bcm_cosq_delay_tolerance_preset_attr_t preset_attr = { 0 };
    bcm_cosq_delay_tolerance_t delay_tolerance;

    bcm_cosq_gport_size_t system_red_size_params;
    bcm_cosq_gport_discard_t system_red_discard_params;
    int system_red_index;

    /*
     * Thresholds default values -- start
     {
     */
    uint32 guaranteed = 0;

    uint32 ecn_max_size = 1024 * 1024 / 2; /** 0.5 MegaBytes -- in bytes */

    int wred_gain = 2;

    /*
     * FADT tail drop thresholds
     */
    dnx_tune_ingress_congestion_fadt_params_t total_bytes_fadt_drop = {
        0, /** 0 Bytes -- min FADT param */
        1024 * 1024 * 256, /** 256M Bytes -- max FADT param */
        0 /** alpha FADT param */
    };

    dnx_tune_ingress_congestion_fadt_params_t ocb_buffers_fadt_drop = {
        0, /** 0 OCB buffers -- min FADT param */
        32 * 1024 - 1, /** 32K - 1 ocb buffers -- max FADT param */
        0 /** alpha FADT param */
    };

    dnx_tune_ingress_congestion_fadt_params_t ocb_packet_descriptors_fadt_drop = {
        0, /** 0 packet descriptors -- min FADT param*/
        64 * 1024 - 1, /** 64K - 1 packet descriptors -- max FADT param*/
        0 /** alpha FADT param */
    };

    /*
     * FADT tail drop thresholds - for OCB only rate classes (only total bytes is changed)
     */
    dnx_tune_ingress_congestion_fadt_params_t total_bytes_fadt_drop_ocb_only = {
        0, /** 0 Bytes -- min FADT param */
        1024 * 1024 * 8, /** 8M Bytes -- max FADT param */
        0 /** alpha FADT param */
    };

    /*
     * FADT tail drop thresholds for a system without dram - for OCB only rate classes (only total bytes is changed)
     */
    dnx_tune_ingress_congestion_fadt_params_t total_bytes_fadt_drop_ocb_only_no_dram = {
        1024 * 1024 * 8, /** same as max */
        1024 * 1024 * 8, /** 8M Bytes -- max FADT param */
        0 /** alpha FADT param */
    };

    /*
     * DRAM Bound Parameters are calculated externally according to formulas described in src/examples/dpp/cint_dram_bound_thresh.c
     * Using values for 400G port as the default.
     */
    dnx_tune_ingress_congestion_fadt_res_range_params_t ocb_buffers_dram_bound = {
        0,      /* buffers -- thresh_min */
        dnx_data_ingr_congestion.dram_bound.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_SRAM_BUFFERS)->max / 2,  /* buffers 
                                                                                                                         * -- 
                                                                                                                         * thresh_max 
                                                                                                                         */
        0,      /* alpha */
        0,      /* buffers -- resource_range_min */
        dnx_data_ingr_congestion.dram_bound.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_SRAM_BUFFERS)->max       /* buffers 
                                                                                                                         * -- 
                                                                                                                         * resource_range_max 
                                                                                                                         */
    };
    dnx_tune_ingress_congestion_fadt_res_range_params_t ocb_pds_dram_bound = {
        0,      /* packet descriptors -- thresh_min */
        dnx_data_ingr_congestion.dram_bound.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_SRAM_PDS)->max / 2,      /* packet 
                                                                                                                         * descriptors 
                                                                                                                         * -- 
                                                                                                                         * thresh_max 
                                                                                                                         */
        0,      /* alpha */
        0,      /* packet descriptors -- resource_range_min */
        dnx_data_ingr_congestion.dram_bound.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_SRAM_PDS)->max   /* packet 
                                                                                                                 * descriptors 
                                                                                                                 * --
                                                                                                                 * resource_range_max 
                                                                                                                 */
    };
    dnx_tune_ingress_congestion_fadt_res_range_params_t ocb_buffers_dram_bound_recovery_failure = {
        0,      /* buffers -- thresh_min */
        dnx_data_ingr_congestion.dram_bound.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_SRAM_BUFFERS)->max / 2,  /* buffers 
                                                                                                                         * -- 
                                                                                                                         * thresh_max 
                                                                                                                         */
        0,      /* alpha */
        0,      /* buffers -- resource_range_min */
        dnx_data_ingr_congestion.dram_bound.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_SRAM_BUFFERS)->max       /* buffers 
                                                                                                                         * -- 
                                                                                                                         * resource_range_max 
                                                                                                                         */
    };
    dnx_tune_ingress_congestion_fadt_res_range_params_t ocb_pds_dram_bound_recovery_failure = {
        0,      /* packet descriptors -- thresh_min */
        dnx_data_ingr_congestion.dram_bound.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_SRAM_PDS)->max / 2,      /* packet 
                                                                                                                         * descriptors 
                                                                                                                         * -- 
                                                                                                                         * thresh_max 
                                                                                                                         */
        0,      /* alpha */
        0,      /* packet descriptors -- resource_range_min */
        dnx_data_ingr_congestion.dram_bound.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_SRAM_PDS)->max   /* packet 
                                                                                                                 * descriptors 
                                                                                                                 * --
                                                                                                                 * resource_range_max 
                                                                                                                 */
    };

    /*
     * Congestion Notification - disable (set to max value)
     */
    dnx_tune_ingress_congestion_fadt_params_t total_bytes_congestion_notification = {
        dnx_data_ingr_congestion.info.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES)->max,     /* min
                                                                                                                 * th */
        dnx_data_ingr_congestion.info.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES)->max,     /* max
                                                                                                                 * th */
        0,      /* alpha */
    };

    dnx_tune_ingress_congestion_fadt_params_t ocb_buffers_congestion_notification = {
        dnx_data_ingr_congestion.info.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_SRAM_BUFFERS)->max,    /* min
                                                                                                                 * th */
        dnx_data_ingr_congestion.info.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_SRAM_BUFFERS)->max,    /* max
                                                                                                                 * th */
        0,      /* alpha */
    };

    dnx_tune_ingress_congestion_fadt_params_t ocb_packet_descriptors_congestion_notification = {
        dnx_data_ingr_congestion.dram_bound.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_SRAM_PDS)->max,  /* min
                                                                                                                 * th */
        dnx_data_ingr_congestion.dram_bound.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_SRAM_PDS)->max,  /* max
                                                                                                                 * th */
        0,      /* alpha */
    };

    /*
     * Thresholds default values -- end
     }
     */

    SHR_FUNC_INIT_VARS(unit);

    BCM_GPORT_PROFILE_SET(gport, rate_class_id);

    /*
     * configure VOQ rate class thresholds 
     */

    /*
     * Note that a user can call this function to return to the default
     * rate class which is configured to non default values.
     * This code must clean all settings and does not assume initial zeros
     */

    /*
     * ------------------------
     * ECN 
     * -------------------------
     * ECN WRED is not enabled. 
     * Setup ECN Drop only.
     */
    bcm_cosq_gport_discard_t_init(&ecn_params);
    ecn_params.flags = BCM_COSQ_DISCARD_MARK_CONGESTION | BCM_COSQ_DISCARD_BYTES;
    ecn_params.ecn_thresh = ecn_max_size;

    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_discard_set(unit, gport, cosq, &ecn_params));

    /*
     * ------------------------
     * WRED
     * -------------------------
     * WRED is not enabled.
     * The params are only in bytes, so no need to set any flags
     * Setup gain only.
     */
    bcm_cosq_gport_discard_t_init(&wred_params);
    wred_params.flags = BCM_COSQ_DISCARD_COLOR_ALL;
    wred_params.gain = wred_gain;

    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_discard_set(unit, gport, cosq, &wred_params));

    /*
     * ------------------------
     * OCB only
     * ------------------------
     */
    is_ocb_only = ((dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap == 0) ||
                   (create_info->attributes & BCM_COSQ_RATE_CLASS_CREATE_ATTR_OCB_ONLY));

    SHR_IF_ERR_EXIT(bcm_dnx_cosq_control_set(unit, gport, cosq, bcmCosqControlOCBOnly, is_ocb_only));

    /*
     * ------------------------
     * FADT drop 
     * ------------------------
     */

    sal_memset(&bytes_fadt_drop_params, 0, sizeof(bytes_fadt_drop_params));
    if (is_ocb_only)
    {
        if (dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap == 0)
        {
            total_bytes_fadt_drop_ptr = &total_bytes_fadt_drop_ocb_only_no_dram;
        }
        else
        {
            total_bytes_fadt_drop_ptr = &total_bytes_fadt_drop_ocb_only;
        }
    }
    else
    {
        total_bytes_fadt_drop_ptr = &total_bytes_fadt_drop;
    }
    bytes_fadt_drop_params.size_min = guaranteed;
    bytes_fadt_drop_params.size_fadt_min = total_bytes_fadt_drop_ptr->fadt_min;
    bytes_fadt_drop_params.size_max = total_bytes_fadt_drop_ptr->fadt_max;
    bytes_fadt_drop_params.size_alpha_max = total_bytes_fadt_drop_ptr->fadt_alpha;

    sal_memset(&ocb_buffers_fadt_drop_params, 0, sizeof(ocb_buffers_fadt_drop_params));
    ocb_buffers_fadt_drop_params.size_min = guaranteed;
    ocb_buffers_fadt_drop_params.size_fadt_min = ocb_buffers_fadt_drop.fadt_min;
    ocb_buffers_fadt_drop_params.size_max = ocb_buffers_fadt_drop.fadt_max;
    ocb_buffers_fadt_drop_params.size_alpha_max = ocb_buffers_fadt_drop.fadt_alpha;

    sal_memset(&ocb_packet_descriptors_fadt_drop_params, 0, sizeof(ocb_packet_descriptors_fadt_drop_params));
    ocb_packet_descriptors_fadt_drop_params.size_min = guaranteed;
    ocb_packet_descriptors_fadt_drop_params.size_fadt_min = ocb_packet_descriptors_fadt_drop.fadt_min;
    ocb_packet_descriptors_fadt_drop_params.size_max = ocb_packet_descriptors_fadt_drop.fadt_max;
    ocb_packet_descriptors_fadt_drop_params.size_alpha_max = ocb_packet_descriptors_fadt_drop.fadt_alpha;

    for (dp = 0; dp < DNX_TUNE_NOF_DP; dp++)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_color_size_set(unit, gport, cosq,
                                                          dp, BCM_COSQ_GPORT_SIZE_BYTES, &bytes_fadt_drop_params));
        SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_color_size_set(unit, gport, cosq,
                                                          dp, BCM_COSQ_GPORT_SIZE_BUFFERS | BCM_COSQ_GPORT_SIZE_OCB,
                                                          &ocb_buffers_fadt_drop_params));
        SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_color_size_set(unit, gport, cosq,
                                                          dp, BCM_COSQ_GPORT_SIZE_PACKET_DESC | BCM_COSQ_GPORT_SIZE_OCB,
                                                          &ocb_packet_descriptors_fadt_drop_params));
    }

    /*
     * ------------------------
     * DRAM Bounds
     * ------------------------
     */
    thresh_info.gport = gport;
    thresh_info.cosq = cosq;

    is_slow_enabled = create_info->attributes & BCM_COSQ_RATE_CLASS_CREATE_ATTR_SLOW_ENABLED;
    is_multicast = create_info->attributes & BCM_COSQ_RATE_CLASS_CREATE_ATTR_MULTICAST;
    /*
     * DRAM bound threshold depend on credit request profile off-to-normal threshold.
     * get the preset of off-to-normal in order to calculate DRAM bound thresholds
     */
    preset_attr.rate = create_info->rate;
    preset_attr.credit_size = dnx_data_iqs.credit.worth_get(unit);
    preset_attr.flags |= (is_ocb_only) ? BCM_COSQ_DELAY_TOLERANCE_PRESET_ATTR_F_OCB_ONLY : 0;
    preset_attr.flags |= (is_slow_enabled) ? BCM_COSQ_DELAY_TOLERANCE_PRESET_ATTR_F_SLOW_ENABLED : 0;
    preset_attr.flags |= (is_multicast) ? BCM_COSQ_DELAY_TOLERANCE_PRESET_ATTR_F_MULTICAST : 0;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_delay_tolerance_preset_get(unit, &preset_attr, &delay_tolerance));

    off_to_normal_th = delay_tolerance.credit_request_hungry_off_to_normal_thresh;
    if (!is_slow_enabled)
    {
        /*
         * when slow is disabled, off_to_normal is < 0, but we need a positive threshold
         */
        off_to_normal_th = -off_to_normal_th;
    }
    /*
     * DRAM bound FADT Ocb Buffers
     */
    thresh_info.thresh_type = bcmCosqFadtDramBound;
    thresh_info.resource_type = bcmResourceOcbBuffers;

    sal_memset(&dram_bound_threshold, 0, sizeof(dram_bound_threshold));
    dram_bound_threshold.thresh_min = ocb_buffers_dram_bound.fadt_min;
    dram_bound_threshold.thresh_max = ocb_buffers_dram_bound.fadt_max;
    dram_bound_threshold.alpha = ocb_buffers_dram_bound.fadt_alpha;
    dram_bound_threshold.resource_range_min = ocb_buffers_dram_bound.resource_range_min;
    dram_bound_threshold.resource_range_max = ocb_buffers_dram_bound.resource_range_max;

    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_fadt_threshold_set(unit, dram_bound_flags, &thresh_info, &dram_bound_threshold));

    /*
     * DRAM bound FADT Ocb Packet Descriptors 
     */
    thresh_info.thresh_type = bcmCosqFadtDramBound;
    thresh_info.resource_type = bcmResourceOcbPacketDescriptors;

    sal_memset(&dram_bound_threshold, 0, sizeof(dram_bound_threshold));
    dram_bound_threshold.thresh_min = ocb_pds_dram_bound.fadt_min;
    dram_bound_threshold.thresh_max = ocb_pds_dram_bound.fadt_max;
    dram_bound_threshold.alpha = ocb_pds_dram_bound.fadt_alpha;
    dram_bound_threshold.resource_range_min = ocb_pds_dram_bound.resource_range_min;
    dram_bound_threshold.resource_range_max = ocb_pds_dram_bound.resource_range_max;

    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_fadt_threshold_set(unit, dram_bound_flags, &thresh_info, &dram_bound_threshold));

    /*
     * DRAM bound FADT Ocb bytes
     */
    thresh_info.thresh_type = bcmCosqFadtDramBound;
    thresh_info.resource_type = bcmResourceOcbBytes;

    SHR_IF_ERR_EXIT(dnx_tune_ingress_congestion_dram_bound_threshold_get
                    (unit, create_info->rate, &dram_bound_bytes_th));

    sal_memset(&dram_bound_threshold, 0, sizeof(dram_bound_threshold));
    dram_bound_threshold.thresh_min = dram_bound_bytes_th;
    dram_bound_threshold.thresh_max = dram_bound_bytes_th;
    dram_bound_threshold.alpha = 0;
    dram_bound_threshold.resource_range_min = 0;
    dram_bound_threshold.resource_range_max = 0; /** OCB bytes threshold does not support range */

    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_fadt_threshold_set(unit, dram_bound_flags, &thresh_info, &dram_bound_threshold));

    /*
     * DRAM bound recovery fail FADT Ocb Buffers
     */
    thresh_info.thresh_type = bcmCosqFadtDramBoundRecoveryFailure;
    thresh_info.resource_type = bcmResourceOcbBuffers;

    sal_memset(&dram_bound_threshold, 0, sizeof(dram_bound_threshold));
    dram_bound_threshold.thresh_min = ocb_buffers_dram_bound_recovery_failure.fadt_min;
    dram_bound_threshold.thresh_max = ocb_buffers_dram_bound_recovery_failure.fadt_max;
    dram_bound_threshold.alpha = ocb_buffers_dram_bound_recovery_failure.fadt_alpha;
    dram_bound_threshold.resource_range_min = ocb_buffers_dram_bound_recovery_failure.resource_range_min;
    dram_bound_threshold.resource_range_max = ocb_buffers_dram_bound_recovery_failure.resource_range_max;

    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_fadt_threshold_set(unit, dram_bound_flags, &thresh_info, &dram_bound_threshold));

    /*
     * DRAM bound recovery fail FADT Ocb Packet Descriptors
     */
    thresh_info.thresh_type = bcmCosqFadtDramBoundRecoveryFailure;
    thresh_info.resource_type = bcmResourceOcbPacketDescriptors;

    sal_memset(&dram_bound_threshold, 0, sizeof(dram_bound_threshold));
    dram_bound_threshold.thresh_min = ocb_pds_dram_bound_recovery_failure.fadt_min;
    dram_bound_threshold.thresh_max = ocb_pds_dram_bound_recovery_failure.fadt_max;
    dram_bound_threshold.alpha = ocb_pds_dram_bound_recovery_failure.fadt_alpha;
    dram_bound_threshold.resource_range_min = ocb_pds_dram_bound_recovery_failure.resource_range_min;
    dram_bound_threshold.resource_range_max = ocb_pds_dram_bound_recovery_failure.resource_range_max;

    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_fadt_threshold_set(unit, dram_bound_flags, &thresh_info, &dram_bound_threshold));

    /*
     * DRAM bound recovery fail FADT Ocb bytes
     */
    thresh_info.thresh_type = bcmCosqFadtDramBoundRecoveryFailure;
    thresh_info.resource_type = bcmResourceOcbBytes;

    /** DRAM Recovery fail thr = DRAM bound thr */
    dram_recovery_fail_th = dram_bound_bytes_th;
    sal_memset(&dram_bound_threshold, 0, sizeof(dram_bound_threshold));
    dram_bound_threshold.thresh_min = dram_recovery_fail_th;
    dram_bound_threshold.thresh_max = dram_recovery_fail_th;
    dram_bound_threshold.alpha = 0;
    dram_bound_threshold.resource_range_min = 0;
    dram_bound_threshold.resource_range_max = 0; /** OCB bytes threshold does not support range */

    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_fadt_threshold_set(unit, dram_bound_flags, &thresh_info, &dram_bound_threshold));

    /*
     * DRAM bound recovery 
     */
    recovery_threshold.type = bcmCosqThresholdBytes;
    recovery_threshold.flags = BCM_COSQ_THRESHOLD_INGRESS | BCM_COSQ_THRESHOLD_QSIZE_RECOVERY;
    SHR_IF_ERR_EXIT(dnx_tune_ingress_congestion_dram_bound_recovery_threshold_get
                    (unit, create_info->rate, dram_bound_bytes_th, &recovery_threshold.value));
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &recovery_threshold));

    /*
     * ------------------------
     * Congestion Notification
     * ------------------------
     */
    /*
     * Congestion Notification FADT total bytes
     */
    thresh_info.thresh_type = bcmCosqFadtCongestionNotification;
    thresh_info.resource_type = bcmResourceBytes;

    sal_memset(&congestion_notification_threshold, 0, sizeof(congestion_notification_threshold));
    congestion_notification_threshold.thresh_min = total_bytes_congestion_notification.fadt_min;
    congestion_notification_threshold.thresh_max = total_bytes_congestion_notification.fadt_max;
    congestion_notification_threshold.alpha = total_bytes_congestion_notification.fadt_alpha;
    congestion_notification_threshold.resource_range_min = 0;
    congestion_notification_threshold.resource_range_max = 0;

    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_fadt_threshold_set
                    (unit, cong_notification_flags, &thresh_info, &congestion_notification_threshold));

    /*
     * Congestion Notification FADT SRAM Buffers
     */
    thresh_info.thresh_type = bcmCosqFadtCongestionNotification;
    thresh_info.resource_type = bcmResourceOcbBuffers;

    sal_memset(&congestion_notification_threshold, 0, sizeof(congestion_notification_threshold));
    congestion_notification_threshold.thresh_min = ocb_buffers_congestion_notification.fadt_min;
    congestion_notification_threshold.thresh_max = ocb_buffers_congestion_notification.fadt_max;
    congestion_notification_threshold.alpha = ocb_buffers_congestion_notification.fadt_alpha;
    congestion_notification_threshold.resource_range_min = 0;
    congestion_notification_threshold.resource_range_max = 0;

    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_fadt_threshold_set
                    (unit, cong_notification_flags, &thresh_info, &congestion_notification_threshold));

    /*
     * Congestion Notification FADT SRAM PDs
     */
    thresh_info.thresh_type = bcmCosqFadtCongestionNotification;
    thresh_info.resource_type = bcmResourceOcbPacketDescriptors;

    sal_memset(&congestion_notification_threshold, 0, sizeof(congestion_notification_threshold));
    congestion_notification_threshold.thresh_min = ocb_packet_descriptors_congestion_notification.fadt_min;
    congestion_notification_threshold.thresh_max = ocb_packet_descriptors_congestion_notification.fadt_max;
    congestion_notification_threshold.alpha = ocb_packet_descriptors_congestion_notification.fadt_alpha;
    congestion_notification_threshold.resource_range_min = 0;
    congestion_notification_threshold.resource_range_max = 0;

    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_fadt_threshold_set
                    (unit, cong_notification_flags, &thresh_info, &congestion_notification_threshold));
    /*
     * ------------------------
     * System RED
     * ------------------------
     */

    if (dnx_data_system_red.config.enable_get(unit))
    {
        /** System RED size */
        sal_memset(&system_red_size_params, 0, sizeof(system_red_size_params));
        for (system_red_index = 0; system_red_index < DNX_TUNE_NOF_SYSTEM_RED_INDEXES; system_red_index++)
        {
            SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_color_size_set(unit, gport, cosq, system_red_index,
                                                              BCM_COSQ_GPORT_SIZE_COLOR_SYSTEM_RED,
                                                              &system_red_size_params));
        }

        /** System RED probability */
        bcm_cosq_gport_discard_t_init(&system_red_discard_params);

        system_red_discard_params.flags =
            BCM_COSQ_DISCARD_SYSTEM | BCM_COSQ_DISCARD_PROBABILITY1 | BCM_COSQ_DISCARD_COLOR_ALL;
        SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_discard_set(unit, gport, cosq, &system_red_discard_params));

        system_red_discard_params.flags =
            BCM_COSQ_DISCARD_SYSTEM | BCM_COSQ_DISCARD_PROBABILITY2 | BCM_COSQ_DISCARD_COLOR_ALL;
        SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_discard_set(unit, gport, cosq, &system_red_discard_params));
    }

exit:
    SHR_FUNC_EXIT;
}

/* 
 * ---------------------------------------------------------
 * -------  VSQ Global Resource Partition  -----------------
 * ---------------------------------------------------------
 */
/**
 * \brief -  Configure global resource partition between pool0 and pool1.
 * Set all available resources to pool0 and nothing to pool1.
 * Set headroom to 0 for both pools -- lossless pool is not choosen.
 */
shr_error_e
dnx_tune_ingress_congestion_global_resource_partition(
    int unit)
{
    bcm_cosq_allocation_entity_t target;
    uint32 flags = 0;
    int pool_id = 0;
    int core = -1;

    bcm_cosq_resource_amounts_t total_bytes_params[2], ocb_buffers_params[2], ocb_pds_params[2];

    SHR_FUNC_INIT_VARS(unit);

    /*
     * -----------------------------------------------------------------------------------------
     * Configure default values 
 */

    /**  Everything goes to pool0 */
    total_bytes_params[0].max_shared_pool = DNX_TUNE_INGRESS_CONGESTION_BYTES_MAX(unit);
    total_bytes_params[0].nominal_headroom = 0;
    total_bytes_params[0].max_headroom = 0;

    ocb_buffers_params[0].max_shared_pool = DNX_TUNE_INGRESS_CONGESTION_SRAM_BUFFERS_MAX(unit);
    ocb_buffers_params[0].nominal_headroom = 0;
    ocb_buffers_params[0].max_headroom = 0;

    ocb_pds_params[0].max_shared_pool = DNX_TUNE_INGRESS_CONGESTION_SRAM_PDS_MAX(unit);
    ocb_pds_params[0].nominal_headroom = 0;
    ocb_pds_params[0].max_headroom = 0;

    /**  Nothing goes to pool1 */
    total_bytes_params[1].max_shared_pool = 0;
    total_bytes_params[1].nominal_headroom = 0;
    total_bytes_params[1].max_headroom = 0;

    ocb_buffers_params[1].max_shared_pool = 0;
    ocb_buffers_params[1].nominal_headroom = 0;
    ocb_buffers_params[1].max_headroom = 0;

    ocb_pds_params[1].max_shared_pool = 0;
    ocb_pds_params[1].nominal_headroom = 0;
    ocb_pds_params[1].max_headroom = 0;

    /*
     * End - configure default values 
     * --------------------------------------------------------------------------------------------
     */

    sal_memset(&target, 0x0, sizeof(target));

    /** Allocate all global resources */
    for (pool_id = 0; pool_id < 2; pool_id++)
    {

        total_bytes_params[pool_id].flags = 0;
        total_bytes_params[pool_id].reserved = 0; /** irrelevant for global allocation */

        ocb_buffers_params[pool_id].flags = 0;
        ocb_buffers_params[pool_id].reserved = 0; /** irrelevant for global allocation */

        ocb_pds_params[pool_id].flags = 0;
        ocb_pds_params[pool_id].reserved = 0; /** irrelevant for global allocation */

        for (core = 0; core < dnx_data_device.general.nof_cores_get(unit); ++core)
        {
            BCM_COSQ_GPORT_VSQ_GL_SET(target.gport, core);
            target.color = 0;   /* irrelevant for global allocation */
            target.pool_id = pool_id;

            SHR_IF_ERR_EXIT(bcm_dnx_cosq_resource_allocation_set(unit, flags, bcmResourceBytes,
                                                                 &target, &total_bytes_params[pool_id]));
            SHR_IF_ERR_EXIT(bcm_dnx_cosq_resource_allocation_set(unit, flags, bcmResourceOcbBuffers,
                                                                 &target, &ocb_buffers_params[pool_id]));
            SHR_IF_ERR_EXIT(bcm_dnx_cosq_resource_allocation_set(unit, flags, bcmResourceOcbPacketDescriptors,
                                                                 &target, &ocb_pds_params[pool_id]));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/* 
 * ---------------------------------------------------------
 * -------   Top   -----------------------------------------
 * ---------------------------------------------------------
 */
/**
 * \brief -  Tune Ingress congestion thresholds
 */
shr_error_e
dnx_tune_ingress_congestion_set(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** partition VSQ global resources between pool0 and pool1 */
    SHR_IF_ERR_EXIT(dnx_tune_ingress_congestion_global_resource_partition(unit));

exit:
    SHR_FUNC_EXIT;
}
