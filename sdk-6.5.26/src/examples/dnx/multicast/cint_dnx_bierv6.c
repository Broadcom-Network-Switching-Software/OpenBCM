/* 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File: cint_dnx_bierv6.c
 * Purpose: Example of a G-BIER application. 
 */
/*
 * Test Scenario 
 * ./bcm.user
 * cd ../../../../regress/bcm
 * bcm shell "config add bier_nof_bfr_entries.0=8192"
 *
 * bcm shell "config add multicast_nof_egress_bitmap.0=0x1000"
 *
 * bcm shell "config add multicast_ingress_group_id_range_max.0=0xa000"
 *
 * bcm shell "config add multicast_egress_group_id_range_max.0=0xa000"
 *
 * bcm shell "config add tm_port_header_type_in_50.0=RCH_0"
 *
 * bcm shell "config add tm_port_header_type_out_50.0=ETH"
 *
 * bcm shell "config add ucode_port_50.0=RCY.10:core_0.50"
 *
 * bcm shell "0: tr 141 NoInit=1"
 *
 * bcm shell "config add custom_feature_init_verify=1"
 *
 * bcm shell "config delete custom_feature_multithread_en*"
 *
 * bcm shell "config delete custom_feature_kbp_multithread_en*"
 *
 * bcm shell "0: tr 141 NoDeinit=1"
 *
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_global.c
 * cint ../../src/./examples/dnx/field/cint_field_utils.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_l3.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_util_rch.c
 * cint ../../src/./examples/dnx/l3/cint_dnx_ip_route_basic.c
 * cint ../../src/./examples/dnx/ipmc/cint_dnx_ipmc_route_basic.c
 * cint ../../src/./examples/dnx/mpls/cint_dnx_mpls_mldp.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utility_mpls.c
 * cint ../../src/./examples/dnx/multicast/cint_dnx_mpls_bier.c
 * cint ../../src/./examples/dnx/multicast/cint_dnx_bierv6.c
 * cint
 * bierv6_edge_node_example(0,1,200,201,202,203,0);
 * exit;
 *
 * BIER over IPv6 over ethernet packet 
 * tx 1 psrc=200 data=0x00110000007700110000005586dd6000000000703c807450a508ee90391f6957f533001100227450a508ee90391f6957f733001100220405702c0012c080503000000584000000000000000000000000000000000000000000000000000000000000000000c04500004000000000800044ae0a0a0a02e0020202c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *  Received packets on unit 0 should be: 
 *  Source port: 0, Destination port: 0 
 *  Data: 0x0011000000660011000000778100005086dd60088a8400703c7f7450a508ee90391f6957f733001100227450a508ee90391f6957f633001100220405702c0012c07f503000000584000000000000000000000000000000000000000000000000000000000000000000404500004000000000800044ae0a0a0a02e0020202c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5 
 *  Source port: 0, Destination port: 0 
 *  Data: 0x00110000ce030011000000778100001e080045000040000000007e0046ae0a0a0a02e0020202c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5 
 *  Source port: 0, Destination port: 0 
 *  Data: 0x00110000ce0400110000007781000028080045000040000000007e0046ae0a0a0a02e0020202c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5 
 *
 * bcm shell "config delete bier_nof_bfr_entries.0"
 *
 * bcm shell "config delete multicast_nof_egress_bitmap.0"
 *
 * bcm shell "config delete multicast_ingress_group_id_range_max.0"
 *
 * bcm shell "config delete multicast_egress_group_id_range_max.0"
 *
 * bcm shell "config delete tm_port_header_type_in_50.0"
 *
 * bcm shell "config delete tm_port_header_type_out_50.0"
 *
 * bcm shell "config delete ucode_port_50.0"
 *
 * bcm shell "0: tr 141 NoInit=1"
 *
 * bcm shell "config add custom_feature_init_verify=1"
 *
 * bcm shell "config delete custom_feature_multithread_en*"
 *
 * bcm shell "config delete custom_feature_kbp_multithread_en*"
 *
 * bcm shell "0: tr 141 NoDeinit=1"
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
 * BIERv6(G-BIER) configuration steps:
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
 *      1.4) Create overlay entry for 2nd PASS
 *      1.5) Create IPv6 tunnel to BFR1,BFR3 and BFR5, For BFR1, pointing to overlay
 *      1.6) Create BIERv6 BIFT entry assigned by BFR3, next pointer is IPv6 tunnel.
 *      1.7) Create BIERv6 BIFT entry assigned by BFR5, next pointer is IPv6 tunnel.
 *      1.8) Create BIERv6 BIFT entry assigned by BFR1, next pointer is IPv6 tunnel.
 *      1.8) Create BIER header entry , next pointer is BFR1 BIFT  entry.
 *      1.9) Create native multicast group with 1 replications:
 *           - cud = BIER header , Destination = RCY port
 *      1.10) Create BIER multicast group with 2 replications:
 *           - cud = BIERv6 BIFT entry to BFR3, Destination = Port to BFR3
 *           - cud = BIERv6 BIFT entry to BFR5, Destination = Port to BFR5
 *      1.11) Set ntavie forwarding(IPv4/IPv6/MPLS/Eth) entry, destination is MC ID in 1.9
 *      1.12) Set NON MPLS BIER forward entry for BFR1 BIFT label, destination is MCID in 1.10
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
 *      2.6) Create egress IPv6 tunnels to BFR6.
 *      2.7) Create overlay entry for 2nd PASS
 *      2.8) Create BIERv6 BIFT header entry from BFR7 to BFR6, next pointer is IPv6 tunnel.
 *      2.9) Create BIERv6 BIFT header entry for BFR7 drop copy, next pointer is overlay entry
 *      2.10) Create BIER multicast group with 2 replications:
 *           - cud = BIFT-tunnel to BFR6, Destination = port to BFR5
 *           - cud = BIFT-tunnel to BFR7 drop copy , Destination = RCY port
 *      2.11) Create native multicast group with 2 replications:
 *           - cud = ARP to CE3, Destination = Port to CE3
 *           - cud = ARP to CE4, Destination = Port to CE4
 *      2.12) Set 2ND Pass BIFT termination 
 *      2.13) Set ntavie forwarding(IPv4/IPv6/MPLS/Eth) entry, destination is MC ID in 2.10
 *      2.14) Set NON MPLS BIER forward  entry for BFR7 BIFT label, destination is MCID in 2.9
 *   
 * Note: for BFR, the configuration is similar with continue copy in BUD node.
 */


struct cint_bierv6_info_s {
    
    /*
     * {
     * Overlay network data
     */
    bcm_ip6_t    bfr1_ipv6_ip_addr;          /* BFR1 IPv6 IP */
    bcm_ip6_t    bfr2_ipv6_ip_addr;          /* BFR2 IPv6 IP */
    bcm_ip6_t    bfr3_ipv6_ip_addr;          /* BFR3 IPv6 IP */
    bcm_ip6_t    bfr4_ipv6_ip_addr;          /* BFR4 IPv6 IP */
    bcm_ip6_t    bfr5_ipv6_ip_addr;          /* BFR5 IPv6 IP */
    bcm_ip6_t    bfr6_ipv6_ip_addr;          /* BFR6 IPv6 IP */
    bcm_ip6_t    bfr7_ipv6_ip_addr;          /* BFR7 IPv6 IP */
    bcm_ip6_t    bfr8_ipv6_ip_addr;          /* BFR8 IPv6 IP */
    bcm_ip6_t    ipv6_ip_addr_mask;          /* IPv6 DIP MASK */


    /*
     * Overlay network data
     * }
     */


    /*
     * {
     * Native forwarding data
     */


    /*
     * Native forwarding data
     * }
     */
    bcm_gport_t bfr1_bfr3_ipv6_doh_tunnel_id; /* DOH Tunnel id of bfr1---->bfr3 */
    bcm_gport_t bfr1_bfr5_ipv6_doh_tunnel_id; /* DOH Tunnel id of bfr1---->bfr5 */
    bcm_gport_t bfr7_bfr6_ipv6_doh_tunnel_id; /* DOH Tunnel id of bfr7---->bfr6 */
    bcm_gport_t bfr4_bfr6_ipv6_doh_tunnel_id; /* DOH Tunnel id of bfr4---->bfr6 */
    bcm_gport_t bfr4_bfr7_ipv6_doh_tunnel_id; /* DOH Tunnel id of bfr4---->bfr7 */
    bcm_gport_t bfr4_bfr8_ipv6_doh_tunnel_id; /* DOH Tunnel id of bfr4---->bfr8 */


