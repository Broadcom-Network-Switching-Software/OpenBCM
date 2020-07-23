/** \file bfd_oamp.c
 * 
 *
 * BFD OAMP table access procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_BFD

 /*
  * Include files.
  * {
  */
#include <soc/dnx/dbal/dbal.h>
#include <bcm/bfd.h>
#include <bcm_int/dnx/bfd/bfd.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_bfd.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <src/bcm/dnx/oam/oam_internal.h>
#include <src/bcm/dnx/bfd/bfd_internal.h>
#include <bcm_int/dnx/algo/oam/algo_oam.h>
#include <bcm_int/dnx/algo/bfd/algo_bfd.h>
#include <bcm_int/dnx/algo/oamp/algo_oamp.h>
#include <src/bcm/dnx/oam/bfd_oamp.h>
#include <src/bcm/dnx/oam/oam_oamp_crc.h>
#include <src/bcm/dnx/oam/oamp_pe_infra.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_bfd_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_oam_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_oamp_access.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/**
 *  Number of local discriminator bits stored for a BFD
 *  endpoint in a 1/4 entry (the rest are in the MDB)
 */
#define YOUR_DISC_LSB_NOF_BITS 26

/** number of bits of the flow label field in IPv6 header */
#define DNX_BFD_OAMP_IPV6_FLOW_LABEL_NOF_BITS 20

/** number of bits of the priority field in IPv6 header */
#define DNX_BFD_OAMP_IPV6_PRIORITY_NOF_BITS 8

/** IPv6 version number */
#define DNX_BFD_OAMP_IPV6_VERSION 6

/** IPv6 payload length offset */
#define DNX_BFD_IPV6_HEADER_PAYLOAD_LENGTH_OFFSET 4

/** IPv6 flow label offset */
#define DNX_BFD_IPV6_HEADER_FLOW_LABEL_OFFSET 2

/** UDP opcode */
#define DNX_BFD_OAMP_UDP_OPCODE 17

/** number of bits of the version field in BFD header */
#define DNX_BFD_OAMP_BFD_VERSION_NOF_BITS 3

/** number of bits of the diag field in BFD header */
#define DNX_BFD_OAMP_BFD_DIAG_NOF_BITS 5

/** number of bits of the flags field in BFD header */
#define DNX_BFD_OAMP_BFD_FLAGS_NOF_BITS 6

/** UDP BFD port */
#define DNX_BFD_OAMP_BFD_UDP_PORT(flags) (!_SHR_IS_FLAG_SET(flags, BCM_BFD_ENDPOINT_MULTIHOP)) ? \
        ((_SHR_IS_FLAG_SET(flags, BCM_BFD_ENDPOINT_MICRO_BFD)) ? 6784 : 3784) : 4784

/** number of bits in the field OPCODES_TO_PREPEND in the EXTRA_DATA_HEADER entry */
#define DNX_BFD_OAMP_EXTRA_DATA_HEADER_OPCODES_TO_PREPEND_NOF_BITS 14

/** Maximum BFD Tx rate - once every scan */
#define MAXIMUM_BFD_TX_RATE 0x1FF

/** Conversion of a 6 bit flags value to a 4 bit profile   */
#define BFD_FLAGS_VAL_TO_PROFILE(x) ((UTILEX_GET_BITS_RANGE(x, 5, 3) << 1) | UTILEX_GET_BIT(x, 1))

/** Single-hop/micro BFD TOS feature supported */
#define SINGLE_HOP_U_BFD_TOS_SUPPORTED(unit) (dnx_data_bfd.general.feature_get(unit, dnx_data_bfd_general_configurable_single_hop_tos) == 1)

/** Chosen action for writing UDP source port to hardware table */
#define WRITE_NONE 0
#define WRITE_UDP  1
#define WRITE_MPLS 2
#define WRITE_SBFD 3

/** Bit offset for TC field in API field */
#define TC_IN_API_OFFSET 2

/** Size of TC field in bits */
#define TC_IN_API_NOF_BITS 3

/** All flags relevant to punt profile */
#define PUNT_PROFILE_FLAGS (\
BCM_BFD_ENDPOINT_REMOTE_UPDATE_STATE_DISABLE | \
BCM_BFD_ENDPOINT_RX_REMOTE_EVENT_DISABLE | \
BCM_BFD_ENDPOINT_RX_RDI_AUTO_UPDATE | \
BCM_BFD_ENDPOINT_REMOTE_UPDATE_STATE_DISABLE | \
BCM_BFD_ENDPOINT_REMOTE_EVENT_DISABLE | \
BCM_BFD_ENDPOINT_RDI_AUTO_UPDATE | \
BCM_BFD_ENDPOINT_RDI_ON_LOC | \
BCM_BFD_ENDPOINT_RDI_CLEAR_ON_LOC_CLEAR)

#define ONES_COMPLEMENT(r) (-(r)-1)

/** BFD Tx Period Cluster definitions */

/** A MEP profile is being modified */
#define REGULAR_MEP_PROFILE_UPDATE 1

/**
 * The tx_period of a cluster is being modified,
 * so all MEP profiles (up to 4) associated with
 * that cluster must be modified as well
 */
#define BFD_CLUSTER_TX_PERIOD_UPDATE 2

/*
 * }
 */

/*
 * MACROs
 * {
 */
/*
 * }
 */

/*
 * Global and Static
 * {
 */

const dbal_fields_e mep_profile_fields[] =
    { DBAL_FIELD_MEP_PROFILE_NO_IPV6_COUNT_0, DBAL_FIELD_MEP_PROFILE_IPV6_COUNT_0,
    DBAL_FIELD_MEP_PROFILE_NO_IPV6_COUNT_1, DBAL_FIELD_MEP_PROFILE_IPV6_COUNT_1
};
const dbal_fields_e mep_profile_valid_fields[] =
    { DBAL_FIELD_MEP_PROFILE_NO_IPV6_COUNT_0_VALID, DBAL_FIELD_MEP_PROFILE_IPV6_COUNT_0_VALID,
    DBAL_FIELD_MEP_PROFILE_NO_IPV6_COUNT_1_VALID, DBAL_FIELD_MEP_PROFILE_IPV6_COUNT_1_VALID
};

/*
 * }
 */

/**
 * \brief - this function calculates checksum
 *        the sum is calculated in one's complement arithmetic
 *        we calculate the sum of the words
 *        word's length is 16 bits
 *
 * \param [in] buff - buffer of words
 * \param [in] size - num of words (each word 16b)
 *
 * \return
 *   uint16 - the calculated checksum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static uint16
dnx_bfd_oamp_ones_comp_checksum(
    uint16 *buff,
    uint32 size)
{
    uint32 sum = 0;
    uint8 carry;
    uint16 all_ones_16b = (1 << 16) - 1;
    int i;
    for (i = 0; i < size; i++)
    {
        sum += buff[i];
        carry = sum >> 16;
        sum += carry;
        sum &= all_ones_16b;
    }
    sum = ONES_COMPLEMENT(sum);
    sum &= all_ones_16b;
    if (sum == 0)
    {
        sum = all_ones_16b;
    }
    return (uint16) sum;
}

static void
dnx_bfd_oamp_ipv6_pseduo_header_to_array_of_bytes(
    uint8 *target_array,
    bcm_bfd_ipv6_pseudo_header_t * ipv6_pseudo_header)
{
    int index = 0;
    sal_memcpy(target_array, ipv6_pseudo_header->source_address, sizeof(ipv6_pseudo_header->source_address));
    index += sizeof(ipv6_pseudo_header->source_address);
    sal_memcpy(target_array + index, ipv6_pseudo_header->destination_address,
               sizeof(ipv6_pseudo_header->destination_address));
    index += sizeof(ipv6_pseudo_header->destination_address);
    sal_memcpy(target_array + index, &ipv6_pseudo_header->zero, sizeof(ipv6_pseudo_header->zero));
    index += sizeof(ipv6_pseudo_header->zero);
    sal_memcpy(target_array + index, &ipv6_pseudo_header->protocol, sizeof(ipv6_pseudo_header->protocol));
    index += sizeof(ipv6_pseudo_header->protocol);
    sal_memcpy(target_array + index, &ipv6_pseudo_header->udp_length, sizeof(ipv6_pseudo_header->udp_length));
    return;
}

static void
dnx_bfd_oamp_ipv6_header_to_array_of_bytes(
    uint8 *target_array,
    bcm_bfd_ipv6_header_t * ipv6_header)
{
    int index = 0;
    sal_memcpy(target_array, &ipv6_header->version_prior_flow_label, sizeof(ipv6_header->version_prior_flow_label));
    index += sizeof(ipv6_header->version_prior_flow_label);
    sal_memcpy(target_array + index, &ipv6_header->payload_length, sizeof(ipv6_header->payload_length));
    index += sizeof(ipv6_header->payload_length);
    sal_memcpy(target_array + index, &ipv6_header->next_header, sizeof(ipv6_header->next_header));
    index += sizeof(ipv6_header->next_header);
    sal_memcpy(target_array + index, &ipv6_header->hop_limit, sizeof(ipv6_header->hop_limit));
    index += sizeof(ipv6_header->hop_limit);
    sal_memcpy(target_array + index, ipv6_header->source_address, sizeof(ipv6_header->source_address));
    index += sizeof(ipv6_header->source_address);
    sal_memcpy(target_array + index, ipv6_header->destination_address, sizeof(ipv6_header->destination_address));
    return;
}

static void
dnx_bfd_oamp_udp_header_to_array_of_bytes(
    uint8 *target_array,
    bcm_bfd_udp_header_t * udp_header)
{
    int index = 0;
    sal_memcpy(target_array, &udp_header->source_port, sizeof(udp_header->source_port));
    index += sizeof(udp_header->source_port);
    sal_memcpy(target_array + index, &udp_header->destination_port, sizeof(udp_header->destination_port));
    index += sizeof(udp_header->destination_port);
    sal_memcpy(target_array + index, &udp_header->length, sizeof(udp_header->length));
    index += sizeof(udp_header->length);
    sal_memcpy(target_array + index, &udp_header->checksum, sizeof(udp_header->checksum));
    return;
}

static void
dnx_bfd_oamp_bfd_header_to_array_of_bytes(
    uint8 *target_array,
    bcm_bfd_bfd_header_t * bfd_header)
{
    int index = 0;
    sal_memcpy(target_array, &bfd_header->vers_diag, sizeof(bfd_header->vers_diag));
    index += sizeof(bfd_header->vers_diag);
    sal_memcpy(target_array + index, &bfd_header->sta_flags, sizeof(bfd_header->sta_flags));
    index += sizeof(bfd_header->sta_flags);
    sal_memcpy(target_array + index, &bfd_header->detect_mult, sizeof(bfd_header->detect_mult));
    index += sizeof(bfd_header->detect_mult);
    sal_memcpy(target_array + index, &bfd_header->length, sizeof(bfd_header->length));
    index += sizeof(bfd_header->length);
    sal_memcpy(target_array + index, &bfd_header->my_discriminator, sizeof(bfd_header->my_discriminator));
    index += sizeof(bfd_header->my_discriminator);
    sal_memcpy(target_array + index, &bfd_header->your_discriminator, sizeof(bfd_header->your_discriminator));
    index += sizeof(bfd_header->your_discriminator);
    sal_memcpy(target_array + index, &bfd_header->desired_min_tx_interval, sizeof(bfd_header->desired_min_tx_interval));
    index += sizeof(bfd_header->desired_min_tx_interval);
    sal_memcpy(target_array + index, &bfd_header->required_min_rx_interval,
               sizeof(bfd_header->required_min_rx_interval));
    index += sizeof(bfd_header->required_min_rx_interval);
    sal_memcpy(target_array + index, &bfd_header->required_min_echo_rx_interval,
               sizeof(bfd_header->required_min_echo_rx_interval));
    return;
}

/**
 * \brief - this function calculates the headers to be saved in the mep db
 *        for OAMP-PE BFD over IPv6 implementation
 *        the calculated headers are saved as mep db extra data entry
 *        OAMP-PE program removes the IPv4 header and adds IPv6 header that is taken from the additional(extra) data
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oam_id - ID of entry to calculate the udp_over_ipv6 header for
 * \param [in] endpoint_info - endpoint information supplied by bcm_dnx_bfd_endpoint_create
 * \param [out] ipv6_extra_data_header - IPv6 header content built by the oam_id and the endpoint_info
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */

static int
dnx_bfd_oamp_udp_over_ipv6_external_data(
    int unit,
    uint16 oam_id,
    bcm_bfd_endpoint_info_t const *endpoint_info,
    uint8 *ipv6_extra_data_header)
{

    bcm_bfd_ipv6_header_t *ipv6_header = NULL;
    uint8 *ipv6_header_packed = NULL;
    bcm_bfd_ipv6_pseudo_header_t *ipv6_pseudo_header = NULL;
    uint8 *ipv6_pseudo_header_packed = NULL;
    bcm_bfd_udp_header_t *udp_header = NULL;
    uint8 *udp_header_packed = NULL;
    bcm_bfd_bfd_header_t *bfd_header = NULL;
    uint8 *bfd_header_packed = NULL;
    uint8 *checksum_header_packed = NULL;
    uint32 entry_handle_id = 0, bfd_vers = 0, bfd_length = 0, bfd_min_echo_rx = 0;
    uint32 ipv6_prior = 0;
    uint32 udp_src_port = 0;
    uint16 udp_dst_port = 0, port = 0;
    int index = 0, dummy;
    uint8 big_endian_checksum[2];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ipv6_extra_data_header, _SHR_E_PARAM, "entry_values");

    /*
     * we used packed arrays for avoiding padding in the structs
     */
    SHR_ALLOC_SET_ZERO(ipv6_header, sizeof(*ipv6_header), "BFD IPv6 header", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(ipv6_header_packed, DNX_BFD_IPV6_HEADER_SIZE, "BFD IPv6 header packed", "%s%s%s\r\n", EMPTY,
                       EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(ipv6_pseudo_header, sizeof(*ipv6_pseudo_header), "BFD IPv6 pseudo header", "%s%s%s\r\n", EMPTY,
                       EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(ipv6_pseudo_header_packed, DNX_BFD_IPV6_PSEUDO_HEADER_SIZE, "BFD IPv6 pseudo header packed",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(udp_header, sizeof(*udp_header), "BFD UDP header", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(udp_header_packed, DNX_BFD_UDP_HEADER_SIZE, "BFD UDP header packed", "%s%s%s\r\n", EMPTY, EMPTY,
                       EMPTY);
    SHR_ALLOC_SET_ZERO(bfd_header, sizeof(*bfd_header), "BFD BFD header", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(bfd_header_packed, DNX_BFD_BFD_HEADER_SIZE, "BFD BFD header packed", "%s%s%s\r\n", EMPTY, EMPTY,
                       EMPTY);
    SHR_ALLOC_SET_ZERO(checksum_header_packed, DNX_BFD_CHECKSUM_HEADER_SIZE, "BFD CHECKSUM header packed", "%s%s%s\r\n",
                       EMPTY, EMPTY, EMPTY);

    /*
     * fill the headers fields according to the information in endpoint_info
     */

    ipv6_prior = endpoint_info->ip_tos;

    ipv6_header->version_prior_flow_label =
        bcm_htonl((DNX_BFD_OAMP_IPV6_VERSION <<
                   (DNX_BFD_OAMP_IPV6_PRIORITY_NOF_BITS + DNX_BFD_OAMP_IPV6_FLOW_LABEL_NOF_BITS)) +
                  ((ipv6_prior & ((1 << DNX_BFD_OAMP_IPV6_PRIORITY_NOF_BITS) - 1)) <<
                   DNX_BFD_OAMP_IPV6_FLOW_LABEL_NOF_BITS));
    ipv6_header->payload_length = bcm_htons((uint16) (DNX_BFD_UDP_HEADER_SIZE + DNX_BFD_BFD_HEADER_SIZE));

    ipv6_header->next_header = DNX_BFD_OAMP_UDP_OPCODE;
    ipv6_header->hop_limit = endpoint_info->ip_ttl;
    sal_memcpy(ipv6_header->source_address, endpoint_info->src_ip6_addr, sizeof(bcm_ip6_t));
    sal_memcpy(ipv6_pseudo_header->source_address, endpoint_info->src_ip6_addr, sizeof(bcm_ip6_t));
    /*
     * When UDP runs over IPv6, the checksum is computed using a "pseudo header" that contains some of the same
     * information from the real IPv6 header. The pseudo header is not the real IPv6 header used to send an IP packet,
     * it is used only for the checksum calculation
     */
    if (bcmBFDTunnelTypeMpls == endpoint_info->type)
    {
        /*
         * In case of mpls, the address should be random. Then, we prepare here the subnet part of the address and
         * leave room for the random part (host part). There is an OAMP-PE program that fills the random part in the
         * host bits of the address.
         * The OAMP-PE program changes the first least significant byte of the address to be random and the third
         * least significant byte to be the one's complement of it
         * and their sum will be 0xff.
         * Then, we put 0xff in the third least significant byte of the address which causes the same checksum result.
         */
        bcm_ip6_t addr = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xff, 0xff, 0x7f, 0xff, 0x0, 0x0 };
        sal_memcpy(ipv6_header->destination_address, addr, sizeof(bcm_ip6_t));
        sal_memcpy(ipv6_pseudo_header->destination_address, addr, sizeof(bcm_ip6_t));
    }
    else
    {
        sal_memcpy(ipv6_header->destination_address, endpoint_info->dst_ip6_addr, sizeof(bcm_ip6_t));
        sal_memcpy(ipv6_pseudo_header->destination_address, endpoint_info->dst_ip6_addr, sizeof(bcm_ip6_t));
    }

    ipv6_pseudo_header->protocol = ipv6_header->next_header;
    ipv6_pseudo_header->udp_length = ipv6_header->payload_length;

    dnx_bfd_oamp_ipv6_header_to_array_of_bytes(ipv6_header_packed, ipv6_header);
    dnx_bfd_oamp_ipv6_pseduo_header_to_array_of_bytes(ipv6_pseudo_header_packed, ipv6_pseudo_header);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_BFD_CONFIGURATION, &entry_handle_id));
    if (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR))
    {
        /** Seamless BFD - get the source port from the template */
        SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_seamless_udp_sport.profile_data_get(unit, DISCR_PROF, &dummy, &port));
        udp_src_port = port;
    }
    else
    {
        /*
         * the OAMP takes the UDP source port from the static configuration table (see dbal table OAMP_BFD_CONFIGURATION)
         */

        if (bcmBFDTunnelTypeMpls == endpoint_info->type)
        {
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAMP_BFD_MPLS_UDP_SPORT, INST_SINGLE,
                                       &udp_src_port);
        }
        else
        {
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAMP_BFD_IPV4_UDP_SPORT, INST_SINGLE,
                                       &udp_src_port);
        }
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }

    DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_OAMP_BFD_CONFIGURATION, entry_handle_id);
    udp_src_port = bcm_htonl(udp_src_port);
    sal_memcpy(&udp_header->source_port, ((uint16 *) &udp_src_port) + 1, sizeof(uint16));

    if (endpoint_info->flags & BCM_BFD_ECHO)
    {
        udp_dst_port = DBAL_DEFINE_UDP_PORT_BFD_ECHO;
    }
    else if (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR))
    {
        udp_dst_port = SBFD_UDP_DEST_PORT;
    }
    else
    {
        udp_dst_port = DNX_BFD_OAMP_BFD_UDP_PORT(endpoint_info->flags);
    }
    udp_header->destination_port = bcm_htons(udp_dst_port);
    udp_header->length = ipv6_header->payload_length;

    dnx_bfd_oamp_udp_header_to_array_of_bytes(udp_header_packed, udp_header);
    /*
     * the OAMP takes the bfd version, the bfd length and the min echo rx fields values from the static configuration
     * table (see dbal table OAMP_BFD_CONFIGURATION)
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAMP_BFD_VERSION, INST_SINGLE, &bfd_vers);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAMP_BFD_LENGTH, INST_SINGLE, &bfd_length);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAMP_BFD_REQ_MIN_ECHO_RX_INTERVAL, INST_SINGLE,
                               &bfd_min_echo_rx);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    bfd_header->vers_diag =
        ((bfd_vers & ((1 << DNX_BFD_OAMP_BFD_VERSION_NOF_BITS) - 1)) << DNX_BFD_OAMP_BFD_DIAG_NOF_BITS) +
        endpoint_info->local_diag;
    bfd_header->sta_flags =
        (endpoint_info->local_state << DNX_BFD_OAMP_BFD_FLAGS_NOF_BITS) + endpoint_info->local_flags;
    bfd_header->detect_mult = endpoint_info->local_detect_mult;
    bfd_header->length = bfd_length;

    bfd_header->my_discriminator = bcm_htonl(endpoint_info->local_discr);
    bfd_header->your_discriminator = bcm_htonl(endpoint_info->remote_discr);

    bfd_header->desired_min_tx_interval = bcm_htonl(endpoint_info->local_min_tx);

    bfd_header->required_min_rx_interval = bcm_htonl(endpoint_info->local_min_rx);

    bfd_header->required_min_echo_rx_interval = bcm_htonl(bfd_min_echo_rx);

    dnx_bfd_oamp_bfd_header_to_array_of_bytes(bfd_header_packed, bfd_header);

    index = 0;
    sal_memcpy(checksum_header_packed, ipv6_pseudo_header_packed, DNX_BFD_IPV6_PSEUDO_HEADER_SIZE);
    index += DNX_BFD_IPV6_PSEUDO_HEADER_SIZE;
    sal_memcpy(checksum_header_packed + index, udp_header_packed, DNX_BFD_UDP_HEADER_SIZE);
    index += DNX_BFD_UDP_HEADER_SIZE;
    sal_memcpy(checksum_header_packed + index, bfd_header_packed, DNX_BFD_BFD_HEADER_SIZE);

    /*
     * update the checksum field. for more information see dnx_bfd_oamp_ones_comp_checksum
     */
    udp_header->checksum =
        bcm_htons(dnx_bfd_oamp_ones_comp_checksum((uint16 *) checksum_header_packed,
                                                  (DNX_BFD_CHECKSUM_HEADER_SIZE) / sizeof(uint16)));

    /** These two bytes are a forced big endian representation, regardless of processor endianness */
    big_endian_checksum[0] = UTILEX_GET_BITS_RANGE(udp_header->checksum,
                                                   2 * UTILEX_NOF_BITS_IN_BYTE - 1, UTILEX_NOF_BITS_IN_BYTE);
    big_endian_checksum[1] = UTILEX_GET_BITS_RANGE(udp_header->checksum, UTILEX_NOF_BITS_IN_BYTE - 1, 0);

    sal_memcpy(udp_header_packed + sizeof(udp_header->source_port) + sizeof(udp_header->destination_port) +
               sizeof(udp_header->length), big_endian_checksum, sizeof(udp_header->checksum));

    sal_memcpy(ipv6_extra_data_header, ipv6_header_packed, DNX_BFD_IPV6_HEADER_SIZE);
    /*
     * save the checksum in the payload length's field
     */
    index = DNX_BFD_IPV6_HEADER_PAYLOAD_LENGTH_OFFSET;
    sal_memcpy(ipv6_extra_data_header + index, big_endian_checksum, sizeof(udp_header->checksum));
    /*
     * save udp src_port in the flow label's field
     */
    index = DNX_BFD_IPV6_HEADER_FLOW_LABEL_OFFSET;
    sal_memcpy(ipv6_extra_data_header + index, &udp_header->source_port, sizeof(udp_header->source_port));

    /*
     * set the ls bit of the second byte to 1 if echo
     */
    if (endpoint_info->flags & BCM_BFD_ECHO)
    {
        ipv6_extra_data_header[1] |= 1;
    }

