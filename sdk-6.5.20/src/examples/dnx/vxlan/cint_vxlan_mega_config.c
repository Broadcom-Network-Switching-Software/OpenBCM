/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * Mega Configuration which includes several application toghther in the same CINT
 * The purpose of the CINT is to try to create one application and several packet scenarios within the same CINT
 *
 *
 * cint ../../src/examples/sand/cint_sand_utils_global.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_multicast.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_vxlan.c
 * cint ../../src/examples/dnx/cint_vxlan_mega_config.c
 * cint
 * int unit = 0;
 * int rv = 0;
 * int access_port1 = 200;
 * int access_port2 = 201;
 * int core_port1 = 202;
 * int core_port2 = 203;
 * rv = vxlan_roo_basic(unit, access_port1, access_port2, core_port1, core_port2);
 * print rv;
 *
 * Traffic:
 *
 *  Scenarios configured in this cint:
 *  The first example
 *  Routing out of overlay:
 *
 *  ###############################################################################################################
 *  Sending packet from port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *      0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
 *   |  ----------------------------------------------------
 *   |  |                                 DA  (00:0C:00:02:00:00)                           |
 *   |  |-----------------------------------------------------------------------------------|    LL Ethernet
 *   |  |               DA                   |              SA  (00:00:07:00:01:00)         |
 *   |  |-----------------------------------------------------------------------------------|       Header
 *   |  |                                 SA                                                |
 *   |  |-----------------------------------------------------------------------------------|     ( 14 bytes)
 *   |  |       Ethertype = 0x0800(IPv4)     |                                              |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |     Version  |  IHL   |   ToS      |          Total Lenght                        |
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |        Identification              |   Flags   |  Fragment Offset                 |   Tunnel   IPv4
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |         TTL       |Protocol=17(UDP)|             Header Checksum                  |       Header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Source address ( 172.0.0.1 )                         |      (20 bytes)
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Destination address (171.0.0.1)                      |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |    Source port (0x5555)            |    Destination port (0x5555)                 |   UDP  Header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |    UDP lenght                      |    UDP checksum                              |    (8 bytes)
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  | | R | R |  Ver  | I | P | R | O |  |    Reserved      | Next Protocol             |  VXLAN  header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |    VXLAN Network Identifier (VNI) = 0x5000            | Reserved                  |    (8 bytes)
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |                               DA  (00:00:00:00:00:f1)                             |
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |               DA                   |              SA (00:0C:00:02:00:01)          |    Native
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                                 SA                                                |   Ethernet
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |          TPID = 0x8100(VLAN)       |         |           VID=30                   |    Header
 *   |  |-----------------------------------------------------------------------------------|  (18 bytes)
 *   |  |       Ethertype = 0x0800(IPv4)     |                                              |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |     Version  |  IHL   |   ToS      |          Total Lenght                        |
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |        Identification              |   Flags   |  Fragment Offset                 |        IPv4
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |         TTL       |Protocol=17(UDP)|             Header Checksum                  |       Header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Source address ( 127.255.255.01 )                    |      (20 bytes)
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Destination address (127.255.255.03)                 |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  ------------------------------------------------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Receiving packet on port == 203:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                               DA   ( 00:00:00:00:CD:1F)                           | 
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |               DA                    |             SA   (00:0F:00:02:0A:44)        |
 *   |  |-----------------------------------------------------------------------------------|   Ethernet
 *   |  |                                 SA                                                |
 *   |  |-----------------------------------------------------------------------------------|    Header
 *   |  |          TPID = 0x8100(VLAN)        |         |           VID=20                  |  ( 18 bytes )
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |       Ethertype = 0x0800(IPv4)       |                                            |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |     Version  |  IHL   |   ToS      |          Total Lenght                        |
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |        Identification              |   Flags   |  Fragment Offset                 |        IPv4
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |         TTL       |Protocol=17(UDP)|             Header Checksum                  |       Header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Source address ( 127.255.255.01 )                    |      (20 bytes)
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Destination address (127.255.255.03)                 |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *
 *  Routing into Overlay:
 *
 *  Sending packet on port == 200: ------------>
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                               DA   ( 00:0F:00:02:0A:44)                           | 
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |               DA                    |             SA   (00:00:00:00:CD:1F)        |
 *   |  |-----------------------------------------------------------------------------------|   Ethernet
 *   |  |                                 SA                                                |
 *   |  |-----------------------------------------------------------------------------------|    Header
 *   |  |          TPID = 0x8100(VLAN)        |         |           VID=20                  |  ( 18 bytes )
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |       Ethertype = 0x0800(IPv4)      |                                             |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |     Version  |  IHL   |   ToS      |          Total Lenght                        |
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |        Identification              |   Flags   |  Fragment Offset                 |        IPv4
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |         TTL       |Protocol=17(UDP)|             Header Checksum                  |       Header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Source address ( 127.255.255.01 )                    |      (20 bytes)
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Destination address (127.255.255.03)                 |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Receiving packet on port = 203 <----------------
 *   |  ----------------------------------------------------
 *   |  |                                 DA  (00:0C:00:02:00:00)                           |
 *   |  |-----------------------------------------------------------------------------------|    LL Ethernet
 *   |  |               DA                   |              SA  (00:00:07:00:01:00)         |
 *   |  |-----------------------------------------------------------------------------------|       Header
 *   |  |                                 SA                                                |
 *   |  |-----------------------------------------------------------------------------------|     ( 14 bytes)
 *   |  |       Ethertype = 0x0800(IPv4)     |                                              |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |     Version  |  IHL   |   ToS      |          Total Lenght                        |
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |        Identification              |   Flags   |  Fragment Offset                 |   Tunnel   IPv4
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |         TTL       |Protocol=17(UDP)|             Header Checksum                  |       Header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Source address ( 172.0.0.1 )                         |      (20 bytes)
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Destination address (171.0.0.1)                      |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |    Source port (0x5555)            |    Destination port (0x5555)                 |   UDP  Header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |    UDP lenght                      |    UDP checksum                              |    (8 bytes)
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  | | R | R |  Ver  | I | P | R | O |  |    Reserved      | Next Protocol             |  VXLAN  header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |    VXLAN Network Identifier (VNI) = 0x5000            | Reserved                  |    (8 bytes)
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |                               DA  (00:00:00:00:00:f1)                             |
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |               DA                   |              SA (00:0C:00:02:00:01)          |    Native
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                                 SA                                                |   Ethernet
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |          TPID = 0x8100(VLAN)       |         |           VID=30                   |    Header
 *   |  |-----------------------------------------------------------------------------------|  (18 bytes)
 *   |  |       Ethertype = 0x0800(IPv4)     |                                              |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |     Version  |  IHL   |   ToS      |          Total Lenght                        |
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |        Identification              |   Flags   |  Fragment Offset                 |        IPv4
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |         TTL       |Protocol=17(UDP)|             Header Checksum                  |       Header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Source address ( 127.255.255.01 )                    |      (20 bytes)
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Destination address (127.255.255.03)                 |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  ------------------------------------------------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Receiving packet on port == 203:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                               DA   ( 00:00:00:00:CD:1E)                           | 
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |               DA                    |             SA   (00:0F:00:02:0A:33)        |
 *   |  |-----------------------------------------------------------------------------------|   Ethernet
 *   |  |                                 SA                                                |
 *   |  |-----------------------------------------------------------------------------------|    Header
 *   |  |          TPID = 0x8100(VLAN)        |         |           VID=20                  |  ( 18 bytes )
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |       Ethertype = 0x0800(IPv4)       |                                            |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |     Version  |  IHL   |   ToS      |          Total Lenght                        |
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |        Identification              |   Flags   |  Fragment Offset                 |   Tunnel   IPv4
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |         TTL       |Protocol=17(UDP)|             Header Checksum                  |       Header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Source address ( 172.0.0.1 )                         |      (20 bytes)
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Destination address (171.0.0.1)                      |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |    Source port (0x5555)            |    Destination port (0x5555)                 |   UDP  Header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |    UDP lenght                      |    UDP checksum                              |    (8 bytes)
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  | | R | R |  Ver  | I | P | R | O |  |    Reserved      | Next Protocol             |  VXLAN  header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |    VXLAN Network Identifier (VNI) = 0x5000            | Reserved                  |    (8 bytes)
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |                               DA  (00:00:00:00:00:f1)                             |
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |               DA                   |              SA (00:0C:00:02:00:01)          |    Native
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                                 SA                                                |   Ethernet
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |          TPID = 0x8100(VLAN)       |         |           VID=30                   |    Header
 *   |  |-----------------------------------------------------------------------------------|  (18 bytes)
 *   |  |       Ethertype = 0x0800(IPv4)     |                                              |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 *   |  |     Version  |  IHL   |   ToS      |          Total Lenght                        |
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |        Identification              |   Flags   |  Fragment Offset                 |        IPv4
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |         TTL       |Protocol=17(UDP)|             Header Checksum                  |       Header
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Source address ( 127.255.255.01 )                    |      (20 bytes)
 *   |  |-----------------------------------------------------------------------------------|
 *   |  |                         IPv4 Destination address (127.255.255.03)                 |
 *   |  |-----------------------------------------------------------------------------------|----------------------
 * ##################################################################################################################
 */

/*
 * VXLAN global structure
 */
