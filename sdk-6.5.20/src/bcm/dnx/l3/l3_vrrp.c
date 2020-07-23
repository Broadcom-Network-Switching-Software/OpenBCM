/** \file l3_vrrp.c
 *
 * Handles the control over the ECMPs entries.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_L3

/*
 * Include files currently used for DNX.
 * {
 */
#include <bcm_int/dnx/l3/l3_vrrp.h>
#include <bcm_int/dnx/l3/l3.h>
#include <bcm_int/dnx/vsi/vsi.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/utilex/utilex_framework.h>
#include <shared/bitop.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_l3_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/vlan_access.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

/*
 * }
 */
/*
 * Include files.
 * {
 */
#include <bcm/l3.h>
#include <bcm/types.h>
#include <bcm/switch.h>
#include <bcm_int/dnx_dispatch.h>
/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/**
 * \brief - default protocol groups that are set on init.
 * Group 3 holds IPv6
 * Group 1 holds IPv4
 * Group 0 holds all the rest of L3 protocols
 */
#define DNX_L3_VRRP_L3_PROTOCOL_GROUP_ID_DEFAULT        0X0
#define DNX_L3_VRRP_L3_PROTOCOL_GROUP_ID_IPV6_DEFAULT   0X1
#define DNX_L3_VRRP_L3_PROTOCOL_GROUP_ID_IPV4_DEFAULT   0X3
/*
 * VLAN size mask
 */
#define DNX_L3_VRRP_L3_VLAN_FIELD_FULL_MASK             0xFFFF

/**
 * \brief first valid group ID. Used in iterations.
 */
#define DNX_L3_VRRP_L3_PROTOCOL_GROUP_ID_LOWEST_VALID   0x1

/**
 * Invalid protocol group or set of l3 protocols. Used as an indication for bcm_dnx_switch_l3_protocol_group_get() API
 * that specifying that the attribute should be retrieved.
 */
#define DNX_L3_VRRP_PROTOCOL_GROUP_INVALID              ((uint32) -1)

/**
 * Full mask of protocol group
 */
#define DNX_L3_VRRP_PROTOCOL_GROUP_MASK_FULL            ((uint32) -1)

/**
 * Maximal VRID value
 */
#define DNX_L3_VRRP_VRID_MAX                            (255)
#define DNX_L3_VRRP_VRID_NOF_BITS                       (8)

/*
 * }
 */

/*
 * MACROs
 * {
 */

/**
 * \brief - Given a VSI, returns true if it's the vrrp's "configure all VSIs" vsi.
 */
#define DNX_L3_VRRP_VLAN_IS_ALL_VSI(_vsi) (_vsi == 0)

/**
 * \brief - EXEM FFC defines
 * These defines are used to construct EXEM FFCs according to each field offset and width (mask).
 * FFC data is as following:
 * {DA(48),VSI(18),In-LIF[0](22),In-Port(8),Layer-Protocol[Forwarding-Layer-Index+1](5), Vrid-My-Mac-Key-Gen-Var(8)},
 * Total width 109 bits.
 */
#define DNX_L3_VRRP_FFC_MAC_WIDTH                    48
#define DNX_L3_VRRP_FFC_VSI_WIDTH                    18
#define DNX_L3_VRRP_FFC_IN_LIF_WIDTH                 22
#define DNX_L3_VRRP_FFC_IN_PORT_WIDTH(_unit)         (dnx_data_port.general.nof_port_bits_in_pp_bus_get(_unit))
#define DNX_L3_VRRP_FFC_LAYER_PROTOCOL_WIDTH         5
#define DNX_L3_VRRP_FFC_GEN_VAR_WIDTH                8
#define DNX_L3_VRRP_FFC_DATA_TOTAL_WIDTH(_unit)      (DNX_L3_VRRP_FFC_MAC_WIDTH + \
                                                      DNX_L3_VRRP_FFC_VSI_WIDTH + \
                                                      DNX_L3_VRRP_FFC_IN_LIF_WIDTH + \
                                                      DNX_L3_VRRP_FFC_IN_PORT_WIDTH(_unit) + \
                                                      DNX_L3_VRRP_FFC_LAYER_PROTOCOL_WIDTH + \
                                                      DNX_L3_VRRP_FFC_GEN_VAR_WIDTH)

#define DNX_L3_VRRP_FFC_MAC_OFFSET(_unit)            (DNX_L3_VRRP_FFC_DATA_TOTAL_WIDTH(_unit) - DNX_L3_VRRP_FFC_MAC_WIDTH)
#define DNX_L3_VRRP_FFC_VSI_OFFSET(_unit)            (DNX_L3_VRRP_FFC_MAC_OFFSET(_unit) - DNX_L3_VRRP_FFC_VSI_WIDTH)
#define DNX_L3_VRRP_FFC_IN_LIF_OFFSET(_unit)         (DNX_L3_VRRP_FFC_VSI_OFFSET(_unit) - DNX_L3_VRRP_FFC_IN_LIF_WIDTH)
#define DNX_L3_VRRP_FFC_IN_PORT_OFFSET(_unit)        (DNX_L3_VRRP_FFC_IN_LIF_OFFSET(_unit) - DNX_L3_VRRP_FFC_IN_PORT_WIDTH(_unit))
#define DNX_L3_VRRP_FFC_LAYER_PROTOCOL_OFFSET(_unit) (DNX_L3_VRRP_FFC_IN_PORT_OFFSET(_unit) - DNX_L3_VRRP_FFC_LAYER_PROTOCOL_WIDTH)
#define DNX_L3_VRRP_FFC_GEN_VAR_OFFSET(_unit)        (DNX_L3_VRRP_FFC_LAYER_PROTOCOL_OFFSET(_unit) - DNX_L3_VRRP_FFC_GEN_VAR_WIDTH)

/**
 * \brief - Macros for building FFC payload for VRRP EXEM.
 * Key offset (6b) - offset to the location in the created payload.
 * Field width (6b) - field width. How many bits to copy to the payload.
 * FFC data (7b) - Copy start bit from the concatenated 109 bits of the resources.
 */
#define DNX_L3_VRRP_FFC_KEY_OFFSET_MASK 0x3f
#define DNX_L3_VRRP_FFC_FIELD_WIDTH_MASK 0x3f
#define DNX_L3_VRRP_FFC_DATA_MASK 0x7f
#define DNX_L3_VRRP_FFC_KEY_OFFSET_WIDTH 6
#define DNX_L3_VRRP_FFC_FIELD_WIDTH_WIDTH 6
#define DNX_L3_VRRP_FFC_DATA_WIDTH 7

/**
 *  \brief - FFC construction MACRO
 */
#define DNX_L3_VRRP_EXEM_FFC_BUILD(key_offset, field_width, ffc_data)  \
 (((key_offset) & DNX_L3_VRRP_FFC_KEY_OFFSET_MASK) | \
 (((field_width) & DNX_L3_VRRP_FFC_FIELD_WIDTH_MASK)<<DNX_L3_VRRP_FFC_KEY_OFFSET_WIDTH) | \
  (((ffc_data) & DNX_L3_VRRP_FFC_DATA_MASK)<<(DNX_L3_VRRP_FFC_KEY_OFFSET_WIDTH+DNX_L3_VRRP_FFC_FIELD_WIDTH_WIDTH)))

/*
 * Return the protocol from the flags field, it assume that only one of the IP flags is set
 */
#define DNX_L3_VRRP_IP_PROTOCOL_FROM_FLAG_GET(flags) ((_SHR_IS_FLAG_SET(flags, BCM_L3_VRRP_IPV4)) ? DBAL_ENUM_FVAL_LAYER_TYPES_IPV4 : DBAL_ENUM_FVAL_LAYER_TYPES_IPV6 )

/*
 * }
 */
/*
 * Enumeration.
 * {
 */

/*
 * }
 */
/*
 * }
 */
/*
 * Structures
 * {
 */

/*
 * Globals.
 * {
 */

/**
 * brief - Dedicated mac address that are used for VRRP protocol (ipv4, ipv6 and a full mask)
 */
static const bcm_mac_t dnx_l3_vrrp_vrid_mac_ipv4 = { 0x00, 0x00, 0x5e, 0x00, 0x01, 0x00 };
static const bcm_mac_t dnx_l3_vrrp_vrid_mac_ipv6 = { 0x00, 0x00, 0x5e, 0x00, 0x02, 0x00 };
static const bcm_mac_t dnx_l3_vrrp_vrid_mac_mask = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
/*
 * }
 */
/*
 * Functions
 * {
 */

