/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : L3 MPLS LSR with Trunk
 *
 * Usage    : BCM.0> cint lsr_trunk.c
 *
 * config   : mpls_config.bcm
 *
 * Log file : lsr_trunk_log.txt
 *
 * Test Topology :
 *
 *                  +------------------------------+
 *                  |                              |  egress_port1
 *                  |                              +-----------------+
 *                  |                              |
 *     ingress_port |                              |  egress_port2
 * +----------------+          SWITCH              +-----------------+
 *                  |                              |  egress_port3
 *                  |                              +-----------------+
 *                  |                              |  
 *                  |                              |
 *                  |                              |
 *                  +------------------------------+
 * Summary:
 * ========
 *   This Cint example to show configuration of L3 MPLS tunnel LSR with trunk
 *   using BCM APIs.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects five ports and configure them in Loopback mode. Out of these
 *        five ports, one port is used as ingress_port and the other as
 *        egress_ports(trunk egress members).
 *
 *     b) Install an IFP rule to copy incoming packets to CPU and start
 *        packet watcher.
 *
 *     Note: IFP rule is meant for a testing purpose only (Internal) and it is
 *           nothing to do with an actual functional test.
 *
 *   2) Step2 - Configuration (Done in config_lsr_trunk()):
 *   ======================================================
 *     a) Configure a basic L3 MPLS Tunnel Swap with Trunk functional scenario and
 *        does the necessary configurations of vlan, interface and next hop.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show', 'l3 intf show' and
 *        'l3 egress show'.
 *
 *     b) Transmit the MPLS packet. The contents of the packet are printed on screen.
 *
 *     c) Expected Result:
 *     ===================
 *       We can see that dmac, smac and vlan are all changed as the packet is
 *       routed through the egress port and observe that each time, the outgoing
 *       packet use a different trunk member port. Also run the 'show c'
 *       to check the Tx/Rx packet stats/counters.
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

/*
 * Verification:
 *  In Packet:
 *  ----------
 *  Note:
 *   With incrementing L2 header SA, packet should be spread into
 *   all ports of the trunk.
 *
 *  000000001111 000000000208 8100 000b
 *  8847 0011 1040 000a a140 0001 0203 0405
 *  0607 0809 0a0b 0c0d 0e0f 1011 1213 1415
 *  1617 1819 1a1b 1c1d 1e1f 2021 2223 2425
 *  b20c 3542 0000 0000
 *
 *  Out Packet:
 *  -----------
 *  000000aa2222 000000aa1212 8100 0016
 *  8847 0022 203f 000a a140 0001 0203 0405
 *  0607 0809 0a0b 0c0d 0e0f 1011 1213 1415
 *  1617 1819 1a1b 1c1d 1e1f 2021 2223 2425
 *  b20c 3542 0000 0000
 *
 *  Note:
 *   Each time, the outgoing packet use/selects a different trunk
 *   member as L2 SA is incrementing and RTAG7 hash should select
 *   the different member.
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
    printf("\n~~~~ With incrementing L2 header SA, packet should be spread into all ports of the trunk");
    printf("\n1) Sending MPLS packet with L2 SA=0x202 to ingress_port:%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x0000000011110000000002028100000B884700111040000AA140000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425B20C3542; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "show c");
    bshell(unit, "sleep 1");
    bshell(unit, "clear c");

    printf("\n2) Sending MPLS packet with L2 SA=0x203 to ingress_port:%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x0000000011110000000002038100000B884700111040000AA140000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425B20C3542; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "show c");
    bshell(unit, "sleep 1");
    bshell(unit, "clear c");

    printf("\n3) Sending MPLS packet with L2 SA=0x204 to ingress_port:%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x0000000011110000000002048100000B884700111040000AA140000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425B20C3542; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "show c");
    bshell(unit, "sleep 1");
    bshell(unit, "clear c");

    printf("\n4) Sending MPLS packet with L2 SA=0x205 to ingress_port:%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x0000000011110000000002058100000B884700111040000AA140000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425B20C3542; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "show c");
    bshell(unit, "sleep 1");
    bshell(unit, "clear c");

    printf("\n5) Sending MPLS packet with L2 SA=0x206 to ingress_port:%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x0000000011110000000002068100000B884700111040000AA140000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425B20C3542; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "show c");
    bshell(unit, "sleep 1");
    bshell(unit, "clear c");

    printf("\n6) Sending MPLS packet with L2 SA=0x207 to ingress_port:%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x0000000011110000000002088100000B884700111040000AA140000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425B20C3542; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "show c");
    bshell(unit, "sleep 1");
    bshell(unit, "clear c");

    return;
}

/*
 * Create a trunk, given array of member ports
 * OUT: trunk ID
 */
