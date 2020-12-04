/*
 * Feature  : L3 routing with DSCP marking
 *
 * Usage    : BCM.0> cint th4_hsdk_ipv4_route_dscp_queuing_remarking.c
 *
 * config   : th4_hsdk_l3_64x400.config.yml
 *
 * Log file : th4_hsdk_ipv4_route_dscp_queuing_remarking_log.txt
 *
 * Test Topology :
 *                      +--------------------+
 *                      |                    |
 *                      |       TH4          |
 *         ingress_port |                    |  egress_port
 *        --------->----+  Routing           +------->------
 *             vlan 2   |    ALPM            |  vlan 30
 *        dst_mac-0x02  |    vrf -100        |  dst_mac-0x22
 *                      |    dip - 0xC0A80114|  src_mac-0x11
 *                      |    mask -0xFFFFFF00|  DSCP-5 (TOS-0x14)
 *                      +--------------------+
 *
 *
 * Summary:
 * ========
 * This cint script demonstrates L3 forwarding through ALPM routing in
 * parallel mode. On egress the DSCP field in the IP header is updated
 * with information from the internal priority and congestion information
 * associated with the egressing packet.
 *
 * Detailed operations performed by the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup):
 *   =============================================
 *     a) Selects two ports and configure them in Loopback mode. Out of these
 *        two ports, one port is used as ingress_port and the other as
 *        egress_port.
 *     b) Install an IFP rule to copy egress packets to CPU and also drop them.
 *        start packet watcher.
 *     c) Configure a basic IPv4 route. This adds the route in L3 ALPM table
 *        and does the necessary configurations of vlan, L3_IIF, egress intf
 *        and next hop.
 *     d) Set up a L3 egress QOS map that maps all internal priority and
 *        congestion combinations to DSCP code 5.
 *
 *     Note: An IFP rule is used to verify the script and is not part of the
 *     feature configuration.
 *
 *   2) Step2 - Verification (Done in test_verify):
 *   ==============================================
 *     a) Check the configurations using 'l3 intf show', 'l3 egress show' and
 *        'l3 route show'
 *     b) Transmit a known IPv4 unicast packet. The packet will take the
 *        route configured by the script.
 *     c) Expected Result:
 *     ===================
 *       The DMAC, SMAC, TTL and VLAN are all changed as the packet is routed
 *       through to the egress port. The TOS field in the L3 header will be
 *       set to 0x14 which corresponds to a DSCP value of 5. The egress FLEX
 *       counter is used to verify that number of L3 egress packets matches
 *       the number of packets transmitted and received.
 */

/* Reset C interpreter*/
cint_reset();

/* Test constants */
const bcm_if_t  ingress_if = 100;
const bcm_ip_t  dip = 0xC0A80114;
const bcm_ip_t  dip_mask = 0xFFFFFF00;
const bcm_mac_t dst_mac = { 0x00, 0x00, 0x00, 0x0, 0x00, 0x22 };
const bcm_mac_t router_mac = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x02 };
const bcm_mac_t src_mac = { 0x00, 0x00, 0x00, 0x0, 0x00, 0x11 };

const bcm_vlan_t egr_vid = 30;
const bcm_vlan_t ing_vid = 2;
const bcm_vrf_t vrf = 100;

/* Global script variables */
bcm_if_t        egr_if;
bcm_port_t      egress_port;
bcm_port_t      ingress_port;

int             test_failed = 0;        /*Final result will be stored here */
int             rx_count;       /* Global received packet count */
uint32_t        stat_counter_id;
uint32_t        flexctr_obj_val = 1;

/*
 * Function: check_port_assigned()
 *
 * Check if given port/index is already present in the list so that same
 * port number is not generated again
 */
int
check_port_assigned(int *port_index_list, int no_entries, int index)
{
    int             i = 0;

    for (i = 0; i < no_entries; i++) {
        if (port_index_list[i] == index)
            return 1;
    }

    /* no entry found */
    return 0;
}

/*
 * Function: port_numbers_get()
 *
 * Provide reqiested number of ports.
 */
