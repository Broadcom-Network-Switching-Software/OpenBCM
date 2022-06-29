/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
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
 *    cint ../../../src/examples/dnx/field/cint_field_epmf_cs_outlif_profile.c
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
 *
 * 5. Example of L2tp-MPLS MP termination
 *
 * Set up sequence:
 *    cint ../../../src/examples/sand/utility/cint_sand_utils_global.c
 *    cint ../../../src/examples/dnx/field/cint_field_utils.c
 *    cint ../../../src/examples/sand/utility/cint_sand_utils_l3.c
 *    cint ../../../src/examples/sand/cint_ip_route_basic.c
 *    cint ../../../src/examples/dnx/utility/cint_dnx_util_rch.c
 *    cint ../../../src/examples/dnx/ppp/cint_dnx_l2tp.c
 *    cint
 *    l2tp_mpls_termination(0,200,201,1,0);
 *    exit;
 * Run traffic:
 *    tx 1  PtchSRCport=201 DATA=0x001100000033000000000044810000128847003e81404500005b000000008011e38fac000001ab00000106a506a500470000000222223333ff030021450000350000000080002cbf8fffff057fffff05000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Expected:
 *     PPPoL2tpv2oUDPoIpv4oMPLSoETH is terminated
 * Delete session entry and resend packet
 *    cint
 *    l2tp_termination_delete(0); 
 *    cint
 *    l2tp_tunnel_terminator_delete(0);
 *    exit;
 * Run traffic:
 *    tx 1  PtchSRCport=201 DATA=0x001100000033000000000044810000128847003e81404500005b000000008011e38fac000001ab00000106a506a500470000000222223333ff030021450000350000000080002cbf8fffff057fffff05000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Expected:
 *     packet is dropped
 *
 * 6. Example of L2tp-MPLS MP encapsulation
 *
 * Set up sequence:
 *    cint ../../../src/examples/sand/utility/cint_sand_utils_global.c
 *    cint ../../../src/examples/dnx/field/cint_field_utils.c
 *    cint ../../../src/examples/sand/cint_ip_route_basic.c
 *    cint ../../../src/examples/dnx/ppp/cint_dnx_pppoe.c
 *    cint ../../../src/examples/dnx/ppp/cint_dnx_l2tp.c
 *    cint
 *    l2tp_mpls_encapsulation(0,200,201,1,0x33);
 *    exit;
 * Run traffic:
 *    tx 1  PtchSRCport=201 DATA=0x00000000000000000000000000000000801d001100000044000000000033810000650800451f00350000000080002ca07fffff058fffff05000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Expected:
 *     Ipv4oETH is encapsulated with PPPoL2TPoUDPoIPV4oMPLS
 * Delete session spoofing entry and resend packet
 *    cint
 *    l2tp_termination_delete(0); 
 *    cint
 *    l2tp_tunnel_terminator_delete(0);
 *    exit;
 * Run traffic:
 *    tx 1  PtchSRCport=201 DATA=0x00000000cd4d00110000003381000012884700d05133453e005b0000000000116352ac000001ab00000106a506a500470000000222223333ff030021451d0035000000007f002da27fffff058fffff05000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
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

