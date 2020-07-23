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
 *  
 *  
 *          
 *                       _________    _________    
 *           host2------| ROUTER E|  | ROUTER B|------host1              
 *                      |_________|  |_________|  
 * access_vlan_2, access_port_2 |     |access_vlan_1, access_port_1
 *                              |     |     
 *                      host3-| |     |                                            
 * access_vlan_1,access_port2 | |     | 
 *       _____________________|_|_____|______________________________
 *      |                   |         |                             |
 *      |                   | ROUTER A| (intra DC IP)    DC FABRIC  |
 *      |   provider_vlan,  |_________|                             |
 *      |       provider_port2/       \provider_port1, provider_vlan|  
 *      | native_vlan1, vni1 /        \ native_vlan1, vni1          |
 *      |            _______/___       \__________                  |
 *      |           |          |      |          |                  |
 *      |           | ROUTER C |      | ROUTER D |                  |
 *      |           |__________|      |__________|                  | 
 *      |            /                          \                   | 
 *      |         /                               \                 | 
 *      |_____ /____________________________________\_______________|
 *       ____/___                                _____\__
 *      |  TOR1 |                               |  TOR2 |                      
 *      |_______|                               |_______|                      
 *      |       |                               |       | 
 *      |       |                               |       | 
 *      |_______|                               |_______| 
 *      | VM:A0 |                               |       | 
 *      |_______|                               |_______| 
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
 * bcm886xx_auxiliary_table_mode=1 (for arad/+ devices only)                                       
 * #enable ROO for vxlan                                                  
 * bcm886xx_roo_enable=1                                                  
 *                                                                        
 * cint;                                                                  
 * cint_reset();                                                          
 * exit;  
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c  
 * cint ../../../../src/examples/dpp/utility/cint_utils_l2.c
 * cint ../../../../src/examples/dpp/utility/cint_utils_l3.c  
 * cint ../../../../src/examples/dpp/utility/cint_utils_multicast.c  
 * cint ../../../../src/examples/dpp/utility/cint_utils_vlan.c       
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
 * cint ../../../../src/examples/dpp/cint_vxlan_roo_mc.c
 * cint ../../../../src/examples/dpp/cint_field_gre_learn_data_rebuild.c  
 * int unit=0;  
 * int access_port1 = 200;  
 * int access_port2 = 201;  
 * int provider_port1 = 202;  
 * int provider_port2 = 203;  
 * int add_routing_table_entry = 0; 
 * vxlan_roo_mc_run(unit, access_port1, access_port2, provider_port1, provider_port2);                            
 * 
 * 
 *  
 * MC Traffic from access routerB to access router: routerE, 
 *                                to overlay network: routerC, routerD 
 *                                to host3
 *  Note: for now we only send to overlay network routerD
 *  
 * Purpose: - check compatible MC at L3 to overlay and to native router 
 *          - check fall to bridge 
 *          
 *    Send:                                           
 *             ---------------------------------------------
 *        eth: |    DA       |     SA      |      VLAN      |
 *             ---------------------------------------------
 *             | provider_MC | routerB_mac |  access_vlan_1 |  
 *             ---------------------------------------------
 * 
 *                 --------------------------- 
 *             ip: |   SIP     | DIP          | 
 *                 ----------------------------
 *                 | host10_ip |  provider_MC | 
 *                 ---------------------------- 
 *  
 * Receive:  to access router: router E
 * 
 *             --------------------------------------------- 
 *        eth: |    DA       |     SA      |      VLAN      |
 *             --------------------------------------------- 
 *             | provider_MC | routerA_mac |  access_vlan_2 | 
 *             --------------------------------------------- 
 * 
 *                 --------------------------- 
 *             ip: |   SIP     | DIP          | 
 *                 ----------------------------
 *                 | host10_ip |  provider_MC | 
 *                 ---------------------------- 
 *  
 * Receive:  to host3 (fall to bridge)
 *     
 *             --------------------------------------------    
 *        eth: |    DA       |     SA      |     VLAN      | 
 *             --------------------------------------------  
 *             | provider_MC | routerB_mac | access_vlan_1 |  
 *             --------------------------------------------  
 * 
 *                 ---------------------------- 
 *             ip: |   SIP     | DIP          | 
 *                 ----------------------------
 *                 | host10_ip |  provider_MC | 
 *                 ---------------------------- 
 *  
 *  
 * Receive:  to router C, router D (overlay)  
 *             --------------------------------------------  
 *        eth: |    DA      |     SA      |      VLAN     |
 *             --------------------------------------------
 *             | provider_MC| routerA_mac | provider_vlan |  
 *             --------------------------------------------
 * 
 *                 --------------------------- 
 *             ip: |   SIP     | DIP         | 
 *                 --------------------------- 
 *                 | routerA_ip| provider_MC | 
 *                 --------------------------- 
 * 
 *                udp:
 * 
 *                   vxlan:  ---------
 *                           |  VNI  |
 *                           ---------
 *                           |  vni  |
 *                           ---------
 *    
 *                                 ------------------------------------- 
 *                     native eth: |    DA       |     SA      |   VLAN | 
 *                                 ------------------------------------- 
 *                                 | provider_MC | routerA_mac |   vpn  | 
 *                                 ------------------------------------- 
 *                                                                      
 *                                     ---------------------------         
 *                         native ip:  |   SIP     | DIP          |        
 *                                     ----------------------------       
 *                                     | host10_ip |  native_MC   |        
 *                                     ----------------------------   
 *  
 * Packet flow  :     
 * - get the VSI from port x vlan 
 * - DA is identified as compatible MC, ethernet header is terminated
 * - get the RIF (= vsi)
 * - RIF is compatible MC 
 * - packet is forwarded according to DIP and VRF (inRif.vrf)
 * - result is MC 
 * - each MC member contains: outLif, outRif and port. 
 * - for MC entry to routerE (routing): LL outLif, outRif and port. 
 *   - DA is built from DIP (compatible MC)
 *   - SA = outRif.myMac
 *   - VLAN = VSI (=outRif)
 * - for MC entry to host3 (fall to bridge): LL outLif, outRif and port. 
 *   - inRif == outRif so do fall to bridge
 *   - packet is bridged
 * - for MC entry to router C and D (overlay): vxlan outLif, outRif and port
 *   - vxlan tunnel is built using IP tunnel eedb entry.     
 *   - vxlan header.vni is resovled from outVsi (outRif) -> vni
 *   - native is built from DIP (compatible MC)
 *   - native SA = outRif.myMac
 *   - VLAN = VSI (=outRif) 
 * 
 * 
 * 
 * 
 * MC traffic from overlay router C to access router B,
 *                                     access router E
 *                                     and provider router D (will be filtered)
 *  Purpose: - check tunnel is terminated, 
 *           - check native MC 
 *           - check fall to bridge and split horizon filter (for copy to provider router C)
 *  Note: this test is separated into 2 tests: 1 MC group to terminate the tunnel and send to access router B and access router E
 *                                             1 MC group to terminate the tunnel, fall back to bridge and split horizon filter
 * 
 * 
 * Send: 
 *             -----------------------------------   
 *        eth: |    DA      |     SA      | VLAN |
 *             -----------------------------------
 *             | provider_MC| routerC_mac |  v2  |  
 *             -----------------------------------
 * 
 *                 ------------------------ 
 *             ip: |   SIP  | DIP         | 
 *                 ------------------------ 
 *                 | tor1_ip| provider_MC | 
 *                 ------------------------ 
 * 
 *                udp:
 * 
 *                   vxlan:  ---------
 *                           |  VNI  |
 *                           ---------
 *                           |  vni  |
 *                           ---------
 *    
 *                                 ------------------------------------- 
 *                     native eth: |    DA       |     SA      |  VLAN | 
 *                                 ------------------------------------- 
 *                                 | native_MC   | routerC_mac |  vpn  | 
 *                                 ------------------------------------- 
 *                                                                      
 *                                     ---------------------------         
 *                         native ip:  |   SIP     | DIP          |        
 *                                     ----------------------------       
 *                                     | vmA0_ip   |  native_MC   |        
 *                                     ----------------------------   
 * 
 * 
 * Recieve 
