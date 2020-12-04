/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : Multicast IP-GRE tunnel
 *
 *  Usage    : BCM.0> cint l3_tunnel_mcast_ipgre.c
 *
 *  config   : ip_tunnel_config.bcm
 *
 *  Log file : l3_tunnel_mcast_ipgre_log.txt
 *
 *  Test Topology :
 *
 *                   +------------------------------+
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *   access_port     |                              |  network_port
 *  +----------------+          SWITCH              +-----------------+
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   +------------------------------+
 *  Summary:
 *  ========
 *
 *     This cint example demonstrates multicast IP-GRE tunnel initiation and termination
 *     using BCM APIs.
 *
 *  Detailed steps done in the CINT script:
 *  =======================================
 *    1) Step1 - Test Setup (Done in test_setup()):
 *    =============================================
 *      a) Selects two ports and configure them in Loopback mode. Out of these two ports,
 *         one port is used as access_port and the other as network port. Install a rule
 *         to copy incoming packets to CPU and start packet watcher.
 *
 *    2) Step2 - Configuration (Done in config_l3_tunnel_mcast_ipgre())
 *    =================================================================
 *      a) Create an access_vlan(21) and add access_port as member.
 *
 *      b) Create network_vlan(22) and add network_port as member.
 *
 *      c) Create two L3 interfaces one each for access side and network side.
 *
 *      d) Create two L3 egress objects one each for access side and network side.
 *
 *      e) Configure tunnel initiator(type = bcmTunnelTypeGre4In4)
 *         on network side L3 interface.
 *
 *      f) Configure tunnel terminator(type = bcmTunnelTypeGre4In4)
 *         to match on packet's tunnel header contents.
 *
 *      g) For tunnel initiation, add a L3 route pointing to network side egress object.
 *
 *      h) For tunnel termination, add a L3 route pointing to access side egress object.
 *
 *    3) Step3 - Verification (Done in verify()):
 *    ===========================================
 *      a) For multicast IP-GRE tunnel initiation, send the below packet on access_port.
 *
 *          Ethernet II, Src: 00:00:00_00:00:01 (00:00:00:00:00:01), Dst: IPv4mcast_01:01:01 (01:00:5e:01:01:01)
 *          802.1Q Virtual LAN, PRI: 0, CFI: 0, ID: 21
 *          Internet Protocol Version 4, Src: 1.1.1.1, Dst: 225.1.1.1
 *          Data (26 bytes)
 *          0000 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f ................
 *          0010 10 11 12 13 14 15 16 17 18 19 ..........
 *
 *          0100 5e01 0101 0000 0000 0001 8100 0015
 *          0800 4500 002e 0000 0000 40ff 95cd 0101
 *          0101 e101 0101 0001 0203 0405 0607 0809
 *          0a0b 0c0d 0e0f 1011 1213 1415 1617 1819
 *
*
 *      b)For IP-GRE Tunnel Termination, send the below packet on network_port
 *
 *          Ethernet II, Src: 00:00:00_00:00:02 (00:00:00:00:00:02), Dst: 00:00:00_00:22:22 (00:00:00:00:22:22)
 *          802.1Q Virtual LAN, PRI: 0, CFI: 0, ID: 22
 *          Internet Protocol Version 4, Src: 9.9.9.9, Dst: 10.10.10.10
 *          Generic Routing Encapsulation (IP)
 *          Internet Protocol Version 4, Src: 2.2.2.2, Dst: 225.1.1.1
 *          Data (2 bytes)
 *          0000 00 01 ..
 *
 *          0000 0000 2222 0000 0000 0002 8100 0016
 *          0800 4500 002e 0000 0000 402f 547c 0909
 *          0909 0a0a 0a0a 0000 0800 4500 0016 0000
 *          0000 40ff 93e3 0202 0202 e101 0101 0001
 *          0d44 1651 1cdf 4421
 *
 *       c) Expected Result:
 *       ===================
 *        After step 3.a, verify that the below IP-GRE tunnel packet egresses out
 *        of network_port.
 *
 *          Ethernet II, Src: 00:00:00_00:22:22 (00:00:00:00:22:22), Dst: 00:00:00_00:00:02 (00:00:00:00:00:02)
 *          802.1Q Virtual LAN, PRI: 0, CFI: 0, ID: 22
 *          Internet Protocol Version 4, Src: 10.10.10.10, Dst: 9.9.9.9
 *          Generic Routing Encapsulation (IP)
 *          Internet Protocol Version 4, Src: 1.1.1.1, Dst: 225.1.1.1
 *          Data (26 bytes)
 *          0000 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f ................
 *          0010 10 11 12 13 14 15 16 17 18 19 ..........
 *
 *          0000 0000 0002 0000 0000 2222 8100 0016
 *          0800 4500 0046 9f30 0000 162f df33 0a0a
 *          0a0a 0909 0909 0000 0800 4500 002e 0000
 *          0000 3fff 96cd 0101 0101 e101 0101 0001
 *          0203 0405 0607 0809 0a0b 0c0d 0e0f 1011
 *          1213 1415 1617 1819 4e14 86f2 93a9 64fc
 *
 *        After step 3.b, verify that the below packet egresses out of access_port.
 *
 *           Ethernet II, Src: 00:00:00_00:11:11 (00:00:00:00:11:11), Dst: IPv4mcast_01:01:01 (01:00:5e:01:01:01)
 *           802.1Q Virtual LAN, PRI: 0, CFI: 0, ID: 21
 *           Internet Protocol Version 4, Src: 2.2.2.2, Dst: 225.1.1.1
 *           Data (2 bytes)
 *           0000 00 01 ..
 *
 *           0100 5e01 0101 0000 0000 1111 8100 0015
 *           0800 4500 0016 0000 0000 3fff 94e3 0202
 *           0202 e101 0101 0001 0d44 1651 1cdf 4421
 *           0000 0000 0000 0000 0000 0000 f3f2 8a9c
 */

