/**
 * \file l3_vpbr.c PBR procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_L3
/*
 * Include files which are specifically for DNX. Final location.
 * {
 */
/*
 * }
 */
/*
 * Include files currently used for DNX. To be modified and moved to
 * final location.
 * {
 */
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>

/*
 * }
 */
/*
 * Include files.
 * {
 */

#include <shared/bslenum.h>
#include <bcm/types.h>
#include <bcm/switch.h>
#include <bcm/l3.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <bcm_int/dnx/stat/stat_pp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tunnel.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

/*
 * }
 */

/*
 * Values for vpbr Valid/ NotValid Mask
 */
#define DNX_L3_VPBR_NOT_VALID_MASK              0x0
#define DNX_L3_VPBR_VALID_MASK                  0xFFFFFFFF
#define DNX_L3_VPBR_DSCP_VALID_MASK             0xFF
#define DNX_L3_VPBR_SOURCE_PORT_VALID_MASK      0xFFFF
#define DNX_L3_VPBR_DESTINATION_PORT_VALID_MASK 0xFFFF
#define DNX_L3_VPBR_GPORT_WIDE_DATA_MAX_VALUE   0xF
/*
 * full mask definition for ipv6
 */
#define DNX_L3_VPBR_ENTRY_FIELD32_MASKED_SET_IF_VALID(unit, entry_handle_id, field_id, field_val, field_mask) \
{\
    if (field_mask != DNX_L3_VPBR_NOT_VALID_MASK) \
    { \
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, field_id, field_val, field_mask); \
    } \
}

#define DNX_L3_VPBR_ENTRY_FIELD32_SET_IF_VALID(unit, entry_handle_id, field_id, field_val, field_mask) \
{\
    if (field_mask != DNX_L3_VPBR_NOT_VALID_MASK) \
    { \
        dbal_entry_key_field32_set(unit, entry_handle_id, field_id, field_val); \
    } \
}

#define DNX_L3_VPBR_ENTRY_FIELD_MASK_SET(_mask_, _valid_mask_) \
    (_mask_) = (field_mask[0] == DNX_L3_VPBR_NOT_VALID_MASK) ? DNX_L3_VPBR_NOT_VALID_MASK : (_valid_mask_)

#define DNX_L3_VPBR_STAT_ID_MAX_VALUE (0xFFFF)

#define DNX_L3_VPBR_SUPPORTED_FLAGS (BCM_L3_VPBR_IP6 | BCM_L3_VPBR_REPLACE | BCM_L3_VPBR_TUNNEL)
#define DNX_L3_VPBR_SUPPORTED_FLAGS_STRING "BCM_L3_VPBR_IP6, BCM_L3_VPBR_REPLACE, BCM_L3_VPBR_TUNNEL"

/**
 * \brief
 * Verify flags
 *     \param [in] unit - Relevant unit.
 *     \param [in] flags - Relevant flags.
 * \return
 *     \retval Negative in case of an error.
 *     \retval Zero in case of NO ERROR
 *
 */
static shr_error_e
dnx_l3_vpbr_flags_verify(
    int unit,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flags & ~DNX_L3_VPBR_SUPPORTED_FLAGS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal value for flags [0x%x]. Supported flags are %s\n",
                     flags, DNX_L3_VPBR_SUPPORTED_FLAGS_STRING);
    }
    if (_SHR_IS_FLAG_SET(flags, BCM_L3_VPBR_TUNNEL))
    {
        if (!dnx_data_l3.feature.feature_get(unit, dnx_data_l3_feature_vpbr_tunnel_available))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "L3 VPBR tunnel feature is not available for this device\n");
        }
        if (_SHR_IS_FLAG_SET(flags, BCM_L3_VPBR_IP6))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal combination of flags BCM_L3_VPBR_IP6 and BCM_L3_VPBR_TUNNEL\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify entry key parameters.
 *     \param [in] unit - Relevant unit.
 *     \param [in] entry - Relevant entry.
 * \return
 *     \retval Negative in case of an error.
 *     \retval Zero in case of NO ERROR
 *
 */