    bcm_gport_t bfr1_bfr3_ipv6_tunnel_id; /* Tunnel id of bfr1---->bfr3 */
    bcm_gport_t bfr1_bfr1_ipv6_tunnel_id; /* Tunnel id of bfr1---->bfr1 */
    bcm_gport_t bfr1_bfr5_ipv6_tunnel_id; /* Tunnel id of bfr1---->bfr5 */
    bcm_gport_t bfr7_bfr6_ipv6_tunnel_id; /* Tunnel id of bfr7---->bfr6 */
    bcm_gport_t bfr4_bfr6_ipv6_tunnel_id; /* Tunnel id of bfr4---->bfr6 */
    bcm_gport_t bfr4_bfr7_ipv6_tunnel_id; /* Tunnel id of bfr4---->bfr7 */
    bcm_gport_t bfr4_bfr8_ipv6_tunnel_id; /* Tunnel id of bfr4---->bfr8 */

};



cint_bierv6_info_s cint_bierv6_info = 
{
    /*
     * bfr1 ipv6 ip
     */
    {0x74, 0x50, 0xA5, 0x08, 0xEE, 0x90, 0x39, 0x1F, 0x69, 0x57, 0xF1, 0x33, 0x00, 0x11, 0x00, 0x22},
    /*
     * bfr2 ipv6 ip
     */
    {0x74, 0x50, 0xA5, 0x08, 0xEE, 0x90, 0x39, 0x1F, 0x69, 0x57, 0xF2, 0x33, 0x00, 0x11, 0x00, 0x22},
    /*
     * bfr3 ipv6 ip
     */
    {0x74, 0x50, 0xA5, 0x08, 0xEE, 0x90, 0x39, 0x1F, 0x69, 0x57, 0xF3, 0x33, 0x00, 0x11, 0x00, 0x22},
    /*
     * bfr4 ipv6 ip
     */
    {0x74, 0x50, 0xA5, 0x08, 0xEE, 0x90, 0x39, 0x1F, 0x69, 0x57, 0xF4, 0x33, 0x00, 0x11, 0x00, 0x22},
    /*
     * bfr5 ipv6 ip
     */
    {0x74, 0x50, 0xA5, 0x08, 0xEE, 0x90, 0x39, 0x1F, 0x69, 0x57, 0xF5, 0x33, 0x00, 0x11, 0x00, 0x22},
    /*
     * bfr6 ipv6 ip
     */
    {0x74, 0x50, 0xA5, 0x08, 0xEE, 0x90, 0x39, 0x1F, 0x69, 0x57, 0xF6, 0x33, 0x00, 0x11, 0x00, 0x22},
    /*
     * bfr7 ipv6 ip
     */
    {0x74, 0x50, 0xA5, 0x08, 0xEE, 0x90, 0x39, 0x1F, 0x69, 0x57, 0xF7, 0x33, 0x00, 0x11, 0x00, 0x22},
    /*
     * bfr8 ipv6 ip
     */
    {0x74, 0x50, 0xA5, 0x08, 0xEE, 0x90, 0x39, 0x1F, 0x69, 0x57, 0xF8, 0x33, 0x00, 0x11, 0x00, 0x22},
    /*
     * tunnel DIP mask
     */
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}


};


