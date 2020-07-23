/*~~~~~~~~~~~~~~~~~~~~~~~Bridge Router: IPMC~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_igmp_example.c
 * Purpose: Example shows how to configure IGMP snooping and IP compatible-MC groups to
 * local copies.
 * The following CINT tries to apply IGMP and IP compatible-MC to local copies on
 * regular Router with packet IPoE and Overlay such as VXLAN and L2VPN such as VPLS.
 * Note: Overlay and L2VPN traffic is supported from ARAD+.
 
 * Test IGMP snooping for IGMPoIPoETH packets
 *                        IGMPoIPoETHoVXLANoUDPoIPoEth packets
 *                           after vxlan tunnel termination
 *                        IGMPoIPoETHoPWEoMPLSoEth packets
 *                           after vpls tunnel termination
 * Test local copies multicast for IPoEth packets.
 *                        IPoETHoVXLANoUDPoIPoEth packets
 *                           after vxlan tunnel termination
 *                        IPoETHoPWEoMPLSoEth packets
 *                           after vpls tunnel termination
 
 * Calling sequence:
 *     IGMP snooping:
 *      - Configure device to support igmp on incoming port
 *        calling bcm_switch_control_port_set(unit, port, bcmSwitchIgmpQueryFlood, val);
 *      - Configure trap: trap IGMP memberships type packets.
 *        bcmRxTrapIgmpMembershipQuery trap at the link layer block (for IPoEth packets)
 *        bcmRxTrapFrwrdIgmpMembershipQuery trap at the forwarding block, after tunnel termination. (for vxlan/vpls packets after tunnel termination)
 *        calling bcm_rx_trap_type_create to create the trap type calling bcm_rx_trap_set to create the trap
 *
 *     multicast:
 *      - Configure device to support multicast
 *          calling bcm_switch_control_set(unit,bcmSwitchL3McastL2,1);
 *      - simulate igmp to create multicast group and add port:
 *        - open a new multicast group
 *          calling bcm_multicast_create
 *        - add port and vlan to the multicast group:
 *          calling bcm_multicast_ingress_add
 *        - create the multicast group
 *          calling bcm_ipmc_add
 
 * Traffic: 
 * IGMP snooping
 * IPoETH: send packet with IGMP Membership query.
 * IpoETH over tunnel: send packet with IGMP Membership query.
 * packets are trapped at LL block (for IPoETH) and at Forwarding block (for IPoETH o tunnel)
   and sent as is to out_port
 * multicast
 * IPoETH: send multicast compatible packet. multicast group: 0.0.1.2 (28 LSB of 224.0.1.2)
 * IPoETHoTunnel: packets arrive from the core. Tunnel is terminated. then check that the
 * remaining packet is multicast compatible. The packet is duplicated to all members of the
 * group (3 packets are sent)
 *
 * How to run: IGMP snooping IGMPoIPoETH
 * BCM> cint ../../../../src/examples/dpp/cint_igmp_example.c
 * BCM> cint
 * int unit=0;
 * int in_port=200;
 * int out_port=201;
 * igmp_snooping_enable(unit,in_port,out_port,1);
 * exit;
 * BCM> tx 1 PSRC=200
 * DATA=0x01005E00010200020500000008004500001C000000004002D92AC0A8000BE00001021164EE9B00000000000102030405060708090A0B0C0D0E0F1011121314150000000000000000000000000000000001000002000000000000000000000000
 * expected results:
 * - packet is sent to port 201 (diag pp FDT)
 * - add a vlan header (outgoing packet = incoming packet + 4B)
 *
 *
 * How to run: IGMP multicast for IPoETH packet
 * BCM> cint ../../../../src/examples/dpp/cint_igmp_example.c
 * BCM> cint
 * int unit=0;
 * int in_port=200;
 * int out_port=201;
 * int out_port2=202;
 * int vsi=4000;
 * igmp_mc_example(unit,in_port,out_port,out_port2, vsi,igmp_example_ipoeth_my_mac_get());
 * bcm_port_untagged_vlan_set(unit,in_port,vsi);
 * exit;
 * tx 1 PSRC=200 DATA=0x01005E0001020002050000000800450000B60000000040FFD793C0A8000BE0000102000102030405060708090A0B0C0D0E0F101112131415161718191A1AAAAA0000000000000000000000000000000001000002000000000000000000000000
 * expected result:
 * - 3 copies are sent to port 200, 201,202. (diag counters g)
 *
 *
 * How to run: IGMP snooping for IGMPoIPoEthoVPLS 
 * BCM> cint ../../../../src/examples/dpp/cint_port_tpid.c                
 * BCM> cint ../../../../src/examples/dpp/cint_advanced_vlan_translation_mode.c
 * BCM> cint ../../../../src/examples/dpp/cint_qos.c                      
 * BCM> cint ../../../../src/examples/dpp/utility/cint_utils_l3.c 
 * BCM> cint ../../../../src/examples/dpp/cint_mpls_lsr.c                  
 * BCM> cint ../../../../src/examples/dpp/cint_vswitch_metro_mp.c         
 * BCM> cint ../../../../src/examples/dpp/cint_vswitch_vpls.c             
 * BCM> cint ../../../../src/examples/dpp/cint_igmp_example.c
 * BCM> cint
 *  int unit=0;
 *  int in_port=200;
 *  int out_port=201;
 *  int vpn=4000;
 *  int sec_unit=-1;
 *  int ext_example=0;
 * vswitch_vlps_info_init(unit,out_port,pwe_in_port,pwe_out_port,10,20,15,30,vpn);
 * vswitch_vpls_run(unit,sec_unit,ext_example);
 * igmp_snooping_enable(unit,in_port,out_port,1);
 * exit;
 * BCM>tx 1 PSRC=200
   DATA=0x000000000011000000000002810000648847003EA040007DA14001005E0001020000000000FF0800450000500000000040020000C0A80101E0000102116400000000000000000001000000000001020304000000000000000000000000000000
 * expected results:
 * - packet is sent to port 201 (diag pp FDT)
 * 
 *
 * How to run IGMP multicast for IPoETHoVPLS_tunnel 
 * BCM> cint ../../../../src/examples/dpp/cint_port_tpid.c                
 * BCM> cint ../../../../src/examples/dpp/cint_advanced_vlan_translation_mode.c
 * BCM> cint ../../../../src/examples/dpp/cint_qos.c                      
 * BCM> cint ../../../../src/examples/dpp/utility/cint_utils_l3.c 
 * BCM> cint ../../../../src/examples/dpp/cint_mpls_lsr.c                  
 * BCM> cint ../../../../src/examples/dpp/cint_vswitch_metro_mp.c         
 * BCM> cint ../../../../src/examples/dpp/cint_vswitch_vpls.c             
 * BCM> cint ../../../../src/examples/dpp/cint_igmp_example.c
 * BCM> cint
 *  int unit=0;
 *  int in_port=200;
 *  int out_port=201;
 *  int out_port2=202;
 *  int vpn=4000;
 *  int sec_unit=-1;
 *  int ext_example=0; 
 * vswitch_vlps_info_init(unit,out_port,pwe_in_port,pwe_out_port,10,20,15,30,vpn);
 * vswitch_vpls_run(unit,sec_unit,ext_example);
 * igmp_mc_example(unit,in_port,out_port,out_port2,vpn,vswitch_vpls_my_mac_get());
 * exit;
 * BCM>tx 1 PSRC=200
 *DATA=0x000000000011000000000002810000648847003EA040007DA14001005E0001020000000000FF08004500005000000000403D0000C0A80101E0000102001020300000000000000001000000000001020304000000000000000000000000000000
 * expected results:
 * - 3 packets are sent to port 200, 201, 202 (diag counters g)
 * - vpls tunnel is terminated: (diag pp TERMination_Info,
 * - outgoing packet = incoming packet - 22B)
 *   incoming packet: ETH(14) + VLAN(4) + MPLS(4) +PWE(4) + nativeETH(14)           + nativeIP(20) + PAYLOAD(36) = 96
 *   outcoming packet:                                      nativeETH(14) + VLAN(4) + nativeIP(20) + PAYLOAD(36) = 74
 *
 *
 * How to run IGMP snooping for IGMPoIPoEthoVXLAN_tunnel
 * soc properties:
 * bcm886xx_ip4_tunnel_termination_mode=0
 * bcm886xx_l2gre_enable=0
 * bcm886xx_vxlan_enable=1
 * bcm886xx_ether_ip_enable=0
 * bcm886xx_vxlan_tunnel_lookup_mode=2
 * bcm886xx_auxiliary_table_mode=1 (for arad/+ devices only)
 *
 * BCM> cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * BCM> cint ../../../../src/examples/dpp/utility/cint_utils_l3.c
 * BCM> cint ../../../../src/examples/dpp/cint_ip_route.c
 * BCM> cint ../../../../src/examples/dpp/cint_port_tpid.c     
 * BCM> cint ../../../../src/examples/dpp/cint_advanced_vlan_translation_mode.c      
 * BCM> cint ../../../../src/examples/dpp/cint_field_gre_learn_data_rebuild.c   
 * BCM> cint ../../../../src/examples/dpp/cint_ip_tunnel.c cint_ip_tunnel_term.c
 * BCM> cint ../../../../src/examples/dpp/cint_mact.c cint_vswitch_metro_mp.c   
 * BCM> cint ../../../../src/examples/dpp/cint_vxlan.c                          
 * BCM> cint_../../../../src/examples/dpp/igmp_example.c                  
 * BCM> cint
 * int unit=0;
 * int in_port=200;
 * int out_port=201;
 * int vpn=4000;
 * vxlan_example(unit,in_port,out_port,vpn);
 * igmp_snooping_enable(unit,in_port,out_port,1);
 * exit;
 * BCM>tx 1 PSRC=200
 *DATA=0x000c00020000000007000100810000140800450000560000000040112f85a0000001ab000011555555550042d257080000000013880001005e00010200020500000008004500001c0000000040020000c0a8000be0000102116400000000000000010203000000000000000000000000000008004500001c0000000040020000
 *
 * expected result
 * - packet is sent to port 201 (diag pp FDT)
 *
 *
 * How to run IGMP multicast for IPoEthpVXLAN_tunnel
 * soc properties:
 * bcm886xx_ip4_tunnel_termination_mode=0
 * bcm886xx_l2gre_enable=0
 * bcm886xx_vxlan_enable=1
 * bcm886xx_ether_ip_enable=0
 * bcm886xx_vxlan_tunnel_lookup_mode=2
 * bcm886xx_auxiliary_table_mode=1
 *
 * BCM> cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * BCM> cint ../../../../src/examples/dpp/utility/cint_utils_l3.c
 * BCM> cint ../../../../src/examples/dpp/cint_ip_route.c
 * BCM> cint ../../../../src/examples/dpp/cint_port_tpid.c     
 * BCM> cint ../../../../src/examples/dpp/cint_advanced_vlan_translation_mode.c      
 * BCM> cint ../../../../src/examples/dpp/cint_field_gre_learn_data_rebuild.c   
 * BCM> cint ../../../../src/examples/dpp/cint_ip_tunnel.c cint_ip_tunnel_term.c
 * BCM> cint ../../../../src/examples/dpp/cint_mact.c cint_vswitch_metro_mp.c   
 * BCM> cint ../../../../src/examples/dpp/cint_vxlan.c                          
 * BCM> cint ../../../../src/examples/dpp/cint_igmp_example.c                  
 * BCM> cint
 * set unit 0
 * int in_port=200;
 * int out_port=201;
 * int out_port2=202;
 * int vpn=4000;
 * vxlan_example(unit,in_port,out_port,vpn);
 * igmp_mc_example(unit,in_port,out_port,out_port2,vpn,vxlan_my_mac_get());
 * exit;
 * tx 1 PSRC=200
 *DATA=0x000c000200000000070001008100001408004500004a0000000040112f91a0000001ab000011555555550036e3a0080000000013880001005e00010200020500000008004500001400000000403d0000c0a8000be00001020001020300000000000008004500001400000000403d0000000008004500001400000000403d0000
 
 * expected result:
 * - 3 packets are sent to port 200, 201, 202 (diag counters g)
 * vxlan tunnel is terminated: (diag pp TERMination_Info.
 * outgoing packet = incoming packet - 50)
 *   incoming packet: ETH(14) + VLAN(4) + IP(20) + UDP(8) + VXLAN(8B) + nativeETH(14)           + nativeIP(20) + PAYLOAD(36) = 124
 *   outcoming packet:                                                  nativeETH(14) + VLAN(4) + nativeIP(20) + PAYLOAD(36) = 74

 
 * Default Settings: 
 * There are two examples, one for snooping and one multicast.
 * 
 * These examples configures the following: 
 *  igmp snooping
 *  igmp multicast
 * 
 * how to run:
cint examples/dpp/cint_igmp_example.c 
 */

