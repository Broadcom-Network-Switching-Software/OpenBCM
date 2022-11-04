/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 *File: cint_dnx_pppoe.c
 * Purpose: An example of PPPoE encapsulation and termination
 *
 * 1. Example of PPPoE MP encapsulation
 * 2. Example of PPPoE MP termination
 * 3. Example of PPPoE MP termination with session spoofing check
 * 4. Example of PPPoE MP termination with SIPv4 anti-spoofing
 * 5. Example of PPPoE MP encapsulation with QOS mapping
 * 6. Example of PPPoE MP termination with wide LIF(Unsupported)
 *
 * 1. Example of PPPoE MP encapsulation
 *
 * Set up sequence:
 * Test Scenario - start
     cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
     cint ../../../../src/examples/dnx/field/cint_field_utils.c
     cint ../../../../src/examples/sand/cint_ip_route_basic.c
     cint ../../../../src/examples/sand/cint_ipmc_route_basic.c
     cint ../../../../src/examples/sand/utility/cint_sand_utils_multicast.c
     cint ../../../../src/examples/dnx/ppp/cint_dnx_pppoe.c
     cint
     pppoe_example_encapsulation(0, 201, 203);
     exit;
 * Run traffic:
 *    DA=00:11:00:00:00:22 VID=0x64
     tx 1 psrc=203 data=0x001100000022000007000100810000640800450000350000000080002cc47fffff038fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *    1.1 NNI->UNI IPv4 host forward: IPv4oETH. DIP=0x8fffff02
     tx 1 psrc=203 data=0x001100000022000007000100810000640800450000350000000080002cc47fffff038fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *                         DA          SA          Vlan                                SIP     DIP
 *
 *    1.2 NNI->UNI IPv4 route forward: IPv4oETH. DIP=0x9fffff02
     tx 1 psrc=203 DATA=0x001100000022000007000100810000640800450000350000000080001cc47fffff039fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *                         DA          SA          Vlan                                SIP     DIP
 *
 *    1.3 NNI->UNI IPv6 route forward: IPv6oETH. DIP=0000:0000:0000:0000:1234:0000:0011:FF10
     tx 1 psrc=203 DATA=0x0011000000220000070001008100006486dd6000000000210680000000000000000000000000001100aa0000000000000000123400000011ff10000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *                         DA          SA          Vlan                       SIP                              DIP
 *
 *    1.4 NNI->UNI IPv4 MC forward: IPv4oETH. DA=01:00:5E:02:02:02 DIP=0xe0020202
     tx 1 psrc=203 DATA=0x01005e02020200000700010081000064080045000035000000008000e9c36fffff01e0020202000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *                         DA          SA          Vlan                                SIP     DIP
 *
 *    1.5 NNI->UNI IPv6 MC forward: IPv6oETH. DA=33:33:00:00:12:12 DIP=FFFF:0000:0000:0000:0000:0000:0000:1212
     tx 1 psrc=203 DATA=0x3333000012120000070001008100006486dd60000000002106800000000000000000123400000000ee10ffff0000000000000000000000001212000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *                         DA          SA          Vlan                       SIP                              DIP
 * Expected:
 *    PPPoPPPoEoETH is encapsulated
 *    PPP=0x0021 for IPv4,  PPP=0x0057 for IPv6
 *    PPPoE.session_id = 0x5555
 *
 *    1.1 IPv4oPPPoPPPoEoETH
 *    DA=00:00:00:00:CD:2D SA=00:11:00:00:00:11 Vid=0x11
 *    PPPoE.session_id = 0x5555 PPP=0x0021
 *    SIP=0x7FFFFF03 DIP=0x8FFFFF02
 *
 *    1.2 IPv4oPPPoPPPoEoETH
 *    DA=00:00:00:00:CD:2D SA=00:11:00:00:00:11 Vid=0x11
 *    PPPoE.session_id = 0x5555 PPP=0x0021
 *    SIP=0x7FFFFF03 DIP=0x9FFFFF02
 *
 *    1.3 IPv6oPPPoPPPoEoETH
 *    DA=00:00:00:00:CD:2D SA=00:11:00:00:00:11 Vid=0x11
 *    PPPoE.session_id = 0x5555 PPP=0x0057
 *    SIP=0x000000000000000000000000001100AA DIP=0x0000000000000000123400000011FF10
 *
 *    1.4 IPv4oPPPoPPPoEoETH
 *    DA=00:00:00:00:CD:2D SA=00:11:00:00:00:11 Vid=0x11
 *    PPPoE.session_id = 0x5555 PPP=0x0021
 *    SIP=0x6fffff01 DIP=0xe0020202
 *
 *    1.5 IPv6oPPPoPPPoEoETH
 *    DA=00:00:00:00:CD:2D SA=00:11:00:00:00:11 Vid=0x11
 *    PPPoE.session_id = 0x5555 PPP=0x0057
 *    SIP=0x0000000000000000123400000000EE10 DIP=0xFFFF0000000000000000000000001212
 * Test Scenario - end
 *
 * 2. Example of PPPoE MP termination
 *
 * Set up sequence:
 * Test Scenario - start
     cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
     cint ../../../../src/examples/dnx/field/cint_field_utils.c
     cint ../../../../src/examples/sand/cint_ip_route_basic.c
     cint ../../../../src/examples/sand/cint_ipmc_route_basic.c
     cint ../../../../src/examples/sand/utility/cint_sand_utils_multicast.c
     cint ../../../../src/examples/dnx/ppp/cint_dnx_pppoe.c
     cint
     pppoe_example_termination(0, 201, 203);
     exit;
 * Run traffic:
 *    DA=00:11:00:00:00:11 VID=0x11
 *    PPPoE.session_id = 0x5555
 *    PPP=0x0021 for IPv4,  PPP=0x0057 for IPv6
 *
 *    2.1 UNI->NNI IPv4 host forward: IPv4oPPPoPPPoEoETH. DIP=0x7fffff02
     tx 1  PtchSRCport=201 DATA=0x0011000000110000070001008100001188641100555500370021450000350000000080004cc66fffff017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *                                 DA          SA          Vlan        PPPoE       PPP                        SIP      DIP
 *
 *    2.2 UNI->NNI IPv4 route forward: IPv4oPPPoPPPoEoETH. DIP=0x6fffff02
     tx 1  PtchSRCport=201 DATA=0x0011000000110000070001008100001188641100555500370021454c00350000000080005c7a6fffff016fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *                                 DA          SA          Vlan        PPPoE       PPP                         SIP     DIP
 *
 *    2.3 UNI->NNI IPv6 route forward: IPv6oPPPoPPPoEoETH. DIP=0x0000000000000000123400000000FF10
     tx 1  PtchSRCport=201 DATA=0x00110000001100000700010081000011886411005555003700576140000000210680000000000000000000000000000000aa0000000000000000123400000000ff10000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *                                 DA          SA          Vlan        PPPoE       PPP                SIP                              DIP
 *
 *    2.4 UNI->NNI IPv4 MC forward: IPv4oPPPoPPPoEoETH. DIP=0xe0020202
     tx 1  PtchSRCport=201 DATA=0x01100000011000007000100810000118864110055550037002145540035000000008000e96f6fffff01e0020202000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *                                 DA         SA          Vlan        PPPoE       PPP                         SIP     DIP
 *
 *    2.5 UNI->NNI IPv6 MC forward: IPv6oPPPoPPPoEoETH. DIP=0xffff0000000000000000000000001212
     tx 1  PtchSRCport=201 DATA=0x001100000011000007000100810000118864110055550037005761700000002106800000000000000000123400000000ee10ffff0000000000000000000000001212000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *                                 DA          SA          Vlan        PPPoE       PPP                SIP                              DIP
 * Expected:
 *    PPPoPPPoEoETH is terminated
 *    2.1 IPv4oE
 *    DA=00:00:00:00:CD:1D SA=00:11:00:00:00:22 Vid=0x64 SIP=0x6fffff01 DIP=0x7fffff02
 *    2.2 IPv4oE
 *    DA=00:00:00:00:CD:1D SA=00:11:00:00:00:22 Vid=0x64 SIP=0x6fffff01 DIP=0x6fffff02
 *    2.3 IPv6oE
 *    DA=00:00:00:00:CD:1D SA=00:11:00:00:00:22 Vid=0x64 SIP=0x000000000000000000000000000000AA DIP=0x0000000000000000123400000000FF10
 *    2.4 IPv4oE
 *    DA=01:00:5E:02:02:02 SA=00:11:00:00:00:22 Vid=0x64 SIP=0x6fffff01 DIP=0xe0020202
 *    2.5 IPv6oE
 *    DA=33:33:00:00:12:12 SA=00:11:00:00:00:22 Vid=0x64 SIP=0x0000000000000000123400000000EE10 DIP=0xFFFF0000000000000000000000001212
 * Test Scenario - end
 *
 * 3. Example of PPPoE MP termination with session spoofing check
 *
 * Set up sequence:
 * Test Scenario - start
     cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
     cint ../../../../src/examples/dnx/field/cint_field_utils.c
     cint ../../../../src/examples/sand/cint_ip_route_basic.c
     cint ../../../../src/examples/sand/cint_ipmc_route_basic.c
     cint ../../../../src/examples/sand/utility/cint_sand_utils_multicast.c
     cint ../../../../src/examples/dnx/ppp/cint_dnx_pppoe.c
     cint
     pppoe_example_termination_session_spoofing_check(0, 201, 203);
     exit;
 * Run traffic:
     tx 1  PtchSRCport=201 DATA=0x0011000000110000070001008100001188641100555500370021450000350000000080004cc66fffff017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Expected:
 *     PPPoPPPoEoETH is terminated
 * Delete session spoofing check entry and resend packet
     cint
     pppoe_session_spoofing_delete(0);
     exit;
 * Run traffic:
     tx 1  PtchSRCport=201 DATA=0x0011000000110000070001008100001188641100555500370021450000350000000080004cc66fffff017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Expected:
 *     packet is dropped
 * Test Scenario - end
 *
 * 4. Example of PPPoE MP termination with SIPv4 anti-spoofing
 * Set up sequence:
 * Test Scenario - start
     cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
     cint ../../../../src/examples/dnx/field/cint_field_utils.c
     cint ../../../../src/examples/sand/cint_ip_route_basic.c
     cint ../../../../src/examples/sand/cint_ipmc_route_basic.c
     cint ../../../../src/examples/sand/utility/cint_sand_utils_multicast.c
     cint ../../../../src/examples/dnx/ppp/cint_dnx_pppoe.c
     cint
     pppoe_example_termination_sipv4_spoofing_check(0, 201, 203, 1);
     exit;
 * Run traffic:
 *    1. IPv4oPPPoPPPoEoETH with incorrect SIP
     tx 1  PtchSRCport=201 DATA=0x001100000011000007000100810000118864110055550037002145000035000000008000fa46c08001007fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *    2. IPv4oPPPoPPPoEoETH with correct SIP
     tx 1  PtchSRCport=201 DATA=0x001100000011000007000100810000118864110055550037002145000035000000008000fa45c08001017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Expected:
 *    1. packet is dropped
 *    2. packet is received
 * Test Scenario - end
 *
 * 5. Example of PPPoE MP termination with QOS mapping
 * Set up sequence:
 * Test Scenario - start
     cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
     cint ../../../../src/examples/dnx/field/cint_field_utils.c
     cint ../../../../src/examples/sand/cint_ip_route_basic.c
     cint ../../../../src/examples/sand/cint_ipmc_route_basic.c
     cint ../../../../src/examples/sand/utility/cint_sand_utils_multicast.c
     cint ../../../../src/examples/dnx/ppp/cint_dnx_pppoe.c
     cint
     pppoe_example_encapsulation_qos(0, 201, 203);
     exit;
 * Run traffic:
      tx 1 psrc=203 data=0x001100000022000007000100810000640800451f00350000000080002cc47fffff038fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
      tx 1 psrc=203 data=0x001100000022000007000100810000640800451000350000000080002cc47fffff038fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Expected:
 *     packets are recevied with PPPoE encapsualtion
 *     IPv4.DSCP 1f -> 1e
 *     IPv4.DSCP 10 -> 0e
 * Test Scenario - end
 *
 * 6. Example of PPPoE MP termination with wide LIF(Unsupported)
 *
 * Set up sequence:
 * Test Scenario - start
     cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
     cint ../../../../src/examples/dnx/field/cint_field_utils.c
     cint ../../../../src/examples/sand/cint_ip_route_basic.c
     cint ../../../../src/examples/sand/cint_ipmc_route_basic.c
     cint ../../../../src/examples/sand/utility/cint_sand_utils_multicast.c
     cint ../../../../src/examples/dnx/ppp/cint_dnx_pppoe.c
     cint
     pppoe_example_termination_wide_lif_mp(0, 201, 203);
     exit;
 * Run traffic:
     tx 1  PtchSRCport=201 DATA=0x0011000000110000070001008100001188641100555500370021450000350000000080004cc66fffff017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Expected:
 *     PPPoPPPoEoETH is terminated
 * Test Scenario - end
 *
 * 7. Example of PPPoE to L2TP P2P directional cross connect via Wide LIF(Unsupported)
 *
 * Set up sequence:
 * Test Scenario - start
     cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
     cint ../../../../src/examples/dnx/field/cint_field_utils.c
     cint ../../../../src/examples/sand/cint_ip_route_basic.c
     cint ../../../../src/examples/dnx/ppp/cint_dnx_l2tp.c
     cint ../../../../src/examples/sand/cint_ipmc_route_basic.c
     cint ../../../../src/examples/sand/utility/cint_sand_utils_multicast.c
     cint ../../../../src/examples/dnx/ppp/cint_dnx_pppoe.c
     cint
     pppoe_example_termination_wide_lif_p2p(0, 201, 203);
     exit;
 *
 * PPPoE -> L2TPv2
 * Run traffic:
     tx 1  PtchSRCport=201 DATA=0x0011000000110000070001009100001188641100555500200021450000350000000080004cc56fffff017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Expected:
 *     PPPoPPPoEoETH is terminated
 *     PPPoL2tpv2oUDPoIpv4oETH is generated
 * Test Scenario - end
 */

