/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : VPLS with Flex Counter
 *
 * Usage    : BCM.0> cint vpls_flex_counter.c
 *
 * config   : mpls_config.bcm
 *
 * Log file : vpls_flex_counter_log.txt
 *
 * Test Topology :
 *
 *                  +------------------------------+
 *                  |                              |
 *                  |                              |
 *                  |                              |
 *     access_port  |                              |  network_port
 * +----------------+          SWITCH              +-----------------+
 *                  |                              |
 *                  |                              |
 *                  |                              |
 *                  |                              |
 *                  |                              |
 *                  +------------------------------+
 * Summary:
 * ========
 *   This Cint example to show configuration of (L2 MPLS) VPLS tunnel initiation
 *   and termination flows with flex counters using BCM APIs.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects two ports and configure them in Loopback mode. Out of these
 *        two ports, one port is used as access_port and the other as
 *        network_port.
 *
 *     b) Install an IFP rule to copy incoming packets to CPU and start
 *        packet watcher.
 *
 *     Note: IFP rule is meant for a testing purpose only (Internal) and it is
 *           nothing to do with an actual functional test.
 *
 *   2) Step2 - Configuration (Done in config_vpls_flex_counter()):
 *   =========================================================
 *     a) Configure a basic VPLS Tunnel initiation/termination scenario and
 *        attach the flex counters for ingress, egress and label objects for VPLS
 *        setup and does the necessary configurations of vlan, interface and next hop.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show', 'l3 intf show' and
 *        'l3 egress show'.
 *
 *     b) Transmit the customer packet on access_port and Transmit the provider
 *        /tunneled packet on network_port. The contents of the packet are
 *        printed on screen.
 *
 *     c) Expected Result:
 *     ===================
 *       We can see that VPLS tunnel initiation flows out of network_port and also
 *       VPLS tunnel terminates, and customer packet flows out of access_port.
 *       Also, we can see the flex counters for ingress, egress and label objects
 *       for tranmitted packets per direction.
 *       The contents of the packet are printed on screen. Also run the 'show c'
 *       to check the Tx/Rx packet stats/counters.
 */

/* Reset C interpreter*/
cint_reset();

bcm_port_t access_port;
bcm_port_t network_port;
bcm_vpn_t vpn;
bcm_gport_t mpls_c_port_id;
bcm_gport_t mpls_p_port_id;
uint32	tunnel_label_term = 0x222;   /* 546 */

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


