/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenSDK/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : L3 Multicast PIM-BIDIR
 *
 * Usage    : BCM.0> cint ipmc_pim_bidir.c
 *
 * config   : l3_config.bcm
 *
 * Log file : ipmc_log.txt
 *
 * Test Topology :
 *
 *                  +------------------------------+  egress_port1
 *                  |                              +-----------------+
 *                  |                              |
 *                  |                              |
 * ingress_port     |                              |  egress_port2
 * +----------------+          SWITCH              +-----------------+
 *                  |                              |
 *                  |                              |
 *                  |                              |  egress_port3
 *                  |                              +-----------------+
 *                  |                              |
 *                  +------------------------------+
 *
 * Summary:
 * ========
 *   This Cint example to show configuration of the IPv4 multicast with PIM-BIDIR
 *   functional scenario using BCM APIs.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Select one ingress (downstream intf) and three egress ports
 *        (upstream intfs) and configure them in Loopback mode.
 *
 *     b) Install an IFP rule to copy incoming packets to CPU and start
 *        packet watcher.
 *
 *     Note: IFP rule is meant for a testing purpose only (Internal) and it is
 *           nothing to do with an actual functional test.
 *
 *   2) Step2 - Configuration (Done in config_l3_ipmc_pim_bidir()):
 *   ==============================================================
 *     a) Configure a basic IPv4 multicast with PIM-BIDIR functional scenario.
 *        This adds the IPMC entry in l3 entry table [IPV4 mcast view] and
 *        does the necessary configurations of vlan, interface and next hop.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show', 'l3 intf show',
 *        'l3 egress show','multicast show', 'l2 show'  and 'ipmc table show'
 *
 *     b) Transmit the known IPv4 multicast pim-bidir packet on Downstream
 *        and Upstream Intfs with different VLANs. The contents of the packet
 *        are printed on screen. The packet used matches the route configured
 *        through script.
 *
 *     c) Expected Result:
 *     ===================
 *       We can see that smac and vlan are all changed as the packet is routed
 *       through the egress port, but dmac is not changed as its a mcast
 *       replication flow. Example,
 *       c.a) For pim-bidir with VID 10 packet from Downstream intf, should be
 *       forwarded to Upstream 1 & 2 intfs.
 *       c.b) For pim-bidir with VID 100 packet from Upstream#1 intf, should be
 *       forwarded to Upstream 2 intf.
 *       c.c) For pim-bidir with VID 200 packet from Upstream#2 intf, should be
 *       forwarded to Upstream 1 intf.
 *       c.d) For pim-bidir with VID 300 packet from No DF intf, shouldn't be
 *       forwarded.
 *       Also run the 'ipmc table show' to check the HIT bit status (y i.e Yes)
         and 'show c' to check the Tx/Rx packet stats/counters.
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
    int i=0,port=0,rv=0;
    bcm_port_config_t configP;
    bcm_pbmp_t ports_pbmp;

    rv = bcm_port_config_get(unit,&configP);
    if(BCM_FAILURE(rv)) {
        printf("\nError in retrieving port configuration: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    ports_pbmp = configP.e;
    for (i= 1; i < BCM_PBMP_PORT_MAX; i++) {
        if (BCM_PBMP_MEMBER(&ports_pbmp,i)&& (port < num_ports)) {
            port_list[port]=i;
            port++;
        }
    }

    if (( port == 0 ) || ( port != num_ports )) {
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
    bcm_field_qset_t  qset;
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
    bcm_field_qset_t  qset;
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
 * Select ingress and egress ports and configure them in
 * Loopback mode. Install a rule to copy incoming packets to CPU and
 * additional action to drop the packets when it loops back on egress
 * ports. Start packet watcher. Ingress port setup is done in
 * ingress_port_setup(). egress port setup is done in egress_port_setup().
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

void
verify(int unit)
{
    char str[512];

    bshell(unit, "hm ieee");
    bshell(unit, "vlan show");
    bshell(unit, "l3 intf show");
    bshell(unit, "l3 egress show");
    bshell(unit, "multicast show");
    bshell(unit, "ipmc table show");
    bshell(unit, "clear c");

    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, ingress_port, BCM_PORT_DISCARD_NONE));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, egress_port1, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, egress_port2, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, egress_port3, BCM_PORT_DISCARD_ALL));
    printf("\na) Sending IPv4 multicast packet (DIP=228.1.1.3),VID 10 from Downstream intf/ ingress_port :%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=01005E0101030000040002008100000a08004500002E0000000040FF92CA01010102E4010103000102030405060708090A0B0C0D0E0F101112131415161718194EEDC154; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "ipmc table show");
    bshell(unit, "show c");

    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, ingress_port, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, egress_port1, BCM_PORT_DISCARD_NONE));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, egress_port2, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, egress_port3, BCM_PORT_DISCARD_ALL));
    printf("\nb) Sending IPv4 multicast packet (DIP=228.1.1.3), VID 100 from Upstream#1 intf/egress_port1 :%d\n", egress_port1);
    snprintf(str, 512, "tx 1 pbm=%d data=01005E0101030000040002008100006408004500002E0000000040FF92CA01010102E4010103000102030405060708090A0B0C0D0E0F101112131415161718194EEDC154; sleep quiet 1", egress_port1);
    bshell(unit, str);
    bshell(unit, "ipmc table show");
    bshell(unit, "show c");

    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, ingress_port, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, egress_port1, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, egress_port2, BCM_PORT_DISCARD_NONE));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, egress_port3, BCM_PORT_DISCARD_ALL));
    printf("\nc) Sending IPv4 multicast packet (DIP=228.1.1.3), VID 200 from Upstream#2 intf/egress_port2 :%d\n", egress_port2);
    snprintf(str, 512, "tx 1 pbm=%d data=01005E010103000004000200810000C808004500002E0000000040FF92CA01010102E4010103000102030405060708090A0B0C0D0E0F101112131415161718194EEDC154; sleep quiet 1", egress_port2);
    bshell(unit, str);
    bshell(unit, "ipmc table show");
    bshell(unit, "show c");

    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, ingress_port, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, egress_port1, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, egress_port2, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, egress_port3, BCM_PORT_DISCARD_NONE));
    printf("\nd) Sending IPv4 multicast packet (DIP=228.1.1.3), VID 300 from None DF intf/egress_port3 :%d\n", egress_port3);
    snprintf(str, 512, "tx 1 pbm=%d data=01005E0101030000040002008100012C08004500002E0000000040FF92CA01010102E4010103000102030405060708090A0B0C0D0E0F101112131415161718194EEDC154; sleep quiet 1", egress_port3);
    bshell(unit, str);
    bshell(unit, "ipmc table show");
    bshell(unit, "show c");

    return;
}


