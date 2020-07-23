/** \file field_map.c
 * 
 *
 * Field database procedures for DNX.
 *
 * This file implements mapping from BCM to DNX qualifiers and access for per DNX qualifier/action information
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX
/*
 * Include files.
 * {
 */

#include <shared/utilex/utilex_rhlist.h>
#include <shared/shrextend/shrextend_debug.h>

#include <soc/dnx/dbal/dbal_string_mgmt.h>
#include <soc/sand/sand_signals.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>

#include <soc/dnx/swstate/auto_generated/access/dnx_field_qualifier_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_actions_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_apptype_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_range_access.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_context_types.h>

#include <bcm_int/dnx/algo/swstate/auto_generated/types/dnx_algo_field_types.h>

#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_presel.h>
#include <bcm_int/dnx/field/field_context.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <bcm_int/dnx/field/tcam/tcam_handler.h>

#include <bcm/field.h>
#include <bcm/switch.h>

#include "field_map_local.h"

#include <include/soc/dnx/pemladrv/pemladrv_virtual_wires.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
#include <soc/dnx/dbal/dbal_dynamic.h>

#include "auto_generated/dbx_pre_compiled_fwd_app_type_auto_generated_xml_parser.h"

/*
 * }Include files
 */

/*
 * See field_map.h
 */
char *
dnx_field_stage_text(
    int unit,
    dnx_field_stage_e stage)
{
    char *stage_n;
    dnx_pp_stage_e pp_stage;

    if (stage == DNX_FIELD_STAGE_INVALID)
    {   /** Intentionally use INVALID(-1) as designation for global objects */
        stage_n = "Global";
    }
    else if (((int) stage < DNX_FIELD_STAGE_FIRST) || (stage >= DNX_FIELD_STAGE_NOF))
    {
        stage_n = "Invalid Stage";
    }
    else if (stage == DNX_FIELD_STAGE_EXTERNAL)
    {
        /**
         * Adding this exception, because the pp_stage name 'FWD2' is different than
         * field stage name "EXTERNAL".
         */
        stage_n = "EXTERNAL";
    }
    else if (stage == DNX_FIELD_STAGE_L4_OPS)
    {
        /**
         * Adding this exception, because no pp_stage is supplied for L4_OPS stage.
         * field stage name "L4_OPS".
         */
        stage_n = "L4_OPS";
    }
    else if ((pp_stage = dnx_field_map_stage_info[stage].pp_stage) == DNX_PP_STAGE_INVALID)
    {
        stage_n = "Unnamed Stage";
    }
    else
    {
        /*
         * Temporary keep pp_stage in map_stage_info and use it to fetch dnx_data
         * will be obsolete after replacement of dnx_field_stage by dnx_pp-stage
         */
        stage_n = dnx_pp_stage_name(unit, pp_stage);
    }

    return stage_n;
}

/*
 * See field_map.h
 */
char *
dnx_field_bcm_stage_text(
    bcm_field_stage_t bcm_stage)
{
    char *stage_n;
    char *bcm_stage_text[bcmFieldStageCount] = BCM_FIELD_STAGE_STRINGS;

    if ((bcm_stage < bcmFieldStageFirst) || (bcm_stage >= bcmFieldStageCount))
    {
        stage_n = "Invalid Stage";
    }
    else
    {
        stage_n = bcm_stage_text[bcm_stage];
    }
    return stage_n;
}

/*
 * See field_map.h
 */
char *
dnx_field_bcm_layer_type_text(
    bcm_field_layer_type_t bcm_layer_type)
{
    char *layer_type_n;
    char *bcm_layer_type_text[bcmFieldLayerTypeCount] = BCM_FIELD_LAYER_TYPE_STRINGS;

    if ((bcm_layer_type < bcmFieldLayerTypeFirst) || (bcm_layer_type >= bcmFieldLayerTypeCount))
    {
        layer_type_n = "Invalid Layer";
    }
    else
    {
        layer_type_n = bcm_layer_type_text[bcm_layer_type];
    }
    return layer_type_n;
}

/*
 * See field_map.h
 */
char *
dnx_field_input_type_text(
    dnx_field_input_type_e input_type)
{
    char *input_type_name;
    if ((input_type < DNX_FIELD_INPUT_TYPE_LAYER_FWD) || (input_type >= DNX_FIELD_INPUT_TYPE_NOF))
    {
        input_type_name = "Invalid Input Type";
    }
    else if (dnx_field_input_types_names[input_type] == NULL)
    {
        input_type_name = "Unnamed Input Type";
    }
    else
    {
        input_type_name = (char *) dnx_field_input_types_names[input_type];
    }

    return input_type_name;
}

/*
 * See field_map.h
 */
char *
dnx_field_bcm_input_type_text(
    bcm_field_input_types_t bcm_input_type)
{
    char *input_type_name;
    char *bcm_input_type_text[bcmFieldInputTypeCount] = BCM_FIELD_INPUT_TYPES_STRINGS;

    if ((bcm_input_type < bcmFieldInputTypeLayerFwd) || (bcm_input_type >= bcmFieldInputTypeCount))
    {
        input_type_name = "Invalid Input Type";
    }
    else
    {
        input_type_name = bcm_input_type_text[bcm_input_type];
    }
    return input_type_name;
}

/*
 * See field_map.h
 */
char *
dnx_field_bcm_packet_remove_layer_text(
    bcm_field_packet_remove_layers_t bcm_packet_remove_layer)
{
    char *remove_layer_name;
    char *bcm_remove_layer_text[bcmFieldPacketRemoveLayerCount] = BCM_FIELD_PACKET_REMOVE_LAYER_STRINGS;

    if ((bcm_packet_remove_layer < bcmFieldPacketRemoveLayerOffset0)
        || (bcm_packet_remove_layer >= bcmFieldPacketRemoveLayerCount))
    {
        remove_layer_name = "Invalid Packet Remove Layer";
    }
    else
    {
        remove_layer_name = bcm_remove_layer_text[bcm_packet_remove_layer];
    }
    return remove_layer_name;
}

/*
 * See field_map.h
 */
char *
dnx_field_bcm_tcam_bank_allocation_mode_text(
    bcm_field_tcam_bank_allocation_mode_t bcm_tcam_bank_allocation_mode)
{
    char *tcam_bank_allocation_mode_name;
    /** The array size should be +1, because of the invalid value, which we have in the strings. */
    char *bcm_tcam_bank_allocation_mode_text[bcmFieldTcamBankAllocationModeCount] =
        BCM_FIELD_TCAM_BANK_ALLOCATION_MODE_STRINGS;

    if ((bcm_tcam_bank_allocation_mode < bcmFieldTcamBankAllocationModeAuto)
        || (bcm_tcam_bank_allocation_mode >= bcmFieldTcamBankAllocationModeCount))
    {
        tcam_bank_allocation_mode_name = "Invalid Tcam Bank Allocation Mode";
    }
    else
    {
        tcam_bank_allocation_mode_name = bcm_tcam_bank_allocation_mode_text[bcm_tcam_bank_allocation_mode];
    }
    return tcam_bank_allocation_mode_name;
}

/*
 * See field_map.h
 */
char *
dnx_field_bcm_apptype_text(
    bcm_field_AppType_t bcm_apptype)
{
    char *apptype_name;
    char *bcm_apptype_text[bcmFieldAppTypeCount] = BCM_FIELD_APPTYPE_STRINGS;

    if ((bcm_apptype < bcmFieldAppTypeAny) || (bcm_apptype >= bcmFieldAppTypeCount))
    {
        apptype_name = "Invalid Apptype";
    }
    else
    {
        apptype_name = bcm_apptype_text[bcm_apptype];
    }
    return apptype_name;
}

/*
 * See field_map.h
 */
char *
dnx_field_bcm_range_type_text(
    bcm_field_range_type_t bcm_rangetype)
{
    char *rangetype_name;
    char *bcm_rangetype_text[bcmFieldRangeTypeCount] = BCM_FIELD_RANGE_TYPE_STR;

    if ((bcm_rangetype < bcmFieldRangeTypeL4SrcPort) || (bcm_rangetype >= bcmFieldRangeTypeCount))
    {
        rangetype_name = "Invalid RangeType";
    }
    else
    {
        rangetype_name = bcm_rangetype_text[bcm_rangetype];
    }
    return rangetype_name;
}

/*
 * See field_map.h
 */
char *
dnx_field_bcm_range_result_map_text(
    bcm_switch_range_result_map_t bcm_rangeresultmap)
{
    char *rangeresultmap_name;
    char *bcm_rangeresultmap_text[bcmSwitchRangeResultMapCount] = BCM_SWITCH_RANGE_RESULT_MAP_STR;

    if ((bcm_rangeresultmap < bcmSwitchRangeResultMapNone) || (bcm_rangeresultmap >= bcmSwitchRangeResultMapCount))
    {
        rangeresultmap_name = "Invalid RangeType";
    }
    else
    {
        rangeresultmap_name = bcm_rangeresultmap_text[bcm_rangeresultmap];
    }
    return rangeresultmap_name;
}

/*
 * See field_map.h
 */
char *
dnx_field_bcm_range_operator_text(
    bcm_switch_range_operator_t bcm_rangeoperator)
{
    char *rangeoperator_name;
    char *bcm_rangeoperator_text[bcmSwitchRangeOperatorCount] = BCM_SWITCH_RANGE_OPERATOR_STR;

    if ((bcm_rangeoperator < bcmSwitchRangeOperatorNone) || (bcm_rangeoperator >= bcmSwitchRangeOperatorCount))
    {
        rangeoperator_name = "Invalid RangeOperator";
    }
    else
    {
        rangeoperator_name = bcm_rangeoperator_text[bcm_rangeoperator];
    }
    return rangeoperator_name;
}

/**
* \brief
*  Verifies if a signal can function as a qualifier for special cases that require extra logic.
*  Used for general data.
* \param [in] unit              - Device ID.
* \param [in] stage              - Stage
* \param [in] signal_name         - The name of the signal the VW is mapped to.
* \param [in] lsb_on_signal        - The lsb on the signal.
* \param [in] nof_bits_on_signal   - The number of bits on the signal.
*                                    Only used for checking if the part of the general data signal is available.
* \param [out] is_legal_special_p  - Whether the signal can be read as qualifier, from the point of view of
*                                    signal special reuqirements.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_qual_vw_signal_special_verify(
    int unit,
    dnx_field_stage_e stage,
    char *signal_name,
    int lsb_on_signal,
    int nof_bits_on_signal,
    int *is_legal_special_p)
{
    int user_general_data_size = dnx_data_field.virtual_wire.general_data_user_general_containers_size_get(unit);
    const dnx_data_field_virtual_wire_signal_mapping_t *dnx_data_signals_struct_p;
    char general_data_signal_name[] = "general_data";

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(signal_name, _SHR_E_PARAM, "signal_name");
    SHR_NULL_CHECK(is_legal_special_p, _SHR_E_PARAM, "is_legal_special_p");

    if (nof_bits_on_signal <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "nof_bits_on_signal (%d) must be at least 1.\r\n", nof_bits_on_signal);
    }

    (*is_legal_special_p) = TRUE;

    /*
     * General data gets special treatment, as only parts of the signal are accessible to the PMF.
     */
    if (0 == sal_strncmp(signal_name, general_data_signal_name, MAX_VW_NAME_LENGTH))
    {
        if ((stage == DNX_FIELD_STAGE_EXTERNAL) || (stage == DNX_FIELD_STAGE_IPMF1))
        {
            /*
             * For external and iPMF1, we only have the user_general_containers available.
             */
            if (lsb_on_signal + nof_bits_on_signal > user_general_data_size)
            {
                LOG_DEBUG_EX(BSL_LOG_MODULE, "only the first %d bits of general data available on stage \"%s\", "
                             "requested lsb (%d) plus size (%d).\n",
                             user_general_data_size, dnx_field_dnx_qual_text(unit, stage),
                             lsb_on_signal, nof_bits_on_signal);
                (*is_legal_special_p) = FALSE;
            }
        }
        else if (stage == DNX_FIELD_STAGE_IPMF3)
        {
            /*
             * For iPMF3, we only have what is writable in iPMF1/2 availabe.
             */
            int nof_bits_on_signal_remaining = nof_bits_on_signal;
            int action_index;
            int max_nof_actions = dnx_data_field.virtual_wire.actions_per_signal_nof_get(unit);
            int action_id;
            int action_lsb_on_signal;
            unsigned int action_size;
            dnx_field_action_t dnx_action;
            int overlapping_bits;

            dnx_data_signals_struct_p =
                dnx_data_field.virtual_wire.signal_mapping_get(unit, DNX_FIELD_STAGE_IPMF1,
                                                               dnx_data_field.virtual_wire.ipmf1_general_data_index_get
                                                               (unit));
            /*
             * Go over all actions in iPMF1 and remove overlapping bits.
             */
            for (action_index = 0; action_index < max_nof_actions; action_index++)
            {
                action_id = dnx_data_signals_struct_p->mapped_action[action_index];
                if (action_id <= 0)
                {
                    break;
                }
                /*
                 * We call dnx_field_map_dnx_action_size, even though we are still in init,
                 * but action init has already taken place.
                 */
                dnx_action = DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF1, action_id);
                SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size(unit, DNX_FIELD_STAGE_IPMF1, dnx_action, &action_size));
                if (action_size <= 0)
                {
                    /** Note we can continue if (action_size == 0), but such a scenario shouldn't occur.*/
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Non positive action size %d for action %d of general data.\r\n",
                                 action_size, action_index);
                }
                action_lsb_on_signal = dnx_data_signals_struct_p->action_offset[action_index];
                if (action_lsb_on_signal < 0)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Negative offset on signal %d for action %d of general data.\r\n",
                                 action_lsb_on_signal, action_index);
                }

                overlapping_bits = MIN(lsb_on_signal + nof_bits_on_signal, action_lsb_on_signal + action_size) -
                    MAX(lsb_on_signal, action_lsb_on_signal);
                if (overlapping_bits > 0)
                {
                    nof_bits_on_signal_remaining -= overlapping_bits;
                }
            }
            if (nof_bits_on_signal_remaining < 0)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Negative remaining bits %d.\r\n", nof_bits_on_signal_remaining);
            }
            if (nof_bits_on_signal_remaining > 0)
            {
                LOG_DEBUG_EX(BSL_LOG_MODULE, "In stage \"%s\" for general data, "
                             "requested lsb (%d) plus size (%d) not supported.\n%s",
                             dnx_field_dnx_qual_text(unit, stage), lsb_on_signal, nof_bits_on_signal, EMPTY);
                (*is_legal_special_p) = FALSE;
            }
        }
        else
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE, "Unsupported stage \"%s\" (%d) for general data.\n%s%s",
                         dnx_field_dnx_qual_text(unit, stage), stage, EMPTY, EMPTY);
            (*is_legal_special_p) = FALSE;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Gets the actions and offsets that write to a signal.
