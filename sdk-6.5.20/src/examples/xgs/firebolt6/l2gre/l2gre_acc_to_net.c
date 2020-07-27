/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : L2GRE Access to Network - Unicast
 *
 *  Usage    : BCM.0> cint l2gre_acc_to_net.c
 *
 *  config   : l2gre_config.bcm
 *
 *  Log file : l2gre_acc_to_net_log.txt
 *
 *  Test Topology :
 *
 *                   +------------------------------+  egress_port1
 *                   |                              +-----------------+  (Ethernet .1Q frame)
 *                   |                              |
 *                   |                              |
 *  cpu_port         |                              |  egress_port2
 *  +----------------+          SWITCH              +-----------------+  (Payload encapsulated in L2GRE header)
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   +------------------------------+
 *  Summary:
 *  ========
 *    This CINT script demonstrates how to configure chip to encap L2UC packet with L2GRE tunnel. 
 *
 *    Detailed steps done in the CINT script:
 *    =====================================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Select three egress port and configure them in loopback mode.
 *      b) Create FP group 
 *      c) Start Packet Watcher diag app (PW start)
 *
 *    2) Step2 - Configuration (Done in l2gre_setup())
 *    ============================================
 *      a) Configure L2GRE for L2UC 
 *
 *    3) Step3 - Verification(Done in verify())
 *    ==========================================
 *      a) Transmit one Ethernet packet(unknown-UC) from CPU to Access port(loopback)
 *      b) Transmit one L2GRE packet(known-UC) from CPU to Netwoek port(loopback)
 *      c) Transmit one Ethernet packet(known-UC) from CPU to Access port(loopback)
 *      d) Expected Result:
 *      ===================
 *         The packets are looped back on egress ports and are received by CPU port.
 *         PW will dump the recieved Encap./Decap. L2GRE packets on console. 
 *
 * -----------------------------------------------------------------------
 * Per VFI/VPN, a multicast GRE tunnel is created for DLF traffic,
 * only one multicast packet is sent out on the selected network port
 * even if there are more than one network port.
 * 
 * Test 1) Initiation
 * 
 * Input to access:
 * 
 * tx port=1 data=0000000011BB0000000011AA8100001508004500002E0000000040FF9B130A0A0A0BC0A80A01000102030405060708090A0B0C0D0E0F101112131415161718193B07EA0A
 * 
 * 00 00 00 00 11 BB 00 00 00 00 11 AA 81 00 00 15
 * 08 00 45 00 00 2E 00 00 00 00 40 FF 9B 13 0A 0A
 * 0A 0B C0 A8 0A 01 00 01 02 03 04 05 06 07 08 09
 * 0A 0B 0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19
 * 
 * Output on network if DA is in L2 table:
 * 
 * 00 00 00 00 00 02 00 00 00 00 22 22 81 00 00 16  -->outer L2 header
 * 08 00 45 00 00 5C 00 02 00 00 10 2F D4 BD 0A 0A  -->IP4 header
 * 0A 01 C0 A8 01 01 
 * 20 00 65 58 12 34 50 00                          --> GRE header & GRE key
 * 00 00 00 00 11 BB 00 00 00 00 11 AA 81 00 00 15  --> original L2 frame
 * 08 00 45 00 00 2E 00 00 00 00 40 FF 9B 13 0A 0A
 * 0A 0B C0 A8 0A 01 00 01 02 03 04 05 06 07 08 09
 * 0A 0B 0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19
 * 
 * or the following if DLF (no L2 is configured)
 * 
 * 01 00 5E 00 00 0A 00 00 00 00 22 22 81 00 00 16  -->outer L2 header (DLF DA)
 * 08 00 45 00 00 5C 00 00 00 00 10 2F B6 5E 0A 0A  -->IP4 header
 * 0A 01 E0 00 00 0A
 * 20 00 65 58 12 34 50 00
 * 00 00 00 00 11 BB 00 00 00 00 11 AA 81 00 00 15  --> original L2 frame
 * 08 00 45 00 00 2E 00 00 00 00 40 FF 9B 13 0A 0A
 * 0A 0B C0 A8 0A 01 00 01 02 03 04 05 06 07 08 09
 * 0A 0B 0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19
 * 
 * Test 2) Termination
 * 
 * Input on network port:
 * 
 * tx port=2 data=0000000022220000000000028100001608004500005C00000000402FA4BFC0A801010A0A0A0120006558123450000000000011AA0000000011BB8100001508004500002E0000000040FF9B130A0A0A0BC0A80A01000102030405060708090A0B0C0D0E0F101112131415161718197C856BB4
 * 
 * 00000000222200000000000281000016
 * 08004500005C00000000402FA4BFC0A801010A0A0A01 --> IP4 header
 * 2000655812345000                             --> GRE header + GRE key
 * 0000000011AA0000000011BB81000015             --> L2 frame
 * 08004500002E0000000040FF9B130A0A
 * 0A0BC0A80A0100010203040506070809
 * 0A0B0C0D0E0F10111213141516171819
 * 7C856BB4
 * 
 * Output on access port:
 * 
 * 00 00 00 00 11 AA 00 00 00 00 11 BB 81 00 00 15
 * 08 00 45 00 00 2E 00 00 00 00 40 FF 9B 13 0A 0A
 * 0A 0B C0 A8 0A 01 00 01 02 03 04 05 06 07 08 09
 * 0A 0B 0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19
 */
