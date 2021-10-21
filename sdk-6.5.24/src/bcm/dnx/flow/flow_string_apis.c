#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_flow.h>
#include "flow_def.h"

/**
 * \brief
 * Strings mapping array for the flow application type enum
 */
static const char *flow_app_type_string[FLOW_APP_TYPE_NOF_TYPES] = {
    "TERMINATOR",
    "INITIATOR",
    "TERM_MATCH",
    "INIT_MATCH",
};

/**
 *  \brief Strings mapping array for the flow init application indications
 */
static const char *flow_app_init_indications_string[FLOW_NOF_APP_INIT_IND] = {
    "SRC_ADDR_PROFILE_IND",
    "L2_GLOBAL_LIF",
    "VIRTUAL_LIF",
    "NO_LOCAL_LIF",
    "MANDATORY_L3_INTF",
    "IGNORE_L3_INTF"
};

/**
 *  \brief Strings mapping array for the special fields indications
 */
static const char *flow_special_fields_indications_string[FLOW_NOF_SPECIAL_FIELDS_IND] = {
    "MASKABLE",
    "IS_MASK",
    "LIF_IGNORE_OPERATION",
    "MATCH_IGNORE_OPERATION",
    "SRC_ADDR_PROFILE_SUPPORTED",
    "LIF_MANDATORY",
    "MATCH_MANDATORY",
    "LIF_UNCHANGEABLE",
    "MATCH_UNCHANGEABLE",
    "VIRTUAL_MATCH_KEY"
};

static const char *flow_payload_types_string[FLOW_NOF_PAYLOAD_TYPES] = {
    "UINT32",
    "UINT32_ARR",
    "UINT8_ARR",
    "ENUM",
    "BCM_ENUM",
    "ENABLER"
};

/**
 * \brief String for returning invalid string
 */
const char *flow_unknown_string = "unknown";

/** 
 *  \brief Strings mapping array for the flow term application indications
 */
static const char *flow_app_term_indications_string[FLOW_NOF_APP_TERM_IND] = {
    "QOS_PHB_DEFAULT_PIPE_IND",
    "IGNORE_L2_LEARN_INFO_SET_IND"
};

/** 
 *  \brief Strings mapping array for the flow term match application indications
 */
static const char *flow_app_term_match_indications_string[FLOW_NOF_APP_TERM_MATCH_IND] = {
    "MANY_TO_ONE"
};

/** 
 *  \brief Strings mapping array for the flow init match application indications
 */
static const char *flow_app_init_match_indications_string[FLOW_NOF_APP_INIT_MATCH_IND] = {
    "DUMMY_INIT_MATCH_IND"
};

/**
 * \brief
 * Strings mapping array for the flow handler flags 
 * The order of the strings should reflect the assigned bits for 
 * the flags. 
 */
static const char *flow_app_valid_flags_string[] = {
    "BCM_FLOW_HANDLE_INFO_WITH_ID",
    "BCM_FLOW_HANDLE_INFO_REPLACE",
    "BCM_FLOW_HANDLE_INFO_SYMMETRIC_ALLOC",
    "BCM_FLOW_HANDLE_INFO_VIRTUAL",
    "BCM_FLOW_HANDLE_INFO_BUD",
    "BCM_FLOW_HANDLE_TRAVERSE_DELETE_ALL",
    "BCM_FLOW_HANDLE_NATIVE"
};

/**
 * \brief
 * Strings mapping array for encap access
 */
static const char *flow_field_string_encap_access[] = {
    "Invalid",  /* bcmEncapAccessInvalid */
    "Tunnel-1", /* bcmEncapAccessTunnel1 */
    "Tunnel-2", /* bcmEncapAccessTunnel2 */
    "Tunnel-3", /* bcmEncapAccessTunnel3 */
    "Tunnel-4", /* bcmEncapAccessTunnel4 */
    "Native-Arp",       /* bcmEncapAccessNativeArp */
    "Native-Ac",        /* bcmEncapAccessNativeAc */
    "Arp",      /* bcmEncapAccessArp */
    "Rif",      /* bcmEncapAccessRif */
    "Ac",       /* bcmEncapAccessAc */
    ""
};

/**
 * \brief
 * Retrieve the flow handle's application name string
 */
const char *
dnx_flow_handle_to_string(
    int unit,
    bcm_flow_handle_t flow_handle_id)
{
    const dnx_flow_app_config_t *flow_app;

    flow_app = dnx_flow_app_info_get(unit, flow_handle_id);

    return flow_app->app_name;
}

