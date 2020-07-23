/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * Feature  : VPWS (L2 MPLS) tunnel initiation flow with
 *            control word sequence number (BCM_MPLS_PORT_SEQUENCED)
 *
 * Usage    : BCM.0> cint td4_hsdk_mpls_vpws_AtoN_ControlWordSequence.c
 *
 * config   : bcm56880_a0-generic-32x400.mpls-config.yml
 *
 * Log file : td4_hsdk_mpls_vpws_AtoN_ControlWordSequence_log.txt
 *
 * Test Topology :
 *                      +-------------------+
 *                      |                   |
 *                      |                   |
 *      customer_port   |                   |  provider_port
 *           ---------->+       TD4         +---------->
 *     Ingress packet   |                   |  Egress mpls packet
 *                      |                   |
 *                      |                   |
 *                      |                   |
 *                      +-------------------+
 *
 *
 * Summary:
 * ========
 * Cint script to demonstrate VPWS (L2 MPLS) tunnel initiation flow with
 * control word sequence number present.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects two ports and configure them in Loopback mode. Out of these
 *        two ports, one port is used as ingress_port and the other as
 *        egress_port.
 *
 *     b) Install an IFP rule to copy ingress & egress packets to CPU and start
 *        packet watcher.
 *
 *     Note: IFP rule is meant for a verification purpose and is not part of
 *           the feature configuration.
 *
 *   2) Step2 - Configuration (Done in setup_vpws()):
 *   =========================================================
 *     a) Ports are configured in the specific vlan/vfi
 *
 *     b) Configure a basic VPWS Tunnel initiation/termination scenario with
 *     necessary configurations of vlan, tunnel header and mpls lables.
 *
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a)Check the configurations by 'vlan show', 'l3 intf show' and
 *        'l3 egress show'.
 *
 *     b) Transmit the customer packet on access_port. The contents of the
 *        packet are printed on screen.
 *
 *     c) Expected Result:
 *     ===================
 *     We can see that VPWS tunnel packet out on egress network port.
 *
 *   Ingress Packet:
 *   0000 0000 0011 0000 0000 0022 8100 0000
 *   0800 4500 0036 0000 0000 40ff 65ac 0a00
 *   0014 0a00 000a 0001 0203 0405 0607 0809
 *   0a0b 0c0d 0e0f 1011 1213 1415 1617 1819
 *   1a1b 1c1d 1e1f 2021 2394 12f0
 *
 *   Egress Packet:
 *   0000 0000 00aa 0000 0000 00bb 8100 0016
 *   8847 0011 1010 0044 4110 0000 5678 0000
 *   0000 0011 0000 0000 0022 0800 4500 0036
 *   0000 0000 40ff 65ac 0a00 0014 0a00 000a
 *   0001 0203 0405 0607 0809 0a0b 0c0d 0e0f
 *   1011 1213 1415 1617 1819 1a1b 1c1d 1e1f
 *   2021 2394 12f0
 */

/* Reset C interpreter*/
cint_reset();

bcm_port_t customer_port;
bcm_port_t provider_port;
bcm_gport_t customer_gport;
bcm_gport_t provider_gport;

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
    int i = 0, port = 0;
    bcm_error_t rv = 0;
    bcm_port_config_t configP;
    bcm_pbmp_t ports_pbmp;

    rv = bcm_port_config_get(unit, &configP);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_port_config_get: %s.\n",bcm_errmsg(rv));
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
ingress_port_multi_setup(int unit, bcm_pbmp_t pbm)
{
    bcm_field_entry_t entry;
    bcm_port_t     port;
    bcm_field_group_config_t group_config;

    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    group_config.priority = 1;

    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    BCM_PBMP_ITER(pbm, port) {
        printf("ingress_port_multi_setup port: %d setup\n", port);
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port,
                                                  BCM_PORT_LOOPBACK_MAC));

        BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group,
                                                   &entry));

        BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port,
                                                  BCM_FIELD_EXACT_MATCH_MASK));
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry,
                                                 bcmFieldActionCopyToCpu,0,0));

        BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
    }

    return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode and installs
 * an IFP rule. This IFP rule copies the packets ingressing
 * on the specified port to CPU. Port is also configured
 * to discard all packets. This is to avoid continuous
 * loopback of the packet from the egress port.
 */
