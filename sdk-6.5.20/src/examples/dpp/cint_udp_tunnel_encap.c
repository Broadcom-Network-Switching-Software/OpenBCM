/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose: Example of UDP tunnel encapsulation
 *
 * Configuration Steps:
 *       Create UDP tunnel
 *       Add IPv4_1, IPv6_1 and MPLS_1 routes to UDP tunnel
 *       Create MPLSoUDP tunnel using previously created UDP tunnel
 *       Add IPv4_2, IPv6_2 and MPLS_2 routes to MPLSoUDP tunnel
 *
 * Test scenario:
 * step 1: UDP Tunnel Encapsulation
 *         Send IPv4_1 packet, Receive IPv4_1oUDPoIPv4
 *         Send IPv6_1 packet, Receive IPv6_1oUDPoIPv4
 *         Send MPLS_1 packet, Receive MPLS_1oUDPoIPv4
 *
 * step 2: MPLS_2oUDP Tunnel Encapsulation
 *         Send IPv4_2 packet, Receive IPv4_2oMPLS_2oUDPoIPv4
 *         Send IPv6_2 packet, Receive IPv6_2oMPLS_2oUDPoIPv4
 *         Send MPLS_2 packet, Receive MPLS_2oMPLS_2oUDPoIPv4
 *
 * step 3: Change UDP tunnel destination ports
           repeat step 1, with the new destination ports
 *
 * Example:
 *
 *     INPUT, port 200    ______________________________     OUTPUT, port 201
 *                       |                    __________|
 *                       |                   |          |
 *        IPv4_1 ------> |------------------>| UDP      | ------> IPv4_1oUDPoIPv4
          IPv6_1 ------> |------------------>| Tunnel   | ------> IPv6_1oUDPoIPv4
 *        MPLS_1 ------> |------------------>| Encap.   | ------> MPLS_1oUDPoIPv4
 *                       |   __________      |          |
 *                       |  |          |     |          |
 *        IPv4_2 ------> |->| MPLS     |---->|          | ------> IPv4_2oMPLS_2oUDPoIPV4
 *        IPv6_2 ------> |->| Tunnel   |---->|          | ------> IPv6_2oMPLS_2oUDPoIPv4
 *        MPLS_3 ------> |->| Encap.   |---->|__________| ------> MPLS_3oMPLS_2oUDPoIPv4
 *                       |  |__________|                |
 *                       |______________________________|
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
 * udp_tunnel_encap__start_run(int unit, udp_tunnel_encap_s *test_data) - test_data = NULL for default params
 */

/*
 * Global Variables Definition and Initialization  START
 */

struct udp_tunnel_s{
   bcm_tunnel_type_t tunnel_type;  /* bcmTunnelTypeUdp for udp tunnel*/
    bcm_mac_t da;       /* ip tunnel da */
    bcm_mac_t sa;       /* ip tunnel my mac */
    int       sip;      /* ip tunnel sip */
    int       dip;      /* ip tunnel dip */
    int       vlan;     /* ip tunnel vlan */
    int       ttl;      /* ip tunnel ttl */
    int       dscp;     /* ip tunnel dscp */
    bcm_tunnel_dscp_select_t dscp_sel; /* ip tunnel dscp_sel */
};


/*
 * struct includes meta information.
 * where the cint logic pull data from this struct.
 * use to control different runs of the cint, without changing the cint code
 */

/*  Main Struct UDP Tunnel Encapsulation */
struct udp_tunnel_encap_s{
    udp_tunnel_s udp_tunnel;              /* udp tunnel parameters */
    int in_vlan;                          /* in vlan */
    int vrf;                              /* vrf */
    bcm_mac_t my_mac;                     /* mymac */
    int in_port;                          /* in port */
    int out_port;                         /* out port */
    int ipv4_dip1;                        /* ipv4 dip for UDP tunnel */
    int ipv4_dip2;                        /* ipv4 dip for MPLSoUDP tunnel */
    int ipv4_mask;                        /* ipv4 mask */
    bcm_ip6_t ipv6_dip1;                  /* ipv6 dip for UDP tunnel */
    bcm_ip6_t ipv6_dip2;                  /* ipv6 dip for MPLSoUDP tunnel */
    bcm_ip6_t ipv6_mask;                  /* ipv6 mask */
    bcm_mpls_label_t in_mpls_lbl_1;       /* MPLS label for UDP tunnel */
    bcm_mpls_label_t in_mpls_lbl_2;       /* MPLS label for MPLSoUDP tunnel */
    bcm_mpls_label_t mpls_tunnel_label;   /* MPLS label of MPLSoUDP tunnel */
    int mpls_tunnel_ttl;                  /* MPLS TTL of MPLSoUDP tunnel */
};

