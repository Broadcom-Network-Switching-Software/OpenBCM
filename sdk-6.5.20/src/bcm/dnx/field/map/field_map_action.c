/** \file field_map_action.c
 * 
 *
 * Field database procedures for DNX.
 *
 * This file implements mapping from BCM to DNX actions and access for per DNX action information
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

#include <soc/dnx/dbal/dbal_dynamic.h>

#include <bcm_int/dnx/rx/rx_trap.h>

#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_presel.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/algo/field/algo_field.h>

#include <soc/dnx/swstate/auto_generated/access/dnx_field_actions_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/types/dnx_algo_field_types.h>
#include <bcm/field.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>

#include "field_map_local.h"

#include <include/soc/dnx/pemladrv/pemladrv_virtual_wires.h>
/*
 * }Include files
 */

/**
 * \brief Checks if a action's mapping include value conversion.
 * \param [in] action_map_entry_p - Pointer to action mapping of type dnx_field_action_map_t.
 * \return
 *      TRUE  - If the mapping includes value conversion.
 *      FALSE - Otherwise.
 */
#define DNX_FIELD_MAP_ACTION_MAPPING_HAS_VALUE_CONVERSION(action_map_entry_p) \
    ((action_map_entry_p != NULL) && (action_map_entry_p->conversion_cb != NULL))

shr_error_e dbal_fields_field_types_info_get(
    int unit,
    dbal_fields_e field_id,
    dbal_field_types_basic_info_t ** field_type_info);

/**
* \brief
*  Gets all relevant action info that the end user might need and fills it in action_info_in_p.
* \param [in] unit              - Device ID
* \param [in] bcm_action        - BCM Action ID
* \param [out] action_info_p    - holds all relevant info for the action, size, stage, action and name.
* \param [out] dbal_field_id_p  - The DBAL field ID of the user defined action.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_action_get_int(
    int unit,
    bcm_field_action_t bcm_action,
    dnx_field_action_in_info_t * action_info_p,
    dbal_fields_e * dbal_field_id_p)
{
    dnx_field_user_action_info_t user_action_info;
    dnx_field_action_id_t action_id;
    uint32 valid;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(action_info_p, _SHR_E_PARAM, "action_info_in_p");

    if (bcm_action < dnx_data_field.action.user_1st_get(unit) ||
        bcm_action >= dnx_data_field.action.user_1st_get(unit) + dnx_data_field.action.user_nof_get(unit))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
        SHR_EXIT();
    }

    action_id = bcm_action - dnx_data_field.action.user_1st_get(unit);

    SHR_IF_ERR_EXIT(dnx_field_action_sw_db.info.valid.get(unit, action_id, &valid));
    /*
     * Cannot retrieve info on a user action which has not been created. Action is marked as 'invalid'.
     */
    if (valid == FALSE)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_field_action_sw_db.info.get(unit, action_id, &user_action_info));

    sal_strncpy(action_info_p->name, dbal_field_to_string(unit, user_action_info.field_id), DBAL_MAX_STRING_LENGTH - 1);
    action_info_p->size = user_action_info.size;
    action_info_p->prefix = user_action_info.prefix;
    action_info_p->prefix_size = user_action_info.prefix_size;
    action_info_p->stage = DNX_ACTION_STAGE(user_action_info.base_dnx_action);
    SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm
                    (unit, action_info_p->stage, user_action_info.base_dnx_action, &(action_info_p->bcm_action)));

    *dbal_field_id_p = user_action_info.field_id;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief  Provides all the information per DNX action requested for HW configuration
 * \param [in] unit - Identifier of HW platform.
 * \param [in] stage - Stage identifier
 * \param [in] dnx_action - HW action value
 * \param [in,out] dnx_action_info_p - pointer to DNX action info structure, data will be copied into it
 * \return
 *   \retval _SHR_E_NONE      - On success
 *   \retval _SHR_E_PARAM     - One of input parameters is out of range
 *   \retval Other - Other errors as per shr_error_e
 * \remark
 *  The assumption is that the amount of data is modest, so we can copy info data
 *  Pay attention that dnx_qual_info_t structure memory is in user responsibility
 */
