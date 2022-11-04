/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

/** \file flow.h
 * 
 *
 * This file contains the public APIs required for using FLOW module.
 *
 */

#ifndef  INCLUDE_FLOW_H_INCLUDED
#define  INCLUDE_FLOW_H_INCLUDED

/*************
 * INCLUDES  *
 *************/
/*
 * {
 */

#include <include/bcm/flow.h>
#include <src/bcm/dnx/flow/flow_def.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr_api.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/types/algo_flow_lif_types.h>

/*
 * }
 */
/*************
 * DEFINES   *
 *************/
/*
 * {
 */
/** list of all existing apps names - FLOW_APP_NAME_XXX */

/** tunnels */
#define FLOW_APP_NAME_IPV4_INITIATOR                                "IPV4_TUNNEL_INITIATOR"
#define FLOW_APP_NAME_IPV6_INITIATOR                                "IPV6_TUNNEL_INITIATOR"
#define FLOW_APP_NAME_IPVX_TERMINATOR                               "IPVX_TUNNEL_TERMINATOR"
#define FLOW_APP_NAME_IPV4_P2P_EM_TERM_MATCH                        "IPV4_TUNNEL_P2P_EM_TERM_MATCH"
#define FLOW_APP_NAME_IPV4_MP_EM_TERM_MATCH                         "IPV4_TUNNEL_MP_EM_TERM_MATCH"

#define FLOW_APP_NAME_IPV6_SRV6_INITIATOR                           "IPV6_SRV6_INITIATOR"
#define FLOW_APP_NAME_IPV6_SRV6_TERMINATOR                          "IPV6_SRV6_TERMINATOR"
#define FLOW_APP_NAME_IPV6_SRV6_TERM_MATCH                          "IPV6_SRV6_TUNNEL_TERM_MATCH"
#define FLOW_APP_NAME_IPV6_SRV6_FUNCTION_TERM_MATCH                 "IPV6_SRV6_TUNNEL_FUNCTION_TERM_MATCH"
#define FLOW_APP_NAME_SRV6_SID_INITIATOR                            "SRV6_SID_INITIATOR"
#define FLOW_APP_NAME_SRV6_SRH_BASE_INITIATOR                       "SRV6_SRH_BASE_INITIATOR"
#define FLOW_APP_NAME_SRV6_EXTENSION_TERMINATOR                     "SRV6_EXTENSION_TERMINATOR"
#define FLOW_APP_NAME_SRV6_EXTENSION_TERM_MATCH                     "SRV6_EXTENSION_TERM_MATCH"

#define FLOW_APP_NAME_IPV6_MP_TCAM_TERM_MATCH                       "IPV6_TUNNEL_MP_TCAM_TERM_MATCH"
#define FLOW_APP_NAME_IPV6_P2P_EM_TERM_MATCH                        "IPV6_TUNNEL_P2P_EM_TERM_MATCH"

/** vlan-port  */
#define FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR                       "VLAN_PORT_LL_TERMINATOR"
#define FLOW_APP_NAME_VLAN_PORT_LL_INITIATOR                        "VLAN_PORT_LL_INITIATOR"
#define FLOW_APP_NAME_VLAN_PORT_LL_PP_PORT_TERM_MATCH               "VLAN_PORT_LL_IN_PP_PORT_TERM_MATCH"
#define FLOW_APP_NAME_VLAN_PORT_LL_AC_S_VLAN_TERM_MATCH             "VLAN_PORT_LL_IN_AC_S_VLAN_TERM_MATCH"
#define FLOW_APP_NAME_VLAN_PORT_LL_AC_C_VLAN_TERM_MATCH             "VLAN_PORT_LL_IN_AC_C_VLAN_TERM_MATCH"
#define FLOW_APP_NAME_VLAN_PORT_LL_AC_S_C_VLAN_TERM_MATCH           "VLAN_PORT_LL_IN_AC_S_C_VLAN_TERM_MATCH"
#define FLOW_APP_NAME_VLAN_PORT_LL_AC_C_C_VLAN_TERM_MATCH           "VLAN_PORT_LL_IN_AC_C_C_VLAN_TERM_MATCH"
#define FLOW_APP_NAME_VLAN_PORT_LL_AC_S_S_VLAN_TERM_MATCH           "VLAN_PORT_LL_IN_AC_S_S_VLAN_TERM_MATCH"
#define FLOW_APP_NAME_VLAN_PORT_LL_AC_UNTAGGED_DPC_TERM_MATCH       "VLAN_PORT_LL_IN_AC_UNTAGGED_DPC_TERM_MATCH"
#define FLOW_APP_NAME_VLAN_PORT_LL_AC_TCAM_TERM_MATCH               "VLAN_PORT_LL_IN_AC_TCAM_TERM_MATCH"

