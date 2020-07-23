/** \file cosq_ingress_api.c
 * 
 *
 * Ingress congestion API implementation for DNX. \n
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
#include <shared/shrextend/shrextend_error.h>
#include <bcm/cosq.h>
#include <bcm_int/dnx/cosq/cosq.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/cosq/ingress/cosq_ingress.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>

#include <soc/types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_congestion.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_ingress_congestion_access.h>

#include "system_red.h"
#include "ingress_congestion.h"
#include "ingress_congestion_dbal.h"
#include <bcm_int/dnx/cosq/ingress/ipq.h>
#include "voq_rate_class.h"
#include "vsq_rate_class.h"

/* Discard info limitation */
#define DNX_COSQ_DISCARD_SYSTEM_RED_FROP_PROB   (0xffff)

static int
dnx_cosq_gport_discard_set_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_gport_discard_t * discard)
{
    uint32 allowed_flags;
    uint32 max_drop_probability, max_discard_gain;
    int is_voq, is_vsq_allowed = TRUE;
    uint32 all_colors_flags = (BCM_COSQ_DISCARD_COLOR_GREEN | BCM_COSQ_DISCARD_COLOR_YELLOW |
                               BCM_COSQ_DISCARD_COLOR_RED | BCM_COSQ_DISCARD_COLOR_BLACK | BCM_COSQ_DISCARD_COLOR_ALL);

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_rate_class_gport_verify(unit, gport, cosq, is_vsq_allowed));

    SHR_NULL_CHECK(discard, _SHR_E_PARAM, "discard");

    is_voq = BCM_GPORT_IS_PROFILE(gport);

    if (discard->flags & BCM_COSQ_DISCARD_SYSTEM)
    {
        allowed_flags =
            (BCM_COSQ_DISCARD_SYSTEM | BCM_COSQ_DISCARD_ENABLE | all_colors_flags | BCM_COSQ_DISCARD_PROBABILITY1 |
             BCM_COSQ_DISCARD_PROBABILITY2);

        max_drop_probability = DNX_COSQ_DISCARD_SYSTEM_RED_FROP_PROB;
        max_discard_gain = 0x0;
    }
    else
    {

        allowed_flags =
            (BCM_COSQ_DISCARD_ENABLE | all_colors_flags | BCM_COSQ_DISCARD_BYTES | BCM_COSQ_DISCARD_GRANULARITY_1K);

        if (is_voq)
        {
            allowed_flags |= BCM_COSQ_DISCARD_MARK_CONGESTION;
        }
        else if (BCM_COSQ_GPORT_IS_VSQ_SRC_PORT(gport))
        {
            allowed_flags |= (BCM_COSQ_DISCARD_POOL_0 | BCM_COSQ_DISCARD_POOL_1);
        }

        max_drop_probability = 100;
        max_discard_gain = dnx_data_ingr_congestion.info.wred_max_gain_get(unit);
    }

    SHR_MASK_VERIFY(discard->flags, allowed_flags, _SHR_E_PARAM, "Unsupported flag provided.\n");

    if (discard->flags & BCM_COSQ_DISCARD_MARK_CONGESTION)
    {
        /** Validate ECN threshold */
        SHR_MASK_VERIFY(discard->flags & all_colors_flags, 0, _SHR_E_PARAM,
                        "Color flag cannot be specified together with BCM_COSQ_DISCARD_MARK_CONGESTION.\n");

        if (discard->ecn_thresh < 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "ECN threshold %d is invalid\n", discard->ecn_thresh);
        }
        /** check the top bound of ecn_thresh only in 1K granularity, since in bytes the "int" top limit of 2G is sufficient */
        if (discard->flags & BCM_COSQ_DISCARD_GRANULARITY_1K)
        {
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_threshold_verify
                            (unit, DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES, "ECN Threshold",
                             KILO(discard->ecn_thresh)));
        }

    }

    SHR_RANGE_VERIFY(discard->drop_probability, 0, max_drop_probability, _SHR_E_PARAM,
                     "drop probablity %d is out of range\n", discard->drop_probability);

    SHR_RANGE_VERIFY(discard->gain, 0, max_discard_gain, _SHR_E_PARAM, "gain %d is out of range\n", discard->gain);

    if (discard->min_thresh > discard->max_thresh)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Min threshold %d is over Max threshold %d\n",
                     discard->min_thresh, discard->max_thresh);
    }
    /** check the top bound of max_thresh only in 1K granularity, since in bytes the "int" top limit of 2G is sufficient */
    if (discard->flags & BCM_COSQ_DISCARD_GRANULARITY_1K)
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_threshold_verify
                        (unit, DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES, "Max Threshold",
                         KILO(discard->max_thresh)));
    }

    if (discard->min_thresh < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Min threshold %d is invalid\n", discard->min_thresh);
    }

    if (discard->max_thresh < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Max threshold %d is invalid\n", discard->max_thresh);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Configure WRED drop parameters
 *
 * \param [in] unit -  Unit-ID
 * \param [in] gport - VOQ rate class or VSQ gport
 * \param [in] cosq - unused. must be 0 or -1
 * \param [in] discard - WRED parameters
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_cosq_gport_discard_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_gport_discard_t * discard)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_gport_discard_set_verify(unit, gport, cosq, discard));

    if (BCM_GPORT_IS_PROFILE(gport))
    {
        if (discard->flags & BCM_COSQ_DISCARD_SYSTEM)
        {
            SHR_IF_ERR_EXIT(dnx_system_red_discard_set(unit, gport, cosq, discard));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_voq_discard_set(unit, gport, cosq, discard));
        }
    }
    if (BCM_COSQ_GPORT_IS_VSQ(gport))
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_discard_set(unit, gport, cosq, discard));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

