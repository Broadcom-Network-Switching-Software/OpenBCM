/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : L3 Multicast (IPMC) functionality
 *
 * Usage    : BCM.0> cint td4_hsdk_l3_ipmc_route.c 
 *
 * config   : bcm56880_a0-generic-32x400.config.yml 
 *
 * Log file : td4_hsdk_l3_ipmc_route_log.txt
 *
 * Test Topology :
 *
 *                  +------------------------------+  egress_port1
 *                  |                              +-----------------+
 *                  |                              |
 *                  |                              |
 *  ingress_port    |                              |  egress_port2
 * +----------------+         SWITCH-TD4           +-----------------+
 *                  |                              |
 *                  |                              |
 *                  |                              |  egress_port3
 *                  |                              +-----------------+
 *                  |                              |
 *                  +------------------------------+
 *
 * Summary:
 * ========
 *   This Cint example to show configuration of the IPv4 multicast scenario
 *   using BCM APIs.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Select one ingress and three egress ports and configure them in
 *        Loopback mode.
 *
 *     b) Install an IFP rule to copy incoming packets to CPU and start
 *        packet watcher.
 *
 *     Note: IFP rule is meant for a testing purpose only (Internal) and it is
 *           nothing to do with an actual functional test.
 *
 *   2) Step2 - Configuration (Done in config_l3_ipmc()):
 *   ====================================================
 *     a) Create an ingress_vlan and add ingress_port as member. 
 *
 *     b) Create a egress_vlan1 and add egress_port1 as member.
 *
 *     c) Create a egress_vlan2 and add egress_port2 as member.
 *
 *     d) Create a egress_vlan3 and add egress_port3 as member.
 *
 *     e) Create a IPMC multicast group.
 *
 *     f) Create L3 ingress interface for ingress side and bind with ingress vlan. 
 *
 *     g) Create a L3 egress interface for egress side interface_1.
 *
 *     h) Create a L3 egress interface for egress side interface_2.
 *
 *     i) Create a L3 egress interface for egress side interface_3.
 *
 *     j) Create L3 IPMC encapsulation for above all three egress interfaces
 *        and add these IPMC encapsulation ids to the multicast group. 
 *
 *     k) Configure IPv4 host entry with group and source address to point mcast group. 
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show', 'l3 ing_intf show',
 *        'l3 intf show', 'l3 egress show', 'multicast show', 'l2 show'
 *        and 'ipmc table show'
 *
 *     b) Transmit the known IPv4 multicast packet with below contents.
 *
 *        DMAC 0x01005e010101
 *        SMAC 0x6000300
 *        VLAN 5
 *        DIP  228.1.1.1
 *        SIP  2.1.1.1
 *
 *        0100 5e01 0101 0000 0600 0300 8100 0005
 *        0800 4500 002e 0000 0000 40ff 91cd 0201
 *        0101 e401 0101 0001 0203 0405 0607 0809
 *        0a0b 0c0d 0e0f 1011 1213 1415 1617 1819
 *        bf2e 44c3
 *
 *     c) Expected Result:
 *     ===================
 *        We can see that smac and vlan are all changed as the packet is routed
 *        through the each egress por, but dmac is not changed as its a mcast
 *        replication flow.
 *
 *        c1) Replicated packet egress out of cd1
 *
 *        DMAC 0x01005e010101
 *        SMAC 0x1
 *        VLAN 2
 *        DIP  228.1.1.1
 *        SIP  2.1.1.1
 *
 *        0100 5e01 0101 0000 0000 0001 8100 0002
 *        0800 4500 002e 0000 0000 3fff 92cd 0201
 *        0101 e401 0101 0001 0203 0405 0607 0809
 *        0a0b 0c0d 0e0f 1011 1213 1415 1617 1819
 *        bf2e 44c3
 *
 *        c2) Replicated packet egress out of cd2
 *
 *        DMAC 0x01005e010101
 *        SMAC 0x2
 *        VLAN 3
 *        DIP  228.1.1.1
 *        SIP  2.1.1.1
 *
 *        0100 5e01 0101 0000 0000 0002 8100 0003
 *        0800 4500 002e 0000 0000 3fff 92cd 0201
 *        0101 e401 0101 0001 0203 0405 0607 0809
 *        0a0b 0c0d 0e0f 1011 1213 1415 1617 1819
 *        bf2e 44c3
 * 
 *        c3) Replicated packet egress out of cd3
 *
 *        DMAC 0x01005e010101
 *        SMAC 0x3
 *        VLAN 4
 *        DIP  228.1.1.1
 *        SIP  2.1.1.1
 *
 *        0100 5e01 0101 0000 0000 0003 8100 0004
 *        0800 4500 002e 0000 0000 3fff 92cd 0201
 *        0101 e401 0101 0001 0203 0405 0607 0809
 *        0a0b 0c0d 0e0f 1011 1213 1415 1617 1819
 *        bf2e 44c3
 */