static shr_error_e
dnx_l3_vpbr_entry_key_parameters_verify(
    int unit,
    bcm_l3_vpbr_entry_t * entry)
{
    SHR_FUNC_INIT_VARS(unit);

    /** In case entry ID is not provided, verify fields masks should be all 1 / all 0 */
    if ((entry->l3_intf_id_mask != DNX_L3_VPBR_VALID_MASK) && (entry->l3_intf_id_mask != DNX_L3_VPBR_NOT_VALID_MASK))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "illegal value for entry->l3_intf_id_mask [0x%x]. Should be 0x%x or 0x%x\n",
                     entry->l3_intf_id_mask, DNX_L3_VPBR_NOT_VALID_MASK, DNX_L3_VPBR_VALID_MASK);
    }
    if ((entry->vrf_mask != DNX_L3_VPBR_VALID_MASK) && (entry->vrf_mask != DNX_L3_VPBR_NOT_VALID_MASK))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "illegal value for entry->vrf_mask [0x%x]. Should be 0x%x or 0x%x\n",
                     entry->vrf_mask, DNX_L3_VPBR_NOT_VALID_MASK, DNX_L3_VPBR_VALID_MASK);
    }
    if ((entry->src_port_mask != DNX_L3_VPBR_SOURCE_PORT_VALID_MASK)
        && (entry->src_port_mask != DNX_L3_VPBR_NOT_VALID_MASK))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "illegal value for entry->src_port_mask [0x%x]. Should be 0x%x or 0x%x\n",
                     entry->src_port_mask, DNX_L3_VPBR_NOT_VALID_MASK, DNX_L3_VPBR_SOURCE_PORT_VALID_MASK);
    }
    if ((entry->dst_port_mask != DNX_L3_VPBR_DESTINATION_PORT_VALID_MASK)
        && (entry->dst_port_mask != DNX_L3_VPBR_NOT_VALID_MASK))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "illegal value for entry->dst_port_mask [0x%x]. Should be 0x%x or 0x%x\n",
                     entry->dst_port_mask, DNX_L3_VPBR_NOT_VALID_MASK, DNX_L3_VPBR_DESTINATION_PORT_VALID_MASK);
    }
    if (entry->gport_wide_data > DNX_L3_VPBR_GPORT_WIDE_DATA_MAX_VALUE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "illegal value for entry->gport_wide_data [0x%x], valid values are from 0 to 0x%x\n",
                     entry->gport_wide_data, DNX_L3_VPBR_GPORT_WIDE_DATA_MAX_VALUE);
    }

    if (entry->vrf > dnx_data_l3.vrf.nof_vrf_get(unit) - 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "vrf value greater then number of vrf in the system [0x%x]\n", entry->vrf);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify entry result parameters.
 *     \param [in] unit - Relevant unit.
 *     \param [in] entry - Relevant entry.
 * \return
 *     \retval Negative in case of an error.
 *     \retval Zero in case of NO ERROR
 *
 */
static shr_error_e
dnx_l3_vpbr_entry_result_parameters_verify(
    int unit,
    bcm_l3_vpbr_entry_t * entry)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((entry->new_vrf > dnx_data_l3.vrf.nof_vrf_get(unit) - 1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "new vrf value greater then number of vrf in the system [0x%x]\n", entry->new_vrf);
    }

    if ((entry->stat_pp_profile != STAT_PP_PROFILE_INVALID))
    {
        if (_SHR_IS_FLAG_SET(entry->flags, BCM_L3_VPBR_TUNNEL))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Statistics are not supported for VPBR tunnel entries\n");
        }
        if (entry->stat_id > DNX_L3_VPBR_STAT_ID_MAX_VALUE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Stat id %d, max value 0x%x", entry->stat_id, DNX_L3_VPBR_STAT_ID_MAX_VALUE);
        }
        SHR_IF_ERR_EXIT(dnx_stat_pp_crps_verify
                        (unit, BCM_CORE_ALL, entry->stat_id, entry->stat_pp_profile,
                         bcmStatCounterInterfaceIngressReceivePp));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify flags for adding and entry and the entry key and result parameters.
 * If entry_id is provided and the entry is for insertion (not update),
 * the entry_id will be ignored and updated by the API.
 *     \param [in] unit - Relevant unit.
 *     \param [in] entry - Relevant entry.
 * \return
 *     \retval Negative in case of an error.
 *     \retval Zero in case of NO ERROR
 *
 */