static shr_error_e
dnx_field_map_action_info(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_action_t dnx_action,
    dnx_field_map_action_info_t * dnx_action_info_p)
{
    dnx_field_action_class_e dnx_action_class = DNX_ACTION_CLASS(dnx_action);
    dnx_field_stage_e dnx_stage = DNX_ACTION_STAGE(dnx_action);
    dnx_field_action_id_t action_id = DNX_ACTION_ID(dnx_action);
    uint32 valid;
    uint32 signal_iter;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_action_info_p, _SHR_E_PARAM, "dnx_action_info_p");

    /*
     * Initialize signal info struct, don't trust the client
     */
    sal_memset(dnx_action_info_p->signal_name, 0,
               (DNX_DATA_MAX_FIELD_DIAG_NOF_SIGNALS_PER_ACTION * sizeof(*dnx_action_info_p->signal_name)));

    DNX_FIELD_STAGE_VERIFY(stage);
    if (stage != dnx_stage)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Inconsistency between API:\"%s\" and encoded:\"%s\" stages\n",
                     dnx_field_stage_text(unit, stage), dnx_field_stage_text(unit, dnx_stage));
    }

    dnx_action_info_p->class = dnx_action_class;
    dnx_action_info_p->dnx_stage = stage;

    switch (dnx_action_class)
    {
        case DNX_FIELD_ACTION_CLASS_USER:
        {
            dnx_field_user_action_info_t user_action_info;
            dnx_field_action_t base_dnx_action;
            dnx_field_map_action_info_t base_dnx_action_info;
            SHR_IF_ERR_EXIT(dnx_field_action_sw_db.info.valid.get(unit, action_id, &valid));
            /*
             * Cannot retrieve info for a user action that has not yet been created, in which case the
             * action is marked as 'invalid'.
             */
            if (valid == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "The user action(%d) in stage %s does not exist!",
                             action_id, dnx_field_stage_text(unit, dnx_stage));
            }
            SHR_IF_ERR_EXIT(dnx_field_action_sw_db.info.get(unit, action_id, &user_action_info));

            dnx_action_info_p->field_id = user_action_info.field_id;
            dnx_action_info_p->bcm_action = user_action_info.bcm_id;
            /*
             * In order to find the action type we check the action type of the base action.
             * Note that that is a recursive action that calls dnx_field_map_action_info(), but we rely on
             * the fact that the parent may not be a user defined action as a stopping condition.
             */
            base_dnx_action = user_action_info.base_dnx_action;
            SHR_IF_ERR_EXIT(dnx_field_map_action_info(unit, stage, base_dnx_action, &base_dnx_action_info));
            dnx_action_info_p->action_type = base_dnx_action_info.action_type;
            dnx_action_info_p->base_dnx_action = base_dnx_action;
            dnx_action_info_p->prefix = user_action_info.prefix;
            dnx_action_info_p->prefix_size = user_action_info.prefix_size;
            break;
        }
        case DNX_FIELD_ACTION_CLASS_STATIC:
        {
            if (dnx_field_map_stage_info[dnx_stage].static_action_info == NULL)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Static Action list is not defined on Stage:%d\n", dnx_stage);
            }
            if ((action_id < 0) || (action_id >= dnx_field_map_stage_info[stage].static_action_nof) ||
                (dnx_field_map_stage_info[stage].static_action_info[action_id].field_id == DBAL_FIELD_EMPTY))
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "The action(%d) in stage %s does not exist!",
                             action_id, dnx_field_stage_text(unit, dnx_stage));
            }
            dnx_action_info_p->field_id = dnx_field_map_stage_info[stage].static_action_info[action_id].field_id;
            dnx_action_info_p->bcm_action = dnx_field_map_stage_info[stage].static_action_info[action_id].bcm_action;
            /**
             * For static actions action_id is the same as action_type
             */
            dnx_action_info_p->action_type = (dnx_field_action_type_t) action_id;
            dnx_action_info_p->base_dnx_action = DNX_FIELD_ACTION_INVALID;
            dnx_action_info_p->prefix = 0;
            dnx_action_info_p->prefix_size = 0;
            for (signal_iter = 0; signal_iter < DNX_DATA_MAX_FIELD_DIAG_NOF_SIGNALS_PER_ACTION; signal_iter++)
            {
                /** Break in case we reach an empty string, which is indication the end of signals array. */
                if (dnx_field_map_stage_info[stage].static_action_info[action_id].signal_name[signal_iter] == 0)
                {
                    break;
                }
                dnx_action_info_p->signal_name[signal_iter] =
                    dnx_field_map_stage_info[stage].static_action_info[action_id].signal_name[signal_iter];
            }
            break;
        }
        case DNX_FIELD_ACTION_CLASS_SW:
        {
            if ((action_id < 0) || (action_id >= DNX_FIELD_SW_ACTION_NOF))
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "The sw action(%d) does not exist!", action_id);
            }
            dnx_action_info_p->field_id = dnx_sw_action_info[action_id].field_id;
            dnx_action_info_p->bcm_action = dnx_sw_action_info[action_id].bcm_action;
            dnx_action_info_p->action_type = DNX_FIELD_ACTION_TYPE_INVALID;
            dnx_action_info_p->base_dnx_action = DNX_FIELD_ACTION_INVALID;
            dnx_action_info_p->prefix = 0;
            dnx_action_info_p->prefix_size = 0;
            break;
        }
        case DNX_FIELD_ACTION_CLASS_VW:
        {
            int vw_index;
            int action_index_within_vw;
            if ((action_id < 0) || (action_id >= DNX_FIELD_MAP_MAX_VW_ACTION))
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "The vw action(%d) does not exist!", action_id);
            }
            vw_index = dnx_vw_action_info[action_id].vw_index;
            if (vw_index < 0)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "The vw action(%d) does not exist!", action_id);
            }
            action_index_within_vw = dnx_vw_action_info[action_id].action_index_within_vw;
            if (dnx_per_vw_action_info[vw_index].nof_actions_per_stage[stage] <=
                dnx_vw_action_info[action_id].action_index_within_vw)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "The vw action(%d) does not exist!", action_id);
            }
            if (dnx_per_vw_action_info[vw_index].field_id[action_index_within_vw] == DBAL_FIELD_EMPTY)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "VW \"%s\" has %d actions but action %d is empty.\n",
                             dbal_field_to_string(unit, dnx_per_vw_action_info[vw_index].field_id[0]),
                             dnx_per_vw_action_info[vw_index].nof_actions_per_stage[stage],
                             dnx_vw_action_info[action_id].action_index_within_vw);
            }
            dnx_action_info_p->field_id = dnx_per_vw_action_info[vw_index].field_id[action_index_within_vw];
            dnx_action_info_p->bcm_action = dnx_data_field.action.vw_1st_get(unit) + action_id;
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_action_type
                            (unit, stage,
                             dnx_per_vw_action_info[vw_index].base_dnx_action_per_stage[action_index_within_vw][stage],
                             &(dnx_action_info_p->action_type)));
            dnx_action_info_p->base_dnx_action = DNX_FIELD_ACTION_INVALID;
            dnx_action_info_p->prefix = 0;
            dnx_action_info_p->prefix_size = 0;
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid Action Class:%d \n", dnx_action_class);
            break;
    }
    /*
     * Verify DBAL field id in all cases besides USER of VW one
     */
    if (dnx_action_class != DNX_FIELD_ACTION_CLASS_USER && dnx_action_class != DNX_FIELD_ACTION_CLASS_VW)
    {
        DNX_FIELD_DBAL_FIELD_ID_VERIFY(dnx_action_info_p->field_id);
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See field_map.h
 */
CONST char *
dnx_field_bcm_action_text(
    int unit,
    bcm_field_action_t bcm_action)
{
    dbal_fields_e dbal_field_id;
    dnx_field_action_in_info_t action_info;
    static char *bcm_action_text[bcmFieldActionCount] = BCM_FIELD_ACTION_STRINGS;
    int user_defined_first = dnx_data_field.action.user_1st_get(unit);
    int user_defined_last = dnx_data_field.action.user_1st_get(unit) + dnx_data_field.action.user_nof_get(unit) - 1;
    int vw_first = dnx_data_field.action.vw_1st_get(unit);
    int vw_last = dnx_data_field.action.vw_1st_get(unit) + dnx_data_field.action.vw_nof_get(unit) - 1;

    if (bcm_action >= 0 && bcm_action < bcmFieldActionCount)
    {
        const char *bcm_action_name = NULL;
        /** Switch case to handle the unambiguous BCM actions. */
        switch (bcm_action)
        {
            case bcmFieldActionRedirect:
            {
                bcm_action_name = "Redirect";
                break;
            }
            default:
            {
                bcm_action_name = bcm_action_text[bcm_action];
                break;
            }
        }
        /** Predefined action.*/
        if (ISEMPTY(bcm_action_name))
        {
            return "Invalid BCM action";
        }
        return bcm_action_name;
    }
    else if ((bcm_action >= user_defined_first) && (bcm_action <= user_defined_last))
    {
        /** User defined action.*/
        if (dnx_field_action_get_int(unit, bcm_action, &action_info, &dbal_field_id) == _SHR_E_NONE)
        {
            return dbal_field_to_string(unit, dbal_field_id);
        }
        else
        {
            return "Invalid BCM action";
        }
    }
    else if ((bcm_action >= vw_first) && (bcm_action <= vw_last))
    {
        /** Virtual wire action.*/
        if (dnx_vw_action_info[bcm_action - vw_first].vw_index < 0)
        {
            return "Invalid BCM action";
        }
        dbal_field_id =
            dnx_per_vw_action_info[dnx_vw_action_info[bcm_action - vw_first].
                                   vw_index].field_id[dnx_vw_action_info[bcm_action - vw_first].action_index_within_vw];
        if (dbal_field_id != DBAL_FIELD_EMPTY)
        {
            return dbal_field_to_string(unit, dbal_field_id);
        }
        else
        {
            return "Invalid BCM action";
        }
    }
    else
    {
        return "Invalid BCM action";
    }
}

/*
 * See field_map.h
 */
CONST char *
dnx_field_dnx_action_text(
    int unit,
    dnx_field_action_t dnx_action)
{
    dnx_field_map_action_info_t dnx_action_info;

    if (dnx_field_map_action_info(unit, DNX_ACTION_STAGE(dnx_action), dnx_action, &dnx_action_info) != _SHR_E_NONE)
    {
        return "Invalid DNX Action";
    }
    return dbal_field_to_string(unit, dnx_action_info.field_id);
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_action_description_get(
    int unit,
    bcm_field_action_t bcm_action,
    char **action_description)
{

    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_BCM_ACTION_VERIFY(bcm_action);

    if (bcm_action_description[bcm_action] != NULL)
    {
        *action_description = (char *) bcm_action_description[bcm_action];
    }
    else
    {
        *action_description = "N/A";
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See field_map.h
 */
char *
dnx_field_action_class_text(
    dnx_field_action_class_e action_class)
{
    char *class_name;
    if ((action_class < DNX_FIELD_ACTION_CLASS_FIRST) || (action_class >= DNX_FIELD_ACTION_CLASS_NOF))
    {
        class_name = "Invalid Class";
    }
    else if (dnx_field_action_class_names[action_class] == NULL)
    {
        class_name = "Unnamed Class";
    }
    else
    {
        class_name = (char *) dnx_field_action_class_names[action_class];
    }

    return class_name;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_action_ref_dec(
    int unit,
    dnx_field_action_t action)
{
    uint32 fg_refs = 0;
    int action_id = DNX_ACTION_ID(action);
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_action_sw_db.info.ref_count.get(unit, action_id, &fg_refs));
    if (fg_refs <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Action id %d didn't have any Field groups ref's (unused)\n", action);
    }
    fg_refs -= 1;
    SHR_IF_ERR_EXIT(dnx_field_action_sw_db.info.ref_count.set(unit, action_id, fg_refs));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_action_ref_inc(
    int unit,
    dnx_field_action_t action)
{
    uint32 fg_refs = 0;
    int action_id = DNX_ACTION_ID(action);
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_action_sw_db.info.ref_count.get(unit, action_id, &fg_refs));
    fg_refs += 1;
    SHR_IF_ERR_EXIT(dnx_field_action_sw_db.info.ref_count.set(unit, action_id, fg_refs));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_action_in_info_t_init(
    int unit,
    dnx_field_action_in_info_t * action_info_in_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(action_info_in_p, _SHR_E_PARAM, "action_info_in_p");

    sal_memset(action_info_in_p, 0, sizeof(*action_info_in_p));

    /** Set the action_info structure fields to constant invalid values */
    action_info_in_p->bcm_action = bcmFieldActionCount;
    sal_strncpy(action_info_in_p->name, "", DBAL_MAX_STRING_LENGTH - 1);
    action_info_in_p->size = DNX_FIELD_ACTION_LENGTH_TYPE_INVALID;
    action_info_in_p->stage = DNX_FIELD_STAGE_INVALID;
    /** Explicitly set the prefix and prefix size, despite the zeroing being done by sal_memset.*/
    action_info_in_p->prefix = 0;
    action_info_in_p->prefix_size = 0;

exit:
    SHR_FUNC_EXIT;

}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_action_create(
    int unit,
    dnx_field_action_flags_e flags,
    dnx_field_action_in_info_t * action_info_in_p,
    bcm_field_action_t * bcm_action_p,
    dnx_field_action_t * dnx_action_p)
{
    dnx_field_action_id_t action_id;
    int alloc_flag = 0;
    bcm_field_action_t bcm_action = bcmFieldActionCount;
    dnx_field_user_action_info_t user_action_info;
    char field_name[DBAL_MAX_STRING_LENGTH];
    dnx_field_action_t base_dnx_action;
    unsigned int base_action_size;
    int is_void;
    int base_action_is_legal;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(action_info_in_p, _SHR_E_PARAM, "action_info_in_p");
    SHR_NULL_CHECK(bcm_action_p, _SHR_E_PARAM, "bcm_action_p");
    DNX_FIELD_STAGE_VERIFY(action_info_in_p->stage);

    /** We are getting the DNX action so we can do error checks and assignments */
    SHR_IF_ERR_EXIT(dnx_field_map_action_bcm_to_dnx(unit,
                                                    action_info_in_p->stage,
                                                    action_info_in_p->bcm_action, &base_dnx_action));
    /** Verify that the base DNX action isn't a user defined action.*/
    if (DNX_ACTION_CLASS(base_dnx_action) == DNX_FIELD_ACTION_CLASS_USER)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot base a used defined action on another user defined action.\n");
    }

    /** Verify that the base DNX action is usable on the device.*/
    SHR_IF_ERR_EXIT(dnx_field_map_action_is_legal
                    (unit, action_info_in_p->stage, base_dnx_action, &base_action_is_legal));
    if (base_action_is_legal == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Base action (\"%s\") is not usable on this device.\n",
                     dnx_field_dnx_action_text(unit, base_dnx_action));
    }

    /** Check if we are based on a void action (and so the created action is also a void action).*/
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_is_void(unit, action_info_in_p->stage, base_dnx_action, &is_void));

    /** Get the size of the base DNX action.*/
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size(unit, action_info_in_p->stage, base_dnx_action, &base_action_size));

    /*
     * Preform size and prefix verifications, depending on whether the action is void or not.
     */
    if (is_void == FALSE)
    {
        /*
         * Verify that the number of bits in the prefix (excluding trailing zeros in the msb) does not exceed prefix size.
         * The prefix is a constant that would by appended to the msb of each action vlaue by the FES.
         * The prefix size is the number of bits appended.
         * We do not check prefix size for void actions.
         */
        if ((action_info_in_p->prefix_size < SAL_UINT32_NOF_BITS) &&
            ((action_info_in_p->prefix & SAL_FROM_BIT(action_info_in_p->prefix_size)) != 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Prefix does not fit into the number of bits given by prefix_size. "
                         "values given are prefix 0x%X and prefix size %d.\n",
                         action_info_in_p->prefix, action_info_in_p->prefix_size);
        }

        /**
         * We check if the requested size, including the prefix size, is different from the base action size,
         * and return an error, it is not allowed.
         */
        if ((action_info_in_p->size + action_info_in_p->prefix_size) != base_action_size)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The size set %d plus prefix size %d is %d, different from  the size of the "
                         "base action %s size: %d\n",
                         action_info_in_p->size,
                         action_info_in_p->prefix_size,
                         action_info_in_p->size + action_info_in_p->prefix_size,
                         dnx_field_bcm_action_text(unit, action_info_in_p->bcm_action), base_action_size);
        }

        /** Sanity check to protect against wrap around  */
        if (action_info_in_p->size > base_action_size || action_info_in_p->prefix_size > base_action_size)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The size set %u plus prefix size %u is %u, wrapped around. \n",
                         action_info_in_p->size, action_info_in_p->prefix_size,
                         action_info_in_p->size + action_info_in_p->prefix_size);
        }
    }
    else
    {
        /** For void action, make sure that the prefix is zero. we do not care about the prefix size.*/
        if (action_info_in_p->prefix != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Void actions cannot have prefix. Received prefix 0x%X.\n",
                         action_info_in_p->prefix);
        }
        /*
         * Verify that the size of the user defined void action does not exceed the size of the predefined void action.
         * Note that there is no technical reason for this scenario not to work.
         */
        if (action_info_in_p->size > base_action_size)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The size set %d for the void action exceeds the  size of the base action %d. "
                         "Base action is %s.\n",
                         action_info_in_p->size, base_action_size,
                         dnx_field_bcm_action_text(unit, action_info_in_p->bcm_action));
        }
    }

    if (flags & DNX_FIELD_ACTION_FLAG_WITH_ID)
    {
        uint32 valid;
        /*
         * BCM Action is leading number, we'll extract DNX one from BCM
         */
        bcm_action = *bcm_action_p;
        if ((bcm_action < dnx_data_field.action.user_1st_get(unit)) ||
            (bcm_action >= dnx_data_field.action.user_1st_get(unit) + dnx_data_field.action.user_nof_get(unit)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "BCM Action ID:%d is out of range:%d-%d\n", bcm_action,
                         dnx_data_field.action.user_1st_get(unit),
                         dnx_data_field.action.user_1st_get(unit) + dnx_data_field.action.user_nof_get(unit));
        }
        action_id = bcm_action - dnx_data_field.action.user_1st_get(unit);
        SHR_IF_ERR_EXIT(dnx_field_action_sw_db.info.valid.get(unit, action_id, &valid));
        if (valid == TRUE)
        {
            /*
             * Cannot create an action which is already marked as 'valid' (i.e., Has already
             * been created).
             */
            SHR_ERR_EXIT(_SHR_E_PARAM, "BCM Action ID:%d was already created\n", bcm_action);
        }

        alloc_flag = DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID;
    }

    SHR_IF_ERR_EXIT(dnx_algo_field_user_action_id_allocate(unit, alloc_flag, &action_id));

    sal_strncpy(field_name, action_info_in_p->name, DBAL_MAX_STRING_LENGTH - 1);

    SHR_IF_ERR_EXIT(dbal_fields_field_create(unit, DBAL_FIELD_TYPE_DEF_UINT, field_name, &user_action_info.field_id));

    if (!(flags & DNX_FIELD_ACTION_FLAG_WITH_ID))
    {
        bcm_action = dnx_data_field.action.user_1st_get(unit) + action_id;
        *bcm_action_p = bcm_action;
    }

    user_action_info.flags = flags;
    user_action_info.ref_count = 0;
    user_action_info.size = action_info_in_p->size;
    user_action_info.prefix = action_info_in_p->prefix;
    user_action_info.prefix_size = action_info_in_p->prefix_size;
    user_action_info.base_dnx_action = base_dnx_action;
    user_action_info.valid = TRUE;
    user_action_info.bcm_id = bcm_action;

    /**If allocation did not fail we can set the values and SW state*/
    SHR_IF_ERR_EXIT(dnx_field_action_sw_db.info.set(unit, action_id, &user_action_info));
    if (dnx_action_p != NULL)
    {
        *dnx_action_p = DNX_ACTION(DNX_FIELD_ACTION_CLASS_USER, action_info_in_p->stage, action_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_action_destroy(
    int unit,
    bcm_field_action_t bcm_action)
{
    dnx_field_action_id_t action_id;
    dnx_field_user_action_info_t user_action_info;

    SHR_FUNC_INIT_VARS(unit);

    if ((bcm_action < dnx_data_field.action.user_1st_get(unit)) ||
        (bcm_action >= dnx_data_field.action.user_1st_get(unit) + dnx_data_field.action.user_nof_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM Action ID:%d is out of range:%d-%d\n", bcm_action,
                     dnx_data_field.action.user_1st_get(unit),
                     dnx_data_field.action.user_1st_get(unit) + dnx_data_field.action.user_nof_get(unit));
    }

    action_id = bcm_action - dnx_data_field.action.user_1st_get(unit);

    SHR_IF_ERR_EXIT(dnx_field_action_sw_db.info.get(unit, action_id, &user_action_info));

    if (user_action_info.ref_count > 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Can't Destroy, Reference number = %d\n", user_action_info.ref_count);
    }

    SHR_IF_ERR_EXIT(dnx_algo_field_user_action_id_deallocate(unit, action_id));
    SHR_IF_ERR_EXIT(dbal_fields_field_destroy(unit, user_action_info.field_id));

    SHR_IF_ERR_EXIT(dnx_field_action_sw_db.info.valid.set(unit, action_id, FALSE));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_action_get(
    int unit,
    bcm_field_action_t bcm_action,
    dnx_field_action_in_info_t * action_info_p)
{
    dbal_fields_e dbal_field_id;
    _shr_error_t rv;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(action_info_p, _SHR_E_PARAM, "action_info_p");

    rv = dnx_field_action_get_int(unit, bcm_action, action_info_p, &dbal_field_id);

    if (rv == _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The user defined action (%d) is not within range (%d-%d)!",
                     bcm_action, dnx_data_field.action.user_1st_get(unit),
                     dnx_data_field.action.user_1st_get(unit) + dnx_data_field.action.user_nof_get(unit) - 1);
    }
    else if (rv == _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "The user defined action (%d) does not exist!", bcm_action);
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_map_action_bcm_to_dnx_int(
    int unit,
    dnx_field_stage_e dnx_stage,
    uint32 print_errors,
    bcm_field_action_t bcm_action,
    dnx_field_action_t * dnx_action_p,
    const dnx_field_action_map_t ** action_map_entry_p)
{
    dnx_field_action_t dnx_action = 0;
    const dnx_field_action_map_t *action_map_entry = NULL;
    int bare_metal_support;
    int action_is_legal;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_action_p, _SHR_E_PARAM, "dnx_action_p");

    DNX_FIELD_STAGE_VERIFY(dnx_stage);
    /*
     * First check if the action is user defined one
     */
    if (DNX_FIELD_BCM_ACTION_IS_USER(unit, bcm_action))
    {
        uint32 valid;
        dnx_field_action_in_info_t action_info_in;
        /*
         * To obtain DNX from BCM just subtract 1st
         */
        dnx_field_action_id_t action_id = bcm_action - dnx_data_field.action.user_1st_get(unit);
        /*
         * Verify validity of user defined action
         */
        SHR_IF_ERR_EXIT(dnx_field_action_sw_db.info.valid.get(unit, action_id, &valid));
        if (valid == FALSE)
        {
            if (print_errors)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                             "User defined BCM Action %s(%d) doesn't exist!%s",
                             dnx_field_bcm_action_text(unit, bcm_action), bcm_action, EMPTY);
            }
            else
            {
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                SHR_EXIT();
            }
        }
        /*
         * Verify the stage
         */
        SHR_IF_ERR_EXIT(dnx_field_action_get(unit, bcm_action, &action_info_in));
        if (dnx_stage != action_info_in.stage)
        {
            if (print_errors)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                             "User defined BCM Action %s stage %s does not match requested stage: %s",
                             dnx_field_bcm_action_text(unit, bcm_action), dnx_field_stage_text(unit,
                                                                                               action_info_in.stage),
                             dnx_field_stage_text(unit, dnx_stage));
            }
            else
            {
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                SHR_EXIT();
            }
        }

        dnx_action = DNX_ACTION(DNX_FIELD_ACTION_CLASS_USER, dnx_stage, action_id);
    }
    else if ((bcm_action >= 0) && (bcm_action < bcmFieldActionCount))
    {
        /*
         * Static action
         */
        /*
         * Check if the action is a global action, otherwise it is a stage specific action.
         */
        if (dnx_global_action_map[bcm_action].dnx_action != 0)
        {
            action_map_entry = &dnx_global_action_map[bcm_action];

            dnx_action = dnx_global_action_map[bcm_action].dnx_action;
            /** Add the stage to the action.*/
            dnx_action |= (dnx_stage << DNX_ACTION_STAGE_SHIFT);
        }
        else
        {
            uint8 is_std_1;
            /*
             * If it is not a global action, then it is a stage specific action.
             */
            if ((dnx_field_map_stage_info[dnx_stage].static_action_id_map == NULL) ||
                (dnx_field_map_stage_info[dnx_stage].static_action_id_map[bcm_action].dnx_action == 0))
            {
                if (print_errors)
                {
                    SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                                 "BCM Action %s is not supported for stage: %s!\n "
                                 "To see the supported actions, run: field action predefined BCM stage=%s",
                                 dnx_field_bcm_action_text(unit, bcm_action), dnx_field_stage_text(unit, dnx_stage),
                                 dnx_field_stage_text(unit, dnx_stage));
                }
                else
                {
                    SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                    SHR_EXIT();
                }
            }

            /*
             * Map BCM action to DNX
             */
            action_map_entry = &dnx_field_map_stage_info[dnx_stage].static_action_id_map[bcm_action];

            SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_std_1));
            if (!is_std_1
                && (dnx_field_map_stage_info[dnx_stage].static_action_id_map[bcm_action].conversion_cb != NULL))
            {
                SHR_IF_ERR_EXIT(dnx_field_bare_metal_support_check(unit,
                                                                   dnx_field_map_stage_info
                                                                   [dnx_stage].static_action_id_map[bcm_action].
                                                                   conversion_cb, &bare_metal_support));
                if (!bare_metal_support)
                {
                    if (print_errors)
                    {
                        SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                                     "BCM Action %s(%d) is not supported in BareMetal mode!%s",
                                     dnx_field_bcm_action_text(unit, bcm_action), bcm_action, EMPTY);
                    }
                    else
                    {
                        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                        SHR_EXIT();
                    }
                }
            }

            dnx_action =
                DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, dnx_stage,
                           dnx_field_map_stage_info[dnx_stage].static_action_id_map[bcm_action].dnx_action);
        }
    }
    else if ((bcm_action >= dnx_data_field.action.vw_1st_get(unit)) &&
             (bcm_action < dnx_data_field.action.vw_1st_get(unit) + dnx_data_field.action.vw_nof_get(unit)))
    {
        /*
         * Virtual wire action.
         */
        dnx_field_action_id_t action_id;
        uint8 is_std_1;
        SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_std_1));
        if (is_std_1)
        {
            if (print_errors)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                             "BCM Action %s(%d) is a virtual wire action. All virtual wires are not supported on standard 1!%s",
                             dnx_field_bcm_action_text(unit, bcm_action), bcm_action, EMPTY);
            }
            else
            {
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                SHR_EXIT();
            }
        }
        action_id = bcm_action - dnx_data_field.action.vw_1st_get(unit);
        if (dnx_vw_action_info[action_id].vw_index < 0)
        {
            if (print_errors)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                             "Virtual wire BCM Action %s(%d) does not exist!%s!",
                             dnx_field_bcm_action_text(unit, bcm_action), bcm_action, EMPTY);
            }
            else
            {
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                SHR_EXIT();
            }
        }
        if (dnx_per_vw_action_info[dnx_vw_action_info[action_id].vw_index].nof_actions_per_stage[dnx_stage] <=
            dnx_vw_action_info[action_id].action_index_within_vw)
        {
            if (print_errors)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                             "Virtual wire BCM Action %s(%d) is not supported for stage %s!",
                             dnx_field_bcm_action_text(unit, bcm_action), bcm_action, dnx_field_stage_text(unit,
                                                                                                           dnx_stage));
            }
            else
            {
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                SHR_EXIT();
            }
        }
        if (dnx_per_vw_action_info[dnx_vw_action_info[action_id].vw_index].field_id
            [dnx_vw_action_info[action_id].action_index_within_vw] == DBAL_FIELD_EMPTY)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "VW action %d. VW \"%s\" has %d actions but action %d is empty.\n",
                         bcm_action, dbal_field_to_string(unit,
                                                          dnx_per_vw_action_info[dnx_vw_action_info
                                                                                 [action_id].vw_index].field_id[0]),
                         dnx_per_vw_action_info[dnx_vw_action_info[action_id].
                                                vw_index].nof_actions_per_stage[dnx_stage],
                         dnx_vw_action_info[action_id].action_index_within_vw);
        }

        dnx_action = DNX_ACTION(DNX_FIELD_ACTION_CLASS_VW, dnx_stage, action_id);
    }
    else
    {
        /*
         * Not in any legal range.
         */
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "BCM Action %s(%d) is not a legal BCM id%s!",
                     dnx_field_bcm_action_text(unit, bcm_action), bcm_action, EMPTY);
    }

    SHR_IF_ERR_EXIT(dnx_field_map_action_is_legal(unit, dnx_stage, dnx_action, &action_is_legal));
    if (action_is_legal == FALSE)
    {
        if (print_errors)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                         "BCM Action %s(%d) is not usable on this device.%s!",
                         dnx_field_bcm_action_text(unit, bcm_action), bcm_action, EMPTY);
        }
        else
        {
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }
    }

    *dnx_action_p = dnx_action;

