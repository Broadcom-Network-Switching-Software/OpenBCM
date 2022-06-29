#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <bcm_int/dnx/flow/flow.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_qos_access.h>
#include <bcm_int/dnx/lif/out_lif_profile.h>

/************************ OUTLIF_PROFILE APIs ************************/

shr_error_e
dnx_flow_dl_outlif_profile_create(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields)
{
    etpp_out_lif_profile_info_t out_lif_profile_info;
    int new_out_lif_profile;
    uint8 is_last = 0;
    dbal_tables_e dbal_table_id;
    shr_error_e rv = _SHR_E_NOT_FOUND;
    bcm_flow_special_field_t qos_pcp_special_field_data = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    etpp_out_lif_profile_info_t_init(unit, &out_lif_profile_info);

    rv = dnx_flow_special_field_data_get(unit, special_fields, FLOW_S_F_QOS_PCP, &qos_pcp_special_field_data);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    if ((rv != _SHR_E_NOT_FOUND) && (qos_pcp_special_field_data.shr_var_uint32 == 0)
        && (qos_pcp_special_field_data.is_clear == 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "PCP profile field cannot be set to the default value. Please use clear function.\r\n");
    }

    if (!_SHR_IS_FLAG_SET(initiator_info->valid_elements_set, BCM_FLOW_INITIATOR_ELEMENT_L2_EGRESS_INFO_VALID) &&
        (rv == _SHR_E_NOT_FOUND))
    {
        /** no related input from user */
        SHR_EXIT();
    }

    out_lif_profile_info.out_lif_orientation = initiator_info->l2_egress_info.egress_network_group_id;
    out_lif_profile_info.oam_lif_profile = qos_pcp_special_field_data.shr_var_uint32;

    SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));

    /** Allocate an Out-LIF profile  */
    SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_exchange(unit,
                                                      dbal_table_id, 0, &out_lif_profile_info,
                                                      DNX_OUT_LIF_PROFILE_DEFAULT, &new_out_lif_profile, &is_last));

    /** Set the allocated Out-LIF profile */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE, new_out_lif_profile);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_dl_outlif_profile_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_initiator_info_t * initiator_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_flow_special_fields_t * special_fields)
{
    shr_error_e rv;
    etpp_out_lif_profile_info_t out_lif_profile_info;
    uint32 out_lif_profile;

    SHR_FUNC_INIT_VARS(unit);

    rv = dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE,
                                             (uint32 *) &out_lif_profile);

    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);

    if (rv == _SHR_E_NOT_FOUND)
    {
        SHR_EXIT();
    }

    initiator_info->valid_elements_set |= BCM_FLOW_INITIATOR_ELEMENT_L2_EGRESS_INFO_VALID;

    /** Retrieve the data for the Out-LIF Profile */
    SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_get_data(unit, out_lif_profile, &out_lif_profile_info));
    initiator_info->l2_egress_info.egress_network_group_id = out_lif_profile_info.out_lif_orientation;

    /*
     * Get special field 
     */
    
    if (out_lif_profile_info.oam_lif_profile != 0)
    {
        SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, FLOW_S_F_QOS_PCP, out_lif_profile_info.oam_lif_profile);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_dl_outlif_profile_destroy_internal(
    int unit,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    int out_lif_profile,
    uint8 *is_last)
{
    etpp_out_lif_profile_info_t out_lif_profile_info;
    int new_out_lif_profile;

    SHR_FUNC_INIT_VARS(unit);

    /** Free the Out-LIF profile by calling the exchange function with the reset default profile data */
    etpp_out_lif_profile_info_t_init(unit, &out_lif_profile_info);

    SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_exchange
                    (unit, gport_hw_resources->outlif_dbal_table_id, 0, &out_lif_profile_info, out_lif_profile,
                     &new_out_lif_profile, is_last));

    /** in dbal table EEDB_IPV4_TUNNEL, OUT_LIF_PROFILE has an arr-prefix */
    if ((!sal_strncasecmp(flow_app_info->app_name, FLOW_APP_NAME_IPV4_INITIATOR, FLOW_STR_MAX)) ||
        (!sal_strncasecmp(flow_app_info->app_name, FLOW_APP_NAME_IPV6_INITIATOR, FLOW_STR_MAX)))
    {
        if (new_out_lif_profile != DNX_OUT_LIF_PROFILE_DEFAULT_ORIENTATION_PROFILE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error! Something is wrong! on freeing oulif_profile, didn't get default out lif profile, default outlif profile = %d, new_out_lif_profile = %d",
                         DNX_OUT_LIF_PROFILE_DEFAULT_ORIENTATION_PROFILE, new_out_lif_profile);
        }
    }
    else
    {
        if (new_out_lif_profile != DNX_OUT_LIF_PROFILE_DEFAULT)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error! Something is wrong! on freeing oulif_profile, didn't get default out lif profile, default outlif profile = %d, new_out_lif_profile = %d",
                         DNX_OUT_LIF_PROFILE_DEFAULT, new_out_lif_profile);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_dl_outlif_profile_destroy(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources)
{
    shr_error_e rv;
    uint8 is_last = FALSE;
    uint32 out_lif_profile;

    SHR_FUNC_INIT_VARS(unit);

    rv = dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE,
                                             &out_lif_profile);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);

    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_IF_ERR_EXIT(dnx_flow_dl_outlif_profile_destroy_internal
                        (unit, flow_app_info, gport_hw_resources, out_lif_profile, &is_last));

        if (is_last)
        {
            /** Clear the old ETPP outlif profile as there are no more references to it*/
            SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_hw_clear(unit, out_lif_profile));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_dl_outlif_profile_replace(
    int unit,
    uint32 entry_handle_id,
    uint32 get_entry_handle_id,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_initiator_info_t * initiator_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_flow_special_fields_t * special_fields,
    int *last_profile)
{
    uint8 is_last = 0;
    shr_error_e rv;
    uint32 old_lif_profile = 0;
    int new_lif_profile = 0;
    int is_field_clear = FALSE, is_field_set = FALSE;
    shr_error_e rv1;
    bcm_flow_special_field_t special_field_data = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    (*last_profile) = FLOW_PROFILE_INVALID;

    /** Get previous profile */
    rv = dbal_entry_handle_value_field32_get(unit, get_entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE,
                                             &old_lif_profile);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);

    if (_SHR_IS_FLAG_SET(initiator_info->valid_elements_set, BCM_FLOW_INITIATOR_ELEMENT_L2_EGRESS_INFO_VALID))
    {
        is_field_set = TRUE;
    }

    if (_SHR_IS_FLAG_SET(initiator_info->valid_elements_clear, BCM_FLOW_INITIATOR_ELEMENT_L2_EGRESS_INFO_VALID))
    {
        is_field_clear = TRUE;
    }

    rv1 = dnx_flow_special_field_data_get(unit, special_fields, FLOW_S_F_QOS_PCP, &special_field_data);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv1, _SHR_E_NOT_FOUND);
    if (rv1 != _SHR_E_NOT_FOUND)
    {
        if ((special_field_data.shr_var_uint32 == 0) && (special_field_data.is_clear == 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "PCP profile field cannot be set to the default value. Please use clear function.\r\n");
        }
        else
        {
            is_field_set = TRUE;
        }
    }

    /*
     * if at least one of the fields is set - create profile 
     */
    if ((rv == _SHR_E_NONE) || (rv1 == _SHR_E_NONE))
    {
        if (!is_field_set && !is_field_clear)
        {
            /** user didn't modify the value, just copy the existing profile to the new handle */
            dbal_entry_value_field32_set
                (unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE, old_lif_profile);
            SHR_EXIT();
        }
        else
        {
            /*
             * If all fields are cleared - destroy lif profile 
             */
            if (is_field_clear)
            {
                SHR_IF_ERR_EXIT(dnx_flow_dl_outlif_profile_destroy_internal
                                (unit, flow_app_info, gport_hw_resources, old_lif_profile, &is_last));
                if (is_last)
                {
                    (*last_profile) = old_lif_profile;
                }
            }
            else
            {
                dbal_tables_e dbal_table_id;
                etpp_out_lif_profile_info_t out_lif_profile_info;

                etpp_out_lif_profile_info_t_init(unit, &out_lif_profile_info);

                out_lif_profile_info.out_lif_orientation = initiator_info->l2_egress_info.egress_network_group_id;
                if (rv1 != _SHR_E_NOT_FOUND)
                {
                    out_lif_profile_info.oam_lif_profile = special_field_data.shr_var_uint32;
                }

                SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));

                /** Allocate an Out-LIF profile  */
                SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_exchange
                                (unit, dbal_table_id, 0, &out_lif_profile_info, old_lif_profile, &new_lif_profile,
                                 &is_last));

                /** Set the allocated Out-LIF profile */
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE,
                                             new_lif_profile);

                if (is_last)
                {
                    (*last_profile) = old_lif_profile;
                }
            }
        }
    }
    else
    {
        if (is_field_set)
        {
            SHR_IF_ERR_EXIT(dnx_flow_dl_outlif_profile_create
                            (unit, entry_handle_id, flow_app_info, initiator_info, special_fields));
        }
    }
exit:
    SHR_FUNC_EXIT;
}
