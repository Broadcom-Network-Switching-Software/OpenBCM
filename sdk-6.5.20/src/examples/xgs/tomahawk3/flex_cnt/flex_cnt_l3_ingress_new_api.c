/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 *  Feature  : Flex counters
 *
 *  Usage    : BCM.0> cint flex_cnt_l3_ingress_new_api.c
 *
 *  config   : flex_cnt_config.bcm
 *
 *  Log file : flex_cnt_l3_ingress_new_api_log.txt
 *  
 *  Test Topology :
 *
 *                   +------------------------------+
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *  ingress_port     |                              |  egress_port
 *  +----------------+          SWITCH              +-----------------+
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   +------------------------------+
 *
 *  Summary:
 *  ========
 *
 *     This cint example configures L3 ingress interface and attaches a flex counter
 *     to the L3 interface. It also demonstrates how to retrieve these counters.
 *
 *  Detailed steps done in the CINT script:
 *  =======================================
 *    1) Step1 - Test Setup (Done in test_setup()):
 *    =============================================
 *      a) Selects one ingress port and one egress port and configure them in Loopback mode.
 *         Install a rule to copy incoming packets to CPU and start packet watcher.
 *
 *    2) Step2 - Configuration (Done in config_flex_cnt_l3_ingress())
 *    ===============================================================
 *      a) Create an ingress_vlan(21) and add ingress_port as it member.
 *
 *      b) Create an egress_vlan(22) and add egress_port as it member.
 *
 *      c) Create two L3 interfaces one for ingress side and the other for egress side.
 *
 *      d) Create two egress objects one for ingress side and the other for egress side.
 *      e) Create a route entry and attach flex counters to ingress L3 interface(L3_IIF).
 *         Three flex counters are attached to the same ingress L3 interface.
 *         Counter at offset 0 is for KNOWN_L3UC_PKT.
 *         Counter at offset 1 is for UNKNOWN_L3UC_PKT
 *         Counter at offset 2 is for L2_BC.
 *
 *    3) Step3 - Verification (Done in verify()):
 *    ===========================================
 *      a) Send 3 KNOWN_L3_UNICAST packets. Below are the packet contents.
 *
 *          Ethernet: dst<00:00:00:00:11:11> src<00:00:00:00:00:01> Tagged Packet ProtID<0x8100> Ctrl<0x0015> Internet Protocol (IP)
 *          IP: V(4) src<2.2.2.2> dst<1.1.1.1> hl<5> service-type<0> tl<46> id<0> frg-off<0> ttl<64> > chk-sum<0x73cc>
 *
 *          Retrieve L3IIF flex counters and print them.
 *
 *      b) Send another 2 KNOWN_L3_UNICAST packets. Packet contents are same as in step 3.a
 *          Retrieve L3IIF flex counters and print them.
 *
 *      c) Send 2 UNKNOWN_L3_UNICAST packets. Below are the packet contents.
 *
 *          Ethernet: dst<00:00:00:00:11:11> src<00:00:00:00:00:01> Tagged Packet ProtID<0x8100> Ctrl<0x0015> Internet Protocol (IP)
 *          IP: V(4) src<2.2.2.2> dst<1.1.1.2> hl<5> service-type<0> tl<46> id<0> frg-off<0> ttl<64> > chk-sum<0x73cb>
 *
 *          Retrieve L3IIF flex counters and print them.
 *
 *      d) Send 2 L2_BC packets. Below are the packet contents.
 *
 *          Ethernet: dst<ff:ff:ff:ff:ff:ff> src<00:00:05:00:01:00> Tagged Packet ProtID<0x8100> Ctrl<0x0015> 802.3 Packet
 *          IP: V(0) src<14.15.16.17> dst<18.19.20.21> hl<2> service-type<3> tl<1029> id<1543> frg-off<2057> ttl<10> > chk-sum<0x0c0d>
 *
 *          Retrieve L3IIF flex counters and print them.
 *      e) Expected Result:
 *      ===================
 *         After step 3.a, verify that counter at offset 0 is 3.
 *         After step 3.b, verify that counter at offset 0 is 5.
 *         After step 3.c, verify that counter at offset 1 is 2.
 *         After step 3.d, verify that counter at offset 2 is 2.
 */

