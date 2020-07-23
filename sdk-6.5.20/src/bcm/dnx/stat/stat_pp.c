/** \file stat_pp.c
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
#error "BSL_LOG_MODULE redefined"
#endif /* */
#define BSL_LOG_MODULE BSL_LS_BCMDNX_CNT

/*
 * Include files.
 * {
 */
#include <shared/utilex/utilex_bitstream.h>
#include <shared/shrextend/shrextend_error.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/stat.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_crps.h>
#include <src/bcm/dnx/stat/crps/crps_mgmt_internal.h>
#include <bcm_int/dnx/algo/stat_pp/algo_stat_pp.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_stat_pp_access.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_dbal_api.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/stat/crps/crps_mgmt.h>
#include <shared/trunk.h>
#include <bcm/trunk.h>
#include <bcm_int/dnx/trunk/trunk.h>
#include <bcm_int/dnx/stat/stat_pp.h>
#include <bcm_int/dnx/qos/qos.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stat.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>

/*
 * }
 */

/*
 * MACROs
 * {
 */
#define IS_REVERSED() dnx_data_stat.stat_pp.etpp_reversed_stat_cmd_get(unit)
#define STAT_PP_IS_ETPP_METERING_PROFILE(profile_id) (profile_id > STAT_PP_ETPP_SWITCH_PROFILE_MAX_PROFILE) ? TRUE : FALSE

/**
 *  \brief Max size of the array used for conversion of header type from
 *  bcm to dbal enum
 */
#define STAT_PP_MAX_CONVERT_ARRAY_SIZE 20

/**
 *  \brief Metadata max value
 */
#define STAT_PP_MAX_METADATA_VALUE 7

#define STAT_PP_SW_STATE_RESOURCE_EXEC_BY_ENGINE_PROFILE(engine,profile_id,etpp_metering_profile,exec) \
                engine == bcmStatCounterInterfaceIngressReceivePp ? stat_pp_db.stat_pp_irpp_profile_id.exec :\
                engine == bcmStatCounterInterfaceEgressReceivePp ? stat_pp_db.stat_pp_erpp_profile_id.exec :\
                (IS_REVERSED() && etpp_metering_profile) ? stat_pp_db.stat_pp_etpp_metering_profile_id.exec :\
                IS_REVERSED() ? stat_pp_db.stat_pp_etpp_counting_profile_id.exec :\
                stat_pp_db.stat_pp_etpp_profile_id.exec \

/*
  * }
  */

/**
 * \brief - Verify function for pp_profile_create API
 *
 * \param [in] unit - Relevant unit
 * \param [in] flags - flags that were passed to the API
 * \param [in] profile_id - used in case of REPLACE flag being
 *        set, in which case we check if the profile already
 *        exists before replacing it
 * \param [in] engine_source - counting source (IRPP, ERPP,
 *        ETPP)
 * \param [in] stat_pp_profile_info - A struct containing the
 *        profile info to be written
 *
 * \return
 *   shr_error_e, negative in case of an error. For example:
 *   REPLACE flag is set, and the profile isn't found
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_stat_pp_profile_create_verify(
    int unit,
    int flags,
    int *profile_id,
    bcm_stat_counter_interface_type_t engine_source,
    bcm_stat_pp_profile_info_t * stat_pp_profile_info)
{
    uint8 is_allocated;
    uint8 is_reversed = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(profile_id, _SHR_E_PARAM, "profile_id");
    SHR_NULL_CHECK(stat_pp_profile_info, _SHR_E_PARAM, "stat_pp_profile_info");

    is_reversed = dnx_data_stat.stat_pp.etpp_reversed_stat_cmd_get(unit);

    switch (engine_source)
    {
        case bcmStatCounterInterfaceIngressReceivePp:
            if (stat_pp_profile_info->meter_command_id != STAT_PP_INGRESS_METER_COMMAND_ID_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "metering is invalid for ingress, meter_command_id must be 0 (was %d)",
                             stat_pp_profile_info->meter_command_id);
            }
            if (stat_pp_profile_info->meter_qos_map_id != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "meter_qos_map_id is invalid for ingress, must be 0 (was %d)",
                             stat_pp_profile_info->meter_qos_map_id);
            }
            if ((stat_pp_profile_info->is_meter_enable) &&
                ((stat_pp_profile_info->counter_command_id < STAT_PP_METER_MIN_ID) ||
                 (stat_pp_profile_info->counter_command_id > STAT_PP_METER_IRPP_MAX_ID)))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Invalid counter_command_id %d used for metering (min value: %d, max value %d)",
                             stat_pp_profile_info->counter_command_id, STAT_PP_METER_MIN_ID, STAT_PP_METER_IRPP_MAX_ID);
            }

            SHR_RANGE_VERIFY(stat_pp_profile_info->ingress_tunnel_metadata_size, 0,
                             STAT_PP_IRPP_META_DATA_UPDATE_MAX_SIZE, _SHR_E_PARAM,
                             "Invalid ingress_tunnel_metadata_size %d (min value: %d, max value %d)",
                             stat_pp_profile_info->ingress_tunnel_metadata_size, 0,
                             STAT_PP_IRPP_META_DATA_UPDATE_MAX_SIZE);

            SHR_RANGE_VERIFY(stat_pp_profile_info->ingress_tunnel_metadata_start_bit, 0,
                             STAT_PP_IRPP_META_DATA_UPDATE_MAX_SHIFT, _SHR_E_PARAM,
                             "Invalid ingress_tunnel_metadata_start_bit %d (min value: %d, max value %d)",
                             stat_pp_profile_info->ingress_tunnel_metadata_start_bit, 0,
                             STAT_PP_IRPP_META_DATA_UPDATE_MAX_SHIFT);

            SHR_RANGE_VERIFY(stat_pp_profile_info->ingress_tunnel_metadata_size +
                             stat_pp_profile_info->ingress_tunnel_metadata_start_bit, 0,
                             STAT_PP_IRPP_META_DATA_MAX_SIZE, _SHR_E_PARAM,
                             "Invalid ingress_tunnel_metadata_size/shift combination (%d, %d) (max size+shift %d)",
                             stat_pp_profile_info->ingress_tunnel_metadata_size,
                             stat_pp_profile_info->ingress_tunnel_metadata_start_bit, STAT_PP_IRPP_META_DATA_MAX_SIZE);

            SHR_RANGE_VERIFY(stat_pp_profile_info->ingress_forward_metadata_size, 0,
                             STAT_PP_IRPP_META_DATA_UPDATE_MAX_SIZE, _SHR_E_PARAM,
                             "Invalid ingress_forward_metadata_size %d (min value: %d, max value %d)",
                             stat_pp_profile_info->ingress_forward_metadata_size, 0,
                             STAT_PP_IRPP_META_DATA_UPDATE_MAX_SIZE);

            SHR_RANGE_VERIFY(stat_pp_profile_info->ingress_forward_metadata_start_bit, 0,
                             STAT_PP_IRPP_META_DATA_UPDATE_MAX_SHIFT, _SHR_E_PARAM,
                             "Invalid ingress_forward_metadata_start_bit %d (min value: %d, max value %d)",
                             stat_pp_profile_info->ingress_forward_metadata_start_bit, 0,
                             STAT_PP_IRPP_META_DATA_UPDATE_MAX_SHIFT);
            SHR_RANGE_VERIFY(stat_pp_profile_info->ingress_forward_metadata_size +
                             stat_pp_profile_info->ingress_forward_metadata_start_bit, 0,
                             STAT_PP_IRPP_META_DATA_MAX_SIZE, _SHR_E_PARAM,
                             "Invalid ingress_forward_metadata_size/shift combination (%d, %d) (max size+shift %d)",
                             stat_pp_profile_info->ingress_forward_metadata_size,
                             stat_pp_profile_info->ingress_forward_metadata_start_bit, STAT_PP_IRPP_META_DATA_MAX_SIZE);

            SHR_RANGE_VERIFY(stat_pp_profile_info->ingress_fwd_plus_one_metadata_size, 0,
                             STAT_PP_IRPP_META_DATA_UPDATE_MAX_SIZE, _SHR_E_PARAM,
                             "Invalid ingress_fwd_plus_one_metadata_size %d (min value: %d, max value %d)",
                             stat_pp_profile_info->ingress_fwd_plus_one_metadata_size, 0,
                             STAT_PP_IRPP_META_DATA_UPDATE_MAX_SIZE);

            SHR_RANGE_VERIFY(stat_pp_profile_info->ingress_fwd_plus_one_metadata_start_bit, 0,
                             STAT_PP_IRPP_META_DATA_UPDATE_MAX_SHIFT, _SHR_E_PARAM,
                             "Invalid ingress_fwd_plus_one_metadata_start_bit %d (min value: %d, max value %d)",
                             stat_pp_profile_info->ingress_fwd_plus_one_metadata_start_bit, 0,
                             STAT_PP_IRPP_META_DATA_UPDATE_MAX_SHIFT);
            SHR_RANGE_VERIFY(stat_pp_profile_info->ingress_fwd_plus_one_metadata_size +
                             stat_pp_profile_info->ingress_fwd_plus_one_metadata_start_bit, 0,
                             STAT_PP_IRPP_META_DATA_MAX_SIZE, _SHR_E_PARAM,
                             "Invalid ingress_fwd_plus_one_metadata_size/shift combination (%d, %d) (max size+shift %d)",
                             stat_pp_profile_info->ingress_fwd_plus_one_metadata_size,
                             stat_pp_profile_info->ingress_fwd_plus_one_metadata_start_bit,
                             STAT_PP_IRPP_META_DATA_MAX_SIZE);
            break;

        case bcmStatCounterInterfaceEgressReceivePp:
            
            SHR_ERR_EXIT(_SHR_E_PARAM, "ERPP counting engine is not supported");
            break;
        case bcmStatCounterInterfaceEgressTransmitPp:
            if (stat_pp_profile_info->is_fp_cs_var != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "is_fp_cs_var must be off for egress (was %d)",
                             stat_pp_profile_info->is_fp_cs_var);
            }

            if ((stat_pp_profile_info->is_meter_enable) &&
                ((stat_pp_profile_info->meter_command_id < STAT_PP_METER_MIN_ID) ||
                 (stat_pp_profile_info->meter_command_id > STAT_PP_METER_ETPP_MAX_ID)))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Invalid meter_command_id %d used for metering (min value: %d, max value %d)",
                             stat_pp_profile_info->meter_command_id, STAT_PP_METER_MIN_ID, STAT_PP_METER_ETPP_MAX_ID);
            }
            if ((is_reversed) && (_SHR_IS_FLAG_SET(flags, BCM_STAT_PP_PROFILE_WITH_ID)))
            {
                if ((stat_pp_profile_info->is_meter_enable)
                    && (*profile_id < dnx_data_stat.stat_pp.etpp_metering_start_profile_value_get(unit)))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal profile id given:%d. Should be above %d",
                                 *profile_id, dnx_data_stat.stat_pp.etpp_metering_start_profile_value_get(unit));
                }
                else if (!(stat_pp_profile_info->is_meter_enable)
                         && (*profile_id > dnx_data_stat.stat_pp.max_etpp_counting_profile_value_get(unit)))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal profile id given:%d. Should be between 1 and %d",
                                 *profile_id, dnx_data_stat.stat_pp.max_etpp_counting_profile_value_get(unit));
                }
            }
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "unknown engine_source option");
            break;
    }

    if ((stat_pp_profile_info->stat_object_type < 0) ||
        (stat_pp_profile_info->stat_object_type >= BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stat object type given:%d. Should be between 0 and %d",
                     stat_pp_profile_info->stat_object_type, BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES);
    }

    if (_SHR_IS_FLAG_SET(flags, BCM_STAT_PP_PROFILE_REPLACE))
    {
        SHR_IF_ERR_EXIT(STAT_PP_SW_STATE_RESOURCE_EXEC_BY_ENGINE_PROFILE
                        (engine_source, *profile_id, stat_pp_profile_info->is_meter_enable,
                         is_allocated(unit, *profile_id, &is_allocated)));
        if (is_allocated == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Profile id %d doesn't exist\n", *profile_id);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for get, delete pp profiles APIs
 * Validate that the profile exists before get or delete it
 *
 * \param [in] unit - Relevant unit
 * \param [in] engine_source - counting source (IRPP, ERPP,
 *        ETPP)
 * \param [in] profile_id - statistics profile id
 *
 * \return
 *   shr_error_e, Negative in case of an error. For example -
 *   profile is not found
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_stat_pp_profile_get_delete_verify(
    int unit,
    uint32 engine_source,
    uint32 profile_id)
{
    uint8 is_allocated;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(STAT_PP_SW_STATE_RESOURCE_EXEC_BY_ENGINE_PROFILE
                    (engine_source, profile_id, STAT_PP_IS_ETPP_METERING_PROFILE(profile_id),
                     is_allocated(unit, profile_id, &is_allocated)));

    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Profile id %d doesn't exist\n", profile_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set statistic profile properties in relevant dbal
 *        tables:
 *          DBAL_TABLE_IRPP_STATISTIC_PROFILE_INFO
 *          DBAL_TABLE_ERPP_STATISTIC_PROFILE_INFO
 *          DBAL_TABLE_ETPP_STATISTIC_PROFILE_INFO
 * \param [in] unit - Relevant unit
 * \param [in] stat_pp_profile_info - A struct containing the
 *        relevant profile info to be written
 * \param [in] stat_pp_profile - profile id, serves as key to
 *        the dbal tables
 * \param [in] engine_source - counting engine, used to
 *        distinguish between the tables
 *
 * \return
 *    Negative in case of an error. See \ref shr_error_e,
 *     for example: dbal failure
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
dnx_stat_pp_profile_hw_set(
    int unit,
    bcm_stat_pp_profile_info_t * stat_pp_profile_info,
    int stat_pp_profile,
    bcm_stat_counter_interface_type_t engine_source)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write values to stat_pp dbal tables according to type
     */
    switch (engine_source)
    {
        case bcmStatCounterInterfaceIngressReceivePp:
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STAT_PP_IRPP_PROFILE_INFO, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_CMD, stat_pp_profile);
            dbal_entry_value_field32_set(unit,
                                         entry_handle_id,
                                         DBAL_FIELD_STAT_IF_OBJECT_ID,
                                         INST_SINGLE, stat_pp_profile_info->counter_command_id);
            dbal_entry_value_field32_set(unit,
                                         entry_handle_id,
                                         DBAL_FIELD_STAT_IF_TYPE_ID, INST_SINGLE,
                                         stat_pp_profile_info->stat_object_type);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_METER, INST_SINGLE,
                                         stat_pp_profile_info->is_meter_enable);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAPPING_REQUIRED, INST_SINGLE,
                                         stat_pp_profile_info->is_fp_cs_var);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_METADATA_SIZE, INST_SINGLE,
                                         stat_pp_profile_info->ingress_tunnel_metadata_size);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_METADATA_SHIFT, INST_SINGLE,
                                         stat_pp_profile_info->ingress_tunnel_metadata_start_bit);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_METADATA_SIZE, INST_SINGLE,
                                         stat_pp_profile_info->ingress_forward_metadata_size);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_METADATA_SHIFT, INST_SINGLE,
                                         stat_pp_profile_info->ingress_forward_metadata_start_bit);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_PLUS_ONE_METADATA_SIZE, INST_SINGLE,
                                         stat_pp_profile_info->ingress_fwd_plus_one_metadata_size);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_PLUS_ONE_METADATA_SHIFT, INST_SINGLE,
                                         stat_pp_profile_info->ingress_fwd_plus_one_metadata_start_bit);
            break;
        case bcmStatCounterInterfaceEgressTransmitPp:
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STAT_PP_ETPP_PROFILE_INFO, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_CMD, stat_pp_profile);
            dbal_entry_value_field32_set(unit,
                                         entry_handle_id,
                                         DBAL_FIELD_STAT_IF_OBJECT_ID,
                                         INST_SINGLE, stat_pp_profile_info->counter_command_id);
            dbal_entry_value_field32_set(unit,
                                         entry_handle_id,
                                         DBAL_FIELD_STAT_IF_TYPE_ID, INST_SINGLE,
                                         stat_pp_profile_info->stat_object_type);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_METER, INST_SINGLE,
                                         stat_pp_profile_info->is_meter_enable);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_METER_INTERFACE, INST_SINGLE,
                                         stat_pp_profile_info->meter_command_id);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_METER_QOS_PROFILE, INST_SINGLE,
                                         DNX_QOS_MAP_PROFILE_GET(stat_pp_profile_info->meter_qos_map_id));
            break;

        case bcmStatCounterInterfaceEgressReceivePp:
            
            SHR_ERR_EXIT(_SHR_E_PARAM, "ERPP counting engine is not supported");
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "unknown engine_source option");
            break;
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * For protection fec - we mark TAKE_LEFT + single indirect counters, in order to use one stat id for both primary
     * and backup fecs (backup fec stat id will be primary fec stat id - 1)
     */
    if (engine_source == bcmStatCounterInterfaceIngressReceivePp)
    {
        if (stat_pp_profile_info->is_protection_fec)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR
                            (unit, DBAL_TABLE_STAT_PP_IRPP_FEC_ECMP_STATISTICS_PROFILE_MAP, entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_STAT_OBJECT_PROFILE, stat_pp_profile);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_TAKE_PRIMARY, INST_SINGLE, TRUE);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_STATISTIC_PROFILE_MAP, INST_SINGLE,
                                         DBAL_ENUM_FVAL_FEC_STATISTIC_PROFILE_MAP_SINGLE_INDIRECT_COUNTERS);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See stat_pp.h file for summary
 */
