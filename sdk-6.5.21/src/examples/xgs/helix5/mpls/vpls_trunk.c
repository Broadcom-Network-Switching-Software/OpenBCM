/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : VPLS with Trunk at access-side
 *
 * Usage    : BCM.0> cint vpls_trunk.c
 *
 * config   : mpls_config.bcm
 *
 * Log file : vpls_trunk_log.txt
 *
 * Test Topology :
 *
 *                  +------------------------------+
 *                  |                              |
 *  access_port1    |                              |
 * +------+         |                              |
 *  _port2| trunk   |                              |  network_port
 * +------+---------+          SWITCH              +-----------------+
 *  _port3|         |                              |
 * +------+         |                              |
 *                  |                              |
 *                  |                              |
 *                  |                              |
 *                  +------------------------------+
 * Summary:
 * ========
 *   This Cint example to show configuration of VPLS with Trunk, the customer
 *   port is a trunk, the trunk port slection is done via RTAG7 configuration
 *   using BCM APIs.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects four ports and configure them in Loopback mode. Out of these
 *        four ports, three ports are used as access_ports and the other port as
 *        network_port.
 *
 *     b) Install an IFP rule to copy incoming packets to CPU and start
 *        packet watcher.
 *
 *     Note: IFP rule is meant for a testing purpose only (Internal) and it is
 *           nothing to do with an actual functional test.
 *
 *   2) Step2 - Configuration (Done in config_vpls_trunk()):
 *   =========================================================
 *     a) Configure a basic VPLS with Trunk functional scenario and
 *        does the necessary configurations of vlan, interface and next hop.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show', 'l3 intf show',
 *        'l3 egress show' and 'trunk show'.
 *
 *     b) Transmit the customer packet on one of the trunk member port/access_port
 *        and also send different customer payload by changing VID and SA Mac of
 *        MPLS Tunnel packet on network_port. The contents of the packet are
 *        printed on screen.
 *
 *     c) Expected Result:
 *     ===================
 *       We can see that the customer port is a trunk, the trunk port slection
 *       is done via RTAG7 configuration. By changing the VID and SA MAC of
 *       inner payload makes to select the different access_port/member_port
 *       of trunk group. Example, tested with different customer packets by
 *       changing said fields and it chooses the different member ports.
 *       Also run the 'show c' to check the Tx/Rx packet stats/counters.
 */

/* Reset C interpreter*/
cint_reset();
int unit = 0;