static int
dnx_cosq_gport_discard_get_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_gport_discard_t * discard)
{
    uint32 allowed_flags, is_vsq_allowed = TRUE;

    uint32 all_colors = (BCM_COSQ_DISCARD_COLOR_GREEN | BCM_COSQ_DISCARD_COLOR_YELLOW | BCM_COSQ_DISCARD_COLOR_RED |
                         BCM_COSQ_DISCARD_COLOR_BLACK);

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(discard, _SHR_E_PARAM, "discard");

    SHR_IF_ERR_EXIT(dnx_cosq_rate_class_gport_verify(unit, gport, cosq, is_vsq_allowed));

    allowed_flags = (BCM_COSQ_DISCARD_ENABLE | all_colors);

    if (discard->flags & BCM_COSQ_DISCARD_SYSTEM)
    {
        /** System RED */

        allowed_flags |= (BCM_COSQ_DISCARD_PROBABILITY1 | BCM_COSQ_DISCARD_PROBABILITY2 | BCM_COSQ_DISCARD_SYSTEM);
    }
    else
    {
        allowed_flags |= BCM_COSQ_DISCARD_BYTES | BCM_COSQ_DISCARD_GRANULARITY_1K;

        if (BCM_GPORT_IS_PROFILE(gport))
        {
            allowed_flags |= BCM_COSQ_DISCARD_MARK_CONGESTION;
        }
        else if (BCM_COSQ_GPORT_IS_VSQ_SRC_PORT(gport))
        {
            /** Source port VSQ */
            allowed_flags |= (BCM_COSQ_DISCARD_POOL_0 | BCM_COSQ_DISCARD_POOL_1);
        }

        /** Verify exactly one color (or ECN) is specified */
        SHR_NOF_SET_BITS_IN_RANGE_VERIFY(discard->flags, (all_colors | BCM_COSQ_DISCARD_MARK_CONGESTION), 1, 1,
                                         _SHR_E_PARAM,
                                         "Exactly one color flag or BCM_COSQ_DISCARD_MARK_CONGESTION should be specified\n");

        /** Verify no more than one pool is specified */
        SHR_NOF_SET_BITS_IN_RANGE_VERIFY(discard->flags, (BCM_COSQ_DISCARD_POOL_0 | BCM_COSQ_DISCARD_POOL_1), 0, 1,
                                         _SHR_E_PARAM, "Only one pool flag can be specified\n");
    }

    SHR_MASK_VERIFY(discard->flags, allowed_flags, _SHR_E_PARAM, "Unsupported flag provided.\n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Obtain WRED drop parameters
 *
 * \param [in] unit -  Unit-ID
 * \param [in] gport - VOQ rate class or VSQ gport
 * \param [in] cosq - unused. must be 0 or -1
 * \param [in] discard - WRED parameters
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_cosq_gport_discard_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_gport_discard_t * discard)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_gport_discard_get_verify(unit, gport, cosq, discard));

    if (BCM_GPORT_IS_PROFILE(gport))
    {
        if (discard->flags & BCM_COSQ_DISCARD_SYSTEM)
        {
            SHR_IF_ERR_EXIT(dnx_system_red_discard_get(unit, gport, cosq, discard));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_voq_discard_get(unit, gport, cosq, discard));
        }
    }

    if (BCM_COSQ_GPORT_IS_VSQ(gport))
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_discard_get(unit, gport, cosq, discard));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * brief -
 * verify parameters of bcm_dnx_cosq_gport_vsq_add / bcm_dnx_cosq_gport_vsq_delete
 */
static int
dnx_cosq_gport_vsq_add_verify(
    int unit,
    bcm_gport_t vsq,
    bcm_gport_t queue,
    bcm_cos_queue_t cosq)
{
    int core;
    int vsq_category;
    int queue_id;
    int is_multicast;
    int force_core_all = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    if (!(BCM_COSQ_GPORT_IS_VSQ_CTCC(vsq) || BCM_COSQ_GPORT_IS_VSQ_CTTC(vsq)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Expected Category-ConnectionClass  or Category-TrafficClass gport\n");
    }

    if (cosq < 0 || cosq >= DNX_COSQ_NOF_TC)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid cosq %d parameter, range 0-7\n", cosq);
    }

    SHR_IF_ERR_EXIT(dnx_cosq_gport_to_voq_get(unit, queue, cosq, force_core_all, &core, &queue_id));

    /** check VSQ core */
    core = BCM_COSQ_GPORT_VSQ_CORE_ID_GET(vsq);
    if (core != BCM_CORE_ALL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "VSQ gport(0x%08x) must be defined with CORE_ALL (core = %d)\n", vsq, core);
    }

    vsq_category = BCM_COSQ_GPORT_VSQ_CT_GET(vsq);

    SHR_IF_ERR_EXIT(dnx_ipq_queue_is_multicast(unit, queue_id, &is_multicast));

    if (!((vsq_category == 0 && is_multicast) || (vsq_category == 2 && !is_multicast)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "vsq 0x%x, voq 0x%x: no match between voq queue category and vsq gport category\n", vsq, queue);

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Assign VOQ to VSQ
 *
 * \param [in] unit -  Unit-ID
 * \param [in] vsq - CTTC or CTCC vsq gport
 * \param [in] queue - VOQ gport
 * \param [in] cosq - index of queue inside a bundle
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_cosq_gport_vsq_add(
    int unit,
    bcm_gport_t vsq,
    bcm_gport_t queue,
    bcm_cos_queue_t cosq)
{
    int queue_id;
    int vsq_connection_cls, vsq_traffic_cls;
    int core;

    int force_core_all = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Validate parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_gport_vsq_add_verify(unit, vsq, queue, cosq));

    SHR_IF_ERR_EXIT(dnx_cosq_gport_to_voq_get(unit, queue, cosq, force_core_all, &core, &queue_id));

    /** Set new mapping */
    if (BCM_COSQ_GPORT_IS_VSQ_CTCC(vsq))
    {
        vsq_connection_cls = BCM_COSQ_GPORT_VSQ_CC_GET(vsq);
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_voq_connection_class_set(unit, core, queue_id, vsq_connection_cls));
    }
    if (BCM_COSQ_GPORT_IS_VSQ_CTTC(vsq))
    {
        vsq_traffic_cls = BCM_COSQ_GPORT_VSQ_TC_GET(vsq);
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_voq_traffic_class_set(unit, core, queue_id, vsq_traffic_cls));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * De-assign VOQ from VSQ
 *
 * \param [in] unit -  Unit-ID
 * \param [in] vsq - CTTC or CTCC vsq gport
 * \param [in] queue - VOQ gport
 * \param [in] cosq - index of queue inside a bundle
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_cosq_gport_vsq_delete(
    int unit,
    bcm_gport_t vsq,
    bcm_gport_t queue,
    bcm_cos_queue_t cosq)
{
    int queue_id;
    int core;

    int force_core_all = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Validate parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_gport_vsq_add_verify(unit, vsq, queue, cosq));

    SHR_IF_ERR_EXIT(dnx_cosq_gport_to_voq_get(unit, queue, cosq, force_core_all, &core, &queue_id));

    /** Delete by mapping queue to default traffic class/connection class, which is 0 */
    if (BCM_COSQ_GPORT_IS_VSQ_CTCC(vsq))
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_voq_connection_class_set(unit, core, queue_id, 0));
    }
    if (BCM_COSQ_GPORT_IS_VSQ_CTTC(vsq))
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_voq_traffic_class_set(unit, core, queue_id, 0));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * brief -
 * verify parameters of bcm_dnx_cosq_gport_vsq_create
 */