/**
 * IPv6 tunnel encap adding
 * INPUT:
 *   unit    - traffic outgoing unit 
*/
int
bierv6_ipv6_tunnel_encap_configure(
    int unit,
    int node_type,
    int payload)
{
    int rv;
    bcm_flow_initiator_info_t info;
    bcm_flow_handle_t flow_handle;
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_special_fields_t special_fields;
    bcm_flow_field_id_t field_id;
    uint32 bit_str_in_mcdb = *dnxc_data_get(unit, "bier", "params", "bfr_bit_str_in_mcdb", NULL);

    char *proc_name;

    proc_name = "bierv6_ipv6_tunnel_encap_configure";

    if (node_type == BFIR)
    {
        /*BFR1----->BFR3*/
        BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "IPV6_TUNNEL_INITIATOR",&flow_handle));
        
        bcm_flow_handle_info_t_init(&flow_handle_info);
        bcm_flow_special_fields_t_init(&special_fields);
        
        flow_handle_info.flow_handle = flow_handle;
        flow_handle_info.flags = 0;
        info.valid_elements_set |=
            (BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID );
        
        info.l3_intf_id = cint_bier_info.bfr1_bfr3_arp_id;
        info.encap_access = bcmEncapAccessTunnel3;
        
        special_fields.actual_nof_special_fields = 4;
        
        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "IPV6_DIP", &field_id));
        special_fields.special_fields[0].field_id = field_id;
        special_fields.special_fields[0].shr_var_uint8_arr = cint_bierv6_info.bfr3_ipv6_ip_addr;
        
        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "IPV6_SIP", &field_id));
        special_fields.special_fields[1].field_id = field_id;
        special_fields.special_fields[1].shr_var_uint8_arr = cint_bierv6_info.bfr1_ipv6_ip_addr;
        
        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "IPV6_TUNNEL_TYPE", &field_id));
        special_fields.special_fields[2].field_id = field_id;
        special_fields.special_fields[2].symbol = bcmTunnelTypeIpAnyIn6;

        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "DEST_OPT_PRESENT", &field_id));
        special_fields.special_fields[3].field_id = field_id;
        special_fields.special_fields[3].shr_var_uint32 = 1;

        
        /** special fields configuration */
        BCM_IF_ERROR_RETURN(bcm_flow_initiator_info_create(unit, &flow_handle_info, &info, &special_fields));
        
        if (rv != BCM_E_NONE) {
            printf("%s()Error, bcm_flow_initiator_info_create \n",proc_name);
        }

        printf ("%s() ipv6 tunnel bfr1-->bfr3 pass initiator_id=0x%x\n", proc_name,flow_handle_info.flow_id);
        cint_bierv6_info.bfr1_bfr3_ipv6_tunnel_id = flow_handle_info.flow_id;

        /*BFR1----->BFR5*/
        BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "IPV6_TUNNEL_INITIATOR",&flow_handle));
        
        bcm_flow_handle_info_t_init(&flow_handle_info);
        bcm_flow_special_fields_t_init(&special_fields);
        
        flow_handle_info.flow_handle = flow_handle;
        flow_handle_info.flags = 0;
        info.valid_elements_set |=
            (BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID );
        
        info.l3_intf_id = cint_bier_info.bfr1_bfr5_arp_id;
        info.encap_access = bcmEncapAccessTunnel3;
        
        special_fields.actual_nof_special_fields = 4;
        
        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "IPV6_DIP", &field_id));
        special_fields.special_fields[0].field_id = field_id;
        special_fields.special_fields[0].shr_var_uint8_arr = cint_bierv6_info.bfr5_ipv6_ip_addr;
        
        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "IPV6_SIP", &field_id));
        special_fields.special_fields[1].field_id = field_id;
        special_fields.special_fields[1].shr_var_uint8_arr = cint_bierv6_info.bfr1_ipv6_ip_addr;
        
        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "IPV6_TUNNEL_TYPE", &field_id));
        special_fields.special_fields[2].field_id = field_id;
        special_fields.special_fields[2].symbol = bcmTunnelTypeIpAnyIn6;

        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "DEST_OPT_PRESENT", &field_id));
        special_fields.special_fields[3].field_id = field_id;
        special_fields.special_fields[3].shr_var_uint32 = 1;
        
        /** special fields configuration */
        BCM_IF_ERROR_RETURN(bcm_flow_initiator_info_create(unit, &flow_handle_info, &info, &special_fields));
        
        if (rv != BCM_E_NONE) {
            printf("%s()Error, bcm_flow_initiator_info_create \n",proc_name);
        }

        printf ("%s() ipv6 tunnel bfr1-->bfr5 pass initiator_id=0x%x\n", proc_name,flow_handle_info.flow_id);
        cint_bierv6_info.bfr1_bfr5_ipv6_tunnel_id = flow_handle_info.flow_id;

        /*BFR1----->BFR1*/
        BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "IPV6_TUNNEL_INITIATOR",&flow_handle));

        bcm_flow_handle_info_t_init(&flow_handle_info);
        bcm_flow_special_fields_t_init(&special_fields);

        flow_handle_info.flow_handle = flow_handle;
        flow_handle_info.flags = 0;
        info.valid_elements_set |=
            (BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID );

        info.l3_intf_id = cint_bier_info.overlay_entry_id;
        info.encap_access = bcmEncapAccessTunnel3;

        special_fields.actual_nof_special_fields = 4;

        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "IPV6_DIP", &field_id));
        special_fields.special_fields[0].field_id = field_id;
        special_fields.special_fields[0].shr_var_uint8_arr = cint_bierv6_info.bfr1_ipv6_ip_addr;

        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "IPV6_SIP", &field_id));
        special_fields.special_fields[1].field_id = field_id;
        special_fields.special_fields[1].shr_var_uint8_arr = cint_bierv6_info.bfr1_ipv6_ip_addr;

        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "IPV6_TUNNEL_TYPE", &field_id));
        special_fields.special_fields[2].field_id = field_id;
        special_fields.special_fields[2].symbol = bcmTunnelTypeIpAnyIn6;

        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "DEST_OPT_PRESENT", &field_id));
        special_fields.special_fields[3].field_id = field_id;
        special_fields.special_fields[3].shr_var_uint32 = 1;

        /** special fields configuration */
        BCM_IF_ERROR_RETURN(bcm_flow_initiator_info_create(unit, &flow_handle_info, &info, &special_fields));
        if (rv != BCM_E_NONE) {
            printf("%s()Error, bcm_flow_initiator_info_create \n",proc_name);
        }

        printf ("%s() ipv6 tunnel bfr1-->bfr5 pass initiator_id=0x%x\n", proc_name,flow_handle_info.flow_id);
        cint_bierv6_info.bfr1_bfr1_ipv6_tunnel_id = flow_handle_info.flow_id;



    }
    else if (node_type == BFER)
    {
        /*BFR7----->BFR6*/
        BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "IPV6_TUNNEL_INITIATOR",&flow_handle));
        
        bcm_flow_handle_info_t_init(&flow_handle_info);
        bcm_flow_special_fields_t_init(&special_fields);
        
        flow_handle_info.flow_handle = flow_handle;
        flow_handle_info.flags = 0;
        info.valid_elements_set |=
            (BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID );
        
        info.l3_intf_id = cint_bier_info.bfr7_bfr6_arp_id;
        info.encap_access = bcmEncapAccessTunnel3;
        
        special_fields.actual_nof_special_fields = 4;
        
        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "IPV6_DIP", &field_id));
        special_fields.special_fields[0].field_id = field_id;
        special_fields.special_fields[0].shr_var_uint8_arr = cint_bierv6_info.bfr6_ipv6_ip_addr;
        
        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "IPV6_SIP", &field_id));
        special_fields.special_fields[1].field_id = field_id;
        special_fields.special_fields[1].shr_var_uint8_arr = cint_bierv6_info.bfr7_ipv6_ip_addr;
        
        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "IPV6_TUNNEL_TYPE", &field_id));
        special_fields.special_fields[2].field_id = field_id;
        special_fields.special_fields[2].symbol = bcmTunnelTypeIpAnyIn6;

        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "DEST_OPT_PRESENT", &field_id));
        special_fields.special_fields[3].field_id = field_id;
        special_fields.special_fields[3].shr_var_uint32 = 1;
        
        /** special fields configuration */
        BCM_IF_ERROR_RETURN(bcm_flow_initiator_info_create(unit, &flow_handle_info, &info, &special_fields));
        
        if (rv != BCM_E_NONE) {
            printf("%s()Error, bcm_flow_initiator_info_create \n",proc_name);
        }

        printf ("%s() ipv6 tunnel bfr7-->bfr6 pass initiator_id=0x%x\n", proc_name,flow_handle_info.flow_id);
        cint_bierv6_info.bfr7_bfr6_ipv6_tunnel_id = flow_handle_info.flow_id;



    }
    else
    {
        /*BFR4----->BFR6*/
        BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "IPV6_TUNNEL_INITIATOR",&flow_handle));
        
        bcm_flow_handle_info_t_init(&flow_handle_info);
        bcm_flow_special_fields_t_init(&special_fields);
        
        flow_handle_info.flow_handle = flow_handle;
        flow_handle_info.flags = 0;
        info.valid_elements_set |=
            (BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID );
        
        info.l3_intf_id = cint_bier_info.bfr4_bfr6_arp_id;
        info.encap_access = bcmEncapAccessTunnel3;
        
        special_fields.actual_nof_special_fields = 4;
        
        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "IPV6_DIP", &field_id));
        special_fields.special_fields[0].field_id = field_id;
        special_fields.special_fields[0].shr_var_uint8_arr = cint_bierv6_info.bfr6_ipv6_ip_addr;
        
        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "IPV6_SIP", &field_id));
        special_fields.special_fields[1].field_id = field_id;
        special_fields.special_fields[1].shr_var_uint8_arr = cint_bierv6_info.bfr4_ipv6_ip_addr;
        
        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "IPV6_TUNNEL_TYPE", &field_id));
        special_fields.special_fields[2].field_id = field_id;
        special_fields.special_fields[2].symbol = bcmTunnelTypeIpAnyIn6;

        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "DEST_OPT_PRESENT", &field_id));
        special_fields.special_fields[3].field_id = field_id;
        special_fields.special_fields[3].shr_var_uint32 = 1;
        
        /** special fields configuration */
        BCM_IF_ERROR_RETURN(bcm_flow_initiator_info_create(unit, &flow_handle_info, &info, &special_fields));
        
        if (rv != BCM_E_NONE) {
            printf("%s()Error, bcm_flow_initiator_info_create \n",proc_name);
        }

        printf ("%s() ipv6 tunnel bfr4-->bfr6 pass initiator_id=0x%x\n", proc_name,flow_handle_info.flow_id);
        cint_bierv6_info.bfr4_bfr6_ipv6_tunnel_id = flow_handle_info.flow_id;



        /*BFR4----->BFR7*/
        BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "IPV6_TUNNEL_INITIATOR",&flow_handle));
        
        bcm_flow_handle_info_t_init(&flow_handle_info);
        bcm_flow_special_fields_t_init(&special_fields);
        
        flow_handle_info.flow_handle = flow_handle;
        flow_handle_info.flags = 0;
        info.valid_elements_set |=
            (BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID );
        
        info.l3_intf_id = cint_bier_info.bfr4_bfr7_arp_id;
        info.encap_access = bcmEncapAccessTunnel3;
        
        special_fields.actual_nof_special_fields = 4;
        
        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "IPV6_DIP", &field_id));
        special_fields.special_fields[0].field_id = field_id;
        special_fields.special_fields[0].shr_var_uint8_arr = cint_bierv6_info.bfr7_ipv6_ip_addr;
        
        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "IPV6_SIP", &field_id));
        special_fields.special_fields[1].field_id = field_id;
        special_fields.special_fields[1].shr_var_uint8_arr = cint_bierv6_info.bfr4_ipv6_ip_addr;
        
        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "IPV6_TUNNEL_TYPE", &field_id));
        special_fields.special_fields[2].field_id = field_id;
        special_fields.special_fields[2].symbol = bcmTunnelTypeIpAnyIn6;

        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "DEST_OPT_PRESENT", &field_id));
        special_fields.special_fields[3].field_id = field_id;
        special_fields.special_fields[3].shr_var_uint32 = 1;

        
        /** special fields configuration */
        BCM_IF_ERROR_RETURN(bcm_flow_initiator_info_create(unit, &flow_handle_info, &info, &special_fields));
        
        if (rv != BCM_E_NONE) {
            printf("%s()Error, bcm_flow_initiator_info_create \n",proc_name);
        }

        printf ("%s() ipv6 tunnel bfr4-->bfr7 pass initiator_id=0x%x\n", proc_name,flow_handle_info.flow_id);
        cint_bierv6_info.bfr4_bfr7_ipv6_tunnel_id = flow_handle_info.flow_id;


        /*BFR4----->BFR8*/
        BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "IPV6_TUNNEL_INITIATOR",&flow_handle));
        
        bcm_flow_handle_info_t_init(&flow_handle_info);
        bcm_flow_special_fields_t_init(&special_fields);
        
        flow_handle_info.flow_handle = flow_handle;
        flow_handle_info.flags = 0;
        info.valid_elements_set |=
            (BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID );
        
        info.l3_intf_id = cint_bier_info.bfr4_bfr8_arp_id;
        info.encap_access = bcmEncapAccessTunnel3;
        
        special_fields.actual_nof_special_fields = 4;
        
        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "IPV6_DIP", &field_id));
        special_fields.special_fields[0].field_id = field_id;
        special_fields.special_fields[0].shr_var_uint8_arr = cint_bierv6_info.bfr8_ipv6_ip_addr;
        
        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "IPV6_SIP", &field_id));
        special_fields.special_fields[1].field_id = field_id;
        special_fields.special_fields[1].shr_var_uint8_arr = cint_bierv6_info.bfr4_ipv6_ip_addr;
        
        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "IPV6_TUNNEL_TYPE", &field_id));
        special_fields.special_fields[2].field_id = field_id;
        special_fields.special_fields[2].symbol = bcmTunnelTypeIpAnyIn6;

        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "DEST_OPT_PRESENT", &field_id));
        special_fields.special_fields[3].field_id = field_id;
        special_fields.special_fields[3].shr_var_uint32 = 1;

        
        /** special fields configuration */
        BCM_IF_ERROR_RETURN(bcm_flow_initiator_info_create(unit, &flow_handle_info, &info, &special_fields));
        
        if (rv != BCM_E_NONE) {
            printf("%s()Error, bcm_flow_initiator_info_create \n",proc_name);
        }

        printf ("%s() ipv6 tunnel bfr4-->bfr8 pass initiator_id=0x%x\n", proc_name,flow_handle_info.flow_id);
        cint_bierv6_info.bfr4_bfr8_ipv6_tunnel_id = flow_handle_info.flow_id;



    }


    return rv;
}

