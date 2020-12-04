/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : L2 MCAST PFM mode functionality
 *
 * Usage    : BCM.0> cint td4_hsdk_l2_mcast_route_pfm.c 
 *
 * config   : bcm56880_a0-generic-l3.config.yml 
 *
 * Log file : td4_hsdk_l2_mcast_route_pfm_log.txt
 *
 * Test Topology :
 *
 *                  +------------------------------+
 *                  |                              +------------------+  
 *                  |                              | egress_port_1 cd1
 * ingress_vlan 20  |                              +------------------+   
 * ingress_port cd0 |                              | egress_port_2 cd2  
 * +----------------+          SWITCH-TD4          |   
 *                  |                              |  
 *                  |                              | egress_port_3 cd3 
 *                  |                              +------------------+ 
 *                  |                              | egress_port_4 cd4 
 *                  |                              +------------------+ 
 *                  +------------------------------+
 *                                                                        
 * Summary:
 * ========
 *   This Cint example to show configuration of the L2 MCAST PFM mode functionality
 *   using BCM APIs.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects Five ports and configure them in Loopback mode. Out of these
 *        Five ports, one port is used as ingress_port and the other Four as
 *        egress_ports.
 *
 *     b) Install an IFP rule to copy incoming and outgoing packets to CPU and
 *        start packet watcher to display packet contents.
 *
 *     Note: IFP rule is meant for a testing purpose only (Internal) and it is
 *           nothing to do with an actual functional test.
 *
 *   2) Step2 - Configuration (Done in setup_l2_mcast_forwarding()):
 *   =========================================================
 *      a) Create forwarding vlan_id.
 *
 *      b) Add ingress_port member of vlan_id.
 *
 *      c) Add egress_port_1 member of vlan_id.
 *
 *      d) Add egress_port_2 member of vlan_id.
 *
 *      e) Add egress_port_3 member of vlan_id.
 *
 *      f) Add egress_port_4 member of vlan_id.
 *
 *      g) Create known L2_MCAST group.
 *
 *      h) Add ingress_port, egress_port_1 and egress_port_2 for known L2_MCAST group.
 *
 *      i) Create unknown L2_MCAST group.
 *
 *      j) Add ingress_port, egress_port_1, egress_port_2 and egress_port_3 for unknown L2_MCAST group.
 *
 *   3) Step3 - MCAST PFM mode concept:
 *   =========================================================
 *   Port Filtering Mode {PFM} supports following threee modes 
 *
 *   PFM = 0 {BCM_VLAN_MCAST_FLOOD_ALL}
 *       All multicast packets are flooded to the unknwon MCAST flood group.
 *  
 *   PFM = 1 {BCM_VLAN_MCAST_FLOOD_UNKNOWN}
 *       Unknown L2_MCAST traffic is flooded to the unknwon MCAST flood group.
 *       Known L2_MCAST traffic forwarded to the known MCAST group derived from L2 table. 
 *
 *   PFM = 2 {BCM_VLAN_MCAST_FLOOD_NONE}
 *       Unknown L2_MCAST traffic is dropped.
 *       Known L2_MCAST traffic forwarded to the known MCAST group derived from L2 table. 
 *
 *   LEGACY device:
 *
 *   Following PFM controls are available on Legacy devices.
 *       VLAN.VLAN_PROFILE.L2_PFM=0/1/2
 *       VLAN.VLAN_PROFILE.L3_IPV4_PFM=0/1/2
 *       VLAN.VLAN_PROFILE.L3_IPV6_PFM=0/1/2
 *
 *   TD4 doesn't support PFM controls but we can do following configuration.
 *
 *   PFM = 0 {BCM_VLAN_MCAST_FLOOD_ALL}
 *       Configure unknown MCAST group and add vlan member ports for unknown MCAST group.
 *       Associate unknown MCAST group with VLAN for known and unknwon L2 MCAST traffic flooding.    
 *   PFM = 1 {BCM_VLAN_MCAST_FLOOD_UNKNOWN}
 *       Configure known MCAST group and add egress ports as member for known MCAST group.
 *       Add L2_HOST entry for incoming known DMAC to derive known MCAST group for replication. 
 *   PFM = 2 {BCM_VLAN_MCAST_FLOOD_NONE}
 *       Set BCM_VLAN_NON_UCAST_DROP for VLAN to drop unknown MCAST traffic. 
 *
 *   4) Step4 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show', 'l3 ing_intf show', 'l3 intf show',
 *        'l3 egress show', 'multicast show' and 'l3 route show'
 *
 *     b) PFM_0 mode configured  
 *           Send known L2_MCAST and Unkown L2_MCAST packets on ingress port  
 *
 *     c) PFM_1 mode configured 
 *           Send known L2_MCAST and Unkown L2_MCAST packets on ingress port  
 *
 *     d) PFM_2 mode configured
 *           Send known L2_MCAST and Unkown L2_MCAST packets on ingress port  
 *
 *     Known L2_MCAST packet
 *
 *       0100 0000 00aa 0000 0000 0011 8100 0014
 *       0800 4500 002e 0000 0000 40ff 3d96 1414
 *       1414 0a0a 0a0a 0000 0000 0000 0000 0000
 *       0000 0000 0000 0000 0000 0000 0000 0000
 *       5daf 10d9
 *
 *     Unknown L2_MCAST packet
 *
 *       0100 0000 00bb 0000 0000 0011 8100 0014
 *       0800 4500 002e 0000 0000 40ff 3d96 1414
 *       1414 0a0a 0a0a 0000 0000 0000 0000 0000
 *       0000 0000 0000 0000 0000 0000 0000 0000
 *       f9ea 2992
 * 
 *     c) Expected Result:
 *     ===================
 *     After step 4.b, Results
 *        Known L2_MCAST traffic hits VLAN.UNKNOWN_MCAST group
 *          Traffic egress out of egress_port_1, egress_port_2 and egress_port_3 
 *        Unknown L2_MCAST traffic hits VLAN.UNKNOWN_MCAST group 
 *          Traffic egress out of egress_port_1, egress_port_2 and egress_port_3
 *
 *     After step 4.c, Results
 *        Known L2_MCAST traffic hits l2_HOST.KNOWN_MCAST group
 *          Traffic egress out of egress_port_1 and egress_port_2 
 *        Unknown L2_MCAST traffic hits VLAN.UNKNOWN_MCAST group 
 *          Traffic egress out of egress_port_1, egress_port_2 and egress_port_3
 *
 *     After step 4.d, Results
 *        Known L2_MCAST traffic hits l2_HOST.KNOWN_MCAST group
 *          Traffic egress out of egress_port_1 and egress_port_2 
 *        Unknown L2_MCAST traffic hits VLAN.L2MC_MISS_ACTION=DROP 
 *          Traffic drop at ingress_port 
 */

