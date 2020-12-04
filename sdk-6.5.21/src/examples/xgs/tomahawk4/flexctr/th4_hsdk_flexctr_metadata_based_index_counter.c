/*
 * Feature  : Flex counter to demonstrate usage of metadata based index generatiom
 *
 * Usage    : BCM.0> cint th4_hsdk_flexctr_metadata_based_index_counter.c
 *
 * Config   : th4_hsdk_flexctr_metadata_based_index_counter_64x400.config.yml
 *
 * Log file : th4_hsdk_flexctr_metadata_based_index_counter_log.txt
 *
 * Test Topology :
 *                      +----------------------+
 *                      |                      |
 *                      |        TH4           |
 *        ingress_port  |                      |  egress_port
 *       ------------->-+  Flexctr             +---------->----
 *                      |   Src: L3Route       |
 *                      |   Obj: AlpmDstLookup |
 *                      |                      |
 *                      |                      |
 *                      +----------------------+
 *
 *
 * Summary:
 * ========
 * cint script which shows flex counter index generation based on metadata
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup):
 *   =============================================
 *     a) Select ingress port and egress port; configure them in loopback mode
 *     b) Do the necessary L3 route configuration
 *     c) Necessary flex counter configurations are done.
 *        1. The ALPM destination lookup is selected as the object
 *        2. Packets are counted on L3 route [source] with setting the user
 *           provided metadata for index generation
 *
 *     Note: IFP configuration on egress port is meant for a verification
 *           purpose and is not part of the feature configuration.
 *
 *   2) Step2 - Verification (Done in test_verify):
 *   ===============================================
 *     a) Send the packet that matches the description [for routing the packet]
 *
 *     b) Expected Result:
 *     ===================
 *     The number of sent packets should match the flex ctr stat get output
 */

/* Reset C interpreter*/
cint_reset();

bcm_port_t egress_port;
bcm_port_t ingress_port;
bcm_vlan_t ing_vid = 2;
bcm_vlan_t egr_vid = 30;
bcm_mac_t src_mac = {0x00, 0x00, 0x00, 0x0, 0x22, 0x11};
bcm_mac_t dst_mac = {0x00, 0x00, 0x00, 0x0, 0x33, 0x22};
bcm_mac_t router_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
bcm_if_t ingress_if = 100;
bcm_vrf_t vrf = 100;
bcm_ip_t dip = 0xC0A80114;
bcm_ip_t dip_mask = 0xFFFFFFFF;
bcm_if_t egr_if;
uint32_t stat_counter_id, flexctr_obj_val = 7;

/*
 *    Checks if given port/index is already present
 *    in the list so that same port number is not generated again
 */

int
check_assigned_port(int *port_index_list,int no_entries, int index)
{
    int i=0;

    for (i=0; i < no_entries; i++) {
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
    bcm_port_config_t port_config;
    bcm_pbmp_t ports_pbmp;
    int temp_ports[BCM_PBMP_PORT_MAX];
    int port_index_list[num_ports];

    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &port_config));

    ports_pbmp = port_config.e;
    for (i = 1; i < BCM_PBMP_PORT_MAX; i++) {
        if (BCM_PBMP_MEMBER(&ports_pbmp, i)) {
            temp_ports[port] = i;
            port++;
        }
    }

    if (( port == 0 ) || ( port < num_ports )) {
        printf("Eror in portNumbersGet \n");
        return -1;
    }

    /* generating random ports */
    for (i= 0; i < num_ports; i++) {
        index = sal_rand() % port;
        if (check_assigned_port(port_index_list, i, index) == 0) {
            port_list[i] = temp_ports[index];
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
    bcm_port_t port;

    BCM_PBMP_ITER(pbm, port) {
        printf("ingress_port_multi_setup port: %d setup\n", port);
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
        printf("egress_port_multi_setup port: %d setup\n", port);

        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

        BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));
        BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));
        BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
    }

    return BCM_E_NONE;
}

/* Create vlan and add port to vlan */
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
        printf("Error in creating vlan : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vlan, pbmp, ubmp));

    return BCM_E_NONE;
}

