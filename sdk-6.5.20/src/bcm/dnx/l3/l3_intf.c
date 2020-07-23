/** \file l3_intf.c
 *
 *  l3 interface for DNX.
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
 * Include files.
 * {
 */
#include <bcm_int/dnx/lif/in_lif_profile.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm/l3.h>
#include <bcm_int/dnx/qos/qos.h>
#include <bcm_int/dnx/l3/l3.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/l3/l3_arp.h>
#include <bcm_int/dnx/l3/l3_fec.h>
#include <bcm_int/dnx/stg/stg.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <shared/util.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <shared/utilex/utilex_bitstream.h>
#include <bcm_int/dnx/algo/l3/source_address_table_allocation.h>
#include <bcm_int/dnx/qos/qos.h>
#include <bcm_int/dnx/algo/lif_mngr/algo_in_lif_profile.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_switch.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/vlan_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_l3_access.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/stat/stat_pp.h>
#include <bcm_int/dnx/lif/out_lif_profile.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stat.h>
/*
 * }
 */

/*
 * DEFINES
 * {
 */

#define L3_INTF_LOWEST_2_BITS_MASK  3
#define L3_INTF_LOWEST_6_BITS_MASK  0x3F
#define L3_INTF_LOWEST_8_BITS_MASK  0xFF
#define L3_INTF_HIGHEST_6_BITS_MASK 0xFC

#define L3_INTF_EEDB_MAX_INDX_SMALL_ENTRY 0x8000
#define OAM_DEFAULT_PROFILE_MAX_VALUE 3
/*
 * }
 */

/*
 * MACROs
 * {
 */

#define L3_INTF_CLEAR_LSB_BITS(my_mac)

/** revert L3_INTF_MY_MAC_ETH_PREFIX */
#define L3_INTF_MY_MAC_ETH_PREFIX_RESTORE(prefix) \
    prefix = prefix << 1

/** mask 10MSB from mac address */
#define L3_INTF_MAC_ADDR_MASK_10MSB(src, dst) \
    sal_memcpy(dst, src, L3_MAC_ADDR_SIZE_IN_BYTES); \
    dst[0] = 0; \
    dst[1] &= 0x3F

/** mask 10LSB from mac address */
#define L3_INTF_MAC_ADDR_MASK_10LSB(mac_addr) \
    mac_addr[5] = 0; \
    mac_addr[4] &= 0xFC;

#define L3_INTF_INGRESS_SUPPORTED_BCM_L3_FLAGS (BCM_L3_INGRESS_WITH_ID | BCM_L3_INGRESS_REPLACE | BCM_L3_INGRESS_L3_MCAST_L2 | BCM_L3_INGRESS_IP6_L3_MCAST_L2 | BCM_L3_INGRESS_MPLS_INTF_NAMESPACE | BCM_L3_INGRESS_VIP_ENABLED | BCM_L3_INGRESS_GLOBAL_ROUTE | BCM_L3_INGRESS_IPMC_BRIDGE_FALLBACK | \
                                                BCM_L3_INGRESS_ROUTE_DISABLE_IP4_UCAST | BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST | BCM_L3_INGRESS_ROUTE_DISABLE_IP6_UCAST | \
                                                BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST | BCM_L3_INGRESS_ROUTE_DISABLE_MPLS | BCM_L3_INGRESS_ROUTE_ENABLE_UNKNOWN)

/**
 * \brief This sets the bottom 10 bits of my_mac according to the vsi.
 * my_mac serves both as input and output
 */
#define L3_INTF_SET_MY_MAC_LSB(my_mac, vsi) \
    my_mac[4] |= (vsi >> SAL_UINT8_NOF_BITS) & L3_INTF_LOWEST_2_BITS_MASK; \
    my_mac[5] |= (vsi & L3_INTF_LOWEST_8_BITS_MASK);

/**
 * \brief This sets the High 38 bits of my_mac according to prefix.
 */
#define L3_INTF_SET_MY_MAC_MSB(my_mac, prefix) \
    my_mac[4] = prefix[4] | my_mac[4];         \
    my_mac[3] = prefix[3];                     \
    my_mac[2] = prefix[2];                     \
    my_mac[1] = prefix[1];                     \
    my_mac[0] = prefix[0];

/*
 * }
 */

/**
* \brief
*  Update the following fields in DBAL_TABLE_EEDB_RIF_BASIC: OUT_LIF FORWARDING_DOMAIN REMARK_PROFILE
* \par DIRECT INPUT:
*   \param [in] unit  -  The unit number.
*   \param [in] intf  -  The l3 interface structure.
* \par INDIRECT INPUT:
*   * \b *intf \n
*     See 'intf' in DIRECT INPUT above
* \par DIRECT OUTPUT:
*   shr_error_e - Non-zero in case of an error.
* \par INDIRECT OUTPUT:
*   * Write to HW OutRIF table
* \remark
*   * intf must be valid pointer
* \see
*   * dnx_l3_intf_create_egress()
*/
static shr_error_e
dnx_l3_intf_egress_outrif_info_set(
    int unit,
    bcm_l3_intf_t * intf)
{
    uint32 entry_handle_id;
    lif_table_mngr_outlif_info_t outlif_info;
    int stat_pp_profile = intf->stat_pp_profile;
    uint32 stat_id = intf->stat_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&outlif_info, 0, sizeof(lif_table_mngr_outlif_info_t));
    outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_RIF;
    if (_SHR_IS_FLAG_SET(intf->l3a_flags, BCM_L3_REPLACE))
    {
        outlif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_REPLACE;
    }
    outlif_info.global_lif = intf->l3a_intf_id;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_RIF_BASIC, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_SUPERSET_RESULT_TYPE);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, INST_SINGLE, intf->l3a_vid);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, INST_SINGLE,
                                 DNX_QOS_MAP_PROFILE_GET(intf->dscp_qos.qos_map_id));

    if (_SHR_IS_FLAG_SET(intf->l3a_flags2, BCM_L3_FLAGS2_EGRESS_STAT_ENABLE))
    {
        if (dnx_data_stat.stat_pp.etpp_reversed_stat_cmd_get(unit) == 1)
        {
            if (STAT_PP_PROFILE_ID_GET(stat_pp_profile) > STAT_PP_ETPP_SWITCH_PROFILE_MAX_PROFILE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "ETPP lif profiles range in A0 are 1-3");
            }
            STAT_PP_ETPP_SWITCH_CMD_SET(stat_id, stat_pp_profile);
        }
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE, stat_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE,
                                     STAT_PP_PROFILE_ID_GET(stat_pp_profile));
    }

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_local_outlif_and_set_hw
                    (unit, entry_handle_id, &intf->l3a_intf_id, &outlif_info));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Update the following fields in
 *          DBAL_TABLE_EG_VSI_INFO: FIELD_VSI MY_MAC
 *          MY_MAC_PREFIX
 *
 * \param [in] unit - The unit number.
 * \param [in] intf - The l3 interface structure.
 * \param [in] mymac_prefix_profile - Mac prefix to be saved.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * intf must be valid pointer
 * \see
 *   * SOURCE_ADDRESS_GET_MY_MAC_LSB
 *   * L3_INTF_MY_MAC_ETH_PREFIX
 */
static shr_error_e
dnx_l3_intf_egress_vsi_info_set(
    int unit,
    bcm_l3_intf_t * intf,
    int mymac_prefix_profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGR_VSI_INFO_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, intf->l3a_vid);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MY_MAC, INST_SINGLE,
                                 SOURCE_ADDRESS_GET_MY_MAC_LSB(intf->l3a_mac_addr));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MY_MAC_PREFIX, INST_SINGLE,
                                 L3_INTF_MY_MAC_ETH_PREFIX(mymac_prefix_profile));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the following fields in DBAL_TABLE_EG_VSI_INFO:
 *          FIELD_VSI MY_MAC MY_MAC_PREFIX
 *          Retrieve the mac prefix from the table. With it -
 *          we'll be able to restore the rest of the 38 bits
 *          Not called from bcm_dnx_l3_intf_get() api, since the
 *          data is already retreived from the ingress vsi table
 *
 * \param [in] unit - The unit number.
 * \param [in,out] intf - The l3 interface structure
 * \param [out] mymac_prefix_profile - Mac prefix to be saved.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * intf must be valid pointer
 * \see
 *   * L3_INTF_SET_MY_MAC_LSB
 */
static shr_error_e
dnx_l3_intf_egress_vsi_info_get(
    int unit,
    bcm_l3_intf_t * intf,
    uint32 *mymac_prefix_profile)
{
    uint32 entry_handle_id;
    uint32 mymac;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get VSI, mac_prefix fields
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGR_VSI_INFO_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, intf->l3a_vid);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MY_MAC, INST_SINGLE, &mymac));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MY_MAC_PREFIX, INST_SINGLE,
                                                        mymac_prefix_profile));
    L3_INTF_SET_MY_MAC_LSB(intf->l3a_mac_addr, mymac);
    /** revetring L3_INTF_MY_MAC_ETH_PREFIX performed in SET */
    L3_INTF_MY_MAC_ETH_PREFIX_RESTORE(*mymac_prefix_profile);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See description in include/bcm_int/dnx/l3/l3.h
 */
shr_error_e
dnx_l3_intf_egress_mymac_set(
    int unit,
    uint8 is_arp,
    bcm_mac_t l3a_mac_addr,
    int mymac_prefix_profile)
{
    dbal_tables_e table_id;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (is_arp)
    {
        table_id = dnx_data_l3.source_address.custom_sa_use_dual_homing_get(unit) ?
            DBAL_TABLE_MAC_SOURCE_ADDRESS_DUAL_HOMING : DBAL_TABLE_MAC_SOURCE_ADDRESS_FULL;
    }
    else
    {
        table_id = DBAL_TABLE_MAC_SOURCE_ADDRESS;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_ADDRESS_INDEX,
                               L3_INTF_MY_MAC_ETH_PREFIX(mymac_prefix_profile));
    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_ADDRESS, INST_SINGLE, l3a_mac_addr);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See description in include/bcm_int/dnx/l3/l3.h
 */