/**
 * IPv6 tunnel term adding
 * INPUT:
 *   unit    - traffic outgoing unit 
*/
int
bierv6_ipv6_tunnel_term_configure(
    int unit,
    int node_type)
{
    int rv = BCM_E_NONE;   
    bcm_flow_handle_t flow_handle;
    bcm_flow_terminator_info_t info;
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_special_fields_t special_fields;
    int terminator_id = 0;

    char *proc_name;

    proc_name = "bierv6_ipv6_tunnel_term_configure";

    if (node_type == BFIR)
    {
       
       bcm_flow_handle_info_t_init(&flow_handle_info);
       bcm_flow_special_fields_t_init(&special_fields);

        BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "IPVX_TUNNEL_TERMINATOR",&flow_handle));
        
        flow_handle_info.flow_handle = flow_handle;
        flow_handle_info.flags = 0;
        info.valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID |
                                   BCM_FLOW_TERMINATOR_ELEMENT_QOS_INGRESS_MODEL_VALID|
                                   BCM_FLOW_TERMINATOR_ELEMENT_VRF_VALID;
        bcm_flow_l3_ingress_info_t_init(&info.l3_ingress_info);
        info.l3_ingress_info.service_type = bcmFlowServiceTypeMultiPoint;
        info.vrf = cint_bier_info.vrf; 
        info.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;
        info.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
        info.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;

        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "IPVX_TUNNEL_TYPE", &special_fields.special_fields[0].field_id));
        special_fields.special_fields[0].symbol = bcmTunnelTypeIpAnyIn6;
        special_fields.actual_nof_special_fields = 1;

        BCM_IF_ERROR_RETURN(bcm_flow_terminator_info_create(unit, &flow_handle_info, &info, special_fields));
        
        cint_bier_info.bfr1_bift_term_id = flow_handle_info.flow_id;
        

        bcm_flow_handle_info_t_init(&flow_handle_info);
        bcm_flow_special_fields_t_init(&special_fields);

        /** point the LIF from the ISEM table */
        BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "IPV6_TUNNEL_MP_TCAM_TERM_MATCH", &flow_handle));
        flow_handle_info.flow_handle = flow_handle;
        flow_handle_info.flow_id = cint_bier_info.bfr1_bift_term_id;
        
        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(
                unit, flow_handle, "IPV6", &special_fields.special_fields[0].field_id));
        special_fields.special_fields[0].shr_var_uint8_arr = cint_bierv6_info.bfr1_ipv6_ip_addr;

        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(
                unit, flow_handle, "IPV6_MASK", &special_fields.special_fields[1].field_id));
        special_fields.special_fields[1].shr_var_uint8_arr = cint_bierv6_info.ipv6_ip_addr_mask;

        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "IPV6_TUNNEL_TYPE", &special_fields.special_fields[2].field_id));
        special_fields.special_fields[2].symbol = bcmTunnelTypeIpAnyIn6;

        special_fields.actual_nof_special_fields = 3;

        BCM_IF_ERROR_RETURN(bcm_flow_match_info_add(unit, &flow_handle_info, &special_fields));
        
        printf ("%s() pass. terminator_id=%x \n", proc_name,cint_bier_info.bfr1_bift_term_id);

    }
    else if (node_type == BFER)
    {

       
       bcm_flow_handle_info_t_init(&flow_handle_info);
       bcm_flow_special_fields_t_init(&special_fields);

        BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "IPVX_TUNNEL_TERMINATOR",&flow_handle));
        
        flow_handle_info.flow_handle = flow_handle;
        flow_handle_info.flags = 0;
        info.valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID |
                                   BCM_FLOW_TERMINATOR_ELEMENT_QOS_INGRESS_MODEL_VALID |
                                   BCM_FLOW_TERMINATOR_ELEMENT_VRF_VALID;

        bcm_flow_l3_ingress_info_t_init(&info.l3_ingress_info);
        info.l3_ingress_info.service_type = bcmFlowServiceTypeMultiPoint;

        info.vrf = cint_bier_info.vrf;        
        info.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;
        info.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
        info.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;

        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "IPVX_TUNNEL_TYPE", &special_fields.special_fields[0].field_id));
        special_fields.special_fields[0].symbol = bcmTunnelTypeIpAnyIn6;
        special_fields.actual_nof_special_fields = 1;

        BCM_IF_ERROR_RETURN(bcm_flow_terminator_info_create(unit, &flow_handle_info, &info, NULL));
        
        cint_bier_info.bfr7_bift_term_id = flow_handle_info.flow_id;
        

        bcm_flow_handle_info_t_init(&flow_handle_info);
        bcm_flow_special_fields_t_init(&special_fields);

        /** point the LIF from the ISEM table */
        BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "IPV6_TUNNEL_MP_TCAM_TERM_MATCH", &flow_handle));
        flow_handle_info.flow_handle = flow_handle;
        flow_handle_info.flow_id = cint_bier_info.bfr7_bift_term_id;
        
        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(
                unit, flow_handle, "IPV6", &special_fields.special_fields[0].field_id));
        special_fields.special_fields[0].shr_var_uint8_arr = cint_bierv6_info.bfr7_ipv6_ip_addr;

        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(
                unit, flow_handle, "IPV6_MASK", &special_fields.special_fields[1].field_id));
        special_fields.special_fields[1].shr_var_uint8_arr = cint_bierv6_info.ipv6_ip_addr_mask;

        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "IPV6_TUNNEL_TYPE", &special_fields.special_fields[2].field_id));
        special_fields.special_fields[2].symbol = bcmTunnelTypeIpAnyIn6;

        special_fields.actual_nof_special_fields = 3;

        BCM_IF_ERROR_RETURN(bcm_flow_match_info_add(unit, &flow_handle_info, &special_fields));
        
        printf ("%s() pass. terminator_id=%x \n", proc_name,cint_bier_info.bfr7_bift_term_id);
    }
    else
    {
        
        bcm_flow_handle_info_t_init(&flow_handle_info);
        bcm_flow_special_fields_t_init(&special_fields);
        
        BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "IPVX_TUNNEL_TERMINATOR",&flow_handle));

        flow_handle_info.flow_handle = flow_handle;
        flow_handle_info.flags = 0;
        info.valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID |
                                BCM_FLOW_TERMINATOR_ELEMENT_QOS_INGRESS_MODEL_VALID |
                                BCM_FLOW_TERMINATOR_ELEMENT_VRF_VALID;

        bcm_flow_l3_ingress_info_t_init(&info.l3_ingress_info);
        info.l3_ingress_info.service_type = bcmFlowServiceTypeMultiPoint;
        info.vrf = cint_bier_info.vrf;

        info.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;
        info.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
        info.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;

        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "IPVX_TUNNEL_TYPE", &special_fields.special_fields[0].field_id));
        special_fields.special_fields[0].symbol = bcmTunnelTypeIpAnyIn6;
        special_fields.actual_nof_special_fields = 1;

        BCM_IF_ERROR_RETURN(bcm_flow_terminator_info_create(unit, &flow_handle_info, &info, NULL));

        cint_bier_info.bfr4_bift_term_id = flow_handle_info.flow_id;


        bcm_flow_handle_info_t_init(&flow_handle_info);
        bcm_flow_special_fields_t_init(&special_fields);

        /** point the LIF from the ISEM table */
        BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "IPV6_TUNNEL_MP_TCAM_TERM_MATCH", &flow_handle));
        flow_handle_info.flow_handle = flow_handle;
        flow_handle_info.flow_id = cint_bier_info.bfr4_bift_term_id;

        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(
             unit, flow_handle, "IPV6", &special_fields.special_fields[0].field_id));
        special_fields.special_fields[0].shr_var_uint8_arr = cint_bierv6_info.bfr4_ipv6_ip_addr;

        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(
             unit, flow_handle, "IPV6_MASK", &special_fields.special_fields[1].field_id));
        special_fields.special_fields[1].shr_var_uint8_arr = cint_bierv6_info.ipv6_ip_addr_mask;

        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "IPV6_TUNNEL_TYPE", &special_fields.special_fields[2].field_id));
        special_fields.special_fields[2].symbol = bcmTunnelTypeIpAnyIn6;

        special_fields.actual_nof_special_fields = 3;

        BCM_IF_ERROR_RETURN(bcm_flow_match_info_add(unit, &flow_handle_info, &special_fields));

        printf ("%s() pass. terminator_id=%x \n", proc_name,cint_bier_info.bfr4_bift_term_id);

    }



    return rv;
}