/** Native */
#define FLOW_APP_NAME_VLAN_PORT_AC_NATIVE_TERMINATOR                    "VLAN_PORT_AC_NATIVE_TERMINATOR"
#define FLOW_APP_NAME_VLAN_PORT_AC_NATIVE_VIRTUAL_TERMINATOR            "VLAN_PORT_AC_NATIVE_VIRTUAL_TERMINATOR"
#define FLOW_APP_NAME_VLAN_PORT_AC_NATIVE_NW_SCOPED_0_VLAN_TERM_MATCH   "VLAN_PORT_AC_NATIVE_NW_SCOPED_0_VLAN_TERM_MATCH"
#define FLOW_APP_NAME_VLAN_PORT_AC_NATIVE_NW_SCOPED_1_VLAN_TERM_MATCH   "VLAN_PORT_AC_NATIVE_NW_SCOPED_1_VLAN_TERM_MATCH"
#define FLOW_APP_NAME_VLAN_PORT_AC_NATIVE_NW_SCOPED_2_VLAN_TERM_MATCH   "VLAN_PORT_AC_NATIVE_NW_SCOPED_2_VLAN_TERM_MATCH"
#define FLOW_APP_NAME_VLAN_PORT_AC_NATIVE_LIF_SCOPED_0_VLAN_TERM_MATCH  "VLAN_PORT_AC_NATIVE_LIF_SCOPED_0_VLAN_TERM_MATCH"
#define FLOW_APP_NAME_VLAN_PORT_AC_NATIVE_LIF_SCOPED_1_VLAN_TERM_MATCH  "VLAN_PORT_AC_NATIVE_LIF_SCOPED_1_VLAN_TERM_MATCH"
#define FLOW_APP_NAME_VLAN_PORT_AC_NATIVE_LIF_SCOPED_2_VLAN_TERM_MATCH  "VLAN_PORT_AC_NATIVE_LIF_SCOPED_2_VLAN_TERM_MATCH"

#define FLOW_APP_NAME_VLAN_PORT_ESEM_NW_SCOPED_INITIATOR            "VLAN_PORT_ESEM_NW_SCOPED_INITIATOR"
#define FLOW_APP_NAME_VLAN_PORT_ESEM_LIF_SCOPED_INITIATOR           "VLAN_PORT_ESEM_LIF_SCOPED_INITIATOR"
#define FLOW_APP_NAME_VLAN_PORT_ESEM_DEFAULT_INITIATOR              "VLAN_PORT_ESEM_DEFAULT_INITIATOR"
#define FLOW_APP_NAME_VLAN_PORT_ESEM_NAMESPACE_VSI_INITIATOR        "VLAN_PORT_ESEM_NAMESPACE_VSI_INITIATOR"
#define FLOW_APP_NAME_VLAN_PORT_ESEM_NAMESPACE_PORT_INITIATOR       "VLAN_PORT_ESEM_NAMESPACE_PORT_INITIATOR"