* \param [in] unit - Device ID.
* \param [in] stage - Stage
* \param [in] signal_name - The name of the signal the VW is mapped to.
* \param [out] nof_actions_p - Number of actions mapped to the signal.
*                              Number of elements in dnx_actions and offsets.
* \param [out] action_dbal_id - The list of action ids used by the signal, in the form of the DBAL enum relevant
*                               to the stage (e.g. dbal_enum_value_field_ipmf1_action_e).
* \param [out] offsets- The offsets on the signal of the actions
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_action_vw_signal_actions_offsets(
    int unit,
    dnx_field_stage_e stage,
    char *signal_name,
    int *nof_actions_p,
    int action_dbal_id[DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF],
    int offsets[DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF])
{
    int signal_index;
    int signals_nof = dnx_data_field.virtual_wire.signals_nof_get(unit);
    int action_index;
    int actions_nof = dnx_data_field.virtual_wire.actions_per_signal_nof_get(unit);
    int found;
    const dnx_data_field_virtual_wire_signal_mapping_t *dnx_data_signals_struct_p;
    /*
     * We assume that for each DBAL enum for actions, 0 is the empty value (like DBAL_ENUM_FVAL_IPMF1_ACTION_EMPTY).
     * 0 is the default value in the DNX DATA array.
     */
    int enum_empty_value = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(signal_name, _SHR_E_PARAM, "signal_name");
    SHR_NULL_CHECK(nof_actions_p, _SHR_E_PARAM, "nof_actions_p");
    SHR_NULL_CHECK(action_dbal_id, _SHR_E_PARAM, "dnx_actions");
    SHR_NULL_CHECK(offsets, _SHR_E_PARAM, "offsets");

    /*
     * Initialize the output arrays.
     */
    for (action_index = 0; action_index < DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF; action_index++)
    {
        action_dbal_id[action_index] = enum_empty_value;
        offsets[action_index] = 0;
    }

    /*
     * Find the signal.
     */
    found = FALSE;
    for (signal_index = 0; signal_index < signals_nof; signal_index++)
    {
        dnx_data_signals_struct_p = dnx_data_field.virtual_wire.signal_mapping_get(unit, stage, signal_index);
        /*
         * We assume that for each DBAL enum for actions, 0 is the empty value (like DBAL_ENUM_FVAL_IPMF1_ACTION_EMPTY).
         */
        if ((dnx_data_signals_struct_p->mapped_action[0] != enum_empty_value) &&
            (0 == sal_strncmp(signal_name, dnx_data_signals_struct_p->signal_name, MAX_VW_NAME_LENGTH)))
        {
            found = TRUE;
            break;
        }
    }

    /*
     * List the actions and offsets.
     */
    (*nof_actions_p) = 0;
    if (found)
    {
        for (action_index = 0; action_index < actions_nof; action_index++)
        {
            if (dnx_data_signals_struct_p->mapped_action[action_index] == enum_empty_value)
            {
                break;
            }
            action_dbal_id[action_index] = dnx_data_signals_struct_p->mapped_action[action_index];
            offsets[action_index] = dnx_data_signals_struct_p->action_offset[action_index];
            (*nof_actions_p)++;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See field_map.h
 */
shr_error_e
dnx_field_action_vw_single_mapping_actions_offsets(
    int unit,
    dnx_field_stage_e stage,
    VirtualWireMappingInfo * mapping_info_p,
    int *nof_actions_p,
    dnx_field_action_t dnx_actions[DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF],
    int offsets[DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF],
    int action_sizes[DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF])
{
    int nof_actions_signal;
    int action_dbal_id_signal[DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF];
    int offsets_signal[DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF];
    int action_signal_size[DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF];
    int action_signal_index;
    int action_mapping_index;
    dnx_field_action_t dnx_action_it;
    unsigned int action_size;
    int bits_on_mapping;
    int bits_unmapped;
    int bits_leaking_below;
    int bits_leaking_above;
    int bits_leaking_above_it;
    int bits_leaking_below_it;
    int action_signal_index_2;
    dnx_field_action_t dnx_actions_unordered[DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF];
    int offsets_unordered[DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF];
    int action_sizes_unordered[DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF];
    int action_ordered_index;
    int action_unordered_index;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(mapping_info_p, _SHR_E_PARAM, "mapping_info_p");
    SHR_NULL_CHECK(nof_actions_p, _SHR_E_PARAM, "nof_actions_p");
    SHR_NULL_CHECK(dnx_actions, _SHR_E_PARAM, "dnx_actions");
    SHR_NULL_CHECK(offsets, _SHR_E_PARAM, "offsets");

    /*
     * Initialize the output arrays.
     */
    for (action_mapping_index = 0; action_mapping_index < DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF;
         action_mapping_index++)
    {
        dnx_actions[action_mapping_index] = DNX_FIELD_ACTION_INVALID;
        offsets[action_mapping_index] = 0;
        action_sizes[action_mapping_index] = 0;
    }

    (*nof_actions_p) = 0;

    /*
     * Find the actions on signal.
     */
    SHR_IF_ERR_EXIT(dnx_field_action_vw_signal_actions_offsets
                    (unit, stage, mapping_info_p->physical_wire_name, &nof_actions_signal, action_dbal_id_signal,
                     offsets_signal));
    if (nof_actions_signal > DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Number of actions %d beyond maximum %d.\n",
                     nof_actions_signal, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF);
    }

    bits_on_mapping = mapping_info_p->virtual_wire_msb - mapping_info_p->virtual_wire_lsb + 1;
    if (bits_on_mapping <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "bits_on_mapping (%d) cannot be zero.\n", bits_on_mapping);
    }
    bits_unmapped = bits_on_mapping;
    bits_leaking_below = 0;
    bits_leaking_above = 0;
    for (action_signal_index = 0, action_mapping_index = 0; action_signal_index < nof_actions_signal;
         action_signal_index++)
    {
        /** Create the base DNX action.*/
        dnx_action_it = DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, stage, action_dbal_id_signal[action_signal_index]);
        /*
         * Get the size of the DNX action.
         * Note that in some cases the size may not be accurate (such as FWD_DOMAIN).
         */
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size(unit, stage, dnx_action_it, &action_size));
        if (action_size <= 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "DNX action 0x%x has 0 bits, not allowed for VW mapping.\n", dnx_action_it);
        }

        action_signal_size[action_signal_index] = action_size;
        /*
         * Check that actions do not collide.
         */
        for (action_signal_index_2 = 0; action_signal_index_2 < action_signal_index; action_signal_index_2++)
        {
            if (!((offsets_signal[action_signal_index_2] + action_signal_size[action_signal_index_2] <=
                   offsets_signal[action_signal_index]) ||
                  (offsets_signal[action_signal_index] + action_signal_size[action_signal_index] <=
                   offsets_signal[action_signal_index_2])))
            {
                /*
                 * Special exceptions, action supposedly colliding but not really
                 */
                if (!((stage == DNX_FIELD_STAGE_IPMF1 &&
                       (action_dbal_id_signal[action_signal_index] == DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_OR_TRANSPLANT ||
                        action_dbal_id_signal[action_signal_index_2] ==
                        DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_OR_TRANSPLANT)) || (stage == DNX_FIELD_STAGE_IPMF3
                                                                              &&
                                                                              (action_dbal_id_signal
                                                                               [action_signal_index] ==
                                                                               DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_OR_TRANSPLANT
                                                                               ||
                                                                               action_dbal_id_signal
                                                                               [action_signal_index_2] ==
                                                                               DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_OR_TRANSPLANT))))
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Colliding actiond %d and %d for signal \"%.*s\".\n",
                                 action_signal_index_2, action_signal_index,
                                 MAX_VW_NAME_LENGTH, mapping_info_p->physical_wire_name);
                }
            }
        }

        if ((offsets_signal[action_signal_index] + action_size <= mapping_info_p->physical_wire_lsb) ||
            (mapping_info_p->physical_wire_lsb + bits_on_mapping <= offsets_signal[action_signal_index]))
        {
            continue;
        }

        bits_leaking_above_it = 0;
        bits_leaking_below_it = 0;
        if (offsets_signal[action_signal_index] < mapping_info_p->physical_wire_lsb)
        {
            bits_leaking_below_it += mapping_info_p->physical_wire_lsb - offsets_signal[action_signal_index];
        }
        if ((offsets_signal[action_signal_index] + action_size) > (mapping_info_p->physical_wire_lsb + bits_on_mapping))
        {
            bits_leaking_above_it += (offsets_signal[action_signal_index] + action_size) -
                (mapping_info_p->physical_wire_lsb + bits_on_mapping);
        }
        if (bits_leaking_above_it + bits_leaking_below_it > action_size)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "illogical calculation. Action with size %d is leaking %d bits "
                         "(below %d, above %d).\n",
                         action_size, bits_leaking_below_it + bits_leaking_above_it,
                         bits_leaking_below_it, bits_leaking_above_it);
        }
        bits_unmapped -= (action_size - (bits_leaking_below_it + bits_leaking_above_it));
        if (bits_leaking_below && bits_leaking_below_it)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Multiple actions leaking below.\n");
        }
        if (bits_leaking_above && bits_leaking_above_it)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Multiple actions leaking above.\n");
        }
        bits_leaking_below += bits_leaking_below_it;
        bits_leaking_above += bits_leaking_above_it;

        dnx_actions_unordered[action_mapping_index] = dnx_action_it;
        offsets_unordered[action_mapping_index] = offsets_signal[action_signal_index];
        action_sizes_unordered[action_mapping_index] = action_size - bits_leaking_above;
        action_mapping_index++;
    }

    if (bits_unmapped < 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "bits_unmapped (%d) reached negative.\n", bits_unmapped);
    }

    /*
     * Order the output variables and check that they are continuous, if we fit the VW.
     */
    if (bits_unmapped == 0 && bits_leaking_below == 0 && bits_leaking_above == 0)
    {
        (*nof_actions_p) = action_mapping_index;
        /** For the first action, look for the zero offset.*/
        for (action_unordered_index = 0; action_unordered_index < (*nof_actions_p); action_unordered_index++)
        {
            if (offsets_unordered[action_unordered_index] == mapping_info_p->physical_wire_lsb)
            {
                dnx_actions[0] = dnx_actions_unordered[action_unordered_index];
                /** Offset if from the VW. */
                offsets[0] = offsets_unordered[action_unordered_index] - mapping_info_p->physical_wire_lsb;
                action_sizes[0] = action_sizes_unordered[action_unordered_index];
                break;
            }
        }
        if (action_unordered_index >= (*nof_actions_p))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error calculating action positions for virtual wire.\n");
        }
        for (action_ordered_index = 1; action_ordered_index < (*nof_actions_p); action_ordered_index++)
        {
            for (action_unordered_index = 0; action_unordered_index < (*nof_actions_p); action_unordered_index++)
            {
                if (offsets_unordered[action_unordered_index] ==
                    mapping_info_p->physical_wire_lsb + offsets[action_ordered_index - 1] +
                    action_sizes[action_ordered_index - 1])
                {
                    dnx_actions[action_ordered_index] = dnx_actions_unordered[action_unordered_index];
                    /** Offset if from the VW. */
                    offsets[action_ordered_index] =
                        offsets_unordered[action_unordered_index] - mapping_info_p->physical_wire_lsb;
                    action_sizes[action_ordered_index] = action_sizes_unordered[action_unordered_index];
                    break;
                }
            }
            if (action_unordered_index >= (*nof_actions_p))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error calculating action positions for virtual wire.\n");
            }
        }
    }

    if (bits_unmapped != 0 || bits_leaking_below != 0 || bits_leaking_above != 0)
    {
        if ((*nof_actions_p) > 0)
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE, "Signal \"%.*s\" was only partially matched for actions.%s%s\n",
                         MAX_VW_NAME_LENGTH, mapping_info_p->physical_wire_name, EMPTY, EMPTY);
        }
        (*nof_actions_p) = 0;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_vw_qual_action_init(
    int unit)
{
    uint8 is_std_1;
    dnx_field_stage_e dnx_stages[] = { DNX_FIELD_STAGE_IPMF1, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_STAGE_EXTERNAL };
    dnx_pp_stage_e pp_stages[sizeof(dnx_stages) / sizeof(dnx_stages[0])];
    uint8 stages_for_qual[sizeof(dnx_stages) / sizeof(dnx_stages[0])];
    int stage_idx;
    int vw_iterator;
    VirtualWireInfo *vw_info_p;
    int is_qual_available;
    int are_special_limitations_allowing;
    int signal_offset;
    int qual_map_index;
    int action_map_index;
    int action_map_vw_index;
    int dbal_field_created;
    dbal_fields_e dbal_field_id;
    dbal_fields_e dbal_field_id_first;
    int mapping_index;
    int size_single_mapping;
    int size_sum_of_mapping;
    int support_qual;
    int support_action;
    uint32 stages_bmp_qual;
    int full_mapping_available_qual;
    int full_mapping_available_action;
    int vw_action_index;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialize virtual wire information arrays.
     * Can be done only for bare metal, but done for standard 1 as well.
     */
    sal_memset(dnx_vw_qual_info, 0x0, sizeof(dnx_vw_qual_info));
    sal_memset(dnx_vw_action_info, 0x0, sizeof(dnx_vw_action_info));
    sal_memset(dnx_per_vw_action_info, 0x0, sizeof(dnx_per_vw_action_info));
    for (vw_action_index = 0; vw_action_index < (sizeof(dnx_vw_action_info) / sizeof(dnx_vw_action_info[0]));
         vw_action_index++)
    {
        dnx_vw_action_info[vw_action_index].vw_index = -1;
    }
    /*
     * Sanity check to ensure non-FP stages don't get used in vw quals
     */
    for (stage_idx = 0; stage_idx < (sizeof(dnx_stages) / sizeof(dnx_stages[0])); stage_idx++)
    {
        if (!DNX_FIELD_IS_FP_STAGE(dnx_stages[stage_idx]))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "DNX stage %s can't be picked for VW quals\n",
                         dnx_field_stage_text(unit, dnx_stages[stage_idx]));
        }
    }
    /*
     * Verify that the DNX DATA of the general data index in iPMF1 is correct.
     */
    {
        int ipmf1_general_data_index = dnx_data_field.virtual_wire.ipmf1_general_data_index_get(unit);
        int nof_signals_per_stage = dnx_data_field.virtual_wire.signals_nof_get(unit);
        char general_data_signal_name[] = "general_data";
        if (ipmf1_general_data_index < 0 || ipmf1_general_data_index >= nof_signals_per_stage)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "DNX DATA ipmf1_general_data_index (%d) should be between 0-%d.\n",
                         ipmf1_general_data_index, nof_signals_per_stage - 1);
        }
        if ((0 !=
             sal_strncmp(general_data_signal_name,
                         dnx_data_field.virtual_wire.signal_mapping_get(unit, DNX_FIELD_STAGE_IPMF1,
                                                                        ipmf1_general_data_index)->signal_name,
                         sizeof(general_data_signal_name))))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "DNX DATA ipmf1_general_data_index (%d) does not point to signal "
                         "\"%s\" but to \"%s\".\n",
                         ipmf1_general_data_index, general_data_signal_name,
                         dnx_data_field.virtual_wire.signal_mapping_get
                         (unit, DNX_FIELD_STAGE_IPMF1, ipmf1_general_data_index)->signal_name);
        }
    }

    /*
     * Read the virtual wires for qualifiers for bare metal.
     */
    SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_std_1));
    if (is_std_1 == FALSE)
    {
        /** Note we do not support DNX_FIELD_STAGE_IPMF2 and DNX_FIELD_STAGE_EPMF yet.*/
        if (DNX_FIELD_STAGE_NOF >= SAL_UINT32_NOF_BITS)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Too many stages (%d) for stage bitmap.\n", DNX_FIELD_STAGE_NOF);
        }

        if (dnx_data_field.qual.vw_nof_get(unit) > DNX_FIELD_MAP_MAX_VW_QUAL)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "Max numebr of VW qualifiers %d larger than %d.\n",
                         dnx_data_field.qual.vw_nof_get(unit), DNX_FIELD_MAP_MAX_VW_QUAL);
        }
        if (dnx_data_field.action.vw_nof_get(unit) > DNX_FIELD_MAP_MAX_VW_ACTION)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "Max numebr of VW actions %d larger than %d.\n",
                         dnx_data_field.action.vw_nof_get(unit), DNX_FIELD_MAP_MAX_VW_ACTION);
        }
        if (dnx_data_field.virtual_wire.actions_per_signal_nof_get(unit) > DNX_FIELD_ACTION_MAX_ACTION_PER_VM)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "Max numebr of actions per signal (%d) "
                         "smaller then max number of actions per VW (%d).\n",
                         dnx_data_field.virtual_wire.actions_per_signal_nof_get(unit),
                         DNX_FIELD_ACTION_MAX_ACTION_PER_VM);
        }
        if (DNX_FIELD_ACTION_MAX_ACTION_PER_VM > BCM_FIELD_NAME_TO_ID_MAX_IDS)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "Max numebr of actions per VM DNX value (%d) larger than "
                         "BCM value for max IDs per name DNX_FIELD_ACTION_MAX_ACTION_PER_VM (%d).\n",
                         DNX_FIELD_ACTION_MAX_ACTION_PER_VM, BCM_FIELD_NAME_TO_ID_MAX_IDS);
        }

        for (stage_idx = 0; stage_idx < (sizeof(dnx_stages) / sizeof(dnx_stages[0])); stage_idx++)
        {
            pp_stages[stage_idx] = dnx_field_map_stage_info[dnx_stages[stage_idx]].pp_stage;
        }

        vw_iterator = PEMLADRV_ITERATOR_BY_STAGE_INIT;
        qual_map_index = 0;
        action_map_index = 0;
        action_map_vw_index = 0;
        while (1)
        {
            SHR_IF_ERR_EXIT(pemladrv_vw_wire_iterator_by_stage_get_next
                            (unit, sizeof(dnx_stages) / sizeof(dnx_stages[0]), pp_stages, &vw_iterator, &vw_info_p,
                             stages_for_qual));
            if (vw_iterator == PEMLADRV_ITERATOR_BY_STAGE_END)
            {
                break;
            }

            support_qual = TRUE;
            support_action = TRUE;
            dbal_field_created = FALSE;
            dbal_field_id_first = DBAL_FIELD_EMPTY;

            if (0 == sal_strncmp(vw_info_p->virtual_wire_name, "EMPTY", MAX_VW_NAME_LENGTH))
            {
                /*
                 * Do not allow the default name in DNX DATA as a VW name.
                 */
                SHR_ERR_EXIT(_SHR_E_CONFIG, "VW \"%.*s\" has an illegal name.\n",
                             MAX_VW_NAME_LENGTH, vw_info_p->virtual_wire_name);
            }
            if (vw_info_p->nof_mappings <= 0)
            {
                LOG_DEBUG_EX(BSL_LOG_MODULE, "VW \"%.*s\" has no mappings (%d).%s\n",
                             MAX_VW_NAME_LENGTH, vw_info_p->virtual_wire_name, vw_info_p->nof_mappings, EMPTY);
                continue;
            }
            if (vw_info_p->nof_mappings != 1)
            {
                /** We do not support split VWs, nor VWs with zero mapping.*/
                LOG_DEBUG_EX(BSL_LOG_MODULE, "VW \"%.*s\" has mapping different than 1 (%d).%s\n",
                             MAX_VW_NAME_LENGTH, vw_info_p->virtual_wire_name, vw_info_p->nof_mappings, EMPTY);
                continue;
            }
            if (vw_info_p->width_in_bits <= 0)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "VW \"%.*s\" has %d bits, must be at least 1.\n",
                             MAX_VW_NAME_LENGTH, vw_info_p->virtual_wire_name, vw_info_p->width_in_bits);
            }
            if (vw_info_p->width_in_bits > dnx_data_field.qual.max_bits_in_qual_get(unit))
            {
                LOG_DEBUG_EX(BSL_LOG_MODULE, "VW \"%.*s\" has %d bit, maximum for qual is %d.\n",
                             MAX_VW_NAME_LENGTH, vw_info_p->virtual_wire_name,
                             vw_info_p->width_in_bits, dnx_data_field.qual.max_bits_in_qual_get(unit));
                support_qual = FALSE;
            }
            if (sal_strnlen(vw_info_p->virtual_wire_name, MAX_VW_NAME_LENGTH) >= DBAL_MAX_STRING_LENGTH)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "VW \"%.*s\" has %d characters, maximum is %d.\n",
                             MAX_VW_NAME_LENGTH, vw_info_p->virtual_wire_name,
                             sal_strnlen(vw_info_p->virtual_wire_name, MAX_VW_NAME_LENGTH), DBAL_MAX_STRING_LENGTH - 1);
            }
            if (vw_info_p->virtual_wire_name[0] == '\0')
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "VW found with empty string for name.\n");
            }
            if (vw_info_p->vw_mappings_arr[0].virtual_wire_lsb != 0)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "First mapping of virtual wire does not start with zero. "
                             "virtual_wire_lsb %d virtual_wire_msb %d width_in_bits %d.\n",
                             vw_info_p->vw_mappings_arr[0].virtual_wire_lsb,
                             vw_info_p->vw_mappings_arr[0].virtual_wire_msb, vw_info_p->width_in_bits);
            }
            size_sum_of_mapping = 0;
            for (mapping_index = 0; mapping_index < vw_info_p->nof_mappings; mapping_index++)
            {
                size_single_mapping = vw_info_p->vw_mappings_arr[mapping_index].virtual_wire_msb -
                    vw_info_p->vw_mappings_arr[mapping_index].virtual_wire_lsb + 1;
                size_sum_of_mapping += size_single_mapping;
                if (vw_info_p->vw_mappings_arr[mapping_index].virtual_wire_msb <
                    vw_info_p->vw_mappings_arr[mapping_index].virtual_wire_lsb)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Mapping %d of VW VW \"%.*s\" has msb smaller than lsb. "
                                 "virtual_wire_lsb %d virtual_wire_msb %d.\n",
                                 mapping_index, MAX_VW_NAME_LENGTH, vw_info_p->virtual_wire_name,
                                 vw_info_p->vw_mappings_arr[mapping_index].virtual_wire_lsb,
                                 vw_info_p->vw_mappings_arr[mapping_index].virtual_wire_msb);
                }
                if (size_single_mapping <= 0)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Mapping %d of VW \"%.*s\" has less than 1 bit. "
                                 "virtual_wire_lsb %d virtual_wire_msb %d.\n",
                                 mapping_index, MAX_VW_NAME_LENGTH, vw_info_p->virtual_wire_name,
                                 vw_info_p->vw_mappings_arr[mapping_index].virtual_wire_lsb,
                                 vw_info_p->vw_mappings_arr[mapping_index].virtual_wire_msb);
                }
                if ((mapping_index > 0) &&
                    (vw_info_p->vw_mappings_arr[mapping_index - 1].virtual_wire_msb + 1 !=
                     vw_info_p->vw_mappings_arr[mapping_index].virtual_wire_lsb))
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Mapping %d of VM \"%.*s\" has lsb (%d) not "
                                 "consecutive to the msb of previous mapping (%d).\n",
                                 mapping_index, MAX_VW_NAME_LENGTH, vw_info_p->virtual_wire_name,
                                 vw_info_p->vw_mappings_arr[mapping_index].virtual_wire_lsb,
                                 vw_info_p->vw_mappings_arr[mapping_index - 1].virtual_wire_msb);
                }
            }
            if (size_sum_of_mapping != vw_info_p->width_in_bits)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "VW \"%.*s\" has %d bits, but the sum of all bits on mapping is %d.\n",
                             MAX_VW_NAME_LENGTH, vw_info_p->virtual_wire_name,
                             vw_info_p->width_in_bits, size_sum_of_mapping);
            }

            stages_bmp_qual = 0;
            for (stage_idx = 0; stage_idx < (sizeof(dnx_stages) / sizeof(dnx_stages[0])); stage_idx++)
            {
                if (support_qual)
                {
                    if (stages_for_qual[stage_idx])
                    {
                        full_mapping_available_qual = TRUE;
                        for (mapping_index = 0; mapping_index < vw_info_p->nof_mappings; mapping_index++)
                        {
                            SHR_IF_ERR_EXIT(dnx_field_qual_vw_signal_offset
                                            (unit, dnx_stages[stage_idx],
                                             vw_info_p->vw_mappings_arr[mapping_index].physical_wire_name,
                                             vw_info_p->vw_mappings_arr[mapping_index].physical_wire_lsb,
                                             &is_qual_available, &signal_offset));
                            if (is_qual_available == FALSE)
                            {
                                full_mapping_available_qual = FALSE;
                                break;
                            }
                            SHR_IF_ERR_EXIT(dnx_field_qual_vw_signal_special_verify
                                            (unit, dnx_stages[stage_idx],
                                             vw_info_p->vw_mappings_arr[mapping_index].physical_wire_name,
                                             vw_info_p->vw_mappings_arr[mapping_index].physical_wire_lsb,
                                             (vw_info_p->vw_mappings_arr[mapping_index].virtual_wire_msb -
                                              vw_info_p->vw_mappings_arr[mapping_index].virtual_wire_lsb + 1),
                                             &are_special_limitations_allowing));
                            if (are_special_limitations_allowing == FALSE)
                            {
                                full_mapping_available_qual = FALSE;
                                break;
                            }
                        }
                        if (full_mapping_available_qual)
                        {
                            stages_bmp_qual |= SAL_BIT(dnx_stages[stage_idx]);
                            /*
                             * (For iPMF1, also add the qualifier to iPMF2)
                             */
                            if (dnx_stages[stage_idx] == DNX_FIELD_STAGE_IPMF1)
                            {
                                stages_bmp_qual |= SAL_BIT(DNX_FIELD_STAGE_IPMF2);
                            }
                        }
                    }
                }
                if (support_action)
                {
                    int nof_actions;
                    dnx_field_action_t dnx_actions[DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF];
                    int action_offsets[DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF];
                    int action_sizes[DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF];
                    int nof_actions_total;
                    dnx_field_action_t dnx_actions_total[DNX_FIELD_ACTION_MAX_ACTION_PER_VM];
                    int action_index;
                    int action_index_2;
                    /** dbal_fitting_string size must be at least one more than the maximum for DBAL name string.*/
                    char dbal_fitting_string[DBAL_MAX_STRING_LENGTH + 1];

                    full_mapping_available_action = TRUE;
                    nof_actions_total = 0;
                    for (mapping_index = 0; mapping_index < vw_info_p->nof_mappings; mapping_index++)
                    {
                        SHR_IF_ERR_EXIT(dnx_field_action_vw_single_mapping_actions_offsets
                                        (unit, dnx_stages[stage_idx], &vw_info_p->vw_mappings_arr[mapping_index],
                                         &nof_actions, dnx_actions, action_offsets, action_sizes));
                        if (nof_actions <= 0)
                        {
                            full_mapping_available_action = FALSE;
                            break;
                        }
                        nof_actions_total += nof_actions;
                        if (nof_actions_total > DNX_FIELD_ACTION_MAX_ACTION_PER_VM)
                        {
                            LOG_DEBUG_EX(BSL_LOG_MODULE, "VW \"%.*s\" exceeds maximum number of actions per VM %d.%s\n",
                                         MAX_VW_NAME_LENGTH, vw_info_p->virtual_wire_name,
                                         DNX_FIELD_ACTION_MAX_ACTION_PER_VM, EMPTY);
                            full_mapping_available_action = FALSE;
                            break;
                        }
                        for (action_index = 0; action_index < nof_actions; action_index++)
                        {
                            dnx_actions_total[action_index + nof_actions_total - nof_actions] =
                                dnx_actions[action_index];
                            if (action_sizes[action_index] <= 0)
                            {
                                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                             "VW \"%.*s\" action %d (0x%x) has size %d. must be at least 1.\n",
                                             MAX_VW_NAME_LENGTH, vw_info_p->virtual_wire_name,
                                             action_index + nof_actions_total - nof_actions, dnx_actions[action_index],
                                             action_sizes[action_index]);
                            }
                        }
                    }
                    if (full_mapping_available_action)
                    {
                        /*
                         * Verify that no action appears twice.
                         */
                        for (action_index = 0; action_index < nof_actions_total; action_index++)
                        {
                            for (action_index_2 = 0; action_index_2 < action_index; action_index_2++)
                            {
                                if (dnx_actions_total[action_index] == dnx_actions_total[action_index_2])
                                {
                                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                                 "VW \"%.*s\" actions %d and %d are the same (0x%x).\n",
                                                 MAX_VW_NAME_LENGTH, vw_info_p->virtual_wire_name,
                                                 action_index, action_index_2, dnx_actions_total[action_index]);
                                }
                            }
                        }
                        dnx_per_vw_action_info[action_map_vw_index].nof_actions_per_stage[dnx_stages[stage_idx]] =
                            nof_actions_total;
                        /*
                         * Create actions, if not created by a previous stages.
                         */
                        for (action_index = 0; action_index < nof_actions_total; action_index++)
                        {
                            if (action_map_vw_index >= DNX_FIELD_MAP_MAX_VW_WITH_ACTION)
                            {
                                SHR_ERR_EXIT(_SHR_E_CONFIG, "More than %d virtual wires to create actions for.\n",
                                             DNX_FIELD_MAP_MAX_VW_WITH_ACTION);
                            }
                            if (action_map_index >= DNX_FIELD_MAP_MAX_VW_ACTION)
                            {
                                SHR_ERR_EXIT(_SHR_E_CONFIG, "More than %d virtual wires actions.\n",
                                             DNX_FIELD_MAP_MAX_VW_ACTION);
                            }

                            dnx_per_vw_action_info[action_map_vw_index].base_dnx_action_per_stage[action_index]
                                [dnx_stages[stage_idx]] = dnx_actions_total[action_index];
                            /** Update the per action table with the indices. */
                            dnx_vw_action_info[action_map_index].vw_index = action_map_vw_index;
                            dnx_vw_action_info[action_map_index].action_index_within_vw = action_index;
                            action_map_index++;

                            if (dnx_per_vw_action_info[action_map_vw_index].field_id[action_index])
                            {
                                continue;
                            }
                            /*
                             * Copy the dbal field name to a string one byte larger then the maximum name size,
                             * to see that we do not overflow.
                             * The first action takes the name of the VM, for the rest we add the index.
                             */
                            if (action_index == 0)
                            {
                                sal_strncpy_s(dbal_fitting_string, vw_info_p->virtual_wire_name,
                                              sizeof(dbal_fitting_string));
                            }
                            else
                            {
                                sal_snprintf(dbal_fitting_string, sizeof(dbal_fitting_string), "%s_%d",
                                             vw_info_p->virtual_wire_name, action_index);
                            }
                            dbal_fitting_string[sizeof(dbal_fitting_string) - 1] = '\0';
                            if (sal_strnlen(dbal_fitting_string, sizeof(dbal_fitting_string)) >= DBAL_MAX_STRING_LENGTH)
                            {
                                SHR_ERR_EXIT(_SHR_E_CONFIG, "VW \"%.*s\" action %d could not fit as a DBAL name.\n",
                                             MAX_VW_NAME_LENGTH, vw_info_p->virtual_wire_name, action_index);
                            }

                            /*
                             * Create a DBAL field.
                             */
                            SHR_IF_ERR_EXIT(dbal_fields_field_create
                                            (unit, DBAL_FIELD_TYPE_DEF_UINT, dbal_fitting_string, &dbal_field_id));
                            dbal_field_created = TRUE;
                            if (action_index == 0)
                            {
                                dbal_field_id_first = dbal_field_id;
                            }

                            dnx_per_vw_action_info[action_map_vw_index].field_id[action_index] = dbal_field_id;
                        }

                    }
                }
            }

            /** Any changes done to iPMF1 must also happen to iPMF2. */
            {
                int internal_action_index;
                dnx_field_action_t internal_dnx_action;

                dnx_per_vw_action_info[action_map_vw_index].nof_actions_per_stage[DNX_FIELD_STAGE_IPMF2] =
                    dnx_per_vw_action_info[action_map_vw_index].nof_actions_per_stage[DNX_FIELD_STAGE_IPMF1];
                for (internal_action_index = 0; internal_action_index < DNX_FIELD_ACTION_MAX_ACTION_PER_VM;
                     internal_action_index++)
                {
                    internal_dnx_action =
                        dnx_per_vw_action_info[action_map_vw_index].base_dnx_action_per_stage[internal_action_index]
                        [DNX_FIELD_STAGE_IPMF1];
                    dnx_per_vw_action_info[action_map_vw_index].base_dnx_action_per_stage[internal_action_index]
                        [DNX_FIELD_STAGE_IPMF2] =
                        DNX_ACTION(DNX_ACTION_CLASS(internal_dnx_action), DNX_FIELD_STAGE_IPMF2,
                                   DNX_ACTION_ID(internal_dnx_action));
                }
            }

            /** Progress the counter of VW with actions. */
            if (dbal_field_created)
            {
                action_map_vw_index++;
            }

            /** Sanity check*/
            if ((dbal_field_created && (dbal_field_id_first == DBAL_FIELD_EMPTY)) ||
                ((!dbal_field_created) && (dbal_field_id_first != DBAL_FIELD_EMPTY)))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Mismatch in DBAL field creation for action.\n");
            }

            if (stages_bmp_qual != 0)
            {
                if (qual_map_index >= DNX_FIELD_MAP_MAX_VW_QUAL)
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG, "More than %d virtual wires to create qualifiers for.\n",
                                 DNX_FIELD_MAP_MAX_VW_QUAL);
                }

                /*
                 * Create a DBAL field, if not created for action.
                 */
                if (dbal_field_created == FALSE)
                {
                    SHR_IF_ERR_EXIT(dbal_fields_field_create
                                    (unit, DBAL_FIELD_TYPE_DEF_UINT, vw_info_p->virtual_wire_name, &dbal_field_id));
                }
                else
                {
                    dbal_field_id = dbal_field_id_first;
                }

                /*
                 * Update the qualifier information.
                 */
                dnx_vw_qual_info[qual_map_index].field_id = dbal_field_id;
                dnx_vw_qual_info[qual_map_index].stage_bmp = stages_bmp_qual;
                qual_map_index++;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See field_map.h
 */