struct cint_pppoe_basic_info_s
{
    int intf_uni;       /* Vlan id at UNI port*/
    int intf_nni;       /* Vlan id at NNI port */
    bcm_mac_t intf_uni_mac_address;     /* My MAC(UNI) */
    bcm_mac_t intf_nni_mac_address;     /* My MAC(NNI) */
    /*
     * ARP information
     */
    bcm_mac_t arp_next_hop_mac_term;    /* Next hop forwarding Dest MAC for IP forward after PPPoE termination */
    bcm_mac_t arp_next_hop_mac_enc;     /* Next hop forwarding Dest MAC for PPPoE encapsulation */
    bcm_if_t arp_id_term;       /* ARP entry after PPPoE termination for IP forward after PPPoE termination*/
    bcm_if_t arp_id_enc;        /* ARP entry for PPPoE encapsulation */
    /*
     * IPv4 forward information
     */
    int vrf;                     /* VRF */
    bcm_ip_t dip_uni2nni;        /* Route IPv4 DIP (UNI to NNI) */
    bcm_ip_t mask_uni2nni;       /* Route IPv4 DIP mask(UNI to NNI) */
    bcm_ip_t host_uni2nni;       /* Host IPv4 DIP (UNI to NNI) */
    bcm_ip_t dip_nni2uni;        /* Route IPv4 DIP (NNI to UNI) */
    bcm_ip_t mask_nni2uni;       /* Route IPv4 DIP mask(NNI to UNI) */
    bcm_ip_t host_nni2uni;       /* Host IPv4 DIP (NNI to UNI) */
    bcm_ip_t host_sip_uni2nni;   /* Host IPv4 SIP (UNI to NNI) */
    /*
     * IPv6 forward information
     */
    bcm_ip6_t dip6_uni2nni;       /* Route IPv6 DIP (UNI to NNI) */
    bcm_ip6_t mask6_uni2nni;      /* Route IPv6  DIP mask(UNI to NNI) */
    bcm_ip6_t dip6_nni2uni;       /* Route IPv6  DIP (NNI to UNI) */
    bcm_ip6_t mask6_nni2uni;      /* Route IPv6  DIP mask(NNI to UNI) */

    /*
     * MC information
     */
    bcm_multicast_t ingress_mc_group;  /* MC ingress group for IPv4*/
    bcm_multicast_t egress_mc_group;   /* MC egress group for IPv4*/
    bcm_ip_t mc_dip;              /* MC dip for IPv4 */
    bcm_ip_t mc_sip;              /* MC sip for IPv4 */
    bcm_ip6_t mc_dip6;            /* MC dip for IPv6 */
    bcm_ip6_t mc_sip6;            /* MC sip for IPv6 */

    int fec_id_term;              /* FEC to FWD entry after termination */
    int fec_id_enc;               /* FEC to PPPoE encapsulation entry */
    bcm_gport_t initiator_id;     /* Encapsulation object id */
    bcm_gport_t terminator_id;    /* Termination object id */
    bcm_gport_t vlan_port_id;     /* GPORT identifier for inAC taken as KEY to PPPoE session spoofing check */

    int l3_eg_map_id;             /* ID of the egress QOS MAP*/

    /*
     * Input variables
     */
    bcm_mac_t source_mac_address; /* PPPoE Source MAC for termination lookup */
    uint32 session_id;            /* PPPoE Session id in PPPoE header, for both termination and encapsulation */
    int is_mp;                    /* Ture for MP and False for P2P */
    int is_session_spoofing_check;/* Ture to do session_spoofing_check, False to disable */
    int urpf_mode;                /* RPF lookup after PPPoE termination */
    int is_wide;                  /* Wide LIF */
    int is_egress_mc;             /* Egress Multicast */
    int is_with_id;               /* Ture to set WITH_ID flag */
};

