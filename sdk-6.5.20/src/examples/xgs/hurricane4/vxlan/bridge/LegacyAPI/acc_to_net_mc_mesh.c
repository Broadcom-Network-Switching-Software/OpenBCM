/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


/*
 * Feature  : VxLan access port to network port multicast function 
 *
 * Usage    :  BCM.0> cint acc_to_net_mc_mesh.c 
 *
 * config   : BCM56275_A1-BRIDGE-LEGACY.bcm
 *
 * Log file : acc_to_net_mc_mesh_log.txt
 *
 * Test Topology :
 *
 *                  +------------------------------+
 *                  |                              |
 *                  |                              |  network port 1<<<< Incoming VxLAN encapsulated IPMC
 *                  |                              +-----------------+
 * acees_port_1     |                              |
 * +----------------+          SWITCH              |
 *                  |                              |
 *                  |                              |
 *                  |                              | network port 2(to VTEP 2)
 *                  |                              +-----------------+
 *                  |                              |
 *                  +------------------------------+
 * Summary:
 * ========
 *   This Cint example to show configuration of acc port to vxlan vpn
 *   
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *    1) Step1 - Test Setup (Done in test_setup()):
 *    =============================================
 *      a) Select 1 ports as 1 accessport , 2 ports as 2 network port 
 *
 *      b) 
 *
 *      c) All ports are configured in loopback mode and IFP rules are installed
 *         on all ports to copy all packets ingressing these ports to CPU.
 *
 *      d) On egress ports an additional action is installed to drop the packets
 *         to avoid loopback.
 *
 *      e) Start packet watcher.
 *

 *    2) Step2 - Configuration (Done in vxlan_access_to_network_multicast_mesh())
 *    =============================================================================

 *       
 *
 *   3) Step3 - Verification ():
 *   ===========================================
 *       Test
 *       
 *       
 *       Test 1) UC Initiation-DLF
 *       
 *       Input to access port 1(the script will send the pkt automatically):
 *       
 *       0000000011BB00000000BBBB810000C8
 *       08004500002E000100004000DB7CC0A8
 *       0A01C0A8140100010203040506070809
 *       0A0B0C0D0E0F10111213141516171819
 *       
 *       
 *       Expected Result:
 *       If there is no L2 entry added, then packet is flooded to both network ports:
 *       
 *       network port 1:
 *       
 *       0000 0000 0002 0000 0000 2222 8100 012c
 *       0800 4501 0060 0000 0000 1011 6e6d 0a0a 
 *       0a01 1414 1401 ffff 2118 004c 0000 0800 
 *       0000 0123 4500 0000 0000 11bb 0000 0000 
 *       bbbb 0800 4500 002e 0001 0000 4000 db7c  No Inner-Tag 
 *       c0a8 0a01 c0a8 1401 0001 0203 0405 0607 
 *       0809 0a0b 0c0d 0e0f 1011 1213 1415 1617 
 *       1819 58cc 0ab2
 *       
 *       network port 2:
 *       
 *       0000 0000 0003 0000 0000 3333 8100 012d
 *       0800 4501 0060 0000 0000 1011 5062 0a0a 
 *       1402 1e1e 1e01 ffff 2118 004c 0000 0800 
 *       0000 0123 4500 0000 0000 11bb 0000 0000 
 *       bbbb 0800 4500 002e 0001 0000 4000 db7c  No Inner-Tag 
 *       c0a8 0a01 c0a8 1401 0001 0203 0405 0607 
 *       0809 0a0b 0c0d 0e0f 1011 1213 1415 1617 
 *       1819 8324 7803
 *       
 *       Test 2) UC Initiation
 *
 *       Add the L2 entry for 00:00:00:00:11:bb to network VP1:
 *       
 *       bcm_mac_t payload_da_1 = "00:00:00:00:11:bb";
 *       add_to_l2_table(unit, payload_da_1, vpn_id, net_vxlan_port_1, 0);
 * 
 *       Input to access port 1 (the script will send the pkt automatically):
 *       
 *       0000000011BB00000000BBBB810000C8
 *       08004500002E000100004000DB7CC0A8
 *       0A01C0A8140100010203040506070809
 *       0A0B0C0D0E0F10111213141516171819
 *       
 *       
 *       Expected Result:
 *       
 *       it is known unicast, VXLAN encap packet goes out of network port 1:
 *       
 *       0000 0000 0002 0000 0000 2222 8100 012c
 *       0800 4501 0060 0001 0000 1011 6e6c 0a0a 
 *       0a01 1414 1401 ffff 2118 004c 0000 0800 
 *       0000 0123 4500 0000 0000 11bb 0000 0000 
 *       bbbb 0800 4500 002e 0001 0000 4000 db7c  No Inner-Tag 
 *       c0a8 0a01 c0a8 1401 0001 0203 0405 0607 
 *       0809 0a0b 0c0d 0e0f 1011 1213 1415 1617 
 *       1819 437b d279
 *       
 *       
 *       Test 3) UC termination:
 *       
 *       Input to network port1 (the script will send the pkt automatically):
 * 
 *       0000000022220000000000028100012C
 *       0800450000600000000040113E6E1414
 *       14010A0A0A01FFFF2118004C00000800
 *       00000123450000000000BBBB00000000
 *       11BB08004500002E000000004000DB7D  No inner tag
 *       C0A81401C0A80A010001020304050607
 *       08090A0B0C0D0E0F1011121314151617
 *       1819
 *       
 *       Expected Result:
 *       Packet is decapped and goes out of access port 1:
 *       
 *       0000 0000 bbbb 0000 0000 11bb 8100 00c8  <<< New Tag 200 added back
 *       0800 4500 002e 0000 0000 4000 db7d c0a8 
 *       1401 c0a8 0a01 0001 0203 0405 0607 0809 
 *       0a0b 0c0d 0e0f 1011 1213 1415 1617 1819 
 *       345b b6c0
 *       
 * 
 *       
 *       Test 4) UnKnown MC Initiation
 *       
 *       Input to access port 1 (the script will send the pkt automatically):
 *       
 *       01005E01010100000000BBBB810000C8
 *       08004500002E000100004000CF23C0A8
 *       0A01E001010100010203040506070809
 *       0A0B0C0D0E0F10111213141516171819
 *       
 *       Expected Result:
 *       The MC traffic is sent to both network ports:
 *       
 *       network Port 1:
 *       
 *       0000 0000 0002 0000 0000 2222 8100 012c
 *       0800 4501 0060 0001 0000 1011 6e6c 0a0a 
 *       0a01 1414 1401 ffff 2118 004c 0000 0800 
 *       0000 0123 4500 0100 5e01 0101 0000 0000 
 *       bbbb 0800 4500 002e 0001 0000 4000 cf23 
 *       c0a8 0a01 e001 0101 0001 0203 0405 0607 
 *       0809 0a0b 0c0d 0e0f 1011 1213 1415 1617 
 *       1819 340d 3d6f
 *       
 *       network port 2:
 *       0000 0000 0003 0000 0000 3333 8100 012d
 *       0800 4501 0060 0001 0000 1011 5061 0a0a 
 *       1402 1e1e 1e01 ffff 2118 004c 0000 0800 
 *       0000 0123 4500 0100 5e01 0101 0000 0000 
 *       bbbb 0800 4500 002e 0001 0000 4000 cf23 
 *       c0a8 0a01 e001 0101 0001 0203 0405 0607 
 *       0809 0a0b 0c0d 0e0f 1011 1213 1415 1617 
 *       1819 ed5c 4cc9
 *       
 *       
 *       Test 5) Known MC Termination
 *       
 *       Input to network port1(the script will send the pkt automatically):
 *
 *       0000000022220000000000028100012C
 *       0800450000600000000040113E6E1414
 *       14010A0A0A01FFFF2118004C00000800
 *       00000123450001005E01010100000000
 *       11BB08004500002E000000004000C524
 *       C0A81401E00101010001020304050607
 *       08090A0B0C0D0E0F1011121314151617
 *       1819
 *       
 *       Expected Result: 
 *       Out on access port 1:
 *       
 *       0100 5e01 0101 0000 0000 11bb 8100 00c8  New Tag 200 added back
 *       0800 4500 002e 0000 0000 4000 c524 c0a8 
 *       1401 e001 0101 0001 0203 0405 0607 0809 
 *       0a0b 0c0d 0e0f 1011 1213 1415 1617 1819 
 *       8f97 b98a
 */


cint_reset();


/**************************   1 basic function begin*************************************/
/* Create vlan and add port to vlan */