shr_error_e
dnx_stat_pp_result_type_verify(
    int unit,
    dbal_tables_e dbal_table_id,
    uint32 dbal_result_type)
{
    dbal_table_field_info_t dbal_info;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Make sure that the result type chosen has the statistics fields inside it
     */
    rv = (dbal_tables_field_info_get_no_err
          (unit, dbal_table_id, DBAL_FIELD_STAT_CMD, 0,
           dbal_result_type, 0, &dbal_info) == _SHR_E_NONE)
        ||
        (dbal_tables_field_info_get_no_err
         (unit, dbal_table_id, DBAL_FIELD_STAT_OBJECT_CMD, 0, dbal_result_type, 0, &dbal_info) == _SHR_E_NONE);

    if (rv)
    {
        SHR_EXIT();
    }

    SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify gport stat set api: crps range validation,
 *          core mode validation, profile range and engine source
 *
 * \param [in] unit - Relevant unit
 * \param [in] gport - physical or logical port
 * \param [in] core_id - core id
 * \param [in] engine_source - counting source (IRPP, ERPP,
 *        ETPP)
 * \param [in] stat_info - statistic information per object
 *
 * \return
 *   Negative in case of an error. See \ref shr_error_e,
 *           for example: unknown engine_source
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_stat_pp_gport_set_verify(
    int unit,
    bcm_gport_t gport,
    bcm_core_t core_id,
    bcm_stat_counter_interface_type_t engine_source,
    bcm_stat_pp_info_t stat_info)
{
    dbal_core_mode_e core_mode;
    uint8 is_physical_port;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint32 lif_flags;
    uint8 is_ingress;
    dbal_tables_e dbal_table_id;
    uint32 dbal_table_result_type;
    dbal_table_field_info_t dbal_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, gport, &is_physical_port));

    if (engine_source != STAT_PP_PROFILE_ENGINE_GET(stat_info.stat_pp_profile))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Engine source %d is inconsistent with stat_info.stat_pp_profile engine %d",
                     engine_source, stat_info.stat_pp_profile);
    }

    is_ingress = FALSE;
    switch (engine_source)
    {
        case bcmStatCounterInterfaceIngressReceivePp:
            is_ingress = TRUE;
            break;
        case bcmStatCounterInterfaceEgressTransmitPp:
            break;
        case bcmStatCounterInterfaceEgressReceivePp:
            
            SHR_ERR_EXIT(_SHR_E_PARAM, "ERPP counting engine is not supported");
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "unknown engine_source option");
            break;
    }

    SHR_IF_ERR_EXIT(dnx_stat_pp_crps_verify
                    (unit, core_id, stat_info.stat_id, stat_info.stat_pp_profile, engine_source));

    if (is_physical_port == 0)
    {
        if (BCM_GPORT_IS_VLAN_PORT(gport) && BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_MATCH(gport))
        {
            /*
             * The LIF is Egress Virtual, only valid for ETPP:
             */
            if (engine_source != bcmStatCounterInterfaceEgressTransmitPp)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error, gport = 0x%08X is Egress Virtual but engine_source = %d is not bcmStatCounterInterfaceEgressTransmitPp (%d)\n",
                             gport, engine_source, bcmStatCounterInterfaceEgressTransmitPp);
            }

            
        }
        else
        {
            if (BCM_GPORT_IS_MPLS_PORT(gport))
            {
                
                uint8 is_stat_cmd_valid;

                /** get lifs */
                lif_flags =
                    is_ingress ? DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS |
                    DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS :
                    DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS;
                SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, lif_flags, &gport_hw_resources));
                /*
                 * Check if statistics are valid on the entry
                 */
                SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_valid_field
                                (unit, core_id,
                                 (is_ingress) ? gport_hw_resources.local_in_lif : gport_hw_resources.local_out_lif,
                                 (is_ingress) ? gport_hw_resources.
                                 inlif_dbal_table_id : gport_hw_resources.outlif_dbal_table_id, is_ingress,
                                 DBAL_FIELD_STAT_OBJECT_CMD, &is_stat_cmd_valid));
                /*
                 * Make sure that the result type chosen has the statistics fields inside it
                 */
                if (is_stat_cmd_valid == FALSE)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Entry type with no statistics. Make sure the statistics flag is on in the lif creation API");
                }
            }
            else
            {
                
                 /** get lifs */
                if (is_ingress)
                {
                    lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS;
                }
                else
                {
                    lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS;
                }
                SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, lif_flags, &gport_hw_resources));
                dbal_table_id =
                    (is_ingress) ? gport_hw_resources.inlif_dbal_table_id : gport_hw_resources.outlif_dbal_table_id;
                dbal_table_result_type =
                    (is_ingress) ? gport_hw_resources.
                    inlif_dbal_result_type : gport_hw_resources.outlif_dbal_result_type;
                SHR_IF_ERR_EXIT(dbal_tables_core_mode_get(unit, dbal_table_id, &core_mode));

                if ((core_mode == DBAL_CORE_MODE_DPC) && (core_id != 0) && (core_id != 1))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Core %d, must be 0\1 in DPC mode", core_id);
                }
                else if ((core_mode == DBAL_CORE_MODE_SBC) && (core_id != DBAL_CORE_ALL))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Core %d, must be set to CORE_ALL %d", core_id, DBAL_CORE_ALL);
                }

                /*
                 * Make sure that the result type chosen has the statistics fields inside it
                 */
                if ((dbal_tables_field_info_get_no_err
                     (unit, dbal_table_id, DBAL_FIELD_STAT_CMD, 0,
                      dbal_table_result_type, 0, &dbal_info) == _SHR_E_NOT_FOUND)
                    &&
                    (dbal_tables_field_info_get_no_err
                     (unit, dbal_table_id, DBAL_FIELD_STAT_OBJECT_CMD, 0,
                      dbal_table_result_type, 0, &dbal_info) == _SHR_E_NOT_FOUND))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Entry type with no statistics. Make sure the statistics flag is on in the lif creation API");
                }
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify gport stat get api
 *
 * \param [in] unit - relevant unit
 * \param [in] engine_source - counting source (IRPP, ERPP,
 *        ETPP)
 *
 * \return
 *   Negative in case of an error. See \ref shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_stat_pp_gport_get_delete_verify(
    int unit,
    bcm_stat_counter_interface_type_t engine_source)
{
    SHR_FUNC_INIT_VARS(unit);

    
    if ((engine_source != bcmStatCounterInterfaceIngressReceivePp) &&
        (engine_source != bcmStatCounterInterfaceEgressTransmitPp))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported engine source %d", engine_source);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Output dbal layer type array as a function of general header type
 *
 * \param [in] unit - relevant unit
 * \param [in] header_type - Header type to be converted
 * \param [out] dbal_layer_type_a - Array to be filled with relevant dbal layer types
 * \param [out] size - how many elements filled in the array
 *
 * \return
 *   Negative in case of an error. See \ref shr_error_e
 */
static shr_error_e
dnx_stat_pp_egress_header_types_to_dbal_convert(
    int unit,
    bcm_stat_pp_metadata_header_type_t header_type,
    dbal_enum_value_field_layer_types_e dbal_layer_type_a[STAT_PP_MAX_CONVERT_ARRAY_SIZE],
    int *size)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (header_type)
    {
        case bcmStatPpMetadataHeaderTypeEth:
            dbal_layer_type_a[0] = DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET;
            *size = 1;
            break;
        case bcmStatPpMetadataHeaderTypeIpv4:
            dbal_layer_type_a[0] = DBAL_ENUM_FVAL_LAYER_TYPES_IPV4;
            *size = 1;
            break;
        case bcmStatPpMetadataHeaderTypeIpv6:
            dbal_layer_type_a[0] = DBAL_ENUM_FVAL_LAYER_TYPES_IPV6;
            *size = 1;
            break;
        case bcmStatPpMetadataHeaderTypeArp:
            dbal_layer_type_a[0] = DBAL_ENUM_FVAL_LAYER_TYPES_ARP;
            *size = 1;
            break;
        case bcmStatPpMetadataHeaderTypeTcp:
            dbal_layer_type_a[0] = DBAL_ENUM_FVAL_LAYER_TYPES_TCP;
            *size = 1;
            break;
        case bcmStatPpMetadataHeaderTypeUdp:
            dbal_layer_type_a[0] = DBAL_ENUM_FVAL_LAYER_TYPES_UDP;
            *size = 1;
            break;
        case bcmStatPpMetadataHeaderTypeBfd:
            dbal_layer_type_a[0] = DBAL_ENUM_FVAL_LAYER_TYPES_BFD_SINGLE_HOP;
            dbal_layer_type_a[1] = DBAL_ENUM_FVAL_LAYER_TYPES_BFD_MULTI_HOP;
            *size = 2;
            break;
        case bcmStatPpMetadataHeaderTypeMpls:
            dbal_layer_type_a[0] = DBAL_ENUM_FVAL_LAYER_TYPES_MPLS;
            dbal_layer_type_a[1] = DBAL_ENUM_FVAL_LAYER_TYPES_MPLS_UNTERM;
            *size = 2;
            break;
        case bcmStatPpMetadataHeaderTypeSrv6:
            dbal_layer_type_a[0] = DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_ENDPOINT;
            dbal_layer_type_a[1] = DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_BEYOND;
            *size = 2;
            break;
        case bcmStatPpMetadataHeaderTypeFcoe:
            dbal_layer_type_a[0] = DBAL_ENUM_FVAL_LAYER_TYPES_FCOE;
            *size = 1;
            break;
        case bcmStatPpMetadataHeaderTypeIcmp:
            dbal_layer_type_a[0] = DBAL_ENUM_FVAL_LAYER_TYPES_ICMP;
            *size = 1;
            break;
        case bcmStatPpMetadataHeaderTypeOthers:
            dbal_layer_type_a[0] = DBAL_ENUM_FVAL_LAYER_TYPES_UNKNOWN;
            dbal_layer_type_a[1] = DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION;
            dbal_layer_type_a[2] = DBAL_ENUM_FVAL_LAYER_TYPES_ITMH_VAL;
            dbal_layer_type_a[3] = DBAL_ENUM_FVAL_LAYER_TYPES_ITMH_J1_VAL;
            dbal_layer_type_a[4] = DBAL_ENUM_FVAL_LAYER_TYPES_INGRESS_SCTP_EGRESS_FTMH;
            dbal_layer_type_a[5] = DBAL_ENUM_FVAL_LAYER_TYPES_GTP;
            dbal_layer_type_a[6] = DBAL_ENUM_FVAL_LAYER_TYPES_PPP;
            dbal_layer_type_a[7] = DBAL_ENUM_FVAL_LAYER_TYPES_IPVX;
            dbal_layer_type_a[8] = DBAL_ENUM_FVAL_LAYER_TYPES_RCH;
            dbal_layer_type_a[9] = DBAL_ENUM_FVAL_LAYER_TYPES_PPPOE;
            dbal_layer_type_a[10] = DBAL_ENUM_FVAL_LAYER_TYPES_IGMP;
            dbal_layer_type_a[11] = DBAL_ENUM_FVAL_LAYER_TYPES_IPT;
            dbal_layer_type_a[12] = DBAL_ENUM_FVAL_LAYER_TYPES_L2TP;
            dbal_layer_type_a[13] = DBAL_ENUM_FVAL_LAYER_TYPES_PTP;
            dbal_layer_type_a[14] = DBAL_ENUM_FVAL_LAYER_TYPES_Y_1731;
            dbal_layer_type_a[15] = DBAL_ENUM_FVAL_LAYER_TYPES_BIER_TI;
            dbal_layer_type_a[16] = DBAL_ENUM_FVAL_LAYER_TYPES_BIER_MPLS;
            *size = 17;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported header type %d", header_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Output dbal forward code array as a function of general header type
 *
 * \param [in] unit - relevant unit
 * \param [in] header_type - Header type to be converted
 * \param [in] address_type - Unicast or multicast flag
 * \param [out] dbal_egress_fwd_code_a - Array to be filled with relevant dbal forward codes
 * \param [out] size - how many elements filled in the array
 *
 * \return
 *   Negative in case of an error. See \ref shr_error_e
 */
static shr_error_e
dnx_stat_pp_egress_fwd_code_to_dbal_convert(
    int unit,
    bcm_stat_pp_metadata_header_type_t header_type,
    int address_type,
    dbal_enum_value_field_egress_fwd_code_e dbal_egress_fwd_code_a[STAT_PP_MAX_CONVERT_ARRAY_SIZE],
    int *size)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (header_type)
    {
        case bcmStatPpMetadataHeaderTypeEth:
            dbal_egress_fwd_code_a[0] = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_ETHERNET;
            *size = 1;
            break;
        case bcmStatPpMetadataHeaderTypeIpv4:
            /*
             * Note: address type cannot be set to ucast | mcast, it is necessarily one of them
             */
            if (address_type == BCM_STAT_PP_METADATA_ADDRESS_TYPE_UCAST)
            {
                dbal_egress_fwd_code_a[0] = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV4_UC_R0;
                dbal_egress_fwd_code_a[1] = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV4_UC_R1;
                *size = 2;
            }
            else if (address_type == BCM_STAT_PP_METADATA_ADDRESS_TYPE_MCAST)
            {
                dbal_egress_fwd_code_a[0] = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV4_MC_R0;
                dbal_egress_fwd_code_a[1] = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV4_MC_R1;
                *size = 2;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported address type");
            }
            break;
        case bcmStatPpMetadataHeaderTypeIpv6:
            /*
             * Note: address type cannot be set to ucast | mcast, it is necessarily one of them
             */
            if (address_type == BCM_STAT_PP_METADATA_ADDRESS_TYPE_UCAST)
            {
                dbal_egress_fwd_code_a[0] = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6_UC_R0;
                dbal_egress_fwd_code_a[1] = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6_UC_R1;
                *size = 2;
            }
            else if (address_type == BCM_STAT_PP_METADATA_ADDRESS_TYPE_MCAST)
            {
                dbal_egress_fwd_code_a[0] = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6_MC_R0;
                dbal_egress_fwd_code_a[1] = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6_MC_R1;
                *size = 2;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported address type");
            }
            break;
        case bcmStatPpMetadataHeaderTypeMpls:
            dbal_egress_fwd_code_a[0] = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_MPLS;
            dbal_egress_fwd_code_a[1] = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_MPLS_INJECTED_FROM_OAMP;
            *size = 2;
            break;
        case bcmStatPpMetadataHeaderTypeSrv6:
            dbal_egress_fwd_code_a[0] = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_SRV6_ENDPOINT;
            dbal_egress_fwd_code_a[1] = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_SRV6_ENDPOINT_PSP;
            dbal_egress_fwd_code_a[2] = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_SRV6_UNIFIED_ENDPOINT;
            dbal_egress_fwd_code_a[3] = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_SRV6_USID_ENDPOINT;
            *size = 4;
            break;
        case bcmStatPpMetadataHeaderTypeFcoe:
            *size = 0;
            break;
        case bcmStatPpMetadataHeaderTypeOthers:
            dbal_egress_fwd_code_a[0] = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_RCH_DROP_AND_CONT;
            dbal_egress_fwd_code_a[1] = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_RCH_PTCH_ENC;
            dbal_egress_fwd_code_a[2] = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_RCH_EXT_ENCAP;
            dbal_egress_fwd_code_a[3] = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_RCH_XTERMINATION;
            dbal_egress_fwd_code_a[4] = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_FWD_CODE_BUG_CODE;
            dbal_egress_fwd_code_a[5] = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_PPP;
            dbal_egress_fwd_code_a[6] = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_DO_NOT_EDIT;
            dbal_egress_fwd_code_a[7] = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_TM;
            dbal_egress_fwd_code_a[8] = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_TDM;
            dbal_egress_fwd_code_a[9] = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_ERPP_TRAPPED;
            dbal_egress_fwd_code_a[10] = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_STACKING_PORT;
            dbal_egress_fwd_code_a[11] = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_CPU_PORT;
            dbal_egress_fwd_code_a[12] = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_RAW_PROCESSING;
            dbal_egress_fwd_code_a[13] = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_INGRESS_TRAPPED_JR1_LEGACY;
            dbal_egress_fwd_code_a[14] = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_BIER_MPLS;
            dbal_egress_fwd_code_a[15] = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_BIER_TI;
            dbal_egress_fwd_code_a[16] = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_MIRROR_OR_SS;
            *size = 17;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported header type %d", header_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Set metadata value for header type specified in metadata_info - for forward step
 */
static shr_error_e
dnx_stat_pp_fwd_metadata_set(
    int unit,
    bcm_stat_pp_metadata_info_t * metadata_info)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_egress_fwd_code_e dbal_fwd_codes_a[STAT_PP_MAX_CONVERT_ARRAY_SIZE];
    int size = STAT_PP_MAX_CONVERT_ARRAY_SIZE;
    int ii;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STAT_PP_ETPP_FWD_STATISTICS_METADATA_VALUE_MAP,
                                      &entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_stat_pp_egress_fwd_code_to_dbal_convert(unit,
                                                                metadata_info->header_type,
                                                                metadata_info->address_type_flags, dbal_fwd_codes_a,
                                                                &size));

    for (ii = 0; ii < size; ++ii)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_CODE, dbal_fwd_codes_a[ii]);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_METADATA_VAL,
                                     INST_SINGLE, metadata_info->metadata);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * Get metadata value for header type specified in metadata_info - for forward step
 */
