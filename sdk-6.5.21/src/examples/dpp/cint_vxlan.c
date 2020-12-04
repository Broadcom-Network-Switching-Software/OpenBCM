/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * Test Scenario: flooding, UC packets
 *
 * soc properties:
 *custom_feature_egress_independent_default_mode=1
 *bcm886xx_ip4_tunnel_termination_mode=0
 *bcm886xx_l2gre_enable=0
 *bcm886xx_vxlan_enable=1
 *bcm886xx_ether_ip_enable=0
 *bcm886xx_vxlan_tunnel_lookup_mode=2
 *bcm886xx_auxiliary_table_mode=1 (for arad/arad+ device only)
 *split_horizon_forwarding_groups_mode=1 (For Jericho device and forward)
 * 
 * how to run the test: 
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/dpp/utility/cint_utils_multicast.c
 * cint ../../src/examples/dpp/utility/cint_utils_vlan.c
 * cint ../../src/examples/dpp/utility/cint_utils_l3.c
 * cint ../../src/examples/dpp/cint_ip_route.c
 * cint ../../src/examples/dpp/cint_port_tpid.c
 * cint ../../src/examples/dpp/cint_advanced_vlan_translation_mode.c
 * cint ../../src/examples/dpp/cint_field_gre_learn_data_rebuild.c
 * cint ../../src/examples/dpp/cint_ip_tunnel.c
 * cint ../../src/examples/dpp/cint_ip_tunnel_term.c
 * cint ../../src/examples/dpp/cint_mact.c
 * cint ../../src/examples/dpp/cint_vswitch_metro_mp.c
 * cint ../../src/examples/dpp/cint_vxlan.c
 * cint
 * vxlan_example(0,200,201,-1);
 * exit;
 * 
 * 
 * Traffic: 
 * vxlan- unknown destination- packet from vxlan tunnel- flood 
 * Purpose: - check VXLAN tunnel termination
 *          - check split horizon filter
 *          - learn native SA
 * Packet flow:
 * - tunnel is terminated
 * - get VSI from VNI
 * - learn reverse FEC for native SA
 * - packet is bridged: forwarded according to DA and VSI
 * - no hit, do flooding
 * - get flooding MC group from VSI
 * - for MC group entry to access: AC outlif and port
 * - for MC group entry to core: tunnel outlif and port
 * -                             entry is filtered at split horizon filter at egress
 * 
 * tx 1 psrc=201 data=0x000c000200000000070001008100001408004500008a0000000040112f51a0000001ab000011555555550076239b0800000000138800000000000100000000000581000064ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f5051525354c2ba3efa
 * // Received packets on unit 0 should be: 
 * 0x000000000100000000000581810001fe000064ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f5051525354c2ba3efa 
 * 
 * 
 *  vxlan- unknown destination- packet from user - flood
 *  Purpose: - check VXLAN tunnel encapsulation
 * Packet flow:
 * - get VSI from port default VSI
 * - learn AC for SA
 * - packet is bridged: forwarded according to DA and VSI
 * - no hit, do flooding
 * - get flooding MC group from VSI
 * - for MC group entry to access: AC outlif and port
 * - for MC group entry to core: tunnel outlif and port
 *   - vxlan tunnel is built using IP tunnel eedb entry.     
 *   - vxlan header.vni is resovled from VSI -> vni
 *   - ethernet header is built using LL eedb entry (IP tunnel eedb entry point to LL eedb entry): 
 *   - DA from LL eedb entry, SA from LL eedb entry.VSI.myMac
 *   - VLAN = entry.VSI
 * 
 * tx 1 psrc=200 data=0x0000000000ab000007000123810001fe0899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 * // Received packets on unit 0 should be: 
 * packet1:
 *  0x0000000000ab000007000123810001fe0899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5 
 * packet2: 
 *  0x20000000cd1d000c00020000810000640800450b0064000000003211335caa000011ab000011500055550050000008000000001388000000000000ab0000070001230899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5 
 * 
 *  
 * vxlan- known learned destination- packet from user -
 * Purpose: - send packet using learned tunnel from previous flow 
 * Packet flow:
 * - get VSI from port default VSI
 * - packet is bridged: forwarded according to DA and VSI
 * - hit, result is FEC: tunnel outlif + port 
 *   - vxlan tunnel is built using IP tunnel eedb entry.     
 *   - vxlan header.vni is resovled from VSI -> vni
 *   - ethernet header is built using LL eedb entry (IP tunnel eedb entry point to LL eedb entry): 
 *   - DA from LL eedb entry, SA from LL eedb entry.VSI.myMac
 *   - VLAN = entry.VSI
 * 
 * tx 1 psrc=200 data=0x000000000581000012345678810001fe0899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 * // Received packets on unit 0 should be: 
 * 0x20000000cd1d000c00020000810000640800450b0064000000003211335caa000011ab000011500055550050000008000000001388000000000005810000123456780899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5 
 * 
 * 
 * vxlan- known learned destination- packet from vxlan tunnel- flood 
 * Purpose: - send packet using learned AC outlif from previous flow
 * 
 * tx 1 psrc=201 data=0x000c000200000000070001008100001408004500008a0000000040112f51a0000001ab0000115555555500761c780800000000138800000007000123000000000581000064ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f5051525354a53ba3e0
 * // Received packets on unit 0 should be: 
 * 0x000007000123000000000581810001fe000064ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f5051525354a53ba3e0
 * 
 * 
 * 
 * 
 * 
 *  Test Scenario: bud node
 * 
 * soc properties
 * bcm886xx_ip4_tunnel_termination_mode=0
 * bcm886xx_vxlan_enable=1
 * bcm886xx_ether_ip_enable=0
 * bcm886xx_vxlan_tunnel_lookup_mode=0  
 * bcm886xx_auxiliary_table_mode=1  (For Arad/Arad+ device only)
 * split_horizon_forwarding_groups_mode=1 (For Jericho device and forward)
 * tm_port_header_type_in_40=ETH
 * tm_port_header_type_out_40=ETH
 * ucode_port_40=RCY.0
 * ucode_port_41=RCY.1
 * ucode_port_42=RCY.2
 * default_logical_interface_ip_tunnel_overlay_mc=16484
 * custom_feature_egress_independent_default_mode=1
 * 
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/dpp/utility/cint_utils_vlan.c
 * cint ../../src/examples/dpp/utility/cint_utils_multicast.c
 * cint ../../src/examples/dpp/utility/cint_utils_l3.c
 * cint ../../src/examples/dpp/cint_port_tpid.c
 * cint ../../src/examples/dpp/cint_field_gre_learn_data_rebuild.c
 * cint ../../src/examples/dpp/cint_mact.c
 * cint ../../src/examples/dpp/cint_ip_route.c
 * cint ../../src/examples/dpp/cint_ip_tunnel.c
 * cint ../../src/examples/dpp/cint_ip_tunnel_term.c
 * cint ../../src/examples/dpp/cint_vswitch_metro_mp.c
 * cint ../../src/examples/dpp/cint_multi_device_utils.c
 * cint ../../src/examples/dpp/cint_vxlan.c
 * cint ../../src/examples/dpp/cint_ipmc_flows.c
 * cint ../../src/examples/dpp/cint_pmf_2pass_snoop.c
 * cint
 * vxlan_mc_bud_node_example(0,201,201,203,40);
 * exit;
 * 
 * Traffic
 * vxlan - bud node
 * Purpose: - check packet is forwarded in core according to multicast DIP (continue)
 *          - check packet is bridged in access 
 * Packet flow:
 * - get VSI from port default VSI
 * - DA is identified as compatible MC
 * - perform <RIF, DIP> lookup, result is MC group
 * - for MC entry to access (routing): RIF + port
 *   - for entry, fall to bridge (inRif == outRif)
 * - packet is snooped and sent to recycle port
 * - VXLAN tunnel is terminated
 * - packet is bridged
 * 
 * tx 1 psrc=201 data=0x01005e0505050000070001008100001408004500008a0000000040112f51aa000011e0050505555555550076239b08000000001388000000000000f00000000000018100000a0899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 * Received packets on unit 0 should be:
 * packet1: 
 * 0x01005e0505050000070001008100001408004500008a0000000040112f51aa000011e0050505555555550076239b08000000001388000000000000f00000000000018100000a0899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 * packet2: 
 * 0x0000000000f0000000000001810001fe0899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5 
 * 
 * 
 * 
 * 
 * Test Scenario: filter by network group using a 2b orientation (network_group_id)
 * soc properties:
 * split_horizon_forwarding_groups_mode=1 (or 2)
 * custom_feature_egress_independent_default_mode.0=1
 * bcm886xx_l2gre_enable.0=0
 * bcm886xx_vxlan_enable.0=1
 * bcm886xx_ip4_tunnel_termination_mode.0=0
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/dpp/utility/cint_utils_multicast.c
 * cint ../../src/examples/dpp/utility/cint_utils_vlan.c
 * cint ../../src/examples/dpp/utility/cint_utils_l3.c
 * cint ../../src/examples/dpp/cint_ip_route.c
 * cint ../../src/examples/dpp/cint_port_tpid.c
 * cint ../../src/examples/dpp/cint_advanced_vlan_translation_mode.c
 * cint ../../src/examples/dpp/cint_field_gre_learn_data_rebuild.c
 * cint ../../src/examples/dpp/cint_ip_tunnel.c
 * cint ../../src/examples/dpp/cint_ip_tunnel_term.c
 * cint ../../src/examples/dpp/cint_mact.c
 * cint ../../src/examples/dpp/cint_vswitch_metro_mp.c
 * cint ../../src/examples/dpp/cint_vxlan.c
 * cint
 * vxlan_orientation_filter_init(0);
 * vxlan_example(0,200,202,-1);
 * exit;
 *
 * filter vxlan port with 2b orientation   
 * tx 1 psrc=200 data=0x0000000000ab000007000123810001feffff
 *
 * Received packets on unit 0 should be: 
 * 0x0000000000ab000007000123810001feffff0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000 
 * 
 * 
 * 
 * 
 * Test scenario: VxLAN tunnel termination accroding to DIP, SIP, VRF using my-vtep-index
 * 
 * soc properties:
 *custom_feature_egress_independent_default_mode=1
 *bcm886xx_ip4_tunnel_termination_mode=0
 *bcm886xx_l2gre_enable=0
 *bcm886xx_vxlan_enable=1
 *bcm886xx_ether_ip_enable=0
 *bcm886xx_vxlan_tunnel_lookup_mode=3
 *bcm886xx_auxiliary_table_mode=1 (for arad/arad+ device only)
 *split_horizon_forwarding_groups_mode=1 (For Jericho device and forward) 
 *vxlan_tunnel_term_in_sem_vrf_nof_bits=3  (indicate nof bis for VRF in my-vtep-index, SIP, VRF lookup)
 *vxlan_tunnel_term_in_sem_my_vtep_index_nof_bits=12 (indicate nof bis for my-vte-index in my-vtep-index, SIP, VRF lookup)
 * Note: limitation: vxlan_tunnel_term_in_sem_vrf_nof_bits + vxlan_tunnel_term_in_sem_my_vtep_index_nof_bits <= 15
 * 
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/dpp/utility/cint_utils_multicast.c
 * cint ../../src/examples/dpp/utility/cint_utils_vlan.c
 * cint ../../src/examples/dpp/utility/cint_utils_l3.c
 * cint ../../src/examples/dpp/cint_ip_route.c
 * cint ../../src/examples/dpp/cint_port_tpid.c
 * cint ../../src/examples/dpp/cint_advanced_vlan_translation_mode.c
 * cint ../../src/examples/dpp/cint_field_gre_learn_data_rebuild.c
 * cint ../../src/examples/dpp/cint_ip_tunnel.c
 * cint ../../src/examples/dpp/cint_ip_tunnel_term.c
 * cint ../../src/examples/dpp/cint_mact.c
 * cint ../../src/examples/dpp/cint_vswitch_metro_mp.c
 * cint ../../src/examples/dpp/cint_vxlan.c
 * cint
 * vxlan_example(0,201,203,-1);
 * exit;
 *
 * Terminate VXLAN tunnel by DIP->my-vtep-index and my-vtep-index, SIP, VRF 
 * tx 1 psrc=203 data=0x000c0002000000000000058181000014080045000036000000008011e594aa000011ab000011c35155550022000008000000001388000000000000f020000000cd1d81000064ffff
 * 
 * Received packets on unit 0 should be: 
 * 0x0000000000f020000000cd1d810001feffff 
 *
 * Terminate VXLAN tunnel by DIP->my-vtep-index and my-vtep-index, SIP, VRF 
 * tx 1 psrc=203 data=0x000c00020000000000000581810000140800450000360000000080118ba001020304ab000011c35155550022000008000000001388000000000000f020000000cd1e81000064ffff
 *
 * Received packets on unit 0 should be: 
 * 0x0000000000f020000000cd1e810001feffff 
 */