cint_reset();
int unit = 0;
bcm_port_t acc_port = 1;
bcm_port_t net_port = 2;
 

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


/* FP Group/Rule Creation/Destruction
*/

bcm_field_group_config_t group_config;
bcm_field_entry_t eid;
        
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
bcm_error_t test_setup(int unit)
{
    int port_list[2];

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 2)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    acc_port = port_list[0];
    net_port = port_list[1];

    /* Put all egress ports into loopback */
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, acc_port, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, net_port, BCM_PORT_LOOPBACK_MAC));

    /* Create FP group */
    BCM_IF_ERROR_RETURN(create_ifp_group(unit, &group_config));
    
    /* Start the packet watcher (quiet mode) */
    bshell(unit, "pw start report +raw");

    return BCM_E_NONE;    
}


/************************ L2GRE common functions ***************************/

bcm_error_t
do_l2gre_global_setting(int unit)
{
    /* Enable L3 Egress Mode */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3EgressMode, TRUE));

    /* Enable L2GRE Protocol-Type */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL2GreProtocolType, l2gre_proto_type));

    /* Enable L2GRE VPNID Size */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL2GreVpnIdSizeSet, 20));

    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL2StaticMoveToCpu, 1));

    /* Enable Vlan Translate per Vlan */
    BCM_IF_ERROR_RETURN(bcm_vlan_control_set(unit, bcmVlanTranslate, 1));
    return BCM_E_NONE;
}

bcm_error_t
do_l2gre_access_port_settings(int unit, bcm_port_t a_port)
{
    /* Enable ingress vlan translation */
    BCM_IF_ERROR_RETURN( bcm_vlan_control_port_set(unit, a_port, bcmVlanTranslateIngressEnable, 1));

    /* Should disable Vxlan Processing on access port */
    BCM_IF_ERROR_RETURN( bcm_port_control_set(unit, a_port, bcmPortControlL2GreEnable, 0));

    /* Should disable Tunnel Based Vxlan-VnId lookup */
    BCM_IF_ERROR_RETURN( bcm_port_control_set(unit, a_port, bcmPortControlL2GreTunnelbasedVpnId, 0));

    return BCM_E_NONE;
}

bcm_error_t
do_l2gre_net_port_settings(int unit, bcm_port_t n_port)
{
    /* Enable L2GRE Processing on network port */
    BCM_IF_ERROR_RETURN( bcm_port_control_set(unit, n_port, bcmPortControlL2GreEnable, 1));

    /* Allow tunnel based L2GRE-VNID lookup */
    BCM_IF_ERROR_RETURN( bcm_port_control_set(unit, n_port, bcmPortControlL2GreTunnelbasedVpnId, 0));
                              /* Only BCM_L2GRE_PORT_MATCH_VN_ID at network port */

    return BCM_E_NONE;
}

bcm_error_t
vlan_create_add_port(int unit, int vlan, int port)
{
    bcm_pbmp_t pbmp, upbmp;

    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vlan));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    BCM_IF_ERROR_RETURN( bcm_vlan_port_add(unit, vlan, pbmp, upbmp));
    
    return BCM_E_NONE;
}