shr_error_e
dnx_l3_intf_egress_mymac_clear(
    int unit,
    uint8 is_arp,
    int mymac_prefix_profile)
{
    dbal_tables_e table_id;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (is_arp)
    {
        table_id = dnx_data_l3.source_address.custom_sa_use_dual_homing_get(unit) ?
            DBAL_TABLE_MAC_SOURCE_ADDRESS_DUAL_HOMING : DBAL_TABLE_MAC_SOURCE_ADDRESS_FULL;
    }
    else
    {
        table_id = DBAL_TABLE_MAC_SOURCE_ADDRESS;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_ADDRESS_INDEX,
                               L3_INTF_MY_MAC_ETH_PREFIX(mymac_prefix_profile));
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief - checks if intf was created, checks VSI table in the corresponding entry,
 * and validates that the my_mac prefix is valid by making sure the prefix profile
 * doesn't point to the default invalid profile (in index 0 in prefix template manager).
 */
shr_error_e
dnx_l3_check_if_ingress_intf_exists(
    int unit,
    bcm_if_t intf_id,
    uint8 *intf_exists)
{
    uint32 entry_handle_id, my_mac_prefix_profile = 0, gport = 0;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    if (BCM_L3_ITF_TYPE_IS_RIF(intf_id))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ING_VSI_INFO_DB, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, BCM_L3_ITF_VAL_GET(intf_id));
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_MY_MAC_PREFIX, INST_SINGLE, &my_mac_prefix_profile));

        /*
         * if my_mac_prefix_profile is zero then intf wasn't created
         */
        if (!my_mac_prefix_profile)
        {
            *intf_exists = 0;
        }
        else
        {
            *intf_exists = 1;
        }
    }
    else if (BCM_L3_ITF_TYPE_IS_LIF(intf_id))
    {
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, intf_id);
        /*
         * call dnx_algo_gpm_gport_to_hw_resources with flag dnx_algo_gpm_gport_to_hw_resources, if no error returned then LIF exists
         */
        SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                    (unit, gport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                                     &gport_hw_resources), _SHR_E_NOT_FOUND, _SHR_E_PARAM);
        *intf_exists = 1;
    }
    else
    {
        /*
         * check if intf type is legal
         */
        SHR_ERR_EXIT(_SHR_E_PARAM, "intf type is illegal");
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Used by intf get/delete to check NULL and existence of intf
 * \par DIRECT INPUT
 *  \param [in] unit - The unit number.
 *  \param [in] intf - The l3 interface structure to be verified.
 *  \param [out] intf_exists - indication if the interface was found
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error.
 * \par INDIRECT INPUT
 *  See *intf above \n
 */
static shr_error_e
dnx_l3_intf_get_delete_verify(
    int unit,
    bcm_l3_intf_t * intf,
    uint8 *intf_exists)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(intf, _SHR_E_PARAM, "l3_intf");
    if (intf->l3a_intf_id == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "l3a_intf_id = 0 is not supported!\n");
    }
    if (!BCM_L3_ITF_TYPE_IS_RIF(intf->l3a_intf_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "bcm_l3_intf_* APIs cannot be used for LIFs or FECs\n");
    }

    /*
     * check if intf exists
     */
    SHR_IF_ERR_EXIT(dnx_l3_check_if_ingress_intf_exists(unit, intf->l3a_intf_id, intf_exists));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Used by intf find to check NULL and existence of intf
 */
static shr_error_e
dnx_l3_intf_find_verify(
    int unit,
    bcm_l3_intf_t * intf)
{
    uint8 intf_exists;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify that the MAC address isn't all zeros
     */
    if (BCM_MAC_IS_ZERO(intf->l3a_mac_addr))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "MAC address can't be a 0 MAC address");
    }

    /*
     * Verify that the MAC address isn't a MC MAC address
     */
    if (BCM_MAC_IS_MCAST(intf->l3a_mac_addr))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "MAC address can't be a multicast MAC address, LSB of the highest byte must be turned off");
    }

    /*
     * check if intf exists
     */
    SHR_IF_ERR_EXIT(dnx_l3_check_if_ingress_intf_exists(unit, intf->l3a_intf_id, &intf_exists));
    if (!intf_exists)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "intf not found");
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Used by intf find vlan to check NULL and existence of intf
 */
static shr_error_e
dnx_l3_intf_find_vlan_verify(
    int unit,
    bcm_l3_intf_t * intf)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Check the provided VLAN ID is in range
     */
    BCM_DNX_VLAN_CHK_ID_NONE(unit, intf->l3a_vid, _SHR_E_PARAM);
    BCM_DNX_VLAN_CHK_ID(unit, intf->l3a_vid);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Used by ingress_get api to check NULL and existence of intf, and also validates intf type
 * \par DIRECT INPUT
 *  \param [in] unit - The unit number.
 *  \param [in] intf - The corresponding L3 interface id
 *  \param [in] ing_intf - The ingress interface structure to be verified.
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error.
 * \par INDIRECT INPUT
 *  See *ing_intf above \n
 */
static shr_error_e
dnx_l3_ingress_get_verify(
    int unit,
    bcm_if_t intf,
    bcm_l3_ingress_t * ing_intf)
{
    uint8 intf_exists;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ing_intf, _SHR_E_PARAM, "ing_intf");

    /*
     * check if intf exists
     */
    SHR_IF_ERR_EXIT(dnx_l3_check_if_ingress_intf_exists(unit, intf, &intf_exists));
    if (!intf_exists)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "intf not found");
    }

    /*
     * check if intf type is legal
     */
    if (!BCM_L3_ITF_TYPE_IS_LIF(intf) && !BCM_L3_ITF_TYPE_IS_RIF(intf))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "intf type is illegal");
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Used by ingress_create api to check NULL and existence of intf, VRF validity and WITH_ID flag
 * \par DIRECT INPUT
 *  \param [in] unit - The unit number.
 *  \param [in] ing_intf - The ingress interface structure to be verified.
 *  \param [in] intf_id - The corresponding L3 interface id
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error.
 * \par INDIRECT INPUT
 *  See *ing_intf above \n
 */
