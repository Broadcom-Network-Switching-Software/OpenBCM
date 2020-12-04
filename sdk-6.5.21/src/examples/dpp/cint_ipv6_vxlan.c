/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * Test Scenario: flooding, UC packets
 *
 * soc properties:
 * parser_mode=1
 * bcm886xx_vxlan_enable=1
 * custom_feature_egress_independent_default_mode=1
 * bcm886xx_ipv6_tunnel_enable=1
 * bcm886xx_ip6_tunnel_encapsulation_mode=2
 * bcm886xx_ip6_tunnel_termination_mode=2
 * ip6_tunnel_term_in_tcam_vrf_nof_bits=11
 * custom_feature_mpls_cw_disable=1
 * custom_feature_ethernet_header_editing_size=0x00003183,0x18318318
 * 
 * how to run the test: 
 * cd ../../../../src/examples/dpp
 * cint ../sand/utility/cint_sand_utils_global.c
 * cint utility/cint_utils_multicast.c 
 * cint utility/cint_utils_l3.c 
 * cint utility/cint_utils_vlan.c 
 * cint cint_ip_route.c 
 * cint cint_ip_tunnel_term.c 
 * cint cint_ipv6_fap.c 
 * cint cint_ipv6_tunnel.c
 * cint cint_ipv6_tunnel_term.c
 * cint cint_port_tpid.c
 * cint cint_advanced_vlan_translation_mode.c
 * cint cint_field_gre_learn_data_rebuild.c
 * cint cint_ip_tunnel.c
 * cint cint_ip_tunnel_term.c 
 * cint cint_mact.c
 * cint cint_vswitch_metro_mp.c
 * cint utility/cint_utils_vxlan.c
 * cint cint_ipv6_vxlan.c
 * cint
 * print vxlan6_example(0, 201, 202, -1, 0);
 * exit;
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
 * Transmitted packets:
 * tx 1 psrc=202 data=0x000c000200000000070001008100001486dd60000000006b1180fe80000000000000020077ffeeedecebecefeeedecebeae9e8e7e6e5e4e3e2e15555555500430000080000000013880000000000010000000000058181000000ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2
 * Received packets: 
 * 0x000000000100000000000581810001feffff000102030000060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
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
 * Transmitted packets:
 * tx 1 psrc=201 data=0x0000000000ab000007000123810001fe0899000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets:
 * packet1:
 *  0x0000000000ab000007000123810001fe0899000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000 
 * packet2: 
 *  0x20000000cd1d000c000200008100006486dd61e00000004a1132cccfcecdcccbcac9c8c7c6c5c4c3c2c1ecefeeedecebeae9e8e7e6e5e4e3e2e150005555004a000008000000001388000000000000ab0000070001230899000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
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
 * Transmitted packets:
 * tx 1 psrc=201 data=0x000000000581000012345678810001fe0899000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets:
 * 0x20000000cd1d000c000200008100006486dd61e00000004a1132cccfcecdcccbcac9c8c7c6c5c4c3c2c1ecefeeedecebeae9e8e7e6e5e4e3e2e150005555004a000008000000001388000000000005810000123456780899000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 * 
 * vxlan- known learned destination- packet from vxlan tunnel- flood 
 * Purpose: - send packet using learned AC outlif from previous flow
 * 
 * Transmitted packets:
 * tx 1 psrc=201 data=0x000c000200000000070001008100001486dd60000000006b1180cccfcecdcccbcac9c8c7c6c5c4c3c2c1ecefeeedecebeae9e8e7e6e5e4e3e2e15555555500430000080000000013880000000700012300000000058181000000ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets: 
 * 0x000007000123000000000581810001feffff000102030000060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * 
 *
 *  Test Scenario: bud node
 * 
 * soc properties
 * parser_mode=1
 * bcm886xx_vxlan_enable=1
 * custom_feature_egress_independent_default_mode=1
 * bcm886xx_ipv6_tunnel_enable=1
 * bcm886xx_ip6_tunnel_encapsulation_mode=2
 * bcm886xx_ip6_tunnel_termination_mode=2
 * ip6_tunnel_term_in_tcam_vrf_nof_bits=11
 * tm_port_header_type_in_40=INJECTED_2_PP
 * tm_port_header_type_out_40=ETH
 * ucode_port_40=RCY.0:core_0.40
 * tm_port_header_type_in_41=INJECTED_2_PP
 * tm_port_header_type_out_41=ETH
 * ucode_port_41=RCY.1:core_1.41
 * add_ptch2_header=1
 * 
 * cd ../../../../src/examples/dpp
 * cint ../sand/utility/cint_sand_utils_global.c
 * cint utility/cint_utils_multicast.c 
 * cint utility/cint_utils_l3.c 
 * cint utility/cint_utils_vlan.c 
 * cint cint_ip_route.c 
 * cint cint_ip_tunnel_term.c 
 * cint cint_ipv6_fap.c 
 * cint cint_ipv6_tunnel.c
 * cint cint_ipv6_tunnel_term.c
 * cint cint_port_tpid.c
 * cint cint_advanced_vlan_translation_mode.c
 * cint cint_field_gre_learn_data_rebuild.c
 * cint cint_ip_tunnel.c
 * cint cint_ip_tunnel_term.c 
 * cint cint_mact.c
 * cint cint_vswitch_metro_mp.c
 * cint utility/cint_utils_vxlan.c
 * cint cint_ipv6_vxlan.c
 * cint cint_ipmc_flows.c 
 * cint_pmf_2pass_snoop.c
 * cint
 * print vxlan6_mc_bud_node_example(0, 200, 200, 201, 41);
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
 * 
 * tx 1 psrc=200 data=0x3333bbccddee0000070001008100000a86dd600000000036110afe80000000000000020077ffbebdbcbbff02112233445566778899aabbccddee555555550057000008000000001388000000000000f00000000000018100000a0899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
  * Received packets on unit 0 should be:
 * packet1: 
 * 0x3333bbccddee0000070001008100000a86dd600000000036110afe80000000000000020077ffbebdbcbbff02112233445566778899aabbccddee555555550057000008000000001388000000000000f00000000000018100000a0899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 * packet2: 
 * 0x0000000000f0000000000001810001fe0899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5 
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
    bcm_if_t vxlan_fec_intf_id; /* fec for the VXLAN tunnel, set at vxlan_example */

};