int print_level = 1;

bcm_gport_t igmp_encap[3];

/********** 
  functions
 */

bcm_mac_t igmp_example_ipoeth_my_mac_get() {
    bcm_mac_t my_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x10};
    return my_mac;
}

/*
 * config snooping for igmp on JR2
 *
 * SOC for trap: protocol_traps_mode=IN_PORT
 *
 * include cint file:
 *      cint ../../../../src/examples/dnx/traps/cint_utils_rx.c
 *      cint ../../../../src/examples/dnx/traps/cint_rx_trap_ingress_traps.c
 */
int igmp_snooping_config(int unit, int port, int out_port, int val)
{
    int rv = BCM_E_NONE;
    int trap_create_flags=0;
    bcm_rx_trap_t trap_type;
    bcm_rx_trap_config_t trap_config;

    bcm_gport_t action_gport;

    /**/
    trap_type = bcmRxTrapIgmpMembershipQuery;
    
    if (val == 0) {
        /* trap id in cint_rx_trap_ingress_traps_trap_id */
        rv = cint_rx_trap_protocol_traps_destroy(unit, trap_type, 2, 0);
        if (rv != BCM_E_NONE) {
            printf("Destroy IGMP snooping config failed!\n");
            return rv;
        }
    } else {
    
        /*
         Set protocol trap configuration flow:
             1. bcm_rx_trap_type_create(unit, flags, bcmRxTrapUserDefine, &trap_id); - Create user defined trap, allocate HW resource
             2. bcm_rx_trap_set(unit, trap_id, &trap_config); - Set action of trap in HW
             3. BCM_GPORT_TRAP_SET(trap_gport, trap_id, fwd_strength, snp_strength); - Encode trap_id, forward strength and snoop strength as gport
             4. bcm_rx_trap_protocol_set(unit, protocol_key, trap_gport); - Set the trap action profile for desired protocol and protocol_profile
             5. bcm_rx_trap_protocol_profiles_set(unit, port, &protocol_profiles); - Set protocol profiles for given LIF/port
         */
        bcm_rx_trap_config_t_init(&trap_config);
        trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;
        trap_config.trap_strength = 0;
        trap_config.dest_port = out_port;
        
        rv = cint_utils_rx_trap_create_and_set(unit, trap_create_flags, bcmRxTrapUserDefine, trap_config, &cint_rx_trap_ingress_traps_trap_id);
        if (rv != BCM_E_NONE) {
            printf("Error, in cint_utils_rx_trap_create_and_set\n");
            return rv;
        }
        printf(">>>Trap ID for IGMP snoop is %d\n", cint_rx_trap_ingress_traps_trap_id);
        
        /*
 */
        BCM_GPORT_TRAP_SET(action_gport, cint_rx_trap_ingress_traps_trap_id, 7, 0);
        rv = cint_rx_trap_protocol_traps_main(unit, port, 0, 2, trap_type, 0, action_gport);
        if (rv != BCM_E_NONE) {
            printf("Error, in cint_rx_trap_protocol_traps_main\n");
            return rv;
        } 

    }

    return BCM_E_NONE;
}