struct cint_vxlan_mega_info_s
{
    int vxlan_network_group_id;                     /* ingress and egress orientation for VXLAN */
    bcm_ip_t tunnel_dip;                            /* tunnel DIP */
    bcm_ip_t tunnel_dip_mask;                       /* tunnel DIP mask */
    bcm_ip_t tunnel_sip;                            /* tunnel SIP */
    bcm_ip_t tunnel_sip_mask;                       /* tunnel SIP mask */
    bcm_ip6_t tunnel_ipv6_dip;                      /* tunnel DIP */
    bcm_ip6_t tunnel_ipv6_dip_mask;                 /* tunnel DIP mask*/
    bcm_ip6_t tunnel_ipv6_sip;                      /* tunnel SIP */
    bcm_ip6_t tunnel_ipv6_sip_mask;                 /* tunnel SIP mask */
    bcm_ip6_t encap_tunnel_ipv6_dip;                /* Encap Tunnel IPV6 DIP*/
    bcm_ip6_t encap_tunnel_ipv6_sip;                /* Encap tunnel IPV6 SIP*/
    bcm_ip6_t vxlan_tunnel_ipv6_dip;                /* VXLAN tunnel ipv6 dip*/
    bcm_ip6_t vxlan_tunnel_ipv6_sip;                /* VXLAN tunnel ipv6 sip*/
    bcm_gport_t tunnel_id;                          /* Tunnel ID */
    int vpn_id;                                     /* VPN ID */
    int vni;                                        /* VNI of the VXLAN */
    int access_port1;                               /* incoming port from access side 1 */
    int access_port2;                               /* incoming port from access side 2 */
    int core_port1;                                 /* incoming port from core side 1 */
    int core_port2;                                 /* incoming port from core side 2 */
    int core_native_eth_rif;                        /* core native RIF */
    int core_overlay_eth_rif;                       /* core overlay RIF */
    int core_overlay_eth_vrf;                       /* core overlay VRF */
    int access_eth_rif;                             /* access RIF */
    int access_eth_rif_public;                      /* access RIF after public LPM lookup*/
    int access_eth_vrf;                             /* access VRF */
    int vpbr_vrf;                                   /* VPBR VRF */
    bcm_vlan_t access_vid;                          /* access vid for AC */
    bcm_gport_t outer_ac_vlan_port_id;              /* vlan port id of the ac_port, used for ingress */
    int core_native_arp_id;                         /* Id for core native ARP entry */
    int core_overlay_arp_id;                        /* Id for core overlay ARP entry */
    int access_arp_id;                              /* Id for access ARP entry */
    int access_arp_id_public;                       /* Id for access ARP entry */
    int mpls_stack_arp_id;                          /* Id for MPLS stack entry */
    int core_native_fec_id1;                        /* core native fec id for native outrif and native arp 1 */
    int core_native_fec_id2;                        /* core native fec id for native outrif and native arp 2 */
    int core_overlay_fec_id1;                       /* core overlay fec id for overlay 1 */
    int core_overlay_fec_id2;                       /* core overlay fec id for overlay 2 */
    int access_fec_id1;                             /* access fec id for access outrif and access arp 1 */
    int access_fec_id2;                             /* access fec id for access outrif and access arp 2 */
    int access_fec_id_public;                       /* access fec id for access outrif and access arp when the routing is public LPM */
    int ipv6_provider_fec;                          /* fec id for ipv6 tunnel encapsulation */
    int mpls_fec_id;                                /* FEC ID pointing to MPLS encapsulation */
    int mpls2_fec_id;                               /* FEC ID2 pointing to MPLS encapsulation */
    int mpls3_fec_id;                               /* FEC ID3 pointing to MPLS encapsulation */
    bcm_mac_t access_eth_fwd_mac1;                  /* mac for bridge fwd 1 */
    bcm_mac_t access_eth_fwd_mac2;                  /* mac for bridge fwd 2 */
    bcm_mac_t core_native_eth_rif_mac;              /* mac for core native RIF */
    bcm_mac_t core_overlay_eth_rif_mac;             /* mac for core overlay RIF */
    bcm_mac_t access_eth_rif_mac;                   /* mac for access RIF */
    bcm_mac_t access_eth_rif_mac_public;            /* mac for access RIF when the routing is public LPM*/
    bcm_mac_t core_native_next_hop_mac;             /* mac for next core native hop */
    bcm_mac_t core_overlay_next_hop_mac;            /* mac for next core overlay hop */
    bcm_mac_t access_next_hop_mac;                  /* mac for next access hop */
    bcm_mac_t access_next_hop_mac_public;           /* mac for next access hop when the routing is public LPM*/
    bcm_ip_t route_dip_into_overlay;                /* first dip */
    bcm_ip_t route_dip_out_of_overlay;              /* second dip */
    bcm_ip_t host_dip_tunnel_out_of_overlay;        /* Host DIP for tunnel out of overlay */
    bcm_ip_t route_dip_into_mpls;                   /* route dip for mpls */
    bcm_ip_t mask_dip_into_mpls;                    /* mask dip for mpls */
    bcm_ip_t host_dip_out_of_overlay;               /* IPv4 Host DIP out of overlay */
    bcm_ip_t host_sip_out_of_overlay;               /* IPv4 Host SIP out of overlay */
    bcm_ip_t host_dip_into_mpls_stack;              /* IPv4 Host DIP into MPLS 12 labels encapsulation */
    bcm_ip_t public_dip_ipv4;                       /* LPM Public DIP4 for FWD lookup */
    bcm_ip_t public_dip_ipv4_mask;                  /* LPM Public DIP4 for FWD lookup mask*/
    bcm_ip6_t routev6_dip_out_of_overlay;           /* IPv6 Route DIP out of overlay */
    bcm_ip6_t maskv6_dip_out_of_overlay;            /* IPv6 Host DIP Mask out of overlay */
    bcm_ip6_t hostv6_dip_out_of_overlay;            /* IPv6 Host DIP out of overlay */
    bcm_ip6_t dip_to_tunnel;                        /* DIP for FWD lookup resulting in MPLS encapsulation */
    bcm_ip6_t dip_to_mpls;                          /* DIP for FWD lookup resulting in MPLS encapsulation*/
    bcm_ip6_t public_dip;                           /* LPM Public DIP for FWD lookup*/
    bcm_ip6_t public_dip_mask;                      /* LPM Public DIP for FWD lookup mask*/
    int outer_tpid;                                 /* Outer tpid for application */
    int inner_tpid;                                 /* Inner tpid for application */
    bcm_gport_t egress_native_ac_vlan_port_id;      /* vlan port id of the inner ac_port, used for egress */
    uint32 egress_vlan_edit_profile;                /* EVE profile */
    uint32 ingress_vlan_edit_profile;               /* IVE Profile */
    int tag_format;                                 /* Tag format */
    int tunnel_termination_vrf;                     /* VRF resolved after tunnel termination*/
    bcm_if_t core_overlay_encap_tunnel_intf;        /* Overlay tunnel encapsulation interface */
    bcm_if_t encap_ipv6_tunnel_intf;                /* Ipv6 tunnel encapsulation interface*/
    bcm_if_t push_tunnel_id;                        /* MPLS Push tunnel ID */
    bcm_if_t push_tunnel2_id;                       /* MPLS Push tunnel ID 2 */
    bcm_if_t push_tunnel3_id;                       /* MPLS Push tunnel ID 3 */
    bcm_if_t push_stack_tunnel_id;                  /* Tunnel ID of stack to push*/
    bcm_mpls_label_t termination_stack[4];          /* MPLS 5 labels stack to terminate */
    bcm_mpls_label_t swap_before_label;             /* MPLS Label before SWAP */
    bcm_mpls_label_t swap_after_label;              /* MPLS Label after SWAP */
    bcm_mpls_label_t push2_label;                   /* MPLS Push label 2 */
    bcm_mpls_label_t push3_label;                   /* MPLS Push label 3*/
    bcm_ip_t dip_term_tunnel;                       /* DIP of IP tunnel to terminate*/
    bcm_ip_t sip_term_tunnel;                       /* SIP of IP tunnel to terminate */

};

