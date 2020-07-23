/** \file stat.c
 * 
 *  stat module gather all functionality that related to the counters(crps, stif, mib) in DNX.
 *  stat.c holds the functionality that is common for more than one counters sub-module (for example: bcm_stat_control API).
 *  It's matching h file is bcm/stat.h.
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_CNT

/*
 * Include files.
 * {
 */
#include <shared/utilex/utilex_bitstream.h>
#include <shared/shrextend/shrextend_error.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/stat.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_crps.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stat.h>
#include <bcm_int/dnx/stat/stat_mgmt.h>
#include <src/bcm/dnx/stat/crps/crps_mgmt_internal.h>
#include <src/bcm/dnx/stat/crps/crps_verify.h>
#include <include/bcm_int/dnx/stat/crps/crps_src_interface.h>
#include <bcm_int/dnx/stat/stif/stif_mgmt.h>
#include <include/soc/dnx/swstate/auto_generated/access/stif_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stif.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>

/*
 * }
 */

/*
 * MACROs/Defines
 * {
 */
#define DNX_STAT_TRAFFIC_CLASS_SIZE (3)
#define DNX_STAT_MULTICAST_SIZE (1)
#define DNX_STAT_NO_SUPPORT (-1)

/*
 * }
 */

/**
 * \brief
 * Verify function for BCM-API dnx_stat_control_set
 */
static shr_error_e
dnx_stat_control_set_verify(
    int unit,
    int flags,
    bcm_stat_control_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case bcmStatControlStifFcEnable:
            /** check if stif is enabled */
            if (dnx_data_stif.config.feature_get(unit, dnx_data_stif_config_stif_enable) == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_DISABLED,
                             "Statistic Interface is not enabled, use soc property stat_if_enable to enable it.\n");
            }

            if (flags != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             " Incorrect flags=%d\n, expected flags=0 for type=bcmStatControlStifFcEnable", flags);
            }
            if ((arg != TRUE) && (arg != FALSE))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             " Incorrect arg=%d\n, type=bcmStatControlStifFcEnable - arg value should be TRUE or FALSE",
                             arg);
            }
            break;
        case bcmStatControlCounterThreadEnable:
            if (flags != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             " Incorrect flags=%d\n, expected flags=0 for type=bcmStatControlCounterThreadEnable",
                             flags);
            }
            if ((arg != TRUE) && (arg != FALSE))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             " Incorrect arg=%d\n, type=bcmStatControlCounterThreadEnable - arg value should be TRUE or FALSE",
                             arg);
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, " invalid stat control type (=%d)\n", type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Generic stat control API function, used for all kinds of stat sub-modules (CRPS/Stif/mib).
 *
 * Use cases:
 * 0. stif - Enable/disable stif flow control
 * Parameters:
 * - flags - must be 0
 * - type - bcmStatControlStifFcEnable
 * - arg -  TRUE/FALSE to enable/disable the flow control
 * 1.
 * crps - Enable/disable crps bg thread operation.
 * Parameters:
 * - type - bcmStatControlCounterThreadEnable
 * - arg -  TRUE/FALSE to enable/disable the bg thread
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stat_control_set(
    int unit,
    int flags,
    bcm_stat_control_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_stat_control_set_verify(unit, flags, type, arg));

    switch (type)
    {
        case bcmStatControlStifFcEnable:
            SHR_IF_ERR_EXIT(dnx_stif_mgmt_flow_control_enable_set(unit, flags, arg));
            break;
        case bcmStatControlCounterThreadEnable:
            SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_background_collection_set(unit, arg));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, " invalid stat control type (=%d)\n", type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify function for BCM-API dnx_stat_control_get
 */
