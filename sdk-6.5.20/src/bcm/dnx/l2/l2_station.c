/** \file l2_station.c
 * 
 *
 * L2 procedures for DNX.
 *
 * This file contains functions for manipulating the various MAC
 * address tables: my_mac, my_b_mac, and my_cfm_mac (for OAM)
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_L2
/*
 * Include files which are specifically for DNX. Final location.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <src/bcm/dnx/oam/oam_internal.h>
#include <bcm_int/dnx_dispatch.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_l3_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/vlan_access.h>
/*
 * }
 */

/*
 * Include files.
 * {
 */
#include <bcm/types.h>
#include <bcm/l2.h>
#include <bcm/switch.h>
#include <bcm_int/dnx/l3/l3.h>
#include <bcm_int/dnx/l3/l3_vrrp.h>
#include <bcm_int/dnx/l2/l2_station.h>
#include <bcm_int/dnx/vsi/vsi.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <src/bcm/dnx/init/init_pp.h>
/*
 * }
 */

/*
 * }
 */
/*
 * Function Declaration.
 * {
 */

/*
 * }
 */
/*
 * Defines.
 * {
 */

/** l2 station bit indications. Available bits are: 23-31 */

/** bit 27: Vrrp l2 EXEM based station indication. */
#define DNX_L2_STATION_VRRP_EXTENDED_BIT SAL_BIT(27)

/** bit 28: OAM CFM l2 station indication. */
#define DNX_L2_STATION_OAM_CFM_BIT SAL_BIT(28)

/** bit 29: MIM l2 station indication. */
#define DNX_L2_STATION_MIM_BIT SAL_BIT(29)

/** bit 30: Vrrp l2 (VSI based) station indication. */
#define DNX_L2_STATION_VRRP_BIT SAL_BIT(30)

/**
 * brief -The following constants and macros are used to create the station_id for l2 stations using the vrrp VSI table.
 * The bit mapping is:
 * bits 0 - 16: vsi
 * bit 17 empty
 * bit 18-22: Index in the vrrp tcam table
 * bits 23-29 empty
 * bit 30: Vrrp l2 station indication.
 * bit 31: Empty
 */

/**
 * brief - The variable in _station_id will be assigned a vrrp station id according to the format above.
 */
#define DNX_L2_STATION_VRRP_SET(_station_id, _vsi, _tcam_index)                \
    ((_station_id) = (((_vsi) << DNX_L2_STATION_VRRP_VSI_SHIFT ) |              \
                    ((_tcam_index) << DNX_L2_STATION_VRRP_TCAM_INDEX_SHIFT) |   \
                    (DNX_L2_STATION_VRRP_BIT)))

/**
 * brief - Defines for VRRP masks and shifts
 */
#define DNX_L2_STATION_VRRP_VSI_SHIFT (0)
#define DNX_L2_STATION_VRRP_VSI_MASK  (0x1ffff)
#define DNX_L2_STATION_VRRP_TCAM_INDEX_SHIFT (18)
#define DNX_L2_STATION_VRRP_TCAM_INDEX_MASK  (0x1f)

/**
 * brief - Wether the station id is a vrrp station
 */
#define DNX_L2_STATION_IS_VRRP(_station_id) \
    ((_station_id) & DNX_L2_STATION_VRRP_BIT ? TRUE : FALSE)

/**
 * brief - Gets the vsi from a vrrp station id.
 */
#define DNX_L2_STATION_VRRP_VSI_GET(_vsi, _station_id)    \
    ((_vsi) = ((_station_id) >>  DNX_L2_STATION_VRRP_VSI_SHIFT)  \
                & DNX_L2_STATION_VRRP_VSI_MASK )

/**
 * brief - Gets the tcam table index from a vrrp station id.
 */
#define DNX_L2_STATION_VRRP_TCAM_INDEX_GET(_tcam_index, _station_id)    \
    ((_tcam_index) = ((_station_id) >>  DNX_L2_STATION_VRRP_TCAM_INDEX_SHIFT)  \
                 & DNX_L2_STATION_VRRP_TCAM_INDEX_MASK )

/**
 * brief -The following constants and macros are used to create the station_id for l2 stations using the vrrp EXEM table.
 * The bit mapping is:
 * bits 0 - 22: EXEM index
 * bits 23-6 empty
 * bit 27: VRRP EXEM l2 station indication.
 * bits 28-31 empty
 */
#define DNX_L2_STATION_VRRP_EXTENDED_INDEX_SHIFT 0
#define DNX_L2_STATION_VRRP_EXTENDED_INDEX_MASK  (0x3fffff)

/**
 * brief - Set station id with VRRP EXEM index
 */
#define DNX_L2_STATION_VRRP_EXTENDED_SET(_station_id, _exem_index)                      \
    ((_station_id) = (((_exem_index) << DNX_L2_STATION_VRRP_EXTENDED_INDEX_SHIFT ) |    \
                    (DNX_L2_STATION_VRRP_EXTENDED_BIT)))

/**
 * brief - Get VRRP EXEM index from station id
 */
#define DNX_L2_STATION_VRRP_EXTENDED_INDEX_GET(_station_id, _exem_index)         \
    ((_exem_index) = ((_station_id) >>  DNX_L2_STATION_VRRP_EXTENDED_INDEX_SHIFT)  \
                 & DNX_L2_STATION_VRRP_EXTENDED_INDEX_MASK )

/**
 * brief - Wether the station id is a vrrp station
 */
#define DNX_L2_STATION_IS_VRRP_EXEM(_station_id) \
    ((_station_id) & DNX_L2_STATION_VRRP_EXTENDED_BIT ? TRUE : FALSE)

/** The following constants and macros are used to create the
 * station_id for l2 stations using the oam cfm table. The bit
 * mapping is:\n
 * bits 0 - 11: port\n
 * bits 12 - 13 empty\n
 * bits 14-21: Mymac lsb\n
 * bits 22-24 empty\n
 * bits 25-27: port type\n
 * bit 28: OAM CFM l2 station indication.\n
 * bit 29-31: empty   */
