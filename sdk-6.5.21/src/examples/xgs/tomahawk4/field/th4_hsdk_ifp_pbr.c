/*
 * Feature  : Policy based routing (PBR) using IFP rule
 *
 * Usage    : BCM.0> c th4_hsdk_ifp_pbr.c
 *
 * config   : th4_hsdk_ifp_pbr_config.yml
 *            with l3_alpm_template: 2
 *
 * Log file : th4_hsdk_ifp_pbr_log.txt
 *
 * Test Topology :
 *                              CPU
 *                                ^
 *                                |  Rx reason : CPU_SFLOW*
 *                                |
 *                      +---------+---------+
 *                      |        TH4        | redir port [2]
 *   ingress port[0]    |                   +------>
 *    in loopback       |   +-----------+   |
 *           ---------->+   | FP Policy |   |
 *      vlan  5         |   +-----------+   | egress port [1]
 *                      |route 192.168.1.xx +------>
 *                      |                   |
 *                      +-------------------+
 *
 *
 * Summary:
 * ========
 * cint script to demonstrate policy based routing which overrides LPM routing
 * decisions of IPv4 packets
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in testSetup()):
 *   =============================================
 *     a) Select three test ports and configure them in Loopback mode.
 *     b) Start packet watcher and register additional custom Rx callback for the
 *        packet verfication.
 *     c) In function l2Setup(), ingress port[0] is configured in a ingress VLAN/VFI and
 *        egress port[1-2] are configured in a egress VLAN/VFI.
 *     d) In function l3Setup(), primary L3 path is setup to route packets (dstIP
 *        192.168.1.[0-255]) subnet from ingress port[0] to port[1] also redirect egress
 *        object/next hop is created for IFP.
 *     d) Enable ingress sampling on the redirect port[2] with maximum rate with CPU as
 *        destination.
 *
 *   2) Step2 - IFP Configuration (Done in ifpPbrSetup()):
 *   =========================================================
 *     a) An IFP group is created with Outer vlan, Dst ip as qualifiers in auto mode
 *        with Redirect next hop as action.
 *     b) Create an IFP group entry to filter on dest IP range 192.168.1.[0-15] of ingress
 *        vlan tagged IPv4 packets and redirect the qualified packets to redirect nexthop
 *        with destination as port[2].
 *     c) In flexCounterSetup(), Add flex counters to the IFP entry solely for
 *        verfication purpose
 *     d) install the IFP entry in hardware
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the outputs of 'fp show' 'show c' 'l3 * show'
 *     b) The test packets routed by LPM/primary L3 path can be seen egress on port[1]
 *        in 'show c'.
 *     c) Expected Result:
 *     ===================
 *     The CPU packet capture shows the redirected packets (with dstIP 192.168.1.[0-15])
 *     are copied by egress port[2] sampler as below.
 *
 *   Egress Packet:
 *   [bcmPWN.0]Packet[1]: Total x
 *   [bcmPWN.0]Packet[1]: data[0000]: {000000000101} {00000000aaaa} 8100 000a
 *   [bcmPWN.0]Packet[1]: data[0010]: 0800 4500 002e 1234 4000 fe06 94ca 0a0a
 *   [bcmPWN.0]Packet[1]: data[0020]: 0a14 c0a8 0105 138c 0569 0000 0064 0000
 *   [bcmPWN.0]Packet[1]: data[0030]: 0064 5000 0fa0 b0b6 0000 aaaa aaaa aaaa
 *   [bcmPWN.0]Packet[1]: data[0040]: aaaa aaaa
 *   [bcmPWN.0]
 *   [bcmPWN.0][RX metadata information]
 *   ...
 *   [RX reasons]
 *   CPU_SFLOW
 *   CPU_SFLOW_CPU_SFLOW_SRC
 */

/* Reset C interpreter*/
cint_reset();

