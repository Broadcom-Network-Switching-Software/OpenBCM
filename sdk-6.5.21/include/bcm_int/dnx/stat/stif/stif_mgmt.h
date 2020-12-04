/** \file bcm_int/dnx/stat/stif/stif_mgmt.h
 *
 * Internal DNX STAT STIF MGMT
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNX_STIF_MGMT_INCLUDED__
/*
 * {
 */
#define _DNX_STIF_MGMT_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/cosq.h>
#include <soc/dnx/dbal/dbal.h>
/*
 * MACROs
 * {
 */

/**
 * \brief
 * defines the instance invalid value
 */
#define DNX_STIF_MGMT_INSTANCE_INVALID (-1)

/**
  * \brief - return if the given element type is Object
  */
#define DNX_STIF_MGMT_BILLING_RECORD_ELEMENT_IS_OBJECT(_unit, _element_type) \
    (_element_type == bcmStatStifRecordElementObj0 || _element_type == bcmStatStifRecordElementObj1 || _element_type == bcmStatStifRecordElementObj2 || _element_type == bcmStatStifRecordElementObj3)

/**
  * \brief - return if the given billing source is ENQUEUE source
  */
#define DNX_STIF_MGMT_BILLING_SOURCE_IS_ENQ_GET(_unit, _source_id, _is_enq) \
{\
        _is_enq = (_source_id == DBAL_ENUM_FVAL_STIF_BILLING_SOURCE_ING_INT_ENQ || _source_id == DBAL_ENUM_FVAL_STIF_BILLING_SOURCE_ING_EXT_ENQ) ? TRUE : FALSE; \
}

/**
  * \brief - return if the given billing source is ENQUEUE source
  */
#define DNX_STIF_MGMT_BILLING_SOURCE_IS_DEQ_GET(_unit, _source_id, _is_deq) \
{\
        _is_deq = (_source_id == DBAL_ENUM_FVAL_STIF_BILLING_SOURCE_ING_INT_DEQ || _source_id == DBAL_ENUM_FVAL_STIF_BILLING_SOURCE_ING_EXT_DEQ) ? TRUE : FALSE; \
}

/**
  * \brief - return if the given qsize source is ENQUEUE source
  */
#define DNX_STIF_MGMT_QSIZE_SOURCE_IS_ENQ_GET(_unit, _source_id, _is_enq) \
{\
        _is_enq = (_source_id == DBAL_ENUM_FVAL_STIF_QSIZE_SOURCE_ING_INT_ENQ || _source_id == DBAL_ENUM_FVAL_STIF_QSIZE_SOURCE_ING_EXT_ENQ) ? TRUE : FALSE; \
}

/**
  * \brief - return if the given qsize source is ENQUEUE source
  */
#define DNX_STIF_MGMT_QSIZE_SOURCE_IS_DEQ_GET(_unit, _source_id, _is_deq) \
{\
        _is_deq = (_source_id == DBAL_ENUM_FVAL_STIF_QSIZE_SOURCE_ING_INT_DEQ || _source_id == DBAL_ENUM_FVAL_STIF_QSIZE_SOURCE_ING_EXT_DEQ) ? TRUE : FALSE; \
}
/*
 * }
 */

typedef enum
{
    dnx_stat_stif_mgmt_report_mode_qsize = 0,
    dnx_stat_stif_mgmt_report_mode_billing = 1,
    dnx_stat_stif_mgmt_report_mode_billing_ingress = 2
} dnx_stat_stif_mgmt_report_mode_t;

typedef enum
{
    dnx_stat_stif_mgmt_opcode_use_one_type = 0,
    dnx_stat_stif_mgmt_opcode_use_two_types = 1,
    dnx_stat_stif_mgmt_opcode_use_three_types = 2,
    dnx_stat_stif_mgmt_opcode_use_four_types = 3
} dnx_stat_stif_mgmt_opcode_t;

/**
  * holds report format element (element type and mask) and
  * shift used for sorting in the correct order the elements
  */
