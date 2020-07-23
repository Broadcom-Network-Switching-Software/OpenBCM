/*$Copyright: (c) 2013 Broadcom Corporation All Rights Reserved.$*/
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
cint ../../../../src/examples/dpp/utility/cint_utils_l3.c
cint ../../../../src/examples/dpp/utility/cint_utils_multicast.c
cint ../../../../src/examples/dpp/cint_ip_route.c
cint ../../../../src/examples/dpp/cint_ip_tunnel_term.c
cint
int rv;
verbose = 2; 
rv = ipv4_tunnel_term_example(unit,13,13);
 
Tunnels according IP next protocol example: 
SOC proeprty: bcm886xx_ip4_tunnel_termination_mode=5 
cint;
cint_reset();
exit;
cint ../../../../src/examples/dpp/utility/cint_utils_l3.c
cint ../../../../src/examples/dpp/utility/cint_utils_multicast.c 
cint ../../../../src/examples/dpp/cint_ip_route.c
cint ../../../../src/examples/dpp/cint_ip_tunnel_term.c
cint
int rv;
verbose = 2; 
ipv4_tunnel_term_next_protocol_example(unit,13,13); 
 
 
 
Tunnels according to DIP, SIP, VRF example: 
SOC property: bcm886xx_ip4_tunnel_termination_mode=6 
cint;
cint_reset();
exit; 
cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c   
cint ../../../../src/examples/dpp/utility/cint_utils_l3.c
cint ../../../../src/examples/dpp/cint_ip_tunnel_term.c
cint
int rv; 
int unit = 0; 
int accessPort1 = 200; 
int providerPort1 = 201; 
int providerPort2 = 202; 
verbose = 2;  
ipv4_tunnel_term_dip_sip_vrf_example(unit,accessPort1,providerPort1,providerPort2); 
 
 
we are configuring router1 
 
traffic from provider to access, tunnel is terminated for this VRF 
 
purpose: - check tunnel termination according to DIP SIP VRF 
    Send:  
             ---------------------------------------------------------
        eth: |    DA             |   SA              | VLAN           |
             ----------------------------------------------------------
             | router1_mac       | provider_mac      | provider_vlan_1|
             | 00:0C:00:02:00:00 | 00:00:07:00:01:00 | 10             |
             ----------------------------------------------------------
                 ------------------------------
             ip: |   SIP        | DIP         | 
                 ------------------------------
                 | provider_ip  | router1_ip  |
                 |  160.0.0.17  | 161.0.0.17  |
                 ------------------------------
                  
                GRE 4B
                   
                       -------------------------    
                   ip: |   SIP     | DIP       |    
                       -------------------------    
                       | 0.0.0.17  |  1.0.0.17 |
                       -------------------------
 
    Receive:
    Send:  
             ---------------------------------------------------------
        eth: |    DA             |   SA              | VLAN           |
             ----------------------------------------------------------
             | access_mac        | router1_mac      | access_vlan_1   |
             | 00:00:00:00:12:34 | 00:0C:00:02:00:00 | 10             |
             ----------------------------------------------------------
                  -------------------------    
              ip: |   SIP     | DIP       |    
                  -------------------------    
                  | 0.0.0.17  |  1.0.0.17 |
                  ------------------------- 
Packet flow: 
- get the in RIF (vlan = vsi = in rif) 
- terminate tunnel by DIP SIP VRF lookup
- route packet according to inner DIP, VRF 

 
 
traffic from provider to provider, tunnel is not terminated for this VRF 
 
purpose: - make sure tunnel is not terminated when VRF is different than previous packet
         
 
    Send:  
             ---------------------------------------------------------
        eth: |    DA             |   SA              | VLAN           |
             ----------------------------------------------------------
             | router1_mac       | provider_mac      | provider_vlan_1|
             | 00:0C:00:02:00:00 | 00:00:07:00:01:00 | 20             |
             ----------------------------------------------------------
                 ------------------------------
             ip: |   SIP        | DIP         | 
                 ------------------------------
                 | provider_ip  | router1_ip  |
                 |  160.0.0.17  | 161.0.0.17  |
                 ------------------------------
                  
                GRE 4B
                   
                       -------------------------    
                   ip: |   SIP     | DIP       |    
                       -------------------------    
                       | 0.0.0.17  |  1.0.0.17 |
                       -------------------------
 
    Receive:
             ---------------------------------------------------------
        eth: |    DA             |   SA              | VLAN           |
             ----------------------------------------------------------
             | provider_mac_2    | router1_mac       | provider_vlan_2|
             | 00:00:00:00:45:67 | 00:0C:00:02:00:00 | 30             |
             ----------------------------------------------------------
                 ------------------------------
             ip: |   SIP        | DIP         | 
                 ------------------------------
                 | provider_ip  | router1_ip  |
                 |  160.0.0.17  | 161.0.0.17  |
                 ------------------------------
                  
                GRE 4B
                   
                       -------------------------    
                   ip: |   SIP     | DIP       |    
                       -------------------------    
                       | 0.0.0.17  |  1.0.0.17 |
                       ------------------------- 
 
Packet flow: 
- get the in RIF (vlan = vsi = in rif) 
- don't terminate tunnel by DIP SIP VRF lookup. The key is not found because of VRF
- route packet according to outer DIP, VRF 
 
 
traffic from provider to access, terminate according to DIP, SIP 
 
purpose: make sure we can terminate according to DIP SIP using this tunnel termination mode 
 
 
    Send:  
             ---------------------------------------------------------
        eth: |    DA             |   SA              | VLAN           |
             ----------------------------------------------------------
             | router1_mac       | provider_mac      | provider_vlan_1|
             | 00:0C:00:02:00:00 | 00:00:07:00:01:00 | 20             |
             ----------------------------------------------------------
                 ------------------------------
             ip: |   SIP        | DIP         | 
                 ------------------------------
                 | provider_ip  | router1_ip  |
                 |  222.97.2.3 | 222.111.4.5 |
                 ------------------------------
                  
                GRE 4B
                   
                       -------------------------    
                   ip: |   SIP     | DIP       |    
                       -------------------------    
                       | 0.0.0.17  |  1.0.0.17 |
                       -------------------------
 
Packet flow: 
- get the in RIF (vlan = vsi = in rif) 
- terminate tunnel by DIP subnet and SIP subnet lookup (DIP: 222.111.0.2/16 and SIP: 222.111.0.1/12)
- route packet according to inner DIP, VRF 

*/ 
 
/*
 * Test Scenario: tunnel termination DIP subnet,SIP subnet. 
 * 
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/dpp/utility/cint_utils_multicast.c
 * cint ../../src/examples/dpp/utility/cint_utils_l3.c
 * cint ../../src/examples/dpp/utility/cint_utils_vlan.c
 * cint ../../src/examples/dpp/cint_ip_route.c
 * cint ../../src/examples/dpp/cint_ip_tunnel_term.c
 * cint
 * ipv4_tunnel_term_example_subnet(0,201,202);
 * exit;
 * 
 * Purpose: Terminate IP-tunnel using SIP subnet and DIP subnet lookup (SIP: 160.0.0.17/15 DIP: 161.0.0.17/12)
 * tx 1 psrc=201 data=0x000c000200000000070001008100000a08004518004a000100000a04f4dca0017beaa108fec645380036000100000a06af68000000110100001104890035002cabe400010100801001f5bcb300000101080a000a198401f6c97c2021
 *
 *   Received packets on unit 0 should be: 
 *  0x000c00020002000c00020000810000c8080045380036000100000906b068000000110100001104890035002cabe400010100801001f5bcb300000101080a000a198401f6c97c2021  
 *
 * Purpose: Terminate IP-Tunnel tunnel using SIP subnet and DIP subnet (SIP: 170.0.0.17/0 DIP: 171.0.0.17/2)
 * tx 1 psrc=201 data=0x000c000200000000070001008100001408004518004a000100000a04ba329b9b9b9b8ab3347b45380036000100000a06af68000000110100001104890035002cabe400010100801001f5bcb300000101080a000a198401f6c97c2021
 *
 *   Received packets on unit 0 should be: 
 * 0x000c00020001000c0002000081000064080045380036000100000906b068000000110100001104890035002cabe400010100801001f5bcb300000101080a000a198401f6c97c2021 
 */



/* ********* 
  Globals/Aux Variables
 */

/* debug prints */
int verbose = 1;
int global_counter_id;
/**********
  functions
 */

/******* Run example ******/

struct ip_tunnel_term_glbl_info_s{
    int tunnel_vrf; /* the router interface due to tunnel termination */
    int rif_vrf;   /* the router interface from RIF (VSI) */
    uint8 plus_1_protection;
    uint8 plus_1_mc_id;
    uint8 skip_ethernet_flag; /* indicate to skip ethernet after terminating ip tunnel. */
    int sip;  /* sip + dip to terminate 1 */
    int dip;
    int sip_mask;
    int dip_mask;
    int sip2; /* sip + dip to terminate 2 */
    int dip2;
    int sip_mask2;
    int dip_mask2;
    int sip3; /* sip + dip to terminate 3 */
    int dip3;
    int sip_mask3;
    int dip_mask3;
    int sip4; /* sip + dip to terminate 4 */
    int dip4;
    int sip_mask4;
    int dip_mask4;
    int tunnel_term_2_flags;
    int provider_vlan_1; /* packet using this vlan will be LL terminated. tunnel's vlan */
    int provider_vlan_2; /* packet using this vlan will be LL terminated. tunnel's vlan */
    int provider_vlan_3; /* packet using this vlan will be LL terminated. tunnel's vlan */
    bcm_mac_t my_mac;
    bcm_mac_t access_next_hop;
    bcm_mac_t provider_next_hop;
    int access_vlan_1; /* vlan to forward to access */
    int access_vlan_2; /* vlan to forward to access */
    int port_property; /* port property */
    int tunnel_type_1;
    int tunnel_type_2;
    int tunnel_type_3;
    int tunnel_type_4;
    bcm_ip6_t host_ipv6;
    bcm_ip6_t route_ipv6;
    bcm_ip6_t mask_ipv6;
    bcm_ip6_t sip_ipv6;
    bcm_ip6_t dip_ipv6;
    bcm_ip6_t dip2_ipv6;
    int my_vtep_index;
};

