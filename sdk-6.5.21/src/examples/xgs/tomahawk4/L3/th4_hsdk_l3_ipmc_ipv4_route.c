/*
 * Feature  : L3 IPMC routing
 *
 * Usage    : BCM.0> cint th4_hsdk_l3_ipmc_ipv4_route.c
 *
 * config   : th4_hsdk_l3_64x400.config.yml
 *
 * Log file : th4_hsdk_l3_ipmc_ipv4_route_log.txt
 *
 * Test Topology :
 *                      +--------------------+
 *                      |                    |
 *                      |       TH4          |
 *         ingress_port |                    |  egress_port
 *        --------->----+  Routing           +------->-------------
 *             vlan 2   |    ALPM            |  vlan 30,40,50
 * dst_mac-0x1005E010101|    vrf -100        |  dst_mac-0x01005E010101
 *                      |    dip - 0xE4010101|  src_mac-0x11, 0x1111, 0x111111
 *                      |    mask -0xFFFFFF00|
 *                      +--------------------+
 *
 *
 * Summary:
 * ========
 * cint script to demonstrate L3 IPMC forwarding.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup):
 *   =============================================
 *     a) Selects four ports [3 ports are chosen for replication in this example
 * 		  and can be any number] and configure them in loopback mode. Out of
 *        these four ports, one port is used as ingress_port and the other as
 *        egress_ports.
 *     b) Install an IFP rule to copy egress packets to CPU and also drop them.
 *        start packet watcher.
 *     c) Configure a IPv4 IPMC route. This adds the route in L3 ALPM table
 *        and does the necessary configurations of vlan, L3_IIF, egress intf
 *        and next hops.
 *
 *     Note: IFP rule is meant for a verification purpose and is not part of the
 * 			 feature configuration.
 *
 *   2) Step2 - Verification (Done in test_verify):
 *   ==============================================
 *     a) Check the configurations by 'l3 intf show', 'l3 egress show' and
 *        'l3 route show', 'multicast show and' and 'ipmc table show'
 *     b) Transmit the IPv4 multicast packet. The packet used matches the ipmc
 *        route configured through script.
 *     c) Expected Result:
 *     ===================
 *       We see that smac, TTL and vlan are all changed as the packet gets
 *       routed through egress ports and the number of sent packets should
 *       match the flex ctr stat get output
 */

/* Reset C interpreter*/
cint_reset();

int num_ports = 3;
bcm_port_t egress_port[num_ports];
bcm_port_t ingress_port;
bcm_vlan_t ing_vid = 5;
bcm_vlan_t egr_vid[num_ports] = { 30, 40, 50};
bcm_mac_t src_mac[num_ports] = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x11},
                                {0x00, 0x00, 0x00, 0x00, 0x11, 0x11},
                                {0x00, 0x00, 0x00, 0x11, 0x11, 0x11}};
bcm_mac_t router_mac = {0x01, 0x00, 0x5E, 0x01, 0x01, 0x01};
bcm_if_t ingress_if = 100;
bcm_vrf_t vrf = 100;
bcm_ip_t sip = 0x02010101;
bcm_ip_t sip_mask = 0xFFFFFFFF;
bcm_ip_t mc = 0xE4010101;
bcm_ip_t mc_mask = 0xFFFFFF00;
bcm_multicast_t multicast_group;
uint32_t stat_counter_id, flexctr_obj_val = 1;

/*
 *    Checks if given port/index is already present
 *    in the list so that same port number is not generated again
 */

int
check_port_assigned(int *port_index_list,int no_entries, int index)
{
    int i=0;

    for (i= 0; i < no_entries; i++) {
        if (port_index_list[i] == index)
        return 1;
    }

    /* no entry found */
    return 0;
}

/*
 *    Provides required number of ports
 */

