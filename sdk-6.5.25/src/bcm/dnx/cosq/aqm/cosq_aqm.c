/** \file cosq_aqm.c
 * 
 *
 * COSQ Global VOQ functionality \n
 *
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/error.h>
#include <bcm/cosq.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm/types.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/port/port.h>
#include <bcm_int/dnx/cosq/aqm/cosq_aqm.h>
#include <bcm_int/dnx/cosq/latency/cosq_latency.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_cosq_aqm.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/cosq_aqm_access.h>

/*
 * }
 */

/*
 * Defines
 * {
 */
#define DNX_COSQ_AQM_DEFAULT_PROFILE (0)
/*
 * }
 */
shr_error_e bcm_dnx_cosq_aqm_profile_create(
    int unit,
    int flags,
    bcm_gport_t * profile_gport);

/** see .h file */
shr_error_e
dnx_cosq_aqm_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    bcm_gport_t gport;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_aqm_db.init(unit));

    if (dnx_data_cosq_aqm.general.feature_get(unit, dnx_data_cosq_aqm_general_is_aqm_supported))
    {
        /** create common AQM res mngr */
        sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
        data.first_element = 0;
        data.nof_elements = dnx_data_cosq_aqm.general.profile_nof_get(unit);
        data.flags = 0;
        sal_strncpy(data.name, DNX_COSQ_AQM_PROFILE_RESOURCE, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
        SHR_IF_ERR_EXIT(dnx_cosq_aqm_db.aqm_profile_res.create(unit, &data, NULL));
        /** capture the first profile as default profile that do not perform any action */
        BCM_GPORT_AQM_PROFILE_SET(gport, DNX_COSQ_AQM_DEFAULT_PROFILE);
        SHR_IF_ERR_EXIT(bcm_dnx_cosq_aqm_profile_create(unit, BCM_COSQ_GPORT_WITH_ID, &gport));

        /** create AQM flow id res mngr */
        sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
        data.first_element = 0;
        data.nof_elements = dnx_data_cosq_aqm.general.flows_nof_get(unit);
        data.flags = 0;
        sal_strncpy(data.name, DNX_COSQ_AQM_PROFILE_RESOURCE, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
        SHR_IF_ERR_EXIT(dnx_cosq_aqm_db.aqm_flow_id_res.create(unit, &data, NULL));
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_aqm_profile_create_verify(
    int unit,
    uint32 flags,
    int *gport_profile)
{
    uint32 legal_flags = BCM_COSQ_GPORT_WITH_ID;

    SHR_FUNC_INIT_VARS(unit);

    DNX_COSQ_AQM_FEATURE_VERIFY(unit);

    SHR_NULL_CHECK(gport_profile, _SHR_E_PARAM, "gport_profile");
    SHR_MASK_VERIFY(flags, legal_flags, _SHR_E_PARAM, "some of the flags are not supported.\n");

    if (_SHR_IS_FLAG_SET(flags, BCM_COSQ_GPORT_WITH_ID))
    {
        if (BCM_GPORT_IS_AQM_PROFILE(*gport_profile) == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid gport_profile %d", *gport_profile);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Create an AQM profile gport.
 *          AQM profile is used to configure a profile where either 'end-to-end latency AQM' or 'Global VOQ AQM' are present.
 *          This API can be used instead of bcm_dnx_cosq_latency_profile_create to create a profile that relates only to 'end-to-end latency AQM'.
 *
 * \param [in] unit
 * \param [in] flags - possible flags:
 *                          BCM_COSQ_GPORT_WITH_ID.
 * \param [out] profile_gport - the created AQM profile
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_cosq_aqm_profile_create(
    int unit,
    int flags,
    bcm_gport_t * profile_gport)
{
    uint32 res_flags = 0;
    int profile_id = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_cosq_aqm_profile_create_verify(unit, flags, profile_gport));

    if (_SHR_IS_FLAG_SET(flags, BCM_COSQ_GPORT_WITH_ID))
    {
        profile_id = BCM_GPORT_AQM_PROFILE_GET(*profile_gport);
        res_flags |= DNX_ALGO_RES_ALLOCATE_WITH_ID;
    }

    SHR_IF_ERR_EXIT(dnx_cosq_aqm_db.aqm_profile_res.allocate_single(unit, res_flags, NULL, &profile_id));

    BCM_GPORT_AQM_PROFILE_SET((*profile_gport), profile_id);

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

static shr_error_e
bcm_dnx_cosq_aqm_profile_destroy_verify(
    int unit,
    int flags,
    bcm_gport_t profile_gport)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_COSQ_AQM_FEATURE_VERIFY(unit);

    if (!BCM_GPORT_IS_AQM_PROFILE(profile_gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid gport_profile %d", profile_gport);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief    destroy a AQM profile or end-to-end latency AQM profile.
*
* \param [in] unit -unit id
* \param [in] flags - NONE
* \param [in] profile_gport - gport of the latency profile
* \return
*    shr_error_e
* \remark
*   None
* \see
*   NONE
*/
shr_error_e
bcm_dnx_cosq_aqm_profile_destroy(
    int unit,
    int flags,
    bcm_gport_t profile_gport)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNXC(bcm_dnx_cosq_aqm_profile_destroy_verify(unit, flags, profile_gport));

    if (BCM_GPORT_IS_AQM_PROFILE(profile_gport))
    {
        int profile_id = BCM_GPORT_AQM_PROFILE_GET(profile_gport);
        SHR_IF_ERR_EXIT(dnx_cosq_aqm_db.aqm_profile_res.free_single(unit, profile_id, NULL));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verification function for AQM flow profile mapping set/get APIs
 */
static shr_error_e
dnx_cosq_aqm_flow_profile_mapping_verify(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg,
    uint8 is_set)
{
    int flow_id = 0;
    int flows_nof = dnx_data_cosq_aqm.general.flows_nof_get(unit);
    int profile_nof = dnx_data_cosq_aqm.general.profile_nof_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    /** Check if active quque management is supported on current device */
    DNX_COSQ_AQM_FEATURE_VERIFY(unit);

    if (type == bcmCosqControlEgressAqmFlowIdToProfile)
    {
        /** Verify Flow ID */
        flow_id = BCM_GPORT_AQM_FLOW_ID_GET(port);

        SHR_RANGE_VERIFY(flow_id, 0, flows_nof - 1, _SHR_E_PARAM,
                         "AQM Flow ID %d is out of range 0-%d\n", flow_id, flows_nof - 1);

        if (is_set == TRUE)
        {
            /** In case of SET verify also argument - must be profile ID within the valid range */
            SHR_RANGE_VERIFY(arg, 0, profile_nof - 1, _SHR_E_PARAM,
                             "AQM profile %d is out of range 0-%d\n", arg, profile_nof - 1);
        }
    }

    if (type == bcmCosqControlEgressAqmFlowId)
    {
        /** Verify port */
        SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));

        /** Verify COSQ */
        SHR_RANGE_VERIFY(cosq, BCM_COS_MIN, BCM_COS_MAX, _SHR_E_PARAM, "COSQ %d is out of range %d-%d\n", cosq,
                         BCM_COS_MIN, BCM_COS_MAX);

        if (is_set)
        {
            /** In case of SET verify also argument - must be flow ID within the valid range */
            SHR_RANGE_VERIFY(arg, 0, flows_nof - 1, _SHR_E_PARAM,
                             "AQM Flow ID %d is out of range 0-%d\n", arg, flows_nof - 1);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_aqm_flow_profile_mapping_aqm_port_to_flow_id_hw_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    uint32 arg)
{
    bcm_core_t core;
    uint32 entry_handle_id, out_tm_port;
    bcm_port_t logical_port = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** map port and TC to aqm flow id gport */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_EGRESS_L4Q_MAP_AQM_FLOW_ID_PORT_TC, &entry_handle_id));
    /*
     * User needs to provide logical port as argument - Get OUT TM Port + Core
     */
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_out_tm_port_get(unit, logical_port, &core, &out_tm_port));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_TM_PORT, out_tm_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, cosq);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_L4Q_FLOW_ID, INST_SINGLE, arg);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_aqm_flow_profile_mapping_aqm_flow_to_profile_hw_set(
    int unit,
    bcm_gport_t aqm_flow_id,
    uint32 arg)
{
    uint32 entry_handle_id;
    uint32 aqm_flow_id_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** get mapped aqm flow id gport to port and tc */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit,
                                      DBAL_TABLE_LATENCY_EGRESS_L4Q_MAP_AQM_FLOW_ID_TO_PROFILE, &entry_handle_id));

    aqm_flow_id_index = BCM_GPORT_LATENCY_AQM_FLOW_ID_GET(aqm_flow_id);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_L4Q_FLOW_ID, aqm_flow_id_index);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_L4Q_FLOW_PROFILE, INST_SINGLE, arg);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_cosq_aqm_flow_profile_mapping_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    uint32 arg)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify inputs parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_cosq_aqm_flow_profile_mapping_verify(unit, port, cosq, type, arg, TRUE));

    switch (type)
    {
        case bcmCosqControlEgressAqmFlowId:
            /** case bcmCosqControlLatencyEgressAqmFlowId: */
            SHR_IF_ERR_EXIT(dnx_cosq_aqm_flow_profile_mapping_aqm_port_to_flow_id_hw_set(unit, port, cosq, arg));
            break;

        case bcmCosqControlEgressAqmFlowIdToProfile:
            /** case bcmCosqControlLatencyEgressAqmFlowIdToProfile: */
            SHR_IF_ERR_EXIT(dnx_cosq_aqm_flow_profile_mapping_aqm_flow_to_profile_hw_set(unit, port, arg));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "type=%d not supported for end to end AQM latency profile \n", type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_aqm_flow_profile_mapping_aqm_port_to_flow_id_hw_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    uint32 *arg)
{
    bcm_core_t core;
    uint32 entry_handle_id, out_tm_port;
    bcm_port_t logical_port = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** get mapped aqm flow id gport to port and tc */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_EGRESS_L4Q_MAP_AQM_FLOW_ID_PORT_TC, &entry_handle_id));
    /*
     * User needs to provide logical port as argument - Get OUT TM Port + Core
     */
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_out_tm_port_get(unit, logical_port, &core, &out_tm_port));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_TM_PORT, out_tm_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, cosq);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_L4Q_FLOW_ID, INST_SINGLE, arg);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_aqm_flow_profile_mapping_aqm_flow_to_profile_hw_get(
    int unit,
    bcm_gport_t aqm_flow_id,
    uint32 *arg)
{
    uint32 entry_handle_id;
    uint32 aqm_flow_id_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** get mapped aqm flow id gport to port and tc */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit,
                                      DBAL_TABLE_LATENCY_EGRESS_L4Q_MAP_AQM_FLOW_ID_TO_PROFILE, &entry_handle_id));

    aqm_flow_id_index = BCM_GPORT_LATENCY_AQM_FLOW_ID_GET(aqm_flow_id);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_L4Q_FLOW_ID, aqm_flow_id_index);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_L4Q_FLOW_PROFILE, INST_SINGLE, arg);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_cosq_aqm_flow_profile_mapping_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    uint32 *arg)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify inputs parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_cosq_aqm_flow_profile_mapping_verify(unit, port, cosq, type, *arg, FALSE));

    switch (type)
    {
        case bcmCosqControlEgressAqmFlowId:
            /** case bcmCosqControlLatencyEgressAqmFlowId: */
            SHR_IF_ERR_EXIT(dnx_cosq_aqm_flow_profile_mapping_aqm_port_to_flow_id_hw_get(unit, port, cosq, arg));
            break;

        case bcmCosqControlEgressAqmFlowIdToProfile:
            /** case bcmCosqControlLatencyEgressAqmFlowIdToProfile */
            SHR_IF_ERR_EXIT(dnx_cosq_aqm_flow_profile_mapping_aqm_flow_to_profile_hw_get(unit, port, arg));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "type=%d not supported for end to end AQM latency profile \n", type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* verification function for AQM flow id gport add API
*/
static shr_error_e
dnx_cosq_aqm_flow_id_gport_verify(
    int unit,
    uint32 flags,
    bcm_gport_t * gport)
{
    uint32 legal_flags = BCM_COSQ_GPORT_WITH_ID | BCM_COSQ_GPORT_LATENCY_AQM_FLOW_ID;
    int flows_nof = dnx_data_cosq_aqm.general.flows_nof_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    DNX_COSQ_AQM_FEATURE_VERIFY(unit);

    SHR_NULL_CHECK(gport, _SHR_E_PARAM, "gport");
    SHR_MASK_VERIFY(flags, legal_flags, _SHR_E_PARAM, "some of the flags are not supported.\n");

    if (_SHR_IS_FLAG_SET(flags, BCM_COSQ_GPORT_WITH_ID))
    {
        int flow_id = BCM_GPORT_LATENCY_AQM_FLOW_ID_GET(*gport);
        SHR_RANGE_VERIFY(flow_id, 0, flows_nof - 1, _SHR_E_PARAM,
                         "AQM Flow ID %d is out of range 0-%d\n", flow_id, flows_nof - 1);
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_cosq_aqm_flow_id_gport_add(
    int unit,
    uint32 flags,
    bcm_gport_t * gport)
{
    uint32 res_flags = 0;
    int flow_id = 0;
    resource_tag_bitmap_extra_arguments_alloc_info_t alloc_info;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_cosq_aqm_flow_id_gport_verify(unit, flags, gport));

    sal_memset(&alloc_info, 0x0, sizeof(resource_tag_bitmap_extra_arguments_alloc_info_t));

    if (_SHR_IS_FLAG_SET(flags, BCM_COSQ_GPORT_WITH_ID))
    {
        flow_id = BCM_GPORT_LATENCY_AQM_FLOW_ID_GET(*gport);
        res_flags |= DNX_ALGO_RES_ALLOCATE_WITH_ID;
    }
    else
    {
        res_flags |= RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE;
        alloc_info.range_start = 0;
        alloc_info.range_end = dnx_data_cosq_aqm.general.flows_nof_get(unit) - 1;
    }

    res_flags |= RESOURCE_TAG_BITMAP_ALWAYS_CHECK_TAG;
    alloc_info.tag = 0;

    SHR_IF_ERR_EXIT(dnx_cosq_aqm_db.aqm_flow_id_res.allocate_single(unit, res_flags, (void *) &alloc_info, &flow_id));

    BCM_GPORT_LATENCY_AQM_FLOW_ID_SET((*gport), flow_id);

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* verification function for AQM flow id gport add API
*/
static shr_error_e
dnx_cosq_aqm_flow_id_gport_delete_verify(
    int unit,
    bcm_gport_t gport)
{
    int flow_id = 0;
    int flow_nof = dnx_data_cosq_aqm.general.flows_nof_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    DNX_COSQ_AQM_FEATURE_VERIFY(unit);

    flow_id = BCM_GPORT_LATENCY_AQM_FLOW_ID_GET(gport);

    SHR_RANGE_VERIFY(flow_id, 0, flow_nof - 1, _SHR_E_PARAM,
                     "AQM Flow ID %d is out of range 0-%d\n", flow_id, flow_nof - 1);

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_cosq_aqm_flow_id_gport_delete(
    int unit,
    bcm_gport_t gport)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_cosq_aqm_flow_id_gport_delete_verify(unit, gport));

    if (BCM_GPORT_IS_LATENCY_AQM_FLOW_ID(gport))
    {
        int flow_id = BCM_GPORT_LATENCY_AQM_FLOW_ID_GET(gport);

        /** free from resource manager*/
        SHR_IF_ERR_EXIT(dnx_cosq_aqm_db.aqm_flow_id_res.free_single(unit, flow_id, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_aqm_config_verify(
    int unit,
    bcm_gport_t aqm_profile,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    int aqm_profile_index;
    int aqm_profile_nof = dnx_data_cosq_aqm.general.profile_nof_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    DNX_COSQ_AQM_FEATURE_VERIFY(unit);

    aqm_profile_index = BCM_GPORT_AQM_PROFILE_GET(aqm_profile);

    SHR_RANGE_VERIFY(aqm_profile_index, 0, aqm_profile_nof - 1, _SHR_E_PARAM,
                     "AQM profile ID %d is out of range 0-%d\n", aqm_profile_index, aqm_profile_nof - 1);

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_cosq_aqm_config_set(
    int unit,
    bcm_gport_t aqm_profile,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_cosq_aqm_config_verify(unit, aqm_profile, cosq, type, arg));

    SHR_IF_ERR_EXIT(dnx_cosq_latency_profile_set(unit, aqm_profile, cosq, type, arg));

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_cosq_aqm_config_get(
    int unit,
    bcm_gport_t aqm_profile,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_cosq_aqm_config_verify(unit, aqm_profile, cosq, type, 0));

    SHR_IF_ERR_EXIT(dnx_cosq_latency_profile_get(unit, aqm_profile, cosq, type, arg));

exit:
    SHR_FUNC_EXIT;
}
