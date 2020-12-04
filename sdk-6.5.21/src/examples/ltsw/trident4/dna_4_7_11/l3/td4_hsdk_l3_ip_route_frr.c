/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : L3 IPv4 route entry protection path
 *
 * Usage    : BCM.0> cint td4_hsdk_l3_ip_route_frr.c 
 *
 * config   : bcm56880_a0-generic-l3.config.yml  
 *
 * Log file : td4_hsdk_l3_ip_route_frr_log.txt
 *
 * Test Topology :
 *
 *                   +------------------------------+
 *                   |                              |
 *                   |                              | egress_primary_port cd1
 *                   |                              +-----------------------+
 *  ingress_port cd0 |                              |
 * +-----------------+          SWITCH-TD4          |
 *                   |                              |
 *                   |                              | egress_backup_port cd2
 *                   |                              +-----------------------+
 *                   |                              |
 *                   |                              |
 *                   +------------------------------+
 * Summary:
 * ========
 *   This Cint example to show configuration of the IPv4 route with failover
 *   scenario using BCM APIs.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects three ports and configure them in Loopback mode. Out of these
 *        three ports, one port is used as ingress_port and the other two ports as
 *        egress_ports(primary & backup).
 *
 *     b) Install an IFP rule to copy incoming packets to CPU and start
 *        packet watcher.
 *
 *     Note: IFP rule is meant for a testing purpose only (Internal) and it is
 *           nothing to do with an actual functional test.
 *
 *   2) Step2 - Configuration (Done in config_l3_ipv4_route_frr()):
 *   ==============================================================
 *      a) Create ingress_vlan and add ingress_port as member.
 *
 *      b) Create primary egress_vlan and add primary egress_port as member.
 *
 *      c) Create backup egress_vlan and add backup egress_port as member. 
 *
 *      d) Create L3 ingress interface and L3 egress interface for ingress side 
 *         and bind L3 ingress interface with ingress vlan and cretae L3 egr obj
 *         for ingress side.
 *
 *      e) Create L3 egress interface and L3 egress object for primary egress path.
 *
 *      f) Create L3 egress interface and L3 egress object for backup egress path by 
 *         passing primary egress object reference to it.
 *
 *      g) Add route entry for DIP by primary L3 egress object.
 *
 *      h) Create station MAC entry for L3 forwarding.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show', 'l3 ing_intf show',
 *        'l3 intf show', 'l3 egress show', 'l3 route show'.
 *
 *     b) Transmit the known IPv4 unicast packet on ingress port cd0. 
 *
 *        DMAC 0xAAAA
 *        SMAC 0x2222
 *        VLAN 12
 *        DIP 192.168.10.1
 *        SIP 10.10.10.10
 *        
 *        0000 0000 aaaa 0000 0000 2222 8100 000c
 *        0800 4500 002e 0000 0000 40ff 9b14 0a0a
 *        0a0a c0a8 0a01 0000 0000 0000 0000 0000
 *        0000 0000 0000 0000 0000 0000 0000 0000
 *        ab47 b8cf 
 *
 *      c) Now switchover from primary egress interface to secondary egress interface 
 *
 *      d) Transmit the above known IPv4 unicast packet again on same ingress port cd0.
 *
 *     c) Expected Result:
 *     ===================
 *       We can see that dmac, smac and vlan are all changed as the packet is
 *       routed through the egress port.
 *
 *       a) After step 3.b, verify that the below packet egress out of cd1 
 *
 *          DMAC 0x1111
 *          SMAC 0xBBBB
 *          VLAN 13
 *          DIP 192.168.10.1
 *          SIP 10.10.10.10
 *
 *          0000 0000 1111 0000 0000 bbbb 8100 000d
 *          0800 4500 002e 0000 0000 3fff 9c14 0a0a
 *          0a0a c0a8 0a01 0000 0000 0000 0000 0000
 *          0000 0000 0000 0000 0000 0000 0000 0000
 *          ab47 b8cf  
 *
 *       b) After step 3.d, verify that the below packet egress out of cd2
 *
 *          DMAC 0x3333
 *          SMAC 0xCCCC
 *          VLAN 14
 *          DIP 192.168.10.1
 *          SIP 10.10.10.10
 *
 *          0000 0000 3333 0000 0000 cccc 8100 000e
 *          0800 4500 002e 0000 0000 3fff 9c14 0a0a
 *          0a0a c0a8 0a01 0000 0000 0000 0000 0000
 *          0000 0000 0000 0000 0000 0000 0000 0000
 *          ab47 b8cf
 */

