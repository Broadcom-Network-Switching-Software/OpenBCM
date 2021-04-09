#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <shared/shrextend/shrextend_debug.h>
#include "flow_def.h"

extern dnx_flow_app_config_t *dnx_flow_objects[];

/** Supported initiator common fields */
#define VALID_IPV4_TUNNEL_INITIATOR_GRE_COMMON_FIELDS \
        BCM_FLOW_INITIATOR_ELEMENT_STAT_ID_VALID | \
        BCM_FLOW_INITIATOR_ELEMENT_STAT_PP_PROFILE_VALID | \
        BCM_FLOW_INITIATOR_ELEMENT_QOS_MAP_ID_VALID | \
        BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID | \
        BCM_FLOW_INITIATOR_ELEMENT_ENCAP_ACCESS_VALID | \
        BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID | \
        BCM_FLOW_INITIATOR_ELEMENT_TTL_VALID | \
        BCM_FLOW_INITIATOR_ELEMENT_DSCP_VALID

/** Supported terminator common fields */
#define VALID_IPV4_TUNNEL_TERMINATOR_COMMON_FIELDS \
        BCM_FLOW_TERMINATOR_ELEMENT_VRF_VALID | \
        BCM_FLOW_TERMINATOR_ELEMENT_STAT_ID_VALID | \
        BCM_FLOW_TERMINATOR_ELEMENT_STAT_PP_PROFILE_VALID | \
        BCM_FLOW_TERMINATOR_ELEMENT_QOS_MAP_ID_VALID | \
        BCM_FLOW_TERMINATOR_ELEMENT_QOS_INGRESS_MODEL_VALID | \
        BCM_FLOW_TERMINATOR_ELEMENT_FLOW_SERVICE_TYPE_VALID | \
        BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID | \
        BCM_FLOW_TERMINATOR_ELEMENT_ACTION_GPORT_VALID | \
        BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID | \
        BCM_FLOW_TERMINATOR_ELEMENT_CLASS_ID_VALID

/** Supported initiator special fields */
#define VALID_IPV4_TUNNEL_INITIATOR_GRE_SPECIAL_FIELDS {DBAL_FIELD_IPV4_DIP, DBAL_FIELD_IPV4_SIP}

/** Supported match special fields */
#define VALID_IPV4_P2P_MATCH_SPECIAL_FIELDS {DBAL_FIELD_VRF, DBAL_FIELD_IPV4_DIP, DBAL_FIELD_IPV4_SIP, DBAL_FIELD_IPV4_TUNNEL_TYPE}

/** Supported initiator indications */
#define VALID_IPV4_TUNNEL_INITIATOR_INDICATIONS SAL_BIT(FLOW_APP_INIT_IND_SRC_ADDR_PROFILE)

/* For IPV4 tunnel initiator gre, need to specify that no additional headers are required per every entry */
int
flow_ipv4_tunnel_initiator_gre_app_specific_operations_cb(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_cmd_control_info_t * flow_cmd_control,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    void *app_entry_data,
    bcm_flow_special_fields_t * special_fields,
    void *lif_info,
    uint32 *selectable_result_types)
{
    lif_table_mngr_outlif_info_t *outlif_info = (lif_table_mngr_outlif_info_t *) lif_info;

    SHR_FUNC_INIT_VARS(unit);

    if (!(flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_SET))
    {
        SHR_EXIT();
    }

    outlif_info->table_specific_flags |= DNX_ALGO_LIF_TABLE_SPECIFIC_GRE_ANY_IN_4;

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIF_AHP_GRE_GENEVE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_LIF_AHP_GRE_GENEVE_LIF_AHP_NONE);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_DECREMENT_DISABLE, INST_SINGLE, FALSE);

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flow_ipv4_tunnel_match_em_p2p_verify_cb(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    void *flow_info,
    bcm_flow_special_fields_t * special_fields)
{
    int idx;

    SHR_FUNC_INIT_VARS(unit);

    for (idx = 0; idx < special_fields->actual_nof_special_fields; idx++)
    {
        if (special_fields->special_fields[idx].field_id == DBAL_FIELD_IPV4_TUNNEL_TYPE)
        {
            break;
        }
    }

    if (idx >= special_fields->actual_nof_special_fields)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "For IPV4 match, IPV4 tunnel type must be provided");
    }

