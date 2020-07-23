/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * 
 * **************************************************************************************************************************************************
 * 
 *  
 * Network diagram
 * 
 * We configure Router A
 *                                          _________
 *                                         | ROUTER B|------host1             
 *                                host2    |_________|                        
 *                                    |       |access_vlan_1, access_port_1                
 *                ____________________|_______|_______________________________
 *               |                   |         |                             |
 *               |                   | ROUTER A| (intra DC IP)    DC FABRIC  |
 *               | native_out_vlan   |_________|                             |
 *               | provider_vlan,      /                                     |
 *               | provider_vlan_2    /                                      |
 *               | vni1       _______/___                                    |
 *               |           |          |                                    |
 *               |           | ROUTER C |                                    |
 *               |           |__________|                                    |
 *               |            /                                              |
 *               |         /                                                 |
 *               |_____ /____________________________________________________|
 *                ____/___                                          
 *               |  TOR1 |                                   
 *               |_______|                                   
 *               |       |                                   
 *               |       |                                   
 *               |_______|                                   
 *               | VM:A0 |                                   
 *               |_______|                                   
 *                                                           
 * 
 * Configuration:
 * 
 * soc properties:                                                        
 * #enable VXLAN according to SIP/DIP                                     
 * #0:none 1:dip_sip termination 2: dip_termination 3: both               
 * bcm886xx_vxlan_enable=1                                                
 * #1:seperated in SEM 2:for joined in TCAM                               
 * bcm886xx_vxlan_tunnel_lookup_mode=1                                    
 * #disable conflicting features                                          
 * bcm886xx_ip4_tunnel_termination_mode=0                                 
 * bcm886xx_l2gre_enable=0                                                
 * bcm886xx_ether_ip_enable=0                                             
 * For Arad/+ devices: bcm886xx_auxiliary_table_mode=1                                
 * #enable ROO for vxlan                                                  
 * bcm886xx_roo_enable=1                                                  
 *                                                                        
 * cint;                                                                  
 * cint_reset();                                                          
 * exit;  
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c   
 * cint ../../../../src/examples/dpp/utility/cint_utils_l3.c
 * cint ../../../../src/examples/dpp/utility/cint_utils_roo.c
 * cint ../../../../src/examples/dpp/cint_ip_route.c                      
 * cint ../../../../src/examples/dpp/cint_ip_tunnel.c                     
 * cint ../../../../src/examples/dpp/cint_ip_tunnel_term.c                
 * cint ../../../../src/examples/dpp/cint_port_tpid.c                     
 * cint ../../../../src/examples/dpp/cint_advanced_vlan_translation_mode.c
 * cint ../../../../src/examples/dpp/cint_mact.c                          
 * cint ../../../../src/examples/dpp/cint_vswitch_metro_mp.c              
 * cint ../../../../src/examples/dpp/cint_vxlan.c
 * cint ../../../../src/examples/dpp/cint_vxlan_roo.c                         
 * cint ../../../../src/examples/dpp/cint_field_gre_learn_data_rebuild.c  
 * cint                                                                   
 * int unit=0;  
 * int access_port = 200;  
 * int provider_port = 201;  
 * int vpn_id = -1; 
 * int add_routing_table_entry = 0; 
 * vxlan_roo_run(unit, access_port, provider_port, vpn_id, add_routing_table_entry);                            
 * 
 * 
 *  UC Traffic from RouterB to VM:A0
 * 
 *  Routing to overlay: host1_mac to VM:A0
 *  Purpose: - check lookup in host table result as vxlan encapsulation and native ethernet encapsulation
 *  Packet flow:
 *  - 
 *  
 *  
 *  
 *  
 *  
 *    Send:                                           
 *             ------------------------------------   
 *        eth: |    DA       |     SA      | VLAN |
 *             ------------------------------------
 *             | routerA_mac | routerB_mac |  v1  |  
 *             ------------------------------------
 * 
 *                 ------------------------ 
 *             ip: |   SIP     | DIP      | 
 *                 ------------------------ 
 *                 | host10_ip |  vmA0_ip | 
 *                 ------------------------ 
 *    Receive:
 *             ------------------------------------   
 *        eth: |    DA       |     SA      | VLAN |
 *             ------------------------------------
 *             | routerC_mac | routerA_mac |  v2  |  
 *             ------------------------------------
 * 
 *                 ----------------------- 
 *             ip: |   SIP     | DIP     | 
 *                 ----------------------- 
 *                 | routerA_ip| tor1_ip | 
 *                 ----------------------- 
 * 
 *                udp:
 * 
 *                   vxlan:  ---------
 *                           |  VNI  |
 *                           ---------
 *                           |  vni  |
 *                           ---------
 *    
 *                                 ------------------------------------ 
 *                     native eth: |    DA       |     SA      | VLAN | 
 *                                 ------------------------------------ 
 *                                 | tor1_mac    | routerA_mac |  v3  | 
 *                                 ------------------------------------ 
 *                                                                      
 *                                     ----------------------         
 *                         native ip:  |   SIP     | DIP     |        
 *                                     ----------------------        
 *                                     | host10_ip | vmA0_ip |        
 *                                     ----------------------
 *
 *
 * vxlan header.vni is resovled from outVsi -> VNI
 * Note: in Jericho, outVSI come from outRif.
 *       in QAX, outVSI may come from outRif or native-LL.vlan 
 * 
 *
 *
 *
 *
 *
 *
 *
 * routerA_mac: native DA/SA and ip_tunnel's SA/DA 00:0c:00:02:00:00 see ip_tunnel_my_mac_get from cint_ip_tunnel.c  
 * routerB_mac: native SA/DA: 00:00:00:00:05:81   
 * v1:          native vlan: 15                
 * host10_ip:   native SIP/DIP: 160:17:17:17       
 * vmA0_ip:     native DIP/SIP: 171:17:17:17 
 * 
 * routerC_mac: ip tunnel's DA/SA: 20:00:00:00:cd:1d next_hop_mac2 from cint_ip_tunnel.c 
 * routerA_mac: ip tunnel's SA/DA: 00:0c:00:02:00:00 ip_tunnel_my_mac_get from cint_ip_tunnel.c
 * v2:          ip tunnel's vid: 100 out_vlan from cint_ip_tunnel.c
 * routerA_ip:  ip tunnel's SIP/DIP. 170.0.0.17 See sip2 from cint_ip_tunnel.c
 * tor1_ip:     ip tunnel's DIP/SIP. 171.0.0.17 See dip2 from cint_ip_tunnel.c
 * vni:         vxlan header's vni: 5000 vxlan_key_vni from cint_vxlan.c
   v3:          native vlan: 20 
 * 
 * overlay to routing: VM:A0 to host1_mac 
 * Purpose: - check overlay termination and native ethernet termination.
 *          - check the packet is routed.
 *          - Encapsulated with native ethernet.
 * 
 *    Send:
 *             ------------------------------------   
 *        eth: |    DA       |     SA      | VLAN |
 *             ------------------------------------
 *             | routerA_mac | routerC_mac |  v2  |  
 *             ------------------------------------
 * 
 *                 ----------------------- 
 *             ip: |   SIP   | DIP       | 
 *                 ----------------------- 
 *                 | tor1_ip | routerA_ip| 
 *                 ----------------------- 
 * 
 *                udp:
 * 
 *                   vxlan:  ---------
 *                           |  VNI  |
 *                           ---------
 *                           |  vni  |
 *                           ---------
 *    
 *                                 ---------------------------------
 *                     native eth: |    DA       |     SA   | VLAN | 
 *                                 ---------------------------------
 *                                 | routerA_mac | tor1_mac |  v3  | 
 *                                 ---------------------------------
 *                                                                      
 *                                     ----------------------         
 *                         native ip:  |   SIP   |   DIP     |        
 *                                     ----------------------        
 *                                     | vmA0_ip | host10_ip |        
 *                                     ----------------------
 * 
 *   
 *    Receive:                                           
 *             ------------------------------------   
 *        eth: |    DA       |     SA      | VLAN |
 *             ------------------------------------
 *             | routerB_mac | routerA_mac |  v1  |  
 *             ------------------------------------
 * 
 *                 ------------------------ 
 *             ip: |   SIP   |   DIP      | 
 *                 ------------------------ 
 *                 | vmA0_ip |  host10_ip | 
 *                 ------------------------  
 * 
 *
 *
 * Native Vlan editing scenarios:
 * In Jericho, native egress Vlan editing is supported by EVE.
 *             Overlay egress Vlan editing is not supported 
 * In QAX, native egress Vlan editing is supported by native EVE
 *             Overlay egress Vlan editing is supported by EVE.
 * To perform native EVE, we need to allocate a full bank in EEDB dedicated to native EVE. 
 * To configure native EVE see vxlan_roo_native_default_out_ac(). 
 * 
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/dpp/utility/cint_utils_multicast.c
 * cint ../../src/examples/dpp/utility/cint_utils_l3.c
 * cint ../../src/examples/dpp/utility/cint_utils_vlan.c
 * cint ../../src/examples/dpp/utility/cint_utils_roo.c
 * cint ../../src/examples/dpp/internal/cint_native_egress_vlan_editing_lif_vsi_lookup.c
 * cint ../../src/examples/dpp/cint_ip_route.c
 * cint ../../src/examples/dpp/cint_port_tpid.c
 * cint ../../src/examples/dpp/cint_advanced_vlan_translation_mode.c
 * cint ../../src/examples/dpp/utility/cint_utils_l2.c
 * cint ../../src/examples/dpp/cint_field_gre_learn_data_rebuild.c
 * cint ../../src/examples/dpp/cint_ip_tunnel.c
 * cint ../../src/examples/dpp/cint_ip_tunnel_term.c
 * cint ../../src/examples/dpp/cint_mact.c
 * cint ../../src/examples/dpp/cint_vswitch_metro_mp.c
 * cint ../../src/examples/dpp/cint_qos_l3_rif_cos.c
 * cint ../../src/examples/dpp/cint_vxlan.c
 * cint ../../src/examples/dpp/cint_vxlan_roo.c
 * cint
 * vxlan_roo_run(0,200,202,-1,0,0);
 * # only for Jericho+ and QAX 
 * vxlan_roo_native_default_out_ac(0);
 * # only for Jericho/Qmx
 * vxlan_roo_out_ac_perform_native_ve(0, 200, 202); 
 * exit;
 *
 * vxlan- packet from user to vxlan. One native vlan tag 
 * Purpose: - check native VLAN editing, add 1 tag. 
 *
 * tx 1 psrc=200 data=0x000c00020000000000000581810000140800450000350000000080000ea86f6f6f6faba1a1a1000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be: 
 * Source port: 0, Destination port: 0 
 * Data: 0x20000000cd1d000c00020000810000640800450b006b0000000032113355aa000011ab00001150005555005700000800000000138800000007000123000c000200008100400f080045000035000000007f000fa86f6f6f6faba1a1a1000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 *
 *
 * vxlan- packet from user to vxlan. untagged native ethernet
 * Purpose: - check native VLAN editing, untag.
 * cint
 * vxlan_roo_modify_native_vlan_number(0, 0);
 * exit;
 * tx 1 psrc=200 data=0x000c00020000000000000581810000140800450000350000000080000ea86f6f6f6faba1a1a1000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be: 
 * Source port: 0, Destination port: 0 
 * Data: 0x20000000cd1d000c00020000810000640800450b00670000000032113359aa000011ab00001150005555005300000800000000138800000007000123000c00020000080045000035000000007f000fa86f6f6f6faba1a1a1000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 *
 *
 * vxlan- packet from user to vxlan. double tagged native ethernet
 * Purpose: - check native vlan editing, set 2 tags.
 * cint
 * vxlan_roo_modify_native_vlan_number(0, 2);
 *exit; 
 * tx 1 psrc=200 data=0x000c00020000000000000581810000140800450000350000000080000ea86f6f6f6faba1a1a1000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be: 
 * Source port: 0, Destination port: 0 
 * Data: 0x20000000cd1d000c00020000810000640800450b006f0000000032113351aa000011ab00001150005555005b00000800000000138800000007000123000c000200008100400f9100007b080045000035000000007f000fa86f6f6f6faba1a1a1000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 */