#define DNX_L2_STATION_OAM_PORT_SHIFT         (0)
#define DNX_L2_STATION_OAM_PORT_NUM_BITS      (12)
#define DNX_L2_STATION_OAM_PORT_MASK          SAL_UPTO_BIT(DNX_L2_STATION_OAM_PORT_NUM_BITS)
#define DNX_L2_STATION_OAM_MYMAC_LSB_SHIFT    (14)
#define DNX_L2_STATION_OAM_MYMAC_LSB_MASK     SAL_UPTO_BIT(UTILEX_NOF_BITS_IN_BYTE)
#define DNX_L2_STATION_OAM_PORT_TYPE_SHIFT    (25)
#define DNX_L2_STATION_OAM_PORT_TYPE_NUM_BITS (3)
#define DNX_L2_STATION_OAM_PORT_TYPE_MASK     (SAL_UPTO_BIT(DNX_L2_STATION_OAM_PORT_TYPE_NUM_BITS) \
                                               << DNX_L2_STATION_OAM_PORT_TYPE_SHIFT)

/**
 * \brief Build station ID for OAM
 *
 * \param [out] _station_id - Result is written to it
 * \param [in] _port - global port ID (gport)
 * \param [in] _lsb - LSB of MY_CFM_MAC for this station
 *
 * \return
 *   * None
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
#define DNX_L2_STATION_OAM_CFM_SET(_station_id, _port, _lsb)              \
    ((_station_id) = (((_port) << DNX_L2_STATION_OAM_PORT_SHIFT) |         \
                    ((_lsb) << DNX_L2_STATION_OAM_MYMAC_LSB_SHIFT) |      \
                    (DNX_L2_STATION_OAM_CFM_BIT)))

/**
 * \brief Extract port from station ID
 *
 * \param [out] _port - Result is written to it
 * \param [in] _station_id - ID of station
 *
 * \return
 *   * None
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
#define DNX_L2_STATION_OAM_PORT_GET(_port, _station_id)    \
    ((_port) = ((((_station_id) >>  DNX_L2_STATION_OAM_PORT_SHIFT)  \
                & DNX_L2_STATION_OAM_PORT_MASK ) | \
               ((_station_id) & DNX_L2_STATION_OAM_PORT_TYPE_MASK)))

/**
 * \brief Extract MY_CFM_MAC LSB from station ID
 *
 * \param [out] _lsb - Result is written to it
 * \param [in] _station_id - ID of station
 *
 * \return
 *   * None
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
#define DNX_L2_STATION_OAM_MYMAC_LSB_GET(_lsb, _station_id)    \
    ((_lsb) = ((_station_id) >>  DNX_L2_STATION_OAM_MYMAC_LSB_SHIFT)  \
                 & DNX_L2_STATION_OAM_MYMAC_LSB_MASK)

/**
 * \brief Extract MY_CFM_MAC LSB from station ID
 *
 * \param [in] _station_id - ID of station
 *
 * \return
 *   1 for OAM CFM, 0 otherwise
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
#define DNX_L2_STATION_IS_OAM_CFM(_station_id) \
    ((_station_id) & DNX_L2_STATION_OAM_CFM_BIT)

/**
 * \brief - Given the api's l3 protocol group flags, returns the internal flags.
 */
static shr_error_e
dnx_l2_station_protocol_group_api_flags_to_internal_translate(
    int unit,
    uint32 api_flags,
    uint32 *internal_flags)
{
    int offset = 0;
    SHR_FUNC_INIT_VARS(unit);

    *internal_flags = 0;

    if (_SHR_IS_FLAG_SET(api_flags, BCM_L2_STATION_IPV4))
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
    if (_SHR_IS_FLAG_SET(api_flags, BCM_L2_STATION_IPV6))
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
    if (_SHR_IS_FLAG_SET(api_flags, BCM_L2_STATION_ARP_RARP))
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
    if (_SHR_IS_FLAG_SET(api_flags, BCM_L2_STATION_MPLS))
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
    if (_SHR_IS_FLAG_SET(api_flags, BCM_L2_STATION_MIM))
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
    if (_SHR_IS_FLAG_SET(api_flags, BCM_L2_STATION_FCOE))
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
 * \brief - Given the internal l3 protocol group flags, returns the api flags.
 */
static shr_error_e
dnx_l2_station_protocol_group_internal_flags_to_api_translate(
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
            *api_flags |= BCM_L2_STATION_IPV4;
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
            *api_flags |= BCM_L2_STATION_IPV6;
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
            *api_flags |= BCM_L2_STATION_ARP_RARP;
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
            *api_flags |= BCM_L2_STATION_MPLS;
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
            *api_flags |= BCM_L2_STATION_MIM;
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
            *api_flags |= BCM_L2_STATION_FCOE;
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid offset value ");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Argument verification unique to dnx_l2_station_add_my_cfm_mac
 */
static shr_error_e
dnx_l2_station_add_my_cfm_mac_verify(
    int unit,
    bcm_l2_station_t * station)
{
    bcm_mac_t mac_mask;
    SHR_FUNC_INIT_VARS(unit);

    /**
     *  No need to verify that station is not a null pointer.
     *  That was already done in dnx_l2_station_add_verify.
     */

    if (station->src_port_mask != -1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid src_port_mask 0x%08X.  For OAM cfm mymac, src_port_mask must be all ones "
                     "for hardware to work correctly.", station->src_port_mask);
    }

    sal_memset(mac_mask, 0xFF, sizeof(bcm_mac_t));
    if (sal_memcmp(mac_mask, station->dst_mac_mask, sizeof(bcm_mac_t)) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid dst_mac_mask %02X:%02X:%02X:%02X:%02X:%02X.  For OAM cfm mymac, mac mask "
                     "must be all ones for hardware to work correctly.", SAL_MAC_ADDR_LIST(station->dst_mac_mask));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add an entry to one of the my_cfm_mac tables
 *  This function is a part of the bcm_dnx_l2_station_add API.
 *  It creates two MY_CFM_MAC entries from data in the structure
 *  to which station points, using the dnx_oam_my_cfm_mac_add
 *  function.  See notes on that function for dbal and physical
 *  tables.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] station_id - pointer to ID of resulting new
 *        entry
 * \param [in] station - see definition of struct for
 *        description of fields
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * Definition for macro DNX_L2_STATION_OAM_CFM_SET
 */
static shr_error_e
dnx_l2_station_my_cfm_mac_add(
    int unit,
    int *station_id,
    bcm_l2_station_t * station)
{
    uint32 flags;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;
    int my_cfm_mac_by_range;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_IF_ERR_EXIT(dnx_l2_station_add_my_cfm_mac_verify(unit, station));

    /** Get physical port  */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, station->src_port,
                                                    DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    /*
     * Configure oam my cfm mac table - ingress.
     */
    my_cfm_mac_by_range = dnx_data_oam.general.oam_my_cfm_mac_by_range_get(unit);

    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        flags = DNX_OAM_CLASSIFIER_INGRESS;
        if (my_cfm_mac_by_range)
        {
            SHR_IF_ERR_EXIT(dnx_oam_my_cfm_mac_range_add
                            (unit, flags, gport_info.internal_port_pp_info.core_id[pp_port_index],
                             gport_info.internal_port_pp_info.pp_port[pp_port_index], station->dst_mac));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_oam_my_cfm_mac_add(unit, flags, gport_info.internal_port_pp_info.core_id[pp_port_index],
                                                   gport_info.internal_port_pp_info.pp_port[pp_port_index],
                                                   station->dst_mac));
        }

        /*
         * Configure oam my cfm mac table - egress.
         */
        flags = DNX_OAM_CLASSIFIER_EGRESS;
        if (my_cfm_mac_by_range)
        {
            SHR_IF_ERR_EXIT(dnx_oam_my_cfm_mac_range_add
                            (unit, flags, gport_info.internal_port_pp_info.core_id[pp_port_index],
                             gport_info.internal_port_pp_info.pp_port[pp_port_index], station->dst_mac));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_oam_my_cfm_mac_add(unit, flags, gport_info.internal_port_pp_info.core_id[pp_port_index],
                                                   gport_info.internal_port_pp_info.pp_port[pp_port_index],
                                                   station->dst_mac));
        }
    }
    /*
     * Set station_id
     */
    DNX_L2_STATION_OAM_CFM_SET(*station_id, station->src_port, station->dst_mac[UTILEX_PP_MAC_ADDRESS_NOF_U8 - 1]);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - add EXEM based multiple my mac entry
 */