int
vlan_create_add_port(int unit, int vid, int port)
{
    bcm_pbmp_t pbmp, upbmp;
    bcm_error_t rv = BCM_E_NONE;

    rv = bcm_vlan_create(unit, vid);
	if (BCM_E_NONE != rv)
	{
			printf("bcm_vlan_create failed %s\n", bcm_errmsg(rv));		
			return rv;	
	}
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    rv = bcm_vlan_port_add(unit, vid, pbmp, upbmp);
	if (BCM_E_NONE != rv)
	{
			printf("bcm_vlan_port_add failed %s\n", bcm_errmsg(rv));		
			return rv;	
	}	
	
	printf("vlan_create_add_port SUCCESS \n");
	return rv;	
}

int
vlan_create_add_ports(int unit, int vid, int count, bcm_port_t *ports)
{
    bcm_pbmp_t pbmp, upbmp;
    int i;
    bcm_error_t rv = BCM_E_NONE;
    rv  = bcm_vlan_create(unit, vid);
	if (BCM_E_NONE != rv)
	{
			printf("bcm_vlan_create failed %s\n", bcm_errmsg(rv));		
			return rv;	
	}
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);

    for (i = 0; i < count; i++) {
        BCM_PBMP_PORT_ADD(pbmp, ports[i]);
    }

     rv = bcm_vlan_port_add(unit, vid, pbmp, upbmp);
	if (BCM_E_NONE != rv)
	{
			printf("bcm_vlan_port_add failed %s\n", bcm_errmsg(rv));		
			return rv;	
	}

     printf("vlan_create_add_ports SUCCESS! \n");
	return rv;	
}

/* Create L3 interface */
int
create_l3_interface(int unit, int flags, bcm_mac_t local_mac, int vid, int vrf, bcm_if_t *intf_id)
{
    bcm_l3_intf_t l3_intf; 
    bcm_error_t rv = BCM_E_NONE;

    /* L3 interface and egress object for access */
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_flags |= flags;
    sal_memcpy(l3_intf.l3a_mac_addr, local_mac, sizeof(local_mac));
    l3_intf.l3a_vid = vid;
    l3_intf.l3a_vrf = vrf;
    rv = bcm_l3_intf_create(unit, &l3_intf);
    *intf_id = l3_intf.l3a_intf_id;
	
	if (BCM_E_NONE != rv)
	{
			printf("bcm_l3_intf_create failed %s\n", bcm_errmsg(rv));		
			return rv;	
	}

     printf("create_l3_interface SUCCESS! \n");
    return rv;
}

int 
create_l3_if_id(int unit, int flags, int intf_id, bcm_mac_t local_mac, int vid, int vrf)
{
    bcm_l3_intf_t l3_intf;
    bcm_error_t rv = BCM_E_NONE;

    /* L3 interface and egress object for access */
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_flags = BCM_L3_WITH_ID | flags;
    l3_intf.l3a_intf_id = intf_id;
    sal_memcpy(l3_intf.l3a_mac_addr, local_mac, sizeof(local_mac));
    l3_intf.l3a_vid = vid;
    l3_intf.l3a_vrf = vrf;

    rv = bcm_l3_intf_create(unit, &l3_intf);
	if (BCM_E_NONE != rv)
	{
			printf("bcm_l3_intf_create failed %s\n", bcm_errmsg(rv));		
			return rv;	
	}

     printf("create_l3_if_id SUCCESS! \n");
    return rv;
	
}

/* Create L3 egress object */
int
l3_create_egr_obj(int unit, int flags, int l3_if, bcm_mac_t nh_mac, bcm_gport_t gport,
                   bcm_if_t *egr_obj_id)
{
    bcm_l3_egress_t l3_egress;
    bcm_error_t rv = BCM_E_NONE;

    bcm_l3_egress_t_init(&l3_egress);
    l3_egress.flags  |= flags;
    sal_memcpy(l3_egress.mac_addr,  nh_mac, sizeof(nh_mac));
    l3_egress.intf  = l3_if;
    l3_egress.port = gport;
    rv = bcm_l3_egress_create(unit, 0, &l3_egress, egr_obj_id);
	if (BCM_E_NONE != rv)
	{
			printf("bcm_l3_egress_create failed %s\n", bcm_errmsg(rv));		
			return rv;	
	}

     printf("l3_create_egr_obj SUCCESS! \n");
    return rv;	
}

int
vxlan_create_egr_obj(int unit, uint32 flag, int l3_if, bcm_mac_t nh_mac, bcm_gport_t gport,
              int vid, bcm_if_t *egr_obj_id)
{
    bcm_l3_egress_t l3_egress;
    bcm_error_t rv = BCM_E_NONE;

    bcm_l3_egress_t_init(&l3_egress);
    l3_egress.flags = BCM_L3_VXLAN_ONLY | flag;
    l3_egress.intf  = l3_if;
    sal_memcpy(l3_egress.mac_addr,  nh_mac, sizeof(nh_mac));
    l3_egress.vlan = vid;
    l3_egress.port = gport;
    rv = bcm_l3_egress_create(unit, flag, &l3_egress, egr_obj_id);
	if (BCM_E_NONE != rv)
	{
			printf("bcm_l3_egress_create failed %s\n", bcm_errmsg(rv));		
			return rv;	
	}

     printf("vxlan_create_egr_obj SUCCESS! \n");
    return rv;		
}

int
add_to_l2_station(int unit, bcm_mac_t mac, bcm_vlan_t vid, int flags)
{
    bcm_l2_station_t l2_station;
    int station_id;
    bcm_error_t rv = BCM_E_NONE;

    bcm_l2_station_t_init(&l2_station);
    sal_memcpy(l2_station.dst_mac, mac, sizeof(mac));
    l2_station.flags   = flags;
    l2_station.dst_mac_mask = "ff:ff:ff:ff:ff:ff";
    l2_station.vlan         = vid;
    l2_station.vlan_mask    = 0xfff;
    rv = bcm_l2_station_add(unit, &station_id, &l2_station);
	if (BCM_E_NONE != rv)
	{
			printf("bcm_l2_station_add failed %s\n", bcm_errmsg(rv));		
			return rv;	
	}

     printf("add_to_l2_station SUCCESS! \n");	
    return rv;
}

int
add_to_l2_table(int unit, bcm_mac_t mac, bcm_vpn_t vpn_id, int port, int stat)
{
    bcm_l2_addr_t l2_addr;
    bcm_error_t rv = BCM_E_NONE;

    bcm_l2_addr_t_init(&l2_addr, mac, vpn_id);
    if (stat) {
        l2_addr.flags = BCM_L2_STATIC;
    }
    l2_addr.port = port;
    rv = bcm_l2_addr_add(unit, &l2_addr);
	if (BCM_E_NONE != rv)
	{
			printf("bcm_l2_addr_add failed %s\n", bcm_errmsg(rv));		
			return rv;	
	}

     printf("add_to_l2_table SUCCESS! \n");	
    return rv;	
}

int do_vxlan_global_setting(int unit)
{
    bcm_error_t rv = BCM_E_NONE;

    /* Enable L3 Egress Mode */
    rv = bcm_switch_control_set(unit, bcmSwitchL3EgressMode, TRUE);
	if (BCM_E_NONE != rv)
	{
			printf("bcm_switch_control_set failed %s\n", bcm_errmsg(rv));		
			return rv;	
	}

     printf("bcm_switch_control_set SUCCESS! \n");		

    /* Set UDP port for VXLAN */
    rv = bcm_switch_control_set(unit, bcmSwitchVxlanUdpDestPortSet, udp_dp);
	if (BCM_E_NONE != rv)
	{
			printf("bcm_switch_control_set bcmSwitchVxlanUdpDestPortSet failed %s\n", bcm_errmsg(rv));		
			return rv;	
	}

     printf("bcm_switch_control_set bcmSwitchVxlanUdpDestPortSet SUCCESS! \n");			

    /* Enable/Disable VXLAN Entropy */
    rv = bcm_switch_control_set(unit, bcmSwitchVxlanEntropyEnable, FALSE);
	if (BCM_E_NONE != rv)
	{
			printf("bcm_switch_control_set bcmSwitchVxlanEntropyEnablefailed %s\n", bcm_errmsg(rv));		
			return rv;	
	}

     printf("bcm_switch_control_set bcmSwitchVxlanEntropyEnable SUCCESS! \n");			

    /* Enable/Disable VXLAN Tunnel lookup failure settings */
    rv = bcm_switch_control_set(unit, bcmSwitchVxlanTunnelMissToCpu, TRUE);
	if (BCM_E_NONE != rv)
	{
			printf("bcm_switch_control_set  bcmSwitchVxlanTunnelMissToCpu failed %s\n", bcm_errmsg(rv));		
			return rv;	
	}

     printf("bcm_switch_control_set bcmSwitchVxlanTunnelMissToCpu SUCCESS! \n");			

    /* Enable/Disable VXLAN VN_ID lookup failure settings */
    rv = bcm_switch_control_set(unit, bcmSwitchVxlanVnIdMissToCpu, TRUE);
	if (BCM_E_NONE != rv)
	{
			printf("bcm_switch_control_set  bcmSwitchVxlanVnIdMissToCpu failed %s\n", bcm_errmsg(rv));		
			return rv;	
	}

     printf("bcm_switch_control_set bcmSwitchVxlanVnIdMissToCpu SUCCESS! \n");			

    return BCM_E_NONE;
}

