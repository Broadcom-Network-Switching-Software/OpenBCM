/**
 * \file l3_vpbr.c PBR procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_L3
/*
 * Include files which are specifically for DNX. Final location.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
/*
 * }
 */
/*
 * Include files currently used for DNX. To be modified and moved to
 * final location.
 * {
 */
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
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <bcm/l3.h>
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
/*
 * full mask definition for ipv6
 */
const bcm_ip6_t ip6_full_mask =
    { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

#define VPBR_ENTRY_FIELD32_MASKED_SET_IF_VALID(unit, entry_handle_id, field_id, field_val, field_mask) \
{\
    if (field_mask != DNX_L3_VPBR_NOT_VALID_MASK) \
    { \
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, field_id, field_val, field_mask); \
    } \
}

#define VPBR_ENTRY_FIELD32_SET_IF_VALID(unit, entry_handle_id, field_id, field_val, field_mask) \
{\
    if (field_mask != DNX_L3_VPBR_NOT_VALID_MASK) \
    { \
        dbal_entry_key_field32_set(unit, entry_handle_id, field_id, field_val); \
    } \
}

#define VPBR_ENTRY_IPV6_MASKED_SET_IF_VALID(unit, entry_handle_id, field_id, field_val, field_mask) \
{\
    if (sal_memcmp(field_mask, &ip6_full_mask, sizeof(bcm_ip6_t)) == 0) \
    {\
        dbal_entry_key_field_arr8_masked_set(unit, entry_handle_id, field_id, field_val, field_mask);\
    }\
}

