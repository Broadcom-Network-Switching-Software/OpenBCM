#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include "flow_ipvx_tunnel_terminator.h"
#include "../../flow_def.h"
#include "../../../tunnel/tunnel_term.h"
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/lif/in_lif_profile.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_tunnel_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tunnel.h>

extern dnx_flow_app_config_t *dnx_flow_objects[];


/** ipv4_p2p_tcam */

/** ipv4_p2p_em */
#define VALID_IPV4_P2P_EM_MATCH_SPECIAL_FIELDS {FLOW_S_F_VRF, FLOW_S_F_IPV4_DIP, FLOW_S_F_IPV4_SIP, FLOW_S_F_IPV4_TUNNEL_TYPE}

/** ipv4_mp_em */
#define VALID_IPV4_MP_EM_MATCH_SPECIAL_FIELDS {FLOW_S_F_VRF, FLOW_S_F_IPV4_DIP, FLOW_S_F_IPV4_TUNNEL_TYPE}

/** ipv6_mp_tcam */
#define VALID_LEGACY_IPV6_MP_TCAM_MATCH_SPECIAL_FIELDS {FLOW_S_F_VRF, FLOW_S_F_IPV6_DIP, FLOW_S_F_IPV6_DIP_MASK, FLOW_S_F_IPV6_TUNNEL_TYPE}

/** ipv6_p2p_em */
#define VALID_IPV6_P2P_EM_MATCH_SPECIAL_FIELDS {FLOW_S_F_IPV6_SIP, FLOW_S_F_TUNNEL_CLASS}

#define IPV4_EM_TERM_MATCH_INDICATIONS SAL_BIT(FLOW_APP_TERM_MATCH_IND_MULTIPLE_DBAL_TABLES) | SAL_BIT(FLOW_APP_TERM_MATCH_IND_BUD_BY_KEY_FIELD)
/*******************************
 * IPVX_TUNNEL_MATCH FUNCTIONS *
 *******************************/

static shr_error_e
flow_term_match_ipv6_p2p_em_key_field_ipv6_sip_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_cb_type_e flow_cb_type,
    bcm_flow_special_fields_t * special_fields)
{
    int ipv6_sip_profile;
    bcm_flow_special_field_t special_field_data;

    SHR_FUNC_INIT_VARS(unit);

    /** gets the ipv6_sip value */
    SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_special_field_data_get
                              (unit, special_fields, FLOW_S_F_IPV6_SIP, &special_field_data), _SHR_E_NOT_FOUND);

    if (flow_cb_type == FLOW_CB_TYPE_SET)
    {
        uint8 first_reference = 0;

        /** allocate profile for the msb bits of the ipv6_sip */
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_sip6_msbs_profile_allocate
                        (unit, (bcm_ip6_t *) special_field_data.shr_var_uint8_arr, &ipv6_sip_profile,
                         &first_reference));

        if (first_reference)
        {
            SHR_IF_ERR_EXIT(dnx_tunnel_terminator_ipv6_sip_compress_table_entry_add
                            (unit, special_field_data.shr_var_uint8_arr, ipv6_sip_profile));
        }
    }
    else        /* flow_cb_type == FLOW_CB_TYPE_GET */
    {
        /** getting the profile */
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_msb_sip6_profile_get
                        (unit, (bcm_ip6_t *) special_field_data.shr_var_uint8_arr, &ipv6_sip_profile));
    }

    /** Set the relevant key fields */
    /** Set IPv6 SIP MSBs profile */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_ADDRESS_MSBS_ID, ipv6_sip_profile);
    /** Set IPv6 SIP LSBs */
    dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_IPV6, special_field_data.shr_var_uint8_arr);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_term_match_ipv6_p2p_em_key_field_ipv6_sip_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_special_fields_t * special_fields)
{
    int field_idx;
    bcm_ip6_t sip6;

    SHR_FUNC_INIT_VARS(unit);

    /** get the sip */
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_traverse_ipv6_p2p_sip6_get(unit, entry_handle_id, &sip6));

    /** update the relevant special field with the sip's value */
    SHR_IF_ERR_EXIT(dnx_flow_special_field_find(unit, special_fields, FLOW_S_F_IPV6_SIP, &field_idx));
    sal_memcpy(special_fields->special_fields[field_idx].shr_var_uint8_arr, sip6,
               sizeof(special_fields->special_fields[field_idx].shr_var_uint8_arr));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_term_match_ipv6_p2p_em_key_field_ipv6_sip_destroy(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_special_fields_t * special_fields)
{
    int ipv6_sip_profile;
    uint8 last_reference = FALSE;
    bcm_flow_special_field_t special_field_data;

    SHR_FUNC_INIT_VARS(unit);

    /** gets the ipv6_sip value */
    SHR_IF_ERR_EXIT(dnx_flow_special_field_data_get(unit, special_fields, FLOW_S_F_IPV6_SIP, &special_field_data));

    /** getting and releasing the profile */
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_msb_sip6_profile_get(unit, (bcm_ip6_t *) special_field_data.shr_var_uint8_arr,
                                                               &ipv6_sip_profile));
    SHR_IF_ERR_EXIT(algo_tunnel_db.tunnel_ipv6_sip_profile.free_single(unit, ipv6_sip_profile, &last_reference));

    if (last_reference)
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_ipv6_sip_compress_table_entry_delete(unit,
                                                                                   special_field_data.shr_var_uint8_arr));
    }

    /** Set the relevant key fields */
    /** Set IPv6 SIP MSBs profile */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_ADDRESS_MSBS_ID, ipv6_sip_profile);
    /** Set IPv6 SIP LSBs */
    dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_IPV6, special_field_data.shr_var_uint8_arr);