/*L2tp  MPLS Termination structure*/
 struct cint_l2tp_mpls_info_s 
{
    int fec_id;                  /* mpls fec id */
    bcm_if_t rcy_extended_term_encap_id;
    int vmpi;                    /*mpls vmpi*/
    uint32 network_domain;         /*mpls tunnel domain*/
    bcm_mpls_label_t mpls_tunnel_term_label;         /*Incoming tunnel label*/
    int recycle_port_id;                    /*recycle port ID*/
    int mpls_tunnel_id[2];                  /*mpls tunnel id*/ 
};
  /*2) L2tp  MPLS Encapsulation structure*/
  
 struct cint_l2tp_mpls_encapsulation_info_s
 {
     bcm_mac_t intf_out_mac_address;     /* mac for out RIF */
     bcm_mac_t arp_next_hop_mac; /* mac for next hop */
     int arp_id_push;            /* Id for ARP entry for push/swap entries */
     int fec_id_enc;             /* fec id for encapsulation entry - outlif for MPLS entry in EEDB */
     int push_tunnel_id;         /* tunnel id for encapsulation entry */
     bcm_mpls_label_t pushed_or_swapping_label;  /* pushed or swapping label */
     int ingress_qos_profile;    /* qos profile, ingress */
     int egress_qos_profile;     /* qos profile, egress */
     int svtag_enable;           /* enable and SVTAG lookup */
     int vmpi;                   /*mpls vmpi*/
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

/**/
cint_l2tp_mpls_info_s cint_l2tp_mpls_info=
{
    /*fec_id*/
    0x2000c35B,
    /*rcy_extended_term_encap_id*/
    0,
    /*vmpi*/
    0,
    /*network_domain*/
    0,
    /*
    * mpls_tunnel_term_label
    */
    1000, 

    /*
    * recycle_port_id
    */
    50
};
cint_l2tp_mpls_encapsulation_info_s cint_l2tp_mpls_encapsulation_info =
{
    /*intf_out_mac_address*/
    {0x00, 0x00, 0x00, 0x00, 0xCD, 0x4D},
    /*arp_next_hop_mac*/
    {0x00, 0x11, 0x00, 0x00, 0x00, 0x33},
    /*
     * arp_id_push
     */
    0,
     /*
     * fec_id_enc
     */
    0x2000C351,
    /*
     * push_tunnel_id
     */
    0,
    /*
     * pushed_or_swapping_label
     */
    3333,
    /*
     * ingress_qos_profile | egress_qos_profile
     */
    3, 1, 
     /*
     * svtag_enable
     */
    0,

     /*
     * vmpi
     */
    0
};

static int l2tp_ing_qos_profile = -1;
static int l2tp_egr_qos_profile = -1;
int l2tp_fec_id_deviation = 0;

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
    
/* Create MPLS  tunnel ID to push mpls label
 * Parameters: unit -unit id
 *                    ttl_value - If ttl_value!=0, tunnel is configured in PIPE mode, which will set ttl_value in out packet
 *                                    If ttl_value==0, configuration is UNIFORM, which takes the ttl_value from in_packet
 * Result:         cint_l2tp_mpls_encapsulation_info.push_tunnel_id
 */
int
l2tp_mpls_encapsulation_create_push_or_swap_tunnel(
    int unit,
    uint8 ttl_value)
{

    bcm_mpls_egress_label_t label_array[1];
    int num_labels = 1;
    int egress_qos_profile;
    int rv;

    bcm_mpls_egress_label_t_init(&label_array[0]);

    /*
     * Pushed or Swapping label.
     * The label will be pushed for IP forwarding context and swapped for
     * MPLS forwarding context.
     */
     /*It is important to set a proper encap_access to get a right phrase in the etps*/
    label_array[0].encap_access = bcmEncapAccessTunnel3;
    label_array[0].label = cint_l2tp_mpls_encapsulation_info.pushed_or_swapping_label;
    /*
     * In JR2: To set the next pointer of this entry to be the arp, use label_array[0].l3_intf_id.
     * Here this link is done in later stage by updating ARP entry with
     * BCM_L3_KEEP_DSTMAC | BCM_L3_KEEP_VLAN flags
     */

    /*
     * If ttl_value != 0, means this value should be taken from the
     * table. (TTL_SET)
     * In case of 0, thee value will be taken from pipe (TTL_COPY)
     */
    label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    if (ttl_value != 0)
    {

        label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
        label_array[0].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
        label_array[0].ttl = ttl_value;
    }
    else
    {
        label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
        label_array[0].egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    }

    /*
     * In non-jericho2 device SWAP_OR_PUSH action needs to be defined in order to use EEDB entry for swap action.
     */

    /** qos remark profile */
    rv = bcm_qos_map_id_get_by_profile(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK,
                                       cint_l2tp_mpls_encapsulation_info.egress_qos_profile, &egress_qos_profile);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_qos_map_id_get_by_profile\n");
        return rv;
    }

    label_array[0].qos_map_id = egress_qos_profile;

    if(cint_l2tp_mpls_encapsulation_info.svtag_enable)
    {
        label_array[0].flags2 |= BCM_MPLS_EGRESS_LABEL_FLAGS2_SVTAG_ENABLE;
    }

    rv = bcm_mpls_tunnel_initiator_create(unit, 0, num_labels, label_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    cint_l2tp_mpls_encapsulation_info.push_tunnel_id = label_array[0].tunnel_id;

    return rv;

}

/* Update predefined ARP entry with required MPLS tunnel.
 * Parameters: unit -unit id
 * Result:         update EEDB_ARP
 */
int
l2tp_mpls_encapsulation_update_arp_entry(
    int unit)
{
    int rv;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid_null;       /* not intersting */
    uint32 flags = BCM_L3_WITH_ID | BCM_L3_REPLACE | BCM_L3_EGRESS_ONLY | BCM_L3_KEEP_DSTMAC | BCM_L3_KEEP_VLAN;

    /** configure arp for push action */
    bcm_l3_egress_t_init(&l3eg);
    l3eg.mac_addr = cint_l2tp_mpls_encapsulation_info.arp_next_hop_mac;
    l3eg.encap_id = cint_l2tp_mpls_encapsulation_info.arp_id_push;
    l3eg.vlan = cint_l2tp_basic_info.intf_in;
    l3eg.intf = cint_l2tp_mpls_encapsulation_info.push_tunnel_id;

    rv = bcm_l3_egress_create(unit, flags, &l3eg, &l3egid_null);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object ARP only\n");
        return rv;
    }

    return rv;

}