bcm_error_t 
create_l3_interface(int unit, int intf_id, bcm_mac_t local_mac, int vid)
{
    bcm_l3_intf_t l3_intf;

    /* L3 interface and egress object for access */
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_flags = BCM_L3_WITH_ID | BCM_L3_ADD_TO_ARL;
    l3_intf.l3a_intf_id = intf_id;
    sal_memcpy(l3_intf.l3a_mac_addr, local_mac, sizeof(local_mac));
    l3_intf.l3a_vid = vid;
    BCM_IF_ERROR_RETURN( bcm_l3_intf_create(unit, &l3_intf));
    
    return BCM_E_NONE;    
}

bcm_error_t
create_egr_obj(int unit, uint32 flag, int l3_if, bcm_mac_t nh_mac, bcm_gport_t gport,
              int vid, bcm_if_t *egr_obj_id)
{
    bcm_l3_egress_t l3_egress;

    bcm_l3_egress_t_init(&l3_egress);
    l3_egress.flags = BCM_L3_L2GRE_ONLY | flag;
    l3_egress.intf  = l3_if;
    sal_memcpy(l3_egress.mac_addr,  nh_mac, sizeof(nh_mac));
    l3_egress.vlan = vid;
    l3_egress.port = gport;
    BCM_IF_ERROR_RETURN( bcm_l3_egress_create(unit, flag, &l3_egress, egr_obj_id));
    
    return BCM_E_NONE;
}

/*
 * Create L2GRE Tunnel Initiator
 */
bcm_error_t
tunnel_initiator_setup(bcm_ip_t lip, bcm_ip_t rip, int ttl, int *tid)
{
    bcm_tunnel_initiator_t tnl_init;

    bcm_tunnel_initiator_t_init(&tnl_init);
    tnl_init.type  = bcmTunnelTypeL2Gre;
    tnl_init.ttl = ttl;
    tnl_init.sip = lip;
    tnl_init.dip = rip;
    BCM_IF_ERROR_RETURN( bcm_l2gre_tunnel_initiator_create(unit, &tnl_init));
    *tid = tnl_init.tunnel_id;
    
    return BCM_E_NONE;
}

/*
 * Create L2GRE Tunnel Terminator
 */
bcm_error_t
tunnel_terminator_setup(bcm_ip_t rip, bcm_ip_t lip, int tunnel_init_id, int *term_id)
{
    bcm_tunnel_terminator_t tnl_term;

    bcm_tunnel_terminator_t_init(&tnl_term);
    tnl_term.type = bcmTunnelTypeL2Gre;
    tnl_term.sip = rip;    /* For MC tunnel, Don't care */
    tnl_term.dip = lip;
    tnl_term.tunnel_id = tunnel_init_id;
    tnl_term.flags = BCM_TUNNEL_TERM_TUNNEL_WITH_ID;
    BCM_IF_ERROR_RETURN( bcm_l2gre_tunnel_terminator_create(unit, &tnl_term));
    *term_id = tnl_term.tunnel_id;

    return BCM_E_NONE;
}

bcm_error_t
create_l2gre_vpn(int vpn_id, int vnid, bcm_multicast_t bc,
                 bcm_multicast_t mc, bcm_multicast_t uuc)
{
    bcm_l2gre_vpn_config_t vpn_info;

    bcm_l2gre_vpn_config_t_init(&vpn_info);
    vpn_info.flags = BCM_L2GRE_VPN_ELAN | BCM_L2GRE_VPN_WITH_ID | BCM_L2GRE_VPN_WITH_VPNID;
    vpn_info.vpn  = vpn_id;
    vpn_info.vpnid = vnid;
    vpn_info.broadcast_group         = bc;
    vpn_info.unknown_multicast_group = mc;
    vpn_info.unknown_unicast_group   = uuc;
    BCM_IF_ERROR_RETURN( bcm_l2gre_vpn_create(unit, &vpn_info));
    print vpn_info.vpn;
    return BCM_E_NONE;
}

bcm_error_t
create_l2gre_acc_vp(int unit, bcm_vpn_t vpn, bcm_gport_t gport,
                    bcm_l2gre_port_match_t criteria,
                    bcm_if_t egr_obj, bcm_vlan_t vid, bcm_gport_t *vp)
{
    bcm_l2gre_port_t l2gre_port;

    bcm_l2gre_port_t_init(&l2gre_port);
    l2gre_port.flags = BCM_L2GRE_PORT_SERVICE_TAGGED;
    l2gre_port.match_port =        gport;
    l2gre_port.criteria =          criteria;
    l2gre_port.egress_if =         egr_obj;
    l2gre_port.match_vlan =        vid;
    BCM_IF_ERROR_RETURN( bcm_l2gre_port_add(unit, vpn, &l2gre_port));
    *vp = l2gre_port.l2gre_port_id;

    return BCM_E_NONE;
}