static shr_error_e
dnx_l2_station_my_mac_multi_exem_add(
    int unit,
    int *station_id,
    bcm_l2_station_t * station)
{
    int index;

    SHR_FUNC_INIT_VARS(unit);

    if ((station->flags & (BCM_L2_STATION_MPLS | BCM_L2_STATION_IPV4 | BCM_L2_STATION_IPV6)
         && (station->src_port_mask != 0)))
    {
        /** port+protocol ignore da */
        SHR_IF_ERR_EXIT(dnx_l3_vrrp_exem_source_port_protocol_add(unit, station->src_port, station->flags, &index));
    }
    else if (station->src_port_mask != 0)
    {
        /** port base multiple my mac */
        SHR_IF_ERR_EXIT(dnx_l3_vrrp_exem_source_port_my_mac_add(unit, station->src_port, station->dst_mac, &index));
    }
    else
    {
        /** vsi based multiple my mac */
        SHR_IF_ERR_EXIT(dnx_l3_vrrp_exem_vsi_my_mac_add(unit, station->vlan, station->dst_mac, &index));
    }

    /** set station */
    DNX_L2_STATION_VRRP_EXTENDED_SET(*station_id, index);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Fill algo_dnx_l3_vrrp_tcam_key_t structure with provided multi my mac data data
 */
static shr_error_e
dnx_l2_station_my_mac_multi_vsi_tcam_info_fill(
    int unit,
    bcm_l2_station_t * station,
    algo_dnx_l3_vrrp_tcam_key_t * tcam_data)
{
    uint32 protocol_flags[1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(tcam_data, 0, sizeof(algo_dnx_l3_vrrp_tcam_key_t));

    /** construct tcam data */
    {
        /** mac */
        sal_memcpy(tcam_data->mac_da, station->dst_mac, sizeof(bcm_mac_t));

        /** mac mask */
        sal_memcpy(tcam_data->mac_da_mask, station->dst_mac_mask, sizeof(bcm_mac_t));

        /** set group and group mask */
        if (station->flags == 0)
        {
            /** Special case. flags==0 indicates all protocols, set mask to 0 */
            tcam_data->protocol_group = 0;
            tcam_data->protocol_group_mask = 0;

        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_l2_station_protocol_group_api_flags_to_internal_translate
                            (unit, station->flags, protocol_flags));
            SHR_IF_ERR_EXIT(dnx_l3_vrrp_members_to_protocol_group_get(unit, protocol_flags, TRUE,
                                                                      &(tcam_data->protocol_group),
                                                                      &(tcam_data->protocol_group_mask)));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - add/remove VSI based multiple my mac entry
 */
static shr_error_e
dnx_l2_station_my_mac_multi_vsi_set(
    int unit,
    int *station_id,
    bcm_l2_station_t * station,
    uint32 is_add)
{

    algo_dnx_l3_vrrp_tcam_key_t tcam_data;
    uint32 tcam_index;

    SHR_FUNC_INIT_VARS(unit);

   /** construct tcam key data */
    SHR_IF_ERR_EXIT(dnx_l2_station_my_mac_multi_vsi_tcam_info_fill(unit, station, &tcam_data));

    if (is_add == TRUE)
    {
        /** allocate and set the entry */
        SHR_IF_ERR_EXIT(dnx_l3_vrrp_setting_add(unit, &tcam_data, station->vlan, &tcam_index));

        /** set station_id */
        DNX_L2_STATION_VRRP_SET(*station_id, station->vlan, tcam_index);
    }
    else
    {
        /** Deallocate and clear the entry */
        SHR_IF_ERR_EXIT(dnx_l3_vrrp_setting_delete(unit, &tcam_data, station->vlan));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add an entry to one of the my_b_mac tables
 *  This function is a part of the bcm_dnx_l2_station_add API.
 *  This part is not supported
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] station_id - pointer to ID of resulting new
 *        entry
 * \param [in] station - see definition of struct for
 *        description of fields
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
dnx_l2_station_my_b_mac_add(
    int unit,
    int *station_id,
    bcm_l2_station_t * station)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_l2_station_add does not support my_b_mac");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Argument verification for dnx_l2_station_my_mac_add
 */
static shr_error_e
dnx_l2_station_my_mac_add_verify(
    int unit,
    int *station_id,
    bcm_l2_station_t * station)
{
    uint32 supported_flags;
    uint8 intf_exists;
    uint8 is_vsi_allocated;
    SHR_FUNC_INIT_VARS(unit);

    /** my mac */
    if ((station->vlan_mask == 0) && (station->src_port_mask == 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "bcm_dnx_l2_station_add does not support my_mac."
                     "Global My MAC was changed and allocated by bcm_l3_intf_create only. See more information in "
                     "backward compatible document.");
    }
    /** exem bsaed multiple my mac */
    else if (_SHR_IS_FLAG_SET(station->flags, BCM_L2_STATION_EXTENDED))
    {
        int capacity;

        SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_SEXEM_1, &capacity));

        if (capacity == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Multiple my mac extended memory is not supported");
        }

        /** Verify supported flags */
        supported_flags = BCM_L2_STATION_EXTENDED | BCM_L2_STATION_MPLS | BCM_L2_STATION_IPV4 | BCM_L2_STATION_IPV6;
        SHR_MASK_VERIFY(station->flags, supported_flags, _SHR_E_PARAM, "non supported flags specified,"
                        "l3 protocols are irrelevant when working in extended mode.\n");

        if ((_SHR_IS_FLAG_SET(station->flags, BCM_L2_STATION_MPLS)
             && _SHR_IS_FLAG_SET(station->flags, BCM_L2_STATION_IPV4))
            || (_SHR_IS_FLAG_SET(station->flags, BCM_L2_STATION_MPLS)
                && _SHR_IS_FLAG_SET(station->flags, BCM_L2_STATION_IPV6))
            || (_SHR_IS_FLAG_SET(station->flags, BCM_L2_STATION_IPV4)
                && _SHR_IS_FLAG_SET(station->flags, BCM_L2_STATION_IPV6)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid protocol flags - Only one of MPLS, IPV4, IPV6 must be provided\n");
        }
        if (station->src_port_mask != 0 && station->vlan_mask)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Multiple my mac extended mode supports either per port or per VSI");
        }

        /** verify port mode */
        if (station->src_port_mask != 0)
        {
            int arg;

            /** verify system mode */
            SHR_IF_ERR_EXIT(bcm_dnx_switch_control_get(unit, bcmSwitchL2StationExtendedMode, &arg));
            if (arg == 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "bcmSwitchL2StationExtendedMode is (%d), to set extended multiple my mac "
                             "system mode to 'port based(1) or port+protocol(2)'", arg);
            }
        }

        /**  verify vsi mode */
        if (station->vlan != 0)
        {
            int arg;
            int nof_vsis;
            SHR_IF_ERR_EXIT(dnx_vsi_count_get(unit, &nof_vsis));

            /** verify system mode */
            SHR_IF_ERR_EXIT(bcm_dnx_switch_control_get(unit, bcmSwitchL2StationExtendedMode, &arg));
            if (arg != 0)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                             "bcmSwitchL2StationExtendedMode is (%d), to set extended multiple my mac "
                             "system mode to 'VSI based' set it to 0", arg);
            }

            /** verify valid VSI ID */
            if (station->vlan >= nof_vsis)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong setting. vsi = %d is not a valid vsi.", station->vlan);
            }

            /** verify VSI is allocated by L3 Intf or some other VPN API */
            is_vsi_allocated = 0;
            SHR_IF_ERR_EXIT(vlan_db.vsi.is_allocated(unit, station->vlan, &is_vsi_allocated));
            intf_exists = 0;
            SHR_IF_ERR_EXIT(dnx_l3_check_if_ingress_intf_exists(unit, station->vlan, &intf_exists));
            if (!intf_exists && !is_vsi_allocated)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                             "Station APIs per VSI is called but VPN API or L3 Intf is not created - vsi_rif_id = %d.",
                             station->vlan);
            }
        }

    }
    /** vsi based multiple my mac */
    else
    {
        if (station->vlan_mask != 0xffff)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "VSI mask must be full");
        }

        if (station->src_port_mask != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Multiple my mac does not supported source port");
        }

        /** verify VSI is allocated by L3 Intf */
        is_vsi_allocated = 0;
        SHR_IF_ERR_EXIT(vlan_db.vsi.is_allocated(unit, station->vlan, &is_vsi_allocated));
        intf_exists = 0;
        SHR_IF_ERR_EXIT(dnx_l3_check_if_ingress_intf_exists(unit, station->vlan, &intf_exists));
        if (!intf_exists && !is_vsi_allocated)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                         "Station APIs per VSI is called but L3 Intf is not created - vsi_rif_id = %d.", station->vlan);
        }

        /** Verify supported flags */
        supported_flags = BCM_L2_STATION_IPV4 | BCM_L2_STATION_IPV6 | BCM_L2_STATION_ARP_RARP | BCM_L2_STATION_MPLS |
            BCM_L2_STATION_MIM | BCM_L2_STATION_FCOE;
        SHR_MASK_VERIFY(station->flags, supported_flags, _SHR_E_PARAM, "non supported flags specified.\n");
    }

    /** verify that non VRRP address was provided */
    {
        uint8 is_vrrp;

        SHR_IF_ERR_EXIT(dnx_l2_station_mac_is_vrrp_mac(unit, station->dst_mac, &is_vrrp));

        if (is_vrrp == TRUE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "To configure VRRP mymac addresses, please use bcm_l3_vrrp_config_add.");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add an entry to one of the my_mac tables
 *  This function is a part of the bcm_dnx_l2_station_add API.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] station_id - pointer to ID of resulting new
 *        entry
 * \param [in] station - see definition of struct for
 *        description of fields
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
dnx_l2_station_my_mac_add(
    int unit,
    int *station_id,
    bcm_l2_station_t * station)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l2_station_my_mac_add_verify(unit, station_id, station));

    if (_SHR_IS_FLAG_SET(station->flags, BCM_L2_STATION_EXTENDED))
    {
        SHR_IF_ERR_EXIT(dnx_l2_station_my_mac_multi_exem_add(unit, station_id, station));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_l2_station_my_mac_multi_vsi_set(unit, station_id, station, TRUE));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Argument verification for bcm_dnx_l2_station_add
 */