/* set  MPLS encapsulation with 1 label for L2TPoUDPoIPV4oETH.
 * Parameters: unit -unit id
 *                    uni_port - uni port id
 *                    uni_port - nni port id
 *                    ttl_value - If ttl_value!=0, tunnel is configured in PIPE mode, which will set ttl_value in out packet
 *                                    If ttl_value==0, configuration is UNIFORM, which takes the ttl_value from in_packet
 */
int
l2tp_mpls_encapsulation_set(
    int unit,
    int uni_port,
    int nni_port,
    uint8 ttl_value)
{
    int rv = BCM_E_NONE;
    int first_fec_id_in_hierarchy;

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit,0,0,&first_fec_id_in_hierarchy);
    first_fec_id_in_hierarchy += 70;
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), get_first_fec_in_range_which_not_in_ecmp_range\n", rv);
        return rv;
    }
    cint_l2tp_mpls_encapsulation_info.fec_id_enc = first_fec_id_in_hierarchy++;


    /*
     * Check if user's qos profile is valid.
     */
    if (l2tp_ing_qos_profile != -1)
    {
        cint_l2tp_mpls_encapsulation_info.ingress_qos_profile = l2tp_ing_qos_profile;
    }

    if (l2tp_ing_qos_profile != -1)
    {
        cint_l2tp_mpls_encapsulation_info.egress_qos_profile = l2tp_egr_qos_profile;
    }

    /*
     * Configure ARP entry for push and swap actions
     */
    rv = l3__egress_only_encap__create(unit, 0, &(cint_l2tp_mpls_encapsulation_info.arp_id_push),
                                       cint_l2tp_mpls_encapsulation_info.intf_out_mac_address,
                                       cint_l2tp_basic_info.intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object ARP only push\n");
        return rv;
    }

    /*
     * Configure a push or swap entry. The label in the entry will swap the swapped label in case
     * context is forwarding. In case context is ip routing, label will be pushed.
     */
    rv = l2tp_mpls_encapsulation_create_push_or_swap_tunnel(unit, ttl_value);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_encapsulation_basic_create_push_or_swap_tunnel\n");
        return rv;
    }
    
    /*
     * Configure fec entry for encapsulation flow
     */
    rv = l3__egress_only_fec__create(unit, cint_l2tp_mpls_encapsulation_info.fec_id_enc,
                                     cint_l2tp_mpls_encapsulation_info.push_tunnel_id, 0,
                                     uni_port, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }

    /*
     * Connect MPLS tunnel entry to ARP.
     * ARP entry configuration can be also done inside bcm_mpls_tunnel_inititator_create.
     */
    rv = l2tp_mpls_encapsulation_update_arp_entry(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_basic_create_arp_entry\n");
        return rv;
    }

    /*
     * Create l3 forwarding entry for the pushed label
     */
    rv = add_host_ipv4(unit, cint_l2tp_basic_info.tunnel_dip, cint_l2tp_basic_info.vrf,
    cint_l2tp_mpls_encapsulation_info.fec_id_enc, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host_ipv4, \n");
        return rv;
    }
    cint_l2tp_mpls_encapsulation_info.fec_id_enc |= 0x20000000;
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
 |   eth      |                        |    mpls     |
 +------------+             +------------+
                                         |    eth     |
                                       +------------+
 */

