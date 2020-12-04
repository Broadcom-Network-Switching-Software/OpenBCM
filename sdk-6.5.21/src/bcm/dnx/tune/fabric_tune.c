/** \file fabric_tune.c
 * Tuning of Fabric parameters
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
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include "fabric_tune.h"

#include <bcm/fabric.h>
#include <bcm_int/dnx_dispatch.h>

#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_iqs.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tune.h>

#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>

/*
 * }
 * Include files.
 */
/*
 * Defines.
 * {
 */
/**
 * \brief - maximal burst for best effort FMQ shaper in credit units
 */
#define DNX_TUNE_FMC_RATES_MAX_BURST_BE 8
/**
 * \brief - maximal burst for guaranteed FMQ shaper in credit units
 */
#define DNX_TUNE_FMC_RATES_MAX_BURST_G 4
/**
 * \brief - maximal burst for FMQ top shaper in credit units
 */
#define DNX_TUNE_FMC_RATES_MAX_BURST_FMC 4

/*
 * }
 * Defines.
 */

/**
 * \brief  - tune FMC (fabric multicast) shapers
 */
static shr_error_e
dnx_tune_fmc_rates_set(
    int unit)
{
    bcm_gport_t gport;
    uint32 root_fmq_max_rate;
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        root_fmq_max_rate = dnx_data_tune.iqs.fmq_max_rate_get(unit);
        /** dnx_data is in GB */
        root_fmq_max_rate = 1024 * 1024 * root_fmq_max_rate;

        /** Root shaper */
        BCM_COSQ_GPORT_FMQ_ROOT_SET(gport);
        SHR_IF_ERR_EXIT(bcm_dnx_cosq_control_set
                        (unit, gport, 0, bcmCosqControlBandwidthBurstMax, DNX_TUNE_FMC_RATES_MAX_BURST_FMC));
        SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_bandwidth_set(unit, gport, 0, 0, root_fmq_max_rate, 0));

        /** Guaranteed shaper */
        BCM_COSQ_GPORT_FMQ_GUARANTEED_SET(gport);
        SHR_IF_ERR_EXIT(bcm_dnx_cosq_control_set
                        (unit, gport, 0, bcmCosqControlBandwidthBurstMax, DNX_TUNE_FMC_RATES_MAX_BURST_G));
        SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_bandwidth_set(unit, gport, 0, 0, root_fmq_max_rate, 0));

        /** Best effort shaper */
        BCM_COSQ_GPORT_FMQ_BESTEFFORT_AGR_SET(gport);
        SHR_IF_ERR_EXIT(bcm_dnx_cosq_control_set
                        (unit, gport, 0, bcmCosqControlBandwidthBurstMax, DNX_TUNE_FMC_RATES_MAX_BURST_BE));
        SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_bandwidth_set(unit, gport, 0, 0, root_fmq_max_rate, 0));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief  - Set RCI throttling mode
 */