static shr_error_e
dnx_stat_control_get_verify(
    int unit,
    int flags,
    bcm_stat_control_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(arg, _SHR_E_PARAM, "arg");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Generic stat control get API function, used for all kinds of stat sub-modules (CRPS/Stif/mib).
 *
 * Use cases:
 * 0. stif - get if Flow Control in stif is Enabled/disabled.
 * Parameters:
 * - flags - must be 0
 * - type - bcmStatControlStifFcEnable
 * - arg - TRUE/FALSE indicates flow control is
 * enabled/disabled
 * 1. crps - get if crps bg thread operation Enable/disable.
 * Parameters:
 * - type - bcmStatControlCounterThreadEnable
 * - arg -  TRUE/FALSE indicates enable/disable bg thread
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stat_control_get(
    int unit,
    int flags,
    bcm_stat_control_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_stat_control_get_verify(unit, flags, type, arg));

    switch (type)
    {
        case bcmStatControlStifFcEnable:
            SHR_IF_ERR_EXIT(dnx_stif_mgmt_flow_control_enable_get(unit, flags, arg));
            break;
        case bcmStatControlCounterThreadEnable:
            SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_background_collection_get(unit, arg));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, " invalid stat control type (=%d)\n", type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Use case 0:
 *  STIF:
 *  enable or disable header-truncate compensation
 *  Use case 1:
 * CRPS:
 * for IRPP/ITM set which IRPP/ITM mask will be used - enable or
 * disable header-truncate compensation
 * \param [in] unit -  Unit-ID
 * \param [in] flags -  flags
 * \param [in] key - source info
 * \param [in] select_type - header adjust select type
 * \param [in] enable - enable/disable header-truncate
 *        compensation
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stat_pkt_size_adjust_select_set(
    int unit,
    uint32 flags,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_pkt_size_adjust_select_type_t select_type,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /** Use case 0: STIF */
    if (flags == BCM_STAT_EXTERNAL)
    {
        /** STIF pkt size adjust */
        SHR_INVOKE_VERIFY_DNX(dnx_stif_mgmt_pkt_size_adjust_select_set_verify(unit, key, select_type, enable));
        SHR_IF_ERR_EXIT(dnx_stif_mgmt_pkt_size_adjust_select_set(unit, key, select_type, enable));
    }
    /** Use case 1: CRPS */
    else
    {
        /** CRPS pkt size adjust */
        SHR_INVOKE_VERIFY_DNX(dnx_crps_ingress_pkt_size_adjust_select_set_verify
                              (unit, flags, key, select_type, enable));
        SHR_IF_ERR_EXIT(dnx_crps_ingress_pkt_size_adjust_select_set(unit, flags, key, select_type, enable));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Use case 0:
 *  STIF:
 *  get if header-truncate compensation is enabled
 * or disabled
 * Use case 1:
 * CRPS: for IRPP/ITM get which IRPP/ITM mask is be used - is
 * enabled or disabled header-truncate compensation
 * \param [in] unit -  Unit-ID
 * \param [in] flags -  flags
 * \param [in] key - source info
 * \param [in] select_type - header adjust select type
 * \param [out] enable - enabled/disabled header-truncate
 *        compensation
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stat_pkt_size_adjust_select_get(
    int unit,
    uint32 flags,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_pkt_size_adjust_select_type_t select_type,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /** Use case 0 : STIF */
    if (flags == BCM_STAT_EXTERNAL)
    {
        /** STIF pkt size adjust */
        SHR_INVOKE_VERIFY_DNX(dnx_stif_mgmt_pkt_size_adjust_select_verify(unit, key, select_type));
        SHR_IF_ERR_EXIT(dnx_stif_mgmt_pkt_size_adjust_select_get(unit, key, select_type, enable));
    }
     /** Use case 1: CRPS */
    else
    {
        /** CRPS pkt size adjust */
        SHR_INVOKE_VERIFY_DNX(dnx_crps_ingress_pkt_size_adjust_select_get_verify
                              (unit, flags, key, select_type, enable));
        SHR_IF_ERR_EXIT(dnx_crps_ingress_pkt_size_adjust_select_get(unit, flags, key, select_type, enable));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_stat_mgmt_reject_filter_masks_init(
    int unit)
{
    const dnx_data_stat_drop_reasons_drop_reasons_groups_t *values;
    int instance_id, drop_reasons_count = 0, flags = 0;
    bcm_cosq_drop_reason_t drop_reasons_array[DNX_DATA_MAX_STAT_DROP_REASONS_NOF_DROP_REASONS];
    const dnx_data_stat_drop_reasons_group_drop_reason_index_t *group_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** set filter masks values */
    for (instance_id = 0; instance_id < bcmStatCounterGroupFilterCount; instance_id++)
    {
        group_index = dnx_data_stat.drop_reasons.group_drop_reason_index_get(unit, instance_id);
        if (group_index->index == DNX_STAT_NO_SUPPORT && instance_id == bcmStatCounterGroupFilterLatencyDrop)
        {
            continue;
        }
        /** do not configure the UserDefined group */
        if (instance_id == bcmStatCounterGroupFilterUserDefinedGroup)
        {
            continue;
        }
        values = dnx_data_stat.drop_reasons.drop_reasons_groups_get(unit, instance_id);
        drop_reasons_count = 0;
        while (bcmCosqDropReasonRejectCount != values->drop_reasons[drop_reasons_count])
        {
            drop_reasons_array[drop_reasons_count] = values->drop_reasons[drop_reasons_count];
            drop_reasons_count++;
        }
        /** fill drop reasons array for current group */
        SHR_IF_ERR_EXIT(bcm_stat_group_drop_reasons_set
                        (unit, flags, instance_id, drop_reasons_array, drop_reasons_count));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Use case 0:
 *  STIF
 *  Activate given filter group and set filter criteria group to
 *  be filtered in/out
 *  Use case 1:
 *  CRPS
 *  Activate given filter group and set filter criteria group to
 *  be filtered in/out
 *  For both - there are 7 filter groups, multiple groups can be
 *  activated, each group represents different activated bits
 *  from the bitmap of 45 reject drop reasons
 *
 * \param [in] unit -  Unit-ID
 * \param [in] flags -  flags
 * \param [in] key - core,source,command_id
 * \param [in] filter - filter group
 * \param [in] is_active - activate/deactivate the filter group
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stat_counter_filter_group_set(
    int unit,
    uint32 flags,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_counter_group_filter_t filter,
    int is_active)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /** Use case 0 : STIF */
    if (flags == BCM_STAT_EXTERNAL)
    {
        SHR_INVOKE_VERIFY_DNX(dnx_stif_mgmt_filter_group_set_verify(unit, key, filter, is_active));
        SHR_IF_ERR_EXIT(dnx_stif_mgmt_filter_group_set(unit, key, filter, is_active));
    }
    /** Use case 1 : CRPS */
    else
    {
        SHR_INVOKE_VERIFY_DNX(dnx_crps_mgmt_filter_group_set_verify(unit, flags, key, filter, is_active));
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_filter_group_set(unit, flags, key, filter, is_active));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Use case 0:
 *  STIF
 *  Determine if given filter criteria is active or not
 *  Use case 1:
 *  CRPS
 *  Determine if given filter criteria is active or not
 *  For both there are 7 filter groups, multiple groups can be
 *  activate, each group represents different activated bits
 *  from the bitmap of 45 reject drop reasons
 *
 * \param [in] unit -  Unit-ID
 * \param [in] flags -  flags
 * \param [in] key - core,source,command_id
 * \param [in] filter - filter group
 * \param [out] is_active - is the filter group active
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stat_counter_filter_group_get(
    int unit,
    uint32 flags,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_counter_group_filter_t filter,
    int *is_active)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /** Use case 0 : STIF */
    if (flags == BCM_STAT_EXTERNAL)
    {
        SHR_INVOKE_VERIFY_DNX(dnx_stif_mgmt_filter_group_verify(unit, key, filter));
        SHR_IF_ERR_EXIT(dnx_stif_mgmt_filter_group_get(unit, key, filter, is_active));
    }
    /** Use case 1 : CRPS */
    else
    {
        SHR_INVOKE_VERIFY_DNX(dnx_crps_mgmt_filter_group_get_verify(unit, flags, key, filter, is_active));
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_filter_group_get(unit, flags, key, filter, is_active));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - API return the encoded stat_id represnt end to end latency per port.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] flags -  flags
 * \param [in] key - the parameters to be decoded into stat_id
 * \param [out] stat_id - encoded stat-id
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stat_latency_port_stat_id_get(
    int unit,
    int flags,
    bcm_stat_latency_port_stat_id_key_t * key,
    int *stat_id)
{
    /** array due to coverity workarround */
    uint32 dsp_pp_port[1];
    bcm_core_t core_id;
    /** coverity workarround */
    int traffic_class[1];
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_NULL_CHECK(key, _SHR_E_PARAM, "key");
    SHR_NULL_CHECK(stat_id, _SHR_E_PARAM, "stat_id");

    SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, key->port, &core_id, dsp_pp_port));
    traffic_class[0] = key->traffic_class;

    *stat_id = 0;
    /** enclode the stat id according to HW: For JR2: {0x0(8),fabric_or_egress_mc(1),ftmh_TC(3),out_pp_dsp(8)} */
    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(dsp_pp_port, dnx_data_crps.latency.stat_id_port_offset_get(unit),
                                                   utilex_log2_round_up(dnx_data_port.general.nof_tm_ports_get(unit)),
                                                   (uint32 *) stat_id));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                    ((uint32 *) traffic_class, dnx_data_crps.latency.stat_id_tc_offset_get(unit),
                     DNX_STAT_TRAFFIC_CLASS_SIZE, (uint32 *) stat_id));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                    ((uint32 *) &key->is_multicast, dnx_data_crps.latency.stat_id_multicast_offset_get(unit),
                     DNX_STAT_MULTICAST_SIZE, (uint32 *) stat_id));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   group drop reasons get - verify function
* \param [in] unit - unit id
* \param [in] flags - flags
* \param [in] group - group id
* \param [in] max_drop_reason_count - max drop reasons
* \param [out] drop_reasons_array - array of drop reasons
* \param [out] drop_reasons_count - drop reasons count
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static int
dnx_stat_group_drop_reasons_get_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_group_filter_t group,
    int max_drop_reason_count,
    bcm_cosq_drop_reason_t * drop_reasons_array,
    int *drop_reasons_count)
{
    const dnx_data_stat_drop_reasons_group_drop_reason_index_t *group_index;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(drop_reasons_array, _SHR_E_PARAM, "drop_reasons_array");
    SHR_NULL_CHECK(drop_reasons_count, _SHR_E_PARAM, "drop_reasons_count");
    if (group < 0 || group >= bcmStatCounterGroupFilterCount)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Incorrect group=%d for API bcm_stat_group_drop_reasons_get.\n", group);
    }
    group_index = dnx_data_stat.drop_reasons.group_drop_reason_index_get(unit, group);
    if (group_index->index == DNX_STAT_NO_SUPPORT && group == bcmStatCounterGroupFilterLatencyDrop)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Group=%d for API bcm_stat_group_drop_reasons_get is not supported for this device type.\n",
                     group);
    }
    if (max_drop_reason_count < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Incorrect max_drop_reason_count=%d for API bcm_stat_group_drop_reasons_get.\n",
                     max_drop_reason_count);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   group drop reasons get - dbal handler function
