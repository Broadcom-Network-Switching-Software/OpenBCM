/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : L3 MPLS LSR label Swa scenario -Method1
 *            (using _mpls_tunnel_switch_add API)
 *
 * Usage    : BCM.0> cint lsr_swap.c
 *
 * config   : mpls_config.bcm
 *
 * Log file : lsr_swap_log.txt
 *
 * Test Topology :
 *
 *                  +------------------------------+
 *                  |                              |
 *                  |                              |
 *                  |                              |
 *     ingress_port |                              |  egress_port
 * +----------------+          SWITCH              +-----------------+
 *                  |                              |
 *                  |                              |
 *                  |                              |
 *                  |                              |
 *                  |                              |
 *                  +------------------------------+
 * Summary:
 * ========
 *   This Cint example to show configuration of L3 MPLS tunnel LSR label Swap
 *   using BCM APIs.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects two ports and configure them in Loopback mode. Out of these
 *        two ports, one port is used as ingress_port and the other as
 *        egress_port.
 *
 *     b) Install an IFP rule to copy incoming packets to CPU and start
 *        packet watcher.
 *
 *     Note: IFP rule is meant for a testing purpose only (Internal) and it is
 *           nothing to do with an actual functional test.
 *
 *   2) Step2 - Configuration (Done in config_lsr_swap()):
 *   =========================================================
 *     a) Configure a basic L3 MPLS Tunnel swap/switching functional scenario and
 *        does the necessary configurations of vlan, interface and next hop.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show', 'l3 intf show' and
 *        'l3 egress show'.
 *
 *     b) Transmit the MPLS with One / Two Label with different TTL packet.
 *        The contents of the packet are printed on screen.
 *
 *     c) Expected Result:
 *     ===================
 *       We can see that dmac, smac and vlan are all changed as the packet is
 *       routed through the egress port and observe that MPLS Label is swapped
 *       for different trails and covered with TTL checks as well. Also run the 'show c'
 *       to check the Tx/Rx packet stats/counters.
 */

/* Reset C interpreter*/
cint_reset();

bcm_port_t ingress_port;
bcm_port_t egress_port;

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
    int port_list[2];

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 2)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    ingress_port = port_list[0];
    egress_port = port_list[1];

    if (BCM_E_NONE != ingress_port_setup(unit, ingress_port)) {
        printf("ingress_port_setup() failed for port %d\n", ingress_port);
        return -1;
    }

    if (BCM_E_NONE != egress_port_setup(unit, egress_port)) {
        printf("egress_port_setup() failed for port %d\n", egress_port);
        return -1;
    }

    bshell(unit, "pw start report +raw +decode");

    return BCM_E_NONE;
}