/* ********* 
  Globals/Aux Variables
 */
/* debug prints */
int verbose = 1;


struct vxlan_s {
    int vpn_id;  
    int vni; 
    int vxlan_vdc_enable; /* Option to enable/disable VXLAN VDC support */
    int vxlan_network_group_id; /* ingress and egress orientation for VXLAN */
    int dip_sip_vrf_using_my_vtep_index_mode; /* configure DIP-> my-vtep-index, tunnel term: my-vtep-index, SIP, VRF and 
                                                 default tunnel term: DIP, SIP, VRF */
    int my_vtep_index; /* used in dip_sip_vrf_using_my_vtep_index_mode */
    bcm_if_t vxlan_fec_intf_id; /* fec for the VXLAN tunnel, set at vxlan_example */
    bcm_gport_t vxlan_port_id[2]; 

};

/*                 vpn_id | vni |       */
vxlan_s g_vxlan = {15,    5000, 
/*                 enable vdc support */
                   0, 
/*                 vxlan orientation */
                   1,
/*                 dip_sip_vrf_using_my_vtep_index_mode */
                   1, 
/*                 my_vtep_index */ 
                   0x2, 
/*                 fec for the VXLAN tunnel */
                   0,
/*                 vxlan_port_id */
                   {0,    0}

};