exit:
    SHR_FUNC_EXIT;
}

const dnx_flow_app_config_t dnx_flow_app_ipv4_tunnel_initiator = {
    /*
     * Application name
     */
    "IPV4_TUNNEL_GRE_INITIATOR",
    /** related DBAL table */
    DBAL_TABLE_EEDB_IPV4_TUNNEL,

    /*
     * Flow app type
     */
    FLOW_APP_TYPE_INIT,

    /*
     * Bitwise for supported common fields
     */
    VALID_IPV4_TUNNEL_INITIATOR_GRE_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_IPV4_TUNNEL_INITIATOR_GRE_SPECIAL_FIELDS,

    /*
     * Bitmap for supported flow handler flags
     */
    BCM_FLOW_HANDLE_INFO_WITH_ID | BCM_FLOW_HANDLE_INFO_REPLACE,

    /*
     * Indications
     */
    VALID_IPV4_TUNNEL_INITIATOR_INDICATIONS,

    /*
     * verify cb
     */
    NULL,
    /*
     * app_specific_operations_cb
     */
    flow_ipv4_tunnel_initiator_gre_app_specific_operations_cb,

    /*
     * Generic callback for marking selectable result types - result_type_select_cb
     */
    NULL
};

const dnx_flow_app_config_t dnx_flow_app_ipv4_tunnel_terminator = {
    /*
     * Application name
     */
    "IPV4_TUNNEL_TERMINATOR",
    /** related DBAL table */
    DBAL_TABLE_IN_LIF_IPvX_TUNNELS,

    /*
     * Flow app type
     */
    FLOW_APP_TYPE_TERM,

    /*
     * Bitwise for supported common fields
     */
    VALID_IPV4_TUNNEL_TERMINATOR_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    {0},

    /*
     * Bitmap for supported flow handler flags
     */
    BCM_FLOW_HANDLE_INFO_WITH_ID | BCM_FLOW_HANDLE_INFO_REPLACE,

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
    NULL
};

const dnx_flow_app_config_t dnx_flow_app_ipv4_tunnel_match_em_p2p = {
    /*
     * Application name
     */
    "IPV4_TUNNEL_P2P_EM_MATCH",
    /** related DBAL table */
    DBAL_TABLE_IPV4_TT_P2P_EM_BASIC,

    /*
     * Flow app type
     */
    FLOW_APP_TYPE_MATCH,

    /*
     * Bitwise for supported common fields
     */
    FLOW_NO_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_IPV4_P2P_MATCH_SPECIAL_FIELDS,

    /*
     * Bitmap for supported flow handler flags
     */
    BCM_FLOW_HANDLE_INFO_WITH_ID | BCM_FLOW_HANDLE_INFO_REPLACE,

    FLOW_APP_INDICATIONS_NONE,
    /*
     * verify cb
     */
    flow_ipv4_tunnel_match_em_p2p_verify_cb,
    /*
     * app_specific_operations_cb
     */
    NULL,

    /*
     * Generic callback for marking selectable result types - result_type_select_cb
     */
    NULL
};

const dnx_flow_app_config_t dnx_flow_app_ipv4_tunnel_match_tcam_p2p = {
    /*
     * Application name
     */
    "IPV4_TUNNEL_P2P_TCAM_MATCH",
    /** related DBAL table */
    DBAL_TABLE_IPV4_TT_TCAM_BASIC,

    /*
     * Flow app type
     */
    FLOW_APP_TYPE_MATCH,

    /*
     * Bitwise for supported common fields
     */
    FLOW_NO_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_IPV4_P2P_MATCH_SPECIAL_FIELDS,

    /*
     * Bitmap for supported flow handler flags
     */
    BCM_FLOW_HANDLE_INFO_WITH_ID | BCM_FLOW_HANDLE_INFO_REPLACE,

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
    NULL
};