/* 
 * Enables igmp snooping
 * Packets meeting the criteria that are going to port <port> will be duplicated and sent out of <out_port>
 * @port        : tx pkt destination
 * @out_port    : rx pkt source
 * @val         : enable igmp
 */
int igmp_snooping_enable(int unit, int port, int out_port, int val){
    int rv = 0;
    int flags = 0;
    
    if (is_device_or_above(unit, JERICHO2)) {
        /*
         * On JR2, the trap type bcmRxTrapFrwrdIgmpMembershipQuery is not support
         * This is different from JR1
         */
        rv = igmp_snooping_config(unit, port, out_port, val);
        if (rv != BCM_E_NONE) {
            printf("Error, in igmp_snooping_config\n");
        }
        printf(">>>igmp_snooping_enable(%s) for JR2 successfully\n", val?"Enable":"Disable");
        return rv;
    }
    
    /* following is for JR1 */    
    rv = bcm_switch_control_port_set(unit, port, bcmSwitchIgmpQueryFlood, val);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_port_set \n");
        return rv;
    }
    
    bcm_rx_trap_config_t conf;
    bcm_rx_trap_config_t_init(&conf);
    bcm_rx_trap_t traps[] = {bcmRxTrapIgmpMembershipQuery, bcmRxTrapFrwrdIgmpMembershipQuery};
    int trap_i;
    conf.flags = (BCM_RX_TRAP_UPDATE_DEST);
    conf.trap_strength = 7;
    conf.dest_port = out_port;
    int trap_id[10];
    int trap_id_cnt = 0;

    for (trap_i = 0; trap_i < sizeof(traps) / sizeof(traps[0]); ++trap_i) {
        trap_id[trap_i]=0xff;
        rv = bcm_rx_trap_type_create(unit, flags, traps[trap_i], &trap_id[trap_i]);
        if (rv != BCM_E_NONE) {
            printf("Error, in trap create, trap %d \n", trap_i);
            return rv;
        }

        rv = bcm_rx_trap_set(unit,trap_id[trap_i],&conf);
        if (rv != BCM_E_NONE) {
            printf("Error, in trap set \n");
            return rv;
        }
        trap_id_cnt++;
    }
    return BCM_E_NONE;
}