/** MPLS */
#define FLOW_APP_NAME_MPLS_TUNNEL_INITIATOR                         "MPLS_TUNNEL_INITIATOR"
#define FLOW_APP_NAME_MPLS_TUNNEL_TERMINATOR                        "MPLS_TUNNEL_TERMINATOR"
#define FLOW_APP_NAME_MPLS_TUNNEL_LABEL_TERM_MATCH                  "MPLS_TUNNEL_LABEL_TERM_MATCH"
#define FLOW_APP_NAME_MPLS_TUNNEL_LABEL_WITH_NETWORK_DOMAIN_TERM_MATCH   "MPLS_TUNNEL_LABEL_WITH_NETWORK_DOMAIN_TERM_MATCH"

/** GTP */
#define FLOW_APP_NAME_GTP_TERMINATOR                                "GTP_TERMINATOR"
#define FLOW_APP_NAME_GTP_INITIATOR                                 "GTP_INITIATOR"
#define FLOW_APP_NAME_GTP_TERM_MATCH                                "GTP_TERM_MATCH"

/** BIER */
#define FLOW_APP_NAME_BIER_TERM_MATCH                               "BIER_TERM_MATCH"
#define FLOW_APP_NAME_BIER_TERMINATOR                               "BIER_TERMINATOR"
#define FLOW_APP_NAME_BIER_BASE_INITIATOR                           "BIER_BASE_INITIATOR"
#define FLOW_APP_NAME_BIER_BITSTR_INITIATOR                         "BIER_BITSTR_INITIATOR"
#define FLOW_APP_NAME_BIER_PREFIX_INITIATOR                         "BIER_PREFIX_INITIATOR"

/** L3 */
#define FLOW_APP_NAME_ARP_INITIATOR                                 "ARP_INITIATOR"
/** MPLS_PORT */
#define FLOW_APP_NAME_MPLS_PORT_TERMINATOR                          "MPLS_PORT_TERMINATOR"
#define FLOW_APP_NAME_MPLS_PORT_INITIATOR                           "MPLS_PORT_INITIATOR"
#define FLOW_APP_NAME_MPLS_PORT_LABEL_TERM_MATCH                    "MPLS_PORT_LABEL_TERM_MATCH"

/** MPLS_EVPN */
#define FLOW_APP_NAME_MPLS_EVPN_TERMINATOR                          "MPLS_EVPN_TERMINATOR"
#define FLOW_APP_NAME_MPLS_EVPN_INITIATOR                           "MPLS_EVPN_INITIATOR"
#define FLOW_APP_NAME_MPLS_EVPN_LABEL_TERM_MATCH                    "MPLS_EVPN_LABEL_TERM_MATCH"

/** OAM */
#define FLOW_APP_NAME_OAM_BFD_INITIATOR                             "OAM_BFD_INITIATOR"
#define FLOW_APP_NAME_OAM_REFLECTOR_INITIATOR                       "OAM_REFLECTOR_INITIATOR"

/** RCH */
#define FLOW_APP_NAME_RCH_INITIATOR                                 "RCH_INITIATOR"

/** Reflector */
#define FLOW_APP_NAME_REFLECTOR_INITIATOR                           "REFLECTOR_INITIATOR"

/** PPP */
#define FLOW_APP_NAME_PPP_TERM_MATCH                               "PPP_TERM_MATCH"
#define FLOW_APP_NAME_PPP_TERMINATOR                               "PPP_TERMINATOR"
#define FLOW_APP_NAME_PPP_INITIATOR                                "PPP_INITIATOR"

/** PPP */
#define FLOW_APP_NAME_L2TP_TERM_MATCH                              "L2TP_TERM_MATCH"
#define FLOW_APP_NAME_L2TP_TERMINATOR                              "L2TP_TERMINATOR"
#define FLOW_APP_NAME_L2TP_INITIATOR                               "L2TP_INITIATOR"