static shr_error_e
dnx_l3_vpbr_entry_add_verify(
    int unit,
    bcm_l3_vpbr_entry_t * entry)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry, _SHR_E_PARAM, "bcm_l3_vpbr_entry_t");

    SHR_INVOKE_VERIFY_DNXC(dnx_l3_vpbr_flags_verify(unit, entry->flags));

    SHR_INVOKE_VERIFY_DNXC(dnx_l3_vpbr_entry_key_parameters_verify(unit, entry));
    SHR_INVOKE_VERIFY_DNXC(dnx_l3_vpbr_entry_result_parameters_verify(unit, entry));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify flags for getting or deleting and entry and the entry key and result parameters.
 *     \param [in] unit - Relevant unit.
 *     \param [in] entry - Relevant entry.
 * \return
 *     \retval Negative in case of an error.
 *     \retval Zero in case of NO ERROR
 *
 */
static shr_error_e
dnx_l3_vpbr_entry_get_delete_verify(
    int unit,
    bcm_l3_vpbr_entry_t * entry)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry, _SHR_E_PARAM, "bcm_l3_vpbr_entry_t");

    SHR_INVOKE_VERIFY_DNXC(dnx_l3_vpbr_flags_verify(unit, entry->flags));
    if (entry->entry_id == 0)
    {
        /** Entry ID is not provided. Validate the key, since it's used to get or delete the entry. */
        SHR_INVOKE_VERIFY_DNXC(dnx_l3_vpbr_entry_key_parameters_verify(unit, entry));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Select the DBAL table for the L3 vPBR database (IPv4 / IPv6).
 *     \param [in] unit - Relevant unit.
 *     \param [in] flags - Relevant flags.
 *     \param [out] dbal_table - Selected DBAL table.
 * \return
 *     \retval Negative in case of an error.
 *     \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_l3_vpbr_dbal_table_select(
    int unit,
    uint32 flags,
    dbal_tables_e * dbal_table)
{
    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(flags, BCM_L3_VPBR_IP6))
    {
        if (_SHR_IS_FLAG_SET(flags, BCM_L3_VPBR_TUNNEL))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "VPBR tunnel entries are not supported for IPv6 tunnels\n");
        }
        else
        {
            *dbal_table = DBAL_TABLE_IPV6_VRF_DB_TCAM;
        }
    }
    else
    {
        if (_SHR_IS_FLAG_SET(flags, BCM_L3_VPBR_TUNNEL))
        {
            if (dnx_data_l3.feature.feature_get(unit, dnx_data_l3_feature_vpbr_tunnel_available))
            {
                *dbal_table = DBAL_TABLE_TT_IPV4_VRF_DB_TCAM;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "L3 VPBR tunnel feature is not available for this device\n");
            }
        }
        else
        {
            *dbal_table = DBAL_TABLE_IPV4_VRF_DB_TCAM;
        }
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Add key to the L3 vPBR database (IPv4 / IPv6).
 *     \param [in] unit - Relevant unit.
 *     \param [in] entry - Relevant entry.
 *     \param [in] entry_handle_id - DBAL entry handle ID.
 * \return
 *     \retval Negative in case of an error.
 *     \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_l3_vpbr_entry_key_set(
    int unit,
    bcm_l3_vpbr_entry_t * entry,
    uint32 entry_handle_id)
{
    int global_in_lif = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (entry->l3_intf_id_mask != DNX_L3_VPBR_NOT_VALID_MASK)
    {
        global_in_lif = BCM_L3_ITF_VAL_GET((entry->l3_intf_id & entry->l3_intf_id_mask));
    }

    if (_SHR_IS_FLAG_SET(entry->flags, BCM_L3_VPBR_IP6))
    {
        dbal_entry_key_field_arr8_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV6_SIP, entry->sip6_addr,
                                             entry->sip6_addr_mask);
        dbal_entry_key_field_arr8_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV6_DIP, entry->dip6_addr,
                                             entry->dip6_addr_mask);
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV6_TC, entry->dscp, entry->dscp_mask);
    }
    else
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV4_SIP, entry->sip_addr,
                                          entry->sip_addr_mask);
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV4_DIP, entry->dip_addr,
                                          entry->dip_addr_mask);
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV4_TOS, entry->dscp, entry->dscp_mask);
    }

    DNX_L3_VPBR_ENTRY_FIELD32_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_VRF, entry->vrf, entry->vrf_mask);

    DNX_L3_VPBR_ENTRY_FIELD32_MASKED_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, global_in_lif,
                                                  entry->l3_intf_id_mask);
    DNX_L3_VPBR_ENTRY_FIELD32_MASKED_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_IP_PROTOCOL, entry->ip_protocol,
                                                  entry->ip_protocol_mask);
    DNX_L3_VPBR_ENTRY_FIELD32_MASKED_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_L4_SRC_PORT, entry->src_port,
                                                  entry->src_port_mask);
    DNX_L3_VPBR_ENTRY_FIELD32_MASKED_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_L4_DST_PORT, entry->dst_port,
                                                  entry->dst_port_mask);
    DNX_L3_VPBR_ENTRY_FIELD32_MASKED_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_LIF_WIDE_DATA_21_TO_18,
                                                  entry->gport_wide_data, entry->gport_wide_data_mask);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Add result to the L3 vPBR database (IPv4 / IPv6).
 *     \param [in] unit - Relevant unit.
 *     \param [in] entry - Relevant entry.
 *     \param [in] entry_handle_id - DBAL entry handle ID.
 * \return
 *     \retval Negative in case of an error.
 *     \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_l3_vpbr_entry_result_set(
    int unit,
    bcm_l3_vpbr_entry_t * entry,
    uint32 entry_handle_id)
{
    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(entry->flags, BCM_L3_VPBR_TUNNEL))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_RESULT_TYPE_FROM_FODO_PD, INST_SINGLE,
                                     DBAL_ENUM_FVAL_FODO_PD_RESULT_TYPE_RESULT_FOUND);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRF, INST_SINGLE, entry->new_vrf);
    }
    else
    {
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRF, INST_SINGLE, entry->new_vrf);
        }
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE, entry->stat_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE,
                                     entry->stat_pp_profile);
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Requests result from the L3 vPBR database (IPv4 / IPv6).
 *     \param [in] unit - Relevant unit.
 *     \param [in,out] entry - Relevant entry.
 *     \param [in] entry_handle_id - DBAL entry handle ID.
 * \return
 *     \retval Negative in case of an error.
 *     \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_l3_vpbr_entry_result_request(
    int unit,
    bcm_l3_vpbr_entry_t * entry,
    uint32 entry_handle_id)
{
    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(entry->flags, BCM_L3_VPBR_TUNNEL))
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_VRF, INST_SINGLE, (uint32 *) &entry->new_vrf);
    }
    else
    {
        {
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE,
                                       (uint32 *) &entry->stat_id);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE,
                                       (uint32 *) &entry->stat_pp_profile);
        }
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_VRF, INST_SINGLE, (uint32 *) &entry->new_vrf);
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get all entry key and result fields from the L3 vPBR database (IPv4 / IPv6).
 * Used in get entry with ID and traverse.
 *     \param [in] unit - Relevant unit.
 *     \param [in,out] entry - Relevant entry.
 *     \param [in] entry_handle_id - DBAL entry handle ID.
 * \return
 *     \retval Negative in case of an error.
 *     \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_l3_vpbr_entry_get_all_fields(
    int unit,
    bcm_l3_vpbr_entry_t * entry,
    uint32 entry_handle_id)
{
    uint32 field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
    uint32 field_mask[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_attribute_get
                    (unit, entry_handle_id, DBAL_ENTRY_ATTR_PRIORITY, (uint32 *) &(entry->priority)));

    if (_SHR_IS_FLAG_SET(entry->flags, BCM_L3_VPBR_IP6))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                        (unit, entry_handle_id, DBAL_FIELD_IPV6_SIP, field_value, field_mask));
        sal_memcpy(entry->sip6_addr, field_value, sizeof(bcm_ip6_t));
        sal_memcpy(entry->sip6_addr_mask, field_mask, sizeof(bcm_ip6_t));

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                        (unit, entry_handle_id, DBAL_FIELD_IPV6_DIP, field_value, field_mask));
        sal_memcpy(entry->dip6_addr, field_value, sizeof(bcm_ip6_t));
        sal_memcpy(entry->dip6_addr_mask, field_mask, sizeof(bcm_ip6_t));

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                        (unit, entry_handle_id, DBAL_FIELD_IPV6_TC, field_value, field_mask));
        entry->dscp = field_value[0];
        entry->dscp_mask = field_mask[0];
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                        (unit, entry_handle_id, DBAL_FIELD_IPV4_SIP, field_value, field_mask));
        entry->sip_addr = field_value[0];
        entry->sip_addr_mask = field_mask[0];

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                        (unit, entry_handle_id, DBAL_FIELD_IPV4_DIP, field_value, field_mask));
        entry->dip_addr = field_value[0];
        entry->dip_addr_mask = field_mask[0];

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                        (unit, entry_handle_id, DBAL_FIELD_IPV4_TOS, field_value, field_mask));
        entry->dscp = field_value[0];
        entry->dscp_mask = field_mask[0];
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                    (unit, entry_handle_id, DBAL_FIELD_VRF, field_value, field_mask));
    entry->vrf = field_value[0];
    DNX_L3_VPBR_ENTRY_FIELD_MASK_SET(entry->vrf_mask, DNX_L3_VPBR_VALID_MASK);

    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                    (unit, entry_handle_id, DBAL_FIELD_IP_PROTOCOL, field_value, field_mask));
    entry->ip_protocol = field_value[0];
    entry->ip_protocol_mask = field_mask[0];

    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                    (unit, entry_handle_id, DBAL_FIELD_L4_SRC_PORT, field_value, field_mask));
    entry->src_port = field_value[0];
    DNX_L3_VPBR_ENTRY_FIELD_MASK_SET(entry->src_port_mask, DNX_L3_VPBR_SOURCE_PORT_VALID_MASK);

    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                    (unit, entry_handle_id, DBAL_FIELD_L4_DST_PORT, field_value, field_mask));
    entry->dst_port = field_value[0];
    DNX_L3_VPBR_ENTRY_FIELD_MASK_SET(entry->dst_port_mask, DNX_L3_VPBR_DESTINATION_PORT_VALID_MASK);

    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                    (unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, field_value, field_mask));
    BCM_L3_ITF_SET(entry->l3_intf_id, _SHR_L3_ITF_TYPE_LIF, field_value[0]);
    DNX_L3_VPBR_ENTRY_FIELD_MASK_SET(entry->l3_intf_id_mask, DNX_L3_VPBR_VALID_MASK);

    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                    (unit, entry_handle_id, DBAL_FIELD_LIF_WIDE_DATA_21_TO_18, field_value, field_mask));
    entry->gport_wide_data = field_value[0];

    if (!_SHR_IS_FLAG_SET(entry->flags, BCM_L3_VPBR_TUNNEL))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE, field_value));
        entry->stat_id = field_value[0];

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE, field_value));
        entry->stat_pp_profile = field_value[0];
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                    (unit, entry_handle_id, DBAL_FIELD_VRF, INST_SINGLE, field_value));
    entry->new_vrf = field_value[0];

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Add entry to the L3 vPBR database according to application
 * type (IPv4 \ IPv6) each field in the entry has mask field.
 * mask is either all or nothing. In case an entry field does
 * not exist (mask = DNX_L3_VPBR_NOT_VALID_MASK) the key field
 * is not used.
 *
 *     \param [in] unit - Relevant unit.
 *     \param [in] entry - entry contains information required
 *        for manipulating L3 vPBR table entries.
 *        the entry consist of the following fields:
 *        flags - available flags: BCM_L3_VPBR_IP6.
 *                in case flags = 0 IPV4 is assumed.
 *        priority - Entry priority.
 *        l3_intf_id - The relevant incoming interface.
 *        l3_intf_id - The relevant incoming interface mask.
 *                     0 - not valid, 0xffffffff - valid
 *        vrf - Virtual Router instance.
 *        vrf_mask - Virtual Router instance mask.
 *                   0 - not valid, 0xffffffff - valid
 *        sip_addr - Source IP address (IPv4).
 *        sip_addr_mask - Source IP address (IPv4) mask.
 *                        0 - not valid, 0xffffffff - valid.
 *        dip_addr - Destination IP address (IPv4).
 *        dip_addr_mask - Destination IP address (IPv4) mask.
 *                        0 - not valid, 0xffffffff - valid
 *        sip6_addr - Source IP address (IPv6).
 *        sip6_addr_mask - Source IP address (IPv6) mask.
 *                         all 0 - not valid, all 1 - valid.
 *        dip6_addr - Destination IP address (IPv6).
 *        dip6_addr_mask - Destination IP address (IPv6) mask
 *                         all 0 - not valid, all 1 - valid.
 *        dscp - DSCP value.
 *        dscp_mask - DSCP value mask.
 *                    0- not valid, 0xff - valid.
 *        src_port - TCP/UDP src port.
 *        src_port_mask - TCP/UDP src port mask.
 *                        0 - not valid, 0xffff - valid.
 *        dst_port - TCP/UDP dst port.
 *        dst_port_mask - TCP/UDP dst port mask.
 *                        0 - not valid, 0xffff - valid.
 *        new_vrf - The new result Virtual Router instance.
 * \return
 *     \retval Negative in case of an error.
 *     \retval Zero in case of NO ERROR
 */
