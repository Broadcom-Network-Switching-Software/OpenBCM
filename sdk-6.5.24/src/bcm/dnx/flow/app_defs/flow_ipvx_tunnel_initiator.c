#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/flow/flow.h>
#include "../flow_def.h"
#include "flow_ipvx_tunnel_initiator.h"
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_tunnel_access.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/init/init_pp.h>
#include "../../tunnel/tunnel_init.h"
#include <bcm_int/dnx/port/port_esem.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>

static shr_error_e
flow_ipvx_tunnel_udp_port_integrity_verify(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    flow_special_fields_e field_id,
    bcm_flow_special_fields_t * special_fields,
    uint8 is_replace)
{
    shr_error_e rv;
    bcm_flow_special_field_t special_field_data;

    SHR_FUNC_INIT_VARS(unit);

    /** verify the src udp port integrity */
    sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
    rv = dnx_flow_special_field_data_get(unit, special_fields, field_id, &special_field_data);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    if ((rv == _SHR_E_NONE) && ((tunnel_type != bcmTunnelTypeUdp) && (tunnel_type != bcmTunnelTypeUdp6)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Cannot set special field %s for tunnel type %d, only for bcmTunnelTypeUdp or bcmTunnelTypeUdp6",
                     dnx_flow_special_field_to_string(unit, field_id), tunnel_type);
    }
    if ((!is_replace) && (rv == _SHR_E_NOT_FOUND)
        && ((tunnel_type == bcmTunnelTypeUdp) || (tunnel_type == bcmTunnelTypeUdp6)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Must set special field %s for tunnel type bcmTunnelTypeUdp or bcmTunnelTypeUdp6",
                     dnx_flow_special_field_to_string(unit, field_id));
    }
    if (special_field_data.is_clear)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Cannot delete special field %s for tunnel typebcmTunnelTypeUdp or bcmTunnelTypeUdp6",
                     dnx_flow_special_field_to_string(unit, field_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/** verify that the udp ports were added correctly */
shr_error_e
flow_ipvx_tunnel_initiator_udp_ports_verify(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    bcm_flow_special_fields_t * special_fields,
    uint8 is_replace)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(flow_ipvx_tunnel_udp_port_integrity_verify
                    (unit, tunnel_type, FLOW_S_F_SRC_UDP_PORT, special_fields, is_replace));

    SHR_IF_ERR_EXIT(flow_ipvx_tunnel_udp_port_integrity_verify
                    (unit, tunnel_type, FLOW_S_F_DST_UDP_PORT, special_fields, is_replace));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_special_field_enabler_get(
    int unit,
    bcm_flow_special_fields_t * special_fields,
    flow_special_fields_e field_id,
    uint8 *enabler_set)
{
    shr_error_e rv;
    bcm_flow_special_field_t special_field_data;

    SHR_FUNC_INIT_VARS(unit);

    rv = dnx_flow_special_field_data_get(unit, special_fields, field_id, &special_field_data);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    if (rv == _SHR_E_NOT_FOUND)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    (*enabler_set) = special_field_data.is_clear ? FALSE : TRUE;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_ipvx_tunnel_gre_flag_enabler_new_value_or_previous_if_not_exist_get(
    int unit,
    bcm_flow_special_fields_t * special_fields,
    bcm_flow_special_fields_t * prev_special_fields,
    flow_special_fields_e field_id,
    uint8 is_replace,
    uint8 *enabler_set)
{
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    /** in case of replace need first to get the old value */
    if (is_replace)
    {
        rv = flow_special_field_enabler_get(unit, prev_special_fields, field_id, enabler_set);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    }

    /** getting the new value (if it was set) */
    rv = flow_special_field_enabler_get(unit, special_fields, field_id, enabler_set);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);

    SHR_SET_CURRENT_ERR(rv);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_ipvx_tunnel_gre_specific_enabler_verify(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    flow_special_fields_e field_id,
    uint8 is_gre_updated,
    uint8 enabler_set)
{
    SHR_FUNC_INIT_VARS(unit);

    if (is_gre_updated && (!enabler_set))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "In create operation, cannot clear special field %s",
                     dnx_flow_special_field_to_string(unit, field_id));
    }
    if (((tunnel_type != bcmTunnelTypeGreAnyIn4) && (tunnel_type != bcmTunnelTypeGreAnyIn6)) && enabler_set)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Cannot set special field %s for tunnel type %d, only for bcmTunnelTypeGreAnyIn4 or bcmTunnelTypeGreAnyIn6",
                     dnx_flow_special_field_to_string(unit, field_id), tunnel_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/** verify that the gre flags were added correctly */
shr_error_e
flow_ipvx_tunnel_initiator_gre_flags_verify(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    bcm_flow_special_fields_t * special_fields,
    bcm_flow_special_fields_t * prev_special_fields,
    uint8 is_replace)
{
    shr_error_e rv_gre_key, rv_gre_with_sn;
    uint8 is_gre_key_enabler_set = FALSE, is_gre_with_sn_enabler_set = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    /** get FLOW_S_F_IPVX_INIT_GRE_KEY_USE_LB new value, in case it was not set, old value will be kept */
    rv_gre_key =
        flow_ipvx_tunnel_gre_flag_enabler_new_value_or_previous_if_not_exist_get(unit, special_fields,
                                                                                 prev_special_fields,
                                                                                 FLOW_S_F_IPVX_INIT_GRE_KEY_USE_LB,
                                                                                 is_replace, &is_gre_key_enabler_set);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv_gre_key, _SHR_E_NOT_FOUND);

    /** get FLOW_S_F_IPVX_INIT_GRE_WITH_SN new value, in case it was not set, old value will be kept */
    rv_gre_with_sn =
        flow_ipvx_tunnel_gre_flag_enabler_new_value_or_previous_if_not_exist_get(unit, special_fields,
                                                                                 prev_special_fields,
                                                                                 FLOW_S_F_IPVX_INIT_GRE_WITH_SN,
                                                                                 is_replace,
                                                                                 &is_gre_with_sn_enabler_set);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv_gre_key, _SHR_E_NOT_FOUND);

    /** verify FLOW_S_F_IPVX_INIT_GRE_KEY_USE_LB is set only for tunnel type GreAnyIn4 */
    SHR_IF_ERR_EXIT(flow_ipvx_tunnel_gre_specific_enabler_verify
                    (unit, tunnel_type, FLOW_S_F_IPVX_INIT_GRE_KEY_USE_LB, UTILEX_NUM2BOOL(rv_gre_key == _SHR_E_NONE),
                     is_gre_key_enabler_set));

    /** verify FLOW_S_F_IPVX_INIT_GRE_WITH_SN is set only for tunnel type GreAnyIn4 */
    SHR_IF_ERR_EXIT(flow_ipvx_tunnel_gre_specific_enabler_verify
                    (unit, tunnel_type, FLOW_S_F_IPVX_INIT_GRE_WITH_SN, UTILEX_NUM2BOOL(rv_gre_with_sn == _SHR_E_NONE),
                     is_gre_with_sn_enabler_set));

    /** verify that FLOW_S_F_IPVX_INIT_GRE_KEY_USE_LB were not both set FLOW_S_F_IPVX_INIT_GRE_WITH_SN */
    if (is_gre_key_enabler_set && is_gre_with_sn_enabler_set)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Cannot enable both FLOW_S_F_IPVX_INIT_GRE_KEY_USE_LB and FLOW_S_F_IPVX_INIT_GRE_WITH_SN");
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_ipvx_tunnel_initiator_udp_profile_allocate_and_hw_set(
    int unit,
    udp_ports_t * udp_ports)
{
    int udp_ports_profile;
    uint8 is_first_reference;

    SHR_FUNC_INIT_VARS(unit);

    /** updating udp profile */
    SHR_IF_ERR_EXIT(algo_tunnel_db.udp_ports_profile.allocate_single(unit, 0, udp_ports, NULL, &udp_ports_profile,
                                                                     &is_first_reference));

    if (is_first_reference)
    {
        SHR_IF_ERR_EXIT(dnx_egress_additional_header_map_tables_configuration_udp_ports_update
                        (unit, (uint32) udp_ports->src_port, (uint32) udp_ports->dst_port, udp_ports_profile));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_ipvx_tunnel_initiator_udp_profile_replace_and_hw_update(
    int unit,
    udp_ports_t * udp_ports,
    uint32 prev_entry_handle_id)
{
    int udp_ports_profile;
    uint8 is_first_reference, is_last_reference, is_field_valid;
    uint32 old_udp_ports_profile;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, prev_entry_handle_id, DBAL_FIELD_LIF_AHP_UDP, &is_field_valid, &old_udp_ports_profile));

    SHR_IF_ERR_EXIT(algo_tunnel_db.udp_ports_profile.exchange
                    (unit, 0, udp_ports, old_udp_ports_profile, NULL, &udp_ports_profile, &is_first_reference,
                     &is_last_reference));

    if (is_first_reference)
    {
        SHR_IF_ERR_EXIT(dnx_egress_additional_header_map_tables_configuration_udp_ports_update
                        (unit, (uint32) udp_ports->src_port, (uint32) udp_ports->dst_port, udp_ports_profile));
    }
    if (is_last_reference)
    {
        SHR_IF_ERR_EXIT(dnx_egress_additional_header_map_tables_configuration_udp_ports_clear
                        (unit, old_udp_ports_profile));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flow_ipvx_tunnel_initiator_gre_flag_get(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    bcm_flow_special_fields_t * special_fields,
    bcm_flow_special_fields_t * prev_special_fields,
    uint8 is_replace,
    uint32 *gre_flag)
{
    shr_error_e rv;
    uint8 enabler_set = FALSE;
    bcm_flow_special_field_t special_field_data;

    SHR_FUNC_INIT_VARS(unit);

    /** getting FLOW_S_F_IPVX_INIT_GRE_KEY_USE_LB */
    rv = flow_special_field_new_value_or_previous_if_not_exist_get(unit, special_fields, prev_special_fields,
                                                                   FLOW_S_F_IPVX_INIT_GRE_KEY_USE_LB, is_replace,
                                                                   &special_field_data, &enabler_set);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);

    /** since the special field is an enabler, if the field was found (old or new), then set it */
    if ((rv != _SHR_E_NOT_FOUND) && (!special_field_data.is_clear))
    {
        *gre_flag = BCM_TUNNEL_INIT_GRE_KEY_USE_LB;
    }
    /** assuming both flags cannot be set at the same time, if one was set can return */
    if (enabler_set)
    {
        SHR_EXIT();
    }

    /** getting FLOW_S_F_IPVX_INIT_GRE_WITH_SN */
    rv = flow_special_field_new_value_or_previous_if_not_exist_get(unit, special_fields, prev_special_fields,
                                                                   FLOW_S_F_IPVX_INIT_GRE_WITH_SN, is_replace,
                                                                   &special_field_data, &enabler_set);

    if ((rv != _SHR_E_NOT_FOUND) && (!special_field_data.is_clear))
    {
        *gre_flag = BCM_TUNNEL_INIT_GRE_WITH_SN;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_ipvx_tunnel_initiator_lif_ahp_field_and_value_calculate(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    uint32 gre_flag,
    udp_ports_t * udp_ports,
    dbal_fields_e * lif_ahp_field,
    int *lif_ahp_data)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (tunnel_type)
    {
        case bcmTunnelTypeGreAnyIn4:
        case bcmTunnelTypeGreAnyIn6:
            *lif_ahp_field = DBAL_FIELD_LIF_AHP_GRE_GENEVE;
            if (gre_flag == BCM_TUNNEL_INIT_GRE_KEY_USE_LB)
            {
                *lif_ahp_data = DBAL_ENUM_FVAL_LIF_AHP_GRE_GENEVE_LIF_AHP_GRE8_W_KEY;
            }
            else if (gre_flag == BCM_TUNNEL_INIT_GRE_WITH_SN)
            {
                *lif_ahp_data = DBAL_ENUM_FVAL_LIF_AHP_GRE_GENEVE_LIF_AHP_GRE8_W_SN;
            }
            else
            {
                *lif_ahp_data = DBAL_ENUM_FVAL_LIF_AHP_GRE_GENEVE_LIF_AHP_NONE;
            }
            break;
        case bcmTunnelTypeUdp:
        case bcmTunnelTypeUdp6:
            *lif_ahp_field = DBAL_FIELD_LIF_AHP_UDP;
            if ((udp_ports->src_port == 0) && (udp_ports->dst_port == 0))
            {
                *lif_ahp_data = DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_NONE;
            }
            else
            {
                SHR_IF_ERR_EXIT(algo_tunnel_db.udp_ports_profile.profile_get(unit, udp_ports, lif_ahp_data));
            }
            break;
        case bcmTunnelTypeIpAnyIn4:
        case bcmTunnelTypeIpAnyIn6:
            *lif_ahp_field = DBAL_FIELD_LIF_AHP_IP;
            *lif_ahp_data = DBAL_ENUM_FVAL_LIF_AHP_IP_LIF_AHP_NONE;
            break;
        case bcmTunnelTypeVxlan:
        case bcmTunnelTypeVxlan6:
            *lif_ahp_field = DBAL_FIELD_LIF_AHP_VXLAN_VXLAN_GPE;
            *lif_ahp_data = DBAL_ENUM_FVAL_LIF_AHP_VXLAN_VXLAN_GPE_LIF_AHP_VXLAN;
            break;
        case bcmTunnelTypeVxlanGpe:
        case bcmTunnelTypeVxlan6Gpe:
            *lif_ahp_field = DBAL_FIELD_LIF_AHP_VXLAN_VXLAN_GPE;
            *lif_ahp_data = DBAL_ENUM_FVAL_LIF_AHP_VXLAN_VXLAN_GPE_LIF_AHP_VXLAN_GPE;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid tunnel_type (%d)", tunnel_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flow_ipvx_tunnel_initiator_dbal_field_lif_ahp_set(
    int unit,
    uint32 entry_handle_id,
    bcm_tunnel_type_t tunnel_type,
    uint32 gre_flag,
    udp_ports_t * udp_ports)
{
    dbal_fields_e lif_ahp_field;
    int lif_ahp_data;

    SHR_FUNC_INIT_VARS(unit);

    /** get which lif_ahp dbal field should be set and what its value should be */
    SHR_IF_ERR_EXIT(flow_ipvx_tunnel_initiator_lif_ahp_field_and_value_calculate
                    (unit, tunnel_type, gre_flag, udp_ports, &lif_ahp_field, &lif_ahp_data));

    dbal_entry_value_field32_set(unit, entry_handle_id, lif_ahp_field, INST_SINGLE, lif_ahp_data);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flow_ipvx_tunnel_initiator_lif_ahp_set(
    int unit,
    uint32 entry_handle_id,
    bcm_tunnel_type_t tunnel_type,
    bcm_flow_special_fields_t * special_fields)
{
    bcm_flow_special_field_t special_field_data;
    uint32 gre_flag = 0;
    udp_ports_t udp_ports = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    /** if the tunnel type is UDP - extract the src and dst udp port */
    if ((tunnel_type == bcmTunnelTypeUdp) || (tunnel_type == bcmTunnelTypeUdp6))
    {
        /** extract src_udp_port if exists */
        sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
        SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_special_field_data_get
                                  (unit, special_fields, FLOW_S_F_SRC_UDP_PORT, &special_field_data), _SHR_E_NOT_FOUND);
        udp_ports.src_port = special_field_data.shr_var_uint32;

        /** extract dst_udp_port if exists */
        sal_memset(&special_field_data, 0, sizeof(bcm_flow_special_field_t));
        SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_special_field_data_get
                                  (unit, special_fields, FLOW_S_F_DST_UDP_PORT, &special_field_data), _SHR_E_NOT_FOUND);
        udp_ports.dst_port = special_field_data.shr_var_uint32;

        /** allocate the udp profile */
        SHR_IF_ERR_EXIT(flow_ipvx_tunnel_initiator_udp_profile_allocate_and_hw_set(unit, &udp_ports));
    }

    /** get the gre_flag from the special fields */
    if ((tunnel_type == bcmTunnelTypeGreAnyIn4) || (tunnel_type == bcmTunnelTypeGreAnyIn6))
    {
        SHR_IF_ERR_EXIT(flow_ipvx_tunnel_initiator_gre_flag_get
                        (unit, tunnel_type, special_fields, NULL, FALSE, &gre_flag));
    }

    /** setting the lif_ahp data */
    SHR_IF_ERR_EXIT(flow_ipvx_tunnel_initiator_dbal_field_lif_ahp_set
                    (unit, entry_handle_id, tunnel_type, gre_flag, &udp_ports));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flow_ipvx_tunnel_initiator_esem_command_set(
    int unit,
    uint32 entry_handle_id,
    bcm_tunnel_type_t tunnel_type)
{
    uint32 esem_cmd_hw_value = 0;

    SHR_FUNC_INIT_VARS(unit);

    if ((tunnel_type == bcmTunnelTypeVxlan) || (tunnel_type == bcmTunnelTypeVxlanGpe) ||
            (tunnel_type == bcmTunnelTypeVxlan6) || (tunnel_type == bcmTunnelTypeVxlan6Gpe))
    {
        uint8 dummy;

        /** Allocate an esem access cmd and configure the cmd table. */
        SHR_IF_ERR_EXIT(dnx_tunnel_init_esem_command_alloc_and_set_hw
                        (unit, 0, 0, TUNNEL_INIT_ESEM_ACCESS_TYPE_VXLAN_VNI,
                         dnx_data_esem.access_cmd.no_action_get(unit), &esem_cmd_hw_value, (uint8 *) &dummy));

        /** set the dbal_field */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ESEM_COMMAND, INST_SINGLE, esem_cmd_hw_value);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_ipvx_tunnel_initiator_udp_profile_free_and_hw_clear(
    int unit,
    uint32 entry_handle_id)
{
    uint8 is_last_reference, is_field_valid;
    uint32 old_udp_ports_profile;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, entry_handle_id, DBAL_FIELD_LIF_AHP_UDP, &is_field_valid, &old_udp_ports_profile));

    SHR_IF_ERR_EXIT(algo_tunnel_db.udp_ports_profile.free_single(unit, old_udp_ports_profile, &is_last_reference));

    if (is_last_reference)
    {
        SHR_IF_ERR_EXIT(dnx_egress_additional_header_map_tables_configuration_udp_ports_clear
                        (unit, old_udp_ports_profile));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flow_ipvx_tunnel_initiator_udp_profile_delete(
    int unit,
    uint32 entry_handle_id)
{
    uint8 is_field_valid;
    uint32 lif_ahp_data;

    SHR_FUNC_INIT_VARS(unit);

    /** in case of udp tunnel - check and free udp profile if required */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, entry_handle_id, DBAL_FIELD_LIF_AHP_UDP, &is_field_valid, &lif_ahp_data));

    /** if tunnel type was udp (field will be valid) and we used a udp profile (value of lif_ahp_data is valid) - need to release the udp profile */
    if ((is_field_valid) && (lif_ahp_data != DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_NONE))
    {
        SHR_IF_ERR_EXIT(flow_ipvx_tunnel_initiator_udp_profile_free_and_hw_clear(unit, entry_handle_id));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flow_ipvx_tunnel_initiator_esem_command_profile_delete(
    int unit,
    uint32 entry_handle_id)
{
    uint8 is_field_valid;
    uint32 esem_cmd_profile;

    SHR_FUNC_INIT_VARS(unit);

    /** if tunnel type was vxlan (field will be valid) and we used a esem profile - need to release the esem profile */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, entry_handle_id, DBAL_FIELD_ESEM_COMMAND, &is_field_valid, &esem_cmd_profile));

    if (is_field_valid)
    {
        /** clearing as if tunnel type is bcmTunnelTypeVxlan, it does not matter if in reality it was another type of vxlan tunnel */
        SHR_IF_ERR_EXIT(dnx_tunnel_init_esem_command_free_and_reset_hw(unit, esem_cmd_profile, bcmTunnelTypeVxlan));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flow_ipvx_tunnel_initiator_replace_verify(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    bcm_flow_special_fields_t * special_fields,
    bcm_flow_special_fields_t * prev_special_fields)
{
    SHR_FUNC_INIT_VARS(unit);

    /** check udp port status */
    SHR_IF_ERR_EXIT(flow_ipvx_tunnel_initiator_udp_ports_verify(unit, tunnel_type, special_fields, TRUE));

    /** check gre flag status */
    SHR_IF_ERR_EXIT(flow_ipvx_tunnel_initiator_gre_flags_verify
                    (unit, tunnel_type, special_fields, prev_special_fields, TRUE));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_ipvx_tunnel_initiator_udp_port_profile_replace(
    int unit,
    bcm_flow_special_fields_t * special_fields,
    bcm_flow_special_fields_t * prev_special_fields,
    uint32 prev_entry_handle_id,
    udp_ports_t * udp_ports)
{
    uint8 udp_src_port_updated = FALSE, udp_dst_port_updated = FALSE;
    bcm_flow_special_field_t special_field_data;

    SHR_FUNC_INIT_VARS(unit);

    /** checking FLOW_S_F_SRC_UDP_PORT was updated by the user, if not - take the old value */
    SHR_IF_ERR_EXIT(flow_special_field_new_value_or_previous_if_not_exist_get
                    (unit, special_fields, prev_special_fields, FLOW_S_F_SRC_UDP_PORT,
                     TRUE, &special_field_data, &udp_src_port_updated));
    udp_ports->src_port = special_field_data.shr_var_uint32;

    /** checking FLOW_S_F_DST_UDP_PORT was updated by the user, if not - take the old value */
    SHR_IF_ERR_EXIT(flow_special_field_new_value_or_previous_if_not_exist_get
                    (unit, special_fields, prev_special_fields, FLOW_S_F_DST_UDP_PORT,
                     TRUE, &special_field_data, &udp_dst_port_updated));
    udp_ports->dst_port = special_field_data.shr_var_uint32;

    /** if user updated the udp ports - update the profile */
    if (udp_src_port_updated || udp_dst_port_updated)
    {
        SHR_IF_ERR_EXIT(flow_ipvx_tunnel_initiator_udp_profile_replace_and_hw_update
                        (unit, udp_ports, prev_entry_handle_id));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flow_ipvx_tunnel_initiator_lif_ahp_replace(
    int unit,
    uint32 current_entry_handle_id,
    uint32 prev_entry_handle_id,
    bcm_tunnel_type_t tunnel_type,
    bcm_flow_special_fields_t * special_fields,
    bcm_flow_special_fields_t * prev_special_fields)
{
    uint32 gre_flag = 0;
    udp_ports_t udp_ports = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    /** tunnel_type is udp - check if user updated the udp ports */
    if ((tunnel_type == bcmTunnelTypeUdp) || (tunnel_type == bcmTunnelTypeUdp6))
    {
        SHR_IF_ERR_EXIT(flow_ipvx_tunnel_initiator_udp_port_profile_replace
                        (unit, special_fields, prev_special_fields, prev_entry_handle_id, &udp_ports));
    }

    /** get the gre_flag from the special fields */
    if ((tunnel_type == bcmTunnelTypeGreAnyIn4) || (tunnel_type == bcmTunnelTypeGreAnyIn6))
    {
        SHR_IF_ERR_EXIT(flow_ipvx_tunnel_initiator_gre_flag_get
                        (unit, tunnel_type, special_fields, prev_special_fields, TRUE, &gre_flag));
    }

    /** resetting the values */
    SHR_IF_ERR_EXIT(flow_ipvx_tunnel_initiator_dbal_field_lif_ahp_set
                    (unit, current_entry_handle_id, tunnel_type, gre_flag, &udp_ports));

exit:
    SHR_FUNC_EXIT;
}

void
flow_ipvx_tunnel_initiator_special_field_gre_flags_add(
    int unit,
    uint32 lif_ahp_data,
    bcm_flow_special_fields_t * special_fields)
{
    if (lif_ahp_data == DBAL_ENUM_FVAL_LIF_AHP_GRE_GENEVE_LIF_AHP_GRE8_W_KEY)
    {
        SPECIAL_FIELD_ENABLER_ADD(special_fields, FLOW_S_F_IPVX_INIT_GRE_KEY_USE_LB);
    }
    else if (lif_ahp_data == DBAL_ENUM_FVAL_LIF_AHP_GRE_GENEVE_LIF_AHP_GRE8_W_SN)
    {
        SPECIAL_FIELD_ENABLER_ADD(special_fields, FLOW_S_F_IPVX_INIT_GRE_WITH_SN);
    }
}

shr_error_e
flow_ipvx_tunnel_initiator_special_field_udp_ports_add(
    int unit,
    uint32 lif_ahp_data,
    udp_ports_t * udp_ports,
    bcm_flow_special_fields_t * special_fields)
{
    SHR_FUNC_INIT_VARS(unit);

    if (lif_ahp_data != DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_NONE)
    {
        SHR_IF_ERR_EXIT(algo_tunnel_db.udp_ports_profile.profile_data_get(unit, lif_ahp_data, NULL, udp_ports));

        SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, FLOW_S_F_SRC_UDP_PORT, udp_ports->src_port);
        SPECIAL_FIELD_UINT32_DATA_ADD(special_fields, FLOW_S_F_DST_UDP_PORT, udp_ports->dst_port);
    }

exit:
    SHR_FUNC_EXIT;
}