exit:
    SHR_FREE(ipv6_header);
    SHR_FREE(ipv6_header_packed);
    SHR_FREE(ipv6_pseudo_header);
    SHR_FREE(ipv6_pseudo_header_packed);
    SHR_FREE(udp_header);
    SHR_FREE(udp_header_packed);
    SHR_FREE(bfd_header);
    SHR_FREE(bfd_header_packed);
    SHR_FREE(checksum_header_packed);

    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_oamp_mep_db_extra_data_ipv6_set(
    int unit,
    uint32 mep_id,
    uint8 *data,
    uint16 crc,
    uint8 is_update)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_extra_data_set(unit,
                                                   OAM_ID_TO_MEP_DB_ENTRY(mep_id),
                                                   DNX_BFD_IPV6_HEADER_SIZE * UTILEX_NOF_BITS_IN_BYTE,
                                                   data,
                                                   (1 << DNX_BFD_OAMP_EXTRA_DATA_HEADER_OPCODES_TO_PREPEND_NOF_BITS)
                                                   - 1, crc, 0, is_update));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_oamp_mep_db_extra_data_clear(
    int unit,
    uint32 entry_id)
{
    uint32 data_length = 0;
    uint32 entry_handle_id = 0;
    uint32 next_bank_offset = MEP_DB_ENTRY_TO_OAM_ID(dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit));
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_ENTRY_ID, entry_id);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_EXT_DATA_LENGTH, INST_SINGLE, &data_length));

    do
    {
        /** Clear MEP DB entry   */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_delete(unit, entry_id));
        /** Release the OAMP_MEP_DB entry id */
        SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_dealloc(unit, entry_id));
        entry_id += next_bank_offset;
        data_length--;
    }
    while (data_length > 0);
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in bfd_oamp.h for function description
 */
shr_error_e
dnx_oamp_mep_db_bfd_endpoint_set(
    int unit,
    const bcm_bfd_endpoint_info_t * endpoint_info,
    dnx_bfd_oamp_endpoint_t * entry_values)
{
    uint32 your_disc_msb, your_disc_lsb;
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    uint8 *ipv6_extra_data_header = NULL;
    int ipv6_extra_data_index_orig = entry_values->ipv6_extra_data_index;
    uint16 calculated_crc = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_values, _SHR_E_PARAM, "entry_values");
    SHR_ALLOC_SET_ZERO(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t), "Param struct for creating single entries",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_BFD_MEP_UPDATE))
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, endpoint_info->id, oamp_params));
    }

    oamp_params->mep_type = entry_values->mep_type;
    oamp_params->flags = 0;

/*
 *       BFD over IPv6 update algorithm
 *        ________________  ____
 *     A |________________ |_p_ |            extra data
 *                           |         B   _______________
 *                            \___________|_______________|
 *                                         _______________
 *                                     C  |_______________|  (free pair)
 *
 *       1) C <- find free pair (in consecutive banks)
 *       2) update C's content
 *       3) update A (with p <- C)
 *       4) update B's content
 *       5) p <- B
 *       6) delete C's entries
 */
    if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_BFD_MEP_UPDATE)
        && _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_BFD_IPV6))
    {
        int bank_index = 0;
        int sub_index = 0;
        int ii = 0, nof_free_tuples = 0;
        int nof_oamp_full_tuples_mep_id_banks;
        int nof_bfd_ipv6_extra_data_entries = DNX_BFD_IPV6_NOF_EXTRA_DATA_ENTRIES;
        int nof_mep_db_entries_per_bank = dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit);
        int current_mep_id_bank_index =
            OAM_ID_TO_MEP_DB_ENTRY(endpoint_info->id -
                                   dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit)) /
            nof_mep_db_entries_per_bank;
        algo_oamp_db.nof_oamp_full_tuples_mep_id_banks.get(unit, &nof_oamp_full_tuples_mep_id_banks);

        /*
         * find free pair to use as temporary memory for the update operation
         */

        for (ii = 0; ii < nof_oamp_full_tuples_mep_id_banks; ++ii)
        {
            /*
             * the tuple's banks must not intersect the endpoint entry's bank
             */
            if (ii <= current_mep_id_bank_index - nof_bfd_ipv6_extra_data_entries || ii > current_mep_id_bank_index)
            {
                algo_oamp_db.oamp_full_tuples_mep_id.nof_free_elements_get(unit, ii, &nof_free_tuples);
                if (nof_free_tuples != 0)
                {
                    bank_index = ii;
                    SW_STATE_ALGO_OAMP_FULL_TUPLES_MEP_ID.allocate_single(unit, ii, 0, NULL, &sub_index);
                    break;
                }
            }
        }
        if (ii == nof_oamp_full_tuples_mep_id_banks)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error: BFD over IPv6 update failed.\n"
                         "BFD over IPv6 update needs two entries in adjacent banks as temporary memory.\n"
                         "No such pair was found.\n");
        }

        entry_values->ipv6_extra_data_index =
            dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit) +
            MEP_DB_ENTRY_TO_OAM_ID(bank_index * nof_mep_db_entries_per_bank + sub_index);
    }

    UTILEX_SET_BIT(oamp_params->flags, _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_UPDATE),
                   OAMP_MEP_UPDATE);
    UTILEX_SET_BIT(oamp_params->flags, _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_BFD_MEP_Q_ENTRY),
                   OAMP_MEP_Q_ENTRY);
    /** Condition for setting OAMP_MEP_BFD_TUNNEL_MPLS_ENABLE (TX packet with TTL 1):
     * ip_ttl field in endpoint_create is 1
     * endpoint type is IP 1HOP*/
    UTILEX_SET_BIT(oamp_params->flags,
                   (entry_values->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_1HOP && endpoint_info->ip_ttl == 1),
                   OAMP_MEP_BFD_TUNNEL_MPLS_ENABLE);
    UTILEX_SET_BIT(oamp_params->flags, _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_BFD_MEP_MICRO_BFD),
                   OAMP_MEP_MICRO_BFD);
    UTILEX_SET_BIT(oamp_params->flags, _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_BFD_ECHO), OAMP_MEP_BFD_ECHO);
    UTILEX_SET_BIT(oamp_params->flags, _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_BFD_MEP_ACH), OAMP_MEP_ACH);
    UTILEX_SET_BIT(oamp_params->flags, _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_BFD_MEP_GAL), OAMP_MEP_GAL);
    UTILEX_SET_BIT(oamp_params->flags, _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_BFD_MEP_ROUTER_ALERT),
                   OAMP_MEP_ROUTER_ALERT);
    UTILEX_SET_BIT(oamp_params->flags, UTILEX_GET_BIT(endpoint_info->local_discr, 31), OAMP_MEP_SET_DISCR_MSB);

    UTILEX_SET_BIT(oamp_params->flags,
                   _SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR),
                   OAMP_MEP_SEAMLESS_BFD);

    if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_BFD_DESTINATION_IS_FEC))
    {
        oamp_params->fec_id_or_glob_out_lif = entry_values->fec_id_or_glob_out_lif.fec_id;
        UTILEX_SET_BIT(oamp_params->fec_id_or_glob_out_lif, TRUE, FIELD_IS_FEC_ID_FLAG);
    }
    else
    {
        oamp_params->fec_id_or_glob_out_lif = entry_values->fec_id_or_glob_out_lif.glob_out_lif;
    }

    if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_BFD_IPV6))
    {
        /** The extra_data_header field points to a MEP DB entry, not an OAM ID */
        oamp_params->extra_data_ptr = OAM_ID_TO_MEP_DB_ENTRY(entry_values->ipv6_extra_data_index);
    }

    oamp_params->mpls_pwe.label = entry_values->label;
    oamp_params->exclusive.bfd_only.bfd_your_disc = entry_values->your_disc;
    oamp_params->exclusive.bfd_only.bfd_ipv4_dip = entry_values->ipv4_dip;
    oamp_params->dest_sys_port_agr = entry_values->dest_sys_port_agr;
    oamp_params->exclusive.bfd_only.bfd_sta = entry_values->sta;
    oamp_params->itmh_tc_dp_profile = entry_values->itmh_tc_dp_profile;
    oamp_params->exclusive.bfd_only.bfd_ip_ttl_tos_profile = entry_values->ip_ttl_tos_profile;
    oamp_params->mep_profile = entry_values->mep_profile;
    oamp_params->mep_pe_profile = entry_values->mep_pe_profile;
    oamp_params->unified_port_access.port_profile = entry_values->pp_port_profile;
    oamp_params->ip_subnet_len = entry_values->ip_subnet_len;
    oamp_params->crps_core_select = entry_values->crps_core_select;
    oamp_params->mpls_pwe.push_profile = entry_values->push_profile;
    oamp_params->exclusive.bfd_only.bfd_tx_rate = entry_values->tx_rate_profile;
    oamp_params->exclusive.bfd_only.bfd_src_ip_profile = entry_values->src_ip_profile;
    oamp_params->exclusive.bfd_only.bfd_detect_mult = entry_values->detect_mult;
    oamp_params->exclusive.bfd_only.bfd_min_tx_interval_profile = entry_values->min_tx_interval_profile;
    oamp_params->exclusive.bfd_only.bfd_min_rx_interval_profile = entry_values->min_rx_interval_profile;
    oamp_params->exclusive.bfd_only.bfd_diag_profile = entry_values->diag_profile;
    oamp_params->exclusive.bfd_only.bfd_flags_profile = entry_values->flags_profile;
    oamp_params->exclusive.bfd_only.bfd_ach_sel = entry_values->ach_sel;
    if (!_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_BFD_MEP_UPDATE))
    {
        /** If updating, just use value from entry */
        oamp_params->flex_lm_dm_ptr = entry_values->mpls_lm_dm_entry;
    }

    /** Is this a 1/4 entry?   */
    if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_BFD_MEP_Q_ENTRY) == 1)
    {
        /**
         *  For 1/4 entry BFD, your-discriminator is split between
         *  the MEP DB (26 bits) and the MDB (6 bits)
         */
        your_disc_lsb =
            UTILEX_GET_BITS_RANGE(oamp_params->exclusive.bfd_only.bfd_your_disc, YOUR_DISC_LSB_NOF_BITS - 1, 0);
        your_disc_msb =
            UTILEX_GET_BITS_RANGE(oamp_params->exclusive.bfd_only.bfd_your_disc, sizeof(uint32) - 1,
                                  YOUR_DISC_LSB_NOF_BITS);

        /** Write MSB to MDB   */
        oamp_params->exclusive.bfd_only.bfd_your_disc = your_disc_msb;

        /*
         * in case of BFD over IPv6 we save the UDP and IPv6 headers as extra data of the entry
         */
        if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_BFD_IPV6))
        {
            uint16 calculated_crc = 0;
            SHR_ALLOC_SET_ZERO(ipv6_extra_data_header, DNX_BFD_IPV6_HEADER_SIZE,
                               "data allocated for BFD over IPv6 implementation", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
            dnx_bfd_oamp_udp_over_ipv6_external_data(unit, endpoint_info->id, endpoint_info, ipv6_extra_data_header);

            SHR_IF_ERR_EXIT(dnx_oam_oamp_crc16(unit, (uint8 *) endpoint_info->dst_ip6_addr, DNX_BFD_BFD_HEADER_SIZE     /* start
                                                                                                                         * * * *
                                                                                                                         * index */ ,
                                               sizeof(bcm_ip6_t) /* data size */ ,
                                               &calculated_crc));

            dnx_oamp_mdb_extra_data_set(unit, endpoint_info->id, 0 /* not offloaded */ , 1 /* 1/4 entry */ ,
                                        calculated_crc,
                                        (1 << DNX_BFD_OAMP_EXTRA_DATA_HEADER_OPCODES_TO_PREPEND_NOF_BITS) - 1,
                                        DNX_BFD_IPV6_HEADER_SIZE * UTILEX_NOF_BITS_IN_BYTE,
                                        ipv6_extra_data_header,
                                        _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_BFD_MEP_UPDATE));
        }

        /** 1/4 MEP DB entry.  Add MDB entry   */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_add(unit, endpoint_info->id, oamp_params));

        /** Write LSB to MEP DB   */
        oamp_params->exclusive.bfd_only.bfd_your_disc = your_disc_lsb;
    }

    /*
     * in case of BFD over IPv6 we save the UDP and IPv6 headers as extra data of the entry
     */
    if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_BFD_IPV6)
        && !_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_BFD_MEP_Q_ENTRY))
    {
        SHR_ALLOC_SET_ZERO(ipv6_extra_data_header, DNX_BFD_IPV6_HEADER_SIZE,
                           "data allocated for BFD over IPv6 implementation", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
        dnx_bfd_oamp_udp_over_ipv6_external_data(unit, endpoint_info->id, endpoint_info, ipv6_extra_data_header);

        SHR_IF_ERR_EXIT(dnx_oam_oamp_crc16(unit, (uint8 *) endpoint_info->dst_ip6_addr, DNX_BFD_BFD_HEADER_SIZE /* start
                                                                                                                 * *
                                                                                                                 * index */ ,
                                           sizeof(bcm_ip6_t) /* data size */ ,
                                           &calculated_crc));

        SHR_IF_ERR_EXIT(dnx_oamp_mep_db_extra_data_ipv6_set(unit, entry_values->ipv6_extra_data_index,
                                                            ipv6_extra_data_header, calculated_crc, 0));
    }

    /** Add MEP DB entry   */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, endpoint_info->id, oamp_params));

    if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_BFD_MEP_UPDATE)
        && _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_BFD_IPV6))
    {
        SHR_IF_ERR_EXIT(dnx_oamp_mep_db_extra_data_ipv6_set
                        (unit, ipv6_extra_data_index_orig, ipv6_extra_data_header, calculated_crc, 1));
        oamp_params->extra_data_ptr = OAM_ID_TO_MEP_DB_ENTRY(ipv6_extra_data_index_orig);
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, endpoint_info->id, oamp_params));
        SHR_IF_ERR_EXIT(dnx_oamp_mep_db_extra_data_clear(unit, entry_values->ipv6_extra_data_index));
    }

exit:
    SHR_FREE(ipv6_extra_data_header);
    SHR_FREE(oamp_params);
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in bfd_oamp.h for function description
 */