cint_vxlan_mega_info_s cint_vxlan_roo_basic_info = {
    /*
     * VXLAN Orientation,
     */
    1,
    /*
     * tunnel DIP
     */
    0xAB000001, /* 171.0.0.1 */
    /*
     * tunnel DIP mask
     */
    0xffffffff,
    /*
     * tunnel SIP
     */
    0xAC000001, /* 172.0.0.1 */
    /*
     * tunnel SIP mask
     */
    0xffffffff,
    /*
     * tunnel DIP
     */
    { 0x20, 0x01, 0x0D, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, 0x22, 0xFF, 0xFE, 0x33, 0x44, 0x55 },
    /*
     * tunnel DIP mask
     */
    { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
    /*
     * tunnel SIP
     */
    { 0xEC, 0xEF, 0xEE, 0xED, 0xEC, 0xEB, 0xEA, 0xE9, 0xE8, 0xE7, 0xE6, 0xE5, 0xE4, 0xE3, 0xE2, 0xE1 },
    /*
     * tunnel SIP mask
     */
    { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
    /*
     * encap_tunnel_ipv6_dip
     */
    { 0x20, 0x01, 0x0D, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, 0x22, 0xFF, 0xFE, 0x33, 0x44, 0x56 },
    /*
     * encap_tunnel_ipv6_sip
     */
    { 0xEC, 0xEF, 0xEE, 0xED, 0xEC, 0xEB, 0xEA, 0xE9, 0xE8, 0xE7, 0xE6, 0xE5, 0xE4, 0xE3, 0xE2, 0xE2 },
    /*
     * vxlan_tunnel_ipv6_dip
     */
    { 0x20, 0x01, 0x0D, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, 0x22, 0xFF, 0xFE, 0x33, 0x40, 0x50},
    /*
     * vxlan_tunnel_ipv6_sip
     */
    {0xEC, 0xEF, 0xEE, 0xED, 0xEC, 0xEB, 0xEA, 0xE9, 0xE8, 0xE7, 0xE6, 0xE5, 0xE4, 0xE3, 0xE0, 0xE0},
    /*
     * tunnel id | VPN ID | VNI
     */
    20, 15, 5000,
    /*
     * ports : access_port1 | access_port2 | core_port1 | core_port2
     */
    200, 201, 202, 203,
    /*
     * core_native_eth_rif | core_overlay_eth_rif| core_overlay_eth_vrf
     */
    15, 20 , 5,
    /*
     * access_eth_rif | access_eth_rif_public | access_eth_vrf | vpbr_vrf | access_vid | outer_ac_vlan_port_id
     */
    10, 20, 1, 0xa5a, 0x5a5, 8888,
    /*
     * core_native_arp_id | core_overlay_arp_id | access_arp_id | access_arp_id_public | mpls_stack_arp_id
     */
    0, 5050, 0, 0, 0,
    /*
     * core_native_fec_id1 | core_native_fec_id2 | core_overlay_fec_id1 | core_overlay_fec_id2 | access_fec_id1 | access_fec_id2 | access_fec_id_public |ipv6_provider_fec | mpls_fec_id | mpls2_fec_id | mpls3_fec_id
     */
    40961, 40963, 65537, 65539, 40965, 40967, 40970, 40978, 40969, 40971, 40973,
    /*
     * access_eth_fwd_mac1 | access_eth_fwd_mac2 | core_native_eth_rif_mac | core_overlay_eth_rif_mac | access_eth_rif_mac | access_eth_rif_mac_public
     */
    {0x00, 0x00, 0xA5, 0x5A, 0xA5, 0x5A}, {0x00, 0x00, 0x5A, 0xA5, 0x5A, 0xA5}, {0x00, 0x0F, 0x00, 0x02, 0x0A, 0x22},
    {0x00, 0x0F, 0x00, 0x02, 0x0A, 0x33}, {0x00, 0x0F, 0x00, 0x02, 0x0A, 0x44}, {0x00, 0x0F, 0x00, 0x02, 0x0A, 0x55},
    /*
     * core_native_next_hop_mac |  core_overlay_next_hop_mac | access_next_hop_mac | access_next_hop_mac_public
     */
    {0x00, 0x00, 0x00, 0x00, 0xCD, 0x1D}, {0x00, 0x00, 0x00, 0x00, 0xCD, 0x1E}, {0x00, 0x00, 0x00, 0x00, 0xCD, 0x1F}, {0x00, 0x00, 0x00, 0x00, 0xCD, 0x2F},
    /*
     *route_dip_into_overlay | route_dip_out_of_overlay | host_dip_tunnel_out_of_overlay
     *127.255.255.02 , 127.255.255.03, 51.136.135.121
     */
    0x7fffff02, 0x7fffff03, 0x33888779,
    /*
     * route_dip_into_mpls | mask_dip_into_mpls | host_dip_out_of_overlay | host_sip_out_of_overlay | host_dip_into_mpls_stack | public_dip_ipv4 | public_dip_ipv4_mask
     *122.90.90.5, 255.255.255.0, 31.90.90.90, 63.165.165.165, 100.99.87.33, 192.136.135.121
     */
    0x7a5a5a05, 0xffffff00, 0x1F5A5A5A, 0x3FA5A5A5, 0x64635721, 0xc0888779, 0xffffff00,
    /*
     * routev6_dip_out_of_overlay | maskv6_dip_out_of_overlay | hostv6_dip_out_of_overlay | dip_to_tunnel | dip_to_mpls | public_dip | public_dip_mask
     */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x10},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x15},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x63},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA5, 0xA5, 0x13, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x63},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x34, 0x00, 0x00, 0x00, 0xAA, 0xFF, 0x63},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0},
    /*
     * outer_tpid | inner_tpid 
     */
    0x8100, 0x88a8,
    /*
     * egress_native_ac_vlan_port_id | egress_vlan_edit_profile | ingress_vlan_edit_profile
     */
    0, 4, 5,
    /*
     * tag_format | tunnel_termination_vrf | core_overlay_encap_tunnel_intf | encap_ipv6_tunnel_intf | push_tunnel_id | push_tunnel2_id | push_tunnel3_id | push_stack_tunnel_id
     */
    0, 5, 0, 0, 0, 0, 0, 0,
    /*
     * termination_stack
     */
    {0xa5a5, 0x5a5a, 7373, 3737},
    /*
     * swap_before_label | swap_after_label | push2_label | push3_label
     */
    0x9a5a5, 0x15a5a, 0x8877, 0x9988,
    /*
     * dip_term_tunnel | sip_term_tunnel
     * 115.165.165.3, 55.90.90.4
     */
    0x73a5a503, 0x375a5a04
};

int
vxlan_roo_basic_init(
    int unit,
    int access_port1,
    int access_port2,
    int core_port1,
    int core_port2)
{

    int rv;
    int fec_id;
    cint_vxlan_roo_basic_info.access_port1 = access_port1;
    cint_vxlan_roo_basic_info.access_port2 = access_port2;
    cint_vxlan_roo_basic_info.core_port1 = core_port1;
    cint_vxlan_roo_basic_info.core_port2 = core_port2;

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }

    cint_vxlan_roo_basic_info.core_native_fec_id1 = fec_id++;
    cint_vxlan_roo_basic_info.core_native_fec_id2 = fec_id++;
    cint_vxlan_roo_basic_info.access_fec_id1 = fec_id++;
    cint_vxlan_roo_basic_info.access_fec_id2 = fec_id++;
    cint_vxlan_roo_basic_info.access_fec_id_public = fec_id++;
    cint_vxlan_roo_basic_info.ipv6_provider_fec = fec_id++;
    cint_vxlan_roo_basic_info.mpls_fec_id = fec_id++;
    cint_vxlan_roo_basic_info.mpls2_fec_id = fec_id++;
    cint_vxlan_roo_basic_info.mpls3_fec_id = fec_id;

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 1, &fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }
    cint_vxlan_roo_basic_info.core_overlay_fec_id1 = fec_id ;
    cint_vxlan_roo_basic_info.core_overlay_fec_id2 = fec_id+ 1;


    return rv;
}


/*
 * The basic VXLAN example with parameters:
 * Unit - relevant unit
 * access_port1,2 - port where core host is connected to.
 * core_port1,2 - port where DC Fabric router is connected to.
 */
