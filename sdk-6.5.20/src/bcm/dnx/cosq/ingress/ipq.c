/** \file ipq.c
 * 
 *
 * General Ingress Packet Queuing functionality for DNX. \n
 * Dedicated set of IPQ APIs are distributed between ipq_*.c files: \n
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

/*
 * Include files.
 * {
 */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stack.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_congestion.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_ipq_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_ipq_alloc_mngr_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_ingress_congestion_access.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/cosq/ingress/ipq.h>
#include <bcm_int/dnx/cosq/cosq.h>
#include <bcm_int/dnx/cosq/ingress/iqs_api.h>
#include <bcm_int/dnx/cosq/ingress/iqs.h>
#include <bcm_int/dnx/cosq/ingress/ingress_congestion.h>
#include <bcm_int/dnx/stk/stk_domain.h>
#include <bcm_int/dnx/cosq/ingress/ipq.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/res_mngr/resource_tag_bitmap.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_types.h>
#include <bcm_int/dnx/cosq/scheduler/scheduler.h>
#include <bcm_int/dnx/fabric/fabric.h>
#include <bcm_int/dnx/stk/stk_sys.h>
#include <bcm/stack.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include "ipq_dbal.h"
#include "ingress_congestion.h"
#include "ingress_congestion_dbal.h"
#include "compensation.h"
#include "ipq.h"

/*
 * }
 */

/*
 * Defines:
 * {
 */
/** Number of iterations to check for empty queue */
#define DNX_COSQ_IPQ_QUEUE_EMPTY_ITERATIONS (5)

/*
 * Queue alloc manager macros
 */

/** bitmap search pattern */
#define DNX_COSQ_IPQ_QUEUE_BUNDLE_BITMAP_ALLOC_PATTERN(unit, core) (core == BCM_CORE_ALL ? (1 << dnx_data_device.general.nof_cores_get(unit)) - 1 : (1 << core))
/** number of bitmap element for queue bundle */
#define DNX_COSQ_IPQ_BITMAP_ELEMS_PER_QUEUE_BUNDLE(unit) (dnx_data_device.general.nof_cores_get(unit))

/*
 * }
 */

/*
 * Static Functions
 * {
 */

static shr_error_e dnx_cosq_ipq_sw_state_db_init(
    int unit);
static shr_error_e dnx_cosq_ipq_defaults_init(
    int unit);

static shr_error_e dnx_cosq_ipq_vsq_multicast_category_set(
    int unit,
    int queue_id);

static shr_error_e dnx_cosq_ipq_queue_type_bundle_range_get(
    int unit,
    dnx_ipq_queue_type_e queue_type,
    int *start_bundle,
    int *end_bundle);
/*
 * }
 */

/**
 * \brief
 *   Convert queue bundle index to alloc manager element.
 */
static shr_error_e
dnx_cosq_ipq_queue_bundle_element_get(
    int unit,
    int core,
    int queue_bundle,
    int *element)
{
    SHR_FUNC_INIT_VARS(unit);

    *element = queue_bundle * dnx_data_device.general.nof_cores_get(unit) + (DNXCMN_CORE_ID2INDEX(unit, core));

    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Convert alloc manager element to queue bundle index.
 */
static shr_error_e
dnx_cosq_ipq_element_queue_bundle_get(
    int unit,
    int core,
    int element,
    int *queue_bundle)
{
    SHR_FUNC_INIT_VARS(unit);

    *queue_bundle = element / dnx_data_device.general.nof_cores_get(unit);

    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get number of allocated elements in queue region
 */
static shr_error_e
dnx_cosq_ipq_nof_allocated_elements_in_region_get(
    int unit,
    int region_id,
    int *nof_elements)
{
    int nof_elements_in_region, region_nof_quartets;
    SHR_FUNC_INIT_VARS(unit);

    region_nof_quartets = DNX_IPQ_NOF_QUEUES_IN_REGION(unit) / DNX_IPQ_MIN_BUNDLE_SIZE(unit);
    nof_elements_in_region = region_nof_quartets * DNX_COSQ_IPQ_BITMAP_ELEMS_PER_QUEUE_BUNDLE(unit);

    SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.
                    ipq_alloc_mngr.nof_allocated_elements_in_range_get(unit, region_id * nof_elements_in_region,
                                                                       nof_elements_in_region, nof_elements));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialize sw_state database.
 *
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * Create sw state db for each queue bundle.
 *   * Create sw state db for each region. The region db will be used
 *     for allocating queues in resource manager.
 * \see
 *   * None
 */
static shr_error_e
dnx_cosq_ipq_sw_state_db_init(
    int unit)
{
    uint32 nof_queues = 0;
    uint32 nof_queues_in_region = 0;
    uint32 default_mc_queues;

    SHR_FUNC_INIT_VARS(unit);

    nof_queues = dnx_data_ipq.queues.nof_queues_get(unit);
    nof_queues_in_region = dnx_data_ipq.regions.nof_queues_get(unit);

    /*
     * Init IPQ sw state DB
     */
    SHR_IF_ERR_EXIT(dnx_ipq_db.init(unit));

    /**
     * allocate queue bundles in sw_state db.
     */
    SHR_IF_ERR_EXIT(dnx_ipq_db.base_queues.alloc(unit));

    /**
     * Allocate Symmetric/Asymmetric VOQs bitmap in sw_state db.
     */
    SHR_IF_ERR_EXIT(dnx_ipq_db.base_queue_is_asymm.alloc_bitmap(unit));

    /*
     * Init IPQ Alloc Manager DB
     */
    SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.init(unit));

    /**
     * Allocate regions in sw_state db in size of nof_q/nof_q_in_region.
     * This db will help when allocating queues in resource manager.
     */
    SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.regions.alloc(unit, nof_queues / nof_queues_in_region));

    /**
     * Init FMQ struct in dnx_ipq_alloc_mngr_db with appropriate values.
     * Default is that we are in simple FMQ scheduler mode, and max FMQ is 3.
     */
    default_mc_queues = (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist) ?
                         DNX_IPQ_FMQ_BUNDLE_SIZE(unit) : 0);
    SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.fmq.max_multicast_queue.set(unit, default_mc_queues - 1));
    SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.fmq.is_enhance_scheduler_mode.set(unit, FALSE));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_ipq_default_tc_map_profile_get(
    int unit,
    int is_fmq,
    dnx_ipq_tc_map_t * tc_default_profile)
{
    uint32 tc, dp, nof_dps;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(tc_default_profile, 0x0, sizeof(*tc_default_profile));

    nof_dps = dnx_data_ipq.queues.feature_get(unit, dnx_data_ipq_queues_voq_resolution_per_dp) ? DNX_COSQ_NOF_DP : 1;

    for (tc = 0; tc < DNX_COSQ_NOF_TC; ++tc)
    {
        for (dp = 0; dp < nof_dps; ++dp)
        {
            /*
             * Default TC mapping is 1:1 (for flow based / system port based forwarding)
             *
             * Default for FMQ: Map incoming MC TC to queues 0-3:
             * TC0-1 --> FMQ0
             * TC2-3 --> FMQ1
             * TC4-5 --> FMQ2
             * TC6-7 --> FMQ3
             *
             * all DPs are mapped to the same offset
             */
            tc_default_profile->offset[tc][dp] = is_fmq ? tc / 2 : tc;
        }
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialize IPQ template manager.
 *   the template manager will be used to handle the profiles
 *   for Flow based / system port based forwarding.
 *   yhe FMQ doesn't have profiles, so it is not handeled through template manager
 *
 * \param [in] unit - Relevant unit.
 *
 * \retval shr_error_e -
 *   Error return value
 * \remark
 *   None
 * \see
 *   shr_error_e
 */
static shr_error_e
dnx_cosq_ipq_tc_map_profile_init(
    int unit)
{
    int is_fmq = FALSE;
    dnx_ipq_tc_map_t tc_mapping;
    dnx_algo_template_create_data_t create_data;
    int core_idx;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&create_data, 0, sizeof(create_data));

    SHR_IF_ERR_EXIT(dnx_cosq_ipq_default_tc_map_profile_get(unit, is_fmq, &tc_mapping));
    /*
     * Set common template info
     */
    create_data.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;
    create_data.first_profile = 0;
    create_data.max_references = dnx_data_ipq.queues.nof_queues_get(unit);
    create_data.data_size = sizeof(dnx_ipq_tc_map_t);
    create_data.default_data = &tc_mapping;

    /*
     * Set unique VOQ Flow mapping info
     */
    create_data.nof_profiles = dnx_data_ipq.tc_map.nof_voq_flow_profiles_get(unit);
    create_data.default_profile = dnx_data_ipq.tc_map.default_voq_flow_profile_idx_get(unit);
    sal_strncpy(create_data.name, "Ipq tc map voq flow profile", DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    /*
     * Create the template manager for the TC flow
     */
    SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.tc_voq_flow_map.alloc(unit));

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_idx)
    {
        SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.tc_voq_flow_map.create(unit, core_idx, &create_data, NULL));
    }

    /*
     * Set unique Sysport mapping info
     */
    create_data.nof_profiles = dnx_data_ipq.tc_map.nof_sysport_profiles_get(unit);
    create_data.default_profile = dnx_data_ipq.tc_map.default_sysport_profile_idx_get(unit);
    sal_strncpy(create_data.name, "Ipq tc map sysport profile", DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    /*
     * Create the template manager for the TC flow
     */
    SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.tc_sysport_map.create(unit, &create_data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_cosq_ipq_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * init sw state db
     */
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_sw_state_db_init(unit));

    /*
     * Create queues bitmap
     */
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_alloc_mngr_init(unit));

    /*
     * Create TC Map profile Template Manager
     */
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_tc_map_profile_init(unit));

    /*
     * Init defaults
     */
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_defaults_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * brief - set mapping from system TC to VOQ offset, for all TCs
 */
static shr_error_e
dnx_cosq_ipq_tc_to_voq_offset_map_set(
    int unit,
    int core,
    uint32 profile,
    uint32 is_flow,
    uint32 is_fmq,
    dnx_ipq_tc_map_t * tc_mapping)
{
    uint32 sys_tc, voq_offset;
    uint32 dp, nof_dps;

    SHR_FUNC_INIT_VARS(unit);

    nof_dps = dnx_data_ipq.queues.feature_get(unit, dnx_data_ipq_queues_voq_resolution_per_dp) ? DNX_COSQ_NOF_DP : 1;

    for (sys_tc = 0; sys_tc < DNX_COSQ_NOF_TC; ++sys_tc)
    {
        for (dp = 0; dp < nof_dps; dp++)
        {
            voq_offset = tc_mapping->offset[sys_tc][dp];
            SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_tc_to_voq_offset_map_set(unit, core, sys_tc, dp, profile, is_flow, is_fmq,
                                                                       voq_offset));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialize IPQ registers with default values.
 *
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_cosq_ipq_defaults_init(
    int unit)
{
    int is_flow = 0;
    int profile, sysport_profile = 0, flow_profile = 0;
    int is_fmq;
    dnx_ipq_tc_map_t default_tc_mapping;
    int last_mc_queue;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.fmq.max_multicast_queue.get(unit, &last_mc_queue));

    /** Configure default VOQ catefories */
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_vsq_multicast_category_set(unit, (last_mc_queue == -1 ? 0 : last_mc_queue)));

    /*
     * get default TC mapping
     */
    is_fmq = FALSE;
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_default_tc_map_profile_get(unit, is_fmq, &default_tc_mapping));
    /*
     * Set default TC mapping to HW
     */
    sysport_profile = dnx_data_ipq.tc_map.default_sysport_profile_idx_get(unit);
    flow_profile = dnx_data_ipq.tc_map.default_voq_flow_profile_idx_get(unit);
    for (is_flow = 0; is_flow <= 1; ++is_flow)
    {
        profile = is_flow ? flow_profile : sysport_profile;
        SHR_IF_ERR_EXIT(dnx_cosq_ipq_tc_to_voq_offset_map_set
                        (unit, BCM_CORE_ALL, profile, is_flow, is_fmq, &default_tc_mapping));
    }

    is_fmq = TRUE;
    is_flow = FALSE;
    /*
     * get default TC mapping for FMQ
     */
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_default_tc_map_profile_get(unit, is_fmq, &default_tc_mapping));
    /*
     * Set default FMQ TC mapping to HW.
     */
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_tc_to_voq_offset_map_set
                    (unit, BCM_CORE_ALL, profile, is_flow, is_fmq, &default_tc_mapping));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * get the TC mapping from element index, using template manager
 *
 * \param [in] unit - Unit-ID
 * \param [in] core - device core id.
 * \param [in] dest_idx - element index
 * \param [in] is_flow - indication if dest ndx is flow id or system port
 * \param [in] tc_profile - TC mapping profile to point by the requested element.
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_cosq_ipq_tc_profile_set(
    int unit,
    int core,
    uint32 dest_idx,
    int is_flow,
    uint32 tc_profile)
{
    uint32 flow_quartet, sysport;
    int min_bundle_size = dnx_data_ipq.queues.min_bundle_size_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (is_flow)
    {
        flow_quartet = dest_idx / min_bundle_size;
        SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_voq_flow_quartet_to_flow_profile_map_set
                        (unit, core, flow_quartet, tc_profile));
    }
    else
    {
        sysport = dest_idx;
        SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_system_port_to_voq_base_tc_profile_set(unit, sysport, tc_profile));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * get the TC mapping from element index, using template manager
 *
 * \param [in] unit - Unit-ID
 * \param [in] core - device core id.
 * \param [in] dest_idx - element index
 * \param [in] is_flow - indication if dest ndx is flow id or system port
 * \param [out] tc_profile - TC mapping profile pointed to by the requested element.
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_cosq_ipq_tc_profile_get(
    int unit,
    int core,
    uint32 dest_idx,
    int is_flow,
    uint32 *tc_profile)
{
    uint32 flow_quartet, sysport;
    int min_bundle_size = dnx_data_ipq.queues.min_bundle_size_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (is_flow)
    {
        flow_quartet = dest_idx / min_bundle_size;
        SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_voq_flow_quartet_to_flow_profile_map_get
                        (unit, core, flow_quartet, tc_profile));
    }
    else
    {
        sysport = dest_idx;
        SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_system_port_to_voq_base_tc_profile_get(unit, sysport, tc_profile));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   get tc mapping profile from destination index
 *
 * \param [in] unit - Relevant unit.
 * \param [in] core - device core id.
 * \param [in] dest_ndx - index of the element to get. the index can be a flow id or system port
 * \param [in] is_flow - indication whether dest_ndx is flow id or sysport.
 * \param [out] tc_mapping - TC mapping for the requested element.
 *
 * \retval shr_error_e -
 *   Error return value
 * \remark
 *   None
 * \see
 *   shr_error_e
 */
