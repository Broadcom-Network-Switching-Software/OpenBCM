/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * Test Scenario: PUSH MPLS + GRE4 tunnel 
 *
  * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/dpp/utility/cint_utils_vlan.c
 * cint ../../src/examples/dpp/utility/cint_utils_l2.c
 * cint ../../src/examples/dpp/utility/cint_utils_l3.c
 * cint ../../src/examples/dpp/utility/cint_utils_mpls_port.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_mpls.c
 * cint ../../src/examples/dpp/cint_ip_route.c
 * cint ../../src/examples/dpp/cint_ip_tunnel.c
 * cint ../../src/examples/dpp/cint_push_mpls_gre.c
 * cint
 * push_mpls_gre_example(0,200,201);
 * exit;
 * 
 * Routing to Tunnels: MPLS+IP-GRE4 
 * Purpose: check encapsulation MPLS+IP-GRE4 tunnel
 * Note: in Jericho A0, IP length is incorrect (doesn't include MPLS length)
 * Packet flow:
 * - get VSI from VLAN
 * - get incoming router interface RIF (=VSI)
 * - check if myMac is enabled for this RIF.
 * - Eth.DA == myMac so terminate Link Layer
 * - packet is forwarded according to DIP, VRF
 * - result is FEC: MPLS + GRE4 tunnel outlif + destination port
 * - encapsulate MPLS header, GRE4 header, IP header and Ethernet header 
 * 
 * tx 1 psrc=200 data=0x000c00020000000007000100810000140800450000140000000040002d9700000011aba1a1a1
 * Received packets on unit 0 should be: 
 * 0x00000000cd1d000c00020000810000640800450a0048000000003c2f3d5ba0000011a100001100008847000c811445000014000000003f002e9700000011aba1a1a100000000000000000000000000000000000000000000000000000000 
 */



/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 */
/* DSCP preserve feature */
int preserve_dscp_lif = 0;
 
struct push_mpls_gre_s {
    /* router instance */
    int vrf; 
    /* mpls label */
    int mpls_label; 
    /* native vlan */
    int native_vlan; 
    /* native DIP */
    int native_dip; 
    /* native DIP mask */
    int native_dip_mask; 
};

push_mpls_gre_s g_push_mpls_gre = 
{
    /* router instance */
    10,
    /* mpls label */
    200,
    /* access native vlan */
    20,
    /* native dip:     171.161.161.161 */
    0xABA1A1A1, 
    /* native dip mask 255.255.255.240 */
    0xfffffff0
}; 


int push_mpls_gre_example(int unit, int access_port, int provider_port)
{
    int rv = BCM_E_NONE; 
    bcm_ip6_t route_ipv6 = ip_tunnel_glbl_info.route_ipv6;
    bcm_ip6_t mask_ipv6 = ip_tunnel_glbl_info.mask_ipv6;
    bcm_gport_t out_tunnel_gports[2];
    int ll_encap_id;
    bcm_if_t out_tunnel_intf_ids[2];/* out tunnels interfaces
                                  out_tunnel_intf_ids[0] : is tunnel-interface-id
                                  out_tunnel_intf_ids[1] : is egress-object (FEC) points to tunnel
                              */

    /* global configuration */

    /* init ip tunnel global */
    ip_tunnel_s tunnel_1;   /* tunnel 1 info from cint_ip_tunnel.c */
    ip_tunnel_s tunnel_2;   /* tunnel 2 info from cint_ip_tunnel.c */
    ip_tunnel_info_get(&tunnel_1, &tunnel_2);

    if (!is_device_or_above(unit, JERICHO2)){
        /* configure global GRE.protocol (set GRE.protocol field when next header is neither IPvX or Eth) */
        int gre_protocol_type = 0x8847;
        rv = bcm_switch_control_set(unit,bcmSwitchL2GreProtocolType,gre_protocol_type);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_switch_control_set\n");
            return rv;
        }
    }

    /* configure inlif to enable preserve DSCP */
    if (preserve_dscp_lif & 0x1) {
        bcm_vlan_port_t vlan_port;
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vlan_port.match_vlan = g_push_mpls_gre.native_vlan;
        vlan_port.egress_vlan = g_push_mpls_gre.native_vlan;
        vlan_port.vsi = g_push_mpls_gre.native_vlan;
        rv = bcm_vlan_port_create(unit, &vlan_port);
        if (rv != BCM_E_NONE) {
            printf("fail create VLAN port, vlan(%d)\n", in_vlan);
            return rv;
        }

        rv = bcm_port_control_set(0, vlan_port.vlan_port_id, bcmPortControlPreserveDscpIngress, 1);
        if (rv != BCM_E_NONE) {
            printf("bcm_port_control_set bcmPortControlPreserveDscpIngress failed, port(0x08x)\n", vlan_port.vlan_port_id);
            return rv;
        }
    }

    /* configure my mac (global mymac MSBs and per rif mymac lsbs)
     * configure routing interface
       inRif is used for access to provider case, inRif = vsi. */
    create_l3_intf_s l3_intf;
    sal_memset(&l3_intf, 0, sizeof(l3_intf));
    l3_intf.vrf = g_push_mpls_gre.vrf; /* router instance */
    l3_intf.vrf_valid = 1; 
    l3_intf.vsi = g_push_mpls_gre.native_vlan; /* routing interface */
    l3_intf.my_global_mac = tunnel_1.sa;  
    l3_intf.my_lsb_mac = tunnel_1.sa;  

    rv = l3__intf_rif__create(unit, &l3_intf);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, g_push_mpls_gre.native_vlan, access_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", access_port, g_push_mpls_gre.native_vlan);
      return rv;
    }

    /* build tunnel */

    /* build IP tunnel with GRE4 and link layer */
    rv = ipv4_tunnel_build_tunnels(unit, provider_port, out_tunnel_intf_ids, out_tunnel_gports, &ll_encap_id); 
    if (rv != BCM_E_NONE) {
        printf("Error, ipv4_tunnel_build_tunnels\n");
        return rv;
    }

    /* build tunnel: build MPLS tunnel, link MPLS to IP tunnel */   
    mpls__egress_tunnel_utils_s mpls_tunnel_properties; 
    mpls_tunnel_properties.label_in = g_push_mpls_gre.mpls_label; 
    mpls_tunnel_properties.next_pointer_intf = out_tunnel_intf_ids[0];  /* mpls point to gre ip tunnel */
    rv = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties); 
    if (rv != BCM_E_NONE) {
        printf("Error, mpls__create_tunnel_initiator__set\n");
        return rv;
    }

    if (preserve_dscp_lif & 0x2) {
        bcm_gport_t gport;
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, mpls_tunnel_properties.tunnel_id);
        rv = bcm_port_control_set(unit, gport, bcmPortControlPreserveDscpEgress, 1);
        if (rv != BCM_E_NONE) {
            printf("bcm_port_control_set bcmPortControlPreserveDscpEgress failed, port(0x08x)\n", gport);
            return rv;
        }
     }

    /* create FEC which point to MPLS IP-GRE tunnel */
    bcm_if_t fec_if_id; 
    create_l3_egress_s l3_egress_fec; 
    l3_egress_fec.out_gport = provider_port; 
    l3_egress_fec.arp_encap_id = mpls_tunnel_properties.tunnel_id; /* tunnel */
    l3_egress_fec.fec_id = fec_if_id;
    
    rv = l3__egress_only_fec__create(unit,&l3_egress_fec); 
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress_only_fec__create\n");
        return rv;
    }
    fec_if_id = l3_egress_fec.fec_id;

    /* create route entry */
    l3_ipv4_route_entry_utils_s route_entry;
    sal_memset(&route_entry, 0, sizeof(route_entry));
    route_entry.address =  g_push_mpls_gre.native_dip;
    route_entry.mask = g_push_mpls_gre.native_dip_mask;
    rv = l3__ipv4_route__add(unit, 
                             route_entry, /* key for routing table entry: dip, dip mask  */
                             g_push_mpls_gre.vrf, /* key for routing table entry: router instance */
                             fec_if_id            /* payload for LPM entry: fec */
                             ); 
    if (rv != BCM_E_NONE) {
        printf("Error, l3__ipv4_route__add\n");
        return rv;
    }

    /* Create IPv6 route entry */
    rv = add_route_ip6(unit, route_ipv6, mask_ipv6, g_push_mpls_gre.vrf, fec_if_id);
    if (rv != BCM_E_NONE) {
        printf("Error, create IPv6 route\n");
        return rv;
    }

    return rv; 
}