int num_ports = 3;
int port_list[num_ports];
bcm_vlan_t ingress_vlan = 5, egress_vlan = 10;
bcm_l3_egress_t l3_egr_redir;
bcm_if_t nh_redir;
bcm_mac_t dmac =                     {0x00, 0x00, 0x00, 0x00, 0x03, 0x03};
bcm_mac_t rdmac =                    {0x00, 0x00, 0x00, 0x00, 0x01, 0x01};
bcm_mac_t outgoing_router_mac =      {0x00, 0x00, 0x00, 0x00, 0xAA, 0xAA};
bcm_mac_t incoming_router_mac =      {0x00, 0x00, 0x00, 0x00, 0x02, 0x02};
bcm_ip_t route = 0xC0A80100;
bcm_ip_t route_mask = 0xffffff00;
bcm_ip_t route_pbr = 0xC0A80100;
bcm_ip_t route_pbrmask = 0xfffffff0;
int vrf_id = 1;
uint32 stat_counter_id;
bcm_field_entry_t entry;
bcm_field_group_config_t group_config;
bcm_field_flexctr_config_t flexctr_cfg;
int test_failed = 0; /* Final result will be stored here */
int rx_count = 0; /* Total test packet received by CPU for verification */

/* PBR IFP rule */
bcm_error_t
ifpPbrSetup(int unit) {

    int rv = 0;

    /* IFP group configuration with vlan and DST IPv4 qualifiers in Auto mode */
    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyDstIp);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyOuterVlanId);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionRedirectEgrNextHop);
    group_config.mode = bcmFieldGroupModeAuto;
    group_config.priority = 1;
    group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_MODE;

    rv = bcm_field_group_config_create(unit, &group_config);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_group_config_create() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create FP entry to match on ingress vlan and Dst IP */
    rv = bcm_field_entry_create(unit, group_config.group, &entry);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_entry_create() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_qualify_OuterVlanId(unit, entry, ingress_vlan, 0xFFF);
    rv |= bcm_field_qualify_DstIp(unit, entry, route_pbr, route_pbrmask);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_qualify_*() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }
    /* Ip packets in dstIP 192.168.1.[0-15] range will be redirected to
       next hop with dest port[2] overriding LPM resolved destination of port[1] */
    rv = bcm_field_action_add(unit, entry, bcmFieldActionRedirectEgrNextHop, nh_redir, 0);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_action_add() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Setup Flex Counters to verify IFP rule hits */
    rv = flexCounterSetup(unit, entry, group_config.group);
    if (BCM_FAILURE(rv)) {
        printf("flexCounterSetup() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Commit the IFP entry */
    rv = bcm_field_entry_install(unit, entry);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_entry_install() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;

}


bcm_error_t
flexCounterSetup(int unit, int eid, int group)  {

    int options = 0;
    bcm_error_t  rv;
    bcm_flexctr_action_t action;
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_a_op = NULL;
    bcm_flexctr_value_operation_t *value_b_op = NULL;

    bcm_flexctr_action_t_init(&action);

    action.flags = 0;
    /* Group ID passed as hint and IFP as source */
    action.hint = group;
    action.hint_type = bcmFlexctrHintFieldGroupId;
    action.source = bcmFlexctrSourceIfp;
    action.mode = bcmFlexctrCounterModeNormal;
    action.index_num = 2;
    action.drop_count_mode=bcmFlexctrDropCountModeAll;

    /* Counter index is IFP. */
    index_op = &action.index_operation;
    index_op->object[0] = bcmFlexctrObjectStaticIngFieldStageIngress;
    index_op->object_id[0] = 0;
    index_op->mask_size[0] = 8;
    index_op->shift[0] = 0;
    index_op->object[1] = bcmFlexctrObjectConstZero;
    index_op->mask_size[1] = 1;
    index_op->shift[1] = 0;

    /* Increase counter per packet. */
    value_a_op = &action.operation_a;
    value_a_op->select = bcmFlexctrValueSelectCounterValue;
    value_a_op->object[0] = bcmFlexctrObjectConstOne;
    value_a_op->mask_size[0] = 16;
    value_a_op->shift[0] = 0;
    value_a_op->object[1] = bcmFlexctrObjectConstZero;
    value_a_op->mask_size[1] = 1;
    value_a_op->shift[1] = 0;
    value_a_op->type = bcmFlexctrValueOperationTypeInc;

    /* Increase counter per packet bytes. */
    value_b_op = &action.operation_b;
    value_b_op->select = bcmFlexctrValueSelectPacketLength;
    value_b_op->type = bcmFlexctrValueOperationTypeInc;

    /* Create an ingress action */
    rv = bcm_flexctr_action_create(unit, options, &action, &stat_counter_id);
    if (BCM_FAILURE(rv)) {
       printf("bcm_flexctr_action_create() FAILED: %s\n", bcm_errmsg(rv));
       return rv;
    }
    printf("stat_counter_id == %d \r\n", stat_counter_id);

    flexctr_cfg.flexctr_action_id = stat_counter_id;
    flexctr_cfg.counter_index = 1;
    rv = bcm_field_entry_flexctr_attach(unit, eid, &flexctr_cfg);
    if (BCM_FAILURE(rv)) {
       printf("bcm_field_entry_flexctr_attach() FAILED: %s\n", bcm_errmsg(rv));
       return rv;
    }

    return BCM_E_NONE;
}

/*
 * This function is written so that hardcoding of port
 * numbers in Cint scripts is removed. This function gives
 * required number of ports
 */
bcm_error_t
portNumbersGet(int unit, int *port_list, int num_ports) {

    int i = 0, port = 0;
    bcm_error_t rv = 0;
    bcm_port_config_t configP;
    bcm_pbmp_t ports_pbmp;
    int no;

    rv = bcm_port_config_get(unit, &configP);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_port_config_get: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    ports_pbmp = configP.e;
    sal_srand(sal_time_usecs());
    /* select random ports between 0-249 */
    no = sal_rand() % 250;
    for (i = no; i < BCM_PBMP_PORT_MAX; i++) {
        if (BCM_PBMP_MEMBER(&ports_pbmp, i) && (port < num_ports)) {
            printf("port %d is selected\n", i);
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

/* Configure port to vlan as tagged or untagged */
bcm_error_t
vlan_add_port(int unit, int vid, bcm_port_t t_port, bcm_port_t ut_port) {

    bcm_pbmp_t pbmp, upbmp;
    bcm_error_t rv = BCM_E_NONE;

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, t_port);
    BCM_PBMP_PORT_ADD(upbmp, ut_port);
    rv = bcm_vlan_port_add(unit, vid, pbmp, upbmp);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_vlan_port_add: %s\n", bcm_errmsg(rv));
        return rv;
    }
    return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode
 */
bcm_error_t
port_lb_setup(int unit, bcm_pbmp_t pbm) {

    bcm_port_t     port;
    BCM_PBMP_ITER(pbm, port) {
        printf("port_lb_setup port: %d setup\n", port);
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
    }

    return BCM_E_NONE;
}

/* Setup learning on test port */
bcm_error_t
port_learn_mode_set(int unit, bcm_port_t port, uint32 flag) {

    bcm_error_t rv = BCM_E_NONE;

    rv = bcm_port_learn_set(unit, port, flag);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_port_learn_set: %s\n", bcm_errmsg(rv));
        return rv;
    }
    return BCM_E_NONE;
}

/* L2 and port configuration function */
bcm_error_t
l2Setup(int unit, bcm_vlan_t ing_vlan, bcm_vlan_t egr_vlan) {

    bcm_error_t rv = BCM_E_NONE;
    int index = 0;

    /* Create the ingress vlan */
    rv = bcm_vlan_create(unit, ing_vlan);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_vlan_create: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create the egress vlan */
    rv = bcm_vlan_create(unit, egr_vlan);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_vlan_create: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Add ingress port port_list[0] to ingress vlan */
    rv = vlan_add_port(unit, ing_vlan, port_list[0], 0);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vlan_add_port: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Add egress ports port_list[1-2] to egress vlan */
    for (index = 1; index < num_ports; index++) {
        rv |= vlan_add_port(unit, egr_vlan, port_list[index], 0);
        if (BCM_FAILURE(rv)) {
            printf("\nError in vlan_add_port: %s\n", bcm_errmsg(rv));
            return rv;
        }
    }

    /* Learn mode port config for ingress port port_list[0] */
    rv = port_learn_mode_set(unit, port_list[0], BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD);
    if (BCM_FAILURE(rv)) {
        printf("\nError in port_learn_mode_set: %s\n", bcm_errmsg(rv));
        return rv;
    }


    for (index = 1; index < num_ports; index++) {
        /* Drop egress ports port[1-2] received packet to avoid looping back */
        rv |= port_learn_mode_set(unit, port_list[index], 0);
        if (BCM_FAILURE(rv)) {
            printf("\nError in port_learn_mode_set: %s\n", bcm_errmsg(rv));
            return rv;
        }

    }

    /* Ingress sampling to CPU on redir port[2] with maximum rate */
    rv = bcm_port_sample_rate_set (unit, port_list[2], 1, 0);
    if (BCM_FAILURE(rv)) {
        printf("bcm_port_sample_rate_set() failed. %s\n", bcm_errmsg(rv));
    }

    return BCM_E_NONE;
}

/* L3 configuration function */
bcm_error_t
l3Setup(int unit) {

    bcm_error_t rv = BCM_E_NONE;
    bcm_l3_intf_t intf;
    bcm_l3_egress_t l3_egress_obj;
    bcm_l3_route_t lpm_route;
    bcm_if_t nh_index;
    bcm_l2_station_t station;
    int station_id;
    bcm_mac_t incoming_router_mac_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    /* Create L3 ingress interface */
    bcm_l3_ingress_t l3_ingress;
    bcm_l3_ingress_t_init(&l3_ingress);
    bcm_if_t ingress_if;
    l3_ingress.vrf = vrf_id;
    l3_ingress.ipmc_intf_id  = intf.l3a_intf_id;
    rv = bcm_l3_ingress_create(unit, &l3_ingress, &ingress_if);
    if (BCM_FAILURE(rv)) {
        printf("bcm_l3_ingress_create() failed. %s\n", bcm_errmsg(rv));
    }

    /* Config vlan_id to l3_iif mapping */
    bcm_vlan_control_vlan_t vlan_ctrl;
    bcm_vlan_control_vlan_t_init(&vlan_ctrl);
    bcm_vlan_control_vlan_get(unit, ingress_vlan, &vlan_ctrl);
    vlan_ctrl.ingress_if = ingress_if;
    rv = bcm_vlan_control_vlan_set(unit, ingress_vlan, vlan_ctrl);
    if (BCM_FAILURE(rv)) {
        printf("bcm_vlan_control_vlan_set() failed. %s\n", bcm_errmsg(rv));
    }

    /* Create Egress L3 Interface */
    bcm_l3_intf_t_init(&intf);
    sal_memcpy(intf.l3a_mac_addr, outgoing_router_mac, 6);
    intf.l3a_vid = egress_vlan;
    rv = bcm_l3_intf_create(unit, &intf);
    if (BCM_FAILURE(rv)) {
        printf("bcm_l3_intf_create() failed. %s\n", bcm_errmsg(rv));
    }

    /* Create Nexthop with port[1] */
    bcm_l3_egress_t_init(&l3_egress_obj);
    sal_memcpy(l3_egress_obj.mac_addr, dmac,6);
    l3_egress_obj.intf = intf.l3a_intf_id;
    l3_egress_obj.port = port_list[1];
    l3_egress_obj.mtu  = 1500;
    rv = bcm_l3_egress_create(unit, 0, &l3_egress_obj, &nh_index);
    if (BCM_FAILURE(rv)) {
        printf("bcm_l3_egress_create() failed. %s\n", bcm_errmsg(rv));
    }

    /* Create redirect Nexthop with port[2] */
    bcm_l3_egress_t_init(&l3_egr_redir);
    sal_memcpy(l3_egr_redir.mac_addr, rdmac,6);
    l3_egr_redir.intf = intf.l3a_intf_id;
    l3_egr_redir.port = port_list[2];
    l3_egr_redir.mtu  = 1500;
    rv = bcm_l3_egress_create(unit, 0, &l3_egr_redir, &nh_redir);
    if (BCM_FAILURE(rv)) {
        printf("bcm_l3_egress_create() failed. %s\n", bcm_errmsg(rv));
    }

    /* Insert LPM route */
    bcm_l3_route_t_init(&lpm_route);
    lpm_route.l3a_subnet = route; /* 192.168.1.xx */
    lpm_route.l3a_ip_mask = route_mask;
    lpm_route.l3a_intf = nh_index;
    lpm_route.l3a_vrf = vrf_id;
    rv = bcm_l3_route_add(unit, &lpm_route);
    if (BCM_FAILURE(rv)) {
        printf("bcm_l3_route_add() failed. %s\n", bcm_errmsg(rv));
    }

    /* Add l2 station entry to allow L3 forwarding */
    bcm_l2_station_t_init(&station);
    sal_memcpy(station.dst_mac_mask, incoming_router_mac_mask, 6);
    sal_memcpy(station.dst_mac, incoming_router_mac, 6);
    station.flags = BCM_L2_STATION_IPV4;
    rv = bcm_l2_station_add(unit, &station_id, &station);
    if (BCM_FAILURE(rv)) {
        printf("bcm_l2_station_add() failed. %s\n", bcm_errmsg(rv));
    }

    return BCM_E_NONE;
}

/* Rx callback function for applications using the HSDK PKTIO */
bcm_pktio_rx_t
pktioRxCallback(int unit, bcm_pktio_pkt_t * packet, void *cookie) {

    int  *count = (auto) cookie;
    int result_port_check = -1;
    int result_mac_check = -1;
    void  *buffer;
    uint32  length, src_port;

    (*count)++; /* Bump received packet count */

    /* Get source port metadata */
    BCM_IF_ERROR_RETURN(bcm_pktio_pmd_field_get
                        (unit, packet, bcmPktioPmdTypeRx,
                         BCM_PKTIO_RXPMD_SRC_PORT_NUM, &src_port));

    if(port_list[2] == src_port) {
        result_port_check = 0;
    }

    /* Get basic packet info */
    if (BCM_FAILURE
       (bcm_pktio_pkt_data_get(unit, packet, (void *) &buffer, &length))) {
       return BCM_PKTIO_RX_NOT_HANDLED;
    }

    if (0 == sal_memcmp(buffer, rdmac, 6)) {
        result_mac_check = 0;
    }

    test_failed = test_failed || (result_port_check != 0) || (result_mac_check != 0);

    return BCM_PKTIO_RX_NOT_HANDLED;
}

/* Register callback function for received packets */
bcm_error_t
registerPktioRxCallback(int unit) {

    const uint8         priority = 101;
    const uint32        flags = 0;      /* Flags only used on match (for now) */

    return bcm_pktio_rx_register
      (unit, "rx_cb", pktioRxCallback, priority, &rx_count, flags);
}

/*
 * Test Setup:
 * This functions gets the port numbers and sets up test configurations.
 */

bcm_error_t
testSetup(int unit) {

    bcm_error_t rv;

    if (BCM_E_NONE != portNumbersGet(unit, port_list, num_ports)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    bcm_pbmp_t  pbmp;
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port_list[0]);
    BCM_PBMP_PORT_ADD(pbmp, port_list[1]);
    BCM_PBMP_PORT_ADD(pbmp, port_list[2]);
    if (BCM_E_NONE != port_lb_setup(unit, pbmp)) {
        printf("port_lb_setup() failed \n");
        return -1;
    }

    bshell(unit, "pw start report all +raw +decode");

    rv = registerPktioRxCallback(unit);
    if (BCM_FAILURE(rv)) {
        printf("registerPktioRxCallback() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = l2Setup(unit, ingress_vlan, egress_vlan);
    if (BCM_FAILURE(rv)) {
        printf("l2Setup() failed. %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = l3Setup(unit);
    if (BCM_FAILURE(rv)) {
        printf("l3Setup() failed. %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Setup a IFP rule with Flex Counters verfication */
    rv = ifpPbrSetup(unit);
    if (BCM_FAILURE(rv)) {
        printf("ifpPbrSetup() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Verify function call */
bcm_error_t
testVerify(int unit) {

    char str[512];
    int rv;
    bcm_flexctr_counter_value_t counter_value;
    uint32 num_entries = 1;
    uint32 counter_index = 1;
    /* select a random packet number from 0-5 */
    sal_srand(sal_time_usecs());
    int count = sal_rand() % 5;
    if (count == 0) {
      count = 1;
    }

    bshell(unit, "fp show");
    bshell(unit, "l3 route show");
    bshell(unit, "l3 egress show");
    bshell(unit, "sleep quiet 1");
    printf("\nSending %d tagged test packets to ingress port:%d\n", (2*count)+1, port_list[0]);
    /* Send test packets with destIP C0:A8:01:14 */
    snprintf(str, 512, "tx %d pbm=%d data=0x0000000002020000000000018100000508004500002E000000004006A4F00A0A0A14C0A80114138C0569000000000000000050000000B2060000000102030405225EB0B6", count, port_list[0]);
    bshell(unit, str);
    bshell(unit, "sleep quiet 1");
    /* Send test packets with destIP C0:A8:01:05 */
    snprintf(str, 512, "tx %d pbm=%d data=0x0000000002020000000000018100000508004500002e12344000ff0693ca0a0a0a14c0a80105138c0569000000640000006450000fa0b0b60000aaaaaaaaaaaaaaaaaaaa", count+1, port_list[0]);
    bshell(unit, str);

    bshell(unit, "sleep quiet 3");
    bshell(unit, "show c");

    printf("\n------------------------------------- \n");
    printf("    RESULT OF RX PACKET VERIFICATION  ");
    printf("\n------------------------------------- \n");
    printf("Test = [%s]",((test_failed == 0 ) && ((count+1) == rx_count))?"PASS":"FAIL");
    printf("\n------------------------------------- \n");
    /* test_failed is asserted by pktioRxCallback().
       sleep command in needed to give pktioRxCallback() chance to run. */
    if (test_failed == 1) {

       return BCM_E_FAIL;
    }

    /* Get flex counter value. */
    sal_memset(&counter_value, 0, sizeof(counter_value));
    rv = bcm_flexctr_stat_get(unit,
                stat_counter_id,
                num_entries,
                &counter_index,
                &counter_value);
    if (BCM_FAILURE(rv)) {
      printf("bcm_flexctr_stat_get %s\n", bcm_errmsg(rv));
      return rv;
    }

    /* print counter value. */
    printf("FlexCtr Get : %d packets / %d bytes\n", COMPILER_64_LO(counter_value.value[0])
                                                , COMPILER_64_LO(counter_value.value[1]));
    if (COMPILER_64_LO(counter_value.value[0]) != count+1) {
      printf("Packet verify failed. Expected packet stat %d but get %d\n", count, COMPILER_64_LO(counter_value.value[0]));
      test_failed = 1;
    };

    printf("\n------------------------------------- \n");
    printf("    RESULT OF FLEX COUNTER VERIFICATION  ");
    printf("\n------------------------------------- \n");
    printf("Test = [%s]",(test_failed == 0)?"PASS":"FAIL");
    printf("\n------------------------------------- \n");

    if(test_failed ==0) {
       return BCM_E_NONE;
    }
    else {
       return BCM_E_FAIL;
    }
}


/*
 *   Cleanup test setup sequentially
 */
bcm_error_t testCleanup(int unit, int eid) {

    int rv;
    /* Remove FP entry from hardware */
    rv = bcm_field_entry_remove(unit, eid);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_entry_remove() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Detach the flex counters of the FP entry */
    rv = bcm_field_entry_flexctr_detach(unit, eid, &flexctr_cfg);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_entry_flexctr_detach() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Destroy flex counter action */
    rv = bcm_flexctr_action_destroy(unit, stat_counter_id);
    if (BCM_FAILURE(rv)) {
       printf("bcm_flexctr_action_destroy %s\n", bcm_errmsg(rv));
       return rv;
    }

    /* Destroy the test FP entry */
    rv = bcm_field_entry_destroy(unit, eid);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_entry_destroy() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Destroy the FP group */
    rv = bcm_field_group_destroy(unit, group_config.group);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_group_destroy() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}


/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in ifpPbrSetup())
 *  c) demonstrates the functionality(done in verify()).
 */
bcm_error_t
execute(void)
{
    bcm_error_t rv;
    int unit = 0;
    print "config show; attach; version";
    bshell(unit, "config show; attach ; version");

    rv = testSetup(unit);
    if (BCM_FAILURE(rv)) {
        printf("testSetup() failed %s \n", bcm_errmsg(rv));
        return rv;
    }
    printf("Completed test setup successfully.\n");

    rv = testVerify(unit);
    if (BCM_FAILURE(rv)) {
        printf("testVerify() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf("Completed test verify successfully.\n");

    rv = testCleanup(unit, entry);
    if (BCM_FAILURE(rv)) {
        printf("testCleanup() failed %s \n", bcm_errmsg(rv));
        return rv;
    }
    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
    print "execute(): Start";
    print execute();
    print "execute(): End";
}
