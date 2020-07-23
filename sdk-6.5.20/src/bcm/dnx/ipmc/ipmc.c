/** \file ipmc.c
 *
 * Handles the control over the IP multicast entries.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_IPMC

/*
 * Include files currently used for DNX.
 * {
 */
#include <soc/dnx/dbal/dbal.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_elk.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/dnx/field/tcam/tcam_handler.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/stat/stat_pp.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <bcm_int/dnx/ipmc/ipmc.h>
#include <soc/dnx/mdb.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
/*
 * }
 */
/*
 * Include files.
 * {
 */
#include <bcm/ipmc.h>
#include <bcm/types.h>
/*
 * }
 */

/*
 * DEFINEs
 * {
 */
/*
 * This is an IPv4 empty mask, if this mask is used all the IPv4 bits are relevant (no masking).
 */
#define IPMC_IPV4_EMPTY_MASK    0xFFFFFFFF
/*
 * This IPv4 group MC mask size is 28 which leaves only the IPv4 group bits (28 LSB bits) on and hides the rest of the  4 bits prefix.
 */
#define IPMC_IPV4_GROUP_MASK_SIZE    28
/*
 * The four MSB of every IPv4 multicast address
 */
#define IPMC_MC_IPV4_ADDRESS_PREFIX 0xe
/*
 * Maximum number of IPMC tables to be traversed
 */
#define MAX_NOF_IPMC_TABLES  15
/*
 * Start ID of IPv4 tables in the IPMC tables array
 */
#define IPMC_IPV4_TABLES_START    0
/*
 * Number of IPv4 MC tables
 */
#define IPMC_IPV4_TABLES_NOF      5
/*
 * Start ID of Bridge tables in the IPMC tables array
 */
#define IPMC_BRIDGE_TABLES_START  (IPMC_IPV4_TABLES_START + IPMC_IPV4_TABLES_NOF)
/*
 * Number of Bridge tables
 */
#define IPMC_BRIDGE_TABLES_NOF    2
/*
 * Start ID of IPv6 Mc tables in the IPMC tables array
 */
#define IPMC_IPV6_TABLES_START    (IPMC_BRIDGE_TABLES_START + IPMC_BRIDGE_TABLES_NOF)
/*
 * Number of IPv6 Mc tables
 */
#define IPMC_IPV6_TABLES_NOF      4
/** Number of LSBs in the GLOBAL IN LIF field which is separated into two for IPV6 tables. */
#define IPMC_IPV6_GLOB_IN_LIF_NOF_LSB  16
/*
 * Start ID of the IPV6 Bridge MC tables in the IPMC tables array
 */
#define IPMC_IPV6_BRIDGE_TABLES_START   (IPMC_IPV6_TABLES_NOF + IPMC_IPV6_TABLES_START)
/*
 * Number of the IPV6 Bridge MC tables
 */
#define IPMC_IPV6_BRIDGE_TABLES_NOF     1
/*
 * }
 */

/*
 * MACROs
 * {
 */
#define IPMC_SUPPORTED_BCM_IPMC_FLAGS (BCM_IPMC_REPLACE | BCM_IPMC_IP6 | BCM_IPMC_HIT_CLEAR | BCM_IPMC_TCAM | BCM_IPMC_RAW_ENTRY | BCM_IPMC_L2 | BCM_IPMC_HIT | BCM_IPMC_DEFAULT_ENTRY)
/*
 * Returns whether the mask (input to the MACRO) is empty (all ones) or not.
 */
#define IPMC_IS_IPV4_NO_MASKING(mask) (mask == IPMC_IPV4_EMPTY_MASK)
/*
 * Returns whether the mask (an uint32 value input to the MACRO) is full (all zeros) or not.
 */
#define IPMC_IS_IPV4_FULL_MASKING(mask) (mask == 0)
/*
 * Returns whether the IPv4 address is a default one (all zeros) or not.
 */
#define IPMC_IS_IPV4_DEFAULT_ADDRESS(address) (address == 0x0)
/*
 * Indicates whether the input address is a valid IPv4 MC address.
 * It is considered valid as long as the unmasked bits of the first nibble of the address
 * are the same as the bits in the MC prefix (0xe).
 */
#define IPMC_IS_IPV4_MC_GROUP_VALID(addr, mask) (((addr & mask) >> IPMC_IPV4_GROUP_MASK_SIZE) == \
                                                    ((mask >> IPMC_IPV4_GROUP_MASK_SIZE) & IPMC_MC_IPV4_ADDRESS_PREFIX))
/*
 * Indicates whether the input address (addr) is an IPV4 multicast address (has a 4 bit prefix of 0xe)
 */
#define IPMC_IS_IPV4_ADDRESS_MULTICAST(addr) (((addr) >> IPMC_IPV4_GROUP_MASK_SIZE) == IPMC_MC_IPV4_ADDRESS_PREFIX)
/*
 * As all the MC destination starts with the same 4 bits prefix (e), we take only the first 28bits of the address (addr)
 * into the table to represent the group.
 */
#define IPMC_IPV4_GET_GROUP(addr) (addr &  SAL_UPTO_BIT(IPMC_IPV4_GROUP_MASK_SIZE))
/*
 * Apply prefix (the four MSBs) of every IPv4 multicast address that has a value of 0xE
 * which has been removed when saving the address in the table to reduce used space.
 */
#define IPMC_IPV4_SET_GROUP(addr) ((IPMC_MC_IPV4_ADDRESS_PREFIX << IPMC_IPV4_GROUP_MASK_SIZE) | addr)
/** Apply the four MSBs to the IPV4 group mask. */
#define IPMC_IPV4_SET_GROUP_MASK(mask) ((0xF << IPMC_IPV4_GROUP_MASK_SIZE) | mask)
/*
 * The IN-LIF (in_lif) can be fully mask (don't care about it value) or not masked at all (nothing in between).
 */
#define IPMC_GET_IN_LIF_MASK(in_lif) (in_lif == 0 ? 0 : SAL_UPTO_BIT(IPMC_GLOBAL_IN_LIF_MASK_SIZE))
/*
 * Determine whether the IPMC data structure defines a host entry.
 */
#define IPMC_IS_IPV4_HOST_ENTRY(data, public_capacity) (!_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_TCAM) && (data->vid > 0 || data->ing_intf > 0) \
                                        && (data->vrf != 0 || (data->vrf == 0 && public_capacity == 0))\
                                        && IPMC_IS_IPV4_FULL_MASKING(data->s_ip_mask) && IPMC_IS_IPV4_NO_MASKING(data->mc_ip_mask))
/*
 * Determine whether the input IPMC data structure (bcm_ipmc_addr_t) with flag BCM_IPMC_IP6 defines a host entry.
 * If the MC IP is not masked and the VID/ing_intf value is valid, non-zero,
 * then the bcm_ipmc_addr_t structure describes a host entry.
 */
#define IPMC_IS_IPV6_HOST_ENTRY(data, public_capacity) (!_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_TCAM) && IPMC_IS_IPV6_NO_MASKING(data->mc_ip6_mask)\
                                       && (data->vid > 0 || data->ing_intf > 0) && (data->vrf != 0 || (data->vrf == 0 && public_capacity == 0)) && IPMC_IS_IPV6_FULL_MASKING(data->s_ip6_mask))
/*
 * Determine whether the address structure with flag BCM_IPMC_L2 describes an EM entry
 */
#define IPMC_IS_BRIDGE_EM_ENTRY(data) (IPMC_IS_IPV4_NO_MASKING(data->mc_ip_mask) \
                                       && IPMC_IS_IPV4_FULL_MASKING(data->s_ip_mask)\
                                       && _SHR_IS_FLAG_SET(data->flags, BCM_IPMC_L2))

/** Determine whether the entry is a default IPv4 one. */
#define IPMC_IS_IPV4_DEFAULT_ENTRY(data) ((data->s_ip_addr & data->s_ip_mask) == 0 && (data->mc_ip_addr & data->mc_ip_mask) == 0 \
                                          && !_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_IP6))
/*
 * Indicates whether the table is KBP
 */
#define IPMC_TABLE_IS_KBP(table) ( \
        (table == DBAL_TABLE_KBP_IPV4_MULTICAST_TCAM_FORWARD) || \
        (table == DBAL_TABLE_KBP_IPV6_MULTICAST_TCAM_FORWARD))
 /*
  * Return the appropriate result type according to the KBP table
  */
#define IPMC_KBP_TABLE_TO_RESULT_TYPE_FWD_DEST(table) ( \
        (table == DBAL_TABLE_KBP_IPV4_MULTICAST_TCAM_FORWARD) ? DBAL_RESULT_TYPE_KBP_IPV4_MULTICAST_TCAM_FORWARD_KBP_DEST_W_DEFAULT : \
        (table == DBAL_TABLE_KBP_IPV6_MULTICAST_TCAM_FORWARD) ? DBAL_RESULT_TYPE_KBP_IPV6_MULTICAST_TCAM_FORWARD_KBP_DEST_W_DEFAULT : -1)
/*
 * KBP has 32 bits MC group instead of 28 bits 24 bits IN_LIF instead of 22 bits due to requirement for the KBP keys
 * to be multiple of 8. In order to cover the LPM mask requirements, need to set the unused MSBs to 1.
 * Set the IN_LIF bits only when it is not 0.
 */
#define IPMC_KBP_IPV4_GROUP_SET(_mc_group_) (~(0xf << 28) & _mc_group_)
#define IPMC_KBP_IN_LIF_SET(_in_lif_)       (~(0x3 << 22) & _in_lif_)

/** Retrieve the 16 LSBs from the in_lif. Used for IPv6 EM and TCAM tables. */
#define DNX_IPMC_GLOB_IN_LIF_LSB_GET(in_lif) (in_lif & ((1 << IPMC_IPV6_GLOB_IN_LIF_NOF_LSB) - 1))
/** Retrieve the 6 MSBs from the in_lif. Used for IPv6 EM and TCAM tables. */
#define DNX_IPMC_GLOB_IN_LIF_MSB_GET(in_lif) (in_lif >> IPMC_IPV6_GLOB_IN_LIF_NOF_LSB)

/*
 * }
 */

/**
 * \brief
 *   Determine whether the IPv6 address is a default one or not. If
 *   the IPv6 address is 0:0:0:0:0:0:0:0 then it is default.
 * \param [in] address - The IPv6 address that needs to be checked
 *                       if it is all zeros.
 * \param [in] mask - The mask of the IPv6 address.
 *   A raised bit in the mask indicates that a bit from the IP address is shown.
 * \return
 *   \retval Zero if the IPv6 address is not default
 *   \retval One if the IPv6 address is default.
 * \see
 *  * dnx_ipmc_verify
 */
static int
dnx_ipmc_ipv6_default_address(
    bcm_ip6_t address,
    bcm_ip6_t mask)
{
    int byte;
    int default_mask = 1;
    for (byte = NOF_IPV6_ADDR_BYTES - 1; byte >= 0 && default_mask == 1; byte--)
    {
        /*
         * If the current bit is raised, then this is definitely not a default mask.
         */
        if (((address[byte] & mask[byte]) & 0xFF) != 0)
        {
            default_mask = 0;
            break;
        }
    }
    return default_mask;
}

/**
 * \brief
 *   Verify the ipmc (Internet protocol multicast)
 *   add/remove/find input.
 * \param [in] unit -
 *   The unit number.
 * \param [in] data -
 *   contains the required information for validating the IPMC entry.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * dnx_ipmc_add_verify
 *  * dnx_ipmc_remove
 *  * dnx_ipmc_find
 */
