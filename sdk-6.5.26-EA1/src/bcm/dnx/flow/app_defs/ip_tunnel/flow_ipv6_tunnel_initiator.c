#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/flow/flow.h>
#include "../../flow_def.h"
#include "flow_ipvx_tunnel_initiator.h"
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esem.h>

/** Supported initiator common fields */
#define VALID_IPV6_TUNNEL_INITIATOR_COMMON_FIELDS \
        BCM_FLOW_INITIATOR_ELEMENT_L2_EGRESS_INFO_VALID | \
        BCM_FLOW_INITIATOR_ELEMENT_QOS_MAP_ID_VALID | \
        BCM_FLOW_INITIATOR_ELEMENT_STAT_ID_VALID | \
        BCM_FLOW_INITIATOR_ELEMENT_STAT_PP_PROFILE_VALID | \
        BCM_FLOW_INITIATOR_ELEMENT_MTU_PROFILE_VALID | \
        BCM_FLOW_INITIATOR_ELEMENT_ACTION_GPORT_VALID | \
        BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID | \
        BCM_FLOW_INITIATOR_ELEMENT_CLASS_ID_VALID | \
        BCM_FLOW_INITIATOR_ELEMENT_TPID_CLASS_ID_VALID | \
        BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID | \
        BCM_FLOW_INITIATOR_ELEMENT_TTL_VALID

/** Supported initiator special fields */
#define VALID_IPV6_TUNNEL_INITIATOR_SPECIAL_FIELDS {FLOW_S_F_IPV6_TUNNEL_TYPE, \
                                                    FLOW_S_F_SRC_UDP_PORT, \
                                                    FLOW_S_F_DST_UDP_PORT, \
                                                    FLOW_S_F_IPVX_INIT_GRE_KEY_USE_LB, \
                                                    FLOW_S_F_IPVX_INIT_GRE_WITH_SN, \
                                                    FLOW_S_F_QOS_DSCP, \
                                                    FLOW_S_F_IPV6_SIP, \
                                                    FLOW_S_F_IPV6_DIP}

/** Supported initiator indications */
#define VALID_IPV6_TUNNEL_INITIATOR_INDICATIONS (SAL_BIT(FLOW_APP_INIT_IND_SRC_ADDR_PROFILE)  | \
                                                 SAL_BIT(FLOW_APP_INIT_IND_MANDATORY_L3_INTF) | \
                                                 SAL_BIT(FLOW_APP_INIT_IND_IGNORE_L3_INTF))

/***********************************
 * IPV6_TUNNEL_INITIATOR FUNCTIONS *
 ***********************************/

