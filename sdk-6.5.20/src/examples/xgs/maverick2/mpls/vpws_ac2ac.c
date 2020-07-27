/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenSDK/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : VPWS Tunnel (L2 MPLS) access port to access port scenarios 
 *
 * Usage    : BCM.0> cint vpws_ac2ac.c
 *
 * config   : mpls_config.bcm
 *
 * Log file : vpws_ac2ac_log.txt
 *
 * Test Topology :
 *
 *                  +------------------------------+
 *                  |                              |
 *                  |                              |
 *                  |                              |
 *     access_port1 |                              |  access_port2
 * +----------------+          SWITCH              +-----------------+
 *                  |                              |
 *                  |                              |
 *                  |                              |
 *                  |                              |
 *                  |                              |
 *                  +------------------------------+
 * Summary:
 * ========
 *   This Cint example to show configuration of (L2 MPLS) VPWS tunnel access_port
 *   to access_port scenario flows using BCM APIs.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects two ports and configure them in Loopback mode. Out of these
 *        two ports, one port is used as access_port1 and the other as
 *        access_port2.
 *
 *     b) Install an IFP rule to copy incoming packets to CPU and start
 *        packet watcher.
 *
 *     Note: IFP rule is meant for a testing purpose only (Internal) and it is
 *           nothing to do with an actual functional test.
 *
 *   2) Step2 - Configuration (Done in config_vpws_ac2ac()):
 *   =========================================================
 *     a) Configure a basic VPWS access port to access port flow scenario and
 *        does the necessary configurations of vlan and VP interface.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Transmit the untaged/single tagged L2 packet to access_port (both 
 *        directions) and observe the local L2 switching based on VPWS VPN 
 *
 *     b) Expected Result:
 *     ===================
 *       We can see that Local L2 switching is done using VPWS VPN id between 
 *       access ports and observe that no change in L2 header for both untagged
 *       and tagged packets in any direction i.e from access_port1 to access_port2
 *       and vice-versa. 
 *       Note that, switching is done using VP flow based (no mac learning).
 */

/* Reset C interpreter*/
cint_reset();

bcm_port_t access_port1;
bcm_port_t access_port2;

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

    access_port1 = port_list[0];
    access_port2 = port_list[1];

    if (BCM_E_NONE != ingress_port_setup(unit, access_port1)) {
        printf("ingress_port_setup() failed for port %d\n", access_port1);
        return -1;
    }

    if (BCM_E_NONE != egress_port_setup(unit, access_port2)) {
        printf("egress_port_setup() failed for port %d\n", access_port2);
        return -1;
    }

    bshell(unit, "pw start report +raw +decode");

    return BCM_E_NONE;
}

void
verify(int unit)
{
    char str[512];

    bshell(unit, "hm ieee");
    bshell(unit, "vlan show");
    bshell(unit, "l2 show");
    bshell(unit, "clear c");
    
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, access_port1, BCM_PORT_DISCARD_NONE));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, access_port2, BCM_PORT_DISCARD_ALL));
    printf("\na) Sending a Untagged packet to access_port1:%d\n", access_port1);
    snprintf(str, 512, "tx 1 pbm=%d data=0x0000000011110000000002028100000B884700111002000AA140000000000000000000000000000000000000000000000000000000000000000000000000000015DB4FD1; sleep quiet 1", access_port1);
    bshell(unit, str);
    bshell(unit, "show c");
    bshell(unit, "l2 show");
    bshell(unit, "sleep 1");
    bshell(unit, "clear c");
    printf("\nb) Sending a Single Tagged packet to access_port1:%d\n", access_port1);
    snprintf(str, 512, "tx 1 pbm=%d data=0x1011121314150001020304058100000b002e00000000000000001234568112345682123456831234568412345685123456861234568712345688123456891234a97245a1; sleep quiet 1", access_port1);
    bshell(unit, str);
    bshell(unit, "show c");
    bshell(unit, "l2 show");
    bshell(unit, "sleep 1");
    bshell(unit, "clear c");

    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, access_port2, BCM_PORT_DISCARD_NONE));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, access_port1, BCM_PORT_DISCARD_ALL));
    printf("\nc) Sending a Untagged packet to access_port2:%d\n", access_port2);
    snprintf(str, 512, "tx 1 pbm=%d data=0x0000000011110000000002028100000B884700111002000AA140000000000000000000000000000000000000000000000000000000000000000000000000000015DB4FD1; sleep quiet 1", access_port2);
    bshell(unit, str);
    bshell(unit, "show c");
    bshell(unit, "l2 show");
    bshell(unit, "sleep 1");
    bshell(unit, "clear c");
    printf("\nd) Sending a Single Tagged packet to access_port2:%d\n", access_port2);
    snprintf(str, 512, "tx 1 pbm=%d data=0x1011121314150001020304058100000b002e00000000000000001234568112345682123456831234568412345685123456861234568712345688123456891234a97245a1; sleep quiet 1", access_port2);
    bshell(unit, str);
    bshell(unit, "show c");
    bshell(unit, "l2 show");
    bshell(unit, "sleep 1");
    bshell(unit, "clear c");

    return;
}

bcm_error_t
config_vpws_ac2ac(int unit)
{
    bcm_port_t customer_port = access_port1;
    bcm_port_t customer_port_1 = access_port2;
    bcm_gport_t customer_gport;
    bcm_gport_t customer_gport_1;
    bcm_vlan_t  customer_vid=11;
    bcm_vlan_t  customer_vid_1=11;
    bcm_pbmp_t  pbmp, ubmp;
    bcm_mpls_vpn_config_t vpn_info;
    bcm_mpls_port_t mpls_c_port;
    bcm_mpls_port_t mpls_c_port_1;
    bcm_vpn_t   vpn;

    /* Enable egress mode */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1));

    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, customer_port, &customer_gport));
    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, customer_port_1, &customer_gport_1));
    printf("customer_gport=0x%x\n", customer_gport);
    printf("customer_gport_1=0x%x\n", customer_gport_1);

    /* VLAN Configuration */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, customer_vid));
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, customer_port);
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, customer_vid, pbmp, ubmp));

    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_PORT_ADD(pbmp, customer_port_1);
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, customer_vid_1, pbmp, ubmp));

    /* Create VPWS VPN
    */
    bcm_mpls_vpn_config_t_init(&vpn_info);
    vpn_info.flags = BCM_MPLS_VPN_VPWS;
    BCM_IF_ERROR_RETURN(bcm_mpls_vpn_id_create(unit, &vpn_info));
    vpn = vpn_info.vpn;

    /* Create and bind access port to VPN
    */
    bcm_mpls_port_t_init (&mpls_c_port);
    mpls_c_port.port        = customer_gport;
    mpls_c_port.criteria = BCM_MPLS_PORT_MATCH_PORT;
    BCM_IF_ERROR_RETURN(bcm_mpls_port_add(unit, vpn, &mpls_c_port));

    bcm_mpls_port_t_init (&mpls_c_port_1);
    mpls_c_port_1.port        = customer_gport_1;
    mpls_c_port_1.criteria = BCM_MPLS_PORT_MATCH_PORT;
    BCM_IF_ERROR_RETURN(bcm_mpls_port_add(unit, vpn, &mpls_c_port_1));

    return BCM_E_NONE;
}

/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in config_vpws_ac2ac())
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

    print "~~~ #2) config_vpws_ac2ac(): ** start **";
    if (BCM_FAILURE((rv = config_vpws_ac2ac(unit)))) {
        printf("config_vpws_ac2ac() failed.\n");
        return -1;
    }
    print "~~~ #2) config_vpws_ac2ac(): ** end **";

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