int
vxlan_roo_basic(
    int unit,
    int access_port1,
    int access_port2,
    int core_port1,
    int core_port2)
{
    int rv;
    char *proc_name;
    bcm_if_t encap_tunnel_intf;
    int term_tunnel_id, vxlan_ipv6_term_tunnel_id;
    int encap_tunnel_id, ipv6_encap_tunnel_id;
    bcm_gport_t vlan_port_id1;
    bcm_gport_t vlan_port_id2;
    bcm_gport_t vxlan_port_id;
    bcm_gport_t tunnel_id;
    proc_name = "vxlan_roo_basic";

    rv = vxlan_roo_basic_init(unit,access_port1, access_port2, core_port1, core_port2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan_roo_basic_init\n");
        return rv;
    }


    /** Build L2 VPN */
    rv = vxlan_roo_basic_vxlan_open_vpn(unit, cint_vxlan_roo_basic_info.vpn_id, cint_vxlan_roo_basic_info.vni);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan_roo_basic_vxlan_open_vpn, vpn=%d, \n", proc_name, cint_vxlan_roo_basic_info.vpn_id);
        return rv;
    }

    /** Configure port properties */
    rv = vxlan_roo_basic_configure_port_properties(unit,
                                                   cint_vxlan_roo_basic_info.access_port1,
                                                   cint_vxlan_roo_basic_info.access_port2,
                                                   cint_vxlan_roo_basic_info.core_port1,
                                                   cint_vxlan_roo_basic_info.core_port2);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vxlan_roo_basic_configure_port_properties\n", rv);
        return rv;
    }

    /** Configure L3 interfaces */
    rv = vxlan_roo_basic_create_l3_interfaces(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_roo_basic_create_l3_interfaces\n", rv);
        return rv;
    }
    /** Create the tunnel termination lookup and the tunnel termination inlif */
    rv = vxlan_tunnel_terminator_create(unit, &term_tunnel_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function vxlan_tunnel_terminator_create \n", proc_name);
        return rv;
    }

    /** Create vxlan6 tunnel terminator */
    rv = vxlan_ipv6_tunnel_terminator_create(unit, &vxlan_ipv6_term_tunnel_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function vxlan_ipv6_tunnel_terminator_create \n", proc_name);
        return rv;
    }

    /** Create the Ipv6 tunnel termination lookup and the tunnel termination inlif*/
    rv = ipv6_gre_tunnel_terminator_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function ipv6_gre_tunnel_terminator_create \n", proc_name);
        return rv;
    }
    /** Create the Ipv6 tunnel termination lookup and the tunnel termination inlif*/
    rv = ipv6_tunnel_terminator_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function ipv6_tunnel_terminator_create \n", proc_name);
        return rv;
    }
    /** Configure ARP entries*/
    rv = vxlan_roo_basic_create_arp_entries(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vxlan_roo_basic_create_arp_entries \n", rv);
        return rv;
    }
    /** Create Overlay tunnel initiator*/
    rv = vxlan_tunnel_initiator_create(unit, cint_vxlan_roo_basic_info.core_overlay_arp_id,
                                       &(cint_vxlan_roo_basic_info.core_overlay_encap_tunnel_intf), &encap_tunnel_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vxlan_tunnel_initiator_create \n", rv);
        return rv;
    }
    /** Create ipv6 tunnel initiator*/
    rv = ipv6_tunnel_initiator_create(unit, cint_vxlan_roo_basic_info.access_arp_id,
                                       &(cint_vxlan_roo_basic_info.encap_ipv6_tunnel_intf));
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function tunnel_initiator_configure \n", proc_name);
        return rv;
    }
    /** Create MPLS swap tunnel*/
    rv = mpls_encapsulation_basic_create_push_or_swap_tunnel(unit, cint_vxlan_roo_basic_info.swap_after_label,
                                                             cint_vxlan_roo_basic_info.access_arp_id,
                                                             bcmEncapAccessTunnel3,
                                                             &cint_vxlan_roo_basic_info.push_tunnel_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, mpls_encapsulation_basic_create_push_or_swap_tunnel\n", proc_name);
        return rv;
    }
    /** Create MPLS push tunnel*/
    rv = mpls_encapsulation_basic_create_push_or_swap_tunnel(unit, cint_vxlan_roo_basic_info.push2_label,
                                                             cint_vxlan_roo_basic_info.push_tunnel_id,
                                                             bcmEncapAccessTunnel2,
                                                             &cint_vxlan_roo_basic_info.push_tunnel2_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, mpls_encapsulation_basic_create_push_or_swap_tunnel\n", proc_name);
        return rv;
    }
    /** Create MPLS push tunnel*/
    rv = mpls_encapsulation_basic_create_push_or_swap_tunnel(unit, cint_vxlan_roo_basic_info.push3_label,
                                                             cint_vxlan_roo_basic_info.push_tunnel2_id,
                                                             bcmEncapAccessTunnel1,
                                                             &cint_vxlan_roo_basic_info.push_tunnel3_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, mpls_encapsulation_basic_create_push_or_swap_tunnel\n", proc_name);
        return rv;
    }
    /** Create MPLS 12 labels push stack*/
    rv = mpls_encapsulation_deep_stack(unit, cint_vxlan_roo_basic_info.mpls_stack_arp_id, &cint_vxlan_roo_basic_info.push_stack_tunnel_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, mpls_encapsulation_deep_stack 5\n", proc_name);
        return rv;
    }
    /** Configure fec entries */
    rv = vxlan_roo_basic_create_fec_entries(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vxlan_roo_basic_create_fec_entries\n", rv);
        return rv;
    }
    /** Add VXLAN port properties and configure the tunnel terminator In-Lif and tunnel initiator Out-LiF */
    rv = vxlan_port_add(unit, core_port1, term_tunnel_id, encap_tunnel_id, &vxlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan_port_add \n", proc_name);
        return rv;
    }
    /** Create Forwarding entries*/
    rv = vxlan_roo_basic_l3_forwarding(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_roo_basic_l3_forwarding\n", rv);
        return rv;
    }
    /**Configure EVE commands for core port 1*/
    rv = vxlan_roo_basic_eve_swap(unit, cint_vxlan_roo_basic_info.core_port1);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_roo_basic_eve_swap\n", rv);
        return rv;
    }
    /**Configure EVE commands for core port 2*/
    rv = vxlan_roo_basic_eve_swap(unit, cint_vxlan_roo_basic_info.core_port2);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_roo_basic_eve_swap\n", rv);
        return rv;
    }
    /** Configure vlan port*/
    rv = vlan_port_add(unit, access_port1, cint_vxlan_roo_basic_info.access_vid, &vlan_port_id1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vlan_port_add \n", proc_name);
        return rv;
    }
    rv = bcm_vlan_gport_add(unit, cint_vxlan_roo_basic_info.access_vid, access_port1, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_vlan_gport_add \n", proc_name);
        return rv;
    }

    /** Configure vlan port*/
    rv = vlan_port_add(unit, access_port2, cint_vxlan_roo_basic_info.access_vid, &vlan_port_id2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vlan_port_add \n", proc_name);
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, cint_vxlan_roo_basic_info.access_vid, access_port2, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_vlan_gport_add \n", proc_name);
        return rv;
    }

    /** Configure vswitch with the created vlan port*/
    rv = vswitch_add_port(unit, cint_vxlan_roo_basic_info.vpn_id, vlan_port_id1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vswitch_add_port\n", proc_name);
        return rv;
    }
    /** Configure vswitch with the created vlan port*/
    rv = vswitch_add_port(unit, cint_vxlan_roo_basic_info.vpn_id, vlan_port_id2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vswitch_add_port\n", proc_name);
        return rv;
    }
    /** Create an MACT entry to VXLAN tunnel, mapped to the VPN */
    rv = mact_entry_add(unit, cint_vxlan_roo_basic_info.access_eth_fwd_mac1, cint_vxlan_roo_basic_info.vpn_id, vxlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, mact_entry_add\n", proc_name);
        return rv;
    }
    /** Create an MACT entry to VXLAN tunnel, mapped to the VPN */
    rv = mact_entry_add(unit, cint_vxlan_roo_basic_info.access_eth_fwd_mac2, cint_vxlan_roo_basic_info.vpn_id, vxlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, mact_entry_add\n", proc_name);
        return rv;
    }
    /** Add MPLS termination entries */
    tunnel_id = 0x15a5a;
    rv = mpls_termination_basic_create_termination_tunnel(unit, cint_vxlan_roo_basic_info.termination_stack[0],
                                                          &tunnel_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, mpls_termination_basic_create_termination_tunnel 1\n", proc_name);
        return rv;
    }
    /** Add MPLS termination entries */
    tunnel_id = 0x2a5a5;
    rv = mpls_termination_basic_create_termination_tunnel(unit, cint_vxlan_roo_basic_info.termination_stack[1],
                                                          &tunnel_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, mpls_termination_basic_create_termination_tunnel 2\n", proc_name);
        return rv;
    }
    /** Add MPLS termination entries */
    tunnel_id = 0x37373;
    rv = mpls_termination_basic_create_termination_tunnel(unit, cint_vxlan_roo_basic_info.termination_stack[2],
                                                          &tunnel_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, mpls_termination_basic_create_termination_tunnel 3\n", proc_name);
        return rv;
    }
    /** Use 18 bits global Out-LIFf for compatiblity with JR1 system headers mode.*/
    tunnel_id = 0x3f737;
    rv = mpls_termination_basic_create_termination_tunnel(unit, cint_vxlan_roo_basic_info.termination_stack[3],
                                                          &tunnel_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, mpls_termination_basic_create_termination_tunnel1 4\n", proc_name);
        return rv;
    }
    /** Add ILM tunnel switch */
    rv = mpls_encapsulation_basic_create_ilm_switch_tunnel_create(unit, cint_vxlan_roo_basic_info.swap_before_label,
                                                                  cint_vxlan_roo_basic_info.mpls_fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, mpls_encapsulation_basic_create_ilm_switch_tunnel_create 4\n", proc_name);
        return rv;
    }
    /** Add IP-Tunnel termination */
    rv = tunnel_terminator_create(unit, cint_vxlan_roo_basic_info.dip_term_tunnel, 0xFFFF0000,
                                  cint_vxlan_roo_basic_info.sip_term_tunnel, 0xFFFFF000,
                                  cint_vxlan_roo_basic_info.core_overlay_eth_vrf,
                                  cint_vxlan_roo_basic_info.tunnel_termination_vrf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, tunnel_terminator_create\n", proc_name);
        return rv;
    }

    return rv;
}
int
vxlan_roo_basic_eve_swap(
    int unit,
    int port)
{
    int action_id_1;
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t action;
    bcm_port_tpid_class_t port_tpid_class;
    bcm_vlan_translate_action_class_t action_class;
    /*
     * set tag format for untagged packets     */
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = port;
    port_tpid_class.tpid1 = cint_vxlan_roo_basic_info.outer_tpid;
    port_tpid_class.tpid2 = cint_vxlan_roo_basic_info.inner_tpid;
    port_tpid_class.tag_format_class_id = cint_vxlan_roo_basic_info.tag_format;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY;
    port_tpid_class.vlan_translation_action_id = 0;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_tpid_class_set, port=%d, \n", port);
        return rv;
    }

    /*
     * Create action ID     */
    rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_EGRESS, &action_id_1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_id_create\n");
        return rv;
    }

    /*
     * Set translation action     */
    bcm_vlan_action_set_t_init(&action);
    action.outer_tpid = cint_vxlan_roo_basic_info.outer_tpid;
    action.inner_tpid = cint_vxlan_roo_basic_info.inner_tpid;
    action.dt_outer = bcmVlanActionDelete;
    action.dt_inner = bcmVlanActionDelete;
    rv = bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_EGRESS, action_id_1, &action);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_id_set\n");
        return rv;
    }

    /*
     * Set translation action class (map edit_profile & tag_format to action_id)     */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = cint_vxlan_roo_basic_info.egress_vlan_edit_profile;
    action_class.tag_format_class_id = cint_vxlan_roo_basic_info.tag_format;
    action_class.vlan_translation_action_id = action_id_1;
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_translate_action_class_set(unit, &action_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }
    return rv;
}
int
vxlan_roo_basic_l3_forwarding(
    int unit)
{
    int rv = BCM_E_NONE;
    sand_utils_l3_host_ipv4_s host_ipv4_structure;
    sand_utils_l3_host_ipv6_s host_ipv6_structure;
    sand_utils_l3_route_ipv4_s route_ipv4_structure;
    sand_utils_l3_route_ipv6_s route_ipv6_structure;
    sand_utils_l3_vpbr_ipv4_s vpbr_ipv4_structure;
    /*
     * Add Host entry for access to core traffic     */

    sand_utils_l3_host_ipv4_s_common_init(unit, 0, &host_ipv4_structure, cint_vxlan_roo_basic_info.route_dip_into_overlay, cint_vxlan_roo_basic_info.access_eth_vrf, 0, 0, cint_vxlan_roo_basic_info.core_native_fec_id1);
    rv = sand_utils_l3_host_ipv4_add(unit, &host_ipv4_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in add_host_ipv41, \n");
        return rv;
    }
    /*
     * Add Host entry for core to access traffic     */
    sand_utils_l3_host_ipv4_s_common_init(unit, 0, &host_ipv4_structure, cint_vxlan_roo_basic_info.route_dip_out_of_overlay, cint_vxlan_roo_basic_info.tunnel_termination_vrf, 0, 0, cint_vxlan_roo_basic_info.access_fec_id1);
    rv = sand_utils_l3_host_ipv4_add(unit, &host_ipv4_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in add_host_ipv42, \n");
        return rv;
    }

    /*
     * Add Host entry for core to access traffic     */
    sand_utils_l3_host_ipv4_s_common_init(unit, 0, &host_ipv4_structure, cint_vxlan_roo_basic_info.route_dip_out_of_overlay+1, cint_vxlan_roo_basic_info.tunnel_termination_vrf, 0, 0, cint_vxlan_roo_basic_info.access_fec_id2);
    rv = sand_utils_l3_host_ipv4_add(unit, &host_ipv4_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in add_host_ipv43, \n");
        return rv;
    }

    /** Add LPM Public route entry for core to access traffic */
    sand_utils_l3_route_ipv4_s_common_init(unit, 0, &route_ipv4_structure, cint_vxlan_roo_basic_info.public_dip_ipv4, cint_vxlan_roo_basic_info.public_dip_ipv4_mask, 0, 0, 0, cint_vxlan_roo_basic_info.access_fec_id_public);
    rv = sand_utils_l3_route_ipv4_add(unit, &route_ipv4_structure);
    if (rv != BCM_E_NONE)
    {
       printf("Error, in add_host_ipv43, \n");
       return rv;
    }
    /*
     * Add Route entry
     */
    sand_utils_l3_route_ipv4_s_common_init(unit, 0, &route_ipv4_structure, cint_vxlan_roo_basic_info.route_dip_into_mpls, cint_vxlan_roo_basic_info.mask_dip_into_mpls,
            cint_vxlan_roo_basic_info.tunnel_termination_vrf, 0, 0, cint_vxlan_roo_basic_info.access_fec_id1);
    rv = sand_utils_l3_route_ipv4_add(unit, &route_ipv4_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function sand_utils_l3_route_ipv4_add, \n");
        return rv;
    }

    /*
     * Add Host entry
     */
    sand_utils_l3_route_ipv6_s_common_init(unit, 0, &route_ipv6_structure, cint_vxlan_roo_basic_info.routev6_dip_out_of_overlay, cint_vxlan_roo_basic_info.maskv6_dip_out_of_overlay,
            cint_vxlan_roo_basic_info.tunnel_termination_vrf, 0, 0, cint_vxlan_roo_basic_info.access_fec_id1);
    rv = sand_utils_l3_route_ipv6_add(unit, &route_ipv6_structure);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function sand_utils_l3_route_ipv6_add, err_id = %d \n", proc_name, rv);
        return rv;
    }

    /*
     * Add Host entry
     */
    sand_utils_l3_host_ipv6_s_common_init(unit, 0, &host_ipv6_structure, cint_vxlan_roo_basic_info.hostv6_dip_out_of_overlay,
            cint_vxlan_roo_basic_info.tunnel_termination_vrf, 0, 0, cint_vxlan_roo_basic_info.access_fec_id1);
    rv = sand_utils_l3_host_ipv6_add(unit, &host_ipv6_structure);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function sand_utils_l3_host_ipv6_add, err_id = %d \n", proc_name, rv);
        return rv;
    }

    /*
     * Add Host entry for routing into Ipv6 tunnel
     */
    sand_utils_l3_host_ipv6_s_common_init(unit, 0, &host_ipv6_structure, cint_vxlan_roo_basic_info.dip_to_tunnel,
            cint_vxlan_roo_basic_info.tunnel_termination_vrf, 0, 0, cint_vxlan_roo_basic_info.ipv6_provider_fec);
    rv = sand_utils_l3_host_ipv6_add(unit, &host_ipv6_structure);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function sand_utils_l3_host_ipv6_add, err_id = %d \n", proc_name, rv);
        return rv;
    }
    /*
     *
     * Add pbr entry
     */
    sand_utils_l3_vpbr_ipv4_s_common_init(unit, 0, &vpbr_ipv4_structure, cint_vxlan_roo_basic_info.host_dip_out_of_overlay, 0xFFFFFFFF,
            cint_vxlan_roo_basic_info.host_sip_out_of_overlay, 0xFFFFFFFF, cint_vxlan_roo_basic_info.tunnel_termination_vrf, 0, cint_vxlan_roo_basic_info.vpbr_vrf);
    rv = sand_utils_l3_vpbr_ipv4_add(unit, &vpbr_ipv4_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in sand_utils_l3_vpbr_ipv4_add, \n");
        return rv;
    }
    /*
     * Add vpbr Host
     */
    sand_utils_l3_host_ipv4_s_common_init(unit, 0, &host_ipv4_structure, cint_vxlan_roo_basic_info.host_dip_out_of_overlay, cint_vxlan_roo_basic_info.vpbr_vrf, 0, 0, cint_vxlan_roo_basic_info.access_fec_id1);
    rv = sand_utils_l3_host_ipv4_add(unit, &host_ipv4_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in add_host_ipv42, \n");
        return rv;
    }

    /*
     * Add Host entry
     */
    sand_utils_l3_host_ipv4_s_common_init(unit, 0, &host_ipv4_structure, cint_vxlan_roo_basic_info.host_dip_tunnel_out_of_overlay, cint_vxlan_roo_basic_info.tunnel_termination_vrf, 0, 0, cint_vxlan_roo_basic_info.mpls2_fec_id);
    rv = sand_utils_l3_host_ipv4_add(unit, &host_ipv4_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in add_host_ipv43, \n");
        return rv;
    }

    /*
     * Add Host entry
     */
    sand_utils_l3_host_ipv4_s_common_init(unit, 0, &host_ipv4_structure, cint_vxlan_roo_basic_info.host_dip_into_mpls_stack, cint_vxlan_roo_basic_info.tunnel_termination_vrf, 0, 0, cint_vxlan_roo_basic_info.mpls3_fec_id);
    rv = sand_utils_l3_host_ipv4_add(unit, &host_ipv4_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in host_dip_into_mpls_stack, \n");
        return rv;
    }
    /*
     * Add Host entry for routing into MPLS stack
     */
    sand_utils_l3_host_ipv6_s_common_init(unit, 0, &host_ipv6_structure, cint_vxlan_roo_basic_info.dip_to_mpls,
            cint_vxlan_roo_basic_info.tunnel_termination_vrf, 0, 0, cint_vxlan_roo_basic_info.mpls3_fec_id);
    rv = sand_utils_l3_host_ipv6_add(unit, &host_ipv6_structure);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function sand_utils_l3_host_ipv6_add, err_id = %d \n", proc_name, rv);
        return rv;
    }

    /*
     * Add Route entry for Public LPM lookup
     */
    sand_utils_l3_route_ipv6_s_common_init(unit, 0, &route_ipv6_structure, cint_vxlan_roo_basic_info.public_dip, cint_vxlan_roo_basic_info.public_dip_mask,
            0, 0, 0, cint_vxlan_roo_basic_info.access_fec_id_public);
    rv = sand_utils_l3_route_ipv6_add(unit, &route_ipv6_structure);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function sand_utils_l3_route_ipv6_add, err_id = %d \n", proc_name, rv);
        return rv;
    }
    return rv;
}
int
vxlan_roo_basic_create_fec_entries(
    int unit)
{
    int rv;
    int temp_out_rif;
    int flags = 0;
    bcm_gport_t gport;
    sand_utils_l3_fec_s fec_structure;

    /*
     * Create native FEC for routing into overlay:
     * 1) Give a fec id.    
     * 2) Native out rif.
     * 3) Native arp id. 
     *  4) Point to overlay fec.
     */
    BCM_GPORT_FORWARD_PORT_SET(gport, cint_vxlan_roo_basic_info.core_overlay_fec_id1);
    sand_utils_l3_fec_s_common_init(unit, BCM_L3_WITH_ID, 0, &fec_structure, cint_vxlan_roo_basic_info.core_native_fec_id1, flags, 0, gport, cint_vxlan_roo_basic_info.core_native_eth_rif, cint_vxlan_roo_basic_info.core_native_arp_id);
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error in sand_utils_l3_fec\n");
        return rv;
    }

    BCM_GPORT_FORWARD_PORT_SET(gport, cint_vxlan_roo_basic_info.core_overlay_fec_id2);
    sand_utils_l3_fec_s_common_init(unit, BCM_L3_WITH_ID, 0, &fec_structure, cint_vxlan_roo_basic_info.core_native_fec_id2, flags, 0, gport, cint_vxlan_roo_basic_info.core_native_eth_rif, cint_vxlan_roo_basic_info.core_native_arp_id);
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error in sand_utils_l3_fec\n");
        return rv;
    }


    /*
     * Create overlay FEC for routing into overlay: 
     * 1) Give a fec id. 
     * 2) Tunnel id of the overlay VXLAN Tunnel. 
     * 3) Tunnel id of the overlay Eth.  
     * 4) Out port.
     */
    gport = 0;
    flags |= BCM_L3_CASCADED;
    BCM_GPORT_LOCAL_SET(gport, cint_vxlan_roo_basic_info.core_port1);
    sand_utils_l3_fec_s_common_init(unit, BCM_L3_WITH_ID, 0, &fec_structure, cint_vxlan_roo_basic_info.core_overlay_fec_id1, flags, 0, gport, cint_vxlan_roo_basic_info.core_overlay_encap_tunnel_intf,
            0);
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error in creating overlay FEC for routing into overlay\n");
        return rv;
    }

    gport = 0;
    flags |= BCM_L3_CASCADED;
    BCM_GPORT_LOCAL_SET(gport, cint_vxlan_roo_basic_info.core_port2);
    sand_utils_l3_fec_s_common_init(unit, BCM_L3_WITH_ID, 0, &fec_structure, cint_vxlan_roo_basic_info.core_overlay_fec_id2, flags, 0, gport, cint_vxlan_roo_basic_info.core_overlay_encap_tunnel_intf,
            0);
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error in creating overlay FEC for routing into overlay\n");
        return rv;
    }


    /*
     * Create FEC for routing out of overlay:      
     * 1) Give a fec id. 
     * 2) Access out rif.
     * 3) Access arp id.
     * 4) Out port
     */
    flags = 0;
    sand_utils_l3_fec_s_common_init(unit, BCM_L3_WITH_ID, 0, &fec_structure, cint_vxlan_roo_basic_info.access_fec_id1, flags, 0, cint_vxlan_roo_basic_info.access_port1, cint_vxlan_roo_basic_info.access_eth_rif,
            cint_vxlan_roo_basic_info.access_arp_id);
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error in creating FEC routing out of overlay\n");
        return rv;
    }
    flags = 0;
    sand_utils_l3_fec_s_common_init(unit, BCM_L3_WITH_ID, 0, &fec_structure, cint_vxlan_roo_basic_info.access_fec_id2, flags, 0, cint_vxlan_roo_basic_info.access_port2, cint_vxlan_roo_basic_info.access_eth_rif,
            cint_vxlan_roo_basic_info.access_arp_id);
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error in creating FEC routing out of overlay\n");
        return rv;
    }

    flags = 0;
    sand_utils_l3_fec_s_common_init(unit, BCM_L3_WITH_ID, 0, &fec_structure, cint_vxlan_roo_basic_info.access_fec_id_public, flags, 0, cint_vxlan_roo_basic_info.access_port2, cint_vxlan_roo_basic_info.access_eth_rif_public,
            cint_vxlan_roo_basic_info.access_arp_id_public);
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error in creating FEC routing out of overlay\n");
        return rv;
    }

    flags = 0;
    sand_utils_l3_fec_s_common_init(unit, BCM_L3_WITH_ID, 0, &fec_structure, cint_vxlan_roo_basic_info.ipv6_provider_fec, flags, 0, cint_vxlan_roo_basic_info.access_port2, cint_vxlan_roo_basic_info.encap_ipv6_tunnel_intf, 0);
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error in creating FEC routing out of overlay\n");
        return rv;
    }

    flags = 0;
    sand_utils_l3_fec_s_common_init(unit, BCM_L3_WITH_ID, 0, &fec_structure, cint_vxlan_roo_basic_info.mpls_fec_id, flags, 0, cint_vxlan_roo_basic_info.access_port1, cint_vxlan_roo_basic_info.push_tunnel_id, 0);
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error in creating FEC mpls\n");
        return rv;
    }

    flags = 0;
    sand_utils_l3_fec_s_common_init(unit, BCM_L3_WITH_ID, 0, &fec_structure, cint_vxlan_roo_basic_info.mpls2_fec_id, flags, 0, cint_vxlan_roo_basic_info.access_port1, cint_vxlan_roo_basic_info.push_tunnel3_id, 0);
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error in creating FEC ip\n");
        return rv;
    }

    flags = 0;
    sand_utils_l3_fec_s_common_init(unit, BCM_L3_WITH_ID, 0, &fec_structure, cint_vxlan_roo_basic_info.mpls3_fec_id, flags, 0, cint_vxlan_roo_basic_info.access_port1, cint_vxlan_roo_basic_info.push_stack_tunnel_id, 0);
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error in creating FEC ip\n");
        return rv;
    }
    return rv;
}
int
vxlan_roo_basic_create_arp_entries(
    int unit)
{
    int rv;
    sand_utils_l3_arp_s arp_structure;

    /** Configure native ARP entry */
    sand_utils_l3_arp_s_common_init(unit, 0, &arp_structure, cint_vxlan_roo_basic_info.core_native_arp_id, BCM_L3_NATIVE_ENCAP, BCM_L3_FLAGS2_VLAN_TRANSLATION, cint_vxlan_roo_basic_info.core_native_next_hop_mac, cint_vxlan_roo_basic_info.core_native_eth_rif);
    rv = sand_utils_l3_arp_create(unit, &arp_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create AC egress object ARP only\n");
        return rv;
    }
    cint_vxlan_roo_basic_info.core_native_arp_id = arp_structure.l3eg_arp_id;

    /** Configure overlay ARP entry  */
    sand_utils_l3_arp_s_common_init(unit, 0, &arp_structure, 0, 0, cint_vxlan_roo_basic_info.core_overlay_arp_id, cint_vxlan_roo_basic_info.core_overlay_next_hop_mac, cint_vxlan_roo_basic_info.core_overlay_eth_rif);
    rv = sand_utils_l3_arp_create(unit, &arp_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create AC egress object ARP only\n");
        return rv;
    }
    cint_vxlan_roo_basic_info.core_overlay_arp_id = arp_structure.l3eg_arp_id;
    /** Configure access ARP entry */
    sand_utils_l3_arp_s_common_init(unit, 0, &arp_structure, 0, 0, cint_vxlan_roo_basic_info.access_arp_id, cint_vxlan_roo_basic_info.access_next_hop_mac, cint_vxlan_roo_basic_info.access_eth_rif);
    rv = sand_utils_l3_arp_create(unit, &arp_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create AC egress object ARP only\n");
        return rv;
    }
    cint_vxlan_roo_basic_info.access_arp_id = arp_structure.l3eg_arp_id;
    /** Configure access ARP entry */
    sand_utils_l3_arp_s_common_init(unit, 0, &arp_structure, 0, 0, cint_vxlan_roo_basic_info.access_arp_id_public, cint_vxlan_roo_basic_info.access_next_hop_mac_public, cint_vxlan_roo_basic_info.access_eth_rif_public);
    rv = sand_utils_l3_arp_create(unit, &arp_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create AC egress object ARP only\n");
        return rv;
    }
    cint_vxlan_roo_basic_info.access_arp_id_public = arp_structure.l3eg_arp_id;

    sand_utils_l3_arp_s_common_init(unit, 0, &arp_structure, cint_vxlan_roo_basic_info.mpls_stack_arp_id, 0, BCM_L3_FLAGS2_VLAN_TRANSLATION, cint_vxlan_roo_basic_info.core_overlay_next_hop_mac, cint_vxlan_roo_basic_info.core_overlay_eth_rif);
    rv = sand_utils_l3_arp_create(unit, &arp_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create AC egress object ARP2 only\n");
    }
    cint_vxlan_roo_basic_info.mpls_stack_arp_id = arp_structure.l3eg_arp_id;

    return rv;
}
int
vxlan_roo_basic_configure_port_properties(
    int unit,
    int access_port1,
    int access_port2,
    int core_port1,
    int core_port2)
{
    int rv = BCM_E_NONE;
    sand_utils_l3_port_s l3_port_structure;
    bcm_gport_t ports_vlan[2];
    uint8 tagged_vlan[2];

    sand_utils_l3_port_s_common_init(unit, 0, &l3_port_structure, cint_vxlan_roo_basic_info.access_port1, cint_vxlan_roo_basic_info.access_eth_rif);
    rv = sand_utils_l3_port_set(unit, &l3_port_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, access_port sand_utils_l3_port_set\n");
        return rv;
    }

    sand_utils_l3_port_s_common_init(unit, 0, &l3_port_structure, cint_vxlan_roo_basic_info.access_port2, cint_vxlan_roo_basic_info.access_eth_rif);
    rv = sand_utils_l3_port_set(unit, &l3_port_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, access_port sand_utils_l3_port_set\n");
        return rv;
    }

    ports_vlan[0] = cint_vxlan_roo_basic_info.access_port1;
    ports_vlan[1] = cint_vxlan_roo_basic_info.access_port2;
    tagged_vlan[0] = 1;
    tagged_vlan[1] = 1;
    rv = vlan__open_vlan_per_ports(unit, cint_vxlan_roo_basic_info.access_eth_rif, ports_vlan, tagged_vlan, 2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, access_port vlan__open_vlan_per_ports\n");
        return rv;
    }

    sand_utils_l3_port_s_common_init(unit, 0, &l3_port_structure, cint_vxlan_roo_basic_info.core_port1, cint_vxlan_roo_basic_info.core_overlay_eth_rif);
    rv = sand_utils_l3_port_set(unit, &l3_port_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, core_port sand_utils_l3_port_set\n");
        return rv;
    }

    sand_utils_l3_port_s_common_init(unit, 0, &l3_port_structure, cint_vxlan_roo_basic_info.core_port2, cint_vxlan_roo_basic_info.core_overlay_eth_rif);
    rv = sand_utils_l3_port_set(unit, &l3_port_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, core_port sand_utils_l3_port_set\n");
        return rv;
    }

    ports_vlan[0] = cint_vxlan_roo_basic_info.core_port1;
    ports_vlan[1] = cint_vxlan_roo_basic_info.core_port2;
    tagged_vlan[0] = 1;
    tagged_vlan[1] = 1;
    rv = vlan__open_vlan_per_ports(unit, cint_vxlan_roo_basic_info.core_overlay_eth_rif, ports_vlan, tagged_vlan, 2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, core_port vlan__open_vlan_per_ports\n");
        return rv;
    }

    return rv;
}
int
vxlan_roo_basic_create_l3_interfaces(
    int unit)
{
    bcm_l3_ingress_t ingress_l3_interface;
    bcm_l3_ingress_t_init(&ingress_l3_interface);

    int rv;
    sand_utils_l3_eth_rif_s eth_rif_structure;

    sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_vxlan_roo_basic_info.core_native_eth_rif, 0, 0, cint_vxlan_roo_basic_info.core_native_eth_rif_mac, cint_vxlan_roo_basic_info.tunnel_termination_vrf);
    rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, sand_utils_l3_eth_rif_create core_native_eth_rif\n");
        return rv;
    }

    sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_vxlan_roo_basic_info.core_overlay_eth_rif, 0, 0, cint_vxlan_roo_basic_info.core_overlay_eth_rif_mac, cint_vxlan_roo_basic_info.core_overlay_eth_vrf);
    rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, sand_utils_l3_eth_rif_create core_overlay_eth_rif\n");
        return rv;
    }

    sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_vxlan_roo_basic_info.access_eth_rif, 0, 0, cint_vxlan_roo_basic_info.access_eth_rif_mac, cint_vxlan_roo_basic_info.access_eth_vrf);
    rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, sand_utils_l3_eth_rif_create access_eth_rif\n");
        return rv;
    }
    return rv;

}
int
mact_entry_add(
    int unit,
    bcm_mac_t mac,
    int vsi,
    int gport)
{
    int rv;
    bcm_l2_addr_t l2addr;
    bcm_l2_addr_t_init(&l2addr, mac, vsi);
    l2addr.port = gport;
    l2addr.flags = BCM_L2_STATIC;
    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_addr_add \n");
        return rv;
    }
    return rv;
}
int
vxlan_port_add(
    int unit,
    int core_port,
    int tunnel_term_id,
    int tunnel_encapsulation_id,
    bcm_gport_t * vxlan_port_id)
{
    int rv;
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t l3_intf;
    vxlan_port_add_s vxlan_port_add;
    vxlan_port_s_clear(&vxlan_port_add);
    vxlan_port_add.vpn = cint_vxlan_roo_basic_info.vpn_id;
    vxlan_port_add.in_port = core_port;
    vxlan_port_add.in_tunnel = tunnel_term_id;
    vxlan_port_add.out_tunnel = tunnel_encapsulation_id;
    BCM_L3_ITF_SET(vxlan_port_add.egress_if, BCM_L3_ITF_TYPE_FEC, cint_vxlan_roo_basic_info.core_overlay_fec_id1);
    vxlan_port_add.flags = 0;
    vxlan_port_add.network_group_id = cint_vxlan_roo_basic_info.vxlan_network_group_id;
    vxlan_port_add.out_tunnel_if = cint_vxlan_roo_basic_info.core_overlay_encap_tunnel_intf;
    
    rv = vxlan__vxlan_port_add(unit, vxlan_port_add);
    *vxlan_port_id = vxlan_port_add.vxlan_port_id;
    return BCM_E_NONE;
}
int
vxlan_tunnel_initiator_create(
    int unit,
    bcm_if_t arp_itf,
    bcm_if_t * tunnel_intf,
    int *encap_tunnel_id)
{
    int rv;
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t l3_intf;
    bcm_l3_intf_t_init(&l3_intf);

    /*
     * Create IP tunnel initiator for encapsulating Vxlan tunnel header
     */
    bcm_tunnel_initiator_t_init(&tunnel_init);
    tunnel_init.dip = cint_vxlan_roo_basic_info.tunnel_dip;     /* default: 171.0.0.1 */
    tunnel_init.sip = cint_vxlan_roo_basic_info.tunnel_sip;     /* default: 172.0.0.1 */
    tunnel_init.flags = 0;
    tunnel_init.dscp = 10;      /* default: 10 */
    tunnel_init.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    tunnel_init.type = bcmTunnelTypeVxlan;
    tunnel_init.l3_intf_id = arp_itf;
    tunnel_init.ttl = 64;       /* default: 64 */
    tunnel_init.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE;   /* default: -1 */
    tunnel_init.encap_access = bcmEncapAccessTunnel4;
    rv = bcm_tunnel_initiator_create(unit, &l3_intf, tunnel_init);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_initiator_create \n");
        return rv;
    }
    *tunnel_intf = l3_intf.l3a_intf_id;
    *encap_tunnel_id = tunnel_init.tunnel_id;
    return rv;
}


