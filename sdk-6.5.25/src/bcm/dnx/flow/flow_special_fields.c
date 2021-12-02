
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <shared/shrextend/shrextend_debug.h>
#include "flow_def.h"
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/l3/source_address_table_allocation.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tunnel.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <bcm_int/dnx/l3/l3_arp.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_l3_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <src/bcm/dnx/tunnel/tunnel_term.h>
#include <bcm_int/dnx/flow/flow.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/qos/qos.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm/l2.h>
#include <bcm/oam.h>


/**************************
 * FUNCTIONS DECLARATIONS *
 **************************/

int flow_special_field_ipv4_tunnel_type_set(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    int field_idx,
    bcm_flow_special_fields_t * special_fields);

int flow_special_field_ipv4_tunnel_type_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    flow_special_fields_e special_field_id,
    bcm_flow_special_field_t * special_field);

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

/** GENERAL DEFINITIONS - START */
/** macro for special field definitions when the operation cb are not used */
#define FLOW_S_FIELD_NO_MAPPING_CBS  NULL, NULL, NULL, NULL, NULL

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
#define FLOW_S_FIELD_IS_MATCH_UNCHANGEABLE      SAL_BIT(FLOW_SPECIAL_FIELD_IND_MATCH_UNCHANGEABLE)
#define FLOW_S_FIELD_IS_ALWAYS_UNCHANGEABLE    (FLOW_S_FIELD_IS_LIF_UNCHANGEABLE | FLOW_S_FIELD_IS_MATCH_UNCHANGEABLE)
#define FLOW_S_FIELD_VIRTUAL_MATCH_KEY          SAL_BIT(FLOW_SPECIAL_FIELD_IND_IS_KEY)

/** external indications are indications that the user suppose to know */
#define FLOW_S_FIELD_IND_IS_EXTERNAL            (FLOW_S_FIELD_CAN_BE_MASKED | \
                                                 FLOW_S_FIELD_IS_ALWAYS_UNCHANGEABLE | \
                                                 FLOW_S_FIELD_IS_ALWAYS_MANDATORY | \
                                                 FLOW_S_FIELD_VIRTUAL_MATCH_KEY)

/** GENERAL DEFINITIONS - END */

const char *
flow_special_field_vsi_assignment_mode_desc(
    int unit)
{
    return "How to get the VSI value, possible values FROM_VLAN / FROM_LIF / FROM_LOOKUP";
}

const char *
flow_special_field_pcp_dei_porf_desc(
    int unit)
{
    return "Set the QOS map preofile that was generated by bcm_qos_map_add()";
}

