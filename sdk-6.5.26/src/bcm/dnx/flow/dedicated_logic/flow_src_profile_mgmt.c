#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <bcm_int/dnx/flow/flow.h>
#include <bcm_int/dnx/flow/flow_lif_mgmt.h>
#include <bcm_int/dnx/algo/l3/source_address_table_allocation.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_l3_access.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <bcm_int/dnx/l3/l3_arp.h>

static shr_error_e
dnx_flow_src_addr_profile_field_get(
    int unit,
    dnx_flow_app_config_t * flow_app_info,
    const flow_special_field_info_t ** curr_field,
    flow_special_fields_e * special_field_id)
{
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    (*special_field_id) = FLOW_S_F_EMPTY;

    for (ii = 0; flow_app_info->special_fields[ii] != FLOW_S_F_EMPTY; ii++)
    {
        SHR_IF_ERR_EXIT(flow_special_field_info_get(unit, flow_app_info->special_fields[ii], curr_field));
        if (dnx_flow_special_field_is_ind_set(unit, (*curr_field), FLOW_SPECIAL_FIELD_IND_SRC_ADDR_PROFILE_SUPPORTED))
        {
            (*special_field_id) = flow_app_info->special_fields[ii];
            break;
        }
    }

    if ((*special_field_id) == FLOW_S_F_EMPTY)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "src profile - no related field found ");

    }

exit:
    SHR_FUNC_EXIT;
}

/** function use the user input as profile and returns the profile index. in case first time profile is used
 *  configures the HW with the profile */