bcm_error_t
port_numbers_get(int unit, int *port_list, int num_ports)
{
    bcm_pbmp_t      ports_pbmp;
    bcm_port_config_t configP;
    int             i = 0;
    int             index = 0;
    int             port = 0;
    int             port_index_list[num_ports];
    int             tempports[BCM_PBMP_PORT_MAX];

    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &configP));
    ports_pbmp = configP.e;
    for (i = 1; i < BCM_PBMP_PORT_MAX; i++) {
        if (BCM_PBMP_MEMBER(&ports_pbmp, i)) {
            tempports[port] = i;
            port++;
        }
    }

    if ((port == 0) || (port < num_ports)) {
        printf("port_numbers_get() failed\n");
        return BCM_E_PORT;
    }

    /* generate random ports */
    for (i = 0; i < num_ports; i++) {
        index = sal_rand() % port;
        if (check_port_assigned(port_index_list, i, index) == 0) {
            port_list[i] = tempports[index];
            port_index_list[i] = index;
        } else {
            i = i - 1;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function: ingress_port_multi_setup()
 *
 * Configure a port in loopback mode
 */
bcm_error_t
ingress_port_multi_setup(int unit, bcm_pbmp_t pbm)
{
    bcm_port_t      port;

    BCM_PBMP_ITER(pbm, port) {
        printf("Ingress_port_multi_setup port: %d setup\n", port);
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set
                            (unit, port, BCM_PORT_LOOPBACK_MAC));
    }

    return BCM_E_NONE;
}

/*
 * Function: egress_port_multi_setup()
 *
 * Configures the port in loopback mode and installs an IFP rule. This IFP
 * rule copies the packets ingressing on the specified port to CPU. It is also
 * configured to drop the packets on that port. This is to avoid continuous
 * loopback of the packet from the egress port.
 */
int
egress_port_multi_setup(int unit, bcm_pbmp_t pbm)
{
    bcm_field_entry_t entry;
    bcm_field_group_config_t group_config;
    bcm_port_t      port;

    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDrop);
    group_config.priority = 2;
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    BCM_PBMP_ITER(pbm, port) {
        printf("Egress_port_multi_setup port: %d setup\n", port);

        BCM_IF_ERROR_RETURN(bcm_port_loopback_set
                            (unit, port, BCM_PORT_LOOPBACK_MAC));

        BCM_IF_ERROR_RETURN(bcm_field_entry_create
                            (unit, group_config.group, &entry));

        BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort
                            (unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
        BCM_IF_ERROR_RETURN(bcm_field_action_add
                            (unit, entry, bcmFieldActionCopyToCpu, 0, 0));
        BCM_IF_ERROR_RETURN(bcm_field_action_add
                            (unit, entry, bcmFieldActionDrop, 0, 0));

        BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
    }

    return BCM_E_NONE;
}

/*
 * Expected Egress packet
 * Expect DSCP in byte 19 to be 0x14  (5 << 2)
 */
unsigned char   expected_egress_packet[64] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x81, 0x00, 0x00, 0x1E,
0x08, 0x00, 0x45, 0x14, 0x00, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0xA4, 0xD1, 0x0A, 0x0A,
0x0A, 0x2A, 0xC0, 0xA8, 0x01, 0x14, 0x00, 0x01, 0x02, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x4D, 0x7B, 0xD6
};

/*
 * Function: pktio_Rx_call_back()
 *
 * Rx callback function for applications using the HSDK PKTIO.
 */
bcm_pktio_rx_t
pktio_Rx_call_back(int unit, bcm_pktio_pkt_t * packet, void *cookie)
{
    int            *count = (auto) cookie;
    int             result_packet_check = 0;
    int             result_port_check = 0;
    uint8          *buffer;
    uint32          length;
    uint32          src_port;
    int             idx;

    (*count)++; /* Bump received packet count */
    printf("Checking Rx Packet: %d\n", *count);

    /* Get basic packet info */
    if (BCM_FAILURE
        (bcm_pktio_pkt_data_get(unit, packet, (auto) & buffer, &length))) {

        return BCM_PKTIO_RX_NOT_HANDLED;
    }

    /* Get source port metadata */
    BCM_IF_ERROR_RETURN(bcm_pktio_pmd_field_get
                        (unit, packet, bcmPktioPmdTypeRx,
                         BCM_PKTIO_RXPMD_SRC_PORT_NUM, &src_port));

    for (idx = 0; idx < length; idx++) {
        if (buffer[idx] != expected_egress_packet[idx]) {
            printf("%3d; Expected: 0x%02X; Actual: 0x%02X\n",
                   idx, expected_egress_packet[idx], buffer[idx]);
            result_packet_check++;
        }
    }

    if (egress_port != src_port) {
        printf("Expected port: %d; Actual port: %d\n", egress_port, src_port);
        result_port_check++;
    }

    test_failed = test_failed || (result_packet_check != 0)
      || (result_port_check != 0);

    return BCM_PKTIO_RX_NOT_HANDLED;
}

