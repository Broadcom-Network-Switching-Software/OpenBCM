/**
 * \file       tcam_bank_algo.c
 * This file implements all the functions for the TCAM Bank Algo.
 */
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/dnx/field/tcam/tcam_bank_algo.h>
#include <bcm_int/dnx/field/tcam/tcam_bank_manager.h>
#include <bcm_int/dnx/field/tcam/tcam_prefix_manager.h>
#include <bcm_int/dnx/field/field_group.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX

shr_error_e
dnx_field_tcam_bank_algo_check_context_collision(
    int unit,
    int bank_id,
    uint32 handler_id,
    uint8 *collide)
{
    uint32 *handler_p;
    uint32 tcam_handler_ids[DNX_DATA_MAX_FIELD_TCAM_NOF_TCAM_HANDLERS + 1] = { DNX_FIELD_TCAM_HANDLER_INVALID };
    uint8 handler_in_bank;
    dnx_field_tcam_access_fg_params_t fg_params;

    SHR_FUNC_INIT_VARS(unit);

    /** We assume at the beginning that there is no collision */
    *collide = FALSE;

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, handler_id, &fg_params));

    /** Get all handlers that share context with the given handler, and check if one of them is in the bank */
    if (fg_params.context_sharing_handlers_cb)
    {
        SHR_IF_ERR_EXIT((*(fg_params.context_sharing_handlers_cb)) (unit, handler_id, tcam_handler_ids));
    }

    for (handler_p = &tcam_handler_ids[0]; *handler_p != DNX_FIELD_TCAM_HANDLER_INVALID; handler_p++)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_algo_handler_in_bank(unit, bank_id, *handler_p, &handler_in_bank));
        if (handler_in_bank)
        {
            /*
             * Same-context handler is in bank, collision will happen
             */
            *collide = TRUE;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_bank_algo_get_owner_stage(
    int unit,
    int bank_id,
    dnx_field_tcam_stage_e * bank_owner_stage)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.owner_stage.get(unit, bank_id, bank_owner_stage));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_bank_algo_get_handler_node_in_bank(
    int unit,
    int bank_id,
    uint32 handler_id,
    sw_state_ll_node_t * handler_node)
{
    sw_state_ll_node_t node;
    uint32 val;

    SHR_FUNC_INIT_VARS(unit);

    *handler_node = SW_STATE_LL_INVALID;

    /*
     * Iterate over the list to check if bank already contains handler
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.active_handlers_id.get_first(unit, bank_id, &node));
    while (DNX_SW_STATE_LL_IS_NODE_VALID(node))
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.active_handlers_id.node_value(unit, bank_id, node, &val));
        if (val == handler_id)
        {
            /*
             * This is the node we're looking for
             */
            break;
        }
        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.active_handlers_id.next_node(unit, bank_id, node, &node));
    }
    *handler_node = node;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_bank_algo_handler_in_bank(
    int unit,
    int bank_id,
    uint32 handler_id,
    uint8 *handler_in_bank)
{
    sw_state_ll_node_t handler_node;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_algo_get_handler_node_in_bank(unit, bank_id, handler_id, &handler_node));

    /*
     * If handler's node exists then handler is in bank
     */
    *handler_in_bank = (DNX_SW_STATE_LL_IS_NODE_VALID(handler_node));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Checks whether the given bank_id can be used for the a DT TCAM handler.
 *   IMPORTANT NOTE: This is only a partial check! The full check is done at
 *                   dnx_field_tcam_bank_algo_single_check()
 *
 * \param [in] unit         - Device ID
 * \param [in] bank_id      - bank_id to be checked
 * \param [out] can_be_used - TRUE if given bank can be used for given handler id, FALSE otherwise
 *
 * \return
 *  \retval _SHR_E_NONE  - Success
 */