cint_reset();
bcm_port_t      access_port = 1;
bcm_port_t      network_port = 2;

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
 * Configures the port in loopback mode and installs
 * an IFP rule. This IFP rule copies the packets ingressing
 * on the specified port to CPU.
 */
bcm_error_t ingress_port_setup(int unit, bcm_port_t port)
{
  bcm_field_qset_t  qset;
  bcm_field_group_t group;
  bcm_field_entry_t entry;

  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_NONE));

  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);

  BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset, BCM_FIELD_GROUP_PRIO_ANY, &group));

  BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &entry));

  BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));

  BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
  return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode and installs
 * an IFP rule. This IFP rule copies the packets ingressing
 * on the specified port to CPU. Port is also configured
 * to discard all packets. This is to avoid continuous
 * loopback of the packet.
 */
bcm_error_t egress_port_setup(int unit, bcm_port_t port)
{
  bcm_field_qset_t  qset;
  bcm_field_group_t group;
  bcm_field_entry_t entry;

  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_ALL));

  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);

  BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset, BCM_FIELD_GROUP_PRIO_ANY, &group));

  BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &entry));

  BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));

  BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

  return BCM_E_NONE;
}
/*
 * This functions gets the port numbers and sets up ingress and
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
bcm_error_t test_setup(int unit)
{
  int port_list[2];

  if (BCM_E_NONE != portNumbersGet(unit, port_list, 2)) {
    printf("portNumbersGet() failed\n");
    return -1;
  }

  access_port = port_list[0];
  network_port = port_list[1];

  if (BCM_E_NONE != ingress_port_setup(unit, access_port)) {
    printf("ingress_port_setup() failed for port %d\n", access_port);
    return -1;
  }

  if (BCM_E_NONE != egress_port_setup(unit, network_port)) {
      printf("egress_port_setup() failed for port %d\n", network_port);
      return -1;
  }

  bshell(unit, "pw start report +raw +decode");
  return BCM_E_NONE;
}
/*
 * Configure multicast IP-GRE tunnel initiation and termination.
 */