/*
 * Function: register_pktioRx_callback()
 *
 * Register callback function for received packets.
 */
bcm_error_t
register_pktioRx_callback(int unit)
{
    const uint8     priority = 101;
    const uint32    flags = 0;  /* Flags only used on match (for now) */

    return bcm_pktio_rx_register
      (unit, "rx_cb", pktio_Rx_call_back, priority, &rx_count, flags);
}

/*
 * Function: create_vlan_add_port()
 *
 * Create VLAN and add port to VLAN.
 */
bcm_error_t
create_vlan_add_port(int unit, bcm_vlan_t vlan, bcm_port_t port)
{
    bcm_error_t     rv;
    bcm_pbmp_t      pbmp, ubmp;

    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);

    rv = bcm_vlan_create(unit, vlan);
    if ((BCM_FAILURE(rv)) & (rv != BCM_E_EXISTS)) {
        printf("Error creating VLAN %d: %s\n", vlan, bcm_errmsg(rv));
        return rv;
    }

    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vlan, pbmp, ubmp));

    return BCM_E_NONE;
}

/*
 * Function: create_l3_iif()
 *
 * Create L3 Ingress Interface.
 */
bcm_error_t
create_l3_iif(int unit, bcm_vrf_t vrf, bcm_if_t ingress_if)
{
    bcm_error_t     rv;
    bcm_l3_ingress_t l3_ingress;

    bcm_l3_ingress_t_init(&l3_ingress);
    l3_ingress.flags = BCM_L3_INGRESS_WITH_ID;
    l3_ingress.vrf = vrf;

    rv = bcm_l3_ingress_create(unit, &l3_ingress, &ingress_if);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring L3_IIF %s.\n", bcm_errmsg(rv));
        return rv;
    }
    return BCM_E_NONE;
}

/*
 * Functon:
 *
 * Bind VLAN to L3_IIF mapping,
 */
bcm_error_t
configure_l3_iif_binding(int unit, bcm_vlan_t vlan, bcm_if_t ingress_if)
{
    bcm_error_t     rv;
    bcm_vlan_control_vlan_t vlan_ctrl;

    bcm_vlan_control_vlan_t_init(&vlan_ctrl);
    BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_get(unit, vlan, &vlan_ctrl));
    vlan_ctrl.ingress_if = ingress_if;

    rv = bcm_vlan_control_vlan_set(unit, vlan, vlan_ctrl);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring L3_IIF binding %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function: createMap()
 *
 * Create an L3/IPv4 egress map. On egress, this function maps all combinatons
 * of internal priority and congestion to a single DSCP code. This routine
 * can be modified to acheive any desired mapping to one or more of the 64
 * possible DSCP codes.
 */
bcm_error_t
createMap(int unit, int *mapId, int dscp)
{
    const int       createFlags =
      BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_EGRESS;
    int             rCos;

    BCM_IF_ERROR_RETURN(bcm_qos_map_create(unit, createFlags, mapId));

    /* Map all COS/CNG settings to DSCP code */
    for (rCos = 0; rCos < 16; rCos++) {
        int             rColor;
        for (rColor = 0; rColor < 2; rColor++) {
            bcm_qos_map_t   qosMap;
            bcm_qos_map_t_init(&qosMap);

            qosMap.dscp = dscp; /* Target DSCP code */

            qosMap.int_pri = rCos;      /* Source internal priority */

            /* Source congestion */
            if (rColor == 0) {
                qosMap.color = bcmColorGreen;
            } else {
                qosMap.color = bcmColorYellow;
            }

            BCM_IF_ERROR_RETURN(bcm_qos_map_add
                                (unit, createFlags, &qosMap, *mapId));
        }
    }
    return BCM_E_NONE;
}