exit:
    SHR_FUNC_EXIT;
}

/*******************************
 * IPVX_TUNNEL_MATCH CALLBACKS *
 *******************************/
int
flow_ipv6_tunnel_p2p_em_term_match_app_specific_operations_cb(
    int unit,
    uint32 current_entry_handle_id,
    uint32 prev_entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_cmd_control_info_t * flow_cmd_control,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    void *app_entry_data,
    bcm_flow_special_fields_t * special_fields,
    void *lif_info,
    uint32 *selectable_result_types)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (flow_cmd_control->flow_cb_type)
    {
        case FLOW_CB_TYPE_SET:
        case FLOW_CB_TYPE_GET:
            SHR_IF_ERR_EXIT(flow_term_match_ipv6_p2p_em_key_field_ipv6_sip_set(unit,
                                                                               current_entry_handle_id,
                                                                               flow_cmd_control->flow_cb_type,
                                                                               special_fields));
            break;
        case FLOW_CB_TYPE_TRAVERSE:
            SHR_IF_ERR_EXIT(flow_term_match_ipv6_p2p_em_key_field_ipv6_sip_get(unit, current_entry_handle_id,
                                                                               special_fields));
            break;
        case FLOW_CB_TYPE_DELETE:
            SHR_IF_ERR_EXIT(flow_term_match_ipv6_p2p_em_key_field_ipv6_sip_destroy(unit,
                                                                                   current_entry_handle_id,
                                                                                   special_fields));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid flow_cb_type (%d)", (uint32) flow_cmd_control->flow_cb_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}


int
flow_legacy_match_app_ipv4_tunnel_type_set(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_special_fields_t * special_fields)
{

    bcm_flow_special_field_t special_field_data;
    uint32 ipv4_layer_qualifier_tunnel_type;
    bcm_tunnel_terminator_t info = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    /** get the tunnel_type */
    SHR_IF_ERR_EXIT(dnx_flow_special_field_data_get
                    (unit, special_fields, FLOW_S_F_IPV4_TUNNEL_TYPE, &special_field_data));
    info.type = (bcm_tunnel_type_t) special_field_data.symbol;

    /** converting the bcm ipv4 tunnel type to dbal tunnel type */
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_type_to_ipv4_layer_qualifier_tunnel_type
                    (unit, &info, &ipv4_layer_qualifier_tunnel_type));
    /** setting the ipv4 tunnel type */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4_TUNNEL_TYPE, ipv4_layer_qualifier_tunnel_type);

exit:
    SHR_FUNC_EXIT;
}