bcm_error_t
create_l2gre_net_vp(int unit, bcm_vpn_t vpn, uint32 flags, bcm_gport_t gport,
                    bcm_l2gre_port_match_t criteria, bcm_if_t egr_obj,
                    bcm_gport_t tun_init, bcm_gport_t tun_term, bcm_gport_t *vp)
{
    bcm_l2gre_port_t l2gre_port;

    bcm_l2gre_port_t_init(&l2gre_port);
    l2gre_port.flags = BCM_L2GRE_PORT_NETWORK | BCM_L2GRE_PORT_EGRESS_TUNNEL |
                       BCM_L2GRE_PORT_SERVICE_TAGGED | flags;
    l2gre_port.match_port =        gport;
    l2gre_port.criteria =          criteria;
    l2gre_port.egress_if =         egr_obj;
    l2gre_port.egress_tunnel_id =  tun_init;
    l2gre_port.match_tunnel_id =   tun_term;
    /* vpn_id parameter is not care for net VP */
    BCM_IF_ERROR_RETURN( bcm_l2gre_port_add(unit, vpn, &l2gre_port));
    *vp = l2gre_port.l2gre_port_id;

    return BCM_E_NONE;
}

bcm_error_t
add_to_l2_station(int unit, bcm_mac_t mac, bcm_vlan_t vid)
{
    bcm_l2_station_t l2_station;
    int station_id;

    bcm_l2_station_t_init(&l2_station);
    sal_memcpy(l2_station.dst_mac, mac, sizeof(mac));
    l2_station.dst_mac_mask = "ff:ff:ff:ff:ff:ff";
    l2_station.vlan         = vid;
    l2_station.vlan_mask    = 0xfff;
    l2_station.flags   = BCM_L2_STATION_IPV4;
    BCM_IF_ERROR_RETURN( bcm_l2_station_add(unit, &station_id, &l2_station));

    return BCM_E_NONE;
}

bcm_error_t
add_to_l2_table(int unit, bcm_mac_t mac, bcm_vpn_t vpn_id, int port)
{
    bcm_l2_addr_t l2_addr; 

    bcm_l2_addr_t_init(&l2_addr, mac, vpn_id);
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = port;
    BCM_IF_ERROR_RETURN( bcm_l2_addr_add(unit, &l2_addr));
    
    return BCM_E_NONE;
}


/************************ L2GRE Setup ***************************/


/* VPN */
bcm_vpn_t vpn_id = 0x7010;
uint32 vnid = 0x12345;
bcm_multicast_t bc_group = 0;

/* Used to create a dummy VPN that is used to create network VP */
int rsvd_network_vpn = 0x7001;
int rsvd_network_vpn_vnid = 0xeeee;
 
/*
 * Access side
 */
/* Access side MAC addresses are not really used */
bcm_mac_t acc_dummy_mac = "00:00:01:00:00:01";

bcm_vlan_t acc_vid = 21;
bcm_if_t acc_intf_id = 1;
bcm_gport_t acc_gport = BCM_GPORT_INVALID;
bcm_if_t acc_egr_obj;
int acc_l2gre_port;
bcm_if_t acc_encap_id;

/*
 * Network side
 */
bcm_vlan_t net_vid = 22;
bcm_if_t net_intf_id = 2;
bcm_gport_t net_gport = BCM_GPORT_INVALID;
bcm_if_t net_egr_obj;
int net_l2gre_port;

/* UC Tunnel */
uint8 ttl = 16;
uint16 l2gre_proto_type = 0x6558;
uint16 udp_sp = 0xffff;
bcm_mac_t net_local_mac = "00:00:00:00:22:22";
bcm_mac_t net_remote_mac = "00:00:00:00:00:02";
bcm_ip_t tnl_local_ip = 0x0a0a0a01;     /* 10.10.10.1 */
bcm_ip_t tnl_remote_ip = 0xC0A80101;    /* 192.168.1.1 */
int tunnel_init_id, tunnel_term_id;

/*
 * DLF/BC tunnel and virtual port
 */
