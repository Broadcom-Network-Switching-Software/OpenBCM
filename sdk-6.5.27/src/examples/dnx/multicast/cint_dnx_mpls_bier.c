/* 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File: cint_dnx_mpls_bier.c
 * Purpose: Example of a MPLS BIER application. 
 */
/*
 * Test Scenario 
 *
 * Test Scenario - start
  config add bier_nof_bfr_entries.0=8192
 *
  config add multicast_nof_egress_bitmap.0=0x1000
 *
  config add multicast_ingress_group_id_range_max.0=0xa000
 *
  config add multicast_egress_group_id_range_max.0=0xa000
 *
  config add tm_port_header_type_in_50.0=RCH_0
 * or IBCH1_MODE, if supported
 *
  config add tm_port_header_type_out_50.0=ETH
 *
  config add ucode_port_50.0=RCY.10:core_0.50
 *
  0: tr 141 NoInit=1
 *
  config add custom_feature_init_verify=1
 *
  config delete custom_feature_multithread_en*
 *
  config delete custom_feature_kbp_multithread_en*
 *
  0: tr 141 NoDeinit=1
 *
  cint ../../../../src/examples/dnx/field/cint_field_utils.c
  cint ../../../../src/examples/sand/utility/cint_dnx_utils_global.c
  cint ../../../../src/examples/sand/utility/cint_dnx_utils_l3.c
  cint ../../../../src/examples/sand/utility/cint_dnx_utils_mpls.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_util_rch.c
  cint ../../../../src/examples/sand/cint_ip_route_basic.c
  cint ../../../../src/examples/sand/cint_ipmc_route_basic.c
  cint ../../../../src/examples/dnx/mpls/cint_dnx_mpls_mldp.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_utility_mpls.c
  cint ../../../../src/examples/dnx/multicast/cint_dnx_mpls_bier.c
  cint ../../../../src/examples/dnx/field/cint_field_epmf_cs_outlif_profile.c
  cint
  mpls_bier_example(0,1,3,200,201,202,203);
  exit;
 *
 *  BIER over MPLS over ethernet packet 
  tx 1 psrc=200 data=0x001100000077001100000055884700258140503000000004000000000000000000000000000000000000000000000000000000000000000000c04500004000000000800044ae0a0a0a02e0020202c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *  Received packets on unit 0 should be: 
 *  Source port: 0, Destination port: 0 
 *  Data: 0x0011000000660011000000778100105088470444323f0333323f0444223f0333223f0444123f0333123f001f413f503000000004000000000000000000000000000000000000000000000000000000000000000000404500004000000000800044ae0a0a0a02e0020202c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5 
 *  Source port: 0, Destination port: 0 
 *  Data: 0x00110000ce030011000000778100001e08004500004000000000fe00c6ad0a0a0a02e0020202c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5 
 *  Source port: 0, Destination port: 0 
 *  Data: 0x00110000ce040011000000778100002808004500004000000000fe00c6ad0a0a0a02e0020202c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5 
 *
  config delete bier_nof_bfr_entries.0
 *
  config delete multicast_nof_egress_bitmap.0
 *
  config delete multicast_ingress_group_id_range_max.0
 *
  config delete multicast_egress_group_id_range_max.0
 *
  config delete tm_port_header_type_in_50.0
 *
  config delete tm_port_header_type_out_50.0
 *
  config delete ucode_port_50.0
 *
  0: tr 141 NoInit=1
 *
  config add custom_feature_init_verify=1
 *
  config delete custom_feature_multithread_en*
 *
  config delete custom_feature_kbp_multithread_en*
 *
  0: tr 141 NoDeinit=1
 * Test Scenario - end
 *
 */

/* 
 *  BIER diagram:
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  |                                            .  .  . . .      . . .   . . .                                            |
 *  |                                          .            .  .       .       .                                           |
 *  |                                       .                                   .                                          |                       
 *  |                                      .                BIER domain          . .                                       |
 *  |                                    .   /+-----+  -   -   -   -   -   -+-----+  .                                     |             
 *  |   +--------+                     .      |BFR-3| \                 /   |BFR-4| \                                      | 
 *  |  /        /|                    .   /   |     |                       |     | \  .                                   | 
 *  | +--------+ |                   .        +-----+   \             /     +-----+   \  .                                 |
 *  | |  CE1   | |\                 .  /       / |                             |      \   .                                |
 *  | |        |/  \    +--------+ .                      \         /                   \   .+--------+\                   |
 *  | +--------+       /        /| /        /    |                             |       \    /        /|                    |
 *  |                \+--------+ |                          \     /                       \+--------+ |  \                 |
 *  |                 | BFR-1  | |      /        |                             |        \  | BFR-7  | |                    |
 *  |                 |        |/ \                          \ /                           |        |/    \                |
 *  |                 +--------+     /           |                             |         \/+--------+                      |
 *  |                               \                        / \                                    \       \              |
 *  |                             /              |                             |        / \          \       +--------+    |
 *  |                   +--------+    \                    /     \                         +--------+ \     /        /|    |
 *  |                  /        /|\              |                             |      /   /        /|  \   +--------+ |    |
 *  |                 +--------+ |      \                /         \                     +--------+ |   \  |  CE3   | |    |
 *  |   +--------+    |  BFR-2 | |. \            |                             |    /    | BFR-8  | |    \ |        |/     |
 *  |  /        /|    |        |/  .      \            /             \                   |        |/      \+--------+      |
 *  | +--------+ |  / +--------+    . \          |                             |  /     /+--------+        \               |
 *  | |  CE2   | | /                 .      \+-----+  /                 \   +-----+       .                 \              |
 *  | |        |/                      .\    |BFR-5|                        |BFR-6|  /                   +----------+      |
 *  | +--------+ /                      . \  |     |/                     \ |     |    .                /          /|      |
 *  |                                   .   \+-----+  -   -   -   -   -  -  +-----+/ .                 +----------+ |      |
 *  |                                     .                                   . .  .                   |   CE4    | |      |
 *  |                                      .               . .       .      .                          |          |/       |
 *  |                                        .  .   .  .  .   .  .  . . . .                            +----------+        |
 *  |                                                                                                                      |
 *  |                                                                                                                      |
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

/*
 * MPLS BIER configuration steps:
 * 
 * 1. Create ingress BFR:
 *      Send a packet into BIER domain, there should be 2 copies generated:
 *        - copy to BFR3
 *        - copy to BFR5
 *      1st PASS, packet is encapsulated BIER header,sent to RCY port
 *      2nd PASS, BIER header will be used for replication.
 *      In this case, BFIR actually acts as BFR at the same time.
 *      1.1) Create out RIF for BFR3 and BFR5.
 *      1.2) Create ARP for BFR3 and BFR5
 *      1.3) Create in RIF on BFR1 port connected to CE1
 *      1.4) Create egress LSP tunnels to BFR3 and BFR5.
 *      1.5) Create overlay entry for 2nd PASS
 *      1.6) Create BIFT MPLS entry assigned by BFR3, next pointer is LSP tunnel.
 *      1.7) Create BIFT MPLS entry assigned by BFR5, next pointer is LSP tunnel.
 *      1.8) Create BIFT MPLS entry assigned by BFR1, next pointer is overlay entry.
 *      1.8) Create BIER header entry , next pointer is BFR1 BIFT MPLS entry.
 *      1.9) Create native multicast group with 1 replications:
 *           - cud = BIER header , Destination = RCY port
 *      1.10) Create BIER multicast group with 2 replications:
 *           - cud = BIFT MPLS entry to BFR3, Destination = Port to BFR3
 *           - cud = BIFT MPLS entry to BFR5, Destination = Port to BFR5
 *      1.11) Set ntavie forwarding(IPv4/IPv6/MPLS/Eth) entry, destination is MC ID in 1.9
 *      1.12) Set ILM entry for BFR1 BIFT label, destination is MCID in 1.10
 * 2. Create egress BUD BFR:
 *      Send a packet with  BIER header to BFR7, there should be 3 copies generated:
 *        - continue copy to BFR6, with BIER header and bitstr is updated;
 *        - drop copy to RCY port, at 2nd PASS, BIER header is terminated, packet is forwarded according
 *        to native forward header. Finally 2 copies generated to CE3 and CE4.
 *      2.1) Create out RIF for CE3 and CE4.
 *      2.2) Create out RIF for BFR6
 *      2.3) Create ARP for CE3 and CE4
 *      2.4) Create ARP for BFR6
 *      2.5) Create in RIF on BFR7 port connected to BFR4.
 *      2.6) Create egress LSP tunnels to BFR6.
 *      2.7) Create overlay entry for 2nd PASS
 *      2.8) Create BIFT header entry from BFR7 to BFR6, next pointer is LSP tunnel.
 *      2.9) Create BIFT header entry for BFR7 drop copy, next pointer is overlay entry
 *      2.10) Create BIER multicast group with 2 replications:
 *           - cud = BIFT-tunnel to BFR6, Destination = port to BFR5
 *           - cud = BIFT-tunnel to BFR7 drop copy , Destination = RCY port
 *      2.11) Create native multicast group with 2 replications:
 *           - cud = ARP to CE3, Destination = Port to CE3
 *           - cud = ARP to CE4, Destination = Port to CE4
 *      2.12) Set 2ND Pass BIFT termination 
 *      2.13) Set ntavie forwarding(IPv4/IPv6/MPLS/Eth) entry, destination is MC ID in 2.10
 *      2.14) Set ILM entry for BFR7 BIFT label, destination is MCID in 2.9
 */

enum bier_bfr_type_defines_e {
    BFIR     = 0,
    BFER     = 1,
    BFR      = 2
};

enum bier_payload_type_defines_e {
    ipv4     = 0,
    ipv6     = 1,
    mpls     = 2,
    ethernet = 3
};


struct cint_bier_info_s {
    
    /*
     * {
     * Overlay network data
     */

    int ce1_port; /* ce1 port (access)*/
    int ce3_port; /* ce3 port (access) */
    int ce4_port; /* ce4 port (access) */

    int bfr3_port;  /* bfr-3 port (provider) */
    int bfr4_port;  /* bfr-4 port (provider) */
    int bfr5_port;  /* bfr-5 port (provider) */
    int bfr6_port;  /* bfr-6 port (provider) */
    int bfr7_port;  /* bfr-7 port (provider) */
    int bfr8_port;  /* bfr-8 port (provider) */

    int ce1_vlan; /* ce1's vlan */
    int ce3_vlan; /* ce3's vlan */
    int ce4_vlan; /* ce4's vlan */

    int bfr3_vlan;  /* bfr-3's vlan */
    int bfr4_vlan;  /* bfr-4's vlan */
    int bfr5_vlan;  /* bfr-5's vlan */
    int bfr6_vlan;  /* bfr-6's vlan */
    int bfr7_vlan;  /* bfr-7's vlan */
    int bfr8_vlan;  /* bfr-8's vlan */

    uint32 bfr1_id; /* bfr-id for bfr-1 */
    uint32 bfr2_id; /* bfr-id for bfr-2 */
    uint32 bfr3_id; /* bfr-id for bfr-3 */
    uint32 bfr4_id; /* bfr-id for bfr-4 */
    uint32 bfr5_id; /* bfr-id for bfr-5 */
    uint32 bfr6_id; /* bfr-id for bfr-6 */
    uint32 bfr7_id; /* bfr-id for bfr-7 */
    uint32 bfr8_id; /* bfr-id for bfr-8 */


    int bfr1_bift_id; /* bift-id asssigned by bfr-1 */
    int bfr3_bift_id; /* bift-id asssigned by bfr-3 */
    int bfr4_bift_id; /* bift-id asssigned by bfr-4 */
    int bfr5_bift_id; /* bift-id asssigned by bfr-5 */
    int bfr6_bift_id; /* bift-id asssigned by bfr-6 */
    int bfr7_bift_id; /* bift-id asssigned by bfr-7 */
    int bfr8_bift_id; /* bift-id asssigned by bfr-8 */

    bcm_mac_t bfr1_mac; /* global mac of BFR-1 */
    bcm_mac_t bfr3_mac; /* global mac of BFR-3 */
    bcm_mac_t bfr4_mac; /* global mac of BFR-4 */
    bcm_mac_t bfr5_mac; /* global mac of BFR-5 */
    bcm_mac_t bfr6_mac; /* global mac of BFR-6 */
    bcm_mac_t bfr7_mac; /* global mac of BFR-7 */
    bcm_mac_t bfr8_mac; /* global mac of BFR-8 */

    bcm_mac_t ce3_mac;  /* ce1's mac address*/
    bcm_mac_t ce4_mac; /* ce2's mac address*/

    bcm_multicast_t native_mc_id;            /* multicast id for native mc*/    
    bcm_multicast_t bier_mc_id;            /* multicast id for bier*/    

    /*
     * Overlay network data
     * }
     */


    /*
     * {
     * Native forwarding data
     */

    int vrf;            /* vrf*/    
    bcm_ip_t dip;                           /* dip */
    bcm_ip_t sip;                           /* sip */
    bcm_ip6_t ipv6_dip;                     /* ipv6_sip */
    bcm_ip6_t ipv6_sip;                     /* ipv6_dip */
    int native_mpls_label;                  /*native_mpls_label*/
    bcm_mac_t native_dmac;                  /*native_dmac*/

    int      rcy_port_id;                   /*rcy port id*/
    bcm_gport_t mpls_port_id;               /* global lif encoded as MPLS port */

    /*
     * Native forwarding data
     * }
     */


