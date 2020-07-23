/** \file crps_src_interface.c
 * 
 *  crps module gather all functionality that related to the counter/statisitc processor driver for DNX.
 *  crps_src_interface.c responsible of source interface/adapter configuration and other source configurations reltaed to CRPS.
 *  It is internal crps file . (do not have interfaces with other modules)
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_CNT

/*
 * Include files.
 * {
 */
#include <shared/utilex/utilex_bitstream.h>
#include <shared/shrextend/shrextend_error.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/stat.h>
#include "crps_mgmt_internal.h"
#include <bcm_int/dnx/stat/crps/crps_src_interface.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_crps.h>
#include <include/shared/bitop.h>
#include <include/bcm_int/dnx/cmn/dnxcmn.h>
#include <shared/utilex/utilex_integer_arithmetic.h>

/*
 * }
 */

/*
 * MACROs
 * {
 */

/** 
  * \brief - get the dbal enum type for specific object index meta-Data type. (Assume that objects enum types are consecutive)
  */
#define DNX_CRPS_SRC_INT_ETPP_OBJ_X_DATA_DBAL_TYPE(_obj_idx) \
    (DBAL_ENUM_FVAL_CRPS_ETPP_COUNTER_DATA_TYPE_TYPE_OBJ0_META_DATA + _obj_idx)

/*
 * }
 */

#define DNX_CRPS_SRC_INTERFACE_DROP_REASON_3_SIZE (3) /**  number of bits for each field that is part of the expansion selection */
#define DNX_CRPS_SRC_INTERFACE_DISPOSITION_SIZE (1) /**  number of bits for each field that is part of the expansion selection */
#define DNX_CRPS_SRC_INTERFACE_TC_SIZE (3) /**  number of bits for each field that is part of the expansion selection */
#define DNX_CRPS_SRC_INTERFACE_DPx_VALID_SIZE (1) /**  number of bits for each field that is part of the expansion selection */
#define DNX_CRPS_SRC_INTERFACE_DP_VALUE_SIZE (2) /**  number of bits for each field that is part of the expansion selection */
#define DNX_CRPS_SRC_INTERFACE_LATENCY_BIN_SIZE (3) /**  number of bits for each field that is part of the expansion selection */
#define DNX_CRPS_SRC_INTERFACE_ITPP_LATENCY_PROFILE_SIZE (5) /**  number of bits for field that is part of the expansion selection */
#define DNX_CRPS_SRC_INTERFACE_MC_SIZE (1) /**  number of bits for each field that is part of the expansion selection */
#define DNX_CRPS_SRC_INTERFACE_TM_ACTION_SIZE (2) /**  number of bits for each field that is part of the expansion selection */
#define DNX_CRPS_SRC_INTERFACE_PP_DROP_REASON_SIZE (2) /**  number of bits for each field that is part of the expansion selection */
#define DNX_CRPS_SRC_INTERFACE_ECN_CNI_SIZE (2) /**  number of bits for each field that is part of the expansion selection */
#define DNX_CRPS_SRC_INTERFACE_PORT_SIZE (8) /**  number of bits for each field that is part of the expansion selection */
#define DNX_CRPS_SRC_INTERFACE_IRPP_META_DATA_SIZE (16) /**  number of bits for each field that is part of the expansion selection */
#define DNX_CRPS_SRC_INTERFACE_ITPP_META_DATA_SIZE (3) /**  number of bits for each field that is part of the expansion selection */
#define DNX_CRPS_SRC_INTERFACE_ERPP_META_DATA_SIZE (4) /**  number of bits for each field that is part of the expansion selection */
#define DNX_CRPS_SRC_INTERFACE_ETPP_OBJ_X_META_DATA_SIZE (4) /**  number of bits for each field that is part of the expansion selection */
#define DNX_CRPS_SRC_INTERFACE_ETM_DISCARD_SIZE (1) /**  number of bits for each field that is part of the expansion selection */
#define DNX_CRPS_SRC_INTERFACE_TM_LAST_COPY (1) /**  number of bits for each field that is part of the expansion selection */

#define DNX_CRPS_INGRESS_COMPENSATION_HEADER_TRUNCATE_DISABLE (10) /** enable IPP-Header-Delta(bit 3), IRPP-Delta(bit 1) - 01010 */
#define DNX_CRPS_INGRESS_COMPENSATION_HEADER_TRUNCATE_ENABLE (11) /** enable IPP-Header-Delta(bit 3),IRPP-Delta(bit 1), Header-Truncate-Size(bit 0) - 01011*/

#define EIGHT_BITS (8)
/*
 * }
 */