* \param [in] unit - unit id
* \param [in] flags - flags
* \param [in] group - group id
* \param [in] max_drop_reason_count - max drop reasons
* \param [out] drop_reasons_array - array of drop reasons
* \param [out] drop_reasons_count - drop reasons count
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static int
dnx_stat_group_drop_reasons_get_dbal(
    int unit,
    uint32 flags,
    bcm_stat_counter_group_filter_t group,
    int max_drop_reason_count,
    bcm_cosq_drop_reason_t * drop_reasons_array,
    int *drop_reasons_count)
{
    uint32 entry_handle_id, is_drop_reason_set = 0;
    int drop_reason_id, nof_set_drop_reasons = 0;
    const dnx_data_stat_drop_reasons_drop_reason_index_t *drop_reason_index;
    const dnx_data_stat_drop_reasons_group_drop_reason_index_t *group_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    group_index = dnx_data_stat.drop_reasons.group_drop_reason_index_get(unit, group);
    /** read drop reasons set for the given grooup */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STAT_RJCT_FILTER_GROUP, &entry_handle_id));
    for (drop_reason_id = 0; drop_reason_id < bcmCosqDropReasonRejectCount; drop_reason_id++)
    {
        drop_reason_index = dnx_data_stat.drop_reasons.drop_reason_index_get(unit, drop_reason_id);
        if (drop_reason_index->index == DNX_STAT_NO_SUPPORT)
        {
            /** drop reason not supported */
            continue;
        }
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DROP_REASON, drop_reason_index->index);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STAT_DROP_REASON_RJCT_MASK, group_index->index,
                                   &is_drop_reason_set);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        if (TRUE == is_drop_reason_set)
        {
            if (nof_set_drop_reasons <= max_drop_reason_count)
            {
                drop_reasons_array[nof_set_drop_reasons] = drop_reason_id;
            }
            nof_set_drop_reasons++;
        }
    }
    *drop_reasons_count = nof_set_drop_reasons;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
