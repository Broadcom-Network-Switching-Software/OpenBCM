/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/* 
 * how to run:
cint;
cint_reset();
exit;
cint ../../../../src/examples/dpp/utility/cint_utils_l3.c 
cint ../../../../src/examples/dpp/cint_ip_route.c
cint ../../../../src/examples/dpp/cint_ip_tunnel_term.c 
cint ../../../../src/examples/dpp/cint_ipv6_tunnel_term.c 
cint
int rv;
verbose = 2; 
rv = ipv6_tunnel_term_example(unit,<in_port>,<out_port>);
 
tunnel no. 1 - from <in_port> run packet: 
    ethernet header with DA 00:0C:00:02:00:00, SA 00:00:07:00:01:00 and Vlan tag id 10
    ipv6 header with DIP ECEF:EEED:ECEB:EAE9:E8E7:E6E5:E4E3:E2E1 and any SIP
    ipv4 header with DIP 1.0.0.17 and SIP 192.128.1.4
the packet be forwarded to <out_port>, after termination of the ipv6 header, and arrive with: 
    ethernet header with DA 00:0C:00:02:00:01, SA 00:0C:00:02:00:00 and Vlan tag id 100
    ipv4 header with DIP 1.0.0.17 and SIP 192.128.1.4
 
tunnel no. 2 - from <in_port> run packet: 
    ethernet header with DA 00:0C:00:02:00:00, SA 00:00:07:00:01:00 and Vlan tag id 10
    ipv6 header with DIP 3555:5555:6666:6666:7777:7777:8888:8888 and any SIP
    ipv4 header with DIP 1.0.0.17 and SIP 192.128.1.5
the packet be forwarded to <out_port>, after termination of the ipv6 header, and arrive with: 
    ethernet header with DA 00:0C:00:02:00:02, SA 00:0C:00:02:00:00 and Vlan tag id 200
    ipv4 header with DIP 1.0.0.17 and SIP 192.128.1.5
*/ 

/* ********* 
  Globals/Aux Variables
 */

/* debug prints */
int verbose = 1;

/********** 
  functions
 */


/******* Run example ******/

struct ip6_tunnel_term_glbl_info_s{
    int tunnel_vrf; /* the router interface due to tunnel termination */
    int rif_vrf;   /* the router interface from RIF (VSI) */
    uint8 plus_1_protection;
    uint8 plus_1_mc_id;
    uint8 skip_ethernet_flag; /* indicate to skip ethernet after terminating ip tunnel. */
    bcm_ip6_t sip;  
    bcm_ip6_t dip;
    bcm_ip6_t sip_mask;
    bcm_ip6_t dip_mask;
    bcm_ip6_t sip2; 
    bcm_ip6_t dip2;
    bcm_ip6_t sip_mask2;
    bcm_ip6_t dip_mask2;
    bcm_ip6_t sip3; 
    bcm_ip6_t dip3;
    bcm_ip6_t sip_mask3;
    bcm_ip6_t dip_mask3;
    bcm_ip6_t sip4; 
    bcm_ip6_t dip4;
    bcm_ip6_t sip_mask4;
    bcm_ip6_t dip_mask4;
    int provider_vlan_1; /* packet using this vlan will be LL terminated. tunnel's vlan */
    int provider_vlan_2; /* packet using this vlan will be LL terminated. tunnel's vlan */
    int provider_vlan_3; /* packet using this vlan will be LL terminated. tunnel's vlan */
    bcm_mac_t my_mac;
    bcm_mac_t access_next_hop;
    bcm_mac_t provider_next_hop;
    int access_vlan_1; /* vlan to forward to access */
    int access_vlan_2; /* vlan to forward to access */
    int tunnel_type_1;
    int tunnel_type_2;
    int tunnel_type_3;
    int tunnel_type_4;
    uint8 cascade_1; /* indicate to whether it is the cascade lookup mode. */
    uint8 cascade_2;
    uint8 cascade_3;
    uint8 cascade_4;
    bcm_ip6_t host_ipv6;
    bcm_ip6_t route_ipv6;
    bcm_ip6_t mask_ipv6;
    int port_property; /* port property */
    int my_vtep_index_1;
    int my_vtep_index_2;
    int my_vtep_index_3;
    int my_vtep_index_4;
    /* BUD node configuration */
    bcm_ip6_t mc_sip; 
    bcm_ip6_t mc_dip;
    bcm_ip6_t mc_sip_mask;
    bcm_ip6_t mc_dip_mask;
    bcm_ip6_t mc_sip2; 
    bcm_ip6_t mc_dip2;
    bcm_ip6_t mc_sip2_mask;
    bcm_ip6_t mc_dip2_mask;
    uint8 mc_cascade_1;
    uint8 mc_cascade_2;
    int mc_my_vtep_index_1;
    int mc_my_vtep_index_2;
};