    bcm_gport_t bfr1_bfr3_bift_tunnel_id; /* BIFT tunnel id of bfr1---->bfr3 */
    bcm_gport_t bfr1_bfr5_bift_tunnel_id; /* BIFT tunnel id of bfr1---->bfr5 */
    bcm_gport_t bfr7_bfr6_bift_tunnel_id; /* BIFT tunnel id of bfr7---->bfr6 */
    bcm_gport_t bfr4_bfr6_bift_tunnel_id; /* BIFT tunnel id of bfr4---->bfr6 */
    bcm_gport_t bfr4_bfr7_bift_tunnel_id; /* BIFT tunnel id of bfr4---->bfr7 */
    bcm_gport_t bfr4_bfr8_bift_tunnel_id; /* BIFT tunnel id of bfr4---->bfr8 */

    bcm_gport_t bfr7_drop_copy_bift_tunnel_id; /* BIFT tunnel id for drop copy on BFR7 */
    bcm_gport_t bfr1_to_2nd_pass_bift_tunnel_id; /* BIFT tunnel id for 2nd PASS on BFR1 */

    bcm_gport_t bfr1_bfr3_lsp_tunnel_id; /* Tunnel id of bfr1---->bfr3 */
    bcm_gport_t bfr1_bfr5_lsp_tunnel_id; /* Tunnel id of bfr1---->bfr5 */
    bcm_gport_t bfr7_bfr6_lsp_tunnel_id; /* Tunnel id of bfr7---->bfr6 */
    bcm_gport_t bfr4_bfr6_lsp_tunnel_id; /* Tunnel id of bfr4---->bfr6 */
    bcm_gport_t bfr4_bfr7_lsp_tunnel_id; /* Tunnel id of bfr4---->bfr7 */
    bcm_gport_t bfr4_bfr8_lsp_tunnel_id; /* Tunnel id of bfr4---->bfr8 */

    bcm_gport_t bfr7_ce3_swap_tunnel_id; /* Tunnel id of bfr7--->ce3 */
    bcm_gport_t bfr7_ce4_swap_tunnel_id; /* Tunnel id of bfr7--->ce4 */
    
    bcm_if_t    bfr1_bfr3_arp_id;         /* arp id of bfr1---->bfr3 */
    bcm_if_t    bfr1_bfr5_arp_id;         /* arp id of bfr1---->bfr5 */
    bcm_if_t    bfr7_bfr6_arp_id;         /* arp id of bfr7---->bfr6 */
    bcm_if_t    bfr7_ce3_arp_id;          /* arp id of bfr7---->ce3 */
    bcm_if_t    bfr7_ce4_arp_id;          /* arp id of bfr7---->ce4 */
    bcm_if_t    bfr4_bfr6_arp_id;         /* arp id of bfr4---->bfr6 */
    bcm_if_t    bfr4_bfr7_arp_id;         /* arp id of bfr4---->bfr7 */
    bcm_if_t    bfr4_bfr8_arp_id;         /* arp id of bfr4---->bfr8 */

    bcm_if_t    overlay_entry_id;        /*rcy entry id on BUD node BFR7*/
    int         bfr1_bift_term_id;       /*tunnel ID for BIFT termiation on node BFR1*/
    int         bfr4_bift_term_id;       /*tunnel ID for BIFT termiation on node BFR4*/
    int         bfr7_bift_term_id;       /*tunnel ID for BIFT termiation on node BFR7*/
    int         bfr1_bift_initator_id;   /*tunnel ID for BIFT initiator on node BFR1*/
    bcm_if_t    app_mpls_tunnel_id;      /*tunnel ID for app mpls tunnel */
    bcm_gport_t    app_mpls_term_id;        /*In LIF for app label*/
    int dscp;                            /* BIER dscp */
};



cint_bier_info_s cint_bier_info = 
/* ports :
   ce1_port | ce3_port | ce4_port | bfr3_port | bfr4_port | bfr5_port | bfr6_port  bfr7_port | bfr8_port*/
    {200,      201,        202 ,     203,       200,         200,        200,       201,          202,        
/* vlans:
   ce1_vlan | ce3_vlan | ce4_vlan | bfr3_vlan | bfr4_vlan | bfr5_vlan | bfr6_vlan | bfr7_vlan | bfr8_vlan*/
    20,          30,        40,        50,      60,          70,           80,        90,         100,
/* BFR-IDs:
   bfr1_id  | bfr2_id  | bfr3_id  | bfr4_id   | bfr5_id   | bfr6_id   | bfr7_id | bfr8_id*/
     1,          2,        3,          4,         5,           6,         7,        8,
/* bfr1_bift_id    |  bfr3_bift_id | bfr4_bift_id |  bfr5_bift_id |  bfr6_bift_id |  bfr7_bift_id | bfr8_bift_id*/ 
     300,               200,            700,            400,             500,           600,          800,

/*               bfr1_mac                |            bfr3_mac                 |               bfr4_mac                | */              
    {0x00, 0x11, 0x00, 0x00, 0x00, 0x11}, {0x00, 0x11, 0x00, 0x00, 0x00, 0x33}, {0x00, 0x11, 0x00, 0x00, 0x00, 0x44},
/*               bfr5_mac                |            bfr6_mac                 |               bfr7_mac                | */              
    {0x00, 0x11, 0x00, 0x00, 0x00, 0x55}, {0x00, 0x11, 0x00, 0x00, 0x00, 0x66}, {0x00, 0x11, 0x00, 0x00, 0x00, 0x77},
/*               bfr8_mac                |            ce3_mac                  |               ce4_mac                 | */              
    {0x00, 0x11, 0x00, 0x00, 0x00, 0x88},{0x00, 0x11, 0x00, 0x00, 0xce, 0x03},  {0x00, 0x11, 0x00, 0x00, 0xce, 0x04},

/* native_mc_id    |  bier_mc_id*/ 
     6000,               0x1800,          

/* vrf  */ 
    1,

/* dip  224.2.2.2 */
0xE0020202,
/* sip  10.10.10.2*/
0x0A0A0A02,

/*
 * ipv6_dip
 */
{0xFF, 0xFF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x11, 0x11, 0x22, 0x22},
/*
 * ipv6_sip
 */
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x11, 0x11, 0x33, 0x33},

/* native_mpls_label*/
6800,
/* native_dmac*/
 {0x01, 0x00, 0x5e, 0x00, 0x00, 0x11},
/* rcy_port_id  */
50,
/*mpls_port_id*/
38888,
};


/**
 * port init
 * INPUT:
 *   node_type    - node type 
*/
 int
 mpls_bier_port_configuration(
    int unit,
    int node_type)
{

    if (node_type == BFIR)
    {
        BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, cint_bier_info.ce1_port, cint_bier_info.ce1_vlan), "ce1 port");

        BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, cint_bier_info.bfr3_port, cint_bier_info.bfr3_vlan), "bfr3 port");

        BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, cint_bier_info.bfr5_port, cint_bier_info.bfr5_vlan), "bfr5 port");


    }
    else if (node_type == BFER)
    {
        BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, cint_bier_info.bfr4_port, cint_bier_info.bfr4_vlan), "bfr4 port");

        BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, cint_bier_info.bfr6_port, cint_bier_info.bfr6_vlan), "bfr6 port");

        BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, cint_bier_info.ce3_port, cint_bier_info.ce3_vlan), "ce3 port");

        BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, cint_bier_info.ce4_port, cint_bier_info.ce4_vlan), "ce4 port");


    }
    else
    {
        BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, cint_bier_info.bfr3_port, cint_bier_info.bfr3_vlan), "bfr3 port");

        BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, cint_bier_info.bfr6_port, cint_bier_info.bfr6_vlan), "bfr6 port");

        BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, cint_bier_info.bfr7_port, cint_bier_info.bfr7_vlan), "bfr7 port");

        BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, cint_bier_info.bfr8_port, cint_bier_info.bfr8_vlan), "bfr8 port");
    }

    return BCM_E_NONE;

}

/**
 * L3 intf init
 * INPUT:
 *   unit    - traffic incoming unit 
*/
 int
 mpls_bier_l3_intf_configuration(
    int unit,
    int node_type)
{
    int rv;
    bcm_l3_intf_t l3_intf;
    dnx_utils_l3_eth_rif_s eth_rif_structure;


    if (node_type == BFIR)
    {
        bcm_l3_intf_t_init(&l3_intf);
        l3_intf.l3a_intf_id = cint_bier_info.ce1_vlan;
        rv = bcm_l3_intf_get(unit,&l3_intf);
        if (rv != BCM_E_NONE) {
            dnx_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_bier_info.ce1_vlan, 0, 0, cint_bier_info.bfr1_mac, cint_bier_info.vrf);
            BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_eth_rif_create(unit, &eth_rif_structure), "ce1 rif");
        }

        bcm_l3_intf_t_init(&l3_intf);
        l3_intf.l3a_intf_id = cint_bier_info.bfr3_vlan;
        rv = bcm_l3_intf_get(unit,&l3_intf);
        if (rv != BCM_E_NONE) {
            dnx_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_bier_info.bfr3_vlan, 0, 0, cint_bier_info.bfr1_mac, cint_bier_info.vrf);
            BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_eth_rif_create(unit, &eth_rif_structure), "bfr3 rif");
        }

        bcm_l3_intf_t_init(&l3_intf);
        l3_intf.l3a_intf_id = cint_bier_info.bfr5_vlan;
        rv = bcm_l3_intf_get(unit,&l3_intf);
        if (rv != BCM_E_NONE) {
            dnx_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_bier_info.bfr5_vlan, 0, 0, cint_bier_info.bfr1_mac, cint_bier_info.vrf);
            BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_eth_rif_create(unit, &eth_rif_structure), "bfr5 rif");
        }


    }
    else if (node_type == BFER)
    {
        bcm_l3_intf_t_init(&l3_intf);
        l3_intf.l3a_intf_id = cint_bier_info.bfr4_vlan;
        rv = bcm_l3_intf_get(unit,&l3_intf);
        if (rv != BCM_E_NONE) {
            dnx_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_bier_info.bfr4_vlan, 0, 0, cint_bier_info.bfr7_mac, cint_bier_info.vrf);
            BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_eth_rif_create(unit, &eth_rif_structure), "ce1 rif");
        }
        bcm_l3_intf_t_init(&l3_intf);
        l3_intf.l3a_intf_id = cint_bier_info.bfr6_vlan;
        rv = bcm_l3_intf_get(unit,&l3_intf);
        if (rv != BCM_E_NONE) {
            dnx_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_bier_info.bfr6_vlan, 0, 0, cint_bier_info.bfr7_mac, cint_bier_info.vrf);
            BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_eth_rif_create(unit, &eth_rif_structure), "bfr3 rif");
        }

        bcm_l3_intf_t_init(&l3_intf);
        l3_intf.l3a_intf_id = cint_bier_info.ce3_vlan;
        rv = bcm_l3_intf_get(unit,&l3_intf);
        if (rv != BCM_E_NONE) {
            dnx_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_bier_info.ce3_vlan, 0, 0, cint_bier_info.bfr7_mac, cint_bier_info.vrf);
            BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_eth_rif_create(unit, &eth_rif_structure), "bfr5 rif");
        }

        bcm_l3_intf_t_init(&l3_intf);
        l3_intf.l3a_intf_id = cint_bier_info.ce4_vlan;
        rv = bcm_l3_intf_get(unit,&l3_intf);
        if (rv != BCM_E_NONE) {
            dnx_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_bier_info.ce4_vlan, 0, 0, cint_bier_info.bfr7_mac, cint_bier_info.vrf);
            BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_eth_rif_create(unit, &eth_rif_structure), "bfr5 rif");
        }
    }
    else
    {
        bcm_l3_intf_t_init(&l3_intf);
        l3_intf.l3a_intf_id = cint_bier_info.bfr3_vlan;
        rv = bcm_l3_intf_get(unit,&l3_intf);
        if (rv != BCM_E_NONE) {
            dnx_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_bier_info.bfr3_vlan, 0, 0, cint_bier_info.bfr4_mac, cint_bier_info.vrf);
            BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_eth_rif_create(unit, &eth_rif_structure), "ce1 rif");
        }
        bcm_l3_intf_t_init(&l3_intf);
        l3_intf.l3a_intf_id = cint_bier_info.bfr6_vlan;
        rv = bcm_l3_intf_get(unit,&l3_intf);
        if (rv != BCM_E_NONE) {
            dnx_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_bier_info.bfr6_vlan, 0, 0, cint_bier_info.bfr4_mac, cint_bier_info.vrf);
            BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_eth_rif_create(unit, &eth_rif_structure), "bfr3 rif");
        }

        bcm_l3_intf_t_init(&l3_intf);
        l3_intf.l3a_intf_id = cint_bier_info.bfr7_vlan;
        rv = bcm_l3_intf_get(unit,&l3_intf);
        if (rv != BCM_E_NONE) {
            dnx_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_bier_info.bfr7_vlan, 0, 0, cint_bier_info.bfr4_mac, cint_bier_info.vrf);
            BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_eth_rif_create(unit, &eth_rif_structure), "bfr3 rif");
        }

        bcm_l3_intf_t_init(&l3_intf);
        l3_intf.l3a_intf_id = cint_bier_info.bfr8_vlan;
        rv = bcm_l3_intf_get(unit,&l3_intf);
        if (rv != BCM_E_NONE) {
            dnx_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_bier_info.bfr8_vlan, 0, 0, cint_bier_info.bfr4_mac, cint_bier_info.vrf);
            BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_eth_rif_create(unit, &eth_rif_structure), "bfr3 rif");
        }



    }
    return BCM_E_NONE;
}