static shr_error_e
dnx_l2_station_add_verify(
    int unit,
    int *station_id,
    bcm_l2_station_t * station)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(station_id, _SHR_E_PARAM, "station_id");
    SHR_NULL_CHECK(station, _SHR_E_PARAM, "station");

    if (station->flags & BCM_L2_STATION_WITH_ID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_L2_STATION_WITH_ID flag is not supported");
    }
    if (station->flags & BCM_L2_STATION_REPLACE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_L2_STATION_REPLACE flag is not supported");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Create a my_mac, my_b_mac or my_cfm_mac entry
 *
 * This API is necessary for backwards compatibility.  An entry
 * is created and written to a table that is used by the
 * hardware for classification of incoming packets.  The exact
 * table and the entry within that table are determined by the
 * fields of the "station" structure.  The resulting
 * "station_id" value can be used to subsequently retrieve or
 * delete the data.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] station_id - pointer to ID of resulting new
 *        entry
 * \param [in] station - see definition of struct for
 *        description of fields
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_l2_station_add(
    int unit,
    int *station_id,
    bcm_l2_station_t * station)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_l2_station_add_verify(unit, station_id, station));

    if ((station->flags & BCM_L2_STATION_OAM) && !station->vlan_mask)
    {
        /** OAM My-CFM-MAC */
        SHR_IF_ERR_EXIT(dnx_l2_station_my_cfm_mac_add(unit, station_id, station));
    }
    else if ((station->flags & BCM_L2_STATION_MIM) && !station->vlan_mask)
    {
        /** my-B-Mac */
        SHR_IF_ERR_EXIT(dnx_l2_station_my_b_mac_add(unit, station_id, station));
    }
    else
    {
        /** my-Mac */
        SHR_IF_ERR_EXIT(dnx_l2_station_my_mac_add(unit, station_id, station));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get an entry from one of the my_cfm_mac tables
 *  This function is a part of the bcm_dnx_l2_station_get API.
 *  It reads a MY_CFM_MAC entry using the dnx_oam_my_cfm_mac_get
 *  function, with arguments taken from breaking the station_id
 *  value into its original components. See notes on the
 *  function bcm_dnx_l2_station_get for dbal and physical table
 *  information.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] station_id - ID of entry to read
 * \param [out] station - see definition of struct for
 *        description of fields
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * Definitions for macros:\n
 *     - DNX_L2_STATION_OAM_PORT_GET\n
 *     - DNX_L2_STATION_OAM_MYMAC_LSB_GET
 */