uint32 vdc_port_class = 10;

void vxlan_init(int unit, vxlan_s *param) {
    if (param != NULL) {
        sal_memcpy(&g_vxlan, param, sizeof(g_vxlan));
    }
    /*  Get soc property about VXLAN VDC support */
    if (soc_property_get(unit , "bcm886xx_vxlan_vpn_lookup_mode", 0)) {
        g_vxlan.vxlan_vdc_enable = 1;
    } else {
        g_vxlan.vxlan_vdc_enable = 0;
    }

    g_vxlan.dip_sip_vrf_using_my_vtep_index_mode = (soc_property_get(unit , "bcm886xx_vxlan_tunnel_lookup_mode",0) == 3);
}

void vxlan_struct_get(vxlan_s *param) {
    sal_memcpy( param, &g_vxlan, sizeof(g_vxlan));
}


/********** 
  functions
 */

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


bcm_mac_t vxlan_my_mac_get() {
    return ip_tunnel_my_mac_get();
}

/*
 * struct include meta information. 
 * where the cint logic pull data from this struct. 
 * use to control differ runs of the cint, without changing the cint code
 */

void ip_tunnel_glbl_init_vxlan(int unit, int flags){

    /* update tunnel info */
    ip_tunnel_glbl_info.tunnel_1_type = bcmTunnelTypeVxlan;
    ip_tunnel_glbl_info.tunnel_2_type = bcmTunnelTypeVxlan;
}

/* this configure the orientation on vxlan to be 2b. 
   Configure the orientation filters: 
   filter AC inLif (network group: 0) to VxLAN outLif (network group : 2)
   filter VxLAN inLif(network group : 2) to AC outLif(network group: 0) 
   */
int vxlan_orientation_filter_init(int unit) {
    int rv = BCM_E_NONE;

    /* set the AC network group */
    bcm_switch_network_group_t ac_network_group = 0; 
    /* set the orientation for vxlan to be 2b */
    g_vxlan.vxlan_network_group_id = 2;

    bcm_switch_network_group_config_t network_group_config; 
    bcm_switch_network_group_config_t_init(&network_group_config); 

    network_group_config.dest_network_group_id = g_vxlan.vxlan_network_group_id; 
    network_group_config.config_flags = BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE; /* drop the packet */

    /* set orientation filter: filter AC inLif (network group: 0) to VxLAN outLif (network group : 2) */
    rv = bcm_switch_network_group_config_set(unit, 
                                        ac_network_group,
                                        &network_group_config); 
	if (rv != BCM_E_NONE) {
		printf("Error, bcm_switch_network_group_config_set\n");
		return rv;
	}

    if (verbose >= 1) {
        printf("add orientation filter from AC (%d) to VXLAN (%d) \n", ac_network_group, g_vxlan.vxlan_network_group_id); 
    }

    bcm_switch_network_group_config_t_init(&network_group_config); 

    network_group_config.dest_network_group_id = ac_network_group;  /* AC network group */
    network_group_config.config_flags = BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE; /* drop the packet */

    /* set orientation filter: filter VxLAN inLif(network group : 2) to AC outLif(network group: 0) */
    rv = bcm_switch_network_group_config_set(unit, 
                                        g_vxlan.vxlan_network_group_id, 
                                        &network_group_config); 
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_network_group_config_set\n");
        return rv;
    }

    if (verbose >= 1) {
        printf("add orientation filter from VXLAN (%d) to AC (%d) \n", g_vxlan.vxlan_network_group_id, ac_network_group); 
    }



    return rv;
}

int vxlan_learn_sip_on_fec(int unit, uint32 sip, uint32 fec){
	int rv;
	bcm_tunnel_terminator_t sip_term;
	sip_term.sip_mask = 0xffffffff;
	sip_term.type = bcmTunnelTypeVxlan;
	sip_term.sip = sip;          
	sip_term.tunnel_id = fec;

	rv = bcm_tunnel_terminator_create(unit,&sip_term);
	if (rv != BCM_E_NONE) {
		printf("Error, bcm_tunnel_terminator_create\n");
		return rv;
	}

	if(verbose >= 1){
		printf("learn  0x%08x with FEC:%d\n\r",sip,fec);
	}
	return rv;
}




int vxlan_open_vpn(int unit, int vpn, int vni){
    int rv = BCM_E_NONE;
 
    egress_mc = 0;
    rv = multicast__open_mc_group(unit, &vpn, BCM_MULTICAST_TYPE_L2);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__open_mc_group\n");
        return rv;
    }
    if(verbose >= 1){
        printf("created multicast   0x%08x\n\r",vpn);
    }

    bcm_vxlan_vpn_config_t vpn_config;
    bcm_vxlan_vpn_config_t_init(&vpn_config);

    vpn_config.flags = BCM_VXLAN_VPN_ELAN|BCM_VXLAN_VPN_WITH_ID|BCM_VXLAN_VPN_WITH_VPNID;
    vpn_config.vpn = vpn;
    vpn_config.broadcast_group = vpn;
    vpn_config.unknown_unicast_group = vpn;
    vpn_config.unknown_multicast_group = vpn;
    vpn_config.vnid = vni;
    if (g_vxlan.vxlan_vdc_enable) {
        vpn_config.match_port_class = vdc_port_class;
    }

    rv = bcm_vxlan_vpn_create(unit,&vpn_config);
    if(rv != BCM_E_NONE) {
        printf("vxlan_open_vpn: error in bcm_vxlan_vpn_create \n");
        return rv;
    }
    if(verbose >= 1){
        printf("vxlan_open_vpn: created vpn   0x%08x\n\r",vpn);
    }

    return rv;
}



/* tunnel termination mode for VXLAN: DIP -> my-vtep-index
   Precondition: soc property bcm886xx_vxlan_tunnel_lookup_mode  == 3 */
int vxlan_tunnel_term_dip_to_my_vtep_index(int unit, int dip, int my_vtep_index) {

    int rv = BCM_E_NONE; 
    bcm_tunnel_terminator_config_key_t config_key; 
    bcm_tunnel_terminator_config_action_t config_action; 

    bcm_tunnel_terminator_config_key_t_init(&config_key); 
    bcm_tunnel_terminator_config_action_t_init(&config_action); 

    config_key.dip = dip; 

    config_action.tunnel_class = my_vtep_index; 

    /* config DIP to my-vtep-index */
    rv = bcm_tunnel_terminator_config_add(unit, 0, &config_key, &config_action); 
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_tunnel_terminator_config_add \n");
        return rv;
    }

    if(verbose >= 1){
        printf("add tunnel term configuration: DIP -> my_vtep_index: 0x%08x -> 0x%x \n\r",dip, my_vtep_index);
    }

    return rv; 
}