bcm_error_t config_l3_tunnel_mcast_ipgre(int unit)
{
  bcm_error_t   rv;
  bcm_gport_t   access_gport, network_gport;
  bcm_l3_intf_t access_l3_intf;
  bcm_l3_intf_t network_l3_intf;
  bcm_if_t access_side_l3_egr_obj_id = 0;
  bcm_l3_egress_t access_side_l3_egr_obj;
  bcm_if_t network_side_l3_egr_obj_id = 0;
  bcm_l3_egress_t network_side_l3_egr_obj;
  bcm_ipmc_addr_t ipmc_addr;
  bcm_multicast_t ipmcast_group;
  bcm_if_t encap_id = 0;
  bcm_tunnel_initiator_t tnl_init_info;
  bcm_tunnel_terminator_t tnl_term_info;
  int tnl_term_flags;
  bcm_ip_t tnl_init_dip = 0xe1010101;  /* 225.1.1.1*/
  bcm_ip_t tnl_init_outer_sip = 0x0a0a0a0a; /* 10.10.10.10 */
  bcm_ip_t tnl_init_outer_dip = 0x09090909; /* 9.9.9.9 */
  bcm_ip_t tnl_term_outer_dip = 0x0a0a0a0a;
  bcm_ip_t tnl_term_outer_sip = 0x09090909;
  bcm_mac_t local_mac_1 = {0x00, 0x00, 0x00, 0x00, 0x11, 0x11};
  bcm_mac_t local_mac_2 = {0x00, 0x00, 0x00, 0x00, 0x22, 0x22};
  bcm_mac_t mc_mac = {0x01, 0x00, 0x5E, 0x01, 0x01, 0x01};
  bcm_mac_t remote_mac_2 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
  bcm_vrf_t   vrf = 0;
  bcm_vlan_t  access_vlan = 21;
  bcm_vlan_t  network_vlan = 22;
  int intf_id_1 = 21;
  int intf_id_2 = 22;
  int station_id;
  bcm_l2_station_t station;
  bcm_mac_t mac_mask = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};


  rv = bcm_port_gport_get(unit, access_port, &access_gport);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_port_gport_get(unit, network_port, &network_gport);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create access-side vlan*/
  rv = bcm_vlan_create(unit, access_vlan);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  /*Add access-port to access-vlan*/
  rv = bcm_vlan_gport_add(unit, access_vlan, access_gport, 0);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_gport_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create network-side vlan*/
  rv = bcm_vlan_create(unit, network_vlan);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /*Add network-port to network-vlan*/
  rv = bcm_vlan_gport_add(unit, network_vlan, network_gport, 0);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_gport_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Enable Egress Interface Mode */
  rv = bcm_switch_control_set(unit, bcmSwitchL3EgressMode, TRUE);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(.., bcmSwitchL3EgressMode, ..): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3IngressMode, TRUE));
  BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3IngressInterfaceMapSet, TRUE));

  /* Create access-side L3 interface*/
  bcm_l3_ingress_t ing_intf;
  bcm_if_t l3_intf_id;
  bcm_vlan_control_vlan_t vlan_info;

  bcm_l3_ingress_t_init(&ing_intf);
  ing_intf.flags = BCM_L3_INGRESS_WITH_ID;
  ing_intf.vrf = vrf;
  ing_intf.ipmc_intf_id = intf_id_1;
  l3_intf_id = intf_id_1;
  BCM_IF_ERROR_RETURN(bcm_l3_ingress_create(unit, &ing_intf, &l3_intf_id));
  bcm_vlan_control_vlan_t_init(&vlan_info);
  BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_get(unit, access_vlan, &vlan_info));
  vlan_info.ingress_if = l3_intf_id;
  BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_set(unit, access_vlan, vlan_info));

  bcm_l3_intf_t_init(&access_l3_intf);
  access_l3_intf.l3a_flags = BCM_L3_WITH_ID;
  access_l3_intf.l3a_intf_id = intf_id_1;
  sal_memcpy(access_l3_intf.l3a_mac_addr, local_mac_1, 6);
  access_l3_intf.l3a_vid = access_vlan;
  access_l3_intf.l3a_vrf = vrf;
  rv = bcm_l3_intf_create(unit, &access_l3_intf);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_intf_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  /* Create network-side L3 interface*/
  bcm_l3_ingress_t_init(&ing_intf);
  ing_intf.flags = BCM_L3_INGRESS_WITH_ID;
  ing_intf.vrf = vrf;
  ing_intf.ipmc_intf_id = intf_id_2;
  l3_intf_id = intf_id_2;
  BCM_IF_ERROR_RETURN(bcm_l3_ingress_create(unit, &ing_intf, &l3_intf_id));
  bcm_vlan_control_vlan_t_init(&vlan_info);
  BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_get(unit, network_vlan, &vlan_info));
  vlan_info.ingress_if = l3_intf_id;
  BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_set(unit, network_vlan, vlan_info));

  bcm_l3_intf_t_init(&network_l3_intf);
  network_l3_intf.l3a_flags = BCM_L3_WITH_ID;
  network_l3_intf.l3a_intf_id = intf_id_2;
  sal_memcpy(network_l3_intf.l3a_mac_addr, local_mac_2, 6);
  network_l3_intf.l3a_vid = network_vlan;
  network_l3_intf.l3a_vrf = vrf;
  rv =  bcm_l3_intf_create(unit, &network_l3_intf);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_intf_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  /* Add tunnel initiation */
  bcm_tunnel_initiator_t_init(&tnl_init_info);
  tnl_init_info.type = bcmTunnelTypeGre4In4;
  tnl_init_info.ttl  = 0x16;
  sal_memcpy(tnl_init_info.dmac, remote_mac_2, 6);
  tnl_init_info.dip = tnl_init_outer_dip;
  tnl_init_info.sip = tnl_init_outer_sip;
  rv = bcm_tunnel_initiator_set(unit, &network_l3_intf, &tnl_init_info);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_tunnel_initiator_set(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create network-side L3 egress object */
  bcm_l3_egress_t_init(&network_side_l3_egr_obj);
  network_side_l3_egr_obj.flags = BCM_L3_IPMC;
  network_side_l3_egr_obj.intf = network_l3_intf.l3a_intf_id;
  sal_memcpy(network_side_l3_egr_obj.mac_addr, remote_mac_2, 6);
  network_side_l3_egr_obj.vlan = network_vlan;
  network_side_l3_egr_obj.port = network_gport;
  rv = bcm_l3_egress_create(unit, 0, &network_side_l3_egr_obj, &network_side_l3_egr_obj_id);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_egress_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create access-side L3 egress object */
  bcm_l3_egress_t_init(&access_side_l3_egr_obj);
  access_side_l3_egr_obj.flags = BCM_L3_IPMC;
  access_side_l3_egr_obj.intf = access_l3_intf.l3a_intf_id;
  sal_memcpy(access_side_l3_egr_obj.mac_addr, mc_mac, 6);
  access_side_l3_egr_obj.vlan = access_vlan;
  access_side_l3_egr_obj.port = access_gport;
  rv = bcm_l3_egress_create(unit, 0, &access_side_l3_egr_obj, &access_side_l3_egr_obj_id);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_egress_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  /* Add tunnel termination */
  bcm_tunnel_terminator_t_init(&tnl_term_info);
  tnl_term_flags = BCM_TUNNEL_TERM_DSCP_TRUST |
                       BCM_TUNNEL_TERM_USE_OUTER_DSCP |
                       BCM_TUNNEL_TERM_USE_OUTER_TTL |
                       BCM_TUNNEL_TERM_KEEP_INNER_DSCP;
  tnl_term_info.flags = tnl_term_flags;
  tnl_term_info.type = bcmTunnelTypeGre4In4;
  tnl_term_info.vrf = BCM_L3_VRF_DEFAULT;
  tnl_term_info.vlan = network_vlan;
  tnl_term_info.sip = tnl_term_outer_sip;
  tnl_term_info.dip = tnl_term_outer_dip;
  tnl_term_info.sip_mask = bcm_ip_mask_create(32);
  tnl_term_info.dip_mask = bcm_ip_mask_create(32);
  BCM_PBMP_PORT_SET(tnl_term_info.pbmp, network_port);
  rv = bcm_tunnel_terminator_add(unit, &tnl_term_info);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_tunnel_terminator_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_L3, &ipmcast_group);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_multicast_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /*Add Multicast route entry for tunnel initiation*/
  bcm_ipmc_addr_t_init(&ipmc_addr);
  ipmc_addr.v = 1;
  ipmc_addr.flags = BCM_IPMC_SOURCE_PORT_NOCHECK;
  ipmc_addr.vid = access_vlan;
  ipmc_addr.mc_ip_addr = tnl_init_dip;
  ipmc_addr.group = ipmcast_group;
  ipmc_addr.port_tgid = access_gport;
  rv = bcm_ipmc_add(unit,&ipmc_addr);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_ipmc_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_multicast_egress_object_encap_get(unit, ipmcast_group, network_side_l3_egr_obj_id, &encap_id);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_multicast_l3_encap_get(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_multicast_egress_add(unit, ipmcast_group, network_gport, encap_id);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_multicast_egress_add() for network port: %s.\n", bcm_errmsg(rv));
      return rv;
  }
  rv = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_L3, &ipmcast_group);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_multicast_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  /*Add Multicast route entry for tunnel termination*/
  bcm_ipmc_addr_t_init(&ipmc_addr);
  ipmc_addr.flags = BCM_IPMC_SOURCE_PORT_NOCHECK;
  ipmc_addr.vid = network_vlan;
  ipmc_addr.mc_ip_addr = tnl_init_dip;
  ipmc_addr.group = ipmcast_group;
  rv = bcm_ipmc_add(unit,&ipmc_addr);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_ipmc_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  rv = bcm_multicast_egress_object_encap_get(unit, ipmcast_group, access_side_l3_egr_obj_id, &encap_id);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_multicast_l3_encap_get(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_multicast_egress_add(unit, ipmcast_group, access_gport, encap_id);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_multicast_egress_add() for access port: %s.\n", bcm_errmsg(rv));
      return rv;
  }

  bcm_l2_station_t_init(&station);
  station.flags = BCM_L2_STATION_IPV4;
  sal_memcpy(station.dst_mac, local_mac_2, sizeof(local_mac_2));
  sal_memcpy(station.dst_mac_mask, mac_mask, sizeof(mac_mask));
  station.vlan = network_vlan;
  station.vlan_mask = network_vlan ? 0xfff: 0x0;
  rv = bcm_l2_station_add(unit, &station_id, &station);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l2_station_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  return BCM_E_NONE;
}

