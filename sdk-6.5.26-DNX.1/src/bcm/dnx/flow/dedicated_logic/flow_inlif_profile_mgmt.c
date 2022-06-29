#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <bcm_int/dnx/flow/flow.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_qos_access.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/lif/cs_in_lif_profile_based_on_stages.h>
#include <src/bcm/dnx/tunnel/tunnel_term_srv6_utils.h>
#include <src/bcm/dnx/flow/app_defs/flow_srv6_ipv6_tunnel_terminator.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_flow.h>

#define DNX_FLOW_INLIF_PROFILE_UPDATED(terminator_info, special_fields) \
    ((terminator_info->valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID) || \
     (terminator_info->valid_elements_clear & BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID) || \
     (terminator_info->valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID) || \
     (terminator_info->valid_elements_clear & BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID) || \
     (special_fields ? special_fields->actual_nof_special_fields : FALSE))

/****************************
 * LEGACY PROFILE FUNCTIONS *
 ****************************/
static shr_error_e
dnx_flow_service_type_update(
    int unit,
    uint32 set_entry_handle_id,
    uint32 get_entry_handle_id,
    bcm_flow_terminator_info_t * terminator_info)
{
    dbal_enum_value_field_vtt_lif_service_type_e service_type = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** user has a request to update the service_type */
    if ((terminator_info->valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID) ||
        (terminator_info->valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID))
    {
        service_type = (terminator_info->valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID) ?
            terminator_info->l2_ingress_info.service_type : terminator_info->l3_ingress_info.service_type;
        if (service_type == bcmFlowServiceTypeCrossConnect)
        {
            service_type = DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_P2P;
        }
        else
        {
            service_type = DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_P2MP;
        }

        dbal_entry_value_field32_set(unit, set_entry_handle_id, DBAL_FIELD_SERVICE_TYPE, INST_SINGLE, service_type);
    }
    /*
     * the next if can only happen for replace (in if one of the valid_sets must be set to get to here)
     * in case the field was cleared, no need to do anything
     * but if no clear and no set, this must be replace and need to update the previous status
     */
    else if ((!(terminator_info->valid_elements_clear & BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID)) &&
             (!(terminator_info->valid_elements_clear & BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID)))
    {
        shr_error_e rv = dbal_entry_handle_value_field32_get(unit, get_entry_handle_id, DBAL_FIELD_SERVICE_TYPE,
                                                             INST_SINGLE, &service_type);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        /** if field previously set, re-set it */
        if (rv == _SHR_E_NONE)
        {
            dbal_entry_value_field32_set(unit, set_entry_handle_id, DBAL_FIELD_SERVICE_TYPE, INST_SINGLE, service_type);
        }

    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_service_type_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_service_type_t * service_type)
{
    dbal_enum_value_field_vtt_lif_service_type_e vtt_lif_service_type = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_SERVICE_TYPE,
                                                        INST_SINGLE, &vtt_lif_service_type));

    if (vtt_lif_service_type == DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_P2P)
    {
        (*service_type) = bcmFlowServiceTypeCrossConnect;
    }
    else
    {
        (*service_type) = bcmFlowServiceTypeMultiPoint;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_inlif_legacy_profile_fields_update(
    int unit,
    uint32 set_entry_handle_id,
    uint32 get_entry_handle_id,
    bcm_flow_terminator_info_t * terminator_info,
    in_lif_profile_info_t * in_lif_profile_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Set/Reset the l2 related common fields if required. */
    if (terminator_info->valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID)
    {
        in_lif_profile_info->egress_fields.in_lif_orientation =
            terminator_info->l2_ingress_info.ingress_network_group_id;
    }
    else if (terminator_info->valid_elements_clear & BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID)
    {
        in_lif_profile_info->egress_fields.in_lif_orientation = DEFAULT_IN_LIF_ORIENTATION;
    }

    /** Set/Reset the l3 related common fields if required. */
    if (terminator_info->valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID)
    {
        in_lif_profile_info->ingress_fields.urpf_mode = terminator_info->l3_ingress_info.urpf_mode;
    }
    else if (terminator_info->valid_elements_clear & BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID)
    {
        in_lif_profile_info->ingress_fields.urpf_mode = bcmL3IngressUrpfDisable;
    }

    /** update the service type */
    SHR_IF_ERR_EXIT(dnx_flow_service_type_update(unit, set_entry_handle_id, get_entry_handle_id, terminator_info));

exit:
    SHR_FUNC_EXIT;
}

static void
dnx_flow_inlif_legacy_profile_fields_get(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_terminator_info_t * terminator_info,
    in_lif_profile_info_t * in_lif_profile_info)
{
    bcm_flow_service_type_t service_type = bcmFlowServiceTypeMultiPoint;

    /** service_type */
    dnx_flow_service_type_get(unit, entry_handle_id, &service_type);

    /** l2_ingress_info */
    if (flow_app_info->valid_common_fields_bitmap & BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID)
    {
        /** if app supports l2, and either not default value or app does not support l3 */
        if ((in_lif_profile_info->egress_fields.in_lif_orientation != DEFAULT_IN_LIF_ORIENTATION) ||
            (!(flow_app_info->valid_common_fields_bitmap & BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID)))
        {
            terminator_info->l2_ingress_info.ingress_network_group_id =
                in_lif_profile_info->egress_fields.in_lif_orientation;
            terminator_info->valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID;
        }
        terminator_info->l2_ingress_info.service_type = service_type;
    }
    /** l3_ingress_info */
    if (flow_app_info->valid_common_fields_bitmap & BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID)
    {
        /** if app supports l3, and either not default value or app does not support l3 */
        if ((in_lif_profile_info->ingress_fields.urpf_mode != bcmL3IngressUrpfDisable) ||
            (!(flow_app_info->valid_common_fields_bitmap & BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID)))
        {
            terminator_info->l3_ingress_info.urpf_mode = in_lif_profile_info->ingress_fields.urpf_mode;
            terminator_info->valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID;
        }
        terminator_info->l3_ingress_info.service_type = service_type;
    }
}

static shr_error_e
dnx_flow_inlif_profile_destroy(
    int unit,
    dbal_tables_e inlif_dbal_table_id,
    uint32 in_lif_profile,
    in_lif_profile_info_t * in_lif_profile_info)
{
    int new_in_lif_profile;

    SHR_FUNC_INIT_VARS(unit);

    /** get the previously set profile */
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, in_lif_profile, in_lif_profile_info, inlif_dbal_table_id, LIF));

    /** Deallocate in_lif_profile */
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_dealloc(unit, in_lif_profile, &new_in_lif_profile, in_lif_profile_info, LIF));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_inlif_legacy_profile_fields_replace(
    int unit,
    uint32 set_entry_handle_id,
    uint32 get_entry_handle_id,
    dbal_tables_e inlif_dbal_table_id,
    bcm_flow_terminator_info_t * terminator_info,
    in_lif_profile_info_t * in_lif_profile_info,
    uint8 *should_update_dbal)
{
    SHR_FUNC_INIT_VARS(unit);

    /** in case during replace, only deleting the set field and not adding a new one */
    if (((terminator_info->valid_elements_clear & BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID) &&
         (!(terminator_info->valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID))) ||
        ((terminator_info->valid_elements_clear & BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID) &&
         (!(terminator_info->valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID))))
    {
        uint32 in_lif_profile;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, get_entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE,
                                                            INST_SINGLE, &in_lif_profile));

        SHR_IF_ERR_EXIT(dnx_flow_inlif_profile_destroy(unit, inlif_dbal_table_id, in_lif_profile, in_lif_profile_info));

        (*should_update_dbal) = FALSE;
        SHR_EXIT();
    }

    (*should_update_dbal) = TRUE;

    /** it is invalid to set l2_ingress_info when l3_ingress_info was previously set and not cleared now */
    if ((terminator_info->valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID) &&
        (in_lif_profile_info->ingress_fields.urpf_mode != bcmL3IngressUrpfDisable) &&
        (!(terminator_info->valid_elements_clear & BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "In replace, cannot set L2_INGRESS_INFO_VALID without clearing L3_INGRESS_INFO_VALID");
    }

    /** it is invalid to set l3_ingress_info when l2_ingress_info was previously set and not cleared now */
    if ((terminator_info->valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID) &&
        (in_lif_profile_info->egress_fields.in_lif_orientation != DEFAULT_IN_LIF_ORIENTATION) &&
        (!(terminator_info->valid_elements_clear & BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "In replace, cannot set L3_INGRESS_INFO_VALID without clearing L2_INGRESS_INFO_VALID");
    }

    SHR_IF_ERR_EXIT(dnx_flow_inlif_legacy_profile_fields_update(unit, set_entry_handle_id, get_entry_handle_id,
                                                                terminator_info, in_lif_profile_info));

exit:
    SHR_FUNC_EXIT;
}

/********************
 * SHARED FUNCTIONS *
 ********************/
shr_error_e
dnx_flow_dl_inlif_profile_create(
    int unit,
    dnx_flow_app_config_t * flow_app_info,
    uint32 entry_handle_id,
    bcm_gport_t lif_gport,
    bcm_flow_terminator_info_t * terminator_info,
    bcm_flow_special_fields_t * special_fields)
{
    dbal_tables_e dbal_table_id;
    dnx_in_lif_profile_last_info_t last_profile;
    in_lif_profile_info_t in_lif_profile_info;
    int old_in_lif_profile = DNX_IN_LIF_PROFILE_DEFAULT;
    int new_in_lif_profile = DNX_IN_LIF_PROFILE_DEFAULT;

    SHR_FUNC_INIT_VARS(unit);

    dnx_in_lif_profile_last_info_t_init(&last_profile);
    SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));

    SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));

    in_lif_profile_info.ingress_fields.lif_gport = lif_gport;

    /** Update the related fields with inputs. table is not relevant if not replace */
    {
        /** in legacy devices, if user didn't request - do not set a default device */
        if ((!(terminator_info->valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID)) &&
            (!(terminator_info->valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID)))
        {
            SHR_EXIT();
        }
        SHR_IF_ERR_EXIT(dnx_flow_inlif_legacy_profile_fields_update(unit, entry_handle_id, 0, terminator_info,
                                                                    &in_lif_profile_info));
    }

    /** update the profile */
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange(unit, &in_lif_profile_info, old_in_lif_profile,
                                                &new_in_lif_profile, LIF, dbal_table_id, &last_profile));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE,
                                 (uint32) new_in_lif_profile);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_dl_inlif_profile_get(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_config_t * flow_app_info,
    bcm_gport_t lif_gport,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_flow_terminator_info_t * terminator_info,
    bcm_flow_special_fields_t * special_fields)
{
    in_lif_profile_info_t in_lif_profile_info;
    in_lif_profile_info_t dft_in_lif_profile_info;
    uint32 in_lif_profile = 0;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    rv = dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE,
                                             INST_SINGLE, &in_lif_profile);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    if (rv == _SHR_E_NOT_FOUND)
    {
        SHR_EXIT();
    }

    /** get in_lif_profile data and store it in in_lif_profile_info */
    SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));
    in_lif_profile_info.ingress_fields.lif_gport = lif_gport;
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, in_lif_profile, &in_lif_profile_info,
                                                gport_hw_resources->inlif_dbal_table_id, LIF));

    /** get the default in_lif_profile data for checking the changes in the fields.*/
    SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &dft_in_lif_profile_info));
    dft_in_lif_profile_info.ingress_fields.is_default_in_lif = TRUE;
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, DNX_IN_LIF_PROFILE_DEFAULT, &dft_in_lif_profile_info,
                                                DBAL_TABLE_EMPTY, LIF));

    {
        dnx_flow_inlif_legacy_profile_fields_get(unit, entry_handle_id, flow_app_info, terminator_info,
                                                 &in_lif_profile_info);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_dl_inlif_profile_destroy(
    int unit,
    uint32 entry_handle_id,
    bcm_gport_t lif_gport,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources)
{
    uint32 in_lif_profile = 0;
    in_lif_profile_info_t in_lif_profile_info;
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));
    /** if the profile is not the default profile, then need to deallocate it */
    rv = dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE,
                                             INST_SINGLE, &in_lif_profile);

    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    if (rv == _SHR_E_NOT_FOUND)
    {
        SHR_EXIT();
    }

    if (in_lif_profile != DNX_IN_LIF_PROFILE_DEFAULT)
    {
        in_lif_profile_info.ingress_fields.lif_gport = lif_gport;

        SHR_IF_ERR_EXIT(dnx_flow_inlif_profile_destroy(unit, gport_hw_resources->inlif_dbal_table_id,
                                                       in_lif_profile, &in_lif_profile_info));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_dl_inlif_profile_replace(
    int unit,
    dnx_flow_app_config_t * flow_app_info,
    uint32 set_entry_handle_id,
    uint32 get_entry_handle_id,
    bcm_gport_t lif_gport,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_flow_terminator_info_t * terminator_info,
    bcm_flow_special_fields_t * special_fields,
    dnx_in_lif_profile_last_info_t * last_profile)
{
    shr_error_e rv;
    in_lif_profile_info_t in_lif_profile_info;
    int old_in_lif_profile = DNX_IN_LIF_PROFILE_DEFAULT;
    int new_in_lif_profile = DNX_IN_LIF_PROFILE_DEFAULT;
    uint8 should_update_dbal = TRUE;

    SHR_FUNC_INIT_VARS(unit);

    dnx_in_lif_profile_last_info_t_init(last_profile);
    SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));

    /** Get entry from inLIF table, */
    rv = dbal_entry_handle_value_field32_get(unit, get_entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE,
                                             INST_SINGLE, (uint32 *) &old_in_lif_profile);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);

    /** in case field wasn't previously set and no new set requirements - exit */
    if ((rv == _SHR_E_NOT_FOUND) && (!DNX_FLOW_INLIF_PROFILE_UPDATED(terminator_info, special_fields)))
    {
        SHR_EXIT();
    }
    in_lif_profile_info.ingress_fields.lif_gport = lif_gport;

    /** get the previously set profile */
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, old_in_lif_profile, &in_lif_profile_info,
                                                gport_hw_resources->inlif_dbal_table_id, LIF));

    {
        SHR_IF_ERR_EXIT(dnx_flow_inlif_legacy_profile_fields_replace(unit, set_entry_handle_id, get_entry_handle_id,
                                                                     gport_hw_resources->inlif_dbal_table_id,
                                                                     terminator_info, &in_lif_profile_info,
                                                                     &should_update_dbal));
    }

    if (should_update_dbal == TRUE)
    {
        /** Exchange for a new in_lif_profile */
        SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange(unit, &in_lif_profile_info, old_in_lif_profile, &new_in_lif_profile,
                                                    LIF, gport_hw_resources->inlif_dbal_table_id, last_profile));

        dbal_entry_value_field32_set(unit, set_entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE,
                                     (uint32) new_in_lif_profile);
    }
exit:
    SHR_FUNC_EXIT;
}
