/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/* 
 * how to run:
 *
cint;
cint_reset();
exit;
cint ../../../../src/examples/dpp/utility/cint_utils_vlan.c
cint ../../../../src/examples/dpp/utility/cint_utils_l3.c 
cint ../../../../src/examples/dpp/cint_ip_route.c
cint ../../../../src/examples/dpp/cint_ip_tunnel.c
cint
int rv; 
ipv4_tunnel_example(unit,13,13); 
//set default values  
ip_tunnel_glbl_init(unit,0); 
rv = ipv4_tunnel_example(unit,13,13);

cint 
*/ 

/* ********* 
  Globals/Aux Variables
 */
int outlif_to_count1;
int outlif_to_count2;
int outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE;
/* DSCP preserve feature */
int preserve_dscp_per_lif = 0;
/* debug prints */
int verbose = 1;

/* Adding global counter id and counter engine variables which can be used in TCL scripts */
int global_counter_id;

/********** 
  functions
 */


bcm_mac_t ip_tunnel_my_mac_get() {
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  /* my-MAC */
    return mac_address;
}


struct ip_tunnel_s{
    bcm_mac_t da; /* ip tunnel da */
    bcm_mac_t sa; /* ip tunnel my mac */
    int       sip; /* ip tunnel sip */    
    int       dip; /* ip tunnel dip */   
    int       ttl; /* ip tunnel ttl */
    int       dscp; /* ip tunnel dscp */
    bcm_tunnel_dscp_select_t    dscp_sel; /* ip tunnel dscp_sel */
    int flags;
    uint16 udp_dest_port; /* udp dest port */
};

/*
 * struct include meta information. 
 * where the cint logic pull data from this struct. 
 * use to control differ runs of the cint, without changing the cint code
 */
struct ip_tunnel_glbl_info_s{
    bcm_tunnel_type_t tunnel_1_type;
    bcm_tunnel_type_t tunnel_2_type;
    ip_tunnel_s tunnel_1;
    ip_tunnel_s tunnel_2;
    int vlan;  /* vlan for ip tunnels */
    bcm_ip6_t host_ipv6;
    bcm_ip6_t route_ipv6;
    bcm_ip6_t mask_ipv6;
    int fec_create;
    int add_ipv6_host_route;
    int ipv6_host_per_tunnel;   
};

ip_tunnel_glbl_info_s ip_tunnel_glbl_info = 
{
/* tunnel 1 type */
bcmTunnelTypeGreAnyIn4, 
/* tunnel 2 type */
bcmTunnelTypeIpAnyIn4, 
/* tunnel 1  
              da                    |      sa               | sip: 160.0.0.17 | dip: 161.0.0.17 |  ttl   | dscp |   dscp_sel  |     flags    | udp dest port         */ 
{{0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}, ip_tunnel_my_mac_get(), 0xA0000011,        0xA1000011,      60,     10,      bcmTunnelDscpAssign,    0, 0x6666},
/* tunnel 2  
              da                    |      sa               | sip: 170.0.0.17 | dip: 171.0.0.17 |  ttl   | dscp |   dscp_sel  |     flags    | udp dest port         */ 
{{0x20, 0x00, 0x00, 0x00, 0xcd, 0x1d}, ip_tunnel_my_mac_get(), 0xAA000011,         0xAB000011,     50,     11,      bcmTunnelDscpAssign,    0, 0x7777}, 
/* vlan for both ip tunnels */
100,
/* host Ipv6 entry */
{0xEC, 0xEF, 0xEE, 0xED, 0xEC, 0xEB, 0xEA, 0xE9, 0xE8, 0xE7, 0xE6, 0xE5, 0xE4, 0xE3, 0xE2, 0xE1},
/* route IPv6 entry */
{0x35, 0x55, 0x55, 0x55, 0x66, 0x66, 0x66, 0x66, 0x77, 0x77, 0x77, 0x77, 0x88, 0x88, 0x88, 0x88},
/* route IPv6 mask */
{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00},
/* fec_create,  add_ipv6_host_route, ipv6_host_per_tunnel*/
0,  0,  0
};

/* Initialization of global structs */

