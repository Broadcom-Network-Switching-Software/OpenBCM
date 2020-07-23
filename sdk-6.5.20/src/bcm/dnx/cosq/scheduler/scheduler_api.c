/** \file scheduler_api.c
 * 
 *
 * scheduler API functions for DNX. \n
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

#include <shared/shrextend/shrextend_debug.h>
#include <bcm/cosq.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/cosq/scheduler/scheduler.h>
#include <bcm_int/dnx/cosq/ingress/fmq.h>

#include <soc/dnx/swstate/auto_generated/access/dnx_scheduler_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include "scheduler_device.h"
#include "scheduler.h"

/**
 * \brief -
 * verify key parameter of bcm_dnx_cosq_bandwidth_fabric_adjust_set/get API
 */
int
dnx_scheduler_api_bandwidth_fabric_adjust_key_verify(
    int unit,
    bcm_cosq_bandwidth_fabric_adjust_key_t * key)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key, _SHR_E_PARAM, "key");

    /** check key struct */
    if (key->rci_level > dnx_data_fabric.cgm.nof_rci_levels_get(unit) - 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "RCI level on key is out of range\n");
    }
    if (key->num_links > dnx_data_fabric.links.nof_links_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Number of links on key is out of range\n");
    }

    DNXCMN_CORE_VALIDATE(unit, key->core, TRUE);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * verify bcm_dnx_cosq_bandwidth_fabric_adjust_set/get parameters
 */
int
dnx_scheduler_api_bandwidth_fabric_adjust_verify(
    int unit,
    uint32 flags,
    bcm_cosq_bandwidth_fabric_adjust_key_t * key)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_api_bandwidth_fabric_adjust_key_verify(unit, key));

    SHR_MASK_VERIFY(flags, BCM_COSQ_BW_FABRIC_ADJUST_CONFIG_SHARED, _SHR_E_PARAM, "provided unsupported flags\n");

    if (!dnx_data_sch.device.feature_get(unit, dnx_data_sch_device_shared_drm) &&
        (flags & BCM_COSQ_BW_FABRIC_ADJUST_CONFIG_SHARED))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Shared credit generator is not supported on this device. BCM_COSQ_BW_FABRIC_ADJUST_CONFIG_SHARED cannot be specifed\n");

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * verify bcm_dnx_cosq_bandwidth_fabric_adjust_set/get parameters
 */
int
dnx_scheduler_api_bandwidth_fabric_adjust_set_verify(
    int unit,
    uint32 flags,
    bcm_cosq_bandwidth_fabric_adjust_key_t * key,
    uint32 bandwidth)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_api_bandwidth_fabric_adjust_verify(unit, flags, key));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief -
 * This API is used to configure the DRM table with the required bandwidth per rci/nof_links
 * This function configure the per core table,
 * Unless BCM_COSQ_BW_FABRIC_ADJUST_CONFIG_SHARED is used.
 * this API replace bcm_dnx_fabric_bandwidth_core_profile_set
 * and bcm_dnx_fabric_bandwidth_profile_set
 *
 * \param [in] unit -  Unit-ID
 * \param [in] flags - flags, currently only BCM_COSQ_BW_FABRIC_ADJUST_CONFIG_SHARED is used.
 * \param [in] key -  hold the rci level and nof links and core as the table key, look at bcm_cosq_bandwidth_fabric_adjust_key_t
 *                    BCM_CORE_ALL is supported
 * \param [in] bandwidth - the key combination bandwidth value, IN MBPS!
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * bcm_cosq_bandwidth_fabric_adjust_key_t
 *   * bcm_dnx_fabric_bandwidth_core_profile_set
 *   * bcm_dnx_fabric_bandwidth_profile_set
 */