/*
 * Verification:
 *  In Packet:
 *  ----------
 *  a) TWO labels - label 0x111, 0xAA, TTL-2, TTL-64:
 *  0000000011110000000002028100000B
 *  884700111002000AA140000000000000
 *  00000000000000000000000000000000
 *  0000000000000000000000000000000015DB4FD1
 *
 *  b) ONE label -label 0x111, TTL=2
 *  0000000011110000000002028100000B
 *  88470011110200000000000000000000
 *  00000000000000000000000000000000
 *  000000000000000000000000000000002E719C44
 *
 *  c) ONE Label with TTL =1
 *  0000000011110000000002028100000B
 *  88470011110100000000000000000000
 *  00000000000000000000000000000000
 *  00000000000000000000000000000000236AA88D
 *
 *   c1) setreg CPU_CONTROL_M MPLS_TTL_CHECK_FAIL=1
 *   if set, packet is copied to CPU
 *
 *  Out Packet:
 *  -----------
 *  a) TWO labels- 0x222-0xAA
 *  0000 00aa 2222 0000 00aa 1212 8100 0016
 *  8847 0022 2001 000a a140 0000 0000 0000
 *  0000 0000 0000 0000 0000 0000 0000 0000
 *  0000 0000 0000 0000 0000 0000 0000 0000
 *  b73e 1369
 *
 *  b) ONE label - 0x222
 *  00 00 00 aa 22 22 00 00 00 aa 12 12 81 00 00 16
 *  88 47 00 22 21 01 00 00 00 00 00 00 00 00 00 00
 *  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
 *  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
 *  8c 94 c0 fc
 *
 *  c)
 *   Packet is dropped due to TTL
 *
 *   c1)setreg CPU_CONTROL_M MPLS_TTL_CHECK_FAIL=1
 *   if set, packet is copied to CPU
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
    printf("\na) Sending MPLS packet with TWO labels to ingress_port:%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x0000000011110000000002028100000B884700111002000AA140000000000000000000000000000000000000000000000000000000000000000000000000000015DB4FD1; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "show c");
    bshell(unit, "sleep 1");
    bshell(unit, "clear c");

    printf("\nb) Sending MPLS packet with ONE label with TTL=2 to ingress_port:%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x0000000011110000000002028100000B8847001111020000000000000000000000000000000000000000000000000000000000000000000000000000000000002E719C44; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "show c");
    bshell(unit, "sleep 1");
    bshell(unit, "clear c");

    printf("\nc) Sending MPLS packet with ONE label with TTL=1 to ingress_port:%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x0000000011110000000002028100000B884700111101000000000000000000000000000000000000000000000000000000000000000000000000000000000000236AA88D; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "show c");
    bshell(unit, "sleep 1");
    bshell(unit, "clear c");
    bshell(unit, "get CPU_CONTROL_M");
    bshell(unit, "setreg CPU_CONTROL_M MPLS_TTL_CHECK_FAIL=1");

    printf("\nc1) now, setreg CPU_CONTROL_M MPLS_TTL_CHECK_FAIL=1 & packet is copied to CPU");
    printf("\nc1) Re-sending MPLS packet with ONE label with TTL=1 to ingress_port:%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x0000000011110000000002028100000B884700111101000000000000000000000000000000000000000000000000000000000000000000000000000000000000236AA88D; sleep quiet 1", ingress_port);
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
config_lsr_swap(int unit)
{
    bcm_error_t rv;
    bcm_vlan_t in_vlan = 11;
    bcm_port_t in_port = ingress_port;
    bcm_vlan_t eg_vlan = 22;
    bcm_port_t eg_port = egress_port;
    bcm_gport_t in_gport, eg_gport;
    bcm_mac_t in_l2_mac    = {0x00, 0x00, 0x00, 0x00, 0x11, 0x11};
    bcm_mac_t local_mac = {0x00, 0x00, 0x00, 0xAA, 0x12, 0x12};
    bcm_mac_t nh_mac    = {0x00, 0x00, 0x00, 0xAA, 0x22, 0x22};
    bcm_if_t intf_id;
    bcm_if_t egr_obj;
    uint32 in_tunnel_label = 0x111;  /* 273 */
    uint32 out_tunnel_label = 0x222;  /* 546 */
    uint32 in_pw_label = 0xAA;  /* 170 */
    uint32 out_pw_label = 0xBB;  /* 187 */
    bcm_mpls_tunnel_switch_t tunnel_switch;

    /* Use Egress Object mode */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1));

    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, in_port, &in_gport));
    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, eg_port, &eg_gport));
    printf("in_gport=0x%x, eg_gport=0x%x\n", in_gport, eg_gport);

    /* Ingress side VLAN */
    rv = vlan_create_add_port(unit, in_vlan, in_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuration: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Enable MPLS lookup for MAC $in_l2_mac and VLAN $in_vlan */
    BCM_IF_ERROR_RETURN(bcm_l2_tunnel_add(unit, in_l2_mac, in_vlan));

    /* Egress side VLAN */
    rv = vlan_create_add_port(unit, eg_vlan, eg_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuration: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Create intf and add next hop */
    rv = create_l3_interface(unit, BCM_L3_ADD_TO_ARL, local_mac, eg_vlan, &intf_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuration: %s.\n",bcm_errmsg(rv));
        return rv;
    }
    printf("l3 intf = %d\n", intf_id);

    /* Create Egress object */
    rv = create_egr_obj(unit, intf_id, nh_mac, eg_gport, &egr_obj);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuration: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Label SWAP operation - tunnel label */
    bcm_mpls_tunnel_switch_t_init (&tunnel_switch);
    tunnel_switch.label   = in_tunnel_label;
    tunnel_switch.port    = BCM_GPORT_INVALID;
    tunnel_switch.action  = BCM_MPLS_SWITCH_ACTION_SWAP;
    tunnel_switch.egress_label.label = out_tunnel_label;
    tunnel_switch.egress_if = egr_obj;
    BCM_IF_ERROR_RETURN(bcm_mpls_tunnel_switch_add(unit, &tunnel_switch));
    return BCM_E_NONE;
}

/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in config_lsr_swap())
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

    print "~~~ #2) config_lsr_swap(): ** start **";
    if (BCM_FAILURE((rv = config_lsr_swap(unit)))) {
        printf("config_lsr_swap() failed.\n");
        return -1;
    }
    print "~~~ #2) config_lsr_swap(): ** end **";

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

