/*
 * Feature  : L3 routing to IPGRE tunnel IPv4inIPv4
 *
 * Usage    : BCM.0> cint th4_hsdk_l3_ip_tunnel_ipgre_4in4.c
 *
 * config   : th4_hsdk_l3_64x400.config.yml
 *
 * Log file : th4_hsdk_l3_ip_tunnel_ipgre_4in4_log.txt
 *
 * Test Topology :
 *                      +---------------------+
 *                      |                     |
 *                      |       TH4           |
 *        port_access   |                     |  port_network
 *        --------->----+  Routing            +------->------
 *            vlan 21   |    ALPM             |  vlan 22
 *      dst_mac-0x1111  |    vrf -100         |  dst_mac-0x2222
 *                      |init dip - 0x09090909|
 *                      |    mask -0xFFFFFFFF |
 *        ---------<----+  Routing            +-------<------
 *                      |term dip - 0x0A0A0A0A|
 *                      |    mask -0xFFFFFFFF |
 *                      +---------------------+
 *
 *
 * Summary:
 * ========
 * cint script to demonstrate L3 forwarding through ALPM routing in parallel
 * mode [l3_alpm_template: 2 in config file]. In parallel mode, there are two
 * DBs - one for global high and low routes and another for private routes.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup):
 *   =============================================
 *     a) Selects two ports and configure them in Loopback mode. Out of these
 *        two ports, one port is used as port_access and the other as
 *        port_network.
 *     c) Configure a basic IPv4 route. This adds the route in L3 ALPM table
 *        and does the necessary configurations of vlan, L3_IIF, egress intf
 *        and next hop.
 *
 *   2) Step2 - Verification (Done in test_verify):
 *   ==============================================
 *     a) Check the configurations by 'l3 intf show', 'l3 egress show' and
 *        'l3 route show'
 *     b) Transmit the known IPv4 unicast packet. The packet used matches the
 *        route configured through script.
 *     c) Expected Result:
 *     ===================
 *       We see that dmac, smac, TTL and vlan are all changed, ipgre tunnel header
 *       added/removed as the packet gets routed through the egress port and the
 *       number of sent packets should match the flex ctr stat get output
 */

/* Reset C interpreter*/
cint_reset();

bcm_port_t port_network;
bcm_port_t port_access;
bcm_vlan_t vid_access = 21;
bcm_vlan_t vid_network = 22;
bcm_mac_t src_mac_access = {0x00, 0x00, 0x00, 0x0, 0x11, 0x11};
bcm_mac_t dst_mac_access = {0x00, 0x00, 0x00, 0x0, 0x00, 0x01};
bcm_mac_t src_mac_network = {0x00, 0x00, 0x00, 0x0, 0x22, 0x22};
bcm_mac_t dst_mac_network = {0x00, 0x00, 0x00, 0x0, 0x00, 0x02};
bcm_mac_t router_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
bcm_if_t ingress_if_access = 100;
bcm_if_t ingress_if_network = 101;
bcm_vrf_t vrf = 100;

bcm_ip_t tnl_init_dip = 0x01010101; /* 1.1.1.1 */
bcm_ip_t tnl_term_dip = 0x02020202; /* 2.2.2.2 */

bcm_ip_t tnl_init_outer_sip = 0x0a0a0a0a; /* 10.10.10.10 */
bcm_ip_t tnl_init_outer_dip = 0x09090909; /* 9.9.9.9 */
bcm_ip_t tnl_term_outer_dip = 0x0a0a0a0a;
bcm_ip_t tnl_term_outer_sip = 0x09090909;
bcm_if_t egr_if_access;
bcm_if_t egr_if_network;
bcm_l3_intf_t egr_l3_if_netwrk;
uint32_t stat_counter_id_terminator, stat_counter_id_initiator;
uint32_t flexctr_obj_val_initiator = 0, flexctr_obj_val_terminator = 7;

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
 * This function is written so that hardcoding of port
 * numbers in Cint scripts is removed. This function gives
 * required number of ports
 */
bcm_error_t
portNumbersGet(int unit, int *port_list, int num_ports)
{
	bcm_error_t rv = BCM_E_NONE;
    int i = 0, port = 0;
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
        return BCM_E_PORT;
    }

    return BCM_E_NONE;
}

/*
 *    Configures the port in loopback mode
 */