shr_error_e
dnx_oamp_mep_db_bfd_endpoint_get(
    int unit,
    uint16 oam_id,
    dnx_bfd_oamp_endpoint_t * entry_values)
{
    uint32 your_disc_msb, your_disc_lsb;
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    uint8 *data = NULL;
    uint8 is_ipv6 = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_values, _SHR_E_PARAM, "entry_values");
    SHR_ALLOC_SET_ZERO(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t), "Param struct for creating single entries",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(data, OAM_OAMP_EXTRA_DATA_MAX_SIZE, "extra data for BFD", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** Read MEP DB entry   */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_params));

    /** Is this a 1/4 entry?   */
    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
    {
        /**
         *  For 1/4 entry BFD, your-discriminator is split between
         *  the MEP DB (26 bits) and the MDB (6 bits)
         */
        your_disc_lsb = oamp_params->exclusive.bfd_only.bfd_your_disc;

        /** 1/4 MEP DB entry.  Get MDB entry   */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_get(unit, oam_id, MDB_Q_ENTRY_ACCESS, oamp_params));

        /** 6 most-signficant-bits read from MDB   */
        your_disc_msb = oamp_params->exclusive.bfd_only.bfd_your_disc;

        oamp_params->exclusive.bfd_only.bfd_your_disc = your_disc_lsb | (your_disc_msb << YOUR_DISC_LSB_NOF_BITS);
    }

    if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_BFD_IPV6))
    {
        is_ipv6 = TRUE;
    }

    /** Convert parameters   */
    entry_values->mep_type = oamp_params->mep_type;
    if (is_ipv6)
    {
        entry_values->flags = DNX_OAMP_BFD_IPV6;
    }
    else
    {
        entry_values->flags = 0;
    }

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
    {
        entry_values->flags |= DNX_OAMP_BFD_MEP_Q_ENTRY;
    }

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_MICRO_BFD))
    {
        entry_values->flags |= DNX_OAMP_BFD_MEP_MICRO_BFD;
    }

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_BFD_ECHO))
    {
        entry_values->flags |= DNX_OAMP_BFD_ECHO;
    }

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_ACH))
    {
        entry_values->flags |= DNX_OAMP_BFD_MEP_ACH;
    }

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_GAL))
    {
        entry_values->flags |= DNX_OAMP_BFD_MEP_GAL;
    }

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_ROUTER_ALERT))
    {
        entry_values->flags |= DNX_OAMP_BFD_MEP_ROUTER_ALERT;
    }

    if (UTILEX_GET_BIT(oamp_params->fec_id_or_glob_out_lif, FIELD_IS_FEC_ID_FLAG))
    {
        entry_values->fec_id_or_glob_out_lif.fec_id = oamp_params->fec_id_or_glob_out_lif;
        /** Clear this bit - it's not part of the field */
        UTILEX_SET_BIT(entry_values->fec_id_or_glob_out_lif.fec_id, 0, FIELD_IS_FEC_ID_FLAG);
        entry_values->flags |= DNX_OAMP_BFD_DESTINATION_IS_FEC;
    }
    else
    {
        entry_values->fec_id_or_glob_out_lif.glob_out_lif = oamp_params->fec_id_or_glob_out_lif;
    }

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_BFD_TUNNEL_MPLS_ENABLE))
    {
        entry_values->flags |= DNX_OAMP_BFD_1HOP_MPLS_TUNNEL_ENABLE;
    }

    entry_values->label = oamp_params->mpls_pwe.label;
    entry_values->your_disc = oamp_params->exclusive.bfd_only.bfd_your_disc;
    entry_values->ipv4_dip = oamp_params->exclusive.bfd_only.bfd_ipv4_dip;
    entry_values->dest_sys_port_agr = oamp_params->dest_sys_port_agr;
    entry_values->sta = oamp_params->exclusive.bfd_only.bfd_sta;
    entry_values->itmh_tc_dp_profile = oamp_params->itmh_tc_dp_profile;
    entry_values->ip_ttl_tos_profile = oamp_params->exclusive.bfd_only.bfd_ip_ttl_tos_profile;
    entry_values->mep_profile = oamp_params->mep_profile;
    entry_values->mep_pe_profile = oamp_params->mep_pe_profile;
    entry_values->pp_port_profile = oamp_params->unified_port_access.port_profile;
    entry_values->ip_subnet_len = oamp_params->ip_subnet_len;
    entry_values->push_profile = oamp_params->mpls_pwe.push_profile;
    entry_values->tx_rate_profile = oamp_params->exclusive.bfd_only.bfd_tx_rate;
    entry_values->src_ip_profile = oamp_params->exclusive.bfd_only.bfd_src_ip_profile;
    entry_values->detect_mult = oamp_params->exclusive.bfd_only.bfd_detect_mult;
    entry_values->min_tx_interval_profile = oamp_params->exclusive.bfd_only.bfd_min_tx_interval_profile;
    entry_values->min_rx_interval_profile = oamp_params->exclusive.bfd_only.bfd_min_rx_interval_profile;
    entry_values->diag_profile = oamp_params->exclusive.bfd_only.bfd_diag_profile;
    entry_values->flags_profile = oamp_params->exclusive.bfd_only.bfd_flags_profile;
    entry_values->ach_sel = oamp_params->exclusive.bfd_only.bfd_ach_sel;
    entry_values->crps_core_select = oamp_params->crps_core_select;
    entry_values->ipv6_extra_data_index = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->extra_data_ptr);

    if (is_ipv6)
    {
        if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
        {
            SHR_IF_ERR_EXIT(dnx_oamp_mdb_extra_data_get
                            (unit, oam_id, DNX_BFD_IPV6_HEADER_SIZE * UTILEX_NOF_BITS_IN_BYTE, data));
        }
        else if (entry_values->ipv6_extra_data_index != 0)
        {
            SHR_IF_ERR_EXIT(dnx_oamp_mep_db_extra_data_get(unit, oamp_params->extra_data_ptr, data, NULL, NULL));
        }
        sal_memcpy(entry_values->ipv6_sip, data + DNX_BFD_IPV6_SOUCRE_ADDRESS_OFFSET, sizeof(bcm_ip6_t));
        sal_memcpy(entry_values->ipv6_dip, data + DNX_BFD_IPV6_DESTINATION_ADDRESS_OFFSET, sizeof(bcm_ip6_t));
    }

exit:
    SHR_FREE(data);
    SHR_FREE(oamp_params);
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in bfd_oamp.h for function description
 */
shr_error_e
dnx_oamp_mep_db_bfd_endpoint_clear(
    int unit,
    uint16 oam_id,
    uint8 is_ipv6)
{
    dnx_bfd_oamp_endpoint_t *entry_values = NULL;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(entry_values, sizeof(dnx_bfd_oamp_endpoint_t),
                       "Endpoint data read to find MDB entries before deleting", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    if (is_ipv6)
    {
        entry_values->flags = DNX_OAMP_BFD_IPV6;
    }
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_bfd_endpoint_get(unit, oam_id, entry_values));

    if (is_ipv6 && !_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_BFD_MEP_Q_ENTRY))
    {
        SHR_IF_ERR_EXIT(dnx_oamp_mep_db_extra_data_clear(unit, entry_values->ipv6_extra_data_index));
    }

    if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_BFD_MEP_Q_ENTRY) == 1)
    {
        int i;
        /** 1/4 MEP DB entry.  Delete MDB entry   */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_delete(unit, oam_id, MDB_Q_ENTRY_ACCESS));
        if (is_ipv6)
        {
            /** in case of BFD_IPV6 entry there are also extra data header and extra data payload entries
             *  hence, in this case we remove all the entries of this oam_id
             */
            for (i = MDB_EXTRA_DATA_HDR_ACCESS; i <= MDB_EXTRA_DATA_PLD3_ACCESS; i++)
            {
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_delete(unit, oam_id, i));
            }
        }
    }

    /** Clear MEP DB entry   */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_delete(unit, oam_id));

exit:
    SHR_FREE(entry_values);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This is a utility function for the BFD period cluster
 *          feature.  Since each EP in a cluster may use one of
 *          four possible MEP profiles, this function chooses
 *          which profile is right for the EP.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_id - ID of BFD EP
 * \param [in] is_ipv6 - 1 for IPv6, 0 otherwise
 * \param [out] resulting_profile_field - pointer to first field
 *        ID.   This field in the BFD_TX_PERIOD_CLUSTER_INFO is
 *        a profile index field for the relevant MEP profile.
 *        If pointer is NULL, no action is taken.
 * \param [out] resulting_valid_field - pointer to second field
 *        ID.  This field in the BFD_TX_PERIOD_CLUSTER_INFO is
 *        a valid field for the relevant MEP profile.  If pointer
 *        is NULL, no action is taken.
 *
 * \return
 *   None
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static void
dnx_bfd_tx_period_cluster_get_mep_profile_field(
    int unit,
    int endpoint_id,
    uint8 is_ipv6,
    dbal_fields_e * resulting_profile_field,
    dbal_fields_e * resulting_valid_field)
{
    int full_entry_threshold;
    dbal_fields_e temp_resulting_profile_field, temp_resulting_valid_field;

    full_entry_threshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);

    /**
     * CCM count is expected to be 1 if the entry is above the threshold and is
     * "odd."  Since entries above the threshold are all multiplies of 4, "odd"
     * means "not a multiple of 8."
     */
    if ((endpoint_id >= full_entry_threshold) && (((endpoint_id >> 2) & 1) != 0))
    {
        /** Expected CCM count is 1, if packets are transmitted from EP */
        if (is_ipv6)
        {
            temp_resulting_profile_field = DBAL_FIELD_MEP_PROFILE_IPV6_COUNT_1;
            temp_resulting_valid_field = DBAL_FIELD_MEP_PROFILE_IPV6_COUNT_1_VALID;
        }
        else
        {
            temp_resulting_profile_field = DBAL_FIELD_MEP_PROFILE_NO_IPV6_COUNT_1;
            temp_resulting_valid_field = DBAL_FIELD_MEP_PROFILE_NO_IPV6_COUNT_1_VALID;
        }
    }
    else
    {
        /** Expected CCM count is 0, if packets are transmitted from EP */
        if (is_ipv6)
        {
            temp_resulting_profile_field = DBAL_FIELD_MEP_PROFILE_IPV6_COUNT_0;
            temp_resulting_valid_field = DBAL_FIELD_MEP_PROFILE_IPV6_COUNT_0_VALID;
        }
        else
        {
            temp_resulting_profile_field = DBAL_FIELD_MEP_PROFILE_NO_IPV6_COUNT_0;
            temp_resulting_valid_field = DBAL_FIELD_MEP_PROFILE_NO_IPV6_COUNT_0_VALID;
        }
    }

    if (resulting_profile_field != NULL)
    {
        *resulting_profile_field = temp_resulting_profile_field;
    }

    if (resulting_valid_field != NULL)
    {
        *resulting_valid_field = temp_resulting_valid_field;
    }
}