static shr_error_e
dnx_l3_ingress_create_verify(
    int unit,
    bcm_l3_ingress_t * ing_intf,
    bcm_if_t * intf_id)
{
    int qos_profile;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ing_intf, _SHR_E_PARAM, "ing_intf");
    SHR_NULL_CHECK(intf_id, _SHR_E_PARAM, "intf_id");

    /*
     * Verify the flags supplied are supported.
     */
    if (_SHR_IS_FLAG_SET(ing_intf->flags, ~L3_INTF_INGRESS_SUPPORTED_BCM_L3_FLAGS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "One or more of the used flags 0x%x aren't supported (supported flags are 0x%x).",
                     ing_intf->flags, L3_INTF_INGRESS_SUPPORTED_BCM_L3_FLAGS);
    }

    /*
     * check if intf type is legal
     */
    if (!BCM_L3_ITF_TYPE_IS_LIF(*intf_id) && !BCM_L3_ITF_TYPE_IS_RIF(*intf_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "intf type is illegal");
    }

    /*
     * check WITH_ID flag is set
     */
    if ((ing_intf->flags & BCM_L3_INGRESS_WITH_ID) == 0)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "flag BCM_L3_INGRESS_WITH_ID must be set");
    }

    /*
     * Verify the the VRF is in range.
     */
    if (ing_intf->vrf >= dnx_data_l3.vrf.nof_vrf_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "VRF %d is out of range, should be lower then %d.", ing_intf->vrf,
                     dnx_data_l3.vrf.nof_vrf_get(unit));
    }
    /*
     * Verify qos map id is in range
     */
    if (ing_intf->qos_map_id != 0 && (!DNX_QOS_MAP_IS_INGRESS(ing_intf->qos_map_id) ||
                                      !DNX_QOS_MAP_IS_PHB(ing_intf->qos_map_id)
                                      || !DNX_QOS_MAP_IS_REMARK(ing_intf->qos_map_id)))
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "qos_map_id 0x%x error", ing_intf->qos_map_id);
    }
    qos_profile = DNX_QOS_MAP_PROFILE_GET(ing_intf->qos_map_id);
    if ((BCM_L3_ITF_TYPE_IS_RIF(*intf_id) && qos_profile > DNX_QOS_L3_MAX_PROFILE) ||
        (BCM_L3_ITF_TYPE_IS_LIF(*intf_id) && qos_profile > DNX_QOS_ING_LIF_MAX_PROFILE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Qos map id %d is out of range", qos_profile);
    }
        /** Verify BCM_L3_INGRESS_IPMC_BRIDGE_FALLBACK flag validity */
    if (_SHR_IS_FLAG_SET(ing_intf->flags, BCM_L3_INGRESS_IPMC_BRIDGE_FALLBACK))
    {
        if (!BCM_L3_ITF_TYPE_IS_RIF(*intf_id))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "BCM_L3_INGRESS_IPMC_BRIDGE_FALLBACK flag is only supported for interfaces of type RIF.\n");
        }
    }

    if (BCM_L3_ITF_TYPE_IS_LIF(*intf_id) && (ing_intf->oam_default_profile > 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Configuring OAM default profile is only supported for interfaces of type RIF.\n");
    }
    /*
     * Verify the oam default profile value
     */
    if (ing_intf->oam_default_profile > OAM_DEFAULT_PROFILE_MAX_VALUE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "OAM Default Profile out of range, should not be higher than %d", OAM_DEFAULT_PROFILE_MAX_VALUE);
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Used by bcm_dnx_l3_intf_create() to verify the route entry add input
 * \par DIRECT INPUT
 *  \param [in] unit - The unit number.
 *  \param [in] intf - The l3 interface structure to be verified.
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error.
 * \par INDIRECT INPUT
 *  See *intf above \n
 */

static shr_error_e
dnx_l3_intf_create_verify(
    int unit,
    bcm_l3_intf_t * intf)
{

    uint8 intf_exists;
    int replace;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(intf, _SHR_E_PARAM, "l3_intf");
    replace = _SHR_IS_FLAG_SET(intf->l3a_flags, BCM_L3_REPLACE);
    /*
     * Verify the the VID value
     */
    if ((intf->l3a_vid == 0) || (intf->l3a_vid >= dnx_data_l3.rif.nof_rifs_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid l3a_vid(0x%x)\n", intf->l3a_vid);
    }

    /*
     * Verify the the VRF is in range.
     */
    if (intf->l3a_vrf >= dnx_data_l3.vrf.nof_vrf_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "VRF %d is out of range, should be lower than %d.", intf->l3a_vrf,
                     dnx_data_l3.vrf.nof_vrf_get(unit));
    }

    /*
     * Verify that the MAC address isn't all zeros
     */
    if (BCM_MAC_IS_ZERO(intf->l3a_mac_addr))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "MAC address can't be a 0 MAC address");
    }

    /*
     * Verify that the TTL is valid
     */
    if (!BCM_TTL_VALID(intf->l3a_ttl))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid TTL set");
    }

    /*
     * Verify that the MAC address isn't a MC MAC address
     */
    if (BCM_MAC_IS_MCAST(intf->l3a_mac_addr))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "MAC address can't be a multicast MAC address, LSB of the highest byte must be turned off");
    }

    /*
     * check input flags
     */
    if ((intf->l3a_flags & BCM_L3_WITH_ID) && (intf->l3a_intf_id != intf->l3a_vid))
    {
        /** l3a_intf_id must be equal to l3a_vid in case BCM_L3_WITH_ID is set */
        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_L3_WITH_ID is set but intf->l3a_intf_id != intf->l3a_vid\n");
    }
    else
    {
        intf->l3a_intf_id = intf->l3a_vid;
    }

    if (intf->dscp_qos.flags & BCM_L3_INTF_QOS_DSCP_REMARK)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "JR2 does not support BCM_L3_INTF_QOS_DSCP_REMARK, use qos model instead!\n");
    }

    if (replace && !_SHR_IS_FLAG_SET(intf->l3a_flags, BCM_L3_WITH_ID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_L3_REPLACE is supported only with BCM_L3_WITH_ID\n");
    }

    /*
     * check if intf exists - ingress side.
     * If the REPLACE flag has been added, an error will be returned if the interface does not exist.
     * If the REPLACE flag is missing, an error will be returned if the interface exists
     */

    SHR_IF_ERR_EXIT(algo_l3_db.rif_alloc.rif_res_manager.is_allocated(unit, intf->l3a_intf_id, &intf_exists));

    if (!replace && intf_exists == 1)
    {
        SHR_ERR_EXIT(_SHR_E_EXISTS, "bcm_l3_intf already created - rif_id = %d.", intf->l3a_intf_id);
    }
    else if (replace && intf_exists == 0)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "bcm_l3_intf does not exist but BCM_L3_REPLACE flag is provided - rif_id = %d.",
                     intf->l3a_intf_id);
    }

    /*
     * If a new egress rif will be allocated, then make sure that there's enough room in the GLEM.
     */
    if (!replace)
    {
        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_count_verify(unit, DNX_ALGO_LIF_EGRESS));
    }

    /*
     * Verify qos model for both ingress and egress RIF.
     */
    if ((intf->ingress_qos_model.ingress_remark > bcmQosIngressModelUniform) ||
        (intf->ingress_qos_model.ingress_phb > bcmQosIngressModelUniform) ||
        (intf->ingress_qos_model.ingress_ttl > bcmQosIngressModelUniform) ||
        (intf->ingress_qos_model.ingress_ecn > bcmQosIngressEcnModelEligible))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "one or more Ingress qos model remark(%d), phb(%d), ttl(%d) ECN(%d) are out of range",
                     intf->ingress_qos_model.ingress_remark, intf->ingress_qos_model.ingress_phb,
                     intf->ingress_qos_model.ingress_ttl, intf->ingress_qos_model.ingress_ecn);
    }

    if ((intf->egress_qos_model.egress_qos != bcmQosEgressModelUniform) ||
        (intf->egress_qos_model.egress_ttl != bcmQosEgressModelUniform))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Available egress qos model for remark(%d), ttl(%d) is Uniform currently.",
                     intf->egress_qos_model.egress_qos, intf->egress_qos_model.egress_ttl);
    }

    /*
     * Statistics check
     */
    if (_SHR_IS_FLAG_SET(intf->l3a_flags2, BCM_L3_FLAGS2_EGRESS_STAT_ENABLE))
    {
        if (intf->stat_pp_profile == STAT_PP_PROFILE_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Flag BCM_L3_FLAGS2_EGRESS_STAT_ENABLE is set, but stat_pp_profile is invalid");
        }
        SHR_IF_ERR_EXIT(dnx_stat_pp_crps_verify
                        (unit, BCM_CORE_ALL, intf->stat_id, intf->stat_pp_profile,
                         bcmStatCounterInterfaceEgressTransmitPp));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Add an entry into the MyMac table or increase the reference counter by on if the entry is
 *  already  exists in the template manager..
 * \par DIRECT INPUT
 *  \param [in] unit - The unit number.
 *  \param [in] intf - The l3 interface structure.
 *  \param [in] my_mac_prefix_index - pointer to the received MyMac prefix index.
 *  \param [in] old_mymac_prefix_profile - the profile that needs to be freed in case of BCM_L3_REPLACE
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error.
 * \par INDIRECT OUTPUT
 *   See *my_mac_prefix_index above \n
 * \par INDIRECT INPUT
 *  See *intf above \n
 */
static shr_error_e
dnx_l3_intf_add_my_mac(
    int unit,
    bcm_l3_intf_t * intf,
    uint32 *my_mac_prefix_index,
    int old_mymac_prefix_profile)
{
    uint8 first_reference = 0;
    uint32 entry_handle_id;
    bcm_mac_t prefix;
    uint8 is_last;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memcpy(prefix, intf->l3a_mac_addr, L3_MAC_ADDR_SIZE_IN_BYTES);

    /** Clear LSB bits from the prefix */
    prefix[5] = 0;
    prefix[4] &= ~SOURCE_ADDRESS_MY_MAC_SECOND_BYTE_MASK;

    if (old_mymac_prefix_profile)
    {
        SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.ingress_mymac_prefix_table.free_single
                        (unit, old_mymac_prefix_profile, &is_last));
    }
    SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.ingress_mymac_prefix_table.allocate_single
                    (unit, 0, prefix, NULL, (int *) my_mac_prefix_index, &first_reference));

    /*
     * In case this is the first time this profile is allocated, enter the new prefix into the prefix table.
     */
    if (first_reference)
    {

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L3_MY_MAC_DA_PREFIXES, &entry_handle_id));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MY_MAC_INDEX, *my_mac_prefix_index);

        dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_L2_MAC, INST_SINGLE, prefix);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See description in include/bcm_int/dnx/l3/l3.h
 */
shr_error_e
dnx_l3_intf_egress_mymac_prefix_allocate(
    int unit,
    uint8 is_arp,
    bcm_mac_t l3a_mac_addr,
    int *mymac_prefix,
    uint8 *is_first_mymac_reference)
{
    source_address_entry_t source_address_entry;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&source_address_entry, 0, sizeof(source_address_entry));

    if (is_arp)
    {
        source_address_entry.address_type = dnx_data_l3.source_address.custom_sa_use_dual_homing_get(unit) ?
            source_address_type_mac_dual_homing : source_address_type_full_mac;
    }
    else
    {
        source_address_entry.address_type = source_address_type_mac;
    }

    sal_memcpy(source_address_entry.address.mac_address, l3a_mac_addr, sizeof(bcm_mac_t));

    if (source_address_entry.address_type != source_address_type_full_mac)
    {
        /** mask mac_address 10 LSB, only 38 MSB are needed */
        L3_INTF_MAC_ADDR_MASK_10LSB(source_address_entry.address.mac_address);
    }

    SHR_IF_ERR_EXIT(algo_l3_db.source_address_table_allocation.egress_source_address_table.allocate_single
                    (unit, 0, &source_address_entry, NULL, mymac_prefix, is_first_mymac_reference));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See description in include/bcm_int/dnx/l3/l3.h
 */
shr_error_e
dnx_l3_intf_egress_mymac_prefix_exchange(
    int unit,
    uint8 is_arp,
    bcm_mac_t l3a_mac_addr,
    int old_mymac_prefix,
    int *new_mymac_prefix,
    uint8 *is_first_mymac_reference,
    uint8 *is_last_mymac_reference)
{
    source_address_entry_t source_address_entry;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&source_address_entry, 0, sizeof(source_address_entry));

    if (is_arp)
    {
        source_address_entry.address_type = dnx_data_l3.source_address.custom_sa_use_dual_homing_get(unit) ?
            source_address_type_mac_dual_homing : source_address_type_full_mac;
    }
    else
    {
        source_address_entry.address_type = source_address_type_mac;
    }

    sal_memcpy(source_address_entry.address.mac_address, l3a_mac_addr, sizeof(bcm_mac_t));

    if (source_address_entry.address_type != source_address_type_full_mac)
    {
        /** mask mac_address 10 LSB, only 38 MSB are needed */
        L3_INTF_MAC_ADDR_MASK_10LSB(source_address_entry.address.mac_address);
    }

    SHR_IF_ERR_EXIT(algo_l3_db.source_address_table_allocation.egress_source_address_table.exchange
                    (unit, 0, &source_address_entry, old_mymac_prefix, NULL, new_mymac_prefix,
                     is_first_mymac_reference, is_last_mymac_reference));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See description in include/bcm_int/dnx/l3/l3.h
 */