shr_error_e
bcm_dnx_l3_vpbr_entry_add(
    int unit,
    bcm_l3_vpbr_entry_t * entry)
{
    dbal_tables_e dbal_table = DBAL_TABLE_EMPTY;
    uint32 entry_handle_id;
    dbal_entry_action_flags_e dbal_action;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_l3_vpbr_entry_add_verify(unit, entry));

    SHR_IF_ERR_EXIT(dnx_l3_vpbr_dbal_table_select(unit, entry->flags, &dbal_table));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));

    if (_SHR_IS_FLAG_SET(entry->flags, BCM_L3_VPBR_REPLACE))
    {
        SHR_IF_ERR_EXIT(dnx_l3_vpbr_entry_key_set(unit, entry, entry_handle_id));
        if (entry->entry_id == 0)
        {
            SHR_IF_ERR_EXIT(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &(entry->entry_id), DBAL_COMMIT));
        }
        SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry->entry_id));
        dbal_action = DBAL_COMMIT_UPDATE;
    }
    else
    {
        /** Ignore the entry_id as input. Set it to 0 so it can be updated at the end. */
        entry->entry_id = 0;
        SHR_IF_ERR_EXIT(dnx_l3_vpbr_entry_key_set(unit, entry, entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_entry_attribute_set(unit, entry_handle_id, DBAL_ENTRY_ATTR_PRIORITY, entry->priority));
        dbal_action = DBAL_COMMIT;
    }

    SHR_IF_ERR_EXIT(dnx_l3_vpbr_entry_result_set(unit, entry, entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, dbal_action));

    if (entry->entry_id == 0)
    {
        SHR_IF_ERR_EXIT(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &(entry->entry_id), DBAL_COMMIT));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get an entry from L3 vPBR database (IPv4 \ IPv6).
 *
 *     \param [in] unit - Relevant unit.
 *     \param [in,out] entry - Relevant entry.
 *
 * \return
 *     \retval Negative in case of an error. When entry is not
 *             found, will return an error.
 *     \retval Zero in case of NO ERROR
 *
 * \see
 *    bcm_dnx_l3_vpbr_entry_add
 */
shr_error_e
bcm_dnx_l3_vpbr_entry_get(
    int unit,
    bcm_l3_vpbr_entry_t * entry)
{
    dbal_tables_e dbal_table = DBAL_TABLE_EMPTY;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_l3_vpbr_entry_get_delete_verify(unit, entry));

    SHR_IF_ERR_EXIT(dnx_l3_vpbr_dbal_table_select(unit, entry->flags, &dbal_table));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));

    if (entry->entry_id == 0)
    {
        SHR_IF_ERR_EXIT(dnx_l3_vpbr_entry_key_set(unit, entry, entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &(entry->entry_id), DBAL_COMMIT));
        SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry->entry_id));
        SHR_IF_ERR_EXIT(dnx_l3_vpbr_entry_result_request(unit, entry, entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, (DBAL_COMMIT)));
        SHR_IF_ERR_EXIT(dbal_entry_attribute_get
                        (unit, entry_handle_id, DBAL_ENTRY_ATTR_PRIORITY, (uint32 *) &(entry->priority)));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry->entry_id));
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, (DBAL_GET_ALL_FIELDS)));
        SHR_IF_ERR_EXIT(dnx_l3_vpbr_entry_get_all_fields(unit, entry, entry_handle_id));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Delete and entry from L3 vPBR database (IPv4 \ IPv6).
 *
 *     \param [in] unit - Relevant unit.
 *     \param [in] entry - Relevant entry.
 *
 * \return
 *     \retval Negative in case of an error. When entry is not
 *             found, will return an error.
 *     \retval Zero in case of NO ERROR
 *
 * \see
 *    bcm_dnx_l3_vpbr_entry_add
 */