static shr_error_e
dnx_l3_vrrp_switch_protocol_group_members_all_valid_set(
    int unit,
    uint32 *target)
{
    int offset_ipv4, offset_ipv6, offset_arp, offset_mpls, offset_ethernet, offset_fcoe;
    SHR_FUNC_INIT_VARS(unit);

    offset_ipv4 = dnx_init_pp_layer_types_value_get(unit, DBAL_ENUM_FVAL_LAYER_TYPES_IPV4);
    offset_ipv6 = dnx_init_pp_layer_types_value_get(unit, DBAL_ENUM_FVAL_LAYER_TYPES_IPV6);
    offset_arp = dnx_init_pp_layer_types_value_get(unit, DBAL_ENUM_FVAL_LAYER_TYPES_ARP);
    offset_mpls = dnx_init_pp_layer_types_value_get(unit, DBAL_ENUM_FVAL_LAYER_TYPES_MPLS);
    offset_ethernet = dnx_init_pp_layer_types_value_get(unit, DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET);
    offset_fcoe = dnx_init_pp_layer_types_value_get(unit, DBAL_ENUM_FVAL_LAYER_TYPES_FCOE);

    if (offset_ipv4 >= 0)
    {
        SHR_BITSET(target, offset_ipv4);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid offset value ");
    }
    if (offset_ipv6 >= 0)
    {
        SHR_BITSET(target, offset_ipv6);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid offset value ");
    }
    if (offset_arp >= 0)
    {
        SHR_BITSET(target, offset_arp);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid offset value ");
    }
    if (offset_mpls >= 0)
    {
        SHR_BITSET(target, offset_mpls);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid offset value ");
    }
    if (offset_ethernet >= 0)
    {
        SHR_BITSET(target, offset_ethernet);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid offset value ");
    }
    if (offset_fcoe >= 0)
    {
        SHR_BITSET(target, offset_fcoe);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid offset value ");
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Given the switch api's l3 protocol group flags, returns the internal flags.
 */
static shr_error_e
dnx_l3_vrrp_switch_protocol_group_api_flags_to_internal_translate(
    int unit,
    uint32 api_flags,
    uint32 *internal_flags)
{
    int offset = 0;
    SHR_FUNC_INIT_VARS(unit);

    *internal_flags = 0;

    if (_SHR_IS_FLAG_SET(api_flags, BCM_SWITCH_L3_PROTOCOL_GROUP_IPV4))
    {
        offset = dnx_init_pp_layer_types_value_get(unit, DBAL_ENUM_FVAL_LAYER_TYPES_IPV4);
        if (offset >= 0)
        {
            SHR_BITSET(internal_flags, offset);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid offset value ");
        }
    }
    if (_SHR_IS_FLAG_SET(api_flags, BCM_SWITCH_L3_PROTOCOL_GROUP_IPV6))
    {
        offset = dnx_init_pp_layer_types_value_get(unit, DBAL_ENUM_FVAL_LAYER_TYPES_IPV6);
        if (offset >= 0)
        {
            SHR_BITSET(internal_flags, offset);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid offset value ");
        }
    }
    if (_SHR_IS_FLAG_SET(api_flags, BCM_SWITCH_L3_PROTOCOL_GROUP_ARP))
    {
        offset = dnx_init_pp_layer_types_value_get(unit, DBAL_ENUM_FVAL_LAYER_TYPES_ARP);
        if (offset >= 0)
        {
            SHR_BITSET(internal_flags, offset);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid offset value ");
        }
    }
    if (_SHR_IS_FLAG_SET(api_flags, BCM_SWITCH_L3_PROTOCOL_GROUP_MPLS))
    {
        offset = dnx_init_pp_layer_types_value_get(unit, DBAL_ENUM_FVAL_LAYER_TYPES_MPLS);
        if (offset >= 0)
        {
            SHR_BITSET(internal_flags, offset);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid offset value ");
        }
    }
    if (_SHR_IS_FLAG_SET(api_flags, BCM_SWITCH_L3_PROTOCOL_GROUP_MIM))
    {
        offset = dnx_init_pp_layer_types_value_get(unit, DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET);
        if (offset >= 0)
        {
            SHR_BITSET(internal_flags, offset);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid offset value ");
        }
    }
    if (_SHR_IS_FLAG_SET(api_flags, BCM_SWITCH_L3_PROTOCOL_GROUP_FCOE))
    {
        offset = dnx_init_pp_layer_types_value_get(unit, DBAL_ENUM_FVAL_LAYER_TYPES_FCOE);
        if (offset >= 0)
        {
            SHR_BITSET(internal_flags, offset);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid offset value ");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Given the internal l3 protocol group flags, returns the switch api flags.
 */
static shr_error_e
dnx_l3_vrrp_switch_protocol_group_internal_flags_to_api_translate(
    int unit,
    uint32 *internal_flags,
    uint32 *api_flags)
{
    int offset_ipv4, offset_ipv6, offset_arp, offset_mpls, offset_ethernet, offset_fcoe;
    SHR_FUNC_INIT_VARS(unit);

    *api_flags = 0;
    offset_ipv4 = dnx_init_pp_layer_types_value_get(unit, DBAL_ENUM_FVAL_LAYER_TYPES_IPV4);
    offset_ipv6 = dnx_init_pp_layer_types_value_get(unit, DBAL_ENUM_FVAL_LAYER_TYPES_IPV6);
    offset_arp = dnx_init_pp_layer_types_value_get(unit, DBAL_ENUM_FVAL_LAYER_TYPES_ARP);
    offset_mpls = dnx_init_pp_layer_types_value_get(unit, DBAL_ENUM_FVAL_LAYER_TYPES_MPLS);
    offset_ethernet = dnx_init_pp_layer_types_value_get(unit, DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET);
    offset_fcoe = dnx_init_pp_layer_types_value_get(unit, DBAL_ENUM_FVAL_LAYER_TYPES_FCOE);

    if (offset_ipv4 >= 0)
    {
        if (SHR_BITGET(internal_flags, offset_ipv4))
        {
            *api_flags |= BCM_SWITCH_L3_PROTOCOL_GROUP_IPV4;
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid offset value ");
    }
    if (offset_ipv6 >= 0)
    {
        if (SHR_BITGET(internal_flags, offset_ipv6))
        {
            *api_flags |= BCM_SWITCH_L3_PROTOCOL_GROUP_IPV6;
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid offset value ");
    }
    if (offset_arp >= 0)
    {
        if (SHR_BITGET(internal_flags, offset_arp))
        {
            *api_flags |= BCM_SWITCH_L3_PROTOCOL_GROUP_ARP;
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid offset value ");
    }
    if (offset_mpls >= 0)
    {
        if (SHR_BITGET(internal_flags, offset_mpls))
        {
            *api_flags |= BCM_SWITCH_L3_PROTOCOL_GROUP_MPLS;
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid offset value ");
    }
    if (offset_ethernet >= 0)
    {
        if (SHR_BITGET(internal_flags, offset_ethernet))
        {
            *api_flags |= BCM_SWITCH_L3_PROTOCOL_GROUP_MIM;
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid offset value ");
    }
    if (offset_fcoe >= 0)
    {
        if (SHR_BITGET(internal_flags, offset_fcoe))
        {
            *api_flags |= BCM_SWITCH_L3_PROTOCOL_GROUP_FCOE;
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid offset value ");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Given the vrrp api's l3 protocol group flags, returns the internal flags.
 */
static shr_error_e
dnx_l3_vrrp_protocol_group_api_flags_to_internal_translate(
    int unit,
    uint32 api_flags,
    uint32 *internal_flags)
{
    SHR_FUNC_INIT_VARS(unit);

    *internal_flags = 0;

    if (_SHR_IS_FLAG_SET(api_flags, BCM_L3_VRRP_IPV4))
    {
        SHR_BITSET(internal_flags, DBAL_ENUM_FVAL_LAYER_TYPES_IPV4);
    }
    else
    {
        SHR_BITSET(internal_flags, DBAL_ENUM_FVAL_LAYER_TYPES_IPV6);
    }

/** exit: */
    SHR_FUNC_EXIT;
}

/**
 * \brief - Given the internal l3 protocol group flags, returns the vrrp api flags.
 */
shr_error_e
dnx_l3_vrrp_protocol_group_internal_flags_to_api_translate(
    int unit,
    uint32 internal_flags,
    uint32 *api_flags)
{
    SHR_FUNC_INIT_VARS(unit);

    *api_flags = 0;

    if (SHR_BITGET(&internal_flags, DBAL_ENUM_FVAL_LAYER_TYPES_IPV4))
    {
        *api_flags |= BCM_L3_VRRP_IPV4;
    }
    else
    {
        *api_flags |= BCM_L3_VRRP_IPV6;
    }

/** exit: */
    SHR_FUNC_EXIT;
}

/**
 * \brief - Map protocol group to its members(set of l3 protocols). The members are represented as a bitmap of
 *  protocols.
 *  See \ref dbal_enum_value_field_layer_types_e for a list of supported protocols and its bit ID.
 *  The mapping is maintained in a dedicated SW state db.
 */
static shr_error_e
dnx_l3_vrrp_protocol_group_members_sw_set(
    int unit,
    int group_id,
    uint32 members_bmp)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set protocol group dbal SW table */

    /** create handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L3_VRRP_PROTOCOL_GROUP_SW, &entry_handle_id));

    /** set key - group id*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_GROUP_ID, group_id);
    /** set value - protocol members bitmap */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_L3_PROTOCOLS_BITMAP, INST_SINGLE, members_bmp);

    /** commit */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get protocol group (group id can be masked) members (set of l3 protocols). The members are represented as
 * a bitmap of protocols.
 * See \ref dbal_enum_value_field_layer_types_e for a list of supported protocols and its bit ID.
 * The mapping is maintained in a dedicated SW state db.
 */
shr_error_e
dnx_l3_vrrp_protocol_group_members_sw_get(
    int unit,
    int group_id,
    int group_mask,
    uint32 *members_bmp)
{
    uint32 entry_handle_id, temp_members;
    int ii;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get protocol group dbal SW table */

    /** create handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L3_VRRP_PROTOCOL_GROUP_SW, &entry_handle_id));

    *members_bmp = 0;
    /** Read all groups from SW database. If the group matches the mask, add its members to the return value. */
    for (ii = 0; ii < dnx_data_l3.vrrp.nof_protocol_groups_get(unit); ii++)
    {
        if ((ii & group_mask) == (group_id & group_mask))
        {
            temp_members = 0;

            /** set key - group id*/
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_GROUP_ID, ii);
            /** Get value - protocol members bitmap */
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_L3_PROTOCOLS_BITMAP, INST_SINGLE,
                                       &temp_members);

            /** commit */
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

            /** add current group protocol members to the total members */
            *members_bmp |= temp_members;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get indication whether the given protocol group is used by VRRP or multiple My MAC rules
 */
shr_error_e
dnx_l3_vrrp_protocol_group_count_sw_get(
    int unit,
    int group_id,
    uint32 *count)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get protocol group dbal SW table */

    /** create handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L3_VRRP_PROTOCOL_GROUP_SW, &entry_handle_id));

    /** set key - group id*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_GROUP_ID, group_id);
    /** Get value - protocol members bitmap */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_NOF_L3_VRRP_TCAM_ENTRIES, INST_SINGLE, count);

    /** commit */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set whether the protocol group is used by VRRP or multiple My mac entries
 */
static shr_error_e
dnx_l3_vrrp_protocol_group_count_sw_set(
    int unit,
    int group_id,
    uint32 group_mask,
    int change)
{
    uint32 entry_handle_id;
    uint32 count;
    int i;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** create handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L3_VRRP_PROTOCOL_GROUP_SW, &entry_handle_id));

    /** iterate over all groups and update counter for mask matching groups */
    for (i = 0; i < dnx_data_l3.vrrp.nof_protocol_groups_get(unit); i++)
    {
        if ((i & group_mask) == (group_id & group_mask))
        {
            /** get current count */
            SHR_IF_ERR_EXIT(dnx_l3_vrrp_protocol_group_count_sw_get(unit, i, &count));

            count += change;

            /** clear old handle. */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_L3_VRRP_PROTOCOL_GROUP_SW, entry_handle_id));

            /** set key - group id*/
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_GROUP_ID, i);
            /** set value - counter */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_L3_VRRP_TCAM_ENTRIES, INST_SINGLE,
                                         count);

            /** commit */
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - map L3 protocol to protocol group.
 */
static shr_error_e
dnx_l3_vrrp_protocol_layer_to_group_id_hw_set(
    int unit,
    int protocol_layer,
    int group_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get protocol group dbal SW table */

    /** create handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L3_VRRP_PROTOCOL_GROUP, &entry_handle_id));

    /** set key - protocol */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPES, protocol_layer);
    /** Get value - protocol members bitmap */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_GROUP_ID, INST_SINGLE, group_id);

    /** commit */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Map protocol group to its members(set of l3 protocols). The members are represented as a bitmap of
 *  protocols.
 *  See \ref dbal_enum_value_field_layer_types_e for a list of supported protocols and its bit ID.
 *  HW mapping - map each protocol in the group to the group itself
 *  SW mapping - map group to a set of protocol (bitmap).
 */
static shr_error_e
dnx_l3_vrrp_protocol_group_members_set(
    int unit,
    int group_id,
    uint32 *members_bmp)
{
    int ii, member = 0;
    uint32 old_group_members[1], members_bmp_arr[1];
    uint32 curr_group_members;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    members_bmp_arr[0] = members_bmp[0];

    /** Get old members of the group so they can be set to group 0 */
    SHR_IF_ERR_EXIT(dnx_l3_vrrp_protocol_group_members_sw_get(unit, group_id, DNX_L3_VRRP_PROTOCOL_GROUP_MASK_FULL,
                                                              old_group_members));

    /*
     * Iterate over all the groups. When reaching group 0, update it to contain all the protocols that were removed
     * from group_id.
     * When reaching group_id, update it with the new members. From all other groups, remove the group members.
     */
    for (ii = 0; ii < dnx_data_l3.vrrp.nof_protocol_groups_get(unit); ii++)
    {
        SHR_IF_ERR_EXIT(dnx_l3_vrrp_protocol_group_members_sw_get(unit, ii, DNX_L3_VRRP_PROTOCOL_GROUP_MASK_FULL,
                                                                  &curr_group_members));

        /** Remove the new group members from all groups. */
        curr_group_members &= ~members_bmp_arr[0];

        if (ii == 0)
        {
            /** Set all old members of the group that are not in the group anymore to group 0. */
            curr_group_members |= (old_group_members[0] & ~members_bmp_arr[0]);
        }

        if (ii == group_id)
        {
            curr_group_members = members_bmp[0];
        }

        /** Set SW - group new members */
        SHR_IF_ERR_EXIT(dnx_l3_vrrp_protocol_group_members_sw_set(unit, ii, curr_group_members));
    }

    /** Write to HW  */
    {
        /** first, iterate over all the provided group protocols and clear them from the group (map to group 0)*/
        SHR_BIT_ITER(old_group_members, DBAL_NOF_ENUM_LAYER_TYPES_VALUES, member)
        {
            SHR_IF_ERR_EXIT(dnx_l3_vrrp_protocol_layer_to_group_id_hw_set(unit, member, 0));
        }

        /** second, map the new members to the group */
        SHR_BIT_ITER(members_bmp_arr, DBAL_NOF_ENUM_LAYER_TYPES_VALUES, member)
        {
            SHR_IF_ERR_EXIT(dnx_l3_vrrp_protocol_layer_to_group_id_hw_set(unit, member, group_id));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Init default protocol groups for vrrp protocol - IPV4, IPV6, and all the rest
 */
static shr_error_e
dnx_l3_vrrp_default_protocol_groups_init(
    int unit)
{
    int i;
    uint32 valid_members[1] = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    /** Initialize all valid protocols to be in group 0 in SW DB. HW is set by default. */
    SHR_IF_ERR_EXIT(dnx_l3_vrrp_switch_protocol_group_members_all_valid_set(unit, valid_members));
    SHR_IF_ERR_EXIT(dnx_l3_vrrp_protocol_group_members_sw_set(unit, DNX_L3_VRRP_L3_PROTOCOL_GROUP_ID_DEFAULT,
                                                              valid_members[0]));

    /** Clear rest of the groups sw state */
    for (i = DNX_L3_VRRP_L3_PROTOCOL_GROUP_ID_LOWEST_VALID; i < dnx_data_l3.vrrp.nof_protocol_groups_get(unit); i++)
    {
        SHR_IF_ERR_EXIT(dnx_l3_vrrp_protocol_group_members_sw_set(unit, i, 0));
    }

    /** Set IPV4 and IPV6 to their default groups to be later used by VRRP protocol legacy APIs */
    SHR_IF_ERR_EXIT(bcm_dnx_switch_l3_protocol_group_set(unit, BCM_SWITCH_L3_PROTOCOL_GROUP_IPV4,
                                                         DNX_L3_VRRP_L3_PROTOCOL_GROUP_ID_IPV4_DEFAULT));

    SHR_IF_ERR_EXIT(bcm_dnx_switch_l3_protocol_group_set(unit, BCM_SWITCH_L3_PROTOCOL_GROUP_IPV6,
                                                         DNX_L3_VRRP_L3_PROTOCOL_GROUP_ID_IPV6_DEFAULT));

exit:
    SHR_FUNC_EXIT;
}

/**
 * brief - Initialize EXEM based multiple my mac DA profiles (FFCs, APP DB ID, etc...)
 */
static shr_error_e
dnx_l3_vrrp_exem_based_multi_my_mac_da_profiles_init(
    int unit)
{
    uint32 entry_handle_id, app_db_id, exem_ffc;
    int app_id_size, core;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** get handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L3_VRRP_EXEM_DA_PROFILE_CONFIG, &entry_handle_id));

    /** set VRID_MY_MAC (VRRP protocol) */
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_L3_VRRP_EXEM_DA_PROFILE_CONFIG, entry_handle_id));

        /** set key */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_DA_PROFILE,
                                   DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_VRID_MY_MAC_WITH_PROTOCOL);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, _SHR_CORE_ALL);

        /** set app db */
        SHR_IF_ERR_EXIT(dbal_tables_app_db_id_get
                        (unit, DBAL_TABLE_EXEM_VRID_MY_MAC_WITH_PROTOCOL, &app_db_id, &app_id_size));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXEM_VRRP_APP_DB_ID, INST_SINGLE, app_db_id);

        /** Set FFC 0 - VRID (8 lsb bits of the mac address)*/
        exem_ffc = DNX_L3_VRRP_EXEM_FFC_BUILD(0, DNX_L3_VRRP_VRID_NOF_BITS - 1, DNX_L3_VRRP_FFC_MAC_OFFSET(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_FFC_INSTRUCTION, 0, exem_ffc);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_KBR_VALID, 0, TRUE);

        /** Set FFC 1 - VSI (use only 17 bits instead of total 18) */
        exem_ffc = DNX_L3_VRRP_EXEM_FFC_BUILD(DNX_L3_VRRP_VRID_NOF_BITS, DNX_L3_VRRP_FFC_VSI_WIDTH - 2,
                                              DNX_L3_VRRP_FFC_VSI_OFFSET(unit));

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_FFC_INSTRUCTION, 1, exem_ffc);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_KBR_VALID, 1, TRUE);

        /** Set FFC 2 - Layer FWD + 1 protocol */
        exem_ffc =
            DNX_L3_VRRP_EXEM_FFC_BUILD(DNX_L3_VRRP_VRID_NOF_BITS + DNX_L3_VRRP_FFC_VSI_WIDTH - 1,
                                       DNX_L3_VRRP_FFC_LAYER_PROTOCOL_WIDTH - 1,
                                       DNX_L3_VRRP_FFC_LAYER_PROTOCOL_OFFSET(unit));

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_FFC_INSTRUCTION, 2, exem_ffc);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_KBR_VALID, 2, TRUE);

        /** commit */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /** set PER_PORT_MY_MAC per core (multiple my mac based on Source Port and MAC address) */
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_L3_VRRP_EXEM_DA_PROFILE_CONFIG, entry_handle_id));

        /** set key */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_DA_PROFILE,
                                   DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_PER_PORT_MY_MAC);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

        /** set app db */
        SHR_IF_ERR_EXIT(dbal_tables_app_db_id_get(unit, DBAL_TABLE_EXEM_PER_PORT_MY_MAC, &app_db_id, &app_id_size));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXEM_VRRP_APP_DB_ID, INST_SINGLE, app_db_id);

        /** Set FFC 0 - MAC address */
        exem_ffc = DNX_L3_VRRP_EXEM_FFC_BUILD(0, DNX_L3_VRRP_FFC_MAC_WIDTH - 1, DNX_L3_VRRP_FFC_MAC_OFFSET(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_FFC_INSTRUCTION, 0, exem_ffc);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_KBR_VALID, 0, TRUE);

        /** Set FFC 1 - source port */
        exem_ffc = DNX_L3_VRRP_EXEM_FFC_BUILD(DNX_L3_VRRP_FFC_MAC_WIDTH, DNX_L3_VRRP_FFC_IN_PORT_WIDTH(unit) - 1,
                                              DNX_L3_VRRP_FFC_IN_PORT_OFFSET(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_FFC_INSTRUCTION, 1, exem_ffc);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_KBR_VALID, 1, TRUE);

        /** Set FFC 2 - source port core (1 bit) */
        exem_ffc = DNX_L3_VRRP_EXEM_FFC_BUILD(DNX_L3_VRRP_FFC_MAC_WIDTH + DNX_L3_VRRP_FFC_IN_PORT_WIDTH(unit), 1 - 1,
                                              DNX_L3_VRRP_FFC_GEN_VAR_OFFSET(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_FFC_INSTRUCTION, 2, exem_ffc);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_KBR_VALID, 2, TRUE);

        /*
         * Feature that indicates the core of the port.
         * Se we set generic var value to be core id.
 */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_KEY_GEN_VAR, INST_SINGLE, core);

        /** commit */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /** set PER_VSI_MULTIPLE_MY_MAC_MSB_0 (multiple my mac based on VSI and MAC address) */
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_L3_VRRP_EXEM_DA_PROFILE_CONFIG, entry_handle_id));

        /** set key */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_DA_PROFILE,
                                   DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_PER_VSI_MULTIPLE_MY_MAC_DA_MSB_0);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, _SHR_CORE_ALL);

        /** set app db */
        SHR_IF_ERR_EXIT(dbal_tables_app_db_id_get(unit, DBAL_TABLE_EXEM_PER_VSI_MULTIPLE_MY_MAC_DA_MSB_0,
                                                  &app_db_id, &app_id_size));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXEM_VRRP_APP_DB_ID, INST_SINGLE, app_db_id);

        /** Set FFC 0 - MAC address - use only 47 LSB bits (instead of total 48) */
        exem_ffc = DNX_L3_VRRP_EXEM_FFC_BUILD(0, DNX_L3_VRRP_FFC_MAC_WIDTH - 2, DNX_L3_VRRP_FFC_MAC_OFFSET(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_FFC_INSTRUCTION, 0, exem_ffc);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_KBR_VALID, 0, TRUE);

        /** Set FFC 1 - VSI (use only 17 bits instead of total 18) */
        exem_ffc = DNX_L3_VRRP_EXEM_FFC_BUILD(DNX_L3_VRRP_FFC_MAC_WIDTH - 1, DNX_L3_VRRP_FFC_VSI_WIDTH - 2,
                                              DNX_L3_VRRP_FFC_VSI_OFFSET(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_FFC_INSTRUCTION, 1, exem_ffc);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_KBR_VALID, 1, TRUE);

        /** commit */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /** set PER_VSI_MULTIPLE_MY_MAC_MSB_1 (multiple my mac based on VSI and MAC address) */
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_L3_VRRP_EXEM_DA_PROFILE_CONFIG, entry_handle_id));

        /** set key */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_DA_PROFILE,
                                   DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_PER_VSI_MULTIPLE_MY_MAC_DA_MSB_1);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, _SHR_CORE_ALL);

        /** set app db */
        SHR_IF_ERR_EXIT(dbal_tables_app_db_id_get(unit, DBAL_TABLE_EXEM_PER_VSI_MULTIPLE_MY_MAC_DA_MSB_1,
                                                  &app_db_id, &app_id_size));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXEM_VRRP_APP_DB_ID, INST_SINGLE, app_db_id);

        /** Set FFC 0 - MAC address - use only 47 LSB bits (instead of total 48) */
        exem_ffc = DNX_L3_VRRP_EXEM_FFC_BUILD(0, DNX_L3_VRRP_FFC_MAC_WIDTH - 2, DNX_L3_VRRP_FFC_MAC_OFFSET(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_FFC_INSTRUCTION, 0, exem_ffc);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_KBR_VALID, 0, TRUE);

        /** Set FFC 1 - VSI (use only 17 bits instead of total 18) */
        exem_ffc = DNX_L3_VRRP_EXEM_FFC_BUILD(DNX_L3_VRRP_FFC_MAC_WIDTH - 1, DNX_L3_VRRP_FFC_VSI_WIDTH - 2,
                                              DNX_L3_VRRP_FFC_VSI_OFFSET(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_FFC_INSTRUCTION, 1, exem_ffc);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_KBR_VALID, 1, TRUE);

        /** commit */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /** set PER_PORT_MY_MAC per core (multiple my mac based on Source Port and protocol) */
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_L3_VRRP_EXEM_DA_PROFILE_CONFIG, entry_handle_id));

        /** set key */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_DA_PROFILE,
                                   DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_PER_PORT_IGNORE_MAC);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

        /** set app db */
        SHR_IF_ERR_EXIT(dbal_tables_app_db_id_get(unit, DBAL_TABLE_EXEM_PER_PORT_IGNORE_MAC, &app_db_id, &app_id_size));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXEM_VRRP_APP_DB_ID, INST_SINGLE, app_db_id);

        /** Set FFC 0 - zero padding */
        exem_ffc = DNX_L3_VRRP_EXEM_FFC_BUILD(0, 5 - 1, DNX_L3_VRRP_FFC_GEN_VAR_OFFSET(unit) + 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_FFC_INSTRUCTION, 0, exem_ffc);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_KBR_VALID, 0, TRUE);

        /** Set FFC 1 - next layer protocol */
        exem_ffc = DNX_L3_VRRP_EXEM_FFC_BUILD(5, DNX_L3_VRRP_FFC_LAYER_PROTOCOL_WIDTH - 1,
                                              DNX_L3_VRRP_FFC_LAYER_PROTOCOL_OFFSET(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_FFC_INSTRUCTION, 1, exem_ffc);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_KBR_VALID, 1, TRUE);

        /** Set FFC 2 - source port */
        exem_ffc =
            DNX_L3_VRRP_EXEM_FFC_BUILD(DNX_L3_VRRP_FFC_LAYER_PROTOCOL_WIDTH + 5,
                                       (dnx_data_port.general.pp_port_bits_size_get(unit)) - 1,
                                       DNX_L3_VRRP_FFC_IN_PORT_OFFSET(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_FFC_INSTRUCTION, 2, exem_ffc);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_KBR_VALID, 2, TRUE);

        if (dnx_data_device.general.nof_cores_get(unit))
        {
            /*
             * Feature that indicates the core of the port.
             * Se we set generic var value to be core id.
 */
            /** Set FFC 3 - source port core (1 bit)  */
            exem_ffc =
                DNX_L3_VRRP_EXEM_FFC_BUILD(DNX_L3_VRRP_FFC_LAYER_PROTOCOL_WIDTH +
                                           (dnx_data_port.general.pp_port_bits_size_get(unit)) + 5, 1 - 1,
                                           DNX_L3_VRRP_FFC_GEN_VAR_OFFSET(unit));
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_FFC_INSTRUCTION, 3, exem_ffc);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_KBR_VALID, 3, TRUE);

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_KEY_GEN_VAR, INST_SINGLE, core);
        }

        /** commit */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * brief - Initialize EXEM based multiple my mac HW defaults
 */
