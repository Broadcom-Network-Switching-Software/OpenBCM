/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : GPON
 *
 *  Usage    : BCM.0> cint vlan_gpon_config.c
 *
 *  Config   : BCM56275_A1-PORT.bcm
 *
 *  Log file : vlan_gpon_config_log.txt
 *
 *  Test Topology :
 *                            (GPON ports)
 *              +-----------+  gemid1
 *              |           +-----------+
 *    Network   |           |  gemid2
 *    +---------+    DUT    +-----------+
 *    Port      |           |  gemid3
 *              |           +-----------+
 *              +-----------+
 *    +-----+                  +-----+
 *    | DMAC|                  | DMAC|
 *    +-----+                  +-----+
 *    | SMAC|                  | SMAC|
 *    +-----+                  +-----+
 *    | OTAG|                  |GemID|
 *    +-----+                  +-----+
 *    | ITAG|                  |ITAG'|
 *    +-----+                  +-----+
 *    |     |                  |     |
 *    |     |                  |     |
 *    +-----+                  +-----+
 *
 *  Summary:
 *  ========
 *    This CINT configures GPON unicast and multicast flows using BCM APIs.
 *
 *  Detailed steps done in the CINT script:
 *  =======================================
 *    1) Step1 - Test Setup (Done in test_setup()):
 *    =============================================
 *      a) Select any four ports(Use one as network_port and 3 as GPON ports) and
 *         configure them in Loopback mode. Install a rule to copy incoming packets to CPU and
 *         discard packets when it loops back on egress ports. Start packet watcher.
 *
 *    2) Step2 - Configuration (Done in configure_vlan_gpon())
 *    ========================================================
 *      a) Create VLAN (4000) and add network_port and GPON ports as members.
 *
 *      b) Create VLAN VP ports with match as gemid and CVID/IVID on gpon_port1, gpon_port2
 *         and gpon_port3.
 *
 *      c) Confugure egress vlan translation rules on VLAN VP ports to replace outer VLAN with
 *         gemid and add inner vlan with CVID.
 *
 *      d) Create a multicast group of type(BCM_MULTICAST_TYPE_VLAN) and add VLAN VP ports and
 *         network_port as members to it. Congigure this multicast group as the broadcast_group,
 *         unknown_multicast_group and unknown_unicast_group for VLAN(4000).
 *
 *      e) On network port,
 *         Add ingress vlan translate rules to match on SVID(500), CVID(501) to replace
 *         outer VLAN with 4000 and delete inner VLAN.
 *         Add egress vlan translate rules to match on OVID(4000) to replace outer VLAN
 *         with SVID(500) and add CVID(501).
 *
 *      f) On gpon_ports,
 *         Add ingress vlan translate rules to match on gemid and cvid to replace
 *         outer VLAN with 4000 and delete inner VLAN.
 *
 *      g) Add static unicast and multicast entries with VLAN 4000 in L2 table.
 *
 *    3) Step3 - Verification (Done in verify()):
 *    ===========================================
 *      a) Send a known unicast double tagged packet(outer VLAN:500 inner vlan: 501) on network_port.
 *
 *      b) Send a known multicast double tagged packet(outer VLAN:500 inner vlan: 501) on network_port.
 *
 *      c) Send a known multicast double tagged packet(outer VLAN:500 inner vlan: 501) on network_port.
 *
 *      d) Expected Result:
 *      ===================
 *         After step 3.a,
 *         double tagged packet(outer VLAN:101, inner vlan:25) egresses out of gpon_port1.
 *
 *         After step 3.b,
 *          double tagged packet(outer VLAN:101, inner vlan:25) egresses out of gpon_port1.
 *          double tagged packet(outer VLAN:102, inner vlan:35) egresses out of gpon_port2.
 *          double tagged packet(outer VLAN:103, inner vlan:45) egresses out of gpon_port3.
 *
 *         After step 3.c,
 *          double tagged packet(outer VLAN:101, inner vlan:25) egresses out of gpon_port1.
 *          double tagged packet(outer VLAN:102, inner vlan:35) egresses out of gpon_port2.
 *          double tagged packet(outer VLAN:103, inner vlan:45) egresses out of gpon_port3.