/**
 * forward entry adding(IPv4 Route)
 * INPUT:
 *   unit    - traffic incoming unit 
*/
 int
 mpls_bier_forward_entry_configuration(
    int unit,
    int node_type,
    int payload,
    int over_mpls)
{
    int l3_mc_id;
    bcm_mpls_tunnel_switch_t entry;
    bcm_ip6_t full_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    bcm_ip6_t full_mask_zero = {0};
    bcm_ip6_t part_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0, 0, 0, 0, 0, 0, 0, 0};
    uint32 compressed_vrf = *dnxc_data_get(unit, "l3", "fwd", "ipmc_config_cmprs_vrf_group", NULL);
    uint16 compressed_sip = 123;
    bcm_l2_addr_t l2add;
    uint32 l2_flags, learn_strength;
    uint32 separate_fwd_learn_mact = *dnxc_data_get(unit, "l2", "general", "separate_fwd_learn_mact", NULL);
  

    bcm_mpls_tunnel_switch_t_init(&entry);
    
    /*
     * 1. Add ILM entry for BIER
     */
    
    /** BIFT label */
    if (node_type == BFER)
    {
        entry.label = cint_bier_info.bfr7_bift_id;
    }
    else if (node_type == BFIR)
    {
        entry.label = cint_bier_info.bfr1_bift_id;
    }
    else 
    {
        entry.label = cint_bier_info.bfr4_bift_id;
    }
    entry.mc_group = cint_bier_info.bier_mc_id;
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT | BCM_MPLS_SWITCH_P2MP;
    entry.action = BCM_MPLS_SWITCH_ACTION_NOP;
    BCM_IF_ERROR_RETURN_MSG(bcm_mpls_tunnel_switch_create(unit, &entry), "bier ILM");
   
    if (node_type == BFR)
    {
        return BCM_E_NONE;
    } 
    
    /*
     * 2. Add IPv4 IPMC entry
     */
    bcm_if_t in_lif = 0;
    if (node_type == BFER)
    {
        if(payload == mpls)
        {
            BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(in_lif,cint_bier_info.app_mpls_term_id);
        }
        else
        {
            BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(in_lif, cint_bier_info.bfr7_bift_term_id);
        }
    }
    else
    {
        in_lif = cint_bier_info.ce1_vlan;
    }

    /*
     * For mpls payload, it acutally
     * encap IPvX/ETH payload with 
     * MPLS APP label, and then
     * in BIER
     **/
    if (payload == ipv4)
    {
        BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc(unit, cint_bier_info.dip, 0xFFFFFFFF, cint_bier_info.sip, 0xFFFFFFFF,
                           in_lif, cint_bier_info.vrf, cint_bier_info.native_mc_id, 0, 0), "IPv4 MC");
        
    }
    else if (payload == mpls)
    {
        if (over_mpls) 
        {
            BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc(unit, cint_bier_info.dip, 0xFFFFFFFF, cint_bier_info.sip, 0xFFFFFFFF,
                               in_lif, cint_bier_info.vrf, cint_bier_info.native_mc_id, 0, 0), "IPv4 MC");
        }
        else
        {
            bcm_mpls_tunnel_switch_t_init(&entry);
            entry.label = cint_bier_info.native_mpls_label;
            entry.mc_group = cint_bier_info.native_mc_id;
            entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT | BCM_MPLS_SWITCH_P2MP;
            entry.action = BCM_MPLS_SWITCH_ACTION_NOP;
            BCM_IF_ERROR_RETURN_MSG(bcm_mpls_tunnel_switch_create(unit, &entry), "");
        }
        
    }
    else if(payload == ipv6)
    {
        BCM_IF_ERROR_RETURN_MSG(add_ipv6_ipmc(unit, cint_bier_info.ipv6_dip, full_mask, cint_bier_info.ipv6_sip, part_mask, 
                           in_lif, cint_bier_info.vrf, compressed_vrf?compressed_sip:0, compressed_vrf?0xFFFF:0,
                           cint_bier_info.native_mc_id, 0, 0, 0), "ipv6 mc");
    }
    else
    {
        bcm_l2_addr_t_init(&l2add, cint_bier_info.native_dmac, (node_type == BFER)?cint_bier_info.vrf:cint_bier_info.ce1_vlan);
        l2_flags = separate_fwd_learn_mact ? 0 : BCM_L2_STATIC;
        learn_strength = separate_fwd_learn_mact ? 1 : 0;
        l2add.l2mc_group = cint_bier_info.native_mc_id;
        l2add.flags = l2_flags | BCM_L2_MCAST;
        l2add.learn_strength = learn_strength;
        BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, &l2add), "");
    }    

    return BCM_E_NONE;
}


/**
 * MPLS APP label adding, only for ETH or IPvX payload
 * INPUT:
 *   unit    - traffic outgoing unit 
*/
int
mpls_bier_mpls_app_label_configure(
    int unit,
    int node_type,
    int payload)
{
    bcm_mpls_egress_label_t label_array[2];
    bcm_mpls_port_t mpls_port;
    bcm_gport_t gport_id;
    bcm_mpls_tunnel_switch_t entry;
    int propogation_profile_flags = BCM_MPLS_SWITCH_OUTER_EXP | BCM_MPLS_SWITCH_OUTER_TTL;
    uint32 estimate_encap_size_required =
        *dnxc_data_get(unit, "lif", "out_lif", "global_lif_allocation_estimated_encap_size_enable", NULL);

    if (node_type == BFIR)
    {
        if (payload == mpls)
        {
            bcm_mpls_egress_label_t_init(&label_array[0]);
            label_array[0].label = cint_bier_info.native_mpls_label;
            label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
            BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(label_array[0].l3_intf_id, cint_bier_info.bfr1_bift_initator_id);
            label_array[0].encap_access = bcmEncapAccessNativeArp;
            label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelPipeMyNameSpace;
            label_array[0].exp = 1;
            if(estimate_encap_size_required)
            {
                label_array[0].estimated_encap_size = 44 + 4;
            }
            BCM_IF_ERROR_RETURN_MSG(bcm_mpls_tunnel_initiator_create(unit, 0, 1, label_array), "");
            cint_bier_info.app_mpls_tunnel_id = label_array[0].tunnel_id;
        }
        else if(payload == ethernet)
        {
            bcm_mpls_port_t_init(&mpls_port);
            mpls_port.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL;
            mpls_port.flags2 |= BCM_MPLS_PORT2_EGRESS_ONLY;
            BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0, cint_bier_info.mpls_port_id);
            BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, gport_id);
            mpls_port.egress_label.label = cint_bier_info.native_mpls_label;
            mpls_port.egress_label.encap_access = bcmEncapAccessNativeArp;
            if(estimate_encap_size_required)
            {
                mpls_port.egress_label.estimated_encap_size = 44 + 4;
            }
            mpls_port.egress_label.flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
            BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(mpls_port.egress_tunnel_if,cint_bier_info.bfr1_bift_initator_id);
            BCM_IF_ERROR_RETURN_MSG(bcm_mpls_port_add(unit, 0, mpls_port), "");
            cint_bier_info.app_mpls_tunnel_id = mpls_port.encap_id;
        }
    }
    else if (node_type == BFER)
    {
        if(payload == mpls)
        {
            bcm_mpls_tunnel_switch_t_init(&entry);
            
            entry.action = BCM_MPLS_SWITCH_ACTION_POP;
            entry.label = cint_bier_info.native_mpls_label;
            entry.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
            entry.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
            entry.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;
            entry.vpn = cint_bier_info.vrf;
            BCM_IF_ERROR_RETURN_MSG(bcm_mpls_tunnel_switch_create(unit, &entry), "");
            cint_bier_info.app_mpls_term_id = entry.tunnel_id;

        }
        else if(payload == ethernet)
        {
            bcm_mpls_port_t_init(&mpls_port);
            mpls_port.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL;
            mpls_port.flags2 |= BCM_MPLS_PORT2_INGRESS_ONLY ;
            mpls_port.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
            mpls_port.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
            mpls_port.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;
            BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0, cint_bier_info.mpls_port_id);
            BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, gport_id);
            mpls_port.criteria = BCM_MPLS_PORT_MATCH_LABEL;
            mpls_port.match_label = cint_bier_info.native_mpls_label;
            mpls_port.port = cint_bier_info.bfr4_port;
            BCM_IF_ERROR_RETURN_MSG(bcm_mpls_port_add(unit, cint_bier_info.vrf, mpls_port), "");

        }
    }

    return BCM_E_NONE;
}

 int
 mpls_bier_multicast_group_configuration(
    int unit,
    int node_type,
    int payload,
    int mode,
    int over_mpls)
{
    bcm_multicast_replication_t replications[3];
    uint32   bfr_id[3];
    int rep_count = 2;
    uint32 no_egress_bier_process = *dnxc_data_get(unit, "bier", "params", "ingress_only", NULL);
    uint32 bit_str_in_mcdb = *dnxc_data_get(unit, "bier", "params", "bfr_bit_str_in_mcdb", NULL);
    uint32 multicast_v2_format = *dnxc_data_get(unit, "multicast", "params", "mcdb_formats_v2", NULL);
    uint32 static_flag = (multicast_v2_format == 1)?BCM_MULTICAST_ELEMENT_STATIC:0;

    /*
     * mode: 
     *     0: fabric+egress
     *     1: Ingress only
     *     2: Ingress+Egress
     */
    if (*dnxc_data_get(unit, "fabric", "general", "blocks_exist", NULL)
        &&*dnxc_data_get(unit, "device", "general", "hw_support", NULL)
        && (mode == 0))
    { 
        int nof_cores = *dnxc_data_get(unit, "device", "general", "valid_cores_bitmap", NULL);
        bcm_module_t mreps[nof_cores];
        uint32 valid_cores_bmp = *(dnxc_data_get(unit, "device", "general", "valid_cores_bitmap", NULL));
        int nof_copies = 0;
        int core_index;
        for(core_index = 0; core_index < nof_cores; core_index++) {
            if ( valid_cores_bmp & (1 << (core_index))){
                mreps[nof_copies]= core_index;
                nof_copies ++;
            }
        }
        BCM_IF_ERROR_RETURN_MSG(bcm_fabric_multicast_set(unit, cint_bier_info.native_mc_id, 0, nof_copies, mreps), "");
        BCM_IF_ERROR_RETURN_MSG(bcm_fabric_multicast_set(unit, cint_bier_info.bier_mc_id, 0, nof_copies, mreps), "");

    }

    if (node_type == BFIR)
    {
        uint32 flag = 0;

        flag = (no_egress_bier_process == 1)?BCM_MULTICAST_INGRESS_GROUP:BCM_MULTICAST_EGRESS_GROUP;
        /*
         * 1st pass, encapsulate BIER header
         */
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, flag|static_flag|BCM_MULTICAST_WITH_ID, &cint_bier_info.native_mc_id), "");

        /* 1st copy: to rcy*/
        bcm_multicast_replication_t_init(&replications[0]);
        if ((payload == mpls) || (payload == ethernet))
        {
            replications[0].encap1 = BCM_L3_ITF_VAL_GET(cint_bier_info.app_mpls_tunnel_id);
        }
        else
        {
            replications[0].encap1 = cint_bier_info.bfr1_bift_initator_id & 0x3fffff;
        }
        replications[0].port = cint_bier_info.rcy_port_id;

        /*MCDB setting*/
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_set(unit, cint_bier_info.native_mc_id, flag|static_flag, 1, replications), "");

        /*
         * 2nd PASS MC, for BIER copy
         */
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, flag|static_flag|BCM_MULTICAST_WITH_ID|BCM_MULTICAST_BIER_256_GROUP, &cint_bier_info.bier_mc_id), "");
        /* 1st copy: to BFR3*/
        bcm_multicast_replication_t_init(&replications[0]);
        bfr_id[0] = cint_bier_info.bfr3_id;
        replications[0].encap1 = BCM_L3_ITF_VAL_GET(cint_bier_info.bfr1_bfr3_bift_tunnel_id);
        replications[0].port = cint_bier_info.bfr3_port;
        
        /* 2nd copy: to ce4*/
        bcm_multicast_replication_t_init(&replications[1]);
        bfr_id[1] = cint_bier_info.bfr5_id;
        replications[1].encap1 = BCM_L3_ITF_VAL_GET(cint_bier_info.bfr1_bfr5_bift_tunnel_id);
        replications[1].port = cint_bier_info.bfr5_port;
        
        /*MCDB setting*/
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_bfr_id_add(unit, cint_bier_info.bier_mc_id, flag, 2, bfr_id, replications), "");
        

    }
    else if (node_type == BFER)
    {
        uint32 flag = 0;
        flag = (no_egress_bier_process == 1)?BCM_MULTICAST_INGRESS_GROUP:BCM_MULTICAST_EGRESS_GROUP;
        /*
         * 1st PASS MC, for BIER header
         */
       
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, BCM_MULTICAST_WITH_ID|flag|static_flag|BCM_MULTICAST_BIER_256_GROUP, &cint_bier_info.bier_mc_id), "");
        /* 1st(continue) copy: to BFR6*/
        bcm_multicast_replication_t_init(&replications[0]);
        bfr_id[0] = cint_bier_info.bfr6_id;
        replications[0].encap1 = BCM_L3_ITF_VAL_GET(cint_bier_info.bfr7_bfr6_bift_tunnel_id);;
        replications[0].port = cint_bier_info.bfr6_port;
        
        /* 2nd(drop) copy: to rcy*/
        bcm_multicast_replication_t_init(&replications[1]);
        bfr_id[1] = cint_bier_info.bfr7_id;
        replications[1].encap1 = BCM_L3_ITF_VAL_GET(cint_bier_info.bfr7_drop_copy_bift_tunnel_id);
        replications[1].port = cint_bier_info.rcy_port_id;
        
        /*MCDB setting*/
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_bfr_id_add(unit, cint_bier_info.bier_mc_id, flag, 2, bfr_id, replications), "");

        /*
         * 2nd PASS MC, for drop copy
         */
       
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, BCM_MULTICAST_WITH_ID|flag|static_flag, &cint_bier_info.native_mc_id), "");

        bcm_multicast_replication_t_init(&replications[0]);
        bcm_multicast_replication_t_init(&replications[1]);

        /* 1st copy: to ce3*/
        replications[0].port = cint_bier_info.ce3_port;
        /* 2nd copy: to ce4*/
        replications[1].port = cint_bier_info.ce4_port;

        if (payload != ethernet)
        {
            replications[0].encap1 = BCM_L3_ITF_VAL_GET(cint_bier_info.bfr7_ce3_arp_id);
            replications[1].encap1 = BCM_L3_ITF_VAL_GET(cint_bier_info.bfr7_ce4_arp_id);
        }
        

        /*MCDB setting*/
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_set(unit, cint_bier_info.native_mc_id, flag|static_flag, 2, replications), "");

    }
    /*
     * BFR
     */
    else 
    {

        if (!no_egress_bier_process)
        {
            BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, BCM_MULTICAST_WITH_ID|BCM_MULTICAST_EGRESS_GROUP|BCM_MULTICAST_BIER_256_GROUP, &cint_bier_info.bier_mc_id), "");
        }

        if (mode == 1)
        {
            /*
             * Ingress MC
             */

            BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, BCM_MULTICAST_WITH_ID|BCM_MULTICAST_INGRESS_GROUP|BCM_MULTICAST_BIER_256_GROUP|static_flag,
                                            &cint_bier_info.bier_mc_id), "");

            /* 1st(continue) copy: to BFR6*/
            bcm_multicast_replication_t_init(&replications[0]);
            bfr_id[0] = cint_bier_info.bfr6_id;
            replications[0].encap1 = !bit_str_in_mcdb?cint_bier_info.bfr4_bfr6_bift_tunnel_id&0x3FFFFF:(cint_bier_info.bier_mc_id + cint_bier_info.bfr6_id);
            replications[0].port = cint_bier_info.bfr6_port;
            
            /* 2nd(continue) copy: to BFR7*/
            bcm_multicast_replication_t_init(&replications[1]);
            bfr_id[1] = cint_bier_info.bfr7_id;
            replications[1].encap1 = !bit_str_in_mcdb?cint_bier_info.bfr4_bfr7_bift_tunnel_id&0x3FFFFF:(cint_bier_info.bier_mc_id + cint_bier_info.bfr7_id);
            replications[1].port = cint_bier_info.bfr7_port;
            
            /* 3rd(continue) copy: to BFR8*/
            bcm_multicast_replication_t_init(&replications[2]);
            bfr_id[2] = cint_bier_info.bfr8_id;
            replications[2].encap1 = !bit_str_in_mcdb?cint_bier_info.bfr4_bfr8_bift_tunnel_id&0x3FFFFF:(cint_bier_info.bier_mc_id + cint_bier_info.bfr8_id);
            replications[2].port = cint_bier_info.bfr8_port;
            
            /*MCDB setting*/
            BCM_IF_ERROR_RETURN_MSG(bcm_multicast_bfr_id_add(unit, cint_bier_info.bier_mc_id, BCM_MULTICAST_INGRESS_GROUP, 3, bfr_id, replications), "");

        }

        if (mode == 2)
        {
            /*
             * Ingress MC
             */
            bcm_port_interface_info_t interface_info;
            bcm_port_mapping_info_t mapping_info;
            uint32 flags = 0;
            int count_erp = 0;
            int erp_sysport_gport;
            int erp_port[2];
            int erp_sys_port[2];
            int i;

            BCM_IF_ERROR_RETURN_MSG(bcm_port_internal_get(unit, BCM_PORT_INTERNAL_EGRESS_REPLICATION, 2, erp_port, &count_erp), "");

            for (i = 0; i < count_erp; i++) {
                BCM_IF_ERROR_RETURN_MSG(bcm_stk_gport_sysport_get(unit, erp_port[i], &erp_sysport_gport), "");
                erp_sys_port[i] = erp_sysport_gport;
            }

            BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, BCM_MULTICAST_WITH_ID|BCM_MULTICAST_BIER_256_GROUP|BCM_MULTICAST_INGRESS_GROUP, &cint_bier_info.bier_mc_id), "");

           
            /* 1st(continue) copy: to BFR6*/
            bcm_port_get(unit,cint_bier_info.bfr6_port,&flags,&interface_info,&mapping_info);
            bcm_multicast_replication_t_init(&replications[0]);
            bfr_id[0] = cint_bier_info.bfr6_id;
            replications[0].encap1 = cint_bier_info.bier_mc_id;
            replications[0].port = mapping_info.core?erp_sys_port[1]:erp_sys_port[0];
            
            /* 2nd(continue) copy: to BFR7*/
            bcm_port_get(unit,cint_bier_info.bfr7_port,&flags,&interface_info,&mapping_info);
            bcm_multicast_replication_t_init(&replications[1]);
            bfr_id[1] = cint_bier_info.bfr7_id;
            replications[1].encap1 = cint_bier_info.bier_mc_id;
            replications[1].port = mapping_info.core?erp_sys_port[1]:erp_sys_port[0];;
            
            /* 3rd(continue) copy: to BFR8*/
            bcm_port_get(unit,cint_bier_info.bfr8_port,&flags,&interface_info,&mapping_info);
            bcm_multicast_replication_t_init(&replications[2]);
            bfr_id[2] = cint_bier_info.bfr8_id;
            replications[2].encap1 = cint_bier_info.bier_mc_id;
            replications[2].port = mapping_info.core?erp_sys_port[1]:erp_sys_port[0];;
            
            /*MCDB setting*/
            BCM_IF_ERROR_RETURN_MSG(bcm_multicast_bfr_id_add(unit, cint_bier_info.bier_mc_id, BCM_MULTICAST_INGRESS_GROUP, 3, bfr_id, replications), "");

        }


        if (!no_egress_bier_process)
        {
            /*
             * 1st PASS MC, for BIER header
             */
            
            /* 1st(continue) copy: to BFR6*/
            bcm_multicast_replication_t_init(&replications[0]);
            bfr_id[0] = cint_bier_info.bfr6_id;
            replications[0].encap1 = BCM_L3_ITF_VAL_GET(cint_bier_info.bfr4_bfr6_bift_tunnel_id);;
            replications[0].port = cint_bier_info.bfr6_port;
            
            /* 2nd(continue) copy: to BFR7*/
            bcm_multicast_replication_t_init(&replications[1]);
            bfr_id[1] = cint_bier_info.bfr7_id;
            replications[1].encap1 = BCM_L3_ITF_VAL_GET(cint_bier_info.bfr4_bfr7_bift_tunnel_id);
            replications[1].port = cint_bier_info.bfr7_port;
            
            /* 3rd(continue) copy: to BFR8*/
            bcm_multicast_replication_t_init(&replications[2]);
            bfr_id[2] = cint_bier_info.bfr8_id;
            replications[2].encap1 = BCM_L3_ITF_VAL_GET(cint_bier_info.bfr4_bfr8_bift_tunnel_id);
            replications[2].port = cint_bier_info.bfr8_port;
            
            /*MCDB setting*/
            BCM_IF_ERROR_RETURN_MSG(bcm_multicast_bfr_id_add(unit, cint_bier_info.bier_mc_id, BCM_MULTICAST_EGRESS_GROUP, 3, bfr_id, replications), "");
        }
    }
     
    return BCM_E_NONE;
}


