/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/* 
 * how to run:
 * cint utility/cint_utils_l3.c
 * cint cint_ip_route.c
 * cint cint_ip_tunnel.c
 * cint cint_ipv6_tunnel.c
 * cint
 * print ipv6_tunnel_example(<unit>, <in_port>, <out_port>); 
 *  
 * creating ipv6 tunnel no. 1 (host) - from <in_port> run packet: 
 *      ethernet header with DA 00:0c:00:02:00:00, SA 00:00:07:00:01:00 and Vlan tag id 2
 *      ipv4 header with DIP 127.255.255.3 and SIP 1.2.3.4
 * the packet will arrive at <out_port> with: 
 *      ethernet header with DA 00:00:00:00:CD:1D, SA 00:0C:00:02:00:00 and Vlan tag id 100
 *      ipv6 header with DIP ACAF:AEAD:ACAB:AAA9:A8A7:A6A5:A4A3:A2A1, SIP FC0F:0E0D:0C0B:0A09:0807:0605:0403:0201 and hop limit 60
 *      ipv4 header with DIP 1.0.0.17 and SIP 192.128.1.4, ttl decremented
 *
 * creating ipv6 tunnel no. 2 (route) - from <in_port> run packet: 
 *      ethernet header with DA 00:0c:00:02:00:00, SA 00:00:07:00:01:00 and Vlan tag id 2
 *      ipv4 header with DIP 127.255.255.0 and SIP 1.2.3.4
 * the packet will arrive at <out_port> with: 
 *      ethernet header with DA 20:00:00:00:CD:1D, SA 00:0C:00:02:00:00 and Vlan tag id 100
 *      ipv6 header with DIP ECEF:EEED:ECEB:EAE9:E8E7:E6E5:E4E3:E2E1, SIP CCCF:CECD:CCCB:CAC9:C8C7:C6C5:C4C3:C2C1 and hop limit 50
 *      ipv4 header with DIP 1.0.0.17 and SIP 192.128.1.4, ttl decremented
*/ 

/* ********* 
  Globals/Aux Variables
 */
struct ip_tunnel_encap_ipv6_glbl_info_s{
    bcm_tunnel_type_t type_1;
    bcm_tunnel_type_t type_2;
    int ttl_1;
    int ttl_2;
    int dscp_1;
    int dscp_2;
    bcm_tunnel_dscp_select_t dscp_sel;
    bcm_ip6_t sip1;  /* sip + dip to encapsulate */
    bcm_ip6_t dip1;  
    bcm_ip6_t sip2;  /* sip + dip to encapsulate */
    bcm_ip6_t dip2; 
    bcm_ip6_t route_ipv6;
    bcm_ip6_t mask_ipv6;
    bcm_ip6_t host_ipv6;
    bcm_mac_t dmac_1;
    bcm_mac_t smac_1;
    bcm_mac_t dmac_2;
    bcm_mac_t smac_2;
    uint16 udp_dest_port_1;
    uint16 udp_dest_port_2;
    int in_vlan_1;
    int out_vlan;
    int flow_label;
};

ip_tunnel_encap_ipv6_glbl_info_s ip_tunnel_encap_ipv6_glbl_info = 
{
/* tunnel 1 type */
bcmTunnelTypeIp4In6, 
/* tunnel 2 type */
bcmTunnelTypeIp6In6, 
/*TTL for tunnels*/
60, 50,
/*TOS for tunnels*/
40, 30,
/*dscp_sel*/
bcmTunnelDscpAssign,
/*SIP1 and DIP1*/
{0xFC, 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01}, {0xAC, 0xAF, 0xAE, 0xAD, 0xAC, 0xAB, 0xAA, 0xA9, 0xA8, 0xA7, 0xA6, 0xA5, 0xA4, 0xA3, 0xA2, 0xA1},
/*SIP2 and DIP2*/
{0xCC, 0xCF, 0xCE, 0xCD, 0xCC, 0xCB, 0xCA, 0xC9, 0xC8, 0xC7, 0xC6, 0xC5, 0xC4, 0xC3, 0xC2, 0xC1}, {0xEC, 0xEF, 0xEE, 0xED, 0xEC, 0xEB, 0xEA, 0xE9, 0xE8, 0xE7, 0xE6, 0xE5, 0xE4, 0xE3, 0xE2, 0xE1},
/*Route Ipv6 entry and Mask*/
{0x35, 0x55, 0x55, 0x55, 0x66, 0x66, 0x66, 0x66, 0x77, 0x77, 0x77, 0x77, 0x88, 0x88, 0x88, 0x88}, {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00},
/* Host Ipv6 entry */
{0xEC, 0xEF, 0xEE, 0xED, 0xEC, 0xEB, 0xEA, 0xE9, 0xE8, 0xE7, 0xE6, 0xE5, 0xE4, 0xE3, 0xE2, 0xE1},
/*DMAC1 and SMAC1*/
{0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}, {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00},
/*DMAC2 and SMAC2*/
{0x20, 0x00, 0x00, 0x00, 0xcd, 0x1d}, {0x00, 0xcc, 0x00, 0x02, 0x00, 0x00},
/*UDP dest port 1 and UDP dest port 2*/
0x6666, 0x7777,
/*in_vlan_1, out_vlan */
2,  100,
/*flow_label*/
0
};