int
create_trunk(int unit, bcm_trunk_t *tid, int count, bcm_port_t *member_port)
{
    int rv, i;
    bcm_trunk_info_t trunk_info;
    bcm_trunk_member_t *member_info;
    bcm_trunk_member_t info;
    int mem_size;

    bcm_trunk_info_t_init(&trunk_info);
    mem_size = sizeof(info) * count;
    member_info = sal_alloc(mem_size, "trunk members");
    for (i = 0 ; i < count ; i++) {
        bcm_trunk_member_t_init(&member_info[i]);
        BCM_GPORT_MODPORT_SET(member_info[i].gport, 0, member_port[i]);
    }

    trunk_info.psc= BCM_TRUNK_PSC_PORTFLOW;
    trunk_info.dlf_index= BCM_TRUNK_UNSPEC_INDEX;
    trunk_info.mc_index= BCM_TRUNK_UNSPEC_INDEX;
    trunk_info.ipmc_index= BCM_TRUNK_UNSPEC_INDEX;

    BCM_IF_ERROR_RETURN(bcm_trunk_create(unit, BCM_TRUNK_FLAG_WITH_ID, tid));

    BCM_IF_ERROR_RETURN(bcm_trunk_set(unit, *tid, &trunk_info, count, member_info));

    return BCM_E_NONE;
}

