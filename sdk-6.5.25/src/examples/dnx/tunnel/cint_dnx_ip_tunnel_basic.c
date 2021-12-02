/*
 * $Id: cint_dnx_ip_tunnel_basic.c,v 1.15 2021/09/13 fh959538
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.File: cint_dnx_ip_tunnel_basic.c Purpose: Various examples for IP Tunnels in Jer2        .
 *                                                                                                                     .
 * Test Scenario: All kind of tunnel encapsulation and termination with ipv4/ipv6/MPLS fwd.
 *
 * Test Scenario
 *
 *GRE4oIpv4 tunnel encapsulation with MPLS as forwarding
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_global.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_l3.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utility_mpls.c
 * cint ../../src/./examples/dnx/l3/cint_dnx_ip_route_basic.c
 * cint ../../src/./examples/dnx/tunnel/cint_dnx_ip_tunnel_basic.c
 * cint
 * ip_tunnel_encap_mpls_fwd(0,201,202);
 * exit;
 *
 * MPLS over ethernet packet
 * tx 1 psrc=201 data=0x000c000200560050000012348847005b0140000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 0 should be:
 *  Source port: 0, Destination port: 0
 *  Data: 0x00020000cd1d000c000200568100a0640800450a004800000000382f415ba0000011a100001100008847005b013f000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 */

struct cint_dnx_ip_tunnel_basic_info_s
{
    int eth_rif_intf_out;          /* out RIF */
    int eth_rif_intf_in;            /* in RIF */
    int eth_rif_intf_in2;            /* in RIF2 */
    bcm_mac_t intf_in_mac_address;  /* mac for in RIF */
    bcm_mac_t intf_out_mac_address;  /* mac for in RIF */
    bcm_mac_t tunnel_next_hop_mac;      /* mac for next hop */
    int in_vrf;                      /* VRF, resolved as Tunnel-InLif property */
    int in_vrf2;                     /* VRF2, resolved as Tunnel-InLif property */
    bcm_ip_t  route_ipv4_dip;           /* Route dip */
    bcm_ip6_t route_ipv6_dip;           /* Route dip6 */
    int mpls_fwd_label;                 /* MPLS label used for forwarding */
    int tunnel_fec_id;                  /* FEC id */
    int tunnel_arp_id;                  /* Id for tunnel encap entry */
    int arp_id;                         /* Id for ARP entry */
    int tunnel_term_id;                 /* Ingress terminated tunnel ID */
    bcm_ip_t  term_dip_mask;            /* mask of IPV4 dip for tunnle termination*/
    bcm_ip_t  term_sip_mask;            /* mask of IPV4 sip for tunnle termination */
};

cint_dnx_ip_tunnel_basic_info_s cint_dnx_ip_tunnel_basic_info = {
    /*
     * eth_rif_intf_out | eth_rif_intf_in | eth_rif_intf_in2
     */
    100, 15, 16,
    /*
     * intf_in_mac_address            intf_out_mac_address              tunnel_next_hop_mac
     */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x56}, {0x00, 0x0c, 0x00, 0x02, 0x00, 0x56}, {0x00, 0x02, 0x00, 0x00, 0xcd, 0x1d},
    /*
     * in_vrf | in_vrf2
     */
    1, 2,
    /*
     * route dip ipv4
     */
    0x7fffff03, /* 127.255.255.03 */
    /*
     * ipv6_dip
     */
    {
     0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8,
     0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 0xec},
     /*
      * mpls_fwd_label
      */
     1456,
    /*
     * tunnel_fec_id
     */
    0,
    /*
     * tunnel_arp_id
     */
    0,
    /*
     * arp_id
     */
    0,
     /*
      * tunnel_term_id
      */
     0,
     /*
      * term_dip_mask
      */
     0xffffffff, /* 255.255.255.255 */
     /*
      * term_sip_mask
      */
     0xffffffff /* 255.255.255.255 */
};

/* ip tunnel encapsulation for outgoing packet */
int dnx_ip_tunnel_encap =1;
/* ip tunnel termination for incoming packet */
int dnx_ip_tunnel_termination =1;

/*
 * Configure in-RIF, forwarding entries, and out-RIF, not include fec
 */
int
dnx_ip_tunnel_fwd_configre(
    int unit,
    int in_port,
    int out_port)
{
    int rv;
    l3_ingress_intf ingress_rif;
    int arp_id;
    bcm_gport_t gport;
    int fec;
    int encoded_fec;
    mpls_util_mpls_tunnel_switch_create_s mpls_switch_tunnel[1];

    int intf_in = cint_dnx_ip_tunnel_basic_info.eth_rif_intf_in;      /* Incoming packet ETH-RIF */
    int intf_out = cint_dnx_ip_tunnel_basic_info.eth_rif_intf_out;     /* Outgoing packet ETH-RIF */
    int vrf = cint_dnx_ip_tunnel_basic_info.in_vrf;
    bcm_mac_t intf_in_mac_address = cint_dnx_ip_tunnel_basic_info.intf_in_mac_address;     /* my-MAC */
    bcm_mac_t intf_out_mac_address = cint_dnx_ip_tunnel_basic_info.intf_out_mac_address;    /* my-MAC */
    bcm_mac_t arp_next_hop_mac = cint_dnx_ip_tunnel_basic_info.tunnel_next_hop_mac;        /* next_hop_mac */
    bcm_ip_t dip_ipv4 = cint_dnx_ip_tunnel_basic_info.route_ipv4_dip;
    bcm_ip6_t dip_ipv6 = cint_dnx_ip_tunnel_basic_info.route_ipv6_dip;
    uint32 mask_ipv4 = 0xffffff00;
    bcm_ip6_t mask_ipv6   = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0,0,0,0,0,0,0,0};
    int fwd_label   = cint_dnx_ip_tunnel_basic_info.mpls_fwd_label;

    char *proc_name;
    proc_name = "dnx_ip_tunnel_fwd_configre";

    fec = 0;
    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }
    BCM_L3_ITF_SET(cint_dnx_ip_tunnel_basic_info.tunnel_fec_id, BCM_L3_ITF_TYPE_FEC, fec);

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, in_port, intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set intf_in\n",proc_name);
        return rv;
    }
    /*
     * 2. Set Out-Port default properties
     */
    rv = out_port_set(unit, out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set intf_out out_port %d\n",proc_name,out_port);
        return rv;
    }

    /*
     * 3. Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, intf_in, intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in %d\n",proc_name,intf_in);
        return rv;
    }
    rv = intf_eth_rif_create(unit, intf_out, intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }
    /*
     * 4. Set Incoming ETH-RIF properties
     */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = vrf;
    ingress_rif.intf_id = intf_in;
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }


    /*
     * 5. Create ARP and set its properties
     */
    rv = l3__egress_only_encap__create(unit, 0, &arp_id, arp_next_hop_mac, intf_out);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only: encap_id = %d, vsi = %d, err_id = %d\n", proc_name,
               arp_id, intf_out, rv);
        return rv;
    }
    cint_dnx_ip_tunnel_basic_info.arp_id = arp_id;

    /*
     * 6. Create FEC and set its properties
     * Create fec when tunnel EEDB allocated
     */


    /*
     * 7. Add Route entry
     */
    printf("%s(): Add route entry.  route 0x%08X mask 0x%08X vrf %d fec 0x%08X. \n",proc_name, dip_ipv4, mask_ipv4, vrf, fec);
    rv = add_route_ipv4(unit, dip_ipv4, mask_ipv4, vrf, fec);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, add_route_ipv4 \n",proc_name);
        return rv;
    }

    printf("%s(): Add IPV6 route entry. \n",proc_name);
    rv = add_route_ipv6(unit, dip_ipv6, mask_ipv6, vrf, fec);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, add_route_ipv6 \n",proc_name);
        return rv;
    }

    printf("%s(): Add MPLS fwd entry.  label %d fec 0x%08X. \n",proc_name, fwd_label, fec);
    mpls_tunnel_switch_create_s_init(mpls_switch_tunnel,1);
    mpls_switch_tunnel[0].label=fwd_label;
    mpls_switch_tunnel[0].egress_if = &fec;
    mpls_switch_tunnel[0].action  = BCM_MPLS_SWITCH_ACTION_NOP;
    mpls_switch_tunnel[0].flags  = BCM_MPLS_SWITCH_TTL_DECREMENT;
    rv = mpls_create_switch_tunnels(unit, mpls_switch_tunnel, 1, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, mpls_create_switch_tunnels \n",proc_name);
        return rv;
    }

    printf("%s(): Exit\r\n",proc_name);
    return rv;
}