static shr_error_e
dnx_field_tcam_bank_algo_single_dt_check(
    int unit,
    int bank_id,
    uint8 *can_be_used)
{
    int bank_mode;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.bank_mode.get(unit, bank_id, &bank_mode));

    /** DT handlers can reside on big banks with mode NONE only when AUTO allocating a bank*/
    *can_be_used = (bank_mode == DNX_FIELD_TCAM_BANK_MODE_NONE && FIELD_TCAM_BANK_IS_BIG_BANK(bank_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Checks whether the given bank_id can be used for the a regular TCAM handler.
 *   IMPORTANT NOTE: This is only a partial check! The full check is done at
 *                   dnx_field_tcam_bank_algo_single_check()
 *
 * \param [in] unit         - Device ID
 * \param [in] bank_id      - bank_id to be checked
 * \param [out] can_be_used - TRUE if given bank can be used for given handler id, FALSE otherwise
 *
 * \return
 *  \retval _SHR_E_NONE  - Success
 */
static shr_error_e
dnx_field_tcam_bank_algo_single_reg_check(
    int unit,
    int bank_id,
    uint8 *can_be_used)
{
    int bank_mode;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.bank_mode.get(unit, bank_id, &bank_mode));

    /** non-DT handlers can reside on all modes except DT */
    if (bank_mode == DNX_FIELD_TCAM_BANK_MODE_RESTRICTED_DT)
    {
        *can_be_used = FALSE;
    }
    else
    {
        *can_be_used = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Checks whether the given bank_id can be used for the given handler id.
 *   (Important note: this function does not check the next bank for handlers with double-sized key, since
 *   handlers with double-sized key use a pair of banks, this function needs to be invoked twice, once for each bank in
 *   the pair in order to check if the handler with double-sized key can be added to a given pair of banks).
 *
 * \param [in] unit         - Device ID
 * \param [in] stage        - Handler's stage
 * \param [in] bank_id      - bank_id to be checked
 * \param [in] handler_id   - handler_id to be checked if it's suitable to be added to the given bank
 * \param [in] is_direct    - Whether this FG is direct table or not
 * \param [out] can_be_used - TRUE if given bank can be used for given handler id, FALSE otherwise
 *
 * \return
 *  \retval _SHR_E_NONE  - Success
 */
static shr_error_e
dnx_field_tcam_bank_algo_single_check(
    int unit,
    dnx_field_tcam_stage_e stage,
    int bank_id,
    uint32 handler_id,
    uint8 is_direct,
    uint8 *can_be_used)
{
    dnx_field_tcam_stage_e bank_owner_stage;
    uint8 handler_in_bank;
    uint8 full;
    uint8 is_free;
    uint8 collide;
    int core;
    uint32 key_size;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * **** Common FGs (DT/reg) checks ****
     */

    /*
     * We assume at the beginning that the bank can be used
     */
    *can_be_used = TRUE;

    /*
     * Check if bank is allocated for a different stage
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_algo_get_owner_stage(unit, bank_id, &bank_owner_stage));
    if (bank_owner_stage != DNX_FIELD_TCAM_STAGE_INVALID && bank_owner_stage != stage)
    {
        *can_be_used = FALSE;
        SHR_EXIT();
    }
    /*
     * Check if handler is already in bank
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_algo_handler_in_bank(unit, bank_id, handler_id, &handler_in_bank));
    if (handler_in_bank)
    {
        *can_be_used = FALSE;
        SHR_EXIT();
    }

    /*
     * **** IMPORTANT NOTE: The following 2 checks are not relevant for DT FGs, but since they are relevant for
     * special mode FGs, they're in the common checks ****
     */

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.key_size.get(unit, handler_id, &key_size));
    /*
     * Check if bank is not full (on all cores)
     */
    for (core = 0; core < dnx_data_device.general.nof_cores_get(unit); core++)
    {
        if (key_size == dnx_data_field.tcam.key_size_double_get(unit) && (bank_id % 2))
        {
            /** Skip odd banks availabilty check for double-sized key handlers */
            break;
        }
        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_is_full(unit, core, bank_id, handler_id, &full));
        if (full)
        {
            *can_be_used = FALSE;
            SHR_EXIT();
        }
    }

    /*
     * Check if handler's prefix is not occupied on the bank (only for non-DT)
     */
    if (!is_direct)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_is_free(unit, bank_id, handler_id, &is_free));
        if (!is_free)
        {
            *can_be_used = FALSE;
            SHR_EXIT();
        }
    }

    /*
     * Below function calls are for a more specific checks related to the FG type (DT/reg)
     */
    if (is_direct)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_algo_single_dt_check(unit, bank_id, can_be_used));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_algo_single_reg_check(unit, bank_id, can_be_used));
        /*
         * Check whether there is a same-context collision for this handler.
         * Same-context collision is a HW limitation in which no FGs that share the same context can share
         * the same TCAM bank
         */
        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_algo_check_context_collision(unit, bank_id, handler_id, &collide));
        if (collide)
        {
            *can_be_used = FALSE;
            SHR_EXIT();
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_bank_algo_find_id(
    int unit,
    uint32 handler_id,
    int *bank_id)
{
    dnx_field_tcam_access_fg_params_t fg_params;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, handler_id, &fg_params));

    if (fg_params.bank_allocation_mode != DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_AUTO)
    {
        /** Finding a bank to allocate is disabled for non-auto bank allocation modes */
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_DISABLED);
    }

    /*
     * Iterater over all banks to check which is suitable to be used for given handler_id
     */
    for (*bank_id = 0; *bank_id < dnx_data_field.tcam.nof_banks_get(unit);)
    {
        uint8 can_be_used;
        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_algo_single_check
                        (unit, fg_params.stage, *bank_id, handler_id, fg_params.direct_table, &can_be_used));
        if (can_be_used && fg_params.key_size == dnx_data_field.tcam.key_size_double_get(unit))
        {
            SHR_IF_ERR_EXIT(dnx_field_tcam_bank_algo_single_check
                            (unit, fg_params.stage, *bank_id + 1, handler_id, fg_params.direct_table, &can_be_used));
        }
        if (can_be_used)
        {
            break;
        }

        /*
         * For handlers with double-sized key, only even bank ids can be used
         */
        *bank_id = *bank_id + ((fg_params.key_size == dnx_data_field.tcam.key_size_double_get(unit)) ? 2 : 1);
    }

    if (*bank_id >= dnx_data_field.tcam.nof_banks_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_FULL, "No suitable bank could be found for handler: %d\n", handler_id);
    }

exit:
    SHR_FUNC_EXIT;
}