/* Reset C interpreter*/
cint_reset();

bcm_port_t   ingress_port;
bcm_port_t   egress_port1;
bcm_port_t   egress_port2;
bcm_port_t   egress_port3;
bcm_gport_t  ingress_gport;
bcm_gport_t  egress_gport1;
bcm_gport_t  egress_gport2;
bcm_gport_t  egress_gport3;

/*
 * This function is written so that hardcoding of port
 * numbers in Cint scripts is removed. This function gives
 * required number of ports
 */
bcm_error_t
portNumbersGet(int unit, int *port_list, int num_ports)
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
    bcm_field_entry_t        entry;
    bcm_field_group_config_t group_config; 

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDrop);
    group_config.priority = 1;
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
bcm_error_t egress_port_setup(int unit, bcm_port_t port)
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
 * Select one ingress and three egress ports and configure them in
 * Loopback mode. Install a rule to copy incoming packets to CPU and
 * additional action to drop the packets when it loops back on egress
 * ports. Start packet watcher. Ingress port setup is done in
 * ingress_port_setup(). egress port setup is done in egress_port_setup().
 */
bcm_error_t
test_setup(int unit)
{
  int port_list[4], i;

  if (BCM_E_NONE != portNumbersGet(unit, port_list, 4)) {
    printf("portNumbersGet() failed\n");
    return -1;
  }

  ingress_port = port_list[0];
  egress_port1 = port_list[1];
  egress_port2 = port_list[2];
  egress_port3 = port_list[3];

  if (BCM_E_NONE != ingress_port_setup(unit, ingress_port)) {
    printf("ingress_port_setup() failed for port %d\n", ingress_port);
    return -1;
  }

  if (BCM_E_NONE != egress_port_setup(unit, egress_port1)) {
      printf("egress_port_setup() failed for port %d\n", egress_port1);
      return -1;
  }

  if (BCM_E_NONE != egress_port_setup(unit, egress_port2)) {
      printf("egress_port_setup() failed for port %d\n", egress_port2);
      return -1;
  }

  if (BCM_E_NONE != egress_port_setup(unit, egress_port3)) {
      printf("egress_port_setup() failed for port %d\n", egress_port3);
      return -1;
  }

  bshell(unit, "pw start report +raw +decode");

  return BCM_E_NONE;
}

void
verify(int unit)
{
  char str[512];

  bshell(unit, "vlan show");
  bshell(unit, "l3 ing_intf show");
  bshell(unit, "l3 intf show");
  bshell(unit, "l3 egress show");
  bshell(unit, "multicast show");
  bshell(unit, "ipmc table show");
  bshell(unit, "clear c");

  printf("\nSending IPv4 multicast packet to ingress_port:%d\n", ingress_port);
  snprintf(str, 512, "tx 1 pbm=%d data=0x01005E0101010000060003008100000508004500002E0000000040FF91CD02010101E4010101000102030405060708090A0B0C0D0E0F10111213141516171819BF2E44C3; sleep quiet 1", ingress_port);
  bshell(unit, str);
  bshell(unit, "ipmc table show");
  bshell(unit, "show c");

  return;
}

/*
 * Configure L3 IPMC forwarding functionality.
 */