shr_error_e
bcm_dnx_l3_vpbr_entry_delete(
    int unit,
    bcm_l3_vpbr_entry_t * entry)
{
    dbal_tables_e dbal_table = DBAL_TABLE_EMPTY;
    uint32 entry_handle_id;
    uint32 entry_access_id = entry->entry_id;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_l3_vpbr_entry_get_delete_verify(unit, entry));

    SHR_IF_ERR_EXIT(dnx_l3_vpbr_dbal_table_select(unit, entry->flags, &dbal_table));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));

    if (entry_access_id == 0)
    {
        SHR_IF_ERR_EXIT(dnx_l3_vpbr_entry_key_set(unit, entry, entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &entry_access_id, DBAL_COMMIT));
    }
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Delete all entries from L3 vPBR database (IPv4 \ IPv6).
 *     \param [in] unit - Relevant unit.
 *     \param [in] entry - Relevant entry.
 *                 entry.l3a_flags - Control flags that indicate choice of
 *                                     table to be cleared:
 *                 * BCM_L3_VPBR_IP6 - If provided, it indicates that
 *                   the IPv6 table entries will be cleared,
 *                   otherwise the IPv4 table entries will be cleared.
 *                 * BCM_L3_VPBR_TUNNEL - If provided, it indicates that
 *                   the tunnel termination table entries will be cleared,
 *                   otherwise the FWD1 table entries will be cleared.
 *                   will be cleared.
 * \return
 *     \retval Negative in case of an error.
 *     \retval Zero in case of NO ERROR
 *
 * \see
 *    bcm_dnx_l3_vpbr_entry_add
 */