bcm_mac_t dlf_mac = "01:00:5e:00:00:0A"; /* 224.0.0.10 */
bcm_ip_t tnl_dlf_dip = 0xe000000A; /* 224.0.0.10 */
bcm_if_t egr_obj_dlf;
int l2gre_port_dlf;
bcm_if_t encap_id_dlf;
int tnl_dlf_init_id, tnl_dlf_term_id;

/* Unicast payment MAC */
bcm_mac_t payload_sa = "00:00:00:00:11:aa";
bcm_mac_t payload_da = "00:00:00:00:11:bb";

bcm_error_t 
l2gre_setup(int unit)
{
    /*
     * Global settings
     */
    BCM_IF_ERROR_RETURN(do_l2gre_global_setting(unit));
    /*
     * Access port settings
     */
    BCM_IF_ERROR_RETURN(do_l2gre_access_port_settings(unit, acc_port));
    /*
     * Network port settings
     */
    BCM_IF_ERROR_RETURN(do_l2gre_net_port_settings(unit, net_port));

    /*
     * Convert to GPORT
     */
    BCM_IF_ERROR_RETURN( bcm_port_gport_get(unit, acc_port, &acc_gport)); 
    BCM_IF_ERROR_RETURN( bcm_port_gport_get(unit, net_port, &net_gport)); 

    /* Multicast group for the segment MC/BC */
    BCM_IF_ERROR_RETURN( bcm_multicast_create(unit, BCM_MULTICAST_TYPE_L2GRE, &bc_group));
    /* Create L2GRE VPN */
    BCM_IF_ERROR_RETURN( create_l2gre_vpn(vpn_id, vnid, bc_group, bc_group, bc_group));
    /* Create L2GRE VPN for assigning Network-VP */
    BCM_IF_ERROR_RETURN( create_l2gre_vpn(rsvd_network_vpn, rsvd_network_vpn, bc_group, bc_group, bc_group));
    
    /*
     * Create access side L2GRE port
     */
    
    /* VLAN settings */
    BCM_IF_ERROR_RETURN( vlan_create_add_port(unit, acc_vid, acc_port));
    
    /* L3 interface and egress object for access - note use a dummy MAC address */
    BCM_IF_ERROR_RETURN( create_l3_interface(unit, acc_intf_id, acc_dummy_mac, acc_vid));
    BCM_IF_ERROR_RETURN( create_egr_obj(unit, 0, acc_intf_id, acc_dummy_mac, acc_gport, acc_vid, &acc_egr_obj));
    
    /* Create L2GRE VP for access port */
    BCM_IF_ERROR_RETURN( create_l2gre_acc_vp(unit, vpn_id, acc_gport, BCM_L2GRE_PORT_MATCH_PORT,
                        acc_egr_obj, acc_vid, &acc_l2gre_port));
    print acc_l2gre_port;
    /* 
     * Network port
     */
    
    /* VLAN settings */
    BCM_IF_ERROR_RETURN( vlan_create_add_port(unit, net_vid, net_port));
    /* L3 interface and egress object for network */
    BCM_IF_ERROR_RETURN( create_l3_interface(unit, net_intf_id, net_local_mac, net_vid));
    BCM_IF_ERROR_RETURN( create_egr_obj(unit, 0, net_intf_id, net_remote_mac, net_gport, net_vid, &net_egr_obj));
    
    /* Tunnel Setup (Initiator & Terminator - UC) */
    BCM_IF_ERROR_RETURN( tunnel_initiator_setup(tnl_local_ip, tnl_remote_ip, ttl, &tunnel_init_id));
    BCM_IF_ERROR_RETURN( tunnel_terminator_setup(tnl_remote_ip, tnl_local_ip, tunnel_init_id, &tunnel_term_id));
    
    /* Create UC L2GRE VP for network port */
    BCM_IF_ERROR_RETURN( create_l2gre_net_vp(unit, rsvd_network_vpn, 0, net_gport, BCM_L2GRE_PORT_MATCH_VPNID,
                        net_egr_obj, tunnel_init_id, tunnel_term_id, &net_l2gre_port));
    print net_l2gre_port;
    
    /* Station MAC set up */
    BCM_IF_ERROR_RETURN( add_to_l2_station(unit, net_local_mac, net_vid));
    
    /*
     * DLF/BC network port set up
     */
    
    /* Egress object for non-UC L2GRE VP, use same interface as UC L2GRE network */
    BCM_IF_ERROR_RETURN( create_egr_obj(unit, BCM_L3_IPMC, net_intf_id, dlf_mac, net_gport, net_vid, &egr_obj_dlf));
    
    /* Tunnel Setup (Initiator & Terminator - non-UC) */
    BCM_IF_ERROR_RETURN( tunnel_initiator_setup(tnl_local_ip, tnl_dlf_dip, ttl, &tnl_dlf_init_id));
    BCM_IF_ERROR_RETURN( tunnel_terminator_setup(tnl_remote_ip, tnl_dlf_dip, tnl_dlf_init_id, &tnl_dlf_term_id));
    
    /* Create non-UC L2GRE VP for network port */
    BCM_IF_ERROR_RETURN( create_l2gre_net_vp(unit, rsvd_network_vpn, BCM_L2GRE_PORT_MULTICAST, net_gport,
                        BCM_L2GRE_PORT_MATCH_VPNID, egr_obj_dlf,
                        tnl_dlf_init_id, tnl_dlf_term_id, &l2gre_port_dlf));
    print l2gre_port_dlf;
    
    /*
     * MC group set up - MC group should contain all Access ports and Network non-UC port
     */
    BCM_IF_ERROR_RETURN( bcm_multicast_l2gre_encap_get(unit, bc_group, net_gport, l2gre_port_dlf, &encap_id_dlf));
    BCM_IF_ERROR_RETURN( bcm_multicast_egress_add(unit, bc_group, l2gre_port_dlf, encap_id_dlf));
    
    BCM_IF_ERROR_RETURN( bcm_multicast_l2gre_encap_get(unit, bc_group, acc_gport, acc_l2gre_port, &acc_encap_id));
    BCM_IF_ERROR_RETURN( bcm_multicast_egress_add(unit, bc_group, acc_l2gre_port, acc_encap_id));
    
    return BCM_E_NONE;    
}