static shr_error_e
dnx_l2_station_my_cfm_mac_get(
    int unit,
    int station_id,
    bcm_l2_station_t * station)
{
    bcm_port_t port;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    SHR_FUNC_INIT_VARS(unit);

    /** Fill in the different fields in station */

    /** Fixed value fields   */
    station->flags = BCM_L2_STATION_OAM;
    station->src_port_mask = -1;
    sal_memset(station->dst_mac_mask, 0xFF, sizeof(bcm_mac_t));

    /** Get port from station id, and write to station. */
    DNX_L2_STATION_OAM_PORT_GET(port, station_id);
    station->src_port = port;

    /** Get information about that port */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY,
                                                    &gport_info));

    /** Use that information to get station mac */
    SHR_IF_ERR_EXIT(dnx_oam_my_cfm_mac_get(unit, 0, gport_info.internal_port_pp_info.core_id[0],
                                           gport_info.internal_port_pp_info.pp_port[0], station->dst_mac));

    /** Get my_cfm_mac LSB from station ID */
    DNX_L2_STATION_OAM_MYMAC_LSB_GET(station->dst_mac[UTILEX_PP_MAC_ADDRESS_NOF_U8 - 1], station_id);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get an entry to one of the my_b_mac tables.
 *  This function is a part of the bcm_dnx_l2_station_get API.
 *  This part is not supported.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] station_id - ID of entry to read
 * \param [out] station - see definition of struct for
 *        description of fields
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
dnx_l2_station_my_b_mac_get(
    int unit,
    int station_id,
    bcm_l2_station_t * station)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_l2_station_get does not support my_b_mac");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify procedure for dnx_l2_station_my_mac_get
 */
static shr_error_e
dnx_l2_station_my_mac_get_verify(
    int unit,
    int station_id,
    bcm_l2_station_t * station)
{
    uint8 is_allocated;

    SHR_FUNC_INIT_VARS(unit);

    /** only multiple my mac (vrrp) is supported */
    if ((DNX_L2_STATION_IS_VRRP(station_id) == FALSE) && (DNX_L2_STATION_IS_VRRP_EXEM(station_id) == FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_l2_station_get does not support my_mac");
    }

    /** EXTENDED (EXEM) verification */
    if (DNX_L2_STATION_IS_VRRP_EXEM(station_id) == TRUE)
    {
        uint32 exem_index;

        /** verify entry exists */
        DNX_L2_STATION_VRRP_EXTENDED_INDEX_GET(station_id, exem_index);
        SHR_IF_ERR_EXIT(algo_l3_db.vrrp_exem_l2_station_id_alloc.
                        res_bitmap.is_allocated(unit, exem_index, &is_allocated));
        if (is_allocated == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "The provided station_id index (%d) was not allocated", exem_index);
        }
    }
    /** VSI based l2 station verification */
    else
    {
        algo_dnx_l3_vrrp_tcam_key_t tcam_info;
        uint32 tcam_index;
        int ref_count;

        /** get tcam index */
        DNX_L2_STATION_VRRP_TCAM_INDEX_GET(tcam_index, station_id);

        /** verify entry exists */
        SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.vrrp_vsi_tcam_table.profile_data_get
                        (unit, tcam_index, &ref_count, (void *) &tcam_info));
        if (ref_count == 0)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "The provided station_id index (%d) was not allocated", tcam_index);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get EXEM based multiple my mac entry
 */