/**
 * \brief - This function prepares to add a BFD Tx Rate entry to
 *        the Tx Rate hardware table, or if that entry already
 *        exists, increases the relevant counter.  This is
 *        achieved by using the template "BFD Tx Rate"
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - see definition of struct for
 *        description of fields
 * \param [out] mep_hw_write_data - Pointer to struct where data
 *        for writing to HW tables is placed.  This data is
 *        relevant to fields in the OAMP MEP DB.
 * \param [in] current_profile - Profile ID used by the endpoint
 *        prior to modification.  Used only for modifying existing
 *        endpoints, and only with a non-zero cluster ID.
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
dnx_bfd_tx_rate_profile_sw_update(
    int unit,
    const bcm_bfd_endpoint_info_t * endpoint_info,
    bfd_temp_oamp_mep_db_data_t * mep_hw_write_data,
    uint8 current_profile)
{
    shr_error_e rv;
    dnx_bfd_tx_period_t tx_rate_data;
    uint32 entry_handle_id, new_tx_rate, cluster_tx_rate_id;
    uint32 mep_profile, cluster_tx_period;
    int tx_rate_id, dummy, new_cluster_profile, loop_index;
    dbal_fields_e mep_profile_field;
    uint8 write_hw, first_reference = FALSE;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (endpoint_info->bfd_period == 0)
    {
        /**
         *  If the user sets bfd_period==0, the endpoint should not transmit packets.
         *  However, if BFD_TX_RATE_PROFILE points to a profile that has a 0 Tx interval,
         *  Packets from the endpoint are sent at the maximum rate.  To overcome this, scan
         *  profile is set to 1, and since count==1 never happens at maximum rate, packets
         *  will not be transmitted.  See also dnx_bfd_mep_profile_sw_update.
         */
        new_tx_rate = 0;
    }
    else
    {
        new_tx_rate = BCM_OAMP_SW_ENTRY_INFO_PERIOD_TO_MEP_DB_SCAN_RATE(endpoint_info->bfd_period);
    }

    /** Which MEP profile do we use? */
    dnx_bfd_tx_period_cluster_get_mep_profile_field(unit, endpoint_info->id,
                                                    _SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_IPV6),
                                                    &mep_profile_field, NULL);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BFD_TX_PERIOD_CLUSTER_INFO, &entry_handle_id));

    if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_UPDATE) && (endpoint_info->bfd_period_cluster != 0))
    {
        /**
         * If an endpoint that belongs to a cluster is being modified,
         * the modification will affect all the endpoints in the cluster.
         * The data in the template, and later in the physical table,
         * will be modified
         */
        SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_tx_rate.profile_data_get(unit, current_profile, &dummy, &tx_rate_data));

        tx_rate_data.tx_period = new_tx_rate;
        SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_tx_rate.replace_data(unit, current_profile, &tx_rate_data));
        tx_rate_id = current_profile;
        write_hw = TRUE;
    }
    else
    {
        /**
         * Either a new endpoint is being added to a cluster,
         * the endpoint is the first in a new cluster,
         * or an endpoint which is not in a cluster is being
         * created or modified
         */
        tx_rate_data.tx_period = new_tx_rate;
        if (endpoint_info->bfd_period_cluster != 0)
        {
            /** A new cluster endpoint */
            rv = algo_bfd_db.bfd_tx_period_cluster.allocate_single
                (unit, NO_FLAGS, (uint8 *) &endpoint_info->bfd_period_cluster, NULL, &new_cluster_profile,
                 &first_reference);
            tx_rate_data.cluster_profile = new_cluster_profile;
            if (rv == _SHR_E_RESOURCE)
            {
                /** Resource error means no match - display error message */
                SHR_ERR_EXIT(_SHR_E_RESOURCE, "Error: No free cluster entries available.\n");
            }
            else
            {
                /** All other errors handled generically */
                SHR_IF_ERR_EXIT(rv);
            }
        }
        else
        {
            /** a non-cluster endpoint */
            tx_rate_data.cluster_profile = 0xFF;
        }

        rv = algo_bfd_db.bfd_oamp_tx_rate.allocate_single(unit, NO_FLAGS, &tx_rate_data, NULL, &tx_rate_id, &write_hw);
        if (rv == _SHR_E_RESOURCE)
        {
            /** Resource error means no match - display error message */
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "Error: No free Tx rate entries available.\n");
        }
        else
        {
            /** All other errors handled generically */
            SHR_IF_ERR_EXIT(rv);
        }

        if (tx_rate_data.cluster_profile != 0xFF)
        {
            if (!first_reference && write_hw)
            {
                /** User error - bfd_period does not match cluster.  Build error message */

                /** Get existing cluster's Tx rate profile from SW state table */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_PERIOD_CLUSTER_INDEX,
                                           new_cluster_profile);
                dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_EXT_DATA_LENGTH, INST_SINGLE,
                                           &cluster_tx_rate_id);
                dbal_value_field32_request(unit, entry_handle_id, mep_profile_field, INST_SINGLE, &mep_profile);
                SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

                /** Get tx rate value from template */
                SHR_IF_ERR_EXIT(algo_bfd_db.
                                bfd_oamp_tx_rate.profile_data_get(unit, cluster_tx_rate_id, &dummy, &tx_rate_data));

                /** Get relevant value from mep profile */
                algo_oam_db.oam_mep_profile.profile_data_get
                    (unit, mep_profile, &dummy, &mep_hw_write_data->mep_profile_data);

                /** Calculate Tx period for cluster */
                cluster_tx_period = dnx_bfd_get_bfd_period(unit, endpoint_info->id, tx_rate_data.tx_period,
                                                           mep_hw_write_data->mep_profile_data.ccm_count);

                SHR_ERR_EXIT(_SHR_E_PARAM, "Error, cluster %d already exists, and has a different tx_period.\n"
                             "Cluster Tx period: 0x%08x, requested Tx period: 0x%08x\n",
                             endpoint_info->bfd_period_cluster, cluster_tx_period, endpoint_info->bfd_period);
            }

            if (first_reference && !write_hw)
            {
                /** How can this Tx period profile have this cluster, when it doesn't exist yet? */
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error, internal problem with cluster feature.\n");
            }
        }
    }

    mep_hw_write_data->tx_rate_write = write_hw;

    /** This will definitely be written to the MEP DB   */
    mep_hw_write_data->tx_rate_write_index = tx_rate_id;

    if (write_hw == TRUE)
    {
        /** Save data for writing to HW table at the end   */
        mep_hw_write_data->tx_rate_write_value = tx_rate_data.tx_period;

        if (first_reference)
        {
            /** Create an entry in the SW state table */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_PERIOD_CLUSTER_INDEX, new_cluster_profile);

            dbal_entry_value_field32_set(unit, entry_handle_id,
                                         DBAL_FIELD_BFD_TX_RATE_PROFILE, INST_SINGLE, tx_rate_id);

            /** Set all the valid bits to 0 */
            for (loop_index = 0; loop_index < 4; loop_index++)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id,
                                             mep_profile_valid_fields[loop_index], INST_SINGLE, 0);
            }

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function prepares to add a BFD Req Int entry
 *        to the Req Int hardware table, or if that entry
 *        already exists, increases the relevant counter.  This
 *        is achieved by using the template "BFD Required
 *        Interval."
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - see definition of struct for
 *        description of fields
 * \param [out] mep_hw_write_data - Pointer to struct where data
 *        for writing to HW tables is placed.  This data is
 *        relevant to fields in the OAMP MEP DB.
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
dnx_bfd_req_int_profile_sw_update(
    int unit,
    const bcm_bfd_endpoint_info_t * endpoint_info,
    bfd_temp_oamp_mep_db_data_t * mep_hw_write_data)
{
    shr_error_e rv;
    uint32 req_ints[2];
    int req_int_id, index;
    uint8 write_hw;
    SHR_FUNC_INIT_VARS(unit);

    req_ints[REQ_MIN_INT_RX] = endpoint_info->local_min_rx;
    req_ints[DES_MIN_INT_TX] = endpoint_info->local_min_tx;

    for (index = 0; index < NOF_BASIC_INT_FIELDS; index++)
    {
        rv = algo_bfd_db.bfd_oamp_req_interval.allocate_single
            (unit, NO_FLAGS, &req_ints[index], NULL, &req_int_id, &write_hw);
        if (rv == _SHR_E_RESOURCE)
        {
            /** Resource error means no match - display error message */
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "Error: No free required interval entries available.\n");
        }
        else
        {
            /** All other errors handled generically */
            SHR_IF_ERR_EXIT(rv);
        }

        mep_hw_write_data->req_int_write[index] = write_hw;

        /** This will definitely be written to the MEP DB   */
        mep_hw_write_data->req_int_write_index[index] = req_int_id;

        if (write_hw == TRUE)
        {
            /** Save data for writing to HW table at the end   */
            mep_hw_write_data->req_int_write_value[index] = req_ints[index];
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function applies the local discriminator
 *        configured for a new accelerated BFD endpoint to the
 *        discriminator range start mechanism.  The API will
 *        update the template manager with the configured
 *        discriminator range start and update the hw_data
 *        values for later HW write.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - see definition of struct for
 *        description of fields
 * \param [out] mep_hw_write_data - Pointer to struct where data
 *        for writing to HW tables is placed.  This data is
 *        relevant to fields in the OAMP MEP DB.
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
dnx_bfd_discriminator_start_update(
    int unit,
    const bcm_bfd_endpoint_info_t * endpoint_info,
    bfd_temp_oamp_mep_db_data_t * mep_hw_write_data)
{
    shr_error_e rv;
    uint32 range_start, new_range_start;
    int dummy;
    uint8 write_hw, discr_start_bit_shift;
    SHR_FUNC_INIT_VARS(unit);

    /** The discriminator is range start offset + BFD_ID.  The
     *  offset is the most significant bits, and the BFD_ID is the
     *  least significant bits.  Therefore if n is the number of
     *  bits needed for the BFD_ID (16 for JR2) then the range start
     *  offset should be calculated as local_discr & ((1 << 32) -
     *  (1<<n).  n least significant bits are masked to 0.
     */
    discr_start_bit_shift = dnx_data_bfd.general.nof_bits_bfd_endpoints_get(unit);

    /**
     *  The discriminator range start "resource" is treated as
     *  a "template"
     */
    new_range_start = (endpoint_info->local_discr & UTILEX_ZERO_BITS_MASK(discr_start_bit_shift - 1, 0));

    /**
     * BFD MEP type in discriminator mode:
     * In this mode, discriminator MSB can very from one endpoint to another,
     * So in the range, the bit is set to 0, and for every endpoint with
     * MSB=1, the OAMP-PE program will set it.
     */
    UTILEX_SET_BIT(new_range_start, 0, 31);

    rv = algo_bfd_db.bfd_oamp_my_discr_range_start.allocate_single
        (unit, NO_FLAGS, &new_range_start, NULL, &dummy, &write_hw);

    if (rv == _SHR_E_RESOURCE)
    {
        /** Resource error means no match - display error message */
        SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_my_discr_range_start.profile_data_get
                        (unit, DISCR_PROF, &dummy, &range_start));
        SHR_ERR_EXIT(_SHR_E_RESOURCE,
                     "Error: the BFD local discriminator range start has been set.\nThe most "
                     "significant 16 bits of the local discriminator must be 0x%04X.\nThe local discriminator "
                     "provided was 0x%08X.", range_start >> discr_start_bit_shift, endpoint_info->local_discr);
    }
    else
    {
        /** All other errors handled generically */
        SHR_IF_ERR_EXIT(rv);
    }

    mep_hw_write_data->discr_range_start_write = write_hw;

    if (write_hw == TRUE)
    {
        /** Save data for writing to HW table at the end   */
        mep_hw_write_data->discr_range_start = new_range_start;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function prepares to add a BFD ToS/TTL entry to
 *        the TOS_TTL hardware table, or if that entry already
 *        exists, increases the relevant counter.
 *        Only Applies to multi-hop
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - see definition of struct for
 *        description of fields
 * \param [out] mep_hw_write_data - Pointer to struct where data
 *        for writing to HW tables is placed.  This data is
 *        relevant to fields in the OAMP MEP DB.
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
dnx_bfd_tos_ttl_profile_sw_update(
    int unit,
    const bcm_bfd_endpoint_info_t * endpoint_info,
    bfd_temp_oamp_mep_db_data_t * mep_hw_write_data)
{
    shr_error_e rv;
    int tos_ttl_entry_id;
    dnx_bfd_tos_ttl_t tos_ttl_data;
    uint8 write_hw;
    SHR_FUNC_INIT_VARS(unit);

    /** Get values   */
    tos_ttl_data.tos = endpoint_info->ip_tos;
    tos_ttl_data.ttl = endpoint_info->ip_ttl;

    rv = algo_bfd_db.bfd_oamp_tos_ttl.allocate_single
        (unit, NO_FLAGS, &tos_ttl_data, NULL, &tos_ttl_entry_id, &write_hw);

    if (rv == _SHR_E_RESOURCE)
    {
        /** Resource error means no match - display error message */
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Error: No free Type of Service/Time To Live entries available.\n");
    }
    else
    {
        /** All other errors handled generically */
        SHR_IF_ERR_EXIT(rv);
    }

    mep_hw_write_data->tos_ttl_write = write_hw;
    mep_hw_write_data->tos_ttl_entry_id = tos_ttl_entry_id;

    if (write_hw == TRUE)
    {
        /** Save data for writing to HW table at the end   */
        mep_hw_write_data->tos = tos_ttl_data.tos;
        mep_hw_write_data->ttl = tos_ttl_data.ttl;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function prepares to add a BFD IPv4 source
 *        address entry to the TOS_TTL hardware table, or if
 *        that entry already exists, increases the relevant
 *        counter. This is achieved by using the template "BFD
 *        IPv4 Source Addr."
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - see definition of struct for
 *        description of fields
 * \param [out] mep_hw_write_data - Pointer to struct where data
 *        for writing to HW tables is placed.  This data is
 *        relevant to fields in the OAMP MEP DB.
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
dnx_bfd_ipv4_src_addr_profile_sw_update(
    int unit,
    const bcm_bfd_endpoint_info_t * endpoint_info,
    bfd_temp_oamp_mep_db_data_t * mep_hw_write_data)
{
    shr_error_e rv;
    int ipv4_src_addr_index;
    uint8 write_hw;
    SHR_FUNC_INIT_VARS(unit);

    rv = algo_bfd_db.bfd_oamp_ipv4_src_addr.allocate_single
        (unit, NO_FLAGS, (void *) &endpoint_info->src_ip_addr, NULL, &ipv4_src_addr_index, &write_hw);

    if (rv == _SHR_E_RESOURCE)
    {
        /** Resource error means no match - display error message */
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Error: No free IPv4 Source Address entries available.\n");
    }
    else
    {
        /** All other errors handled generically */
        SHR_IF_ERR_EXIT(rv);
    }

    mep_hw_write_data->ipv4_src_addr_write = write_hw;
    mep_hw_write_data->ipv4_src_addr_index = ipv4_src_addr_index;

    if (write_hw == TRUE)
    {
        /** Save data for writing to HW table at the end   */
        mep_hw_write_data->ipv4_src_addr = endpoint_info->src_ip_addr;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function prepares to set TC and DP values to
 *        the OAMP_PR2_FW_DTC and OAMP_PR2_FWDDP hardware
 *        registers respectively, or if those values already
 *        exist, increases the relevant counter. This is
 *        achieved by using the template "OAM TC/DP."
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - see definition of struct for
 *        description of fields
 * \param [out] mep_hw_write_data - Pointer to struct where data
 *        for writing to HW tables is placed.  This data is
 *        relevant to fields in the OAMP MEP DB.
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
dnx_bfd_itmh_priority_profile_sw_update(
    int unit,
    const bcm_bfd_endpoint_info_t * endpoint_info,
    bfd_temp_oamp_mep_db_data_t * mep_hw_write_data)
{
    shr_error_e rv;
    int itmh_priority_index;
    dnx_oam_itmh_priority_t itmh_priority_data;
    uint8 write_hw;
    SHR_FUNC_INIT_VARS(unit);

    itmh_priority_data.tc =
        UTILEX_GET_BITS_RANGE(endpoint_info->int_pri, TC_IN_API_OFFSET + TC_IN_API_NOF_BITS - 1, TC_IN_API_OFFSET);
    itmh_priority_data.dp = UTILEX_GET_BITS_RANGE(endpoint_info->int_pri, TC_IN_API_OFFSET - 1, 0);

    rv = algo_oam_db.oam_itmh_priority_profile.allocate_single
        (unit, NO_FLAGS, &itmh_priority_data, NULL, &itmh_priority_index, &write_hw);

    if (rv == _SHR_E_RESOURCE)
    {
        /** Resource error means no match - display error message */
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Error: No free TC/DP profile entries available.\n");
    }
    else
    {
        /** All other errors handled generically */
        SHR_IF_ERR_EXIT(rv);
    }

    mep_hw_write_data->itmh_priority_write = write_hw;
    mep_hw_write_data->itmh_priority_index = itmh_priority_index;

    if (write_hw == TRUE)
    {
        /** Save data for writing to HW table at the end   */
        mep_hw_write_data->tc = itmh_priority_data.tc;
        mep_hw_write_data->dp = itmh_priority_data.dp;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function prepares to add an OAMP MPLS/PWE TTL/EXP
 *        profile entry to the MPLS/PWE profile hardware table,
 *        or if that entry already exists, increases the
 *        relevant counter. This is achieved by using the
 *        template "OAM MPLS/PWE TTL+EXP profile."
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - see definition of struct for
 *        description of fields
 * \param [out] mep_hw_write_data - Pointer to struct where data
 *        for writing to HW tables is placed.  This data is
 *        relevant to fields in the OAMP MEP DB.
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
dnx_bfd_mpls_pwe_ttl_exp_profile_sw_update(
    int unit,
    const bcm_bfd_endpoint_info_t * endpoint_info,
    bfd_temp_oamp_mep_db_data_t * mep_hw_write_data)
{
    shr_error_e rv;
    dnx_oam_ttl_exp_profile_t ttl_exp_data;
    int ttl_exp_index;
    uint8 write_hw;
    SHR_FUNC_INIT_VARS(unit);

    ttl_exp_data.ttl = endpoint_info->egress_label.ttl;
    ttl_exp_data.exp = endpoint_info->egress_label.exp;

    rv = algo_oam_db.oam_mpls_pwe_exp_ttl_profile.allocate_single
        (unit, NO_FLAGS, &ttl_exp_data, NULL, &ttl_exp_index, &write_hw);

    if (rv == _SHR_E_RESOURCE)
    {
        /** Resource error means no match - display error message */
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Error: No free MPLS/PWE TTL/EXP profile entries available.\n");
    }
    else
    {
        /** All other errors handled generically */
        SHR_IF_ERR_EXIT(rv);
    }

    mep_hw_write_data->ttl_exp_profile_write = write_hw;
    mep_hw_write_data->ttl_exp_profile_index = ttl_exp_index;

    if (write_hw == TRUE)
    {
        /** Save data for writing to HW table at the end   */
        mep_hw_write_data->mpls_pwe_ttl = ttl_exp_data.ttl;
        mep_hw_write_data->exp = ttl_exp_data.exp;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function prepares to add an OAMP punt profile
 *        entry to the punt profile hardware table, or if that
 *        entry already exists, increases the relevant counter.
 *        This is achieved by using the template "OAM punt
 *        profile."
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - see definition of struct for
 *        description of fields
 * \param [out] rmep_hw_write_data - Pointer to struct where data
 *        for writing to HW tables is placed.  This data is
 *        relevant to fields in the OAMP RMEP DB.
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
dnx_bfd_punt_profile_sw_update(
    int unit,
    const bcm_bfd_endpoint_info_t * endpoint_info,
    bfd_temp_oamp_rmep_db_data_t * rmep_hw_write_data)
{
    shr_error_e rv;
    dnx_oam_oamp_punt_event_profile_t punt_data;
    int punt_profile_index;
    uint8 write_hw;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Since 'punt_data' is eventually used as key (MULTI_SET_KEY) and
     * since there are 'alignment holes' in 'dnx_oam_oamp_punt_event_profile_t',
     * it must be, first, be fully initialized (each single byte).
     */
    sal_memset(&punt_data, 0, sizeof(punt_data));
    punt_data.punt_rate = (endpoint_info->sampling_ratio > 0) ? endpoint_info->sampling_ratio - 1 : 0;
    punt_data.punt_enable = (endpoint_info->sampling_ratio > 0) ? 1 : 0;
    punt_data.rx_state_update_en = (endpoint_info->flags & BCM_BFD_ENDPOINT_REMOTE_UPDATE_STATE_DISABLE) ? 0 :
        (endpoint_info->flags & BCM_BFD_ENDPOINT_RX_REMOTE_EVENT_DISABLE) ? 1 :
        (endpoint_info->flags & BCM_BFD_ENDPOINT_RX_RDI_AUTO_UPDATE) ? 3 : 2;
    punt_data.profile_scan_state_update_en = (endpoint_info->flags & BCM_BFD_ENDPOINT_REMOTE_UPDATE_STATE_DISABLE) ? 0 :
        (endpoint_info->flags & BCM_BFD_ENDPOINT_REMOTE_EVENT_DISABLE) ? 1 :
        (endpoint_info->flags & BCM_BFD_ENDPOINT_RDI_AUTO_UPDATE) ? 3 : 2;
    punt_data.mep_rdi_update_loc_en = (endpoint_info->flags & BCM_BFD_ENDPOINT_RDI_ON_LOC);
    punt_data.mep_rdi_update_loc_clear_en = (endpoint_info->flags & BCM_BFD_ENDPOINT_RDI_CLEAR_ON_LOC_CLEAR);
    punt_data.mep_rdi_update_rx_en = FALSE;

    rv = algo_oam_db.oamp_punt_event_prof_template.allocate_single
        (unit, NO_FLAGS, &punt_data, NULL, &punt_profile_index, &write_hw);

    if (rv == _SHR_E_RESOURCE)
    {
        /** Resource error means no match - display error message */
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Error: No free punt profile entries available.\n");
    }
    else
    {
        /** All other errors handled generically */
        SHR_IF_ERR_EXIT(rv);
    }

    rmep_hw_write_data->punt_profile_write = write_hw;
    rmep_hw_write_data->punt_profile_index = punt_profile_index;

    if (write_hw == TRUE)
    {
        /** Save data for writing to HW table at the end   */
        sal_memcpy(&rmep_hw_write_data->punt_profile, &punt_data, sizeof(dnx_oam_oamp_punt_event_profile_t));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function prepares to add an OAMP system port profile
 *        entry to the system port profile hardware table, or if that
 *        entry already exists, increases the relevant counter.
 *        This is achieved by using the template "OAM punt
 *        profile."
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - see definition of struct for
 *        description of fields
 * \param [out] mep_hw_write_data - Pointer to struct where data
 *        for writing to HW tables is placed.  This data is
 *        relevant to fields in the OAMP MEP DB.
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
dnx_bfd_system_port_profile_sw_update(
    int unit,
    const bcm_bfd_endpoint_info_t * endpoint_info,
    bfd_temp_oamp_mep_db_data_t * mep_hw_write_data)
{
    shr_error_e rv;
    dnx_algo_gpm_gport_phy_info_t *tx_gport_data = NULL;
    int system_port_profile_index;
    uint16 system_port;
    uint8 write_hw;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(tx_gport_data, sizeof(dnx_algo_gpm_gport_phy_info_t),
                       "Structure for reading data about tx_gport", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, endpoint_info->tx_gport,
                                                    DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, tx_gport_data));

    if (BFD_TYPE_NOT_PWE(endpoint_info->type))
    {
        system_port = tx_gport_data->sys_port;
        rv = algo_oamp_db.oamp_pp_port_2_sys_port.allocate_single
            (unit, NO_FLAGS, (uint32 *) &system_port, NULL, &system_port_profile_index, &write_hw);

        if (rv == _SHR_E_RESOURCE)
        {
            /** Resource error means no match - display error message */
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "Error: No free system port profile entries available.\n");
        }
        else
        {
            /** All other errors handled generically */
            SHR_IF_ERR_EXIT(rv);
        }

        mep_hw_write_data->system_port_profile_write = write_hw;
        mep_hw_write_data->pp_port_profile = system_port_profile_index;
    }
    else
    {
        write_hw = TRUE;
    }

    if (write_hw)
    {
        /** Save data for writing to HW table at the end   */
        mep_hw_write_data->system_port = tx_gport_data->sys_port;
    }

exit:
    SHR_FREE(tx_gport_data);
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function applies the UDP source port
 *          value of the new BFD endpoint to the source
 *          port templates.  If the relevant template
 *          (UDP or MPLS) is not initialized yet, the
 *          provided value will be written to it.  If
 *          it is initialized, the provided value will
 *          be compared to it.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - see definition of struct for
 *        description of fields
 * \param [out] mep_hw_write_data - Pointer to struct where data
 *        for writing to HW tables is placed.  This data is
 *        relevant to fields in the OAMP MEP DB.
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
dnx_bfd_udp_src_port_profile_sw_update(
    int unit,
    const bcm_bfd_endpoint_info_t * endpoint_info,
    bfd_temp_oamp_mep_db_data_t * mep_hw_write_data)
{
    shr_error_e rv;
    int dummy;
    uint16 actual_port, param_port;
    uint8 write_hw;
    SHR_FUNC_INIT_VARS(unit);

    /**
     *  The UDP source ports are treated as templates.  Depending on the type,
     *  the value will either be used to initialize the value in the only
     *  profile, or will be compared to that value
     */
    param_port = endpoint_info->udp_src_port;

    if (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR))
    {
        /** Seamless BFD */
        rv = algo_bfd_db.bfd_oamp_seamless_udp_sport.allocate_single(unit, NO_FLAGS, &param_port, NULL, &dummy,
                                                                     &write_hw);
    }
    else
    {
        if (endpoint_info->type != bcmBFDTunnelTypeUdp)
        {
            rv = algo_bfd_db.bfd_oamp_mpls_sport.allocate_single(unit, NO_FLAGS, &param_port, NULL, &dummy, &write_hw);
        }
        else
        {
            rv = algo_bfd_db.bfd_oamp_udp_sport.allocate_single(unit, NO_FLAGS, &param_port, NULL, &dummy, &write_hw);
        }
    }

    if (rv == _SHR_E_RESOURCE)
    {
        /** Resource error means no match - display error message */
        if (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR))
        {
            /** Seamless BFD */
            SHR_IF_ERR_EXIT(algo_bfd_db.
                            bfd_oamp_seamless_udp_sport.profile_data_get(unit, DISCR_PROF, &dummy, &actual_port));
        }
        else
        {
            if (endpoint_info->type != bcmBFDTunnelTypeUdp)
            {
                SHR_IF_ERR_EXIT(algo_bfd_db.
                                bfd_oamp_mpls_sport.profile_data_get(unit, DISCR_PROF, &dummy, &actual_port));
            }
            else
            {
                SHR_IF_ERR_EXIT(algo_bfd_db.
                                bfd_oamp_udp_sport.profile_data_get(unit, DISCR_PROF, &dummy, &actual_port));
            }
        }

        SHR_ERR_EXIT(_SHR_E_RESOURCE,
                     "Error: the relevant source port has been set.\nThe source "
                     "port value must be %d.\nThe source port provided was %d.", actual_port, param_port);
    }
    else
    {
        /** All other errors handled generically */
        SHR_IF_ERR_EXIT(rv);
    }

    mep_hw_write_data->upd_src_port_write = write_hw;

    if (write_hw == TRUE)
    {
        /** Save data for writing to HW table at the end   */
        mep_hw_write_data->src_port = param_port;

        if (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR))
        {
            mep_hw_write_data->upd_src_port_write = WRITE_SBFD;
        }
        else
        {
            if (endpoint_info->type != bcmBFDTunnelTypeUdp)
            {
                mep_hw_write_data->upd_src_port_write = WRITE_MPLS;
            }
            else
            {
                mep_hw_write_data->upd_src_port_write = WRITE_UDP;
            }
        }
    }
    else
    {
        mep_hw_write_data->upd_src_port_write = WRITE_NONE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - For BFD, MEP profiles are needed only for "CCM
 *          count" (or BFD count, in this case.)  To disable
 *          Tx transmissions, count is set to "1" while period
 *          is set to "0."  For other cases, a calculation
 *          function shared with OAM is used.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - see definition of struct for
 *        description of fields
 * \param [out] mep_hw_write_data - Pointer to struct where data
 *        for writing to HW tables is placed.  This data is
 *        relevant to fields in the OAMP MEP DB.
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
dnx_bfd_mep_profile_sw_update(
    int unit,
    const bcm_bfd_endpoint_info_t * endpoint_info,
    bfd_temp_oamp_mep_db_data_t * mep_hw_write_data)
{
    dnx_oam_mep_profile_t cluster_mep_profile_data;
    dnx_bfd_tx_period_t tx_rate_data;
    uint32 entry_handle_id, full_entry_threshold, cluster_mep_profile, cluster_mep_profile_valid;
    int dummy, loop_index, is_ipv6, is_odd_ep;
    dbal_fields_e mep_profile_field, mep_profile_valid_field;
    uint8 dummy2;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    full_entry_threshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);
    SHR_IF_ERR_EXIT(dnx_oam_oamp_scan_count_calc(unit, endpoint_info->id, 0,
                                                 (endpoint_info->id < full_entry_threshold),
                                                 DBAL_ENUM_FVAL_CCM_INTERVAL_3MS,
                                                 /*
                                                  * don't care for BFD
                                                  */
                                                 0,
                                                 OAMP_MEP_TX_OPCODE_CCM_BFD, &(mep_hw_write_data->mep_profile_data)));

    if (UTILEX_GET_BIT(endpoint_info->local_discr, 31))
    {
        /**
         * Since the BFD MEP type in discriminator feature is active, discriminators
         * with MSB=1 will be punted, unless we disable this verification
         */
        mep_hw_write_data->mep_profile_data.your_disc_check_dis = TRUE;
    }

    /** Which MEP profile do we use? */
    dnx_bfd_tx_period_cluster_get_mep_profile_field(unit, endpoint_info->id,
                                                    _SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_IPV6),
                                                    &mep_profile_field, &mep_profile_valid_field);

    /*
     * in case of BFD over IPv6 we check the IPv6 source address using the CRC check mechanism hence, we want to skip
     * the IPv6 source address check
     */
    if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_IPV6))
    {
        mep_hw_write_data->mep_profile_data.src_ip_check_dis = TRUE;
    }

    /**
     * For seamless BFD initiator endpoints, the UDP dest port 7784,
     * which becomes the UDP source port when the response packet
     * is received from the reflector.  There is no resource to use
     * to check this value, so we disable the UDP src port check.
     */
    if (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR))
    {
        mep_hw_write_data->mep_profile_data.udp_src_port_check_dis = TRUE;
    }

    /** OAMP Statistics*/
    if ((_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_BFD_ENDPOINT_FLAGS2_TX_STATISTICS))
        || (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_BFD_ENDPOINT_FLAGS2_RX_STATISTICS)))
    {
        dnx_oam_oamp_statistics_enable(unit, endpoint_info->flags2, &mep_hw_write_data->mep_profile_data);
    }

    /** Tx period cluster */
    SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_tx_rate.profile_data_get(unit, mep_hw_write_data->tx_rate_write_index, &dummy,
                                                                  &tx_rate_data));
    mep_hw_write_data->mep_profile_data.cluster_id = tx_rate_data.cluster_profile;

    if (endpoint_info->bfd_period == 0)
    {
        /**
         *  If the user sets bfd_period==0, the endpoint should not transmit packets.
         *  However, if BFD_TX_RATE_PROFILE points to a profile that has a 0 Tx interval,
         *  Packets from the endpoint are sent at the maximum rate.  To overcome this, scan
         *  profile is set to 1, and since count==1 never happens at maximum rate, packets
         *  will not be transmitted.  See also dnx_bfd_tx_rate_profile_sw_update.
         */
        if (tx_rate_data.cluster_profile != 0xFF)
        {
            /**
             * For clusters, force endpoints with potential for ccm_count=0
             * to have a different profile than ccm_count=1.  When not transmitting,
             * they'll have ccm_count=2 and ccm_count=3, respectively, thus being
             * associated with separate profiles.
             */
            mep_hw_write_data->mep_profile_data.ccm_count += 2;
        }
        else
        {
            /**
             * For non clustered MEPs there is no need to waste
             * two MEP profiles
             */
            mep_hw_write_data->mep_profile_data.ccm_count = 2;
        }
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BFD_TX_PERIOD_CLUSTER_INFO, &entry_handle_id));

    if (tx_rate_data.cluster_profile != 0xFF)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_PERIOD_CLUSTER_INDEX,
                                   tx_rate_data.cluster_profile);
    }

    if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_UPDATE) && (endpoint_info->bfd_period_cluster != 0))
    {
        /** Modify the all existing MEP profiles for the cluster */
        for (loop_index = 0; loop_index < 4; loop_index++)
        {
            is_ipv6 = UTILEX_GET_BIT(loop_index, 0);
            is_odd_ep = UTILEX_GET_BIT(loop_index, 1);

            mep_profile_field = mep_profile_fields[loop_index];
            mep_profile_valid_field = mep_profile_valid_fields[loop_index];
            dbal_value_field32_request(unit, entry_handle_id, mep_profile_field, INST_SINGLE, &cluster_mep_profile);
            dbal_value_field32_request(unit, entry_handle_id, mep_profile_valid_field, INST_SINGLE,
                                       &cluster_mep_profile_valid);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

            if (cluster_mep_profile_valid)
            {
                /** Profile exists - modify values accordingly */
                if (endpoint_info->bfd_period != 0)
                {
                    /** This bit is the CCM count value */
                    mep_hw_write_data->mep_profile_data.ccm_count = is_odd_ep;
                }
                else
                {
                    mep_hw_write_data->mep_profile_data.ccm_count = 2 + is_odd_ep;
                }
                /** This bit is for IPv6 */
                mep_hw_write_data->mep_profile_data.src_ip_check_dis = is_ipv6;
                SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.replace_data(unit, cluster_mep_profile,
                                                                         &mep_hw_write_data->mep_profile_data));
            }
        }

        /**
         * BFD_CLUSTER_TX_PERIOD_UPDATE means that an existing MEP profile will
         * be modified instead of connecting an EP to a different profile and
         * disconnecting it from its current profile.  This can only happen
         * when the EP is a part of a cluster.
         */
        mep_hw_write_data->mep_profile_write = BFD_CLUSTER_TX_PERIOD_UPDATE;
        mep_hw_write_data->cluster_id = tx_rate_data.cluster_profile;
    }
    else
    {
        if (tx_rate_data.cluster_profile != 0xFF)
        {
            SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.allocate_single
                            (unit, 0, &(mep_hw_write_data->mep_profile_data), NULL,
                             &(mep_hw_write_data->mep_profile), &(mep_hw_write_data->mep_profile_write)));

            if (mep_hw_write_data->mep_profile_write)
            {
                /** Set the MEP profile field in the SW state table */
                dbal_entry_value_field32_set(unit, entry_handle_id, mep_profile_field, INST_SINGLE,
                                             mep_hw_write_data->mep_profile);
                dbal_entry_value_field32_set(unit, entry_handle_id, mep_profile_valid_field, INST_SINGLE, TRUE);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
            }
            else
            {
                /** Get the cluster's MEP profile and compare fields */
                dbal_value_field32_request(unit, entry_handle_id, mep_profile_field, INST_SINGLE, &cluster_mep_profile);
                SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

                SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.profile_data_get(unit, cluster_mep_profile, &dummy,
                                                                             &cluster_mep_profile_data));

                if (UTILEX_GET_BIT(endpoint_info->local_discr, 31) != cluster_mep_profile_data.your_disc_check_dis)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Error, cannot have endpoints with discriminator MSB=1 and endpoints with MSB=0 in the same cluster\n");
                }

                if ((cluster_mep_profile_data.opcode_tx_statistics_enable !=
                     mep_hw_write_data->mep_profile_data.opcode_tx_statistics_enable) ||
                    (cluster_mep_profile_data.opcode_rx_statistics_enable !=
                     mep_hw_write_data->mep_profile_data.opcode_rx_statistics_enable) ||
                    (cluster_mep_profile_data.opcode_mask != mep_hw_write_data->mep_profile_data.opcode_mask) ||
                    (cluster_mep_profile_data.mep_id_shift != mep_hw_write_data->mep_profile_data.mep_id_shift))

                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Error, in a cluster, all endpoints must have the same statistics settings\n");
                }
            }
        }
        else
        {
            if (!_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_UPDATE) &&
                (mep_hw_write_data->mep_profile != 0xFF))
            {
                /**
                 * This BFD endpoint is being added to an existing MPLS-LM-DM endpoint,
                 * and it already has a MEP profile, so reduce the reference
                 */
                SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.free_single(unit, mep_hw_write_data->mep_profile, &dummy2));
            }
            SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.allocate_single
                            (unit, 0, &(mep_hw_write_data->mep_profile_data), NULL,
                             &(mep_hw_write_data->mep_profile), &(mep_hw_write_data->mep_profile_write)));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function applies the Type of Service value
 *        configured for a new accelerated BFD endpoint to the
 *        OAMP single-hop and micro BFD types of service mechanism.
 *        The API will update the template manager with the
 *        configured OAMP type of service and update the hw_data
 *        values for later HW write.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - see definition of struct for
 *        description of fields
 * \param [out] mep_hw_write_data - Pointer to struct where data
 *        for writing to HW tables is placed.  This data is
 *        relevant to fields in the OAMP MEP DB.
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
dnx_bfd_oamp_single_hop_tos_sw_update(
    int unit,
    const bcm_bfd_endpoint_info_t * endpoint_info,
    bfd_temp_oamp_mep_db_data_t * mep_hw_write_data)
{
    shr_error_e rv;
    int dummy;
    uint8 write_hw, oamp_1_hop_tos, new_oamp_1_hop_tos;
    SHR_FUNC_INIT_VARS(unit);

    /**
     *  For all accelerated single-hop BFD endpoints, TOS for
     *  generated Tx packets is read from one HW register
     */
    new_oamp_1_hop_tos = endpoint_info->ip_tos;

    /**
     *  The BFD OAMP single-hop TOS "resource" is treated as
     *  a "template"
     */
    rv = algo_bfd_db.bfd_oamp_single_hop_tos.allocate_single
        (unit, NO_FLAGS, &new_oamp_1_hop_tos, NULL, &dummy, &write_hw);

    if (rv == _SHR_E_RESOURCE)
    {
        /** Resource error means no match - display error message */
        SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_single_hop_tos.profile_data_get
                        (unit, DISCR_PROF, &dummy, &oamp_1_hop_tos));
        SHR_ERR_EXIT(_SHR_E_RESOURCE,
                     "Error: the BFD OAMP single-hop TOS has been set.\n "
                     "The value is 0x%04X.\nThe TOS " "provided was 0x%08X.", oamp_1_hop_tos, new_oamp_1_hop_tos);
    }
    else
    {
        /** All other errors handled generically */
        SHR_IF_ERR_EXIT(rv);
    }

    mep_hw_write_data->oamp_single_hop_tos_write = write_hw;

    if (write_hw == TRUE)
    {
        /** Save data for writing to HW table at the end   */
        mep_hw_write_data->oamp_single_hop_tos = new_oamp_1_hop_tos;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function applies the minimum echo interval value
 *        configured for a new accelerated BFD.  One value is used
 *        for all Tx packets generated by the OAMP.
 *        The API will update the template manager with the
 *        configured OAMP minimum echo interval and update the hw_data
 *        values for later HW write.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - see definition of struct for
 *        description of fields
 * \param [out] mep_hw_write_data - Pointer to struct where data
 *        for writing to HW tables is placed.  This data is
 *        relevant to fields in the OAMP MEP DB.
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
dnx_bfd_oamp_local_min_echo_sw_update(
    int unit,
    const bcm_bfd_endpoint_info_t * endpoint_info,
    bfd_temp_oamp_mep_db_data_t * mep_hw_write_data)
{
    shr_error_e rv;
    int dummy;
    uint8 write_hw;
    uint32 oamp_local_min_echo, new_oamp_local_min_echo;
    SHR_FUNC_INIT_VARS(unit);

    /**
     *  For all accelerated BFD endpoints, local min echo in
     *  generated Tx packets is read from one HW register
     */
    new_oamp_local_min_echo = endpoint_info->local_min_echo;

    /**
     *  The BFD OAMP local min echo "resource" is treated as
     *  a "template"
     */
    rv = algo_bfd_db.bfd_local_min_echo.allocate_single
        (unit, NO_FLAGS, &new_oamp_local_min_echo, NULL, &dummy, &write_hw);

    if (rv == _SHR_E_RESOURCE)
    {
        /** Resource error means no match - display error message */
        SHR_IF_ERR_EXIT(algo_bfd_db.bfd_local_min_echo.profile_data_get
                        (unit, DISCR_PROF, &dummy, &oamp_local_min_echo));
        SHR_ERR_EXIT(_SHR_E_RESOURCE,
                     "Error: the BFD OAMP local min echo has been set.\n "
                     "The value is 0x%04X.\nThe local min echo "
                     "provided was 0x%08X.", oamp_local_min_echo, new_oamp_local_min_echo);
    }
    else
    {
        /** All other errors handled generically */
        SHR_IF_ERR_EXIT(rv);
    }

    mep_hw_write_data->oamp_local_min_echo_write = write_hw;

    if (write_hw == TRUE)
    {
        /** Save data for writing to HW table at the end   */
        mep_hw_write_data->oamp_local_min_echo = new_oamp_local_min_echo;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in bfd_oamp.h for function description
 */
shr_error_e
dnx_bfd_endpoint_db_get_const_for_oamp_rmep_index_db(
    int unit,
    dbal_enum_value_field_oamp_mep_type_e mep_type,
    uint16 *rmep_index_const)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (mep_type)
    {
        case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_1HOP:
        case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP:
            *rmep_index_const = dnx_data_bfd.general.rmep_index_db_ipv4_const_get(unit);
            break;

        case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_MPLS:
            *rmep_index_const = dnx_data_bfd.general.rmep_index_db_mpls_const_get(unit);
            break;

        case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE:
            *rmep_index_const = dnx_data_bfd.general.rmep_index_db_pwe_const_get(unit);
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: illegal type for endpoint\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in bfd_oamp.h for function description
 */
shr_error_e
dnx_bfd_write_oamp_data_to_templates(
    int unit,
    const bcm_bfd_endpoint_info_t * endpoint_info,
    bfd_temp_oamp_mep_db_data_t * mep_hw_write_data,
    bfd_temp_oamp_rmep_db_data_t * rmep_hw_write_data,
    dnx_oam_oamp_punt_good_profile_temp_data_t * punt_good_hw_write_data)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Templates are grouped by relevant header */

    /** ITMH header relevant templates */
    SHR_IF_ERR_EXIT(dnx_bfd_itmh_priority_profile_sw_update(unit, endpoint_info, mep_hw_write_data));

    /** System port profile */
    if (!BCM_L3_ITF_TYPE_IS_FEC(endpoint_info->egress_if))
    {
        SHR_IF_ERR_EXIT(dnx_bfd_system_port_profile_sw_update(unit, endpoint_info, mep_hw_write_data));
    }

    /** End of ITMH header relevant templates */

    /** IPv4 header relevant templates */

    /** Connect to a new/existing Tx rate profile   */
    SHR_IF_ERR_EXIT(dnx_bfd_tx_rate_profile_sw_update(unit, endpoint_info, mep_hw_write_data, 0xFF));

    /**
     *  Connect to a new/existing IPv4 source address profile if
     *  applicable
     */
    if (BFD_TYPE_NOT_PWE(endpoint_info->type))
    {
        SHR_IF_ERR_EXIT(dnx_bfd_ipv4_src_addr_profile_sw_update(unit, endpoint_info, mep_hw_write_data));
    }

    /** Connect to a new/existing ToS/TTL profile if applicable   */
    if (((bcmBFDTunnelTypeUdp == endpoint_info->type)
         && (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_MULTIHOP) ||
             _SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ECHO))) || (bcmBFDTunnelTypeMpls == endpoint_info->type) ||
        _SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR))
    {
        SHR_IF_ERR_EXIT(dnx_bfd_tos_ttl_profile_sw_update(unit, endpoint_info, mep_hw_write_data));
    }

    /** Single-hop IPv4 and micro BFD - TOS is read from a general register */
    if (SINGLE_HOP_U_BFD_TOS_SUPPORTED(unit) &&
        (bcmBFDTunnelTypeUdp == endpoint_info->type) &&
        !_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_MULTIHOP) &&
        !_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR))
    {
        SHR_IF_ERR_EXIT(dnx_bfd_oamp_single_hop_tos_sw_update(unit, endpoint_info, mep_hw_write_data));
    }

    /** End of IPv4 header relevant templates */

    /** MPLS/PWE header relevant templates */

    if ((endpoint_info->type != bcmBFDTunnelTypeUdp)
        && (!_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_BFD_ENDPOINT_FLAGS2_SINGLE_HOP_WITH_RANDOM_DIP)))
    {
        /** Only one supported type is not MPLS/PWE   */
        SHR_IF_ERR_EXIT(dnx_bfd_mpls_pwe_ttl_exp_profile_sw_update(unit, endpoint_info, mep_hw_write_data));
    }

    /** End of MPLS/PWE header relevant templates */

    /** Template relevant to both UDP and MPLS headers */

    if (BFD_TYPE_NOT_PWE(endpoint_info->type))
    {
        SHR_IF_ERR_EXIT(dnx_bfd_udp_src_port_profile_sw_update(unit, endpoint_info, mep_hw_write_data));
    }

    /** End of template relevant to both UDP and MPLS headers */

    /** BFD PDU relevant templates */

    /** Connect to a new/existing required interval profile   */
    SHR_IF_ERR_EXIT(dnx_bfd_req_int_profile_sw_update(unit, endpoint_info, mep_hw_write_data));

    /** Adjust my discriminator range start if necessary */
    SHR_IF_ERR_EXIT(dnx_bfd_discriminator_start_update(unit, endpoint_info, mep_hw_write_data));

    /** Adjust local minimum echo interval if necessary */
    SHR_IF_ERR_EXIT(dnx_bfd_oamp_local_min_echo_sw_update(unit, endpoint_info, mep_hw_write_data));

    /** End of BFD PDU relevant templates */

    /** Punt profile - used for reception only */
    SHR_IF_ERR_EXIT(dnx_bfd_punt_profile_sw_update(unit, endpoint_info, rmep_hw_write_data));

    /** Punt good profile */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_punt_good_profile_alloc(unit, 0 /* Allocate */ , endpoint_info->punt_good_packet_period, 0     /* Not
                                                                                                                                 * used 
                                                                                                                                 */ , punt_good_hw_write_data));

    /**
     * Mep profile - Only relevant thing in MEP Profile (and MEP scan profile) for BFD is
     * phase count. Allocate profile based on that.
     * Passing 3.3ms for BFD always since the profile used for BFD endpoints will
     * only have CCM_CNT variable filled and hence the rate does not really matter
     */
    SHR_IF_ERR_EXIT(dnx_bfd_mep_profile_sw_update(unit, endpoint_info, mep_hw_write_data));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in bfd_oamp.h for function description
 */