/** Mirror */
#define FLOW_APP_NAME_MIRROR_RSPAN_INITIATOR                       "MIRROR_RSPAN_INITIATOR"
#define FLOW_APP_NAME_MIRROR_ERSPAN_INITIATOR                      "MIRROR_ERSPAN_INITIATOR"
#define FLOW_APP_NAME_HIGH_SCALE_MIRROR_MAPPING_INITIATOR          "HIGH_SCALE_MIRROR_MAPPING_INITIATOR"

#define FLOW_TERMINATOR_L3_INGRESS_FLAGS_SUPPORTED (BCM_L3_INGRESS_ROUTE_DISABLE_IP4_UCAST | BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST | BCM_L3_INGRESS_ROUTE_DISABLE_IP6_UCAST | \
                                                BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST | BCM_L3_INGRESS_ROUTE_DISABLE_MPLS | BCM_L3_INGRESS_ROUTE_ENABLE_UNKNOWN | BCM_L3_INGRESS_TUNNEL_DISABLE_NATIVE_ETH)

/** VNI */
#define FLOW_APP_NAME_VNI_TERM_MATCH                               "VNI_TERM_MATCH"
#define FLOW_APP_NAME_VNI_TERMINATOR                               "VNI_TERMINATOR"

/* Max number of match entries per gport */
#define FLOW_MAX_NOF_MATCH_ENTRIES 10

/*
 * }
 */
/*************
 * MACROS    *
 *************/
/*
 * {
 */
/** macros for adding data to special_fields struct
 *  Use it only if you don't set manually actual_nof_special_fields  */
#define SPECIAL_FIELD_SYMBOL_DATA_ADD(special_fields_struct, special_field_id, value) \
{ \
    (special_fields_struct)->special_fields[(special_fields_struct)->actual_nof_special_fields].field_id = special_field_id; \
    (special_fields_struct)->special_fields[(special_fields_struct)->actual_nof_special_fields].symbol = value; \
    (special_fields_struct)->actual_nof_special_fields++; \
}

#define SPECIAL_FIELD_UINT32_DATA_ADD(special_fields_struct, special_field_id, value) \
{ \
    (special_fields_struct)->special_fields[(special_fields_struct)->actual_nof_special_fields].field_id = special_field_id; \
    (special_fields_struct)->special_fields[(special_fields_struct)->actual_nof_special_fields].shr_var_uint32 = value; \
    (special_fields_struct)->actual_nof_special_fields++; \
}

#define SPECIAL_FIELD_UINT32_ARR_DATA_ADD(special_fields_struct, special_field_id, value_uint32_arr) \
{ \
    (special_fields_struct)->special_fields[(special_fields_struct)->actual_nof_special_fields].field_id = special_field_id; \
    sal_memcpy((special_fields_struct)->special_fields[(special_fields_struct)->actual_nof_special_fields].shr_var_uint32_arr, \
               value_uint32_arr, \
               sizeof((special_fields_struct)->special_fields[(special_fields_struct)->actual_nof_special_fields].shr_var_uint32_arr)); \
    (special_fields_struct)->actual_nof_special_fields++; \
}

#define SPECIAL_FIELD_UINT8_ARR_DATA_ADD(special_fields_struct, special_field_id, value_uint8_arr) \
{ \
    (special_fields_struct)->special_fields[(special_fields_struct)->actual_nof_special_fields].field_id = special_field_id; \
    sal_memcpy((special_fields_struct)->special_fields[(special_fields_struct)->actual_nof_special_fields].shr_var_uint8_arr, \
               value_uint8_arr, \
               sizeof((special_fields_struct)->special_fields[(special_fields_struct)->actual_nof_special_fields].shr_var_uint8_arr)); \
    (special_fields_struct)->actual_nof_special_fields++; \
}