typedef struct dnx_stif_mgmt_report_format_element_with_shift_s
{
    bcm_stat_stif_record_format_element_t element;
    int shift;
} dnx_stif_mgmt_report_format_element_with_shift_t;

#define STIF_MGMT_VERIFY_GET 0
#define STIF_MGMT_VERIFY_SET 1

#define DNX_STIF_MGMT_ENABLE_COMPENSATION 1
#define DNX_STIF_MGMT_DISABLE_COMPENSATION 0

/**
 * \brief
 * defines the size of ingress elements (excluding the 4 objects
 * and the PacketSize)
 */
#define DNX_STIF_INGRESS_MAX_RECORD_ELEMENTS (16)
/**
 * \brief
 * defines the size of egress elements (excluding the 4 objects
 * and the PacketSize)
 */
#define DNX_STIF_EGRESS_MAX_RECORD_METADATA_ELEMENTS (10)
 /**
 * \brief
 * defines the max possible nof object elements in a record
 */
#define DNX_STIF_RECORD_MAX_NOF_OBJECT_ELEMENTS (4)

#define DNX_STIF_MGMT_STAT_OBJ_MASK_16_BITS 0xFFFF
#define DNX_STIF_MGMT_STAT_OBJ_MASK_18_BITS 0x3FFFF
#define DNX_STIF_MGMT_STAT_OBJ_SHIFT_16_BITS 16
#define DNX_STIF_MGMT_STAT_OBJ_SHIFT_18_BITS 18

/**
 * holds report format element type and its corresponding dbal
 * mask field and dbal shift field
*/
typedef struct dnx_stif_mgmt_report_format_element_s
{
    bcm_stat_stif_record_element_type_t element_type;
    dbal_fields_e dbal_element_mask_field;
    dbal_fields_e dbal_element_shift_field;
} dnx_stif_mgmt_report_format_element_t;

/**
 * holds table for ingress billing report format types and their
 * corresponding dbal mask and shift fields
 */
