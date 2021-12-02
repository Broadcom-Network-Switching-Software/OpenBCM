/** \file field_map_vw.c
 * 
 *
 * Field database procedures for DNX.
 *
 * This file implements mapping from BCM to DNX VW qualifiers and actions
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX
/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_rhlist.h>

#include <soc/dnx/dbal/dbal_string_mgmt.h>
#include <soc/sand/sand_signals.h>

#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/algo/field/algo_field.h>

#include <bcm/field.h>

#include "field_map_local.h"

#include <include/soc/dnx/pemladrv/pemladrv_virtual_wires.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
#include <soc/dnx/dbal/dbal_dynamic.h>
/*
 * }Include files
 */

static dnx_field_vw_qual_info_t dnx_vw_qual_info[DNX_FIELD_MAP_MAX_VW_QUAL];
static dnx_field_per_vw_action_info_t dnx_per_vw_action_info[DNX_FIELD_MAP_MAX_VW_WITH_ACTION];
static dnx_field_vw_action_info_t dnx_vw_action_info[DNX_FIELD_MAP_MAX_VW_ACTION];

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
    dnx_field_map_stage_info_t field_map_stage_info;

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
        SHR_IF_ERR_EXIT(dnx_field_map_stage_info_get_dispatch(unit, dnx_stages[stage_idx], &field_map_stage_info));
        if (field_map_stage_info.pp_stage == DNX_PP_STAGE_INVALID)
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
            SHR_IF_ERR_EXIT(dnx_field_map_stage_info_get_dispatch(unit, dnx_stages[stage_idx], &field_map_stage_info));
            pp_stages[stage_idx] = field_map_stage_info.pp_stage;
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

/**see H file: field_map.h */
shr_error_e
dnx_field_map_vw_qual_info_get(
    int unit,
    dnx_field_qual_id_t qual_id,
    dnx_field_vw_qual_info_t * vw_qual_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(vw_qual_info_p, _SHR_E_PARAM, "vw_qual_info_p");
    DNX_FIELD_VW_QUAL_VERIFY(qual_id);

    sal_memcpy(vw_qual_info_p, &dnx_vw_qual_info[qual_id], sizeof(*vw_qual_info_p));
    if (vw_qual_info_p->field_id == 0)
    {
        vw_qual_info_p->field_id = DBAL_FIELD_EMPTY;
    }

exit:
    SHR_FUNC_EXIT;
}

/**see H file: field_map.h */
shr_error_e
dnx_field_map_vw_action_info_get(
    int unit,
    dnx_field_action_id_t action_id,
    dnx_field_vw_action_info_t * vw_action_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(vw_action_info_p, _SHR_E_PARAM, "vw_action_info_p");
    DNX_FIELD_VW_ACTION_VERIFY(action_id);

    sal_memcpy(vw_action_info_p, &dnx_vw_action_info[action_id], sizeof(*vw_action_info_p));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: field_map.h */
shr_error_e
dnx_field_map_per_vw_action_info_get(
    int unit,
    dnx_field_action_id_t action_id,
    dnx_field_per_vw_action_info_t * per_vw_action_info_p)
{
    int ii;
    int vw_index;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(per_vw_action_info_p, _SHR_E_PARAM, "per_vw_action_info_p");
    DNX_FIELD_VW_ACTION_VERIFY(action_id);

    vw_index = dnx_vw_action_info[action_id].vw_index;
    if (vw_index < 0 || vw_index >= DNX_FIELD_MAP_MAX_VW_WITH_ACTION)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The vw action(%d) does not exist!", action_id);
    }

    sal_memcpy(per_vw_action_info_p, &dnx_per_vw_action_info[vw_index], sizeof(*per_vw_action_info_p));
    for (ii = 0; ii < DNX_FIELD_ACTION_MAX_ACTION_PER_VM; ii++)
    {
        if (per_vw_action_info_p->field_id[ii] == 0)
        {
            per_vw_action_info_p->field_id[ii] = DBAL_FIELD_EMPTY;
        }
    }

exit:
    SHR_FUNC_EXIT;
}
