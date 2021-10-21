#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include "flow_ipvx_tunnel.h"
#include "../flow_def.h"
#include "../../tunnel/tunnel_term.h"
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/lif/in_lif_profile.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_tunnel_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tunnel.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_flow.h>

/** Supported terminator common fields */
#define VALID_IPVX_TUNNEL_TERMINATOR_COMMON_FIELDS \
        BCM_FLOW_TERMINATOR_ELEMENT_VRF_VALID | \
        BCM_FLOW_TERMINATOR_ELEMENT_VSI_VALID | \
        BCM_FLOW_TERMINATOR_ELEMENT_STAT_ID_VALID | \
        BCM_FLOW_TERMINATOR_ELEMENT_STAT_PP_PROFILE_VALID | \
        BCM_FLOW_TERMINATOR_ELEMENT_QOS_MAP_ID_VALID | \
        BCM_FLOW_TERMINATOR_ELEMENT_QOS_INGRESS_MODEL_VALID | \
        BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID | \
        BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID | \
        BCM_FLOW_TERMINATOR_ELEMENT_ADDITIONAL_DATA_VALID | \
        BCM_FLOW_TERMINATOR_ELEMENT_ACTION_GPORT_VALID | \
        BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID | \
        BCM_FLOW_TERMINATOR_ELEMENT_CLASS_ID_VALID


/** Supported terminator special fields */
#define VALID_IPVX_TUNNEL_TERMINATOR_SPECIAL_FIELDS {FLOW_S_F_VSI_ASSIGNMENT_MODE}

/** Supported match special fields */

/** ipv4_p2p_em */
#define VALID_IPV4_P2P_EM_MATCH_SPECIAL_FIELDS {FLOW_S_F_VRF, FLOW_S_F_IPV4_DIP, FLOW_S_F_IPV4_SIP, FLOW_S_F_IPV4_TUNNEL_TYPE}

/** ipv4_mp_em */
#define VALID_IPV4_MP_MATCH_SPECIAL_FIELDS {FLOW_S_F_VRF, FLOW_S_F_IPV4_DIP, FLOW_S_F_IPV4_TUNNEL_TYPE}

/** ipv6_mp_tcam */
#define VALID_IPV6_MP_TCAM_MATCH_SPECIAL_FIELDS {FLOW_S_F_VRF, FLOW_S_F_IPV6_DIP, FLOW_S_F_IPV6_DIP_MASK, FLOW_S_F_IPV6_TUNNEL_TYPE}

/** ipv6_p2p_em */
#define VALID_IPV6_P2P_EM_MATCH_SPECIAL_FIELDS {FLOW_S_F_IPV6_SIP, FLOW_S_F_TUNNEL_CLASS}

/** ipvx_tunnel_term indications */
#define VALID_TUNNEL_TERMINATOR_INDICATIONS SAL_BIT(FLOW_APP_TERM_IND_QOS_PHB_DEFAULT_PIPE)

/*************************
 * IPVX_TUNNEL FUNCTIONS *
 */

/** Functions for ipvx_tunnel_terminator l2_learn_info set (field specific code and not app specific code) */

int
flow_ipvx_match_legacy_app_is_valid(
    int unit)
{
    if (dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc))
    {
        return FALSE;
    }

    return TRUE;
}

static void
dnx_flow_field_learn_type_get(
    int unit,
    uint32 l2_learn_info_flags,
    uint8 is_fec,
    uint8 is_outlif,
    dnx_vxlan_learn_type_e * learn_type)
{
    uint8 in_lif_symmetric_with_out_lif =
        UTILEX_NUM2BOOL_INVERSE(l2_learn_info_flags & BCM_FLOW_HANDLE_INFO_SYMMETRIC_ALLOC);

    if (is_outlif)
    {
        /*
         * learn dest:FEC + outlif
         */
        if (is_fec)
        {
            if (in_lif_symmetric_with_out_lif)
            {
                *learn_type = DNX_VXLAN_LEARN_TYPE_FEC_SYM_OUTLIF;
            }
            else
            {
                *learn_type = DNX_VXLAN_LEARN_TYPE_FEC_ASYM_OUTLIF;
            }
        }
        else
        {
            *learn_type = DNX_VXLAN_LEARN_TYPE_PORT_SYM_OUTLIF;
        }
    }
    /*
     * learn destination only
     */
    else
    {
        *learn_type = DNX_VXLAN_LEARN_TYPE_FEC_ONLY;
    }
}

