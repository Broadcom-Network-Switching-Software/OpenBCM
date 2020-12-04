/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose: Example of UDP tunnel termination
 *
 * Test Configuration:
 *    Create UDP tunnel terminators with default udp destination ports
 *    for IPv4, IPv6 and MPLS over udp.
 *    Create Ipv4, IPv6 and MPLS forwarding routes
 *    Create Ipv4 forwarding route with UDP tunnel's DIP
 *
 * Test scenario:
 * Stage 1:
 *    Send IPv4oUDP tunnel packet, Receive IPv4 packet
 *    Send IPv6oUDP tunnel packet, Receive IPv6 packet
 *    Send MPLSoUDP tunnel packet, Receive MPLS packet
 *    Send IPv4oUDPoIPv4 packet, with udp dest port different from
 *    IPv4, IPv6 and MPLS detination ports, Receive IPv4oUDPoIPv4 packet
 *
 * Stage 2:
 *    Change UDP tunnel destination ports
 *    repeat step 1, with the new destination ports
 *
 * Example 1:
 *
 *          INPUT, port 200      __________     OUTPUT, port 201
 *                              |          |
 *        IPv4oUDPoIPv4_udp---> | UDP      | ------> IPv4
 *        IPv6oUDPoIPv4_udp---> | Tunnel   | ------> IPv6
 *        MPLSoUDPoIPv4_udp---> | Term.    | ------> MPLS
 *                              |__________|
 *
 * Example 2:
 *
 *  in this example udp destination port is different from udp tunnel's possible
 *  destination ports (ipv4, ipv6 or mpls)
 *
 *          INPUT, port 200      __________     OUTPUT, port 201
 *                              |          |
 *                              | UDP      |
 *        IPv4oUDPoIPv4_udp---> | Tunnel   | ------> IPv4oUDPoIPv4_udp
 *                              | Term.    |
 *                              |__________|
 *
 *
 * SOC properties:
 * udp_tunnel_enable=1
 * bcm886xx_ip4_tunnel_termination_mode=6
 *
 * CINT files:
 * cd ../../../../src/examples/dpp/
 * utility/cint_utils_l3.c
 * utility/cint_utils_vlan.c
 * cint_ip_tunnel.c
 * cint_ip_route.c
 *
 * udp_tunnel_term__start_run(int unit, udp_tunnel_term_s *test_data) - test_data = NULL for default params
 */

/*
 * Global Variables Definition and Initialization  START
 */

/*
 * struct includes meta information.
 * where the cint logic pull data from this struct.
 * use to control differ runs of the cint, without changing the cint code
 */

/*  Main Struct Udp Tunnel Termination */
struct udp_tunnel_term_s{
    int udp_tunnel_dip;    /* UDP tunnel terminator dip */
    int udp_tunnel_sip;    /* UDP tunnel terminator sip */
    int udp_tunnel_mask;   /* UDP tunnel terminator mask */
    int udp_tunnel_ipv4_dst_port_1;    /* UDP tunnel ipv4 destination port */
    int udp_tunnel_ipv6_dst_port_1;    /* udp tunnel ipv6 destination port */
    int udp_tunnel_mpls_dst_port_1;    /* udp tunnel mpls destination port */
    int udp_tunnel_ipv4_dst_port_2;    /* UDP tunnel ipv4 destination port after dynamic update */
    int udp_tunnel_ipv6_dst_port_2;    /* udp tunnel ipv6 destination port after dynamic update */
    int udp_tunnel_mpls_dst_port_2;    /* udp tunnel mpls destination port after dynamic update */
    int ipv4_dip;          /* ipv4 forwarding dip */
    int ipv4_sip;          /* ipv4 forwarding sip */
    int ipv4_mask;         /* ipv4 forwarding mask */
    bcm_ip6_t ipv6_dip;    /* ipv6 forwarding dip */
    bcm_ip6_t ipv6_sip;    /* ipv6 forwarding sip */
    bcm_ip6_t ipv6_mask;   /* ipv6 forwarding mask */
    int mpls_label;        /* MPLS forwarding label */
    bcm_mac_t my_mac;      /* mymac */
    int in_vlan;           /* in vlan */
    int out_vlan;          /* out vlan */
    int vrf;               /* vrf */
    int in_port;           /* in port */
    int out_port;          /* out port */
    bcm_mac_t sa;          /* sa*/
    bcm_mac_t da;          /* da */
};