static shr_error_e
dnx_cosq_ipq_tc_map_profile_data_get(
    int unit,
    int core,
    int dest_ndx,
    int is_flow,
    dnx_ipq_tc_map_t * tc_mapping)
{
    uint32 profile_ndx;
    int ref_count = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ipq_tc_profile_get(unit, core, dest_ndx, is_flow, &profile_ndx));

    if (is_flow)
    {
        SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.
                        tc_voq_flow_map.profile_data_get(unit, core, profile_ndx, &ref_count, tc_mapping));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.
                        tc_sysport_map.profile_data_get(unit, profile_ndx, &ref_count, tc_mapping));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   get tc mapping profile from destination index
 *
 * \param [in] unit - Relevant unit.
 * \param [in] core - device core id.
 * \param [in] dest_ndx - index of the element to exchange. the index can be a flow id or system port
 * \param [in] is_flow - indication whether dest_ndx is flow id or sysport.
 * \param [in] tc_mapping - TC mapping for the element.
 * \param [in] old_map_profile - old profile id. (to reduce reference counter).
 * \param [out] new_map_profile - new profile id.
 * \param [out] is_last - indication if this was the last element pointing to the old profile.
 * \param [out] is_allocated - indication is this the first pointer to the new profile.
 *
 * \retval shr_error_e -
 *   Error return value
 * \remark
 *   None
 * \see
 *   shr_error_e
 */
static shr_error_e
dnx_cosq_ipq_tc_map_profile_exchange(
    int unit,
    int core,
    int dest_ndx,
    int is_flow,
    dnx_ipq_tc_map_t * tc_mapping,
    uint32 *old_map_profile,
    int *new_map_profile,
    int *is_last,
    int *is_allocated)
{
    int flags = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ipq_tc_profile_get(unit, core, dest_ndx, is_flow, old_map_profile));

    if (is_flow)
    {
        SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.
                        tc_voq_flow_map.exchange(unit, core, flags, tc_mapping, *old_map_profile, NULL, new_map_profile,
                                                 (uint8 *) is_allocated, (uint8 *) is_last));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.tc_sysport_map.exchange(unit, flags, tc_mapping, *old_map_profile, NULL,
                                                                      new_map_profile, (uint8 *) is_allocated,
                                                                      (uint8 *) is_last));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e
dnx_cosq_ipq_queue_allocate(
    int unit,
    int core,
    uint32 flags,
    dnx_ipq_queue_type_e queue_type,
    int num_cos,
    int *base_queue)
{
    int nof_queue_bundles_to_alloc = num_cos / DNX_IPQ_MIN_BUNDLE_SIZE(unit);
    int base_bundle = 0, element = 0;
    int is_fmq_enhance_scheduler_mode = 0;
    dnx_ipq_alloc_info_t alloc_info;
    int core_idx;
    int start_bundle = 0, end_bundle = 0;
    int start_element = 0, end_element = 0;
    int region_id, nof_allocated_in_region;
    int nof_elements_allocated = 0;
    int is_with_id = _SHR_IS_FLAG_SET(flags, DNX_ALGO_RES_ALLOCATE_WITH_ID);
    int nof_elements_per_queue_bundles;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&alloc_info, 0, sizeof(dnx_ipq_alloc_info_t));

    /** Interdigitated cause the queues to be doubled. */
    if (queue_type == DNX_ALGO_IPQ_QUEUE_TYPE_UNICAST_INTERDIGITATED)
    {
        nof_queue_bundles_to_alloc *= 2;
    }

    /*
     * get bitmap search range
     * first, get quartet range from queue type.
     * The quartet range is converted into bitmap elements.
     */
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_queue_type_bundle_range_get(unit, queue_type, &start_bundle, &end_bundle));

    if (is_with_id)
    {
        base_bundle = *base_queue / DNX_IPQ_MIN_BUNDLE_SIZE(unit);

        /*
         * Verify the requested ID is in valid range
         * (alloc manager don't take both flags with_id and in_range together)
         */
        if (base_bundle < start_bundle || base_bundle > end_bundle)
        {
            SHR_ERR_EXIT(BCM_E_PARAM, "Requested queue quartet %d is out of bounds. Valid range is: %d - %d",
                         base_bundle, start_bundle, end_bundle);
        }

        /** get the requested bitmap element base(relevant if DNX_ALGO_RES_ALLOCATE_WITH_ID is set) */
        SHR_IF_ERR_EXIT(dnx_cosq_ipq_queue_bundle_element_get(unit, core, base_bundle, &element));
    }
    else
    {
        /** calculate bitmap element range */
        SHR_IF_ERR_EXIT(dnx_cosq_ipq_queue_bundle_element_get(unit, BCM_CORE_ALL, start_bundle, &start_element));
        SHR_IF_ERR_EXIT(dnx_cosq_ipq_queue_bundle_element_get(unit, BCM_CORE_ALL, end_bundle, &end_element));
    }

    /** Try to allocate element */
    alloc_info.core = core;
    /** set queue type to alloc info, override it later if needed */
    alloc_info.queue_type = queue_type;
    if (queue_type == DNX_ALGO_IPQ_QUEUE_TYPE_MULTICAST)
    {
        SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.fmq.is_enhance_scheduler_mode.get(unit, &is_fmq_enhance_scheduler_mode));
        if (is_fmq_enhance_scheduler_mode == FALSE)
        {
            /**
             * scheduler mode is currently simple,
             * in simple mode all MC allocations in ipq_alloc_mngr are done as UC allocation.
             * This is done to avoid wasting a whole region as MC for very few Queues.
             */
            alloc_info.queue_type = DNX_ALGO_IPQ_QUEUE_TYPE_UNICAST;
        }
    }
    alloc_info.nof_queue_bundles_to_alloc = nof_queue_bundles_to_alloc;
    alloc_info.start_element = start_element;
    alloc_info.end_element = end_element;
    SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.ipq_alloc_mngr.allocate_single(unit, flags, (void *) &alloc_info, &element));

    /** element represent bundle of queues, whereas base_queue holds the real queue id */
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_element_queue_bundle_get(unit, core, element, &base_bundle));
    *base_queue = base_bundle * DNX_IPQ_MIN_BUNDLE_SIZE(unit);

    /*
     * Save in SW Sate an indication if the queue was allocated symmetrically or not.
     * If it is allocated with BCM_CORE_ALL, then it is symmetrical allocation
     */
    if (core != BCM_CORE_ALL)
    {
        SHR_IF_ERR_EXIT(dnx_ipq_db.base_queue_is_asymm.bit_set(unit, *base_queue));
    }

    /** After allocation done, save queue's num_cos */
    DNXCMN_CORES_ITER(unit, core, core_idx)
    {
        SHR_IF_ERR_EXIT(dnx_ipq_db.base_queues.num_cos.set(unit, core_idx, base_bundle, num_cos));
    }

    /** Check if we allocated a new region */
    region_id = *base_queue / DNX_IPQ_NOF_QUEUES_IN_REGION(unit);
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_nof_allocated_elements_in_region_get(unit, region_id, &nof_allocated_in_region));

    nof_elements_per_queue_bundles = (core == BCM_CORE_ALL ? dnx_data_device.general.nof_cores_get(unit) : 1);
    nof_elements_allocated = nof_queue_bundles_to_alloc * nof_elements_per_queue_bundles;

    if (nof_elements_allocated == nof_allocated_in_region)
    {
        /*
         * we just allocated in a new region.
         * set the region type in SW state
         */
        if (queue_type == DNX_ALGO_IPQ_QUEUE_TYPE_UNICAST_INTERDIGITATED)
        {
            SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.regions.type.set(unit, region_id, DNX_IPQ_REGION_TYPE_INTERDIG));

            /** if this is interdigitated region, we need to set it in HW as well. */
            SHR_IF_ERR_EXIT(dnx_cosq_ipq_region_config_set(unit, region_id, TRUE));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.regions.type.set(unit, region_id, DNX_IPQ_REGION_TYPE_NORMAL));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file.
 */
shr_error_e
dnx_cosq_ipq_queue_deallocate(
    int unit,
    int core,
    int base_queue)
{
    int base_bundle = base_queue / DNX_IPQ_MIN_BUNDLE_SIZE(unit);
    int core_idx;
    int nof_bundles_to_free;
    uint32 region_id;
    int bundle_iter;
    int element;
    dnx_ipq_region_type_e region_type = DNX_IPQ_REGION_TYPE_NONE;
    int nof_allocated_in_region;
    int num_cos;
    uint8 queue_is_alocated_on_another_core = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    /** Reset systemport ref counter */
    /** SwState array is with elements equal to NOF of Queue
     *  quarted.That is why SwState is acceessed with
     *  base_queue/min_queue_size */
    DNXCMN_CORES_ITER(unit, core, core_idx)
    {
        SHR_IF_ERR_EXIT(dnx_ipq_db.base_queues.sys_port_ref_counter.set(unit, core_idx, base_bundle, 0));
    }

    /**
     * Get number of allocated bits that associate with this element.
     * Need to free all of them.
     */

    SHR_IF_ERR_EXIT(dnx_ipq_queue_num_cos_get(unit, core, base_queue, &num_cos));
    nof_bundles_to_free = num_cos / DNX_IPQ_MIN_BUNDLE_SIZE(unit);

    region_id = base_queue / DNX_IPQ_NOF_QUEUES_IN_REGION(unit);
    SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.regions.type.get(unit, region_id, &region_type));
    /*
     * Interdigitated cause queues bundle to be doubled, because
     * only half of the queues in the bundle are active, so it takes
     * 2 bundles instead of 1.
     */
    if (region_type == DNX_IPQ_REGION_TYPE_INTERDIG)
    {
        nof_bundles_to_free *= 2;
    }

    /*
     * deallocate elements
     */
    for (bundle_iter = 0; bundle_iter < nof_bundles_to_free; bundle_iter++)
    {
        DNXCMN_CORES_ITER(unit, core, core_idx)
        {
            SHR_IF_ERR_EXIT(dnx_cosq_ipq_queue_bundle_element_get(unit, core_idx, base_bundle + bundle_iter, &element));
            SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.ipq_alloc_mngr.free_single(unit, element));
        }
    }

    /*
     * Reset queue's num_cos.
     */
    DNXCMN_CORES_ITER(unit, core, core_idx)
    {
        SHR_IF_ERR_EXIT(dnx_ipq_db.base_queues.num_cos.set(unit, core_idx, base_bundle, 0));
    }

    /*
     * Clear in SW Sate an indication if the queue was allocated symmetrically or not.
     * In case of asymmetrical VOQ, the indication will be cleared only if
     * the VOQ is not also allocated on another core
     */
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_idx)
    {
        SHR_IF_ERR_EXIT(dnx_ipq_queue_num_cos_get(unit, core_idx, base_queue, &num_cos));
        if ((core_idx != core) && (num_cos != 0))
        {
            queue_is_alocated_on_another_core = TRUE;
            break;
        }
    }
    if ((core == BCM_CORE_ALL) || (queue_is_alocated_on_another_core == FALSE))
    {
        SHR_IF_ERR_EXIT(dnx_ipq_db.base_queue_is_asymm.bit_clear(unit, base_queue));
    }

    /*
     * Check if we delete the last queue bundle in the region
     */
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_nof_allocated_elements_in_region_get(unit, region_id, &nof_allocated_in_region));

    if (nof_allocated_in_region == 0)
    {
        /*
         * we deleted last queue in the region.
         * set mode as not interdigitated.
         */
        SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.regions.type.set(unit, region_id, DNX_IPQ_REGION_TYPE_NONE));
        SHR_IF_ERR_EXIT(dnx_cosq_ipq_region_config_set(unit, region_id, FALSE));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file.
 */