int
do_vxlan_access_port_settings(int unit, bcm_port_t a_port)
{
    bcm_error_t rv = BCM_E_NONE;

	/* Enable VLAN translation on access port */
    rv = bcm_vlan_control_port_set(unit, a_port, bcmVlanTranslateIngressEnable, TRUE);
	if (BCM_E_NONE != rv)
	{
			printf("bcm_vlan_control_port_set  bcmVlanTranslateIngressEnable failed %s\n", bcm_errmsg(rv));		
			return rv;	
	}

     printf("bcm_vlan_control_port_set bcmVlanTranslateIngressEnable SUCCESS! \n");			

    /* Should disable Vxlan Processing on access port */
    rv = bcm_port_control_set(unit, a_port, bcmPortControlVxlanEnable, FALSE);
	if (BCM_E_NONE != rv)
	{
			printf("bcm_vlan_control_port_set  bcmPortControlVxlanEnable failed %s\n", bcm_errmsg(rv));		
			return rv;	
	}

     printf("bcm_vlan_control_port_set bcmPortControlVxlanEnable SUCCESS! \n");				

    /* Should disable Tunnel Based Vxlan-VnId lookup */
    rv = bcm_port_control_set(unit, a_port, bcmPortControlVxlanTunnelbasedVnId, FALSE);
	if (BCM_E_NONE != rv)
	{
			printf("bcm_vlan_control_port_set  bcmPortControlVxlanTunnelbasedVnId failed %s\n", bcm_errmsg(rv));		
			return rv;	
	}

     printf("bcm_vlan_control_port_set bcmPortControlVxlanTunnelbasedVnId SUCCESS! \n");				

    return BCM_E_NONE;
}

int
do_vxlan_net_port_settings(int unit, bcm_port_t n_port)
{
    bcm_error_t rv = BCM_E_NONE;

    /* Enable VXLAN Processing on network port */
    rv =  bcm_port_control_set(unit, n_port, bcmPortControlVxlanEnable, TRUE);
	if (BCM_E_NONE != rv)
	{
			printf("bcm_port_control_set  bcmPortControlVxlanEnable failed %s\n", bcm_errmsg(rv));		
			return rv;	
	}

     printf("bcm_port_control_set bcmPortControlVxlanEnable SUCCESS! \n");				
	

    /* Allow tunnel based VXLAN-VNID lookup */
    rv =  bcm_port_control_set(unit, n_port, bcmPortControlVxlanTunnelbasedVnId, FALSE);
                              /* Only BCM_VXLAN_PORT_MATCH_VN_ID at network port */
	if (BCM_E_NONE != rv)
	{
			printf("bcm_port_control_set  bcmPortControlVxlanTunnelbasedVnId failed %s\n", bcm_errmsg(rv));		
			return rv;	
	}

     printf("bcm_port_control_set bcmPortControlVxlanTunnelbasedVnId SUCCESS! \n");				
								  

    return BCM_E_NONE;
}

int
tunnel_initiator_setup(int unit, bcm_ip_t lip, bcm_ip_t rip, int dp, int sp, int ttl, int *tid)
{
    bcm_tunnel_initiator_t tnl_init;
    bcm_error_t rv = BCM_E_NONE;

    bcm_tunnel_initiator_t_init(&tnl_init);
    tnl_init.type  = bcmTunnelTypeVxlan;
    tnl_init.ttl = ttl;
    tnl_init.sip = lip;
    tnl_init.dip = rip;
    tnl_init.udp_dst_port = dp;
    tnl_init.udp_src_port = sp;
    rv = bcm_vxlan_tunnel_initiator_create(unit, &tnl_init);
    *tid = tnl_init.tunnel_id;
	if (BCM_E_NONE != rv)
	{
			printf("bcm_vxlan_tunnel_initiator_create failed %s\n", bcm_errmsg(rv));		
			return rv;	
	}

     printf("tunnel_initiator_setup SUCCESS! \n");			
	
    return rv;
}

int
tunnel_terminator_setup(int unit, bcm_ip_t rip, bcm_ip_t lip, bcm_vlan_t net_vid,
                        int tunnel_init_id, int *term_id)
{
    bcm_tunnel_terminator_t tnl_term;
    bcm_error_t rv = BCM_E_NONE;

    bcm_tunnel_terminator_t_init(&tnl_term);
    tnl_term.type = bcmTunnelTypeVxlan;
    tnl_term.sip = rip;    /* For MC tunnel, Don't care */
    tnl_term.dip = lip;
    tnl_term.tunnel_id = tunnel_init_id;
    tnl_term.flags = BCM_TUNNEL_TERM_TUNNEL_WITH_ID;
    if (net_vid != -1) {
        tnl_term.vlan = net_vid;  /* MC tunnel only - for Bud check */
    }
    rv = bcm_vxlan_tunnel_terminator_create(unit, &tnl_term);
    *term_id = tnl_term.tunnel_id;
	if (BCM_E_NONE != rv)
	{
			printf("bcm_vxlan_tunnel_terminator_create failed %s\n", bcm_errmsg(rv));		
			return rv;	
	}

     printf("tunnel_terminator_setup SUCCESS! \n");			

    return rv;
}

int
create_vxlan_vpn(int unit, int vpn_id, int vnid, bcm_multicast_t bc,
                 bcm_multicast_t mc, bcm_multicast_t uuc)
{
    bcm_vxlan_vpn_config_t vpn_info;
    bcm_error_t rv = BCM_E_NONE;

    bcm_vxlan_vpn_config_t_init(&vpn_info);
    vpn_info.flags = BCM_VXLAN_VPN_ELAN | BCM_VXLAN_VPN_WITH_ID | BCM_VXLAN_VPN_WITH_VPNID;
    vpn_info.vpn  = vpn_id;
    vpn_info.vnid = vnid;
    vpn_info.broadcast_group         = bc;
    vpn_info.unknown_multicast_group = mc;
    vpn_info.unknown_unicast_group   = uuc;
    rv = bcm_vxlan_vpn_create(unit, &vpn_info);
	if (BCM_E_NONE != rv)
	{
			printf("bcm_vxlan_vpn_create failed %s\n", bcm_errmsg(rv));		
			return rv;	
	}

     printf("create_vxlan_vpn SUCCESS! \n");			

    return rv;
}

int
sdtag_create_vxlan_vpn(int unit, int vpn_id, int vnid, bcm_multicast_t bc,
              bcm_multicast_t mc, bcm_multicast_t uuc)
{
    bcm_vxlan_vpn_config_t vpn_info;
    bcm_error_t rv = BCM_E_NONE;

    bcm_vxlan_vpn_config_t_init(&vpn_info);
    /* Service tag will be removed going into UDP tunnel */
    vpn_info.flags = BCM_VXLAN_VPN_ELAN | BCM_VXLAN_VPN_WITH_ID | BCM_VXLAN_VPN_WITH_VPNID |
                     BCM_VXLAN_VPN_SERVICE_TAGGED | BCM_VXLAN_VPN_SERVICE_VLAN_DELETE;
    vpn_info.vpn  = vpn_id;
    vpn_info.vnid = vnid;
    vpn_info.broadcast_group         = bc;
    vpn_info.unknown_multicast_group = mc;
    vpn_info.unknown_unicast_group   = uuc;
    vpn_info.egress_service_tpid     = 0x8100;
    rv = bcm_vxlan_vpn_create(unit, &vpn_info);
	if (BCM_E_NONE != rv)
	{
			printf("bcm_vxlan_vpn_create failed %s\n", bcm_errmsg(rv));		
			return rv;	
	}

     printf("sdtag_create_vxlan_vpn SUCCESS! \n");		
    return rv;
}

int
create_vxlan_acc_vp(int unit, bcm_vpn_t vpn, uint32 flags, bcm_gport_t port,
                    bcm_vxlan_port_match_t criteria,
                    bcm_if_t egr_obj, bcm_vlan_t vid, bcm_gport_t *vp)
{
    bcm_vxlan_port_t vxlan_port;
    bcm_error_t rv = BCM_E_NONE;

    bcm_vxlan_port_t_init(&vxlan_port);
    vxlan_port.flags = BCM_VXLAN_PORT_SERVICE_TAGGED | flags;
    vxlan_port.match_port =        port;
    vxlan_port.criteria =          criteria;
    vxlan_port.egress_if =         egr_obj;
    vxlan_port.match_vlan =        vid;
    rv = bcm_vxlan_port_add(unit, vpn, &vxlan_port);
    *vp = vxlan_port.vxlan_port_id;
	if (BCM_E_NONE != rv)
	{
			printf("bcm_vxlan_port_add failed %s\n", bcm_errmsg(rv));		
			return rv;	
	}

     printf("create_vxlan_acc_vp SUCCESS! \n");			

    return rv;
}