bcm_port_t access_port1;
bcm_port_t access_port2;
bcm_port_t access_port3;
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
    int port_list[4], i;

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 4)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    access_port1 = port_list[0];
    access_port2 = port_list[1];
    access_port3 = port_list[2];
    network_port = port_list[3];

    for (i = 0; i <= 2; i++) {
        if (BCM_E_NONE != ingress_port_setup(unit, port_list[i])) {
            printf("ingress_port_setup() failed for port %d\n", port_list[i]);
            return -1;
        }
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
    bshell(unit, "trunk show");
    bshell(unit, "clear c");

    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, access_port1, BCM_PORT_DISCARD_NONE));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, access_port2, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, access_port3, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, network_port, BCM_PORT_DISCARD_ALL));

    printf("\na) Sending a customer packet on 1st member port (of the trunk group)/access_port:%d\n", access_port1);
    snprintf(str, 512, "tx 1 pbm=%d data=0x000000445566000000AABBCC8100000B08004500002E0000000040FF79D20000000000000000000102030405060708090A0B0C0D0E0F10111213141516171819; sleep quiet 1", access_port1);
    bshell(unit, str);
    bshell(unit, "show c");
    bshell(unit, "sleep 4");
    bshell(unit, "clear c");

    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, access_port1, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, access_port2, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, access_port3, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, network_port, BCM_PORT_DISCARD_NONE));

    printf("\nb) Sending a provider packet on network_port:%d\n", network_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x0000000000BB0000000000AA8100001688470022204000333140000000AABBCC0000004455668100000B08004500002E0000000040FF79D20000000000000000910000038902A0010446000000000000D36D6169643438627974; sleep quiet 1", network_port);
    bshell(unit, str);
    bshell(unit, "show c");
    bshell(unit, "sleep 4");
    bshell(unit, "clear c");

    printf("\nc) Sending a provider packet with payload VID changed to 0x11 on network_port:%d\n", network_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x0000000000BB0000000000AA8100001688470022204000333140000000AABBCC0000004455668100001108004500002E0000000040FF79D20000000000000000910000038902A0010446000000000000D36D6169643438627974; sleep quiet 1", network_port);
    bshell(unit, str);
    bshell(unit, "show c");
    bshell(unit, "sleep 4");
    bshell(unit, "clear c");

    printf("\nd) Sending a provider packet with payload SA changed on network_port:%d\n", network_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x0000000000BB0000000000AA8100001688470022204000333140000000AABBCC0000110055668100000B08004500002E0000000040FF79D20000000000000000910000038902A0010446000000000000D36D6169643438627974; sleep quiet 1", network_port);
    bshell(unit, str);
    bshell(unit, "show c");
    bshell(unit, "sleep 4");
    bshell(unit, "clear c");

    printf("\ne) Sending a provider packet/ MiM packet on network_port:%d\n", network_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x0000000000BB0000000000AA8100001688470022204000333140000000AABBCC00000044556688E70000FACE00000001000100000100F300008100AFA000010203040506070809F30A0B0C0D0E0F10111213141516171819F31A1B1C1D1E1F20212223242526272829F32A2B2C06477D29F3000000000000000000000000000000000000258348EA; sleep quiet 1", network_port);
    bshell(unit, str);
    bshell(unit, "show c");
    bshell(unit, "sleep 4");
    bshell(unit, "clear c");

    printf("\nf) Sending a provider packet/ MiM packet with MiM SA changed on network_port:%d\n", network_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x0000000000BB0000000000AA8100001688470022204000333140000000AABBCC00001100556688E70000FACE00000001000100000100F300008100AFA000010203040506070809F30A0B0C0D0E0F10111213141516171819F31A1B1C1D1E1F20212223242526272829F32A2B2C06477D29F3000000000000000000000000000000000000258348EA; sleep quiet 1", network_port);
    bshell(unit, str);
    bshell(unit, "show c");
    bshell(unit, "sleep 4");
    bshell(unit, "clear c");

    return;
}

bcm_error_t
CreateTrunk(int u, bcm_trunk_t *tid, int port1, int port2, int port3)
{
    int rv,i;
    int flags;

    /* Create trunk with ports ge3, ge4, ge7 and ge8 */
    bcm_trunk_info_t trunk_info;
    bcm_trunk_member_t  member_info[3];

    bcm_trunk_info_t_init(&trunk_info);
    for (i = 0 ; i<3 ; i++) {
	  bcm_trunk_member_t_init(&member_info[i]);
    }

    trunk_info.psc= BCM_TRUNK_PSC_PORTFLOW;
    trunk_info.dlf_index= BCM_TRUNK_UNSPEC_INDEX;
    trunk_info.mc_index= BCM_TRUNK_UNSPEC_INDEX;
    trunk_info.ipmc_index= BCM_TRUNK_UNSPEC_INDEX;

    BCM_GPORT_MODPORT_SET(member_info[0].gport, 0, port1);
    BCM_GPORT_MODPORT_SET(member_info[1].gport, 0, port2);
    BCM_GPORT_MODPORT_SET(member_info[2].gport, 0, port3);
   // BCM_GPORT_MODPORT_SET(member_info[3].gport, 0, port4);

    BCM_IF_ERROR_RETURN(bcm_trunk_create(u, BCM_TRUNK_FLAG_WITH_ID, tid));

    BCM_IF_ERROR_RETURN(bcm_trunk_set(u, *tid, &trunk_info, 3, member_info));

    BCM_IF_ERROR_RETURN(bcm_switch_control_get(u, bcmSwitchHashControl, &flags));
    flags |= (BCM_HASH_CONTROL_MULTIPATH_L4PORTS | BCM_HASH_CONTROL_TRUNK_NUC_ENHANCE |
              BCM_HASH_CONTROL_TRUNK_NUC_SRC | BCM_HASH_CONTROL_TRUNK_NUC_DST |
              BCM_HASH_CONTROL_TRUNK_NUC_MODPORT);
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(u, bcmSwitchHashControl, flags));

    /* Block A - L2 packet field selection (RTAG7_HASH_FIELD_BMAP3) */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(u, bcmSwitchHashL2Field0,
	    BCM_HASH_FIELD_SRCMOD |
	    BCM_HASH_FIELD_SRCPORT |
	    BCM_HASH_FIELD_VLAN |
	    BCM_HASH_FIELD_ETHER_TYPE |
	    BCM_HASH_FIELD_MACDA_LO |
	    BCM_HASH_FIELD_MACDA_MI |
	    BCM_HASH_FIELD_MACDA_HI |
	    BCM_HASH_FIELD_MACSA_LO |
	    BCM_HASH_FIELD_MACSA_MI |
	    BCM_HASH_FIELD_MACSA_HI));

    /* Block B - L2 packet field selection */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(u, bcmSwitchHashL2Field1,
	    BCM_HASH_FIELD_SRCMOD |
	    BCM_HASH_FIELD_SRCPORT |
	    BCM_HASH_FIELD_VLAN |
	    BCM_HASH_FIELD_ETHER_TYPE |
	    BCM_HASH_FIELD_MACDA_LO |
	    BCM_HASH_FIELD_MACDA_MI |
	    BCM_HASH_FIELD_MACDA_HI |
	    BCM_HASH_FIELD_MACSA_LO |
	    BCM_HASH_FIELD_MACSA_MI |
	    BCM_HASH_FIELD_MACSA_HI));

    /* Block A - L2 MPLS terminated packet(RTAG7_MPLS_L2_PAYLOAD_HASH_FIELD) */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(u, bcmSwitchHashL2MPLSField0,
	    BCM_HASH_FIELD_SRCMOD |
	    BCM_HASH_FIELD_SRCPORT |
	    BCM_HASH_FIELD_ETHER_TYPE |
	    BCM_HASH_FIELD_MACDA_LO |
	    BCM_HASH_FIELD_MACDA_MI |
	    BCM_HASH_FIELD_MACDA_HI |
	    BCM_HASH_FIELD_MACSA_LO |
	    BCM_HASH_FIELD_MACSA_MI |
	    BCM_HASH_FIELD_MACSA_HI));

    /* Block B - L2 MPLS terminated packet */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(u, bcmSwitchHashL2MPLSField1,
	    BCM_HASH_FIELD_SRCMOD |
	    BCM_HASH_FIELD_SRCPORT |
	    BCM_HASH_FIELD_ETHER_TYPE |
	    BCM_HASH_FIELD_MACDA_LO |
	    BCM_HASH_FIELD_MACDA_MI |
	    BCM_HASH_FIELD_MACDA_HI |
	    BCM_HASH_FIELD_MACSA_LO |
	    BCM_HASH_FIELD_MACSA_MI |
	    BCM_HASH_FIELD_MACSA_HI));

    /* Block A - MPLS non-terminated or tunneled packet(RTAG7_MPLS_OUTER_HASH_FIELD_BMAP) */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashMPLSTunnelField0,
            BCM_HASH_FIELD_SRCMOD |
            BCM_HASH_FIELD_SRCPORT |
            BCM_HASH_MPLS_FIELD_TOP_LABEL |
            BCM_HASH_MPLS_FIELD_2ND_LABEL |
            BCM_HASH_MPLS_FIELD_3RD_LABEL |
            BCM_HASH_MPLS_FIELD_LABELS_4MSB |
            BCM_HASH_FIELD_IP4DST_LO |
            BCM_HASH_FIELD_IP4DST_HI |
            BCM_HASH_FIELD_IP4SRC_LO |
            BCM_HASH_FIELD_IP4SRC_HI));

    /* Block B - MPLS non-terminated or tunneled packet */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashMPLSTunnelField1,
            BCM_HASH_FIELD_SRCMOD |
            BCM_HASH_FIELD_SRCPORT |
            BCM_HASH_MPLS_FIELD_TOP_LABEL |
            BCM_HASH_MPLS_FIELD_2ND_LABEL |
            BCM_HASH_MPLS_FIELD_3RD_LABEL |
            BCM_HASH_MPLS_FIELD_LABELS_4MSB |
            BCM_HASH_FIELD_IP4DST_LO |
            BCM_HASH_FIELD_IP4DST_HI |
            BCM_HASH_FIELD_IP4SRC_LO |
            BCM_HASH_FIELD_IP4SRC_HI));

    /* Configure HASH A and HASH B functions */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(u, bcmSwitchHashField0Config,
	    BCM_HASH_FIELD_CONFIG_CRC32LO));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(u, bcmSwitchHashField1Config,
	    BCM_HASH_FIELD_CONFIG_CRC32HI));

    return BCM_E_NONE;
}


