/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 *File: cint_dnx_l2tp.c
 * Purpose: An example of L2TPv2 encapsulation and termination
 *
 * 1. Example of L2tp MP encapsulation
 * 2. Example of L2tp MP termincation
 * 3. Example of L2tp to PPPoE bidirection cross connect
 *
 * 1. Example of L2tp MP encapsulation
 *
 * Set up sequence:
 *    cint ../../../src/examples/sand/utility/cint_sand_utils_global.c
 *    cint ../../../src/examples/dnx/field/cint_field_utils.c
 *    cint ../../../src/examples/sand/cint_ip_route_basic.c
 *    cint ../../../src/examples/dnx/ppp/cint_dnx_l2tp.c
 *    cint
 *    l2tp_encapsulation(0, 201, 203); 
 *    exit;
 * Run traffic:
 *    tx 1 psrc=203 DATA=0x001100000044000000000033810000650800450000350000000080002cc47fffff058fffff05000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Expected:
 *     PPPoL2tpv2oUDPoIpv4oETH is generated
 *
 * 2. Example of L2tp MP termination
 *
 * Set up sequence:
 *    cint ../../../src/examples/sand/utility/cint_sand_utils_global.c
 *    cint ../../../src/examples/dnx/field/cint_field_utils.c
 *    cint ../../../src/examples/sand/cint_ip_route_basic.c
 *    cint ../../../src/examples/dnx/ppp/cint_dnx_l2tp.c
 *    cint
 *    l2tp_termination(0, 201, 203, 1, 0);
 *    exit;
 * Run traffic:
 *    tx 1  PtchSRCport=201 DATA=0x0011000000330000000000448100001208004500005b0000000000116390ac000001ab00000106a506a500470000000222223333ff030021450000350000000080004cc58fffff057fffff05000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Expected:
 *     PPPoL2tpv2oUDPoIpv4oETH is terminated
 *
 * 3. Example of L2tp to PPPoE P2P cross connect
 *
 * Set up sequence:
 *    cint ../../../src/examples/sand/utility/cint_sand_utils_global.c
 *    cint ../../../src/examples/dnx/field/cint_field_utils.c
 *    cint ../../../src/examples/sand/cint_ip_route_basic.c
 *    cint ../../../src/examples/dnx/ppp/cint_dnx_pppoe.c
 *    cint ../../../src/examples/dnx/ppp/cint_dnx_l2tp.c
 *    cint
 *    pppoe_2_l2tp_bidirection_cross_connect(0, 201, 203);
 *    exit;
 * L2TPv2 -> PPPoE
 * Run traffic:
 *    L2tp without length optional header
 *    tx 1  PtchSRCport=203 DATA=0x0011000000330000000000448100001208004500005b0000000000116390ac000001ab00000106a506a500470000000222223333ff030021450000350000000080004cc58fffff057fffff05000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *    L2tp with length optional header
 *    tx 1  PtchSRCport=203 DATA=0x0011000000330000000000448100001208004500005b0000000000116390ac000001ab00000106a506a5004700004002000022223333ff030021450000350000000080004cc58fffff057fffff05000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Expected:
 *     PPPoL2tpv2oUDPoIpv4oETH is terminated
 *     PPPoPPPoEoETH is generated
 *
 * PPPoE -> L2TPv2
 * Run traffic:
 *    tx 1  PtchSRCport=201 DATA=0x0011000000110000070001008100001188641100555500200021450000350000000080004cc56fffff017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Expected:
 *     PPPoPPPoEoETH is terminated
 *     PPPoL2tpv2oUDPoIpv4oETH is generated
 *
 * 4. Example of L2tp MP termination with session spoofing
 *
 * Set up sequence:
 *    cint ../../../src/examples/sand/utility/cint_sand_utils_global.c
 *    cint ../../../src/examples/dnx/field/cint_field_utils.c
 *    cint ../../../src/examples/sand/cint_ip_route_basic.c
 *    cint ../../../src/examples/dnx/ppp/cint_dnx_l2tp.c
 *    cint
 *    l2tp_termination_session_spoofing_add(0, 201, 203);
 *    exit;
 * Run traffic:
 *    tx 1  PtchSRCport=201 DATA=0x0011000000330000000000448100001208004500005b0000000000116390ac000001ab00000106a506a500470000000222223333ff030021450000350000000080004cc58fffff057fffff05000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Expected:
 *     PPPoL2tpv2oUDPoIpv4oETH is terminated
 * Delete session spoofing entry and resend packet
 *    cint
 *    l2tp_termination_session_spoofing_delete(0, 201, 203); 
 *    exit;
 * Run traffic:
 *    tx 1  PtchSRCport=201 DATA=0x0011000000330000000000448100001208004500005b0000000000116390ac000001ab00000106a506a500470000000222223333ff030021450000350000000080004cc58fffff057fffff05000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Expected:
 *     packet is dropped
 */