int
ingress_port_multi_setup(int unit, bcm_pbmp_t pbm)
{
    bcm_port_t port;
				bcm_field_group_t group;
    bcm_field_entry_t entry;

    bcm_field_group_config_t group_config;

    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDrop);
    group_config.priority = 1;
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    BCM_PBMP_ITER(pbm, port) {
        printf("Ingress_port_multi_setup port: %d setup\n", port);
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
	    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));
        BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
        BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
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
        BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
    }

    return BCM_E_NONE;
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
    l3_ingress.vrf   = vrf;

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
create_l3_intf(int unit, bcm_mac_t mac, bcm_vlan_t vlan, bcm_l3_intf_t *l3_intf)
{
    bcm_error_t rv;

    bcm_l3_intf_t_init(l3_intf);
    l3_intf->l3a_vid = vlan;
    sal_memcpy(l3_intf->l3a_mac_addr, mac, sizeof(mac));

    rv = bcm_l3_intf_create(unit, l3_intf);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring L3 interface %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*    Create L3 egress object    */

int
create_egr_obj(int unit, bcm_if_t l3_if, bcm_mac_t mac, bcm_gport_t gport,
               bcm_vlan_t vid, bcm_if_t *egr_obj)
{
    bcm_error_t rv;
    bcm_l3_egress_t l3_egr;

    bcm_l3_egress_t_init(&l3_egr);
    l3_egr.intf = l3_if;
    l3_egr.port = gport;
    l3_egr.vlan = vid;
    sal_memcpy(l3_egr.mac_addr, mac, sizeof(mac));

    rv = bcm_l3_egress_create(unit, 0, &l3_egr, egr_obj);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring L3 egress object %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*    Tunnel initiation configuration    */
int
configure_tunnel_initiation(int unit, bcm_tunnel_type_t type, int ttl,
                            bcm_mac_t dst_mac, bcm_l3_intf_t *egr_l3_intf)
{
    bcm_error_t rv;
    bcm_tunnel_initiator_t tnl_init_info;

    bcm_tunnel_initiator_t_init(&tnl_init_info);
    tnl_init_info.type = type;
    tnl_init_info.ttl  = ttl;
    sal_memcpy(tnl_init_info.dmac, dst_mac, sizeof(dst_mac));
    tnl_init_info.dip = tnl_init_outer_dip;
    tnl_init_info.sip = tnl_init_outer_sip;

    rv = bcm_tunnel_initiator_set(unit, egr_l3_intf, &tnl_init_info);
    if (BCM_FAILURE(rv)) {
       printf("Error in bcm_tunnel_initiator_set %s.\n", bcm_errmsg(rv));
       return rv;
    }

    print tnl_init_info.tunnel_id;

    return BCM_E_NONE;
}

/*    Tunnel termination configuration    */
int
configure_tunnel_termination(int unit, bcm_tunnel_type_t type, bcm_vrf_t vrf,
                              bcm_if_t ingress_if, bcm_port_t port)
{
    bcm_error_t rv;
    bcm_tunnel_terminator_t tnl_term_info;

    bcm_tunnel_terminator_t_init(&tnl_term_info);
    int tnl_term_flags = BCM_TUNNEL_TERM_USE_OUTER_DSCP |
                         BCM_TUNNEL_TERM_USE_OUTER_TTL |
                         BCM_TUNNEL_TERM_KEEP_INNER_DSCP;
    tnl_term_info.flags = tnl_term_flags;
    tnl_term_info.type = type;
    tnl_term_info.vrf = vrf;
    tnl_term_info.vlan = ingress_if;
    tnl_term_info.sip = tnl_term_outer_sip;
    tnl_term_info.dip = tnl_term_outer_dip;
    tnl_term_info.sip_mask = bcm_ip_mask_create(32);
    tnl_term_info.dip_mask = bcm_ip_mask_create(32);
    BCM_PBMP_PORT_SET(tnl_term_info.pbmp, port);

    rv = bcm_tunnel_terminator_add(unit, &tnl_term_info);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_tunnel_terminator_add %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*    Configure my station    */

int
configure_my_station_tcam(int unit, bcm_mac_t mac, bcm_vlan_t vid)
{
    bcm_error_t rv;
    bcm_mac_t mac_mask={0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    bcm_l2_station_t l2_station;
    int station_id;

    bcm_l2_station_t_init(&l2_station);
    sal_memcpy(l2_station.dst_mac, mac, sizeof(mac));
    sal_memcpy(l2_station.dst_mac_mask, mac_mask, sizeof(mac_mask));
    l2_station.flags = BCM_L2_STATION_IPV4;
    l2_station.vlan = vid;
    l2_station.vlan_mask = 0xFFF;

    rv = bcm_l2_station_add(unit, &station_id, &l2_station);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring L2 station %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*    Adding subnet address to ALPM table    */

int
configure_route(int unit, bcm_ip_t subnet_addr, bcm_ip_t subnet_mask, bcm_if_t egr_if,
                bcm_vrf_t vrf)
{
    bcm_error_t  rv;
    bcm_l3_route_t route;

    bcm_l3_route_t_init(&route);
    route.l3a_subnet  = subnet_addr;
    route.l3a_ip_mask = subnet_mask;
    route.l3a_intf    = egr_if;
    route.l3a_vrf     = vrf;
    rv = bcm_l3_route_add(unit, &route);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring L3 ALPM table %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*    L3 route configuration function    */

int
configure_ipgre_tunnel(int unit)
{
    bcm_error_t rv = BCM_E_NONE;

    bcm_l3_intf_t egr_l3_if_access;
    bcm_l3_intf_t egr_l3_if_network;
    bcm_gport_t port_network_gport;
    bcm_gport_t port_access_gport;
    int ttl = 0x40;

    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, port_access, bcmPortControlIP4, 1));

    /* Create vlan and add ports for access side */
    rv = create_vlan_add_port(unit, vid_access, port_access);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring ing vlan %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create vlan and add ports for network side */
    rv = create_vlan_add_port(unit, vid_network, port_network);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring egr vlan %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Set the default VLAN for the access port */
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, port_access, vid_access));

    /* Set the default VLAN for the network port */
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, port_network, vid_network));

    /* Create L3 ingress interface - l3_iif access side */
    rv = create_l3_iif(unit, vrf, ingress_if_access);
    if (BCM_FAILURE(rv)) {
        printf("Error in create_l3_iif %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create L3 ingress interface - l3_iif network side */
    rv = create_l3_iif(unit, vrf, ingress_if_network);
    if (BCM_FAILURE(rv)) {
       printf("Error in create_l3_iif %s.\n", bcm_errmsg(rv));
       return rv;
    }

    /* VLAN to L3_IIF mapping access side */
    rv = configure_l3_iif_binding(unit, vid_access, ingress_if_access);
    if (BCM_FAILURE(rv)) {
        printf("Error in configure_l3_iif_binding %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* VLAN to L3_IIF mapping network side */
    rv = configure_l3_iif_binding(unit, vid_network, ingress_if_network);
    if (BCM_FAILURE(rv)) {
       printf("Error in configure_l3_iif_binding %s.\n", bcm_errmsg(rv));
       return rv;
    }

    /* Create L3 egress interface for access side */
    rv = create_l3_intf(unit, src_mac_access, vid_access, &egr_l3_if_access);
    if (BCM_FAILURE(rv)) {
        printf("Error in create_l3_intf %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create L3 egress interface for network side */
    rv = create_l3_intf(unit, src_mac_network, vid_network, &egr_l3_if_network);
    if (BCM_FAILURE(rv)) {
       printf("Error in create_l3_intf %s.\n", bcm_errmsg(rv));
       return rv;
    }

    /* Configure tunnel initiation */
    rv = configure_tunnel_initiation(unit, bcmTunnelTypeGre4In4, ttl,
                                     dst_mac_network, &egr_l3_if_network);
    if (BCM_FAILURE(rv)) {
       printf("Error in configure_tunnel_initiation %s.\n", bcm_errmsg(rv));
       return rv;
    }
    egr_l3_if_netwrk = egr_l3_if_network;
    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, port_network, &port_network_gport));

    /* Create L3 egress object for network side */
    rv = create_egr_obj(unit, egr_l3_if_network.l3a_intf_id, dst_mac_network,
                        port_network_gport, vid_network, &egr_if_network);
    if (BCM_FAILURE(rv)) {
        printf("Error in create_egr_obj %s.\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, port_access, &port_access_gport));

    /* Create L3 egress object for access side */
    rv = create_egr_obj(unit, egr_l3_if_access.l3a_intf_id, dst_mac_access,
                        port_access_gport, vid_access, &egr_if_access);
    if (BCM_FAILURE(rv)) {
       printf("Error in create_egr_obj %s.\n", bcm_errmsg(rv));
       return rv;
    }

    rv = configure_tunnel_termination(unit, bcmTunnelTypeGre4In4, BCM_L3_VRF_DEFAULT,
                                      ingress_if_network, port_network);
    if (BCM_FAILURE(rv)) {
       printf("Error in create_egr_obj %s.\n", bcm_errmsg(rv));
       return rv;
    }


    /* Configure MY_STATION TCAM for access side */
    rv = configure_my_station_tcam(unit, src_mac_access, vid_access);
    if (BCM_FAILURE(rv)) {
        printf("Error in configure_my_station_tcam for access side %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Configure MY_STATION TCAM for network side */
    rv = configure_my_station_tcam(unit, src_mac_network, vid_network);
    if (BCM_FAILURE(rv)) {
        printf("Error in configure_my_station_tcam for network side %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Program ROUTE for tunnel initiation */
    rv = configure_route(unit, tnl_init_dip, bcm_ip_mask_create(32), egr_if_network, vrf);
    if (BCM_FAILURE(rv)) {
        printf("Error in configure_route for tunnel initiation %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Program ROUTE for tunnel termination */
    rv = configure_route(unit, tnl_term_dip, bcm_ip_mask_create(32), egr_if_access, vrf);
    if (BCM_FAILURE(rv)) {
        printf("Error in configure_route for tunnel initiation %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Create flex counter action with IpTunnelInitiator as source and user set index */

int
configure_flex_counter_tunnel_initiator(int unit)
{
    int options = 0;
    bcm_error_t  rv;
    bcm_flexctr_action_t action;
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_a_op = NULL;
    bcm_flexctr_value_operation_t *value_b_op = NULL;

    print bcm_flexctr_action_t_init(&action);
    action.flags = 0;
    action.source = bcmFlexctrSourceEgrL3Intf;
    action.mode = bcmFlexctrCounterModeNormal;
    action.index_num = 100; /* Number of required counters */

    index_op = &action.index_operation;
    index_op->object[0] = bcmFlexctrObjectConstZero;
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
    rv = bcm_flexctr_action_create(unit, options, &action, &stat_counter_id_initiator);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_flexctr_action_create %s\n", bcm_errmsg(rv));
        return rv;
    }
	print stat_counter_id_initiator;

    /* Attach counter action to route */
    rv = bcm_l3_intf_stat_attach(unit, egr_l3_if_netwrk.l3a_intf_id, stat_counter_id_initiator);
    if (BCM_FAILURE(rv)) {
        printf("Error in  bcm_tunnel_terminator_stat_attach: %s.\n", bcm_errmsg(rv));
    }

    return BCM_E_NONE;
}

/* Create flex counter action with IpTunnelTerminator as source and user set index */

int
configure_flex_counter_tunnel_terminator(int unit)
{
    int options = 0;
    bcm_error_t  rv;
    bcm_flexctr_action_t action;
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_a_op = NULL;
    bcm_flexctr_value_operation_t *value_b_op = NULL;

    print bcm_flexctr_action_t_init(&action);
    action.flags = 0;
    action.source = bcmFlexctrSourceIpTunnelTerminator;
    action.mode = bcmFlexctrCounterModeNormal;
    action.index_num = 100; /* Number of required counters */

    index_op = &action.index_operation;
    index_op->object[0] = bcmFlexctrObjectStaticIngL3Tunnel;
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
    rv = bcm_flexctr_action_create(unit, options, &action, &stat_counter_id_terminator);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_flexctr_action_create %s\n", bcm_errmsg(rv));
        return rv;
    }
	print stat_counter_id_terminator;

    bcm_tunnel_terminator_t tnl_term_info;

    bcm_tunnel_terminator_t_init(&tnl_term_info);
    int tnl_term_flags = BCM_TUNNEL_TERM_USE_OUTER_DSCP |
                         BCM_TUNNEL_TERM_USE_OUTER_TTL |
                         BCM_TUNNEL_TERM_KEEP_INNER_DSCP;
    tnl_term_info.flags = tnl_term_flags;
    tnl_term_info.type = bcmTunnelTypeGre4In4; /* Choose Ipv4-in-Ipv4 tunnel type */
    tnl_term_info.vrf = BCM_L3_VRF_DEFAULT;
    tnl_term_info.vlan = ingress_if_network;
    tnl_term_info.sip = tnl_term_outer_sip;
    tnl_term_info.dip = tnl_term_outer_dip;
    tnl_term_info.sip_mask = bcm_ip_mask_create(32);
    tnl_term_info.dip_mask = bcm_ip_mask_create(32);
    BCM_PBMP_PORT_SET(tnl_term_info.pbmp, port_network);

    /* Attach counter action to route */
    rv = bcm_tunnel_terminator_stat_attach(unit, &tnl_term_info, stat_counter_id_terminator);
    if (BCM_FAILURE(rv)) {
        printf("Error in  bcm_tunnel_terminator_stat_attach: %s.\n", bcm_errmsg(rv));
    }

    /* Set the counter index */
    rv = bcm_tunnel_terminator_flexctr_object_set(unit, &tnl_term_info, flexctr_obj_val_terminator);
    if (BCM_FAILURE (rv)) {
        printf ("Error in bcm_l3_route_flexctr_object_set %s\n", bcm_errmsg (rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 *    Configures the IPV4 unicast route and the required set up to verify it
 */

int
test_setup(int unit)
{
    int num_ports = 2;
    int port_list[num_ports];
    bcm_error_t rv;
    bcm_pbmp_t pbmp;

    rv = portNumbersGet(unit, port_list, num_ports);
    if (BCM_FAILURE(rv)) {
        printf("Error in portNumbersGet%s\n", bcm_errmsg(rv));
        return rv;
    }

    port_access = port_list[0];
    port_network = port_list[1];
	print port_access;
	print port_network;

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port_access);
    rv = ingress_port_multi_setup(unit, pbmp);
    if (BCM_FAILURE(rv)) {
        printf("Error in ingress_port_multi_setup %s\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port_network);
    rv = egress_port_multi_setup(unit, pbmp);
    if (BCM_FAILURE(rv)) {
        printf("Error in egress_port_multi_setup %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = configure_ipgre_tunnel(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in configure_ipgre_tunnel %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = configure_flex_counter_tunnel_initiator(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in configure_flex_counter_tunnel_initiator %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = configure_flex_counter_tunnel_terminator(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in configure_flex_counter_tunnel_terminator %s\n", bcm_errmsg(rv));
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
    uint32 counter_index_terminator = flexctr_obj_val_terminator;
    uint32 counter_index_initiator = flexctr_obj_val_initiator;
    int count = sal_rand() % 10;
    if (count == 0) {
      count = 1;
    }
    int test_failed = 0;

    print "l3 route show";
    bshell(unit, "l3 route show");
    printf("\n");
    print "l3 intf show";
    bshell(unit, "l3 intf show");
    print "l3 egress show";
    bshell(unit, "l3 egress show");

    printf("Sending %d unicast packets to ingress port %d\n", count, port_access);
	snprintf(str, 512, "tx %d pbm=%d data=0x0000000011110000000000018100001508004500002E0000000040FF73CC0202020201010101000000000000000000000000000000000000000000000000000058863ECA", count, port_access);
    bshell(unit, str);
	sal_sleep(2);

	printf("Sending %d unicast packets to ingress port %d\n", count, port_network);
	snprintf(str, 512, "tx %d pbm=%d data=0x0000000022220000000000028100001608004500004654B40000402FFFAF090909090A0A0A0A000008004500002E000000003FFF74CC01010101020202020000000000000000000000000000000000000000000000000000", count, port_network);
    bshell(unit, str);
    sal_sleep(2);

    /* Get counter value. */
    sal_memset(&counter_value, 0, sizeof(counter_value));
    rv = bcm_flexctr_stat_get(unit, stat_counter_id_initiator, num_entries, &counter_index_initiator,
                              &counter_value);
    if (BCM_FAILURE(rv)) {
      printf("Error in bcm_flexctr_stat_get %s\n", bcm_errmsg(rv));
      return rv;
    }

    /* print counter value. */
    printf("Tunnel initiated Stats: %d packets / %d bytes\n", COMPILER_64_LO(counter_value.value[0]),
                                               COMPILER_64_LO(counter_value.value[1]));
    if (COMPILER_64_LO(counter_value.value[0]) != count) {
      printf("Error in packet verification. Expected packet stat %d but got %d\n", count, COMPILER_64_LO(counter_value.value[0]));
      test_failed = 1;
    };

    /* Get counter value. */
    sal_memset(&counter_value, 0, sizeof(counter_value));
    rv = bcm_flexctr_stat_get(unit, stat_counter_id_terminator, num_entries, &counter_index_terminator,
                              &counter_value);
    if (BCM_FAILURE(rv)) {
      printf("Error in bcm_flexctr_stat_get %s\n", bcm_errmsg(rv));
      return rv;
    }

    /* print counter value. */
    printf("Tunnel terminated Stats: %d packets / %d bytes\n", COMPILER_64_LO(counter_value.value[0]),
                                               COMPILER_64_LO(counter_value.value[1]));
    if (COMPILER_64_LO(counter_value.value[0]) != count) {
      printf("Error in packet verification. Expected packet stat %d but got %d\n", count, COMPILER_64_LO(counter_value.value[0]));
      test_failed = 1;
    };

    printf("\n-------------------------------------------- \n");
    printf("    RESULT OF L3 ROUTE to IPGRE 4in4 TUNNEL VERIFICATION  ");
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