cint_pppoe_basic_info_s cint_pppoe_basic_info =
{
    /*
     * intf_uni | intf_nni
     */
    0x11, 0x64,
    /*
     * intf_uni_mac_address
     */
    {0x00, 0x11, 0x00, 0x00, 0x00, 0x11},
    /*
     * intf_nni_mac_address
     */
    {0x00, 0x11, 0x00, 0x00, 0x00, 0x22},
    /*
     * arp_next_hop_mac_term | arp_next_hop_mac_enc
     */
    {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}, {0x00, 0x00, 0x00, 0x00, 0xcd, 0x2d},
    /*
     * arp_id_term | arp_id_enc
     */
    0, 0,
    /*
     * vrf
     */
    0xf,
    /*
     * dip_uni2nni | mask_uni2nni | host_uni2nni
     */
    0x6fffff00, 0xfffffff0, 0x7fffff02,
    /*
     * dip_nni2uni | mask_nni2uni | host_nni2uni
     */
    0x9fffff00, 0xfffffff0, 0x8fffff02,
    /*
     * host_sip_uni2nni
     */
    0x8fffff02,
    /*
     * dip6_uni2nni
     */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x10},
    /*
     * mask6_uni2nni
     */
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0},
    /*
     * dip6_nni2uni
     */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x56, 0x78, 0x00, 0x00, 0x00, 0x11, 0xFF, 0x10},
    /*
     * mask6_nni2uni
     */
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0},
    /*
     * ingress_mc_group
     */
    6000,
    /*
     * egress_mc_group
     */
    6002,
    /*
     * mc_dip
     */
    0xE0020202,
    /*
     * mc_sip
     */
    0x6fffff01,
    /*
     * mc_dip6
     */
    {0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x12},
    /*
     * mc_sip6
     */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xee, 0x10},
    /*
     * fec_id_term | fec_id_enc
     */
    0xCDDD, 0xCDDE,
    /*
     * initiator_id
     */
    0,
    /*
     * terminator_id
     */
    0,
    /*
     * vlan_port_id
     */
    0,
    /*
     * l3_eg_map_id
     */
    0,
    /*
     * source_mac_address
     */
    {0x00, 0x00, 0x07, 0x00, 0x01, 0x00},
    /*
     * session_id
     */
    0x5555,
    /*
     * is_mp
     */
    1,
    /*
     * is_session_spoofing_check
     */
    0,
    /*
     * urpf_mode
     */
    0,
    /*
     * is_wide
     */
    0,
    /*
     * is_egress_mc
     */
    0,
    /*
     * is_with_id
     */
    0
};

/*
 * Clear MC configuraiton after test
 */
int
pppoe_mc_clear(
    int unit)
{

    /** Remove all entries from IPMC table */
    BCM_IF_ERROR_RETURN_MSG(bcm_ipmc_remove_all(unit), "");

    /** Destory MC group */
    if (cint_pppoe_basic_info.is_egress_mc) {
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_encap_extension_delete_all(unit), "");
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_destroy(unit, cint_pppoe_basic_info.egress_mc_group), "");
    } else {
        if (*dnxc_data_get(unit, "multicast", "params", "mcdb_formats_v2", NULL)) {
            BCM_IF_ERROR_RETURN_MSG(bcm_multicast_group_destroy(unit, (BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_ELEMENT_STATIC), cint_pppoe_basic_info.ingress_mc_group), "");
        } else {
            BCM_IF_ERROR_RETURN_MSG(bcm_multicast_destroy(unit, cint_pppoe_basic_info.ingress_mc_group), "");
        }
    }

    return BCM_E_NONE;
}

/*
 * Create PPPoE encapsulation object
 * - session_id: PPPoE session_id in PPPoE header
 * - next_outlif_pointer: next outlif pointer following PPPoE entry, it's ARP normally
 */
int
pppoe_encapsulation_create(
    int unit,
    int session_id,
    bcm_if_t next_outlif_pointer,
    bcm_gport_t *gport)
{

    bcm_ppp_initiator_t initiator_info;

    initiator_info.type = bcmPPPTypePPPoE;
    initiator_info.flags = 0;
    initiator_info.session_id = session_id;
    initiator_info.l3_intf_id = next_outlif_pointer;
    initiator_info.encap_access = bcmEncapAccessTunnel1;
    BCM_IF_ERROR_RETURN_MSG(bcm_ppp_initiator_create(unit, &initiator_info), "");

    printf("pppoe encapsulation: pppoe session id:  0x%x \n", session_id);
    printf("pppoe encapsulation: pppoe encap gport: 0x%x \n", initiator_info.ppp_initiator_id);

    *gport = initiator_info.ppp_initiator_id;
    return BCM_E_NONE;
}

/*
 * Deleter PPPoE encapsulation object per given object ID
 */
int
pppoe_encapsulation_delete(
    int unit)
{
    bcm_ppp_initiator_t initiator_info;

    initiator_info.ppp_initiator_id = cint_pppoe_basic_info.initiator_id;

    BCM_IF_ERROR_RETURN_MSG(bcm_ppp_initiator_delete(unit, &initiator_info), "");

    return BCM_E_NONE;
}

/*
 * Create PPPoE termination object
 * VTT lookup keys
 * - session_id: session_id in PPPoE header
 * - src_mac: source MAC in ETH header
 * PPPoE LIF properties
 * - is_mp: Ture for MP and False for P2P
 * - is_session_spoofing_check: Ture to do session_spoofing_check, False is default
 * RPF
 * - urpf_mode: Enable or disable RPF lookup after PPPoE termination
 * -    0 to bcmL3IngressUrpfDisable
 * -    1 to bcmL3IngressUrpfLoose
 * -    2 to bcmL3IngressUrpfStrict
 * Wide LIF
 * - is_wide: using wide LIF
 */
int
pppoe_termination_create(
    int unit,
    int session_id,
    bcm_mac_t src_mac,
    int is_mp,
    int is_session_spoofing_check,
    int urpf_mode,
    int is_wide,
    int is_with_id,
    bcm_gport_t *gport)
{
    bcm_ppp_terminator_t terminator_info;
    bcm_l3_ingress_t ing_intf;
    bcm_if_t intf_id;

    terminator_info.type = bcmPPPTypePPPoE;
    terminator_info.src_mac = src_mac;
    terminator_info.session_id = session_id;
    terminator_info.flags = 0;
    if (!is_mp)
    {
        terminator_info.flags |= BCM_PPP_TERM_CROSS_CONNECT;
    }

    if (is_session_spoofing_check)
    {
        terminator_info.flags |= BCM_PPP_TERM_SESSION_ANTI_SPOOFING;
    }

    if (is_wide)
    {
        terminator_info.flags |= BCM_PPP_TERM_WIDE;
    }

    if (is_with_id)
    {
        terminator_info.flags |= BCM_PPP_TERM_WITH_ID;
        terminator_info.ppp_terminator_id = cint_pppoe_basic_info.terminator_id;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_ppp_terminator_create(unit, &terminator_info), "");

    printf("pppoe termination: pppoe term lookup src_mac:    %02x:%02x:%02x:%02x:%02x:%02x\n",
        src_mac[0], src_mac[1], src_mac[2],
        src_mac[3], src_mac[4], src_mac[5]);
    printf("pppoe termination: pppoe term lookup session id: 0x%x \n", session_id);
    printf("pppoe termination: pppoe term %s\n", is_mp ? "MP" : "P2P");
    printf("pppoe termination: pppoe term session spoofing check %s\n", is_session_spoofing_check ? "Enabled" : "Disabled");
    printf("pppoe termination: pppoe term gport: 0x%x \n", terminator_info.ppp_terminator_id);

    if (is_mp)
    {
        /** In JR2 VRF is a tunnel-LIF property set by bcm_l3_ingress_create API */
        bcm_l3_ingress_t_init(&ing_intf);
        ing_intf.flags = BCM_L3_INGRESS_WITH_ID;    /* must, as we update exist RIF */
        ing_intf.vrf = cint_pppoe_basic_info.vrf;

        /** Set URFP mode */
        switch(cint_pppoe_basic_info.urpf_mode)
        {
            case 0:
                ing_intf.urpf_mode = bcmL3IngressUrpfDisable;
                break;
            case 1:
                ing_intf.urpf_mode = bcmL3IngressUrpfLoose;
                 break;
            case 2:
                ing_intf.urpf_mode = bcmL3IngressUrpfStrict;
                 break;
            default:
                ing_intf.urpf_mode = bcmL3IngressUrpfDisable;
                break;
        }

        /** Convert tunnel's GPort ID to intf ID */
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(intf_id, terminator_info.ppp_terminator_id);

        BCM_IF_ERROR_RETURN_MSG(bcm_l3_ingress_create(unit, ing_intf, intf_id), "");
    }

    *gport = terminator_info.ppp_terminator_id;
    return BCM_E_NONE;
}