shr_error_e
dnx_bfd_mep_type_parsing(
    int unit,
    bcm_bfd_tunnel_type_t bfd_api_mep_type,
    uint32 in_flags,
    uint32 flags2,
    uint32 label_ttl,
    dbal_enum_value_field_oamp_mep_type_e * bfd_dbal_mep_type,
    uint32 *out_flags,
    uint8 *ach_sel)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (bfd_api_mep_type)
    {
        case bcmBFDTunnelTypeUdp:
            if (_SHR_IS_FLAG_SET(in_flags, BCM_BFD_ENDPOINT_MULTIHOP) ||
                _SHR_IS_FLAG_SET(in_flags, BCM_BFD_ECHO) ||
                _SHR_IS_FLAG_SET(flags2, BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR))
            {
                *bfd_dbal_mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP;
            }
            else
            {
                *bfd_dbal_mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_1HOP;
            }
            break;

        case bcmBFDTunnelTypeMpls:
            *bfd_dbal_mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_MPLS;
            break;

        case bcmBFDTunnelTypePweControlWord:
            *bfd_dbal_mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE;
            *out_flags |= DNX_OAMP_BFD_MEP_ACH;
            *ach_sel = 0;
            break;

        case bcmBFDTunnelTypePweRouterAlert:
            *out_flags |= DNX_OAMP_BFD_MEP_ROUTER_ALERT;
            *bfd_dbal_mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE;
            *ach_sel = 0;
            if (_SHR_IS_FLAG_SET(in_flags, BCM_BFD_ENDPOINT_PWE_ACH))
            {
                *out_flags |= DNX_OAMP_BFD_MEP_ACH;
            }
            break;

        case bcmBFDTunnelTypePweTtl:
            if (label_ttl)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Error: for type bcmBFDTunnelTypePweTtl, TTL must be 1.\n");
            }
            if (_SHR_IS_FLAG_SET(in_flags, BCM_BFD_ENDPOINT_PWE_ACH))
            {
                *out_flags |= DNX_OAMP_BFD_MEP_ACH;
                *ach_sel = 0;
            }
            *bfd_dbal_mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE;
            break;

        case bcmBFDTunnelTypeMplsTpCc:
        case bcmBFDTunnelTypePweGal:
            *out_flags |= DNX_OAMP_BFD_MEP_ACH;
            *out_flags |= DNX_OAMP_BFD_MEP_GAL;
            *bfd_dbal_mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE;
            *ach_sel = 1;
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: tunnel type %d not supported\n", bfd_api_mep_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in bfd_oamp.h for function description
 */