static const dnx_stif_mgmt_report_format_element_t ingress_element_enum_to_dbal_fields_binding_table[] = {
    {bcmStatStifRecordElementObj0, DBAL_FIELD_EMPTY, DBAL_FIELD_EMPTY},
    {bcmStatStifRecordElementObj1, DBAL_FIELD_EMPTY, DBAL_FIELD_EMPTY},
    {bcmStatStifRecordElementObj2, DBAL_FIELD_EMPTY, DBAL_FIELD_EMPTY},
    {bcmStatStifRecordElementObj3, DBAL_FIELD_EMPTY, DBAL_FIELD_EMPTY},
    {bcmStatStifRecordElementPacketSize, DBAL_FIELD_EMPTY, DBAL_FIELD_EMPTY},
    {bcmStatStifRecordElementOpCode, DBAL_FIELD_STIF_CGM_BILL_OP_CODE_MASK, DBAL_FIELD_STIF_CGM_BILL_OP_CODE_SHIFT},
    {bcmStatStifRecordElementIngressDispositionIsDrop, DBAL_FIELD_STIF_CGM_BILL_RJCT_MASK,
     DBAL_FIELD_STIF_CGM_BILL_RJCT_SHIFT},
    {bcmStatStifRecordElementIngressTmDropReason, DBAL_FIELD_STIF_CGM_BILL_RJCT_REASON_MASK,
     DBAL_FIELD_STIF_CGM_BILL_RJCT_REASON_SHIFT},
    {bcmStatStifRecordElementIngressTrafficClass, DBAL_FIELD_STIF_CGM_BILL_TC_MASK, DBAL_FIELD_STIF_CGM_BILL_TC_SHIFT},
    {bcmStatStifRecordElementIngressIncomingDropPrecedence, DBAL_FIELD_STIF_CGM_BILL_IN_DP_MASK,
     DBAL_FIELD_STIF_CGM_BILL_IN_DP_SHIFT},
    {bcmStatStifRecordElementIngressDropPrecedenceMeterResolved, DBAL_FIELD_STIF_CGM_BILL_FINAL_DP_MASK,
     DBAL_FIELD_STIF_CGM_BILL_FINAL_DP_SHIFT},
    {bcmStatStifRecordElementIngressDropPrecedenceMeter0Valid, DBAL_FIELD_STIF_CGM_BILL_OBJ_0_DP_VALID_MASK,
     DBAL_FIELD_STIF_CGM_BILL_OBJ_0_DP_VALID_SHIFT},
    {bcmStatStifRecordElementIngressDropPrecedenceMeter0Value, DBAL_FIELD_STIF_CGM_BILL_OBJ_0_DP_MASK,
     DBAL_FIELD_STIF_CGM_BILL_OBJ_0_DP_SHIFT},
    {bcmStatStifRecordElementIngressDropPrecedenceMeter1Valid, DBAL_FIELD_STIF_CGM_BILL_OBJ_1_DP_VALID_MASK,
     DBAL_FIELD_STIF_CGM_BILL_OBJ_1_DP_VALID_SHIFT},
    {bcmStatStifRecordElementIngressDropPrecedenceMeter1Value, DBAL_FIELD_STIF_CGM_BILL_OBJ_1_DP_MASK,
     DBAL_FIELD_STIF_CGM_BILL_OBJ_1_DP_SHIFT},
    {bcmStatStifRecordElementIngressDropPrecedenceMeter2Valid, DBAL_FIELD_STIF_CGM_BILL_OBJ_2_DP_VALID_MASK,
     DBAL_FIELD_STIF_CGM_BILL_OBJ_2_DP_VALID_SHIFT},
    {bcmStatStifRecordElementIngressDropPrecedenceMeter2Value, DBAL_FIELD_STIF_CGM_BILL_OBJ_2_DP_MASK,
     DBAL_FIELD_STIF_CGM_BILL_OBJ_2_DP_SHIFT},
    {bcmStatStifRecordElementIngressCore, DBAL_FIELD_STIF_CGM_BILL_CORE_ID_MASK,
     DBAL_FIELD_STIF_CGM_BILL_CORE_ID_SHIFT},
    {bcmStatStifRecordElementIngressPpMetaData, DBAL_FIELD_STIF_CGM_BILL_PP_META_DATA_MASK,
     DBAL_FIELD_STIF_CGM_BILL_PP_META_DATA_SHIFT},
    {bcmStatStifRecordElementIngressQueueNumber, DBAL_FIELD_STIF_CGM_BILL_QNUM_MASK,
     DBAL_FIELD_STIF_CGM_BILL_QNUM_SHIFT},
    {bcmStatStifRecordElementEgressMetaDataMultiCast, -1, -1},
    {bcmStatStifRecordElementEgressMetaDataPpDropReason, -1, -1},
    {bcmStatStifRecordElementEgressMetaDataPort, -1, -1},
    {bcmStatStifRecordElementEgressMetaDataEcnEligibleAndCni, -1, -1},
    {bcmStatStifRecordElementEgressMetaDataTrafficClass, -1, -1},
    {bcmStatStifRecordElementEgressMetaDataDropPrecedence, -1, -1},
    {bcmStatStifRecordElementEgressMetaDataObj0, -1, -1},
    {bcmStatStifRecordElementEgressMetaDataObj1, -1, -1},
    {bcmStatStifRecordElementEgressMetaDataObj2, -1, -1},
    {bcmStatStifRecordElementEgressMetaDataObj3, -1, -1},
    {bcmStatStifRecordElementEgressMetaDataCore, -1, -1},
    {bcmStatStifRecordElementIngressIsLastCopy, DBAL_FIELD_STIF_CGM_BILL_IS_LAST_COPY_MASK,
     DBAL_FIELD_STIF_CGM_BILL_IS_LAST_COPY_SHIFT}
};

/*
 * Functions prototypes
 * {
 */