#define SPECIAL_FIELD_ENABLER_ADD(special_fields_struct, special_field_id) \
{ \
    (special_fields_struct)->special_fields[(special_fields_struct)->actual_nof_special_fields].field_id = special_field_id; \
    (special_fields_struct)->actual_nof_special_fields++; \
}

#define SPECIAL_FIELD_CLEAR(special_fields_struct, special_field_id) \
{ \
    (special_fields_struct)->special_fields[(special_fields_struct)->actual_nof_special_fields].field_id = special_field_id; \
    (special_fields_struct)->special_fields[(special_fields_struct)->actual_nof_special_fields].is_clear = TRUE; \
    (special_fields_struct)->actual_nof_special_fields++; \
}

/** basic flags that all flow applications must support */
#define FLOW_APP_BASE_FLAGS  (BCM_FLOW_HANDLE_INFO_WITH_ID | BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_TRAVERSE_DELETE_ALL)

#define FLOW_GPORT_IS_VIRTUAL(flow_gport)   (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_INGRESS(flow_gport) || \
                                             BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_MATCH(flow_gport) || \
                                             BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_POINTED(flow_gport) || \
                                             BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(flow_gport))

/*
 * }
 */
/*************
 * TYPE DEFS *
 *************/
/*
 * {
 */
/* Enum defining action of match entry */
typedef enum
{
    FLOW_MATCH_ACTION_ADD = 0,
    FLOW_MATCH_ACTION_GET = 1,
    FLOW_MATCH_ACTION_DELETE = 2,
    FLOW_MATCH_ACTION_UPDATE = 3
} dnx_flow_match_action_e;

/** Enum used in order to use a specific operation during traverse operation (which operation will be performed once
 *  entry found */
typedef enum
{
    FLOW_TRAVERSE_OPERATION_USE_CALLBACK,
    FLOW_TRAVERSE_OPERATION_DELETE
} dnx_flow_traverse_operation_e;

/* Match entry info - handle info + key */
typedef struct flow_match_entry_s
{
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_special_fields_t special_fields;
} flow_match_entry_t;

/* Match entries info - nof entries and the entries themselves */
typedef struct flow_match_entries_s
{
    int nof_entries;
    flow_match_entry_t match_entries[FLOW_MAX_NOF_MATCH_ENTRIES];
} flow_match_entries_t;

/* Used by flow_app_match_entry_cb, holds gport to be matched in cb and match_entries struct to be updated */
typedef struct
{
    int gport;
    flow_match_entries_t *entries;
} dnx_flow_match_gport_traverse_data_t;

/* Used by flow_match_info_by_gport_entry_cb, holds gport to be matched in cb, max_size of the entries we have room to add, size for current nof_entries and match_array struct to be updated */
typedef struct
{
    int gport;
    int max_size;
    int nof_entries;
    bcm_flow_match_entry_info_t *match_array;
} dnx_flow_match_info_by_gport_traverse_data_t;

/** holds the current dbal logger info in order to restore it after the flow operation is done */
typedef struct
{
    bsl_severity_t api_severity;
    bsl_severity_t access_severity;
    bsl_severity_t diag_severity;
    dbal_logger_mode_e mode;
    dbal_tables_e table;
} dnx_flow_logger_data_t;

/** if flow logger is active, print the flow data */
#define DNX_FLOW_LOGGER_PRINT(unit, flow_handle_info, flow_app_info, flow_cmd_control, common_fields, special_fields) \
    { \
        if (bsl_check(bslLayerBcmdnx, bslSourceFlow, bslSeverityVerbose, unit)) \
        { \
            SHR_IF_ERR_EXIT(dnx_flow_logs_print(unit, flow_handle_info, flow_app_info, flow_cmd_control, \
                                                (void *) common_fields, special_fields)); \
        } \
    }