ip_tunnel_term_glbl_info_s ip_tunnel_term_glbl_info =
{
/* tunnel_vrf  | rif_vrf | plus_1_protection | plus_1_mc_id | skip_ethernet_flag */
3,               2,        0,                  6200,          0,
/* sip + dip to terminate 1 */
/* sip:      160.0.0.17      | dip:      161.0.0.17 */
0xA0000011,                    0xA1000011,
/* sip mask: 255.255.255.255 | dip mask: 255.255.255.255 */
0xFFFFFFFF,                    0xFFFFFFFF,
/* sip + dip to terminate 2 */
/* sip:170.0.0.17    | dip: 171.0.0.17 */
0xAA000011,                   0xAB000011,
/* sip mask: 0.0.0.0 | dip mask: 255.255.255.255 */
0x0,                   0xFFFFFFFF,
/* sip + dip to terminate 3 */
/* sip: 222.111.0.1      | dip: 222.111.0.2 */
0xDE6F0001,                0xDE6F0002,
/* sip mask: 255.240.0.0 | dip mask: 255.255.0.0 */
0xFFF00000,                0xFFFF0000,
/* sip + dip to terminate 4 */
/* sip: 127.13.14.15      | dip: 134.125.178.65 */
0x7F0D0E0F,                0x867DB241,
/* sip mask: 255.255.255.255 | dip mask: 255.255.255.255 */
0xFFFFFFFF,                    0xFFFFFFFF,
/* tunnel term flags*/
0,
/* provider vlans 1 | provider vlan 2 | provider vlan 3 */
10,                   20,               30,
/* my mac */
{0x00, 0x0c, 0x00, 0x02, 0x00, 0x00},
/*access next hop                    |   provider_next_hop*/
{0x00, 0x00, 0x00, 0x0, 0x12, 0x34},   {0x00, 0x00, 0x00, 0x0, 0x45, 0x67},
/* access valn 1 | access vlan 2 */
100,                200,
/* port property */
9,
/*tunnel_type_1,         tunnel_type_2,             tunnel_type_3,          tunnel_type_4      */
bcmTunnelTypeIp4In4,     bcmTunnelTypeIpAnyIn4,     bcmTunnelTypeIp6In4,    bcmTunnelTypeIp6In4,
/* create Ipv4 and Ipv6 hosts and routes*/
{0xEC, 0xEF, 0xEE, 0xED, 0xEC, 0xEB, 0xEA, 0xE9, 0xE8, 0xE7, 0xE6, 0xE5, 0xE4, 0xE3, 0xE2, 0xE1},
/* route IPv6 entry */
{0x35, 0x55, 0x55, 0x55, 0x66, 0x66, 0x66, 0x66, 0x77, 0x77, 0x77, 0x77, 0x88, 0x88, 0x88, 0x88},
/* route IPv6 mask */
{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
/*SIP IPV6*/
{0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x77, 0xFF, 0xFE, 0x00, 0x01, 0x00},
/*DIP IPV6*/
{0x35, 0x55, 0x55, 0x55, 0x66, 0x66, 0x66, 0x66, 0x77, 0x77, 0x77, 0x77, 0x88, 0x88, 0x88, 0x88},
/*DIP2 IPV6*/
{0xEC, 0xEF, 0xEE, 0xED, 0xEC, 0xEB, 0xEA, 0xE9, 0xE8, 0xE7, 0xE6, 0xE5, 0xE4, 0xE3, 0xE2, 0xE1},
/* my_vtep_index */
10
};



/* check if <DIP,SIP> lookup done in TCAM according to soc property */
int tcam_dip_sip_enabled(int unit){
    if(soc_property_get(unit , "bcm886xx_vxlan_tunnel_lookup_mode",2)==2 &&
       soc_property_get(unit , "bcm886xx_vxlan_enable",0))
        return 1;
    if(soc_property_get(unit , "bcm886xx_l2gre_tunnel_lookup_mode",2)==3 &&
       soc_property_get(unit , "bcm886xx_l2gre_enable",0))
        return 1;

    if(soc_property_get(unit , "bcm886xx_ip4_tunnel_termination_mode",3)==3 ||
       soc_property_get(unit , "bcm886xx_ip4_tunnel_termination_mode",3)==4 ||
       soc_property_get(unit , "bcm886xx_ip4_tunnel_termination_mode",3)==5)
     return 1;

    return 0;
}


void ip_tunnel_term_print_key(
    char *type, 
    bcm_tunnel_terminator_t *tunnel_term
)
{
    printf("%s  key: ", type);

    if(tunnel_term->type == bcmTunnelTypeIpAnyIn6) {
        printf("IPv6 DIP \n\r");
        return;
    }
    if(tunnel_term->sip_mask == 0) {
        printf("SIP: masked    ");
    }
    else if(tunnel_term->sip_mask == 0xFFFFFFFF) {
        printf("SIP:");
        print_ip_addr(tunnel_term->sip);
    }
    else if(tunnel_term->sip_mask == 0xFFFFFFFF) {
        printf("SIP:");
        print_ip_addr(tunnel_term->sip);
        printf("/0x%08x ",tunnel_term->sip_mask);
    }
    if(tunnel_term->dip_mask == 0) {
        printf("DIP: masked    ");
    }
    else if(tunnel_term->dip_mask == 0xFFFFFFFF) {
        printf("DIP:");
        print_ip_addr(tunnel_term->dip);
    }
    else if(tunnel_term->dip_mask == 0xFFFFFFFF) {
        printf("DIP:");
        print_ip_addr(tunnel_term->dip);
        printf("/0x%08x ",tunnel_term->dip_mask);
    }

    printf("RIF: 0x%08x ", tunnel_term->tunnel_if); 

    printf("\n\r");
}

int tcam_dip_sip_next_protocol_enabled(int unit){
    if(soc_property_get(unit , "bcm886xx_ip4_tunnel_termination_mode",3)==4 ||
       soc_property_get(unit , "bcm886xx_ip4_tunnel_termination_mode",3)==5)
     return 1;

    return 0;
}

int tcam_dip_sip_vrf_enabled(int unit) {
   return (soc_property_get(unit , "bcm886xx_ip4_tunnel_termination_mode",3)==6); 
}



int ip_tunnel_term_open_route_interfaces(int unit, int in_port, int out_port, int rif_vrf,int tunnel_vrf,bcm_if_t *tunnel_rif_id, bcm_if_t *egress_intf1, bcm_if_t *egress_intf2, bcm_if_t *ll_encap_id){
    int rv;
    /* my-mac for LL termination*/
    bcm_mac_t my_mac  =  ip_tunnel_term_glbl_info.my_mac;  /* my-MAC */
    /* packets income on this vlan with my-mac will be LL termination*/
    int in_vlan_1 = ip_tunnel_term_glbl_info.provider_vlan_1; 
    int in_vlan_2 = ip_tunnel_term_glbl_info.provider_vlan_2;
    bcm_if_t ing_intf_1;
    bcm_if_t ing_intf_2;
    create_l3_intf_s intf;

    /* dummy mac as this interface is resolved due to tunnel termination*/
    bcm_mac_t dummy_mac  = {0x00, 0x0c, 0x00, 0x00, 0x00, 0x00};  

    /* egress interfaces for outgoin side */
    int out_vlan_1 = ip_tunnel_term_glbl_info.access_vlan_1;
    int out_vlan_2 = ip_tunnel_term_glbl_info.access_vlan_2;
    int tunnel_rif = 300;

    bcm_mac_t next_hop_mac_1  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01};  /* my-MAC */
    bcm_mac_t next_hop_mac_2  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x02};  /* my-MAC */
    bcm_if_t ing_intf_out_1;
    bcm_if_t ing_intf_out_2;
    int encap_id[2];


    /* create ingress l3-intf 1 */
    rv = vlan__open_vlan_per_mc(unit, in_vlan_1, 0x1);  
    if (rv != BCM_E_NONE) {
    	printf("Error, open_vlan=%d, in unit %d \n", in_vlan_1, unit);
    }
    rv = bcm_vlan_gport_add(unit, in_vlan_1, in_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
    	printf("fail add port(0x%08x) to vlan(%d)\n", in_port, in_vlan_1);
      return rv;
    }
    
    intf.vsi = in_vlan_1;
    intf.my_global_mac = my_mac;
    intf.my_lsb_mac = my_mac;
    intf.vrf_valid = 1;
    intf.vrf = rif_vrf;
    intf.ingress_flags = BCM_L3_INGRESS_GLOBAL_ROUTE; /* enable public searches */
    
    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_1 = intf.rif;        
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create\n");
    }

    /* create ingress l3-intf 2 */
    rv = vlan__open_vlan_per_mc(unit, in_vlan_2, 0x1);  
    if (rv != BCM_E_NONE) {
    	printf("Error, open_vlan=%d, in unit %d \n", in_vlan_2, unit);
    }
    rv = bcm_vlan_gport_add(unit, in_vlan_2, in_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
    	printf("fail add port(0x%08x) to vlan(%d)\n", in_port, in_vlan_2);
      return rv;
    }
    
    intf.ingress_flags = BCM_L3_INGRESS_GLOBAL_ROUTE; /* enable public searches */
    intf.vsi = in_vlan_2;
    
    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_2 = intf.rif;        
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create\n");
    }

    /* create ingress l3-intf for tunnel termination */
    *tunnel_rif_id = tunnel_rif;
    
    rv = vlan__open_vlan_per_mc(unit, tunnel_rif, 0x1);  
    if (rv != BCM_E_NONE) {
    	printf("Error, open_vlan=%d, in unit %d \n", tunnel_rif, unit);
    }
    rv = bcm_vlan_gport_add(unit, tunnel_rif, in_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
    	printf("fail add port(0x%08x) to vlan(%d)\n", in_port, tunnel_rif);
      return rv;
    }
    
    intf.vsi = tunnel_rif;
    intf.my_global_mac = dummy_mac;
    intf.my_lsb_mac = dummy_mac;
    intf.vrf = tunnel_vrf;
    intf.ingress_flags = BCM_L3_INGRESS_GLOBAL_ROUTE; /* enable public searches */
    
    rv = l3__intf_rif__create(unit, &intf);
    *tunnel_rif_id = intf.rif;        
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create\n");
    }

   /* create egress route interfaces for MY-MAC encapsulation */
  /* create egress l3-intf 1 */
    rv = vlan__open_vlan_per_mc(unit, out_vlan_1, 0x1);  
    if (rv != BCM_E_NONE) {
    	printf("Error, open_vlan=%d, in unit %d \n", out_vlan_1, unit);
    }
    rv = bcm_vlan_gport_add(unit, out_vlan_1, out_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
    	printf("fail add port(0x%08x) to vlan(%d)\n", out_port, out_vlan_1);
      return rv;
    }
    
    intf.vsi = out_vlan_1;
    intf.my_global_mac = my_mac;
    intf.my_lsb_mac = my_mac;
    intf.vrf = 0;
    intf.ingress_flags = BCM_L3_INGRESS_GLOBAL_ROUTE; /* enable public searches */

    
    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_out_1 = intf.rif;        
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create\n");
    }

  /* create egress l3-intf 1 */
    rv = vlan__open_vlan_per_mc(unit, out_vlan_2, 0x1);  
    if (rv != BCM_E_NONE) {
    	printf("Error, open_vlan=%d, in unit %d \n", out_vlan_2, unit);
    }
    rv = bcm_vlan_gport_add(unit, out_vlan_2, out_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
    	printf("fail add port(0x%08x) to vlan(%d)\n", out_port, out_vlan_2);
      return rv;
    }
    
    intf.vsi = out_vlan_2;
    intf.ingress_flags = BCM_L3_INGRESS_GLOBAL_ROUTE; /* enable public searches */

    
    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_out_2 = intf.rif;        
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create\n");
    }

    /*** create egress object 1 ***/
    create_l3_egress_s l3eg;
    sal_memcpy(l3eg.next_hop_mac_addr, next_hop_mac_1 , 6);
    l3eg.out_tunnel_or_rif = ing_intf_out_1;
    l3eg.out_gport = out_port;
    l3eg.vlan = out_vlan_1;
    l3eg.fec_id = *egress_intf1;
    l3eg.arp_encap_id = encap_id[0];

    rv = l3__egress__create(unit,&l3eg);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
        return rv;
    }

    *egress_intf1 = l3eg.fec_id;
    encap_id[0] = l3eg.arp_encap_id;
    if(verbose >= 1) {
        printf("created FEC-id =0x%08x, \n", *egress_intf1);
        printf("next hop mac at encap-id %08x, \n", encap_id[0]);
    }


    /*** create egress object 2 ***/
    sal_memcpy(l3eg.next_hop_mac_addr, next_hop_mac_2 , 6);
    l3eg.out_tunnel_or_rif = ing_intf_out_2;
    l3eg.vlan = out_vlan_2;
    l3eg.fec_id = *egress_intf2;
    l3eg.arp_encap_id = encap_id[1];

    rv = l3__egress__create(unit,&l3eg);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
        return rv;
    }

    *egress_intf2 = l3eg.fec_id;
    encap_id[1] = l3eg.arp_encap_id;
    if(verbose >= 1) {
        printf("created FEC-id =0x%08x, \n", *egress_intf2);
        printf("next hop mac at encap-id %08x, \n", encap_id[1]);
    }
    *ll_encap_id = encap_id[1];
  return rv;
}