cint_reset();

bcm_port_t ingress_port;
bcm_port_t egress_port;

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
 * on the specified port to CPU and drop the packets. This is
 * to avoid continuous loopback of the packet.
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
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));
  
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

  ingress_port = port_list[0];
  egress_port = port_list[1];

  if (BCM_E_NONE != ingress_port_setup(unit, ingress_port)) {
    printf("ingress_port_setup() failed for port %d\n", ingress_port);
    return -1;
  }

  if (BCM_E_NONE != egress_port_setup(unit, egress_port)) {
      printf("egress_port_setup() failed for port %d\n", egress_port);
      return -1;
  }

  bshell(unit, "pw start report +raw +decode");
  return BCM_E_NONE;
}

bcm_error_t config_flex_cnt_l3_ingress(int unit, bcm_if_t *intf_id)
{
  bcm_error_t rv;
  bcm_if_t ingress_if;
  bcm_l3_ingress_t l3_ingress;
  bcm_l3_intf_t ing_l3_intf;
  bcm_l3_intf_t egr_l3_intf;
  bcm_if_t l3_egr_obj_id_i = 0;
  bcm_l3_egress_t l3_egr_obj_i;
  bcm_if_t l3_egr_obj_id = 0;
  bcm_l3_egress_t l3_egr_obj;
  uint32 l3iif_flex_cnt_id = 0;
  uint32 num_counters;
  uint32 total_counters = 3;
  int num_selectors=3;
  bcm_stat_group_mode_attr_selector_t attr_selectors[3];
  uint32 mode_id = 0;

  bcm_vlan_control_vlan_t vlan_control;
  bcm_gport_t ingress_gport, egress_gport;
  bcm_ip_t l3_routing_dest_ip = 0x01010101; /* 1.1.1.1 */
  bcm_ip_t l3_routing_dest_ip_1 = 0x02020202; /* 2.2.2.2 */
  bcm_mac_t local_mac_1 = {0x00, 0x00, 0x00, 0x00, 0x11, 0x11};   
  bcm_mac_t local_mac_2 = {0x00, 0x00, 0x00, 0x00, 0x22, 0x22};
  bcm_mac_t remote_mac_1 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
  bcm_mac_t remote_mac_2 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
  bcm_vrf_t vrf = 0;
  bcm_vlan_t ingress_vlan = 21;
  bcm_vlan_t egress_vlan = 22;
  int intf_id_1 = 1;
  int intf_id_2 = 2;


  rv = bcm_port_gport_get(unit, ingress_port, &ingress_gport);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_port_gport_get(unit, egress_port, &egress_gport);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create ingress_vlan */
  rv = bcm_vlan_create(unit, ingress_vlan);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_vlan_gport_add(unit, ingress_vlan, ingress_gport, 0);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_gport_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create egress_vlan */
  rv = bcm_vlan_create(unit, egress_vlan);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_vlan_gport_add(unit, egress_vlan, egress_gport, 0);
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

  /* Enable Ingress Interface Mode */
  rv = bcm_switch_control_set(unit, bcmSwitchL3IngressMode, TRUE);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(.., bcmSwitchL3IngressMode, ..): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_switch_control_set(unit, bcmSwitchL3IngressInterfaceMapSet, TRUE);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(.., bcmSwitchL3IngressInterfaceMapSet, ..): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create L3 interface for ingress port */
  bcm_l3_intf_t_init(&ing_l3_intf);
  ing_l3_intf.l3a_flags = BCM_L3_ADD_TO_ARL | BCM_L3_WITH_ID;
  ing_l3_intf.l3a_intf_id = intf_id_1;
  sal_memcpy(ing_l3_intf.l3a_mac_addr, local_mac_1, 6);
  ing_l3_intf.l3a_vid = ingress_vlan;
  ing_l3_intf.l3a_vrf = vrf;
  rv = bcm_l3_intf_create(unit, &ing_l3_intf);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_intf_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  bcm_l3_ingress_t_init(&l3_ingress);
  l3_ingress.vrf = 0;
  rv = bcm_l3_ingress_create(unit, &l3_ingress, &ingress_if);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_ingress_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  bcm_vlan_control_vlan_t_init(&vlan_control);
  rv = bcm_vlan_control_vlan_get(unit, ingress_vlan, &vlan_control);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_control_vlan_get(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  vlan_control.ingress_if = ingress_if;
  rv = bcm_vlan_control_vlan_set(unit, ingress_vlan, vlan_control);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_control_vlan_set(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create L3 interface for egress port */
  bcm_l3_intf_t_init(&egr_l3_intf);
  egr_l3_intf.l3a_flags = BCM_L3_ADD_TO_ARL | BCM_L3_WITH_ID;
  egr_l3_intf.l3a_intf_id = intf_id_2;
  sal_memcpy(egr_l3_intf.l3a_mac_addr, local_mac_2, 6);
  egr_l3_intf.l3a_vid = egress_vlan;
  egr_l3_intf.l3a_vrf = vrf;
  rv =  bcm_l3_intf_create(unit, &egr_l3_intf);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_intf_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

/* Create L3 egress object for egress port */
  bcm_l3_egress_t_init(&l3_egr_obj);
  l3_egr_obj.flags = 0;
  l3_egr_obj.intf = intf_id_2;
  sal_memcpy(l3_egr_obj.mac_addr, remote_mac_2, 6);
  l3_egr_obj.vlan = egress_vlan;
  l3_egr_obj.port = egress_gport;
  rv = bcm_l3_egress_create(unit, 0, &l3_egr_obj, &l3_egr_obj_id);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_egress_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

/* Create L3 egress object for ingress port */
  bcm_l3_egress_t_init(&l3_egr_obj_i);
  l3_egr_obj_i.flags = 0;
  l3_egr_obj_i.intf = intf_id_1;
  sal_memcpy(l3_egr_obj_i.mac_addr, remote_mac_1, 6);
  l3_egr_obj_i.vlan = ingress_vlan;
  l3_egr_obj_i.port = ingress_gport;
  rv = bcm_l3_egress_create(unit, 0, &l3_egr_obj_i, &l3_egr_obj_id_i);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_egress_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

/* Add L3 Route Entry one direction */
  bcm_l3_route_t route_1;
  bcm_l3_route_t_init(&route_1);
  route_1.l3a_subnet = l3_routing_dest_ip;
  route_1.l3a_ip_mask = 0xFFFFFFFF;
  route_1.l3a_intf = l3_egr_obj_id;
  route_1.l3a_vrf = vrf;
  rv = bcm_l3_route_add(unit, &route_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_route_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

/* Add L3 Route Entry another direction */
  bcm_l3_route_t route_2;
  bcm_l3_route_t_init(&route_2);
  route_2.l3a_subnet = l3_routing_dest_ip_1;
  route_2.l3a_ip_mask = 0xFFFFFFFF;
  route_2.l3a_intf = l3_egr_obj_id_i;
  route_2.l3a_vrf = vrf;
  rv = bcm_l3_route_add(unit, &route_2);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_route_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Selector0 for KNOWN_L3UC_PKT. Assigned to 1st counter. */
  bcm_stat_group_mode_attr_selector_t_init(&attr_selectors[0]);
  attr_selectors[0].counter_offset = 0;
  attr_selectors[0].attr = bcmStatGroupModeAttrPktType;
  attr_selectors[0].attr_value = bcmStatGroupModeAttrPktTypeKnownL3UC;

  /* Selector1 for UNKNOWN_L3UC_PKT Assigned to 2nd counter. */
  bcm_stat_group_mode_attr_selector_t_init(&attr_selectors[1]);
  attr_selectors[1].counter_offset = 1;
  attr_selectors[1].attr = bcmStatGroupModeAttrPktType;
  attr_selectors[1].attr_value = bcmStatGroupModeAttrPktTypeUnknownL3UC;

  /* Selector2 for L2_BC. Assigned to 3rd counter. */
  bcm_stat_group_mode_attr_selector_t_init(&attr_selectors[2]);
  attr_selectors[2].counter_offset = 2;
  attr_selectors[2].attr = bcmStatGroupModeAttrPktType;
  attr_selectors[2].attr_value = bcmStatGroupModeAttrPktTypeL2BC;


  /* Create customized stat mode */
  rv = bcm_stat_group_mode_id_create(unit, BCM_STAT_GROUP_MODE_INGRESS, total_counters, num_selectors, attr_selectors, &mode_id);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_stat_group_mode_id_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Attach stat to customized group mode */
  rv = bcm_stat_custom_group_create(unit, mode_id, bcmStatObjectIngL3Intf, &l3iif_flex_cnt_id, &num_counters);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_stat_custom_group_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_l3_ingress_stat_attach(unit, ingress_if, l3iif_flex_cnt_id);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_ingress_stat_attach(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  *intf_id = ingress_if;
  return BCM_E_NONE;
}

/* Get L3IIF flex cnt stats*/
bcm_error_t l3iif_flex_stat_get(int unit, bcm_if_t intf_id)
{
  bcm_error_t rv;
  bcm_stat_value_t l3iif_flex_cnt_val[3];
  uint32 l3iif_flex_cnt_idx[3] = {0, 1, 2};
  uint32 num_counters = 3, i;

  rv = bcm_l3_ingress_stat_counter_sync_get(unit, intf_id, bcmL3StatInPackets, num_counters, l3iif_flex_cnt_idx, l3iif_flex_cnt_val);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_ingress_stat_counter_sync_get(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  for (i = 0; i < num_counters; i++) {
    printf("The packet counter value at offset %d is 0x%x%x \n",
            i,
            COMPILER_64_HI(l3iif_flex_cnt_val[i].packets64),
            COMPILER_64_LO(l3iif_flex_cnt_val[i].packets64));
  }


  rv = bcm_l3_ingress_stat_counter_sync_get(unit, intf_id, bcmL3StatInBytes, num_counters, l3iif_flex_cnt_idx, l3iif_flex_cnt_val);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_ingress_stat_counter_sync_get(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  for (i = 0; i < num_counters; i++) {
    printf("The byte counter value at offset %d is 0x%x%x \n",
            i,
            COMPILER_64_HI(l3iif_flex_cnt_val[i].bytes),
            COMPILER_64_LO(l3iif_flex_cnt_val[i].bytes));
  }

  return BCM_E_NONE;
}

void verify(int unit, bcm_if_t intf_id)
{
  char   str[512];

  bshell(unit, "hm ieee");

 /* Send 3 packets. KNOWN_L3_UNICAST
 *  Ethernet: dst<00:00:00:00:11:11> src<00:00:00:00:00:01> Tagged Packet ProtID<0x8100> Ctrl<0x0015> Internet Protocol (IP)
 *  IP: V(4) src<2.2.2.2> dst<1.1.1.1> hl<5> service-type<0> tl<46> id<0> frg-off<0> ttl<64> > chk-sum<0x73cc>
 */
  printf("Sending 3 KNOWN_L3_UNICAST packets..\n");
  snprintf(str, 512, "tx 3 pbm=%d data=0x0000000011110000000000018100001508004500002E0000000040FF73CC0202020201010101000000000000000000000000000000000000000000000000000058863ECA; sleep 1", ingress_port);
  bshell(unit, str);

  /*Retrieve L3IIF flex counters and print them.*/
  printf("\nDumping L3IIF counters after sending 3 KNOWN_L3_UNICAST packets..\n");
  l3iif_flex_stat_get(unit, intf_id);

  /* Send another 2 packets.. KNOWN_L3_UNICAST
 *  Ethernet: dst<00:00:00:00:11:11> src<00:00:00:00:00:01> Tagged Packet ProtID<0x8100> Ctrl<0x0015> Internet Protocol (IP)
 *  IP: V(4) src<2.2.2.2> dst<1.1.1.1> hl<5> service-type<0> tl<46> id<0> frg-off<0> ttl<64> > chk-sum<0x73cc>
 */
 
  printf("Sending another 2 KNOWN_L3_UNICAST packets..\n");
  snprintf(str, 512, "tx 2 pbm=%d data=0x0000000011110000000000018100001508004500002E0000000040FF73CC0202020201010101000000000000000000000000000000000000000000000000000058863ECA; sleep 1", ingress_port);
  bshell(unit, str);

  /*Retrieve L3IIF flex counters and print them.*/
  printf("\nDumping L3IIF counters after sending another 2 KNOWN_L3_UNICAST packets..\n");
  l3iif_flex_stat_get(unit, intf_id);


  /*Send another 2 packets.. UNKNOWN_L3_UNICAST
   * Ethernet: dst<00:00:00:00:11:11> src<00:00:00:00:00:01> Tagged Packet ProtID<0x8100> Ctrl<0x0015> Internet Protocol (IP)
   * IP: V(4) src<2.2.2.2> dst<1.1.1.2> hl<5> service-type<0> tl<46> id<0> frg-off<0> ttl<64> > chk-sum<0x73cb>  
   */
  printf("Sending 2 UNKNOWN_L3_UNICAST packets..\n");
  snprintf(str, 512, "tx 2 pbm=%d data=0x0000000011110000000000018100001508004500002E0000000040FF73CB0202020201010102000102030405060708090A0B0C0D0E0F10111213141516171819A0F3086A; sleep 1", ingress_port);
  bshell(unit, str); 

  /*Retrieve L3IIF flex counters and print them.*/
  printf("\nDumping L3IIF counters after sending  2 UNKNOWN L3 UNICAST packets..\n");
  l3iif_flex_stat_get(unit, intf_id);

  /*Send 2 L2 Broadcat packets..
   *Ethernet: dst<ff:ff:ff:ff:ff:ff> src<00:00:05:00:01:00> Tagged Packet ProtID<0x8100> Ctrl<0x0015> 802.3 Packet
   * IP: V(0) src<14.15.16.17> dst<18.19.20.21> hl<2> service-type<3> tl<1029> id<1543> frg-off<2057> ttl<10> > chk-sum<0x0c0d>
   */
  printf("Sending 2 L2 Broadcat packets..\n");
  snprintf(str, 512, "tx 2 pbm=%d data=0xFFFFFFFFFFFF00000500010081000015000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2FD75BBFBB; sleep 1", ingress_port);
  bshell(unit, str);

  /*Retrieve L3IIF flex counters and print them.*/
  printf("\nDumping L3IIF counters after sending 2 L2 Broadcast packets..\n");
  l3iif_flex_stat_get(unit, intf_id);

}
/*
 * This functions does the following
 * a)test setup
 * b)actual configuration (Done in config_flex_cnt_l3_ingress())
 * c)demonstrates the functionality (Done in verify()).
 */
bcm_error_t execute()
{
  bcm_error_t rv;
  int unit = 0;
  bcm_if_t intf_id = 0;

  bshell(unit, "config show; a ; version");

  if (BCM_FAILURE((rv = test_setup(unit)))) {
    printf("test_setup() failed.\n");
    return -1;
  }
  printf("Completed test setup successfully.\n");

  if (BCM_FAILURE((rv = config_flex_cnt_l3_ingress(unit, &intf_id)))) {
    printf("config_flex_cnt_l3_ingress() failed.\n");
    return -1;
  }
  printf("Completed configuration(i.e executing config_flex_cnt_l3_ingress()) successfully.\n");

  verify(unit, intf_id);
  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}