struct cint_l2tp_basic_info_s
{
    int intf_in;                        /* in RIF */
    int intf_out;                       /* out RIF */
    bcm_mac_t intf_in_mac_address;      /* mac for in RIF */
    bcm_mac_t intf_out_mac_address;     /* mac for out RIF */
    bcm_mac_t arp_next_hop_mac;         /* mac for next hop */
    bcm_mac_t arp_next_hop_mac_in;      /* mac for next hop */
    int vrf;                            /* VRF */
    int fec_id;                         /* FEC id */
    int arp_id;                         /* Id for ARP entry */

    int access_vrf;                     /* router instance VRF resolved at VTT1 */
    int provider_vrf;                   /* VRF after termination, resolved as Tunn-InLif property */
    bcm_ip_t tunnel_dip;                /* tunnel DIP */
    bcm_ip_t tunnel_dip_mask;           /* tunnel DIP mask */
    bcm_ip_t tunnel_sip;                /* tunnel SIP */
    bcm_ip_t tunnel_sip_mask;           /* tunnel SIP mask */
    int tunnel_type;                    /* bcmTunnelType */
    uint16 udp_dst_port;                /* Destination UDP port */
    uint16 udp_src_port;                /* Source UDP port */
    bcm_gport_t ip_tunnel_id;           /* ip Tunnel id */
    int ip_tunnel_outlif;               /* OUTLIF to be used in ip tunnel encapsulation */

    uint32 network_domain;              /* l2tpv2 network domain */
    uint32 tunnel_id;                   /* l2tpv2 tunnel id */
    uint32 session_id;                  /* l2tpv2 session id */
    int terminator_id;                  /* Gport for termination entry */
    int initiator_id;                   /* Gport for initirator entry */

    bcm_ip_t host_uni2nni;              /* dip from UNI to NNI */
    bcm_ip_t host_nni2uni;              /* dip from NNI to UNI */

    bcm_ip6_t host6_uni2nni;            /* dip6 from UNI to NNI */
    bcm_ip6_t host6_nni2uni;            /* dip6 from NNI to UNI */
};

cint_l2tp_basic_info_s cint_l2tp_basic_info = {
    /*
     * intf_in | intf_out
     */
    0x12, 0x65,
    /*
     * intf_in_mac_address
     */
    {0x00, 0x11, 0x00, 0x00, 0x00, 0x33},
    /*
     * intf_out_mac_address
     */
    {0x00, 0x11, 0x00, 0x00, 0x00, 0x44},
    /*
     * arp_next_hop_mac
     */
    {0x00, 0x00, 0x00, 0x00, 0xcd, 0x3d},
    /*
     * arp_next_hop_mac_in
     */
    {0x00, 0x00, 0x00, 0x00, 0xcd, 0x4d},
    /*
     * vrf,
     */
    0xf,
    /*
     * fec_id
     */
    0xCDDF,
    /*
     * arp_id
     */
    0,

     /*
     * access_vrf, provider_vrf
     */
    6, 7,
    /*
     * tunnel_dip
     */
    0xAB000001, /* 171.0.0.1 */
    /*
     * tunnel_dip_mask
     */
    0xffffffff,
    /*
     * tunnel_sip
     */
    0xAC000001, /* 172.0.0.1 */
    /*
     * tunnel_sip_mask
     */
    0xffffffff,
    /*
     * tunnel_type
     */
    bcmTunnelTypeUdp,
    /*
     * udp_dst_port, udp_src_port
     */
    1701, 1701,
    /*
     * ip_tunnel_id
     */
    0,
    /*
     * ip_tunnel_outlif
     */
    0x45346,

    /*
     * network_domain
     */
    201,
    /*
     * tunnel_id
     */
    0x2222,
    /*
     * session_id
     */
    0x3333,

    /*
     * terminator_id
     */
    0,
    /*
     * initiator_id
     */
    0,
    /*
     * host_uni2nni
     */
    0x7fffff05,
    /*
     * host_nni2uni
     */
    0x8fffff05,
    /*
     * host6_uni2nni
     */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x13},
    /*
     * host6_nni2uni
     */
    {0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02}
};

int
l2tp_encapsulation_create(
    int unit,
    bcm_if_t next_local_outlif)
{
    int rv;

    bcm_ppp_initiator_t initiator_info;

    initiator_info.type = bcmPPPTypeL2TPv2;
    initiator_info.flags = 0;
    initiator_info.l2tpv2_tunnel_id = cint_l2tp_basic_info.tunnel_id;
    initiator_info.session_id = cint_l2tp_basic_info.session_id;
    initiator_info.l3_intf_id = next_local_outlif;    
    initiator_info.encap_access = bcmEncapAccessTunnel1;
    rv = bcm_ppp_initiator_create(unit, &initiator_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_ppp_initiator_create\n");
        return rv;
    }

    printf("l2tp encapsulation: pppoe encap gport: 0x%x \n", initiator_info.ppp_initiator_id);
    cint_l2tp_basic_info.initiator_id = initiator_info.ppp_initiator_id;

    return rv;
}