/*
 * buid two IP tunnels termination
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
 */
int ipv4_tunnel_term_example(int unit, int in_port, int out_port){
    int rv;
    bcm_if_t eg_intf_ids[2]; /* to include egress router interfaces */
    bcm_gport_t in_tunnel_gports[2]; /* to include IP tunnel interfaces interfaces */
    bcm_if_t eg_arp_id;


    /*** build tunnel terminations ***/
    rv = ipv4_tunnel_term_build_tunnel_terms(unit,in_port,out_port,eg_intf_ids,in_tunnel_gports); 
    if (rv != BCM_E_NONE) {
        printf("Error, ipv4_tunnel_term_build_tunnel_terms, in_port=%d, \n", in_port);
    }

    /*** add routes to egress interfaces ***/
    rv = ipv4_tunnel_term_add_routes(unit, eg_intf_ids, eg_arp_id);
    if (rv != BCM_E_NONE) {
        printf("Error, ipv4_tunnel_term_add_routes, eg_intf_ids=0x%08x,%0x%08x \n", eg_intf_ids[0],eg_intf_ids[1]);
    }

    if (tunnel_counter_enable) {
       
        int counter_id;
        uint64 counter_id64;

        rv=set_counter_source_and_engines_for_tunnel_counting(unit,&counter_id,in_port);
        if (rv != BCM_E_NONE) {
            printf("Error, set_counter_source_and_engines_for_tunnel_counting");
        }  

        COMPILER_64_SET(counter_id64, 0, counter_id);
	global_counter_id = counter_id;
        rv= bcm_port_stat_set(unit,in_tunnel_gports[0],bcmPortStatIngressPackets,counter_id64);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_stat_set");
        }

    }

    return rv;
}


/*
 * buid two IP tunnels termination with same DIP,SIP key but different VPN-ID 
 * The different will be in the above header: 
 *  In case header above is IPV4 then use Tunnel 1 
 *  In case header above is GRE then use Tunnel 2
 * 
 * Tunnnel 1: 
 *   -  sip   = 160.0.0.17
 *   -  dip   = 161.0.0.17
 *   -  dscp  = 10;
 *   -  ttl   = 60;
 *   -  type  = bcmTunnelTypeIp4In4;
 *   -  Vlan  = 100
 *   -  Dmac  = 00:00:00:00:cd:1d
 *   -  Smac  = 00:0c:00:02:00:00
 *   -  port_property = (use_port_property) ? 5 : none
 *  
 * Tunnnel 2: 
 *   -  sip   = 160.0.0.17
 *   -  dip   = 161.0.0.17
 *   -  dscp  = 10;
 *   -  ttl   = 60;
 *   -  type  = bcmTunnelTypeGre4In4;
 *   -  Vlan  = 100
 *   -  Dmac  = 20:00:00:00:cd:1d
 *   -  Smac  = 00:0c:00:02:00:00
 *   -  GRE header
 *  
 *   IPv4 Header above:
 *   -  DIP 1.0.0.17
 *  
 *   Input parameters:
 *   -  In-Port: Incoming port packet is injected to
 *   -  Out-Port: Expected outgoing port after tunnel termination
 *  
 *   Traffic:
 *   Send traffic :
 *    - Stream1 : Packet with DataoIPV4oTunnel1
 *    - Stream2 : Packet with DataoIPV4oTunnel2
 *   Receive traffic:
 *    - Both streams will go to Out-Port
 *    - Stream1 receive packet DataoIPV4oEthernet with VLAN 10 DA 00:0C:00:02:00:02
 *    - Stream2 receive packet DataoIPV4oEthernet with VLAN 20 DA 00:0C:00:02:00:01
 */
int ipv4_tunnel_term_next_protocol_example(int unit, int in_port, int out_port, uint8 use_port_property){
    int rv;
    int tunnel_vrf = ip_tunnel_term_glbl_info.tunnel_vrf; /* the router interface due to tunnel termination */
    int rif_vrf = ip_tunnel_term_glbl_info.rif_vrf;   /* the router interface from RIF (VSI) */
    int tunnel_rif;
    bcm_gport_t in_tunnel_gports[2];
    bcm_if_t eg_intf_ids[2]; /* to include egress router interfaces */
    bcm_gport_t port_property = 5;
   
    int egress_intf_1; /* egress object 1*/
    int egress_intf_2; /* egress object 2*/
    int ll_encap_id;  /* ARP pointer */

    /* tunnel term 1 info*/
    bcm_tunnel_terminator_t tunnel_term_1;
    int sip1 = ip_tunnel_term_glbl_info.sip; /* default: 160.0.0.17*/
    int dip1 = ip_tunnel_term_glbl_info.dip; /* default: 161.0.0.17*/
    int sip1_mask = ip_tunnel_term_glbl_info.sip_mask;  /* default: 255.255.255.255 */
    int dip1_mask = ip_tunnel_term_glbl_info.dip_mask; /* default: 255.255.255.255 */
    int type1 = bcmTunnelTypeIp4In4; /* header above is IPV4 */
    
    /* tunnel term 2 info: according to DIP  only*/
    bcm_tunnel_terminator_t tunnel_term_2;
    int sip2 = sip1;
    int dip2 = dip1;
    int sip2_mask = sip1_mask; 
    int dip2_mask = dip1_mask; 
    int type2 = bcmTunnelTypeGreAnyIn4; /* header above is GRE */

    /* 1+1 protection info */
    int failover_id, proection_mc_id = ip_tunnel_term_glbl_info.plus_1_mc_id;

    if (!tcam_dip_sip_next_protocol_enabled(unit)) {
        printf("Error, dip sip next protocol not enabled\n");
        return BCM_E_PARAM;
    }

    /* If using port property as part of the key, configure in_port's port property */
    if (use_port_property) {
        rv = bcm_port_class_set(unit, in_port, bcmPortClassFieldIngressVlanTranslation, port_property);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_class_set\n");
            return rv;
        }
    }

    if(ip_tunnel_term_glbl_info.plus_1_protection){
        rv = plus_1_protection_init(unit,&failover_id,&proection_mc_id);
        if (rv != BCM_E_NONE) {
            printf("Error, plus_1_protection_init\n");
            return rv;
        }
    }
    /*** build router interface ***/
    rv = ip_tunnel_term_open_route_interfaces(unit,in_port,out_port,rif_vrf, tunnel_vrf, &tunnel_rif,&egress_intf_1,&egress_intf_2, &ll_encap_id); 
    if (rv != BCM_E_NONE) {
        printf("Error, cip_tunnel_term_open_route_interfaces, in_port=%d, \n", in_port);
    }

    printf("TT tunnelrif=%d\n", tunnel_rif);

    /* store egress interfaces, for routing */
    eg_intf_ids[0] = egress_intf_1;
    eg_intf_ids[1] = egress_intf_2;

    /* create IP tunnel terminators */
    /*** create IP tunnel terminator 1 ***/
    bcm_tunnel_terminator_t_init(&tunnel_term_1);    
    tunnel_term_1.dip = dip1;
    tunnel_term_1.dip_mask = dip1_mask;
    tunnel_term_1.sip = sip1;
    tunnel_term_1.sip_mask = sip1_mask;
    tunnel_term_1.tunnel_if = tunnel_rif;
    tunnel_term_1.type = type1; /* this is IPv4 termination */
    if(ip_tunnel_term_glbl_info.plus_1_protection){
        tunnel_term_1.ingress_failover_id = failover_id;
        tunnel_term_1.failover_mc_group = proection_mc_id;
    }
    if (use_port_property) {
        BCM_PBMP_PORT_SET(tunnel_term_1.pbmp, port_property); /* Add port property to tunnel.  */
    }
    if (in_tunnel_gports[0] != 0) {
        BCM_GPORT_TUNNEL_ID_SET(tunnel_term_1.tunnel_id,in_tunnel_gports[0]);
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
    bcm_tunnel_terminator_t_init(&tunnel_term_2);
    tunnel_term_2.dip = dip2;
    tunnel_term_2.dip_mask = dip2_mask;
    tunnel_term_2.sip = sip2;
    tunnel_term_2.sip_mask = sip2_mask;
    tunnel_term_2.tunnel_if = -1; /* means don't overwite RIF */
    tunnel_term_2.type = type2;/* this is IPv4 termination */
    if(ip_tunnel_term_glbl_info.plus_1_protection){
        tunnel_term_2.ingress_failover_id = failover_id;
        tunnel_term_2.failover_tunnel_id = tunnel_term_1.tunnel_id;
        tunnel_term_1.failover_mc_group = proection_mc_id;
    }
    tunnel_term_2.tunnel_id = in_tunnel_gports[1];

    rv = bcm_tunnel_terminator_create(unit,&tunnel_term_2);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_terminator_create_2, in_port=%d, \n", in_port);
    }
    if(verbose >= 1) {
        printf("created tunnel terminator_2 =0x%08x, \n", tunnel_term_2.tunnel_id);
        ip_tunnel_term_print_key("created term 2",&tunnel_term_2);
    }
    in_tunnel_gports[1] = tunnel_term_2.tunnel_id;            
    eg_intf_ids[0] = egress_intf_1;
    eg_intf_ids[1] = egress_intf_2;

    /*** add routes to egress interfaces ***/
    rv = ipv4_tunnel_term_add_routes(unit, eg_intf_ids, ll_encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, ipv4_tunnel_term_add_routes, eg_intf_ids=0x%08x,%0x%08x \n", eg_intf_ids[0],eg_intf_ids[1]);
    }

    return rv;
}
/*Configure the tunnel types, required for the test*/
int ipv4_tunnel_term_dip_sip_vrf_next_protocol_ip6_gre6_greany_in4_example(int unit, int access_port, int provider_port1, int provider_port2, int ot_sip1_v4, int ot_dip1_v4, int ot_sip2_v4, int ot_dip2_v4, int ot_sip3_v4, int ot_dip3_v4){

    int rv = BCM_E_NONE;

    ip_tunnel_term_glbl_info.tunnel_type_1 = bcmTunnelTypeGre6In4;
    ip_tunnel_term_glbl_info.tunnel_type_3 = bcmTunnelTypeGreAnyIn4;
    ip_tunnel_term_glbl_info.tunnel_type_4 = bcmTunnelTypeIp6In4;

    ip_tunnel_term_glbl_info.sip = ot_sip1_v4;
    ip_tunnel_term_glbl_info.dip = ot_dip1_v4;
    ip_tunnel_term_glbl_info.sip4 = ot_sip2_v4;
    ip_tunnel_term_glbl_info.dip4 = ot_dip2_v4;
    ip_tunnel_term_glbl_info.sip3 = ot_sip3_v4;
    ip_tunnel_term_glbl_info.dip3 = ot_dip3_v4;

    ipv4_tunnel_term_dip_sip_vrf_example(unit, access_port, provider_port1, provider_port2);
    return rv;
}