int
sdtag_create_vxlan_acc_vp(int unit, bcm_vpn_t vpn, uint32 flags, bcm_gport_t port,
                    bcm_vxlan_port_match_t criteria, bcm_if_t egr_obj,
                    bcm_vlan_t vid, bcm_vlan_t sdtag, bcm_gport_t *vp)
{
    bcm_vxlan_port_t vxlan_port;
    bcm_error_t rv = BCM_E_NONE;

    bcm_vxlan_port_t_init(&vxlan_port);
    vxlan_port.flags = BCM_VXLAN_PORT_SERVICE_TAGGED | flags;
    vxlan_port.criteria =          criteria;
    vxlan_port.match_port =        port;
    vxlan_port.egress_if =         egr_obj;
    vxlan_port.match_vlan =        vid;
    vxlan_port.egress_service_tpid = 0x8100;
    vxlan_port.egress_service_vlan = sdtag;
    rv = bcm_vxlan_port_add(unit, vpn, &vxlan_port);
    *vp = vxlan_port.vxlan_port_id;
	if (BCM_E_NONE != rv)
	{
			printf("bcm_vxlan_port_add failed %s\n", bcm_errmsg(rv));		
			return rv;	
	}

     printf("sdtag_create_vxlan_acc_vp SUCCESS! \n");			
	
    return rv;
}

int
create_vxlan_net_vp(int unit, bcm_vpn_t vpn, uint32 flags, bcm_gport_t port,
                    bcm_vxlan_port_match_t criteria, bcm_if_t egr_obj,
                    bcm_gport_t tun_init, bcm_gport_t tun_term, bcm_gport_t *vp)
{
    bcm_vxlan_port_t vxlan_port;
    bcm_error_t rv = BCM_E_NONE;

    bcm_vxlan_port_t_init(&vxlan_port);
    vxlan_port.flags = BCM_VXLAN_PORT_NETWORK | BCM_VXLAN_PORT_EGRESS_TUNNEL |
                       BCM_VXLAN_PORT_SERVICE_TAGGED | flags;
    vxlan_port.match_port =        port;
    vxlan_port.criteria =          criteria;
    vxlan_port.egress_if =         egr_obj;
    vxlan_port.egress_tunnel_id =  tun_init;
    vxlan_port.match_tunnel_id =   tun_term;
    /* vpn_id parameter is not care for net VP */
    rv = bcm_vxlan_port_add(unit, vpn, &vxlan_port);
    *vp = vxlan_port.vxlan_port_id;
	if (BCM_E_NONE != rv)
	{
			printf("bcm_vxlan_port_add failed %s\n", bcm_errmsg(rv));		
			return rv;	
	}

     printf("create_vxlan_net_vp SUCCESS! \n");			

    return rv;
}



        
bcm_error_t
create_ifp_group (int unit, bcm_field_group_config_t *group_config)
{
    bcm_error_t rv = BCM_E_NONE;

    /* FP group configuration and creation */
    bcm_field_group_config_t_init(group_config);

    BCM_FIELD_QSET_INIT(group_config->qset);
    BCM_FIELD_QSET_ADD(group_config->qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config->qset, bcmFieldQualifyInPort);

    group_config->mode = bcmFieldGroupModeAuto;

    rv = bcm_field_group_config_create(unit, group_config);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_field_group_config_create() : %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}


bcm_error_t
create_ifp_to_cpu_rule (int unit, bcm_field_group_t gid, bcm_field_entry_t *eid, bcm_port_t port)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_port_t port_mask = 0xffffffff;
    
    /* FP entry configuration and creation */
    rv = bcm_field_entry_create(unit, gid, eid);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_field_entry_create() : %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_qualify_InPort(unit, *eid, port, port_mask);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_field_qualify_InPort() : %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* FP entry actions configuration */
    rv = bcm_field_action_add(unit, *eid, bcmFieldActionDrop, 0, 0);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_field_action_add() : %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_action_add(unit, *eid, bcmFieldActionCopyToCpu, 0, 0);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_field_action_add() : %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Installing FP entry to FP TCAM */
    rv = bcm_field_entry_install(unit, *eid);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_field_entry_install() : %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}


bcm_error_t
destroy_ifp_to_cpu_rule (int unit, bcm_field_entry_t eid)
{
    bcm_error_t rv = BCM_E_NONE;
    
    /* Destroying FP entry */
    rv = bcm_field_entry_destroy(unit, eid);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_field_entry_destroy() : %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}



/**************************1 basic function end*************************************/

/**************************2 global variable defination begin***********************/

bcm_error_t rv = BCM_E_NONE;
uint16 udp_dp = 8472;   /* UDP dst port used for OTV */

bcm_port_t access_port_1  = BCM_PORT_INVALID;
bcm_port_t network_port_1 = BCM_PORT_INVALID;
bcm_port_t network_port_2 = BCM_PORT_INVALID;


bcm_gport_t access_gport_1  = BCM_GPORT_INVALID;
bcm_gport_t network_gport_1 = BCM_GPORT_INVALID;
bcm_gport_t network_gport_2 = BCM_GPORT_INVALID;

/* FP Group/Rule Creation/Destruction*/
bcm_field_group_config_t group_config;
bcm_field_entry_t access_port_1_eid = BCM_FIELD_ENTRY_INVALID;
bcm_field_entry_t network_port_1_eid = BCM_FIELD_ENTRY_INVALID;
bcm_field_entry_t network_port_2_eid = BCM_FIELD_ENTRY_INVALID;

/**************************2 global variable defination end ***********************/

/**************************3 test setup functions begin***********************/

bcm_error_t
create_ifp_group (int unit, bcm_field_group_config_t *group_config)
{
    bcm_error_t rv = BCM_E_NONE;
    /* FP group configuration and creation */
    bcm_field_group_config_t_init(group_config);
    BCM_FIELD_QSET_INIT(group_config->qset);
    BCM_FIELD_QSET_ADD(group_config->qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config->qset, bcmFieldQualifyInPort);
    group_config->mode = bcmFieldGroupModeAuto;
    rv = bcm_field_group_config_create(unit, group_config);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_field_group_config_create() : %s\n", bcm_errmsg(rv));
        return rv;
    }
    return rv;
}

bcm_error_t
create_ifp_to_cpu_rule (int unit, bcm_field_group_t gid, bcm_field_entry_t *eid, bcm_port_t port, int drop)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_port_t port_mask = 0xffffffff;
    /* FP entry configuration and creation */
    rv = bcm_field_entry_create(unit, gid, eid);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_field_entry_create() : %s\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_field_qualify_InPort(unit, *eid, port, port_mask);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_field_qualify_InPort() : %s\n", bcm_errmsg(rv));
        return rv;
    }
    /* FP entry actions configuration */
    if (1 == drop) {
        rv = bcm_field_action_add(unit, *eid, bcmFieldActionDrop, 0, 0);
        if(BCM_FAILURE(rv)) {
            printf("\nError in bcm_field_action_add() : %s\n", bcm_errmsg(rv));
            return rv;
        }
    }
    rv = bcm_field_action_add(unit, *eid, bcmFieldActionCopyToCpu, 0, 0);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_field_action_add() : %s\n", bcm_errmsg(rv));
        return rv;
    }
    /* Installing FP entry to FP TCAM */
    rv = bcm_field_entry_install(unit, *eid);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_field_entry_install() : %s\n", bcm_errmsg(rv));
        return rv;
    }
    return rv;
}

bcm_error_t
destroy_ifp_to_cpu_rule (int unit, bcm_field_entry_t eid)
{
    bcm_error_t rv = BCM_E_NONE;

    /* Destroying FP entry */
    rv = bcm_field_entry_destroy(unit, eid);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_field_entry_destroy() : %s\n", bcm_errmsg(rv));
        return rv;
    }
    return rv;
}


/* This function is written so that hardcoding of port
   numbers in Cint scripts is removed. This function gives
   required number of ports
*/
bcm_error_t portNumbersGet(int unit, int *port_list, int num_ports)
{

  int i=0,port=0,rv=0;
  bcm_port_config_t configP;
  bcm_pbmp_t ports_pbmp;

  rv = bcm_port_config_get(unit,&configP);
  if(BCM_FAILURE(rv)) {
    printf("\nError in retrieving port configuration: %s.\n",bcm_errmsg(rv));
    return rv;
  }

  ports_pbmp = configP.e;
  for (i= 1; i < BCM_PBMP_PORT_MAX; i++) {
    if (BCM_PBMP_MEMBER(&ports_pbmp,i)&& (port < num_ports)) {
      port_list[port]=i;
      port++;
    }
  }

  if (( port == 0 ) || ( port != num_ports )) {
      printf("portNumbersGet() failed \n");
      return -1;
  }

  return BCM_E_NONE;

}