shr_error_e
bcm_dnx_l3_vpbr_entry_delete_all(
    int unit,
    bcm_l3_vpbr_entry_t * entry)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    if (entry == NULL)
    {
        SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_IPV4_VRF_DB_TCAM));
        SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_IPV6_VRF_DB_TCAM));

        if (dnx_data_l3.feature.feature_get(unit, dnx_data_l3_feature_vpbr_tunnel_available))
        {
            SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_TT_IPV4_VRF_DB_TCAM));
        }
    }
    else
    {
        dbal_tables_e dbal_table = DBAL_TABLE_EMPTY;
        SHR_INVOKE_VERIFY_DNXC(dnx_l3_vpbr_flags_verify(unit, entry->flags));
        SHR_IF_ERR_EXIT(dnx_l3_vpbr_dbal_table_select(unit, entry->flags, &dbal_table));
        SHR_IF_ERR_EXIT(dbal_table_clear(unit, dbal_table));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Traverse all L3 vPBR entries in DNX devices and call a
 *  function with the data for each entry found.
 * \param [in] unit - The unit number.
 * \param [in] flags - Control flags that indicate choice of
 *        table to be traversed:
 *          * BCM_L3_VPBR_IP6 - if provided, it indicates that
 *            the IPv6 table entries will be traversed,
 *            otherwise the IPv4 table entries will be
 *            traversed.
 * \param [in] cb - The name of the callback function that is
 *        going to be called for each valid entry.
 * \param [in] user_data - Data that is passed to the callback
 *        function.
 * \return
 *   \retval Zero in case of no error.
 *   \retval Non-zero in case of an error.
 * \remark
 *      DBAL_TABLE_IPV4_VRF_DB_TCAM
 *      DBAL_TABLE_IPV6_VRF_DB_TCAM
 * \see
 *   * BCM L3 Flags \n
 */
shr_error_e
bcm_dnx_l3_vpbr_traverse(
    int unit,
    uint32 flags,
    bcm_l3_vpbr_traverse_cb cb,
    void *user_data)
{
    dbal_tables_e dbal_table = DBAL_TABLE_EMPTY;
    uint32 entry_handle_id;
    uint32 index = 0;
    int is_end;
    bcm_l3_vpbr_entry_t entry;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_l3_vpbr_flags_verify(unit, flags));

    /** In case no call back is provided return and do nothing */
    if (cb == NULL)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_l3_vpbr_dbal_table_select(unit, flags, &dbal_table));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));

    /** Receive first entry in table. */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        bcm_l3_vpbr_entry_t_init(&entry);

        entry.flags = flags;

        SHR_IF_ERR_EXIT(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &(entry.entry_id), DBAL_COMMIT));
        SHR_IF_ERR_EXIT(dnx_l3_vpbr_entry_get_all_fields(unit, &entry, entry_handle_id));

        /*
         * If user provided a name of the callback function, it will be invoked with sending the vpbr_entry structure
         * of the entry that was found.
         */
        if (cb != NULL)
        {
            /** Invoke callback function */
            SHR_IF_ERR_EXIT((*cb) (unit, &entry, user_data));
        }
        /** Receive next entry in table. */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        index++;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