int
flow_special_field_vsi_assignment_mode_print(
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

const char *
flow_special_field_ipv4_tunnel_desc(
    int unit)
{
    return
        "IPv4 Tunnel type, legal values are taken from bcm_tunnel_type_t. For Lif application value cannot be changed in replace.";
}

const char *
flow_special_field_ipv6_tunnel_desc(
    int unit)
{
    return "IPv6 Tunnel type, legal values are taken from bcm_tunnel_type_t";
}

const char *
flow_special_field_ipvx_tunnel_desc(
    int unit)
{
    return "IPvX Tunnel type, legal values are taken from bcm_tunnel_type_t";
}

const char *
flow_special_field_qos_desc(
    int unit)
{
    return "sets QoS info, require the setting of the common field BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID";
}

const char *
flow_special_field_mpls_port_vccv_type_desc(
    int unit)
{
    return "Legal values are mpls port types taken from bcm_mpls_port_control_channel_type_t";
}

const char *
flow_special_field_tunnel_init_flags_desc(
    int unit)
{
    return
        "IPv4 Tunnel initiator flags, mapping according to BCM_TUNNEL_INIT flags.\nField can only be set when setting IPV4_TUNNEL_TYPE.\nCan only set flags matching the matching tunnel_type";
}

const char *
flow_special_field_rch_type_desc(
    int unit)
{
    return "L2 Egress recycle Apps.";
}

const char *
flow_special_field_gtp_pdu_type_desc(
    int unit)
{
    return "sets GTP extension header PDU type: 0 or 1";
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
        SHR_IF_ERR_EXIT(flow_special_field_ipv4_tunnel_print(unit, special_field, print_string));
    }
    else
    {
        SHR_IF_ERR_EXIT(flow_special_field_ipv6_tunnel_print(unit, special_field, print_string));
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
/** SPECIAL FIELDS DEFINITIONS - START */

/* *INDENT-OFF* */ 
 
/** DB for all existing special fields, in order to add a new field, user should also update the
 *  flow_special_fields_e enum in the same order. In addition, when adding new special field,
 *  need to update ctest_flow_ut_special_field_legal_value_get with proper value for the special field
 *  */

    /** field_name  */            /** mapped_dbal_field  */             /** payload_type  */                  /** linked_special_field */   /** description cb */   /** print cb */  /** field indications */                        /** cb for operations */
const flow_special_field_info_t flow_special_fields_db[NOF_FLOW_SPECIAL_FIELDS] = {
    {"",                            FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,      NULL,                   NULL,           FLOW_S_FIELD_NO_INDICATIONS,                                          FLOW_S_FIELD_NO_MAPPING_CBS},

    {"IPV4_DIP",                    DBAL_FIELD_IPV4_DIP,                    FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_F_IPV4_DIP_MASK,     NULL,                   NULL,            FLOW_S_FIELD_CAN_BE_MASKED,                                                                                                                   FLOW_S_FIELD_NO_MAPPING_CBS},
    {"IPV4_DIP_MASK",               DBAL_FIELD_DST_IPV4_MASK,               FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,      NULL,                   NULL,           (FLOW_S_FIELD_SHOULD_BE_IGNORED       | FLOW_S_FIELD_USED_AS_MASK),                                                                            FLOW_S_FIELD_NO_MAPPING_CBS},
    {"IPV4_SIP",                    DBAL_FIELD_IPV4_SIP,                    FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_F_IPV4_SIP_MASK,     NULL,                   NULL,           (FLOW_S_FIELD_CAN_BE_MASKED           | FLOW_S_FIELD_SRC_ADDR_PROFILE_SUPPORT),                                                                FLOW_S_FIELD_NO_MAPPING_CBS},
    {"IPV4_SIP_MASK",               DBAL_FIELD_SRC_IPV4_MASK,               FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,      NULL,                   NULL,           (FLOW_S_FIELD_SHOULD_BE_IGNORED       | FLOW_S_FIELD_USED_AS_MASK),                                                                            FLOW_S_FIELD_NO_MAPPING_CBS},
    {"IPV4",                        DBAL_FIELD_IPV4,                        FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,      NULL,                   NULL,            FLOW_S_FIELD_NO_INDICATIONS,                                                                                                                   FLOW_S_FIELD_NO_MAPPING_CBS},
    {"IPV6_DIP",                    DBAL_FIELD_IPV6_DIP,                    FLOW_PAYLOAD_TYPE_UINT8_ARR,        FLOW_S_F_IPV6_DIP_MASK,     NULL,                   NULL,           (FLOW_S_FIELD_LIF_SHOULD_BE_IGNORED   | FLOW_S_FIELD_IS_LIF_MANDATORY   | FLOW_S_FIELD_CAN_BE_MASKED),                                         FLOW_S_FIELD_NO_MAPPING_CBS},
    {"IPV6_DIP_MASK",               DBAL_FIELD_DST_IPV6_MASK,               FLOW_PAYLOAD_TYPE_UINT8_ARR,        FLOW_S_FIELD_UNLINKED,      NULL,                   NULL,           (FLOW_S_FIELD_SHOULD_BE_IGNORED       | FLOW_S_FIELD_USED_AS_MASK),                                                                            FLOW_S_FIELD_NO_MAPPING_CBS},
    {"IPV6_SIP",                    DBAL_FIELD_IPV6_SIP,                    FLOW_PAYLOAD_TYPE_UINT8_ARR,        FLOW_S_F_IPV6_SIP_MASK,     NULL,                   NULL,           (FLOW_S_FIELD_MATCH_SHOULD_BE_IGNORED | FLOW_S_FIELD_IS_MATCH_MANDATORY | FLOW_S_FIELD_CAN_BE_MASKED | FLOW_S_FIELD_SRC_ADDR_PROFILE_SUPPORT), FLOW_S_FIELD_NO_MAPPING_CBS},
    {"IPV6_SIP_MASK",               DBAL_FIELD_SRC_IPV6_MASK,               FLOW_PAYLOAD_TYPE_UINT8_ARR,        FLOW_S_FIELD_UNLINKED,      NULL,                   NULL,           (FLOW_S_FIELD_SHOULD_BE_IGNORED       | FLOW_S_FIELD_USED_AS_MASK),                                                                            FLOW_S_FIELD_NO_MAPPING_CBS},
    {"IPV6",                        DBAL_FIELD_IPV6,                        FLOW_PAYLOAD_TYPE_UINT8_ARR,        FLOW_S_F_IPV6_MASK,         NULL,                   NULL,           (FLOW_S_FIELD_LIF_SHOULD_BE_IGNORED   | FLOW_S_FIELD_IS_LIF_MANDATORY   | FLOW_S_FIELD_CAN_BE_MASKED),                                         FLOW_S_FIELD_NO_MAPPING_CBS},
    {"IPV6_MASK",                   DBAL_FIELD_DST_IPV6_MASK,               FLOW_PAYLOAD_TYPE_UINT8_ARR,        FLOW_S_FIELD_UNLINKED,      NULL,                   NULL,           (FLOW_S_FIELD_SHOULD_BE_IGNORED       | FLOW_S_FIELD_USED_AS_MASK),                                                                            FLOW_S_FIELD_NO_MAPPING_CBS},
    {"SRC_MAC_ADDRESS",             DBAL_FIELD_SOURCE_ADDRESS,              FLOW_PAYLOAD_TYPE_UINT8_ARR,        FLOW_S_FIELD_UNLINKED,      NULL,                   NULL,            FLOW_S_FIELD_SRC_ADDR_PROFILE_SUPPORT,                                                                                                        FLOW_S_FIELD_NO_MAPPING_CBS},
    {"DST_MAC_ADDRESS",             DBAL_FIELD_L2_MAC,                      FLOW_PAYLOAD_TYPE_UINT8_ARR,        FLOW_S_FIELD_DIRECT_MAPPING},
    {"DUAL_HOMING",                 DBAL_FIELD_DUAL_HOMING,                 FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,      NULL,                   NULL,           FLOW_S_FIELD_SHOULD_BE_IGNORED,    FLOW_S_FIELD_NO_MAPPING_CBS},


    {"TUNNEL_ENDPOINT_IDENTIFIER",  DBAL_FIELD_TEID,                        FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_DIRECT_MAPPING},
    {"NEXT_LAYER_NETWORK_DOMAIN",   DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN,   FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_DIRECT_MAPPING},
    {"MPLS_PHP",                    FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_ENABLER,          FLOW_S_FIELD_UNLINKED, NULL,                  NULL,           FLOW_S_FIELD_SHOULD_BE_IGNORED,  FLOW_S_FIELD_NO_MAPPING_CBS},
    {"MPLS_LABEL",                  DBAL_FIELD_MPLS_LABEL,                  FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_DIRECT_MAPPING},
    {"MPLS_LABEL_2",                DBAL_FIELD_MPLS_LABEL_2,                FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_DIRECT_MAPPING},
    {"MPLS_LABEL_3",                FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED, NULL,                  NULL,           FLOW_S_FIELD_SHOULD_BE_IGNORED,  FLOW_S_FIELD_NO_MAPPING_CBS},
    {"MPLS_LABEL_4",                FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED, NULL,                  NULL,           FLOW_S_FIELD_SHOULD_BE_IGNORED,  FLOW_S_FIELD_NO_MAPPING_CBS},
    {"MPLS_LABEL_5",                FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED, NULL,                  NULL,           FLOW_S_FIELD_SHOULD_BE_IGNORED,  FLOW_S_FIELD_NO_MAPPING_CBS},
    {"MPLS_LABEL_6",                FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED, NULL,                  NULL,           FLOW_S_FIELD_SHOULD_BE_IGNORED,  FLOW_S_FIELD_NO_MAPPING_CBS},
    {"MPLS_LABEL_7",                FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED, NULL,                  NULL,           FLOW_S_FIELD_SHOULD_BE_IGNORED,  FLOW_S_FIELD_NO_MAPPING_CBS},
    {"MPLS_LABEL_8",                FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED, NULL,                  NULL,           FLOW_S_FIELD_SHOULD_BE_IGNORED,  FLOW_S_FIELD_NO_MAPPING_CBS},
    {"VRF",                         DBAL_FIELD_VRF,                         FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_DIRECT_MAPPING},

    {"SRC_UDP_PORT",                DBAL_FIELD_LIF_AHP_UDP,                 FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,      NULL,                                     NULL,               FLOW_S_FIELD_SHOULD_BE_IGNORED,     FLOW_S_FIELD_NO_MAPPING_CBS},
    {"DST_UDP_PORT",                DBAL_FIELD_LIF_AHP_UDP,                 FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,      NULL,                                     NULL,               FLOW_S_FIELD_SHOULD_BE_IGNORED,     FLOW_S_FIELD_NO_MAPPING_CBS},
    {"IPV4_HEADER_DF",              DBAL_FIELD_IPV4_HEADER_DF_FLAG,         FLOW_PAYLOAD_TYPE_ENABLER,          FLOW_S_FIELD_DIRECT_MAPPING},
    {"IPVX_INIT_GRE_KEY_USE_LB",    FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_ENABLER,          FLOW_S_FIELD_UNLINKED,      NULL,                                     NULL,               FLOW_S_FIELD_SHOULD_BE_IGNORED,     FLOW_S_FIELD_NO_MAPPING_CBS},
    {"IPVX_INIT_GRE_WITH_SN",       FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_ENABLER,          FLOW_S_FIELD_UNLINKED,      NULL,                                     NULL,               FLOW_S_FIELD_SHOULD_BE_IGNORED,     FLOW_S_FIELD_NO_MAPPING_CBS},
                                                                                                                                                                    
    {"VLAN_EDIT_PROFILE",           DBAL_FIELD_VLAN_EDIT_PROFILE,           FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_DIRECT_MAPPING},
    {"VLAN_EDIT_PCP_DEI_PROFILE",   DBAL_FIELD_VLAN_EDIT_PCP_DEI_PROFILE,   FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,   flow_special_field_pcp_dei_porf_desc,        NULL,           FLOW_S_FIELD_NO_INDICATIONS,        flow_special_field_pcp_dei_profile_set, flow_special_field_pcp_dei_profile_get, NULL, NULL, NULL},
    {"VLAN_EDIT_VID_1",             DBAL_FIELD_VLAN_EDIT_VID_1,             FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_DIRECT_MAPPING},
    {"VLAN_EDIT_VID_2",             DBAL_FIELD_VLAN_EDIT_VID_2,             FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_DIRECT_MAPPING},
    {"VSI_ASSIGNMENT_MODE",         DBAL_FIELD_FODO_ASSIGNMENT_MODE,        FLOW_PAYLOAD_TYPE_ENUM,             FLOW_S_FIELD_UNLINKED,   flow_special_field_vsi_assignment_mode_desc, flow_special_field_vsi_assignment_mode_print,     FLOW_S_FIELD_NO_INDICATIONS,        FLOW_S_FIELD_NO_MAPPING_CBS},
    {"PORT",                        DBAL_FIELD_PP_PORT,                     FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,      NULL,                   NULL, FLOW_S_FIELD_MATCH_SHOULD_BE_IGNORED, NULL, NULL, NULL, NULL, NULL},
    {"IN_PORT",                     DBAL_FIELD_IN_PP_PORT,                  FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,      NULL,                   NULL, FLOW_S_FIELD_MATCH_SHOULD_BE_IGNORED, NULL, NULL, NULL, NULL, NULL},
    {"MAPPED_PORT",                 DBAL_FIELD_MAPPED_PP_PORT,              FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,      NULL,                   NULL, FLOW_S_FIELD_MATCH_SHOULD_BE_IGNORED, NULL, NULL, NULL, NULL, NULL},
    {"VLAN_DOMAIN",                 DBAL_FIELD_VLAN_DOMAIN,                 FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_DIRECT_MAPPING},
    {"ESEM_NAME_SPACE",             DBAL_FIELD_ESEM_NAME_SPACE,             FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_DIRECT_MAPPING},
    {"GLOB_IN_LIF",                 DBAL_FIELD_GLOB_IN_LIF,                 FLOW_PAYLOAD_TYPE_UINT32,            FLOW_S_FIELD_UNLINKED,     NULL,                   NULL, FLOW_S_FIELD_NO_INDICATIONS, flow_special_field_glob_in_lif_set, flow_special_field_glob_in_lif_get, NULL, NULL, NULL},

    {"VID",                         DBAL_FIELD_VID,                         FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_DIRECT_MAPPING},
    {"S_VID",                       DBAL_FIELD_S_VID,                       FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_DIRECT_MAPPING},
    {"C_VID",                       DBAL_FIELD_C_VID,                       FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_DIRECT_MAPPING},
    {"S_VID_1",                     DBAL_FIELD_S_VID_1,                     FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_DIRECT_MAPPING},
    {"S_VID_2",                     DBAL_FIELD_S_VID_2,                     FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_DIRECT_MAPPING},
    {"C_VID_1",                     DBAL_FIELD_C_VID_1,                     FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_DIRECT_MAPPING},
    {"C_VID_2",                     DBAL_FIELD_C_VID_2,                     FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_DIRECT_MAPPING},
    {"VSI",                         DBAL_FIELD_VSI,                         FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_DIRECT_MAPPING},

    {"VID_OUTER_VLAN",              DBAL_FIELD_VID_OUTER_VLAN,              FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_DIRECT_MAPPING},
    {"VID_INNER_VLAN",              DBAL_FIELD_VID_INNER_VLAN,              FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_DIRECT_MAPPING},
    {"PCP_DEI_OUTER_VLAN",          DBAL_FIELD_PCP_DEI_OUTER_VLAN,          FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_DIRECT_MAPPING},
    {"TUNNEL_CLASS",                FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,   NULL, NULL, (FLOW_S_FIELD_LIF_SHOULD_BE_IGNORED| FLOW_S_FIELD_IS_ALWAYS_MANDATORY | FLOW_S_FIELD_IS_LIF_UNCHANGEABLE), flow_special_field_tunnel_class_set, flow_special_field_tunnel_class_get, NULL, NULL, NULL},

    {"QOS_TTL",                     FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED, flow_special_field_qos_desc, NULL,  FLOW_S_FIELD_SHOULD_BE_IGNORED,        FLOW_S_FIELD_NO_MAPPING_CBS},
    {"QOS_DSCP",                    FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED, flow_special_field_qos_desc, NULL,  FLOW_S_FIELD_SHOULD_BE_IGNORED,        FLOW_S_FIELD_NO_MAPPING_CBS},
    {"QOS_EXP",                     FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED, flow_special_field_qos_desc, NULL,  FLOW_S_FIELD_SHOULD_BE_IGNORED,        FLOW_S_FIELD_NO_MAPPING_CBS},
    {"QOS_PRI",                     FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED, flow_special_field_qos_desc, NULL,  FLOW_S_FIELD_SHOULD_BE_IGNORED,        FLOW_S_FIELD_NO_MAPPING_CBS},
    {"QOS_CFI",                     FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED, flow_special_field_qos_desc, NULL,  FLOW_S_FIELD_SHOULD_BE_IGNORED,        FLOW_S_FIELD_NO_MAPPING_CBS},

    {"BITSTR",                      DBAL_FIELD_BITSTR,                      FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_DIRECT_MAPPING},
    {"BFR_BIT_STR",                 DBAL_FIELD_BFR_BIT_STR,                 FLOW_PAYLOAD_TYPE_UINT32_ARR,       FLOW_S_FIELD_DIRECT_MAPPING},
    {"BIER_BIFT_ID",                DBAL_FIELD_BIER_BIFT_ID,                FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_DIRECT_MAPPING},
    {"BIER_BSL",                    DBAL_FIELD_BIER_BSL,                    FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_DIRECT_MAPPING},
    {"BIER_RSV",                    DBAL_FIELD_BIER_RSV,                    FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_DIRECT_MAPPING},
    {"BIER_OAM",                    DBAL_FIELD_BIER_OAM,                    FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_DIRECT_MAPPING},
    {"BIER_BFIR_ID",                DBAL_FIELD_BIER_BFIR_ID,                FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_DIRECT_MAPPING},

    {"IPV4_TUNNEL_TYPE",            DBAL_FIELD_IPV4_TUNNEL_TYPE,            FLOW_PAYLOAD_TYPE_BCM_ENUM,         FLOW_S_FIELD_UNLINKED,   flow_special_field_ipv4_tunnel_desc, flow_special_field_ipv4_tunnel_print, (FLOW_S_FIELD_LIF_SHOULD_BE_IGNORED| FLOW_S_FIELD_IS_ALWAYS_MANDATORY | FLOW_S_FIELD_IS_LIF_UNCHANGEABLE), flow_special_field_ipv4_tunnel_type_set, flow_special_field_ipv4_tunnel_type_get, NULL, NULL, NULL},
    {"IPV6_TUNNEL_TYPE",  DBAL_FIELD_IPV6_QUALIFIER_ADDITIONAL_HEADER_1ST,  FLOW_PAYLOAD_TYPE_BCM_ENUM,         FLOW_S_FIELD_UNLINKED,   flow_special_field_ipv6_tunnel_desc, flow_special_field_ipv6_tunnel_print, (FLOW_S_FIELD_LIF_SHOULD_BE_IGNORED| FLOW_S_FIELD_IS_ALWAYS_MANDATORY | FLOW_S_FIELD_IS_LIF_UNCHANGEABLE), flow_special_field_ipv6_tunnel_type_set, flow_special_field_ipv6_tunnel_type_get, NULL, NULL, NULL},
    {"IPVX_TUNNEL_TYPE",            FLOW_S_F_UNMAPPED,                      FLOW_PAYLOAD_TYPE_BCM_ENUM,         FLOW_S_FIELD_UNLINKED,   flow_special_field_ipvx_tunnel_desc, flow_special_field_ipvx_tunnel_print,  (FLOW_S_FIELD_IS_ALWAYS_MANDATORY | FLOW_S_FIELD_IS_LIF_UNCHANGEABLE), NULL, NULL, NULL, NULL, NULL},
    {"NOF_LAYERS_TO_TERMINATE",     DBAL_FIELD_TERMINATION_TYPE,            FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_DIRECT_MAPPING},

    /** LAYER TYPE should changed to ENUM */
    {"LAYER_TYPE",                  DBAL_FIELD_LAYER_TYPE,                  FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_DIRECT_MAPPING},
    {"MPLS_ENTROPY_LABEL",            FLOW_S_F_UNMAPPED,                    FLOW_PAYLOAD_TYPE_ENABLER,          FLOW_S_FIELD_UNLINKED, NULL,                  NULL,           FLOW_S_FIELD_SHOULD_BE_IGNORED,  FLOW_S_FIELD_NO_MAPPING_CBS},
    {"MPLS_ENTROPY_LABEL_INDICATION", FLOW_S_F_UNMAPPED,                    FLOW_PAYLOAD_TYPE_ENABLER,          FLOW_S_FIELD_UNLINKED, NULL,                  NULL,           FLOW_S_FIELD_SHOULD_BE_IGNORED,  FLOW_S_FIELD_NO_MAPPING_CBS},
    {"MPLS_CONTROL_WORD_ENABLE",      FLOW_S_F_UNMAPPED,                    FLOW_PAYLOAD_TYPE_ENABLER,          FLOW_S_FIELD_UNLINKED, NULL,                  NULL,           FLOW_S_FIELD_SHOULD_BE_IGNORED,  FLOW_S_FIELD_NO_MAPPING_CBS},
    {"MPLS_CONTROL_PLATFORM_NAMESPACE", FLOW_S_F_UNMAPPED,                  FLOW_PAYLOAD_TYPE_ENABLER,          FLOW_S_FIELD_UNLINKED, NULL,                  NULL,           FLOW_S_FIELD_SHOULD_BE_IGNORED,  FLOW_S_FIELD_NO_MAPPING_CBS},
    {"MPLS_INCLUSIVE_MULTICAST_LABEL",FLOW_S_F_UNMAPPED,                    FLOW_PAYLOAD_TYPE_ENABLER,          FLOW_S_FIELD_UNLINKED, NULL,                  NULL,           FLOW_S_FIELD_SHOULD_BE_IGNORED,  FLOW_S_FIELD_NO_MAPPING_CBS},  
    {"MPLS_SVTAG",                    FLOW_S_F_UNMAPPED,                    FLOW_PAYLOAD_TYPE_ENABLER,          FLOW_S_FIELD_UNLINKED, NULL,                  NULL,           FLOW_S_FIELD_SHOULD_BE_IGNORED,  FLOW_S_FIELD_NO_MAPPING_CBS},  

    {"MPLS_PORT_VCCV_TYPE",         DBAL_FIELD_TERMINATION_TYPE,            FLOW_PAYLOAD_TYPE_BCM_ENUM,         FLOW_S_FIELD_UNLINKED,   flow_special_field_mpls_port_vccv_type_desc, flow_special_field_mpls_port_vccv_type_print, FLOW_S_FIELD_SHOULD_BE_IGNORED,  FLOW_S_FIELD_NO_MAPPING_CBS},
    {"EXPECT_BOS",                    FLOW_S_F_UNMAPPED,                    FLOW_PAYLOAD_TYPE_ENABLER,          FLOW_S_FIELD_UNLINKED, NULL,                  NULL,           FLOW_S_FIELD_SHOULD_BE_IGNORED,  FLOW_S_FIELD_NO_MAPPING_CBS},
    {"TRAP_TTL_0",                    FLOW_S_F_UNMAPPED,                    FLOW_PAYLOAD_TYPE_ENABLER,          FLOW_S_FIELD_UNLINKED, NULL,                  NULL,           FLOW_S_FIELD_SHOULD_BE_IGNORED,  FLOW_S_FIELD_NO_MAPPING_CBS},
    {"TRAP_TTL_1",                    FLOW_S_F_UNMAPPED,                    FLOW_PAYLOAD_TYPE_ENABLER,          FLOW_S_FIELD_UNLINKED, NULL,                  NULL,           FLOW_S_FIELD_SHOULD_BE_IGNORED,  FLOW_S_FIELD_NO_MAPPING_CBS},

    {"MATCH_LABEL",                 DBAL_FIELD_MPLS_LABEL,                  FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED, NULL,                  NULL,           FLOW_S_FIELD_IS_MATCH_MANDATORY,     FLOW_S_FIELD_NO_MAPPING_CBS},
    {"VSI_MATCH",                   DBAL_FIELD_VSI,                         FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,  NULL,               NULL,           (FLOW_S_FIELD_VIRTUAL_MATCH_KEY | FLOW_S_FIELD_IS_LIF_MANDATORY),   FLOW_S_FIELD_NO_MAPPING_CBS},
    {"CLASS_ID_MATCH",              DBAL_FIELD_VLAN_DOMAIN,                 FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,  NULL,               NULL,           (FLOW_S_FIELD_VIRTUAL_MATCH_KEY | FLOW_S_FIELD_IS_LIF_MANDATORY),   FLOW_S_FIELD_NO_MAPPING_CBS},
    {"NAME_SPACE_MATCH",            DBAL_FIELD_ESEM_NAME_SPACE,                FLOW_PAYLOAD_TYPE_UINT32,            FLOW_S_FIELD_UNLINKED,    NULL,                NULL,            (FLOW_S_FIELD_VIRTUAL_MATCH_KEY | FLOW_S_FIELD_IS_LIF_MANDATORY),    FLOW_S_FIELD_NO_MAPPING_CBS},
    {"VID_MATCH",                   DBAL_FIELD_C_VID,                       FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,  NULL,               NULL,           (FLOW_S_FIELD_VIRTUAL_MATCH_KEY | FLOW_S_FIELD_IS_LIF_MANDATORY),   FLOW_S_FIELD_NO_MAPPING_CBS},
    {"SYSTEM_PORT_MATCH",           DBAL_FIELD_SYSTEM_PORT_AGGREGATE,       FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,  NULL,               NULL,           (FLOW_S_FIELD_VIRTUAL_MATCH_KEY),   FLOW_S_FIELD_NO_MAPPING_CBS},
    {"SRC_ENCAP_ID_MATCH",          DBAL_FIELD_GLOB_IN_LIF,                 FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,  NULL,               NULL,           (FLOW_S_FIELD_VIRTUAL_MATCH_KEY),   FLOW_S_FIELD_NO_MAPPING_CBS},


    {"RCH_VRF",                     DBAL_FIELD_FODO_VRF,                    FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_DIRECT_MAPPING},
    {"RCH_OUTLIF_0_PUSHED_TO_STACK",DBAL_FIELD_OUTLIF_0_PUSHED_TO_STACK,    FLOW_PAYLOAD_TYPE_ENABLER,          FLOW_S_FIELD_DIRECT_MAPPING},
    {"RCH_P2P",                     DBAL_FIELD_SERVICE_TYPE_IS_P2P,         FLOW_PAYLOAD_TYPE_ENABLER,          FLOW_S_FIELD_DIRECT_MAPPING},
    {"RCH_DESTINATION",             DBAL_FIELD_DESTINATION,                 FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED, NULL,                NULL,           FLOW_S_FIELD_NO_INDICATIONS,  flow_special_field_rch_destination_set, flow_special_field_rch_destination_get, NULL, NULL, NULL},
    {"RCH_CONTROL_VLAN_PORT",       DBAL_FIELD_CONTROL_LIF,                 FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED, NULL,                NULL,           FLOW_S_FIELD_NO_INDICATIONS,  flow_special_field_rch_control_vlan_port_set, flow_special_field_rch_control_vlan_port_get, NULL, NULL, NULL},

    {"REFLECTOR_PROCESS_TYPE",      DBAL_FIELD_RAW_DATA,                    FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_DIRECT_MAPPING},
    {"REFLECTOR_SRC_MAC",           DBAL_FIELD_L2_MAC,                      FLOW_PAYLOAD_TYPE_UINT8_ARR,        FLOW_S_FIELD_DIRECT_MAPPING},
    {"GTP_PDU_TYPE",                DBAL_FIELD_GTP_PDU_TYPE,                FLOW_PAYLOAD_TYPE_UINT32,           FLOW_S_FIELD_UNLINKED,   flow_special_field_gtp_pdu_type_desc, NULL,  FLOW_S_FIELD_NO_INDICATIONS,   FLOW_S_FIELD_NO_MAPPING_CBS},
    {"IS_INTERMEDIATE_LIF",         DBAL_FIELD_IPV6_DIP_IDX_TYPE,           FLOW_PAYLOAD_TYPE_ENABLER,          FLOW_S_FIELD_UNLINKED, NULL,                  NULL,           FLOW_S_FIELD_SHOULD_BE_IGNORED,  FLOW_S_FIELD_NO_MAPPING_CBS}
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
    uint8 idx;
    const flow_special_field_info_t *special_field_info;

    SHR_FUNC_INIT_VARS(unit);

    if (special_fields == NULL)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    for (idx = 0; idx < special_fields->actual_nof_special_fields; idx++)
    {
        if (special_fields->special_fields[idx].field_id == field_id)
        {
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
            break;
        }
    }

    if (idx == special_fields->actual_nof_special_fields)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

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
    if (rv == _SHR_E_NONE)
    {
        (*is_value_new) = TRUE;
    }
    else if (is_replace)        /* _SHR_E_NOT_FOUND */
    {
        SHR_NULL_CHECK(prev_special_fields, _SHR_E_PARAM, "prev_special_fields");
        (*is_value_new) = FALSE;
        sal_memset(special_field_data, 0, sizeof(bcm_flow_special_field_t));
        rv = dnx_flow_special_field_data_get(unit, prev_special_fields, field_id, special_field_data);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
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


static int
flow_tunnel_class_is_managed_by_user(
    int unit)
{
    return !UTILEX_NUM2BOOL(dnx_data_tunnel.tunnel_type.udp_tunnel_type_get(unit));
}


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
    uint32 tunnel_class;

    SHR_FUNC_INIT_VARS(unit);

    tunnel_class = special_fields->special_fields[field_idx].shr_var_uint32;

    if (flow_tunnel_class_is_managed_by_user(unit))
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_MP_TUNNEL_IDX, tunnel_class);
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

    if (flow_tunnel_class_is_managed_by_user(unit))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_IPV6_MP_TUNNEL_IDX, &(special_field->shr_var_uint32)));
    }