/*
 * Configures the port in loopback mode and discard setting to BCM_PORT_DISCARD_NONE.
 * Also add a rule to copy packets ingressing on the port to CPU.
 */
bcm_error_t ingress_port_setup(int unit, bcm_port_t port, bcm_field_entry_t *entry)
{
  int drop;
  bcm_error_t rv;
  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_NONE));
  if (BCM_FIELD_ENTRY_INVALID != *entry)
  {
    rv = destroy_ifp_to_cpu_rule(unit, *entry);
    if(BCM_FAILURE(rv))
    {
        printf("\nError executing destroy_ifp_to_cpu_rule%s.\n",bcm_errmsg(rv));
        return rv;
    }
  }
  rv = create_ifp_to_cpu_rule(unit, group_config.group, entry, port, (drop = 0));
  if(BCM_FAILURE(rv))
  {
      printf("\nError executing create_ifp_to_cpu_rule(): %s.\n",bcm_errmsg(rv));
      return rv;
  }

  return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode and discard setting to BCM_PORT_DISCARD_ALL.
 * Also add a rule to copy packets ingressing on the port to CPU and drop them
 * to avoid loopback.
 */

bcm_error_t egress_port_setup(int unit, bcm_port_t port, bcm_field_entry_t *entry)
{
  int drop;
  bcm_error_t rv;
  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_ALL));
  if (BCM_FIELD_ENTRY_INVALID != *entry)
  {
    rv = destroy_ifp_to_cpu_rule(unit, *entry);
    if(BCM_FAILURE(rv))
    {
        printf("\nError executing destroy_ifp_to_cpu_rule%s.\n",bcm_errmsg(rv));
        return rv;
    }
  }
  rv = create_ifp_to_cpu_rule(unit, group_config.group, entry, port, (drop = 1));
  if(BCM_FAILURE(rv))
  {
      printf("\nError executing create_ifp_to_cpu_rule(): %s.\n",bcm_errmsg(rv));
      return rv;
  }

  return BCM_E_NONE;
}



/*   Select 3 ports and does ingess and egress port setting on respective ports.
 */
bcm_error_t test_setup(int unit)
{
  int port_list[3], i;
  bcm_field_entry_t entry[3] = {BCM_FIELD_ENTRY_INVALID, BCM_FIELD_ENTRY_INVALID, BCM_FIELD_ENTRY_INVALID};  
  if (BCM_E_NONE != portNumbersGet(unit, port_list, 3)) {
    printf("portNumbersGet() failed\n");
    return -1;
  }

  access_port_1 = port_list[0];
  network_port_1 = port_list[1];
  network_port_2 = port_list[2];

  BCM_IF_ERROR_RETURN(create_ifp_group(unit, &group_config));
  printf("Using port number:%d as access_port_1\n", access_port_1);
  if (BCM_E_NONE != ingress_port_setup(unit, access_port_1, &entry[0])) {
    printf("ingress_port_setup() failed for port %d\n", access_port_1);
    return -1;
  }
  access_port_1_eid = entry[0];
  
  for (i = 1; i <= 2; i++) {
    printf("Using port number:%d as network_port_%d\n", port_list[i], (i ));
    if (BCM_E_NONE != egress_port_setup(unit, port_list[i], &entry[i])) {
      printf("egress_port_setup() failed for port %d\n", port_list[i]);
      return -1;
    }
  }
  network_port_1_eid = entry[1];
  network_port_2_eid = entry[2];


  bshell(unit, "pw start report +raw +decode");
  return BCM_E_NONE;
}

/**************************3 test setup functions end ***********************/

/**************************4 main functions begin ***********************/

/********************************************************************
*********************************************************************
*************** main script *********************
 */



bcm_vpn_t vpn_id = 0x7010;
int net_vxlan_port_1;

