
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#include <soc/dnx/field/tcam/tcam_bank_algo.h>

#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/field/field_group.h>
#include <src/bcm/dnx/init/init_pemla.h>
#include <soc/dnx/field/tcam/tcam_bank_manager.h>
#include <soc/dnx/field/tcam/tcam_prefix_manager.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX

shr_error_e
dnx_field_tcam_bank_algo_check_context_collision(
    int unit,
    int bank_id,
    uint32 handler_id,
    bcm_core_t core_id,
    uint8 *collide)
{
    uint32 *handler_p;
    uint32 tcam_handler_ids[DNX_DATA_MAX_FIELD_TCAM_NOF_TCAM_HANDLERS + 1] = { DNX_FIELD_TCAM_HANDLER_INVALID };
    uint8 handler_in_bank;
    dnx_field_tcam_access_fg_params_t fg_params;

    SHR_FUNC_INIT_VARS(unit);

    *collide = FALSE;

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, handler_id, &fg_params));

    if (fg_params.stage != DNX_FIELD_TCAM_STAGE_IPMF1 &&
        fg_params.stage != DNX_FIELD_TCAM_STAGE_IPMF2 &&
        fg_params.stage != DNX_FIELD_TCAM_STAGE_IPMF3 && fg_params.stage != DNX_FIELD_TCAM_STAGE_EPMF)
    {

        SHR_IF_ERR_EXIT(dnx_init_pemla_context_sharing_handler_ids_get_cb(unit, handler_id, tcam_handler_ids));
    }
    else
    {

        SHR_IF_ERR_EXIT(dnx_field_group_context_sharing_tcam_handlers_get(unit, handler_id, tcam_handler_ids));
    }

    for (handler_p = &tcam_handler_ids[0]; *handler_p != DNX_FIELD_TCAM_HANDLER_INVALID; handler_p++)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_algo_handler_in_bank(unit, bank_id, *handler_p, core_id, &handler_in_bank));
        if (handler_in_bank)
        {

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
    bcm_core_t core_id,
    sw_state_ll_node_t * handler_node)
{
    sw_state_ll_node_t node;
    uint32 val;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORE_VALIDATE(unit, core_id, FALSE);

    *handler_node = SW_STATE_LL_INVALID;

    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.active_handlers_id.get_first(unit, bank_id, core_id, &node));
    while (DNX_SW_STATE_LL_IS_NODE_VALID(node))
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.
                        active_handlers_id.node_value(unit, bank_id, core_id, node, &val));
        if (val == handler_id)
        {

            break;
        }
        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.
                        active_handlers_id.next_node(unit, bank_id, core_id, node, &node));
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
    bcm_core_t core_id,
    uint8 *handler_in_bank)
{
    sw_state_ll_node_t handler_node;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORE_VALIDATE(unit, core_id, FALSE);

    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_algo_get_handler_node_in_bank
                    (unit, bank_id, handler_id, core_id, &handler_node));

    *handler_in_bank = (DNX_SW_STATE_LL_IS_NODE_VALID(handler_node));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_bank_algo_single_dt_check(
    int unit,
    int bank_id,
    uint8 *can_be_used)
{
    int bank_mode;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.bank_mode.get(unit, bank_id, &bank_mode));

    *can_be_used = (bank_mode == DNX_FIELD_TCAM_BANK_MODE_NONE && FIELD_TCAM_BANK_IS_BIG_BANK(bank_id));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_bank_algo_single_reg_check(
    int unit,
    int bank_id,
    uint8 *can_be_used)
{
    int bank_mode;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.bank_mode.get(unit, bank_id, &bank_mode));

    if (bank_mode == DNX_FIELD_TCAM_BANK_MODE_RESTRICTED_DT ||
        bank_mode == DNX_FIELD_TCAM_BANK_MODE_RESTRICTED || bank_mode == DNX_FIELD_TCAM_BANK_MODE_RESTRICTED_LOCATION)
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