int
bcm_dnx_cosq_bandwidth_fabric_adjust_set(
    int unit,
    uint32 flags,
    bcm_cosq_bandwidth_fabric_adjust_key_t * key,
    uint32 bandwidth)
{
    bcm_core_t core_i = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_api_bandwidth_fabric_adjust_set_verify(unit, flags, key, bandwidth));

    if (BCM_COSQ_BW_FABRIC_ADJUST_CONFIG_SHARED & flags)
    {
        /** update shared drm table */
        SHR_IF_ERR_EXIT(dnx_scheduler_device_shared_rate_entry_set(unit, key->rci_level, key->num_links, bandwidth));
    }
    else
    {
        DNXCMN_CORES_ITER(unit, key->core, core_i)
        {
            /** update drm table per core */
            SHR_IF_ERR_EXIT(dnx_scheduler_device_core_rate_entry_set(unit, core_i, key->rci_level, key->num_links,
                                                                     bandwidth));
        }
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

int
dnx_scheduler_api_bandwidth_fabric_adjust_get_verify(
    int unit,
    uint32 flags,
    bcm_cosq_bandwidth_fabric_adjust_key_t * key,
    uint32 *bandwidth)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bandwidth, _SHR_E_PARAM, "bandwidth");

    SHR_IF_ERR_EXIT(dnx_scheduler_api_bandwidth_fabric_adjust_verify(unit, flags, key));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This API is used to get the configured bandwidth in DRM table for specific rci/nof_links
 * This function get the configuration per core table,
 * Unless BCM_COSQ_BW_FABRIC_ADJUST_CONFIG_SHARED is used.
 * this API replace bcm_dnx_fabric_bandwidth_core_profile_get
 * and bcm_dnx_fabric_bandwidth_profile_get
 *
 * \param [in] unit -  Unit-ID
 * \param [in] flags - flags, currently only BCM_COSQ_BW_FABRIC_ADJUST_CONFIG_SHARED is used.
 * \param [in] key -  hold the rci level and nof links and core as the table key, look at bcm_cosq_bandwidth_fabric_adjust_key_t
 * \param [out] bandwidth - the key combination bandwidth value, IN MBPS!
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * bcm_cosq_bandwidth_fabric_adjust_key_t
 *   * bcm_dnx_fabric_bandwidth_core_profile_get
 *   * bcm_dnx_fabric_bandwidth_profile_get
 */