/* Initialization of global struct */
udp_tunnel_encap_s g_udp_tunnel_encap =
{
    {   /* udp tunnel parameters */
        bcmTunnelTypeUdp,                     /* tunnel type */
        {0x20, 0x00, 0x00, 0x00, 0xab, 0xcd}, /* da */
        {0x00, 0x0C, 0x00, 0x02, 0x00, 0x00}, /* sa */
        0xAA000011,                           /* sip: 170.0.0.17 */
        0xAB000011,                           /* dip: 171.0.0.17 */
        100,                                  /* tunnel vlan */
        50,                                   /* ttl */
        11,                                   /* dscp */
        bcmTunnelDscpAssign                   /* dscp_sel */
    },
    2,            /* in vlan */
    0,            /* vrf */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00},  /* my-MAC */
    200,          /* in port */
    201,          /* out port */
    0x7fffff00,   /* ipv4 dip for UDP tunnel */
    0x6fffff00,   /* ipv4 dip for MPLSoUDP tunnel */
    0xfffffff0,   /* ipv4 mask */
    { /* ipv6 dip for UDP tunnel */
       0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8,
       0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 0xec
    },
    { /* ipv6 dip for MPLSoUDP tunnel */
       0xea, 0xeb, 0xec, 0xed, 0xe5, 0xe6, 0xe7, 0xe8,
       0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 0xec
    },
    { /* ipv6 mask */
       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
    },
    500,    /* MPLS label for UDP tunnel */
    700,    /* MPLS label for MPLSoUDP tunnel */
    600,    /* MPLS label of MPLSoUDP tunnel */
    55      /* MPLS TTL of MPLSoUDP tunnel */
};

/*
 * Global Variables Definition and Initialization  END
 */

/* Get test data */
void udp_tunnel_encap__get_struct(udp_tunnel_encap_s *test_data)
{
    sal_memcpy(test_data, &g_udp_tunnel_encap, sizeof(*test_data));
    return;
}

/* Initialize test data struct */
int udp_tunnel_encap_init(int unit, udp_tunnel_encap_s *test_data)
{
    if (test_data != NULL) {
       sal_memcpy(&g_udp_tunnel_encap, test_data, sizeof(g_udp_tunnel_encap));
    }

    if (!soc_property_get(unit, "udp_tunnel_enable", 1)) {
        printf("fail, udp tunnel soc property is disbaled\n");
        return BCM_E_CONFIG;
    }

    return BCM_E_NONE;
}

int udp_tunnel_encap_create_rif_and_gports(int unit)
{
    int rv;

    rv = bcm_vlan_gport_add(unit, g_udp_tunnel_encap.in_vlan, g_udp_tunnel_encap.in_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("Error, bcm_vlan_gport_add\n");
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, g_udp_tunnel_encap.udp_tunnel.vlan, g_udp_tunnel_encap.out_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("Error, bcm_vlan_gport_add\n");
        return rv;
    }

    /* set RIF properties */
    create_l3_intf_s intf;
    intf.vsi = g_udp_tunnel_encap.in_vlan;
    intf.my_global_mac = g_udp_tunnel_encap.my_mac;
    intf.my_lsb_mac = g_udp_tunnel_encap.my_mac;
    intf.mtu_valid = 1;
    intf.mtu = 0;
    intf.mtu_forwarding = 0;
    intf.vrf_valid = 1;
    intf.vrf = g_udp_tunnel_encap.vrf;

    rv = l3__intf_rif__create(unit, &intf);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
        return rv;
    }

    return rv;
}