shr_error_e
dnx_l3_intf_egress_mymac_prefix_free(
    int unit,
    int src_mac_prefix,
    uint8 *last_ref)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_l3_db.source_address_table_allocation.egress_source_address_table.free_single
                    (unit, src_mac_prefix, last_ref));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Write L3 interface (egress part) to HW tables
* \par DIRECT INPUT:
*  \param [in] unit - The Unit number.
*  \param [in] intf - The L3 interface structure to be written.
* \par INDIRECT INPUT:
*   * \b *intf \n
*     See 'intf' in DIRECT INPUT above
* \par DIRECT OUTPUT:
*   shr_error_e - Non-zero in case of an error.
* \par INDIRECT OUTPUT:
*   * Following tables will be updated:
*   * DBAL_TABLE_EG_VSI_INFO,
*   * DBAL_TABLE_EEDB_RIF_BASIC,
*   * DBAL_TABLE_MAC_SOURCE_ADDRESS (if this is the first allocation)
* \remark
*   * intf must be valid pointer
* \see
*   * None
*/
static shr_error_e
dnx_l3_intf_eth_rif_create_egress(
    int unit,
    bcm_l3_intf_t * intf)
{
    int mymac_prefix_profile, old_mymac_prefix;
    uint8 is_first_mymac_reference = 0, is_last_mymac_reference = 0;
    erpp_out_lif_profile_info_t erpp_out_lif_profile_info;
    bcm_l3_intf_t old_intf;
    bcm_gport_t intf_gport;
    int old_erpp_out_lif_profile, new_erpp_out_lif_profile, rv;

    SHR_FUNC_INIT_VARS(unit);

    bcm_l3_intf_t_init(&old_intf);
    erpp_out_lif_profile_info_t_init(unit, &erpp_out_lif_profile_info);
    /*
     * SW allocations
     */
    if (_SHR_IS_FLAG_SET(intf->l3a_flags, BCM_L3_REPLACE))
    {
        old_intf.l3a_vid = intf->l3a_vid;
        SHR_IF_ERR_EXIT(dnx_l3_intf_egress_vsi_info_get(unit, &old_intf, (uint32 *) &old_mymac_prefix));
        /** is_dual_homing parameter is assumed not to be changed by REPLACE, otherwise internal logic must be updated */
        SHR_IF_ERR_EXIT(dnx_l3_intf_egress_mymac_prefix_exchange
                        (unit, FALSE, intf->l3a_mac_addr, old_mymac_prefix, &mymac_prefix_profile,
                         &is_first_mymac_reference, &is_last_mymac_reference));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_l3_intf_egress_mymac_prefix_allocate
                        (unit, FALSE, intf->l3a_mac_addr, &mymac_prefix_profile, &is_first_mymac_reference));
    }

    /*
     * writing to HW tables
     */
    if (is_first_mymac_reference)
    {
        /** Set prefix MSB MyMAC information in case it is the first time. */
        SHR_IF_ERR_EXIT(dnx_l3_intf_egress_mymac_set(unit, FALSE, intf->l3a_mac_addr, mymac_prefix_profile));
    }

    /** Add entry to RIF table */
    SHR_IF_ERR_EXIT(dnx_l3_intf_egress_outrif_info_set(unit, intf));

    /** Add entry to EG_VSI table */
    SHR_IF_ERR_EXIT(dnx_l3_intf_egress_vsi_info_set(unit, intf, mymac_prefix_profile));
    /*
     * Clear unused profiles after entry set/updated
     */
    if (is_last_mymac_reference)
    {
        SHR_IF_ERR_EXIT(dnx_l3_intf_egress_mymac_clear(unit, FALSE, old_mymac_prefix));
    }
    /** Get the old outlif profile from GLEM*/
    BCM_GPORT_TUNNEL_ID_SET(intf_gport, intf->l3a_intf_id);
    rv = dnx_out_lif_profile_get(unit, intf_gport, &old_erpp_out_lif_profile);
    erpp_out_lif_profile_info.ttl_scope_threshold = intf->l3a_ttl;

    if (rv == _SHR_E_NOT_FOUND)
    {
        SHR_EXIT();
    }
    else if (rv != _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Couldn't retrieve the outlif profile");
    }

    /*
     * with external kbp OutLif profile not in use, all bits allocated for EEDB pointer
     */
    if ((dnx_data_lif.out_lif.outlif_profile_width_get(unit)) != 0)
    {
        /** Exchange with a new outlif profile, if the new profile is different, than the old one - write it to GLEM*/
        SHR_IF_ERR_EXIT(dnx_erpp_out_lif_profile_exchange
                        (unit, &erpp_out_lif_profile_info, old_erpp_out_lif_profile, &new_erpp_out_lif_profile));
        /**Update the GLEM, only if a new ERPP profile is allocated*/
        if (old_erpp_out_lif_profile != new_erpp_out_lif_profile)
        {
            SHR_IF_ERR_EXIT(dnx_out_lif_profile_set(unit, intf_gport, new_erpp_out_lif_profile));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  set/update VSI entry with relevant information
*   \param [in] unit     - The Unit number.
*   \param [in] intf         - The L3 interface structure to be written.
*   \param [in] table_result_type         -  entry result type
*   \param [in] profile_to_write         -  VSI profile
*   \param [in] propag_profile         -  propagation profile to be used
*   \param [in] old_mymac_prefix_profile - the old_mymac_prefix_profile that needs to be freed in case of BCM_L3_REPLACE
*   \param [in] ecn_mapping_profile - ecn mapping profile
* \return
*   shr_error_e
*/
static shr_error_e
dnx_l3_intf_ingress_vsi_hw_set(
    int unit,
    bcm_l3_intf_t * intf,
    uint32 table_result_type,
    uint32 profile_to_write,
    uint32 propag_profile,
    int old_mymac_prefix_profile,
    uint32 ecn_mapping_profile)
{
    uint32 entry_handle_id, my_mac_prefix_index;
    uint8 stp_topology_id;
    bcm_stg_t stg_defl;
    uint8 is_vsi_allocated;
    int is_replace;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_dnx_stg_default_get(unit, &stg_defl));

    stp_topology_id = BCM_DNX_STG_TO_TOPOLOGY_ID_GET(stg_defl);
    is_replace = _SHR_IS_FLAG_SET(intf->l3a_flags, BCM_L3_REPLACE);
    SHR_IF_ERR_EXIT(vlan_db.vsi.is_allocated(unit, intf->l3a_intf_id, &is_vsi_allocated));
    /*
     * set new VSI profile (only VRF and LSB bits of VSI_PROFILE may have been changed)
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ING_VSI_INFO_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, intf->l3a_vid);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, table_result_type);
    if (!is_replace && !is_vsi_allocated)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TOPOLOGY_ID, INST_SINGLE, stp_topology_id);
        if (table_result_type == DBAL_RESULT_TYPE_ING_VSI_INFO_DB_VSI_ENTRY_BASIC)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MC_ID, INST_SINGLE, intf->l3a_vid);
        }
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRF, INST_SINGLE, intf->l3a_vrf);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI_PROFILE, INST_SINGLE, profile_to_write);

    /*
     * MYMAC HW config
     */
    SHR_IF_ERR_EXIT(dnx_l3_intf_add_my_mac(unit, intf, &my_mac_prefix_index, old_mymac_prefix_profile));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MY_MAC_PREFIX, INST_SINGLE, my_mac_prefix_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MY_MAC, INST_SINGLE,
                                 SOURCE_ADDRESS_GET_MY_MAC_LSB(intf->l3a_mac_addr));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROF, INST_SINGLE, propag_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECN_MAPPING_PROFILE, INST_SINGLE,
                                 ecn_mapping_profile);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *  Write L3 interface (ingress part) to HW tables
 * \par DIRECT INPUT
 *  \param [in] unit - The Unit number.
 *  \param [in] intf - The L3 interface structure to be written.
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error.
 * \par INDIRECT INPUT
 *  See *intf above \n
 */