/**
 * bift entry for bier header editing 
 * as transit role
 * INPUT:
 *   unit    - traffic outgoing unit 
*/
int
bierv6_bift_entry_create(
    int unit,
    int node_type,
    int payload)
{
    int rv = BCM_E_NONE;
    bcm_flow_initiator_info_t info;
    bcm_flow_handle_t flow_handle;
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_special_fields_t special_fields;
    bcm_flow_field_id_t field_id;
    bcm_if_t   intf_id = 0;
    uint32 bit_str_in_mcdb = *dnxc_data_get(unit, "bier", "params", "bfr_bit_str_in_mcdb", NULL);
    char *proc_name;

    proc_name = "bierv6_bift_entry_create";

    if (node_type == BFIR)
    {
        /*BFR1----->BFR3*/
        if (!bit_str_in_mcdb) 
        {
            BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "BIER_BITSTR_INITIATOR",&flow_handle));

            bcm_flow_handle_info_t_init(&flow_handle_info);
            bcm_flow_special_fields_t_init(&special_fields);

            flow_handle_info.flow_handle = flow_handle;
            flow_handle_info.flags = 0;
            info.valid_elements_set |=
                (BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID );

            BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(intf_id, cint_bierv6_info.bfr1_bfr3_ipv6_tunnel_id);
            info.l3_intf_id = intf_id;
            info.encap_access = bcmEncapAccessNativeArp;

            special_fields.actual_nof_special_fields = 1;

            BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "BFR_BIT_STR", &field_id));
            special_fields.special_fields[0].field_id = field_id;
            special_fields.special_fields[0].shr_var_uint32_arr[4] = 0x8;

            /** special fields configuration */
            BCM_IF_ERROR_RETURN(bcm_flow_initiator_info_create(unit, &flow_handle_info, &info, &special_fields));

            printf ("%s():bcm_flow_initiator_info_create bitstr entry pass. initiator_id=0x%x\n", 
                                                  proc_name,flow_handle_info.flow_id);
            BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(intf_id, flow_handle_info.flow_id);
        }

        /*BIFT entry*/
        BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "BIER_PREFIX_INITIATOR",&flow_handle));
        
        bcm_flow_handle_info_t_init(&flow_handle_info);
        bcm_flow_special_fields_t_init(&special_fields);
        
        flow_handle_info.flow_handle = flow_handle;
        flow_handle_info.flags = 0;
        info.valid_elements_set |=
            (BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID );
        
        info.l3_intf_id = intf_id;
        info.encap_access = bcmEncapAccessRif;

        /** special fields configuration */
        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "BIER_BIFT_ID", &field_id));
        special_fields.special_fields[0].field_id = field_id;
        special_fields.special_fields[0].shr_var_uint32 = cint_bier_info.bfr1_bift_id;       

        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "ENCAP_IPV6_DOH", &field_id));
        special_fields.special_fields[1].field_id = field_id;
        special_fields.special_fields[1].shr_var_uint32 = 1;

        special_fields.actual_nof_special_fields = 2;

        BCM_IF_ERROR_RETURN(bcm_flow_initiator_info_create(unit, &flow_handle_info, &info, &special_fields));
        
        printf ("%s():bcm_flow_initiator_info_create bier prefix entry pass. initiator_id=0x%x\n", 
                               proc_name,flow_handle_info.flow_id);

        BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(intf_id, flow_handle_info.flow_id);

        cint_bier_info.bfr1_bfr3_bift_tunnel_id = intf_id;


        /*BFR1----->BFR5*/
        if (!bit_str_in_mcdb)
        {
            BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "BIER_BITSTR_INITIATOR",&flow_handle));

            bcm_flow_handle_info_t_init(&flow_handle_info);
            bcm_flow_special_fields_t_init(&special_fields);

            flow_handle_info.flow_handle = flow_handle;
            flow_handle_info.flags = 0;
            info.valid_elements_set |=
                (BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID );

            BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(intf_id, cint_bierv6_info.bfr1_bfr5_ipv6_tunnel_id);
            info.l3_intf_id = intf_id;

            info.encap_access = bcmEncapAccessNativeArp;

            special_fields.actual_nof_special_fields = 1;

            BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "BFR_BIT_STR", &field_id));
            special_fields.special_fields[0].field_id = field_id;
            special_fields.special_fields[0].shr_var_uint32_arr[4] = 0x20;

            /** special fields configuration */
            BCM_IF_ERROR_RETURN(bcm_flow_initiator_info_create(unit, &flow_handle_info, &info, &special_fields));

            printf ("%s():bcm_flow_initiator_info_create bitstr entry pass. initiator_id=0x%x\n", 
                        proc_name,flow_handle_info.flow_id);
            BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(intf_id, flow_handle_info.flow_id);
        }

        /*BIFT entry*/
        BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "BIER_PREFIX_INITIATOR",&flow_handle));
        
        bcm_flow_handle_info_t_init(&flow_handle_info);
        bcm_flow_special_fields_t_init(&special_fields);
        
        flow_handle_info.flow_handle = flow_handle;
        flow_handle_info.flags = 0;
        info.valid_elements_set |=
            (BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID );
        
        info.l3_intf_id = intf_id;
        info.encap_access = bcmEncapAccessRif;

        /** special fields configuration */
        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "BIER_BIFT_ID", &field_id));
        special_fields.special_fields[0].field_id = field_id;
        special_fields.special_fields[0].shr_var_uint32 = cint_bier_info.bfr1_bift_id;       

        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "ENCAP_IPV6_DOH", &field_id));
        special_fields.special_fields[1].field_id = field_id;
        special_fields.special_fields[1].shr_var_uint32 = 1;

        special_fields.actual_nof_special_fields = 2;

        BCM_IF_ERROR_RETURN(bcm_flow_initiator_info_create(unit, &flow_handle_info, &info, &special_fields));
        
        printf ("%s():bcm_flow_initiator_info_create bier prefix entry pass. initiator_id=0x%x\n", 
                       proc_name,flow_handle_info.flow_id);

        BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(intf_id, flow_handle_info.flow_id);

        cint_bier_info.bfr1_bfr5_bift_tunnel_id = intf_id;


    }
    else if (node_type == BFER)
    {
        /*BFR7------->BFR6*/
        if (!bit_str_in_mcdb)
        {   
            BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "BIER_BITSTR_INITIATOR",&flow_handle));
            
            bcm_flow_handle_info_t_init(&flow_handle_info);
            bcm_flow_special_fields_t_init(&special_fields);
            
            flow_handle_info.flow_handle = flow_handle;
            flow_handle_info.flags = 0;
            info.valid_elements_set |=
                (BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID );

            BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(intf_id, cint_bierv6_info.bfr7_bfr6_ipv6_tunnel_id);
            
            info.l3_intf_id = intf_id;

            info.encap_access = bcmEncapAccessTunnel1;
            
            special_fields.actual_nof_special_fields = 1;
            
            BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "BFR_BIT_STR", &field_id));
            special_fields.special_fields[0].field_id = field_id;
            special_fields.special_fields[0].shr_var_uint32_arr[4] = 0x40;
            
            /** special fields configuration */
            BCM_IF_ERROR_RETURN(bcm_flow_initiator_info_create(unit, &flow_handle_info, &info, &special_fields));
            
            printf ("%s():bcm_flow_initiator_info_create bitstr entry pass. initiator_id=0x%x\n", 
                              proc_name,flow_handle_info.flow_id);
            BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(intf_id, flow_handle_info.flow_id);      
        }

        /*
         * BIFT entry for transit copy
         */
        /*BIFT entry BFR7-->BFR6*/
        BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "BIER_PREFIX_INITIATOR",&flow_handle));
        
        bcm_flow_handle_info_t_init(&flow_handle_info);
        bcm_flow_special_fields_t_init(&special_fields);
        
        flow_handle_info.flow_handle = flow_handle;
        flow_handle_info.flags = 0;
        info.valid_elements_set |=
            (BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID );
        
        info.l3_intf_id = intf_id;
        info.encap_access = bcmEncapAccessNativeArp;

        /** special fields configuration */
        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "BIER_BIFT_ID", &field_id));
        special_fields.special_fields[0].field_id = field_id;
        special_fields.special_fields[0].shr_var_uint32 = cint_bier_info.bfr1_bift_id;       

        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "ENCAP_IPV6_DOH", &field_id));
        special_fields.special_fields[1].field_id = field_id;
        special_fields.special_fields[1].shr_var_uint32 = 1;

        special_fields.actual_nof_special_fields = 2;

        BCM_IF_ERROR_RETURN(bcm_flow_initiator_info_create(unit, &flow_handle_info, &info, &special_fields));
        
        printf ("%s():bcm_flow_initiator_info_create bier prefix entry pass. initiator_id=0x%x\n", 
                               proc_name,flow_handle_info.flow_id);

        BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(intf_id, flow_handle_info.flow_id);
        cint_bier_info.bfr7_bfr6_bift_tunnel_id = intf_id;

        if (!bit_str_in_mcdb)
        {
            BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "BIER_BITSTR_INITIATOR",&flow_handle));

            bcm_flow_handle_info_t_init(&flow_handle_info);
            bcm_flow_special_fields_t_init(&special_fields);

            flow_handle_info.flow_handle = flow_handle;
            flow_handle_info.flags = 0;
            info.valid_elements_set |=
                (BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID );

            info.l3_intf_id = cint_bier_info.overlay_entry_id;
            info.encap_access = bcmEncapAccessTunnel1;

            special_fields.actual_nof_special_fields = 1;

            BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "BFR_BIT_STR", &field_id));
            special_fields.special_fields[0].field_id = field_id;
            special_fields.special_fields[0].shr_var_uint32_arr[4] = 0x20;

            /** special fields configuration */
            BCM_IF_ERROR_RETURN(bcm_flow_initiator_info_create(unit, &flow_handle_info, &info, &special_fields));

            printf ("%s():bcm_flow_initiator_info_create bitstr entry pass. initiator_id=0x%x\n", 
                                    proc_name,flow_handle_info.flow_id);
            BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(intf_id, flow_handle_info.flow_id);
        }

        /*
         * BIFT entry for local copy
         */
        /*BIFT entry local copy*/
        BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "BIER_PREFIX_INITIATOR",&flow_handle));
        
        bcm_flow_handle_info_t_init(&flow_handle_info);
        bcm_flow_special_fields_t_init(&special_fields);
        
        flow_handle_info.flow_handle = flow_handle;
        flow_handle_info.flags = 0;
        info.valid_elements_set |=
            (BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID );
        
        info.l3_intf_id = intf_id;
        info.encap_access = bcmEncapAccessNativeArp;

        /** special fields configuration */
        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "BIER_BIFT_ID", &field_id));
        special_fields.special_fields[0].field_id = field_id;
        special_fields.special_fields[0].shr_var_uint32 = cint_bier_info.bfr1_bift_id;       

        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "ENCAP_IPV6_DOH", &field_id));
        special_fields.special_fields[1].field_id = field_id;
        special_fields.special_fields[1].shr_var_uint32 = 1;

        special_fields.actual_nof_special_fields = 2;

        BCM_IF_ERROR_RETURN(bcm_flow_initiator_info_create(unit, &flow_handle_info, &info, &special_fields));
        
        printf ("%s():bcm_flow_initiator_info_create bier prefix entry pass. initiator_id=0x%x\n", 
                               proc_name,flow_handle_info.flow_id);

        BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(intf_id, flow_handle_info.flow_id);
        cint_bier_info.bfr7_drop_copy_bift_tunnel_id = intf_id;
    }
    else
    {
        /*BFR4----------->BFR6*/
        if (!bit_str_in_mcdb)
        {
            BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "BIER_BITSTR_INITIATOR",&flow_handle));

            bcm_flow_handle_info_t_init(&flow_handle_info);
            bcm_flow_special_fields_t_init(&special_fields);

            flow_handle_info.flow_handle = flow_handle;
            flow_handle_info.flags = 0;
            info.valid_elements_set |=
                (BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID );

            BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(intf_id, cint_bierv6_info.bfr4_bfr6_ipv6_tunnel_id);
            info.l3_intf_id = intf_id;

            info.encap_access = bcmEncapAccessTunnel1;

            special_fields.actual_nof_special_fields = 1;

            BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "BFR_BIT_STR", &field_id));
            special_fields.special_fields[0].field_id = field_id;
            special_fields.special_fields[0].shr_var_uint32_arr[4] = 0x40;

            /** special fields configuration */
            BCM_IF_ERROR_RETURN(bcm_flow_initiator_info_create(unit, &flow_handle_info, &info, &special_fields));

            printf ("%s():bcm_flow_initiator_info_create bitstr entry pass. initiator_id=0x%x\n", 
                                 proc_name,flow_handle_info.flow_id);
            BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(intf_id, flow_handle_info.flow_id);
        }

        /*BIFT entry BFR4-->BFR6*/
        BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "BIER_PREFIX_INITIATOR",&flow_handle));
        
        bcm_flow_handle_info_t_init(&flow_handle_info);
        bcm_flow_special_fields_t_init(&special_fields);
        
        flow_handle_info.flow_handle = flow_handle;
        flow_handle_info.flags = 0;
        info.valid_elements_set |=
            (BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID );
        
        info.l3_intf_id = intf_id;
        info.encap_access = bcmEncapAccessNativeArp;

        /** special fields configuration */
        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "BIER_BIFT_ID", &field_id));
        special_fields.special_fields[0].field_id = field_id;
        special_fields.special_fields[0].shr_var_uint32 = cint_bier_info.bfr1_bift_id;       

        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "ENCAP_IPV6_DOH", &field_id));
        special_fields.special_fields[1].field_id = field_id;
        special_fields.special_fields[1].shr_var_uint32 = 1;

        special_fields.actual_nof_special_fields = 2;

        BCM_IF_ERROR_RETURN(bcm_flow_initiator_info_create(unit, &flow_handle_info, &info, &special_fields));
        
        printf ("%s():bcm_flow_initiator_info_create bier prefix entry pass. initiator_id=0x%x\n", 
                               proc_name,flow_handle_info.flow_id);

        BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(intf_id, flow_handle_info.flow_id);
        cint_bier_info.bfr4_bfr6_bift_tunnel_id = intf_id;

        /*BFR4----------->BFR7*/
        if (!bit_str_in_mcdb)
        {
            BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "BIER_BITSTR_INITIATOR",&flow_handle));

            bcm_flow_handle_info_t_init(&flow_handle_info);
            bcm_flow_special_fields_t_init(&special_fields);

            flow_handle_info.flow_handle = flow_handle;
            flow_handle_info.flags = 0;
            info.valid_elements_set |=
                (BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID );

            BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(intf_id, cint_bierv6_info.bfr4_bfr7_ipv6_tunnel_id);
            info.l3_intf_id = intf_id;

            info.encap_access = bcmEncapAccessTunnel1;

            special_fields.actual_nof_special_fields = 1;

            BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "BFR_BIT_STR", &field_id));
            special_fields.special_fields[0].field_id = field_id;
            special_fields.special_fields[0].shr_var_uint32_arr[4] = 0x80;

            /** special fields configuration */
            BCM_IF_ERROR_RETURN(bcm_flow_initiator_info_create(unit, &flow_handle_info, &info, &special_fields));

            printf ("%s():bcm_flow_initiator_info_create bitstr entry pass. initiator_id=0x%x\n", 
                                proc_name,flow_handle_info.flow_id);
            BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(intf_id, flow_handle_info.flow_id);
        }

        /*BIFT entry BFR4-->BFR7*/
        BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "BIER_PREFIX_INITIATOR",&flow_handle));
        bcm_flow_handle_info_t_init(&flow_handle_info);
        bcm_flow_special_fields_t_init(&special_fields);

        flow_handle_info.flow_handle = flow_handle;
        flow_handle_info.flags = 0;
        info.valid_elements_set |=
            (BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID );

        info.l3_intf_id = intf_id;
        info.encap_access = bcmEncapAccessNativeArp;

        /** special fields configuration */
        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "BIER_BIFT_ID", &field_id));
        special_fields.special_fields[0].field_id = field_id;
        special_fields.special_fields[0].shr_var_uint32 = cint_bier_info.bfr1_bift_id;       

        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "ENCAP_IPV6_DOH", &field_id));
        special_fields.special_fields[1].field_id = field_id;
        special_fields.special_fields[1].shr_var_uint32 = 1;

        special_fields.actual_nof_special_fields = 2;

        BCM_IF_ERROR_RETURN(bcm_flow_initiator_info_create(unit, &flow_handle_info, &info, &special_fields));
        
        printf ("%s():bcm_flow_initiator_info_create bier prefix entry pass. initiator_id=0x%x\n", 
                               proc_name,flow_handle_info.flow_id);

        BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(intf_id, flow_handle_info.flow_id);

        cint_bier_info.bfr4_bfr7_bift_tunnel_id = intf_id;


        /*BFR4----------->BFR8*/
        if (!bit_str_in_mcdb)
        {   
            BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "BIER_BITSTR_INITIATOR",&flow_handle));
            
            bcm_flow_handle_info_t_init(&flow_handle_info);
            bcm_flow_special_fields_t_init(&special_fields);
            
            flow_handle_info.flow_handle = flow_handle;
            flow_handle_info.flags = 0;
            info.valid_elements_set |=
                (BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID );

            BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(intf_id, cint_bierv6_info.bfr4_bfr8_ipv6_tunnel_id);
            info.l3_intf_id = intf_id;
            info.encap_access = bcmEncapAccessTunnel1;
            
            special_fields.actual_nof_special_fields = 1;
            
            BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "BFR_BIT_STR", &field_id));
            special_fields.special_fields[0].field_id = field_id;
            special_fields.special_fields[0].shr_var_uint32_arr[4] = 0x100;
            
            /** special fields configuration */
            BCM_IF_ERROR_RETURN(bcm_flow_initiator_info_create(unit, &flow_handle_info, &info, &special_fields));
            
            printf ("%s():bcm_flow_initiator_info_create bitstr entry pass. initiator_id=0x%x\n", 
                           proc_name,flow_handle_info.flow_id);
            BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(intf_id, flow_handle_info.flow_id);
        }

        /*BIFT entry BFR4-->BFR8*/
        BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "BIER_PREFIX_INITIATOR",&flow_handle));
        bcm_flow_handle_info_t_init(&flow_handle_info);
        bcm_flow_special_fields_t_init(&special_fields);

        flow_handle_info.flow_handle = flow_handle;
        flow_handle_info.flags = 0;
        info.valid_elements_set |=
            (BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID );

        info.l3_intf_id = intf_id;
        info.encap_access = bcmEncapAccessNativeArp;

        /** special fields configuration */
        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "BIER_BIFT_ID", &field_id));
        special_fields.special_fields[0].field_id = field_id;
        special_fields.special_fields[0].shr_var_uint32 = cint_bier_info.bfr1_bift_id;       

        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "ENCAP_IPV6_DOH", &field_id));
        special_fields.special_fields[1].field_id = field_id;
        special_fields.special_fields[1].shr_var_uint32 = 1;

        special_fields.actual_nof_special_fields = 2;

        BCM_IF_ERROR_RETURN(bcm_flow_initiator_info_create(unit, &flow_handle_info, &info, &special_fields));
        
        printf ("%s():bcm_flow_initiator_info_create bier prefix entry pass. initiator_id=0x%x\n", 
                               proc_name,flow_handle_info.flow_id);

        BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(intf_id, flow_handle_info.flow_id);

        cint_bier_info.bfr4_bfr8_bift_tunnel_id = intf_id;

    }


    return rv;

}