/* Main function, tcl interface */
int udp_tunnel_encap__start_run_with_ports(int unit, int in_port, int out_port)
{
    int rv;
    udp_tunnel_encap_s temp_struct;

    sal_memcpy(&temp_struct, &g_udp_tunnel_encap, sizeof(temp_struct));

    temp_struct.in_port = in_port;
    temp_struct.out_port = out_port;

    rv = udp_tunnel_encap__start_run(unit, &temp_struct);
    if (rv != BCM_E_NONE) {
        printf("Error, udp_tunnel_encap__start_run\n");
        return rv;
    }

    return rv;
}

/* Main function */
int udp_tunnel_encap__start_run(int unit, udp_tunnel_encap_s *test_data)
{
    int rv;
    int udp_intf_id, mpls_udp_intf_id;

    rv = udp_tunnel_encap_init(unit, test_data);
    if (rv != BCM_E_NONE) {
        printf("Error, udp_tunnel_encap_init\n");
        return rv;
    }

    rv = udp_tunnel_encap_create_rif_and_gports(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, udp_tunnel_encap_create_rif_and_gports\n");
        return rv;
    }

    /* build UDP tunnel and get back interface_id */
    rv = udp_tunnel_encap_add_udp_tunnel(unit, &udp_intf_id);
    if (rv != BCM_E_NONE) {
        printf("Error, udp_tunnel_encap_add_udp_tunnel\n");
        return rv;
    }

    /* add route host points to the UDP tunnel */
    rv = udp_tunnel_encap_add_udp_tunnel_routes(unit, udp_intf_id);
    if (rv != BCM_E_NONE) {
        printf("Error, udp_tunnel_encap_add_udp_tunnel_routes\n");
        return rv;
    }

    /* build MPLS tunnel over previously created UDP tunnel */
    rv = udp_tunnel_encap_add_mpls_o_udp_tunnel(unit, udp_intf_id, &mpls_udp_intf_id);
    if (rv != BCM_E_NONE) {
        printf("Error, udp_tunnel_encap_add_mpls_o_udp_tunnel\n");
        return rv;
    }

    /* add route host points to the MPLSoUDP tunnel */
    rv = udp_tunnel_encap_add_mpls_o_udp_tunnel_routes(unit, mpls_udp_intf_id);
    if (rv != BCM_E_NONE) {
        printf("Error, udp_tunnel_encap_add_mpls_o_udp_tunnel_routes\n");
        return rv;
    }

    return rv;
}

/* create udp tunnel initiator and return its interface id */
int udp_tunnel_encap_add_udp_tunnel(int unit, bcm_if_t* udp_intf_id)
{
    int rv;

    bcm_l3_intf_t l3_intf;
    bcm_l3_intf_t_init(&l3_intf);

    bcm_tunnel_initiator_t udp_tunnel;
    udp_tunnel_s tunnel_info = g_udp_tunnel_encap.udp_tunnel;
    bcm_mac_t next_hop_mac  = g_udp_tunnel_encap.udp_tunnel.da;

    bcm_tunnel_initiator_t_init(&udp_tunnel);
    udp_tunnel.dip = tunnel_info.dip;
    udp_tunnel.sip = tunnel_info.sip;
    udp_tunnel.dscp = tunnel_info.dscp;
    udp_tunnel.flags = 0;
    udp_tunnel.ttl = tunnel_info.ttl;
    udp_tunnel.type = tunnel_info.tunnel_type;
    udp_tunnel.vlan = tunnel_info.vlan;
    udp_tunnel.dscp_sel = tunnel_info.dscp_sel;

    rv = bcm_tunnel_initiator_create(unit, &l3_intf, &udp_tunnel);
    if(rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_initiator_create\n");
        return rv;
    }
    *udp_intf_id = l3_intf.l3a_intf_id;

    /* Set ARP entry parameters */
    create_l3_egress_s l3eg;
    sal_memcpy(l3eg.next_hop_mac_addr, next_hop_mac, 6);
    l3eg.out_tunnel_or_rif = *udp_intf_id;
    l3eg.out_gport = g_udp_tunnel_encap.out_port;
    l3eg.vlan = g_udp_tunnel_encap.udp_tunnel.vlan;

    /* Set ARP entry, without allocating FEC entry */
    rv = l3__egress_only_encap__create(unit, &l3eg);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
        return rv;
    }

    return rv;
}