/*
 * Configure fec
 */
int
dnx_ip_tunnel_fec_configre(
    int unit,
    int out_port)
{
    int rv;
    bcm_gport_t gport;
    int encoded_fec;
    int encap_id = cint_dnx_ip_tunnel_basic_info.tunnel_arp_id;
    int intf_out = cint_dnx_ip_tunnel_basic_info.eth_rif_intf_out;
    int fec =cint_dnx_ip_tunnel_basic_info.tunnel_fec_id;

    char *proc_name;
    proc_name = "dnx_ip_tunnel_fec_configre";

    if (dnx_ip_tunnel_encap ==0) {
        encap_id = cint_dnx_ip_tunnel_basic_info.arp_id;
    }

    printf("%s():. Create main FEC and set its properties.\r\n",proc_name);
    if (BCM_GPORT_IS_SET(out_port)) {
       gport = out_port;
    }
    else {
       BCM_GPORT_LOCAL_SET(gport, out_port);
    }

    int fec_flags2 = 0;
    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }

    rv = l3__egress_only_fec__create_inner(unit, fec, intf_out, encap_id, gport, 0,
                                           fec_flags2, 0, 0,&encoded_fec);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC only\n",proc_name);
        return rv;
    }
    printf("%s(): fec 0x%08X encoded_fec 0x%08X.\r\n",proc_name, fec, encoded_fec);

    return rv;

}

/*
 * create tunnel egress entry
 */
int
dnx_ip_tunnel_initiator_create(
    int unit,
    bcm_tunnel_initiator_t tunnel_init,
    bcm_if_t *tunnel_intf)
{
    int rv;
    bcm_l3_intf_t l3_intf;
    bcm_l3_intf_t_init(&l3_intf);

    /* Create IP tunnel initiator*/
    rv = bcm_tunnel_initiator_create(unit,&l3_intf, tunnel_init);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_initiator_create \n");
    }
    cint_dnx_ip_tunnel_basic_info.tunnel_arp_id=tunnel_init.tunnel_id;
    printf(" Create egress IP tunnel: 0x%08X.\n", l3_intf.l3a_intf_id);

    *tunnel_intf = l3_intf.l3a_intf_id;
    return rv;
}

/*
 * create tunnel ingress entry
 */
int
dnx_ip_tunnel_terminator_create(
    int unit,
    bcm_tunnel_terminator_t tunnel_term)
{
    int rv;
    bcm_l3_ingress_t ing_intf;

    /* Create IP tunnel initiator*/
    rv = bcm_tunnel_terminator_create(unit, &tunnel_term);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_terminator_create \n");
    }
    cint_dnx_ip_tunnel_basic_info.tunnel_term_id = tunnel_term.tunnel_id;
    printf("Create ingress IP tunnel: 0x%08X\n", tunnel_term.tunnel_id);
    /*update the VRF */
    if ((tunnel_term.type != bcmTunnelTypeVxlanGpe)
        && (tunnel_term.type != bcmTunnelTypeVxlan6Gpe)){
         bcm_l3_ingress_t_init(&ing_intf);
         ing_intf.flags = BCM_L3_INGRESS_WITH_ID; /* must, as we update exist RIF */
         ing_intf.vrf   = cint_dnx_ip_tunnel_basic_info.in_vrf;

         /** Convert tunnel's GPort ID to intf ID */
          bcm_if_t intf_id;
          BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(intf_id, tunnel_term.tunnel_id);

          rv = bcm_l3_ingress_create(unit, ing_intf, intf_id);
          if (rv != BCM_E_NONE) {
              printf("Error, bcm_l3_ingress_create\n");
              return rv;
          }
    }

    return rv;
}

/*
 * Disable tunnel lif IPV4 routing function
 */
int
dnx_ip_tunnel_term_traffic_disable(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_l3_ingress_t ing_intf;
    bcm_l3_ingress_t_init(&ing_intf);
    ing_intf.flags = BCM_L3_INGRESS_WITH_ID | BCM_L3_INGRESS_ROUTE_DISABLE_IP4_UCAST; /* must, as we update exist RIF */
    ing_intf.vrf   = cint_dnx_ip_tunnel_basic_info.in_vrf;
     /** Convert tunnel's GPort ID to intf ID */
    bcm_if_t intf_id;
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(intf_id, cint_dnx_ip_tunnel_basic_info.tunnel_term_id);

    rv = bcm_l3_ingress_create(unit, ing_intf, intf_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_ingress_create\n");
        return rv;
    }
    return rv;
}

/*
 * Main function for basic ip tunnel encapsulation scenarios
 * The function dnx_ip_tunnel_encap_gre4 implements the basic tunnel encapsulation scenario.
 * Inputs:
 * unit - relevant unit;
 * in_port - incoming port;
 * out_port - outgoing port;
 */
int
dnx_ip_tunnel_encap_gre4(
    int unit,
    int in_port,
    int out_port)
{
    int rv;
    bcm_tunnel_initiator_t tunnel_init;
    bcm_if_t tunnel_intf=0;
    char *proc_name;
    proc_name = "dnx_ip_tunnel_encap_gre4";

    rv = dnx_ip_tunnel_fwd_configre(unit,in_port,out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_fwd_configre \n", proc_name);
        return rv;
    }

    /* Create IP tunnel initiator for encapsulating GRE4oIPv4 tunnel header*/
    bcm_tunnel_initiator_t_init(&tunnel_init);
    tunnel_init.dip        = 0xA1000011; /* 161.0.0.17 */
    tunnel_init.sip        = 0xA0000011; /* 160.0.0.17 */
    tunnel_init.flags      = BCM_TUNNEL_INIT_STAT_ENABLE ;
    tunnel_init.dscp       = 10;
    tunnel_init.type       = bcmTunnelTypeGreAnyIn4;
    tunnel_init.ttl        = 64;
    tunnel_init.l3_intf_id = cint_dnx_ip_tunnel_basic_info.arp_id ;
    tunnel_init.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    tunnel_init.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE; /* default: -1 */

    rv = dnx_ip_tunnel_initiator_create(unit, tunnel_init, &tunnel_intf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_initiator_create \n", proc_name);
        return rv;
    }

    rv = dnx_ip_tunnel_fec_configre(unit,out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_fwd_configre \n", proc_name);
        return rv;
    }
    return rv;
}

/*
 * function for GRE8 encapsulation scenarios
 * Inputs:
 * unit - relevant unit;
 * in_port - incoming port;
 * out_port - outgoing port;
 */
int
dnx_ip_tunnel_encap_gre8(
    int unit,
    int in_port,
    int out_port)
{
    int rv;
    bcm_tunnel_initiator_t tunnel_init;
    bcm_if_t tunnel_intf=0;
    char *proc_name;
    proc_name = "dnx_ip_tunnel_encap_gre8";

    rv = dnx_ip_tunnel_fwd_configre(unit,in_port,out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_fwd_configre \n", proc_name);
        return rv;
    }

    /* Create IP tunnel initiator for encapsulating GRE4oIPv4 tunnel header*/
    bcm_tunnel_initiator_t_init(&tunnel_init);
    tunnel_init.dip        = 0xA1000011; /* 161.0.0.17 */
    tunnel_init.sip        = 0xA0000011; /* 160.0.0.17 */
    tunnel_init.flags      = BCM_TUNNEL_INIT_STAT_ENABLE | BCM_TUNNEL_INIT_GRE_KEY_USE_LB;
    tunnel_init.dscp       = 10;
    tunnel_init.type       = bcmTunnelTypeGreAnyIn4;
    tunnel_init.ttl        = 64;
    tunnel_init.l3_intf_id = cint_dnx_ip_tunnel_basic_info.arp_id ;
    tunnel_init.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    tunnel_init.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE; /* default: -1 */

    rv = dnx_ip_tunnel_initiator_create(unit, tunnel_init, &tunnel_intf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_initiator_create \n", proc_name);
        return rv;
    }

    rv = dnx_ip_tunnel_fec_configre(unit,out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_fwd_configre \n", proc_name);
        return rv;
    }
    return rv;
}

/*
 * function for MPLSoGRE4 encapsulation scenarios
 * Inputs:
 * unit - relevant unit;
 * in_port - incoming port;
 * out_port - outgoing port;
 */