int
l2tp_encapsulation_delete(
    int unit)
{
    int rv;
    bcm_ppp_initiator_t initiator_info;
    
    initiator_info.ppp_initiator_id = cint_l2tp_basic_info.initiator_id;
    rv = bcm_ppp_initiator_delete(unit, &initiator_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_ppp_initiator_create\n");
        return rv;
    }
    return rv;
}

int 
l2tp_termination_create(
    int unit,
    int is_mp,
    int is_anti_spoofing)
{
    int rv;
    bcm_ppp_terminator_t terminator_info;
    bcm_l3_ingress_t ing_intf;
    bcm_if_t intf_id;

    terminator_info.type = bcmPPPTypeL2TPv2;
    terminator_info.network_domain = cint_l2tp_basic_info.network_domain;
    terminator_info.l2tpv2_tunnel_id = cint_l2tp_basic_info.tunnel_id;
    terminator_info.session_id = cint_l2tp_basic_info.session_id;
    if (!is_mp)
    {
        terminator_info.flags = BCM_PPP_TERM_CROSS_CONNECT;
    }
    
    if (is_anti_spoofing)
    {
        terminator_info.flags = BCM_PPP_TERM_SESSION_ANTI_SPOOFING;
    }

    rv = bcm_ppp_terminator_create(unit, &terminator_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_ppp_terminator_create \n");
        return rv;
    }
    cint_l2tp_basic_info.terminator_id = terminator_info.ppp_terminator_id;
    printf("l2tp termination: l2tp term gport: 0x%x \n", cint_l2tp_basic_info.terminator_id);

    if (is_mp)
    {
        /** In JR2 VRF is a tunnel-LIF property set by bcm_l3_ingress_create API */
        bcm_l3_ingress_t_init(&ing_intf);
        ing_intf.flags = BCM_L3_INGRESS_WITH_ID;    /* must, as we update exist RIF */
        ing_intf.vrf = cint_l2tp_basic_info.vrf;

        /** Convert tunnel's GPort ID to intf ID */
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(intf_id, cint_l2tp_basic_info.terminator_id);
        rv = bcm_l3_ingress_create(unit, ing_intf, intf_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_l3_ingress_create\n");
            return rv;
        }    
    }
    
    return rv;
}

int 
l2tp_termination_delete(
    int unit)
{
    int rv;
    bcm_ppp_terminator_t terminator_info;

    terminator_info.type = bcmPPPTypeL2TPv2;
    terminator_info.ppp_terminator_id = cint_l2tp_basic_info.terminator_id;

    rv = bcm_ppp_terminator_delete(unit, &terminator_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_ppp_terminator_delete \n");
        return rv;
    }
    return rv;
}


/* Create IP tunnel
 * Parameters: arp_itf - tunnel ARP interface Id
 *             *tunnel_intf - tunnel interface
 *             gre_protocol_type - configure global GRE.protocol (when not IPvX or Eth)
 *             gre_lb_key_enable - boolean indicating GRE8 with lb key
 */
int
l2tp_tunnel_initiator_create(
    int unit,
    bcm_if_t arp_itf,
    bcm_if_t * tunnel_intf)
{
    int rv;
    bcm_tunnel_initiator_t tunnel_info;
    bcm_l3_intf_t l3_intf;

    /*
     * Create IP tunnel initiator for encapsulating GRE4oIPv4 tunnel header
     */
    bcm_tunnel_initiator_t_init(&tunnel_info);
    tunnel_info.dip = cint_l2tp_basic_info.tunnel_dip;
    tunnel_info.sip = cint_l2tp_basic_info.tunnel_sip;

    tunnel_info.flags = 0;
    /*tunnel_info.dscp = cint_l2tp_basic_info.tunnel_dscp;
    tunnel_info.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;*/
    tunnel_info.type = cint_l2tp_basic_info.tunnel_type;
    tunnel_info.udp_dst_port = cint_l2tp_basic_info.udp_dst_port;
    tunnel_info.udp_src_port = cint_l2tp_basic_info.udp_src_port;
    /*tunnel_info.ttl = cint_l2tp_basic_info.tunnel_ttl;*/
    tunnel_info.l3_intf_id = arp_itf;
    tunnel_info.encap_access = bcmEncapAccessTunnel2;
    if (*tunnel_intf > 0)
    {
        BCM_GPORT_TUNNEL_ID_SET(tunnel_info.tunnel_id, *tunnel_intf);
        tunnel_info.flags |= BCM_TUNNEL_WITH_ID;
    }

    tunnel_info.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE;

    bcm_l3_intf_t_init(&l3_intf);
    rv = bcm_tunnel_initiator_create(unit, &l3_intf, tunnel_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_initiator_create \n");
    }

    *tunnel_intf = l3_intf.l3a_intf_id;

    return rv;
}

int
l2tp_tunnel_initiator_delete(
    int unit)
{
    int rv;
    bcm_l3_intf_t initiator_info;
    
    bcm_l3_intf_t_init(&initiator_info);
    
    initiator_info.l3a_intf_id = cint_l2tp_basic_info.ip_tunnel_outlif;
    rv = bcm_tunnel_initiator_clear(unit, &initiator_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_ppp_initiator_create\n");
        return rv;
    }
    return rv;
}