/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 */

verbose = 2; 


struct vxlan_roo_s {
    /* native headers */
    int native_dip; 
    int native_out_vlan; 
    int native_inner_vlan; 
    bcm_mac_t native_da; 
    bcm_ip6_t native_dip6; /* For ipv6 mode feature */

    /* outer vlan for the Eth of the overlay: */
    int overlay_out_vlan; 
    /* inner vlan for the Eth of the overlay: */
    int overlay_in_vlan; 

    /* native arp/LL */
    int native_ll_outlif; /* interface of type encap: outlif (native arp eedb index) */

    /* vrf */
    int vrf; 

    /* core VSI. */
    int vpn; 

    /* access VSI. VSI at access side. Note: VPN is the VSI at core side */
    int access_vsi; 

    /* overlay arp/LL */
    bcm_if_t overlay_ll_outlif; /* interface of type encap: outlif (or overlay arp eedb index) */ 

    /* vxlan tunnel gport. Shouldn't be set by user. Readonly variable */
    bcm_gport_t vxlan_port_id;  

    /* ip tunnel initiator gport. Shouldn't be set by user. Readonly variable */
    bcm_gport_t ip_tunnel_initiator_port_id; 

    /* From QAX: EVEC command for native vlan editing. */
    int native_vlan_action_id; 

    int enable_add_entry_routing_table; /* feature: add entry to routing table: available for Jericho and above */
    
    uint8 use_vrrp; /* Feature: use vrrp to l2 terminate mymac, instead of global mymac */

    uint8 use_ipv6; /* Feature: use ipv6 instead of ipv4 */

    int add_double_vlan_overlay; /* Feature: add double vlan at overlay Ethernet header: available for Jericho and above */

    int first_level_ecmp_enable; /* use ECMP in first level FEC */

    int second_level_ecmp_enable; /* use ECMP in second level FEC */

	int use_slb_hashing_for_ecmp; /* use slb hashing for ecmp (vs. configured ECMP hashing)*/

};

vxlan_roo_s g_vxlan_roo = 
/* native headers: 
 native dip: 171:161:161:161 | native outer vlan | native inner vlan | native da                        */
{0xABA1A1A1,                   20,                 21,                 ip_tunnel_glbl_info.tunnel_2.sa, 

/* Native dip ipv6 for ipv6 mode feature: */
    {0x12, 0x34, 0x56, 0x78, 0x90, 0x23, 0x45, 0x67, 0x89, 0x01, 0x09, 0x87, 0x65, 0x43, 0x21, 0x99},
/* overlay headers:
 * overlay out vlan | overlay in vlan */
 0x64,                0xC8, 

/* native ll outlif */
0x3006,

/* vrf */
123, 

/* core VSI */
15, 
/* access VSI */
35,

 /* overlay ll outlif */
 0, 
/* vxlan tunnel gport. */
 0, 
/*  ip tunnel initiator gport */
 0,
/* native vlan evec command */
 0, 
/* feature: add routing table entry */
 0, 
/* Feature: use vrrp to l2 terminate mymac, instead of global mymac */
 0,
/* Feature: use ipv6 instead of ipv4 */
 0, 
/* Feature: Feature: add overlay double vlan */
 0,
/* first level ECMP */
 0,
/* second level ECMP */
 0,
/* use Flexible-Hashing for ECMP*/
 0
};


void vxlan_roo_init(vxlan_roo_s *param) {
    if (param != NULL) {
        sal_memcpy(&g_vxlan_roo, param, sizeof(g_vxlan_roo));
    }
}

void vxlan_roo_struct_get(vxlan_roo_s *param) {
    sal_memcpy( param, &g_vxlan_roo, sizeof(g_vxlan_roo));
}

/* Use this version of ipv4_tunnel_build_tunnels only if:
 * you have already built the LL of the tunnel,
 * you want to use a specific vlan, 
 * you want to get the out tunnel interface for both tunnels (intf_ids return out tunnel intfs for tunnel0 and FEC for tunnel1)
 * Otherwise, use ipv4_tunnel_build_tunnels
 * 
 * Description:
 * the ip tunnel will be created and will connect the ip to the LL 
 * tunnel_itfs: out tunnel interface for both tunnels (intf_ids return out tunnel intfs for tunnel0 and FEC for tunnel1)
 * For more details, see: ipv4_tunnel_build_tunnels
 */