static shr_error_e
dnx_l3_vrrp_exem_based_multi_my_mac_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Set DA profile defaults - FFCs, APP DB, etc... */
    SHR_IF_ERR_EXIT(dnx_l3_vrrp_exem_based_multi_my_mac_da_profiles_init(unit));

    /** set default multiple my mac exem mode to VSI */
    SHR_IF_ERR_EXIT(bcm_dnx_switch_control_set(unit, bcmSwitchL2StationExtendedMode, 0 /** VSI */ ));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_l3_vrrp_init(
    int unit)
{
    int capacity;
    SHR_FUNC_INIT_VARS(unit);

    /** initialize default protocol groups for vs based vrrp protocol */
    SHR_IF_ERR_EXIT(dnx_l3_vrrp_default_protocol_groups_init(unit));

    SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_SEXEM_1, &capacity));

    if (capacity > 0)
    {
        /** Initialize EXEM based multiple my mac HW defaults */
        SHR_IF_ERR_EXIT(dnx_l3_vrrp_exem_based_multi_my_mac_init(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * brief - Given a vsi and vrid bitmap, set its vrid bitmap
 */
static shr_error_e
dnx_l3_vrrp_vsi_vrid_bitmap_verify(
    int unit,
    int vlan)
{
    dbal_table_field_info_t field_info;
    int rv;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** verify whether vsi type supports multiple my mac (vrid bitmap field exists) */
    {
        uint32 entry_handle_id;
        dbal_enum_value_result_type_ing_vsi_info_db_e vsi_result_type;

        /** Allocate handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ING_VSI_INFO_DB, &entry_handle_id));

        /** set key */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vlan);

        /** get result type */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                            &vsi_result_type));

        rv = dbal_tables_field_info_get(unit, DBAL_TABLE_ING_VSI_INFO_DB, DBAL_FIELD_VRID_BITMAP,
                                        FALSE, vsi_result_type, 0, &field_info);
        SHR_IF_ERR_EXIT_WITH_LOG(rv, "VSI (%d) result type doesn't support multiple my mac\n%s%s", vlan, EMPTY, EMPTY);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * brief - Given a vsi, return its vrid bitmap
 */
static shr_error_e
dnx_l3_vrrp_vsi_vrid_bitmap_get(
    int unit,
    int vlan,
    uint32 *vrid_bitmap)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_l3_vrrp_vsi_vrid_bitmap_verify(unit, vlan));

    /** get handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ING_VSI_INFO_DB, &entry_handle_id));

    /** set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vlan);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_VRID_BITMAP, INST_SINGLE,
                                                        vrid_bitmap));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * brief - Given a vsi and vrid bitmap, set its vrid bitmap
 */
static shr_error_e
dnx_l3_vrrp_vsi_vrid_bitmap_set(
    int unit,
    int vlan,
    uint32 vrid_bitmap)
{
    uint32 entry_handle_id;

    dbal_enum_value_result_type_ing_vsi_info_db_e vsi_result_type;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_l3_vrrp_vsi_vrid_bitmap_verify(unit, vlan));

    /** get result type */
    {
        /** Allocate handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ING_VSI_INFO_DB, &entry_handle_id));

        /** set key */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vlan);

        /** get result type */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                            &vsi_result_type));
    }

    /** set vrid bitmap */
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ING_VSI_INFO_DB, entry_handle_id));

        /** set key */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vlan);

        /** set result type*/
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, vsi_result_type);

        /** set vrid bitmap */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRID_BITMAP, INST_SINGLE, vrid_bitmap);

        /** commit */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * brief - Given a da profile and VSI (vlan), add/remove this da profile to the vrid members bitmap.
 */