*/
/*reset cint*/
cint_reset();

bcm_port_t network_port;
bcm_port_t gpon_port1, gpon_port2, gpon_port3;
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
    BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset, BCM_FIELD_GROUP_PRIO_ANY, &group));
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
    BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset, BCM_FIELD_GROUP_PRIO_ANY, &group));
  }
  BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &entry));

  BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));

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

  network_port = port_list[0];
  gpon_port1 = port_list[1];
  gpon_port2 = port_list[2];
  gpon_port3 = port_list[3];

  if (BCM_E_NONE != ingress_port_setup(unit, network_port)) {
    printf("ingress_port_setup() failed for port %d\n", network_port);
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

/* create the vlan virtual ports */
int
create_gpon_vp(int unit, bcm_vlan_t *gemid, bcm_vlan_t *cvid, bcm_vlan_t vid,
                    bcm_gport_t *gpon_port, bcm_vlan_port_t *vlan_port)
{
    int i;

    /* GPON -> network, match on stacked VLAN, translate to client ID on ingress */
    for (i = 0; i < 3; i++) {
        bcm_vlan_port_t_init(vlan_port[i]);
        vlan_port[i].match_vlan = gemid[i];
        vlan_port[i].match_inner_vlan = cvid[i];
        vlan_port[i].criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
        vlan_port[i].egress_vlan = vid;
        BCM_GPORT_LOCAL_SET(vlan_port[i].port, gpon_port[i]);

        BCM_IF_ERROR_RETURN(bcm_vlan_port_create(unit, vlan_port[i]));
    }
    return BCM_E_NONE;
}

/*GPON port setup*/
int
create_gpon_egr_tag(int unit, bcm_vlan_t *gemid, bcm_vlan_t *cvid, bcm_vlan_t vid,
                           bcm_vlan_port_t *vlan_port)
{
    /* create egress translation entries for virtual ports to do VLAN tag manipulation
     * i.e. egress with gem_id + cvid */
    int i;
    bcm_vlan_action_set_t action;
    for (i = 0; i < 3; i++) {
        bcm_vlan_action_set_t_init(action);

        /* for outer tagged packet => outer tag replaced with gem_id */
        action.ot_outer = bcmVlanActionReplace;
        action.new_outer_vlan = gemid[i];

        /* for outer tagged packet => inner tag added with cvid */
        action.ot_inner = bcmVlanActionAdd;
        action.new_inner_vlan = cvid[i];

        BCM_IF_ERROR_RETURN(bcm_vlan_translate_egress_action_add(unit, vlan_port[i].vlan_port_id, vid, 0, &action));
    }
    return BCM_E_NONE;
}

/*L2 lookup address for knoen traffic*/
int
create_l2_addr(int unit, bcm_vlan_port_t *vlan_port, bcm_vlan_t vid, bcm_multicast_t *mcast_group)
{
    int i;
    bcm_mac_t mac[3] = {{0,0,0,0,0,0x08},{0,0,0,0,0,0x09},{0,0,0,0,0,0x10}};
    bcm_l2_addr_t l2addr;
    for (i = 0; i < 3; i++) {
        bcm_l2_addr_t_init(&l2addr, mac[i], vid);
        l2addr.flags = BCM_L2_STATIC ;
        l2addr.port = vlan_port[i].vlan_port_id;
        l2addr.vid = vid;
        BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2addr));

        bcm_mac_t       mmac = {0x1,0x00,0x5e,0x1,0x2,0x3};
        /* Add Multicast address Lookup*/
        bcm_l2_addr_t_init(l2addr, mmac, vid);
        l2addr.l2mc_group = *mcast_group;
        l2addr.flags      = BCM_L2_STATIC;
        BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2addr));
    }
    return BCM_E_NONE;
}