static int
dnx_l3_intf_eth_rif_create_ingress(
    int unit,
    bcm_l3_intf_t * intf)
{
    uint32 table_result_type, entry_handle_id, old_vsi_profile;
    int propag_profile, org_propag_profile;
    uint32 ecn_mapping_profile;
    dnx_qos_propagation_type_e phb_propag_type, remark_propag_type, ecn_propag_type, ttl_propag_type;
    in_lif_profile_info_t in_lif_profile_info;
    int old_in_lif_profile, new_in_lif_profile;
    uint32 old_mymac_prefix_profile;
    uint8 first_propagation_reference, last_propagation_reference;
    uint8 is_vsi_allocated;
    int is_replace;
    int vsi_value;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    vsi_value = intf->l3a_intf_id;
    /*
     * init vars to default values
     */
    new_in_lif_profile = 0;
    old_mymac_prefix_profile = 0;
    is_vsi_allocated = 0;
    SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));

    is_replace = _SHR_IS_FLAG_SET(intf->l3a_flags, BCM_L3_REPLACE);
    SHR_IF_ERR_EXIT(vlan_db.vsi.is_allocated(unit, vsi_value, &is_vsi_allocated));
    /*
     *  In case of Replace or VSI was already allocated, take it from DBAL
     *  In case VSI was not allocated, configure default to VSI
     */
    org_propag_profile = DNX_QOS_INGRESS_DEFAULT_PROPAGATION_PROFILE;
    if (is_replace || is_vsi_allocated)
    {
        /** table_result_type is taken from VSI table */
        /*
         * get the current VSI-PROFILE from VSI-TABLE
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ING_VSI_INFO_DB, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, BCM_L3_ITF_VAL_GET(intf->l3a_intf_id));
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &table_result_type));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_VSI_PROFILE, INST_SINGLE, &old_vsi_profile));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_MY_MAC_PREFIX, INST_SINGLE, &old_mymac_prefix_profile));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROF, INST_SINGLE,
                         (uint32 *) &org_propag_profile));
        old_in_lif_profile = DNX_IN_LIF_PROFILE_DEFAULT;
        if (is_replace)
        {
            /*
             * old IN-LIF-PROFILE is the LSB bits of old VSI-PROFILE
             */
            old_in_lif_profile = CALCULATE_VSI_PROFILE_FROM_ETH_RIF_PROFILE(VSI_MSB_PREFIX, old_vsi_profile);
        }
    }
    else
    {
        old_in_lif_profile = DNX_IN_LIF_PROFILE_DEFAULT;
        table_result_type = DBAL_RESULT_TYPE_ING_VSI_INFO_DB_VSI_ENTRY_BASIC;
    }

    /*
     * SW allocations
     */
    ecn_mapping_profile = DNX_QOS_ECN_MAPPING_PROFILE_DEFAULT;
    if (intf->ingress_qos_model.ingress_ecn == bcmQosIngressEcnModelEligible)
    {
        ecn_mapping_profile = DNX_QOS_ECN_MAPPING_PROFILE_ELIGIBLE;
    }
    phb_propag_type = remark_propag_type = ecn_propag_type = ttl_propag_type = DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE;
    /** QoS propagation profile config */

    if (intf->ingress_qos_model.ingress_phb != bcmQosIngressModelInvalid)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_qos_model_to_propag_type
                        (unit, intf->ingress_qos_model.ingress_phb, &phb_propag_type));
    }

    if (intf->ingress_qos_model.ingress_remark != bcmQosIngressModelInvalid)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_qos_model_to_propag_type
                        (unit, intf->ingress_qos_model.ingress_remark, &remark_propag_type));
    }

    /** Set qos inheritance mode for TTL*/
    if (intf->ingress_qos_model.ingress_ttl != bcmQosIngressModelInvalid)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_qos_model_to_propag_type
                        (unit, intf->ingress_qos_model.ingress_ttl, &ttl_propag_type));
    }

    SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_alloc
                    (unit, phb_propag_type, remark_propag_type, ecn_propag_type,
                     ttl_propag_type, org_propag_profile, &propag_profile,
                     &first_propagation_reference, &last_propagation_reference));

    /** change only LSB bits which are RIF/IN-LIF profile */
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange
                    (unit, &in_lif_profile_info, old_in_lif_profile, &new_in_lif_profile, ETH_RIF,
                     DBAL_TABLE_ING_VSI_INFO_DB));
    new_in_lif_profile = CALCULATE_VSI_PROFILE_FROM_ETH_RIF_PROFILE(VSI_MSB_PREFIX, new_in_lif_profile);

    /*
     * HW allocations
     */

    if (first_propagation_reference)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_hw_set
                        (unit, propag_profile, phb_propag_type, remark_propag_type, ecn_propag_type, ttl_propag_type));
    }

    /** save VSI info */
    SHR_IF_ERR_EXIT(dnx_l3_intf_ingress_vsi_hw_set
                    (unit, intf, table_result_type, new_in_lif_profile, propag_profile, old_mymac_prefix_profile,
                     ecn_mapping_profile));

    if (last_propagation_reference)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_hw_clear(unit, org_propag_profile));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Create an ETH-RIF object and set its main properties \n
 * ETH-RIF is the L3 logical interface which derived from VSI. \n
 * For more information about ETH-RIF object, see the PG PP document.
 *
 * The API configures both Ingress & Egress properties of ETH-RIF.
 *
 *
 * \param [in] unit -
 *   Relevant unit.
 * \param [in,out] intf - The incoming intf structure \n
 *   [in,out] intf.l3a_intf_id - ID of ETH-RIF, must be set (must be WITH-ID) \n
 *   [in] intf.l3a_flags - BCM_L3_XXX flags \n
 *        BCM_L3_WITH_ID - WITH-ID \n
 *        BCM_L3_REPLACE - Replace functionality, must be called when object already created \n
 *        BCM_L3_FLAGS2_EGRESS_STAT_ENABLE - Enable egress statistics. When
 *        on, a result type with statistics config will be chosen \n
 *   [in] intf.l3a_mac_addr - MAC address of ETH-RIF \n
 *   [in] intf.l3a_vrf - VRF \n
 *   [in] intf.l3a_vid - VSI (must be set same value as l3a_intf_id) \n
 *   [in] intf.dscp_qos.qos_map_id - QOS Remark profile (egress)
 * \return
 *   \retval Negative in case of an error. See shr_error_e, for example: VSI already created or input VSI range value is incorrect
 *   \retval Zero in case of NO ERROR
 * \remark
 *
 * The supported result-types in Egress ETH-RIF table:
 *
 * result-type    | params decision     | description
 * ---------------| --------------------|------------
 * ETPS_RIF_BASIC | default             |Default format
 * ETPS_RIF_W_STATS     | (not supported yet) |In case stats required
 *
 * Note: VSI table is used for both L2 & L3 applications.
 * ETH-RIF and VSI are sharing the same tables in HW (ING and EG VSI).
 * For ING VSI the sharing is more important since the result-type may vary depending on the required fields.
 * On creation/replace, ETH-RIF doesn't change result-type for ING_VSI.
 * In case ING_VSI wasn't created before then result-type is initialized to VSI_ENTRY_BASIC.
 *
 *
 * ETH-RIF ID 0 is invalid value. Its range value is 1-max_rif_id (soc-property). \n
 * API set only subset of the ETH-RIF configuration. Its main companion API is bcm_dnx_l3_ingress_create.
 * \see
 *   * bcm_dnx_l3_ingress_create
 */
int
bcm_dnx_l3_intf_create(
    int unit,
    bcm_l3_intf_t * intf)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l3_intf_create_verify(unit, intf));

    /*
     * Write to HW ingress RIF tables
     */
    SHR_IF_ERR_EXIT(dnx_l3_intf_eth_rif_create_ingress(unit, intf));

    /*
     * Write to HW egress RIF tables
     */
    SHR_IF_ERR_EXIT(dnx_l3_intf_eth_rif_create_egress(unit, intf));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Creates L3 Ingress Interface.
 * \par DIRECT INPUT
 *  \param [in] unit - The unit number.
 *  \param [in] ing_intf - the ingress interface.
 *                       * urpf_mode - configure urpf mode
 *                       * flags - specify flags
 *                       * oam_default_profile - configure OAM default profile for interfaces of type RIF.
 *                       * qos_map_id - configure qos_map_id for the interface
 *                       * vrf - update the interface with VRF value
 *  \param [in] intf_id - allocated interface ID.
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error.
 * \par INDIRECT INPUT
 *  See *ing_intf above
 *  See *intf_id above \n
 */