shr_error_e
dnx_cosq_ipq_queue_is_allocated(
    int unit,
    int core,
    uint32 base_voq,
    int *is_allocated)
{
    uint8 sw_state_is_allocated = TRUE;
    int base_bundle;
    int element;
    int core_idx;
    SHR_FUNC_INIT_VARS(unit);

    base_bundle = base_voq / DNX_IPQ_MIN_BUNDLE_SIZE(unit);
    DNXCMN_CORES_ITER(unit, core, core_idx)
    {

        SHR_IF_ERR_EXIT(dnx_cosq_ipq_queue_bundle_element_get(unit, core_idx, base_bundle, &element));
        SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.ipq_alloc_mngr.is_allocated(unit, element, &sw_state_is_allocated));
        if (sw_state_is_allocated == FALSE)
        {
            /*
             * bundle is not allocated, exit and return FALSE
             */
            break;
        }
    }

    *is_allocated = sw_state_is_allocated;
exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file.
 */
shr_error_e
dnx_ipq_queue_sysport_map_get(
    int unit,
    int core,
    int queue,
    uint32 *sysport)
{
    int min_bundle_size = dnx_data_ipq.queues.min_bundle_size_get(unit);
    uint32 queue_quartet;
    SHR_FUNC_INIT_VARS(unit);

    if (queue >= dnx_data_ipq.queues.nof_queues_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "voq 0x%x is out of range", queue);
    }
    queue_quartet = queue / min_bundle_size;
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_queue_quartet_to_sys_port_map_get(unit, core, queue_quartet, sysport));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e
dnx_ipq_queue_num_cos_get(
    int unit,
    int core,
    int queue_id,
    int *num_cos)
{
    int queue_bundle;
    uint8 sw_state_cos;
    int core_idx;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * element represent bundle of queues, whereas base_queue holds the real queue id
     */
    queue_bundle = queue_id / DNX_IPQ_MIN_BUNDLE_SIZE(unit);

    /*
     * Get queue num_cos
     */
    core_idx = DNXCMN_CORE_ID2INDEX(unit, core);
    SHR_IF_ERR_EXIT(dnx_ipq_db.base_queues.num_cos.get(unit, core_idx, queue_bundle, &sw_state_cos));
    *num_cos = sw_state_cos;
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e
dnx_ipq_queue_is_multicast(
    int unit,
    int queue_id,
    int *is_multicast)
{
    int max_multicast_queue;

    SHR_FUNC_INIT_VARS(unit);

    *is_multicast = FALSE;

    /*
     * Get max FMQ from sw_state
     */
    SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.fmq.max_multicast_queue.get(unit, &max_multicast_queue));

    *is_multicast = UTILEX_IS_IN_RANGE(queue_id, 0, max_multicast_queue);

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e
dnx_ipq_queue_gport_get(
    int unit,
    int core,
    int queue_id,
    bcm_gport_t * queue_gport)
{
    int is_multicast;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get cosq gport from the queue id
     */
    SHR_IF_ERR_EXIT(dnx_ipq_queue_is_multicast(unit, queue_id, &is_multicast));
    if (is_multicast)
    {
        BCM_GPORT_MCAST_QUEUE_GROUP_CORE_QUEUE_SET(*queue_gport, core, queue_id);
    }
    else
    {
        BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(*queue_gport, core, queue_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Function to validate the inputs of
 * dnx_cosq_ipq_fabric_multicast_queue_range_get
 */
static int
dnx_cosq_ipq_fabric_multicast_queue_range_get_verify(
    int unit,
    int *queue_id)
{
    SHR_FUNC_INIT_VARS(unit);
    if (!dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "FMQ APIs are not supported on this device\n");
    }

    SHR_NULL_CHECK(queue_id, _SHR_E_PARAM, "queue_id");

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e
dnx_cosq_ipq_fabric_multicast_queue_range_get(
    int unit,
    int *queue_id)
{
    dnx_cosq_ipq_dbal_fmq_range_t fmq_range;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ipq_fabric_multicast_queue_range_get_verify(unit, queue_id));

    /*
     * Get queue id range
     */
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_fmq_range_get(unit, &fmq_range));

    *queue_id = fmq_range.last_queue;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Function to validate the inputs of
 * dnx_cosq_ipq_fabric_multicast_queue_range_set
 */
static int
dnx_cosq_ipq_fabric_multicast_queue_range_set_verify(
    int unit,
    int queue_id)
{
    int region_size = 0;
    int simple_mode_max_fmq = 0;
    int min_bundle_size = 0;
    int is_enhance_scheduler_mode = 0;
    int old_max_mc_queue = 0;
    uint8 num_cos = 0;
    int range_change_start = 0;
    int range_change_end = 0;
    int queue_i = 0;
    int base_queue = 0;
    int core_idx;
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "FMQ APIs are not supported on this device\n");
    }
    region_size = dnx_data_ipq.regions.nof_queues_get(unit);
    simple_mode_max_fmq = dnx_data_ipq.queues.fmq_bundle_size_get(unit) - 1;
    min_bundle_size = dnx_data_ipq.queues.min_bundle_size_get(unit);

    /*
     * Validate queue id
     */
    if (queue_id > dnx_data_ipq.queues.max_fmq_id_get(unit) || queue_id < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "queue_id %d invalid", queue_id);
    }

    /*
     * validate input per range type
     */
    if (((queue_id + 1) % region_size) && (queue_id != simple_mode_max_fmq))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FMQ queue_id max %d, must be ((multiple of %d)-1) or %d.", queue_id, region_size,
                     simple_mode_max_fmq);
    }

    /*
     * validate we are in FMQ enhance scheduler mode
     */
    SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.fmq.is_enhance_scheduler_mode.get(unit, &is_enhance_scheduler_mode));
    if (!is_enhance_scheduler_mode && (queue_id != simple_mode_max_fmq))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FMQ enhance scheduling mode must be set in order to change FMQs maximum range");
    }

    /*
     * Validate there are no allocated queues in the new range.
     */
    SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.fmq.max_multicast_queue.get(unit, &old_max_mc_queue));
    range_change_start = (old_max_mc_queue > queue_id) ? (queue_id + 1) : (old_max_mc_queue + 1);
    range_change_end = (old_max_mc_queue > queue_id) ? (old_max_mc_queue) : (queue_id);
    for (queue_i = range_change_start; queue_i < range_change_end; queue_i += min_bundle_size)
    {
        base_queue = queue_i / min_bundle_size;

        DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_idx)
        {
            SHR_IF_ERR_EXIT(dnx_ipq_db.base_queues.num_cos.get(unit, core_idx, base_queue, &num_cos));
            if (num_cos != 0)
            {
                SHR_ERR_EXIT(_SHR_E_EXISTS, "There are allocated queues in the range %d-%d."
                             " Need to deallocate them before changing queues range.", range_change_start,
                             range_change_end);
            }
        }

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Configure the VSQ categories boundaries.
 *   Currently only 2 categories are supported - Multicast (FMQ) and Unicast (Fabric).
 *
 * \param [in] unit -
 *   The unit number.
 * \param [in] queue_id - upper limit for the multicast category.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_cosq_ipq_vsq_multicast_category_set(
    int unit,
    int queue_id)
{
    dnx_cosq_ipq_dbal_vsq_categories_t vsq_categories;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Currently support 2 categories only :
     * Muticast (FMQ) is category 0 and Unicast (Fabric) is category 2.
     *
     * This function is invoked impicitly when Max Multicast queue id is set
     */

    vsq_categories.last_queue_vsq_category_0 = queue_id;
    vsq_categories.last_queue_vsq_category_1 = queue_id;
    vsq_categories.last_queue_vsq_category_2 = dnx_data_ipq.queues.nof_queues_get(unit) - 1;

    SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_vsq_categories_set(unit, &vsq_categories));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e
dnx_cosq_ipq_fabric_multicast_queue_range_set(
    int unit,
    int queue_id)
{
    dnx_cosq_ipq_dbal_fmq_range_t fmq_range;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify input
     */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ipq_fabric_multicast_queue_range_set_verify(unit, queue_id));

    /*
     * Set FMQ range to HW. FMQ range starts from queue 0, user configures the upper limit of the queue range.
     * when the upper queue limit > 3, the FMQ will be in enhanced mode, and these queues will require a scheduling scheme.
     */
    fmq_range.first_queue = 0;
    fmq_range.last_queue = queue_id;
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_fmq_range_set(unit, &fmq_range));

    /*
     * Set FMQ VSQ category.
     */
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_vsq_multicast_category_set(unit, fmq_range.last_queue));

    /*
     * Update FMQ range in sw_state
     */
    SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.fmq.max_multicast_queue.set(unit, fmq_range.last_queue));

exit:
    SHR_FUNC_EXIT;
}

/*
 * This is a callback function, registered to the alloc manager to be called
 * whenever a new region is allocated. At that time, we need to set the
 * region to be interdigitated or not.
 */
shr_error_e
dnx_cosq_ipq_region_config_set(
    int unit,
    int queue_region,
    int is_non_contiguous)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set is interdigitated mode for the region.
     */
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_region_is_interdigitated_set(unit, queue_region, is_non_contiguous));

exit:
    SHR_FUNC_EXIT;
}

/**
 * brief - Verify TC mapping gport for both functions
 * dnx_cosq_ipq_tc_profile_set/get
 */