shr_error_e
dnx_bfd_endpoint_db_build_param_for_oamp_mep_db(
    int unit,
    const bcm_bfd_endpoint_info_t * endpoint_info,
    const bfd_temp_oamp_mep_db_data_t * mep_hw_write_data,
    const uint32 bfd_sw_state_flags,
    dnx_bfd_oamp_endpoint_t * entry_values)
{
    uint32 full_entry_threshold;
    uint32 mep_pe_profile;
    dbal_enum_value_field_mep_pe_profile_sw_e profile_sw = DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_DEFAULT;
    SHR_FUNC_INIT_VARS(unit);

    /**
     *  Create entry in OAMP MEP DB, and also in MDB if 1/4 entry
     *  format
     */

    /** General, Rx only and non-structure fields */

    /** Above or below the full entry threshold? */
    full_entry_threshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);
    if (endpoint_info->id < full_entry_threshold)
    {
        entry_values->flags |= DNX_OAMP_BFD_MEP_Q_ENTRY;
    }

    SHR_IF_ERR_EXIT(dnx_bfd_mep_type_parsing
                    (unit, endpoint_info->type, endpoint_info->flags, endpoint_info->flags2, endpoint_info->ip_tos,
                     &entry_values->mep_type, &entry_values->flags, &entry_values->ach_sel));

    if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ECHO))
    {
        entry_values->your_disc = endpoint_info->local_discr;
    }
    else
    {
        entry_values->your_disc = endpoint_info->remote_discr;
    }

    entry_values->ip_subnet_len = ((endpoint_info->ip_subnet_length == 32) ? 31 : endpoint_info->ip_subnet_length);
    entry_values->tx_rate_profile = mep_hw_write_data->tx_rate_write_index;

    /** End of general, Rx only and non-structure fields */

    /** Fields relevant to ITMH header */

    entry_values->itmh_tc_dp_profile = mep_hw_write_data->itmh_priority_index;

    /** Is the provided interface a FEC ID? */
    if (BCM_L3_ITF_TYPE_IS_FEC(endpoint_info->egress_if))
    {
        entry_values->flags |= DNX_OAMP_BFD_DESTINATION_IS_FEC;
        entry_values->fec_id_or_glob_out_lif.fec_id = BCM_L3_ITF_VAL_GET(endpoint_info->egress_if);
    }
    else
    {
        /** Not a valid FEC ID.  Is it a global out-LIF */
        if (BCM_L3_ITF_TYPE_IS_LIF(endpoint_info->egress_if))
        {
            entry_values->fec_id_or_glob_out_lif.glob_out_lif = BCM_L3_ITF_VAL_GET(endpoint_info->egress_if);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: provided interface is not a valid FEC nor an out-LIF.\n");
        }
    }

    if (entry_values->mep_type != DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE)
    {
        entry_values->pp_port_profile = mep_hw_write_data->pp_port_profile;
    }
    else
    {
        entry_values->dest_sys_port_agr = mep_hw_write_data->system_port;
    }

    /** End of fields relevant to ITMH header */

    /** Fields relevant to IPv4/6 header */
    if (endpoint_info->type == bcmBFDTunnelTypeUdp || endpoint_info->type == bcmBFDTunnelTypeMpls)
    {
        if (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR))
        {
            profile_sw = DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SEAMLESS_BFD_IPV4;
            entry_values->src_ip_profile = mep_hw_write_data->ipv4_src_addr_index;
        }
        if (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_BFD_ENDPOINT_FLAGS2_SINGLE_HOP_WITH_RANDOM_DIP))
        {
            profile_sw = DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SINGLE_HOP_BFD_RANDOM_DIP;
            entry_values->src_ip_profile = mep_hw_write_data->ipv4_src_addr_index;
        }
        if (_SHR_IS_FLAG_SET(bfd_sw_state_flags, DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_BFD_VCCV_TYPE))
        {
            profile_sw = DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_MPLS_TP_VCCV;
        }
        if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_IPV6))
        {
            entry_values->flags |= DNX_OAMP_BFD_IPV6;
            if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_BFD_MEP_Q_ENTRY) &&
                entry_values->ipv6_extra_data_index != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Error: ipv6_extra_data_index must be 0 for short entries.\n");
            }
            entry_values->ipv6_extra_data_index = endpoint_info->ipv6_extra_data_index;
            if (endpoint_info->type == bcmBFDTunnelTypeUdp)
            {
                if (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR))
                {
                    profile_sw = DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SEAMLESS_BFD_IPV6;
                }
                else
                {
                    profile_sw = DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_IPV6;
                }
            }
            else if (endpoint_info->type == bcmBFDTunnelTypeMpls)
            {
                profile_sw = DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_IPV6_MPLS;
            }
            entry_values->src_ip_profile = mep_hw_write_data->ipv4_src_addr_index;
        }
        else if (endpoint_info->type == bcmBFDTunnelTypeUdp)
        {
            entry_values->ipv4_dip = endpoint_info->dst_ip_addr;
            entry_values->src_ip_profile = mep_hw_write_data->ipv4_src_addr_index;
        }

    }

    if ((entry_values->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP) ||
        (entry_values->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_MPLS))
    {
        entry_values->ip_ttl_tos_profile = mep_hw_write_data->tos_ttl_entry_id;
    }

    /** End of fields relevant to IPv4/6 header */

    /** Fields relevant to UDP header */

    if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_MICRO_BFD))
    {
        entry_values->flags |= DNX_OAMP_BFD_MEP_MICRO_BFD;
    }

    if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ECHO))
    {

        entry_values->flags |= DNX_OAMP_BFD_ECHO;

        if (!_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_IPV6))
        {
            profile_sw = DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_ECHO;
        }
    }

    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get(unit, profile_sw, &mep_pe_profile));
    entry_values->mep_pe_profile = (uint8) mep_pe_profile;

    /** End of fields relevant to UDP header */

    /** Fields relevant to MPLS tunnel header */

    if (endpoint_info->type != bcmBFDTunnelTypeUdp)
    {
        entry_values->label = endpoint_info->label;
        entry_values->push_profile = mep_hw_write_data->ttl_exp_profile_index;
    }

    /** End of fields relevant to MPLS tunnel header */

    /** Fields relevant to BFD packet */

    entry_values->sta = endpoint_info->local_state;
    entry_values->diag_profile = endpoint_info->local_diag;

    /** Calculate flags profile   */
    entry_values->flags_profile = BFD_FLAGS_VAL_TO_PROFILE(endpoint_info->local_flags);

    entry_values->detect_mult = endpoint_info->local_detect_mult;
    entry_values->min_rx_interval_profile = mep_hw_write_data->req_int_write_index[REQ_MIN_INT_RX];
    entry_values->min_tx_interval_profile = mep_hw_write_data->req_int_write_index[DES_MIN_INT_TX];
    entry_values->mep_profile = mep_hw_write_data->mep_profile;

    /** End of fields relevant to BFD packet */

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in bfd_oamp.h for function description
 */
void
dnx_bfd_endpoint_db_build_param_for_oamp_rmep_db(
    int unit,
    const bcm_bfd_endpoint_info_t * endpoint_info,
    const bfd_temp_oamp_rmep_db_data_t * rmep_hw_write_data,
    dnx_oam_oamp_punt_good_profile_temp_data_t * punt_good_hw_write_data,
    dnx_oam_oamp_rmep_db_entry_t * entry_values)
{
    entry_values->oam_id = rmep_hw_write_data->endpoint_id;
    if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_EXPLICIT_DETECTION_TIME))
    {
        /**
         * Explicit detection time flag is set: get detection time
         * value from "bfd_detection time" field.
         */
        entry_values->period = endpoint_info->bfd_detection_time;
    }
    else
    {
        /**
         * Explicit detection time flag is clear: get detection time
         * value by multiplying local_min_rx by remote_detect_mult.
         */
        entry_values->period = endpoint_info->local_min_rx * endpoint_info->remote_detect_mult;
    }
    entry_values->loc_clear_threshold = endpoint_info->loc_clear_threshold;
    entry_values->punt_profile = rmep_hw_write_data->punt_profile_index;
    entry_values->punt_good_profile = punt_good_hw_write_data->new_punt_good_profile;
    entry_values->punt_next_good_packet =
        ((endpoint_info->flags2 & BCM_BFD_ENDPOINT_FLAGS2_PUNT_NEXT_GOOD_PACKET) ? 1 : 0);

    entry_values->rmep_state.bfd_state.detect_multiplier = endpoint_info->remote_detect_mult;
    entry_values->rmep_state.bfd_state.diag_profile = endpoint_info->remote_diag;
    entry_values->rmep_state.bfd_state.flags_profile = BFD_FLAGS_VAL_TO_PROFILE(endpoint_info->remote_flags);
    entry_values->rmep_state.bfd_state.state = endpoint_info->remote_state;

    entry_values->last_ccm_lifetime_valid_on_create =
        _SHR_IS_FLAG_SET(endpoint_info->faults, BCM_BFD_ENDPOINT_REMOTE_LOC);
}

/*
 * See prototype definition in bfd_oamp.h for function description
 */
shr_error_e
dnx_bfd_endpoint_db_write_tx_rate_to_oamp_hw(
    int unit,
    uint32 entry_index,
    uint32 tx_period_value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_BFD_TX_RATES, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_TX_RATE_PROFILE, entry_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_RATE_VALUE, INST_SINGLE, tx_period_value);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in bfd_oamp.h for function description
 */
shr_error_e
dnx_bfd_endpoint_db_write_to_oamp_hw(
    int unit,
    const bfd_temp_oamp_mep_db_data_t * mep_hw_write_data,
    const bfd_temp_oamp_rmep_db_data_t * rmep_hw_write_data,
    dnx_oam_oamp_punt_good_profile_temp_data_t * punt_good_hw_write_data)
{
    dnx_oam_mep_profile_t mep_profile_data;
    int index, prog_num, *prog_ptr;
    uint32 entry_handle_id, cluster_mep_profile, cluster_mep_profile_valid;
    uint32 sbfd_ports;
    dbal_fields_e mep_profile_field, mep_profile_valid_field;
    int prog_index[] = { DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SEAMLESS_BFD_IPV4,
        DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SEAMLESS_BFD_IPV6,
        DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_IPV6_MPLS
    };
    int prog_index_jr1[] = { DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SEAMLESS_BFD_IPV4_JR1,
        DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SEAMLESS_BFD_IPV6_JR1,
        DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_IPV6_MPLS_JR1
    };
    uint32 hw_index = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /**
     * Done first because in the event of an update, this
     * modifies Tx packets from multiple endpoints
     */
    if (mep_hw_write_data->oamp_single_hop_tos_write)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_GENERAL_CONFIGURATION, &entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_BFD_IPV4_ONE_HOP_TOS, INST_SINGLE,
                                     mep_hw_write_data->oamp_single_hop_tos);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));
    }

    /**
     * Also modifies Tx packets from multiple endpoints
     */
    if (mep_hw_write_data->oamp_local_min_echo_write)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_BFD_CONFIGURATION, &entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_BFD_REQ_MIN_ECHO_RX_INTERVAL, INST_SINGLE,
                                     mep_hw_write_data->oamp_local_min_echo);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));
    }

    /**
     * For clusters, the order matters - when
     * stopping transmission, CCM count should be modified
     * before Tx rate.  In all other cases, Tx rate should
     * be modified first or the order doesn't matter.
     */
    if ((mep_hw_write_data->tx_rate_write) &&
        ((mep_hw_write_data->mep_profile_write != BFD_CLUSTER_TX_PERIOD_UPDATE)
         || (mep_hw_write_data->mep_profile_data.ccm_count < 2)))
    {
        /** Write data to BFD Tx rate table   */
        SHR_IF_ERR_EXIT(dnx_bfd_endpoint_db_write_tx_rate_to_oamp_hw(unit,
                                                                     mep_hw_write_data->tx_rate_write_index,
                                                                     mep_hw_write_data->tx_rate_write_value));
    }

    for (index = 0; index < NOF_BASIC_INT_FIELDS; index++)
    {
        if (mep_hw_write_data->req_int_write[index])
        {
            /** Write data to BFD Required Interval table   */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_BFD_REQ_INTERVALS, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_REQ_INT_INDEX,
                                       mep_hw_write_data->req_int_write_index[index]);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REQ_INT_VALUE, INST_SINGLE,
                                         mep_hw_write_data->req_int_write_value[index]);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));
        }
    }

    if (mep_hw_write_data->discr_range_start_write)
    {
        /** Write data to discriminator start range field   */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_BFD_CONFIGURATION, &entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_BFD_MY_DISCR_RANGE_START, INST_SINGLE,
                                     mep_hw_write_data->discr_range_start);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    if (mep_hw_write_data->tos_ttl_write)
    {
        /** Write data to BFD TOS/TTL table   */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_BFD_IPV4_TOS_TTL_SELECT, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IP_TTL_TOS_PROFILE,
                                   mep_hw_write_data->tos_ttl_entry_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4_TTL_VALUE, INST_SINGLE,
                                     mep_hw_write_data->ttl);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4_TOS_VALUE, INST_SINGLE,
                                     mep_hw_write_data->tos);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));
    }

    if (mep_hw_write_data->ipv4_src_addr_write)
    {
        /** Write data to BFD IPv4 Source Address table   */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_BFD_IPV4_SRC_ADDR_SELECT, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_SRC_IP_PROFILE,
                                   mep_hw_write_data->ipv4_src_addr_index);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4_SIP, INST_SINGLE,
                                     mep_hw_write_data->ipv4_src_addr);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));
    }

    if (mep_hw_write_data->itmh_priority_write)
    {
        /** Write data to TC and DP registers   */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_itmh_priority_profile_set
                        (unit, mep_hw_write_data->itmh_priority_index, mep_hw_write_data->tc, mep_hw_write_data->dp));
    }

    if (mep_hw_write_data->ttl_exp_profile_write)
    {
        /** Write data to MPLS/PWE TTL/EXP profile table   */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mpls_pwe_exp_ttl_profile_set
                        (unit, mep_hw_write_data->ttl_exp_profile_index,
                         mep_hw_write_data->mpls_pwe_ttl, mep_hw_write_data->exp));
    }

    if (rmep_hw_write_data->punt_profile_write)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_punt_profile_tbl_set
                        (unit, rmep_hw_write_data->punt_profile_index, &rmep_hw_write_data->punt_profile));
    }

    /*
     * Configure punt good profile data in HW
     */
    SHR_IF_ERR_EXIT(dnx_oam_remote_endpoint_punt_good_profile_hw_configure(unit, punt_good_hw_write_data));

    if (mep_hw_write_data->system_port_profile_write)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_LOCAL_PORT_2_SYSTEM_PORT, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE,
                                   mep_hw_write_data->pp_port_profile);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT, INST_SINGLE,
                                     mep_hw_write_data->system_port);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    if (mep_hw_write_data->upd_src_port_write != WRITE_NONE)
    {
        if (mep_hw_write_data->upd_src_port_write == WRITE_SBFD)
        {
            /**
             * The OAMP-PE programs need to write the UDP source port (user defined)
             * and destination port (constant.)  These two 16 bit values are combined
             * into one 32 bit value - the constant value field in the program properties
             * table entry.  These must be overwritten any time the SBFD UDP source port
             * is modified.
             */

            sbfd_ports = (mep_hw_write_data->src_port << 16) | SBFD_UDP_DEST_PORT;
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_PROGRAMS_PROPERTIES, &entry_handle_id));
            if (dnx_data_headers.system_headers.system_headers_mode_get(unit) ==
                dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit))
            {
                prog_ptr = prog_index_jr1;
                prog_num = sizeof(prog_index_jr1) / sizeof(int);
            }
            else
            {
                prog_ptr = prog_index;
                prog_num = sizeof(prog_index) / sizeof(int);
            }
            for (index = 0; index < prog_num; index++)
            {
                SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, prog_ptr[index], &hw_index));
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_PE_PROGRAM_INDEX, hw_index);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CONST_VALUE, INST_SINGLE, sbfd_ports);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_BFD_CONFIGURATION, &entry_handle_id));
            if (mep_hw_write_data->upd_src_port_write == WRITE_UDP)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_BFD_IPV4_UDP_SPORT, INST_SINGLE,
                                             mep_hw_write_data->src_port);
            }
            else if (mep_hw_write_data->upd_src_port_write == WRITE_MPLS)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_BFD_MPLS_UDP_SPORT, INST_SINGLE,
                                             mep_hw_write_data->src_port);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error: illegal value found in internal variable\n");
            }
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

    if (mep_hw_write_data->mep_profile_write == REGULAR_MEP_PROFILE_UPDATE)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_profile_set
                        (unit, mep_hw_write_data->mep_profile, TRUE, &(mep_hw_write_data->mep_profile_data)));
    }

    if (mep_hw_write_data->mep_profile_write == BFD_CLUSTER_TX_PERIOD_UPDATE)
    {
        /** Modify the all existing MEP profiles for the cluster */
        sal_memcpy(&mep_profile_data, &(mep_hw_write_data->mep_profile_data), sizeof(mep_profile_data));
        for (index = 0; index < 4; index++)
        {
            mep_profile_field = mep_profile_fields[index];
            mep_profile_valid_field = mep_profile_valid_fields[index];
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BFD_TX_PERIOD_CLUSTER_INFO, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_PERIOD_CLUSTER_INDEX,
                                       mep_hw_write_data->cluster_id);
            dbal_value_field32_request(unit, entry_handle_id, mep_profile_field, INST_SINGLE, &cluster_mep_profile);
            dbal_value_field32_request(unit, entry_handle_id, mep_profile_valid_field, INST_SINGLE,
                                       &cluster_mep_profile_valid);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

            if (cluster_mep_profile_valid)
            {
                /** Profile exists - modify values accordingly */
                if (mep_hw_write_data->mep_profile_data.ccm_count < 2)
                {
                    /** This bit is the CCM count value */
                    mep_profile_data.ccm_count = UTILEX_GET_BIT(index, 1);
                }
                else
                {
                    mep_profile_data.ccm_count = 2 + UTILEX_GET_BIT(index, 1);
                }
                /** This bit is for IPv6 */
                mep_profile_data.src_ip_check_dis = UTILEX_GET_BIT(index, 0);
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_profile_set
                                (unit, cluster_mep_profile, 0 /** Not rfc mep */ , &mep_profile_data));
            }
        }

        /**
         * If the entire cluster is stopping transmission,
         * Tx rate should be set now, that all MEP profiles
         * have their new CCM counts
         */
        if (mep_hw_write_data->tx_rate_write && (mep_hw_write_data->mep_profile_data.ccm_count >= 2))
        {
            /** Write data to BFD Tx rate table   */
            SHR_IF_ERR_EXIT(dnx_bfd_endpoint_db_write_tx_rate_to_oamp_hw(unit,
                                                                         mep_hw_write_data->tx_rate_write_index,
                                                                         mep_hw_write_data->tx_rate_write_value));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in bfd_oamp.h for function description
 */
void
dnx_bfd_endpoint_db_analyze_param_from_oamp_mep_db(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info,
    bfd_temp_oamp_mep_db_data_t * mep_sw_read_data,
    const dnx_bfd_oamp_endpoint_t * entry_values)
{
    int dummy;
    endpoint_info->remote_discr = entry_values->your_disc;

    endpoint_info->ip_subnet_length = ((entry_values->ip_subnet_len == 31) ? 32 : entry_values->ip_subnet_len);

    mep_sw_read_data->tx_rate_write_index = entry_values->tx_rate_profile;
    mep_sw_read_data->itmh_priority_index = entry_values->itmh_tc_dp_profile;

    if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_BFD_DESTINATION_IS_FEC))
    {
        BCM_L3_ITF_SET(endpoint_info->egress_if, BCM_L3_ITF_TYPE_FEC, entry_values->fec_id_or_glob_out_lif.fec_id);
    }
    else
    {
        BCM_L3_ITF_SET(endpoint_info->egress_if, _SHR_L3_ITF_TYPE_LIF,
                       entry_values->fec_id_or_glob_out_lif.glob_out_lif);
    }

    if (entry_values->mep_type != DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE)
    {
        mep_sw_read_data->pp_port_profile = entry_values->pp_port_profile;
    }
    else
    {
        mep_sw_read_data->system_port = entry_values->dest_sys_port_agr;
    }

    if ((endpoint_info->type == bcmBFDTunnelTypeUdp) ||
        ((endpoint_info->type == bcmBFDTunnelTypeMpls)
         && (endpoint_info->flags2 & BCM_BFD_ENDPOINT_FLAGS2_SINGLE_HOP_WITH_RANDOM_DIP)))
    {
        endpoint_info->dst_ip_addr = entry_values->ipv4_dip;
        mep_sw_read_data->ipv4_src_addr_index = entry_values->src_ip_profile;
    }

    if ((entry_values->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP) ||
        (entry_values->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_MPLS))
    {
        mep_sw_read_data->tos_ttl_entry_id = entry_values->ip_ttl_tos_profile;
    }

    if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_BFD_MEP_MICRO_BFD))
    {
        endpoint_info->flags |= BCM_BFD_ENDPOINT_MICRO_BFD;
    }

    if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_BFD_ECHO))
    {
        endpoint_info->flags |= BCM_BFD_ECHO;
    }

    if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_BFD_IPV6))
    {
        endpoint_info->flags |= BCM_BFD_ENDPOINT_IPV6;
        endpoint_info->ipv6_extra_data_index = entry_values->ipv6_extra_data_index;
    }

    if (entry_values->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_1HOP)
    {
        /*
         * For multi hop the ip-ttl is gotten through the tos-ttl profile
         */
        endpoint_info->ip_ttl = (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_BFD_1HOP_MPLS_TUNNEL_ENABLE)) ? 1 : 255;
    }

    if (endpoint_info->type != bcmBFDTunnelTypeUdp)
    {
        endpoint_info->label = entry_values->label;
        mep_sw_read_data->ttl_exp_profile_index = entry_values->push_profile;
    }
    else
    {
        endpoint_info->egress_label.exp = 0xFF;
    }

    if ((bcmBFDTunnelTypePweTtl == endpoint_info->type) && _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_BFD_MEP_ACH))
    {
        /** This flag is irrelevant otherwise */
        endpoint_info->flags |= BCM_BFD_ENDPOINT_PWE_ACH;
    }

    endpoint_info->local_state = entry_values->sta;
    endpoint_info->local_diag = entry_values->diag_profile;

    endpoint_info->local_flags = BFD_FLAGS_PROFILE_TO_VAL(entry_values->flags_profile);

    endpoint_info->local_detect_mult = entry_values->detect_mult;
    mep_sw_read_data->req_int_write_index[REQ_MIN_INT_RX] = entry_values->min_rx_interval_profile;
    mep_sw_read_data->req_int_write_index[DES_MIN_INT_TX] = entry_values->min_tx_interval_profile;

    endpoint_info->ipv6_extra_data_index = entry_values->ipv6_extra_data_index;
    /** Get MEP profile data */
    mep_sw_read_data->mep_profile = entry_values->mep_profile;
    algo_oam_db.oam_mep_profile.profile_data_get
        (unit, mep_sw_read_data->mep_profile, &dummy, &mep_sw_read_data->mep_profile_data);
}