int
bcm_dnx_l3_ingress_create(
    int unit,
    bcm_l3_ingress_t * ing_intf,
    bcm_if_t * intf_id)
{
    int new_in_lif_profile;
    uint32 old_vsi_profile, old_in_lif_profile, new_vsi_profile, entry_handle_id;
    in_lif_profile_info_t in_lif_profile_info;
    in_lif_intf_type_e intf_kind;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint32 gport;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l3_ingress_create_verify(unit, ing_intf, intf_id));

    SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));
    new_vsi_profile = 0;
    sal_memset(&gport_hw_resources, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));

    /*
     * check if intf_id is ETH-RIF or TUNNEL and get the old IN-LIF-PROFILE
     */
    if (BCM_L3_ITF_TYPE_IS_RIF(*intf_id))
    {
        uint32 my_mac_prefix_profile;
        /*
         * get the current VSI-PROFILE from VSI-TABLE
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ING_VSI_INFO_DB, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, BCM_L3_ITF_VAL_GET(*intf_id));
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_VSI_PROFILE, INST_SINGLE,
                                                            &old_vsi_profile));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                            &(gport_hw_resources.inlif_dbal_result_type)));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_MY_MAC_PREFIX, INST_SINGLE, &my_mac_prefix_profile));
        /*
         * if my_mac_prefix_profile is zero then intf wasn't created
         */
        if (!my_mac_prefix_profile)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Interface with ID %d was not found\n", BCM_L3_ITF_VAL_GET(*intf_id));
        }
        gport_hw_resources.inlif_dbal_table_id = DBAL_TABLE_ING_VSI_INFO_DB;
        in_lif_profile_info.ingress_fields.oam_default_profile = ing_intf->oam_default_profile;

        /*
         * old IN-LIF-PROFILE is the LSB bits of old VSI-PROFILE
         */
        old_in_lif_profile = CALCULATE_VSI_PROFILE_FROM_ETH_RIF_PROFILE(VSI_MSB_PREFIX, old_vsi_profile);
        intf_kind = ETH_RIF;

        /*
         * Update the forwarding mode in in_lif_profile_info to IP if L3_MCAST_L2 flag is provided
         * Update the forwarding mode in in_lif_profile_info to Bridge if L3_MCAST_L2 flag is missing
         */
        in_lif_profile_info.ingress_fields.l2_v4_mc_fwd_type =
            (_SHR_IS_FLAG_SET(ing_intf->flags, BCM_L3_INGRESS_L3_MCAST_L2)) ?
            DBAL_ENUM_FVAL_L2_V4_MC_FWD_TYPE_IPV4MC : DBAL_ENUM_FVAL_L2_V4_MC_FWD_TYPE_V4BRIDGE;
        in_lif_profile_info.ingress_fields.l2_v6_mc_fwd_type =
            (_SHR_IS_FLAG_SET(ing_intf->flags, BCM_L3_INGRESS_IP6_L3_MCAST_L2)) ?
            DBAL_ENUM_FVAL_L2_V6_MC_FWD_TYPE_IPV6MC : DBAL_ENUM_FVAL_L2_V6_MC_FWD_TYPE_V6BRIDGE;

        if (ing_intf->hash_layers_disable != 0 && dnx_data_switch.load_balancing.hashing_selection_per_layer_get(unit))
        {
            /** Check the flags */
            if (_SHR_IS_FLAG_SET(ing_intf->hash_layers_disable, ~LB_ENABLERS_DISABLE_SUPPORTED_FLAGS))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal LB hash layer flags are used: %d.", ing_intf->hash_layers_disable);
            }
            in_lif_profile_info.ingress_fields.lb_per_lif_flags = ing_intf->hash_layers_disable;
        }
    }
    /*
     * else if intf_id is TUNNEL
     */
    else if (BCM_L3_ITF_TYPE_IS_LIF(*intf_id))
    {
        /*
         * get the current IN-LIF-PROFILE from LIF-TABLE, if no error returned then LIF exists
         */
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, *intf_id);
        SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                    (unit, gport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                                     &gport_hw_resources), _SHR_E_NOT_FOUND, _SHR_E_PARAM);
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     gport_hw_resources.inlif_dbal_result_type);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, &old_in_lif_profile));
        if (old_in_lif_profile != 0)
        {
            /*
             * Tunnel APIs might allocate the CS profile, so retrieve the profile info in case non-default profile is set
             */
            SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, old_in_lif_profile, &in_lif_profile_info, LIF));
        }
        intf_kind = LIF;
    }
    /*
     * invalid intf_id
     */
    else
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "invalid intf_id, INTF TYPE should be LIF or RIF\n");
    }

    in_lif_profile_info.ingress_fields.l3_ingress_flags = ing_intf->flags;
    in_lif_profile_info.ingress_fields.urpf_mode = ing_intf->urpf_mode;

    /*
     * exchange the old in_lif_profile with a new one
     */
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange
                    (unit, &in_lif_profile_info, old_in_lif_profile, &new_in_lif_profile, intf_kind,
                     gport_hw_resources.inlif_dbal_table_id));

    /*
     * check if intf_id is ETH-RIF or TUNNEL and update VSI or LIF table
     */
    if (BCM_L3_ITF_TYPE_IS_RIF(*intf_id))
    {
        uint32 qos_profile = DNX_QOS_MAP_PROFILE_GET(ing_intf->qos_map_id);
        /*
         * change only 7 LSB bits which are RIF profile
         */
        new_vsi_profile = CALCULATE_VSI_PROFILE_FROM_ETH_RIF_PROFILE(VSI_MSB_PREFIX, new_in_lif_profile);

        /*
         * set new VSI profile (only VRF and 8 LSB bits of VSI_PROFILE may have been changed)
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ING_VSI_INFO_DB, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, BCM_L3_ITF_VAL_GET(*intf_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     gport_hw_resources.inlif_dbal_result_type);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRF, INST_SINGLE, ing_intf->vrf);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI_PROFILE, INST_SINGLE, new_vsi_profile);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_PROFILE, INST_SINGLE, qos_profile);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    /*
     * else if intf_id is TUNNEL
     */
    else if (BCM_L3_ITF_TYPE_IS_LIF(*intf_id))
    {
        uint32 qos_profile = DNX_QOS_MAP_PROFILE_GET(ing_intf->qos_map_id);
        /*
         * set new IN-LIF profile and VRF
         */
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, *intf_id);
        SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                    (unit, gport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                                     &gport_hw_resources), _SHR_E_NOT_FOUND, _SHR_E_PARAM);
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, gport_hw_resources.inlif_dbal_table_id, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     gport_hw_resources.inlif_dbal_result_type);
        /*
         * no VRF field for result type in_lif_ipvx_1 and in_lif_ipvx2 for table in_lif_ipvx_tunnels
         */
        if ((gport_hw_resources.inlif_dbal_table_id != DBAL_TABLE_IN_LIF_IPvX_TUNNELS)
            || ((gport_hw_resources.inlif_dbal_result_type != DBAL_RESULT_TYPE_IN_LIF_IPvX_TUNNELS_IN_LIF_IPVX_1)
                && (gport_hw_resources.inlif_dbal_result_type != DBAL_RESULT_TYPE_IN_LIF_IPvX_TUNNELS_IN_LIF_IPVX_2)))
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRF, INST_SINGLE, ing_intf->vrf);
        }
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, new_in_lif_profile);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_PROFILE, INST_SINGLE, qos_profile);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief - Delete L3 interface - ingress part
 */
static int
dnx_l3_intf_delete_ingress(
    int unit,
    bcm_l3_intf_t * intf,
    uint8 print_error_if_not_found)
{
    uint32 entry_handle_id;
    uint32 profile_to_write, vsi_profile, old_prefix_profile, propag_profile;
    uint32 result_type;
    int new_in_lif_profile, is_allocated;
    bcm_mac_t prefix;
    uint8 is_last, is_propag_profile_last;
    uint8 intf_exists = 0;
    uint8 is_vsi_allocated;
    int vsi;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(intf, _SHR_E_PARAM, "intf");

    SHR_IF_ERR_EXIT(dnx_l3_intf_get_delete_verify(unit, intf, &intf_exists));
    if (print_error_if_not_found && !intf_exists)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "intf not found\n");
    }
    else if (!intf_exists)
    {
        return _SHR_E_NOT_FOUND;
    }

    /*
     * init vars to default values
     */
    prefix[0] = prefix[1] = prefix[2] = prefix[3] = prefix[4] = prefix[5] = 0x0;
    new_in_lif_profile = 0;
    profile_to_write = 0;
    is_propag_profile_last = 0;

    /*
     * get the current VSI_profile and prefix profile from VSI table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ING_VSI_INFO_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, intf->l3a_intf_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_VSI_PROFILE, INST_SINGLE,
                                                        &vsi_profile));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MY_MAC_PREFIX, INST_SINGLE,
                                                        &old_prefix_profile));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                        &result_type));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROF, INST_SINGLE,
                                                        &propag_profile));

    /*
     * Free instance in template manager. Valid profiles starts from 1, so call the template manager free only
     * If that's the case
     */
    if (old_prefix_profile)
    {
        SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.ingress_mymac_prefix_table.profile_data_get
                        (unit, old_prefix_profile, &is_allocated, NULL));
        if (is_allocated)
        {
            SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.ingress_mymac_prefix_table.free_single
                            (unit, old_prefix_profile, &is_last));
        }
    }

    SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_free_and_hw_clear
                    (unit, propag_profile, &is_propag_profile_last));

    SHR_IF_ERR_EXIT(dnx_in_lif_profile_dealloc(unit, vsi_profile, &new_in_lif_profile, ETH_RIF));
    /*
     * apply VSI_PROFILE MSB bits to the RIF/IN-LIF profile
     */
    profile_to_write = CALCULATE_VSI_PROFILE_FROM_ETH_RIF_PROFILE(VSI_MSB_PREFIX, new_in_lif_profile);

    /*
     * In case VSI is allocated, then only clear L3 fields
     * Otherwise, clear all
     */
    vsi = intf->l3a_intf_id;
    is_vsi_allocated = 0;
    SHR_IF_ERR_EXIT(vlan_db.vsi.is_allocated(unit, vsi, &is_vsi_allocated));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ING_VSI_INFO_DB, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, intf->l3a_intf_id);
    if (is_vsi_allocated)
    {
        /*
         * set default VSI profile (only VSI_PROFILE LSB bits),VRF, mymac and mymac prefix
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRF, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI_PROFILE, INST_SINGLE, profile_to_write);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MY_MAC, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MY_MAC_PREFIX, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROF, INST_SINGLE, 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

    /*
     * if is_last then we want to delete the prefix from the HW table
     */
    if (is_last)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_L3_MY_MAC_DA_PREFIXES, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MY_MAC_INDEX, old_prefix_profile);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get out rif information from table manager
 */