static shr_error_e
dnx_stat_pp_fwd_metadata_get(
    int unit,
    bcm_stat_pp_metadata_info_t * metadata_info)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_egress_fwd_code_e dbal_fwd_codes_a[STAT_PP_MAX_CONVERT_ARRAY_SIZE];
    int size = STAT_PP_MAX_CONVERT_ARRAY_SIZE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STAT_PP_ETPP_FWD_STATISTICS_METADATA_VALUE_MAP,
                                      &entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_stat_pp_egress_fwd_code_to_dbal_convert(unit,
                                                                metadata_info->header_type,
                                                                metadata_info->address_type_flags, dbal_fwd_codes_a,
                                                                &size));

    /*
     * For get operation, any value of the output array is ok, they are all the same (at the condition set was done
     * before)
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_CODE, dbal_fwd_codes_a[0]);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_STAT_METADATA_VAL, INST_SINGLE, &metadata_info->metadata));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * Set metadata value for header type specified in metadata_info - for encap step
 */
static shr_error_e
dnx_stat_pp_tunnel_metadata_set(
    int unit,
    bcm_stat_pp_metadata_info_t * metadata_info)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_layer_types_e dbal_layer_types_a[STAT_PP_MAX_CONVERT_ARRAY_SIZE];
    int size = STAT_PP_MAX_CONVERT_ARRAY_SIZE;
    int ii;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STAT_PP_ETPP_ENC_STATISTICS_METADATA_VALUE_MAP,
                                      &entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_stat_pp_egress_header_types_to_dbal_convert
                    (unit, metadata_info->header_type, dbal_layer_types_a, &size));

    for (ii = 0; ii < size; ++ii)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE, dbal_layer_types_a[ii]);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_METADATA_VAL,
                                     INST_SINGLE, metadata_info->metadata);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * Get metadata value for header type specified in metadata_info - for encap step
 */