/*                 vpn_id | vni |       */
vxlan_s g_vxlan = {15,    5000, 
/*                 enable vdc support */
                   0, 
/*                 vxlan orientation */
                   1,
/*                 fec for the VXLAN tunnel */
                   0
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

void ip6_tunnel_glbl_init_vxlan(int unit, int is_cascade){

    /* update tunnel info */
    ip_tunnel_encap_ipv6_glbl_info.type_1 = bcmTunnelTypeVxlan6;
    ip_tunnel_encap_ipv6_glbl_info.type_2 = bcmTunnelTypeVxlan6;
    ip6_tunnel_term_glbl_info.tunnel_type_1 = bcmTunnelTypeVxlan6;
    ip6_tunnel_term_glbl_info.tunnel_type_2 = bcmTunnelTypeVxlan6;
    ip6_tunnel_term_glbl_info.cascade_1 = is_cascade;
    ip6_tunnel_term_glbl_info.cascade_2 = is_cascade;
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

/*
 * add access, 
 */
int
vxlan_vlan_port_add(int unit,  bcm_gport_t in_port, bcm_gport_t *port_id){
    int rv;
    bcm_vlan_port_t vp1;
    bcm_vlan_port_t_init(&vp1);
    
    /* the match criteria is port:1, out-vlan:510   */
    vp1.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vp1.port = in_port;
    vp1.match_vlan = 510;
    /* when packet forwarded to this port then it will be set with out-vlan:100,  in-vlan:200 */
    vp1.egress_vlan = 510;
    vp1.flags = 0;
    /* this is relevant only when get the gport, not relevant for creation */
    /* will be pupulated when the gport is added to service, using vswitch_port_add */
    vp1.vsi = 0;
    rv = bcm_vlan_port_create(unit,&vp1);
    
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }


    /* In advanced vlan translation mode, bcm_vlan_port_create does not create ingress / egress
       action mapping. This is here to compensate. */
    if (advanced_vlan_translation_mode) {
        rv = vlan_translation_vlan_port_create_to_translation(unit, &vp1);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan_translation_vlan_port_create_to_translation\n");
            return rv;
        }
    }


    if(verbose >= 2) {
        printf("Add vlan-port-id:0x%08x in-port:0x%08x match_vlan:0x%08x match_inner_vlan:0x%08x\n\r",vp1.vlan_port_id, vp1.port, vp1.match_vlan, vp1.match_inner_vlan);
    }
    
    /* handle of the created gport */
    *port_id = vp1.vlan_port_id;
    
    return rv;
}