void ip_tunnel_glbl_init(int unit, int flags){
    ip_tunnel_glbl_info.tunnel_1_type = bcmTunnelTypeGreAnyIn4;
    ip_tunnel_glbl_info.tunnel_2_type = bcmTunnelTypeIpAnyIn4;
}


void ip_tunnel_info_get(ip_tunnel_s *tunnel_param_1, ip_tunnel_s *tunnel_param_2) {
    sal_memcpy( tunnel_param_1, &ip_tunnel_glbl_info.tunnel_1, sizeof(ip_tunnel_glbl_info.tunnel_1));
    sal_memcpy( tunnel_param_2, &ip_tunnel_glbl_info.tunnel_2, sizeof(ip_tunnel_glbl_info.tunnel_2));
}


/*
 * create ipv4 tunnel
 *  bcm_tunnel_initiator_t *tunnel: include tunnel information
 *  bcm_if_t *itf : (IN/OUT) the placement of the tunnel as encap-id
 *  tunnel->tunnel_id : is update to include the placement of the created tunnel as gport.
 */
int add_ip_tunnel(int unit,bcm_if_t *itf, bcm_tunnel_initiator_t *tunnel){

    bcm_l3_intf_t l3_intf;
    int rv = BCM_E_NONE;

    bcm_l3_intf_t_init(&l3_intf);

    /* if given ID is set, then use it as placement of the ipv4-tunnel */
    if(*itf != 0) {
        BCM_GPORT_TUNNEL_ID_SET(tunnel->tunnel_id,*itf);
        tunnel->flags |= BCM_TUNNEL_WITH_ID; 
    }

    rv = bcm_tunnel_initiator_create(unit, &l3_intf, tunnel);
    if(rv != BCM_E_NONE) {
        printf("Error, create tunnel initiator \n");
        return rv;
    }

    /* include interface-id points to the tunnel */
    *itf = l3_intf.l3a_intf_id;
    return rv;
}

/*
 * Remove ipv4 tunnel
 *  tunnel->tunnel_id : is update to include the placement of the created tunnel as gport.
 */
int remove_ip_tunnel(int unit,bcm_if_t itf){

    bcm_l3_intf_t l3_intf;
    int rv = BCM_E_NONE;

    bcm_l3_intf_t_init(&l3_intf);

    l3_intf.l3a_intf_id = itf;

    rv = bcm_tunnel_initiator_clear(unit, &l3_intf);
    if(rv != BCM_E_NONE) {
        printf("Error, create tunnel initiator \n");
        return rv;
    }
    return rv;
}


/******* Run example ******/
 
/*
 * IP tunnel example 
 * - build IP tunnels. 
 * - add ip routes/host points to the tunnels
 */
int ipv4_tunnel_example(int unit, int in_port, int out_port){
    int intf_ids[2];
    bcm_gport_t tunnel_gport_ids[2];
    int ll_encap_id;
    int rv;


    /* build IP tunnels, and get back interfaces */
    rv = ipv4_tunnel_build_tunnels(unit, out_port, intf_ids, tunnel_gport_ids, &ll_encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, ipv4_tunnel_add_routes, in_port=%d, \n", in_port);
    }


    /* add IP route host points to the Tunnels */
    rv = ipv4_tunnel_add_routes(unit, in_port, out_port, intf_ids, ll_encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, ipv4_tunnel_add_routes, in_port=%d, \n", in_port);
    }

    /* counts encapsulated packest using OAM counter engine*/
    if (tunnel_counter_enable) {
       
        int counter_id;
        uint64 counter_id64;

        rv=set_counter_source_and_engines_for_tunnel_counting(unit,&counter_id,out_port);
        if (rv != BCM_E_NONE) {
            printf("Error, set_counter_source_and_engines_for_tunnel_counting");
        }  

        COMPILER_64_SET(counter_id64, 0, counter_id);
	global_counter_id = counter_id;
        rv= bcm_port_stat_set(unit,tunnel_gport_ids[0],bcmPortStatEgressPackets,counter_id64);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_stat_set");
        }

    }

    return rv;
}


