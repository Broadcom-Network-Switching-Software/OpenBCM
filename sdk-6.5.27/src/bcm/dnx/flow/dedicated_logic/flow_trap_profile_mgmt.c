#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <bcm_int/dnx/flow/flow.h>
#include <include/bcm_int/dnx/rx/rx_trap.h>
#include <include/bcm_int/dnx/rx/rx_trap.h>
#include <include/bcm_int/dnx/algo/rx/algo_rx.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_rx_access.h>

/********************** INGRESSS_TRAP ***********************/

shr_error_e
dnx_flow_dl_ingress_trap_profile_verify(
    int unit,
    bcm_flow_terminator_info_t * terminator_info)
{
    uint32 action_gport = terminator_info->action_gport;
    int trap_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_MIRROR_MIRROR(action_gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Mirror configuration can only be done for ETPP lif traps");
    }

    if (!BCM_GPORT_IS_TRAP(action_gport) && (action_gport != BCM_GPORT_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The Trap-Gport is not valid");
    }

    if ((action_gport != BCM_GPORT_INVALID)
        && (BCM_GPORT_TRAP_GET_STRENGTH(action_gport) != 0) && (BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(action_gport) != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The Trap-Gport is not valid, "
                     "Snoop and Forwarding strengths cannot be both set to non-zero values.");
    }

    if (action_gport != BCM_GPORT_INVALID)
    {
        dnx_rx_trap_block_e trap_block = DNX_RX_TRAP_BLOCK_INVALID;
        trap_id = BCM_GPORT_TRAP_GET_ID(action_gport);
        trap_block = DNX_RX_TRAP_ID_BLOCK_GET(trap_id);

        if (trap_block != DNX_RX_TRAP_BLOCK_INGRESS)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "For lif types InLif and InRif, an IRPP trap_id must be provided! "
                         "The supplied trap_id 0x%08x is on trap_block=%d!", trap_id, trap_block);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_dl_ingress_trap_profile_get(
    int unit,
    uint32 entry_handle_id,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_flow_terminator_info_t * terminator_info,
    bcm_flow_special_fields_t * special_fields)
{
    uint32 lif_trap_profile;
    int ref_count;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    rv = dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE_IDX, INST_SINGLE,
                                             &lif_trap_profile);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);

    if (rv == _SHR_E_NOT_FOUND)
    {
        SHR_EXIT();
    }

    terminator_info->valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_ACTION_GPORT_VALID;
    SHR_IF_ERR_EXIT(algo_rx_db.trap_lif_ingress.profile_data_get(unit, lif_trap_profile, &ref_count,
                                                                 &terminator_info->action_gport));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_dl_ingress_trap_profile_create(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_terminator_info_t * terminator_info,
    bcm_flow_special_fields_t * special_fields)
{
    uint32 new_lif_trap_profile = 0;
    uint8 is_last = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (!_SHR_IS_FLAG_SET(terminator_info->valid_elements_set, BCM_FLOW_TERMINATOR_ELEMENT_ACTION_GPORT_VALID))
    {
        /** no trap related fields, just exit */
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_rx_trap_lif_ingress_trap_profile_allocate_and_hw_update
                    (unit, 0, terminator_info->action_gport, &new_lif_trap_profile, &is_last));

    /** Set the LIF trap profile */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE_IDX, INST_SINGLE,
                                 new_lif_trap_profile);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_dl_ingress_trap_profile_destroy(
    int unit,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_handle_info_t * flow_handle_info,
    uint32 entry_handle_id,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources)
{
    uint8 is_last = 0;
    uint32 old_lif_trap_profile = 0, new_lif_trap_profile = 0;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    rv = dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE_IDX, INST_SINGLE,
                                             &old_lif_trap_profile);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);

    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_IF_ERR_EXIT(dnx_rx_trap_lif_ingress_trap_profile_allocate_and_hw_update
                        (unit, old_lif_trap_profile, BCM_GPORT_INVALID, &new_lif_trap_profile, &is_last));
        if (is_last)
        {
            SHR_IF_ERR_EXIT(dnx_rx_trap_lif_ingress_action_profile_hw_clear(unit, old_lif_trap_profile));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_dl_ingress_trap_profile_replace(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    uint32 entry_handle_id,
    uint32 get_entry_handle_id,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_flow_terminator_info_t * terminator_info,
    bcm_flow_special_fields_t * special_fields,
    int *last_profile)
{
    uint8 is_last = 0;
    shr_error_e rv;
    uint32 old_lif_trap_profile = 0, new_lif_trap_profile = 0;
    int is_field_clear = FALSE, is_field_set = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    (*last_profile) = FLOW_PROFILE_INVALID;

    /** Get previous profile */
    rv = dbal_entry_handle_value_field32_get(unit, get_entry_handle_id, DBAL_FIELD_ACTION_PROFILE_IDX, INST_SINGLE,
                                             &old_lif_trap_profile);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);

    if (_SHR_IS_FLAG_SET(terminator_info->valid_elements_set, BCM_FLOW_TERMINATOR_ELEMENT_ACTION_GPORT_VALID))
    {
        is_field_set = TRUE;
    }

    if (_SHR_IS_FLAG_SET(terminator_info->valid_elements_clear, BCM_FLOW_TERMINATOR_ELEMENT_ACTION_GPORT_VALID))
    {
        is_field_clear = TRUE;
    }

    if (rv == _SHR_E_NONE)
    {
        if (!is_field_set && !is_field_clear)
        {
            /** user didn't modify the value, just copy the existing profile to the new handle */
            dbal_entry_value_field32_set
                (unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE_IDX, INST_SINGLE, old_lif_trap_profile);
            SHR_EXIT();
        }
        else
        {
            if (is_field_clear)
            {
                /** removing the profile - releasing the profile, no need to set the handle to default. */
                SHR_IF_ERR_EXIT(dnx_rx_trap_lif_ingress_trap_profile_allocate_and_hw_update
                                (unit, old_lif_trap_profile, BCM_GPORT_INVALID, &new_lif_trap_profile, &is_last));
                if (is_last)
                {
                    (*last_profile) = old_lif_trap_profile;
                }
            }
            else
            {
                /** replace old value with a new value */
                SHR_IF_ERR_EXIT(dnx_rx_trap_lif_ingress_trap_profile_allocate_and_hw_update
                                (unit, old_lif_trap_profile, terminator_info->action_gport, &new_lif_trap_profile,
                                 &is_last));
                if (is_last)
                {
                    (*last_profile) = old_lif_trap_profile;
                }
                /** Set the LIF trap profile */
                dbal_entry_value_field32_set
                    (unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE_IDX, INST_SINGLE, new_lif_trap_profile);
            }
        }
    }
    else
    {
        if (is_field_set)
        {
            SHR_IF_ERR_EXIT(dnx_flow_dl_ingress_trap_profile_create
                            (unit, entry_handle_id, flow_app_info, terminator_info, special_fields));

        }
    }

exit:
    SHR_FUNC_EXIT;
}

