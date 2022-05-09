/** \file crps_src_interface.c
 * 
 *  crps module gather all functionality that related to the counter/statisitc processor driver for DNX.
 *  crps_src_interface.c responsible of source interface/adapter configuration and other source configurations reltaed to CRPS.
 *  It is internal crps file . (do not have interfaces with other modules)
 * 
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_CNT

/*
 * Include files.
 * {
 */
#include <soc/sand/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/stat.h>
#include "crps_mgmt_internal.h"
#include <bcm_int/dnx/stat/crps/crps_src_interface.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_crps.h>
#include <bcm_int/dnx/stat/crps/crps_mgmt.h>
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
#define DNX_CRPS_SRC_INT_DYNAMIC_OBJ_X_DATA_DBAL_TYPE(_obj_idx) \
    (DBAL_ENUM_FVAL_CRPS_DYNAMIC_COUNTER_DATA_TYPE_TYPE_OBJ0_META_DATA + _obj_idx)

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
 *      convert expansion types from BCM enum to dbal enum
 * \param [in] unit -
 *     unit id
 * \param [in] expansion_type -
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
dnx_crps_src_interface_dynamic_expansion_type_convert(
    int unit,
    bcm_stat_expansion_types_t expansion_type,
    uint32 object_idx,
    dbal_enum_value_field_crps_dynamic_counter_data_type_e * dbal_type)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (expansion_type)
    {
        case bcmStatExpansionTypeDropPrecedenceMeterResolved:
            *dbal_type = DBAL_ENUM_FVAL_CRPS_DYNAMIC_COUNTER_DATA_TYPE_TYPE_DP;
            break;
        case bcmStatExpansionTypeTrafficClass:
            *dbal_type = DBAL_ENUM_FVAL_CRPS_DYNAMIC_COUNTER_DATA_TYPE_TYPE_TC;
            break;
        case bcmStatExpansionTypeSystemMultiCast:
            *dbal_type = DBAL_ENUM_FVAL_CRPS_DYNAMIC_COUNTER_DATA_TYPE_TYPE_IS_MC;
            break;
        case bcmStatExpansionTypeEcnEligibleAndCni:
            *dbal_type = DBAL_ENUM_FVAL_CRPS_DYNAMIC_COUNTER_DATA_TYPE_TYPE_ECN_CNI;
            break;
        case bcmStatExpansionTypePortMetaData:
            *dbal_type = DBAL_ENUM_FVAL_CRPS_DYNAMIC_COUNTER_DATA_TYPE_TYPE_PORT_DATA;
            break;
        case bcmStatExpansionTypePpDropReason:
            *dbal_type = DBAL_ENUM_FVAL_CRPS_DYNAMIC_COUNTER_DATA_TYPE_TYPE_PP_DROP_REASON;
            break;
        case bcmStatExpansionTypePortRateBin:
            *dbal_type = DBAL_ENUM_FVAL_CRPS_DYNAMIC_COUNTER_DATA_TYPE_TYPE_PORT_RATE_BIN;
            break;
        case bcmStatExpansionTypeMetaData:
            /** assuming enum values for Obj[x]_meta_data are consecutive. so we take the base val and add the object_idx */
            *dbal_type = DNX_CRPS_SRC_INT_DYNAMIC_OBJ_X_DATA_DBAL_TYPE(object_idx);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, " invalid bcm_type. type=%d\n", expansion_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *      get the etpp/epp meta data format configuration. each API call agive back position of one type
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
dnx_crps_src_interface_epp_dynamic_metadata_type_shift_get(
    int unit,
    bcm_core_t core_id,
    dbal_enum_value_field_crps_dynamic_counter_data_type_e type,
    uint32 *shift_val)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_EPP_COUNTER_DATA_SELECTION, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_DYNAMIC_COUNTER_DATA_TYPE, type);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SHIFT_VALUE, INST_SINGLE, shift_val);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_crps_src_interface_dynamic_metadata_type_shift_get(
    int unit,
    bcm_core_t core_id,
    bcm_stat_expansion_types_t expansion_type,
    int obj_idx,
    uint32 *shift_val)
{
    dbal_enum_value_field_crps_dynamic_counter_data_type_e dbal_type;
    uint32 get_value;
    SHR_FUNC_INIT_VARS(unit);

    /** convert the expansion type to dbal type*/
    SHR_IF_ERR_EXIT(dnx_crps_src_interface_dynamic_expansion_type_convert(unit, expansion_type, obj_idx, &dbal_type));

    /** get shift value */
    SHR_IF_ERR_EXIT(dnx_crps_src_interface_epp_dynamic_metadata_type_shift_get(unit, core_id, dbal_type, &get_value));

    *shift_val = get_value;

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_src_interface_expansion_type_offset_get(
    int unit,
    int core_id,
    bcm_stat_counter_interface_type_t src,
    int command_id,
    bcm_stat_expansion_types_t field_type,
    uint32 *offset)
{
    int tmp_offset;
    SHR_FUNC_INIT_VARS(unit);

    tmp_offset = dnx_data_crps.expansion.source_metadata_types_get(unit, src, field_type)->offset;

    if (tmp_offset >= 0)
    {
        *offset = tmp_offset;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_crps_src_interface_dynamic_metadata_type_shift_get
                        (unit, core_id, field_type, command_id, offset));
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
shr_error_e
dnx_crps_src_interface_expansion_select_calc(
    int unit,
    int core_id,
    bcm_stat_counter_interface_type_t src,
    int command_id,
    bcm_stat_expansion_select_t * expansion_select,
    uint32 *ret_val)
{
    int elem_index, nof_bits;
    uint32 offset;
    SHR_FUNC_INIT_VARS(unit);

    /** build the selection bit map */
    for (elem_index = 0; elem_index < expansion_select->nof_elements; elem_index++)
    {
        /** get expansion element type number of bits */
        nof_bits =
            dnx_data_crps.expansion.source_metadata_types_get(unit, src,
                                                              expansion_select->expansion_elements[elem_index].
                                                              type)->nof_bits;

        /** get expansion element type offset */
        SHR_IF_ERR_EXIT(dnx_crps_src_interface_expansion_type_offset_get
                        (unit, core_id, src, command_id, expansion_select->expansion_elements[elem_index].type,
                         &offset));

        /** for need to copy the set the bits according to the bitmap. */
        SHR_BITCOPY_RANGE(ret_val, offset, &expansion_select->expansion_elements[elem_index].bitmap, 0, nof_bits);
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_src_interface_epp_dynamic_expansion_select_set(
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
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_EPP_DYNAMIC_INTERFACE_EXPANSION_CFG, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_IF_OBJECT_ID, interface->command_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, interface->core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_IF_TYPE_ID, interface->type_id);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_CTR_EXPANSION_SELECT, INST_SINGLE,
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
    uint32 *src_bitmap,
    uint32 *dest_bitmap,
    int *is_selected)
{
    uint32 nof_bits, offset, field_val = 0;
    SHR_FUNC_INIT_VARS(unit);

    nof_bits = dnx_data_crps.expansion.source_metadata_types_get(unit, src, field_type)->nof_bits;

    SHR_IF_ERR_EXIT(dnx_crps_src_interface_expansion_type_offset_get
                    (unit, core_id, src, command_id, field_type, &offset));

    SHR_BITCOPY_RANGE(&field_val, 0, src_bitmap, offset, nof_bits);
    *is_selected = (field_val) ? TRUE : FALSE;

    /** if metadata, need to update the metadata bit map */
    if (*is_selected == TRUE)
    {
        *dest_bitmap = field_val;
    }

exit:
    SHR_FUNC_EXIT;

}

/** see .h file */
shr_error_e
dnx_crps_src_interface_expansion_bitmap_convert(
    int unit,
    int core_id,
    bcm_stat_counter_interface_type_t src,
    int command_id,
    uint32 *selection_bitmap,
    bcm_stat_expansion_select_t * expansion_select)
{
    int is_selected = FALSE;
    bcm_stat_expansion_types_t expansion_element;
    SHR_FUNC_INIT_VARS(unit);

    /** fill the expansion elements array with all the valid elements that have at least one bit set in the range represents it in the bitmap */
    expansion_select->nof_elements = 0;
    for (expansion_element = bcmStatExpansionTypeTmDropReason; expansion_element < bcmStatExpansionTypeMax;
         expansion_element++)
    {
        if (dnx_data_crps.expansion.source_metadata_types_get(unit, src, expansion_element)->nof_bits > 0)
        {
            SHR_IF_ERR_EXIT(dnx_crps_src_interface_expansion_field_is_selected
                            (unit, core_id, src, command_id, expansion_element, selection_bitmap,
                             &expansion_select->expansion_elements[expansion_select->nof_elements].bitmap,
                             &is_selected));

            if (is_selected)
            {
                expansion_select->expansion_elements[expansion_select->nof_elements].type = expansion_element;
                expansion_select->nof_elements++;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_mgmt_counter_expansion_src_table_get(
    int unit,
    bcm_stat_counter_interface_type_t src,
    dbal_tables_e * dbal_table)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (src)
    {
        case bcmStatCounterInterfaceIngressReceivePp:
            *dbal_table = DBAL_TABLE_CRPS_IRPP_INTERFACE_EXPANSION_CFG;
            break;
        case bcmStatCounterInterfaceIngressTransmitPp:
            *dbal_table = DBAL_TABLE_CRPS_ITPP_INTERFACE_EXPANSION_CFG;
            break;
        case bcmStatCounterInterfaceEgressReceivePp:
            *dbal_table = DBAL_TABLE_CRPS_ERPP_INTERFACE_EXPANSION_CFG;
            break;
        case bcmStatCounterInterfaceEgressTransmitPp:
        case bcmStatCounterInterfaceEgressPp:
            *dbal_table = DBAL_TABLE_CRPS_EPP_DYNAMIC_INTERFACE_EXPANSION_CFG;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, " invalid interface source (=%d)\n", src);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_src_interface_expansion_select_hw_get(
    int unit,
    bcm_stat_counter_interface_key_t * interface,
    uint32 *val_arr)
{
    uint32 entry_handle_id;
    dbal_tables_e dbal_table;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_expansion_src_table_get(unit, interface->interface_source, &dbal_table));

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, interface->core_id);

    if (dnx_data_crps.expansion.
        source_expansion_info_get(unit, interface->interface_source)->supports_expansion_per_type)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_IF_TYPE_ID, interface->type_id);
    }

    if (dnx_data_crps.src_interface.command_id_get(unit, interface->interface_source)->size > 1)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_IF_OBJECT_ID, interface->command_id);
    }

    /** Setting pointers value to receive the fields */
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_CRPS_CTR_EXPANSION_SELECT, INST_SINGLE, val_arr);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_src_interface_expansion_select_hw_set(
    int unit,
    bcm_stat_counter_interface_key_t * interface,
    uint32 *selection)
{

    dbal_tables_e dbal_table;
    uint32 entry_handle_id;
    int field_size;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** get the table holds the expansion configuration for the current source */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_expansion_src_table_get(unit, interface->interface_source, &dbal_table));

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, interface->core_id);

    if (dnx_data_crps.expansion.
        source_expansion_info_get(unit, interface->interface_source)->supports_expansion_per_type)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_IF_TYPE_ID, interface->type_id);
    }

    if (dnx_data_crps.src_interface.command_id_get(unit, interface->interface_source)->size > 1)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_IF_OBJECT_ID, interface->command_id);
    }

    /** Setting pointers value to receive the fields */
    SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                    (unit, dbal_table, DBAL_FIELD_CRPS_CTR_EXPANSION_SELECT, FALSE, 0, 0, &field_size));

    if (field_size > 32)
    {
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_CTR_EXPANSION_SELECT, INST_SINGLE,
                                         selection);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_CTR_EXPANSION_SELECT, INST_SINGLE,
                                     *selection);
    }

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
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
dnx_crps_src_interface_epp_dynamic_metadata_type_shift_set(
    int unit,
    bcm_core_t core_id,
    dbal_enum_value_field_crps_dynamic_counter_data_type_e type,
    uint32 shift_val)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

        /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_EPP_COUNTER_DATA_SELECTION, &entry_handle_id));

        /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_DYNAMIC_COUNTER_DATA_TYPE, type);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SHIFT_VALUE, INST_SINGLE, shift_val);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *      set the etpp/epp meta data format configuration. each API call add/remove one data type to the format configuration.
 * \param [in] unit -
 *     unit id
 * \param [in] core_id -
 *     core id
 * \param [in] expansion_type -
 *     The expansion type that need to add/remove to/from the format configuration
 * \param [in] obj_idx -
 *     Command id for bcmStatExpansionTypeMetaData
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
dnx_crps_src_interface_dynamic_metadata_type_shift_set(
    int unit,
    bcm_core_t core_id,
    bcm_stat_expansion_types_t expansion_type,
    uint32 obj_idx,
    uint32 shift_val)
{
    dbal_enum_value_field_crps_dynamic_counter_data_type_e dbal_type;
    SHR_FUNC_INIT_VARS(unit);

    /** convert the expansion type to dbal type*/
    SHR_IF_ERR_EXIT(dnx_crps_src_interface_dynamic_expansion_type_convert(unit, expansion_type, obj_idx, &dbal_type));

    /** set shift value*/
    SHR_IF_ERR_EXIT(dnx_crps_src_interface_epp_dynamic_metadata_type_shift_set(unit, core_id, dbal_type, shift_val));

exit:
    SHR_FUNC_EXIT;

}