/*
 * Function that creates the general-case tunnel terminator.
 * Inputs: unit - relevant unit;
 */
int
l2tp_tunnel_terminator_create(
    int unit,
    bcm_if_t *tunnel_inlif_id,
    bcm_gport_t *tunnel_id)
{
    int rv;
    bcm_tunnel_terminator_t tunnel_term;

    /** Create IP tunnel terminator for SIP,DIP, VRF lookup */
    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.dip = cint_l2tp_basic_info.tunnel_dip;
    tunnel_term.dip_mask = cint_l2tp_basic_info.tunnel_dip_mask;
    tunnel_term.sip = cint_l2tp_basic_info.tunnel_sip;
    tunnel_term.sip_mask = cint_l2tp_basic_info.tunnel_sip_mask;
    tunnel_term.udp_dst_port = cint_l2tp_basic_info.udp_dst_port;
    tunnel_term.vrf = cint_l2tp_basic_info.provider_vrf;
    tunnel_term.type = cint_l2tp_basic_info.tunnel_type;
    /*tunnel_term.priority = cint_l2tp_basic_info.priority;*/
    
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
    ing_intf.vrf = cint_l2tp_basic_info.vrf;

    /** Convert tunnel's GPort ID to intf ID */
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(intf_id, tunnel_term.tunnel_id);

    rv = bcm_l3_ingress_create(unit, ing_intf, intf_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l3_ingress_create\n");
        return rv;
    }
    *tunnel_inlif_id = intf_id;

    *tunnel_id = tunnel_term.tunnel_id;
    return rv;
}

int
l2tp_tunnel_terminator_delete(
    int unit)
{
    int rv;
    bcm_tunnel_terminator_t tunnel_term;
    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.dip = cint_l2tp_basic_info.tunnel_dip;
    tunnel_term.dip_mask = cint_l2tp_basic_info.tunnel_dip_mask;
    tunnel_term.sip = cint_l2tp_basic_info.tunnel_sip;
    tunnel_term.sip_mask = cint_l2tp_basic_info.tunnel_sip_mask;
    tunnel_term.udp_dst_port = cint_l2tp_basic_info.udp_dst_port;
    tunnel_term.vrf = cint_l2tp_basic_info.provider_vrf;
    tunnel_term.type = cint_l2tp_basic_info.tunnel_type;    
    tunnel_term.tunnel_id = cint_l2tp_basic_info.ip_tunnel_id;
    
    rv = bcm_tunnel_terminator_delete(unit, &tunnel_term);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_ppp_initiator_create\n");
        return rv;
    }
    return rv;
}

/*
 * UNI->NNI Termination 
 +------------+
 |   data     |
 +------------+
 | private ip |
 +------------+
 |    ppp     |
 +------------+
 |    l2tp    |
 +------------+            +-----------+
 |    udp     |            |   data    |
 +------------+            +-----------+
 | public ip  +----------->+ private ip|
 +------------+            +-----------+
 |    eth     |            |   eth     |
 +------------+            +-----------+
 */
int
l2tp_termination(
    int unit,
    int uni_port,
    int nni_port,
    int is_ipv4,
    int is_anti_spoofing)
{
    int rv = BCM_E_NONE;
    char *proc_name = "l2tp_termination";    
    l3_ingress_intf ingress_rif;
    bcm_gport_t gport;
    bcm_if_t tunnel_inlif_id;

    /*
     * 1. Set UNI-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, uni_port, cint_l2tp_basic_info.intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set intf_in\n", proc_name);
        return rv;
    }

    /*
     * 2. Set NNI-Port default properties, in case of ARP+AC no need
     */
    rv = out_port_set(unit, nni_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_intf_set intf_out\n", proc_name);
        return rv;
    }

    /*
     * 3. Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, cint_l2tp_basic_info.intf_in, cint_l2tp_basic_info.intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in\n", proc_name);
        return rv;
    }
    rv = intf_eth_rif_create(unit, cint_l2tp_basic_info.intf_out, cint_l2tp_basic_info.intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n", proc_name);
        return rv;
    }
    /*
     * 4. Set Incoming ETH-RIF properties
     */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_l2tp_basic_info.provider_vrf;
    ingress_rif.intf_id = cint_l2tp_basic_info.intf_in;
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    /*
     * UNI->NNI Termination
     */
    /*
     * 5. Create ip tunnel termination
     */
    rv = l2tp_tunnel_terminator_create(unit, &tunnel_inlif_id, &cint_l2tp_basic_info.ip_tunnel_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create ip tunnel termination\n",proc_name);
        return rv;
    }

    /*
     * 6. Create l2tp tunnel termination
     */
    cint_l2tp_basic_info.network_domain = uni_port;
    rv = l2tp_termination_create(unit, 1, is_anti_spoofing);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create tunnel termination\n",proc_name);
        return rv;
    }
    
    /*
     * 7. Create ARP and set its properties
     */
    rv = l3__egress_only_encap__create(unit, 0, &cint_l2tp_basic_info.arp_id,
                cint_l2tp_basic_info.arp_next_hop_mac, cint_l2tp_basic_info.intf_out);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only\n",proc_name);
        return rv;
    }
    
    /*
     * 8. Create FEC and set its properties
     */
    BCM_GPORT_LOCAL_SET(gport, nni_port);
    rv = l3__egress_only_fec__create(unit, cint_l2tp_basic_info.fec_id, cint_l2tp_basic_info.intf_out, cint_l2tp_basic_info.arp_id, gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC only\n",proc_name);
        return rv;
    }

    /*
     * 9. Add host entries
     */
    if (is_ipv4)
    {
        rv = add_host_ipv4(unit, cint_l2tp_basic_info.host_uni2nni, cint_l2tp_basic_info.vrf, cint_l2tp_basic_info.fec_id, 0, 0);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in function add_host_ipv4(), \n",proc_name);
            return rv;
        }
    }
    else
    {
        rv = add_host_ipv6(unit, cint_l2tp_basic_info.host6_uni2nni, cint_l2tp_basic_info.vrf, cint_l2tp_basic_info.fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in function add_host_ipv6(), \n",proc_name);
            return rv;
        }
    }

    return rv;
}