static shr_error_e
dnx_flow_field_forward_information_get(
    int unit,
    bcm_flow_l2_learn_info_t * l2_learn_info,
    int global_out_lif,
    uint8 is_fec,
    uint8 is_outlif,
    dnx_algo_gpm_forward_info_t * forward_info)
{
    bcm_gport_t gport_forward_port;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(forward_info, 0, sizeof(dnx_algo_gpm_forward_info_t));

    /*
     * Fill destination (from Gport) info Forward Info table (SW state)
     */
    if (is_fec)
    {
        /*
         * set destination with FEC
         */
        BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(gport_forward_port, l2_learn_info->dest_port);
        SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                        (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, gport_forward_port,
                         &(forward_info->destination)));

        /*
         * outlif + FEC
         */
        if (is_outlif)
        {
            forward_info->fwd_info_result_type = DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF;
            forward_info->outlif = global_out_lif;
        }
        /*
         * FEC only
         */
        else
        {
            forward_info->fwd_info_result_type = DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_ONLY;
        }
    }
    /*
     * outlif + port
     */
    else
    {
        forward_info->fwd_info_result_type = DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF;
        SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                        (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, l2_learn_info->dest_port,
                         &(forward_info->destination)));
        forward_info->outlif = global_out_lif;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_field_payload_context_set(
    int unit,
    uint32 entry_handle_id,
    dnx_vxlan_learn_type_e learn_type)
{
    uint32 learn_payload_context;

    SHR_FUNC_INIT_VARS(unit);

    switch (learn_type)
    {
        case DNX_VXLAN_LEARN_TYPE_FEC_ASYM_OUTLIF:
            learn_payload_context = DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_ASYMMETRIC_LIF_BASIC_SVL;
            break;
        case DNX_VXLAN_LEARN_TYPE_FEC_SYM_OUTLIF:
        case DNX_VXLAN_LEARN_TYPE_PORT_SYM_OUTLIF:
            learn_payload_context = DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_EXTENDED_SVL;
            break;
        case DNX_VXLAN_LEARN_TYPE_FEC_ONLY:
            learn_payload_context = DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF_EXTENDED;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid type (%d)", (uint32) learn_type);
            break;
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT, INST_SINGLE,
                                 learn_payload_context);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_l2_learn_info_dbal_field_set(
    int unit,
    uint32 entry_handle_id,
    dnx_vxlan_learn_type_e learn_type,
    dnx_algo_gpm_forward_info_t * forward_info)
{
    uint32 learn_info[2];
    dbal_fields_e dbal_field_learn_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(learn_info, 0x0, sizeof(learn_info));

    switch (learn_type)
    {
        case DNX_VXLAN_LEARN_TYPE_FEC_ASYM_OUTLIF:
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_LEARN_INFO_ASYM_LIF_BASIC_EXT, DBAL_FIELD_DESTINATION,
                             &forward_info->destination, learn_info));
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_LEARN_INFO_ASYM_LIF_BASIC_EXT, DBAL_FIELD_GLOB_OUT_LIF,
                             &forward_info->outlif, learn_info));
            dbal_field_learn_info = DBAL_FIELD_LEARN_INFO_ASYM_LIF_BASIC_EXT;
            break;
        case DNX_VXLAN_LEARN_TYPE_FEC_SYM_OUTLIF:
        case DNX_VXLAN_LEARN_TYPE_PORT_SYM_OUTLIF:
        case DNX_VXLAN_LEARN_TYPE_FEC_ONLY:
            
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_LEARN_INFO_ASYM_LIF_BASIC_EXT, DBAL_FIELD_DESTINATION,
                             &forward_info->destination, learn_info));
            dbal_field_learn_info = DBAL_FIELD_LEARN_INFO_ASYM_LIF_BASIC_EXT;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid type (%d)", (uint32) learn_type);
            break;
    }

    dbal_entry_value_field_arr32_set(unit, entry_handle_id, dbal_field_learn_info, INST_SINGLE, learn_info);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_ipvx_term_l2_learn_info_set(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_l2_learn_info_t * l2_learn_info)
{
    dnx_vxlan_learn_type_e learn_type;
    dnx_algo_gpm_forward_info_t forward_info;
    uint32 lif_flags;
    dnx_algo_gpm_gport_hw_resources_t tunnel_init_gport_hw_resources = { 0 };
    uint8 is_fec = _SHR_GPORT_SUB_TYPE_IS_FORWARD_GROUP(l2_learn_info->dest_port);
    uint8 is_outlif = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get global and local out_lif
     */
    if (l2_learn_info->encap_id)
    {
        sal_memset(&tunnel_init_gport_hw_resources, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
        lif_flags =
            (DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS);
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, l2_learn_info->encap_id, lif_flags, &tunnel_init_gport_hw_resources));

        is_outlif = UTILEX_NUM2BOOL(tunnel_init_gport_hw_resources.global_out_lif == UTILEX_INTERN_VAL_INVALID_32);
    }

    /** calculation of learn_type */
    dnx_flow_field_learn_type_get(unit, l2_learn_info->l2_learn_info_flags, is_fec, is_outlif, &learn_type);

    /** calculation of forward_info */
    dnx_flow_field_forward_information_get(unit, l2_learn_info, tunnel_init_gport_hw_resources.global_out_lif, is_fec,
                                           is_outlif, &forward_info);

    /** setting the learn_info fields */
    dnx_flow_l2_learn_info_dbal_field_set(unit, entry_handle_id, learn_type, &forward_info);

    /** setting the payload_context field */
    SHR_IF_ERR_EXIT(dnx_flow_field_payload_context_set(unit, entry_handle_id, learn_type));

