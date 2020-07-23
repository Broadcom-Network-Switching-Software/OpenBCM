/** \file instru_ifa1.c
 * 
 *
 * Visibility procedures for DNX in IFA module.
 *
 * This file contains functions for visibility configuration.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INSTRU
/*
 * Include
 * {
 */

#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/instru_access.h>

#include <shared/shrextend/shrextend_debug.h>

#include <bcm_int/dnx/instru/instru.h>
#include <bcm_int/dnx/algo/algo_gpm.h>

#include <bcm/types.h>

#include <bcm/ifa.h>
/*
 * }
 */

/*
 * Function Declaration.
 * {
 */

/*
 * }
 */

/*
 * Defines.
 * {
 */

/** Default Probe Header and MetaData Values */
#define DEFAULT_IFA_VERSION     0x1
#define DEFAULT_DEVICE_ID       0xFFFF
#define DEFAULT_HOP_LIMIT       0xff
#define DEFAULT_MAX_IFA_LENGTH  0x1000
#define DEFAULT_PROBE_MARKER_1  0xFFFFFFFF
#define DEFAULT_PROBE_MARKER_2  0xFFFFFFFF
#define DEFAULT_IFA_SENDER      0xFFFF
#define DEFAULT_IFA_TEMPLATE_ID 0x2

/** Constant Probe Header Values */
/* Init Value for Length is the length of Probe Header */
#define CONST_CURRENT_LENGTH 28
#define CONST_HOP_COUNT 0
#define CONST_PROBE_HEADER_C 0
#define CONST_PROBE_HEADER_E 0
#define CONST_PROBE_HEADER_CTRL 0
#define CONST_PROBE_HEADER_FLAG 0
#define CONST_PROBE_HEADER_MTYPE 1
#define CONST_PROBE_HEADER_RSVD 0
#define CONST_IFA_PROBE_HEADER_REP 0
#define CONST_IFA_PROBE_HEADER_VER 1
#define CONST_IFA_PROBE_HEADER_ZERO 0
#define CONST_IFA_SEQUENCE_NUMBER_INIT 0
#define CONST_IFA_TELEMETRY_ACTION 0
#define CONST_IFA_TELEMETRY_REQUEST 0x3b

#define CONST_IFA2_ACTION_VECTOR 0
#define CONST_IFA2_REQUEST_VECTOR 0xff
#define CONST_IFA2_VERSION 2
#define CONST_IFA2_GNS 0xf
#define CONST_IFA2_INITIAL_CURRENT_LENGTH 8

/*
  * }
  */

 /*
  * Internal functions.
  * {
  */