static shr_error_e
dnx_ipmc_verify(
    int unit,
    bcm_ipmc_addr_t * data)
{
    int kaps_public_capacity;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(data, _SHR_E_PARAM, "bcm_ipmc_addr_t");

    SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_KAPS_2, &kaps_public_capacity));
    /** Verify supported flags */
    if (_SHR_IS_FLAG_SET(data->flags, ~IPMC_SUPPORTED_BCM_IPMC_FLAGS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal IPMC flags are passed to bcm_ipmc_* API.");
    }

    /*
     * Verify that the VRF is in range.
     */
    if (data->vrf >= dnx_data_l3.vrf.nof_vrf_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "VRF %d is out of range, should be lower then %d.", data->vrf,
                     dnx_data_l3.vrf.nof_vrf_get(unit));
    }

    /*
     * If BCM_IPMC_IP6 flag is set, then an IPv6 fields will be verified.
     * Valid fields for it are mc_ip6_addr, mc_ip6_mask, s_ip6_addr.
     *
     * If flag BCM_IPMC_IP6 is NOT set, then an IPv4 structure fields will be verified.
     * Valid fields are mc_ip_addr, mc_ip_mask, s_ip_addr and s_ip_mask.
     */
    if (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_IP6))
    {
        /** Check whether IPv6 address is MC */
        if (!IPMC_IS_IPV6_ADDRESS_MULTICAST(data->mc_ip6_addr, data->mc_ip6_mask)
            && !(IPMC_IS_IPV6_FULL_MASKING(data->mc_ip6_mask)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "IPv6 address group is not multicast - expecting the first byte to be 0xFF, instead it is 0x%02X",
                         data->mc_ip6_addr[0] & data->mc_ip6_mask[0]);
        }
        /** Verify that the IPv6 source address is not multicast */
        if (IPMC_IS_IPV6_ADDRESS_MULTICAST(data->s_ip6_addr, data->s_ip6_mask))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "IPv6 source address is multicast - expecting the first byte not to be 0xFF");
        }
        /** Verify the cascaded IPv6 entry input - valid for bridge and LPM route entries */
        if (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_L2)
            || (!IPMC_IS_IPV6_HOST_ENTRY(data, kaps_public_capacity) && !_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_TCAM)))
        {
            if (data->s_ip_addr >= dnx_data_l3.fwd.nof_compressed_svl_sip_get(unit))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Invalid value for IPv6 compressed SIP, expected value which is less than %d \n",
                             dnx_data_l3.fwd.nof_compressed_svl_sip_get(unit));
            }
        }
        else if (!_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_L2))
        {
            if ((!_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_TCAM) && IPMC_IS_IPV6_NO_MASKING(data->mc_ip6_mask)
                 && (data->vid > 0 || data->ing_intf > 0) && data->vrf == 0
                 && IPMC_IS_IPV6_FULL_MASKING(data->s_ip6_mask)) && kaps_public_capacity != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot create an IPv6 MC public LPM entry.\n");
            }
            if (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_DEFAULT_ENTRY)
                && IPMC_IS_IPV6_HOST_ENTRY(data, kaps_public_capacity))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid flag BCM_IPMC_DEFAULT_ENTRY for IPv6 host entry\n");
            }
        }
    }
    else
    {
        /*
         * Verify that the SIP address is valid.
         */
        if (IPMC_IS_IPV4_NO_MASKING(data->s_ip_mask) && data->s_ip_addr == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal IPv4 source address 0x%08X with mask 0x%08X- address is empty.",
                         data->s_ip_addr, data->s_ip_mask);
        }
        if (IPMC_IS_IPV4_ADDRESS_MULTICAST(data->s_ip_addr))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal IPv4 source address 0x%08X with mask 0x%08X - address is multicast.",
                         data->s_ip_addr, data->s_ip_mask);
        }
        /*
         * Verify that the DIP address is valid.
         */
        if (!IPMC_IS_IPV4_MC_GROUP_VALID(data->mc_ip_addr, data->mc_ip_mask)
            && !(IPMC_IS_IPV4_FULL_MASKING(data->mc_ip_mask) && IPMC_IS_IPV4_DEFAULT_ADDRESS(data->mc_ip_addr)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal IPv4 destination address 0x%x, not a multicast address.",
                         data->mc_ip_addr);
        }

        /*
         * Verify that the data structure describes a valid entry with a consistent key mask for a bridge entry.
         * If the BCM_IPMC_L2 flag hasn't been provided, then it's not a bridge entry.
         *
         * For host entries VRF = 0 is a valid value as long as a single DB check is set up or it's a public entry.
         * When the property is not set, VRF = 0 is not allowed.
         * Currently only private multicast host table is supported.
         * When the capacity of the MDB table is 0, then only a private lookup will be carried out.
         * Otherwise, both public and private are active.
         */
        if (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_L2))
        {
            if (!IPMC_IS_IPV4_NO_MASKING(data->mc_ip_mask) && !IPMC_IS_IPV4_FULL_MASKING(data->s_ip_mask))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Illegal IPV4 group mask 0x%08X and source address mask 0x%08X values - they should be consistent.",
                             data->mc_ip_mask, data->s_ip_mask);
            }
        }
        else
        {
            int lem_capacity = 0;
            int kaps2_capacity = 0;
            SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_LEM, &lem_capacity));
            SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_KAPS_2, &kaps2_capacity));
            if (!_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_TCAM) && (data->vid > 0 || data->ing_intf > 0)
                && IPMC_IS_IPV4_FULL_MASKING(data->s_ip_mask) && IPMC_IS_IPV4_NO_MASKING(data->mc_ip_mask)
                && data->vrf == 0 && kaps2_capacity != 0)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG,
                             "Illegal VRF value 0 is reserved for public entries and cannot be used for MC host entry.");
            }
            if (IPMC_IS_IPV4_HOST_ENTRY(data, kaps2_capacity) && _SHR_IS_FLAG_SET(data->flags, BCM_IPMC_DEFAULT_ENTRY))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid flag BCM_IPMC_DEFAULT_ENTRY for IPV4 host entry\n");
            }
            /** Verify that the mask of the LPM IPMC entry is consistent */
            if (!IPMC_IS_IPV4_HOST_ENTRY(data, kaps2_capacity) && !_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_TCAM))
            {
                /** If the IPMC DIP is partially or fully masked, then the SIP cannot be anything but fully masked. */
                if (!IPMC_IS_IPV4_NO_MASKING(data->mc_ip_mask) && !IPMC_IS_IPV4_FULL_MASKING(data->s_ip_mask))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Inconsistent mask of IPMC key - it is expected that if SIP mask has any bits unmasked, then DIP should have no mask\n");
                }
                /** If the DIP is different than fully unmasked, then the VID/ing_intf cannot be different than 0. */
                if ((data->vid > 0 || data->ing_intf > 0) && !IPMC_IS_IPV4_NO_MASKING(data->mc_ip_mask))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Inconsistent mask of IPMC key - it is expected that if VID is defined, DIP should have no mask\n");
                }
            }
        }
        if (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_DEFAULT_ENTRY) && _SHR_IS_FLAG_SET(data->flags, BCM_IPMC_TCAM))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid flag combination - TCAM and DEFAULT_ENTRY\n");
        }
    }
    /** Verify mod_id is 0 */
    if (data->mod_id != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "mod_id is not supported,should be 0!\n");
    }
    /** Verify combination of L2 flag with TCAM flag */
    if (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_L2) && _SHR_IS_FLAG_SET(data->flags, BCM_IPMC_TCAM))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid combination of flags - BCM_IPMC_TCAM and BCM_IPMC_L2\n");
    }
    /** Verify that only one out of vid, ing_intf is defined. */
    if (data->vid > 0 && data->ing_intf != BCM_IF_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "It is expected that only one of vid (%d) or ing_intf (%d) will be defined\n",
                     data->vid, data->ing_intf);
    }
    else
    {
        /*
         * Verify that the ETH-RIF value is valid.
         */
        if (data->vid >= dnx_data_l3.rif.nof_rifs_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal ETH-RIF (vid) value %d, should be positive and less than %d.",
                         data->vid, dnx_data_l3.rif.nof_rifs_get(unit));
        }
        /** Verify that the ing_intf value is valid */
        if (data->ing_intf != BCM_IF_INVALID)
        {
            /** Make sure that the value is in range of the LIFs. */
            if (BCM_L3_ITF_VAL_GET(data->ing_intf) >= dnx_data_lif.global_lif.nof_global_in_lifs_get(unit))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal TT-LIF (ing_intf) value %d, should be positive and less than %d.",
                             BCM_L3_ITF_VAL_GET(data->ing_intf), dnx_data_lif.global_lif.nof_global_in_lifs_get(unit));
            }
            /** Make sure that the ing_intf has a LIF ingress type encoding */
            if (!BCM_L3_ITF_TYPE_IS_LIF(data->ing_intf))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Incorrect interface type encoding of ing_intf field - LIF is expected\n");
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify the ipmc (Internet protocol multicast) add input.
 * \param [in] unit -
 *   The unit number.
 * \param [in] data -
 *   contains the required information for validating the IPMC entry.
 *   for field information see the bcm_dnx_ipmc_add.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_ipmc_add
 *  * BCM_IPMC_IP6, BCM_IPMC_RAW_ENTRY
 */
static shr_error_e
dnx_ipmc_add_verify(
    int unit,
    bcm_ipmc_addr_t * data)
{
    unsigned int fec_index;
    unsigned int port;
    uint32 mc_group;
    int kaps_public_capacity;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(data, _SHR_E_PARAM, "bcm_ipmc_addr_t");

    fec_index = BCM_L3_ITF_VAL_GET(data->l3a_intf);
    port = data->port_tgid;
    mc_group = _BCM_MULTICAST_ID_GET(data->group);

    SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_KAPS_2, &kaps_public_capacity));
    /*
     * Verify common fields of the bcm_ipmc_addr_t structure
     */
    SHR_IF_ERR_EXIT(dnx_ipmc_verify(unit, data));
    if (data->group == 0 && data->l3a_intf == 0 && port == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Result field for IPMC entry not defined - expecting either MC group, FEC or port\n");
    }
    /** Make sure that only one of the destination fields (group, l3a_intf, port_tgid) is used */
    if (!(data->group != 0 && data->l3a_intf == 0 && port == 0)
        && !(data->group == 0 && data->l3a_intf != 0 && port == 0)
        && !(data->group == 0 && data->l3a_intf == 0 && port != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "bcm_ipmc_add expects only one destination field to be defined (group = 0x%x; l3a_intf = 0x%x; port_tgid = 0x%x)\n",
                     data->group, data->l3a_intf, port);
    }
    /** Make sure that if the entry is a default one, an l3_intf will be used as destination. */
    if (data->l3a_intf == 0 && (IPMC_IS_IPV4_DEFAULT_ENTRY(data)
                                || (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_IP6)
                                    && dnx_ipmc_ipv6_default_address(data->s_ip6_addr, data->s_ip6_mask)
                                    && dnx_ipmc_ipv6_default_address(data->mc_ip6_addr, data->mc_ip6_mask))
                                || (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_DEFAULT_ENTRY))))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "When adding a default entry, l3a_intf must be used as destination field\n");
    }

    /*
     * The result of the IPMC entry an be an L3 MC group, a FEC or a port.
     * If a MC group is provided, verify that it is in range.
     * If an l3a_intf is defined, verify that the FEC is in range.
     * If a port is provided, verify that it is valid.
     * If not one of the destination fields is defined, return an error.
     */
    if (data->group != 0)
    {
        /*
         * If the Mc group ID is valid, then it needs to be verified if its value is in range.
         * There are different maximum values for it based on whether the entry is a MC route entry or a MC host entry.
         *
         * If the structure describes a MC host entry or a TCAM entry,
         * the maximum value of the MC group is taken from max_mc_group_em.
         * Otherwise, it is a route entry and belongs to the LPM. Its maximum value is taken from the max_mc_group_lpm.
         */
        if ((IPMC_IS_IPV4_HOST_ENTRY(data, kaps_public_capacity) && !_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_IP6))
            || (IPMC_IS_IPV6_HOST_ENTRY(data, kaps_public_capacity) && _SHR_IS_FLAG_SET(data->flags, BCM_IPMC_IP6))
            || IPMC_IS_BRIDGE_EM_ENTRY(data) || _SHR_IS_FLAG_SET(data->flags, BCM_IPMC_TCAM))
        {
            if (mc_group > dnx_data_l3.fwd.max_mc_group_em_get(unit))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Illegal MC group ID value %d, should be positive and less than or equal to %d.",
                             mc_group, dnx_data_l3.fwd.max_mc_group_em_get(unit));
            }
        }
        else
        {
            if (mc_group > dnx_data_l3.fwd.max_mc_group_lpm_get(unit))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Illegal MC group ID value %d, should be positive and less than or equal to %d.", mc_group,
                             dnx_data_l3.fwd.max_mc_group_lpm_get(unit));
            }
        }
    }
    else if (data->l3a_intf != 0)
    {
        if (!_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_RAW_ENTRY))
        {
            /*
             * Enter for Non-raw entries.
             * Raw entries verification will be done after selecting the dbal table, because KAPS and KBP have
             * different available payload sizes - KAPS_RESULT (20 bits) DESTINATION (21 bits).
             */
            if (fec_index >= dnx_data_l3.fec.nof_fecs_get(unit))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "FEC index %d is out of range, max FEC index is %d.", fec_index,
                             dnx_data_l3.fec.nof_fecs_get(unit) - 1);
            }
            /*
             * Default FEC validation will be done after selecting the dbal table,
             * because KAPS uses encoding for default indication - default FEC.
             * This limits the max number of FECs with default indication.
             * With KBP the default indication is a separate result field, which doesn't impose such limitation.
             */
        }
    }
    else if (port != 0)
    {
        /*
         * tgid_port field can currently only have the value of a gport pointing to a trap.
         * If port_tgid value is not a gport or it's not a trap destination, then return an error.
         */
        if (BCM_GPORT_IS_SET(port) && !BCM_GPORT_IS_TRAP(port))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "IPMC entry gport destination different than trap is not supported.\n");
        }
        else if (!BCM_GPORT_IS_SET(port))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "IPMC entry destination for field tgid_port different than gport is not supported.\n");
        }

        /*
         * tgid_port is only valid for EM and TCAM entries.
         * If it is a LPM entry, return an error.
         */
        if (!IPMC_IS_IPV4_HOST_ENTRY(data, kaps_public_capacity) && !_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_TCAM))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "IPMC port destination is only valid for EM and TCAM table entries\n");
        }
    }

    /** If the data structure describes a TCAM entry, then its priority needs to be valid. */
    if (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_TCAM))
    {
        /** Verify that the priority value is in range */
        if (data->priority == DNX_FIELD_TCAM_ENTRY_PRIORITY_INVALID
            || data->priority >= dnx_data_field.tcam.max_tcam_priority_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Priority value %d for TCAM entry is invalid - should be bigger than %d and should be less or equal than %d.\n",
                         data->priority, DNX_FIELD_TCAM_ENTRY_PRIORITY_INVALID,
                         dnx_data_field.tcam.max_tcam_priority_get(unit));
        }
    }
    else if (data->priority != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_IPMC_TCAM flag is not provided, but priority is defined\n");
    }
    /*
     * Statistics check
     */
    SHR_IF_ERR_EXIT(dnx_stat_pp_crps_verify
                    (unit, BCM_CORE_ALL, data->stat_id, data->stat_pp_profile,
                     bcmStatCounterInterfaceIngressReceivePp));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set the fields in the key for an IPMC LMP route table
 *   entry.
 * \param [in] unit - The unit number.
 * \param [in] data - contains valid fields for LPM Public and
 *        Private tables
 * \param [in] entry_handle_id - a handle to the LPM table
 *        (public or private based on the data->vrf value)
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_ipmc_add
 *  * bcm_dnx_ipmc_remove
 */