/**
 * ARP adding
 * INPUT:
 *   unit    - traffic outgoing unit 
*/
 int
 mpls_bier_arp_configuration(
    int unit,
    int node_type)
{
    dnx_utils_l3_arp_s arp_structure;

    if (node_type == BFIR)
    {
        
        /*
         * Configure access side ARP entry      */
        dnx_utils_l3_arp_s_common_init(unit, 0, &arp_structure, cint_bier_info.bfr1_bfr3_arp_id, 0, BCM_L3_FLAGS2_VLAN_TRANSLATION , cint_bier_info.bfr3_mac, cint_bier_info.bfr3_vlan);
        BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_arp_create(unit, &arp_structure), "bfr3");
        cint_bier_info.bfr1_bfr3_arp_id = arp_structure.l3eg_arp_id;

        /*
         * Configure access side ARP entry      */
        dnx_utils_l3_arp_s_common_init(unit, 0, &arp_structure, cint_bier_info.bfr1_bfr5_arp_id, 0, BCM_L3_FLAGS2_VLAN_TRANSLATION , cint_bier_info.bfr5_mac, cint_bier_info.bfr5_vlan);
        BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_arp_create(unit, &arp_structure), "bfr5");
        cint_bier_info.bfr1_bfr5_arp_id = arp_structure.l3eg_arp_id;

    }
    else if (node_type == BFER)
    {
        /*
         * Configure access side ARP entry      */
        dnx_utils_l3_arp_s_common_init(unit, 0, &arp_structure, cint_bier_info.bfr7_bfr6_arp_id, 0, BCM_L3_FLAGS2_VLAN_TRANSLATION , cint_bier_info.bfr6_mac, cint_bier_info.bfr6_vlan);
        BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_arp_create(unit, &arp_structure), "bfr6");
        cint_bier_info.bfr7_bfr6_arp_id = arp_structure.l3eg_arp_id;

        /*
         * Configure access side ARP entry      */
        dnx_utils_l3_arp_s_common_init(unit, 0, &arp_structure, cint_bier_info.bfr7_ce3_arp_id, 0, BCM_L3_FLAGS2_VLAN_TRANSLATION , cint_bier_info.ce3_mac, cint_bier_info.ce3_vlan);
        BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_arp_create(unit, &arp_structure), "ce3");
        cint_bier_info.bfr7_ce3_arp_id = arp_structure.l3eg_arp_id;

        /*
         * Configure access side ARP entry      */
        dnx_utils_l3_arp_s_common_init(unit, 0, &arp_structure, cint_bier_info.bfr7_ce4_arp_id, 0, BCM_L3_FLAGS2_VLAN_TRANSLATION , cint_bier_info.ce4_mac, cint_bier_info.ce4_vlan);
        BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_arp_create(unit, &arp_structure), "ce4");
        cint_bier_info.bfr7_ce4_arp_id = arp_structure.l3eg_arp_id;
    }
    else
    {
        /*
         * Configure  ARP entry to BFR6      */
        dnx_utils_l3_arp_s_common_init(unit, 0, &arp_structure, cint_bier_info.bfr4_bfr6_arp_id, 0, BCM_L3_FLAGS2_VLAN_TRANSLATION , cint_bier_info.bfr6_mac, cint_bier_info.bfr6_vlan);
        BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_arp_create(unit, &arp_structure), "bfr6");
        cint_bier_info.bfr4_bfr6_arp_id = arp_structure.l3eg_arp_id;

        /*
         * Configure  ARP entry to BFR7      */
        dnx_utils_l3_arp_s_common_init(unit, 0, &arp_structure, cint_bier_info.bfr4_bfr7_arp_id, 0, BCM_L3_FLAGS2_VLAN_TRANSLATION , cint_bier_info.bfr7_mac, cint_bier_info.bfr7_vlan);
        BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_arp_create(unit, &arp_structure), "bfr6");
        cint_bier_info.bfr4_bfr7_arp_id = arp_structure.l3eg_arp_id;

        /*
         * Configure  ARP entry to BFR8      */
        dnx_utils_l3_arp_s_common_init(unit, 0, &arp_structure, cint_bier_info.bfr4_bfr8_arp_id, 0, BCM_L3_FLAGS2_VLAN_TRANSLATION , cint_bier_info.bfr8_mac, cint_bier_info.bfr8_vlan);
        BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_arp_create(unit, &arp_structure), "bfr6");
        cint_bier_info.bfr4_bfr8_arp_id = arp_structure.l3eg_arp_id;
    }


    return BCM_E_NONE;
}