/**
* \brief
*   initialize dnx stat stif module
* \param [in] unit - unit id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_stif_init(
    int unit);

/**
* \brief
*   deinitialize dnx stif module
* \param [in] unit - unit id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_stif_deinit(
    int unit);

/**
* \brief
*   for the given port reset the credits for the sif instance
* \param [in] unit - unit id
* \param [in] instance_core - core id of the instance
* \param [in] instance_id -  id of the instance
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_stif_mgmt_credit_reset(
    int unit,
    int instance_core,
    int instance_id);

/**
* \brief
*   for the given logical port return if it's connected to any sif instance,
*   if yes: return the instance id and the core id of the instance
* \param [in] unit - unit id
* \param [in] port - logical port stat
* \param [out] instance_core - core id of the instance
* \param [out] instance_id - the instance id it is represented
* \param [out] is_connected - if port is connected to any of the SIF instances
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_stif_mgmt_logical_port_is_connected(
    int unit,
    bcm_port_t port,
    int *instance_core,
    int *instance_id,
    uint8 *is_connected);
/**
* \brief
*   for the given logical port return the instance id and the
*   core id of the instance
* \param [in] unit - unit id
* \param [in] port - logical port stat
* \param [in] source - source given
* \param [in] instance_core - core id of the instance
* \param [out] instance_id - the instance id it is represented
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_stif_mgmt_logical_port_to_instance_id_get(
    int unit,
    bcm_port_t port,
    bcm_stat_stif_source_t source,
    int *instance_core,
    int *instance_id);

/**
* \brief
*   map in DBAL statistics source (statistics interface source)
*  to logical port
* \param [in] unit    -  unit ID
* \param [in] flags   -  flags
* \param [in] source  -  stif source type and core
* \param [in] port    -  logical port
* \return
*   shr_error_e - Error Type
* \remark
*   *
* \see
*   * None
*/
shr_error_e dnx_stif_mgmt_source_mapping_set(
    int unit,
    int flags,
    bcm_stat_stif_source_t source,
    bcm_port_t port);

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
shr_error_e dnx_stif_mgmt_source_mapping_get(
    int unit,
    int flags,
    bcm_stat_stif_source_t source,
    bcm_port_t * port,
    int *instance_id,
    int *instance_core);

/**
 * \brief -
 * API bcm_stat_control_set - called with control bcmStatControlStifFcEnable
 * Flow control enable (dequeue only)
 * \param [in] unit - unit id
 * \param [in] flags - flags
 * \param [in] fc_enable - flow control enabled/disabled
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stif_mgmt_flow_control_enable_set(
    int unit,
    int flags,
    int fc_enable);

/**
 * \brief -
 * API bcm_stat_control_get - called with control
 * bcmStatControlStifFcEnable Flow control enabled/disabled
 * (dequeue only)
 * \param [in] unit - unit id
 * \param [in] flags - flags
 * \param [out] fc_enable - flow control enabled/disabled
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stif_mgmt_flow_control_enable_get(
    int unit,
    int flags,
    int *fc_enable);

/**
* \brief
*   sort the given array of report formats according to the
*   shift value in ascending order
* \param [in] unit - unit id
* \param [in] object_count - count of the objects
* \param [out] count - count the elements
* \param [out] elements_array_get - elements array with shift
*        and masks values
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_stif_mgmt_ingress_format_elements_shift_values_get(
    int unit,
    int object_count,
    int *count,
    dnx_stif_mgmt_report_format_element_with_shift_t * elements_array_get);

/**
* \brief
*   sort the given array of report formats according to the
*   shift value in ascending order
* \param [in] unit - unit id
* \param [in] is_ingress - if it is called from ingress or
*        egress
* \param [in] nof_elements - nof elements
* \param [in] elements_array - array of elements and shifts
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_stif_mgmt_format_elements_array_sort(
    int unit,
    int is_ingress,
    int nof_elements,
    dnx_stif_mgmt_report_format_element_with_shift_t * elements_array);

/**
* \brief
*   service function - builds the format of the billing records
*   - ingress
* \param [in] unit          -  unit ID
* \param [in] nof_elements  -  nof of elements set in the format
* \param [in] record_format_elements    -  format elements
* \return
*   shr_error_e - Error Type
* \remark
*   *
* \see
*   * None
*/
shr_error_e dnx_stif_mgmt_ingress_record_format_set(
    int unit,
    int nof_elements,
    bcm_stat_stif_record_format_element_t * record_format_elements);

