/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenSDK/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : VPWS with failover
 *
 * Usage    : BCM.0> cint vpws_frr.c
 *
 * config   : mpls_config.bcm
 *
 * Log file : vpws_frr_log.txt
 *
 * Test Topology :
 *
 *                  +------------------------------+
 *                  |                              |
 *                  |                              |
 *                  |                              +-----------------+
 *     access_port  |                              | network_primary_port
 * +----------------+          SWITCH              |
 *                  |                              |
 *                  |                              |
 *                  |                              +-----------------+
 *                  |                              | network_backup_port
 *                  |                              |
 *                  +------------------------------+
 * Summary:
 * ========
 *   This Cint example to show configuration of VPWS with failover/protection
 *   switching mechanism using BCM APIs.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects three ports and configure them in Loopback mode. Out of these
 *        two ports, one port is used as access_port and the other as
 *        network_ports (primary & backup).
 *
 *     b) Install an IFP rule to copy incoming packets to CPU and start
 *        packet watcher.
 *
 *     Note: IFP rule is meant for a testing purpose only (Internal) and it is
 *           nothing to do with an actual functional test.
 *
 *   2) Step2 - Configuration (Done in config_vpws_frr()):
 *   =========================================================
 *     a) Configure a basic VPWS Tunnel initiation/termination flow with failover
 *        functional scenario and does the necessary configurations of vlan,
 *        interface and next hop.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show', 'l3 intf show' and
 *        'l3 egress show'.
 *
 *     b) Transmit the customer packet with single VLAN on access port, then set
 *        the failoverId and resend the same customer packet on access port.
 *        The contents of the packet are printed on screen.
 *
 *     c) Expected Result:
 *     ===================
 *       We can see that VPWS tunnel initiation flows out of network_primary_port
 *       (primary-path) for the first packet and after setting failoverId,
 *       egress traffic switchovers to network_backup_port (backup-path).
 *       The contents of the packet are printed on screen.
 *       Also run the 'show c' to check the Tx/Rx packet stats/counters.
 */

/* Reset C interpreter*/
cint_reset();