/* create multicast group, and add virtual ports to it */
int
create_multicast_vp(int unit, bcm_multicast_t *mcast_group, bcm_vlan_port_t *vlan_port,
                             bcm_gport_t network_gport)
{
    int i;
    bcm_multicast_t encap_id;

    BCM_IF_ERROR_RETURN(bcm_multicast_create(unit, BCM_MULTICAST_TYPE_VLAN, mcast_group));
    for (i = 0; i < 3; i++) {
        BCM_IF_ERROR_RETURN(bcm_multicast_vlan_encap_get(unit, *mcast_group, vlan_port[i].port,
		                    vlan_port[i].vlan_port_id, &encap_id));
        BCM_IF_ERROR_RETURN(bcm_multicast_egress_add(unit, *mcast_group, vlan_port[i].port, encap_id));
    }

    /* add network port to multicast group as L2 member */
    BCM_IF_ERROR_RETURN(bcm_multicast_l2_encap_get(unit, *mcast_group, network_gport, -1, &encap_id));
    BCM_IF_ERROR_RETURN(bcm_multicast_egress_add(unit, *mcast_group, network_gport, encap_id));

    return BCM_E_NONE;

}

/* configure vlan membership */
int
create_vlan_add_port(int unit, bcm_vlan_t vlan, bcm_port_t *gpon_port, bcm_port_t network_port)
{
    int i;
    bcm_pbmp_t pbmp, ubmp;
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);
    for (i = 0; i < 3; i++) {
        BCM_PBMP_PORT_ADD(pbmp, gpon_port[i]);
    }
    BCM_PBMP_PORT_ADD(pbmp, network_port);

    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vlan));
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vlan, pbmp, ubmp));

    return BCM_E_NONE;

}

/* configure the VLAN to enable multicast replication on vlan ports */
int
enable_gpon_multicast(int unit, bcm_vlan_t vlan, bcm_multicast_t *mcast_group)
{
    bcm_vlan_control_vlan_t vlan_control;
    BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_get(unit, vlan, &vlan_control));
    vlan_control.vp_mc_ctrl = bcmVlanVPMcControlEnable;
    vlan_control.broadcast_group = *mcast_group;
    vlan_control.unknown_multicast_group = *mcast_group;
    vlan_control.unknown_unicast_group = *mcast_group;

    /* in SVL, configure fid_id */
    vlan_control.forwarding_vlan = vlan;
    BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_set(unit, vlan, vlan_control));

    return BCM_E_NONE;

}