/* Create L3 Ingress Interface */
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
        printf("Error in configuring L3_IIF : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Config vlan_id to l3_iif mapping. */
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
        printf("Error in configuring L3_IIF binding : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Create L3 interface */
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
        printf("Error in configuring L3 interface : %s.\n", bcm_errmsg(rv));
        return rv;
    }
    *l3_intf_id = l3_intf.l3a_intf_id;

    return BCM_E_NONE;
}

/* Create L3 egress object */
int
create_egr_obj(int unit, bcm_if_t l3_if, bcm_mac_t mac, bcm_gport_t gport)
{
    bcm_error_t rv;
    bcm_l3_egress_t l3_egr;

    bcm_l3_egress_t_init(&l3_egr);
    l3_egr.intf = l3_if;
    l3_egr.port = gport;
    sal_memcpy(l3_egr.mac_addr, mac, sizeof(mac));

    rv = bcm_l3_egress_create(unit, 0, &l3_egr, &egr_if);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring L3 egress object : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Configure my station */
int
configure_my_station_tcam(int unit, bcm_mac_t mac)
{
    bcm_error_t rv;
    bcm_mac_t mac_mask={0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    bcm_l2_station_t l2_station;
    int station_id;

    bcm_l2_station_t_init(&l2_station);
    sal_memcpy(l2_station.dst_mac, router_mac, sizeof(mac));
    sal_memcpy(l2_station.dst_mac_mask, mac_mask, sizeof(mac_mask));
    l2_station.flags = BCM_L2_STATION_IPV4;

    rv = bcm_l2_station_add(unit, &station_id, &l2_station);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring L3 station : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Adding subnet address to LPM/ALPM table */
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
        printf("Error in configuring L3 LPM/ALPM table : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* L3 route configuration function */
bcm_error_t
setup_l3_routing(int unit)
{
    bcm_error_t rv = BCM_E_NONE;

    bcm_if_t egr_l3_if;
    bcm_gport_t egress_gport;

    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, ingress_port, bcmPortControlIP4, 1));

    /*Add ports to vlan*/
    rv = create_vlan_add_port(unit, ing_vid, ingress_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring ing vlan: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = create_vlan_add_port(unit, egr_vid, egress_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring egr vlan: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Set the default VLAN for the port */
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, ingress_port, ing_vid));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port, egr_vid));

    /* Create L3 ingress interface - l3_iif */
    rv = create_l3_iif(unit, vrf, ingress_if);
    if (BCM_FAILURE(rv)) {
        printf("create_l3_iif return error : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* VLAN to L3_IIF mapping */
    rv = configure_l3_iif_binding(unit, ing_vid, ingress_if);
    if (BCM_FAILURE(rv)) {
        printf("configure_l3_iif_binding return error : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create L3 egress interface */
    rv = create_l3_intf(unit, src_mac, egr_vid, &egr_l3_if);
    if (BCM_FAILURE(rv)) {
        printf("create_l3_intf return error : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, egress_port, &egress_gport));

    /* Cretae L3 egress object */
    rv = create_egr_obj(unit, egr_l3_if, dst_mac, egress_gport);
    if (BCM_FAILURE(rv)) {
        printf("create_egr_obj return error : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Configure MY_STATION TCAM for router MAC */
    rv = configure_my_station_tcam(unit, router_mac);
    if (BCM_FAILURE(rv)) {
        printf("configure_my_station_tcam return error : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Program ROUTE in LPM/ALPM table */
    rv = configure_route(unit, dip, dip_mask, egr_if, vrf);
    if (BCM_FAILURE(rv)) {
        printf("configure_route return error : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*    Configures the flex counters to count on route with the provided metadata
 *    as index
 */

int
configure_flex_counter_metadata_based_index(int unit)
{
    int options = 0;
    bcm_error_t  rv;
    bcm_flexctr_action_t action;
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_a_op = NULL;
    bcm_flexctr_value_operation_t *value_b_op = NULL;

    bcm_flexctr_action_t_init(&action);
    action.flags = 0;
    action.source = bcmFlexctrSourceL3Route;
    action.mode = bcmFlexctrCounterModeNormal;
    action.index_num = 8192;

    /* Counter index is PKT_ATTR_OBJ0. */
    index_op = &action.index_operation;
    index_op->object[0] = bcmFlexctrObjectStaticIngAlpmDstLookup;
    index_op->mask_size[0] = 13;
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

    bcm_l3_route_t route;

    bcm_l3_route_t_init(&route);
    route.l3a_subnet = dip;
    route.l3a_ip_mask = dip_mask;
    route.l3a_intf = egr_if;
    route.l3a_vrf = vrf;

    /* Attach counter action to route */
    rv = bcm_l3_route_stat_attach(unit, &route, stat_counter_id);
    if (BCM_FAILURE (rv)) {
        printf ("bcm_l3_route_stat_attach() FAILED: %s\n", bcm_errmsg (rv));
        return rv;
    }

    rv = bcm_l3_route_flexctr_object_set(unit, &route, flexctr_obj_val);
    if (BCM_FAILURE (rv)) {
        printf ("bcm_l3_route_flexctr_object_set() FAILED: %s\n", bcm_errmsg (rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 *    Configures the flex counter feature and the required set up to verify it
 */

int test_setup(int unit)
{
    int num_ports = 2;
    int port_list[num_ports];
    bcm_error_t rv;
    bcm_pbmp_t pbmp;

    rv = port_numbers_get(unit, port_list, num_ports);
    if (BCM_FAILURE(rv)) {
        printf("Error in portNumbersGet %s\n", bcm_errmsg(rv));
        return rv;
    }

    ingress_port = port_list[0];
    egress_port = port_list[1];
    printf("Choosing port %d as ingress port and %d as egress port\n", ingress_port, egress_port);

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port);
    rv = ingress_port_multi_setup(unit, pbmp);
    if (BCM_FAILURE(rv)) {
        printf("Error in ingress_port_multi_setup %s\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, egress_port);
    rv = egress_port_multi_setup(unit, pbmp);
    if (BCM_FAILURE(rv)) {
        printf("Error in egress_port_multi_setup %s\n", bcm_errmsg(rv));
        return rv;
    }

     bshell(unit, "pw start report +raw +decode");

    /* Configure L3 route */
    rv = setup_l3_routing(unit);
    if (BCM_FAILURE(rv)) {
        printf("setup_l3_routing() failed. %s\n", bcm_errmsg(rv));
        return rv;
    }

    /*    Configure flex counters to count on route with provided metadata which
     *    provides index to the counter
     */
    rv = configure_flex_counter_metadata_based_index(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in configure_flex_counter_egr_pkt_attr %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Flex counter feature Verification */
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

    print "vlan show";
    bshell(unit, "vlan show");
    print "l2 show";
    bshell(unit, "l2 show");
    bshell(unit, "clear c");
    print "l3 route show";
    bshell(unit, "l3 route show");
    print "l3 intf show";
    bshell(unit, "l3 intf show");
    print "l3 egress show";
    bshell(unit, "l3 egress show");

    printf("\nSending %d unicast packets to %d ingress port \n", count, ingress_port);
    snprintf(str, 512, "tx %d pbm=%d data=0x0000000000020000000000018100000208004500002A0000000040FFA3E50A0A0A2AC0A8011400010203000000000000000000000000000000000000024D7BD6", count, ingress_port);
    bshell(unit, str);
    sal_sleep(5);

    /* Get counter value. */
    sal_memset(&counter_value, 0, sizeof(counter_value));
    rv = bcm_flexctr_stat_get(unit, stat_counter_id, num_entries, &counter_index, &counter_value);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_flexctr_stat_sync_get %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* print counter value. */
    printf("\nStat - %d packets / %d bytes\n", COMPILER_64_LO(counter_value.value[0]),
                                             COMPILER_64_LO(counter_value.value[1]));

    return BCM_E_NONE;
}

/*
 *    This functions does the following
 *    a)test setup [configures the feature]
 *    b)test verify [verifies the configured feature]
 */

int execute()
{
    bcm_error_t rv;
    int unit = 0;

    bshell(unit, "config show; attach ; version");

    rv = test_setup(unit);
    if (BCM_FAILURE(rv)) {
        printf("test_setup() failed, %s.\n", bcm_errmsg(rv));
        return -1;
    }
    printf("\nCompleted test setup successfully\n");

    rv = test_verify(unit);
    if (BCM_FAILURE(rv)) {
        printf("test_verify() failed\n");
        return -1;
    }
    printf("\nCompleted test verify successfully\n");

    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}