/*
 * Function: create_l3_intf()
 *
 * Create L3 interface. Set up DSCP egress mapping.
 */
bcm_error_t
create_l3_intf(int unit, bcm_mac_t mac, bcm_vlan_t vlan, bcm_if_t * l3_intf_id)
{
    const int       DSCP1 = 5;
    int             dfltMapId;

    bcm_error_t     rv;
    bcm_l3_intf_t   l3_intf;

    BCM_IF_ERROR_RETURN(createMap(unit, &dfltMapId, DSCP1));

    bcm_l3_intf_t_init(&l3_intf);
    sal_memcpy(l3_intf.l3a_mac_addr, mac, sizeof(mac));
    l3_intf.l3a_vid = vlan;
    l3_intf.l3a_mtu = 1500;
    l3_intf.l3a_vrf = 0;
    l3_intf.dscp_qos.flags |= BCM_L3_INTF_QOS_DSCP_REMARK;
    l3_intf.dscp_qos.qos_map_id = dfltMapId;

    rv = bcm_l3_intf_create(unit, &l3_intf);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring L3 interface: %s\n", bcm_errmsg(rv));
        return rv;
    }
    *l3_intf_id = l3_intf.l3a_intf_id;

    return BCM_E_NONE;
}

/*
 * Function: create_egr_obj()
 *
 * Create an L3 egress object.
 */