void
port_discard(int port, int discard)
{
    char buf[50];
    sprintf(buf, "port %d discard=%s", port, (discard)?"All":"none");
    bshell (unit, buf);    
}

int _2_hex_char_2_one_int(char *c) {
    int value;
    if (*c >= '0' && *c <= '9')
	    value = (*c - '0');
    else if (*c >= 'a' && *c <= 'f')
	    value = (*c - 'a') + 10;
    else if (*c >= 'A' && *c <= 'F')
	    value = (*c - 'A') + 10;
	value = value * 16;   
	c++;
    if (*c >= '0' && *c <= '9')
	    value += (*c - '0');
    else if (*c >= 'a' && *c <= 'f')
	    value += (*c - 'a') + 10;
    else if (*c >= 'A' && *c <= 'F')
	    value += (*c - 'A') + 10;	 
    return value;
}

void
tx_packet_data_from_cpu(int unit, int port, int packet_count, int pkt_size, char *data)
{
    bcm_pkt_t *pkt;
    int flags = BCM_TX_CRC_APPEND; 
    int untagged = 0; 
    int i;
                
    if (BCM_E_NONE != bcm_pkt_alloc(unit, pkt_size, flags, &pkt)) {
        printf("TX: Failed to allocate packets\n");
    } 
    for (i=0; i < pkt_size; i++){
        pkt->_pkt_data.data[i] = _2_hex_char_2_one_int(data[i*2]);
	}
    BCM_PKT_PORT_SET(&pkt, port, untagged, 0);
    pkt->call_back = NULL;
    if (BCM_E_NONE != bcm_tx(unit, pkt, NULL)) {                            
            printf("bcm tx error\n");
    }    
    bcm_pkt_free(unit, pkt);
}



/*
 * Test Harness
 *
 * Demonstrate use of L2GRE Encapsulation/Decapsulation.
 *
 * Verify behavior by sending packets from CPU -> ingress port(loopback) -> egress_port(loopback) -> CPU.
 */