bcm_error_t configure_vlan_gpon(int unit)
{
    bcm_error_t rv;
    int gpon_port[] = {gpon_port1, gpon_port2, gpon_port3};
    bcm_gport_t gpon_gport[3];
    bcm_vlan_port_t vlan_port[3];
    bcm_gport_t network_gport;

    bcm_vlan_t gemid[] = {101, 102, 103};
    bcm_vlan_t cvid[]  = {25, 35, 45};
    bcm_vlan_t s_vlan = 500;
    bcm_vlan_t c_vlan = 501;
    bcm_vlan_action_set_t network_ing_action;
    bcm_vlan_action_set_t network_egr_action;
    bcm_multicast_t mcast_group;

    int client_vid = 4000;

    int i;

    /* enable L3 egress mode */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1));

    BCM_GPORT_LOCAL_SET(network_gport, network_port);

    /*Vlan configuration*/
    rv = create_vlan_add_port(unit, client_vid , &gpon_port, network_port);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring vlan : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* create the virtual ports */
    rv = create_gpon_vp(unit, &gemid, &cvid, client_vid, &gpon_port,
                        &vlan_port);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring gpon : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = create_gpon_egr_tag(unit, &gemid, &cvid, client_vid, &vlan_port);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring gpon egress translation : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* create multicast group, and add virtual ports to it */
    rv = create_multicast_vp(unit, &mcast_group, &vlan_port, network_gport);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring multicast group : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = enable_gpon_multicast(unit, client_vid, &mcast_group);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring multicast group : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* enable VLAN translation & configure ingress VLAN translation key for GPON port */
    BCM_IF_ERROR_RETURN(bcm_vlan_control_set(unit, bcmVlanTranslate, 1));

    for (i = 0; i < 3; i++) {
        BCM_GPORT_LOCAL_SET(gpon_gport[i], gpon_port[i]);
    }

    for (i = 0; i < 3; i++) {
        /* device will do 2 lookups in VLAN_XLATE, configure the keys here */
        BCM_IF_ERROR_RETURN(bcm_vlan_control_port_set(unit, gpon_port[i], bcmVlanPortTranslateKeyFirst,
		                    bcmVlanTranslateKeyPortDouble));
        BCM_IF_ERROR_RETURN(bcm_vlan_control_port_set(unit, gpon_port[i], bcmVlanPortTranslateKeySecond,
		                    bcmVlanTranslateKeyPortOuter));
    }

    /* VLAN translation for network port, you need to use the
     * bcm_vlan_translate_action_add() API and
     * bcm_vlan_translate_egress_action_add() API and
     * for instance, translate stacked VLANs S=500/C=501 -> 4000 for network port
     */

    bcm_vlan_action_set_t_init(network_ing_action);
    /* for double tagged packets => replace outer tag & delete inner tag */
    network_ing_action.dt_outer = bcmVlanActionReplace;
    network_ing_action.new_outer_vlan = client_vid;
    network_ing_action.dt_inner = bcmVlanActionDelete;
    BCM_IF_ERROR_RETURN(bcm_vlan_translate_action_add(unit, network_gport, bcmVlanTranslateKeyPortDouble,
	                    s_vlan, c_vlan, &network_ing_action));
    BCM_IF_ERROR_RETURN(bcm_vlan_control_port_set(unit, network_gport, bcmVlanPortTranslateKeyFirst, bcmVlanTranslateKeyPortDouble));
    BCM_IF_ERROR_RETURN(bcm_vlan_control_port_set(unit, network_gport, bcmVlanPortTranslateKeySecond, bcmVlanTranslateKeyPortOuter));

    /*Same translate for gpon_port*/
    for (i = 0; i < 3; i++) {
        BCM_IF_ERROR_RETURN(bcm_vlan_translate_action_add(unit, gpon_gport[i],
		                    bcmVlanTranslateKeyPortDouble, gemid[i], cvid[i], &network_ing_action));
    }

    bcm_vlan_action_set_t_init(network_egr_action);
    /* for single tagged packets (client 4000) => replace outer tag & add inner tag */
    network_egr_action.ot_outer = bcmVlanActionReplace;
    network_egr_action.new_outer_vlan = s_vlan;
    network_egr_action.ot_inner = bcmVlanActionAdd;
    network_egr_action.new_inner_vlan = c_vlan;
    BCM_IF_ERROR_RETURN(bcm_vlan_translate_egress_action_add(unit, network_gport, client_vid, 0, &network_egr_action));

    rv = create_l2_addr(unit, &vlan_port, client_vid, &mcast_group);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring L2 address : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;

}
void verify(int unit)
{
  char   str[512];

  bshell(unit, "hm ieee");
  bshell(unit, "sleep quiet 1");
  printf("Sending known unicast packet on network_port:%d\n", network_port);
  snprintf(str, 512, "tx 1 pbm=%d data=0x0000000000080000000000AA810001F4810001F5000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262761A93725; sleep 1", network_port);
  bshell(unit, str);

  printf("Sending known multicast packet on network_port:%d\n", network_port);
  snprintf(str, 512, "tx 1 pbm=%d data=0x01005E0102030000000000AA810001F4810001F5000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F20212223242526278A90384E; sleep 1", network_port);
  bshell(unit, str);

  printf("Sending un-known multicast packet on network_port:%d\n", network_port);
  snprintf(str, 512, "tx 1 pbm=%d data=0x01005E0000AA0000000000AA810001F4810001F5000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F2021222324252627CCEB551F; sleep 1", network_port);
  bshell(unit, str);

}

/*
 * This functions does the following
 * a)test setup
 * b)actual configuration (Done in configure_vlan_gpon())
 * c)demonstrates the functionality (Done in verify()).
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

  if (BCM_FAILURE((rv = configure_vlan_gpon(unit)))) {
    printf("configure_vlan_gpon() failed.\n");
    return -1;
  }
  printf("Completed configuration(i.e executing configure_vlan_gpon()) successfully.\n");

  verify(unit);
  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}