shr_error_e
dnx_field_map_sw_state_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_action_sw_db.init(unit));
     /** Assumption all SW state init to 0, hence all action are set to valid=FALSE by default*/
    SHR_IF_ERR_EXIT(dnx_field_qual_sw_db.init(unit));
     /** Assumption all SW state init to 0, hence all qualifier are set to valid=FALSE by default*/

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Read mapping of apptypes to contexts from XML.
* \param [in] unit - Device ID.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_map_apptype_to_context_parse_init(
    int unit)
{
    void *catalogue = NULL;
    void *opcode_node;
    char *image_name;
    char *opcodes_path = NULL;
    uint32 opcode_id;
    void *context_iter;
    int context_index;
    uint8 is_standard_image;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(opcodes_path, DBAL_MAX_PRINTABLE_BUFFER_SIZE, "opcodes_path", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_standard_image));
    SHR_IF_ERR_EXIT(dnx_pp_prgm_device_image_name_get(unit, &image_name));
    sal_snprintf(opcodes_path, DBAL_MAX_PRINTABLE_BUFFER_SIZE, "auto_generated/%s_fwd_app_type.xml", image_name);

    catalogue = dbx_pre_compiled_fwd_app_types_top_get(unit, opcodes_path, "FWD2_APP_TYPES", CONF_OPEN_PER_DEVICE);
    if (catalogue == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find FWD2_APP_TYPES configuration xml file: %s\n", opcodes_path);
    }

    /** reading each opcode and updating the info */
    RHDATA_ITERATOR(opcode_node, catalogue, "AppType")
    {
        bcm_field_AppType_t apptype = bcmFieldAppTypeCount;
        char opcode_name_full_size[RHSTRING_MAX_SIZE] = { 0 };
        char opcode_name[DBAL_MAX_STRING_LENGTH] = { 0 };

        RHDATA_GET_STR_STOP(opcode_node, "name", opcode_name_full_size);

        if (sal_strnlen(opcode_name_full_size, (int) sizeof(opcode_name_full_size)) >= sizeof(opcode_name))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "Opcode \"%.*s\" has name with %d charachters, maximum %d.\n",
                         (int) sizeof(opcode_name_full_size), opcode_name_full_size,
                         sal_strnlen(opcode_name_full_size, (int) sizeof(opcode_name_full_size)),
                         (int) sizeof(opcode_name) - 1);
        }

        sal_strncpy_s(opcode_name, opcode_name_full_size, sizeof(opcode_name));

        /** Verify that the opcode name matches the ENUM value. */
        rv = dbal_enum_type_string_to_id_no_err(unit, DBAL_FIELD_TYPE_DEF_KBP_FWD_OPCODE, opcode_name, &opcode_id);
        if (rv == _SHR_E_NOT_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "Opcode \"%.*s\" is not found as an ENUM value.\n",
                         (int) sizeof(opcode_name), opcode_name);
        }
        SHR_IF_ERR_EXIT(rv);

        if ((((int) opcode_id) < 0)
            || (opcode_id >= (sizeof(dnx_field_static_opcode_info) / sizeof(dnx_field_static_opcode_info[0]))))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "opcode_id %d (\"%.*s\") is out of range (0-%d).\n",
                         opcode_id, (int) sizeof(opcode_name), opcode_name,
                         (int) ((sizeof(dnx_field_static_opcode_info) / sizeof(dnx_field_static_opcode_info[0])) - 1));
        }

        /*
         * Check if the context has an apptype associated with it.
         * For bare metal, apptype==opcode.
         */
        if (is_standard_image)
        {
            int apptype_index;
            for (apptype_index = 0;
                 apptype_index < (sizeof(dnx_field_standard_1_static_apptype_map) /
                                  sizeof(dnx_field_standard_1_static_apptype_map[0])); apptype_index++)
            {
                if (dnx_field_standard_1_static_apptype_map[apptype_index].opcode_id == opcode_id)
                {
                    apptype = apptype_index;
                    break;
                }
            }
        }
        else
        {
            apptype = opcode_id;
        }
        if (apptype >= bcmFieldAppTypeCount)
        {
            /*
             * For now we tolerate unmapped apptypes and don't send an error.
             */
            LOG_DEBUG_EX(BSL_LOG_MODULE, "Could not map opcode %d (\"%.*s\") to apptype.%s\n",
                         opcode_id, (int) sizeof(opcode_name), opcode_name, EMPTY);
            continue;
        }

        context_index = 0;
        RHDATA_ITERATOR(context_iter, opcode_node, "FwdContext")
        {
            uint32 fwd_context_enum_value;
            uint32 fwd_context_uint32;
            char ctx_name_full_size[RHSTRING_MAX_SIZE] = { 0 };
            char ctx_name[DBAL_MAX_STRING_LENGTH] = { 0 };

            RHDATA_GET_STR_STOP(context_iter, "name", ctx_name_full_size);

            if (sal_strnlen(opcode_name_full_size, (int) sizeof(ctx_name_full_size)) >= sizeof(ctx_name))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "Context \"%.*s\" has name with %d charachters, maximum %d. "
                             "Mapped to opcode %d (\"%.*s\").\n",
                             (int) sizeof(opcode_name_full_size), opcode_name_full_size,
                             sal_strnlen(opcode_name_full_size, sizeof(ctx_name_full_size)),
                             (int) sizeof(ctx_name) - 1, opcode_id, (int) sizeof(opcode_name), opcode_name);
            }

            sal_strncpy_s(ctx_name, ctx_name_full_size, (int) sizeof(ctx_name));

            if (context_index >= DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "Opcode %d (\"%.*s\") is mapped to more than %d contexts.\n",
                             opcode_id, (int) sizeof(opcode_name), opcode_name,
                             DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE);
            }

            /*
             * Get the context ID from the ENUM HW value.
             * Note that for predefiend contexts, FWD and ACL context IDs are the same.
             * Verify the the context ID fits in uint8.
             */
            SHR_IF_ERR_EXIT(dbal_enum_type_string_to_id
                            (unit, DBAL_FIELD_TYPE_DEF_FWD2_CONTEXT_ID, ctx_name, &fwd_context_enum_value));
            SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                            (unit, DBAL_FIELD_FWD2_CONTEXT_ID, fwd_context_enum_value, &fwd_context_uint32));
            if (((uint8) fwd_context_uint32) != fwd_context_uint32)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "context %d (\"%.*s\") doesn't fit inside one byte.\n",
                             fwd_context_uint32, (int) sizeof(ctx_name), ctx_name);
            }

            /*
             * Save the context to SW state.
             */
            SHR_IF_ERR_EXIT(dnx_field_apptype_sw.predef_info.
                            context_ids.set(unit, apptype, context_index, fwd_context_uint32));

            context_index++;
        }
        if (context_index <= 0)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "No contexts for opcode %d (\"%.*s\"\n",
                         opcode_id, (int) sizeof(opcode_name), opcode_name);
        }
        SHR_IF_ERR_EXIT(dnx_field_apptype_sw.predef_info.nof_context_ids.set(unit, apptype, context_index));
    }