/* add IPv4 route */
int udp_tunnel_encap_add_ipv4_route(int unit, int intf_id, int is_mpls_o_udp)
{
    int rv;

    bcm_l3_route_t l3rt;
    bcm_l3_route_t_init(l3rt);
    l3rt.l3a_flags = BCM_L3_RPF;
    if (is_mpls_o_udp) {
       l3rt.l3a_subnet = g_udp_tunnel_encap.ipv4_dip2;
    } else {
        l3rt.l3a_subnet = g_udp_tunnel_encap.ipv4_dip1;
    }
    l3rt.l3a_ip_mask = g_udp_tunnel_encap.ipv4_mask;
    l3rt.l3a_vrf = g_udp_tunnel_encap.vrf;
    l3rt.l3a_intf = intf_id;
    l3rt.l3a_modid = 0;
    l3rt.l3a_port_tgid = 0;

    rv = bcm_l3_route_add(unit, l3rt);
    if (rv != BCM_E_NONE) {
        printf ("Error, bcm_l3_route_add\n");
        return rv;
    }

    return rv;
}

/* add IPv6 route */
int udp_tunnel_encap_add_ipv6_route(int unit, int intf_id, int is_mpls_o_udp)
{
    int rv;

    bcm_l3_route_t l3rt;
    bcm_l3_route_t_init(l3rt);
    l3rt.l3a_flags = BCM_L3_IP6;
    if (is_mpls_o_udp) {
       l3rt.l3a_ip6_net = g_udp_tunnel_encap.ipv6_dip2;
    } else {
        l3rt.l3a_ip6_net = g_udp_tunnel_encap.ipv6_dip1;
    }
    l3rt.l3a_ip6_mask = g_udp_tunnel_encap.ipv6_mask;
    l3rt.l3a_vrf = g_udp_tunnel_encap.vrf;
    l3rt.l3a_intf = intf_id;
    l3rt.l3a_modid = 0;
    l3rt.l3a_port_tgid = 0;

    rv = bcm_l3_route_add(unit, l3rt);
    if (rv != BCM_E_NONE) {
        printf ("Error, bcm_l3_route_add\n");
        return rv;
    }

    return rv;
}