int
bcm_dnx_cosq_bandwidth_fabric_adjust_get(
    int unit,
    uint32 flags,
    bcm_cosq_bandwidth_fabric_adjust_key_t * key,
    uint32 *bandwidth)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_api_bandwidth_fabric_adjust_get_verify(unit, flags, key, bandwidth));

    if (key->core == BCM_CORE_ALL)
    {
        key->core = 0;
    }

    if (BCM_COSQ_BW_FABRIC_ADJUST_CONFIG_SHARED & flags)
    {    /** get from shared drm table */
        SHR_IF_ERR_EXIT(dnx_scheduler_device_shared_rate_entry_get(unit, key->rci_level, key->num_links, bandwidth));
    }
    else
    {
        /** get from drm table per core */
        SHR_IF_ERR_EXIT(dnx_scheduler_device_core_rate_entry_get(unit, key->core, key->rci_level, key->num_links,
                                                                 bandwidth));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Create VOQ connector
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] config - parameters of required VOQ connectoe
 *   \param [out] gport - gport for created VOQ connector
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_cosq_voq_connector_gport_add(
    int unit,
    bcm_cosq_voq_connector_gport_t * config,
    bcm_gport_t * gport)
{
    int core;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** get core id */
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_core_get(unit, ALGO_GPM_GPORT_CORE_F_NO_CORE_ALL, config->port, &core));

    SHR_IF_ERR_EXIT(dnx_scheduler_connector_gport_add
                    (unit, core, config->remote_modid, config->nof_remote_cores, 0, config->numq, config->flags,
                     gport));
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get VOQ connector parameters
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] gport - VOQ connector gport
 *   \param [out] config - parameters of the VOQ connector
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_cosq_voq_connector_gport_get(
    int unit,
    bcm_gport_t gport,
    bcm_cosq_voq_connector_gport_t * config)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_connector_gport_get(unit, gport, config));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - attach flow to a parent
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] sched_port - parent flow gport
 *   \param [in] input_port - child flow gport
 *   \param [in] cosq - child flow cosq
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \remark
 *   * The mode and weight of connection to the parent are determined by bcm_cosq_gport_sched_set
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_cosq_gport_attach(
    int unit,
    bcm_gport_t sched_port,
    bcm_gport_t input_port,
    bcm_cos_queue_t cosq)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    if (BCM_GPORT_IS_LOCAL(input_port))
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_attach_port_to_lag_scheduler(unit, sched_port, input_port, cosq));
    }
    else
    {
        /*
         * there are several gport types for input_port in this use case
         * they are validated inside the function
         */
        SHR_IF_ERR_EXIT(dnx_scheduler_gport_attach(unit, sched_port, input_port, cosq));
    }
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - detach flow from a parent
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] sched_port - parent flow gport
 *   \param [in] input_port - child flow gport
 *   \param [in] cosq - child flow cosq
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \remark
 *   * 
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_cosq_gport_detach(
    int unit,
    bcm_gport_t sched_port,
    bcm_gport_t input_port,
    bcm_cos_queue_t cosq)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    if (BCM_GPORT_IS_LOCAL(input_port))
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_detach_port_from_lag_scheduler(unit, sched_port, input_port, cosq));
    }
    else
    {
        /*
         * there are several gport types for input_port in this use case
         * they are validated inside the function
         */
        SHR_IF_ERR_EXIT(dnx_scheduler_gport_detach(unit, sched_port, input_port, cosq));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - get a parent of the flow
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] child_port - child flow gport
 *   \param [in] cos - child flow cosq
 *   \param [out] parent_port - parent flow gport
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \remark
 *   * 
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_cosq_gport_parent_get(
    int unit,
    bcm_gport_t child_port,
    bcm_cos_queue_t cos,
    bcm_gport_t * parent_port)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    if (BCM_GPORT_IS_LOCAL(child_port))
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_port_lag_scheduler_get(unit, child_port, cos, parent_port));
    }
    else
    {
        /*
         * there are several gport types for input_port in this use case
         * they are validated inside the function
         */
        SHR_IF_ERR_EXIT(dnx_scheduler_gport_parent_get(unit, child_port, cos, parent_port));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify parameters of slow_profile_set
 */
int
dnx_scheduler_api_slow_profile_set_verify(
    int unit,
    bcm_cosq_slow_level_t * slow_level,
    bcm_cosq_slow_profile_attributes_t * attr)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(slow_level, _SHR_E_PARAM, "slow_level");
    SHR_NULL_CHECK(attr, _SHR_E_PARAM, "attr");

    /** validate slow profile */
    SHR_RANGE_VERIFY(slow_level->profile, 1, dnx_data_sch.general.nof_slow_profiles_get(unit), _SHR_E_PARAM,
                     "Invalid slow profile %d\n", slow_level->profile);

    /** validate slow level */
    SHR_MAX_VERIFY(slow_level->level, dnx_data_sch.general.nof_slow_levels_get(unit) - 1, _SHR_E_PARAM,
                   "Invalid slow level %d\n", slow_level->level);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Configure rate of specified slow level
 */
int
bcm_dnx_cosq_slow_profile_set(
    int unit,
    bcm_cosq_slow_level_t * slow_level,
    bcm_cosq_slow_profile_attributes_t * attr)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_api_slow_profile_set_verify(unit, slow_level, attr));

    SHR_IF_ERR_EXIT(dnx_scheduler_slow_profile_rate_set
                    (unit, slow_level->core, slow_level->profile - 1, slow_level->level, attr->max_rate));
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   Get rate of specified slow level
 */
int
bcm_dnx_cosq_slow_profile_get(
    int unit,
    bcm_cosq_slow_level_t * slow_level,
    bcm_cosq_slow_profile_attributes_t * attr)
{

    int slow_rate_val;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_api_slow_profile_set_verify(unit, slow_level, attr));

    SHR_IF_ERR_EXIT(dnx_scheduler_slow_profile_rate_get
                    (unit, slow_level->core, slow_level->profile - 1, slow_level->level, &slow_rate_val));
    attr->max_rate = slow_rate_val;

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   Verify parameters of scheduler_gport_add
 */