int ipv4_encap_tunnel_ipany_greany(int unit, int in_port, int out_port){
    int intf_ids[2];
    int ll_encap_id;
    bcm_gport_t tunnel_gport_ids[2];
    int rv;

    ip_tunnel_glbl_info.fec_create = 1;
    ip_tunnel_glbl_info.add_ipv6_host_route = 1;
    ip_tunnel_glbl_info.tunnel_1_type = bcmTunnelTypeGreAnyIn4;
    ip_tunnel_glbl_info.tunnel_2_type = bcmTunnelTypeIpAnyIn4;

    ip_tunnel_glbl_info.tunnel_1.dscp = 20;
    ip_tunnel_glbl_info.tunnel_1.ttl = 64;

    ip_tunnel_glbl_info.tunnel_2.dscp = 30;
    ip_tunnel_glbl_info.tunnel_2.ttl =  40;

    /* build IP tunnels, and get back interfaces */
    rv = ipv4_tunnel_build_tunnels(unit, out_port, intf_ids, tunnel_gport_ids, &ll_encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, ipv4_tunnel_encap_tunnels, in_port=%d, \n", in_port);
    }

    
    /* add IP route host points to the Tunnels */
    rv = ipv4_tunnel_add_routes(unit, in_port, out_port, intf_ids, ll_encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, ipv4_tunnel_add_routes, in_port=%d, \n", in_port);
    }

    return rv;
}
/* The function configures pipe and uniform TTL ad DSCP assignment for 2 IPv4 tunnels */
int ipv4_encap_tunnel_dscp_ttl_ipany_greany(int unit, int in_port, int out_port){
    int intf_ids[2];
    int ll_encap_id;
    bcm_gport_t tunnel_gport_ids[2];
    int rv;

	ip_tunnel_glbl_info.fec_create = 1;
	ip_tunnel_glbl_info.add_ipv6_host_route = 1;
	ip_tunnel_glbl_info.tunnel_1_type = bcmTunnelTypeGreAnyIn4;
	ip_tunnel_glbl_info.tunnel_2_type = bcmTunnelTypeIpAnyIn4;
	
	
	ip_tunnel_glbl_info.tunnel_1.dscp = 5;
	ip_tunnel_glbl_info.tunnel_1.ttl = 64;
	
	ip_tunnel_glbl_info.tunnel_2.dscp = 0;
	ip_tunnel_glbl_info.tunnel_2.dscp_sel = bcmTunnelDscpPacket;
	ip_tunnel_glbl_info.tunnel_2.flags = BCM_TUNNEL_TERM_USE_OUTER_TTL;
	
	
    /* build IP tunnels, and get back interfaces */
    rv = ipv4_tunnel_build_tunnels(unit, out_port, intf_ids, tunnel_gport_ids, &ll_encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, ipv4_tunnel_encap_tunnels, in_port=%d, \n", in_port);
    }

    /* add IP route host points to the Tunnels */
    rv = ipv4_tunnel_add_routes(unit, in_port, out_port, intf_ids, ll_encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, ipv4_tunnel_add_routes, in_port=%d, \n", in_port);
    }

    return rv;
}

int ipv4_tunnel_example_dvapi_run(int unit, int in_port, int out_port){
    ip_tunnel_glbl_init(unit,0); 
    return ipv4_tunnel_example(unit,in_port,out_port);
}

 
int tunnel_counter_enable=0;
/*count encapsulated packet per outlif using the OAM counter engine*/
int ipv4_tunnel_example_lif_counter(int unit, int in_port, int out_port){
    tunnel_counter_enable=1;
    ip_tunnel_glbl_init(unit,0); 
    return ipv4_tunnel_example(unit,in_port,out_port);
}

/* 
 * Create 1:1 mapping from TC to DSCP [for DP = yellow]
 */