static shr_error_e
dnx_flow_src_addr_profile_create(
    int unit,
    bcm_flow_field_id_t field_id,
    bcm_ip_t ipv4_sip,
    uint8 ipv6_sip_or_src_mc[16],
    int *profile_id)
{
    uint8 is_first_sip_ref;
    uint8 is_last_sip_ref;      /* Not used */
    source_address_entry_t source_address_entry;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Initialize variables */
    sal_memset(&source_address_entry, 0, sizeof(source_address_entry));

    if (field_id == DBAL_FIELD_IPV4_SIP)
    {
        source_address_entry.address_type = source_address_type_ipv4;
        sal_memcpy(&source_address_entry.address.ipv4_address, &ipv4_sip, sizeof(bcm_ip_t));
    }
    else if (field_id == DBAL_FIELD_IPV6_SIP)
    {
        source_address_entry.address_type = source_address_type_ipv6;
        sal_memcpy(source_address_entry.address.ipv6_address, ipv6_sip_or_src_mc, sizeof(bcm_ip6_t));
    }
    else if (field_id == DBAL_FIELD_SOURCE_ADDRESS)
    {
        source_address_entry.address_type = source_address_type_full_mac;
        sal_memcpy(source_address_entry.address.mac_address, ipv6_sip_or_src_mc, sizeof(bcm_mac_t));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " profile - unsupported field %s", dnx_flow_special_field_to_string(unit, field_id));
    }

    SHR_IF_ERR_EXIT(algo_l3_db.source_address_table_allocation.source_address_table.exchange
                    (unit, 0, &source_address_entry, SOURCE_ADDRESS_TABLE_DEFAULT_PROFILE, NULL, profile_id,
                     &is_first_sip_ref, &is_last_sip_ref));

    /** write to HW if new SIP index was allocated */
    if (is_first_sip_ref)
    {
        uint32 profile_handle_id = (uint32) (-1);

        if (field_id == DBAL_FIELD_IPV4_SIP)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TUNNEL_SOURCE_ADDRESS_IPV4, &profile_handle_id));
            /** Set data field */
            dbal_entry_value_field32_set(unit, profile_handle_id, DBAL_FIELD_IPV4_SIP, INST_SINGLE, ipv4_sip);
            /** Set key field*/
            dbal_entry_key_field32_set(unit, profile_handle_id, DBAL_FIELD_SA_SIP_PROFILE, *profile_id);
        }
        else if (field_id == DBAL_FIELD_IPV6_SIP)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TUNNEL_SOURCE_ADDRESS_IPV6, &profile_handle_id));
            /** Set data field */
            dbal_entry_value_field_arr8_set(unit, profile_handle_id, DBAL_FIELD_IPV6_SIP, INST_SINGLE,
                                            ipv6_sip_or_src_mc);
            /** Set key field*/
            dbal_entry_key_field32_set(unit, profile_handle_id, DBAL_FIELD_SA_SIP_PROFILE, *profile_id);
        }
        else if (field_id == DBAL_FIELD_SOURCE_ADDRESS)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MAC_SOURCE_ADDRESS_FULL, &profile_handle_id));
            /** Set data field */
            dbal_entry_value_field_arr8_set(unit, profile_handle_id, DBAL_FIELD_SOURCE_ADDRESS, INST_SINGLE,
                                            ipv6_sip_or_src_mc);
            /** Set key field*/
            dbal_entry_key_field32_set(unit, profile_handle_id, DBAL_FIELD_SOURCE_ADDRESS_INDEX,
                                       L3_SOURCE_MAC_ETH_PROFILE(*profile_id));
        }

        /** Write to HW */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, profile_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_src_addr_profile_hw_get(
    int unit,
    bcm_flow_field_id_t field_id,
    uint32 profile_id,
    bcm_flow_special_field_t * special_field)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (field_id == DBAL_FIELD_IPV4_SIP)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TUNNEL_SOURCE_ADDRESS_IPV4, &entry_handle_id));
        /** Set key field*/
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SA_SIP_PROFILE, profile_id);

        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IPV4_SIP, INST_SINGLE, &(special_field->shr_var_uint32)));
    }
    else if (field_id == DBAL_FIELD_IPV6_SIP)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TUNNEL_SOURCE_ADDRESS_IPV6, &entry_handle_id));
        /** Set key field*/
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SA_SIP_PROFILE, profile_id);

        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                        (unit, entry_handle_id, DBAL_FIELD_IPV6_SIP, INST_SINGLE, special_field->shr_var_uint8_arr));
    }
    else if (field_id == DBAL_FIELD_SOURCE_ADDRESS)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MAC_SOURCE_ADDRESS_FULL, &entry_handle_id));
        /** Set key field*/
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_ADDRESS_INDEX,
                                   L3_SOURCE_MAC_ETH_PROFILE(profile_id));

        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                        (unit, entry_handle_id, DBAL_FIELD_SOURCE_ADDRESS, INST_SINGLE,
                         special_field->shr_var_uint8_arr));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "profile - unsupported field %s ", dnx_flow_special_field_to_string(unit, field_id));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_src_addr_profile_replace_internal(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_config_t * flow_app_info,
    const flow_special_field_info_t * curr_field,
    bcm_flow_special_field_t * special_field,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    int old_profile,
    uint8 *is_last_profile_ref)
{
    uint8 is_first_profile_ref;
    source_address_entry_t source_address_entry;
    int new_profile;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_SRC_ADDR_PROFILE_LSB))
    {
        /** Source address type full mac has special masking */
        SOURCE_ADDRESS_FULL_UNMASK(old_profile);
    }

    /** Initialize variables */
    is_first_profile_ref = 0;
    (*is_last_profile_ref) = 0;
    new_profile = old_profile;
    sal_memset(&source_address_entry, 0, sizeof(source_address_entry));

    if (curr_field->mapped_dbal_field == DBAL_FIELD_IPV4_SIP)
    {
        source_address_entry.address_type = source_address_type_ipv4;
        source_address_entry.address.ipv4_address = special_field->shr_var_uint32;
    }
    else if (curr_field->mapped_dbal_field == DBAL_FIELD_IPV6_SIP)
    {
        source_address_entry.address_type = source_address_type_ipv6;
        sal_memcpy(source_address_entry.address.ipv6_address, special_field->shr_var_uint8_arr, sizeof(bcm_ip6_t));
    }
    else if (curr_field->mapped_dbal_field == DBAL_FIELD_SOURCE_ADDRESS)
    {
        source_address_entry.address_type = source_address_type_full_mac;
        sal_memcpy(source_address_entry.address.mac_address, special_field->shr_var_uint8_arr, sizeof(bcm_mac_t));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "profile - unsupported field %s ",
                     dnx_flow_special_field_to_string(unit, curr_field->mapped_dbal_field));
    }

    /** Exchange SIP template*/
    SHR_IF_ERR_EXIT(algo_l3_db.source_address_table_allocation.source_address_table.exchange
                    (unit, 0, &source_address_entry, old_profile, NULL,
                     (int *) &new_profile, &is_first_profile_ref, is_last_profile_ref));

    /** write to HW if new SIP profile was allocated */
    if (is_first_profile_ref)
    {
        uint32 profile_handle_id = (uint32) (-1);;

        if (curr_field->mapped_dbal_field == DBAL_FIELD_IPV4_SIP)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TUNNEL_SOURCE_ADDRESS_IPV4, &profile_handle_id));
            /** Set data field */
            dbal_entry_value_field32_set(unit, profile_handle_id, curr_field->mapped_dbal_field, INST_SINGLE,
                                         special_field->shr_var_uint32);

            /** Set key field*/
            dbal_entry_key_field32_set(unit, profile_handle_id, DBAL_FIELD_SA_SIP_PROFILE, new_profile);
        }
        else if (curr_field->mapped_dbal_field == DBAL_FIELD_IPV6_SIP)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TUNNEL_SOURCE_ADDRESS_IPV6, &profile_handle_id));
            /** Set data field */
            dbal_entry_value_field_arr8_set(unit, profile_handle_id, DBAL_FIELD_IPV6_SIP, INST_SINGLE,
                                            special_field->shr_var_uint8_arr);
            /** Set key field*/
            dbal_entry_key_field32_set(unit, profile_handle_id, DBAL_FIELD_SA_SIP_PROFILE, new_profile);
        }
        else if (curr_field->mapped_dbal_field == DBAL_FIELD_SOURCE_ADDRESS)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MAC_SOURCE_ADDRESS_FULL, &profile_handle_id));
            /** Set data field */
            dbal_entry_value_field_arr8_set(unit, profile_handle_id, DBAL_FIELD_SOURCE_ADDRESS, INST_SINGLE,
                                            special_field->shr_var_uint8_arr);
            /** Set key field*/
            dbal_entry_key_field32_set(unit, profile_handle_id, DBAL_FIELD_SOURCE_ADDRESS_INDEX,
                                       L3_SOURCE_MAC_ETH_PROFILE(new_profile));
        }

        /** Write to HW */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, profile_handle_id, DBAL_COMMIT));
    }

    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_SRC_ADDR_PROFILE_LSB))
    {
        SOURCE_ADDRESS_FULL_MASK(new_profile);
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_IDX, INST_SINGLE, new_profile);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_dl_egress_src_addr_profile_create(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_special_fields_t * special_fields)
{
    const flow_special_field_info_t *curr_field;
    int profile, field_idx;

    SHR_FUNC_INIT_VARS(unit);

    for (field_idx = 0; field_idx < special_fields->actual_nof_special_fields; field_idx++)
    {
        SHR_IF_ERR_EXIT(flow_special_field_info_get
                        (unit, special_fields->special_fields[field_idx].field_id, &curr_field));

        if (dnx_flow_special_field_is_ind_set(unit, curr_field, FLOW_SPECIAL_FIELD_IND_SRC_ADDR_PROFILE_SUPPORTED))
        {
            break;
        }
    }

    if (field_idx == special_fields->actual_nof_special_fields)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_flow_src_addr_profile_create
                    (unit, curr_field->mapped_dbal_field,
                     special_fields->special_fields[field_idx].shr_var_uint32,
                     special_fields->special_fields[field_idx].shr_var_uint8_arr, &profile));

    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_SRC_ADDR_PROFILE_LSB))
    {
        /** Source address type full mac has special masking */
        SOURCE_ADDRESS_FULL_MASK(profile);
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_IDX, INST_SINGLE, profile);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_dl_egress_src_addr_profile_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_flow_special_fields_t * special_fields)
{
    const flow_special_field_info_t *curr_field;
    uint32 profile;
    flow_special_fields_e special_field_id = 0;
    bcm_flow_special_field_t *bcm_special_field =
        &(special_fields->special_fields[special_fields->actual_nof_special_fields]);
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_flow_src_addr_profile_field_get(unit, flow_app_info, &curr_field, &special_field_id));

    FLOW_FIELD_GET_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_SOURCE_IDX, INST_SINGLE, &profile));

    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_SRC_ADDR_PROFILE_LSB))
    {
        /** Source address type full mac has special masking */
        SOURCE_ADDRESS_FULL_UNMASK(profile);
    }

    rv = dnx_flow_src_addr_profile_hw_get(unit, curr_field->mapped_dbal_field, profile, bcm_special_field);

    if (rv == _SHR_E_NONE)
    {
        bcm_special_field->field_id = special_field_id;
        special_fields->actual_nof_special_fields++;

    }
    else
    {
        if (rv != _SHR_E_NOT_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "issue when getting field %s ", curr_field->name);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_src_addr_profile_hw_clear_internal(
    int unit,
    bcm_flow_field_id_t field_id,
    uint32 profile_id)
{
    uint32 entry_handle_id;
    uint8 remove_profile;
    uint8 first_ref;            /* Not used */
    int new_profile;            /* Not used */
    source_address_entry_t source_address_entry;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&source_address_entry, 0, sizeof(source_address_entry));

    /** Exchange the old SIP profile with the default one from sw algo template.*/
    SHR_IF_ERR_EXIT(algo_l3_db.source_address_table_allocation.source_address_table.exchange
                    (unit, 0, &source_address_entry, profile_id, NULL, &new_profile, &first_ref, &remove_profile));

    if (remove_profile)
    {
        if (field_id == DBAL_FIELD_IPV4_SIP)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TUNNEL_SOURCE_ADDRESS_IPV4, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SA_SIP_PROFILE, profile_id);
        }
        else if (field_id == DBAL_FIELD_IPV6_SIP)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TUNNEL_SOURCE_ADDRESS_IPV6, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SA_SIP_PROFILE, profile_id);
        }
        else if (field_id == DBAL_FIELD_SOURCE_ADDRESS)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MAC_SOURCE_ADDRESS_FULL, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_ADDRESS_INDEX,
                                       L3_SOURCE_MAC_ETH_PROFILE(profile_id));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "profile - unsupported field %s ",
                         dnx_flow_special_field_to_string(unit, field_id));
        }

        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_dl_egress_src_addr_profile_destroy(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources)
{
    uint32 src_addr_profile;
    const flow_special_field_info_t *curr_field;
    uint8 is_valid;
    flow_special_fields_e special_field_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_field_is_set_in_hw
                    (unit, flow_handle_info, gport_hw_resources, DBAL_FIELD_SOURCE_IDX, &is_valid));
    if (!is_valid)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_flow_src_addr_profile_field_get(unit, flow_app_info, &curr_field, &special_field_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SOURCE_IDX, INST_SINGLE, &src_addr_profile));

    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_SRC_ADDR_PROFILE_LSB))
    {
        /** Source address type full mac has special masking */
        SOURCE_ADDRESS_FULL_UNMASK(src_addr_profile);
    }

    SHR_IF_ERR_EXIT(dnx_flow_src_addr_profile_hw_clear_internal(unit, curr_field->mapped_dbal_field, src_addr_profile));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_dl_egress_src_addr_profile_hw_clear(
    int unit,
    dnx_flow_app_config_t * flow_app_info,
    uint32 profile_id)
{
    const flow_special_field_info_t *curr_field;
    flow_special_fields_e special_field_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_flow_src_addr_profile_field_get(unit, flow_app_info, &curr_field, &special_field_id));
    SHR_IF_ERR_EXIT(dnx_flow_src_addr_profile_hw_clear_internal(unit, special_field_id, profile_id));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_dl_egress_src_addr_profile_replace(
    int unit,
    uint32 entry_handle_id,
    uint32 get_entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_flow_special_fields_t * special_fields,
    int *last_profile)
{
    uint32 old_profile = SOURCE_ADDRESS_TABLE_DEFAULT_PROFILE;
    const flow_special_field_info_t *curr_field;
    flow_special_fields_e special_field_id;
    shr_error_e rv;
    uint8 is_last_profile_ref;
    int field_idx;

    SHR_FUNC_INIT_VARS(unit);

    (*last_profile) = FLOW_PROFILE_INVALID;

    SHR_IF_ERR_EXIT(dnx_flow_src_addr_profile_field_get(unit, flow_app_info, &curr_field, &special_field_id));

    for (field_idx = 0; field_idx < special_fields->actual_nof_special_fields; field_idx++)
    {
        if (special_field_id == special_fields->special_fields[field_idx].field_id)
        {
            break;
        }
    }

    rv = dbal_entry_handle_value_field32_get
        (unit, get_entry_handle_id, DBAL_FIELD_SOURCE_IDX, INST_SINGLE, &old_profile);

    /** User did not set the field. We should either copy the old value or just exist if it was not set before.   */
    if (field_idx == special_fields->actual_nof_special_fields)
    {
        if (rv == _SHR_E_NONE)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_IDX, INST_SINGLE, old_profile);
        }
        SHR_EXIT();
    }

    if (special_fields->special_fields[field_idx].is_clear)
    {
        uint8 is_valid;

        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_field_is_set_in_hw
                        (unit, flow_handle_info, gport_hw_resources, DBAL_FIELD_SOURCE_IDX, &is_valid));
        if (is_valid)
        {
            SHR_IF_ERR_EXIT(dnx_flow_dl_egress_src_addr_profile_destroy
                            (unit, get_entry_handle_id, flow_handle_info, flow_app_info, gport_hw_resources));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_flow_src_addr_profile_replace_internal
                        (unit, entry_handle_id, flow_app_info, curr_field, &(special_fields->special_fields[field_idx]),
                         gport_hw_resources, old_profile, &is_last_profile_ref));

        if (is_last_profile_ref)
        {
            (*last_profile) = old_profile;
        }
    }

exit:
    SHR_FUNC_EXIT;
}