bcm_error_t
create_egr_obj(int unit, bcm_if_t l3_if, bcm_mac_t mac, bcm_gport_t gport)
{
    bcm_error_t     rv;
    bcm_l3_egress_t l3_egr;

    bcm_l3_egress_t_init(&l3_egr);
    l3_egr.intf = l3_if;
    l3_egr.port = gport;
    sal_memcpy(l3_egr.mac_addr, mac, sizeof(mac));

    rv = bcm_l3_egress_create(unit, 0, &l3_egr, &egr_if);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring L3 egress object: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function: configure_my_station_tcam()
 *
 * Configure L2 station forwarding
 */

bcm_error_t
configure_my_station_tcam(int unit, bcm_mac_t mac)
{
    bcm_error_t     rv;
    bcm_mac_t       mac_mask = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    bcm_l2_station_t l2_station;
    int             station_id;

    bcm_l2_station_t_init(&l2_station);
    sal_memcpy(l2_station.dst_mac, mac, sizeof(mac));
    sal_memcpy(l2_station.dst_mac_mask, mac_mask, sizeof(mac_mask));
    l2_station.flags =
      BCM_L2_STATION_IPV4 | BCM_L2_STATION_IPV6 | BCM_L2_STATION_UNDERLAY;

    rv = bcm_l2_station_add(unit, &station_id, &l2_station);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring L2 station %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function: configure_route()
 *
 * Configure route, adding subnet address to ALPM table.
 */
bcm_error_t
configure_route(int unit, bcm_ip_t subnet_addr, bcm_ip_t subnet_mask,
                bcm_if_t egr_if, bcm_vrf_t vrf)
{
    bcm_error_t     rv;
    bcm_l3_route_t  route;

    bcm_l3_route_t_init(&route);
    route.l3a_subnet = subnet_addr;
    route.l3a_ip_mask = subnet_mask;
    route.l3a_intf = egr_if;
    route.l3a_vrf = vrf;
    rv = bcm_l3_route_add(unit, &route);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring L3 ALPM table %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function: configure_l3_routing
 *
 * Main L3 route configuration.
 */
bcm_error_t
configure_l3_routing(int unit)
{
    bcm_error_t     rv = BCM_E_NONE;

    bcm_if_t        egr_l3_if;
    bcm_gport_t     egress_gport;

    BCM_IF_ERROR_RETURN(bcm_port_control_set
                        (unit, ingress_port, bcmPortControlIP4, 1));

    /* Create vlan and add ports to it */
    rv = create_vlan_add_port(unit, ing_vid, ingress_port);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring ing vlan: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = create_vlan_add_port(unit, egr_vid, egress_port);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring egr vlan: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Set the default VLAN for the port */
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set
                        (unit, ingress_port, ing_vid));

    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port, egr_vid));

    /* Create L3 ingress interface - l3_iif */
    rv = create_l3_iif(unit, vrf, ingress_if);
    if (BCM_FAILURE(rv)) {
        printf("Error in create_l3_iif: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* VLAN to L3_IIF mapping */
    rv = configure_l3_iif_binding(unit, ing_vid, ingress_if);
    if (BCM_FAILURE(rv)) {
        printf("Error in configure_l3_iif_binding: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create L3 egress interface */
    rv = create_l3_intf(unit, src_mac, egr_vid, &egr_l3_if);
    if (BCM_FAILURE(rv)) {
        printf("Error in create_l3_intf: %s\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, egress_port, &egress_gport));

    /* Create L3 egress object */
    rv = create_egr_obj(unit, egr_l3_if, dst_mac, egress_gport);
    if (BCM_FAILURE(rv)) {
        printf("Error in create_egr_obj: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Configure MY_STATION TCAM for router MAC */
    rv = configure_my_station_tcam(unit, router_mac);
    if (BCM_FAILURE(rv)) {
        printf("Error in configure_my_station_tcam: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Program ROUTE in ALPM table */
    rv = configure_route(unit, dip, dip_mask, egr_if, vrf);
    if (BCM_FAILURE(rv)) {
        printf("Error in configure_route: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function: configure_flex_counter()
 *
 * Create flex counter action with L3 Route table as source and user set index.
 */
bcm_error_t
configure_flex_counter(int unit)
{
    const int       options = 0;

    bcm_error_t     rv;
    bcm_flexctr_action_t action;
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_a_op = NULL;
    bcm_flexctr_value_operation_t *value_b_op = NULL;

    bcm_flexctr_action_t_init(&action);
    action.flags = 0;
    action.source = bcmFlexctrSourceL3Route;
    action.mode = bcmFlexctrCounterModeNormal;
    action.index_num = 100;     /* Number of required counters */

    index_op = &action.index_operation;
    index_op->object[0] = bcmFlexctrObjectStaticIngAlpmDstLookup;
    index_op->mask_size[0] = 7;
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
        printf("Error in bcm_flexctr_action_create: %s\n", bcm_errmsg(rv));
        return rv;
    }

    bcm_l3_route_t  route;

    bcm_l3_route_t_init(&route);
    route.l3a_subnet = dip;
    route.l3a_ip_mask = dip_mask;
    route.l3a_intf = egr_if;
    route.l3a_vrf = vrf;

    /* Attach counter action to route */
    rv = bcm_l3_route_stat_attach(unit, &route, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_l3_route_stat_attach: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Set the counter index */
    rv = bcm_l3_route_flexctr_object_set(unit, &route, flexctr_obj_val);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_l3_route_flexctr_object_set: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function test_setup()
 *
 * Main configuration function. Configures the IPV4 unicast route and the
 * required set up to verify correct behavior.
 */
bcm_error_t
test_setup(int unit)
{
    const int       num_ports = 2;
    int             port_list[num_ports];
    bcm_error_t     rv;
    bcm_pbmp_t      pbmp;

    rv = port_numbers_get(unit, port_list, num_ports);
    if (BCM_FAILURE(rv)) {
        printf("Error in port_numbers_get: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Assign globals */
    ingress_port = port_list[0];
    egress_port = port_list[1];

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port);
    rv = ingress_port_multi_setup(unit, pbmp);
    if (BCM_FAILURE(rv)) {
        printf("Error in ingress_port_multi_setup: %s\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, egress_port);
    rv = egress_port_multi_setup(unit, pbmp);
    if (BCM_FAILURE(rv)) {
        printf("Error in egress_port_multi_setup: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = register_pktioRx_callback(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in register_pktioRx_callback: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = configure_l3_routing(unit);
    if (BCM_FAILURE(rv)) {
        printf("error in configure_l3_routing: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = configure_flex_counter(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in configure_flex_counter: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Test verification
 *
 * Send the following packet:
 * 0000 0000 0002 0000 0000 0001 8100 0002
 * 0800 4500 002A 0000 0000 40FF A3E5 0A0A
 * 0A2A C0A8 0114 0001 0203 0000 0000 0000
 * 0000 0000 0000 0000 0000 0000 024D 7BD6 
 *
 * Verify that the expected L2 header changes are performed, including
 * DSCP updates.  Verity that the expected number of packets are returned.
 */
bcm_error_t
test_verify(int unit)
{
    const int       count = (sal_rand() % 9) + 1;
    const uint32    num_entries = 1;

    bcm_flexctr_counter_value_t counter_value;
    char            str[180];
    int             rv;
    uint32          counter_index = flexctr_obj_val;

    print "l3 route show";
    bshell(unit, "l3 route show");
    printf("\n");
    print "l3 intf show";
    bshell(unit, "l3 intf show");
    print "l3 egress show";
    bshell(unit, "l3 egress show");

    printf("Sending %d unicast packets to ingress port %d\n", count,
           ingress_port);
    snprintf(str, 180,
             "tx %d pbm=%d data=0x0000000000020000000000018100000208004500002A0000000040FFA3E50A0A0A2AC0A8011400010203000000000000000000000000000000000000024D7BD6",
             count, ingress_port);
    bshell(unit, str);
    sal_sleep(3);

    printf("------------------------------------- \n");
    printf("    RESULT OF PACKET VERIFICATION\n");
    printf("------------------------------------- \n");
    printf("Test = [%s]\n", (test_failed == 0) ? "PASS" : "FAIL");
    printf("------------------------------------- \n\n");

    /*
     * test_fail is asserted by pktio_Rx_call_back. sleep command in needed
     * to give pktio_Rx_call_back chance to run.
 */
    if (test_failed) {
        return BCM_E_FAIL;
    }

    /* Get Flex counter value. */
    sal_memset(&counter_value, 0, sizeof(counter_value));
    rv =
      bcm_flexctr_stat_get(unit, stat_counter_id, num_entries, &counter_index,
                           &counter_value);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_flexctr_stat_get: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* print counter value. */
    printf("FLEX COUNTER: Routed %d packets/%3d bytes\n",
           COMPILER_64_LO(counter_value.value[0]),
           COMPILER_64_LO(counter_value.value[1]));
    if (COMPILER_64_LO(counter_value.value[0]) != count) {
        printf
          ("Error in packet verification. Expected packet stat %d but got %d\n",
           count, COMPILER_64_LO(counter_value.value[0]));
        test_failed = 1;
    }

    printf("-------------------------------------------- \n");
    printf("    RESULT OF FLEX COUNTER VERIFICATION\n");
    printf("-------------------------------------------- \n");
    printf("Test = [%s]\n", (test_failed == 0) ? "PASS" : "FAIL");
    printf("-------------------------------------------- \n\n");

    if (rx_count != count) {
        printf
          ("Error in packet verification. Expected to receive %d packets but got %d\n",
           count, rx_count);
        test_failed = 1;
    }
    printf("-------------------------------------------- \n");
    printf("    RESULT OF RX CALLBACK VERIFICATION\n");
    printf("-------------------------------------------- \n");
    printf("Test = [%s]\n", (test_failed == 0) ? "PASS" : "FAIL");
    printf("-------------------------------------------- \n\n");
    return test_failed ? BCM_E_FAIL : BCM_E_NONE;
}

/*
 * Function: execute()
 *
 * Main entry point.
 *
 * This functions does the following:
 *   a) Test setup [configures all features]
 *   b) Test verify [verifies configured features]
 */
bcm_error_t
execute()
{
    bcm_error_t     rv;
    int             unit = 0;

    printf("attach; version\n");
    bshell(unit, "attach ; version");
    printf("\n");

    rv = test_setup(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in test_setup: %s \n", bcm_errmsg(rv));
        return rv;
    }
    printf("Completed test setup successfully.\n\n");

    rv = test_verify(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in test_verify: %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf("Completed test verification successfully.\n");

    return BCM_E_NONE;
}

const char     *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
    print           execute();
}