int
trunk_rtag7_config(int unit)
{
    int flags;

    /* MPLS LSR non-terminated packet (RTAG7_MPLS_OUTER_HASH_FIELD_BMAP)
     * based on labels */
    flags = BCM_HASH_FIELD_SRCMOD |
            BCM_HASH_FIELD_SRCPORT |
            BCM_HASH_MPLS_FIELD_TOP_LABEL |
            BCM_HASH_MPLS_FIELD_2ND_LABEL |
            BCM_HASH_MPLS_FIELD_3RD_LABEL |
            BCM_HASH_MPLS_FIELD_LABELS_4MSB |
            BCM_HASH_FIELD_IP4DST_LO |
            BCM_HASH_FIELD_IP4DST_HI |
            BCM_HASH_FIELD_IP4SRC_LO |
            BCM_HASH_FIELD_IP4SRC_HI;
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashMPLSTunnelField0, flags)); /* Block A */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashMPLSTunnelField1, flags)); /* Block B */

    /* Only use L2 fields for hashing even for MPLS packets */
    BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit,bcmSwitchHashSelectControl, &flags));
    flags = BCM_HASH_FIELD0_DISABLE_MPLS |
            BCM_HASH_FIELD1_DISABLE_MPLS;
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashSelectControl, flags));

    /* L2 packet field selection (RTAG7_HASH_FIELD_BMAP3) */
    flags = BCM_HASH_FIELD_SRCMOD |
            BCM_HASH_FIELD_SRCPORT |
            BCM_HASH_FIELD_VLAN |
            BCM_HASH_FIELD_ETHER_TYPE |
            BCM_HASH_FIELD_MACDA_LO |
            BCM_HASH_FIELD_MACDA_MI |
            BCM_HASH_FIELD_MACDA_HI |
            BCM_HASH_FIELD_MACSA_LO |
            BCM_HASH_FIELD_MACSA_MI |
            BCM_HASH_FIELD_MACSA_HI;
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashL2Field0, flags)); /* Block A */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashL2Field1, flags)); /* Block B */

    /* L2 MPLS terminated packet(RTAG7_MPLS_L2_PAYLOAD_HASH_FIELD)
    flags = BCM_HASH_FIELD_SRCMOD |
	    BCM_HASH_FIELD_SRCPORT |
	    BCM_HASH_FIELD_ETHER_TYPE |
	    BCM_HASH_FIELD_MACDA_LO |
	    BCM_HASH_FIELD_MACDA_MI |
	    BCM_HASH_FIELD_MACDA_HI |
	    BCM_HASH_FIELD_MACSA_LO |
	    BCM_HASH_FIELD_MACSA_MI |
	    BCM_HASH_FIELD_MACSA_HI;
    print bcm_switch_control_set(unit, bcmSwitchHashL2MPLSField0, flags);
    print bcm_switch_control_set(unit, bcmSwitchHashL2MPLSField1, flags); */

    /* Set seeds */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashSeed0, 0x33333333));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashSeed1, 0x55555555));

    /* Enable preprocess */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashField0PreProcessEnable, TRUE));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashField1PreProcessEnable, TRUE));

    /* Configure HASH A and HASH B functions */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashField0Config,
                                       BCM_HASH_FIELD_CONFIG_CRC32LO));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashField0Config1,
                                       BCM_HASH_FIELD_CONFIG_CRC32HI));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashField1Config,
                                       BCM_HASH_FIELD_CONFIG_CRC32LO));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashField1Config1,
                                       BCM_HASH_FIELD_CONFIG_CRC32HI));

    /* Optional - Trunk application hash bits selection */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchTrunkHashSet0UnicastOffset, 0 + 0));

    return BCM_E_NONE;
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
config_lsr_trunk(int unit)
{
    bcm_error_t rv;
    bcm_vlan_t in_vlan = 11;
    bcm_vlan_t eg_vlan = 22;
    bcm_port_t in_port = ingress_port;
    bcm_port_t eg_port1 = egress_port1, eg_port2 = egress_port2;
    bcm_port_t eg_port3 = egress_port3;
    bcm_gport_t eg_gport1, eg_gport2, eg_gport3; 
    bcm_gport_t in_gport, trunk_gport;
    bcm_mac_t l2_mac    = {0x00, 0x00, 0x00, 0x00, 0x11, 0x11};
    bcm_mac_t local_mac = {0x00, 0x00, 0x00, 0xAA, 0x12, 0x12};
    bcm_mac_t nh_mac    = {0x00, 0x00, 0x00, 0xAA, 0x22, 0x22};
    bcm_if_t  intf_id = 10;
    bcm_l3_intf_t l3_intf;
    bcm_l3_egress_t  l3_egress;
    bcm_if_t egr_obj;
    uint32 in_tunnel_label = 0x111;  /* 273 */
    uint32 out_tunnel_label = 0x222;  /* 546 */
    uint32 in_pw_label = 0xAA;  /* 170 */
    uint32 out_pw_label = 0xBB;  /* 187 */
    bcm_mpls_tunnel_switch_t tunnel_switch;
    bcm_trunk_t trunk_id = 1;
    bcm_port_t trunk_ports[4];

    /* Use Egress Object mode */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1));

    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, in_port, &in_gport));

    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, eg_port1, &eg_gport1));
    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, eg_port2, &eg_gport2));
    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, eg_port3, &eg_gport3));

    /* Ingress side VLAN */
    print vlan_create_add_port(unit, in_vlan, in_port);

    /* Enable MPLS lookup for MAC $l2_mac and VLAN $in_vlan */
    BCM_IF_ERROR_RETURN(bcm_l2_tunnel_add(unit, l2_mac, in_vlan));

    /* Egress side VLAN */
    trunk_ports[0] = eg_port1;
    trunk_ports[1] = eg_port2;
    trunk_ports[2] = eg_port3;
    rv = vlan_create_add_ports(unit, eg_vlan, 3, &trunk_ports);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuration: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Create the trunk group */
    rv = create_trunk(unit, &trunk_id, 3, &trunk_ports);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuration: %s.\n",bcm_errmsg(rv));
        return rv;
    }
    BCM_GPORT_TRUNK_SET(trunk_gport, trunk_id);
    print trunk_gport;

    rv = trunk_rtag7_config(unit);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuration: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Create intf and add next hop */
    rv = create_l3_interface(unit, BCM_L3_WITH_ID|BCM_L3_ADD_TO_ARL,
            local_mac, eg_vlan, &intf_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuration: %s.\n",bcm_errmsg(rv));
        return rv;
    }
    printf("l3 intf = %d\n", intf_id);

    /* Create Egress object */
    rv = create_egr_obj(unit, intf_id, nh_mac, trunk_gport, &egr_obj);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuration: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Label swap operation - tunnel label */
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
 *  b) actual configuration (Done in config_lsr_trunk())
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

    print "~~~ #2) config_lsr_trunk(): ** start **";
    if (BCM_FAILURE((rv = config_lsr_trunk(unit)))) {
        printf("config_lsr_trunk() failed.\n");
        return -1;
    }
    print "~~~ #2) config_lsr_trunk(): ** end **";

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