static int
dnx_cosq_gport_vsq_create_verify(
    int unit,
    bcm_cosq_vsq_info_t * vsq_info,
    bcm_gport_t * vsq_gport)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(vsq_info, _SHR_E_PARAM, "vsq_info");
    SHR_NULL_CHECK(vsq_gport, _SHR_E_PARAM, "vsq_gport");

    /*
     * Note, vsq_info->flags is called "flags"
     * but it behaves like an enum
     */
    if (vsq_info->flags == BCM_COSQ_VSQ_LLFC || vsq_info->flags == BCM_COSQ_VSQ_PFC)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Invalid vsq flags 0x%x: BCM_COSQ_VSQ_LLFC and BCM_COSQ_VSQ_PFC are not supported on this device. Use bcm_cosq_src_vsqs_gport_add API\n",
                     vsq_info->flags);
    }

    /** valid values for flags are between BCM_COSQ_VSQ_GL and BCM_COSQ_VSQ_PP */
    SHR_RANGE_VERIFY(vsq_info->flags, BCM_COSQ_VSQ_GL, BCM_COSQ_VSQ_PP, _SHR_E_PARAM, "invalid vsq flag 0x%x\n",
                     vsq_info->flags);

    /** Validate category range */
    if ((vsq_info->flags == BCM_COSQ_VSQ_CT ||
         vsq_info->flags == BCM_COSQ_VSQ_CTTC || vsq_info->flags == BCM_COSQ_VSQ_CTCC))
    {
        SHR_RANGE_VERIFY(vsq_info->ct_id, 0,
                         dnx_data_ingr_congestion.vsq.info_get(unit, DNX_INGRESS_CONGESTION_VSQ_GROUP_CTGRY)->nof - 1,
                         _SHR_E_PARAM, "invalid category %d\n", vsq_info->ct_id);

        if (vsq_info->ct_id != 0 && vsq_info->ct_id != 2)
        {
            /*
             * partition into categories is currently done hardcoded as following:
             * * multicast - category 0
             * * unicast   - category 2
             * If/when user defined partition into category is supported, this check should be removed
             */

            SHR_ERR_EXIT(_SHR_E_PARAM, "ct_id %d is not supported\n", vsq_info->ct_id);
        }
    }

    DNXCMN_CORE_VALIDATE(unit, vsq_info->core_id, TRUE);

    if (vsq_info->flags == BCM_COSQ_VSQ_CT ||
        vsq_info->flags == BCM_COSQ_VSQ_CTTC || vsq_info->flags == BCM_COSQ_VSQ_CTCC)
    {
        SHR_VAL_VERIFY(vsq_info->core_id, BCM_CORE_ALL, _SHR_E_PARAM, "BCM_CORE_ALL is expected\n");

    }

    /** Validate connection class range */
    if (vsq_info->flags == BCM_COSQ_VSQ_CTCC)
    {
        SHR_RANGE_VERIFY(vsq_info->cc_id, 0,
                         dnx_data_ingr_congestion.vsq.connection_class_nof_get(unit) - 1,
                         _SHR_E_PARAM, "invalid connection class %d\n", vsq_info->cc_id);
    }

    /** Validate traffic class range */
    if (vsq_info->flags == BCM_COSQ_VSQ_CTTC)
    {
        SHR_RANGE_VERIFY(vsq_info->traffic_class, 0, DNX_COSQ_NOF_TC - 1,
                         _SHR_E_PARAM, "traffic class %d is out of range\n", vsq_info->traffic_class);
    }
    /** Validate PP range */
    if (vsq_info->flags == BCM_COSQ_VSQ_PP)
    {
        SHR_RANGE_VERIFY(vsq_info->pp_vsq_id, 0,
                         dnx_data_ingr_congestion.vsq.info_get(unit,
                                                               DNX_INGRESS_CONGESTION_VSQ_GROUP_STTSTCS_TAG)->nof - 1,
                         _SHR_E_PARAM, "invalid pp vsq id %d ", vsq_info->pp_vsq_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Create VSQ
 *
 * \param [in] unit -  Unit-ID
 * \param [in] vsq_info - parameters for VSQ creation
 * \param [out] vsq_gport - VSQ gport
 * \return
 *   See shr_error_e
 * \remark
 *   * this API is used for VSQ A-D and global VSQ only
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_cosq_gport_vsq_create(
    int unit,
    bcm_cosq_vsq_info_t * vsq_info,
    bcm_gport_t * vsq_gport)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_gport_vsq_create_verify(unit, vsq_info, vsq_gport));

    /** Set VSQ gport */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_gport_set(unit, vsq_info, vsq_gport));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * brief -
 * verify parameters of bcm_cosq_src_vsqs_gport_add
 */