/** if flow logger is active, activate the dbal logger and print the flow data */
#define DNX_FLOW_LOGGER_START(unit, flow_handle_info, flow_app_info, flow_cmd_control, common_fields, special_fields, logger_data) \
    { \
        if (bsl_check(bslLayerBcmdnx, bslSourceFlow, bslSeverityVerbose, unit)) \
        { \
            dnx_flow_logger_update(unit, FLOW_LIF_DBAL_TABLE_GET(flow_app_info), &old_logger_data); \
            SHR_IF_ERR_EXIT(dnx_flow_logs_print(unit, flow_handle_info, flow_app_info, flow_cmd_control, \
                                                (void *) common_fields, special_fields)); \
        } \
    }

/** if flow logger is active, restore the dbal logger to its original status */
#define DNX_FLOW_LOGGER_END(unit, logger_data) \
    { \
        if (bsl_check(bslLayerBcmdnx, bslSourceFlow, bslSeverityVerbose, unit)) \
        { \
            dnx_flow_logger_restore(unit, &logger_data); \
        } \
    }

/*
 * }
 */
/*************
 * GLOBALS   *
 *************/
/*
 * {
 */

/*
 * }
 */
/*************
 * FUNCTIONS *
 *************/
/*
 * {
 */

int dnx_flow_init(
    int unit);

int dnx_flow_deinit(
    int unit);

shr_error_e dnx_flow_special_field_data_get(
    int unit,
    bcm_flow_special_fields_t * special_fields,
    bcm_flow_field_id_t field_id,
    bcm_flow_special_field_t * special_field_data);

shr_error_e dnx_flow_special_field_is_enabled(
    int unit,
    bcm_flow_special_fields_t * special_fields,
    bcm_flow_field_id_t field_id,
    uint8 *is_enabled);

shr_error_e dnx_flow_special_field_find(
    int unit,
    bcm_flow_special_fields_t * special_fields,
    flow_special_fields_e searched_special_field_id,
    int *special_field_index);

shr_error_e dnx_flow_special_field_find_in_flow_app(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    flow_special_fields_e searched_special_field_id,
    int *special_field_index);

/**
 * \brief
 * get an indication and returns if the indication is external (visible in diagnostics) or not
 */
shr_error_e dnx_flow_special_ind_is_external(
    int unit,
    uint32 indication,
    uint8 *is_ind_external);

/**
 * \brief
 * get special field id and returns its value, and if the value does not exists, and in case of replace, the old value that was set
 * also returns an indication if the value is new or old
 */
shr_error_e flow_special_field_new_value_or_previous_if_not_exist_get(
    int unit,
    bcm_flow_special_fields_t * special_fields,
    bcm_flow_special_fields_t * prev_special_fields,
    flow_special_fields_e field_id,
    uint8 is_replace,
    bcm_flow_special_field_t * special_field_data,
    uint8 *is_value_new);

/** for a specific gport and flow_app_type, returns the matching flow LIF app ID (terminator / initiator) */
shr_error_e flow_lif_flow_app_from_gport_get(
    int unit,
    bcm_gport_t gport,
    dnx_flow_app_type_e flow_app_type,
    uint32 *flow_handle);

/** for a specific gport which is encoded, return the matching flow handle */
shr_error_e flow_lif_flow_app_from_encoded_gport_get(
    int unit,
    bcm_gport_t gport,
    uint32 *flow_handle);

/*
 * \brief
 *   Retrieve the flow handle ID based on the DBAL table ID
 * \param [in] unit - relevant unit
 * \param [in] table_id - DBAL table ID
 * \param [out] flow_handle - relevant flow handle
 * \return
 *   Negative in case of an error. See \ref shr_error_e 
 */
shr_error_e flow_lif_flow_app_from_table_id_get(
    int unit,
    dbal_tables_e table_id,
    dnx_flow_app_type_e flow_app_type,
    uint32 *flow_handle);

/** Inputs are the global_lif and flow_handle_info->flow_handle ( flow match application) the function returns the
 *  releted entry key & flow_id */
