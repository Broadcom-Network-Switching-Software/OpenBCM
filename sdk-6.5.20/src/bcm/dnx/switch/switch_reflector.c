/**
 * \file switch_reflector.c
 *
 * Reflector Switch API implementation.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SWITCH

/*
 * Include files.
 * {
 */
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm/switch.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <soc/dnx/swstate/auto_generated/access/reflector_access.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

/* IPv4(20)+UDP(8)+TWAMP(41) */
#define TWAMP_IPV4_TERM_DEPTH   69
/* IPv6(40)+UDP(8)+TWAMP(41) */
#define TWAMP_IPV6_TERM_DEPTH   89

/*
 * }
 */
/*
 * See .h
 */
shr_error_e
dnx_switch_reflector_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** init sw state */
    SHR_IF_ERR_EXIT(reflector_sw_db_info.init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Set OUTLIF used for reflectors.
 *  The table and data to set depends in data->type
 *  side.
 * \param [in] unit  - unit #.
 * \param [in] local_lif - local LIF #
 * \param [in] data - data to set
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
*/
static shr_error_e
dnx_switch_reflector_reflector_encap_set(
    int unit,
    int local_lif,
    bcm_switch_reflector_data_t * data)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_REFLECTOR, &entry_handle_id));

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_lif);

    /** Set DATA fields */
    switch (data->type)
    {
        case bcmSwitchReflectorUc:
        case bcmSwitchReflectorSbfdIp4:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         DBAL_RESULT_TYPE_EEDB_REFLECTOR_ETPS_REFLECTOR);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RAW_DATA, INST_SINGLE,
                                         DBAL_ENUM_FVAL_ETPP_TRAP_ACE_CTXT_VALUE_OVERRIDE_DROP);
            break;
        case bcmSwitchReflectorL2UcInternal:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         DBAL_RESULT_TYPE_EEDB_REFLECTOR_ETPS_REFLECTOR);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RAW_DATA, INST_SINGLE,
                                         DBAL_ENUM_FVAL_ETPP_TRAP_ACE_CTXT_VALUE_RECYCLE_W_PTCH2);
            break;
        case bcmSwitchReflectorL2McInternal:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         DBAL_RESULT_TYPE_EEDB_REFLECTOR_ETPS_REFLECTOR_L2_MC);
            dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_L2_MAC, INST_SINGLE,
                                            data->mc_reflector_my_mac);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RAW_DATA, INST_SINGLE,
                                         DBAL_ENUM_FVAL_ETPP_TRAP_ACE_CTXT_VALUE_RECYCLE_W_PTCH2);
            break;
        case bcmSwitchReflectorL2McExternal:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         DBAL_RESULT_TYPE_EEDB_REFLECTOR_ETPS_REFLECTOR_L2_MC);
            dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_L2_MAC, INST_SINGLE,
                                            data->mc_reflector_my_mac);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RAW_DATA, INST_SINGLE,
                                         DBAL_ENUM_FVAL_ETPP_TRAP_ACE_CTXT_VALUE_NULL);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "unexpected reflector type %d.\n", data->type);
            break;
    }

    /** Write to HW */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Set OUTLIF used for TWAMP reflectors.
 *  The table and data to set depends in data->type
 *  side.
 * \param [in] unit  - unit #.
 * \param [in] local_lif - local LIF #
 * \param [in] data - data to set
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
*/
static shr_error_e
dnx_switch_reflector_twamp_encap_set(
    int unit,
    int local_lif,
    bcm_switch_reflector_data_t * data)
{
    uint32 entry_handle_id;
    uint32 termination_depth;
    int lif_get_flags = 0;
    bcm_gport_t gport;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_TWAMP, &entry_handle_id));

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_lif);

    /** Set DATA fields */
    lif_get_flags =
        DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, data->next_encap_id);
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, lif_get_flags, &gport_hw_resources),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);
    if (gport_hw_resources.outlif_dbal_table_id != DBAL_TABLE_EEDB_RCH)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal to replace non RCH OutLIF to RCH OutLIF");
    }
    termination_depth = ((data->type == bcmSwitchReflectorTwampIp6) ? TWAMP_IPV6_TERM_DEPTH : TWAMP_IPV4_TERM_DEPTH);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_TWAMP_ETPS_REFLECTOR_TWAMP);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ERROR_ESTIMATE, INST_SINGLE, data->error_estimate);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TERMINATION_DEPTH, INST_SINGLE, termination_depth);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE,
                                 gport_hw_resources.local_out_lif);

    /** Write to HW */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Set OUTLIF used for sbfd reflector over mpls
 *  The only field required to be set is the opcode to stamp (reflector requires LBR=2)
 *  side.
 * \param [in] unit  - unit #.
 * \param [in] local_lif - local LIF #
 * \param [in] data - data to set
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
*/
static shr_error_e
dnx_sbfd_reflector_mpls_encap_set(
    int unit,
    int local_lif,
    bcm_switch_reflector_data_t * data)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_OAM_REFLECTOR, &entry_handle_id));

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_lif);

    /** Set DATA fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_OAM_REFLECTOR_ETPS_REFLECTOR_OAM);
    /** Write to HW */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Set OUTLIF used for reflectors.
 *  The table and data to set depends in data->type
 *  side.
 * \param [in] unit  - unit #.
 * \param [in] local_lif - local LIF #
 * \param [in] data - data to set
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
*/
static shr_error_e
dnx_switch_reflector_encap_set(
    int unit,
    int local_lif,
    bcm_switch_reflector_data_t * data)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if ((data->type == bcmSwitchReflectorTwampIp4) || (data->type == bcmSwitchReflectorTwampIp6))
    {
        SHR_IF_ERR_EXIT(dnx_switch_reflector_twamp_encap_set(unit, local_lif, data));
    }
    else if (data->type == bcmSwitchReflectorSbfdMpls)
    {
        SHR_IF_ERR_EXIT(dnx_sbfd_reflector_mpls_encap_set(unit, local_lif, data));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_switch_reflector_reflector_encap_set(unit, local_lif, data));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Get outlif data
 *  The table and data to set depends in result_type
 *  side.
 * \param [in] unit  - unit #.
 * \param [in] local_lif - local LIF #
 * \param [in] result_type - result type of outlif table
 * \param [in] data - data to set
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
*/
static shr_error_e
dnx_switch_reflector_reflector_encap_get(
    int unit,
    int local_lif,
    dbal_fields_e result_type,
    bcm_switch_reflector_data_t * data)
{
    uint32 entry_handle_id;
    uint32 ace_context;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /** init output structure */
    bcm_switch_reflector_data_t_init(data);

    switch (result_type)
    {
        case DBAL_RESULT_TYPE_EEDB_REFLECTOR_ETPS_REFLECTOR:
            /** get tyoe - for UC reflectors no other data besides type*/
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_REFLECTOR, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_lif);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         DBAL_RESULT_TYPE_EEDB_REFLECTOR_ETPS_REFLECTOR);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RAW_DATA, INST_SINGLE, &ace_context);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

            if (ace_context == DBAL_ENUM_FVAL_ETPP_TRAP_ACE_CTXT_VALUE_RECYCLE_W_PTCH2)
            {
                data->type = bcmSwitchReflectorL2UcInternal;
            }
            else
            {
                data->type = bcmSwitchReflectorUc;
            }
            break;
        case DBAL_RESULT_TYPE_EEDB_REFLECTOR_ETPS_REFLECTOR_L2_MC:
            /** Take DBAL handle */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_REFLECTOR, &entry_handle_id));

            /** Set KEY field */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_lif);
            /** get values*/
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         DBAL_RESULT_TYPE_EEDB_REFLECTOR_ETPS_REFLECTOR_L2_MC);
            dbal_value_field_arr8_request(unit, entry_handle_id, DBAL_FIELD_L2_MAC, INST_SINGLE,
                                          data->mc_reflector_my_mac);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RAW_DATA, INST_SINGLE, &ace_context);
            /** get */
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

            if (ace_context == DBAL_ENUM_FVAL_ETPP_TRAP_ACE_CTXT_VALUE_RECYCLE_W_PTCH2)
            {
                data->type = bcmSwitchReflectorL2McInternal;
            }
            else
            {
                data->type = bcmSwitchReflectorL2McExternal;
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "unexpected reflector result type %d.\n", result_type);
            break;

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Get outlif data
 *  The table and data to set depends in result_type
 *  side.
 * \param [in] unit  - unit #.
 * \param [in] local_lif - local LIF #
 * \param [in] result_type - result type of outlif table
 * \param [in] data - data to set
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
*/
static shr_error_e
dnx_switch_reflector_twamp_encap_get(
    int unit,
    int local_lif,
    dbal_fields_e result_type,
    bcm_switch_reflector_data_t * data)
{
    uint32 entry_handle_id;
    uint32 termination_depth;
    uint32 next_local_lif;
    bcm_gport_t next_global_lif_gport = BCM_GPORT_INVALID;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /** init output structure */
    bcm_switch_reflector_data_t_init(data);

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_TWAMP, &entry_handle_id));

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_lif);
    /** get values*/
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_TWAMP_ETPS_REFLECTOR_TWAMP);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ERROR_ESTIMATE, INST_SINGLE, &(data->error_estimate));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TERMINATION_DEPTH, INST_SINGLE, &termination_depth);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE, &next_local_lif);
    /** get */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if (termination_depth == TWAMP_IPV6_TERM_DEPTH)
    {
        data->type = bcmSwitchReflectorTwampIp6;
    }
    else
    {
        data->type = bcmSwitchReflectorTwampIp4;
    }

    /** map local to global LIF */
    /** In case local lif is not found, return next pointer 0 */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit,
                                                DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS |
                                                DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT, _SHR_CORE_ALL,
                                                next_local_lif, &next_global_lif_gport));
    if (next_global_lif_gport != BCM_GPORT_INVALID)
    {
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(data->next_encap_id, next_global_lif_gport);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Get outlif data
 *  The table and data to set depends in result_type
 *  side.
 * \param [in] unit  - unit #.
 * \param [in] local_lif - local LIF #
 * \param [in] table_id - table id
 * \param [in] result_type - result type of outlif table
 * \param [in] data - data to set
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
*/
static shr_error_e
dnx_switch_reflector_encap_get(
    int unit,
    int local_lif,
    dbal_tables_e table_id,
    dbal_fields_e result_type,
    bcm_switch_reflector_data_t * data)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (table_id == DBAL_TABLE_EEDB_TWAMP)
    {
        SHR_IF_ERR_EXIT(dnx_switch_reflector_twamp_encap_get(unit, local_lif, result_type, data));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_switch_reflector_reflector_encap_get(unit, local_lif, result_type, data));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Set OUTLIF used for reflectors.
 *  The table and data to set depends in data->type
 *  side.
 * \param [in] unit  - unit #.
 * \param [in] local_lif - local LIF #
 * \param [in] type - reflector type
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
*/
static shr_error_e
dnx_switch_reflector_reflector_encap_clear(
    int unit,
    int local_lif,
    bcm_switch_reflector_type_t type)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_REFLECTOR, &entry_handle_id));

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_lif);

    /** Set DATA fields */
    switch (type)
    {
        case bcmSwitchReflectorUc:
        case bcmSwitchReflectorL2UcInternal:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         DBAL_RESULT_TYPE_EEDB_REFLECTOR_ETPS_REFLECTOR);
            break;
        case bcmSwitchReflectorL2McExternal:
        case bcmSwitchReflectorL2McInternal:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         DBAL_RESULT_TYPE_EEDB_REFLECTOR_ETPS_REFLECTOR_L2_MC);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "unexpected reflector type %d.\n", type);
            break;
    }

    /** Write to HW */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Set OUTLIF used for reflectors.
 *  The table and data to set depends in data->type
 *  side.
 * \param [in] unit  - unit #.
 * \param [in] local_lif - local LIF #
 * \param [in] type - reflector type
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
*/
static shr_error_e
dnx_switch_reflector_twamp_encap_clear(
    int unit,
    int local_lif,
    bcm_switch_reflector_type_t type)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_TWAMP, &entry_handle_id));

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_lif);

    /** Set DATA fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_TWAMP_ETPS_REFLECTOR_TWAMP);

    /** Write to HW */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Set OUTLIF used for reflectors.
 *  The table and data to set depends in data->type
 *  side.
 * \param [in] unit  - unit #.
 * \param [in] local_lif - local LIF #
 * \param [in] type - reflector type
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
*/
static shr_error_e
dnx_switch_reflector_encap_clear(
    int unit,
    int local_lif,
    bcm_switch_reflector_type_t type)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take DBAL handle */
    if ((type == bcmSwitchReflectorTwampIp4) || (type == bcmSwitchReflectorTwampIp6))
    {
        SHR_IF_ERR_EXIT(dnx_switch_reflector_twamp_encap_clear(unit, local_lif, type));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_switch_reflector_reflector_encap_clear(unit, local_lif, type));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *  Verification of dnx_switch_reflector_get_verify() input parameters.
 *  For details about the parameters refer to dnx_switch_reflector_get_verify()