exit:
    if (action_map_entry_p != NULL)
    {
        *action_map_entry_p = action_map_entry;
    }
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_map_action_bcm_to_dnx(
    int unit,
    dnx_field_stage_e stage,
    bcm_field_action_t bcm_action,
    dnx_field_action_t * dnx_action_p)
{
    const dnx_field_action_map_t *action_map_entry = NULL;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_map_action_bcm_to_dnx_int
                    (unit, stage, TRUE, bcm_action, dnx_action_p, &action_map_entry));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_map_action_info_bcm_to_dnx(
    int unit,
    bcm_core_t core,
    dnx_field_stage_e stage,
    bcm_field_entry_action_t * bcm_action_info,
    dnx_field_entry_action_t * dnx_action_info)
{
    const dnx_field_action_map_t *action_map_entry = NULL;
    int i_data;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_action_info, _SHR_E_PARAM, "bcm_action_info");
    SHR_NULL_CHECK(dnx_action_info, _SHR_E_PARAM, "dnx_action_info");

    DNX_FIELD_STAGE_VERIFY(stage);

    SHR_IF_ERR_EXIT(dnx_field_map_action_bcm_to_dnx_int
                    (unit, stage, TRUE, bcm_action_info->type, &dnx_action_info->dnx_action, &action_map_entry));

    if ((action_map_entry == NULL) || (action_map_entry->conversion_cb == NULL))
    {
        for (i_data = 0; i_data < BCM_FIELD_ACTION_WIDTH_IN_WORDS; i_data++)
        {
            if ((i_data != 0) && (bcm_action_info->value[i_data] != 0))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "BCM Action %s(%d) only supports one value",
                             dnx_field_bcm_action_text(unit, bcm_action_info->type), bcm_action_info->type);
            }
            if (bcm_action_info->type == bcmFieldActionTrap && stage == DNX_FIELD_STAGE_EPMF)
            {
                dnx_action_info->action_value[i_data] = DNX_RX_TRAP_ID_TYPE_GET(bcm_action_info->value[i_data]);
            }
            else
            {
                dnx_action_info->action_value[i_data] = bcm_action_info->value[i_data];
            }
        }
    }
    else
    {
        SHR_IF_ERR_EXIT_WITH_LOG(action_map_entry->conversion_cb
                                 (unit, 0, core, bcm_action_info->value, dnx_action_info->action_value),
                                 "BCM Action %s(%d) failed to convert value.%s",
                                 dnx_field_bcm_action_text(unit, bcm_action_info->type), bcm_action_info->type, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_map_action_dnx_to_bcm(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_action_t dnx_action,
    bcm_field_action_t * bcm_action_p)
{
    dnx_field_map_action_info_t dnx_action_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_action_p, _SHR_E_PARAM, "bcm_action_p");

    SHR_IF_ERR_EXIT(dnx_field_map_action_info(unit, stage, dnx_action, &dnx_action_info));

    *bcm_action_p = dnx_action_info.bcm_action;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_map_action_list(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_action_t ** dnx_action_list_p,
    int *dnx_action_nof_p)
{
    int i_info, i_action;
    dnx_field_action_t *dnx_action_list = NULL;
    dnx_field_action_t dnx_action;
    int action_is_legal;
    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_STAGE_VERIFY(stage);

    SHR_NULL_CHECK(dnx_action_list_p, _SHR_E_PARAM, "dnx_action_list_p");
    SHR_NULL_CHECK(dnx_action_nof_p, _SHR_E_PARAM, "dnx_action_nof_p");

    if ((dnx_action_list =
         sal_alloc(dnx_field_map_stage_info[stage].static_action_nof * sizeof(dnx_field_action_t), "act_list")) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error allocating memory of dnx action list for stage:\"%s\"\n",
                     dnx_field_stage_text(unit, stage));
    }

    i_action = 0;
    for (i_info = 0; i_info < dnx_field_map_stage_info[stage].static_action_nof; i_info++)
    {
        if (dnx_field_map_stage_info[stage].static_action_info[i_info].field_id != DBAL_FIELD_EMPTY)
        {
            dnx_action = DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, stage, i_info);
            SHR_IF_ERR_EXIT(dnx_field_map_action_is_legal(unit, stage, dnx_action, &action_is_legal));
            if (action_is_legal == FALSE)
            {
                continue;
            }
            if (i_action >= dnx_field_map_stage_info[stage].static_action_nof)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Too much actions for:\"%s\"\n", dnx_field_stage_text(unit, stage));
            }
            dnx_action_list[i_action++] = dnx_action;
        }
    }

    *dnx_action_list_p = dnx_action_list;
    *dnx_action_nof_p = i_action;