/**
 * MPLS tunnel adding
 * INPUT:
 *   unit    - traffic outgoing unit 
*/
int
mpls_bier_mpls_tunnel_configure(
    int unit,
    int node_type,
    int nof_tunnel,
    int payload)
{
    int encap_access_array[6] = {
                                        bcmEncapAccessTunnel1,
                                        bcmEncapAccessTunnel2,
                                        bcmEncapAccessTunnel3,
                                        bcmEncapAccessTunnel4,
                                        bcmEncapAccessArp,
                                        bcmEncapAccessInvalid
                                    };
    int mpls_tunnel_id = 0;
    bcm_mpls_label_t stage_enc_label_1 = 0x3331;
    bcm_mpls_label_t stage_enc_label_2 = 0x4441;
    bcm_mpls_egress_label_t label_array[2];
    uint32 bit_str_in_mcdb = *dnxc_data_get(unit, "bier", "params", "bfr_bit_str_in_mcdb", NULL);
    int encap_access_idx = bit_str_in_mcdb?0:1;

    if (node_type == BFIR)
    {
        /*BFR1----->BFR3*/
        mpls_tunnel_initiator_create_s_init(mpls_util_entity[0].mpls_encap_tunnel, MPLS_UTIL_MAX_NOF_TUNNELS);
        encap_access_idx = bit_str_in_mcdb?0:1;        
        /* encapsulation entries, not including BIFT entries */
        for (mpls_tunnel_id = 0; mpls_tunnel_id < nof_tunnel; mpls_tunnel_id++, encap_access_idx++, stage_enc_label_1++, stage_enc_label_2++)
        {
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].label[0] = stage_enc_label_1;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].label[1] = stage_enc_label_2;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].num_labels = 2;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT | BCM_MPLS_EGRESS_LABEL_TANDEM;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].l3_intf_id = &mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id+1].tunnel_id;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].encap_access = encap_access_array[encap_access_idx];
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].egress_qos_model.egress_qos = bcmQosEgressModelPipeMyNameSpace;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].exp0 = 1;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].exp1 = 1;
        }
        
        mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id-1].l3_intf_id = &cint_bier_info.bfr1_bfr3_arp_id;
        
        
        BCM_IF_ERROR_RETURN_MSG(mpls_create_initiator_tunnels(unit, mpls_util_entity[0].mpls_encap_tunnel, nof_tunnel), "");
        
        cint_bier_info.bfr1_bfr3_lsp_tunnel_id = mpls_util_entity[0].mpls_encap_tunnel[0].tunnel_id;


        /*BFR1----->BFR5*/
        mpls_tunnel_initiator_create_s_init(mpls_util_entity[0].mpls_encap_tunnel, MPLS_UTIL_MAX_NOF_TUNNELS);
        encap_access_idx = bit_str_in_mcdb?0:1;
        /* encapsulation entries, not including BIFT entries */
        for (mpls_tunnel_id = 0; mpls_tunnel_id < nof_tunnel; mpls_tunnel_id++, encap_access_idx++, stage_enc_label_1++, stage_enc_label_2++)
        {
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].label[0] = stage_enc_label_1;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].label[1] = stage_enc_label_2;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].num_labels = 2;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT | BCM_MPLS_EGRESS_LABEL_TANDEM;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].l3_intf_id = &mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id+1].tunnel_id;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].encap_access = encap_access_array[encap_access_idx];
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].egress_qos_model.egress_qos = bcmQosEgressModelPipeMyNameSpace;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].exp0 = 1;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].exp1 = 1;
        }
        
        mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id-1].l3_intf_id = &cint_bier_info.bfr1_bfr5_arp_id;
        
        
        BCM_IF_ERROR_RETURN_MSG(mpls_create_initiator_tunnels(unit, mpls_util_entity[0].mpls_encap_tunnel, nof_tunnel), "");
        
        cint_bier_info.bfr1_bfr5_lsp_tunnel_id = mpls_util_entity[0].mpls_encap_tunnel[0].tunnel_id;

    }
    else if (node_type == BFER)
    {
        /*BFR7----->BFR6*/
        mpls_tunnel_initiator_create_s_init(mpls_util_entity[0].mpls_encap_tunnel, MPLS_UTIL_MAX_NOF_TUNNELS);
        encap_access_idx = bit_str_in_mcdb?0:1;
        /* encapsulation entries, not including BIFT entries */
        for (mpls_tunnel_id = 0; mpls_tunnel_id < nof_tunnel; mpls_tunnel_id++, encap_access_idx++, stage_enc_label_1++, stage_enc_label_2++)
        {
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].label[0] = stage_enc_label_1;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].label[1] = stage_enc_label_2;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].num_labels = 2;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT | BCM_MPLS_EGRESS_LABEL_TANDEM;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].l3_intf_id = &mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id+1].tunnel_id;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].encap_access = encap_access_array[encap_access_idx];
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].egress_qos_model.egress_qos = bcmQosEgressModelPipeMyNameSpace;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].exp0 = 1;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].exp1 = 1;
        }
        
        mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id-1].l3_intf_id = &cint_bier_info.bfr7_bfr6_arp_id;
        
        
        BCM_IF_ERROR_RETURN_MSG(mpls_create_initiator_tunnels(unit, mpls_util_entity[0].mpls_encap_tunnel, nof_tunnel), "");
        
        cint_bier_info.bfr7_bfr6_lsp_tunnel_id = mpls_util_entity[0].mpls_encap_tunnel[0].tunnel_id;

    }
    else
    {
        /*BFR4----->BFR6*/
        mpls_tunnel_initiator_create_s_init(mpls_util_entity[0].mpls_encap_tunnel, MPLS_UTIL_MAX_NOF_TUNNELS);
        encap_access_idx = bit_str_in_mcdb?0:1;
        /* encapsulation entries, not including BIFT entries */
        for (mpls_tunnel_id = 0; mpls_tunnel_id < nof_tunnel; mpls_tunnel_id++, encap_access_idx++, stage_enc_label_1++, stage_enc_label_2++)
        {
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].label[0] = stage_enc_label_1;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].label[1] = stage_enc_label_2;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].num_labels = 2;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT | BCM_MPLS_EGRESS_LABEL_TANDEM;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].l3_intf_id = &mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id+1].tunnel_id;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].encap_access = encap_access_array[encap_access_idx];
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].egress_qos_model.egress_qos = bcmQosEgressModelPipeMyNameSpace;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].exp0 = 1;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].exp1 = 1;
        }

        mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id-1].l3_intf_id = &cint_bier_info.bfr4_bfr6_arp_id;


        BCM_IF_ERROR_RETURN_MSG(mpls_create_initiator_tunnels(unit, mpls_util_entity[0].mpls_encap_tunnel, nof_tunnel), "");

        cint_bier_info.bfr4_bfr6_lsp_tunnel_id = mpls_util_entity[0].mpls_encap_tunnel[0].tunnel_id;


        /*BFR4----->BFR7*/
        mpls_tunnel_initiator_create_s_init(mpls_util_entity[0].mpls_encap_tunnel, MPLS_UTIL_MAX_NOF_TUNNELS);
        encap_access_idx = bit_str_in_mcdb?0:1;;
        /* encapsulation entries, not including BIFT entries */
        for (mpls_tunnel_id = 0; mpls_tunnel_id < nof_tunnel; mpls_tunnel_id++, encap_access_idx++, stage_enc_label_1++, stage_enc_label_2++)
        {   
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].label[0] = stage_enc_label_1;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].label[1] = stage_enc_label_2;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].num_labels = 2;            
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT | BCM_MPLS_EGRESS_LABEL_TANDEM;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].l3_intf_id = &mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id+1].tunnel_id;           
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].encap_access = encap_access_array[encap_access_idx];
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].egress_qos_model.egress_qos = bcmQosEgressModelPipeMyNameSpace;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].exp0 = 1;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].exp1 = 1;
        }

        mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id-1].l3_intf_id = &cint_bier_info.bfr4_bfr7_arp_id;


        BCM_IF_ERROR_RETURN_MSG(mpls_create_initiator_tunnels(unit, mpls_util_entity[0].mpls_encap_tunnel, nof_tunnel), "");

        cint_bier_info.bfr4_bfr7_lsp_tunnel_id = mpls_util_entity[0].mpls_encap_tunnel[0].tunnel_id;


        /*BFR4----->BFR8*/
        mpls_tunnel_initiator_create_s_init(mpls_util_entity[0].mpls_encap_tunnel, MPLS_UTIL_MAX_NOF_TUNNELS);
        encap_access_idx = bit_str_in_mcdb?0:1;;
        /* encapsulation entries, not including BIFT entries */
        for (mpls_tunnel_id = 0; mpls_tunnel_id < nof_tunnel; mpls_tunnel_id++, encap_access_idx++, stage_enc_label_1++, stage_enc_label_2++)
        {
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].label[0] = stage_enc_label_1;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].label[1] = stage_enc_label_2;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].num_labels = 2;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT | BCM_MPLS_EGRESS_LABEL_TANDEM;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].l3_intf_id = &mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id+1].tunnel_id;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].encap_access = encap_access_array[encap_access_idx];
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].egress_qos_model.egress_qos = bcmQosEgressModelPipeMyNameSpace;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].exp0 = 1;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].exp1 = 1;
        }

        mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id-1].l3_intf_id = &cint_bier_info.bfr4_bfr8_arp_id;


        BCM_IF_ERROR_RETURN_MSG(mpls_create_initiator_tunnels(unit, mpls_util_entity[0].mpls_encap_tunnel, nof_tunnel), "");

        cint_bier_info.bfr4_bfr8_lsp_tunnel_id = mpls_util_entity[0].mpls_encap_tunnel[0].tunnel_id;

    }


    return BCM_E_NONE;
}