/* tunnel termination mode for VXLAN: my-vtep-index, SIP, VRF -> inLif
   Precondition: soc property bcm886xx_vxlan_tunnel_lookup_mode  == 3 */
int vxlan_tunnel_term_my_vtep_index_sip_vrf(int unit, int my_vtep_index, int sip, int vrf, int* tunnel_gport) {

    int rv = BCM_E_NONE; 

    bcm_tunnel_terminator_t tunnel_term;

    bcm_tunnel_terminator_t_init(&tunnel_term);    
    tunnel_term.tunnel_class = my_vtep_index; 
    tunnel_term.flags |= BCM_TUNNEL_TERM_USE_TUNNEL_CLASS; /* indicate tunnel_class is a valid value */
    tunnel_term.sip = sip; 
    tunnel_term.sip_mask = 0xFFFFFFFF; /* lookup is in ISEM, don't support subnets, SIP must be fully masked */
    tunnel_term.vrf = vrf; 
    tunnel_term.type = bcmTunnelTypeVxlan; 
    tunnel_term.tunnel_if = BCM_IF_INVALID; /* no rif */

    if (tunnel_gport != 0) {
        tunnel_term.tunnel_id = *tunnel_gport; 
        tunnel_term.flags |= BCM_TUNNEL_TERM_TUNNEL_WITH_ID; 
    }

    rv = bcm_tunnel_terminator_create(unit,&tunnel_term);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_terminator_create, \n");
    }

    if(verbose >= 1){
        printf("add tunnel term: my_vtep_index, SIP, VRF -> inLif: 0x%x, 0x%08x, 0x%x -> 0x%x \n\r",my_vtep_index, sip, vrf, tunnel_term.tunnel_id);
    }

    *tunnel_gport = tunnel_term.tunnel_id; 

    return rv; 
}

/* tunnel termination mode for VXLAN: DIP SIP VRF -> inLif
   Precondition: soc property bcm886xx_vxlan_tunnel_lookup_mode  == 3 */
int vxlan_tunnel_term_dip_sip_vrf(int unit, int dip, int dip_mask, int sip, int sip_mask, int vrf, int* tunnel_gport) {

    int rv = BCM_E_NONE; 

    bcm_tunnel_terminator_t tunnel_term;

    bcm_tunnel_terminator_t_init(&tunnel_term);    
    tunnel_term.dip = dip;  
    tunnel_term.dip_mask = dip_mask;  

    tunnel_term.sip = sip;  
    tunnel_term.sip_mask = sip_mask; 
    tunnel_term.vrf = vrf; 
    tunnel_term.type = bcmTunnelTypeVxlan; 
    tunnel_term.tunnel_if = BCM_IF_INVALID; /* no rif */

    rv = bcm_tunnel_terminator_create(unit,&tunnel_term);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_terminator_create, \n");
    }


    if(verbose >= 1){
        printf("add tunnel term: DIP, SIP, VRF -> inLif: 0x%08x, 0x%08x, 0x%x -> 0x%x \n\r",dip, sip, vrf, tunnel_term.tunnel_id);
    }

    *tunnel_gport = tunnel_term.tunnel_id; 

    return rv; 
}



/*
 * add access, 
 */
int
vxlan_vlan_port_add(int unit,  bcm_gport_t in_port, bcm_gport_t *port_id){
    int rv;
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);
    
    /* the match criteria is port:1, out-vlan:510   */
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = in_port;
    vlan_port.match_vlan = 510;
    /* when packet forwarded to this port then it will be set with out-vlan:100,  in-vlan:200 */
    vlan_port.egress_vlan = 510;
    vlan_port.flags = 0;
    /* this is relevant only when get the gport, not relevant for creation */
    /* will be pupulated when the gport is added to service, using vswitch_port_add */
    vlan_port.vsi = 0;
    rv = bcm_vlan_port_create(unit,&vlan_port);
    
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }

    bcm_vlan_action_set_t action;
    /* In advanced vlan translation mode, bcm_vlan_port_create does not create ingress / egress
       action mapping. This is here to compensate. */
    if (advanced_vlan_translation_mode)
    {
        /* In advanced vlan translation mode, the default ingress/ egress actions and mapping
           are not configured. This is here to compensate. */
        rv = vlan_translation_default_mode_init(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan_translation_default_mode_init\n");
            return rv;
        }

        rv = vlan_translation_vlan_port_create_to_translation(unit, &vlan_port);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan_translation_vlan_port_create_to_translation\n");
            return rv;
        }

        bcm_vlan_port_translation_t port_trans;
        bcm_vlan_translate_action_class_t action_class;
        int ing_vlan_action_id;
        int ing_tag_format         = 2;
        int ing_vlan_edit_class_id = 4;

        /* Map LIF -> vlan_edit_class_id */
        bcm_vlan_port_translation_t_init(&port_trans);
        port_trans.gport              = vlan_port.vlan_port_id;
        port_trans.vlan_edit_class_id = ing_vlan_edit_class_id;
        port_trans.flags              = BCM_VLAN_ACTION_SET_INGRESS;
        rv = bcm_vlan_port_translation_set(unit, &port_trans);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_translation_set ing $rv\n");
            return rv;
        }

        /* Create ingress vlan action id: ing_vlan_action_id */
        rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_INGRESS, &ing_vlan_action_id);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_create ing $rv\n");
            return rv;
        }

        /* Map <vlan_edit_class_id,  tag_format_class_id> -> ing_vlan_action_id */
        bcm_vlan_translate_action_class_t_init(&action_class);
        action_class.vlan_edit_class_id         = ing_vlan_edit_class_id;
        action_class.tag_format_class_id        = ing_tag_format;
        action_class.vlan_translation_action_id = ing_vlan_action_id;
        action_class.flags                      = BCM_VLAN_ACTION_SET_INGRESS;
        rv = bcm_vlan_translate_action_class_set( unit,  &action_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_class_set ing $rv\n");
            return rv;
        }

        /* Map ing_vlan_action_id to action */
        bcm_vlan_action_set_t_init(&action);
        action.dt_outer   = bcmVlanActionNone;
        action.dt_inner   = bcmVlanActionNone;
        action.outer_tpid = 0x8100;
        action.inner_tpid = 0x8100;
        rv = bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_INGRESS, ing_vlan_action_id, &action);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_set ing $rv\n");
            return rv;
        }
    }
    else
    {
        bcm_vlan_action_set_t_init(&action);
        action.ot_outer = bcmVlanActionNone;
        action.ot_inner = bcmVlanActionNone;
        rv = bcm_vlan_translate_action_create(unit, vlan_port.vlan_port_id, bcmVlanTranslateKeyPortOuter, BCM_VLAN_INVALID, BCM_VLAN_NONE, &action);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_translate_action_create $rv\n");
            return rv;
        }
    }

    if(verbose >= 2) {
        printf("Add vlan-port-id:0x%08x in-port:0x%08x match_vlan:0x%08x match_inner_vlan:0x%08x\n\r",vlan_port.vlan_port_id, vlan_port.port, vlan_port.match_vlan, vlan_port.match_inner_vlan);
    }

    /* handle of the created gport */
    *port_id = vlan_port.vlan_port_id;
    
    return rv;
}