static shr_error_e
dnx_ipmc_entry_lpm_key_set(
    int unit,
    bcm_ipmc_addr_t * data,
    uint32 *entry_handle_id)
{
    int kaps_public_capacity;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * If the VRF value is > 0 or only a private lookup is set, then the entry belongs to a private table.
     * In the case of private table the VRF value is used in key construction.
     * If the VRF is equal to 0, then the VRF value is NOT used in key construction.
     * Capacity of MDB table = 0 means that only a single private lookup is going to be carried out.
     */
    SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_KAPS_2, &kaps_public_capacity));
    if (data->vrf > 0 || kaps_public_capacity == 0)
    {
        dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_VRF, data->vrf);
    }
    /*
     * If the BCM_IPMC_IP6 flag is set, then the IPv6 address (mc_ip6_addr) will be set as a key field of the entry.
     * Otherwise, valid fields for the key of the entry are the mc_ip_addr and s_ip_addr.
     * A mask will be applied to each valid key field.
     */
    if (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_IP6))
    {
        dbal_entry_key_field_arr8_masked_set(unit, *entry_handle_id, DBAL_FIELD_IPV6_GROUP, data->mc_ip6_addr,
                                             data->mc_ip6_mask);
        dbal_entry_key_field32_masked_set(unit, *entry_handle_id, DBAL_FIELD_COMPRESSED_SI,
                                          data->s_ip_addr, data->s_ip_mask);
    }
    else
    {
        dbal_entry_key_field32_masked_set(unit, *entry_handle_id, DBAL_FIELD_IPV4_GROUP,
                                          IPMC_IPV4_GET_GROUP(data->mc_ip_addr), IPMC_IPV4_GET_GROUP(data->mc_ip_mask));
        dbal_entry_key_field32_masked_set(unit, *entry_handle_id, DBAL_FIELD_IPV4_SIP, data->s_ip_addr,
                                          data->s_ip_mask);
        /** Set the RIF key field using either the ing_intf or the vid field */
        if (data->ing_intf != BCM_IF_INVALID)
        {
            dbal_entry_key_field32_masked_set(unit, *entry_handle_id, DBAL_FIELD_GLOB_IN_LIF,
                                              BCM_L3_ITF_VAL_GET(data->ing_intf),
                                              IPMC_GET_IN_LIF_MASK(BCM_L3_ITF_VAL_GET(data->ing_intf)));
        }
        else
        {
            dbal_entry_key_field32_masked_set(unit, *entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, data->vid,
                                              IPMC_GET_IN_LIF_MASK(data->vid));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set the fields in the key for an IPMC EM host table entry.
 * \param [in] unit - The unit number.
 * \param [in] data - contains valid fields for the IPMC entry
 * \param [in] entry_handle_id - a handle to the EM private
 *        table
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_ipmc_add
 *  * bcm_dnx_ipmc_remove
 */
static shr_error_e
dnx_ipmc_entry_em_key_set(
    int unit,
    bcm_ipmc_addr_t * data,
    uint32 *entry_handle_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * The key for MC host entry is comprised by VRF, Mc IP address and ETH-RIF.
     * VRF = 0 is a valid value as long as it is a single DB check set up or it's a public entry
     * Currently only private multicast host table is supported.
     */
    dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_VRF, data->vrf);
    if (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_IP6))
    {
        dbal_entry_key_field_arr8_set(unit, *entry_handle_id, DBAL_FIELD_IPV6_GROUP, data->mc_ip6_addr);
        /** Set the RIF key field using either the ing_intf or the vid field */
        if (data->ing_intf != BCM_IF_INVALID)
        {
            dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_GLOBAL_IN_LIF_15_00,
                                       DNX_IPMC_GLOB_IN_LIF_LSB_GET(BCM_L3_ITF_VAL_GET(data->ing_intf)));
            dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_GLOBAL_IN_LIF_21_16,
                                       DNX_IPMC_GLOB_IN_LIF_MSB_GET(BCM_L3_ITF_VAL_GET(data->ing_intf)));
        }
        else
        {
            dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_GLOBAL_IN_LIF_15_00,
                                       DNX_IPMC_GLOB_IN_LIF_LSB_GET(data->vid));
            dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_GLOBAL_IN_LIF_21_16, 0);
        }
    }
    else
    {
        dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_IPV4_GROUP,
                                   IPMC_IPV4_GET_GROUP(data->mc_ip_addr));
        /** Set the RIF key field using either the ing_intf or the vid field */
        if (data->ing_intf != BCM_IF_INVALID)
        {
            dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_GLOB_IN_LIF,
                                       BCM_L3_ITF_VAL_GET(data->ing_intf));
        }
        else
        {
            dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, data->vid);
        }
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set the fields in the key for bridge entries - this
 *   includes SVL(shared vlan learning) only
 * \param [in] unit - The unit number.
 * \param [in] data - contains valid fields for the bridge entry
 *      - vid        -> VSI
 *      - mc_ip_addr -> the IPv4 MC address.
 *      - mc_ip_mask -> the mask of the IPv4 MC address.
 *      - s_ip_addr  -> the source IPv4 address - used for SSM
 *      - s_ip_mask  -> mask of the source IP (IPv4 address).
 * \param [in] entry_handle_id - a handle to the Bridge table
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  DBAL_TABLE_BRIDGE_IPV4_MULTICAST_SVL
 *  DBAL_TABLE_BRIDGE_IPV4_MULTICAST_SOURCE_SPECIFIC_SVL
 */