exit:
    SHR_FREE(opcodes_path);
    dbx_xml_top_close(catalogue);
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Init the mapping and information of apptypes and opcodes.
* \param [in] unit  - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_map_kbp_apptype_opcode_init(
    int unit)
{
    dbal_enum_value_field_kbp_fwd_opcode_e i_opcode;
    dbal_enum_value_field_kbp_fwd_opcode_e mapped_opcode;
    bcm_field_AppType_t i_apptype;
    int nof_static_apptypes_standard_1;
    int nof_static_opcodes;
    int nof_static_apptypes;
    int cs_profile_id_counter;
    int i_user_defined_opcode;
    int first_user_defined_apptype = dnx_data_field.kbp.apptype_user_1st_get(unit);
    int nof_user_defined_apptypes = dnx_data_field.kbp.apptype_user_nof_get(unit);
    uint8 is_init;
    uint8 is_standard_image;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_standard_image));

    nof_static_opcodes = sizeof(dnx_field_static_opcode_info) / sizeof(dnx_field_static_opcode_info[0]);
    nof_static_apptypes_standard_1 = sizeof(dnx_field_standard_1_static_apptype_map) /
        sizeof(dnx_field_standard_1_static_apptype_map[0]);

    if (nof_static_opcodes != DBAL_NOF_ENUM_KBP_FWD_OPCODE_VALUES)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Unexpected number of elements in dnx_field_static_opcode_info %d. Expected %d.\n",
                     nof_static_opcodes, DBAL_NOF_ENUM_KBP_FWD_OPCODE_VALUES);
    }
    if (nof_static_apptypes_standard_1 != bcmFieldAppTypeCount)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Unexpected number of elements in dnx_field_static_apptype_map %d. Expected %d.\n",
                     nof_static_apptypes_standard_1, bcmFieldAppTypeCount);
    }
#if defined(INCLUDE_KBP)
    if (nof_static_opcodes > DNX_KBP_MAX_NOF_OPCODES)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Number of elements in dnx_field_static_opcode_info is %d, "
                     "Exceeds maximum number of opcodes %d.\n", nof_static_opcodes, DNX_KBP_MAX_NOF_OPCODES);
    }
