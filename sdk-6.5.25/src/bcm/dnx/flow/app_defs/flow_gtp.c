#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/flow/flow.h>
#include "../flow_def.h"

/** Supported terminator common fields */
#define VALID_GTP_TERMINATOR_COMMON_FIELDS BCM_FLOW_TERMINATOR_ELEMENT_VRF_VALID | \
                                           BCM_FLOW_TERMINATOR_ELEMENT_STAT_ID_VALID | \
                                           BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID | \
                                           BCM_FLOW_TERMINATOR_ELEMENT_STAT_PP_PROFILE_VALID | \
                                           BCM_FLOW_TERMINATOR_ELEMENT_QOS_INGRESS_MODEL_VALID | \
                                           BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID | \
                                           BCM_FLOW_TERMINATOR_ELEMENT_ADDITIONAL_DATA_VALID | \
                                           BCM_FLOW_TERMINATOR_ELEMENT_ACTION_GPORT_VALID | \
                                           BCM_FLOW_TERMINATOR_ELEMENT_QOS_MAP_ID_VALID | \
                                           BCM_FLOW_TERMINATOR_ELEMENT_CLASS_ID_VALID

/** Supported initiator common fields */
#define VALID_GTP_INITIATOR_COMMON_FIELDS BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID | \
                                          BCM_FLOW_INITIATOR_ELEMENT_STAT_PP_PROFILE_VALID | \
                                          BCM_FLOW_INITIATOR_ELEMENT_STAT_ID_VALID | \
                                          BCM_FLOW_INITIATOR_ELEMENT_ACTION_GPORT_VALID | \
                                          BCM_FLOW_INITIATOR_ELEMENT_MTU_PROFILE_VALID |\
                                          BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID

/** Supported initiator speciel fields */
#define VALID_GTP_INITIATOR_SPECIAL_FIELDS {FLOW_S_F_TUNNEL_ENDPOINT_IDENTIFIER, FLOW_S_F_QOS_DSCP, FLOW_S_F_GTP_PDU_TYPE}

/** Supported Match speciel fields */
#define VALID_GTP_MATCH_SPECIAL_FIELDS {FLOW_S_F_TUNNEL_ENDPOINT_IDENTIFIER, FLOW_S_F_NEXT_LAYER_NETWORK_DOMAIN}


int
flow_gtp_is_valid(
    int unit)
{
    if (dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc))
    {
        return FALSE;
    }

    return TRUE;
}

const dnx_flow_app_config_t dnx_flow_app_gtp_terminator = {
    /*
     * Application name
     */
    FLOW_APP_NAME_GTP_TERMINATOR,

    /** related DBAL table */
    FLOW_LIF_DBAL_TABLE_SET(DBAL_TABLE_IN_LIF_FORMAT_GTP),

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
    VALID_GTP_TERMINATOR_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    {0},

    /*
     * Bitmap for supported flow handler flags
     */
    FLOW_APP_BASE_FLAGS,

    /*
     * initiator indication
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
    flow_gtp_is_valid
};

const dnx_flow_app_config_t dnx_flow_app_gtp_initiator = {
    /*
     * Application name
     */
    FLOW_APP_NAME_GTP_INITIATOR,

    /** related DBAL table */
    FLOW_LIF_DBAL_TABLE_SET(DBAL_TABLE_EEDB_GTP),

    /** Second Pass related DBAL table */
    DBAL_TABLE_EMPTY,

    /*
     * Flow app type 
     */
    FLOW_APP_TYPE_INIT,

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
    VALID_GTP_INITIATOR_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_GTP_INITIATOR_SPECIAL_FIELDS,

    /*
     * Bitmap for supported flow handler flags
     */
    FLOW_APP_BASE_FLAGS,

    /*
     * initiator indication
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
    flow_gtp_is_valid
};

const dnx_flow_app_config_t dnx_flow_app_gtp_term_match = {
    /*
     * Application name
     */
    FLOW_APP_NAME_GTP_TERM_MATCH,

    /** related DBAL table */
    FLOW_MATCH_DBAL_TABLE_SET(DBAL_TABLE_GTP_CLASSIFICATION),

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
    {FLOW_APP_NAME_GTP_TERMINATOR},

    /*
     * Bitwise for supported common fields
     */
    FLOW_NO_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_GTP_MATCH_SPECIAL_FIELDS,

    /*
     * Bitmap for supported flow handler flags
     */
    FLOW_APP_BASE_FLAGS,

    /*
     * initiator indication
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
    flow_gtp_is_valid
};