static shr_error_e
dnx_l3_vrrp_vsi_vrid_member_set(
    int unit,
    int vlan,
    uint32 da_profile,
    uint8 is_add)
{
    uint32 vrid_bitmap[1];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_l3_vrrp_vsi_vrid_bitmap_get(unit, vlan, &vrid_bitmap[0]));

    /** add/remove member */
    if (is_add == TRUE)
    {
        SHR_BITSET(vrid_bitmap, da_profile);
    }
    else
    {
        SHR_BITCLR(vrid_bitmap, da_profile);
    }

    SHR_IF_ERR_EXIT(dnx_l3_vrrp_vsi_vrid_bitmap_set(unit, vlan, vrid_bitmap[0]));

exit:
    SHR_FUNC_EXIT;
}

/**
 * brief - Given a da profile and VSI (vlan), return whether the VSI already points to this da profile.
 */
static shr_error_e
dnx_l3_vrrp_vsi_vrid_member_is_exist_get(
    int unit,
    int vlan,
    uint32 da_profile,
    uint8 *is_exist)
{
    uint32 vrid_bitmap[1];

    SHR_FUNC_INIT_VARS(unit);

    *is_exist = FALSE;
    /*
     * Check if the entry in the vrrp mac table points to the tcam index
     */
    SHR_IF_ERR_EXIT(dnx_l3_vrrp_vsi_vrid_bitmap_get(unit, vlan, &vrid_bitmap[0]));

    if (SHR_BITGET(vrid_bitmap, da_profile))
    {
        *is_exist = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_l3_vrrp_tcam_info_set(
    int unit,
    int index,
    algo_dnx_l3_vrrp_tcam_key_t * tcam_info,
    int da_profile)
{
    uint32 entry_handle_id;
    bcm_mac_t mac_da_mask;
    int i;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** create handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L3_VRRP_TCAM, &entry_handle_id));

    /** set access ID (index) */
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, index));

    /** set key - mac da and protocol group, mask 1 means bit is ignored */
    for (i = 0; i < sizeof(bcm_mac_t); i++)
    {
        mac_da_mask[i] = ~(tcam_info->mac_da_mask[i]);
    }
    dbal_entry_key_field_arr8_masked_set(unit, entry_handle_id, DBAL_FIELD_DA, tcam_info->mac_da, mac_da_mask);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_VRRP_GROUP_ID, tcam_info->protocol_group,
                                      ~(tcam_info->protocol_group_mask));

    /** set result - da profile and valid bit */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_DA_PROFILE, INST_SINGLE, da_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);

    /** commit */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - remove entry (index) from VRRP TCAM
 */