/* Reset C interpreter*/
cint_reset();

bcm_port_t   ingress_port;
bcm_port_t   egress_primary_port;
bcm_port_t   egress_backup_port;
bcm_gport_t  ingress_gport;
bcm_gport_t  egress_primary_gport;
bcm_gport_t  egress_backup_gport;
bcm_if_t     l3_egr_id_primary;

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
    bcm_pbmp_t ports_pbmp;

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
 * Test Setup:
 * This functions gets the port numbers and sets up ingress and
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
bcm_error_t
test_setup(int unit)
{
    int port_list[3], i;

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 3)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    ingress_port        = port_list[0];
    egress_primary_port = port_list[1];
    egress_backup_port  = port_list[2];

    if (BCM_E_NONE != ingress_port_setup(unit, ingress_port)) {
        printf("ingress_port_setup() failed for port %d\n", ingress_port);
        return -1;
    }

    if (BCM_E_NONE != egress_port_setup(unit, egress_primary_port)) {
        printf("egress_port_setup() failed for port %d\n", egress_primary_port);
        return -1;
    }

    if (BCM_E_NONE != egress_port_setup(unit, egress_backup_port)) {
        printf("egress_port_setup() failed for port %d\n", egress_backup_port);
        return -1;
    }

    bshell(unit, "pw start report +raw +decode");

    return BCM_E_NONE;
}