static shr_error_e
dnx_l3_intf_egress_outlif_info_get(
    int unit,
    bcm_l3_intf_t * intf)
{
    uint32 entry_handle_id;
    uint32 qos_map_id;
    lif_table_mngr_outlif_info_t outlif_info;
    uint8 is_field_valid;
    uint32 stat_pp_profile;
    int erpp_out_lif_profile, rv;
    bcm_gport_t intf_gport;
    erpp_out_lif_profile_info_t erpp_out_lif_profile_info;
    uint8 is_reversed;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_RIF_BASIC, &entry_handle_id));

    /*
     * Get lif info from lif table manager
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_outlif_info(unit, intf->l3a_intf_id, entry_handle_id, &outlif_info));

    /*
     * Get Statistics flag - if set stat field exists set the stat flag and values
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, &is_field_valid, &stat_pp_profile));
    if (is_field_valid == TRUE)
    {
        uint32 stat_id;

        is_reversed = dnx_data_stat.stat_pp.etpp_reversed_stat_cmd_get(unit);

        intf->l3a_flags2 |= BCM_L3_FLAGS2_EGRESS_STAT_ENABLE;

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                            entry_handle_id,
                                                            DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE, &stat_id));

        if (is_reversed)
        {
            STAT_PP_ETPP_SWITCH_CMD_GET(stat_id, stat_pp_profile);
        }
        /*
         * Get and set the statistic data only if the profile is valid
         */
        if (stat_pp_profile != STAT_PP_PROFILE_INVALID)
        {
            intf->stat_id = stat_id;
            STAT_PP_ENGINE_PROFILE_SET(intf->stat_pp_profile, stat_pp_profile, bcmStatCounterInterfaceEgressTransmitPp);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid stat_pp profile\n");
        }
    }
    /*
     * Get Remark profile
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE,
                                                        INST_SINGLE, &qos_map_id));
    if (qos_map_id != 0)
    {
        DNX_QOS_REMARK_MAP_SET(qos_map_id);
        DNX_QOS_EGRESS_MAP_SET(qos_map_id);
        intf->dscp_qos.qos_map_id = qos_map_id;
    }

    /*
     * with external kbp OutLif profile not in use, all bits allocated for EEDB pointer
     */
    if ((dnx_data_lif.out_lif.outlif_profile_width_get(unit)) != 0)
    {
        /** Get the old outlif profile from GLEM and read the configurred TTL Threshold for it*/
        BCM_GPORT_TUNNEL_ID_SET(intf_gport, intf->l3a_intf_id);
        rv = dnx_out_lif_profile_get(unit, intf_gport, &erpp_out_lif_profile);
        if (rv == _SHR_E_NOT_FOUND)
        {
            SHR_EXIT();
        }
        else if (rv != _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Couldn't retrieve the outlif profile");
        }
        erpp_out_lif_profile_info_t_init(unit, &erpp_out_lif_profile_info);
        SHR_IF_ERR_EXIT(dnx_erpp_out_lif_profile_get_data(unit, erpp_out_lif_profile, &erpp_out_lif_profile_info));
        intf->l3a_ttl = erpp_out_lif_profile_info.ttl_scope_threshold;
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Get L3 interface info (ingress part) from HW tables
 */
static int
dnx_l3_intf_ingress_get(
    int unit,
    bcm_l3_intf_t * intf)
{
    uint32 entry_handle_id, vrf, my_mac_prefix, mac_address;
    int ref_count;
    bcm_mac_t prefix, mac_get = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * get the VSI entry from ingress VSI table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ING_VSI_INFO_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, intf->l3a_intf_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_VRF, INST_SINGLE, &vrf));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MY_MAC_PREFIX, INST_SINGLE,
                                                        &my_mac_prefix));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MY_MAC, INST_SINGLE,
                                                        &mac_address));
    /*
     * get mymac prefix
     */
    SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.ingress_mymac_prefix_table.profile_data_get
                    (unit, my_mac_prefix, &ref_count, (uint8 *) &prefix));
    /*
     * get mymac
     */
    L3_INTF_SET_MY_MAC_LSB(mac_get, mac_address);
    L3_INTF_SET_MY_MAC_MSB(mac_get, prefix);

    /*
     * compare required mac to mac in ingress table
     */
    if (sal_memcmp(intf->l3a_mac_addr, mac_get, L3_MAC_ADDR_SIZE_IN_BYTES) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "mac addr in ingress table doesn't match required mac");
    }

    intf->l3a_vrf = vrf;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get L3 interface info (egress part) from HW tables
*/
static shr_error_e
dnx_l3_intf_egress_get(
    int unit,
    bcm_l3_intf_t * intf)
{
    uint32 my_mac_prefix;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get entry from EG_VSI table
     */
    SHR_IF_ERR_EXIT(dnx_l3_intf_egress_vsi_info_get(unit, intf, &my_mac_prefix));

    /*
     *Get entry from RIF table
     */
    SHR_IF_ERR_EXIT(dnx_l3_intf_egress_outlif_info_get(unit, intf));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Delete L3 interface (egress part) from SW, HW tables
*  Deleted entries in tables: source_address_ethernet, rif_basic, eg_vsi_info
*  Also - clear the egress sw template relevant info
* \par DIRECT INPUT:
*  \param [in] unit -
*    The Unit number.
*  \param [in] intf -
*    The L3 interface structure to be deleted.
* \par INDIRECT INPUT:
*   * \b *intf \n
*     See 'intf' in DIRECT INPUT above
* \par DIRECT OUTPUT:
*   shr_error_e - Non-zero in case of an error.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   *  Important: the assumption is that the interface exists.
*      This means egress should be always cleared after ingress.
*   *  intf must be valid pointer
* \see
*   * None
*/
static shr_error_e
dnx_l3_intf_delete_egress(
    int unit,
    bcm_l3_intf_t * intf)
{
    uint32 mymac_prefix;
    uint32 entry_handle_id;
    uint8 last_ref = 0;
    int is_allocated, old_erpp_out_lif_profile = 0, new_erpp_out_lif_profile = 0, rv;
    erpp_out_lif_profile_info_t erpp_out_lif_profile_info;
    bcm_gport_t intf_gport;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(intf, _SHR_E_PARAM, "NULL input - intf");

    /*
     * vid(for vsi) and intf_id(for rif) must match
     */
    intf->l3a_vid = intf->l3a_intf_id;
     /** Get entry from EG_VSI table */
    SHR_IF_ERR_EXIT(dnx_l3_intf_egress_vsi_info_get(unit, intf, &mymac_prefix));

    /*
     * SW clear
     */
    /** Clear mymac prefix from sw algo template. In case of last_ref - clear it from source address eth table */
    SHR_IF_ERR_EXIT(algo_l3_db.source_address_table_allocation.egress_source_address_table.profile_data_get
                    (unit, mymac_prefix, &is_allocated, NULL));
    if (is_allocated)
    {
        SHR_IF_ERR_EXIT(algo_l3_db.source_address_table_allocation.egress_source_address_table.free_single
                        (unit, mymac_prefix, &last_ref));
    }

    /*
     * with external kbp OutLif profile not in use, all bits allocated for EEDB pointer
     */
    if ((dnx_data_lif.out_lif.outlif_profile_width_get(unit)) != 0)
    {
        BCM_GPORT_TUNNEL_ID_SET(intf_gport, intf->l3a_intf_id);
        rv = dnx_out_lif_profile_get(unit, intf_gport, &old_erpp_out_lif_profile);
        if (rv == _SHR_E_NOT_FOUND)
        {
            SHR_EXIT();
        }
        else if (rv != _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Couldn't retrieve the outlif profile");
        }
        erpp_out_lif_profile_info_t_init(unit, &erpp_out_lif_profile_info);
        SHR_IF_ERR_EXIT(dnx_erpp_out_lif_profile_exchange
                        (unit, &erpp_out_lif_profile_info, old_erpp_out_lif_profile, &new_erpp_out_lif_profile));
        if (new_erpp_out_lif_profile != DNX_OUT_LIF_PROFILE_DEFAULT)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error! Something is wrong! on freeing oulif_profile, didn't get default out lif profile, default outlif profile = %d, new_out_lif_profile = %d",
                         DNX_OUT_LIF_PROFILE_DEFAULT, new_erpp_out_lif_profile);
        }
    }

    /*
     * Free OuRif and EEDB HW
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_outlif_info_clear(unit, intf->l3a_intf_id, 0));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGR_VSI_INFO_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, intf->l3a_vid);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

    if (last_ref)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_MAC_SOURCE_ADDRESS, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_ADDRESS_INDEX,
                                   L3_INTF_MY_MAC_ETH_PREFIX(mymac_prefix));
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Deletes L3 interface.
 * \param [in] unit -
 *   The unit number.
 * \param [in] intf -
 *   Pointer to bcm_l3_intf_t \n
 *     Pointed memory includes L3 interface to delete,
 *     only sub-field l3a_intf_id is used as input to identify the interface entry in VSI table
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   We don't clean HW tables in case of last profile case, because of the
 *   issue that when we delete the profile till we move the InLIF
 *   to the new place traffic will get some "undefine" state.
 * \see
 *   * None
 */