/** see .h file */
shr_error_e
dnx_crps_src_interface_dynamic_meta_data_selection_init(
    int unit)
{
    int core_idx;
    bcm_stat_expansion_types_t expansion_element;
    int nof_bits, total_nof_bits, obj_idx, nof_objects;
    bcm_stat_counter_interface_type_t dynamic_metadata_src;
    SHR_FUNC_INIT_VARS(unit);

    /** get the interface source has the dynamic meta data */
    dynamic_metadata_src =
        dnx_data_crps.src_interface.command_id_get(unit,
                                                   bcmStatCounterInterfaceEgressTransmitPp)->size >
        0 ? bcmStatCounterInterfaceEgressTransmitPp : bcmStatCounterInterfaceEgressPp;

    /** get the max shift value */
    total_nof_bits = dnx_data_crps.expansion.source_expansion_info_get(unit, dynamic_metadata_src)->expansion_size;

    /** get the number of objects (commands)  ids*/
    nof_objects = dnx_data_crps.src_interface.command_id_get(unit, dynamic_metadata_src)->size;

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_idx)
    {
        obj_idx = 0;
        /** first expansion element */
        expansion_element = bcmStatExpansionTypeTmDropReason;

        /** running on all etpp/epp valid types and give them invalid shift. */
        /** valid_type array ends with value "bcmStatExpansionTypeMax", which is the loop stop condition */
        while (expansion_element != bcmStatExpansionTypeMax)
        {
            /** the expansion type supported for the dynamic source */
            nof_bits =
                dnx_data_crps.expansion.source_metadata_types_get(unit, dynamic_metadata_src,
                                                                  expansion_element)->nof_bits;
            if (nof_bits > 0)
            {
                SHR_IF_ERR_EXIT(dnx_crps_src_interface_dynamic_metadata_type_shift_set
                                (unit, core_idx, expansion_element, obj_idx, total_nof_bits));

            }

            /** meta_data type gather all sub meta_datas per object_if, therefore, we will init all sub metadata */
            /** T.B.D - change the value 6 to dnx_data */
            if ((expansion_element == bcmStatExpansionTypeMetaData) && (obj_idx < nof_objects - 1))
            {
                obj_idx++;
            }
            else
            {
                expansion_element++;
                obj_idx = 0;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_crps_src_interface_dynamic_metadata_next_shift_get(
    int unit,
    int core_id,
    bcm_stat_counter_interface_type_t src,
    uint32 *next_free_shift)
{
    bcm_stat_expansion_types_t element_type;
    uint32 max_shift = 0, current_element_shift = 0, last_element_size = 0, metadata_size = 0;
    int obj_idx = 0, nof_obj_per_type = 1;
    int current_element_size = -1;

    SHR_FUNC_INIT_VARS(unit);

    metadata_size = dnx_data_crps.expansion.source_expansion_info_get(unit, src)->expansion_size;

    for (element_type = bcmStatExpansionTypeTmDropReason; element_type < bcmStatExpansionTypeMax; element_type++)
    {
        current_element_size = dnx_data_crps.expansion.source_metadata_types_get(unit, src, element_type)->nof_bits;

        /*
         * The type dosn't supported for the current dynamic interface source 
         */
        if (current_element_size < 0)
        {
            continue;
        }

        nof_obj_per_type =
            element_type == bcmStatExpansionTypeMetaData ? dnx_data_crps.src_interface.command_id_get(unit,
                                                                                                      src)->size : 1;

        for (obj_idx = 0; obj_idx < nof_obj_per_type; obj_idx++)
        {

            SHR_IF_ERR_EXIT(dnx_crps_src_interface_dynamic_metadata_type_shift_get(unit, core_id, element_type, obj_idx /* for 
                                                                                                                         * TypeMetaData 
                                                                                                                         * only */ ,
                                                                                   &current_element_shift));

           /** The element shift didn't set*/
            if (current_element_shift == metadata_size)
            {
                continue;
            }
           /** the current element has the biggest shift until now */
            if (current_element_shift >= max_shift)
            {
                max_shift = current_element_shift;
                last_element_size = current_element_size;
            }
        }
    }

    *next_free_shift = max_shift + last_element_size;

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_src_interface_dynamic_metadata_format_config(
    int unit,
    bcm_stat_counter_interface_key_t * interface,
    bcm_stat_expansion_select_t * expansion_select)
{
    uint32 shift_value_get = 0, shift_value = 0;
    int nof_bits;
    int obj_idx = 0;
    int elem_idx;
    int metadata_size;
    SHR_FUNC_INIT_VARS(unit);

    /** get shift max value */
    metadata_size =
        dnx_data_crps.expansion.source_expansion_info_get(unit, interface->interface_source)->expansion_size;

    /** the object index that may be relevant  (if metaData was selected) is according to the interface that we want to updates */
    obj_idx = interface->command_id;

    SHR_IF_ERR_EXIT(dnx_crps_src_interface_dynamic_metadata_next_shift_get
                    (unit, interface->core_id, interface->interface_source, &shift_value));

    /** set the shift values for the input expansion types */
    for (elem_idx = 0; elem_idx < expansion_select->nof_elements; elem_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_crps_src_interface_dynamic_metadata_type_shift_get
                        (unit, interface->core_id, expansion_select->expansion_elements[elem_idx].type, obj_idx,
                         &shift_value_get));

        /** skip elements that already exist in the meta data */
        if (shift_value_get < metadata_size)
        {
            continue;
        }

        SHR_BITCOUNT_RANGE(&expansion_select->expansion_elements[elem_idx].bitmap, nof_bits, 0, (sizeof(uint32) * 8));

        /**The total metadata is too big */
        if (shift_value + nof_bits >= metadata_size)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "source=%d metadata must be smaller than %d ",
                         interface->interface_source, metadata_size);
        }

        SHR_IF_ERR_EXIT(dnx_crps_src_interface_dynamic_metadata_type_shift_set
                        (unit, interface->core_id, expansion_select->expansion_elements[elem_idx].type, obj_idx,
                         shift_value));

        shift_value += nof_bits;

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

            if (dnx_data_crps.src_interface.feature_get(unit,
                                                        dnx_data_crps_src_interface_etm_count_pp_port_enable_table_is_supported))
            {
                dnx_crps_src_egress_queue_hw_set(unit, core_id, TRUE);
            }

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