bcm_dnx_stat_group_drop_reasons_get(
    int unit,
    uint32 flags,
    bcm_stat_counter_group_filter_t group,
    int max_drop_reason_count,
    bcm_cosq_drop_reason_t * drop_reasons_array,
    int *drop_reasons_count)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_stat_group_drop_reasons_get_verify
                          (unit, flags, group, max_drop_reason_count, drop_reasons_array, drop_reasons_count));
    SHR_IF_ERR_EXIT(dnx_stat_group_drop_reasons_get_dbal
                    (unit, flags, group, max_drop_reason_count, drop_reasons_array, drop_reasons_count));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   group drop reasons set - verify function
* \param [in] unit - unit id
* \param [in] flags - flags
* \param [in] group - group id
* \param [in] drop_reasons_array - array of drop reasons
* \param [in] drop_reasons_count - drop reasons count
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static int
dnx_stat_group_drop_reasons_set_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_group_filter_t group,
    bcm_cosq_drop_reason_t * drop_reasons_array,
    int drop_reasons_count)
{
    int drop_reason_id = 0;
    const dnx_data_stat_drop_reasons_group_drop_reason_index_t *group_index;
    const dnx_data_stat_drop_reasons_drop_reason_index_t *drop_reason_index;
    SHR_FUNC_INIT_VARS(unit);

    if (drop_reasons_count != 0)
    {
        SHR_NULL_CHECK(drop_reasons_array, _SHR_E_PARAM, "drop_reasons_array");
    }
    if (group < 0 || group >= bcmStatCounterGroupFilterCount)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Incorrect group=%d for API bcm_stat_group_drop_reasons_get.\n", group);
    }
    group_index = dnx_data_stat.drop_reasons.group_drop_reason_index_get(unit, group);
    if (group_index->index == DNX_STAT_NO_SUPPORT && group == bcmStatCounterGroupFilterLatencyDrop)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Group=%d for API bcm_stat_group_drop_reasons_set is not supported for this device type.\n",
                     group);
    }
    if (drop_reasons_count < 0 || drop_reasons_count >= dnx_data_stat.drop_reasons.nof_drop_reasons_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Incorrect drop_reasons_count=%d for API bcm_stat_group_drop_reasons_set.\n",
                     drop_reasons_count);
    }
    for (drop_reason_id = 0; drop_reason_id < drop_reasons_count; drop_reason_id++)
    {
        if (drop_reasons_array[drop_reason_id] < 0
            || drop_reasons_array[drop_reason_id] >= bcmCosqDropReasonRejectCount)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Incorrect drop reason=%d for API bcm_stat_group_drop_reasons_set.\n",
                         drop_reasons_array[drop_reason_id]);
        }
        drop_reason_index = dnx_data_stat.drop_reasons.drop_reason_index_get(unit, drop_reason_id);
        if (drop_reason_index->index == DNX_STAT_NO_SUPPORT
            && drop_reasons_array[drop_reason_id] == bcmCosqDropReasonLatencyReject)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Drop Reason=%d for API bcm_stat_group_drop_reasons_set is not supported for this device type.\n",
                         drop_reasons_array[drop_reason_id]);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   group drop reasons set - dbal handler function