/*
 * Deleter PPPoE termination object per given object ID
 */
int pppoe_termination_delete(
    int unit)
{
    bcm_ppp_terminator_t terminator_info;

    /** PPP type is mandatory */
    terminator_info.type = bcmPPPTypePPPoE;
    terminator_info.ppp_terminator_id = cint_pppoe_basic_info.terminator_id;

    BCM_IF_ERROR_RETURN_MSG(bcm_ppp_terminator_delete(unit, &terminator_info), "");
    return BCM_E_NONE;
}

/*
 * NNI->UNI PPPoE Encapsulation
 * Input variables
 * - cint_pppoe_basic_info.session_id: session_id in PPPoE header
 * - cint_pppoe_basic_info.is_egress_mc: use multilcast egress group
 */
int
pppoe_encapsulation(
    int unit,
    int uni_port,
    int nni_port)
{
    char error_msg[200]={0,};
    l3_ingress_intf ingress_rif;
    bcm_gport_t gport;
    bcm_if_t l3_intf_id;
    uint32 dip_mask = 0xFFFFFFFF;
    uint32 sip_mask = 0x0;
    bcm_ip6_t dip6_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    bcm_ip6_t sip6_mask = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    int flags;
    bcm_multicast_replication_t rep_array;
    uint32 ibch1_supported = *dnxc_data_get(unit, "headers", "system_headers", "system_headers_ibch1_supported", NULL);
    uint32 compressed_vrf = *dnxc_data_get(unit, "l3", "fwd", "ipmc_config_cmprs_vrf_group", NULL);


    /*
     * 1. Set NNI-Port default ETh-RIF
     */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, nni_port, cint_pppoe_basic_info.intf_nni), "intf_in");

    /*
     * 2. Set UNI-Port default properties
     */
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, uni_port), "uni_port");

    /*
     * 3. Create ETH-RIF and set MY-Mac
     */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_pppoe_basic_info.intf_nni, cint_pppoe_basic_info.intf_nni_mac_address), "intf_nni");
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_pppoe_basic_info.intf_uni, cint_pppoe_basic_info.intf_uni_mac_address), "intf_uni");

    /*
     * 4. Set VRF to ETH-RIF
     */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_pppoe_basic_info.vrf;
    ingress_rif.intf_id = cint_pppoe_basic_info.intf_nni;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), intf_nni);

    /*
     * 5. Create ARP and set its properties
     */
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create(unit, 0, &cint_pppoe_basic_info.arp_id_enc,
        cint_pppoe_basic_info.arp_next_hop_mac_enc, cint_pppoe_basic_info.intf_uni), "create egress object ARP only");

    /*
     * 6. Create encap object and set its properties
     */
    BCM_IF_ERROR_RETURN_MSG(pppoe_encapsulation_create(unit, cint_pppoe_basic_info.session_id, cint_pppoe_basic_info.arp_id_enc, &(cint_pppoe_basic_info.initiator_id)), "");
    /*
     * 7. Create FEC and set its properties
     */
    int flags2 = 0;
    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
       flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    BCM_GPORT_LOCAL_SET(gport, uni_port);
    /** Convert tunnel's GPort ID to intf ID */
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(l3_intf_id, cint_pppoe_basic_info.initiator_id);
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, cint_pppoe_basic_info.fec_id_enc, l3_intf_id, 0, gport, 0, flags2),
        "create egress object FEC only");

    /*
     * 8. Add IPv4 host entry
     */
    int fec_id;
    BCM_L3_ITF_SET(&fec_id, BCM_L3_ITF_TYPE_FEC, cint_pppoe_basic_info.fec_id_enc);
    if(*dnxc_data_get(unit, "l3", "fwd", "host_entry_support", NULL))
    {
        BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(unit,
            cint_pppoe_basic_info.host_nni2uni,
            cint_pppoe_basic_info.vrf,
            fec_id, 0, 0), "");
    }
    else 
    {
        BCM_IF_ERROR_RETURN_MSG(add_route_ipv4(unit, cint_pppoe_basic_info.host_nni2uni, 0xffffffff,
                cint_pppoe_basic_info.vrf,
                fec_id), "with full mask");
    }

    /*
     * 9. Add IPv4 Route entry
     */
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv4(unit, cint_pppoe_basic_info.dip_nni2uni,
            cint_pppoe_basic_info.mask_nni2uni,
            cint_pppoe_basic_info.vrf,
            fec_id), "");

    /*
     * 10. Add IPv6 Route entry
     */
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, cint_pppoe_basic_info.dip6_nni2uni,
            cint_pppoe_basic_info.mask6_nni2uni,
            cint_pppoe_basic_info.vrf,
            fec_id), "");

    /*
     * 11. Add Multicast group and entry
     */
    if (!dnx_utils_is_jr2_system_header_mode(unit)) {
        /*
         * If it's IOP mode, use egress MC due to multiple outlifs in MC entries
         */
        cint_pppoe_basic_info.is_egress_mc = 1;
    }

    if (cint_pppoe_basic_info.is_egress_mc)
    {
        flags = BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID;
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, flags, &cint_pppoe_basic_info.egress_mc_group), "");

        bcm_if_t rep_idx = 0x300200;
        bcm_multicast_replication_t_init(&rep_array);
        rep_array.port = uni_port;
        rep_array.encap1 = rep_idx;
        snprintf(error_msg, sizeof(error_msg), "Port= %d", uni_port);
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_add(unit, cint_pppoe_basic_info.egress_mc_group, BCM_MULTICAST_EGRESS_GROUP, 1, &rep_array), error_msg);

        /* Add PPMC entry */
        int cuds[2];
        cuds[0] = cint_pppoe_basic_info.arp_id_enc;
        BCM_L3_ITF_SET(cuds[1], BCM_L3_ITF_TYPE_LIF, BCM_GPORT_TUNNEL_ID_GET(cint_pppoe_basic_info.initiator_id));
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_encap_extension_create(unit, BCM_MULTICAST_ENCAP_EXTENSION_WITH_ID, &rep_idx, 2, cuds), "");

        snprintf(error_msg, sizeof(error_msg), "mc_group= %d", cint_pppoe_basic_info.egress_mc_group);
        BCM_IF_ERROR_RETURN_MSG(multicast__open_fabric_mc_or_ingress_mc_for_egress_mc(unit, cint_pppoe_basic_info.egress_mc_group, cint_pppoe_basic_info.egress_mc_group), error_msg);

        /* Add IPMC entry */
        snprintf(error_msg, sizeof(error_msg), "vrf = %d, mc group = %d", cint_pppoe_basic_info.vrf, cint_pppoe_basic_info.egress_mc_group);
        BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc(unit, cint_pppoe_basic_info.mc_dip, dip_mask, cint_pppoe_basic_info.mc_sip, sip_mask, 0, cint_pppoe_basic_info.vrf, cint_pppoe_basic_info.egress_mc_group, 0,0), error_msg);
        BCM_IF_ERROR_RETURN_MSG(add_ipv6_ipmc(unit, cint_pppoe_basic_info.mc_dip6, dip6_mask, cint_pppoe_basic_info.mc_sip6, sip6_mask, 0, cint_pppoe_basic_info.vrf, 0, 0, cint_pppoe_basic_info.egress_mc_group, 0, 0, 0), error_msg);
    }
    else
    {
        if (*dnxc_data_get(unit, "multicast", "params", "mcdb_formats_v2", NULL)) {
            flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_ELEMENT_STATIC;
        } else {
            flags = BCM_MULTICAST_INGRESS_GROUP;
        }
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, (flags | BCM_MULTICAST_WITH_ID), &cint_pppoe_basic_info.ingress_mc_group), "");
        if (ibch1_supported)
        {
            bcm_multicast_replication_t_init(&rep_array);
            rep_array.port = uni_port;
            rep_array.encap1 = BCM_GPORT_TUNNEL_ID_GET(cint_pppoe_basic_info.initiator_id) & 0x3fffff;
            snprintf(error_msg, sizeof(error_msg), "Port= %d", uni_port);
            BCM_IF_ERROR_RETURN_MSG(bcm_multicast_add(unit, cint_pppoe_basic_info.ingress_mc_group, flags, 1, &rep_array), error_msg);
        }
        else
        {
            bcm_if_t rep_idx = 0x300100;
            bcm_multicast_replication_t_init(&rep_array);
            rep_array.port = uni_port;
            rep_array.encap1 = rep_idx;
            snprintf(error_msg, sizeof(error_msg), "Port= %d", uni_port);
            BCM_IF_ERROR_RETURN_MSG(bcm_multicast_add(unit, cint_pppoe_basic_info.ingress_mc_group, flags, 1, &rep_array), error_msg);

            /* Add PPMC entry */
            int cuds[2];
            cuds[0] = cint_pppoe_basic_info.arp_id_enc;
            BCM_L3_ITF_SET(cuds[1], BCM_L3_ITF_TYPE_LIF, BCM_GPORT_TUNNEL_ID_GET(cint_pppoe_basic_info.initiator_id));
            BCM_IF_ERROR_RETURN_MSG(bcm_multicast_encap_extension_create(unit, BCM_MULTICAST_ENCAP_EXTENSION_WITH_ID, &rep_idx, 2, cuds), "");
        }

        /* Add IPMC entry */
        snprintf(error_msg, sizeof(error_msg), "vrf = %d, mc group = %d", cint_pppoe_basic_info.vrf, cint_pppoe_basic_info.ingress_mc_group);
        BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc(unit, cint_pppoe_basic_info.mc_dip, dip_mask, cint_pppoe_basic_info.mc_sip, sip_mask, 0, cint_pppoe_basic_info.vrf, cint_pppoe_basic_info.ingress_mc_group, 0,0), error_msg);
        
   		if(!compressed_vrf)
        {   
            BCM_IF_ERROR_RETURN_MSG(add_ipv6_ipmc(unit, cint_pppoe_basic_info.mc_dip6, dip6_mask, cint_pppoe_basic_info.mc_sip6, sip6_mask, 0, cint_pppoe_basic_info.vrf, 0, 0, cint_pppoe_basic_info.ingress_mc_group, 0, 0, 0), error_msg);
        }
    }

    return BCM_E_NONE;
}

