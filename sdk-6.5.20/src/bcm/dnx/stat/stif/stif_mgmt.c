/** \file stif_mgmt.c
 * 
 *  stif module gather all functionality that related to the statistic interface.
 *  stif_mgmt.c gather the managements of stif functionality for DNX.
 *  It is external stif file . (it have interfaces with other modules)
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_STAT

/*
 * Include files.
 * {
 */
#include <shared/utilex/utilex_bitstream.h>
#include <shared/utilex/utilex_framework.h>
#include <shared/shrextend/shrextend_error.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/stat.h>
#include <bcm/cosq.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/stat/stif/stif_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_crps.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stat.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <include/soc/dnx/swstate/auto_generated/access/stif_access.h>
#include <include/bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <include/bcm_int/dnx/algo/port/algo_port_utils.h>
#include <include/bcm_int/dnx/cmn/dnxcmn.h>
#include <include/bcm_int/dnx/stat/stat_mgmt.h>
#include <include/bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <bcm_int/dnx/port/imb/imb.h>

/**
 * \brief
 * This function encodes a enum field. encode enum means
 * translate its SW value to HW value
 */
extern shr_error_e dbal_field_types_encode_enum(
    int unit,
    dbal_field_types_defs_e field_type,
    uint32 sw_field_val,
    uint32 *hw_field_val);

/*
 * }
 */

/*
 * MACROs
 * {
 */

/**
  * \brief - check if given billing source type is External or
  *        Internal
  */
#define DNX_STIF_BILLING_SOURCE_DBAL_TYPE_IS_EXTERNAL(_src_type) \
    ((_src_type == DBAL_ENUM_FVAL_STIF_BILLING_SOURCE_ING_EXT_ENQ || _src_type == DBAL_ENUM_FVAL_STIF_BILLING_SOURCE_ING_EXT_DEQ || _src_type == DBAL_ENUM_FVAL_STIF_BILLING_SOURCE_EGR_EXT) ? 1 : 0)
/**
  * \brief - check if given qsize source type is External or
  *        Internal
  */
#define DNX_STIF_QSIZE_SOURCE_DBAL_TYPE_IS_EXTERNAL(_src_type) \
    ((_src_type == DBAL_ENUM_FVAL_STIF_QSIZE_SOURCE_ING_EXT_ENQ || _src_type == DBAL_ENUM_FVAL_STIF_QSIZE_SOURCE_ING_EXT_DEQ || _src_type == DBAL_ENUM_FVAL_STIF_QSIZE_SOURCE_ING_EXT_SCRUB) ? 1 : 0)
/**
  * \brief - check if given qsize source type is Scrubber
  */
#define DNX_STIF_QSIZE_SOURCE_DBAL_TYPE_IS_SCRUBBER(_src_type) \
    ((_src_type == DBAL_ENUM_FVAL_STIF_QSIZE_SOURCE_ING_EXT_SCRUB || _src_type == DBAL_ENUM_FVAL_STIF_QSIZE_SOURCE_ING_INT_SCRUB) ? 1 : 0)

/*
 * }
 */
 /*************
 * TYPEDEFS  *
 */

/**
 * holds egress billing report format types and their
 * corresponding dbal mask and shift fields
 */
static const dnx_stif_mgmt_report_format_element_t egress_element_enum_to_dbal_fields_binding_table[] = {
    {bcmStatStifRecordElementObj0, DBAL_FIELD_EMPTY, DBAL_FIELD_STIF_ETPP_OBJ_0_DP_SHIFT},
    {bcmStatStifRecordElementObj1, DBAL_FIELD_EMPTY, DBAL_FIELD_STIF_ETPP_OBJ_1_DP_SHIFT},
    {bcmStatStifRecordElementObj2, DBAL_FIELD_EMPTY, DBAL_FIELD_STIF_ETPP_OBJ_2_DP_SHIFT},
    {bcmStatStifRecordElementObj3, DBAL_FIELD_EMPTY, DBAL_FIELD_STIF_ETPP_OBJ_3_DP_SHIFT},
    {bcmStatStifRecordElementPacketSize, DBAL_FIELD_EMPTY, DBAL_FIELD_STIF_ETPP_PACKET_SIZE_SHIFT},
    {bcmStatStifRecordElementOpCode, DBAL_FIELD_EMPTY, DBAL_FIELD_STIF_ETPP_OPCODE_SHIFT}
};
/**
 * holds dbal enum field value and its corresponding bcm enum
 * value for the metadata record format
 */
typedef struct dnx_stif_mgmt_report_metadata_element_s
{
    dbal_enum_value_field_stif_billing_report_metadata_element_e element_type_dbal;
    bcm_stat_stif_record_element_type_t element_type;
} dnx_stif_mgmt_report_metadata_element_t;
/**
 * holds table with dbal enum field values and theirs
 * corresponding bcm enum value for the metadata record format
 */
static const dnx_stif_mgmt_report_metadata_element_t egress_metadata_element_enum_to_dbal_fields_binding_table[] = {
    {DBAL_ENUM_FVAL_STIF_BILLING_REPORT_METADATA_ELEMENT_MC, bcmStatStifRecordElementEgressMetaDataMultiCast},
    {DBAL_ENUM_FVAL_STIF_BILLING_REPORT_METADATA_ELEMENT_PP_DROP_REASON,
     bcmStatStifRecordElementEgressMetaDataPpDropReason},
    {DBAL_ENUM_FVAL_STIF_BILLING_REPORT_METADATA_ELEMENT_CORE, bcmStatStifRecordElementEgressMetaDataCore},
    {DBAL_ENUM_FVAL_STIF_BILLING_REPORT_METADATA_ELEMENT_ECN, bcmStatStifRecordElementEgressMetaDataEcnEligibleAndCni},
    {DBAL_ENUM_FVAL_STIF_BILLING_REPORT_METADATA_ELEMENT_TC, bcmStatStifRecordElementEgressMetaDataTrafficClass},
    {DBAL_ENUM_FVAL_STIF_BILLING_REPORT_METADATA_ELEMENT_DP, bcmStatStifRecordElementEgressMetaDataDropPrecedence},
    {DBAL_ENUM_FVAL_STIF_BILLING_REPORT_METADATA_ELEMENT_OBJ_0, bcmStatStifRecordElementEgressMetaDataObj0},
    {DBAL_ENUM_FVAL_STIF_BILLING_REPORT_METADATA_ELEMENT_OBJ_1, bcmStatStifRecordElementEgressMetaDataObj1},
    {DBAL_ENUM_FVAL_STIF_BILLING_REPORT_METADATA_ELEMENT_OBJ_2, bcmStatStifRecordElementEgressMetaDataObj2},
    {DBAL_ENUM_FVAL_STIF_BILLING_REPORT_METADATA_ELEMENT_OBJ_3, bcmStatStifRecordElementEgressMetaDataObj3}
};

/**
 * \brief
 * defines the default STIF QSIZE IDLE record value high
 */
#define STIF_MGMT_IDLE_RECORD_VALUE_HI (0x3FFFF000)

/**
 * \brief
 * defines the default STIF QSIZE IDLE record value low
 */
#define STIF_MGMT_IDLE_RECORD_VALUE_LO (0x7FFFFFFF)

/**
 * \brief
 * defines the core offset for STIF QSIZE IDLE record
 */
#define STIF_MGMT_IDLE_RECORD_CORE_OFFSET (31)

/**
* \brief
*   init/deinit stif hw
* \param [in] unit - unit id
* \param [in] enable - enable/disable hw
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_stif_mgmt_enable_hw_set(
    int unit,
    uint32 enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Write default values to DBAL_TABLE_STIF_ENABLERS table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_ENABLERS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_INIT, INST_SINGLE, (enable == TRUE) ? 0 : 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_DATA_PATH_EN, INST_SINGLE, enable);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* qsize -  set core id in QSIZE idle report format
* \param [in] unit - unit id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_stif_mgmt_qsize_idle_report_core_id_set(
    int unit)
{
    uint32 entry_handle_id, field_value[3] = { 0 }, core_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_INSTANCE_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_STIF_INSTANCE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        /** Last bit of the record is the core id */
        field_value[0] = STIF_MGMT_IDLE_RECORD_VALUE_LO;
        field_value[1] = STIF_MGMT_IDLE_RECORD_VALUE_HI | (core_id << STIF_MGMT_IDLE_RECORD_CORE_OFFSET);
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_STIF_INSTANCE_OUTPUT_EMPTY_RPRT, INST_SINGLE,
                                         field_value);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   configure all stif instances settings via soc property
*   values
* \param [in] unit - unit id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_stif_mgmt_per_instance_settings_hw_set(
    int unit)
{
    uint32 entry_handle_id, stif_instance_id;
    int report_mode;
    uint32 packet_size, idle_report_period;
    const dnx_data_stif_report_report_size_t *ingress_report_size;
    int idle_record_size_in_bytes;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    report_mode = dnx_data_stif.config.stif_report_mode_get(unit);
    packet_size = dnx_data_stif.config.stif_packet_size_get(unit);
    ingress_report_size =
        dnx_data_stif.report.report_size_get(unit, dnx_data_stif.config.stif_report_size_ingress_get(unit));
    /** In QSIZE mode the idle report should have the default
     *  value of an idle report; for BILLING/INGRESS BILLING the
     *  idle report size should be the ingress report size */
    idle_record_size_in_bytes =
        (report_mode ==
         dnx_stat_stif_mgmt_report_mode_qsize) ? dnx_data_stif.
        report.qsize_idle_report_size_get(unit) : (ingress_report_size->size / UTILEX_NOF_BITS_IN_BYTE);

    /** packet size is calculated from formula - remove size of 4
     *  records */
    packet_size = packet_size - (dnx_data_stif.port.max_nof_instances_get(unit) * idle_record_size_in_bytes);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_INSTANCE_CONFIG, &entry_handle_id));
    for (stif_instance_id = 0; stif_instance_id < dnx_data_stif.port.max_nof_instances_get(unit); stif_instance_id++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_INSTANCE, stif_instance_id);

        /** Defines the minimal period in clocks, between consecutive empty (NULL/IDLE) reports
            generated. An empty report is generated in case no valid reports are available, and only to
            close the report packet. 0 disables the NULL reports generation.*/
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_INSTANCE_OUTPUT_EMPTY_RPRT_PERIOD,
                                     INST_SINGLE, dnx_data_stif.config.stif_idle_report_get(unit));
        /** empty report size in bytes - default in HW is 96b, also
         *  the size of the field is 96b, use the ingress size - it
         *  can be max 96b */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_INSTANCE_OUTPUT_EMPTY_RPRT_SIZE,
                                     INST_SINGLE, idle_record_size_in_bytes);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_INSTANCE_OUTPUT_PACKET_SIZE, INST_SINGLE,
                                     packet_size);
        /** qsize handling needed only if qsize mode is supported for
         *  the device */
        if (dnx_data_stif.config.feature_get(unit, dnx_data_stif_config_only_billing_mode_support) == FALSE)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_INSTANCE_OUTPUT_IS_QSIZE, INST_SINGLE,
                                         (report_mode == dnx_stat_stif_mgmt_report_mode_qsize) ? 1 : 0);
            /** Defines the Idle-report preiod for Qsize mode. When a single ENQ/DEQ report is waiting to
                be sent for IDLE period, an IDLE report will be generated. Period of 0xFFFF, disables the
                IDLE reports generation. - in clocks */
            /** if user selected idle reports to be disabled: */
            if (dnx_data_stif.config.stif_idle_report_get(unit) == 0)
            {
                idle_report_period = dnx_data_stif.report.qsize_idle_report_period_invalid_get(unit);
            }
            else
            {
                idle_report_period = dnx_data_stif.report.qsize_idle_report_period_get(unit);
            }
            dbal_entry_value_field32_set(unit, entry_handle_id,
                                         DBAL_FIELD_STIF_INSTANCE_OUTPUT_INT_QSIZE_RPRT_PERIOD, INST_SINGLE,
                                         idle_report_period);
            if (dnx_data_device.general.nof_cores_get(unit) > 1)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id,
                                             DBAL_FIELD_STIF_INSTANCE_OUTPUT_EXT_QSIZE_RPRT_PERIOD, INST_SINGLE,
                                             idle_report_period);
            }
        }
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    /** in QSIZE idle report has specific format */
    if (report_mode == dnx_stat_stif_mgmt_report_mode_qsize)
    {
        SHR_IF_ERR_EXIT(dnx_stif_mgmt_qsize_idle_report_core_id_set(unit));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   init qsize hw registers
* \param [in] unit - unit id
* \param [in] core_id - logical port core id
* \param [in] qsize_source - the qsize source that is updated
* \param [in] enable - enable/disable the source given
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_stif_mgmt_qsize_source_enable_set(
    int unit,
    int core_id,
    int qsize_source,
    int enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_QSIZE_INIT, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    if (qsize_source == DBAL_RANGE_ALL)
    {
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_STIF_QSIZE_SOURCE, DBAL_RANGE_ALL,
                                         DBAL_RANGE_ALL);
    }
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_QSIZE_SOURCE, qsize_source);
    }
    /** if set fifo enters reset */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_QSIZE_INIT, INST_SINGLE,
                                 (enable == TRUE) ? 0 : 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   init billing hw registers