bcm_error_t vxlan_access_to_network_multicast_mesh(int unit)
{
  /* VPN */
  uint32 vnid = 0x12345;
  bcm_multicast_t dlf_group = 0;

  /* Used to create a dummy VPN that is used to create network VP */
  int rsvd_network_vpn = 0x7001;
  int rsvd_network_vpn_vnid = 0xeeee;

  /*
   * Access side
   */
  /* Access side MAC addresses are not really used */
  bcm_mac_t acc_dummy_mac = "00:00:01:00:00:01";


  bcm_vlan_t acc_vid = 200;
  bcm_gport_t acc_gport = BCM_GPORT_INVALID;
  bcm_if_t vxlan_acc_intf_id = 1;
  bcm_if_t vxlan_acc_egr_obj;
  int acc_vxlan_port;
  bcm_if_t vxlan_acc_encap;

  /*
   * Network side
   */

  /* Tunnel */
  uint8 ttl = 16;

  uint16 udp_sp = 0xffff;
  bcm_vrf_t vrf = 0;

  /* Network 1 */

  bcm_vlan_t net_vid_1 = 300;
  bcm_if_t net_intf_id_1 = 2;
  bcm_gport_t net_gport_1 = BCM_GPORT_INVALID;
  bcm_if_t net_egr_obj_1;
  bcm_mac_t net_local_mac_1 = "00:00:00:00:22:22";
  bcm_mac_t net_remote_mac_1 = "00:00:00:00:00:02";
  bcm_ip_t tnl_local_ip_1  = 0x0a0a0a01;    /* 10.10.10.1 */
  bcm_ip_t tnl_remote_ip_1 = 0x14141401;    /* 20.20.20.1 */
  int tnl_uc_init_id_1, tnl_uc_term_id_1;

  /* Network 2 */

  bcm_vlan_t net_vid_2 = 301;
  bcm_if_t net_intf_id_2 = 3;
  bcm_gport_t net_gport_2 = BCM_GPORT_INVALID;
  bcm_if_t net_egr_obj_2;
  int net_vxlan_port_2;
  bcm_mac_t net_local_mac_2 = "00:00:00:00:33:33";
  bcm_mac_t net_remote_mac_2 = "00:00:00:00:00:03";
  bcm_ip_t tnl_local_ip_2  = 0x0a0a1402;    /* 10.10.20.2 */
  bcm_ip_t tnl_remote_ip_2 = 0x1E1E1E01;    /* 30.30.30.1 */
  int tnl_uc_init_id_2, tnl_uc_term_id_2;

  /*
   * DLF/BC tunnel and virtual port
   */
  bcm_if_t vxlan_mc_eo_1, vxlan_mc_eo_2;
  int vxlan_mc_port_1, vxlan_mc_port_2;
  bcm_if_t encap_dlf_id_1, encap_dlf_id_2;

  /* Global settings */
  BCM_IF_ERROR_RETURN(do_vxlan_global_setting(unit));

  /* Access port settings */
  BCM_IF_ERROR_RETURN(do_vxlan_access_port_settings(unit, access_port_1));

  /* Network port settings */
  BCM_IF_ERROR_RETURN(do_vxlan_net_port_settings(unit, network_port_1));
  BCM_IF_ERROR_RETURN(do_vxlan_net_port_settings(unit, network_port_2));

  BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, access_port_1, &acc_gport)); 
  BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, network_port_1, &net_gport_1)); 
  BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, network_port_2, &net_gport_2)); 

  /* VLAN settings */
  BCM_IF_ERROR_RETURN(vlan_create_add_port(unit, acc_vid, access_port_1));
  BCM_IF_ERROR_RETURN(vlan_create_add_port(unit, net_vid_1, network_port_1));
  BCM_IF_ERROR_RETURN(vlan_create_add_port(unit, net_vid_2, network_port_2));

  /* Multicast group for the segment MC/BC */
  BCM_IF_ERROR_RETURN(bcm_multicast_create(unit, BCM_MULTICAST_TYPE_VXLAN, &dlf_group));
  print dlf_group;

  /* Create VXLAN VPN */
  BCM_IF_ERROR_RETURN(sdtag_create_vxlan_vpn(unit,vpn_id, vnid, dlf_group, dlf_group, dlf_group));

  /* Create VXLAN VPN for assigning Network-VP */
  BCM_IF_ERROR_RETURN(sdtag_create_vxlan_vpn(unit,rsvd_network_vpn, rsvd_network_vpn_vnid, dlf_group, dlf_group, dlf_group));

  /* L3 interface and egress object for access - note use a dummy MAC address */
  BCM_IF_ERROR_RETURN(create_l3_interface(unit, 0, acc_dummy_mac, acc_vid, vrf, &vxlan_acc_intf_id));
  BCM_IF_ERROR_RETURN(vxlan_create_egr_obj(unit, 0, vxlan_acc_intf_id, acc_dummy_mac, acc_gport, acc_vid, &vxlan_acc_egr_obj));

  /* Create VXLAN VP for access port */
  BCM_IF_ERROR_RETURN(sdtag_create_vxlan_acc_vp(unit, vpn_id, BCM_VXLAN_PORT_SERVICE_VLAN_ADD,
                      acc_gport, BCM_VXLAN_PORT_MATCH_PORT_VLAN,
                      vxlan_acc_egr_obj, acc_vid, acc_vid, &acc_vxlan_port));
  print acc_vxlan_port;

  /*
   * Network port 1
   */
  /* Tunnel Setup (UC Initiator & Terminator) */
  BCM_IF_ERROR_RETURN(tunnel_initiator_setup(unit,tnl_local_ip_1, tnl_remote_ip_1, udp_dp, udp_sp, ttl, &tnl_uc_init_id_1));
  BCM_IF_ERROR_RETURN(tunnel_terminator_setup(unit, tnl_remote_ip_1, tnl_local_ip_1, -1, tnl_uc_init_id_1, &tnl_uc_term_id_1));

  /* L3 interface and egress object */
  BCM_IF_ERROR_RETURN(create_l3_interface(unit, 0, net_local_mac_1, net_vid_1, vrf, &net_intf_id_1));
  BCM_IF_ERROR_RETURN(add_to_l2_station(unit, net_local_mac_1, net_vid_1, BCM_L2_STATION_IPV4 | BCM_L2_STATION_UNDERLAY));

  BCM_IF_ERROR_RETURN(vxlan_create_egr_obj(unit, 0, net_intf_id_1, net_remote_mac_1, net_gport_1,
                 net_vid_1, &net_egr_obj_1));

  /* Create VXLAN VP */
  BCM_IF_ERROR_RETURN(create_vxlan_net_vp(unit, rsvd_network_vpn, 0, net_gport_1, BCM_VXLAN_PORT_MATCH_VN_ID,
                      net_egr_obj_1, tnl_uc_init_id_1, tnl_uc_term_id_1, &net_vxlan_port_1));
  print net_vxlan_port_1;

  /*
   * Network port 2
   */
  /* Tunnel Setup (UC Initiator & Terminator) */
  BCM_IF_ERROR_RETURN(tunnel_initiator_setup(unit, tnl_local_ip_2, tnl_remote_ip_2, udp_dp, udp_sp, ttl, &tnl_uc_init_id_2));
  BCM_IF_ERROR_RETURN(tunnel_terminator_setup(unit, tnl_remote_ip_2, tnl_local_ip_2, -1, tnl_uc_init_id_2, &tnl_uc_term_id_2));

  /* L3 interface and egress object */
  BCM_IF_ERROR_RETURN(create_l3_interface(unit, 0, net_local_mac_2, net_vid_2, vrf, &net_intf_id_2));
  BCM_IF_ERROR_RETURN(add_to_l2_station(unit, net_local_mac_2, net_vid_2, BCM_L2_STATION_IPV4 | BCM_L2_STATION_UNDERLAY));

  BCM_IF_ERROR_RETURN(vxlan_create_egr_obj(unit, 0, net_intf_id_2, net_remote_mac_2, net_gport_2, net_vid_2, &net_egr_obj_2));

  /* Create VXLAN VP */
  BCM_IF_ERROR_RETURN(create_vxlan_net_vp(unit, rsvd_network_vpn, 0, net_gport_2, BCM_VXLAN_PORT_MATCH_VN_ID,
                net_egr_obj_2, tnl_uc_init_id_2, tnl_uc_term_id_2, &net_vxlan_port_2));
  print net_vxlan_port_2;

  /*
   * DLF/BC
   */
  /* On UC VXLAN network 1 */
  BCM_IF_ERROR_RETURN(vxlan_create_egr_obj(unit, BCM_L3_IPMC, net_intf_id_1, net_remote_mac_1, net_gport_1,
                       net_vid_1, &vxlan_mc_eo_1));

  /* Create DLF/BC VXLAN VP at network port 1 */
  BCM_IF_ERROR_RETURN(create_vxlan_net_vp(unit, rsvd_network_vpn, BCM_VXLAN_PORT_MULTICAST, net_gport_1,
                      BCM_VXLAN_PORT_MATCH_VN_ID, vxlan_mc_eo_1,
                      tnl_uc_init_id_1, tnl_uc_term_id_1, &vxlan_mc_port_1));
  print vxlan_mc_port_1;

  /* On UC VXLAN network 2 */
  BCM_IF_ERROR_RETURN(vxlan_create_egr_obj(unit, BCM_L3_IPMC, net_intf_id_2, net_remote_mac_2, net_gport_2,
                       net_vid_2, &vxlan_mc_eo_2));

  /* Create DLF/BC VXLAN VP at network port 2 */
  BCM_IF_ERROR_RETURN(create_vxlan_net_vp(unit, rsvd_network_vpn, BCM_VXLAN_PORT_MULTICAST, net_gport_2,
                      BCM_VXLAN_PORT_MATCH_VN_ID, vxlan_mc_eo_2,
                      tnl_uc_init_id_2, tnl_uc_term_id_2, &vxlan_mc_port_2));
  print vxlan_mc_port_2;

  /* DLF MC group set up - should contain all Access ports and Network DLF port */
  BCM_IF_ERROR_RETURN(bcm_multicast_vxlan_encap_get(unit, dlf_group, acc_gport, acc_vxlan_port, &vxlan_acc_encap));
  BCM_IF_ERROR_RETURN(bcm_multicast_egress_add(unit, dlf_group, acc_vxlan_port, vxlan_acc_encap));

  BCM_IF_ERROR_RETURN(bcm_multicast_vxlan_encap_get(unit, dlf_group, net_gport_1, vxlan_mc_port_1, &encap_dlf_id_1));
  print encap_dlf_id_1;
  BCM_IF_ERROR_RETURN(bcm_multicast_egress_add(unit, dlf_group, vxlan_mc_port_1, encap_dlf_id_1));

  BCM_IF_ERROR_RETURN(bcm_multicast_vxlan_encap_get(unit, dlf_group, net_gport_2, vxlan_mc_port_2, &encap_dlf_id_2));
  print encap_dlf_id_2;
  BCM_IF_ERROR_RETURN(bcm_multicast_egress_add(unit, dlf_group, vxlan_mc_port_2, encap_dlf_id_2));

  /*
   * Add unicast Payload L2 address to L2 table to avoid DLF flooding
  bcm_mac_t payload_sa   = "00:00:00:00:11:aa";
  bcm_mac_t payload_da_1 = "00:00:00:00:11:bb";
  bcm_mac_t payload_da_2 = "00:00:00:00:11:cc";

  add_to_l2_table(unit, payload_sa, vpn_id, acc_vxlan_port, 0);
  add_to_l2_table(unit, payload_da_1, vpn_id, net_vxlan_port_1, 0);
  add_to_l2_table(unit, payload_da_2, vpn_id, net_vxlan_port_2, 0);
   */

  /*=====================================
   * Known multicast Setup
   *====================================*/
  bcm_mac_t  known_mc_mac = "01:00:5e:01:01:01";
  bcm_multicast_t known_mc_group;
  bcm_if_t known_mc_encap_1;
  bcm_if_t known_mc_encap_2;

  /* Multicast group for the known MC group */
  BCM_IF_ERROR_RETURN(bcm_multicast_create(unit, BCM_MULTICAST_TYPE_VXLAN, &known_mc_group));
  print known_mc_group;

  /* Known MC group replication add all necessary access & network VPs */
  BCM_IF_ERROR_RETURN(bcm_multicast_vxlan_encap_get(unit, known_mc_group, acc_gport, acc_vxlan_port, &vxlan_acc_encap));
  BCM_IF_ERROR_RETURN(bcm_multicast_egress_add(unit, known_mc_group, acc_vxlan_port, vxlan_acc_encap));

  BCM_IF_ERROR_RETURN(bcm_multicast_vxlan_encap_get(unit, known_mc_group, net_gport_1, vxlan_mc_port_1, &known_mc_encap_1));
  print known_mc_encap_1;
  BCM_IF_ERROR_RETURN(bcm_multicast_egress_add(unit, known_mc_group, vxlan_mc_port_1, known_mc_encap_1));

  BCM_IF_ERROR_RETURN(bcm_multicast_vxlan_encap_get(unit, known_mc_group, net_gport_2, vxlan_mc_port_2, &known_mc_encap_2));
  print known_mc_encap_2;
  BCM_IF_ERROR_RETURN(bcm_multicast_egress_add(unit, known_mc_group, vxlan_mc_port_2, known_mc_encap_2));

  bcm_l2_addr_t mc_addr;
  bcm_l2_addr_t_init(&mc_addr, known_mc_mac, vpn_id);
  mc_addr.flags = BCM_L2_STATIC | BCM_L2_MCAST;
  mc_addr.l2mc_group = known_mc_group;
  BCM_IF_ERROR_RETURN(bcm_l2_addr_add (unit, &mc_addr));

  printf("acc_to_net_mc_mesh SUCCESS \n");		

  return BCM_E_NONE;

}

 
/* Simple wrapper function used to transmit test packet */
bcm_error_t
test_harness(int unit)
{
		bcm_error_t rv=0; 
		printf(" \n**************start  test_harness**************\n\n\n\n\n");	 

		/************************************************************************************************************/	
		printf(" \n **************TEST 1  UC Initiation-DLF: access bc to 2 network ports **************\n");	 
		bshell(0, "l2 show ");


		/*		
		*		0000000011BB00000000BBBB810000C8
		*		08004500002E000100004000DB7CC0A8
		*		0A01C0A8140100010203040506070809
		*		0A0B0C0D0E0F10111213141516171819
		*/


		bshell(0, "port ce0 discard=none ");

		printf(" \n **************create ifp rules to copy ce1/ce2 pkt to cpu ************** \n");	   
		create_ifp_to_cpu_rule(unit, group_config.group, &eid1, network_port_1);		
		create_ifp_to_cpu_rule(unit, group_config.group, &eid2, network_port_2);

		/* sent DLF packet */
		bshell(0, "tx 1 pbm=ce0 DATA=0x0000000011BB00000000BBBB810000C808004500002E000100004000DB7CC0A80A01C0A81401000102030405060708090A0B0C0D0E0F10111213141516171819");
		bshell(0, "sleep 1");

		bshell(0, "l2 show ");
		bshell(0, "port ce0 discard=all ");
		printf(" \n**************destroy the ifp rules **************\n");	  
		destroy_ifp_to_cpu_rule(unit, eid1);   
		destroy_ifp_to_cpu_rule(unit, eid2);   


		/************************************************************************************************************/
		printf(" \n **************TEST 2  UC Initiation-UC : access uc to network port1**************\n");	 
		bshell(0, "l2 show ");

		bcm_mac_t payload_da_1 = "00:00:00:00:11:bb";
		add_to_l2_table(unit, payload_da_1, vpn_id, net_vxlan_port_1, 0);

		/*		 
		 *		 0000000011BB00000000BBBB810000C8
		 *		 08004500002E000100004000DB7CC0A8
		 *		 0A01C0A8140100010203040506070809
		 *		 0A0B0C0D0E0F10111213141516171819
		 */


		bshell(0, "port ce0 discard=none ");
		printf(" \n **************create ifp rules to copy ce1 pkt to cpu**************  \n");    
		create_ifp_to_cpu_rule(unit, group_config.group, &eid1, network_port_1);

		/* sent DLF packet */
		bshell(0, "tx 1 pbm=ce0 DATA=0x0000000011BB00000000BBBB810000C808004500002E000100004000DB7CC0A80A01C0A81401000102030405060708090A0B0C0D0E0F10111213141516171819");
		bshell(0, "sleep 1");

		bshell(0, "l2 show ");
		bshell(0, "port ce0 discard=all ");
		printf(" \n **************destroy the ifp rules************** \n");	  
		destroy_ifp_to_cpu_rule(unit, eid1);   

		/************************************************************************************************************/
		printf(" \n **************TEST 3 UC termination: network port1 to access port **************\n");	 
		bshell(0, "l2 show ");

		/* 
		*		0000000022220000000000028100012C
		*		0800450000600000000040113E6E1414
		*		14010A0A0A01FFFF2118004C00000800
		*		00000123450000000000BBBB00000000
		*		11BB08004500002E000000004000DB7D  No inner tag
		*		C0A81401C0A80A010001020304050607
		*		08090A0B0C0D0E0F1011121314151617
		*		1819
		*/

		bshell(0, "port ce1 discard=none ");
		printf(" \n **************create ifp rules to copy ce0 pkt to cpu**************  \n");    
		create_ifp_to_cpu_rule(unit, group_config.group, &eid1, access_port_1);

		/* sent DLF packet */
		bshell(0, "tx 1 pbm=ce1 DATA=0x0000000022220000000000028100012C0800450000600000000040113E6E141414010A0A0A01FFFF2118004C0000080000000123450000000000BBBB0000000011BB08004500002E000000004000DB7DC0A81401C0A80A01000102030405060708090A0B0C0D0E0F1011121314151617");
		bshell(0, "sleep 1");

		bshell(0, "l2 show ");
		bshell(0, "port ce1 discard=all ");
		printf(" \n **************destroy the ifp rules************** \n");	  
		destroy_ifp_to_cpu_rule(unit, eid1);   

		/************************************************************************************************************/
		printf(" \n **************TEST4  UnKnown MC Initiation: acc port mc to 2 network ports**************\n");	 
		bshell(0, "l2 show ");

		 /*
		*		01005E01010100000000BBBB810000C8
		*		08004500002E000100004000CF23C0A8
		*		0A01E001010100010203040506070809
		*		0A0B0C0D0E0F10111213141516171819
		*/		

		bshell(0, "port ce0 discard=none ");
		printf(" \n **************create ifp rules to copy ce1/ce2 pkt to cpu**************  \n");    
		create_ifp_to_cpu_rule(unit, group_config.group, &eid1, network_port_1);		
		create_ifp_to_cpu_rule(unit, group_config.group, &eid2, network_port_2);

		/* sent DLF packet */
		bshell(0, "tx 1 pbm=ce0 DATA=0x01005E01010100000000BBBB810000C808004500002E000100004000CF23C0A80A01E0010101000102030405060708090A0B0C0D0E0F10111213141516171819");
		bshell(0, "sleep 1");

		bshell(0, "l2 show ");
		bshell(0, "port ce0 discard=all ");
		printf(" \n **************destroy the ifp rules************** \n");	  
		destroy_ifp_to_cpu_rule(unit, eid1);   		
		destroy_ifp_to_cpu_rule(unit, eid2);   

		/************************************************************************************************************/
		printf(" \n **************TEST5  Known MC Termination network port1 to acc port **************\n");	 
		bshell(0, "l2 show ");

		/*
		*		0000000022220000000000028100012C
		*		0800450000600000000040113E6E1414
		*		14010A0A0A01FFFF2118004C00000800
		*		00000123450001005E01010100000000
		*		11BB08004500002E000000004000C524
		*		C0A81401E00101010001020304050607
		*		08090A0B0C0D0E0F1011121314151617
		*		1819
		*/		


		bshell(0, "port ce1 discard=none ");
		printf(" \n **************create ifp rules to copy ce0 pkt to cpu**************  \n");    
		create_ifp_to_cpu_rule(unit, group_config.group, &eid1, access_port_1);		

		/* sent DLF packet */
		bshell(0, "tx 1 pbm=ce1 DATA=0x0000000022220000000000028100012C0800450000600000000040113E6E141414010A0A0A01FFFF2118004C0000080000000123450001005E0101010000000011BB08004500002E000000004000C524C0A81401E0010101000102030405060708090A0B0C0D0E0F10111213141516171819");
		bshell(0, "sleep 1");

		bshell(0, "l2 show ");
		bshell(0, "port ce1 discard=all ");
		printf(" \n **************destroy the ifp rules************** \n");	  
		destroy_ifp_to_cpu_rule(unit, eid1);   

		/************************************************************************************************************/
				
		bshell(0, "port ce0,ce1,ce2 lb=none");
		bshell(0, "port ce0,ce1,ce2 discard=none ");

    return rv;
}
 


 /**************************4 main functions end ***********************/
 
 
 /**************************5 verify functions begin ***********************/
 
 /*
  * send the pkt to verify the script
 */
 void verify(int unit)
 {
	char	   str[512];
	bshell(unit, "hm ieee");
	printf(" \n****************************\n\n\n\n\n");  

	/************************************************************************************************************/  
	printf(" \n **************TEST 1  UC Initiation-DLF: access bc to  network port 2 and 3  **************\n");	  
	bshell(0, "l2 show ");


	/* 	 
	*		 0000000011BB00000000BBBB810000C8
	*		 08004500002E000100004000DB7CC0A8
	*		 0A01C0A8140100010203040506070809
	*		 0A0B0C0D0E0F10111213141516171819
	*/


	if (BCM_E_NONE != ingress_port_setup(unit, access_port_1, &access_port_1_eid)) {
	printf("ingress_port_setup() failed for port %d\n", access_port_1);
	return ;
	}

	if (BCM_E_NONE != egress_port_setup(unit, network_port_1, &network_port_1_eid)) {
	printf("egress_port_setup() failed for port %d\n", network_port_1);
	return ;
	}	   

	if (BCM_E_NONE != egress_port_setup(unit, network_port_2, &network_port_2_eid)) {
	printf("egress_port_setup() failed for port %d\n", network_port_2);
	return ;
	}	   

	/* sent DLF packet */
	snprintf(str, 512, "tx 1 pbm=%d DATA=0x0000000011BB00000000BBBB810000C808004500002E000100004000DB7CC0A80A01C0A81401000102030405060708090A0B0C0D0E0F10111213141516171819", access_port_1);
	bshell(unit, str); 			 

	bshell(0, "sleep 1");

	bshell(0, "l2 show ");
	bshell(0, "sleep 1");
	bshell(0, "show c ");

	/************************************************************************************************************/
	printf(" \n **************TEST 2  UC Initiation-UC : access port 1  uc to network port 1**************\n");   
	bshell(0, "l2 show ");

	bshell(0, "add mac");
	bcm_mac_t payload_da_1 = "00:00:00:00:11:bb";
	add_to_l2_table(unit, payload_da_1, vpn_id, net_vxlan_port_1, 0);

	/* 	  
	* 	  0000000011BB00000000BBBB810000C8
	* 	  08004500002E000100004000DB7CC0A8
	* 	  0A01C0A8140100010203040506070809
	* 	  0A0B0C0D0E0F10111213141516171819
	*/

	if (BCM_E_NONE != ingress_port_setup(unit, access_port_1, &access_port_1_eid)) {
	printf("ingress_port_setup() failed for port %d\n", access_port_1);
	return ;
	}

	if (BCM_E_NONE != egress_port_setup(unit, network_port_1, &network_port_1_eid)) {
	printf("egress_port_setup() failed for port %d\n", network_port_1);
	return ;
	}	   

	/* sent DLF packet */
	snprintf(str, 512,  "tx 1 pbm=%d DATA=0x0000000011BB00000000BBBB810000C808004500002E000100004000DB7CC0A80A01C0A81401000102030405060708090A0B0C0D0E0F10111213141516171819", access_port_1);
	bshell(unit, str);
	bshell(0, "sleep 1");
	bshell(0, "l2 show ");
	bshell(0, "sleep 1");
	bshell(0, "show c ");

	/************************************************************************************************************/
	printf(" \n **************TEST 3 UC termination: network port 1 to access port 1 **************\n");	  
	bshell(0, "l2 show ");

	/* 
	*		 0000000022220000000000028100012C
	*		 0800450000600000000040113E6E1414
	*		 14010A0A0A01FFFF2118004C00000800
	*		 00000123450000000000BBBB00000000
	*		 11BB08004500002E000000004000DB7D  No inner tag
	*		 C0A81401C0A80A010001020304050607
	*		 08090A0B0C0D0E0F1011121314151617
	*		 1819
	*/

	if (BCM_E_NONE != ingress_port_setup(unit, network_port_1, &network_port_1_eid)) {
	printf("ingress_port_setup() failed for port %d\n", network_port_1);
	return ;
	}

	if (BCM_E_NONE != egress_port_setup(unit, access_port_1, &access_port_1_eid )) {
	printf("egress_port_setup() failed for port %d\n", access_port_1);
	return ;
	}	   


	snprintf(str, 512,  "tx 1 pbm=%d DATA=0x0000000022220000000000028100012C0800450000600000000040113E6E141414010A0A0A01FFFF2118004C0000080000000123450000000000BBBB0000000011BB08004500002E000000004000DB7DC0A81401C0A80A01000102030405060708090A0B0C0D0E0F1011121314151617", network_port_1);
	bshell(unit, str); 
	bshell(0, "sleep 1");
	bshell(0, "l2 show ");
	bshell(0, "sleep 1");
	bshell(0, "show c ");

	/************************************************************************************************************/
	printf(" \n **************TEST4  UnKnown MC Initiation: access port 1  mc to network port 2 and 3 **************\n");	  
	bshell(0, "l2 show ");

	/*
	*		 01005E01010100000000BBBB810000C8
	*		 08004500002E000100004000CF23C0A8
	*		 0A01E001010100010203040506070809
	*		 0A0B0C0D0E0F10111213141516171819
	*/ 	 

	if (BCM_E_NONE != ingress_port_setup(unit, access_port_1, &access_port_1_eid)) {
	printf("ingress_port_setup() failed for port %d\n", access_port_1);
	return ;
	}

	if (BCM_E_NONE != egress_port_setup(unit, network_port_1, &network_port_1_eid)) {
	printf("egress_port_setup() failed for port %d\n", network_port_1);
	return ;
	}	   

	if (BCM_E_NONE != egress_port_setup(unit, network_port_2, &network_port_2_eid)) {
	printf("egress_port_setup() failed for port %d\n", network_port_2);
	return ;
	}	    


	snprintf(str, 512, "tx 1 pbm=%d DATA=0x01005E01010100000000BBBB810000C808004500002E000100004000CF23C0A80A01E0010101000102030405060708090A0B0C0D0E0F10111213141516171819", access_port_1);
	bshell(unit, str); 			 

	bshell(0, "sleep 1");

	bshell(0, "l2 show ");
	bshell(0, "sleep 1");
	bshell(0, "show c ");


	/************************************************************************************************************/
	printf(" \n **************TEST5  Known MC Termination network port 1 to access port 1  **************\n");  
	bshell(0, "l2 show ");

	/*
	*		 0000000022220000000000028100012C
	*		 0800450000600000000040113E6E1414
	*		 14010A0A0A01FFFF2118004C00000800
	*		 00000123450001005E01010100000000
	*		 11BB08004500002E000000004000C524
	*		 C0A81401E00101010001020304050607
	*		 08090A0B0C0D0E0F1011121314151617
	*		 1819
	*/ 	 



	if (BCM_E_NONE != ingress_port_setup(unit, network_port_1, &network_port_1_eid)) {
	printf("ingress_port_setup() failed for port %d\n", network_port_1);
	return ;
	}

	if (BCM_E_NONE != egress_port_setup(unit, access_port_1, &access_port_1_eid )) {
	printf("egress_port_setup() failed for port %d\n", access_port_1);
	return ;
	}	   

	snprintf(str, 512,  "tx 1 pbm=%d DATA=0x0000000022220000000000028100012C0800450000600000000040113E6E141414010A0A0A01FFFF2118004C0000080000000123450001005E0101010000000011BB08004500002E000000004000C524C0A81401E0010101000102030405060708090A0B0C0D0E0F10111213141516171819", network_port_1);
	bshell(unit, str); 
	bshell(0, "sleep 1");
	bshell(0, "l2 show ");
	bshell(0, "sleep 1");
	bshell(0, "show c ");


	/************************************************************************************************************/


	printf(" \n **************destroy the ifp rules************** \n"); 
	destroy_ifp_to_cpu_rule(unit,access_port_1_eid);
	destroy_ifp_to_cpu_rule(unit,network_port_1_eid);
	destroy_ifp_to_cpu_rule(unit,network_port_2_eid);	   

	return; 
 }
 
 /**************************5 verify functions end ***********************/
 
 
 
 ////////////////////////////////////////////////////////////////////////////////////////
 /*
  * This function does the following
  * a)test setup
  * b)actual configuration (Done in config_vxlan_access_to_network_multicast())
  * c)demonstrates the functionality(done in verify()).
  */
 
 bcm_error_t execute()
 {
   bcm_error_t rv;
   int unit =0;
 
   
   printf("**************************** basic information begin****************************\n", bcm_errmsg(rv));	   
   bshell(unit, "config show; a ; version; cancun stat");
   
   printf("**************************** basic information end****************************\n\n", bcm_errmsg(rv));	   
   printf("**************************** test setup begin****************************\n", bcm_errmsg(rv));	   
 
   if (BCM_FAILURE((rv = test_setup(unit)))) {
	 printf("test_setup() failed.\n");
	 return -1;
   }
   printf("**************************** test setup end****************************\n\n", bcm_errmsg(rv));	   
   printf("****************************run main script begin****************************\n", bcm_errmsg(rv));	   
 
   if (BCM_FAILURE((rv = vxlan_access_to_network_multicast_mesh(unit)))) {
	 printf("vxlan_access_to_access_multicast() Failed\n");
	 return -1;
   }
   printf("****************************run main script end****************************\n\n", bcm_errmsg(rv));	   
   printf("****************************Verify begin****************************\n", bcm_errmsg(rv));	   
 
   verify(unit);
   
   printf("****************************Verify end****************************\n", bcm_errmsg(rv));	   
   return BCM_E_NONE;
 }
 
 const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
 if (!sal_strcmp(auto_execute, "YES")) {
   print execute();
 }