exit:
    SHR_FUNC_EXIT;
}

int
flow_special_field_ipv4_tunnel_type_set(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    int field_idx,
    bcm_flow_special_fields_t * special_fields)
{
    bcm_tunnel_type_t tunnel_type;

    SHR_FUNC_INIT_VARS(unit);

    tunnel_type = special_fields->special_fields[field_idx].symbol;

    if (!dnx_data_tunnel.tunnel_type.udp_tunnel_type_get(unit))
    {
        uint32 ipv4_layer_qualifier_tunnel_type;
        bcm_tunnel_terminator_t info = { 0 };
        info.type = tunnel_type;
        /** converting the bcm ipv4 tunnel type to dbal tunnel type */
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_type_to_ipv4_layer_qualifier_tunnel_type
                        (unit, &info, &ipv4_layer_qualifier_tunnel_type));
        /** setting the ipv4 tunnel type */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4_TUNNEL_TYPE,
                                   ipv4_layer_qualifier_tunnel_type);
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

int
flow_special_field_ipv4_tunnel_type_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    flow_special_fields_e special_field_id,
    bcm_flow_special_field_t * special_field)
{

    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_tunnel.tunnel_type.udp_tunnel_type_get(unit))
    {
        uint32 ipv4_layer_qualifier_tunnel_type, dummy;
        /** getting the ipv4 tunnel type */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_IPV4_TUNNEL_TYPE, &ipv4_layer_qualifier_tunnel_type));
        /** converting the dbal tunnel type to bcm ipv4 tunnel type */
        SHR_IF_ERR_EXIT(dnx_ipv4_layer_qualifier_tunnel_type_to_tunnel_terminator_type
                        (unit, ipv4_layer_qualifier_tunnel_type, (bcm_tunnel_type_t *) & special_field->symbol,
                         &dummy));
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
    bcm_tunnel_type_t tunnel_type = special_fields->special_fields[field_idx].symbol;

    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_tunnel.tunnel_type.udp_tunnel_type_get(unit))
    {
        uint32 first_additional_header, second_additional_header;

        /** calculating the addiotnal_header values from the tunnel_type */
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_type_to_ipv6_additional_headers
                        (unit, tunnel_type, &first_additional_header, &second_additional_header));

        /** setting the addiotnal_header values */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_QUALIFIER_ADDITIONAL_HEADER_1ST,
                                   first_additional_header);

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_QUALIFIER_ADDITIONAL_HEADER_2ND,
                                   second_additional_header);
    }

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

    if (!dnx_data_tunnel.tunnel_type.udp_tunnel_type_get(unit))
    {
        bcm_tunnel_type_t tunnel_type;
        uint32 first_additional_header, second_additional_header;
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_IPV6_QUALIFIER_ADDITIONAL_HEADER_1ST,
                         &first_additional_header));

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_IPV6_QUALIFIER_ADDITIONAL_HEADER_2ND,
                         &second_additional_header));

        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_additional_headers_to_ipv6_type
                        (unit, first_additional_header, second_additional_header, &tunnel_type));

        special_field->symbol = (uint32) tunnel_type;
    }
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
    bcm_gport_t gport;
    uint32 matched_global_in_lif;

    SHR_FUNC_INIT_VARS(unit);

    FLOW_FIELD_GET_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, &matched_global_in_lif));

    /** convert from global-in-lif to gport ! */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                    (unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS, DBAL_CORE_ALL, matched_global_in_lif,
                     &gport));

    special_field->shr_var_uint32 = gport;
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
    uint32 control_lif;
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
            if (special_fields->special_fields[sf_idx].field_id == FLOW_S_F_RCH_P2P)
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
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
        uint32 flags = 0;
        bcm_gport_t gport;
        sal_memset(&gport_hw_resources, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
        flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_DPC_LOCAL_LIF_INGRESS;
        SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_algo_gpm_gport_from_lif
                                  (unit, flags, _SHR_CORE_ALL, (int) control_lif, &gport), _SHR_E_NOT_FOUND);
        SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert(unit, gport, _SHR_GPORT_TYPE_FLOW_LIF, &gport));
        special_field->shr_var_uint32 = (uint32) gport;
    }
    special_field->field_id = special_field_id;

