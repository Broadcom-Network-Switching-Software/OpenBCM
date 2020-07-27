/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : L3 RTAG7 ECMP
 *
 *  Usage    : BCM.0> cint l3_rtag7_ecmp.c
 *
 *  config   : BCM56275_A1-PORT.bcm
 *
 *  Log file : l3_rtag7_ecmp_log.txt
 *
 *  Test Topology :
 *
 *              +-------+
 *              |       |
 *              |       |--outPort3--->
 *    -inPort-->|       |
 *              |   L3  |--outPort4--->
 *              |       |
 *              +-------+
 *
 *    Summary:
 *    ========
 *    CINT script to show L3 routing using ECMP nexthop.
 *    there are two ECMP paths, RTAG7 is used as hashing for ECMP.
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1)Step1 - Test Setup (Done in test_setup())
 *    =========================================
 *    a) Selects Three ports and configure them in Loopback mode. Out of these Three ports,
 *       one port is used as Ingress port and the other as Egress port. Install a rule
 *       to copy incoming packets to CPU and start packet watcher.
 *
 *    2)Step2 - Configuration (Done in config_ecmp_rtag7())
 *    ===================================================
 *    a) Create the ECMP path using Two port and Configure RTAG7 setting on the ECMP path. 
 *       This is done in config_ecmp_rtag7().
 *       
 *    3)Step3 - Verification (Done in verify())
 *    =======================================
 *    a) For checking RTAG7 setting on the ECMP Path. send the below packet on ingress port. 
 *
 *       SAMPLE-1
 *       =========
 *       DA 0x00000000AAAA
 *       SA 0x000000002222
 *       VLAN 12
 *       DIP=192.168.10.1 
 *       SIP =10.10.10.1
 *
 *       00 00 00 00 AA AA 00 00 00 00 22 22 81 00 00 0C 
 *       08 00 45 00 00 2E 00 00 00 00 40 FF 9B 1D 0A 0A 
 *       0A 01 C0 A8 0A 01 00 01 02 03 04 05 06 07 08 09 
 *       0A 0B 0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19 
 *       1D BB 9F BE 
 *
 *       SAMPLE-2
 *       =========
 *       DA 0x00000000AAAA
 *       SA 0x000000002222
 *       VLAN 12
 *       DIP=192.168.10.2 
 *       SIP =10.10.10.2
 *
 *       00 00 00 00 AA AA 00 00 00 00 22 22 81 00 00 0C 
 *       08 00 45 00 00 2E 00 00 00 00 40 FF 9B 1B 0A 0A 
 *       0A 02 C0 A8 0A 02 00 01 02 03 04 05 06 07 08 09 
 *       0A 0B 0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19 
 *       DE D5 F4 D2 
 *
 *    b)Expected Result:
 *      ================
 *     NOTE: Below is the snipet of show c output to illustrate packets are loadbalanced
 *           Across all egress ports
 *     For validating the RTAG7 setting on the trunk. Below is the counters output
 *
 *       GRPKT.ge0                   :                     2   
 *       GTPKT.ge0                   :                     2   
 *       GRPKT.ge1                   :                     1   
 *       GTPKT.ge1                   :                     1   
 *       GRPKT.ge2                   :                     1   
 *       GTPKT.ge2                   :                     1   

 */