/******* Run example ******/
 
 
/*
 * IP tunnel example 
 * - build IP tunnels. 
 * - add ip routes/host points to the tunnels
 */
int vxlan_example(int unit, int vm_port, int core_port, int vpn_id){

    uint32 vpn;

    /* init vxlan global */
    vxlan_s vxlan_param; 
    vxlan_struct_get(&vxlan_param);
    if (vpn_id >=0) {
        vxlan_param.vpn_id = vpn_id;
    }
    vxlan_init(unit, vxlan_param);
    vpn = g_vxlan.vpn_id;


    printf("Parameters \n");
    printf("inPort: %d \n", vm_port);
    printf("outPort: %d \n", core_port);
    printf("vpn: %d \n", vpn_id);


    
    bcm_gport_t in_tunnel_gports[2];
    bcm_gport_t out_tunnel_gports[2];
    bcm_if_t out_tunnel_intf_ids[2];/* out tunnels interfaces
                                  out_tunnel_intf_ids[0] : is tunnel-interface-id
                                  out_tunnel_intf_ids[1] : is egress-object (FEC) points to tunnel
                              */
    int eg_intf_ids[2];/* interface for routing, not used */
    bcm_gport_t vlan_port_id;
    bcm_mac_t mac_address  = {0x00, 0x00, 0x00, 0x00, 0x00, 0xf0};
	uint32 tunnel_sip = 0x0a050001;
	uint32 sip_learned_fec = 0x1234;
    int vxlan_protocol_type = 0xfeef;
    int rv;
    int vxlan_port_flags = 0;
    int ll_encap_id;
    
    verbose = 10;

    bcm_ip_t native_dip = 0xABA1A1A1; /* ip address: 171:17:17:17 */ 

    /* init ip tunnel info, to fit VXLAN usage */
    ip_tunnel_glbl_init_vxlan(unit,0);

    if (g_vxlan.vxlan_vdc_enable == 1) {
        /* set port vlan domain */
        rv = bcm_port_class_set(unit, core_port, bcmPortClassId, vdc_port_class);
        if (rv != BCM_E_NONE) {
          printf("Error, bcm_port_class_set unit %d, port %d rv %d\n", unit, core_port, rv);
          return rv;
        }
         rv = bcm_port_class_set(unit, vm_port, bcmPortClassId, vdc_port_class);
        if (rv != BCM_E_NONE) {
          printf("Error, bcm_port_class_set unit %d, port %d rv %d\n", unit, vm_port, rv);
          return rv;
        }
    }

    /* init L2 VXLAN module */
    rv = bcm_vxlan_init(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set \n");
    }

	/* learn SIP lookup result as FEC  */
	gre_learn_data_rebuild_example(unit); 

	rv = vxlan_learn_sip_on_fec(unit,tunnel_sip,sip_learned_fec);
	if (rv != BCM_E_NONE) {
		printf("Error, vxlan_learn_sip_on_fec\n");
		return rv;
	}

    /* set trap for bounce back filter to drop */
    set_trap_to_drop(unit, bcmRxTrapEgTrillBounceBack);

    /* build L2 VPN */

    rv = vxlan_open_vpn(unit,vpn,g_vxlan.vni);
    if (rv != BCM_E_NONE) {
        printf("Error, vxlan_open_vpn, vpn=%d, \n", vpn);
    }

    if(verbose >= 2){
        printf("Open tunnels: \n\r");
    }
    /*** build tunnel initiators ***/
    rv = ipv4_tunnel_build_tunnels(unit, core_port, out_tunnel_intf_ids,out_tunnel_gports, &ll_encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, ipv4_tunnel_add_routes, in_port=%d, \n", vm_port);
    }
    if(verbose >= 2){
        printf("Open out-tunnel 1 gport-id: 0x%08x (No FEC)\n\r", out_tunnel_gports[0]);
        printf("Open out-tunnel 2 gport-id: 0x%08x FEC-id:0x%08x\n\r", out_tunnel_gports[1],out_tunnel_intf_ids[1]);
    }

    if(verbose >= 2){
        printf("Open tunnels tunnel terminators: \n\r");
    }

    /* save FEC intf id in global var so it can be reused later (for bud node) */
    g_vxlan.vxlan_fec_intf_id = out_tunnel_intf_ids[1]; 

    /* tunnel termination inLif configuration*/

    if (g_vxlan.dip_sip_vrf_using_my_vtep_index_mode) {
        /* create router interface */
        bcm_if_t tunnel_rif, egress_intf1, egress_intf2, ll_encap_id; /* not used */
        ip_tunnel_term_open_route_interfaces(unit, vm_port, core_port, ip_tunnel_term_glbl_info.rif_vrf, 
                                             ip_tunnel_term_glbl_info.tunnel_vrf, 
                                             &tunnel_rif,  &egress_intf1, &egress_intf2, &ll_encap_id); 

        /* configure DIP -> my-vtep-index */
        rv = vxlan_tunnel_term_dip_to_my_vtep_index(unit, 
                                                    ip_tunnel_term_glbl_info.dip2, /* 0xAB000011 */   /* default 171:0:0:17 */
                                                    g_vxlan.my_vtep_index); /* default 0 */
        if (rv != BCM_E_NONE) {
            printf("Error, in vxlan_tunnel_term_dip_to_my_vtep_index \n");
            return rv; 
        }

        
        /* tunnel termination: create tunnel inLif and configure my-vtep-index, SIP, VRF to inLif */
        rv = vxlan_tunnel_term_my_vtep_index_sip_vrf(unit, 
                                                     g_vxlan.my_vtep_index, /* default 0 */
                                                     ip_tunnel_term_glbl_info.sip2, /* default: 170.0.0.17 */
                                                     ip_tunnel_term_glbl_info.rif_vrf, /* default: 2 */
                                                     &(in_tunnel_gports[0])
                                                     ); 
        if (rv != BCM_E_NONE) {
            printf("Error, in vxlan_tunnel_term_my_vtep_index_sip_vrf \n");
            return rv; 
        }

        /* create VxLAN gport using tunnel inlif */
        vxlan_port_add_s vxlan_port_add; 
        vxlan_port_s_clear(&vxlan_port_add); 
        vxlan_port_add.vpn = vpn; 
        vxlan_port_add.in_port = vm_port; 
        vxlan_port_add.in_tunnel = in_tunnel_gports[0]; 
        vxlan_port_add.out_tunnel = out_tunnel_gports[1]; 
        vxlan_port_add.egress_if = out_tunnel_intf_ids[1]; 
        vxlan_port_add.flags = vxlan_port_flags; 

        rv = vxlan__vxlan_port_add(unit, vxlan_port_add); 
        if (rv != BCM_E_NONE) {
            printf("Error, vxlan__vxlan_port_add 2, in_gport=0x%08x --> out_intf=0x%08x \n", in_tunnel_gports[0],out_tunnel_intf_ids[1]);
            return rv; 
        }

        g_vxlan.vxlan_port_id[0] = vxlan_port_add.vxlan_port_id; 

        if (verbose >=2 ) {
            printf("vxlan port created, vxlan_port_id: 0x%08x \n", g_vxlan.vxlan_port_id[0]); 
        }


        /* default tunnel termination: create tunnel inLif and configure DIP, SIP, VRF
           Because it's a default tunnel termination, we configure DIP, VRF (SIP is not masked) */
        rv = vxlan_tunnel_term_dip_sip_vrf(unit, 
                                           ip_tunnel_term_glbl_info.dip2, /* 0xA1000011 */   /* default 161.0.0.17 */
                                           0xFFFFFFFF, /* fully masked */
                                           ip_tunnel_term_glbl_info.sip2, /* default: 160.0.0.17*/
                                           0x00000000, /* not masked */
                                           ip_tunnel_term_glbl_info.rif_vrf, /* default: 2 */
                                           &(in_tunnel_gports[1])
                                           ); 
        if (rv != BCM_E_NONE) {
            printf("Error, in vxlan_tunnel_term_dip_sip_vrf \n");
            return rv; 
        }

        /* create VxLAN gport using tunnel inlif, tunnel outlif and FEC.
           Note: this is default tunnel termination, we use the same tunnel outlif than previous vxlan port */

        /* create VxLAN gport using tunnel inlif */
        vxlan_port_s_clear(&vxlan_port_add); 
        vxlan_port_add.vpn = vpn; 
        vxlan_port_add.in_port = vm_port; 
        vxlan_port_add.in_tunnel = in_tunnel_gports[1]; 
        vxlan_port_add.out_tunnel = out_tunnel_gports[1]; 
        vxlan_port_add.egress_if = out_tunnel_intf_ids[1]; 
        vxlan_port_add.flags = vxlan_port_flags; 
        vxlan_port_add.network_group_id = g_vxlan.vxlan_network_group_id; 

        rv = vxlan__vxlan_port_add(unit, vxlan_port_add); 
        if (rv != BCM_E_NONE) {
            printf("Error, vxlan__vxlan_port_add 2, in_gport=0x%08x --> out_intf=0x%08x \n", in_tunnel_gports[0],out_tunnel_intf_ids[1]);
            return rv; 
        }

        g_vxlan.vxlan_port_id[1] = vxlan_port_add.vxlan_port_id; 




    } else {
        /*** build tunnel terminations and router interfaces ***/
        rv = ipv4_tunnel_term_build_tunnel_terms(unit,vm_port,core_port,eg_intf_ids,in_tunnel_gports); 
        if (rv != BCM_E_NONE) {
            printf("Error, cip_tunnel_term_open_route_interfaces, in_port=%d, \n", vm_port);
            return rv; 
        }
        if(verbose >= 2){
            printf("Open in-tunnel 1 gport-id: 0x%08x (No FEC)\n\r", in_tunnel_gports[0]);
            printf("Open in-tunnel 2 gport-id: 0x%08x FEC-id\n\r", in_tunnel_gports[1]);
        }

        /* build l2 vxlan ports */

        /* add vxlan port identified with in-tunnel and egress with tunnel id 
        rv = vxlan_add_port(unit,vpn,in_port,in_tunnel_gports[0],out_tunnel_gports[0],0, &vxlan_port_id[0]);
        if (rv != BCM_E_NONE) {
            printf("Error, vxlan_add_port, in_gport=0x%08x --> out_gport=0x%08x \n", in_tunnel_gports[0],out_tunnel_gports[0]);
        }*/

        /* add vxlan port identified with in-tunnel and egress with egress-object id */
        vxlan_port_add_s vxlan_port_add; 
        vxlan_port_s_clear(&vxlan_port_add); 
        vxlan_port_add.vpn = vpn; 
        vxlan_port_add.in_port = core_port; 
        vxlan_port_add.in_tunnel = in_tunnel_gports[1]; 
        vxlan_port_add.out_tunnel = out_tunnel_gports[1]; 
        vxlan_port_add.egress_if = out_tunnel_intf_ids[1]; 
        vxlan_port_add.flags = vxlan_port_flags; 
        vxlan_port_add.network_group_id = g_vxlan.vxlan_network_group_id; 

        rv = vxlan__vxlan_port_add(unit, vxlan_port_add); 
        if (rv != BCM_E_NONE) {
            printf("Error, vxlan__vxlan_port_add 2, in_gport=0x%08x --> out_intf=0x%08x \n", in_tunnel_gports[0],out_tunnel_intf_ids[1]);
            return rv; 
        }

        g_vxlan.vxlan_port_id[1] = vxlan_port_add.vxlan_port_id; 
    }



    /* port vlan port*/

    /* init vlan port glbl info */
    vswitch_metro_mp_info_init(core_port,0,0);

    port_tpid_init(vm_port, 1, 1);
    rv = port_tpid_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, port_tpid_set with port_1\n");
        print rv;
        return rv;
    }

    port_tpid_init(core_port, 1, 1);
    rv = port_tpid_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, port_tpid_set with port_1\n");
        print rv;
        return rv;
    }

    /* In advanced vlan translation mode, the default ingress/ egress actions and mapping
       are not configured. This is here to compensate. */
    if (advanced_vlan_translation_mode) {
        rv = vlan_translation_default_mode_init(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan_translation_default_mode_init\n");
            print rv;
            return rv;
        }
    }


    /* enable learning for port core_port */
    bcm_gport_t vlan_port_id_dummy;                                                            
    rv = vxlan_vlan_port_add(unit, core_port, &vlan_port_id_dummy);                           
    if (rv != BCM_E_NONE) {                                                           
        printf("Error, vswitch_metro_add_port_1 2, in_intf=0x%08x \n", vlan_port_id); 
    }                                                                                 

    
    rv = vxlan_vlan_port_add(unit, vm_port, &vlan_port_id);                           
    if (rv != BCM_E_NONE) {                                                           
        printf("Error, vswitch_metro_add_port_1 2, in_intf=0x%08x \n", vlan_port_id); 
    }                                                                                                       
                                                                                       
     rv = vswitch_add_port(unit, vpn,vm_port, vlan_port_id);                         
    if (rv != BCM_E_NONE) {                                                           
        printf("Error, vswitch_add_port\n");                                          
        return rv;                                                                    
    }                                                                                 
                                                                                      
  
    /* add mact entries point to created gports */
    rv = l2_addr_add(unit,mac_address,vpn,vlan_port_id); 
    if (rv != BCM_E_NONE) {                              
        printf("Error, l2_addr_add to vlan port \n");    
        return rv;                                       
    }                                                    
    mac_inc(mac_address);

    rv = l2_addr_add(unit,mac_address,vpn,g_vxlan.vxlan_port_id[0]);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_addr_add to g_vxlan.vxlan_port_id[0]\n");
        return rv;
    }
    mac_inc(mac_address);

    rv = l2_addr_add(unit,mac_address,vpn,g_vxlan.vxlan_port_id[1]);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_addr_add to g_vxlan.vxlan_port_id[1]\n");
        return rv;
    }
   
    /* adding a registered MC using flood MC */  
    bcm_mac_t mac = {0x01,0x00,0x5E,0x01,0x01,0x14};
    bcm_l2_addr_t l2addr;
    bcm_l2_addr_t_init(&l2addr, mac, vpn);

    l2addr.mac = mac;
    l2addr.l2mc_group = vpn;
    l2addr.vid = vpn;
    l2addr.flags = BCM_L2_STATIC|BCM_L2_MCAST;

    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add\n");
        print rv;
        return rv;
    } 
 
    return rv;
}