shr_error_e dnx_flow_match_entry_from_global_lif_get(
    int unit,
    int global_lif,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_special_fields_t * key_special_fields);

shr_error_e dnx_flow_match_info_traverse(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_match_info_traverse_cb cb,
    void *user_data,
    dnx_flow_traverse_operation_e traverse_operation);

shr_error_e dnx_flow_terminator_info_traverse(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_terminator_info_traverse_cb cb,
    void *user_data,
    dnx_flow_traverse_operation_e traverse_operation);

shr_error_e dnx_flow_initiator_info_traverse(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_initiator_info_traverse_cb cb,
    void *user_data,
    dnx_flow_traverse_operation_e traverse_operation);

/**
 * \brief - Encode a Flow gport based on the application info
 *        and the supplied Global-LIF or Virtual-ID, and a
 *        Virtual indication.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] global_lif_or_virtual_id - Global-LIF or a
 *          Virtual-ID.
 *   \param [in] is_virtual - Indication whether the encoding
 *          should be done for a Virtual object.
 *   \param [in] flow_app_info - Flow application info structure
 * \return DIRECT OUTPUT:
 *   bcm_gport_t - The encoded flow gport
 */
bcm_gport_t dnx_flow_gport_encode_by_app(
    int unit,
    uint32 global_lif_or_virtual_id,
    uint32 is_virtual,
    const dnx_flow_app_config_t * flow_app_info);

shr_error_e dnx_flow_special_field_remove(
    int unit,
    bcm_flow_special_fields_t * special_fields,
    flow_special_fields_e searched_special_field_id);

/* Get match entries which payload is a given gport.
 * This is done by iterating over existing match entries of all the related match applications */
shr_error_e dnx_flow_match_info_by_gport_multiple_get(
    int unit,
    bcm_gport_t gport,
    flow_match_entries_t * match_entries);

/* Get match entries which payload is a given gport.
 * This is done by iterating over dedicated sw state */
shr_error_e dnx_flow_match_info_by_gport_multi_get(
    int unit,
    bcm_gport_t gport,
    int size,
    bcm_flow_match_entry_info_t * match_entries,
    int *count);

/** For egress pointed data entries LIFs (LIFs that dont have a related GLOBAL LIF and there are created internally),
 *  used to allocate SW and set to HW, the entry_handle_id should be filled with all the result fields and the correct
 *  result type. Note that in order to perform get need to user regular DBAL APIs. */
shr_error_e dnx_flow_egress_pointed_data_entry_lif_create(
    int unit,
    uint32 entry_handle_id,
    dnx_algo_local_outlif_logical_phase_e logical_phase,
    int *local_lif_id);

/** For egress pointed data entries LIFs (LIFs that dont have a related GLOBAL LIF and there are created internally),
 *  used to update existing lif. It is not supporting result type change. The entry_handle_id should be filled with
 *  relevant result fields that needs update and the correct result type */
shr_error_e dnx_flow_egress_pointed_data_entry_lif_update(
    int unit,
    uint32 entry_handle_id,
    int *local_lif_id);

/** For egress pointed data entries LIFs (LIFs that dont have a related GLOBAL LIF and there are created internally),
 *  used to free from SW and clear from HW */
shr_error_e dnx_flow_egress_pointed_data_entry_lif_delete(
    int unit,
    int local_lif_id);

void dnx_flow_logger_update(
    int unit,
    dbal_tables_e flow_dbal_table,
    dnx_flow_logger_data_t * old_logger_data);

void dnx_flow_logger_restore(
    int unit,
    dnx_flow_logger_data_t * dbal_logger_data);

shr_error_e dnx_flow_logs_print(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_flow_cmd_control_info_t * flow_cmd_control,
    void *common_fields,
    bcm_flow_special_fields_t * special_fields);

/*
 * }
 */

#endif /* INCLUDE_FLOW_H_INCLUDED */