shr_error_e
dnx_l3_vrrp_tcam_info_clear(
    int unit,
    int index)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** create handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L3_VRRP_TCAM, &entry_handle_id));

    /** set access ID (index) */
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, index));

    /** clear the entry */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_l3_vrrp_members_to_protocol_group_get(
    int unit,
    uint32 *members_flags,
    uint8 exact_match,
    uint32 *group_id,
    uint32 *group_mask)
{
    int i, nof_bits;
    uint32 internal_flags, and_groups, nand_groups, full_mask, existing_members = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (members_flags[0] == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Can't find empty protocol group.");
    }

    *group_id = DNX_L3_VRRP_PROTOCOL_GROUP_INVALID;
    *group_mask = DNX_L3_VRRP_PROTOCOL_GROUP_MASK_FULL;

    /*
     * Traverse over the groups. If a match is found, keep it. If more than one match is found, calculate all the
     * matches's mask.
     */
    for (i = 0; i < dnx_data_l3.vrrp.nof_protocol_groups_get(unit); i++)
    {

        SHR_IF_ERR_EXIT(dnx_l3_vrrp_protocol_group_members_sw_get(unit, i, DNX_L3_VRRP_PROTOCOL_GROUP_MASK_FULL,
                                                                  &internal_flags));

        /*
         * If there's a match between the required protocol and the group, accumulate it.
         *   We'll return error later if we accumulated too many groups.
         */
        if (internal_flags & members_flags[0])
        {
            if (*group_id == DNX_L3_VRRP_PROTOCOL_GROUP_INVALID)
            {
                /** When the first match if found, keep it. */
                *group_id = i;
            }
            else
            {
                /** Only keep in the mask the bits that are common to all the groups found. */
                and_groups = *group_id & i;
                nand_groups = ~(*group_id) & ~i;
                *group_mask &= (and_groups | nand_groups);
            }

            /** Accumulate all members of the group in case the user wants to be warned. */
            existing_members |= internal_flags;
        }
    }

    if (exact_match && (existing_members != members_flags[0]))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Required protocols' group(s) contains other members. "
                     "Remove extra members and try again.");
    }

    /** set full mask indication if required */
    SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                    (unit, DBAL_TABLE_L3_VRRP_PROTOCOL_GROUP_SW, DBAL_FIELD_VRRP_GROUP_ID, TRUE, 0, 0, &nof_bits));
    full_mask = UTILEX_BITS_MASK(nof_bits - 1, 0);

    if (*group_mask == full_mask)
    {
        *group_mask = DNX_L3_VRRP_PROTOCOL_GROUP_MASK_FULL;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_l3_vrrp_setting_add(
    int unit,
    algo_dnx_l3_vrrp_tcam_key_t * tcam_info,
    int vlan,
    uint32 *tcam_index)
{
    uint8 is_exist, is_first, is_last;
    int da_profile;

    SHR_FUNC_INIT_VARS(unit);

    /** Get the entry where the mac is, or where the mac is to be placed. */
    SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.vrrp_vsi_tcam_table.allocate_single
                    (unit, 0, tcam_info, NULL, (int *) tcam_index, &is_first));

    /** for vsi based multiple my mac, da profile is the same as the tcam_index */
    da_profile = *tcam_index;

    /** Check that the entry already exist */
    SHR_IF_ERR_EXIT(dnx_l3_vrrp_vsi_vrid_member_is_exist_get(unit, vlan, da_profile, &is_exist));
    if (is_exist == TRUE)
    {
        SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.vrrp_vsi_tcam_table.free_single(unit, *tcam_index, &is_last));

        SHR_ERR_EXIT(_SHR_E_EXISTS, "VSI (%d) is already configured for this mac address and protocols", vlan);
    }

    /** If this is a first TCAM entry, set it to vrrp tcam table */
    if (is_first)
    {
        SHR_IF_ERR_EXIT(dnx_l3_vrrp_tcam_info_set(unit, *tcam_index, tcam_info, da_profile));

        /** update group usage counter */
        SHR_IF_ERR_EXIT(dnx_l3_vrrp_protocol_group_count_sw_set(unit, tcam_info->protocol_group,
                                                                tcam_info->protocol_group_mask, 1));
    }

    /** Map the vrrp mac table to the da profile*/
    SHR_IF_ERR_EXIT(dnx_l3_vrrp_vsi_vrid_member_set(unit, vlan, da_profile, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_l3_vrrp_exem_based_tcam_info_clear(
    int unit,
    algo_dnx_l3_vrrp_tcam_key_t * tcam_info)
{
    uint8 is_last;
    int tcam_index;

    SHR_FUNC_INIT_VARS(unit);

    /** get tcam profile index  */
    SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.vrrp_exem_tcam_table.profile_get(unit, tcam_info, &tcam_index));
    /** deallocate the profile (tcam_index) */
    SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.vrrp_exem_tcam_table.free_single(unit, tcam_index, &is_last));

    /** If this is the last entry using the mac, remove it from tcam table and mark group as unused */
    if (is_last == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_l3_vrrp_tcam_info_clear(unit, tcam_index));

        /** update group usage counter */
        SHR_IF_ERR_EXIT(dnx_l3_vrrp_protocol_group_count_sw_set(unit, tcam_info->protocol_group,
                                                                tcam_info->protocol_group_mask, -1));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_l3_vrrp_setting_delete(
    int unit,
    algo_dnx_l3_vrrp_tcam_key_t * tcam_info,
    int vlan)
{
    int tcam_index, da_profile;
    uint8 is_exist, is_last;

    SHR_FUNC_INIT_VARS(unit);

    /** Get the entry where the mac is  */
    SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.vrrp_vsi_tcam_table.profile_get(unit, tcam_info, &tcam_index));

    /** for vsi based multiple mac, da profile is the same the tcam index */
    da_profile = tcam_index;

    /** Check that the entry doesn't already exist */
    SHR_IF_ERR_EXIT(dnx_l3_vrrp_vsi_vrid_member_is_exist_get(unit, vlan, da_profile, &is_exist));
    if (is_exist == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "VSI (%d) is not configured for the given mac adress", vlan);
    }

    /** deallocate the profile (tcam_index) */
    SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.vrrp_vsi_tcam_table.free_single(unit, tcam_index, &is_last));

    /** If this is the last entry using the mac, remove it from tcam table and mark group as unused */
    if (is_last == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_l3_vrrp_tcam_info_clear(unit, tcam_index));

        /** update group usage counter */
        SHR_IF_ERR_EXIT(dnx_l3_vrrp_protocol_group_count_sw_set(unit, tcam_info->protocol_group,
                                                                tcam_info->protocol_group_mask, -1));
    }

    /** remove da_profile from vsi vrid members */
    SHR_IF_ERR_EXIT(dnx_l3_vrrp_vsi_vrid_member_set(unit, vlan, da_profile, FALSE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Fill algo_dnx_l3_vrrp_tcam_key_t structure with provided data according to VRRP protocol
 */
static shr_error_e
dnx_l3_vrrp_vsi_based_vrid_tcam_info_fill(
    int unit,
    uint32 flags,
    uint32 vrid,
    algo_dnx_l3_vrrp_tcam_key_t * tcam_data)
{
    uint32 protocol_flags[1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(tcam_data, 0, sizeof(algo_dnx_l3_vrrp_tcam_key_t));

    /** construct tcam data */
    {
        if (_SHR_IS_FLAG_SET(flags, BCM_L3_VRRP_IPV4))
        {
            /** IPv4 only or both IPv4 and Ipv6 */
            sal_memcpy(tcam_data->mac_da, dnx_l3_vrrp_vrid_mac_ipv4, sizeof(bcm_mac_t));
        }
        else
        {
            /** IPv6 only */
            sal_memcpy(tcam_data->mac_da, dnx_l3_vrrp_vrid_mac_ipv6, sizeof(bcm_mac_t));
        }

        /** set mac DA LSB to VRID value */
        tcam_data->mac_da[5] = (uint8) vrid;

        /** set mac mask to full mask */
        sal_memcpy(tcam_data->mac_da_mask, dnx_l3_vrrp_vrid_mac_mask, sizeof(bcm_mac_t));

        /** set group and group mask */
        SHR_IF_ERR_EXIT(dnx_l3_vrrp_protocol_group_api_flags_to_internal_translate(unit, flags, protocol_flags));
        SHR_IF_ERR_EXIT(dnx_l3_vrrp_members_to_protocol_group_get
                        (unit, protocol_flags, TRUE, &(tcam_data->protocol_group), &(tcam_data->protocol_group_mask)));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Given tcam_data and internal (dbal) protocol flags, check if it's a VRRP protocol entry.
 *
 * \param [in] unit - Relevant unit
 * \param [in] flags - l3 vrrp protocols.
 * \param [in] tcam_data - entry tcam data.
 * \param [out] is_vrrp - TRUE if the entry of type VRRP protocol, otherwise false).
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_l3_vrrp_vsi_is_vrrp_protocol_get(
    int unit,
    uint32 flags,
    algo_dnx_l3_vrrp_tcam_key_t * tcam_data,
    uint8 *is_vrrp)
{
    algo_dnx_l3_vrrp_tcam_key_t vrrp_tcam_data;

    SHR_FUNC_INIT_VARS(unit);

    /** create a vrrp tcam data and compare it to the provided tcam data */
    SHR_IF_ERR_EXIT(dnx_l3_vrrp_vsi_based_vrid_tcam_info_fill(unit, flags, tcam_data->mac_da[5], &vrrp_tcam_data));

    /** in case both data is equal it means that the provided tcam data is of VRRP protocol */
    *is_vrrp = !sal_memcmp((void *) tcam_data, (void *) &vrrp_tcam_data, sizeof(algo_dnx_l3_vrrp_tcam_key_t));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - add EXEM VRID MY-MAC entry
 */
static shr_error_e
dnx_l3_vrrp_exem_vrid_my_mac_add(
    int unit,
    bcm_vlan_t vlan,
    uint32 vrid,
    dbal_enum_value_field_layer_types_e protocol)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** create handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EXEM_VRID_MY_MAC_WITH_PROTOCOL, &entry_handle_id));

    /** set key - protocol */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE_CURRENT_LAYER_PLUS_1, protocol);

    /** set key - vlan */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vlan);
    /** set key - vrid */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VRID, vrid);

    /** set value - my mac indication*/
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MY_MAC_TYPE, INST_SINGLE, 1);

    /** commit */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_l3_vrrp_exem_source_port_my_mac_add(
    int unit,
    bcm_port_t port,
    bcm_mac_t mac_da,
    int *index)
{
    uint32 entry_handle_id, pp_port;
    bcm_core_t core;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    int pp_port_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get Port + Core  */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        uint32 mapped_pp_port;
        pp_port = gport_info.internal_port_pp_info.pp_port[pp_port_index];
        core = gport_info.internal_port_pp_info.core_id[pp_port_index];

        mapped_pp_port = pp_port | (core << dnx_data_port.general.pp_port_bits_size_get(unit));

        /** Set HW table (order is important - if entry exists it will fail) */
        {
            /** create handle */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EXEM_PER_PORT_MY_MAC, &entry_handle_id));

            /** set key - mapped_pp_port */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAPPED_PP_PORT, mapped_pp_port);

            /** set key - l2 mac */
            dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_L2_MAC, mac_da);

            /** set value - my mac indication*/
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MY_MAC_TYPE, INST_SINGLE, 1);

            /** commit */
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }

        /** Allocate SW index and set SW table */
        {
            /** allocate sw index */
            SHR_IF_ERR_EXIT(algo_l3_db.vrrp_exem_l2_station_id_alloc.res_bitmap.allocate_single(unit, 0, NULL, index));

            /** create handle */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L3_VRRP_EXEM_DB_SW, &entry_handle_id));

            /** set key - station_id */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BCM_STATION_ID, *index);

            /** set value - result type */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         DBAL_RESULT_TYPE_L3_VRRP_EXEM_DB_SW_PORT);
            /** set value - pp port */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, INST_SINGLE, pp_port);
            /** set value - core */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE, INST_SINGLE, core);
            /** set value - mac address */
            dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_L2_MAC, INST_SINGLE, mac_da);

            /** commit */
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_l3_vrrp_exem_source_port_protocol_add(
    int unit,
    bcm_port_t port,
    uint32 protocol,
    int *index)
{
    uint32 entry_handle_id, pp_port, layer_protocol;
    bcm_core_t core;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    int pp_port_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    layer_protocol = (protocol & BCM_L2_STATION_MPLS) ? DBAL_ENUM_FVAL_LAYER_TYPES_MPLS :
        ((protocol & BCM_L2_STATION_IPV4) ? DBAL_ENUM_FVAL_LAYER_TYPES_IPV4 : DBAL_ENUM_FVAL_LAYER_TYPES_IPV6);
    /** Get Port + Core  */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        uint32 mapped_pp_port;
        pp_port = gport_info.internal_port_pp_info.pp_port[pp_port_index];
        core = gport_info.internal_port_pp_info.core_id[pp_port_index];

        mapped_pp_port = pp_port | (core << dnx_data_port.general.pp_port_bits_size_get(unit));

        /** Set HW table (order is important - if entry exists it will fail) */
        {
            /** create handle */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EXEM_PER_PORT_IGNORE_MAC, &entry_handle_id));

            /** set key - mapped_pp_port */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAPPED_PP_PORT, mapped_pp_port);

            /** set key - protocol */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE_CURRENT_LAYER_PLUS_1,
                                       layer_protocol);

            /** set value - my mac indication*/
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MY_MAC_TYPE, INST_SINGLE, 1);

            /** commit */
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }

        /** Allocate SW index and set SW table */
        {
            /** allocate sw index */
            SHR_IF_ERR_EXIT(algo_l3_db.vrrp_exem_l2_station_id_alloc.res_bitmap.allocate_single(unit, 0, NULL, index));

            /** create handle */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L3_VRRP_EXEM_DB_SW, &entry_handle_id));

            /** set key - station_id */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BCM_STATION_ID, *index);

            /** set value - result type */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         DBAL_RESULT_TYPE_L3_VRRP_EXEM_DB_SW_PORT_PROTOCOL);
            /** set value - pp port */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, INST_SINGLE, pp_port);
            /** set value - core */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE, INST_SINGLE, core);

            /** set key - protocol */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPES, INST_SINGLE, layer_protocol);

            /** commit */
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_l3_vrrp_port_protocol_tcam_info_set(
    int unit,
    uint32 protocol)
{
    algo_dnx_l3_vrrp_tcam_key_t tcam_data;
    uint32 tcam_index, tcam_index_msb, da_profile;
    bcm_l3_protocol_group_id_t group_id = DNX_L3_VRRP_PROTOCOL_GROUP_INVALID;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_l3_vrrp_protocol_group_get(unit, &protocol, &group_id));
    /**
     * Default protocol contain All the other types, should NOT be used here.
     */
    if (group_id == DNX_L3_VRRP_L3_PROTOCOL_GROUP_ID_DEFAULT)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Can't Use group %d for port+protocol termination\n", group_id);
    }

    /** clear TCAM data since its data is irrelevant */
    sal_memset(&tcam_data, 0, sizeof(algo_dnx_l3_vrrp_tcam_key_t));

    /** get dedicated tcam index for exem vsi/source-port */
    tcam_index = dnx_data_l3.vrrp.exem_default_tcam_index_get(unit);
    tcam_index_msb = dnx_data_l3.vrrp.exem_default_tcam_index_msb_get(unit);

    da_profile = DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_PER_PORT_IGNORE_MAC;

    tcam_data.protocol_group = group_id;
    tcam_data.protocol_group_mask = 3;

    /** set TCAM */
    SHR_IF_ERR_EXIT(dnx_l3_vrrp_tcam_info_set(unit, tcam_index, &tcam_data, da_profile));

    /** clear second TCAM index (it's only used for VSI) */
    SHR_IF_ERR_EXIT(dnx_l3_vrrp_tcam_info_clear(unit, tcam_index_msb));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_l3_vrrp_exem_vsi_my_mac_add(
    int unit,
    bcm_vlan_t vlan,
    bcm_mac_t mac_da,
    int *index)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set HW table (order is important - if entry exists it will fail) */
    {
        /** create handle for msb 1 mac*/
        if (mac_da[0] & 0x80)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EXEM_PER_VSI_MULTIPLE_MY_MAC_DA_MSB_1,
                                              &entry_handle_id));
        }
        else
        {
            /** create handle for msb 0 mac*/
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EXEM_PER_VSI_MULTIPLE_MY_MAC_DA_MSB_0,
                                              &entry_handle_id));
        }

        /** set key - VSI */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vlan);

        /** set key - l2 mac */
        dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_L2_MAC, mac_da);

        /** set value - my mac indication*/
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MY_MAC_TYPE, INST_SINGLE, 1);

        /** commit */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /** Allocate SW index and set SW table */
    {

        /** allocate sw index */
        SHR_IF_ERR_EXIT(algo_l3_db.vrrp_exem_l2_station_id_alloc.res_bitmap.allocate_single(unit, 0, NULL, index));

        /** create handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L3_VRRP_EXEM_DB_SW, &entry_handle_id));

        /** set key - station_id */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BCM_STATION_ID, *index);

        /** set value - result type */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_L3_VRRP_EXEM_DB_SW_VSI);
        /** set value - vsi */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, INST_SINGLE, vlan);
        /** set value - mac address */
        dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_L2_MAC, INST_SINGLE, mac_da);

        /** commit */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_l3_vrrp_exem_multi_my_mac_delete(
    int unit,
    int index)
{
    uint32 entry_handle_id, pp_port;
    dbal_enum_value_result_type_l3_vrrp_exem_db_sw_e result_type;
    int core;
    bcm_vlan_t vlan;
    bcm_mac_t mac_da;
    dbal_enum_value_field_layer_types_e protocol;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** clear HW EXEM entry */
    {
        uint32 mapped_pp_port;
        /** get entry data from sw db */
        SHR_IF_ERR_EXIT(dnx_l3_vrrp_exem_multi_my_mac_get
                        (unit, index, &result_type, &vlan, &core, &pp_port, mac_da, &protocol));
        mapped_pp_port = pp_port | (core << dnx_data_port.general.pp_port_bits_size_get(unit));

        if (result_type == DBAL_RESULT_TYPE_L3_VRRP_EXEM_DB_SW_VSI)
        {
            /** create handle */
            if (mac_da[0] & 0x80)
            {
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                                (unit, DBAL_TABLE_EXEM_PER_VSI_MULTIPLE_MY_MAC_DA_MSB_1, &entry_handle_id));
            }
            else
            {
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                                (unit, DBAL_TABLE_EXEM_PER_VSI_MULTIPLE_MY_MAC_DA_MSB_0, &entry_handle_id));
            }

            /** set key - VSI */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vlan);
        }
        else if (result_type == DBAL_RESULT_TYPE_L3_VRRP_EXEM_DB_SW_PORT)
        {
            /** create handle */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EXEM_PER_PORT_MY_MAC, &entry_handle_id));

            /** set key - pp port */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAPPED_PP_PORT, mapped_pp_port);
        }
        else if (result_type == DBAL_RESULT_TYPE_L3_VRRP_EXEM_DB_SW_PORT_PROTOCOL)
        {
            /** create handle */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EXEM_PER_PORT_IGNORE_MAC, &entry_handle_id));

            /** set key - pp port */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAPPED_PP_PORT, mapped_pp_port);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE_CURRENT_LAYER_PLUS_1, protocol);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported EXEM VRRP entry type = %d.", result_type);
        }

        if (result_type != DBAL_RESULT_TYPE_L3_VRRP_EXEM_DB_SW_PORT_PROTOCOL)
        {
            /** set key - l2 mac */
            dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_L2_MAC, mac_da);
        }

        /** clear */
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

    /** clear SW EXEM entry */
    {
        /** deallocate SW resource */
        SHR_IF_ERR_EXIT(algo_l3_db.vrrp_exem_l2_station_id_alloc.res_bitmap.free_single(unit, index));

        /** create handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L3_VRRP_EXEM_DB_SW, &entry_handle_id));

        /** set key - station_id */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BCM_STATION_ID, index);

        /** clear the entry */
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_l3_vrrp_exem_multi_my_mac_get(
    int unit,
    int index,
    dbal_enum_value_result_type_l3_vrrp_exem_db_sw_e * result_type,
    bcm_vlan_t * vlan,
    int *core,
    uint32 *pp_port,
    bcm_mac_t mac_da,
    dbal_enum_value_field_layer_types_e * protocol)
{
    uint32 entry_handle_id, vsi;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** init values */
    *core = *pp_port = *vlan = 0;
    sal_memset(mac_da, 0, sizeof(bcm_mac_t));

    /** create handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L3_VRRP_EXEM_DB_SW, &entry_handle_id));

    /** set key - station_id */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BCM_STATION_ID, index);

    /** read the entry */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** retrieve information according to entry result type */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                        result_type));

    if (*result_type == DBAL_RESULT_TYPE_L3_VRRP_EXEM_DB_SW_VSI)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_VSI, INST_SINGLE, &vsi));
        *vlan = vsi;
    }
    else if (*result_type == DBAL_RESULT_TYPE_L3_VRRP_EXEM_DB_SW_PORT)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_PP_PORT, INST_SINGLE,
                                                            pp_port));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_CORE, INST_SINGLE,
                                                            (uint32 *) core));
    }
    else if (*result_type == DBAL_RESULT_TYPE_L3_VRRP_EXEM_DB_SW_PORT_PROTOCOL)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_PP_PORT, INST_SINGLE,
                                                            pp_port));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_CORE, INST_SINGLE,
                                                            (uint32 *) core));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPES, INST_SINGLE,
                                                            (uint32 *) protocol));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported EXEM VRRP entry type = %d.", *result_type);
    }

    if (*result_type != DBAL_RESULT_TYPE_L3_VRRP_EXEM_DB_SW_PORT_PROTOCOL)
    {
        /** get mac address */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get(unit, entry_handle_id, DBAL_FIELD_L2_MAC, INST_SINGLE,
                                                               mac_da));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - add EXEM VRID MY-MAC entry
 */