static shr_error_e
dnx_cosq_ipq_tc_mapping_verify(
    int unit,
    bcm_port_t port)
{
    bcm_cos_queue_t cosq = 0;
    int system_port = 0, core, queue_id, force_core_all = FALSE;
    uint32 max_system_ports = dnx_data_device.general.max_nof_system_ports_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) || BCM_GPORT_IS_MCAST_QUEUE_GROUP(port))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_gport_to_voq_get(unit, port, cosq, force_core_all, &core, &queue_id));
    }
    else
    {
        /** Gport is system port */
        system_port = BCM_GPORT_SYSTEM_PORT_ID_GET(port);
        SHR_RANGE_VERIFY(system_port, 0, max_system_ports - 1, _SHR_E_PARAM, "System port out of bounds\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_cosq_ipq_voq_offset_mapping_set(
    int unit,
    bcm_gport_t gport,
    int count,
    bcm_cosq_queue_offset_mapping_key_t key_array[],
    bcm_cosq_queue_offset_t offset_array[])
{
    int min_bundle_size = DNX_IPQ_MIN_BUNDLE_SIZE(unit);
    uint32 destination, sys_tc, voq_offset, old_profile, is_fmq;
    dnx_ipq_tc_map_t tc_mapping;
    int is_flow, dest_base_queue = 0, dest_ndx = 0, is_last, is_allocated;
    int nof_quartet, quartet_index, new_profile;
    int sw_state_num_cos;
    int i;
    int nof_dps, dp;
    int start_dp, end_dp;
    int core, core_idx;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ipq_tc_mapping_verify(unit, gport));

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport))
    {
        is_flow = TRUE;
        dest_base_queue = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(gport);
        core = BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_GET(gport);
    }
    else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport))
    {
        is_flow = TRUE;
        dest_base_queue = BCM_GPORT_MCAST_QUEUE_GROUP_QID_GET(gport);
        core = BCM_GPORT_MCAST_QUEUE_GROUP_CORE_GET(gport);
    }
    else
    {
        /** Gport is system port */
        is_flow = FALSE;
        dest_ndx = BCM_GPORT_SYSTEM_PORT_ID_GET(gport);

        /*
         * system port configuration is common to all cores.
         * core id is not used for DBAL access (SBC) and not used by profile template manager.
         * set core to '0' in order to iterate only once on TC profile update.
         */
        core = 0;
    }

    if (is_flow)
    {
        /*
         * if dest_ndx is flow then we need to find its num of quartets
         */
        SHR_IF_ERR_EXIT(dnx_ipq_queue_num_cos_get(unit, core, dest_base_queue, &sw_state_num_cos));
        nof_quartet = sw_state_num_cos / min_bundle_size;

        /*
         * dest_ndx is the flow id before its mapped to VOQ: flow_id = dest_base_queue - flow_profile_offset
         * Flows can be connected to a flow profile.
         * each flow profile can be set to start from a different queue_base, and this is the flow_profile_offset.
         * but we always set all profiles to base_queue 0, so effectively dest_ndx = dest_base_queue.
         */
        dest_ndx = dest_base_queue;
    }
    else
    {
        /*
         * if dest_ndx is system port than num of quartets is 1
         */
        nof_quartet = 1;
    }

    DNXCMN_CORES_ITER(unit, core, core_idx)
    {
        /*
         * Get old data
         */
        SHR_IF_ERR_EXIT(dnx_cosq_ipq_tc_map_profile_data_get(unit, core_idx, dest_ndx, is_flow, &tc_mapping));

        nof_dps =
            dnx_data_ipq.queues.feature_get(unit, dnx_data_ipq_queues_voq_resolution_per_dp) ? DNX_COSQ_NOF_DP : 1;

        /*
         * Set data configuration
         */
        for (i = 0; i < count; i++)
        {
            sys_tc = key_array[i].tc;
            voq_offset = offset_array[i].cosq;

            if (key_array[i].dp == BCM_COS_INVALID)
            {
                start_dp = 0;
                end_dp = nof_dps;
            }
            else
            {
                start_dp = key_array[i].dp;
                end_dp = start_dp + 1;
            }
            for (dp = start_dp; dp < end_dp; dp++)
            {
                tc_mapping.offset[sys_tc][dp] = voq_offset;
            }
        }
        /*
         * Exchange SW Data
         */
        SHR_IF_ERR_EXIT(dnx_cosq_ipq_tc_map_profile_exchange
                        (unit, core_idx, dest_ndx, is_flow, &tc_mapping, &old_profile, &new_profile, &is_last,
                         &is_allocated));

        if (is_allocated)
        {
            /*
             * Set new profile data
             */
            is_fmq = FALSE;
            SHR_IF_ERR_EXIT(dnx_cosq_ipq_tc_to_voq_offset_map_set
                            (unit, core_idx, new_profile, is_flow, is_fmq, &tc_mapping));
        }

        /*
         * for each quartet, configure its profile using new_profile
         */
        for (quartet_index = 0; quartet_index < nof_quartet; quartet_index++)
        {
            /*
             * Set new mapping.
             */
            destination = dest_ndx + (quartet_index * min_bundle_size);
            SHR_IF_ERR_EXIT(dnx_cosq_ipq_tc_profile_set(unit, core_idx, destination, is_flow, new_profile));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_cosq_ipq_voq_offset_mapping_get(
    int unit,
    bcm_gport_t gport,
    int count,
    bcm_cosq_queue_offset_mapping_key_t key_array[],
    bcm_cosq_queue_offset_t offset_array[])
{
    dnx_ipq_tc_map_t tc_mapping;
    int dest_ndx = 0;
    int is_flow = 0;
    int i, tc, dp;
    int core = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ipq_tc_mapping_verify(unit, gport));

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport))
    {
        is_flow = 1;
        dest_ndx = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(gport);
        core = BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_GET(gport);
    }
    else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport))
    {
        is_flow = 1;
        dest_ndx = BCM_GPORT_MCAST_QUEUE_GROUP_QID_GET(gport);
        core = BCM_GPORT_MCAST_QUEUE_GROUP_CORE_GET(gport);
    }
    else if (BCM_GPORT_IS_SYSTEM_PORT(gport))
    {
        is_flow = 0;
        dest_ndx = BCM_GPORT_SYSTEM_PORT_ID_GET(gport);

        /*
         * system port configuration is common to all cores.
         * set core to '0' assuming symmetrical configuration.
         */
        core = 0;

    }

    /*
     * For symmetric configuration get first core
     */
    if (core == BCM_CORE_ALL)
    {
        core = 0;
    }

    /*
     * Get old data
     */
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_tc_map_profile_data_get(unit, core, dest_ndx, is_flow, &tc_mapping));

    for (i = 0; i < count; i++)
    {
        tc = key_array[i].tc;
        dp = (key_array[i].dp == BCM_COS_INVALID ? 0 : key_array[i].dp);
        offset_array[i].cosq = tc_mapping.offset[tc][dp];
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Map system TC (TC on the packet) and DP to VOQ offset for FMQ
 */
shr_error_e
dnx_cosq_ipq_fmq_voq_offset_mapping_set(
    int unit,
    int count,
    bcm_cosq_queue_offset_mapping_key_t key_array[],
    bcm_cosq_queue_offset_t offset_array[])
{
    int i;
    int tc, dp, start_dp, end_dp, nof_dps;

    uint32 is_fmq = TRUE, is_flow = FALSE, profile = 0;
    uint32 voq_offset;

    SHR_FUNC_INIT_VARS(unit);

    nof_dps = dnx_data_ipq.queues.feature_get(unit, dnx_data_ipq_queues_voq_resolution_per_dp) ? DNX_COSQ_NOF_DP : 1;
    for (i = 0; i < count; i++)
    {
        tc = key_array[i].tc;
        if (key_array[i].dp == BCM_COS_INVALID)
        {
            start_dp = 0;
            end_dp = nof_dps;
        }
        else
        {
            start_dp = key_array[i].dp;
            end_dp = start_dp + 1;
        }
        voq_offset = offset_array[i].cosq;

        for (dp = start_dp; dp < end_dp; dp++)
        {

            SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_tc_to_voq_offset_map_set
                            (unit, BCM_CORE_ALL, tc, dp, profile, is_flow, is_fmq, voq_offset));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Get mapping of system TC and DP to VOQ offset for FMQ
 */
shr_error_e
dnx_cosq_ipq_fmq_voq_offset_mapping_get(
    int unit,
    int count,
    bcm_cosq_queue_offset_mapping_key_t key_array[],
    bcm_cosq_queue_offset_t offset_array[])
{
    int i;
    int tc, dp;

    uint32 is_fmq = TRUE, is_flow = FALSE, profile = 0;

    SHR_FUNC_INIT_VARS(unit);

    for (i = 0; i < count; i++)
    {
        tc = key_array[i].tc;
        dp = key_array[i].dp == BCM_COS_INVALID ? 0 : key_array[i].dp;

        SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_tc_to_voq_offset_map_get(unit, BCM_CORE_ALL, tc, dp, profile, is_flow, is_fmq,
                                                                   (uint32 *) &offset_array[i].cosq));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
* check modid override
*/
static shr_error_e
dnx_cosq_ipq_connection_set_modid_override(
    int unit,
    int base_qid,
    int queue_quad_base,
    int ingress_core,
    bcm_cosq_gport_connection_t * gport_connect)
{

    uint32 sys_port_id = 0, tm_port_id = 0, new_sys_port_id = 0;
    bcm_gport_t mod_port_gport = 0, sys_port_gport = 0, new_sys_port_gport = 0;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    bcm_core_t remote_core;
    int fap_id_index;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);

    /** first, check that ingress_core is diff from core all **/
    if (ingress_core == BCM_CORE_ALL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "API is not supported when ingress core is core-all.\n");
    }

    /** voq to sysport **/
    SHR_IF_ERR_EXIT(dnx_ipq_queue_sysport_map_get(unit, ingress_core, base_qid, &sys_port_id));
    /** sysport to modport , translate uint 32 to gport **/
    BCM_GPORT_SYSTEM_PORT_ID_SET(sys_port_gport, sys_port_id);
    SHR_IF_ERR_EXIT(bcm_dnx_stk_sysport_gport_get(unit, sys_port_gport, &mod_port_gport));
    /** update the modport with the new fapid **/
    /** get tm port from old modport**/
    tm_port_id = BCM_GPORT_MODPORT_PORT_GET(mod_port_gport);
    /** wishing modport to sysport. trying get local port from fap id + tm port**/
    SHR_IF_ERR_EXIT(dnx_stk_sys_modid_local_core_get(unit, gport_connect->remote_modid, &remote_core, &fap_id_index));
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, remote_core, tm_port_id, &logical_port));

    /** modport to new sysport. not using the api so that warning won't come out **/
    /*
     * Get system port from gport
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, logical_port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_info));
    /*
     * Getting system port from modport is not supported. Make sure that indeed gport management managed to
     * retrieve the system port
     */
    if (gport_info.sys_port == -1)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                     "API is not supported for non-local ports. Getting port attributes should use system port explicitly.\n");
    }
    /** Create the required gport */
    BCM_GPORT_SYSTEM_PORT_ID_SET(new_sys_port_gport, gport_info.sys_port);
    /** translate new_sys_port to sys port id = uint32 **/
    new_sys_port_id = BCM_GPORT_SYSTEM_PORT_ID_GET(new_sys_port_gport);
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_queue_quartet_to_sys_port_map_range_set(unit,
                                                                              ingress_core, queue_quad_base, 1,
                                                                              new_sys_port_id));
exit:
    SHR_FUNC_EXIT;
}

/*
 *  See .h file
 */
shr_error_e
dnx_cosq_ipq_gport_connection_set(
    int unit,
    bcm_cosq_gport_connection_t * gport_connect)
{
    uint8 nof_cos;
    int is_remove = FALSE;
    int base_qid, base_cid, queue_quad_base, new_queue_quad, queue_region, quad;
    int ingress_core = 0, core_id = 0;
    int flow_subflow[2], nof_subflows, flow_id;
    int min_bundle_size = dnx_data_ipq.queues.min_bundle_size_get(unit);
    int nof_cores, egress_region_size, nof_quads, modid_override;
    uint32 is_non_contiguous, is_composite, flow_quartet_index;

    SHR_FUNC_INIT_VARS(unit);

    is_remove = (gport_connect->flags & BCM_COSQ_GPORT_CONNECTION_INVALID) ? TRUE : FALSE;

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport_connect->voq))
    {
        base_qid = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(gport_connect->voq);
        ingress_core = BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_GET(gport_connect->voq);
    }
    else
    {
        base_qid = BCM_GPORT_MCAST_QUEUE_GROUP_GET(gport_connect->voq);
        ingress_core = BCM_GPORT_MCAST_QUEUE_GROUP_CORE_GET(gport_connect->voq);
    }

    queue_quad_base = base_qid / min_bundle_size;
    base_cid = BCM_COSQ_GPORT_VOQ_CONNECTOR_ID_GET(gport_connect->voq_connector);

    /*
     * check modid override
     */
    modid_override = (gport_connect->flags & BCM_COSQ_GPORT_CONNECTION_MODID_OVERRIDE) ? TRUE : FALSE;
    if (modid_override)
    {
        SHR_IF_ERR_EXIT(dnx_cosq_ipq_connection_set_modid_override
                        (unit, base_qid, queue_quad_base, ingress_core, gport_connect));
    }
    /*
     * Determine queue region mode - is interdigitated
     */
    queue_region = base_qid / dnx_data_ipq.regions.nof_queues_get(unit);
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_region_is_interdigitated_get(unit, queue_region, &is_non_contiguous));

    DNXCMN_CORES_ITER(unit, ingress_core, core_id)
    {
        SHR_IF_ERR_EXIT(dnx_ipq_db.base_queues.num_cos.get(unit, core_id, queue_quad_base, &nof_cos));
        nof_quads = nof_cos / min_bundle_size;

        /*
         * Get queue quartet to flow id mapping
         */
        SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_queue_quartet_to_flow_map_is_composite_get
                        (unit, core_id, queue_quad_base, &is_composite));

        for (quad = 0; quad < nof_quads; quad++)
        {
            /*
             * Get Connector flow id
             */
            SHR_IF_ERR_EXIT(dnx_scheduler_connector_flow_id_calc(unit, core_id, base_cid,
                                                                 quad * min_bundle_size, is_non_contiguous,
                                                                 is_composite, is_remove, flow_subflow, &nof_subflows));
            /*
             * The first flow id is always in subflow[0]
             */
            flow_id = flow_subflow[0];

            /*
             * when creating a connection to both cores, we need to take core 1 into account manually,
             * since the connector flow id returned from scheduler
             * will return the flow id of core 0
             */
            if (!is_remove && ingress_core == BCM_CORE_ALL)
            {
                /*
                 * Note: there is an assumption here that the region size in the remote device
                 * is the same as the region size in this device!
                 */
                egress_region_size = dnx_data_sch.flow.region_size_get(unit);
                nof_cores = dnx_data_device.general.nof_cores_get(unit);
                flow_id += (egress_region_size / (nof_cores)) * (core_id);
            }

            flow_quartet_index = DNX_SCHEDULER_FLOW_ID_TO_QUARTET(flow_id);

            new_queue_quad = queue_quad_base + quad;
            SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_queue_quartet_to_flow_map_flow_id_set
                            (unit, core_id, new_queue_quad, flow_quartet_index));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 *  See .h file
 */