/*
 * UNI->NNI PPPoE Termination
 * Input variables
 * 1. VTT lookup keys
 * - cint_pppoe_basic_info.session_id: session_id in PPPoE header
 * - cint_pppoe_basic_info.source_mac_address: source MAC in ETH header
 * 2. PPPoE LIF properties
 * - cint_pppoe_basic_info.is_mp: Ture for MP and False for P2P
 * - cint_pppoe_basic_info.is_session_spoofing_check: Ture to do session_spoofing_check, False is default
 * 3. SIPv4 anti-spoofing
 * - cint_pppoe_basic_info.urpf_mode: disable/enable URPF for SIPv4 anti-spoofing
 * 4. cint_pppoe_basic_info.is_egress_mc: use multilcast egress group
 */
int
pppoe_termination(
    int unit,
    int uni_port,
    int nni_port)
{
    int rv = BCM_E_NONE;
    char error_msg[200]={0,};
    l3_ingress_intf ingress_rif;
    bcm_gport_t gport;
    uint32 dip_mask = 0xFFFFFFFF;
    uint32 sip_mask = 0x0;
    bcm_ip6_t dip6_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    bcm_ip6_t sip6_mask = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    int flags;
    bcm_multicast_replication_t rep_array;
    uint32 compressed_vrf = *dnxc_data_get(unit, "l3", "fwd", "ipmc_config_cmprs_vrf_group", NULL);


    /*
     * 1. Set UNI-Port to In ETh-RIF
     */
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    vlan_port.port = uni_port;
    vlan_port.match_vlan = cint_pppoe_basic_info.intf_uni;
    vlan_port.vsi = cint_pppoe_basic_info.intf_uni;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");
    cint_pppoe_basic_info.vlan_port_id = vlan_port.vlan_port_id;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, cint_pppoe_basic_info.intf_uni, uni_port, 0), "");

    /*
     * 2. Set NNI-Port default properties
     */
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, nni_port), "nni_port");

    /*
     * 3. Create ETH-RIF and set MY-Mac
     */
    rv = intf_eth_rif_create(unit, cint_pppoe_basic_info.intf_uni, cint_pppoe_basic_info.intf_uni_mac_address);
    if (rv == BCM_E_EXISTS) {
        rv = BCM_E_NONE;
    }
    BCM_IF_ERROR_RETURN_MSG(rv, "intf_uni");
    rv = intf_eth_rif_create(unit, cint_pppoe_basic_info.intf_nni, cint_pppoe_basic_info.intf_nni_mac_address);
    if (rv == BCM_E_EXISTS) {
        rv = BCM_E_NONE;
    }
    BCM_IF_ERROR_RETURN_MSG(rv, "intf_nni");

    /*
     * 4. Set VRF to ETH-RIF
     */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_pppoe_basic_info.vrf;
    ingress_rif.intf_id = cint_pppoe_basic_info.intf_uni;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "intf_uni");

    /*
     * 5. Create PPPoE tunnel termination
     */
    BCM_IF_ERROR_RETURN_MSG(pppoe_termination_create(unit,
                        cint_pppoe_basic_info.session_id,
                        cint_pppoe_basic_info.source_mac_address,
                        cint_pppoe_basic_info.is_mp,
                        cint_pppoe_basic_info.is_session_spoofing_check,
                        cint_pppoe_basic_info.urpf_mode,
                        cint_pppoe_basic_info.is_wide,
                        cint_pppoe_basic_info.is_with_id,
                        &(cint_pppoe_basic_info.terminator_id)), "");
    /*
     * 6. Create ARP and set its properties
     */
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create(unit, 0, &cint_pppoe_basic_info.arp_id_term,
        cint_pppoe_basic_info.arp_next_hop_mac_term, cint_pppoe_basic_info.intf_nni), "create egress object ARP only");

    /*
     * 7. Create FEC and set its properties
     */
    BCM_GPORT_LOCAL_SET(gport, nni_port);
    int flags2 = 0;
    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
       flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, cint_pppoe_basic_info.fec_id_term, cint_pppoe_basic_info.intf_nni, cint_pppoe_basic_info.arp_id_term, gport, 0, flags2),
        "create egress object FEC only");

    /*
     * 8. Add IPv4 Host entries
     */
    if(*dnxc_data_get(unit, "l3", "fwd", "host_entry_support", NULL))
    {
        BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(unit, cint_pppoe_basic_info.host_uni2nni,
                cint_pppoe_basic_info.vrf,
                cint_pppoe_basic_info.fec_id_term, 0, 0), "");
    }
    else 
    {
        BCM_IF_ERROR_RETURN_MSG(add_route_ipv4(unit, cint_pppoe_basic_info.host_uni2nni, 0xffffffff,
                cint_pppoe_basic_info.vrf,
                cint_pppoe_basic_info.fec_id_term), "with full mask");
    }

    /*
     * 9. Add IPv4 Route entry
     */
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv4(unit, cint_pppoe_basic_info.dip_uni2nni,
            cint_pppoe_basic_info.mask_uni2nni,
            cint_pppoe_basic_info.vrf,
            cint_pppoe_basic_info.fec_id_term), "");

    /*
     * 10. Add IPv6 Route entry
     */
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, cint_pppoe_basic_info.dip6_uni2nni,
            cint_pppoe_basic_info.mask6_uni2nni,
            cint_pppoe_basic_info.vrf,
            cint_pppoe_basic_info.fec_id_term), "");

    /*
     * 11. Add multicast group and add entries
     */
    if (cint_pppoe_basic_info.is_egress_mc)
    {
        flags = BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID;
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, flags, &cint_pppoe_basic_info.egress_mc_group), "");
        bcm_multicast_replication_t_init(&rep_array);
        rep_array.port = nni_port;
        rep_array.encap1 = cint_pppoe_basic_info.intf_nni;
        snprintf(error_msg, sizeof(error_msg), "Port= %d", nni_port);
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_add(unit, cint_pppoe_basic_info.egress_mc_group, BCM_MULTICAST_EGRESS_GROUP, 1, &rep_array), error_msg);
        snprintf(error_msg, sizeof(error_msg), "mc_group= %d", cint_pppoe_basic_info.egress_mc_group);
        BCM_IF_ERROR_RETURN_MSG(multicast__open_fabric_mc_or_ingress_mc_for_egress_mc(unit, cint_pppoe_basic_info.egress_mc_group, cint_pppoe_basic_info.egress_mc_group), error_msg);
        snprintf(error_msg, sizeof(error_msg), "vrf = %d, mc group = %d", cint_pppoe_basic_info.vrf, cint_pppoe_basic_info.egress_mc_group);
        BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc(unit, cint_pppoe_basic_info.mc_dip, dip_mask, cint_pppoe_basic_info.mc_sip, sip_mask, 0, cint_pppoe_basic_info.vrf, cint_pppoe_basic_info.egress_mc_group, 0,0), error_msg);
        BCM_IF_ERROR_RETURN_MSG(add_ipv6_ipmc(unit, cint_pppoe_basic_info.mc_dip6, dip6_mask, cint_pppoe_basic_info.mc_sip6, sip6_mask, 0, cint_pppoe_basic_info.vrf, 0, 0, cint_pppoe_basic_info.egress_mc_group, 0, 0, 0), error_msg);
    }
    else
    {
        if (*dnxc_data_get(unit, "multicast", "params", "mcdb_formats_v2", NULL)) {
            flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_ELEMENT_STATIC;
        } else {
            flags = BCM_MULTICAST_INGRESS_GROUP;
        }
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, (flags | BCM_MULTICAST_WITH_ID), &cint_pppoe_basic_info.ingress_mc_group), "");
        bcm_multicast_replication_t_init(&rep_array);
        rep_array.port = nni_port;
        rep_array.encap1 = cint_pppoe_basic_info.intf_nni;
        snprintf(error_msg, sizeof(error_msg), "Port= %d", nni_port);
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_add(unit, cint_pppoe_basic_info.ingress_mc_group, flags, 1, &rep_array), error_msg);
        snprintf(error_msg, sizeof(error_msg), "vrf = %d, mc group = %d", cint_pppoe_basic_info.vrf, cint_pppoe_basic_info.ingress_mc_group);
        BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc(unit, cint_pppoe_basic_info.mc_dip, dip_mask, cint_pppoe_basic_info.mc_sip, sip_mask, 0, cint_pppoe_basic_info.vrf, cint_pppoe_basic_info.ingress_mc_group, 0,0), error_msg);
        
   		if(!compressed_vrf)
        {
            BCM_IF_ERROR_RETURN_MSG(add_ipv6_ipmc(unit, cint_pppoe_basic_info.mc_dip6, dip6_mask, cint_pppoe_basic_info.mc_sip6, sip6_mask, 0, cint_pppoe_basic_info.vrf, 0, 0, cint_pppoe_basic_info.ingress_mc_group, 0, 0, 0), error_msg);
        }
    }

    return BCM_E_NONE;
}