int verify(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    char        str[512];

    bshell(unit, "vlan show");
    bshell(unit, "l3 ing_intf show");
    bshell(unit, "l3 intf show");
    bshell(unit, "l3 egress show");
    bshell(unit, "l3 route show");
    bshell(unit, "clear c");

    printf("\nSending IPv4 unicast packet to ingress_port:%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000aaaa0000000022228100000c08004500002e0000000040ff9b140a0a0a0ac0a80a010000000000000000000000000000000000000000000000000000ab47b8cf; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "show c");

    /* Now do switch over from prmary to backup path */ 
    printf("\nSwitch-over from primary-path to backup/protection-path\n");
    rv = bcm_failover_egress_status_set(unit, l3_egr_id_primary, 1);
    if (BCM_FAILURE(rv)) {
        printf("Error executing bcm_failover_egress_status_set(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("\nRe-Sending IPv4 unicast packet to ingress_port:%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000aaaa0000000022228100000c08004500002e0000000040ff9b140a0a0a0ac0a80a010000000000000000000000000000000000000000000000000000ab47b8cf; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "show c");

    return BCM_E_NONE;
}

bcm_error_t
config_l3_route_frr(int unit)
{
  bcm_error_t       rv;
  bcm_vlan_t        vid_in = 12;
  bcm_vlan_t        vid_egr_primary = 13;
  bcm_vlan_t        vid_egr_backup = 14;
  bcm_if_t          ingress_if_in = 100;
  bcm_vrf_t         vrf = 1;
  bcm_if_t          intf_id_in = 201;
  bcm_if_t          intf_id_primary = 202;
  bcm_if_t          intf_id_backup = 203;
  bcm_if_t          l3_egr_id_in;
  bcm_if_t          l3_egr_id_backup;
  bcm_if_t          l3_egr_id_def;
  int               station_id;

  bcm_l3_ingress_t  l3_ingress;
  bcm_l3_intf_t     ing_l3_intf;
  bcm_l3_egress_t   l3_egress;
  bcm_l3_egress_t   l3_egress_backup;
  bcm_l3_route_t    route;
  bcm_l2_station_t  station;

  bcm_vlan_control_vlan_t vlan_ctrl;

  bcm_mac_t smac_backup = {0x00, 0x00, 0x00, 0x00, 0xCC, 0xCC};
  bcm_mac_t dmac_backup = {0x00, 0x00, 0x00, 0x00, 0x33, 0x33};
  bcm_mac_t router_mac_egr = {0x00, 0x00, 0x00, 0x00, 0xBB, 0xBB};
  bcm_mac_t router_mac_in = {0x00, 0x00, 0x00, 0x00, 0xAA, 0xAA};
  bcm_mac_t nxt_hop_egr = {0x00, 0x00, 0x00, 0x00, 0x11, 0x11};
  bcm_mac_t nxt_hop_in = {0x00, 0x00, 0x00, 0x00, 0x22, 0x22};
  bcm_mac_t mac_mask = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

  bcm_ip_t   dip = 0xC0A80A01;  /* 192.168.10.1 */
  bcm_ip_t   mask = 0xffffff00;  /* 255.255.255.0 */

  rv = bcm_port_gport_get(unit, ingress_port, &ingress_gport);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_port_gport_get(unit, egress_primary_port, &egress_primary_gport);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_port_gport_get(unit, egress_backup_port, &egress_backup_gport);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create ingress_vlan  */
  rv = bcm_vlan_create(unit, vid_in);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_vlan_gport_add(unit, vid_in, ingress_gport, 0);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_gport_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create egress vlan for primary path */
  rv = bcm_vlan_create(unit, vid_egr_primary);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_vlan_gport_add(unit, vid_egr_primary, egress_primary_gport, 0);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_gport_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create egress vlan for backup path */
  rv = bcm_vlan_create(unit, vid_egr_backup);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_vlan_gport_add(unit, vid_egr_backup, egress_backup_gport, 0);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_gport_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create L3 ingress interface for ingress side */
  bcm_l3_ingress_t_init(&l3_ingress);
  l3_ingress.flags       = BCM_L3_INGRESS_WITH_ID;
  l3_ingress.vrf         = vrf;
  rv = bcm_l3_ingress_create(unit, &l3_ingress, &ingress_if_in);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_ingress_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Config vlan_id to l3_iif mapping for ingress side */
  bcm_vlan_control_vlan_t_init(&vlan_ctrl);
  rv = bcm_vlan_control_vlan_get(unit, vid_in, &vlan_ctrl);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_control_vlan_get(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  vlan_ctrl.ingress_if = ingress_if_in;
  rv = bcm_vlan_control_vlan_set(unit, vid_in, vlan_ctrl);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_control_vlan_set(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create egr L3 interface for ingress side */
  bcm_l3_intf_t_init(&ing_l3_intf);
  ing_l3_intf.l3a_flags   = BCM_L3_WITH_ID;
  ing_l3_intf.l3a_intf_id = intf_id_in;
  ing_l3_intf.l3a_vid     = vid_in;
  ing_l3_intf.l3a_vrf     = vrf;
  ing_l3_intf.l3a_mtu     = 1500;
  sal_memcpy(ing_l3_intf.l3a_mac_addr, router_mac_in, sizeof(router_mac_in));
  rv = bcm_l3_intf_create(unit, &ing_l3_intf);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_intf_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create egr L3 interface for primary path */
  bcm_l3_intf_t_init(&ing_l3_intf);
  ing_l3_intf.l3a_flags   = BCM_L3_WITH_ID;
  ing_l3_intf.l3a_intf_id = intf_id_primary;
  ing_l3_intf.l3a_vid     = vid_egr_primary;
  ing_l3_intf.l3a_vrf     = vrf;
  ing_l3_intf.l3a_mtu     = 1500;
  sal_memcpy(ing_l3_intf.l3a_mac_addr, router_mac_egr, sizeof(router_mac_egr));
  rv = bcm_l3_intf_create(unit, &ing_l3_intf);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_intf_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create egr L3 interface for backup path */
  bcm_l3_intf_t_init(&ing_l3_intf);
  ing_l3_intf.l3a_flags   = BCM_L3_WITH_ID;
  ing_l3_intf.l3a_intf_id = intf_id_backup;
  ing_l3_intf.l3a_vid     = vid_egr_backup;
  ing_l3_intf.l3a_vrf     = vrf;
  ing_l3_intf.l3a_mtu     = 1500;
  sal_memcpy(ing_l3_intf.l3a_mac_addr, smac_backup, sizeof(smac_backup));
  rv = bcm_l3_intf_create(unit, &ing_l3_intf);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_intf_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create default egress object */
  bcm_l3_egress_t_init(&l3_egress);
  l3_egress.intf  = intf_id_in;
  l3_egress.flags = BCM_L3_COPY_TO_CPU | BCM_L3_DST_DISCARD;
  rv = bcm_l3_egress_create(unit, 0, &l3_egress, &l3_egr_id_def);
  if (BCM_FAILURE(rv)) {
      printf("Error in configuring def L3 egress object : %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create L3 egress object for ingress side */
  bcm_l3_egress_t_init(&l3_egress);
  l3_egress.port  = ingress_gport;
  l3_egress.intf  = intf_id_in;
  l3_egress.mtu   = 1500;
  sal_memcpy(l3_egress.mac_addr, nxt_hop_in, sizeof(nxt_hop_in));
  rv = bcm_l3_egress_create(unit, 0, &l3_egress, &l3_egr_id_in);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_egress_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  print l3_egr_id_in;

  /* Create L3 egress object for primary egress path */
  bcm_l3_egress_t_init(&l3_egress);
  l3_egress.port     = egress_primary_gport;
  l3_egress.intf     = intf_id_primary;
  l3_egress.mtu      = 1500;
  sal_memcpy(l3_egress.mac_addr, nxt_hop_egr, sizeof(nxt_hop_egr));
  rv = bcm_l3_egress_create(unit, 0, &l3_egress, &l3_egr_id_primary);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_egress_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  print l3_egr_id_primary;

  /* Initialize failover software module */ 
  rv = bcm_failover_init(unit); 
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_failover_init(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create L3 egress object for backup egress path */
  bcm_l3_egress_t_init(&l3_egress_backup);
  l3_egress_backup.port  = egress_backup_gport;
  l3_egress_backup.intf  = intf_id_backup;
  l3_egress_backup.mtu   = 1500;
  sal_memcpy(l3_egress_backup.mac_addr, dmac_backup, sizeof(dmac_backup));
  rv = bcm_failover_egress_set(unit, l3_egr_id_primary, &l3_egress_backup);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_failover_egress_set(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Add L3 default Route in the system fisrt */
  bcm_l3_route_t_init(&route);
  route.l3a_subnet  = 0x00000000;  /* 0.0.0.0 */
  route.l3a_ip_mask = 0x00000000;  /* 0.0.0.0 */
  route.l3a_intf    = l3_egr_id_def;
  route.l3a_vrf     = vrf;
  rv = bcm_l3_route_add(unit, &route);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_route_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Add L3 Route for tunnel initiation */
  bcm_l3_route_t_init(&route);
  route.l3a_subnet  = dip;
  route.l3a_ip_mask = mask;
  route.l3a_intf    = l3_egr_id_primary;
  route.l3a_vrf     = vrf;
  rv = bcm_l3_route_add(unit, &route);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_route_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  bcm_l2_station_t_init(&station);
  station.vlan      = vid_in;
  station.vlan_mask = vid_in ? 0xfff: 0x0;
  sal_memcpy(station.dst_mac, router_mac_in, sizeof(router_mac_in));
  sal_memcpy(station.dst_mac_mask, mac_mask, sizeof(mac_mask));
  rv = bcm_l2_station_add(unit, &station_id, &station);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l2_station_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  return BCM_E_NONE;
}

/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in config_l3_ipv4_route_frr())
 *  c) demonstrates the functionality(done in verify()).
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

    print "~~~ #2) config_l3_route_frr(): ** start **";
    if (BCM_FAILURE((rv = config_l3_route_frr(unit)))) {
        printf("config_l3_route_frr() failed.\n");
        return -1;
    }
    print "~~~ #2) config_l3_route_frr(): ** end **";

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