int add_TC_to_DSCP_mapping(int unit, bcm_gport_t gport) {

    bcm_qos_map_t map;
    bcm_l3_ingress_t l3_ing_if;

    /* Mapping for ethernet forward */
    uint32 map_flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_ENCAP;

    int map_id;
    int inPcp;
    int rv = BCM_E_NONE;

    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS, &map_id);
    if (rv != BCM_E_NONE) {
        printf("Error, QOS map create \n");
        return -1;
    }

    bcm_qos_map_t_init(&map);
    rv = bcm_qos_map_add(unit, map_flags, &map ,map_id);
    if (rv != BCM_E_NONE) {
        printf("Error, adding map");
        return -1;
    }

    for (inPcp = 0; inPcp < 8; inPcp++) {

        /* From input PCP value */
        map.remark_int_pri = inPcp; /* TC */
        map.color = bcmColorYellow; /* DP */

        /* To output DSCP value */
        map.dscp = inPcp; 

        rv = bcm_qos_map_add(unit, map_flags, &map ,map_id);
        if (rv != BCM_E_NONE) {
            printf("Error, adding mapping from PCP = %d to DSCP = %d\n", inPcp, inPcp/2);
            return -1;
        }
    }

    return map_id;
}
/* 
 * Sets the global tunnel information of tunnel 2 (only) for
 * mapping the dscp value instead of the default assigning.
 */
int set_tunnel_info_for_mapping() {
  ip_tunnel_glbl_info.tunnel_2.dscp = 0;
  ip_tunnel_glbl_info.tunnel_2.dscp_sel = bcmTunnelDscpMap;

  return BCM_E_NONE;
}



/*
 * buid two IP tunnels with following information:
 * Tunnnel 1: 
 *   -  sip   = 160.0.0.17
 *   -  dip   =  161.0.0.17
 *   -  dscp  = 10;
 *   -  ttl   = 60;
 *   -  type  = bcmTunnelTypeIp4In4;
 *   -  Vlan  = 100
 *   -  Dmac  = 00:00:00:00:cd:1d
 *   -  Smac  = 00:0c:00:02:00:00
 *   + No Fec defined over this tunnel
 *  
 * Tunnnel 2: 
 *   -  sip   = 170.0.0.17
 *   -  dip   =  171.0.0.17
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
 *      tunnel_gports[] array includes the gport-ids pointing to the IP tunnels
 */