int vxlan_roo_ipv4_tunnel_build_tunnels(
   int unit, int provider_port, bcm_if_t ll_eep_id, int out_vlan, bcm_if_t* intf_ids, bcm_gport_t *tunnel_gports, bcm_if_t* tunnel_itfs){
    int rv;
    int ing_intf_in;
    int ing_intf_out; 
    int fec[3] = {0x0,0x0,0x0};
    int flags = 0;
    int encap_id[3]={0x0,0x0,0x0};

    /* my-MAC {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  */
    bcm_mac_t mac_address  = ip_tunnel_my_mac_get();

    /* tunnel 1 info*/
    bcm_tunnel_initiator_t tunnel_1;
    ip_tunnel_s tunnel_info_1 = ip_tunnel_glbl_info.tunnel_1;
    bcm_mac_t next_hop_mac  = tunnel_info_1.da; /* default: 00:00:00:00:cd:1d} */
    int tunnel_itf1=0;

    /* tunnel 2 info */
    bcm_tunnel_initiator_t tunnel_2;
    ip_tunnel_s tunnel_info_2 = ip_tunnel_glbl_info.tunnel_2;
    bcm_mac_t next_hop_mac2  = tunnel_info_2.da; /* default: {0x20:00:00:00:cd:1d} */
    int tunnel_itf2=0;

    /* tunnel 2.2 info (for ECMP) */
    bcm_tunnel_initiator_t tunnel_2_2;
    int tunnel_itf2_2=0;


	/*** create egress router interface ***/
	rv = vlan__open_vlan_per_mc(unit,out_vlan,0x1); 
	if (rv != BCM_E_NONE) {
		printf("Error, open_vlan=%d, in unit %d \n", out_vlan, unit);
	}

	rv = bcm_vlan_gport_add(unit, out_vlan, provider_port, 0);
	if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
		printf("fail add port(0x%08x) to vlan(%d)\n", provider_port, out_vlan);
	  return rv;
	}

    create_l3_intf_s intf;
    intf.vsi = out_vlan;
    intf.my_global_mac = mac_address;
    intf.my_lsb_mac = mac_address;
    intf.vrf_valid = 1;
    intf.vrf = 0;
    
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
    tunnel_1.vlan = out_vlan;
    tunnel_1.dscp_sel = bcmTunnelDscpAssign;
    tunnel_1.l3_intf_id = ((ll_eep_id > 0)? ll_eep_id :0) ;
    tunnel_itf1 = tunnel_gports[0];
    rv = add_ip_tunnel(unit,&tunnel_itf1,&tunnel_1);
    if (rv != BCM_E_NONE) {
        printf("Error, add_ip_tunnel 1\n");
    }
    if(verbose >= 1) {
        printf("created IP tunnel_1 on intf:0x%08x \n",tunnel_itf1);
        if (ll_eep_id >0) {
            printf("next hop mac at encap-id 0x%08x, \n", ll_eep_id);
        }

    }

    /*** create tunnel 2 ***/
    bcm_tunnel_initiator_t_init(&tunnel_2);
    tunnel_2.dip = tunnel_info_2.dip; /* default: 171.0.0.17*/
    tunnel_2.sip = tunnel_info_2.sip; /* default: 170.0.0.17*/
    tunnel_2.dscp = tunnel_info_2.dscp; /* default: 11 */
    tunnel_2.flags = 0;
    tunnel_2.ttl = tunnel_info_2.ttl; /* default: 50 */
    tunnel_2.type = ip_tunnel_glbl_info.tunnel_2_type; /* default: bcmTunnelTypeIpAnyIn4 */
    tunnel_2.vlan = out_vlan;
    tunnel_2.dscp_sel = bcmTunnelDscpAssign;
    tunnel_2.l3_intf_id = ((ll_eep_id > 0)? ll_eep_id :0) ;
    tunnel_itf2 = tunnel_gports[1];
    rv = add_ip_tunnel(unit,&tunnel_itf2,&tunnel_2);
    if (rv != BCM_E_NONE) {
        printf("Error, add_ip_tunnel 2\n");
    }
    if(verbose >= 1) {
        printf("created IP tunnel_2 on intf:0x%08x \n",tunnel_itf2);
        if (ll_eep_id >0) {
            printf("next hop mac at encap-id 0x%08x, \n", ll_eep_id);
        }
    }

    if (g_vxlan_roo.second_level_ecmp_enable) {
        /*** create tunnel 2.2 ***/
        bcm_tunnel_initiator_t_init(&tunnel_2_2);
        tunnel_2_2.dip = tunnel_info_2.dip + 1; /* default: 171.0.0.18*/
        tunnel_2_2.sip = tunnel_info_2.sip; /* default: 170.0.0.17*/
        tunnel_2_2.dscp = tunnel_info_2.dscp; /* default: 11 */
        tunnel_2_2.flags = 0;
        tunnel_2_2.ttl = tunnel_info_2.ttl; /* default: 50 */
        tunnel_2_2.type = ip_tunnel_glbl_info.tunnel_2_type; /* default: bcmTunnelTypeIpAnyIn4 */
        tunnel_2_2.vlan = out_vlan;
        tunnel_2_2.dscp_sel = bcmTunnelDscpAssign;
        tunnel_2_2.l3_intf_id = ((ll_eep_id > 0)? ll_eep_id :0) ;
        tunnel_itf2_2 = tunnel_gports[2];
        rv = add_ip_tunnel(unit,&tunnel_itf2_2,&tunnel_2_2);
        if (rv != BCM_E_NONE) {
            printf("Error, add_ip_tunnel 2.2\n");
        }
        if(verbose >= 1) {
            printf("created IP tunnel_2_2 on intf:0x%08x \n",tunnel_itf2_2);
        }
    }

    /*** using egress object API set MAC address for tunnel 1 interface, without allocating FEC enty ***/
    if (ll_eep_id < 0) {
        create_l3_egress_s l3eg;
        sal_memcpy(l3eg.next_hop_mac_addr, next_hop_mac , 6);
        l3eg.out_tunnel_or_rif = tunnel_itf1;
        l3eg.vlan = 0;
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
    } 

    /*** create egress object 2: points into tunnel 2, with allocating FEC, and da-mac = next_hop_mac2  ***/
    if (ll_eep_id < 0) {
        flags = 0;
    } else {
        flags = BCM_L3_INGRESS_ONLY;
    }

    /* create cascaded fec. Fec will be pointed by (native) routing fec.
       routing table entry is cascaded fec  */
    if (g_vxlan_roo.enable_add_entry_routing_table) {
        flags |= BCM_L3_CASCADED; 
    }
    create_l3_egress_s l3eg1;
    l3eg1.allocation_flags = flags;
    sal_memcpy(l3eg1.next_hop_mac_addr, next_hop_mac2 , 6);
    l3eg1.out_tunnel_or_rif = tunnel_itf2;
    l3eg1.out_gport = provider_port;
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
        printf("created FEC-id =0x%08x, \n", fec[1]);
        printf("next hop mac at encap-id %08x, \n", encap_id[1]);
    }

    if (g_vxlan_roo.second_level_ecmp_enable) {
        create_l3_egress_s l3eg2;
        l3eg2.allocation_flags = flags;
        sal_memcpy(l3eg2.next_hop_mac_addr, next_hop_mac2 , 6);
        l3eg2.out_tunnel_or_rif = tunnel_itf2_2;
        l3eg2.out_gport = provider_port;
        l3eg2.vlan = out_vlan;
        l3eg2.fec_id = fec[2];
        l3eg2.arp_encap_id = encap_id[2];

        rv = l3__egress__create(unit,&l3eg2);
        if (rv != BCM_E_NONE) {
            printf("Error, l3__egress__create\n");
            return rv;
        }

        fec[2] = l3eg2.fec_id;
        encap_id[2] = l3eg2.arp_encap_id;
        if(verbose >= 1) {
            printf("created FEC-id =0x%08x, \n", fec[2]);
            printf("next hop mac at encap-id %08x, \n", encap_id[2]);
        }
    }

   /* interface for tunnel_1 is IP-tunnel ID */
    intf_ids[0] = tunnel_itf1;

    /* interface for tunnel_2 is egress-object (FEC) */
    intf_ids[1] = fec[1];
    if (g_vxlan_roo.second_level_ecmp_enable) {
        intf_ids[2] = fec[2];
    }


    /* refernces to created tunnels as gport */
    tunnel_gports[0] = tunnel_1.tunnel_id;
    tunnel_gports[1] = tunnel_2.tunnel_id;
    if (g_vxlan_roo.second_level_ecmp_enable) {
        tunnel_gports[2] = tunnel_2_2.tunnel_id;
    }

    tunnel_itfs[0] = tunnel_itf1; 
    tunnel_itfs[1] = tunnel_itf2;
    if (g_vxlan_roo.second_level_ecmp_enable) {
        tunnel_itfs[2] = tunnel_itf2_2;
    }

    printf("tunnel id: %x. tunnel intf: %x\n", tunnel_2.tunnel_id, tunnel_itf2);

    return rv;
}



/* For QAX / Jer+ devices only. 
   Create a native AC-LIF.
   Configure it to add 1 native vlans
   Configure it as default AC. */ 
int vxlan_roo_native_default_out_ac(int unit) {
    int rv = BCM_E_NONE; 
   
    vlan_action_utils_s action; 
    action.o_tpid = 0x8100; 
    action.i_tpid = 0x9100; 
    action.o_action = bcmVlanActionAdd; /* outer action */
    action.i_action = bcmVlanActionNone; /* outer action */
    rv = vlan__native_default_out_ac_allocate_and_set(unit, g_vxlan_roo.native_out_vlan, g_vxlan_roo.native_inner_vlan, &action); 
    if (rv != BCM_E_NONE) {
        printf("Error, vlan__native_default_out_ac_allocate_and_set\n");
        return rv;
    }
    g_vxlan_roo.native_vlan_action_id = action.action_id; 
    return rv;
}