int
dnx_ip_tunnel_encap_MPLSoGRE(
    int unit,
    int in_port,
    int out_port)
{
    int rv;
    int encap_label = 200;
    bcm_tunnel_initiator_t tunnel_init;
    mpls_util_mpls_tunnel_initiator_create_s mpls_tunnel_initiator[1];
    bcm_if_t tunnel_intf=0;
    int ip_tunnel_id=0;
    char *proc_name;
    proc_name = "dnx_ip_tunnel_encap_MPLSoGRE";

    rv = dnx_ip_tunnel_fwd_configre(unit,in_port,out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_fwd_configre \n", proc_name);
        return rv;
    }

    /* Create IP tunnel initiator for encapsulating GRE4oIPv4 tunnel header*/
    bcm_tunnel_initiator_t_init(&tunnel_init);
    tunnel_init.dip        = 0xA1000011; /* 161.0.0.17 */
    tunnel_init.sip        = 0xA0000011; /* 160.0.0.17 */
    tunnel_init.flags      = BCM_TUNNEL_INIT_STAT_ENABLE;
    tunnel_init.dscp       = 10;
    tunnel_init.type       = bcmTunnelTypeGreAnyIn4;
    tunnel_init.ttl        = 64;
    tunnel_init.l3_intf_id = cint_dnx_ip_tunnel_basic_info.arp_id ;
    tunnel_init.encap_access = bcmEncapAccessTunnel4;
    tunnel_init.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    tunnel_init.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE; /* default: -1 */
    rv = dnx_ip_tunnel_initiator_create(unit, tunnel_init, &tunnel_intf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_initiator_create \n", proc_name);
        return rv;
    }
    ip_tunnel_id = cint_dnx_ip_tunnel_basic_info.tunnel_arp_id;

    mpls_tunnel_initiator_create_s_init(mpls_tunnel_initiator,1);
    mpls_tunnel_initiator[0].label[0]=encap_label;
    mpls_tunnel_initiator[0].num_labels = 1;
    mpls_tunnel_initiator[0].l3_intf_id  = &tunnel_intf;
    mpls_tunnel_initiator[0].flags  = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    rv = mpls_create_initiator_tunnels(unit, mpls_tunnel_initiator, 1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, mpls_create_switch_tunnels \n",proc_name);
        return rv;
    }
    cint_dnx_ip_tunnel_basic_info.tunnel_arp_id=mpls_tunnel_initiator[0].tunnel_id;

    rv = dnx_ip_tunnel_fec_configre(unit,out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_fwd_configre \n", proc_name);
        return rv;
    }

    return rv;
}

/*
 * Main function for basic ip tunnel encapsulation scenarios
 * The function dnx_ip_tunnel_encap_udp4 implements the basic tunnel encapsulation scenario.
 * Inputs:
 * unit - relevant unit;
 * in_port - incoming port;
 * out_port - outgoing port;
 */
int
dnx_ip_tunnel_encap_udp4(
    int unit,
    int in_port,
    int out_port)
{
    int rv;
    bcm_tunnel_initiator_t tunnel_init;
    bcm_if_t tunnel_intf=0;
    char *proc_name;
    proc_name = "dnx_ip_tunnel_encap_gre4";

    rv = dnx_ip_tunnel_fwd_configre(unit,in_port,out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_fwd_configre \n", proc_name);
        return rv;
    }

    /* Create IP tunnel initiator for encapsulating GRE4oIPv4 tunnel header*/
    bcm_tunnel_initiator_t_init(&tunnel_init);
    tunnel_init.dip        = 0xA1000011; /* 161.0.0.17 */
    tunnel_init.sip        = 0xA0000011; /* 160.0.0.17 */
    tunnel_init.flags      = BCM_TUNNEL_INIT_STAT_ENABLE ;
    tunnel_init.dscp       = 10;
    tunnel_init.type       = bcmTunnelTypeUdp;
    tunnel_init.ttl        = 64;
    tunnel_init.l3_intf_id = cint_dnx_ip_tunnel_basic_info.arp_id ;
    tunnel_init.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    tunnel_init.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE; /* default: -1 */

    rv = dnx_ip_tunnel_initiator_create(unit, tunnel_init, &tunnel_intf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_initiator_create \n", proc_name);
        return rv;
    }

    rv = dnx_ip_tunnel_fec_configre(unit,out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_fwd_configre \n", proc_name);
        return rv;
    }
    return rv;
}

/*
 * function for GRE4 termination scenarios
 * Inputs:
 * unit - relevant unit;
 * in_port - incoming port;
 * out_port - outgoing port;
 */
int
dnx_ip_tunnel_term_gre4(
    int unit,
    int in_port,
    int out_port)
{
    int rv;
    bcm_tunnel_terminator_t tunnel_term;
    bcm_if_t tunnel_intf=0;
    char *proc_name;
    proc_name = "dnx_ip_tunnel_term_gre4";

    dnx_ip_tunnel_encap=0;

    rv = dnx_ip_tunnel_fwd_configre(unit,in_port,out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_fwd_configre \n", proc_name);
        return rv;
    }

    rv = dnx_ip_tunnel_fec_configre(unit,out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_fwd_configre \n", proc_name);
        return rv;
    }

    /* Create IP tunnel terminator for GRE4oIPv4 tunnel header*/
    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.dip        = 0xA1000011; /* 161.0.0.17 */
    tunnel_term.sip        = 0xA0000011; /* 160.0.0.17 */
    tunnel_term.dip_mask   = cint_dnx_ip_tunnel_basic_info.term_dip_mask;
    tunnel_term.sip_mask   = cint_dnx_ip_tunnel_basic_info.term_sip_mask;
    tunnel_term.flags      = BCM_TUNNEL_INIT_STAT_ENABLE ;
    tunnel_term.vrf        = cint_dnx_ip_tunnel_basic_info.in_vrf;
    tunnel_term.type       = bcmTunnelTypeGreAnyIn4;
    rv = dnx_ip_tunnel_terminator_create(unit, tunnel_term);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_terminator_create \n", proc_name);
        return rv;
    }
    return rv;
}

/*
 * function for GRE4 termination with mask scenarios
 * Inputs:
 * unit - relevant unit;
 * in_port - incoming port;
 * out_port - outgoing port;
 */
int
dnx_ip_tunnel_term_gre4_with_subnet_mask(
    int unit,
    int in_port,
    int out_port)
{
    int rv;
    char *proc_name;
    proc_name = "dnx_ip_tunnel_term_gre4_with_subnet_mask";

    cint_dnx_ip_tunnel_basic_info.term_dip_mask = 0xFFF00000;  /* subnet 255.240.0.0 */
    cint_dnx_ip_tunnel_basic_info.term_sip_mask = 0xFFFE0000;  /* subnet 255.254.0.0 */

    rv = dnx_ip_tunnel_term_gre4(unit,in_port,out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_term_gre4 \n", proc_name);
        return rv;
    }

    return rv;
}

/*
 * function for IpAnyIn4 termination scenarios
 * Inputs:
 * unit - relevant unit;
 * in_port - incoming port;
 * out_port - outgoing port;
 * with_subnet_mask - 0: with full mask, save to EM, 1: with subnet mask, save to TCAM
 */
int
dnx_ip_tunnel_term_gre4_with_faulty_ip(
    int unit,
    int in_port,
    int out_port,
    int with_subnet_mask)
{
    int rv;
    int fec = 0;
    int vrf = cint_dnx_ip_tunnel_basic_info.in_vrf;
    bcm_ip_t dip_ipv4        = 0xA1000011; /* 161.0.0.17 */
    bcm_ip_t dip2_ipv4       = 0xAD000001; /* 173.0.0.1 */
    bcm_ip_t mask_ipv4       = 0xFFFFFF00; /* 255.255.255.0 */
    char *proc_name;
    proc_name = "dnx_ip_tunnel_term_gre4_with_subnet_mask";

    if (with_subnet_mask==1) {
        rv = dnx_ip_tunnel_term_gre4(unit,in_port,out_port);
    } else {
        rv = dnx_ip_tunnel_term_gre4_with_subnet_mask(unit,in_port,out_port);
    }
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_term_gre4 \n", proc_name);
        return rv;
    }

    /*
     * 1. Add Route entry for tunnel dip
     */
    fec = cint_dnx_ip_tunnel_basic_info.tunnel_fec_id;
    printf("%s(): Add route entry.  route 0x%08X mask 0x%08X vrf %d fec 0x%08X. \n",proc_name, dip_ipv4, mask_ipv4, vrf, fec);
    rv = add_route_ipv4(unit, dip_ipv4, mask_ipv4, vrf, fec);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, add_route_ipv4 \n",proc_name);
        return rv;
    }

    /*
     * 2. Add Route entry for faulty dip
     */
    printf("%s(): Add route entry.  route 0x%08X mask 0x%08X vrf %d fec 0x%08X. \n",proc_name, dip2_ipv4, mask_ipv4, vrf, fec);
    rv = add_route_ipv4(unit, dip2_ipv4, mask_ipv4, vrf, fec);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, add_route_ipv4 \n",proc_name);
        return rv;
    }
    return rv;
}