int ipv4_tunnel_build_tunnels(int unit, int out_port, bcm_if_t* intf_ids, bcm_gport_t *tunnel_gports, bcm_if_t* ll_encap_id){
    int rv;
    int ing_intf_in;
    int ing_intf_out; 
    int fec[2] = {0x0,0x0};
    int flags = 0;
    int out_vlan = ip_tunnel_glbl_info.vlan;
    int encap_id[2]={0x0,0x0};
    char *proc_name;

    proc_name = "ipv4_tunnel_build_tunnels";
    /* my-MAC {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  */
    bcm_mac_t mac_address  = ip_tunnel_my_mac_get();
    bcm_tunnel_initiator_t tunnel_1;

    /* tunnel 1 info*/
    ip_tunnel_s tunnel_info_1 = ip_tunnel_glbl_info.tunnel_1;
    bcm_mac_t next_hop_mac  = tunnel_info_1.da; /* default: {00:00:00:00:cd:1d} */
    int tunnel_itf1=0;

    /* tunnel 2 info */
    bcm_tunnel_initiator_t tunnel_2;
    ip_tunnel_s tunnel_info_2 = ip_tunnel_glbl_info.tunnel_2;
    bcm_mac_t next_hop_mac2  = tunnel_info_2.da; /* default: {0x20:00:00:00:cd:1d} */
    int tunnel_itf2=0;

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
    tunnel_1.dip = tunnel_info_1.dip; /* default: 161.0.0.17*/
    tunnel_1.sip = tunnel_info_1.sip; /* default: 160.0.0.17*/
    tunnel_1.dscp = tunnel_info_1.dscp; /* default: 10 */
    tunnel_1.flags = 0;
    tunnel_1.ttl = tunnel_info_1.ttl; /* default: 60 */
    tunnel_1.type = ip_tunnel_glbl_info.tunnel_1_type; /* default: bcmTunnelTypeGreAnyIn4*/ 
    tunnel_1.dscp_sel = tunnel_info_1.dscp_sel; /* default: bcmTunnelDscpAssign */
    tunnel_itf1 = tunnel_gports[0];
    tunnel_1.outlif_counting_profile = outlif_counting_profile;
    if(is_device_or_above(unit, JERICHO2))
    {
        tunnel_1.encap_access = bcmEncapAccessTunnel3;
        tunnel_1.vlan = 0;
    }
    else
    {
        tunnel_1.vlan = out_vlan;
    }
    rv = add_ip_tunnel(unit,&tunnel_itf1,&tunnel_1);
    if (rv != BCM_E_NONE) {
        printf("Error, add_ip_tunnel 1\n");
        return rv;
    }
    if(verbose >= 1) {
        printf("%s(): created IP tunnel_1 on intf:0x%08X - gport 0x%08X\n",proc_name, tunnel_itf1, tunnel_1.tunnel_id);
    }
    if (outlif_counting_profile != BCM_STAT_LIF_COUNTING_PROFILE_NONE)
    {
        outlif_to_count1 = tunnel_1.tunnel_id;        
    }
    
    /*** create tunnel 2 ***/
    bcm_tunnel_initiator_t_init(&tunnel_2);
    tunnel_2.dip = tunnel_info_2.dip; /* default: 171.0.0.17*/
    tunnel_2.sip = tunnel_info_2.sip; /* default: 170.0.0.17*/
    tunnel_2.dscp = tunnel_info_2.dscp; /* default: 11 */ 
    tunnel_2.flags = tunnel_info_2.flags;
    tunnel_2.ttl = tunnel_info_2.ttl; /* default: 50 */
    tunnel_2.type = ip_tunnel_glbl_info.tunnel_2_type; /* default: bcmTunnelTypeIpAnyIn4 */
    tunnel_2.dscp_sel = tunnel_info_2.dscp_sel; /* default: bcmTunnelDscpAssign */
    tunnel_2.outlif_counting_profile = outlif_counting_profile;
    tunnel_itf2 = tunnel_gports[1];
    if(is_device_or_above(unit, JERICHO2))
    {
        /* In JR2, for tunnels, VSI is coming from ARP and not from the tunnel itself (vlan field) */
        tunnel_2.encap_access = bcmEncapAccessTunnel3;
        tunnel_2.vlan = 0;
    }
    else
    {
        tunnel_2.vlan = out_vlan;
    }
    rv = add_ip_tunnel(unit,&tunnel_itf2,&tunnel_2);
    if (rv != BCM_E_NONE) {
        printf("Error, add_ip_tunnel 2\n");
        return rv;
    }
    if(verbose >= 1) {
        printf("%s(): created IP tunnel_2 on intf:0x%08x - gport 0x%08X\n",proc_name, tunnel_itf2, tunnel_2.tunnel_id);
    }
    if (outlif_counting_profile != BCM_STAT_LIF_COUNTING_PROFILE_NONE)
    {
        outlif_to_count2 = tunnel_2.tunnel_id;
    }

    if (!ip_tunnel_glbl_info.fec_create){
        /*** using egress object API set MAC address for tunnel 1 interface, without allocating FEC enty ***/
        create_l3_egress_s l3eg;
        sal_memcpy(l3eg.next_hop_mac_addr, next_hop_mac , 6);
        l3eg.out_tunnel_or_rif = tunnel_itf1;
        /* In Jericho2, VSI should be part of ARP when doing tunneling. */
        l3eg.vlan = is_device_or_above(unit, JERICHO2) ? out_vlan : 0;
        l3eg.arp_encap_id = encap_id[0];

        rv = l3__egress_only_encap__create(unit,&l3eg);
        if (rv != BCM_E_NONE) {
            printf("Error, l3__egress_only_encap__create\n");
            return rv;
        }
        fec[0] = l3eg.fec_id;
        encap_id[0] = l3eg.arp_encap_id;
        if(verbose >= 1) {
            printf("FEC is allocated FEC-id =0x%08x, \n", fec[0]);
            printf("next hop mac at encap-id 0x%08x, \n", encap_id[0]);
        }
         intf_ids[0] = tunnel_itf1;
    }
    else {
        /*** create egress object 1: points into tunnel 1, with allocating FEC, and da-mac = next_hop_mac1 ***/
        create_l3_egress_s l3eg0;
        sal_memcpy(l3eg0.next_hop_mac_addr, next_hop_mac , 6);
        l3eg0.out_tunnel_or_rif = tunnel_itf1;
        l3eg0.out_gport = out_port;
        l3eg0.vlan = out_vlan;
        l3eg0.fec_id = fec[0];
        l3eg0.arp_encap_id = encap_id[0];

        rv = l3__egress__create(unit,&l3eg0);
        if (rv != BCM_E_NONE) {
            printf("Error, l3__egress__create\n");
            return rv;
        }

        fec[0] = l3eg0.fec_id;
        encap_id[0] = l3eg0.arp_encap_id;
        if(verbose >= 1) {
            printf("%s(): 1. created FEC-id =0x%08x, \n", proc_name, fec[0]);
            printf("%s(): 1. next hop mac at encap-id %08x, \n", proc_name, encap_id[0]);
        }
         intf_ids[0] = fec[0];
    }
    /*** create egress object 2: points into tunnel 2, with allocating FEC, and da-mac = next_hop_mac2  ***/
    create_l3_egress_s l3eg1;
    sal_memcpy(l3eg1.next_hop_mac_addr, next_hop_mac2 , 6);
    l3eg1.out_tunnel_or_rif = tunnel_itf2;
    l3eg1.out_gport = out_port;
    l3eg1.vlan = out_vlan;
    l3eg1.fec_id = fec[1];
    l3eg1.arp_encap_id = encap_id[1];

    rv = l3__egress__create(unit,&l3eg1);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
        return rv;
    }

    fec[1] = l3eg1.fec_id;
    encap_id[1] = l3eg1.arp_encap_id;
    if(verbose >= 1) {
        printf("%s(): 2. created FEC-id =0x%08x, \n", proc_name, fec[1]);
        printf("%s(): 2. next hop mac at encap-id %08x, \n", proc_name, encap_id[1]);
    }

    /* interface for tunnel_2 is egress-object (FEC) */
    intf_ids[1] = fec[1];
    *ll_encap_id = encap_id[1];
    /* refernces to created tunnels as gport */
    tunnel_gports[0] = tunnel_1.tunnel_id;
    tunnel_gports[1] = tunnel_2.tunnel_id;

    if (preserve_dscp_per_lif & 0x2) {
        int i;
        bcm_gport_t gport;
        for (i = 0; i < 2; i++) {
            rv = bcm_port_control_set(unit, tunnel_gports[i], bcmPortControlPreserveDscpEgress, 1);
            if (rv != BCM_E_NONE) {
                printf("bcm_port_control_set bcmPortControlPreserveDscpEgress failed, port(0x08x)\n", gport);
                return rv;
            }
        }
    }

    return rv;
}

