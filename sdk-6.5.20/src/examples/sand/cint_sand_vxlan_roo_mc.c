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
 * cint ../../../../src/examples/dnx/utility/cint_dnx_util_rch.c
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

struct cint_vxlan_roo_mc_s 
{
    int access_port[2];                     /* incoming port from access side */
    int provider_port[2];                   /* incoming port from provider side */
    int access_eth_rif[2];                  /* access RIF */
    bcm_gport_t vlan_port_id[2];
    
    /* native header */
    bcm_ip_t  native_mc_dip;
    bcm_ip_t  native_mc_dip_mask;
    bcm_ip6_t native_mc_dip6; 
    bcm_ip6_t native_mc_dip6_mask; 
    int       native_ipmc;
    /* overlay header */
    bcm_ip_t  overlay_mc_dip; 
    bcm_ip_t  overlay_mc_dip_mask;
    bcm_ip6_t overlay_mc_dip6; 
    bcm_ip6_t overlay_mc_dip6_mask; 
    bcm_mac_t overlay_mc_da; 
    int       overlay_ipmc;
    int recycle_port;
    int provider_eth_dummy_rif;
    int recycle_entry_encap_id;
    int       mtu_valid;
    int       mtu;       /* link layer mtu size */
    int       mtu_fwd;   /* forward layer mtu size */
    int       ttl_valid;
    int       ttl;       /* ttl scope */
};


cint_vxlan_roo_mc_s g_vxlan_roo_mc =
{
    /* access_port */
    {200, 201},
    /* provider_port */
    {202, 203},
    /* access_eth_rif */
    {35, 40},
    /* vlan_port_id */
    {0, 0},
    /* native_mc_dip */
    0xE0000101,
    /* native_mc_dip_mask */
    0xFFFFFFFF,
    /* native_mc_dip6 */
    {0xFF, 0x34, 0x56, 0x78, 0x90, 0x23, 0x45, 0x67, 0x89, 0x01, 0x09, 0x87, 0x65, 0x43, 0x21, 0x01},
    /* native_mc_dip6_mask */
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
    /* native_ipmc */
    5000,
    /* overlay_mc_dip */
    0xE0000202,
    /* overlay_mc_dip_mask */
    0xFFFFFFFF,
    /* overlay_mc_dip6 */
    {0xFF, 0xFF, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE},
    /* overlay_mc_dip6_mask */
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
    /* overlay_mc_da */
    {0x01, 0x00, 0x5e, 0x00, 0x02, 0x02},
    /* overlay_ipmc */
    5001,
    /* recycle_port */
    40,
    /* provider_eth_dummy_rif */
    77,
    /* recycle_entry_encap_id */
    0,
    /* mtu_valid */
    0, 
    /* mtu */
    0,
    /* mtu_fwd */
    0,
    /* ttl_valid */
    0,  
    /* ttl */
    0   
};

int set_trap_to_drop(int unit, bcm_rx_trap_t trap) {
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t config;
    int flags = 0;
    int trap_id;

    bcm_rx_trap_config_t_init(&config);
             
    config.flags = (BCM_RX_TRAP_UPDATE_DEST); 
    config.trap_strength = 7;
    config.dest_port=BCM_GPORT_BLACK_HOLE;

    rv = bcm_rx_trap_type_create(unit,flags,trap,&trap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in trap create, trap %d \n", trap_id);
        return rv;
    }

    rv = bcm_rx_trap_set(unit,trap_id,&config);
    if (rv != BCM_E_NONE) {
        printf("Error, in trap set \n");
        return rv;
    }

    return rv;
}

int
vxlan_roo_mc_dip_init(
    int unit)
{
    int rv = BCM_E_NONE;

    ip_tunnel_basic_info.dip[0] = g_vxlan_roo_mc.overlay_mc_dip;
    sal_memcpy(&(ip_tunnel_basic_info.dip6[0]), g_vxlan_roo_mc.overlay_mc_dip6, 16);
    
    return rv;
}