/**
 * \brief
 * verify traverse parameters
 *     \param [in] unit - Relevant unit.
 *     \param [in] flags - relevant flags. availiable flags: BCM_L3_VPBR_IP6.
 *                in case flags = 0 IPV4 is assumed.
 * \return
 *     \retval Negative in case of an error.
 *     \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_l3_vpbr_traverse_verify(
    int unit,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Supporting IPV6 only flag, flags=0 is IPV4
     */
    if (flags & (~BCM_L3_VPBR_IP6))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "illegal value for flags [%x]\n", flags);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * verify entry parameters
 *     \param [in] unit - Relevant unit.
 *     \param [in] entry - relevant entry.
 * \return
 *     \retval Negative in case of an error.
 *     \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_l3_vpbr_entry_verify(
    int unit,
    bcm_l3_vpbr_entry_t * entry)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry, _SHR_E_PARAM, "bcm_l3_vpbr_entry_t");
    /*
     * Supporting IPV6 only flag, flags=0 is IPV4
     */
    if (entry->flags & ~BCM_L3_VPBR_IP6)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "illegal value for flags [%x]\n", entry->flags);
    }

    /*
     * Verify fields masks should be all 1 / all 0
     */
    if ((entry->l3_intf_id_mask != DNX_L3_VPBR_VALID_MASK) && (entry->l3_intf_id_mask != DNX_L3_VPBR_NOT_VALID_MASK))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "illegal value for entry->l3_intf_id_mask [%x]\n", entry->l3_intf_id_mask);
    }
    if ((entry->vrf_mask != DNX_L3_VPBR_VALID_MASK) && (entry->vrf_mask != DNX_L3_VPBR_NOT_VALID_MASK))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "illegal value for entry->vrf_mask [%x]\n", entry->vrf_mask);
    }
    if ((entry->dscp_mask != DNX_L3_VPBR_DSCP_VALID_MASK) && (entry->dscp_mask != DNX_L3_VPBR_NOT_VALID_MASK))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "illegal value for entry->dscp_mask [%x]\n", entry->dscp_mask);
    }
    if ((entry->src_port_mask != DNX_L3_VPBR_SOURCE_PORT_VALID_MASK)
        && (entry->src_port_mask != DNX_L3_VPBR_NOT_VALID_MASK))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "illegal value for entry->src_port_mask [%x]\n", entry->src_port_mask);
    }
    if ((entry->dst_port_mask != DNX_L3_VPBR_DESTINATION_PORT_VALID_MASK)
        && (entry->dst_port_mask != DNX_L3_VPBR_NOT_VALID_MASK))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "illegal value for entry->dst_port_mask [%x]\n", entry->dst_port_mask);
    }

    if (entry->vrf > dnx_data_l3.vrf.nof_vrf_get(unit) - 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "vrf value greater then number of vrf in the system [%x]\n", entry->vrf);
    }

    if (entry->new_vrf > dnx_data_l3.vrf.nof_vrf_get(unit) - 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "new vrf value greater then number of vrf in the system [%x]\n", entry->new_vrf);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * add ipv4 entry to the L3 vPBR database
 *     \param [in] unit - Relevant unit.
 *     \param [in] global_in_lif - relevant Inlif
 *     \param [in] entry - relevant entry.
 * \return
 *     \retval Negative in case of an error.
 *     \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_l3_vpbr_ipv4_entry_add(
    int unit,
    int global_in_lif,
    bcm_l3_vpbr_entry_t * entry)
{
    uint32 entry_access_id;
    uint32 entry_handle_id;
    int core = DBAL_CORE_ALL;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_entry_access_id_create
                    (unit, core, DBAL_TABLE_IPV4_VRF_DB_TCAM, entry->priority, &entry_access_id));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPV4_VRF_DB_TCAM, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));

    VPBR_ENTRY_FIELD32_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_VRF, entry->vrf, entry->vrf_mask);

    VPBR_ENTRY_FIELD32_MASKED_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, global_in_lif,
                                           entry->l3_intf_id_mask);

    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV4_SIP, entry->sip_addr,
                                      entry->sip_addr_mask);

    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV4_DIP, entry->dip_addr,
                                      entry->dip_addr_mask);

    VPBR_ENTRY_FIELD32_MASKED_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_IP_PROTOCOL, entry->ip_protocol,
                                           entry->ip_protocol_mask);

    VPBR_ENTRY_FIELD32_MASKED_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_IPV4_TOS, entry->dscp, entry->dscp_mask);

    VPBR_ENTRY_FIELD32_MASKED_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_L4_SRC_PORT, entry->src_port,
                                           entry->src_port_mask);

    VPBR_ENTRY_FIELD32_MASKED_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_L4_DST_PORT, entry->dst_port,
                                           entry->dst_port_mask);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRF, INST_SINGLE, entry->new_vrf);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, (DBAL_COMMIT)));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * add ipv6 entry to L3 vPBR database.
 *     \param [in] unit - Relevant unit.
 *     \param [in] global_in_lif - relevant Inlif
 *     \param [in] entry - relevant entry.
 * \return
 *     \retval Negative in case of an error.
 *     \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_l3_vpbr_ipv6_entry_add(
    int unit,
    int global_in_lif,
    bcm_l3_vpbr_entry_t * entry)
{
    uint32 entry_access_id;
    uint32 entry_handle_id;
    int core = DBAL_CORE_ALL;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_entry_access_id_create
                    (unit, core, DBAL_TABLE_IPV6_VRF_DB_TCAM, entry->priority, &entry_access_id));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPV6_VRF_DB_TCAM, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));

    VPBR_ENTRY_FIELD32_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_VRF, entry->vrf, entry->vrf_mask);

    VPBR_ENTRY_FIELD32_MASKED_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, global_in_lif,
                                           entry->l3_intf_id_mask);

    dbal_entry_key_field_arr8_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV6_SIP, entry->sip6_addr,
                                         entry->sip6_addr_mask);

    dbal_entry_key_field_arr8_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV6_DIP, entry->dip6_addr,
                                         entry->dip6_addr_mask);

    VPBR_ENTRY_FIELD32_MASKED_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_IP_PROTOCOL, entry->ip_protocol,
                                           entry->ip_protocol_mask);

    VPBR_ENTRY_FIELD32_MASKED_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_IPV6_TC, entry->dscp, entry->dscp_mask);

    VPBR_ENTRY_FIELD32_MASKED_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_L4_SRC_PORT, entry->src_port,
                                           entry->src_port_mask);

    VPBR_ENTRY_FIELD32_MASKED_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_L4_DST_PORT, entry->dst_port,
                                           entry->dst_port_mask);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRF, INST_SINGLE, entry->new_vrf);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, (DBAL_COMMIT)));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * add entry to the L3 vPBR database according to application
 * type (ipb4 \ ipv6) each field in the entry has mask field.
 * mask is either all or nothing. In case an entry field does
 * not exist (mask = DNX_L3_VPBR_NOT_VALID_MASK) the key field
 * is not used.
 *
 *     \param [in] unit - Relevant unit.
 *     \param [in] entry - entry contains information required
 *        for manipulating L3 vPBR table entries.
 *        the entry consist of the following fields:
 *        flags - availiable flags: BCM_L3_VPBR_IP6.
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
    int global_in_lif = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l3_vpbr_entry_verify(unit, entry));

    if (entry->l3_intf_id_mask != DNX_L3_VPBR_NOT_VALID_MASK)
    {
        global_in_lif = BCM_L3_ITF_VAL_GET((entry->l3_intf_id & entry->l3_intf_id_mask));
    }

    if (_SHR_IS_FLAG_SET(entry->flags, BCM_L3_VPBR_IP6))
    {
        SHR_IF_ERR_EXIT(dnx_l3_vpbr_ipv6_entry_add(unit, global_in_lif, entry));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_l3_vpbr_ipv4_entry_add(unit, global_in_lif, entry));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * get entry from the TCAM DB for application type ipv4
 *     \param [in] unit - Relevant unit.
 *     \param [in] global_in_lif - relevant Inlif
 *     \param [inout] entry - relevant entry.
 * \return
 *     \retval Negative in case of an error. When entry is not
 *             found, will return an error.
 *     \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_l3_vpbr_ipv4_entry_get(
    int unit,
    int global_in_lif,
    bcm_l3_vpbr_entry_t * entry)
{
    uint32 entry_access_id;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPV4_VRF_DB_TCAM, &entry_handle_id));

    VPBR_ENTRY_FIELD32_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_VRF, entry->vrf, entry->vrf_mask);

    VPBR_ENTRY_FIELD32_MASKED_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, global_in_lif,
                                           entry->l3_intf_id_mask);

    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV4_SIP, entry->sip_addr,
                                      entry->sip_addr_mask);

    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV4_DIP, entry->dip_addr,
                                      entry->dip_addr_mask);

    VPBR_ENTRY_FIELD32_MASKED_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_IP_PROTOCOL, entry->ip_protocol,
                                           entry->ip_protocol_mask);

    VPBR_ENTRY_FIELD32_MASKED_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_IPV4_TOS, entry->dscp, entry->dscp_mask);

    VPBR_ENTRY_FIELD32_MASKED_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_L4_SRC_PORT, entry->src_port,
                                           entry->src_port_mask);

    VPBR_ENTRY_FIELD32_MASKED_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_L4_DST_PORT, entry->dst_port,
                                           entry->dst_port_mask);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_VRF, INST_SINGLE, (uint32 *) &entry->new_vrf);

    SHR_IF_ERR_EXIT(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &entry_access_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, (DBAL_COMMIT)));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * get ipv6 entry from L3 vPBR database
 *     \param [in] unit - Relevant unit.
 *     \param [in] global_in_lif - relevant Inlif
 *     \param [inout] entry - relevant entry.
 * \return
 *     \retval Negative in case of an error. When entry is not
 *             found, will return an error.
 *     \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_l3_vpbr_ipv6_entry_get(
    int unit,
    int global_in_lif,
    bcm_l3_vpbr_entry_t * entry)
{
    uint32 entry_access_id;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPV6_VRF_DB_TCAM, &entry_handle_id));

    VPBR_ENTRY_FIELD32_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_VRF, entry->vrf, entry->vrf_mask);

    VPBR_ENTRY_FIELD32_MASKED_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, global_in_lif,
                                           entry->l3_intf_id_mask);

    dbal_entry_key_field_arr8_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV6_SIP, entry->sip6_addr,
                                         entry->sip6_addr_mask);

    dbal_entry_key_field_arr8_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV6_DIP, entry->dip6_addr,
                                         entry->dip6_addr_mask);

    VPBR_ENTRY_FIELD32_MASKED_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_IP_PROTOCOL, entry->ip_protocol,
                                           entry->ip_protocol_mask);

    VPBR_ENTRY_FIELD32_MASKED_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_IPV6_TC, entry->dscp, entry->dscp_mask);

    VPBR_ENTRY_FIELD32_MASKED_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_L4_SRC_PORT, entry->src_port,
                                           entry->src_port_mask);

    VPBR_ENTRY_FIELD32_MASKED_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_L4_DST_PORT, entry->dst_port,
                                           entry->dst_port_mask);

    SHR_IF_ERR_EXIT(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &entry_access_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_VRF, INST_SINGLE, (uint32 *) &entry->new_vrf);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, (DBAL_COMMIT)));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * get entry from L3 vPBR database.
 *
 *     \param [in] unit - Relevant unit.
 *     \param [inout] entry - relevant entry.
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
    int global_in_lif = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l3_vpbr_entry_verify(unit, entry));

    if (entry->l3_intf_id_mask != DNX_L3_VPBR_NOT_VALID_MASK)
    {
        global_in_lif = BCM_L3_ITF_VAL_GET((entry->l3_intf_id & entry->l3_intf_id_mask));
    }

    if (_SHR_IS_FLAG_SET(entry->flags, BCM_L3_VPBR_IP6))
    {
        SHR_IF_ERR_EXIT(dnx_l3_vpbr_ipv6_entry_get(unit, global_in_lif, entry));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_l3_vpbr_ipv4_entry_get(unit, global_in_lif, entry));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * delete ipv4 entry from from L3 vPBR database
 *     \param [in] unit - Relevant unit.
 *     \param [in] global_in_lif - relevant Inlif
 *     \param [in] entry - relevant entry.
 * \return
 *     \retval Negative in case of an error. When entry is not
 *             found, will return an error.
 *     \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_l3_vpbr_ipv4_entry_delete(
    int unit,
    int global_in_lif,
    bcm_l3_vpbr_entry_t * entry)
{
    uint32 entry_access_id;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPV4_VRF_DB_TCAM, &entry_handle_id));

    VPBR_ENTRY_FIELD32_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_VRF, entry->vrf, entry->vrf_mask);

    VPBR_ENTRY_FIELD32_MASKED_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, global_in_lif,
                                           entry->l3_intf_id_mask);

    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV4_SIP, entry->sip_addr,
                                      entry->sip_addr_mask);

    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV4_DIP, entry->dip_addr,
                                      entry->dip_addr_mask);

    VPBR_ENTRY_FIELD32_MASKED_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_IP_PROTOCOL, entry->ip_protocol,
                                           entry->ip_protocol_mask);

    VPBR_ENTRY_FIELD32_MASKED_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_IPV4_TOS, entry->dscp, entry->dscp_mask);

    VPBR_ENTRY_FIELD32_MASKED_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_L4_SRC_PORT, entry->src_port,
                                           entry->src_port_mask);

    VPBR_ENTRY_FIELD32_MASKED_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_L4_DST_PORT, entry->dst_port,
                                           entry->dst_port_mask);

    SHR_IF_ERR_EXIT(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &entry_access_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dnx_field_entry_access_id_destroy(unit, DBAL_TABLE_IPV4_VRF_DB_TCAM, entry_access_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * delete ipv6 entry from from L3 vPBR database
 *     \param [in] unit - Relevant unit.
 *     \param [in] global_in_lif - relevant Inlif
 *     \param [in] entry - relevant entry.
 * \return
 *     \retval Negative in case of an error. When entry is not
 *             found, will return an error.
 *     \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_l3_vpbr_ipv6_entry_delete(
    int unit,
    int global_in_lif,
    bcm_l3_vpbr_entry_t * entry)
{
    uint32 entry_access_id;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPV6_VRF_DB_TCAM, &entry_handle_id));

    VPBR_ENTRY_FIELD32_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_VRF, entry->vrf, entry->vrf_mask);

    VPBR_ENTRY_FIELD32_MASKED_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, global_in_lif,
                                           entry->l3_intf_id_mask);

    dbal_entry_key_field_arr8_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV6_SIP, entry->sip6_addr,
                                         entry->sip6_addr_mask);

    dbal_entry_key_field_arr8_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV6_DIP, entry->dip6_addr,
                                         entry->dip6_addr_mask);

    VPBR_ENTRY_FIELD32_MASKED_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_IP_PROTOCOL, entry->ip_protocol,
                                           entry->ip_protocol_mask);

    VPBR_ENTRY_FIELD32_MASKED_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_IPV6_TC, entry->dscp, entry->dscp_mask);

    VPBR_ENTRY_FIELD32_MASKED_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_L4_SRC_PORT, entry->src_port,
                                           entry->src_port_mask);

    VPBR_ENTRY_FIELD32_MASKED_SET_IF_VALID(unit, entry_handle_id, DBAL_FIELD_L4_DST_PORT, entry->dst_port,
                                           entry->dst_port_mask);

    SHR_IF_ERR_EXIT(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &entry_access_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, (DBAL_COMMIT)));

    SHR_IF_ERR_EXIT(dnx_field_entry_access_id_destroy(unit, DBAL_TABLE_IPV6_VRF_DB_TCAM, entry_access_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * delete entry from from L3 vPBR database (ipb4 \ ipv6)
 *
 *     \param [in] unit - Relevant unit.
 *     \param [in] entry - relevant entry.
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
    int global_in_lif = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l3_vpbr_entry_verify(unit, entry));

    if (entry->l3_intf_id_mask != DNX_L3_VPBR_NOT_VALID_MASK)
    {
        global_in_lif = BCM_L3_ITF_VAL_GET((entry->l3_intf_id & entry->l3_intf_id_mask));
    }

    if (_SHR_IS_FLAG_SET(entry->flags, BCM_L3_VPBR_IP6))
    {
        SHR_IF_ERR_EXIT(dnx_l3_vpbr_ipv6_entry_delete(unit, global_in_lif, entry));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_l3_vpbr_ipv4_entry_delete(unit, global_in_lif, entry));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  delete all entries from specific application table
 *     \param [in] unit - Relevant unit.
 *     \param [in] entry - relevant entry.
 *                 flags field is relevant for this function.
 *                 in case flags = 0 IPV4 is assumed.
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

    if (_SHR_IS_FLAG_SET(entry->flags, BCM_L3_VPBR_IP6))
    {
        SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_IPV6_VRF_DB_TCAM));
        SHR_IF_ERR_EXIT(dnx_field_entry_access_id_destroy_all(unit, DBAL_TABLE_IPV6_VRF_DB_TCAM));
    }
    else
    {
        /*
         * In case no flag is specified it is IPV4
         */
        SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_IPV4_VRF_DB_TCAM));
        SHR_IF_ERR_EXIT(dnx_field_entry_access_id_destroy_all(unit, DBAL_TABLE_IPV4_VRF_DB_TCAM));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Traverse all L3 vPBR ipv4 entries in DNX devices and call a
 *  function with the data for each entry found.
 * \param [in] unit - The unit number.
 * \param [in] cb - the name of the callback function that
 *        is going to be called for each valid entry
 * \param [in] user_data - data that is passed to the callback
 *        function
 * \return
 *   \retval Zero in case of no error.
 *   \retval Non-zero in case of an error.
 * \remark
 *      DBAL_TABLE_IPV4_VRF_DB_TCAM
 * \see
 *   * bcm_dnx_l3_vpbr_traverse \n
 */