/********************** EGRESSS_TRAP ***********************/

shr_error_e
dnx_flow_dl_egress_trap_profile_verify(
    int unit,
    bcm_flow_initiator_info_t * initiator_info)
{
    uint32 action_gport = initiator_info->action_gport;
    int trap_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_MIRROR_MIRROR(action_gport))
    {
        /** Mirror valid case, no need for additional checks */
        SHR_EXIT();
    }

    if (!BCM_GPORT_IS_TRAP(action_gport) && (action_gport != BCM_GPORT_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The Trap-Gport is not valid");
    }

    if ((action_gport != BCM_GPORT_INVALID)
        && (BCM_GPORT_TRAP_GET_STRENGTH(action_gport) != 0) && (BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(action_gport) != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The Trap-Gport is not valid, "
                     "Snoop and Forwarding strengths cannot be both set to non-zero values.");
    }

    if (action_gport != BCM_GPORT_INVALID)
    {
        dnx_rx_trap_block_e trap_block = DNX_RX_TRAP_BLOCK_INVALID;
        trap_id = BCM_GPORT_TRAP_GET_ID(action_gport);
        trap_block = DNX_RX_TRAP_ID_BLOCK_GET(trap_id);

        if (trap_block != DNX_RX_TRAP_BLOCK_ETPP)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "For lif types OutLif and OutRif, an ETPP trap_id must be provided! "
                         "The supplied trap_id 0x%08x is on trap_block=%d!", trap_id, trap_block);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_dl_egress_trap_profile_get(
    int unit,
    uint32 entry_handle_id,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields)
{
    uint32 lif_trap_profile;
    int ref_count;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    rv = dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE, INST_SINGLE,
                                             &lif_trap_profile);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);

    if (rv == _SHR_E_NOT_FOUND)
    {
        SHR_EXIT();
    }

    initiator_info->valid_elements_set |= BCM_FLOW_INITIATOR_ELEMENT_ACTION_GPORT_VALID;
    SHR_IF_ERR_EXIT(algo_rx_db.
                    trap_lif_etpp.profile_data_get(unit, lif_trap_profile, &ref_count, &initiator_info->action_gport));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_dl_egress_trap_profile_create(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields)
{
    uint32 lif_trap_profile = 0;
    uint8 is_last = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (!_SHR_IS_FLAG_SET(initiator_info->valid_elements_set, BCM_FLOW_INITIATOR_ELEMENT_ACTION_GPORT_VALID))
    {
        /** no trap related fields, just exit */
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_rx_trap_lif_etpp_trap_profile_allocate_and_hw_update
                    (unit, 0, initiator_info->action_gport, &lif_trap_profile, &is_last));

    /** Set the LIF trap profile */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE, INST_SINGLE, lif_trap_profile);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_dl_egress_trap_profile_destroy(
    int unit,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_handle_info_t * flow_handle_info,
    uint32 entry_handle_id,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources)
{
    uint8 is_last = 0;
    uint32 old_lif_trap_profile = 0, new_lif_trap_profile = 0;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    rv = dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE, INST_SINGLE,
                                             &old_lif_trap_profile);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);

    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_IF_ERR_EXIT(dnx_rx_trap_lif_etpp_trap_profile_allocate_and_hw_update
                        (unit, old_lif_trap_profile, BCM_GPORT_INVALID, &new_lif_trap_profile, &is_last));
        if (is_last)
        {
            SHR_IF_ERR_EXIT(dnx_rx_trap_lif_etpp_trap_action_profile_hw_clear(unit, old_lif_trap_profile));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_dl_egress_trap_profile_replace(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    uint32 entry_handle_id,
    uint32 get_entry_handle_id,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields,
    int *last_profile)
{
    uint8 is_last = 0;
    shr_error_e rv;
    uint32 old_lif_trap_profile = 0, new_lif_trap_profile = 0;
    int is_field_clear = FALSE, is_field_set = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    (*last_profile) = FLOW_PROFILE_INVALID;

    /** Get previous profile */
    rv = dbal_entry_handle_value_field32_get(unit, get_entry_handle_id, DBAL_FIELD_ACTION_PROFILE, INST_SINGLE,
                                             &old_lif_trap_profile);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);

    if (_SHR_IS_FLAG_SET(initiator_info->valid_elements_set, BCM_FLOW_INITIATOR_ELEMENT_ACTION_GPORT_VALID))
    {
        is_field_set = TRUE;
    }

    if (_SHR_IS_FLAG_SET(initiator_info->valid_elements_clear, BCM_FLOW_INITIATOR_ELEMENT_ACTION_GPORT_VALID))
    {
        is_field_clear = TRUE;
    }

    if (rv == _SHR_E_NONE)
    {
        if (!is_field_set && !is_field_clear)
        {
            /** user didn't modify the value, just copy the existing profile to the new handle */
            dbal_entry_value_field32_set
                (unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE, INST_SINGLE, old_lif_trap_profile);
            SHR_EXIT();
        }
        else
        {
            if (is_field_clear)
            {
                /** removing the profile - releasing the profile, no need to set the handle to default. */
                SHR_IF_ERR_EXIT(dnx_rx_trap_lif_etpp_trap_profile_allocate_and_hw_update
                                (unit, old_lif_trap_profile, BCM_GPORT_INVALID, &new_lif_trap_profile, &is_last));
                if (is_last)
                {
                    (*last_profile) = old_lif_trap_profile;
                }
            }
            else
            {
                /** replace old value with a new value */
                SHR_IF_ERR_EXIT(dnx_rx_trap_lif_etpp_trap_profile_allocate_and_hw_update
                                (unit, old_lif_trap_profile, initiator_info->action_gport, &new_lif_trap_profile,
                                 &is_last));

                if (is_last)
                {
                    (*last_profile) = old_lif_trap_profile;
                }
                /** Set the LIF trap profile */
                dbal_entry_value_field32_set
                    (unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE, INST_SINGLE, new_lif_trap_profile);
            }
        }
    }
    else
    {
        if (is_field_set)
        {
            SHR_IF_ERR_EXIT(dnx_flow_dl_egress_trap_profile_create
                            (unit, entry_handle_id, flow_app_info, initiator_info, special_fields));

        }
    }

exit:
    SHR_FUNC_EXIT;
}