/**
 * \brief
 * Retrieve the flow common field name string
 */
const char *
dnx_flow_common_field_enabler_to_string(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    uint32 common_field_enabler)
{
    int common_field_index;
    const dnx_flow_common_field_desc_t *field_desc = NULL;
    uint8 rv;

    /** gets the common field's index */
    rv = dnx_flow_common_fields_index_by_enabler_get(unit, flow_app_type, common_field_enabler, &common_field_index);

    if (rv != _SHR_E_NONE)
    {
        return flow_unknown_string;
    }

    /** gets the common field's desc */
    rv = dnx_flow_common_fields_desc_by_app_type_get(unit, flow_app_type, common_field_index, &field_desc);

    if (rv != _SHR_E_NONE)
    {
        return flow_unknown_string;
    }

    /** return "UNKNOWN" in case or error or miss-match */
    if (field_desc == NULL)
    {
        return flow_unknown_string;
    }

    return field_desc->field_name;
}

/**
 * \brief
 * Retrieve the flow handle's indication name string
 */
const char *
dnx_flow_app_indications_to_string(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    uint32 app_inidication)
{
    switch (flow_app_type)
    {
        case FLOW_APP_TYPE_INIT:
            if (app_inidication >= FLOW_NOF_APP_INIT_IND)
            {
                return "INVALID - out of range Init indication";
            }
            return flow_app_init_indications_string[app_inidication];
            break;

        case FLOW_APP_TYPE_TERM:
            if (app_inidication >= FLOW_NOF_APP_TERM_IND)
            {
                return "INVALID - out of range Term indication";
            }
            return flow_app_term_indications_string[app_inidication];
            break;

        case FLOW_APP_TYPE_TERM_MATCH:
            if (app_inidication >= FLOW_NOF_APP_TERM_MATCH_IND)
            {
                return "INVALID - out of range Term Match indication";
            }
            return flow_app_term_match_indications_string[app_inidication];
            break;

        case FLOW_APP_TYPE_INIT_MATCH:
            if (app_inidication >= FLOW_NOF_APP_INIT_MATCH_IND)
            {
                return "INVALID - out of range Init Match indication";
            }
            return flow_app_init_match_indications_string[app_inidication];
            break;

        default:
            return "INVALID - no such app type";
    }
}

const char *
dnx_flow_special_fields_indications_to_string(
    int unit,
    flow_special_fields_indications_e special_field_inidication)
{
    if (special_field_inidication >= FLOW_NOF_SPECIAL_FIELDS_IND)
    {
        return flow_unknown_string;
    }
    return flow_special_fields_indications_string[special_field_inidication];
}

const char *
dnx_flow_payload_types_to_string(
    int unit,
    flow_payload_type_e payload_type)
{
    if (payload_type >= FLOW_NOF_PAYLOAD_TYPES)
    {
        return flow_unknown_string;
    }
    return flow_payload_types_string[payload_type];
}

/**
 * \brief
 * Convert an application type to its name string
 */
const char *
flow_app_type_to_string(
    int unit,
    dnx_flow_app_type_e flow_app_type)
{
    if (flow_app_type < FLOW_APP_TYPE_NOF_TYPES)
    {
        return flow_app_type_string[flow_app_type];
    }
    return flow_unknown_string;
}

/**
 * \brief
 * Convert a FLOW handler flag to its name string
 */
const char *
flow_app_valid_flag_to_string(
    int unit,
    uint32 flow_handler_flag)
{
    int nof_valid_flags = sizeof(flow_app_valid_flags_string) / sizeof(flow_app_valid_flags_string[0]);
    if (flow_handler_flag < nof_valid_flags)
    {
        return flow_app_valid_flags_string[flow_handler_flag];
    }
    return flow_unknown_string;
}

const char *
dnx_flow_special_field_to_string(
    int unit,
    flow_special_fields_e field_id)
{
    const flow_special_field_info_t *special_field;
    int rv;

    rv = flow_special_field_info_get(unit, field_id, &special_field);
    if (rv)
    {
        return flow_unknown_string;
    }

    return special_field->name;
}

/**
 * \brief
 * Convert encap access value to its name string
 */
const char *
flow_encap_access_to_string(
    int unit,
    bcm_encap_access_t encap_access)
{
    if (encap_access < bcmEncapAccessAc)
    {
        return flow_field_string_encap_access[encap_access];
    }
    return flow_unknown_string;
}
