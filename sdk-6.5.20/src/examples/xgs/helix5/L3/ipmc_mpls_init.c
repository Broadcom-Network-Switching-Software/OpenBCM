/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenSDK/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : L3 Multicast with MPLS Tunnel Initiation
 *
 * Usage    : BCM.0> cint ipmc_mpls_init.c
 *
 * config   : l3_config.bcm
 *
 * Log file : ipmc_mpls_init__log.txt
 *
 * Test Topology :
 *
 *                  +------------------------------+
 *                  |                              | egress_port1
 *                  |                              +-----------------+
 *                  |                              |
 * ingress_port     |                              |
 * +----------------+          SWITCH              |
 *                  |                              |
 *                  |                              |
 *                  |                              | egress_port2
 *                  |                              +-----------------+
 *                  |                              |
 *                  +------------------------------+
 *
 * Summary:
 * ========
 *   This Cint example to show configuration of the IPv4 multicast scenario
 *   with MPLS Tunnel Initiation using BCM APIs. Replication of IPMC packet
 *   to a set of MPLS tunnels for the given (S, G)/(*, G)
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Select one ingress and two egress ports and configure them in
 *        Loopback mode.
 *
 *     b) Install an IFP rule to copy incoming packets to CPU and start
 *        packet watcher.
 *
 *     Note: IFP rule is meant for a testing purpose only (Internal) and it is
 *           nothing to do with an actual functional test.
 *
 *   2) Step2 - Configuration (Done in config_ipmc_mpls_init()):
 *   ===========================================================
 *     a) Configure a basic IPv4 multicast functional scenario. This adds the
 *        host in l3 table[host] and does the necessary configurations of vlan,
 *        interface and next hop.
 *     b) Configure MPLS Tunnel Initiation and associate with multicast egress
 *        object.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show', 'l3 intf show',
 *        'l3 egress show','multicast show', 'l2 show'  and 'ipmc table show'
 *
 *     b) Transmit the known IPv4 multicast packet. The contents of the packet
 *        are printed on screen. The packet used matches the route configured
 *        through script.
 *
 *     c) Expected Result:
 *     ===================
 *       We can see that dmac, smac and vlan are all changed as the packet is
 *       routed through the egress port. Also run the 'ipmc table show' to
 *       check the HIT bit status (y i.e Yes) and 'show c' to check the Tx/Rx
 *       packet stats/counters.
 *       Packet replicate to egress_port1 & egress_port2 in below format
 *       L2 + MPLS + L3 MC + Payload
 */

/* Reset C interpreter*/
cint_reset();

bcm_port_t ingress_port;
bcm_port_t egress_port1, egress_port2;

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
bcm_error_t
ingress_port_setup(int unit, bcm_port_t port)
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
bcm_error_t
egress_port_setup(int unit, bcm_port_t port)
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
 * Select one ingress and two egress ports and configure them in
 * Loopback mode. Install a rule to copy incoming packets to CPU and
 * additional action to drop the packets when it loops back on egress
 * ports. Start packet watcher. Ingress port setup is done in
 * ingress_port_setup(). egress port setup is done in egress_port_setup().
 */
bcm_error_t
test_setup(int unit)
{
  int port_list[3], i;

  if (BCM_E_NONE != portNumbersGet(unit, port_list, 3)) {
    printf("portNumbersGet() failed\n");
    return -1;
  }

  ingress_port = port_list[0];
  egress_port1 = port_list[1];
  egress_port2 = port_list[2];

  if (BCM_E_NONE != ingress_port_setup(unit, ingress_port)) {
    printf("ingress_port_setup() failed for port %d\n", ingress_port);
    return -1;
  }

  for (i = 1; i <= 2; i++) {
    if (BCM_E_NONE != egress_port_setup(unit, port_list[i])) {
      printf("egress_port_setup() failed for port %d\n", port_list[i]);
      return -1;
    }
  }

  bshell(unit, "pw start report +raw +decode");
  return BCM_E_NONE;
}

/*  Verification:
 *
 *  In Packet:
 *  ----------
 *   01005e010101 000000000011 8100 000c
 *   0800 4500 002e 0000 0000 40ff c324 c0a8
 *   1402 e101 0101 0001 0203 0405 0607 0809
 *   0a0b 0c0d 0e0f 1011 1213 1415 1617 1819
 *   f620 fe33 0000 0000
 *
 *  Out Packet:
 *  ----------
 *   000000001313 00000000bbbb 8100 000d
 *   8847 0006 5040 000c 913f 4500 002e 0000
 *   0000 3fff c424 c0a8 1402 e101 0101 0001
 *   0203 0405 0607 0809 0a0b 0c0d 0e0f 1011
 *   1213 1415 1617 1819 f620 fe33 0000 0000
 *
 *   000000001414 00000000bbbb 8100 000e
 *   8847 0012 d040 0019 113f 4500 002e 0000
 *   0000 3fff c424 c0a8 1402 e101 0101 0001
 *   0203 0405 0607 0809 0a0b 0c0d 0e0f 1011
 *   1213 1415 1617 1819 f620 fe33 0000 0000
 */
