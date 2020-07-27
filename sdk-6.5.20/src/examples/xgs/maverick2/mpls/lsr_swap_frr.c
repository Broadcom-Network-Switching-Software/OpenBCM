/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenSDK/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : L3 MPLS LSR Swap with FRR
 *
 * Usage    : BCM.0> cint lsr_swap_frr.c
 *
 * config   : mpls_config.bcm
 *
 * Log file : lsr_swap_frr_log.txt
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
 *   This Cint example to show configuration of the L3 MPLS LSR swap with
 *   failover scenario using BCM APIs.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects three ports and configure them in Loopback mode. Out of these
 *        three ports, one port is used as ingress_port and the other as
 *        egress_ports (primary & backup).
 *
 *     b) Install an IFP rule to copy incoming packets to CPU and start
 *        packet watcher.
 *
 *     Note: IFP rule is meant for a testing purpose only (Internal) and it is
 *           nothing to do with an actual functional test.
 *
 *   2) Step2 - Configuration (Done in config_lsr_swap_frr()):
 *   =========================================================
 *     a) Configure a basic L3 MPLS Tunnel swap functional scenario with
 *        failover and does the necessary configurations of vlan, interface,
 *        next hop and swap labels.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show', 'l3 intf show' and
 *        'l3 egress show'.
 *
 *     b) Transmit the MPLS Label packet. The contents of the packet
 *        are printed on screen.
 *
 *     c) Expected Result:
 *     ===================
 *       We can see that dmac, smac and vlan are all changed as the packet is
 *       routed through the egress port and also MPLS Labels are swapped
 *       as part of tunnel switching on egress port. Also run the 'show c'
 *       to check the Tx/Rx packet stats/counters.
 *       Now, switch to backup path with *_failover_set or
 *       *_failover_egress_status_set and observe that packet is switched/routed
 *       to backup path/tunnel.
 */

/* Reset C interpreter*/
cint_reset();

bcm_port_t ingress_port;
bcm_port_t egress_primary_port, egress_backup_port;

bcm_if_t primary_egr_obj;
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
 *   MPLS packet - label 0x64
 *      00000000111100000000001181000015
 *      88470006414000010203040506070809
 *      0A0B0C0D0E0F10111213141516171819
 *      1A1B1C1D1E1F20212223242526272829
 *      620F4A16
 *
 *  Out Packet:
 *  -----------
 *  Routed/Switched out on primary path/tunnel - label 0x222:
 *      0000 0000 2222 0000 0000 0022 8100 0016
 *      8847 0006 513f 0001 0203 0405 0607 0809
 *      0a0b 0c0d 0e0f 1011 1213 1415 1617 1819
 *      1a1b 1c1d 1e1f 2021 2223 2425 2627 2829
 *      813f 8a84
 *
 *      Now, switch to backup path with:
 *      bcm_failover_set or
 *      bcm_failover_egress_status_set
 *      packet is routed/switched to backup tunnel with pushi label 0x999 + 0x222
 *
 *      0000 0000 3333 0000 0000 0033 8100 0017
 *      8847 0099 9040 0022 213f 0001 0203 0405
 *      0607 0809 0a0b 0c0d 0e0f 1011 1213 1415
 *      1617 1819 1a1b 1c1d 1e1f 2021 2223 2425
 *      2627 2829 21d4 8674
 */