#endif
    if (nof_static_apptypes_standard_1 >= first_user_defined_apptype)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Number of predefiend Apptypes (%d) reached the user defined range, starting from %d.\n",
                     nof_static_apptypes_standard_1, first_user_defined_apptype);
    }
    if (DBAL_NOF_ENUM_KBP_FWD_OPCODE_VALUES >= first_user_defined_apptype)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Number of predefiend opcodes (%d), which in bare metal serve as predefiend "
                     "Apptypes as well, reached the user defined range, starting from %d.\n",
                     DBAL_NOF_ENUM_KBP_FWD_OPCODE_VALUES, first_user_defined_apptype);
    }

    if (DBAL_ENUM_FVAL_KBP_FWD_OPCODE_INVALID != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "DBAL_ENUM_FVAL_KBP_FWD_OPCODE_INVALID isn't zero, leading to imporoper ititialization of "
                     "dnx_field_static_apptype_map.\n");
    }

    if (is_standard_image)
    {
        nof_static_apptypes = nof_static_apptypes_standard_1;
    }
    else
    {
        nof_static_apptypes = DBAL_NOF_ENUM_KBP_FWD_OPCODE_VALUES;
    }

    if (!sw_state_is_warm_boot(unit))
    {
        /** Initialize the apptype SW state*/
        SHR_IF_ERR_EXIT(dnx_field_apptype_sw.is_init(unit, &is_init));
        if (is_init)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "dnx_field_apptype_sw is iitialized twice.\r\n");
        }
        SHR_IF_ERR_EXIT(dnx_field_apptype_sw.init(unit));
    }

    /*
     * Initialize the OPCODE to apptype mapping.
     */
    for (i_opcode = 0; i_opcode < nof_static_opcodes; i_opcode++)
    {
        dnx_field_static_opcode_info[i_opcode].app_type = DNX_BCM_ID_INVALID;
        /** The following two lines should have no effect, as the array is initialized.*/
        dnx_field_static_opcode_info[i_opcode].is_valid = FALSE;
        dnx_field_static_opcode_info[i_opcode].in_use = FALSE;
    }

    
    {
        uint8 is_std_1;
        SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_std_1));
        if (is_std_1 == FALSE)
        {
            SHR_EXIT();
        }
    }

    if (!sw_state_is_warm_boot(unit))
    {
        /*
         * Read the mapping of apptypes to contexts from mapping.
         */
        SHR_IF_ERR_EXIT(dnx_field_map_apptype_to_context_parse_init(unit));
    }

    /*
     * Go over all static apptypes and add their mapping to static opcodes.
     */
    for (i_apptype = 0; i_apptype < nof_static_apptypes; i_apptype++)
    {
        uint8 nof_contexts;
        uint32 opcode_hw_value;
        shr_error_e rv;

        if (is_standard_image)
        {
            mapped_opcode = dnx_field_standard_1_static_apptype_map[i_apptype].opcode_id;
            if (mapped_opcode == DBAL_ENUM_FVAL_KBP_FWD_OPCODE_INVALID)
            {
                continue;
            }
        }
        else
        {
            mapped_opcode = i_apptype;
        }

        if (mapped_opcode < 0 || mapped_opcode >= DBAL_NOF_ENUM_KBP_FWD_OPCODE_VALUES)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Apptype %d mapped to opcode %d. Maximum static opcode number is %d\n",
                         i_apptype, mapped_opcode, (DBAL_NOF_ENUM_KBP_FWD_OPCODE_VALUES - 1));
        }

        rv = dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_KBP_FWD_OPCODE, mapped_opcode, &opcode_hw_value);
        if (rv == _SHR_E_PARAM)
        {
            if (is_standard_image)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Apptype %d mapped to opcode %d, which has no DBAL mapping.\n",
                             i_apptype, mapped_opcode);
            }
            else
            {
                /** If bare metal, the apptype/opcode (both are the same is bare metal) is simply not allocated.*/
                continue;
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(rv);
        }
        if (opcode_hw_value >= DNX_KBP_MAX_STATIC_OPCODES_NOF)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Apptype %d mapped to opcode %d with HW value %d, above dynamic range (%d).\n",
                         i_apptype, mapped_opcode, opcode_hw_value, DNX_KBP_MAX_STATIC_OPCODES_NOF);
        }

        dnx_field_static_opcode_info[mapped_opcode].app_type = i_apptype;
        dnx_field_static_opcode_info[mapped_opcode].is_valid = TRUE;

        /*
         * Check if the opcode is used by a FWD context.
         */
        SHR_IF_ERR_EXIT(dnx_field_apptype_sw.predef_info.nof_context_ids.get(unit, i_apptype, &nof_contexts));
        if (nof_contexts > 0)
        {
            dnx_field_static_opcode_info[mapped_opcode].in_use = TRUE;
        }
    }

    /*
     * Allocated profile IDs. We allocate profiles for static apptypes with WITH_ID flag so that it will have the
     * same mapping for all units in a multi unit system.
     * We do not allocate profile 0, the default profile.
     */
    cs_profile_id_counter = 1;
    for (i_opcode = 0; i_opcode < nof_static_opcodes; i_opcode++)
    {
        if (dnx_field_static_opcode_info[i_opcode].is_valid && dnx_field_static_opcode_info[i_opcode].in_use)
        {
            if (!sw_state_is_warm_boot(unit))
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_kbp_opcode_cs_profile_id_allocate
                                (unit, DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID, &cs_profile_id_counter));
            }
            dnx_field_static_opcode_info[i_opcode].cs_profile_id = cs_profile_id_counter;
            cs_profile_id_counter++;
        }
    }

    /*
     * Now check if we have enough profiles for the user define Apptypes
     */
    if (!sw_state_is_warm_boot(unit))
    {
        int test_profile_id;
        for (i_user_defined_opcode = 0; i_user_defined_opcode < nof_user_defined_apptypes; i_user_defined_opcode++)
        {
            test_profile_id = cs_profile_id_counter + i_user_defined_opcode;
            SHR_IF_ERR_EXIT_WITH_LOG(dnx_algo_field_kbp_opcode_cs_profile_id_allocate
                                     (unit, DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID, &test_profile_id),
                                     "Only has room for %d user defined Apptypes, %d is required.%s\n",
                                     i_user_defined_opcode, nof_user_defined_apptypes, EMPTY);
        }
        for (i_user_defined_opcode = 0; i_user_defined_opcode < nof_user_defined_apptypes; i_user_defined_opcode++)
        {
            test_profile_id = cs_profile_id_counter + i_user_defined_opcode;
            SHR_IF_ERR_EXIT(dnx_algo_field_kbp_opcode_cs_profile_id_deallocate(unit, test_profile_id));
        }
    }

    /*
     * Chcek that the apptypes, opcodes and contexts are aligned with KBP.
     */
    for (i_apptype = 0; i_apptype < nof_static_apptypes; i_apptype++)
    {
        shr_error_e rv;
        int mapped_nof_contexts;
        dnx_field_context_t mapped_fwd_contexts[DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];
        dnx_field_context_t mapped_acl_contexts[DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];

        sal_memset(mapped_fwd_contexts, 0x0, sizeof(mapped_fwd_contexts));
        sal_memset(mapped_acl_contexts, 0x0, sizeof(mapped_acl_contexts));

        rv = dnx_field_map_apptype_to_opcode_bcm_to_dnx_int(unit, i_apptype, &mapped_opcode);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        if (rv != _SHR_E_NOT_FOUND)
        {
            if (is_standard_image && (dnx_field_standard_1_static_apptype_map[i_apptype].opcode_id != mapped_opcode))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Apptype %d mapped alternatively to opcodes %d and %d.\n",
                             i_apptype, dnx_field_standard_1_static_apptype_map[i_apptype].opcode_id, mapped_opcode);
            }
            /** Check that the KBP opcode to contexts mapping agrees with the field mapping.*/
            SHR_IF_ERR_EXIT(dnx_field_map_apptype_to_fwd_contexts
                            (unit, i_apptype, &mapped_nof_contexts, mapped_fwd_contexts, mapped_acl_contexts));
            if (!sw_state_is_warm_boot(unit) && dnx_kbp_device_enabled(unit))
            {
                uint8 fwd_nof_contexts;
                kbp_mngr_fwd_acl_context_mapping_t
                    fwd_acl_ctx_mapping[DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];
                unsigned int fwd_context_idx;

                SHR_IF_ERR_EXIT(kbp_mngr_opcode_to_contexts_get
                                (unit, mapped_opcode, &fwd_nof_contexts, fwd_acl_ctx_mapping));
                if (mapped_nof_contexts != fwd_nof_contexts)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Apptype %d mapped alternatively to %d and %d FWD contexts.\n",
                                 i_apptype, mapped_nof_contexts, fwd_nof_contexts);
                }
                if (fwd_nof_contexts > DNX_DATA_MAX_FIELD_KBP_MAX_ACL_CONTEXT_NUM)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Apptype %d mapped to %d FWD contexts, maximum is %d.\n",
                                 i_apptype, fwd_nof_contexts, DNX_DATA_MAX_FIELD_KBP_MAX_ACL_CONTEXT_NUM);
                }
                for (fwd_context_idx = 0; fwd_context_idx < fwd_nof_contexts; fwd_context_idx++)
                {
                    if (fwd_acl_ctx_mapping[fwd_context_idx].nof_acl_contexts != 1)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "For Apptype %d opcode_id %d, FWD context %d, "
                                     "the number of ACL contexts is %d."
                                     "Only one ACL context per FWD context is supported.\n",
                                     i_apptype, mapped_opcode, fwd_acl_ctx_mapping[fwd_context_idx].fwd_context,
                                     fwd_acl_ctx_mapping[fwd_context_idx].nof_acl_contexts);
                    }
                    if (mapped_fwd_contexts[fwd_context_idx] != fwd_acl_ctx_mapping[fwd_context_idx].fwd_context)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "Apptype %d opcode %d, FWD context number %d is alternatively to %d and %d.\n",
                                     i_apptype, mapped_opcode, fwd_nof_contexts, mapped_fwd_contexts[fwd_context_idx],
                                     fwd_acl_ctx_mapping[fwd_context_idx].fwd_context);
                    }
                    if (mapped_acl_contexts[fwd_context_idx] != fwd_acl_ctx_mapping[fwd_context_idx].acl_contexts[0])
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "Apptype %d FWD context %d mapped alternatively to ACL contexts %d and %d.\n",
                                     i_apptype, mapped_fwd_contexts[fwd_context_idx],
                                     mapped_acl_contexts[fwd_context_idx],
                                     fwd_acl_ctx_mapping[fwd_context_idx].acl_contexts[0]);
                    }
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_map_kbp_apptype_opcode_init(unit));
    SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_init(unit));
    SHR_IF_ERR_EXIT(dnx_field_map_qual_init(unit));
    SHR_IF_ERR_EXIT(dnx_field_map_action_init(unit));
    SHR_IF_ERR_EXIT(dnx_field_map_vw_qual_action_init(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field.h
 */
shr_error_e
dnx_field_map_ffc_type_to_field(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_ffc_type_e ffc_type,
    dbal_fields_e * ffc_type_field_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(ffc_type_field_p, _SHR_E_PARAM, "ffc_type_field_p");
    DNX_FIELD_STAGE_VERIFY(stage);

    *ffc_type_field_p = dnx_field_map_stage_info[stage].ffc_type_field_a[ffc_type];

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field.h
 */
shr_error_e
dnx_field_map_stage_to_ffc_table(
    int unit,
    dnx_field_stage_e stage,
    dbal_tables_e * ffc_table_name_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ffc_table_name_p, _SHR_E_PARAM, "ffc_table_name_p");

    /*
     * TBD: Handle Initial ffc's
     */
    *ffc_table_name_p = dnx_field_map_stage_info[stage].ffc_table;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field.h
 */
shr_error_e
dnx_field_map_stage_to_ffc_instruction(
    int unit,
    dnx_field_stage_e stage,
    dbal_fields_e * ffc_instruction_name_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ffc_instruction_name_p, _SHR_E_PARAM, "ffc_table_name_p");

    *ffc_instruction_name_p = dnx_field_map_stage_info[stage].ffc_instruction;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field.h
 */
shr_error_e
dnx_field_map_stage_bcm_to_dnx(
    int unit,
    bcm_field_stage_t bcm_stage,
    dnx_field_stage_e * dnx_stage_p)
{
    dnx_field_stage_e dnx_stage;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_stage_p, _SHR_E_PARAM, "dnx_stage_p");

    if ((bcm_stage < bcmFieldStageFirst) || (bcm_stage >= bcmFieldStageCount))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid BCM Stage:%d \n", bcm_stage);
    }

    DNX_FIELD_STAGE_ITERATOR(dnx_stage)
    {
        if (dnx_field_map_stage_info[dnx_stage].bcm_stage == bcm_stage)
        {
            *dnx_stage_p = dnx_stage;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "BCM Stage:\"%s\" is not supported\n", dnx_field_bcm_stage_text(bcm_stage));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See procedure header in field.h
 */
shr_error_e
dnx_field_map_ctx_id_to_field(
    int unit,
    dnx_field_stage_e stage,
    dbal_fields_e * dbal_ctx_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dbal_ctx_id, _SHR_E_PARAM, "dbal_ctx_id");

    *dbal_ctx_id = dnx_field_map_stage_info[stage].ctx_id;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field.h
 */
shr_error_e
dnx_field_map_key_to_field(
    int unit,
    dnx_field_stage_e stage,
    dbal_fields_e * dbal_key)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dbal_key, _SHR_E_PARAM, "dbal_key");

    *dbal_key = dnx_field_map_stage_info[stage].key_field;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field.h
 */
shr_error_e
dnx_field_map_stage_dnx_to_bcm(
    int unit,
    dnx_field_stage_e dnx_stage,
    bcm_field_stage_t * bcm_stage_p)
{
    bcm_field_stage_t bcm_stage;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(bcm_stage_p, _SHR_E_PARAM, "bcm_stage_p");

    DNX_FIELD_STAGE_VERIFY(dnx_stage);

    bcm_stage = dnx_field_map_stage_info[dnx_stage].bcm_stage;

    if ((bcm_stage < bcmFieldStageFirst) || (bcm_stage >= bcmFieldStageCount))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "DNX Stage:\"%s\" has no BCM mapping\n", dnx_field_stage_text(unit, dnx_stage));
    }
    *bcm_stage_p = bcm_stage;
exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_group_type_bcm_to_dnx(
    int unit,
    bcm_field_group_type_t bcm_fg_type,
    dnx_field_group_type_e * dnx_fg_type_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_fg_type_p, _SHR_E_PARAM, "dnx_fg_type_p");

    switch (bcm_fg_type)
    {
        case bcmFieldGroupTypeTcam:
        {
            *dnx_fg_type_p = DNX_FIELD_GROUP_TYPE_TCAM;
            break;
        }
        case bcmFieldGroupTypeDirectTcam:
        {
            *dnx_fg_type_p = DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM;
            break;
        }
        case bcmFieldGroupTypeDirectExtraction:
        {
            *dnx_fg_type_p = DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION;
            break;
        }
        case bcmFieldGroupTypeExactMatch:
        {
            *dnx_fg_type_p = DNX_FIELD_GROUP_TYPE_EXEM;
            break;
        }

        case bcmFieldGroupTypeStateTable:
        {
            *dnx_fg_type_p = DNX_FIELD_GROUP_TYPE_STATE_TABLE;
            break;
        }
        case bcmFieldGroupTypeDirectMdb:
        {
            *dnx_fg_type_p = DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB;
            break;
        }
        case bcmFieldGroupTypeExternalTcam:
        {
            *dnx_fg_type_p = DNX_FIELD_GROUP_TYPE_KBP;
            break;
        }
        case bcmFieldGroupTypeConst:
        {
            *dnx_fg_type_p = DNX_FIELD_GROUP_TYPE_CONST;
            break;
        }
        default:
        {
            /*
             * None of the supported types
             */
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid BCM Field Group Type:%d \n", bcm_fg_type);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_group_type_dnx_to_bcm(
    int unit,
    dnx_field_group_type_e dnx_fg_type,
    bcm_field_group_type_t * bcm_fg_type_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_fg_type_p, _SHR_E_PARAM, "bcm_fg_type_p");

    switch (dnx_fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_TCAM:
        {
            *bcm_fg_type_p = bcmFieldGroupTypeTcam;
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
        {
            *bcm_fg_type_p = bcmFieldGroupTypeDirectTcam;
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION:
        {
            *bcm_fg_type_p = bcmFieldGroupTypeDirectExtraction;
            break;
        }
        case DNX_FIELD_GROUP_TYPE_EXEM:
        {
            *bcm_fg_type_p = bcmFieldGroupTypeExactMatch;
            break;
        }
        case DNX_FIELD_GROUP_TYPE_STATE_TABLE:
        {
            *bcm_fg_type_p = bcmFieldGroupTypeStateTable;
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB:
        {
            *bcm_fg_type_p = bcmFieldGroupTypeDirectMdb;
            break;
        }
        case DNX_FIELD_GROUP_TYPE_KBP:
        {
            *bcm_fg_type_p = bcmFieldGroupTypeExternalTcam;
            break;
        }
        case DNX_FIELD_GROUP_TYPE_CONST:
        {
            *bcm_fg_type_p = bcmFieldGroupTypeConst;
            break;
        }
        default:
        {
            /*
             * None of the supported types
             */
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid DNX Field Group Type:%d \n", dnx_fg_type);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_layer_type_bcm_to_dnx(
    int unit,
    bcm_field_layer_type_t bcm_layer_type,
    dbal_enum_value_field_layer_types_e * dnx_layer_type)
{
    static char *layer_type_names[bcmFieldLayerTypeCount] = BCM_FIELD_LAYER_TYPE_STRINGS;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_layer_type, _SHR_E_PARAM, "dnx_layer_type");

    if ((bcm_layer_type < 0) || (bcm_layer_type >= bcmFieldLayerTypeCount))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Layer Type:%d is out of range\n", bcm_layer_type);
    }
    switch (bcm_layer_type)
    {
        case bcmFieldLayerTypeEth:
            *dnx_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET;
            break;
        case bcmFieldLayerTypeIp4:
            *dnx_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_IPV4;
            break;
        case bcmFieldLayerTypeIp6:
            *dnx_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_IPV6;
            break;
        case bcmFieldLayerTypeMpls:
            *dnx_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_MPLS;
            break;
        case bcmFieldLayerTypeArp:
            *dnx_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_ARP;
            break;
        case bcmFieldLayerTypeFcoe:
            *dnx_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_FCOE;
            break;
        case bcmFieldLayerTypeTcp:
            *dnx_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_TCP;
            break;
        case bcmFieldLayerTypeUdp:
            *dnx_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_UDP;
            break;
        case bcmFieldLayerTypeBfdSingleHop:
            *dnx_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_BFD_SINGLE_HOP;
            break;
        case bcmFieldLayerTypeBfdMultiHop:
            *dnx_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_BFD_MULTI_HOP;
            break;
        case bcmFieldLayerTypeY1731:
            *dnx_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_Y_1731;
            break;
        case bcmFieldLayerTypeIcmp:
            *dnx_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_ICMP;
            break;
        case bcmFieldLayerTypeBierTi:
            *dnx_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_BIER_TI;
            break;
        case bcmFieldLayerTypeBierMpls:
            *dnx_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_BIER_MPLS;
            break;
        case bcmFieldLayerTypeRch:
            *dnx_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_RCH;
            break;
        case bcmFieldLayerTypePppoe:
            *dnx_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_PPPOE;
            break;
        case bcmFieldLayerTypeSrv6Endpoint:
            *dnx_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_ENDPOINT;
            break;
        case bcmFieldLayerTypeSrv6Beyond:
            *dnx_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_BEYOND;
            break;
        case bcmFieldLayerTypeIgmp:
            *dnx_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_IGMP;
            break;
        case bcmFieldLayerTypeIpAny:
            *dnx_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_IPVX;
            break;
        case bcmFieldLayerTypeIpt:
            *dnx_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_IPT;
            break;
        case bcmFieldLayerTypeTm:
            *dnx_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_ITMH_VAL;
            break;
        case bcmFieldLayerTypeTmLegacy:
            *dnx_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_ITMH_J1_VAL;
            break;
        case bcmFieldLayerTypeForwardingMPLS:
            *dnx_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_MPLS_UNTERM;
            break;
        case bcmFieldLayerTypeUnknown:
            *dnx_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_UNKNOWN;
            break;
        case bcmFieldLayerTypeSctp:
            *dnx_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_INGRESS_SCTP_EGRESS_FTMH;
            break;
        case bcmFieldLayerTypePtpGeneral:
            *dnx_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_PTP;
            break;
        case bcmFieldLayerTypeGtp:
            *dnx_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_GTP;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Layer Type:\"%s\"(%d) is not supported.\n",
                         layer_type_names[bcm_layer_type], bcm_layer_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_bank_allocation_mode_bcm_to_dnx(
    int unit,
    bcm_field_tcam_bank_allocation_mode_t bcm_bank_allocation_mode,
    dnx_field_tcam_bank_allocation_mode_e * dnx_bank_allocation_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (bcm_bank_allocation_mode)
    {
        case bcmFieldTcamBankAllocationModeAuto:
        {
            *dnx_bank_allocation_mode = DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_AUTO;
            break;
        }
        case bcmFieldTcamBankAllocationModeSelect:
        {
            *dnx_bank_allocation_mode = DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_SELECT;
            break;
        }
        case bcmFieldTcamBankAllocationModeSelectWithLocation:
        {
            *dnx_bank_allocation_mode = DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_SELECT_WITH_LOCATION;
            break;
        }
        default:
        {
            /*
             * None of the supported types
             */
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid BCM Bank Allocation Mode:%d \n", bcm_bank_allocation_mode);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_bank_allocation_mode_dnx_to_bcm(
    int unit,
    dnx_field_tcam_bank_allocation_mode_e dnx_bank_allocation_mode,
    bcm_field_tcam_bank_allocation_mode_t * bcm_bank_allocation_mode_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_bank_allocation_mode_p, _SHR_E_PARAM, "bcm_bank_allocation_mode_p");

    switch (dnx_bank_allocation_mode)
    {
        case DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_AUTO:
        {
            *bcm_bank_allocation_mode_p = bcmFieldTcamBankAllocationModeAuto;
            break;
        }
        case DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_SELECT:
        {
            *bcm_bank_allocation_mode_p = bcmFieldTcamBankAllocationModeSelect;
            break;
        }
        case DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_SELECT_WITH_LOCATION:
        {
            *bcm_bank_allocation_mode_p = bcmFieldTcamBankAllocationModeSelectWithLocation;
            break;
        }
        default:
        {
            /*
             * None of the supported types
             */
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid DNX Bank Allocation Mode:%d \n", dnx_bank_allocation_mode);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_context_compare_type_bcm_to_dnx(
    int unit,
    bcm_field_context_compare_type_t bcm_context_compare_type,
    dnx_field_context_compare_mode_e * dnx_context_compare_mode_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_context_compare_mode_p, _SHR_E_PARAM, "dnx_context_compare_mode_p");

    switch (bcm_context_compare_type)
    {
        case bcmFieldContextCompareTypeNone:
        {
            *dnx_context_compare_mode_p = DNX_FIELD_CONTEXT_COMPARE_MODE_NONE;
            break;
        }
        case bcmFieldContextCompareTypeSingle:
        {
            *dnx_context_compare_mode_p = DNX_FIELD_CONTEXT_COMPARE_MODE_SINGLE;
            break;
        }
        case bcmFieldContextCompareTypeDouble:
        {
            *dnx_context_compare_mode_p = DNX_FIELD_CONTEXT_COMPARE_MODE_DOUBLE;
            break;
        }
        default:
        {
            /*
             * None of the supported types
             */
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid BCM Context Compare Type:%d \n", bcm_context_compare_type);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_context_compare_type_dnx_to_bcm(
    int unit,
    dnx_field_context_compare_mode_e dnx_context_compare_mode,
    bcm_field_context_compare_type_t * bcm_context_compare_type_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_context_compare_type_p, _SHR_E_PARAM, "bcm_context_compare_type_p");

    switch (dnx_context_compare_mode)
    {
        case DNX_FIELD_CONTEXT_COMPARE_MODE_NONE:
        {
            *bcm_context_compare_type_p = bcmFieldContextCompareTypeNone;
            break;
        }
        case DNX_FIELD_CONTEXT_COMPARE_MODE_SINGLE:
        {
            *bcm_context_compare_type_p = bcmFieldContextCompareTypeSingle;
            break;
        }
        case DNX_FIELD_CONTEXT_COMPARE_MODE_DOUBLE:
        {
            *bcm_context_compare_type_p = bcmFieldContextCompareTypeDouble;
            break;
        }
        default:
        {
            /*
             * None of the supported types
             */
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid DNX Context Compare Mode:%d \n", dnx_context_compare_mode);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_layer_type_dnx_to_bcm(
    int unit,
    dbal_enum_value_field_layer_types_e dnx_layer_type,
    bcm_field_layer_type_t * bcm_layer_type_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_layer_type_p, _SHR_E_PARAM, "bcm_layer_type_p");

    switch (dnx_layer_type)
    {
        case DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeEth;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_IPV4:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeIp4;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_IPV6:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeIp6;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_MPLS:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeMpls;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_ARP:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeArp;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_FCOE:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeFcoe;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_TCP:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeTcp;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_UDP:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeUdp;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_BFD_SINGLE_HOP:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeBfdSingleHop;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_BFD_MULTI_HOP:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeBfdMultiHop;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_Y_1731:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeY1731;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_ICMP:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeIcmp;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_BIER_TI:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeBierTi;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_BIER_MPLS:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeBierMpls;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_RCH:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeRch;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_PPPOE:
        {
            *bcm_layer_type_p = bcmFieldLayerTypePppoe;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_ENDPOINT:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeSrv6Endpoint;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_BEYOND:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeSrv6Beyond;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_IGMP:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeIgmp;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_IPVX:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeIpAny;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_IPT:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeIpt;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_ITMH_VAL:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeTm;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_ITMH_J1_VAL:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeTmLegacy;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_MPLS_UNTERM:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeForwardingMPLS;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_UNKNOWN:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeUnknown;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_INGRESS_SCTP_EGRESS_FTMH:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeSctp;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_PTP:
        {
            *bcm_layer_type_p = bcmFieldLayerTypePtpGeneral;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_GTP:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeGtp;
            break;
        }
        default:
        {
            /** None of the supported types. */
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_vlan_format_dnx_to_bcm(
    int unit,
    dbal_enum_value_field_incoming_tag_structure_e dnx_vlan_format,
    uint32 *bcm_vlan_format_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_vlan_format_p, _SHR_E_PARAM, "bcm_vlan_format_p");

    switch (dnx_vlan_format)
    {
        case DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_UNTAGGED_1:
        {
            *bcm_vlan_format_p = BCM_FIELD_VLAN_FORMAT_UNTAGGED;
            break;
        }
        case DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_TAG_1:
        {
            *bcm_vlan_format_p = BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED;
            break;
        }
        case DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_C_TAG_1:
        {
            *bcm_vlan_format_p = BCM_FIELD_VLAN_FORMAT_INNER_TAGGED;
            break;
        }
        case DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_PRIORITY_C_TAG_1:
        {
            *bcm_vlan_format_p = BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED_VID_ZERO;
            break;
        }
        default:
        {
            /** None of the supported types. */
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_lookup_dbal_table_get(
    int unit,
    dnx_field_stage_e dnx_stage,
    dbal_tables_e * table_id_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(table_id_p, _SHR_E_PARAM, "table_id_p");

    DNX_FIELD_STAGE_VERIFY(dnx_stage);

    *table_id_p = dnx_field_map_stage_info[dnx_stage].lookup_enabler_table_id;
exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_compare_id_bcm_to_dnx(
    int unit,
    int compare_id,
    dnx_field_group_compare_id_e * dnx_compare_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_compare_id_p, _SHR_E_PARAM, "dnx_compare_id_p");

    switch (compare_id)
    {
        case BCM_FIELD_INVALID:
        {
            *dnx_compare_id_p = DNX_FIELD_GROUP_COMPARE_ID_NONE;
            break;
        }
        case 0:
        {
            *dnx_compare_id_p = DNX_FIELD_GROUP_COMPARE_ID_CMP_1;
            break;
        }
        case 1:
        {
            *dnx_compare_id_p = DNX_FIELD_GROUP_COMPARE_ID_CMP_2;
            break;
        }
        default:
        {
            /*
             * None of the supported compare Ids
             */
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid compare_id:%d \n", compare_id);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_context_param_bcm_to_dnx(
    int unit,
    bcm_field_stage_t bcm_stage,
    dnx_field_context_t context_id,
    bcm_field_context_param_info_t * bcm_context_param_set_p,
    dnx_field_dbal_entry_t * field_dbal_entry_p)
{
    dnx_field_context_param_t *dnx_context_param_set;
    dnx_field_stage_e dnx_stage;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_context_param_set_p, _SHR_E_PARAM, "bcm_context_param_set_p");
    SHR_NULL_CHECK(field_dbal_entry_p, _SHR_E_PARAM, "field_dbal_entry_p");

    /** Convert BCM to DNX Field Stage. And verify it on way*/
    SHR_IF_ERR_EXIT(dnx_field_map_stage_bcm_to_dnx(unit, bcm_stage, &dnx_stage));

    dnx_context_param_set =
        (dnx_field_context_param_t *) & context_param_set[bcm_context_param_set_p->param_type][bcm_stage];
    if (dnx_context_param_set->table_id == DBAL_TABLE_EMPTY)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Context Parameter Type %d for stage:%s is not supported",
                     bcm_context_param_set_p->param_type, dnx_field_bcm_stage_text(bcm_stage));
    }
    /*
     * First of all initialize to NULL
     */
    sal_memset(field_dbal_entry_p, 0, sizeof(*field_dbal_entry_p));
    /*
     * Fetch table id
     */
    field_dbal_entry_p->table_id = dnx_context_param_set->table_id;
    if (dnx_context_param_set->conversion_cb != NULL)
    {
        /*
         * Conversion callback will take care of filling dbal fields and their values
         */
        SHR_IF_ERR_EXIT(dnx_context_param_set->conversion_cb(unit, dnx_stage, context_id,
                                                             bcm_context_param_set_p->param_val, dnx_context_param_set,
                                                             field_dbal_entry_p));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "No Data Conversion CB for Context Parameter:\"%d\" stage:\"%s\"\n",
                     bcm_context_param_set_p->param_type, dnx_field_bcm_stage_text(bcm_stage));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_hash_func_bcm_to_dbal(
    int unit,
    bcm_field_context_hash_function_t bcm_hash_func,
    dbal_enum_value_field_context_hash_function_e * dbal_hash_func_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dbal_hash_func_p, _SHR_E_PARAM, "dbal_hash_func_p");
    switch (bcm_hash_func)
    {
        case bcmFieldContextHashFunctionFirstReserved:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_FIRST_RESERVED;
            break;
        }
        case bcmFieldContextHashFunctionSecondReserved:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_SECOND_RESERVED;
            break;
        }
        case bcmFieldContextHashFunctionThirdReserved:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_THIRD_RESERVED;
            break;
        }
        case bcmFieldContextHashFunctionCrc16Bisync:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_BISYNC;
            break;
        }
        case bcmFieldContextHashFunctionCrc16Xor1:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR1;
            break;
        }
        case bcmFieldContextHashFunctionCrc16Xor2:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR2;
            break;
        }
        case bcmFieldContextHashFunctionCrc16Xor4:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR4;
            break;
        }
        case bcmFieldContextHashFunctionCrc16Xor8:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR8;
            break;
        }
        case bcmFieldContextHashFunctionXor16:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_XOR16;
            break;
        }
        case bcmFieldContextHashFunctionCrc16Ccitt:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_CCITT;
            break;
        }
        case bcmFieldContextHashFunctionCrc32ALow:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_ALOW;
            break;
        }
        case bcmFieldContextHashFunctionCrc32Ahigh:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_AHIGH;
            break;
        }
        case bcmFieldContextHashFunctionCrc32BLow:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_BLOW;
            break;
        }
        case bcmFieldContextHashFunctionCrc32BHigh:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_BHIGH;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid Hash Function type: %d . Valid ranges from: %d to %d \n",
                         bcm_hash_func, bcmFieldContextHashFunctionFirst, (bcmFieldContextHashFunctionCount - 1));
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_hash_func_dbal_to_bcm(
    int unit,
    dbal_enum_value_field_context_hash_function_e dbal_hash_func,
    bcm_field_context_hash_function_t * bcm_hash_func_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_hash_func_p, _SHR_E_PARAM, "bcm_hash_func_p");
    switch (dbal_hash_func)
    {
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_FIRST_RESERVED:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionFirstReserved;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_SECOND_RESERVED:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionSecondReserved;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_THIRD_RESERVED:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionThirdReserved;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_BISYNC:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionCrc16Bisync;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR1:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionCrc16Xor1;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR2:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionCrc16Xor2;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR4:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionCrc16Xor4;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR8:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionCrc16Xor8;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_XOR16:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionXor16;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_CCITT:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionCrc16Ccitt;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_ALOW:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionCrc32ALow;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_AHIGH:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionCrc32Ahigh;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_BLOW:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionCrc32BLow;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_BHIGH:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionCrc32BHigh;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid Hash Function type: %d. \n", dbal_hash_func);
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_hash_lb_key_bcm_to_dbal(
    int unit,
    bcm_field_context_hash_action_key_t bcm_hash_lb_key,
    dbal_enum_value_field_field_hash_lb_key_e * dbal_hash_lb_key_p)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dbal_hash_lb_key_p, _SHR_E_PARAM, "dbal_hash_lb_key_p");
    switch (bcm_hash_lb_key)
    {
        case bcmFieldContextHashActionKeyEcmpLbKey0:
        {
            *dbal_hash_lb_key_p = DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_ECMP0_LB;
            break;
        }
        case bcmFieldContextHashActionKeyEcmpLbKey1:
        {
            *dbal_hash_lb_key_p = DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_ECMP1_LB;
            break;
        }
        case bcmFieldContextHashActionKeyEcmpLbKey2:
        {
            *dbal_hash_lb_key_p = DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_ECMP2_LB;
            break;
        }
        case bcmFieldContextHashActionKeyNetworkLbKey:
        {
            *dbal_hash_lb_key_p = DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_NWK_LB;
            break;
        }
        case bcmFieldContextHashActionKeyLagLbKey:
        {
            *dbal_hash_lb_key_p = DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_LAG_LB;
            break;
        }
        case bcmFieldContextHashActionKeyAdditionalLbKey:
        {
            *dbal_hash_lb_key_p = DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_ADDITIONAL_LB;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid Hashing Load Balancing Key type: %d ."
                         " Valid values from: %d to %d . \n",
                         bcm_hash_lb_key, bcmFieldContextHashActionKeyFirst, (bcmFieldContextHashActionKeyCount - 1));
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_hash_lb_key_dbal_to_bcm(
    int unit,
    dbal_enum_value_field_field_hash_lb_key_e dbal_hash_lb_key,
    bcm_field_context_hash_action_key_t * bcm_hash_lb_key_p)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_hash_lb_key_p, _SHR_E_PARAM, "bcm_hash_lb_key_p");
    switch (dbal_hash_lb_key)
    {
        case DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_ECMP0_LB:
        {
            *bcm_hash_lb_key_p = bcmFieldContextHashActionKeyEcmpLbKey0;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_ECMP1_LB:
        {
            *bcm_hash_lb_key_p = bcmFieldContextHashActionKeyEcmpLbKey1;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_ECMP2_LB:
        {
            *bcm_hash_lb_key_p = bcmFieldContextHashActionKeyEcmpLbKey2;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_NWK_LB:
        {
            *bcm_hash_lb_key_p = bcmFieldContextHashActionKeyNetworkLbKey;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_LAG_LB:
        {
            *bcm_hash_lb_key_p = bcmFieldContextHashActionKeyLagLbKey;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_ADDITIONAL_LB:
        {
            *bcm_hash_lb_key_p = bcmFieldContextHashActionKeyAdditionalLbKey;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid Hashing Load Balancing Key type: %d. \n", dbal_hash_lb_key);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_hash_action_bcm_to_dbal(
    int unit,
    bcm_field_context_hash_action_value_t bcm_hash_action,
    dbal_enum_value_field_hash_action_e * dbal_hash_action_p)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dbal_hash_action_p, _SHR_E_PARAM, "dbal_hash_action_p");
    switch (bcm_hash_action)
    {
        case bcmFieldContextHashActionValueNone:
        {
            *dbal_hash_action_p = DBAL_ENUM_FVAL_HASH_ACTION_NONE;
            break;
        }
        case bcmFieldContextHashActionValueReplaceCrc:
        {
            *dbal_hash_action_p = DBAL_ENUM_FVAL_HASH_ACTION_REPLACE_CRC;
            break;
        }
        case bcmFieldContextHashActionValueAugmentCrc:
        {
            *dbal_hash_action_p = DBAL_ENUM_FVAL_HASH_ACTION_AUGMENT_CRC;
            break;
        }
        case bcmFieldContextHashActionValueAugmentKey:
        {
            *dbal_hash_action_p = DBAL_ENUM_FVAL_HASH_ACTION_AUGMENT_KEY;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid Hash Action type: %d. Valid values from: %d to %d . \n",
                         bcm_hash_action,
                         bcmFieldContextHashActionValueFirst, (bcmFieldContextHashActionValueCount - 1));
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_hash_action_dbal_to_bcm(
    int unit,
    dbal_enum_value_field_hash_action_e dbal_hash_action,
    bcm_field_context_hash_action_value_t * bcm_hash_action_p)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_hash_action_p, _SHR_E_PARAM, "bcm_hash_action_p");
    switch (dbal_hash_action)
    {
        case DBAL_ENUM_FVAL_HASH_ACTION_NONE:
        {
            *bcm_hash_action_p = bcmFieldContextHashActionValueNone;
            break;
        }
        case DBAL_ENUM_FVAL_HASH_ACTION_REPLACE_CRC:
        {
            *bcm_hash_action_p = bcmFieldContextHashActionValueReplaceCrc;
            break;
        }
        case DBAL_ENUM_FVAL_HASH_ACTION_AUGMENT_CRC:
        {
            *bcm_hash_action_p = bcmFieldContextHashActionValueAugmentCrc;
            break;
        }
        case DBAL_ENUM_FVAL_HASH_ACTION_AUGMENT_KEY:
        {
            *bcm_hash_action_p = bcmFieldContextHashActionValueAugmentKey;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid Hash Action type: %d. \n", dbal_hash_action);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_crc_select_bcm_to_dbal(
    int unit,
    bcm_field_crc_select_t bcm_crc_select,
    dbal_enum_value_field_crc_select_e * dbal_crc_select_p)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dbal_crc_select_p, _SHR_E_PARAM, "dbal_crc_select_p");
    switch (bcm_crc_select)
    {
        case bcmFieldCrcSelectCrc16P0x10039:
        {
            *dbal_crc_select_p = DBAL_ENUM_FVAL_CRC_SELECT_CRC16_0X10039;
            break;
        }
        case bcmFieldCrcSelectCrc16P0x100d7:
        {
            *dbal_crc_select_p = DBAL_ENUM_FVAL_CRC_SELECT_CRC16_0X100D7;
            break;
        }
        case bcmFieldCrcSelectCrc16P0x1015d:
        {
            *dbal_crc_select_p = DBAL_ENUM_FVAL_CRC_SELECT_CRC16_0X1015D;
            break;
        }
        case bcmFieldCrcSelectCrc16P0x10939:
        {
            *dbal_crc_select_p = DBAL_ENUM_FVAL_CRC_SELECT_CRC16_0X10939;
            break;
        }
        case bcmFieldCrcSelectCrc16P0x109e7:
        {
            *dbal_crc_select_p = DBAL_ENUM_FVAL_CRC_SELECT_CRC16_0X109E7;
            break;
        }
        case bcmFieldCrcSelectCrc16P0x10ac5:
        {
            *dbal_crc_select_p = DBAL_ENUM_FVAL_CRC_SELECT_CRC16_0X10AC5;
            break;
        }
        case bcmFieldCrcSelectCrc16P0x1203d:
        {
            *dbal_crc_select_p = DBAL_ENUM_FVAL_CRC_SELECT_CRC16_0X1203D;
            break;
        }
        case bcmFieldCrcSelectCrc16P0x12105:
        {
            *dbal_crc_select_p = DBAL_ENUM_FVAL_CRC_SELECT_CRC16_0x12105;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid CRC Select type: %d. Valid values from: %d to %d . \n",
                         bcm_crc_select, bcmFieldCrcSelectFirst, (bcmFieldCrcSelectCount - 1));
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_crc_select_dbal_to_bcm(
    int unit,
    dbal_enum_value_field_crc_select_e dbal_crc_select,
    bcm_field_crc_select_t * bcm_crc_select_p)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_crc_select_p, _SHR_E_PARAM, "bcm_crc_select_p");
    switch (dbal_crc_select)
    {
        case DBAL_ENUM_FVAL_CRC_SELECT_CRC16_0X10039:
        {
            *bcm_crc_select_p = bcmFieldCrcSelectCrc16P0x10039;
            break;
        }
        case DBAL_ENUM_FVAL_CRC_SELECT_CRC16_0X100D7:
        {
            *bcm_crc_select_p = bcmFieldCrcSelectCrc16P0x100d7;
            break;
        }
        case DBAL_ENUM_FVAL_CRC_SELECT_CRC16_0X1015D:
        {
            *bcm_crc_select_p = bcmFieldCrcSelectCrc16P0x1015d;
            break;
        }
        case DBAL_ENUM_FVAL_CRC_SELECT_CRC16_0X10939:
        {
            *bcm_crc_select_p = bcmFieldCrcSelectCrc16P0x10939;
            break;
        }
        case DBAL_ENUM_FVAL_CRC_SELECT_CRC16_0X109E7:
        {
            *bcm_crc_select_p = bcmFieldCrcSelectCrc16P0x109e7;
            break;
        }
        case DBAL_ENUM_FVAL_CRC_SELECT_CRC16_0X10AC5:
        {
            *bcm_crc_select_p = bcmFieldCrcSelectCrc16P0x10ac5;
            break;
        }
        case DBAL_ENUM_FVAL_CRC_SELECT_CRC16_0X1203D:
        {
            *bcm_crc_select_p = bcmFieldCrcSelectCrc16P0x1203d;
            break;
        }
        case DBAL_ENUM_FVAL_CRC_SELECT_CRC16_0x12105:
        {
            *bcm_crc_select_p = bcmFieldCrcSelectCrc16P0x12105;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid CRC Select type: %d. Valid values from: %d to %d . \n",
                         dbal_crc_select, 0, (DBAL_NOF_ENUM_CRC_SELECT_VALUES - 1));
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_range_type_info_legacy(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_range_type_e dnx_range_type,
    dnx_field_map_range_type_info_t * range_type_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(range_type_info_p, _SHR_E_PARAM, "range_type_info_p");

    if (range_map_legacy[dnx_range_type][field_stage].table_id == DBAL_TABLE_EMPTY)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Range Type %d for stage:%s is not supported",
                     dnx_range_type, dnx_field_bcm_stage_text(field_stage));
    }
    /**
     * First initialize range_type_info_p
     */
    sal_memset(range_type_info_p, 0, sizeof(*range_type_info_p));
    /**
     * Fetch table id
     */
    range_type_info_p->table_id = range_map_legacy[dnx_range_type][field_stage].table_id;
    /**
     *  Fill the dbal fields.
     */
    range_type_info_p->range_id_dbal_field = range_map_legacy[dnx_range_type][field_stage].range_id_dbal_field;
    range_type_info_p->range_min_dbal_field = range_map_legacy[dnx_range_type][field_stage].range_min_dbal_field;
    range_type_info_p->range_max_dbal_field = range_map_legacy[dnx_range_type][field_stage].range_max_dbal_field;

    /*
     * Indicate whether the HW returns the range ID or a bitmap of range IDs.
     * Used to determine the default value for the range.
     */
    range_type_info_p->id_not_bitmap = range_map_legacy[dnx_range_type][field_stage].id_not_bitmap;

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_range_type_bcm_to_dnx_int(
    int unit,
    bcm_field_range_type_t bcm_range_type,
    dnx_field_range_type_e * dnx_range_type_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dnx_range_type_p, _SHR_E_PARAM, "dnx_range_type_p");

    switch (bcm_range_type)
    {
        case bcmFieldRangeTypeL4SrcPort:
        {
            (*dnx_range_type_p) = DNX_FIELD_RANGE_TYPE_L4_SRC_PORT;
            break;
        }
        case bcmFieldRangeTypeL4DstPort:
        {
            (*dnx_range_type_p) = DNX_FIELD_RANGE_TYPE_L4_DST_PORT;
            break;
        }
        case bcmFieldRangeTypeOutVport:
        {
            (*dnx_range_type_p) = DNX_FIELD_RANGE_TYPE_OUT_LIF;
            break;
        }
        case bcmFieldRangeTypePacketHeaderSize:
        {
            (*dnx_range_type_p) = DNX_FIELD_RANGE_TYPE_PKT_HDR_SIZE;
            break;
        }
        case bcmFieldRangeTypeL4OpsPacketHeaderSize:
        {
            (*dnx_range_type_p) = DNX_FIELD_RANGE_TYPE_L4OPS_PKT_HDR_SIZE;
            break;
        }
        case bcmFieldRangeTypeInTTL:
        {
            (*dnx_range_type_p) = DNX_FIELD_RANGE_TYPE_IN_TTL;
            break;
        }
        case bcmFieldRangeTypeUserDefined1Low:
        {
            (*dnx_range_type_p) = DNX_FIELD_RANGE_TYPE_FFC1_LOW;
            break;
        }
        case bcmFieldRangeTypeUserDefined1High:
        {
            (*dnx_range_type_p) = DNX_FIELD_RANGE_TYPE_FFC1_HIGH;
            break;
        }
        case bcmFieldRangeTypeUserDefined2Low:
        {
            (*dnx_range_type_p) = DNX_FIELD_RANGE_TYPE_FFC2_LOW;
            break;
        }
        case bcmFieldRangeTypeUserDefined2High:
        {
            (*dnx_range_type_p) = DNX_FIELD_RANGE_TYPE_FFC2_HIGH;
            break;
        }
        default:
        {
            /**
             * None of the supported types
             */
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_range_type_bcm_to_dnx(
    int unit,
    bcm_field_range_type_t bcm_range_type,
    dnx_field_range_type_e * dnx_range_type_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dnx_range_type_p, _SHR_E_PARAM, "dnx_range_type_p");

    SHR_IF_ERR_EXIT_WITH_LOG(dnx_field_map_range_type_bcm_to_dnx_int(unit, bcm_range_type, dnx_range_type_p),
                             "Invalid Range Type:%d.%s%s\n", bcm_range_type, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_range_type_dnx_to_bcm(
    int unit,
    dnx_field_range_type_e dnx_range_type,
    bcm_field_range_type_t * bcm_range_type_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(bcm_range_type_p, _SHR_E_PARAM, "bcm_range_type_p");

    switch (dnx_range_type)
    {
        case DNX_FIELD_RANGE_TYPE_L4_SRC_PORT:
        {
            (*bcm_range_type_p) = bcmFieldRangeTypeL4SrcPort;
            break;
        }
        case DNX_FIELD_RANGE_TYPE_L4_DST_PORT:
        {
            (*bcm_range_type_p) = bcmFieldRangeTypeL4DstPort;
            break;
        }
        case DNX_FIELD_RANGE_TYPE_OUT_LIF:
        {
            (*bcm_range_type_p) = bcmFieldRangeTypeOutVport;
            break;
        }
        case DNX_FIELD_RANGE_TYPE_PKT_HDR_SIZE:
        {
            (*bcm_range_type_p) = bcmFieldRangeTypePacketHeaderSize;
            break;
        }
        case DNX_FIELD_RANGE_TYPE_L4OPS_PKT_HDR_SIZE:
        {
            (*bcm_range_type_p) = bcmFieldRangeTypeL4OpsPacketHeaderSize;
            break;
        }
        case DNX_FIELD_RANGE_TYPE_IN_TTL:
        {
            (*bcm_range_type_p) = bcmFieldRangeTypeInTTL;
            break;
        }
        case DNX_FIELD_RANGE_TYPE_FFC1_LOW:
        {
            (*bcm_range_type_p) = bcmFieldRangeTypeUserDefined1Low;
            break;
        }
        case DNX_FIELD_RANGE_TYPE_FFC1_HIGH:
        {
            (*bcm_range_type_p) = bcmFieldRangeTypeUserDefined1High;
            break;
        }
        case DNX_FIELD_RANGE_TYPE_FFC2_LOW:
        {
            (*bcm_range_type_p) = bcmFieldRangeTypeUserDefined2Low;
            break;
        }
        case DNX_FIELD_RANGE_TYPE_FFC2_HIGH:
        {
            (*bcm_range_type_p) = bcmFieldRangeTypeUserDefined2High;
            break;
        }
        default:
        {
            /**
             * None of the supported types
             */
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid Range Type:%d \n", dnx_range_type);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_system_header_profile_bcm_to_dnx(
    int unit,
    bcm_field_system_header_profile_t bcm_sys_hdr_profile,
    dnx_field_context_sys_hdr_profile_e * dnx_sys_hdr_profile_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_sys_hdr_profile_p, _SHR_E_PARAM, "dnx_sys_hdr_profile_p");

    switch (bcm_sys_hdr_profile)
    {
        case bcmFieldSystemHeaderProfileNone:
        {
            *dnx_sys_hdr_profile_p = DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_EMPTY;
            break;
        }
        case bcmFieldSystemHeaderProfileFtmh:
        {
            *dnx_sys_hdr_profile_p = DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH;
            break;
        }
        case bcmFieldSystemHeaderProfileFtmhPph:
        {
            *dnx_sys_hdr_profile_p = DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH_PPH;
            break;
        }
        case bcmFieldSystemHeaderProfileFtmhTsh:
        {
            *dnx_sys_hdr_profile_p = DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH_TSH;
            break;
        }
        case bcmFieldSystemHeaderProfileFtmhTshPph:
        {
            *dnx_sys_hdr_profile_p = DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH_TSH_PPH;
            break;
        }
        case bcmFieldSystemHeaderProfileFtmhTshPphUdh:
        {
            *dnx_sys_hdr_profile_p = DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH_TSH_PPH_UDH;
            break;
        }
        case bcmFieldSystemHeaderProfileFtmhUdh:
        {
            *dnx_sys_hdr_profile_p = DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH_UDH;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "BCM System Header Profile:\"%d\" is not supported\n", bcm_sys_hdr_profile);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_apptype_to_opcode_bcm_to_dnx_int(
    int unit,
    bcm_field_AppType_t app_type,
    dbal_enum_value_field_kbp_fwd_opcode_e * opcode_id_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(opcode_id_p, _SHR_E_PARAM, "opcode_id_p");

    if (DNX_FIELD_APPTYPE_IS_USER_DEF(unit, app_type))
    {
        int relative_dyn_apptype = app_type - dnx_data_field.kbp.apptype_user_1st_get(unit);
        uint8 is_alloc;
        uint8 opcode_id_uint8;
        SHR_IF_ERR_EXIT(dnx_algo_field_context_apptype_is_allocated(unit, app_type, &is_alloc));
        if (is_alloc == FALSE)
        {
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }
        SHR_IF_ERR_EXIT(dnx_field_apptype_sw.user_def_info.opcode_id.get(unit, relative_dyn_apptype, &opcode_id_uint8));
        (*opcode_id_p) = opcode_id_uint8;
    }
    else
    {
        uint8 is_standard_image;
        dbal_enum_value_field_kbp_fwd_opcode_e opcode_id;

        SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_standard_image));
        if ((int) app_type < 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Apptype (%d) cannot be negative.\n", app_type);
        }
        if (is_standard_image)
        {
            /** For standard_1 use dnx_field_standard_1_static_apptype_map to convert apptype to opcode.*/
            if (app_type >= bcmFieldAppTypeCount)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Apptype %d is out of range. ranges are %d-%d for static Apptypes, and "
                             "%d-%d for dynamic.\n",
                             app_type, 0, bcmFieldAppTypeCount - 1,
                             dnx_data_field.kbp.apptype_user_1st_get(unit),
                             dnx_data_field.kbp.apptype_user_1st_get(unit) +
                             dnx_data_field.kbp.apptype_user_nof_get(unit) - 1);
            }
            opcode_id = dnx_field_standard_1_static_apptype_map[app_type].opcode_id;
            if (opcode_id == DBAL_ENUM_FVAL_KBP_FWD_OPCODE_INVALID)
            {
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                SHR_EXIT();
            }
            if (dnx_field_static_opcode_info[opcode_id].in_use == FALSE)
            {
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                SHR_EXIT();
            }
            (*opcode_id_p) = opcode_id;
        }
        else
        {
            shr_error_e rv;
            uint32 opcode_hw_value;
            /** For non standard_1, Apptype==opcode.*/
            if (app_type >= ((int) DBAL_NOF_ENUM_KBP_FWD_OPCODE_VALUES))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Apptype %d is out of range. ranges are %d-%d for static Apptypes, and "
                             "%d-%d for dynamic.\n",
                             app_type, 0, DBAL_NOF_ENUM_KBP_FWD_OPCODE_VALUES,
                             dnx_data_field.kbp.apptype_user_1st_get(unit),
                             dnx_data_field.kbp.apptype_user_1st_get(unit) +
                             dnx_data_field.kbp.apptype_user_nof_get(unit) - 1);
            }
            rv = dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_KBP_FWD_OPCODE, app_type, &opcode_hw_value);
            if (rv == _SHR_E_PARAM)
            {
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                SHR_EXIT();
            }
            else
            {
                SHR_IF_ERR_EXIT(rv);
            }
            (*opcode_id_p) = app_type;
        }

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_apptype_to_opcode_bcm_to_dnx(
    int unit,
    bcm_field_AppType_t app_type,
    dbal_enum_value_field_kbp_fwd_opcode_e * opcode_id_p)
{
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(opcode_id_p, _SHR_E_PARAM, "opcode_id_p");

    rv = dnx_field_map_apptype_to_opcode_bcm_to_dnx_int(unit, app_type, opcode_id_p);
    if (rv == _SHR_E_NOT_FOUND)
    {
        if (app_type >= dnx_data_field.kbp.apptype_user_1st_get(unit) &&
            app_type < dnx_data_field.kbp.apptype_user_1st_get(unit) + dnx_data_field.kbp.apptype_user_nof_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Dynamic Apptype %d is not allocated.\n", app_type);
        }
        else
        {
            uint8 is_standard_image;

            SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_standard_image));
            if (is_standard_image)
            {
                dbal_enum_value_field_kbp_fwd_opcode_e opcode_id;
                opcode_id = dnx_field_standard_1_static_apptype_map[app_type].opcode_id;
                if (opcode_id == DBAL_ENUM_FVAL_KBP_FWD_OPCODE_INVALID ||
                    dnx_field_static_opcode_info[opcode_id].is_valid == FALSE)
                {
                    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Unmapped predefined apptype %d.\n", app_type);
                }
                else if (dnx_field_static_opcode_info[opcode_id].in_use == FALSE)
                {
                    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Predefined apptype \"%s\" (%d) is mapped but not used.\n",
                                 dnx_field_bcm_apptype_text(app_type), app_type);
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Predefined apptype \"%s\" (%d) is mapped and used but was not found.\n",
                                 dnx_field_bcm_apptype_text(app_type), app_type);
                }
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Predefined apptype %d does not exist.\n", app_type);
            }
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_opcode_to_apptype_dnx_to_bcm(
    int unit,
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_id,
    bcm_field_AppType_t * app_type_p)
{
    uint32 nof_user_apptypes;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(app_type_p, _SHR_E_PARAM, "app_type_p");

    nof_user_apptypes = dnx_data_field.kbp.apptype_user_nof_get(unit);

    

    if (opcode_id > (sizeof(dnx_field_static_opcode_info) / sizeof(dnx_field_static_opcode_info[0])))
    {
        bcm_field_AppType_t app_type_index;
        uint8 is_app_type_found = FALSE;
        uint8 opcode_id_uint8;
        for (app_type_index = 0; app_type_index < nof_user_apptypes; app_type_index++)
        {
            SHR_IF_ERR_EXIT(dnx_field_apptype_sw.user_def_info.opcode_id.get(unit, app_type_index, &opcode_id_uint8));
            if (opcode_id_uint8 == opcode_id)
            {
                (*app_type_p) = dnx_data_field.kbp.apptype_user_1st_get(unit) + app_type_index;
                is_app_type_found = TRUE;
                break;
            }
        }

        if (!is_app_type_found)
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE, "Dynamic opcode_id %d not found.%s%s%s\r\n", opcode_id, EMPTY, EMPTY, EMPTY);
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }
    }
    else
    {
        if (opcode_id < 0
            || opcode_id >= (sizeof(dnx_field_static_opcode_info) / sizeof(dnx_field_static_opcode_info[0])))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Static opcode_id %d is out of range. Maximum %d.\n",
                         opcode_id,
                         (int) ((sizeof(dnx_field_static_opcode_info) / sizeof(dnx_field_static_opcode_info[0])) - 1));
        }

        if (!dnx_field_static_opcode_info[opcode_id].is_valid)
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE, "Static opcode_id %d not found.%s%s%s\r\n", opcode_id, EMPTY, EMPTY, EMPTY);
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }
        if (!dnx_field_static_opcode_info[opcode_id].in_use)
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE, "Static opcode_id %d not used by any FWD context.%s%s%s\r\n",
                         opcode_id, EMPTY, EMPTY, EMPTY);
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }

        (*app_type_p) = dnx_field_static_opcode_info[opcode_id].app_type;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_opcode_to_profile_get(
    int unit,
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_id,
    uint32 *cs_profile_id_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(cs_profile_id_p, _SHR_E_PARAM, "cs_profile_id_p");

    

    if (opcode_id < 0 || opcode_id >= (sizeof(dnx_field_static_opcode_info) / sizeof(dnx_field_static_opcode_info[0])))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "opcode_id %d is out of range. Maximum %d.\n",
                     opcode_id,
                     (int) ((sizeof(dnx_field_static_opcode_info) / sizeof(dnx_field_static_opcode_info[0])) - 1));
    }

    if (dnx_field_static_opcode_info[opcode_id].is_valid == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "opcode_id %d not found.\n", opcode_id);
    }
    if (dnx_field_static_opcode_info[opcode_id].in_use == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "opcode_id %d not used by any FWD context.\n", opcode_id);
    }

    (*cs_profile_id_p) = dnx_field_static_opcode_info[opcode_id].cs_profile_id;

    /*
     * Sanity check.
     */
    if ((*cs_profile_id_p) <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "opcode_id %d has no CS profile allocated.\n", opcode_id);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_apptype_to_profile_get(
    int unit,
    bcm_field_AppType_t app_type,
    uint32 *cs_profile_id_p)
{
    
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_id;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(cs_profile_id_p, _SHR_E_PARAM, "cs_profile_id_p");

    SHR_IF_ERR_EXIT(dnx_field_map_apptype_to_opcode_bcm_to_dnx(unit, app_type, &opcode_id));

    if (app_type >= 0 && app_type < bcmFieldAppTypeCount)
    {
        /** Static Apptypes.*/
        SHR_IF_ERR_EXIT(dnx_field_map_opcode_to_profile_get(unit, opcode_id, cs_profile_id_p));
    }
    else
    {
        int relative_dyn_apptype = app_type - dnx_data_field.kbp.apptype_user_1st_get(unit);
        uint8 profile_id_uint8;

        SHR_IF_ERR_EXIT(dnx_field_apptype_sw.user_def_info.
                        profile_id.get(unit, relative_dyn_apptype, &profile_id_uint8));
        (*cs_profile_id_p) = profile_id_uint8;

        /*
         * Sanity check.
         */
        if ((*cs_profile_id_p) <= 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Dynamic apptype %d has no CS profile allocated.\n", app_type);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_profile_to_apptype_get(
    int unit,
    uint32 cs_profile_id,
    bcm_field_AppType_t * app_type_p)
{
    uint8 is_alloc;
    int found;
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_id;
    uint32 cs_profile_id_get;
    uint8 cs_profile_id_get_uint8;
    unsigned int app_type_index;
    int nof_user_defined_apptypes;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(app_type_p, _SHR_E_PARAM, "app_type_p");

    /*
     * Check if the profile is allocated.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_kbp_opcode_cs_profile_id_is_allocated(unit, cs_profile_id, &is_alloc));
    if (is_alloc == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Profile ID %d is not allocated.\n", cs_profile_id);
    }

    found = FALSE;

    /*
     * Search all Predefined Apptypes.
     */
    DNX_FIELD_KBP_OPCODE_IN_USE_ITERATOR(opcode_id)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_opcode_to_profile_get(unit, opcode_id, &cs_profile_id_get));
        if (cs_profile_id == cs_profile_id_get)
        {
            SHR_IF_ERR_EXIT(dnx_field_map_opcode_to_apptype_dnx_to_bcm(unit, opcode_id, app_type_p));
            found = TRUE;
            break;
        }
    }

    /*
     * Search all user defined Apptypes if not found in predefined.
     */
    if (found == FALSE)
    {
        nof_user_defined_apptypes = dnx_data_field.kbp.apptype_user_nof_get(unit);
        for (app_type_index = 0; app_type_index < nof_user_defined_apptypes; app_type_index++)
        {
            SHR_IF_ERR_EXIT(dnx_field_apptype_sw.user_def_info.
                            profile_id.get(unit, app_type_index, &cs_profile_id_get_uint8));
            if (cs_profile_id == cs_profile_id_get_uint8)
            {
                (*app_type_p) = dnx_data_field.kbp.apptype_user_1st_get(unit) + app_type_index;
                found = TRUE;
                break;
            }
        }
    }

    /** Sanity check.*/
    if (found == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Profile ID %d is allocated but not used.\n", cs_profile_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_apptype_to_fwd_contexts(
    int unit,
    bcm_field_AppType_t apptype,
    int *nof_contexts_p,
    dnx_field_context_t fwd_contexts[DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE],
    dnx_field_context_t acl_contexts[DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE])
{
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_id;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(nof_contexts_p, _SHR_E_PARAM, "nof_contexts_p");
    SHR_NULL_CHECK(fwd_contexts, _SHR_E_PARAM, "fwd_contexts");
    SHR_NULL_CHECK(acl_contexts, _SHR_E_PARAM, "acl_contexts");

    /*
     * Check that the apptype exists, and get the opcode for the user defined case.
     */
    SHR_IF_ERR_EXIT(dnx_field_map_apptype_to_opcode_bcm_to_dnx(unit, apptype, &opcode_id));

    if (apptype >= 0 && apptype < bcmFieldAppTypeCount)
    {
        /*
         * Predefined Apptype. Take contexts from SW state.
         */
        int context_index;
        uint8 fwd_context;
        uint8 nof_contexts;

        SHR_IF_ERR_EXIT(dnx_field_apptype_sw.predef_info.nof_context_ids.get(unit, apptype, &nof_contexts));
        if (nof_contexts > DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Number of FWD contexts for apptype %d (\"%s\") is %d, maximum is %d.\n",
                         apptype, dnx_field_bcm_apptype_text(apptype), nof_contexts,
                         DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE);
        }
        for (context_index = 0; context_index < nof_contexts; context_index++)
        {
            SHR_IF_ERR_EXIT(dnx_field_apptype_sw.predef_info.
                            context_ids.get(unit, apptype, context_index, &fwd_context));
            fwd_contexts[context_index] = fwd_context;
            acl_contexts[context_index] = fwd_context;
        }
        (*nof_contexts_p) = nof_contexts;
    }
    else
    {
        /*
         * User defined Apptype. Take contexts form KBP module.
         */
        uint8 fwd_nof_contexts;
        kbp_mngr_fwd_acl_context_mapping_t
            fwd_acl_ctx_mapping[DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];
        unsigned int fwd_context_idx;

        SHR_IF_ERR_EXIT(kbp_mngr_opcode_to_contexts_get(unit, opcode_id, &fwd_nof_contexts, fwd_acl_ctx_mapping));
        if (fwd_nof_contexts > DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Number of FWD contexts for apptype %d (\"%s\") is %d, maximum is %d.\n",
                         apptype, dnx_field_bcm_apptype_text(apptype), fwd_nof_contexts,
                         DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE);
        }
        for (fwd_context_idx = 0; fwd_context_idx < fwd_nof_contexts; fwd_context_idx++)
        {
            fwd_contexts[fwd_context_idx] = fwd_acl_ctx_mapping[fwd_context_idx].fwd_context;
            if (fwd_acl_ctx_mapping[fwd_context_idx].nof_acl_contexts != 1)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "For opcode_id %d, FWD context %d, the number of ACL contexts is %d."
                             "Only one ACL context per FWD context is supported.\n",
                             opcode_id, fwd_acl_ctx_mapping[fwd_context_idx].fwd_context,
                             fwd_acl_ctx_mapping[fwd_context_idx].nof_acl_contexts);
            }
            acl_contexts[fwd_context_idx] = fwd_acl_ctx_mapping[fwd_context_idx].acl_contexts[0];
        }
        (*nof_contexts_p) = fwd_nof_contexts;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_port_profile_type_get(
    int unit,
    bcm_port_class_t bcm_port_class,
    dnx_field_port_porfile_type_e * port_profile_type_p)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_NCM_PORT_CLASS_VERIFY(bcm_port_class);

    (*port_profile_type_p) = dnx_field_map_port_profile_info[bcm_port_class].port_profile_type;

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_port_profile_info_get(
    int unit,
    bcm_port_class_t bcm_port_class,
    dnx_field_map_port_profile_info_t * port_profile_info_p)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_NCM_PORT_CLASS_VERIFY(bcm_port_class);

    sal_memcpy(port_profile_info_p, &dnx_field_map_port_profile_info[bcm_port_class], sizeof(*port_profile_info_p));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_pp_app_dbal_table_get(
    int unit,
    bcm_field_app_db_t bcm_pp_app,
    dbal_tables_e * dbal_table_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dbal_table_id_p, _SHR_E_PARAM, "dbal_table_id_p");

    if (bcm_pp_app == bcmFieldAppDbInvalid || bcm_pp_app >= bcmFieldAppDbCount)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Given PP APP DB is invalid or out of range, maximum: %d\n", bcmFieldAppDbCount);
    }

    if (!dnx_field_pp_app_map[bcm_pp_app].valid)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "No valid table found for the given PP APP DB ID (%d)\n", bcm_pp_app);
    }

    *dbal_table_id_p = dnx_field_pp_app_map[bcm_pp_app].dbal_table_id;

