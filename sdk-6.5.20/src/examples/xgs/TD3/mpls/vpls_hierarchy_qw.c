/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * Feature  : H-VPLS with MPLS at access-layer
 *            i.e Spoke & Hub with MPLS backbone
 *
 * Usage    : BCM.0> cint vpls_hierarchy_qw.c
 *
 * config   : mpls_config.bcm
 *
 * Log file : vpls_hierarchy_qw_log.txt
 *
 * Test Topology :
 *
 *                  +------------------------------+
 *                  |                              |
 *                  |                              |
 *                  |                              |
 *     access_port  |                              |  network_port
 * +----------------+          SWITCH              +-----------------+
 *     Spoke        |                              |  Hub
 *                  |                              |
 *                  |                              |
 *                  |                              |
 *                  |                              |
 *                  +------------------------------+
 * Summary:
 * ========
 *   This Cint example to show configuration of H-VPLS tunnel with MPLS/ spoke
 *   and hub with PW scenario using BCM APIs.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects two ports and configure them in Loopback mode. Out of these
 *        two ports, one port is used as access_port/spoke and the other as
 *        network_port/hub.
 *
 *     b) Install an IFP rule to copy incoming packets to CPU and start
 *        packet watcher.
 *
 *     Note: IFP rule is meant for a testing purpose only (Internal) and it is
 *           nothing to do with an actual functional test.
 *
 *   2) Step2 - Configuration (Done in config_vpls_hierarchy_qw()):
 *   ===============================================================
 *     a) Configure a basic H-VPLS Tunnel with MPLS scenario/ spoke PW & Hub
 *        Backbone scenario and does the necessary configurations of vlan,
 *        interface and next hop.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show', 'l3 intf show' and
 *        'l3 egress show'.
 *
 *     b) Transmit the MPLS packet with on access_port/spoke and also on
 *        network_port. The contents of the packet are printed on screen.
 *
 *     c) Expected Result:
 *     ===================
 *       We can see that MPLS/VPLS tunneled packet flows out of hub port and also
 *       in reverse direction from hub to spoke port. The contents of the packet
 *       are printed on screen. Also run the 'show c' to check the Tx/Rx packet
 *       stats/counters.
 */

/* Reset C interpreter*/
cint_reset();

bcm_port_t access_port;
bcm_port_t network_port;

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

    access_port = port_list[0];
    network_port = port_list[1];

    if (BCM_E_NONE != ingress_port_setup(unit, access_port)) {
        printf("ingress_port_setup() failed for port %d\n", access_port);
        return -1;
    }

    if (BCM_E_NONE != egress_port_setup(unit, network_port)) {
        printf("egress_port_setup() failed for port %d\n", network_port);
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
    bshell(unit, "l3 intf show");
    bshell(unit, "l3 egress show");
    bshell(unit, "clear c");

    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, access_port, BCM_PORT_DISCARD_NONE));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, network_port, BCM_PORT_DISCARD_ALL));
    printf("\na) Sending a Spoke/MPLS packet to access_port/spoke port:%d\n", access_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x0000000000880000000000778100000B88470077704000888140000000445566000000AABBCC9100000B8100000C08004500002A0000000040FF79D60000000000000000000102030405060708090A0BACB73B3E; sleep quiet 1", access_port);
    bshell(unit, str);
    bshell(unit, "show c");
    bshell(unit, "sleep 1");
    bshell(unit, "clear c");

    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, access_port, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, network_port, BCM_PORT_DISCARD_NONE));
    printf("\nb) Sending a Hub/MPLS packet to network_port/hub port:%d\n", network_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x0000000000BB0000000000AA8100001688470022204000333140000000AABBCC0000004455668100000108004500002E0000000040FF79D20000000000000000910000038902A0010446000000000000D36D6169643438627974; sleep quiet 1", network_port);
    bshell(unit, str);
    bshell(unit, "show c");
    bshell(unit, "sleep 1");
    bshell(unit, "clear c");

    return;
}