int
vxlan_roo_mc_init(
    int unit,
    int is_ipv6,
    int is_leaf_node,
    int access_port1,
    int access_port2,
    int provider_port1,
    int provider_port2)
{
    int rv = BCM_E_NONE;

    ip_tunnel_basic_info.roo = TRUE;  
    ip_tunnel_basic_info.tunnel_type[0] = is_ipv6 ? bcmTunnelTypeVxlan6 : bcmTunnelTypeVxlan;
    ip_tunnel_basic_info.nof_tunnels = 1;
    g_vxlan_roo_mc.access_port[0] = access_port1;
    g_vxlan_roo_mc.access_port[1] = access_port2;
    g_vxlan_roo_mc.provider_port[0] = provider_port1;
    g_vxlan_roo_mc.provider_port[1] = provider_port2;    
    ip_tunnel_basic_info.dip[0] = g_vxlan_roo_mc.overlay_mc_dip;
    sal_memcpy(&(ip_tunnel_basic_info.dip6[0]), g_vxlan_roo_mc.overlay_mc_dip6, 16);
    sal_memcpy(ip_tunnel_basic_info.encap_next_hop_mac[0], g_vxlan_roo_mc.overlay_mc_da, 6);    
    ip_tunnel_basic_info.term_bud = (is_device_or_above(unit,JERICHO_PLUS)) && !is_leaf_node;
    
    if (!is_device_or_above(unit, JERICHO2)) {       
        /* init L2 VXLAN module,  needed only for JER1*/
        rv = bcm_vxlan_init(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vxlan_init \n");
            return rv;
        }

        port_tpid_init(access_port1, 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, port_tpid_set with access_port1\n");
            return rv;
        }

        
        port_tpid_init(access_port2, 1, 1);
                rv = port_tpid_set(unit);
                if (rv != BCM_E_NONE) {
                    printf("Error, port_tpid_set with access_port2\n");
                    return rv;
                }

        port_tpid_init(provider_port1, 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, port_tpid_set with provider_port1\n");
            return rv;
        }

        port_tpid_init(provider_port2, 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, port_tpid_set with provider_port2\n");
            return rv;
        }
        
        /* In advanced vlan translation mode, the default ingress/ egress actions and mapping
               are not configured. This is here to compensate. */
        rv = vlan_translation_default_mode_init(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan_translation_default_mode_init\n");
            return rv;
        }
    }

    return rv;
}


/*
 * Interfaces initialisation function for ip tunnel termination and encapsulation scenarios
 * The function ip_tunnel_open_route_interfaces implements the basic configuration,
 * on top of witch all tunnel terminators and initiators are built. All tunnel termination and encapsulation scenarios will call it.
 * Inputs:   unit          - relevant unit;
 *           access_port - access port;
 *           provider_port   - provider port;
 */
int
vxlan_roo_mc_open_route_interfaces(
    int unit)
{
    int rv = BCM_E_NONE;
    char *proc_name;

    proc_name = "vxlan_roo_mc_open_route_interfaces";

    /*
     * Configure ingress interfaces
     */
    rv = vxlan_roo_mc_open_route_interfaces_access(unit, proc_name);
    if (rv != BCM_E_NONE)
    {
        printf("Error, ip_tunnel_open_route_interfaces_access \n");
        return rv;
    }

    /*
     * Configure egress interfaces
     */
    rv = vxlan_roo_mc_open_route_interfaces_provider(unit, proc_name);
    if (rv != BCM_E_NONE)
    {
        printf("Error, ip_tunnel_open_route_interfaces_provider \n");
        return rv;
    }

    /*
     * Configure egress interfaces
     */
    rv = vxlan_roo_mc_open_route_interfaces_provider_dummy(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vxlan_roo_mc_open_route_interfaces_provider_dummy \n");
        return rv;
    }

    return rv;
}


/*
 * Configure ip tunnel termination egress interfaces and ip tunnel encapsulation ingress interfaces
 * Inputs: access_port - outgoing port;
 *         *proc_name  - main function name;
 */
int
vxlan_roo_mc_open_route_interfaces_access(
    int unit,
    char *proc_name)
{
    int idx;
    int rv = BCM_E_NONE;
    sand_utils_l3_eth_rif_s eth_rif_structure;

    for (idx = 0; idx < 2; idx++)
    {
        /* Set <port, vlan> to LIF  */        
        rv = vxlan_vlan_port_add(unit, g_vxlan_roo_mc.access_port[idx], g_vxlan_roo_mc.access_eth_rif[idx], &g_vxlan_roo_mc.vlan_port_id[idx]);
        if (rv != BCM_E_NONE) 
        {
            printf("Error(%d), vxlan_vlan_port_add \n", rv);
            return rv;
        }
        
        /* Create VSI */
        rv = bcm_vswitch_create_with_id(unit, g_vxlan_roo_mc.access_eth_rif[idx]);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in bcm_dnx_vswitch_create_with_id\n");
            return rv;
        }
        
        /* Set LIF -> VSI(RIF) */  
        rv = bcm_vswitch_port_add(unit, g_vxlan_roo_mc.access_eth_rif[idx], g_vxlan_roo_mc.vlan_port_id[idx]);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_port_add\n");
            return rv;
        }        

        /* Set RIF -> My-Mac  and RIF -> VRF */
        sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, g_vxlan_roo_mc.access_eth_rif[idx], 0, BCM_L3_INGRESS_GLOBAL_ROUTE, ip_tunnel_basic_info.access_eth_rif_mac, ip_tunnel_basic_info.access_eth_vrf);
        sand_utils_l3_eth_rif_s_additonal_mtu_ttl_init(unit, &eth_rif_structure, g_vxlan_roo_mc.mtu_valid, g_vxlan_roo_mc.mtu, g_vxlan_roo_mc.mtu_fwd, g_vxlan_roo_mc.ttl_valid, g_vxlan_roo_mc.ttl);
        rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
        if (rv != BCM_E_NONE)
        {
            printf("Error, sand_utils_l3_eth_rif_create access_eth_rif[%d]\n", idx);
            return rv;
        }
    }

    return rv;
}


