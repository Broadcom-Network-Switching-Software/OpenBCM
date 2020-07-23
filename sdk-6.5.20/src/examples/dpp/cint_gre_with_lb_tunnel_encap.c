/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose: Example of GRE with LB key tunnel encapsulation
 *
 * Configuration Steps:
 *       Create GRE4 with LB key (GRE8) tunnel
 *       Add IPv4_1, IPv6_1 and MPLS_1 routes to GRE8 tunnel
 *       Create MPLSoGRE8 tunnel using previously created GRE8 tunnel
 *       Add IPv4_2 and IPv6_2 routes to MPLSoGRE8 tunnel
 *
 * Test scenario:
 * step 1: GRE8 Tunnel Encapsulation
 *         Send IPv4_1 packet, Receive IPv4_1oGRE8
 *         Send IPv6_1 packet, Receive IPv6_1oGRE8
 *         Send MPLS_1 packet, Receive MPLS_1oGRE8
 *
 * step 2: MPLS_2oGRE8 Tunnel Encapsulation
 *         Send IPv4_2 packet, Receive IPv4_2oMPLS_2oGRE8
 *         Send IPv6_2 packet, Receive IPv6_2oMPLS_2oGRE8
 *
 * Example:
 *
 *     INPUT, port 200    ______________________________     OUTPUT, port 201
 *                       |                    __________|
 *                       |                   |          |
 *        IPv4_1 ------> |------------------>| GRE8     | ------> IPv4_1oGRE8
 *        IPv6_1 ------> |------------------>| Tunnel   | ------> IPv6_1oGRE8
 *        MPLS_1 ------> |------------------>| Encap.   | ------> MPLS_1oGRE8
 *                       |   __________      |          |
 *                       |  |          |     |          |
 *        IPv4_2 ------> |->| MPLS     |---->|          | ------> IPv4_2oMPLS_2oGRE8
 *                       |  | Tunnel   |     |          |
 *        IPv6_2 ------> |->| Encap.   |---->|__________| ------> IPv6_2oMPLS_2oGRE8
 *                       |  |__________|                |
 *                       |______________________________|
 *
 * SOC properties:
 * system_ftmh_load_balancing_ext_mode=ENABLED
 *
 * CINT files:
 * cd ../../../../src/examples/dpp/
 * utility/cint_utils_l3.c
 * utility/cint_utils_vlan.c
 * utility/cint_tunnel_utils.c
 *
 * gre_with_lb_tunnel_encap__start_run(int unit, udp_tunnel_encap_s *test_data) - test_data = NULL for default params
 */


/*
 * Global Variables Definition and Initialization  START
 */

/* IP tunnel parameters */
struct ip_tunnel_info_s{
    bcm_tunnel_type_t tunnel_type;  /* tunnel type */
    bcm_mac_t da;       /* tunnel da */
    bcm_mac_t sa;       /* tunnel sa */
    int       dip;      /* tunnel dip */
    int       sip;      /* tunnel sip */
    uint32    flags;    /* tunnel flags */
    int       vlan;     /* tunnel vlan */
    int       ttl;      /* tunnel ttl */
    int       dscp;     /* tunnel dscp */
    bcm_tunnel_dscp_select_t dscp_sel; /* dscp_sel */
};

struct gre_with_lb_tunnel_encap_s{
    int in_port;     /* in port */
    int out_port;    /* out port */
    ip_tunnel_info_s gre_with_lb_tunnel;   /* GRE with LB key tunnel parameters */
};

gre_with_lb_tunnel_encap_s g_gre_with_lb_tunnel_encap =
{
    200,    /* in port */
    201,    /* out port */

    {   /* gre with LB key tunnel parameters */
        bcmTunnelTypeGreAnyIn4,               /* tunnel type */
        {0x20, 0x00, 0x00, 0x00, 0xab, 0xcd}, /* da */
        {0x00, 0x0C, 0x00, 0x02, 0x00, 0x00}, /* sa */
        0xAB000011,                           /* dip: 171.0.0.17 */
        0xAA000011,                           /* sip: 170.0.0.17 */
        BCM_TUNNEL_INIT_GRE_KEY_USE_LB,       /* gre with lb tunnel flag */
        100,                                  /* tunnel vlan */
        56,                                   /* ttl */
        11,                                   /* dscp */
        bcmTunnelDscpAssign                   /* dscp_sel */
    }
};