/*Configure the tunnel types, required for the test*/
int ipv4_tunnel_term_dip_sip_vrf_next_protocol_ip4_gre4_greany_in4_example(int unit, int access_port, int provider_port1, int provider_port2){

    int rv = BCM_E_NONE;
    ip_tunnel_term_glbl_info.tunnel_type_1 = bcmTunnelTypeGre4In4;   /*#7*/
    ip_tunnel_term_glbl_info.tunnel_type_4 = bcmTunnelTypeIp4In4;    /*#1*/
    ip_tunnel_term_glbl_info.tunnel_type_3 = bcmTunnelTypeGreAnyIn4; /*#9*/

    ipv4_tunnel_term_dip_sip_vrf_example(unit, access_port, provider_port1, provider_port2);
    return rv;
}
/*Configure the tunnel types, required for the test*/
int ipv4_tunnel_term_dip_sip_vrf_next_protocol_greany_ipany_in4_example(int unit, int access_port, int access_port_2, int provider_port1, int provider_port_2){

    int rv = BCM_E_NONE;
    ip_tunnel_term_glbl_info.tunnel_type_1 = bcmTunnelTypeGreAnyIn4; /*#9*/
    ip_tunnel_term_glbl_info.tunnel_type_3 = bcmTunnelTypeGreAnyIn4; /*#9*/
    ip_tunnel_term_glbl_info.tunnel_type_4 = bcmTunnelTypeIpAnyIn4; /*#3*/



    ipv4_tunnel_term_dip_sip_vrf_example(unit, access_port, provider_port1, provider_port_2);
    return rv;

}

/*Configure the tunnel types, required for the test*/
int ipv4_tunnel_term_dip_sip_vrf_internal_example(int unit, int access_port, int provider_port1, int provider_port2){

    int rv = BCM_E_NONE;
    ip_tunnel_term_glbl_info.tunnel_type_1 = bcmTunnelTypeGre4In4; /*#7*/
    ip_tunnel_term_glbl_info.tunnel_type_2 = bcmTunnelTypeGre4In4; /*#7*/
    ip_tunnel_term_glbl_info.tunnel_type_3 = bcmTunnelTypeGre4In4; /*#7*/
    ip_tunnel_term_glbl_info.tunnel_type_4 = bcmTunnelTypeIpAnyIn4; /*#3*/

    ipv4_tunnel_term_dip_sip_vrf_example(unit, access_port, provider_port1, provider_port2);
    return rv;
}

/*
 * Test DIP SIP VRF tunnel termination mode.
 * See file's header for more informations.
 *
 * build IP tunnel termination
 * - SIP = 160.0.0.17
 * - DIP = 161.0.0.17
 * - VRF = 3
 * to terminate according to DIP SIP VRF
 *
 * build IP tunnel termination
 * - SIP = 134.125.178.65
 * - DIP = 127.13.14.15
 * - VRF = 3
 * to terminate according to DIP SIP VRF
 *
 * build IP tunnel termination
 * - SIP = 222.111.0.1
 * - DIP = 222.111.0.2
 * - VRF is invalid
 * to terminate according to DIP SIP
 *
 * build IP tunnel termination
 * - SIP = 170.0.0.17
 * - DIP = 171.0.0.17
 * - VRF = 3
 * - port_property = 9
 * to terminate according to port property
 */
