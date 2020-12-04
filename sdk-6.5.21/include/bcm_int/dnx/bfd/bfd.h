/** \file dnx/bfd/bfd.h
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BFD_INIT_H_INCLUDED
/*
 * {
 */
#define BFD_INIT_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * }
 */
#include <soc/dnx/dbal/dbal.h>

#define DNX_BFD_IPV6_HEADER_SIZE 40

#define DNX_BFD_IPV6_PSEUDO_HEADER_SIZE 36

#define DNX_BFD_IPV6_SOUCRE_ADDRESS_OFFSET 8

#define DNX_BFD_IPV6_DESTINATION_ADDRESS_OFFSET 24

#define DNX_BFD_UDP_HEADER_SIZE 8

#define DNX_BFD_BFD_HEADER_SIZE 24

#define DNX_BFD_CHECKSUM_HEADER_SIZE (DNX_BFD_IPV6_PSEUDO_HEADER_SIZE + DNX_BFD_UDP_HEADER_SIZE + DNX_BFD_BFD_HEADER_SIZE)

#define DNX_BFD_UDP_OVER_IPV6_HEADER_SIZE (DNX_BFD_UDP_HEADER_SIZE + DNX_BFD_IPV6_HEADER_SIZE)

#define DNX_BFD_IPV6_NOF_EXTRA_DATA_ENTRIES 2

#define DNX_BFD_SBFD_REFLECTOR_UDP 7784

#define DNX_BFD_OAM_LIF_FROM_TCAM_INSTRUCTION 0X400000

/* BFDoIPv6 network headers - used for the implementation of BFDoIPv6
 * OAMP-PE program removes IPv4 header and adds IPv6 header that is taken from additional(extra) data
 * we use these headers structures for conveniently build the headers.
 */

/* IPv6 header */
typedef struct
{
    uint32 version_prior_flow_label;    /* 4 bits version, 8 bits prior, 20 bits flow-ID */
    uint16 payload_length;
    uint8 next_header;
    uint8 hop_limit;
    bcm_ip6_t source_address;
    bcm_ip6_t destination_address;
} bcm_bfd_ipv6_header_t;

/* IPv6 pseudo header - used for UDP checksum calculation */
typedef struct
{
    bcm_ip6_t source_address;
    bcm_ip6_t destination_address;
    uint8 zero;
    uint8 protocol;
    uint16 udp_length;
} bcm_bfd_ipv6_pseudo_header_t;

/* UDP header */
typedef struct
{
    uint16 source_port;
    uint16 destination_port;
    uint16 length;
    uint16 checksum;
} bcm_bfd_udp_header_t;

/* bfd header - used for UDP checksum calculation (OAMP-PE needs only UDP and IPv6 headers) */
typedef struct
{
    uint8 vers_diag;            /* 3 bits vers, 5 bits diag */
    uint8 sta_flags;            /* 2 bits sta, 6 bits flags */
    uint8 detect_mult;
    uint8 length;
    uint32 my_discriminator;
    uint32 your_discriminator;
    uint32 desired_min_tx_interval;
    uint32 required_min_rx_interval;
    uint32 required_min_echo_rx_interval;
} bcm_bfd_bfd_header_t;

/* in UDP, the checksum of IPv6 pseudo header, UDP header and UDP data
 * (bfd in our case) is being calculated and saved in the checksum field
 * before sending the packet
 */

typedef struct
{
    bcm_bfd_ipv6_pseudo_header_t ipv6_pseudo_header;
    bcm_bfd_udp_header_t udp_header;
    bcm_bfd_bfd_header_t bfd_header;
} bcm_bfd_checksum_header_t;

/* this represents the structure of the data we should send to the OAMP-PE */
typedef struct
{
    bcm_bfd_ipv6_header_t ipv6_header;
    bcm_bfd_udp_header_t udp_header;
} bcm_bfd_upd_over_ipv6_header_t;

/**
 * \brief - Initialize BFD module.
 * This function calls initialization functions for BFD
 * procedures
 *
 * Called from init sequence as part of dnx init pp sequence
 *
 * \param [in] unit - Identifying number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_bfd_init(
    int unit);

/**
 * \brief - This function sets the value of the my-BFD-DIP destination.
 *          This value can then be used to create BFD endpoints that use
 *          DIPs that will not be stored in the 16-entry hard logic table,
 *          thus allowing more than 16 different DIPs.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] destination - value to write to register.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_bfd_my_dip_destination_set(
    int unit,
    uint32 destination);

/**
 * \brief - This function reads the value of the my-BFD-DIP destination.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] destination - pointer to which result should be written.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_bfd_my_dip_destination_get(
    int unit,
    uint32 *destination);

/**
 * \brief - Modify the MP profile ID for a BFD EP.
 * Delete ACC-MEP-action-DB entry and create a new one.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint - ID of endpoint to modify.
 * \param [in] new_profile_id - profile ID to set.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_bfd_modify_mp_profile_id(
    int unit,
    int endpoint,
    int new_profile_id);

#endif /* BFD_INIT_H_INCLUDED */