exit:
    SHR_FUNC_EXIT;
}
/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_state_table_source_get(
    int unit,
    int *use_acr_p,
    dnx_field_stage_e * stage_p,
    dbal_enum_value_field_field_key_e * key_id_p,
    int *lsb_on_key_p)
{
    dnx_field_state_table_source_t state_table_source = dnx_data_field.state_table.state_table_stage_key_get(unit);

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(stage_p, _SHR_E_PARAM, "stage_p");
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");
    SHR_NULL_CHECK(lsb_on_key_p, _SHR_E_PARAM, "lsb_on_key_p");

    switch (state_table_source)
    {
        case DNX_FIELD_STATE_TABLE_SOURCE_IPMF1_KEY_J:
        {
            (*use_acr_p) = FALSE;
            (*stage_p) = DNX_FIELD_STAGE_IPMF1;
            (*key_id_p) = DBAL_ENUM_FVAL_FIELD_KEY_J;
            (*lsb_on_key_p) = dnx_data_field.tcam.key_size_single_get(unit) -
                dnx_data_field.state_table.key_size_get(unit);
            break;
        }
        case DNX_FIELD_STATE_TABLE_SOURCE_IPMF2_KEY_J:
        {
            (*use_acr_p) = FALSE;
            (*stage_p) = DNX_FIELD_STAGE_IPMF2;
            (*key_id_p) = DBAL_ENUM_FVAL_FIELD_KEY_J;
            (*lsb_on_key_p) = dnx_data_field.tcam.key_size_single_get(unit) -
                dnx_data_field.state_table.key_size_get(unit);
            break;
        }
        case DNX_FIELD_STATE_TABLE_SOURCE_IPMF1_KEY_F:
            /*
             * Key F not supported. If it was:
             * (*use_acr_p) = FALSE;
             * (*stage_p) = DNX_FIELD_STAGE_IPMF1;
             * (*key_id_p) = DBAL_ENUM_FVAL_FIELD_KEY_F;
             * (*lsb_on_key_p) = dnx_data_field.state_table.address_size_entry_max_get(unit) +
             *                   dnx_data_field.state_table.data_size_entry_max_get(unit) -
             *                   dnx_data_field.state_table.address_size_get(unit) -
             *                   dnx_data_field.state_table.data_size_get(unit);
             */
        default:
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "Unsupported option in SOC property pmf_state_table_rmw_source.\r\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**see H file: field_key.h */
shr_error_e
dnx_field_map_compare_key_id_get(
    int unit,
    dnx_field_context_compare_mode_pair_e compare_pair,
    dnx_field_key_id_t *key_id_1_p,
    dnx_field_key_id_t *key_id_2_p)
{
    SHR_FUNC_INIT_VARS(unit);

    /**
     * Compare uses KEY F,G as first pair, and H,I as second pair
     */
    switch (compare_pair)
    {
        case DNX_FIELD_CONTEXT_COMPARE_MODE_PAIR_1:
            /** First pair gets F and G keys,*/
            key_id_1_p->id[0] = DBAL_ENUM_FVAL_FIELD_KEY_F;
            key_id_2_p->id[0] = DBAL_ENUM_FVAL_FIELD_KEY_G;
            break;
        case DNX_FIELD_CONTEXT_COMPARE_MODE_PAIR_2:
            /** Second pair gets H and I keys*/
            key_id_1_p->id[0] = DBAL_ENUM_FVAL_FIELD_KEY_H;
            key_id_2_p->id[0] = DBAL_ENUM_FVAL_FIELD_KEY_I;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid compare key pair %d", compare_pair);
    }

exit:
    SHR_FUNC_EXIT;
}

/**see H file: field_key.h */
shr_error_e
dnx_field_map_hash_key_id_get(
    int unit,
    dnx_field_key_id_t *key_id_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");

    
    key_id_p->id[0] = DBAL_ENUM_FVAL_FIELD_KEY_I;
    key_id_p->id[1] = DBAL_ENUM_FVAL_FIELD_KEY_J;

exit:
    SHR_FUNC_EXIT;
}

/**see H file: field_key.h */
shr_error_e
dnx_field_map_state_table_key_id_get(
    int unit,
    dnx_field_key_id_t *key_id_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");

    /*
     * State table uses initial key J.
     */
    key_id_p->id[0] = DBAL_ENUM_FVAL_FIELD_KEY_J;
    key_id_p->id[1] = DNX_FIELD_KEY_ID_INVALID;

exit:
    SHR_FUNC_EXIT;
}