static shr_error_e
dnx_l2_station_my_mac_multi_exem_get(
    int unit,
    int station_id,
    bcm_l2_station_t * station)
{
    dbal_enum_value_result_type_l3_vrrp_exem_db_sw_e result_type;
    uint32 pp_port, index, layer_type;
    bcm_vlan_t vlan;
    int core;
    bcm_mac_t mac_da;
    bcm_mac_t mac_da_mask = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    dbal_enum_value_field_layer_types_e protocol;

    SHR_FUNC_INIT_VARS(unit);

    station->flags |= BCM_L2_STATION_EXTENDED;

    DNX_L2_STATION_VRRP_EXTENDED_INDEX_GET(station_id, index);
    SHR_IF_ERR_EXIT(dnx_l3_vrrp_exem_multi_my_mac_get
                    (unit, index, &result_type, &vlan, &core, &pp_port, mac_da, &protocol));

    if (result_type == DBAL_RESULT_TYPE_L3_VRRP_EXEM_DB_SW_VSI)
    {
        station->vlan = vlan;
        station->vlan_mask = 0xffff;
    }
    else if (result_type == DBAL_RESULT_TYPE_L3_VRRP_EXEM_DB_SW_PORT)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_pp_to_logical_get(unit, core, pp_port, &(station->src_port)));
        station->src_port_mask = 0xffff;
    }
    else if (result_type == DBAL_RESULT_TYPE_L3_VRRP_EXEM_DB_SW_PORT_PROTOCOL)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_pp_to_logical_get(unit, core, pp_port, &(station->src_port)));
        station->src_port_mask = 0xffff;
        SHR_IF_ERR_EXIT(dbal_fields_enum_value_get(unit, DBAL_FIELD_LAYER_TYPES, protocol, &layer_type));
        if (layer_type == DBAL_ENUM_FVAL_LAYER_TYPES_MPLS)
        {
            station->flags |= BCM_L2_STATION_MPLS;
        }
        else if (layer_type == DBAL_ENUM_FVAL_LAYER_TYPES_IPV4)
        {
            station->flags |= BCM_L2_STATION_IPV4;
        }
        else if (layer_type == DBAL_ENUM_FVAL_LAYER_TYPES_IPV6)
        {
            station->flags |= BCM_L2_STATION_IPV6;
        }

    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported EXEM VRRP entry type = %d.", result_type);
    }

    if (result_type != DBAL_RESULT_TYPE_L3_VRRP_EXEM_DB_SW_PORT_PROTOCOL)
    {
        sal_memcpy(station->dst_mac, mac_da, sizeof(bcm_mac_t));
        sal_memcpy(station->dst_mac_mask, mac_da_mask, sizeof(bcm_mac_t));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get VSI based multiple my mac entry
 */
static shr_error_e
dnx_l2_station_my_mac_multi_vsi_get(
    int unit,
    int station_id,
    bcm_l2_station_t * station)
{
    algo_dnx_l3_vrrp_tcam_key_t tcam_info;
    uint32 tcam_index, internal_flags[1] = { 0 };
    int ref_count;

    SHR_FUNC_INIT_VARS(unit);

    /** get tcam index */
    DNX_L2_STATION_VRRP_TCAM_INDEX_GET(tcam_index, station_id);

    /** read the tcam entry */
    SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.vrrp_vsi_tcam_table.profile_data_get
                    (unit, tcam_index, &ref_count, (void *) &tcam_info));

    /** get mac */
    sal_memcpy(station->dst_mac, tcam_info.mac_da, sizeof(bcm_mac_t));
    sal_memcpy(station->dst_mac_mask, tcam_info.mac_da_mask, sizeof(bcm_mac_t));

    /** get flags */
    {
        /** get internal protocol flags */
        SHR_IF_ERR_EXIT(dnx_l3_vrrp_protocol_group_members_sw_get(unit, tcam_info.protocol_group,
                                                                  tcam_info.protocol_group_mask, internal_flags));

        /** translate internal flags to API flags */
        SHR_IF_ERR_EXIT(dnx_l2_station_protocol_group_internal_flags_to_api_translate
                        (unit, internal_flags, &(station->flags)));
    }

    /** get vsi */
    DNX_L2_STATION_VRRP_VSI_GET(station->vlan, station_id);
    station->vlan_mask = -1;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get an entry to one of the my_mac tables.
 *  This function is a part of the bcm_dnx_l2_station_get API.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] station_id - ID of entry to read
 * \param [out] station - see definition of struct for
 *        description of fields
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
dnx_l2_station_my_mac_get(
    int unit,
    int station_id,
    bcm_l2_station_t * station)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l2_station_my_mac_get_verify(unit, station_id, station));

    /** init */
    sal_memset(station, 0x0, sizeof(bcm_l2_station_t));

    if ((DNX_L2_STATION_IS_VRRP_EXEM(station_id) == TRUE))
    {
        SHR_IF_ERR_EXIT(dnx_l2_station_my_mac_multi_exem_get(unit, station_id, station));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_l2_station_my_mac_multi_vsi_get(unit, station_id, station));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Argument verification for bcm_dnx_l2_station_get
 */
static shr_error_e
dnx_l2_station_get_verify(
    int unit,
    int station_id,
    bcm_l2_station_t * station)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(station, _SHR_E_PARAM, "station");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Read a my_mac, my_b_mac or my_cfm_mac entry
 *
 * This API is necessary for backwards compatibility.  An entry
 * from one of a variety of physical tables is read and
 * formatted into a "station" structure.  The exact table and
 * the exact entry within the table are determined by breaking
 * down the "station ID" value into its original components.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] station_id - ID of entry to read
 * \param [out] station - see definition of struct for
 *        description of fields
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * Definition for macro DNX_L2_STATION_IS_OAM_CFM.
 */