static int
dnx_cosq_src_vsqs_gport_add_verify(
    int unit,
    bcm_gport_t port,
    bcm_cosq_src_vsqs_gport_config_t * config,
    bcm_gport_t * src_port_vsq_gport,
    bcm_gport_t * pg_base_vsq_gport)
{

    bcm_port_t src_port;
    dnx_algo_port_info_s port_info;
    uint32 found_tc_mapping[DNX_COSQ_NOF_TC];
    int tc, pg;
    int lossless_pool = -1;
    bcm_core_t core_id;
    int enable;
    int is_over_fabric;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(src_port_vsq_gport, _SHR_E_PARAM, "src_port_vsq_gport");
    SHR_NULL_CHECK(pg_base_vsq_gport, _SHR_E_PARAM, "pg_base_vsq_gport");
    SHR_NULL_CHECK(config, _SHR_E_PARAM, "config");

    SHR_RANGE_VERIFY(config->numq, 0, BCM_COSQ_VSQ_NOF_PG,
                     _SHR_E_PARAM, "The numq %d value is out of range, valid range is 0-%d\n",
                     config->numq, BCM_COSQ_VSQ_NOF_PG);

    if (config->flags & BCM_COSQ_GPORT_WITH_ID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_COSQ_GPORT_WITH_ID is not supported on this device\n");
    }

    SHR_MASK_VERIFY(config->flags, 0, _SHR_E_PARAM, "No flag supported.\n");

    if (BCM_GPORT_IS_LOCAL(port))
    {
        src_port = BCM_GPORT_LOCAL_GET(port);
    }
    else
    {
        src_port = port;
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, src_port));

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, src_port, &port_info));

    if (!DNX_ALGO_PORT_TYPE_IS_TM(unit, port_info) || DNX_ALGO_PORT_TYPE_IS_ERP(unit, port_info))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Port type of port %d is not supported by this API", src_port);
    }
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, FALSE, FALSE))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, src_port, &is_over_fabric));
        if (is_over_fabric && config->numq != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "PG VSQ cannot be created for ILKN over fabric port. config.numq should be 0 (%d)\n",
                         config->numq);

        }
    }

    if (!DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, FALSE, TRUE /* STIF */ , FALSE, FALSE) && config->numq != 0 &&
        dnx_data_ingr_congestion.vsq.non_nif_vsq_f_nof_get(unit) == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "PG VSQ cannot be created for non NIF port on this device. config.numq should be 0 (%d)\n",
                     config->numq);
    }

    if (config->numq > 0)
    {
        sal_memset(found_tc_mapping, 0xffffffff, sizeof(uint32[DNX_COSQ_NOF_TC]));
        for (pg = 0; pg < config->numq; pg++)
        {
            for (tc = 0; tc < DNX_COSQ_NOF_TC; tc++)
            {
                if (config->pg_attributes[pg].cosq_bitmap & (0x1 << tc))
                {
                    if (found_tc_mapping[tc] != 0xffffffff)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "Invalid cosq bitmap configuration. TC %d, Cannot be mapped to %d, and to %d.\n",
                                     tc, found_tc_mapping[tc], pg);
                    }
                    found_tc_mapping[tc] = pg;
                }
            }
        }
        for (tc = 0; tc < DNX_COSQ_NOF_TC; tc++)
        {
            if (found_tc_mapping[tc] == 0xffffffff)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid cosq bitmap configuration. TC %d is not mapped.\n", tc);
            }
        }

        /** Get the relevant core ID from the src_port*/
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, src_port, &core_id));

        SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.port_based_vsq.lossless_pool_id.get(unit, core_id, &lossless_pool));

        for (pg = 0; pg < config->numq; pg++)
        {
            SHR_MAX_VERIFY(config->pg_attributes[pg].pool_id, dnx_data_ingr_congestion.vsq.pool_nof_get(unit) - 1,
                           _SHR_E_PARAM, "Invalid pool ID, PG offset %d, pool ID %d.\n",
                           pg, config->pg_attributes[pg].pool_id);

            if (config->pg_attributes[pg].is_lossles != 0 && config->pg_attributes[pg].pool_id != lossless_pool)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "PG offset %d: can't create lossless VSQ on pool %d. Set pool %d to be lossless first\n",
                             pg, config->pg_attributes[pg].pool_id, config->pg_attributes[pg].pool_id);
            }

        }
    }

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_src_vsq_mapping_enable_get(unit, src_port, &enable));
    if (enable)
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Source port %d, is already mapped\n", src_port);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Create source port based VSQs
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port - logical port
 * \param [in] config - creation parameters
 * \param [out] src_port_vsq_gport - VSQ-E gport
 * \param [out] pg_base_vsq_gport  - base VSQ-F gport
 * \return
 *   See shr_error_e
 * \remark
 *   * VSQ A-D and global VSQ are created using bcm_cosq_gport_vsq_create
 *   * if config.numq == 0, VSQ-F is not created
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_cosq_src_vsqs_gport_add(
    int unit,
    bcm_gport_t port,
    bcm_cosq_src_vsqs_gport_config_t * config,
    bcm_gport_t * src_port_vsq_gport,
    bcm_gport_t * pg_base_vsq_gport)
{
    int pg_base = -1, src_port = 0;
    int core_id;
    int src_port_vsq_index;
    int cosq = 0;
    dnx_ingress_congestion_pg_params_t pg_params;
    dnx_ingress_congestion_vsq_tc_pg_mapping_t tc_pg_mapping;
    int tc, pg;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_src_vsqs_gport_add_verify(unit, port, config,
                                                             src_port_vsq_gport, pg_base_vsq_gport));

    if (BCM_GPORT_IS_LOCAL(port))
    {
        src_port = BCM_GPORT_LOCAL_GET(port);
    }
    else
    {
        src_port = port;
    }

    /** Get the relevant core ID from the src_port*/
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, src_port, &core_id));

    /** Allocate VSQ-E/VSQ-F id */
    /** first allocate VSQ-F as its resources are more limited */
    if (config->numq > 0)
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_f_id_alloc(unit, src_port, config->numq, &pg_base));
    }

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_e_id_alloc(unit, src_port, &src_port_vsq_index));

    BCM_COSQ_GPORT_VSQ_SRC_PORT_SET(*src_port_vsq_gport, core_id, 0, src_port_vsq_index);

    if (config->numq == 0)
    {
        *pg_base_vsq_gport = BCM_GPORT_INVALID;
    }
    else
    {
        BCM_COSQ_GPORT_VSQ_PG_SET(*pg_base_vsq_gport, core_id, pg_base);

        /** mapping tc cosq_bitmaps to profile*/
        for (pg = 0; pg < config->numq; pg++)
        {
            for (tc = 0; tc < DNX_COSQ_NOF_TC; tc++)
            {
                if (config->pg_attributes[pg].cosq_bitmap & (0x1 << tc))
                {
                    tc_pg_mapping.pg[tc] = pg;
                }
            }
        }
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_pg_tc_profile_exchange_and_set(unit, src_port, &tc_pg_mapping));

        /** configure a VSQ src port to VSQ PG mapping*/
        for (cosq = 0; cosq < config->numq; cosq++)
        {
            sal_memset(&pg_params, 0x0, sizeof(pg_params));

            /** In PG-params are per PG-VSQ */
            pg_params.pool_id = config->pg_attributes[cosq].pool_id;
            pg_params.is_lossless = config->pg_attributes[cosq].is_lossles;
            /** we don't manage different profiles. the only used profile is 0 */
            pg_params.admit_profile = 0;

            SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_pg_params_set(unit, core_id, pg_base + cosq, &pg_params));
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_pg_use_port_guaranteed_set(unit, core_id, pg_base + cosq,
                                                                                   TRUE));
        }
    }

    /** Map port to PG*/
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_src_vsq_mapping_set(unit, src_port,
                                                               src_port_vsq_index, pg_base, config->numq));

    /** update reverse mapping in SW state */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_port_based_vsq_sw_state_update(unit, core_id, src_port,
                                                                          src_port_vsq_index, pg_base));

    /** store numq in SW state */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.pg_numq.set(unit, src_port, config->numq));

    /** Last -- mark the port to have unchangable priority */
    SHR_IF_ERR_EXIT(dnx_cosq_port_fixed_priority_set(unit, src_port, 1));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify bcm_dnx_cosq_src_vsqs_gport_get parameters
 */