/**
 * bift entry for bier header editing 
 * as transit role
 * INPUT:
 *   unit    - traffic outgoing unit 
*/
int
mpls_bier_bift_entry_create(
    int unit,
    int node_type,
    int payload)
{
    bcm_mpls_egress_label_t label_array[2];
    bcm_flow_initiator_info_t info;
    bcm_flow_handle_t flow_handle;
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_special_fields_t special_fields;
    bcm_flow_field_id_t field_id;
    bcm_if_t   intf_id = 0;
    uint32 bit_str_in_mcdb = *dnxc_data_get(unit, "bier", "params", "bfr_bit_str_in_mcdb", NULL);

    if (node_type == BFIR)
    {
        /*BFR1----->BFR3*/
        if (!bit_str_in_mcdb) 
        {
            BCM_IF_ERROR_RETURN_MSG(bcm_flow_handle_get(unit, "BIER_BITSTR_INITIATOR",&flow_handle), "");

            bcm_flow_handle_info_t_init(&flow_handle_info);
            bcm_flow_special_fields_t_init(&special_fields);

            flow_handle_info.flow_handle = flow_handle;
            flow_handle_info.flags = 0;
            info.valid_elements_set |=
                (BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID );

            info.l3_intf_id = cint_bier_info.bfr1_bfr3_lsp_tunnel_id;;
            info.encap_access = bcmEncapAccessNativeArp;

            special_fields.actual_nof_special_fields = 1;

            BCM_IF_ERROR_RETURN_MSG(bcm_flow_logical_field_id_get(unit, flow_handle, "BFR_BIT_STR", &field_id), "");
            special_fields.special_fields[0].field_id = field_id;
            special_fields.special_fields[0].shr_var_uint32_arr[4] = 0x8;

            /** special fields configuration */
            BCM_IF_ERROR_RETURN_MSG(bcm_flow_initiator_info_create(unit, &flow_handle_info, &info, &special_fields), "");

            printf ("bcm_flow_initiator_info_create bitstr entry pass. initiator_id=0x%x\n", flow_handle_info.flow_id);
            BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(intf_id, flow_handle_info.flow_id);
        }

        /*BIFT entry*/
        bcm_mpls_egress_label_t_init(&label_array[0]);
        label_array[0].label = cint_bier_info.bfr3_bift_id;
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        label_array[0].l3_intf_id = bit_str_in_mcdb?cint_bier_info.bfr1_bfr3_lsp_tunnel_id:intf_id;
        label_array[0].encap_access = bcmEncapAccessRif;
        label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelPipeMyNameSpace;
        label_array[0].exp = 1;
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_STAT_ENABLE;

        BCM_IF_ERROR_RETURN_MSG(bcm_mpls_tunnel_initiator_create(unit, 0, 1, label_array), "");
        cint_bier_info.bfr1_bfr3_bift_tunnel_id = label_array[0].tunnel_id;

        /*BFR1----->BFR5*/
        if (!bit_str_in_mcdb)
        {
            BCM_IF_ERROR_RETURN_MSG(bcm_flow_handle_get(unit, "BIER_BITSTR_INITIATOR",&flow_handle), "");

            bcm_flow_handle_info_t_init(&flow_handle_info);
            bcm_flow_special_fields_t_init(&special_fields);

            flow_handle_info.flow_handle = flow_handle;
            flow_handle_info.flags = 0;
            info.valid_elements_set |=
                (BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID );

            info.l3_intf_id = cint_bier_info.bfr1_bfr5_lsp_tunnel_id;;
            info.encap_access = bcmEncapAccessNativeArp;

            special_fields.actual_nof_special_fields = 1;

            BCM_IF_ERROR_RETURN_MSG(bcm_flow_logical_field_id_get(unit, flow_handle, "BFR_BIT_STR", &field_id), "");
            special_fields.special_fields[0].field_id = field_id;
            special_fields.special_fields[0].shr_var_uint32_arr[4] = 0x20;

            /** special fields configuration */
            BCM_IF_ERROR_RETURN_MSG(bcm_flow_initiator_info_create(unit, &flow_handle_info, &info, &special_fields), "");

            printf ("bcm_flow_initiator_info_create bitstr entry pass. initiator_id=0x%x\n", flow_handle_info.flow_id);
            BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(intf_id, flow_handle_info.flow_id);
        }

        /*BIFT entry*/
        bcm_mpls_egress_label_t_init(&label_array[0]);
        label_array[0].label = cint_bier_info.bfr5_bift_id;
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        label_array[0].l3_intf_id = bit_str_in_mcdb?cint_bier_info.bfr1_bfr5_lsp_tunnel_id:intf_id;
        label_array[0].encap_access = bcmEncapAccessRif;
        label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelPipeMyNameSpace;
        label_array[0].exp = 1;
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_STAT_ENABLE;

        BCM_IF_ERROR_RETURN_MSG(bcm_mpls_tunnel_initiator_create(unit, 0, 1, label_array), "");
        cint_bier_info.bfr1_bfr5_bift_tunnel_id = label_array[0].tunnel_id;

        
        bcm_mpls_egress_label_t_init(&label_array[0]);
        label_array[0].label = cint_bier_info.bfr1_bift_id;
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        label_array[0].l3_intf_id = cint_bier_info.overlay_entry_id;
        label_array[0].encap_access = bcmEncapAccessTunnel4;
        label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelPipeMyNameSpace;
        label_array[0].exp = 1;
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_STAT_ENABLE;

        BCM_IF_ERROR_RETURN_MSG(bcm_mpls_tunnel_initiator_create(unit, 0, 1, label_array), "");
        cint_bier_info.bfr1_to_2nd_pass_bift_tunnel_id = label_array[0].tunnel_id;
    }
    else if (node_type == BFER)
    {
        /*BFR7------->BFR6*/
        if (!bit_str_in_mcdb)
        {   
            BCM_IF_ERROR_RETURN_MSG(bcm_flow_handle_get(unit, "BIER_BITSTR_INITIATOR",&flow_handle), "");
            
            bcm_flow_handle_info_t_init(&flow_handle_info);
            bcm_flow_special_fields_t_init(&special_fields);
            
            flow_handle_info.flow_handle = flow_handle;
            flow_handle_info.flags = 0;
            info.valid_elements_set |=
                (BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID );
            
            info.l3_intf_id = cint_bier_info.bfr7_bfr6_lsp_tunnel_id;
            info.encap_access = bcmEncapAccessNativeArp;
            
            special_fields.actual_nof_special_fields = 1;
            
            BCM_IF_ERROR_RETURN_MSG(bcm_flow_logical_field_id_get(unit, flow_handle, "BFR_BIT_STR", &field_id), "");
            special_fields.special_fields[0].field_id = field_id;
            special_fields.special_fields[0].shr_var_uint32_arr[4] = 0x40;
            
            /** special fields configuration */
            BCM_IF_ERROR_RETURN_MSG(bcm_flow_initiator_info_create(unit, &flow_handle_info, &info, &special_fields), "");
            
            printf ("bcm_flow_initiator_info_create bitstr entry pass. initiator_id=0x%x\n", flow_handle_info.flow_id);
            BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(intf_id, flow_handle_info.flow_id);      
        }

        /*
         * BIFT entry for transit copy
         */
        bcm_mpls_egress_label_t_init(&label_array[0]);
        label_array[0].label = cint_bier_info.bfr6_bift_id;
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT ;
        label_array[0].l3_intf_id = bit_str_in_mcdb?cint_bier_info.bfr7_bfr6_lsp_tunnel_id:intf_id;
        label_array[0].encap_access = bcmEncapAccessRif;
        label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelPipeMyNameSpace;
        label_array[0].exp = 1;
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_STAT_ENABLE;

        BCM_IF_ERROR_RETURN_MSG(bcm_mpls_tunnel_initiator_create(unit, 0, 1, label_array), "");
        cint_bier_info.bfr7_bfr6_bift_tunnel_id = label_array[0].tunnel_id;

        if (!bit_str_in_mcdb)
        {
            BCM_IF_ERROR_RETURN_MSG(bcm_flow_handle_get(unit, "BIER_BITSTR_INITIATOR",&flow_handle), "");

            bcm_flow_handle_info_t_init(&flow_handle_info);
            bcm_flow_special_fields_t_init(&special_fields);

            flow_handle_info.flow_handle = flow_handle;
            flow_handle_info.flags = 0;
            info.valid_elements_set |=
                (BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID );

            info.l3_intf_id = cint_bier_info.overlay_entry_id;
            info.encap_access = bcmEncapAccessNativeArp;

            special_fields.actual_nof_special_fields = 1;

            BCM_IF_ERROR_RETURN_MSG(bcm_flow_logical_field_id_get(unit, flow_handle, "BFR_BIT_STR", &field_id), "");
            special_fields.special_fields[0].field_id = field_id;
            special_fields.special_fields[0].shr_var_uint32_arr[4] = 0x20;

            /** special fields configuration */
            BCM_IF_ERROR_RETURN_MSG(bcm_flow_initiator_info_create(unit, &flow_handle_info, &info, &special_fields), "");

            printf ("bcm_flow_initiator_info_create bitstr entry pass. initiator_id=0x%x\n", flow_handle_info.flow_id);
            BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(intf_id, flow_handle_info.flow_id);
        }

        /*
         * BIFT entry for local copy
         */

        bcm_mpls_egress_label_t_init(&label_array[0]);
        label_array[0].label = cint_bier_info.bfr7_bift_id;
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        label_array[0].l3_intf_id = bit_str_in_mcdb?cint_bier_info.overlay_entry_id:intf_id;
        label_array[0].encap_access = bcmEncapAccessRif;
        label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelPipeMyNameSpace;
        label_array[0].exp = 1;
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_STAT_ENABLE;

        BCM_IF_ERROR_RETURN_MSG(bcm_mpls_tunnel_initiator_create(unit, 0, 1, label_array), "");
        cint_bier_info.bfr7_drop_copy_bift_tunnel_id = label_array[0].tunnel_id;
    }
    else
    {
        /*BFR4----------->BFR6*/
        if (!bit_str_in_mcdb)
        {
            BCM_IF_ERROR_RETURN_MSG(bcm_flow_handle_get(unit, "BIER_BITSTR_INITIATOR",&flow_handle), "");

            bcm_flow_handle_info_t_init(&flow_handle_info);
            bcm_flow_special_fields_t_init(&special_fields);

            flow_handle_info.flow_handle = flow_handle;
            flow_handle_info.flags = 0;
            info.valid_elements_set |=
                (BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID );

            info.l3_intf_id = cint_bier_info.bfr4_bfr6_lsp_tunnel_id;
            info.encap_access = bcmEncapAccessNativeArp;

            special_fields.actual_nof_special_fields = 1;

            BCM_IF_ERROR_RETURN_MSG(bcm_flow_logical_field_id_get(unit, flow_handle, "BFR_BIT_STR", &field_id), "");
            special_fields.special_fields[0].field_id = field_id;
            special_fields.special_fields[0].shr_var_uint32_arr[4] = 0x40;

            /** special fields configuration */
            BCM_IF_ERROR_RETURN_MSG(bcm_flow_initiator_info_create(unit, &flow_handle_info, &info, &special_fields), "");

            printf ("bcm_flow_initiator_info_create bitstr entry pass. initiator_id=0x%x\n", flow_handle_info.flow_id);
            BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(intf_id, flow_handle_info.flow_id);
        }

        /*BIFT entry BFR4-->BFR6*/
        bcm_mpls_egress_label_t_init(&label_array[0]);
        label_array[0].label = cint_bier_info.bfr6_bift_id;
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        label_array[0].l3_intf_id = bit_str_in_mcdb?cint_bier_info.bfr4_bfr6_lsp_tunnel_id:intf_id;
        label_array[0].encap_access = bcmEncapAccessRif;
        label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelPipeMyNameSpace;
        label_array[0].exp = 1;
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_STAT_ENABLE;

        BCM_IF_ERROR_RETURN_MSG(bcm_mpls_tunnel_initiator_create(unit, 0, 1, label_array), "");
        cint_bier_info.bfr4_bfr6_bift_tunnel_id = label_array[0].tunnel_id;

        /*BFR4----------->BFR7*/
        if (!bit_str_in_mcdb)
        {
            BCM_IF_ERROR_RETURN_MSG(bcm_flow_handle_get(unit, "BIER_BITSTR_INITIATOR",&flow_handle), "");

            bcm_flow_handle_info_t_init(&flow_handle_info);
            bcm_flow_special_fields_t_init(&special_fields);

            flow_handle_info.flow_handle = flow_handle;
            flow_handle_info.flags = 0;
            info.valid_elements_set |=
                (BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID );

            info.l3_intf_id = cint_bier_info.bfr4_bfr7_lsp_tunnel_id;
            info.encap_access = bcmEncapAccessNativeArp;

            special_fields.actual_nof_special_fields = 1;

            BCM_IF_ERROR_RETURN_MSG(bcm_flow_logical_field_id_get(unit, flow_handle, "BFR_BIT_STR", &field_id), "");
            special_fields.special_fields[0].field_id = field_id;
            special_fields.special_fields[0].shr_var_uint32_arr[4] = 0x80;

            /** special fields configuration */
            BCM_IF_ERROR_RETURN_MSG(bcm_flow_initiator_info_create(unit, &flow_handle_info, &info, &special_fields), "");

            printf ("bcm_flow_initiator_info_create bitstr entry pass. initiator_id=0x%x\n", flow_handle_info.flow_id);
            BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(intf_id, flow_handle_info.flow_id);
        }

        /*BIFT entry BFR4-->BFR7*/
        bcm_mpls_egress_label_t_init(&label_array[0]);
        label_array[0].label = cint_bier_info.bfr7_bift_id;
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT ;
        label_array[0].l3_intf_id = bit_str_in_mcdb?cint_bier_info.bfr4_bfr7_lsp_tunnel_id:intf_id;
        label_array[0].encap_access = bcmEncapAccessRif;
        label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelPipeMyNameSpace;
        label_array[0].exp = 1;
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_STAT_ENABLE;

        BCM_IF_ERROR_RETURN_MSG(bcm_mpls_tunnel_initiator_create(unit, 0, 1, label_array), "");
        cint_bier_info.bfr4_bfr7_bift_tunnel_id = label_array[0].tunnel_id;

        /*BFR4----------->BFR7*/
        if (!bit_str_in_mcdb)
        {   
            BCM_IF_ERROR_RETURN_MSG(bcm_flow_handle_get(unit, "BIER_BITSTR_INITIATOR",&flow_handle), "");
            
            bcm_flow_handle_info_t_init(&flow_handle_info);
            bcm_flow_special_fields_t_init(&special_fields);
            
            flow_handle_info.flow_handle = flow_handle;
            flow_handle_info.flags = 0;
            info.valid_elements_set |=
                (BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID );
            
            info.l3_intf_id = cint_bier_info.bfr4_bfr8_lsp_tunnel_id;
            info.encap_access = bcmEncapAccessNativeArp;
            
            special_fields.actual_nof_special_fields = 1;
            
            BCM_IF_ERROR_RETURN_MSG(bcm_flow_logical_field_id_get(unit, flow_handle, "BFR_BIT_STR", &field_id), "");
            special_fields.special_fields[0].field_id = field_id;
            special_fields.special_fields[0].shr_var_uint32_arr[4] = 0x100;
            
            /** special fields configuration */
            BCM_IF_ERROR_RETURN_MSG(bcm_flow_initiator_info_create(unit, &flow_handle_info, &info, &special_fields), "");
            
            printf ("bcm_flow_initiator_info_create bitstr entry pass. initiator_id=0x%x\n", flow_handle_info.flow_id);
            BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(intf_id, flow_handle_info.flow_id);
        }

        /*BIFT entry BFR4-->BFR8*/
        bcm_mpls_egress_label_t_init(&label_array[0]);
        label_array[0].label = cint_bier_info.bfr8_bift_id;
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        label_array[0].l3_intf_id = bit_str_in_mcdb?cint_bier_info.bfr4_bfr8_lsp_tunnel_id:intf_id;
        label_array[0].encap_access = bcmEncapAccessRif;
        label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelPipeMyNameSpace;
        label_array[0].exp = 1;
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_STAT_ENABLE;

        BCM_IF_ERROR_RETURN_MSG(bcm_mpls_tunnel_initiator_create(unit, 0, 1, label_array), "");
        cint_bier_info.bfr4_bfr8_bift_tunnel_id = label_array[0].tunnel_id;
    }


    return BCM_E_NONE;

}

/*
  * Create a Recycle Entry.
  * unit - relevant unit
  * node_type - node type.
  */
 int 
 mpls_bier_create_recycle_entry(
     int unit,
     int node_type)
 {
    uint32 egress_filters_enable_per_recycle_port_supported = *dnxc_data_get(unit, "port_pp", "filters", "egress_filters_enable_per_recycle_port_supported", NULL);
    recycle_entry_params_s recycle_entry_params;
    uint32 ibch1_supported = *dnxc_data_get(unit, "headers", "system_headers", "system_headers_ibch1_supported", NULL);

    recycle_entry_params.recycle_app = (node_type == BFIR)?bcmL2EgressRecycleAppExtendedEncap:bcmL2EgressRecycleAppDropAndContinue;
    BCM_IF_ERROR_RETURN_MSG(create_recycle_entry(unit, recycle_entry_params, &cint_bier_info.overlay_entry_id), "");
    /** eed to disable egress filters explicitly for ExtTerm and Drop&Cont, base on the RCH out-lif */
    if (egress_filters_enable_per_recycle_port_supported == 0 && recycle_entry_params.recycle_app == bcmL2EgressRecycleAppDropAndContinue)
    {
        int cs_profile_id = 5;
        BCM_IF_ERROR_RETURN_MSG(cint_field_epmf_cs_outlif_profile_disable_egress_filters_main(unit, cint_bier_info.overlay_entry_id, cs_profile_id), "");
    }

     return BCM_E_NONE;
 }

/*
  * Create a termination for BIFT label.
  * unit - relevant unit
  */