int
bcm_dnx_l2_station_get(
    int unit,
    int station_id,
    bcm_l2_station_t * station)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_l2_station_get_verify(unit, station_id, station));

    if (DNX_L2_STATION_IS_OAM_CFM(station_id))  /* OAM endpoint */
    {
        /** OAM endpoint */
        SHR_IF_ERR_EXIT(dnx_l2_station_my_cfm_mac_get(unit, station_id, station));
    }
    else if (station_id & DNX_L2_STATION_MIM_BIT)
    {
        /** my-B-Mac */
        SHR_IF_ERR_EXIT(dnx_l2_station_my_b_mac_get(unit, station_id, station));
    }
    else
    {
        /** my-Mac */
        SHR_IF_ERR_EXIT(dnx_l2_station_my_mac_get(unit, station_id, station));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete an entry from one of the my_cfm_mac tables.
 *  This function is a part of df the bcm_dnx_l2_station_delete
 *  API.  It deletes a MY_CFM_MAC entry using the
 *  dnx_oam_my_cfm_mac_delete function, with arguments taken
 *  from breaking the station_id value into its original
 *  components. See notes on the function
 *  dnx_oam_my_cfm_mac_delete for dbal and physical table
 *  information.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] station_id - ID of entry to delete
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * Definitions for macros:\n
 *     - DNX_L2_STATION_OAM_PORT_GET\n
 *     - DNX_L2_STATION_OAM_MYMAC_LSB_GET
 */