bcm_error_t config_l3_ipmc(int unit)
{
  bcm_error_t       rv;
  bcm_vlan_t        ingress_vlan = 5;
  bcm_vlan_t        egress_vlan1 = 2;
  bcm_vlan_t        egress_vlan2 = 3;
  bcm_vlan_t        egress_vlan3 = 4;
  bcm_if_t          ingress_if_in = 0x1b2;
  bcm_vrf_t         vrf = 1;
  bcm_if_t          encap_id;
  bcm_if_t          egr_l3_if_1 = 11;
  bcm_if_t          egr_l3_if_2 = 12;
  bcm_if_t          egr_l3_if_3 = 13;

  bcm_ip_t          mc_ip_addr = 0xe4010101; /* 228.1.1.1 */
  bcm_ip_t          sip_addr = 0x02010101;   /* 2.1.1.1 */

  bcm_mac_t         src_mac_1 = {0x00, 0x00, 0x00, 0x0, 0x0, 0x01};
  bcm_mac_t         src_mac_2 = {0x00, 0x00, 0x00, 0x0, 0x0, 0x02};
  bcm_mac_t         src_mac_3 = {0x00, 0x00, 0x00, 0x0, 0x0, 0x03};

  bcm_vlan_control_vlan_t  vlan_ctrl;
  bcm_multicast_encap_t    ipmc_encap;
  bcm_multicast_t          multicast_group;
  bcm_ipmc_addr_t          ipmc_entry;
  bcm_l3_ingress_t         l3_ingress;
  bcm_l3_intf_t            egr_l3_intf;

  rv = bcm_port_gport_get(unit, ingress_port, &ingress_gport);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_port_gport_get(unit, egress_port1, &egress_gport1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_port_gport_get(unit, egress_port2, &egress_gport2);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_port_gport_get(unit, egress_port3, &egress_gport3);
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

  /* Create egress_vlan_1 */
  rv = bcm_vlan_create(unit, egress_vlan1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_vlan_gport_add(unit, egress_vlan1, egress_gport1, 0);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_gport_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create egress_vlan_2 */
  rv = bcm_vlan_create(unit, egress_vlan2);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_vlan_gport_add(unit, egress_vlan2, egress_gport2, 0);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_gport_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create egress_vlan_3 */
  rv = bcm_vlan_create(unit, egress_vlan3);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_vlan_gport_add(unit, egress_vlan3, egress_gport3, 0);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_gport_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_L3, multicast_group);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_multicast_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  print multicast_group;

  /* Create L3 ingress interface for ingress side */
  bcm_l3_ingress_t_init(&l3_ingress);
  l3_ingress.flags       = BCM_L3_INGRESS_WITH_ID;
  l3_ingress.vrf         = vrf;
  rv = bcm_l3_ingress_create(unit, &l3_ingress, &ingress_if_in);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_ingress_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Config ingress vlan to l3_iif mapping for ingress side */
  bcm_vlan_control_vlan_t_init(&vlan_ctrl);
  rv = bcm_vlan_control_vlan_get(unit, ingress_vlan, &vlan_ctrl);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_control_vlan_get(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  vlan_ctrl.ingress_if = ingress_if_in;
  rv = bcm_vlan_control_vlan_set(unit, ingress_vlan, vlan_ctrl);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_control_vlan_set(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create L3 egress interface for egress side interface_1 */
  bcm_l3_intf_t_init(&egr_l3_intf);
  egr_l3_intf.l3a_flags      = BCM_L3_WITH_ID;
  egr_l3_intf.l3a_intf_id    = egr_l3_if_1;
  egr_l3_intf.l3a_vid        = egress_vlan1;
  sal_memcpy(egr_l3_intf.l3a_mac_addr, src_mac_1, sizeof(src_mac_1));
  rv =  bcm_l3_intf_create(unit, &egr_l3_intf);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_intf_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create L3 egress interface for egress side interface_2 */
  bcm_l3_intf_t_init(&egr_l3_intf);
  egr_l3_intf.l3a_flags      = BCM_L3_WITH_ID;
  egr_l3_intf.l3a_intf_id    = egr_l3_if_2;
  egr_l3_intf.l3a_vid        = egress_vlan2;
  sal_memcpy(egr_l3_intf.l3a_mac_addr, src_mac_2, sizeof(src_mac_2));
  rv =  bcm_l3_intf_create(unit, &egr_l3_intf);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_intf_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create L3 egress interface for egress side interface_3 */
  bcm_l3_intf_t_init(&egr_l3_intf);
  egr_l3_intf.l3a_flags      = BCM_L3_WITH_ID;
  egr_l3_intf.l3a_intf_id    = egr_l3_if_3;
  egr_l3_intf.l3a_vid        = egress_vlan3;
  sal_memcpy(egr_l3_intf.l3a_mac_addr, src_mac_3, sizeof(src_mac_3));
  rv =  bcm_l3_intf_create(unit, &egr_l3_intf);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_intf_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create IPMC encapsulation_id for egress side interface_1 */
  bcm_multicast_encap_t_init(&ipmc_encap);
  ipmc_encap.encap_type = bcmMulticastEncapTypeL3;
  ipmc_encap.l3_intf    = egr_l3_if_1;
  rv = bcm_multicast_encap_create(unit, &ipmc_encap, &encap_id);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_multicast_encap_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  print encap_id;
  rv = bcm_multicast_egress_add(unit, multicast_group, egress_gport1, encap_id);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_multicast_egress_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create IPMC encapsulation_id for egress side interface_2 */
  bcm_multicast_encap_t_init(&ipmc_encap);
  ipmc_encap.encap_type = bcmMulticastEncapTypeL3;
  ipmc_encap.l3_intf    = egr_l3_if_2;
  rv = bcm_multicast_encap_create(unit, &ipmc_encap, &encap_id);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_multicast_encap_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  print encap_id;
  rv = bcm_multicast_egress_add(unit, multicast_group, egress_gport2, encap_id);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_multicast_egress_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create IPMC encapsulation_id for egress side interface_3 */
  bcm_multicast_encap_t_init(&ipmc_encap);
  ipmc_encap.encap_type = bcmMulticastEncapTypeL3;
  ipmc_encap.l3_intf    = egr_l3_if_3;
  rv = bcm_multicast_encap_create(unit, &ipmc_encap, &encap_id);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_multicast_encap_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  print encap_id;
  rv = bcm_multicast_egress_add(unit, multicast_group, egress_gport3, encap_id);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_multicast_egress_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create an IPv4 host SG entry. */
  bcm_ipmc_addr_t_init(&ipmc_entry);
  ipmc_entry.mc_ip_addr = mc_ip_addr;
  ipmc_entry.mc_ip_mask = 0xFFFFFFFF;
  ipmc_entry.s_ip_addr  = sip_addr;
  ipmc_entry.vrf        = vrf;
  ipmc_entry.mtu        = 1500;
  ipmc_entry.rp_id      = 0;
  ipmc_entry.l3a_intf   = 0;
  ipmc_entry.ing_intf   = ingress_if_in;
  ipmc_entry.group      = multicast_group;
  rv = bcm_ipmc_add(unit, &ipmc_entry);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_ipmc_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  return BCM_E_NONE;
}

/*
 * execute:
 *  This functions does the following
 *  a)test setup
 *  b)actual configuration (Done in config_l3_ipmc())
 *  c)demonstrates the functionality(done in verify()).
 */
bcm_error_t
execute(void)
{
    bcm_error_t rv;
    int unit = 0;
    print "config show; attach; version";
    bshell(unit, "config show; a ; version");
    print "~~~ #1) test_setup(): ** start **";
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return -1;
    }
    print "~~~ #1) test_setup(): ** end **";

    print "~~~ #2) config_l3_ipmc(): ** start **";
    if (BCM_FAILURE((rv = config_l3_ipmc(unit)))) {
        printf("config_l3_ipmc() failed.\n");
        return -1;
    }
    print "~~~ #2) config_l3_ipmc(): ** end **";

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