bcm_error_t
test_harness(int unit)
{
    print "";
    print "==========  Test: L2GRE packet from Access port to Network port (Unknown UC - DLF) ==================";
    port_discard(net_port, 1);
    port_discard(acc_port, 0);
    print ">>> Input customer packet @ port-1:";    
    create_ifp_to_cpu_rule(unit, group_config.group, &eid, acc_port);
    tx_packet_data_from_cpu(unit, acc_port, 1, 72, "0000000011BB0000000011AA8100001508004500002E0000000040FF9B130A0A0A0BC0A80A01000102030405060708090A0B0C0D0E0F101112131415161718193B07EA0A");
    sal_sleep(2); /* wait for printing packet */
    print "";
    print ">>> Ouput L2GRE encapsulated packet @ port-2:";
    destroy_ifp_to_cpu_rule(unit, eid);
    create_ifp_to_cpu_rule(unit, group_config.group, &eid, net_port);
    tx_packet_data_from_cpu(unit, acc_port, 1, 72, "0000000011BB0000000011AA8100001508004500002E0000000040FF9B130A0A0A0BC0A80A01000102030405060708090A0B0C0D0E0F101112131415161718193B07EA0A");
    sal_sleep(2); /* wait for printing packet */       
    bshell(unit, "show c");
    bshell(unit, "sleep 1");
    bshell(unit, "clear c");
    print "";
    print "==========  Test: L2GRE packet from Network port to Access port (known UC) ==================";
    port_discard(net_port, 0);
    port_discard(acc_port, 1);
    print ">>> Input L2GRE packet @ port-2:";
    destroy_ifp_to_cpu_rule(unit, eid);
    create_ifp_to_cpu_rule(unit, group_config.group, &eid, net_port);
    tx_packet_data_from_cpu(unit, net_port, 1, 118, "0000000022220000000000028100001608004500005C00000000402FA4BFC0A801010A0A0A0120006558123450000000000011AA0000000011BB8100001508004500002E0000000040FF9B130A0A0A0BC0A80A01000102030405060708090A0B0C0D0E0F101112131415161718197C856BB4");
    sal_sleep(2); /* wait for printing packet */
    print "";    
    print ">>> Ouput L2GRE decapsulated packet @ port-1:";
    destroy_ifp_to_cpu_rule(unit, eid);
    create_ifp_to_cpu_rule(unit, group_config.group, &eid, acc_port);
    tx_packet_data_from_cpu(unit, net_port, 1, 118, "0000000022220000000000028100001608004500005C00000000402FA4BFC0A801010A0A0A0120006558123450000000000011AA0000000011BB8100001508004500002E0000000040FF9B130A0A0A0BC0A80A01000102030405060708090A0B0C0D0E0F101112131415161718197C856BB4");
    sal_sleep(2); /* wait for printing packet */
    bshell(unit, "show c");
    bshell(unit, "sleep 1");
    bshell(unit, "clear c");
    print "";
    print "==========  Test: L2GRE packet from Access port to Network port (known UC)==================";
    port_discard(net_port, 1);
    port_discard(acc_port, 0);    
    print ">>> Input customer packet @ port-1:";
    destroy_ifp_to_cpu_rule(unit, eid);
    create_ifp_to_cpu_rule(unit, group_config.group, &eid, acc_port);
    tx_packet_data_from_cpu(unit, acc_port, 1, 72, "0000000011BB0000000011AA8100001508004500002E0000000040FF9B130A0A0A0BC0A80A01000102030405060708090A0B0C0D0E0F101112131415161718193B07EA0A");
    sal_sleep(2); /* wait for printing packet */
    print "";    
    print ">>> Ouput L2GRE encapsulated packet @ port-2:";
    destroy_ifp_to_cpu_rule(unit, eid);
    create_ifp_to_cpu_rule(unit, group_config.group, &eid, net_port);
    tx_packet_data_from_cpu(unit, acc_port, 1, 72, "0000000011BB0000000011AA8100001508004500002E0000000040FF9B130A0A0A0BC0A80A01000102030405060708090A0B0C0D0E0F101112131415161718193B07EA0A");
    bshell(unit, "show c");
    bshell(unit, "sleep 1");

    return BCM_E_NONE;
}

void verify(int unit)
{
    bcm_error_t rv;
    if (BCM_FAILURE(rv = test_harness(unit))) {
        printf("Rx verification failed: %s\n", bcm_errmsg(rv));
    }
}

bcm_error_t execute()
{
    bcm_error_t rv;
    int unit =0;
    bshell(unit, "config show; a ; version; cancun stat");
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed %s.\n", bcm_errmsg(rv));
        return -1;
    }

    if (BCM_FAILURE((rv = l2gre_setup(unit)))) {
        printf("L2GRE Setup Failed %s.\n", bcm_errmsg(rv));
        return -1;
    }

    verify(unit);
    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}