/*
 * Add a PPP Term spoofing check per given PPP Tunnel Gport and Vlan Gport.
 * - pppoe_terminator_id: PPP Tunnel Gport.
 * - vlan_port_id: Vlan Gport.
 */
int
pppoe_termination_session_spoofing_add(
    int unit,
    bcm_gport_t pppoe_terminator_id,
    bcm_gport_t vlan_port_id)
{
    bcm_ppp_term_spoofing_check_t info;

    info.ppp_terminator_id = pppoe_terminator_id;
    info.vlan_port_id = vlan_port_id;

    BCM_IF_ERROR_RETURN_MSG(bcm_ppp_term_spoofing_check_add(unit, &info), "");

    printf("pppoe termination: session spooffing add pppoe_terminator_id: 0x%x \n", pppoe_terminator_id);
    printf("pppoe termination: session spooffing add vlan_port_id: 0x%x \n", vlan_port_id);

    return BCM_E_NONE;
}

/*
 * Delete a PPP Term spoofing check per given PPP Tunnel Gport and Vlan Gport.
 * - pppoe_terminator_id: PPP Tunnel Gport.
 * - vlan_port_id: Vlan Gport.
 */
int
pppoe_termination_session_spoofing_delete(
    int unit,
    bcm_gport_t pppoe_terminator_id,
    bcm_gport_t vlan_port_id)
{
    bcm_ppp_term_spoofing_check_t info;

    info.ppp_terminator_id = pppoe_terminator_id;
    info.vlan_port_id = vlan_port_id;

    BCM_IF_ERROR_RETURN_MSG(bcm_ppp_term_spoofing_check_delete(unit, &info), "");
    return BCM_E_NONE;
}

/*
 * Add a PPPoE session spoofing check entry per given PPP Tunnel Gport and Vlan Gport.
 */
int
pppoe_session_spoofing_add(
    int unit)
{

    BCM_IF_ERROR_RETURN_MSG(pppoe_termination_session_spoofing_add(unit, cint_pppoe_basic_info.terminator_id, cint_pppoe_basic_info.vlan_port_id), "");
    return BCM_E_NONE;
}

/*
 * Delete a PPPoE session spoofing check entry per given PPP Tunnel Gport and Vlan Gport.
 */
int
pppoe_session_spoofing_delete(
    int unit)
{

    BCM_IF_ERROR_RETURN_MSG(pppoe_termination_session_spoofing_delete(unit, cint_pppoe_basic_info.terminator_id, cint_pppoe_basic_info.vlan_port_id), "");
    return BCM_E_NONE;
}

/*
 * Set trap bcmRxTrapUcLooseRpfFail/bcmRxTrapUcStrictRpfFail to drop packet
 */
int
pppoe_ip_rpf_config_traps(
    int unit,
    int urpf_mode)
{
    int rv = BCM_E_NONE, i;
    bcm_rx_trap_config_t config;
    int flags = 0;
    int urpf_trap = 0;

    /** Set discard as dest port */
    bcm_rx_trap_config_t_init(&config);
    config.flags |= (BCM_RX_TRAP_UPDATE_DEST);
    config.trap_strength = 7;
    config.dest_port = BCM_GPORT_BLACK_HOLE;

    switch(urpf_mode)
    {
        case 1:
            rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapUcLooseRpfFail, &urpf_trap);
            if (rv != BCM_E_NONE)
            {
                BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_create(unit, 0, bcmRxTrapUcLooseRpfFail, &urpf_trap),
                    "bcmRxTrapUcLooseRpfFail");
            }
            break;
        case 2:
             rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapUcStrictRpfFail, &urpf_trap);
             if (rv != BCM_E_NONE)
             {
                 BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_create(unit, 0, bcmRxTrapUcStrictRpfFail, &urpf_trap),
                    "bcmRxTrapUcLooseRpfFail");
             }
             break;
        default:
            urpf_trap = 0;
            break;
    }

    if (urpf_trap)
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_set(unit, urpf_trap, &config), "bcmRxTrapUcLooseRpfFail");
    }

    return BCM_E_NONE;
}

/*
 * Main function for qos egress remark setting:
 *   1. Create QOS egress profile.
 *   2. Create QOS egress opcode for PHB
 *   3. Set the qos mapping
 */
int
pppoe_qos_map_egress_phb_remark_set(int unit)
{
    char error_msg[200]={0,};
    int flags = 0;
    bcm_qos_map_t l3_eg_map;
    int dscp;
    int tc,dp;
    int l3_eg_opcode_id;

    /** 1. Create qos-profile */
    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK;
    BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_create(unit, flags, &cint_pppoe_basic_info.l3_eg_map_id), "for egress cos-profile");

    printf("bcm_qos_map_create egress cos-profile: 0x%x\n", cint_pppoe_basic_info.l3_eg_map_id);

    /** 2. Create opcode */
    BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE, &l3_eg_opcode_id), "for egress opcode");

    printf("bcm_qos_map_create egress opcode: 0x%x\n", l3_eg_opcode_id);

    bcm_qos_map_t_init(&l3_eg_map);
    l3_eg_map.opcode = l3_eg_opcode_id;

    /** Add the maps for remarking */
    BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE, &l3_eg_map, cint_pppoe_basic_info.l3_eg_map_id),
        "for egress opcode");

    /* set QoS mapping for : map DSCP => (DSCP-2) */
    for (dp = 0; dp < 4; dp++)
    {
        for (dscp = 0; dscp < 64; dscp++) {
            bcm_qos_map_t_init(&l3_eg_map);
            l3_eg_map.int_pri = dscp;
            l3_eg_map.color = dp;
            l3_eg_map.dscp = (dscp - 2) & 0x3f;

            snprintf(error_msg, sizeof(error_msg), "set L3 PCP QoS mapping, dscp=%d, dp=%d", dscp, dp);
            BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK, &l3_eg_map, l3_eg_opcode_id), error_msg);
        }
    }

    return BCM_E_NONE;
}

/*
 * \brief - Clean the PMF setting: delete FG / UDF / entry, detach context.
 *
 */
int pppoe_inlif_large_wide_data_pmf_clean(int unit, bcm_field_context_t ipmf1_context_id)
{
    bcm_field_presel_entry_id_t presel_entry_id;
    bcm_field_presel_entry_data_t presel_entry_data;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_delete(unit, cint_pppoe_basic_info.fg_id, NULL, cint_pppoe_basic_info.ent_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_detach(unit, cint_pppoe_basic_info.fg_id, ipmf1_context_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_delete(unit, cint_pppoe_basic_info.fg_id), "");

    cint_pppoe_basic_info.is_pmf_config=0;

    return BCM_E_NONE;
}

/**
* \brief -
*       PMF configuration for 28 bits wide data.
*       8lsb use bcmFieldQualifyAcInLifWideData, 20 msb use UDF qualifier.
* Input variables
*  - dest_port: redirect port
*  - data: wide data to set
*  - mask: mask for wide data
*  - ipmf1_context_id: The context used in iPMF1
*/
int pppoe_inlif_large_wide_data_pmf_configure(int unit, int dest_port, uint32 data, uint32 mask, bcm_field_context_t ipmf1_context_id)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t entry_info;
    void *dest_char;
    int sys_gport = 0;
    bcm_field_qualifier_info_get_t qual_info_get;

    if (cint_pppoe_basic_info.is_pmf_config) {
        BCM_IF_ERROR_RETURN_MSG(pppoe_inlif_large_wide_data_pmf_clean(unit), "");
    }

    /*Create FG*/
    bcm_field_group_info_t_init(&fg_info);
    fg_info.stage = bcmFieldStageIngressPMF1;
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyAcInLifWideData;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionRedirect;

    dest_char = &fg_info.name[0];
    sal_strncpy_s(dest_char, "Wide-Data filter", sizeof(fg_info.name));
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_add(unit, 0, &fg_info, &cint_pppoe_basic_info.fg_id), "");

    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].input_arg = 0;

    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_attach(unit, 0, cint_pppoe_basic_info.fg_id, ipmf1_context_id, &attach_info), "");
    printf("Create FG[%d] for inlif-wide-data!\n", cint_pppoe_basic_info.fg_id);

    BCM_GPORT_SYSTEM_PORT_ID_SET(sys_gport, dest_port);
    bcm_field_entry_info_t_init(&entry_info);
    entry_info.priority = 1;
    entry_info.nof_entry_quals = 1;
    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = data;
    entry_info.entry_qual[0].mask[0] = mask;

    entry_info.nof_entry_actions = fg_info.nof_actions;
    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = sys_gport;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, 0, cint_pppoe_basic_info.fg_id, &entry_info, &cint_pppoe_basic_info.ent_id), "");
    printf("PPPoE PMF configuration on wide LIF successfully!\n");

    cint_pppoe_basic_info.is_pmf_config = 1;
    return BCM_E_NONE;
}