static int
dnx_cosq_src_vsqs_gport_get_verify(
    int unit,
    bcm_gport_t port,
    bcm_cosq_src_vsqs_gport_config_t * config,
    bcm_gport_t * src_port_vsq_gport,
    bcm_gport_t * pg_base_vsq_gport)
{
    bcm_port_t src_port;
    int enable;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(src_port_vsq_gport, _SHR_E_PARAM, "src_port_vsq_gport");
    SHR_NULL_CHECK(pg_base_vsq_gport, _SHR_E_PARAM, "pg_base_vsq_gport");
    SHR_NULL_CHECK(config, _SHR_E_PARAM, "config");

    if (BCM_GPORT_IS_LOCAL(port))
    {
        src_port = BCM_GPORT_LOCAL_GET(port);
    }
    else
    {
        src_port = port;
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, src_port));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_src_vsq_mapping_enable_get(unit, src_port, &enable));

    if (!enable)
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Source VSQs for src port %d, does not exist.\n", src_port);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * get source port based VSQs
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port - logical port
 * \param [out] config - creation parameters
 * \param [out] src_port_vsq_gport - VSQ-E gport
 * \param [out] pg_base_vsq_gport  - base VSQ-F gport
 * \return
 *   See shr_error_e
 * \remark
 *   * VSQ A-D and global VSQ are created using bcm_cosq_gport_vsq_create
 *   * if config.numq == 0, VSQ-F does not exist
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_cosq_src_vsqs_gport_get(
    int unit,
    bcm_gport_t port,
    bcm_cosq_src_vsqs_gport_config_t * config,
    bcm_gport_t * src_port_vsq_gport,
    bcm_gport_t * pg_base_vsq_gport)
{
    int pg_base = 0, src_port = 0;
    int core_id;
    int cosq = 0;
    dnx_ingress_congestion_vsq_tc_pg_mapping_t tc_pg_mapping;
    int src_port_vsq_index = 0;
    int tc, pg;

    dnx_ingress_congestion_pg_params_t pg_params;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_src_vsqs_gport_get_verify(unit, port, config,
                                                             src_port_vsq_gport, pg_base_vsq_gport));

    if (BCM_GPORT_IS_LOCAL(port))
    {
        src_port = BCM_GPORT_LOCAL_GET(port);
    }
    else
    {
        src_port = port;
    }

    /** Get the relevant core ID from the src_port*/
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, src_port, &core_id));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_src_vsq_mapping_get(unit, src_port, &src_port_vsq_index, &pg_base));

    /** clear config to make sure unused fields are cleared */
    sal_memset(config, 0x0, sizeof(bcm_cosq_src_vsqs_gport_config_t));

    /** configure a VSQ src gport*/
    BCM_COSQ_GPORT_VSQ_SRC_PORT_SET(*src_port_vsq_gport, core_id, 0, src_port_vsq_index);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.pg_numq.get(unit, src_port, &(config->numq)));

    if (config->numq > 0)
    {
        BCM_COSQ_GPORT_VSQ_PG_SET(*pg_base_vsq_gport, core_id, pg_base);
    }
    else
    {
        *pg_base_vsq_gport = BCM_GPORT_INVALID;
    }

    if (config->numq > 0)
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_tc_pg_mapping_get(unit, src_port, &tc_pg_mapping));

        for (tc = 0; tc < DNX_COSQ_NOF_TC; tc++)
        {
            pg = tc_pg_mapping.pg[tc];
            config->pg_attributes[pg].cosq_bitmap |= (0x1 << tc);
        }

        for (cosq = 0; cosq < config->numq; cosq++)
        {
            sal_memset(&pg_params, 0x0, sizeof(pg_params));

            SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_pg_params_get(unit, core_id, pg_base + cosq, &pg_params));

            config->pg_attributes[cosq].pool_id = pg_params.pool_id;
            config->pg_attributes[cosq].is_lossles = pg_params.is_lossless;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * verify parameters of bcm_dnx_cosq_gport_vsq_destroy
 */