/*
 * function for IpAnyIn4 termination scenarios
 * Inputs:
 * unit - relevant unit;
 * in_port - incoming port;
 * in_port_faulty - incoming port with wrong VRF;
 * out_port - outgoing port;
 * with_subnet_mask - 0: with full mask, save to EM, 1: with subnet mask, save to TCAM
 */
int
dnx_ip_tunnel_term_gre4_with_faulty_port(
    int unit,
    int in_port,
    int in_port_faulty,
    int out_port,
    int with_subnet_mask)
{
    int rv;
    int fec = 0;
    int intf_in2 = cint_dnx_ip_tunnel_basic_info.eth_rif_intf_in2;      /* Incoming packet ETH-RIF */
    bcm_mac_t intf_in_mac_address = cint_dnx_ip_tunnel_basic_info.intf_in_mac_address;     /* my-MAC */
    int vrf2 = cint_dnx_ip_tunnel_basic_info.in_vrf2;
    bcm_ip_t dip_ipv4        = 0xA1000011; /* 161.0.0.17 */
    bcm_ip_t mask_ipv4       = 0xFFFFFF00; /* 255.255.255.0 */
    char *proc_name;
    proc_name = "dnx_ip_tunnel_term_gre4_with_faulty_port";

    if (with_subnet_mask==1) {
        rv = dnx_ip_tunnel_term_gre4(unit,in_port,out_port);
    } else {
        rv = dnx_ip_tunnel_term_gre4_with_subnet_mask(unit,in_port,out_port);
    }
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_term_gre4 \n", proc_name);
        return rv;
    }

    /*
     * 2. Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, in_port_faulty, intf_in2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set intf_in\n",proc_name);
        return rv;
    }
    /*
     * 3. Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, intf_in2, intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in %d\n",proc_name,intf_in);
        return rv;
    }

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = vrf2;
    ingress_rif.intf_id = intf_in2;
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    /*
     * 5. Add Route entry for tunnel dip
     */
    fec = cint_dnx_ip_tunnel_basic_info.tunnel_fec_id;
    printf("%s(): Add route entry.  route 0x%08X mask 0x%08X vrf %d fec 0x%08X. \n",proc_name, dip_ipv4, mask_ipv4, vrf2, fec);
    rv = add_route_ipv4(unit, dip_ipv4, mask_ipv4, vrf2, fec);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, add_route_ipv4 \n",proc_name);
        return rv;
    }
    return rv;
}

/*
 * function for IpAnyIn4 termination scenarios
 * Inputs:
 * unit - relevant unit;
 * in_port - incoming port;
 * out_port - outgoing port;
 */
int
dnx_ip_tunnel_term_ip4(
    int unit,
    int in_port,
    int out_port)
{
    int rv;
    bcm_tunnel_terminator_t tunnel_term;
    bcm_if_t tunnel_intf=0;
    char *proc_name;
    proc_name = "dnx_ip_tunnel_term_ip4";

    dnx_ip_tunnel_encap=0;

    rv = dnx_ip_tunnel_fwd_configre(unit,in_port,out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_fwd_configre \n", proc_name);
        return rv;
    }

    rv = dnx_ip_tunnel_fec_configre(unit,out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_fwd_configre \n", proc_name);
        return rv;
    }

    /* Create IP tunnel terminator for GRE4oIPv4 tunnel header*/
    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.dip        = 0xA1000011; /* 161.0.0.17 */
    tunnel_term.sip        = 0xA0000011; /* 160.0.0.17 */
    tunnel_term.dip_mask   = cint_dnx_ip_tunnel_basic_info.term_dip_mask;
    tunnel_term.sip_mask   = cint_dnx_ip_tunnel_basic_info.term_sip_mask;
    tunnel_term.flags      = BCM_TUNNEL_INIT_STAT_ENABLE ;
    tunnel_term.vrf        = cint_dnx_ip_tunnel_basic_info.in_vrf;
    tunnel_term.type       = bcmTunnelTypeIpAnyIn4;
    rv = dnx_ip_tunnel_terminator_create(unit, tunnel_term);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_terminator_create \n", proc_name);
        return rv;
    }
    return rv;
}

/*
 * function for IpAnyIn4 termination with mask scenarios
 * Inputs:
 * unit - relevant unit;
 * in_port - incoming port;
 * out_port - outgoing port;
 */
int
dnx_ip_tunnel_term_ip4_with_subnet_mask(
    int unit,
    int in_port,
    int out_port)
{
    int rv;
    char *proc_name;
    proc_name = "dnx_ip_tunnel_term_ip4_with_subnet_mask";

    cint_dnx_ip_tunnel_basic_info.term_dip_mask = 0xFFF00000;  /* subnet 255.240.0.0 */
    cint_dnx_ip_tunnel_basic_info.term_sip_mask = 0xFFFE0000;  /* subnet 255.254.0.0 */

    rv = dnx_ip_tunnel_term_ip4(unit,in_port,out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_term_ip4 \n", proc_name);
        return rv;
    }

    return rv;
}


/*
 * function for IpAnyIn4 termination scenarios
 * Inputs:
 * unit - relevant unit;
 * in_port - incoming port;
 * in_port_faulty - incoming port with wrong VRF;
 * out_port - outgoing port;
 * with_subnet_mask - 0: with full mask, save to EM, 1: with subnet mask, save to TCAM
 */
int
dnx_ip_tunnel_term_ip4_with_faulty_port(
    int unit,
    int in_port,
    int in_port_faulty,
    int out_port,
    int with_subnet_mask)
{
    int rv;
    int intf_in2 = cint_dnx_ip_tunnel_basic_info.eth_rif_intf_in2;      /* Incoming packet ETH-RIF */
    bcm_mac_t intf_in_mac_address = cint_dnx_ip_tunnel_basic_info.intf_in_mac_address;     /* my-MAC */
    int vrf2 = cint_dnx_ip_tunnel_basic_info.in_vrf2;
    int fec = 0;
    bcm_ip_t dip_ipv4        = 0xA1000011; /* 161.0.0.17 */
    bcm_ip_t mask_ipv4       = 0xFFFFFF00; /* 255.255.255.0 */
    char *proc_name;
    proc_name = "dnx_ip_tunnel_term_ip4_with_faulty_port";

    if (with_subnet_mask==1) {
        rv = dnx_ip_tunnel_term_ip4(unit,in_port,out_port);
    } else {
        rv = dnx_ip_tunnel_term_ip4_with_subnet_mask(unit,in_port,out_port);
    }
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_term_ip4 \n", proc_name);
        return rv;
    }

    /*
     * 2. Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, in_port_faulty, intf_in2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set intf_in\n",proc_name);
        return rv;
    }
    /*
     * 3. Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, intf_in2, intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in %d\n",proc_name,intf_in);
        return rv;
    }

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = vrf2;
    ingress_rif.intf_id = intf_in2;
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    /*
     * 5. Add Route entry
     */
    fec = cint_dnx_ip_tunnel_basic_info.tunnel_fec_id;
    printf("%s(): Add route entry.  route 0x%08X mask 0x%08X vrf %d fec 0x%08X. \n",proc_name, dip_ipv4, mask_ipv4, vrf2, fec);
    rv = add_route_ipv4(unit, dip_ipv4, mask_ipv4, vrf2, fec);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, add_route_ipv4 \n",proc_name);
        return rv;
    }
    return rv;
}

/*
 * function for UDP termination scenarios
 * Inputs:
 * unit - relevant unit;
 * in_port - incoming port;
 * out_port - outgoing port;
 */