shr_error_e
dnx_ifa_config_info_set_verify(
    int unit,
    uint32 options,
    bcm_ifa_config_info_t * config_data)
{
    SHR_FUNC_INIT_VARS(unit);

    if (options != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported IFA config set");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure dnx for IFA
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] options - Currently options are not supported
 * \param [in] config_data - Information data to set ifa
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * dnx_ifa_config_info_get
 */
shr_error_e
dnx_ifa_config_info_set(
    int unit,
    uint32 options,
    bcm_ifa_config_info_t * config_data)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** create handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_IFA1, &entry_handle_id));

    /** IFA Destination Port */
    /** IFA 2.0 Device ID will include 4 MSB bits of LNS=0x2 */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IFA1_DEVICE_ID, INST_SINGLE, config_data->device_id);

    /** IFA Hop Limit */
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IFA1_HOP_LIMIT, INST_SINGLE, config_data->hop_limit);

    /** IFA Max Length */
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_IFA1_MAX_LENGTH, INST_SINGLE,
                                 config_data->max_payload_length);

    /** IFA Probe Header 1 */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IFA1_PROBE_MARKER_1, INST_SINGLE,
                                 config_data->probemarker_1);

    /** IFA Probe Header 2 */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IFA1_PROBE_MARKER_2, INST_SINGLE,
                                 config_data->probemarker_2);

    /** IFA Sender's Handle */
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_IFA1_SENDER, INST_SINGLE,
                                 config_data->senders_handle);

    /** IFA Template ID */
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IFA1_TID, INST_SINGLE, config_data->template_id);

    /** IFA Probe Header Constant Values */
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_IFA1_CURRENT_LENGTH, INST_SINGLE,
                                 CONST_CURRENT_LENGTH);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IFA1_HOP_COUNT, INST_SINGLE, CONST_HOP_COUNT);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IFA1_PROBE_HEADER_C, INST_SINGLE,
                                 CONST_PROBE_HEADER_C);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IFA1_PROBE_HEADER_E, INST_SINGLE,
                                CONST_PROBE_HEADER_E);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IFA1_PROBE_HEADER_CTRL, INST_SINGLE,
                                CONST_PROBE_HEADER_CTRL);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_IFA1_PROBE_HEADER_FLAG, INST_SINGLE,
                                 CONST_PROBE_HEADER_FLAG);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IFA1_PROBE_HEADER_MTYPE, INST_SINGLE,
                                CONST_PROBE_HEADER_MTYPE);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IFA1_PROBE_HEADER_RSVD, INST_SINGLE,
                                CONST_PROBE_HEADER_RSVD);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IFA1_PROBE_HEADER_REP, INST_SINGLE,
                                CONST_IFA_PROBE_HEADER_REP);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IFA1_PROBE_HEADER_VER, INST_SINGLE,
                                CONST_IFA_PROBE_HEADER_VER);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_IFA1_PROBE_HEADER_ZERO, INST_SINGLE,
                                 CONST_IFA_PROBE_HEADER_ZERO);

    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_IFA1_TELEMETRY_ACTION, INST_SINGLE,
                                 CONST_IFA_TELEMETRY_ACTION);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_IFA1_TELEMETRY_REQUEST, INST_SINGLE,
                                 CONST_IFA_TELEMETRY_REQUEST);

    /** commit */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** create handle for IFA2 header */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PEMLA_IFA2, entry_handle_id));

    /** Constant IFA 2.0 Values */
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IFA2_0_ACTION_VECTOR, INST_SINGLE,
                                CONST_IFA2_ACTION_VECTOR);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IFA2_0_REQUEST_VECTOR, INST_SINGLE,
                                CONST_IFA2_REQUEST_VECTOR);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IFA2_0_VERSION, INST_SINGLE, CONST_IFA2_VERSION);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IFA2_0_GNS, INST_SINGLE, CONST_IFA2_GNS);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IFA2_0_CURRENT_LENGTH, INST_SINGLE,
                                CONST_IFA2_INITIAL_CURRENT_LENGTH);

    /** commit */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - IFA config info get from HW
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] config_data - Information of ifa to be returned
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * bcm_dnx_ifa_config_info_set
 */
