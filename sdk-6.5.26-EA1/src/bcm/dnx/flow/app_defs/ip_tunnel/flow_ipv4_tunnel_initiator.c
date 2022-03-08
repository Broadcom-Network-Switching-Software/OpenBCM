#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/flow/flow.h>
#include "../../flow_def.h"
#include "flow_ipvx_tunnel_initiator.h"
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_tunnel_access.h>
#include <bcm_int/dnx/init/init_pp.h>
#include "../../../tunnel/tunnel_init.h"
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esem.h>

/** Supported initiator common fields */
#define VALID_IPV4_TUNNEL_INITIATOR_COMMON_FIELDS \
        BCM_FLOW_INITIATOR_ELEMENT_STAT_ID_VALID | \
        BCM_FLOW_INITIATOR_ELEMENT_STAT_PP_PROFILE_VALID | \
        BCM_FLOW_INITIATOR_ELEMENT_QOS_MAP_ID_VALID | \
        BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID | \
        BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID | \
        BCM_FLOW_INITIATOR_ELEMENT_L2_EGRESS_INFO_VALID | \
        BCM_FLOW_INITIATOR_ELEMENT_MTU_PROFILE_VALID | \
        BCM_FLOW_INITIATOR_ELEMENT_ACTION_GPORT_VALID | \
        BCM_FLOW_INITIATOR_ELEMENT_TPID_CLASS_ID_VALID | \
        BCM_FLOW_INITIATOR_ELEMENT_CLASS_ID_VALID | \
        BCM_FLOW_INITIATOR_ELEMENT_TTL_VALID

/** Supported initiator special fields */
#define VALID_IPV4_TUNNEL_INITIATOR_SPECIAL_FIELDS {FLOW_S_F_IPV4_DIP, \
                                                    FLOW_S_F_IPV4_SIP, \
                                                    FLOW_S_F_IPV4_TUNNEL_TYPE, \
                                                    FLOW_S_F_SRC_UDP_PORT, \
                                                    FLOW_S_F_DST_UDP_PORT, \
                                                    FLOW_S_F_IPV4_HEADER_DF, \
                                                    FLOW_S_F_IPVX_INIT_GRE_KEY_USE_LB, \
                                                    FLOW_S_F_IPVX_INIT_GRE_WITH_SN, \
                                                    FLOW_S_F_QOS_DSCP}

/** Supported initiator indications */
#define VALID_IPV4_TUNNEL_INITIATOR_INDICATIONS SAL_BIT(FLOW_APP_INIT_IND_SRC_ADDR_PROFILE)

/***********************************
 * IPV4_TUNNEL_INITIATOR FUNCTIONS *
 ***********************************/