int vxlan_port_get(
    int            unit,
    bcm_vpn_t      l2vpn,
    bcm_gport_t    vxlan_gport_id)
{
    int rv;
    

    bcm_vxlan_port_t vxlan_port;

    vxlan_port.vxlan_port_id  = vxlan_gport_id;

    rv = bcm_vxlan_port_get(
            unit, 
            l2vpn, 
            &vxlan_port);

    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vxlan_port_get\n");
        return rv;
    }

    print vxlan_port ;

    return rv;
}


/* Add a IPMC route rule, so Some copies are forwarded to out_port. */
int vxlan_ipv4_mc_route(int unit, int vlan, int* ipmc_index)
{
    bcm_error_t rv = BCM_E_NONE;
    int vrf = 0;
    bcm_l3_intf_t intf1;

    rv = create_rif(mac_address_1, unit, vlan, &intf1, vrf);
    if (rv != BCM_E_NONE) {
        printf("Error, create_rif $rv\n");
        return rv;
    }

    rv = enable_rif_ipmc(unit,&intf1, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, enable_rif_ipmc, enable == 0 $rv\n");
        return rv;
    }

    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID, ipmc_index);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create $rv\n");
        return rv;
    }

    return rv;
}

/*-----------------------------------------------------------------------------------------
 * Build qualifier set for the group we want to create.  Entries must exist
 * within groups, and groups define what kind of things the entries can
 * match, by their inclusion in the group's qualitier set.
 *
 * The Field group is done at the ingress stage
 * The Field group qualifiers are EtherType (to match only IPv4 packets),
 * Build the action set for this Field group.  Entries can only do
 * something that is included on this list.
 *
 * This Field Group is used to snoop matched packet to recycle port
*/
int vxlan_mc_field_group_set( int unit, int in_port, bcm_gport_t snoop_trap_gport_id, bcm_field_group_t *group)
{
    int result;
    int group_priority = BCM_FIELD_GROUP_PRIO_ANY;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_group_t grp;
    bcm_field_entry_t ent;
    uint8 data, mask;

    printf("\t===============  PMF rule set   ==================\n");
    /* define the qualifier */
    BCM_FIELD_QSET_INIT(&qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIpTunnelHit);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyForwardingType);

    /* define the action */
    BCM_FIELD_ASET_INIT(&aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionSnoop);

    result = bcm_field_group_create(unit, qset, group_priority, &grp);
    if ( result != BCM_E_NONE) {
      printf("Error in bcm_field_group_create $rv\n");
      return result;
    }

    /*  Attach the action set */
    result = bcm_field_group_action_set(unit, grp, aset);
    if (result != BCM_E_NONE) {
        printf("Error in bcm_field_group_action_set $rv\n");
        return result;
    }

    /*
     * Add multiple entries to the Field group.
     * Match the packets with IP tunnel lookup hit and
     * forward type is IPv4 Multicast Routing forwarding
     */
    result = bcm_field_entry_create(unit, grp, &ent);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create $rv\n");
        return result;
    }

    result = bcm_field_qualify_InPort(unit, ent, in_port, 0xffffffff);
    if (result != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_InPort $rv\n");
        return result;
    }

    /* VNI->VSI lookup is hit */
    data = 1;
    mask = 1;
    result = bcm_field_qualify_IpTunnelHit(unit, ent, data, mask);
    if (result != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_IpTunnelHit $rv\n");
        return result;
    }

    result = bcm_field_qualify_ForwardingType(unit, ent, bcmFieldForwardingTypeIp4Mcast);
    if (result != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_ForwardingType $rv\n");
        return result;
    }

    /* Get the trap gport to snoop, A matched frame is snooped to the defined gport. */
    result = bcm_field_action_add(unit, ent, bcmFieldActionSnoop, snoop_trap_gport_id, 0);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_action_add $rv\n");
        return result;
    }

    /*
    *  Commit the entry group to hardware.  We could do individual entries,
    *  but since we just created the group here and want the entry/entries all
    *  in hardware at this point, it is quicker to commit the whole group.
    */
    result = bcm_field_group_install(unit, grp);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_install $rv\n");
        return result;
    }

    *group = grp;

    return result;
}