int
l2tp_mpls_encapsulation(
  int unit,
  int uni_port,
  int nni_port,
  int is_ipv4,
  int ttl_value)
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
        printf("%s(): Error, intf_eth_rif_create in_rif\n", proc_name);
        return rv;
    }

    /*
    * 4. Create NNI ETH-RIF and set its properties
    */
    bcm_vlan_create(unit ,cint_l2tp_basic_info.intf_out); /* to avoid error from vlan_db_vsi_allocate_single */
    rv = intf_eth_rif_create(unit, cint_l2tp_basic_info.intf_out, cint_l2tp_basic_info.intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create out_rif\n", proc_name);
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
        printf("%s(): Error, intf_eth_rif_create out_rif\n",proc_name);
        return rv;
    }

    l2tp_mpls_encapsulation_set(unit,uni_port,nni_port,ttl_value);

    /*
    * 7. Create UDP IP tunnel, it should be connected to MPLS tunnel inforamation with 
    */
    rv = l2tp_tunnel_initiator_create(unit, cint_l2tp_mpls_encapsulation_info.push_tunnel_id, &cint_l2tp_basic_info.ip_tunnel_outlif);
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

    rv = l3__egress_only_fec__create(unit, cint_l2tp_basic_info.fec_id, l3_intf_id, 0, gport, 0, 0);
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
    if (is_ipv4)
    {
        rv = add_host_ipv4(unit, cint_l2tp_basic_info.host_nni2uni, cint_l2tp_basic_info.vrf, fec_id, 0, 0);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in function add_host_ipv4(), \n", proc_name);
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

  /**
   * \brief
   *
   *  Setup ingress PMF rules to identify all LSR missing packets not come from rcy port and trap to recycle port.
   *
   * \param [in] unit     - The unit number.
   * \param [in] in_port  - in port.
   * \param [in] rcy_port - recycle port
   *
   * \return
   *  int - Error Type, in sense of bcm_error_t
   *
   */
int l2tp_field_extended_term_flow_set_ipmf1(int unit, 
    int in_port, 
    int rcy_port, 
    bcm_if_t rcy_extended_term_encap_id) 
{
    int rv = 0;
    bcm_field_group_t fg_id = 0;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    int nof_quals = 3;
    int nof_actions = 1;
    int ii = 0;
    bcm_field_context_t context_id;
    void *dest_char;
    bcm_field_entry_info_t entry_info;
    bcm_field_entry_t entry_handle;
    int mpls_ingress_trap_id_unknown_dest = 0xa;
    uint32 normal_port_profile = 2;
    bcm_gport_t gport_trap = 0;
    int new_trap_id = 0;
    bcm_rx_trap_config_t config;
    int trap_strength = 7;

    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_id_t presel_entry_id;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_param_info_t param_info;
    uint32 ibch1_supported = *dnxc_data_get(unit, "headers", "system_headers", "system_headers_ibch1_supported", NULL);

    /* Set ipmf_port_profile to differ general port(2) and rcycle port(0) */
    rv = bcm_port_class_set(unit, in_port, bcmPortClassFieldIngressPMF1PacketProcessingPort, normal_port_profile);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_port_class_set with type == bcmPortClassFieldIngressPMF1PacketProcessingPort\n");
        return rv;
    }

    if (!ibch1_supported)
    {
        /** Configure RCH port of Extended Termination type */
        rv = bcm_port_control_set(unit, rcy_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppExtendedTerm);
        if (rv != BCM_E_NONE)
        {
            printf("mpls_field_extended_term_flow_set_ipmf1(): Error, bcm_port_control_set for rch_port %d\n", rcy_port);
            return rv;
        }
    }
    context_id = *dnxc_data_get(unit, "field", "context", "default_context", NULL);
    printf("l2tp_field_extended_term_flow_set_ipmf1(): context_id %u\n", context_id);

    /*It is a example to set LayerRecordType, RxTrapCode and PortClassPacketProcessing as qualifier ;
        * customer can define their own qualifier
        */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;
    fg_info.nof_quals = nof_quals;
    fg_info.qual_types[0] = bcmFieldQualifyRxTrapCode;
    fg_info.qual_types[1] = bcmFieldQualifyPortClassPacketProcessing;
    fg_info.qual_types[2] = bcmFieldQualifyLayerRecordType;


    fg_info.nof_actions = nof_actions;
    fg_info.action_types[0] = bcmFieldActionTrap;
    fg_info.action_with_valid_bit[0] = TRUE;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "Mpls_L2TP_ENTRY_HIT", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, 0, &fg_info, &fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_field_group_add\n");
        return rv;
    }

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
        attach_info.payload_info.action_info[ii].priority = BCM_FIELD_ACTION_PRIORITY(0, (ii + 1));
    }

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;
    
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[1].input_arg = 0;
    attach_info.key_info.qual_info[1].offset = 0;
    
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerRecordsFwd; 
    attach_info.key_info.qual_info[2].input_arg = 0;
    attach_info.key_info.qual_info[2].offset = 0;

    rv = bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_field_group_context_attach\n");
        return rv;
    }

    /*
    * Create a trap that new destination is rcy port and bring extended term encap to egress.
    */
    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &new_trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_rx_trap_type_create\n");
        return rv;
    }

    bcm_rx_trap_config_t_init(&config);
    config.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_UPDATE_ENCAP_ID;
    config.dest_port = rcy_port;
    config.trap_strength = 0;
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(config.encap_id, rcy_extended_term_encap_id);
    rv = bcm_rx_trap_set(unit, new_trap_id, &config);     
    BCM_GPORT_TRAP_SET(gport_trap, new_trap_id, trap_strength, 0);

    /*
    * Qualify all LSR miss packets from normal port, 
    * Trap these packet to recycle port and stard term more label at 2nd pass.
    */
    bcm_field_entry_info_t_init(&entry_info);
    entry_info.nof_entry_actions = fg_info.nof_actions;
    entry_info.nof_entry_quals = fg_info.nof_quals;
    
    entry_info.entry_qual[0].type = bcmFieldQualifyRxTrapCode;
    entry_info.entry_qual[0].value[0] = mpls_ingress_trap_id_unknown_dest;
    entry_info.entry_qual[0].mask[0] = 0x1FF;
    
    entry_info.entry_qual[1].type = bcmFieldQualifyPortClassPacketProcessing;
    entry_info.entry_qual[1].value[0] = normal_port_profile;
    entry_info.entry_qual[1].mask[0] = 0x7;
 
    entry_info.entry_qual[2].type = bcmFieldQualifyLayerRecordType;
    entry_info.entry_qual[2].value[0] = bcmFieldLayerTypeIp4;
    entry_info.entry_qual[2].mask[0] = 0x1f;
       
    entry_info.entry_action[0].type = bcmFieldActionTrap;
    entry_info.entry_action[0].value[0] = gport_trap;
    rv = bcm_field_entry_add(unit, 0, fg_id, &entry_info, &entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_field_entry_add\n");
        return rv;
    }

    return rv;
}