bcm_error_t
config_vpls_trunk(int unit)
{
    bcm_port_t customer_port1 = access_port1, customer_port2 = access_port2;
    bcm_port_t customer_port3 = access_port3;
    bcm_port_t provider_port = network_port;
    bcm_gport_t customer_gport1, customer_gport2, customer_gport3;
    bcm_gport_t customer_gport, provider_gport;
    bcm_vlan_t	customer_vid=11, provider_vid=22;
    bcm_pbmp_t	pbmp, ubmp;
    bcm_l3_intf_t l3_intf;
    bcm_vrf_t	vrf = 0;
    bcm_mac_t   outer_da = {0x00, 0x00, 0x00, 0x00, 0x00, 0xaa};
    bcm_mac_t   outer_sa = {0x00, 0x00, 0x00, 0x00, 0x00, 0xbb};
    bcm_l3_egress_t l3_egress;
    bcm_if_t	egr_obj;
    bcm_mpls_egress_label_t tun_label;
    uint8	ttl = 16;
    uint32	tunnel_label_init = 0x111;
    uint32	tunnel_label_term = 0x222;
    uint32	vc_label_term     = 0x333;
    uint32	vc_label_init     = 0x444;
    bcm_multicast_t bcast_group;
    bcm_mpls_vpn_config_t vpn_info;
    bcm_mpls_port_t mpls_c_port, mpls_p_port;
    bcm_mpls_tunnel_switch_t tunnel_switch;
    bcm_vpn_t vpn;
    bcm_trunk_t tid = 1;

    /* Enable egress mode */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1));

    /* Enable Ingress VLAN Translation on customer facing ports */
    BCM_IF_ERROR_RETURN(bcm_vlan_control_port_set(unit, customer_port1, bcmVlanTranslateIngressEnable, 1));
    BCM_IF_ERROR_RETURN(bcm_vlan_control_port_set(unit, customer_port2, bcmVlanTranslateIngressEnable, 1));
    BCM_IF_ERROR_RETURN(bcm_vlan_control_port_set(unit, customer_port3, bcmVlanTranslateIngressEnable, 1));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, customer_port1, bcmPortControlDoNotCheckVlan, 1));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, customer_port2, bcmPortControlDoNotCheckVlan, 1));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, customer_port3, bcmPortControlDoNotCheckVlan, 1));

    /* Disable VLAN checks on provider network facing port */
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, provider_port, bcmPortControlDoNotCheckVlan, 1));

    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, customer_port1, &customer_gport1));
    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, customer_port2, &customer_gport2));
    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, customer_port3, &customer_gport3));
    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, provider_port, &provider_gport));

    /* VLAN Configuration */
    /* Customer side VLAN Configuration */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, customer_vid));
    BCM_IF_ERROR_RETURN(bcm_vlan_gport_add(unit, customer_vid, customer_gport1, 0));
    BCM_IF_ERROR_RETURN(bcm_vlan_gport_add(unit, customer_vid, customer_gport2, 0));
    BCM_IF_ERROR_RETURN(bcm_vlan_gport_add(unit, customer_vid, customer_gport3, 0));

    /* Create the trunk group */
    if (BCM_FAILURE(CreateTrunk(unit, &tid, customer_port1, customer_port2, customer_port3))) {
        printf("CreateTrunk() failed.\n");
        return -1;
    }

    BCM_GPORT_TRUNK_SET(customer_gport, tid);
    print customer_gport;

    /* Provider side VLAN Configuration */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, provider_vid));
    BCM_IF_ERROR_RETURN(bcm_vlan_gport_add(unit, provider_vid, provider_gport, 0));

    /* Create L3 interface for MPLS tunnel */
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_flags  = BCM_L3_ADD_TO_ARL;
    sal_memcpy(l3_intf.l3a_mac_addr, outer_sa, 6);
    l3_intf.l3a_vid = provider_vid;
    l3_intf.l3a_vrf = vrf;
    BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &l3_intf));

    /* Set MPLS tunnel initiator */
    bcm_mpls_egress_label_t_init(&tun_label);
    tun_label.flags   =   BCM_MPLS_EGRESS_LABEL_TTL_SET;
    tun_label.label   =   tunnel_label_init;
    tun_label.exp     =   0;
    tun_label.ttl     =   ttl;
    BCM_IF_ERROR_RETURN(bcm_mpls_tunnel_initiator_set(unit, l3_intf.l3a_intf_id, 1, &tun_label));

    /* Create L3 egress object to MPLS tunnel */
    bcm_l3_egress_t_init(&l3_egress);
    l3_egress.intf     = l3_intf.l3a_intf_id;
    sal_memcpy(l3_egress.mac_addr, outer_da, 6);
    l3_egress.vlan     =  provider_vid;
    l3_egress.port     =  provider_port;
    BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj));

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

    /* Create and bind the customer side VP to VPLS VPN (repeate for all customer ports) */
    bcm_mpls_port_t_init(&mpls_c_port);
    mpls_c_port.port        =  customer_gport;
    mpls_c_port.criteria    =  BCM_MPLS_PORT_MATCH_PORT_VLAN;
    mpls_c_port.match_vlan  =  customer_vid;
    BCM_IF_ERROR_RETURN(bcm_mpls_port_add(unit, vpn, &mpls_c_port));

    BCM_IF_ERROR_RETURN(bcm_multicast_vpls_encap_get (unit, bcast_group, customer_gport,
            mpls_c_port.mpls_port_id, &mpls_c_port.encap_id));

    /* Add customer port to Bcast group to enable DLF flooding */
    printf("mpls_c_port.encap_id=%d\n", mpls_c_port.encap_id);
    BCM_IF_ERROR_RETURN(bcm_multicast_egress_add(unit, bcast_group, customer_gport, mpls_c_port.encap_id));

    /* Create and bind provider port to VPLS VPN (repeate for all provider ports) */
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

    BCM_IF_ERROR_RETURN(bcm_multicast_vpls_encap_get (unit, bcast_group, provider_gport,
            mpls_p_port.mpls_port_id, &mpls_p_port.encap_id));

    /* Add provider port to Bcast group to enable DLF flooding */
    printf("mpls_p_port.encap_id=%d\n", mpls_p_port.encap_id);
    BCM_IF_ERROR_RETURN(bcm_multicast_egress_add(unit, bcast_group, provider_gport, mpls_p_port.encap_id));

    /* Add MPLS incoming label lookup */
    bcm_mpls_tunnel_switch_t_init(&tunnel_switch);
    tunnel_switch.flags  =  BCM_MPLS_SWITCH_COUNTED;
    tunnel_switch.label  =  tunnel_label_term;
    tunnel_switch.port   =  BCM_GPORT_INVALID;
    tunnel_switch.action =  BCM_MPLS_SWITCH_ACTION_POP;
    BCM_IF_ERROR_RETURN(bcm_mpls_tunnel_switch_add(unit, &tunnel_switch));

    /* Install L2 tunnel MAC to enable MPLS packet label lookup */
    BCM_IF_ERROR_RETURN(bcm_l2_tunnel_add(unit, outer_sa, provider_vid));

    return BCM_E_NONE;
}


/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in config_vpls_trunk())
 *  c) demonstrates the functionality(done in verify()).
 */
bcm_error_t
execute(void)
{
    bcm_error_t rv;
    print "config show; attach; cancun stat; version";
    bshell(unit, "config show; a ; cancun stat; version");

    print "~~~ #1) test_setup(): ** start **";
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return -1;
    }
    print "~~~ #1) test_setup(): ** end **";

    print "~~~ #2) config_vpls_trunk(): ** start **";
    if (BCM_FAILURE((rv = config_vpls_trunk(unit)))) {
        printf("config_vpls_trunk() failed.\n");
        return -1;
    }
    print "~~~ #2) config_vpls_trunk(): ** end **";

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