/* add MPLS route */
int udp_tunnel_encap_add_mpls_route(int unit, int intf_id, int is_mpls_o_udp)
{
    int rv = 0;

    bcm_mpls_egress_label_t label;
    label.flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    if (is_mpls_o_udp) {
       label.label = g_udp_tunnel_encap.in_mpls_lbl_2;
    } else {
       label.label = g_udp_tunnel_encap.in_mpls_lbl_1;
    }
    label.action = BCM_MPLS_EGRESS_ACTION_SWAP;
    rv = bcm_mpls_tunnel_initiator_create(unit, 0, 1, &label);
    if (rv != BCM_E_NONE) {
        printf ("Error, bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    bcm_mpls_tunnel_switch_t tun_switch;
    tun_switch.egress_if = intf_id;
    tun_switch.port = g_udp_tunnel_encap.in_port;
    if (is_mpls_o_udp) {
       tun_switch.label = g_udp_tunnel_encap.in_mpls_lbl_2;
    } else {
       tun_switch.label = g_udp_tunnel_encap.in_mpls_lbl_1;
    }
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

/* create MPLSoUDP tunnel using preallocated UDP tunnel */
int udp_tunnel_encap_add_mpls_o_udp_tunnel(int unit, bcm_if_t udp_intf_ids, bcm_if_t* mpls_udp_intf_ids)
{
    int rv;

    bcm_mpls_egress_label_t label;
    label.ttl = g_udp_tunnel_encap.mpls_tunnel_ttl;
    label.flags = BCM_MPLS_EGRESS_LABEL_TTL_SET;
    label.label = g_udp_tunnel_encap.mpls_tunnel_label;
    label.l3_intf_id =  udp_intf_ids;
    label.action = BCM_MPLS_EGRESS_ACTION_PUSH;
    rv = bcm_mpls_tunnel_initiator_create(unit, 0, 1, &label);
    if (rv != BCM_E_NONE) {
        printf ("Error, bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    *mpls_udp_intf_ids = label.tunnel_id;

    return rv;
}

/* create MPLSoUDP FEC tunnel and add IPv4, IPv6  and MPLS routes */
int udp_tunnel_encap_add_mpls_o_udp_tunnel_routes(int unit, bcm_if_t mpls_udp_intf_id)
{
    int rv;
    bcm_if_t udp_tunnel_fec_id;

    create_l3_egress_s l3eg;
    l3eg.out_tunnel_or_rif = mpls_udp_intf_id;
    l3eg.out_gport = g_udp_tunnel_encap.out_port;
    l3eg.vlan = g_udp_tunnel_encap.udp_tunnel.vlan;

    /* Allocate FEC for MPLSoUDP tunnel */
    rv = l3__egress_only_fec__create(unit, &l3eg);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress_only_fec__create\n");
        return rv;
    }
    udp_tunnel_fec_id = l3eg.fec_id;

    rv = udp_tunnel_encap_add_ipv4_route(unit, udp_tunnel_fec_id, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, udp_tunnel_encap_add_ipv4_route\n");
        return rv;
    }

    rv = udp_tunnel_encap_add_ipv6_route(unit, udp_tunnel_fec_id, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, udp_tunnel_encap_add_ipv6_route\n");
        return rv;
    }

    rv = udp_tunnel_encap_add_mpls_route(unit, udp_tunnel_fec_id, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, udp_tunnel_encap_add_mpls_route\n");
        return rv;
    }

    return rv;
}

/* create UDP tunnel FEC and add routes */
int udp_tunnel_encap_add_udp_tunnel_routes(int unit, int udp_intf_id)
{
    int rv;
    bcm_if_t udp_tunnel_fec_id;

    create_l3_egress_s l3eg;
    l3eg.out_tunnel_or_rif = udp_intf_id;
    l3eg.vlan = g_udp_tunnel_encap.udp_tunnel.vlan;
    l3eg.out_gport = g_udp_tunnel_encap.out_port;

    /* Allocate FEC for UDP Tunnel*/
    rv = l3__egress_only_fec__create(unit, &l3eg);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
        return rv;
    }
    udp_tunnel_fec_id = l3eg.fec_id;

    rv = udp_tunnel_encap_add_ipv4_route(unit, udp_tunnel_fec_id, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, udp_tunnel_encap_add_ipv4_route\n");
        return rv;
    }

    rv = udp_tunnel_encap_add_ipv6_route(unit, udp_tunnel_fec_id, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, udp_tunnel_encap_add_ipv6_route\n");
        return rv;
    }

    rv = udp_tunnel_encap_add_mpls_route(unit, udp_tunnel_fec_id, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, udp_tunnel_encap_add_mpls_route\n");
        return rv;
    }

    return rv;
}

/* switch_control API to change udp tunnel destination port */
int udp_tunnel_encap_change_udp_dst_port(int unit, bcm_switch_control_t dst_port_type, int dst_port)
{
   int rv;

   if (dst_port_type == bcmSwitchUdpTunnelIPv4DstPort) {
       rv = bcm_switch_control_set(unit, bcmSwitchUdpTunnelIPv4DstPort, dst_port);
   } else if (dst_port_type == bcmSwitchUdpTunnelIPv6DstPort) {
       rv = bcm_switch_control_set(unit, bcmSwitchUdpTunnelIPv6DstPort, dst_port);
   } else if(dst_port_type == bcmSwitchUdpTunnelMplsDstPort) {
       rv = bcm_switch_control_set(unit, bcmSwitchUdpTunnelMplsDstPort, dst_port);
   } else {
       rv = BCM_E_PARAM;
   }

   return rv;
}

