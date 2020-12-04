/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : L2 Multicast
 *
 *  Usage    : BCM.0> cint l2_mc.c
 *
 *  config   : l2_config.bcm
 *
 *  Log file : l2_mc_log.txt
 *
 *  Test Topology :
 *
 *                   +------------------------------+  egress_port1
 *                   |                              +-----------------+
 *                   |                              |
 *                   |                              |
 *  ingress_port     |                              |  egress_port2
 *  +----------------+          SWITCH              +-----------------+
 *                   |                              |
 *                   |                              |
 *                   |                              |  egress_port3
 *                   |                              +-----------------+
 *                   |                              |
 *                   +------------------------------+
 *
 *
 *  Summary:
 *  ========
 *    This CINT configures L2 multicast using BCM APIs.
 *    This example shows the steps to set up L2 multicast along with Port
 *    Filtering Mode(PFM) on a per Vlan basis. The PFM controls the forwarding of
 *    known and unknown multicast packets.
 *
 *    PFM
 *    ===
 *    0 - All multicast packets are flooded to the entire Vlan
 *    1 - Known multicast packets are forwarded only to the ports in the
 *        multicast group. Unknown multicast packets are flooded to the Vlan.
 *    2 - Known multicast packets are forwarded only to the ports in the
 *        multicast group. Unknown multicast packets are dropped.
 *
 *  Detailed steps done in the CINT script:
 *  =======================================
 *    1) Step1 - Test Setup (Done in test_setup()):
 *    =============================================
 *      a) Select one ingress and three egress ports and configure them in
 *         Loopback mode. Install a rule to copy incoming packets to CPU and
 *         additional action to drop the packets when it loops back on egress
 *         ports. Start packet watcher.
 *
 *    2) Step2 - Configuration (Done in configure_l2_mc())
 *    ====================================================
 *      a) Create a VLAN(2) and add egress_port1, egress_port2 and egress_port3
 *         as members.
 *      b) Create a L2 multicast group and add egress_port1, egress_port2 and egress_port3 to
 *         this L2 multicast group.
 *      c) Configure the PFM mode as BCM_VLAN_MCAST_FLOOD_NONE(mode 2) for VLAN(2).
 *      d) Add ingress_port as member of vlan(2) and add an entry into L2 table
 *         specifying the L2 multicast group as destination for the multicast MAC
 *         address.
 *
 *    3) Step3 - Verification (Done in verify()):
 *    ===========================================
 *      a) Transmit the below known multicast packet on ingress_port.
 *         Packet:
 *         ======
 *         Ethernet II, Src: Xerox_00:00:00 (00:00:04:00:00:00), Dst: Xerox_00:01:00 (01:00:04:00:01:00)
 *         802.1Q Virtual LAN, PRI: 0, CFI: 0, ID: 2
 *
 *          0100 0400 0100 0000 0400 0000 8100 0002
 *          0800 0001 0203 0405 0607 0809 0a0b 0c0d
 *          0e0f 1011 1213 1415 1617 1819 1a1b 1c1d
 *          1e1f 2021 2223 2425 2627 2829 2a2b 2c2d
 *          8e0b ec9e 1cdf 4421
 *
 *      b) Transmit the below unknown multicast packet on ingress_port and verify that the
 *         packet is dropped as PFM for VLAN(2) is set to 2.
 *
 *         Packet:
 *         ======
 *         Ethernet II, Src: Xerox_00:00:00 (00:00:04:00:00:00), Dst: Xerox_00:01:00 (01:00:03:00:01:00)
 *         802.1Q Virtual LAN, PRI: 0, CFI: 0, ID: 2
 *
 *         0100 0300 0100 0000 0400 0000 8100 0002
 *         0800 0001 0203 0405 0607 0809 0a0b 0c0d
 *         0e0f 1011 1213 1415 1617 1819 1a1b 1c1d
 *         1e1f 2021 2223 2425 2627 2829 2a2b 2c2d
 *         5ca6 4d2a 1cdf 4421
 *
 *      c) Expected Result:
 *      ===================
 *         After Step 3.a, the packet egresses out of egress_port1, egress_port2 and egress_port3.
 *         This can be observed using "show counters" and/or packet watcher dump on console.
 *
 *         After step 3.b, it can be observed that packet is dropped as PFM for VLAN(2) is set to
 *         BCM_VLAN_MCAST_FLOOD_NONE(mode 2)
 */
cint_reset();

bcm_port_t ingress_port;
bcm_port_t egress_port1, egress_port2, egress_port3;

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
/*   Select one ingress and three egress ports and configure them in
 *   Loopback mode. Install a rule to copy incoming packets to CPU and
 *   additional action to drop the packets when it loops back on egress
 *   ports. Start packet watcher. Ingress port setup is done in
 *   ingress_port_setup(). egress port setup is done in egress_port_setup().
 */