int ipv6_tunnel_initiator_create(
    int unit,
    bcm_if_t arp_id,
    bcm_if_t *tunnel_inlif)
{
    int rv;
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t l3_intf;

    bcm_l3_intf_t_init(&l3_intf);

    /* Create IPv6 tunnel initiator for encapsulating GRE4oIPv6 tunnel header*/
    bcm_tunnel_initiator_t_init(&tunnel_init);
    tunnel_init.dip6            = cint_vxlan_roo_basic_info.encap_tunnel_ipv6_dip;
    tunnel_init.sip6            = cint_vxlan_roo_basic_info.encap_tunnel_ipv6_sip;
    tunnel_init.flags           = 0;
    tunnel_init.dscp            = 8;
    tunnel_init.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    tunnel_init.type            = bcmTunnelTypeIpAnyIn6;
    tunnel_init.ttl             = 128;
    tunnel_init.encap_access    = bcmEncapAccessTunnel2;
    tunnel_init.l3_intf_id      = arp_id;

    tunnel_init.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE; /* default: -1 */
    rv = bcm_tunnel_initiator_create(unit,&l3_intf, tunnel_init);

    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_initiator_create \n");
    }

    *tunnel_inlif = l3_intf.l3a_intf_id;
    return rv;
}
/* 
 * Main function for configuring the VXLAN VPN
 */