void verify(int unit)
{
  char   str[512];

  bshell(unit, "hm ieee");

  /*
 *        For multicast IP-GRE tunnel initiation, send the below packet on access_port.
 *
 *        Ethernet II, Src: 00:00:00_00:00:01 (00:00:00:00:00:01), Dst: IPv4mcast_01:01:01 (01:00:5e:01:01:01)
 *        802.1Q Virtual LAN, PRI: 0, CFI: 0, ID: 21
 *        Internet Protocol Version 4, Src: 1.1.1.1, Dst: 225.1.1.1
 *        Data (26 bytes)
 *        0000 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f ................
 *        0010 10 11 12 13 14 15 16 17 18 19 ..........
 *
 *        0100 5e01 0101 0000 0000 0001 8100 0015
 *        0800 4500 002e 0000 0000 40ff 95cd 0101
 *        0101 e101 0101 0001 0203 0405 0607 0809
 *        0a0b 0c0d 0e0f 1011 1213 1415 1617 1819
 */
  printf("\nSending packet to access_port:%d\n", access_port);
  snprintf(str, 512, "tx 1 pbm=%d data=0x01005E0101010000000000018100001508004500002E0000000040FF95CD01010101E1010101000102030405060708090A0B0C0D0E0F101112131415161718194E1486F2; sleep quiet 1", access_port);
  bshell(unit, str);

  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, access_port, BCM_PORT_DISCARD_ALL));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, network_port, BCM_PORT_DISCARD_NONE));
  /*
 *       For IP-GRE Tunnel Termination, send the below packet on network_port
 *
 *        Ethernet II, Src: 00:00:00_00:00:02 (00:00:00:00:00:02), Dst: 00:00:00_00:22:22 (00:00:00:00:22:22)
 *        802.1Q Virtual LAN, PRI: 0, CFI: 0, ID: 22
 *        Internet Protocol Version 4, Src: 9.9.9.9, Dst: 10.10.10.10
 *        Generic Routing Encapsulation (IP)
 *        Internet Protocol Version 4, Src: 2.2.2.2, Dst: 225.1.1.1
 *        Data (2 bytes)
 *        0000 00 01 ..
 *
 *        0000 0000 2222 0000 0000 0002 8100 0016
 *        0800 4500 002e 0000 0000 402f 547c 0909
 *        0909 0a0a 0a0a 0000 0800 4500 0016 0000
 *        0000 40ff 93e3 0202 0202 e101 0101 0001
 *        0d44 1651 1cdf 4421
 */
  printf("\nSending packet to network_port:%d\n", network_port);
  snprintf(str, 512, "tx 1 pbm=%d data=0x0000000022220000000000028100001608004500002E00000000402F547C090909090A0A0A0A00000800450000160000000040FF93E302020202E101010100010D441651; sleep quiet 1", network_port);
  bshell(unit, str);
}
/*
 * This functions does the following
 * a)test setup
 * b)actual configuration (Done in config_l3_tunnel_mcast_ipgre())
 * c)demonstrates the functionality (Done in verify()).
 */

bcm_error_t execute()
{
  bcm_error_t rv;
  int unit = 0;

  bshell(unit, "config show; a ; version");

  if (BCM_FAILURE((rv = test_setup(unit)))) {
    printf("test_setup() failed.\n");
    return -1;
  }
  printf("Completed test setup successfully.\n");

  if (BCM_FAILURE((rv = config_l3_tunnel_mcast_ipgre(unit)))) {
    printf("config_l3_tunnel_mcast_ipgre() failed.\n");
    return -1;
  }
  printf("Completed configuration(i.e executing config_l3_tunnel_mcast_ipgre()) successfully.\n");

  verify(unit);

  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}