* \param [in] unit - unit id
* \param [in] core_id - logical port core id
* \param [in] billing_source - the billing source that is
*        updated
* \param [in] enable - enable/disable the source
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_stif_mgmt_billing_source_enable_set(
    int unit,
    int core_id,
    int billing_source,
    int enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_BILLING_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    if (billing_source == DBAL_RANGE_ALL)
    {
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_STIF_BILLING_SOURCE, DBAL_RANGE_ALL,
                                         DBAL_RANGE_ALL);
    }
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_BILLING_SOURCE, billing_source);
    }
    /**if set fifo enters reset */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_BILLING_INIT, INST_SINGLE,
                                 (enable == TRUE) ? 0 : 1);
    if (billing_source == DBAL_ENUM_FVAL_STIF_BILLING_SOURCE_ING_INT_ENQ
        || billing_source == DBAL_ENUM_FVAL_STIF_BILLING_SOURCE_ING_EXT_ENQ)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_BILLING_REPORT_SIZE, INST_SINGLE,
                                     dnx_data_stif.config.stif_report_size_ingress_get(unit));
    }
    else if (billing_source == DBAL_ENUM_FVAL_STIF_BILLING_SOURCE_ING_INT_DEQ
             || billing_source == DBAL_ENUM_FVAL_STIF_BILLING_SOURCE_ING_EXT_DEQ)
    {
        /** for ING_DEQ (internal and external) architecture defined
         *  the report size to have default value of 64 bits */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_BILLING_REPORT_SIZE, INST_SINGLE,
                                     DBAL_ENUM_FVAL_STIF_BILLING_REPORT_SIZE_64_BITS);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_BILLING_REPORT_SIZE, INST_SINGLE,
                                     dnx_data_stif.config.stif_report_size_egress_get(unit));
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   configure scrubber settings via soc property values
* \param [in] unit - unit id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_stif_mgmt_scrubber_hw_set(
    int unit)
{
    uint32 entry_handle_id;
    uint32 stif_scrubber_rate_min, stif_scrubber_rate_max;
    int scrubber_limit;
    uint32 nof_clock_cycles = 0;
    dnxcmn_time_t time;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    time.time_units = DNXCMN_TIME_UNIT_NSEC;
    /** scrubber min rate - in nanoseconds */
    COMPILER_64_SET(time.time, 0, dnx_data_stif.config.stif_scrubber_rate_min_get(unit));
    SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &time, &nof_clock_cycles));
    stif_scrubber_rate_min =
        UTILEX_DIV_ROUND_DOWN(nof_clock_cycles, dnx_data_stif.report.qsize_scrubber_report_cycles_unit_get(unit));
    /** scrubber max rate - in nanoseconds */
    COMPILER_64_SET(time.time, 0, dnx_data_stif.config.stif_scrubber_rate_max_get(unit));
    SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &time, &nof_clock_cycles));
    stif_scrubber_rate_max =
        UTILEX_DIV_ROUND_DOWN(nof_clock_cycles, dnx_data_stif.report.qsize_scrubber_report_cycles_unit_get(unit));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_CGM_SCRUBBER_SETTINGS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    /** scrubber is disabled on init - once scrubber is connected
     *  via API - enable the scrubber */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_CGM_SCRUB_EN, INST_SINGLE, FALSE);
    if (dnx_data_device.general.nof_cores_get(unit) > 1)
    {
        /** once API is called the use external core will be set */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_CGM_SCRUB_USE_EXT_CORE, INST_SINGLE, FALSE);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_CGM_SCRUB_QUEUE_RANGE_LOW, INST_SINGLE,
                                 dnx_data_stif.config.stif_scrubber_queue_min_get(unit));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_CGM_SCRUB_QUEUE_RANGE_HIGH, INST_SINGLE,
                                 dnx_data_stif.config.stif_scrubber_queue_max_get(unit));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_STIF_QSIZE_SCRUBBER_CONFIG, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    /** if max scrubber rate is set to 0 - scrubber is disabled */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_QSIZE_INT_SCRB_MIN_PERIOD, INST_SINGLE,
                                 stif_scrubber_rate_min);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_QSIZE_INT_SCRB_MAX_PERIOD, INST_SINGLE,
                                 stif_scrubber_rate_max);
    if (dnx_data_device.general.nof_cores_get(unit) > 1)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_QSIZE_EXT_SCRB_MIN_PERIOD, INST_SINGLE,
                                     stif_scrubber_rate_min);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_QSIZE_EXT_SCRB_MAX_PERIOD, INST_SINGLE,
                                     stif_scrubber_rate_max);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_STIF_CGM_SCRUBBER_THRESHOLDS, entry_handle_id));
    for (scrubber_limit = 0; scrubber_limit < dnx_data_stif.report.max_thresholds_per_buffer_get(unit);
         scrubber_limit++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCRUBBER_LIMIT, scrubber_limit);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SRAM_BUFFERS_TH, INST_SINGLE,
                                     (dnx_data_stif.config.scrubber_buffers_get(unit, scrubber_limit)->sram_buffers /
                                      dnx_data_stif.config.sram_buffers_resolution_get(unit)));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SRAM_PDBS_TH, INST_SINGLE,
                                     (dnx_data_stif.config.scrubber_buffers_get(unit, scrubber_limit)->sram_pdbs /
                                      dnx_data_stif.config.sram_pdbs_resolution_get(unit)));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_BDBS_TH, INST_SINGLE,
                                     (dnx_data_stif.config.scrubber_buffers_get(unit, scrubber_limit)->dram_bdb /
                                      dnx_data_stif.config.dram_bdb_resolution_get(unit)));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   configure CGM qnum thresholds settings - range of queues for
*   qsize reports
* \param [in] unit - unit id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_stif_mgmt_cgm_qnum_filter_thresholds_hw_set(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_CGM_QSIZE_SETTINGS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

     /** if only billing is supported - no need to init qsize */
    if (dnx_data_stif.config.feature_get(unit, dnx_data_stif_config_only_billing_mode_support) == FALSE)
    {
        /** set default report combined Qsize(SRAM + DRAM) */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QSIZE_REPORT_SEL, INST_SINGLE, 1);
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QNUM_FILTER_LOW_THRESHOLD_0, INST_SINGLE,
                                 dnx_data_stif.config.selective_report_range_get(unit, 0)->min);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QNUM_FILTER_HIGH_THRESHOLD_0, INST_SINGLE,
                                 dnx_data_stif.config.selective_report_range_get(unit, 0)->max);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QNUM_FILTER_LOW_THRESHOLD_1, INST_SINGLE,
                                 dnx_data_stif.config.selective_report_range_get(unit, 1)->min);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QNUM_FILTER_HIGH_THRESHOLD_1, INST_SINGLE,
                                 dnx_data_stif.config.selective_report_range_get(unit, 1)->max);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   configure billing mode settings via soc property values
* \param [in] unit - unit id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_stif_mgmt_billing_mode_hw_set(
    int unit)
{
    uint32 entry_handle_id;
    int report_mode;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    report_mode = dnx_data_stif.config.stif_report_mode_get(unit);

    SHR_IF_ERR_EXIT(dnx_stif_mgmt_cgm_qnum_filter_thresholds_hw_set(unit));

    /** if ingress billing mode- disable etpp; if ing+egr billing
     *  mode - enable etpp */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_ETPP_SETTINGS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_ETPP_ENABLE, INST_SINGLE,
                                 (report_mode == dnx_stat_stif_mgmt_report_mode_billing_ingress) ? FALSE : TRUE);
    /** two MSB bits of the record should be set to 2'b11 */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_ETPP_BUS_IDENTIFIER, INST_SINGLE,
                                 dnx_data_stif.report.record_two_msb_get(unit));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** enable/disable counting ingress MC copies as one */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_STIF_CGM_BILLING_SETTINGS, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_CGM_BILL_MC_ONCE, INST_SINGLE,
                                 dnx_data_stif.config.feature_get(unit,
                                                                  dnx_data_stif_config_stif_report_mc_single_copy));
    /** 2'b10 - incoming TC (TC reported by the PP) should be set
     *  as default and user cannot change it */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_CGM_BILL_TC_SEL, INST_SINGLE,
                                 dnx_data_stif.report.incoming_tc_get(unit));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_CGM_BILL_ENQ_REPORT_SIZE, INST_SINGLE,
                                 dnx_data_stif.config.stif_report_size_ingress_get(unit));
    /** ingress dequeue report size should be 64b */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_CGM_BILL_DEQ_REPORT_SIZE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_STIF_BILLING_REPORT_SIZE_64_BITS);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Header compensation default values */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_STIF_COMPENSATION_MASKS, entry_handle_id));
     /** Default - Header-Truncate is disabled */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_MASK_HEADER_DELTA,
                               DBAL_ENUM_FVAL_STIF_MASK_HEADER_DELTA_TRUNCATE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_COMPENSATION, INST_SINGLE,
                                 DNX_STIF_MGMT_DISABLE_COMPENSATION);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    /** Default - IRPP-Header-Delta is enabled */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_MASK_HEADER_DELTA,
                               DBAL_ENUM_FVAL_STIF_MASK_HEADER_DELTA_IRPP);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_COMPENSATION, INST_SINGLE,
                                 DNX_STIF_MGMT_ENABLE_COMPENSATION);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    /** Default - In-PP-Port-Header-Delta is enabled */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_MASK_HEADER_DELTA,
                               DBAL_ENUM_FVAL_STIF_MASK_HEADER_DELTA_IN_PP_PORT);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_COMPENSATION, INST_SINGLE,
                                 DNX_STIF_MGMT_ENABLE_COMPENSATION);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   init sw state stif tables