/* Initialization of global struct*/
udp_tunnel_term_s g_udp_tunnel_term =
{
    0xA1000011,   /* udp terminator dip */
    0xA0000011,   /* udp terminator sip */
    0xffffffff,   /* udp terminator mask */
    0x6605,       /* udp tunnel ipv4 destination port */
    0x6615,       /* udp tunnel ipv6 destination port */
    0x6635,       /* udp tunnel mpls destination port */
    0x1234,       /* udp tunnel ipv4 destination port after dynamic update */
    0x1245,       /* udp tunnel ipv6 destination port after dynamic update */
    0x1256,       /* udp tunnel mpls destination port after dynamic update */
    0x01000011,   /* ipv4 forwarding dip */
    0x00000011,   /* ipv4 forwarding sip */
    0xfffffff0,   /* ipv4 forwarding mask */
    { /* ipv6 forwarding dip */
       0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8,
       0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 0xec
    },
    { /* ipv6 forwarding sip */
       0x3f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
       0xfa, 0x4f, 0x57, 0xff, 0xfe, 0x72, 0x00, 0x00
    },
    { /* ipv6 forwarding mask */
       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
    },
    500,          /* mpls label */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00},  /* my-MAC */
    2,            /* in vlan */
    100,          /* out vlan */
    0,            /* vrf */
    200,          /* in port */
    201,          /* out port */
    {0x00, 0x0C, 0x00, 0x02, 0x00, 0x00}, /* sa */
    {0x00, 0x00, 0x00, 0x00, 0x12, 0x34}  /* da */
};

/*
 * Global Variables Definition and Initialization  END
 */

/* Get test data */
void udp_tunnel_encap__get_struct(udp_tunnel_term_s *test_data)
{
    sal_memcpy(test_data, &g_udp_tunnel_term, sizeof(*test_data));
    return;
}

/* Initialize test data struct */
int udp_tunnel_term_init(int unit, udp_tunnel_term_s *test_data)
{
    if (test_data != NULL) {
       sal_memcpy(&g_udp_tunnel_term, test_data, sizeof(g_udp_tunnel_term));
    }

    if (!soc_property_get(unit, "udp_tunnel_enable", 1)) {
        printf("Error, udp tunnel soc property is disbaled\n");
        return BCM_E_CONFIG;
    }

    return BCM_E_NONE;
}

int udp_tunnel_term_create_rif_and_gports(int unit)
{
    int rv;

    rv = bcm_vlan_gport_add(unit, g_udp_tunnel_term.in_vlan, g_udp_tunnel_term.in_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("Error, bcm_vlan_gport_add\n");
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, g_udp_tunnel_term.out_vlan, g_udp_tunnel_term.out_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("Error, bcm_vlan_gport_add\n");
        return rv;
    }

    create_l3_intf_s intf;
    intf.vsi = g_udp_tunnel_term.in_vlan;
    intf.my_global_mac = g_udp_tunnel_term.my_mac;
    intf.my_lsb_mac = g_udp_tunnel_term.my_mac;
    intf.mtu_valid = 1;
    intf.mtu = 0;
    intf.mtu_forwarding = 0;
    intf.vrf_valid = 1;
    intf.vrf = g_udp_tunnel_term.vrf;

    rv = l3__intf_rif__create(unit, &intf);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
        return rv;
    }

    return rv;
}

/* Main function, tcl interface */
int udp_tunnel_term__start_run_with_ports(int unit, int in_port, int out_port)
{
    int rv;
    udp_tunnel_term_s temp_struct;

    sal_memcpy(&temp_struct, &g_udp_tunnel_term, sizeof(temp_struct));

    temp_struct.in_port = in_port;
    temp_struct.out_port = out_port;

    rv = udp_tunnel_term__start_run(unit, &temp_struct);
    if (rv != BCM_E_NONE) {
        printf("Error, udp_tunnel_encap__start_run\n");
        return rv;
    }

    return rv;
}