static shr_error_e
flow_ipv6_tunnel_initiator_tunnel_type_verify(
    int unit,
    bcm_tunnel_type_t tunnel_type)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (tunnel_type)
    {
        case bcmTunnelTypeIpAnyIn6:
        case bcmTunnelTypeGreAnyIn6:
        case bcmTunnelTypeUdp6:
        case bcmTunnelTypeVxlan6:
        case bcmTunnelTypeVxlan6Gpe:
            /** Do nothing as this is a valid tunnel types */
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Invalid tunnel type (%d) was set", (int) tunnel_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_ipv6_tunnel_initiator_tunnel_type_get(
    int unit,
    bcm_flow_special_fields_t * special_fields,
    bcm_tunnel_type_t * tunnel_type)
{
    bcm_flow_special_field_t special_field_data;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
    SHR_IF_ERR_EXIT(dnx_flow_special_field_data_get
                    (unit, special_fields, FLOW_S_F_IPV6_TUNNEL_TYPE, &special_field_data));
    (*tunnel_type) = (bcm_tunnel_type_t) special_field_data.symbol;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_ipv6_tunnel_initiator_dip6_hw_update(
    int unit,
    uint8 dip6[BCM_FLOW_SPECIAL_FIELD_UIN8_ARR_MAX_SIZE],
    bcm_encap_access_t ipv6_dip_encap_phase,
    uint32 arp_outlif,
    uint8 is_replace,
    uint32 *dip6_data_entry_outlif)
{
    uint32 entry_handle_id;
    lif_table_mngr_outlif_info_t lif_table_mngr_outlif_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** zero the mngr outlif info */
    sal_memset(&lif_table_mngr_outlif_info, 0, sizeof(lif_table_mngr_outlif_info_t));

    /** dip eedb data entry has no global lif  */
    lif_table_mngr_outlif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING;

    /** don't allocate lif if replace  */
    if (is_replace)
    {
        lif_table_mngr_outlif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_REPLACE;
    }

    /** set outlif phase */
    SHR_IF_ERR_EXIT(dnx_lif_lib_encap_access_to_logical_phase_convert
                    (unit, ipv6_dip_encap_phase, &lif_table_mngr_outlif_info.logical_phase));

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_IPV6_DATA_DIP, &entry_handle_id));

    /** Result type is superset, will be decided internally by lif table manager */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_SUPERSET_RESULT_TYPE);

    /** Set DATA fields */
    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_IPV6_DIP, INST_SINGLE, dip6);
    if (arp_outlif != DNX_ALGO_GPM_LIF_INVALID)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE, arp_outlif);
    }

    /** Write to HW */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_local_outlif_and_set_hw
                    (unit, entry_handle_id, (int *) dip6_data_entry_outlif, &lif_table_mngr_outlif_info));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_ipv6_tunnel_initiator_arp_outlif_get(
    int unit,
    bcm_flow_initiator_info_t * initiator_info,
    uint32 *arp_outlif)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    bcm_gport_t arp_id;

    SHR_FUNC_INIT_VARS(unit);

    /** get arp local outlif if exist */
    if (initiator_info->l3_intf_id)
    {
        BCM_L3_ITF_LIF_TO_GPORT_FLOW_LIF(arp_id, initiator_info->l3_intf_id);
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, arp_id, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &gport_hw_resources));
        (*arp_outlif) = gport_hw_resources.local_out_lif;
    }
    else
    {
        (*arp_outlif) = 0;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_ipv6_tunnel_initiator_ipv6_l3_intf_and_dip6_update(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_initiator_info_t * initiator_info,
    uint8 dip6[BCM_FLOW_SPECIAL_FIELD_UIN8_ARR_MAX_SIZE],
    uint32 next_outlif,
    uint8 is_replace)
{
    uint32 arp_outlif = 0;
    /** default case is to put IPv6 DIP in right after the IPv6 EEDB entry phase */
    bcm_encap_access_t ipv6_dip_encap_phase = initiator_info->encap_access + 1;

    SHR_FUNC_INIT_VARS(unit);

    /** getting the arp_outlif */
    SHR_IF_ERR_EXIT(flow_ipv6_tunnel_initiator_arp_outlif_get(unit, initiator_info, &arp_outlif));

    /** update the dip6 dbal table */
    SHR_IF_ERR_EXIT(flow_ipv6_tunnel_initiator_dip6_hw_update(unit, dip6, ipv6_dip_encap_phase,
                                                              arp_outlif, is_replace, &next_outlif));

    /** update the next_outlif pointer */
    if (next_outlif != DNX_ALGO_GPM_LIF_INVALID)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE, next_outlif);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_ipv6_tunnel_initiator_l3_intf_id_set(
    int unit,
    uint32 next_outlif,
    bcm_flow_initiator_info_t * initiator_info)
{
    bcm_gport_t gport = BCM_GPORT_INVALID;

    SHR_FUNC_INIT_VARS(unit);

    /** update the l3_intf (always set) */
    initiator_info->valid_elements_set |= BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID;

    if (next_outlif)
    {
        /** In case local lif is not found, return next pointer 0 */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS |
                                                    DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT, _SHR_CORE_ALL,
                                                    next_outlif, &gport));
        if (gport != BCM_GPORT_INVALID)
        {
            BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(initiator_info->l3_intf_id, gport);
        }
    }
    else
    {
        initiator_info->l3_intf_id = 0;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_ipv6_tunnel_initiator_ipv6_l3_intf_and_dip6_set(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields)
{
    uint32 next_outlif = 0;
    bcm_flow_special_field_t special_field_data;

    SHR_FUNC_INIT_VARS(unit);

    /** get the dip6 data */
    sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
    SHR_IF_ERR_EXIT(dnx_flow_special_field_data_get(unit, special_fields, FLOW_S_F_IPV6_DIP, &special_field_data));

    /** set the l3_intf and dip6 */
    SHR_IF_ERR_EXIT(flow_ipv6_tunnel_initiator_ipv6_l3_intf_and_dip6_update(unit, entry_handle_id, initiator_info,
                                                                            special_field_data.shr_var_uint8_arr,
                                                                            next_outlif, FALSE));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_ipv6_tunnel_initiator_dip6_data_entry_get(
    int unit,
    bcm_flow_special_fields_t * special_fields,
    uint32 next_outlif,
    uint32 *arp_outlif)
{
    uint32 entry_handle_id;
    uint8 dip6[BCM_FLOW_SPECIAL_FIELD_UIN8_ARR_MAX_SIZE];
    lif_table_mngr_outlif_info_t outlif_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** read DATA_ENTRY table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_IPV6_DATA_DIP, &entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_outlif_info(unit, next_outlif, entry_handle_id, &outlif_info));

    /** get the ipv6 dip */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                    (unit, entry_handle_id, DBAL_FIELD_IPV6_DIP, INST_SINGLE, dip6));

    SPECIAL_FIELD_UINT8_ARR_DATA_ADD(special_fields, FLOW_S_F_IPV6_DIP, dip6);

    /** get arp_outlif pointer - an ARP entry if exist */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER,
                                                        INST_SINGLE, arp_outlif));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_ipv6_tunnel_initiator_ipv6_l3_intf_and_dip6_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields)
{
    uint32 arp_outlif, next_outlif;

    SHR_FUNC_INIT_VARS(unit);

    /** getting the local_outlif */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER,
                                                        INST_SINGLE, &next_outlif));

    /** get eedb_ipv6_dip dbal_table data */
    SHR_IF_ERR_EXIT(flow_ipv6_tunnel_initiator_dip6_data_entry_get(unit, special_fields, next_outlif, &arp_outlif));

    /** get the l3_intf */
    SHR_IF_ERR_EXIT(flow_ipv6_tunnel_initiator_l3_intf_id_set(unit, next_outlif, initiator_info));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_ipv6_tunnel_initiator_ipv6_l3_intf_and_dip6_replace(
    int unit,
    uint32 entry_handle_id,
    uint32 next_outlif,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_initiator_info_t * prev_initiator_info,
    bcm_flow_special_fields_t * special_fields,
    bcm_flow_special_fields_t * prev_special_fields)
{
    bcm_flow_special_field_t dip6_data;
    uint8 dummy;
    SHR_FUNC_INIT_VARS(unit);

    /** get the dip6 data */
    SHR_IF_ERR_EXIT(flow_special_field_new_value_or_previous_if_not_exist_get(unit, special_fields, prev_special_fields,
                                                                              FLOW_S_F_IPV6_DIP, TRUE, &dip6_data,
                                                                              &dummy));

    /** if l3_intf was not re-set, take the old value */
    if (!(initiator_info->valid_elements_set & BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID))
    {
        initiator_info->l3_intf_id = prev_initiator_info->l3_intf_id;
    }

    /** set the l3_intf and dip6 */
    SHR_IF_ERR_EXIT(flow_ipv6_tunnel_initiator_ipv6_l3_intf_and_dip6_update(unit, entry_handle_id, initiator_info,
                                                                            dip6_data.shr_var_uint8_arr, next_outlif,
                                                                            TRUE));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_ipv6_tunnel_initiator_flow_tunnel_type_to_tunnel_type(
    int unit,
    flow_ipvx_tunnel_type_t flow_ipvx_tunnel_type,
    bcm_tunnel_type_t * tunnel_type)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (flow_ipvx_tunnel_type)
    {
        case TUNNEL_TYPE_RAW:
            *tunnel_type = bcmTunnelTypeIpAnyIn6;
            break;
        case TUNNEL_TYPE_GRE:
            *tunnel_type = bcmTunnelTypeGreAnyIn6;
            break;
        case TUNNEL_TYPE_UDP:
            *tunnel_type = bcmTunnelTypeUdp6;
            break;
        case TUNNEL_TYPE_VXLAN:
            *tunnel_type = bcmTunnelTypeVxlan6;
            break;
        case TUNNEL_TYPE_VXLAN_GPE:
            *tunnel_type = bcmTunnelTypeVxlan6Gpe;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "unexpected type (%d)", (uint32) flow_ipvx_tunnel_type);
            break;
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_ipv6_tunnel_initiator_special_fields_tunnel_type_add(
    int unit,
    uint32 entry_handle_id,
    flow_ipvx_tunnel_type_t flow_ipvx_tunnel_type,
    bcm_flow_special_fields_t * special_fields)
{
    bcm_tunnel_type_t tunnel_type;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(flow_ipv6_tunnel_initiator_flow_tunnel_type_to_tunnel_type
                    (unit, flow_ipvx_tunnel_type, &tunnel_type));
    SPECIAL_FIELD_SYMBOL_DATA_ADD(special_fields, FLOW_S_F_IPV6_TUNNEL_TYPE, tunnel_type);
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_ipv6_tunnel_initiator_lif_ahp_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_special_fields_t * special_fields)
{
    uint32 lif_ahp;
    flow_ipvx_tunnel_type_t flow_ipvx_tunnel_type;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(flow_ipvx_tunnel_initiator_flow_tunnel_type_and_lif_ahp_get
                    (unit, entry_handle_id, &flow_ipvx_tunnel_type, &lif_ahp));
    SHR_IF_ERR_EXIT(flow_ipv6_tunnel_initiator_special_fields_tunnel_type_add
                    (unit, entry_handle_id, flow_ipvx_tunnel_type, special_fields));
    SHR_IF_ERR_EXIT(flow_ipvx_tunnel_initiator_special_fields_tunnel_type_derivatives_add
                    (unit, flow_ipvx_tunnel_type, lif_ahp, special_fields));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_ipv6_tunnel_initiator_dip6_dbal_entry_delete(
    int unit,
    uint32 ipv6_entry_handle_id)
{
    uint32 dip6_data_entry_outlif;

    SHR_FUNC_INIT_VARS(unit);

    /** get the next_outlif */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, ipv6_entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER,
                                                        INST_SINGLE, &dip6_data_entry_outlif));

    /** clear the ipv6_dip dbal table */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_outlif_info_clear(unit, dip6_data_entry_outlif,
                                                         LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_ipv6_tunnel_initiator_create(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields)
{
    bcm_tunnel_type_t tunnel_type;

    SHR_FUNC_INIT_VARS(unit);

    /** extracting the tunnel_type from the special fields */
    SHR_IF_ERR_EXIT(flow_ipv6_tunnel_initiator_tunnel_type_get(unit, special_fields, &tunnel_type));

    /** setting the lif_ahp */
    SHR_IF_ERR_EXIT(flow_ipvx_tunnel_initiator_tunnel_type_and_derivatives_set
                    (unit, entry_handle_id, tunnel_type, special_fields));

    /** setting dip6 */
    SHR_IF_ERR_EXIT(flow_ipv6_tunnel_initiator_ipv6_l3_intf_and_dip6_set
                    (unit, entry_handle_id, initiator_info, special_fields));

    /** setting the esem command */
    SHR_IF_ERR_EXIT(flow_ipvx_tunnel_initiator_esem_command_set(unit, entry_handle_id, tunnel_type));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_ipv6_tunnel_initiator_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields)
{
    SHR_FUNC_INIT_VARS(unit);

    /** getting the lif_ahp and all the special fields derived from it */
    SHR_IF_ERR_EXIT(flow_ipv6_tunnel_initiator_lif_ahp_get(unit, entry_handle_id, special_fields));

    /** getting the dip6 value */
    SHR_IF_ERR_EXIT(flow_ipv6_tunnel_initiator_ipv6_l3_intf_and_dip6_get
                    (unit, entry_handle_id, initiator_info, special_fields));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_ipv6_tunnel_initiator_replace(
    int unit,
    uint32 current_entry_handle_id,
    uint32 prev_entry_handle_id,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields)
{
    bcm_flow_special_fields_t prev_special_fields = { 0 };
    bcm_flow_initiator_info_t prev_initiator_info = { 0 };
    bcm_tunnel_type_t tunnel_type;
    uint32 next_outlif;

    SHR_FUNC_INIT_VARS(unit);

    /** getting the values set previously */
    SHR_IF_ERR_EXIT(flow_ipv6_tunnel_initiator_get
                    (unit, prev_entry_handle_id, &prev_initiator_info, &prev_special_fields));

    /** get previous FLOW_S_F_IPV4_TUNNEL_TYPE value */
    SHR_IF_ERR_EXIT(flow_ipv6_tunnel_initiator_tunnel_type_get(unit, &prev_special_fields, &tunnel_type));

    /** verify the integrity of the replace */
    SHR_IF_ERR_EXIT(flow_ipvx_tunnel_initiator_replace_verify(unit, tunnel_type, special_fields, &prev_special_fields));

    /** checking and resetting the lif_ahp values */
    SHR_IF_ERR_EXIT(flow_ipvx_tunnel_initiator_tunnel_type_and_derivatives_replace
                    (unit, current_entry_handle_id, prev_entry_handle_id, tunnel_type, special_fields,
                     &prev_special_fields));

    /** getting the next_outlif value */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, prev_entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER,
                                                        INST_SINGLE, &next_outlif));

    /** checking and resetting the dip6 value */
    SHR_IF_ERR_EXIT(flow_ipv6_tunnel_initiator_ipv6_l3_intf_and_dip6_replace(unit, current_entry_handle_id, next_outlif,
                                                                             initiator_info, &prev_initiator_info,
                                                                             special_fields, &prev_special_fields));
    /** setting the esem command */
    SHR_IF_ERR_EXIT(flow_ipvx_tunnel_initiator_esem_command_set(unit, current_entry_handle_id, tunnel_type));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_ipv6_tunnel_initiator_delete(
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

    /** delete the dip6 entry */
    SHR_IF_ERR_EXIT(flow_ipv6_tunnel_initiator_dip6_dbal_entry_delete(unit, entry_handle_id));

exit:
    SHR_FUNC_EXIT;
}

/***********************************
 * IPV6_TUNNEL_INITIATOR CALLBACKS *
 ***********************************/

int
flow_ipv6_tunnel_initiator_app_specific_operations_cb(
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

    if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE)
        && (flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_SET))
    {
        SHR_IF_ERR_EXIT(flow_ipv6_tunnel_initiator_replace
                        (unit, current_entry_handle_id, prev_entry_handle_id,
                         (bcm_flow_initiator_info_t *) app_entry_data, special_fields));
    }
    else if (flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_DELETE)
    {
        SHR_IF_ERR_EXIT(flow_ipv6_tunnel_initiator_delete(unit, current_entry_handle_id));
    }
    else if (flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_SET)
    {
        SHR_IF_ERR_EXIT(flow_ipv6_tunnel_initiator_create(unit, current_entry_handle_id,
                                                          (bcm_flow_initiator_info_t *) app_entry_data,
                                                          special_fields));
    }
    else
    {
        SHR_IF_ERR_EXIT(flow_ipv6_tunnel_initiator_get(unit, current_entry_handle_id,
                                                       (bcm_flow_initiator_info_t *) app_entry_data, special_fields));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flow_ipv6_tunnel_initiator_verify_cb(
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
    bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) flow_info;
    int l3_intf_id;

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

    /** l3_intf verify */
    l3_intf_id = initiator_info->l3_intf_id;
    if (l3_intf_id == BCM_IF_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid value for initiator_info->l3_intf_id  (%d)", l3_intf_id);
    }

    /** extracting the tunnel_type from the special fields */
    rv = dnx_flow_special_field_data_get(unit, special_fields, FLOW_S_F_IPV6_TUNNEL_TYPE, &special_field_data);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    if (rv == _SHR_E_NONE)
    {
        tunnel_type = (bcm_tunnel_type_t) special_field_data.symbol;

        /** check the tunnel_type is valid */
        SHR_IF_ERR_EXIT(flow_ipv6_tunnel_initiator_tunnel_type_verify(unit, tunnel_type));
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
flow_ipv6_is_valid(
    int unit)
{
    if (dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc))
    {
        return TRUE;
    }
    return FALSE;

}
const dnx_flow_app_config_t dnx_flow_app_ipv6_tunnel_initiator = {
    /*
     * Application name
     */
    FLOW_APP_NAME_IPV6_INITIATOR,

    /** related DBAL table */
    FLOW_LIF_DBAL_TABLE_SET(DBAL_TABLE_EEDB_IPV6_TUNNEL),

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
    VALID_IPV6_TUNNEL_INITIATOR_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_IPV6_TUNNEL_INITIATOR_SPECIAL_FIELDS,

    /*
     * Bitmap for supported flow handler flags
     */
    FLOW_APP_BASE_FLAGS,

    /*
     * Indications
     */
    VALID_IPV6_TUNNEL_INITIATOR_INDICATIONS,

    /*
     * verify cb
     */
    flow_ipv6_tunnel_initiator_verify_cb,

    /*
     * app_specific_operations_cb
     */
    flow_ipv6_tunnel_initiator_app_specific_operations_cb,

    /*
     * Generic callback for marking selectable result types - result_type_select_cb
     */
    NULL,

    /*
     *  Generic callback, used to indicate if the app is valid. usually the app should be
     *  valid according to dnx_data
     */
    flow_ipv6_is_valid
};