* \param [in] unit - unit id
* \param [in] flags - flags
* \param [in] group - group id
* \param [in] drop_reasons_array - array of drop reasons
* \param [in] drop_reasons_count - drop reasons count
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static int
dnx_stat_group_drop_reasons_set_dbal(
    int unit,
    uint32 flags,
    bcm_stat_counter_group_filter_t group,
    bcm_cosq_drop_reason_t * drop_reasons_array,
    int drop_reasons_count)
{
    uint32 entry_handle_id;
    int drop_reason_id, set_drop_reason = 1, clear_drop_reason = 0;
    const dnx_data_stat_drop_reasons_drop_reason_index_t *drop_reason_index;
    const dnx_data_stat_drop_reasons_group_drop_reason_index_t *group_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** configure drop reason group  */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STAT_RJCT_FILTER_GROUP, &entry_handle_id));
    /** first clear the group */
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_DROP_REASON, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_DROP_REASON_RJCT_MASK, group,
                                 clear_drop_reason);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    group_index = dnx_data_stat.drop_reasons.group_drop_reason_index_get(unit, group);
    /** set the given drop reasons accordingly, if given array is
     *  empty - the group will remain cleared */
    for (drop_reason_id = 0; drop_reason_id < drop_reasons_count; drop_reason_id++)
    {
        drop_reason_index = dnx_data_stat.drop_reasons.drop_reason_index_get(unit, drop_reasons_array[drop_reason_id]);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DROP_REASON, drop_reason_index->index);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_DROP_REASON_RJCT_MASK, group_index->index,
                                     set_drop_reason);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
bcm_dnx_stat_group_drop_reasons_set(
    int unit,
    uint32 flags,
    bcm_stat_counter_group_filter_t group,
    bcm_cosq_drop_reason_t * drop_reasons_array,
    int drop_reasons_count)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_stat_group_drop_reasons_set_verify
                          (unit, flags, group, drop_reasons_array, drop_reasons_count));
    SHR_IF_ERR_EXIT(dnx_stat_group_drop_reasons_set_dbal(unit, flags, group, drop_reasons_array, drop_reasons_count));

exit:
    SHR_FUNC_EXIT;
}