exit:
    SHR_FUNC_EXIT;
}


/** function use the user input as profile and returns the profile index. in case first time profile is used
 *  configures the HW with the profile */
static shr_error_e
flow_special_field_src_address_profile_create(
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
                                       L3_INTF_MY_MAC_ETH_PREFIX(*profile_id));
        }

        /** Write to HW */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, profile_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
flow_special_field_src_address_profile_get(
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
                                   L3_INTF_MY_MAC_ETH_PREFIX(profile_id));

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

shr_error_e
flow_special_field_src_address_set(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    int field_idx,
    bcm_flow_special_fields_t * special_fields)
{
    const flow_special_field_info_t *curr_field;
    flow_special_fields_e sf_field_id = special_fields->special_fields[field_idx].field_id;
    int profile;

    SHR_FUNC_INIT_VARS(unit);

    curr_field = &flow_special_fields_db[sf_field_id];

    SHR_IF_ERR_EXIT(flow_special_field_src_address_profile_create
                    (unit, curr_field->mapped_dbal_field,
                     special_fields->special_fields[field_idx].shr_var_uint32,
                     special_fields->special_fields[field_idx].shr_var_uint8_arr, &profile));

    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_SRC_ADDR_PROFILE_LSB))
    {
        /*
         * Source address type full mac has special masking
         */
        SOURCE_ADDRESS_FULL_MASK(profile);
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_IDX, INST_SINGLE, profile);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flow_special_field_src_address_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    flow_special_fields_e special_field_id,
    bcm_flow_special_field_t * special_field)
{
    const flow_special_field_info_t *curr_field;
    uint32 profile;

    SHR_FUNC_INIT_VARS(unit);

    curr_field = &flow_special_fields_db[special_field_id];

    FLOW_FIELD_GET_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_SOURCE_IDX, INST_SINGLE, &profile));

    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_SRC_ADDR_PROFILE_LSB))
    {
        /*
         * Source address type full mac has special masking
         */
        SOURCE_ADDRESS_FULL_UNMASK(profile);
    }

    SHR_IF_ERR_EXIT(flow_special_field_src_address_profile_get
                    (unit, curr_field->mapped_dbal_field, profile, special_field));
    special_field->field_id = special_field_id;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_initiator_src_address_profile_delete(
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
                                       L3_INTF_MY_MAC_ETH_PREFIX(profile_id));
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
flow_special_field_src_address_destroy(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    flow_special_fields_e special_field_id)
{
    uint32 src_addr_profile;
    const flow_special_field_info_t *curr_field;

    SHR_FUNC_INIT_VARS(unit);

    curr_field = &flow_special_fields_db[special_field_id];

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SOURCE_IDX, INST_SINGLE, &src_addr_profile));

    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_SRC_ADDR_PROFILE_LSB))
    {
        /*
         * Source address type full mac has special masking
         */
        SOURCE_ADDRESS_FULL_UNMASK(src_addr_profile);
    }

    SHR_IF_ERR_EXIT(dnx_flow_initiator_src_address_profile_delete
                    (unit, curr_field->mapped_dbal_field, src_addr_profile));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_initiator_src_address_profile_replace(
    int unit,
    uint32 entry_handle_id,
    uint32 get_entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_flow_special_field_t * special_field)
{
    uint8 is_first_profile_ref;
    uint8 is_last_profile_ref;
    source_address_entry_t source_address_entry;
    uint32 new_profile, old_profile = SOURCE_ADDRESS_TABLE_DEFAULT_PROFILE;
    const flow_special_field_info_t *curr_field;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    curr_field = &flow_special_fields_db[special_field->field_id];

    SHR_IF_ERR_EXIT_EXCEPT_IF(dbal_entry_handle_value_field32_get(unit, get_entry_handle_id,
                                                                  DBAL_FIELD_SOURCE_IDX, INST_SINGLE,
                                                                  &old_profile), _SHR_E_NOT_FOUND);

    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_SRC_ADDR_PROFILE_LSB))
    {
        /*
         * Source address type full mac has special masking
         */
        SOURCE_ADDRESS_FULL_UNMASK(old_profile);
    }

    /** Initialize variables */
    is_first_profile_ref = 0;
    is_last_profile_ref = 0;
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
                     (int *) &new_profile, &is_first_profile_ref, &is_last_profile_ref));

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
                                       L3_INTF_MY_MAC_ETH_PREFIX(new_profile));
        }

        /** Write to HW */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, profile_handle_id, DBAL_COMMIT));
    }

    /** If last profile was deallocated remove the old entry from HW */
    if (is_last_profile_ref)
    {
        SHR_IF_ERR_EXIT(dnx_flow_initiator_src_address_profile_delete
                        (unit, curr_field->mapped_dbal_field, old_profile));
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