/* Set MPLS label termination for  L2TPoUDPoIPV4oMPLSoETH
 * Parameters: unit -unit id
 *                    port1 - uni port id
 *                    port2 - nni port id
 */
 int
l2tp_mpls_termination_set(
  int unit,
  int port1,
  int port2)
{
    int rv = BCM_E_NONE;
    int i;
    bcm_gport_t vlan_default;
    char *proc_name;
    uint32 egress_filters_enable_per_recycle_port_supported = *dnxc_data_get(unit, "port_pp", "filters", "egress_filters_enable_per_recycle_port_supported", NULL);
    bcm_l2_egress_t recycle_entry;

    bcm_mpls_tunnel_switch_t entry;
    bcm_if_t l3_intf_id;
    l3_ingress_intf ingr_itf;
    uint32 network_domain = 11; 
    proc_name = "l2tp_mpls_create";

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &cint_l2tp_mpls_info.fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }

    rv = in_port_intf_set(unit, port1, cint_l2tp_basic_info.intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("Error, multi_dev_ingress_port_configuration \n");
        return rv;
    }

    /*
    * Create ETH-RIF and set its properties 
    */
    rv = intf_eth_rif_create(unit, cint_l2tp_basic_info.intf_in,
    cint_l2tp_basic_info.intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in\n");
        return rv;
    }
    printf("PASS! intf_eth_rif_create intf_in \n");
    /*
    * Create Recycle port application
    */

    bcm_l2_egress_t_init(&recycle_entry);
    recycle_entry.flags = BCM_L2_EGRESS_RECYCLE_HEADER;
    recycle_entry.recycle_app = bcmL2EgressRecycleAppExtendedTerm;
    rv = bcm_l2_egress_create(unit, &recycle_entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_l2_egress_create with recycle_app is bcmL2EgressRecycleAppExtendedTerm\n");
        return rv;
    }
    cint_l2tp_mpls_info.rcy_extended_term_encap_id = recycle_entry.encap_id;
    printf("PASS, in bcm_l2_egress_create with recycle_app is bcmL2EgressRecycleAppExtendedTerm\n");

    /** need to disable egress filters explicitly for ExtTerm and Drop&Cont, base on the RCH out-lif */
    if (egress_filters_enable_per_recycle_port_supported == 0)
    {
        int outlif_profile_id = 5;
        rv = cint_field_epmf_cs_outlif_profile_disable_egress_filters_main(unit, recycle_entry.encap_id, outlif_profile_id);
        if (rv != BCM_E_NONE)
        {
             printf("Error, cint_field_epmf_cs_outlif_profile_disable_egress_filters_main\n");
             return rv;
        }
    }
    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = BCM_MPLS_SWITCH_ACTION_POP;
    entry.class_id = cint_l2tp_mpls_info.vmpi;
    /*
    * incoming label 
    */
    entry.label = cint_l2tp_mpls_info.mpls_tunnel_term_label;
    entry.flags2 |= BCM_MPLS_SWITCH2_EXTENDED_TERMINATION;
    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }
    cint_l2tp_mpls_info.mpls_tunnel_id[0] = entry.tunnel_id;
    printf("PASS, in bcm_mpls_tunnel_switch_create\n");

    /*set mpls port vlan_domain, it will be used to set L2TPV2_DATA_MESSAGE_TT*/
    rv = bcm_port_class_set(unit, entry.tunnel_id, bcmPortClassIngress, network_domain);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_port_class_set with type == bcmPortClassFieldIngressPMF1PacketProcessingPort\n");
        return rv;
    }

    cint_l2tp_mpls_info.network_domain = network_domain;

    /*
    * Set Incoming Tunnel-RIF properties 
    */
    /* In case of J2 the RIF is the mpls tunnel id */
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(l3_intf_id, entry.tunnel_id);
    ingr_itf.intf_id = l3_intf_id;
    ingr_itf.vrf = cint_l2tp_basic_info.provider_vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_ingress_rif_set\n");
        return rv;
    }
    printf("PASS, intf_ingress_rif_set\n");

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
 +------------+           +-----------+
 |    udp     |                  |   data    |
 +------------+           +-----------+
 | public ip  +--------->+ private ip|
 +------------+           +-----------+
 |    mpls     |                 |   eth     |
  +------------+          +-----------+
 |    eth     |            
 +------------+            
 */

