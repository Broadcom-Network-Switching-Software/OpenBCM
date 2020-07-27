/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenSDK/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : L3 IPv4 route/ LPM entry
 *
 * Usage    : BCM.0> cint ipv4_route_frr.c
 *
 * config   : l3_config.bcm
 *
 * Log file : ipv4_route_frr_log.txt
 *
 * Test Topology :
 *
 *                   +------------------------------+
 *                   |                              |
 *                   |                              | egress_primary_port
 *                   |                              +--------------------+
 *  ingress_port     |                              |
 * +-----------------+          SWITCH              |
 *                   |                              |
 *                   |                              | egress_backup_port
 *                   |                              +--------------------+
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
 *        three ports, one port is used as ingress_port and the other as
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
 *     a) Configure a basic IPv4 route/entry with failover functional scenario.
 *        This adds the route in l3 table[route] and does the necessary
 *        configurations of vlan, interface and next hop.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show', 'l3 intf show',
 *        'l3 egress show' and 'l3 defip show'
 *
 *     b) Transmit the known IPv4 unicast packet. The contents of the packet
 *        are printed on screen. The packet used matches the route configured
 *        through script.
 *
 *     c) Expected Result:
 *     ===================
 *       We can see that dmac, smac and vlan are all changed as the packet is
 *       routed through the egress port. Also run the 'l3 defip show' to
 *       check the HIT bit status (y i.e Yes) and 'show c' to check the Tx/Rx
 *       packet stats/counters.
 */

/* Reset C interpreter*/
cint_reset();

bcm_port_t ingress_port;
bcm_port_t egress_primary_port, egress_backup_port;