/*
  * Create a initiator for BIER header
  * unit - relevant unit
  */
int
bierv6_bier_initiator_create(
    int unit)
{    
    int rv = BCM_E_NONE;
    bcm_flow_initiator_info_t info;
    bcm_flow_handle_t flow_handle;
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_special_fields_t special_fields;
    bcm_flow_field_id_t field_id;
    bcm_if_t   intf_id = 0;
    uint32 bfr_bit_str_in_mcdb = *dnxc_data_get(unit, "bier", "params", "bfr_bit_str_in_mcdb", NULL);


    /*BIFT entry*/
    BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "BIER_PREFIX_INITIATOR",&flow_handle));
    
    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_special_fields_t_init(&special_fields);
   
    BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(intf_id, cint_bierv6_info.bfr1_bfr1_ipv6_tunnel_id); 
    flow_handle_info.flow_handle = flow_handle;
    flow_handle_info.flags = 0;
    info.valid_elements_set |=
        (BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID );
    
    info.l3_intf_id = intf_id;
    info.encap_access = bcmEncapAccessTunnel2;
    
    /** special fields configuration */
    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "BIER_BIFT_ID", &field_id));
    special_fields.special_fields[0].field_id = field_id;
    special_fields.special_fields[0].shr_var_uint32 = cint_bier_info.bfr1_bift_id;       

    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "ENCAP_IPV6_DOH", &field_id));
    special_fields.special_fields[1].field_id = field_id;
    special_fields.special_fields[1].shr_var_uint32 = 1;

    special_fields.actual_nof_special_fields = 2;
    
    BCM_IF_ERROR_RETURN(bcm_flow_initiator_info_create(unit, &flow_handle_info, &info, &special_fields));
    
    printf ("bcm_flow_initiator_info_create bier prefix entry pass. initiator_id=0x%x\n", flow_handle_info.flow_id);
    
    BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(intf_id, flow_handle_info.flow_id);


    BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "BIER_BITSTR_INITIATOR",&flow_handle));
    
    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_special_fields_t_init(&special_fields);

    flow_handle_info.flow_handle = flow_handle;
    flow_handle_info.flags = 0;
    info.valid_elements_set |=
        (BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID );

    info.l3_intf_id = intf_id;
    info.encap_access = bcmEncapAccessNativeArp;

    special_fields.actual_nof_special_fields = 1;

    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "BFR_BIT_STR", &field_id));
    special_fields.special_fields[0].field_id = field_id;
    special_fields.special_fields[0].shr_var_uint32_arr[4] = 0x28;

    /** special fields configuration */
    BCM_IF_ERROR_RETURN(bcm_flow_initiator_info_create(unit, &flow_handle_info, &info, &special_fields));

    printf ("bcm_flow_initiator_info_create bitstr entry pass. initiator_id=%d \n", flow_handle_info.flow_id);


    BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "BIER_BASE_INITIATOR",&flow_handle));

    BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(intf_id, flow_handle_info.flow_id);

    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_special_fields_t_init(&special_fields);

    flow_handle_info.flow_handle = flow_handle;
    flow_handle_info.flags = 0;
    info.valid_elements_set |=
        (BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID );

    info.l3_intf_id = intf_id;
    info.encap_access = bcmEncapAccessRif;

    if(!bfr_bit_str_in_mcdb)
    {
        special_fields.actual_nof_special_fields = 5;
    }
    else
    {
        special_fields.actual_nof_special_fields = 4;
    }
    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "BIER_BFIR_ID", &field_id));
    special_fields.special_fields[0].field_id = field_id;
    special_fields.special_fields[0].shr_var_uint32 = cint_bier_info.bfr1_bift_id;

    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "BIER_BSL", &field_id));
    special_fields.special_fields[1].field_id = field_id;
    special_fields.special_fields[1].shr_var_uint32 = 3;

    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "BIER_RSV", &field_id));
    special_fields.special_fields[2].field_id = field_id;
    special_fields.special_fields[2].shr_var_uint32 = 3;

    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "BIER_OAM", &field_id));
    special_fields.special_fields[3].field_id = field_id;
    special_fields.special_fields[3].shr_var_uint32 = 3;

    if(!bfr_bit_str_in_mcdb)
    {
        BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "BIER_NIBBLE", &field_id));
        special_fields.special_fields[4].field_id = field_id;
        special_fields.special_fields[4].shr_var_uint32 = 5;
    }
    /** special fields configuration */
    BCM_IF_ERROR_RETURN(bcm_flow_initiator_info_create(unit, &flow_handle_info, &info, &special_fields));

    BCM_GPORT_TUNNEL_ID_SET(cint_bier_info.bfr1_bift_initator_id, _SHR_GPORT_FLOW_LIF_VAL_GET_ID(flow_handle_info.flow_id));

    printf ("bcm_flow_initiator_info_create base header entry pass. initiator_id=0x%x\n", cint_bier_info.bfr1_bift_initator_id);

    return rv;

}