/**
* \brief
*   service function - get the format of the billing records -
*   ingress
* \param [in] unit          -  unit ID
* \param [in] max_nof_elements  -  max nof of elements
* \param [out] elements_array   -  format elements
* \param [out] nof_elements     -  nof elements in the array
* \param [out]  elements_size_array - the sizes of the elements
* \return
*   shr_error_e - Error Type
* \remark
*   *
* \see
*   * None
*/
shr_error_e dnx_stif_mgmt_ingress_record_format_get(
    int unit,
    int max_nof_elements,
    bcm_stat_stif_record_format_element_t * elements_array,
    int *nof_elements,
    int *elements_size_array);

/**
* \brief
*   return the dbal value representation for the given ETPP
*   metadata value)
* \param [in] unit - unit id
* \param [in] value - hw value
* \param [out] opcode_mask - mask value
* \param [out] shift - how many bits to shift
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_stif_mgmt_report_format_opcode_hw_mask_value_get(
    int unit,
    uint32 value,
    uint32 *opcode_mask,
    int *shift);

/**
* \brief
*   service function - builds the format of the billing records
*   - egress
* \param [in] unit          -  unit ID
* \param [in] nof_elements  -  nof of elements set in the format
* \param [in] record_format_elements    -  format elements
* \return
*   shr_error_e - Error Type
* \remark
*   *
* \see
*   * None
*/
shr_error_e dnx_stif_mgmt_egress_record_format_set(
    int unit,
    int nof_elements,
    bcm_stat_stif_record_format_element_t * record_format_elements);

/**
* \brief
*   service function - get the format of the billing records -
*   egress
* \param [in] unit          -  unit ID
* \param [in] max_nof_elements  -  max nof of elements
* \param [out] elements_array   -  format elements
* \param [out] nof_elements     -  nof elements in the array
* \param [out]  elements_size_array - the sizes of the
*   elements
* \return
*   shr_error_e - Error Type
* \remark
*   *
* \see
*   * None
*/
shr_error_e dnx_stif_mgmt_egress_record_format_get(
    int unit,
    int max_nof_elements,
    bcm_stat_stif_record_format_element_t * elements_array,
    int *nof_elements,
    int *elements_size_array);

/**
* \brief
*   verify function for API bcm_cosq_control_range_set
* \param [in] unit      -  unit ID
* \param [in] verify_set   -  if is called from set or get API
* \param [in] port      - gport
* \param [in] type     -  control type - Queues/ScrubberQueues
* \param [in] range   -  queue range
* \return
*   shr_error_e - Error Type
* \remark
*   *
* \see
*   * None
*/
shr_error_e dnx_stif_mgmt_control_range_verify(
    int unit,
    int verify_set,
    bcm_gport_t port,
    bcm_cosq_control_range_type_t type,
    bcm_cosq_range_t * range);

