
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <soc/sand/shrextend/shrextend_debug.h>
#include "flow_def.h"
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_flow.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <src/bcm/dnx/tunnel/tunnel_term.h>
#include <src/bcm/dnx/flow/flow_special_fields_desc.h>
#include <bcm_int/dnx/flow/flow.h>
#include <bcm_int/dnx/flow/flow_virtual_lif_mgmt.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/qos/qos.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm/l2.h>
#include <bcm/oam.h>

/**************************
 * FUNCTIONS DECLARATIONS *
 **************************/

int flow_special_field_ipv6_tunnel_type_set(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    int field_idx,
    bcm_flow_special_fields_t * special_fields);

int flow_special_field_ipv6_tunnel_type_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    flow_special_fields_e special_field_id,
    bcm_flow_special_field_t * special_field);

int flow_special_field_ipvx_ipsec_tunnel_type_set(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    int field_idx,
    bcm_flow_special_fields_t * special_fields);

int flow_special_field_ipvx_ipsec_tunnel_type_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    flow_special_fields_e special_field_id,
    bcm_flow_special_field_t * special_field);

int flow_special_field_pcp_dei_profile_set(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    int field_idx,
    bcm_flow_special_fields_t * special_fields);

int flow_special_field_pcp_dei_profile_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    flow_special_fields_e special_field_id,
    bcm_flow_special_field_t * special_field);

int flow_special_field_glob_in_lif_set(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    int field_idx,
    bcm_flow_special_fields_t * special_fields);

int flow_special_field_glob_in_lif_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    flow_special_fields_e special_field_id,
    bcm_flow_special_field_t * special_field);

int flow_special_field_rch_destination_set(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    int field_idx,
    bcm_flow_special_fields_t * special_fields);

int flow_special_field_rch_destination_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    flow_special_fields_e special_field_id,
    bcm_flow_special_field_t * special_field);

int flow_special_field_rch_control_vlan_port_set(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    int field_idx,
    bcm_flow_special_fields_t * special_fields);

int flow_special_field_rch_control_vlan_port_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    flow_special_fields_e special_field_id,
    bcm_flow_special_field_t * special_field);

int flow_special_field_rch_type_set(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    int field_idx,
    bcm_flow_special_fields_t * special_fields);

int flow_special_field_rch_type_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    flow_special_fields_e special_field_id,
    bcm_flow_special_field_t * special_field);

int flow_special_field_tunnel_class_set(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    int field_idx,
    bcm_flow_special_fields_t * special_fields);

int flow_special_field_tunnel_class_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    flow_special_fields_e special_field_id,
    bcm_flow_special_field_t * special_field);

int flow_special_field_next_outlif_pointer_set(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    int field_idx,
    bcm_flow_special_fields_t * special_fields);

int flow_special_field_next_outlif_pointer_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    flow_special_fields_e special_field_id,
    bcm_flow_special_field_t * special_field);

int flow_special_field_outlif_match_set(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    int field_idx,
    bcm_flow_special_fields_t * special_fields);

int flow_special_field_outlif_match_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    flow_special_fields_e special_field_id,
    bcm_flow_special_field_t * special_field);

/** GENERAL DEFINITIONS - START */
/** macro for special field definitions when the operation cb are not used */
#define FLOW_S_FIELD_NO_MAPPING_CBS  NULL, NULL, NULL, NULL

/* *INDENT-OFF* */ 
#define FLOW_S_FIELD_DIRECT_MAPPING  FLOW_S_FIELD_UNLINKED,NULL,NULL,FLOW_S_FIELD_NO_INDICATIONS,FLOW_S_FIELD_NO_MAPPING_CBS
/* *INDENT-ON* */ 

#define FLOW_S_FIELD_NO_INDICATIONS               0

#define FLOW_S_FIELD_UNLINKED                     FLOW_S_F_EMPTY

/** used to mark that a special field is not mapped to a specific DBAL field
 *  in ctest ut, unmapped fields are automatically set for all result types unless, a different logic is added at ctest_flow_ut_app_unmapped_special_fields_add */
#define FLOW_S_F_UNMAPPED                         DBAL_FIELD_EMPTY

/** Flow indications defines */
#define FLOW_S_FIELD_CAN_BE_MASKED              SAL_BIT(FLOW_SPECIAL_FIELD_IND_MASK_SUPPORTED)
#define FLOW_S_FIELD_LIF_SHOULD_BE_IGNORED      SAL_BIT(FLOW_SPECIAL_FIELD_IND_LIF_IGNORE_OPERATION)
#define FLOW_S_FIELD_MATCH_SHOULD_BE_IGNORED    SAL_BIT(FLOW_SPECIAL_FIELD_IND_MATCH_IGNORE_OPERATION)
#define FLOW_S_FIELD_SHOULD_BE_IGNORED         (FLOW_S_FIELD_LIF_SHOULD_BE_IGNORED | FLOW_S_FIELD_MATCH_SHOULD_BE_IGNORED)
#define FLOW_S_FIELD_USED_AS_MASK               SAL_BIT(FLOW_SPECIAL_FIELD_IND_IS_MASK)
#define FLOW_S_FIELD_SRC_ADDR_PROFILE_SUPPORT   SAL_BIT(FLOW_SPECIAL_FIELD_IND_SRC_ADDR_PROFILE_SUPPORTED)
#define FLOW_S_FIELD_IS_LIF_MANDATORY           SAL_BIT(FLOW_SPECIAL_FIELD_IND_LIF_MANDATORY)
#define FLOW_S_FIELD_IS_MATCH_MANDATORY         SAL_BIT(FLOW_SPECIAL_FIELD_IND_MATCH_MANDATORY)
#define FLOW_S_FIELD_IS_ALWAYS_MANDATORY       (FLOW_S_FIELD_IS_LIF_MANDATORY | FLOW_S_FIELD_IS_MATCH_MANDATORY)
#define FLOW_S_FIELD_IS_LIF_UNCHANGEABLE        SAL_BIT(FLOW_SPECIAL_FIELD_IND_LIF_UNCHANGEABLE)
#define FLOW_S_FIELD_VIRTUAL_MATCH_KEY          SAL_BIT(FLOW_SPECIAL_FIELD_IND_IS_KEY)

/** external indications are indications that the user suppose to know */
#define FLOW_S_FIELD_IND_IS_EXTERNAL            (FLOW_S_FIELD_CAN_BE_MASKED | \
                                                 FLOW_S_FIELD_IS_LIF_UNCHANGEABLE | \
                                                 FLOW_S_FIELD_IS_ALWAYS_MANDATORY | \
                                                 FLOW_S_FIELD_VIRTUAL_MATCH_KEY)

#define MASK_FIELD_INDICATIONS                  (FLOW_S_FIELD_SHOULD_BE_IGNORED | FLOW_S_FIELD_USED_AS_MASK)
#define IPV4_SIP_INDICATIONS                    (FLOW_S_FIELD_CAN_BE_MASKED | FLOW_S_FIELD_SRC_ADDR_PROFILE_SUPPORT)
#define IPV6_INDICATIONS                        (FLOW_S_FIELD_LIF_SHOULD_BE_IGNORED | FLOW_S_FIELD_IS_LIF_MANDATORY | FLOW_S_FIELD_CAN_BE_MASKED)
#define IPV6_SIP_INDICATIONS                    (FLOW_S_FIELD_MATCH_SHOULD_BE_IGNORED | FLOW_S_FIELD_IS_MATCH_MANDATORY | \
                                                 FLOW_S_FIELD_CAN_BE_MASKED | FLOW_S_FIELD_SRC_ADDR_PROFILE_SUPPORT)
#define IPV6_DIP_INDICATIONS                    (FLOW_S_FIELD_LIF_SHOULD_BE_IGNORED | FLOW_S_FIELD_IS_LIF_MANDATORY | FLOW_S_FIELD_CAN_BE_MASKED)
#define VLAN_PORT_SVTAG_PORT_INDICATIONS        (FLOW_S_FIELD_SHOULD_BE_IGNORED | FLOW_S_FIELD_IS_LIF_MANDATORY)
#define TUNNEL_CLASS_INDICATIONS                (FLOW_S_FIELD_LIF_SHOULD_BE_IGNORED| FLOW_S_FIELD_IS_ALWAYS_MANDATORY | FLOW_S_FIELD_IS_LIF_UNCHANGEABLE)
#define IPV4_TUNNEL_TYPE_INDICATIONS            (FLOW_S_FIELD_LIF_SHOULD_BE_IGNORED | FLOW_S_FIELD_IS_ALWAYS_MANDATORY | \
                                                 FLOW_S_FIELD_IS_LIF_UNCHANGEABLE | FLOW_S_FIELD_MATCH_SHOULD_BE_IGNORED)
#define IPV6_TUNNEL_TYPE_INDICATIONS            (FLOW_S_FIELD_LIF_SHOULD_BE_IGNORED| FLOW_S_FIELD_IS_ALWAYS_MANDATORY | FLOW_S_FIELD_IS_LIF_UNCHANGEABLE)
#define IPVX_TUNNEL_TYPE_INDICATIONS            (FLOW_S_FIELD_LIF_SHOULD_BE_IGNORED | FLOW_S_FIELD_IS_LIF_MANDATORY | FLOW_S_FIELD_IS_LIF_UNCHANGEABLE)
#define MPLS_PORT_VCCV_TYPE_INDICATIONS         (FLOW_S_FIELD_SHOULD_BE_IGNORED | FLOW_S_FIELD_IS_LIF_MANDATORY)
#define VIRTUAL_MATCH_AND_LIF_MANDATORY         (FLOW_S_FIELD_VIRTUAL_MATCH_KEY | FLOW_S_FIELD_IS_LIF_MANDATORY)
/** GENERAL DEFINITIONS - END */