int
mpls_bier_2nd_pass_terminator_create(
    int unit,
    int over_mpls)
{
    bcm_flow_handle_t flow_handle;
    bcm_flow_terminator_info_t info;
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_special_fields_t special_fields;
    int terminator_id = 0;



    BCM_IF_ERROR_RETURN_MSG(bcm_flow_handle_get(unit, "BIER_TERMINATOR",&flow_handle), "");
    
    flow_handle_info.flow_handle = flow_handle;
    flow_handle_info.flags = 0;
    info.valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID |
                               BCM_FLOW_TERMINATOR_ELEMENT_QOS_INGRESS_MODEL_VALID;
    bcm_flow_l3_ingress_info_t_init(&info.l3_ingress_info);
    info.l3_ingress_info.service_type = bcmFlowServiceTypeMultiPoint;

    info.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;
    info.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
    info.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
    BCM_IF_ERROR_RETURN_MSG(bcm_flow_terminator_info_create(unit, &flow_handle_info, &info, NULL), "");

    cint_bier_info.bfr7_bift_term_id = flow_handle_info.flow_id;
    
    /** point the LIF from the ISEM table */
    BCM_IF_ERROR_RETURN_MSG(bcm_flow_handle_get(unit, "BIER_TERM_MATCH", &flow_handle), "");
    flow_handle_info.flow_handle = flow_handle;
    flow_handle_info.flow_id = cint_bier_info.bfr7_bift_term_id;

    BCM_IF_ERROR_RETURN_MSG(bcm_flow_logical_field_id_get(
            unit, flow_handle, "BIER_BIFT_ID", &special_fields.special_fields[0].field_id), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_flow_logical_field_id_get(
            unit, flow_handle, "NEXT_LAYER_NETWORK_DOMAIN", &special_fields.special_fields[1].field_id), "");
    special_fields.actual_nof_special_fields = 2;
    special_fields.special_fields[0].is_clear = 0;
    special_fields.special_fields[0].shr_var_uint32 = 
                     over_mpls?cint_bier_info.bfr7_bift_id:cint_bier_info.bfr1_bift_id;
    special_fields.special_fields[1].is_clear = 0;
    special_fields.special_fields[1].shr_var_uint32 = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_flow_match_info_add(unit, &flow_handle_info, &special_fields), "");

    printf ("mpls_bier_2nd_pass_terminator_create pass. terminator_id=%x \n", cint_bier_info.bfr7_bift_term_id);
    
    return BCM_E_NONE;
}



/*
  * Create a initiator for BIER header
  * unit - relevant unit
  */
int
mpls_bier_initiator_create(
    int unit)
{
    bcm_flow_initiator_info_t info;
    bcm_flow_handle_t flow_handle;
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_special_fields_t special_fields;
    bcm_flow_field_id_t field_id;
    bcm_if_t   intf_id = 0;
    uint32 bfr_bit_str_in_mcdb = *dnxc_data_get(unit, "bier", "params", "bfr_bit_str_in_mcdb", NULL);

    BCM_IF_ERROR_RETURN_MSG(bcm_flow_handle_get(unit, "BIER_BITSTR_INITIATOR",&flow_handle), "");
    
    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_special_fields_t_init(&special_fields);

    flow_handle_info.flow_handle = flow_handle;
    flow_handle_info.flags = 0;
    info.valid_elements_set |=
        (BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID );

    info.l3_intf_id = cint_bier_info.bfr1_to_2nd_pass_bift_tunnel_id;
    info.encap_access = bcmEncapAccessTunnel2;

    special_fields.actual_nof_special_fields = 1;

    BCM_IF_ERROR_RETURN_MSG(bcm_flow_logical_field_id_get(unit, flow_handle, "BFR_BIT_STR", &field_id), "");
    special_fields.special_fields[0].field_id = field_id;
    special_fields.special_fields[0].shr_var_uint32_arr[4] = 0x28;

    /** special fields configuration */
    BCM_IF_ERROR_RETURN_MSG(bcm_flow_initiator_info_create(unit, &flow_handle_info, &info, &special_fields), "");

    printf ("bcm_flow_initiator_info_create bitstr entry pass. initiator_id=%d \n", flow_handle_info.flow_id);


    BCM_IF_ERROR_RETURN_MSG(bcm_flow_handle_get(unit, "BIER_BASE_INITIATOR",&flow_handle), "");

    BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(intf_id, flow_handle_info.flow_id);

    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_special_fields_t_init(&special_fields);

    flow_handle_info.flow_handle = flow_handle;
    flow_handle_info.flags = 0;
    info.valid_elements_set |=
        (BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID );

    info.l3_intf_id = intf_id;
    info.encap_access = bcmEncapAccessTunnel1;

    if(!bfr_bit_str_in_mcdb)
    {
        special_fields.actual_nof_special_fields = 5;
    }
    else
    {
        special_fields.actual_nof_special_fields = 4;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_flow_logical_field_id_get(unit, flow_handle, "BIER_BFIR_ID", &field_id), "");
    special_fields.special_fields[0].field_id = field_id;
    special_fields.special_fields[0].shr_var_uint32 = cint_bier_info.bfr1_bift_id;

    BCM_IF_ERROR_RETURN_MSG(bcm_flow_logical_field_id_get(unit, flow_handle, "BIER_BSL", &field_id), "");
    special_fields.special_fields[1].field_id = field_id;
    special_fields.special_fields[1].shr_var_uint32 = 3;

    BCM_IF_ERROR_RETURN_MSG(bcm_flow_logical_field_id_get(unit, flow_handle, "BIER_RSV", &field_id), "");
    special_fields.special_fields[2].field_id = field_id;
    special_fields.special_fields[2].shr_var_uint32 = 3;

    BCM_IF_ERROR_RETURN_MSG(bcm_flow_logical_field_id_get(unit, flow_handle, "BIER_OAM", &field_id), "");
    special_fields.special_fields[3].field_id = field_id;
    special_fields.special_fields[3].shr_var_uint32 = 3;

    if(!bfr_bit_str_in_mcdb)
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_flow_logical_field_id_get(unit, flow_handle, "BIER_NIBBLE", &field_id), "");
        special_fields.special_fields[4].field_id = field_id;
        special_fields.special_fields[4].shr_var_uint32 = 5;
    }
    /** special fields configuration */
    BCM_IF_ERROR_RETURN_MSG(bcm_flow_initiator_info_create(unit, &flow_handle_info, &info, &special_fields), "");

    BCM_GPORT_TUNNEL_ID_SET(cint_bier_info.bfr1_bift_initator_id, _SHR_GPORT_FLOW_LIF_VAL_GET_ID(flow_handle_info.flow_id));

    printf ("bcm_flow_initiator_info_create base header entry pass. initiator_id=0x%x\n", cint_bier_info.bfr1_bift_initator_id);

    return BCM_E_NONE;

}

/*
  * Minimize the fwd header termination size
  * unit - relevant unit
  * rcy_port -rcy port
  */
int mpls_bier_field_ipv6_fwd_term_min_bytes(
    int unit,
    int rcy_port)
{
    /* IPMF3 IPv4 Context Create + Presel */
    int    etpp_fwd_term_presel_id_v6 = 23;
    bcm_field_context_t etpp_fwd_term_context_id_ipmf3_v6;
    bcm_field_group_t   etpp_fwd_term_tcam_fg_id;
    bcm_field_entry_t   etpp_fwd_term_ent_ipv6_id;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_context_info_t context_info;
    void *dest_char;
    int hw_value;


    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, rcy_port, bcmPortClassFieldIngressPMF3PacketProcessingPortCs, 7), "");

    /* IPMF3 IPv6 Context Create + Presel */

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "nof_rebuild_ipmf3_v6", sizeof(context_info.name));
    BCM_IF_ERROR_RETURN_MSG(bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF3, &context_info, &etpp_fwd_term_context_id_ipmf3_v6), "");

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = etpp_fwd_term_presel_id_v6;
    p_id.stage = bcmFieldStageIngressPMF3;
    p_data.entry_valid = TRUE;
    p_data.context_id = etpp_fwd_term_context_id_ipmf3_v6;
    p_data.nof_qualifiers = 2;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp6;
    p_data.qual_data[0].qual_mask = 0x1F;
    p_data.qual_data[1].qual_type = bcmFieldQualifyPortClassPacketProcessing;
    p_data.qual_data[1].qual_arg = 0;
    p_data.qual_data[1].qual_value = 7;
    p_data.qual_data[1].qual_mask = 0x7;


    BCM_IF_ERROR_RETURN_MSG(bcm_field_presel_set(unit, 0, &p_id, &p_data), "");

    /* IPMF3 FG Create + Attach */

    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;

    /*
     * Create and attach TCAM group in iPMF3
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF3;

    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyLayerRecordType;

    /* Set actions */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionParsingStartTypeRaw;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "nof_rebuild_tcam_fg", sizeof(fg_info.name));
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_add(unit, 0, &fg_info, &etpp_fwd_term_tcam_fg_id), "");

    /*
     * Attach the FG to contexts
     */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerRecordsFwd;
    attach_info.key_info.qual_info[0].input_arg= 0;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_attach(unit, 0, etpp_fwd_term_tcam_fg_id, etpp_fwd_term_context_id_ipmf3_v6, &attach_info), "");

    /*
     * Add TCAM entries
     */
    bcm_field_entry_info_t ent_info;
    bcm_field_entry_info_t_init(&ent_info);

    ent_info.nof_entry_quals = 1;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];

    ent_info.entry_qual[0].value[0] = bcmFieldLayerTypeIp6;
    ent_info.entry_qual[0].mask[0] = 0x1F;

    ent_info.nof_entry_actions= 1;
    ent_info.entry_action[0].type = fg_info.action_types[0];

    dnx_dbal_fields_enum_value_get(unit, "PARSING_START_TYPE_ENCODING", "IPV6_8B", &hw_value); /* DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_IPV6_8B */
    hw_value = hw_value << 1 | 1; /* The valid bit value is the LSB, and it's set to 1 */
    ent_info.entry_action[0].value[0] = hw_value;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, 0, etpp_fwd_term_tcam_fg_id, &ent_info, &etpp_fwd_term_ent_ipv6_id), "");

    return BCM_E_NONE;
}
 /**
  * inititate parameter for MPLS BIER
  * INPUT:
  *   node_type    - node type 
  *   nof_tunnels  - number of mpls tunnels
  *   port_1       - port_1
  *   port_2       - port_2
  *   port_3       - port_3
  *   port_3       - port_4
 */
  int
  mpls_bier_init_para(
     int unit,
     int node_type,
     int nof_tunnels,
     int port_1,
     int port_2,
     int port_3,
     int port_4)
{
    if (node_type == BFIR)
    {
        cint_bier_info.ce1_port = port_1;
        cint_bier_info.bfr3_port = port_2;
        cint_bier_info.bfr5_port = port_3;

    }
    else if (node_type == BFER)
    {
        cint_bier_info.bfr4_port = port_1;
        cint_bier_info.bfr6_port = port_2;
        cint_bier_info.ce3_port =  port_3;
        cint_bier_info.ce4_port =  port_4;

    }
    else {
        cint_bier_info.bfr3_port = port_1;
        cint_bier_info.bfr6_port = port_2;
        cint_bier_info.bfr7_port = port_3;
        cint_bier_info.bfr8_port = port_4;
    }
    return BCM_E_NONE;

} 
 
 /**
  * Main entrance for MPLS BIER
  * INPUT:
  *   node_type    - node type 
  *   nof_tunnels  - number of mpls tunnels
  *   port_1       - port_1
  *   port_2       - port_2
  *   port_3       - port_3
  *   port_4       - port_4
  *   payload      - payload
 */
  int
  mpls_bier_edge_node_example(
    int unit,
    int node_type,
    int nof_tunnels,
    int port_1,
    int port_2,
    int port_3,
    int port_4,
    int payload)
 {
 
    mpls_bier_init_para(unit, node_type, nof_tunnels, port_1, port_2, port_3, port_4);   
 
    BCM_IF_ERROR_RETURN_MSG(mpls_bier_port_configuration(unit, node_type), "");

    BCM_IF_ERROR_RETURN_MSG(mpls_bier_l3_intf_configuration(unit, node_type), "");
 
    BCM_IF_ERROR_RETURN_MSG(mpls_bier_arp_configuration(unit, node_type), "");

     
    BCM_IF_ERROR_RETURN_MSG(mpls_bier_create_recycle_entry(unit, node_type), "");


    BCM_IF_ERROR_RETURN_MSG(mpls_bier_mpls_tunnel_configure(unit, node_type, nof_tunnels, payload), "");

    BCM_IF_ERROR_RETURN_MSG(mpls_bier_bift_entry_create(unit, node_type,  payload), "");

    if (node_type == BFER) {
        BCM_IF_ERROR_RETURN_MSG(mpls_bier_2nd_pass_terminator_create(unit, 1), "");
    }
    else
    {
        BCM_IF_ERROR_RETURN_MSG(mpls_bier_initiator_create(unit), "");
    }

    BCM_IF_ERROR_RETURN_MSG(mpls_bier_mpls_app_label_configure(unit, node_type, payload), "");

    BCM_IF_ERROR_RETURN_MSG(mpls_bier_forward_entry_configuration(unit, node_type, payload, 1), "");
    
    int mc_mode = 0;
    uint32 *ingress_only = dnxc_data_get(unit, "bier", "params", "ingress_only", NULL);
    mc_mode = (*ingress_only == 1)?1:0; 
    BCM_IF_ERROR_RETURN_MSG(mpls_bier_multicast_group_configuration(unit, node_type,payload,mc_mode, 1), "");

    return BCM_E_NONE;
 }

  int
  mpls_bier_transit_node_example(
    int unit,
    int nof_tunnels,
    int port_1,
    int port_2,
    int port_3,
    int port_4,
    int payload,
    int mode)
 {

    mpls_bier_init_para(unit, BFR, nof_tunnels, port_1, port_2, port_3, port_4);

    BCM_IF_ERROR_RETURN_MSG(mpls_bier_port_configuration(unit, BFR), "");

    BCM_IF_ERROR_RETURN_MSG(mpls_bier_l3_intf_configuration(unit, BFR), "");

    BCM_IF_ERROR_RETURN_MSG(mpls_bier_arp_configuration(unit, BFR), "");

    BCM_IF_ERROR_RETURN_MSG(mpls_bier_mpls_tunnel_configure(unit, BFR, nof_tunnels, payload), "");

    BCM_IF_ERROR_RETURN_MSG(mpls_bier_bift_entry_create(unit, BFR, payload), "");


    BCM_IF_ERROR_RETURN_MSG(mpls_bier_forward_entry_configuration(unit, BFR, payload, 1), "");

    BCM_IF_ERROR_RETURN_MSG(mpls_bier_multicast_group_configuration(unit, BFR,payload,mode, 1), "");

    return BCM_E_NONE;
 }