/**
 * \brief
 *      get the etpp meta data format configuration. each API call agive back position of one type
 * \param [in] unit -
 *     unit id
 * \param [in] core_id -
 *     core id
 * \param [in] type -
 *     The expansion type that user wants to get its position in the metadata format
 * \param [out] shift_val -
 *     the position of the type in the meta data.
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
static shr_error_e
dnx_crps_src_interface_etpp_metadata_type_shift_get(
    int unit,
    bcm_core_t core_id,
    dbal_enum_value_field_crps_etpp_counter_data_type_e type,
    uint32 *shift_val)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_ETPP_COUNTER_DATA_SELECTION, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ETPP_COUNTER_DATA_TYPE, type);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SHIFT_VALUE, INST_SINGLE, shift_val);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_src_interface_data_field_params_get(
    int unit,
    int core_id,
    bcm_stat_counter_interface_type_t src,
    int command_id,
    bcm_stat_expansion_types_t field_type,
    uint32 *size,
    uint32 *field_offset)
{
    uint32 nof_bits = 0, offset = 0;
    const dnx_data_crps_expansion_irpp_types_t *irpp_types;
    const dnx_data_crps_expansion_itpp_types_t *itpp_types;
    const dnx_data_crps_expansion_itm_types_t *itm_types;
    const dnx_data_crps_expansion_erpp_types_t *erpp_types;
    const dnx_data_crps_expansion_etm_types_t *etm_types;
    SHR_FUNC_INIT_VARS(unit);

    switch (src)
    {
        case bcmStatCounterInterfaceIngressReceivePp:
        case bcmStatCounterInterfaceIngressOam:
            irpp_types = dnx_data_crps.expansion.irpp_types_get(unit);
            switch (field_type)
            {
                case bcmStatExpansionTypeTmDropReason:
                    nof_bits = DNX_CRPS_SRC_INTERFACE_DROP_REASON_3_SIZE;
                    offset = irpp_types->drop_reason_offset;
                    break;
                case bcmStatExpansionTypeDispositionIsDrop:
                    nof_bits = DNX_CRPS_SRC_INTERFACE_DISPOSITION_SIZE;
                    offset = irpp_types->disposition_offset;
                    break;
                case bcmStatExpansionTypeTrafficClass:
                    nof_bits = DNX_CRPS_SRC_INTERFACE_TC_SIZE;
                    offset = irpp_types->tc_offset;
                    break;
                case bcmStatExpansionTypeDropPrecedenceMeter0Valid:
                    nof_bits = DNX_CRPS_SRC_INTERFACE_DPx_VALID_SIZE;
                    offset = irpp_types->dp0_valid_offset;
                    break;
                case bcmStatExpansionTypeDropPrecedenceMeter0Value:
                    nof_bits = DNX_CRPS_SRC_INTERFACE_DP_VALUE_SIZE;
                    offset = irpp_types->dp0_value_offset;
                    break;
                case bcmStatExpansionTypeDropPrecedenceMeter1Valid:
                    nof_bits = DNX_CRPS_SRC_INTERFACE_DPx_VALID_SIZE;
                    offset = irpp_types->dp1_valid_offset;
                    break;
                case bcmStatExpansionTypDropPrecedenceMeter1Value:
                    nof_bits = DNX_CRPS_SRC_INTERFACE_DP_VALUE_SIZE;
                    offset = irpp_types->dp1_value_offset;
                    break;
                case bcmStatExpansionTypeDropPrecedenceMeter2Valid:
                    nof_bits = DNX_CRPS_SRC_INTERFACE_DPx_VALID_SIZE;
                    offset = irpp_types->dp2_valid_offset;
                    break;
                case bcmStatExpansionTypeDropPrecedenceMeter2Value:
                    nof_bits = DNX_CRPS_SRC_INTERFACE_DP_VALUE_SIZE;
                    offset = irpp_types->dp2_value_offset;
                    break;
                case bcmStatExpansionTypeDropPrecedenceMeterResolved:
                    nof_bits = DNX_CRPS_SRC_INTERFACE_DP_VALUE_SIZE;
                    offset = irpp_types->final_dp_offset;
                    break;
                case bcmStatExpansionTypeMetaData:
                    nof_bits = DNX_CRPS_SRC_INTERFACE_IRPP_META_DATA_SIZE;
                    offset = irpp_types->metadata_offset;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, " invalid expansion_elements type. src=%d, type=%d\n", src, field_type);
                    break;
            }
            break;
        case bcmStatCounterInterfaceIngressTransmitPp:
            itpp_types = dnx_data_crps.expansion.itpp_types_get(unit);
            switch (field_type)
            {
                case bcmStatExpansionTypeLatencyBin:
                    nof_bits = DNX_CRPS_SRC_INTERFACE_LATENCY_BIN_SIZE;
                    offset = itpp_types->latency_bin_offset;
                    break;
                case bcmStatExpansionTypeTrafficClass:
                    nof_bits = DNX_CRPS_SRC_INTERFACE_TC_SIZE;
                    offset = itpp_types->tc_offset;
                    break;
                case bcmStatExpansionTypeLatencyFlowProfile:
                    nof_bits = DNX_CRPS_SRC_INTERFACE_ITPP_LATENCY_PROFILE_SIZE;
                    offset = itpp_types->latency_profile_offset;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, " invalid expansion_elements type. src=%d, type=%d\n", src, field_type);
                    break;
            }
            break;
        case bcmStatCounterInterfaceEgressReceivePp:
            erpp_types = dnx_data_crps.expansion.erpp_types_get(unit);
            switch (field_type)
            {
                case bcmStatExpansionTypeMetaData:
                    nof_bits = DNX_CRPS_SRC_INTERFACE_ERPP_META_DATA_SIZE;
                    offset = erpp_types->metadata_offset;
                    break;
                case bcmStatExpansionTypeDropPrecedenceMeterResolved:
                    nof_bits = DNX_CRPS_SRC_INTERFACE_DP_VALUE_SIZE;
                    offset = erpp_types->dp_offset;
                    break;
                case bcmStatExpansionTypeTrafficClass:
                    nof_bits = DNX_CRPS_SRC_INTERFACE_TC_SIZE;
                    offset = erpp_types->tc_offset;
                    break;
                case bcmStatExpansionTypeSystemMultiCast:
                    nof_bits = DNX_CRPS_SRC_INTERFACE_MC_SIZE;
                    offset = erpp_types->mc_offset;
                    break;
                case bcmStatExpansionTypeEgressTmActionType:
                    nof_bits = DNX_CRPS_SRC_INTERFACE_TM_ACTION_SIZE;
                    offset = erpp_types->tm_action_type_offset;
                    break;
                case bcmStatExpansionTypePpDropReason:
                    nof_bits = DNX_CRPS_SRC_INTERFACE_PP_DROP_REASON_SIZE;
                    offset = erpp_types->pp_drop_reason_offset;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, " invalid expansion_elements type. src=%d, type=%d\n", src, field_type);
                    break;
            }
            break;
        case bcmStatCounterInterfaceIngressVoq:
            itm_types = dnx_data_crps.expansion.itm_types_get(unit);
            switch (field_type)
            {
                case bcmStatExpansionTypeTmDropReason:
                    nof_bits = DNX_CRPS_SRC_INTERFACE_DROP_REASON_3_SIZE;
                    offset = itm_types->drop_reason_offset;
                    break;
                case bcmStatExpansionTypeDispositionIsDrop:
                    nof_bits = DNX_CRPS_SRC_INTERFACE_DISPOSITION_SIZE;
                    offset = itm_types->disposition_offset;
                    break;
                case bcmStatExpansionTypeTrafficClass:
                    nof_bits = DNX_CRPS_SRC_INTERFACE_TC_SIZE;
                    offset = itm_types->tc_offset;
                    break;
                case bcmStatExpansionTypeDropPrecedenceMeterResolved:
                    nof_bits = DNX_CRPS_SRC_INTERFACE_DP_VALUE_SIZE;
                    offset = itm_types->final_dp_offset;
                    break;
                case bcmStatExpansionTypeTmLastCopy:
                    nof_bits = DNX_CRPS_SRC_INTERFACE_TM_LAST_COPY;
                    offset = itm_types->last_copy_offset;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, " invalid expansion_elements type. src=%d, type=%d\n", src, field_type);
                    break;
            }
            break;
        case bcmStatCounterInterfaceEgressReceiveQueue:
            etm_types = dnx_data_crps.expansion.etm_types_get(unit);
            switch (field_type)
            {
                case bcmStatExpansionTypeTmDropReason:
                    nof_bits = DNX_CRPS_SRC_INTERFACE_DROP_REASON_3_SIZE;
                    offset = etm_types->drop_reason_offset;
                    break;
                case bcmStatExpansionTypeDiscardTm:
                    nof_bits = DNX_CRPS_SRC_INTERFACE_ETM_DISCARD_SIZE;
                    offset = etm_types->tm_discard_offset;
                    break;
                case bcmStatExpansionTypeDiscardPp:
                    nof_bits = DNX_CRPS_SRC_INTERFACE_ETM_DISCARD_SIZE;
                    offset = etm_types->pp_discard_offset;
                    break;
                case bcmStatExpansionTypeTrafficClass:
                    nof_bits = DNX_CRPS_SRC_INTERFACE_TC_SIZE;
                    offset = etm_types->tc_offset;
                    break;
                case bcmStatExpansionTypeDropPrecedenceMeterResolved:
                    nof_bits = DNX_CRPS_SRC_INTERFACE_DP_VALUE_SIZE;
                    offset = etm_types->dp_offset;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, " invalid expansion_elements type. src=%d, type=%d\n", src, field_type);
                    break;
            }
            break;
        case bcmStatCounterInterfaceEgressTransmitPp:
        case bcmStatCounterInterfaceEgressOam:
            switch (field_type)
            {
                case bcmStatExpansionTypeDropPrecedenceMeterResolved:
                    nof_bits = DNX_CRPS_SRC_INTERFACE_DP_VALUE_SIZE;
                    SHR_IF_ERR_EXIT(dnx_crps_src_interface_etpp_metadata_type_shift_get
                                    (unit, core_id, DBAL_ENUM_FVAL_CRPS_ETPP_COUNTER_DATA_TYPE_TYPE_DP, &offset));
                    break;
                case bcmStatExpansionTypeTrafficClass:
                    nof_bits = DNX_CRPS_SRC_INTERFACE_TC_SIZE;
                    SHR_IF_ERR_EXIT(dnx_crps_src_interface_etpp_metadata_type_shift_get
                                    (unit, core_id, DBAL_ENUM_FVAL_CRPS_ETPP_COUNTER_DATA_TYPE_TYPE_TC, &offset));
                    break;
                case bcmStatExpansionTypeSystemMultiCast:
                    nof_bits = DNX_CRPS_SRC_INTERFACE_MC_SIZE;
                    SHR_IF_ERR_EXIT(dnx_crps_src_interface_etpp_metadata_type_shift_get
                                    (unit, core_id, DBAL_ENUM_FVAL_CRPS_ETPP_COUNTER_DATA_TYPE_TYPE_IS_MC, &offset));
                    break;
                case bcmStatExpansionTypeEcnEligibleAndCni:
                    nof_bits = DNX_CRPS_SRC_INTERFACE_ECN_CNI_SIZE;
                    SHR_IF_ERR_EXIT(dnx_crps_src_interface_etpp_metadata_type_shift_get
                                    (unit, core_id, DBAL_ENUM_FVAL_CRPS_ETPP_COUNTER_DATA_TYPE_TYPE_ECN_CNI, &offset));
                    break;
                case bcmStatExpansionTypePortMetaData:
                    nof_bits = DNX_CRPS_SRC_INTERFACE_PORT_SIZE;
                    SHR_IF_ERR_EXIT(dnx_crps_src_interface_etpp_metadata_type_shift_get
                                    (unit, core_id, DBAL_ENUM_FVAL_CRPS_ETPP_COUNTER_DATA_TYPE_TYPE_PORT_DATA,
                                     &offset));
                    break;
                case bcmStatExpansionTypePpDropReason:
                    nof_bits = DNX_CRPS_SRC_INTERFACE_PP_DROP_REASON_SIZE;
                    SHR_IF_ERR_EXIT(dnx_crps_src_interface_etpp_metadata_type_shift_get
                                    (unit, core_id, DBAL_ENUM_FVAL_CRPS_ETPP_COUNTER_DATA_TYPE_TYPE_PP_DROP_REASON,
                                     &offset));
                    break;
                case bcmStatExpansionTypeMetaData:
                    nof_bits = DNX_CRPS_SRC_INTERFACE_ETPP_OBJ_X_META_DATA_SIZE;
                    SHR_IF_ERR_EXIT(dnx_crps_src_interface_etpp_metadata_type_shift_get(unit, core_id,
                                                                                        DNX_CRPS_SRC_INT_ETPP_OBJ_X_DATA_DBAL_TYPE
                                                                                        (command_id), &offset));
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, " invalid expansion_elements type.src=%d, type=%d\n", src, field_type);
                    break;
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, " invalid expansion_elements type. src=%d, type=%d\n", src, field_type);
            break;
    }

    *size = nof_bits;
    if (field_offset != NULL)
    {
        *field_offset = offset;
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief 
 *   expansion selection calculation. go over all the expansion selection elements from API and build a bitmap of expansion selection
 *  need to select from x bits meta-data only max 10 bits.
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID 
 *   \param [in] core_id - core_id
 * \par INDIRECT INPUT:
 *   \param [in] src  -  the source block.
 *   \param [in] command_id  - command/object id
 *   \param [in] expansion_select  -  configuration structure
 *   \param [out] ret_val  -  the return value for the expansion selection bit map.  define which bits to consider when building the expansion. 
 * \par INDIRECT INPUT: 
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_crps_src_interface_expansion_select_calc(
    int unit,
    int core_id,
    bcm_stat_counter_interface_type_t src,
    int command_id,
    bcm_stat_expansion_select_t * expansion_select,
    uint32 *ret_val)
{
    int elem_index;
    uint32 nof_bits, offset;
    SHR_FUNC_INIT_VARS(unit);

    /** build the selection bit map */
    for (elem_index = 0; elem_index < expansion_select->nof_elements; elem_index++)
    {
        SHR_IF_ERR_EXIT(dnx_crps_src_interface_data_field_params_get
                        (unit, core_id, src, command_id, expansion_select->expansion_elements[elem_index].type,
                         &nof_bits, &offset));
        /** for need to copy the set the bits according to the bitmap. */
        SHR_BITCOPY_RANGE(ret_val, offset, &expansion_select->expansion_elements[elem_index].bitmap, 0, nof_bits);
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_src_interface_irpp_expansion_select_set(
    int unit,
    bcm_stat_counter_interface_key_t * interface,
    bcm_stat_expansion_select_t * expansion_select)
{
    uint32 selection[CRPS_MGMT_SELECTION_ARR_SIZE] = { 0 };
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_crps_src_interface_expansion_select_calc
                    (unit, interface->core_id, interface->interface_source, interface->command_id, expansion_select,
                     selection));

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_IRPP_INTERFACE_CFG, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_IF_OBJECT_ID, interface->command_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, interface->core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_IF_TYPE_ID, interface->type_id);

    /** Setting value fields */
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_IRPP_CTR_EXPANSION_SELECT, INST_SINGLE,
                                     selection);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_src_interface_itpp_expansion_select_set(
    int unit,
    bcm_stat_counter_interface_key_t * interface,
    bcm_stat_expansion_select_t * expansion_select)
{
    uint32 selection[CRPS_MGMT_SELECTION_ARR_SIZE] = { 0 };

    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** calc the expansion selection bitmap */
    SHR_IF_ERR_EXIT(dnx_crps_src_interface_expansion_select_calc
                    (unit, interface->core_id, interface->interface_source, interface->command_id, expansion_select,
                     selection));

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_ITPP_INTERFACE_CFG, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, interface->core_id);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ITPP_LTNCY_CTR_EXPANSION_SELECT, INST_SINGLE,
                                 selection[0]);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_src_interface_erpp_expansion_select_set(
    int unit,
    bcm_stat_counter_interface_key_t * interface,
    bcm_stat_expansion_select_t * expansion_select)
{
    uint32 selection[CRPS_MGMT_SELECTION_ARR_SIZE] = { 0 };
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** calc the expansion selection bitmap */
    SHR_IF_ERR_EXIT(dnx_crps_src_interface_expansion_select_calc
                    (unit, interface->core_id, interface->interface_source, interface->command_id, expansion_select,
                     selection));

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_ERPP_INTERFACE_CFG, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_IF_OBJECT_ID, interface->command_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, interface->core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_IF_TYPE_ID, interface->type_id);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ERPP_CTR_EXPANSION_SELECT, INST_SINGLE,
                                 selection[0]);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_src_interface_etpp_expansion_select_set(
    int unit,
    bcm_stat_counter_interface_key_t * interface,
    bcm_stat_expansion_select_t * expansion_select)
{
    uint32 selection[CRPS_MGMT_SELECTION_ARR_SIZE] = { 0 };
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** calc the expansion selection bitmap */
    SHR_IF_ERR_EXIT(dnx_crps_src_interface_expansion_select_calc
                    (unit, interface->core_id, interface->interface_source, interface->command_id, expansion_select,
                     selection));

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_ETPP_INTERFACE_CFG, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_IF_ETPP_OBJECT_ID, interface->command_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, interface->core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_IF_TYPE_ID, interface->type_id);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ETPP_CTR_EXPANSION_SELECT, INST_SINGLE,
                                 selection[0]);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/** see .h file */