static shr_error_e
dnx_l3_vrrp_exem_vrid_my_mac_remove(
    int unit,
    bcm_vlan_t vlan,
    uint32 vrid,
    dbal_enum_value_field_layer_types_e protocol)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** create handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EXEM_VRID_MY_MAC_WITH_PROTOCOL, &entry_handle_id));

    /** set key - vlan */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vlan);
    /** set key - vrid */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VRID, vrid);
    /** set key - protocol */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE_CURRENT_LAYER_PLUS_1, protocol);
    /** commit */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add handle for an EXEM based VRRP
 */
static shr_error_e
dnx_l3_vrrp_exem_based_vrid_add(
    int unit,
    uint32 flags,
    bcm_vlan_t vlan,
    uint32 vrid)
{
    algo_dnx_l3_vrrp_tcam_key_t tcam_data;
    uint8 is_first;
    int tcam_index;
    dbal_enum_value_field_layer_types_e protocol;
    SHR_FUNC_INIT_VARS(unit);

    /** construct tcam key data */
    SHR_IF_ERR_EXIT(dnx_l3_vrrp_vsi_based_vrid_tcam_info_fill(unit, flags, 0, &tcam_data));

    /** set LSB byte of mac address to 0 since vrid is set in EXEM table */
    tcam_data.mac_da_mask[5] = 0x0;

    /** According to the flag the protocol type and the TCAM index will be decided, (one of them must be set but not both) */
    if (_SHR_IS_FLAG_SET(flags, BCM_L3_VRRP_IPV4) == TRUE)
    {
        tcam_index = dnx_data_l3.vrrp.exem_vrid_ipv4_tcam_index_get(unit);
        protocol = DBAL_ENUM_FVAL_LAYER_TYPES_IPV4;
    }
    else
    {
        tcam_index = dnx_data_l3.vrrp.exem_vrid_ipv6_tcam_index_get(unit);
        protocol = DBAL_ENUM_FVAL_LAYER_TYPES_IPV6;
    }

    SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.vrrp_exem_tcam_table.allocate_single
                    (unit, DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID, &tcam_data, NULL, &tcam_index, &is_first));

    /** set tcam */
    if (is_first)
    {
        SHR_IF_ERR_EXIT(dnx_l3_vrrp_tcam_info_set(unit, tcam_index, &tcam_data,
                                                  DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_VRID_MY_MAC_WITH_PROTOCOL));

        /** update group counter */
        SHR_IF_ERR_EXIT(dnx_l3_vrrp_protocol_group_count_sw_set(unit, tcam_data.protocol_group,
                                                                tcam_data.protocol_group_mask, 1));
    }

    /** add entry to exem table */
    SHR_IF_ERR_EXIT(dnx_l3_vrrp_exem_vrid_my_mac_add(unit, vlan, vrid, protocol));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete handle for EXEM based VRRP
 */