* \param [in] unit - unit id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_stif_mgmt_init_sw_state(
    int unit)
{
    uint8 is_init;
    int nof_cores, nof_instances, nof_sources;
    SHR_FUNC_INIT_VARS(unit);

    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    nof_instances = dnx_data_stif.port.max_nof_instances_get(unit);
    nof_sources = dnx_data_stif.report.nof_source_types_get(unit);

    SHR_IF_ERR_EXIT(dnx_stif_db.is_init(unit, &is_init));
    /** if init wasn't made => init */
    if (is_init == FALSE)
    {
        SHR_IF_ERR_EXIT(dnx_stif_db.init(unit));
        /** default values for logical_port; first_port; second_port
         *  are set to -1 in the sw state */
        SHR_IF_ERR_EXIT(dnx_stif_db.instance.alloc(unit, nof_cores, nof_instances));
        /** allocate the sw state for saving the first and second
         *  port, if source mapping to two ports is not supported -
         *  still map the first port and set the second to invalid */
        SHR_IF_ERR_EXIT(dnx_stif_db.source_mapping.alloc(unit, nof_cores, nof_sources));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   when source is connected/disconnected - before
*   connecting/disconnecting the port to the instance -
*   init/deinit sif instance per port
* \param [in] unit - unit id
* \param [in] instance_core - core id of the instance
* \param [in] instance_id - the instance id it is represented
* \param [in] init - init/deinit the sif port instance
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_stif_mgmt_instance_init(
    int unit,
    int instance_core,
    int instance_id,
    uint32 init)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_INSTANCE_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, instance_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_INSTANCE, instance_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_INSTANCE_PORT_ENABLE, INST_SINGLE, init);
    if (dnx_data_stif.flow_control.feature_get(unit, dnx_data_stif_flow_control_instance_0_must_be_on) == FALSE
        && instance_id == 0)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_INSTANCE_ENABLE, INST_SINGLE, 0);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_INSTANCE_ENABLE, INST_SINGLE,
                                     (init == TRUE) ? 0 : 1);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_stif_mgmt_credit_reset(
    int unit,
    int instance_core,
    int instance_id)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_INSTANCE_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, instance_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_INSTANCE, instance_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_INSTANCE_PORT_CREDITS_RESET, INST_SINGLE, TRUE);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** The credit reset must be set to 0 in order to take affect */
    sal_usleep(1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_INSTANCE_PORT_CREDITS_RESET, INST_SINGLE,
                                 FALSE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* egress -  set core id in port metadata variable; will be used
* as report element
* \param [in] unit - unit id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_stif_mgmt_billing_egress_core_id_init(
    int unit)
{
    uint32 entry_handle_id;
    int core_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_ETPP_CORE_ID, &entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE, INST_SINGLE, core_id);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_stif_init(
    int unit)
{
    int instance_id, source_dbal;
    uint32 enable_stif;
    SHR_FUNC_INIT_VARS(unit);

    /** FIRST: Reset all STIF related FIFOs */
     /** enter SIF in reset */

    SHR_IF_ERR_EXIT(dnx_stif_mgmt_enable_hw_set(unit, FALSE));
    /** enter port and instance fifos in reset */
    for (instance_id = 0; instance_id < dnx_data_stif.port.max_nof_instances_get(unit); instance_id++)
    {
        SHR_IF_ERR_EXIT(dnx_stif_mgmt_instance_init(unit, BCM_CORE_ALL, instance_id, FALSE));
    }
    /** if only billing is supported - no need to init qsize */
    if (dnx_data_stif.config.feature_get(unit, dnx_data_stif_config_only_billing_mode_support) == FALSE)
    {
        /** reset fifos for all qsize sources - once source is
             *  connected the fifo state will be changed */
        SHR_IF_ERR_EXIT(dnx_stif_mgmt_qsize_source_enable_set(unit, BCM_CORE_ALL, DBAL_RANGE_ALL, FALSE));
    }

    /** reset fifos for all billing sources - once source is
     *  connected the fifo state will be changed   */
    for (source_dbal = 0; source_dbal < DBAL_NOF_ENUM_STIF_BILLING_SOURCE_VALUES; source_dbal++)
    {
        /** When working with one core there are only Internal
         *  sources, need to skip the External sources */
        if (!DNX_STIF_BILLING_SOURCE_DBAL_TYPE_IS_EXTERNAL(source_dbal)
            || dnx_data_device.general.nof_cores_get(unit) > 1)
        {
            SHR_IF_ERR_EXIT(dnx_stif_mgmt_billing_source_enable_set(unit, BCM_CORE_ALL, source_dbal, FALSE));
        }
    }

    if (dnx_data_device.general.nof_cores_get(unit) > 1) 
    {
        /** init Egress CoreID <-> Port-MetaData */
        SHR_IF_ERR_EXIT(dnx_stif_mgmt_billing_egress_core_id_init(unit));
    }
    /** SECOND: Check if STIF is enabled and init STIF */
    enable_stif = dnx_data_stif.config.feature_get(unit, dnx_data_stif_config_stif_enable);
    if (enable_stif == 1)
    {
        SHR_IF_ERR_EXIT(dnx_stif_mgmt_init_sw_state(unit));

        SHR_IF_ERR_EXIT(dnx_stif_mgmt_per_instance_settings_hw_set(unit));
        /** enable billing mode */
        if (dnx_data_stif.config.stif_report_mode_get(unit) != dnx_stat_stif_mgmt_report_mode_qsize)
        {
            /** init billing related hw regs */
            SHR_IF_ERR_EXIT(dnx_stif_mgmt_billing_mode_hw_set(unit));
            /** init filter drop reason reject masks */
            SHR_IF_ERR_EXIT(dnx_stat_mgmt_reject_filter_masks_init(unit));
        }
        /** enable qsize mode */
        else
        {
            /** init qsize related hw regs */
            SHR_IF_ERR_EXIT(dnx_stif_mgmt_scrubber_hw_set(unit));
            SHR_IF_ERR_EXIT(dnx_stif_mgmt_cgm_qnum_filter_thresholds_hw_set(unit));
        }

        /** enable STIF block */
        SHR_IF_ERR_EXIT(dnx_stif_mgmt_enable_hw_set(unit, enable_stif));
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_stif_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   for the given source return the correct dbal value to be
*   used as key in the STIF_ACTIVE_SOURCE_PER_PORT dbal table
* \param [in] unit - unit id
* \param [in] port_core - the internal core id
* \param [in] source  -  stif source type and core
* \param [in] report_mode - stif report mode
* \param [out] field_source - the dbal value of the source
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_stif_mgmt_source_dbal_value_get(
    int unit,
    bcm_core_t port_core,
    bcm_stat_stif_source_t source,
    int report_mode,
    uint32 *field_source)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (source.src_type)
    {
        case bcmStatStifSourceIngressEnqueue:
            /** internal core is the STIF instance logical port core if
             *  the source core given is different from the internal core
             *  this means that the reports are send from different core
             *  called external core */
            if (report_mode == dnx_stat_stif_mgmt_report_mode_qsize && port_core == source.core)
            {
                *field_source = DBAL_ENUM_FVAL_STIF_QSIZE_SOURCE_ING_INT_ENQ;
            }
            else if (report_mode == dnx_stat_stif_mgmt_report_mode_qsize && port_core != source.core)
            {
                *field_source = DBAL_ENUM_FVAL_STIF_QSIZE_SOURCE_ING_EXT_ENQ;
            }
            else if (report_mode != dnx_stat_stif_mgmt_report_mode_qsize && port_core == source.core)
            {
                *field_source = DBAL_ENUM_FVAL_STIF_BILLING_SOURCE_ING_INT_ENQ;
            }
            else
            {
                *field_source = DBAL_ENUM_FVAL_STIF_BILLING_SOURCE_ING_EXT_ENQ;
            }
            break;
        case bcmStatStifSourceIngressDequeue:
            if (report_mode == dnx_stat_stif_mgmt_report_mode_qsize && port_core == source.core)
            {
                *field_source = DBAL_ENUM_FVAL_STIF_QSIZE_SOURCE_ING_INT_DEQ;
            }
            else if (report_mode == dnx_stat_stif_mgmt_report_mode_qsize && port_core != source.core)
            {
                *field_source = DBAL_ENUM_FVAL_STIF_QSIZE_SOURCE_ING_EXT_DEQ;
            }
            else if (report_mode != dnx_stat_stif_mgmt_report_mode_qsize && port_core == source.core)
            {
                *field_source = DBAL_ENUM_FVAL_STIF_BILLING_SOURCE_ING_INT_DEQ;
            }
            else
            {
                *field_source = DBAL_ENUM_FVAL_STIF_BILLING_SOURCE_ING_EXT_DEQ;
            }
            break;
        case bcmStatStifSourceIngressScrubber:
            *field_source = (port_core == source.core) ? DBAL_ENUM_FVAL_STIF_QSIZE_SOURCE_ING_INT_SCRUB :
                DBAL_ENUM_FVAL_STIF_QSIZE_SOURCE_ING_EXT_SCRUB;
            break;
        case bcmStatStifSourceEgressDequeue:
            *field_source = (port_core == source.core) ? DBAL_ENUM_FVAL_STIF_BILLING_SOURCE_EGR_INT :
                DBAL_ENUM_FVAL_STIF_BILLING_SOURCE_EGR_EXT;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, " invalid source type (=%d)\n", source.src_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   connect the given source - get if the given source is
*   connected or not
* \param [in] unit - unit id
* \param [in] port_core - the internal core id
* \param [in] source  -  stif source type and core
* \param [in] instance_id - instance id
* \param [out] is_connected - is the source connected
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_stif_mgmt_source_connect_get(
    int unit,
    bcm_core_t port_core,
    bcm_stat_stif_source_t source,
    int instance_id,
    uint32 *is_connected)
{
    uint32 source_connected = 0;
    uint32 entry_handle_id, field_source;
    int report_mode;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    report_mode = dnx_data_stif.config.stif_report_mode_get(unit);
    SHR_IF_ERR_EXIT(dnx_stif_mgmt_source_dbal_value_get(unit, port_core, source, report_mode, &field_source));
    if (report_mode == dnx_stat_stif_mgmt_report_mode_qsize)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_ACTIVE_QSIZE_SOURCE_PER_PORT, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_QSIZE_SOURCE, field_source);
    }
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_ACTIVE_BILLING_SOURCE_PER_PORT, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_BILLING_SOURCE, field_source);
    }

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_INSTANCE, instance_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, port_core);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STIF_SOURCE_IS_ACTIVE, INST_SINGLE, &source_connected);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *is_connected = source_connected;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   connect/disconnect the given source
* \param [in] unit - unit id
* \param [in] port_core - the internal core id
* \param [in] source  -  stif source type and core
* \param [in] instance_id - instance id
* \param [in] is_connected - connect/disconnect the source
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_stif_mgmt_source_connect_active_set(
    int unit,
    bcm_core_t port_core,
    bcm_stat_stif_source_t source,
    int instance_id,
    uint32 is_connected)
{
    uint32 entry_handle_id, field_source;
    int report_mode;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    report_mode = dnx_data_stif.config.stif_report_mode_get(unit);
    SHR_IF_ERR_EXIT(dnx_stif_mgmt_source_dbal_value_get(unit, port_core, source, report_mode, &field_source));

    if (report_mode == dnx_stat_stif_mgmt_report_mode_qsize)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_ACTIVE_QSIZE_SOURCE_PER_PORT, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_QSIZE_SOURCE, field_source);
    }
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_ACTIVE_BILLING_SOURCE_PER_PORT, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_BILLING_SOURCE, field_source);
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_INSTANCE, instance_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, port_core);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_SOURCE_IS_ACTIVE, INST_SINGLE, is_connected);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   when source is connected - if this port was not connected to
*   instance id - connect the instance id, set in sw state db
*   and send message to NIF to enable the port
* \param [in] unit - unit id
* \param [in] port - logical port stat
* \param [in] instance_core - core id of the instance
* \param [in] instance_id - the instance id it is represented
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_stif_mgmt_source_instance_id_connect(
    int unit,
    bcm_port_t port,
    int instance_core,
    int instance_id)
{
    bcm_port_t port_get;
    SHR_FUNC_INIT_VARS(unit);

    /** check sw state db - check if already connected - do
     *  nothing, if instance was not yet connected - save in sw
     *  state db and send message to NIF to enable the port */
    SHR_IF_ERR_EXIT(dnx_stif_db.instance.logical_port.get(unit, instance_core, instance_id, &port_get));
    if (port_get != port)
    {
        SHR_IF_ERR_EXIT(dnx_stif_db.instance.logical_port.set(unit, instance_core, instance_id, port));
        /** update STIF-NIF interface with the new connection */
        SHR_IF_ERR_EXIT(imb_port_to_stif_instance_map_set(unit, port, instance_id, instance_core, TRUE));
        SHR_IF_ERR_EXIT(dnx_stif_mgmt_instance_init(unit, instance_core, instance_id, TRUE));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   when source is disconnected - check if there is any left
*   source connected to its instance - if not - remove from sw
*   state db and send message to NIF to disable the port
* \param [in] unit - unit id
* \param [in] port - logical port stat
* \param [in] source - source given
* \param [in] instance_core - core id of the instance
* \param [in] instance_id - the instance id it is represented
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_stif_mgmt_source_instance_id_disconnect(
    int unit,
    bcm_port_t port,
    bcm_stat_stif_source_t source,
    int instance_core,
    int instance_id)
{
    int source_core, source_id, report_mode;
    bcm_stat_stif_source_t source_connected_check;
    uint32 is_connected = 0;
    SHR_FUNC_INIT_VARS(unit);

    report_mode = dnx_data_stif.config.stif_report_mode_get(unit);
    /** go over all sources - check if there is connected source to
    this instance - if there are none disable the port (send message
    to NIF)*/
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, source_core)
    {
        for (source_id = 0; source_id < dnx_data_stif.report.nof_source_types_get(unit); source_id++)
        {
            /** Skip redundant checks of connection for irrelevant source
             *  types */
            if ((report_mode == dnx_stat_stif_mgmt_report_mode_qsize ||
                 report_mode == dnx_stat_stif_mgmt_report_mode_billing_ingress)
                && source_id == bcmStatStifSourceEgressDequeue)
            {
                continue;
            }
            if (report_mode != dnx_stat_stif_mgmt_report_mode_qsize && source_id == bcmStatStifSourceIngressScrubber)
            {
                continue;
            }
            if (report_mode == dnx_stat_stif_mgmt_report_mode_billing && source_id == bcmStatStifSourceIngressDequeue)
            {
                continue;
            }
             /** once disconnecting - check if other sources are connected
             *  to the instance, but skip the disconnected one */
            if (source.core == source_core && source.src_type == source_id)
            {
                continue;
            }
            source_connected_check.core = source_core;
            source_connected_check.src_type = source_id;
            SHR_IF_ERR_EXIT(dnx_stif_mgmt_source_connect_get
                            (unit, instance_core, source_connected_check, instance_id, &is_connected));
            if (is_connected == TRUE)
            {
                break;
            }
        }
        if (is_connected == TRUE)
        {
            break;
        }
    }
    if (is_connected == FALSE)
    {
        /** set in sw state invalid port */
        SHR_IF_ERR_EXIT(dnx_stif_db.instance.logical_port.set(unit, instance_core, instance_id, BCM_PORT_INVALID));
        /** update STIF-NIF interface - disable the SIF port - no sources are connected to it */
        SHR_IF_ERR_EXIT(imb_port_to_stif_instance_map_set(unit, port, instance_id, instance_core, FALSE));
        SHR_IF_ERR_EXIT(dnx_stif_mgmt_instance_init(unit, instance_core, instance_id, FALSE));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   connect the given source - set up hw regs
* \param [in] unit - unit id
* \param [in] port_core - the internal core id
* \param [in] source  -  stif source type and core
* \param [in] instance_id - instance id
* \param [in] report_mode - stif report mode
* \param [in] connect_source - connect/disconnect source
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_stif_mgmt_source_connect_set(
    int unit,
    bcm_core_t port_core,
    bcm_stat_stif_source_t source,
    int instance_id,
    int report_mode,
    int connect_source)
{
    uint32 entry_handle_id, field_source, is_connected;
    int instance_core_idx, instance_idx, scrubber_source_exists = 0;
    bcm_stat_stif_source_t scrubber_source;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_stif_mgmt_source_dbal_value_get(unit, port_core, source, report_mode, &field_source));
    /** Activate/deactivate source */
    SHR_IF_ERR_EXIT(dnx_stif_mgmt_source_connect_active_set(unit, port_core, source, instance_id, connect_source));

    if (report_mode == dnx_stat_stif_mgmt_report_mode_qsize)
    {
        SHR_IF_ERR_EXIT(dnx_stif_mgmt_qsize_source_enable_set(unit, port_core, field_source, connect_source));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_stif_mgmt_billing_source_enable_set(unit, port_core, field_source, connect_source));
    }
    /** For scrubber - set if scrubber is enabled and external core
     *  is used */
    if (source.src_type == bcmStatStifSourceIngressScrubber)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_CGM_SCRUBBER_SETTINGS, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, source.core);
        if (dnx_data_device.general.nof_cores_get(unit) > 1)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_CGM_SCRUB_USE_EXT_CORE, INST_SINGLE,
                                         (port_core != source.core) ? TRUE : FALSE);
        }
        if (connect_source == TRUE)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_CGM_SCRUB_EN, INST_SINGLE, TRUE);
        }
        else
        {
            /** When scrubber source is disconnected - check if there are
             *  any other scrubber sources connected - if not - the
             *  scrubber scan should be disabled */
            scrubber_source.src_type = bcmStatStifSourceIngressScrubber;
            scrubber_source.core = source.core;
            DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, instance_core_idx)
            {
                for (instance_idx = 0; instance_idx < dnx_data_stif.port.max_nof_instances_get(unit); instance_idx++)
                {
                    /** skip current scrubber source */
                    if (instance_idx == instance_id && instance_core_idx == port_core)
                    {
                        continue;
                    }
                    SHR_IF_ERR_EXIT(dnx_stif_mgmt_source_connect_get
                                    (unit, instance_core_idx, scrubber_source, instance_idx, &is_connected));
                    if (is_connected)
                    {
                        scrubber_source_exists = 1;
                        /** no need to update the dbal - there is already connected
                         *  source that has already enabled the scrubber */
                        break;
                    }
                }
            }
            if (scrubber_source_exists == 0)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_CGM_SCRUB_EN, INST_SINGLE, FALSE);
            }
        }
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_stif_mgmt_logical_port_is_connected(
    int unit,
    bcm_port_t port,
    int *instance_core,
    int *instance_id,
    uint8 *is_connected)
{
    uint8 instance_found = FALSE;
    bcm_port_t connected_logical_port;
    int instance, port_core, enable_stif;
    SHR_FUNC_INIT_VARS(unit);

    enable_stif = dnx_data_stif.config.feature_get(unit, dnx_data_stif_config_stif_enable);
    /*
     * Check if stif is enabled
     */
    if (enable_stif)
    {
        /** get the internal core from the given logical port */
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &port_core));
        *instance_core = port_core;
        /** go over the instances - check if the given logical port is
         *  connected to one of it - if it is - return the port, if
         *  it is not - return port invalid */
        for (instance = 0; instance < dnx_data_stif.port.max_nof_instances_get(unit); instance++)
        {
            SHR_IF_ERR_EXIT(dnx_stif_db.instance.logical_port.get(unit, port_core, instance, &connected_logical_port));
            /** the logical port is already connected to an instance */
            if (connected_logical_port == port)
            {
                *instance_id = instance;
                instance_found = TRUE;
                break;
            }
        }
        *is_connected = instance_found;
    }
    else
    {
        *is_connected = 0;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_stif_mgmt_logical_port_to_instance_id_get(
    int unit,
    bcm_port_t port,
    bcm_stat_stif_source_t source,
    int *instance_core,
    int *instance_id)
{
    uint8 instance_found = FALSE;
    bcm_port_t connected_logical_port;
    int instance, port_core;
    int free_instance = -1;
    SHR_FUNC_INIT_VARS(unit);

    /** get the internal core from the given logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &port_core));
    *instance_core = port_core;
    /** go over the instances - check if the given logical port is
     *  connected to one of it - if it is - return the port, if
     *  it is not - return port invalid */
    for (instance = 0; instance < dnx_data_stif.port.max_nof_instances_get(unit); instance++)
    {
        SHR_IF_ERR_EXIT(dnx_stif_db.instance.logical_port.get(unit, port_core, instance, &connected_logical_port));
        /** the logical port is already connected to an instance */
        if (connected_logical_port == port)
        {
            *instance_id = instance;
            instance_found = TRUE;
            break;
        }
        else
        {
            /** if there is an instance that is not connected - save the
             *  first occurance */
            if (connected_logical_port == BCM_PORT_INVALID && free_instance == -1)
            {
                free_instance = instance;
            }
        }
    }
    /** return internal error if no free instance is found and
     *  the port is not connected to any instance, but is a valid
     *  port */
    if (free_instance == -1 && instance_found == FALSE && port != BCM_PORT_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "invalid  port (=%d) - not connected / cannot be connected to stif instance \n", port);
    }
    /** if this port wasnt connected to an instance - connect it
     *  to the fist found free instance to the same port core */
    if (instance_found != TRUE)
    {
        *instance_id = free_instance;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See stif_mgmt.h
 */
shr_error_e
dnx_stif_mgmt_source_mapping_set(
    int unit,
    int flags,
    bcm_stat_stif_source_t source,
    bcm_port_t port)
{
    int report_mode, instance_id = DNX_STIF_MGMT_INSTANCE_INVALID;
    bcm_core_t port_core = BCM_CORE_ALL;
    int connect_source;
    bcm_port_t port_get = BCM_PORT_INVALID;
    SHR_FUNC_INIT_VARS(unit);

    /** make port check and whether instance id
     *  disconnect/connect is needed */
    if (port == BCM_PORT_INVALID)
    {
        connect_source = FALSE;
        /** if the port is invalid - the source should be disabled */
        SHR_IF_ERR_EXIT(dnx_stif_mgmt_source_mapping_get(unit, flags, source, &port_get, &instance_id, &port_core));

        /** if instance invalid, it means that that source is not connect at all, so no need to disconnect it */
        if (instance_id != DNX_STIF_MGMT_INSTANCE_INVALID)
        {
            SHR_IF_ERR_EXIT(dnx_stif_mgmt_source_instance_id_disconnect
                            (unit, port_get, source, port_core, instance_id));
        }
    }
    else
    {
        connect_source = TRUE;
        /** get the instance id from the logical port (or determine new instance) */
        SHR_IF_ERR_EXIT(dnx_stif_mgmt_logical_port_to_instance_id_get(unit, port, source, &port_core, &instance_id));

        /** reset credits for the sif instance */
        SHR_IF_ERR_EXIT(dnx_stif_mgmt_credit_reset(unit, port_core, instance_id));

        /** connect instance id to port in sw state db and in NIF */
        SHR_IF_ERR_EXIT(dnx_stif_mgmt_source_instance_id_connect(unit, port, port_core, instance_id));
    }

    if (instance_id != DNX_STIF_MGMT_INSTANCE_INVALID)
    {
        report_mode = dnx_data_stif.config.stif_report_mode_get(unit);
        /** update all relevant dbal tables when connecting/disconnecting new source */
        SHR_IF_ERR_EXIT(dnx_stif_mgmt_source_connect_set
                        (unit, port_core, source, instance_id, report_mode, connect_source));
    }

    /** save the source to port mapping in sw state */
    if (flags & BCM_STAT_SOURCE_MAPPING_SECOND_PORT)
    {
        SHR_IF_ERR_EXIT(dnx_stif_db.source_mapping.second_port.set(unit, source.core, source.src_type, port));
    }
     /** flags are 0 meaning feature for split sources is not
      *  supported or flag is the first port - set the first port */
    else
    {
        SHR_IF_ERR_EXIT(dnx_stif_db.source_mapping.first_port.set(unit, source.core, source.src_type, port));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   API get the mapping of statistics source to its logical port.
*   port=invalid means that the source is disconnected.
*   in addition it can return the SIF instance id and core that connect to that port.
* \param [in] unit    -  unit ID
* \param [in] flags   -  flags
* \param [in] source  -  stif source type and core
* \param [out] port    -  logical port the source is connected
* \param [out] instance_id    -  the SIF instance id that the port is conencted to
* \param [out] instance_core  -  the SIF instance core the port is connected to
* \return
*   shr_error_e - Error Type
* \remark
*   *
* \see
*   * None
*/
shr_error_e
dnx_stif_mgmt_source_mapping_get(
    int unit,
    int flags,
    bcm_stat_stif_source_t source,
    bcm_port_t * port,
    int *instance_id,
    int *instance_core)
{
    uint32 is_connected = FALSE;
    int instance_idx, instance_core_idx;
    bcm_port_t port_get = BCM_PORT_INVALID;
    SHR_FUNC_INIT_VARS(unit);

     /** if source mapping to two ports is supported and flag is
      *  for second port - return second port - else feature is
      *  not supported / flag is not for second port - return
      *  first port */
    if (flags & BCM_STAT_SOURCE_MAPPING_SECOND_PORT)
    {
        SHR_IF_ERR_EXIT(dnx_stif_db.source_mapping.second_port.get(unit, source.core, source.src_type, port));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_stif_db.source_mapping.first_port.get(unit, source.core, source.src_type, port));
    }
    /** go over the instances - check if the port is connected -
     *  if it is - return instance id and instance core to which
     *  it is connected */
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, instance_core_idx)
    {
        for (instance_idx = 0; instance_idx < dnx_data_stif.port.max_nof_instances_get(unit); instance_idx++)
        {
            SHR_IF_ERR_EXIT(dnx_stif_mgmt_source_connect_get
                            (unit, instance_core_idx, source, instance_idx, &is_connected));
            if (is_connected)
            {
                SHR_IF_ERR_EXIT(dnx_stif_db.instance.
                                logical_port.get(unit, instance_core_idx, instance_idx, &port_get));
                if (port_get == *port)
                {
                    if (instance_id != NULL)
                    {
                        *instance_id = instance_idx;
                    }
                    if (instance_core != NULL)
                    {
                        *instance_core = instance_core_idx;
                    }
                    break;
                }
            }
        }
        if (is_connected)
        {
            break;
        }
    }
    if (is_connected == FALSE)
    {
        *port = BCM_PORT_INVALID;
        if (instance_id != NULL)
        {
            *instance_id = DNX_STIF_MGMT_INSTANCE_INVALID;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   configure flow control for billing mode
* \param [in] unit - unit id
* \param [in] fc_enable - enable/disable FC
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_stif_mgmt_billing_flow_control_set(
    int unit,
    int fc_enable)
{
    uint32 entry_handle_id;
    uint32 flow_control_low, flow_control_high;
    int source_id, is_enq, is_deq;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    for (source_id = 0; source_id < DBAL_NOF_ENUM_STIF_BILLING_SOURCE_VALUES; source_id++)
    {
        /** for devices with one core - support only internal sources */
        if (DNX_STIF_BILLING_SOURCE_DBAL_TYPE_IS_EXTERNAL(source_id)
            && dnx_data_device.general.nof_cores_get(unit) <= 1)
        {
            continue;
        }
        DNX_STIF_MGMT_BILLING_SOURCE_IS_ENQ_GET(unit, source_id, is_enq);
        DNX_STIF_MGMT_BILLING_SOURCE_IS_DEQ_GET(unit, source_id, is_deq);
        if (fc_enable == TRUE)
        {
            /** flow control will be always disabled for ENQ */
            if (is_enq == TRUE)
            {
                flow_control_low = dnx_data_stif.flow_control.fc_disable_value_get(unit);
                flow_control_high = dnx_data_stif.flow_control.fc_disable_value_get(unit);
            }
            else if (is_deq == TRUE)
            {
                flow_control_high = dnx_data_stif.flow_control.billing_cgm_flow_control_high_get(unit);
                flow_control_low = dnx_data_stif.flow_control.billing_cgm_flow_control_low_get(unit);
            }
            /** ETPP DEQ */
            else
            {
                flow_control_high = dnx_data_stif.flow_control.billing_etpp_flow_control_high_get(unit);
                flow_control_low = dnx_data_stif.flow_control.billing_etpp_flow_control_low_get(unit);
            }
        }
        else
        {
            flow_control_low = dnx_data_stif.flow_control.fc_disable_value_get(unit);
            flow_control_high = dnx_data_stif.flow_control.fc_disable_value_get(unit);
        }
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_BILLING_CONFIG, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_BILLING_SOURCE, source_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_BILLING_FC_LOW, INST_SINGLE,
                                     flow_control_low);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_BILLING_FC_HIGH, INST_SINGLE,
                                     flow_control_high);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   configure flow control for qsize mode
* \param [in] unit - unit id
* \param [in] fc_enable - enable/disable FC
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_stif_mgmt_qsize_flow_control_set(
    int unit,
    int fc_enable)
{
    uint32 entry_handle_id;
    uint32 flow_control_low, flow_control_high, hw_field_val;
    int source_id, is_enq, skip_field = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** in STIF_QSIZE_CONFIG table the source id keys does not
     *  include the scrubber source */
    for (source_id = 0; source_id < DBAL_NOF_ENUM_STIF_QSIZE_SOURCE_VALUES; source_id++)
    {
        /** flow control is not set for scrubber, only for enq and deq;
         *  for devices with one core it is supported only for the
         *  internal sources */
        if ((DNX_STIF_QSIZE_SOURCE_DBAL_TYPE_IS_EXTERNAL(source_id) && dnx_data_device.general.nof_cores_get(unit) <= 1)
            || DNX_STIF_QSIZE_SOURCE_DBAL_TYPE_IS_SCRUBBER(source_id))
        {
            continue;
        }
        skip_field =
            dbal_field_types_encode_enum(unit, DBAL_FIELD_TYPE_DEF_STIF_QSIZE_SOURCE, source_id, &hw_field_val);
        if (skip_field != _SHR_E_UNAVAIL)
        {
            DNX_STIF_MGMT_QSIZE_SOURCE_IS_ENQ_GET(unit, source_id, is_enq);
            /** flow control will be always disabled for ENQ */
            if (fc_enable == TRUE)
            {
                if (is_enq == TRUE)
                {
                    flow_control_low = dnx_data_stif.flow_control.fc_disable_value_get(unit);
                    flow_control_high = dnx_data_stif.flow_control.fc_disable_value_get(unit);
                }
                else
                {
                    flow_control_high = dnx_data_stif.flow_control.qsize_cgm_flow_control_high_get(unit);
                    flow_control_low = dnx_data_stif.flow_control.qsize_cgm_flow_control_low_get(unit);
                }
            }
            else
            {
                flow_control_low = dnx_data_stif.flow_control.fc_disable_value_get(unit);
                flow_control_high = dnx_data_stif.flow_control.fc_disable_value_get(unit);
            }
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_QSIZE_CONFIG, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_QSIZE_SOURCE, source_id);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_QSIZE_FC_LOW, INST_SINGLE,
                                         flow_control_low);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_QSIZE_FC_HIGH, INST_SINGLE,
                                         flow_control_high);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_stif_mgmt_flow_control_enable_set(
    int unit,
    int flags,
    int fc_enable)
{
    int report_mode;
    SHR_FUNC_INIT_VARS(unit);

    report_mode = dnx_data_stif.config.stif_report_mode_get(unit);
    if (report_mode != dnx_stat_stif_mgmt_report_mode_qsize)
    {
        SHR_IF_ERR_EXIT(dnx_stif_mgmt_billing_flow_control_set(unit, fc_enable));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_stif_mgmt_qsize_flow_control_set(unit, fc_enable));
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_stif_mgmt_flow_control_enable_get(
    int unit,
    int flags,
    int *fc_enable)
{
    uint32 entry_handle_id;
    int report_mode, source_id;
    uint32 flow_control_low, flow_control_high;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    report_mode = dnx_data_stif.config.stif_report_mode_get(unit);
    /** in order to check if flow control is enabled check for
     *  source DEQ (as for ENQ is always disabled) */
    source_id =
        (report_mode !=
         dnx_stat_stif_mgmt_report_mode_qsize) ? DBAL_ENUM_FVAL_STIF_BILLING_SOURCE_ING_INT_DEQ :
        DBAL_ENUM_FVAL_STIF_QSIZE_SOURCE_ING_INT_DEQ;

    if (report_mode != dnx_stat_stif_mgmt_report_mode_qsize)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_BILLING_CONFIG, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_BILLING_SOURCE, source_id);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STIF_BILLING_FC_LOW, INST_SINGLE,
                                   &flow_control_low);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STIF_BILLING_FC_HIGH, INST_SINGLE,
                                   &flow_control_high);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_QSIZE_CONFIG, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_QSIZE_SOURCE, source_id);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STIF_QSIZE_FC_LOW, INST_SINGLE, &flow_control_low);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STIF_QSIZE_FC_HIGH, INST_SINGLE,
                                   &flow_control_high);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }

    if (flow_control_low == dnx_data_stif.flow_control.fc_disable_value_get(unit)
        && flow_control_high == dnx_data_stif.flow_control.fc_disable_value_get(unit))
    {
        *fc_enable = FALSE;
    }
    else
    {
        *fc_enable = TRUE;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   get current shift place and add the current shift,
*   check for overflow and return error
*   move the current shift place and return it
* \param [in] unit - unit id
* \param [out] current_shift_place - current shift place
* \param [in] shift_value - shift value
* \param [in] overflow_value - overflow value
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_stif_mgmt_current_shift_set(
    int unit,
    int *current_shift_place,
    int shift_value,
    int overflow_value)
{
    SHR_FUNC_INIT_VARS(unit);
    *current_shift_place = *current_shift_place + shift_value;
    if (*current_shift_place > overflow_value)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Record size is filled, cannot add more format types\n");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   algorithm to calculate current and next shift for ingress
*   - get msb 1 and lsb 1 from the mask to calculate what the
*   next shift is and verify what the current shift should be
*   according to the lsb 0s in the mask
* \param [in] unit - unit id
* \param [in] msb_bit - msb bit 1 of the mask
* \param [in] lsb_bit - lsb bit 1 of the mask
* \param [in,out] current_shift_place - current shift place
* \param [out] shift - next shift size
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_stif_mgmt_ingress_report_shift_algorithm(
    int unit,
    uint32 msb_bit,
    uint32 lsb_bit,
    int *current_shift_place,
    int *shift)
{
    int count_lsb_zeroes;
    SHR_FUNC_INIT_VARS(unit);

    /** count_lsb_zeroes holds how many 0 bits are there in the lsb */
    count_lsb_zeroes = lsb_bit;

    /** this means there are 0 bits in the lsb - the field looks
     *  like that for example 00110100 */
    /** two cases - depending on the current shift place we
     *  either shift with calculated value or we dont shift and
     *  have to add more bits to the shift size of the field */
    if ((*current_shift_place - count_lsb_zeroes) <= 0)
    {
        /** example1: current
         *  shift place=2, field_mask = b'111100(00) - only 2 lsb 0s
         *  can be removed by the shift
         */
        /** example2: current shift place=4, field_mask = b'1111(0000) -
         *  field can be exactly set with shift 0
         */
        *current_shift_place = 0;
    }
    else
    {
        /** example: current shift place=6, field_mask = b'11(110000) -
         *  4 bits are 0, 2 valid mask bits should be saved - the
         *  field should be moved with 2 bits */
        *current_shift_place = *current_shift_place - count_lsb_zeroes;
    }

    *shift = msb_bit + 1;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_stif_mgmt_format_elements_array_sort(
    int unit,
    int is_ingress,
    int nof_elements,
    dnx_stif_mgmt_report_format_element_with_shift_t * elements_array)
{
    dnx_stif_mgmt_report_format_element_with_shift_t temp;
    int array_row_i, array_row_j;
    uint32 lsb_set_bit_in_mask_i = 0, lsb_set_bit_in_mask_j = 0;
    SHR_FUNC_INIT_VARS(unit);

    for (array_row_i = 0; array_row_i < nof_elements; array_row_i++)
    {
        for (array_row_j = array_row_i + 1; array_row_j < nof_elements; array_row_j++)
        {
            lsb_set_bit_in_mask_i = (is_ingress) ? utilex_lsb_bit_on(elements_array[array_row_i].element.mask) : 0;
            lsb_set_bit_in_mask_j = (is_ingress) ? utilex_lsb_bit_on(elements_array[array_row_j].element.mask) : 0;

            if ((elements_array[array_row_i].shift + lsb_set_bit_in_mask_i) >
                (elements_array[array_row_j].shift + lsb_set_bit_in_mask_j) ||
                ((elements_array[array_row_i].shift + lsb_set_bit_in_mask_i) ==
                 (elements_array[array_row_j].shift + lsb_set_bit_in_mask_j) &&
                 elements_array[array_row_i].shift > elements_array[array_row_j].shift))
            {
                temp = elements_array[array_row_i];
                elements_array[array_row_i] = elements_array[array_row_j];
                elements_array[array_row_j] = temp;
            }

        }
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_stif_mgmt_ingress_format_elements_shift_values_get(
    int unit,
    int object_count,
    int *count,
    dnx_stif_mgmt_report_format_element_with_shift_t * elements_array_get)
{
    int element, elements_count, object_id;
    uint32 entry_handle_id;
    uint32 mask_get, mask_max, shift_get;
    dbal_fields_e mask_field, shift_field;
            /** save all ingress elements (excluding the objects and
         *  packet size) and use it to read the mask and shift for
         *  all of them */
    bcm_stat_stif_record_element_type_t ingress_elements[DNX_STIF_INGRESS_MAX_RECORD_ELEMENTS + 1]
        = { bcmStatStifRecordElementOpCode, bcmStatStifRecordElementIngressDispositionIsDrop,
        bcmStatStifRecordElementIngressTmDropReason, bcmStatStifRecordElementIngressTrafficClass,
        bcmStatStifRecordElementIngressIncomingDropPrecedence,
        bcmStatStifRecordElementIngressDropPrecedenceMeterResolved,
        bcmStatStifRecordElementIngressDropPrecedenceMeter0Valid,
        bcmStatStifRecordElementIngressDropPrecedenceMeter0Value,
        bcmStatStifRecordElementIngressDropPrecedenceMeter1Valid,
        bcmStatStifRecordElementIngressDropPrecedenceMeter1Value,
        bcmStatStifRecordElementIngressDropPrecedenceMeter2Valid,
        bcmStatStifRecordElementIngressDropPrecedenceMeter2Value,
        bcmStatStifRecordElementIngressCore, bcmStatStifRecordElementIngressPpMetaData,
        bcmStatStifRecordElementIngressIsLastCopy,
        bcmStatStifRecordElementIngressQueueNumber
    };
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** hold the count of all elements including the objects
     *  count */
    elements_count = 0;

    /** go over the other elements and check which one was set */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_CGM_BILLING_STAMP, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    /** go over all possible ingress elements */
    for (element = 0; element < DNX_STIF_INGRESS_MAX_RECORD_ELEMENTS; element++)
    {
        /** ingress core element is supported only for devices with
         *  more than one core */
        if (dnx_data_device.general.nof_cores_get(unit) <= 1
            && ingress_elements[element] == bcmStatStifRecordElementIngressCore)
        {
            continue;
        }

        if ((!dnx_data_stif.report.feature_get(unit, dnx_data_stif_report_stif_record_is_last_copy)) &&
            ingress_elements[element] == bcmStatStifRecordElementIngressIsLastCopy)
        {
            continue;
        }

        mask_get = 0;
        shift_get = 0;
        mask_field =
            ingress_element_enum_to_dbal_fields_binding_table[ingress_elements[element]].dbal_element_mask_field;
        shift_field =
            ingress_element_enum_to_dbal_fields_binding_table[ingress_elements[element]].dbal_element_shift_field;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, mask_field, INST_SINGLE, &mask_get));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, shift_field, INST_SINGLE, &shift_get));

        /** if mask is set this field is used */
        if (mask_get != 0)
        {
            /** get max mask */
            SHR_IF_ERR_EXIT(dbal_fields_predefine_value_get
                            (unit, entry_handle_id, mask_field, FALSE, DBAL_PREDEF_VAL_MAX_VALUE, &mask_max));
            elements_array_get[elements_count].element.element_type = ingress_elements[element];
            elements_array_get[elements_count].element.mask = (mask_get == mask_max) ? BCM_STAT_FULL_MASK : mask_get;
            /** due to Hw issue only if the record size is 96 bits, when setting the shift value we redcude a constant value. */
            /** here need to add again the constant value for getting the right shift value */
            if ((dnx_data_stif.config.stif_report_size_ingress_get(unit) ==
                 DBAL_ENUM_FVAL_STIF_BILLING_REPORT_SIZE_96_BITS))
            {
                elements_array_get[elements_count].shift =
                    shift_get + dnx_data_stif.report.billing_enq_ing_size_shift_get(unit);
            }
            else
            {
                elements_array_get[elements_count].shift = shift_get;
            }

            elements_count++;
        }
    }
    /** include the objects into the array, set shift according
     *  to the max size of the object and set mask for now to be
     *  full mask, the correct one will be later calculated */
    for (object_id = 0; object_id < object_count; object_id++)
    {
        elements_array_get[elements_count].element.element_type = bcmStatStifRecordElementObj0 + object_id;
        elements_array_get[elements_count].element.mask = BCM_STAT_FULL_MASK;
        elements_array_get[elements_count].shift = object_id * dnx_data_stif.report.stat_object_size_get(unit);
        elements_count++;
    }

    *count = elements_count;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_stif_mgmt_ingress_record_format_set(
    int unit,
    int nof_elements,
    bcm_stat_stif_record_format_element_t * record_format_elements)
{
    uint32 entry_handle_id, object_handle, max_mask_value, mask_field_size = 0;
    int element, record_size, shift = 0, current_shift_place = 0, final_shift_place;
    int object_count = 0, nof_cores;
    dbal_fields_e mask_field, shift_field;
    const dnx_data_stif_report_report_size_t *report_size_translate;
    dbal_table_field_info_t field_info;
    uint32 msb_bit, lsb_bit;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    report_size_translate =
        dnx_data_stif.report.report_size_get(unit, dnx_data_stif.config.stif_report_size_ingress_get(unit));
    /** from the record size should be removed the packet size
     *  field and the 2 msb as they are not user configurable */
    record_size =
        report_size_translate->size -
        dnx_data_stif.report.billing_format_element_packet_size_get(unit) -
        dnx_data_stif.report.billing_format_msb_opcode_size_get(unit);
    /** first init all shift and mask config in hw */
    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    for (int core = 0; core < nof_cores; ++core)
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_CGM_BILLING_STAMP, &entry_handle_id));

        /** Setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

        /** clearing the entry */
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_CGM_BILLING_STAMP, &entry_handle_id));

    for (element = 0; element < nof_elements - 1; element++)
    {
        /** PacketSize should be the last element and nothing should
         *  be changed for it */
        if (record_format_elements[element].element_type == bcmStatStifRecordElementPacketSize)
        {
            continue;
        }
        /** if we reached object - verify if there is a hole */
        if (DNX_STIF_MGMT_BILLING_RECORD_ELEMENT_IS_OBJECT(unit, record_format_elements[element].element_type))
        {
            current_shift_place = object_count * dnx_data_stif.report.stat_object_size_get(unit);
            object_count++;

            /** Objects can be masked with full mask - meaning the size
             *  is 20b, or can be set with masks accordingly for 16b or
             *  18b size  */
            if (record_format_elements[element].mask == BCM_STAT_FULL_MASK)
            {
                shift = dnx_data_stif.report.stat_object_size_get(unit);
            }
            else if (record_format_elements[element].mask == DNX_STIF_MGMT_STAT_OBJ_MASK_16_BITS)
            {
                shift = DNX_STIF_MGMT_STAT_OBJ_SHIFT_16_BITS;
            }
            else
            {
                shift = DNX_STIF_MGMT_STAT_OBJ_SHIFT_18_BITS;
            }
            SHR_IF_ERR_EXIT(dnx_stif_mgmt_current_shift_set(unit, &current_shift_place, shift, record_size));
        }
        else
        {

            mask_field =
                ingress_element_enum_to_dbal_fields_binding_table[record_format_elements[element].
                                                                  element_type].dbal_element_mask_field;
            shift_field =
                ingress_element_enum_to_dbal_fields_binding_table[record_format_elements[element].
                                                                  element_type].dbal_element_shift_field;
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
            /** get mask size in bits to calculate the shift */
            SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                            (unit, DBAL_TABLE_STIF_CGM_BILLING_STAMP, mask_field, 0, 0, 0, &field_info));
            mask_field_size = field_info.field_nof_bits;
            /** get the max value of the mask field - mask all bits */
            SHR_IF_ERR_EXIT(dbal_fields_predefine_value_get
                            (unit, entry_handle_id, mask_field, FALSE, DBAL_PREDEF_VAL_MAX_VALUE, &max_mask_value));
            /** get mask - if mask=mask all - set max field value of the
             *  mask if mask is not mask all - check accortingly if mask
             *  is msb or lsb and shift accordingly */
            if (record_format_elements[element].mask != BCM_STAT_FULL_MASK)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, mask_field, INST_SINGLE,
                                             record_format_elements[element].mask);
                /** get msb 1 and lsb 1 from the mask and all bits in between
                 *  will be counted and will be masked */
                msb_bit = utilex_msb_bit_on(record_format_elements[element].mask);
                lsb_bit = utilex_lsb_bit_on(record_format_elements[element].mask);
                /** first check if the mask given fits in the mask field */
                if ((msb_bit + 1) > mask_field_size)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "invalid  mask for element type (=%d), max possible mask for this element=%d\n",
                                 record_format_elements[element].element_type, max_mask_value);
                }
                /** calculate current and next shift according to the mask
                 *  msb and lsb 1s and 0s */
                SHR_IF_ERR_EXIT(dnx_stif_mgmt_ingress_report_shift_algorithm
                                (unit, msb_bit, lsb_bit, &current_shift_place, &shift));
            }
            else
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, mask_field, INST_SINGLE, max_mask_value);
                /** check how many bits are active */
                SHR_BITCOUNT_RANGE(&record_format_elements[element].mask, shift, 0, mask_field_size);
            }

            /** due to Hw issue, which add constant value of zeros before the TM attributes for record size 96 bits, */
            /** need to reduce from the calculated shift the constant value */
            if ((dnx_data_stif.config.stif_report_size_ingress_get(unit) ==
                 DBAL_ENUM_FVAL_STIF_BILLING_REPORT_SIZE_96_BITS))
            {
                if (dnx_data_stif.report.billing_enq_ing_size_shift_get(unit) > current_shift_place)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "wrong ingress billing shift calculation. current_shift_place=%d \n",
                                 current_shift_place);
                }
                else
                {
                    final_shift_place = current_shift_place - dnx_data_stif.report.billing_enq_ing_size_shift_get(unit);
                }
            }
            else
            {
                final_shift_place = current_shift_place;
            }
            dbal_entry_value_field32_set(unit, entry_handle_id, shift_field, INST_SINGLE, final_shift_place);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            SHR_IF_ERR_EXIT(dnx_stif_mgmt_current_shift_set(unit, &current_shift_place, shift, record_size));
        }

    }

    /** set the object count size */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_CGM_BILLING_SETTINGS, &object_handle));
    dbal_entry_key_field32_set(unit, object_handle, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    dbal_entry_value_field32_set(unit, object_handle, DBAL_FIELD_STIF_CGM_BILL_VALID_OBJ_NUM, INST_SINGLE,
                                 object_count);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, object_handle, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_stif_mgmt_ingress_record_format_get(
    int unit,
    int max_nof_elements,
    bcm_stat_stif_record_format_element_t * elements_array,
    int *nof_elements,
    int *elements_size_array)
{
    uint32 entry_handle_id;
    int max_ingress_elements = DNX_STIF_INGRESS_MAX_RECORD_ELEMENTS + DNX_STIF_RECORD_MAX_NOF_OBJECT_ELEMENTS + 1;
    bcm_stat_stif_record_format_element_t elements[DNX_STIF_INGRESS_MAX_RECORD_ELEMENTS +
                                                   DNX_STIF_RECORD_MAX_NOF_OBJECT_ELEMENTS + 1];
    dnx_stif_mgmt_report_format_element_with_shift_t elements_array_get[DNX_STIF_RECORD_MAX_NOF_OBJECT_ELEMENTS +
                                                                        DNX_STIF_INGRESS_MAX_RECORD_ELEMENTS + 1];
    uint32 object_count, msb_bit, lsb_bit, mask_field_size;
    int element_id, record_size, elements_count = 0, elements_array_size = 0, current_shift_place = 0, object_id = 0;
    dbal_fields_e mask_field;
    dbal_table_field_info_t field_info;
    const dnx_data_stif_report_report_size_t *report_size_translate;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    report_size_translate =
        dnx_data_stif.report.report_size_get(unit, dnx_data_stif.config.stif_report_size_ingress_get(unit));
    record_size =
        report_size_translate->size - dnx_data_stif.report.billing_format_element_packet_size_get(unit) -
        dnx_data_stif.report.billing_format_msb_opcode_size_get(unit);
    sal_memset(elements, 0,
               sizeof(bcm_stat_stif_record_format_element_t) * (DNX_STIF_INGRESS_MAX_RECORD_ELEMENTS +
                                                                DNX_STIF_RECORD_MAX_NOF_OBJECT_ELEMENTS + 1));
    sal_memset(elements_array_get, 0,
               sizeof(dnx_stif_mgmt_report_format_element_with_shift_t) * (DNX_STIF_INGRESS_MAX_RECORD_ELEMENTS +
                                                                           DNX_STIF_RECORD_MAX_NOF_OBJECT_ELEMENTS +
                                                                           1));
    /** First - get the Object Elements as they are the first
     *  elements in the record - if there are any */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_CGM_BILLING_SETTINGS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STIF_CGM_BILL_VALID_OBJ_NUM, INST_SINGLE,
                               &object_count);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** get the shift values from the dbal and return the element
     *  count */
    SHR_IF_ERR_EXIT(dnx_stif_mgmt_ingress_format_elements_shift_values_get
                    (unit, object_count, &elements_count, elements_array_get));

    /** sort the array in ascending order according to the shift
     *  - sort only the elements after the Objects - exclude them
     *    from the all elements count */
    SHR_IF_ERR_EXIT(dnx_stif_mgmt_format_elements_array_sort(unit, 1, elements_count, elements_array_get));

    /** move the sorted struct array to the array that will be
     *  given - loop till the max_nof_elements (or the last
     *  element)- if there are more elements than that - they wont
     *  be returned in the array */
    elements_array_size = (elements_count < max_nof_elements) ? elements_count : max_nof_elements;
    /** verify max nof elements is in correct range */
    if (max_nof_elements > max_ingress_elements)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid  max_nof_elements (=%d) - max nof elements can be maximum %d  \n",
                     max_nof_elements, max_ingress_elements);
    }

    /** SIZE calculation of the elements and mask calculation of
     *  the Objects */
    /** Due to the possibility there to be an hole, the last
     *  element before the hole cannot be calculated using only
     *  (the next_shift - current_shift) - save the current size
     *  and the element size is (its shift value - the size of the
     *  elements before it) */
    object_id = 0;
    for (element_id = 0; element_id < elements_array_size; element_id++)
    {
        elements[element_id] = elements_array_get[element_id].element;

        /** if element is bject - calculte the size of the object
         *  according to the mask and shift position of the next
         *  element */
        if (DNX_STIF_MGMT_BILLING_RECORD_ELEMENT_IS_OBJECT(unit, elements[element_id].element_type))
        {
            current_shift_place = object_id * dnx_data_stif.report.stat_object_size_get(unit);
            object_id++;
            elements_size_array[element_id] = dnx_data_stif.report.stat_object_size_get(unit);
            /** if object was the last element - no changes needed */
            if (elements_array_get[element_id + 1].element.element_type != bcmStatStifRecordElementPacketSize
                && !DNX_STIF_MGMT_BILLING_RECORD_ELEMENT_IS_OBJECT(unit,
                                                                   elements_array_get[element_id +
                                                                                      1].element.element_type))
            {
                elements_size_array[element_id] = elements_array_get[element_id + 1].shift - current_shift_place;
                elements[element_id].mask =
                    (elements_size_array[element_id] == DNX_STIF_MGMT_STAT_OBJ_SHIFT_16_BITS) ?
                    DNX_STIF_MGMT_STAT_OBJ_MASK_16_BITS : DNX_STIF_MGMT_STAT_OBJ_MASK_18_BITS;
            }
            SHR_IF_ERR_EXIT(dnx_stif_mgmt_current_shift_set
                            (unit, &current_shift_place, elements_size_array[element_id], record_size));
        }
        else
        {
                /** use the algorithm used in set that calculates the size */
            mask_field =
                ingress_element_enum_to_dbal_fields_binding_table[elements[element_id].
                                                                  element_type].dbal_element_mask_field;
                /** get mask size in bits to calculate the shift */
            SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                            (unit, DBAL_TABLE_STIF_CGM_BILLING_STAMP, mask_field, 0, 0, 0, &field_info));
            mask_field_size = field_info.field_nof_bits;
            if (elements[element_id].mask != BCM_STAT_FULL_MASK)
            {
                msb_bit = utilex_msb_bit_on(elements[element_id].mask);
                lsb_bit = utilex_lsb_bit_on(elements[element_id].mask);
                    /** calculate the size of the element according to the mask
                     *  msb and lsb 1s and 0s */
                SHR_IF_ERR_EXIT(dnx_stif_mgmt_ingress_report_shift_algorithm
                                (unit, msb_bit, lsb_bit, &current_shift_place, &elements_size_array[element_id]));
            }
            else
            {
                SHR_BITCOUNT_RANGE(&elements[element_id].mask, elements_size_array[element_id], 0, mask_field_size);
                SHR_IF_ERR_EXIT(dnx_stif_mgmt_current_shift_set
                                (unit, &current_shift_place, elements_size_array[element_id], record_size));
            }
        }
    }

    if (elements_count < max_nof_elements)
    {
        /** After all elements return the PacketSize
         *  as it is always part of the record even if the user
         *  didn't gave it in the set API */
        elements[elements_count].element_type = bcmStatStifRecordElementPacketSize;
        elements[elements_count].mask = BCM_STAT_FULL_MASK;
        elements_size_array[elements_count] = dnx_data_stif.report.billing_format_element_packet_size_get(unit);
    }
    /** count always PacketSize */
    elements_count++;

    *nof_elements = elements_count;
    sal_memcpy(elements_array, elements, (sizeof(bcm_stat_stif_record_format_element_t) * max_nof_elements));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   init ETPP metadata builder table to values bigger than