exit:
    if (SHR_FUNC_ERR() && (dnx_action_list != NULL))
    {
        sal_free(dnx_action_list);
    }
    SHR_FUNC_EXIT;
}

/**
* See field_map.h
*/
shr_error_e
dnx_field_map_dnx_to_dbal_action(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_action_t dnx_action,
    dbal_fields_e * dbal_field_p)
{
    dnx_field_map_action_info_t dnx_action_info;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dbal_field_p, _SHR_E_PARAM, "dbal_field_p");

    SHR_IF_ERR_EXIT(dnx_field_map_action_info(unit, stage, dnx_action, &dnx_action_info));

    *dbal_field_p = dnx_action_info.field_id;
exit:
    SHR_FUNC_EXIT;
}

/**
* See field_map.h
*/
shr_error_e
dnx_field_map_dnx_to_action_type(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_action_t dnx_action,
    dnx_field_action_type_t * action_type_p)
{
    dnx_field_map_action_info_t dnx_action_info;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(action_type_p, _SHR_E_PARAM, "action_type_p");

    SHR_IF_ERR_EXIT(dnx_field_map_action_info(unit, stage, dnx_action, &dnx_action_info));

    if (dnx_action_info.action_type == DNX_FIELD_ACTION_TYPE_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "DNX action 0x%X has no action type associated with it.\n", dnx_action);
    }

    *action_type_p = dnx_action_info.action_type;