int ipv4_tunnel_term_dip_sip_vrf_example(int unit, int access_port, int provider_port1, int provider_port2){

    int rv = BCM_E_NONE;
    bcm_tunnel_terminator_t tunnel_term;

    if (!tcam_dip_sip_vrf_enabled(unit)) {
        printf("Error, dip sip vrf not enabled\n");
        return BCM_E_PARAM;
    }

    /*** build router interfaces ***/

    /* router interface */

    /* router interface, for tunnel to be terminated:
     * - inRif is used to terminate tunnel's LL
     * - inRif.vrf value is used in tunnel termination key.
       - outRif is used for tunnel's DIP routing */
    create_l3_intf_s l3_intf;
    sal_memset(&l3_intf, 0, sizeof (l3_intf));
    l3_intf.vrf = ip_tunnel_term_glbl_info.tunnel_vrf; /* vrf is used in tunnel termination */
    l3_intf.vrf_valid = 1;
    l3_intf.my_global_mac = l3_intf.my_lsb_mac = ip_tunnel_term_glbl_info.my_mac; /* my-MAC */
    l3_intf.vsi = ip_tunnel_term_glbl_info.provider_vlan_1; /* rif */
    l3_intf.mtu_valid = 1;
    l3_intf.mtu = 0;
    l3_intf.mtu_forwarding = 0;
    l3_intf.ingress_flags = BCM_L3_INGRESS_GLOBAL_ROUTE; /* enable public searches */
    rv = l3__intf_rif__create(unit, &l3_intf);
    if (rv != BCM_E_NONE) {
        printf("Error in l3__intf_rif__create \n");
    }

    /* router interface
       - outRif is used to forward after tunnel termination */
    sal_memset(&l3_intf, 0, sizeof (l3_intf));
    l3_intf.my_global_mac = l3_intf.my_lsb_mac = ip_tunnel_term_glbl_info.my_mac;
    l3_intf.vsi = ip_tunnel_term_glbl_info.access_vlan_1;
    rv = l3__intf_rif__create(unit, &l3_intf);
    if (rv != BCM_E_NONE) {
        printf("Error in l3__intf_rif__create \n");
    }

    /* router interface for tunnel's IP to be routed, not terminated
       - inRif is used to terminate tunnel's LL
       - inRif.vrf value is NOT used in tunnel termination key. */
    sal_memset(&l3_intf, 0, sizeof (l3_intf));
    l3_intf.vrf = ip_tunnel_term_glbl_info.rif_vrf; /* vrf is NOT used in tunnel termination */
    l3_intf.vrf_valid = 1;
    l3_intf.my_global_mac = l3_intf.my_lsb_mac = ip_tunnel_term_glbl_info.my_mac; /* my-MAC */
    l3_intf.vsi = ip_tunnel_term_glbl_info.provider_vlan_2; /* rif */
    l3_intf.mtu_valid = 1;
    l3_intf.mtu = 0;
    l3_intf.mtu_forwarding = 0;
    l3_intf.ingress_flags = BCM_L3_INGRESS_GLOBAL_ROUTE; /* enable public searches */
    rv = l3__intf_rif__create(unit, &l3_intf);
    if (rv != BCM_E_NONE) {
        printf("Error in l3__intf_rif__create \n");
    }

    /* create IP tunnel terminators: DIP SIP VRF, IP.protocol, GRE.protocol_type lookup */
    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.dip = ip_tunnel_term_glbl_info.dip; /* default: 161.0.0.17*/
    tunnel_term.dip_mask = ip_tunnel_term_glbl_info.dip_mask;  /* default: 255.255.255.255 */
    tunnel_term.sip = ip_tunnel_term_glbl_info.sip; /* default: 160.0.0.17*/
    tunnel_term.sip_mask = ip_tunnel_term_glbl_info.sip_mask;  /* default: 255.255.255.255 */
    tunnel_term.vrf = ip_tunnel_term_glbl_info.tunnel_vrf;
    tunnel_term.type = ip_tunnel_term_glbl_info.tunnel_type_1;
    rv = bcm_tunnel_terminator_create(unit,&tunnel_term);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_terminator_create, \n");
    }

    /* create IP tunnel terminations: DIP, SIP, VRF, IP.protocol: MPLS/IPv4/IPv6. */
    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.dip =ip_tunnel_term_glbl_info.dip4; /* default: 134.125.178.65 */
    tunnel_term.dip_mask = ip_tunnel_term_glbl_info.sip_mask4;  /*default: 255.255.255.255 */
    tunnel_term.sip = ip_tunnel_term_glbl_info.sip4; /* default: 127.13.14.15 */
    tunnel_term.sip_mask = ip_tunnel_term_glbl_info.sip_mask4; /* default: 255.255.255.255 */
    tunnel_term.vrf = ip_tunnel_term_glbl_info.tunnel_vrf;
    tunnel_term.type = ip_tunnel_term_glbl_info.tunnel_type_4;
    rv = bcm_tunnel_terminator_create(unit,&tunnel_term);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_terminator_create, \n");
    }

    /* create IP tunnel terminations: DIP SIP lookup (vrf is invalid) */
    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.dip = ip_tunnel_term_glbl_info.dip3; /* default: 222.111.0.2 */
    tunnel_term.dip_mask = ip_tunnel_term_glbl_info.dip_mask3; /* default: 255.255.0.0 */
    tunnel_term.sip = ip_tunnel_term_glbl_info.sip3; /* default: 222.111.0.1 */
    tunnel_term.sip_mask = ip_tunnel_term_glbl_info.sip_mask3; /* default: 255.240.0.0 */
    tunnel_term.vrf = BCM_IF_INVALID;  /* invalid vrf */
    tunnel_term.type = ip_tunnel_term_glbl_info.tunnel_type_3;
    rv = bcm_tunnel_terminator_create(unit,&tunnel_term);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_terminator_create \n");
    }

    /*create IP tunnel terminators: DIP SIP VRF, IP.protocol, GRE.protocol_type, port_property lookup  */
    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.dip = ip_tunnel_term_glbl_info.dip2; /* default: dip: 171.0.0.17 */
    tunnel_term.dip_mask = ip_tunnel_term_glbl_info.dip_mask2; /* default: 255.255.255.255 */
    tunnel_term.sip = ip_tunnel_term_glbl_info.sip2; /* default: 170.0.0.17 */
    tunnel_term.sip_mask = ip_tunnel_term_glbl_info.dip_mask2; /* default: 255.255.255.255 */
    tunnel_term.vrf = ip_tunnel_term_glbl_info.tunnel_vrf;
    tunnel_term.type = ip_tunnel_term_glbl_info.tunnel_type_2;
    BCM_PBMP_PORT_SET(tunnel_term.pbmp, ip_tunnel_term_glbl_info.port_property); /* Add port property to tunnel. default: 9 */
    rv = bcm_tunnel_terminator_create(unit,&tunnel_term);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_terminator_create \n");
    }

    /* add route */

    /* add route for DIP after tunnel termination */
    /* create FEC and LL for access routing */
    create_l3_egress_s l3_egress;
    sal_memset(&l3_egress, 0, sizeof (l3_egress));
    l3_egress.vlan = ip_tunnel_term_glbl_info.access_vlan_1;
    l3_egress.next_hop_mac_addr = ip_tunnel_term_glbl_info.access_next_hop; /* next-hop */
    l3_egress.out_gport = access_port;
    rv = l3__egress__create(unit, &l3_egress);

    /* add route after tunnel termination */
    l3_ipv4_route_entry_utils_s route_entry;
    sal_memset(&route_entry, 0, sizeof(route_entry));
    route_entry.address =  0x01000011; /* 1.0.0.17 */
    route_entry.mask = 0xfffffff0;
    rv = l3__ipv4_route__add(unit,
                    route_entry,
                    ip_tunnel_term_glbl_info.tunnel_vrf,
                    l3_egress.fec_id /* FEC */
    );
    if (rv != BCM_E_NONE) {
          printf("Error, in l3__ipv4_route__add\n");
          return rv;
    }

    /* add route after tunnel termination */
    rv = add_route_ip6(unit,
                       ip_tunnel_term_glbl_info.route_ipv6,
                       ip_tunnel_term_glbl_info.mask_ipv6,
                       ip_tunnel_term_glbl_info.tunnel_vrf,
                       l3_egress.fec_id);
    if (rv != BCM_E_NONE) {
          printf("Error, in l3__ipv6_route__add\n");
          return rv;
    }
	
    /* add route for tunnel's DIP (no tunnel termination) */
    /* create FEC and LL for tunnel's IP routing */
    sal_memset(&l3_egress, 0, sizeof (l3_egress));
    l3_egress.vlan = ip_tunnel_term_glbl_info.provider_vlan_3;
    l3_egress.next_hop_mac_addr = ip_tunnel_term_glbl_info.provider_next_hop; /* next-hop */
    l3_egress.out_gport = provider_port2;
    rv = l3__egress__create(unit, &l3_egress);
    if (rv != BCM_E_NONE) {
          printf("Error, in l3__egress__create\n");
          return rv;
      }

    /* add route for tunnel's IP (for rif_vrf) */
    sal_memset(&route_entry, 0, sizeof(route_entry));
    route_entry.address =  ip_tunnel_term_glbl_info.dip; /* default: 161.0.0.17*/
    route_entry.mask = 0xfffffff0;
    rv = l3__ipv4_route__add(unit,
                    route_entry,
                    ip_tunnel_term_glbl_info.rif_vrf,
                    l3_egress.fec_id /* FEC */
    );
    if (rv != BCM_E_NONE) {
          printf("Error, in l3__ipv4_route__add\n");
          return rv;
    }

    /* add route for tunnel's IP (for tunnel_vrf) */
    sal_memset(&route_entry, 0, sizeof(route_entry));
    route_entry.address =  ip_tunnel_term_glbl_info.dip; /* default: 161.0.0.17*/
    route_entry.mask = 0xfffffff0;
    rv = l3__ipv4_route__add(unit,
                    route_entry,
                    ip_tunnel_term_glbl_info.tunnel_vrf,
                    l3_egress.fec_id /* FEC */
    );
    if (rv != BCM_E_NONE) {
          printf("Error, in l3__ipv4_route__add\n");
          return rv;
    }

    /* add route for tunnel's DIP (no tunnel termination) */
    sal_memset(&route_entry, 0, sizeof(route_entry));
    route_entry.address =  ip_tunnel_term_glbl_info.dip4; /* default: 134.125.178.65 */
    route_entry.mask = 0xfffffff0;
    rv = l3__ipv4_route__add(unit,
                    route_entry,
                    ip_tunnel_term_glbl_info.tunnel_vrf,
                    l3_egress.fec_id /* FEC */
    );
    if (rv != BCM_E_NONE) {
          printf("Error, in l3__ipv4_route__add\n");
          return rv;
    }

    /* add route for tunnel's IP (for port_property) */
    sal_memset(&route_entry, 0, sizeof(route_entry));
    route_entry.address =  ip_tunnel_term_glbl_info.dip2; /* default: 171.0.0.17*/
    route_entry.mask = 0xfffffff0;
    rv = l3__ipv4_route__add(unit,
                    route_entry,
                    ip_tunnel_term_glbl_info.tunnel_vrf,
                    l3_egress.fec_id /* FEC */
    );
    if (rv != BCM_E_NONE) {
          printf("Error, in l3__ipv4_route__add\n");
          return rv;
    }

    return rv;

}

/*
 * buid two IP tunnels termination
 * Tunnnel 1: 
 *   -  sip   = 160.0.0.17
 *   -  dip   = 161.0.0.17
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
 */

int ipv4_tunnel_term_example_dvapi(int unit, int in_port, int out_port){
    return ipv4_tunnel_term_example(unit,in_port,out_port);
}


int ipv4_tunnel_term_example_subnet(int unit, int in_port, int out_port) {

    ip_tunnel_term_glbl_info.sip_mask  = 0xFFFE0000;  /* 255.254.0.0   */
    ip_tunnel_term_glbl_info.dip_mask  = 0xFFF00000;  /* 255.240.0.0   */
    ip_tunnel_term_glbl_info.sip_mask2 = 0x0;         /* 0 */
    ip_tunnel_term_glbl_info.dip_mask2 = 0xC0000000;  /* 192.0.0.0     */ 

    return ipv4_tunnel_term_example(unit,in_port,out_port);
}
int ipv4_tunnel_term_sip_dip_ip4_ip6_ipany_in4_priority (int unit, int in_port, int out_port) {

    ip_tunnel_term_glbl_info.sip_mask  = 0x00000000;
    ip_tunnel_term_glbl_info.dip_mask  = 0xFFFFFFFF;
    ip_tunnel_term_glbl_info.sip_mask2 = 0x00000000;
    ip_tunnel_term_glbl_info.dip_mask2 = 0xFFFFFFFF;    
    ip_tunnel_term_glbl_info.sip_mask3 = 0xFF000000;  
    ip_tunnel_term_glbl_info.dip_mask3 = 0xFFFFFFFF;  
    ip_tunnel_term_glbl_info.sip_mask4 = 0x00000000; 
    ip_tunnel_term_glbl_info.dip_mask4 = 0xFFFFFFFF;  
    ip_tunnel_term_glbl_info.dip2 = ip_tunnel_term_glbl_info.dip;
    ip_tunnel_term_glbl_info.sip2 = ip_tunnel_term_glbl_info.sip;
    ip_tunnel_term_glbl_info.dip3 = 0xAB000011;
    ip_tunnel_term_glbl_info.sip3 = 0xAA000011;
    ip_tunnel_term_glbl_info.dip4 = ip_tunnel_term_glbl_info.dip3;
    ip_tunnel_term_glbl_info.sip4 = ip_tunnel_term_glbl_info.sip3;
    return tunnel_term_example_with_4_tunnels(unit,in_port,out_port);
}
/* Test for basic Ip and Gre4oIpv4 tunnel termination. */
/* The tunnel SIP and DIP values are written both in SEM and TCAM DBs. */
/* The SIP is being fully masked and the priority of the match is being exercised. */