*   metadata size(24b)
* \param [in] unit - unit id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_stif_mgmt_etpp_report_format_tables_clear(
    int unit)
{
    uint32 entry_handle_id;
    int element;
    const dnx_data_stif_report_report_size_t *report_size_translate;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    report_size_translate =
        dnx_data_stif.report.report_size_get(unit, dnx_data_stif.config.stif_report_size_egress_get(unit));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_ETPP_METADATA, &entry_handle_id));
    for (element = 0; element < DBAL_NOF_ENUM_STIF_BILLING_REPORT_METADATA_ELEMENT_VALUES; element++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_BILLING_REPORT_METADATA_ELEMENT, element);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SHIFT_VALUE, INST_SINGLE,
                                     dnx_data_stif.report.billing_format_etpp_metadata_size_get(unit));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_STIF_ETPP_REPORT, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_ETPP_OPCODE_SHIFT, INST_SINGLE,
                                 report_size_translate->size);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_ETPP_METADATA_SHIFT, INST_SINGLE,
                                 report_size_translate->size);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_ETPP_PACKET_SIZE_SHIFT, INST_SINGLE,
                                 report_size_translate->size);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_ETPP_OBJ_0_DP_SHIFT, INST_SINGLE,
                                 report_size_translate->size);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_ETPP_OBJ_1_DP_SHIFT, INST_SINGLE,
                                 report_size_translate->size);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_ETPP_OBJ_2_DP_SHIFT, INST_SINGLE,
                                 report_size_translate->size);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_ETPP_OBJ_3_DP_SHIFT, INST_SINGLE,
                                 report_size_translate->size);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   return the dbal value representation for the given ETPP