/*
 * NNI->UNI encapsulation
                            +------------+
                            |   data     |
                            +------------+
                            | private ip |
                            +------------+
                            |    ppp     |
                            +------------+
                            |    l2tp    |
 +------------+             +------------+
 |   data     |             |    udp     |
 +------------+             +------------+
 | private ip +-----------> | public ip  |
 +------------+             +------------+
 |   eth      |             |    eth     |
 +------------+             +------------+
 */
int
l2tp_encapsulation(
    int unit,
    int uni_port,
    int nni_port,
    int is_ipv4)
{
    int rv = BCM_E_NONE;
    char *proc_name = "l2tp_encapsulation";
    l3_ingress_intf ingress_rif;
    bcm_gport_t gport;
    bcm_if_t l3_intf_id = 0;

    /*
     * 1. Set NNI-Port default ETh-RIF
     */
    rv = in_port_intf_set(unit, nni_port, cint_l2tp_basic_info.intf_out);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set intf_in\n", proc_name);
        return rv;
    }

    /*
     * 2. Set NNI-Port default properties, in case of ARP+AC no need
     */
    rv = out_port_set(unit, uni_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_intf_set intf_out\n", proc_name);
        return rv;
    }

    /*
     * 3. Create L2TP ETH-RIF and set its properties
     */
    bcm_vlan_create(unit ,cint_l2tp_basic_info.intf_in);  /* to avoid error from vlan_db_vsi_allocate_single */
    rv = intf_eth_rif_create(unit, cint_l2tp_basic_info.intf_in, cint_l2tp_basic_info.intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in\n", proc_name);
        return rv;
    }

    /*
     * 4. Create NNI ETH-RIF and set its properties
     */
    bcm_vlan_create(unit ,cint_l2tp_basic_info.intf_out); /* to avoid error from vlan_db_vsi_allocate_single */
    rv = intf_eth_rif_create(unit, cint_l2tp_basic_info.intf_out, cint_l2tp_basic_info.intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n", proc_name);
        return rv;
    }

    /*
     * 5. Set Incoming ETH-RIF properties
     */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_l2tp_basic_info.vrf;
    ingress_rif.intf_id = cint_l2tp_basic_info.intf_out;
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    /*
     * 6. Create ARP and set its properties
     */
    rv = l3__egress_only_encap__create(unit, 0, &cint_l2tp_basic_info.arp_id,
                cint_l2tp_basic_info.arp_next_hop_mac_in, cint_l2tp_basic_info.intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only\n",proc_name);
        return rv;
    }

    /*
     * 7. Create UDP IP tunnel
     */
    rv = l2tp_tunnel_initiator_create(unit, cint_l2tp_basic_info.arp_id, &cint_l2tp_basic_info.ip_tunnel_outlif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create l2tp ip tunnel\n",proc_name);
        return rv;
    }

    /*
     * 8. Create l2tp encap object and set its properties
     */
    rv = l2tp_encapsulation_create(unit, cint_l2tp_basic_info.ip_tunnel_outlif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create l2tp tunnel\n",proc_name);
        return rv;
    }

    /*
     * 9. Create FEC and set its properties
     */
    BCM_GPORT_LOCAL_SET(gport, uni_port);
    /*
     * Convert tunnel's GPort ID to intf ID
     */
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(l3_intf_id, cint_l2tp_basic_info.initiator_id);

    rv = l3__egress_only_fec__create(unit, cint_l2tp_basic_info.fec_id, l3_intf_id, 0, gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC only\n", proc_name);
        return rv;
    }

    /*
     * 10. Add host entries
     */
    int fec_id;
    BCM_L3_ITF_SET(&fec_id, BCM_L3_ITF_TYPE_FEC, cint_l2tp_basic_info.fec_id);
    rv = add_host_ipv4(unit, cint_l2tp_basic_info.host_nni2uni, cint_l2tp_basic_info.vrf, fec_id, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv4(), \n", proc_name);
        return rv;
    }

    if (is_ipv4)
    {
        rv = add_host_ipv4(unit, cint_l2tp_basic_info.host_nni2uni, cint_l2tp_basic_info.vrf, fec_id, 0, 0);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in function add_host_ipv4(), \n", proc_name);
            return rv;
        }
    }
    else
    {
        rv = add_host_ipv6(unit, cint_l2tp_basic_info.host6_nni2uni, cint_l2tp_basic_info.vrf, cint_l2tp_basic_info.fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in function add_host_ipv6(), \n",proc_name);
            return rv;
        }
    }

    /** QOS egress remark setting */
    rv = pppoe_qos_map_egress_phb_remark_set(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, pppoe_qos_map_egress_phb_remark_set\n");
        return rv;
    }

    /** Set QOS profile to tunnel gport */
    rv = bcm_qos_port_map_set(unit, cint_l2tp_basic_info.initiator_id, -1, cint_pppoe_basic_info.l3_eg_map_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_qos_port_map_set\n");
        return rv;
    }
    
    return rv;
}


