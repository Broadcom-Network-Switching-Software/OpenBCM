/**
 * \file algo_bfd.h Internal DNX L3 Managment APIs
PIs This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
PIs 
PIs Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef ALGO_BFD_H_INCLUDED
/*
 * { 
 */
#define ALGO_BFD_H_INCLUDED

#include <shared/shrextend/shrextend_debug.h>

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>

/**
 * Resource name defines for algo bfd 
 * \see
 * dnx_bfd_init_templates 
 * {
 */
#define DNX_BFD_TEMPLATE_OAMA_ACTION               "BFD MEP OAMA Action"

#define DNX_BFD_TEMPLATE_OAMB_ACTION               "BFD MEP OAMB Action"

#define DNX_BFD_TEMPLATE_DIP                       "BFD DIP"

#define DNX_BFD_SERVER_TRAP_CODES                  "BFD SERVER Trap Codes"

#define DNX_BFD_TEMPLATE_YOUR_DISCR_RANGE          "BFD Your-Discr Range"

#define DNX_BFD_OAMP_TEMPLATE_TX_RATE              "BFD OAMP TX Rate"

#define DNX_BFD_OAMP_TEMPLATE_REQ_INTERVAL         "BFD OAMP Required Interval"

#define DNX_BFD_OAMP_TEMPLATE_MY_DISCR_RANGE_START "BFD OAMP My-Discr Range Start"

#define DNX_BFD_OAMP_TEMPLATE_TOS_TTL              "BFD OAMP TOS-TTL"

#define DNX_BFD_OAMP_TEMPLATE_IPV4_SRC_ADDR        "BFD OAMP IPv4 Source Addr"

#define DNX_BFD_OAMP_TEMPLATE_UDP_SPORT            "BFD OAMP UDP SPORT"

#define DNX_BFD_OAMP_TEMPLATE_MPLS_SPORT           "BFD OAMP MPLS SPORT"

#define DNX_BFD_TEMPLATE_OAMP_SINGLE_HOP_TOS       "BFD OAMP SINGLE-HOP TOS"

#define DNX_BFD_TEMPLATE_LOCAL_MIN_ECHO            "BFD OAMP LOCAL MIN ECHO"

#define DNX_BFD_TEMPLATE_TX_PERIOD_CLUSTER         "BFD OAMP TX PERIOD CLUSTER"

#define DNX_BFD_OAMP_TEMPLATE_SEAMLESS_UDP_SPORT   "BFD OAMP SEAMLESS UDP SPORT"

#define DNX_BFD_TEMPLATE_SEAMLESS_UDP_SPORT        "BFD SEAMLESS UDP SPORT"

/**
 * }
 */

/** Structure used to store BFD MEP profiles as templates */
typedef struct
{
    /** 
     *  The trap code points to a defined trap - a possible
     *  destination for a received packet.  In each stage of the
     *  pipeline different traps with different forwarding
     *  strengths are encountered.  The trap with the highest
     *  forwrarding strength wins.
     */
    uint16 trap_code;

    /** The forwarding strength for the above trap code.   */
    uint8 forwarding_stregth;

    /** 
     *  Similarly to trap strength, snoop strength is used to
     *  to decide the destination of a copy of the original
     *  packet.  While trap strength and snoop strength can be
     *  different, trap code is also used for snoop code, so at
     *  the end of the pipeline it is possible for the resulting
     *  trap code and snoop code to be different (because one
     *  stage of the pipeline had the highest trap strength,
     *  while a different stage had the highest snoop strength.)
     */
    uint8 snoop_strength;
} dnx_bfd_mep_profile_t;

/** 
 *  Structure used to store BFD discriminator range as
 *  template.  In BFD packets, the discriminator is used to
 *  establish a sequence of packets between two endpoints, where
 *  each packet contains a "my discriminator" and a "your
 *  discriminator" field.  If device X sends BFD packets to
 *  device Y with my discriminator=A and your-discriminator=B,
 *  device Y should send BFD packet to device X with the values
 *  inverted: my discriminator=B and your-discriminator=A.  This
 *  setting limits the values allowed for my-discriminator.
 *  These limits are applied to the most significant 18 bits in
 *  a 32 bit word.  Therefore, even if range_min=range_max
 *  (which is currently the case for dune devices) there are
 *  2^14 possible values for my-discriminator.
 */
typedef struct
{
    uint32 range_min;
    uint32 range_max;
} dnx_bfd_discr_range_t;

/** 
 *  Structure used to store profiles that contain "type of
 *  service/time to live" profiles as template.  These
 *  values are used by the OAMP to construct BFD packets for
 *  transmission.
 */
typedef struct
{
    uint8 tos;
    uint8 ttl;
} dnx_bfd_tos_ttl_t;

/**
 *  Structure used to store profiles that contain "(clustered)
 *  Tx period" profiles as template.  These values are used by
 *  the OAMP for transmission rate, and if cluster ID is not 0,
 *  allow the modification of transmission rate for multiple
 *  endpoints simultaneously.
 */
typedef struct
{
    uint32 tx_period;
    uint32 cluster_profile;
} dnx_bfd_tx_period_t;

/**
 * }
 */