int igmp_traps_destroy(int unit){
    bcm_rx_trap_t traps[] = {bcmRxTrapIgmpMembershipQuery, bcmRxTrapFrwrdIgmpMembershipQuery};
    int trap_i;
    int rv = 0;
    int trap_id[10];

    if (is_device_or_above(unit, JERICHO2)) {
        /*Clear the trap action profile for desired protocol and protocol_profile*/
        rv = igmp_snooping_config(unit, 0, 0, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, in igmp_snooping_config\n");
            return rv;
        }
        printf(">>>Clear the trap action profile done!\n");
        /*Destroy trap, deallocate HW resources*/
        rv = bcm_rx_trap_type_destroy(unit, cint_rx_trap_ingress_traps_trap_id);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_rx_trap_type_destroy\n");
            return rv;
        }
        printf(">>>Clear the trap ID done!\n");
    } else {
        for (trap_i = 0; trap_i < sizeof(traps) / sizeof(traps[0]); ++trap_i) {
            rv = bcm_rx_trap_type_get(unit, 0, traps[trap_i], &trap_id[trap_i]);
            if (rv == BCM_E_NONE) {
                rv = bcm_rx_trap_type_destroy(unit, trap_id[trap_i]);
                if (rv != BCM_E_NONE) {
                    printf("Error, in trap destroy, trap %d \n", trap_id[trap_i]);
                    return rv;
                }
            }
        }
    }
    return BCM_E_NONE;
}

