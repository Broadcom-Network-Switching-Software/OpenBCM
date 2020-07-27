/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenSDK/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : L3 Multicast
 *
 * Usage    : BCM.0> cint ipmc.c
 *
 * config   : l3_config.bcm
 *
 * Log file : ipmc_log.txt
 *
 * Test Topology :
 *
 *                  +------------------------------+  egress_port1
 *                  |                              +-----------------+
 *                  |                              |
 *                  |                              |
 * ingress_port     |                              |  egress_port2
 * +----------------+          SWITCH              +-----------------+
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
 *     a) Configure a basic IPv4 multicast functional scenario. This adds the
 *        host in l3 table[host] and does the necessary configurations of vlan,
 *        interface and next hop.
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
 *       We can see that smac and vlan are all changed as the packet is routed
 *       through the egress por, but dmac is not changed as its a mcast
 *       replication flow. Also run the 'ipmc table show' to check the HIT
 *       bit status (y i.e Yes) and 'show c' to check the Tx/Rx packet
 *       stats/counters.
 */

/* Reset C interpreter*/
cint_reset();

bcm_port_t ingress_port;
bcm_port_t egress_port1, egress_port2, egress_port3;

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

  for (i = 1; i <= 3; i++) {
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
 *  ---------
 *  01005e010101 000006000300 8100 0005
 *  0800 4500 002e 0000 0000 40ff 91cd 0201
 *  0101 e401 0101 0001 0203 0405 0607 0809
 *  0a0b 0c0d 0e0f 1011 1213 1415 1617 1819
 *  bf2e 44c3 0000 0000
 *
 *  Out Packet:
 *  -----------
 *  01005e010101 000000000001 8100 0002
 *  0800 4500 002e 0000 0000 3fff 92cd 0201
 *  0101 e401 0101 0001 0203 0405 0607 0809
 *  0a0b 0c0d 0e0f 1011 1213 1415 1617 1819
 *  bf2e 44c3 0000 0000
 *
 *
 *  01005e010101 000100000001 8100 0003
 *  0800 4500 002e 0000 0000 3fff 92cd 0201
 *  0101 e401 0101 0001 0203 0405 0607 0809
 *  0a0b 0c0d 0e0f 1011 1213 1415 1617 1819
 *  bf2e 44c3 0000 0000
 *
 *  01005e010101 000102000001 8100 0004
 *  0800 4500 002e 0000 0000 3fff 92cd 0201
 *  0101 e401 0101 0001 0203 0405 0607 0809
 *  0a0b 0c0d 0e0f 1011 1213 1415 1617 1819
 *  bf2e 44c3 0000 0000
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
    snprintf(str, 512, "tx 1 pbm=%d data=0x01005E0101010000060003008100000508004500002E0000000040FF91CD02010101E4010101000102030405060708090A0B0C0D0E0F10111213141516171819BF2E44C3; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "ipmc table show");
    bshell(unit, "show c");

    return;
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
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vlan, pbmp, ubmp));

    return BCM_E_NONE;
}

/* Create egress L3 interface */
int
create_ingress_l3_intf_vlan_set(int unit, bcm_vrf_t vrf, bcm_if_t l3_if1, bcm_vlan_t vid)
{
    bcm_l3_ingress_t ing_intf;
    bcm_if_t l3_intf_id;
    bcm_vlan_control_vlan_t vlan_info;

    bcm_l3_ingress_t_init(&ing_intf);
    ing_intf.flags = BCM_L3_INGRESS_WITH_ID;
    ing_intf.vrf = vrf;
    ing_intf.ipmc_intf_id = l3_if1;
    l3_intf_id = l3_if1;
    BCM_IF_ERROR_RETURN(bcm_l3_ingress_create(unit, &ing_intf, &l3_intf_id));

    bcm_vlan_control_vlan_t_init(&vlan_info);
    BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_get(unit, vid, &vlan_info));
    vlan_info.ingress_if = l3_intf_id;
    BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_set(unit, vid, vlan_info));
    
    return BCM_E_NONE;
}

int
create_l3_intf(int unit, bcm_mac_t mac, bcm_vlan_t vlan, bcm_if_t l3_intf_id)
{
    bcm_l3_intf_t l3_intf;

    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_flags = (BCM_L3_WITH_ID | BCM_L3_ADD_TO_ARL);
    l3_intf.l3a_vid = vlan;
    l3_intf.l3a_intf_id = l3_intf_id;
    sal_memcpy(l3_intf.l3a_mac_addr, mac, sizeof(mac));

    BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &l3_intf));

    return BCM_E_NONE;
}

/* Create L3 egress object */
int
create_egr_obj(int unit, bcm_if_t l3_if, bcm_gport_t gport, bcm_multicast_t mcg)
{
    bcm_if_t encap;

    BCM_IF_ERROR_RETURN(bcm_multicast_l3_encap_get(unit, mcg, gport, l3_if,
                                                   &encap));
    BCM_IF_ERROR_RETURN(bcm_multicast_egress_add(unit, mcg, gport, encap));

    return BCM_E_NONE;
}