cint_reset();
bcm_port_t      Ingress_port = 1;
bcm_port_t      Egress_port_1 = 2,Egress_port_2 = 3,Egress_port_3 = 4;
bcm_field_group_t group = -1;

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
  bcm_field_entry_t entry;

  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_NONE));

  if (group == -1)
  {
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
    BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset, 0, &group));
  }

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
  bcm_field_entry_t entry;

  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_ALL));

  if (group == -1)
  {
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
    BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset, 0, &group));
  }
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
  int port_list[3];

  if (BCM_E_NONE != portNumbersGet(unit, port_list, 3)) {
    printf("portNumbersGet() failed\n");
    return -1;
  }

  Ingress_port = port_list[0];
  Egress_port_1 = port_list[1];
  Egress_port_2 = port_list[2];


  if (BCM_E_NONE != ingress_port_setup(unit, Ingress_port)) {
    printf("ingress_port_setup() failed for port %d\n", Ingress_port);
    return -1;
  }


  if (BCM_E_NONE != egress_port_setup(unit, Egress_port_1)) {
    printf("egress_port_setup() failed for port %d\n", Egress_port_1);
    return -1;
  }


  if (BCM_E_NONE != egress_port_setup(unit, Egress_port_2)) {
    printf("egress_port_setup() failed for port %d\n", Egress_port_2);
    return -1;
  }

  bshell(unit, "pw start report +raw +decode");
  return BCM_E_NONE;
}

/* This functions Create vlan and add port to vlan */
bcm_error_t vlan_create_add_port(int unit, int vid, int port)
{
  bcm_pbmp_t pbmp, upbmp;
  bcm_error_t rv;

  BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));
  BCM_PBMP_CLEAR(pbmp);
  BCM_PBMP_CLEAR(upbmp);
  BCM_PBMP_PORT_ADD(pbmp, port);
  rv = bcm_vlan_port_add(unit, vid, pbmp, upbmp);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_vlan_port_add(): %s.\n", bcm_errmsg(rv));
    return rv;
  }
  return rv;
}

/* This functions Create vlan and add array of ports to vlan */
bcm_error_t vlan_create_add_ports(int unit, int vid, int count, bcm_port_t *ports)
{
  bcm_pbmp_t pbmp, upbmp;
  bcm_error_t rv;
  int i;

  BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));
  BCM_PBMP_CLEAR(pbmp);
  BCM_PBMP_CLEAR(upbmp);

  for (i = 0; i < count; i++) {
    BCM_PBMP_PORT_ADD(pbmp, ports[i]);
  }

  rv= bcm_vlan_port_add(unit, vid, pbmp, upbmp);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_vlan_port_add(): %s.\n", bcm_errmsg(rv));
    return rv;
  }
  return rv;
  /*
     bcm_vlan_create(unit, egr_vid);
     print bcm_vlan_gport_add(unit, vid, out_gport_1, 0);
     print bcm_vlan_gport_add(unit, vid, out_gport_2, 0);
     print bcm_vlan_gport_add(unit, vid, out_gport_3, 0);
     */
}

/* This functions Create L3 interface */
bcm_error_t create_l3_interface(int unit, bcm_mac_t local_mac, int vid, bcm_if_t *intf_id)
{
  bcm_l3_intf_t l3_intf;
  bcm_error_t rv = BCM_E_NONE;

  /* L3 interface and egress object for access */
  bcm_l3_intf_t_init(&l3_intf);
  l3_intf.l3a_flags |= BCM_L3_ADD_TO_ARL;
  sal_memcpy(l3_intf.l3a_mac_addr, local_mac, sizeof(local_mac));
  l3_intf.l3a_vid = vid;
  rv = bcm_l3_intf_create(unit, &l3_intf);
  *intf_id = l3_intf.l3a_intf_id;

  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_l3_intf_create(): %s.\n", bcm_errmsg(rv));
    return rv;
  }
  return rv;
}

/* This functions Create L3 egress object */
bcm_error_t create_egr_obj(int unit, int l3_if, bcm_mac_t nh_mac, bcm_gport_t gport,
    bcm_if_t *egr_obj_id)
{
  bcm_l3_egress_t l3_egress;
  bcm_error_t rv = BCM_E_NONE;

  bcm_l3_egress_t_init(&l3_egress);
  sal_memcpy(l3_egress.mac_addr,  nh_mac, sizeof(nh_mac));
  /* l3_egress.vlan = l3_if; vid; */
  l3_egress.intf  = l3_if;
  l3_egress.port = gport;
  rv= bcm_l3_egress_create(unit, 0, &l3_egress, egr_obj_id);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_l3_egress_create(): %s.\n", bcm_errmsg(rv));
    return rv;
  }
  return rv;
}