/* 
 * An example of creating IP compatible-multicast groups
 * Enables igmp multicast
 * Packets going to <inP> will be replicated and sent through <inP>, <ouP> and <ouP2>
 * added to multicast group:
 * <inP,vlan>, <ouP,vlan> and <ouP2,vlan>
 * unit: 
 * @inP        : tx pkt destination, rx pkt source 
 * @ouP        : rx pkt source
 * @ouP2       : rx pkt source
 * @my_mac     : my mac 
 */
int igmp_mc_example(int unit, int inP, int ouP, int ouP2, int vlan, bcm_mac_t my_mac){
    int rv = BCM_E_NONE;

    /*
    On JR provided two options of controlling the sub-flows selection based on the ipmc_l3mcastl2_mode SoC Property:
        Property is disabled -  using global settings: The lookup is made depending on the bcm_switch_control_set
                                with the bcmSwitchL3McastL2 option to globally select the IGMP option for all RIFs.
        Property is enabled -   using per RIF settings: The lookup is made based on the BCM_L3_INGRESS_L3_MCAST_L2 flag 
                                that can be provided when calling the bcm_l3_ingress_create API. 
                                If flag is active, then the IGMP V3 lookup will be made, otherwise - the IGMP V2 will be done.

    On JR2 global selection for one of the IGMP modes is not supported - selection is made based on the RIF settings only.
    */
    rv = bcm_switch_control_set(unit,bcmSwitchL3McastL2,1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set \n");
        return rv;
    }

    if (is_device_or_above(unit, JERICHO2)) {

        /*disable vlan member ship check*/
        rv = bcm_port_vlan_member_set(unit, inP, 0);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_port_vlan_member_set for port[%d]\n", inP);
            return rv;
        }
        rv = bcm_port_vlan_member_set(unit, ouP, 0);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_port_vlan_member_set for port[%d]\n", ouP);
            return rv;
        }
        rv = bcm_port_vlan_member_set(unit, ouP2, 0);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_port_vlan_member_set for port[%d]\n", ouP2);
            return rv;
        }
        printf(">>>Disable vlan member ship check done!\n");

        /*Create outlif as CUD*/
        bcm_vlan_port_t vlan_port;
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vlan_port.match_vlan = vlan;
        vlan_port.vsi = 0;
        vlan_port.port = inP;
        rv = bcm_vlan_port_create(unit, &vlan_port);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_port_create(port = %d)\n", vlan_port.port);
            return rv;
        }
        igmp_encap[0] = vlan_port.vlan_port_id;

        vlan_port.port = ouP;
        rv = bcm_vlan_port_create(unit, &vlan_port);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_port_create(port = %d)\n", vlan_port.port);
            return rv;
        }
        igmp_encap[1] = vlan_port.vlan_port_id;

        vlan_port.port = ouP2;
        rv = bcm_vlan_port_create(unit, &vlan_port);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_port_create(port = %d)\n", vlan_port.port);
            return rv;
        }
        igmp_encap[2] = vlan_port.vlan_port_id;

    }

    bcm_ipmc_addr_t data;

    /* Set parameters: User can set different parameters. */
    bcm_ip_t mc_ip = 0xE0000102; /* 224.0.1.2 */
    bcm_ip_t src_ip = 0xC0800101; /* 192.128.1.1 */
    
    int ipmc_index = 5555;
    int dest_local_port_id   = ouP;
    int dest_local_port_id_2 = ouP2;
    int source_local_port_id = inP;
    int nof_dest_ports = 2, port_ndx;
    bcm_l3_intf_t intf, intf_ori;
    bcm_l3_ingress_t ingress_intf;
    uint32 flags;

    /* Create L3 Intf and disable its IPMC */
    bcm_l3_intf_t_init(&intf);
    sal_memcpy(intf.l3a_mac_addr, my_mac, 6);
    intf.l3a_vid = vlan;    

    /* before creating L3 INTF, check whether L3 INTF already exists*/
    bcm_l3_intf_t_init(&intf_ori);
    intf_ori.l3a_intf_id = vlan;
    rv = bcm_l3_intf_get(unit, &intf_ori);
    if (rv == BCM_E_NONE) {
        /* if L3 INTF already exists, replace it*/
        intf.l3a_flags = BCM_L3_REPLACE | BCM_L3_WITH_ID;
        intf.l3a_intf_id = vlan;
    }
    /***/
    intf.l3a_vrf = vlan;
    
    rv = bcm_l3_intf_create(unit, &intf);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_intf_create \n");
        return rv;
    }
    bcm_l3_ingress_t_init(&ingress_intf);

    if (is_device_or_above(unit, JERICHO2)) {
        flags = BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST | BCM_L3_INGRESS_WITH_ID | BCM_L3_INGRESS_L3_MCAST_L2 | BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST;
        /*flags = BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST | BCM_L3_INGRESS_WITH_ID | BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST;*/
        ingress_intf.vrf= vlan;
    }
    else
    {
        flags = BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST | BCM_L3_INGRESS_WITH_ID | BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST;
    }

    ingress_intf.flags = flags;
    rv = bcm_l3_ingress_create(unit, &ingress_intf, &intf.l3a_intf_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_ingress_create\n");
        return rv;
    }

    rv = bcm_multicast_destroy(unit,ipmc_index);
    if (rv != BCM_E_NOT_FOUND && rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_destroy \n");
        return rv;
    }

    /* Create the IP MC Group on given vlan */
    printf(">>>Create multicast[%d]...\n", ipmc_index);
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP |(is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L3) | BCM_MULTICAST_WITH_ID, &ipmc_index);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }
    printf(">>>Add member for MC group...\n");
    
    if (is_device_or_above(unit, JERICHO2)) {
        bcm_multicast_replication_t replications[3];
        bcm_gport_t dest_gport;
        
        bcm_multicast_replication_t_init(&replications[0]);
        bcm_multicast_replication_t_init(&replications[1]);
        bcm_multicast_replication_t_init(&replications[2]);

        BCM_GPORT_LOCAL_SET(dest_gport, dest_local_port_id);
        replications[0].port = dest_gport;
        replications[0].encap1 = igmp_encap[1] & 0x3fffff;
        BCM_GPORT_LOCAL_SET(dest_gport, dest_local_port_id_2);
        replications[1].port = dest_gport;
        replications[1].encap1 = igmp_encap[2] & 0x3fffff;
        BCM_GPORT_LOCAL_SET(dest_gport, source_local_port_id);
        replications[2].port = dest_gport;
        replications[2].encap1 = igmp_encap[0] & 0x3fffff;
        
        rv = bcm_multicast_set(unit, ipmc_index, BCM_MULTICAST_INGRESS_GROUP, 3, replications);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_set\n");
            return rv;
        }

        /* Configure the MC group to work with both cores */
        /*
        bcm_module_t mreps[2];
        mreps[0] = 0;
        mreps[1] = 1;
        rv = bcm_fabric_multicast_set(unit, ipmc_index, 0, 2, mreps);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_fabric_multicast_set \n");
            return rv;
        }
        printf(">>> Config MC group for both cores done.\n");
        */
    } else {
        rv = bcm_multicast_ingress_add(unit,ipmc_index,dest_local_port_id   ,vlan);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_ingress_add: port %d vlan: %d \n", dest_local_port_id   ,vlan);
            return rv;
        }

        rv = bcm_multicast_ingress_add(unit,ipmc_index,dest_local_port_id_2 ,vlan);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_ingress_add: port %d vlan: %d \n", dest_local_port_id_2   ,vlan);
            return rv;
        }

        rv =  bcm_multicast_ingress_add(unit,ipmc_index,source_local_port_id ,vlan);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_ingress_add: port %d vlan: %d \n", source_local_port_id   ,vlan);
            return rv;
        }
    }

    /* Add ipmc group entry to LEM */

    bcm_ipmc_addr_t_init(&data);
    data.mc_ip_addr = mc_ip;
    data.s_ip_addr = 0x0;
    data.vid = vlan;
    data.flags = BCM_IPMC_L2;
    data.group = ipmc_index;
    if (is_device_or_above(unit, JERICHO2)) {
        /*SIP empty with mask is illegal on JR2, set mask to 0x0.*/
        data.s_ip_mask = 0x0;
        data.mc_ip_mask = 0xFFFFFFFF;
    }

    /* Creates the entry */
    rv = bcm_ipmc_add(unit,&data);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_ipmc_add \n");
        return rv;
    }

    return rv;
}
 
 