/* NNI->UNI
 * L2TP -> PPPoE P2P service
                             +------------+
                             |   data     |
                             +------------+
                             | private ip |
 +-------------+             +------------+
 |    data     |             |   ppp      |
 +-------------+             +------------+
 |  private ip |             |   l2tp     |
 +-------------+             +------------+
 |    ppp      +             +   udp      |
 +-------------+             +------------+
 |    pppoe    |             | public ip  |
 +-------------+             +------------+
 |    eth      +<------------+   eth      |
 +-------------+             +------------+
 */

int
l2tp_2_pppoe_p2p(
    int unit,
    int uni_port,
    int nni_port)
{
    int rv = BCM_E_NONE;
    char *proc_name = "l2tp_2_pppoe_p2p";
    l3_ingress_intf ingress_rif;
    bcm_gport_t gport;
    bcm_if_t tunnel_inlif_id;

    /* L2tp termination */

    /*
     * Common steps:
     * Set NNI-Port default ETH-RIF
     * Create ETH-RIF and set its properties
     * Set Incoming ETH-RIF properties
     */
    /*
     * 1. Create ip tunnel termination
     */
    rv = l2tp_tunnel_terminator_create(unit, &tunnel_inlif_id, &cint_l2tp_basic_info.ip_tunnel_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create ip tunnel termination\n",proc_name);
        return rv;
    }

    /*
     * 2. Create l2tp tunnel termination
     */
    cint_l2tp_basic_info.network_domain = nni_port;
    rv = l2tp_termination_create(unit, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create tunnel termination\n", proc_name);
        return rv;
    }

    /* PPPoE encapsulation */
    /*
     * Common steps:
     * Set UNI-Port default properties
     */
    /*
     * Encapsulation on UNI port
     */
    /*
     * 3. Create ARP and set its properties
     */
    rv = l3__egress_only_encap__create(unit, 0, &cint_pppoe_basic_info.arp_id_enc,
                cint_pppoe_basic_info.arp_next_hop_mac_enc, cint_pppoe_basic_info.intf_uni);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only\n",proc_name);
        return rv;
    }
    
    /*
     * 4. Create encap object and set its properties
     */
    cint_pppoe_basic_info.initiator_id = pppoe_encapsulation_create(unit, cint_pppoe_basic_info.session_id, cint_pppoe_basic_info.arp_id_enc);

    /*
     * 5. Create FEC and set its properties
     */
    bcm_gport_t fwd_gport;
    bcm_if_t l3_intf_id;
    /*
     * Encapsulation on NNI port in the case P2P test
     */
    BCM_GPORT_LOCAL_SET(gport, uni_port);
    /*
     * Convert tunnel's GPort ID to intf ID
     */
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(l3_intf_id, cint_pppoe_basic_info.initiator_id);
    rv = l3__egress_only_fec__create(unit, cint_pppoe_basic_info.fec_id_enc, l3_intf_id, 0, gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC only\n", proc_name);
        return rv;
    }
    BCM_GPORT_FORWARD_PORT_SET(fwd_gport, cint_pppoe_basic_info.fec_id_enc);
    
    /*
     * 6. Cross connect the l2tp terminate port with PPPoE encap port
     */
    bcm_vswitch_cross_connect_t gports;
    gports.port1 = cint_l2tp_basic_info.terminator_id;
    gports.port2 = fwd_gport;
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;

    rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, bcm_vswitch_cross_connect_add\n",proc_name);
        return rv;
    }

    return rv;
}