/* This functions Adding router mac */
bcm_error_t config_router_mac(int unit, bcm_mac_t router_mac, bcm_vlan_t ing_vlan)
{
  bcm_l2_addr_t l2_addr;
  bcm_error_t rv = BCM_E_NONE;

  bcm_l2_addr_t_init(&l2_addr, router_mac, ing_vlan);
  l2_addr.flags |= (BCM_L2_L3LOOKUP | BCM_L2_STATIC);
  rv= bcm_l2_addr_add(unit, &l2_addr);

  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_l2_addr_add(): %s.\n", bcm_errmsg(rv));
    return rv;
  }
  return rv;
}

/* This functions Enable RTAG7 on port */
bcm_error_t ecmp_rtag7_enable(int unit, int port)
{
  int flags;
  bcm_error_t rv = BCM_E_NONE;

  rv= bcm_switch_control_port_get(unit, port, bcmSwitchHashControl, &flags);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_port_get(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  flags |= BCM_HASH_CONTROL_ECMP_ENHANCE;

  rv= bcm_switch_control_port_set(unit, port, bcmSwitchHashControl, flags);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_port_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }
  return rv;
}

bcm_error_t config_ecmp_rtag7(int unit)
{
  int flags;
  bcm_error_t rv = BCM_E_NONE;
  bcm_vlan_t vid_in = 12;
  bcm_gport_t gport_in;

  bcm_vlan_t egr_vid_1 = 13;
  bcm_gport_t egr_gport_1;
  bcm_if_t egr_l3_if_1;
  bcm_if_t egr_obj_1;

  bcm_vlan_t egr_vid_2 = 14;
  bcm_gport_t egr_gport_2;
  bcm_if_t egr_l3_if_2;
  bcm_if_t egr_obj_2;

  bcm_l3_route_t route_info;

  bcm_mac_t egr_nxt_hop_1  = {0x00,0x00,0x00,0x00,0x11,0x11};
  bcm_mac_t egr_router_mac_1  = {0x00,0x00,0x00,0x00,0xBB,0x1B};

  bcm_mac_t egr_nxt_hop_2  = {0x00,0x00,0x00,0x00,0x11,0x21};
  bcm_mac_t egr_router_mac_2  = {0x00,0x00,0x00,0x00,0xBB,0x2B};

  bcm_mac_t nxt_hop_in  = {0x00,0x00,0x00,0x00,0x22,0x22};
  bcm_mac_t router_mac_in  = {0x00,0x00,0x00,0x00,0xAA,0xAA};

  bcm_ip_t dip = 0xC0A80A01;  /* 192.168.10.1 */
  bcm_ip_t mask = 0xffffff00;  /* 255.255.255.0 */

  rv= bcm_switch_control_set(unit, bcmSwitchL3EgressMode, TRUE);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv=ecmp_rtag7_enable(unit, Ingress_port);
  if (BCM_FAILURE(rv)) {
    printf("Error executing ecmp_rtag7_enable(): %s.\n", bcm_errmsg(rv));
    return rv;
  }


  /* Use port based hash selection (RTAG7_HASH_SEL->USE_FLOW_SEL_ECMP) */
  rv= bcm_switch_control_set(unit, bcmSwitchHashUseFlowSelEcmp, 0);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  flags = BCM_HASH_FIELD_SRCMOD |
    BCM_HASH_FIELD_SRCPORT |
    BCM_HASH_FIELD_PROTOCOL |
    BCM_HASH_FIELD_DSTL4 |
    BCM_HASH_FIELD_SRCL4 |
    BCM_HASH_FIELD_IP4DST_LO |
    BCM_HASH_FIELD_IP4DST_HI |
    BCM_HASH_FIELD_IP4SRC_LO |
    BCM_HASH_FIELD_IP4SRC_HI;

  /* Block A - L3 packet field selection (RTAG7_HASH_FIELD_BMAP_1) */
  rv= bcm_switch_control_set(unit, bcmSwitchHashIP4Field0, flags);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv= bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpField0, flags);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* Block B - L3 packet field selection (RTAG7_HASH_FIELD_BMAP_1) */
  rv= bcm_switch_control_set(unit, bcmSwitchHashIP4Field1, flags);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv= bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpField1, flags);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* Configure Seed */
  rv= bcm_switch_control_set(unit, bcmSwitchHashSeed0, 0x33333333);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv= bcm_switch_control_set(unit, bcmSwitchHashSeed1, 0x55555555);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* Enable preprocess */
  rv= bcm_switch_control_set(unit, bcmSwitchHashField0PreProcessEnable, TRUE);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv= bcm_switch_control_set(unit, bcmSwitchHashField1PreProcessEnable, TRUE);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* Configure HASH A and HASH B functions */
  rv=bcm_switch_control_set(unit, bcmSwitchHashField0Config, BCM_HASH_FIELD_CONFIG_CRC32LO);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv= bcm_switch_control_set(unit, bcmSwitchHashField0Config1, BCM_HASH_FIELD_CONFIG_CRC32HI);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv= bcm_switch_control_set(unit, bcmSwitchHashField1Config, BCM_HASH_FIELD_CONFIG_CRC16CCITT);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv= bcm_switch_control_set(unit, bcmSwitchHashField1Config1, BCM_HASH_FIELD_CONFIG_CRC16CCITT);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv=bcm_port_gport_get(unit, Ingress_port, &gport_in);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv=bcm_port_gport_get(unit, Egress_port_1, &egr_gport_1);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv=bcm_port_gport_get(unit, Egress_port_2, &egr_gport_2);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* 
   * Ingress side of the VLAN
   */ 
  rv=vlan_create_add_port(unit, vid_in, Ingress_port);
  if (BCM_FAILURE(rv)) {
    printf("Error executing vlan_create_add_port(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* Enable ingress L3 lookup */
  rv= config_router_mac(unit, router_mac_in, vid_in);
  if (BCM_FAILURE(rv)) {
    printf("Error executing config_router_mac(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /*
   * Egress side of the VLAN
   */
  rv=vlan_create_add_port(unit, egr_vid_1, Egress_port_1);
  if (BCM_FAILURE(rv)) {
    printf("Error executing vlan_create_add_port(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv=vlan_create_add_port(unit, egr_vid_2, Egress_port_2);
  if (BCM_FAILURE(rv)) {
    printf("Error executing vlan_create_add_port(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* 
   * Create egress L3 interface
   */
  rv=create_l3_interface(unit, egr_router_mac_1, egr_vid_1, &egr_l3_if_1);
  if (BCM_FAILURE(rv)) {
    printf("Error executing create_l3_interface(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv=create_l3_interface(unit, egr_router_mac_2, egr_vid_2, &egr_l3_if_2);
  if (BCM_FAILURE(rv)) {
    printf("Error executing create_l3_interface(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* 
   * Egress side of the next hop object
   */
  rv= create_egr_obj(unit, egr_l3_if_1, egr_nxt_hop_1, egr_gport_1, &egr_obj_1);
  if (BCM_FAILURE(rv)) {
    printf("Error executing create_egr_obj(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv= create_egr_obj(unit, egr_l3_if_2, egr_nxt_hop_2, egr_gport_2, &egr_obj_2);
  if (BCM_FAILURE(rv)) {
    printf("Error executing create_egr_obj(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  print egr_obj_1;
  print egr_obj_2;

  /* ECMP group */
  bcm_if_t ecmp_group[2];
  bcm_if_t ecmp_group_id;

  /*
     bcm_l3_egress_ecmp_t ecmp_info;
     bcm_l3_egress_ecmp_t_init(&ecmp_info);
     ecmp_info.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RANDOM;
     ecmp_info.dynamic_size = 64;
     ecmp_group[0] = egr_obj_1;
     ecmp_group[1] = egr_obj_2;
     print bcm_l3_egress_ecmp_create(unit, &ecmp_info, 2, ecmp_group);
     ecmp_group_id = ecmp_info.ecmp_intf;
     */
  ecmp_group[0] = egr_obj_1;
  ecmp_group[1] = egr_obj_2;
  rv= bcm_l3_egress_multipath_create(unit, 0, 2, ecmp_group, &ecmp_group_id);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_l3_egress_multipath_create(): %s.\n", bcm_errmsg(rv));
    return rv;
  }


  /* 
   * Install the route for DIP, note this is routed out on the port_out
   */
  bcm_l3_route_t_init(&route_info);
  route_info.l3a_flags = BCM_L3_MULTIPATH;
  route_info.l3a_intf = ecmp_group_id;
  route_info.l3a_subnet = dip;
  route_info.l3a_ip_mask = mask;
  rv= bcm_l3_route_add(unit, &route_info);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_l3_route_add(): %s.\n", bcm_errmsg(rv));
    return rv;
  }


  return BCM_E_NONE;
}


void verify(int unit)
{
  char   str[512];

  bshell(unit, "hm ieee");

  /*
   * For checking RTAG7 setting on the ECMP Path. send the below packet on ingress port. 
   *
   *       SAMPLE-1
   *       =========
   *       DA 0x00000000AAAA
   *       SA 0x000000002222
   *       VLAN 12
   *       DIP=192.168.10.1 
   *       SIP =10.10.10.1
   *
   *       00 00 00 00 AA AA 00 00 00 00 22 22 81 00 00 0C 
   *       08 00 45 00 00 2E 00 00 00 00 40 FF 9B 1D 0A 0A 
   *       0A 01 C0 A8 0A 01 00 01 02 03 04 05 06 07 08 09 
   *       0A 0B 0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19 
   *       1D BB 9F BE 
   *
   *
   *       SAMPLE-2
   *       =========
   *       DA 0x00000000AAAA
   *       SA 0x000000002222
   *       VLAN 12
   *       DIP=192.168.10.2 
   *       SIP =10.10.10.2
   *
   *       00 00 00 00 AA AA 00 00 00 00 22 22 81 00 00 0C 
   *       08 00 45 00 00 2E 00 00 00 00 40 FF 9B 1B 0A 0A 
   *       0A 02 C0 A8 0A 02 00 01 02 03 04 05 06 07 08 09 
   *       0A 0B 0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19 
   *       DE D5 F4 D2 
   *
   * NOTE: Use above traffic to get loadbalance on all egress port and 
   *       show c out after each packet shows every egress port getting 
   *       one packet each
   */


  bshell(unit, "clear c"); 

  printf("\nSending packet to Ingress_port:%d\n", Ingress_port);
  snprintf(str, 512, "tx 1 pbm=%d data=0x00000000AAAA0000000022228100000C08004500002E0000000040FF9B1D0A0A0A01C0A80A01000102030405060708090A0B0C0D0E0F101112131415161718191DBB9FBE; sleep quiet 1", Ingress_port);
  bshell(unit, str);
  bshell(unit, "show c");
  snprintf(str, 512, "tx 1 pbm=%d data=0x00000000AAAA0000000022228100000C08004500002E0000000040FF9B1B0A0A0A02C0A80A02000102030405060708090A0B0C0D0E0F10111213141516171819DED5F4D2; sleep quiet 1", Ingress_port);
  bshell(unit, str);
  bshell(unit, "show c");

  return BCM_E_NONE;

}

bcm_error_t execute()
{
  bcm_error_t rv;
  int unit = 0;
  bshell(unit, "config show; a ; cancun stat; version");
  if (BCM_FAILURE((rv = test_setup(unit)))) {
    printf("test_setup() failed.\n");
    return -1;
  }

  if (BCM_FAILURE((rv = config_ecmp_rtag7(unit)))) {
    printf("config_ecmp_rtag7() failed.\n");
    return -1;
  }

  verify(unit);
  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}


