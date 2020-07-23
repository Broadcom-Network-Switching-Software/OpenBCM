/** \file egr_queuing_tune.c
 * Tuning of egress queuing parameters
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
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <bcm/cosq.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/cosq/egress/rate_measurement.h>

/*
 * }
 * Include files.
 */

/** 24 = 12 (ipg) + 8 (preamble) + 4 (crc)*/
#define DNX_TUNE_EGR_QUEUING_PORT_RATE_COMPENSATION    24

/*
 * See .h file
 */

shr_error_e
dnx_tune_egr_ofp_rate_set(
    int unit,
    bcm_port_t port,
    int if_speed,
    int commit_changes,
    int is_remove)
{
    bcm_port_t master_port;
    bcm_cos_queue_t cosq;
    int num_priorities;
    bcm_gport_t gport, channelized;
    int nof_channels;
    int rate_measurement;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &num_priorities));
    SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, 0, &master_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_is_channelized_get(unit, port, &channelized));
    SHR_IF_ERR_EXIT(dnx_algo_port_channels_nof_get(unit, port, &nof_channels));

    /** set speed to be 1% speedup over interface speed */
    if_speed = if_speed + (if_speed / 100) * 1;

    rate_measurement = if_speed / nof_channels;

    /*
     * set interface rate:
     * 1. For channelized interfaces only (since if non-channelized - the if shaper is set to max and port shaper is doing the shaping)
     * and
     * 2. One of the below
     *     a. When adding / changing port (!is_remove) and this is master port (port == master_port)
     *     or
     *     b. When removing port (is_remove) and this is the last port (nof_channels == 1)
     */
    if (channelized)
    {
        if (((port == master_port) && (!is_remove)) || ((nof_channels == 1) && (is_remove)))
        {
            BCM_GPORT_LOCAL_SET(gport, port);
            SHR_IF_ERR_EXIT(bcm_fabric_port_get(unit, gport, 0, &gport));
            /*
             * Use if_speed (and not egr_speed) for interface
             */
            SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_bandwidth_set(unit, gport, 0, 0, if_speed, 0));
        }
    }

    /*
     * set port rate
     * Optimization: Only the last port will perform a commit to HW, according to commit_changes argument.
     */
    BCM_GPORT_LOCAL_SET(gport, port);
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_bandwidth_set
                    (unit, gport, 0, 0, rate_measurement, commit_changes ? 0 : BCM_COSQ_BW_NOT_COMMIT));

    /*
     * set queue rate
     */
    for (cosq = 0; cosq < num_priorities; cosq++)
    {
        BCM_COSQ_GPORT_PORT_TC_SET(gport, port);
        SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_bandwidth_set(unit, gport, cosq, 0, rate_measurement, 0));
    }

    /*
     * configure tcg rate for supported number of priorities
     */
    if (num_priorities >= dnx_data_egr_queuing.params.tcg_min_priorities_get(unit))
    {
        BCM_COSQ_GPORT_PORT_TCG_SET(gport, port);
        SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_bandwidth_set(unit, gport, 0, 0, rate_measurement, 0));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_tune_egr_queuing_rate_tune(
    int unit)
{
    int if_speed, if_speed_khz;
    bcm_port_t port, last_port;
    bcm_pbmp_t port_bm;
    int is_remove = 0;
    int rate_measurement_supported, interval;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * perform rate tuning for egress per port.
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING,
                                               0, &port_bm));

    _SHR_PBMP_LAST(port_bm, last_port);

    BCM_PBMP_ITER(port_bm, port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_interface_rate_get(unit, port, DNX_ALGO_PORT_SPEED_F_TX, &if_speed));

        if_speed_khz = if_speed * 1000;

        SHR_IF_ERR_EXIT(dnx_tune_egr_ofp_rate_set(unit, port, if_speed_khz, (port == last_port), is_remove));
    }

    /*
     * set interval and weight for egress rate measure
     */
    rate_measurement_supported =
        dnx_data_egr_queuing.rate_measurement.feature_get(unit, dnx_data_egr_queuing_rate_measurement_is_supported);
    if (rate_measurement_supported)
    {
        /** default interval is 10 usec */
        interval = 10;
        SHR_IF_ERR_EXIT(dnx_rate_measurement_interval_set(unit, interval));

        SHR_IF_ERR_EXIT(dnx_rate_measurement_weight_set(unit, BCM_COSQ_RATE_WEIGHT_FULL));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_tune_egr_queuing_port_add(
    int unit,
    bcm_port_t port)
{
    bcm_cos_queue_t cosq;
    int num_priorities;
    bcm_gport_t gport;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * map queue to tcg and configure tcg rate for supported number of priorities
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &num_priorities));
    if (num_priorities >= dnx_data_egr_queuing.params.tcg_min_priorities_get(unit))
    {
        for (cosq = 0; cosq < num_priorities; cosq++)
        {
            BCM_COSQ_GPORT_PORT_TC_SET(gport, port);
            SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_sched_set(unit, gport, cosq, 0, -1));
        }
    }

    /*
     * configure rate compensation
     */
    BCM_GPORT_LOCAL_SET(gport, port);
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_control_set
                    (unit, gport, 0, bcmCosqControlPacketLengthAdjust, DNX_TUNE_EGR_QUEUING_PORT_RATE_COMPENSATION));

exit:
    SHR_FUNC_EXIT;
}