*/
static int
dnx_switch_reflector_get_verify(
    int unit,
    uint32 flags,
    bcm_if_t encap_id,
    bcm_switch_reflector_data_t * data)
{
    uint32 supported_flags;
    int global_lif_id;
    SHR_FUNC_INIT_VARS(unit);

    /** pointers */
    SHR_NULL_CHECK(data, _SHR_E_PARAM, "data");

    /** flags */
    supported_flags = 0;
    SHR_MASK_VERIFY(flags, supported_flags, _SHR_E_PARAM, "some of the flags are not supported \n");

    /** encap_id */
    /** range check */
    global_lif_id = BCM_L3_ITF_VAL_GET(encap_id);
    LIF_MNGR_GLOBAL_OUT_LIF_VERIFY(unit, global_lif_id);

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *  Get OUTLIF data of reflector applications
 * \param [in] unit  - unit #.
 * \param [in] flags  - Not used, set to 0.
 * \param [in] encap_id - encap allocated by API bcm_dnx_switch_reflector_create()
 *
 * \param [in] data - OUTLIF data:\n
 *                    type of the OUTLIF, the following are supported:\n
 *                    - bcmSwitchReflectorUc - used for unicast L2 external and unicast L3 internal reflectors\n
 *                    - bcmSwitchReflectorL2McExternal - used for L2 external multicast reflector\n
 *                    - bcmSwitchReflectorL2McInternal - used for L2 internal multicast reflector\n
 *                    mc_reflector_my_mac - source MAC to stamp the packet. used only by multicast reflectors.\n
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_switch_reflector_get(
    int unit,
    uint32 flags,
    bcm_if_t encap_id,
    bcm_switch_reflector_data_t * data)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint32 hw_resources_flags;
    bcm_gport_t gport;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_switch_reflector_get_verify(unit, flags, encap_id, data));

    /** get local LIF and result_type */
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, encap_id);
    hw_resources_flags =
        DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, hw_resources_flags, &gport_hw_resources));
    if ((gport_hw_resources.outlif_dbal_table_id != DBAL_TABLE_EEDB_REFLECTOR) &&
        (gport_hw_resources.outlif_dbal_table_id != DBAL_TABLE_EEDB_TWAMP))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Expecting reflector encap ID, got 0x%x", encap_id);
    }

    /** get the data */
    SHR_IF_ERR_EXIT(dnx_switch_reflector_encap_get
                    (unit, gport_hw_resources.local_out_lif, gport_hw_resources.outlif_dbal_table_id,
                     gport_hw_resources.outlif_dbal_result_type, data));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *  Verification of bcm_dnx_switch_reflector_create() input parameters.
 *  For details about the parameters refer to bcm_dnx_switch_reflector_create()