bcm_error_t test_setup(int unit)
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

  for (i = 1; i <= 3; i++) {
    if (BCM_E_NONE != egress_port_setup(unit, port_list[i])) {
      printf("egress_port_setup() failed for port %d\n", port_list[i]);
      return -1;
    }
  }

  bshell(unit, "pw start report +raw +decode");
  return BCM_E_NONE;
}

/*     This function does the following.
 *     Transmit the below known multicast packet on ingress_port and verify
 *     that the packet egresses out of egress_port1, egress_port2 and egress_port3 using
 *     "show counters" and/or packet watcher dump on console.
 *       Packet:
 *       ======
 *       Ethernet II, Src: Xerox_00:00:00 (00:00:04:00:00:00), Dst: Xerox_00:01:00 (01:00:04:00:01:00)
 *       802.1Q Virtual LAN, PRI: 0, CFI: 0, ID: 2
 *
 *        0100 0400 0100 0000 0400 0000 8100 0002
 *        0800 0001 0203 0405 0607 0809 0a0b 0c0d
 *        0e0f 1011 1213 1415 1617 1819 1a1b 1c1d
 *        1e1f 2021 2223 2425 2627 2829 2a2b 2c2d
 *        8e0b ec9e 1cdf 4421
 *
 *     Transmit the below unknown multicast packet on ingress_port and verify that the
 *     packet is dropped as PFM for VLAN(2) is set to 2.
 *
 *        Packet:
 *        ======
 *        Ethernet II, Src: Xerox_00:00:00 (00:00:04:00:00:00), Dst: Xerox_00:01:00 (01:00:03:00:01:00)
 *        802.1Q Virtual LAN, PRI: 0, CFI: 0, ID: 2
 *
 *        0100 0300 0100 0000 0400 0000 8100 0002
 *        0800 0001 0203 0405 0607 0809 0a0b 0c0d
 *        0e0f 1011 1213 1415 1617 1819 1a1b 1c1d
 *        1e1f 2021 2223 2425 2627 2829 2a2b 2c2d
 *        5ca6 4d2a 1cdf 4421
 */
void verify(int unit)
{
  char   str[512];
  bshell(unit, "hm ieee");
  printf("Transmitting known multicast packet on ingress_port:%d\n", ingress_port);
  snprintf(str, 512, "tx 1 pbm=%d data=0x010004000100000004000000810000020800000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D8E0BEC9E; sleep 1", ingress_port);
  bshell(unit, str);
  printf("Executing 'show c'\n");
  bshell(unit, "show c");
  bshell(unit, "clear c");


  printf("Transmitting unknown multicast packet on ingress_port:%d\n", ingress_port);
  snprintf(str, 512, "tx 1 pbm=%d data=0x010003000100000004000000810000020800000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D5CA64D2A; sleep 1", ingress_port);
  bshell(unit, str);
  printf("Executing 'show c'\n");
  bshell(unit, "show c");
  bshell(unit, "clear c");

}