int unit = 0;

/* Create vlan and add port to vlan */
int
create_vlan_add_port(int unit, bcm_vlan_t vlan, bcm_port_t port)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_pbmp_t pbmp, ubmp;

    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);

    rv = bcm_vlan_create(unit, vlan);
    if ((BCM_FAILURE(rv)) & (rv != BCM_E_EXISTS)) {
        printf("Error vlan: %d port:%d\n", vlan, port);
        return rv;
    }

    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vlan, pbmp, ubmp));

    return BCM_E_NONE;

}

/* Enable VLAN filtering */
int
enable_vlan_filtering(int unit, bcm_port_t port, uint32 flags)
{
    BCM_IF_ERROR_RETURN(bcm_port_vlan_member_set(unit, port, flags));

    return BCM_E_NONE;
}

/* Create ingress L3 interface */
int
create_ingress_l3_intf_vlan_set(int unit, bcm_vrf_t vrf, bcm_if_t l3_if1, bcm_vlan_t vid)
{
    bcm_l3_ingress_t ing_intf;
    bcm_if_t l3_intf_id;
    bcm_vlan_control_vlan_t vlan_info;

    bcm_l3_ingress_t_init(&ing_intf);
    ing_intf.flags = BCM_L3_INGRESS_WITH_ID;
    ing_intf.vrf = vrf;
    ing_intf.ipmc_intf_id = l3_if1;
    l3_intf_id = l3_if1;
    BCM_IF_ERROR_RETURN(bcm_l3_ingress_create(unit, &ing_intf, &l3_intf_id));

    bcm_vlan_control_vlan_t_init(&vlan_info);
    BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_get(unit, vid, &vlan_info));
    vlan_info.ingress_if = l3_intf_id;
    BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_set(unit, vid, vlan_info));

    return BCM_E_NONE;
}