*/
static int
dnx_switch_reflector_create_verify(
    int unit,
    uint32 flags,
    bcm_if_t * encap_id,
    bcm_switch_reflector_data_t * data)
{
    uint32 supported_flags;
    int is_allocated;
    bcm_switch_reflector_data_t prev_data;
    SHR_FUNC_INIT_VARS(unit);

    /** flags */
    supported_flags = BCM_SWITCH_REFELCTOR_WITH_ID | BCM_SWITCH_REFELCTOR_REPLACE;
    SHR_MASK_VERIFY(flags, supported_flags, _SHR_E_PARAM, "some of the flags are not supported \n");

    /** pointers verify */
    SHR_NULL_CHECK(encap_id, _SHR_E_PARAM, "encap_id");
    SHR_NULL_CHECK(data, _SHR_E_PARAM, "data");

    /** type verify */
    switch (data->type)
    {
        case bcmSwitchReflectorUc:
        case bcmSwitchReflectorL2UcInternal:
        case bcmSwitchReflectorL2McExternal:
        case bcmSwitchReflectorL2McInternal:
        case bcmSwitchReflectorTwampIp4:
        case bcmSwitchReflectorTwampIp6:
        case bcmSwitchReflectorSbfdIp4:
        case bcmSwitchReflectorSbfdMpls:
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected reflector type %d.\n", data->type);
            break;
    }

    /** mc mac verify*/
    if (data->type != bcmSwitchReflectorL2McExternal && data->type != bcmSwitchReflectorL2McInternal)
    {
        if (!BCM_MAC_IS_ZERO(data->mc_reflector_my_mac))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "mc_reflector_my_mac is relevant for MC types only.\n");
        }
    }

    /** error estimate and next encap id verify*/
    if (data->type != bcmSwitchReflectorTwampIp4 && data->type != bcmSwitchReflectorTwampIp6)
    {
        if (data->error_estimate != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "error_estimate is relevant for TWAMP types only.\n");
        }

        if (data->next_encap_id != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "next_encap_id is relevant for TWAMP types only.\n");
        }
    }

    /** replace verify*/
    if (flags & BCM_SWITCH_REFELCTOR_REPLACE)
    {
        /** make sure that flags with_id set */
        if ((flags & BCM_SWITCH_REFELCTOR_WITH_ID) == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "For replace operation, flag BCM_SWITCH_REFELCTOR_WITH_ID must be set, .\n");
        }

        /** make sure already exist */
        SHR_IF_ERR_EXIT(bcm_dnx_switch_reflector_get(unit, 0, *encap_id, &prev_data));

        /** make sure type was not changed */
        if (prev_data.type != data->type)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "reflectors types cannot be changed on replace operation.\n");
        }

    }

    /** make sure that no other encap allocated for UC reflector */
    if ((data->type == bcmSwitchReflectorUc) || (data->type == bcmSwitchReflectorL2UcInternal))
    {
        SHR_IF_ERR_EXIT(reflector_sw_db_info.uc.is_allocated.get(unit, &is_allocated));
        if (is_allocated == 1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "encap ID for UC reflector already allocated - just a single global ID can be allocated for UC reflector.\n");
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Allocate OUTLIF for reflector applications
 * \param [in] unit  - unit #.
 * \param [in] flags  - Use BCM_SWITCH_REFELCTOR_WITH_ID to allocate a specific OUTLIF or 0 otherwise.\n
 *                      Use BCM_SWITCH_REFELCTOR_REPLACEto modify data.
 * \param [in,out] encap_id - IN: Specify the required OUTLIF in a case flag BCM_SWITCH_REFELCTOR_WITH_ID set.\n
 *                            OUT: the allocated OUTTLIF
 * \param [in] data - OUTLIF data:\n
 *                    type of the OUTLIF, the following are supported:\n
 *                    - bcmSwitchReflectorUc - used for unicast L2 external and unicast L3 internal reflectors\n
 *                                             Just a single global ID can be allocated for UC reflector.
 *                                             This global ID can be used for both L2 and L3 unicast reflectors.
 *                    - bcmSwitchReflectorL2UcInternal - used for L2 internal unicast reflector\n
 *                    - bcmSwitchReflectorL2McExternal - used for L2 external multicast reflector\n
 *                    - bcmSwitchReflectorL2McInternal - used for L2 internal multicast reflector\n 
 *                    - bcmSwitchReflectorTwampIp4 - used for  TWAMP reflector over IPv4\n
 *                    - bcmSwitchReflectorTwampIp6 - used for  TWAMP reflector over IPv6\n
 *                    mc_reflector_my_mac - source MAC to stamp the packet. used only by multicast reflectors.\n
 *                    error_estimate - ERROR_ESTIMATE.\n
 *                    next_encap_id - Next pointer encap id for the RCH Ethernet encapsulation.\n
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_switch_reflector_create(
    int unit,
    uint32 flags,
    bcm_if_t * encap_id,
    bcm_switch_reflector_data_t * data)
{
    lif_mngr_local_outlif_info_t outlif_info;
    uint32 lif_alloc_flags;
    int global_lif_id;
    dbal_tables_e outlif_table;
    dbal_fields_e outlif_field;
    uint32 lif_get_flags;
    bcm_gport_t gport;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_switch_reflector_create_verify(unit, flags, encap_id, data));

    /** get outlif table info per type */
    switch (data->type)
    {
        case bcmSwitchReflectorUc:
        case bcmSwitchReflectorSbfdIp4:
        case bcmSwitchReflectorL2UcInternal:
            outlif_table = DBAL_TABLE_EEDB_REFLECTOR;
            outlif_field = DBAL_RESULT_TYPE_EEDB_REFLECTOR_ETPS_REFLECTOR;
            break;
        case bcmSwitchReflectorL2McInternal:
        case bcmSwitchReflectorL2McExternal:
            outlif_table = DBAL_TABLE_EEDB_REFLECTOR;
            outlif_field = DBAL_RESULT_TYPE_EEDB_REFLECTOR_ETPS_REFLECTOR_L2_MC;
            break;
        case bcmSwitchReflectorTwampIp4:
        case bcmSwitchReflectorTwampIp6:
            outlif_table = DBAL_TABLE_EEDB_TWAMP;
            outlif_field = DBAL_RESULT_TYPE_EEDB_TWAMP_ETPS_REFLECTOR_TWAMP;
            break;
        case bcmSwitchReflectorSbfdMpls:
            outlif_table = DBAL_TABLE_EEDB_OAM_REFLECTOR;
            outlif_field = DBAL_RESULT_TYPE_EEDB_OAM_REFLECTOR_ETPS_REFLECTOR_OAM;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "unexpected reflector type %d.\n", data->type);
            break;
    }

    /** Allocate OUTLIF: both global and local. */
    lif_alloc_flags = 0;
    if (flags & BCM_SWITCH_REFELCTOR_WITH_ID)
    {
        lif_alloc_flags |= LIF_MNGR_GLOBAL_LIF_WITH_ID;
        global_lif_id = BCM_L3_ITF_VAL_GET(*encap_id);
    }
    sal_memset(&outlif_info, 0, sizeof(lif_mngr_local_outlif_info_t));
    /** type to table and result type */
    outlif_info.dbal_table_id = outlif_table;
    outlif_info.dbal_result_type = outlif_field;
    outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_REFLECTOR;

    if ((flags & BCM_SWITCH_REFELCTOR_REPLACE) == 0)
    {
        /** Allocate LIF - WITH_ID if the flag was set */
        SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, lif_alloc_flags, &global_lif_id, NULL, &outlif_info));
        BCM_L3_ITF_SET(*encap_id, BCM_L3_ITF_TYPE_LIF, global_lif_id);

        /** Set OUTLIF table*/
        SHR_IF_ERR_EXIT(dnx_switch_reflector_encap_set(unit, outlif_info.local_outlif, data));

        /** Write global to local LIF mapping to GLEM. */
        SHR_IF_ERR_EXIT(dnx_lif_lib_add_to_glem(unit, BCM_CORE_ALL, global_lif_id, outlif_info.local_outlif));

        /** store global UC reflector in sw state */
        if ((data->type == bcmSwitchReflectorUc) || (data->type == bcmSwitchReflectorL2UcInternal))
        {
            SHR_IF_ERR_EXIT(reflector_sw_db_info.uc.encap_id.set(unit, *encap_id));
            SHR_IF_ERR_EXIT(reflector_sw_db_info.uc.is_allocated.set(unit, 1));
        }
    }
    else
    {
        /** replace - just modify data */
        /** get GPort HW resources (global and local tunnel outlif) */
        lif_get_flags =
            DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;

        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, *encap_id);

        /** we don't have gport here, so we're using gport tunnel */
        sal_memset(&gport_hw_resources, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
        SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, lif_get_flags, &gport_hw_resources),
                                    _SHR_E_NOT_FOUND, _SHR_E_PARAM);

        /** Check that eep is an index of a RCH OutLIF (in SW DB) */
        if (gport_hw_resources.outlif_dbal_table_id != outlif_table)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Illegal to replace non REFLECTOR OutLIF to REFLECTOR OutLIF");
        }

        /** Set OUTLIF table*/
        SHR_IF_ERR_EXIT(dnx_switch_reflector_encap_set(unit, gport_hw_resources.local_out_lif, data));

    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *  Verification of dnx_switch_reflector_destroy_verify() input parameters.
 *  For details about the parameters refer to dnx_switch_reflector_destroy_verify()