bcm_error_t
egress_port_multi_setup(int unit, bcm_pbmp_t pbm)
{
    bcm_field_group_t group;
    bcm_field_entry_t entry;
    bcm_port_t     port;

    bcm_field_group_config_t group_config;

    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDrop);
    group_config.priority = 2;

    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    BCM_PBMP_ITER(pbm, port) {
        printf("egress_port_multi_setup port: %d setup\n", port);

        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port,
                                                  BCM_PORT_LOOPBACK_MAC));

        BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group,
                                                   &entry));

        BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port,
                                                  BCM_FIELD_EXACT_MATCH_MASK));
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry,
                                                 bcmFieldActionCopyToCpu,0,0));
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry,
                                                 bcmFieldActionDrop, 0, 0));

        BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
    }

    return BCM_E_NONE;
}

/*
 * Test Setup:
 * This functions gets the port numbers and sets up ingress and
 * egress ports.Check ingress_port_multi_setup() and egress_port_multi_setup().
 */
bcm_error_t
test_setup(int unit)
{
    int num_ports = 2;
    int port_list[num_ports];

    if (BCM_E_NONE != portNumbersGet(unit, port_list, num_ports)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    customer_port = port_list[0];
    provider_port = port_list[1];

    ingress_port = customer_port;
    egress_port = provider_port;

    if (BCM_E_NONE !=bcm_port_gport_get(unit, customer_port, &customer_gport)){
        printf("bcm_port_gport_get() failed to get gport for port %d\n",
                                                         customer_port);
        return -1;
    }

    if (BCM_E_NONE != bcm_port_gport_get(unit, provider_port,&provider_gport)){
        printf("bcm_port_gport_get() failed to get gport for port %d\n",
                                                          provider_port);
        return -1;
    }

    bcm_pbmp_t  pbmp;
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port);
    if (BCM_E_NONE != ingress_port_multi_setup(unit, pbmp)) {
        printf("ingress_port_multi_setup() failed for port %d\n",ingress_port);
        return -1;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, egress_port);
    if (BCM_E_NONE != egress_port_multi_setup(unit, pbmp)) {
        printf("egress_port_multi_setup() failed for port %d\n", egress_port);
        return -1;
    }

    bshell(unit, "pw start report +raw +decode +pmd");

    return BCM_E_NONE;
}

/*Verify function call*/
void
verify(int unit)
{
    char str[512];

    print "vlan show";
    bshell(unit, "vlan show");
    print "l2 show";
    bshell(unit, "l2 show");
    print "l3 intf show";
    bshell(unit, "l3 intf show");
    print "l3 egress show";
    bshell(unit, "l3 egress show");
    bshell(unit, "clear c");
    printf("\nSending unicast packet to ingress port:%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x0000000000110000000000220800450000360000000040FF65AC0A0000140A00000A000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F2021239412F0; sleep quiet 3", ingress_port);
    bshell(unit, str);
    print "show c";
    bshell(unit, "show c");

    return;
}

/* Create vlan and add port to vlan */
bcm_error_t
vlan_create_add_port(int unit, int vid, int port)
{
    bcm_pbmp_t pbmp, upbmp;

    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    return bcm_vlan_port_add(unit, vid, pbmp, upbmp);
}

/* Create L2 Station Entry */
bcm_error_t
l2_station_add(int unit, bcm_mac_t mac_addr, bcm_vlan_t vlan, int flags)
{
    int station_id;
    int rv = BCM_E_NONE;

    bcm_l2_station_t l2_station;
    bcm_l2_station_t_init(&l2_station);

    l2_station.dst_mac = mac_addr;
    l2_station.vlan = vlan;
    l2_station.dst_mac_mask = "FF:FF:FF:FF:FF:FF";
    l2_station.flags = flags;

    return bcm_l2_station_add(unit, &station_id, &l2_station);
}

/* Create L3 interface */
bcm_error_t
create_l3_interface(int unit, int flags, int intf_flags, bcm_mac_t local_mac,
                    int vid, bcm_if_t *intf_id)
{
    bcm_l3_intf_t l3_intf;
    bcm_error_t rv = BCM_E_NONE;

    /* L3 interface and egress object for access */
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_flags = flags;
    sal_memcpy(l3_intf.l3a_mac_addr, local_mac, sizeof(local_mac));
    l3_intf.l3a_vid = vid;
    l3_intf.l3a_intf_flags = intf_flags;
    rv = bcm_l3_intf_create(unit, &l3_intf);
    *intf_id = l3_intf.l3a_intf_id;
    return rv;
}

