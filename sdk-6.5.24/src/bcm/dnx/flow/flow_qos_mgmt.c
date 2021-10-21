#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <bcm_int/dnx/flow/flow.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_qos_access.h>
#include <bcm/qos.h>
#include <bcm_int/dnx/qos/qos.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/flow/flow_lif_mgmt.h>

static shr_error_e
dnx_flow_init_ttl_create(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields)
{
    shr_error_e rv;
    bcm_flow_special_field_t special_field_data;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
    rv = dnx_flow_special_field_data_get(unit, special_fields, FLOW_S_F_QOS_TTL, &special_field_data);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    if (rv == _SHR_E_NONE)
    {
        if (initiator_info->egress_qos_model.egress_ttl == bcmQosEgressModelPipeMyNameSpace)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_MODE, INST_SINGLE,
                                         DBAL_ENUM_FVAL_TTL_MODE_PIPE);
        }
        else
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_MODE, INST_SINGLE,
                                         DBAL_ENUM_FVAL_TTL_MODE_UNIFORM);
        }

        /** Allocate TTL model */
        if (initiator_info->egress_qos_model.egress_ttl == bcmQosEgressModelPipeMyNameSpace)
        {
            uint8 is_first_ttl_profile_ref;
            uint32 ttl_pipe_profile;

            /** allocate TTL PIPE profile */
            SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_ttl_pipe_profile.allocate_single
                            (unit, 0, (int *) &special_field_data.shr_var_uint32, NULL, (int *) &ttl_pipe_profile,
                             &is_first_ttl_profile_ref));

            /** Write to HW if new TTL profile was allocated */
            if (is_first_ttl_profile_ref)
            {
                SHR_IF_ERR_EXIT(dnx_qos_egress_ttl_profile_hw_set
                                (unit, ttl_pipe_profile, special_field_data.shr_var_uint32));
            }
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_PIPE_PROFILE, INST_SINGLE,
                                         ttl_pipe_profile);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_init_qos_app_params_create(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields)
{
    uint8 field_idx;
    uint8 nwk_qos_idx = FALSE, nwk_pri_cfi_en = FALSE, nwk_qos_enabled = FALSE;
    uint8 nwk_qos_pri = 0, nwk_qos_cfi = 0;

    SHR_FUNC_INIT_VARS(unit);

    for (field_idx = 0; field_idx < special_fields->actual_nof_special_fields; field_idx++)
    {
        if ((special_fields->special_fields[field_idx].field_id == FLOW_S_F_QOS_DSCP) ||
            (special_fields->special_fields[field_idx].field_id == FLOW_S_F_QOS_EXP))
        {
            nwk_qos_idx = special_fields->special_fields[field_idx].shr_var_uint32;
            nwk_qos_enabled = TRUE;
            break;
        }
        else if (special_fields->special_fields[field_idx].field_id == FLOW_S_F_QOS_PRI)
        {
            nwk_qos_pri = special_fields->special_fields[field_idx].shr_var_uint32;
            nwk_pri_cfi_en = TRUE;
        }
        else if (special_fields->special_fields[field_idx].field_id == FLOW_S_F_QOS_CFI)
        {
            nwk_qos_cfi = special_fields->special_fields[field_idx].shr_var_uint32;
            nwk_pri_cfi_en = TRUE;
        }
    }

    if (nwk_pri_cfi_en == TRUE)
    {
        nwk_qos_idx = DNX_QOS_IDX_PCP_DEI_TO_NWK_QOS(nwk_qos_pri, nwk_qos_cfi);
        nwk_qos_enabled = TRUE;
    }

    if (nwk_qos_enabled)
    {
        /** QOS PIPE Model */
        if ((initiator_info->egress_qos_model.egress_qos != bcmQosEgressModelUniform))
        {
            uint32 nwk_qos_profile;
            uint8 is_first_pipe_propag_profile_ref;

            SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_allocate
                            (unit, nwk_qos_idx, nwk_qos_idx, 0, (int *) &nwk_qos_profile,
                             &is_first_pipe_propag_profile_ref));

            if (is_first_pipe_propag_profile_ref)
            {
                SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_set
                                (unit, nwk_qos_profile, nwk_qos_idx, nwk_qos_idx, 0));
            }

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NWK_QOS_IDX, INST_SINGLE, nwk_qos_profile);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_init_dedicated_logic_qos_create(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields)
{
    dbal_enum_value_field_encap_qos_model_e encap_qos_model;

    SHR_FUNC_INIT_VARS(unit);

    if (special_fields == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "special_fields struct must exists");
    }

    /** treat with the ttl field */
    SHR_IF_ERR_EXIT(dnx_flow_init_ttl_create(unit, entry_handle_id, initiator_info, special_fields));

    /** treat with the rest of the qos fields */
    SHR_IF_ERR_EXIT(dnx_flow_init_qos_app_params_create(unit, entry_handle_id, initiator_info, special_fields));

    /** treat with the common field QOS_EGRESS_MODEL data */
    SHR_IF_ERR_EXIT(dnx_qos_egress_model_bcm_to_dbal(unit, &initiator_info->egress_qos_model, &encap_qos_model));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, INST_SINGLE, encap_qos_model);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_init_ttl_replace(
    int unit,
    uint32 entry_handle_id,
    uint32 get_entry_handle_id,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields,
    uint8 *is_pipe_mode_old,
    uint8 *is_pipe_mode_new,
    uint32 *old_profile,
    uint32 *new_profile)
{
    uint32 ttl_mode;
    uint8 remove_old_profile = FALSE;
    uint8 is_field_valid;
    uint8 is_first_profile_ref = FALSE, is_last_profile_ref = FALSE;
    shr_error_e rv;
    bcm_flow_special_field_t special_field_data;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, get_entry_handle_id, DBAL_FIELD_TTL_MODE, &is_field_valid, &ttl_mode));
    if (is_field_valid == TRUE)
    {
        if (ttl_mode == DBAL_ENUM_FVAL_TTL_MODE_PIPE)
        {
            (*is_pipe_mode_old) = TRUE;

            /** Get TTL PIPE profile */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, get_entry_handle_id, DBAL_FIELD_TTL_PIPE_PROFILE, INST_SINGLE, old_profile));
            *new_profile = (*old_profile);
        }
    }

    sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
    rv = dnx_flow_special_field_data_get(unit, special_fields, FLOW_S_F_QOS_TTL, &special_field_data);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    if (rv == _SHR_E_NONE)
    {
        if (initiator_info->egress_qos_model.egress_ttl == bcmQosEgressModelUniform)
        {
            (*is_pipe_mode_new) = FALSE;
        }

        /** Allocate TTL template */
        if (((*is_pipe_mode_old) == TRUE) && ((*is_pipe_mode_new) == TRUE))
        {
            /** In this case the old and new TTL model is PIPE - exchange templates */

            /** Exchange TTL PIPE profile */
            SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_ttl_pipe_profile.exchange
                            (unit, 0, (int *) &special_field_data.shr_var_uint32, *old_profile, NULL,
                             (int *) new_profile, &is_first_profile_ref, &is_last_profile_ref));

            /** If last profile was deallocated remove the old entry from HW */
            if (is_last_profile_ref)
            {
                remove_old_profile = TRUE;
            }
        }
        else if (((*is_pipe_mode_old) == FALSE) && ((*is_pipe_mode_new) == TRUE))
        {
            /** In this case the old model is UNIFORM and new TTL model is PIPE - allocate new profile */

            /** allocate TTL PIPE profile */
            SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_ttl_pipe_profile.allocate_single
                            (unit, 0, (int *) &special_field_data.shr_var_uint32, NULL, (int *) new_profile,
                             &is_first_profile_ref));
        }
        else if (((*is_pipe_mode_old) == TRUE) && ((*is_pipe_mode_new) == FALSE))
        {
            /** In this case the old model is PIPE and new TTL model is UNIFORM - remove old template */

            /** Free old TTL profile template */
            SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_ttl_pipe_profile.free_single
                            (unit, *old_profile, &is_last_profile_ref));

            /** If last profile was deallocated remove the entry from HW */
            if (is_last_profile_ref)
            {
                remove_old_profile = TRUE;
            }
        }

        /** Write to HW if new TTL profile was allocated */
        if (is_first_profile_ref)
        {
            SHR_IF_ERR_EXIT(dnx_qos_egress_ttl_profile_hw_set(unit, *new_profile, special_field_data.shr_var_uint32));
        }
       /** Remove TTL entry from HW */
        if (remove_old_profile)
        {
            SHR_IF_ERR_EXIT(dnx_qos_egress_ttl_profile_hw_clear(unit, *old_profile));
        }
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_PIPE_PROFILE, INST_SINGLE, *new_profile);
        if (*is_pipe_mode_new)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_MODE, INST_SINGLE,
                                         DBAL_ENUM_FVAL_TTL_MODE_PIPE);
        }
    }
    else
    {
        if (is_field_valid)
        {
            /** Set old TTL PIPE profile */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_PIPE_PROFILE, INST_SINGLE, *old_profile);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_init_qos_app_params_replace(
    int unit,
    uint32 entry_handle_id,
    uint32 get_entry_handle_id,
    uint8 is_pipe_mode_old,
    uint8 is_pipe_mode_new,
    uint32 old_profile,
    uint32 new_profile,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields)
{
    uint8 is_field_valid = FALSE;
    uint8 is_first_profile_ref = FALSE, is_last_profile_ref = FALSE;
    uint8 field_idx;
    uint8 nwk_qos_idx = FALSE, nwk_pri_cfi_en = FALSE, nwk_qos_enabled = FALSE;
    uint8 nwk_qos_pri = 0, nwk_qos_cfi = 0;
    dbal_enum_value_field_encap_qos_model_e encap_qos_model;
    bcm_qos_egress_model_t model;

    SHR_FUNC_INIT_VARS(unit);

    for (field_idx = 0; field_idx < special_fields->actual_nof_special_fields; field_idx++)
    {
        if ((special_fields->special_fields[field_idx].field_id == FLOW_S_F_QOS_DSCP) ||
            (special_fields->special_fields[field_idx].field_id == FLOW_S_F_QOS_EXP))
        {
            nwk_qos_idx = special_fields->special_fields[field_idx].shr_var_uint32;
            nwk_qos_enabled = TRUE;
            break;
        }
        else if (special_fields->special_fields[field_idx].field_id == FLOW_S_F_QOS_PRI)
        {
            nwk_qos_pri = special_fields->special_fields[field_idx].shr_var_uint32;
            nwk_pri_cfi_en = TRUE;
        }
        else if (special_fields->special_fields[field_idx].field_id == FLOW_S_F_QOS_CFI)
        {
            nwk_qos_cfi = special_fields->special_fields[field_idx].shr_var_uint32;
            nwk_pri_cfi_en = TRUE;
        }
    }

    if (nwk_pri_cfi_en == TRUE)
    {
        nwk_qos_idx = DNX_QOS_IDX_PCP_DEI_TO_NWK_QOS(nwk_qos_pri, nwk_qos_cfi);
        nwk_qos_enabled = TRUE;
    }

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, get_entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, &is_field_valid, &encap_qos_model));
    if (is_field_valid == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_model_dbal_to_bcm(unit, encap_qos_model, &model));

        if ((encap_qos_model == DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_NEXT_SPACE) ||
            (encap_qos_model == DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_MY_SPACE) ||
            (encap_qos_model == DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_ECN))
        {
            /** Get QOS pipe index */
            is_pipe_mode_old = TRUE;
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, get_entry_handle_id, DBAL_FIELD_NWK_QOS_IDX,
                                                                INST_SINGLE, &old_profile));
        }
    }

    if (nwk_qos_enabled)
    {
        /** Exchange QOS idx */
        if (is_pipe_mode_old && is_pipe_mode_new)
        {
            /** In this case the old and new model is PIPE - exchange templates */

            /** Exchange QOS PIPE profile */
            new_profile = old_profile;
            SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_update
                            (unit, (uint16) nwk_qos_idx, (uint16) nwk_qos_idx, 0, (int *) &new_profile,
                             &is_first_profile_ref, &is_last_profile_ref));
            if (is_first_profile_ref)
            {
                SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_set
                                (unit, new_profile, (uint8) nwk_qos_idx, (uint8) nwk_qos_idx, 0));
            }
        }
        else if (!is_pipe_mode_old && is_pipe_mode_new)
        {
            /** In this case the old model is UNIFORM and new model is PIPE - allocate new profile */

            /** allocate QOS PIPE profile */
            SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_allocate
                            (unit, (uint8) nwk_qos_idx, (uint8) nwk_qos_idx, 0, (int *) &new_profile,
                             &is_first_profile_ref));
            if (is_first_profile_ref)
            {
                SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_set
                                (unit, new_profile, (uint8) nwk_qos_idx, (uint8) nwk_qos_idx, 0));
            }
        }
        else if (is_pipe_mode_old && !is_pipe_mode_new)
        {
            /** In this case the old model is PIPE and new model is UNIFORM - remove old template */

            /** Free old QOS profile template */
            SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_free
                            (unit, (int) old_profile, &is_last_profile_ref));
        }

        if (is_last_profile_ref)
        {
            SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_hw_clear(unit, old_profile));
        }
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NWK_QOS_IDX, INST_SINGLE, new_profile);
        if (is_pipe_mode_new)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, INST_SINGLE,
                                         encap_qos_model);
        }
    }
    else
    {
        if (is_field_valid)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NWK_QOS_IDX, INST_SINGLE, old_profile);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_init_qos_replace(
    int unit,
    uint32 entry_handle_id,
    uint32 get_entry_handle_id,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields)
{
    uint8 is_pipe_mode_old = FALSE;
    uint8 is_pipe_mode_new = TRUE;
    uint32 old_profile = 0;
    uint32 new_profile = 0;
    dbal_enum_value_field_encap_qos_model_e encap_qos_model;

    SHR_FUNC_INIT_VARS(unit);

    /** treat with the ttl */
    SHR_IF_ERR_EXIT(dnx_flow_init_ttl_replace
                    (unit, entry_handle_id, get_entry_handle_id, initiator_info, special_fields,
                     &is_pipe_mode_old, &is_pipe_mode_new, &old_profile, &new_profile));

    /** treat with the qos app params */
    SHR_IF_ERR_EXIT(dnx_flow_init_qos_app_params_replace
                    (unit, entry_handle_id, get_entry_handle_id, is_pipe_mode_old, is_pipe_mode_new,
                     old_profile, new_profile, initiator_info, special_fields));

    /** treat with the common field QOS_EGRESS_MODEL data */
    if (initiator_info->valid_elements_set & BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID)
    {
        /** set egress_qos_model with the new value */
        SHR_IF_ERR_EXIT(dnx_qos_egress_model_bcm_to_dbal(unit, &initiator_info->egress_qos_model, &encap_qos_model));
    }
    else
    {
        /** value was not replaced - get the old value */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, get_entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL,
                                                            INST_SINGLE, &encap_qos_model));
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, INST_SINGLE, encap_qos_model);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_init_ttl_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields,
    uint8 *is_qos_valid)
{
    uint32 value_uint32 = 0;
    uint32 ttl_mode;
    uint8 is_field_valid;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, entry_handle_id, DBAL_FIELD_TTL_MODE, &is_field_valid, &ttl_mode));
    if (is_field_valid == TRUE)
    {
        (*is_qos_valid) = TRUE;

        /** uniform is default, do nothing*/
        if (ttl_mode == DBAL_ENUM_FVAL_TTL_MODE_PIPE)
        {
            uint32 ttl_pipe_profile;
            int ref_count = 0;

            initiator_info->egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;

            /** Get TTL PIPE profile */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_TTL_PIPE_PROFILE,
                                                                INST_SINGLE, &ttl_pipe_profile));
            /** get TTL value from template manager */
            SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_ttl_pipe_profile.profile_data_get
                            (unit, ttl_pipe_profile, &ref_count, (void *) &value_uint32));
            SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, FLOW_S_F_QOS_TTL, value_uint32);
        }
        else
        {
            initiator_info->egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_init_qos_app_params_get(
    int unit,
    dnx_flow_app_config_t * flow_app_info,
    uint32 entry_handle_id,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields,
    uint8 *is_qos_valid)
{
    uint8 is_field_valid;
    uint8 idx;
    dbal_enum_value_field_encap_qos_model_e encap_qos_model;
    bcm_qos_egress_model_t model;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, &is_field_valid, &encap_qos_model));
    if (is_field_valid == TRUE)
    {
        (*is_qos_valid) = TRUE;

        SHR_IF_ERR_EXIT(dnx_qos_egress_model_dbal_to_bcm(unit, encap_qos_model, &model));
        initiator_info->egress_qos_model.egress_qos = model.egress_qos;
        initiator_info->egress_qos_model.egress_ecn = model.egress_ecn;

        if ((encap_qos_model == DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_NEXT_SPACE) ||
            (encap_qos_model == DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_MY_SPACE) ||
            (encap_qos_model == DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_ECN))
        {
            uint32 qos_model_idx;
            uint8 qos_profile_get_dummy_var;
            uint8 nwk_idx;
            uint8 pri_cfi_found = 0;

            /** Get QOS pipe index */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_NWK_QOS_IDX,
                                                                INST_SINGLE, &qos_model_idx));

            SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_attr_get
                            (unit, qos_model_idx, &nwk_idx, &qos_profile_get_dummy_var, &qos_profile_get_dummy_var));

            for (idx = 0; flow_app_info->special_fields[idx] != FLOW_S_F_EMPTY; idx++)
            {
                if (flow_app_info->special_fields[idx] == FLOW_S_F_QOS_DSCP)
                {
                    SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, FLOW_S_F_QOS_DSCP, nwk_idx);
                    break;
                }
                else if (flow_app_info->special_fields[idx] == FLOW_S_F_QOS_EXP)
                {
                    SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, FLOW_S_F_QOS_EXP, nwk_idx);
                    break;
                }
                else if (flow_app_info->special_fields[idx] == FLOW_S_F_QOS_PRI)
                {
                    SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, FLOW_S_F_QOS_PRI,
                                                  DNX_QOS_IDX_NWK_QOS_PCP_GET(nwk_idx));
                    pri_cfi_found++;
                    if (pri_cfi_found > 1)
                    {
                        break;
                    }
                }
                else if (flow_app_info->special_fields[idx] == FLOW_S_F_QOS_CFI)
                {
                    SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, FLOW_S_F_QOS_CFI,
                                                  DNX_QOS_IDX_NWK_QOS_CFI_GET(nwk_idx));
                    pri_cfi_found++;
                    if (pri_cfi_found > 1)
                    {
                        break;
                    }
                }
            }
            if (flow_app_info->special_fields[idx] == FLOW_S_F_EMPTY)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "No network qos field supported for app");
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_init_dedicated_logic_qos_get(
    int unit,
    dnx_flow_app_config_t * flow_app_info,
    uint32 entry_handle_id,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields)
{
    uint8 is_qos_valid = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_flow_init_ttl_get(unit, entry_handle_id, initiator_info, special_fields, &is_qos_valid));

    SHR_IF_ERR_EXIT(dnx_flow_init_qos_app_params_get
                    (unit, flow_app_info, entry_handle_id, initiator_info, special_fields, &is_qos_valid));

    if (is_qos_valid == TRUE)
    {
        initiator_info->valid_elements_set |= BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_init_ttl_destroy(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    uint32 entry_handle_id,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources)
{
    uint32 profile, ttl_mode;
    uint8 is_last_profile_ref = FALSE, is_set = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * A delete CB should be run only in case the field was set at create (resources were allocated).
     * Check only the necessary fields.
     */
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_field_is_set_in_hw
                    (unit, flow_handle_info, gport_hw_resources, DBAL_FIELD_TTL_MODE, &is_set));
    if (is_set)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_TTL_MODE, INST_SINGLE, &ttl_mode));

        if (ttl_mode == DBAL_ENUM_FVAL_TTL_MODE_PIPE)
        {
            /** Get TTL PIPE profile */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_TTL_PIPE_PROFILE, INST_SINGLE, &profile));

            /** Free old TTL profile template */
            SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_ttl_pipe_profile.free_single(unit, profile, &is_last_profile_ref));
        }
        if (is_last_profile_ref)
        {
            SHR_IF_ERR_EXIT(dnx_qos_egress_ttl_profile_hw_clear(unit, profile));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_init_encap_qos_destroy(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    uint32 entry_handle_id,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources)
{
    dbal_enum_value_field_encap_qos_model_e encap_qos_model;
    bcm_qos_egress_model_t model;
    uint32 profile;
    uint8 is_last_profile_ref = FALSE, is_set = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_field_is_set_in_hw
                    (unit, flow_handle_info, gport_hw_resources, DBAL_FIELD_ENCAP_QOS_MODEL, &is_set));
    if (is_set)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, INST_SINGLE, &encap_qos_model));
        SHR_IF_ERR_EXIT(dnx_qos_egress_model_dbal_to_bcm(unit, encap_qos_model, &model));
        if ((encap_qos_model == DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_NEXT_SPACE) ||
            (encap_qos_model == DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_MY_SPACE) ||
            (encap_qos_model == DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_ECN))
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_NWK_QOS_IDX, INST_SINGLE, &profile));

            /** Free old QOS profile template */
            SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_free(unit, (int) profile, &is_last_profile_ref));
            if (is_last_profile_ref)
            {
                SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_hw_clear(unit, profile));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_init_dedicated_logic_qos_destroy(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    uint32 entry_handle_id,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources)
{
    SHR_FUNC_INIT_VARS(unit);

    /** destroy ttl profile if relevant */
    SHR_IF_ERR_EXIT(dnx_flow_init_ttl_destroy(unit, flow_handle_info, entry_handle_id, gport_hw_resources));

    /** destroy encap qos profile if relevant */
    SHR_IF_ERR_EXIT(dnx_flow_init_encap_qos_destroy(unit, flow_handle_info, entry_handle_id, gport_hw_resources));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_init_dedicated_logic_qos_replace(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    uint32 entry_handle_id,
    uint32 get_entry_handle_id,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields)
{
    uint8 is_set;
    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(initiator_info->valid_elements_set, BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID))
    {
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_field_is_set_in_hw
                        (unit, flow_handle_info, gport_hw_resources, DBAL_FIELD_ENCAP_QOS_MODEL, &is_set));

        if (is_set)
        {
            /** update the QoS */
            SHR_IF_ERR_EXIT(dnx_flow_init_qos_replace(unit, entry_handle_id, get_entry_handle_id,
                                                      initiator_info, special_fields));
        }
        else
        {
            /** in case egress_qos_model was not set previous - need to do a create */
            SHR_IF_ERR_EXIT(dnx_flow_init_dedicated_logic_qos_create(unit, entry_handle_id,
                                                                     initiator_info, special_fields));
        }
    }
    else if (_SHR_IS_FLAG_SET(initiator_info->valid_elements_clear, BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID))
    {
        /** need to delete the QoS */
        SHR_IF_ERR_EXIT(dnx_flow_init_dedicated_logic_qos_destroy
                        (unit, flow_handle_info, get_entry_handle_id, gport_hw_resources));
    }
    else
    {
        /** no changes, so if previously set, reset it */
        SHR_IF_ERR_EXIT(dnx_flow_init_dedicated_logic_qos_get
                        (unit, flow_app_info, get_entry_handle_id, initiator_info, special_fields));
        if (_SHR_IS_FLAG_SET(initiator_info->valid_elements_set, BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID))
        {
            SHR_IF_ERR_EXIT(dnx_flow_init_dedicated_logic_qos_create
                            (unit, entry_handle_id, initiator_info, special_fields));
        }

    }

exit:
    SHR_FUNC_EXIT;
}
