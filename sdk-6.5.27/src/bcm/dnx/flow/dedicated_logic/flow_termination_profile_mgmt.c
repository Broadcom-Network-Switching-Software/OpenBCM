#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <bcm_int/dnx/flow/flow.h>
#include <bcm_int/dnx/mpls/mpls_tunnel_encap.h>
#include <bcm_int/dnx/mpls/mpls_tunnel_term.h>
#include <bcm_int/dnx/mpls/mpls.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_mpls_access.h>
#include <src/bcm/dnx/mpls_port/mpls_port.h>

/********************** MPLS_TERMINATION_PROFILE ***********************/

static shr_error_e
dnx_flow_mpls_utils_terminator_term_profile_delete(
    int unit,
    uint32 current_entry_handle_id)
{
    uint32 termination_profile = 0;
    uint8 is_last;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT_NO_MSG(dbal_entry_handle_value_field32_get
                           (unit, current_entry_handle_id, DBAL_FIELD_TERMINATION_TYPE, INST_SINGLE,
                            &termination_profile));

    SHR_IF_ERR_EXIT(algo_mpls_db.termination_profile.free_single(unit, termination_profile, &is_last));

    /** Delete profile termination entry from the HW table */
    if (is_last)
    {
        SHR_IF_ERR_EXIT(dnx_mpls_hw_termination_profile_delete(unit, termination_profile));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_mpls_utils_terminator_term_profile_set(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    uint32 current_entry_handle_id,
    uint32 prev_entry_handle_id,
    bcm_flow_special_fields_t * special_fields,
    uint8 *is_last_term_profile_reference)
{
    uint32 termination_profile_old;
    int termination_profile = 0;
    int ref_count, actual_nof_special_fields;
    uint8 is_first_term_profile_reference = 0;
    dnx_mpls_termination_profile_t term_profile_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&term_profile_info, 0, sizeof(dnx_mpls_termination_profile_t));

    /** if replace, first get the profile, update the enablers, and then read the new inputs */
    if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE))
    {
        SHR_IF_ERR_EXIT_NO_MSG(dbal_entry_handle_value_field32_get
                               (unit, prev_entry_handle_id, DBAL_FIELD_TERMINATION_TYPE, INST_SINGLE,
                                &termination_profile_old));
        SHR_IF_ERR_EXIT(algo_mpls_db.termination_profile.profile_data_get(unit, termination_profile_old, &ref_count,
                                                                          &term_profile_info));
    }
    else
    {
        term_profile_info.labels_to_terminate = ONE_MPLS_LABEL_TO_TERMINATE;
        term_profile_info.ttl_exp_label_index = TTL_EXP_CURRENT_MPLS_LABEL;
    }

    if (special_fields == NULL)
    {
        actual_nof_special_fields = 0;
    }
    else
    {
        actual_nof_special_fields = special_fields->actual_nof_special_fields;
    }

    for (int i = 0; i < actual_nof_special_fields; i++)
    {
        if (special_fields->special_fields[i].field_id == FLOW_S_F_MPLS_EXPECT_BOS)
        {
            term_profile_info.expect_bos = (special_fields->special_fields[i].is_clear == 0) ? TRUE : FALSE;
            /** Check bos if it is expected.*/
            term_profile_info.check_bos = (special_fields->special_fields[i].is_clear == 0) ? TRUE : FALSE;
        }
        if (special_fields->special_fields[i].field_id == FLOW_S_F_MPLS_EXPECT_NON_BOS)
        {
            term_profile_info.expect_bos = FALSE;
            /** Check bos if it is expected.*/
            term_profile_info.check_bos = (special_fields->special_fields[i].is_clear == 0) ? TRUE : FALSE;
        }
        else if (special_fields->special_fields[i].field_id == FLOW_S_F_MPLS_TRAP_TTL_0)
        {
            term_profile_info.reject_ttl_0 = (special_fields->special_fields[i].is_clear == 0) ? TRUE : FALSE;
        }
        else if (special_fields->special_fields[i].field_id == FLOW_S_F_MPLS_TRAP_TTL_1)
        {
            term_profile_info.reject_ttl_1 = (special_fields->special_fields[i].is_clear == 0) ? TRUE : FALSE;
        }
        else if (special_fields->special_fields[i].field_id == FLOW_S_F_MPLS_CONTROL_WORD_ENABLE)
        {
            term_profile_info.has_cw = (special_fields->special_fields[i].is_clear == 0) ? TRUE : FALSE;
        }
        /** If EL, two label at most should be terminated. While if it's terminating more(e.g.IML), never override it.*/
        else if (special_fields->special_fields[i].field_id == FLOW_S_F_MPLS_ENTROPY_LABEL &&
                 (term_profile_info.labels_to_terminate <= TWO_MPLS_LABELS_TO_TERMINATE))
        {
            if (special_fields->special_fields[i].is_clear &&
                term_profile_info.labels_to_terminate == TWO_MPLS_LABELS_TO_TERMINATE)
            {
                /**If not IML, and EL is clear, reset the labels_to_terminate.*/
                term_profile_info.labels_to_terminate = ONE_MPLS_LABEL_TO_TERMINATE;
            }
            else if (!special_fields->special_fields[i].is_clear)
            {
                /**If not IML, and EL is set, set the labels_to_terminate to 2.*/
                term_profile_info.labels_to_terminate = TWO_MPLS_LABELS_TO_TERMINATE;
            }
        }
        /** If IML and term_till_bos is supported, will try to terminate as more labels as possible.*/
        else if (special_fields->special_fields[i].field_id == FLOW_S_F_MPLS_INCLUSIVE_MULTICAST_LABEL &&
                 dnx_data_mpls.general.feature_get(unit, dnx_data_mpls_general_mpls_term_till_bos))
        {
            if (special_fields->special_fields[i].is_clear &&
                term_profile_info.labels_to_terminate == TILL_BOS_MPLS_LABELS_TO_TERMINATE)
            {
                /** If it's clearing IML flag, reset the labels_to_terminate*/
                term_profile_info.labels_to_terminate = ONE_MPLS_LABEL_TO_TERMINATE;
            }
            else if (!special_fields->special_fields[i].is_clear)
            {
                /** If it's setting IML flag, set the labels_to_terminate with maximum.*/
                term_profile_info.labels_to_terminate = TILL_BOS_MPLS_LABELS_TO_TERMINATE;
            }
        }
        else if (special_fields->special_fields[i].field_id == FLOW_S_F_MPLS_VCCV_TYPE)
        {
            if ((special_fields->special_fields[i].is_clear == 0)
                && (special_fields->special_fields[i].symbol == bcmMplsPortControlChannelTtl))
            {
                term_profile_info.reject_ttl_0 = 1;
                term_profile_info.reject_ttl_1 = 1;
            }
            else
            {
                term_profile_info.reject_ttl_0 = 0;
                term_profile_info.reject_ttl_1 = 0;
            }
        }
    }

    if (term_profile_info.labels_to_terminate > THREE_MPLS_LABEL_TO_TERMINATE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Trying to add too many MPLS label terminations to a single object\n");
    }

    if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE))
    {
        SHR_IF_ERR_EXIT(dnx_mpls_termination_profile_allocation
                        (unit, term_profile_info, &termination_profile, termination_profile_old,
                         &is_first_term_profile_reference, is_last_term_profile_reference));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_mpls_termination_profile_allocation
                        (unit, term_profile_info, &termination_profile, 0, &is_first_term_profile_reference, NULL));
    }
    if (is_first_term_profile_reference)
    {
        /** Set profile termination in case it is the first time. */
        SHR_IF_ERR_EXIT(dnx_mpls_termination_profile_hw_set(unit, &term_profile_info, termination_profile));
    }

    /** set termination profile */
    dbal_entry_value_field32_set(unit, current_entry_handle_id, DBAL_FIELD_TERMINATION_TYPE,
                                 INST_SINGLE, termination_profile);
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_dl_ingress_mpls_termination_profile_get(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    uint32 current_entry_handle_id,
    bcm_flow_special_fields_t * special_fields)
{
    uint32 termination_profile = 0;
    dnx_mpls_termination_profile_t term_profile_info;
    int ref_count;
    int is_mpls_tunnel = 0, is_mpls_port = 0;
    const dnx_flow_app_config_t *flow_app_cfg;

    SHR_FUNC_INIT_VARS(unit);

    flow_app_cfg = dnx_flow_app_info_get(unit, flow_handle_info->flow_handle);
    if (!sal_strncasecmp(FLOW_APP_NAME_MPLS_TUNNEL_TERMINATOR, flow_app_cfg->app_name, FLOW_STR_MAX))
    {
        is_mpls_tunnel = 1;
    }
    else if (!sal_strncasecmp(FLOW_APP_NAME_MPLS_PORT_TERMINATOR, flow_app_cfg->app_name, FLOW_STR_MAX))
    {
        is_mpls_port = 1;
    }

    SHR_IF_ERR_EXIT_NO_MSG(dbal_entry_handle_value_field32_get
                           (unit, current_entry_handle_id, DBAL_FIELD_TERMINATION_TYPE, INST_SINGLE,
                            &termination_profile));
    SHR_IF_ERR_EXIT(algo_mpls_db.
                    termination_profile.profile_data_get(unit, termination_profile, &ref_count, &term_profile_info));

    if (is_mpls_port)
    {
        if (term_profile_info.reject_ttl_0 && term_profile_info.reject_ttl_1)
        {
            SPECIAL_FIELD_SYMBOL_DATA_ADD(special_fields, FLOW_S_F_MPLS_VCCV_TYPE, bcmMplsPortControlChannelTtl);
        }
    }
    else if (is_mpls_tunnel)
    {
        if (term_profile_info.reject_ttl_0)
        {
            SPECIAL_FIELD_ENABLER_ADD(special_fields, FLOW_S_F_MPLS_TRAP_TTL_0);
        }
        if (term_profile_info.reject_ttl_1)
        {
            SPECIAL_FIELD_ENABLER_ADD(special_fields, FLOW_S_F_MPLS_TRAP_TTL_1);
        }
    }
    if (term_profile_info.expect_bos && term_profile_info.check_bos)
    {
        SPECIAL_FIELD_ENABLER_ADD(special_fields, FLOW_S_F_MPLS_EXPECT_BOS);
    }
    else if (term_profile_info.check_bos)
    {
        SPECIAL_FIELD_ENABLER_ADD(special_fields, FLOW_S_F_MPLS_EXPECT_NON_BOS);
    }
    if (term_profile_info.labels_to_terminate == TWO_MPLS_LABELS_TO_TERMINATE)
    {
        SPECIAL_FIELD_ENABLER_ADD(special_fields, FLOW_S_F_MPLS_ENTROPY_LABEL);
    }
    else if (term_profile_info.labels_to_terminate == TILL_BOS_MPLS_LABELS_TO_TERMINATE &&
             dnx_data_mpls.general.feature_get(unit, dnx_data_mpls_general_mpls_term_till_bos))
    {
        SPECIAL_FIELD_ENABLER_ADD(special_fields, FLOW_S_F_MPLS_INCLUSIVE_MULTICAST_LABEL);
    }
    if (term_profile_info.has_cw)
    {
        SPECIAL_FIELD_ENABLER_ADD(special_fields, FLOW_S_F_MPLS_CONTROL_WORD_ENABLE);
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_dl_ingress_mpls_termination_profile_destroy(
    int unit,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_handle_info_t * flow_handle_info,
    uint32 entry_handle_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_flow_mpls_utils_terminator_term_profile_delete(unit, entry_handle_id));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_dl_ingress_mpls_termination_profile_create(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_special_fields_t * special_fields)
{
    uint8 is_last_term_profile_reference;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_flow_mpls_utils_terminator_term_profile_set
                    (unit, flow_handle_info, entry_handle_id, -1, special_fields, &is_last_term_profile_reference));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_dl_ingress_mpls_termination_profile_replace(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    uint32 entry_handle_id,
    uint32 get_entry_handle_id,
    bcm_flow_special_fields_t * special_fields,
    int *last_profile)
{
    uint8 is_last_term_profile_reference;

    SHR_FUNC_INIT_VARS(unit);

    (*last_profile) = FLOW_PROFILE_INVALID;

    SHR_IF_ERR_EXIT(dnx_flow_mpls_utils_terminator_term_profile_set
                    (unit, flow_handle_info, entry_handle_id, get_entry_handle_id, special_fields,
                     &is_last_term_profile_reference));

    if (is_last_term_profile_reference)
    {
        uint32 termination_profile_old;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, get_entry_handle_id, DBAL_FIELD_TERMINATION_TYPE, INST_SINGLE,
                         &termination_profile_old));
        (*last_profile) = termination_profile_old;
    }

exit:
    SHR_FUNC_EXIT;
}