/* Create L3 next_hop */
bcm_error_t
create_egr_object(int unit, uint32 flag, int l3_if, bcm_mac_t nh_mac,
                  bcm_gport_t gport, int vid, bcm_if_t *egr_obj_id)
{
    bcm_l3_egress_t l3_egress;
    bcm_error_t rv = BCM_E_NONE;

    bcm_l3_egress_t_init(&l3_egress);
    l3_egress.flags2 |= flag;
    l3_egress.intf  = l3_if;
    sal_memcpy(l3_egress.mac_addr,  nh_mac, sizeof(nh_mac));
    l3_egress.vlan = vid;
    l3_egress.port = gport;
    rv = bcm_l3_egress_create(unit, 0, &l3_egress, egr_obj_id);
    return rv;
}

/* Create vpws vpn */
bcm_error_t
mpls_vpn_create(int unit, bcm_vpn_t *vpn)
{
    bcm_mpls_vpn_config_t vpn_info;
    bcm_error_t rv = BCM_E_NONE;

    bcm_mpls_vpn_config_t_init(&vpn_info);
    vpn_info.flags = BCM_MPLS_VPN_VPWS;
    rv = bcm_mpls_vpn_id_create(unit, &vpn_info);
    printf("VPN ID = %x\n", vpn_info.vpn);
    *vpn = vpn_info.vpn;
    return rv;
}

/* Create mpls customer port and add it to the vpn */
bcm_error_t
mpls_customer_port_create(int unit, bcm_vpn_t vpn, uint32 vid)
{
    bcm_mpls_port_t  cust_mpls_port;
    bcm_error_t rv = BCM_E_NONE;

    bcm_mpls_port_t_init(&cust_mpls_port);
    cust_mpls_port.port = customer_gport;
    cust_mpls_port.criteria = BCM_MPLS_PORT_MATCH_PORT;
    rv = bcm_mpls_port_add(unit, vpn, &cust_mpls_port);
    printf("MPLS customer port = 0x%x\n", cust_mpls_port.mpls_port_id);
    return rv;
}

/* Create mpls network port and add it to the vpn */
bcm_error_t
mpls_provider_port_create(int unit, bcm_vpn_t vpn, uint32 vc_label_init,
                          uint32 vc_label_term, uint8 ttl, bcm_if_t egr_obj,
                          bcm_if_t tunnel_id)
{

    bcm_mpls_port_t  net_mpls_port;
    bcm_error_t rv = BCM_E_NONE;

    bcm_mpls_port_t_init(&net_mpls_port);
    net_mpls_port.flags =  BCM_MPLS_PORT_NETWORK | BCM_MPLS_PORT_ENCAP_WITH_ID | BCM_MPLS_PORT_SEQUENCED;
    net_mpls_port.pw_seq_number = 0x5678;
    net_mpls_port.port = provider_gport;
    net_mpls_port.criteria = BCM_MPLS_PORT_MATCH_LABEL;
    net_mpls_port.match_label = vc_label_term;
    net_mpls_port.encap_id = egr_obj;
    net_mpls_port.egress_label.label = vc_label_init;
    net_mpls_port.egress_label.ttl = ttl;
    net_mpls_port.egress_label.flags = BCM_MPLS_EGRESS_LABEL_TTL_SET;
    net_mpls_port.tunnel_id = tunnel_id;
    rv = bcm_mpls_port_add(unit, vpn, &net_mpls_port);
    printf("MPLS network port = 0x%x\n", net_mpls_port.mpls_port_id);
    return rv;
}

/* Create mpls tunnel label */
bcm_error_t
mpls_tunnel_initiator(int unit, uint32 label, uint8 ttl, uint32 flags,
                      bcm_gport_t *tunnel_id)
{
    bcm_mpls_tunnel_encap_t  tunnel_encap;
    bcm_error_t rv = BCM_E_NONE;

    bcm_mpls_tunnel_encap_t_init(&tunnel_encap);
    tunnel_encap.label_array[0].label = label;
    tunnel_encap.label_array[0].ttl = ttl;
    tunnel_encap.label_array[0].flags = flags;
    tunnel_encap.num_labels = 1;
    rv = bcm_mpls_tunnel_encap_create(unit, 0, &tunnel_encap);
    printf("Tunnel ID = %x\n", tunnel_encap.tunnel_id);
    *tunnel_id = tunnel_encap.tunnel_id;
    return rv;
}

/* Create mpls tunnel termination label */
bcm_error_t
mpls_tunnel_terminator(int unit, uint32 label, uint32 flags,
                       bcm_mpls_switch_action_t action)
{
    bcm_mpls_tunnel_switch_t info;
    bcm_error_t rv = BCM_E_NONE;

    bcm_mpls_tunnel_switch_t_init(&info);
    info.flags = flags;
    info.label = label;
    info.port  = BCM_GPORT_INVALID;
    info.action = action;
    rv = bcm_mpls_tunnel_switch_add(unit, &info);
    return rv;
}