/*
 * UNI->NNI
 * PPPoE -> L2TP P2P service
                             +------------+
                             |   data     |
                             +------------+
                             | private ip |
 +-------------+             +------------+
 |    data     |             |   ppp      |
 +-------------+             +------------+
 |  private ip |             |   l2tp     |
 +-------------+             +------------+
 |    ppp      +             +   udp      |
 +-------------+             +------------+
 |    pppoe    |             | public ip  |
 +-------------+             +------------+
 |    eth      +------------>+   eth      |
 +-------------+             +------------+
 */

int
pppoe_2_l2tp_p2p(
    int unit,
    int uni_port,
    int nni_port)
{

    int rv = BCM_E_NONE;
    char *proc_name = "pppoe_2_l2tp_p2p";
    l3_ingress_intf ingress_rif;
    bcm_gport_t gport;
    bcm_if_t tunnel_inlif_id;

    /* PPPoE termination */

    /*
     * Common steps:
     * Set UNI-Port default ETH-RIF
     * Create ETH-RIF and set its properties
     * Set Incoming ETH-RIF properties
     */

    /*
     * 1. Create pppoe termination
     */
    cint_pppoe_basic_info.terminator_id = pppoe_termination_create(unit,
                        cint_pppoe_basic_info.session_id,
                        cint_pppoe_basic_info.source_mac_address,
                        cint_pppoe_basic_info.is_mp,
                        cint_pppoe_basic_info.is_session_spoofing_check,
                        cint_pppoe_basic_info.urpf_mode,
                        cint_pppoe_basic_info.is_wide);
    /* L2tp encapsulation */

    /*
     * Common steps:
     * Set UNI-Port default properties
     * Create ARP and set its properties
     */

    /*
     * 2. Create ARP and set its properties
     */
    rv = l3__egress_only_encap__create(unit, 0, &cint_l2tp_basic_info.arp_id,
                cint_l2tp_basic_info.arp_next_hop_mac, cint_l2tp_basic_info.intf_out);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only\n",proc_name);
        return rv;
    }

    /*
     * 3. Create UDP IP tunnel
     */
    rv = l2tp_tunnel_initiator_create(unit, cint_l2tp_basic_info.arp_id, &cint_l2tp_basic_info.ip_tunnel_outlif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create l2tp ip tunnel\n",proc_name);
        return rv;
    }

    /*
     * 4. Create l2tp encap object and set its properties
     */
    rv = l2tp_encapsulation_create(unit, cint_l2tp_basic_info.ip_tunnel_outlif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create l2tp tunnel\n",proc_name);
        return rv;
    }

    /*
     * 5. Create FEC and set its properties
     */
    BCM_GPORT_LOCAL_SET(gport, nni_port);
    /*
     * Convert tunnel's GPort ID to intf ID
     */
    bcm_if_t l3_intf_id;
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(l3_intf_id, cint_l2tp_basic_info.initiator_id);

    rv = l3__egress_only_fec__create(unit, cint_l2tp_basic_info.fec_id, l3_intf_id, 0, gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC only\n", proc_name);
        return rv;
    }
    bcm_gport_t fwd_gport;
    BCM_GPORT_FORWARD_PORT_SET(fwd_gport, cint_l2tp_basic_info.fec_id);

    /*
     * 6. Cross connect the l2tp terminate port with PPPoE encap port
     */
    bcm_vswitch_cross_connect_t gports;
    gports.port1 = cint_pppoe_basic_info.terminator_id;
    gports.port2 = fwd_gport;
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;

    rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, bcm_vswitch_cross_connect_add\n",proc_name);
        return rv;
    }

    return rv;
}

/*
 * UNI<->NNI
 * PPPoE <-> L2TP P2P service 
                             +------------+
                             |   data     |
                             +------------+
                             | private ip |
 +-------------+             +------------+
 |    data     |             |   ppp      |
 +-------------+             +------------+
 |  private ip |             |   l2tp     |
 +-------------+             +------------+
 |    ppp      +             +   udp      |
 +-------------+             +------------+
 |    pppoe    |             | public ip  |
 +-------------+             +------------+
 |    eth      +<------------>+   eth      |
 +-------------+             +------------+
 */