/*
 * Configure ip tunnel termination ingress interfaces and ip tunnel encapsulation egress interfaces
 * Inputs: provider_port - incomming port;
 *         *proc_name    - main function name;
 */
int
vxlan_roo_mc_open_route_interfaces_provider(
    int unit,
    char *proc_name)
{
    int rv = BCM_E_NONE;
    int tunnel_idx;
    sand_utils_l3_eth_rif_s eth_rif_structure;

    /*
     * Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, g_vxlan_roo_mc.provider_port[0], ip_tunnel_basic_info.provider_eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set provider_eth_rif \n", proc_name);
        return rv;
    }

    /*
     * Create routing interface for the tunnel's IP.
     * We are using it as ingress ETH-RIF to perform ETH termination (my-mac procedure), to enable
     * IPv4 routing for this ETH-RIF and to set the VRF.
     */
    sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, ip_tunnel_basic_info.provider_eth_rif, 0, BCM_L3_INGRESS_GLOBAL_ROUTE, ip_tunnel_basic_info.provider_eth_rif_mac, ip_tunnel_basic_info.provider_eth_vrf);
    sand_utils_l3_eth_rif_s_additonal_mtu_ttl_init(unit, &eth_rif_structure, g_vxlan_roo_mc.mtu_valid, g_vxlan_roo_mc.mtu, g_vxlan_roo_mc.mtu_fwd, g_vxlan_roo_mc.ttl_valid, g_vxlan_roo_mc.ttl);
    rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, sand_utils_l3_eth_rif_create provider_eth_rif\n");
        return rv;
    }

    /*
     * In DPP (JR1) architecture IP-Tunnel LIF retrieve its RIF properties according to RIF-ID object (from tunnel_if).
     * While in DNX architecture (JR2) RIF properties are retrieved directly from IP-Tunnel LIF.
     */
    if (!is_device_or_above(unit, JERICHO2))
    {
        for (tunnel_idx = 0; tunnel_idx < ip_tunnel_basic_info.nof_tunnels; tunnel_idx++)
        {
            /** Create ingress Tunnel interfaces - the function is used for Jericho 1 only. */
            sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, ip_tunnel_basic_info.tunnel_rif[tunnel_idx], 0, 0, ip_tunnel_basic_info.provider_eth_rif_mac, ip_tunnel_basic_info.tunnel_vrf[tunnel_idx]);
            rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
            if (rv != BCM_E_NONE)
            {
                printf("Error, sand_utils_l3_eth_rif_create tunnel_rif[%d]\n", tunnel_idx);
                return rv;
            }
        }
    }    

    if (ip_tunnel_basic_info.roo) {
        /*
         * Create routing interface for the routing's IP.
         * We use it as ingress ETH-RIF to perform native-ETH termination (my-mac procedure), 
         * to enable IPv4 routing for this ETH-RIF and to set the VRF.
         */       
        sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, ip_tunnel_basic_info.provider_native_eth_rif, 0, BCM_L3_INGRESS_GLOBAL_ROUTE, ip_tunnel_basic_info.provider_native_eth_rif_mac, ip_tunnel_basic_info.provider_native_eth_vrf);
        sand_utils_l3_eth_rif_s_additonal_mtu_ttl_init(unit, &eth_rif_structure, g_vxlan_roo_mc.mtu_valid, g_vxlan_roo_mc.mtu, g_vxlan_roo_mc.mtu_fwd, g_vxlan_roo_mc.ttl_valid, g_vxlan_roo_mc.ttl);
        rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
        if (rv != BCM_E_NONE)
        {
            printf("Error, sand_utils_l3_eth_rif_create provider_native_eth_rif\n");
            return rv;
        }
    }

    return rv;
}

/*
 * Configure ip tunnel termination ingress interfaces and ip tunnel encapsulation egress interfaces
 * Inputs: provider_port - incomming port;
 *         *proc_name    - main function name;
 */