bcm_if_t primary_intf;
bcm_failover_t failover_id;

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
bcm_error_t
ingress_port_setup(int unit, bcm_port_t port)
{
    bcm_field_qset_t qset;
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
    bcm_field_qset_t qset;
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

    ingress_port = port_list[0];
    egress_primary_port = port_list[1];
    egress_backup_port = port_list[2];

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

/*
 * Verification:
 *   In Packet:
 *   ----------
 *      00000000AAAA   000000002222   8100 000C
 *      0800 4500 0040 0001 0000 4000 DB65 C0A8
 *      1405 C0A8 0A02 BEBA BEBA BEBA BEBA BEBA
 *      BEBA BEBA BEBA BEBA BEBA BEBA BEBA BEBA
 *      BEBA BEBA BEBA BEBA BEBA BEBA BEBA BEBA
 *      BEBA
 *
 *  Out Packet:
 *  -----------
 *      Routed out on primary path:
 *
 *      000000001111   00000000bbbb   8100 000d
 *      0800 4500 0040 0001 0000 3f00 dc65 c0a8
 *      1405 c0a8 0a02 beba beba beba beba beba
 *      beba beba beba beba beba beba beba beba
 *      beba beba beba beba beba beba beba 3696
 *      7f23
 *
 *      Now, switch to backup path with:
 *      Non-TH3: bcm_failover_set or
 *      TH3: bcm_failover_egress_status_set
 *      packet is routed to backup path:
 *
 *      000000003333   000000000033   8100 0017
 *      0800 4500 0040 0001 0000 3f00 dc65 c0a8
 *      1405 c0a8 0a02 beba beba beba beba beba
 *      beba beba beba beba beba beba beba beba
 *      beba beba beba beba beba beba beba bc2f
 *      dac7
 */
void
verify(int unit)
{
    char str[512];

    bshell(unit, "hm ieee");
    bshell(unit, "vlan show");
    bshell(unit, "l3 intf show");
    bshell(unit, "l3 egress show");
    bshell(unit, "l3 defip show");
    bshell(unit, "dump chg INITIAL_PROT_NHI_TABLE");
    bshell(unit, "clear c");
    printf("\nSending IPv4 unicast packet to ingress_port:%d\n", ingress_port);
   snprintf(str, 512, "tx 1 pbm=%d data=0x00000000AAAA0000000022228100000C080045000040000100004000DB65C0A81405C0A80A02BEBABEBABEBABEBABEBABEBABEBABEBABEBABEBABEBABEBABEBABEBABEBABEBABEBABEBABEBABEBABEBABEBA; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "l3 defip show");
    bshell(unit, "show c");

     /* Switch From Primary to Protection */
    print "Switch-over from primary-path to backup/protection-path";
    BCM_IF_ERROR_RETURN(bcm_failover_set(unit, failover_id, 1));
    bshell(unit, "dump chg INITIAL_PROT_NHI_TABLE");

    bshell(unit, "clear c");
    printf("\nRe-Sending IPv4 unicast packet to ingress_port:%d\n", ingress_port);
   snprintf(str, 512, "tx 1 pbm=%d data=0x00000000AAAA0000000022228100000C080045000040000100004000DB65C0A81405C0A80A02BEBABEBABEBABEBABEBABEBABEBABEBABEBABEBABEBABEBABEBABEBABEBABEBABEBABEBABEBABEBABEBABEBA; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "l3 defip show");
    bshell(unit, "show c");

    return;
}

bcm_error_t
config_l3_route_frr(int unit)
{
    bcm_vlan_t vid_in = 12;
    bcm_vlan_t vid_egr = 13;
    int port_egr = 3;
    bcm_l3_intf_t l3_intf;
    bcm_if_t l3_egr_if_egr;
    bcm_l3_ingress_t l3_ingress;
    bcm_if_t ingress_if;
    bcm_l3_egress_t l3_egress;
    bcm_vlan_control_vlan_t vlan_control;
    bcm_pbmp_t pbmp, upbmp;
    bcm_l3_route_t route_info;
    bcm_l2_addr_t l2addr;

    bcm_mac_t router_mac_egr  = {0x00,0x00,0x00,0x00,0xBB,0xBB};
    bcm_mac_t router_mac_in  = {0x00,0x00,0x00,0x00,0xAA,0xAA};
    bcm_mac_t nxt_hop_egr  = {0x00,0x00,0x00,0x00,0x11,0x11};
    bcm_mac_t nxt_hop_in  = {0x00,0x00,0x00,0x00,0x22,0x22};

    bcm_ip_t dip = 0xC0A80A01;  /* 192.168.10.1 */
    bcm_ip_t mask = 0xffffff00;  /* 255.255.255.0 */

    /* 0 for TH/TH2/TD2/TD3 failover,
     * 1 for TH3 failover_fixed_nh_offset_enable
     */
    int failover_fixed_nh_offset_support = 0;

    /* Use Egress Object mode */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3EgressMode, TRUE));

    /* Allow explicaitly configure ingress intf */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3IngressMode, TRUE));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3IngressInterfaceMapSet, TRUE));

    /*
     * Ingress side of the VLAN
     */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid_in));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port);
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vid_in, pbmp, upbmp));

    /* Adding router mac, enable ingress L3 lookup */
    bcm_l2_addr_t l2_addr;

    bcm_l2_addr_t_init(&l2_addr, router_mac_in, vid_in);
    l2_addr.flags |= (BCM_L2_L3LOOKUP | BCM_L2_STATIC);
    BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2_addr));

    /************************************
     * Create failover path
 */

    bcm_if_t backup_intf_id = 20;
    bcm_mac_t backup_sa = {0x00, 0x00, 0x00, 0x00, 0x00, 0x33};
    bcm_mac_t back_da = {0x00, 0x00, 0x00, 0x00, 0x33, 0x33};
    bcm_vlan_t back_vid = 23;
    bcm_gport_t backup_gport;
    bcm_if_t backup_egr_obj;
    bcm_l3_egress_t backup_l3_egress;

    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, egress_backup_port, &backup_gport));

    /* Egress side VLAN for backup path*/
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, back_vid));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, egress_backup_port);
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, back_vid, pbmp, upbmp));

    /* Create L3 Interface for Backup path */
    bcm_l3_intf_t_init(&l3_intf);
    sal_memcpy(l3_intf.l3a_mac_addr, backup_sa, 6);
    l3_intf.l3a_flags   = BCM_L3_WITH_ID;
    l3_intf.l3a_intf_id = backup_intf_id;
    l3_intf.l3a_vid     = back_vid;
    BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &l3_intf));

    /*  Check failover_fixed_nh_offset_support (TH3),
     *  Create Protection Object */
    int rv = bcm_failover_create(unit, 0, &failover_id);
    if (rv == BCM_E_UNAVAIL) {
        failover_fixed_nh_offset_support = 1;
        printf("TH3 failover fixed nh offset support\n");
    } else {
        printf("bcm_failover_create return=%d\n", rv);
    }


    /* Create Egr Object for Backup LSP */
    bcm_l3_egress_t_init(&backup_l3_egress);
    sal_memcpy(backup_l3_egress.mac_addr, back_da, 6);
    backup_l3_egress.port = backup_gport;
    backup_l3_egress.intf = backup_intf_id;
    if (!failover_fixed_nh_offset_support) {
        BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &backup_l3_egress, &backup_egr_obj));
        printf("backup_egr_obj=%d\n", backup_egr_obj);
    }


    /************************************
     * Create primary path
 */

    /* Egress side of the VLAN */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid_egr));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, egress_primary_port);
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vid_egr, pbmp, upbmp));

    /* Create egress L3 interface */
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_flags |= BCM_L3_ADD_TO_ARL;
    sal_memcpy(l3_intf.l3a_mac_addr, router_mac_egr, sizeof(router_mac_egr));
    l3_intf.l3a_vid = vid_egr;
    l3_intf.l3a_mtu = 1500;
    BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &l3_intf));
    l3_egr_if_egr = l3_intf.l3a_intf_id;

    /*
     * Use the same ID to allocate the ingress interface (L3_IIF)
     * (This is really not needed for the routing to work, we are dealing with
     *  one direction)
     */
    ingress_if = l3_egr_if_egr;
    bcm_l3_ingress_t_init(&l3_ingress);
    l3_ingress.flags = BCM_L3_INGRESS_REPLACE;
    l3_ingress.vrf = 0;
    l3_ingress.ipmc_intf_id  = l3_egr_if_egr;
    BCM_IF_ERROR_RETURN(bcm_l3_ingress_create(unit, &l3_ingress, &ingress_if));

    /*
     * configure VLAN_ID to L3_IIF mapping
     */
    bcm_vlan_control_vlan_t_init(&vlan_control);
    BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_get(unit, vid_egr, &vlan_control));
    vlan_control.ingress_if = ingress_if;
    BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_set(unit, vid_egr, vlan_control));

    /*
     * Egress side of the next hop object
     */
    bcm_l3_egress_t_init(&l3_egress);
    sal_memcpy(l3_egress.mac_addr, nxt_hop_egr, sizeof(nxt_hop_egr));
    l3_egress.vlan = l3_egr_if_egr;
    l3_egress.port = egress_primary_port;
    l3_egress.intf = l3_egr_if_egr;
    if (!failover_fixed_nh_offset_support) {
        l3_egress.failover_id    = failover_id;    /* failover ID */
        l3_egress.failover_if_id = backup_egr_obj; /* backup path */
        BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &l3_egress, &primary_intf));
    } else {
        BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &l3_egress, &primary_intf));
        BCM_IF_ERROR_RETURN(bcm_failover_egress_set(unit, primary_intf, &backup_l3_egress));
    }

    /*
     * Install the route for DIP, note this is routed out on the port_out
     */
    bcm_l3_route_t_init(&route_info);
    route_info.l3a_subnet = dip;
    route_info.l3a_ip_mask = mask;
    route_info.l3a_intf = primary_intf;
    BCM_IF_ERROR_RETURN(bcm_l3_route_add(unit, &route_info));

    if (!failover_fixed_nh_offset_support) {
        /* Switch From Primary to Protection */
        BCM_IF_ERROR_RETURN(bcm_failover_set(unit, failover_id, 1));
        /* Switch From Protection to Primary */
        BCM_IF_ERROR_RETURN(bcm_failover_set(unit, failover_id, 0));
    } else {
        /* Switch From Primary to Protection */
        BCM_IF_ERROR_RETURN(bcm_failover_egress_status_set(unit, primary_intf, 1));
        /* Switch From Protection to Primary */
        BCM_IF_ERROR_RETURN(bcm_failover_egress_status_set(unit, primary_intf, 0));
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

    print "config show; attach; cancun stat; version";
    bshell(unit, "config show; a ; cancun stat; version");

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