static shr_error_e
dnx_stat_pp_tunnel_metadata_get(
    int unit,
    bcm_stat_pp_metadata_info_t * metadata_info)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_layer_types_e dbal_layer_types_a[STAT_PP_MAX_CONVERT_ARRAY_SIZE];
    int size = STAT_PP_MAX_CONVERT_ARRAY_SIZE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STAT_PP_ETPP_ENC_STATISTICS_METADATA_VALUE_MAP,
                                      &entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_stat_pp_egress_header_types_to_dbal_convert
                    (unit, metadata_info->header_type, dbal_layer_types_a, &size));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE, dbal_layer_types_a[0]);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_STAT_METADATA_VAL, INST_SINGLE, &metadata_info->metadata));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_stat_pp_egress_metadata_set(
    int unit,
    bcm_stat_pp_metadata_info_t * metadata_info)
{
    uint32 flow_step = metadata_info->flags;
    SHR_FUNC_INIT_VARS(unit);

    if ((_SHR_IS_FLAG_SET(flow_step, BCM_STAT_PP_METADATA_TUNNEL)))
    {
        SHR_IF_ERR_EXIT(dnx_stat_pp_tunnel_metadata_set(unit, metadata_info));
    }
    else if ((_SHR_IS_FLAG_SET(flow_step, BCM_STAT_PP_METADATA_FORWARD)))
    {
        SHR_IF_ERR_EXIT(dnx_stat_pp_fwd_metadata_set(unit, metadata_info));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_stat_pp_egress_metadata_get(
    int unit,
    bcm_stat_pp_metadata_info_t * metadata_info)
{
    uint32 flow_step = metadata_info->flags;
    SHR_FUNC_INIT_VARS(unit);

    if ((_SHR_IS_FLAG_SET(flow_step, BCM_STAT_PP_METADATA_TUNNEL)))
    {
        SHR_IF_ERR_EXIT(dnx_stat_pp_tunnel_metadata_get(unit, metadata_info));
    }
    else if ((_SHR_IS_FLAG_SET(flow_step, BCM_STAT_PP_METADATA_FORWARD)))
    {
        SHR_IF_ERR_EXIT(dnx_stat_pp_fwd_metadata_get(unit, metadata_info));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Set metadata value for header type and IS-MC specified in metadata_info - for next layer after tunnel termination,
 * or for next layer after forwarding.
 */
static shr_error_e
dnx_stat_pp_ingress_header_metadata_set(
    int unit,
    bcm_stat_pp_metadata_info_t * metadata_info)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_layer_types_e dbal_layer_types_a[STAT_PP_MAX_CONVERT_ARRAY_SIZE];
    int size = STAT_PP_MAX_CONVERT_ARRAY_SIZE;
    uint32 address_type_bitmap = 0;
    int ii, is_mc;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(metadata_info->flags, BCM_STAT_PP_METADATA_TUNNEL))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STAT_PP_IRPP_VTT_STATISTICS_METADATA_VALUE_MAP,
                                          &entry_handle_id));
    }
    else if (_SHR_IS_FLAG_SET(metadata_info->flags, BCM_STAT_PP_METADATA_FORWARD_PLUS_ONE))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STAT_PP_IRPP_FWD_PLUS_ONE_STATISTICS_METADATA_VALUE_MAP,
                                          &entry_handle_id));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flags %d", metadata_info->flags);
    }

    SHR_IF_ERR_EXIT(dnx_stat_pp_egress_header_types_to_dbal_convert
                    (unit, metadata_info->header_type, dbal_layer_types_a, &size));

    if ((metadata_info->header_type == bcmStatPpMetadataHeaderTypeEth) ||
        (metadata_info->header_type == bcmStatPpMetadataHeaderTypeIpv4) ||
        (metadata_info->header_type == bcmStatPpMetadataHeaderTypeIpv6))
    {
        SHR_BITSET(&address_type_bitmap,
                   _SHR_IS_FLAG_SET(metadata_info->address_type_flags, BCM_STAT_PP_METADATA_ADDRESS_TYPE_MCAST));
    }
    else
    {
        /*
         * If header type does not support IS_MC bit, set both IS_MC = 0/1
         */
        address_type_bitmap = 0x3;
    }

    for (is_mc = 0; is_mc < 2; ++is_mc)
    {
         /* coverity[ptr_arith:FALSE]  */
        if (SHR_BITGET(&address_type_bitmap, is_mc))
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MC, is_mc);

            for (ii = 0; ii < size; ++ii)
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE, dbal_layer_types_a[ii]);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_METADATA_VAL,
                                             INST_SINGLE, metadata_info->metadata);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * Get metadata value for header type and IS-MC specified in metadata_info - for next layer after tunnel termination,
 * or for next layer after forwarding.
 */
static shr_error_e
dnx_stat_pp_ingress_header_metadata_get(
    int unit,
    bcm_stat_pp_metadata_info_t * metadata_info)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_layer_types_e dbal_layer_types_a[STAT_PP_MAX_CONVERT_ARRAY_SIZE];
    int size = STAT_PP_MAX_CONVERT_ARRAY_SIZE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(metadata_info->flags, BCM_STAT_PP_METADATA_TUNNEL))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STAT_PP_IRPP_VTT_STATISTICS_METADATA_VALUE_MAP,
                                          &entry_handle_id));
    }
    else if (_SHR_IS_FLAG_SET(metadata_info->flags, BCM_STAT_PP_METADATA_FORWARD_PLUS_ONE))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STAT_PP_IRPP_FWD_PLUS_ONE_STATISTICS_METADATA_VALUE_MAP,
                                          &entry_handle_id));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flags %d", metadata_info->flags);
    }

    SHR_IF_ERR_EXIT(dnx_stat_pp_egress_header_types_to_dbal_convert
                    (unit, metadata_info->header_type, dbal_layer_types_a, &size));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MC,
                               _SHR_IS_FLAG_SET(metadata_info->address_type_flags,
                                                BCM_STAT_PP_METADATA_ADDRESS_TYPE_MCAST));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE, dbal_layer_types_a[0]);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_STAT_METADATA_VAL, INST_SINGLE, &metadata_info->metadata));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * Set metadata value for {Is-MC, Is-BC and Is-Unknown} specified in metadata_info - for ingress forwarding step
 */