int tunnel_term_sip_dip_gre4_gre6_greany_in4_priority (int unit, int in_port, int out_port) {

    ip_tunnel_term_glbl_info.sip_mask  = 0x00000000;
    ip_tunnel_term_glbl_info.dip_mask  = 0xFFFFFFFF;
    ip_tunnel_term_glbl_info.sip_mask2 = 0x00000000;
    ip_tunnel_term_glbl_info.dip_mask2 = 0xFFFFFFFF;    
    ip_tunnel_term_glbl_info.sip_mask3 = 0xFF000000;  
    ip_tunnel_term_glbl_info.dip_mask3 = 0xFFFFFFFF;  
    ip_tunnel_term_glbl_info.sip_mask4 = 0x00000000; 
    ip_tunnel_term_glbl_info.dip_mask4 = 0xFFFFFFFF;  
    ip_tunnel_term_glbl_info.dip2 = ip_tunnel_term_glbl_info.dip;
    ip_tunnel_term_glbl_info.sip2 = ip_tunnel_term_glbl_info.sip;
    ip_tunnel_term_glbl_info.dip3 = 0xAB000011;
    ip_tunnel_term_glbl_info.sip3 = 0xAA000011;
    ip_tunnel_term_glbl_info.dip4 = ip_tunnel_term_glbl_info.dip3;
    ip_tunnel_term_glbl_info.sip4 = ip_tunnel_term_glbl_info.sip3; 
    ip_tunnel_term_glbl_info.tunnel_type_1 = bcmTunnelTypeGreAnyIn4;
    ip_tunnel_term_glbl_info.tunnel_type_2 = bcmTunnelTypeGreAnyIn4;
    ip_tunnel_term_glbl_info.tunnel_type_3 = bcmTunnelTypeGre6In4;
    ip_tunnel_term_glbl_info.tunnel_type_4 = bcmTunnelTypeGre6In4;
    return tunnel_term_example_with_4_tunnels(unit,in_port,out_port);
}

int tunnel_term_example_with_4_tunnels(int unit, int in_port, int out_port) {

    int rv;
    bcm_if_t eg_intf_ids[2]; /* to include egress router interfaces */
    bcm_if_t eg_encap_id;
    bcm_gport_t in_tunnel_gports[4]; /* to include IP tunnel interfaces interfaces */
    int tunnel_vrf = ip_tunnel_term_glbl_info.tunnel_vrf; /* the router interface due to tunnel termination */
    int rif_vrf = ip_tunnel_term_glbl_info.rif_vrf;   /* the router interface from RIF (VSI) */
    int tunnel_rif; /* RIF from tunnel termination*/
    
    int egress_intf_1; /* egress object 1*/
    int egress_intf_2; /* egress object 2*/    
    int ll_encap_id; /*ARP pointer*/
    /* tunnel term 1 info*/
    bcm_tunnel_terminator_t tunnel_term_1;
    /* tunnel term 2 info: according to DIP  only*/
    bcm_tunnel_terminator_t tunnel_term_2;
    /* tunnel term 3 info*/
    bcm_tunnel_terminator_t tunnel_term_3;
    /* tunnel term 4 info: according to DIP  only*/
    bcm_tunnel_terminator_t tunnel_term_4;
    /* 1+1 protection info */
    int failover_id, proection_mc_id = ip_tunnel_term_glbl_info.plus_1_mc_id;

    /* if TCAM not enabled reset SIP from key1 */
    if (!tcam_dip_sip_enabled(unit)) {
        sip1 = sip1_mask = 0;
    }

    if(ip_tunnel_term_glbl_info.plus_1_protection){
        rv = plus_1_protection_init(unit,&failover_id,&proection_mc_id);
        if (rv != BCM_E_NONE) {
            printf("Error, plus_1_protection_init\n");
        }
    }
    /*** build router interface ***/
    rv = ip_tunnel_term_open_route_interfaces(unit,in_port,out_port,rif_vrf, tunnel_vrf, &tunnel_rif, &egress_intf_1, &egress_intf_2, &ll_encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, cip_tunnel_term_open_route_interfaces, in_port=%d, \n", in_port);
    }

    printf("TT tunnelrif=%d\n", tunnel_rif);

    /* store egress interfaces, for routing */
    eg_intf_ids[0] = egress_intf_1;
    eg_intf_ids[1] = egress_intf_2;

    /* create IP tunnel terminators */
    /*** create IP tunnel terminator 1 ***/
    bcm_tunnel_terminator_t_init(&tunnel_term_1);    
    tunnel_term_1.dip = ip_tunnel_term_glbl_info.dip;
    tunnel_term_1.dip_mask = ip_tunnel_term_glbl_info.dip_mask;
    tunnel_term_1.sip = ip_tunnel_term_glbl_info.sip;
    tunnel_term_1.sip_mask = ip_tunnel_term_glbl_info.sip_mask;
    tunnel_term_1.tunnel_if = tunnel_rif;
    tunnel_term_1.type = ip_tunnel_term_glbl_info.tunnel_type_1; /* this is IPv4 termination */
    if(ip_tunnel_term_glbl_info.plus_1_protection){
        tunnel_term_1.ingress_failover_id = failover_id;
        tunnel_term_1.failover_mc_group = proection_mc_id;
    }

    /* check skip ethernet */
    if (ip_tunnel_term_glbl_info.skip_ethernet_flag) {
        tunnel_term_1.flags |= BCM_TUNNEL_TERM_ETHERNET;
    }

    if (in_tunnel_gports[0] != 0) {
        BCM_GPORT_TUNNEL_ID_SET(tunnel_term_1.tunnel_id,in_tunnel_gports[0]);
        tunnel_term_1.flags |= BCM_TUNNEL_TERM_TUNNEL_WITH_ID;
    }

    rv = bcm_tunnel_terminator_create(unit,&tunnel_term_1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_terminator_create_1, in_port=%d, \n", in_port);
    }
    if(verbose >= 1) {
        printf("created tunnel terminator_1 =0x%08x, tunnel_term_1.dip =0x%08x \n", tunnel_term_1.tunnel_id, tunnel_term_1.dip);
        ip_tunnel_term_print_key("created term 1",&tunnel_term_1);
    }

    in_tunnel_gports[0] = tunnel_term_1.tunnel_id;
    
    /*** create IP tunnel terminator 2 - in ISEM ***/
    bcm_tunnel_terminator_t_init(&tunnel_term_2);    
    tunnel_term_2.dip = ip_tunnel_term_glbl_info.dip2;
    tunnel_term_2.dip_mask = ip_tunnel_term_glbl_info.dip_mask2;
    tunnel_term_2.tunnel_if = tunnel_rif;
    tunnel_term_2.type = ip_tunnel_term_glbl_info.tunnel_type_2; /* this is IPv4 termination */
    
    if (in_tunnel_gports[1] != 0) {
        BCM_GPORT_TUNNEL_ID_SET(tunnel_term_2.tunnel_id,in_tunnel_gports[1]);
        tunnel_term_2.flags |= BCM_TUNNEL_TERM_TUNNEL_WITH_ID;
    }

    rv = bcm_tunnel_terminator_create(unit,&tunnel_term_2);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_terminator_create_2, in_port=%d, \n", in_port);
    }
    if(verbose >= 1) {
        printf("created tunnel terminator_2 =0x%08x, tunnel_term_2.dip =0x%08x \n", tunnel_term_2.tunnel_id, tunnel_term_2.dip);
        ip_tunnel_term_print_key("created term 2",&tunnel_term_2);
    }
    in_tunnel_gports[1] = tunnel_term_2.tunnel_id;
    
     /*** create IP tunnel terminator 3 ***/
    bcm_tunnel_terminator_t_init(&tunnel_term_3);    
    tunnel_term_3.dip = ip_tunnel_term_glbl_info.dip3;
    tunnel_term_3.dip_mask = ip_tunnel_term_glbl_info.dip_mask3;
    tunnel_term_3.sip = ip_tunnel_term_glbl_info.sip3;
    tunnel_term_3.sip_mask = ip_tunnel_term_glbl_info.sip_mask3;
    tunnel_term_3.tunnel_if = tunnel_rif;
    tunnel_term_3.type = ip_tunnel_term_glbl_info.tunnel_type_3; /* this is IPv4 termination */
    if(ip_tunnel_term_glbl_info.plus_1_protection){
        tunnel_term_3.ingress_failover_id = failover_id;
        tunnel_term_3.failover_mc_group = proection_mc_id;
    }
    
    /* check skip ethernet */
    if (ip_tunnel_term_glbl_info.skip_ethernet_flag) {
        tunnel_term_3.flags |= BCM_TUNNEL_TERM_ETHERNET;
    }

    if (in_tunnel_gports[2] != 0) {
        BCM_GPORT_TUNNEL_ID_SET(tunnel_term_3.tunnel_id,in_tunnel_gports[2]);
        tunnel_term_3.flags |= BCM_TUNNEL_TERM_TUNNEL_WITH_ID;
    }

    rv = bcm_tunnel_terminator_create(unit,&tunnel_term_3);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_terminator_create_3, in_port=%d, \n", in_port);
    }
    if(verbose >= 1) {
        printf("created tunnel terminator_3 =0x%08x, tunnel_term_3.dip =0x%08x \n", tunnel_term_3.tunnel_id, tunnel_term_3.dip);
        ip_tunnel_term_print_key("created term 3",&tunnel_term_3);
    }

    in_tunnel_gports[2] = tunnel_term_3.tunnel_id;
    
    /*** create IP tunnel terminator 4 - in ISEM ***/
    bcm_tunnel_terminator_t_init(&tunnel_term_4);    
    tunnel_term_4.dip = ip_tunnel_term_glbl_info.dip4;
    tunnel_term_4.dip_mask = ip_tunnel_term_glbl_info.dip_mask4;
    tunnel_term_4.tunnel_if = tunnel_rif;
    tunnel_term_4.type = ip_tunnel_term_glbl_info.tunnel_type_4; /* this is IPv4 termination */
    
    if (in_tunnel_gports[3] != 0) {
        BCM_GPORT_TUNNEL_ID_SET(tunnel_term_4.tunnel_id, in_tunnel_gports[3]);
        tunnel_term_4.flags |= BCM_TUNNEL_TERM_TUNNEL_WITH_ID;
    }

    rv = bcm_tunnel_terminator_create(unit, &tunnel_term_4);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_terminator_create_4, in_port=%d, \n", in_port);
    }
    if(verbose >= 1) {
        printf("created tunnel terminator_4 =0x%08x, tunnel_term_4.dip =0x%08x \n", tunnel_term_4.tunnel_id, tunnel_term_4.dip);
        ip_tunnel_term_print_key("created term 4",&tunnel_term_4);
    }
    in_tunnel_gports[3] = tunnel_term_4.tunnel_id;
    
    /* Add Ipv4 and Ipv6 hosts and routes, pointing to the egress objects*/
    ipv4_tunnel_term_add_routes (unit, eg_intf_ids, ll_encap_id);
    
    return rv;    
}
/* The following cint function calls the tunnel_term_example_term_mode_1 cint */
/* It configures GreAnyIn4, Gre4In4 and Gre6In4 tunnels in the TCAM */