exit:
    SHR_FUNC_EXIT;
}

/**
* See field_map.h
*/
shr_error_e
dnx_field_map_action_is_legal(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_action_t dnx_action,
    int *action_is_legal_p)
{
    int action_is_void;
    dnx_field_action_type_t action_type;
    uint8 dbal_enum_val_illegal;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(action_is_legal_p, _SHR_E_PARAM, "action_is_legal_p");

    SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_is_void(unit, stage, dnx_action, &action_is_void));

    if (action_is_void)
    {
        (*action_is_legal_p) = TRUE;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_action_type(unit, stage, dnx_action, &action_type));

        SHR_IF_ERR_EXIT(dbal_fields_is_illegal_value
                        (unit, dnx_field_map_stage_info[stage].action_field_id, action_type, &dbal_enum_val_illegal));

        (*action_is_legal_p) = !dbal_enum_val_illegal;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* See field_map.h
*/
shr_error_e
dnx_field_map_action_type_to_predef_dnx_action(
    int unit,
    dnx_field_stage_e dnx_stage,
    dnx_field_action_type_t action_type,
    dnx_field_action_t * dnx_action_p)
{
    dnx_field_action_id_t action_id;
    dnx_field_map_action_info_t dnx_action_info;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dnx_action_p, _SHR_E_PARAM, "dnx_action_p");

    /*
     * For static action class the action type equals action id.
     */
    action_id = action_type;

    /*
     * Check if the action exists.
     */
    if (dnx_field_map_stage_info[dnx_stage].static_action_info == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Static Action list is not defined on Stage:%d\n", dnx_stage);
    }
    if ((action_id >= dnx_field_map_stage_info[dnx_stage].static_action_nof) ||
        (dnx_field_map_stage_info[dnx_stage].static_action_info[action_id].field_id == DBAL_FIELD_EMPTY))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "The action(%d) in stage %s does not exist!",
                     action_id, dnx_field_stage_text(unit, dnx_stage));
    }

    /*
     * Build a static action.
     */
    *dnx_action_p = DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, dnx_stage, action_id);

    /** Sanity check, verify that there is no problem with the action.*/
    SHR_IF_ERR_EXIT(dnx_field_map_action_info(unit, dnx_stage, *dnx_action_p, &dnx_action_info));

exit:
    SHR_FUNC_EXIT;
}

/**
* See field_map.h
*/
shr_error_e
dnx_field_map_dnx_action_size(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_action_t dnx_action,
    unsigned int *action_size_p)
{
    uint32 size;

    dnx_field_action_class_e dnx_action_class = DNX_ACTION_CLASS(dnx_action);
    dnx_field_stage_e dnx_stage = DNX_ACTION_STAGE(dnx_action);
    dnx_field_action_id_t action_id = DNX_ACTION_ID(dnx_action);

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(action_size_p, _SHR_E_PARAM, "action_size_p");

    DNX_FIELD_STAGE_VERIFY(stage);

    if (stage != dnx_stage)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Inconsistency between API:\"%s\" and encoded:\"%s\" stages\n",
                     dnx_field_stage_text(unit, stage), dnx_field_stage_text(unit, dnx_stage));
    }

    switch (dnx_action_class)
    {
        case DNX_FIELD_ACTION_CLASS_USER:
        {
            dnx_field_user_action_info_t user_action_info;

            SHR_IF_ERR_EXIT(dnx_field_action_sw_db.info.get(unit, action_id, &user_action_info));
            size = user_action_info.size;
            break;
        }
        case DNX_FIELD_ACTION_CLASS_STATIC:
            if (dnx_field_map_stage_info[dnx_stage].static_action_info == NULL)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Static Action list is not defined on Stage:%d\n", dnx_stage);
            }
            if ((action_id >= dnx_field_map_stage_info[stage].static_action_nof) ||
                (dnx_field_map_stage_info[stage].static_action_info[action_id].field_id == DBAL_FIELD_EMPTY))
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "The action(%d) in stage %s does not exist!",
                             action_id, dnx_field_stage_text(unit, dnx_stage));
            }

            SHR_IF_ERR_EXIT(dbal_field_struct_field_size_get(unit,
                                                             dnx_field_map_stage_info[stage].container_act_field_type,
                                                             dnx_field_map_stage_info[stage].static_action_info
                                                             [action_id].field_id, &size));
            break;
        case DNX_FIELD_ACTION_CLASS_SW:
            if (action_id >= DNX_FIELD_SW_ACTION_NOF)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "The sw action(%d) does not exist!", action_id);
            }
            size = dnx_sw_action_info[action_id].size_sw;
            break;
        case DNX_FIELD_ACTION_CLASS_VW:
        {
            unsigned int uint_size;
            if (action_id >= DNX_FIELD_MAP_MAX_VW_ACTION)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "The VW action(%d) does not exist!", action_id);
            }
            if (dnx_vw_action_info[action_id].vw_index < 0)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "The VW action(%d) not found", action_id);
            }
            if (dnx_vw_action_info[action_id].action_index_within_vw >=
                dnx_per_vw_action_info[dnx_vw_action_info[action_id].vw_index].nof_actions_per_stage[stage])
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "The VW action(%d) in stage %s not found",
                             action_id, dnx_field_stage_text(unit, dnx_stage));
            }
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size
                            (unit, stage,
                             dnx_per_vw_action_info[dnx_vw_action_info[action_id].
                                                    vw_index].base_dnx_action_per_stage[dnx_vw_action_info[action_id].
                                                                                        action_index_within_vw][stage],
                             &uint_size));
            size = uint_size;
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid Action Class:%d \n", dnx_action_class);
            break;
    }

    *action_size_p = size;