int
dnx_ip_tunnel_term_udp4(
    int unit,
    int in_port,
    int out_port)
{
    int rv;
    bcm_tunnel_terminator_t tunnel_term;
    bcm_if_t tunnel_intf=0;
    char *proc_name;
    proc_name = "dnx_ip_tunnel_term_udp4";

    dnx_ip_tunnel_encap=0;

    rv = dnx_ip_tunnel_fwd_configre(unit,in_port,out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_fwd_configre \n", proc_name);
        return rv;
    }

    rv = dnx_ip_tunnel_fec_configre(unit,out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_fwd_configre \n", proc_name);
        return rv;
    }

    /* Create IP tunnel terminator for GRE4oIPv4 tunnel header*/
    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.dip        = 0xA1000011; /* 161.0.0.17 */
    tunnel_term.sip        = 0xA0000011; /* 160.0.0.17 */
    tunnel_term.dip_mask   = cint_dnx_ip_tunnel_basic_info.term_dip_mask;
    tunnel_term.sip_mask   = cint_dnx_ip_tunnel_basic_info.term_sip_mask;
    tunnel_term.flags      = BCM_TUNNEL_INIT_STAT_ENABLE ;
    tunnel_term.vrf        = cint_dnx_ip_tunnel_basic_info.in_vrf;
    tunnel_term.type       = bcmTunnelTypeUdp;
    rv = dnx_ip_tunnel_terminator_create(unit, tunnel_term);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_terminator_create \n", proc_name);
        return rv;
    }
    return rv;
}

/*
 * function for GRE4/IP4 tunnel termination scenarios
 * Inputs:
 * unit - relevant unit;
 * in_port - incoming port;
 * out_port - outgoing port;
 * tunnel_type - bcmTunnelTypeGreAnyIn4 or bcmTunnelTypeIpAnyIn4;
 * type - 1:subnet  2:priority  other:full mask;
 */
int
dnx_ip_tunnel_term_ip4_gre4(
    int unit,
    int in_port,
    int out_port,
    int tunnel_type,
    int type)
{
    int rv;
    bcm_tunnel_terminator_t tunnel_term;
    bcm_if_t tunnel_intf=0;
    char *proc_name;
    proc_name = "dnx_ip_tunnel_term_ip4_gre4";
    int fec =0;
    int vrf =0;

    bcm_ip_t dip1_mask = 0xffffffff;
    bcm_ip_t sip1_mask = 0xffffffff;
    bcm_ip_t dip2_mask = 0xffffffff;
    bcm_ip_t sip2_mask = 0xffffffff;
    bcm_ip_t dip3_mask = 0xffffffff;
    bcm_ip_t sip3_mask = 0xffffffff;
    /* DIP, SIP with subnet */
    if (type ==1){
        dip1_mask = 0xffffff00;
        sip1_mask = 0xffffff00;
        dip2_mask = 0xfffff000;
        sip2_mask = 0xfffff000;
        dip3_mask = 0xffff0000;
        sip3_mask = 0xffff0000;
    }
    if (type ==2) {
        /* tunnel 1: DIP only */
        dip1_mask = 0xffffffff;
        sip1_mask = 0x00000000;
        /* tunnel 2: DIP, SIP */
        dip2_mask = 0xffffffff;
        sip2_mask = 0xffffffff;
        /* tunnel 3: DIP, SIP with mask */
        dip3_mask = 0xffffff00;
        sip3_mask = 0xffffff00;
    }

    dnx_ip_tunnel_encap=0;

    rv = dnx_ip_tunnel_fwd_configre(unit,in_port,out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_fwd_configre \n", proc_name);
        return rv;
    }

    /*
     * Add Route entry for 191.15.16.17
     */
    fec =cint_dnx_ip_tunnel_basic_info.tunnel_fec_id;
    vrf =cint_dnx_ip_tunnel_basic_info.in_vrf;
    printf("%s(): Add route entry.  route 191.15.16.17 mask 255.255.255.0 vrf %d fec 0x%08X. \n",proc_name, vrf, fec);
    rv = add_route_ipv4(unit, 0xBF0F1011, 0xffffff00, vrf, fec);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, add_route_ipv4 \n",proc_name);
        return rv;
    }

    rv = dnx_ip_tunnel_fec_configre(unit,out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_fwd_configre \n", proc_name);
        return rv;
    }

    /* tunnel 1 */
    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.dip        = 0xA10F1011; /* 161.15.16.17 */
    tunnel_term.sip        = 0xA00F1011; /* 160.15.16.17 */
    tunnel_term.dip_mask   = dip1_mask;
    tunnel_term.sip_mask   = sip1_mask;
    tunnel_term.flags      = BCM_TUNNEL_INIT_STAT_ENABLE ;
    tunnel_term.vrf        = cint_dnx_ip_tunnel_basic_info.in_vrf;
    tunnel_term.type       = tunnel_type;
    rv = dnx_ip_tunnel_terminator_create(unit, tunnel_term);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_terminator_create \n", proc_name);
        return rv;
    }

    /* tunnel 2 */
    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.dip        = 0xAB0F1011; /* 171.15.16.17 */
    tunnel_term.sip        = 0xAA0F1011; /* 170.15.16.17 */
    tunnel_term.dip_mask   = dip2_mask;
    tunnel_term.sip_mask   = sip2_mask;
    tunnel_term.flags      = BCM_TUNNEL_INIT_STAT_ENABLE ;
    tunnel_term.vrf        = cint_dnx_ip_tunnel_basic_info.in_vrf;
    tunnel_term.type       = tunnel_type;
    rv = dnx_ip_tunnel_terminator_create(unit, tunnel_term);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_terminator_create \n", proc_name);
        return rv;
    }

    /* tunnel 3 */
    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.dip        = 0xB50F1011; /* 181.15.16.17 */
    tunnel_term.sip        = 0xB40F1011; /* 180.15.16.17 */
    tunnel_term.dip_mask   = dip3_mask;
    tunnel_term.sip_mask   = sip3_mask;
    tunnel_term.flags      = BCM_TUNNEL_INIT_STAT_ENABLE ;
    tunnel_term.vrf        = cint_dnx_ip_tunnel_basic_info.in_vrf;
    tunnel_term.type       = tunnel_type;
    rv = dnx_ip_tunnel_terminator_create(unit, tunnel_term);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_terminator_create \n", proc_name);
        return rv;
    }

    return rv;
}

int dnx_ip_tunnel_term_sip_dip_ip4_ip6_ipany_in4 (int unit, int provider_port, int access_port) {
    return dnx_ip_tunnel_term_ip4_gre4(unit, provider_port, access_port,bcmTunnelTypeIpAnyIn4,0);
}

int dnx_ip_tunnel_term_sip_dip_gre4_gre6_greany_in4 (int unit, int provider_port, int access_port) {
    return dnx_ip_tunnel_term_ip4_gre4(unit, provider_port, access_port,bcmTunnelTypeGreAnyIn4,0);
}

int dnx_ip_tunnel_term_sip_dip_ip4_ip6_ipany_in4_subnet (int unit, int provider_port, int access_port) {
    return dnx_ip_tunnel_term_ip4_gre4(unit, provider_port, access_port,bcmTunnelTypeIpAnyIn4,1);
}

int dnx_ip_tunnel_term_sip_dip_gre4_gre6_greany_in4_subnet (int unit, int provider_port, int access_port) {
    return dnx_ip_tunnel_term_ip4_gre4(unit, provider_port, access_port,bcmTunnelTypeGreAnyIn4,1);
}

int dnx_ip_tunnel_term_sip_dip_ip4_ip6_ipany_in4_priority (int unit, int provider_port, int access_port) {
    return dnx_ip_tunnel_term_ip4_gre4(unit, provider_port, access_port,bcmTunnelTypeIpAnyIn4,2);
}

int dnx_ip_tunnel_term_sip_dip_gre4_gre6_greany_in4_priority (int unit, int provider_port, int access_port) {
    return dnx_ip_tunnel_term_ip4_gre4(unit, provider_port, access_port,bcmTunnelTypeGreAnyIn4,2);
}

/*
 * function for GRE4/IP4 tunnel encapsulation scenarios, with Uniform and Pipe model
 * Inputs:
 * unit - relevant unit;
 * in_port - incoming port;
 * out_port - outgoing port;
 */