*                  ------------------------------------- 
*      native eth: |    DA       |     SA      |  VLAN | 
*                  ------------------------------------- 
*                  | native_MC   | routerA_mac |   v1  | 
*                  ------------------------------------- 
*                                                       
*                      ---------------------------         
*          native ip:  |   SIP     | DIP          |        
*                      ----------------------------       
*                      | vmA0_ip   |  native_MC   |        
*                      ----------------------------   
 * 
 * 
 * - get the VSI from the VNI (vni -> vsi table in isem)
 * - DA is identified as compatible MC, ethernet header is terminated
 * - get the RIF (=vsi)
 * - RIF is compatible MC
 * - tunnel is terminated at TT (currently, tunnel is terminated at second pass: VTT program: if recycle port and IP MC packet, terminate the tunnel)
 * - get the inRif (=VSI)
 * - packet is forwarded according to MC
 * - RIF is compatible MC                                      
 * - packet is forwarded according to DIP and VRF (inRif.vrf)  
 * - result is MC  
 * - MC members: outLif, outRif and port
 * - for MC entry to provider routerD, vxlan outLif, outRif and port 
 *   - outRif = inRif ( = vsi (vsi come from VNI -> VSI mapping)) 
 *   - fall to bridge
 *   - ip tunnel inLif.fwrd_group = ip tunnel outLif. frwrd_group, filter the packet. (orientation filter, BUD to BUD is filtered)
 *  - for MC entries to access router E and access Router B, do routing
 */


/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 */

verbose = 2; 

struct vxlan_roo_mc_s {
    /* native header */
    int       native_mc_dip; 
    int       access_vlan; /* vlan at access ( = vsi = rif) */
    int       access_vlan_2; /* vlan at access ( = vsi = rif) */
    bcm_mac_t native_mc_da; 
    bcm_ip6_t native_mc_dip6; /* For ipv6 mode feature */

    int       vpn; /* vxlan vpn, in native vlan (= vsi = rif) for packet that arrive from provider network.
                                 out native vlan (= vsi = rif) for packet that go to provider network. */

    /* native arp/LL */
    int native_ll_outlif; /* interface of type encap: outlif (native arp eedb index) */

