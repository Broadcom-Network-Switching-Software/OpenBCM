/** \file scheduler_tune.c
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
#include <shared/utilex/utilex_bitstream.h>
#include <bcm/cosq.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>

/*
 * }
 * Include files.
 */

/**
 * \brief -  Set default Slow rates
 */
shr_error_e
dnx_tune_scheduler_slow_rate_default_set(
    int unit)
{
    int default_slow_rates[2][8] = {
        {
       /**default profile for port speed <= 100G */
         3, /** level 0 -- rate in GBits per second */
         6,  /** level 1 -- rate in GBits per second */
         12, /** level 2 -- rate in GBits per second */
         24, /** level 3 -- rate in GBits per second */
         36, /** level 4 -- rate in GBits per second */
         48, /** level 5 -- rate in GBits per second */
         64, /** level 6 -- rate in GBits per second */
         96  /** level 7 -- rate in GBits per second */
         },
        {
       /**default profile for port speed > 100G */
         6,  /** level 0 -- rate in GBits per second */
         20, /** level 1 -- rate in GBits per second */
         45, /** level 2 -- rate in GBits per second */
         80, /** level 3 -- rate in GBits per second */
         128,/** level 4 -- rate in GBits per second */
         192,/** level 5 -- rate in GBits per second */
         272,/** level 6 -- rate in GBits per second */
         360 /** level 7 -- rate in GBits per second */
         }
    };

    int level, slow_type;
    int rate_kbps;
    bcm_cosq_slow_level_t slow_level;
    bcm_cosq_slow_profile_attributes_t attr;

    SHR_FUNC_INIT_VARS(unit);

    /** set slow rates */
    for (level = 0; level < 8; level++)
    {
        for (slow_type = 1; slow_type <= 2; slow_type++)
        {
            rate_kbps = default_slow_rates[slow_type - 1][level] * 1000000;
                                                                          /** Gbits to Kbits */

            slow_level.core = BCM_CORE_ALL;
            slow_level.profile = slow_type;
            slow_level.level = level;

            attr.max_rate = rate_kbps;

            SHR_IF_ERR_EXIT(bcm_dnx_cosq_slow_profile_set(unit, &slow_level, &attr));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */

shr_error_e
dnx_tune_scheduler_port_rates_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int if_speed,
    int commit_changes,
    int is_remove)
{
    bcm_port_t master_port;
    bcm_cos_queue_t cosq;
    int num_priorities;
    bcm_gport_t local_port_gport;
    bcm_gport_t e2e_port_gport, e2e_itf_gport, e2e_tc_gport, e2e_tcg_gport;
    int mode, weight, tcg_index;
    int sch_rate;
    int nof_channels;
    int priority_propagation_enable;

    bcm_cosq_gport_info_t gport_info;
    uint32 tcg_configured_bitmap[1];
    uint32 supported_flags;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify that the required flags are supported */
    supported_flags = DNX_ALGO_PORT_SPEED_F_MBPS | DNX_ALGO_PORT_SPEED_F_KBPS;
    SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                             "set scheduler rate flags does not support the requested flags 0x%x %s%s.\n", flags, EMPTY,
                             EMPTY);

    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, port, &num_priorities));
    SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, 0, &master_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_channels_nof_get(unit, port, &nof_channels));

    if (flags & DNX_ALGO_PORT_SPEED_F_MBPS)
    {
        if_speed = if_speed * 1000;
    }
    /** set speed to be 5% speedup over interface speed */
    if_speed = if_speed + (if_speed / 100) * 5;

    sch_rate = if_speed / nof_channels;

    /*
     * set port rate
     */

    BCM_GPORT_LOCAL_SET(local_port_gport, port);

    gport_info.in_gport = local_port_gport;
    gport_info.cosq = 0;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_handle_get(unit, bcmCosqGportTypeE2EPort, &gport_info));
    e2e_port_gport = gport_info.out_gport;

    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_bandwidth_set
                    (unit, e2e_port_gport, 0, 0, sch_rate, commit_changes ? 0 : BCM_COSQ_BW_NOT_COMMIT));

    /*
     * set interface rate:
     * All ports reside on channelized calendar, so interface shaper is required to be set for any
     * port even if it's not really channelized.
     * For channalized ports, configure only when:
     *     1. Adding / changing port (!is_remove) and this is master port (port == master_port)
     *     or
     *     2. Removing port (is_remove) and this is the last port (nof_channels == 1)
     */
    if (((port == master_port) && (!is_remove)) || ((nof_channels == 1) && (is_remove)))
    {
        SHR_IF_ERR_EXIT(bcm_dnx_fabric_port_get(unit, e2e_port_gport, 0, &e2e_itf_gport));
        /*
         * Use if_speed (and not sch_speed) for interface
         */
        SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_bandwidth_set(unit, e2e_itf_gport, 0, 0, if_speed, 0));
    }

    /*
     * set TC rate
     */
    for (cosq = 0; cosq < num_priorities; cosq++)
    {
        gport_info.in_gport = local_port_gport;
        SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_handle_get(unit, bcmCosqGportTypeE2EPortTC, &gport_info));
        e2e_tc_gport = gport_info.out_gport;
        SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_bandwidth_set(unit, e2e_tc_gport, cosq, 0, sch_rate, 0));
    }

    /*
     * set tcg rate
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priority_propagation_enable_get(unit, port, &priority_propagation_enable));
    if (!priority_propagation_enable && (num_priorities >= dnx_data_sch.ps.min_priority_for_tcg_get(unit)))
    {
        /** TC gport */
        gport_info.in_gport = local_port_gport;
        SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_handle_get(unit, bcmCosqGportTypeE2EPortTC, &gport_info));
        e2e_tc_gport = gport_info.out_gport;

        /** TCG gport */
        gport_info.in_gport = local_port_gport;
        SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_handle_get(unit, bcmCosqGportTypeE2EPortTCG, &gport_info));
        e2e_tcg_gport = gport_info.out_gport;

        /** all tcg(s) are not configured */
        SHR_BITCLR_RANGE(tcg_configured_bitmap, 0, 31);

        for (cosq = 0; cosq < num_priorities; cosq++)
        {
            /** sched_set is called earlier */
            SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_sched_get(unit, e2e_tc_gport, cosq, &mode, &weight));
            tcg_index = mode - BCM_COSQ_SP0;

            if (SHR_BITGET(tcg_configured_bitmap, tcg_index) == 0)
            /** this tcg is not configured yet */
            {
                SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_bandwidth_set(unit, e2e_tcg_gport, tcg_index, 0, sch_rate, 0));
                /** mask this tcg as already configured */
                SHR_BITSET(tcg_configured_bitmap, tcg_index);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -  configures initial rates for ports, interfaces, TCs and TCGs 
 *
 * \param [in] unit -  Unit-ID
 *  *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_tune_scheduler_initial_rates_set(
    int unit)
{
    int if_speed, if_speed_khz;
    bcm_port_t port, last_port;
    bcm_pbmp_t port_bm;
    int is_remove = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_TM_E2E_SCH, 0, &port_bm));

    _SHR_PBMP_LAST(port_bm, last_port);

    BCM_PBMP_ITER(port_bm, port)
    {

        SHR_IF_ERR_EXIT(dnx_algo_port_interface_rate_get(unit, port, DNX_ALGO_PORT_SPEED_F_TX, &if_speed));
        if_speed_khz = if_speed * 1000;
        SHR_IF_ERR_EXIT(dnx_tune_scheduler_port_rates_set(unit, port, 0, if_speed_khz, port == last_port, is_remove));
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
 * \brief -  Tune Scheduler Configuration
 */
shr_error_e
dnx_tune_scheduler_tune(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** set initial rates on scheduling hierarchy */
    SHR_IF_ERR_EXIT(dnx_tune_scheduler_initial_rates_set(unit));

    /** set slow rates */
    SHR_IF_ERR_EXIT(dnx_tune_scheduler_slow_rate_default_set(unit));

exit:
    SHR_FUNC_EXIT;
}