int
vxlan_roo_mc_open_route_interfaces_provider_dummy(
    int unit)
{
    int rv = BCM_E_NONE;
    int tunnel_idx;    
    sand_utils_l3_eth_rif_s eth_rif_structure;    

    if (!is_device_or_above(unit, JERICHO2)) {
         /*
         * Create routing interface for the tunnel's IP.
         * We are using it as ingress ETH-RIF to perform ETH termination (my-mac procedure), to enable
         * IPv4 routing for this ETH-RIF and to set the VRF.
         */
        sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, g_vxlan_roo_mc.provider_eth_dummy_rif, 0, 0, ip_tunnel_basic_info.provider_eth_rif_mac, ip_tunnel_basic_info.provider_eth_vrf);
        /* don't enable the ttl scope filter on the dummy lif */
        sand_utils_l3_eth_rif_s_additonal_mtu_ttl_init(unit, &eth_rif_structure, g_vxlan_roo_mc.mtu_valid, g_vxlan_roo_mc.mtu, g_vxlan_roo_mc.mtu_fwd, 0, 0);
        rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
        if (rv != BCM_E_NONE)
        {
            printf("Error, sand_utils_l3_eth_rif_create provider_eth_dummy_rif\n");
            return rv;
        } 
    } else {
        rv = create_recycle_entry(unit, &g_vxlan_roo_mc.recycle_entry_encap_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create_recycle_entry \n");
            return rv;
        }
    }

    return rv;
}

/*
 * MTU trap by RIF
 */
int vxlan_roo_mc_mtu_rif_trap(int unit){
    int rv = BCM_E_NONE;
    int rif_mtu, tunnel_mtu;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t info;
    int comp_fwd_layer_type = 2;

    /* Eth(12) + VLAN(4) + Ethertype(2) + VXLAN(36) = 54 */
    rif_mtu = g_vxlan_roo_mc.mtu - 54;
    tunnel_mtu = g_vxlan_roo_mc.mtu;
    if ( g_vxlan_roo_mc.mtu_valid && is_device_or_above(unit, JERICHO2)) {  
        /* Creating a compressed forwarding layer type */
        key.type = bcmSwitchLinkLayerMtuFilter;
        key.index = bcmFieldLayerTypeIp4;
        info.value = comp_fwd_layer_type;

        rv = bcm_switch_control_indexed_set(unit, key, info);
        if(rv != BCM_E_NONE)
        {
            printf("Error in bcm_switch_control_indexed_set \n");
            return rv;
        }
        
        rv = mtu_check_etpp_rif_set(unit, g_vxlan_roo_mc.access_eth_rif[0], comp_fwd_layer_type, rif_mtu, 1);
        if (rv != BCM_E_NONE) {
            printf("Error, mtu_check_etpp_rif_set g_vxlan_roo_mc.access_eth_rif[0] \n");
            return rv;
        }
        rv = mtu_check_etpp_rif_set(unit, g_vxlan_roo_mc.access_eth_rif[1], comp_fwd_layer_type, rif_mtu, 1);
        if (rv != BCM_E_NONE) {
            printf("Error, mtu_check_etpp_rif_set g_vxlan_roo_mc.access_eth_rif[1] \n");
            return rv;
        }
        rv = mtu_check_etpp_rif_set(unit, ip_tunnel_basic_info.encap_tunnel_intf[0], comp_fwd_layer_type, tunnel_mtu, 1);
        if (rv != BCM_E_NONE) {
            printf("Error, mtu_check_etpp_rif_set ip_tunnel_basic_info.encap_tunnel_intf[0] \n");
            return rv;
        }
    }

    return rv;
}

int 
multicast_vxlan_encap_get(
    int unit, 
    bcm_gport_t vxlan_port_id,
    bcm_if_t * encap_lif) {

    int rv = BCM_E_NONE;
    rv = bcm_multicast_vxlan_encap_get(unit, -1 /* not used */, -1 /* not used */, 
                                       vxlan_port_id, encap_lif);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_vxlan_encap_get \n");
        return rv;
    }
    return rv;
}

/* get the vxlan encap lif.
   From the vxlan gport if possible, otherwise, directly from the tunnel outlif */
int 
vxlan_encap_lif_get(
    int unit, 
    int tunnel_idx,
    bcm_if_t * encap_lif) {

    int rv = BCM_E_NONE;

    /* in case we learn FEC only, the vxlan port has been configured with a FEC/ECMP.
       The outlif is pointed by the FEC. But from the FEC/ECMP we don't have access. 
       Therefore, we'll get the tunnel initiator outlif */
    if (cint_vxlan_basic_info.learn_mode == LEARN_FEC_ONLY) {
        *encap_lif = ip_tunnel_basic_info.encap_tunnel_intf[tunnel_idx]&0x3fffff;
    }
    /* get the outlif from the vxlan port */
    else 
    {
        rv = multicast_vxlan_encap_get(unit, cint_vxlan_basic_info.vxlan_port_id, encap_lif);
        if (rv != BCM_E_NONE) {
            printf("Error, multicast_vxlan_encap_get \n");
            return rv;
        }
    }
    return rv;
}

/*
 * add gport of type vlan-port to the multicast
 */