/* Main function */
int udp_tunnel_term__start_run(int unit, udp_tunnel_term_s *test_data)
{
    int rv;

    rv = udp_tunnel_term_init(unit, test_data);
    if (rv != BCM_E_NONE) {
        printf("Error, udp_tunnel_term_init\n");
        return rv;
    }

    rv = udp_tunnel_term_create_rif_and_gports(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, udp_tunnel_term_create_rif_and_gports\n");
        return rv;
    }

    /* create tunnel terminators */
    rv = udp_tunnel_term_init_tunnel_terminators(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, udp_tunnel_term_init_tunnel_terminators\n");
        return rv;
    }

    /* create forwarding routes */
    rv = udp_tunnel_term_add_routes(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, udp_tunnel_term_add_routes\n");
        return rv;
    }

    return rv;
}

/* create tunnel terminators */
int udp_tunnel_term_init_tunnel_terminators(int unit)
{
    int rv;

    /* tunnel terminator with default ipv4 udp destination port */
    rv = udp_tunnel_term_init_tunnel_terminator(unit, g_udp_tunnel_term.udp_tunnel_ipv4_dst_port_1);
    if (rv != BCM_E_NONE) {
        printf("Error, udp_tunnel_term_init_tunnel_terminator\n");
        return rv;
    }

    /* tunnel terminator with user defined ipv4 udp destination port */
    rv = udp_tunnel_term_init_tunnel_terminator(unit, g_udp_tunnel_term.udp_tunnel_ipv4_dst_port_2);
    if (rv != BCM_E_NONE) {
        printf("Error, udp_tunnel_term_init_tunnel_terminator\n");
        return rv;
    }

    /* tunnel terminator with default ipv6 udp destination port */
    rv = udp_tunnel_term_init_tunnel_terminator(unit, g_udp_tunnel_term.udp_tunnel_ipv6_dst_port_1);
    if (rv != BCM_E_NONE) {
        printf("Error, udp_tunnel_term_init_tunnel_terminator\n");
        return rv;
    }

    /* tunnel terminator with user defined ipv6 udp destination port*/
    rv = udp_tunnel_term_init_tunnel_terminator(unit, g_udp_tunnel_term.udp_tunnel_ipv6_dst_port_2);
    if (rv != BCM_E_NONE) {
        printf("Error, udp_tunnel_term_init_tunnel_terminator\n");
        return rv;
    }

    /* tunnel terminator with default mpls udp destination port*/
    rv = udp_tunnel_term_init_tunnel_terminator(unit, g_udp_tunnel_term.udp_tunnel_mpls_dst_port_1);
    if (rv != BCM_E_NONE) {
        printf("Error, udp_tunnel_term_init_tunnel_terminator\n");
        return rv;
    }

    /* tunnel terminator with user defined mpls udp destination port*/
    rv = udp_tunnel_term_init_tunnel_terminator(unit, g_udp_tunnel_term.udp_tunnel_mpls_dst_port_2);
    if (rv != BCM_E_NONE) {
        printf("Error, udp_tunnel_term_init_tunnel_terminator\n");
        return rv;
    }

    return rv;
}

/* create tunnel terminator */
int udp_tunnel_term_init_tunnel_terminator(int unit, int udp_dst_port)
{
    int rv;

    bcm_tunnel_terminator_t tunnel_term;

    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.dip = g_udp_tunnel_term.udp_tunnel_dip;
    tunnel_term.dip_mask = g_udp_tunnel_term.udp_tunnel_mask;
    tunnel_term.sip = g_udp_tunnel_term.udp_tunnel_sip;
    tunnel_term.sip_mask = g_udp_tunnel_term.udp_tunnel_mask;
    tunnel_term.vrf = g_udp_tunnel_term.vrf;
    tunnel_term.udp_dst_port = udp_dst_port;
    tunnel_term.type = bcmTunnelTypeUdp;

    rv = bcm_tunnel_terminator_create(unit, &tunnel_term);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_terminator_create\n");
        return rv;
    }

   return rv;
}