exit:
    SHR_FUNC_EXIT;
}

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
            SHR_IF_ERR_EXIT(dnx_tunnel_terminator_ipv6_sip_compress_table_entry_add(unit,
                                                                                    special_field_data.shr_var_uint8_arr,
                                                                                    ipv6_sip_profile));
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
    uint8 field_idx;
    bcm_ip6_t sip6;

    SHR_FUNC_INIT_VARS(unit);

    /** get the sip */
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_traverse_ipv6_p2p_sip6_get(unit, entry_handle_id, &sip6));

    /** update the relevant special field with the sip's value */
    for (field_idx = 0; field_idx < special_fields->actual_nof_special_fields; field_idx++)
    {
        if (special_fields->special_fields[field_idx].field_id == FLOW_S_F_IPV6_SIP)
        {
            sal_memcpy(special_fields->special_fields[field_idx].shr_var_uint8_arr, sip6,
                       sizeof(special_fields->special_fields[field_idx].shr_var_uint8_arr));
            SHR_EXIT();
        }
    }
    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Key field IPV6_SIP was not part of the special_fields struct");

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_term_match_ipv6_p2p_em_ipv6_sip_destroy(
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

/*************************
 * IPVX_TUNNEL CALLBACKS *
 */

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
            SHR_IF_ERR_EXIT(flow_term_match_ipv6_p2p_em_ipv6_sip_destroy(unit,
                                                                         current_entry_handle_id, special_fields));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid flow_cb_type (%d)", (uint32) flow_cmd_control->flow_cb_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

int
flow_ipv4_tunnel_terminator_app_specific_operations_cb(
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
    uint32 common_fields_bitmap;
    uint32 ingress_info_bitmap;
    SHR_FUNC_INIT_VARS(unit);

    /** relevant only for create */
    if ((!(flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_SET)) ||
        _SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE))
    {
        SHR_EXIT();
    }

    /** get the common field bitmap */
    common_fields_bitmap = ((bcm_flow_terminator_info_t *) app_entry_data)->valid_elements_set;

    ingress_info_bitmap = (BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID |
                           BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID);

    /*
     * if neither of the ingress_info common fields was set, update to a default profile
     * this is required to avoid a break in the replace where no profile was set
     * this is a unique situation for this application as it is the only one which have
     * the two ingress_info common fields, and thus restricts setting of default values
     * Relevant only when not using dedicated logic for inlif profile settings
     */

    if ((!(common_fields_bitmap & ingress_info_bitmap)) &&
        (!dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_is_inlif_profile_dedicated_logic_enabled)))
    {
        in_lif_profile_info_t in_lif_profile_info;
        dnx_in_lif_profile_last_info_t last_profile;
        dbal_tables_e dbal_table_id;
        int old_in_lif_profile = DNX_IN_LIF_PROFILE_DEFAULT, new_in_lif_profile = DNX_IN_LIF_PROFILE_DEFAULT;

        dnx_in_lif_profile_last_info_t_init(&last_profile);
        SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));

        in_lif_profile_info.ingress_fields.lif_gport = flow_handle_info->flow_id;

        /** Allocate default inLIF profile */

        SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, current_entry_handle_id, &dbal_table_id));

        SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange(unit, &in_lif_profile_info, old_in_lif_profile,
                                                    &new_in_lif_profile, LIF, dbal_table_id, &last_profile));

        old_in_lif_profile = new_in_lif_profile;

        dbal_entry_value_field32_set(unit, current_entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE,
                                     (uint32) new_in_lif_profile);
    }

    /*
     */
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
 */