static shr_error_e
dnx_ipmc_entry_bridge_key_set(
    int unit,
    bcm_ipmc_addr_t * data,
    uint32 *entry_handle_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * The keys are formed as follows:
     * IPV4:
     *      * SVL - <FID, G>
     *      * SSM SVL - <FID, G, S>
     * IPV6:
     *      * SSM SVL - <FID, G, S>
     * If the entry is BRIDGE_EM, then the source IP is not used when forming the key.
     * If it is an LPM one (SSM), then both the group IP and the source IP are used with their accorded masks.
     */
    if (!_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_IP6))
    {

        if (IPMC_IS_BRIDGE_EM_ENTRY(data))
        {
            dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_IPV4_GROUP,
                                       IPMC_IPV4_GET_GROUP(data->mc_ip_addr));
            dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_VSI, data->vid);
        }
        else
        {
            dbal_entry_key_field32_masked_set(unit, *entry_handle_id, DBAL_FIELD_IPV4_GROUP,
                                              IPMC_IPV4_GET_GROUP(data->mc_ip_addr),
                                              IPMC_IPV4_GET_GROUP(data->mc_ip_mask));
            dbal_entry_key_field32_masked_set(unit, *entry_handle_id, DBAL_FIELD_IPV4_SIP, data->s_ip_addr,
                                              data->s_ip_mask);
            dbal_entry_key_field32_masked_set(unit, *entry_handle_id, DBAL_FIELD_VSI, data->vid,
                                              IPMC_GET_IN_LIF_MASK(data->vid));
        }
    }
    else
    {
        dbal_entry_key_field_arr8_masked_set(unit, *entry_handle_id, DBAL_FIELD_IPV6_GROUP,
                                             data->mc_ip6_addr, data->mc_ip6_mask);
        dbal_entry_key_field16_masked_set(unit, *entry_handle_id, DBAL_FIELD_COMPRESSED_S,
                                          data->s_ip_addr, data->s_ip_mask);
        dbal_entry_key_field32_masked_set(unit, *entry_handle_id, DBAL_FIELD_VSI, data->vid,
                                          IPMC_GET_IN_LIF_MASK(data->vid));
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set the fields in the key for TCAM entries
 * \param [in] unit - The unit number.
 * \param [in] dbal_table - dbal table (required by KBP)
 * \param [in] data - contains valid fields for the entry
 * \param [in] entry_handle_id - a handle to the TCAM table
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  DBAL_TABLE_IPV4_MULTICAST_TCAM_FORWARD
 *  DBAL_TABLE_IPV6_MULTICAST_TCAM_FORWARD
 */
static shr_error_e
dnx_ipmc_entry_tcam_key_set(
    int unit,
    uint32 dbal_table,
    bcm_ipmc_addr_t * data,
    uint32 *entry_handle_id)
{
    uint32 ipv4_group = IPMC_TABLE_IS_KBP(dbal_table) ? DBAL_FIELD_IPV4_DIP : DBAL_FIELD_IPV4_GROUP;
    uint32 ipv6_group = IPMC_TABLE_IS_KBP(dbal_table) ? DBAL_FIELD_IPV6_DIP : DBAL_FIELD_IPV6_GROUP;
    uint32 glob_in_lif = IPMC_TABLE_IS_KBP(dbal_table) ? DBAL_FIELD_KBP_GLOB_IN_LIF : DBAL_FIELD_GLOB_IN_LIF;
    uint32 glob_in_lif_ipv6 = IPMC_TABLE_IS_KBP(dbal_table) ?
        DBAL_FIELD_KBP_GLOBAL_IN_LIF_21_16 : DBAL_FIELD_GLOBAL_IN_LIF_21_16;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Supported entry keys are:
     *      * <VRF, DIP, SIP, RIF>
     * Where the VRF, DIP and RIF are full masked while the SIP is partially masked.
     */
    dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_VRF, data->vrf);
    /*
     * If the BCM_IPMC_IP6 flag is set, then the IPv6 address (mc_ip6_addr) will be set as a key field of the entry.
     * Otherwise, valid fields for the key of the entry are the mc_ip_addr and s_ip_addr.
     * A mask will be applied to each valid key field.
     */
    if (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_IP6))
    {
        dbal_entry_key_field_arr8_masked_set(unit, *entry_handle_id, ipv6_group, data->mc_ip6_addr, data->mc_ip6_mask);
        dbal_entry_key_field_arr8_masked_set(unit, *entry_handle_id, DBAL_FIELD_IPV6_SIP, data->s_ip6_addr,
                                             data->s_ip6_mask);
        if (IPMC_TABLE_IS_KBP(dbal_table))
        {
            if (data->ing_intf != BCM_IF_INVALID)
            {
                dbal_entry_key_field32_masked_set(unit, *entry_handle_id, DBAL_FIELD_GLOBAL_IN_LIF_15_00,
                                                  DNX_IPMC_GLOB_IN_LIF_LSB_GET(BCM_L3_ITF_VAL_GET(data->ing_intf)),
                                                  IPMC_GET_IN_LIF_MASK(DNX_IPMC_GLOB_IN_LIF_LSB_GET
                                                                       (BCM_L3_ITF_VAL_GET(data->ing_intf))));
                dbal_entry_key_field32_masked_set(unit, *entry_handle_id, glob_in_lif_ipv6,
                                                  DNX_IPMC_GLOB_IN_LIF_MSB_GET(BCM_L3_ITF_VAL_GET(data->ing_intf)),
                                                  IPMC_GET_IN_LIF_MASK(DNX_IPMC_GLOB_IN_LIF_MSB_GET
                                                                       (BCM_L3_ITF_VAL_GET(data->ing_intf))));
            }
            else
            {
                dbal_entry_key_field32_masked_set(unit, *entry_handle_id, DBAL_FIELD_GLOBAL_IN_LIF_15_00,
                                                  DNX_IPMC_GLOB_IN_LIF_LSB_GET(data->vid),
                                                  IPMC_GET_IN_LIF_MASK(data->vid));
                dbal_entry_key_field32_masked_set(unit, *entry_handle_id, glob_in_lif_ipv6, 0, 0);
            }
        }
        else
        {
            /** Set the RIF key field using either the ing_intf or the vid field */
            if (data->ing_intf != BCM_IF_INVALID)
            {
                dbal_entry_key_field32_masked_set(unit, *entry_handle_id, glob_in_lif,
                                                  BCM_L3_ITF_VAL_GET(data->ing_intf),
                                                  IPMC_GET_IN_LIF_MASK(BCM_L3_ITF_VAL_GET(data->ing_intf)));
            }
            else
            {
                dbal_entry_key_field32_masked_set(unit, *entry_handle_id, glob_in_lif, data->vid,
                                                  IPMC_GET_IN_LIF_MASK(data->vid));
            }
        }
    }
    else
    {
        dbal_entry_key_field32_masked_set(unit, *entry_handle_id, ipv4_group,
                                          IPMC_IPV4_GET_GROUP(data->mc_ip_addr), IPMC_IPV4_GET_GROUP(data->mc_ip_mask));
        dbal_entry_key_field32_masked_set(unit, *entry_handle_id, DBAL_FIELD_IPV4_SIP, data->s_ip_addr,
                                          data->s_ip_mask);
        /** Set the RIF key field using either the ing_intf or the vid field */
        if (data->ing_intf != BCM_IF_INVALID)
        {
            dbal_entry_key_field32_masked_set(unit, *entry_handle_id, glob_in_lif,
                                              BCM_L3_ITF_VAL_GET(data->ing_intf),
                                              IPMC_GET_IN_LIF_MASK(BCM_L3_ITF_VAL_GET(data->ing_intf)));
        }
        else
        {
            dbal_entry_key_field32_masked_set(unit, *entry_handle_id, glob_in_lif, data->vid,
                                              IPMC_GET_IN_LIF_MASK(data->vid));
        }
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Determine the field of the result in use based on the defined fields - group, l3a_intf and port_tgid.
 *   If none of those have been defined, then return an error.
 *   It is expected that the port_tgid will have a value of a trap gport.
 * \param [in] unit - The unit number.
 * \param [in] data - contains valid fields for the entry
 * \param [in] entry_handle_id - a handle to the selected table
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_ipmc_entry_result_destination_set(
    int unit,
    bcm_ipmc_addr_t * data,
    uint32 *entry_handle_id)
{
    uint32 fec_index = BCM_L3_ITF_VAL_GET(data->l3a_intf);
    uint32 mc_group = _BCM_MULTICAST_ID_GET(data->group);
    uint32 port = data->port_tgid;

    SHR_FUNC_INIT_VARS(unit);

    if (data->group != 0)
    {
        dbal_entry_value_field32_set(unit, *entry_handle_id, DBAL_FIELD_MC_ID, INST_SINGLE, mc_group);
    }
    else if (data->l3a_intf != 0)
    {
        dbal_entry_value_field32_set(unit, *entry_handle_id, DBAL_FIELD_FEC, INST_SINGLE, fec_index);
    }
    else if (data->port_tgid != 0 && BCM_GPORT_IS_TRAP(data->port_tgid))
    {
        SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                        (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, data->port_tgid, &port));
        dbal_entry_value_field32_set(unit, *entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, port);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "No result field defined - expecting either MC group (data->group), FEC (data->l3a_intf) or Port (data->port_tgid).\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Configure the result for the IPMC entry based on its type:
 *   EM, LPM, TCAM.
 * \param [in] unit - The unit number.
 * \param [in] data - contains valid fields for the bridge entry
 * \param [in] dbal_table - dbal table (required by KBP)
 * \param [in] entry_handle_id - a handle to the selected table
 * \param [in] dbal_table_type - Type of the selected MDB table
 *        LPM, EM or TCAM
 * \param [out] result_type - Dbal table result type
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark
 *   * This procedure assumes that verification was already carried out by having called
 *     dnx_ipmc_dbal_table_select(). This means that if the 'raw' flag is set, we only
 *     get here for the 'lpm' table case.
 * \see
 *  * dnx_ipmc_dbal_table_select
 *  * bcm_dnx_ipmc_add
 *  * BCM_IPMC_RAW_ENTRY
 */
static shr_error_e
dnx_ipmc_entry_result_set(
    int unit,
    bcm_ipmc_addr_t * data,
    uint32 dbal_table,
    uint32 *entry_handle_id,
    uint32 dbal_table_type,
    uint32 *result_type)
{
    uint32 fec_index;
    uint32 mc_group;

    SHR_FUNC_INIT_VARS(unit);

    fec_index = BCM_L3_ITF_VAL_GET(data->l3a_intf);
    mc_group = _BCM_MULTICAST_ID_GET(data->group);

    /*
     * KBP tables behave the same way TCAM tables do, but the implementation differ.
     */
    if (IPMC_TABLE_IS_KBP(dbal_table))
    {
        if (data->stat_pp_profile == STAT_PP_PROFILE_INVALID)
        {
            *result_type = IPMC_KBP_TABLE_TO_RESULT_TYPE_FWD_DEST(dbal_table);
            dbal_entry_value_field32_set(unit, *entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, *result_type);
            dbal_entry_value_field8_set(unit, *entry_handle_id, DBAL_FIELD_SOURCE_DROP, INST_SINGLE, FALSE);
        }
        else
        {
            if (dbal_table == DBAL_TABLE_KBP_IPV4_MULTICAST_TCAM_FORWARD)
            {
                dbal_entry_value_field32_set(unit, *entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                             DBAL_RESULT_TYPE_KBP_IPV4_MULTICAST_TCAM_FORWARD_KBP_FORWARD_DEST_STAT_W_DEFAULT);
            }
            else if (dbal_table == DBAL_TABLE_KBP_IPV6_MULTICAST_TCAM_FORWARD)
            {
                dbal_entry_value_field32_set(unit, *entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                             DBAL_RESULT_TYPE_KBP_IPV6_MULTICAST_TCAM_FORWARD_KBP_FORWARD_DEST_STAT_W_DEFAULT);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "The provided dbal table [%s] does not support STAT\n",
                             dbal_logical_table_to_string(unit, dbal_table));
            }
            dbal_entry_value_field32_set(unit, *entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE, data->stat_id);
            dbal_entry_value_field32_set(unit, *entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE,
                                         data->stat_pp_profile);
        }

        if (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_RAW_ENTRY))
        {
            dbal_entry_value_field32_set(unit, *entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, fec_index);
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_ipmc_entry_result_destination_set(unit, data, entry_handle_id));
        }
        if (IPMC_IS_IPV4_DEFAULT_ENTRY(data)
            || (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_IP6)
                && dnx_ipmc_ipv6_default_address(data->s_ip6_addr, data->s_ip6_mask)
                && dnx_ipmc_ipv6_default_address(data->mc_ip6_addr, data->mc_ip6_mask))
            || _SHR_IS_FLAG_SET(data->flags, BCM_IPMC_DEFAULT_ENTRY))
        {
            dbal_entry_value_field32_set(unit, *entry_handle_id, DBAL_FIELD_IS_DEFAULT, INST_SINGLE, TRUE);
        }
        else
        {
            dbal_entry_value_field32_set(unit, *entry_handle_id, DBAL_FIELD_IS_DEFAULT, INST_SINGLE, FALSE);
        }
        dbal_entry_value_field32_set(unit, *entry_handle_id, DBAL_FIELD_KBP_FORWARD_PMF_GENERIC_DATA, INST_SINGLE, 0);
    }
    /*
     * Build the result fields and the result type (if needed) based on the type of the dbal table.
     *      * LPM  - MC_ID_KAPS / FEC / FEC_DEFAULT / KAPS_RESULT
     *      * EM   - MC_ID / FEC + result type
     *      * TCAM - DESTINATION + result type
     */
    else if (dbal_table_type == DBAL_TABLE_TYPE_LPM)
    {
        /*
         * Determine the result field based on whether the group field is defined or the l3a_intf.
         * If neither, then return an error.
         * However, if the 'raw' flag is set, then just use 'l3a_intf' (Actually use 'fec_index' which is, efectively,
         * the same).
         */

        if (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_RAW_ENTRY))
        {
            dbal_entry_value_field32_set(unit, *entry_handle_id, DBAL_FIELD_KAPS_RESULT, INST_SINGLE, fec_index);
        }
        else
        {
            if (data->group != 0)
            {
                dbal_entry_value_field32_set(unit, *entry_handle_id, DBAL_FIELD_MC_ID_KAPS, INST_SINGLE, mc_group);
            }
            else if (data->l3a_intf != 0
                     && ((IPMC_IS_IPV4_FULL_MASKING(data->mc_ip_mask) && !_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_IP6))
                         || (IPMC_IS_IPV6_FULL_MASKING(data->mc_ip6_mask)
                             && _SHR_IS_FLAG_SET(data->flags, BCM_IPMC_IP6))
                         || _SHR_IS_FLAG_SET(data->flags, BCM_IPMC_DEFAULT_ENTRY)))
            {
                dbal_entry_value_field32_set(unit, *entry_handle_id, DBAL_FIELD_FEC_DEFAULT, INST_SINGLE, fec_index);
            }
            else if (data->l3a_intf != 0)
            {
                dbal_entry_value_field32_set(unit, *entry_handle_id, DBAL_FIELD_FEC, INST_SINGLE, fec_index);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "No result field defined - expecting either MC group (data->group) or FEC defined (data->l3a_intf).\n");
            }
        }
        *result_type = 0;
    }
    else
    {
        if (dbal_table_type == DBAL_TABLE_TYPE_EM)
        {
            /*
             * Fill in the result type based on the flags that have been provided.
             */
            if (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_L2))
            {
                *result_type =
                    (data->stat_pp_profile !=
                     STAT_PP_PROFILE_INVALID) ? DBAL_RESULT_TYPE_BRIDGE_IPV4_MULTICAST_SVL_FWD_DEST_STAT :
                    DBAL_RESULT_TYPE_BRIDGE_IPV4_MULTICAST_SVL_FWD_DEST;
                dbal_entry_value_field32_set(unit, *entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, *result_type);
            }
            else if (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_IP6))
            {
                *result_type =
                    (data->stat_pp_profile !=
                     STAT_PP_PROFILE_INVALID) ? DBAL_RESULT_TYPE_IPV6_MULTICAST_EM_FORWARD_FWD_DEST_STAT :
                    DBAL_RESULT_TYPE_IPV6_MULTICAST_EM_FORWARD_FWD_DEST;
                dbal_entry_value_field32_set(unit, *entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, *result_type);
            }
            else
            {
                *result_type =
                    (data->stat_pp_profile !=
                     STAT_PP_PROFILE_INVALID) ? DBAL_RESULT_TYPE_IPV4_MULTICAST_EM_FORWARD_FWD_DEST_STAT :
                    DBAL_RESULT_TYPE_IPV4_MULTICAST_EM_FORWARD_FWD_DEST;
                dbal_entry_value_field32_set(unit, *entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, *result_type);
            }
        }
        else if (dbal_table_type == DBAL_TABLE_TYPE_TCAM)
        {
            /*
             * Fill in the result type for a TCAM entry.
             */
            if (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_IP6))
            {
                *result_type =
                    (data->stat_pp_profile !=
                     STAT_PP_PROFILE_INVALID) ? DBAL_RESULT_TYPE_IPV6_MULTICAST_TCAM_FORWARD_FWD_DEST_STAT :
                    DBAL_RESULT_TYPE_IPV6_MULTICAST_TCAM_FORWARD_FWD_DEST;
            }
            else
            {
                *result_type =
                    (data->stat_pp_profile !=
                     STAT_PP_PROFILE_INVALID) ? DBAL_RESULT_TYPE_IPV4_MULTICAST_TCAM_FORWARD_FWD_DEST_STAT :
                    DBAL_RESULT_TYPE_IPV4_MULTICAST_TCAM_FORWARD_FWD_DEST;
            }
            dbal_entry_value_field32_set(unit, *entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, *result_type);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Incorrect table type found for L3 MC table - %d\n", dbal_table_type);
        }

        SHR_IF_ERR_EXIT(dnx_ipmc_entry_result_destination_set(unit, data, entry_handle_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Determine which DBAL MC table the entry belongs to based on
 *   the input data.
 * \param [in] unit - The unit number.
 * \param [in] data - contains valid fields for the IPMC entry
 * \param [out] dbal_table - the name of the table that the
 *        entry belongs to.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * This procedure verifies that the 'raw' flag (in data->flags) is only set for
 *     LPM tables. If it is set (by the caller) on non-LPM tables, an error is injected!
 * \see
 *  * bcm_dnx_ipmc_add
 *  * bcm_dnx_ipmc_remove
 *  * BCM_IPMC_IP6
 *  * BCM_IPMC_RAW_ENTRY
 */
static shr_error_e
dnx_ipmc_dbal_table_select(
    int unit,
    bcm_ipmc_addr_t * data,
    uint32 *dbal_table)
{
    int allow_raw;
    int kaps_public_capacity;
    uint32 kbp_mngr_status;
    uint8 use_kbp_ipv4 = FALSE;
    uint8 use_kbp_ipv6 = FALSE;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_KAPS_2, &kaps_public_capacity));
    /*
     * 'allow_raw' is set to a non-zero value if the table relates to 'LPM'.
     * Note that only LPM and KBP tables are allowed when 'raw' flag (BCM_IPMC_RAW_ENTRY)
     * is set.
     */
    allow_raw = 0;

    /** KBP doesn't support IPMC bridge; Do not check for KBP if BCM_IPMC_L2 is set */
    if (!_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_L2))
    {
        /*
         * Test if KBP is enabled for MC application and use the KBP tables
         */
        SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));
        if (kbp_mngr_status != DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_BEFORE_INIT)
        {
            if (dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv4)
                && !(_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_IP6)))
            {
                if (kbp_mngr_status != DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED)
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG, "KBP device in use, but not locked\n");
                }
                else if (!_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_TCAM))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "KBP IPMC entries are TCAM, need to use BCM_IPMC_TCAM flag\n");
                }
                use_kbp_ipv4 = TRUE;
            }
            if (dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv6)
                && (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_IP6)))
            {
                if (kbp_mngr_status != DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED)
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG, "KBP device in use, but not locked\n");
                }
                else if (!_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_TCAM))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "KBP IPMC entries are TCAM, need to use BCM_IPMC_TCAM flag\n");
                }
                use_kbp_ipv6 = TRUE;
            }
        }
    }

    /*
     * If the BCM_IPMC_IP6 flag is provided, then an IPv6 entry will be created. It can be an EM, TCAM or LPM.
     * If the BCM_IPMC_L2 flag is added, then a bridge entry will be created. It can be EM or LPM.
     * It can be SVL(shared vlan learning) only
     * If no flag has been added, then an IPv4 entry will be created.
     */
    if (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_IP6))
    {
        if (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_TCAM))
        {
            if (use_kbp_ipv6)
            {
                *dbal_table = DBAL_TABLE_KBP_IPV6_MULTICAST_TCAM_FORWARD;
                allow_raw = 1;
            }
            else
            {
                *dbal_table = DBAL_TABLE_IPV6_MULTICAST_TCAM_FORWARD;
            }
        }
        else if (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_L2))
        {
            if (use_kbp_ipv6)
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "KBP Tables for Bridge IPV6 is not yet supported. \n");
            }
            *dbal_table = DBAL_TABLE_BRIDGE_IPV6_MULTICAST_SOURCE_SPECIFIC_SVL;
        }
        else if (IPMC_IS_IPV6_HOST_ENTRY(data, kaps_public_capacity))
        {
            *dbal_table = DBAL_TABLE_IPV6_MULTICAST_EM_FORWARD;
        }
        else
        {
            *dbal_table = DBAL_TABLE_IPV6_MULTICAST_PRIVATE_LPM_FORWARD;
            allow_raw = 1;
        }
    }
    else if (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_L2))
    {
        /*
         * If the IPv4 group IP address is not masked and the source IP is fully masked, then an EM entry will created.
         * Otherwise, if the mask <FID, G, S> is consistent, then it is an LPM SSM (source specific multicast) entry.
         */
        if (IPMC_IS_BRIDGE_EM_ENTRY(data))
        {
            *dbal_table = DBAL_TABLE_BRIDGE_IPV4_MULTICAST_SVL;
        }
        else
        {
            *dbal_table = DBAL_TABLE_BRIDGE_IPV4_MULTICAST_SOURCE_SPECIFIC_SVL;
        }
    }
    else
    {
        /*
         * In case the BCM_IPMC_TCAM flag is added, a TCAM entry will be created regardless of key masking.
         * In case the In-ETH-RIF is valid, the SIP is fully masked and the IPv4 group is fully unmasked,
         * (which means that this isn't a valid LPM <VRF, G, SIP, RIF> entry)
         * assume that this is a host entry in the EM table and form key <VRF, G, RIF).
         * Otherwise (else clause) this is a route entry and will be added to the LPM table.
         */
        if (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_TCAM))
        {
            if (use_kbp_ipv4)
            {
                *dbal_table = DBAL_TABLE_KBP_IPV4_MULTICAST_TCAM_FORWARD;
                allow_raw = 1;
            }
            else
            {
                *dbal_table = DBAL_TABLE_IPV4_MULTICAST_TCAM_FORWARD;
            }
        }
        else if (IPMC_IS_IPV4_HOST_ENTRY(data, kaps_public_capacity))
        {
            *dbal_table = DBAL_TABLE_IPV4_MULTICAST_EM_FORWARD;
        }
        else
        {
            /*
             * Will take handle to an IPv4 MC LPM FORWARD table entry.
             * If the vrf field or single DB lookup is defined, a handle to the private MC table will be allocated.
             * Otherwise, a handle to the public MC table will be taken.
             */
            if (data->vrf > 0 || kaps_public_capacity == 0)
            {
                *dbal_table = DBAL_TABLE_IPV4_MULTICAST_PRIVATE_LPM_FORWARD;
                allow_raw = 1;
            }
            else
            {
                *dbal_table = DBAL_TABLE_IPV4_MULTICAST_PUBLIC_LPM_FORWARD;
                allow_raw = 1;
            }
        }
    }

    if (allow_raw == 0)
    {
        /*
         * Only LPM tables are allowed when 'raw' flag (BCM_IPMC_RAW_ENTRY)
         * is set.
         */
        if (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_RAW_ENTRY))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "Only LPM and KBP tables are allowed when 'raw' flag (BCM_IPMC_RAW_ENTRY = 0x%08X) is set.\r\n"
                         "data->flags = 0x%08X, dbal_table = %d. See 'dbal_tables_e'\r\n",
                         (unsigned int) BCM_IPMC_RAW_ENTRY, (unsigned int) (data->flags), (int) (*dbal_table));
        }
    }
    else
    {
        if (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_RAW_ENTRY))
        {
            uint32 field_size;
            int table_field_size;
            uint32 dbal_field = (IPMC_TABLE_IS_KBP(*dbal_table) ? DBAL_FIELD_DESTINATION : DBAL_FIELD_KAPS_RESULT);

            /*
             * Validate the result size using the field size (KAPS_RESULT 20 bits; DESTINATION 21 bits).
             * Also check the table field size in case it's truncated.
             */
            SHR_IF_ERR_EXIT(dbal_fields_max_size_get(unit, dbal_field, &field_size));
            SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                            (unit, (*dbal_table), dbal_field, FALSE, 0, INST_SINGLE, &table_field_size));
            /** Take the smaller of the two sizes */
            field_size = (field_size > table_field_size ? table_field_size : field_size);
            if (BCM_L3_ITF_VAL_GET(data->l3a_intf) >= SAL_BIT(field_size))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Raw value for 'FEC' (%d) is out of range, should be lower than %d.",
                             BCM_L3_ITF_VAL_GET(data->l3a_intf), SAL_BIT(field_size));
            }
        }
        else if (data->l3a_intf != 0)
        {
            /*
             * If the entry is an LPM/TCAM one, it can have a result of default FEC.
             * The result type is default FEC when the key of the entry is fully masked.
             * Default FEC has a different range of valid values.
             * Whether the entry is fully masked is decided for each IPv4/v6 type
             * based on the mask of the fields that form the entry key.
             */
            /*
             * In case of KBP this check is not required, because for KBP default indication is not part of the FEC.
             * The indication is a separate result field, which doesn't limit the range of FECs.
             */
            if (!IPMC_TABLE_IS_KBP(*dbal_table))
            {
                unsigned int fec_index = BCM_L3_ITF_VAL_GET(data->l3a_intf);
                if (fec_index > dnx_data_l3.fec.max_default_fec_get(unit)
                    &&
                    ((!_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_IP6)
                      && !IPMC_IS_IPV4_HOST_ENTRY(data, kaps_public_capacity)
                      && IPMC_IS_IPV4_FULL_MASKING(data->mc_ip_mask) && IPMC_IS_IPV4_FULL_MASKING(data->s_ip_mask))
                     || (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_IP6)
                         && !IPMC_IS_IPV6_HOST_ENTRY(data, kaps_public_capacity)
                         && IPMC_IS_IPV6_FULL_MASKING(data->mc_ip6_mask))
                     || _SHR_IS_FLAG_SET(data->flags, BCM_IPMC_DEFAULT_ENTRY)))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Default FEC index %d is out of range, should be lower then %d.",
                                 fec_index, dnx_data_l3.fec.max_default_fec_get(unit));
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Resolve the result of the LPM entry. It can be a MC ID, FEC
 *   or default FEC. The result value is assigned to the
 *   appropriate field - either group or l3a_intf of the
 *   bcm_ipmc_addr_t structure.
 * \param [in] unit - The unit number.
 * \param [in] destination - The encoded destination result
 *        found in the LPM table
 * \param [out] data - the result of the IPMC entry
 *          * group    -> MC group ID
 *          * l3a_intf -> FEC index
 *          * flags      ->                                                \n
 *              Indication control flags for the IPMC entry.               \n
 *              Within that, only BCM_IPMC_RAW_ENTRY is meaningful.        \n
 *              If this procedure identifies the found entry as 'raw' then \n
 *              this flag is set. Otherwise, it is reset.                  \n
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_ipmc_find
 *  * bcm_dnx_ipmc_traverse
 */