int
port_numbers_get(int unit, int *port_list, int num_ports)
{
    int i = 0, port = 0, index = 0;
    bcm_port_config_t configP;
    bcm_pbmp_t ports_pbmp;
    int tempports[BCM_PBMP_PORT_MAX];
    int port_index_list[num_ports];

    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &configP));

    ports_pbmp = configP.e;
    for (i = 1; i < BCM_PBMP_PORT_MAX; i++) {
        if (BCM_PBMP_MEMBER(&ports_pbmp, i)) {
            tempports[port] = i;
            port++;
        }
    }

    if (( port == 0 ) || ( port < num_ports )) {
        printf("port_numbers_get() failed \n");
        return -1;
    }

    /* generating random ports */
    for (i= 0; i < num_ports; i++) {
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
 *    Configures the port in loopback mode
 */

int
ingress_port_multi_setup(int unit, bcm_pbmp_t pbm)
{
    bcm_port_t     port;

    BCM_PBMP_ITER(pbm, port) {
        printf("Ingress_port_multi_setup port: %d setup\n", port);
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
    }

    return BCM_E_NONE;
}

/*
 *    Configures the port in loopback mode and installs an IFP rule. This IFP
 *    rule copies the packets ingressing on the specified port to CPU. It is
 *    also configured to drop the packets on that port. This is to avoid
 *    continuous loopback of the packet from the egress port.
 */

int
egress_port_multi_setup(int unit, bcm_pbmp_t pbm)
{
    bcm_field_group_t group;
    bcm_field_entry_t entry;
    bcm_port_t port;

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
        printf("Egress_port_multi_setup port: %d setup\n", port);

        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

        BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));

        BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));

        BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
    }

    return BCM_E_NONE;
}

int test_failed = 0; /*Final result will be stored here */
int rx_count;   /* Global received packet count */

/*    Expected Egress packets     */

unsigned char expected_egress_packet_1[68] = {
        0x01, 0x00, 0x5E, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x81, 0x00, 0x00, 0x1E,
        0x08, 0x00, 0x45, 0x00, 0x00, 0x2E, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0x92, 0xCD, 0x02, 0x01,
        0x01, 0x01, 0xE4, 0x01, 0x01, 0x01, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
        0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
	0xBF, 0x2E, 0x44, 0xC3
};

unsigned char expected_egress_packet_2[68] = {
        0x01, 0x00, 0x5E, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x81, 0x00, 0x00, 0x28,
        0x08, 0x00, 0x45, 0x00, 0x00, 0x2E, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0x92, 0xCD, 0x02, 0x01,
        0x01, 0x01, 0xE4, 0x01, 0x01, 0x01, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
        0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
	0xBF, 0x2E, 0x44, 0xC3
};

unsigned char expected_egress_packet_3[68] = {
        0x01, 0x00, 0x5E, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x81, 0x00, 0x00, 0x32,
        0x08, 0x00, 0x45, 0x00, 0x00, 0x2E, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0x92, 0xCD, 0x02, 0x01,
        0x01, 0x01, 0xE4, 0x01, 0x01, 0x01, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
        0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
	0xBF, 0x2E, 0x44, 0xC3
};

/*    Rx callback function for applications using the HSDK PKTIO    */

bcm_pktio_rx_t
pktio_Rx_call_back(int unit, bcm_pktio_pkt_t * packet, void *cookie)
{
    int *count = (auto) cookie;
    int result_packet_check = -1;
    int result_port_check = -1;
    int result_vlan_check = -1;
    void *buffer;
    uint32 length;
    uint32 src_port;

    (*count)++; /* Bump received packet count */

    /* Get basic packet info */
    if (BCM_FAILURE
        (bcm_pktio_pkt_data_get(unit, packet, (void *) &buffer, &length))) {

        return BCM_PKTIO_RX_NOT_HANDLED;
    }

    /* Get source port metadata */
    BCM_IF_ERROR_RETURN(bcm_pktio_pmd_field_get
                        (unit, packet, bcmPktioPmdTypeRx,
                         BCM_PKTIO_RXPMD_SRC_PORT_NUM, &src_port));

    if ((sal_memcmp(buffer, expected_egress_packet_1, length) == 0) ||
	(sal_memcmp(buffer, expected_egress_packet_2, length) == 0) ||
	(sal_memcmp(buffer, expected_egress_packet_3, length) == 0)) {
            result_packet_check = 0;
	}

    if((egress_port[0] == src_port) || (egress_port[1] == src_port) || (egress_port[2] == src_port))  {
        result_port_check = 0;
    }

    test_failed = test_failed || (result_packet_check != 0) || (result_port_check != 0);

    return BCM_PKTIO_RX_NOT_HANDLED;
}

/*    Register callback function for received packets    */

int
register_pktioRx_callback(int unit)
{
    const uint8 priority = 101;
    const uint32 flags = 0;      /* Flags only used on match (for now) */

    return bcm_pktio_rx_register
      (unit, "rx_cb", pktio_Rx_call_back, priority, &rx_count, flags);
}

/*    Create vlan and add port to vlan    */