/* Create vlan and add port to vlan */
int
create_vlan_add_port(int unit, bcm_vlan_t vlan, bcm_port_t port)
{
    bcm_error_t rv;
    bcm_pbmp_t pbmp, ubmp;

    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);

    rv = bcm_vlan_create(unit, vlan);
    if ((BCM_FAILURE(rv)) & (rv != BCM_E_EXISTS)) {
        printf("Error in configuring vlan : %s.\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_vlan_port_add(unit, vlan, pbmp, ubmp);
    if (BCM_FAILURE(rv)) {
        printf("Error executing bcm_vlan_port_add(): %s.\n", bcm_errmsg(rv));
        return rv;
    }
    return BCM_E_NONE;
}

/* Adding egress ports to multicast group */
int
multicast_egress_add(int unit, bcm_multicast_t l2mc_group, bcm_port_t port,
                     bcm_vlan_t vlan)
{
    bcm_error_t rv;
    bcm_gport_t gport;
    bcm_if_t encap;

    rv = bcm_port_gport_get(unit, port, &gport);
    if (BCM_FAILURE(rv)) {
        printf("Error in gport get : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Getting encap for L2 multicast */
    rv = bcm_multicast_l2_encap_get(unit, l2mc_group, gport, vlan, &encap);
    if (BCM_FAILURE(rv)) {
        printf("Error in encap get : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Adding port to multicast index */
    rv = bcm_multicast_egress_add(unit, l2mc_group, gport, encap);
    if (BCM_FAILURE(rv)) {
        printf("Error in egress add : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Configuring L2 Multicast */
int
configure_multicast(int unit, bcm_vlan_t egr_vlan, bcm_multicast_t* l2mc_group)
{
    bcm_error_t rv;
    int num_ports = 3;
    bcm_port_t egr_port[num_ports] = {egress_port1, egress_port2, egress_port3};
    int i;

    /* Create multicast group */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_L2, l2mc_group);
    if (BCM_FAILURE(rv)) {
        printf("Error in creating multicast group : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    for(i = 0; i < num_ports; i++) {
        rv = create_vlan_add_port(unit, egr_vlan, egr_port[i]);
        if (BCM_FAILURE(rv)) {
            printf("Error in configuring vlan : %s.\n", bcm_errmsg(rv));
            return rv;
        }
        rv = multicast_egress_add(unit, *l2mc_group, egr_port[i], egr_vlan);
        if (BCM_FAILURE(rv)) {
            printf("Error in multicast egress add : %s.\n", bcm_errmsg(rv));
            return rv;
        }
    }

    return BCM_E_NONE;
}

/* Configuring Port Filtering Mode */
int
configure_pfm(int unit, bcm_vlan_mcast_flood_t mode, bcm_vlan_t vlan,
              bcm_multicast_t l2mc_group)
{
    bcm_error_t rv;
    bcm_vlan_control_vlan_t vlan_ctrl;
    int valid_field = BCM_VLAN_CONTROL_VLAN_L2_MCAST_FLOOD_MODE_MASK;

    bcm_vlan_control_vlan_t_init(&vlan_ctrl);
    vlan_ctrl.l2_mcast_flood_mode = mode;

    rv = bcm_vlan_control_vlan_selective_set(unit, vlan, valid_field, &vlan_ctrl);
    if(BCM_FAILURE(rv)) {
        printf("\nError in configuring vlan control: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Adding multicast mac in L2 entry */
int
add_multicast_mac(int unit, bcm_mac_t mc_mac, bcm_vlan_t ing_vlan,
                  bcm_port_t ing_port, bcm_multicast_t l2mc_group)
{
    bcm_error_t rv;
    bcm_l2_addr_t l2_addr;

    rv = create_vlan_add_port(unit, ing_vlan, ing_port);
    if (BCM_FAILURE(rv)) {
        printf("Error in adding ingress port to vlan : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    bcm_l2_addr_t_init(l2_addr, mc_mac, ing_vlan);

    l2_addr.flags =  BCM_L2_MCAST;
    l2_addr.l2mc_group = l2mc_group;

    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (BCM_FAILURE(rv)) {
        printf("Error executing bcm_l2_addr_add() : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}
/*    1)Create a VLAN(2) and add egress_port1, egress_port2 and egress_port3
 *      as members.
 *    2)Create a L2 multicast group and add the L2 multicast encap IDs to
 *      this L2 multicast group.
 *    3)Configure the PFM mode as BCM_VLAN_MCAST_FLOOD_NONE(mode 2).
 *    4)Add ingress_port as member of vlan(2) and add an entry into L2 table
 *      specifying the L2 multicast group as destination for the multicast MAC
 *       address.
 */

bcm_error_t configure_l2_mc(int unit)
{
    bcm_error_t rv;
    bcm_vlan_mcast_flood_t mode = BCM_VLAN_MCAST_FLOOD_NONE; /* Mode 2 */
    bcm_mac_t l2mc_mac = {0x01, 0x00, 0x04, 0x00, 0x01, 0x00};
    bcm_vlan_t vlan = 2;
    bcm_port_t ing_port = ingress_port;
    bcm_multicast_t mc_group;

    rv = configure_multicast(unit, vlan, &mc_group);
    if(BCM_FAILURE(rv)) {
        printf("\nError in configuring L2 multicast: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Configuring Port Filtering Mode */

    /*
     * Mode 0 - BCM_VLAN_MCAST_FLOOD_ALL
     * Mode 1 - BCM_VLAN_MCAST_FLOOD_UNKNOWN
     * Mode 2 - BCM_VLAN_MCAST_FLOOD_NONE
     */

    rv = configure_pfm(unit, mode, vlan, mc_group);
    if(BCM_FAILURE(rv)) {
        printf("\nError in configuring PFM for L2 multicast: %s.\n",
                bcm_errmsg(rv));
        return rv;
    }

    rv = add_multicast_mac(unit, l2mc_mac, vlan, ing_port, mc_group);
    if(BCM_FAILURE(rv)) {
        printf("\nError in adding multicast mac: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}
/*
 * This functions does the following
 * a)test setup
 * b)actual configuration (Done in configure_l2_mc())
 * c)demonstrates the functionality(done in verify()).
 */
bcm_error_t execute()
{
  bcm_error_t rv;
  int unit =0;

  bshell(unit, "config show; a ; version");

  if (BCM_FAILURE((rv = test_setup(unit)))) {
    printf("test_setup() failed.\n");
    return -1;
  }
  printf("Completed test setup successfully.\n");

  if (BCM_FAILURE((rv = configure_l2_mc(unit)))) {
    printf("configure_l2_mc() failed.\n");
    return -1;
  }
  printf("Completed configuration(i.e executing configure_l2_mc()) successfully.\n");

  verify(unit);
  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}