bcm_port_t access_port;
bcm_port_t network_primary_port;
bcm_port_t network_backup_port;
bcm_failover_t failoverId = 0;

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
    int port_list[3], i;

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 3)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    access_port = port_list[0];
    network_primary_port = port_list[1];
    network_backup_port = port_list[2];

    if (BCM_E_NONE != ingress_port_setup(unit, access_port)) {
        printf("ingress_port_setup() failed for port %d\n", access_port);
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

void
verify(int unit)
{
    char str[512];

    bshell(unit, "hm ieee");
    bshell(unit, "vlan show");
    bshell(unit, "l3 intf show");
    bshell(unit, "l3 egress show");
    bshell(unit, "clear c");

    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, access_port, BCM_PORT_DISCARD_NONE));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, network_primary_port, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, network_backup_port, BCM_PORT_DISCARD_ALL));
    printf("\na) Sending a customer packet with Single VLAN to access_port:%d\n", access_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x000000000001000000AABBCC8100001508004500002E0000000040FFA920BEA80A0104040404000102030405060708090A0B0C0D0E0F10111213141516171819ED54ED54; sleep quiet 1", access_port);
    bshell(unit, str);
    bshell(unit, "show c");
    bshell(unit, "sleep 1");
    bshell(unit, "clear c");
    print "Switch-over from primary-path to backup/protection-path";
    /* Set Failover, traffic going through backup path */
    BCM_IF_ERROR_RETURN(bcm_failover_set(unit, failoverId, 1));
    printf("\nb) Re-Sending a customer packet with Single VLAN to access_port:%d\n", access_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x000000000001000000AABBCC8100001508004500002E0000000040FFA920BEA80A0104040404000102030405060708090A0B0C0D0E0F10111213141516171819ED54ED54; sleep quiet 1", access_port);
    bshell(unit, str);
    bshell(unit, "show c");
    bshell(unit, "sleep 1");
    bshell(unit, "clear c");

    return;
}

bcm_error_t
config_vpws_frr(int unit)
{
    bcm_port_t c_port = access_port;
    bcm_port_t p_port_backup = network_backup_port;
    bcm_port_t p_port_primary = network_primary_port;
    bcm_gport_t c_gport, p_gport_backup, p_gport_primary;
    bcm_vlan_t c_vid = 21;
    bcm_vlan_t p_vid_backup = 22;
    bcm_vlan_t p_vid_primary = 23;

    bcm_mpls_vpn_config_t vpn_info;

    int  intf_id_backup = 1;
    int  intf_id_primary = 2;
    int  ttl = 16;
    uint32 tunnel_label_init_backup = 0x111;  /* 273 */
    uint32 tunnel_label_init_primary = 0x555;  /* 1365 */
    uint32 vc_label_term_backup     = 0x333;  /* 819 */
    uint32 vc_label_term_primary     = 0x777;  /* 1911 */
    uint32 vc_label_init_backup     = 0x444;  /* 1092 */
    uint32 vc_label_init_primary     = 0x666;  /* 1638 */

    bcm_l3_egress_t          l3_egress_backup;
    bcm_if_t                 l3_egr_obj_backup;
    bcm_l3_egress_t          l3_egress_primary;
    bcm_if_t                 l3_egr_obj_primary;
    bcm_l3_intf_t            l3_intf_backup;
    bcm_l3_intf_t            l3_intf_primary;
    bcm_mpls_egress_label_t  mpls_egress_label[2];
    bcm_mac_t remote_mac_backup = {00, 00, 00, 00, 00, 01};
    bcm_mac_t local_mac_backup  = {00, 00, 00, 00, 11, 11};
    bcm_mac_t remote_mac_primary = {00, 00, 00, 00, 00, 02};
    bcm_mac_t local_mac_primary  = {00, 00, 00, 00, 22, 22};

    bcm_mpls_port_t          cust_mpls_port;       /* customer port */
    bcm_mpls_port_t          service_mpls_port_backup;  /* provider port */
    bcm_mpls_port_t          service_mpls_port_primary;  /* provider port */


    /* Init Failover */
    bcm_failover_init(unit);

    /*
     * Initialize gport values
     */
    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, c_port, &c_gport));
    printf("c_gport=0x%x\n", c_gport);
    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, p_port_backup, &p_gport_backup));
    printf("c_gport=0x%x\n", p_gport_backup);
    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, p_port_primary, &p_gport_primary));
    printf("c_gport=0x%x\n", p_gport_primary);

    /* Create VLANs */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, c_vid));
    BCM_IF_ERROR_RETURN(bcm_vlan_gport_add(unit, c_vid, c_gport, 0));

    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, p_vid_backup));
    BCM_IF_ERROR_RETURN(bcm_vlan_gport_add(unit, p_vid_backup, p_gport_backup, 0));

    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, p_vid_primary));
    BCM_IF_ERROR_RETURN(bcm_vlan_gport_add(unit, p_vid_primary, p_gport_primary, 0));

    /*
     * Enable L3 egress mode & VLAN translation
     */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1));
    BCM_IF_ERROR_RETURN(bcm_vlan_control_set(unit, bcmVlanTranslate, 1));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL2StaticMoveToCpu, 1));

    /*
     * Create tunnel A - failover
     */

    /* Create L3 interface for MPLS tunnel A */
    bcm_l3_intf_t_init(&l3_intf_backup);
    l3_intf_backup.l3a_flags |= BCM_L3_WITH_ID | BCM_L3_ADD_TO_ARL;
    l3_intf_backup.l3a_intf_id = intf_id_backup;
    sal_memcpy(l3_intf_backup.l3a_mac_addr, local_mac_backup, 6);
    l3_intf_backup.l3a_vid = p_vid_backup;
    BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &l3_intf_backup));

    /* Set MPLS tunnel initiator A */
    bcm_mpls_egress_label_t_init(&mpls_egress_label[0]);
    bcm_mpls_egress_label_t_init(&mpls_egress_label[1]);
    mpls_egress_label[0].flags = BCM_MPLS_EGRESS_LABEL_TTL_SET;
    mpls_egress_label[0].label = tunnel_label_init_backup;
    mpls_egress_label[0].ttl = ttl;
    BCM_IF_ERROR_RETURN(bcm_mpls_tunnel_initiator_set(unit, l3_intf_backup.l3a_intf_id, 1, mpls_egress_label));

    /* Create L3 egress object for MPLS tunnel A */
    bcm_l3_egress_t_init(&l3_egress_backup);
    l3_egress_backup.intf = intf_id_backup;
    sal_memcpy(l3_egress_backup.mac_addr, remote_mac_backup, 6);
    l3_egress_backup.vlan = p_vid_backup;
    l3_egress_backup.port = p_gport_backup;
    BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &l3_egress_backup, &l3_egr_obj_backup));

    /* Create Failover object */
    BCM_IF_ERROR_RETURN(bcm_failover_create(unit, 0, &failoverId));
    print("Failover group %d\n", failoverId);

    /*
     * Create tunnel B - primary
     */

    /* Create L3 interface for MPLS tunnel B */
    bcm_l3_intf_t_init(&l3_intf_primary);
    l3_intf_primary.l3a_flags = BCM_L3_WITH_ID | BCM_L3_ADD_TO_ARL;
    l3_intf_primary.l3a_intf_id = intf_id_primary;
    sal_memcpy(l3_intf_primary.l3a_mac_addr, local_mac_primary, 6);
    l3_intf_primary.l3a_vid = p_vid_primary;
    BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &l3_intf_primary));

    /* Set MPLS tunnel initiator B */
    bcm_mpls_egress_label_t_init(&mpls_egress_label[0]);
    bcm_mpls_egress_label_t_init(&mpls_egress_label[1]);
    mpls_egress_label[0].flags = BCM_MPLS_EGRESS_LABEL_TTL_SET;
    mpls_egress_label[0].label = tunnel_label_init_primary;
    mpls_egress_label[0].ttl = ttl;
    BCM_IF_ERROR_RETURN(bcm_mpls_tunnel_initiator_set(unit, intf_id_primary, 1, mpls_egress_label));

    /* Create L3 egress object for MPLS tunnel B */
    bcm_l3_egress_t_init(&l3_egress_primary);
    l3_egress_primary.intf = intf_id_primary;
    l3_egress_primary.port = p_gport_primary;
    l3_egress_primary.vlan = p_vid_primary;    /* VLAN field not used,
                                                  but API requires it to be a valid VLAN */
    sal_memcpy(l3_egress_primary.mac_addr, remote_mac_primary, 6);
    BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &l3_egress_primary, &l3_egr_obj_primary));

    /*
     * Create VPWS VPN
     */
    bcm_mpls_vpn_config_t_init(&vpn_info);
    vpn_info.flags = BCM_MPLS_VPN_VPWS;
    BCM_IF_ERROR_RETURN(bcm_mpls_vpn_id_create(unit, &vpn_info));
    printf("vpn_id=%d\n", vpn_info.vpn);

    /*
     * Add customer facing port to VPN
     */
    bcm_mpls_port_t_init(&cust_mpls_port);
    cust_mpls_port.port = c_gport;
    cust_mpls_port.criteria = BCM_MPLS_PORT_MATCH_PORT_VLAN;
    cust_mpls_port.match_vlan = c_vid;
    BCM_IF_ERROR_RETURN(bcm_mpls_port_add(unit, vpn_info.vpn, &cust_mpls_port));
    printf("MPLS port 1 = 0x%x\n", cust_mpls_port.mpls_port_id);

    /*
     * Add failover provider port (_backup) to VPN
     */
    bcm_mpls_port_t_init(&service_mpls_port_backup);
    service_mpls_port_backup.flags = BCM_MPLS_PORT_EGRESS_TUNNEL |
        BCM_MPLS_PORT_NETWORK |
        BCM_MPLS_PORT_COUNTED;
    service_mpls_port_backup.port = p_gport_backup;
    service_mpls_port_backup.criteria = BCM_MPLS_PORT_MATCH_LABEL;
    service_mpls_port_backup.match_label = vc_label_term_backup;
    service_mpls_port_backup.egress_tunnel_if = l3_egr_obj_backup;
    service_mpls_port_backup.egress_label.flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
    service_mpls_port_backup.egress_label.label = vc_label_init_backup;
    service_mpls_port_backup.egress_label.ttl = ttl;
    BCM_IF_ERROR_RETURN(bcm_mpls_port_add (unit, vpn_info.vpn, &service_mpls_port_backup));
    printf("MPLS port 2 = 0x%x\n", service_mpls_port_backup.mpls_port_id);

    /*
     * Add primary service provider facing MPLS port (_primary) to VPN
     */
    bcm_mpls_port_t_init(&service_mpls_port_primary);
    service_mpls_port_primary.flags = BCM_MPLS_PORT_EGRESS_TUNNEL |
        BCM_MPLS_PORT_NETWORK |
        BCM_MPLS_PORT_COUNTED;
    service_mpls_port_primary.port = p_gport_primary;
    service_mpls_port_primary.criteria = BCM_MPLS_PORT_MATCH_LABEL;
    service_mpls_port_primary.match_label = vc_label_term_backup;
    service_mpls_port_primary.egress_tunnel_if = l3_egr_obj_primary;
    /* Failover group and port ID */
    service_mpls_port_primary.failover_id = failoverId;
    service_mpls_port_primary.failover_port_id = service_mpls_port_backup.mpls_port_id;
    service_mpls_port_primary.egress_label.flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
    service_mpls_port_primary.egress_label.label = vc_label_init_primary;
    service_mpls_port_primary.egress_label.ttl = ttl;
    BCM_IF_ERROR_RETURN(bcm_mpls_port_add (unit, vpn_info.vpn, &service_mpls_port_primary));
    printf("MPLS port 3 = 0x%x\n", service_mpls_port_primary.mpls_port_id);

    /* ======================================================================= */
    /* Before setting failover, PW is using tunnel _primary */
    return BCM_E_NONE;
}


/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in config_vpws_frr())
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

    print "~~~ #2) config_vpws_frr(): ** start **";
    if (BCM_FAILURE((rv = config_vpws_frr(unit)))) {
        printf("config_vpws_frr() failed.\n");
        return -1;
    }
    print "~~~ #2) config_vpws_frr(): ** end **";

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