int
dnx_cosq_ipq_gport_connection_get(
    int unit,
    bcm_cosq_gport_connection_t * gport_connect)
{
    bcm_gport_t sysport_gport, modport_gport;
    int queue_region, queue_quad_base, base_qid = 0;
    int egress_core, ingress_core = 0, core_id, connector_id;
    int min_bundle_size = dnx_data_ipq.queues.min_bundle_size_get(unit);
    uint32 is_interdigitated, flow_quartet_index;
    uint32 sysport = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport_connect->voq))
    {
        base_qid = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(gport_connect->voq);
        ingress_core = BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_GET(gport_connect->voq);
    }
    else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport_connect->voq))
    {
        base_qid = BCM_GPORT_MCAST_QUEUE_GROUP_GET(gport_connect->voq);
        ingress_core = BCM_GPORT_MCAST_QUEUE_GROUP_CORE_GET(gport_connect->voq);
    }

    /*
     * Since the scheduler at the egress side is not necessarily configured symmetrically,
     * the connection from ingress VOQ to egress flow does not have to be symmetrical in both cores.
     */
    core_id = DNXCMN_CORE_ID2INDEX(unit, ingress_core);

    queue_quad_base = base_qid / min_bundle_size;
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_queue_quartet_to_flow_map_flow_id_get
                    (unit, core_id, queue_quad_base, &flow_quartet_index));

    queue_region = base_qid / dnx_data_ipq.regions.nof_queues_get(unit);
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_region_is_interdigitated_get(unit, queue_region, &is_interdigitated));

    /*
     * 'gport_connect->voq_connector' is used as INOUT parameter in this function.
     * here it is used to get the core on the egress device.
     * This info is not known in this procedure, so it is supplied by the caller.
     */
    egress_core = BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_GET(gport_connect->voq_connector);

    connector_id = DNX_SCHEDULER_QUARTET_TO_FLOW_ID(flow_quartet_index);
    if (is_interdigitated)
    {
        /*
         * if the flow is interdigitated, the connector index is not the first flow id within the quad
         */
        connector_id += dnx_data_sch.flow.conn_idx_interdig_flow_get(unit);
    }

    /*
     * here the 'gport_connect->voq_connector' is set to the correct value.
     */
    BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(gport_connect->voq_connector, connector_id, egress_core);

    /*
     * Get the mapping from queue quartet to system port
     */
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_queue_quartet_to_sys_port_map_get(unit, core_id, queue_quad_base, &sysport));
    BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, sysport);

    /*
     * Get remote modid from sysport
     */
    SHR_IF_ERR_EXIT(bcm_dnx_stk_sysport_gport_get(unit, sysport_gport, &modport_gport));
    gport_connect->remote_modid = BCM_GPORT_MODPORT_MODID_GET(modport_gport);
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Check if the given system port is on another TM domain. if yes, it means we have a system with Stacking.
 *
 * \param [in] unit -The unit number.
 * \param [in] sysport_gport - system port gport
 * \param [out] is_stk - output value to return if stacking exist.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_cosq_ipq_sysport_is_stk_get(
    int unit,
    bcm_gport_t sysport_gport,
    uint32 *is_stk)
{
    int sysport_modid;
    uint32 is_same_domain = 0;
    bcm_gport_t modport_gport;
    uint32 *local_modid = NULL;
    int nof_fap_ids_per_core;

    SHR_FUNC_INIT_VARS(unit);

    *is_stk = FALSE;

    /*
     * get this unit local modid
     */
    local_modid = sal_alloc(sizeof(uint32) * dnx_data_device.general.max_nof_fap_ids_per_core_get(unit), "local_modid");
    SHR_IF_ERR_EXIT(dnx_stk_sys_modid_get_all(unit, 0 /** core */ , local_modid, &nof_fap_ids_per_core));
    /*
     * get the modid of the given system port
     */
    SHR_IF_ERR_EXIT(bcm_dnx_stk_sysport_gport_get(unit, sysport_gport, &modport_gport));
    sysport_modid = BCM_GPORT_MODPORT_MODID_GET(modport_gport);
    /*
     * check if local modid and the given modid are on the same TM domain
     */
    /** note: all fap ids of local device belong to the same TM domain */
    SHR_IF_ERR_EXIT(dnx_stk_domain_stk_modid_to_domain_find(unit, local_modid[0], sysport_modid, &is_same_domain));

    if (is_same_domain == FALSE)
    {
        /*
         * if the local modid and the modid of the given system port are not on the same TM domain,
         * it means stacking is enabled.
         */
        *is_stk = TRUE;
    }
exit:
    SHR_FREE(local_modid);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Map the system port to the reserved stacking queues.
 * When stacking is enabled, the table which maps system ports to base VOQs reserves the last entries for stacking.
 * In this entries, we write the TM domain to which the destination system port belongs.
 * from there the stacking resolution takes over.
 *
 * \param [in] unit -The unit number.
 * \param [in] sysport_gport - system port gport
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_cosq_ipq_system_port_to_stk_queue_map_set(
    int unit,
    bcm_gport_t sysport_gport)
{
    int sysport, sysport_modid, sysport_tm_domain;
    bcm_gport_t modport_gport;
    uint32 stk_base_queue;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * get sysport from system port Gport
     */
    sysport = BCM_GPORT_SYSTEM_PORT_ID_GET(sysport_gport);

    /*
     * get the modid of the given system port
     */
    SHR_IF_ERR_EXIT(bcm_dnx_stk_sysport_gport_get(unit, sysport_gport, &modport_gport));
    sysport_modid = BCM_GPORT_MODPORT_MODID_GET(modport_gport);

    /*
     * get the TM domain of the given syste port
     */
    SHR_IF_ERR_EXIT(dnx_stk_domain_stk_domain_modid_get(unit, sysport_modid, &sysport_tm_domain));

    /*
     * get the base queue reserved for staking for the given TM port
     */
    stk_base_queue = sysport_tm_domain + dnx_data_stack.general.dest_port_base_queue_min_get(unit);

    /*
     * fill the queue info to be written to HW
     */
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_system_port_to_voq_base_set(unit, BCM_CORE_ALL, sysport, stk_base_queue, TRUE));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - After a base queue was allocated, map the queue to system port and flow info.
 * if required, map from sysport to the queue, and update the sysport ref count.
 *
 * \param [in] unit -The unit number.
 * \param [in] core - core id.
 * \param [in] sysport- system port. (not Gport!)
 * \param [in] base_voq- base VOQ allocated beforehand.
 * \param [in] numq- number of queues allocated.
 * \param [in] is_flow- if set, the user requested to have flow based mapping,
 * which means we do not need to map from system port to voq.
 * \param [in] is_composite- specify whether the connector for the VOQ is a composite
 * (gets crdits from 2 parents instead of one).
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_cosq_ipq_gport_add_queue_map_set(
    int unit,
    int core,
    int sysport,
    int base_voq,
    int numq,
    int is_flow,
    int is_composite)
{
    int base_voq_qrtt = 0, num_quartets;
    int min_bundle_size = dnx_data_ipq.queues.min_bundle_size_get(unit);
    int core_idx;
    SHR_FUNC_INIT_VARS(unit);

    /** Increase the system port ref counter for the VOQ */
    /*
     * SwState array is with elements equal to NOF of Queue quartet.
     * That is why SwState is accessed with base_queue/min_queue_size.
     */
    base_voq_qrtt = base_voq / min_bundle_size;

    DNXCMN_CORES_ITER(unit, core, core_idx)
    {
        SHR_IF_ERR_EXIT(dnx_ipq_db.base_queues.sys_port_ref_counter.inc(unit, core_idx, base_voq_qrtt, 1));
    }

    num_quartets = numq / min_bundle_size;
    /*
     * Set is composite to all queue quartets connected flows
     */
    if (is_composite)
    {
        SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_queue_quartet_to_flow_map_is_composite_range_set
                        (unit, core, base_voq_qrtt, num_quartets, is_composite));
    }

    /*
     * Base queue to sysport mapping
     */
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_queue_quartet_to_sys_port_map_range_set
                    (unit, core, base_voq_qrtt, num_quartets, sysport));

    /*
     * sysport to base queue mapping. This not required in case of flow based forwarding.
     */
    if (!is_flow)
    {
        SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_system_port_to_voq_base_set(unit, core, sysport, base_voq, TRUE));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - After a base queue was allocated,
 * map the queue to the relevant credit request and rate class profiles.
 *
 * \param [in] unit -The unit number.
 * \param [in] core - Device core id.
 * \param [in] base_voq- base VOQ allocated beforehand.
 * \param [in] config- configuration info from the user.
 * here we get the requested profiles to set to the base queue.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_cosq_ipq_gport_add_queue_profile_set(
    int unit,
    int core,
    int base_voq,
    bcm_cosq_ingress_queue_bundle_gport_config_t * config)
{
    int cosq, is_add = TRUE;
    int delay_tolerance_level, delay_tolerance_profile;
    uint32 rate_class;
    int core_idx;
    SHR_FUNC_INIT_VARS(unit);

    for (cosq = 0; cosq < config->numq; cosq++)
    {
        delay_tolerance_level = config->queue_atrributes[cosq].delay_tolerance_level;
        rate_class = config->queue_atrributes[cosq].rate_class;
        /*
         * convert to actual credit request profile id
         */
        SHR_IF_ERR_EXIT(dnx_iqs_api_delay_tolerance_level_to_profile_id_get
                        (unit, delay_tolerance_level, &delay_tolerance_profile));
        /*
         * Map each queue offset to relevant credit request profile
         */
        SHR_IF_ERR_EXIT(dnx_iqs_credit_request_profile_map_set(unit, core, base_voq, cosq, delay_tolerance_profile));
        /*
         * Set new rate class in HW
         */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_voq_rate_class_set(unit, core, base_voq + cosq, rate_class));
        /*
         * update ref count for the new rate class profile
         */
        DNXCMN_CORES_ITER(unit, core, core_idx)
        {
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.voq_rate_class.ref_count.inc(unit, core_idx, rate_class, 1));
        }
    }

    /*
     * update shared resource according to rate class profile guaranteed
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_voq_guaranteed_set(unit, is_add, config));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify bcm_dnx_dnx_cosq_ingress_queue_bundle_gport_add parameters for ucast gport
 *
 * \param [in] unit -The unit number.
 * \param [in] config- user queue configuration from API.
 * \param [in] gport- requested queue Gport (in case WITH_ID is specified)
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_cosq_ipq_ucast_gport_add_verify(
    int unit,
    bcm_cosq_ingress_queue_bundle_gport_config_t * config,
    bcm_gport_t * gport)
{
    int base_voq, core;
    uint32 supported_flags;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify local core id.
     */
    DNXCMN_CORE_VALIDATE(unit, config->local_core_id, TRUE);

    /*
     * Verify port - make sure it is a system port
     */
    if (!BCM_GPORT_IS_SYSTEM_PORT(config->port))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Gport(0x%08x) must be a system port gport.", config->port);
    }

    supported_flags =
        (BCM_COSQ_GPORT_UCAST_QUEUE_GROUP | BCM_COSQ_GPORT_WITH_ID | BCM_COSQ_GPORT_NON_CONTIGUOUS_VOQ_CONNECTOR |
         BCM_COSQ_GPORT_COMPOSITE | BCM_COSQ_GPORT_TM_FLOW_ID);
    SHR_MASK_VERIFY(config->flags, supported_flags, _SHR_E_PARAM, "non supported flags specified.\n");

    if (config->flags & BCM_COSQ_GPORT_WITH_ID)
    {
        base_voq = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(*gport);
        core = BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_GET(*gport);

        if (base_voq + config->numq >= dnx_data_ipq.queues.nof_queues_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Add uc q gport(0x%08x) with_id failed, queue %d is out of range",
                         *gport, base_voq + config->numq);
        }

        if (core != config->local_core_id)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "gport core %d does not match local core id %d", core, config->local_core_id);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_cosq_ipq_ucast_gport_add(
    int unit,
    bcm_cosq_ingress_queue_bundle_gport_config_t * config,
    bcm_gport_t * gport)
{
    bcm_gport_t sysport_gport = config->port;
    uint32 flags = config->flags;
    uint32 stk_exist;
    int numq = config->numq;
    int is_non_contiguous = FALSE, is_flow = FALSE, is_composite = FALSE;
    int alloc_flags = 0, base_voq = 0, sysport;
    dnx_ipq_queue_type_e queue_type = DNX_ALGO_IPQ_QUEUE_TYPE_INVALID;
    int core = config->local_core_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ipq_ucast_gport_add_verify(unit, config, gport));

    sysport = BCM_GPORT_SYSTEM_PORT_ID_GET(sysport_gport);

    if (BCM_COSQ_GPORT_NON_CONTIGUOUS_VOQ_CONNECTOR & flags)
    {
        is_non_contiguous = TRUE;
    }
    if (BCM_COSQ_GPORT_TM_FLOW_ID & flags)
    {
        /*
         *  when is_flow is set, it means the mapping to voq will be given in direct flow id instead of system port.
         *  so we dont need to configure the mapping from system port to VOQ.
         */
        is_flow = TRUE;
    }
    if (BCM_COSQ_GPORT_COMPOSITE & flags)
    {
        is_composite = TRUE;
    }
    if (BCM_COSQ_GPORT_WITH_ID & flags)
    {
        base_voq = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(*gport);
        alloc_flags |= DNX_ALGO_RES_ALLOCATE_WITH_ID;
    }
    else
    {
        *gport = -1;
    }

    /*
     * Handle Staking - if exist.
     * Relevant only if not in flow based mapping
     */
    if (!is_flow)
    {
        SHR_IF_ERR_EXIT(dnx_cosq_ipq_sysport_is_stk_get(unit, sysport_gport, &stk_exist));
        if (stk_exist)
        {
            /*
             * When stacking is enabled (using Soc property),
             * we use the [sysport->queue] table to map from sysport to the remote TM domain.
             * so in case of stacking there is nothing more else to do.
             */
            SHR_IF_ERR_EXIT(dnx_cosq_ipq_system_port_to_stk_queue_map_set(unit, sysport_gport));
            SHR_EXIT();
        }
    }

    /*
     * Allocate queue
     */
    if (is_non_contiguous)
    {
        queue_type = DNX_ALGO_IPQ_QUEUE_TYPE_UNICAST_INTERDIGITATED;
    }
    else
    {
        queue_type = DNX_ALGO_IPQ_QUEUE_TYPE_UNICAST;
    }
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_queue_allocate(unit, core, alloc_flags, queue_type, numq, &base_voq));

    /*
     * Map the allocated VOQ to and from system port
     */
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_gport_add_queue_map_set(unit, core, sysport, base_voq, numq, is_flow, is_composite));

    /*
     * Set Queue profiles - credit request profile and rate class according to config info
     */
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_gport_add_queue_profile_set(unit, core, base_voq, config));

    /*
     * set the output gport with the allocated base voq.
     */
    BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(*gport, core, base_voq);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify bcm_dnx_dnx_cosq_ingress_queue_bundle_gport_add parameters for mcast gport
 *
 * \param [in] unit -The unit number.
 * \param [in] config- user queue configuration from API.
 * \param [in] gport- requested queue Gport (in case WITH_ID is specified)
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_cosq_ipq_mcast_gport_add_verify(
    int unit,
    bcm_cosq_ingress_queue_bundle_gport_config_t * config,
    bcm_gport_t * gport)
{
    int core, base_fmq, end_fmq, last_queue;
    uint32 supported_mcast_flags;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * MC VOQs support only symmetrical configuration
     */
    if (config->local_core_id != BCM_CORE_ALL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "core must be BCM_CORE_ALL (core id %d)", config->local_core_id);
    }

    supported_mcast_flags = (BCM_COSQ_GPORT_WITH_ID | BCM_COSQ_GPORT_MCAST_QUEUE_GROUP);
    SHR_MASK_VERIFY(config->flags, supported_mcast_flags, _SHR_E_PARAM, "non supported flags specified.\n");

    if (BCM_COSQ_GPORT_WITH_ID & config->flags)
    {
        base_fmq = BCM_GPORT_MCAST_QUEUE_GROUP_QID_GET(*gport);
        core = BCM_GPORT_MCAST_QUEUE_GROUP_CORE_GET(*gport);
        /*
         * Range check - check we are not out of bounds of FMQ range
         */
        SHR_IF_ERR_EXIT(bcm_dnx_fabric_control_get(unit, bcmFabricMulticastQueueMax, &end_fmq));
        last_queue = base_fmq + config->numq - 1;
        if (last_queue > end_fmq)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Queue %d is out of range of multicast fabric queues ", last_queue);
        }

        if (core != config->local_core_id)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "gport core %d does not match local core id %d", core, config->local_core_id);
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Add mc q gport(0x%08x) not supported without WITH_ID flag", *gport);
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_cosq_ipq_mcast_gport_add(
    int unit,
    bcm_cosq_ingress_queue_bundle_gport_config_t * config,
    bcm_gport_t * gport)
{
    int numq = config->numq;
    uint32 flags = config->flags;
    int base_fmq = 0, alloc_flags = 0, is_composite = FALSE, sysport;
    int is_enhance_scheduler_mode = 0;
    int core = config->local_core_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ipq_mcast_gport_add_verify(unit, config, gport));

    if (BCM_COSQ_GPORT_COMPOSITE & flags)
    {
        is_composite = TRUE;
    }
    if (BCM_COSQ_GPORT_WITH_ID & flags)
    {
        base_fmq = BCM_GPORT_MCAST_QUEUE_GROUP_QID_GET(*gport);
        alloc_flags |= DNX_ALGO_RES_ALLOCATE_WITH_ID;
    }

    /*
     * Simple FMQs mode - System port is not relevant (Set sysport to invalid_sysport).
     * Enhanced FMQs mode - System port should be mapped to local fap ID since local scheduler provides credits (Set
     * sysport to dedicated pre-confiugred system port - fmq_sysport).
     */
    SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.fmq.is_enhance_scheduler_mode.get(unit, &is_enhance_scheduler_mode));
    if (is_enhance_scheduler_mode == 0)
    {
        base_fmq = 0;
        sysport = dnx_data_device.general.invalid_system_port_get(unit);
    }
    else
    {
        sysport = dnx_data_device.general.fmq_system_port_get(unit);
    }
    /*
     * Allocate multicast queue
     */
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_queue_allocate
                    (unit, core, alloc_flags, DNX_ALGO_IPQ_QUEUE_TYPE_MULTICAST, numq, &base_fmq));

    /*
     * Map the allocated VOQ to system port
     * (is_flow is passed as TRUE since there is not need to map system port to VOQ base)
     */
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_gport_add_queue_map_set(unit, core, sysport, base_fmq, numq, TRUE, is_composite));
    /*
     * Set Queue profiles - credit request profile and rate class according to config info
     */
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_gport_add_queue_profile_set(unit, core, base_fmq, config));

    BCM_GPORT_MCAST_QUEUE_GROUP_CORE_QUEUE_SET(*gport, core, base_fmq);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify Unicast gport
 *
 * \param [in] unit -The unit number.
 * \param [in] gport- requested queue Gport
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_cosq_ipq_ucast_gport_verify(
    int unit,
    bcm_gport_t gport)
{
    int base_voq = 0;
    int core = BCM_CORE_ALL;
    uint32 min_bundle_size = dnx_data_ipq.queues.min_bundle_size_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    base_voq = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(gport);
    core = BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_GET(gport);

    DNXCMN_CORE_VALIDATE(unit, core, TRUE);

    if (base_voq >= dnx_data_ipq.queues.nof_queues_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Base VOQ %d is out of range", base_voq);
    }
    if (base_voq % min_bundle_size != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Base VOQ %d should be a multiply of %d \n", base_voq, min_bundle_size);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify Multiicast gport
 *
 * \param [in] unit -The unit number.
 * \param [in] gport- requested queue Gport
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_cosq_ipq_mcast_gport_verify(
    int unit,
    bcm_gport_t gport)
{
    int base_fmq = 0, end_fmq = 0;
    int core = BCM_CORE_ALL;
    SHR_FUNC_INIT_VARS(unit);

    base_fmq = BCM_GPORT_MCAST_QUEUE_GROUP_QID_GET(gport);
    core = BCM_GPORT_MCAST_QUEUE_GROUP_CORE_GET(gport);

    SHR_IF_ERR_EXIT(bcm_dnx_fabric_control_get(unit, bcmFabricMulticastQueueMax, &end_fmq));

    if (base_fmq > end_fmq)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Queue %d out of range of multicast fabric queues ", base_fmq);
    }
    if (core != BCM_CORE_ALL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Gport(0x%08x), core must be BCM_CORE_ALL", gport);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Gport get verify function.
 *
 * \param [in] unit -The unit number.
 * \param [in] core -core device id.
 * \param [in] base_voq - base queue for which we want to get the info. extracted from the Gport given by the user.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_cosq_ipq_gport_get_verify(
    int unit,
    int core,
    uint32 base_voq)
{
    int is_allocated;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ipq_queue_is_allocated(unit, core, base_voq, &is_allocated));

    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Queue %d is not allocated", base_voq);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Gport get function. this function is called by both
 * unicast and multicast gport get methods and doing the common work.
 *
 * \param [in] unit -The unit number.
 * \param [in] core -core device id.
 * \param [in] base_voq - base queue for which we want to get the info. extracted from the Gport given by the user.
 * \param [in] is_mc - flag to specify if are we getting Unicast or Multicast queue info.
 * \param [out] physical_port - physical port info (modid and port) of the system port connected to the base queue
 * \param [out] num_cos_levels - number of queues in the bundle.
 * \param [out] flags - flags specified when the queues were added
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_cosq_ipq_gport_get(
    int unit,
    int core,
    int base_voq,
    int is_mc,
    bcm_gport_t * physical_port,
    int *num_cos_levels,
    uint32 *flags)
{
    uint32 base_voq_quartet, is_composite, is_non_contiguous = FALSE;
    uint32 sysport = 0, queue_region;
    uint32 min_bundle_size = dnx_data_ipq.queues.min_bundle_size_get(unit);
    bcm_gport_t sysport_gport;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * get num cos levels from SW state
     */
    SHR_IF_ERR_EXIT(dnx_ipq_queue_num_cos_get(unit, core, base_voq, num_cos_levels));
    /*
     * Check if the Queue is mapped to a composite flow
     */
    base_voq_quartet = base_voq / min_bundle_size;
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_queue_quartet_to_flow_map_is_composite_get
                    (unit, core, base_voq_quartet, &is_composite));
    /*
     * Get the mapping from queue quartet to system port
     */
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_queue_quartet_to_sys_port_map_get(unit, core, base_voq_quartet, &sysport));

    /** FMQs system port is irrlevant (there is no real system port) */
    if (is_mc)
    {   /* if not mapped return 0 */
        *physical_port = 0;
    }
    else
    {
        /*
         * Get the physical port from the system port gport
         */
        BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, sysport);
        SHR_IF_ERR_EXIT(bcm_dnx_stk_sysport_gport_get(unit, sysport_gport, physical_port));
    }
    /*
     * determine flow attributes (non-contiguous)
     */
    queue_region = base_voq / dnx_data_ipq.regions.nof_queues_get(unit);
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_region_is_interdigitated_get(unit, queue_region, &is_non_contiguous));

    if (is_composite)
    {
        (*flags) |= BCM_COSQ_GPORT_COMPOSITE;
    }
    if (is_non_contiguous)
    {
        (*flags) |= BCM_COSQ_GPORT_NON_CONTIGUOUS_VOQ_CONNECTOR;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_cosq_ipq_ucast_gport_get(
    int unit,
    bcm_gport_t gport,
    bcm_gport_t * physical_port,
    int *num_cos_levels,
    uint32 *flags)
{

    int base_voq = 0, is_mc = FALSE, core = BCM_CORE_ALL;
    uint32 sysport = 0, dest_base_queue = 0, is_dest_valid = 0;
    uint32 min_bundle_size = dnx_data_ipq.queues.min_bundle_size_get(unit);
    uint32 base_voq_quartet;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ipq_ucast_gport_verify(unit, gport));

    *flags = 0;
    base_voq = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(gport);
    core = BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_GET(gport);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ipq_gport_get_verify(unit, core, base_voq));

    SHR_IF_ERR_EXIT(dnx_cosq_ipq_gport_get(unit, core, base_voq, is_mc, physical_port, num_cos_levels, flags));

    /*
     * Get the mapping from queue quartet to system port
     */
    base_voq_quartet = base_voq / min_bundle_size;
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_queue_quartet_to_sys_port_map_get(unit, core, base_voq_quartet, &sysport));
    /*
     * determine if this queue group is non-flow based
     */
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_system_port_to_voq_base_get
                    (unit, core, sysport, &dest_base_queue, &is_dest_valid));

    (*flags) |= BCM_COSQ_GPORT_UCAST_QUEUE_GROUP;
    /*
     * If dest_base_queue != base_voq, than we are in flow based mapping
     */
    if (dest_base_queue != base_voq)
    {
        (*flags) |= BCM_COSQ_GPORT_TM_FLOW_ID;
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_cosq_ipq_mcast_gport_get(
    int unit,
    bcm_gport_t gport,
    bcm_gport_t * physical_port,
    int *num_cos_levels,
    uint32 *flags)
{
    int base_voq = 0, is_mc = TRUE, core = BCM_CORE_ALL;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ipq_mcast_gport_verify(unit, gport));

    *flags = 0;
    base_voq = BCM_GPORT_MCAST_QUEUE_GROUP_QID_GET(gport);
    core = BCM_GPORT_MCAST_QUEUE_GROUP_CORE_GET(gport);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ipq_gport_get_verify(unit, core, base_voq));

    SHR_IF_ERR_EXIT(dnx_cosq_ipq_gport_get(unit, core, base_voq, is_mc, physical_port, num_cos_levels, flags));

    (*flags) |= BCM_COSQ_GPORT_MCAST_QUEUE_GROUP;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - check queue is empty before it is deleted. this check is doing several HW reads to the queue level,
 * to avoid a HW read in which the queue was momentarily empty. Its enough to get a non-empty read once to return not-empty status.
 * only if all reads from HW indicated the queue was empty, the status will return as empty.
 *
 * \param [in] unit -The unit number.
 * \param [in] core -device core id.
 * \param [in] base_voq - base queue to check if empty
 * \param [in] nof_iterations - how many HW reads to do.
 * \param [out] is_empty - return status of the queue
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_cosq_ipq_queue_is_empty_get(
    int unit,
    int core,
    int base_voq,
    int nof_iterations,
    int *is_empty)
{
    uint32 iter_idx;
    int core_idx = 0, queue_level = 0;
    dnx_ingress_congestion_voq_occupancy_info_t queue_info;
    SHR_FUNC_INIT_VARS(unit);

    (*is_empty) = 1;

    for (iter_idx = 0; iter_idx < nof_iterations; iter_idx++)
    {
        DNXCMN_CORES_ITER(unit, core, core_idx)
        {

            SHR_IF_ERR_EXIT(dnx_ingress_congestion_voq_occupancy_info_get(unit, core_idx, base_voq, &queue_info));

            queue_level = queue_info.size;
            /*
             * We want to make sure the queue is really empty,
             * and we didn't get a momentarily empty status while there is still traffic entering the queue.
             * So we read the queue level several times (even though the queue is empty).
             * its enough one read in which the queue is not empty to fail the empty check.
             */
            if (queue_level > 0)
            {
                break;
            }
        }
        if (queue_level > 0)
        {
            break;
        }
    }
    *is_empty = (queue_level > 0) ? FALSE : TRUE;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Unmap the base queue from system port and flow id.
 * Note that for Mesh there is a special handling, trying to make sure the queue is empty before unmapping.
 *
 * \param [in] unit -The unit number.
 * \param [in] core -device core id.
 * \param [in] queue_quartet - queue quartet id to unmap
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_cosq_ipq_queue_qrtt_unmap(
    int unit,
    int core,
    int queue_quartet)
{
    int base_voq, voq, is_empty;
    uint32 min_bundle_size = dnx_data_ipq.queues.min_bundle_size_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    base_voq = queue_quartet * min_bundle_size;

    /*
     * When in mesh mode, we need to protect the mapping of queue quartet to invalid system port.
     * if traffic tries to exit the VOQ when the queue is mapped to invalid system port, it can lead to VOQ stuck.
     * its only relevant to MESH, since in MESH the range of supported modids is limited, as oppose to CLOS.
     * If the system port is invalid, the modid it mapped to is also invalid in MESH systems.
     */
    if (dnx_data_fabric.general.connect_mode_get(unit) != DNX_FABRIC_CONNECT_MODE_FE)
    {
        for (voq = base_voq; voq < base_voq + min_bundle_size; voq++)
        {
            SHR_IF_ERR_EXIT(dnx_cosq_ipq_queue_is_empty_get
                            (unit, core, voq, DNX_COSQ_IPQ_QUEUE_EMPTY_ITERATIONS, &is_empty));
            if (!is_empty)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Queue %d is not empty. cannot remove it.", voq);
            }
        }
    }
    /*
     * Map the queue quartet to invalid sys port
     */
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_queue_quartet_to_sys_port_map_clear(unit, core, queue_quartet));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Common delete function for a queue bundle.
 * This function is called from both Unicast and Multicast contexts, and performing all the common work.
 *
 * \param [in] unit - The unit number.
 * \param [in] core - core id
 * \param [in] base_voq - base queue to delete. extracted from Gport given by the user.
 * \param [in] is_mc - are we deleting muticast or Unicast queue
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_cosq_ipq_gport_delete(
    int unit,
    int core,
    uint32 base_voq,
    int is_mc)
{
    int voq = 0, numq = 0, cosq, is_add = FALSE, default_compensation;
    uint32 sysport = 0, voq_quartet, base_voq_quartet, rate_class, sysport_base_voq, is_map_valid;
    uint32 min_bundle_size = dnx_data_ipq.queues.min_bundle_size_get(unit);
    bcm_cosq_ingress_queue_bundle_gport_config_t config;
    int core_idx;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&config, 0, sizeof(config));

    /*
     * get num cos levels from SW state
     */
    SHR_IF_ERR_EXIT(dnx_ipq_queue_num_cos_get(unit, core, base_voq, &numq));
    config.numq = numq;
    /*
     * Reset queue to flow mapping
     */
    for (voq = base_voq; voq < base_voq + numq; voq += min_bundle_size)
    {
        voq_quartet = voq / min_bundle_size;
        SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_queue_quartet_to_flow_map_clear(unit, core, voq_quartet));
    }
    if (!is_mc)
    {
        base_voq_quartet = base_voq / min_bundle_size;
        /*
         * Get queue to sysport mapping
         */
        SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_queue_quartet_to_sys_port_map_get(unit, core, base_voq_quartet, &sysport));

        /*
         * Check if the queue we are deleting is pointed to by this system port
         */
        SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_system_port_to_voq_base_get
                        (unit, core, sysport, &sysport_base_voq, &is_map_valid));

        if (base_voq == sysport_base_voq)
        {
            /*
             * disable configuration
             */
            SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_system_port_to_voq_base_map_clear(unit, core, sysport));
        }
    }
    /*
     * Delete queue group
     */
    for (voq = base_voq; voq < (base_voq + numq); voq += min_bundle_size)
    {
        voq_quartet = voq / min_bundle_size;
        SHR_IF_ERR_EXIT(dnx_cosq_ipq_queue_qrtt_unmap(unit, core, voq_quartet));
    }
    /*
     * free resources
     */
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_queue_deallocate(unit, core, base_voq));

    for (cosq = 0; cosq < numq; cosq++)
    {
        /*
         * Get the VOQ rate class
         */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_voq_rate_class_get(unit, core, base_voq + cosq, &rate_class));
        /*
         * Decrement the rate class ref counter since the VOQ is about to be removed.
         */
        DNXCMN_CORES_ITER(unit, core, core_idx)
        {
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.voq_rate_class.ref_count.dec(unit, core_idx, rate_class, 1));
        }

        config.queue_atrributes[cosq].rate_class = rate_class;
    }

    /*
     * Set local core ID
     */
    config.local_core_id = core;

    /*
     * update shared resource according to removed VOQs guaranteed
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_voq_guaranteed_set(unit, is_add, &config));

    /*
     * set the compensation back to default compensation.
     */
    default_compensation = dnx_data_ingr_congestion.voq.default_compensation_get(unit);
    for (cosq = 0; cosq < numq; cosq++)
    {
        SHR_IF_ERR_EXIT(dnx_cosq_ingress_compensation_voq_delta_set(unit, core, base_voq + cosq, default_compensation));
    }

    /** clear voq info */
    for (cosq = 0; cosq < numq; cosq++)
    {
        /** rate class, connection class */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_voq_info_clear(unit, core, base_voq + cosq));

        /** credit request profile */
        SHR_IF_ERR_EXIT(dnx_iqs_credit_request_profile_map_clear(unit, core, base_voq, cosq));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_ipq_gport_delete_verify(
    int unit,
    int core,
    uint32 base_voq)
{
    int voq = 0, flow_id, numq = 0;
    uint32 voq_quartet, flow_quartet;
    uint32 min_bundle_size = dnx_data_ipq.queues.min_bundle_size_get(unit);
    int is_allocated;
    uint8 is_asymm = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ipq_queue_is_allocated(unit, core, base_voq, &is_allocated));

    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Queue %d is not allocated", base_voq);
    }

    SHR_IF_ERR_EXIT(dnx_ipq_db.base_queue_is_asymm.bit_get(unit, base_voq, &is_asymm));
    if ((is_asymm == TRUE) && (core == BCM_CORE_ALL))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Queue %d is asymmetrical and can not be deleted using BCM_CORE_ALL", base_voq);
    }
    if ((is_asymm == FALSE) && (core != BCM_CORE_ALL))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Queue %d is symmetrical and can be deleted only using BCM_CORE_ALL", base_voq);
    }

    SHR_IF_ERR_EXIT(dnx_ipq_queue_num_cos_get(unit, core, base_voq, &numq));
    for (voq = base_voq; voq < base_voq + numq; voq += min_bundle_size)
    {
        voq_quartet = voq / min_bundle_size;
        SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_queue_quartet_to_flow_map_flow_id_get
                        (unit, core, voq_quartet, &flow_quartet));
        /*
         * Check if queue is disconnected
         */
        flow_id = flow_quartet * min_bundle_size;
        if (flow_id < DNX_COSQ_RESERVED_FLOW_START || flow_id >= DNX_COSQ_RESERVED_FLOW_NOF)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Queue %d is not disconnected", voq);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_ipq_mcast_gport_delete_verify(
    int unit,
    bcm_gport_t gport)
{
    uint32 base_fmq;
    int core;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ipq_mcast_gport_verify(unit, gport));

    base_fmq = BCM_GPORT_MCAST_QUEUE_GROUP_QID_GET(gport);
    core = BCM_GPORT_MCAST_QUEUE_GROUP_CORE_GET(gport);

    SHR_IF_ERR_EXIT(dnx_cosq_ipq_gport_delete_verify(unit, core, base_fmq));