bcm_error_t
config_vpls_flex_counter(int unit)
{
    bcm_port_t customer_port = access_port;
    bcm_port_t provider_port = network_port;
    bcm_gport_t customer_gport, provider_gport;
    bcm_vlan_t  customer_vid=11, provider_vid=22;
    bcm_pbmp_t  pbmp, ubmp;
    bcm_l3_intf_t l3_intf;
    bcm_mac_t   outer_da = {0x00, 0x00, 0x00, 0x00, 0x00, 0xaa};
    bcm_mac_t   outer_sa = {0x00, 0x00, 0x00, 0x00, 0x00, 0xbb};
    bcm_l3_egress_t l3_egress;
    bcm_if_t	egr_obj;
    bcm_mpls_egress_label_t tun_label;
    uint8	ttl = 16;
    uint32	tunnel_label_init = 0x111;   /* 273 */
    uint32	vc_label_term     = 0x333;   /* 819 */
    uint32	vc_label_init     = 0x444;
    bcm_multicast_t bcast_group;
    bcm_mpls_vpn_config_t vpn_info;
    bcm_mpls_port_t mpls_c_port, mpls_p_port;
    bcm_mpls_tunnel_switch_t tunnel_switch;

    /* Enable egress mode */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1));

    /* Enable Ingress VLAN Translation on customer facing ports */
    BCM_IF_ERROR_RETURN(bcm_vlan_control_port_set(unit, customer_port, bcmVlanTranslateIngressEnable, 1));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, customer_port, bcmPortControlDoNotCheckVlan, 1));

    /* Disable VLAN checks on provider network facing port */
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, provider_port, bcmPortControlDoNotCheckVlan, 1));

    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, customer_port, &customer_gport));
    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, provider_port, &provider_gport));

    /* VLAN Configuration */
    /* Customer side VLAN Configuration */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, customer_vid));
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, customer_port);
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, customer_vid, pbmp, ubmp));

    /* Provider side VLAN Configuration */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, provider_vid));
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, provider_port);
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, provider_vid, pbmp, ubmp));

    /* Create L3 interface for MPLS tunnel
     *   l3 intf add INtf=$intf_id Mac=$outer_sa Vlan=$provider_vid */
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_flags  = BCM_L3_ADD_TO_ARL;
    sal_memcpy(l3_intf.l3a_mac_addr, outer_sa, 6);
    l3_intf.l3a_vid = provider_vid;
    BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &l3_intf));

    /* Set MPLS tunnel initiator
     *   mpls tunnel init set EgrTunnelLabel=$tunnel_label_init
     *           EgrLabelOption=TTL_SET EgrTTL=$ttl INtf=$intf_id */
    bcm_mpls_egress_label_t_init(&tun_label);
    tun_label.flags   =   BCM_MPLS_EGRESS_LABEL_TTL_SET;
    tun_label.label   =   tunnel_label_init;
    tun_label.exp     =   0;
    tun_label.ttl     =   ttl;
    BCM_IF_ERROR_RETURN(bcm_mpls_tunnel_initiator_set(unit, l3_intf.l3a_intf_id, 1, &tun_label));

    /* Create L3 egress object to MPLS tunnel
     *   l3 egress add INtf=$intf_id Mac=$outer_da Port=$provider_port
     */
    bcm_l3_egress_t_init(&l3_egress);
    l3_egress.intf     = l3_intf.l3a_intf_id;
    sal_memcpy(l3_egress.mac_addr, outer_da, 6);
    l3_egress.vlan     =  provider_vid;
    l3_egress.port     =  provider_port;
    BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj));

    /* Create a broadcast group for VPLS L2 lookup DLF or Broadcast traffic
     *   mpls mcast group create vpls McastGroup=10 */
    BCM_IF_ERROR_RETURN(bcm_multicast_create(unit, BCM_MULTICAST_TYPE_VPLS, &bcast_group));

    /*
     * Create a VPLS VPN, used by SDK to bind the VPLS customer and provider ports
     *   mpls vpn create vpls VPN=$vpn_id BcastGroup=$bcast_group */
    bcm_mpls_vpn_config_t_init(&vpn_info);
    vpn_info.flags = BCM_MPLS_VPN_VPLS;
    vpn_info.broadcast_group = bcast_group;
    vpn_info.unknown_unicast_group = bcast_group;
    vpn_info.unknown_multicast_group = bcast_group;
    BCM_IF_ERROR_RETURN(bcm_mpls_vpn_id_create(unit, &vpn_info));
    vpn = vpn_info.vpn;
    printf ("VPN ID = 0x%x\n", vpn);

    /* Create and bind the customer side Virtual Port to VPLS VPN (repeate for all customer ports)
     *   mpls vpn port add VPN=$vpn_id Port=$customer_port Vlan=$customer_vid PortMatch=PortVlan */
    bcm_mpls_port_t_init(&mpls_c_port);
    mpls_c_port.port        =  customer_gport;
    mpls_c_port.criteria    =  BCM_MPLS_PORT_MATCH_PORT_VLAN;
    mpls_c_port.match_vlan  =  customer_vid;
    BCM_IF_ERROR_RETURN(bcm_mpls_port_add(unit, vpn, &mpls_c_port));
    mpls_c_port_id = mpls_c_port.mpls_port_id;

    BCM_IF_ERROR_RETURN(bcm_multicast_vpls_encap_get (unit, bcast_group, customer_gport,
            mpls_c_port.mpls_port_id, &mpls_c_port.encap_id));

    /* Add customer port to Bcast group to enable DLF flooding */
    printf("mpls_c_port.encap_id=%d\n", mpls_c_port.encap_id);
    BCM_IF_ERROR_RETURN(bcm_multicast_egress_add(unit, bcast_group, customer_gport, mpls_c_port.encap_id));

    /* Create and bind provider port to VPLS VPN (repeate for all provider ports)
     *   mpls vpn port add VPN=$vpn_id Port=$provider_port
     *     PortMatch=Label IntfFLaGs=NetWork,EgressTunnel,CouNTed
     *     VCInitLabel=$vc_label_init VCTermLabel=$vc_label_term EgrObj=$egr_object_id
     *     EgrLabelOption=TTL EgrTTL=$ttl */
    bcm_mpls_port_t_init(&mpls_p_port);
    mpls_p_port.flags = BCM_MPLS_PORT_NETWORK | BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_p_port.port  = provider_gport;
    mpls_p_port.criteria = BCM_MPLS_PORT_MATCH_LABEL;
    mpls_p_port.match_label = vc_label_term;
    mpls_p_port.egress_tunnel_if = egr_obj;
    mpls_p_port.egress_label.flags = BCM_MPLS_EGRESS_LABEL_TTL_SET;
    mpls_p_port.egress_label.label = vc_label_init;
    mpls_p_port.egress_label.exp = 0;
    mpls_p_port.egress_label.ttl = ttl;
    mpls_p_port.egress_label.pkt_pri = mpls_p_port.egress_label.pkt_cfi = 0;
    BCM_IF_ERROR_RETURN(bcm_mpls_port_add(unit, vpn, &mpls_p_port));
    mpls_p_port_id = mpls_p_port.mpls_port_id;

    BCM_IF_ERROR_RETURN(bcm_multicast_vpls_encap_get (unit, bcast_group, provider_gport,
            mpls_p_port.mpls_port_id, &mpls_p_port.encap_id));

    /* Add provider port to Bcast group to enable DLF flooding */
    printf("mpls_p_port.encap_id=%d\n", mpls_p_port.encap_id);
    BCM_IF_ERROR_RETURN(bcm_multicast_egress_add(unit, bcast_group, provider_gport, mpls_p_port.encap_id));

    /* Add MPLS incoming label lookup
     *   mpls tunnel switch add SwitchOptions=CouNTed
     *      ACTion=POP IngLabel=$tunnel_label_term IngInnerLabel=0
     *      EgrLabel=0  EgrObj=$egr_object_id */
    bcm_mpls_tunnel_switch_t_init(&tunnel_switch);
    tunnel_switch.flags  =  BCM_MPLS_SWITCH_COUNTED;
    tunnel_switch.label  =  tunnel_label_term;
    tunnel_switch.port   =  BCM_GPORT_INVALID;
    tunnel_switch.action =  BCM_MPLS_SWITCH_ACTION_POP;
    BCM_IF_ERROR_RETURN(bcm_mpls_tunnel_switch_add(unit, &tunnel_switch));

    /* Install L2 tunnel MAC to enable MPLS packet label lookup
     *    mpls tunnel l2 add MAC=$outer_Da Vlan=$outer_vid */
    BCM_IF_ERROR_RETURN(bcm_l2_tunnel_add(unit, outer_sa, provider_vid));

    return BCM_E_NONE;
}