/**
 * \brief - initialize templates for all BFD profile types. 
 *        Current BFD Templates:\n
 *  BFD MEP OAMA Action - action profiles for non-accelerated
 *  BFD endpoints (stored in OAMA table.)\n
 *  BFD MEP OAMB Action - action profiles for accelerated BFD
 *  endpoints (either to local or remote OAMP, stored in OAMB
 *  table.)\n
 *  BFD DIP - Destination IPs.  In non-accelerated BFD
 *  endpoints, these are only relevant for multi-hop.
 *  BFD Your-Discr Range - The upper and lower values of the
 *  most significant 18 bits of the my-discriminator field.
 * 
 * \param [in] unit - Number of hardware unit used.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_bfd_init(
    int unit);

/**
 * \brief - Printing callback for the BFD MEP action templates, 
 *        which are used to track the references to BFD profile
 *        entries in the OAMA and OAMB tables.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data.  The BFD 
 *        action profile structure dnx_bfd_mep_profile_t is
 *        defined and described above.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */

void dnx_bfd_mep_action_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Printing callback for the BFD DIP template, which is 
 *        use to track the refernces to entries in the BFD DIP
 *        hardware table by multihop BFD endpoints.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data.  This 
 *        data is in the format of an IPv6 address, and is
 *        displayed as such.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_bfd_dip_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Printing callback for the BFD SERVER trap codes template, which is 
 *        use to track the references to BFD SERVER trap codes in the virtual
 *        registers CFG_BFDv6_TRAP_TO_REMOTE_OAMP_TRAP_CODE_0/1/2
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to BFD SERVER trap codes data.  This 
 *        data is in the format of uint16, and is
 *        displayed as such.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_bfd_server_trap_codes_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Printing callback for the BFD Discriminator Range 
 *        template, which is used to track the references to the
 *        discriminator range registers by BFD endpoints that
 *        receive packets classified by discriminator.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data.  The BFD
 *        discriminator range structure dnx_bfd_discr_range_t is
 *        defined and described above.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_bfd_your_discr_range_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Printing callback for the BFD Tx rate 
 *        template, which is used to configure packet
 *        transmissions for BFD accelerated endpoints.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data.  The Tx 
 *        rate is a 10-bit integer.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_bfd_tx_rate_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Printing callback for the BFD required interval 
 *        template, which is used to negotiate the rate that
 *        remote endpoints transmit to BFD accelerated
 *        endpoints.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data. 
 *        Intervals are 32-bit integers.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_bfd_req_int_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Printing callback for the BFD My Discriminator Range
 *        Start template, which is used to track the references
 *        to the discriminator range start registers by
 *        accekerated BFD endpoints that have IPv4 or MPLS
 *        tunneling.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data. This 
 *        offset is a 32-bit integer.
 * \return
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_bfd_discr_range_start_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Printing callback for the BFD ToS TTL template. 
 *        These two values are used by the OAMP to construct BFD
 *        packet to transmit for accelerated endpoint.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data. These
 *        values are one byte each.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_bfd_tos_ttl_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Printing callback for the BFD source address 
 *        template. This IPv4 address is used by the OAMP to
 *        construct the BFD packet to transmit for an
 *        accelerated endpoint.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data. The data
 *        is an IPv4 address - 4 bytes.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_bfd_ipv4_src_addr_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Printing callback for the BFD UDP source port
 *        template. This IPv4 address is used by the OAMP to
 *        construct the BFD packet to transmit for an
 *        accelerated endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data. The data
 *        is a port - 2 bytes.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM sdnx_bfd_udp_src_port_profile_print_cbhell diagnostics.
 * \see
 *   * None
 */
void dnx_bfd_udp_src_port_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Printing callback for the BFD MPLS source port
 *        template. This IPv4 address is used by the OAMP to
 *        construct the BFD packet to transmit for an
 *        accelerated endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data. The data
 *        is a port - 2 bytes.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_bfd_mpls_src_port_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Printing callback for the BFD MPLS single hop TOS
 *        template. This TOS value is used by the OAMP to
 *        construct the BFD packet to transmit for a single-hop
 *        IPv4 accelerated endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data. The data
 *        is a TOS - 1 byte.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_bfd_oamp_single_hop_tos_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Printing callback for the BFD local min echo
 *        template. The local min echo is used in Tx packets
 *        to notify remote devices what is the minimum echo
 *        interval this device can support.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data. The data
 *        is the interval - 4 bytes..
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_bfd_oamp_bfd_local_min_echo_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Printing callback for the BFD Tx Period Cluster
 *        template. The Tx Period Cluster parameter allows
 *        the user to modify the Tx Period for any number
 *        of accelerated BFD endpoints simultaneously.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data. The data
 *        is the Tx period cluster (1-8)
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_bfd_oamp_bfd_tx_period_cluster_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Printing callback for the seamless BFD UDP source
 *        port template. This IPv4 address is used by the OAMP
 *        to construct the seamless BFD packet to transmit for
 *        an accelerated endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data. The data
 *        is an port - 2 bytes.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM sdnx_bfd_udp_src_port_profile_print_cbhell diagnostics.
 * \see
 *   * None
 */
void dnx_bfd_seamless_udp_src_port_profile_print_cb(
    int unit,
    const void *data);

/*
 * } 
 */
#endif/*_ALGO_QOS_API_INCLUDED__*/