/* switch vpws configuration function */
bcm_error_t
setup_vpws(int unit)
{
    bcm_vlan_t  customer_vid = 11, provider_vid = 22;
    bcm_mac_t   outer_da = {0x00, 0x00, 0x00, 0x00, 0x00, 0xaa};
    bcm_mac_t   outer_sa = {0x00, 0x00, 0x00, 0x00, 0x00, 0xbb};
    bcm_pbmp_t  pbmp, ubmp;
    bcm_if_t    l3_intf;
    bcm_if_t    egr_obj;
    uint32 tunnel_label_init = 0x111;
    uint32 tunnel_label_term = 0x222;
    uint32 vc_label_term     = 0x333;
    uint32 vc_label_init     = 0x444;
    uint8 ttl = 16;
    bcm_mpls_vpn_config_t vpn_info;
    bcm_mpls_egress_label_t tun_label;
    bcm_mpls_tunnel_switch_t tunnel_switch;
    bcm_vpn_t   vpn;
    bcm_gport_t tunnel_id;
    bcm_error_t rv = BCM_E_NONE;

    printf("create vlan %d\n", customer_vid);
    rv = vlan_create_add_port(unit, customer_vid, customer_port);
    if (BCM_FAILURE(rv)) {
        printf("Error in vlan_create_add_port: %s\n", bcm_errmsg(rv));
        return rv;
    }

    printf("create vlan %d\n", provider_vid);
    rv = vlan_create_add_port(unit, provider_vid, provider_port);
    if (BCM_FAILURE(rv)) {
        printf("Error in vlan_create_add_port: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create an L3 interface to be used for the MPLS tunnel */
    rv = create_l3_interface(unit, 0, BCM_L3_INTF_UNDERLAY, outer_sa,
                             provider_vid, &l3_intf);
    if (BCM_FAILURE(rv)) {
        printf("\nError in create_l3_interface: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create L3 egress object for MPLS tunnel */
    rv = create_egr_object(unit, BCM_L3_FLAGS2_UNDERLAY, l3_intf, outer_da,
                           provider_gport, provider_vid, &egr_obj);
    if (BCM_FAILURE(rv)) {
        printf("\nError in create_egr_object: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create VPWS VPN */
    rv = mpls_vpn_create(unit, &vpn);
    if (BCM_FAILURE(rv)) {
        printf("\nError in mpls_vpn_create: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Set MPLS tunnel initiator */
    rv = mpls_tunnel_initiator(unit, tunnel_label_init, ttl,
                               BCM_MPLS_EGRESS_LABEL_TTL_SET, &tunnel_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in mpls_tunnel_initiator: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Add MPLS label lookup for tunnel termination */
    rv = mpls_tunnel_terminator(unit, tunnel_label_term,
                                BCM_MPLS_SWITCH_COUNTED,
                                BCM_MPLS_SWITCH_ACTION_POP);
    if (BCM_FAILURE(rv)) {
        printf("\nError in mpls_tunnel_terminator: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create mpls access port and bind it to vpn */
    rv = mpls_customer_port_create(unit, vpn, customer_vid);
    if (BCM_FAILURE(rv)) {
        printf("\nError in mpls_customer_port_create: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create mpls provider port and bind it to vpn */
    rv = mpls_provider_port_create(unit, vpn, vc_label_init, vc_label_term,
                                   ttl, egr_obj, tunnel_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in mpls_provider_port_create: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Config my station entry */
    rv = l2_station_add(unit, outer_sa, provider_vid, 0);
    if (BCM_FAILURE(rv)) {
        printf("\nError in l2_station_add: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in setup_vpws())
 *  c) demonstrates the functionality(done in verify()).
 */
bcm_error_t
execute(void)
{
    bcm_error_t rv;
    int unit = 0;
    print "config show; attach; version";
    bshell(unit, "config show; attach ; version");

    print "~~~ #1) test_setup(): ** start **";
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return -1;
    }
    print "~~~ #1) test_setup(): ** end **";

    print "~~~ #2) setup_vpws(): ** start **";
    if (BCM_FAILURE((rv = setup_vpws(unit)))) {
        printf("setup_vpws() failed.\n");
        return -1;
    }
    print "~~~ #2) setup_vpws(): ** end **";

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