void
verify(int unit)
{
    char str[512];

    bshell(unit, "hm ieee");
    bshell(unit, "vlan show");
    bshell(unit, "l3 intf show");
    bshell(unit, "l3 egress show");
    bshell(unit, "multicast show");
    bshell(unit, "ipmc table show");
    bshell(unit, "clear c");
    printf("\nSending IPv4 multicast packet to ingress_port:%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x01005E0101010000000000118100000C08004500002E0000000040FFC324C0A81402E1010101000102030405060708090A0B0C0D0E0F10111213141516171819F620FE33; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "ipmc table show");
    bshell(unit, "show c");

    return;
}

int
vlan_create_add_port(int unit, int vid, int port)
{
    bcm_pbmp_t         pbmp, upbmp;

    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vid, pbmp, upbmp));

    return BCM_E_NONE;
}

int
create_l3_interface(int unit, bcm_mac_t local_mac, int vid, bcm_if_t *intf_id)
{
    bcm_l3_intf_t      l3_intf;

    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_flags |= BCM_L3_ADD_TO_ARL;
    sal_memcpy(l3_intf.l3a_mac_addr, local_mac, sizeof(local_mac));
    l3_intf.l3a_vid = vid;
    BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &l3_intf));
    *intf_id = l3_intf.l3a_intf_id;

    return BCM_E_NONE;
}

/* Create L3 egress object */
int
create_l3_egr_obj(int unit, int flags, int l3_if, bcm_mac_t nh_mac,
               bcm_gport_t gport, bcm_vlan_t p_vlan,
               bcm_mpls_label_t vc_label_1, bcm_if_t *egr_obj_id)
{
    bcm_l3_egress_t    l3_egress;

    bcm_l3_egress_t_init(&l3_egress);
    l3_egress.flags = flags;
    sal_memcpy(l3_egress.mac_addr, nh_mac, sizeof(nh_mac));
    l3_egress.intf  = l3_if;
    l3_egress.port = gport;
    l3_egress.vlan = p_vlan;
    l3_egress.mpls_label = vc_label_1;
    BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, BCM_L3_ROUTE_LABEL,
                                             &l3_egress, egr_obj_id));

    return BCM_E_NONE;
}

/* Add IPMC entry */
int
add_ipmc(int unit, bcm_vlan_t vid, bcm_gport_t gport,
         bcm_ip_t mc_ip, bcm_multicast_t ipmcast_group)
{
    bcm_ipmc_addr_t        ipmc_addr;

    bcm_ipmc_addr_t_init(&ipmc_addr);
    ipmc_addr.flags = BCM_IPMC_SOURCE_PORT_NOCHECK;
    ipmc_addr.group = ipmcast_group;
    ipmc_addr.mc_ip_addr = mc_ip;
    ipmc_addr.vid = vid;
    ipmc_addr.port_tgid = gport;
    BCM_IF_ERROR_RETURN(bcm_ipmc_add(unit, &ipmc_addr));

    return BCM_E_NONE;
}

/* Create multicast egress object */
int
create_mc_egr_obj(int unit, bcm_if_t l3_egr_if, bcm_gport_t gport,
                  bcm_multicast_t mcg)
{
    bcm_if_t           encap_id;

    BCM_IF_ERROR_RETURN(bcm_multicast_egress_object_encap_get(unit, mcg,
                                                              l3_egr_if,
                                                              &encap_id));
    BCM_IF_ERROR_RETURN(bcm_multicast_egress_add(unit, mcg, gport, encap_id));

    return BCM_E_NONE;
}

int
mpls_tunnel_initiator(int unit, bcm_if_t l3_egr_if,
                          bcm_mpls_label_t tun_label, uint8 ttl)
{
    bcm_mpls_egress_label_t     tunnel_label;

    bcm_mpls_egress_label_t_init(&tunnel_label);
    tunnel_label.flags = BCM_MPLS_EGRESS_LABEL_TTL_SET;
    tunnel_label.label = tun_label;
    tunnel_label.ttl = ttl;
    BCM_IF_ERROR_RETURN(bcm_mpls_tunnel_initiator_set(unit, l3_egr_if, 1,
                                                      &tunnel_label));
    return BCM_E_NONE;
}