exit:
    SHR_FUNC_EXIT;
}

/**
* See field_map.h
*/
shr_error_e
dnx_field_map_dnx_action_signals(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_action_t dnx_action,
    char *action_signals[DNX_DATA_MAX_FIELD_DIAG_NOF_SIGNALS_PER_ACTION])
{
    dnx_field_map_action_info_t dnx_action_info;
    uint32 signal_iter;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(action_signals, _SHR_E_PARAM, "action_signals");

    /*
     * Initialize signal info struct, don't trust the client
     */
    sal_memset(action_signals, 0, (DNX_DATA_MAX_FIELD_DIAG_NOF_SIGNALS_PER_ACTION * sizeof(*action_signals)));

    SHR_IF_ERR_EXIT(dnx_field_map_action_info(unit, stage, dnx_action, &dnx_action_info));

    for (signal_iter = 0; signal_iter < DNX_DATA_MAX_FIELD_DIAG_NOF_SIGNALS_PER_ACTION; signal_iter++)
    {
        /** Break in case we reach an empty string, which is indication the end of signals array. */
        if (dnx_action_info.signal_name[signal_iter] == 0)
        {
            break;
        }
        action_signals[signal_iter] = dnx_action_info.signal_name[signal_iter];
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* See field_map.h
*/
shr_error_e
dnx_field_map_dnx_action_prefix(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_action_t dnx_action,
    uint32 *prefix_p,
    unsigned int *prefix_size_p)
{
    dnx_field_map_action_info_t dnx_action_info;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(prefix_p, _SHR_E_PARAM, "prefix_p");
    SHR_NULL_CHECK(prefix_size_p, _SHR_E_PARAM, "prefix_size_p");

    SHR_IF_ERR_EXIT(dnx_field_map_action_info(unit, stage, dnx_action, &dnx_action_info));

    /*
     * Only user defined actions have prefixes. Perform sanity check.
     */
    if (DNX_ACTION_CLASS(dnx_action) != DNX_FIELD_ACTION_CLASS_USER
        && (dnx_action_info.prefix != 0 || dnx_action_info.prefix_size != 0))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "DNX action 0x%X is of class %s. Only user defined actions are supposed to "
                     "have a prefix, and yet it has one: prefix 0x%X prefix size %d.\n",
                     dnx_action, dnx_field_action_class_text(DNX_ACTION_CLASS(dnx_action)),
                     dnx_action_info.prefix, dnx_action_info.prefix_size);
    }

    *prefix_p = dnx_action_info.prefix;
    *prefix_size_p = dnx_action_info.prefix_size;

exit:
    SHR_FUNC_EXIT;
}

/**
* See field_map.h
*/
shr_error_e
dnx_field_map_dnx_action_base_action(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_action_t dnx_action,
    dnx_field_action_t * base_dnx_action_p)
{
    dnx_field_map_action_info_t dnx_action_info;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(base_dnx_action_p, _SHR_E_PARAM, "base_dnx_action_p");

    /*
     * Check if the action is user defined.
     * Alternatively, we could check after dnx_field_map_action_info() if we receive a valid base dnx action.
     */
    if (DNX_ACTION_CLASS(dnx_action) != DNX_FIELD_ACTION_CLASS_USER)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "DNX action 0x%X is of class %s. Only user defined actions have a base action.\n",
                     dnx_action, dnx_field_action_class_text(DNX_ACTION_CLASS(dnx_action)));
    }
    SHR_IF_ERR_EXIT(dnx_field_map_action_info(unit, stage, dnx_action, &dnx_action_info));

    (*base_dnx_action_p) = dnx_action_info.base_dnx_action;

exit:
    SHR_FUNC_EXIT;
}