bcm_error_t
config_vpls_hierarchy_qw(int unit)
{
    bcm_port_t spoke_port = access_port;
    bcm_port_t hub_port = network_port;
    bcm_gport_t spoke_gport, hub_gport;
    bcm_vlan_t  spoke_vid=11, hub_vid=22;
    bcm_pbmp_t  pbmp, ubmp;
    bcm_mac_t   hub_nh_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0xaa};
    bcm_mac_t   hub_sa = {0x00, 0x00, 0x00, 0x00, 0x00, 0xbb};
    bcm_mac_t   spoke_nh_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x77};
    bcm_mac_t   spoke_sa = {0x00, 0x00, 0x00, 0x00, 0x00, 0x88};
    bcm_l3_intf_t hub_l3_intf;
    bcm_l3_intf_t spoke_l3_intf;
    bcm_l3_egress_t l3_egress;
    bcm_if_t    hub_egr_obj;
    bcm_if_t    spoke_egr_obj;
    bcm_mpls_egress_label_t tun_label;
    uint8 ttl = 16;
    uint32 hub_tunnel_label_init = 0x111;
    uint32 hub_tunnel_label_term = 0x222;
    uint32 hub_vc_label_term     = 0x333;
    uint32 hub_vc_label_init     = 0x444;
    uint32 spoke_tunnel_label_init = 0x666;
    uint32 spoke_tunnel_label_term = 0x777;
    uint32 spoke_vc_label_term     = 0x888;
    uint32 spoke_vc_label_init     = 0x999;
    bcm_multicast_t bcast_group;
    bcm_mpls_vpn_config_t vpn_info;
    bcm_mpls_port_t mpls_spoke_port, mpls_hub_port;
    bcm_mpls_tunnel_switch_t tunnel_switch;
    bcm_vpn_t vpn;

    /* Enable egress mode */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1));

    /* Enable Ingress VLAN Translation on customer facing ports */
    BCM_IF_ERROR_RETURN(bcm_vlan_control_port_set(unit, spoke_port, bcmVlanTranslateIngressEnable, 1));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, spoke_port, bcmPortControlDoNotCheckVlan, 1));

    /* Disable VLAN checks on provider network facing port */
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, hub_port, bcmPortControlDoNotCheckVlan, 1));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, spoke_port, bcmPortControlDoNotCheckVlan, 1));

    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, spoke_port, &spoke_gport));
    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, hub_port, &hub_gport));

    /* Spoke side VLAN Configuration */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, spoke_vid));
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, spoke_port);
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, spoke_vid, pbmp, ubmp));

    /* Hub side VLAN Configuration */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, hub_vid));
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, hub_port);
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, hub_vid, pbmp, ubmp));

    /* Hub Side */

    /* Create L3 interface for MPLS tunnel */
    bcm_l3_intf_t_init(&hub_l3_intf);
    hub_l3_intf.l3a_flags  = BCM_L3_ADD_TO_ARL;
    sal_memcpy(hub_l3_intf.l3a_mac_addr, hub_sa, 6);
    hub_l3_intf.l3a_vid = hub_vid;
    BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &hub_l3_intf));

    /* Set MPLS tunnel initiator */
    bcm_mpls_egress_label_t_init(&tun_label);
    tun_label.flags   =   BCM_MPLS_EGRESS_LABEL_TTL_SET;
    tun_label.label   =   hub_tunnel_label_init;
    tun_label.exp     =   0;
    tun_label.ttl     =   ttl;
    BCM_IF_ERROR_RETURN(bcm_mpls_tunnel_initiator_set(unit, hub_l3_intf.l3a_intf_id, 1, &tun_label));

    /* Create L3 egress object to MPLS tunnel */
    bcm_l3_egress_t_init(&l3_egress);
    l3_egress.intf     = hub_l3_intf.l3a_intf_id;
    sal_memcpy(l3_egress.mac_addr, hub_nh_mac, 6);
    l3_egress.vlan     =  hub_vid;
    l3_egress.port     =  hub_port;
    BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &l3_egress, &hub_egr_obj));

    /* Spoke Side */

    /* Create L3 interface for MPLS tunnel */
    bcm_l3_intf_t_init(&spoke_l3_intf);
    spoke_l3_intf.l3a_flags  = BCM_L3_ADD_TO_ARL;
    sal_memcpy(spoke_l3_intf.l3a_mac_addr, spoke_sa, 6);
    spoke_l3_intf.l3a_vid = spoke_vid;
    BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &spoke_l3_intf));

    /* Set MPLS tunnel initiator */
    bcm_mpls_egress_label_t_init(&tun_label);
    tun_label.flags   =   BCM_MPLS_EGRESS_LABEL_TTL_SET;
    tun_label.label   =   spoke_tunnel_label_init;
    tun_label.exp     =   0;
    tun_label.ttl     =   ttl;
    BCM_IF_ERROR_RETURN(bcm_mpls_tunnel_initiator_set(unit, spoke_l3_intf.l3a_intf_id, 1, &tun_label));

    /* Create L3 egress object to MPLS tunnel */
    bcm_l3_egress_t_init(&l3_egress);
    l3_egress.intf     = spoke_l3_intf.l3a_intf_id;
    sal_memcpy(l3_egress.mac_addr, spoke_nh_mac, 6);
    l3_egress.vlan     =  spoke_vid;
    l3_egress.port     =  spoke_port;
    BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &l3_egress, &spoke_egr_obj));

    /* Create a broadcast group for VPLS L2 lookup DLF or Broadcast traffic */
    BCM_IF_ERROR_RETURN(bcm_multicast_create(unit, BCM_MULTICAST_TYPE_VPLS, &bcast_group));

    /* Create a VPLS VPN, used by SDK to bind the VPLS customer and provider ports */
    bcm_mpls_vpn_config_t_init(&vpn_info);
    vpn_info.flags = BCM_MPLS_VPN_VPLS;
    vpn_info.broadcast_group = bcast_group;
    vpn_info.unknown_unicast_group = bcast_group;
    vpn_info.unknown_multicast_group = bcast_group;
    BCM_IF_ERROR_RETURN(bcm_mpls_vpn_id_create(unit, &vpn_info));
    vpn = vpn_info.vpn;
    printf ("VPN ID = 0x%x\n", vpn);

    /* Create and bind the customer side Virtual Port to VPLS VPN (repeate for all customer ports) */
    bcm_mpls_port_t_init(&mpls_spoke_port);
    mpls_spoke_port.flags = BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_spoke_port.port  = spoke_gport;
    mpls_spoke_port.criteria    = BCM_MPLS_PORT_MATCH_LABEL;
    mpls_spoke_port.match_label = spoke_vc_label_term;
    mpls_spoke_port.egress_tunnel_if = spoke_egr_obj;
    mpls_spoke_port.egress_label.flags = BCM_MPLS_EGRESS_LABEL_TTL_SET;
    mpls_spoke_port.egress_label.label = spoke_vc_label_init;
    mpls_spoke_port.egress_label.exp = 0;
    mpls_spoke_port.egress_label.ttl = ttl;
    mpls_spoke_port.egress_label.pkt_pri = 0;
    mpls_spoke_port.egress_label.pkt_cfi = 0;
    mpls_spoke_port.match_vlan  = spoke_vid;
    BCM_IF_ERROR_RETURN(bcm_mpls_port_add(unit, vpn, &mpls_spoke_port));

    BCM_IF_ERROR_RETURN(bcm_multicast_vpls_encap_get (unit, bcast_group, spoke_gport,
            mpls_spoke_port.mpls_port_id, &mpls_spoke_port.encap_id));

    /* Add customer port to Bcast group to enable DLF flooding */
    printf("mpls_spoke_port.encap_id=%d\n", mpls_spoke_port.encap_id);
    BCM_IF_ERROR_RETURN(bcm_multicast_egress_add(unit, bcast_group, spoke_gport, mpls_spoke_port.encap_id));

    /* Add MPLS incoming label lookup */
    bcm_mpls_tunnel_switch_t_init(&tunnel_switch);
    tunnel_switch.flags  =  BCM_MPLS_SWITCH_COUNTED;
    tunnel_switch.label  =  spoke_tunnel_label_term;
    tunnel_switch.port   =  BCM_GPORT_INVALID;
    tunnel_switch.action =  BCM_MPLS_SWITCH_ACTION_POP;
    BCM_IF_ERROR_RETURN(bcm_mpls_tunnel_switch_add(unit, &tunnel_switch));

    /* Install L2 tunnel MAC to enable MPLS packet label lookup */
    BCM_IF_ERROR_RETURN(bcm_l2_tunnel_add(unit, spoke_sa, spoke_vid));

    /* Create and bind provider port to VPLS VPN (repeate for all provider ports) */
    bcm_mpls_port_t_init(&mpls_hub_port);
    mpls_hub_port.flags = BCM_MPLS_PORT_NETWORK | BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_hub_port.port  = hub_gport;
    mpls_hub_port.criteria = BCM_MPLS_PORT_MATCH_LABEL;
    mpls_hub_port.match_label = hub_vc_label_term;
    mpls_hub_port.egress_tunnel_if = hub_egr_obj;
    mpls_hub_port.egress_label.flags = BCM_MPLS_EGRESS_LABEL_TTL_SET;
    mpls_hub_port.egress_label.label = hub_vc_label_init;
    mpls_hub_port.egress_label.exp = 0;
    mpls_hub_port.egress_label.ttl = ttl;
    mpls_hub_port.egress_label.pkt_pri = mpls_hub_port.egress_label.pkt_cfi = 0;
    BCM_IF_ERROR_RETURN(bcm_mpls_port_add(unit, vpn, &mpls_hub_port));

    BCM_IF_ERROR_RETURN(bcm_multicast_vpls_encap_get (unit, bcast_group, hub_gport,
            mpls_hub_port.mpls_port_id, &mpls_hub_port.encap_id));

    /* Add provider port to Bcast group to enable DLF flooding */
    printf("mpls_hub_port.encap_id=%d\n", mpls_hub_port.encap_id);
    BCM_IF_ERROR_RETURN(bcm_multicast_egress_add(unit, bcast_group, hub_gport, mpls_hub_port.encap_id));

    /* Add MPLS incoming label lookup */
    bcm_mpls_tunnel_switch_t_init(&tunnel_switch);
    tunnel_switch.flags  =  BCM_MPLS_SWITCH_COUNTED;
    tunnel_switch.label  =  hub_tunnel_label_term;
    tunnel_switch.port   =  BCM_GPORT_INVALID;
    tunnel_switch.action =  BCM_MPLS_SWITCH_ACTION_POP;
    BCM_IF_ERROR_RETURN(bcm_mpls_tunnel_switch_add(unit, &tunnel_switch));

    /* Install L2 tunnel MAC to enable MPLS packet label lookup */
    BCM_IF_ERROR_RETURN(bcm_l2_tunnel_add(unit, hub_sa, hub_vid));

    return BCM_E_NONE;
}


/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in config_vpls_hierarchy_qw())
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

    print "~~~ #2) config_vpls_hierarchy_qw(): ** start **";
    if (BCM_FAILURE((rv = config_vpls_hierarchy_qw(unit)))) {
        printf("config_vpls_hierarchy_qw() failed.\n");
        return -1;
    }
    print "~~~ #2) config_vpls_hierarchy_qw(): ** end **";

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