int
dnx_ip_tunnel_encap_ip4_gre4(
    int unit,
    int in_port,
    int out_port)
{
    int rv;
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t l3_intf;
    bcm_if_t tunnel_intf=0;
    bcm_gport_t gport;
    int encoded_fec = 0;
    int encap_id = 0;
    int intf_out = cint_dnx_ip_tunnel_basic_info.eth_rif_intf_out;
    int fec = 0;
    int vrf = cint_dnx_ip_tunnel_basic_info.in_vrf;
    bcm_ip_t  host_ipv4_dip = 0x7fffff03;
    bcm_ip6_t host_ipv6_dip = {0x20, 0x00, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6,0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae};
    char *proc_name;
    proc_name = "dnx_ip_tunnel_encap_ip4_gre4";

    /*Step1: configure IPV4 tunnel encapsulation*/
    rv = dnx_ip_tunnel_fwd_configre(unit,in_port,out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_fwd_configre \n", proc_name);
        return rv;
    }

    /* Create IP tunnel initiator for encapsulating GRE4oIPv4 tunnel header*/
    bcm_tunnel_initiator_t_init(&tunnel_init);
    tunnel_init.dip        = 0xA1000011; /* 161.0.0.17 */
    tunnel_init.sip        = 0xA0000011; /* 160.0.0.17 */
    tunnel_init.flags      = BCM_TUNNEL_INIT_STAT_ENABLE | BCM_TUNNEL_TERM_USE_OUTER_TTL ;
    tunnel_init.dscp_sel   = bcmTunnelDscpPacket;
    tunnel_init.dscp       = 0;
    tunnel_init.type       = bcmTunnelTypeGreAnyIn4;
    tunnel_init.ttl        = 0;
    tunnel_init.l3_intf_id = cint_dnx_ip_tunnel_basic_info.arp_id ;
    tunnel_init.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    tunnel_init.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    tunnel_init.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE; /* default: -1 */

    rv = dnx_ip_tunnel_initiator_create(unit, tunnel_init, &tunnel_intf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_initiator_create \n", proc_name);
        return rv;
    }

    rv = dnx_ip_tunnel_fec_configre(unit,out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_fwd_configre \n", proc_name);
        return rv;
    }

    /*Step2: configure GRE4 encapsulation*/
    bcm_tunnel_initiator_t_init(&tunnel_init);
    tunnel_init.dip        = 0xAB0F1011; /* 171.15.16.17 */
    tunnel_init.sip        = 0xAA0F1011; /* 170.15.16.17 */
    tunnel_init.flags      = BCM_TUNNEL_INIT_STAT_ENABLE ;
    tunnel_init.dscp_sel   = bcmTunnelDscpAssign;
    tunnel_init.dscp       = 5;
    tunnel_init.type       = bcmTunnelTypeIpAnyIn4;
    tunnel_init.ttl        = 64;
    tunnel_init.l3_intf_id = cint_dnx_ip_tunnel_basic_info.arp_id ;
    tunnel_init.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    tunnel_init.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    tunnel_init.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE; /* default: -1 */
    bcm_l3_intf_t_init(&l3_intf);
    rv = bcm_tunnel_initiator_create(unit,&l3_intf, tunnel_init);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_initiator_create \n");
    }
    encap_id=tunnel_init.tunnel_id;

    /* Create FEc for encapsulating IPV4 tunnel header*/
    printf("%s():. Create main FEC and set its properties.\r\n",proc_name);
    if (BCM_GPORT_IS_SET(out_port)) {
       gport = out_port;
    }
    else {
       BCM_GPORT_LOCAL_SET(gport, out_port);
    }

    int fec_flags2 = 0;
    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    fec = cint_dnx_ip_tunnel_basic_info.tunnel_fec_id + 2;
    rv = l3__egress_only_fec__create_inner(unit, fec, intf_out, encap_id, gport, 0,
                                           fec_flags2, 0, 0,&encoded_fec);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC only\n",proc_name);
        return rv;
    }
    printf("%s(): fec 0x%08X encoded_fec 0x%08X.\r\n",proc_name, fec, encoded_fec);


    /*
     * Add Route entry for encapsulating IPV4 tunnel header
     */
    printf("%s(): Add host entry.  host 0x%08X vrf %d fec 0x%08X. \n",proc_name, host_ipv4_dip, vrf, fec);
    rv = add_host_ipv4(unit, host_ipv4_dip, vrf, fec,0,0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, add_host_ipv4 \n",proc_name);
        return rv;
    }

    printf("%s(): Add IPV6 host entry. \n",proc_name);
    rv = add_host_ipv6(unit, host_ipv6_dip, vrf, fec);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, add_host_ipv6 \n",proc_name);
        return rv;
    }
    return rv;
}

/*
 * function for IP6 tunnel encapsulation scenarios
 * Inputs:
 * unit - relevant unit;
 * in_port - incoming port;
 * out_port - outgoing port;
 * is_ip6 - 1:IPV6 tunnel, 0:GRE6 tunnel ;
 */
int
dnx_ip_tunnel_encap_ip6_or_gre6(
    int unit,
    int in_port,
    int out_port,
    int is_ip6)
{
    int rv;
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t l3_intf;
    bcm_if_t tunnel_intf=0;
    bcm_gport_t gport;
    int encoded_fec = 0;
    int encap_id = 0;
    int intf_out = cint_dnx_ip_tunnel_basic_info.eth_rif_intf_out;
    int fec = 0;
    int vrf = cint_dnx_ip_tunnel_basic_info.in_vrf;
    bcm_ip_t  host_ipv4_dip = 0x7fffff03;
    bcm_ip6_t host_ipv6_dip = {0x20, 0x00, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6,0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae};
    bcm_ip6_t dip6_1 = {0x20, 0x00, 0xee, 0xed, 0xec, 0xeb, 0xea, 0xe9,0xe8, 0xe7, 0xe6, 0xe5, 0xe4, 0xe3, 0xe2, 0xe1};
    bcm_ip6_t sip6_1 = {0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x03, 0x00, 0x77, 0xff, 0xee, 0xed, 0xec, 0xeb};
    bcm_ip6_t dip6_2 = {0x20, 0x00, 0xfe, 0xfd, 0xfc, 0xfb, 0xfa, 0xf9,0xf8, 0xf7, 0xf6, 0xf5, 0xf4, 0xf3, 0xf2, 0xf1};
    bcm_ip6_t sip6_2 = {0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x02, 0x00, 0x77, 0xff, 0xfe, 0xfd, 0xfc, 0xfb};

    char *proc_name;
    proc_name = "dnx_ip_tunnel_encap_ip6_or_gre6";
    int type =bcmTunnelTypeGreAnyIn6;

    if (is_ip6==1){
        type=bcmTunnelTypeIpAnyIn6;
    }

    /*Step1: configure IPV6/GRE6 encapsulation for route forwarding*/
    rv = dnx_ip_tunnel_fwd_configre(unit,in_port,out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_fwd_configre \n", proc_name);
        return rv;
    }

    /* Create IP tunnel initiator for encapsulating IPV6/GRE6 tunnel header*/
    bcm_tunnel_initiator_t_init(&tunnel_init);
    sal_memcpy(&(tunnel_init.dip6),&dip6_1,16);
    sal_memcpy(&(tunnel_init.sip6),&sip6_1,16);
    tunnel_init.flags      = BCM_TUNNEL_INIT_STAT_ENABLE ;
    tunnel_init.dscp_sel   = bcmTunnelDscpAssign;
    tunnel_init.dscp       = 10;
    tunnel_init.type       = type;
    tunnel_init.encap_access  = bcmEncapAccessTunnel2;
    tunnel_init.ttl        = 40;
    tunnel_init.l3_intf_id = cint_dnx_ip_tunnel_basic_info.arp_id ;
    tunnel_init.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    tunnel_init.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    tunnel_init.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE; /* default: -1 */
    rv = dnx_ip_tunnel_initiator_create(unit, tunnel_init, &tunnel_intf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_initiator_create \n", proc_name);
        return rv;
    }

    rv = dnx_ip_tunnel_fec_configre(unit,out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_fwd_configre \n", proc_name);
        return rv;
    }

    /*Step2: configure IPV6/GRE6 encapsulation for host forwarding*/
    bcm_tunnel_initiator_t_init(&tunnel_init);
    sal_memcpy(&(tunnel_init.dip6),&dip6_2,16);
    sal_memcpy(&(tunnel_init.sip6),&sip6_2,16);
    tunnel_init.flags      = BCM_TUNNEL_INIT_STAT_ENABLE ;
    tunnel_init.dscp_sel   = bcmTunnelDscpAssign;
    tunnel_init.dscp       = 5;
    tunnel_init.type       = type;
    tunnel_init.encap_access  = bcmEncapAccessTunnel2;
    tunnel_init.ttl        = 64;
    tunnel_init.l3_intf_id = cint_dnx_ip_tunnel_basic_info.arp_id ;
    tunnel_init.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    tunnel_init.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    tunnel_init.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE; /* default: -1 */
    bcm_l3_intf_t_init(&l3_intf);
    rv = bcm_tunnel_initiator_create(unit,&l3_intf, tunnel_init);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_initiator_create \n");
    }
    encap_id=tunnel_init.tunnel_id;
    printf("%s(): Create IP tunnel: 0x%08X.\n",proc_name, l3_intf.l3a_intf_id);

    /* Create FEc for encapsulating IPV4 tunnel header*/
    printf("%s():. Create main FEC and set its properties.\r\n",proc_name);
    if (BCM_GPORT_IS_SET(out_port)) {
       gport = out_port;
    }
    else {
       BCM_GPORT_LOCAL_SET(gport, out_port);
    }

    int fec_flags2 = 0;
    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    fec = cint_dnx_ip_tunnel_basic_info.tunnel_fec_id + 2;
    rv = l3__egress_only_fec__create_inner(unit, fec, intf_out, encap_id, gport, 0,
                                           fec_flags2, 0, 0,&encoded_fec);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC only\n",proc_name);
        return rv;
    }
    printf("%s(): fec 0x%08X encoded_fec 0x%08X.\r\n",proc_name, fec, encoded_fec);


    /*
     * Add host entry for encapsulating IPV6/GRE6 tunnel header
     */
    printf("%s(): Add host entry.  host 0x%08X vrf %d fec 0x%08X. \n",proc_name, host_ipv4_dip, vrf, fec);
    rv = add_host_ipv4(unit, host_ipv4_dip, vrf, fec,0,0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, add_host_ipv4 \n",proc_name);
        return rv;
    }

    printf("%s(): Add IPV6 host entry. \n",proc_name);
    rv = add_host_ipv6(unit, host_ipv6_dip, vrf, fec);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, add_host_ipv6 \n",proc_name);
        return rv;
    }
    return rv;
}