int
bcm_dnx_l3_intf_delete(
    int unit,
    bcm_l3_intf_t * intf)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_IF_ERR_EXIT(dnx_l3_intf_delete_ingress(unit, intf, 1));
    /** Egress delete must be called after ingress delete because it doesn't have existence check */
    SHR_IF_ERR_EXIT(dnx_l3_intf_delete_egress(unit, intf));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Deletes all L3 interfaces.
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_l3_intf_delete_all(
    int unit)
{
    bcm_l3_intf_t intf;
    int rif_idx;
    int rv;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * loop over the range of intf ids and call bcm_dnx_l3_intf_delete per intf
     */
    for (rif_idx = 1; rif_idx < dnx_data_l3.rif.nof_rifs_get(unit); rif_idx++)
    {
        bcm_l3_intf_t_init(&intf);
        intf.l3a_intf_id = rif_idx;
        rv = dnx_l3_intf_delete_ingress(unit, &intf, 0);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, BCM_E_NOT_FOUND);
        if (rv == BCM_E_NONE)
        {
                /** Egress configuration is present only in case ingress interface was found */
            SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_l3_intf_delete_egress(unit, &intf), BCM_E_NOT_FOUND);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   get L3 interface given interface ID.
 * \param [in] unit -
 *   The unit number.
 * \param [in] intf -
 *   Pointer to bcm_l3_intf_t \n
 *     Pointed memory includes L3 interface ID to get properties for.
 *     used sub-fields:
 *     input : l3a_intf_id
 *     output : l3a_mac_addr and l3a_vrf
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_l3_intf_get(
    int unit,
    bcm_l3_intf_t * intf)
{
    uint32 entry_handle_id, vrf, my_mac_prefix, mac_address;
    int ref_count;
    bcm_mac_t prefix;
    uint8 intf_exists = 0;
    uint32 result_type;
    uint32 ecn_mapping_profile;
    uint32 propag_profile;
    dnx_qos_propagation_type_e phb_propagation_type;
    dnx_qos_propagation_type_e remark_propagation_type;
    dnx_qos_propagation_type_e ecn_propagation_type;
    dnx_qos_propagation_type_e ttl_propagation_type;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_l3_intf_get_delete_verify(unit, intf, &intf_exists));
    if (!intf_exists)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "intf not found\n");
    }

    /*
     * get the VSI entry from ingress VSI table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ING_VSI_INFO_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, intf->l3a_intf_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_VRF, INST_SINGLE, &vrf));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MY_MAC_PREFIX, INST_SINGLE,
                                                        &my_mac_prefix));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                        &result_type));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MY_MAC, INST_SINGLE,
                                                        &mac_address));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_ECN_MAPPING_PROFILE, INST_SINGLE, &ecn_mapping_profile));
    /*
     * get mymac prefix
     */
    SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.ingress_mymac_prefix_table.profile_data_get
                    (unit, my_mac_prefix, &ref_count, (uint8 *) &prefix));
    /*
     * assign mymac
     */
    prefix[5] = mac_address & 0xFF;
    prefix[4] = prefix[4] | ((mac_address & 0x300) >> SAL_UINT8_NOF_BITS);

    sal_memcpy(intf->l3a_mac_addr, prefix, L3_MAC_ADDR_SIZE_IN_BYTES);
    intf->l3a_vrf = vrf;

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                        &result_type));

    /** get propagation profile type */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROF, INST_SINGLE, &propag_profile));

    SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_attr_get
                    (unit, propag_profile, &phb_propagation_type, &remark_propagation_type, &ecn_propagation_type,
                     &ttl_propagation_type));

    if (phb_propagation_type != DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_propag_type_to_qos_model
                        (unit, phb_propagation_type, &(intf->ingress_qos_model.ingress_phb)));
    }
    if (remark_propagation_type != DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_propag_type_to_qos_model
                        (unit, remark_propagation_type, &(intf->ingress_qos_model.ingress_remark)));
    }

    if (ttl_propagation_type != DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_propag_type_to_qos_model
                        (unit, ttl_propagation_type, &(intf->ingress_qos_model.ingress_ttl)));
    }

    if (ecn_mapping_profile == DNX_QOS_ECN_MAPPING_PROFILE_ELIGIBLE)
    {
        intf->ingress_qos_model.ingress_ecn = bcmQosIngressEcnModelEligible;
    }
    else
    {
        intf->ingress_qos_model.ingress_ecn = bcmQosIngressEcnModelInvalid;
    }

    /*
     * get egress outlif info
     */
    SHR_IF_ERR_EXIT(dnx_l3_intf_egress_outlif_info_get(unit, intf));

    /*
     * vid(for vsi) and intf_id(for rif) must match
     */
    intf->l3a_vid = intf->l3a_intf_id;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Search for L3 interface by MAC address and VLAN.
 * \param [in] unit -
 *   Relevant unit.
 * \param [in,out] intf - The incoming intf structure \n
 *   [out] intf.l3a_intf_id - ID of ETH-RIF, must be set (must be WITH-ID) \n
 *   [in] intf.l3a_mac_addr - MAC address of ETH-RIF \n
 *   [out] intf.l3a_vrf - VRF \n
 *   [in] intf.l3a_vid - VSI (must be set same value as l3a_intf_id) \n
 *   [out] intf.dscp_qos.qos_map_id - QOS Remark profile (egress)
 * \return
 *   \retval Negative in case of an error. See shr_error_e, for example: intf not found
 *   \retval Zero in case of NO ERROR
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_l3_intf_find(
    int unit,
    bcm_l3_intf_t * intf)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(intf, _SHR_E_PARAM, "l3_intf");

    /*
     * Check the provided VLAN ID is in range
     */
    BCM_DNX_VLAN_CHK_ID_NONE(unit, intf->l3a_vid, _SHR_E_PARAM);
    BCM_DNX_VLAN_CHK_ID(unit, intf->l3a_vid);

    /*
     * vid(for vsi) and intf_id(for rif) must match
     */
    intf->l3a_intf_id = intf->l3a_vid;

    SHR_IF_ERR_EXIT(dnx_l3_intf_find_verify(unit, intf));

    /*
     * Get L3 intf info (ingress part) from HW tables
     */
    SHR_IF_ERR_EXIT(dnx_l3_intf_ingress_get(unit, intf));

    /*
     * Get L3 intf info (egress part) from HW tables
     */
    SHR_IF_ERR_EXIT(dnx_l3_intf_egress_get(unit, intf));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Search for L3 interface by VLAN.
 * \param [in] unit
 *   Relevant unit.
 * \param [in,out] intf - The incoming intf structure \n
 *   [out] intf.l3a_intf_id - ID of ETH-RIF, must be set (must be WITH-ID) \n
 *   [out] intf.l3a_vrf - VRF \n
 *   [in] intf.l3a_vid - VSI (must be set same value as l3a_intf_id) \n
 *   [out] intf.dscp_qos.qos_map_id - QOS Remark profile (egress)
 * \return
 *   \retval Negative in case of an error. See shr_error_e, for example: intf not found
 *   \retval Zero in case of NO ERROR
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_l3_intf_find_vlan(
    int unit,
    bcm_l3_intf_t * intf)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(intf, _SHR_E_PARAM, "l3_intf");

    SHR_IF_ERR_EXIT(dnx_l3_intf_find_vlan_verify(unit, intf));

    /*
     * vid(for vsi) and intf_id(for rif) must match
     */
    intf->l3a_intf_id = intf->l3a_vid;

    SHR_IF_ERR_EXIT(bcm_dnx_l3_intf_get(unit, intf));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   gets intf id and returns ingress properties
 * \param [in] unit -
 *   The unit number.
 * \param [in] intf -
 *   Interface ID
 * \param [out] ing_intf -
 *   Pointer to bcm_l3_ingress_t \n
 *     this struct will include the ingress properties of the given interface as following:
 *     flags: ingress flags BCM_L3_INGRESS_*
 *     urpf_mode : the RPF mode, can be one of {strict,loose,disabled}
 *     vrf
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_l3_ingress_get(
    int unit,
    bcm_if_t intf,
    bcm_l3_ingress_t * ing_intf)
{
    in_lif_profile_info_t in_lif_profile_info;
    uint32 in_lif_profile, qos_profile, vsi_profile, entry_handle_id, vrf, gport;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    in_lif_intf_type_e intf_type;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_l3_ingress_get_verify(unit, intf, ing_intf));

    /*
     * init intf_type
     */
    intf_type = ETH_RIF;
    /*
     * get in_lif_profile from VSI or LIF table
     * in case BCM_L3_ITF_TYPE_IS_RIF is true then get the in_lif_profile from VSI table,
     * else if BCM_L3_ITF_TYPE_IS_LIF is true then get the in_lif_profile from LIF table.
     */
    if (BCM_L3_ITF_TYPE_IS_RIF(intf))
    {
        /*
         * get the current VSI-PROFILE and vrf from VSI-TABLE
         */
        vsi_profile = CALCULATE_VSI_PROFILE_FROM_ETH_RIF_PROFILE(VSI_MSB_PREFIX, vsi_profile);
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ING_VSI_INFO_DB, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, BCM_L3_ITF_VAL_GET(intf));
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_QOS_PROFILE, INST_SINGLE,
                                                            &qos_profile));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_VSI_PROFILE, INST_SINGLE,
                                                            &vsi_profile));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_VRF, INST_SINGLE, &vrf));

        /*
         * IN-LIF-PROFILE is the LSB bits of VSI-PROFILE
         */
        in_lif_profile = (vsi_profile & VSI_LSB_BITS_MASK_FOR_ETH_RIF_PROFILE);
        intf_type = ETH_RIF;
    }
    /*
     * else if intf is TUNNEL
     */
    else if (BCM_L3_ITF_TYPE_IS_LIF(intf))
    {
        /*
         * get the current IN-LIF-PROFILE and vrf from LIF-TABLE
         */
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, intf);
        SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                    (unit, gport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                                     &gport_hw_resources), _SHR_E_NOT_FOUND, _SHR_E_PARAM);
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     gport_hw_resources.inlif_dbal_result_type);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_QOS_PROFILE, INST_SINGLE,
                                                            &qos_profile));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, &in_lif_profile));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_VRF, INST_SINGLE, &vrf));
        intf_type = LIF;
    }
    if (qos_profile != 0)
    {
        DNX_QOS_PHB_MAP_SET(qos_profile);
        DNX_QOS_REMARK_MAP_SET(qos_profile);
        DNX_QOS_INGRESS_MAP_SET(qos_profile);
    }
    ing_intf->qos_map_id = qos_profile;

    /*
     * get in_lif_profile data and store it in in_lif_profile_info
     */
    SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, in_lif_profile, &in_lif_profile_info, intf_type));

    /*
     * assign output
     */
    ing_intf->flags = in_lif_profile_info.ingress_fields.l3_ingress_flags;
    ing_intf->oam_default_profile = in_lif_profile_info.ingress_fields.oam_default_profile;
    ing_intf->urpf_mode = in_lif_profile_info.ingress_fields.urpf_mode;
    ing_intf->vrf = vrf;
    if (dnx_data_switch.load_balancing.hashing_selection_per_layer_get(unit))
    {
        ing_intf->hash_layers_disable = in_lif_profile_info.ingress_fields.lb_per_lif_flags;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