/* Reset C interpreter*/
cint_reset();

bcm_port_t    ingress_port;
bcm_port_t    egress_port_1;
bcm_port_t    egress_port_2;
bcm_port_t    egress_port_3;
bcm_port_t    egress_port_4;
bcm_gport_t   ingress_gport;
bcm_gport_t   egress_gport_1;
bcm_gport_t   egress_gport_2;
bcm_gport_t   egress_gport_3;
bcm_gport_t   egress_gport_4;

bcm_vlan_t        vlan_id = 20;
bcm_multicast_t   l2mc_known_group = 100;
bcm_multicast_t   l2mc_unknown_group = 200;

bcm_mac_t         known_dst_mac = "01:00:00:00:00:AA";
bcm_mac_t         unknown_dst_mac = "01:00:00:00:00:BB";

/*
 * This function is written so that hardcoding of port
 * numbers in Cint scripts is removed. This function gives
 * required number of ports
 */
bcm_error_t
portNumbersGet(int unit, int *port_list, int num_ports)
{
    int i = 0, port = 0, rv = 0;
    bcm_port_config_t configP;
    bcm_pbmp_t        ports_pbmp;

    rv = bcm_port_config_get(unit, &configP);
    if (BCM_FAILURE(rv)) {
        printf("\nError in retrieving port configuration: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    ports_pbmp = configP.e;
    for (i= 1; i < BCM_PBMP_PORT_MAX; i++) {
        if (BCM_PBMP_MEMBER(&ports_pbmp, i) && (port < num_ports)) {
            port_list[port] = i;
            port++;
        }
    }

    if ((0 == port) || (port != num_ports)) {
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
bcm_error_t
ingress_port_setup(int unit, bcm_port_t port)
{
    bcm_field_entry_t        entry;
    bcm_field_group_config_t group_config; 

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    group_config.priority = port;
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

    printf("ingress_port_setup configured for ingress_port : %d\n", port);

    return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode and installs
 * an IFP rule. This IFP rule copies the packets ingressing
 * on the specified port to CPU. Port is also configured
 * to discard all packets. This is to avoid continuous
 * loopback of the packet.
 */
bcm_error_t
egress_port_setup(int unit, bcm_port_t port)
{
    bcm_field_entry_t        entry;
    bcm_field_group_config_t group_config; 

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDrop);
    group_config.priority = port;
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));

    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

    printf("egress_port_setup configured for egress_port : %d\n", port);

    return BCM_E_NONE;
}

/*
 * Test Setup:
 * This functions gets the port numbers and sets up ingress and
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
bcm_error_t
test_setup(int unit)
{
    int port_list[5];

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 5)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    ingress_port = port_list[0];
    egress_port_1 = port_list[1];
    egress_port_2 = port_list[2];
    egress_port_3 = port_list[3];
    egress_port_4 = port_list[4];

    if (BCM_E_NONE != ingress_port_setup(unit, ingress_port)) {
        printf("ingress_port_setup() failed for port %d\n", ingress_port);
        return -1;
    }

    if (BCM_E_NONE != egress_port_setup(unit, egress_port_1)) {
        printf("egress_port_setup() failed for port %d\n", egress_port_1);
        return -1;
    }

    if (BCM_E_NONE != egress_port_setup(unit, egress_port_2)) {
        printf("egress_port_setup() failed for port %d\n", egress_port_2);
        return -1;
    }

    if (BCM_E_NONE != egress_port_setup(unit, egress_port_3)) {
        printf("egress_port_setup() failed for port %d\n", egress_port_3);
        return -1;
    }

    if (BCM_E_NONE != egress_port_setup(unit, egress_port_4)) {
        printf("egress_port_setup() failed for port %d\n", egress_port_4);
        return -1;
    }

    bshell(unit, "pw start report +raw +decode +pmd");

    return BCM_E_NONE;
}

void
verify(int unit)
{
  int                      rv = BCM_E_NONE;
  char                     str[512];
  int                      flags;
  bcm_l2_addr_t            l2_host_entry;
  bcm_vlan_control_vlan_t  vlan_control;
  bcm_vlan_control_vlan_t  vlan_ctrl;

  bshell(unit, "vlan show");
  bshell(unit, "l3 ing_intf show");
  bshell(unit, "l3 intf show");
  bshell(unit, "l3 egress show");
  bshell(unit, "multicast show");
  bshell(unit, "l3 route show");
  bshell(unit, "l2 show");
  bshell(unit, "clear c");

  printf("\n\nL2_MCAST PFM=0 mode checking>>\n\n");

  /* Associate unknown L2_MCAST group with forwarding VLAN_ID */
  bcm_vlan_control_vlan_t_init(&vlan_control);
  vlan_control.broadcast_group         = l2mc_unknown_group;
  vlan_control.unknown_multicast_group = l2mc_unknown_group;
  vlan_control.unknown_unicast_group   = l2mc_unknown_group;
  flags = BCM_VLAN_CONTROL_VLAN_BROADCAST_GROUP_MASK | BCM_VLAN_CONTROL_VLAN_UNKNOWN_MULTICAST_GROUP_MASK | BCM_VLAN_CONTROL_VLAN_UNKNOWN_UNICAST_GROUP_MASK;
  rv = bcm_vlan_control_vlan_selective_set(unit, vlan_id, flags, &vlan_control);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_control_vlan_selective_set(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  printf("\n\nSending known L2 MCAST packet on ingress_port : %d\n", ingress_port);
  snprintf(str, 512, "tx 1 pbm=%d data=0x0100000000AA0000000000118100001408004500002e0000000040ff3d96141414140a0a0a0a00000000000000000000000000000000000000000000000000005daf10d9; sleep quiet 1", ingress_port);
  bshell(unit, str);
  bshell(unit, "sleep 1");
  bshell(unit, "l2 show");
  bshell(unit, "show c");

  printf("\n\nSending unknown L2 MCAST packet on ingress_port : %d\n", ingress_port);
  snprintf(str, 512, "tx 1 pbm=%d data=0x0100000000BB0000000000118100001408004500002e0000000040ff3d96141414140a0a0a0a0000000000000000000000000000000000000000000000000000f9ea2992; sleep quiet 1", ingress_port);
  bshell(unit, str);
  bshell(unit, "sleep 1");
  bshell(unit, "l2 show");
  bshell(unit, "show c");

  printf("\n\nL2_MCAST PFM=1 mode checking>>\n\n");

  /* Add known L2_MCAST destination MAC in the L2 HOST table */
  bcm_l2_addr_t_init(&l2_host_entry, known_dst_mac, vlan_id);
  l2_host_entry.l2mc_group = l2mc_known_group;
  l2_host_entry.flags      = BCM_L2_MCAST | BCM_L2_STATIC;
  rv = bcm_l2_addr_add(unit, &l2_host_entry);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l2_addr_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  printf("\n\nSending known L2 MCAST packet on ingress_port : %d\n", ingress_port);
  snprintf(str, 512, "tx 1 pbm=%d data=0x0100000000AA0000000000118100001408004500002e0000000040ff3d96141414140a0a0a0a00000000000000000000000000000000000000000000000000005daf10d9; sleep quiet 1", ingress_port);
  bshell(unit, str);
  bshell(unit, "sleep 1");
  bshell(unit, "l2 show");
  bshell(unit, "show c");

  printf("\n\nSending unknown L2 MCAST packet on ingress_port : %d\n", ingress_port);
  snprintf(str, 512, "tx 1 pbm=%d data=0x0100000000BB0000000000118100001408004500002e0000000040ff3d96141414140a0a0a0a0000000000000000000000000000000000000000000000000000f9ea2992; sleep quiet 1", ingress_port);
  bshell(unit, str);
  bshell(unit, "sleep 1");
  bshell(unit, "l2 show");
  bshell(unit, "show c");

  printf("\n\nL2_MCAST PFM=2 mode checking>>\n\n");

  /* Set DROP flag for forwarding VLAN to drop incoming unknown L2_MCAST traffic */
  bcm_vlan_control_vlan_t_init(&vlan_ctrl);
  rv = bcm_vlan_control_vlan_get(unit, vlan_id, &vlan_ctrl);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_control_vlan_get(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  print vlan_ctrl.flags; 
  vlan_ctrl.flags = vlan_ctrl.flags | BCM_VLAN_NON_UCAST_DROP;
  print vlan_ctrl.flags; 
  rv = bcm_vlan_control_vlan_set(unit, vlan_id, vlan_ctrl);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_control_vlan_set(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  printf("\n\nSending known L2 MCAST packet on ingress_port : %d\n", ingress_port);
  snprintf(str, 512, "tx 1 pbm=%d data=0x0100000000AA0000000000118100001408004500002e0000000040ff3d96141414140a0a0a0a00000000000000000000000000000000000000000000000000005daf10d9; sleep quiet 1", ingress_port);
  bshell(unit, str);
  bshell(unit, "sleep 1");
  bshell(unit, "l2 show");
  bshell(unit, "show c");

  printf("\n\nSending unknown L2 MCAST packet on ingress_port : %d\n", ingress_port);
  snprintf(str, 512, "tx 1 pbm=%d data=0x0100000000BB0000000000118100001408004500002e0000000040ff3d96141414140a0a0a0a0000000000000000000000000000000000000000000000000000f9ea2992; sleep quiet 1", ingress_port);
  bshell(unit, str);
  bshell(unit, "sleep 1");
  bshell(unit, "l2 show");
  bshell(unit, "show c");

  return;
}

/* 
 * L2 MCAST functionality configuration. 
 */
bcm_error_t
setup_l2_mcast_forwarding(int unit)
{
  bcm_error_t       rv;

  rv = bcm_port_gport_get(unit, ingress_port, &ingress_gport);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_port_gport_get(unit, egress_port_1, &egress_gport_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_port_gport_get(unit, egress_port_2, &egress_gport_2);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_port_gport_get(unit, egress_port_3, &egress_gport_3);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_port_gport_get(unit, egress_port_4, &egress_gport_4);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create vlan */
  rv = bcm_vlan_create(unit, vlan_id);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Add ingress_port member of vlan */
  rv = bcm_vlan_gport_add(unit, vlan_id, ingress_gport, 0);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_gport_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Add egress_port_1 member of vlan */
  rv = bcm_vlan_gport_add(unit, vlan_id, egress_gport_1, 0);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_gport_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Add egress_port_2 member of vlan */
  rv = bcm_vlan_gport_add(unit, vlan_id, egress_gport_2, 0);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_gport_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Add egress_port_3 member of vlan */
  rv = bcm_vlan_gport_add(unit, vlan_id, egress_gport_3, 0);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_gport_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Add egress_port_4 member of vlan */
  rv = bcm_vlan_gport_add(unit, vlan_id, egress_gport_4, 0);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_gport_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create known L2_MCAST group */
  rv = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_WITH_ID, &l2mc_known_group);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_multicast_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Add ingress_port for known L2_MCAST group */
  rv = bcm_multicast_egress_add(unit, l2mc_known_group, ingress_gport, -1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_multicast_egress_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Add egress_port_1 for known L2_MCAST group */
  rv = bcm_multicast_egress_add(unit, l2mc_known_group, egress_gport_1, -1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_multicast_egress_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Add egress_port_2 for known L2_MCAST group */
  rv = bcm_multicast_egress_add(unit, l2mc_known_group, egress_gport_2, -1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_multicast_egress_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create known L2_MCAST group */
  rv = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_WITH_ID, &l2mc_unknown_group);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_multicast_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Add ingress_port for unknown L2_MCAST group */
  rv = bcm_multicast_egress_add(unit, l2mc_unknown_group, ingress_gport, -1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_multicast_egress_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Add egress_port_1 for unknown L2_MCAST group */
  rv = bcm_multicast_egress_add(unit, l2mc_unknown_group, egress_gport_1, -1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_multicast_egress_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Add egress_port_2 for unknown L2_MCAST group */
  rv = bcm_multicast_egress_add(unit, l2mc_unknown_group, egress_gport_2, -1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_multicast_egress_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Add egress_port_3 for unknown L2_MCAST group */
  rv = bcm_multicast_egress_add(unit, l2mc_unknown_group, egress_gport_3, -1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_multicast_egress_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  return rv;
}

/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in setup_l2_mcastforwarding())
 *  c) demonstrates the functionality(done in verify()).
 */
bcm_error_t
execute(void)
{
    bcm_error_t rv;
    int unit = 0;
    print "config show; attach; version";
    bshell(unit, "config show; attach ; version");

    print "~~~ #1) test_setup(): ** start **";
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return -1;
    }
    print "~~~ #1) test_setup(): ** end **";

    print "~~~ #2) setup_l2_mcast_forwarding(): ** start **";
    if (BCM_FAILURE((rv = setup_l2_mcast_forwarding(unit)))) {
        printf("setup_l2_mcast_forwarding() failed.\n");
        return -1;
    }
    print "~~~ #2) setup_l2_mcast_forwarding(): ** end **";

    print "~~~ #3) verify(): ** start **";
    verify(unit);
    print "~~~ #3) verify(): ** end **";

    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
    print "execute(): Start";
    print execute();
    print "execute(): End";
}