*/
static int
dnx_switch_reflector_destroy_verify(
    int unit,
    uint32 flags,
    bcm_if_t encap_id)
{
    uint32 supported_flags;
    bcm_switch_reflector_data_t prev_data;
    SHR_FUNC_INIT_VARS(unit);

    /** flags */
    supported_flags = 0;
    SHR_MASK_VERIFY(flags, supported_flags, _SHR_E_PARAM, "some of the flags are not supported \n");

    /** make sure already exist */
    SHR_IF_ERR_EXIT(dnx_switch_reflector_get_verify(unit, 0, encap_id, &prev_data));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Destroy OUTLIF of reflector applications
 * \param [in] unit  - unit #.
 * \param [in] flags  - Not used, set to 0.
 * \param [in] encap_id - encap allocated by API bcm_dnx_switch_reflector_create()
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_switch_reflector_destroy(
    int unit,
    uint32 flags,
    bcm_if_t encap_id)
{
    uint32 local_lif, global_lif_id;
    uint32 hw_resources_flags;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    bcm_gport_t gport;
    bcm_switch_reflector_data_t prev_data;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_switch_reflector_destroy_verify(unit, flags, encap_id));

    /** get global lif and local lif*/
    global_lif_id = BCM_L3_ITF_VAL_GET(encap_id);
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, encap_id);
    hw_resources_flags =
        DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, hw_resources_flags, &gport_hw_resources));
    local_lif = gport_hw_resources.local_out_lif;

    /** Remove global to local LIF mapping from GLEM. */
    SHR_IF_ERR_EXIT(dnx_lif_lib_remove_from_glem(unit, BCM_CORE_ALL, global_lif_id));

    /** Clear table values */
    SHR_IF_ERR_EXIT(bcm_dnx_switch_reflector_get(unit, 0, encap_id, &prev_data));
    SHR_IF_ERR_EXIT(dnx_switch_reflector_encap_clear(unit, local_lif, prev_data.type));

    /** Free LIF */
    SHR_IF_ERR_EXIT(dnx_lif_lib_free(unit, global_lif_id, NULL, local_lif));

    /** mark as not allocated */
    if ((prev_data.type == bcmSwitchReflectorUc) || (prev_data.type == bcmSwitchReflectorL2UcInternal))
    {
        SHR_IF_ERR_EXIT(reflector_sw_db_info.uc.encap_id.set(unit, 0));
        SHR_IF_ERR_EXIT(reflector_sw_db_info.uc.is_allocated.set(unit, 0));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verification of dnx_switch_reflector_traverse_verify() input parameters.
 *  For details about the parameters refer to dnx_switch_reflector_traverse_verify()
*/
static int
dnx_switch_reflector_traverse_verify(
    int unit,
    uint32 flags,
    bcm_switch_reflector_traverse_cb traverse_cb,
    void *user_data)
{
    uint32 supported_flags;
    SHR_FUNC_INIT_VARS(unit);

    /** flags */
    supported_flags = 0;
    SHR_MASK_VERIFY(flags, supported_flags, _SHR_E_PARAM, "some of the flags are not supported \n");

    /**pointers */
    SHR_NULL_CHECK(traverse_cb, _SHR_E_PARAM, "traverse_cb");

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *  Traverse reflector OUTLIF
 * \param [in] unit  - unit #.
 * \param [in] flags  - Not used, set to 0.
 * \param [in] traverse_cb - callback for traverse operation
 * \param [in] user_data - any user data
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_switch_reflector_traverse(
    int unit,
    uint32 flags,
    bcm_switch_reflector_traverse_cb traverse_cb,
    void *user_data)
{
    uint32 entry_handle_id = 0;
    uint32 local_out_lif = 0;
    uint32 table_id = 0;
    int is_end = 0;
    dbal_tables_e dbal_tables[] = { DBAL_TABLE_EEDB_REFLECTOR, DBAL_TABLE_EEDB_TWAMP };
    uint32 nof_tables = COUNTOF(dbal_tables);
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    bcm_gport_t next_gport;
    bcm_if_t encap_id;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_switch_reflector_traverse_verify(unit, flags, traverse_cb, user_data));

    for (table_id = 0; table_id < nof_tables; table_id++)
    {
        /** traverse operation **/
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_tables[table_id], &entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
        /*
         * Receive first entry in table.
         */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        while (!is_end)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_OUT_LIF, &local_out_lif));

            /*
             * Verify the out lif is valid
             */
            if (local_out_lif != 0)
            {
                next_gport = BCM_GPORT_INVALID;

                /** In case local lif is not found, return next pointer 0 */
                SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit,
                                                            DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS |
                                                            DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT, _SHR_CORE_ALL,
                                                            local_out_lif, &next_gport));
                if (next_gport != BCM_GPORT_INVALID)
                {

                    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                    (unit, next_gport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS,
                                     &gport_hw_resources));
                    BCM_L3_ITF_SET(encap_id, BCM_L3_ITF_TYPE_LIF, gport_hw_resources.global_out_lif);

                    /** Invoke callback function */
                    (*traverse_cb) (unit, encap_id, user_data);
                }
            }
            /*
             * Receive next entry in table.
             */
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