/*
 * Global Variables Definition and Initialization  END
 */

/* Get test data */
void gre_with_lb_tunnel_encap__get_struct(gre_with_lb_tunnel_encap_s *test_data)
{
    sal_memcpy(test_data, &g_gre_with_lb_tunnel_encap, sizeof(*test_data));
    return;
}

/* Initialize test data struct and check SoC property */
int gre_with_lb_tunnel_encap_init(int unit, gre_with_lb_tunnel_encap_s *test_data)
{
    if (test_data != NULL) {
       sal_memcpy(&g_gre_with_lb_tunnel_encap, test_data, sizeof(g_gre_with_lb_tunnel_encap));
    }

    /* check soc property system_ftmh_load_balancing_ext_mode==ENABLED*/
    if (soc_property_get(unit, "system_ftmh_load_balancing_ext_mode", 0)) {
        printf("fail, system_ftmh_load_balancing_ext_mode soc property is disabled\n");
        return BCM_E_CONFIG;
    }

    return BCM_E_NONE;
}

int gre_with_lb_tunnel_encap_create_rif_and_gports(int unit)
{
    int rv;

    rv = bcm_vlan_gport_add(unit, g__tunnel_utils.in_vlan, g_gre_with_lb_tunnel_encap.in_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("Error, bcm_vlan_gport_add\n");
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, g_gre_with_lb_tunnel_encap.gre_with_lb_tunnel.vlan, g_gre_with_lb_tunnel_encap.out_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("Error, bcm_vlan_gport_add\n");
        return rv;
    }

    /* set RIF properties */
    create_l3_intf_s intf;
    intf.vsi = g__tunnel_utils.in_vlan;
    intf.my_global_mac = g__tunnel_utils.my_mac;
    intf.my_lsb_mac = g__tunnel_utils.my_mac;
    intf.mtu_valid = 1;
    intf.mtu = 0;
    intf.mtu_forwarding = 0;
    intf.vrf_valid = 1;
    intf.vrf = g__tunnel_utils.vrf;

    /* create RIF */
    rv = l3__intf_rif__create(unit, &intf);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
        return rv;
    }

    return rv;
}

/* Main function, tcl interface */
int gre_with_lb_tunnel_encap__start_run_with_ports(int unit, int in_port, int out_port)
{
    int rv;
    gre_with_lb_tunnel_encap_s temp_struct;

    sal_memcpy(&temp_struct, &g_gre_with_lb_tunnel_encap, sizeof(temp_struct));

    temp_struct.in_port = in_port;
    temp_struct.out_port = out_port;

    rv = gre_with_lb_tunnel_encap__start_run(unit, &temp_struct);
    if (rv != BCM_E_NONE) {
        printf("Error, gre_with_lb_tunnel_encap__start_run\n");
        return rv;
    }

    return rv;
}