int
l2tp_mpls_termination(
    int unit,
    int uni_port,
    int nni_port,
    int is_ipv4,
    int is_anti_spoofing)
{
    int rv = BCM_E_NONE;
    char *proc_name = "l2tp_mpls_termination";    
    l3_ingress_intf ingress_rif;
    bcm_gport_t gport;
    bcm_if_t tunnel_inlif_id;
    
    rv = l2tp_mpls_termination_set(unit,uni_port,nni_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set intf_in\n", proc_name);
        return rv;
    }
    
    l2tp_field_extended_term_flow_set_ipmf1(unit, uni_port, cint_l2tp_mpls_info.recycle_port_id, 
        cint_l2tp_mpls_info.rcy_extended_term_encap_id);
    /*
       * 1. Set UNI-Port to In ETh-RIF
       */
    rv = in_port_intf_set(unit, cint_l2tp_mpls_info.recycle_port_id, cint_l2tp_basic_info.intf_in);
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
    printf("PASS! out_port_set intf_in \n");
        
      /*
       * 3. Create ETH-RIF and set its properties
       */

    rv = intf_eth_rif_create(unit, cint_l2tp_basic_info.intf_out, cint_l2tp_basic_info.intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n", proc_name);
        return rv;
    }
    printf("PASS! intf_eth_rif_create core_side_out_rif-core_side_next_hop_mac \n");
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
    cint_l2tp_basic_info.network_domain = cint_l2tp_mpls_info.network_domain;
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
    rv = l3__egress_only_fec__create(unit, cint_l2tp_basic_info.fec_id, cint_l2tp_basic_info.intf_out, cint_l2tp_basic_info.arp_id, gport, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC only\n",proc_name);
        return rv;
    }

    printf("PASS, create ingress object FEC only FEC=0x%x\n",cint_l2tp_basic_info.fec_id);
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
    rv = l3__egress_only_fec__create(unit, cint_l2tp_basic_info.fec_id, cint_l2tp_basic_info.intf_out, cint_l2tp_basic_info.arp_id, gport, 0, 0);
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

    rv = l3__egress_only_fec__create(unit, cint_l2tp_basic_info.fec_id, l3_intf_id, 0, gport, 0 ,0);
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
    rv = l3__egress_only_fec__create(unit, cint_pppoe_basic_info.fec_id_enc, l3_intf_id, 0, gport, 0, 0);
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
                        cint_pppoe_basic_info.is_wide,
                        cint_pppoe_basic_info.is_with_id);
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

    rv = l3__egress_only_fec__create(unit, cint_l2tp_basic_info.fec_id, l3_intf_id, 0, gport, 0, 0);
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