/**
* See field_map.h
*/
shr_error_e
dnx_field_map_bcm_action_base_action(
    int unit,
    bcm_field_action_t bcm_action,
    bcm_field_action_t * base_bcm_action_p)
{
    dnx_field_action_in_info_t action_info;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(base_bcm_action_p, _SHR_E_PARAM, "base_bcm_action_p");

    /*
     * Check if the action is user defined.
     */
    if (DNX_FIELD_BCM_ACTION_IS_USER(unit, bcm_action))
    {
        SHR_IF_ERR_EXIT(dnx_field_action_get(unit, bcm_action, &action_info));
        (*base_bcm_action_p) = action_info.bcm_action;
    }
    else
    {
        /*
         * Predefined actions don't have base action, so we return an invalid value.
         */
        (*base_bcm_action_p) = bcmFieldActionCount;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* See field_map.h
*/
shr_error_e
dnx_field_map_dnx_action_is_void(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_action_t dnx_action,
    int *action_is_void_p)
{
    dnx_field_action_t base_dnx_action;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(action_is_void_p, _SHR_E_PARAM, "action_is_void_p");
    DNX_FIELD_STAGE_VERIFY(stage);

    if (dnx_action == DNX_FIELD_ACTION_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Action is invalid.\n");
    }

    /*
     * Void action is defined as an action that is the predefined void action or a user defined action
     * based on it.
     */
    (*action_is_void_p) = FALSE;
    if (DNX_ACTION_CLASS(dnx_action) == DNX_FIELD_ACTION_CLASS_SW
        && DNX_ACTION_ID(dnx_action) == DNX_FIELD_SW_ACTION_VOID)
    {
        /*
         * The action is the predefined void action.
         */
        (*action_is_void_p) = TRUE;
    }
    else if (DNX_ACTION_CLASS(dnx_action) == DNX_FIELD_ACTION_CLASS_USER)
    {
        /*
         * The action is a user defined action.
         * Check if the user defined action is based on the predefined void action.
         */
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_base_action(unit, stage, dnx_action, &base_dnx_action));
        if (base_dnx_action != DNX_FIELD_ACTION_INVALID
            && DNX_ACTION_CLASS(base_dnx_action) == DNX_FIELD_ACTION_CLASS_SW
            && DNX_ACTION_ID(base_dnx_action) == DNX_FIELD_SW_ACTION_VOID)
        {
            (*action_is_void_p) = TRUE;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_get_max_dnx_action(
    int unit,
    dnx_field_stage_e dnx_stage,
    dnx_field_action_t * dnx_action_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dnx_action_p, _SHR_E_PARAM, "dnx_action_p");

    DNX_FIELD_STAGE_VERIFY(dnx_stage);

    *dnx_action_p = DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, dnx_stage,
                               dnx_field_map_stage_info[dnx_stage].static_action_nof);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_get_invalid_dnx_action(
    int unit,
    dnx_field_stage_e dnx_stage,
    dnx_field_action_t * dnx_action_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dnx_action_p, _SHR_E_PARAM, "dnx_action_p");

    DNX_FIELD_STAGE_VERIFY(dnx_stage);

    if (dnx_field_map_stage_info[dnx_stage].static_action_info == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Static Action list is not defined on Stage:%d\n", dnx_stage);
    }

    *dnx_action_p = DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, dnx_stage,
                               dnx_field_map_stage_info[dnx_stage].static_action_nof - 1);

exit:
    SHR_FUNC_EXIT;
}

/*
 * See field_map.h
 */
shr_error_e
dnx_field_map_get_invalid_action_type(
    int unit,
    dnx_field_stage_e dnx_stage,
    dnx_field_action_type_t * action_type_p)
{
    dnx_field_action_t dnx_action;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(action_type_p, _SHR_E_PARAM, "action_type_p");

    DNX_FIELD_STAGE_VERIFY(dnx_stage);

    if (dnx_field_map_stage_info[dnx_stage].static_action_info == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Static Action list is not defined on Stage:%d\n", dnx_stage);
    }

    SHR_IF_ERR_EXIT(dnx_field_map_field_id_dnx_action(unit, dnx_stage, DBAL_FIELD_INVALID, &dnx_action));
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_action_type(unit, dnx_stage, dnx_action, action_type_p));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See field_map.h
 */
shr_error_e
dnx_field_map_get_invalidate_next_action_type(
    int unit,
    dnx_field_stage_e dnx_stage,
    dnx_field_action_type_t * action_type_p)
{
    dnx_field_action_t dnx_action;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(action_type_p, _SHR_E_PARAM, "action_type_p");

    DNX_FIELD_STAGE_VERIFY(dnx_stage);

    if (dnx_field_map_stage_info[dnx_stage].static_action_info == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Static Action list is not defined on Stage:%d\n", dnx_stage);
    }

    SHR_IF_ERR_EXIT(dnx_field_map_field_id_dnx_action(unit, dnx_stage, DBAL_FIELD_INVALIDATE_NEXT, &dnx_action));
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_action_type(unit, dnx_stage, dnx_action, action_type_p));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See field_map.h
 */
shr_error_e
dnx_field_map_field_id_dnx_action(
    int unit,
    dnx_field_stage_e dnx_stage,
    dbal_fields_e field_id,
    dnx_field_action_t * dnx_action_p)
{
    unsigned int action_ndx;
    int action_found;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dnx_action_p, _SHR_E_PARAM, "dnx_action_p");

    DNX_FIELD_STAGE_VERIFY(dnx_stage);

    if (dnx_field_map_stage_info[dnx_stage].static_action_info == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Static Action list is not defined on Stage:%d\n", dnx_stage);
    }

    DNX_FIELD_DBAL_FIELD_ID_VERIFY(field_id);

    action_found = FALSE;
    for (action_ndx = 0; action_ndx < dnx_field_map_stage_info[dnx_stage].static_action_nof; action_ndx++)
    {
        if (dnx_field_map_stage_info[dnx_stage].static_action_info[action_ndx].field_id == field_id)
        {
            if (action_found)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field ID %d is being used by multiple DNX actions in stage \"%s\".\n",
                             field_id, dnx_field_stage_text(unit, dnx_stage));
            }
            (*dnx_action_p) = DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, dnx_stage, action_ndx);
            action_found = TRUE;
        }
    }

    if (action_found == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Field ID %d isn't used by any action in stage \"%s\".\n",
                     field_id, dnx_field_stage_text(unit, dnx_stage));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_fem_ext_output_src_type_bcm_to_dnx(
    int unit,
    bcm_field_fem_extraction_output_source_type_t bcm_fem_ext_output_src_type,
    dnx_field_fem_bit_format_e * dnx_fem_bit_format_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_fem_bit_format_p, _SHR_E_PARAM, "dnx_fem_bit_format_p");

    switch (bcm_fem_ext_output_src_type)
    {
        case bcmFieldFemExtractionOutputSourceTypeForce:
        {
            *dnx_fem_bit_format_p = DNX_FIELD_FEM_BIT_FORMAT_FROM_THIS_FIELD;
            break;
        }
        case bcmFieldFemExtractionOutputSourceTypeKey:
        {
            *dnx_fem_bit_format_p = DNX_FIELD_FEM_BIT_FORMAT_FROM_KEY_SELECT;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid FEM Extraction Output Source Type:%d \n",
                         bcm_fem_ext_output_src_type);
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
dnx_field_map_fem_ext_output_src_type_dnx_to_bcm(
    int unit,
    dnx_field_fem_bit_format_e dnx_fem_bit_format,
    bcm_field_fem_extraction_output_source_type_t * bcm_fem_ext_output_src_type_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_fem_ext_output_src_type_p, _SHR_E_PARAM, "bcm_fem_ext_output_src_type_p");

    switch (dnx_fem_bit_format)
    {
        case DNX_FIELD_FEM_BIT_FORMAT_FROM_THIS_FIELD:
        {
            *bcm_fem_ext_output_src_type_p = bcmFieldFemExtractionOutputSourceTypeForce;
            break;
        }
        case DNX_FIELD_FEM_BIT_FORMAT_FROM_KEY_SELECT:
        {
            *bcm_fem_ext_output_src_type_p = bcmFieldFemExtractionOutputSourceTypeKey;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid FEM Extraction Output Source Type:%d \n", dnx_fem_bit_format);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See field_map.h
 */
shr_error_e
dnx_field_map_name_to_action(
    int unit,
    char name[DBAL_MAX_STRING_LENGTH],
    int *nof_actions,
    bcm_field_action_t bcm_actions[DNX_FIELD_ACTION_MAX_ACTION_PER_VM])
{
    bcm_field_action_t bcm_action_ndx;
    int nof_found = 0;
    int user_defined_first = dnx_data_field.action.user_1st_get(unit);
    int user_defined_last = dnx_data_field.action.user_1st_get(unit) + dnx_data_field.action.user_nof_get(unit) - 1;
    int vw_first = dnx_data_field.action.vw_1st_get(unit);
    int vw_last = dnx_data_field.action.vw_1st_get(unit) + dnx_data_field.action.vw_nof_get(unit) - 1;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(name, _SHR_E_PARAM, "name");
    SHR_NULL_CHECK(bcm_actions, _SHR_E_PARAM, "bcm_actions");

    if (name[0] == '\0')
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Name is an empty string.\n");
    }
    if (0 == sal_strncmp(name, "Invalid BCM action", DBAL_MAX_STRING_LENGTH))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Name is \"%.*s\". \"%s\" is an illegal string.\n",
                     DBAL_MAX_STRING_LENGTH, name, "Invalid BCM action");
    }

    /*
     * Go over virtual wires actions. Can be more than one.
     * Ignore the names of the non first actions, only relate to the name of the first action (the virtual wire name).
     */
    for (bcm_action_ndx = vw_first; bcm_action_ndx <= vw_last; bcm_action_ndx++)
    {
        int action_index = bcm_action_ndx - vw_first;
        if ((dnx_vw_action_info[action_index].vw_index >= 0) &&
            (dnx_vw_action_info[action_index].action_index_within_vw == 0))
        {
            dbal_fields_e field_id_0 = dnx_per_vw_action_info[dnx_vw_action_info[action_index].vw_index].field_id[0];
            if (field_id_0 == DBAL_FIELD_EMPTY)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Action %d has an index but no DBAL field.\n", bcm_action_ndx);
            }
            if (0 == sal_strncmp(name, dbal_field_to_string(unit, field_id_0), DBAL_MAX_STRING_LENGTH))
            {
                for (nof_found = 0; nof_found < DNX_FIELD_ACTION_MAX_ACTION_PER_VM; nof_found++)
                {
                    if (dnx_per_vw_action_info[dnx_vw_action_info[action_index].vw_index].field_id[nof_found] ==
                        DBAL_FIELD_EMPTY)
                    {
                        break;
                    }
                    /*
                     * We assume that in init the actions for the same VW are added one after another.
                     */
                    if ((nof_found > 0) &&
                        (dnx_vw_action_info[action_index + nof_found].vw_index !=
                         dnx_vw_action_info[action_index + nof_found - 1].vw_index))
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Name is \"%.*s\". Action %d is not consecutively mapped.\n",
                                     DBAL_MAX_STRING_LENGTH, name, nof_found);
                    }
                    if (dnx_vw_action_info[action_index + nof_found].action_index_within_vw != nof_found)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Name is \"%.*s\". Action %d is not consecutively mapped.\n",
                                     DBAL_MAX_STRING_LENGTH, name, nof_found);
                    }
                    bcm_actions[nof_found] = bcm_action_ndx + nof_found;
                }
            }
        }
    }

    /*
     * Go over user defined actions.
     */
    if (nof_found == 0)
    {
        for (bcm_action_ndx = user_defined_first; bcm_action_ndx <= user_defined_last; bcm_action_ndx++)
        {
            if (0 == sal_strncmp(name, dnx_field_bcm_action_text(unit, bcm_action_ndx), DBAL_MAX_STRING_LENGTH))
            {
                nof_found = 1;
                bcm_actions[0] = bcm_action_ndx;
                break;
            }
        }
    }

    /*
     * Go over predefiend actions.
     */
    if (nof_found == 0)
    {
        for (bcm_action_ndx = 0; bcm_action_ndx < bcmFieldActionCount; bcm_action_ndx++)
        {
            if (0 == sal_strncmp(name, dnx_field_bcm_action_text(unit, bcm_action_ndx), DBAL_MAX_STRING_LENGTH))
            {
                nof_found = 1;
                bcm_actions[0] = bcm_action_ndx;
                break;
            }
        }
    }

    if (nof_found <= 0)
    {
        /** We set the first element for consistency, even though nof_actions is zero.*/
        bcm_actions[0] = bcmFieldActionCount;
    }
    (*nof_actions) = nof_found;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_action_init(
    int unit)
{
    bcm_field_action_t i_bcm_act;
    dnx_field_stage_e stage;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialize SW Actions
     */
    {
        int i_info;
        for (i_info = DNX_FIELD_SW_ACTION_FIRST; i_info < DNX_FIELD_SW_ACTION_NOF; i_info++)
        {
            dnx_sw_action_info[i_info].bcm_action = DNX_BCM_ID_INVALID;
            if (dnx_sw_action_info[i_info].field_id == DBAL_FIELD_EMPTY)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "No DBAL field ID for sw action: %d\n", i_info);
            }
        }
    }

    /*
     * Initialize stage specific Static Actions
     */
    DNX_FIELD_STAGE_ACTION_ITERATOR(stage)
    {
        int i_info;
        for (i_info = 0; i_info < dnx_field_map_stage_info[stage].static_action_nof; i_info++)
        {
            uint32 size;
            uint8 dbal_enum_val_illegal;
            dnx_field_base_action_info_t *base_action_info =
                &dnx_field_map_stage_info[stage].static_action_info[i_info];
            base_action_info->bcm_action = DNX_BCM_ID_INVALID;
            if (base_action_info->field_id == DBAL_FIELD_EMPTY)
            {
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META("No DBAL field ID for static action:%s %d\n"),
                                             dnx_field_stage_text(unit, stage), i_info));
                continue;
            }
            SHR_IF_ERR_EXIT(dbal_fields_is_illegal_value
                            (unit, dnx_field_map_stage_info[stage].action_field_id, base_action_info->field_id,
                             &dbal_enum_val_illegal));
            if (dbal_enum_val_illegal == FALSE)
            {
                /** Verify we can get the size of the action.*/
                SHR_IF_ERR_EXIT(dbal_field_struct_field_size_get(unit,
                                                                 dnx_field_map_stage_info
                                                                 [stage].container_act_field_type,
                                                                 base_action_info->field_id, &size));
            }
        }
    }

    for (i_bcm_act = 0; i_bcm_act < bcmFieldActionCount; i_bcm_act++)
    {
        dnx_field_action_class_e action_class;
        dnx_field_action_id_t action_id;
        dnx_field_stage_e dnx_stage;
        if (dnx_global_action_map[i_bcm_act].dnx_action != 0)
        {
            action_id = DNX_ACTION_ID(dnx_global_action_map[i_bcm_act].dnx_action);
            action_class = DNX_ACTION_CLASS(dnx_global_action_map[i_bcm_act].dnx_action);
            if (action_class == DNX_FIELD_ACTION_CLASS_SW)
            {
                if (dnx_sw_action_info[action_id].size_sw != 0)
                {
                    dnx_sw_action_info[action_id].bcm_action = i_bcm_act;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "BCM Action:%s mapped to invalid static dnx action type:%s\n",
                                 dnx_field_bcm_action_text(unit, i_bcm_act),
                                 dnx_field_dnx_action_text(unit, action_id));
                }
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "BCM Action:%s mapped as global action to dnx class: %s, "
                             "yet only sw action class has non stage specific information array.\n",
                             dnx_field_bcm_action_text(unit, i_bcm_act), dnx_field_action_class_text(action_class));
            }
            if (ISEMPTY(bcm_action_description[i_bcm_act]))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "BCM Action:\"%s\" has no valid description\n",
                             dnx_field_bcm_action_text(unit, i_bcm_act));
            }
        }
        else
        {
            dnx_field_action_map_t *base_action_map;
            dnx_field_base_action_info_t *base_action_info;

            DNX_FIELD_STAGE_ACTION_ITERATOR(dnx_stage)
            {
                uint8 dbal_enum_val_illegal;
                base_action_map = (dnx_field_action_map_t *)
                    & dnx_field_map_stage_info[dnx_stage].static_action_id_map[i_bcm_act];
                if (base_action_map->dnx_action == 0)
                    continue;
                /*
                 * Here dnx_action is not encoded - it is actually DBAL enum for the ACTION field
                 */
                action_id = base_action_map->dnx_action;
                /*
                 * Check if this enum is enabled for this stage.
                 * Only write to debug, as we need to support multiple devices.
                 */
                SHR_IF_ERR_EXIT(dbal_fields_is_illegal_value
                                (unit, dnx_field_map_stage_info[dnx_stage].action_field_id, action_id,
                                 &dbal_enum_val_illegal));
                if (dbal_enum_val_illegal == FALSE)
                {
                    LOG_DEBUG_EX(BSL_LOG_MODULE,
                                 "BCM Action %s(%d) not supported for stage:%s.%s\n",
                                 dnx_field_bcm_action_text(unit, i_bcm_act), i_bcm_act,
                                 dnx_field_stage_text(unit, dnx_stage), EMPTY);
                }

                base_action_info = &dnx_field_map_stage_info[dnx_stage].static_action_info[action_id];
                if (base_action_info->field_id != DBAL_FIELD_EMPTY)
                {
                    /*
                     * Assign bcm it is first time or if assigning bcm is basic one
                     * All other verification checks were run by appropriate test
                     * No device dependency here all devices that support certain bcm action will use same dnx one
                     */
                    if ((base_action_info->bcm_action == DNX_BCM_ID_INVALID) ||
                        (base_action_map->flags & BCM_TO_DNX_BASIC_OBJECT))
                    {
                        base_action_info->bcm_action = i_bcm_act;
                    }
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "BCM Action:%s mapped to invalid static dnx action type:%s\n",
                                 dnx_field_bcm_action_text(unit, i_bcm_act), dnx_field_dnx_action_text(unit,
                                                                                                       action_id));
                }
                if (ISEMPTY(bcm_action_description[i_bcm_act]))
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "BCM Action:\"%s\" is mapped but has no valid description\n",
                                 dnx_field_bcm_action_text(unit, i_bcm_act));
                }
            }
        }
    }

    /*
     * Verify that all conversion CBs, have BareMetal support macro check
     */
    for (i_bcm_act = 0; i_bcm_act < bcmFieldActionCount; i_bcm_act++)
    {
        dnx_field_stage_e dnx_stage;
        int bare_metal_support;
        DNX_FIELD_STAGE_ACTION_ITERATOR(dnx_stage)
        {
            if (dnx_field_map_stage_info[dnx_stage].static_action_id_map[i_bcm_act].conversion_cb != NULL)
            {
                SHR_IF_ERR_EXIT(dnx_field_bare_metal_support_check(unit,
                                                                   dnx_field_map_stage_info
                                                                   [dnx_stage].static_action_id_map[i_bcm_act].
                                                                   conversion_cb, &bare_metal_support));
            }
        }
    }

    /*
     * Verify that all actions that have description are mapped.
     */
    for (i_bcm_act = 0; i_bcm_act < bcmFieldActionCount; i_bcm_act++)
    {
        dnx_field_stage_e dnx_stage;
        dnx_field_action_t dnx_action;
        const dnx_field_action_map_t *action_map_entry_p;
        int is_mapped;
        int rv;
        if ((ISEMPTY(bcm_action_description[i_bcm_act])) == FALSE)
        {
            is_mapped = FALSE;
            DNX_FIELD_STAGE_ACTION_ITERATOR(dnx_stage)
            {
                rv = dnx_field_map_action_bcm_to_dnx_int(unit, dnx_stage, FALSE, i_bcm_act, &dnx_action,
                                                         &action_map_entry_p);
                if (rv != _SHR_E_NOT_FOUND && rv != _SHR_E_PARAM)
                {
                    SHR_IF_ERR_EXIT(rv);
                }
                if (rv == _SHR_E_NONE)
                {
                    is_mapped = TRUE;
                    break;
                }
            }
            if (is_mapped == FALSE)
            {
                
            }
        }
    }

    if (dnx_data_field.action.user_1st_get(unit) != BCM_FIELD_FIRST_USER_ACTION_ID)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "In DNX data the first BCM user defined action is %d, "
                     "while BCM_FIELD_FIRST_USER_ACTION_ID is %d.\n",
                     dnx_data_field.action.user_1st_get(unit), BCM_FIELD_FIRST_USER_ACTION_ID);
    }

    if (dnx_data_field.action.user_1st_get(unit) < bcmFieldActionCount)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "First BCM User Action ID is %d, should be least bcmFieldActionCount:%d\n",
                     dnx_data_field.action.user_1st_get(unit), bcmFieldActionCount);
    }

    if ((dnx_data_field.action.user_1st_get(unit) + dnx_data_field.action.user_nof_get(unit)) >
        dnx_data_field.action.vw_1st_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "First VW action ID is %d, and last User defined ation ID is %d.\n",
                     dnx_data_field.action.vw_1st_get(unit),
                     dnx_data_field.action.user_1st_get(unit) + dnx_data_field.action.user_nof_get(unit) - 1);
    }

exit:
    SHR_FUNC_EXIT;
}