/*
 * create ipv6 tunnel ingress entry
 * type: 0-MP; 1-P2P; 2-P2P with MP defined
 */
int dnx_ipv6_tunnel_terminator_create(int unit, int type, bcm_tunnel_terminator_t tunnel_term)
{

    int rv;
    int tunnel_class = 11;
    int tunnel_id =0;
    bcm_tunnel_terminator_t tunnel_termination_obj;
    bcm_tunnel_terminator_config_action_t p2p_tunnel_terminator_config_action;
    bcm_tunnel_terminator_config_key_t p2p_tunnel_terminator_config_key;

    /*
     * Check if the user wants to create P2P tunnel termination
     */
    if (type == 1) {
        /*P2P, 2 consecutive lookups*/
        p2p_tunnel_terminator_config_action.tunnel_class = tunnel_class;
        bcm_tunnel_terminator_config_key_t_init(&p2p_tunnel_terminator_config_key);
        sal_memcpy(p2p_tunnel_terminator_config_key.dip6, tunnel_term.dip6, 16);
        sal_memcpy(p2p_tunnel_terminator_config_key.dip6_mask, tunnel_term.dip6_mask, 16);
        p2p_tunnel_terminator_config_key.flags = tunnel_term.flags;
        p2p_tunnel_terminator_config_key.type = tunnel_term.type;
        p2p_tunnel_terminator_config_key.vrf = tunnel_term.vrf;

        rv = bcm_tunnel_terminator_config_add(unit, 0, p2p_tunnel_terminator_config_key, p2p_tunnel_terminator_config_action);
        if (rv != BCM_E_NONE) {
            printf("Error: dnx_ipv6_tunnel_terminator_create failed to call bcm_tunnel_terminator_config_add.\n");
        }

        tunnel_term.flags |=  BCM_TUNNEL_TERM_USE_TUNNEL_CLASS;
        tunnel_term.tunnel_class = tunnel_class;
    } else if (type ==2) {
        /* {DIP, VRF, Tunnel type} -> IP-Tunnel ID, for P2P and MP together */
        bcm_tunnel_terminator_t_init(&tunnel_termination_obj);
        sal_memcpy(tunnel_termination_obj.dip6, tunnel_term.dip6, 16);
        sal_memcpy(tunnel_termination_obj.dip6_mask, tunnel_term.dip6_mask, 16);
        tunnel_termination_obj.flags = tunnel_term.flags;
        tunnel_termination_obj.type = tunnel_term.type;
        tunnel_termination_obj.vrf = tunnel_term.vrf;
        rv = dnx_ip_tunnel_terminator_create(unit, tunnel_termination_obj);
        if (rv != BCM_E_NONE)
        {
            printf("Error: dnx_ipv6_tunnel_terminator_create failed to create tunnel termination object firstly. \n");
            return rv;
        }

        /* {SIP, IP-Tunnel ID} -> IP-Tunnel In-LIF, for P2P */
        tunnel_term.flags |=  BCM_TUNNEL_TERM_USE_TUNNEL_CLASS;
        tunnel_term.tunnel_class = cint_dnx_ip_tunnel_basic_info.tunnel_term_id;
    }

    /* Call the actual API method for tunnel termination creation */
    rv = dnx_ip_tunnel_terminator_create(unit, tunnel_term);
    if (rv != BCM_E_NONE)
    {
        printf("Error: dnx_ipv6_tunnel_terminator_create failed to create tunnel termination object. \n");
        return rv;
    }
    return rv;
}

/*
 * function for IPV6 tunnel termination scenarios
 * Inputs:
 * unit - relevant unit;
 * in_port - incoming port;
 * out_port - outgoing port;
 */
int
dnx_ipv6_tunnel_term_same_dip_vrf(
    int unit,
    int in_port,
    int out_port)
{
    int rv;
    bcm_tunnel_terminator_t tunnel_term;
    bcm_if_t tunnel_intf=0;
    char *proc_name;
    bcm_ip6_t sip6 = {0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x03, 0x00, 0x77, 0xff, 0xee, 0xed, 0xec, 0xeb};
    bcm_ip6_t dip6 = {0x20, 0x00, 0xfe, 0xfd, 0xfc, 0xfb, 0xfa, 0xf9,0xf8, 0xf7, 0xf6, 0xf5, 0xf4, 0xf3, 0xf2, 0xf1};
    bcm_ip6_t mask6 = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    proc_name = "dnx_ipv6_tunnel_term_same_dip_vrf";

    dnx_ip_tunnel_encap=0;

    rv = dnx_ip_tunnel_fwd_configre(unit,in_port,out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_fwd_configre \n", proc_name);
        return rv;
    }

    rv = dnx_ip_tunnel_fec_configre(unit,out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_fwd_configre \n", proc_name);
        return rv;
    }

    /* Create IP tunnel terminator for IPV6 tunnel header*/
    bcm_tunnel_terminator_t_init(&tunnel_term);
    sal_memcpy(&(tunnel_term.dip6),&dip6,16);
    sal_memcpy(&(tunnel_term.sip6),&sip6,16);
    sal_memcpy(&(tunnel_term.dip6_mask),&mask6,16);
    sal_memcpy(&(tunnel_term.sip6_mask),&mask6,16);
    tunnel_term.flags      = BCM_TUNNEL_INIT_STAT_ENABLE ;
    tunnel_term.vrf        = cint_dnx_ip_tunnel_basic_info.in_vrf;
    tunnel_term.type       = bcmTunnelTypeIpAnyIn6;
    rv = dnx_ipv6_tunnel_terminator_create(unit, 2, tunnel_term);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ipv6_tunnel_terminator_create \n", proc_name);
        return rv;
    }
    return rv;
}

/*
 * function for IPV6 and GREoIPV6 tunnel termination scenarios
 * Inputs:
 * unit - relevant unit;
 * in_port - incoming port;
 * out_port - outgoing port;
 */