/** SPECIAL FIELDS DESCRIPTIONS - START */
int
flow_special_field_fodo_assignment_mode_print(
    int unit,
    bcm_flow_special_field_t * special_field,
    char print_string[FLOW_STR_MAX])
{
    SHR_FUNC_INIT_VARS(unit);

    switch (special_field->symbol)
    {
        case DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_VLAN:
            sal_snprintf(print_string, FLOW_STR_MAX, " %s (%d) ", "FROM_VLAN", special_field->symbol);
            break;

        case DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LIF:
            sal_snprintf(print_string, FLOW_STR_MAX, " %s (%d) ", "FROM_LIF", special_field->symbol);
            break;

        case DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LOOKUP:
            sal_snprintf(print_string, FLOW_STR_MAX, " %s (%d) ", "FROM_LOOKUP", special_field->symbol);
            break;

        default:
            sal_snprintf(print_string, FLOW_STR_MAX, "invalid mode (%d) ", special_field->symbol);
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            break;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

int
flow_special_field_rch_type_print(
    int unit,
    bcm_flow_special_field_t * special_field,
    char print_string[FLOW_STR_MAX])
{
    SHR_FUNC_INIT_VARS(unit);

    switch (special_field->symbol)
    {
        case bcmL2EgressRecycleAppDropAndContinue:
            sal_snprintf(print_string, FLOW_STR_MAX, " %s (%d) ", "Drop and continue", special_field->symbol);
            break;

        case bcmL2EgressRecycleAppExtendedTerm:
            sal_snprintf(print_string, FLOW_STR_MAX, " %s (%d) ", "Extended Termination", special_field->symbol);
            break;

        case bcmL2EgressRecycleAppExtendedEncap:
            sal_snprintf(print_string, FLOW_STR_MAX, " %s (%d) ", "Extended Encapsulation", special_field->symbol);
            break;

        case bcmL2EgressRecycleAppReflector:
            sal_snprintf(print_string, FLOW_STR_MAX, " %s (%d) ", "Reflector", special_field->symbol);
            break;

        case bcmL2EgressRecycleAppRedirectVrf:
            sal_snprintf(print_string, FLOW_STR_MAX, " %s (%d) ", "Redirect vrf", special_field->symbol);
            break;

        default:
            sal_snprintf(print_string, FLOW_STR_MAX, "invalid tunnel type (%d) ", special_field->symbol);
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            break;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

int
flow_special_field_ipv4_tunnel_print(
    int unit,
    bcm_flow_special_field_t * special_field,
    char print_string[FLOW_STR_MAX])
{
    SHR_FUNC_INIT_VARS(unit);

    switch (special_field->symbol)
    {
        case bcmTunnelTypeIpAnyIn4:
            sal_snprintf(print_string, FLOW_STR_MAX, " %s (%d) ", "Ip Any 4", special_field->symbol);
            break;

        case bcmTunnelTypeGreAnyIn4:
            sal_snprintf(print_string, FLOW_STR_MAX, " %s (%d) ", "Gre Any 4", special_field->symbol);
            break;

        case bcmTunnelTypeVxlan:
            sal_snprintf(print_string, FLOW_STR_MAX, " %s (%d) ", "Vxlan", special_field->symbol);
            break;

        case bcmTunnelTypeVxlanGpe:
            sal_snprintf(print_string, FLOW_STR_MAX, " %s (%d) ", "Vxlan Gpe", special_field->symbol);
            break;

        case bcmTunnelTypeUdp:
            sal_snprintf(print_string, FLOW_STR_MAX, " %s (%d) ", "Udp", special_field->symbol);
            break;

        default:
            sal_snprintf(print_string, FLOW_STR_MAX, "invalid tunnel type (%d) ", special_field->symbol);
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            break;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

int
flow_special_field_ipv6_tunnel_print(
    int unit,
    bcm_flow_special_field_t * special_field,
    char print_string[FLOW_STR_MAX])
{
    SHR_FUNC_INIT_VARS(unit);

    switch (special_field->symbol)
    {
        case bcmTunnelTypeIpAnyIn6:
            sal_snprintf(print_string, FLOW_STR_MAX, " %s (%d) ", "Ip Any 6", special_field->symbol);
            break;

        case bcmTunnelTypeGreAnyIn6:
            sal_snprintf(print_string, FLOW_STR_MAX, " %s (%d) ", "Gre Any 6", special_field->symbol);
            break;

        case bcmTunnelTypeVxlan6:
            sal_snprintf(print_string, FLOW_STR_MAX, " %s (%d) ", "Vxlan6", special_field->symbol);
            break;

        case bcmTunnelTypeVxlan6Gpe:
            sal_snprintf(print_string, FLOW_STR_MAX, " %s (%d) ", "Vxlan6 Gpe", special_field->symbol);
            break;

        case bcmTunnelTypeUdp6:
            sal_snprintf(print_string, FLOW_STR_MAX, " %s (%d) ", "Udp6", special_field->symbol);
            break;

        case bcmTunnelTypeEsp:
            sal_snprintf(print_string, FLOW_STR_MAX, " %s (%d) ", "IPSEC ESP", special_field->symbol);
            break;

        default:
            sal_snprintf(print_string, FLOW_STR_MAX, "invalid tunnel type (%d) ", special_field->symbol);
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            break;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

int
flow_special_field_ipvx_tunnel_print(
    int unit,
    bcm_flow_special_field_t * special_field,
    char print_string[FLOW_STR_MAX])
{
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_tunnel_terminator_tunnel_type_is_ipv4(special_field->symbol))
    {
        SHR_IF_ERR_EXIT_WITH_MSG_EXCEPT_IF(flow_special_field_ipv4_tunnel_print(unit, special_field, print_string),
                                           _SHR_E_NOT_FOUND);
    }
    else
    {
        SHR_IF_ERR_EXIT_WITH_MSG_EXCEPT_IF(flow_special_field_ipv6_tunnel_print(unit, special_field, print_string),
                                           _SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT;
}

int
flow_special_field_mpls_port_vccv_type_print(
    int unit,
    bcm_flow_special_field_t * special_field,
    char print_string[FLOW_STR_MAX])
{
    SHR_FUNC_INIT_VARS(unit);
    switch (special_field->symbol)
    {
        case bcmMplsPortControlChannelNone:
            sal_snprintf(print_string, FLOW_STR_MAX, " %s (%d) ", "None", special_field->symbol);
            break;

        case bcmMplsPortControlChannelAch:
            sal_snprintf(print_string, FLOW_STR_MAX, " %s (%d) ", "Ach", special_field->symbol);
            break;

        case bcmMplsPortControlChannelRouterAlert:
            sal_snprintf(print_string, FLOW_STR_MAX, " %s (%d) ", "Router Alert", special_field->symbol);
            break;

        case bcmMplsPortControlChannelTtl:
            sal_snprintf(print_string, FLOW_STR_MAX, " %s (%d) ", "Ttl", special_field->symbol);
            break;

        case bcmMplsPortControlChannelGalUnderPw:
            sal_snprintf(print_string, FLOW_STR_MAX, " %s (%d) ", "GalUnderPw", special_field->symbol);
            break;
        default:
            sal_snprintf(print_string, FLOW_STR_MAX, "invalid mpls port control channel (%d) ", special_field->symbol);
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            break;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
/** SPECIAL FIELDS DESCRIPTIONS - END */

/** SPECIAL FIELDS DEFINITIONS - START */

/* *INDENT-OFF* */

/*
 * DB for all existing special fields, in order to add a new field, user should also update the
 * flow_special_fields_e enum in the same order. In addition, when adding new special field,
 * need to update ctest_flow_ut_special_field_legal_value_get with proper value for the special field
 */

    /** field_name */             /** mapped_dbal_field */                   /** payload_type */             /** linked_special_field */  /** description cb */                 /** print cb */      /** field indications */               /** cb for operations */
const flow_special_field_info_t flow_special_fields_db[NOF_FLOW_SPECIAL_FIELDS+1] = {
    /** The empty must be the first */
    {"",                            FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},

    {"IPV4_DIP",                    DBAL_FIELD_IPV4_DIP,                    FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_F_IPV4_DIP_MASK,    flow_special_field_ipv4_dip_desc,            NULL,      FLOW_S_FIELD_CAN_BE_MASKED,              FLOW_S_FIELD_NO_MAPPING_CBS},
    {"IPV4_DIP_MASK",               DBAL_FIELD_DST_IPV4_MASK,               FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     flow_special_field_ipv4_dip_mask_desc,       NULL,      MASK_FIELD_INDICATIONS,                  FLOW_S_FIELD_NO_MAPPING_CBS},
    {"IPV4_SIP",                    DBAL_FIELD_IPV4_SIP,                    FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_F_IPV4_SIP_MASK,    flow_special_field_ipv4_sip_desc,            NULL,      IPV4_SIP_INDICATIONS,                    FLOW_S_FIELD_NO_MAPPING_CBS},
    {"IPV4_SIP_MASK",               DBAL_FIELD_SRC_IPV4_MASK,               FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     flow_special_field_ipv4_sip_mask_desc,       NULL,      MASK_FIELD_INDICATIONS,                  FLOW_S_FIELD_NO_MAPPING_CBS},
    {"IPV6_DIP",                    DBAL_FIELD_IPV6_DIP,                    FLOW_PAYLOAD_TYPE_UINT8_ARR,        FLOW_S_F_IPV6_DIP_MASK,    flow_special_field_ipv6_dip_desc,            NULL,      IPV6_DIP_INDICATIONS,                    FLOW_S_FIELD_NO_MAPPING_CBS},
    {"IPV6_DIP_MASK",               DBAL_FIELD_DST_IPV6_MASK,               FLOW_PAYLOAD_TYPE_UINT8_ARR,        FLOW_S_FIELD_UNLINKED,     flow_special_field_ipv6_dip_mask_desc,       NULL,      MASK_FIELD_INDICATIONS,                  FLOW_S_FIELD_NO_MAPPING_CBS},
    {"IPV6_SIP",                    DBAL_FIELD_IPV6_SIP,                    FLOW_PAYLOAD_TYPE_UINT8_ARR,        FLOW_S_F_IPV6_SIP_MASK,    flow_special_field_ipv6_sip_desc,            NULL,      IPV6_SIP_INDICATIONS,                    FLOW_S_FIELD_NO_MAPPING_CBS},
    {"IPV6_SIP_MASK",               DBAL_FIELD_SRC_IPV6_MASK,               FLOW_PAYLOAD_TYPE_UINT8_ARR,        FLOW_S_FIELD_UNLINKED,     flow_special_field_ipv6_sip_mask_desc,       NULL,      MASK_FIELD_INDICATIONS,                  FLOW_S_FIELD_NO_MAPPING_CBS},
    {"IPV6",                        DBAL_FIELD_IPV6,                        FLOW_PAYLOAD_TYPE_UINT8_ARR,        FLOW_S_F_IPV6_MASK,        NULL,                                        NULL,      IPV6_INDICATIONS,                        FLOW_S_FIELD_NO_MAPPING_CBS},
    {"IPV6_MASK",                   DBAL_FIELD_DST_IPV6_MASK,               FLOW_PAYLOAD_TYPE_UINT8_ARR,        FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      MASK_FIELD_INDICATIONS,                  FLOW_S_FIELD_NO_MAPPING_CBS},
    {"SRC_MAC_ADDRESS",             DBAL_FIELD_SOURCE_ADDRESS,              FLOW_PAYLOAD_TYPE_UINT8_ARR,        FLOW_S_FIELD_UNLINKED,     flow_special_field_src_mac_address_desc,     NULL,      FLOW_S_FIELD_SRC_ADDR_PROFILE_SUPPORT,   FLOW_S_FIELD_NO_MAPPING_CBS},
    {"DST_MAC_ADDRESS",             DBAL_FIELD_L2_MAC,                      FLOW_PAYLOAD_TYPE_UINT8_ARR,        FLOW_S_FIELD_UNLINKED,     flow_special_field_dst_mac_address_desc,     NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},


    {"TUNNEL_ENDPOINT_IDENTIFIER",  DBAL_FIELD_TEID,                        FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
    {"NEXT_LAYER_NETWORK_DOMAIN",   DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN,   FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
    {"MPLS_PHP",                    FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_ENABLER,          FLOW_S_FIELD_UNLINKED,     flow_special_field_mpls_php_desc,            NULL,      FLOW_S_FIELD_SHOULD_BE_IGNORED,          FLOW_S_FIELD_NO_MAPPING_CBS},
    {"MPLS_LABEL",                  DBAL_FIELD_MPLS_LABEL,                  FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     flow_special_field_mpls_label_desc,          NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
    {"MPLS_LABEL_2",                DBAL_FIELD_MPLS_LABEL_2,                FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     flow_special_field_mpls_label_2_desc,        NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
    {"MPLS_LABEL_3",                FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     flow_special_field_mpls_label_3_desc,        NULL,      FLOW_S_FIELD_SHOULD_BE_IGNORED,          FLOW_S_FIELD_NO_MAPPING_CBS},
    {"MPLS_LABEL_4",                FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     flow_special_field_mpls_label_4_desc,        NULL,      FLOW_S_FIELD_SHOULD_BE_IGNORED,          FLOW_S_FIELD_NO_MAPPING_CBS},
    {"MPLS_LABEL_5",                FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     flow_special_field_mpls_label_5_desc,        NULL,      FLOW_S_FIELD_SHOULD_BE_IGNORED,          FLOW_S_FIELD_NO_MAPPING_CBS},
    {"MPLS_LABEL_6",                FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     flow_special_field_mpls_label_6_desc,        NULL,      FLOW_S_FIELD_SHOULD_BE_IGNORED,          FLOW_S_FIELD_NO_MAPPING_CBS},
    {"MPLS_LABEL_7",                FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     flow_special_field_mpls_label_7_desc,        NULL,      FLOW_S_FIELD_SHOULD_BE_IGNORED,          FLOW_S_FIELD_NO_MAPPING_CBS},
    {"MPLS_LABEL_8",                FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     flow_special_field_mpls_label_8_desc,        NULL,      FLOW_S_FIELD_SHOULD_BE_IGNORED,          FLOW_S_FIELD_NO_MAPPING_CBS},

    {"VRF",                         DBAL_FIELD_VRF,                         FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     flow_special_field_vrf_desc,                 NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},

    {"SRC_UDP_PORT",                DBAL_FIELD_LIF_AHP_UDP,                 FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     flow_special_field_udp_src_desc,             NULL,      FLOW_S_FIELD_SHOULD_BE_IGNORED,          FLOW_S_FIELD_NO_MAPPING_CBS},
    {"DST_UDP_PORT",                DBAL_FIELD_LIF_AHP_UDP,                 FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     flow_special_field_udp_dst_desc,             NULL,      FLOW_S_FIELD_SHOULD_BE_IGNORED,          FLOW_S_FIELD_NO_MAPPING_CBS},
    {"IPV4_HEADER_DF",              DBAL_FIELD_IPV4_HEADER_DF_FLAG,         FLOW_PAYLOAD_TYPE_ENABLER,          FLOW_S_FIELD_UNLINKED,     flow_special_field_hdr_df_desc,              NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
    {"IPVX_INIT_GRE_KEY_USE_LB",    FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_ENABLER,          FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_SHOULD_BE_IGNORED,          FLOW_S_FIELD_NO_MAPPING_CBS},
    {"IPVX_INIT_GRE_WITH_SN",       FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_ENABLER,          FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_SHOULD_BE_IGNORED,          FLOW_S_FIELD_NO_MAPPING_CBS},

    {"SVTAG_TXSCI",                 DBAL_FIELD_SVTAG_TXSCI,                 FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},

    {"VLAN_EDIT_PROFILE",           DBAL_FIELD_VLAN_EDIT_PROFILE,           FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
    {"VLAN_EDIT_PCP_DEI_PROFILE",   DBAL_FIELD_VLAN_EDIT_PCP_DEI_PROFILE,   FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     flow_special_field_pcp_dei_porf_desc,        NULL,      FLOW_S_FIELD_NO_INDICATIONS,             flow_special_field_pcp_dei_profile_set, flow_special_field_pcp_dei_profile_get, NULL, NULL},
    {"VLAN_EDIT_VID_1",             DBAL_FIELD_VLAN_EDIT_VID_1,             FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
    {"VLAN_EDIT_VID_2",             DBAL_FIELD_VLAN_EDIT_VID_2,             FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
    {"VSI_ASSIGNMENT_MODE",         DBAL_FIELD_FODO_ASSIGNMENT_MODE,        FLOW_PAYLOAD_TYPE_ENUM,             FLOW_S_FIELD_UNLINKED,     flow_special_field_fodo_assignment_mode_desc, flow_special_field_fodo_assignment_mode_print, FLOW_S_FIELD_NO_INDICATIONS,     FLOW_S_FIELD_NO_MAPPING_CBS},
    {"PORT",                        DBAL_FIELD_PP_PORT,                     FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_MATCH_SHOULD_BE_IGNORED,    FLOW_S_FIELD_NO_MAPPING_CBS},
    {"IN_PORT",                     DBAL_FIELD_IN_PP_PORT,                  FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_MATCH_SHOULD_BE_IGNORED,    FLOW_S_FIELD_NO_MAPPING_CBS},
    {"MAPPED_PORT",                 DBAL_FIELD_MAPPED_PP_PORT,              FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_MATCH_SHOULD_BE_IGNORED,    FLOW_S_FIELD_NO_MAPPING_CBS},
    {"VLAN_DOMAIN",                 DBAL_FIELD_VLAN_DOMAIN,                 FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
    {"GLOB_IN_LIF",                 DBAL_FIELD_GLOB_IN_LIF,                 FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_NO_INDICATIONS,             flow_special_field_glob_in_lif_set, flow_special_field_glob_in_lif_get, NULL, NULL},
    {"VLAN_PORT_SVTAG_PORT",        FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_ENABLER,          FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      VLAN_PORT_SVTAG_PORT_INDICATIONS,        FLOW_S_FIELD_NO_MAPPING_CBS},

    {"VID",                         DBAL_FIELD_VID,                         FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
    {"S_VID",                       DBAL_FIELD_S_VID,                       FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
    {"C_VID",                       DBAL_FIELD_C_VID,                       FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
    {"S_VID_1",                     DBAL_FIELD_S_VID_1,                     FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
    {"S_VID_2",                     DBAL_FIELD_S_VID_2,                     FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
    {"C_VID_1",                     DBAL_FIELD_C_VID_1,                     FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
    {"C_VID_2",                     DBAL_FIELD_C_VID_2,                     FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
    {"VSI",                         DBAL_FIELD_VSI,                         FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     flow_special_field_vsi_desc,                 NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
    {"UNKNOWN_UNICAST_DEFAULT_DESTINATION",  DBAL_FIELD_IN_LIF_PROFILE,     FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_SHOULD_BE_IGNORED,          FLOW_S_FIELD_NO_MAPPING_CBS},
    {"UNKNOWN_MULTICAST_DEFAULT_DESTINATION",DBAL_FIELD_IN_LIF_PROFILE,     FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_SHOULD_BE_IGNORED,          FLOW_S_FIELD_NO_MAPPING_CBS},
    {"BROADCAST_DEFAULT_DESTINATION",        DBAL_FIELD_IN_LIF_PROFILE,     FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_SHOULD_BE_IGNORED,          FLOW_S_FIELD_NO_MAPPING_CBS},

    {"VID_OUTER_VLAN",              DBAL_FIELD_VID_OUTER_VLAN,              FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
    {"VID_INNER_VLAN",              DBAL_FIELD_VID_INNER_VLAN,              FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
    {"PCP_DEI_OUTER_VLAN",          DBAL_FIELD_PCP_DEI_OUTER_VLAN,          FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
    {"TUNNEL_CLASS",                FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      TUNNEL_CLASS_INDICATIONS,                flow_special_field_tunnel_class_set, flow_special_field_tunnel_class_get, NULL, NULL},

    {"QOS_DSCP",                    FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     flow_special_field_qos_desc,                 NULL,      FLOW_S_FIELD_SHOULD_BE_IGNORED,          FLOW_S_FIELD_NO_MAPPING_CBS},
    {"QOS_EXP",                     FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     flow_special_field_qos_desc,                 NULL,      FLOW_S_FIELD_SHOULD_BE_IGNORED,          FLOW_S_FIELD_NO_MAPPING_CBS},
    {"QOS_EXP_1",                   FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     flow_special_field_qos_desc,                 NULL,      FLOW_S_FIELD_SHOULD_BE_IGNORED,          FLOW_S_FIELD_NO_MAPPING_CBS},
    {"QOS_PRI",                     FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     flow_special_field_qos_desc,                 NULL,      FLOW_S_FIELD_SHOULD_BE_IGNORED,          FLOW_S_FIELD_NO_MAPPING_CBS},
    {"QOS_CFI",                     FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     flow_special_field_qos_desc,                 NULL,      FLOW_S_FIELD_SHOULD_BE_IGNORED,          FLOW_S_FIELD_NO_MAPPING_CBS},
    {"QOS_PCP",                     FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     flow_special_field_qos_pcp_desc,             NULL,      FLOW_S_FIELD_SHOULD_BE_IGNORED,          FLOW_S_FIELD_NO_MAPPING_CBS},

    {"BFR_BIT_STR",                 DBAL_FIELD_BFR_BIT_STR,                 FLOW_PAYLOAD_TYPE_UINT32_ARR,       FLOW_S_FIELD_UNLINKED,     flow_special_field_bier_desc,                NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
    {"BIER_BIFT_ID",                DBAL_FIELD_BIER_BIFT_ID,                FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     flow_special_field_bier_desc,                NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
    {"BIER_BSL",                    DBAL_FIELD_BIER_BSL,                    FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     flow_special_field_bier_desc,                NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
    {"BIER_RSV",                    DBAL_FIELD_BIER_RSV,                    FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     flow_special_field_bier_desc,                NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
    {"BIER_OAM",                    DBAL_FIELD_BIER_OAM,                    FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     flow_special_field_bier_desc,                NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
    {"BIER_BFIR_ID",                DBAL_FIELD_BIER_BFIR_ID,                FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     flow_special_field_bier_desc,                NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},

    {"IPV4_TUNNEL_TYPE",            DBAL_FIELD_IPV4_TUNNEL_TYPE,            FLOW_PAYLOAD_TYPE_BCM_ENUM,         FLOW_S_FIELD_UNLINKED,     flow_special_field_ipv4_tunnel_desc,         flow_special_field_ipv4_tunnel_print, IPV4_TUNNEL_TYPE_INDICATIONS, FLOW_S_FIELD_NO_MAPPING_CBS},
    {"IPV6_TUNNEL_TYPE",  DBAL_FIELD_IPV6_QUALIFIER_ADDITIONAL_HEADER_1ST,  FLOW_PAYLOAD_TYPE_BCM_ENUM,         FLOW_S_FIELD_UNLINKED,     flow_special_field_ipv6_tunnel_desc,         flow_special_field_ipv6_tunnel_print, IPV6_TUNNEL_TYPE_INDICATIONS, flow_special_field_ipv6_tunnel_type_set, flow_special_field_ipv6_tunnel_type_get, NULL, NULL},
    {"IPVX_TUNNEL_TYPE",            FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_BCM_ENUM,         FLOW_S_FIELD_UNLINKED,     flow_special_field_ipvx_tunnel_desc,         flow_special_field_ipvx_tunnel_print, IPVX_TUNNEL_TYPE_INDICATIONS, FLOW_S_FIELD_NO_MAPPING_CBS},

    /** LAYER TYPE should changed to ENUM */
    {"LAYER_TYPE",                  DBAL_FIELD_LAYER_TYPE,                  FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
    {"MPLS_ENTROPY_LABEL",              FLOW_S_F_UNMAPPED,                  FLOW_PAYLOAD_TYPE_ENABLER,          FLOW_S_FIELD_UNLINKED,     flow_special_field_mpls_entropy_label_desc,  NULL,      FLOW_S_FIELD_SHOULD_BE_IGNORED,          FLOW_S_FIELD_NO_MAPPING_CBS},
    {"MPLS_ENTROPY_LABEL_INDICATION",   FLOW_S_F_UNMAPPED,                  FLOW_PAYLOAD_TYPE_ENABLER,          FLOW_S_FIELD_UNLINKED,     flow_special_field_mpls_entropy_label_indication_desc,  NULL,      FLOW_S_FIELD_SHOULD_BE_IGNORED,          FLOW_S_FIELD_NO_MAPPING_CBS},
    {"MPLS_CONTROL_WORD_ENABLE",        FLOW_S_F_UNMAPPED,                  FLOW_PAYLOAD_TYPE_ENABLER,          FLOW_S_FIELD_UNLINKED,     flow_special_field_mpls_control_word_enable_desc,       NULL,      FLOW_S_FIELD_SHOULD_BE_IGNORED,          FLOW_S_FIELD_NO_MAPPING_CBS},
    {"MPLS_PLATFORM_NAMESPACE",         FLOW_S_F_UNMAPPED,                  FLOW_PAYLOAD_TYPE_ENABLER,          FLOW_S_FIELD_UNLINKED,     flow_special_field_mpls_platform_namespace_desc,        NULL,      FLOW_S_FIELD_SHOULD_BE_IGNORED,          FLOW_S_FIELD_NO_MAPPING_CBS},
    {"MPLS_INCLUSIVE_MULTICAST_LABEL",  FLOW_S_F_UNMAPPED,                  FLOW_PAYLOAD_TYPE_ENABLER,          FLOW_S_FIELD_UNLINKED,     flow_special_field_mpls_inclusive_multicast_label_desc, NULL,      FLOW_S_FIELD_SHOULD_BE_IGNORED,          FLOW_S_FIELD_NO_MAPPING_CBS},
    {"MPLS_NOF_SERVICE_TAGS",           FLOW_S_F_UNMAPPED,                  FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     flow_special_field_mpls_nof_service_tags_desc,          NULL,      FLOW_S_FIELD_SHOULD_BE_IGNORED,          FLOW_S_FIELD_NO_MAPPING_CBS},

    {"MPLS_PORT_VCCV_TYPE",         DBAL_FIELD_TERMINATION_TYPE,            FLOW_PAYLOAD_TYPE_BCM_ENUM,         FLOW_S_FIELD_UNLINKED,     flow_special_field_mpls_port_vccv_type_desc, flow_special_field_mpls_port_vccv_type_print, MPLS_PORT_VCCV_TYPE_INDICATIONS,  FLOW_S_FIELD_NO_MAPPING_CBS},
    {"MPLS_EXPECT_BOS",               FLOW_S_F_UNMAPPED,                    FLOW_PAYLOAD_TYPE_ENABLER,          FLOW_S_FIELD_UNLINKED,     flow_special_field_mpls_expect_bos_desc,     NULL,      FLOW_S_FIELD_SHOULD_BE_IGNORED,          FLOW_S_FIELD_NO_MAPPING_CBS},
    {"MPLS_EXPECT_NON_BOS",           FLOW_S_F_UNMAPPED,                    FLOW_PAYLOAD_TYPE_ENABLER,          FLOW_S_FIELD_UNLINKED,     flow_special_field_mpls_expect_non_bos_desc, NULL,      FLOW_S_FIELD_SHOULD_BE_IGNORED,          FLOW_S_FIELD_NO_MAPPING_CBS},
    {"MPLS_TRAP_TTL_0",               FLOW_S_F_UNMAPPED,                    FLOW_PAYLOAD_TYPE_ENABLER,          FLOW_S_FIELD_UNLINKED,     flow_special_field_mpls_trap_ttl_0_desc,     NULL,      FLOW_S_FIELD_SHOULD_BE_IGNORED,          FLOW_S_FIELD_NO_MAPPING_CBS},
    {"MPLS_TRAP_TTL_1",               FLOW_S_F_UNMAPPED,                    FLOW_PAYLOAD_TYPE_ENABLER,          FLOW_S_FIELD_UNLINKED,     flow_special_field_mpls_trap_ttl_1_desc,     NULL,      FLOW_S_FIELD_SHOULD_BE_IGNORED,          FLOW_S_FIELD_NO_MAPPING_CBS},
    {"MATCH_LABEL",                 DBAL_FIELD_MPLS_LABEL,                  FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     flow_special_field_match_label_desc,                                        NULL,      FLOW_S_FIELD_IS_MATCH_MANDATORY,         FLOW_S_FIELD_NO_MAPPING_CBS},
    {"VSI_MATCH",                   DBAL_FIELD_VSI,                         FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      VIRTUAL_MATCH_AND_LIF_MANDATORY,         FLOW_S_FIELD_NO_MAPPING_CBS},
    {"CLASS_ID_MATCH",              DBAL_FIELD_VLAN_DOMAIN,                 FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      VIRTUAL_MATCH_AND_LIF_MANDATORY,         FLOW_S_FIELD_NO_MAPPING_CBS},
    {"NAME_SPACE_MATCH",            DBAL_FIELD_ESEM_NAME_SPACE,             FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      VIRTUAL_MATCH_AND_LIF_MANDATORY,         FLOW_S_FIELD_NO_MAPPING_CBS},
    {"OUT_LIF_MATCH",               DBAL_FIELD_OUT_LIF,                     FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      VIRTUAL_MATCH_AND_LIF_MANDATORY,         flow_special_field_outlif_match_set, flow_special_field_outlif_match_get, NULL, NULL},
    {"VID_MATCH",                   DBAL_FIELD_C_VID,                       FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      VIRTUAL_MATCH_AND_LIF_MANDATORY,         FLOW_S_FIELD_NO_MAPPING_CBS},
    {"SYSTEM_PORT_MATCH",           DBAL_FIELD_SYSTEM_PORT_AGGREGATE,       FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      VIRTUAL_MATCH_AND_LIF_MANDATORY,         FLOW_S_FIELD_NO_MAPPING_CBS},

    {"RCH_VRF",                     DBAL_FIELD_VRF,                         FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
    {"RCH_OUTLIF_0_PUSHED_TO_STACK",DBAL_FIELD_OUTLIF_0_PUSHED_TO_STACK,    FLOW_PAYLOAD_TYPE_ENABLER,          FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
    {"RCH_P2P",                     DBAL_FIELD_SERVICE_TYPE_IS_P2P,         FLOW_PAYLOAD_TYPE_ENABLER,          FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
    {"RCH_DESTINATION",             DBAL_FIELD_DESTINATION,                 FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_NO_INDICATIONS,             flow_special_field_rch_destination_set, flow_special_field_rch_destination_get, NULL, NULL},
    {"RCH_ADDITIONAL_BYTES_TO_STRIP", DBAL_FIELD_BTS_ADDITIONAL_TERMINATION_SIZE, FLOW_PAYLOAD_TYPE_UINT32,     FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
    {"RCH_CONTROL_VLAN_PORT",       DBAL_FIELD_CONTROL_LIF,                 FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_NO_INDICATIONS,             flow_special_field_rch_control_vlan_port_set, flow_special_field_rch_control_vlan_port_get, NULL, NULL},

    {"REFLECTOR_PROCESS_TYPE",      DBAL_FIELD_RAW_DATA,                    FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
    {"REFLECTOR_SRC_MAC",           DBAL_FIELD_L2_MAC,                      FLOW_PAYLOAD_TYPE_UINT8_ARR,        FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
    {"REFLECTOR_EGRESS_LAST_LAYER", DBAL_FIELD_EGRESS_LAST_LAYER,           FLOW_PAYLOAD_TYPE_ENABLER,          FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},

    {"GTP_EXT_HEADER_TYPE",         DBAL_FIELD_GTP_EXT_HEADER_TYPE,         FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     flow_special_field_gtp_ext_header_type_desc, NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
    {"GTP_EXT_SSN_TYPE",            DBAL_FIELD_GTP_EXT_SSN_TYPE,            FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     flow_special_field_gtp_ext_ssn_type_desc,    NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
    {"GTP_EXT_SSN_RQI",             DBAL_FIELD_GTP_EXT_SSN_RQI,             FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     flow_special_field_gtp_ssn_rqi_desc,         NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
    {"NEXT_OUTLIF_POINTER",         DBAL_FIELD_NEXT_OUTLIF_POINTER,         FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_NO_INDICATIONS,             flow_special_field_next_outlif_pointer_set, flow_special_field_next_outlif_pointer_get, NULL, NULL},
    {"NATIVE_INDEX_MODE",           FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_SHOULD_BE_IGNORED,          FLOW_S_FIELD_NO_MAPPING_CBS},
    {"OAM_REFLECTOR_RAW_DATA",      DBAL_FIELD_RAW_DATA,                    FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
    {"VNI",                         DBAL_FIELD_VNI,                         FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     flow_special_field_vni_desc,                 NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
    /** The INVALID must be the last entry */
    {"LAST",                        FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,     NULL,                                        NULL,      FLOW_S_FIELD_NO_INDICATIONS,             FLOW_S_FIELD_NO_MAPPING_CBS},
};
/* *INDENT-ON* */

shr_error_e
flow_special_field_info_get(
    int unit,
    flow_special_fields_e field_id,
    const flow_special_field_info_t ** special_field)
{
    SHR_FUNC_INIT_VARS(unit);

    if (field_id > NOF_FLOW_SPECIAL_FIELDS)
    {
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }

    (*special_field) = &(flow_special_fields_db[field_id]);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flow_special_field_value_print(
    int unit,
    bcm_flow_special_field_t * special_field,
    char print_string[FLOW_STR_MAX])
{
    const flow_special_field_info_t *special_field_info;
    uint32 char_count;
    uint8 idx;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(flow_special_field_info_get(unit, special_field->field_id, &special_field_info));
    switch (special_field_info->payload_type)
    {
        case FLOW_PAYLOAD_TYPE_UINT32:
            sal_snprintf(print_string, FLOW_STR_MAX, "0x%x", special_field->shr_var_uint32);
            break;
        case FLOW_PAYLOAD_TYPE_ENUM:
        case FLOW_PAYLOAD_TYPE_BCM_ENUM:
            SHR_IF_ERR_EXIT(special_field_info->print(unit, special_field, print_string));
            break;
        case FLOW_PAYLOAD_TYPE_UINT32_ARR:
            char_count = sal_snprintf(print_string, FLOW_STR_MAX, "0x");
            for (idx = 0; idx < BCM_FLOW_SPECIAL_FIELD_UIN32_ARR_MAX_SIZE; idx++)
            {
                print_string += char_count;
                char_count = sal_snprintf(print_string, FLOW_STR_MAX, "%8x", special_field->shr_var_uint32_arr[idx]);

            }
            break;
        case FLOW_PAYLOAD_TYPE_UINT8_ARR:
            char_count = sal_snprintf(print_string, FLOW_STR_MAX, "0x");
            for (idx = 0; idx < BCM_FLOW_SPECIAL_FIELD_UIN8_ARR_MAX_SIZE; idx++)
            {
                print_string += char_count;
                char_count = sal_snprintf(print_string, FLOW_STR_MAX, "%2x", special_field->shr_var_uint8_arr[idx]);
            }
            break;
        case FLOW_PAYLOAD_TYPE_ENABLER:
            sal_snprintf(print_string, FLOW_STR_MAX, "Enabled");
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid special field payload type (%d)\n", special_field_info->payload_type);
            break;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Search for a special field. Return its index.
 */
shr_error_e
dnx_flow_special_field_find(
    int unit,
    bcm_flow_special_fields_t * special_fields,
    flow_special_fields_e searched_special_field_id,
    int *special_field_index)
{
    uint8 field_idx;
    SHR_FUNC_INIT_VARS(unit);

    for (field_idx = 0; field_idx < special_fields->actual_nof_special_fields; field_idx++)
    {
        if (special_fields->special_fields[field_idx].field_id == searched_special_field_id)
        {
            *special_field_index = field_idx;
            SHR_EXIT();
        }
    }
    SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Search for a special field. Return its index.
 */
shr_error_e
dnx_flow_special_field_find_in_flow_app(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    flow_special_fields_e searched_special_field_id,
    int *special_field_index)
{
    int field_idx = 0;
    SHR_FUNC_INIT_VARS(unit);

    while (flow_app_info->special_fields[field_idx] != FLOW_S_F_EMPTY)
    {
        if (flow_app_info->special_fields[field_idx] == searched_special_field_id)
        {
            *special_field_index = field_idx;
            SHR_EXIT();
        }
        field_idx++;
    }
    SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/** this API removes all the unchangeble fields from the special_fields/ */
shr_error_e
dnx_flow_special_fields_unchangeable_fields_remove(
    int unit,
    bcm_flow_special_fields_t * special_fields)
{
    bcm_flow_special_fields_t special_fields_copy;
    int field_idx;
    const flow_special_field_info_t *curr_field;

    SHR_FUNC_INIT_VARS(unit);

    sal_memcpy(&special_fields_copy, special_fields, sizeof(special_fields_copy));
    sal_memset(special_fields, 0, sizeof(bcm_flow_special_fields_t));

    for (field_idx = 0; field_idx < special_fields_copy.actual_nof_special_fields; field_idx++)
    {
        SHR_IF_ERR_EXIT(flow_special_field_info_get
                        (unit, special_fields_copy.special_fields[field_idx].field_id, &curr_field));

        if (dnx_flow_special_field_is_ind_set(unit, curr_field, FLOW_SPECIAL_FIELD_IND_LIF_UNCHANGEABLE))
        {
            continue;
        }
        sal_memcpy(&special_fields->special_fields[special_fields->actual_nof_special_fields],
                   &special_fields_copy.special_fields[field_idx], sizeof(bcm_flow_special_field_t));
        special_fields->actual_nof_special_fields++;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_special_field_remove_verify(
    int unit,
    bcm_flow_special_fields_t * special_fields)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(special_fields, _SHR_E_PARAM, "special_fields");
    if (special_fields->actual_nof_special_fields == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "expect to have at least one special field");
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_special_field_remove(
    int unit,
    bcm_flow_special_fields_t * special_fields,
    flow_special_fields_e searched_special_field_id)
{
    int last_special_field_index = special_fields->actual_nof_special_fields - 1;
    bcm_flow_special_field_t *last_special_field_p = NULL;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_flow_special_field_remove_verify(unit, special_fields));
    last_special_field_p = &(special_fields->special_fields[last_special_field_index]);

    /*
     * copy the last special field to the special field that we want to clear 
     */
    if (special_fields->actual_nof_special_fields > 1)
    {
        int special_field_to_remove_index = 0;
        bcm_flow_special_field_t *special_field_to_remove_p = NULL;
        SHR_IF_ERR_EXIT(dnx_flow_special_field_find
                        (unit, special_fields, searched_special_field_id, &special_field_to_remove_index));
        if (last_special_field_index != special_field_to_remove_index)
        {
            special_field_to_remove_p = &(special_fields->special_fields[special_field_to_remove_index]);
            sal_memcpy(special_field_to_remove_p, last_special_field_p, sizeof(bcm_flow_special_field_t));
        }
    }
    sal_memset(last_special_field_p, 0, sizeof(bcm_flow_special_field_t));
    special_fields->actual_nof_special_fields--;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * get special field id and returns its value in the special_field struct
 */
shr_error_e
dnx_flow_special_field_data_get(
    int unit,
    bcm_flow_special_fields_t * special_fields,
    bcm_flow_field_id_t field_id,
    bcm_flow_special_field_t * special_field_data)
{
    int idx;
    const flow_special_field_info_t *special_field_info;

    SHR_FUNC_INIT_VARS(unit);

    if (special_fields == NULL)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    FLOW_FIELD_GET_ERR_EXIT(dnx_flow_special_field_find(unit, special_fields, field_id, &idx));

    special_field_data->is_clear = special_fields->special_fields[idx].is_clear;
    SHR_IF_ERR_EXIT(flow_special_field_info_get(unit, field_id, &special_field_info));

    if (special_field_info->payload_type == FLOW_PAYLOAD_TYPE_UINT32_ARR)
    {
        sal_memcpy(special_field_data->shr_var_uint32_arr,
                   special_fields->special_fields[idx].shr_var_uint32_arr,
                   sizeof(special_fields->special_fields[idx].shr_var_uint32_arr));
    }
    else if (special_field_info->payload_type == FLOW_PAYLOAD_TYPE_UINT8_ARR)
    {
        sal_memcpy(special_field_data->shr_var_uint8_arr,
                   special_fields->special_fields[idx].shr_var_uint8_arr,
                   sizeof(special_fields->special_fields[idx].shr_var_uint8_arr));
    }
    else if (special_field_info->payload_type == FLOW_PAYLOAD_TYPE_UINT32)
    {
        special_field_data->shr_var_uint32 = special_fields->special_fields[idx].shr_var_uint32;
    }
    else if ((special_field_info->payload_type == FLOW_PAYLOAD_TYPE_ENUM)
             || (special_field_info->payload_type == FLOW_PAYLOAD_TYPE_BCM_ENUM))
    {
        special_field_data->symbol = special_fields->special_fields[idx].symbol;
    }
    else
    {
        /** type is FLOW_PAYLOAD_TYPE_ENABLER, no value */
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Indicate for an enabler field if it's enabled
 */
shr_error_e
dnx_flow_special_field_is_enabled(
    int unit,
    bcm_flow_special_fields_t * special_fields,
    bcm_flow_field_id_t field_id,
    uint8 *is_enabled)
{
    bcm_flow_special_field_t special_field_data;

    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);

    rv = dnx_flow_special_field_data_get(unit, special_fields, field_id, &special_field_data);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    *is_enabled = ((rv == _SHR_E_NONE) && (!special_field_data.is_clear));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * get an indication and returns if the indication is external or not
 */
shr_error_e
dnx_flow_special_ind_is_external(
    int unit,
    uint32 indication,
    uint8 *is_ind_external)
{
    SHR_FUNC_INIT_VARS(unit);

    if (indication & FLOW_S_FIELD_IND_IS_EXTERNAL)
    {
        (*is_ind_external) = TRUE;
    }
    else
    {
        (*is_ind_external) = FALSE;
    }

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flow_special_field_new_value_or_previous_if_not_exist_get(
    int unit,
    bcm_flow_special_fields_t * special_fields,
    bcm_flow_special_fields_t * prev_special_fields,
    flow_special_fields_e field_id,
    uint8 is_replace,
    bcm_flow_special_field_t * special_field_data,
    uint8 *is_value_new)
{
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(special_field_data, 0, sizeof(bcm_flow_special_field_t));
    rv = dnx_flow_special_field_data_get(unit, special_fields, field_id, special_field_data);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    *is_value_new = UTILEX_NUM2BOOL(rv == _SHR_E_NONE);

    if (is_replace && !(*is_value_new))
    {
        SHR_NULL_CHECK(prev_special_fields, _SHR_E_PARAM, "prev_special_fields");
        sal_memset(special_field_data, 0, sizeof(bcm_flow_special_field_t));
        rv = dnx_flow_special_field_data_get(unit, prev_special_fields, field_id, special_field_data);
    }

    SHR_SET_CURRENT_ERR(rv);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flow_special_fields_mask_field_value_get(
    int unit,
    int entry_handle_id,
    const flow_special_field_info_t * curr_field,
    bcm_flow_special_fields_t * key_special_fields,
    int key_idx)
{
    uint8 field_idx;
    uint8 shr_var_uint8_arr[BCM_FLOW_SPECIAL_FIELD_UIN8_ARR_MAX_SIZE] = { 0 };
    uint32 shr_var_uint32_arr[BCM_FLOW_SPECIAL_FIELD_UIN32_ARR_MAX_SIZE] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    switch (curr_field->payload_type)
    {
        case FLOW_PAYLOAD_TYPE_UINT8_ARR:
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_masked_get
                            (unit, entry_handle_id, curr_field->mapped_dbal_field,
                             key_special_fields->special_fields[key_idx].shr_var_uint8_arr, shr_var_uint8_arr));
            break;

        case FLOW_PAYLOAD_TYPE_UINT32_ARR:
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                            (unit, entry_handle_id, curr_field->mapped_dbal_field,
                             key_special_fields->special_fields[key_idx].shr_var_uint32_arr, shr_var_uint32_arr));
            break;

        case FLOW_PAYLOAD_TYPE_UINT32:
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                            (unit, entry_handle_id, curr_field->mapped_dbal_field,
                             &key_special_fields->special_fields[key_idx].shr_var_uint32, shr_var_uint32_arr));
            break;

        case FLOW_PAYLOAD_TYPE_ENUM:
        case FLOW_PAYLOAD_TYPE_BCM_ENUM:
            SHR_ERR_EXIT(_SHR_E_PARAM, " payload_type Enum is not supported for masking ");
            break;

        case FLOW_PAYLOAD_TYPE_ENABLER:
            SHR_ERR_EXIT(_SHR_E_PARAM, " payload_type Enabler is not supported for masking ");
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, " payload_type not supported %d ", curr_field->payload_type);
            break;

    }

    /** get the field's mask value */
    for (field_idx = 0; field_idx < key_special_fields->actual_nof_special_fields; field_idx++)
    {
        if (key_special_fields->special_fields[field_idx].field_id == curr_field->linked_special_field)
        {
            break;
        }
    }
    if (field_idx == key_special_fields->actual_nof_special_fields)
    {
        SHR_EXIT();
    }
    /** the mask field have the same payload type has its data field */
    switch (curr_field->payload_type)
    {
        case FLOW_PAYLOAD_TYPE_UINT8_ARR:
            sal_memcpy(key_special_fields->special_fields[field_idx].shr_var_uint8_arr, shr_var_uint8_arr,
                       sizeof(shr_var_uint8_arr));
            break;
        case FLOW_PAYLOAD_TYPE_UINT32_ARR:
            sal_memcpy(key_special_fields->special_fields[field_idx].shr_var_uint32_arr, shr_var_uint32_arr,
                       sizeof(shr_var_uint32_arr));
            break;
        case FLOW_PAYLOAD_TYPE_UINT32:
            key_special_fields->special_fields[field_idx].shr_var_uint32 = shr_var_uint32_arr[0];
            break;
        /** Should not be able to reach these cases */
        case FLOW_PAYLOAD_TYPE_ENUM:
        case FLOW_PAYLOAD_TYPE_BCM_ENUM:
            SHR_ERR_EXIT(_SHR_E_PARAM, " payload_type Enum is not supported for masking ");
            break;
        case FLOW_PAYLOAD_TYPE_ENABLER:
            SHR_ERR_EXIT(_SHR_E_PARAM, " payload_type Enabler is not supported for masking ");
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, " payload_type not supported %d ", curr_field->payload_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flow_special_fields_mask_field_value_set(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    int field_idx,
    bcm_flow_special_fields_t * special_fields)
{
    const flow_special_field_info_t *curr_field;
    bcm_flow_special_field_t *bcm_special_field = &special_fields->special_fields[field_idx];
    flow_special_fields_e mask_field_id;
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    curr_field = &flow_special_fields_db[special_fields->special_fields[field_idx].field_id];
    mask_field_id = curr_field->linked_special_field;

    for (ii = 0; ii < special_fields->actual_nof_special_fields; ii++)
    {
        if (ii == field_idx)
        {
            continue;
        }
        if (special_fields->special_fields[ii].field_id == mask_field_id)
        {
            if (curr_field->payload_type == FLOW_PAYLOAD_TYPE_UINT8_ARR)
            {
                dbal_entry_key_field_arr8_masked_set(unit, entry_handle_id, curr_field->mapped_dbal_field,
                                                     bcm_special_field->shr_var_uint8_arr,
                                                     special_fields->special_fields[ii].shr_var_uint8_arr);
            }
            else if (curr_field->payload_type == FLOW_PAYLOAD_TYPE_UINT32)
            {
                dbal_entry_key_field32_masked_set(unit, entry_handle_id, curr_field->mapped_dbal_field,
                                                  bcm_special_field->shr_var_uint32,
                                                  special_fields->special_fields[ii].shr_var_uint32);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "unsupported payload type for mask fields %s",
                             dnx_flow_payload_types_to_string(unit, curr_field->payload_type));
            }
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(_SHR_E_PARAM, "for field %s, related field mask fields %s was not set ", curr_field->name,
                 dnx_flow_special_field_to_string(unit, mask_field_id));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flow_special_field_form_dbal_field_info_get(
    int unit,
    dbal_fields_e dbal_field_id,
    flow_special_fields_e * field_id)
{
    int ii;
    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < NOF_FLOW_SPECIAL_FIELDS; ii++)
    {
        if (flow_special_fields_db[ii].mapped_dbal_field == dbal_field_id)
        {
            (*field_id) = ii;
            break;
        }
    }

    if (ii == NOF_FLOW_SPECIAL_FIELDS)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flow_special_field_printable_string_get(
    int unit,
    flow_special_fields_e field_id,
    bcm_flow_special_field_t * special_field,
    char print_buffer[FLOW_STR_MAX])
{
    const flow_special_field_info_t *curr_field;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(flow_special_field_info_get(unit, field_id, &curr_field));

    if (curr_field->print != NULL)
    {
        curr_field->print(unit, special_field, print_buffer);
    }
    else
    {
        if (curr_field->payload_type == FLOW_PAYLOAD_TYPE_UINT32_ARR)
        {
            int iter, char_count = 0;

            for (iter = 0; iter < BCM_FLOW_SPECIAL_FIELD_UIN32_ARR_MAX_SIZE - 1; iter++)
            {
                char_count = sal_snprintf(print_buffer, FLOW_STR_MAX, "0x%x:", special_field->shr_var_uint32_arr[iter]);
                print_buffer += char_count;
            }
            char_count = sal_snprintf(print_buffer, FLOW_STR_MAX, "0x%x", special_field->shr_var_uint32_arr[iter]);
            print_buffer += char_count;
        }
        else if (curr_field->payload_type == FLOW_PAYLOAD_TYPE_UINT8_ARR)
        {
            int iter, char_count = 0;

            for (iter = 0; iter < BCM_FLOW_SPECIAL_FIELD_UIN8_ARR_MAX_SIZE - 1; iter++)
            {
                char_count = sal_snprintf(print_buffer, FLOW_STR_MAX, "%02x:", special_field->shr_var_uint8_arr[iter]);
                print_buffer += char_count;
            }
            char_count = sal_snprintf(print_buffer, FLOW_STR_MAX, "%02x", special_field->shr_var_uint8_arr[iter]);
            print_buffer += char_count;
        }
        else if (curr_field->payload_type == FLOW_PAYLOAD_TYPE_UINT32)
        {
            sal_snprintf(print_buffer, FLOW_STR_MAX, "0x%x", special_field->shr_var_uint32);
        }
        else if ((curr_field->payload_type == FLOW_PAYLOAD_TYPE_ENUM)
                 || (curr_field->payload_type == FLOW_PAYLOAD_TYPE_BCM_ENUM))
        {
            sal_snprintf(print_buffer, FLOW_STR_MAX, "%d", special_field->symbol);
        }
        else
        {
            /** FLOW_PAYLOAD_TYPE_ENABLER - has no value */
            sal_snprintf(print_buffer, FLOW_STR_MAX, "ENABLED");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** SPECIAL FIELDS DEFINITIONS - END*/

/** SPECIAL FIELDS CB - START */

int
flow_special_field_tunnel_class_set(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    int field_idx,
    bcm_flow_special_fields_t * special_fields)
{

    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_is_flow_enabled_for_legacy_applications))
    {
        SHR_EXIT();
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

int
flow_special_field_tunnel_class_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    flow_special_fields_e special_field_id,
    bcm_flow_special_field_t * special_field)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_is_flow_enabled_for_legacy_applications))
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}

int
flow_special_field_ipv6_tunnel_type_set(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    int field_idx,
    bcm_flow_special_fields_t * special_fields)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

int
flow_special_field_ipv6_tunnel_type_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    flow_special_fields_e special_field_id,
    bcm_flow_special_field_t * special_field)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

int
flow_special_field_pcp_dei_profile_set(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    int field_idx,
    bcm_flow_special_fields_t * special_fields)
{
    uint32 user_val = special_fields->special_fields[field_idx].shr_var_uint32;

    SHR_FUNC_INIT_VARS(unit);

    if (DNX_QOS_MAP_IS_VLAN_PCP(user_val))
    {
        uint32 vlan_extend_pcp_dei_profile = 0;
        uint32 pcp_dei_map_extend = DBAL_ENUM_FVAL_PCP_DEI_MAP_TYPE_EXPLICIT;
        uint32 profile_ndx = DNX_QOS_MAP_PROFILE_GET(user_val);

        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                        (unit, DBAL_FIELD_VLAN_EDIT_PCP_DEI_PROFILE, DBAL_FIELD_PCP_DEI_QOS_PROFILE,
                         &profile_ndx, &vlan_extend_pcp_dei_profile));
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                        (unit, DBAL_FIELD_VLAN_EDIT_PCP_DEI_PROFILE, DBAL_FIELD_PCP_DEI_MAP_EXTEND,
                         &pcp_dei_map_extend, &vlan_extend_pcp_dei_profile));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PCP_DEI_PROFILE,
                                     INST_SINGLE, vlan_extend_pcp_dei_profile);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid qos map id [0x%x]\n", user_val);
    }

exit:
    SHR_FUNC_EXIT;
}

int
flow_special_field_pcp_dei_profile_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    flow_special_fields_e special_field_id,
    bcm_flow_special_field_t * special_field)
{
    uint32 extend_pcp_dei_profile, profile_ndx;

    SHR_FUNC_INIT_VARS(unit);

    FLOW_FIELD_GET_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PCP_DEI_PROFILE, INST_SINGLE,
                             &extend_pcp_dei_profile));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                    (unit, DBAL_FIELD_VLAN_EDIT_PCP_DEI_PROFILE, DBAL_FIELD_PCP_DEI_QOS_PROFILE, &profile_ndx,
                     &extend_pcp_dei_profile));

    DNX_QOS_VLAN_PCP_MAP_SET(profile_ndx);

    special_field->shr_var_uint32 = profile_ndx;
    special_field->field_id = special_field_id;

exit:
    SHR_FUNC_EXIT;
}

int
flow_special_field_glob_in_lif_set(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    int field_idx,
    bcm_flow_special_fields_t * special_fields)
{
    bcm_gport_t gport;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources_match;
    uint32 matched_global_in_lif;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get Global In-LIF of the port using DNX Algo Gport Management:
     */
    gport = special_fields->special_fields[field_idx].shr_var_uint32;
    sal_memset(&gport_hw_resources_match, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, gport,
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS |
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources_match),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);
    matched_global_in_lif = gport_hw_resources_match.global_in_lif;

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, matched_global_in_lif);

exit:
    SHR_FUNC_EXIT;
}

int
flow_special_field_glob_in_lif_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    flow_special_fields_e special_field_id,
    bcm_flow_special_field_t * special_field)
{
    uint32 matched_global_in_lif;

    SHR_FUNC_INIT_VARS(unit);

    FLOW_FIELD_GET_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, &matched_global_in_lif));

    special_field->shr_var_uint32 =
        dnx_algo_gpm_flow_gport_encode(unit, matched_global_in_lif, TRUE, FALSE, TRUE, FALSE);
    special_field->field_id = special_field_id;

exit:
    SHR_FUNC_EXIT;
}

int
flow_special_field_rch_destination_set(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    int field_idx,
    bcm_flow_special_fields_t * special_fields)
{
    uint32 user_val = special_fields->special_fields[field_idx].shr_var_uint32;
    uint32 destination;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                    (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, user_val, &destination));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, destination);

exit:
    SHR_FUNC_EXIT;
}

int
flow_special_field_rch_destination_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    flow_special_fields_e special_field_id,
    bcm_flow_special_field_t * special_field)
{
    uint32 destination;
    bcm_gport_t gport;
    SHR_FUNC_INIT_VARS(unit);

    FLOW_FIELD_GET_ERR_EXIT
        (dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, &destination));

    SHR_IF_ERR_EXIT(algo_gpm_gport_from_encoded_destination_field
                    (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, destination, &gport));
    special_field->shr_var_uint32 = (uint32) gport;
    special_field->field_id = special_field_id;

exit:
    SHR_FUNC_EXIT;
}

int
flow_special_field_rch_control_vlan_port_set(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    int field_idx,
    bcm_flow_special_fields_t * special_fields)
{
    uint32 user_val = special_fields->special_fields[field_idx].shr_var_uint32;
    uint32 control_lif = 0;
    int sf_idx;
    int is_p2p = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    if (user_val != 0)
    {
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources_local;
        int flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS;
        sal_memset(&gport_hw_resources_local, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
        SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                    (unit, user_val, flags, &gport_hw_resources_local), _SHR_E_NOT_FOUND, _SHR_E_PARAM);
        control_lif = (uint32) gport_hw_resources_local.local_in_lif;
    }
    else
    {
        for (sf_idx = 0; sf_idx < special_fields->actual_nof_special_fields; sf_idx++)
        {
            if ((special_fields->special_fields[sf_idx].field_id == FLOW_S_F_RCH_P2P) &&
                (special_fields->special_fields[sf_idx].is_clear == FALSE))
            {
                is_p2p = TRUE;
                break;
            }
        }
        /** use recycle default lif */
        SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_default_recyle_lif_get(unit, is_p2p, &control_lif));
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTROL_LIF, INST_SINGLE, control_lif);

exit:
    SHR_FUNC_EXIT;
}

int
flow_special_field_rch_control_vlan_port_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    flow_special_fields_e special_field_id,
    bcm_flow_special_field_t * special_field)
{
    uint32 recycle_default_lif_mp, recycle_default_lif_p2p, control_lif;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_default_recyle_lif_get(unit, FALSE, &recycle_default_lif_mp));
    SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_default_recyle_lif_get(unit, TRUE, &recycle_default_lif_p2p));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_CONTROL_LIF, INST_SINGLE, &control_lif));

    if ((control_lif != recycle_default_lif_mp) && (control_lif != recycle_default_lif_p2p))
    {
        bcm_gport_t flow_id;

        SHR_IF_ERR_EXIT(dnx_algo_gpm_flow_gport_from_local_lif(unit, control_lif, TRUE, _SHR_CORE_ALL, &flow_id));
        special_field->shr_var_uint32 = (uint32) flow_id;
    }
    special_field->field_id = special_field_id;

exit:
    SHR_FUNC_EXIT;
}

int
flow_special_field_next_outlif_pointer_set(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    int field_idx,
    bcm_flow_special_fields_t * special_fields)
{
    bcm_gport_t gport;
    dnx_algo_gpm_gport_hw_resources_t next_outlif_gport_hw_resources;
    int local_out_lif;

    SHR_FUNC_INIT_VARS(unit);

    gport = special_fields->special_fields[field_idx].shr_var_uint32;

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport,
                                                       DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                                                       &next_outlif_gport_hw_resources));
    local_out_lif = 0;
    if (next_outlif_gport_hw_resources.local_out_lif != DNX_ALGO_GPM_LIF_INVALID)
    {
        local_out_lif = next_outlif_gport_hw_resources.local_out_lif;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error! gport = 0x%08X has invalid local_out_lif! please check NEXT_OUTLIF_POINTER settings! \n",
                     gport);
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE, local_out_lif);

exit:
    SHR_FUNC_EXIT;
}

int
flow_special_field_next_outlif_pointer_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    flow_special_fields_e special_field_id,
    bcm_flow_special_field_t * special_field)
{
    uint32 local_out_lif;

    SHR_FUNC_INIT_VARS(unit);

    FLOW_FIELD_GET_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE, &local_out_lif));

    if (local_out_lif != 0)
    {
        /** Retrieve the flow id from the local lif and encode it as a gport */
        lif_mapping_local_lif_key_t local_lif_info;
        int global_lif;
        bcm_gport_t flow_id;
        shr_error_e rv;

        sal_memset(&local_lif_info, 0, sizeof(local_lif_info));
        local_lif_info.local_lif = local_out_lif;
        local_lif_info.core_id = DBAL_CORE_DEFAULT;
        rv = dnx_algo_lif_mapping_local_to_global_get(unit, DNX_ALGO_LIF_EGRESS, &local_lif_info, &global_lif);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        if (rv == _SHR_E_NONE)
        {
            BCM_GPORT_SUB_TYPE_OUT_LIF_SET(flow_id, global_lif);

        }
        else
        {
            uint32 virtual_id;
            /** If no Global-LIF is found - it may be a virtual gport */
            SHR_IF_ERR_EXIT(dnx_flow_egr_pointed_virtual_gport_get(unit, local_out_lif, &virtual_id));
            BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED_SET(flow_id, virtual_id);
        }
        BCM_GPORT_FLOW_LIF_ID_SET(flow_id, flow_id);

        special_field->shr_var_uint32 = flow_id;
        special_field->field_id = special_field_id;
    }

exit:
    SHR_FUNC_EXIT;
}

int
flow_special_field_outlif_match_set(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    int field_idx,
    bcm_flow_special_fields_t * special_fields)
{
    bcm_gport_t gport;
    dnx_algo_gpm_gport_hw_resources_t outlif_gport_hw_resources;
    int local_out_lif;

    SHR_FUNC_INIT_VARS(unit);

    gport = special_fields->special_fields[field_idx].shr_var_uint32;

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport,
                                                       DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                                                       &outlif_gport_hw_resources));
    local_out_lif = 0;
    if (outlif_gport_hw_resources.local_out_lif != DNX_ALGO_GPM_LIF_INVALID)
    {
        local_out_lif = outlif_gport_hw_resources.local_out_lif;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error! gport = 0x%08X has invalid local_out_lif! please check OUT_LIF_MATCH settings! \n", gport);
    }

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_out_lif);

exit:
    SHR_FUNC_EXIT;
}

int
flow_special_field_outlif_match_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    flow_special_fields_e special_field_id,
    bcm_flow_special_field_t * special_field)
{
    uint32 local_out_lif;

    SHR_FUNC_INIT_VARS(unit);

    FLOW_FIELD_GET_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_OUT_LIF, &local_out_lif));

    if (local_out_lif != 0)
    {
        /** Retrieve the flow id from the local lif and encode it as a gport */
        lif_mapping_local_lif_key_t local_lif_info;
        int global_lif;
        bcm_gport_t flow_id;
        shr_error_e rv;

        sal_memset(&local_lif_info, 0, sizeof(local_lif_info));
        local_lif_info.local_lif = local_out_lif;
        local_lif_info.core_id = DBAL_CORE_DEFAULT;
        rv = dnx_algo_lif_mapping_local_to_global_get(unit, DNX_ALGO_LIF_EGRESS, &local_lif_info, &global_lif);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        if (rv == _SHR_E_NONE)
        {
            BCM_GPORT_SUB_TYPE_OUT_LIF_SET(flow_id, global_lif);

        }
        else
        {
            uint32 virtual_id;
            /** If no Global-LIF is found - it may be a virtual gport */
            SHR_IF_ERR_EXIT(dnx_flow_egr_pointed_virtual_gport_get(unit, local_out_lif, &virtual_id));
            BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED_SET(flow_id, virtual_id);
        }
        BCM_GPORT_FLOW_LIF_ID_SET(flow_id, flow_id);

        special_field->shr_var_uint32 = flow_id;
        special_field->field_id = special_field_id;
    }

exit:
    SHR_FUNC_EXIT;
}