/*
 * Example of PPPoE Encapsulation(NNI->UNI)
 * Input variables
 * - cint_pppoe_basic_info.session_id: session_id in PPPoE header
 */
int
pppoe_example_encapsulation(
    int unit,
    int uni_port,
    int nni_port)
{
    int fec_start = 0;
    int nof_fecs = 0;

    BCM_IF_ERROR_RETURN_MSG(get_fec_range(unit, 0, 0, fec_start, &nof_fecs), "");
    cint_pppoe_basic_info.fec_id_term = fec_start + 500;
    cint_pppoe_basic_info.fec_id_enc = fec_start + 501;

    BCM_IF_ERROR_RETURN_MSG(pppoe_encapsulation(unit, uni_port, nni_port), "");
    return BCM_E_NONE;
}

/*
 * Example of PPPoE Termination(UNI->NNI)
 * Input variables
 * 1. VTT lookup keys(as default)
 * - cint_pppoe_basic_info.session_id: session_id in PPPoE header
 * - cint_pppoe_basic_info.source_mac_address: source MAC in ETH header
 * 2. PPPoE LIF properties(as default)
 * - cint_pppoe_basic_info.is_mp: Ture for MP and False for P2P
 * - cint_pppoe_basic_info.is_session_spoofing_check: Ture to do session_spoofing_check, False is default
 * 3. SIPv4 anti-spoofing(as default)
 * - cint_pppoe_basic_info.urpf_mode: disable/enable URPF for SIPv4 anti-spoofing
 */
int
pppoe_example_termination(
    int unit,
    int uni_port,
    int nni_port)
{
    int fec_start = 0;
    int nof_fecs = 0;

    /** Enable MP */
    cint_pppoe_basic_info.is_mp = 1;
    /** Disable PPPoE sesion spoofing check */
    cint_pppoe_basic_info.is_session_spoofing_check = 0;
    /** No RPF */
    cint_pppoe_basic_info.urpf_mode = 0;
    /** No Wide LIF */
    cint_pppoe_basic_info.is_wide = 0;

    BCM_IF_ERROR_RETURN_MSG(get_fec_range(unit, 0, 0, fec_start, &nof_fecs), "");
    cint_pppoe_basic_info.fec_id_term = fec_start + 500;
    cint_pppoe_basic_info.fec_id_enc = fec_start + 501;

    BCM_IF_ERROR_RETURN_MSG(pppoe_termination(unit, uni_port, nni_port), "");
    return BCM_E_NONE;
}

/*
 * Example of PPPoE Termination with session spoofing check(UNI->NNI)
 * Input variables
 * 1. VTT lookup keys(as default)
 * - cint_pppoe_basic_info.session_id: session_id in PPPoE header
 * - cint_pppoe_basic_info.source_mac_address: source MAC in ETH header
 * 2. PPPoE LIF properties
 * - cint_pppoe_basic_info.is_mp: Ture for MP and False for P2P(as default)
 * - cint_pppoe_basic_info.is_session_spoofing_check: Ture to do session_spoofing_check here
 * 3. SIPv4 anti-spoofing(as default)
 * - cint_pppoe_basic_info.urpf_mode: disable/enable URPF for SIPv4 anti-spoofing
 */
int
pppoe_example_termination_session_spoofing_check(
    int unit,
    int uni_port,
    int nni_port)
{
    /** Enable MP */
    cint_pppoe_basic_info.is_mp = 1;
    /** Enable PPPoE sesion spoofing check */
    cint_pppoe_basic_info.is_session_spoofing_check = 1;
    /** No RPF */
    cint_pppoe_basic_info.urpf_mode = 0;
    /** No Wide LIF */
    cint_pppoe_basic_info.is_wide = 0;

    /** Get FEC IDs ***/
    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &cint_pppoe_basic_info.fec_id_term), "");

    /** UNI->NNI PPPoE Termination */
    BCM_IF_ERROR_RETURN_MSG(pppoe_termination(unit, uni_port, nni_port), "");

    /** PPPoE session spoofing check */
    BCM_IF_ERROR_RETURN_MSG(pppoe_session_spoofing_add(unit), "");
    return BCM_E_NONE;
}

/*
 * Example of SIPv4 ant-spoofing via RPF(Loose Mode) for PPPoE over IPv4 unicast
 * - RPF miss: drop
 * - RPF hit: normal forward
 * Input variables
 * 1 VTT lookup keys(as default)
 * - cint_pppoe_basic_info.session_id: session_id in PPPoE header
 * - cint_pppoe_basic_info.source_mac_address: source MAC in ETH header
 * 2 PPPoE LIF properties(as default)
 * - cint_pppoe_basic_info.is_mp: Ture for MP, must be MP here
 * - cint_pppoe_basic_info.is_session_spoofing_check: Ture to do session_spoofing_check, False is default
 * 3 Enable RPF check
 * - cint_pppoe_basic_info.urpf_mode: Loose mode here
 */
int
pppoe_example_termination_sipv4_spoofing_check(
    int unit,
    int uni_port,
    int nni_port,
    int urpf_mode)
{
    l3_ingress_intf ingress_rif;
    int fec_rpf;
    bcm_gport_t gport;


    /** MP */
    cint_pppoe_basic_info.is_mp = 1;
    /** Disable PPPoE sesion spoofing check */
    cint_pppoe_basic_info.is_session_spoofing_check = 0;
    /** Enable Loose RPF mode */
    cint_pppoe_basic_info.urpf_mode = urpf_mode;
    /** No Wide LIF */
    cint_pppoe_basic_info.is_wide = 0;

    /** Get FEC IDs ***/
    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &cint_pppoe_basic_info.fec_id_term), "");

    if (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 1, &fec_rpf), "");
    }
    else
    {
        fec_rpf = cint_pppoe_basic_info.fec_id_term + 2;
    }

    /** UNI->NNI PPPoE Termination, RPF enabled at PPPoE LIF per bcm_l3_ingress_create()  */
    BCM_IF_ERROR_RETURN_MSG(pppoe_termination(unit, uni_port, nni_port), "");

    /*
     * Set trap bcmRxTrapUcLooseRpfFail to drop packet
     */
    BCM_IF_ERROR_RETURN_MSG(pppoe_ip_rpf_config_traps(unit, urpf_mode), "");

    /*
     * FECs for RPF
     */
    BCM_GPORT_LOCAL_SET(gport, nni_port);
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, fec_rpf, cint_pppoe_basic_info.terminator_id, 0, gport, 0, 0), "");

    /*
     * Add IPv4 SIP entries
     */
    BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(unit, cint_pppoe_basic_info.host_sip_uni2nni,
            cint_pppoe_basic_info.vrf,
            fec_rpf, 0, 0), "");

    return BCM_E_NONE;
}

/*
 * Example of PPPoE Encapsulaiton with egress QOS map(NNI->UNI)
 */
int
pppoe_example_encapsulation_qos(
    int unit,
    int uni_port,
    int nni_port)
{

    /** Get FEC IDs ***/
    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &cint_pppoe_basic_info.fec_id_enc), "");

    /** Basic PPPoE encapsulatio */
    BCM_IF_ERROR_RETURN_MSG(pppoe_encapsulation(unit, uni_port, nni_port), "");

    /** QOS egress remark setting */
    BCM_IF_ERROR_RETURN_MSG(pppoe_qos_map_egress_phb_remark_set(unit), "");

    /** Set QOS profile to tunnel gport */
    BCM_IF_ERROR_RETURN_MSG(bcm_qos_port_map_set(unit, cint_pppoe_basic_info.initiator_id, -1, cint_pppoe_basic_info.l3_eg_map_id), "");

    return BCM_E_NONE;
}