/* Main function */
int gre_with_lb_tunnel_encap__start_run(int unit, gre_with_lb_tunnel_encap_s *test_data)
{
    int rv;
    int udp_intf_id, mpls_udp_intf_id;
    bcm_if_t gre_intf_id;
    bcm_if_t mpls_intf_id;

    rv = gre_with_lb_tunnel_encap_init(unit, test_data);
    if (rv != BCM_E_NONE) {
        printf("Error, gre_with_lb_tunnel_encap_init\n");
        return rv;
    }

    /* set LB key switch control */
    rv = bcm_switch_control_port_set(unit, g_gre_with_lb_tunnel_encap.in_port, bcmSwitchTrunkHashPktHeaderCount ,2);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_port_set\n");
        return rv;
    }

    rv = gre_with_lb_tunnel_encap_create_rif_and_gports(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, gre_with_lb_tunnel_encap_create_rif_and_gports\n");
        return rv;
    }

    rv = gre_with_lb_tunnel_encap_tunnel_initiators_create(unit, &gre_intf_id, &mpls_intf_id);
    if (rv != BCM_E_NONE) {
        printf("Error, gre_with_lb_tunnel_encap_tunnel_initiators_create\n");
        return rv;
    }

    rv = gre_with_lb_tunnel_encap_add_gre_tunnel_routes(unit, gre_intf_id);
    if (rv != BCM_E_NONE) {
        printf("Error, gre_with_lb_tunnel_encap_add_gre_tunnel_routes\n");
        return rv;
    }

    rv = gre_with_lb_tunnel_encap_add_mpls_o_gre_tunnel_routes(unit, mpls_intf_id);
    if (rv != BCM_E_NONE) {
        printf("Error, gre_with_lb_tunnel_encap_add_mpls_o_gre_tunnel_routes\n");
        return rv;
    }

    return rv;
}

/* Create GRE8 and MPLS tunnel initiators */
int gre_with_lb_tunnel_encap_tunnel_initiators_create(int unit, bcm_if_t *gre_intfs_id, bcm_if_t *mpls_intfs_id)
{
    int rv;

    /* create GRE with LB key tunnel initiator */
    rv = gre_with_lb_tunnel_encap_gre_with_lb_tunnel_create(unit, 0x8847 /* mpls port */, gre_intfs_id);
    if (rv != BCM_E_NONE) {
        printf("Error, gre_with_lb_tunnel_encap_gre_with_lb_tunnel_create\n");
        return rv;
    }

    /* Set GRE tunnel ARP entry parameters */
    create_l3_egress_s l3eg;
    sal_memcpy(l3eg.next_hop_mac_addr, g_gre_with_lb_tunnel_encap.gre_with_lb_tunnel.da, 6);
    l3eg.out_tunnel_or_rif = *gre_intfs_id;
    l3eg.out_gport = g_gre_with_lb_tunnel_encap.out_port;
    l3eg.vlan = g_gre_with_lb_tunnel_encap.gre_with_lb_tunnel.vlan;

    /* Set ARP entry, without allocating FEC entry to the gre tunnel */
    rv = l3__egress_only_encap__create(unit, &l3eg);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
        return rv;
    }

    /* create MPLS tunnel inititator pointing to previously created GRE tunnel */
    rv = tunnel__mpls_encap_create(unit, *gre_intfs_id, mpls_intfs_id);
    if (rv != BCM_E_NONE) {
        printf("Error, tunnel__mpls_encap_create\n");
        return rv;
    }
    printf("END tunnel__mpls_encap_create\n");

    return rv;
}

/* create FEC and add ipv4, ipv6 and mpls routes */
int gre_with_lb_tunnel_encap_add_gre_tunnel_routes(int unit, bcm_if_t gre_intf_id)
{
    int rv;
    bcm_if_t gre_with_lb_tunnel_fec_id;

    /* set GRE with LB key tunnel FEC parameters */
    create_l3_egress_s l3eg;
    l3eg.out_tunnel_or_rif = gre_intf_id;
    l3eg.vlan = g_gre_with_lb_tunnel_encap.gre_with_lb_tunnel.vlan;
    l3eg.out_gport = g_gre_with_lb_tunnel_encap.out_port;

    /* Allocate FEC for GRE Tunnel*/
    rv = l3__egress_only_fec__create(unit, &l3eg);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
        return rv;
    }

    gre_with_lb_tunnel_fec_id = l3eg.fec_id;

    rv = tunnel__add_ipv4_route(unit, gre_with_lb_tunnel_fec_id, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, udp_tunnel_encap_add_ipv4_route\n");
        return rv;
    }

    rv = tunnel__add_ipv6_route(unit, gre_with_lb_tunnel_fec_id, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, udp_tunnel_encap_add_ipv4_route\n");
        return rv;
    }

    rv = tunnel__add_mpls_route(unit, g_gre_with_lb_tunnel_encap.in_port, gre_with_lb_tunnel_fec_id, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, gre_with_lb_tunnel_encap_add_mpls_route\n");
        return rv;
    }

    return rv;
}