int
flow_legacy_match_app_ipv4_tunnel_type_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_special_fields_t * special_fields)
{
    int field_idx;
    uint32 ipv4_layer_qualifier_tunnel_type, dummy;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                    (unit, entry_handle_id, DBAL_FIELD_IPV4_TUNNEL_TYPE, &ipv4_layer_qualifier_tunnel_type));
    SHR_IF_ERR_EXIT(dnx_flow_special_field_find(unit, special_fields, FLOW_S_F_IPV4_TUNNEL_TYPE, &field_idx));
    SHR_IF_ERR_EXIT(dnx_ipv4_layer_qualifier_tunnel_type_to_tunnel_terminator_type
                    (unit,
                     ipv4_layer_qualifier_tunnel_type,
                     (bcm_tunnel_type_t *) & (special_fields->special_fields[field_idx].symbol), &dummy));
exit:
    SHR_FUNC_EXIT;
}

int
flow_legacy_ipv4_match_app_specific_operation_cb(
    int unit,
    uint32 current_entry_handle_id,
    uint32 prev_entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_cmd_control_info_t * flow_cmd_control,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    void *app_entry_data,
    bcm_flow_special_fields_t * special_fields,
    void *lif_info,
    uint32 *selectable_result_types)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (flow_cmd_control->flow_cb_type)
    {
        case FLOW_CB_TYPE_SET:
        case FLOW_CB_TYPE_GET:
        case FLOW_CB_TYPE_DELETE:
            SHR_IF_ERR_EXIT(flow_legacy_match_app_ipv4_tunnel_type_set(unit, current_entry_handle_id, special_fields));
            break;
        case FLOW_CB_TYPE_TRAVERSE:
            SHR_IF_ERR_EXIT(flow_legacy_match_app_ipv4_tunnel_type_get(unit, current_entry_handle_id, special_fields));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid flow_cb_type (%d)", (uint32) flow_cmd_control->flow_cb_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

int
flow_ipv6_tcam_match_is_valid(
    int unit)
{
    return UTILEX_NUM2BOOL(dnx_data_tunnel.tcam_table.has_raw_udp_tunnel_type_is_2nd_pass_key_fields_get(unit));
}

/*************************
 * IPVX_TUNNEL FUNCTIONS *
 *************************/
const dnx_flow_app_config_t dnx_legacy_flow_app_ipv4_tunnel_p2p_em_term_match = {
    /*
     * Application name
     */
    FLOW_APP_NAME_IPV4_P2P_EM_TERM_MATCH,

    /** related DBAL table */
    FLOW_MATCH_DBAL_TABLE_SET(DBAL_TABLE_IPV4_TT_P2P_EM_BASIC),

    /** Second Pass related DBAL table */
    DBAL_TABLE_IPV4_TT_P2P_EM_2ND_PASS,

    /*
     * Flow app type
     */
    FLOW_APP_TYPE_TERM_MATCH,

    /*
     * encap_access_default_mapping
     */
    bcmEncapAccessInvalid,

    /*
     * Match payload valid applications
     */
    {FLOW_APP_NAME_IPVX_TERMINATOR},

    /*
     * Bitwise for supported common fields
     */
    FLOW_NO_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_IPV4_P2P_EM_MATCH_SPECIAL_FIELDS,

    /*
     * Bitmap for supported flow handler flags
     */
    FLOW_APP_BASE_FLAGS | BCM_FLOW_HANDLE_INFO_BUD,

    /*
     * Indications
     */
    FLOW_APP_INDICATIONS_NONE,

    /*
     * verify cb
     */
    NULL,

    /*
     * app_specific_operations_cb
     */
    flow_legacy_ipv4_match_app_specific_operation_cb,

    /*
     * Generic callback for marking selectable result types - result_type_select_cb
     */
    NULL,

    /*
     *  Generic callback, used to indicate if the app is valid. usually the app should be
     *  valid according to dnx_data
     */
    flow_legacy_ipvx_match_app_is_valid
};

const dnx_flow_app_config_t dnx_legacy_flow_app_ipv4_tunnel_mp_em_term_match = {
    /*
     * Application name
     */
    FLOW_APP_NAME_IPV4_MP_EM_TERM_MATCH,

    /** related DBAL table */
    FLOW_MATCH_DBAL_TABLE_SET(DBAL_TABLE_IPV4_TT_MP_EM_BASIC),

    /** Second Pass related DBAL table */
    DBAL_TABLE_IPV4_TT_MP_EM_2ND_PASS,

    /*
     * Flow app type
     */
    FLOW_APP_TYPE_TERM_MATCH,

    /*
     * encap_access_default_mapping
     */
    bcmEncapAccessInvalid,

    /*
     * Match payload valid applications
     */
    {FLOW_APP_NAME_IPVX_TERMINATOR},

    /*
     * Bitwise for supported common fields
     */
    FLOW_NO_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_IPV4_MP_EM_MATCH_SPECIAL_FIELDS,

    /*
     * Bitmap for supported flow handler flags
     */
    FLOW_APP_BASE_FLAGS | BCM_FLOW_HANDLE_INFO_BUD,

    /*
     * Indications
     */
    FLOW_APP_INDICATIONS_NONE,

    /*
     * verify cb
     */
    NULL,

    /*
     * app_specific_operations_cb
     */
    flow_legacy_ipv4_match_app_specific_operation_cb,

    /*
     * Generic callback for marking selectable result types - result_type_select_cb
     */
    NULL,

    /*
     *  Generic callback, used to indicate if the app is valid. usually the app should be
     *  valid according to dnx_data
     */
    flow_legacy_ipvx_match_app_is_valid
};

const dnx_flow_app_config_t dnx_legacy_flow_app_ipv6_tunnel_mp_tcam_term_match = {
    /*
     * Application name
     */
    FLOW_APP_NAME_IPV6_MP_TCAM_TERM_MATCH,

    /** related DBAL table */
    FLOW_MATCH_DBAL_TABLE_SET(DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC),

    /** Second Pass related DBAL table */
    DBAL_TABLE_IPV6_MP_TT_TCAM_2ND_PASS,

    /*
     * Flow app type
     */
    FLOW_APP_TYPE_TERM_MATCH,

    /*
     * encap_access_default_mapping
     */
    bcmEncapAccessInvalid,

    /*
     * Match payload valid applications
     */
    {FLOW_APP_NAME_IPVX_TERMINATOR},

    /*
     * Bitwise for supported common fields
     */
    FLOW_NO_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_LEGACY_IPV6_MP_TCAM_MATCH_SPECIAL_FIELDS,

    /*
     * Bitmap for supported flow handler flags
     */
    FLOW_APP_BASE_FLAGS | BCM_FLOW_HANDLE_INFO_BUD,

    /*
     * Indications
     */
    FLOW_APP_INDICATIONS_NONE,

    /*
     * verify cb
     */
    NULL,

    /*
     * app_specific_operations_cb
     */
    NULL,

    /*
     * Generic callback for marking selectable result types - result_type_select_cb
     */
    NULL,

    /*
     *  Generic callback, used to indicate if the app is valid. usually the app should be
     *  valid according to dnx_data
     */
    flow_legacy_ipvx_match_app_is_valid
};

const dnx_flow_app_config_t dnx_legacy_flow_app_ipv6_tunnel_p2p_em_term_match = {
    /*
     * Application name
     */
    FLOW_APP_NAME_IPV6_P2P_EM_TERM_MATCH,

    /** related DBAL table */
    FLOW_MATCH_DBAL_TABLE_SET(DBAL_TABLE_IPV6_TT_P2P_EM_BASIC),

    /** Second Pass related DBAL table */
    DBAL_TABLE_EMPTY,

    /*
     * Flow app type
     */
    FLOW_APP_TYPE_TERM_MATCH,

    /*
     * encap_access_default_mapping
     */
    bcmEncapAccessInvalid,

    /*
     * Match payload valid applications
     */
    {FLOW_APP_NAME_IPVX_TERMINATOR},

    /*
     * Bitwise for supported common fields
     */
    FLOW_NO_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_IPV6_P2P_EM_MATCH_SPECIAL_FIELDS,

    /*
     * Bitmap for supported flow handler flags
     */
    FLOW_APP_BASE_FLAGS,

    /*
     * Indications
     */
    FLOW_APP_INDICATIONS_NONE,
    /*
     * verify cb
     */
    NULL,

    /*
     * app_specific_operations_cb
     */
    flow_ipv6_tunnel_p2p_em_term_match_app_specific_operations_cb,

    /*
     * Generic callback for marking selectable result types - result_type_select_cb
     */
    NULL,

    /*
     *  Generic callback, used to indicate if the app is valid. usually the app should be
     *  valid according to dnx_data
     */
    flow_legacy_ipvx_match_app_is_valid
};