/********************
 * snoop matched vxlan MC packet to recycle port
 *   1. create snoop command
 *   2. create a FG to qualify vxlan MC packet local to the node
 */
int vxlan_mc_snoop2recycle_port(int unit, int in_port, int recycle_port)
{
    int rv = BCM_E_NONE;
    bcm_gport_t gport;
    bcm_field_group_t field;

    rv = two_pass_snoop_set(unit, bcmRxTrapUserDefine, recycle_port, &gport);
    if (rv != BCM_E_NONE) {
        printf("Error, two_pass_snoop_set\n");
        return rv;
    }

    rv = vxlan_mc_field_group_set(unit, in_port, gport, &field);
    if (rv != BCM_E_NONE) {
        printf("Error, vxlan_mc_field_group_set\n");
        return rv;
    }

    rv = bcm_port_control_set(unit,recycle_port, bcmPortControlOverlayRecycle, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set\n");
        return rv;
    }

    return rv;
}

/****************************
 * VXLAN MC bud node example.
 *   Bud node some copies are IP-forwarded, some copies are IP-terminated;
 *   and L2-bridged according to the exposed DA.
 *
 * - build IPMC rule for IP-forwarded
 * - build IP tunnels.
 * - add ip routes/host points to the tunnels
 * - create a FG to snoop matched vxlan MC to recycle port.
 * - build learn information
 * - create a FG to rebuild the Learn-Data according to the FEC-Pointer
 */
