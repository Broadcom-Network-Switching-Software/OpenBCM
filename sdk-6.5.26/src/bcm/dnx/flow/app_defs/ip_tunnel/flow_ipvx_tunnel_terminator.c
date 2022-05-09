#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include "flow_ipvx_tunnel_terminator.h"
#include "../../flow_def.h"
#include "../../../tunnel/tunnel_term.h"
#include "../../../tunnel/tunnel_types.h"
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/lif/in_lif_profile.h>
#include <bcm_int/dnx/l2/l2.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_tunnel_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tunnel.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_flow.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>

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
        BCM_FLOW_TERMINATOR_ELEMENT_CLASS_ID_VALID | \
        BCM_FLOW_TERMINATOR_ELEMENT_LEARN_ENABLE_VALID | \
        BCM_FLOW_TERMINATOR_ELEMENT_L2_LEARN_INFO_VALID

/** Supported terminator special fields */
#define VALID_LEGACY_IPVX_TUNNEL_TERMINATOR_SPECIAL_FIELDS {FLOW_S_F_FODO_ASSIGNMENT_MODE}

/** ipvx_tunnel_term indications */
#define VALID_TUNNEL_TERMINATOR_INDICATIONS SAL_BIT(FLOW_APP_TERM_IND_QOS_PHB_DEFAULT_PIPE)

/************************************
 * IPVX_TUNNEL_TERMINATOR FUNCTIONS *
 ************************************/
/** Functions for ipvx_tunnel_terminator l2_learn_info set (field specific code and not app specific code) */
int
flow_legacy_ipvx_match_app_is_valid(
    int unit)
{
    return !UTILEX_NUM2BOOL(dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc));
}

int
flow_legacy_ivpx_tunnel_terminator_is_valid(
    int unit)
{
    return !UTILEX_NUM2BOOL(dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc));
}

int
flow_ivpx_tunnel_terminator_is_valid(
    int unit)
{
    return !flow_legacy_ivpx_tunnel_terminator_is_valid(unit);
}