shr_error_e
dnx_crps_src_interface_expansion_field_is_selected(
    int unit,
    int core_id,
    bcm_stat_counter_interface_type_t src,
    int command_id,
    bcm_stat_expansion_types_t field_type,
    uint32 *data,
    int *is_selected,
    uint32 *bitmap)
{
    uint32 field_val = 0;
    uint32 nof_bits, offset;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_crps_src_interface_data_field_params_get
                    (unit, core_id, src, command_id, field_type, &nof_bits, &offset));

    SHR_BITCOPY_RANGE(&field_val, 0, data, offset, nof_bits);
    *is_selected = (field_val) ? TRUE : FALSE;

    /** if metadata, need to update the metadata bit map */
    if (*is_selected == TRUE)
    {
        *bitmap = field_val;
    }

exit:
    SHR_FUNC_EXIT;

}

/** see .h file */
shr_error_e
dnx_crps_src_interface_expansion_select_get(
    int unit,
    int core_id,
    bcm_stat_counter_interface_type_t src,
    int command_id,
    uint32 *selection_arr,
    const bcm_stat_expansion_types_t * valid_types,
    bcm_stat_expansion_select_t * expansion_select)
{
    int type_index = 0, is_selected = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    expansion_select->nof_elements = 0;

    /** fill the structure of the expansion selection, base of the HW field. for each field that may be part of the expansion, 
              check the bits that represent it are set or not.  If set, add this field to the returned structure */
    while (valid_types[type_index] != bcmStatExpansionTypeMax)
    {
        expansion_select->expansion_elements[expansion_select->nof_elements].bitmap = 0;
        SHR_IF_ERR_EXIT(dnx_crps_src_interface_expansion_field_is_selected
                        (unit, core_id, src, command_id, valid_types[type_index], selection_arr, &is_selected,
                         &expansion_select->expansion_elements[expansion_select->nof_elements].bitmap));
        if (is_selected == TRUE)
        {
            expansion_select->expansion_elements[expansion_select->nof_elements].type = valid_types[type_index];
            expansion_select->nof_elements++;
        }
        type_index++;
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_src_interface_irpp_expansion_select_hw_get(
    int unit,
    bcm_stat_counter_interface_key_t * interface,
    uint32 *val_arr)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_IRPP_INTERFACE_CFG, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_IF_OBJECT_ID, interface->command_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, interface->core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_IF_TYPE_ID, interface->type_id);

    /** Setting pointers value to receive the fields */
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_CRPS_IRPP_CTR_EXPANSION_SELECT, INST_SINGLE,
                                   val_arr);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_src_interface_itpp_expansion_select_hw_get(
    int unit,
    bcm_stat_counter_interface_key_t * interface,
    uint32 *selection)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_ITPP_INTERFACE_CFG, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, interface->core_id);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CRPS_ITPP_LTNCY_CTR_EXPANSION_SELECT, INST_SINGLE,
                               selection);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_src_interface_erpp_expansion_select_hw_get(
    int unit,
    bcm_stat_counter_interface_key_t * interface,
    uint32 *selection)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_ERPP_INTERFACE_CFG, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_IF_OBJECT_ID, interface->command_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, interface->core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_IF_TYPE_ID, interface->type_id);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CRPS_ERPP_CTR_EXPANSION_SELECT, INST_SINGLE,
                               selection);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_src_interface_etpp_expansion_select_hw_get(
    int unit,
    bcm_stat_counter_interface_key_t * interface,
    uint32 *selection)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_ETPP_INTERFACE_CFG, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_IF_ETPP_OBJECT_ID, interface->command_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, interface->core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_IF_TYPE_ID, interface->type_id);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CRPS_ETPP_CTR_EXPANSION_SELECT, INST_SINGLE,
                               selection);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *      convert expansion types from BCM enum to dbal enum
 * \param [in] unit -
 *     unit id
 * \param [in] bcm_type -
 *    expansion type in bcm enum units 
 * \param [in] object_idx -
 *     there is one meta_data type in bcm level for all objects in the etpp, but in dbal level, there is a type for each object[x] meta_data.
 *     therefore, we need the object_idx in order to convert it (one>many).
 * \param [out] dbal_type -
 *        dbal type
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
static shr_error_e
dnx_crps_src_interface_etpp_type_convert(
    int unit,
    bcm_stat_expansion_types_t bcm_type,
    int object_idx,
    dbal_enum_value_field_crps_etpp_counter_data_type_e * dbal_type)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (bcm_type)
    {
        case bcmStatExpansionTypeDropPrecedenceMeterResolved:
            *dbal_type = DBAL_ENUM_FVAL_CRPS_ETPP_COUNTER_DATA_TYPE_TYPE_DP;
            break;
        case bcmStatExpansionTypeTrafficClass:
            *dbal_type = DBAL_ENUM_FVAL_CRPS_ETPP_COUNTER_DATA_TYPE_TYPE_TC;
            break;
        case bcmStatExpansionTypeSystemMultiCast:
            *dbal_type = DBAL_ENUM_FVAL_CRPS_ETPP_COUNTER_DATA_TYPE_TYPE_IS_MC;
            break;
        case bcmStatExpansionTypeEcnEligibleAndCni:
            *dbal_type = DBAL_ENUM_FVAL_CRPS_ETPP_COUNTER_DATA_TYPE_TYPE_ECN_CNI;
            break;
        case bcmStatExpansionTypePortMetaData:
            *dbal_type = DBAL_ENUM_FVAL_CRPS_ETPP_COUNTER_DATA_TYPE_TYPE_PORT_DATA;
            break;
        case bcmStatExpansionTypePpDropReason:
            *dbal_type = DBAL_ENUM_FVAL_CRPS_ETPP_COUNTER_DATA_TYPE_TYPE_PP_DROP_REASON;
            break;
        case bcmStatExpansionTypeMetaData:
            /** assuming enum values for Obj[x]_meta_data are consecutive. so we take the base val and add the object_idx */
            *dbal_type = DNX_CRPS_SRC_INT_ETPP_OBJ_X_DATA_DBAL_TYPE(object_idx);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, " invalid bcm_type. type=%d\n", bcm_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *      set the etpp meta data format configuration. each API call add/remove one data type to the format configuration.
 * \param [in] unit -
 *     unit id
 * \param [in] core_id -
 *     core id
 * \param [in] type -
 *     The type that need to add/remove to/from the format configuration
 * \param [in] shift_val -
 *     the position of the type in the meta data. total 24 bits. It means that to remove a type, need to set it shift_value=24.
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
static shr_error_e
dnx_crps_src_interface_etpp_metadata_type_shift_set(
    int unit,
    bcm_core_t core_id,
    dbal_enum_value_field_crps_etpp_counter_data_type_e type,
    uint32 shift_val)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

        /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_ETPP_COUNTER_DATA_SELECTION, &entry_handle_id));

        /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ETPP_COUNTER_DATA_TYPE, type);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SHIFT_VALUE, INST_SINGLE, shift_val);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_src_interface_etpp_meta_data_selection_init(
    int unit)
{
    int core_idx;
    int type_idx, obj_idx;
    const dnx_data_crps_expansion_etpp_types_t *etpp_types = dnx_data_crps.expansion.etpp_types_get(unit);
    dbal_enum_value_field_crps_etpp_counter_data_type_e dbal_type;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_idx)
    {
        type_idx = 0;
        obj_idx = 0;
        /** running on all etpp valid types and give them invalid shift. */
        /** valid_type array ends with value "bcmStatExpansionTypeMax", which is the loop stop condition */
        while (etpp_types->valid_types[type_idx] != bcmStatExpansionTypeMax)
        {
            SHR_IF_ERR_EXIT(dnx_crps_src_interface_etpp_type_convert
                            (unit, etpp_types->valid_types[type_idx], obj_idx, &dbal_type));

            SHR_IF_ERR_EXIT(dnx_crps_src_interface_etpp_metadata_type_shift_set
                            (unit, core_idx, dbal_type, etpp_types->total_nof_bits));

                        /** meta_data type gather all sub meta_datas per object_if, therefore, we will init all sub meta datas */
                        /** T.B.D - change the value 6 to dnx_data */
            if ((etpp_types->valid_types[type_idx] == bcmStatExpansionTypeMetaData) && (obj_idx < 5))
            {
                obj_idx++;
            }
            else
            {
                type_idx++;
                obj_idx = 0;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *      calc the next available place in the etpp meta data and return its location. 
 *      this is the next_shift_value to set for the next type in the etpp metadata format
 * \param [in] unit -
 *     unit id
 * \param [in] core_id -
 *     core id 
 * \param [out] ret_val -
 *     return value is the next position available in the etpp metadat format
 * \return
 *   see shr_error_e
 * \remark
 *   NONE
 * \see
 *   NONE
 */
static shr_error_e
dnx_crps_src_interface_etpp_next_shift_val_calc(
    int unit,
    int core_id,
    uint32 *ret_val)
{
    int type_idx = 0, obj_idx = 0;
    const dnx_data_crps_expansion_etpp_types_t *etpp_types = dnx_data_crps.expansion.etpp_types_get(unit);
    dbal_enum_value_field_crps_etpp_counter_data_type_e dbal_type;
    uint32 shift_value, next_shift_value;
    uint32 nof_bits;

    SHR_FUNC_INIT_VARS(unit);

    next_shift_value = 0;

    /** running on all etpp valid types and search the next available offset */
    /** valid_type array ends with value "bcmStatExpansionTypeMax", which is the loop stop condition */
    while (etpp_types->valid_types[type_idx] != bcmStatExpansionTypeMax)
    {
        SHR_IF_ERR_EXIT(dnx_crps_src_interface_etpp_type_convert
                        (unit, etpp_types->valid_types[type_idx], obj_idx, &dbal_type));

        SHR_IF_ERR_EXIT(dnx_crps_src_interface_etpp_metadata_type_shift_get(unit, core_id, dbal_type, &shift_value));

        /** hold the next shift value, which is in the valid range */
        if ((shift_value < etpp_types->total_nof_bits) && (next_shift_value <= shift_value))
        {
            /** get the size of the field type */
            SHR_IF_ERR_EXIT(dnx_crps_src_interface_data_field_params_get
                            (unit, core_id, bcmStatCounterInterfaceEgressTransmitPp, obj_idx,
                             etpp_types->valid_types[type_idx], &nof_bits, NULL));
            /** next type, will be placed right after the last one inside the 24 bits metadata */
            next_shift_value = shift_value + nof_bits;
        }

        /** meta_data type gather all sub meta_datas per object_if, therefore, we will init all sub meta datas */
        /** T.B.D - change the value 6 to dnx_data */
        if ((etpp_types->valid_types[type_idx] == bcmStatExpansionTypeMetaData) && (obj_idx < 5))
        {
            obj_idx++;
        }
        else
        {
            type_idx++;
        }
    }

    *ret_val = next_shift_value;
exit:
    SHR_FUNC_EXIT;

}

/** see .h file */
shr_error_e
dnx_crps_src_interface_etpp_metadata_format_config(
    int unit,
    bcm_stat_counter_interface_key_t * interface,
    bcm_stat_expansion_select_t * expansion_select)
{
    uint32 shift_value;
    uint32 nof_bits;
    uint32 next_shift_value = 0;
    int obj_idx = 0;
    int elem_idx;
    const dnx_data_crps_expansion_etpp_types_t *etpp_types = dnx_data_crps.expansion.etpp_types_get(unit);
    dbal_enum_value_field_crps_etpp_counter_data_type_e dbal_type;
    uint32 set_val_arr[DNX_DATA_MAX_CRPS_EXPANSION_EXPANSION_SIZE];
    dbal_enum_value_field_crps_etpp_counter_data_type_e set_type_arr[10];
    int count = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** get the next free position in the metadata format */
    SHR_IF_ERR_EXIT(dnx_crps_src_interface_etpp_next_shift_val_calc(unit, interface->core_id, &next_shift_value));

    /** the object index that may be relevant  (if metaData was selected) is according to the interface that we want to updates */
    obj_idx = interface->command_id;

    /** set the shift values for the input expansion types */
    for (elem_idx = 0; elem_idx < expansion_select->nof_elements; elem_idx++)
    {
        /** if type already in the metadata formata ->continue */
        SHR_IF_ERR_EXIT(dnx_crps_src_interface_etpp_type_convert
                        (unit, expansion_select->expansion_elements[elem_idx].type, obj_idx, &dbal_type));

        SHR_IF_ERR_EXIT(dnx_crps_src_interface_etpp_metadata_type_shift_get
                        (unit, interface->core_id, dbal_type, &shift_value));

        if (shift_value < etpp_types->total_nof_bits)
        {
            continue;
        }

                /** get the  size of the type */
        SHR_IF_ERR_EXIT(dnx_crps_src_interface_data_field_params_get
                        (unit, interface->core_id, interface->interface_source, obj_idx,
                         expansion_select->expansion_elements[elem_idx].type, &nof_bits, NULL));
        /** check that the metadata format is not full */
        if ((next_shift_value + nof_bits) >= etpp_types->total_nof_bits)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Etpp metadata format is full. type=%d can't be added\n",
                         expansion_select->expansion_elements[elem_idx].type);
        }

        /** In order to avoid scenario of "metadata format full" after some of the types were already configure in HW,  */
        /** at this loop, we do not set the HW, but just set array with the relevant data. after finish to get the information for all types */
        /** without errors, we will set the hw in the next loop */
        set_val_arr[count] = next_shift_value;
        set_type_arr[count] = dbal_type;
        count++;

        /** determine the next shift value, for the next type */
        next_shift_value += nof_bits;
    }
    /** set the shift value for all types */
    while (count > 0)
    {
        count--;
        SHR_IF_ERR_EXIT(dnx_crps_src_interface_etpp_metadata_type_shift_set
                        (unit, interface->core_id, set_type_arr[count], set_val_arr[count]));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *      specific hw configuration that need to do if the source is ingress voq.
 * \param [in] unit -unit id
 * \param [in] core_id - core_id
 * \param [in] command_id -object id
 * \param [in] format -format of counter.
 * \return
 *    shr_error_e
 * \remark
 *   NONE
 * \see
 *   NONE
 */
static shr_error_e
dnx_crps_src_ingress_voq_hw_set(
    int unit,
    bcm_core_t core_id,
    int command_id,
    bcm_stat_counter_format_type_t format)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** 1. handle the adapter selection - is to count packet size or max queue size */

        /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_ITM_INTERFACE_CFG, &entry_handle_id));

        /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_IF_OBJECT_ID, command_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ITM_PKTSIZE_NOT_QSIZE_SEL, INST_SINGLE,
                                 (format != bcmStatCounterFormatMaxSize));

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see crps_src_interface.h file */
shr_error_e
dnx_crps_src_egress_queue_hw_set(
    int unit,
    bcm_core_t core_id,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

   /** enable or disable the counter pointer from EGQ. */
   /** table is per core and PP port, SDK configure all entries the same for a specific core */

        /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_ETM_INTERFACE_CFG, &entry_handle_id));

        /** Set Range Key Fields */
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
        /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_ETM_COUNTER_PTR, INST_SINGLE, enable);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_src_config_set(
    int unit,
    int core_id,
    bcm_stat_counter_interface_t * config)
{

    SHR_FUNC_INIT_VARS(unit);

    /** some sources required special configuration. If the source doesn't have special configuration, go out of the switch. No need "default" */
    switch (config->source)
    {
        case bcmStatCounterInterfaceIngressVoq:
            dnx_crps_src_ingress_voq_hw_set(unit, core_id, config->command_id, config->format_type);
            break;
        case bcmStatCounterInterfaceEgressReceiveQueue:
            dnx_crps_src_egress_queue_hw_set(unit, core_id, TRUE);
            break;
        default:
            break;
    }

/** exit: */
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_src_ingress_compensation_mask_hw_set(
    int unit,
    bcm_core_t core_id,
    int select,
    uint32 mask)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_INGRESS_COMPENSATION_MASKS, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting pointers value to receive the fields */
    switch (select)
    {
        case 0:
            /** IRPP mask 0 */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IRPP_STAT_HDR_CMP_MASK_0, INST_SINGLE, mask);
            break;
        case 1:
            /** IRPP mask 1 */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IRPP_STAT_HDR_CMP_MASK_1, INST_SINGLE, mask);
            break;
        case 2:
            /** ITM mask */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_STAT_HDR_CMP_MASK, INST_SINGLE, mask);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "invalid select for mask (value=%d) - valid 0 for IRPP-0, 1 for IRPP-1, 2 for ITM \n", select);
            break;
    }

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_src_ingress_compensation_mask_hw_get(
    int unit,
    bcm_core_t core_id,
    int select,
    uint32 *mask)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_INGRESS_COMPENSATION_MASKS, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting pointers value to receive the fields */
    switch (select)
    {
        case 0:
            /** IRPP mask 0 */
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IRPP_STAT_HDR_CMP_MASK_0, INST_SINGLE, mask);
            break;
        case 1:
            /** IRPP mask 1 */
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IRPP_STAT_HDR_CMP_MASK_1, INST_SINGLE, mask);
            break;
        case 2:
            /** ITM mask */
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TM_STAT_HDR_CMP_MASK, INST_SINGLE, mask);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "invalid select for mask (value=%d) - valid 0 for IRPP-0, 1 for IRPP-1, 2 for ITM \n", select);
            break;
    }
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_src_irpp_compensation_mask_select_set(
    int unit,
    bcm_core_t core_id,
    int command_id,
    uint8 select)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_IRPP_COMPENSATION_SELECT, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_COMMAND_ID, command_id);

    /** Setting pointers value to receive the fields */
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_PACKET_SIZE_SELECT, INST_SINGLE, select);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_src_irpp_compensation_mask_select_get(
    int unit,
    bcm_core_t core_id,
    int command_id,
    uint8 *select)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_IRPP_COMPENSATION_SELECT, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_COMMAND_ID, command_id);

    /** Setting pointers value to receive the fields */
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_PACKET_SIZE_SELECT, INST_SINGLE, select);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_src_interface_compensation_init(
    int unit)
{
    int core_idx;
    int irpp_mask_0 = 0, irpp_mask_1 = 1, itm_mask = 2;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_idx)
    {
        /**   bit-0 - masks Header-Truncate compensation
              bit 1 - masks IRPP-Delta compensation
              bit-2 - masks VOQ.Credit-Class compensation
              bit-3 - masks In-PP-Port compensation
              bit-4 - masks Header-Append-Ptr compensatio
            */
        SHR_IF_ERR_EXIT(dnx_crps_src_ingress_compensation_mask_hw_set
                        (unit, core_idx, irpp_mask_0, DNX_CRPS_INGRESS_COMPENSATION_HEADER_TRUNCATE_DISABLE));
        SHR_IF_ERR_EXIT(dnx_crps_src_ingress_compensation_mask_hw_set
                        (unit, core_idx, irpp_mask_1, DNX_CRPS_INGRESS_COMPENSATION_HEADER_TRUNCATE_ENABLE));
        SHR_IF_ERR_EXIT(dnx_crps_src_ingress_compensation_mask_hw_set
                        (unit, core_idx, itm_mask, DNX_CRPS_INGRESS_COMPENSATION_HEADER_TRUNCATE_ENABLE));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_ingress_pkt_size_adjust_select_set(
    int unit,
    uint32 flags,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_pkt_size_adjust_select_type_t select_type,
    int enable)
{
    int disable_truncate = 1;
    int enable_truncate = 0;
    int itm_mask = 2;
    SHR_FUNC_INIT_VARS(unit);

    if (key->source == bcmStatCounterInterfaceIngressVoq)
    {
        if (enable)
        {
            SHR_IF_ERR_EXIT(dnx_crps_src_ingress_compensation_mask_hw_set
                            (unit, key->core_id, itm_mask, DNX_CRPS_INGRESS_COMPENSATION_HEADER_TRUNCATE_ENABLE));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_crps_src_ingress_compensation_mask_hw_set
                            (unit, key->core_id, itm_mask, DNX_CRPS_INGRESS_COMPENSATION_HEADER_TRUNCATE_DISABLE));
        }
    }
    else if (key->source == bcmStatCounterInterfaceIngressReceivePp || key->source == bcmStatCounterInterfaceIngressOam)
    {
        /** in select -  1 = mask A = disable, 0 = mask B = enable */
        if (enable == TRUE)
        {
            SHR_IF_ERR_EXIT(dnx_crps_src_irpp_compensation_mask_select_set
                            (unit, key->core_id, key->command_id, enable_truncate));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_crps_src_irpp_compensation_mask_select_set
                            (unit, key->core_id, key->command_id, disable_truncate));
        }

    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "bcm_stat_pkt_size_adjust_select_set called with invalid source enum value");
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_ingress_pkt_size_adjust_select_get(
    int unit,
    uint32 flags,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_pkt_size_adjust_select_type_t select_type,
    int *enable)
{
    uint32 mask_get;
    uint8 mask_select;
    int enable_truncate = 0;
    int itm_mask = 2;
    SHR_FUNC_INIT_VARS(unit);

    if (key->source == bcmStatCounterInterfaceIngressVoq)
    {
        SHR_IF_ERR_EXIT(dnx_crps_src_ingress_compensation_mask_hw_get(unit, key->core_id, itm_mask, &mask_get));
        if (mask_get == DNX_CRPS_INGRESS_COMPENSATION_HEADER_TRUNCATE_ENABLE)
        {
            *enable = 1;
        }
        else
        {
            *enable = 0;
        }
    }
    else if (key->source == bcmStatCounterInterfaceIngressReceivePp || key->source == bcmStatCounterInterfaceIngressOam)
    {
        /** in select -  1 = mask A = disable, 0 = mask B = enable */
        SHR_IF_ERR_EXIT(dnx_crps_src_irpp_compensation_mask_select_get
                        (unit, key->core_id, key->command_id, &mask_select));
        if (mask_select == enable_truncate)
        {
            *enable = 1;
        }
        else
        {
            *enable = 0;
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "bcm_stat_pkt_size_adjust_select_get called with invalid source enum value");
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_src_interface_etpp_latency_ctr_set(
    int unit,
    int core_id,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_ETPP_LATENCY_CTR_SELECT, &entry_handle_id));
    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable);
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}