/* 
 *  add IPv4 routes pointing to IP-tunnel
 *   - add host points directly to tunnel
 *   - add route entry points egress-object which points to tunnel.
 *  
 *   host:
 *      - DIP: 127.255.255.03
 *   route:
 *      - subnet: 127.255.255.00/28
 */
int ipv4_tunnel_add_routes(int unit, int in_port, int out_port, int* intf_ids, int ll_encap_id){
    int vrf = 0;
    int host;
    int route;
    int mask;
    int in_vlan = 2; 
    int ing_intf_in;
    int encap_id[2] = {0x0, 0x0}; 
    int open_vlan = 1;
    bcm_ip6_t host_ipv6 = ip_tunnel_glbl_info.host_ipv6;
    bcm_ip6_t route_ipv6 = ip_tunnel_glbl_info.route_ipv6;
    bcm_ip6_t mask_ipv6 = ip_tunnel_glbl_info.mask_ipv6;
    /* my-MAC  {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}; */
    bcm_mac_t mac_address  = ip_tunnel_my_mac_get();

    int rv;

    /*** create ingress router interface ***/
    rv = vlan__open_vlan_per_mc(unit, in_vlan, 0x1);  
    if (rv != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", in_vlan, unit);
    }

    if (preserve_dscp_per_lif & 0x1) {
        bcm_vlan_port_t vlan_port;
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.port=in_port;
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vlan_port.match_vlan = in_vlan;
        vlan_port.egress_vlan = in_vlan;
        vlan_port.vsi = in_vlan;
        rv = bcm_vlan_port_create(unit, &vlan_port);
        if (rv != BCM_E_NONE) {
            printf("fail create VLAN port, port(0x%08x), vlan(%d)\n", in_port, in_vlan);
            return rv;
        }

        rv = bcm_port_control_set(unit, vlan_port.vlan_port_id, bcmPortControlPreserveDscpIngress, 1);
        if (rv != BCM_E_NONE) {
            printf("bcm_port_control_set bcmPortControlPreserveDscpIngress failed, port(0x08x)\n", vlan_port.vlan_port_id);
            return rv;
        }
    } else {
        rv = bcm_vlan_gport_add(unit, in_vlan, in_port, 0);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
            printf("fail add port(0x%08x) to vlan(%d)\n", in_port, in_vlan);
          return rv;
        }
    }

    create_l3_intf_s intf;
    intf.vsi = in_vlan;
    intf.my_global_mac = mac_address;
    intf.my_lsb_mac = mac_address;
    intf.mtu_valid = 1;
    intf.mtu = 0;
    intf.mtu_forwarding = 0;
    intf.vrf_valid = 1;
    intf.vrf = vrf;
    
    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_in = intf.rif;        
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }

    /*** add host point to FEC ***/
    host = 0x7fffff03;
    route = 0x7fffff00;
    mask  = 0xfffffff0;
    if (!ip_tunnel_glbl_info.ipv6_host_per_tunnel){
        rv = add_host_direct(unit, host, vrf, intf_ids[0],out_port); 
        if (rv != BCM_E_NONE) {
            printf("Error, create egress object, in_port=%d, \n", in_port);
        }

        if(verbose >= 1) {
            printf("---> egress-tunnel=0x%08x, port=%d, \n", intf_ids[0], out_port);
        }

        /*** add route point to FEC2 ***/
        rv = add_route(unit, route, mask , vrf, intf_ids[1]); 
        if (rv != BCM_E_NONE) {
            printf("Error, create egress object, out_port=%d, \n", out_port);
        }

        if(verbose >= 1) {
            printf("---> egress-object=0x%08x, port=%d, \n", intf_ids[1], out_port);
        }
        
        if (ip_tunnel_glbl_info.add_ipv6_host_route){
            rv = add_route_ip6(unit, route_ipv6, mask_ipv6, vrf, intf_ids[0]);
            rv = add_ipv6_host(unit, host_ipv6, vrf, intf_ids[1], ll_encap_id); 
            if (rv != BCM_E_NONE) {
                printf("Error,  add ipv6 host \n");
            }
        }
    }
    else{
        rv = add_host_direct(unit, host, vrf, intf_ids[0],out_port); 
        if (rv != BCM_E_NONE) {
            printf("Error, create egress object, in_port=%d, \n", in_port);
        }

        if(verbose >= 1) {
            print_host("add entry ", host,vrf);
            printf("---> egress-tunnel=0x%08x, port=%d, \n", intf_ids[0], out_port);
        }
        /*** add route point to FEC2 ***/
        rv = add_route(unit, route, mask , vrf, intf_ids[0]);
        if (rv != BCM_E_NONE) {
            printf("Error, create egress object, out_port=%d, \n", out_port);
        }

        if(verbose >= 1) {
            print_route("add entry ", route,mask,vrf);
            printf("---> egress-object=0x%08x, port=%d, \n", intf_ids[0], out_port);
        }
        rv = add_route_ip6(unit, route_ipv6, mask_ipv6, vrf, intf_ids[1]);
        rv = add_ipv6_host(unit, host_ipv6, vrf, intf_ids[1], ll_encap_id);
        if (rv != BCM_E_NONE) {
            printf("Error,  add ipv6 host \n");
        }
    }
    return rv;
}

int outlif_counting_profile_set(int profile)
{
    outlif_counting_profile = profile;
    printf("Tunnel create: outlif_counting_profile_set function: outlif_counting_profile=%d \n",outlif_counting_profile);
    return BCM_E_NONE;
}