static shr_error_e
dnx_l3_vrrp_exem_based_vrid_delete(
    int unit,
    uint32 flags,
    bcm_vlan_t vlan,
    uint32 vrid)
{
    algo_dnx_l3_vrrp_tcam_key_t tcam_data;

    SHR_FUNC_INIT_VARS(unit);

    /** construct tcam key data */
    SHR_IF_ERR_EXIT(dnx_l3_vrrp_vsi_based_vrid_tcam_info_fill(unit, flags, 0, &tcam_data));

    /** set LSB byte of mac address to 0 since vrid is set in EXEM table */
    tcam_data.mac_da_mask[5] = 0x0;

    /** deallocate and clear TCAM entry */
    SHR_IF_ERR_EXIT(dnx_l3_vrrp_exem_based_tcam_info_clear(unit, &tcam_data));

    /** Delete entry from EXEM table */
    SHR_IF_ERR_EXIT(dnx_l3_vrrp_exem_vrid_my_mac_remove
                    (unit, vlan, vrid, DNX_L3_VRRP_IP_PROTOCOL_FROM_FLAG_GET(flags)));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify procedure for bcm_dnx_l3_vrrp_config_add API
 */
static shr_error_e
dnx_l3_vrrp_config_add_delete_verify(
    int unit,
    uint32 flags,
    bcm_vlan_t vlan,
    uint32 vrid)
{
    uint32 supported_flags;
    uint8 intf_exists;
    uint8 is_vsi_allocated;
    int nof_vsis;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_vsi_count_get(unit, &nof_vsis));
    /** Verify supported flags */
    supported_flags = BCM_L3_VRRP_IPV6 | BCM_L3_VRRP_IPV4 | BCM_L3_VRRP_EXTENDED;
    SHR_MASK_VERIFY(flags, supported_flags, _SHR_E_PARAM, "non supported flags specified.\n");

    /** Check mandatory flags */
    if ((_SHR_IS_FLAG_SET(flags, BCM_L3_VRRP_IPV4) == FALSE) && (_SHR_IS_FLAG_SET(flags, BCM_L3_VRRP_IPV6) == FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "At least one of the flags: IPV4, IPV6 must be set.");
    }

    /** Check that the IPv4 and the IPv6 flags are not set together */
    if ((_SHR_IS_FLAG_SET(flags, BCM_L3_VRRP_IPV4) == TRUE) && (_SHR_IS_FLAG_SET(flags, BCM_L3_VRRP_IPV6) == TRUE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The IPV4 and IPV6 are mutually exclusive.");
    }

    /** Check vrid */
    if (vrid > DNX_L3_VRRP_VRID_MAX)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "VRID must be a value between 0 and %d.", DNX_L3_VRRP_VRID_MAX);
    }

    /** verify valid VSI ID */
    if (vlan >= nof_vsis)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong setting. vsi = %d is not a valid vsi.", vlan);
    }

    /** verify VSI is allocated by L3 Intf */
    is_vsi_allocated = 0;
    SHR_IF_ERR_EXIT(vlan_db.vsi.is_allocated(unit, vlan, &is_vsi_allocated));
    intf_exists = 0;
    SHR_IF_ERR_EXIT(dnx_l3_check_if_ingress_intf_exists(unit, vlan, &intf_exists));
    if (!intf_exists && !is_vsi_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                     "VRRP APIs per VSI is called but L3 Intf or VPN-VSI is not created - vsi_rif_id = %d.", vlan);
    }

    /** Setting all VSIs is not supported */
    if (DNX_L3_VRRP_VLAN_IS_ALL_VSI(vlan))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong setting. Setting all VSIs isn't supported.\n");
    }

    /** Verify extended mode is supported if asked for */
    if (_SHR_IS_FLAG_SET(flags, BCM_L3_VRRP_EXTENDED))
    {
        int capacity;

        SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_SEXEM_1, &capacity));

        if (capacity == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "VRRP extended memory is not supported.");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - add l3 vrrp (my mac) entry
 *
 * \param [in] unit - Unit ID
 * \param [in] flags:
 *  * BCM_L3_VRRP_IPV4 - IPv4 VRRP entry
 *  * BCM_L3_VRRP_IPV6 - IPv6 VRRP entry
 *  * BCM_L3_VRRP_EXTENDED - Indicate usage of extended memory instead of VSI table.
 * \param [in] vlan - VSI ID
 * \param [in] vrid - VRRP VRID
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
int
bcm_dnx_l3_vrrp_config_add(
    int unit,
    uint32 flags,
    bcm_vlan_t vlan,
    uint32 vrid)
{
    algo_dnx_l3_vrrp_tcam_key_t tcam_data;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_l3_vrrp_config_add_delete_verify(unit, flags, vlan, vrid));

    if (_SHR_IS_FLAG_SET(flags, BCM_L3_VRRP_EXTENDED))
    {
        /** EXEM based VRRP */
        SHR_IF_ERR_EXIT(dnx_l3_vrrp_exem_based_vrid_add(unit, flags, vlan, vrid));
    }
    else
    {
        /** VSI based VRRP */
        uint32 tcam_index;
        SHR_IF_ERR_EXIT(dnx_l3_vrrp_vsi_based_vrid_tcam_info_fill(unit, flags, vrid, &tcam_data));
        /** allocate and set the entry */
        SHR_IF_ERR_EXIT(dnx_l3_vrrp_setting_add(unit, &tcam_data, vlan, &tcam_index));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete l3 vrrp (my mac) entry
 *
 * \param [in] unit - Unit ID
 * \param [in] flags:
 *  * BCM_L3_VRRP_IPV4 - IPv4 VRRP entry
 *  * BCM_L3_VRRP_IPV6 - IPv6 VRRP entry
 *  * BCM_L3_VRRP_EXTENDED - Indicate usage of extended memory instead of VSI table.
 * \param [in] vlan - VSI ID
 * \param [in] vrid - VRRP VRID
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
int
bcm_dnx_l3_vrrp_config_delete(
    int unit,
    uint32 flags,
    bcm_vlan_t vlan,
    uint32 vrid)
{
    algo_dnx_l3_vrrp_tcam_key_t tcam_data;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_l3_vrrp_config_add_delete_verify(unit, flags, vlan, vrid));

    if (_SHR_IS_FLAG_SET(flags, BCM_L3_VRRP_EXTENDED))
    {
        /** EXEM based VRRP */
        SHR_IF_ERR_EXIT(dnx_l3_vrrp_exem_based_vrid_delete(unit, flags, vlan, vrid));
    }
    else
    {
        /** VSI based VRRP */
        /** construct tcam key data */
        SHR_IF_ERR_EXIT(dnx_l3_vrrp_vsi_based_vrid_tcam_info_fill(unit, flags, vrid, &tcam_data));
        /** Deallocate and clear the entry */
        SHR_IF_ERR_EXIT(dnx_l3_vrrp_setting_delete(unit, &tcam_data, vlan));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete all handle for VSI based VRRP
 */
static int
dnx_l3_vrrp_vsi_based_vrid_delete_all(
    int unit,
    uint32 flags,
    bcm_vlan_t vlan)
{
    int ref_count;
    algo_dnx_l3_vrrp_tcam_key_t tcam_data;
    int tcam_index = 0;
    uint32 vrid_bitmap[1];
    uint8 is_vrrp = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    /**  get the tcam entries that the provided VSI points to */
    SHR_IF_ERR_EXIT(dnx_l3_vrrp_vsi_vrid_bitmap_get(unit, vlan, &vrid_bitmap[0]));

    /** iterate over the vrid members and remove them */
    SHR_BIT_ITER(vrid_bitmap, dnx_data_l3.vrrp.nof_vsi_tcam_entries_get(unit), tcam_index)
    {

        /** read the tcam entry */
        SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.vrrp_vsi_tcam_table.profile_data_get
                        (unit, tcam_index, &ref_count, (void *) &tcam_data));

        SHR_IF_ERR_EXIT(dnx_l3_vrrp_vsi_is_vrrp_protocol_get(unit, flags, &tcam_data, &is_vrrp));
        /** delete the entry only if it's of type VRRP protocol*/
        if (is_vrrp == TRUE)
        {
            /** Deallocate and clear the entry */
            SHR_IF_ERR_EXIT(dnx_l3_vrrp_setting_delete(unit, &tcam_data, vlan));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete all handle for EXEM based VRRP
 */
static int
dnx_l3_vrrp_exem_based_vrid_delete_all(
    int unit,
    uint32 flags,
    bcm_vlan_t vlan)
{
    algo_dnx_l3_vrrp_tcam_key_t tcam_data;
    int tcam_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** construct tcam key data */
    SHR_IF_ERR_EXIT(dnx_l3_vrrp_vsi_based_vrid_tcam_info_fill(unit, flags, 0, &tcam_data));

    /** set LSB byte of mac mask address to 0 since vrid is set in EXEM table */
    tcam_data.mac_da_mask[5] = 0x0;

    /** get tcam profile index (verify that such a profile exists) */
    SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.vrrp_exem_tcam_table.profile_get(unit, &tcam_data, &tcam_index));

    /** iterate over all EXEM_VRID_MY_MAC entries with a given vlan, and delete them */
    {
        uint32 entry_handle_id, key_rule_value, vrid, protocol_rule;
        int is_end;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EXEM_VRID_MY_MAC_WITH_PROTOCOL, &entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

        /** Add KEY rule */
        key_rule_value = vlan;
        SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add(unit, entry_handle_id, DBAL_FIELD_VSI,
                                                               DBAL_CONDITION_EQUAL_TO, &key_rule_value, NULL));

        protocol_rule = DNX_L3_VRRP_IP_PROTOCOL_FROM_FLAG_GET(flags);
        SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add
                        (unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE_CURRENT_LAYER_PLUS_1, DBAL_CONDITION_EQUAL_TO,
                         &protocol_rule, NULL));

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        while (!is_end)
        {
            /** get vrid */
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_VRID, &vrid));

            /** delete entry using API */
            SHR_IF_ERR_EXIT(bcm_dnx_l3_vrrp_config_delete(unit, flags, vlan, vrid));

            /** get next entry */
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l3_vrrp_config_delete_all(
    int unit,
    uint32 flags,
    bcm_vlan_t vlan)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_l3_vrrp_config_add_delete_verify(unit, flags, vlan, 0));

    if (_SHR_IS_FLAG_SET(flags, BCM_L3_VRRP_EXTENDED))
    {
        /** EXEM based VRRP */
        SHR_IF_ERR_EXIT(dnx_l3_vrrp_exem_based_vrid_delete_all(unit, flags, vlan));
    }
    else
    {
        /** VSI based VRRP */
        SHR_IF_ERR_EXIT(dnx_l3_vrrp_vsi_based_vrid_delete_all(unit, flags, vlan));
    }

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify procedure for bcm_dnx_l3_vrrp_config_add API
 */
static shr_error_e
dnx_l3_vrrp_config_get_verify(
    int unit,
    uint32 flags,
    bcm_vlan_t vlan,
    uint32 alloc_size,
    uint32 *vrid_array,
    uint32 *count)
{
    SHR_FUNC_INIT_VARS(unit);

    /** NULL check */
    SHR_NULL_CHECK(vrid_array, _SHR_E_PARAM, "vrid_array");
    SHR_NULL_CHECK(count, _SHR_E_PARAM, "count");

    /** verify given array size is valid */
    if (alloc_size < 1
        || (!_SHR_IS_FLAG_SET(flags, BCM_L3_VRRP_EXTENDED)
            && alloc_size > dnx_data_l3.vrrp.nof_vsi_tcam_entries_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The array size (%d) must be a positive number and is limited to %d or less in case the extended option is omitted.",
                     alloc_size, dnx_data_l3.vrrp.nof_vsi_tcam_entries_get(unit));
    }

    SHR_INVOKE_VERIFY_DNX(dnx_l3_vrrp_config_add_delete_verify(unit, flags, vlan, 0));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get handle for EXEM based VRRP
 */
static shr_error_e
dnx_l3_vrrp_exem_based_vrid_get(
    int unit,
    uint32 flags,
    bcm_vlan_t vlan,
    uint32 alloc_size,
    uint32 *vrid_array,
    uint32 *count)
{
    algo_dnx_l3_vrrp_tcam_key_t tcam_data;
    int tcam_index, rv;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    *count = 0;

    /** construct tcam key data */
    SHR_IF_ERR_EXIT(dnx_l3_vrrp_vsi_based_vrid_tcam_info_fill(unit, flags, 0, &tcam_data));

    /** set LSB byte of mac mask address to 0 since vrid is set in EXEM table */
    tcam_data.mac_da_mask[5] = 0x0;

    /** get tcam profile index (verify that such a profile exists) */
    rv = algo_l3_db.algo_l3_templates.vrrp_exem_tcam_table.profile_get(unit, &tcam_data, &tcam_index);
    /** if no such profile exist - exit with count 0 */
    if (rv == _SHR_E_NOT_FOUND)
    {
        SHR_EXIT();
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }

    /** iterate over all EXEM_VRID_MY_MAC entries with a given vlan, and retrieve them */
    {
        uint32 entry_handle_id, vrid, protocol_rule;
        uint32 key_rule_value[1];
        uint32 key_rule_mask[1];
        int is_end;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EXEM_VRID_MY_MAC_WITH_PROTOCOL, &entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

        /** Add KEY rule */
        key_rule_value[0] = vlan;
        key_rule_mask[0] = DNX_L3_VRRP_L3_VLAN_FIELD_FULL_MASK;

        SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add(unit, entry_handle_id, DBAL_FIELD_VSI,
                                                               DBAL_CONDITION_EQUAL_TO, key_rule_value, key_rule_mask));

        protocol_rule = DNX_L3_VRRP_IP_PROTOCOL_FROM_FLAG_GET(flags);
        SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add
                        (unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE_CURRENT_LAYER_PLUS_1, DBAL_CONDITION_EQUAL_TO,
                         &protocol_rule, NULL));

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        while (!is_end)
        {
            /** get vrid */
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_VRID, &vrid));

            /** add vrid to the returned array */
            vrid_array[(*count)++] = vrid;

            /**  break if all of the array has been filled */
            if (*count == alloc_size)
            {
                break;
            }

            /** read next entry */
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get handle for VSI based VRRP
 */
static shr_error_e
dnx_l3_vrrp_vsi_based_vrid_get(
    int unit,
    uint32 flags,
    bcm_vlan_t vlan,
    uint32 alloc_size,
    uint32 *vrid_array,
    uint32 *count)
{
    int ref_count;
    algo_dnx_l3_vrrp_tcam_key_t tcam_data;
    int tcam_index = 0;
    uint32 vrid_bitmap[1], members, api_flags;
    uint8 is_vrrp;

    SHR_FUNC_INIT_VARS(unit);

    *count = 0;

    /**  get the tcam entries that the provided VSI points to */
    SHR_IF_ERR_EXIT(dnx_l3_vrrp_vsi_vrid_bitmap_get(unit, vlan, &vrid_bitmap[0]));

    /** iterate over the vrid members and retrieve them */
    SHR_BIT_ITER(vrid_bitmap, dnx_data_l3.vrrp.nof_vsi_tcam_entries_get(unit), tcam_index)
    {
        /** read the tcam entry */
        SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.vrrp_vsi_tcam_table.profile_data_get
                        (unit, tcam_index, &ref_count, (void *) &tcam_data));

        /** filter non vrrp entries (multiple my mac entries */
        SHR_IF_ERR_EXIT(dnx_l2_station_mac_is_vrrp_mac(unit, tcam_data.mac_da, &is_vrrp));
        if (is_vrrp == FALSE)
        {
            continue;
        }

        /** filter according to the required l3 protocols */
        SHR_IF_ERR_EXIT(dnx_l3_vrrp_protocol_group_members_sw_get
                        (unit, tcam_data.protocol_group, tcam_data.protocol_group_mask, &members));

        SHR_IF_ERR_EXIT(dnx_l3_vrrp_protocol_group_internal_flags_to_api_translate(unit, members, &api_flags));
        if (api_flags != flags)
        {
            continue;
        }

        /** add vrid to the returned array */
        vrid_array[(*count)++] = tcam_data.mac_da[5];

        /**  break if all of the array has been filled */
        if (*count == alloc_size)
        {
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get all the VRIDs for which the physical node is master for the
 *      virtual routers on the given VLAN/VSI. VRIDs can be for IPV4 or IPV6
 *
 * \param [in] unit - Unit ID
 * \param [in] flags:
 *  * BCM_L3_VRRP_IPV4 - IPv4 VRRP entry
 *  * BCM_L3_VRRP_IPV6 - IPv6 VRRP entry
 *  * BCM_L3_VRRP_EXTENDED - Indicate usage of extended memory instead of VSI table.
 * \param [in] vlan - VSI ID
 * \param [in] alloc_size - Number of vrid_array elements
 * \param [out] vrid_array -  Pointer to the array to which the VRIDs will be copied
 * \param [out] count - Number of VRIDs copied
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
int
bcm_dnx_l3_vrrp_config_get(
    int unit,
    uint32 flags,
    bcm_vlan_t vlan,
    uint32 alloc_size,
    uint32 *vrid_array,
    uint32 *count)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_l3_vrrp_config_get_verify(unit, flags, vlan, alloc_size, vrid_array, count));

    /** init parameters */
    sal_memset(vrid_array, 0, sizeof(int) * alloc_size);
    *count = 0;

    if (_SHR_IS_FLAG_SET(flags, BCM_L3_VRRP_EXTENDED))
    {
        /** EXEM based VRRP */
        SHR_IF_ERR_EXIT(dnx_l3_vrrp_exem_based_vrid_get(unit, flags, vlan, alloc_size, vrid_array, count));
    }
    else
    {
        /** VSI based VRRP */
        SHR_IF_ERR_EXIT(dnx_l3_vrrp_vsi_based_vrid_get(unit, flags, vlan, alloc_size, vrid_array, count));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify procedure for dnx_l3_vrrp_protocol_group_get().
 */
static shr_error_e
dnx_l3_vrrp_protocol_group_get_verify(
    int unit,
    uint32 *group_members,
    bcm_l3_protocol_group_id_t * group_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify that one of the attributes is not set so it will be later retrieved */
    if (*group_members == DNX_L3_VRRP_PROTOCOL_GROUP_INVALID && *group_id == DNX_L3_VRRP_PROTOCOL_GROUP_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Either group members or group_id must be set.");
    }

    /** verify that not both of the attributes are set */
    if (*group_members != DNX_L3_VRRP_PROTOCOL_GROUP_INVALID && *group_id != DNX_L3_VRRP_PROTOCOL_GROUP_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Group members (%d) and group_id (%d) cant be both set.", *group_members, *group_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_l3_vrrp_protocol_group_get(
    int unit,
    uint32 *group_members,
    bcm_l3_protocol_group_id_t * group_id)
{
    uint32 internal_flags[1] = { 0 }, requested_flags[1] =
    {
    0}, group_mask = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Input validation */
    SHR_INVOKE_VERIFY_DNX(dnx_l3_vrrp_protocol_group_get_verify(unit, group_members, group_id));

    /** Get members by group */
    if (*group_members == DNX_L3_VRRP_PROTOCOL_GROUP_INVALID)
    {
        SHR_IF_ERR_EXIT(dnx_l3_vrrp_protocol_group_members_sw_get(unit, *group_id, DNX_L3_VRRP_PROTOCOL_GROUP_MASK_FULL,
                                                                  internal_flags));

        /** Translate to API. */
        SHR_IF_ERR_EXIT(dnx_l3_vrrp_switch_protocol_group_internal_flags_to_api_translate
                        (unit, internal_flags, group_members));
        SHR_EXIT();
    }

    /** Get group by members */
    if (*group_id == DNX_L3_VRRP_PROTOCOL_GROUP_INVALID)
    {
        /** Translate the flags to match the internal flags */
        SHR_IF_ERR_EXIT(dnx_l3_vrrp_switch_protocol_group_api_flags_to_internal_translate
                        (unit, *group_members, requested_flags));

        SHR_IF_ERR_EXIT(dnx_l3_vrrp_members_to_protocol_group_get(unit, requested_flags, FALSE, group_id, &group_mask));

        /** If the mask is not exact match mask, then the protocols are in more than one group. Return error. */
        if (group_mask != DNX_L3_VRRP_PROTOCOL_GROUP_MASK_FULL || *group_id == DNX_L3_VRRP_PROTOCOL_GROUP_INVALID)
        {
            *group_id = DNX_L3_VRRP_PROTOCOL_GROUP_INVALID;
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Given protocols (%08x) are in more than one group.", *group_members);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify procedure for dnx_l3_vrrp_protocol_group_set()
 */
static shr_error_e
dnx_l3_vrrp_protocol_group_set_verify(
    int unit,
    uint32 group_members,
    bcm_l3_protocol_group_id_t group_id)
{
    uint32 internal_group_members[1] = { 0 }, curr_group_members, count;
    int i;
    SHR_FUNC_INIT_VARS(unit);

    if (group_id >= dnx_data_l3.vrrp.nof_protocol_groups_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "group id (%d) is too high.\n", group_id);
    }

    if (group_id == DNX_L3_VRRP_L3_PROTOCOL_GROUP_ID_DEFAULT)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Can't change group %d. Remove members from other groups to put them in "
                     "this group.\n", group_id);
    }

    /** Translate flags to internal flags */
    SHR_IF_ERR_EXIT(dnx_l3_vrrp_switch_protocol_group_api_flags_to_internal_translate
                    (unit, group_members, internal_group_members));

    /*
     * Traverse all groups other than 0. If the given protocol group is in use, or one of the protocols is in a group
     * that's in use, return error
     */
    for (i = DNX_L3_VRRP_L3_PROTOCOL_GROUP_ID_LOWEST_VALID; i < dnx_data_l3.vrrp.nof_protocol_groups_get(unit); i++)
    {
        /** Read group members from SW DB, use full mask for group id */
        SHR_IF_ERR_EXIT(dnx_l3_vrrp_protocol_group_members_sw_get(unit, i, DNX_L3_VRRP_PROTOCOL_GROUP_MASK_FULL,
                                                                  &curr_group_members));

        /** Read group number of pointers from SW DB. if count is not 0 it means that the group is in use */
        SHR_IF_ERR_EXIT(dnx_l3_vrrp_protocol_group_count_sw_get(unit, i, &count));

        /** verify that the provided protocols are not already mapped go a group which is already in use */
        if (internal_group_members[0] & curr_group_members)
        {
            if (count > 0)
            {
                SHR_ERR_EXIT(_SHR_E_EXISTS, "One of the provided protocols is already mapped to a "
                             "different group (%d) which is already in use. Delete all multiple mymac "
                             "termination or vrrp entries associated with it and try again", i);
            }
        }

        /** verify that the provided group is in use */
        if ((i == group_id))
        {
            if (count > 0)
            {
                SHR_ERR_EXIT(_SHR_E_EXISTS, "The provided group (%d) is already in use. Delete all multiple mymac "
                             "termination or vrrp entries associated with it and try again.", group_id);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_l3_vrrp_protocol_group_set(
    int unit,
    uint32 group_members,
    bcm_l3_protocol_group_id_t group_id)
{
    uint32 internal_group_members[1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    /** verify input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_l3_vrrp_protocol_group_set_verify(unit, group_members, group_id));

    /** Translate flags to internal flags */
    SHR_IF_ERR_EXIT(dnx_l3_vrrp_switch_protocol_group_api_flags_to_internal_translate
                    (unit, group_members, internal_group_members));

    /** Write group members to HW and SW */
    SHR_IF_ERR_EXIT(dnx_l3_vrrp_protocol_group_members_set(unit, group_id, internal_group_members));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_l2_station_mac_is_vrrp_mac(
    int unit,
    bcm_mac_t mac,
    uint8 *is_vrrp)
{
    int i;

    SHR_FUNC_INIT_VARS(unit);

    *is_vrrp = TRUE;

    for (i = 0; i < sizeof(bcm_mac_t) - 1 /** skip vrid */ ; i++)
    {
        if (mac[i] != dnx_l3_vrrp_vrid_mac_ipv4[i] && mac[i] != dnx_l3_vrrp_vrid_mac_ipv6[i])
        {
            *is_vrrp = FALSE;
            break;
        }
    }

/** exit: */
    SHR_FUNC_EXIT;
}

/*
 * }
 */