const dnx_flow_app_config_t dnx_flow_app_ipvx_tunnel_terminator = {
    /*
     * Application name
     */

    FLOW_APP_NAME_IPVX_TERMINATOR,

    /** related DBAL table */
    DBAL_TABLE_IN_LIF_IPvX_TUNNELS,

    /** Second Pass related DBAL table */
    DBAL_TABLE_EMPTY,

    /*
     * Flow app type
     */
    FLOW_APP_TYPE_TERM,

    /*
     * encap_access_default_mapping
     */
    bcmEncapAccessInvalid,

    /*
     * Match payload valid applications
     */
    FLOW_NO_MATCH_PAYLOAD_APPS,

    /*
     * Bitwise for supported common fields
     */
    VALID_IPVX_TUNNEL_TERMINATOR_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_IPVX_TUNNEL_TERMINATOR_SPECIAL_FIELDS,

    /*
     * Bitmap for supported flow handler flags
     */
    FLOW_APP_BASE_FLAGS,

    /*
     * Indications
     */
    VALID_TUNNEL_TERMINATOR_INDICATIONS,

    /*
     * verify cb
     */
    NULL,

    /*
     * app_specific_operations_cb
     */
    flow_ipv4_tunnel_terminator_app_specific_operations_cb,

    /*
     * Generic callback for marking selectable result types - result_type_select_cb
     */
    NULL,

    /*
     *  Generic callback, used to indicate if the app is valid. usually the app should be
     *  valid according to dnx_data
     */
    NULL
};

const dnx_flow_app_config_t dnx_flow_app_ipv4_tunnel_p2p_em_term_match = {
    /*
     * Application name
     */
    FLOW_APP_NAME_IPV4_P2P_EM_TERM_MATCH,

    /** related DBAL table */
    DBAL_TABLE_IPV4_TT_P2P_EM_BASIC,

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
    NULL,

    /*
     * Generic callback for marking selectable result types - result_type_select_cb
     */
    NULL,

    /*
     *  Generic callback, used to indicate if the app is valid. usually the app should be
     *  valid according to dnx_data
     */
    flow_ipvx_match_legacy_app_is_valid
};

const dnx_flow_app_config_t dnx_flow_app_ipv4_tunnel_mp_em_term_match = {
    /*
     * Application name
     */
    FLOW_APP_NAME_IPV4_MP_EM_TERM_MATCH,

    /** related DBAL table */
    DBAL_TABLE_IPV4_TT_MP_EM_BASIC,

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
    VALID_IPV4_MP_MATCH_SPECIAL_FIELDS,

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
    flow_ipvx_match_legacy_app_is_valid
};

const dnx_flow_app_config_t dnx_flow_app_ipv6_tunnel_mp_tcam_term_match = {
    /*
     * Application name
     */
    FLOW_APP_NAME_IPV6_MP_TCAM_TERM_MATCH,

    /** related DBAL table */
    DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC,

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
    VALID_IPV6_MP_TCAM_MATCH_SPECIAL_FIELDS,

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
    flow_ipvx_match_legacy_app_is_valid
};

const dnx_flow_app_config_t dnx_flow_app_ipv6_tunnel_p2p_em_term_match = {
    /*
     * Application name
     */
    FLOW_APP_NAME_IPV6_P2P_EM_TERM_MATCH,

    /** related DBAL table */
    DBAL_TABLE_IPV6_TT_P2P_EM_BASIC,

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
    flow_ipvx_match_legacy_app_is_valid
};