    /* vrf */
    int vrf; 

    /* overlay arp/LL */
    bcm_if_t overlay_ll_outlif; /* interface of type encap: outlif (or overlay arp eedb index) */ 

    /* outer vlan for the Eth of the overlay: */
    int overlay_out_vlan; 

    /* overlay header */
    int       overlay_mc_dip; 
    bcm_mac_t overlay_mc_da; 

    uint8 use_ipv6; /* Feature: use ipv6 instead of ipv4 */

};

struct vxlan_roo_filter_s {
    int       mtu_valid;
    int       mtu;       /* link layer mtu size */
    int       mtu_fwd;   /* forward layer mtu size */
    int       ttl_valid;
    int       ttl;       /* ttl scope */

};

vxlan_roo_filter_s g_vxlan_roo_filter =
{
    0,  /* mtu_valid */
    0,  /* mtu */
    0,  /* mtu_fwd */
    0,  /* ttl_valid */
    0   /* ttl */
};


vxlan_roo_mc_s g_vxlan_roo_mc = 
{
/* native header
 * native MC DIP: 224:0.1.1 | native vlan | native vlan 2 | native MC DA:  01:00:5e:00:01:01 */
0xE0000101,                   35,           40,            {0x01, 0x00, 0x5e, 0x00, 0x01, 0x01},

/* Native dip ipv6 for ipv6 mode feature: */
    {0xff, 0x34, 0x56, 0x78, 0x90, 0x23, 0x45, 0x67, 0x89, 0x01, 0x09, 0x87, 0x65, 0x43, 0x21, 0x1},
/* vpn */
15, 
/* native ll outlif */
0x3006,

/* vrf */
123,
     
/* overlay ll outlif */
 0, 

/* overlay header
 * 
 * overlay out vlan */
 0x64, 
/* overlay MC DIP: 224:0.2.2 | overlay MC DA:  01:00:5e:00:02:02 */
0xE0000202,                    {0x01, 0x00, 0x5e, 0x00, 0x02, 0x02},
/* Feature: use ipv6 instead of ipv4 */
 0
};

void vxlan_roo_mc_init(vxlan_roo_mc_s *param) {
    if (param != NULL) {
        sal_memcpy(&g_vxlan_roo_mc, param, sizeof(g_vxlan_roo_mc));
    }
}

void vxlan_roo_mc_struct_get(vxlan_roo_mc_s *param) {
    sal_memcpy( param, &g_vxlan_roo_mc, sizeof(g_vxlan_roo_mc));
}


/* create native rif,
 * update it with vrf and RPF,
   IPMC is enabled by default */
int vxlan_roo_mc_create_rif(int unit, bcm_mac_t my_mac, int vlan, int vrf, int* native_ing_intf) {
    int rv; 
    bcm_l3_intf_t native_l3_intf; 

    bcm_l3_intf_t_init(&native_l3_intf); 

    sal_memcpy(native_l3_intf.l3a_mac_addr, my_mac, 6); /* native my mac */
    native_l3_intf.l3a_vid = vlan; /* rif id */
    if (g_vxlan_roo_filter.mtu_valid) {
        native_l3_intf.l3a_mtu = g_vxlan_roo_filter.mtu;
        native_l3_intf.l3a_mtu_forwarding= g_vxlan_roo_filter.mtu_fwd;
    }
    if (g_vxlan_roo_filter.ttl_valid) {
        native_l3_intf.l3a_ttl= g_vxlan_roo_filter.ttl;
    }

    /* native ethernet compensation is supported until Jericho.
       Helps to to calculate the UDP.length by indicating how many vlans are expected
       Not needed in QAX, since native LL has already been built in native LL block */
    if (!is_device_or_above(unit,JERICHO_PLUS)) {
       native_l3_intf.native_routing_vlan_tags = 1; /* native ethernet compensation */ 
    }
    rv = bcm_l3_intf_create(unit, &native_l3_intf);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
      printf("Error, bcm_l3_intf_create, vlan: 0x%x \n", vlan);
      return rv;
    }
    *native_ing_intf = native_l3_intf.l3a_intf_id; 

    /* update RIF: update vrf and rpf */
    rv = l3__update_rif_vrf_rpf(unit,
                                &native_l3_intf,
                                vrf, /* router instance */
                                0 /* flags ( no rpf check for roo application: so flag BCM_L3_RPF is forbidden here.)  */
    ); 
    if (rv != BCM_E_NONE) {
        printf("Error, l3__update_rif_vrf_rpf\n");
    }

    if (verbose >= 2) {
        printf("update native rif 0x%x with vrf 0x%x, and disable rpf check \n", *native_ing_intf, g_vxlan_roo_mc.vrf);
    }

    return rv; 
}


/* disable split horizon per lif orientation.
 * diable hub (lif.forwarding_group = 1) to hub split horizon. 
 */
int vxlan_roo_mc_disable_split_horizon(int unit) {
    
    int rv; 

    bcm_switch_network_group_t inlif_forwarding_group = 1; 
    bcm_switch_network_group_config_t group_config; 
    bcm_switch_network_group_config_t_init(&group_config); 

    group_config.dest_network_group_id = 1; /* outlif forwarding group */
    group_config.config_flags = 0; /* disable filter */

    rv = bcm_switch_network_group_config_set(unit, inlif_forwarding_group, &group_config); 
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_network_group_config_set\n");
    }

    return rv; 
}