/*
 * Example of PPPoE Termination with Wide LIF MP(NNI->UNI)
 * Note that BCM_PPP_TERM_WIDE is not supported in current version of SDK(6.5.18)
 * becasue wide data feature is not available for PPP termination LIF
 */
int
pppoe_example_termination_wide_lif_mp(
    int unit,
    int uni_port,
    int nni_port)
{

    /** Enable MP */
    cint_pppoe_basic_info.is_mp = 1;
    /** Disable PPPoE sesion spoofing check */
    cint_pppoe_basic_info.is_session_spoofing_check = 0;
    /** No RPF */
    cint_pppoe_basic_info.urpf_mode = 0;
    /** Wide LIF */
    cint_pppoe_basic_info.is_wide = 1;
    BCM_IF_ERROR_RETURN_MSG(pppoe_termination(unit, uni_port, nni_port), "");
    return BCM_E_NONE;
}

/*
 * Example of PPPoE Termination with Wide LIF P2P(NNI->UNI)
 * Note that BCM_PPP_TERM_WIDE is not supported in current version of SDK(6.5.18)
 * becasue wide data feature is not available for PPP termination LIF
 */
int
pppoe_example_termination_wide_lif_p2p(
    int unit,
    int uni_port,
    int nni_port)
{

    l3_ingress_intf ingress_rif;
    bcm_gport_t gport;
    bcm_if_t tunnel_inlif_id;

    /*
     * 1. Set In-Port default ETH-RIF
     */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, uni_port, cint_pppoe_basic_info.intf_uni), "intf_in");
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, nni_port, cint_l2tp_basic_info.intf_in), "intf_in");

    /*
     * 2. Create ETH-RIF and set its properties
     */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_pppoe_basic_info.intf_uni, cint_pppoe_basic_info.intf_uni_mac_address), "intf_uni");
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_l2tp_basic_info.intf_in, cint_l2tp_basic_info.intf_in_mac_address), "intf_in");

    /*
     * 3. Set Incoming ETH-RIF properties
     */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_pppoe_basic_info.vrf;
    ingress_rif.intf_id = cint_pppoe_basic_info.intf_uni;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "intf_out");

    /*
     * 4. Set NNI-Port default properties, Create ETH-RIF and set its properties
     */
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, nni_port), "intf_out");
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_l2tp_basic_info.intf_out, cint_l2tp_basic_info.intf_out_mac_address), "intf_out");

    /*
     * 5. PPPoE -> L2TPv2 P2P service
     */
    /** Enable P2P */
    cint_pppoe_basic_info.is_mp = 0;
    /** Disable PPPoE sesion spoofing check */
    cint_pppoe_basic_info.is_session_spoofing_check = 0;
    /** No RPF */
    cint_pppoe_basic_info.urpf_mode = 0;
    /** Wide LIF */
    cint_pppoe_basic_info.is_wide = 1;
    BCM_IF_ERROR_RETURN_MSG(pppoe_2_l2tp_p2p(unit, uni_port, nni_port), "create tunnel termination");
    return BCM_E_NONE;
}

/*
 * Example of strict uRPF support for PPPoE subscriber
 * For PPPoE termination direction, do the strict uRPF
     a. PPPoE termination -> PPPoE Terminated id
     b. uRPF lookup -> Customer SIP  lookup -> RPF_OUT_LIF -> PPPoE Terminated id.
 * For PPPoE Encapsulation direction
     a. DIP lookup -> PPPoE Encatpsulation id.
 * Since it's a bi-direction service, the SIP of termination is same as DIP of encapsulation
 * Stpes
 * 1. Config encapsulation
 * 2. Config termination. termination id = encapsulation id.
 * 3. Config trap to drop urpf failed packets
 */
int
pppoe_example_strict_urpf_for_pppoe_subscriber(
    int unit,
    int uni_port,
    int nni_port,
    int urpf_mode)
{
    int fec_start = 0;
    int nof_fecs = 0;


    BCM_IF_ERROR_RETURN_MSG(get_fec_range(unit, 0, 0, fec_start, &nof_fecs), "");
    cint_pppoe_basic_info.fec_id_term = fec_start + 500;
    cint_pppoe_basic_info.fec_id_enc = fec_start + 501;

    /** 1. Ecapsulation Direction */
    BCM_IF_ERROR_RETURN_MSG(pppoe_encapsulation(unit, uni_port, nni_port), "");

    /** 2. Termination Direction */
    l3_ingress_intf ingress_rif;
    bcm_gport_t gport;
    uint32 dip_mask = 0xFFFFFFFF;
    uint32 sip_mask = 0x0;
    bcm_ip6_t dip6_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    bcm_ip6_t sip6_mask = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    int flags;

    /** MP */
    cint_pppoe_basic_info.is_mp = 1;
    /** Disable PPPoE sesion spoofing check */
    cint_pppoe_basic_info.is_session_spoofing_check = 0;
    /** Enable Loose/Strict RPF mode */
    cint_pppoe_basic_info.urpf_mode = urpf_mode;
    /** No Wide LIF */
    cint_pppoe_basic_info.is_wide = 0;
    /** PPoE Encatpsulation id = PPPoE Terminated id */
    cint_pppoe_basic_info.is_with_id = 1;
    cint_pppoe_basic_info.terminator_id = cint_pppoe_basic_info.initiator_id;

    /*
     * 2.1. Set UNI-Port to In ETh-RIF
     */
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    vlan_port.port = uni_port;
    vlan_port.match_vlan = cint_pppoe_basic_info.intf_uni;
    vlan_port.vsi = cint_pppoe_basic_info.intf_uni;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");
    cint_pppoe_basic_info.vlan_port_id = vlan_port.vlan_port_id;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, cint_pppoe_basic_info.intf_uni, uni_port, 0), "");

    /*
     * 2.2. Set NNI-Port default properties
     */
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, nni_port), "nni_port");

    /*
     * 2.3. Set VRF to ETH-RIF
     */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_pppoe_basic_info.vrf;
    ingress_rif.intf_id = cint_pppoe_basic_info.intf_uni;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "intf_uni");

    /*
     * 2.4. Create PPPoE tunnel termination
     */
    BCM_IF_ERROR_RETURN_MSG(pppoe_termination_create(unit,
                        cint_pppoe_basic_info.session_id,
                        cint_pppoe_basic_info.source_mac_address,
                        cint_pppoe_basic_info.is_mp,
                        cint_pppoe_basic_info.is_session_spoofing_check,
                        cint_pppoe_basic_info.urpf_mode,
                        cint_pppoe_basic_info.is_wide,
                        cint_pppoe_basic_info.is_with_id,
                        &(cint_pppoe_basic_info.terminator_id)), "");
    /*
     * 2.5. Create ARP and set its properties
     */
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create(unit, 0, &cint_pppoe_basic_info.arp_id_term,
        cint_pppoe_basic_info.arp_next_hop_mac_term, cint_pppoe_basic_info.intf_nni), "create egress object ARP only");

    /*
     * 2.6. Create FEC and set its properties
     */
    BCM_GPORT_LOCAL_SET(gport, nni_port);
    int flags2 = 0;
    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
       flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, cint_pppoe_basic_info.fec_id_term, cint_pppoe_basic_info.intf_nni, cint_pppoe_basic_info.arp_id_term, gport, 0, flags2),
        "create egress object FEC only");

    /*
     * 2.7. Add IPv4 Host entries
     */
    if(*dnxc_data_get(unit, "l3", "fwd", "host_entry_support", NULL))
    {
        BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(unit, cint_pppoe_basic_info.host_uni2nni,
                cint_pppoe_basic_info.vrf,
                cint_pppoe_basic_info.fec_id_term, 0, 0), "");
    }
    else
    {
        BCM_IF_ERROR_RETURN_MSG(add_route_ipv4(unit, cint_pppoe_basic_info.host_uni2nni, 0xffffffff,
                cint_pppoe_basic_info.vrf,
                cint_pppoe_basic_info.fec_id_term), "with full mask");
    }

    /*
     * 2.8. Add IPv4 Route entry
     */
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv4(unit, cint_pppoe_basic_info.dip_uni2nni,
            cint_pppoe_basic_info.mask_uni2nni,
            cint_pppoe_basic_info.vrf,
            cint_pppoe_basic_info.fec_id_term), "");

    /*
     * 2.9. Add IPv6 Route entry
     */
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, cint_pppoe_basic_info.dip6_uni2nni,
            cint_pppoe_basic_info.mask6_uni2nni,
            cint_pppoe_basic_info.vrf,
            cint_pppoe_basic_info.fec_id_term), "");

    /*
     * 3. Set trap bcmRxTrapUcLooseRpfFail to drop packet
     */
    BCM_IF_ERROR_RETURN_MSG(pppoe_ip_rpf_config_traps(unit, urpf_mode), "");

    return BCM_E_NONE;
}