exit:
    SHR_FUNC_EXIT;
}

/**
 * See ipq.h
 */
shr_error_e
dnx_cosq_ipq_mcast_gport_delete(
    int unit,
    bcm_gport_t gport)
{
    int core, base_voq = 0, is_mc = TRUE;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ipq_mcast_gport_delete_verify(unit, gport));

    /*
     * Get local core id
     */
    core = BCM_GPORT_MCAST_QUEUE_GROUP_CORE_GET(gport);

    /*
     * Determine base queue id
     */
    base_voq = BCM_GPORT_MCAST_QUEUE_GROUP_QID_GET(gport);
     /* coverity[negative_returns:FALSE]  */
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_gport_delete(unit, core, base_voq, is_mc));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for unicast gport delete.
 *
 * \param [in] unit -The unit number.
 * \param [in] gport - base voq Gport given by the user.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_cosq_ipq_ucast_gport_delete_verify(
    int unit,
    bcm_gport_t gport)
{
    int sysports_ref_counter = 0;
    uint32 base_voq, base_voq_quartet;
    uint32 min_bundle_size = dnx_data_ipq.queues.min_bundle_size_get(unit);
    int core, core_idx;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ipq_ucast_gport_verify(unit, gport));

    base_voq = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(gport);
    core = BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_GET(gport);

    SHR_IF_ERR_EXIT(dnx_cosq_ipq_gport_delete_verify(unit, core, base_voq));

    /** Check if there are still slave ports connected to this VOQ  */
    /** SwState array is with elements equal to NOF of Queue
     *  quarted.That is why SwState is acceessed with
     *  base_queue/min_queue_size */
    base_voq_quartet = base_voq / min_bundle_size;

    DNXCMN_CORES_ITER(unit, core, core_idx)
    {
        SHR_IF_ERR_EXIT(dnx_ipq_db.base_queues.
                        sys_port_ref_counter.get(unit, core_idx, base_voq_quartet, &sysports_ref_counter));

        if (sysports_ref_counter > 1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "VOQ %d still has slave sysports pointing to it, cannot delete.", base_voq);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See ipq.h
 */
shr_error_e
dnx_cosq_ipq_ucast_gport_delete(
    int unit,
    bcm_gport_t gport)
{
    int core, base_voq = 0, is_mc = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ipq_ucast_gport_delete_verify(unit, gport));

    /*
     * Get local core id
     */
    core = BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_GET(gport);

    /*
     * determine base queue id
     */
    base_voq = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(gport);

    SHR_IF_ERR_EXIT(dnx_cosq_ipq_gport_delete(unit, core, base_voq, is_mc));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_cosq_ipq_alloc_mngr_queue_qrtt_bitmap_create(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t * res_tag_bitmap,
    dnx_algo_res_create_data_t * create_data,
    void *extra_arguments,
    uint32 nof_elements,
    uint32 alloc_flags)
{
    uint32 region_nof_quartets = 0;
    dnx_algo_res_create_data_t res_tag_bitmap_create_info;
    resource_tag_bitmap_extra_arguments_create_info_t extra_create_info;
    SHR_FUNC_INIT_VARS(unit);

    region_nof_quartets = DNX_IPQ_NOF_QUEUES_IN_REGION(unit) / DNX_IPQ_MIN_BUNDLE_SIZE(unit);

    /*
     * Create the queue qrtt bitmap.
     * Tag will be used to specify multicast / interdigitated mode of the region.
     */
    sal_memset(&extra_create_info, 0, sizeof(extra_create_info));
    extra_create_info.grain_size = region_nof_quartets * DNX_COSQ_IPQ_BITMAP_ELEMS_PER_QUEUE_BUNDLE(unit);

    /*
     * Tag is actually the queue type. so max tag value is [nof_queue_types - 1]
     */
    extra_create_info.max_tag_value = NUM_DNX_ALGO_IPQ_QUEUE_TYPE - 1;

    sal_memset(&res_tag_bitmap_create_info, 0, sizeof(res_tag_bitmap_create_info));
    res_tag_bitmap_create_info.first_element = create_data->first_element;
    res_tag_bitmap_create_info.nof_elements = create_data->nof_elements;
    res_tag_bitmap_create_info.flags = RESOURCE_TAG_BITMAP_CREATE_FLAGS_NONE;

    SHR_IF_ERR_EXIT(resource_tag_bitmap_create
                    (unit, module_id, res_tag_bitmap, &res_tag_bitmap_create_info, &extra_create_info, nof_elements,
                     alloc_flags));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file.
 */
shr_error_e
dnx_cosq_ipq_alloc_mngr_queue_qrtt_bitmap_free(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int element)
{
    resource_tag_bitmap_alloc_info_t res_tag_bitmap_free_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Free the allocated bits
     */
    sal_memset(&res_tag_bitmap_free_info, 0, sizeof(resource_tag_bitmap_alloc_info_t));
    res_tag_bitmap_free_info.flags = RESOURCE_TAG_BITMAP_ALLOC_SPARSE;
    res_tag_bitmap_free_info.pattern = 1;
    res_tag_bitmap_free_info.length = 1;
    res_tag_bitmap_free_info.repeats = 1;

    SHR_IF_ERR_EXIT(resource_tag_bitmap_advanced_free
                    (unit, module_id, res_tag_bitmap, res_tag_bitmap_free_info, element));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file.
 */
shr_error_e
dnx_cosq_ipq_alloc_mngr_init(
    int unit)
{
    dnx_algo_res_create_data_t create_data;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&create_data, 0, sizeof(create_data));

    /*
     * Set bitmap info
     * In a device of N cores and K queue bundles, each queue bundle is represented by N bits and the total number of bits is K*N.
     */
    create_data.flags = DNX_ALGO_RES_CREATE_USE_ADVANCED_ALGORITHM;
    create_data.first_element = 0;
    create_data.nof_elements =
        (DNX_IPQ_NOF_QUEUES(unit) / DNX_IPQ_MIN_BUNDLE_SIZE(unit)) * DNX_COSQ_IPQ_BITMAP_ELEMS_PER_QUEUE_BUNDLE(unit);
    create_data.advanced_algorithm = DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_IQM;
    sal_strncpy(create_data.name, "Ipq alloc mngr", DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    /*
     * Create queues bitmap
     */
    SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.ipq_alloc_mngr.create(unit, &create_data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Get valid bundle range to alloc from, depending on queue type.
 */
static shr_error_e
dnx_cosq_ipq_queue_type_bundle_range_get(
    int unit,
    dnx_ipq_queue_type_e queue_type,
    int *start_bundle,
    int *end_bundle)
{
    uint32 invalid_queue = dnx_data_ipq.queues.invalid_queue_get(unit);
    int invalid_queue_is_in_range;
    int check_invalid_queue = FALSE;
    int max_mc_queue = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** The max_mc_queue is relevant to determine the range for unicast and multicast, multicast always comes first. */
    SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.fmq.max_multicast_queue.get(unit, &max_mc_queue));

    if (queue_type == DNX_ALGO_IPQ_QUEUE_TYPE_MULTICAST)
    {
        /*
         * Multicast Queues.
         */
        *start_bundle = 0;
        /** end quartet is the first invalid element */
        *end_bundle = (max_mc_queue + 1) / DNX_IPQ_MIN_BUNDLE_SIZE(unit);
        /*
         * we need to check if HW invalid queue is in range
         * only in the special case where all the queues are marked as MC
         */
        check_invalid_queue = (max_mc_queue + 1 == DNX_IPQ_NOF_QUEUES(unit)) ? TRUE : FALSE;
    }
    else
    {
        /*
         * Unicast queues always come after multicast queues
         */
        *start_bundle = (max_mc_queue + 1) / DNX_IPQ_MIN_BUNDLE_SIZE(unit);
        /** end quartet is the first invalid element */
        *end_bundle = DNX_IPQ_NOF_QUEUES(unit) / DNX_IPQ_MIN_BUNDLE_SIZE(unit);
        check_invalid_queue = TRUE;
    }

    if (check_invalid_queue)
    {
        /*
         * When the number of queues fill the number of bits exactly (for example 64K queues are exactly 16 bits)
         * the last queue is set to invalid queue (in HW).
         * we want to make sure not to allocate this queue (so we won't allocate the entire bundle).
         */
        invalid_queue_is_in_range = invalid_queue < DNX_IPQ_NOF_QUEUES(unit) ? TRUE : FALSE;
        if (invalid_queue_is_in_range)
        {
            *end_bundle -= 1;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file.
 */
shr_error_e
dnx_cosq_ipq_alloc_mngr_queue_qrtt_bitmap_allocate(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 flags,
    void *alloc_info,
    int *element)
{
    dnx_ipq_alloc_info_t *alloc_info_p = (dnx_ipq_alloc_info_t *) alloc_info;
    bcm_core_t core = alloc_info_p->core;
    dnx_ipq_queue_type_e queue_type = alloc_info_p->queue_type;
    resource_tag_bitmap_alloc_info_t res_tag_bitmap_alloc_info;
    int is_with_id = _SHR_IS_FLAG_SET(flags, DNX_ALGO_RES_ALLOCATE_WITH_ID);
    int nof_quartets_to_alloc = alloc_info_p->nof_queue_bundles_to_alloc;
    int start_element = alloc_info_p->start_element;
    int end_element = alloc_info_p->end_element;
    int tag;
    uint32 internal_flags = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Queue type Enum is used as tag.
     */
    tag = queue_type;

    if (is_with_id)
    {
        /*
         * Translate the allocation flags.
         * The input flags are of type DNX_ALGO_RES_ALLOCATE_*, but the sw_state_res_tag_bitmap use a
         * different set of flags.
         */
        internal_flags |= RESOURCE_TAG_BITMAP_ALLOC_WITH_ID;
    }
    else
    {
        /*
         * We ask the Alloc manager to allocate the queue only in the requested range.
         */
        internal_flags |= RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE;
    }

    /*
     * We ask the Alloc manager to verify we do not have cross region allocation.
     */
    internal_flags |= RESOURCE_TAG_BITMAP_ALLOC_CROSS_REGION_CHECK;

    /*
     * We ask the Alloc manager to search for a pattern
     */
    internal_flags |= (RESOURCE_TAG_BITMAP_ALLOC_SPARSE | RESOURCE_TAG_BITMAP_ALLOC_ALIGN);

    /*
     * Allocate bitmap element in a matching type region, or a new region
     */
    sal_memset(&res_tag_bitmap_alloc_info, 0, sizeof(resource_tag_bitmap_alloc_info_t));
    res_tag_bitmap_alloc_info.flags = internal_flags;
    res_tag_bitmap_alloc_info.tag = tag;
    res_tag_bitmap_alloc_info.range_start = start_element;
    res_tag_bitmap_alloc_info.range_end = end_element;
    res_tag_bitmap_alloc_info.pattern = DNX_COSQ_IPQ_QUEUE_BUNDLE_BITMAP_ALLOC_PATTERN(unit, core);
    res_tag_bitmap_alloc_info.length = DNX_COSQ_IPQ_BITMAP_ELEMS_PER_QUEUE_BUNDLE(unit);
    res_tag_bitmap_alloc_info.align = DNX_COSQ_IPQ_BITMAP_ELEMS_PER_QUEUE_BUNDLE(unit);
    res_tag_bitmap_alloc_info.repeats = nof_quartets_to_alloc;
    SHR_IF_ERR_EXIT(resource_tag_bitmap_alloc(unit, module_id, res_tag_bitmap, res_tag_bitmap_alloc_info, element));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_ipq_tc_to_multicast_prio_map_set_verify(
    int unit,
    bcm_cos_queue_t cosq,
    int mc_prio)
{
    SHR_FUNC_INIT_VARS(unit);

    if (cosq < BCM_COS_MIN || cosq > BCM_COS_MAX)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid cosq %d parameter, valid range is %d-%d", cosq, BCM_COS_MIN, BCM_COS_MAX);
    }

    if (mc_prio != BCM_COSQ_HIGH_PRIORITY && mc_prio != BCM_COSQ_LOW_PRIORITY)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid Arg parameter %d, valid values are BCM_COSQ_HIGH/LOW_PRIORITY", mc_prio);
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_ipq_tc_to_multicast_prio_map_get_verify(
    int unit,
    bcm_cos_queue_t cosq,
    int *mc_prio)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(mc_prio, _SHR_E_PARAM, "mc_prio");

    if (cosq < BCM_COS_MIN || cosq > BCM_COS_MAX)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid cosq %d parameter, valid range is %d-%d", cosq, BCM_COS_MIN, BCM_COS_MAX);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_cosq_ipq_tc_to_multicast_prio_map_set(
    int unit,
    bcm_cos_queue_t cosq,
    int mc_prio)
{
    dbal_enum_value_field_multicast_priority_e dbal_mc_prio = DBAL_NOF_ENUM_MULTICAST_PRIORITY_VALUES;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify arguments */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ipq_tc_to_multicast_prio_map_set_verify(unit, cosq, mc_prio));

    if (mc_prio == BCM_COSQ_LOW_PRIORITY)
    {
        dbal_mc_prio = DBAL_ENUM_FVAL_MULTICAST_PRIORITY_LOW_PRIORITY;
    }
    else if (mc_prio == BCM_COSQ_HIGH_PRIORITY)
    {
        dbal_mc_prio = DBAL_ENUM_FVAL_MULTICAST_PRIORITY_HIGH_PRIORITY;
    }
    /** Set mapping to HW */
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_tc_to_multicast_prio_map_set(unit, cosq, dbal_mc_prio));
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_cosq_ipq_tc_to_multicast_prio_map_get(
    int unit,
    bcm_cos_queue_t cosq,
    int *mc_prio)
{
    dbal_enum_value_field_multicast_priority_e dbal_mc_prio = DBAL_NOF_ENUM_MULTICAST_PRIORITY_VALUES;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify arguments */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ipq_tc_to_multicast_prio_map_get_verify(unit, cosq, mc_prio));

    /** Get mapping from HW */
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_tc_to_multicast_prio_map_get(unit, cosq, &dbal_mc_prio));

    if (dbal_mc_prio == DBAL_ENUM_FVAL_MULTICAST_PRIORITY_LOW_PRIORITY)
    {
        *mc_prio = BCM_COSQ_LOW_PRIORITY;
    }
    else if (dbal_mc_prio == DBAL_ENUM_FVAL_MULTICAST_PRIORITY_HIGH_PRIORITY)
    {
        *mc_prio = BCM_COSQ_HIGH_PRIORITY;
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * brief - verify function for dnx_cosq_ipq_default_invalid_queue_set
 */
static shr_error_e
dnx_cosq_ipq_invalid_destination_queue_set_verify(
    int unit,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_RANGE_VERIFY(enable, 0, 1, _SHR_E_PARAM, "Enable argument should be set to 0 or 1.\n");
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_cosq_ipq_invalid_destination_queue_set(
    int unit,
    bcm_gport_t voq_gport,
    bcm_cos_queue_t cosq,
    int enable)
{
    int core, queue_id, force_core_all = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    /** verify Gport and Cosq, and retrieve queue id */
    SHR_IF_ERR_EXIT(dnx_cosq_gport_to_voq_get(unit, voq_gport, cosq, force_core_all, &core, &queue_id));

    /** verify enable param*/
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ipq_invalid_destination_queue_set_verify(unit, enable));

    if (enable == 0)
    {
        /** set invalid destination queue id to invalid queue number */
        queue_id = dnx_data_ipq.queues.invalid_queue_get(unit);
    }
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_invalid_destination_queue_set(unit, core, queue_id));

exit:
    SHR_FUNC_EXIT;
}

/*
 * brief - verify function for dnx_cosq_ipq_default_invalid_queue_get
 */
static shr_error_e
dnx_cosq_ipq_invalid_destination_queue_get_verify(
    int unit,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(enable, _SHR_E_PARAM, "enable");
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_cosq_ipq_invalid_destination_queue_get(
    int unit,
    bcm_gport_t voq_gport,
    bcm_cos_queue_t cosq,
    int *enable)
{
    int core, core_idx, queue_id, force_core_all = FALSE;
    uint32 invalid_dest_queue_id;
    SHR_FUNC_INIT_VARS(unit);

    *enable = TRUE;

    /** verify Gport and Cosq, and retrieve queue id */
    SHR_IF_ERR_EXIT(dnx_cosq_gport_to_voq_get(unit, voq_gport, cosq, force_core_all, &core, &queue_id));

    /** verify enable param*/
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ipq_invalid_destination_queue_get_verify(unit, enable));

    DNXCMN_CORES_ITER(unit, core, core_idx)
    {
        SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_invalid_destination_queue_get(unit, core_idx, &invalid_dest_queue_id));

        if (queue_id != invalid_dest_queue_id)
        {
            *enable = FALSE;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Function to validate the inputs of
 * dnx_cosq_ipq_sysport_ingress_queue_map_enable_set/get
 */
static shr_error_e
dnx_cosq_ipq_sysport_ingress_queue_map_enable_validate(
    int unit,
    bcm_gport_t sysport_gport)
{
    uint32 system_port = 0;

    SHR_FUNC_INIT_VARS(unit);

    system_port = BCM_GPORT_SYSTEM_PORT_ID_GET(sysport_gport);
    if (system_port > dnx_data_device.general.invalid_system_port_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "system port is out of range. Max system port is %d",
                     dnx_data_device.general.invalid_system_port_get(unit));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_cosq_ipq_sysport_ingress_queue_map_enable_set(
    int unit,
    bcm_gport_t sysport_gport,
    int enable)
{
    uint32 system_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ipq_sysport_ingress_queue_map_enable_validate(unit, sysport_gport));

    system_port = BCM_GPORT_SYSTEM_PORT_ID_GET(sysport_gport);
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_system_port_to_voq_base_valid_set
                    (unit, BCM_CORE_ALL, system_port, enable ? 1 : 0));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_cosq_ipq_sysport_ingress_queue_map_enable_get(
    int unit,
    bcm_gport_t sysport_gport,
    int *enable)
{
    uint32 system_port;
    uint32 base_voq;
    uint32 is_valid;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ipq_sysport_ingress_queue_map_enable_validate(unit, sysport_gport));

    system_port = BCM_GPORT_SYSTEM_PORT_ID_GET(sysport_gport);
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_system_port_to_voq_base_get
                    (unit, BCM_CORE_ALL, system_port, &base_voq, &is_valid));

    *enable = is_valid;

exit:
    SHR_FUNC_EXIT;
}