void
verify(int unit)
{
    char str[512];

    bshell(unit, "hm ieee");
    bshell(unit, "vlan show");
    bshell(unit, "l3 intf show");
    bshell(unit, "l3 egress show");
    bshell(unit, "clear c");
    printf("\nSending MPLS Label packet to ingress_port:%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000111100000000001181000015884700064140000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F20212223242526272829620F4A16; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "show c");

    /* Switch From Primary to Protection */
    print "Switch-over from primary-path to backup/protection-path";
    BCM_IF_ERROR_RETURN(bcm_failover_set(unit, failover_id, 1));

    bshell(unit, "clear c");
    printf("\nRe-Sending MPLS Label packet to ingress_port:%d\n", ingress_port);
   snprintf(str, 512, "tx 1 pbm=%d data=0x00000000111100000000001181000015884700064140000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F20212223242526272829620F4A16; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "show c");

    return;
}

/* Create vlan and add port to vlan */
int
vlan_create_add_port(int unit, int vid, int port)
{
    bcm_pbmp_t pbmp, upbmp;

    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    return bcm_vlan_port_add(unit, vid, pbmp, upbmp);
}

/* Create vlan and add array of ports to vlan */
int
vlan_create_add_ports(int unit, int vid, int count, bcm_port_t *ports)
{
    bcm_pbmp_t pbmp, upbmp;
    int i;

    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);

    for (i = 0; i < count; i++) {
        BCM_PBMP_PORT_ADD(pbmp, ports[i]);
    }

    return bcm_vlan_port_add(unit, vid, pbmp, upbmp);
/*
    bcm_vlan_create(unit, egr_vid);
    print bcm_vlan_gport_add(unit, vid, out_gport_1, 0);
    print bcm_vlan_gport_add(unit, vid, out_gport_2, 0);
    print bcm_vlan_gport_add(unit, vid, out_gport_3, 0);
*/
}

int
create_l3_interface(int unit, int flags, bcm_mac_t local_mac, int vid, bcm_if_t *intf_id)
{
    bcm_l3_intf_t l3_intf;
    bcm_error_t rv = BCM_E_NONE;

    /* L3 interface and egress object for access */
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_flags |= flags;
    sal_memcpy(l3_intf.l3a_mac_addr, local_mac, sizeof(local_mac));
    l3_intf.l3a_vid = vid;
    l3_intf.l3a_intf_id = *intf_id;
    rv = bcm_l3_intf_create(unit, &l3_intf);
    *intf_id = l3_intf.l3a_intf_id;

    return rv;
}

/* Create L3 egress object */
int
create_egr_obj(int unit, int l3_if, bcm_mac_t nh_mac, bcm_gport_t gport,
                   bcm_if_t *egr_obj_id)
{
    bcm_l3_egress_t l3_egress;

    bcm_l3_egress_t_init(&l3_egress);
    sal_memcpy(l3_egress.mac_addr,  nh_mac, sizeof(nh_mac));
    /* l3_egress.vlan = l3_if; vid; */
    l3_egress.intf  = l3_if;
    l3_egress.port = gport;
    return bcm_l3_egress_create(unit, 0, &l3_egress, egr_obj_id);
}

bcm_error_t
config_lsr_swap_frr(int unit)
{
    bcm_error_t rv;
    bcm_vlan_t in_vlan = 21;
    bcm_vlan_t eg_vlan = 22;
    bcm_port_t in_port = ingress_port;
    bcm_port_t eg_port = egress_primary_port;
    bcm_gport_t in_gport, eg_gport;
    bcm_mac_t local_mac_in = {0x00, 0x00, 0x00, 0x00, 0x11, 0x11};
    bcm_mac_t local_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x22};
    bcm_mac_t nh_mac    = {0x00, 0x00, 0x00, 0x00, 0x22, 0x22};
    bcm_if_t  intf_id = 10;
    bcm_l3_intf_t l3_intf;
    bcm_l3_egress_t  l3_egress;
    uint32 in_tunnel_label = 100;
    uint32 out_tunnel_label = 0x222;
    bcm_mpls_tunnel_switch_t tunnel_switch;

    /* Use Egress Object mode */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1));

    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, in_port, &in_gport));
    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, eg_port, &eg_gport));

    /* Ingress side VLAN */
    rv = vlan_create_add_port(unit, in_vlan, in_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuration: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Enable MPLS lookup for MAC $local_mac_in and VLAN $in_vlan */
    BCM_IF_ERROR_RETURN(bcm_l2_tunnel_add(unit, local_mac_in, in_vlan));


    /************************************
     * Create failover path
 */

    /*
     * The packet going out of the backup path will have an additional tunnel label
     * backup_tunnel_label, the old swapped label "out_tunnel_label"
     * now becomes the second/inner label
     * So this is similar to initiation case, e.g. we use
     * bcm_l3_egress_create() and bcm_mpls_tunnel_initiator_set() to add 2 labels
     */
    /* 0 for TH/TH2/TD2/TD3 failover,
     * 1 for TH3 failover_fixed_nh_offset_enable
     */
    int failover_fixed_nh_offset_support = 0;

    bcm_if_t backup_tunnel_intf_id = 20;
    bcm_mac_t backup_sa = {0x00, 0x00, 0x00, 0x00, 0x00, 0x33};
    bcm_mac_t back_da = {0x00, 0x00, 0x00, 0x00, 0x33, 0x33};
    bcm_vlan_t back_vid = 23;
    bcm_port_t backup_port = egress_backup_port;
    bcm_gport_t backup_gport;
    bcm_if_t backup_egr_obj;
    bcm_l3_egress_t backup_l3_egress;
    bcm_mpls_egress_label_t tun_label;
    uint32 backup_tunnel_label = 0x999;

    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, backup_port, &backup_gport));

    /* Egress side VLAN */
    rv = vlan_create_add_port(unit, back_vid, backup_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuration: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Create L3 Interface for Backup LSP */
    rv = create_l3_interface(unit, BCM_L3_WITH_ID, local_mac, back_vid, &backup_tunnel_intf_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuration: %s.\n",bcm_errmsg(rv));
        return rv;
    }
    printf("l3 intf = %d\n", backup_tunnel_intf_id);

    /*  Check failover_fixed_nh_offset_support (TH3),
     *  Create Protection Object */
    int ret = bcm_failover_create(unit, 0, &failover_id);
    if (ret == BCM_E_UNAVAIL) {
        failover_fixed_nh_offset_support = 1;
        printf("TH3 failover fixed nh offset support\n");
    } else {
        printf("bcm_failover_create return=%d\n", ret);
    }

    /* Create Egr Object for Backup LSP */
    bcm_l3_egress_t_init(&backup_l3_egress);
    sal_memcpy(backup_l3_egress.mac_addr, back_da, 6);
    backup_l3_egress.port = backup_gport;
    backup_l3_egress.intf       = backup_tunnel_intf_id;
    backup_l3_egress.mpls_label = out_tunnel_label;
    if (!failover_fixed_nh_offset_support) {
        BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &backup_l3_egress, &backup_egr_obj));
        printf("backup_egr_obj=%d\n", backup_egr_obj);
    }

    /* Push Backup Tunnel Label*/
    bcm_mpls_egress_label_t_init(&tun_label);
    tun_label.flags   =   BCM_MPLS_EGRESS_LABEL_TTL_SET;
    tun_label.label   =   backup_tunnel_label;
    tun_label.ttl     =   64;
    BCM_IF_ERROR_RETURN(bcm_mpls_tunnel_initiator_set(unit, backup_tunnel_intf_id, 1, &tun_label));

    /************************************
     * Create primary Label swap path
 */

    /* Egress side VLAN */
    rv = vlan_create_add_port(unit, eg_vlan, eg_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuration: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Create intf and add next hop */
    rv = create_l3_interface(unit, BCM_L3_WITH_ID | BCM_L3_ADD_TO_ARL,
            local_mac, eg_vlan, &intf_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuration: %s.\n",bcm_errmsg(rv));
        return rv;
    }
    printf("l3 intf = %d\n", intf_id);

    /* Create Egress object with outgoing label */
    bcm_l3_egress_t_init(&l3_egress);
    sal_memcpy(l3_egress.mac_addr, nh_mac, 6);
    l3_egress.intf     = intf_id;
    l3_egress.vlan     =  eg_vlan;
    l3_egress.port     =  eg_gport;
    l3_egress.mpls_label = out_tunnel_label;
    if (!failover_fixed_nh_offset_support) {
        l3_egress.failover_id    = failover_id;    /* failover ID */
        l3_egress.failover_if_id = backup_egr_obj; /* backup path */
        BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &l3_egress, &primary_egr_obj));
        printf("l3 primary_egr_obj = %d\n", primary_egr_obj);
    } else {
        BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &l3_egress, &primary_egr_obj));
        BCM_IF_ERROR_RETURN(bcm_failover_egress_set(unit, primary_egr_obj, &backup_l3_egress));
        printf("l3 primary_egr_obj = %d\n", primary_egr_obj);
    }

    /* Label lookp and PHP operation */
    bcm_mpls_tunnel_switch_t_init (&tunnel_switch);
    tunnel_switch.label   = in_tunnel_label;
    tunnel_switch.port    = BCM_GPORT_INVALID;   /* in_gport; */
    tunnel_switch.action  = BCM_MPLS_SWITCH_ACTION_SWAP;
    tunnel_switch.egress_if = primary_egr_obj;  /* egress object contains the out label info */
    BCM_IF_ERROR_RETURN(bcm_mpls_tunnel_switch_add(unit, &tunnel_switch));

    /************************************
     * Switching between primary & backup paths
 */


    if (!failover_fixed_nh_offset_support) {
        /* Switch From Primary to Protection */
        BCM_IF_ERROR_RETURN(bcm_failover_set(unit, failover_id, 1));
        /* Switch From Protection to Primary */
        BCM_IF_ERROR_RETURN(bcm_failover_set(unit, failover_id, 0));
    } else {
        /* Switch From Primary to Protection */
        BCM_IF_ERROR_RETURN(bcm_failover_egress_status_set(unit, primary_egr_obj, 1));
        /* Switch From Protection to Primary */
        BCM_IF_ERROR_RETURN(bcm_failover_egress_status_set(unit, primary_egr_obj, 0));
    }
    return BCM_E_NONE;
}

/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in config_lsr_swap_frr())
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

    print "~~~ #2) config_lsr_swap_frr(): ** start **";
    if (BCM_FAILURE((rv = config_lsr_swap_frr(unit)))) {
        printf("config_lsr_swap_frr() failed.\n");
        return -1;
    }
    print "~~~ #2) config_lsr_swap_frr(): ** end **";

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