/******* Run example ******/
 
 
/*
 * IP tunnel example 
 * - build IP tunnels. 
 * - add ip routes/host points to the tunnels
 */
int vxlan6_example(int unit, int in_port, int out_port, int vpn_id, int is_cascade){

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
    printf("inPort: %d \n", in_port);
    printf("outPort: %d \n", out_port);
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
    bcm_gport_t vxlan_port_id[2];
	uint32 tunnel_sip = 0x0a050001;
	uint32 sip_learned_fec = 0x1234;
    int vxlan_protocol_type = 0xfeef;
    int rv;
    int vxlan_port_flags = 0;
    int ll_encap_id;
    
    verbose = 10;

    bcm_ip_t native_dip = 0xABA1A1A1; /* ip address: 171:17:17:17 */ 

    /* init ip tunnel info, to fit VXLAN usage */
    ip6_tunnel_glbl_init_vxlan(unit, is_cascade);

    if (g_vxlan.vxlan_vdc_enable == 1) {
        /* set port vlan domain */
        rv = bcm_port_class_set(unit, out_port, bcmPortClassId, vdc_port_class);
        if (rv != BCM_E_NONE) {
          printf("Error, bcm_port_class_set unit %d, port %d rv %d\n", unit, out_port, rv);
          return rv;
        }
         rv = bcm_port_class_set(unit, in_port, bcmPortClassId, vdc_port_class);
        if (rv != BCM_E_NONE) {
          printf("Error, bcm_port_class_set unit %d, port %d rv %d\n", unit, in_port, rv);
          return rv;
        }
    }

    rv = bcm_port_class_set(unit, out_port, bcmPortClassFieldIngressTunnelTerminated, ip6_tunnel_term_glbl_info.port_property); 
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_class_set, in_port=%d, \n", in_port);
    }

    if (is_cascade) {
        rv = bcm_port_control_set(unit, out_port, bcmPortControlIPv6TerminationCascadedModeEnable, 1); 
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_control_set, in_port=%d, \n", in_port);
        }
    }

    /* init L2 VXLAN module */
    rv = bcm_vxlan_init(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set \n");
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
    rv = ipv6_tunnel_build_tunnels(unit, in_port, out_tunnel_intf_ids,out_tunnel_gports, &ll_encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, ipv4_tunnel_add_routes, in_port=%d, \n", in_port);
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
    
    /*** build tunnel terminations and router interfaces ***/
    rv = ipv6_tunnel_term_build_2_tunnels(unit,in_port,out_port,eg_intf_ids,in_tunnel_gports,&ll_encap_id); 
    if (rv != BCM_E_NONE) {
        printf("Error, ipv6_tunnel_term_build_2_tunnels, in_port=%d, \n", in_port);
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
    vxlan_port_add.in_port = in_port; 
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

    vxlan_port_id[1] = vxlan_port_add.vxlan_port_id; 



    /* port vlan port*/

    /* init vlan port glbl info */
    vswitch_metro_mp_info_init(out_port,0,0);

    port_tpid_init(in_port, 1, 1);
    rv = port_tpid_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, port_tpid_set with port_1\n");
        print rv;
        return rv;
    }

    port_tpid_init(out_port, 1, 1);
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



                                                                                    
    rv = vxlan_vlan_port_add(unit, out_port, &vlan_port_id);                           
    if (rv != BCM_E_NONE) {                                                           
        printf("Error, vswitch_metro_add_port_1 2, in_intf=0x%08x \n", vlan_port_id); 
    }                                                                                 
                                                                                      
     rv = vswitch_add_port(unit, vpn,out_port, vlan_port_id);                         
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

    rv = l2_addr_add(unit,mac_address,vpn,vxlan_port_id[0]);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_addr_add to vxlan_port_id[0]\n");
        return rv;
    }
    mac_inc(mac_address);

    rv = l2_addr_add(unit,mac_address,vpn,vxlan_port_id[1]);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_addr_add to vxlan_port_id[1]\n");
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
int vxlan6_mc_route(int unit, int vlan, int* ipmc_index)
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

    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID, ipmc_index[0]);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create $rv\n");
        return rv;
    }
    
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID, ipmc_index[1]);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create $rv\n");
        return rv;
    }

    return rv;
}