static shr_error_e
dnx_l2_station_my_cfm_mac_delete(
    int unit,
    int station_id)
{
    bcm_port_t port;
    bcm_mac_t mac_to_delete;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;
    int my_cfm_mac_by_range;

    SHR_FUNC_INIT_VARS(unit);

    /** Get port from station id. */
    DNX_L2_STATION_OAM_PORT_GET(port, station_id);
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY,
                                                    &gport_info));

    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        /** Get my_cfm_mac MSB from table   */
        SHR_IF_ERR_EXIT(dnx_oam_my_cfm_mac_get(unit, 0, gport_info.internal_port_pp_info.core_id[pp_port_index],
                                               gport_info.internal_port_pp_info.pp_port[pp_port_index], mac_to_delete));

        /** Get my_cfm_mac LSB from station ID */
        DNX_L2_STATION_OAM_MYMAC_LSB_GET(mac_to_delete[UTILEX_PP_MAC_ADDRESS_NOF_U8 - 1], station_id);

        /** Delete the resulting MAC address from ingress */
        my_cfm_mac_by_range = dnx_data_oam.general.oam_my_cfm_mac_by_range_get(unit);

        if (my_cfm_mac_by_range)
        {
            /** Delete the resulting MAC address from ingress */
            SHR_IF_ERR_EXIT(dnx_oam_my_cfm_mac_range_delete
                            (unit, DNX_OAM_CLASSIFIER_INGRESS, gport_info.internal_port_pp_info.core_id[pp_port_index],
                             gport_info.internal_port_pp_info.pp_port[pp_port_index], mac_to_delete));

            /** Delete the resulting MAC address from egress */
            SHR_IF_ERR_EXIT(dnx_oam_my_cfm_mac_range_delete
                            (unit, DNX_OAM_CLASSIFIER_EGRESS, gport_info.internal_port_pp_info.core_id[pp_port_index],
                             gport_info.internal_port_pp_info.pp_port[pp_port_index], mac_to_delete));
        }
        else
        {
            /** Delete the resulting MAC address from ingress */
            SHR_IF_ERR_EXIT(dnx_oam_my_cfm_mac_delete
                            (unit, DNX_OAM_CLASSIFIER_INGRESS, gport_info.internal_port_pp_info.core_id[pp_port_index],
                             gport_info.internal_port_pp_info.pp_port[pp_port_index], mac_to_delete));

            /** Delete the resulting MAC address from egress */
            SHR_IF_ERR_EXIT(dnx_oam_my_cfm_mac_delete
                            (unit, DNX_OAM_CLASSIFIER_EGRESS, gport_info.internal_port_pp_info.core_id[pp_port_index],
                             gport_info.internal_port_pp_info.pp_port[pp_port_index], mac_to_delete));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete an entry to one of the my_b_mac tables.
 *  This function is a part of the bcm_dnx_l2_station_delete
 *  API. This part is not supported.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] station_id - ID of entry to read
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
dnx_l2_station_my_b_mac_delete(
    int unit,
    int station_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_l2_station_delete does not support my_b_mac");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - delete EXEM based multiple my mac entry
 */
static shr_error_e
dnx_l2_station_my_mac_multi_exem_delete(
    int unit,
    int station_id)
{
    uint32 index;

    SHR_FUNC_INIT_VARS(unit);

    /** get EXEM sw index */
    DNX_L2_STATION_VRRP_EXTENDED_INDEX_GET(station_id, index);

    /** delete the entry from SW and HW tables */
    SHR_IF_ERR_EXIT(dnx_l3_vrrp_exem_multi_my_mac_delete(unit, index));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - delete VSI based multiple my mac entry
 */
static shr_error_e
dnx_l2_station_my_mac_multi_vsi_delete(
    int unit,
    int station_id)
{
    algo_dnx_l3_vrrp_tcam_key_t tcam_info;
    uint32 tcam_index;
    int vlan, ref_count;

    SHR_FUNC_INIT_VARS(unit);

    /** get tcam index */
    DNX_L2_STATION_VRRP_TCAM_INDEX_GET(tcam_index, station_id);

    /** read the tcam entry */
    SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.vrrp_vsi_tcam_table.profile_data_get
                    (unit, tcam_index, &ref_count, (void *) &tcam_info));
    /** get vsi */
    DNX_L2_STATION_VRRP_VSI_GET(vlan, station_id);

    /*
     * 1. Deallocate and clear the tcam entry.
     * 2. Remove tcam entry from vsi members bitmap
     */
    SHR_IF_ERR_EXIT(dnx_l3_vrrp_setting_delete(unit, &tcam_info, vlan));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete an entry to one of the my_mac tables.
 *  This function is a part of the bcm_dnx_l2_station_delete
 *  API.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] station_id - ID of entry to read
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
dnx_l2_station_my_mac_delete(
    int unit,
    int station_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify */
    SHR_INVOKE_VERIFY_DNX(dnx_l2_station_my_mac_get_verify(unit, station_id, NULL));

    if ((DNX_L2_STATION_IS_VRRP_EXEM(station_id) == TRUE))
    {
        SHR_IF_ERR_EXIT(dnx_l2_station_my_mac_multi_exem_delete(unit, station_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_l2_station_my_mac_multi_vsi_delete(unit, station_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete a my_mac, my_b_mac or my_cfm_mac entry
 *
 * This is an API that is also necessary for backwards
 * compatibility.  An entry from one of a variety of physical
 * tables is deleted.  The exact table and the exact entry
 * within the table are determined by breaking down the "station
 * ID" value into its original components.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] station_id - ID of entry to delete
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_l2_station_delete(
    int unit,
    int station_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    if (DNX_L2_STATION_IS_OAM_CFM(station_id))  /* OAM endpoint */
    {
        /** OAM endpoint */
        SHR_IF_ERR_EXIT(dnx_l2_station_my_cfm_mac_delete(unit, station_id));
    }
    else if (station_id & DNX_L2_STATION_MIM_BIT)
    {
        /** my-B-Mac */
        SHR_IF_ERR_EXIT(dnx_l2_station_my_b_mac_delete(unit, station_id));
    }
    else
    {
        /** my-Mac */
        SHR_IF_ERR_EXIT(dnx_l2_station_my_mac_delete(unit, station_id));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify procedure for dnx_l2_station_multi_my_mac_exem_mode_set().
 */
static shr_error_e
dnx_l2_station_multi_my_mac_exem_mode_set_verify(
    int unit,
    int arg)
{
    int nof_free_elements;
    int capacity;
    SHR_FUNC_INIT_VARS(unit);

    if (arg > 2)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Multiple my mac exem mode (%d) not supported", arg);
    }

    /** Before setting the mode, verify that L2 station EXEM tables are empty */
    SHR_IF_ERR_EXIT(algo_l3_db.vrrp_exem_l2_station_id_alloc.res_bitmap.nof_free_elements_get
                    (unit, &nof_free_elements));

    SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_SEXEM_1, &capacity));

    if (nof_free_elements != capacity)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "My Mac EXEM mode can't be changed if EXEM table isn't empty");
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_l2_station_multi_my_mac_exem_mode_set(
    int unit,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l2_station_multi_my_mac_exem_mode_set_verify(unit, arg));

    /** set mode to hw (tcam da profile) */
    {
        algo_dnx_l3_vrrp_tcam_key_t tcam_data;
        uint32 tcam_index, tcam_index_msb, da_profile;

        /** clear TCAM data since its data is irrelevant */
        sal_memset(&tcam_data, 0, sizeof(algo_dnx_l3_vrrp_tcam_key_t));

        /** get dedicated tcam index for exem vsi/source-port */
        tcam_index = dnx_data_l3.vrrp.exem_default_tcam_index_get(unit);
        tcam_index_msb = dnx_data_l3.vrrp.exem_default_tcam_index_msb_get(unit);

        /** set tcam */
        if (arg == 0)
        {
            /** set VSI TCAM first index */
            {
                da_profile = DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_PER_VSI_MULTIPLE_MY_MAC_DA_MSB_0;

                /** set MAC MSB bit to 0 */
                tcam_data.mac_da[0] = 0x0;
                tcam_data.mac_da_mask[0] = 0x80;

                SHR_IF_ERR_EXIT(dnx_l3_vrrp_tcam_info_set(unit, tcam_index, &tcam_data, da_profile));
            }

            /** set VSI TCAM second index */
            {
                da_profile = DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_PER_VSI_MULTIPLE_MY_MAC_DA_MSB_1;

                /** set MAC MSB bit to 1 */
                tcam_data.mac_da[0] = 0x80;
                tcam_data.mac_da_mask[0] = 0x80;

                SHR_IF_ERR_EXIT(dnx_l3_vrrp_tcam_info_set(unit, tcam_index_msb, &tcam_data, da_profile));
            }
        }
        else if (arg == 1)
        {
            da_profile = DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_PER_PORT_MY_MAC;
            /** set TCAM */
            SHR_IF_ERR_EXIT(dnx_l3_vrrp_tcam_info_set(unit, tcam_index, &tcam_data, da_profile));

            /** clear second TCAM index (it's only used for VSI) */
            SHR_IF_ERR_EXIT(dnx_l3_vrrp_tcam_info_clear(unit, tcam_index_msb));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_l3_vrrp_port_protocol_tcam_info_set(unit, BCM_SWITCH_L3_PROTOCOL_GROUP_MPLS));
        }

    }

    /** set mode in sw for easy access */
    {
        uint32 entry_handle_id;

        /** create handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L3_VRRP_SYSTEM_CONFIG_SW, &entry_handle_id));

        /** set exem system mode */
        if (arg == 0)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_EXEM_MODE, INST_SINGLE,
                                         DBAL_ENUM_FVAL_VRRP_EXEM_MODE_VSI);
        }
        else if (arg == 1)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_EXEM_MODE, INST_SINGLE,
                                         DBAL_ENUM_FVAL_VRRP_EXEM_MODE_SOURCE_PORT);
        }
        else if (arg == 2)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRRP_EXEM_MODE, INST_SINGLE,
                                         DBAL_ENUM_FVAL_VRRP_EXEM_MODE_SOURCE_PORT_PROTOCOL);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Multiple my mac exem mode (%d) not supported.", arg);
        }

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
dnx_l2_station_multi_my_mac_exem_mode_get(
    int unit,
    int *arg)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_vrrp_exem_mode_e exem_mode;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** create handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L3_VRRP_SYSTEM_CONFIG_SW, &entry_handle_id));

    /** Get exem mode */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_VRRP_EXEM_MODE, INST_SINGLE, &exem_mode);

    /** commit */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** set system mode */
    if (exem_mode == DBAL_ENUM_FVAL_VRRP_EXEM_MODE_VSI)
    {
        *arg = 0;
    }
    else if (exem_mode == DBAL_ENUM_FVAL_VRRP_EXEM_MODE_SOURCE_PORT)
    {
        *arg = 1;
    }
    else if (exem_mode == DBAL_ENUM_FVAL_VRRP_EXEM_MODE_SOURCE_PORT_PROTOCOL)
    {
        *arg = 2;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Multiple my mac exem mode (%d) not supported.", exem_mode);
    }

    /** commit */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