int
vxlan_roo_basic_vxlan_open_vpn(
    int unit,
    int vpn_id,
    int vni)
{
    int rv = BCM_E_NONE;
    char *proc_name;
    proc_name = "vxlan_roo_basic_vxlan_open_vpn";
    bcm_vxlan_vpn_config_t vpn_config;
    bcm_vxlan_vpn_config_t_init(&vpn_config);
    vpn_config.flags = BCM_VXLAN_VPN_WITH_ID | BCM_VXLAN_VPN_WITH_VPNID;
    vpn_config.vpn = vpn_id;
    vpn_config.broadcast_group = vpn_id;
    vpn_config.unknown_unicast_group = vpn_id;
    vpn_config.unknown_multicast_group = vpn_id;
    vpn_config.vnid = vni;
    rv = bcm_vxlan_vpn_create(unit, &vpn_config);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error in bcm_vxlan_vpn_create \n", proc_name);
        return rv;
    }
    return rv;
}
int
vxlan_tunnel_terminator_create(
    int unit,
    int *term_tunnel_id)
{
    int rv;
    bcm_tunnel_terminator_t tunnel_term;

    /*
     * Create IP tunnel terminator for SIP,DIP, VRF lookup 
     */
    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.dip = cint_vxlan_roo_basic_info.tunnel_dip;
    tunnel_term.dip_mask = cint_vxlan_roo_basic_info.tunnel_dip_mask;
    tunnel_term.sip = cint_vxlan_roo_basic_info.tunnel_sip;
    tunnel_term.sip_mask = cint_vxlan_roo_basic_info.tunnel_sip_mask;
    tunnel_term.vrf = cint_vxlan_roo_basic_info.core_overlay_eth_vrf;
    BCM_GPORT_TUNNEL_ID_SET(tunnel_term.tunnel_id, 0x3000);
    tunnel_term.flags = BCM_TUNNEL_TERM_TUNNEL_WITH_ID;
    tunnel_term.type = bcmTunnelTypeVxlan;
    rv = bcm_tunnel_terminator_create(unit, &tunnel_term);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_terminator_create \n");
        return rv;
    }
    *term_tunnel_id = tunnel_term.tunnel_id;
    return rv;
}