int vxlan_roo_mc_add_native_mc(int unit, int tunnel_idx){
    int rv;
    int idx;
    uint32 flags = BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID;;
    bcm_if_t encap_rif[4];
    bcm_if_t encap_lif[4];
    bcm_if_t ifs[2];
    bcm_if_t rep_idx = 0x300100;
    bcm_multicast_replication_t reps[4];    
    bcm_ipmc_addr_t data;
    
    bcm_multicast_replication_t_init(reps[0]);
    bcm_multicast_replication_t_init(reps[1]); 
    bcm_multicast_replication_t_init(reps[2]);
    bcm_multicast_replication_t_init(reps[3]); 

    
    /* create mc group */
    if (!is_device_or_above(unit, JERICHO2)) {
        flags |= BCM_MULTICAST_TYPE_L3;
    }
    rv = bcm_multicast_create(unit, flags, &g_vxlan_roo_mc.native_ipmc);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }
    if (verbose >=2) {
        printf("multicast group created: %x \n", g_vxlan_roo_mc.native_ipmc);
    }

    /** add provider mc entries (entries with 2 CUDS)
     *  and add access mc entries (entries with 1 CUD) **/

    /* IDX 0: add access mc entry: to router B */
    /* IDX 1: add access mc entry: to router E */
    for (idx = 0; idx < 2; idx++) 
    {
        rv = bcm_multicast_l3_encap_get(unit, -1 /* not used */, -1 /* not used */, 
                                   g_vxlan_roo_mc.access_eth_rif[idx], &(encap_rif[idx]));
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_l3_encap_get \n");
            return rv;
        }
        rv = bcm_multicast_vlan_encap_get(unit, -1 /* not used */, -1 /* not used */,
                                     g_vxlan_roo_mc.vlan_port_id[idx], &(encap_lif[idx]));
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_vlan_encap_get \n");
            return rv;
        }
        BCM_GPORT_LOCAL_SET(reps[idx].port, g_vxlan_roo_mc.access_port[idx]);
        reps[idx].flags = BCM_MUTICAST_REPLICATION_ENCAP2_VALID  | BCM_MUTICAST_REPLICATION_ENCAP1_L3_INTF;
    }

    /* IDX 2: add provider mc entry: to router D */
    /* IDX 3: add provider mc entry: to router C */
    for (idx = 2; idx < 4; idx++) 
    {
        /* cud outRif */
        rv = bcm_multicast_l3_encap_get(unit, -1 /* not used */, -1 /* not used */, 
                                        ip_tunnel_basic_info.provider_native_eth_rif, &(encap_rif[idx]));
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_l3_encap_get \n");
            return rv;
        }
        /* cud outLif */
        rv = vxlan_encap_lif_get(unit, tunnel_idx, &(encap_lif[idx]));
        if (rv != BCM_E_NONE) {
            printf("Error, vxlan_encap_lif_get \n");
            return rv;
        }
        BCM_GPORT_LOCAL_SET(reps[idx].port, g_vxlan_roo_mc.provider_port[idx - 2]);
        reps[idx].flags = BCM_MUTICAST_REPLICATION_ENCAP2_VALID  | BCM_MUTICAST_REPLICATION_ENCAP1_L3_INTF;
    }

    if (is_device_or_above(unit, JERICHO2)) {
        flags = BCM_MULTICAST_EGRESS_GROUP;
        for (idx = 0; idx < 4; idx++) 
        {
            reps[idx].encap1 = rep_idx++;

            /*JR2 use PP DB to carry 2 pointers*/
            BCM_L3_ITF_SET(ifs[0], BCM_L3_ITF_TYPE_RIF, encap_rif[idx]);
            BCM_L3_ITF_SET(ifs[1], BCM_L3_ITF_TYPE_LIF, encap_lif[idx]);
            rv = bcm_multicast_encap_extension_create(unit, BCM_MULTICAST_ENCAP_EXTENSION_WITH_ID, &(reps[idx].encap1), 2, ifs);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in bcm_multicast_encap_extension_create \n");
                return rv;
            }
            printf("PPMC: CUD=0x%x  -->  ITF0=0x%x, ITF1=0x%x\n", reps[idx].encap1, ifs[0], ifs[1]);
        }        
    } else {
        flags = 0;
        for (idx = 0; idx < 4; idx++) 
        {
            reps[idx].encap1 = encap_rif[idx];
            reps[idx].encap2 = encap_lif[idx];
        }
    }
    rv = bcm_multicast_set(unit, g_vxlan_roo_mc.native_ipmc, flags, 4, &reps[0]);
    if (rv != BCM_E_NONE) {
       printf("Error, bcm_multicast_set \n");
       return rv;
    }
                                                                                                                                                       
    if (verbose >=2) {                                                                                                                                   
        printf("multicast entries added to 0x%x \n", g_vxlan_roo_mc.native_ipmc);                                                                                        
        printf("mc entry with 2 cuds: outLif: 0x%x outRif: 0x%x port: 0x%x added to 0x%x \n", reps[0].encap2, reps[0].encap1, reps[0].port, g_vxlan_roo_mc.native_ipmc);                               
        printf("mc entry with 2 cuds: outLif: 0x%x outRif: 0x%x port: 0x%x added to 0x%x \n", reps[1].encap2, reps[1].encap1, reps[1].port, g_vxlan_roo_mc.native_ipmc);                               
        printf("mc entry with 2 cuds: outLif: 0x%x outRif: 0x%x port: 0x%x added to 0x%x \n", reps[2].encap2, reps[2].encap1, reps[2].port, g_vxlan_roo_mc.native_ipmc); 
        printf("mc entry with 2 cuds: outLif: 0x%x outRif: 0x%x port: 0x%x added to 0x%x \n", reps[3].encap2, reps[3].encap1, reps[3].port, g_vxlan_roo_mc.native_ipmc); 
    }  

    rv = multicast__open_fabric_mc_or_ingress_mc_for_egress_mc(unit, g_vxlan_roo_mc.native_ipmc, g_vxlan_roo_mc.native_ipmc);
    if (rv != BCM_E_NONE) {
       printf("Error, multicast__open_ingress_mc_group_for_egress_mc \n");
       return rv;
    }

    /* Add IPv4 IPMC */
    bcm_ipmc_addr_t_init(&data);
    data.flags = 0x0;
    data.group = g_vxlan_roo_mc.native_ipmc;  
    data.mc_ip_addr = g_vxlan_roo_mc.native_mc_dip;
    data.mc_ip_mask = g_vxlan_roo_mc.native_mc_dip_mask;
    rv = bcm_ipmc_add(unit,&data);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_ipmc_add IPv4\n");
        return rv;
    }

    /* Add IPv6 IPMC */
    bcm_ipmc_addr_t_init(&data);
    data.flags = 0x0;
    data.group = g_vxlan_roo_mc.native_ipmc;  
    data.flags |= BCM_IPMC_IP6;
    data.vrf = ip_tunnel_basic_info.access_eth_vrf;
    sal_memcpy(data.mc_ip6_addr, g_vxlan_roo_mc.native_mc_dip6, 16);
    sal_memcpy(data.mc_ip6_mask, g_vxlan_roo_mc.native_mc_dip6_mask, 16);
    rv = bcm_ipmc_add(unit,&data);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_ipmc_add IPv6\n");
        return rv;
    }
	
    bcm_ipmc_addr_t_init(&data);
    data.flags = 0x0;
    data.group = g_vxlan_roo_mc.native_ipmc;  
    data.flags |= BCM_IPMC_IP6;
    data.vrf = ip_tunnel_basic_info.provider_native_eth_vrf;
    sal_memcpy(data.mc_ip6_addr, g_vxlan_roo_mc.native_mc_dip6, 16);
    sal_memcpy(data.mc_ip6_mask, g_vxlan_roo_mc.native_mc_dip6_mask, 16);
    rv = bcm_ipmc_add(unit,&data);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_ipmc_add IPv6\n");
        return rv;
    }
        
    return rv;
}