static shr_error_e
flow_ipv4_tunnel_initiator_tunnel_type_verify(
    int unit,
    bcm_tunnel_type_t tunnel_type)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (tunnel_type)
    {
        case bcmTunnelTypeIpAnyIn4:
        case bcmTunnelTypeGreAnyIn4:
        case bcmTunnelTypeUdp:
        case bcmTunnelTypeVxlan:
        case bcmTunnelTypeVxlanGpe:
            /** Do nothing as this is a valid tunnel types */
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Invalid tunnel type (%d) was set", (int) tunnel_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flow_ipv4_tunnel_initiator_tunnel_type_from_special_fields_get(
    int unit,
    bcm_flow_special_fields_t * special_fields,
    bcm_tunnel_type_t * tunnel_type)
{
    bcm_flow_special_field_t special_field_data;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
    SHR_IF_ERR_EXIT(dnx_flow_special_field_data_get
                    (unit, special_fields, FLOW_S_F_IPV4_TUNNEL_TYPE, &special_field_data));
    (*tunnel_type) = (bcm_tunnel_type_t) special_field_data.symbol;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_ipv4_tunnel_initiator_create(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_special_fields_t * special_fields)
{
    bcm_tunnel_type_t tunnel_type;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(flow_ipv4_tunnel_initiator_tunnel_type_from_special_fields_get(unit, special_fields, &tunnel_type));
    SHR_IF_ERR_EXIT(flow_ipvx_tunnel_initiator_tunnel_type_and_derivatives_set
                    (unit, entry_handle_id, tunnel_type, special_fields));
    SHR_IF_ERR_EXIT(flow_ipvx_tunnel_initiator_esem_command_set(unit, entry_handle_id, tunnel_type));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_ipv4_tunnel_initiator_flow_tunnel_type_to_tunnel_type(
    int unit,
    flow_ipvx_tunnel_type_t flow_ipvx_tunnel_type,
    bcm_tunnel_type_t * tunnel_type)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (flow_ipvx_tunnel_type)
    {
        case TUNNEL_TYPE_RAW:
            *tunnel_type = bcmTunnelTypeIpAnyIn4;
            break;
        case TUNNEL_TYPE_GRE:
            *tunnel_type = bcmTunnelTypeGreAnyIn4;
            break;
        case TUNNEL_TYPE_UDP:
            *tunnel_type = bcmTunnelTypeUdp;
            break;
        case TUNNEL_TYPE_VXLAN:
            *tunnel_type = bcmTunnelTypeVxlan;
            break;
        case TUNNEL_TYPE_VXLAN_GPE:
            *tunnel_type = bcmTunnelTypeVxlanGpe;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "unexpected type (%d)", (uint32) flow_ipvx_tunnel_type);
            break;
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_ipv4_tunnel_initiator_special_fields_tunnel_type_add(
    int unit,
    uint32 entry_handle_id,
    flow_ipvx_tunnel_type_t flow_ipvx_tunnel_type,
    bcm_flow_special_fields_t * special_fields)
{
    bcm_tunnel_type_t tunnel_type;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(flow_ipv4_tunnel_initiator_flow_tunnel_type_to_tunnel_type
                    (unit, flow_ipvx_tunnel_type, &tunnel_type));
    SPECIAL_FIELD_SYMBOL_DATA_ADD(special_fields, FLOW_S_F_IPV4_TUNNEL_TYPE, tunnel_type);
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_ipv4_tunnel_initiator_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_special_fields_t * special_fields)
{
    uint32 lif_ahp;
    flow_ipvx_tunnel_type_t flow_ipvx_tunnel_type;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(flow_ipvx_tunnel_initiator_flow_tunnel_type_and_lif_ahp_get
                    (unit, entry_handle_id, &flow_ipvx_tunnel_type, &lif_ahp));
    SHR_IF_ERR_EXIT(flow_ipv4_tunnel_initiator_special_fields_tunnel_type_add
                    (unit, entry_handle_id, flow_ipvx_tunnel_type, special_fields));
    SHR_IF_ERR_EXIT(flow_ipvx_tunnel_initiator_special_fields_tunnel_type_derivatives_add
                    (unit, flow_ipvx_tunnel_type, lif_ahp, special_fields));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_ipv4_tunnel_initiator_replace(
    int unit,
    uint32 current_entry_handle_id,
    uint32 prev_entry_handle_id,
    bcm_flow_special_fields_t * special_fields)
{
    bcm_flow_special_fields_t prev_special_fields = { 0 };
    bcm_tunnel_type_t tunnel_type;

    SHR_FUNC_INIT_VARS(unit);

    /** getting the values set previously */
    SHR_IF_ERR_EXIT(flow_ipv4_tunnel_initiator_get(unit, prev_entry_handle_id, &prev_special_fields));

    /** get previous FLOW_S_F_IPV4_TUNNEL_TYPE value */
    SHR_IF_ERR_EXIT(flow_ipv4_tunnel_initiator_tunnel_type_from_special_fields_get
                    (unit, &prev_special_fields, &tunnel_type));

    /** verify the integrity of the replace */
    SHR_IF_ERR_EXIT(flow_ipvx_tunnel_initiator_replace_verify(unit, tunnel_type, special_fields, &prev_special_fields));

    /** checking and resetting the lif_ahp values */
    SHR_IF_ERR_EXIT(flow_ipvx_tunnel_initiator_tunnel_type_and_derivatives_replace
                    (unit, current_entry_handle_id, prev_entry_handle_id, tunnel_type, special_fields,
                     &prev_special_fields));
    SHR_IF_ERR_EXIT(flow_ipvx_tunnel_initiator_esem_command_set(unit, current_entry_handle_id, tunnel_type));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_ipv4_tunnel_initiator_delete(
    int unit,
    uint32 entry_handle_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /** if needed, delete the udp profile */
    SHR_IF_ERR_EXIT(flow_ipvx_tunnel_initiator_udp_profile_delete(unit, entry_handle_id));

    /** if needed, delete the esem command profile */
    if (!dnx_data_esem.access_cmd.esem_cmd_predefine_allocations_get(unit))
    {
        SHR_IF_ERR_EXIT(flow_ipvx_tunnel_initiator_esem_command_profile_delete(unit, entry_handle_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/***********************************
 * IPV4_TUNNEL_INITIATOR CALLBACKS *
 ***********************************/

int
flow_ipv4_tunnel_initiator_app_specific_operations_cb(
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

    if (special_fields == NULL)
    {
        SHR_EXIT();
    }

    /** if replace, first get the profile, update the enablers, and then read the new inputs */
    if (flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_SET)
    {
        if (!_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE))
        {
            SHR_IF_ERR_EXIT(flow_ipv4_tunnel_initiator_create(unit, current_entry_handle_id, special_fields));
        }
        else
        {
            SHR_IF_ERR_EXIT(flow_ipv4_tunnel_initiator_replace
                            (unit, current_entry_handle_id, prev_entry_handle_id, special_fields));
        }
    }
    else if (flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_DELETE)
    {
        SHR_IF_ERR_EXIT(flow_ipv4_tunnel_initiator_delete(unit, current_entry_handle_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(flow_ipv4_tunnel_initiator_get(unit, current_entry_handle_id, special_fields));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flow_ipv4_tunnel_initiator_verify_cb(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_cmd_control_info_t * flow_cmd_control,
    void *flow_info,
    bcm_flow_special_fields_t * special_fields,
    uint32 get_entry_handle)
{
    shr_error_e rv;
    bcm_flow_special_field_t special_field_data;
    bcm_tunnel_type_t tunnel_type = bcmTunnelTypeNone;

    SHR_FUNC_INIT_VARS(unit);

    /** relevant only for create and replace */
    if ((flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_GET) || (flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_DELETE))
    {
        SHR_EXIT();
    }

    /** relevant only if special fields exists */
    if (special_fields == NULL)
    {
        SHR_EXIT();
    }

    /** extracting the tunnel_type from the special fields */
    rv = dnx_flow_special_field_data_get(unit, special_fields, FLOW_S_F_IPV4_TUNNEL_TYPE, &special_field_data);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    if (rv == _SHR_E_NONE)
    {
        tunnel_type = (bcm_tunnel_type_t) special_field_data.symbol;

        /** check the tunnel_type is valid */
        SHR_IF_ERR_EXIT(flow_ipv4_tunnel_initiator_tunnel_type_verify(unit, tunnel_type));
    }

    /** checks relevant only for create and not for replace */
    if (!_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE))
    {
        /** verifying udp port set \ not set according to tunnel type requirement */
        SHR_IF_ERR_EXIT(flow_ipvx_tunnel_initiator_udp_ports_verify(unit, tunnel_type, special_fields, FALSE));

        /** verifying gre flag set \ not set according to tunnel type requirement */
        SHR_IF_ERR_EXIT(flow_ipvx_tunnel_initiator_gre_flags_verify(unit, tunnel_type, special_fields, NULL, FALSE));
    }

exit:
    SHR_FUNC_EXIT;
}

int
flow_ipv4_is_valid(
    int unit)
{
    if (dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc))
    {
        return TRUE;
    }

    return TRUE;
}

const dnx_flow_app_config_t dnx_flow_app_ipv4_tunnel_initiator = {
    /*
     * Application name
     */
    FLOW_APP_NAME_IPV4_INITIATOR,

    /** related DBAL table */
    FLOW_LIF_DBAL_TABLE_SET(DBAL_TABLE_EEDB_IPV4_TUNNEL),

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
    VALID_IPV4_TUNNEL_INITIATOR_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_IPV4_TUNNEL_INITIATOR_SPECIAL_FIELDS,

    /*
     * Bitmap for supported flow handler flags
     */
    FLOW_APP_BASE_FLAGS,

    /*
     * Indications
     */
    VALID_IPV4_TUNNEL_INITIATOR_INDICATIONS,

    /*
     * verify cb
     */
    flow_ipv4_tunnel_initiator_verify_cb,

    /*
     * app_specific_operations_cb
     */
    flow_ipv4_tunnel_initiator_app_specific_operations_cb,

    /*
     * Generic callback for marking selectable result types - result_type_select_cb
     */
    NULL,

    /*
     *  Generic callback, used to indicate if the app is valid. usually the app should be
     *  valid according to dnx_data
     */
    flow_ipv4_is_valid
};