bcm_error_t
config_ipmc_mpls_init(int unit)
{
    bcm_error_t rv;
    bcm_mpls_label_t tun_label_1 = 101;
    bcm_mpls_label_t vc_label_1 = 201;
    bcm_mpls_label_t tun_label_2 = 301;
    bcm_mpls_label_t vc_label_2 = 401;
    bcm_multicast_t ipmc_group;
    bcm_ipmc_addr_t ipmc;
    bcm_gport_t gport_in;
    bcm_gport_t gport_tunnel_1;
    bcm_gport_t gport_tunnel_2;
    bcm_port_t port_in = ingress_port;
    bcm_port_t port_tunnel_1 = egress_port1;
    bcm_port_t port_tunnel_2 = egress_port2;
    bcm_pbmp_t pbmp, upbmp;
    bcm_vlan_t vid_in = 12;
    bcm_vlan_t tunnel_vid_1 = 13;
    bcm_vlan_t tunnel_vid_2 = 14;
    bcm_mac_t tunnel_mac  = {0x00,0x00,0x00,0x00,0xBB,0xBB};
    bcm_mac_t tunnel_nh_mac_1  = {0x00,0x00,0x00,0x00,0x13,0x13};
    bcm_mac_t tunnel_nh_mac_2  = {0x00,0x00,0x00,0x00,0x14,0x14};
    bcm_ip_t mc_ip_addr=0xe1010101;
    bcm_if_t l3_egr_if_1;
    bcm_if_t l3_egr_if_2;
    bcm_if_t tunnel_obj_egr_1;
    bcm_if_t tunnel_obj_egr_2;
    uint8 ttl=64;

    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3EgressMode, TRUE));

    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, port_in, &gport_in));
    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, port_tunnel_1, &gport_tunnel_1));
    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, port_tunnel_2, &gport_tunnel_2));

    /* Create VLAN & add member ports */
    rv = vlan_create_add_port(unit, vid_in, port_in);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring : %s.\n", bcm_errmsg(rv));
        return rv;
    }
    rv =  vlan_create_add_port(unit, tunnel_vid_1, port_tunnel_1);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring : %s.\n", bcm_errmsg(rv));
        return rv;
    }
    rv =  vlan_create_add_port(unit, tunnel_vid_2, port_tunnel_2);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Tunnel L3 interface */
    rv =  create_l3_interface(unit, tunnel_mac, tunnel_vid_1, &l3_egr_if_1);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring : %s.\n", bcm_errmsg(rv));
        return rv;
    }
    rv =  create_l3_interface(unit, tunnel_mac, tunnel_vid_2, &l3_egr_if_2);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Tunnel initiator */
    rv =  mpls_tunnel_initiator(unit, l3_egr_if_1, tun_label_1, ttl);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring : %s.\n", bcm_errmsg(rv));
        return rv;
    }
    rv =  mpls_tunnel_initiator(unit, l3_egr_if_2, tun_label_2, ttl);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Tunnel egress object */
    rv =  create_l3_egr_obj(unit, 0, l3_egr_if_1, tunnel_nh_mac_1, gport_tunnel_1,
            tunnel_vid_1, vc_label_1, &tunnel_obj_egr_1);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring : %s.\n", bcm_errmsg(rv));
        return rv;
    }
    rv =  create_l3_egr_obj(unit, 0, l3_egr_if_2, tunnel_nh_mac_2, gport_tunnel_2,
            tunnel_vid_2, vc_label_2, &tunnel_obj_egr_2);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create L3 multicast group */
    BCM_IF_ERROR_RETURN(bcm_multicast_create(unit, BCM_MULTICAST_TYPE_L3, &ipmc_group));

    /* Set up IPMC  */
    rv =  add_ipmc(unit, vid_in, gport_in, mc_ip_addr, ipmc_group);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* L3 encap */
    rv =  create_mc_egr_obj(unit, tunnel_obj_egr_1, gport_tunnel_1, ipmc_group);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring : %s.\n", bcm_errmsg(rv));
        return rv;
    }
    rv =  create_mc_egr_obj(unit, tunnel_obj_egr_2, gport_tunnel_2, ipmc_group);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring : %s.\n", bcm_errmsg(rv));
        return rv;
    }
    return BCM_E_NONE;
}

/*
 * execute:
 *  This functions does the following
 *  a)test setup
 *  b)actual configuration (Done in config_ipmc_mpls_init())
 *  c)demonstrates the functionality(done in verify()).
 */
bcm_error_t
execute(void)
{
    bcm_error_t rv;
    int unit = 0;
    print "config show; attach; cancun stat; version";
    bshell(unit, "config show; a ; cancun stat; version");
    print "~~~ #1) test_setup(): ** start **";
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return -1;
    }
    print "~~~ #1) test_setup(): ** end **";

    print "~~~ #2) config_ipmc_mpls_init(): ** start **";
    if (BCM_FAILURE((rv = config_ipmc_mpls_init(unit)))) {
        printf("config_ipmc_mpls_init() failed.\n");
        return -1;
    }
    print "~~~ #2) config_ipmc_mpls_init(): ** end **";

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