static shr_error_e
dnx_scheduler_gport_add_verify(
    int unit,
    uint32 flags,
    bcm_cosq_scheduler_gport_params_t * params,
    bcm_gport_t * gport)
{
    uint32 valid_flags;
    int is_cl, is_hr;
    dnx_sch_element_se_type_e se_type;
    int nof_reserved_hrs;
    int fmq_mode;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(params, _SHR_E_PARAM, "params");
    SHR_NULL_CHECK(gport, _SHR_E_PARAM, "gport");

    /** Verify all flags are valid */
    valid_flags =
        BCM_COSQ_SCHEDULER_GPORT_WITH_ID | BCM_COSQ_SCHEDULER_GPORT_REPLACE | BCM_COSQ_SCHEDULER_GPORT_FMQ_RESERVE;
    SHR_MASK_VERIFY(flags, valid_flags, _SHR_E_PARAM, "provided unsupported flags\n");

    /** Verify WITH_ID and REPLACE are not specified together */
    SHR_NOF_SET_BITS_IN_RANGE_VERIFY(flags, BCM_COSQ_SCHEDULER_GPORT_WITH_ID | BCM_COSQ_SCHEDULER_GPORT_REPLACE, 0, 1,
                                     _SHR_E_PARAM, "cannot specify both REPLACE and WITH_ID\n");

    /** Verify parameters */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_element_type_get(unit, params->type, &se_type));
    is_cl = (se_type == DNX_SCH_ELEMENT_SE_TYPE_CL);
    is_hr = (se_type == DNX_SCH_ELEMENT_SE_TYPE_HR);

    switch (params->cl_wfq_mode)
    {
        case bcmCosqSchedulerGportClWfqModeNone:
            if (is_cl && params->type != bcmCosqSchedulerGportTypeClass4Sp)
            {
                /** CL with WFQ */
                SHR_ERR_EXIT(_SHR_E_PARAM, "cl_wfq_mode mode must be specified for type %d\n", params->type);
            }
            break;

        case bcmCosqSchedulerGportClWfqModeDiscrete:
        case bcmCosqSchedulerGportClWfqModeClass:
            if (!is_cl || params->type == bcmCosqSchedulerGportTypeClass4Sp)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "cl_wfq_mode mode cannot be specified for type %d\n", params->type);
            }
            break;
        case bcmCosqSchedulerGportClWfqModeIndependent:
        case bcmCosqSchedulerGportClWfqModeIndependentProportional:
            if (!is_cl || params->type == bcmCosqSchedulerGportTypeClass4Sp
                || params->type == bcmCosqSchedulerGportTypeClass3SpWfq)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "cl_wfq_mode independent mode cannot be specified for type %d\n",
                             params->type);
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected CL WFQ mode %d\n", params->cl_wfq_mode);
            break;
    }

    switch (params->mode)
    {
        case bcmCosqSchedulerGportModeEnhancedClLowPrioFQ:
        case bcmCosqSchedulerGportModeEnhancedClHighPrioFQ:
        case bcmCosqSchedulerGportModeSharedDual:
        case bcmCosqSchedulerGportModeSharedQuad:
        case bcmCosqSchedulerGportModeSharedOcta:
            if (!is_cl)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "mode %d can be specified for CL elements only\n", params->mode);

            }
            break;
        case bcmCosqSchedulerGportModeComposite:
            if (dnx_data_sch.flow.feature_get(unit, dnx_data_sch_flow_cl_fq_cl_fq_quartet)
                && se_type == DNX_SCH_ELEMENT_SE_TYPE_FQ)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Composite Cl is not supported on this device\n");
            }
            break;
        default:
            /** nothing to check */
            break;
    }

    /** validate flow (if specified by user) */
    if (flags & BCM_COSQ_SCHEDULER_GPORT_WITH_ID)
    {
        if (!(BCM_GPORT_IS_SCHEDULER(*gport)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid gport (0x%x) specified when WITH_ID flag presents\n", (*gport));
        }

    }

    /** validate core */
    DNXCMN_CORE_VALIDATE(unit, params->core, FALSE);

    if (flags & BCM_COSQ_SCHEDULER_GPORT_FMQ_RESERVE)
    {
        if (!is_hr)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "BCM_COSQ_SCHEDULER_GPORT_FMQ_RESERVE flag can be specified for HR allocation only\n");
        }

        SHR_IF_ERR_EXIT(dnx_scheduler_db.fmq.nof_reserved_hr.get(unit, params->core, &nof_reserved_hrs));

        if (nof_reserved_hrs >= dnx_data_sch.general.nof_fmq_class_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "BCM_COSQ_SCHEDULER_GPORT_FMQ_RESERVE allocation can be done at most %d times on core %d\n",
                         dnx_data_sch.general.nof_fmq_class_get(unit), params->core);

        }

        SHR_IF_ERR_EXIT(dnx_cosq_fmq_scheduler_mode_get(unit, &fmq_mode));
        if (fmq_mode != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "BCM_COSQ_SCHEDULER_GPORT_FMQ_RESERVE flag can be used in simple FMQ mode only\n");

        }

    }

    SHR_NOF_SET_BITS_IN_RANGE_VERIFY(flags, BCM_COSQ_SCHEDULER_GPORT_FMQ_RESERVE | BCM_COSQ_SCHEDULER_GPORT_REPLACE,
                                     0, 1, _SHR_E_PARAM,
                                     "BCM_COSQ_SCHEDULER_GPORT_FMQ_RESERVE and BCM_COSQ_SCHEDULER_GPORT_REPLACE cannot be specified together\n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Allocate and configure new scheduling element
 *
 * \param [in] unit -  Unit-ID
 * \param [in] flags - spefific flags 
 * \param [in] params - specification of the requested element
 * \param [out] gport - result gport

 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_cosq_scheduler_gport_add(
    int unit,
    uint32 flags,
    bcm_cosq_scheduler_gport_params_t * params,
    bcm_gport_t * gport)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_gport_add_verify(unit, flags, params, gport));

    if (flags & BCM_COSQ_SCHEDULER_GPORT_REPLACE)
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_element_gport_replace(unit, flags, params, gport));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_element_gport_add(unit, flags, params, gport));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify bcm_dnx_cosq_scheduler_gport_get parameters
 */