shr_error_e
dnx_ifa_config_info_get(
    int unit,
    bcm_ifa_config_info_t * config_data)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** create handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_IFA1, &entry_handle_id));

    /** IFA Destination Port */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IFA1_DEVICE_ID, INST_SINGLE, &config_data->device_id);

    /** IFA Hop Limit */
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_IFA1_HOP_LIMIT, INST_SINGLE, &config_data->hop_limit);

    /** IFA Max Length */
    dbal_value_field16_request(unit, entry_handle_id, DBAL_FIELD_IFA1_MAX_LENGTH, INST_SINGLE,
                               &config_data->max_payload_length);

    /** IFA Probe Header 1 */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IFA1_PROBE_MARKER_1, INST_SINGLE,
                               &config_data->probemarker_1);

    /** IFA Probe Header 2 */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IFA1_PROBE_MARKER_2, INST_SINGLE,
                               &config_data->probemarker_2);

    /** IFA Sender's Handle */
    dbal_value_field16_request(unit, entry_handle_id, DBAL_FIELD_IFA1_SENDER, INST_SINGLE,
                               &config_data->senders_handle);

    /** IFA Template ID */
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_IFA1_TID, INST_SINGLE, &config_data->template_id);

    /** commit */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief Create the IFA header.
 *
 * \param [in] unit - Unit number.
 * \param [in] options - Create options
 * \param [in] header_id - Header ID pointing to IFA header
 * \param [in] header - IFA header
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_dnx_ifa_header_create(
    int unit,
    uint32 options,
    int *header_id,
    bcm_ifa_header_t * header)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    if (header->hop_limit == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "HOP limit should not be 0");
    }
    if (header->max_length < CONST_IFA2_INITIAL_CURRENT_LENGTH)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Max length should be at least Metadata length");
    }

  /** create handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_IFA2, &entry_handle_id));

    /*
     * IFA 2.0 Header Max Length
     */
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IFA2_0_MAX_LENGTH, INST_SINGLE, header->max_length);

    /*
     * IFA 2.0 Header HOP Limit
     */
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IFA2_0_HOP_LIMIT, INST_SINGLE, header->hop_limit);

    /*
     * IFA 2.0 Header Protocol ID
     */
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IFA2_0_PROTOCOL, INST_SINGLE, header->ip_protocol);

  /** commit */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief Get the IFA header.
 *
 * \param [in] unit - Unit number.
 * \param [in] header_id - Header ID pointing to IFA header
 * \param [out] header - IFA header
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_dnx_ifa_header_get(
    int unit,
    int *header_id,
    bcm_ifa_header_t * header)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

  /** create handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_IFA2, &entry_handle_id));

    /*
     * IFA 2.0 Header Max Length
     */
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_IFA2_0_MAX_LENGTH, INST_SINGLE, &header->max_length);

    /*
     * IFA 2.0 Header HOP Limit
     */
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_IFA2_0_HOP_LIMIT, INST_SINGLE, &header->hop_limit);

    /*
     * IFA 2.0 Header Protocol ID
     */
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_IFA2_0_PROTOCOL, INST_SINGLE, &header->ip_protocol);

  /** commit */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Default IFA config info set for dnx
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \see
 */
shr_error_e
dnx_ifa_1_datapath_init(
    int unit)
{
    bcm_ifa_config_info_t config_data;

    SHR_FUNC_INIT_VARS(unit);

    config_data.device_id = DEFAULT_DEVICE_ID;
    config_data.hop_limit = DEFAULT_HOP_LIMIT;
    config_data.max_payload_length = DEFAULT_MAX_IFA_LENGTH;
    config_data.probemarker_1 = DEFAULT_PROBE_MARKER_1;
    config_data.probemarker_2 = DEFAULT_PROBE_MARKER_2;
    config_data.senders_handle = DEFAULT_IFA_SENDER;
    config_data.template_id = DEFAULT_IFA_TEMPLATE_ID;

    SHR_IF_ERR_EXIT(dnx_ifa_config_info_set(unit, 0, &config_data));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - IFA config info set for dnx
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] options - Currently options are not supported
 * \param [in] config_data - Information data to set ifa
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * bcm_dnx_ifa_config_info_get
 */
shr_error_e
bcm_dnx_ifa_config_info_set(
    int unit,
    uint32 options,
    bcm_ifa_config_info_t * config_data)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_IF_ERR_EXIT(dnx_ifa_config_info_set_verify(unit, options, config_data));

    SHR_IF_ERR_EXIT(dnx_ifa_config_info_set(unit, options, config_data));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - IFA config info get for dnx
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] config_data - Information of ifa to be returned
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * bcm_dnx_ifa_config_info_set
 */
shr_error_e
bcm_dnx_ifa_config_info_get(
    int unit,
    bcm_ifa_config_info_t * config_data)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_IF_ERR_EXIT(dnx_ifa_config_info_get(unit, config_data));

exit:
    SHR_FUNC_EXIT;
}

 /*
  * }
  */