int
dnx_ipv6_tunnel_term_ipany_and_greany(
    int unit,
    int in_port,
    int out_port)
{
    int rv;
    bcm_tunnel_terminator_t tunnel_term;
    bcm_if_t tunnel_intf=0;
    char *proc_name;
    bcm_ip6_t sip6 = {0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x02, 0x00, 0x77, 0xff, 0xfe, 0xfd, 0xfc, 0xfb};
    bcm_ip6_t dip6 = {0x20, 0x00, 0xfe, 0xfd, 0xfc, 0xfb, 0xfa, 0xf9,0xf8, 0xf7, 0xf6, 0xf5, 0xf4, 0xf3, 0xf2, 0xf1};
    bcm_ip6_t mask6 = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    bcm_ip6_t sip6_2 = {0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x03, 0x00, 0x77, 0xff, 0xee, 0xed, 0xec, 0xeb};
    bcm_ip6_t dip6_2 = {0x20, 0x00, 0xee, 0xed, 0xec, 0xeb, 0xea, 0xe9,0xe8, 0xe7, 0xe6, 0xe5, 0xe4, 0xe3, 0xe2, 0xe1};
    bcm_ip6_t mask6_2 = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    proc_name = "dnx_ipv6_tunnel_term_ipany_and_greany";

    dnx_ip_tunnel_encap=0;

    rv = dnx_ip_tunnel_fwd_configre(unit,in_port,out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_fwd_configre \n", proc_name);
        return rv;
    }

    rv = dnx_ip_tunnel_fec_configre(unit,out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_fwd_configre \n", proc_name);
        return rv;
    }

    /* Create IPV6 tunnel terminator*/
    bcm_tunnel_terminator_t_init(&tunnel_term);
    sal_memcpy(&(tunnel_term.dip6),&dip6,16);
    sal_memcpy(&(tunnel_term.sip6),&sip6,16);
    sal_memcpy(&(tunnel_term.dip6_mask),&mask6,16);
    sal_memcpy(&(tunnel_term.sip6_mask),&mask6,16);
    tunnel_term.flags      = BCM_TUNNEL_INIT_STAT_ENABLE ;
    tunnel_term.vrf        = cint_dnx_ip_tunnel_basic_info.in_vrf;
    tunnel_term.type       = bcmTunnelTypeIpAnyIn6;
    rv = dnx_ipv6_tunnel_terminator_create(unit, 0, tunnel_term);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ipv6_tunnel_terminator_create \n", proc_name);
        return rv;
    }

    /* Create GREoIPV6 tunnel terminator*/
    bcm_tunnel_terminator_t_init(&tunnel_term);
    sal_memcpy(&(tunnel_term.dip6),&dip6_2,16);
    sal_memcpy(&(tunnel_term.sip6),&sip6_2,16);
    sal_memcpy(&(tunnel_term.dip6_mask),&mask6_2,16);
    sal_memcpy(&(tunnel_term.sip6_mask),&mask6_2,16);
    tunnel_term.flags      = BCM_TUNNEL_INIT_STAT_ENABLE ;
    tunnel_term.vrf        = cint_dnx_ip_tunnel_basic_info.in_vrf;
    tunnel_term.type       = bcmTunnelTypeGreAnyIn6;
    rv = dnx_ipv6_tunnel_terminator_create(unit, 0, tunnel_term);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ipv6_tunnel_terminator_create \n", proc_name);
        return rv;
    }
    return rv;
}

/*
 * function for IPV6 tunnel termination scenarios
 * Inputs:
 * unit - relevant unit;
 * in_port - incoming port;
 * out_port - outgoing port;
 * tunnel_type - bcmTunnelTypeGreAnyIn6 or bcmTunnelTypeIpAnyIn6;
 * is_cascade - 1:P2P  other:MP;
 * is_subnet -  1:subnet  other:full mask;
 */
int
dnx_ipv6_tunnel_term_ip6_gre6(
    int unit,
    int in_port,
    int out_port,
    int tunnel_type,
    int is_cascade,
    int is_subnet)
{
    int rv;
    bcm_tunnel_terminator_t tunnel_term;
    bcm_if_t tunnel_intf=0;
    char *proc_name;
    bcm_ip6_t sip6 = {0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x03, 0x00, 0x77, 0xff, 0xee, 0xed, 0xec, 0xeb};
    bcm_ip6_t dip6 = {0x20, 0x00, 0xfe, 0xfd, 0xfc, 0xfb, 0xfa, 0xf9,0xf8, 0xf7, 0xf6, 0xf5, 0xf4, 0xf3, 0xf2, 0xf1};
    bcm_ip6_t mask6 = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    bcm_ip6_t mask6_subnet = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00};
    int type =0;

    proc_name = "dnx_ipv6_tunnel_term_ip6_gre6";

    dnx_ip_tunnel_encap=0;

    rv = dnx_ip_tunnel_fwd_configre(unit,in_port,out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_fwd_configre \n", proc_name);
        return rv;
    }

    rv = dnx_ip_tunnel_fec_configre(unit,out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_fwd_configre \n", proc_name);
        return rv;
    }

    /* Create IP tunnel terminator for IPV6 tunnel header*/
    if (is_cascade ==1) {
        type =1;
    }
    bcm_tunnel_terminator_t_init(&tunnel_term);
    sal_memcpy(&(tunnel_term.dip6),&dip6,16);
    sal_memcpy(&(tunnel_term.sip6),&sip6,16);
    if (is_subnet ==1) {
        sal_memcpy(&(tunnel_term.dip6_mask),&mask6_subnet,16);
    } else {
        sal_memcpy(&(tunnel_term.dip6_mask),&mask6,16);
    }
    sal_memcpy(&(tunnel_term.sip6_mask),&mask6,16);
    tunnel_term.flags      = BCM_TUNNEL_INIT_STAT_ENABLE ;
    tunnel_term.vrf        = cint_dnx_ip_tunnel_basic_info.in_vrf;
    tunnel_term.type       = tunnel_type;
    rv = dnx_ipv6_tunnel_terminator_create(unit, type, tunnel_term);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ipv6_tunnel_terminator_create \n", proc_name);
        return rv;
    }
    return rv;
}

/*
 * function for two ipv6 tunnel terminators
 * Inputs:
 * unit - relevant unit;
 * in_port - incoming port;
 * out_port - outgoing port;
 */
int
dnx_ipv6_tunnel_term_ipany_and_ipany(
    int unit,
    int in_port,
    int out_port)
{
    int rv;
    bcm_tunnel_terminator_t tunnel_term;
    bcm_if_t tunnel_intf=0;
    char *proc_name;
    bcm_ip6_t sip6 = {0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x02, 0x00, 0x77, 0xff, 0xfe, 0xfd, 0xfc, 0xfb};
    bcm_ip6_t dip6 = {0x20, 0x00, 0xfe, 0xfd, 0xfc, 0xfb, 0xfa, 0xf9,0xf8, 0xf7, 0xf6, 0xf5, 0xf4, 0xf3, 0xf2, 0xf1};
    bcm_ip6_t mask6 = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    bcm_ip6_t sip6_2 = {0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x03, 0x00, 0x77, 0xff, 0xee, 0xed, 0xec, 0xeb};
    bcm_ip6_t dip6_2 = {0x20, 0x00, 0xee, 0xed, 0xec, 0xeb, 0xea, 0xe9,0xe8, 0xe7, 0xe6, 0xe5, 0xe4, 0xe3, 0xe2, 0xe1};
    bcm_ip6_t mask6_2 = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    proc_name = "dnx_ipv6_tunnel_term_ipany_and_ipany";

    dnx_ip_tunnel_encap=0;

    rv = dnx_ip_tunnel_fwd_configre(unit,in_port,out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_fwd_configre \n", proc_name);
        return rv;
    }

    rv = dnx_ip_tunnel_fec_configre(unit,out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ip_tunnel_fwd_configre \n", proc_name);
        return rv;
    }

    /* Create the first IPV6 tunnel terminator*/
    bcm_tunnel_terminator_t_init(&tunnel_term);
    sal_memcpy(&(tunnel_term.dip6),&dip6,16);
    sal_memcpy(&(tunnel_term.sip6),&sip6,16);
    sal_memcpy(&(tunnel_term.dip6_mask),&mask6,16);
    sal_memcpy(&(tunnel_term.sip6_mask),&mask6,16);
    tunnel_term.flags      = BCM_TUNNEL_INIT_STAT_ENABLE ;
    tunnel_term.vrf        = cint_dnx_ip_tunnel_basic_info.in_vrf;
    tunnel_term.type       = bcmTunnelTypeIpAnyIn6;
    rv = dnx_ipv6_tunnel_terminator_create(unit, 0, tunnel_term);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ipv6_tunnel_terminator_create \n", proc_name);
        return rv;
    }

    /* Create the second IPV6 tunnel terminator*/
    bcm_tunnel_terminator_t_init(&tunnel_term);
    sal_memcpy(&(tunnel_term.dip6),&dip6_2,16);
    sal_memcpy(&(tunnel_term.sip6),&sip6_2,16);
    sal_memcpy(&(tunnel_term.dip6_mask),&mask6_2,16);
    sal_memcpy(&(tunnel_term.sip6_mask),&mask6_2,16);
    tunnel_term.flags      = BCM_TUNNEL_INIT_STAT_ENABLE ;
    tunnel_term.vrf        = cint_dnx_ip_tunnel_basic_info.in_vrf;
    tunnel_term.type       = bcmTunnelTypeIpAnyIn6;
    rv = dnx_ipv6_tunnel_terminator_create(unit, 0, tunnel_term);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function dnx_ipv6_tunnel_terminator_create \n", proc_name);
        return rv;
    }
    return rv;
}