int tunnel_term_sip_dip_gre4_gre6_greany_in4 (int unit, int in_port, int out_port) {

    ip_tunnel_term_glbl_info.sip_mask  = 0xFFFFFFFF;
    ip_tunnel_term_glbl_info.dip_mask  = 0xFFFFFFFF;
    ip_tunnel_term_glbl_info.sip_mask2 = 0xFFFFFFFF;
    ip_tunnel_term_glbl_info.dip_mask2 = 0xFFFFFFFF;
    ip_tunnel_term_glbl_info.sip_mask3 = 0xFFFFFFFF;
    ip_tunnel_term_glbl_info.dip_mask3 = 0xFFFFFFFF;
    ip_tunnel_term_glbl_info.tunnel_type_1 = bcmTunnelTypeGreAnyIn4;
    ip_tunnel_term_glbl_info.tunnel_type_2 = bcmTunnelTypeGre4In4;
    ip_tunnel_term_glbl_info.tunnel_type_3 = bcmTunnelTypeGre6In4;

    return tunnel_term_example_term_mode_1(unit, in_port, out_port);
}

/* The following cint function calls the tunnel_term_example_term_mode_2 cint */
/* It configures GreAnyIn4, Gre4In4 and Gre6In4 tunnels in the TCAM */
/* The SIP and DIP values for the tunnels are masked, to exercise subnet mask */

int tunnel_term_sip_dip_gre4_gre6_greany_in4_subnet (int unit, int in_port, int out_port) {

    ip_tunnel_term_glbl_info.sip_mask  = 0xFFF0000;
    ip_tunnel_term_glbl_info.dip_mask  = 0xFF00000;
    ip_tunnel_term_glbl_info.sip_mask2 = 0xFFFF000;
    ip_tunnel_term_glbl_info.dip_mask2 = 0xFFF0000;
    ip_tunnel_term_glbl_info.sip_mask3 = 0xF000000;
    ip_tunnel_term_glbl_info.tunnel_type_1 = bcmTunnelTypeGreAnyIn4;
    ip_tunnel_term_glbl_info.tunnel_type_2 = bcmTunnelTypeGre4In4;
    ip_tunnel_term_glbl_info.tunnel_type_3 = bcmTunnelTypeGre6In4;

    return tunnel_term_example_term_mode_1(unit, in_port, out_port);
}

/* The following cint function calls the tunnel_term_example_term_mode_2 cint */
/* It configures IpAnyIn4, Ip4In4 and Ip6In4 tunnels in the TCAM */

int tunnel_term_sip_dip_ip4_ip6_ipany_in4 (int unit, int in_port, int out_port) {

    ip_tunnel_term_glbl_info.sip_mask  = 0xFFFFFFFF;
    ip_tunnel_term_glbl_info.dip_mask  = 0xFFFFFFFF;
    ip_tunnel_term_glbl_info.sip_mask2 = 0xFFFFFFFF;
    ip_tunnel_term_glbl_info.dip_mask2 = 0xFFFFFFFF;
    ip_tunnel_term_glbl_info.sip_mask3 = 0xFFFFFFFF;
    ip_tunnel_term_glbl_info.dip_mask3 = 0xFFFFFFFF;
    ip_tunnel_term_glbl_info.tunnel_type_1 = bcmTunnelTypeIpAnyIn4;
    ip_tunnel_term_glbl_info.tunnel_type_2 = bcmTunnelTypeIp4In4;
    ip_tunnel_term_glbl_info.tunnel_type_3 = bcmTunnelTypeIp6In4;

    return tunnel_term_example_term_mode_1(unit, in_port, out_port);
}

/* The following cint function calls the tunnel_term_example_term_mode_2 cint */
/* It configures IpAnyIn4, Ip4In4 and Ip6In4 tunnels in the TCAM */
/* The SIP and DIP values for the tunnels are masked, to exercise subnet mask */

int tunnel_term_sip_dip_ip4_ip6_ipany_in4_subnet (int unit, int in_port, int out_port) {

    ip_tunnel_term_glbl_info.sip_mask  = 0xFFF0000;
    ip_tunnel_term_glbl_info.dip_mask  = 0xFF00000;
    ip_tunnel_term_glbl_info.sip_mask2 = 0xFFFF000;
    ip_tunnel_term_glbl_info.dip_mask2 = 0xFFF0000;
    ip_tunnel_term_glbl_info.sip_mask3 = 0xF000000;
    ip_tunnel_term_glbl_info.dip_mask3 = 0xFF00000;
    ip_tunnel_term_glbl_info.tunnel_type_1 = bcmTunnelTypeIpAnyIn4;
    ip_tunnel_term_glbl_info.tunnel_type_2 = bcmTunnelTypeIp4In4;
    ip_tunnel_term_glbl_info.tunnel_type_3 = bcmTunnelTypeIp6In4;

    return tunnel_term_example_term_mode_1(unit, in_port, out_port);
}
/* Basic Cint configuration, which adds 3 tunnels */
/* All tunnels are configured in the TCAM DB */
/* All configurations exercising termination mode 1, call this function */

int tunnel_term_example_term_mode_1(int unit, int in_port, int out_port) {

    int rv;
    bcm_if_t eg_intf_ids[2]; /* to include egress router interfaces */
    bcm_if_t eg_encap_id;
    bcm_gport_t in_tunnel_gports[3]; /* to include IP tunnel interfaces interfaces */
    int tunnel_vrf = ip_tunnel_term_glbl_info.tunnel_vrf; /* the router interface due to tunnel termination */
    int rif_vrf = ip_tunnel_term_glbl_info.rif_vrf;   /* the router interface from RIF (VSI) */
    int tunnel_rif; /* RIF from tunnel termination*/
    
    int egress_intf_1; /* egress object 1*/
    int egress_intf_2; /* egress object 2*/
    int ll_encap_id; /*ARP pointer*/
    /* tunnel term 1 info*/
    bcm_tunnel_terminator_t tunnel_term_1;
    /* tunnel term 2 info: according to DIP  only*/
    bcm_tunnel_terminator_t tunnel_term_2;
    /* tunnel term 3 info*/
    bcm_tunnel_terminator_t tunnel_term_3;
    
    /*** build router interface ***/
    rv = ip_tunnel_term_open_route_interfaces(unit,in_port,out_port,rif_vrf, tunnel_vrf, &tunnel_rif, &egress_intf_1, &egress_intf_2, &ll_encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, cip_tunnel_term_open_route_interfaces, in_port=%d, \n", in_port);
    }

    printf("TT tunnelrif=%d\n", tunnel_rif);

    /* store egress interfaces, for routing */
    eg_intf_ids[0] = egress_intf_1;
    eg_intf_ids[1] = egress_intf_2;

    /* create IP tunnel terminators */
    /*** create IP tunnel terminator 1 ***/
    bcm_tunnel_terminator_t_init(&tunnel_term_1);
    tunnel_term_1.dip = ip_tunnel_term_glbl_info.dip;
    tunnel_term_1.dip_mask = ip_tunnel_term_glbl_info.dip_mask;
    tunnel_term_1.sip = ip_tunnel_term_glbl_info.sip;
    tunnel_term_1.sip_mask = ip_tunnel_term_glbl_info.sip_mask;
    tunnel_term_1.tunnel_if = tunnel_rif;
    tunnel_term_1.type = ip_tunnel_term_glbl_info.tunnel_type_1; /* this is IPv4 termination */

    rv = bcm_tunnel_terminator_create(unit,&tunnel_term_1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_terminator_create_1, in_port=%d, \n", in_port);
    }
    if(verbose >= 1) {
        printf("created tunnel terminator_1 =0x%08x, tunnel_term_1.dip =0x%08x \n", tunnel_term_1.tunnel_id, tunnel_term_1.dip);
        ip_tunnel_term_print_key("created term 1",&tunnel_term_1);
    }

    in_tunnel_gports[0] = tunnel_term_1.tunnel_id;
    
    /*** create IP tunnel terminator 2 - in ISEM ***/
    bcm_tunnel_terminator_t_init(&tunnel_term_2);
    tunnel_term_2.dip = ip_tunnel_term_glbl_info.dip2;
    tunnel_term_2.dip_mask = ip_tunnel_term_glbl_info.dip_mask2;
    tunnel_term_2.tunnel_if = tunnel_rif;
    tunnel_term_2.sip = ip_tunnel_term_glbl_info.sip2;
    tunnel_term_2.sip_mask = ip_tunnel_term_glbl_info.sip_mask2;
    tunnel_term_2.type = ip_tunnel_term_glbl_info.tunnel_type_2; /* this is IPv4 termination */
    
    rv = bcm_tunnel_terminator_create(unit,&tunnel_term_2);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_terminator_create_2, in_port=%d, \n", in_port);
    }
    if(verbose >= 1) {
        printf("created tunnel terminator_2 =0x%08x, tunnel_term_2.dip =0x%08x \n", tunnel_term_2.tunnel_id, tunnel_term_2.dip);
        ip_tunnel_term_print_key("created term 2",&tunnel_term_2);
    }
    in_tunnel_gports[1] = tunnel_term_2.tunnel_id;
    
     /*** create IP tunnel terminator 3 ***/
    bcm_tunnel_terminator_t_init(&tunnel_term_3);
    tunnel_term_3.dip = ip_tunnel_term_glbl_info.dip3;
    tunnel_term_3.dip_mask = ip_tunnel_term_glbl_info.dip_mask3;
    tunnel_term_3.sip = ip_tunnel_term_glbl_info.sip3;
    tunnel_term_3.sip_mask = ip_tunnel_term_glbl_info.sip_mask3;
    tunnel_term_3.tunnel_if = tunnel_rif;
    tunnel_term_3.type = ip_tunnel_term_glbl_info.tunnel_type_3; /* this is IPv4 termination */
   
    rv = bcm_tunnel_terminator_create(unit,&tunnel_term_3);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_terminator_create_3, in_port=%d, \n", in_port);
    }
    if(verbose >= 1) {
        printf("created tunnel terminator_3 =0x%08x, tunnel_term_3.dip =0x%08x \n", tunnel_term_3.tunnel_id, tunnel_term_3.dip);
        ip_tunnel_term_print_key("created term 3",&tunnel_term_3);
    }

    in_tunnel_gports[2] = tunnel_term_3.tunnel_id;
    
    /* Add Ipv4 and Ipv6 hosts and routes, pointing to the egress objects*/
    ipv4_tunnel_term_add_routes (unit, eg_intf_ids, ll_encap_id);
    
    return rv;
}