static shr_error_e
dnx_ipmc_resolve_lpm_result(
    int unit,
    uint32 destination,
    bcm_ipmc_addr_t * data)
{
    dbal_fields_e dbal_sub_field;
    uint32 result_value;
    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_RAW_ENTRY))
    {
        /** If 'raw' flag was set at input then return extracted value as is, no encoding. */
        data->l3a_intf = destination;
    }
    else
    {
        /*
         * Get the used sub-field and its value. The sub-field ID value is assigned to dbal_sub_field variable
         * and its decoded value is assigned to result_value.
         */
        SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get
                        (unit, DBAL_FIELD_KAPS_RESULT, destination, &dbal_sub_field, &result_value));
        /*
         * Determine the bcm_ipmc_addr_t field to which to assign the result value based on the ID of the dbal_sub_field.
         * If its meaning is MC_ID_KAPS, then the result is a multicast group and is assigned to data->group.
         * If it is either FEC_DEFAULT or FEC it is assigned to the data->l3a_intf.
         */
        switch (dbal_sub_field)
        {
            case DBAL_FIELD_MC_ID_KAPS:
            {
                data->group = result_value;
                break;
            }
            case DBAL_FIELD_FEC:
            {
                if (result_value >= dnx_data_l3.fec.nof_fecs_get(unit))
                {
                    data->flags |= BCM_IPMC_RAW_ENTRY;
                }
                BCM_L3_ITF_SET(data->l3a_intf, BCM_L3_ITF_TYPE_FEC, result_value);
                break;
            }
            case DBAL_FIELD_FEC_DEFAULT:
            {
                if (result_value > dnx_data_l3.fec.max_default_fec_get(unit))
                {
                    data->flags |= BCM_IPMC_RAW_ENTRY;
                }
                data->flags |= BCM_IPMC_DEFAULT_ENTRY;
                BCM_L3_ITF_SET(data->l3a_intf, BCM_L3_ITF_TYPE_FEC, result_value);
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal sub-field with ID %d found as result of IPMC route entry.\n",
                             dbal_sub_field);
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Resolve the result of an EM or a TCAM entry. It can be a
 *   MC ID or a FEC. The result value is assigned to the
 *   appropriate field: either group or l3a_intf of the
 *   bcm_ipmc_addr_t
 *     structure.
 * \param [in] unit - The unit number.
 * \param [in] destination - The encoded destination result
 *        found in the LPM table
 * \param [in] dbal_table - dbal table (required by KBP)
 * \param [out] data - the result of the IPMC entry
 *          - group    -> MC group ID
 *          - l3a_intf -> FEC index
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_ipmc_find
 *  * bcm_dnx_ipmc_traverse
 */
static shr_error_e
dnx_ipmc_resolve_entry_destination(
    int unit,
    uint32 destination,
    uint32 dbal_table,
    bcm_ipmc_addr_t * data)
{
    dbal_fields_e dbal_sub_field;
    uint32 result_value;

    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_RAW_ENTRY))
    {
        if (!IPMC_TABLE_IS_KBP(dbal_table))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_IPMC_RAW_ENTRY flag is used with table %s that does not support it.\n",
                         dbal_logical_table_to_string(unit, dbal_table));
        }
        /** If 'raw' flag was set at input then return extracted value as is, no encoding. */
        data->l3a_intf = destination;
    }
    else
    {
        /*
         * Get the used sub-field of field DESTINATION that is in use and its value.
         * The sub-field ID value is assigned to dbal_sub_field variable and its decoded value is assigned to result_value.
         */
        SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get
                        (unit, DBAL_FIELD_DESTINATION, destination, &dbal_sub_field, &result_value));

        /*
         * Determine the bcm_ipmc_addr_t field to which to assign the result value based on the ID of the dbal_sub_field.
         * If its meaning is MC_ID, then the result is a multicast group and is assigned to data->group.
         * If it is FEC it is assigned to the data->l3a_intf.
         * It it is a trap destination, it is assigned to data->port_tgid.
         */
        switch (dbal_sub_field)
        {
            case DBAL_FIELD_MC_ID:
            {
                data->group = result_value;
                break;
            }
            case DBAL_FIELD_FEC:
            {
                if (IPMC_TABLE_IS_KBP(dbal_table) && (result_value >= dnx_data_l3.fec.nof_fecs_get(unit)))
                {
                    data->flags |= BCM_IPMC_RAW_ENTRY;
                }
                BCM_L3_ITF_SET(data->l3a_intf, BCM_L3_ITF_TYPE_FEC, result_value);
                break;
            }
            case DBAL_FIELD_INGRESS_TRAP_DEST:
            {
                SHR_IF_ERR_EXIT(algo_gpm_gport_from_encoded_destination_field
                                (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, destination, &data->port_tgid));
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal sub-field with ID %d found as result of IPMC host entry.\n",
                             dbal_sub_field);
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Extract the values of the key fields of a bridge entry and
 *     assign them to a bcm_ipmc_addr_t structure.
 * \param [in] unit - The unit number.
 * \param [in] entry_handle_id - a handle to the table entry
 * \param [in] dbal_table - the ID of the dbal table to which
 *        the entry belongs
 * \param [out] dbal_entry - the result of the IPMC entry
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_ipmc_traverse
 */
static shr_error_e
dnx_ipmc_bridge_key_field_get(
    int unit,
    uint32 entry_handle_id,
    uint32 dbal_table,
    bcm_ipmc_addr_t * dbal_entry)
{
    uint32 field_value[1];
    uint32 field_value_mask[1];

    SHR_FUNC_INIT_VARS(unit);

    dbal_entry->flags |= BCM_IPMC_L2;

    if (dbal_table == DBAL_TABLE_BRIDGE_IPV4_MULTICAST_SVL
        || dbal_table == DBAL_TABLE_BRIDGE_IPV4_MULTICAST_SOURCE_SPECIFIC_SVL)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_IPV4_GROUP, field_value));
        dbal_entry->mc_ip_addr = field_value[0];
        /*
         * If the table is an LPM bridge table, then the SIP is also part of the key and needs to be received.
         */
        if (dbal_table == DBAL_TABLE_BRIDGE_IPV4_MULTICAST_SOURCE_SPECIFIC_SVL)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_IPV4_SIP, field_value));
            dbal_entry->s_ip_addr = field_value[0];
        }
    }
    else
    {
        dbal_entry->flags |= BCM_IPMC_IP6;
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_masked_get
                        (unit, entry_handle_id, DBAL_FIELD_IPV6_GROUP, dbal_entry->mc_ip6_addr,
                         dbal_entry->mc_ip6_mask));
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                        (unit, entry_handle_id, DBAL_FIELD_COMPRESSED_S, field_value, field_value_mask));
        dbal_entry->s_ip_addr = field_value[0];
        dbal_entry->s_ip_mask = field_value_mask[0];
    }
    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_VSI, field_value));
    dbal_entry->vid = field_value[0];

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Extract the values of the key fields of an IPv6 entry and
 *     assign them to a bcm_ipmc_addr_t structure.
 * \param [in] unit - The unit number.
 * \param [in] entry_handle_id - a handle to the table entry
 * \param [in] dbal_table - the ID of the dbal table to which
 *        the entry belongs
 * \param [out] dbal_entry - the result of the IPMC entry
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_ipmc_traverse
 */