static shr_error_e
dnx_stat_pp_ingress_fwd_metadata_set(
    int unit,
    bcm_stat_pp_metadata_info_t * metadata_info)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STAT_PP_IRPP_FWD_STATISTICS_METADATA_VALUE_MAP,
                                      &entry_handle_id));

    /**
     * Set key fields {Is-MC, Is-BC and Is-Unknown}
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_UNKNOWN,
                               _SHR_IS_FLAG_SET(metadata_info->address_type_flags,
                                                BCM_STAT_PP_METADATA_ADDRESS_TYPE_UNKNOWN));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_BC,
                               _SHR_IS_FLAG_SET(metadata_info->address_type_flags,
                                                BCM_STAT_PP_METADATA_ADDRESS_TYPE_BCAST));

    if (dnx_data_l2.feature.feature_get(unit, dnx_data_l2_feature_eth_qual_is_mc))
    {
        /**
         * Is-MC needs to be set for broadcast as well.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MC,
                                   _SHR_IS_FLAG_SET(metadata_info->address_type_flags,
                                                    BCM_STAT_PP_METADATA_ADDRESS_TYPE_MCAST)
                                   || _SHR_IS_FLAG_SET(metadata_info->address_type_flags,
                                                       BCM_STAT_PP_METADATA_ADDRESS_TYPE_BCAST));
    }
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MC,
                                   _SHR_IS_FLAG_SET(metadata_info->address_type_flags,
                                                    BCM_STAT_PP_METADATA_ADDRESS_TYPE_MCAST));
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_METADATA_VAL,
                                 INST_SINGLE, metadata_info->metadata);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * Get metadata value for {Is-MC, Is-BC and Is-Unknown} specified in metadata_info - for ingress forwarding step
 */