static int
dnx_cosq_gport_vsq_destroy_verify(
    int unit,
    bcm_gport_t vsq_gport)
{
    bcm_port_t port;
    bcm_cosq_vsq_info_t vsq_info;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify src based VSQ */
    if (BCM_COSQ_GPORT_IS_VSQ_SRC_PORT(vsq_gport) || BCM_COSQ_GPORT_IS_VSQ_PG(vsq_gport) ||
        !BCM_COSQ_GPORT_IS_VSQ(vsq_gport))
    {
        if (BCM_GPORT_IS_LOCAL(vsq_gport))
        {
            port = BCM_GPORT_LOCAL_GET(vsq_gport);
        }
        else if (BCM_COSQ_GPORT_IS_VSQ_SRC_PORT(vsq_gport) || BCM_COSQ_GPORT_IS_VSQ_PG(vsq_gport))
        {
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_info_get(unit, vsq_gport, &vsq_info));
            port = vsq_info.src_port;
        }
        else
        {
            port = vsq_gport;
        }
        SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));

        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

        if (!DNX_ALGO_PORT_TYPE_IS_TM(unit, port_info) || DNX_ALGO_PORT_TYPE_IS_ERP(unit, port_info))
        {
            SHR_ERR_EXIT(_SHR_E_PORT, "Port type of port %d is not supported by this API\n", port);
        }
    }
    /*
     * else -- destination VSQ
     * nothing to verify
     */
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Destroy VSQ
 *
 * \param [in] unit -  Unit-ID
 * \param [in] vsq_gport - VSQ gport
 * \return
 *   See shr_error_e
 * \remark
 *   * this API is used for all VSQ types
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_cosq_gport_vsq_destroy(
    int unit,
    bcm_gport_t vsq_gport)
{
    int core_id = BCM_CORE_ALL;
    dnx_ingress_congestion_vsq_group_e vsq_type = DNX_INGRESS_CONGESTION_VSQ_GROUP_NOF;
    int vsq_index;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_gport_vsq_destroy_verify(unit, vsq_gport));

    /*
     * Upon deleting VSQ, the following configuration taking place:
     * 1. Map vsq to default rate class.
     * 2. If relevant, deallocate in allocation management
     */

    if (BCM_COSQ_GPORT_IS_VSQ(vsq_gport) &&
        !BCM_COSQ_GPORT_IS_VSQ_SRC_PORT(vsq_gport) && !BCM_COSQ_GPORT_IS_VSQ_PG(vsq_gport))
    {

        SHR_IF_ERR_EXIT(dnx_ingress_congestion_gport_vsq_get(unit, vsq_gport, 0, &core_id, &vsq_type, &vsq_index));
        SHR_IF_ERR_EXIT(dnx_vsq_rate_class_destroy(unit, core_id, vsq_index, vsq_type));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_src_vsqs_gport_destroy(unit, vsq_gport));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Verify bcm_cosq_resource_allocation_set/get common parameters
 */
static int
dnx_cosq_resource_allocation_verify(
    int unit,
    uint32 flags,
    bcm_cosq_resource_t resource,
    bcm_cosq_allocation_entity_t * target,
    bcm_cosq_resource_amounts_t * amounts)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(target, _SHR_E_PARAM, "target");
    SHR_NULL_CHECK(amounts, _SHR_E_PARAM, "amounts");

    if (target->color >= bcmColorPreserve)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Target color(%d) out of range.\n", target->color);
    }

    SHR_MAX_VERIFY(target->pool_id, dnx_data_ingr_congestion.vsq.pool_nof_get(unit) - 1,
                   _SHR_E_PARAM, "Invalid pool ID %d.\n", target->pool_id);

    if (!BCM_COSQ_GPORT_IS_VSQ_SRC_PORT(target->gport) && !BCM_COSQ_GPORT_IS_VSQ_PG(target->gport) &&
        !BCM_COSQ_GPORT_IS_VSQ_GL(target->gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "VSQ(%d) - expected global VSQ, source port VSQ or PG VSQ\n", target->gport);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Verify bcm_cosq_resource_allocation_get parameters
 */
static int
dnx_cosq_resource_allocation_get_verify(
    int unit,
    uint32 flags,
    bcm_cosq_resource_t resource,
    bcm_cosq_allocation_entity_t * target,
    bcm_cosq_resource_amounts_t * amounts)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_resource_allocation_verify(unit, flags, resource, target, amounts));

    SHR_MASK_VERIFY(flags, 0, _SHR_E_PARAM, "No flag supported.\n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Get VSQ resource allocation
 *
 * \param [in] unit -  Unit-ID
 * \param [in] flags - flags
 * \param [in] resource - total bytes, SRAM buffers or SRAM PDs
 * \param [in] target - describes the object to get the resource allocation - either global VSQ of VSQ E/F
 * \param [out] amounts - the resource allocation details
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_cosq_resource_allocation_get(
    int unit,
    uint32 flags,
    bcm_cosq_resource_t resource,
    bcm_cosq_allocation_entity_t * target,
    bcm_cosq_resource_amounts_t * amounts)
{
    dnx_ingress_congestion_resource_type_e rsrc_type;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_resource_allocation_get_verify(unit, flags, resource, target, amounts));

    /** get mapping between VSQE and VSQF*/

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_cosq_resource_to_ingress_resource(unit, resource, &rsrc_type));

    if (BCM_COSQ_GPORT_IS_VSQ_GL(target->gport))
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_global_vsq_resource_allocation_get(unit, flags, rsrc_type,
                                                                                  target, amounts));
    }
    else
    {   /* VSQ E-F */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_src_vsq_resource_allocation_get(unit, flags, rsrc_type,
                                                                               target, amounts));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Verify bcm_cosq_resource_allocation_set parameters
 */