/*
 * Create the tunnel terminator.
 *
 */
int bierv6_ipv6_tunnel_terminator_create(
    int unit,
    int node_type)
{

    int rv;
    int mp_tunnel_id;
    bcm_tunnel_terminator_t tunnel_termination_obj;

    char *proc_name;

    proc_name = "bierv6_ipv6_tunnel_terminator_create";

    /* Create IPv6 tunnel terminator for SIP,DIP, VRF lookup */
    bcm_tunnel_terminator_t_init(&tunnel_termination_obj);

    tunnel_termination_obj.type             = bcmTunnelTypeIpAnyIn6;
    tunnel_termination_obj.vrf             = cint_bier_info.vrf;
    if (node_type == BFR)
    {
        tunnel_termination_obj.dip6             = cint_bierv6_info.bfr4_ipv6_ip_addr;
    }
    else
    {
        tunnel_termination_obj.dip6             = cint_bierv6_info.bfr7_ipv6_ip_addr;
    }
    tunnel_termination_obj.dip6_mask        = cint_bierv6_info.ipv6_ip_addr_mask;
    tunnel_termination_obj.priority         = 4;
    
    rv = bcm_tunnel_terminator_create(unit, &tunnel_termination_obj);
    if (rv != BCM_E_NONE)
    {
        printf("%s()Error: bcm_tunnel_terminator_create() %d failed.\n", proc_name);
        return rv;
    }

    printf ("%s() pass. terminator_id=%x \n", proc_name,tunnel_termination_obj.tunnel_id);

    return rv;
}


 /**
  * Main entrance for MPLS BIER
  * INPUT:
  *   node_type    - node type 
  *   port_1       - port_1
  *   port_2       - port_2
  *   port_3       - port_3
  *   port_4       - port_4
  *   payload      - payload
 */
 int
 bierv6_edge_node_example(
    int unit,
    int node_type,
    int port_1,
    int port_2,
    int port_3,
    int port_4,
    int payload)
 {
    int rv;
    char *proc_name;

    proc_name = "bierv6_edge_node_example";
    
    mpls_bier_init_para(unit, node_type, 0, port_1, port_2, port_3, port_4);   
 
    rv = mpls_bier_port_configuration(unit, node_type);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in mpls_bier_port_configuration create\n", proc_name);
        return rv;
    }

    rv = mpls_bier_l3_intf_configuration(unit, node_type);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in mpls_bier_l3_intf_configuration create\n", proc_name);
        return rv;
    }
 
     rv = mpls_bier_arp_configuration(unit, node_type);
     if (rv != BCM_E_NONE)
     {
         printf("%s(): Error, in mpls_bier_arp_configuration create\n", proc_name);
         return rv;
     }

     
     rv = mpls_bier_create_recycle_entry(unit, node_type);
     if (rv != BCM_E_NONE)
     {
         printf("%s(): Error, in mpls_bier_create_recycle_entry create\n", proc_name);
         return rv;
     }


     rv = bierv6_ipv6_tunnel_encap_configure(unit, node_type, payload);
     if (rv != BCM_E_NONE)
     {
         printf("%s(): Error, in bierv6_mpls_tunnel_configure create\n", proc_name);
         return rv;
     }

     rv = bierv6_bift_entry_create(unit, node_type,  payload);
     if (rv != BCM_E_NONE)
     {
         printf("%s(): Error, in bierv6_bift_entry_create create\n", proc_name);
         return rv;
     }

     rv = bierv6_ipv6_tunnel_term_configure(unit,node_type);
     if (rv != BCM_E_NONE)
     {
         printf("%s(): Error, in bierv6_ipv6_tunnel_term_configure\n", proc_name);
         return rv;
     }

     if (node_type == BFIR)
     {
         rv = bierv6_bier_initiator_create(unit);
         if (rv != BCM_E_NONE)
         {
             printf("%s(): Error, in bierv6_initiator_create create bier initator\n", proc_name);
             return rv;
         }
     }
     else
     {
         rv = mpls_bier_2nd_pass_terminator_create(unit, 0);
         if (rv != BCM_E_NONE)
         {
             printf("%s(): Error, in bierv6_initiator_create create 2nd term\n", proc_name);
             return rv;
         }
     }

     rv = mpls_bier_mpls_app_label_configure(unit, node_type, payload);
     if (rv != BCM_E_NONE)
     {
         printf("%s(): Error, in mpls_bier_mpls_app_label_configure create\n", proc_name);
         return rv;
     }

     rv = mpls_bier_forward_entry_configuration(unit, node_type, payload, 0);
     if (rv != BCM_E_NONE)
     {
         printf("%s(): Error, in mpls_bier_forward_entry_configuration create\n", proc_name);
         return rv;
     }
    
     int mc_mode = 0;
     uint32 *ingress_only = dnxc_data_get(unit, "bier", "params", "ingress_only", NULL);
     mc_mode = (*ingress_only == 1)?1:0; 
     rv = mpls_bier_multicast_group_configuration(unit, node_type, payload, mc_mode, 0);
     if (rv != BCM_E_NONE)
     {
         printf("%s(): Error, in mpls_bier_multicast_group_configuration create\n", proc_name);

         return rv;
     }

     return rv;
 }

 int
 bierv6_transit_node_example(
    int unit,
    int port_1,
    int port_2,
    int port_3,
    int port_4,
    int payload,
    int mode)
 {
    int rv;
    char *proc_name;

    proc_name = "bierv6_edge_node_example";

    mpls_bier_init_para(unit, BFR, 0, port_1, port_2, port_3, port_4);

    rv = mpls_bier_port_configuration(unit, BFR);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in mpls_bier_port_configuration create\n", proc_name);
        return rv;
    }

    rv = mpls_bier_l3_intf_configuration(unit, BFR);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in mpls_bier_l3_intf_configuration create\n", proc_name);
        return rv;
    }

     rv = mpls_bier_arp_configuration(unit, BFR);
     if (rv != BCM_E_NONE)
     {
         printf("%s(): Error, in mpls_bier_arp_configuration create\n", proc_name);
         return rv;
     }

     rv = bierv6_ipv6_tunnel_term_configure(unit, BFR);
     if (rv != BCM_E_NONE)
     {
         printf("%s(): Error, in bierv6_ipv6_tunnel_term_configure\n", proc_name);
         return rv;
     }

     rv = bierv6_ipv6_tunnel_encap_configure(unit, BFR, payload);
     if (rv != BCM_E_NONE)
     {
         printf("%s(): Error, in bierv6_mpls_tunnel_configure create\n", proc_name);
         return rv;
     }

     rv = bierv6_bift_entry_create(unit, BFR, payload);
     if (rv != BCM_E_NONE)
     {
         printf("%s(): Error, in bierv6_bift_entry_create create\n", proc_name);
         return rv;
     }


     rv = mpls_bier_forward_entry_configuration(unit, BFR, payload, 0);
     if (rv != BCM_E_NONE)
     {
         printf("%s(): Error, in mpls_bier_forward_entry_configuration create\n", proc_name);
         return rv;
     }

     rv = mpls_bier_multicast_group_configuration(unit, BFR, payload, mode, 0);
     if (rv != BCM_E_NONE)
     {
         printf("%s(): Error, in mpls_bier_multicast_group_configuration create\n", proc_name);
         return rv;
     }

     return rv;
 }