static shr_error_e
dnx_stat_pp_ingress_fwd_metadata_get(
    int unit,
    bcm_stat_pp_metadata_info_t * metadata_info)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STAT_PP_IRPP_FWD_STATISTICS_METADATA_VALUE_MAP,
                                      &entry_handle_id));

    /**
     * Set key fields {Is-MC, Is-BC and Is-Unknown}
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_UNKNOWN,
                               _SHR_IS_FLAG_SET(metadata_info->address_type_flags,
                                                BCM_STAT_PP_METADATA_ADDRESS_TYPE_UNKNOWN));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_BC,
                               _SHR_IS_FLAG_SET(metadata_info->address_type_flags,
                                                BCM_STAT_PP_METADATA_ADDRESS_TYPE_BCAST));

    if (dnx_data_l2.feature.feature_get(unit, dnx_data_l2_feature_eth_qual_is_mc))
    {
        /**
         * Is-MC needs to be set for broadcast as well.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MC,
                                   _SHR_IS_FLAG_SET(metadata_info->address_type_flags,
                                                    BCM_STAT_PP_METADATA_ADDRESS_TYPE_MCAST)
                                   || _SHR_IS_FLAG_SET(metadata_info->address_type_flags,
                                                       BCM_STAT_PP_METADATA_ADDRESS_TYPE_BCAST));
    }
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MC,
                                   _SHR_IS_FLAG_SET(metadata_info->address_type_flags,
                                                    BCM_STAT_PP_METADATA_ADDRESS_TYPE_MCAST));
    }

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_STAT_METADATA_VAL, INST_SINGLE, &metadata_info->metadata));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_stat_pp_ingress_metadata_set(
    int unit,
    bcm_stat_pp_metadata_info_t * metadata_info)
{
    uint32 flow_step = metadata_info->flags;
    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(flow_step, BCM_STAT_PP_METADATA_TUNNEL) ||
        _SHR_IS_FLAG_SET(flow_step, BCM_STAT_PP_METADATA_FORWARD_PLUS_ONE))
    {
        SHR_IF_ERR_EXIT(dnx_stat_pp_ingress_header_metadata_set(unit, metadata_info));
    }
    else if ((_SHR_IS_FLAG_SET(flow_step, BCM_STAT_PP_METADATA_FORWARD)))
    {
        SHR_IF_ERR_EXIT(dnx_stat_pp_ingress_fwd_metadata_set(unit, metadata_info));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_stat_pp_ingress_metadata_get(
    int unit,
    bcm_stat_pp_metadata_info_t * metadata_info)
{
    uint32 flow_step = metadata_info->flags;
    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(flow_step, BCM_STAT_PP_METADATA_TUNNEL) ||
        _SHR_IS_FLAG_SET(flow_step, BCM_STAT_PP_METADATA_FORWARD_PLUS_ONE))
    {
        SHR_IF_ERR_EXIT(dnx_stat_pp_ingress_header_metadata_get(unit, metadata_info));
    }
    else if ((_SHR_IS_FLAG_SET(flow_step, BCM_STAT_PP_METADATA_FORWARD)))
    {
        SHR_IF_ERR_EXIT(dnx_stat_pp_ingress_fwd_metadata_get(unit, metadata_info));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_stat_pp_metadata_ingress_check_args(
    int unit,
    bcm_stat_pp_metadata_info_t * metadata_info)
{
    uint32 supported_flags;

    SHR_FUNC_INIT_VARS(unit);

    supported_flags = BCM_STAT_PP_METADATA_INGRESS | BCM_STAT_PP_METADATA_TUNNEL |
        BCM_STAT_PP_METADATA_FORWARD | BCM_STAT_PP_METADATA_FORWARD_PLUS_ONE;

    SHR_MASK_VERIFY(metadata_info->flags, supported_flags, _SHR_E_PARAM, "Unsupported flags");

    if ((_SHR_IS_FLAG_SET(metadata_info->flags, BCM_STAT_PP_METADATA_TUNNEL) +
         _SHR_IS_FLAG_SET(metadata_info->flags, BCM_STAT_PP_METADATA_FORWARD) +
         _SHR_IS_FLAG_SET(metadata_info->flags, BCM_STAT_PP_METADATA_FORWARD_PLUS_ONE)) != 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "More than one step set in flag 0x%X", metadata_info->flags);
    }

    /*
     * tunnel/forward_plus_one step support IS_MC
     */
    if (_SHR_IS_FLAG_SET(metadata_info->flags, BCM_STAT_PP_METADATA_TUNNEL | BCM_STAT_PP_METADATA_FORWARD_PLUS_ONE))
    {
        SHR_MASK_VERIFY(metadata_info->address_type_flags, BCM_STAT_PP_METADATA_ADDRESS_TYPE_MCAST,
                        _SHR_E_PARAM, "Unsupported address_type_flags");

        if (_SHR_IS_FLAG_SET(metadata_info->address_type_flags, BCM_STAT_PP_METADATA_ADDRESS_TYPE_MCAST) &&
            (metadata_info->header_type != bcmStatPpMetadataHeaderTypeEth) &&
            (metadata_info->header_type != bcmStatPpMetadataHeaderTypeIpv4) &&
            (metadata_info->header_type != bcmStatPpMetadataHeaderTypeIpv6))
        {
            /*
             * If multicast flag is set, then header type must be one of ETH/IPv4/IPv6.
             * For all other header types, address type will be ignored, both IS_MC=0/1 will be set.
             */
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported address_type_flags 0x%X", metadata_info->address_type_flags);
        }
    }

    supported_flags = BCM_STAT_PP_METADATA_ADDRESS_TYPE_UCAST | BCM_STAT_PP_METADATA_ADDRESS_TYPE_MCAST |
        BCM_STAT_PP_METADATA_ADDRESS_TYPE_BCAST | BCM_STAT_PP_METADATA_ADDRESS_TYPE_UNKNOWN;

    /*
     * forward step supports IS_MC/IS_BC/UNKNOWN
     */
    if (_SHR_IS_FLAG_SET(metadata_info->flags, BCM_STAT_PP_METADATA_FORWARD))
    {
        SHR_MASK_VERIFY(metadata_info->address_type_flags, supported_flags,
                        _SHR_E_PARAM, "Unsupported address_type_flags");

        /*
         * Header type is not supported for forwarding header metadata
         */
        if (metadata_info->header_type != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported header type %d", metadata_info->header_type);
        }
    }

    if (metadata_info->metadata > STAT_PP_MAX_METADATA_VALUE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Metadata value is too big %d, max allowed %d",
                     metadata_info->metadata, STAT_PP_MAX_METADATA_VALUE);
    }

    if (metadata_info->header_type >= bcmStatPpMetadataHeaderTypeCount)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported header type %d", metadata_info->header_type);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_stat_pp_metadata_check_args(
    int unit,
    bcm_stat_pp_metadata_info_t * metadata_info)
{
    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(metadata_info->flags, BCM_STAT_PP_METADATA_INGRESS))
    {
        if (_SHR_IS_FLAG_SET(metadata_info->flags, BCM_STAT_PP_METADATA_EGRESS))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Ingress and egress metadata need to be set separately");
        }

        return dnx_stat_pp_metadata_ingress_check_args(unit, metadata_info);
    }

    if (metadata_info->flags != (BCM_STAT_PP_METADATA_EGRESS | BCM_STAT_PP_METADATA_TUNNEL) &&
        metadata_info->flags != (BCM_STAT_PP_METADATA_EGRESS | BCM_STAT_PP_METADATA_FORWARD))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flags %d", metadata_info->flags);
    }

    /*
     * In forward step, address type should be properly defined
     */
    if (_SHR_IS_FLAG_SET(metadata_info->flags, BCM_STAT_PP_METADATA_FORWARD) &&
        ((metadata_info->header_type == bcmStatPpMetadataHeaderTypeIpv4)
         || (metadata_info->header_type == bcmStatPpMetadataHeaderTypeIpv6))
        && (metadata_info->address_type_flags != BCM_STAT_PP_METADATA_ADDRESS_TYPE_UCAST)
        && (metadata_info->address_type_flags != BCM_STAT_PP_METADATA_ADDRESS_TYPE_MCAST)
        && (metadata_info->address_type_flags != BCM_STAT_PP_METADATA_ADDRESS_TYPE_BCAST))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported type of address %d", metadata_info->address_type_flags);
    }

    if (metadata_info->metadata > STAT_PP_MAX_METADATA_VALUE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Metadata value is too big %d, max allowed %d",
                     metadata_info->metadata, STAT_PP_MAX_METADATA_VALUE);
    }

    if (metadata_info->header_type >= bcmStatPpMetadataHeaderTypeCount)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported header type %d", metadata_info->header_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Set metadata value for the defined header type \n
 *
 * \param [in] unit -
 *   Relevant unit.
 * \param [in] metadata_info \n
 *      metadata_info.flags - BCM_STAT_PP_METADATA_XXX \n
 *          set flow side: \n
 *              BCM_STAT_PP_METADATA_INGRESS \n
 *              BCM_STAT_PP_METADATA_EGRESS \n
 *          set step:\n
 *              BCM_STAT_PP_METADATA_TUNNEL\n
 *              BCM_STAT_PP_METADATA_FORWARD\n
 *              BCM_STAT_PP_METADATA_FORWARD_PLUS_ONE\n
 *      metadata_info.address_type_flags - BCM_STAT_PP_METADATA_ADDRESS_TYPE_XXX\n
 *          BCM_STAT_PP_METADATA_ADDRESS_TYPE_UCAST - unicast\n
 *          BCM_STAT_PP_METADATA_ADDRESS_TYPE_MCAST - multicast\n
 *          BCM_STAT_PP_METADATA_ADDRESS_TYPE_BCAST - broadcast\n
 *      metadata_info.header_type\n - Provide the headr type for which metadata value should be set
 *          see options in bcm_stat_pp_metadata_header_type_t
 *      [in]metadata_info.metadata - value to be set
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 */
int
bcm_dnx_stat_pp_metadata_set(
    int unit,
    bcm_stat_pp_metadata_info_t * metadata_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_stat_pp_metadata_check_args(unit, metadata_info));

    if (_SHR_IS_FLAG_SET(metadata_info->flags, BCM_STAT_PP_METADATA_EGRESS))
    {
        SHR_IF_ERR_EXIT(dnx_stat_pp_egress_metadata_set(unit, metadata_info));
    }

    if (_SHR_IS_FLAG_SET(metadata_info->flags, BCM_STAT_PP_METADATA_INGRESS))
    {
        SHR_IF_ERR_EXIT(dnx_stat_pp_ingress_metadata_set(unit, metadata_info));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Set metadata value for the defined header type \n
 *
 * \param [in] unit -
 *   Relevant unit.
 * \param [in,out] metadata_info \n
 *      [in]metadata_info.flags - BCM_STAT_PP_METADATA_XXX \n
 *          set flow side: \n
 *              BCM_STAT_PP_METADATA_INGRESS \n
 *              BCM_STAT_PP_METADATA_EGRESS \n
 *          set step:\n
 *              BCM_STAT_PP_METADATA_TUNNEL\n
 *              BCM_STAT_PP_METADATA_FORWARD\n
 *              BCM_STAT_PP_METADATA_FORWARD_PLUS_ONE\n
 *      [in]metadata_info.address_type_flags - BCM_STAT_PP_METADATA_ADDRESS_TYPE_XXX\n
 *          BCM_STAT_PP_METADATA_ADDRESS_TYPE_UCAST - unicast\n
 *          BCM_STAT_PP_METADATA_ADDRESS_TYPE_MCAST - multicast\n
 *          BCM_STAT_PP_METADATA_ADDRESS_TYPE_BCAST - broadcast\n
 *      [in]metadata_info.header_type\n - Provide the headr type for which metadata value should be set
 *          see options in bcm_stat_pp_metadata_header_type_t
 *      [out] metadata_info.metadata - returned metadata value
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 */
int
bcm_dnx_stat_pp_metadata_get(
    int unit,
    bcm_stat_pp_metadata_info_t * metadata_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR_PTR(unit, sizeof(bcm_stat_pp_metadata_info_t), metadata_info);

    SHR_INVOKE_VERIFY_DNX(dnx_stat_pp_metadata_check_args(unit, metadata_info));

    if (_SHR_IS_FLAG_SET(metadata_info->flags, BCM_STAT_PP_METADATA_EGRESS))
    {
        SHR_IF_ERR_EXIT(dnx_stat_pp_egress_metadata_get(unit, metadata_info));
    }

    if (_SHR_IS_FLAG_SET(metadata_info->flags, BCM_STAT_PP_METADATA_INGRESS))
    {
        SHR_IF_ERR_EXIT(dnx_stat_pp_ingress_metadata_get(unit, metadata_info));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - called by stat gport set api in case of a physical
 *        port. Write the statistic data to the ingress port or
 *        egress port dbal table according to the engine_source
 *
 * \param [in] unit - relevant unit
 * \param [in] gport - physical or logical port
 * \param [in] core_id - core id
 * \param [in] engine_source - counting source (IRPP, ERPP,
 *        ETPP)
 * \param [in] stat_info - statistic information per object
 *
 * \return
 *   Negative in case of an error. See \ref shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_stat_pp_physical_gport_set(
    int unit,
    bcm_gport_t gport,
    bcm_core_t core_id,
    bcm_stat_counter_interface_type_t engine_source,
    bcm_stat_pp_info_t stat_info)
{
    uint32 entry_handle_id;
    int port_iter;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get pp port */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    switch (engine_source)
    {
        case bcmStatCounterInterfaceIngressReceivePp:
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
            break;
        case bcmStatCounterInterfaceEgressTransmitPp:
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));
            break;
        case bcmStatCounterInterfaceEgressReceivePp:
            
            SHR_ERR_EXIT(_SHR_E_PARAM, "ERPP counting engine is not supported");
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported engine_source");
            break;
    }

    for (port_iter = 0; port_iter < gport_info.internal_port_pp_info.nof_pp_ports; port_iter++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[port_iter]);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[port_iter]);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE, stat_info.stat_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE,
                                     STAT_PP_PROFILE_ID_GET(stat_info.stat_pp_profile));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - called by stat gport get api in case of a physical
 *        port. Read the statistic data from the relevant pp
 *        port dbal table (ingress or egress)
 *
 * \param [in] unit - relevant unit
 * \param [in] gport - physical or logical port
 * \param [in] core_id - core id
 * \param [in] engine_source - counting source (IRPP, ERPP,
 *        ETPP)
 * \param [in] stat_info - statistic information per object
 *
 * \return
 *   Negative in case of an error. See \ref shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_stat_pp_physical_gport_get(
    int unit,
    bcm_gport_t gport,
    bcm_core_t core_id,
    bcm_stat_counter_interface_type_t engine_source,
    bcm_stat_pp_info_t * stat_info)
{
    uint32 entry_handle_id;
    uint32 stat_pp_profile;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get PP Port
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    switch (engine_source)
    {
        case bcmStatCounterInterfaceIngressReceivePp:
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
            break;
        case bcmStatCounterInterfaceEgressTransmitPp:
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));
            break;
        case bcmStatCounterInterfaceEgressReceivePp:
            
            SHR_ERR_EXIT(_SHR_E_PARAM, "ERPP counting engine is not supported");
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported engine_source");
            break;
    }

    /**All relevant gport pp ports are configured with the same stats attributes -> take only first entry data*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, gport_info.internal_port_pp_info.core_id[0]);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, gport_info.internal_port_pp_info.pp_port[0]);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                        entry_handle_id,
                                                        DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE, &stat_info->stat_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                        entry_handle_id,
                                                        DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE, &stat_pp_profile));
    STAT_PP_ENGINE_PROFILE_SET(stat_info->stat_pp_profile, stat_pp_profile, engine_source);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set egress virtual lif dbal table id key
 *
 * \param [in] unit - relevant unit
 * \param [in] gport - virtual gport
 * \param [in] local_out_lif - egress local out lif
 * \param [in] outlif_entry_handle_id - dbal table handle id, for setting key, result type (if needed)
 *
 * \return
 *   Negative in case of an error. See \ref shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_stat_pp_set_virtual_lif_table_id_and_key(
    int unit,
    bcm_gport_t gport,
    int local_out_lif,
    uint32 *outlif_entry_handle_id)
{
    uint32 lif_table_result_type;
    uint32 get_entry_handle_id;
    uint32 lif_table_key;
    dbal_fields_e lif_table_key_field;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    lif_table_key = gport;
    lif_table_key_field = DBAL_FIELD_GPORT;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, &get_entry_handle_id));
    dbal_entry_key_field32_set(unit, get_entry_handle_id, lif_table_key_field, lif_table_key);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, get_entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, get_entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &lif_table_result_type));

    dbal_entry_key_field32_set(unit, *outlif_entry_handle_id, lif_table_key_field, lif_table_key);
    dbal_entry_value_field32_set(unit, *outlif_entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 lif_table_result_type);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - called by stat gport set api in case of a logical
 *        port. Write the statistic data to the relevant lif
 *        dbal table
 *
 * \param [in] unit - relevant unit
 * \param [in] gport - physical or logical port
 * \param [in] engine_source - counting source (IRPP, ERPP,
 *        ETPP)
 * \param [in] stat_info - statistic information per object
 *
 * \return
 *   Negative in case of an error. See \ref shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_stat_pp_logical_gport_set(
    int unit,
    bcm_gport_t gport,
    bcm_stat_counter_interface_type_t engine_source,
    bcm_stat_pp_info_t stat_info)
{
    uint32 stat_id = stat_info.stat_id;
    int stat_pp_profile = stat_info.stat_pp_profile;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint32 lif_flags;
    uint32 entry_handle_id, outlif_entry_handle_id = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get the HW resources
     */
    switch (engine_source)
    {
        case bcmStatCounterInterfaceIngressReceivePp:
            lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS;
            break;
        case bcmStatCounterInterfaceEgressTransmitPp:
            /*
             * Use for both virtual and non-virtual
             */
            lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_EGRESS_LOCAL_LIF_OR_VIRTUAL;
            break;
        case bcmStatCounterInterfaceEgressReceivePp:
            
            SHR_ERR_EXIT(_SHR_E_PARAM, "ERPP counting engine is not supported");
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported engine_source");
            break;
    }
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, lif_flags, &gport_hw_resources));

    if (engine_source == bcmStatCounterInterfaceIngressReceivePp)
    {
        /*
         * Set table key
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     gport_hw_resources.inlif_dbal_result_type);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
    }
    else
    {
        /*
         * engine_source == bcmStatCounterInterfaceEgressTransmitPp
         * Set table key: relevant for virtual and non-virtual
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.outlif_dbal_table_id, &entry_handle_id));
        SHR_IF_ERR_EXIT(algo_gpm_gport_egress_hw_key_set(unit, entry_handle_id, gport, &gport_hw_resources));
        if ((dnx_data_stat.stat_pp.etpp_reversed_stat_cmd_get(unit) == 1)
            && (STAT_PP_PROFILE_ID_GET(stat_pp_profile) <= STAT_PP_ETPP_SWITCH_PROFILE_MAX_PROFILE)
                                                                                                 /**Counting profile*/
            && ((gport_hw_resources.outlif_dbal_table_id != DBAL_TABLE_EEDB_MPLS_TUNNEL)
                || (gport_hw_resources.outlif_dbal_result_type !=
                    DBAL_RESULT_TYPE_EEDB_MPLS_TUNNEL_ETPS_MPLS_PHP_STAT)))
        {
            int field_nof_bits;

            SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                            (unit, gport_hw_resources.outlif_dbal_table_id, DBAL_FIELD_STAT_OBJECT_ID, FALSE,
                             gport_hw_resources.outlif_dbal_result_type, INST_SINGLE, &field_nof_bits));
            if (field_nof_bits < 16)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "stat_id #nof bits in table %s, result type %s is too low per this device, unsupported",
                             dbal_logical_table_to_string(unit, gport_hw_resources.outlif_dbal_table_id),
                             dbal_result_type_to_string(unit, gport_hw_resources.outlif_dbal_table_id,
                                                        gport_hw_resources.outlif_dbal_result_type));
            }
            STAT_PP_ETPP_SWITCH_CMD_SET(stat_id, stat_pp_profile);
            if ((stat_id >> field_nof_bits) > 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "stat profile %d is too big for table %s, result type %s stat_id field. Max possible value: %d",
                             STAT_PP_PROFILE_ID_GET(stat_info.stat_pp_profile), dbal_logical_table_to_string(unit,
                                                                                                             gport_hw_resources.outlif_dbal_table_id),
                             dbal_result_type_to_string(unit, gport_hw_resources.outlif_dbal_table_id,
                                                        gport_hw_resources.outlif_dbal_result_type),
                             (field_nof_bits == 16) ? 1 : 3);
            }
            /**Indicate counting profile, in get() API - entry should also be reversed*/
            if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_MATCH(gport)
                || BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(gport))
            {
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                                (unit, DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, &outlif_entry_handle_id));
                SHR_IF_ERR_EXIT(dnx_stat_pp_set_virtual_lif_table_id_and_key
                                (unit, gport, gport_hw_resources.local_out_lif, &outlif_entry_handle_id));
                dbal_entry_value_field32_set(unit, outlif_entry_handle_id, DBAL_FIELD_STAT_PP_ETPP_IS_COUNTING_PROFILE,
                                             INST_SINGLE, TRUE);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, outlif_entry_handle_id, DBAL_COMMIT_UPDATE));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_is_count_profile_set
                                (unit, gport_hw_resources.local_out_lif));
            }
        }
    }

    /*
     * Set values
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE, stat_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE,
                                 STAT_PP_PROFILE_ID_GET(stat_pp_profile));

    if (BCM_GPORT_IS_VLAN_PORT(gport) && BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_MATCH(gport))
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - called by stat gport get api in case of a logical
 *        port. Read the statistic data from the relevant lif
 *        dbal table
 *
 * \param [in] unit - relevant unit
 * \param [in] gport - physical or logical port
 * \param [in] engine_source - counting source (IRPP, ERPP,
 *        ETPP)
 * \param [in] stat_info - statistic information per object
 *
 * \return
 *   Negative in case of an error. See \ref shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_stat_pp_logical_gport_get(
    int unit,
    bcm_gport_t gport,
    bcm_stat_counter_interface_type_t engine_source,
    bcm_stat_pp_info_t * stat_info)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint32 lif_flags;
    uint32 stat_pp_profile;
    uint32 entry_handle_id, outlif_entry_handle_id = 0;
    uint32 is_counting_profile;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    switch (engine_source)
    {
        case bcmStatCounterInterfaceIngressReceivePp:
            lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS;
            break;
        case bcmStatCounterInterfaceEgressTransmitPp:
            lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_EGRESS_LOCAL_LIF_OR_VIRTUAL;
            break;
        case bcmStatCounterInterfaceEgressReceivePp:
                
            SHR_ERR_EXIT(_SHR_E_PARAM, "ERPP counting engine is not supported");
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported engine_source");
            break;
    }

    /** get local and global out lifs */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, lif_flags, &gport_hw_resources));

    switch (engine_source)
    {
        case bcmStatCounterInterfaceIngressReceivePp:
            /*
             * Set keys for ingress
             */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         gport_hw_resources.inlif_dbal_result_type);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
            break;
        case bcmStatCounterInterfaceEgressTransmitPp:
            /*
             * Set keys for egress (including virtual)
             */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.outlif_dbal_table_id, &entry_handle_id));
            SHR_IF_ERR_EXIT(algo_gpm_gport_egress_hw_key_set(unit, entry_handle_id, gport, &gport_hw_resources));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Shouldn't get here!");
            break;
    }

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                        entry_handle_id,
                                                        DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE, &stat_info->stat_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE, &stat_pp_profile));

    if ((engine_source == bcmStatCounterInterfaceEgressTransmitPp)
        && (dnx_data_stat.stat_pp.etpp_reversed_stat_cmd_get(unit) == 1) &&
        ((gport_hw_resources.outlif_dbal_table_id != DBAL_TABLE_EEDB_MPLS_TUNNEL) ||
         (gport_hw_resources.outlif_dbal_result_type != DBAL_RESULT_TYPE_EEDB_MPLS_TUNNEL_ETPS_MPLS_PHP_STAT)))
    {
        /**Check if this is a counting profile - to be reversed*/
        if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_MATCH(gport) || BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(gport))
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit, DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, &outlif_entry_handle_id));

            SHR_IF_ERR_EXIT(dnx_stat_pp_set_virtual_lif_table_id_and_key
                            (unit, gport, gport_hw_resources.local_out_lif, &outlif_entry_handle_id));
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, outlif_entry_handle_id, DBAL_GET_ALL_FIELDS));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                                outlif_entry_handle_id,
                                                                DBAL_FIELD_STAT_PP_ETPP_IS_COUNTING_PROFILE,
                                                                INST_SINGLE, &is_counting_profile));
        }
        else
        {
            egress_lif_info_t egress_lif_info;
            uint8 found;

            SHR_IF_ERR_EXIT(lif_mngr_db.local_lif_info.
                            egress_lif_htb.find(unit, &gport_hw_resources.local_out_lif, &egress_lif_info, &found));
            if (!found)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "entry not found for local Out-LIF %d, found - %d",
                             gport_hw_resources.local_out_lif, found);
            }
            is_counting_profile = egress_lif_info.stat_pp_etpp_is_count_profile;
        }

        if (is_counting_profile)
        {
            int field_nof_bits;
            SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                            (unit, gport_hw_resources.outlif_dbal_table_id, DBAL_FIELD_STAT_OBJECT_ID, FALSE,
                             gport_hw_resources.outlif_dbal_result_type, INST_SINGLE, &field_nof_bits));
            STAT_PP_ETPP_SWITCH_CMD_GET(stat_info->stat_id, stat_pp_profile);
        }
    }
    STAT_PP_ENGINE_PROFILE_SET(stat_info->stat_pp_profile, stat_pp_profile, engine_source);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See stat_pp.h file for summary
 */