static int
dnx_cosq_resource_allocation_set_verify(
    int unit,
    uint32 flags,
    bcm_cosq_resource_t resource,
    bcm_cosq_allocation_entity_t * target,
    bcm_cosq_resource_amounts_t * amounts)
{
    dnx_ingress_congestion_resource_type_e rsrc_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_resource_allocation_verify(unit, flags, resource, target, amounts));

    SHR_MASK_VERIFY(flags, BCM_COSQ_RSRC_ALLOC_COLOR_BLIND, _SHR_E_PARAM, "Unsupported flag provided.\n");

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_cosq_resource_to_ingress_resource(unit, resource, &rsrc_type));

    /** Validate thresholds */
    if (BCM_COSQ_GPORT_IS_VSQ_GL(target->gport))
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_threshold_verify(unit, rsrc_type, "max_shared_pool",
                                                                amounts->max_shared_pool));

        SHR_IF_ERR_EXIT(dnx_ingress_congestion_threshold_verify(unit, rsrc_type, "max_headroom",
                                                                amounts->max_headroom));

        SHR_IF_ERR_EXIT(dnx_ingress_congestion_threshold_verify(unit, rsrc_type, "nominal_headroom",
                                                                amounts->nominal_headroom));

        if (amounts->nominal_headroom > amounts->max_headroom)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Nominal Headroom size (%u) can't be larger than Max Headroom size (%u)",
                         amounts->nominal_headroom, amounts->max_headroom);
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_threshold_verify(unit, rsrc_type, "reserved", amounts->reserved));

        SHR_IF_ERR_EXIT(dnx_ingress_congestion_threshold_verify(unit, rsrc_type, "max_shared_pool",
                                                                amounts->max_shared_pool));

        SHR_IF_ERR_EXIT(dnx_ingress_congestion_threshold_verify(unit, rsrc_type, "max_headroom",
                                                                amounts->max_headroom));
    }
    if (BCM_COSQ_GPORT_IS_VSQ_PG(target->gport))
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_threshold_verify(unit, rsrc_type, "shared_pool_fadt.thresh_max",
                                                                amounts->shared_pool_fadt.thresh_max));
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_threshold_verify(unit, rsrc_type, "shared_pool_fadt.thresh_min",
                                                                amounts->shared_pool_fadt.thresh_min));

        SHR_RANGE_VERIFY(amounts->shared_pool_fadt.alpha,
                         dnx_data_ingr_congestion.info.resource_get(unit, rsrc_type)->fadt_alpha_min,
                         dnx_data_ingr_congestion.info.resource_get(unit, rsrc_type)->fadt_alpha_max,
                         _SHR_E_PARAM, "alpha %d is out of range\n", amounts->shared_pool_fadt.alpha);

        SHR_IF_ERR_EXIT(dnx_ingress_congestion_threshold_verify(unit, rsrc_type, "nominal_headroom",
                                                                amounts->nominal_headroom));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Set VSQ resource allocation
 *
 * \param [in] unit -  Unit-ID
 * \param [in] flags - flags
 * \param [in] resource - total bytes, SRAM buffers or SRAM PDs
 * \param [in] target - describes the object to get the resource allocation - either global VSQ of VSQ E/F
 * \param [in] amounts - the resource allocation details
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_cosq_resource_allocation_set(
    int unit,
    uint32 flags,
    bcm_cosq_resource_t resource,
    bcm_cosq_allocation_entity_t * target,
    bcm_cosq_resource_amounts_t * amounts)
{
    dnx_ingress_congestion_resource_type_e rsrc_type;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Input validation */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_resource_allocation_set_verify(unit, flags, resource, target, amounts));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_cosq_resource_to_ingress_resource(unit, resource, &rsrc_type));

    if (BCM_COSQ_GPORT_IS_VSQ_GL(target->gport))
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_global_vsq_resource_allocation_set(unit, flags, rsrc_type,
                                                                                  target, amounts));

    }
    else
    {   /* VSQ E/F */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_src_vsq_resource_allocation_set(unit, flags, rsrc_type,
                                                                               target, amounts));

    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/*
 * brief - function to verify general arguments for bcm_dnx_cosq_gport_color_size_get/set API
 * more specific verifiers can be found in inner functions
 */