/* Create egress L3 interface */
int
create_l3_intf(int unit, bcm_mac_t mac, bcm_vlan_t vlan, bcm_vrf_t vrf, bcm_if_t l3_intf_id)
{
    bcm_l3_intf_t l3_intf;

    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_flags = BCM_L3_WITH_ID;
    l3_intf.l3a_intf_id = l3_intf_id;
    l3_intf.l3a_vid = vlan;
    l3_intf.l3a_vrf = vrf;
    sal_memcpy(l3_intf.l3a_mac_addr, mac, sizeof(mac));

    BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &l3_intf));

    return BCM_E_NONE;
}

int
pim_bidir_config(int unit, int gport_count, bcm_gport_t *gport_array, bcm_if_t *encap_id_array, bcm_if_t l3_ingif)
{
    bcm_multicast_t mcg0 = 1;
    int rp_id = 1;
    bcm_ipmc_addr_t ipmc_addr;
    bcm_ipmc_range_t ipmc_range;
    int ipmc_range_id;

    /* Create multicast group 0x2000000 */
    BCM_IF_ERROR_RETURN(bcm_multicast_create(unit, BCM_MULTICAST_WITH_ID | BCM_MULTICAST_TYPE_L3, mcg0));

    BCM_IF_ERROR_RETURN(bcm_multicast_egress_set(unit, mcg0, gport_count, gport_array, encap_id_array));

    /* Create RP */
    BCM_IF_ERROR_RETURN(bcm_ipmc_rp_create(unit, BCM_IPMC_RP_WITH_ID, &rp_id));

    /* Add IPMC entry for multicast group 0x2000000*/
    bcm_ipmc_addr_t_init(&ipmc_addr);
    ipmc_addr.group = mcg0;
    ipmc_addr.mc_ip_addr = 0xe4010103;  /* 228.1.1.3 */
    ipmc_addr.s_ip_addr  = 0;  /* Should be empty for PIM-BIDIR */
    ipmc_addr.vrf = 0;
    ipmc_addr.vid = 0;
    ipmc_addr.flags = BCM_IPMC_SOURCE_PORT_NOCHECK;
    ipmc_addr.rp_id = rp_id;
    BCM_IF_ERROR_RETURN(bcm_ipmc_add(unit, &ipmc_addr));

    /* IPMC range setting */
    bcm_ipmc_range_t_init(&ipmc_range);
    ipmc_range.flags = BCM_IPMC_RANGE_PIM_BIDIR;
    ipmc_range.priority = 0;
    ipmc_range.mc_ip_addr = 0xe4010000; /* 228.1.1.0 */
    ipmc_range.mc_ip_addr_mask = 0xffff0000;
    ipmc_range.vrf = 0;
    ipmc_range.vrf_mask = 0;
    BCM_IF_ERROR_RETURN(bcm_ipmc_range_add(unit, &ipmc_range_id, &ipmc_range));

    /* RP setting */
    BCM_IF_ERROR_RETURN(bcm_ipmc_rp_add(unit, rp_id, l3_ingif));
    BCM_IF_ERROR_RETURN(bcm_ipmc_rp_add(unit, rp_id, encap_id_array[0]));
    BCM_IF_ERROR_RETURN(bcm_ipmc_rp_add(unit, rp_id, encap_id_array[1]));

    return BCM_E_NONE;
}