static shr_error_e
dnx_tune_rci_throttling_mode_set(
    int unit)
{
    bcm_fabric_control_rci_throttling_mode_t default_throttling_mode = bcmFabricControlRciThrottlingModeGlobal;
    SHR_FUNC_INIT_VARS(unit);

    /**
     * Set  DRM/RCI mode
     */
    SHR_IF_ERR_EXIT(bcm_fabric_control_set(unit, bcmFabricControlRciThrottlingMode, default_throttling_mode));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Tune Fabric thresholds
 */
shr_error_e
dnx_tune_credit_generator_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tune_drm_tables_set(unit));

    SHR_IF_ERR_EXIT(dnx_tune_fmc_rates_set(unit));

    SHR_IF_ERR_EXIT(dnx_tune_rci_throttling_mode_set(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Tune DRM tables 
 */
shr_error_e
dnx_tune_drm_tables_set(
    int unit)
{
    shr_error_e rc = _SHR_E_NONE;
    bcm_port_t port_i;
    uint32 rci_i, link_rate = 0, link_rate_tmp, links_num_i;
    int speed, core;
    bcm_port_resource_t port_resource;

    bcm_cosq_bandwidth_fabric_adjust_key_t key;
    uint32 bandwidth = 0;

    int fec_factor;
    int cell_header_overhead;
    int control_cell_overhead;
    int fabric_util_factor;
    int drm_bandwidth_factor;

    uint32 nof_fabric_links = dnx_data_fabric.links.nof_links_get(unit);
    uint32 max_rate_in_mbps = 0;
    uint64 u64_calc;

    SHR_FUNC_INIT_VARS(unit);

    /** calculate device max rate in MBPS : core_clock_khz/1000*PPC*busSize(in bits)*nof_cores */
    COMPILER_64_ZERO(u64_calc);
    COMPILER_64_SET(u64_calc, 0, dnx_data_device.general.core_clock_khz_get(unit));
    COMPILER_64_UMUL_32(u64_calc, dnx_data_device.general.packet_per_clock_get(unit));
    COMPILER_64_UMUL_32(u64_calc, dnx_data_device.general.bus_size_in_bits_get(unit));
    COMPILER_64_UMUL_32(u64_calc, dnx_data_device.general.nof_cores_get(unit));
    /** Rate should be in [Mbits/sec] */
    COMPILER_64_UDIV_32(u64_calc, 1000);
    max_rate_in_mbps = COMPILER_64_LO(u64_calc);

    /*
     * factors - in percents
     */
    fec_factor = 90;
    cell_header_overhead = 95;
    control_cell_overhead = 99;
    fabric_util_factor = 95;

    /*
     * DRM  bandwidth factor is defined as the following
     * link_capacity_factor = fec factor * cell_header_overhead * control_cell_overhead * fabric_util_factor
     * eack factor is a value between 0 and 1.
     *
     * Since we use interger calculation
     * we use factors in percents (0 - 100) and 
     * we first multiply all percentages
     * and then divide by 100.
     * Note that we have 4 numbers in numerator and 3 numbers in denominator, 
     * thus the result is percentage in range 0 - 100
 */
    drm_bandwidth_factor =
        (fec_factor * cell_header_overhead * control_cell_overhead * fabric_util_factor) / (100 * 100 * 100);

    bcm_port_resource_t_init(&port_resource);

    /*
     * Currently link_rate is triggered by maximal speed rate 
     */
    link_rate = 0;

    /*
     * calculate maximal link rate 
     */
    if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        int port_min, port_max;
        SHR_IF_ERR_EXIT(dnx_drv_soc_port_min(unit, SFI_PTYPE, &port_min));
        SHR_IF_ERR_EXIT(dnx_drv_soc_port_max(unit, SFI_PTYPE, &port_max));
        for (port_i = port_min; port_i >= 0 && port_i <= port_max; port_i++)
        {
            pbmp_t *pbmp_p;

            SHR_IF_ERR_EXIT(soc_info_pbmp_address_get(unit, PBMP_SFI_ALL_BITMAP, &pbmp_p));
            if (_SHR_PBMP_MEMBER(*pbmp_p, port_i))
            {
                /*
                 * To calculate max link rate, need to get the speed of each link.
                 * But if link speed was initialized with BCM_PORT_RESOURCE_DEFAULT_REQUEST
                 * need to take the maximum speed a link can have, since the link speed
                 * hasn't been configured yet.
                 */
                SHR_IF_ERR_EXIT(bcm_dnx_port_resource_get(unit, port_i, &port_resource));
                if (port_resource.speed == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
                {
                    /*
                     * Take the maximum speed in case speed wasn't set
                     */
                    speed = dnx_data_fabric.links.max_speed_get(unit);
                }
                else
                {
                    speed = port_resource.speed;
                }

                /** speed is in MBPS and use DRM factor */
                link_rate_tmp = (speed * drm_bandwidth_factor) / 100;
                link_rate = (link_rate > link_rate_tmp) ? link_rate : link_rate_tmp;
            }
        }
    }
    else
    {
        link_rate = dnx_data_fabric.links.max_speed_get(unit);
    }

    for (rci_i = 0; rci_i <= BCM_FABRIC_NUM_OF_RCI_LEVELS; ++rci_i)
    {
        for (links_num_i = 0; links_num_i <= nof_fabric_links; links_num_i++)
        {

            key.rci_level = rci_i;
            key.num_links = links_num_i;
            key.core = 0;
            bandwidth = (((links_num_i * link_rate) / (rci_i + 1)));
            /*
             *  0 links means local route
             *  RCI 0 - if the number of active links is small,
             *  and the links are indeed in-use, the RCI will rise above 0.
             *  Otherwise - we don't limit the scheduler rate.
             */
            if ((bandwidth > max_rate_in_mbps) || (key.rci_level == 0) || (key.num_links == 0))
            {
                /*
                 * Fill up the first eight entries in DRM tables. These (first 8) relate to 'local'
                 * route and they get values corresponding to high bit rates.
                 */
                bandwidth = max_rate_in_mbps;
            }

            if (dnx_data_sch.device.feature_get(unit, dnx_data_sch_device_shared_drm))
            {
                /*
                 * in multi-core system, cofigure shared DRM table
                 */
                rc = bcm_dnx_cosq_bandwidth_fabric_adjust_set(unit, BCM_COSQ_BW_FABRIC_ADJUST_CONFIG_SHARED, &key,
                                                              bandwidth);
                if (SHR_FAILURE(rc))
                {
                    LOG_ERROR(BSL_LS_BCM_COSQ,
                              (BSL_META_U(unit,
                                          "unit %d, error in setting bcm_fabric_bandwidth_core_profile_set rci_level %d, num_active_links %d, error 0x%x\n"),
                               unit, key.rci_level, key.num_links, rc));
                    SHR_IF_ERR_EXIT(rc);
                }
            }

            {
                /*
                 * configure the SCH_DEVICE_RATE_MEMORY_DRM table which is specific per core
                 * (unlike SCH_SHARED_DEVICE_RATE_SHARED_DRM, which is shared by the two cores and is loaded
                 * in the clause above and exists in multi-core systems core
                 */
                DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
                {
                    key.core = core;
                    rc = bcm_dnx_cosq_bandwidth_fabric_adjust_set(unit, 0, &key, bandwidth);
                    if (SHR_FAILURE(rc))
                    {
                        LOG_ERROR(BSL_LS_BCM_COSQ,
                                  (BSL_META_U(unit,
                                              "unit %d, error in setting bcm_fabric_bandwidth_core_profile_set rci_level %d, num_active_links %d, error 0x%x\n"),
                                   unit, key.rci_level, key.num_links, rc));
                        SHR_IF_ERR_EXIT(rc);
                    }
                }
            }

        }

    }

exit:
    SHR_FUNC_EXIT;
}