/* Replace part of the LIF common properties*/
int
mpls_bier_initiator_replace(
    int unit,
    int global_lif,
    bcm_flow_initiator_info_t info)
{
    bcm_flow_handle_t flow_handle;
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_field_id_t field_id;
    bcm_flow_special_fields_t special_fields;
    int nof_spec_fields = 0;

    BCM_IF_ERROR_RETURN_MSG(bcm_flow_handle_get(unit, "BIER_BASE_INITIATOR",&flow_handle), "");

    flow_handle_info.flow_handle = flow_handle;
    flow_handle_info.flags = BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID;
    flow_handle_info.flow_id = global_lif;

    if ((info.valid_elements_set & BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID) && (info.egress_qos_model.egress_qos != bcmQosEgressModelUniform))
    {
        special_fields.actual_nof_special_fields = 1;
        BCM_IF_ERROR_RETURN_MSG(bcm_flow_logical_field_id_get(unit, flow_handle, "QOS_DSCP", &field_id), "");
        special_fields.special_fields[nof_spec_fields].field_id = field_id;
        special_fields.special_fields[nof_spec_fields].shr_var_uint32 = cint_bier_info.dscp;
        nof_spec_fields++;
    } else {
        /* Keep original value from the previous flow create */
        special_fields.actual_nof_special_fields = 0;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_flow_initiator_info_create(unit, &flow_handle_info, info, &special_fields), "");

    return BCM_E_NONE;
}

 int
 mpls_bier_bfr_ecmp_create(
    int unit,
    int payload,
    int mode)
{
    int i;
    bcm_multicast_replication_t replications[3];
    uint32 bfr_id[3];
    int rep_count = 2;
    uint32 no_egress_bier_process = *dnxc_data_get(unit, "bier", "params", "ingress_only", NULL);
    uint32 bit_str_in_mcdb = *dnxc_data_get(unit, "bier", "params", "bfr_bit_str_in_mcdb", NULL);
    uint32 multicast_v2_format = *dnxc_data_get(unit, "multicast", "params", "mcdb_formats_v2", NULL);
    uint32 static_flag = (multicast_v2_format == 1)?BCM_MULTICAST_ELEMENT_STATIC:0;
    int bier_mc_id[2] = {6400,6656};
    int fec_ids[2];
    bcm_if_t ecmp_intf = 100;
    int mc_gport;

    /* Remove existed groups */
    if (mode !=0)
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_group_destroy(unit, BCM_MULTICAST_INGRESS_GROUP|BCM_MULTICAST_BIER_256_GROUP|static_flag, cint_bier_info.bier_mc_id), "");
    }

    if (!no_egress_bier_process)
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_group_destroy(unit, BCM_MULTICAST_EGRESS_GROUP|BCM_MULTICAST_BIER_256_GROUP, cint_bier_info.bier_mc_id), "");
    }

    /* Create 2 bier groups and add members */
    for (i = 0; i < 2; i++)
    {
        /*
         * mode:
         *     0: fabric+egress
         *     1: Ingress only
         *     2: Ingress+Egress
         */
        if (*dnxc_data_get(unit, "fabric", "general", "blocks_exist", NULL)
            &&*dnxc_data_get(unit, "device", "general", "hw_support", NULL)
            && (mode == 0))
        {
            int nof_cores = *dnxc_data_get(unit, "device", "general", "valid_cores_bitmap", NULL);
            bcm_module_t mreps[nof_cores];
            uint32 valid_cores_bmp = *(dnxc_data_get(unit, "device", "general", "valid_cores_bitmap", NULL));
            int nof_copies = 0;
            int core_index;
            for(core_index = 0; core_index < nof_cores; core_index++) {
                if ( valid_cores_bmp & (1 << (core_index))){
                    mreps[nof_copies]= core_index;
                    nof_copies ++;
                }
            }
            BCM_IF_ERROR_RETURN_MSG(bcm_fabric_multicast_set(unit, bier_mc_id[i], 0, nof_copies, mreps), "");
        }

        if (mode == 1)
        {
            /*
             * Ingress MC
             */
            BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, BCM_MULTICAST_WITH_ID|BCM_MULTICAST_INGRESS_GROUP|BCM_MULTICAST_BIER_256_GROUP|static_flag, 
                                            &bier_mc_id[i]), "");

            /* 1st(continue) copy: to BFR6*/
            bcm_multicast_replication_t_init(&replications[0]);
            bfr_id[0] = cint_bier_info.bfr6_id;
            replications[0].encap1 = !bit_str_in_mcdb?cint_bier_info.bfr4_bfr6_bift_tunnel_id&0x3FFFFF:(bier_mc_id[i] + cint_bier_info.bfr6_id);
            replications[0].port = (i==0) ? cint_bier_info.bfr6_port : cint_bier_info.bfr7_port; /* 1st path all goes to a single port */

            /* 2nd(continue) copy: to BFR7*/
            bcm_multicast_replication_t_init(&replications[1]);
            bfr_id[1] = cint_bier_info.bfr7_id;
            replications[1].encap1 = !bit_str_in_mcdb?cint_bier_info.bfr4_bfr7_bift_tunnel_id&0x3FFFFF:(bier_mc_id[i] + cint_bier_info.bfr7_id);
            replications[1].port = (i==0) ? cint_bier_info.bfr6_port : cint_bier_info.bfr7_port;

            /* 3rd(continue) copy: to BFR8*/
            bcm_multicast_replication_t_init(&replications[2]);
            bfr_id[2] = cint_bier_info.bfr8_id;
            replications[2].encap1 = !bit_str_in_mcdb?cint_bier_info.bfr4_bfr8_bift_tunnel_id&0x3FFFFF:(bier_mc_id[i] + cint_bier_info.bfr8_id);
            replications[2].port = (i==0) ? cint_bier_info.bfr6_port : cint_bier_info.bfr7_port;

            /*MCDB setting*/
            BCM_IF_ERROR_RETURN_MSG(bcm_multicast_bfr_id_add(unit, bier_mc_id[i], BCM_MULTICAST_INGRESS_GROUP, 3, bfr_id, replications), "");
        }

        if (mode == 2)
        {
            /*
             * Ingress MC
             */
            bcm_port_interface_info_t interface_info;
            bcm_port_mapping_info_t mapping_info;
            uint32 flags = 0;
            int count_erp = 0;
            int erp_sysport_gport;
            int erp_port[2];
            int erp_sys_port[2];
            int j;

            BCM_IF_ERROR_RETURN_MSG(bcm_port_internal_get(unit, BCM_PORT_INTERNAL_EGRESS_REPLICATION, 2, erp_port, &count_erp), "");

            for (j = 0; j < count_erp; j++) {
                BCM_IF_ERROR_RETURN_MSG(bcm_stk_gport_sysport_get(unit, erp_port[j], &erp_sysport_gport), "");
                erp_sys_port[j] = erp_sysport_gport;
            }

            BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, BCM_MULTICAST_WITH_ID|BCM_MULTICAST_BIER_256_GROUP|BCM_MULTICAST_INGRESS_GROUP, &bier_mc_id[i]), "");

            /* 1st(continue) copy: to BFR6*/
            bcm_port_get(unit,cint_bier_info.bfr6_port,&flags,&interface_info,&mapping_info);
            bcm_multicast_replication_t_init(&replications[0]);
            bfr_id[0] = cint_bier_info.bfr6_id;
            replications[0].encap1 = bier_mc_id[i];
            replications[0].port = mapping_info.core?erp_sys_port[1]:erp_sys_port[0];

            /* 2nd(continue) copy: to BFR7*/
            bcm_port_get(unit,cint_bier_info.bfr7_port,&flags,&interface_info,&mapping_info);
            bcm_multicast_replication_t_init(&replications[1]);
            bfr_id[1] = cint_bier_info.bfr7_id;
            replications[1].encap1 = bier_mc_id[i];
            replications[1].port = mapping_info.core?erp_sys_port[1]:erp_sys_port[0];

            /* 3rd(continue) copy: to BFR8*/
            bcm_port_get(unit,cint_bier_info.bfr8_port,&flags,&interface_info,&mapping_info);
            bcm_multicast_replication_t_init(&replications[2]);
            bfr_id[2] = cint_bier_info.bfr8_id;
            replications[2].encap1 = bier_mc_id[i];
            replications[2].port = mapping_info.core?erp_sys_port[1]:erp_sys_port[0];

            /*MCDB setting*/
            BCM_IF_ERROR_RETURN_MSG(bcm_multicast_bfr_id_add(unit, bier_mc_id[i], BCM_MULTICAST_INGRESS_GROUP, 3, bfr_id, replications), "");

        }

        if (!no_egress_bier_process)
        {
            /*
             * Egress MC
             */
            BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, BCM_MULTICAST_WITH_ID|BCM_MULTICAST_EGRESS_GROUP|BCM_MULTICAST_BIER_256_GROUP, &bier_mc_id[i]), "");

            /*
             * 1st PASS MC, for BIER header
             */
            /* 1st(continue) copy: to BFR6*/
            bcm_multicast_replication_t_init(&replications[0]);
            bfr_id[0] = cint_bier_info.bfr6_id;
            replications[0].encap1 = BCM_L3_ITF_VAL_GET(cint_bier_info.bfr4_bfr6_bift_tunnel_id);;
            replications[0].port = (i==0) ? cint_bier_info.bfr6_port : cint_bier_info.bfr7_port;

            /* 2nd(continue) copy: to BFR7*/
            bcm_multicast_replication_t_init(&replications[1]);
            bfr_id[1] = cint_bier_info.bfr7_id;
            replications[1].encap1 = BCM_L3_ITF_VAL_GET(cint_bier_info.bfr4_bfr7_bift_tunnel_id);
            replications[1].port = (i==0) ? cint_bier_info.bfr6_port : cint_bier_info.bfr7_port;

            /* 3rd(continue) copy: to BFR8*/
            bcm_multicast_replication_t_init(&replications[2]);
            bfr_id[2] = cint_bier_info.bfr8_id;
            replications[2].encap1 = BCM_L3_ITF_VAL_GET(cint_bier_info.bfr4_bfr8_bift_tunnel_id);
            replications[2].port = (i==0) ? cint_bier_info.bfr6_port : cint_bier_info.bfr7_port;

            /*MCDB setting*/
            BCM_IF_ERROR_RETURN_MSG(bcm_multicast_bfr_id_add(unit, bier_mc_id[i], BCM_MULTICAST_EGRESS_GROUP, 3, bfr_id, replications), "");
        }
    }

    /* Create FECs pointing to mc group */
    int flags2;
    int fec_direction;
    if (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
        fec_direction = 0;
    }
    else
    {
        fec_direction = 1;
    }

    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, fec_direction, &fec_ids[0]), "");

    fec_ids[1] = fec_ids[0] + 1;
    for (i = 0; i < 2; i++)
    {
        BCM_GPORT_MCAST_SET(mc_gport, bier_mc_id[i]);
        BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, fec_ids[i], 0, 0, mc_gport, 0, flags2), "");
    }

    /* 
     * Create ecmp
     */
    bcm_l3_egress_ecmp_t ecmp;
    BCM_IF_ERROR_RETURN_MSG(l3__ecmp_create(unit, ecmp_intf, 2, fec_ids, BCM_L3_WITH_ID, BCM_L3_ECMP_DYNAMIC_MODE_DISABLED,&ecmp,0), "");

    /* Replace MPLS fwd entry to point to ECMP */
    bcm_mpls_tunnel_switch_t entry;
    bcm_mpls_tunnel_switch_t_init(&entry);

    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT | BCM_MPLS_SWITCH_REPLACE | BCM_MPLS_SWITCH_WITH_ID;
    entry.action = BCM_MPLS_SWITCH_ACTION_NOP;
    entry.label = cint_bier_info.bfr4_bift_id;
    entry.mc_group = 0;
    BCM_GPORT_FORWARD_PORT_SET(entry.port, ecmp_intf);
    BCM_IF_ERROR_RETURN_MSG(bcm_mpls_tunnel_switch_create(unit, &entry), "bier ILM");

    return BCM_E_NONE;
}