int vxlan_mc_bud_node_example(int unit, int in_port, int de_encap_out_port, int route_out_port, int recycle_port)
{
    int rv = BCM_E_NONE;
    int vlan = 20;
    int ipmc_index = 5000;
    int port_member_of_mc[1] = {route_out_port};
    bcm_ip_t mc_ip = 0xe0050505;
    bcm_ip_t src_ip = 0;
    bcm_gport_t vxlan_port_id;
    bcm_tunnel_terminator_t tunnel_term;
    int ip_tunnel_overlay_mc_lif = 0;
    bcm_gport_t ip_tunnel_overlay_mc_tunnel = 0;
    bcm_gport_t out_tunnel_id = 0;
    int vpn = g_vxlan.vpn_id;

    /* add port to vlan */
    bcm_pbmp_t pbmp;
    bcm_pbmp_t ubmp;
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_PORT_ADD(pbmp, in_port);
    rv = bcm_vlan_port_add (unit, vlan, pbmp, ubmp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_add $rv\n");
        return rv;
    }

    rv = vxlan_ipv4_mc_route(unit, vlan, &ipmc_index);
    if (rv != BCM_E_NONE) {
        printf("Error, vxlan_ipv4_mc_route $rv\n");
        return rv;
    }

    /* add ports to mc group */
    rv = add_ingress_multicast_forwarding(unit,ipmc_index, port_member_of_mc, 1, vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, add_ingress_multicast_forwarding $rv\n");
        return rv;
    }

    /* add routing table entry to mc group */
    rv = create_forwarding_entry_dip_sip(unit, mc_ip, src_ip, ipmc_index, vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, create_forwarding_entry_dip_sip $rv\n");
        return rv;
    }

    rv = vxlan_example(unit, in_port, de_encap_out_port, vpn);
    if (rv != BCM_E_NONE) {
        printf("Error, vxlan_example $rv\n");
        return rv;
    }

    /* If lookup{DIP} result is an IP-LIF, then the vxlan header will be terminated.
     * In case of bud node and the traffic from recycle port, we always want to terminate
     * vxlan header and forward with bridge, so we add an IP-LIF-dummy when bcm.user start.
     * and here get the LIF-ID of IP-lIF-dummy and add it to VPN.
     */
    ip_tunnel_overlay_mc_lif = soc_property_suffix_num_get(unit, -1, "default_logical_interface_ip_tunnel_overlay_mc", "vxlan", 0);
    BCM_GPORT_TUNNEL_ID_SET(ip_tunnel_overlay_mc_tunnel, ip_tunnel_overlay_mc_lif);



    vxlan_port_add_s vxlan_port_add; 
    vxlan_port_s_clear(&vxlan_port_add); 
    vxlan_port_add.vpn = vpn; 
    vxlan_port_add.in_port = recycle_port; 
    vxlan_port_add.in_tunnel = ip_tunnel_overlay_mc_tunnel; 
    vxlan_port_add.out_tunnel = 0; 
    vxlan_port_add.egress_if = g_vxlan.vxlan_fec_intf_id; 
    vxlan_port_add.flags = 0; 
    vxlan_port_add.network_group_id = g_vxlan.vxlan_network_group_id; 

    rv = vxlan__vxlan_port_add(unit, vxlan_port_add); 
    if (rv != BCM_E_NONE) {
        printf("Error, vxlan__vxlan_port_add $rv\n");
        return rv;
    }

    rv = vxlan_mc_snoop2recycle_port(unit, in_port, recycle_port);
    if (rv != BCM_E_NONE) {
        printf("Error, vxlan_mc_snoop2recycle_port $rv\n");
        return rv;
    }

    /* SIP termination for vxlan:
     * the taget of this termination is mapping SIP to Learning info
     */
    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.sip = ip_tunnel_term_glbl_info.sip2;
    tunnel_term.sip_mask = 0xffffffff;
    tunnel_term.type = bcmTunnelTypeVxlan;
    tunnel_term.tunnel_id = BCM_GPORT_TUNNEL_ID_GET(out_tunnel_id);
    printf("tunnel id %d\n", tunnel_term.tunnel_id); 
/*  rv = bcm_tunnel_terminator_create(unit,&tunnel_term);*/
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_terminator_create $rv\n");
        return rv;
    }

    /* Rebuild the Learn-Data according to the FEC-Pointer */
/*  rv = gre_learn_data_rebuild_example(unit);*/
    if (rv != BCM_E_NONE) {
        printf("Error, gre_learn_data_rebuild_example $rv\n");
        return rv;
    }

    return rv;
}



int 
vxlan_example_get_remove(int unit, int delete_all, int delete, int vpn_id){
    int rv;
    bcm_vxlan_port_t port_array[3];
    int port1 = 0x7c001000;
    int indx;
    int port2 = 0x7c001001;
    int vpn;
    int port_count;

    vxlan_s vxlan_param; 
    vxlan_struct_get(&vxlan_param);
    if (vpn_id >=0) {
        vxlan_param.vpn_id = vpn_id;
    }
    vxlan_init(unit, vxlan_param);

    vpn = g_vxlan.vpn_id;

    printf("get port1 =0x%08x, \n", port1);
    vxlan_port_get(unit, vpn, port1);
    printf("get port2 =0x%08x, \n", port2);
    vxlan_port_get(unit, vpn, port2);


    /*   get all */
    rv = bcm_vxlan_port_get_all(
        unit, 
        vpn, 
        3, 
        port_array, 
        &port_count);
    printf("get all: num-of-ports =%d, \n", port_count);
    for(indx = 0; indx < port_count; ++indx) {
        print port_array[indx];
    }

    if (port_count != 2) {
        printf("In this CINT script, should have 2 ports\n");
        return BCM_E_UNAVAIL;
    }

    port1 = port_array[0].vxlan_port_id;
    port2 = port_array[1].vxlan_port_id;

    if(delete_all) {

        printf("delete all:\n");
        rv = bcm_vxlan_port_delete_all(unit, vpn);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vxlan_port_delete_all\n");
            return rv;
        }
    }
    else if(delete){
        printf("delete port1 =0x%08x, \n", port1);
        rv = bcm_vxlan_port_delete(unit, vpn, port1);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_petra_vxlan_port_delete\n");
            return rv;
        }

        printf("delete port2 =0x%08x, \n", port2);
        rv = bcm_vxlan_port_delete(unit, vpn, port2);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_petra_vxlan_port_delete\n");
            return rv;
        }
    }
    printf("get port1 =0x%08x, \n", port1);
    vxlan_port_get(unit, vpn, port1);
    printf("get port2 =0x%08x, \n", port2);
    vxlan_port_get(unit, vpn, port2);

    return rv;
}