int
vxlan_ipv6_tunnel_terminator_create(
    int unit,
    int *term_tunnel_id)
{
    int rv;
    bcm_tunnel_terminator_t tunnel_term;

    /*
     * Create vxlan6 tunnel terminator
     */
    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.dip6 = cint_vxlan_roo_basic_info.vxlan_tunnel_ipv6_dip;
    tunnel_term.dip6_mask = cint_vxlan_roo_basic_info.tunnel_ipv6_dip_mask;
    tunnel_term.sip6 = cint_vxlan_roo_basic_info.vxlan_tunnel_ipv6_sip;
    tunnel_term.sip6_mask = cint_vxlan_roo_basic_info.tunnel_ipv6_sip_mask;
    tunnel_term.vrf = cint_vxlan_roo_basic_info.core_overlay_eth_vrf;

    tunnel_term.type = bcmTunnelTypeVxlan6;
    rv = bcm_tunnel_terminator_create(unit, &tunnel_term);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_terminator_create \n");
        return rv;
    }

    *term_tunnel_id = tunnel_term.tunnel_id;

    return rv;
}


int ipv6_gre_tunnel_terminator_create(
        int unit)
{

    int rv;
    bcm_tunnel_terminator_t tunnel_termination_obj;

    /* Create IP tunnel terminator for SIP,DIP, VRF lookup */
    bcm_tunnel_terminator_t_init(&tunnel_termination_obj);
    tunnel_termination_obj.dip6 = cint_vxlan_roo_basic_info.tunnel_ipv6_dip;
    tunnel_termination_obj.dip6_mask = cint_vxlan_roo_basic_info.tunnel_ipv6_dip_mask;
    tunnel_termination_obj.sip6 = cint_vxlan_roo_basic_info.tunnel_ipv6_sip;
    tunnel_termination_obj.sip6_mask = cint_vxlan_roo_basic_info.tunnel_ipv6_sip_mask;
    tunnel_termination_obj.ingress_qos_model.ingress_phb = bcmQosIngressModelShortpipe;
    tunnel_termination_obj.ingress_qos_model.ingress_remark = bcmQosIngressModelShortpipe;
    tunnel_termination_obj.ingress_qos_model.ingress_ttl = bcmQosIngressModelShortpipe;
    tunnel_termination_obj.vrf = cint_vxlan_roo_basic_info.core_overlay_eth_vrf;
    tunnel_termination_obj.type = bcmTunnelTypeGreAnyIn6;

    rv = bcm_tunnel_terminator_create(unit, &tunnel_termination_obj);
    if (rv != BCM_E_NONE)
    {
        printf(
                "Error: ipv6_tunnel_terminator_create failed to create tunnel termination object -> bcm_tunnel_terminator_create() failed. \n");
        return rv;
    }

    /** In JR2 VRF is a tunnel-LIF property set by bcm_l3_ingress_create API */
    bcm_l3_ingress_t ingress_in_lif;
    bcm_l3_ingress_t_init(&ingress_in_lif);
    bcm_if_t in_lif_id;

    ingress_in_lif.flags = BCM_L3_INGRESS_WITH_ID; /* must, as we update exist RIF */
    ingress_in_lif.vrf = cint_vxlan_roo_basic_info.tunnel_termination_vrf;

    /* Convert tunnel's GPort ID to intf ID */
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(in_lif_id, tunnel_termination_obj.tunnel_id);

    rv = bcm_l3_ingress_create(unit, ingress_in_lif, in_lif_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error: ipv6_tunnel_terminator_create failed -> bcm_l3_ingress_create() failed.\n");
        return rv;
    }
    return rv;
}