*   metadata value)
* \param [in] unit - unit id
* \param [in] type - element type
* \param [out] dbal_value - dbal value
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_stif_mgmt_metadata_builder_dbal_key_get(
    int unit,
    bcm_stat_stif_record_element_type_t type,
    dbal_enum_value_field_stif_billing_report_metadata_element_e * dbal_value)
{
    int i, found = 0;
    SHR_FUNC_INIT_VARS(unit);

    for (i = 0; i < DBAL_NOF_ENUM_STIF_BILLING_REPORT_METADATA_ELEMENT_VALUES; i++)
    {
        if (egress_metadata_element_enum_to_dbal_fields_binding_table[i].element_type == type)
        {
            *dbal_value = egress_metadata_element_enum_to_dbal_fields_binding_table[i].element_type_dbal;
            found = 1;
            break;
        }
    }
    if (found == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid  element metadata type (=%d) - not found\n", type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   return the dbal value representation for the given ETPP
*   metadata value)
* \param [in] unit - unit id
* \param [in] opcode_mask - hw value
* \param [out] value - mask value
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_stif_mgmt_report_format_opcode_mask_get(
    int unit,
    int opcode_mask,
    uint32 *value)
{
    SHR_FUNC_INIT_VARS(unit);
    if (opcode_mask == dnx_stat_stif_mgmt_opcode_use_one_type)
    {
        *value = dnx_data_stif.report.billing_opsize_use_one_type_mask_get(unit);
    }
    else if (opcode_mask == dnx_stat_stif_mgmt_opcode_use_two_types)
    {
        *value = dnx_data_stif.report.billing_opsize_use_two_types_mask_get(unit);
    }
    else if (opcode_mask == dnx_stat_stif_mgmt_opcode_use_three_types)
    {
        *value = dnx_data_stif.report.billing_opsize_use_three_types_mask_get(unit);
    }
    else
    {
        *value = dnx_data_stif.report.billing_opsize_use_four_types_mask_get(unit);
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_stif_mgmt_report_format_opcode_hw_mask_value_get(
    int unit,
    uint32 value,
    uint32 *opcode_mask,
    int *shift)
{
    int field_size;
    dbal_table_field_info_t field_info;
    SHR_FUNC_INIT_VARS(unit);
    if (value == dnx_data_stif.report.billing_opsize_use_one_type_mask_get(unit))
    {
        *opcode_mask = dnx_stat_stif_mgmt_opcode_use_one_type;
    }
    else if (value == dnx_data_stif.report.billing_opsize_use_two_types_mask_get(unit))
    {
        *opcode_mask = dnx_stat_stif_mgmt_opcode_use_two_types;
    }
    else if (value == dnx_data_stif.report.billing_opsize_use_three_types_mask_get(unit))
    {
        *opcode_mask = dnx_stat_stif_mgmt_opcode_use_three_types;
    }
    else
    {
        /** mask for 4 types is either 0xFF or BCM_STAT_FULL_MASK */
        *opcode_mask = dnx_stat_stif_mgmt_opcode_use_four_types;
    }
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                    (unit, DBAL_TABLE_STIF_CGM_BILLING_STAMP, DBAL_FIELD_STIF_CGM_BILL_OP_CODE_MASK, 0, 0, 0,
                     &field_info));
    field_size = field_info.field_nof_bits;
    /** check how many bits are active */
    SHR_BITCOUNT_RANGE(&value, *shift, 0, field_size);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_stif_mgmt_egress_record_format_set(
    int unit,
    int nof_elements,
    bcm_stat_stif_record_format_element_t * record_format_elements)
{
    uint32 metadata_handle, object_handle;
    int element, record_size, shift = 0, current_shift_place = 0, current_metadata_shift = 0, packet_size_shift;
    const dnx_data_stif_report_etpp_metadata_t *element_size;
    const dnx_data_stif_report_report_size_t *report_size_translate;
    dbal_fields_e shift_field;
    uint32 opcode_mask;
    uint8 metadata_shift_already_set = 0;
    dbal_enum_value_field_stif_billing_report_metadata_element_e element_type_get = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /** set all shifting fields to invalid shift value beyond the
     *  max possible */
    SHR_IF_ERR_EXIT(dnx_stif_mgmt_etpp_report_format_tables_clear(unit));

    report_size_translate =
        dnx_data_stif.report.report_size_get(unit, dnx_data_stif.config.stif_report_size_egress_get(unit));
    /** calculate the record size - from the record size should be
        removed the packet size field and the 2 msb as they are not
        user configurable */
    record_size =
        report_size_translate->size -
        dnx_data_stif.report.billing_format_element_packet_size_get(unit) -
        dnx_data_stif.report.billing_format_msb_opcode_size_get(unit);
    /** PacketSize should be placed in the end of the record and
     *  if hole exists in the record it will be placed between
     *  the elements and the PacketSize, so the packetsize place
     *  is fixed to the MSB of the record (in the end of the
     *  record there is the PacketSize field and the 2MSB are
     *  opcode indication) */
    packet_size_shift = record_size;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_ETPP_REPORT, &object_handle));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_ETPP_METADATA, &metadata_handle));

    /** keep same format as ingress: obj0(obj1,obj2,obj3),
     *  metadata and opcode, packetsize */
    for (element = 0; element < nof_elements; element++)
    {
        if (record_format_elements[element].element_type == bcmStatStifRecordElementPacketSize)
        {
            /** PacketSize should be shifted */
            dbal_entry_key_field32_set(unit, object_handle, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
            dbal_entry_value_field32_set(unit, object_handle, DBAL_FIELD_STIF_ETPP_PACKET_SIZE_SHIFT, INST_SINGLE,
                                         packet_size_shift);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, object_handle, DBAL_COMMIT));
            break;
        }
        /** for ElementObj(0-3) other handling is needed */
        if (record_format_elements[element].element_type == bcmStatStifRecordElementObj0
            || record_format_elements[element].element_type == bcmStatStifRecordElementObj1
            || record_format_elements[element].element_type == bcmStatStifRecordElementObj2
            || record_format_elements[element].element_type == bcmStatStifRecordElementObj3)
        {
            shift_field =
                egress_element_enum_to_dbal_fields_binding_table[record_format_elements[element].
                                                                 element_type].dbal_element_shift_field;
            dbal_entry_key_field32_set(unit, object_handle, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
            dbal_entry_value_field32_set(unit, object_handle, shift_field, INST_SINGLE, current_shift_place);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, object_handle, DBAL_COMMIT));
            /** Objects can be masked with full mask - meaning the size
             *  is 20b, or can be set with masks accordingly for 16b or
             *  18b size  */
            if (record_format_elements[element].mask == BCM_STAT_FULL_MASK)
            {
                shift = dnx_data_stif.report.stat_object_size_get(unit);
            }
            else if (record_format_elements[element].mask == DNX_STIF_MGMT_STAT_OBJ_MASK_16_BITS)
            {
                shift = DNX_STIF_MGMT_STAT_OBJ_SHIFT_16_BITS;
            }
            else
            {
                shift = DNX_STIF_MGMT_STAT_OBJ_SHIFT_18_BITS;
            }
            /** calculate the new current shift */
            SHR_IF_ERR_EXIT(dnx_stif_mgmt_current_shift_set(unit, &current_shift_place, shift, record_size));
        }
        /** go over Opcode and metadata - in the verify it is
         *  verified that the order is correct and first is Opcode
         *  after that is Metadata */
        else
        {
            if (record_format_elements[element].element_type == bcmStatStifRecordElementOpCode)
            {
                /** After Metadata is shifted - shift Opcode (if exists)*/
                /** current_metadata_shift holds the size of the elements
                 *  constructing the metadata - move the shift with the bits
                 *  from the current metadata */
                if (metadata_shift_already_set)
                {
                    SHR_IF_ERR_EXIT(dnx_stif_mgmt_current_shift_set
                                    (unit, &current_shift_place, current_metadata_shift, record_size));
                }

                shift_field =
                    egress_element_enum_to_dbal_fields_binding_table[record_format_elements[element].
                                                                     element_type].dbal_element_shift_field;
                dbal_entry_key_field32_set(unit, object_handle, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
                dbal_entry_value_field32_set(unit, object_handle, shift_field, INST_SINGLE, current_shift_place);
                /** update the opcode mask */
                SHR_IF_ERR_EXIT(dnx_stif_mgmt_report_format_opcode_hw_mask_value_get
                                (unit, record_format_elements[element].mask, &opcode_mask, &shift));
                dbal_entry_value_field32_set(unit, object_handle, DBAL_FIELD_STIF_ETPP_OPCODE_SIZE, INST_SINGLE,
                                             opcode_mask);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, object_handle, DBAL_COMMIT));
                SHR_IF_ERR_EXIT(dnx_stif_mgmt_current_shift_set(unit, &current_shift_place, shift, record_size));
            }
            /** handle metadata elements shifts */
            /** for the ones that are not set - set the shift to have
             *  value 24 */
            else
            {
                /** Metadata should be shifted (if
                 *  exists)*/
                if (!metadata_shift_already_set)
                {
                    dbal_entry_key_field32_set(unit, object_handle, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
                    dbal_entry_value_field32_set(unit, object_handle, DBAL_FIELD_STIF_ETPP_METADATA_SHIFT, INST_SINGLE,
                                                 current_shift_place);
                    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, object_handle, DBAL_COMMIT));
                    metadata_shift_already_set = 1;
                }
                /** current_metadata_shift - hold the shift regarding the
                 *  elements constructing the metadata */
                /** convert the element type to the dbal enum used as key for
                 *  the metadata builder table */
                SHR_IF_ERR_EXIT(dnx_stif_mgmt_metadata_builder_dbal_key_get
                                (unit, record_format_elements[element].element_type, &element_type_get));
                dbal_entry_key_field32_set(unit, metadata_handle, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
                dbal_entry_key_field32_set(unit, metadata_handle, DBAL_FIELD_STIF_BILLING_REPORT_METADATA_ELEMENT,
                                           element_type_get);
                dbal_entry_value_field32_set(unit, metadata_handle, DBAL_FIELD_SHIFT_VALUE, INST_SINGLE,
                                             current_metadata_shift);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, metadata_handle, DBAL_COMMIT));
                /** get max size of the element */
                element_size =
                    dnx_data_stif.report.etpp_metadata_get(unit, record_format_elements[element].element_type);
                /** metadata elements are masked as whole field */
                SHR_IF_ERR_EXIT(dnx_stif_mgmt_current_shift_set
                                (unit, &current_metadata_shift, element_size->size,
                                 dnx_data_stif.report.billing_format_etpp_metadata_size_get(unit)));
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_stif_mgmt_egress_record_format_get(
    int unit,
    int max_nof_elements,
    bcm_stat_stif_record_format_element_t * elements_array,
    int *nof_elements,
    int *elements_size_array)
{
    uint32 object_handle, metadata_handle;
    int max_egress_elements =
        DNX_STIF_RECORD_MAX_NOF_OBJECT_ELEMENTS + DNX_STIF_EGRESS_MAX_RECORD_METADATA_ELEMENTS + 3;
    bcm_stat_stif_record_format_element_t elements[DNX_STIF_RECORD_MAX_NOF_OBJECT_ELEMENTS +
                                                   DNX_STIF_EGRESS_MAX_RECORD_METADATA_ELEMENTS + 3];
    int metadata_element, elements_count = 0, object_count = 0, elements_array_size = 0;
    dbal_fields_e shift_field;
    int element_id, object_id, field_size;
    uint32 report_size, metadata_element_shift, opcode_shift, opcode_size, object_shift,
        object_shift_arr[DNX_STIF_RECORD_MAX_NOF_OBJECT_ELEMENTS + 1], metadata_shift;
    dnx_stif_mgmt_report_format_element_with_shift_t elements_array_get[DNX_STIF_EGRESS_MAX_RECORD_METADATA_ELEMENTS];
    const dnx_data_stif_report_report_size_t *report_size_translate;
    const dnx_data_stif_report_etpp_metadata_t *metadata_element_size;
    dbal_table_field_info_t field_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(elements, 0,
               sizeof(bcm_stat_stif_record_format_element_t) * (DNX_STIF_RECORD_MAX_NOF_OBJECT_ELEMENTS +
                                                                DNX_STIF_EGRESS_MAX_RECORD_METADATA_ELEMENTS + 3));
    sal_memset(elements_array_get, 0,
               sizeof(dnx_stif_mgmt_report_format_element_with_shift_t) * DNX_STIF_EGRESS_MAX_RECORD_METADATA_ELEMENTS);
    report_size_translate =
        dnx_data_stif.report.report_size_get(unit, dnx_data_stif.config.stif_report_size_egress_get(unit));
    report_size =
        report_size_translate->size - dnx_data_stif.report.billing_format_element_packet_size_get(unit) -
        dnx_data_stif.report.billing_format_msb_opcode_size_get(unit);

    /** Order of elements: Obj0,..,Metadata, Opcode, Packetsize */
    /** First get if there is MetaData - in order to use its
     *  shift to know the objects shifts :
     *  For example :
     *  MetaData(24b) Obj3 (18b) Obj2(16b) Obj1(20b) Obj0 (16b)
     *
     *  MetaData shift will give the size of all objects selected
     *  Obj0 if exist has shift 0, Obj1 shift is the size of Obj0 */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_ETPP_REPORT, &object_handle));
    dbal_entry_key_field32_set(unit, object_handle, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    dbal_value_field32_request(unit, object_handle, DBAL_FIELD_STIF_ETPP_METADATA_SHIFT, INST_SINGLE, &metadata_shift);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, object_handle, DBAL_COMMIT));

    /** go over the objects and get their shifts*/
    for (object_id = 0; object_id < DNX_STIF_RECORD_MAX_NOF_OBJECT_ELEMENTS; object_id++)
    {
        shift_field = egress_element_enum_to_dbal_fields_binding_table[object_id].dbal_element_shift_field;
        dbal_entry_key_field32_set(unit, object_handle, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
        dbal_value_field32_request(unit, object_handle, shift_field, INST_SINGLE, &object_shift);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, object_handle, DBAL_COMMIT));
        if (object_shift != report_size_translate->size)
        {
            /** Object is set - save in the array, will calculate the
             *  mask and size outside the loop once the object count is
             *  known */
            elements[object_count].element_type = object_id;
            object_shift_arr[object_count] = object_shift;
            object_count++;
        }
    }
     /** Check if there is Opcode shift set
     *  In case no MetaData is set and Opcode is set - need to
     *  use the Opcode shift to calculate the size of the Objects
     *
     *  In case no MetaData and no Opcode - use the size of the
     *  report excluding the PacketSize and the msb Header Type */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_STIF_ETPP_REPORT, object_handle));
    dbal_entry_key_field32_set(unit, object_handle, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    dbal_value_field32_request(unit, object_handle, DBAL_FIELD_STIF_ETPP_OPCODE_SHIFT, INST_SINGLE, &opcode_shift);
    dbal_value_field32_request(unit, object_handle, DBAL_FIELD_STIF_ETPP_OPCODE_SIZE, INST_SINGLE, &opcode_size);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, object_handle, DBAL_COMMIT));

    /** include MetaData shift/Opcode shift/Report size - PacketSize
     *  shift as final element in the object array */
    if (metadata_shift != report_size_translate->size)
    {
        object_shift_arr[object_count] = metadata_shift; /** MetaData exists*/
    }
    else
    {
        object_shift_arr[object_count] = (opcode_shift != report_size_translate->size) ? opcode_shift : report_size;
    }

    /** Calculate the the object size and mask accordingly:
     *
     *  Start from the element after the objects, for example it
     *  is Metadata, and 4 objects:
     *
     *  size[object3] = shift[MetaData(4)] - shift[ object3]
     *  size[object2] = shift[object3] - shift[object2] ...
     *  size[object0] = shift[object1] - shift[object0] */
    for (object_id = 0; object_id < object_count; object_id++)
    {
        elements_size_array[object_id] = object_shift_arr[object_id + 1] - object_shift_arr[object_id];
        /** set the mask according to the size of the element */
        if (elements_size_array[object_id] == dnx_data_stif.report.stat_object_size_get(unit))
        {
            elements[object_id].mask = BCM_STAT_FULL_MASK;
        }
        else if (elements_size_array[object_id] == DNX_STIF_MGMT_STAT_OBJ_SHIFT_16_BITS)
        {
            elements[object_id].mask = DNX_STIF_MGMT_STAT_OBJ_MASK_16_BITS;
        }
        else
        {
            elements[object_id].mask = DNX_STIF_MGMT_STAT_OBJ_MASK_18_BITS;
        }
    }

    elements_count = 0;
    /** Go over the metadata builder DBAL and check which fields
     *  were set, also save the shift */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_ETPP_METADATA, &metadata_handle));
    for (metadata_element = 0; metadata_element < DBAL_NOF_ENUM_STIF_BILLING_REPORT_METADATA_ELEMENT_VALUES;
         metadata_element++)
    {
        dbal_entry_key_field32_set(unit, metadata_handle, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
        dbal_entry_key_field32_set(unit, metadata_handle, DBAL_FIELD_STIF_BILLING_REPORT_METADATA_ELEMENT,
                                   metadata_element);
        dbal_value_field32_request(unit, metadata_handle, DBAL_FIELD_SHIFT_VALUE, INST_SINGLE, &metadata_element_shift);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, metadata_handle, DBAL_COMMIT));
        if (metadata_element_shift != (dnx_data_stif.report.billing_format_etpp_metadata_size_get(unit)))
        {
            elements_array_get[elements_count].element.element_type =
                egress_metadata_element_enum_to_dbal_fields_binding_table[metadata_element].element_type;
            elements_array_get[elements_count].element.mask = BCM_STAT_FULL_MASK;
            elements_array_get[elements_count].shift = metadata_element_shift;
            elements_count++;
        }
    }
    /** Sort metadata element fields according to the shift in
     *  ascending order */
    SHR_IF_ERR_EXIT(dnx_stif_mgmt_format_elements_array_sort(unit, 0, elements_count, elements_array_get));
    elements_count = elements_count + object_count;

    /** move the sorted struct array to the array that will be
     *  given - loop till the max_nof_elements (or the last
     *  element)- if there are more elements than that - they wont
     *  be returned in the array */
    elements_array_size = (elements_count < max_nof_elements) ? elements_count : max_nof_elements;
    /** verify max nof elements is in correct range */
    if (max_nof_elements > max_egress_elements)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid  max_nof_elements (=%d) - max nof elements can be maximum %d  \n",
                     max_nof_elements, max_egress_elements);
    }

    for (element_id = object_count; element_id < elements_array_size; element_id++)
    {
        elements[element_id] = elements_array_get[element_id - object_count].element;
        metadata_element_size =
            dnx_data_stif.report.etpp_metadata_get(unit,
                                                   elements_array_get[element_id - object_count].element.element_type);
        elements_size_array[element_id] = metadata_element_size->size;
    }

    /** In order to return correct count of elements should check
     *  if there is Opcode and also count the PacketSize. They
     *  will be returned only if the user asked for them */
    /** Return Opcode and Packetsize */
    /** Check if Opcode exists */
    if (opcode_shift != report_size_translate->size)
    {
        if (elements_count < max_nof_elements)
        {
            elements[elements_count].element_type = bcmStatStifRecordElementOpCode;
            /** get the mask of the OpCode element */
            SHR_IF_ERR_EXIT(dnx_stif_mgmt_report_format_opcode_mask_get
                            (unit, opcode_size, &elements[elements_count].mask));
            /** Get the maximum possible size of the Opcode field - use
             *  the CGM field from the ingress as in the egress it is
             *  stored differently */
            SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                            (unit, DBAL_TABLE_STIF_CGM_BILLING_STAMP, DBAL_FIELD_STIF_CGM_BILL_OP_CODE_MASK, 0, 0, 0,
                             &field_info));
            field_size = field_info.field_nof_bits;
            /** count the on bits  from the Opcode mask; note that there
             *  are no holes in the Opcode */
            SHR_BITCOUNT_RANGE(&elements[elements_count].mask, elements_size_array[elements_count], 0, field_size);
        }
        elements_count++;
    }
    if (elements_count < max_nof_elements)
    {
       /** After all elements return the PacketSize
        *  as it is always part of the record even if the user
        *  didn't gave it in the set API */
        elements[elements_count].element_type = bcmStatStifRecordElementPacketSize;
        elements[elements_count].mask = BCM_STAT_FULL_MASK;
        elements_size_array[elements_count] = dnx_data_stif.report.billing_format_element_packet_size_get(unit);
    }
    /** count always PacketSize */
    elements_count++;

    *nof_elements = elements_count;
    sal_memcpy(elements_array, elements, (sizeof(bcm_stat_stif_record_format_element_t) * max_nof_elements));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_stif_mgmt_control_range_verify(
    int unit,
    int verify_set,
    bcm_gport_t port,
    bcm_cosq_control_range_type_t type,
    bcm_cosq_range_t * range)
{
    uint32 range_min, range_max;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify STIF is enabled */
    if (dnx_data_stif.config.feature_get(unit, dnx_data_stif_config_stif_enable) == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_DISABLED,
                     "Statistic Interface is not enabled, use soc property stat_if_enable to enable it.\n");
    }
    if (dnx_data_stif.config.stif_report_mode_get(unit) != dnx_stat_stif_mgmt_report_mode_qsize
        && type == bcmCosqStatIfScrubberQueues)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Statistic Interface is not configured in mode QSIZE,bcm_cosq_control_range_set with type=(%d)  is for qsize mode only. \n",
                     type);
    }
    /** Verify port */
    if (!BCM_COSQ_GPORT_IS_CORE(port))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, given gport %d is invalid\n", unit, port);
    }
    if (verify_set == STIF_MGMT_VERIFY_SET)
    {
        /** Verify range */
        range_min = 0;
        range_max = dnx_data_ipq.queues.nof_queues_get(unit);
        if ((range->range_start > range->range_end) ||
            (range->range_start < range_min) || (range->range_end >= range_max))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, " invalid source range start (=%d) and end (=%d) \n", range->range_start,
                         range->range_end);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_stif_mgmt_queues_range_dbal_set(
    int unit,
    bcm_core_t core,
    bcm_cosq_control_range_type_t type,
    int range_start,
    int range_end)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (type == bcmCosqStatIfQueues)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_CGM_QSIZE_SETTINGS, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QNUM_FILTER_LOW_THRESHOLD_0, INST_SINGLE,
                                     range_start);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QNUM_FILTER_HIGH_THRESHOLD_0, INST_SINGLE,
                                     range_end);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QNUM_FILTER_LOW_THRESHOLD_1, INST_SINGLE,
                                     range_start);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QNUM_FILTER_HIGH_THRESHOLD_1, INST_SINGLE,
                                     range_end);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else if (type == bcmCosqStatIfScrubberQueues)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_CGM_SCRUBBER_SETTINGS, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_CGM_SCRUB_QUEUE_RANGE_LOW, INST_SINGLE,
                                     range_start);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_CGM_SCRUB_QUEUE_RANGE_HIGH, INST_SINGLE,
                                     range_end);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "invalid  type (=%d) -  should be bcmCosqStatIfQueues or bcmCosqStatIfScrubberQueues \n", type);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_stif_mgmt_queues_range_dbal_get(
    int unit,
    bcm_core_t core,
    bcm_cosq_control_range_type_t type,
    int *range_start,
    int *range_end)
{
    uint32 entry_handle_id;
    uint32 start, end;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (type == bcmCosqStatIfQueues)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_CGM_QSIZE_SETTINGS, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_QNUM_FILTER_LOW_THRESHOLD_0, INST_SINGLE, &start);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_QNUM_FILTER_HIGH_THRESHOLD_0, INST_SINGLE, &end);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }
    else if (type == bcmCosqStatIfScrubberQueues)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_CGM_SCRUBBER_SETTINGS, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STIF_CGM_SCRUB_QUEUE_RANGE_LOW, INST_SINGLE,
                                   &start);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STIF_CGM_SCRUB_QUEUE_RANGE_HIGH, INST_SINGLE,
                                   &end);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "invalid  type (=%d) -  should be bcmCosqStatIfQueues or bcmCosqStatIfScrubberQueues \n", type);
    }

    *range_start = start;
    *range_end = end;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_stif_mgmt_pkt_size_adjust_select_verify(
    int unit,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_pkt_size_adjust_select_type_t select_type)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key, _SHR_E_PARAM, "key");
    /** verify STIF is enabled */
    if (dnx_data_stif.config.feature_get(unit, dnx_data_stif_config_stif_enable) == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_DISABLED,
                     "Statistic Interface is not enabled, use soc property stat_if_enable to enable it.\n");
    }
    /** verify STIF is in billing mode */
    if (dnx_data_stif.config.stif_report_mode_get(unit) == dnx_stat_stif_mgmt_report_mode_qsize)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "invalid  stif report mode - API is supported for report mode BILLING or BILLING INGRESS \n");
    }
    /** verify the core are valid - other parameters are not
     *  valid for STIF */
    DNXCMN_CORE_VALIDATE(unit, key->core_id, TRUE);
   /** verify HeaderTruncate is chosen */
    if (select_type != bcmStatPktSizeSelectCounterIngressHeaderTruncate)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     " Incorrect select_type=%d - supported is only bcmStatPktSizeSelectCounterIngressHeaderTruncate \n",
                     select_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_stif_mgmt_pkt_size_adjust_select_set_verify(
    int unit,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_pkt_size_adjust_select_type_t select_type,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key, _SHR_E_PARAM, "key");
    /** basic verification - stif enabled, billing report */
    SHR_IF_ERR_EXIT(dnx_stif_mgmt_pkt_size_adjust_select_verify(unit, key, select_type));
    /** verify enable 0/1 */
    if (enable != TRUE && enable != FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect enable=%d - must be true/false \n", enable);
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_stif_mgmt_pkt_size_adjust_select_set(
    int unit,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_pkt_size_adjust_select_type_t select_type,
    int enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_COMPENSATION_MASKS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, key->core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_MASK_HEADER_DELTA, select_type);
    /** according to user */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_COMPENSATION, INST_SINGLE, enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_stif_mgmt_pkt_size_adjust_select_get(
    int unit,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_pkt_size_adjust_select_type_t select_type,
    int *enable)
{
    uint32 entry_handle_id, enable_get;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_COMPENSATION_MASKS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, key->core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_MASK_HEADER_DELTA, select_type);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE_COMPENSATION, INST_SINGLE, &enable_get);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    *enable = enable_get;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_stif_mgmt_filter_group_verify(
    int unit,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_counter_group_filter_t filter)
{
    const dnx_data_stat_drop_reasons_group_drop_reason_index_t *group_index;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key, _SHR_E_PARAM, "key");

    /** verify STIF is enabled */
    if (dnx_data_stif.config.feature_get(unit, dnx_data_stif_config_stif_enable) == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_DISABLED,
                     "Statistic Interface is not enabled, use soc property stat_if_enable to enable it.\n");
    }
    /** verify STIF is in billing mode */
    if (dnx_data_stif.config.stif_report_mode_get(unit) == dnx_stat_stif_mgmt_report_mode_qsize)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "invalid  report mode - API is supported only for report mode BILLING and BILLING INGRESS \n");
    }
    /** verify the core is valid - other parameters are not
     *  valid for STIF */
    DNXCMN_CORE_VALIDATE(unit, key->core_id, TRUE);

    /** verify filter group */
    group_index = dnx_data_stat.drop_reasons.group_drop_reason_index_get(unit, filter);
    if (filter < 0 || filter >= bcmStatCounterGroupFilterCount
        || (group_index->index == -1 && filter == bcmStatCounterGroupFilterLatencyDrop))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect filter group=%d\n", filter);
    }
exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_stif_mgmt_filter_group_set_verify(
    int unit,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_counter_group_filter_t filter,
    int is_active)
{

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key, _SHR_E_PARAM, "key");
    /** verify is_active */
    if (is_active != TRUE && is_active != FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect is_active=%d\n - TRUE/FALSE", is_active);
    }
    /** verify key struct and filter */
    SHR_IF_ERR_EXIT(dnx_stif_mgmt_filter_group_verify(unit, key, filter));

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_stif_mgmt_filter_group_set(
    int unit,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_counter_group_filter_t filter,
    int is_active)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** get profile_id of the filter group */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_CGM_FILTER_MASK, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FILTER_GROUP_ID, filter);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, key->core_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FILTER_MASK_ENABLE, INST_SINGLE, is_active);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_stif_mgmt_filter_group_get(
    int unit,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_counter_group_filter_t filter,
    int *is_active)
{
    uint32 active;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** get profile_id of the filter group */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STIF_CGM_FILTER_MASK, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FILTER_GROUP_ID, filter);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, key->core_id);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FILTER_MASK_ENABLE, INST_SINGLE, &active);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *is_active = active;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