/* Steps to configure IPMC */
int
configure_ipmc(int unit, bcm_multicast_t *mcast_grp)
{
    bcm_multicast_t multicast_group;
    bcm_error_t rv;
    int i;
    int num_ports = 3;
    bcm_if_t egr_l3_if[3] ={2, 3, 4};
    bcm_mac_t src_mac = {0x00, 0x00, 0x00, 0x0, 0x0, 0x01};
    bcm_port_t egr_port[3];
    egr_port[0] = egress_port1;
    egr_port[1] = egress_port2;
    egr_port[2] = egress_port3;
    bcm_gport_t egr_gport;
    bcm_vlan_t egr_vlan = 2;

    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3IngressMode, 1));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3IngressInterfaceMapSet, 1));

    BCM_IF_ERROR_RETURN(bcm_multicast_create(unit, BCM_MULTICAST_TYPE_L3,
                                             mcast_grp));
    for(i=0; i<num_ports; i++) {
        rv = create_vlan_add_port(unit, egr_vlan+i, egr_port[i]);
        if (BCM_FAILURE(rv)) {
            printf("Error in configuring vlan : %s.\n", bcm_errmsg(rv));
            return rv;
        }

        /* Create ingress and egress L3 interfaces */
        rv = create_ingress_l3_intf_vlan_set(unit, 0, egr_l3_if[i], egr_vlan+i);
        if (BCM_FAILURE(rv)) {
            printf("Error in configuring ingress l3 interface : %s.\n", bcm_errmsg(rv));
            return rv;
        }

        src_mac[i] = src_mac[i]+i;
        rv = create_l3_intf(unit, src_mac, egr_vlan+i, egr_l3_if[i]);
        if (BCM_FAILURE(rv)) {
            printf("Error in configuring l3 interface : %s.\n", bcm_errmsg(rv));
            return rv;
        }

        BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, egr_port[i], &egr_gport));
        rv = create_egr_obj(unit, egr_l3_if[i], egr_gport, *mcast_grp);
        if (BCM_FAILURE(rv)) {
            printf("Error in configuring l3 egress object : %s.\n", bcm_errmsg(rv));
            return rv;
        }
    }

    return BCM_E_NONE;
}

/* Adding Source IP, Multicast IP and Vlan in L3 entry table */
int
add_ipmc(int unit, bcm_vlan_t vlan, bcm_port_t port, bcm_ip_t sip, bcm_ip_t mc_ip,
         bcm_multicast_t ipmcast_group)
{
    bcm_error_t rv;
    bcm_if_t ing_if = 5;
    bcm_mac_t ing_mac = {0x00, 0x00, 0x06, 0x10, 0x03, 0x05};
    bcm_ipmc_addr_t ipmc_addr;

    rv = create_vlan_add_port(unit, vlan, port);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring vlan : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = create_ingress_l3_intf_vlan_set(unit, 0, ing_if, vlan);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring ingress l3 interface : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = create_l3_intf(unit, ing_mac, vlan, ing_if);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring egr l3 interface : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    bcm_ipmc_addr_t_init(&ipmc_addr);

    ipmc_addr.mc_ip_addr = mc_ip;

    ipmc_addr.group = ipmcast_group;
    /* Can skip this SIP for [*,G,V] multicast configuration  */
    ipmc_addr.s_ip_addr = sip;
    /* Can skip flags corresponding to RPF if RPF configuration is not needed */
    ipmc_addr.flags = ( BCM_IPMC_POST_LOOKUP_RPF_CHECK | BCM_IPMC_RPF_FAIL_TOCPU
		                | BCM_IPMC_SOURCE_PORT_NOCHECK );
    ipmc_addr.vid = vlan;
    /*
     *  This step is essential for RPF which is the expected l3_iif which is
     *  checked against the incoming ipmc_l3_iif
     */
    ipmc_addr.l3a_intf = ing_if;

    BCM_IF_ERROR_RETURN(bcm_ipmc_add(unit, &ipmc_addr));

    return BCM_E_NONE;

}

/* Adding multicast mac in L2 entry */
int
add_multicast_mac(int unit, bcm_mac_t mc_mac, bcm_vlan_t ing_vlan,
                  bcm_port_t ing_port, bcm_multicast_t mc_group)
{
    bcm_error_t rv;
    bcm_l2_addr_t l2_addr;

    bcm_l2_addr_t_init(l2_addr, mc_mac, ing_vlan);
    l2_addr.flags =  BCM_L2_L3LOOKUP;
    BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2_addr));

    return BCM_E_NONE;
}

bcm_error_t
config_l3_ipmc(int unit)
{
    bcm_error_t rv;
    bcm_multicast_t multicast_group;
    bcm_mac_t mac = {0x01, 0x00, 0x5E, 0x01, 0x01, 0x01};
    bcm_vlan_t ing_vlan = 5;
    bcm_port_t ing_port = ingress_port;
    bcm_ip_t sip_addr = 0x02010101;
    bcm_ip_t mc_ip_addr = 0xe4010101;

    rv = configure_ipmc(unit, &multicast_group);
    if(BCM_FAILURE(rv)) {
        printf("\nError in configuring ipmc: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    rv = add_ipmc(unit, ing_vlan, ing_port, sip_addr, mc_ip_addr, multicast_group);
    if(BCM_FAILURE(rv)) {
        printf("\nError in adding ipmc: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    rv = add_multicast_mac(unit, mac, ing_vlan, ing_port, multicast_group);
    if(BCM_FAILURE(rv)) {
        printf("\nError in adding multicast mac: %s.\n",bcm_errmsg(rv));
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
    print "config show; attach; cancun stat; version";
    bshell(unit, "config show; a ; cancun stat; version");
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