/* For Jericho/qmx devices only.
   Create an out ac. Used to perform native VE. */
 int vxlan_roo_out_ac_perform_native_ve(int unit, int access_port, int provider_port) {

    int rv = BCM_E_NONE; 

    bcm_gport_t vlan_port_id;

    rv = l2__port_vlan__create(unit, 0, access_port, g_vxlan_roo.native_out_vlan, g_vxlan_roo.vpn, &vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, l2__port_vlan__create, in_intf=0x%08x \n", vlan_port_id);
    }

    rv = vswitch_add_port(unit, g_vxlan_roo.vpn,provider_port, vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_add_port\n");
        return rv;
    }
 
  /* In advanced vlan translation mode, the default ingress/ egress actions and mapping
     are not configured. This is here to compensate. */
  if (advanced_vlan_translation_mode) {
       /* set egress vlan translation: double tagged */
    rv = vlan__eve_default__set(unit, vlan_port_id, g_vxlan_roo.native_out_vlan, g_vxlan_roo.native_inner_vlan, bcmVlanActionAdd, bcmVlanActionNone);
    if (rv != BCM_E_NONE) {                                                                       
        printf("Error, vlan__eve_default__set\n");              
    } 
    printf("For advance vlan editing: default out AC : %d. Add one vlan: %d \n", vlan_port_id, g_vxlan_roo.native_out_vlan); 
  }
        return rv; 
}




/* modify current configuration to build the native ethernet header with specific number of vlans:
 * - update EVE. Add "nbr_native_vlans" vlans. (up to 2 vlans) 
 * - update native vsi compensation: number of expected native vlan for the vsi. 
 *    Number of native vlan is specified in EVE. EVE is applied after UDP is built, 
 *   so UDP header size field is calculated according to native vsi compensation.
 */
int vxlan_roo_modify_native_vlan_number(int unit, int nbr_native_vlans) {
   int rv; 

   
   /* update EVE according to number of vlan to build */

   /* set action according to number of native vlans to build */
   uint32 flags = BCM_VLAN_ACTION_SET_EGRESS;

   bcm_vlan_action_t outer_action, inner_action; 
   if (nbr_native_vlans == 2) {
       outer_action = bcmVlanActionAdd; 
       inner_action = bcmVlanActionAdd; 
   } else if (nbr_native_vlans == 1) {
       outer_action = bcmVlanActionAdd; 
       inner_action = bcmVlanActionNone; 
   } else if (nbr_native_vlans == 0) {
       outer_action = bcmVlanActionNone; 
       inner_action = bcmVlanActionNone; 
   } else {
       printf("Error, invalid number of native vlans \n");
       return rv; 
   }

   /* overwrite EVE action */
   bcm_vlan_action_set_t action;
   
   bcm_vlan_action_set_t_init(&action);
   action.dt_outer = outer_action;
   action.dt_inner = inner_action;
   action.outer_tpid = 0x8100;
   action.inner_tpid = 0x9100; 

   if (is_device_or_above(unit,JERICHO_PLUS)) {
       /* for QAX, set action_id for action from native VE. */
       rv = bcm_vlan_translate_action_id_set( unit, 
                                              flags,
                                              g_vxlan_roo.native_vlan_action_id, /* use dedicated native vlan edit command */
                                              &action);
   } else {
       rv = bcm_vlan_translate_action_id_set( unit, 
                                              flags,
                                              g_eve_edit_utils.action_id /* use global action_id */,
                                              &action);
   }

   if (rv != BCM_E_NONE) {
      printf("Error, bcm_vlan_translate_action_id_set \n");
      return rv; 
   }

   /* update native vsi compensation */

   /* use replace functionality to update the native vsi compensation */
   if (is_device_or_above(unit,ARAD_PLUS) && !is_device_or_above(unit,JERICHO_PLUS)) {

       bcm_l3_intf_t l3if; 
       bcm_l3_intf_t_init(&l3if);
       int out_vlan = g_vxlan_roo.vpn; /* vpn is used as native outRif*/
       l3if.l3a_intf_id = out_vlan;
       rv = bcm_l3_intf_get(unit, &l3if);
       if (rv != BCM_E_NONE) {
            printf("Error, bcm_l3_intf_get \n");
            return rv; 
       }
       l3if.l3a_flags = l3if.l3a_flags | BCM_L3_REPLACE | BCM_L3_WITH_ID;
       l3if.native_routing_vlan_tags = nbr_native_vlans; 
       rv = bcm_l3_intf_create(unit, &l3if); 
       if (rv != BCM_E_NONE) {
            printf("Error, bcm_petra_l3_intf_create \n");
            return rv; 
       }
   }
   return rv; 
}

/*add eve action on out_port in vsi
   update outer tag vid to out_vlan
   update outer tag priority by QoS map: pcp_vlan_profile
   if it is for native link layer for JerichoPlus, QAX and above, 
   should create native AC and match it to tunnel*/
int eve_action(int unit, int out_port, int out_vlan, int vsi, int is_native)
{
    int rv;
    bcm_vlan_action_set_t action;
    bcm_gport_t vlan_port_id;
    int is_adv_vt;

    is_adv_vt = soc_property_get(unit , "bcm886xx_vlan_translate_mode",0);

    rv = bcm_port_class_set(unit, out_port, bcmPortClassId, out_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_class_set\n");
        return rv;
    }

    bcm_vlan_port_t vlan_port;

    bcm_vlan_port_t_init(&vlan_port);
    if (is_device_or_above(unit,JERICHO_PLUS) && is_native){
        vlan_port.flags = BCM_VLAN_PORT_NATIVE;
    }
    vlan_port.port = out_port;
    vlan_port.vsi = vsi;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
       printf("Error, bcm_vlan_port_create\n");
       return rv;
    }

    vlan_port_id = vlan_port.vlan_port_id;

    if (is_device_or_above(unit,JERICHO_PLUS) && is_native){
        bcm_port_match_info_t port_match_info;
        /*bind eve ac to tunnel port*/
        bcm_port_match_info_t_init(&port_match_info);

        port_match_info.flags = BCM_PORT_MATCH_NATIVE | BCM_PORT_MATCH_EGRESS_ONLY; /* indicate that it's for native VE */
        port_match_info.match = BCM_PORT_MATCH_PORT_VPN; /* indicate that lookup fields are port and VPN */
        port_match_info.vpn = vsi;
        port_match_info.port = g_vxlan_roo.ip_tunnel_initiator_port_id;

        rv = bcm_port_match_add(unit, vlan_port_id, &port_match_info);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_match_add \n ");
        }
    }

    /* set port translation info*/
    if (is_adv_vt) {
        rv = vlan_port_translation_set(unit, out_vlan, 0, vlan_port_id, g_eve_edit_utils.edit_profile, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan_port_translation_set\n");
            return rv;
        }


        /* Create action IDs*/
        rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_EGRESS, &g_eve_edit_utils.action_id);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_create\n");
            return rv;
        }

        /* Set translation action 1. outer action, set TPID 0x8100. */
        bcm_vlan_action_set_t_init(&action);
        action.dt_outer = bcmVlanActionAdd;
        action.dt_inner = bcmVlanActionNone;
        action.dt_outer_pkt_prio = bcmVlanActionAdd;
        action.outer_tpid = 0x8100;
        action.priority = pcp_vlan_profile;
        rv = bcm_vlan_translate_action_id_set(unit,
                                               BCM_VLAN_ACTION_SET_EGRESS,
                                               g_eve_edit_utils.action_id,
                                               &action);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_set\n");
            return rv;
        }

        /* set action class */
        rv = vlan_default_translate_action_class_set(unit, g_eve_edit_utils.action_id);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan_default_translate_action_class_set\n");
            return rv;
        }

        /* in advanced vlan translation mode, The priority field isn't used, The QoS mapping is associated with the Out-LIF using bcm_qos_port_map_set(). */
        rv = bcm_qos_port_map_set(unit, vlan_port_id,-1,pcp_vlan_profile);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_qos_port_map_set\n");
            return rv;
        }

    } else {
        /* Associate Out-AC to QOS profile ID, by egress vlan editing */
        bcm_vlan_action_set_t_init(&action);
        action.ut_outer = bcmVlanActionAdd;
        action.ut_outer_pkt_prio = bcmVlanActionAdd;
        action.new_outer_vlan = out_vlan;
        action.priority = pcp_vlan_profile;

        rv = bcm_vlan_translate_egress_action_add(unit, out_port, vsi, BCM_VLAN_NONE, &action);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_egress_action_add\n");
            return rv;
        }
    }

    return rv;
}        