int vxlan_roo_mc_run(int unit, 
                     int access_port1, 
                     int access_port2, 
                     int provider_port1, 
                     int provider_port2) {

    int rv; 


    printf("access port1: %d \n", access_port1); 
    printf("access port2: %d \n", access_port2); 
    printf("provider port1: %d \n", provider_port1); 
    printf("provider port2: %d \n", provider_port2); 


    /*** init ***/

    rv = vlan_translation_default_mode_init(unit);
    if (rv != BCM_E_NONE) {
		printf("Error, vlan_translation_default_mode_init \n");
	}

    /** init global variables **/

    /* init ip tunnel global */
    ip_tunnel_s tunnel_1;   /* tunnel 1 info from cint_ip_tunnel.c */
    ip_tunnel_s tunnel_2;   /* tunnel 2 is not used in vxlan roo mc*/
    /* init ip tunnel info, to fit VXLAN usage */
    ip_tunnel_glbl_init_vxlan(unit,0);
    ip_tunnel_info_get(&tunnel_1, &tunnel_2);
    ip_tunnel_glbl_info.tunnel_1.dip = g_vxlan_roo_mc.overlay_mc_dip; 

    /* init ip tunnel termination global 
       skip ethernet flag is available until arad+. 
       For jericho, support 2nd my mac termination for native ethernet termination. */
    ip_tunnel_term_glbl_info.skip_ethernet_flag = (!is_device_or_above(unit,JERICHO));
    printf("skip ethernet flag: %d \n", ip_tunnel_term_glbl_info.skip_ethernet_flag); 
    ip_tunnel_term_glbl_info.dip2 = g_vxlan_roo_mc.overlay_mc_dip; 


     /* init vxlan globals */
     vxlan_s vxlan_param; 
     vxlan_struct_get(&vxlan_param);
     vxlan_param.vpn_id =  g_vxlan_roo_mc.vpn;
     vxlan_init(unit, &vxlan_param);


    /* use default vxlan roo global configuration */


    /** init local variables **/

    bcm_gport_t in_tunnel_gports[2];
    bcm_gport_t out_tunnel_gports[2];
    bcm_if_t out_tunnel_intf_ids[2];/* out tunnels interfaces
                                  out_tunnel_intf_ids[0] : is tunnel-interface-id
                                  out_tunnel_intf_ids[1] : is egress-object (FEC) points to tunnel
                              */
    bcm_if_t out_tunnel_intf_ids_only[2]; /* out tunnels interfaces only (out_tunnel_intf_ids returns FEC for 1) */

    int eg_intf_ids[2];/* interface for routing, not used */
    bcm_gport_t vlan_port_id;
    int vpn = g_vxlan.vpn_id; /* vpn = vsi at the provider side of the router */

    /** init modules **/

    /* init L2 VXLAN module */
    rv = bcm_vxlan_init(unit); 
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vxlan_init \n");
        return rv;
    }

    /* set trap for bounce back filter to drop */
    set_trap_to_drop(unit, bcmRxTrapEgTrillBounceBack);

    /** configure global my mac **/
    l2__mymac_properties_s mymac;
    sal_memset(&mymac, 0, sizeof(mymac));

    sal_memcpy(mymac.mymac, tunnel_1.sa, sizeof(tunnel_1.sa));
    mymac.mymac_type = l2__mymac_type_global_msb;

    rv = l2__mymac__set(unit, &mymac); 
    if (rv != BCM_E_NONE) {
        printf("Error, l2__mymac__set\n");
        return rv;
    }


    /*** build LL for overlay/tunnel ***/
    /* In ROO, the overlay LL encapsulation is built with a different API call 
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
     * - we provide roo__overlay_link_layer_create and roo_overlay_eve to help to migrate from Jericho to QAX 
     */
    bcm_l2_egress_t l2_egress_overlay;
    bcm_l2_egress_t_init(&l2_egress_overlay);

    l2_egress_overlay.dest_mac   = g_vxlan_roo_mc.overlay_mc_da; /* next hop. default: 01:00:5e:00:02:02 */
    l2_egress_overlay.src_mac    = tunnel_1.sa; /* my-mac. default:  {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}  */
    l2_egress_overlay.outer_vlan = g_vxlan_roo_mc.overlay_out_vlan; /* PCP DEI (0) + outer_vlan (96=0x60)  */
    l2_egress_overlay.ethertype  = 0x800;       /* ethertype for IP */
    l2_egress_overlay.outer_tpid = 0x8100;      /* outer tpid */
    if (g_vxlan_roo.overlay_ll_outlif != 0) {   /* outlif (or overlay LL eedb index) */
        l2_egress_overlay.encap_id = g_vxlan_roo.overlay_ll_outlif ;     
        /* indicate outlif is provided */
        l2_egress_overlay.flags    = BCM_L2_EGRESS_WITH_ID;  
    }
    rv = roo__overlay_link_layer_create(unit, &l2_egress_overlay,
                                        0, /* l3 flags */
                                        &(g_vxlan_roo_mc.overlay_ll_outlif),
                                        0);
    if (rv != BCM_E_NONE) {
        printf("Error, roo__overlay_link_layer_create \n");
        return rv; 
    } 
 
    if(verbose >= 2){
        printf("over LL encapsulation, outlif: 0x%x: \n", g_vxlan_roo_mc.overlay_ll_outlif);
    }


    /*** build tunnel initiators ***/
    if(verbose >= 2){
        printf("Open tunnels: \n\r");
    }
    rv = vxlan_roo_ipv4_tunnel_build_tunnels(unit, 
                                             provider_port1, 
                                             g_vxlan_roo_mc.overlay_ll_outlif, /* overlay LL */
                                             g_vxlan_roo_mc.overlay_out_vlan,
                                             out_tunnel_intf_ids,
                                             out_tunnel_gports, 
                                             out_tunnel_intf_ids_only);
    if (rv != BCM_E_NONE) {
        printf("Error, ipv4_tunnel_add_routes, in_port=%d, \n", provider_port1);
    }
    if(verbose >= 2){
        printf("Open out-tunnel 1 gport-id: 0x%08x (No FEC)\n\r", out_tunnel_gports[0]);
    }


    /** build tunnel terminations and router interfaces **/
    if(verbose >= 2){
        printf("Open tunnels tunnel terminators: \n\r");
    }

    /* make sure in tunnel gport and out tunnel gport have same lif value
       Requirement for vxlan port add api */
    BCM_GPORT_TUNNEL_ID_SET(in_tunnel_gports[0], out_tunnel_gports[0]); 

    rv = ipv4_tunnel_term_build_tunnel_terms(unit,
                                             provider_port1,
                                             access_port1,
                                             eg_intf_ids,
                                             in_tunnel_gports); 
    if (rv != BCM_E_NONE) {
        printf("Error, cip_tunnel_term_open_route_interfaces, in_port=%d, \n", provider_port1);
    }
    if(verbose >= 2){
        printf("Open in-tunnel 1 gport-id: 0x%08x (No FEC)\n\r", in_tunnel_gports[1]);
    }


    /*** build L2 VPN ***/

    rv = vxlan_open_vpn(unit,vpn,g_vxlan.vni);
    if (rv != BCM_E_NONE) {
        printf("Error, vxlan_open_vpn, vpn=%d, \n", vpn);
    }

    /*** ac ***/
    /* create inLif, outLif for routerB
     * Don't link lif to VSI. This is done in next api
     * inLif is used for access to provider or access. vlan x port -> inLif.
     * outLif is used fo access or provider to access. (CUD in access mc entry). 
     */
    bcm_gport_t access_vlan_port_id; 
    rv = l2__port_vlan__create(unit, 
                               0 /* flags*/, 
                               access_port1 /* port */,  
                               g_vxlan_roo_mc.access_vlan /* vlan */,  
                               0 /* vsi */,
                               access_vlan_port_id /* vlan port */); 
    if (rv != BCM_E_NONE) {
        printf("Error, l2__port_vlan__create\n");
        return rv;
    }

    /* link VSI to LIF */
    rv = bcm_vswitch_port_add(unit, g_vxlan_roo_mc.access_vlan /* VSI */, access_vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }


    if(verbose >= 1){
        printf("create AC-lif: 0x%x, port: %d, vlan: %d \n\r", access_vlan_port_id, access_port1, g_vxlan_roo_mc.access_vlan);
        printf("Connect AC-lif: 0x%x to VSI: %d \n", access_vlan_port_id, g_vxlan_roo_mc.access_vlan); 
    }

    /* create inLif, outLif for routerE
     * inLif is used for access to provider or access. vlan x port -> inLif.
     * outLif is used fo access or provider to access. (CUD in access mc entry). */
    bcm_gport_t access_vlan_port_id_2; 
    rv = l2__port_vlan__create(unit, 
                               0 /* flags*/, 
                               access_port2 /* port */,  
                               g_vxlan_roo_mc.access_vlan_2 /* vlan */,  
                               0 /* vsi */,
                               access_vlan_port_id_2 /* vlan port */); 
    if (rv != BCM_E_NONE) {
        printf("Error, l2__port_vlan__create\n");
        return rv;
    }

    /* link VSI to LIF */
    rv = bcm_vswitch_port_add(unit, g_vxlan_roo_mc.access_vlan_2 /* VSI */, access_vlan_port_id_2);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }

    if(verbose >= 1){
        printf("create AC-lif: 0x%x, port: %d, vlan: %d \n\r", access_vlan_port_id_2, access_port2, g_vxlan_roo_mc.access_vlan_2);
        printf("Connect AC-lif: 0x%x to VSI: %d \n", access_vlan_port_id_2, g_vxlan_roo_mc.access_vlan_2); 
    }

    /***  native routing ***/

    /* create rif, update vrf, rpf, enable IPMC
       inRif is used for provider to access, inRif = vsi (aka vpn, from mapping vni to vsi) */
    int vpn_native_ing_intf; 
    rv = vxlan_roo_mc_create_rif(unit, 
                                 tunnel_1.sa, /* native my mac: 00:0c:00:02:00:00 */
                                 vpn, /* vlan: Use vxlan vpn */
                                 g_vxlan_roo_mc.vrf,  /* router instance */
                                 &vpn_native_ing_intf 
                                 ); 
    if (rv != BCM_E_NONE) {
        printf("Error, vxlan_roo_mc_create_rif\n");
    }
    printf("native rif 0x%x for access copies \n", vpn_native_ing_intf);

    /* create rif, update vrf, rpf, enable IPMC,
     * inRif is used for access to provider. inRif = vsi = vlan. 
     * outRif is used for provider to access. 
             It specifies the native my mac and the out Vlan (outRif = out vsi = out vlan) */
    int access_native_ing_intf; 
    rv = vxlan_roo_mc_create_rif(unit,                     
                                   tunnel_1.sa,                    /* native my mac: 00:0c:00:02:00:00 */
                                   g_vxlan_roo_mc.access_vlan, /* rif_id = vlan */
                                   g_vxlan_roo_mc.vrf,             /* router instance */
                                   &access_native_ing_intf               
                                   );                             
    if (rv != BCM_E_NONE) {                                       
        printf("Error, vxlan_roo_mc_create_rif\n");        
    }                                                             
    printf("native rif 0x%x for access copies \n", access_native_ing_intf);


 
    /* create rif, update vrf,  rpf, enable IPMC
       inRif is used for provider to access. inRif = vsi = vlan */
    int provider_ing_intf; 
    rv = vxlan_roo_mc_create_rif(unit, 
                                 tunnel_1.sa, /* not used at inRif since mc packet, not used at outRif since overlay LL is built using l2_egress_create */
                                 g_vxlan_roo_mc.overlay_out_vlan, /* rif (= vsi = vlan) */
                                 g_vxlan_roo_mc.vrf,  /* router instance */
                                 &provider_ing_intf 
                                 ); 
    if (rv != BCM_E_NONE) {
        printf("Error, vxlan_roo_mc_create_rif\n");
    }
       
    /* create rif, update vrf, rpf, enable IPMC,
     * outRif is used for provider to access, and access to access 
             It specifies the native my mac and the out Vlan (outRif = out vsi = out vlan) */
    int access_2_native_ing_intf; 
    rv = vxlan_roo_mc_create_rif(unit, 
                                 tunnel_1.sa, /* not used at inRif since mc packet, not used at outRif since overlay LL is built using l2_egress_create */
                                 g_vxlan_roo_mc.access_vlan_2, /* rif (= vsi = vlan) */
                                 g_vxlan_roo_mc.vrf,  /* router instance */
                                 &access_2_native_ing_intf 
                                 ); 
    if (rv != BCM_E_NONE) {                                       
        printf("Error, vxlan_roo_mc_create_rif\n");        
    }                                                             
    printf("native rif 0x%x for access copies \n", access_2_native_ing_intf);


    /* configure vxlan gport using tunnel.
       Note: Don't need FEC since we're configuring mc entries */
  vxlan_port_add_s vxlan_port_add; 
  vxlan_port_s_clear(&vxlan_port_add); 
  vxlan_port_add.vpn = vpn; 
  vxlan_port_add.in_port = provider_port1; /* dummy, since no learning */
  vxlan_port_add.in_tunnel = in_tunnel_gports[0]; /* tunnel termination gport */
  vxlan_port_add.out_tunnel = out_tunnel_gports[0]; /* tunnel initiator gport */
  vxlan_port_add.egress_if = 0; /* fec entry for ip tunnel */
  vxlan_port_add.flags = 0; /* vxlan port flag */
  vxlan_port_add.network_group_id = g_vxlan.vxlan_network_group_id; 

  rv = vxlan__vxlan_port_add(unit, vxlan_port_add); 
  if (rv != BCM_E_NONE) {
      printf("Error, vxlan__vxlan_port_add 2, in_gport=0x%08x --> out_intf=0x%08x \n", in_tunnel_gports[1],out_tunnel_intf_ids[1]);
      return rv; 
  }

  g_vxlan_roo.vxlan_port_id = vxlan_port_add.vxlan_port_id; 



    /*** mc configuration 
     * we have only one MC group with entries:
     * - native routing entries: for access to access scenario and provider to access scenario (after tunnel termination)
     * - provider entries (roo entries): for access to provider scenario and provider to provider: interesting case is when we have fall back to bridge on provider to provider, should do same interface filter.
     * 
     * Note: 
     * Issue with IP tunnel termination for DIP compatible MC.
     * we do 2 pass solution, until we adjust VTT program for IP tunnel term
     * we need to terminate IPMC tunnel, VTT programs need to be adjusted in order to use IP tunnel terminator
     * To overcome this issue, we'll terminate IPMC tunnel by recycling the packet.
     * MC group from the IP tunnel has 1 entry: recycle port. 
 */

    /** create mc group **/

    /* with 1 roo entry, for access to provider scenario
     * and fall back to bridge and same interface filter scenario */
    int ipmc_index = 5000; 

    /* create mc group */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID, &ipmc_index);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }
    if (verbose >=2) {
        printf("multicast group created: %x \n", ipmc_index);
    }

    /** add provider mc entries (entries with 2 CUDS)
     *  and add access mc entries (entries with 1 CUD) **/

    bcm_multicast_replication_t reps[4];
    bcm_multicast_replication_t_init(reps[0]);
    bcm_multicast_replication_t_init(reps[1]); 
    bcm_multicast_replication_t_init(reps[2]);
    bcm_multicast_replication_t_init(reps[3]); 

     /* add access mc entry: to router B */
    /* cud outRif */
    rv = bcm_multicast_l3_encap_get(unit, -1 /* not used */, -1 /* not used */, 
                               access_native_ing_intf, &(reps[0].encap1));
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_l3_encap_get \n");
        return rv;
    }
    rv = bcm_multicast_vlan_encap_get(unit, -1 /* not used */, -1 /* not used */,
                                 access_vlan_port_id, &(reps[0].encap2));
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_vlan_encap_get \n");
        return rv;
    }
    BCM_GPORT_LOCAL_SET(reps[0].port, access_port1);
    reps[0].flags = BCM_MUTICAST_REPLICATION_ENCAP2_VALID  | BCM_MUTICAST_REPLICATION_ENCAP1_L3_INTF;

    /* add access mc entry: to router E */
    rv = bcm_multicast_l3_encap_get(unit, -1 /* not used */, -1 /* not used */, 
                               access_2_native_ing_intf, &(reps[1].encap1));
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_l3_encap_get \n");
        return rv;
    }
    rv = bcm_multicast_vlan_encap_get(unit, -1 /* not used */, -1 /* not used */, 
                                 access_vlan_port_id_2, &(reps[1].encap2)); 
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_vlan_encap_get \n");
        return rv;
    }
    reps[1].flags = BCM_MUTICAST_REPLICATION_ENCAP2_VALID  | BCM_MUTICAST_REPLICATION_ENCAP1_L3_INTF;
    BCM_GPORT_LOCAL_SET(reps[1].port, access_port2);


    /* add provider mc entry: to router D */
    /* cud outRif */
    rv = bcm_multicast_l3_encap_get(unit, -1 /* not used */, -1 /* not used */, 
                                    vpn_native_ing_intf, &(reps[2].encap1));
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_l3_encap_get \n");
        return rv;
    }
    /* cud outLif */
    rv = bcm_multicast_vxlan_encap_get(unit, -1 /* not used */, -1 /* not used */, 
                                       g_vxlan_roo.vxlan_port_id, reps[2].encap2); 
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_vxlan_encap_get \n");
        return rv;
    }
    BCM_GPORT_LOCAL_SET(reps[2].port, provider_port1);
    reps[2].flags = BCM_MUTICAST_REPLICATION_ENCAP2_VALID  | BCM_MUTICAST_REPLICATION_ENCAP1_L3_INTF;

    /* add provider mc entry: to router C  */
    /* cud outRif */
    rv = bcm_multicast_l3_encap_get(unit, -1 /* not used */, -1 /* not used */, 
                               vpn_native_ing_intf, &(reps[3].encap1));
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_l3_encap_get \n");
        return rv;
    }
    /* cud outLif */
    rv = bcm_multicast_vxlan_encap_get(unit, -1 /* not used */, -1 /* not used */, 
                                  g_vxlan_roo.vxlan_port_id, reps[3].encap2); 
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_vxlan_encap_get \n");
        return rv;
    }
    BCM_GPORT_LOCAL_SET(reps[3].port, provider_port2);
    reps[3].flags = BCM_MUTICAST_REPLICATION_ENCAP2_VALID  | BCM_MUTICAST_REPLICATION_ENCAP1_L3_INTF;

   rv = bcm_multicast_set(unit, ipmc_index, 0, 4, reps);
   if (rv != BCM_E_NONE) {
       printf("Error, bcm_multicast_set \n");
       return rv;
   }
                                                                                                                                                       
    if (verbose >=2) {                                                                                                                                   
        printf("multicast entries added to 0x%x \n", ipmc_index);                                                                                        
        printf("mc entry with 2 cuds: outLif: 0x%x outRif: 0x%x port: 0x%x added to 0x%x \n", reps[0].encap2, reps[0].encap1, reps[0].port, ipmc_index);                               
        printf("mc entry with 2 cuds: outLif: 0x%x outRif: 0x%x port: 0x%x added to 0x%x \n", reps[1].encap2, reps[1].encap1, reps[1].port, ipmc_index);                               
        printf("mc entry with 2 cuds: outLif: 0x%x outRif: 0x%x port: 0x%x added to 0x%x \n", reps[2].encap2, reps[2].encap1, reps[2].port, ipmc_index); 
        printf("mc entry with 2 cuds: outLif: 0x%x outRif: 0x%x port: 0x%x added to 0x%x \n", reps[3].encap2, reps[3].encap1, reps[3].port, ipmc_index); 
    }                                                                                                                                                    
                                                                                                                                                         

    /** add routing table entry **/
    multicast_forwarding_entry_dip_sip_s forwarding_entry;
    sal_memset(&forwarding_entry, 0, sizeof(forwarding_entry));
    if (g_vxlan_roo_mc.use_ipv6) {
        sal_memcpy(forwarding_entry.mc_ip6, g_vxlan_roo_mc.native_mc_dip6, sizeof(forwarding_entry.mc_ip6));
        forwarding_entry.src_ip = -1; /* Indicate we don't use src_ip */
        forwarding_entry.use_ipv6 = TRUE;
    } else {
        forwarding_entry.mc_ip = g_vxlan_roo_mc.native_mc_dip; /* dst mc ip, default: 224.0.1.1 */
        forwarding_entry.src_ip = -1;
    }
    rv = multicast__create_forwarding_entry_dip_sip(unit,                         
                                                    &forwarding_entry,
                                                    ipmc_index,                   
                                                    0, /* inRif */
                                                    g_vxlan_roo_mc.vrf
                                                    );                          
    if (rv != BCM_E_NONE) {                                                       
        printf("Error, multicast__create_forwarding_entry_dip_sip\n");                       
        return rv;                                                                
    }               
    if (verbose >=2) {                                                              
        printf("mc group entry %x added to routing table \n ", ipmc_index);         
    }


    /* create mc group with recycle port (for provider to access scenario) */

    /* we need to terminate IPMC tunnel, VTT programs need to be adjusted in order to use IP tunnel terminator
     * For now we'll terminate IPMC tunnel by recycling the packet.
     * MC group from the IP tunnel has 1 entry: recycle port.   */

    /* configure recycle port */
    int recycle_port = 40; 
    rv = bcm_port_control_set(unit,recycle_port, bcmPortControlOverlayRecycle, 1);  
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set\n");
        return rv;
    }

    /* create mc group */
    ipmc_index++;

    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID, ipmc_index); 
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }
    if (verbose >=2) {  
        printf("multicast group created: %x \n", ipmc_index);
    }

    /* overlay rif, for recycle port 
     * outrif is used to build the new link layer, SA = outrif.sa
     * inRif is used at the 2nd pass. Enable routing. 
       */
    int overlay_ing_intf; 
    int dummy_vlan = 77; 

    rv = vxlan_roo_mc_create_rif(unit, 
                                        tunnel_1.sa, 
                                        dummy_vlan, /* rif id: Use dummy vlan */
                                        g_vxlan_roo_mc.vrf,  /* router instance */
                                        &overlay_ing_intf 
                                        ); 
    if (rv != BCM_E_NONE) {
        printf("Error, vxlan_roo_mc_create_rif\n");
        return rv;
    }

    /* add recycle port to mc group */
    int members_of_mc[1] = {recycle_port};
    int nof_member_of_mc = 1;
    rv = multicast__add_multicast_entry(unit,
                                        ipmc_index, /* mc group */
                                        members_of_mc, /* port members of mc group */
                                        &dummy_vlan, /* encap_id aka CUD: outRif */
                                        nof_member_of_mc, /* nof members in mc group */
                                        0 /* ingress replication */); 
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__add_multicast_entry\n");
        return rv;
    }
    if (verbose >=2) {  
        printf("recycle port entrie added to mc group %x \n", ipmc_index);
    }

    /* add forwarding: DIP, VRF -> MC-group */
    multicast_forwarding_entry_dip_sip_s tmp_forwarding_entry;
    sal_memcpy(&tmp_forwarding_entry, &forwarding_entry, sizeof(forwarding_entry));
    tmp_forwarding_entry.mc_ip = g_vxlan_roo_mc.overlay_mc_dip; /* dst mc ip, default: 224:0.2.2 */
    tmp_forwarding_entry.use_ipv6 = 0;
    /*tmp_forwarding_entry.mc_ip6[14] += 1;*/
    rv = multicast__create_forwarding_entry_dip_sip(unit, 
                                                    &tmp_forwarding_entry, 
                                                    ipmc_index, 
                                                    0, /* inRif */
                                                    g_vxlan_roo_mc.vrf
                                                    ); 
    if (rv != BCM_E_NONE) {
        printf("Error, vxlan_roo_mc_create_rif\n");
        return rv;
    }
    if (verbose >=2) {                                                              
        printf("mc group entry %x added to routing table \n ", ipmc_index);       
    }

    /* dummy lif configuration for vxlan.
       Configure the tunnel termination to be a vxlan tunnel termination instead of the default ip tunnel termination */
    int tunnel_gport_dummy_lif = 0; 
    BCM_GPORT_TUNNEL_ID_SET(tunnel_gport_dummy_lif, 16484); 

  
    bcm_vxlan_port_t vxlan_port_dummy;
    bcm_vxlan_port_t_init(&vxlan_port_dummy);
    vxlan_port_dummy.match_port = provider_port1;
    vxlan_port_dummy.match_tunnel_id = tunnel_gport_dummy_lif;
    vxlan_port_dummy.criteria = BCM_VXLAN_PORT_MATCH_VN_ID;
    vxlan_port_dummy.flags |= BCM_VXLAN_PORT_NETWORK;
    vxlan_port_dummy.network_group_id = g_vxlan.vxlan_network_group_id;
    rv = bcm_vxlan_port_add(unit,vpn,&vxlan_port_dummy);
    if(rv != BCM_E_NONE) {
        printf("error bcm_vxlan_port_add \n");
        return rv;
    }



    return rv; 

}