/* add forwarding routes after tunnel termination */
int udp_tunnel_term_add_routes(int unit)
{
    int rv = BCM_E_NONE;

    /* create FEC and LL for routing */
    create_l3_egress_s l3_egress;
    sal_memset(&l3_egress, 0, sizeof (l3_egress));
    l3_egress.vlan = g_udp_tunnel_term.out_vlan;
    l3_egress.next_hop_mac_addr = g_udp_tunnel_term.da;
    l3_egress.out_gport = g_udp_tunnel_term.out_port;
    rv = l3__egress__create(unit, &l3_egress);

    /* add ipv4 route after tunnel termination */

    bcm_l3_route_t l3rt;
    bcm_l3_route_t_init(l3rt);

    l3rt.l3a_flags = BCM_L3_RPF;
    l3rt.l3a_subnet = g_udp_tunnel_term.ipv4_dip;
    l3rt.l3a_ip_mask = g_udp_tunnel_term.ipv4_mask;
    l3rt.l3a_vrf = g_udp_tunnel_term.vrf;
    l3rt.l3a_intf = l3_egress.fec_id;
    l3rt.l3a_modid = 0;
    l3rt.l3a_port_tgid = 0;
    rv = bcm_l3_route_add(unit, l3rt);
    if (rv != BCM_E_NONE) {
        printf ("Error, bcm_l3_route_add\n");
        return rv;
    }

    /* add ipv4 route - no termination
     * same DIP as UDP tunnels, hit this entry
     * with udp destination port not equal to
     * udp tunnel's udp destination ports */
    bcm_l3_route_t_init(l3rt);
    l3rt.l3a_flags = BCM_L3_RPF;
    l3rt.l3a_subnet = g_udp_tunnel_term.udp_tunnel_dip;
    l3rt.l3a_ip_mask = g_udp_tunnel_term.ipv4_mask;
    l3rt.l3a_vrf = g_udp_tunnel_term.vrf;
    l3rt.l3a_intf = l3_egress.fec_id;
    l3rt.l3a_modid = 0;
    l3rt.l3a_port_tgid = 0;
    rv = bcm_l3_route_add(unit, l3rt);
    if (rv != BCM_E_NONE) {
        printf ("Error, bcm_l3_route_add\n");
        return rv;
    }

    /* create IPv6 route after termination */
    bcm_l3_route_t_init(l3rt);
    l3rt.l3a_flags = BCM_L3_IP6;
    l3rt.l3a_ip6_net = g_udp_tunnel_term.ipv6_dip;
    l3rt.l3a_ip6_mask = g_udp_tunnel_term.ipv6_mask;
    l3rt.l3a_vrf = g_udp_tunnel_term.vrf;
    l3rt.l3a_intf = l3_egress.fec_id;
    l3rt.l3a_modid = 0;
    l3rt.l3a_port_tgid = 0;

    rv = bcm_l3_route_add(unit, l3rt);
    if (rv != BCM_E_NONE) {
          printf("Error, in l3__ipv4_route__add\n");
          return rv;
    }

    /* create MPLS egress label */
    bcm_mpls_egress_label_t label;
    label.flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    label.label = g_udp_tunnel_term.mpls_label;
    label.action = BCM_MPLS_EGRESS_ACTION_SWAP;
    rv = bcm_mpls_tunnel_initiator_create(unit, 0, 1, &label);
    if (rv != BCM_E_NONE) {
        printf ("Error, bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    /* create MPLS switch */
    bcm_mpls_tunnel_switch_t tun_switch;
    tun_switch.egress_if = l3_egress.fec_id;
    tun_switch.port = g_udp_tunnel_term.in_port;
    tun_switch.label = g_udp_tunnel_term.mpls_label;
    tun_switch.egress_label = label;
    tun_switch.action = BCM_MPLS_SWITCH_ACTION_SWAP;
    tun_switch.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;
    rv = bcm_mpls_tunnel_switch_create(unit, tun_switch);
    if (rv != BCM_E_NONE) {
        printf ("Error, bcm_mpls_tunnel_switch_create\n");
        return rv;
    }

    return rv;
}