int ipv6_tunnel_terminator_create(
        int unit)
{

    int rv;
    bcm_tunnel_terminator_t tunnel_termination_obj;

    /* Create IP tunnel terminator for SIP,DIP, VRF lookup */
    bcm_tunnel_terminator_t_init(&tunnel_termination_obj);
    tunnel_termination_obj.dip = cint_vxlan_roo_basic_info.tunnel_dip;
    tunnel_termination_obj.dip_mask = cint_vxlan_roo_basic_info.tunnel_dip_mask;
    tunnel_termination_obj.sip = cint_vxlan_roo_basic_info.tunnel_sip;
    tunnel_termination_obj.sip_mask = cint_vxlan_roo_basic_info.tunnel_sip_mask;
    tunnel_termination_obj.flags = 0;
    tunnel_termination_obj.vrf = cint_vxlan_roo_basic_info.core_overlay_eth_vrf;
    tunnel_termination_obj.type = bcmTunnelTypeIpAnyIn4;

    rv = bcm_tunnel_terminator_create(unit, &tunnel_termination_obj);
    if (rv != BCM_E_NONE)
    {
        printf(
                "Error: ipv6_tunnel_terminator_create failed to create tunnel termination object -> bcm_tunnel_terminator_create() failed. \n");
        return rv;
    }

    /** In JR2 VRF is a tunnel-LIF property set by bcm_l3_ingress_create API */
    bcm_l3_ingress_t ingress_in_lif;
    bcm_l3_ingress_t_init(&ingress_in_lif);
    bcm_if_t in_lif_id;

    ingress_in_lif.flags = BCM_L3_INGRESS_WITH_ID | BCM_L3_INGRESS_GLOBAL_ROUTE; /* must, as we update exist RIF */
    ingress_in_lif.vrf = 0 ;

    /* Convert tunnel's GPort ID to intf ID */
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(in_lif_id, tunnel_termination_obj.tunnel_id);

    rv = bcm_l3_ingress_create(unit, ingress_in_lif, in_lif_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error: ipv6_tunnel_terminator_create failed -> bcm_l3_ingress_create() failed.\n");
        return rv;
    }
    return rv;
}
int
vlan_port_add(
    int unit,
    bcm_gport_t in_port,
    bcm_vlan_t vid,
    bcm_gport_t * port_id)
{
    int rv;
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);

    /*
     * the match criteria is port:1, out-vlan:510 
     */
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    vlan_port.port = in_port;
    vlan_port.match_vlan = vid;
    vlan_port.flags = 0;
    vlan_port.vsi = 0;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }

    /*
     * handle of the created gport 
     */
    *port_id = vlan_port.vlan_port_id;
    return rv;
}
int
vswitch_add_port(
    int unit,
    bcm_vlan_t vsi,
    bcm_gport_t gport)
{
    int rv;

    /*
     * add to vswitch 
     */
    rv = bcm_vswitch_port_add(unit, vsi, gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }
    return rv;
}
int
mpls_termination_basic_create_termination_tunnel(
    int unit,
    bcm_mpls_label_t terminated_label,
    bcm_gport_t * tunnel_id)
{
    int rv;
    bcm_mpls_tunnel_switch_t entry;
    bcm_if_t l3_intf_id;
    bcm_mpls_tunnel_switch_t_init(&entry);
    BCM_GPORT_TUNNEL_ID_SET(entry.tunnel_id, *tunnel_id);
    entry.flags = BCM_MPLS_SWITCH_WITH_ID;
    entry.ingress_qos_model.ingress_phb = bcmQosIngressModelShortpipe;
    entry.ingress_qos_model.ingress_remark = bcmQosIngressModelShortpipe;
    entry.ingress_qos_model.ingress_ttl = bcmQosIngressModelShortpipe;
    entry.action = BCM_MPLS_SWITCH_ACTION_POP;

    /*
     * incoming label
     */
    entry.label = terminated_label;
    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }
    *tunnel_id = entry.tunnel_id;
    print entry.tunnel_id;

    /*
     * Set Incoming Tunnel-RIF properties
     */
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(l3_intf_id, entry.tunnel_id);
    printf("Right before update mpls entry with VRF \n");

    bcm_l3_ingress_t l3_ing_if;
    bcm_l3_ingress_t_init(&l3_ing_if);
    l3_ing_if.flags = BCM_L3_INGRESS_WITH_ID;      /* must, as we update exist RIF */
    l3_ing_if.vrf = cint_vxlan_roo_basic_info.tunnel_termination_vrf;
    rv = bcm_l3_ingress_create(unit, l3_ing_if, l3_intf_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l3_ingress_create\n");
        return rv;
    }
    return rv;
}
int
mpls_encapsulation_basic_create_ilm_switch_tunnel_create(
    int unit,
    bcm_mpls_label_t label,
    int fec_id)
{
    int rv;
    bcm_mpls_tunnel_switch_t entry;

    /*
     * No flag is needed to indicate the action, since the information for the swap/php action is included in the EEDB,
     * pointed from the ILM.
     */
    bcm_mpls_tunnel_switch_t_init(&entry);

    /** incoming label */
    entry.label = label;

    entry.action = BCM_MPLS_SWITCH_ACTION_NOP;
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;

    /** FEC entry pointing to port */
    BCM_GPORT_FORWARD_PORT_SET(entry.port, fec_id);

    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }
    return rv;
}
int
mpls_encapsulation_basic_create_push_or_swap_tunnel(
    int unit,
    bcm_mpls_label_t label,
    bcm_if_t next_if,
    bcm_encap_access_t encap_access,
    bcm_if_t * tunnel_id)
{
    bcm_mpls_egress_label_t label_array[1];
    int num_labels = 1;
    int rv;
    bcm_mpls_egress_label_t_init(&label_array[0]);

    /*
     * Pushed or Swapping label.
     * The label will be pushed for IP forwarding context and swapped for
     * MPLS forwarding context.
     */
    label_array[0].label = label;
    /*
     * Set the next pointer of this entry to be the arp. This configuration is new compared to Jericho, where the arp
     * pointer used to be connected to the EEDB entry via bcm_l3_egress_create (with egress flag indication).
     */
    label_array[0].l3_intf_id = next_if;

    /**
     * If ttl_value != 0, means this value should be taken from the
     * table. (TTL_SET)
     * In case of 0, thee value will be taken from pipe (TTL_COPY)
     */
    label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    label_array[0].egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    label_array[0].encap_access = encap_access;
    rv = bcm_mpls_tunnel_initiator_create(unit, 0, num_labels, label_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }
    *tunnel_id = label_array[0].tunnel_id;
    return rv;
}
int
mpls_encapsulation_deep_stack(
    int unit,
    bcm_if_t arp,
    bcm_if_t * tunnel_id)
{
    bcm_mpls_egress_label_t label_array[2];
    int num_labels = 1;
    int rv, i;
    bcm_mpls_egress_label_t_init(&label_array[0]);
    bcm_if_t next_pointer = arp;
    bcm_mpls_label_t entry_first[6] = {0x123, 0x234, 0x345, 0x456, 0x567, 0x321};
    bcm_mpls_label_t entry_second[6] = {0x432, 0x543, 0x654, 0x765, 0x999, 0x889};
    bcm_encap_access_t encap_access[6] = {bcmEncapAccessNativeArp, bcmEncapAccessTunnel1, bcmEncapAccessTunnel2,
            bcmEncapAccessTunnel3, bcmEncapAccessTunnel4, bcmEncapAccessArp};



    /*
     * Label to be pushed as part of the MPLS tunnel.
     */
    for (i = 5; i >= 0; i--)
    {
        label_array[0].label = entry_first[i];
        label_array[0].flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
        label_array[0].egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
        label_array[0].l3_intf_id = next_pointer;
        label_array[0].encap_access = encap_access[i];
        label_array[1].label = entry_second[i];
        label_array[1].flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        label_array[1].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
        label_array[1].egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
        label_array[1].l3_intf_id = next_pointer;
        label_array[1].encap_access = encap_access[i];
        if ((i&1)) {
            label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TANDEM;
            label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_TANDEM;
        }
        rv = bcm_mpls_tunnel_initiator_create(unit, 0, 2, label_array);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_mpls_tunnel_initiator_create i %d\n",i);
            return rv;
        }

        next_pointer = label_array[0].tunnel_id;
    }
    *tunnel_id = label_array[0].tunnel_id;
    return rv;
}

int
tunnel_terminator_create(
    int unit,
    bcm_ip_t dip,
    bcm_ip_t dip_mask,
    bcm_ip_t sip,
    bcm_ip_t sip_mask,
    int vrf_lookup,
    int new_vrf)
{
    int rv;
    bcm_tunnel_terminator_t tunnel_term;

    /*
     * Create IP tunnel terminator for SIP,DIP, VRF lookup 
     */
    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.dip = dip;
    tunnel_term.dip_mask = dip_mask;
    tunnel_term.sip = sip;
    tunnel_term.sip_mask = sip_mask;
    tunnel_term.flags = BCM_TUNNEL_TERM_TUNNEL_WITH_ID;
    tunnel_term.ingress_qos_model.ingress_phb = bcmQosIngressModelShortpipe;
    tunnel_term.ingress_qos_model.ingress_remark = bcmQosIngressModelShortpipe;
    tunnel_term.ingress_qos_model.ingress_ttl = bcmQosIngressModelShortpipe;
    BCM_GPORT_TUNNEL_ID_SET(tunnel_term.tunnel_id, 0x33333);
    tunnel_term.vrf = vrf_lookup;
    tunnel_term.type = bcmTunnelTypeGreAnyIn4;

    rv = bcm_tunnel_terminator_create(unit, &tunnel_term);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_terminator_create \n");
        return rv;
    }

    /** In JR2 VRF is a tunnel-LIF property set by bcm_l3_ingress_create API */
    bcm_l3_ingress_t ing_intf;
    bcm_l3_ingress_t_init(&ing_intf);
    bcm_if_t intf_id;
    ing_intf.flags = BCM_L3_INGRESS_WITH_ID;    /* must, as we update exist RIF */
    ing_intf.vrf = new_vrf;
    /*
     * Convert tunnel's GPort ID to intf ID 
     */
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(intf_id, tunnel_term.tunnel_id);
    rv = bcm_l3_ingress_create(unit, ing_intf, intf_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l3_ingress_create\n");
        return rv;
    }

    return rv;
}