/*
 * add gport of type vlan-port to the multicast
 */
int vxlan_roo_mc_add_overlay_mc(int unit, int is_leaf_node){
    int rv;
    uint32 flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID;
     bcm_multicast_replication_t replications[2];  
    bcm_ipmc_addr_t data;
    
    /* create mc group */
    if (!is_device_or_above(unit, JERICHO2)) {
        flags |= BCM_MULTICAST_TYPE_L3;
    }    
    rv = bcm_multicast_create(unit, flags, &g_vxlan_roo_mc.overlay_ipmc); 
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }
    if (verbose >=2) {  
        printf("multicast group created: %x \n", g_vxlan_roo_mc.overlay_ipmc);
    }

    if (is_leaf_node) {
        /* In Jericho 2, we can terminate VXLAN ROO leaf node MC in 1st pass */
        if (!is_device_or_above(unit, JERICHO2)) {
            rv = bcm_multicast_ingress_add(unit, g_vxlan_roo_mc.overlay_ipmc, g_vxlan_roo_mc.recycle_port, g_vxlan_roo_mc.provider_eth_dummy_rif);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_multicast_ingress_add mc_group_id:  0x%08x  port:  0x%08x  encap_id:  0x%08x \n", g_vxlan_roo_mc.overlay_ipmc, g_vxlan_roo_mc.recycle_port, g_vxlan_roo_mc.provider_eth_dummy_rif);
                return rv;
            } 
         } 
    } else {
        if (!is_device_or_above(unit, JERICHO2)) {
            rv = bcm_multicast_ingress_add(unit, g_vxlan_roo_mc.overlay_ipmc, g_vxlan_roo_mc.provider_port[1], ip_tunnel_basic_info.provider_eth_rif);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_multicast_ingress_add mc_group_id:  0x%08x  port:  0x%08x  encap_id:  0x%08x \n", g_vxlan_roo_mc.overlay_ipmc, g_vxlan_roo_mc.recycle_port, g_vxlan_roo_mc.provider_eth_dummy_rif);
                return rv;
            } 
            
            rv = bcm_multicast_ingress_add(unit, g_vxlan_roo_mc.overlay_ipmc, g_vxlan_roo_mc.recycle_port, g_vxlan_roo_mc.provider_eth_dummy_rif);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_multicast_ingress_add mc_group_id:  0x%08x  port:  0x%08x  encap_id:  0x%08x \n", g_vxlan_roo_mc.overlay_ipmc, g_vxlan_roo_mc.recycle_port, g_vxlan_roo_mc.provider_eth_dummy_rif);
                return rv;
            } 
         } else {  
            bcm_multicast_replication_t_init(&replications[0]);
            replications[0].encap1 = ip_tunnel_basic_info.provider_eth_rif;
            replications[0].port = g_vxlan_roo_mc.provider_port[1];

            bcm_multicast_replication_t_init(&replications[1]);
            replications[1].encap1 = g_vxlan_roo_mc.recycle_entry_encap_id & 0x3fffff;
            replications[1].port = g_vxlan_roo_mc.recycle_port;
            
            rv = bcm_multicast_add(unit, g_vxlan_roo_mc.overlay_ipmc, BCM_MULTICAST_INGRESS_GROUP, 2, replications);
            if (rv != BCM_E_NONE)
            {
              printf("Error, in bcm_multicast_add mc_group_id:  0x%08x  port:  0x%08x  encap_id:  0x%08x \n", g_vxlan_roo_mc.overlay_ipmc, g_vxlan_roo_mc.recycle_port, g_vxlan_roo_mc.recycle_entry_encap_id);
              return rv;
            }
        }  
    }

    /* Add IPv4 IPMC */
    bcm_ipmc_addr_t_init(&data);
    data.flags = 0x0;
    data.group = g_vxlan_roo_mc.overlay_ipmc;  
    data.mc_ip_addr = g_vxlan_roo_mc.overlay_mc_dip;
    data.mc_ip_mask = g_vxlan_roo_mc.overlay_mc_dip_mask;
    data.vrf = ip_tunnel_basic_info.provider_eth_vrf;
    if (is_device_or_above(unit, JERICHO2)) {
        data.vid = ip_tunnel_basic_info.provider_eth_rif;
    }
    rv = bcm_ipmc_add(unit,&data);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_ipmc_add IPv4\n");
        return rv;
    }

    /* Add IPv6 IPMC */
    bcm_ipmc_addr_t_init(&data);
    data.flags = 0x0;
    data.group = g_vxlan_roo_mc.overlay_ipmc;  
    data.flags |= BCM_IPMC_IP6;
    data.vrf = ip_tunnel_basic_info.access_eth_vrf;
    sal_memcpy(data.mc_ip6_addr, g_vxlan_roo_mc.overlay_mc_dip6, 16);
    sal_memcpy(data.mc_ip6_mask, g_vxlan_roo_mc.overlay_mc_dip6_mask, 16);
    
    rv = bcm_ipmc_add(unit,&data);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_ipmc_add IPv4\n");
        return rv;
    }

    bcm_ipmc_addr_t_init(&data);
    data.flags = 0x0;
    data.group = g_vxlan_roo_mc.overlay_ipmc;  
    data.flags |= BCM_IPMC_IP6;
    data.vrf = ip_tunnel_basic_info.provider_eth_vrf;
    sal_memcpy(data.mc_ip6_addr, g_vxlan_roo_mc.overlay_mc_dip6, 16);
    sal_memcpy(data.mc_ip6_mask, g_vxlan_roo_mc.overlay_mc_dip6_mask, 16);
    
    rv = bcm_ipmc_add(unit,&data);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_ipmc_add IPv4\n");
        return rv;
    }
        
    return rv;
}