/*
 * See prototype definition in bfd_oamp.h for function description
 */
void
dnx_bfd_endpoint_db_analyze_param_from_oamp_rmep_db(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info,
    bfd_temp_oamp_rmep_db_data_t * rmep_sw_read_data,
    dnx_oam_oamp_punt_good_profile_temp_data_t * punt_good_hw_write_data,
    const dnx_oam_oamp_rmep_db_entry_t * entry_values)
{
    endpoint_info->loc_clear_threshold = entry_values->loc_clear_threshold;
    rmep_sw_read_data->punt_profile_index = entry_values->punt_profile;
    punt_good_hw_write_data->old_punt_good_profile = entry_values->punt_good_profile;

    endpoint_info->remote_detect_mult = entry_values->rmep_state.bfd_state.detect_multiplier;
    endpoint_info->remote_diag = entry_values->rmep_state.bfd_state.diag_profile;
    endpoint_info->remote_flags = BFD_FLAGS_PROFILE_TO_VAL(entry_values->rmep_state.bfd_state.flags_profile);
    endpoint_info->remote_state = entry_values->rmep_state.bfd_state.state;

    endpoint_info->bfd_detection_time = entry_values->period;
    endpoint_info->faults |= entry_values->loc ? BCM_BFD_ENDPOINT_REMOTE_LOC : 0;
}

/*
 * See prototype definition in bfd_oamp.h for function description
 */
int
dnx_bfd_get_bfd_period(
    int unit,
    uint16 endpoint_id,
    uint16 tx_rate,
    uint32 ccm_count)
{
    uint32 full_entry_threshold;

    full_entry_threshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);

    if (tx_rate == 0)
    {
        /**
         * When this value is configured using bcm_bfd_endpoint_create,
         * it means that no BFD packets should be transmitted for this
         * endpoint, so CCM (BFD) count is set to 1.  However, if the
         * user writes this value directly to the register and doesn't
         * set CCM count, Tx packets are transmitted at maximum rate.
         */
        if ((ccm_count >= 2) || ((endpoint_id >= full_entry_threshold) && (ccm_count == 1)))
        {
            /** In this case, there will be no packets transmitted */
            return 0;
        }
    }

    /** If it's not the "no transmission" case, use this conversion */
    return BCM_OAMP_MEP_DB_SCAN_RATE_TO_SW_ENTRY_PERIOD(tx_rate);
}

/**
 * \brief - This function reads a BFD Tx Rate entry from the
 *        template "BFD Tx Rate"
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] endpoint_info - see definition of struct for
 *        description of fields
 * \param [in] mep_sw_read_data - Pointer to struct where
 *        profile ID is stored.
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
dnx_bfd_tx_rate_profile_sw_read(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info,
    const bfd_temp_oamp_mep_db_data_t * mep_sw_read_data)
{
    dnx_bfd_tx_period_t tx_rate_data;
    int dummy;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_tx_rate.profile_data_get
                    (unit, mep_sw_read_data->tx_rate_write_index, &dummy, &tx_rate_data));

    endpoint_info->bfd_period =
        dnx_bfd_get_bfd_period(unit, endpoint_info->id, tx_rate_data.tx_period,
                               mep_sw_read_data->mep_profile_data.ccm_count);

    endpoint_info->bfd_period_cluster = 0;
    if (tx_rate_data.cluster_profile != 0xFF)
    {
        SHR_IF_ERR_EXIT(algo_bfd_db.bfd_tx_period_cluster.profile_data_get
                        (unit, tx_rate_data.cluster_profile, &dummy, &endpoint_info->bfd_period_cluster));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads a BFD Req Int entry from
 *          the template "BFD Required Interval."
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] endpoint_info - see definition of struct for
 *        description of fields
 * \param [in] mep_sw_read_data - Pointer to struct where
 *        profile ID is stored.
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
dnx_bfd_req_int_profile_sw_read(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info,
    const bfd_temp_oamp_mep_db_data_t * mep_sw_read_data)
{
    uint32 req_ints[2];
    int dummy, index;
    SHR_FUNC_INIT_VARS(unit);

    for (index = 0; index < NOF_BASIC_INT_FIELDS; index++)
    {
        SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_req_interval.profile_data_get
                        (unit, mep_sw_read_data->req_int_write_index[index], &dummy, &req_ints[index]));
    }

    endpoint_info->local_min_rx = req_ints[REQ_MIN_INT_RX];
    endpoint_info->local_min_tx = req_ints[DES_MIN_INT_TX];

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function prepares reads a BFD ToS/TTL entry from
 *        the template "BFD TOS TTL."
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] endpoint_info - see definition of struct for
 *        description of fields
 * \param [in] mep_sw_read_data - Pointer to struct where
 *        profile ID is stored.
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
dnx_bfd_tos_ttl_profile_sw_read(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info,
    const bfd_temp_oamp_mep_db_data_t * mep_sw_read_data)
{
    int dummy;
    dnx_bfd_tos_ttl_t tos_ttl_data;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_tos_ttl.profile_data_get
                    (unit, mep_sw_read_data->tos_ttl_entry_id, &dummy, &tos_ttl_data));

    /** Get values   */
    endpoint_info->ip_tos = tos_ttl_data.tos;
    endpoint_info->ip_ttl = tos_ttl_data.ttl;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads a BFD IPv4 source address
 *        entry from the template "BFD IPv4 Source Addr."
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] endpoint_info - see definition of struct for
 *        description of fields
 * \param [in] mep_sw_read_data - Pointer to struct where
 *        profile ID is stored.
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
dnx_bfd_ipv4_src_addr_profile_sw_read(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info,
    const bfd_temp_oamp_mep_db_data_t * mep_sw_read_data)
{
    int dummy;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_ipv4_src_addr.profile_data_get
                    (unit, mep_sw_read_data->ipv4_src_addr_index, &dummy, &endpoint_info->src_ip_addr));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads TC and DP values from
 *        the template "OAM TC/DP."
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - see definition of struct for
 *        description of fields
 * \param [in] mep_sw_read_data - Pointer to struct where
 *        profile ID is stored.
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
dnx_bfd_itmh_priority_profile_sw_read(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info,
    const bfd_temp_oamp_mep_db_data_t * mep_sw_read_data)
{
    int dummy;
    dnx_oam_itmh_priority_t itmh_priority_data;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_oam_db.oam_itmh_priority_profile.profile_data_get
                    (unit, mep_sw_read_data->itmh_priority_index, &dummy, &itmh_priority_data));

    endpoint_info->int_pri = itmh_priority_data.dp | (itmh_priority_data.tc << TC_IN_API_OFFSET);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads an OAMP MPLS/PWE TTL/EXP profile
 *          entry from the template "OAM MPLS/PWE TTL+EXP profile."
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] endpoint_info - see definition of struct for
 *        description of fields
 * \param [in] mep_sw_read_data - Pointer to struct where
 *        profile ID is stored.
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
dnx_bfd_mpls_pwe_ttl_exp_profile_sw_read(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info,
    const bfd_temp_oamp_mep_db_data_t * mep_sw_read_data)
{
    dnx_oam_ttl_exp_profile_t ttl_exp_data;
    int dummy;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_oam_db.oam_mpls_pwe_exp_ttl_profile.profile_data_get
                    (unit, mep_sw_read_data->ttl_exp_profile_index, &dummy, &ttl_exp_data));

    endpoint_info->egress_label.ttl = ttl_exp_data.ttl;
    endpoint_info->egress_label.exp = ttl_exp_data.exp;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads an OAMP punt profile
 *        entry from the template "OAM punt profile."
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] endpoint_info - see definition of struct for
 *        description of fields
 * \param [in] rmep_sw_read_data - Pointer to struct where
 *        profile ID is stored.
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
dnx_bfd_punt_profile_sw_read(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info,
    const bfd_temp_oamp_rmep_db_data_t * rmep_sw_read_data)
{
    dnx_oam_oamp_punt_event_profile_t punt_data;
    int dummy;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_oam_db.oamp_punt_event_prof_template.profile_data_get
                    (unit, rmep_sw_read_data->punt_profile_index, &dummy, &punt_data));

    endpoint_info->sampling_ratio = (punt_data.punt_enable ? punt_data.punt_rate + 1 : 0);

    switch (punt_data.rx_state_update_en)
    {
        case 0:
            endpoint_info->flags |= BCM_BFD_ENDPOINT_REMOTE_UPDATE_STATE_DISABLE;
            break;

        case 1:
            endpoint_info->flags |= BCM_BFD_ENDPOINT_RX_REMOTE_EVENT_DISABLE;
            break;

        case 3:
            endpoint_info->flags |= BCM_BFD_ENDPOINT_RX_RDI_AUTO_UPDATE;
            break;

        default:
            break;
    }

    if (1 == punt_data.profile_scan_state_update_en)
    {
        endpoint_info->flags |= BCM_BFD_ENDPOINT_REMOTE_EVENT_DISABLE;
    }

    if (punt_data.mep_rdi_update_loc_en)
    {
        endpoint_info->flags |= BCM_BFD_ENDPOINT_RDI_ON_LOC;
    }

    if (punt_data.mep_rdi_update_loc_clear_en)
    {
        endpoint_info->flags |= BCM_BFD_ENDPOINT_RDI_CLEAR_ON_LOC_CLEAR;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads an OAMP system port profile entry
 *        from the  template "OAM punt profile."
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] endpoint_info - see definition of struct for
 *        description of fields
 * \param [in] mep_sw_read_data - Pointer to struct where
 *        profile ID is stored.
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
dnx_bfd_system_port_profile_sw_read(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info,
    const bfd_temp_oamp_mep_db_data_t * mep_sw_read_data)
{
    uint16 system_port = 0;
    int dummy;
    SHR_FUNC_INIT_VARS(unit);

    if (BFD_TYPE_NOT_PWE(endpoint_info->type))
    {
        SHR_IF_ERR_EXIT(algo_oamp_db.oamp_pp_port_2_sys_port.profile_data_get
                        (unit, mep_sw_read_data->pp_port_profile, &dummy, (uint32 *) &system_port));
    }
    else
    {
        system_port = mep_sw_read_data->system_port;
    }

    /** Get gport from system port */
    BCM_GPORT_SYSTEM_PORT_ID_SET(endpoint_info->tx_gport, system_port);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads the UDP source port
 *          value from the relevant source port template.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] endpoint_info - see definition of struct for
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
dnx_bfd_udp_src_port_profile_sw_read(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info)
{
    int dummy;
    uint16 udp_port;
    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR))
    {
        /** Seamless BFD */
        SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_seamless_udp_sport.profile_data_get(unit, DISCR_PROF, &dummy, &udp_port));
    }
    else
    {
        if (endpoint_info->type != bcmBFDTunnelTypeUdp)
        {
            SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_mpls_sport.profile_data_get(unit, DISCR_PROF, &dummy, &udp_port));
        }
        else
        {
            SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_udp_sport.profile_data_get(unit, DISCR_PROF, &dummy, &udp_port));
        }
    }

    endpoint_info->udp_src_port = udp_port;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads the OAMP BFD single-hop
 *          TOS value from the template
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] endpoint_info - see definition of struct for
 *        description of fields
 * \param [in] mep_sw_read_data - Pointer to struct where
 *        profile ID is stored.
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
dnx_bfd_oamp_single_hop_tos_sw_read(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info,
    const bfd_temp_oamp_mep_db_data_t * mep_sw_read_data)
{
    uint8 tos;
    int dummy;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_single_hop_tos.profile_data_get(unit, 0, &dummy, &tos));

    endpoint_info->ip_tos = tos;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads the OAMP BFD local
 *          minimum echo value from the template
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] endpoint_info - see definition of struct for
 *        description of fields
 * \param [in] mep_sw_read_data - Pointer to struct where
 *        profile ID is stored.
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
dnx_bfd_oamp_local_min_echo_sw_read(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info,
    const bfd_temp_oamp_mep_db_data_t * mep_sw_read_data)
{
    uint32 local_min_echo;
    int dummy;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_bfd_db.bfd_local_min_echo.profile_data_get(unit, 0, &dummy, &local_min_echo));

    endpoint_info->local_min_echo = local_min_echo;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in bfd_oamp.h for function description
 */
shr_error_e
dnx_bfd_read_oamp_data_from_templates(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info,
    const bfd_temp_oamp_mep_db_data_t * mep_sw_read_data,
    const bfd_temp_oamp_rmep_db_data_t * rmep_sw_read_data,
    const dnx_oam_oamp_punt_good_profile_temp_data_t * punt_good_hw_write_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_bfd_itmh_priority_profile_sw_read(unit, endpoint_info, mep_sw_read_data));

    if (!BCM_L3_ITF_TYPE_IS_FEC(endpoint_info->egress_if))
    {
        SHR_IF_ERR_EXIT(dnx_bfd_system_port_profile_sw_read(unit, endpoint_info, mep_sw_read_data));
    }
    else
    {
        endpoint_info->tx_gport = BCM_GPORT_INVALID;
    }

    SHR_IF_ERR_EXIT(dnx_bfd_tx_rate_profile_sw_read(unit, endpoint_info, mep_sw_read_data));

    if (BFD_TYPE_NOT_PWE(endpoint_info->type))
    {
        SHR_IF_ERR_EXIT(dnx_bfd_ipv4_src_addr_profile_sw_read(unit, endpoint_info, mep_sw_read_data));
    }

    if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_MULTIHOP) ||
        _SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR) ||
        (bcmBFDTunnelTypeMpls == endpoint_info->type))
    {
        SHR_IF_ERR_EXIT(dnx_bfd_tos_ttl_profile_sw_read(unit, endpoint_info, mep_sw_read_data));
    }

    if (endpoint_info->type != bcmBFDTunnelTypeUdp)
    {
        /** Only one supported type is not MPLS/PWE   */
        SHR_IF_ERR_EXIT(dnx_bfd_mpls_pwe_ttl_exp_profile_sw_read(unit, endpoint_info, mep_sw_read_data));
    }

    if (BFD_TYPE_NOT_PWE(endpoint_info->type))
    {
        SHR_IF_ERR_EXIT(dnx_bfd_udp_src_port_profile_sw_read(unit, endpoint_info));
    }

    SHR_IF_ERR_EXIT(dnx_bfd_req_int_profile_sw_read(unit, endpoint_info, mep_sw_read_data));

    SHR_IF_ERR_EXIT(dnx_bfd_punt_profile_sw_read(unit, endpoint_info, rmep_sw_read_data));

    SHR_IF_ERR_EXIT(dnx_oam_oamp_punt_good_profile_get
                    (unit, punt_good_hw_write_data->old_punt_good_profile, &endpoint_info->punt_good_packet_period));

    if (SINGLE_HOP_U_BFD_TOS_SUPPORTED(unit) &&
        (bcmBFDTunnelTypeUdp == endpoint_info->type) &&
        (!_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_MULTIHOP) &&
         !_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR)))
    {
        SHR_IF_ERR_EXIT(dnx_bfd_oamp_single_hop_tos_sw_read(unit, endpoint_info, mep_sw_read_data));
    }

    /** BFD MEP type in discriminator feature: MSB=1? */
    if (mep_sw_read_data->mep_profile_data.your_disc_check_dis)
    {
        UTILEX_SET_BIT(endpoint_info->local_discr, 1, 31);
    }

    SHR_IF_ERR_EXIT(dnx_bfd_oamp_local_min_echo_sw_read(unit, endpoint_info, mep_sw_read_data));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function decrements a reference to each
 *          of a bfd period clusters MEP prorilfes
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint - ID of deleted endpoint
 * \param [in] cluster_profile - ID of the bfd period
 *             cluster
 * \param [in] ipv6_flag - 1 for IPv6 EP, 0 otherwise
 * \param [out] oamp_hw_delete_data - Pointer to struct where
 *        HW deletion instructions are stored.
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
dnx_bfd_delete_oamp_cluster_mep_profiles(
    int unit,
    bcm_bfd_endpoint_t endpoint,
    uint8 cluster_profile,
    uint8 ipv6_flag,
    bfd_temp_oamp_db_delete_data_t * oamp_hw_delete_data)
{
    uint32 entry_handle_id, cluster_mep_profile;
    dbal_fields_e mep_profile_field, mep_profile_valid_field;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Which MEP profile did we use? */
    dnx_bfd_tx_period_cluster_get_mep_profile_field(unit, endpoint, ipv6_flag, &mep_profile_field,
                                                    &mep_profile_valid_field);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BFD_TX_PERIOD_CLUSTER_INFO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_PERIOD_CLUSTER_INDEX, cluster_profile);
    dbal_value_field32_request(unit, entry_handle_id, mep_profile_field, INST_SINGLE, &cluster_mep_profile);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.free_single
                    (unit, cluster_mep_profile, &oamp_hw_delete_data->mep_profile_delete));

    if (oamp_hw_delete_data->mep_profile_delete != 0)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, mep_profile_valid_field, INST_SINGLE, FALSE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in bfd_oamp.h for function description
 */
shr_error_e
dnx_bfd_delete_oamp_data_from_templates(
    int unit,
    bcm_bfd_endpoint_t endpoint,
    const dnx_bfd_oamp_endpoint_t * mep_entry_values,
    const dnx_oam_oamp_rmep_db_entry_t * rmep_entry_values,
    bfd_temp_oamp_db_delete_data_t * oamp_hw_delete_data,
    dnx_oam_oamp_punt_good_profile_temp_data_t * punt_good_hw_write_data,
    int is_single_hop_with_random_dip)
{
    dnx_bfd_tx_period_t tx_rate_data;
    uint32 entry_handle_id;
    int dummy2;
    uint8 dummy, delete_cluster = FALSE;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_oam_db.oam_itmh_priority_profile.free_single
                    (unit, mep_entry_values->itmh_tc_dp_profile, &oamp_hw_delete_data->itmh_priority_delete));

    if ((mep_entry_values->mep_type != DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE) &&
        !_SHR_IS_FLAG_SET(mep_entry_values->flags, DNX_OAMP_BFD_DESTINATION_IS_FEC))
    {
        SHR_IF_ERR_EXIT(algo_oamp_db.oamp_pp_port_2_sys_port.free_single
                        (unit, mep_entry_values->pp_port_profile, &oamp_hw_delete_data->system_port_profile_delete));
    }

    /** Is this endpoint part of a Tx period cluster? */
    SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_tx_rate.profile_data_get
                    (unit, mep_entry_values->tx_rate_profile, &dummy2, &tx_rate_data));

    if (tx_rate_data.cluster_profile != 0xFF)
    {
        /** Then subtract it from the cluster profile */
        SHR_IF_ERR_EXIT(algo_bfd_db.
                        bfd_tx_period_cluster.free_single(unit, tx_rate_data.cluster_profile, &delete_cluster));
    }

    SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_tx_rate.free_single
                    (unit, mep_entry_values->tx_rate_profile, &oamp_hw_delete_data->tx_rate_write_delete));

    if (mep_entry_values->mep_type != DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE)
    {

        SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_ipv4_src_addr.free_single
                        (unit, mep_entry_values->src_ip_profile, &oamp_hw_delete_data->ipv4_src_addr_delete));
    }

    if ((DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP == mep_entry_values->mep_type) ||
        (DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_MPLS == mep_entry_values->mep_type))
    {
        SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_tos_ttl.free_single
                        (unit, mep_entry_values->ip_ttl_tos_profile, &oamp_hw_delete_data->tos_ttl_delete));
    }

    if (((DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_MPLS == mep_entry_values->mep_type) ||
         (DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE == mep_entry_values->mep_type)) && (!is_single_hop_with_random_dip)
        && !oamp_hw_delete_data->mpls_lm_dm)
    {
        SHR_IF_ERR_EXIT(algo_oam_db.oam_mpls_pwe_exp_ttl_profile.free_single
                        (unit, mep_entry_values->push_profile, &oamp_hw_delete_data->ttl_exp_profile_delete));
    }

    if (mep_entry_values->mep_type != DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE)
    {
        if (oamp_hw_delete_data->seamless_bfd)
        {
            /** Seamless BFD */
            SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_seamless_udp_sport.free_single(unit, 0, &dummy));
        }
        else
        {
            if (mep_entry_values->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_MPLS)
            {
                SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_mpls_sport.free_single(unit, 0, &dummy));
            }
            else
            {
                SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_udp_sport.free_single(unit, 0, &dummy));
            }
        }
    }

    SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_req_interval.free_single
                    (unit, mep_entry_values->min_rx_interval_profile, &oamp_hw_delete_data->req_min_rx_int_delete));
    SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_req_interval.free_single(unit, mep_entry_values->min_tx_interval_profile,
                                                                  &oamp_hw_delete_data->req_min_tx_int_delete));

    SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_my_discr_range_start.free_single(unit, 0, &dummy));

    SHR_IF_ERR_EXIT(algo_oam_db.oamp_punt_event_prof_template.free_single
                    (unit, rmep_entry_values->punt_profile, &oamp_hw_delete_data->punt_profile_delete));

    /*
     * Free punt good profile from SW
     */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_punt_good_profile_free
                    (unit, rmep_entry_values->punt_good_profile, punt_good_hw_write_data));

    if (SINGLE_HOP_U_BFD_TOS_SUPPORTED(unit) &&
        (DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_1HOP == mep_entry_values->mep_type))
    {
        SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_single_hop_tos.free_single(unit, 0, &dummy));
    }

    if (tx_rate_data.cluster_profile != 0xFF)
    {
        SHR_IF_ERR_EXIT(dnx_bfd_delete_oamp_cluster_mep_profiles(unit, endpoint, tx_rate_data.cluster_profile,
                                                                 _SHR_IS_FLAG_SET(mep_entry_values->flags,
                                                                                  DNX_OAMP_BFD_IPV6),
                                                                 oamp_hw_delete_data));
    }
    else
    {
        if (oamp_hw_delete_data->mpls_lm_dm)
        {
            /**
             * For MPLS-LM-DM, the OAM EP still needs the MEP profile.
             * However, BFD packet transmission needs to stop, so
             * set the CCM count to maximum
             */

            /** Get current MEP profile Data */
            SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.profile_data_get(unit, (int) mep_entry_values->mep_profile,
                                                                         &dummy2,
                                                                         &
                                                                         (oamp_hw_delete_data->mpls_lm_dm_only_profile_data)));

            /** Set the CCM count to maximum */
            oamp_hw_delete_data->mpls_lm_dm_only_profile_data.ccm_count = DNX_OAM_OAMP_ETH1731_MEP_PROFILE_MAX_CCM_CNT;

            /** Allocate the new MEP profile */
            SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.allocate_single
                            (unit, 0, &(oamp_hw_delete_data->mpls_lm_dm_only_profile_data), NULL,
                             &(oamp_hw_delete_data->mpls_lm_dm_only_profile),
                             &(oamp_hw_delete_data->mpls_lm_dm_only_profile_write)));
        }
        SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.free_single
                        (unit, (int) mep_entry_values->mep_profile, &oamp_hw_delete_data->mep_profile_delete));
    }

    /** Now that the MEP profiles are deleted, we can delete this entry */
    if (delete_cluster)
    {
        /** Delete cluster's entry from the SW state table */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BFD_TX_PERIOD_CLUSTER_INFO, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_PERIOD_CLUSTER_INDEX,
                                   tx_rate_data.cluster_profile);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

    SHR_IF_ERR_EXIT(algo_bfd_db.bfd_local_min_echo.free_single(unit, 0, &dummy));