int
dnx_stat_pp_crps_verify(
    int unit,
    bcm_core_t core_id,
    uint32 stat_id,
    int stat_pp_profile,
    int interface_type)
{
    uint32 entry_handle_id;
    dbal_tables_e dbal_table_id;
    uint32 type_id;
    uint32 counter_command;
    uint32 max_cprs_counter_command;
    int profile_id;
    uint32 is_meter;
    uint32 max_pp_profile_value;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /**Exit in case of invalid pp profile*/
    if (STAT_PP_PROFILE_ID_GET(stat_pp_profile) == STAT_PP_PROFILE_INVALID)
    {
        SHR_EXIT();
    }

    if (interface_type == bcmStatCounterInterfaceIngressReceivePp)
    {
        dbal_table_id = DBAL_TABLE_STAT_PP_IRPP_PROFILE_INFO;
        max_pp_profile_value = STAT_PP_IRPP_PROFILE_MAX_VALUE;
        max_cprs_counter_command = STAT_PP_IRPP_MAX_CPRS_ITF_ID;
    }
    else if (interface_type == bcmStatCounterInterfaceEgressReceivePp)
    {
        
        SHR_ERR_EXIT(_SHR_E_PARAM, "ERPP counting engine is not supported");
    }
    else if (interface_type == bcmStatCounterInterfaceEgressTransmitPp)
    {
        dbal_table_id = DBAL_TABLE_STAT_PP_ETPP_PROFILE_INFO;
        max_pp_profile_value = STAT_PP_ETPP_PROFILE_MAX_VALUE;
        max_cprs_counter_command = STAT_PP_ETPP_MAX_CPRS_ITF_ID;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported engine source");
    }

    profile_id = STAT_PP_PROFILE_ID_GET(stat_pp_profile);
    if (profile_id > max_pp_profile_value)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal pp profile value %d", profile_id);
    }
    /** Get statistic type stored in HW for crps validation */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_CMD, profile_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                        entry_handle_id, DBAL_FIELD_STAT_IF_TYPE_ID, INST_SINGLE,
                                                        &type_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_IS_METER, INST_SINGLE, &is_meter));

    /** CRPS validation */
    /** if is_meter=TRUE, it doesn't have to be configure CRPS engine. therefore, skip validation */
    if (is_meter == FALSE)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                            entry_handle_id, DBAL_FIELD_STAT_IF_OBJECT_ID, INST_SINGLE,
                                                            &counter_command));
        /**Run crps validation only of interfaces which are mapped to the crps*/
        if (counter_command < max_cprs_counter_command)
        {
            SHR_INVOKE_VERIFY_DNX(dnx_crps_mgmt_counter_generation_verify
                                  (unit, core_id, interface_type, counter_command, type_id, stat_id));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See stat_pp.h file for summary
 */
shr_error_e
dnx_stat_pp_init(
    int unit)
{
    uint32 entry_handle_id;
    int stat_pp_profile;
    int statistic_if;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    stat_pp_profile = STAT_PP_PROFILE_INVALID;
    statistic_if = STAT_PP_STAT_IF_NO_STATISTICS_INDICATION;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STAT_PP_IRPP_PROFILE_INFO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_CMD, stat_pp_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_IF_OBJECT_ID, INST_SINGLE, statistic_if);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_STAT_PP_ETPP_PROFILE_INFO, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_CMD, stat_pp_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_IF_OBJECT_ID, INST_SINGLE, statistic_if);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * APIs
 * {
 */

/**
 * \brief - Delete a statistic profile according to the profile
 *        id. Use the source engine, which is taken from the 2
 *        MSBs of the stat_pp_profile to choose the relevant
 *        dbal table and resource manager. Use the profile id as
 *        key to the dbal table
 *
 * \param [in] unit - Relevant unit
 * \param [in] stat_pp_profile - An integer, containing the
 *        profile id and the engine source
 *
 * \return
 *    *   Negative in case of an error. See \ref shr_error_e,
 *           for example: profile is not found
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stat_pp_profile_delete(
    int unit,
    int stat_pp_profile)
{
    uint32 entry_handle_id;
    uint32 engine_source;
    uint32 profile_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    engine_source = STAT_PP_PROFILE_ENGINE_GET(stat_pp_profile);
    profile_id = STAT_PP_PROFILE_ID_GET(stat_pp_profile);

    SHR_INVOKE_VERIFY_DNX(dnx_stat_pp_profile_get_delete_verify(unit, engine_source, profile_id));

    switch (engine_source)
    {
        case bcmStatCounterInterfaceIngressReceivePp:
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STAT_PP_IRPP_PROFILE_INFO, &entry_handle_id));
            break;

        case bcmStatCounterInterfaceEgressReceivePp:
            
            SHR_ERR_EXIT(_SHR_E_PARAM, "ERPP counting engine is not supported");
            break;

        case bcmStatCounterInterfaceEgressTransmitPp:
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STAT_PP_ETPP_PROFILE_INFO, &entry_handle_id));
            break;

        default:
            break;
    }

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_CMD, profile_id);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(STAT_PP_SW_STATE_RESOURCE_EXEC_BY_ENGINE_PROFILE
                    (engine_source, profile_id, STAT_PP_IS_ETPP_METERING_PROFILE(profile_id),
                     free_single(unit, profile_id)));

exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Map statistic profile to profile properties
 * Profile id is saved in designated resource manager, profile
 * info in dbal tables: IRPP_STATISTIC_PROFILE_INFO,
 * ERPP_STATISTIC_PROFILE_INFO, ETPP_STATISTIC_PROFILE_INFO
 * \param [in] unit - Relevant unit
 * \param [in] flags - BCM_STAT_PP_PROFILE_WITH_ID - allocate with id
 *     BCM_STAT_PP_PROFILE_REPLACE - replace existing profile
 * \param [in] engine_source - counting source: IRPP, ERPP, ETPP
 * \param [in,out] stat_pp_profile - A pointer to an integer containing the profile id.
 *     Can be either passed empty (without id) or valid (with_id
 *     or replace flags are on). engine_source will be encoded
 *     on 2 MSBs
 * \param [in] stat_pp_profile_info - A struct contatining the profile info to be saved and
 *     mapped
 *
 * \return
 *   Negative in case of an error. See \ref shr_error_e,
 *           for example: REPLACE flag is on, but profile id
 *           doesn't exist.
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stat_pp_profile_create(
    int unit,
    int flags,
    bcm_stat_counter_interface_type_t engine_source,
    int *stat_pp_profile,
    bcm_stat_pp_profile_info_t * stat_pp_profile_info)
{
    int algo_flags;
    uint32 profile_id;

    SHR_FUNC_INIT_VARS(unit);
    algo_flags = 0;

    DNX_ERR_RECOVERY_START(unit);
    DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR_PTR(unit, sizeof(int), stat_pp_profile);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_stat_pp_profile_create_verify
                          (unit, flags, stat_pp_profile, engine_source, stat_pp_profile_info));

    if ((_SHR_IS_FLAG_SET(flags, BCM_STAT_PP_PROFILE_WITH_ID))
        || (_SHR_IS_FLAG_SET(flags, BCM_STAT_PP_PROFILE_REPLACE)))
    {
        algo_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
    }

    if (!(_SHR_IS_FLAG_SET(flags, BCM_STAT_PP_PROFILE_REPLACE)))
    {
        SHR_IF_ERR_EXIT(STAT_PP_SW_STATE_RESOURCE_EXEC_BY_ENGINE_PROFILE
                        (engine_source, *stat_pp_profile, stat_pp_profile_info->is_meter_enable,
                         allocate_single(unit, algo_flags, NULL, stat_pp_profile)));
    }

    profile_id = *stat_pp_profile;
    STAT_PP_ENGINE_PROFILE_SET(*stat_pp_profile, profile_id, engine_source);
    SHR_IF_ERR_EXIT(dnx_stat_pp_profile_hw_set(unit, stat_pp_profile_info, profile_id, engine_source));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get profile_info according to profile index
 * Use the engine source to choose the relevant dbal table and
 * resource manager resource, and the profile id as key to the
 * dbal table
 *
 * \param [in] unit - Relevant unit
 * \param [in] stat_pp_profile - statistics profile id, from
 *        which the profile id is retreived, along with the
 *        engine source
 * \param [in] stat_pp_profile_info - A strcut that will be
 *        filled with the retreived profile info
 *
 * \return
 *   Negative in case of an error. See \ref shr_error_e,
 *           for example: profile is not found
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stat_pp_profile_get(
    int unit,
    int stat_pp_profile,
    bcm_stat_pp_profile_info_t * stat_pp_profile_info)
{
    uint32 entry_handle_id;
    uint32 engine_source;
    uint32 profile_id;
    uint8 fec_profile;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(stat_pp_profile_info, _SHR_E_PARAM, "stat_pp_profile_info");
    sal_memset(stat_pp_profile_info, 0, sizeof(bcm_stat_pp_profile_info_t));

    engine_source = STAT_PP_PROFILE_ENGINE_GET(stat_pp_profile);
    profile_id = STAT_PP_PROFILE_ID_GET(stat_pp_profile);

    SHR_INVOKE_VERIFY_DNX(dnx_stat_pp_profile_get_delete_verify(unit, engine_source, profile_id));

    switch (engine_source)
    {
        case bcmStatCounterInterfaceIngressReceivePp:
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STAT_PP_IRPP_PROFILE_INFO, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_CMD, profile_id);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                                entry_handle_id,
                                                                DBAL_FIELD_STAT_IF_OBJECT_ID,
                                                                INST_SINGLE,
                                                                (uint32 *) &stat_pp_profile_info->counter_command_id));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                                entry_handle_id,
                                                                DBAL_FIELD_STAT_IF_TYPE_ID,
                                                                INST_SINGLE,
                                                                (uint32 *) &stat_pp_profile_info->stat_object_type));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get(unit,
                                                               entry_handle_id,
                                                               DBAL_FIELD_IS_METER,
                                                               INST_SINGLE, &stat_pp_profile_info->is_meter_enable));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get(unit,
                                                               entry_handle_id,
                                                               DBAL_FIELD_MAPPING_REQUIRED,
                                                               INST_SINGLE, &stat_pp_profile_info->is_fp_cs_var));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                                entry_handle_id,
                                                                DBAL_FIELD_STAT_OBJECT_METADATA_SIZE,
                                                                INST_SINGLE,
                                                                (uint32 *)
                                                                &stat_pp_profile_info->ingress_tunnel_metadata_size));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_METADATA_SHIFT, INST_SINGLE,
                             (uint32 *) &stat_pp_profile_info->ingress_tunnel_metadata_start_bit));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_FWD_METADATA_SIZE, INST_SINGLE,
                             (uint32 *) &stat_pp_profile_info->ingress_forward_metadata_size));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_FWD_METADATA_SHIFT, INST_SINGLE,
                             (uint32 *) &stat_pp_profile_info->ingress_forward_metadata_start_bit));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_FWD_PLUS_ONE_METADATA_SIZE, INST_SINGLE,
                             (uint32 *) &stat_pp_profile_info->ingress_fwd_plus_one_metadata_size));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_FWD_PLUS_ONE_METADATA_SHIFT, INST_SINGLE,
                             (uint32 *) &stat_pp_profile_info->ingress_fwd_plus_one_metadata_start_bit));

            /*
             * Check if this profile was created for protection fecs as well
             */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR
                            (unit, DBAL_TABLE_STAT_PP_IRPP_FEC_ECMP_STATISTICS_PROFILE_MAP, entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_STAT_OBJECT_PROFILE, profile_id);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get(unit,
                                                               entry_handle_id,
                                                               DBAL_FIELD_FEC_STATISTIC_PROFILE_MAP,
                                                               INST_SINGLE, &fec_profile));

            if (fec_profile == DBAL_ENUM_FVAL_FEC_STATISTIC_PROFILE_MAP_SINGLE_INDIRECT_COUNTERS)
            {
                stat_pp_profile_info->is_protection_fec = TRUE;
            }

            break;

        case bcmStatCounterInterfaceEgressTransmitPp:
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STAT_PP_ETPP_PROFILE_INFO, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_CMD, profile_id);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                                entry_handle_id,
                                                                DBAL_FIELD_STAT_IF_OBJECT_ID,
                                                                INST_SINGLE,
                                                                (uint32 *) &stat_pp_profile_info->counter_command_id));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                                entry_handle_id,
                                                                DBAL_FIELD_STAT_IF_TYPE_ID,
                                                                INST_SINGLE,
                                                                (uint32 *) &stat_pp_profile_info->stat_object_type));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get(unit,
                                                               entry_handle_id,
                                                               DBAL_FIELD_IS_METER,
                                                               INST_SINGLE, &stat_pp_profile_info->is_meter_enable));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                                entry_handle_id,
                                                                DBAL_FIELD_METER_INTERFACE,
                                                                INST_SINGLE,
                                                                (uint32 *) &stat_pp_profile_info->meter_command_id));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                                entry_handle_id,
                                                                DBAL_FIELD_METER_QOS_PROFILE,
                                                                INST_SINGLE,
                                                                (uint32 *) &stat_pp_profile_info->meter_qos_map_id));
            break;

        case bcmStatCounterInterfaceEgressReceivePp:
            
            SHR_ERR_EXIT(_SHR_E_PARAM, "ERPP counting engine is not supported");
            break;

        default:
            break;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Map physical or logical port to stat id and stat
 *        profile. Incase of DPC LIF the api can accept core
 *        parameter to distinguish between the stat id on the
 *        two cores
 *
 * \param [in] unit - relevant unit
 * \param [in] gport - physical or logical port
 * \param [in] core_id - core id
 * \param [in] engine_source - counting source (IRPP, ERPP,
 *        ETPP)
 * \param [in] stat_info - statistic information per object
 *
 * \return
 *   Negative in case of an error
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_gport_stat_set(
    int unit,
    bcm_gport_t gport,
    bcm_core_t core_id,
    bcm_stat_counter_interface_type_t engine_source,
    bcm_stat_pp_info_t stat_info)
{
    uint8 is_physical_port;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_stat_pp_gport_set_verify(unit, gport, core_id, engine_source, stat_info));

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, gport, &is_physical_port));

    if (is_physical_port)
    {
        SHR_IF_ERR_EXIT(dnx_stat_pp_physical_gport_set(unit, gport, core_id, engine_source, stat_info));
    }
    /** gport is lif */
    else
    {
        SHR_IF_ERR_EXIT(dnx_stat_pp_logical_gport_set(unit, gport, engine_source, stat_info));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get mapping of physical or logical port to stat id
 *        and stat profile. In case of DPC LIF the api can
 *        accept core parameter to distinguish between the stat
 *        id on the two cores
 *
 * \param [in] unit - relevant unit
 * \param [in] gport - physical or logical port
 * \param [in] core_id - core id
 * \param [in] engine_source - counting source (IRPP, ERPP,
 *        ETPP)
 * \param [in] stat_info - statistic information per object
 *
 * \return
 *   Negative in case of an error
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_gport_stat_get(
    int unit,
    bcm_gport_t gport,
    bcm_core_t core_id,
    bcm_stat_counter_interface_type_t engine_source,
    bcm_stat_pp_info_t * stat_info)
{
    uint8 is_physical_port;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_stat_pp_gport_get_delete_verify(unit, engine_source));

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, gport, &is_physical_port));
    if (is_physical_port)
    {
        SHR_IF_ERR_EXIT(dnx_stat_pp_physical_gport_get(unit, gport, core_id, engine_source, stat_info));
    }
    /** gport is lif */
    else
    {
        SHR_IF_ERR_EXIT(dnx_stat_pp_logical_gport_get(unit, gport, engine_source, stat_info));
    }

exit:
    SHR_FUNC_EXIT;
}