int
create_vlan_add_port(int unit, bcm_vlan_t vlan, bcm_port_t port)
{
    bcm_error_t rv;
    bcm_pbmp_t pbmp, ubmp;

    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);

    rv = bcm_vlan_create(unit, vlan);
    if ((BCM_FAILURE(rv)) & (rv != BCM_E_EXISTS)) {
        printf("Error in creating vlan %s.\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vlan, pbmp, ubmp));

    return BCM_E_NONE;
}

/*    Create L3 Ingress Interface    */

int
create_l3_iif(int unit, bcm_vrf_t vrf, bcm_if_t ingress_if)
{
    bcm_error_t rv;
    bcm_l3_ingress_t l3_ingress;

    bcm_l3_ingress_t_init(&l3_ingress);
    l3_ingress.flags = BCM_L3_INGRESS_WITH_ID;
    l3_ingress.ipmc_intf_id  = ingress_if;
    l3_ingress.vrf = vrf;

    rv = bcm_l3_ingress_create(unit, &l3_ingress, &ingress_if);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring L3_IIF %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*    Config vlan_id to l3_iif mapping    */

int
configure_l3_iif_binding(int unit, bcm_vlan_t vlan, bcm_if_t ingress_if)
{
    bcm_error_t rv;
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

/*    Create L3 interface    */

int
create_l3_intf(int unit, bcm_mac_t mac, bcm_vlan_t vlan, bcm_if_t *l3_intf_id)
{
    bcm_error_t rv;
    bcm_l3_intf_t l3_intf;

    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_vid = vlan;
    sal_memcpy(l3_intf.l3a_mac_addr, mac, sizeof(mac));

    rv = bcm_l3_intf_create(unit, &l3_intf);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring L3 interface %s.\n", bcm_errmsg(rv));
        return rv;
    }
    *l3_intf_id = l3_intf.l3a_intf_id;

    return BCM_E_NONE;
}

/*    Create L3 egress object    */

int
create_egr_obj(int unit, bcm_if_t l3_if, bcm_gport_t gport, bcm_if_t *egr_obj)
{
    bcm_error_t rv;
    bcm_l3_egress_t l3_egr;
    uint32 flags;

    bcm_l3_egress_t_init(&l3_egr);
    l3_egr.intf = l3_if;
    flags = BCM_L3_IPMC | BCM_L3_KEEP_DSTMAC;

    rv = bcm_l3_egress_create(unit, flags, &l3_egr, egr_obj);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring L3 egress object %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*	  Configure multicast	*/

int
configure_multicast(int unit, bcm_if_t egr_obj, bcm_gport_t gport)
{
    bcm_error_t rv;
    bcm_if_t encap_id;

    rv = bcm_multicast_egress_object_encap_get(unit, multicast_group, egr_obj, &encap_id);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_multicast_egress_object_encap_get %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_multicast_egress_add(unit, multicast_group, gport, encap_id);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_multicast_egress_add %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*    Configure my station    */

int
configure_my_station_tcam(int unit, bcm_mac_t mac)
{
    bcm_error_t rv;
    bcm_mac_t mac_mask={0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    bcm_l2_station_t l2_station;
    int station_id;

    bcm_l2_station_t_init(&l2_station);
    sal_memcpy(l2_station.dst_mac, mac, sizeof(mac));
    sal_memcpy(l2_station.dst_mac_mask, mac_mask, sizeof(mac_mask));
    l2_station.flags = BCM_L2_STATION_IPV4 | BCM_L2_STATION_IPV6 | BCM_L2_STATION_UNDERLAY;

    rv = bcm_l2_station_add(unit, &station_id, &l2_station);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring L2 station %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*    Adding ipmc address to ALPM table    */

int
ipmc_route_add(int unit, bcm_ip_t sip, bcm_ip_t sip_mask, bcm_ip_t mc,
                bcm_ip_t mc_mask, bcm_if_t ing_if, bcm_vrf_t vrf)
{
    bcm_error_t  rv;
    bcm_ipmc_addr_t ipmc_addr;

    bcm_ipmc_addr_t_init(&ipmc_addr);
    ipmc_addr.s_ip_addr = sip;
    ipmc_addr.s_ip_mask = sip_mask;
    ipmc_addr.mc_ip_addr = mc;
    ipmc_addr.mc_ip_mask = mc_mask;
    ipmc_addr.ing_intf = ing_if;
    ipmc_addr.vrf = vrf;
    ipmc_addr.group = multicast_group;
    ipmc_addr.flags = BCM_IPMC_SOURCE_PORT_NOCHECK;

    rv = bcm_ipmc_add(unit, &ipmc_addr);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_ipmc_add %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*    L3 IPMC configuration function    */

int
configure_ipmc_routing(int unit)
{
    bcm_error_t rv = BCM_E_NONE;

    bcm_if_t egr_l3_if[num_ports];
    bcm_gport_t egress_gport[num_ports];
    bcm_if_t egr_obj[num_ports];
    int i;

    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, ingress_port, bcmPortControlIP4, 1));

    /* Create vlan and add ports to it */
    rv = create_vlan_add_port(unit, ing_vid, ingress_port);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring ing vlan %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Set the default VLAN for the port */
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, ingress_port, ing_vid));

    /* Create L3 ingress interface - l3_iif */
    rv = create_l3_iif(unit, vrf, ingress_if);
    if (BCM_FAILURE(rv)) {
        printf("Error in create_l3_iif %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* VLAN to L3_IIF mapping */
    rv = configure_l3_iif_binding(unit, ing_vid, ingress_if);
    if (BCM_FAILURE(rv)) {
        printf("Error in configure_l3_iif_binding %s.\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_IF_ERROR_RETURN(bcm_multicast_create(unit, BCM_MULTICAST_TYPE_L3, &multicast_group));

    for (i=0; i<num_ports; i++) {
        /* Create vlan and add ports to it */
        rv = create_vlan_add_port(unit, egr_vid[i], egress_port[i]);
        if (BCM_FAILURE(rv)) {
            printf("Error in configuring egr vlan %s\n", bcm_errmsg(rv));
            return rv;
        }

        /* Set the default VLAN for the port */
        BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port[i], egr_vid[i]));

        /* Create L3 egress interface */
        rv = create_l3_intf(unit, src_mac[i], egr_vid[i], &egr_l3_if[i]);
        if (BCM_FAILURE(rv)) {
            printf("Error in create_l3_intf %s.\n", bcm_errmsg(rv));
            return rv;
        }

        BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, egress_port[i], &egress_gport[i]));

        /* Create L3 egress object */
        rv = create_egr_obj(unit, egr_l3_if[i], egress_gport[i], egr_obj[i]);
        if (BCM_FAILURE(rv)) {
            printf("Error in create_egr_obj %s.\n", bcm_errmsg(rv));
            return rv;
        }

        /* Configure multicast */
        rv = configure_multicast(unit, egr_obj[i], egress_gport[i]);
        if (BCM_FAILURE(rv)) {
           printf("Error in configure_multicast %s.\n", bcm_errmsg(rv));
           return rv;
        }
   }

    /* Configure MY_STATION TCAM for router MAC */
    rv = configure_my_station_tcam(unit, router_mac);
    if (BCM_FAILURE(rv)) {
        printf("Error in configure_my_station_tcam %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Program ROUTE in ALPM table */
    rv = ipmc_route_add(unit, sip, sip_mask, mc, mc_mask, ingress_if, vrf);
    if (BCM_FAILURE(rv)) {
        printf("Error in ipmc_route_add %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Create flex counter action with L3 IPMC Route table as source and user set index */

int
configure_flex_counter(int unit)
{
    int options = 0;
    bcm_error_t  rv;
    bcm_flexctr_action_t action;
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_a_op = NULL;
    bcm_flexctr_value_operation_t *value_b_op = NULL;

    print bcm_flexctr_action_t_init(&action);
    action.flags = 0;
    action.source = bcmFlexctrSourceIpmc;
    action.mode = bcmFlexctrCounterModeNormal;
    action.index_num = 100; /* Number of required counters */

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
        printf("Error in bcm_flexctr_action_create %s\n", bcm_errmsg(rv));
        return rv;
    }

    bcm_ipmc_addr_t ipmc_addr;

    bcm_ipmc_addr_t_init(&ipmc_addr);
    ipmc_addr.s_ip_addr = sip;
    ipmc_addr.s_ip_mask = sip_mask;
    ipmc_addr.mc_ip_addr = mc;
    ipmc_addr.mc_ip_mask = mc_mask;
    ipmc_addr.ing_intf = ingress_if;
    ipmc_addr.vrf = vrf;
    ipmc_addr.group = multicast_group;

    /* Attach counter action to route */
    rv = bcm_ipmc_stat_attach(unit, &ipmc_addr, stat_counter_id);
    if (BCM_FAILURE (rv)) {
        printf ("Error in bcm_l3_route_stat_attach %s\n", bcm_errmsg (rv));
        return rv;
    }

    /* Set the counter index */
    rv = bcm_ipmc_flexctr_object_set(unit, &ipmc_addr, flexctr_obj_val);
    if (BCM_FAILURE (rv)) {
        printf ("Error in bcm_l3_route_flexctr_object_set %s\n", bcm_errmsg (rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 *    Configures the IPV4 IPMC route and the required set up to verify it
 */

int
test_setup(int unit)
{
    int num_prts = num_ports+1;
    int port_list[num_prts];
    bcm_error_t rv;
    bcm_pbmp_t pbmp;
    int i;

    rv = port_numbers_get(unit, port_list, num_prts);
    if (BCM_FAILURE(rv)) {
        printf("Error in port_numbers_get %s\n", bcm_errmsg(rv));
        return rv;
    }

    ingress_port = port_list[0];
    for (i=1; i<num_prts; i++)
	egress_port [i-1] = port_list[i];

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port);
    rv = ingress_port_multi_setup(unit, pbmp);
    if (BCM_FAILURE(rv)) {
        printf("Error in ingress_port_multi_setup %s\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_PBMP_CLEAR(pbmp);
    for (i=0; i<num_ports; i++)
	BCM_PBMP_PORT_ADD(pbmp, egress_port[i]);
    rv = egress_port_multi_setup(unit, pbmp);
    if (BCM_FAILURE(rv)) {
        printf("Error in egress_port_multi_setup %s\n", bcm_errmsg(rv));
        return rv;
    }

    bshell(unit, "pw start report +raw +decode");

    rv = register_pktioRx_callback(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in register_pktioRx_callback %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = configure_ipmc_routing(unit);
    if (BCM_FAILURE(rv)) {
        printf("error in configure_ipmc_routing %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = configure_flex_counter(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in configure_flex_counter %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Flex counter verification */

int
test_verify(int unit)
{
    char str[512];
    int rv;
    bcm_flexctr_counter_value_t counter_value;
    uint32 num_entries = 1;
    uint32 counter_index = flexctr_obj_val;
    int count = sal_rand() % 10;
    if (count == 0) {
      count = 1;
    }

    print "l3 ing_intf show";
    bshell(unit, "l3 ing_intf show");
    print "multicast show";
    bshell(unit, "multicast show");
    printf("\n");
    print "ipmc table show";
    bshell(unit, "ipmc table show");
    printf("\n");
    print "l3 intf show";
    bshell(unit, "l3 intf show");
    print "l3 egress show";
    bshell(unit, "l3 egress show");

    printf("Sending %d unicast packets to ingress port %d\n", count, ingress_port);
    snprintf(str, 512, "tx %d pbm=%d data=0x01005e0101010000060003008100000508004500002e0000000040ff91cd02010101e4010101000102030405060708090a0b0c0d0e0f10111213141516171819bf2e44c3", count, ingress_port);
    bshell(unit, str);
    sal_sleep(5);

    printf("\n------------------------------------- \n");
    printf("    RESULT OF PACKET VERIFICATION  ");
    printf("\n------------------------------------- \n");
    printf("Test = [%s]",(test_failed == 0)?"PASS":"FAIL");
    printf("\n------------------------------------- \n\n");

    /* test_fail is asserted by pktio_Rx_call_back.
       sleep command in needed to give pktio_Rx_call_back chance to run. */
    if (test_failed == 1) {
       return BCM_E_FAIL;
    }

    /* Get counter value. */
    sal_memset(&counter_value, 0, sizeof(counter_value));
    rv = bcm_flexctr_stat_get(unit, stat_counter_id, num_entries, &counter_index,
                              &counter_value);
    if (BCM_FAILURE(rv)) {
      printf("Error in bcm_flexctr_stat_get %s\n", bcm_errmsg(rv));
      return rv;
    }

    /* print counter value. */
    printf("Routed : %d packets / %d bytes\n", COMPILER_64_LO(counter_value.value[0]),
                                               COMPILER_64_LO(counter_value.value[1]));
    if (COMPILER_64_LO(counter_value.value[0]) != count) {
      printf("Error in packet verification. Expected packet stat %d but got %d\n", count, COMPILER_64_LO(counter_value.value[0]));
      test_failed = 1;
    };

    printf("\n-------------------------------------------- \n");
    printf("    RESULT OF FLEX COUNTER VERIFICATION  ");
    printf("\n-------------------------------------------- \n");
    printf("Test = [%s]",(test_failed == 0)?"PASS":"FAIL");
    printf("\n-------------------------------------------- \n\n");


    if(test_failed ==0)
    {
       return BCM_E_NONE;
    }
    else
    {
       return BCM_E_FAIL;
    }
}

/*
 *    This functions does the following
 *    a)test setup [configures the feature]
 *    b)test verify [verifies the configured feature]
 */

int
execute()
{
    bcm_error_t rv;
    int unit = 0;

    rv = test_setup(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in test_setup %s \n", bcm_errmsg(rv));
        return rv;
    }
    printf("\nCompleted test setup successfully.\n\n");

    rv = test_verify(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in test_verify %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf("Completed test verify successfully.\n");

    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}