static shr_error_e
dnx_cosq_gport_color_size_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_color_t color,
    uint32 flags,
    bcm_cosq_gport_size_t * gport_size)
{

    int is_color_blind, is_system_red;
    uint32 supported_flags, is_vsq_allowed = TRUE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(gport_size, _SHR_E_PARAM, "gport_size");

    /** make sure the gport is either for (VOQ) rate class or for VSQ */
    SHR_IF_ERR_EXIT(dnx_cosq_rate_class_gport_verify(unit, gport, cosq, is_vsq_allowed));

    /*
     * Verify Flags
     */
    supported_flags = BCM_COSQ_GPORT_SIZE_BYTES | BCM_COSQ_GPORT_SIZE_BUFFERS | BCM_COSQ_GPORT_SIZE_PACKET_DESC |
        BCM_COSQ_GPORT_SIZE_OCB | BCM_COSQ_GPORT_SIZE_COLOR_BLIND;

    if (BCM_GPORT_IS_PROFILE(gport))
    {
        /** VOQ */
        supported_flags |= BCM_COSQ_GPORT_SIZE_COLOR_SYSTEM_RED;
    }

    SHR_MASK_VERIFY(flags, supported_flags, _SHR_E_PARAM, "non supported flags specified.\n");

    /*
     * Verify color (DP)
     */
    is_color_blind = flags & BCM_COSQ_GPORT_SIZE_COLOR_BLIND;
    is_system_red = flags & BCM_COSQ_GPORT_SIZE_COLOR_SYSTEM_RED;

    if (!is_color_blind && !is_system_red)
    {
        SHR_RANGE_VERIFY(color, 0, DNX_COSQ_NOF_DP - 1, _SHR_E_PARAM, "Invalid color levels specified %d\n", color);
    }

    if (!is_system_red)
    {
        /*
         * Verify exactly one resource flag is set
         */
        SHR_NOF_SET_BITS_IN_RANGE_VERIFY(flags,
                                         (BCM_COSQ_GPORT_SIZE_BYTES | BCM_COSQ_GPORT_SIZE_BUFFERS |
                                          BCM_COSQ_GPORT_SIZE_PACKET_DESC), 1, 1, _SHR_E_PARAM,
                                         "Exactly one resource flag should be specified\n");
    }

    /*
     * Make sure buffers and packet descriptors is always specified with OCB
     */
    if ((flags & (BCM_COSQ_GPORT_SIZE_BUFFERS | BCM_COSQ_GPORT_SIZE_PACKET_DESC)) && !(flags & BCM_COSQ_GPORT_SIZE_OCB))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Buffers and Packet descriptors should always be specified with OCB flag\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get queue guaranteed and tail drop thresholds
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] gport -  gport to retrieve the information for. should be rate class or VSQ gport
 *   \param [in] cosq - irrelevant here. should be set to 0 / -1
 *   \param [in] color - DP (0-3)
 *   \param [in] flags - resource type to set, color blind, system-red
 *   \param [out] gport_size - threshold sizes.
 *
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_cosq_gport_color_size_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_color_t color,
    uint32 flags,
    bcm_cosq_gport_size_t * gport_size)
{
    uint32 is_system_red;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_gport_color_size_verify(unit, gport, cosq, color, flags, gport_size));

    if (BCM_GPORT_IS_PROFILE(gport))
    {
        /** Rate class gport */
        is_system_red = (flags & BCM_COSQ_GPORT_SIZE_COLOR_SYSTEM_RED);
        if (is_system_red)
        {
            SHR_IF_ERR_EXIT(dnx_system_red_voq_max_size_get(unit, gport, color, gport_size));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_voq_rate_class_color_size_get(unit, gport, color, flags, gport_size));
        }
    }
    else
    {
        /** VSQ gport */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_color_size_get(unit, gport, cosq, color, flags, gport_size));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set queue guaranteed and tail drop thresholds
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] gport -  gport to set the information for. should be rate class or VSQ gport
 *   \param [in] cosq - irrelevant here. should be set to 0 / -1
 *   \param [in] color - DP (0-3)
 *   \param [in] flags - resource type to set, color blind, system-red
 *   \param [in] gport_size - threshold sizes.
 *
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_cosq_gport_color_size_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_color_t color,
    uint32 flags,
    bcm_cosq_gport_size_t * gport_size)
{
    int is_system_red;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_gport_color_size_verify(unit, gport, cosq, color, flags, gport_size));

    if (BCM_GPORT_IS_PROFILE(gport))
    {
        /** Rate Class gport*/
        is_system_red = (flags & BCM_COSQ_GPORT_SIZE_COLOR_SYSTEM_RED);
        if (is_system_red)
        {
            SHR_IF_ERR_EXIT(dnx_system_red_voq_max_size_set(unit, gport, color, gport_size));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_voq_rate_class_color_size_set(unit, gport, color, flags, gport_size));
        }
    }
    else
    {
        /** VSQ gport */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_color_size_set(unit, gport, cosq, color, flags, gport_size));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify procedure for bcm_dnx_cosq_inst_congestion_info_get API
 */
static int
dnx_cosq_inst_congestion_info_get_verify(
    int unit,
    uint32 flags,
    bcm_cosq_inst_congestion_info_key_t * key,
    bcm_cosq_inst_congestion_info_data_t * data)
{
    uint32 allowed_flags;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key, _SHR_E_PARAM, "key");
    SHR_NULL_CHECK(data, _SHR_E_PARAM, "data");

    /** verify allowed flags */
    allowed_flags = 0;
    SHR_MASK_VERIFY(flags, allowed_flags, _SHR_E_PARAM, "Unsupported flag provided.\n");

    /** verify core */
    if ((key->core_id < 0) || (key->core_id >= dnx_data_device.general.nof_cores_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal core %d\n", key->core_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get next congested element from congestion fifo
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] flags - flags
 *   \param [in] key -  key attributes that represent the congestion element (type, core)
 *   \param [out] data - returned congested element info (gport id, type of resource)
 *
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 *
 * \see
 *   * None
 */
int
bcm_dnx_cosq_inst_congestion_info_get(
    int unit,
    uint32 flags,
    bcm_cosq_inst_congestion_info_key_t * key,
    bcm_cosq_inst_congestion_info_data_t * data)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_inst_congestion_info_get_verify(unit, flags, key, data));

    /** get congestion data */
    SHR_IF_ERR_EXIT(dnx_cosq_inst_congestion_info_get(unit, flags, key, data));

exit:
    SHR_FUNC_EXIT;
}