static shr_error_e
dnx_ipmc_ipv6_key_field_get(
    int unit,
    uint32 entry_handle_id,
    uint32 dbal_table,
    bcm_ipmc_addr_t * dbal_entry)
{
    uint32 field_value_lsb;
    uint32 field_value[1];
    uint32 field_value_mask[1];
    bcm_ip6_t empty_mask =
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    SHR_FUNC_INIT_VARS(unit);

    dbal_entry->flags |= BCM_IPMC_IP6;

    if (dbal_table == DBAL_TABLE_IPV6_MULTICAST_PRIVATE_LPM_FORWARD)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                        (unit, entry_handle_id, DBAL_FIELD_COMPRESSED_SI, field_value, field_value_mask));
        dbal_entry->s_ip_addr = field_value[0];
        dbal_entry->s_ip_mask = field_value_mask[0];
    }
    else
    {
        if (dbal_table == DBAL_TABLE_IPV6_MULTICAST_TCAM_FORWARD)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, field_value));
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_GLOBAL_IN_LIF_15_00, &field_value_lsb));
            if (IPMC_TABLE_IS_KBP(dbal_table))
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_KBP_GLOBAL_IN_LIF_21_16, field_value));
                field_value[0] =
                    IPMC_KBP_IN_LIF_SET(((field_value[0] << IPMC_IPV6_GLOB_IN_LIF_NOF_LSB) | field_value_lsb));
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_GLOBAL_IN_LIF_21_16, field_value));
                field_value[0] = (field_value[0] << IPMC_IPV6_GLOB_IN_LIF_NOF_LSB) | field_value_lsb;
            }
        }

        /** Choose the field to which the GLOB_IN_LIF value should be assigned to. */
        if (field_value[0] >= dnx_data_l3.rif.nof_rifs_get(unit))
        {
            dbal_entry->ing_intf = field_value[0];
        }
        else
        {
            dbal_entry->vid = field_value[0];
        }
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_VRF, field_value));
    dbal_entry->vrf = field_value[0];
    if (dbal_table != DBAL_TABLE_IPV6_MULTICAST_EM_FORWARD)
    {
        uint32 ipv6_group = IPMC_TABLE_IS_KBP(dbal_table) ? DBAL_FIELD_IPV6_DIP : DBAL_FIELD_IPV6_GROUP;

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_masked_get
                        (unit, entry_handle_id, ipv6_group, dbal_entry->mc_ip6_addr, dbal_entry->mc_ip6_mask));
        if ((dbal_table == DBAL_TABLE_IPV6_MULTICAST_TCAM_FORWARD) ||
            (dbal_table == DBAL_TABLE_KBP_IPV6_MULTICAST_TCAM_FORWARD))
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_masked_get(unit, entry_handle_id, DBAL_FIELD_IPV6_SIP,
                                                                        dbal_entry->s_ip6_addr,
                                                                        dbal_entry->s_ip6_mask));
            dbal_entry->flags |= BCM_IPMC_TCAM;
        }
        dbal_entry->mc_ip6_mask[0] = IPMC_MC_IPV6_ADDRESS_PREFIX;
        dbal_entry->mc_ip6_mask[1] = IPMC_MC_IPV6_ADDRESS_PREFIX;
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_get
                        (unit, entry_handle_id, DBAL_FIELD_IPV6_GROUP, dbal_entry->mc_ip6_addr));
        sal_memcpy(dbal_entry->mc_ip6_mask, empty_mask, sizeof(bcm_ip6_t));
    }
    dbal_entry->mc_ip6_addr[0] = IPMC_MC_IPV6_ADDRESS_PREFIX;
    dbal_entry->mc_ip6_addr[1] = IPMC_MC_IPV6_ADDRESS_PREFIX;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Extract the values of the key fields of an IPv4 host or
 *     route entry and assign them to a bcm_ipmc_addr_t
 *     structure.
 * \param [in] unit - The unit number.
 * \param [in] entry_handle_id - a handle to the table entry
 * \param [in] dbal_table - the ID of the dbal table to which
 *        the entry belongs
 * \param [out] dbal_entry - the result of the IPMC entry
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_ipmc_traverse
 */
static shr_error_e
dnx_ipmc_ipv4_key_field_get(
    int unit,
    uint32 entry_handle_id,
    uint32 dbal_table,
    bcm_ipmc_addr_t * dbal_entry)
{
    uint32 field_value[1];
    uint32 field_mask[1];
    SHR_FUNC_INIT_VARS(unit);

    if (IPMC_TABLE_IS_KBP(dbal_table))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                        (unit, entry_handle_id, DBAL_FIELD_IPV4_DIP, field_value, field_mask));
        dbal_entry->mc_ip_addr = IPMC_KBP_IPV4_GROUP_SET(IPMC_IPV4_SET_GROUP(field_value[0]));
        dbal_entry->mc_ip_mask = IPMC_KBP_IPV4_GROUP_SET(IPMC_IPV4_SET_GROUP(field_mask[0]));
    }
    else if (dbal_table == DBAL_TABLE_IPV4_MULTICAST_PRIVATE_LPM_FORWARD
             || dbal_table == DBAL_TABLE_IPV4_MULTICAST_PUBLIC_LPM_FORWARD
             || dbal_table == DBAL_TABLE_IPV4_MULTICAST_TCAM_FORWARD)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                        (unit, entry_handle_id, DBAL_FIELD_IPV4_GROUP, field_value, field_mask));
        dbal_entry->mc_ip_addr = IPMC_IPV4_SET_GROUP(field_value[0]);
        dbal_entry->mc_ip_mask = IPMC_IPV4_SET_GROUP_MASK(field_mask[0]);
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_IPV4_GROUP, field_value));
        dbal_entry->mc_ip_addr = IPMC_IPV4_SET_GROUP(field_value[0]);
        dbal_entry->mc_ip_mask = IPMC_IPV4_EMPTY_MASK;
    }

    /*
     * If the table is an LPM or TCAM table, then the SIP is also part of the key.
     */
    if (dbal_table == DBAL_TABLE_IPV4_MULTICAST_PRIVATE_LPM_FORWARD
        || dbal_table == DBAL_TABLE_IPV4_MULTICAST_PUBLIC_LPM_FORWARD
        || dbal_table == DBAL_TABLE_IPV4_MULTICAST_TCAM_FORWARD
        || dbal_table == DBAL_TABLE_KBP_IPV4_MULTICAST_TCAM_FORWARD)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_IPV4_SIP, field_value));
        dbal_entry->s_ip_addr = field_value[0];
        if ((dbal_table == DBAL_TABLE_IPV4_MULTICAST_TCAM_FORWARD)
            || dbal_table == DBAL_TABLE_KBP_IPV4_MULTICAST_TCAM_FORWARD)
        {
            dbal_entry->flags |= BCM_IPMC_TCAM;
        }
    }

    if (IPMC_TABLE_IS_KBP(dbal_table))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_KBP_GLOB_IN_LIF, &field_value[0]));
        field_value[0] = IPMC_KBP_IN_LIF_SET(field_value[0]);
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, field_value));
    }

    /** Choose the field to which the GLOB_IN_LIF value should be assigned to. */
    if (field_value[0] >= dnx_data_l3.rif.nof_rifs_get(unit))
    {
        dbal_entry->ing_intf = field_value[0];
    }
    else
    {
        dbal_entry->vid = field_value[0];
    }
    /*
     * If the table is a private one, the VRF field value will be taken.
     */
    if (dbal_table != DBAL_TABLE_IPV4_MULTICAST_PUBLIC_LPM_FORWARD)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_VRF, field_value));
        dbal_entry->vrf = field_value[0];
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Find the result of an IP multicast entry.
 *  \param [in] unit -
 *  The unit number.
 *  \param [in,out] data - contains the required information for
 *  uniquely identifying an IPMC entry. The following are
 *  relevant fields for finding the result of an entry:
 *      As input -                                                    \n
 *          * vrf        -> the VRF of the incoming packet.           \n
 *          * vid        -> Not the best name to the entry ETH-RIF.   \n
 *          * mc_ip_addr -> the IPv4 MC address.                      \n
 *          * mc_ip_mask -> the mask of the IPv4 MC address.          \n
 *          * s_ip_addr  -> the source IP (IPv4 address).             \n
 *          * s_ip_mask  -> mask of the source IP (IPv4 address).     \n
 *          * mc_ip6_addr, mc_ip6_mask - address and mask of the IPv6 entry                       \n
 *          * s_ip6_addr, s_ip6_mask - address and mask of the IPv6 entry (valid for TCAM only)   \n
 *          * flags      -> control flags for the IPMC entry:         \n
 *          *   * BCM_IPMC_L2 - indicate bridge entry                 \n
 *          *   * BCM_IPMC_IP6 - indicate IPv6 entry                  \n
 *      As output -                                                   \n
 *          * group      ->                                           \n
 *              MC group ID that can be used as the result of the entry.   \n
 *          * flags      ->                                                \n
 *              Indication control flags for the IPMC entry.               \n
 *              Only meaningful if BCM_IPMC_L2 is zero and corresponding   \n
 *              table is LPM:                                              \n
 *              DBAL_TABLE_IPV6_MULTICAST_PRIVATE_LPM_FORWARD              \n
 *              DBAL_TABLE_IPV4_MULTICAST_PRIVATE_LPM_FORWARD              \n
 *              DBAL_TABLE_IPV4_MULTICAST_PUBLIC_LPM_FORWARD               \n
 *              Within that, only BCM_IPMC_RAW_ENTRY is meaningful.        \n
 *              If this procedure identifies the found entry as 'raw' then \n
 *              this flag is set.                                          \n
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * Note that if the 'raw' flag (BCM_IPMC_RAW_ENTRY) is set, this procedure only accepts the
 *     'lpm' table case. In all other cases, an error will be injected.
 *   * Note that if a '_SHR_E_NOT_FOUND' error is detected by this procedure, no
 *     log message is printed but return value, still, is '_SHR_E_NOT_FOUND'. For all
 *     other errors, if error is encountered at all, log error message is printed on
 *     top of returned error value.
 */
