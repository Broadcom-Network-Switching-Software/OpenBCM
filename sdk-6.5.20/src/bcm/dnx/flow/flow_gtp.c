/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include "flow_def.h"

/** Supported terminator common fields */
#define VALID_GTP_TERMINATOR_COMMON_FIELDS BCM_FLOW_TERMINATOR_ELEMENT_VRF_VALID | \
                                           BCM_FLOW_TERMINATOR_ELEMENT_STAT_ID_VALID | \
                                           BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID | \
                                           BCM_FLOW_TERMINATOR_ELEMENT_STAT_PP_PROFILE_VALID | \
                                           BCM_FLOW_TERMINATOR_ELEMENT_QOS_INGRESS_MODEL_VALID | \
                                           BCM_FLOW_TERMINATOR_ELEMENT_FLOW_SERVICE_TYPE_VALID | \
                                           BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID

/** Supported initiator common fields */
#define VALID_GTP_INITIATOR_COMMON_FIELDS BCM_FLOW_INITIATOR_ELEMENT_ENCAP_ACCESS_VALID | \
                                          BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID | \
                                          BCM_FLOW_INITIATOR_ELEMENT_STAT_PP_PROFILE_VALID | \
                                          BCM_FLOW_INITIATOR_ELEMENT_STAT_ID_VALID | \
                                          BCM_FLOW_INITIATOR_ELEMENT_ACTION_GPORT_VALID | \
                                          BCM_FLOW_INITIATOR_ELEMENT_MTU_PROFILE_VALID

/** Supported initiator speciel fields */
#define VALID_GTP_INITIATOR_SPECIAL_FIELDS {DBAL_FIELD_TUNNEL_ENDPOINT_IDENTIFIER}

/** Supported Match speciel fields */
#define VALID_GTP_MATCH_SPECIAL_FIELDS {DBAL_FIELD_TUNNEL_ENDPOINT_IDENTIFIER, \
                                            DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN}

dnx_flow_app_common_config_t dnx_flow_gtp_common_config = {
    /*
     * is_verify_disabled
     */
    0,
    /*
     * is_error_recovery_disabled
     */
    0
};

const dnx_flow_app_config_t dnx_flow_app_gtp_terminator = {
    /*
     * Application name
     */
    "GTP_TERMINATION",
    /** related DBAL table */
    DBAL_TABLE_IN_LIF_FORMAT_GTP,

    /*
     * Flow app type 
     */
    FLOW_APP_TYPE_TERM,

    /*
     * Common application configuration 
     */
    &dnx_flow_gtp_common_config,

    /*
     * Bitwise for supported common fields
     */
    VALID_GTP_TERMINATOR_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_GTP_INITIATOR_SPECIAL_FIELDS,

    /*
     * verify cb 
     */
    0
        /*
         * TBD mechanism to pick the result type (override generic method)
         */
        /*
         * NULL
         */
};

const dnx_flow_app_config_t dnx_flow_app_gtp_initiator = {
    /*
     * Application name
     */
    "GTP_INITIATOR",
    /** related DBAL table */
    DBAL_TABLE_EEDB_GTP,

    /*
     * Flow app type 
     */
    FLOW_APP_TYPE_INIT,

    /*
     * Common application configuration
     */
    &dnx_flow_gtp_common_config,

    /*
     * Bitwise for supported common fields
     */
    VALID_GTP_INITIATOR_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_GTP_INITIATOR_SPECIAL_FIELDS,

    /*
     * verify cb
     */
    0
        /*
         * TBD mechanism to pick the result type (override generic method)
         */
        /*
         * NULL
         */
};

const dnx_flow_app_config_t dnx_flow_app_gtp_match = {
    /*
     * Application name
     */
    "GTP_CLASSIFICATION",

    /** related DBAL table */
    DBAL_TABLE_GTP_CLASSIFICATION,

    /*
     * Flow app type 
     */
    FLOW_APP_TYPE_MATCH,

    /*
     * Common application configuration
     */
    &dnx_flow_gtp_common_config,

    /*
     * Bitwise for supported common fields
     */
    0,

    /*
     * specific table special fields
     */
    VALID_GTP_MATCH_SPECIAL_FIELDS,

    /*
     * verify cb
     */
    0
        /*
         * TBD mechanism to pick the result type (override generic method)
         */
        /*
         * NULL
         */
};