static shr_error_e
dnx_field_tcam_bank_algo_single_check(
    int unit,
    dnx_field_tcam_access_fg_params_t * fg_params,
    int bank_id,
    uint32 handler_id,
    bcm_core_t core_id,
    uint8 *can_be_used)
{
    dnx_field_tcam_stage_e bank_owner_stage;
    uint8 handler_in_bank;
    uint8 full;
    uint8 is_free;
    uint8 collide;
    uint32 key_size;
    dnx_field_tcam_stage_e stage = fg_params->stage;
    uint8 is_direct = fg_params->direct_table;

    SHR_FUNC_INIT_VARS(unit);

    *can_be_used = TRUE;

    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_algo_get_owner_stage(unit, bank_id, &bank_owner_stage));
    if (bank_owner_stage != DNX_FIELD_TCAM_STAGE_INVALID && bank_owner_stage != stage)
    {
        *can_be_used = FALSE;
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_algo_handler_in_bank(unit, bank_id, handler_id, core_id, &handler_in_bank));
    if (handler_in_bank)
    {
        *can_be_used = FALSE;
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.key_size.get(unit, handler_id, &key_size));

    if (key_size != dnx_data_field.tcam.key_size_double_get(unit) || (bank_id % 2 == 0))
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_is_full(unit, core_id, bank_id, handler_id, &full));
        if (full)
        {
            *can_be_used = FALSE;
            SHR_EXIT();
        }
    }

    if (!is_direct)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_is_free(unit, bank_id, handler_id, core_id, &is_free));
        if (!is_free)
        {
            *can_be_used = FALSE;
            SHR_EXIT();
        }
    }

    if (is_direct)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_algo_single_dt_check(unit, bank_id, can_be_used));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_algo_single_reg_check(unit, bank_id, can_be_used));

        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_algo_check_context_collision(unit, bank_id, handler_id, core_id, &collide));
        if (collide)
        {
            *can_be_used = FALSE;
            SHR_EXIT();
        }
    }

    if (!dnx_data_field.tcam.feature_get(unit, dnx_data_field_tcam_mixed_ratio_supported))
    {
        uint32 bank_key_size;

        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.key_size.get(unit, bank_id, &bank_key_size));
        if (bank_key_size != 0 && bank_key_size != key_size)
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
    bcm_core_t core_id,
    int *bank_id)
{
    dnx_field_tcam_access_fg_params_t fg_params;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, handler_id, &fg_params));

    if (fg_params.bank_allocation_mode != DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_AUTO)
    {

        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_DISABLED);
    }

    for (*bank_id = 0; *bank_id < dnx_data_field.tcam.nof_banks_get(unit);)
    {
        uint8 can_be_used = FALSE;
        bcm_core_t core_iter;

        DNXCMN_CORES_ITER(unit, core_id, core_iter)
        {
            SHR_IF_ERR_EXIT(dnx_field_tcam_bank_algo_single_check
                            (unit, &fg_params, *bank_id, handler_id, core_iter, &can_be_used));
            if (can_be_used && fg_params.key_size == dnx_data_field.tcam.key_size_double_get(unit))
            {
                SHR_IF_ERR_EXIT(dnx_field_tcam_bank_algo_single_check
                                (unit, &fg_params, *bank_id + 1, handler_id, core_iter, &can_be_used));
            }
            if (!can_be_used)
            {
                break;
            }
        }
        if (can_be_used)
        {
            break;
        }

        *bank_id = *bank_id + ((fg_params.key_size == dnx_data_field.tcam.key_size_double_get(unit)) ? 2 : 1);
    }

    if (*bank_id >= dnx_data_field.tcam.nof_banks_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_FULL, "No suitable bank could be found for handler: %d\n", handler_id);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_bank_algo_bank_contains_half_key_handler(
    int unit,
    int bank_id,
    bcm_core_t core_id,
    uint8 *contains_half_key_handler)
{
    sw_state_ll_node_t node;
    uint32 key_size;

    SHR_FUNC_INIT_VARS(unit);

    *contains_half_key_handler = FALSE;

    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.active_handlers_id.get_first(unit, bank_id, core_id, &node));
    while (DNX_SW_STATE_LL_IS_NODE_VALID(node))
    {
        uint32 handler_id;
        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.
                        active_handlers_id.node_value(unit, bank_id, core_id, node, &handler_id));
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.key_size.get(unit, handler_id, &key_size));
        if (key_size == dnx_data_field.tcam.key_size_half_get(unit))
        {
            *contains_half_key_handler = TRUE;
            break;
        }
        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.
                        active_handlers_id.next_node(unit, bank_id, core_id, node, &node));
    }

exit:
    SHR_FUNC_EXIT;
}