/* Function to add IPv6 entries to IPMC table*/
int vxlan6_mc_add_entry(int unit, int* ipmc_index, int vlan) {
    int rv = BCM_E_NONE;
    bcm_ipmc_addr_t ip6_ipmc;

    bcm_ipmc_addr_t_init(&ip6_ipmc);
    ip6_ipmc.flags = BCM_IPMC_IP6;
    sal_memcpy(ip6_ipmc.mc_ip6_addr, ip6_tunnel_term_glbl_info.mc_dip, 16);
    sal_memcpy(ip6_ipmc.mc_ip6_mask, ip6_tunnel_term_glbl_info.mc_dip_mask, 16);
    sal_memcpy(ip6_ipmc.s_ip6_addr, ip6_tunnel_term_glbl_info.mc_sip, 16);
    ip6_ipmc.vrf = ip6_tunnel_term_glbl_info.rif_vrf;
    ip6_ipmc.vid = vlan;
    ip6_ipmc.group = ipmc_index[0];
    rv = bcm_ipmc_add(unit, &ip6_ipmc);
    if (rv != BCM_E_NONE) {
        printf("bcm_ipmc_add() returned with failure code '%s'\n", bcm_errmsg(rv));
        return rv;
    }

    bcm_ipmc_addr_t_init(&ip6_ipmc);
    ip6_ipmc.flags = BCM_IPMC_IP6;
    sal_memcpy(ip6_ipmc.mc_ip6_addr, ip6_tunnel_term_glbl_info.mc_dip2, 16);
    sal_memcpy(ip6_ipmc.mc_ip6_mask, ip6_tunnel_term_glbl_info.mc_dip2_mask, 16);
    sal_memcpy(ip6_ipmc.s_ip6_addr, ip6_tunnel_term_glbl_info.mc_sip2, 16);
    ip6_ipmc.vrf = ip6_tunnel_term_glbl_info.rif_vrf;
    ip6_ipmc.vid = vlan;
    ip6_ipmc.group = ipmc_index[1];
    rv = bcm_ipmc_add(unit, &ip6_ipmc);
    if (rv != BCM_E_NONE) {
        printf("bcm_ipmc_add() returned with failure code '%s'\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/*
 * delete gport of type vlan-port to the multicast
 */
int multicast__vxlan_port_delete(int unit, int mc_group_id, int sys_port, int gport, uint8 is_egress) 
{

    int encap_id;
    int rv = BCM_E_NONE;
    
    rv = bcm_multicast_vxlan_encap_get(unit, mc_group_id, sys_port, gport, &encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_multicast_vxlan_encap_get mc_group_id:  0x%08x  phy_port:  0x%08x  gport:  0x%08x \n", mc_group_id, sys_port, gport);
        return rv;
    }
    rv = bcm_multicast_ingress_delete(unit, mc_group_id, sys_port, encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_multicast_ingress_delete mc_group_id:  0x%08x  phy_port:  0x%08x  encap_id:  0x%08x \n", mc_group_id, sys_port, encap_id);
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
int vxlan6_mc_field_group_set( int unit, int in_port, bcm_gport_t snoop_trap_gport_id, bcm_field_group_t *group)
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
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInterfaceClassPort);

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

    result = bcm_field_qualify_ForwardingType(unit, ent, bcmFieldForwardingTypeIp6Mcast);
    if (result != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_ForwardingType $rv\n");
        return result;
    }

    /* not recycle port  */
    result = bcm_field_qualify_InterfaceClassPort(unit, ent, 0, 0x1);
    if (result != BCM_E_NONE)
    {
      printf("ERROR: bcm_field_qualify_InterfaceClassPort returned %s\n", bcm_errmsg(result));
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
int vxlan6_mc_snoop2recycle_port(int unit, int in_port, int recycle_port)
{
    int rv = BCM_E_NONE;
    bcm_gport_t gport;
    bcm_field_group_t field;

    rv = bcm_port_class_set(unit, recycle_port, bcmPortClassFieldIngress, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_class_set\n");
        return rv;
    }

    rv = two_pass_snoop_set(unit, bcmRxTrapUserDefine, recycle_port, &gport);
    if (rv != BCM_E_NONE) {
        printf("Error, two_pass_snoop_set\n");
        return rv;
    }

    rv = vxlan6_mc_field_group_set(unit, in_port, gport, &field);
    if (rv != BCM_E_NONE) {
        printf("Error, vxlan_mc_field_group_set\n");
        return rv;
    }

    /* set egress PMF to add PTCH of port_1 when transmitting from recycle port*/
    rv = system_port_lookup_ptch(unit, 20, 17, in_port, recycle_port); 
    if (rv != BCM_E_NONE) {
        printf("Error, system_port_lookup_ptch\n");
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
int vxlan6_mc_bud_node_example(int unit, int in_port, int de_encap_out_port, int route_out_port, int recycle_port)
{
    int rv = BCM_E_NONE;
    int i;
    int vlan = 10;
    int ipmc_index[2] = {3000, 4000};
    int port_member_of_mc[1] = {route_out_port};
    bcm_gport_t vxlan_port_id;
    bcm_tunnel_terminator_t tunnel_term;
    int ip_tunnel_overlay_mc_lif = 0;
    bcm_gport_t ip_tunnel_overlay_mc_tunnel = 0;
    bcm_gport_t out_tunnel_id = 0;
    int vpn = g_vxlan.vpn_id;
    bcm_gport_t in_tunnel_gports[2];
    vxlan_port_add_s vxlan_port_add[2]; 
    
    rv = bcm_port_control_set(unit, in_port, bcmPortControlIPv6TerminationCascadedModeEnable, 1); 
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set, in_port=%d, \n", in_port);
    }

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

    rv = vxlan6_mc_route(unit, vlan, ipmc_index);
    if (rv != BCM_E_NONE) {
        printf("Error, vxlan_ipv4_mc_route $rv\n");
        return rv;
    }

    /* add ports to mc group */
    rv = add_ingress_multicast_forwarding(unit,ipmc_index[0], port_member_of_mc, 1, vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, add_ingress_multicast_forwarding $rv\n");
        return rv;
    }

    rv = add_ingress_multicast_forwarding(unit,ipmc_index[1], port_member_of_mc, 1, vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, add_ingress_multicast_forwarding $rv\n");
        return rv;
    }

    /* add routing entry to mc group */
    rv = vxlan6_mc_add_entry(unit, ipmc_index, vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, vxlan6_mc_add_entry $rv\n");
        return rv;
    }

    rv = vxlan6_example(unit, in_port, de_encap_out_port, vpn, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, vxlan6_example $rv\n");
        return rv;
    }

    rv = ipv6_tunnel_term_build_bud_tunnels(unit, in_port, de_encap_out_port, in_tunnel_gports);
    if (rv != BCM_E_NONE) {
        printf("Error, ipv6_tunnel_term_build_bud_tunnels $rv\n");
        return rv;
    }   

    for (i = 0; i < 2; i++) {
        vxlan_port_s_clear(&vxlan_port_add[i]); 
        vxlan_port_add[i].vpn = vpn; 
        vxlan_port_add[i].in_port = recycle_port; 
        vxlan_port_add[i].in_tunnel = in_tunnel_gports[i]; 
        vxlan_port_add[i].out_tunnel = 0; 
        vxlan_port_add[i].egress_if = g_vxlan.vxlan_fec_intf_id; 
        vxlan_port_add[i].flags = 0; 
        vxlan_port_add[i].network_group_id = g_vxlan.vxlan_network_group_id; 

        rv = vxlan__vxlan_port_add(unit, vxlan_port_add[i]); 
        if (rv != BCM_E_NONE) {
            printf("Error, vxlan__vxlan_port_add $rv\n");
            return rv;
        }

        /*Remove recycle port from the mc group VPN */
        rv = multicast__vxlan_port_delete(unit, vxlan_port_add[i].vpn, vxlan_port_add[i].in_port, vxlan_port_add[i].vxlan_port_id, 1);
        if (rv != BCM_E_NONE) {
            printf("Error, multicast__vxlan_port_delete $rv\n");
            return rv;
        }
    }
    
    rv = vxlan6_mc_snoop2recycle_port(unit, in_port, recycle_port);
    if (rv != BCM_E_NONE) {
        printf("Error, vxlan_mc_snoop2recycle_port $rv\n");
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