bcm_error_t
get_flex_counters(int unit)
{
    print "===+  Display Flex Counters for VPLS Setup +===";
    bshell(unit, "sleep 5");
    uint32 stat_id_c;
    uint32 stat_id_p;
    uint32 stat_id_label;
    uint32 num_entries_c;
    uint32 num_entries_p;
    uint32 num_entries_label;
    uint32 egr_stat_id_c;
    uint32 egr_stat_id_p;
    uint32 egr_num_entries_c;
    uint32 egr_num_entries_p;
    uint32 counter_indexes[] = {0, 1};
    bcm_stat_value_t counter_values[2];
    int i;

    /*====================
     * Ingress counters
     ====================*/

    print "=== Flex counter for access VP ===";


    /* Flex counter for access VP */
    BCM_IF_ERROR_RETURN(bcm_stat_group_create(unit, bcmStatObjectIngMplsVcLabel,
            bcmStatGroupModeDlfAll, &stat_id_c, &num_entries_c));
    print num_entries_c;

    /* Flex counter for provider VP */
    BCM_IF_ERROR_RETURN(bcm_stat_group_create(unit, bcmStatObjectIngMplsVcLabel,
            bcmStatGroupModeDlfAll, &stat_id_p, &num_entries_p));
    print num_entries_p;

    BCM_IF_ERROR_RETURN(bcm_mpls_port_stat_attach(unit, vpn, mpls_c_port_id, stat_id_c));
    BCM_IF_ERROR_RETURN(bcm_mpls_port_stat_attach(unit, vpn, mpls_p_port_id, stat_id_p));

    /* Retrieve flex counters */
    BCM_IF_ERROR_RETURN(bcm_mpls_port_stat_counter_get(unit, vpn, mpls_c_port_id, bcmMplsInPkts,
            num_entries_c, counter_indexes, counter_values));
    for (i = 0; i < num_entries_c; i++) {
        printf("Access Ingress Packet count = %d\n", counter_values[i].packets);
    }
    for (i = 0; i < num_entries_c; i++) {
        printf("Access Ingress Packet count = %d:%d\n",
                COMPILER_64_HI(counter_values[i].packets64), COMPILER_64_LO(counter_values[i].packets64));
    }

    BCM_IF_ERROR_RETURN(bcm_mpls_port_stat_counter_get(unit, vpn, mpls_c_port_id, bcmMplsInBytes,
            num_entries_c, counter_indexes, counter_values));
    for (i = 0; i < num_entries_c; i++) {
        printf("Access Ingress BYTE count = %d:%d\n",
                COMPILER_64_HI(counter_values[i].bytes), COMPILER_64_LO(counter_values[i].bytes));
    }

    BCM_IF_ERROR_RETURN(bcm_mpls_port_stat_counter_get(unit, vpn, mpls_p_port_id, bcmMplsInPkts,
            num_entries_p, counter_indexes, counter_values));
    for (i = 0; i < num_entries_p; i++) {
        printf("Provider Ingress Packet count = %d\n", counter_values[i].packets);
    }

    BCM_IF_ERROR_RETURN(bcm_mpls_port_stat_counter_get(unit, vpn, mpls_p_port_id, bcmMplsInBytes,
            num_entries_p, counter_indexes, counter_values));
    for (i = 0; i < num_entries_p; i++) {
        printf("Access Ingress BYTE count = %d:%d\n",
                COMPILER_64_HI(counter_values[i].bytes), COMPILER_64_LO(counter_values[i].bytes));
    }

    /*====================
     * Egress counters
     ====================*/


    print "=== Flex counter for provider VP === ";

    /* Flex counter for provider VP */
    BCM_IF_ERROR_RETURN(bcm_stat_group_create(unit, bcmStatObjectEgrL3Intf,
            /* bcmStatGroupModeDlfAll*/bcmStatGroupModeSingle, &egr_stat_id_c, &egr_num_entries_c));
    print egr_num_entries_c;

    /* Flex counter for provider VP */
    BCM_IF_ERROR_RETURN(bcm_stat_group_create(unit, bcmStatObjectEgrL3Intf,
            /* bcmStatGroupModeDlfAll*/bcmStatGroupModeSingle, &egr_stat_id_p, &egr_num_entries_p));
    print egr_num_entries_p;

    BCM_IF_ERROR_RETURN(bcm_mpls_port_stat_attach(unit, vpn, mpls_c_port_id, egr_stat_id_c));
    BCM_IF_ERROR_RETURN(bcm_mpls_port_stat_attach(unit, vpn, mpls_p_port_id, egr_stat_id_p));

    /* Retrieve flex counters */
    BCM_IF_ERROR_RETURN(bcm_mpls_port_stat_counter_get(unit, vpn, mpls_c_port_id, bcmMplsOutPkts,
            egr_num_entries_c, counter_indexes, counter_values));
    for (i = 0; i < egr_num_entries_c; i++) {
        printf("Access Egress Packet count = %d\n", counter_values[i].packets);
    }

    BCM_IF_ERROR_RETURN(bcm_mpls_port_stat_counter_get(unit, vpn, mpls_c_port_id, bcmMplsOutBytes,
            egr_num_entries_c, counter_indexes, counter_values));
    for (i = 0; i < egr_num_entries_c; i++) {
        printf("Access Egress BYTE count = %d:%d\n",
                COMPILER_64_HI(counter_values[i].bytes), COMPILER_64_LO(counter_values[i].bytes));
    }

    BCM_IF_ERROR_RETURN(bcm_mpls_port_stat_counter_get(unit, vpn, mpls_p_port_id, bcmMplsOutPkts,
            egr_num_entries_p, counter_indexes, counter_values));
    for (i = 0; i < egr_num_entries_p; i++) {
        printf("Provider Egress Packet count = %d\n", counter_values[i].packets);
    }

    BCM_IF_ERROR_RETURN(bcm_mpls_port_stat_counter_get(unit, vpn, mpls_p_port_id, bcmMplsOutBytes,
            egr_num_entries_p, counter_indexes, counter_values));
    for (i = 0; i < egr_num_entries_p; i++) {
        printf("Provider Egress BYTE count = %d:%d\n",
                COMPILER_64_HI(counter_values[i].bytes), COMPILER_64_LO(counter_values[i].bytes));
    }

    /*=====================
     * Label counters
     =====================*/

    print "=== Label counters ===";
    BCM_IF_ERROR_RETURN(bcm_stat_group_create(unit, bcmStatObjectIngMplsSwitchLabel,
            bcmStatGroupModeDlfAll, &stat_id_label, &num_entries_label));
    print num_entries_label;

    BCM_IF_ERROR_RETURN(bcm_mpls_label_stat_attach(unit, tunnel_label_term, BCM_GPORT_INVALID, stat_id_label));
    /* BCM_IF_ERROR_RETURN(bcm_mpls_label_stat_attach(unit, tunnel_label_term, mpls_p_port_id, stat_id_label); */

    BCM_IF_ERROR_RETURN(bcm_mpls_label_stat_counter_get(unit, tunnel_label_term, BCM_GPORT_INVALID,
            bcmMplsInPkts, num_entries_label, counter_indexes, counter_values));
    for (i = 0; i < num_entries_label; i++) {
        printf("Provider Ingress Packet count = %d\n", counter_values[i].packets);
    }
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
    printf("\na) Sending a customer packet with Single VLAN to access_port:%d\n", access_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x000000445566000000AABBCC8100000B08004500002E0000000040FF79D20000000000000000000102030405060708090A0B0C0D0E0F10111213141516171819; sleep quiet 1", access_port);
    bshell(unit, str);
    bshell(unit, "show c");
    bshell(unit, "sleep 1");

    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, access_port, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, network_port, BCM_PORT_DISCARD_NONE));
    printf("\nb) Sending a MPLS packet with TWO Labels to network_port:%d\n", network_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x0000000000BB0000000000AA8100001688470022204000333140000000AABBCC0000004455668100000108004500002E0000000040FF79D20000000000000000910000038902A0010446000000000000D36D6169643438627974; sleep quiet 1", network_port);
    bshell(unit, str);
    bshell(unit, "show c");
    bshell(unit, "sleep 1");

    print "~~~ #3.1) Get Flex Counters for ingress, egress and label paths for VPLS Setup: ** start ** ~~~";
    if (BCM_FAILURE(get_flex_counters(unit))) {
        printf("get_flex_counters() failed.\n");
        return -1;
    }
    print "~~~ #3.1) Get Flex Counters for ingress, egress and label paths for VPLS Setup: ** end ** ~~~";

    return;
}

/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in config_vpls_flex_counter())
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

    print "~~~ #2) config_vpls_flex_counter(): ** start **";
    if (BCM_FAILURE((rv = config_vpls_flex_counter(unit)))) {
        printf("config_vpls_flex_counter() failed.\n");
        return -1;
    }
    print "~~~ #2) config_vpls_flex_counter(): ** end **";

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