/******* Run example ******/
 
/* 
 * This test is based on vxlan example
 */
int vxlan_roo_run(int unit, int access_port, int provider_port, int vpn_id, int add_routing_table_entry, int uc_rpf_mode){
    int rv;
    int flags = 0;
    int port_id; 
    int ports[2] = {access_port, provider_port}; 

    /*** init ***/
    for (port_id = 0; port_id < 2; port_id++) {
        port_tpid_init(ports[port_id], 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, port_tpid_set\n");
            print rv;
            return rv;
        }
    }


    /* init ip tunnel global */
    ip_tunnel_s tunnel_1;   /* tunnel 1 is not used in vxlan roo */
    ip_tunnel_s tunnel_2;   /* tunnel 2 info from cint_ip_tunnel.c */
    ip_tunnel_info_get(&tunnel_1, &tunnel_2);


    /* init ip tunnel termination global 
       skip ethernet flag is available until arad+. 
       For jericho, support 2nd my mac termination for native ethernet termination. */
    ip_tunnel_term_glbl_info.skip_ethernet_flag = (!is_device_or_above(unit,JERICHO));
    printf("skip ethernet flag: %d \n", ip_tunnel_term_glbl_info.skip_ethernet_flag); 

    /* init vxlan globals */
    vxlan_s vxlan_param; 
    vxlan_struct_get(&vxlan_param);
    vxlan_init(unit, &vxlan_param);

    g_vxlan_roo.vpn = g_vxlan.vpn_id;

    /* init vxlan roo global */
    vxlan_roo_s vxlan_roo_param; 
    vxlan_roo_struct_get(&vxlan_roo_param); 
    vxlan_roo_param.enable_add_entry_routing_table = add_routing_table_entry; 

    vxlan_roo_init(&vxlan_roo_param); 

    bcm_gport_t in_tunnel_gports[2];
    bcm_gport_t out_tunnel_gports[3];
    bcm_if_t out_tunnel_intf_ids[3];/* out tunnels interfaces
                                  out_tunnel_intf_ids[0] : is tunnel-interface-id
                                  out_tunnel_intf_ids[1] : is egress-object (FEC) points to tunnel
                                  out_tunnel_intf_ids[2] : is egress-object (FEC) points to tunnel
                              */
    bcm_if_t out_tunnel_intf_ids_only[3]; /* out tunnels interfaces only (out_tunnel_intf_ids returns FEC for 1) */
    bcm_if_t out_tunnel_ecmp_int_ids; /* ECMP tunnel interface: ECMP points to ip tunnel FEC (out_tunnel_intf_ids[1]) */

    int eg_intf_ids[2];/* interface for routing, not used */
    int vxlan_port_flags = 0;


    /* init ip tunnel info, to fit VXLAN usage */
    ip_tunnel_glbl_init_vxlan(unit,0);

    /* init L2 VXLAN module */
    rv = bcm_vxlan_init(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set \n");
    }

    /* set trap for bounce back filter to drop */
    set_trap_to_drop(unit, bcmRxTrapEgTrillBounceBack);


    /** configure my mac **/
    if (vxlan_roo_param.use_vrrp) {
        /* 
         *  Configure 00:00:5e:00:01:77 as a mymac address for all VSIs.
         *  The regular Router A mac is configured as mymac when calling bcm_l3_intf_create.
 */
        int vrid = 0x77;
        rv = bcm_l3_vrrp_config_add(unit, BCM_L3_VRRP_IPV4 | BCM_L3_VRRP_IPV6, 0, vrid);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_l3_vrrp_config_add. \n");
            return rv;
        }
    } 

    /*** build L2 VPN ***/

    rv = vxlan_open_vpn(unit,g_vxlan_roo.vpn,g_vxlan.vni);
    if (rv != BCM_E_NONE) {
       printf("Error, vxlan_open_vpn, vpn=%d, \n", g_vxlan_roo.vpn);
    }

    /* port vlan port*/
    /* init vlan port glbl info
       vlan domain x VLAN -> in-ac LIF 
       vlan domain x VSI -> out-ac LIF
       */
    vswitch_metro_mp_info_init(provider_port,0,0);


    /***  Build the native link layer (part of ROO encapsulation). ***/

  bcm_mac_t native_next_hop = {0x00, 0x00, 0x07, 0x00, 0x01, 0x23}; 

    /* create outlif using next hop
     *   Jericho note: nof bits for native outlif is limited in host_add api. (14b in arad, 15 in jericho)
     *                 so specify the outlif id to be sure it fits.
     */
    if (verbose >= 2) {
        printf("Create native LL for native routing. \n"); 
    }

    create_l3_egress_s l3_egress; 
    sal_memset(&l3_egress, 0, sizeof(l3_egress));
    l3_egress.next_hop_mac_addr = native_next_hop, /* next-hop */

    l3_egress.arp_encap_id = g_vxlan_roo.native_ll_outlif;  /* LL eedb index */ 
    /* qax note: BCM_L3_NATIVE_ENCAP is for QAX and above devices:
       indicate we build a native LL. (eedb entry is allocated in a native LL eedb phase)  */
    if (is_device_or_above(unit,JERICHO_PLUS)) {
        l3_egress.l3_flags = BCM_L3_NATIVE_ENCAP;
    }

    rv = l3__egress_only_encap__create(unit, &l3_egress); 
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress_only_encap__create\n");
    }

    g_vxlan_roo.native_ll_outlif = l3_egress.arp_encap_id;/* LL eedb index */

    if (verbose >= 2) {
        printf("successfully created native LL: 0x%x, and  \n", g_vxlan_roo.native_ll_outlif);
    }



    /*** build LL for overlay/tunnel ***/

    /* In ROO, until Jericho, the overlay LL encapsulation is built with a different API call 
     * (bcm_l2_egress_create instead of bcm_l3_egress create)
     * Jericho notes: 
     *  - In Jericho, api l2_egress_create configures only 12 lsbs for overlay LL encapsulation, 
     *    unlike in arad+ version, where all the 48bits are saved in the LL encapsulation
     *  - In Jericho, we support only 8 msbs of overlay outer vlan: 4 lsbs of overlay outer vlan
     *    and overlay inner vlan will be supported when eedb extension (1/4 EEDB entry)
     * 36 MSBs are  global configuration.
     *  
     * For QAX devices, use bcm_l3_egress API. 
     * QAX note: 
     * - we provide roo__overlay_link_layer_create and roo__overlay_eve to help to migrate from Jericho to QAX 
 */
    bcm_l2_egress_t l2_egress_overlay;
    bcm_l2_egress_t_init(&l2_egress_overlay);
     
    l2_egress_overlay.dest_mac   = tunnel_2.da; /* next hop. default: {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d} */
    l2_egress_overlay.src_mac    = tunnel_2.sa; /* my-mac. default:  {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}  */
    l2_egress_overlay.outer_vlan = g_vxlan_roo.overlay_out_vlan; /* PCP DEI (0) + outer_vlan (100=0x64)  */
    l2_egress_overlay.ethertype  = 0x800;       /* ethertype for IP */
    l2_egress_overlay.outer_tpid = 0x8100;      /* outer tpid */
    l2_egress_overlay.vlan_qos_map_id = pcp_vlan_profile;
    if (l2_egress_overlay.vlan_qos_map_id) {
        ip_tunnel_term_glbl_info.tunnel_term_2_flags=1;
    }  


    if (vxlan_roo_param.add_double_vlan_overlay) {
        l2_egress_overlay.inner_vlan = g_vxlan_roo.overlay_in_vlan;   
        l2_egress_overlay.outer_tpid = 0x9100;      /* outer tpid */
        l2_egress_overlay.inner_tpid = 0x8100;   
    } 
    
    if (g_vxlan_roo.overlay_ll_outlif != 0) {   /* outlif (or overlay LL eedb index) */
        l2_egress_overlay.encap_id = g_vxlan_roo.overlay_ll_outlif ;     
        /* indicate outlif is provided */
        l2_egress_overlay.flags    = BCM_L2_EGRESS_WITH_ID;  
    }

    rv = roo__overlay_link_layer_create(unit, &l2_egress_overlay,
                                        0, /* l3 flags */
                                        &(g_vxlan_roo.overlay_ll_outlif),
                                        0);
    if (rv != BCM_E_NONE) {
        printf("Error, roo__overlay_link_layer_create \n");
        return rv; 
    } 

    if(verbose >= 2){
        printf("overlay LL encapsulation, outlif: 0x%x: \n", g_vxlan_roo.overlay_ll_outlif);
    }

    /* QAX: add 2 vlans require eve (in case we're in qax) */
    /* TEMP, until advance Vlan translation works ! */