/* create FEC and add IPv4 and IPv6 routes */
int gre_with_lb_tunnel_encap_add_mpls_o_gre_tunnel_routes(int unit, bcm_if_t mpls_intf_id)
{
    int rv;
    bcm_if_t mpls_tunnel_fec_id;

    create_l3_egress_s l3eg;
    l3eg.out_tunnel_or_rif = mpls_intf_id;
    l3eg.out_gport = g_gre_with_lb_tunnel_encap.out_port;
    l3eg.vlan = g_gre_with_lb_tunnel_encap.gre_with_lb_tunnel.vlan;

    /* Allocate FEC for MPLSoGRE tunnel */
    rv = l3__egress_only_fec__create(unit, &l3eg);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress_only_fec__create\n");
        return rv;
    }

    mpls_tunnel_fec_id = l3eg.fec_id;

    rv = tunnel__add_ipv4_route(unit, mpls_tunnel_fec_id, 2);
    if (rv != BCM_E_NONE) {
        printf("Error, gre_with_lb_tunnel_encap_add_ipv6_route\n");
        return rv;
    }

    rv = tunnel__add_ipv6_route(unit, mpls_tunnel_fec_id, 2);
    if (rv != BCM_E_NONE) {
        printf("Error, gre_with_lb_tunnel_encap_add_ipv6_route\n");
        return rv;
    }

    return rv;
}

/* create GRE with LB key (GRE8) tunnel (ip_tunnel_info_s gre_with_lb_tunnel)
 * Parameters: gre_protocol_type - configure global GRE.protocol (when not IPvX or Eth)
 *             gre_with_lb_intf_id - interface id of the created GRE8 tunnel */
int gre_with_lb_tunnel_encap_gre_with_lb_tunnel_create(int unit, int gre_protocol_type, bcm_if_t* gre_with_lb_intf_id)
{
    int rv;
    bcm_tunnel_initiator_t tunnel_info;
    bcm_l3_intf_t l3_intf;

    /* configure global GRE.protocol (set GRE.protocol field when next header is neither IPvX or Eth) */
    rv = bcm_switch_control_set(unit, bcmSwitchL2GreProtocolType, gre_protocol_type);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set\n");
        return rv;
    }

    bcm_tunnel_initiator_t_init(&tunnel_info);
    tunnel_info.type = g_gre_with_lb_tunnel_encap.gre_with_lb_tunnel.tunnel_type;  /* default: bcmTunnelTypeGreAnyIn4*/
    tunnel_info.dip = g_gre_with_lb_tunnel_encap.gre_with_lb_tunnel.dip;           /* default: 171.0.0.17*/
    tunnel_info.sip = g_gre_with_lb_tunnel_encap.gre_with_lb_tunnel.sip;           /* default: 170.0.0.17*/
    tunnel_info.flags = g_gre_with_lb_tunnel_encap.gre_with_lb_tunnel.flags;       /* default: BCM_TUNNEL_INIT_GRE_KEY_USE_LB*/
    tunnel_info.vlan = g_gre_with_lb_tunnel_encap.gre_with_lb_tunnel.vlan;         /* default: 100*/
    tunnel_info.ttl = g_gre_with_lb_tunnel_encap.gre_with_lb_tunnel.ttl;           /* default: 56 */
    tunnel_info.dscp = g_gre_with_lb_tunnel_encap.gre_with_lb_tunnel.dscp;         /* default: 11 */
    tunnel_info.dscp_sel = g_gre_with_lb_tunnel_encap.gre_with_lb_tunnel.dscp_sel; /* default: bcmTunnelDscpAssign */

    bcm_l3_intf_t_init(&l3_intf);
    rv = bcm_tunnel_initiator_create(unit, &l3_intf, &tunnel_info);
    if(rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_initiator_create\n");
        return rv;
    }

    *gre_with_lb_intf_id = l3_intf.l3a_intf_id;

    return rv;
}