exit:
    DBAL_FUNC_FREE_VARS SHR_FUNC_EXIT;
}

/*
 * See prototype definition in bfd_oamp.h for function description
 */
shr_error_e
dnx_bfd_endpoint_db_delete_from_oamp_hw(
    int unit,
    const dnx_bfd_oamp_endpoint_t * mep_entry_values,
    const dnx_oam_oamp_rmep_db_entry_t * rmep_entry_values,
    bfd_temp_oamp_db_delete_data_t * oamp_hw_delete_data,
    dnx_oam_oamp_punt_good_profile_temp_data_t * punt_good_hw_write_data)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (oamp_hw_delete_data->itmh_priority_delete)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_ITMH_PRIORITY_PROFILE, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ITMH_TC_DP_PROFILE,
                                   mep_entry_values->itmh_tc_dp_profile);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));
    }

    if (oamp_hw_delete_data->system_port_profile_delete)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_LOCAL_PORT_2_SYSTEM_PORT, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE,
                                   mep_entry_values->pp_port_profile);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));
    }

    if (oamp_hw_delete_data->tx_rate_write_delete)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_BFD_TX_RATES, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_TX_RATE_PROFILE,
                                   mep_entry_values->tx_rate_profile);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_RATE_VALUE, INST_SINGLE, 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));
    }

    if (oamp_hw_delete_data->ipv4_src_addr_delete)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_BFD_IPV4_SRC_ADDR_SELECT, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_SRC_IP_PROFILE,
                                   mep_entry_values->src_ip_profile);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));
    }

    if (oamp_hw_delete_data->tos_ttl_delete)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_BFD_IPV4_TOS_TTL_SELECT, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IP_TTL_TOS_PROFILE,
                                   mep_entry_values->ip_ttl_tos_profile);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));
    }

    if (oamp_hw_delete_data->ttl_exp_profile_delete)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MPLS_PWE_EXP_TTL_PROFILE, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_PUSH_PROFILE, mep_entry_values->push_profile);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));
    }

    if (oamp_hw_delete_data->req_min_rx_int_delete)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_BFD_REQ_INTERVALS, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_REQ_INT_INDEX,
                                   mep_entry_values->min_rx_interval_profile);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));
    }

    if (oamp_hw_delete_data->req_min_tx_int_delete)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_BFD_REQ_INTERVALS, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_REQ_INT_INDEX,
                                   mep_entry_values->min_tx_interval_profile);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));
    }

    if (oamp_hw_delete_data->punt_profile_delete)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_PUNT_PROFILE_TBL, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PUNT_PROFILE, rmep_entry_values->punt_profile);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));
    }

    /*
     * Clear punt good profile data from HW
     */
    SHR_IF_ERR_EXIT(dnx_oam_remote_endpoint_punt_good_profile_hw_configure(unit, punt_good_hw_write_data));

    /** Create MEP profile for MPLS-LM-DM only EP */
    if (oamp_hw_delete_data->mpls_lm_dm_only_profile_write)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_profile_set(unit,
                                                     oamp_hw_delete_data->mpls_lm_dm_only_profile,
                                                     TRUE, &(oamp_hw_delete_data->mpls_lm_dm_only_profile_data)));

    }

    if (oamp_hw_delete_data->mep_profile_delete)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_PROFILE, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PROFILE_ID, mep_entry_values->mep_profile);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in bfd_oamp.h for function description
 */
shr_error_e
dnx_bfd_modify_oamp_data_in_templates(
    int unit,
    const bcm_bfd_endpoint_info_t * new_endpoint_info,
    const bcm_bfd_endpoint_info_t * existing_endpoint_info,
    const dnx_bfd_oamp_endpoint_t * mep_entry_values,
    const dnx_oam_oamp_rmep_db_entry_t * rmep_entry_values,
    bfd_temp_oamp_db_delete_data_t * oamp_hw_delete_data,
    bfd_temp_oamp_mep_db_data_t * mep_hw_write_data,
    bfd_temp_oamp_rmep_db_data_t * rmep_hw_write_data,
    dnx_oam_oamp_punt_good_profile_temp_data_t * punt_good_hw_write_data)
{
    int dummy;
    SHR_FUNC_INIT_VARS(unit);

    /** First, get the existing endpoint data */
    mep_hw_write_data->itmh_priority_index = mep_entry_values->itmh_tc_dp_profile;
    mep_hw_write_data->pp_port_profile = mep_entry_values->pp_port_profile;
    mep_hw_write_data->tx_rate_write_index = mep_entry_values->tx_rate_profile;
    mep_hw_write_data->ipv4_src_addr_index = mep_entry_values->src_ip_profile;
    mep_hw_write_data->tos_ttl_entry_id = mep_entry_values->ip_ttl_tos_profile;
    mep_hw_write_data->ttl_exp_profile_index = mep_entry_values->push_profile;
    mep_hw_write_data->req_int_write_index[REQ_MIN_INT_RX] = mep_entry_values->min_rx_interval_profile;
    mep_hw_write_data->req_int_write_index[DES_MIN_INT_TX] = mep_entry_values->min_tx_interval_profile;
    rmep_hw_write_data->punt_profile_index = rmep_entry_values->punt_profile;

    /*
     * Get the existing MEP profile data, just in case this is an MPLS-LM-DM endpoint
     */
    SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.profile_data_get(unit, (int) mep_entry_values->mep_profile,
                                                                 &dummy, &(mep_hw_write_data->mep_profile_data)));

    if (new_endpoint_info->int_pri != existing_endpoint_info->int_pri)
    {
        /** A different TC/DP priority profile is needed */
        SHR_IF_ERR_EXIT(dnx_bfd_itmh_priority_profile_sw_update(unit, new_endpoint_info, mep_hw_write_data));

        /**
         * Freeing after deleting makes sure new profiles
         * are not written over the freed profile - so all
         * parameters are changed at once in the Tx packet
         * when the OAMP_MEP_DB and OAMP_MEP_STATIC_DATA_DB
         * entries are updated - see more examples below
         */
        SHR_IF_ERR_EXIT(algo_oam_db.oam_itmh_priority_profile.free_single
                        (unit, mep_entry_values->itmh_tc_dp_profile, &oamp_hw_delete_data->itmh_priority_delete));
    }

    if (new_endpoint_info->tx_gport != existing_endpoint_info->tx_gport)
    {
        /** A different Tx port is needed */
        if (!BCM_L3_ITF_TYPE_IS_FEC(new_endpoint_info->egress_if))
        {
            SHR_IF_ERR_EXIT(dnx_bfd_system_port_profile_sw_update(unit, new_endpoint_info, mep_hw_write_data));
        }

        if (BFD_TYPE_NOT_PWE(existing_endpoint_info->type) &&
            (!BCM_L3_ITF_TYPE_IS_FEC(existing_endpoint_info->egress_if)))
        {
            SHR_IF_ERR_EXIT(algo_oamp_db.oamp_pp_port_2_sys_port.free_single
                            (unit, mep_entry_values->pp_port_profile,
                             &oamp_hw_delete_data->system_port_profile_delete));
        }
    }
    else
    {
        mep_hw_write_data->system_port = new_endpoint_info->tx_gport;
    }

    mep_hw_write_data->mep_profile = mep_entry_values->mep_profile;
    if ((new_endpoint_info->bfd_period != existing_endpoint_info->bfd_period) &&
        (BCM_OAMP_SW_ENTRY_INFO_PERIOD_TO_MEP_DB_SCAN_RATE(new_endpoint_info->bfd_period) !=
         BCM_OAMP_SW_ENTRY_INFO_PERIOD_TO_MEP_DB_SCAN_RATE(existing_endpoint_info->bfd_period)))
    {
        /** Create a new Tx profile, or modify the existing profile */
        SHR_IF_ERR_EXIT(dnx_bfd_tx_rate_profile_sw_update(unit, new_endpoint_info,
                                                          mep_hw_write_data, mep_entry_values->tx_rate_profile));

        if (mep_hw_write_data->tx_rate_write_index != mep_entry_values->tx_rate_profile)
        {
            /** If a different profile is used, subtract a reference from the profile previously used */
            SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_tx_rate.free_single(unit, mep_entry_values->tx_rate_profile,
                                                                     &oamp_hw_delete_data->tx_rate_write_delete));
        }
    }

    if ((existing_endpoint_info->bfd_period == 0) || (new_endpoint_info->bfd_period == 0)
        || (existing_endpoint_info->flags2 != new_endpoint_info->flags2))
    {
        /**
         * A different MEP profile is needed - or modify a MEP profile if endpoint is
         * in a Tx rate cluster
         */
        SHR_IF_ERR_EXIT(dnx_bfd_mep_profile_sw_update(unit, new_endpoint_info, mep_hw_write_data));

        if (new_endpoint_info->bfd_period_cluster == 0)
        {
            /** For clusters, the existing MEP profile is modified */
            SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.free_single
                            (unit, mep_entry_values->mep_profile, &oamp_hw_delete_data->mep_profile_delete));
        }
    }

    if (BFD_TYPE_NOT_PWE(new_endpoint_info->type) &&
        (new_endpoint_info->src_ip_addr != existing_endpoint_info->src_ip_addr))
    {
                /** A different source IP address profile is needed */
        SHR_IF_ERR_EXIT(dnx_bfd_ipv4_src_addr_profile_sw_update(unit, new_endpoint_info, mep_hw_write_data));

        SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_ipv4_src_addr.free_single(unit, mep_entry_values->src_ip_profile,
                                                                       &oamp_hw_delete_data->ipv4_src_addr_delete));
    }

    if ((((bcmBFDTunnelTypeUdp == new_endpoint_info->type)
          && (_SHR_IS_FLAG_SET(new_endpoint_info->flags, BCM_BFD_ENDPOINT_MULTIHOP) ||
              _SHR_IS_FLAG_SET(new_endpoint_info->flags2, BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR)))
         || (bcmBFDTunnelTypeMpls == new_endpoint_info->type)) &&
        ((new_endpoint_info->ip_tos != existing_endpoint_info->ip_tos) ||
         (new_endpoint_info->ip_ttl != existing_endpoint_info->ip_ttl)))
    {
                /** A different TOS/TTL profile is needed */
        SHR_IF_ERR_EXIT(dnx_bfd_tos_ttl_profile_sw_update(unit, new_endpoint_info, mep_hw_write_data));

        SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_tos_ttl.free_single
                        (unit, mep_entry_values->ip_ttl_tos_profile, &oamp_hw_delete_data->tos_ttl_delete));
    }

    if ((new_endpoint_info->type != bcmBFDTunnelTypeUdp) &&
        ((new_endpoint_info->egress_label.ttl != existing_endpoint_info->egress_label.ttl) ||
         (new_endpoint_info->egress_label.exp != existing_endpoint_info->egress_label.exp))
        && (!_SHR_IS_FLAG_SET(new_endpoint_info->flags2, BCM_BFD_ENDPOINT_FLAGS2_SINGLE_HOP_WITH_RANDOM_DIP)))
    {
                /** A different TTL/EXP profile is needed */
        SHR_IF_ERR_EXIT(dnx_bfd_mpls_pwe_ttl_exp_profile_sw_update(unit, new_endpoint_info, mep_hw_write_data));

        SHR_IF_ERR_EXIT(algo_oam_db.oam_mpls_pwe_exp_ttl_profile.free_single
                        (unit, mep_entry_values->push_profile, &oamp_hw_delete_data->ttl_exp_profile_delete));
    }

    if ((new_endpoint_info->local_min_rx != existing_endpoint_info->local_min_rx) ||
        (new_endpoint_info->local_min_tx != existing_endpoint_info->local_min_tx))
    {
                /** Different interval profiles are needed */
        SHR_IF_ERR_EXIT(dnx_bfd_req_int_profile_sw_update(unit, new_endpoint_info, mep_hw_write_data));

        SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_req_interval.free_single
                        (unit, mep_entry_values->min_rx_interval_profile, &oamp_hw_delete_data->req_min_rx_int_delete));
        SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_req_interval.free_single(unit,
                                                                      mep_entry_values->min_tx_interval_profile,
                                                                      &oamp_hw_delete_data->req_min_tx_int_delete));
    }

    if ((new_endpoint_info->sampling_ratio != existing_endpoint_info->sampling_ratio) ||
        ((new_endpoint_info->flags & PUNT_PROFILE_FLAGS) != (existing_endpoint_info->flags & PUNT_PROFILE_FLAGS)))
    {
                /** A different punt profile is needed */
        SHR_IF_ERR_EXIT(dnx_bfd_punt_profile_sw_update(unit, new_endpoint_info, rmep_hw_write_data));

        SHR_IF_ERR_EXIT(algo_oam_db.oamp_punt_event_prof_template.free_single
                        (unit, rmep_entry_values->punt_profile, &oamp_hw_delete_data->punt_profile_delete));
    }

    if ((new_endpoint_info->punt_good_packet_period != existing_endpoint_info->punt_good_packet_period))
    {
        /** A different punt good profile is needed */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_punt_good_profile_alloc
                        (unit, 1 /* Exchange */ , new_endpoint_info->punt_good_packet_period,
                         rmep_entry_values->punt_good_profile, punt_good_hw_write_data));
    }

    /**
     * Single-hop IPv4 TOS - one value for all endpoints of this type.
     * Modifying the value from one endpoint changes it for all
     * single-hop IPv4 accelerated endpoints
     */
    if (SINGLE_HOP_U_BFD_TOS_SUPPORTED(unit) &&
        (bcmBFDTunnelTypeUdp == new_endpoint_info->type) &&
        !_SHR_IS_FLAG_SET(new_endpoint_info->flags, BCM_BFD_ENDPOINT_MULTIHOP) &&
        (new_endpoint_info->ip_tos != existing_endpoint_info->ip_tos))
    {
        SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_single_hop_tos.replace_data(unit, 0, &new_endpoint_info->ip_tos));

        mep_hw_write_data->oamp_single_hop_tos_write = TRUE;
        mep_hw_write_data->oamp_single_hop_tos = new_endpoint_info->ip_tos;
    }

    /**
     * Local minimum echo interval - one value for all accelerated BFD endpoints.
     * Modifying the value from one endpoint changes it for all accelerated
     * BFD endpoints
     */
    if (new_endpoint_info->local_min_echo != existing_endpoint_info->local_min_echo)
    {
        SHR_IF_ERR_EXIT(algo_bfd_db.bfd_local_min_echo.replace_data(unit, 0, &new_endpoint_info->local_min_echo));

        mep_hw_write_data->oamp_local_min_echo_write = TRUE;
        mep_hw_write_data->oamp_local_min_echo = new_endpoint_info->local_min_echo;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in bfd_oamp.h for function description
 */
shr_error_e
dnx_bfd_oamp_short_entry_type_verify(
    int unit,
    uint16 oam_id,
    const bcm_bfd_endpoint_info_t * endpoint_info)
{
    uint32 dummy_flags = 0;
    int sub_index;
    dbal_enum_value_field_oamp_mep_type_e req_mep_type;
    uint8 is_leader_alloc, dummy_ach_sel, is_leader_type_match;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_bfd_mep_type_parsing
                    (unit, endpoint_info->type, endpoint_info->flags, endpoint_info->flags2, endpoint_info->ip_tos,
                     &req_mep_type, &dummy_flags, &dummy_ach_sel));

    SHR_IF_ERR_EXIT(dnx_oam_oamp_short_entry_type_check_mep_db
                    (unit, oam_id, req_mep_type, &sub_index, &is_leader_alloc, &is_leader_type_match));

    if (sub_index != 0)
    {
        if (is_leader_alloc == FALSE)
        {
            /** Entry not found */
            SHR_ERR_EXIT(_SHR_E_PARAM, "Short entries with a non-zero sub-index can only "
                         "be added after the zero sub-index of the same full entry was added, "
                         "so you must first add an entry with local_discr=0x%08X before you can "
                         "add an entry with local_discr=0x%08X", endpoint_info->local_discr - sub_index,
                         endpoint_info->local_discr);
        }

        if (is_leader_type_match == FALSE)
        {
            /** MEP type is not the same */
            SHR_ERR_EXIT(_SHR_E_PARAM, "Short entries with a non-zero sub-index must have "
                         "the same MEP-type as the zero sub-index of the same full entry, "
                         "so entry with local_discr=0x%08X must have the same MEP type as "
                         "entry with local_discr=0x%08X", endpoint_info->local_discr - sub_index,
                         endpoint_info->local_discr);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in bfd_oamp.h for function description
 */
shr_error_e
dnx_oamp_set_mpls_lm_dm_mep_profile(
    int unit,
    bcm_bfd_endpoint_t endpoint,
    uint8 new_profile)
{
    dnx_oam_oamp_mep_db_args_t *oamp_mep_db_entry = NULL;
    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC_SET_ZERO(oamp_mep_db_entry, sizeof(dnx_oam_oamp_mep_db_args_t),
                       "Param struct for creating single entries", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, endpoint, oamp_mep_db_entry));
    oamp_mep_db_entry->mep_profile = new_profile;
    UTILEX_SET_BIT(oamp_mep_db_entry->flags, TRUE, OAMP_MEP_UPDATE);
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, endpoint, oamp_mep_db_entry));

exit:
    SHR_FREE(oamp_mep_db_entry);
    SHR_FUNC_EXIT;
}