static int
dnx_scheduler_gport_get_verify(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    bcm_cosq_scheduler_gport_params_t * params)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(params, _SHR_E_PARAM, "params");

    SHR_MASK_VERIFY(flags, 0, _SHR_E_PARAM, "no flags supported\n");

    if (!BCM_GPORT_IS_SCHEDULER(gport) && !BCM_COSQ_GPORT_IS_SHARED_SHAPER_ELEM(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Scheduler or shared shaper element gport is expected\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get scheduling element gport parameters
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] flags - non currently supported
 *   \param [in] gport -  scheduler element gport to retrieve the parameters for
 *   \param [out] params -  gport parameters
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * 
 * \see
 *   * None
 */
int
bcm_dnx_cosq_scheduler_gport_get(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    bcm_cosq_scheduler_gport_params_t * params)
{

    int flow_id;
    bcm_core_t core;
    bcm_gport_t actual_sched_gport;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_gport_get_verify(unit, flags, gport, params));

    SHR_IF_ERR_EXIT(dnx_scheduler_gport_actual_sched_gport_get(unit, gport, FALSE, &actual_sched_gport));

    /** get flow id + core */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_core_base_flow_id_get(unit, actual_sched_gport, 0, &core, &flow_id));

    /** actual implementation */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_get(unit, core, flow_id, params));

exit:
    SHR_FUNC_EXIT;
}