bcm_error_t
config_l3_ipmc_pim_bidir(int unit)
{
    bcm_port_config_t port_cfg;
    bcm_pbmp_t e_pbm;
    bcm_error_t rv;

    bcm_mac_t if1_mac = {0x00, 0x00, 0x00, 0x00, 0x01, 0x00};
    bcm_mac_t if2_mac = {0x00, 0x00, 0x00, 0x00, 0x02, 0x00};
    bcm_mac_t if3_mac = {0x00, 0x00, 0x00, 0x00, 0x03, 0x00};
    bcm_mac_t ingif_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x10};

    bcm_vlan_t vlan_if1 = 100, vlan_if2 = 200, vlan_if3 = 300, vlan_ingif=10;
    bcm_l3_intf_t l3_intf1, l3_intf2, l3_intf3, l3_ingintf;
    bcm_l3_ingress_t l3_ingress;
    bcm_if_t l3_if1 = 100, l3_if2 = 200, l3_if3 = 300, l3_ingif = 10;

    bcm_if_t encap_id_array[3];
    bcm_gport_t gport_xe0, gport_xe1, gport_xe2, gport_xe3, gport_array[3];
    bcm_port_t port_xe0 = ingress_port, port_xe1 = egress_port1, port_xe2 = egress_port2, port_xe3 = egress_port3;
    bcm_vrf_t vrf = 0;
    int gport_count;

    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3IngressMode, 1));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3IngressInterfaceMapSet, 1));

    /* Set local port gport */
    BCM_GPORT_LOCAL_SET(gport_xe0, port_xe0);
    BCM_GPORT_LOCAL_SET(gport_xe1, port_xe1);
    BCM_GPORT_LOCAL_SET(gport_xe2, port_xe2);
    BCM_GPORT_LOCAL_SET(gport_xe3, port_xe3);

    /* Create VLANs */
    bcm_port_config_t_init(&port_cfg);
    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &port_cfg));
    BCM_PBMP_ASSIGN(e_pbm, port_cfg.all);
    BCM_IF_ERROR_RETURN(bcm_vlan_port_remove(unit, 1, e_pbm));

    /* Create vlans and add port to vlans */
    rv = create_vlan_add_port(unit, vlan_if1, port_xe1);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring vlan : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = create_vlan_add_port(unit, vlan_if2, port_xe2);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring vlan : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = create_vlan_add_port(unit, vlan_if3, port_xe3);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring vlan : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = create_vlan_add_port(unit, vlan_ingif, port_xe0);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring vlan : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Enable VLAN filtering */
    uint32 flags = BCM_PORT_VLAN_MEMBER_INGRESS | BCM_PORT_VLAN_MEMBER_EGRESS;
    rv = enable_vlan_filtering(unit, port_xe0, flags);
    if (BCM_FAILURE(rv)) {
        printf("Error in enabling vlan filter : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = enable_vlan_filtering(unit, port_xe1, flags);
    if (BCM_FAILURE(rv)) {
        printf("Error in enabling vlan filter : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = enable_vlan_filtering(unit, port_xe2, flags);
    if (BCM_FAILURE(rv)) {
        printf("Error in enabling vlan filter : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = enable_vlan_filtering(unit, port_xe3, flags);
    if (BCM_FAILURE(rv)) {
        printf("Error in enabling vlan filter : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create ingress and egress L3 interfaces */
    rv = create_ingress_l3_intf_vlan_set(unit, vrf, l3_if1, vlan_if1);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring ingress l3 interface : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = create_l3_intf(unit, if1_mac, vlan_if1, vrf, l3_if1);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring l3 interface : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = create_l3_intf(unit, if2_mac, vlan_if2, vrf, l3_if2);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring l3 interface : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = create_ingress_l3_intf_vlan_set(unit, vrf, l3_if2, vlan_if2);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring ingress l3 interface : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = create_l3_intf(unit, if3_mac, vlan_if3, vrf, l3_if3);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring l3 interface : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = create_ingress_l3_intf_vlan_set(unit, vrf, l3_if3, vlan_if3);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring ingress l3 interface : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = create_l3_intf(unit, ingif_mac, vlan_ingif, vrf, l3_ingif);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring l3 interface : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = create_ingress_l3_intf_vlan_set(unit, vrf, l3_ingif, vlan_ingif);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring ingress l3 interface : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Set multicast group. if1 => xe1, if2 => xe2, if3 => xe3 */
    /* We are not DF for IF3 so it is not included in the rep list */
    encap_id_array[0] = l3_if1;
    encap_id_array[1] = l3_if2;
    gport_array[0] = gport_xe1;
    gport_array[1] = gport_xe2;
    gport_count = 2;

    rv = pim_bidir_config(unit, gport_count, gport_array, encap_id_array, l3_ingif);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring ip multicast : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("PIM BIDIR config done\n");
    return BCM_E_NONE;
}

/*
 * execute:
 *  This functions does the following
 *  a)test setup
 *  b)actual configuration (Done in config_l3_ipmc_pim_bidir())
 *  c)demonstrates the functionality(done in verify()).
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

    print "~~~ #2) config_l3_ipmc_pim_bidir(): ** start **";
    if (BCM_FAILURE((rv = config_l3_ipmc_pim_bidir(unit)))) {
        printf("config_l3_ipmc_pim_bidir() failed.\n");
        return -1;
    }
    print "~~~ #2) config_l3_ipmc_pim_bidir(): ** end **";

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