int
bcm_dnx_ipmc_find(
    int unit,
    bcm_ipmc_addr_t * data)
{
    uint32 entry_handle_id;
    uint32 dbal_table;
    uint32 result_type = 0;
    uint32 destination_val;
    uint32 entry_access_id;
    uint32 hit_bit = 0;
    uint32 tcam_handler_id;
    uint32 hitbit_flags = DBAL_ENTRY_ATTR_HIT_GET;
    dbal_table_type_e dbal_table_type;
    shr_error_e rv;
    uint32 stat_pp_profile;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Make sure that the output parameters are 0 before receiving data.
     * In case of an error while receiving the result value, output value will be 0.
     */
    data->group = 0;
    BCM_L3_ITF_SET(data->l3a_intf, BCM_L3_ITF_TYPE_FEC, 0);

    SHR_INVOKE_VERIFY_DNX(dnx_ipmc_verify(unit, data));
    hitbit_flags |= (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_HIT_CLEAR)) ? DBAL_ENTRY_ATTR_HIT_CLEAR : 0;

    /*
     * Select the appropriate table that the entry belongs to based on the input fields.
     * Allocate a handle to that table.
     */
    SHR_IF_ERR_EXIT(dnx_ipmc_dbal_table_select(unit, data, &dbal_table));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));

    /*
     * Build the key of the entry based on input parameters.
     * If the L2 flag has been added, then a bridge entry will be looked for.
     * If the table that was selected is IPV4_MULTICAST_EM_FORWARD, then a MC host entry will be searched for.
     * Otherwise (else-clause) a MC route entry will be searched for in the LPM table.
     */
    /*
     * Receive the table type of the selected table and based on it add the key fields and seach for the result.
     */
    SHR_IF_ERR_EXIT(dbal_tables_table_type_get(unit, dbal_table, &dbal_table_type));

    /*
     * KBP tables behave the same way TCAM tables do, but the implementation differ.
     */
    if (IPMC_TABLE_IS_KBP(dbal_table))
    {
        SHR_IF_ERR_EXIT(dnx_ipmc_entry_tcam_key_set(unit, dbal_table, data, &entry_handle_id));

        rv = dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS);
        if (rv == _SHR_E_NOT_FOUND)
        {
            SHR_SET_CURRENT_ERR(rv);
            SHR_EXIT();
        }
        else
        {
            SHR_IF_ERR_EXIT(rv);
        }

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE,
                                                            &destination_val));
        SHR_IF_ERR_EXIT(dnx_ipmc_resolve_entry_destination(unit, destination_val, dbal_table, data));
        SHR_IF_ERR_EXIT(dbal_entry_attribute_get
                        (unit, entry_handle_id, DBAL_ENTRY_ATTR_PRIORITY, (uint32 *) &data->priority));
    }
    else if (dbal_table_type == DBAL_TABLE_TYPE_TCAM)
    {
        SHR_IF_ERR_EXIT(dnx_ipmc_entry_tcam_key_set(unit, dbal_table, data, &entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_tables_tcam_handler_id_get(unit, dbal_table, &tcam_handler_id));
        SHR_IF_ERR_EXIT_NO_MSG(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &entry_access_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));
        SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_priority_get
                        (unit, tcam_handler_id, entry_access_id, (uint32 *) &(data->priority)));
        SHR_IF_ERR_EXIT(dbal_entry_attribute_request(unit, entry_handle_id, hitbit_flags, &hit_bit));

        /*
         * If returned error is 'not_found' then exit with this error but do
         * not log an error message.
         */
        rv = dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS);
        if (rv == _SHR_E_NOT_FOUND)
        {
            SHR_SET_CURRENT_ERR(rv);
            SHR_EXIT();
        }
        else
        {
            SHR_IF_ERR_EXIT(rv);
        }
        /*
         * Receive value of destination fields based on the result type.
         * Currently supported:
         *      * L3 MC group ID
         *      * FEC
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &result_type));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, &destination_val));

        SHR_IF_ERR_EXIT(dnx_ipmc_resolve_entry_destination(unit, destination_val, dbal_table, data));
    }
    else
    {
        /*
         * If the entry belongs to an EM table, then its result can be MC_ID or FEC.
         * Otherwise, if the entry belongs to the LPM table, then its result is MC_ID_KAPS, FEC or default FEC.
         */
        if (dbal_table_type == DBAL_TABLE_TYPE_EM)
        {
            /*
             * If the L2 flag has been provided, then the key for a bridge entry will be built.
             * Otherwise, the fields for a host entry key will be used.
             */
            if (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_L2))
            {
                SHR_IF_ERR_EXIT(dnx_ipmc_entry_bridge_key_set(unit, data, &entry_handle_id));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_ipmc_entry_em_key_set(unit, data, &entry_handle_id));
            }
            SHR_IF_ERR_EXIT(dbal_entry_attribute_request(unit, entry_handle_id, hitbit_flags, &hit_bit));
            /*
             * Getting all destination fields of the entry.
             * Assign output value based on result type.
             */
            /*
             * If returned error is 'not_found' then exit with this error but do
             * not log an error message.
             */
            rv = dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS);
            if (rv == _SHR_E_NOT_FOUND)
            {
                SHR_SET_CURRENT_ERR(rv);
                SHR_EXIT();
            }
            else
            {
                SHR_IF_ERR_EXIT(rv);
            }
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE,
                                                                INST_SINGLE, &result_type));
            /*
             * Receive value of destination fields based on the result type.
             * Currently supported:
             *      * L3 MC group ID
             *      * FEC
             */
            if (result_type == DBAL_RESULT_TYPE_IPV4_MULTICAST_EM_FORWARD_FWD_DEST
                || result_type == DBAL_RESULT_TYPE_IPV4_MULTICAST_EM_FORWARD_FWD_DEST_STAT
                || result_type == DBAL_RESULT_TYPE_IPV6_MULTICAST_EM_FORWARD_FWD_DEST
                || result_type == DBAL_RESULT_TYPE_IPV6_MULTICAST_EM_FORWARD_FWD_DEST_STAT
                || result_type == DBAL_RESULT_TYPE_BRIDGE_IPV4_MULTICAST_SVL_FWD_DEST
                || result_type == DBAL_RESULT_TYPE_BRIDGE_IPV4_MULTICAST_SVL_FWD_DEST_STAT)
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_DESTINATION,
                                                                    INST_SINGLE, &destination_val));
                SHR_IF_ERR_EXIT(dnx_ipmc_resolve_entry_destination(unit, destination_val, dbal_table, data));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Illegal destination type of the found entry. Could not resolve to a forwarding destination.");
            }
        }
        else if (dbal_table_type == DBAL_TABLE_TYPE_LPM)
        {
            /*
             * If the L2 flag has been provided, then the key for a bridge entry will be built.
             * Otherwise, the fields for a host entry key will be used.
             */
            if (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_L2))
            {
                SHR_IF_ERR_EXIT(dnx_ipmc_entry_bridge_key_set(unit, data, &entry_handle_id));
            }
            else
            {
                /** Build LPM entry key */
                SHR_IF_ERR_EXIT(dnx_ipmc_entry_lpm_key_set(unit, data, &entry_handle_id));
            }
            SHR_IF_ERR_EXIT(dbal_entry_attribute_request(unit, entry_handle_id, hitbit_flags, &hit_bit));
            /** find the entry from the table */
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_KAPS_RESULT, INST_SINGLE, &destination_val);

            /*
             * Get the result of the entry and resolve it.
             * It can be one of the following types:
             *      * MC ID - the result of the entry will be assigned to the group field of the bcm_ipmc_addr_t structure.
             *      * FEC - the result of the entry will be assigned to the l3a_intf field.
             */
            /*
             * If returned error is 'not_found' then exit with this error but do
             * not log an error message.
             */
            rv = dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT);
            if (rv == _SHR_E_NOT_FOUND)
            {
                SHR_SET_CURRENT_ERR(rv);
                SHR_EXIT();
            }
            else
            {
                SHR_IF_ERR_EXIT(rv);
            }

            SHR_IF_ERR_EXIT(dnx_ipmc_resolve_lpm_result(unit, destination_val, data));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal dbal table type of selected table for entry");
        }
    }

    /** If the hit bit indication is active then the flag BCM_IPMC_HIT needs to be set. */
    if (hit_bit)
    {
        data->flags |= BCM_IPMC_HIT;
    }

    if (dnx_stat_pp_result_type_verify(unit, dbal_table, result_type) == _SHR_E_NONE)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                            entry_handle_id,
                                                            DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE, &stat_pp_profile));
        /*
         * Get the statistic data only if the profile is valid
         */
        if (stat_pp_profile != STAT_PP_PROFILE_INVALID)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                                entry_handle_id,
                                                                DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE,
                                                                &data->stat_id));

            STAT_PP_ENGINE_PROFILE_SET(data->stat_pp_profile, stat_pp_profile, bcmStatCounterInterfaceIngressReceivePp);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Adds/updates an IP multicast entry.
 *  \param [in] unit -
 *  The unit number.
 *  \param [in] data -
 *  contains the required information for selecting and update an IPMC entry.
 *  The following are relevant field of the bcm_ipmc_addr_t structure:
 *  - vrf        -> the VRF of the incoming packet.
 *  - group      -> the MC group ID (can be used as the result of the entry).
 *  - vid        -> Not the best name to the entry ETH-RIF.
 *  - mc_ip_addr -> the IPv4 MC address.
 *  - mc_ip_mask -> the mask of the IPv4 MC address.
 *  - s_ip_addr  -> the source IP (IPv4 address).
 *  - s_ip_mask  -> mask of the source IP (IPv4 address).
 *  - mc_ip6_addr, mc_ip6_mask -> address and mask of the IPv6 entry
 *  - s_ip6_addr, s_ip6_mask -> address and mask of the IPv6 entry (valid for TCAM only)
 *  - flags      -> control flags for the IPMC entry:
 *      * BCM_IPMC_L2 - indicates bridge entry
 *      * BCM_IPMC_IP6 - indicates IPv6 entry
 *      * BCM_IPMC_RAW_ENTRY - indicates 'raw' entry (Use 'l3a_intf' as is).
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
int
bcm_dnx_ipmc_add(
    int unit,
    bcm_ipmc_addr_t * data)
{
    uint32 entry_handle_id;
    uint32 dbal_table;
    uint32 entry_access_id = 0;
    dbal_table_type_e dbal_table_type;
    int core = DBAL_CORE_ALL;
    uint32 result_type = 0;
    dbal_entry_action_flags_e dbal_action;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_ipmc_add_verify(unit, data));

    /*
     * Select the table to which this entry should be added.
     * This is done based on the input parameters provided in the bcm_ipmc_addr_t variable (data).
     * A handle to the chosen table is allocated.
     */
    SHR_IF_ERR_EXIT(dnx_ipmc_dbal_table_select(unit, data, &dbal_table));

    /*
     * Create an entry handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));

    /*
     * Find the type of the table to which the entry belongs - EM, TCAM or LPM.
     * Based on the type add the key and result fields for the entry.
     */
    SHR_IF_ERR_EXIT(dbal_tables_table_type_get(unit, dbal_table, &dbal_table_type));

    /*
     * KBP tables behave the same way TCAM tables do, but the implementation differ.
     */
    if (IPMC_TABLE_IS_KBP(dbal_table))
    {
        SHR_IF_ERR_EXIT(dnx_ipmc_entry_tcam_key_set(unit, dbal_table, data, &entry_handle_id));
        SHR_IF_ERR_EXIT(dnx_ipmc_entry_result_set
                        (unit, data, dbal_table, &entry_handle_id, dbal_table_type, &result_type));
        SHR_IF_ERR_EXIT(dbal_entry_attribute_set(unit, entry_handle_id, DBAL_ENTRY_ATTR_PRIORITY, data->priority));

        if (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_REPLACE))
        {
            dbal_action = DBAL_COMMIT_UPDATE;
        }
        else
        {
            dbal_action = DBAL_COMMIT;
        }
    }
    else if (dbal_table_type == DBAL_TABLE_TYPE_TCAM)
    {
        SHR_IF_ERR_EXIT(dnx_ipmc_entry_tcam_key_set(unit, dbal_table, data, &entry_handle_id));
        if (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_REPLACE))
        {
            SHR_IF_ERR_EXIT(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &entry_access_id, DBAL_COMMIT));
            dbal_action = DBAL_COMMIT_UPDATE;
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_field_entry_access_id_create(unit, core, dbal_table, data->priority, &entry_access_id));
            dbal_action = DBAL_COMMIT;
        }
        SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));

        SHR_IF_ERR_EXIT(dnx_ipmc_entry_result_set
                        (unit, data, dbal_table, &entry_handle_id, dbal_table_type, &result_type));
    }
    else
    {
        /*
         * If it is an EM table, then the result is MC_ID (currently only result type supported)
         * If it is an LPM table, then the result is MC_ID_KAPS.
         */
        if (dbal_table_type == DBAL_TABLE_TYPE_EM)
        {
            if (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_L2))
            {
                SHR_IF_ERR_EXIT(dnx_ipmc_entry_bridge_key_set(unit, data, &entry_handle_id));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_ipmc_entry_em_key_set(unit, data, &entry_handle_id));
            }
        }
        else if (dbal_table_type == DBAL_TABLE_TYPE_LPM)
        {
            if (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_L2))
            {
                SHR_IF_ERR_EXIT(dnx_ipmc_entry_bridge_key_set(unit, data, &entry_handle_id));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_ipmc_entry_lpm_key_set(unit, data, &entry_handle_id));
            }
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid DBAL table type found for IPMC add - %d\n", dbal_table_type);
        }
        /*
         * Set the result field of the route or host entry.
         * Based on what fields are defined in the bcm_ipmc_addr_t structure either group or l3a_intf field is used.
         * Different sub-fields for the result are set for route - either MC_ID_KAPS, FEC or FEC_DEFAULT.
         * For host entry the result can be either MC_ID or FEC.
         */
        SHR_IF_ERR_EXIT(dnx_ipmc_entry_result_set
                        (unit, data, dbal_table, &entry_handle_id, dbal_table_type, &result_type));
        /*
         * Commit entry to the table.
         * If the BCM_IPMC_REPLACE flag has been provided, then the entry needs to be updated.
         * If the flag is added but no such entry exists, an error will be returned.
         */
        if (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_REPLACE))
        {
            dbal_action = DBAL_COMMIT_UPDATE;
        }
        else
        {
            dbal_action = DBAL_COMMIT;
        }
    }

    /*
     * Statistics enabled
     */
    if (dnx_stat_pp_result_type_verify(unit, dbal_table, result_type) == _SHR_E_NONE)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE, data->stat_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE,
                                     STAT_PP_PROFILE_ID_GET(data->stat_pp_profile));
    }

    /*
     * Commit entry to the table.
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, dbal_action));

exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Deletes an IP multicast entry.
 *  \param [in] unit -
 *  The unit number.
 *  \param [in] data -
 *  contains the required information for uniquely identifying
 *  an IPMC entry. The following are relevant field of the
 *  bcm_ipmc_addr_t structure:
 *  - vrf        -> the VRF of the incoming packet.
 *  - vid        -> Not the best name to the entry ETH-RIF.
 *  - mc_ip_addr -> the IPv4 MC address.
 *  - mc_ip_mask -> the mask of the IPv4 MC address.
 *  - s_ip_addr  -> the source IP (IPv4 address).
 *  - s_ip_mask  -> mask of the source IP (IPv4 address).
 *  - mc_ip6_addr, mc_ip6_mask - address and mask of the IPv6 entry
 *  - s_ip6_addr, s_ip6_mask - address and mask of the IPv6 entry (valid for TCAM only)
 *  - flags      -> control flags for the IPMC entry:
 *      * BCM_IPMC_L2 - indicate bridge entry
 *      * BCM_IPMC_IP6 - indicate IPv6 entry
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
int
bcm_dnx_ipmc_remove(
    int unit,
    bcm_ipmc_addr_t * data)
{
    uint32 entry_handle_id;
    uint32 dbal_table;
    uint32 entry_access_id = 0;
    dbal_table_type_e dbal_table_type;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_ipmc_verify(unit, data));

    SHR_IF_ERR_EXIT(dnx_ipmc_dbal_table_select(unit, data, &dbal_table));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));

    /*
     * Build key to uniquely identify an IPMC entry.
     * If the BCM_IPMC_L2 flag is provided, then a bridge entry will be deleted.
     * If the table that was selected is IPV4_MULTICAST_EM_FORWARD, that means that a MC host entry will be deleted.
     * Otherwise (else-clause) a MC route entry will be deleted from the LPM table.
     */
    SHR_IF_ERR_EXIT(dbal_tables_table_type_get(unit, dbal_table, &dbal_table_type));

    /*
     * KBP tables behave the same way TCAM tables do, but the implementation differ.
     */
    if (IPMC_TABLE_IS_KBP(dbal_table))
    {
        SHR_IF_ERR_EXIT(dnx_ipmc_entry_tcam_key_set(unit, dbal_table, data, &entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }
    else if (dbal_table_type == DBAL_TABLE_TYPE_EM)
    {
        if (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_L2))
        {
            SHR_IF_ERR_EXIT(dnx_ipmc_entry_bridge_key_set(unit, data, &entry_handle_id));
        }
        else
        {
            /*
             * Set the key fields for the IPMC entry in EM table
             */
            SHR_IF_ERR_EXIT(dnx_ipmc_entry_em_key_set(unit, data, &entry_handle_id));
        }
        /** clearing the entry */
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }
    else if (dbal_table_type == DBAL_TABLE_TYPE_TCAM)
    {
        SHR_IF_ERR_EXIT(dnx_ipmc_entry_tcam_key_set(unit, dbal_table, data, &entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &entry_access_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));
        /** Clear the entry ID */
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(dnx_field_entry_access_id_destroy(unit, dbal_table, entry_access_id));
    }
    else
    {
        if (_SHR_IS_FLAG_SET(data->flags, BCM_IPMC_L2))
        {
            SHR_IF_ERR_EXIT(dnx_ipmc_entry_bridge_key_set(unit, data, &entry_handle_id));
        }
        else
        {
            /** Build LPM entry key */
            SHR_IF_ERR_EXIT(dnx_ipmc_entry_lpm_key_set(unit, data, &entry_handle_id));
        }
        /** clearing the entry */
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Deletes all IP multicast entries.
 *  \param [in] unit - The unit number
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 */
int
bcm_dnx_ipmc_remove_all(
    int unit)
{
    uint32 table;
    uint32 nof_tables;
    uint8 use_kbp_ipv4 = dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv4) ? TRUE : FALSE;
    uint8 use_kbp_ipv6 = dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv6) ? TRUE : FALSE;

    uint32 dbal_mc_tables[] = {
        DBAL_TABLE_IPV4_MULTICAST_EM_FORWARD,
        DBAL_TABLE_IPV4_MULTICAST_PRIVATE_LPM_FORWARD,
        DBAL_TABLE_IPV4_MULTICAST_PUBLIC_LPM_FORWARD,
        DBAL_TABLE_IPV4_MULTICAST_TCAM_FORWARD,
        DBAL_TABLE_KBP_IPV4_MULTICAST_TCAM_FORWARD,
        DBAL_TABLE_BRIDGE_IPV4_MULTICAST_SVL,
        DBAL_TABLE_BRIDGE_IPV4_MULTICAST_SOURCE_SPECIFIC_SVL,
        DBAL_TABLE_IPV6_MULTICAST_EM_FORWARD,
        DBAL_TABLE_IPV6_MULTICAST_PRIVATE_LPM_FORWARD,
        DBAL_TABLE_IPV6_MULTICAST_TCAM_FORWARD,
        DBAL_TABLE_KBP_IPV6_MULTICAST_TCAM_FORWARD,
        DBAL_TABLE_BRIDGE_IPV6_MULTICAST_SOURCE_SPECIFIC_SVL
    };

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_tables = sizeof(dbal_mc_tables) / sizeof(dbal_mc_tables[0]);

    /*
     * Iterate over all L3-Multicast tables and clear their contents.
     * This includes host and route tables for IPv4 public and private entries.
     */
    for (table = 0; table < nof_tables; table++)
    {
        /** Skip clear for KBP tables if they are not used */
        if (((dbal_mc_tables[table] == DBAL_TABLE_KBP_IPV4_MULTICAST_TCAM_FORWARD) && !use_kbp_ipv4)
            || ((dbal_mc_tables[table] == DBAL_TABLE_KBP_IPV6_MULTICAST_TCAM_FORWARD) && !use_kbp_ipv6))
        {
            continue;
        }

        SHR_IF_ERR_EXIT(dbal_table_clear(unit, dbal_mc_tables[table]));
        /** If the table is a TCAM one, then we need to clear all access IDs. */
        if (dbal_mc_tables[table] == DBAL_TABLE_IPV4_MULTICAST_TCAM_FORWARD
            || dbal_mc_tables[table] == DBAL_TABLE_IPV6_MULTICAST_TCAM_FORWARD)
        {
            SHR_IF_ERR_EXIT(dnx_field_entry_access_id_destroy_all(unit, dbal_mc_tables[table]));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Traverse all entries in the IP multicast tables and run a
 *  callback function with the data from the found entry.
 *  \param [in] unit - The unit number.
 *  \param [in] flags - control flags
 *         * BCM_IPMC_IP6 - if set IPv6 tables will be
 *           traversed, otherwise IPv4 tables.
 *         * BCM_IPMC_L2 - if provided, bridge entries will be
 *           traversed as well as route entries.
 *  \param [in] cb - the name of the callback function that will
 *         be called for each entry.
 *  \param [in] user_data - data passed to the callback function
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \see
 *  * DBAL_TABLE_IPV4_MULTICAST_*
 *  * DBAL_TABLE_IPV6_MULTICAST_*
 *  * DBAL_TABLE_BRIDGE_IPV4_MULTICAST_*
 */
int
bcm_dnx_ipmc_traverse(
    int unit,
    uint32 flags,
    bcm_ipmc_traverse_cb cb,
    void *user_data)
{
    uint32 entry_handle_id;
    uint32 table;
    uint32 dbal_tables[] = {
        DBAL_TABLE_IPV4_MULTICAST_EM_FORWARD,
        DBAL_TABLE_IPV4_MULTICAST_PRIVATE_LPM_FORWARD, DBAL_TABLE_IPV4_MULTICAST_PUBLIC_LPM_FORWARD,
        DBAL_TABLE_IPV4_MULTICAST_TCAM_FORWARD,
        DBAL_TABLE_KBP_IPV4_MULTICAST_TCAM_FORWARD,
        DBAL_TABLE_BRIDGE_IPV4_MULTICAST_SVL,
        DBAL_TABLE_BRIDGE_IPV4_MULTICAST_SOURCE_SPECIFIC_SVL,
        DBAL_TABLE_IPV6_MULTICAST_EM_FORWARD,
        DBAL_TABLE_IPV6_MULTICAST_PRIVATE_LPM_FORWARD,
        DBAL_TABLE_IPV6_MULTICAST_TCAM_FORWARD,
        DBAL_TABLE_KBP_IPV6_MULTICAST_TCAM_FORWARD,
        DBAL_TABLE_BRIDGE_IPV6_MULTICAST_SOURCE_SPECIFIC_SVL
    };
    uint32 nof_tables;
    uint32 start;
    uint32 field_value;
    uint32 hitbit_flags = DBAL_ENTRY_ATTR_HIT_GET | DBAL_ENTRY_ATTR_HIT_PRIMARY | DBAL_ENTRY_ATTR_HIT_SECONDARY;
    bcm_ipmc_addr_t dbal_entry;
    dbal_table_type_e dbal_table_type;
    int is_end;
    uint32 hit_bit = 0;
    uint8 use_kbp_ipv4 = dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv4) ? TRUE : FALSE;
    /** Update the assignment below when IPv6 MC is supported with KBP */
    uint8 use_kbp_ipv6 = dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv6) ? TRUE : FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * If the BCM_IPMC_IP6 flag is not set, then all IPv4 tables will be traversed.
     * Otherwise, an error message will be returned that it is not currently supported.
     */
    if (!_SHR_IS_FLAG_SET(flags, BCM_IPMC_IP6))
    {
        /*
         * If the BCM_IPMC_L2 flag is set, all BRIDGE tables will be traversed.
         * This includes SVL EM and LPM tables.
         * Otherwise the L3 IPMC tables will be traversed - host, public and private route.
         */
        if (_SHR_IS_FLAG_SET(flags, BCM_IPMC_L2))
        {
            nof_tables = IPMC_BRIDGE_TABLES_NOF;
            start = IPMC_BRIDGE_TABLES_START;
        }
        else
        {
            nof_tables = IPMC_IPV4_TABLES_NOF;
            start = IPMC_IPV4_TABLES_START;
        }
    }
    else
    {
        if (_SHR_IS_FLAG_SET(flags, BCM_IPMC_L2))
        {
            nof_tables = IPMC_IPV6_BRIDGE_TABLES_NOF;
            start = IPMC_IPV6_BRIDGE_TABLES_START;
        }
        else
        {
            nof_tables = IPMC_IPV6_TABLES_NOF;
            start = IPMC_IPV6_TABLES_START;
        }
    }

    /*
     * Traverse all IPv4 or IPv6 tables one after another.
     * This includes LPM(route) and EM(host) tables
     */
    for (table = start; table < (start + nof_tables); table++)
    {
        /** Skip traverse for KBP tables if they are not used */
        if (((dbal_tables[table] == DBAL_TABLE_KBP_IPV4_MULTICAST_TCAM_FORWARD) && !use_kbp_ipv4)
            || ((dbal_tables[table] == DBAL_TABLE_KBP_IPV6_MULTICAST_TCAM_FORWARD) && !use_kbp_ipv6))
        {
            continue;
        }
        /*
         * Allocate handle to the table of the iteration and initialize an iterator entity.
         * The iterator is in mode ALL, which means that it will consider all entries regardless
         * of them being default entries or not.
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_tables[table], &entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
        SHR_IF_ERR_EXIT(dbal_tables_table_type_get(unit, dbal_tables[table], &dbal_table_type));

        if (!IPMC_TABLE_IS_KBP(dbal_tables[table]))
        {
            SHR_IF_ERR_EXIT(dbal_iterator_attribute_action_add(unit, entry_handle_id, hitbit_flags));
            if (_SHR_IS_FLAG_SET(flags, BCM_IPMC_HIT_CLEAR))
            {
                SHR_IF_ERR_EXIT(dbal_iterator_attribute_action_add(unit, entry_handle_id, DBAL_ENTRY_ATTR_HIT_CLEAR));
            }
        }

        /*
         * Receive first entry in table.
         */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        while (!is_end)
        {
            hit_bit = 0;
            bcm_ipmc_addr_t_init(&dbal_entry);
            /*
             * The combination of fields that form the different route entries are:
             *      - VRF, G, S, RIF (LPM, TCAM)
             *      - G, S, RIF (LPM)
             *      - VRF, G, RIF (EM)
             *
             * When the BCM_IPMC_L2 flag has been provided, bridge entries are also traversed.
             * Valid combination of fields for key are:
             *      - VSI, VLAN-ID, G (EM)
             *      - VSI, VLAN-ID, G, S (LPM)
             *      - VSI, G (EM)
             *      - VSI, G, S (LPM)
             *
             * When the BCM_IPMC_IP6 flag has been provided, IPv6 tables are traversed. For them valid combinations are:
             *      - VRF, G, RIF (EM)
             *      - VRF, G, RIF (LPM)
             *      - G, RIF (LPM)
             *      - VRF, G, S, RIF (TCAM)
             */
            if (dbal_table_type == DBAL_TABLE_TYPE_TCAM)
            {
                dbal_entry.flags |= BCM_IPMC_TCAM;
            }
            if (_SHR_IS_FLAG_SET(flags, BCM_IPMC_L2))
            {
                SHR_IF_ERR_EXIT(dnx_ipmc_bridge_key_field_get(unit, entry_handle_id, dbal_tables[table], &dbal_entry));
            }
            else if (_SHR_IS_FLAG_SET(flags, BCM_IPMC_IP6))
            {
                SHR_IF_ERR_EXIT(dnx_ipmc_ipv6_key_field_get(unit, entry_handle_id, dbal_tables[table], &dbal_entry));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_ipmc_ipv4_key_field_get(unit, entry_handle_id, dbal_tables[table], &dbal_entry));
            }

            if (!IPMC_TABLE_IS_KBP(dbal_tables[table]))
            {
                SHR_IF_ERR_EXIT(dbal_entry_attribute_get(unit, entry_handle_id, hitbit_flags, &hit_bit));
                if (hit_bit)
                {
                    dbal_entry.flags |= BCM_IPMC_HIT;
                }
            }

            /*
             * If the current table is an EM or TCAM table, then the result of the entry is retrieved
             * using DESTINATION field.
             * It is resolved to either a mc group or a FEC using dnx_ipmc_resolve_entry_destination function.
             *
             * If the current table is an LPM table, the result of this entry is resolved
             * using the dnx_ipmc_resolve_lpm_result function.
             */
            if (IPMC_TABLE_IS_KBP(dbal_tables[table]))
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id,
                                                                        DBAL_FIELD_DESTINATION, INST_SINGLE,
                                                                        &field_value));
                SHR_IF_ERR_EXIT(dnx_ipmc_resolve_entry_destination(unit, field_value, dbal_tables[table], &dbal_entry));
            }
            else if (dbal_table_type != DBAL_TABLE_TYPE_LPM)
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_DESTINATION,
                                                                        INST_SINGLE, &field_value));
                SHR_IF_ERR_EXIT(dnx_ipmc_resolve_entry_destination(unit, field_value, dbal_tables[table], &dbal_entry));
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_KAPS_RESULT,
                                                                        INST_SINGLE, &field_value));
                SHR_IF_ERR_EXIT(dnx_ipmc_resolve_lpm_result(unit, field_value, &dbal_entry));
            }

            /*
             * If user provided a name for the callback function,
             * call it with passing the data from the found entry.
             */
            if (cb != NULL)
            {
                /*
                 * Invoke callback function
                 */
                SHR_IF_ERR_EXIT((*cb) (unit, &dbal_entry, user_data));
            }
            /*
             * Receive next entry in table.
             */

            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * }
 */