static shr_error_e
dnx_flow_field_learn_type_get_verify(
    int unit,
    uint8 is_fec,
    uint8 is_outlif,
    uint32 l2_learn_info_flags)
{
    uint8 has_symmetric_flag = UTILEX_NUM2BOOL(l2_learn_info_flags & BCM_FLOW_HANDLE_INFO_SYMMETRIC_ALLOC);
    uint8 in_lif_asymmetric_with_out_lif = (is_outlif && !has_symmetric_flag);
    SHR_FUNC_INIT_VARS(unit);
    if (is_fec && in_lif_asymmetric_with_out_lif)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "no support for FEC with asymmetric lif \n");
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_field_learn_type_get(
    int unit,
    uint32 l2_learn_info_flags,
    uint8 is_fec,
    uint8 is_outlif,
    dnx_vxlan_learn_type_e * learn_type)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_flow_field_learn_type_get_verify(unit, is_fec, is_outlif, l2_learn_info_flags));

    if (is_outlif)
    {
        /*
         * learn dest:FEC + outlif
         */
        if (is_fec)
        {
            *learn_type = DNX_VXLAN_LEARN_TYPE_FEC_SYM_OUTLIF;
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
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_field_forward_information_get(
    int unit,
    bcm_gport_t dest_port,
    int global_out_lif,
    uint8 is_fec,
    uint8 is_outlif,
    dnx_algo_gpm_forward_info_t * forward_info)
{
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
        SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                        (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, dest_port, &(forward_info->destination)));

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
                        (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, dest_port, &(forward_info->destination)));
        forward_info->outlif = global_out_lif;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_legacy_field_payload_context_set(
    int unit,
    uint32 entry_handle_id,
    dnx_vxlan_learn_type_e learn_type)
{
    uint32 learn_payload_context;

    SHR_FUNC_INIT_VARS(unit);

    switch (learn_type)
    {
        case DNX_VXLAN_LEARN_TYPE_FEC_ASYM_OUTLIF:
            /*
             * ASYMMETRIC_LIF_BASIC_SVL 
             */
            learn_payload_context =
                DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_ASYMMETRIC_LIF__ANY_DEST__16_GROUP__2L_STRENGTH__SVL;
            break;
        case DNX_VXLAN_LEARN_TYPE_FEC_SYM_OUTLIF:
        case DNX_VXLAN_LEARN_TYPE_PORT_SYM_OUTLIF:
            /*
             * SYMMETRIC_LIF_EXTENDED_SVL 
             */
            learn_payload_context =
                DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF__FEC_OR_SPA_OR_TRAP_DEST__16_GROUP__2L_STRENGTH__SVL;
            break;
        case DNX_VXLAN_LEARN_TYPE_FEC_ONLY:
            /*
             * NO_LIF_EXTENDED 
             */
            learn_payload_context =
                DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF__FEC_OR_SPA_OR_TRAP_DEST__16_GROUP__2L_STRENGTH__BOTH;
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
dnx_flow_l2_learn_info_dbal_field_and_data_get(
    int unit,
    dnx_vxlan_learn_type_e learn_type,
    dnx_algo_gpm_forward_info_t * forward_info,
    dbal_fields_e * dbal_field_learn_info,
    uint32 *learn_info)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (learn_type)
    {
        case DNX_VXLAN_LEARN_TYPE_FEC_SYM_OUTLIF:
        case DNX_VXLAN_LEARN_TYPE_PORT_SYM_OUTLIF:
        case DNX_VXLAN_LEARN_TYPE_FEC_ONLY:
        case DNX_VXLAN_LERAN_TYPE_NO_LIF_BASIC_SYM_LIF:
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_LEARN_INFO_NO_LIF_BASIC_SYM_LIF_BASIC,
                                                            DBAL_FIELD_DESTINATION,
                                                            &forward_info->destination, learn_info));
            (*dbal_field_learn_info) = DBAL_FIELD_LEARN_INFO_NO_LIF_BASIC_SYM_LIF_BASIC;
            break;
        case DNX_VXLAN_LEARN_TYPE_FEC_ASYM_OUTLIF:
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported learn type  %d \n", learn_type);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_l2_legacy_learn_info_dbal_field_and_data_get(
    int unit,
    dnx_vxlan_learn_type_e learn_type,
    dnx_algo_gpm_forward_info_t * forward_info,
    dbal_fields_e * dbal_field_learn_info,
    uint32 *learn_info)
{
    /*
     * VXLAN needn't learn strength function, set it as 0 always 
     */
    uint32 learn_strength = 0;
    uint32 learn_strength_dest = forward_info->destination;

    SHR_FUNC_INIT_VARS(unit);

    SHR_BITCLR(&learn_strength_dest, DNX_L2_LEARN_STRENGT_OFFSET_IN_LEARN_INFO(0));

    switch (learn_type)
    {
        case DNX_VXLAN_LEARN_TYPE_FEC_ASYM_OUTLIF:
            /*
             * ASYMMETRIC_LIF_BASIC_SVL==>ASYMMETRIC_LIF__ANY_DEST__16_GROUP__2L_STRENGTH__SVL 
             */
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_LEARN_INFO_ASYM_LIF_BASIC_EXT, DBAL_FIELD_DESTINATION,
                             &forward_info->destination, learn_info));
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_LEARN_INFO_ASYM_LIF_BASIC_EXT, DBAL_FIELD_GLOB_OUT_LIF,
                             &forward_info->outlif, learn_info));
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_LEARN_INFO_ASYM_LIF_BASIC_EXT, DBAL_FIELD_LEARN_STRENGTH,
                             &learn_strength, learn_info));
            (*dbal_field_learn_info) = DBAL_FIELD_LEARN_INFO_ASYM_LIF_BASIC_EXT;
            break;
        case DNX_VXLAN_LEARN_TYPE_FEC_SYM_OUTLIF:
        case DNX_VXLAN_LEARN_TYPE_PORT_SYM_OUTLIF:
        case DNX_VXLAN_LEARN_TYPE_FEC_ONLY:
            
            /*
             * SYMMETRIC context, dest[20] is learn_strength 
             */
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_LEARN_INFO_ASYM_LIF_BASIC_EXT, DBAL_FIELD_DESTINATION,
                             &learn_strength_dest, learn_info));
            (*dbal_field_learn_info) = DBAL_FIELD_LEARN_INFO_ASYM_LIF_BASIC_EXT;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid type (%d)", (uint32) learn_type);
            break;
    }

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

    if ((dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_is_flow_enabled_for_legacy_applications))
        || (dnx_data_flow.ipv4.use_flow_lif_term_get(unit)))
    {
        SHR_IF_ERR_EXIT(dnx_flow_l2_learn_info_dbal_field_and_data_get(unit, learn_type, forward_info,
                                                                       &dbal_field_learn_info, learn_info));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_flow_l2_legacy_learn_info_dbal_field_and_data_get(unit, learn_type, forward_info,
                                                                              &dbal_field_learn_info, learn_info));
    }

    dbal_entry_value_field_arr32_set(unit, entry_handle_id, dbal_field_learn_info, INST_SINGLE, learn_info);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_ipvx_term_forward_information_get(
    int unit,
    bcm_flow_l2_learn_info_t * l2_learn_info,
    dnx_algo_gpm_forward_info_t * forward_info)
{
    dnx_algo_gpm_gport_hw_resources_t tunnel_init_gport_hw_resources = { 0 };
    uint8 is_fec = _SHR_GPORT_IS_FORWARD_PORT(l2_learn_info->dest_port);
    uint8 is_outlif = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get global out_lif
     */
    if (l2_learn_info->encap_id)
    {
        uint32 lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, l2_learn_info->encap_id, lif_flags, &tunnel_init_gport_hw_resources));

        is_outlif = UTILEX_NUM2BOOL(tunnel_init_gport_hw_resources.global_out_lif != UTILEX_INTERN_VAL_INVALID_32);
    }

    /** calculation of forward_info */
    SHR_IF_ERR_EXIT(dnx_flow_field_forward_information_get(unit, l2_learn_info->dest_port,
                                                           tunnel_init_gport_hw_resources.global_out_lif,
                                                           is_fec, is_outlif, forward_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_ipvx_term_learn_type_get(
    int unit,
    bcm_flow_l2_learn_info_t * l2_learn_info,
    dnx_vxlan_learn_type_e * learn_type)
{
    dnx_algo_gpm_gport_hw_resources_t tunnel_init_gport_hw_resources = { 0 };
    uint8 is_fec = _SHR_GPORT_IS_FORWARD_PORT(l2_learn_info->dest_port);
    uint8 is_outlif = FALSE;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get global out_lif
     */
    if (l2_learn_info->encap_id)
    {
        uint32 lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, l2_learn_info->encap_id, lif_flags, &tunnel_init_gport_hw_resources));

        is_outlif = UTILEX_NUM2BOOL(tunnel_init_gport_hw_resources.global_out_lif != UTILEX_INTERN_VAL_INVALID_32);
    }
    dnx_flow_field_learn_type_get(unit, l2_learn_info->l2_learn_info_flags, is_fec, is_outlif, learn_type);
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
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_flow_field_ipvx_term_learn_type_get(unit, l2_learn_info, &learn_type));
    SHR_IF_ERR_EXIT(dnx_flow_field_ipvx_term_forward_information_get(unit, l2_learn_info, &forward_info));
    SHR_IF_ERR_EXIT(dnx_flow_l2_learn_info_dbal_field_set(unit, entry_handle_id, learn_type, &forward_info));

    /** setting the payload_context field */
    if (dnx_data_l2.general.feature_get(unit, dnx_data_l2_general_learn_payload_res_optimize))
    {
        SHR_IF_ERR_EXIT(dnx_flow_legacy_field_payload_context_set(unit, entry_handle_id, learn_type));
    }
    else
    {
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_ipvx_tunnel_terminator_create(
    int unit,
    uint32 current_entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    void *app_entry_data,
    bcm_flow_special_fields_t * special_fields)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_ipvx_tunnel_terminator_replace(
    int unit,
    uint32 current_entry_handle_id,
    uint32 prev_entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_special_fields_t * special_fields)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_tunnel.feature.dip_idx_type_is_lif_field_get(unit))
    {
        SHR_EXIT();
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_ipvx_tunnel_terminator_get_traverse(
    int unit,
    uint32 current_entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_special_fields_t * special_fields)
{
    SHR_FUNC_INIT_VARS(unit);
    if (!dnx_data_tunnel.feature.dip_idx_type_is_lif_field_get(unit))
    {
        SHR_EXIT();
    }
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_ipvx_tunnel_terminator_delete(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_tunnel.feature.dip_idx_type_is_lif_field_get(unit))
    {
        SHR_EXIT();
    }
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;

}

/************************************
 * IPVX_TUNNEL_TERMINATOR CALLBACKS *
 ************************************/

int
flow_ipvx_tunnel_terminator_app_specific_operations_cb(
    int unit,
    uint32 current_entry_handle_id,
    uint32 prev_entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_cmd_control_info_t * flow_cmd_control,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    void *app_entry_data,
    bcm_flow_special_fields_t * special_fields,
    uint32 *selectable_result_types)
{
    SHR_FUNC_INIT_VARS(unit);

    /** relevant only for create */
    if (flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_SET)
    {
        uint8 is_replace = _SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE);
        if (!is_replace)
        {
            SHR_IF_ERR_EXIT(flow_ipvx_tunnel_terminator_create
                            (unit, current_entry_handle_id, flow_handle_info, app_entry_data, special_fields));
        }
        else
        {
            SHR_IF_ERR_EXIT(flow_ipvx_tunnel_terminator_replace
                            (unit, current_entry_handle_id, prev_entry_handle_id, flow_handle_info, special_fields));
        }
    }
    else if ((flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_GET)
             || (flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_TRAVERSE))
    {
        SHR_IF_ERR_EXIT(flow_ipvx_tunnel_terminator_get_traverse
                        (unit, current_entry_handle_id, flow_handle_info, special_fields));

    }
    else if (flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_DELETE)
    {
        SHR_IF_ERR_EXIT(flow_ipvx_tunnel_terminator_delete(unit, flow_handle_info));
    }
exit:
    SHR_FUNC_EXIT;
}

/************************************
 * IPVX_TUNNEL_TERMINATOR FUNCTIONS *
 ************************************/

const dnx_flow_app_config_t dnx_flow_app_ipvx_tunnel_terminator = {
    /*
     * Application name
     */

    FLOW_APP_NAME_IPVX_TERMINATOR,

    /** related DBAL table */
    FLOW_LIF_DBAL_TABLE_SET(DBAL_TABLE_IN_LIF_IPvX_TUNNELS),

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
    VALID_LEGACY_IPVX_TUNNEL_TERMINATOR_SPECIAL_FIELDS,

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
    flow_ipvx_tunnel_terminator_app_specific_operations_cb,

    /*
     * Generic callback for marking selectable result types - result_type_select_cb
     */
    NULL,

    /*
     *  Generic callback, used to indicate if the app is valid. usually the app should be
     *  valid according to dnx_data
     */
    flow_legacy_ivpx_tunnel_terminator_is_valid
};