int
pppoe_2_l2tp_bidirection_cross_connect(
    int unit,
    int uni_port,
    int nni_port)
{

    int rv = BCM_E_NONE;
    char *proc_name = "pppoe_2_l2tp_p2p";
    l3_ingress_intf ingress_rif;
    bcm_gport_t gport;
    bcm_if_t tunnel_inlif_id;

    /*
     * 1. Set In-Port default ETH-RIF
     */
    rv = in_port_intf_set(unit, uni_port, cint_pppoe_basic_info.intf_uni);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set intf_in\n", proc_name);
        return rv;
    }
    rv = in_port_intf_set(unit, nni_port, cint_l2tp_basic_info.intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set intf_in\n", proc_name);
        return rv;
    }

    /*
     * 2. Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, cint_pppoe_basic_info.intf_uni, cint_pppoe_basic_info.intf_uni_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_uni\n", proc_name);
        return rv;
    }
    rv = intf_eth_rif_create(unit, cint_l2tp_basic_info.intf_in, cint_l2tp_basic_info.intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in\n", proc_name);
        return rv;
    }

    /*
     * 3. Set Incoming ETH-RIF properties
     */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_pppoe_basic_info.vrf;
    ingress_rif.intf_id = cint_pppoe_basic_info.intf_uni;
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_l2tp_basic_info.provider_vrf;
    ingress_rif.intf_id = cint_l2tp_basic_info.intf_in;
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    /*
     * 4. Set UNI-Port default properties
     */
    rv = out_port_set(unit, uni_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_intf_set intf_out\n", proc_name);
        return rv;
    }
    /*
     * 5. Set NNI-Port default properties, Create ETH-RIF and set its properties
     */
    rv = out_port_set(unit, nni_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_intf_set intf_out\n", proc_name);
        return rv;
    }
    rv = intf_eth_rif_create(unit, cint_l2tp_basic_info.intf_out, cint_l2tp_basic_info.intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n", proc_name);
        return rv;
    }

    /*
     * 7. L2TPv2 -> PPPoE P2P service
     */
    rv = l2tp_2_pppoe_p2p(unit, uni_port, nni_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create tunnel termination\n",proc_name);
        return rv;
    }
    /*
     * 8. PPPoE -> L2TPv2 P2P service
     */
    /** Enable P2P */
    cint_pppoe_basic_info.is_mp = 0;
    /** Disable PPPoE sesion spoofing check */
    cint_pppoe_basic_info.is_session_spoofing_check = 0;
    /** No RPF */
    cint_pppoe_basic_info.urpf_mode = 0;
    /** No Wide LIF */
    cint_pppoe_basic_info.is_wide = 0;
    rv = pppoe_2_l2tp_p2p(unit, uni_port, nni_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create tunnel termination\n",proc_name);
        return rv;
    }
    return rv;
}

/*
 * Example of L2TP Termination with session spoofing check(UNI->NNI)
 * Input variables
 * 1. VTT lookup keys(as default)
 * - cint_l2tp_basic_info.network_domain: Network identifier
 * - cint_l2tp_basic_info.tunnel_id: session_id in L2TP header
 * - cint_l2tp_basic_info.session_id: session_id in L2TP header
 * 2. PPPoE LIF properties
 * - cint_l2tp_basic_info.is_mp: Ture for MP and False for P2P(as default)
 * - cint_l2tp_basic_info.is_session_spoofing_check: Ture to do session_spoofing_check here
 */
int
l2tp_termination_session_spoofing_add(
    int unit,
    int uni_port,
    int nni_port)
{
    int rv = BCM_E_NONE;
    int is_ipv4 = 1;
    int is_anti_spoofing = 1;

    /** UNI->NNI PPPoE Termination */
    rv = l2tp_termination(unit, uni_port, nni_port, is_ipv4, is_anti_spoofing);
    if (rv != BCM_E_NONE)
    {
        printf("Error, pppoe_termination\n");
        return rv;
    }

    /** l2tp session spoofing check */
    bcm_ppp_term_spoofing_check_t info;

    info.ppp_terminator_id = cint_l2tp_basic_info.terminator_id;
    info.vlan_port_id = cint_l2tp_basic_info.ip_tunnel_id;

    rv = bcm_ppp_term_spoofing_check_add(unit, &info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_ppp_term_spoofing_check_add \n");
        return rv;
    }

    printf("l2tp termination: session spooffing add terminator_id: 0x%x \n", info.ppp_terminator_id);
    printf("l2tp termination: session spooffing add vlan_port_id: 0x%x \n", info.vlan_port_id);

    return rv;
}

/*
 * Delete a l2tp session spoofing check entry per given PPP Tunnel Gport and Vlan Gport.
 */
int
l2tp_termination_session_spoofing_delete(
    int unit,
    int uni_port,
    int nni_port)
{
    int rv;

    bcm_ppp_term_spoofing_check_t info;

    info.ppp_terminator_id = cint_l2tp_basic_info.terminator_id;
    info.vlan_port_id = cint_l2tp_basic_info.ip_tunnel_id;

    rv = bcm_ppp_term_spoofing_check_delete(unit, &info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_ppp_term_spoofing_check_delete \n");
        return rv;
    }
    return rv;
}

int ppp_term_spoofing_check_traverse_cb(
    int unit, 
    bcm_ppp_term_spoofing_check_t *info, 
    void *user_data)
{
    printf("l2tp termination: session spooffing add terminator_id: 0x%x \n", info->ppp_terminator_id);
    printf("l2tp termination: session spooffing add vlan_port_id: 0x%x \n", info->vlan_port_id);
    return BCM_E_NONE;
}


int
l2tp_termination_session_spoofing_traverse(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_ppp_term_spoofing_check_additional_info_t additional_info;

    rv = bcm_ppp_term_spoofing_check_traverse(unit, &additional_info, &ppp_term_spoofing_check_traverse_cb, NULL);

    return rv;    
}