static shr_error_e
dnx_l3_vpbr_ipv4_traverse(
    int unit,
    bcm_l3_vpbr_traverse_cb cb,
    void *user_data)
{
    uint32 entry_handle_id;
    uint32 index = 0;
    int is_end;
    bcm_l3_vpbr_entry_t vpbr_entry;
    uint32 field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
    uint32 field_mask[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Allocate handle to the table of the iteration and initialise an iterator entity.
     * The iterator is in mode ALL, which means that it will consider all entries regardless
     * of them being default entries or not.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPV4_VRF_DB_TCAM, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
    /*
     * Receive first entry in table.
     */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        bcm_l3_vpbr_entry_t_init(&vpbr_entry);

        vpbr_entry.flags = 0;
        vpbr_entry.priority = index;

        /*
         * Receive key and value fields of the entry.
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                        (unit, entry_handle_id, DBAL_FIELD_VRF, field_value, field_mask));
        vpbr_entry.vrf = field_value[0];
        vpbr_entry.vrf_mask = field_mask[0];

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                        (unit, entry_handle_id, DBAL_FIELD_IPV4_SIP, field_value, field_mask));
        vpbr_entry.sip_addr = field_value[0];
        vpbr_entry.sip_addr_mask = field_mask[0];
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                        (unit, entry_handle_id, DBAL_FIELD_IPV4_DIP, field_value, field_mask));
        vpbr_entry.dip_addr = field_value[0];
        vpbr_entry.dip_addr_mask = field_mask[0];

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                        (unit, entry_handle_id, DBAL_FIELD_IP_PROTOCOL, field_value, field_mask));
        vpbr_entry.ip_protocol = field_value[0];
        vpbr_entry.ip_protocol_mask = field_mask[0];

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                        (unit, entry_handle_id, DBAL_FIELD_IPV4_TOS, field_value, field_mask));
        vpbr_entry.dscp = field_value[0];
        vpbr_entry.dscp_mask = field_mask[0];

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                        (unit, entry_handle_id, DBAL_FIELD_L4_SRC_PORT, field_value, field_mask));
        vpbr_entry.src_port = field_value[0];
        vpbr_entry.src_port_mask = field_mask[0];

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                        (unit, entry_handle_id, DBAL_FIELD_L4_DST_PORT, field_value, field_mask));
        vpbr_entry.dst_port = field_value[0];
        vpbr_entry.dst_port_mask = field_mask[0];

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                        (unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, field_value, field_mask));
        BCM_L3_ITF_SET(vpbr_entry.l3_intf_id, _SHR_L3_ITF_TYPE_LIF, field_value[0]);
        vpbr_entry.l3_intf_id_mask = field_mask[0];

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_VRF, INST_SINGLE, field_value));
        vpbr_entry.new_vrf = field_value[0];

        /*
         * If user provided a name of the callback function, it will be invoked with sending the vpbr_entry structure
         * of the entry that was found.
         */
        if (cb != NULL)
        {
            /*
             * Invoke callback function
             */
            SHR_IF_ERR_EXIT((*cb) (unit, &vpbr_entry, user_data));
        }
        /*
         * Receive next entry in table.
         */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        index++;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Traverse all L3 vPBR ipv6 entries in DNX devices and call a
 *  function with the data for each entry found.
 * \param [in] unit - The unit number.
 * \param [in] cb - the name of the callback function that
 *        is going to be called for each valid entry
 * \param [in] user_data - data that is passed to the callback
 *        function
 * \return
 *   \retval Zero in case of no error.
 *   \retval Non-zero in case of an error.
 * \remark
 *      DBAL_TABLE_IPV6_VRF_DB_TCAM
 * \see
 *   * bcm_dnx_l3_vpbr_traverse \n
 */
static shr_error_e
dnx_l3_vpbr_ipv6_traverse(
    int unit,
    bcm_l3_vpbr_traverse_cb cb,
    void *user_data)
{
    uint32 entry_handle_id;
    uint32 index = 0;
    int is_end;
    bcm_l3_vpbr_entry_t vpbr_entry;
    uint32 field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
    uint32 field_mask[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Allocate handle to the table of the iteration and initialise an iterator entity.
     * The iterator is in mode ALL, which means that it will consider all entries regardless
     * of them being default entries or not.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPV6_VRF_DB_TCAM, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
    /*
     * Receive first entry in table.
     */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        bcm_l3_vpbr_entry_t_init(&vpbr_entry);

        vpbr_entry.flags = BCM_L3_VPBR_IP6;
        vpbr_entry.priority = index;

        /*
         * Receive key and value fields of the entry.
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                        (unit, entry_handle_id, DBAL_FIELD_VRF, field_value, field_mask));
        vpbr_entry.vrf = field_value[0];
        vpbr_entry.vrf_mask = field_mask[0];

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                        (unit, entry_handle_id, DBAL_FIELD_IPV6_SIP, field_value, field_mask));
        sal_memcpy(vpbr_entry.sip6_addr, field_value, sizeof(bcm_ip6_t));
        sal_memcpy(vpbr_entry.sip6_addr_mask, field_mask, sizeof(bcm_ip6_t));

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                        (unit, entry_handle_id, DBAL_FIELD_IPV6_DIP, field_value, field_mask));
        sal_memcpy(vpbr_entry.dip6_addr, field_value, sizeof(bcm_ip6_t));
        sal_memcpy(vpbr_entry.dip6_addr_mask, field_mask, sizeof(bcm_ip6_t));

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                        (unit, entry_handle_id, DBAL_FIELD_IP_PROTOCOL, field_value, field_mask));
        vpbr_entry.ip_protocol = field_value[0];
        vpbr_entry.ip_protocol_mask = field_mask[0];

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                        (unit, entry_handle_id, DBAL_FIELD_IPV6_TC, field_value, field_mask));
        vpbr_entry.dscp = field_value[0];
        vpbr_entry.dscp_mask = field_mask[0];

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                        (unit, entry_handle_id, DBAL_FIELD_L4_SRC_PORT, field_value, field_mask));
        vpbr_entry.src_port = field_value[0];
        vpbr_entry.src_port_mask = field_mask[0];

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                        (unit, entry_handle_id, DBAL_FIELD_L4_DST_PORT, field_value, field_mask));
        vpbr_entry.dst_port = field_value[0];
        vpbr_entry.dst_port_mask = field_mask[0];

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                        (unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, field_value, field_mask));
        BCM_L3_ITF_SET(vpbr_entry.l3_intf_id, _SHR_L3_ITF_TYPE_LIF, field_value[0]);
        vpbr_entry.l3_intf_id_mask = field_mask[0];

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_VRF, INST_SINGLE, field_value));
        vpbr_entry.new_vrf = field_value[0];

        /*
         * If user provided a name of the callback function, it will be invoked with sending the vpbr_entry structure
         * of the entry that was found.
         */
        if (cb != NULL)
        {
            /*
             * Invoke callback function
             */
            SHR_IF_ERR_EXIT((*cb) (unit, &vpbr_entry, user_data));
        }
        /*
         * Receive next entry in table.
         */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        index++;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Traverse all L3 vPBR entries in DNX devices and call a
 *  function with the data for each entry found.
 * \param [in] unit - The unit number.
 * \param [in] flags - control flags that indicate choice of
 *        table to be traversed:
 *          * BCM_L3_VPBR_IP6 - if provided, it indicates that
 *            the IPv6 table entries will be traversed,
 *            otherwise the IPv4 table entries will be
 *            traversed.
 * \param [in] cb - the name of the callback function that is
 *        going to be called for each valid entry
 * \param [in] user_data - data that is passed to the callback
 *        function
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
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l3_vpbr_traverse_verify(unit, flags));

    /*
     * In case no call back is provided return and do nothing
     */
    if (cb == NULL)
    {
        SHR_EXIT();
    }
    /*
     * Determine the correct table from which all vpbr entries will be traversed based on the BCM_L3_VPBR_IP6 flag.
     * If the BCM_L3_VPBR_IP6 flag is not set, then IPv4 vpbr_entry table will be traversed.
     */
    if (_SHR_IS_FLAG_SET(flags, BCM_L3_VPBR_IP6))
    {
        SHR_IF_ERR_EXIT(dnx_l3_vpbr_ipv6_traverse(unit, cb, user_data));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_l3_vpbr_ipv4_traverse(unit, cb, user_data));
    }

exit:
    SHR_FUNC_EXIT;
}