/**
* \brief
*   configure range of queues for queues/scrubber reports -
*   service func to API bcm_cosq_control_range_set
*   type=bcmCosqStatIfQueues or type=bcmCosqStatIfScrubberQueues
*   for StatIfQueues - configure the 2 queues with same range
*
*   valid for all report modes
* \param [in] unit - unit id
* \param [in] core - core id
* \param [in] type - qnum or scrubber
* \param [in] range_start - q range start
* \param [in] range_end - q range end
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_stif_mgmt_queues_range_dbal_set(
    int unit,
    bcm_core_t core,
    bcm_cosq_control_range_type_t type,
    int range_start,
    int range_end);

/**
* \brief
*   get queue range for qsize/scrubber  reports - service func
*   to API bcm_cosq_control_range_get type=bcmCosqStatIfQueues
*   or type=bcmCosqStatIfScrubberQueues
*   for StatIfQueues return range of queue 0 (API assumes both
*   has the same value)
*
*   valid for all report modes
* \param [in] unit - unit id
* \param [in] core - core id
* \param [in] type - type - qnum or scrubber
* \param [out] range_start - q range start
* \param [out] range_end - q range end
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_stif_mgmt_queues_range_dbal_get(
    int unit,
    bcm_core_t core,
    bcm_cosq_control_range_type_t type,
    int *range_start,
    int *range_end);

/**
 * \brief -
 * verification for API bcm_stat_pkt_size_adjust_select_set/get
 * \param [in] unit -  Unit-ID
 * \param [in] key - core,source,command_id - only core is used
 * \param [in] select_type - header adjust select type
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stif_mgmt_pkt_size_adjust_select_verify(
    int unit,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_pkt_size_adjust_select_type_t select_type);

/**
 * \brief -
 * verification for API bcm_stat_pkt_size_adjust_select_set
 * \param [in] unit -  Unit-ID
 * \param [in] key - core,source,command_id - only core is used
 * \param [in] select_type - header adjust select type
 * \param [in] enable - header-truncate enabled/disabled
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stif_mgmt_pkt_size_adjust_select_set_verify(
    int unit,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_pkt_size_adjust_select_type_t select_type,
    int enable);

/**
 * \brief -
 * for IRPP/ITM set which IRPP/ITM mask is used - enable/disable
 * header truncate compensation
 * \param [in] unit -  Unit-ID
 * \param [in] key - core,source,command_id
 * \param [in] select_type - header adjust select type
 * \param [in] enable - header-truncate enabled/disabled
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stif_mgmt_pkt_size_adjust_select_set(
    int unit,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_pkt_size_adjust_select_type_t select_type,
    int enable);

/**
 * \brief -
 * for IRPP/ITM get which IRPP/ITM mask is used - if
 * header-truncate is enabled or not
 * \param [in] unit -  Unit-ID
 * \param [in] key - core,source,command_id
 * \param [in] select_type - header adjust select type
 * \param [out] enable - header-truncate enabled/disabled
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stif_mgmt_pkt_size_adjust_select_get(
    int unit,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_pkt_size_adjust_select_type_t select_type,
    int *enable);

/**
 * \brief -
 * verification for API bcm_stat_pkt_size_adjust_select_set/get
 * \param [in] unit -  Unit-ID
 * \param [in] key - core,source,command_id
 * \param [in] filter - select filter group to be set
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stif_mgmt_filter_group_verify(
    int unit,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_counter_group_filter_t filter);

/**
 * \brief -
 * verification for API bcm_stat_pkt_size_adjust_select_set
 * \param [in] unit -  Unit-ID
 * \param [in] key - core,source,command_id
 * \param [in] filter - select filter group to be set
 * \param [in] is_active - is the filter group active
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stif_mgmt_filter_group_set_verify(
    int unit,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_counter_group_filter_t filter,
    int is_active);

/**
 * \brief
 * set filter criteria group to be filtered in/out
 * \param [in] unit - unit id
 * \param [in] key - core, source, command_id
 * \param [in] filter - filter group
 * \param [in] is_active - activate/deactivate the filter group
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_stif_mgmt_filter_group_set(
    int unit,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_counter_group_filter_t filter,
    int is_active);

/**
 * \brief
 * determine if certain filter is active
 * \param [in] unit - unit id
 * \param [in] key - core, source, command_id
 * \param [in] filter - filter group
 * \param [out] is_active - is the filter active
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_stif_mgmt_filter_group_get(
    int unit,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_counter_group_filter_t filter,
    int *is_active);

#endif/*_DNX_STIF_MGMT_INCLUDED__*/