/*  if (vxlan_roo_param.add_double_vlan_overlay) {
      rv = roo__overlay_eve(unit,
                            &l2_egress_overlay,
                            provider_port,
                            g_vxlan_roo.overlay_out_vlan);
      if (rv != BCM_E_NONE) {
         printf("Error, roo__overlay_eve \n");
         return rv;
       }
   } */

    if(verbose >= 2){
        printf("Open tunnels: \n\r");
    }


    /*** build tunnel initiators ***/
    /*note: constraint: we have to provide the overlay LL in order to configure the tunnel accordingly. 
      Otherwise both ip tunnel and overlay LL are in the same encapsulation phase */
    rv = vxlan_roo_ipv4_tunnel_build_tunnels(unit, 
                                             provider_port, 
                                             g_vxlan_roo.overlay_ll_outlif, 
                                             g_vxlan_roo.overlay_out_vlan, 
                                             out_tunnel_intf_ids,
                                             out_tunnel_gports, 
                                             out_tunnel_intf_ids_only);
    if (rv != BCM_E_NONE) {
        printf("Error, ipv4_tunnel_add_routes, access_port=%d, \n", access_port);
    }
    if(verbose >= 2){
        printf("Open out-tunnel 1 gport-id: 0x%08x (No FEC)\n\r", out_tunnel_gports[0]);
        printf("Open out-tunnel 2 gport-id: 0x%08x FEC-id:0x%08x\n\r", out_tunnel_gports[1],out_tunnel_intf_ids[1]);
        if (g_vxlan_roo.second_level_ecmp_enable) {
            printf("Open out-tunnel 2.2 gport-id: 0x%08x FEC-id:0x%08x\n\r", out_tunnel_gports[2],out_tunnel_intf_ids[2]);
        }
    }

    /* set global var: ip tunnel initiator gport. */
    g_vxlan_roo.ip_tunnel_initiator_port_id = out_tunnel_gports[1]; 


    /* create ecmp: ECMP points to ip tunnel FEC 
     * Jericho note: 
     * host entry can hold up to 12b of FEC id (4k ids)                                                    
     * In Jericho, ECMP is also 4k.                                                                        
     * Consequently, to add a host entry in jericho, we'll have to add a ECMP entry instead of the FEC. */  
    bcm_l3_egress_ecmp_t ecmp; 
    int ecmp_nof_paths; 
    bcm_l3_egress_ecmp_t_init(&ecmp);

    if (g_vxlan_roo.second_level_ecmp_enable) {
        ecmp_nof_paths = 2; 
        ecmp.max_paths = ecmp_nof_paths;
		if (g_vxlan_roo.use_slb_hashing_for_ecmp) {
			ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
		}
		rv = bcm_l3_egress_ecmp_create(unit, &ecmp, 
                                       ecmp_nof_paths, /* nof paths */
                                       &out_tunnel_intf_ids[1] /* interface object of type FEC */
                                       ); 
    }
    else {
        ecmp_nof_paths = 1; 
        ecmp.max_paths = ecmp_nof_paths;
        rv = bcm_l3_egress_ecmp_create(unit, &ecmp, 
                                       ecmp_nof_paths, /* nof paths */
                                       out_tunnel_intf_ids[1] /* interface object of type FEC */
                                       ); 
    }

    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_egress_ecmp_create \n");
        return rv; 
    }
    out_tunnel_ecmp_int_ids = ecmp.ecmp_intf; 
    if(verbose >= 1){
        printf("Created ecmp for ip tunnel FEC, ECMP-id: 0x%x for FEC-id: 0x%x \n", out_tunnel_ecmp_int_ids, out_tunnel_intf_ids[1]);
    }

    if(verbose >= 2){
        printf("Open tunnels tunnel terminators: \n\r");
    }
    /*** build tunnel terminations and router interfaces ***/
    rv = ipv4_tunnel_term_build_tunnel_terms(unit,access_port,provider_port,eg_intf_ids,in_tunnel_gports); 
    if (rv != BCM_E_NONE) {
        printf("Error, ipv4_tunnel_term_build_tunnel_terms, access_port=%d, \n", access_port);
    }
    if(verbose >= 2){
        printf("Open in-tunnel 1 gport-id: 0x%08x (No FEC)\n\r", in_tunnel_gports[0]);
        printf("Open in-tunnel 2 gport-id: 0x%08x FEC-id\n\r", in_tunnel_gports[1]);
    }

    /*** access routing: routing at access side ***/
    bcm_mac_t access_my_mac  = tunnel_2.sa; /* my-MAC: 00:0c:00:02:00:00 */


    create_l3_intf_s access_l3_intf;
    if (uc_rpf_mode != 0) {
        flags |= BCM_L3_RPF;
        access_l3_intf.rpf_valid = TRUE;
        int units[1] = {unit};
        rv = l3_ip_rpf_config_traps(units /*units_ids*/, 1/*nof_units*/);
        if (rv != BCM_E_NONE) {
            printf("Error, l3_ip_rpf_config_traps \n");
        }
        rv =  bcm_switch_control_set(unit, bcmSwitchL3UrpfMode, uc_rpf_mode);
        if (rv != BCM_E_NONE) {
          return rv;
        }
    } else {
        access_l3_intf.rpf_valid = FALSE;
    }
    access_l3_intf.flags = flags;
    access_l3_intf.urpf_mode = uc_rpf_mode;
    access_l3_intf.vrf = g_vxlan_roo.vrf;/* router instance */
    access_l3_intf.vrf_valid = TRUE;
    access_l3_intf.vsi = g_vxlan_roo.access_vsi; /* access VSI */
   
    access_l3_intf.my_global_mac = access_my_mac; /* my mac at access side */
    access_l3_intf.my_lsb_mac = access_my_mac;    /* my mac at access side */
    access_l3_intf.ingress_flags = BCM_L3_INGRESS_DSCP_TRUST;
    access_l3_intf.qos_map_valid = 1;
    access_l3_intf.qos_map_id = pcp_vlan_profile;

    rv = l3__intf_rif__create(unit, &access_l3_intf);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }    
    
    /***  native routing ***/
    
    /* create L3 intf: myMac, VLAN for native routing */
    bcm_mac_t native_my_mac  = tunnel_2.sa; /* my-MAC: 00:0c:00:02:00:00 */

    int native_ing_intf; 

    create_l3_intf_s l3_intf;
    if (uc_rpf_mode != 0) {
        flags |= BCM_L3_RPF;
        l3_intf.rpf_valid = TRUE;
        int units[1] = {unit};
        rv = l3_ip_rpf_config_traps(units /*units_ids*/, 1/*nof_units*/);
        if (rv != BCM_E_NONE) {
            printf("Error, l3_ip_rpf_config_traps \n");
        }
        rv =  bcm_switch_control_set(unit, bcmSwitchL3UrpfMode, uc_rpf_mode);
        if (rv != BCM_E_NONE) {
          return rv;
        }
    } else {
        l3_intf.rpf_valid = FALSE;
    }
    l3_intf.flags = flags;
    l3_intf.urpf_mode = uc_rpf_mode;
    l3_intf.vrf = g_vxlan_roo.vrf;/* router instance */
    l3_intf.vrf_valid = TRUE;
    l3_intf.vsi = g_vxlan_roo.vpn; /* vlan: Use vxlan vpn */
    /* native ethernet compensation is supported until Jericho.
       Helps to to calculate the UDP.length by indicating how many vlans are expected
       Not needed in QAX, since native LL has already been built in native LL block */
    if (!is_device_or_above(unit,JERICHO_PLUS)) {
        l3_intf.native_routing_vlan_tags = 1; /* native ethernet compensation */
    }
    l3_intf.my_global_mac = native_my_mac; /* native my mac */
    l3_intf.my_lsb_mac = native_my_mac; /* native my mac */
    l3_intf.ingress_flags = BCM_L3_INGRESS_DSCP_TRUST;
    l3_intf.qos_map_valid = 1;
    l3_intf.qos_map_id = pcp_vlan_profile;

    rv = l3__intf_rif__create(unit, &l3_intf);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }

    native_ing_intf = l3_intf.rif; 

  /* add host entry: add entry to host table to access network */


    /* native router interface: 
       create LL for native: next hop, VLAN for native routing */
  int simple_routing_fec; 
  int simple_routing_id;
  int simple_encap_id = 0;
  bcm_mac_t simple_routing_next_hop = {0x00, 0x00, 0x00, 0x00, 0x05, 0x81}; 
    create_l3_egress_s l3eg;
    /* l3_egress: FEC configuration */
    l3eg.out_tunnel_or_rif = access_l3_intf.rif; /* outRIF */
    l3eg.out_gport = access_port;
    l3eg.fec_id = simple_routing_fec;
    /* l3_egress: ARP configuration */
    sal_memcpy(l3eg.next_hop_mac_addr, simple_routing_next_hop , 6);
    l3eg.vlan = g_vxlan_roo.access_vsi;
    l3eg.arp_encap_id = simple_encap_id;

    rv = l3__egress__create(unit,&l3eg);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
        return rv;
    }

    simple_routing_fec = l3eg.fec_id;
    simple_encap_id = l3eg.arp_encap_id;

  /*rpf related settings*/
  if (uc_rpf_mode != 0) {

    /*create dummy fec for a failed strict rpf check
    note: rif doesn't exist */
    int rpf_routing_fec;
    int rpf_encap_id = 0;
    create_l3_egress_s l3eg2;
    sal_memcpy(l3eg2.next_hop_mac_addr, simple_routing_next_hop , 6);
    l3eg2.out_tunnel_or_rif = native_ing_intf+1;
    l3eg2.out_gport = access_port;
    l3eg2.vlan = g_vxlan_roo.vpn;
    l3eg2.fec_id = rpf_routing_fec;
    l3eg2.arp_encap_id = rpf_encap_id;

    rv = l3__egress__create(unit,&l3eg2);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
        return rv;
    }

    rpf_routing_fec = l3eg2.fec_id;
    rpf_encap_id = l3eg2.arp_encap_id;


      /* add simple routing host entry */
      bcm_l3_host_t l3_host_simple_routing;
      bcm_l3_host_t_init(l3_host_simple_routing);
      /* key of host entry */
      l3_host_simple_routing.l3a_vrf = g_vxlan_roo.vrf;/* router instance, different inrif for the test */
      l3_host_simple_routing.l3a_ip_addr = 0xACACACAC; /* ip host entry */
      /* data of host entry */
      l3_host_simple_routing.l3a_intf  = rpf_routing_fec; /*dummy fec just for rpf*/
      rv = bcm_l3_host_add(unit, &l3_host_simple_routing);
      if (rv != BCM_E_NONE) {
          printf("Error, bcm_l3_host_add\n");
          return rv;
      }

      if (add_routing_table_entry != 0) {
          /*Additional route for KAPS rpf test*/
          int route1 =  0xACACAC05; /* ip host entry */
          int mask1  = 0xffffff00;
          add_route(unit,
                    route1,               /* key of LPM entry: ip               */
                    mask1,                /*                   mask for route   */
                    g_vxlan_roo.vrf,     /*                   router instance */
                    rpf_routing_fec   /* simple fec of the access network */
          );
      }
  }

  vxlan_port_add_s vxlan_port_add; 
  vxlan_port_s_clear(&vxlan_port_add); 
  vxlan_port_add.vpn = g_vxlan_roo.vpn; 
  vxlan_port_add.in_port = access_port; 
  vxlan_port_add.in_tunnel = in_tunnel_gports[1]; 
  vxlan_port_add.out_tunnel = out_tunnel_gports[1]; 
  vxlan_port_add.egress_if = out_tunnel_ecmp_int_ids; 
  vxlan_port_add.flags = vxlan_port_flags; 
  vxlan_port_add.network_group_id = g_vxlan.vxlan_network_group_id; 
  /* FEC passed to vxlan port is an ECMP. Therefore, we can't add directly the vxlan port to the VSI flooding MC group
     We'll add instead the tunnel initiator gport to the VSI flooding MC group */
  vxlan_port_add.is_ecmp = 1;    /* this vxlan port uses ECMP */
  vxlan_port_add.out_tunnel_if = out_tunnel_intf_ids_only[1];  /* ip tunnel initiator itf */

  rv = vxlan__vxlan_port_add(unit, vxlan_port_add); 
  if (rv != BCM_E_NONE) {
      printf("Error, vxlan__vxlan_port_add 2, in_gport=0x%08x --> out_intf=0x%08x \n", in_tunnel_gports[1],out_tunnel_intf_ids[1]);
      return rv; 
  }

  g_vxlan_roo.vxlan_port_id = vxlan_port_add.vxlan_port_id; 


  /* create an additional vxlan port:
     configure the IP tunnel to be a l2 lif */
  vxlan_port_s_clear(&vxlan_port_add); 
  vxlan_port_add.vpn = g_vxlan_roo.vpn; 
  vxlan_port_add.in_port = access_port; 
  vxlan_port_add.in_tunnel = in_tunnel_gports[0]; 
  vxlan_port_add.out_tunnel = out_tunnel_gports[2]; 
  vxlan_port_add.egress_if = out_tunnel_ecmp_int_ids;
  vxlan_port_add.flags = vxlan_port_flags; 
  vxlan_port_add.network_group_id = g_vxlan.vxlan_network_group_id; 
  /* FEC passed to vxlan port is an ECMP. Therefore, we can't add directly the vxlan port to the VSI flooding MC group
     We'll add instead the tunnel initiator gport to the VSI flooding MC group */
  vxlan_port_add.is_ecmp = 1;    /* this vxlan port uses ECMP */
  vxlan_port_add.out_tunnel_if = out_tunnel_intf_ids_only[2];  /* ip tunnel initiator itf */

  rv = vxlan__vxlan_port_add(unit, vxlan_port_add); 
  if (rv != BCM_E_NONE) {
      printf("Error, vxlan__vxlan_port_add 2, in_gport=0x%08x --> out_intf=0x%08x \n", in_tunnel_gports[0],out_tunnel_intf_ids[2]);
      return rv; 
  }
  g_vxlan_roo.vxlan_port_id = vxlan_port_add.vxlan_port_id; 

  /* Routing table entry is cascaded fec: 
   * create additional fec for native routing, 
   * which point to cascaded fec:
   * - create outLif using next hop 
   * - FEC contains another FEC: to build overlay  
   * -              outLif and outRif: to build native LL.
   */
  int native_hi_fec_fec_id; 
  bcm_if_t native_fec[2] = {0x0,0x0};
  bcm_if_t native_ecmp_int_id;

  if (g_vxlan_roo.enable_add_entry_routing_table) {
      /* Hierarchical FEC does not work when the 1st FEC hierarchy is unprotected.
       * In case we are using hierarchical FEC, the first FEC must be protected.
       * This was fixed in Jericho B0.
       */
      if (verbose >= 2) {
          printf("Create FEC point to cascaded FEC \n");
          printf("cascaded FEC: 0x%08x \n", g_vxlan_roo.vxlan_port_id);
      }
      bcm_failover_t failover_id_fec;

      if (!is_device_or_above(unit,JERICHO_B0)) {
          /* create failover id for VxLAN */
          rv = bcm_failover_create(unit, BCM_FAILOVER_FEC, &failover_id_fec);
          if (rv != BCM_E_NONE) {
              printf("Error, bcm_failover_create 1\n");
              return rv;
          }
          if (verbose >= 2) {
              printf("Failover id: 0x%x\n", failover_id_fec);
          }
      }


      bcm_mac_t native_next_hop_kaps = {0x00, 0x00, 0x00, 0x00, 0x17, 0x12};
      create_l3_egress_s create_l3_egress_params;
 
      /* qax note: BCM_L3_NATIVE_ENCAP is for QAX and above devices:
         indicate we build a native LL. (eedb entry is allocated in a native LL eedb phase)  */
      if (is_device_or_above(unit,JERICHO_PLUS)) {
          create_l3_egress_params.l3_flags = BCM_L3_NATIVE_ENCAP;  /* flags  */
      }

      create_l3_egress_params.out_gport = g_vxlan_roo.vxlan_port_id; /* dest: fec-id instead of port */
      create_l3_egress_params.out_tunnel_or_rif = native_ing_intf; /* l3_intf  */
      create_l3_egress_params.next_hop_mac_addr = native_next_hop_kaps; /* next-hop */
      create_l3_egress_params.failover_id = failover_id_fec;
      create_l3_egress_params.failover_if_id = 0;

      if (!is_device_or_above(unit,JERICHO_B0)) {
          /* Create protected FEC */
          rv = l3__egress__create(unit, &create_l3_egress_params);
          if (rv != BCM_E_NONE) {
              printf("Error, l3__egress__create\n");
            return rv;
          }
          create_l3_egress_params.failover_if_id = create_l3_egress_params.fec_id;
          create_l3_egress_params.arp_encap_id = 0;
      }

      /* primary FEC */
      rv = l3__egress__create(unit, &create_l3_egress_params);
      if (rv != BCM_E_NONE) {
          printf("Error, l3__egress__create\n");
        return rv;
      }

      native_hi_fec_fec_id = create_l3_egress_params.fec_id;
  }

 
  
  
  if (g_vxlan_roo.first_level_ecmp_enable) {

      /* Hierarchical FEC does not work when the 1st FEC hierarchy is unprotected.
       * In case we are using hierarchical FEC, the first FEC must be protected.
       * This was fixed in Jericho B0.
       */
      if (verbose >= 2) {
          printf("Create FEC point to cascaded FEC \n");
          printf("cascaded FEC: 0x%08x \n", g_vxlan_roo.vxlan_port_id);
      }
      bcm_failover_t failover_id_fec;

      if (!is_device_or_above(unit,JERICHO_B0)) {
          /* create failover id for VxLAN */
          rv = bcm_failover_create(unit, BCM_FAILOVER_FEC, &failover_id_fec);
          if (rv != BCM_E_NONE) {
              printf("Error, bcm_failover_create 2\n");
              return rv;
          }
          if (verbose >= 2) {
              printf("Failover id: 0x%x\n", failover_id_fec);
          }
      }

      bcm_mac_t native_next_hop_kaps = {0x00, 0x00, 0x00, 0x00, 0x17, 0x13};
      create_l3_egress_s create_l3_egress_params;

       /* qax note: BCM_L3_NATIVE_ENCAP is for QAX and above devices:
         indicate we build a native LL. (eedb entry is allocated in a native LL eedb phase)  */
      if (is_device_or_above(unit,JERICHO_PLUS)) {
          create_l3_egress_params.l3_flags = BCM_L3_NATIVE_ENCAP;  /* flags  */
      }

      create_l3_egress_params.out_gport = g_vxlan_roo.vxlan_port_id; /* dest: fec-id instead of port */
      create_l3_egress_params.out_tunnel_or_rif = native_ing_intf; /* l3_intf  */
      create_l3_egress_params.next_hop_mac_addr = native_next_hop_kaps; /* next-hop */
      create_l3_egress_params.failover_id = failover_id_fec;
      create_l3_egress_params.failover_if_id = 0;

      if (!is_device_or_above(unit,JERICHO_B0)) {
          /* Create protected FEC */
          rv = l3__egress__create(unit, &create_l3_egress_params);
          if (rv != BCM_E_NONE) {
              printf("Error, l3__egress__create\n");
            return rv;
          }
          create_l3_egress_params.failover_if_id = create_l3_egress_params.fec_id;
          create_l3_egress_params.arp_encap_id = 0;
      }

      /* primary FEC */
      rv = l3__egress__create(unit, &create_l3_egress_params);
      if (rv != BCM_E_NONE) {
          printf("Error, l3__egress__create\n");
        return rv;
      }

      native_fec[0] = native_hi_fec_fec_id;
      native_fec[1] = create_l3_egress_params.fec_id;


      bcm_l3_egress_ecmp_t_init(&ecmp);
      ecmp_nof_paths = 2; 
      ecmp.max_paths = ecmp_nof_paths;
	  if (g_vxlan_roo.use_slb_hashing_for_ecmp) {
		  ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
	  }
      rv = bcm_l3_egress_ecmp_create(unit, &ecmp, 
                                     ecmp_nof_paths, /* nof paths */
                                     native_fec /* interface object of type FEC */
                                     ); 

      native_ecmp_int_id = ecmp.ecmp_intf;
  }


  printf("vrf:   %x \n", g_vxlan_roo.vrf);
  printf("l3_host.l3a_port_tgid: tunnel gport %x \n", g_vxlan_roo.vxlan_port_id);
  printf("l3_host.l3a_intf: native out rif    %x \n", native_ing_intf);
  printf("l3_host.encap_id: native arp pointer: eei entry %x \n", g_vxlan_roo.native_ll_outlif);

  /* add native host entry: add entry through vxlan overlay in host table */
  bcm_l3_host_t l3_host;      
  bcm_l3_host_t_init(l3_host);  
  /* key of host entry */
  l3_host.l3a_vrf = g_vxlan_roo.vrf;              /* router instance */ 
  if (g_vxlan_roo.use_ipv6) {
      sal_memcpy(l3_host.l3a_ip6_addr, g_vxlan_roo.native_dip6, sizeof(g_vxlan_roo.native_dip6));
  } else {
      l3_host.l3a_ip_addr = g_vxlan_roo.native_dip;  /* ip host entry */
  }

  /* data of host entry */
  /* FEC to overlay tunnel, native outrif, arp pointer are saved in host table */
  l3_host.l3a_port_tgid = g_vxlan_roo.vxlan_port_id; /* overlay tunnel: vxlan gport */
  l3_host.l3a_intf = native_ing_intf;       /* native out rif */
  l3_host.encap_id = g_vxlan_roo.native_ll_outlif; /* native arp pointer: encap/eei entry */

  rv = bcm_l3_host_add(unit, &l3_host);
  if (rv != BCM_E_NONE) {
      printf("Error, bcm_l3_host_add\n");
      return rv;
  }

  /* add LPM entry: add entry through vxlan overlay in host table using cascaded fec. 
     For cascaded fec, only need to provide HI-FEC: 
     hi fec contains native out rif, native arp pointer and point to overlay fec */ 
  if (g_vxlan_roo.enable_add_entry_routing_table) {
      l3_ipv4_route_entry_utils_s route_entry;
      sal_memset(&route_entry, 0, sizeof(route_entry));

      if (g_vxlan_roo.use_ipv6) {
          bcm_ip6_t mask6 = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
          route_entry.use_ipv6 = 1;
          sal_memcpy(route_entry.mask6, mask6, sizeof(mask6));
          sal_memcpy(route_entry.address6, g_vxlan_roo.native_dip6, sizeof(g_vxlan_roo.native_dip6));
      } else {
          route_entry.address =  g_vxlan_roo.native_dip;
          route_entry.mask = 0xfffffff0;
      }
      if (g_vxlan_roo.first_level_ecmp_enable) {
          rv = l3__ipv4_route__add(unit, 
                              route_entry,
                              g_vxlan_roo.vrf,     /* router instance */
                              native_ecmp_int_id /* data of LPM entry: cascaded fec in ECMP group */        
          );
      }
      else {
          rv = l3__ipv4_route__add(unit, 
                              route_entry,
                              g_vxlan_roo.vrf,     /* router instance */
                              native_hi_fec_fec_id /* data of LPM entry: cascaded fec */        
          ); 
      }
      if (rv != BCM_E_NONE) {
          printf("Error, in l3__ipv4_route__add\n");
          return rv;
      }
  }

  /* Add simple routing host entry to access network. If we're using IPv6, it should be added to the LPM.
     Otherwise, it can be added to the host table. */
  if (g_vxlan_roo.use_ipv6) {
      l3_ipv4_route_entry_utils_s simple_route_entry;
      bcm_ip6_t mask6 = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
      bcm_ip6_t dst6 =  {0x01, 0x00, 0x16, 0x00, 0x35, 0x00, 0x70, 0x00, 0x00, 0x00, 0xdb, 0x07, 0x00, 0x00, 0x00, 0x00};
      sal_memset(&simple_route_entry, 0, sizeof(simple_route_entry));

      
      simple_route_entry.use_ipv6 = 1;
      sal_memcpy(simple_route_entry.mask6, mask6, sizeof(mask6));
      sal_memcpy(simple_route_entry.address6, dst6, sizeof(g_vxlan_roo.native_dip6));
      rv = l3__ipv4_route__add(unit, 
                          simple_route_entry,
                          g_vxlan_roo.vrf,     /* router instance */
                          simple_routing_fec   
      ); 
      if (rv != BCM_E_NONE) {
          printf("Error, in l3__ipv4_route__add\n");
          return rv;
      }

  } else {
      bcm_l3_host_t l3_host_simple_routing; 
      bcm_l3_host_t_init(l3_host_simple_routing);  
      /* key of host entry */
      l3_host_simple_routing.l3a_vrf = g_vxlan_roo.vrf;/* router instance */ 
      l3_host_simple_routing.l3a_ip_addr = 0xA0A1A1A1; /* ip host entry */
      /* data of host entry */
      l3_host_simple_routing.l3a_intf  = simple_routing_fec;
      rv = bcm_l3_host_add(unit, &l3_host_simple_routing);
      if (rv != BCM_E_NONE) {
          printf("Error, bcm_l3_host_add\n");
          return rv;
      }
  }

  /* add bridge entry to provider network */
  bcm_mac_t mact_entry = {0x00, 0x00, 0x00, 0x02, 0x02, 0x02};
  rv = l2_addr_add(unit,mact_entry,g_vxlan_roo.vpn,g_vxlan_roo.vxlan_port_id);
  if (rv != BCM_E_NONE) {
      printf("Error, l2_addr_add to vxlan_port_id[0]\n");
      return rv;
  }

  /* add bridge entry to the access network (in case of core network to access network) */
  bcm_mac_t mact_entry_2 = {0x00, 0x0C, 0x00, 0x02, 0x00, 0x00};
  rv = l2_addr_add(unit,mact_entry_2,g_vxlan_roo.vpn,access_port);
  if (rv != BCM_E_NONE) {
      printf("Error, l2_addr_add to provider_port\n");
      return rv;
  }




 

  return rv;
}