ip6_tunnel_term_glbl_info_s ip6_tunnel_term_glbl_info =
{
/* tunnel_vrf  | rif_vrf | plus_1_protection | plus_1_mc_id | skip_ethernet_flag */
3,               2,        0,                  6200,          0,
/* dip to terminate 1 */
/*sip*/
{0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x77, 0xFF, 0xFE, 0x00, 0x01, 0x00},
/*dip*/
{0x35, 0x55, 0x55, 0x55, 0x66, 0x66, 0x66, 0x66, 0x77, 0x77, 0x77, 0x77, 0x88, 0x88, 0x88, 0x88},
/*sip_mask */
{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
/*dip_mask */
{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
/* sip + dip to terminate 2 */
/*sip2*/
{0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x77, 0xFF, 0xEE, 0xED, 0xEC, 0xEB},
/*dip2*/
{0xEC, 0xEF, 0xEE, 0xED, 0xEC, 0xEB, 0xEA, 0xE9, 0xE8, 0xE7, 0xE6, 0xE5, 0xE4, 0xE3, 0xE2, 0xE1},
/*sip2_mask */
{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
/*dip2_mask */
{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
/* sip + dip cascade to terminate 3 */
/*sip3*/
{0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x77, 0xFF, 0xDE, 0xDD, 0xDC, 0xDB},
/*dip3*/
{0xDC, 0xDF, 0xDE, 0xDD, 0xDC, 0xDB, 0xDA, 0xD9, 0xD8, 0xD7, 0xD6, 0xD5, 0xD4, 0xD3, 0xD2, 0xD1},
/*sip3_mask */
{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
/*dip3_mask */
{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
/* sip + dip  to terminate 4 */
/*sip4*/
{0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x77, 0xFF, 0xCE, 0xCD, 0xCC, 0xCB},
/*dip4*/
{0xCC, 0xCF, 0xCE, 0xCD, 0xCC, 0xCB, 0xCA, 0xC9, 0xC8, 0xC7, 0xC6, 0xC5, 0xC4, 0xC3, 0xC2, 0xC1},
/*sip4_mask */
{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
/*dip4_mask */
{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
/* provider vlans 1 | provider vlan 2 | provider vlan 3 */
10,                   20,               30,
/* my mac */
{0x00, 0x0c, 0x00, 0x02, 0x00, 0x00},
/*access next hop                    |   provider_next_hop*/
{0x00, 0x00, 0x00, 0x0, 0x12, 0x34},   {0x00, 0x00, 0x00, 0x0, 0x45, 0x67},
/* access valn 1 | access vlan 2 */
100,                200,
/*tunnel_type_1,         tunnel_type_2,             tunnel_type_3,          tunnel_type_4      */
bcmTunnelTypeIpAnyIn6,     bcmTunnelTypeIpAnyIn6,     bcmTunnelTypeIpAnyIn6,    bcmTunnelTypeIpAnyIn6,
/*cascade_1,         cascade_2,             cascade_3,          cascade_4      */
0,     0,     0,    0,
/* create Ipv4 and Ipv6 hosts and routes*/
{0xAA, 0xAA, 0xAA, 0xAA, 0xBB, 0xBB, 0xBB, 0xBB, 0xCC, 0xCC, 0xCC, 0xCC, 0xDD, 0xDD, 0xDD, 0xDD},
/* route IPv6 entry */
{0x11, 0x11, 0x11, 0x11, 0x22, 0x22, 0x22, 0x22, 0x33, 0x33, 0x33, 0x33, 0x44, 0x44, 0x44, 0x44},
/* route IPv6 mask */
{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
/* port property */
9,
/* my_vtep_index */
10, 9, 8, 7,
/* mc_sip */
{0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x77, 0xFF, 0xBE, 0xBD, 0xBC, 0xBB},
/* mc_dip */
{0xFF, 0x02, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE},
/* mc_sip_mask */
{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
/* mc_dip_mask */
{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
/* mc_sip */
{0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x77, 0xFF, 0xAE, 0xAD, 0xAC, 0xAB},
/* mc_dip */
{0xFF, 0x02, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xFF},
/* mc_sip_mask */
{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
/* mc_dip_mask */
{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
/*mc_cascade_1,  mc_cascade_2 */
0, 1,
/* mc_my_vtep_index_1, mc_my_vtep_index_2 */
3, 2
};


int tunnel_term_ipany_greany_ipv6(int unit, int in_port, int out_port){
    int rv;
    bcm_if_t eg_intf_ids[2]; /* to include egress router interfaces */
    bcm_gport_t in_tunnel_gports[2]; /* to include IP tunnel interfaces interfaces */
    bcm_if_t ll_encap_id;
    ip_tunnel_term_glbl_info.tunnel_type_1 = bcmTunnelTypeIpAnyIn6;
    ip_tunnel_term_glbl_info.tunnel_type_2 = bcmTunnelTypeIpAnyIn6;
    /*** build tunnel terminations ***/
    rv = ipv6_tunnel_term_build_2_tunnels(unit,in_port,out_port,eg_intf_ids,in_tunnel_gports, &ll_encap_id); 
    if (rv != BCM_E_NONE) {
        printf("Error, ipv6_tunnel_term_build_2_tunnels, in_port=%d, \n", in_port);
    }

    /*** add routes to egress interfaces ***/
    rv = ipv4_tunnel_term_add_routes(unit,eg_intf_ids, ll_encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, ipv4_tunnel_term_add_routes, eg_intf_ids=0x%08x,%0x%08x \n", eg_intf_ids[0],eg_intf_ids[1]);
    }

    return rv;
}
 
int ipv6_tunnel_term_build_2_tunnels(int unit, int in_port, int out_port,bcm_if_t *eg_intf_ids, bcm_gport_t *in_tunnel_gports, bcm_if_t *ll_encap_id){
    int rv;

    int tunnel_vrf = ip6_tunnel_term_glbl_info.tunnel_vrf; /* the router interface due to tunnel termination */
    int rif_vrf = ip6_tunnel_term_glbl_info.rif_vrf;   /* the router interface from RIF (VSI) */
    int tunnel_rif; /* RIF from tunnel termination*/
    int port_property = ip6_tunnel_term_glbl_info.port_property;

    int egress_intf_1; /* egress object 1*/
    int egress_intf_2; /* egress object 2*/    

    bcm_tunnel_terminator_config_key_t tunnel_term_config_key_1, tunnel_term_config_key_2;
    bcm_tunnel_terminator_config_action_t tunnel_term_config_action_1, tunnel_term_config_action_2;
    bcm_tunnel_terminator_t tunnel_term_1, tunnel_term_2;    
    
    /*** build router interface ***/
    rv = ip_tunnel_term_open_route_interfaces(unit, in_port, out_port, rif_vrf, tunnel_vrf, &tunnel_rif, &egress_intf_1, &egress_intf_2, ll_encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, cip_tunnel_term_open_route_interfaces, in_port=%d, \n", in_port);
    }

    /* store egress interfaces, for routing */
    eg_intf_ids[0] = egress_intf_1;
    eg_intf_ids[1] = egress_intf_2;

    /* create IP tunnel terminators */
    /*** create IP tunnel terminator 1 ***/
    /* Cascade lookup when cascade_1 is 1 */
    if (ip6_tunnel_term_glbl_info.cascade_1) {
        bcm_tunnel_terminator_config_key_t_init(&tunnel_term_config_key_1);
        bcm_tunnel_terminator_config_action_t_init(&tunnel_term_config_action_1);
        /* key is DIP6, IP.Next-Protocol, Qualifier2.Next-Protocol */
        sal_memcpy(tunnel_term_config_key_1.dip6, ip6_tunnel_term_glbl_info.dip, 16);
        tunnel_term_config_key_1.type = ip6_tunnel_term_glbl_info.tunnel_type_1;
        tunnel_term_config_action_1.tunnel_class = ip6_tunnel_term_glbl_info.my_vtep_index_1;
        rv = bcm_tunnel_terminator_config_add(unit, 0, &tunnel_term_config_key_1, &tunnel_term_config_action_1);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_tunnel_terminator_config_add, in_port=%d, \n", in_port);
        }
       
        bcm_tunnel_terminator_t_init(&tunnel_term_1);
        /* key is SIP6, VRF, Compressed-Dest, Port-Property */
        sal_memcpy(tunnel_term_1.sip6, ip6_tunnel_term_glbl_info.sip, 16);
        sal_memcpy(tunnel_term_1.sip6_mask, ip6_tunnel_term_glbl_info.sip_mask, 16);
        tunnel_term_1.tunnel_if = tunnel_rif; 
        tunnel_term_1.type = ip6_tunnel_term_glbl_info.tunnel_type_1;/* this is IPv4 termination */
        tunnel_term_1.vrf = ip6_tunnel_term_glbl_info.rif_vrf;
        tunnel_term_1.flags |= BCM_TUNNEL_TERM_USE_TUNNEL_CLASS; /* indicate tunnel_class is a valid value */
        tunnel_term_1.tunnel_class = ip6_tunnel_term_glbl_info.my_vtep_index_1;
        BCM_PBMP_PORT_SET(tunnel_term_1.pbmp, port_property); 
    } else {        
        bcm_tunnel_terminator_t_init(&tunnel_term_1);
        /* key is DIP6 */
        sal_memcpy(tunnel_term_1.dip6, ip6_tunnel_term_glbl_info.dip, 16);
        sal_memcpy(tunnel_term_1.dip6_mask, ip6_tunnel_term_glbl_info.dip_mask, 16);
        tunnel_term_1.tunnel_if = tunnel_rif;
        tunnel_term_1.type = ip6_tunnel_term_glbl_info.tunnel_type_1; /* this is IPv6 termination */
        tunnel_term_1.vrf = ip6_tunnel_term_glbl_info.rif_vrf;
        BCM_PBMP_PORT_SET(tunnel_term_1.pbmp, port_property); 
    }

    rv = bcm_tunnel_terminator_create(unit,&tunnel_term_1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_terminator_create_1, in_port=%d, \n", in_port);
    }
    if(verbose >= 1) {
        printf("created tunnel terminator_1 =0x%08x, \n", tunnel_term_1.tunnel_id);
        ip_tunnel_term_print_key("created term 1",&tunnel_term_1);
    }

    in_tunnel_gports[0] = tunnel_term_1.tunnel_id;
    
    /*** create IP tunnel terminator 2 ***/
    /* Cascade lookup when ipv6_tunnel_term_mode is 2 */
    if (ip6_tunnel_term_glbl_info.cascade_2) {
        bcm_tunnel_terminator_config_key_t_init(&tunnel_term_config_key_2);
        bcm_tunnel_terminator_config_action_t_init(&tunnel_term_config_action_2);
        /* key is DIP6, IP.Next-Protocol, Qualifier2.Next-Protocol */
        sal_memcpy(tunnel_term_config_key_2.dip6, ip6_tunnel_term_glbl_info.dip2, 16);
        tunnel_term_config_key_2.type = ip6_tunnel_term_glbl_info.tunnel_type_2;
        tunnel_term_config_action_2.tunnel_class = ip6_tunnel_term_glbl_info.my_vtep_index_2;
        rv = bcm_tunnel_terminator_config_add(unit, 0, &tunnel_term_config_key_2, &tunnel_term_config_action_2);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_tunnel_terminator_config_add, in_port=%d, \n", in_port);
        }
       
        bcm_tunnel_terminator_t_init(&tunnel_term_2);
        /* key is SIP6, VRF, Compressed-Dest, Port-Property */
        sal_memcpy(tunnel_term_2.sip6, ip6_tunnel_term_glbl_info.sip2, 16);
        sal_memcpy(tunnel_term_2.sip6_mask, ip6_tunnel_term_glbl_info.sip_mask2, 16);
        tunnel_term_2.tunnel_if = -1; /* means don't overwite RIF */
        tunnel_term_2.type = ip6_tunnel_term_glbl_info.tunnel_type_2;/* this is IPv4 termination */
        tunnel_term_2.vrf = ip6_tunnel_term_glbl_info.rif_vrf;
        tunnel_term_2.flags |= BCM_TUNNEL_TERM_USE_TUNNEL_CLASS; /* indicate tunnel_class is a valid value */
        tunnel_term_2.tunnel_class = ip6_tunnel_term_glbl_info.my_vtep_index_2;
        BCM_PBMP_PORT_SET(tunnel_term_2.pbmp, port_property); 
    } else {        
        bcm_tunnel_terminator_t_init(&tunnel_term_2);
        /* key is DIP6 */
        sal_memcpy(tunnel_term_2.dip6, ip6_tunnel_term_glbl_info.dip2, 16);
        sal_memcpy(tunnel_term_2.dip6_mask, ip6_tunnel_term_glbl_info.dip_mask2, 16);
        tunnel_term_2.tunnel_if = -1; /* means don't overwite RIF */
        tunnel_term_2.type = ip6_tunnel_term_glbl_info.tunnel_type_2;/* this is IPv4 termination */
        tunnel_term_2.vrf = ip6_tunnel_term_glbl_info.rif_vrf;
        BCM_PBMP_PORT_SET(tunnel_term_2.pbmp, port_property); 
    }

    rv = bcm_tunnel_terminator_create(unit,&tunnel_term_2);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_terminator_create_2, in_port=%d, \n", in_port);
    }
    if(verbose >= 1) {
        printf("created tunnel terminator_2 =0x%08x, \n", tunnel_term_2.tunnel_id);
        ip_tunnel_term_print_key("created term 2",&tunnel_term_2);
    }
    in_tunnel_gports[1] = tunnel_term_2.tunnel_id;

    return rv;
}

int ipv6_tunnel_term_build_3_tunnels(int unit, int in_port, int out_port,bcm_if_t *eg_intf_ids, bcm_gport_t *in_tunnel_gports, bcm_if_t *ll_encap_id){
    int rv;

    int tunnel_vrf = ip6_tunnel_term_glbl_info.tunnel_vrf; /* the router interface due to tunnel termination */
    int rif_vrf = ip6_tunnel_term_glbl_info.rif_vrf;   /* the router interface from RIF (VSI) */
    int tunnel_rif; /* RIF from tunnel termination*/
    int port_property = ip6_tunnel_term_glbl_info.port_property;

    int egress_intf_1; /* egress object 1*/
    int egress_intf_2; /* egress object 2*/    

    bcm_tunnel_terminator_config_key_t tunnel_term_config_key_1, tunnel_term_config_key_2, tunnel_term_config_key_3;
    bcm_tunnel_terminator_config_action_t tunnel_term_config_action_1, tunnel_term_config_action_2, tunnel_term_config_action_3;
    bcm_tunnel_terminator_t tunnel_term_1, tunnel_term_2, tunnel_term_3;  
    
    int ipv6_tunnel_term_mode;

    ipv6_tunnel_term_mode = soc_property_get(unit, "bcm886xx_ip6_tunnel_termination_mode", 1);

    if (ipv6_tunnel_term_mode == 2) { 
        /* Configure the port to support the IPv6 tunnel cascade lookup: DIP+SIP */
        rv = bcm_port_control_set(unit, in_port, bcmPortControlIPv6TerminationCascadedModeEnable, 1); 
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_control_set, in_port=%d, \n", in_port);
        }
        rv = bcm_port_class_set(unit, in_port, bcmPortClassFieldIngressTunnelTerminated, port_property); 
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_class_set, in_port=%d, \n", in_port);
        }
    }
    
    /*** build router interface ***/
    rv = ip_tunnel_term_open_route_interfaces(unit, in_port, out_port, rif_vrf, tunnel_vrf, &tunnel_rif, &egress_intf_1, &egress_intf_2, ll_encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, cip_tunnel_term_open_route_interfaces, in_port=%d, \n", in_port);
    }

    /* store egress interfaces, for routing */
    eg_intf_ids[0] = egress_intf_1;
    eg_intf_ids[1] = egress_intf_2;

    /*** create IP tunnel terminator 1 ***/
    /* Cascade lookup when cascade_1 is 1 */
    if (ip6_tunnel_term_glbl_info.cascade_1) {
        bcm_tunnel_terminator_config_key_t_init(&tunnel_term_config_key_1);
        bcm_tunnel_terminator_config_action_t_init(&tunnel_term_config_action_1);
        /* key is DIP6, IP.Next-Protocol, Qualifier2.Next-Protocol */
        sal_memcpy(tunnel_term_config_key_1.dip6, ip6_tunnel_term_glbl_info.dip, 16);
        tunnel_term_config_key_1.type = ip6_tunnel_term_glbl_info.tunnel_type_1;
        tunnel_term_config_action_1.tunnel_class = ip6_tunnel_term_glbl_info.my_vtep_index_1;
        rv = bcm_tunnel_terminator_config_add(unit, 0, &tunnel_term_config_key_1, &tunnel_term_config_action_1);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_tunnel_terminator_config_add, in_port=%d, \n", in_port);
        }
       
        bcm_tunnel_terminator_t_init(&tunnel_term_1);
        /* key is SIP6, VRF, Compressed-Dest, Port-Property */
        sal_memcpy(tunnel_term_1.sip6, ip6_tunnel_term_glbl_info.sip, 16);
        sal_memcpy(tunnel_term_1.sip6_mask, ip6_tunnel_term_glbl_info.sip_mask, 16);
        tunnel_term_1.tunnel_if = tunnel_rif; 
        tunnel_term_1.type = ip6_tunnel_term_glbl_info.tunnel_type_1;/* this is IPv4 termination */
        tunnel_term_1.vrf = ip6_tunnel_term_glbl_info.rif_vrf;
        tunnel_term_1.flags |= BCM_TUNNEL_TERM_USE_TUNNEL_CLASS; /* indicate tunnel_class is a valid value */
        tunnel_term_1.tunnel_class = ip6_tunnel_term_glbl_info.my_vtep_index_1;
        BCM_PBMP_PORT_SET(tunnel_term_1.pbmp, port_property); 
    } else {        
        bcm_tunnel_terminator_t_init(&tunnel_term_1);
        /* key is DIP6 */
        sal_memcpy(tunnel_term_1.dip6, ip6_tunnel_term_glbl_info.dip, 16);
        sal_memcpy(tunnel_term_1.dip6_mask, ip6_tunnel_term_glbl_info.dip_mask, 16);
        tunnel_term_1.tunnel_if = tunnel_rif;
        tunnel_term_1.type = ip6_tunnel_term_glbl_info.tunnel_type_1; /* this is IPv6 termination */
        tunnel_term_1.vrf = ip6_tunnel_term_glbl_info.rif_vrf;
        BCM_PBMP_PORT_SET(tunnel_term_1.pbmp, port_property); 
    }

    rv = bcm_tunnel_terminator_create(unit,&tunnel_term_1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_terminator_create_1, in_port=%d, \n", in_port);
    }
    if(verbose >= 1) {
        printf("created tunnel terminator_1 =0x%08x, \n", tunnel_term_1.tunnel_id);
        ip_tunnel_term_print_key("created term 1",&tunnel_term_1);
    }

    in_tunnel_gports[0] = tunnel_term_1.tunnel_id;
    
    /*** create IP tunnel terminator 2 ***/
    /* Cascade lookup when cascade_2 is 1 */
    if (ip6_tunnel_term_glbl_info.cascade_2) {
        bcm_tunnel_terminator_config_key_t_init(&tunnel_term_config_key_2);
        bcm_tunnel_terminator_config_action_t_init(&tunnel_term_config_action_2);
        /* key is DIP6, IP.Next-Protocol, Qualifier2.Next-Protocol */
        sal_memcpy(tunnel_term_config_key_2.dip6, ip6_tunnel_term_glbl_info.dip2, 16);
        tunnel_term_config_key_2.type = ip6_tunnel_term_glbl_info.tunnel_type_2;
        tunnel_term_config_action_2.tunnel_class = ip6_tunnel_term_glbl_info.my_vtep_index_2;
        rv = bcm_tunnel_terminator_config_add(unit, 0, &tunnel_term_config_key_2, &tunnel_term_config_action_2);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_tunnel_terminator_config_add, in_port=%d, \n", in_port);
        }
       
        bcm_tunnel_terminator_t_init(&tunnel_term_2);
        /* key is SIP6, VRF, Compressed-Dest, Port-Property */
        sal_memcpy(tunnel_term_2.sip6, ip6_tunnel_term_glbl_info.sip2, 16);
        sal_memcpy(tunnel_term_2.sip6_mask, ip6_tunnel_term_glbl_info.sip_mask2, 16);
        tunnel_term_2.tunnel_if = tunnel_rif; 
        tunnel_term_2.type = ip6_tunnel_term_glbl_info.tunnel_type_2;/* this is IPv4 termination */
        tunnel_term_2.vrf = ip6_tunnel_term_glbl_info.rif_vrf;
        tunnel_term_2.flags |= BCM_TUNNEL_TERM_USE_TUNNEL_CLASS; /* indicate tunnel_class is a valid value */
        tunnel_term_2.tunnel_class = ip6_tunnel_term_glbl_info.my_vtep_index_2;
        BCM_PBMP_PORT_SET(tunnel_term_2.pbmp, port_property); 
    } else {        
        bcm_tunnel_terminator_t_init(&tunnel_term_2);
        /* key is DIP6 */
        sal_memcpy(tunnel_term_2.dip6, ip6_tunnel_term_glbl_info.dip2, 16);
        sal_memcpy(tunnel_term_2.dip6_mask, ip6_tunnel_term_glbl_info.dip_mask2, 16);
        tunnel_term_2.tunnel_if = tunnel_rif;
        tunnel_term_2.type = ip6_tunnel_term_glbl_info.tunnel_type_2; /* this is IPv6 termination */
        tunnel_term_2.vrf = ip6_tunnel_term_glbl_info.rif_vrf;
        BCM_PBMP_PORT_SET(tunnel_term_2.pbmp, port_property); 
    }

    rv = bcm_tunnel_terminator_create(unit,&tunnel_term_2);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_terminator_create_2, in_port=%d, \n", in_port);
    }
    if(verbose >= 1) {
        printf("created tunnel terminator_2 =0x%08x, \n", tunnel_term_2.tunnel_id);
        ip_tunnel_term_print_key("created term 2",&tunnel_term_2);
    }
    in_tunnel_gports[1] = tunnel_term_2.tunnel_id;


    /*** create IP tunnel terminator 3 ***/
    /* Cascade lookup when cascade_3 is 1 */
    if (ip6_tunnel_term_glbl_info.cascade_3) {
        bcm_tunnel_terminator_config_key_t_init(&tunnel_term_config_key_3);
        bcm_tunnel_terminator_config_action_t_init(&tunnel_term_config_action_3);
        /* key is DIP6, IP.Next-Protocol, Qualifier2.Next-Protocol */
        sal_memcpy(tunnel_term_config_key_3.dip6, ip6_tunnel_term_glbl_info.dip3, 16);
        tunnel_term_config_key_3.type = ip6_tunnel_term_glbl_info.tunnel_type_3;
        tunnel_term_config_action_3.tunnel_class = ip6_tunnel_term_glbl_info.my_vtep_index_3;
        rv = bcm_tunnel_terminator_config_add(unit, 0, &tunnel_term_config_key_3, &tunnel_term_config_action_3);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_tunnel_terminator_config_add, in_port=%d, \n", in_port);
        }
       
        bcm_tunnel_terminator_t_init(&tunnel_term_3);
        /* key is SIP6, VRF, Compressed-Dest, Port-Property */
        sal_memcpy(tunnel_term_3.sip6, ip6_tunnel_term_glbl_info.sip3, 16);
        sal_memcpy(tunnel_term_3.sip6_mask, ip6_tunnel_term_glbl_info.sip_mask3, 16);
        tunnel_term_3.tunnel_if = tunnel_rif; 
        tunnel_term_3.type = ip6_tunnel_term_glbl_info.tunnel_type_3;/* this is IPv4 termination */
        tunnel_term_3.vrf = ip6_tunnel_term_glbl_info.rif_vrf;
        tunnel_term_3.flags |= BCM_TUNNEL_TERM_USE_TUNNEL_CLASS; /* indicate tunnel_class is a valid value */
        tunnel_term_3.tunnel_class = ip6_tunnel_term_glbl_info.my_vtep_index_3;
        BCM_PBMP_PORT_SET(tunnel_term_3.pbmp, port_property); 
    } else {        
        bcm_tunnel_terminator_t_init(&tunnel_term_3);
        /* key is DIP6 */
        sal_memcpy(tunnel_term_3.dip6, ip6_tunnel_term_glbl_info.dip3, 16);
        sal_memcpy(tunnel_term_3.dip6_mask, ip6_tunnel_term_glbl_info.dip_mask3, 16);
        tunnel_term_3.tunnel_if = tunnel_rif;
        tunnel_term_3.type = ip6_tunnel_term_glbl_info.tunnel_type_3; /* this is IPv6 termination */
        tunnel_term_3.vrf = ip6_tunnel_term_glbl_info.rif_vrf;
        BCM_PBMP_PORT_SET(tunnel_term_3.pbmp, port_property); 
    }

    rv = bcm_tunnel_terminator_create(unit,&tunnel_term_3);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_terminator_create_3, in_port=%d, \n", in_port);
    }
    if(verbose >= 1) {
        printf("created tunnel terminator_3 =0x%08x, \n", tunnel_term_3.tunnel_id);
        ip_tunnel_term_print_key("created term 3",&tunnel_term_3);
    }
    in_tunnel_gports[2] = tunnel_term_3.tunnel_id;

    return rv;
}

 
 
/*
 * buid two IP tunnels with following information:
 * Tunnnel 1: 
 *   -  dscp  = 10;
 *   -  ttl   = 60;
 *   -  type  = bcmTunnelTypeIp4In4;
 *   -  Vlan  = 100
 *   -  Dmac  = 00:00:00:00:cd:1d
 *   -  Smac  = 00:0c:00:02:00:00
 *   + No Fec defined over this tunnel
 *  
 * Tunnnel 2: 
 *   -  dscp  = 11;
 *   -  ttl   = 50;
 *   -  type  = bcmTunnelTypeGre4In4;
 *   -  Vlan  = 100
 *   -  Dmac  = 20:00:00:00:cd:1d
 *   -  Smac  = 00:0c:00:02:00:00
 *   + Define FEC point to this tunnel
 *  
 *   returned value:
 *      intf_ids[] : array includes the interface-id to forward to the built tunnels.
 *         intf_ids[0] : is IP-tunnel Id.
 *         intf_ids[1] : is egress-object (FEC) points to the IP-tunnel
 */
int ipv6_tunnel_term_build_tunnels(int unit, int out_port, int* intf_ids){
    int rv;
    int ing_intf_in;
    int ing_intf_out; 
    int fec[2] = {0x0,0x0};
    int flags;
    int out_vlan = 100;
    int encap_id[2]={0x0,0x0};
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  /* my-MAC */
    bcm_tunnel_initiator_t tunnel_1;


    /* tunnel 1 info*/
    int dscp1 = 0; /* not supported for IPv6 */
    int flow_label1 = 0x53; 
    int ttl1 = 60; 
    int type1 = bcmTunnelTypeIp4In6; /*possible types Ip4In6 and Ip6In6 */
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */
    int tunnel_itf1=0;
    bcm_ip6_t sip1;
    bcm_ip6_t dip1;
    /* tunnel 2 info */
    bcm_tunnel_initiator_t tunnel_2;
    int dscp2 = 0; /* not supported for IPv6 */
    int flow_label2 = 0x77;
    int ttl2 = 50;
    int type2= bcmTunnelTypeIp4In6;
    bcm_mac_t next_hop_mac2  = {0x20, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */
    int tunnel_itf2=0;
    bcm_ip6_t sip2;
    bcm_ip6_t dip2;


    sip1[15]= 0x01;    sip1[14]= 0x02;    sip1[13]= 0x03;    sip1[12]= 0x04;
    sip1[11]= 0x05;    sip1[10]= 0x06;    sip1[9] = 0x07;    sip1[8] = 0x08;
    sip1[7] = 0x09;    sip1[6] = 0x0a;    sip1[5] = 0x0b;    sip1[4] = 0x0c;
    sip1[3] = 0x0d;    sip1[2] = 0x0e;    sip1[1] = 0x0f;    sip1[0] = 0xfc;
  
    dip1[15]= 0xa1;    dip1[14]= 0xa2;    dip1[13]= 0xa3;    dip1[12]= 0xa4;
    dip1[11]= 0xa5;    dip1[10]= 0xa6;    dip1[9] = 0xa7;    dip1[8] = 0xa8;
    dip1[7] = 0xa9;    dip1[6] = 0xaa;    dip1[5] = 0xab;    dip1[4] = 0xac;
    dip1[3] = 0xad;    dip1[2] = 0xae;    dip1[1] = 0xaf;    dip1[0] = 0xac;
  
    sip2[15]= 0xc1;    sip2[14]= 0xc2;    sip2[13]= 0xc3;    sip2[12]= 0xc4;
    sip2[11]= 0xc5;    sip2[10]= 0xc6;    sip2[9] = 0xc7;    sip2[8] = 0xc8;
    sip2[7] = 0xc9;    sip2[6] = 0xca;    sip2[5] = 0xcb;    sip2[4] = 0xcc;
    sip2[3] = 0xcd;    sip2[2] = 0xce;    sip2[1] = 0xcf;    sip2[0] = 0xcc;
  
    dip2[15]= 0xe1;    dip2[14]= 0xe2;    dip2[13]= 0xe3;    dip2[12]= 0xe4;
    dip2[11]= 0xe5;    dip2[10]= 0xe6;    dip2[9] = 0xe7;    dip2[8] = 0xe8;
    dip2[7] = 0xe9;    dip2[6] = 0xea;    dip2[5] = 0xeb;    dip2[4] = 0xec;
    dip2[3] = 0xed;    dip2[2] = 0xee;    dip2[1] = 0xef;    dip2[0] = 0xec;
  
    /*** create egress router interface ***/
    rv = vlan__open_vlan_per_mc(unit, out_vlan, 0x1);  
    if (rv != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", out_vlan, unit);
    }
    rv = bcm_vlan_gport_add(unit, out_vlan, out_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", out_port, out_vlan);
      return rv;
    }

    create_l3_intf_s intf;
    intf.vsi = out_vlan;
    intf.my_global_mac = mac_address;
    intf.my_lsb_mac = mac_address;
    
    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_out = intf.rif;        
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }
    
    /*** create IP tunnel 1 ***/
    bcm_tunnel_initiator_t_init(&tunnel_1);

    sal_memcpy(&(tunnel_1.dip6),&(dip1),16);
    sal_memcpy(&(tunnel_1.sip6),&(sip1),16);
    tunnel_1.flags = 0;
    tunnel_1.ttl = ttl1;
    tunnel_1.flow_label = flow_label1;
    tunnel_1.type = type1;
    tunnel_1.l3_intf_id = ing_intf_out;
    tunnel_itf1 = 0;
    rv = add_ip_tunnel(unit,&tunnel_itf1,&tunnel_1);
    if (rv != BCM_E_NONE) {
        printf("Error, add_ip_tunnel 1\n");
    }
    if(verbose >= 1) {
        printf("created IP tunnel_1 on intf:0x%08x \n",tunnel_itf1);
    }

    /*** create tunnel 2 ***/
    bcm_tunnel_initiator_t_init(&tunnel_2);
    sal_memcpy(&(tunnel_2.dip6),&(dip2),16);
    sal_memcpy(&(tunnel_2.sip6),&(sip2),16);
    tunnel_2.dscp = dscp2;
    tunnel_2.flags = 0;
    tunnel_2.ttl = ttl2;
    tunnel_2.flow_label = flow_label2;
    tunnel_2.type = type2;
    tunnel_2.l3_intf_id = ing_intf_out;
    tunnel_itf2 = 0;
    rv = add_ip_tunnel(unit,&tunnel_itf2,&tunnel_2);
    if (rv != BCM_E_NONE) {
        printf("Error, add_ip_tunnel 2\n");
    }
    if(verbose >= 1) {
        printf("created IP tunnel_2 on intf:0x%08x \n",tunnel_itf2);
    }

    /*** using egress object API set MAC address for tunnel 1 interface, without allocating FEC enty ***/
    create_l3_egress_s l3eg;
    sal_memcpy(l3eg.next_hop_mac_addr, next_hop_mac , 6);
    l3eg.out_tunnel_or_rif = tunnel_itf1;
    l3eg.vlan = out_vlan;
    l3eg.arp_encap_id = encap_id[0];

    rv = l3__egress_only_encap__create(unit,&l3eg);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress_only_encap__create\n");
        return rv;
    }

    encap_id[0] = l3eg.arp_encap_id;
    if(verbose >= 1) {
        printf("no FEC is allocated FEC-id =0x%08x, \n", fec[0]);
        printf("next hop mac at encap-id 0x%08x, \n", encap_id[0]);
    }

    /*** create egress object 2: points into tunnel 2, with allocating FEC, and da-mac = next_hop_mac2  ***/
    create_l3_egress_s l3eg;
    sal_memcpy(l3eg.next_hop_mac_addr, next_hop_mac2 , 6);
    l3eg.out_tunnel_or_rif = tunnel_itf2;
    l3eg.fec_id = fec[1];
    l3eg.arp_encap_id = encap_id[1];

    rv = l3__egress__create(unit,&l3eg);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
        return rv;
    }

    fec[1] = l3eg.fec_id;
    encap_id[1] = l3eg.arp_encap_id;
    if(verbose >= 1) {
        printf("created FEC-id =0x%08x, \n", fec[1]);
        printf("next hop mac at encap-id %08x, \n", encap_id[1]);
    }

   /* interface for tunnel_1 is IP-tunnel ID */
    intf_ids[0] = tunnel_itf1;

    /* interface for tunnel_2 is egress-object (FEC) */
    intf_ids[1] = fec[1];

    return rv;
}

int ipv6_tunnel_term_add_routes(int unit, int *eg_intf_ids, int eg_encap_id){
    int rv = BCM_E_NONE;

    /* internal IP after termination */
    int inter_subnet = 0x01000011; /* 1.0.0.17*/
    int inter_mask =    0xffffff00; /* 255.255.255.0*/
    int route_address = 0x03000011;
    int subnet_address = 0xAC000011;
    int egress_intf_1; /* egress object 1*/
    int egress_intf_2; /* egress object 2*/
    int egress_encap_id;
    int tunnel_vrf = ip6_tunnel_term_glbl_info.tunnel_vrf; /* the router interface due to tunnel termination */
    int rif_vrf = ip6_tunnel_term_glbl_info.rif_vrf;   /* the router interface from RIF (VSI) */
    bcm_ip6_t route_ipv6 = ip6_tunnel_term_glbl_info.route_ipv6;
    bcm_ip6_t mask_ipv6 = ip6_tunnel_term_glbl_info.mask_ipv6;
    bcm_ip6_t host_ipv6 = ip6_tunnel_term_glbl_info.host_ipv6;
    
    egress_intf_2 = eg_intf_ids[0];
    egress_intf_1 = eg_intf_ids[1];    
    egress_encap_id = eg_encap_id; 
    
    rv = add_route(unit, inter_subnet, inter_mask, tunnel_vrf, egress_intf_1);
    if (rv != BCM_E_NONE) {
        printf("Error, add_route 1, egress_intf_1=%d, \n", egress_intf_1);
    }
    
    rv = add_route(unit, route_address, inter_mask, tunnel_vrf, egress_intf_2); 
    if (rv != BCM_E_NONE) {
        printf("Error, add_route , egress_intf_2=%d, \n", egress_intf_2);
    }
    
    rv = add_route_ip6(unit, route_ipv6, mask_ipv6, tunnel_vrf, egress_intf_2);
    rv = add_ipv6_host(unit, host_ipv6, tunnel_vrf, egress_intf_1, egress_encap_id); 
    if (rv != BCM_E_NONE) {
        printf("Error,  add ipv6 host \n");
    }

    rv = add_route(unit, subnet_address, inter_mask, rif_vrf, egress_intf_2);
    if (rv != BCM_E_NONE) {
        printf("Error, add_route , egress_intf_2=%d, \n", egress_intf_2);
    }
    
    /* add subnet point to created egress object 1 */
    rv = add_route(unit,inter_subnet,inter_mask,rif_vrf, egress_intf_2);
    if (rv != BCM_E_NONE) {
        printf("Error, add_route 1, egress_intf_2=%d, \n", egress_intf_2);
    }
    rv = add_ipv6_host(unit, host_ipv6, rif_vrf, egress_intf_1, egress_encap_id); 
    if (rv != BCM_E_NONE) {
        printf("Error,  add ipv6 host \n");
    }
    rv = add_route_ip6(unit, route_ipv6, mask_ipv6, rif_vrf, egress_intf_1);
    return rv;
}

int ipv6_tunnel_term_build_bud_tunnels(int unit, int in_port, int out_port, bcm_gport_t *in_tunnel_gports){
    int rv;
    int port_property = ip6_tunnel_term_glbl_info.port_property;
    bcm_tunnel_terminator_config_key_t tunnel_term_config_key_bud_1, tunnel_term_config_key_bud_2;
    bcm_tunnel_terminator_config_action_t tunnel_term_config_action_bud_1, tunnel_term_config_action_bud_2;
    bcm_tunnel_terminator_t tunnel_term_bud_1, tunnel_term_bud_2;    
    

    /* create IP tunnel terminators */
    /*** create IP tunnel terminator 1 ***/
    /* Cascade lookup when cascade_1 is 1 */
    if (ip6_tunnel_term_glbl_info.mc_cascade_1) {
        bcm_tunnel_terminator_config_key_t_init(&tunnel_term_config_key_bud_1);
        bcm_tunnel_terminator_config_action_t_init(&tunnel_term_config_action_bud_1);
        /* key is DIP6, IP.Next-Protocol, Qualifier2.Next-Protocol */
        sal_memcpy(tunnel_term_config_key_bud_1.dip6, ip6_tunnel_term_glbl_info.mc_dip, 16);
        tunnel_term_config_key_bud_1.type = ip6_tunnel_term_glbl_info.tunnel_type_1;        
        tunnel_term_config_key_bud_1.flags |= BCM_TUNNEL_TERM_BUD;
        tunnel_term_config_action_bud_1.tunnel_class = ip6_tunnel_term_glbl_info.mc_my_vtep_index_1;
        rv = bcm_tunnel_terminator_config_add(unit, 0, &tunnel_term_config_key_bud_1, &tunnel_term_config_action_bud_1);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_tunnel_terminator_config_add, in_port=%d, \n", in_port);
        }
       
        bcm_tunnel_terminator_t_init(&tunnel_term_bud_1);
        /* key is SIP6, VRF, Compressed-Dest, Port-Property */
        sal_memcpy(tunnel_term_bud_1.sip6, ip6_tunnel_term_glbl_info.mc_sip, 16);
        sal_memcpy(tunnel_term_bud_1.sip6_mask, ip6_tunnel_term_glbl_info.mc_sip_mask, 16);
        tunnel_term_bud_1.tunnel_if = 0; /* ? */
        tunnel_term_bud_1.type = ip6_tunnel_term_glbl_info.tunnel_type_1;/* this is IPv4 termination */
        tunnel_term_bud_1.vrf = ip6_tunnel_term_glbl_info.rif_vrf;
        tunnel_term_bud_1.flags |= BCM_TUNNEL_TERM_USE_TUNNEL_CLASS; /* indicate tunnel_class is a valid value */
        tunnel_term_bud_1.tunnel_class = ip6_tunnel_term_glbl_info.mc_my_vtep_index_1;
        BCM_PBMP_PORT_SET(tunnel_term_bud_1.pbmp, port_property); 
    } else {        
        bcm_tunnel_terminator_t_init(&tunnel_term_bud_1);
        /* key is DIP6 */
        sal_memcpy(tunnel_term_bud_1.dip6, ip6_tunnel_term_glbl_info.mc_dip, 16);
        sal_memcpy(tunnel_term_bud_1.dip6_mask, ip6_tunnel_term_glbl_info.mc_dip_mask, 16);
        tunnel_term_bud_1.tunnel_if = 0; /* ? */
        tunnel_term_bud_1.type = ip6_tunnel_term_glbl_info.tunnel_type_1; /* this is IPv6 termination */
        tunnel_term_bud_1.vrf = ip6_tunnel_term_glbl_info.rif_vrf;
        tunnel_term_bud_1.flags |= BCM_TUNNEL_TERM_BUD;
        BCM_PBMP_PORT_SET(tunnel_term_bud_1.pbmp, port_property);
    }
    rv = bcm_tunnel_terminator_create(unit,&tunnel_term_bud_1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_terminator_create_1, in_port=%d, \n", in_port);
    }
    if(verbose >= 1) {
        printf("created tunnel terminator_bud =0x%08x, \n", tunnel_term_bud_1.tunnel_id);
    }
    in_tunnel_gports[0] = tunnel_term_bud_1.tunnel_id;

    /* create IP tunnel terminators */
    /*** create IP tunnel terminator 2 ***/
    /* Cascade lookup when cascade_2 is 1 */
    if (ip6_tunnel_term_glbl_info.mc_cascade_2) {
        bcm_tunnel_terminator_config_key_t_init(&tunnel_term_config_key_bud_2);
        bcm_tunnel_terminator_config_action_t_init(&tunnel_term_config_action_bud_2);
        /* key is DIP6, IP.Next-Protocol, Qualifier2.Next-Protocol */
        sal_memcpy(tunnel_term_config_key_bud_2.dip6, ip6_tunnel_term_glbl_info.mc_dip2, 16);
        tunnel_term_config_key_bud_2.type = ip6_tunnel_term_glbl_info.tunnel_type_2;
        tunnel_term_config_key_bud_2.flags |= BCM_TUNNEL_TERM_BUD;
        tunnel_term_config_action_bud_2.tunnel_class = ip6_tunnel_term_glbl_info.mc_my_vtep_index_2;
        rv = bcm_tunnel_terminator_config_add(unit, 0, &tunnel_term_config_key_bud_2, &tunnel_term_config_action_bud_2);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_tunnel_terminator_config_add, in_port=%d, \n", in_port);
        }
       
        bcm_tunnel_terminator_t_init(&tunnel_term_bud_2);
        /* key is SIP6, VRF, Compressed-Dest, Port-Property */
        sal_memcpy(tunnel_term_bud_2.sip6, ip6_tunnel_term_glbl_info.mc_sip2, 16);
        sal_memcpy(tunnel_term_bud_2.sip6_mask, ip6_tunnel_term_glbl_info.mc_sip2_mask, 16);
        tunnel_term_bud_2.tunnel_if = 0; /* ? */
        tunnel_term_bud_2.type = ip6_tunnel_term_glbl_info.tunnel_type_2;/* this is IPv4 termination */
        tunnel_term_bud_2.vrf = ip6_tunnel_term_glbl_info.rif_vrf;
        tunnel_term_bud_2.flags |= BCM_TUNNEL_TERM_USE_TUNNEL_CLASS; /* indicate tunnel_class is a valid value */
        tunnel_term_bud_2.tunnel_class = ip6_tunnel_term_glbl_info.mc_my_vtep_index_2;
        BCM_PBMP_PORT_SET(tunnel_term_bud_2.pbmp, port_property);
    } else {        
        bcm_tunnel_terminator_t_init(&tunnel_term_bud_2);
        /* key is DIP6 */
        sal_memcpy(tunnel_term_bud_2.dip6, ip6_tunnel_term_glbl_info.mc_dip2, 16);
        sal_memcpy(tunnel_term_bud_2.dip6_mask, ip6_tunnel_term_glbl_info.mc_dip2_mask, 16);
        tunnel_term_bud_2.tunnel_if = 0; /* ? */
        tunnel_term_bud_2.type = ip6_tunnel_term_glbl_info.tunnel_type_2; /* this is IPv6 termination */
        tunnel_term_bud_2.vrf = ip6_tunnel_term_glbl_info.rif_vrf;
        tunnel_term_bud_2.flags |= BCM_TUNNEL_TERM_BUD;
        BCM_PBMP_PORT_SET(tunnel_term_bud_2.pbmp, port_property);
    }
    rv = bcm_tunnel_terminator_create(unit,&tunnel_term_bud_2);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_terminator_create_1, in_port=%d, \n", in_port);
    }
    if(verbose >= 1) {
        printf("created tunnel terminator_bud =0x%08x, \n", tunnel_term_bud_2.tunnel_id);
    }
    in_tunnel_gports[1] = tunnel_term_bud_2.tunnel_id;
    
    
    return rv;
}