int vxlan_roo_mc_vxlan_dummy_port_add(
    int unit)
{    
    /* dummy lif configuration for vxlan.
       Configure the tunnel termination to be a vxlan tunnel termination instead of the default ip tunnel termination */
    int rv = BCM_E_NONE;
    int tunnel_gport_dummy_lif = 0; 
    bcm_vxlan_port_t vxlan_port_dummy;

    if (!is_device_or_above(unit, JERICHO2)) {
        BCM_GPORT_TUNNEL_ID_SET(tunnel_gport_dummy_lif, 16484);     
        bcm_vxlan_port_t_init(&vxlan_port_dummy);
        vxlan_port_dummy.match_port = g_vxlan_roo_mc.provider_port[0];
        vxlan_port_dummy.match_tunnel_id = tunnel_gport_dummy_lif;
        vxlan_port_dummy.criteria = BCM_VXLAN_PORT_MATCH_VN_ID;
        vxlan_port_dummy.flags |= BCM_VXLAN_PORT_NETWORK;
        vxlan_port_dummy.network_group_id = cint_vxlan_basic_info.vxlan_network_group_id;
        rv = bcm_vxlan_port_add(unit, cint_vxlan_basic_info.vpn_id, &vxlan_port_dummy);
        if(rv != BCM_E_NONE) {
            printf("error bcm_vxlan_port_add \n");
            return rv;
        }
    }

    return BCM_E_NONE;
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

/*
 * The basic VXLAN example with parameters:
 * Unit - relevant unit
 * access_port - port where core host is connected to.
 * provider_port - port where DC Fabric router is connected to.
 */
int vxlan_roo_mc_example(
    int unit,
    int is_ipv6,
    int is_leaf_node,
    int access_port1,
    int access_port2,
    int provider_port1,
    int provider_port2)
{

    int rv;
    int tunnel_idx = 0;
    
    rv = vxlan_roo_mc_init(unit, is_ipv6, is_leaf_node, access_port1, access_port2, provider_port1, provider_port2);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), vxlan_roo_mc_init \n", rv);
        return rv;
    }
    /* 
     * Build L2 VPN
     */
    rv = sand_utils_vxlan_open_vpn(unit, cint_vxlan_basic_info.vpn_id, cint_vxlan_basic_info.vxlan_vdc, cint_vxlan_basic_info.vni);
    if (rv != BCM_E_NONE) 
    {
        printf("Error(%d), in sand_utils_vxlan_open_vpn, vpn=%d, vdc=%d \n", rv, cint_vxlan_basic_info.vpn_id, cint_vxlan_basic_info.vxlan_vdc);
        return rv;
    }

    /*
     * Set the split horizon 
     */
    rv = vxlan_split_horizon_set(unit, cint_vxlan_basic_info.vpn_id, cint_vxlan_basic_info.vxlan_vdc, cint_vxlan_basic_info.vxlan_network_group_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vxlan_split_horizon_set\n", rv);
        return rv;
    }

    rv = ip_tunnel_fec_arp_map(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), ip_tunnel_fec_arp_map \n", rv);
        return rv;
    }
    
    /* Open route interfaces */
    rv = vxlan_roo_mc_open_route_interfaces(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), ip_tunnel_open_route_interfaces \n", rv);
        return rv;
    } 
   
    /*
     * Create the tunnel termination lookup and the tunnel termination inlif
     */
    rv = ip_tunnel_term_create(unit, tunnel_idx);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in function ip_tunnel_term_create \n", rv);
        return rv;
    }

    /*
     *Configure ARP entries 
     */
    rv = ip_tunnel_create_arp(unit, tunnel_idx);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in ip_tunnel_create_arp\n", rv);
        return rv;
    }
    /*
     * Create the tunnel initiator
     */
    rv = ip_tunnel_encap_create(unit, tunnel_idx);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in ip_tunnel_encap_create \n", rv);
        return rv;
    }

    /*
     * Configure fec entry
     */
    rv = ip_tunnel_create_fec(unit, tunnel_idx);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in ip_tunnel_create_fec\n", rv);
        return rv;
    }

    /* 
     * Add VXLAN port properties and configure the tunnel terminator In-Lif and tunnel initiatior Out-Lif
     */
    rv = vxlan_vxlan_port_add(unit, g_vxlan_roo_mc.recycle_port, tunnel_idx, FALSE);
    if (rv != BCM_E_NONE) 
    {
        printf("Error(%d), vxlan_vxlan_port_add \n", rv);
        return rv;
    }


    rv = vxlan_roo_mc_add_native_mc(unit, tunnel_idx);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), vxlan_roo_mc_add_native_mc \n", rv);
        return rv;
    }      

    /* this is for j1 only. overlay MC was used to perform 2nd pass.
       Not needed in J2 device where we can terminate the IP tunnel with MC DIP in one pass */
    if (!is_leaf_node || !is_device_or_above(unit, JERICHO2)) {
        /* configure recycle port */
        rv = bcm_port_control_set(unit,g_vxlan_roo_mc.recycle_port, bcmPortControlOverlayRecycle, 1);  
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_control_set\n");
            return rv;
        }

        rv = vxlan_roo_mc_add_overlay_mc(unit, is_leaf_node);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), vxlan_roo_mc_add_overlay_mc \n", rv);
            return rv;
        } 
    }

    rv = vxlan_roo_mc_vxlan_dummy_port_add(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), vxlan_roo_mc_vxlan_dummy_port_add \n", rv);
        return rv;
    }      
    
    /* MTU trap by RIF */
    rv = vxlan_roo_mc_mtu_rif_trap(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error in vxlan_roo_mc_mtu_rif_trap\n");
        return rv;
    }
    
    return rv;
}