int tunnel_counter_enable=0;
/*count terminated packet per inlif using the OAM counter engine*/
int ipv4_tunnel_term_example_lif_counter (int unit, int in_port, int out_port){
    tunnel_counter_enable=1;
    return ipv4_tunnel_term_example_dvapi(unit,in_port,out_port);
}

int plus_1_protection_init(int unit, int *failover_id, int *mc_id){

    int rv = BCM_E_NONE;
    if(verbose >= 1) {
        printf("run with 1+1 protection! \n");
    }

    rv = bcm_failover_create(unit, BCM_FAILOVER_INGRESS, failover_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_failover_create (failover_id)\n");
        print rv;
        return rv;
    }
    if(verbose >= 1) {
        printf("create Failover-ID: %d\n", *failover_id);
    }

    /* create MC for protection */
	egress_mc = 0;
    rv = multicast__open_mc_group(unit, &mc_id, BCM_MULTICAST_TYPE_L3);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__open_mc_group\n");
        return rv;
    }

    return rv;


}

int ipv4_tunnel_term_build_tunnel_terms(int unit, int in_port, int out_port,bcm_if_t *eg_intf_ids, bcm_gport_t *in_tunnel_gports)
{
    int rv;

    int tunnel_vrf = ip_tunnel_term_glbl_info.tunnel_vrf; /* the router interface due to tunnel termination */
    int rif_vrf = ip_tunnel_term_glbl_info.rif_vrf;   /* the router interface from RIF (VSI) */
    int tunnel_rif; /* RIF from tunnel termination*/

    int egress_intf_1; /* egress object 1*/
    int egress_intf_2; /* egress object 2*/
    int ll_encap_id;
    /* tunnel term 1 info*/
    bcm_tunnel_terminator_t tunnel_term_1;
    int sip1 = ip_tunnel_term_glbl_info.sip; /* default: 160.0.0.17*/
    int dip1 = ip_tunnel_term_glbl_info.dip; /* default: 161.0.0.17*/
    int sip1_mask = ip_tunnel_term_glbl_info.sip_mask;   /* default: 0xFFFFFFFF */
    int dip1_mask = ip_tunnel_term_glbl_info.dip_mask;   /* default: 0xFFFFFFFF */

    /* tunnel term 2 info: according to DIP  only*/
    bcm_tunnel_terminator_t tunnel_term_2;
    int sip2 = ip_tunnel_term_glbl_info.sip2; /* 0.0.0.10*/
    int dip2 = ip_tunnel_term_glbl_info.dip2; /* 171.0.0.17*/
    int sip2_mask = ip_tunnel_term_glbl_info.sip_mask2; /* 0x0 */
    int dip2_mask = ip_tunnel_term_glbl_info.dip_mask2; /* 0xFFFFFFFF */

    /* 1+1 protection info */
    int failover_id, proection_mc_id = ip_tunnel_term_glbl_info.plus_1_mc_id;

    /* if TCAM not enabled reset SIP from key1 */
    if (!tcam_dip_sip_enabled(unit)) {
        sip1 = sip1_mask = 0;
    }

    if(ip_tunnel_term_glbl_info.plus_1_protection){
        rv = plus_1_protection_init(unit,&failover_id,&proection_mc_id);
        if (rv != BCM_E_NONE) {
            printf("Error, plus_1_protection_init\n");
        }
    }
    /*** build router interface ***/
    rv = ip_tunnel_term_open_route_interfaces(unit,in_port,out_port,rif_vrf, tunnel_vrf, &tunnel_rif,&egress_intf_1,&egress_intf_2, &ll_encap_id); 
    if (rv != BCM_E_NONE) {
        printf("Error, cip_tunnel_term_open_route_interfaces, in_port=%d, \n", in_port);
    }

    printf("TT tunnelrif=%d\n", tunnel_rif);

    /* store egress interfaces, for routing */
    eg_intf_ids[0] = egress_intf_1;
    eg_intf_ids[1] = egress_intf_2;

    /* create IP tunnel terminators */
    /*** create IP tunnel terminator 1 ***/
    bcm_tunnel_terminator_t_init(&tunnel_term_1);    
    tunnel_term_1.dip = dip1;
    tunnel_term_1.dip_mask = dip1_mask;
    tunnel_term_1.sip = sip1;
    tunnel_term_1.sip_mask = sip1_mask;
    tunnel_term_1.tunnel_if = tunnel_rif;
    tunnel_term_1.type = bcmTunnelTypeIpAnyIn4; /* this is IPv4 termination */
    if(ip_tunnel_term_glbl_info.plus_1_protection){
        tunnel_term_1.ingress_failover_id = failover_id;
        tunnel_term_1.failover_mc_group = proection_mc_id;
    }

    /* check skip ethernet */
    if (ip_tunnel_term_glbl_info.skip_ethernet_flag) {
        tunnel_term_1.flags |= BCM_TUNNEL_TERM_ETHERNET;
    }

    if (in_tunnel_gports[0] != 0) {
        BCM_GPORT_TUNNEL_ID_SET(tunnel_term_1.tunnel_id,in_tunnel_gports[0]);
        tunnel_term_1.flags |= BCM_TUNNEL_TERM_TUNNEL_WITH_ID;
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
    bcm_tunnel_terminator_t_init(&tunnel_term_2);
    tunnel_term_2.dip = dip2;
    tunnel_term_2.dip_mask = dip2_mask;
    tunnel_term_2.sip = sip2;
    tunnel_term_2.sip_mask = sip2_mask;
    tunnel_term_2.tunnel_if = -1; /* means don't overwite RIF */
    tunnel_term_2.type = bcmTunnelTypeIpAnyIn4;/* this is IPv4 termination */
    if(ip_tunnel_term_glbl_info.plus_1_protection){
        tunnel_term_2.ingress_failover_id = failover_id;
        tunnel_term_2.failover_tunnel_id = tunnel_term_1.tunnel_id;
        tunnel_term_1.failover_mc_group = proection_mc_id;
    }
    tunnel_term_2.tunnel_id = in_tunnel_gports[1];

    /* check skip ethernet */
    if (ip_tunnel_term_glbl_info.skip_ethernet_flag) {
        tunnel_term_2.flags |= BCM_TUNNEL_TERM_ETHERNET;
    }

    if (in_tunnel_gports[1] != 0) {
        BCM_GPORT_TUNNEL_ID_SET(tunnel_term_2.tunnel_id,in_tunnel_gports[1]);
        tunnel_term_2.flags |= BCM_TUNNEL_TERM_TUNNEL_WITH_ID;
    }
    
    if (ip_tunnel_term_glbl_info.tunnel_term_2_flags) {
        tunnel_term_2.flags |= BCM_TUNNEL_TERM_USE_OUTER_DSCP | BCM_TUNNEL_TERM_USE_OUTER_TTL;
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


int ipv4_tunnel_term_add_routes(int unit, int *eg_intf_ids, int eg_encap_id){
    int rv = BCM_E_NONE;

    /* internal IP after termination */
    int inter_subnet = 0x01000011; /* 1.0.0.17*/
    int inter_mask =    0xffffff00; /* 255.255.255.0*/
    int route_address = 0x03000011;
    int subnet_address = 0xAC000011;
    int egress_intf_1; /* egress object 1*/
    int egress_intf_2; /* egress object 2*/
    int egress_encap_id;
    int tunnel_vrf = ip_tunnel_term_glbl_info.tunnel_vrf; /* the router interface due to tunnel termination */
    int rif_vrf = ip_tunnel_term_glbl_info.rif_vrf;   /* the router interface from RIF (VSI) */
    bcm_ip6_t route_ipv6 = ip_tunnel_term_glbl_info.route_ipv6;
    bcm_ip6_t mask_ipv6 = ip_tunnel_term_glbl_info.mask_ipv6;
    bcm_ip6_t host_ipv6 = ip_tunnel_term_glbl_info.host_ipv6;
    
    egress_intf_2 = eg_intf_ids[0];
    egress_intf_1 = eg_intf_ids[1];    
    egress_encap_id = eg_encap_id; 
    
    rv = add_route(unit, inter_subnet, inter_mask, tunnel_vrf, egress_intf_1);
    if (rv != BCM_E_NONE) {
        printf("Error, add_route 1, egress_intf_1=%d, \n", egress_intf_1);
    }
    
    rv = add_route(unit, route_address, inter_mask, tunnel_vrf, egress_intf_2); 
        if (rv != BCM_E_NONE) {
            printf("Error, create egress object, in_port=%d, \n", in_port);
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

/* set port_property value for {dip,sip,vrf,port_property} tunnel termination */
int ipv4_tunnel_term_port_property_set(int unit, int in_port, int port_property_value)
{
    int rv;

    rv = bcm_port_class_set(unit, in_port, bcmPortClassFieldIngressTunnelTerminated, port_property_value);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_class_set\n");
        return rv;
    }

    return rv;
}