struct ipv6_tunnel_s{
    int intf_ids[2];    /* interface IDs of IP tunnels*/
};

ipv6_tunnel_s g_ipv6_tunnel = {
    {0,0}
};

/* debug prints */
int verbose = 1;

/********** 
  functions
 */

/*
 * buid two IP tunnels with following information:
 * Tunnnel 1: 
 *   -  dscp  = 10;
 *   -  ttl   = 60;
 *   -  type  = bcmTunnelTypeIp4In6;
 *   -  Vlan  = 100
 *   -  Dmac  = 00:00:00:00:cd:1d
 *   -  Smac  = 00:0c:00:02:00:00
 *   + No Fec defined over this tunnel
 *  
 * Tunnnel 2: 
 *   -  dscp  = 11;
 *   -  ttl   = 50;
 *   -  type  = bcmTunnelTypeIp4In6;
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
int ipv6_tunnel_build_tunnels(int unit, int out_port, int* intf_ids, bcm_gport_t *tunnel_gports, int *ll_encap_id){
    int rv;
    int ing_intf_in;
    int ing_intf_out; 
    int fec[2] = {0x0, 0x0};
    int flags;
    int encap_id[2] = {0x0, 0x0};
    /* Tunnel Terminator 1 info*/
    bcm_tunnel_initiator_t tunnel_1;  
    int tunnel_itf1 = 0;
    /* Tunnel Terminator 2 info*/
    bcm_tunnel_initiator_t tunnel_2;
    int tunnel_itf2 = 0;
    int ipv6_tunnel_encap_mode;

    ipv6_tunnel_encap_mode = soc_property_get(unit, "bcm886xx_ip6_tunnel_encapsulation_mode", 1);

    /*** create egress router interface ***/
    rv = vlan__open_vlan_per_mc(unit, ip_tunnel_encap_ipv6_glbl_info.out_vlan, 0x1);  
    if (rv != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", ip_tunnel_encap_ipv6_glbl_info.out_vlan, unit);
    }
    rv = bcm_vlan_gport_add(unit, ip_tunnel_encap_ipv6_glbl_info.out_vlan, out_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", out_port, ip_tunnel_encap_ipv6_glbl_info.out_vlan);
      return rv;
    }

    create_l3_intf_s intf;
    intf.vsi = ip_tunnel_encap_ipv6_glbl_info.out_vlan;
    intf.my_global_mac = ip_tunnel_encap_ipv6_glbl_info.smac_1;
    intf.my_lsb_mac = ip_tunnel_encap_ipv6_glbl_info.smac_1;
    
    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_out = intf.rif;        
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }
    
    /*** create IP tunnel 1 ***/
    bcm_tunnel_initiator_t_init(&tunnel_1);
    sal_memcpy(&(tunnel_1.dip6),&(ip_tunnel_encap_ipv6_glbl_info.dip1),16);
    sal_memcpy(&(tunnel_1.sip6),&(ip_tunnel_encap_ipv6_glbl_info.sip1),16);
    tunnel_1.flags = 0;
    tunnel_1.dscp_sel = ip_tunnel_encap_ipv6_glbl_info.dscp_sel;
    tunnel_1.ttl = ip_tunnel_encap_ipv6_glbl_info.ttl_1;
    tunnel_1.dscp = ip_tunnel_encap_ipv6_glbl_info.dscp_1;
    tunnel_1.flow_label = ip_tunnel_encap_ipv6_glbl_info.flow_label;
    tunnel_1.type = ip_tunnel_encap_ipv6_glbl_info.type_1;
    tunnel_1.l3_intf_id = ing_intf_out;
    if (ipv6_tunnel_encap_mode == 2) {
        tunnel_1.flags = BCM_TUNNEL_INIT_WIDE;
    }
    tunnel_itf1 = 0;
    rv = add_ip_tunnel(unit,&tunnel_itf1,&tunnel_1);
    if (rv != BCM_E_NONE) {
        printf("Error, in add_ip_tunnel\n");
    }
    if(verbose >= 1) {
        printf("created IP tunnel_1 on intf:0x%08x \n",tunnel_itf1);
    }

    /*** create IP tunnel 2 ***/
    bcm_tunnel_initiator_t_init(&tunnel_2);
    sal_memcpy(&(tunnel_2.dip6),&(ip_tunnel_encap_ipv6_glbl_info.dip2),16);
    sal_memcpy(&(tunnel_2.sip6),&(ip_tunnel_encap_ipv6_glbl_info.sip2),16);
    tunnel_2.flags = 0;
    tunnel_2.dscp_sel = ip_tunnel_encap_ipv6_glbl_info.dscp_sel;
    tunnel_2.ttl = ip_tunnel_encap_ipv6_glbl_info.ttl_2;  
    tunnel_2.dscp = ip_tunnel_encap_ipv6_glbl_info.dscp_2;  
    tunnel_2.flow_label = ip_tunnel_encap_ipv6_glbl_info.flow_label;
    tunnel_2.type = ip_tunnel_encap_ipv6_glbl_info.type_2;
    tunnel_2.l3_intf_id = 0;
    if (ipv6_tunnel_encap_mode == 2) {
        tunnel_2.flags = BCM_TUNNEL_INIT_WIDE;
    }
    tunnel_itf2 = 0;
    rv = add_ip_tunnel(unit,&tunnel_itf2,&tunnel_2); 
    if (rv != BCM_E_NONE) {
        printf("Error, add_ip_tunnel 2\n");
    }
    if(verbose >= 1) {
        printf("created IP tunnel_2 on intf:0x%08x \n",tunnel_itf2);
    }

    if (!ip_tunnel_glbl_info.fec_create){
        /*** using egress object API set MAC address for tunnel 1 interface, without allocating FEC enty ***/
        create_l3_egress_s l3eg;
        sal_memcpy(l3eg.next_hop_mac_addr, ip_tunnel_encap_ipv6_glbl_info.dmac_1, 6);
        l3eg.out_tunnel_or_rif = tunnel_itf1;
        l3eg.vlan = ip_tunnel_encap_ipv6_glbl_info.out_vlan;
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
        intf_ids[0] = tunnel_itf1;
    }
    else{
        /*** create egress object 1: points into tunnel 1, with allocating FEC, and da-mac = next_hop_mac ***/
        create_l3_egress_s l3eg;
        sal_memcpy(l3eg.next_hop_mac_addr, ip_tunnel_encap_ipv6_glbl_info.dmac_1 , 6);
        l3eg.vlan = ip_tunnel_encap_ipv6_glbl_info.out_vlan;
        l3eg.out_gport = out_port;
        l3eg.out_tunnel_or_rif = tunnel_itf1;
        l3eg.fec_id = fec[0];
        l3eg.arp_encap_id = encap_id[0];

        rv = l3__egress__create(unit,&l3eg);
        if (rv != BCM_E_NONE) {
            printf("Error, l3__egress__create\n");
            return rv;
        }

        fec[0] = l3eg.fec_id;
        encap_id[0] = l3eg.arp_encap_id;
        if(verbose >= 1) {
            printf("created FEC-id =0x%08x, \n", fec[0]);
            printf("next hop mac at encap-id %08x, \n", encap_id[0]);
        }
        intf_ids[0] = fec[0];
    }
    /*** create egress object 2: points into tunnel 2, with allocating FEC, and da-mac = next_hop_mac2  ***/
    create_l3_egress_s l3eg1;
    sal_memcpy(l3eg1.next_hop_mac_addr, ip_tunnel_encap_ipv6_glbl_info.dmac_2 , 6);
    l3eg1.vlan = ip_tunnel_encap_ipv6_glbl_info.out_vlan;
    l3eg1.out_gport = out_port;
    l3eg1.out_tunnel_or_rif = tunnel_itf2;
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
        printf("created FEC-id =0x%08x, \n", fec[1]);
        printf("next hop mac at encap-id %08x, \n", encap_id[1]);
    }

    /* interface for tunnel_2 is egress-object (FEC) */
    intf_ids[1] = fec[1];
    *ll_encap_id = encap_id[1];
    /* refernces to created tunnels as gport */
    tunnel_gports[0] = tunnel_1.tunnel_id;
    tunnel_gports[1] = tunnel_2.tunnel_id;
    return rv;
}


