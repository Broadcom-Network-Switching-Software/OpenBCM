/*
 * Feature        : WRED-ECN
 *
 * Usage          : BCM.0> cint th4_hsdk_mmu_wred_latency_ecn.c
 *
 * Config         : th4_hsdk_mmu_config.yml
 *
 * Log file       : th4_hsdk_mmu_wred_latency_ecn_log.txt
 *
 * Test Topology  :
 *
 *     DMAC=0x000000bbbbbb  +------------+         DMAC=0x00000000fe02
 *     SMAC=0x2   +-------->|  56990     |-------> SMAC=0x22
 *     VLAN=3, Pri=0 cd0(1) |            |cd1(2)   VLAN=2, Pri=0
 *                          |            |
 *     TEST-1:              |            |        TEST-1:
 *     IP.Protocol=1(TCP)   |            |        IF congested:
 *     ECN bits=10          |            |          ECN bits=11
 *                          +------------+        If not congested:
 *                                                  ECN bits=10
 *
 *  Summary:
 *  ========
 *  This cint example illustrates configuration of WRED and ECN feature
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup()) 
 *    ============================================
 *      a) Select one ingress and one egress port
 *      b) Set up required L3 routing for traffic
 *      c) Set up Flex counters per route
 *      d) Set up Packet IO callback for packet verification
 *
 *    2) Step2 - Configuration (Done in wred_ecn_config())
 *    ====================================================
 *      a) set_responsive_protocol_indication - sets IP Protocol's Responsive indication setting
 *      b) ip_to_int_cn_mapping - maps IP header ECN bits to INT_CN value
 *      c) int_cn_to_mmuif_mapping - INT_CN value to WRED_RESPONSIVE and MARK_ELIGIBLE configuration
 *      d) mmu_wred_ecn_setup - MMU wred and ecn profile Settings
 *      e) setup_latency_ecn_profile_port_queue - Setup per port per queue latency profile
 *      f) egr_int_cn_latency_cn_update - configure egress INT_CN value based on latency  status
 *      g) egr_ecn_mark - Mark outgoing IP header ECN bits based on final INT_CN value
 *
 *    3) Step3 - Verification (Done in test_verify()) 
 *    ==================================
 *      a) Transmit IP-TCP packets to the ingress port, packet is routed to egress port
 *      b) Since the WRED is configured to low threshold, packet is ECN marked
 *      c) Check flex counter
 *      d) Expected Result:
 *         ================
 *         For IP TCP packets - In case of congestion we should see ECN marking (ECN bits=11)
 */

cint_reset();

int unit = 0;
int debug = 0;     /* Make it '1' to dump tables that are configured by each API */
int rv = 0;

uint8 iphdr_ecn = 2; /* Incoming packet IP headers ECN field(2 bits).
                        0b01 taken as example for this test.*/
uint8 resp_mapped_int_cn = 2; /* INT_CN value to which Responsive protocol is mapped to.
                                 0b10 taken as example for this test */
uint8 non_cong_int_cn = 2; /* INT_CN value at Dequeue from MMU when there is no congestion.
                                 0b10 taken as example for this test */
uint8 cong_int_cn = 3; /* INT_CN value at Dequeue from MMU when there is congestion.
                                 0b11 taken as example for this test  */
uint8 new_outgoing_noncong_ecn = 2; /* ECN bits marked in outgoing packets when there is no congestion.
                                 0b10 taken as example */
uint8 new_outgoing_cong_ecn = 3; /* ECN bits marked in outgoing packets when there is congestion.
                                 0b11 taken as example  */

bcm_port_t ing_port;
bcm_port_t egr_port;
bcm_vlan_t vlan_in = 3, vlan_out = 2;
bcm_mac_t router_mac_in = {0x00, 0x00, 0x00, 0xBB, 0xBB, 0xBB};
bcm_mac_t router_mac_out = {0x00, 0x00, 0x00, 0x00, 0x02, 0x02};
bcm_mac_t nh_mac = {0x00, 0x00, 0x00, 0x00, 0xfe, 0x02};
bcm_ip_t dip = 0xc0a80064;
bcm_ip_t mask = 0xffffffff;
bcm_vrf_t vrf = 1;
uint32_t stat_counter_id;
bcm_flexctr_counter_value_t counter_value;

bcm_error_t portNumbersGet(int unit, int *port_list, int num_ports)
{
    int i=0, port=0, rv=0;
    bcm_port_config_t configP;
    bcm_pbmp_t ports_pbmp;

    rv = bcm_port_config_get(unit, &configP);
    if(BCM_FAILURE(rv)) {
       printf("\nError in retrieving port configuration: %s.\n", bcm_errmsg(rv));
       return rv;
    }

    ports_pbmp = configP.e;
    for (i= 1; i < BCM_PBMP_PORT_MAX; i++) {
      if (BCM_PBMP_MEMBER(&ports_pbmp, i)&& (port < num_ports)) {
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

        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

        BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));

        BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));

        BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
    }

    return BCM_E_NONE;
}

/*Expected Egress packets */
unsigned char expected_egress_packet[68] = {
    0x00, 0x00, 0x00, 0x00, 0xfe, 0x02, 0x00, 0x00,
    0x00, 0x00, 0x02, 0x02, 0x81, 0x00, 0x00, 0x02,
    0x08, 0x00, 0x45, 0x03, 0x00, 0x2e, 0x00, 0x00,
    0x40, 0x00, 0x09, 0x06, 0xee, 0xae, 0xc0, 0xa8,
    0x01, 0x64, 0xc0, 0xa8, 0x00, 0x64, 0x81, 0x81,
    0x51, 0x51, 0xb1, 0xb2, 0xb3, 0xb4, 0xa1, 0xa2,
    0xa3, 0xa4, 0x50, 0x0f, 0x01, 0xf5, 0x43, 0xce,
    0x00, 0x00, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
    0x17, 0x18, 0x19, 0x1a
};

int test_failed = 0; /*Final result will be stored here */
int rx_count;   /* Global received packet count */

/* Rx callback function for applications using the HSDK PKTIO */
bcm_pktio_rx_t
pktioRxCallback(int unit, bcm_pktio_pkt_t * packet, void *cookie)
{
    int *count = (auto) cookie;
    int result_packet_check = -1;
    int result_port_check = -1;
    void    *buffer;
    uint32  length;
    uint32  src_port;

    (*count)++;

    /* Get basic packet info */
    if (BCM_FAILURE(bcm_pktio_pkt_data_get(unit, packet, (void *) &buffer, &length))) {
        return BCM_PKTIO_RX_NOT_HANDLED;
    }
    printf("length = %d\n", length);

    /* Get source port metadata */
    BCM_IF_ERROR_RETURN(bcm_pktio_pmd_field_get(unit, packet, bcmPktioPmdTypeRx,
                                            BCM_PKTIO_RXPMD_SRC_PORT_NUM, &src_port));

    result_packet_check = sal_memcmp(buffer, expected_egress_packet, length);
    if(egr_port == src_port) {
        result_port_check = 0;
    }
    printf ("result_packet_check %d result_port_check %d\r\n", result_packet_check, result_port_check);

    test_failed = test_failed || (result_packet_check != 0) || (result_port_check != 0);

    return BCM_PKTIO_RX_NOT_HANDLED;
}

/* Register callback function for received packets */
bcm_error_t
registerPktioRxCallback(int unit)
{
    const uint8  priority = 101;
    const uint32 flags = 0;      /* Flags only used on match (for now) */

    return bcm_pktio_rx_register(unit, "rx_cb", pktioRxCallback, priority, &rx_count, flags);
}

bcm_error_t
unregisterPktioRxCallback(int unit)
{
    return bcm_pktio_rx_unregister(unit, pktioRxCallback, 101);
}

int
flex_ctr_setup(int unit, bcm_l3_route_t *route)
{
    bcm_flexctr_action_t action;
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_a_op = NULL;
    bcm_flexctr_value_operation_t *value_b_op = NULL;
    int flexctr_obj_val =7;
    int options = 0;
    bcm_error_t rv;

    bcm_flexctr_action_t_init (&action);
    action.flags = 0;
    action.source = bcmFlexctrSourceL3Route;
    action.mode = bcmFlexctrCounterModeNormal;
    action.index_num = 64;

    index_op = &action.index_operation;
    index_op->object[0] = bcmFlexctrObjectConstZero;
    index_op->mask_size[0] = 1;
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
    if(BCM_FAILURE(rv)) {
        printf("\nError in flex counter action create: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Attach counter action to route */
    rv = bcm_l3_route_stat_attach(unit, route, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_l3_route_stat_attach %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Set Flex Ctr Object from Route table for counter Index Computation at Counter Processor Table */
    BCM_IF_ERROR_RETURN(bcm_l3_route_flexctr_object_set(unit, route, flexctr_obj_val));

    return BCM_E_NONE;
}

bcm_error_t
flex_ctr_cleanup(int unit)
{
    bcm_l3_route_t route_info;
    int rv;

    bcm_l3_route_t_init(&route_info);
    route_info.l3a_subnet = dip;
    route_info.l3a_ip_mask = mask;
    route_info.l3a_vrf = vrf;

    /* Detach counter action. */
    rv = bcm_l3_route_stat_detach(unit, &route_info);
    if (BCM_FAILURE(rv)) {
        printf("bcm_l3_route_stat_detach: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Destroy counter action. */
    rv = bcm_flexctr_action_destroy(unit, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_action_destroy %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

int
flex_ctr_check(int unit)
{
    uint32 num_entries = 1;
    uint32 counter_index = 0;
    int rv;

    /* Get counter value. */
    sal_memset(&counter_value, 0, sizeof(counter_value));
    rv = bcm_flexctr_stat_sync_get(unit, stat_counter_id, num_entries, &counter_index, &counter_value);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_stat_get %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* print counter value. */
    printf("FlexCtr Get : %d packets / %d bytes\n",
            COMPILER_64_LO(counter_value.value[0]), COMPILER_64_LO(counter_value.value[1]));

    if (COMPILER_64_LO(counter_value.value[0]) != 1) {
        return -1;
    }

    return BCM_E_NONE;
}

bcm_error_t l3_route_uc_create(int unit)
{
    /* Create L3 interface */
    bcm_l3_intf_t l3_intf_in, l3_intf_out;

    bcm_l3_intf_t_init(&l3_intf_in);
    sal_memcpy(l3_intf_in.l3a_mac_addr, router_mac_in, sizeof(router_mac_in));
    l3_intf_in.l3a_vid = vlan_in;
    print bcm_l3_intf_create(unit, &l3_intf_in);

    bcm_l3_intf_t_init(&l3_intf_out);
    sal_memcpy(l3_intf_out.l3a_mac_addr, router_mac_out, sizeof(router_mac_out));
    l3_intf_out.l3a_vid = vlan_out;
    if (BCM_E_NONE != bcm_l3_intf_create(unit, &l3_intf_out)) { 
        printf("bcm_l3_intf_create failed \n");
        return -1;
    }

    /* Create L3 ingress interface */
    bcm_l3_ingress_t l3_ingress;
    bcm_l3_ingress_t_init(&l3_ingress);
    l3_ingress.flags = BCM_L3_INGRESS_WITH_ID;
    bcm_if_t ingress_if = 0x1b2;
    l3_ingress.vrf = vrf;
    if (BCM_E_NONE != bcm_l3_ingress_create(unit, &l3_ingress, &ingress_if)) { 
        printf("bcm_l3_ingress_create() failed \n");
        return -1;
    }

    /* Config vlan_id to l3_iif mapping */
    bcm_vlan_control_vlan_t vlan_ctrl;
    bcm_vlan_control_vlan_t_init(&vlan_ctrl);
    bcm_vlan_control_vlan_get(unit, vlan_in, &vlan_ctrl);
    vlan_ctrl.ingress_if = ingress_if;
    if (BCM_E_NONE != bcm_vlan_control_vlan_set(unit, vlan_in, vlan_ctrl)) {
        printf("bcm_vlan_control_vlan_set() failed \n");
        return -1;
    }

    /* Create L3 egress object */
    bcm_l3_egress_t l3_egress;
    bcm_if_t egr_obj_id;
    bcm_l3_egress_t_init(&l3_egress);
    sal_memcpy(l3_egress.mac_addr, nh_mac, sizeof(nh_mac));
    l3_egress.port = egr_port;
    l3_egress.intf = l3_intf_out.l3a_intf_id;
    if (BCM_E_NONE != bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj_id)) {
        printf("bcm_l3_egress_create() failed \n");
        return -1;
    }

    /* Install the route for DIP */
    bcm_l3_route_t route_info;
    bcm_l3_route_t_init(&route_info);
    route_info.l3a_intf = egr_obj_id;
    route_info.l3a_subnet = dip;
    route_info.l3a_ip_mask = mask;
    route_info.l3a_vrf = vrf;
    if (BCM_E_NONE !=  bcm_l3_route_add(unit, &route_info)) {
        printf("bcm_l3_route_add() failed for port %d\n", ing_port);
        return -1;
    }

    /* Config my station */
    bcm_l2_station_t l2_station;
    int station_id;
    bcm_l2_station_t_init(&l2_station);
    sal_memcpy(l2_station.dst_mac, router_mac_in, sizeof(router_mac_in));
    l2_station.dst_mac_mask = "ff:ff:ff:ff:ff:ff";
    l2_station.flags = BCM_L2_STATION_IPV4 | BCM_L2_STATION_IPV6;
    if (BCM_E_NONE != bcm_l2_station_add(unit, &station_id, &l2_station)) {
        printf("bcm_l2_station_add() failed for port %d\n", ing_port);
        return -1;
    }

    if (BCM_E_NONE != flex_ctr_setup(unit, &route_info)) {
        printf("flex_ctr_setup() failed \n");
        return -1;
    }

    return BCM_E_NONE;
}

bcm_error_t l3_data_path_setup(int unit)
{
    bcm_pbmp_t pbmp, ubmp;

    /* Create ingress vlan and add port to vlan */
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, ing_port);
    if (BCM_E_NONE !=  bcm_vlan_create(unit, vlan_in)) { 
        printf("bcm_vlan_create() failed \n");
        return -1;
    }
    if (BCM_E_NONE != bcm_vlan_port_add(unit, vlan_in, pbmp, ubmp)) { 
        printf("bcm_vlan_port_add() failed \n");
        return -1;
    }

    /* Create egress vlan and add port to vlan */
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, egr_port);
    BCM_PBMP_PORT_ADD(pbmp, 0); /* Added CPU port to same vlan. */
    if (BCM_E_NONE !=  bcm_vlan_create(unit, vlan_out)) {
        printf("bcm_vlan_create() failed \n");
        return -1;
    }
    if (BCM_E_NONE != bcm_vlan_port_add(unit, vlan_out, pbmp, ubmp)) {
        printf("bcm_vlan_port_add() failed \n");
        return -1;
    }

    /* Set the default VLAN for the port */
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, ing_port, vlan_in));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egr_port, vlan_out));

    if (BCM_E_NONE != l3_route_uc_create(unit)) {
        printf("l3_route_uc_create() failed for port %d\n", ing_port);
        return -1;
    }

    return BCM_E_NONE;
}

bcm_error_t test_setup(int unit)
{
    int port_list[2];
    bcm_pbmp_t  pbmp;

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 2)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }
    ing_port = port_list[0];
    egr_port = port_list[1];

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, ing_port);
    if (BCM_E_NONE != ingress_port_multi_setup(unit, pbmp)) {
        printf("ingress_port_multi_setup() failed for port %d\n", ing_port);
        return -1;
    }

    if (BCM_E_NONE != bcm_port_control_set(unit, ing_port, bcmPortControlIP4, 1)) {
        printf("bcm_port_control_set() failed for port %d\n", ing_port);
        return -1;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, egr_port);
    if (BCM_E_NONE != egress_port_multi_setup(unit, pbmp)) {
        printf("egress_port_multi_setup() failed for port %d\n", egr_port);
        return -1;
    }

    bshell(unit, "pw start report +raw +decode");

    if (BCM_FAILURE((rv = registerPktioRxCallback(unit)))){
        printf("registerPktioRxCallback() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    if (BCM_FAILURE((rv = l3_data_path_setup(unit)))) {
        printf("l3_data_path_setup failed. : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}


bcm_error_t
set_responsive_protocol_indication(int unit)
{ 
    uint8 tcp_protocol_resp = 6; /* IP header Protocol field that gets programmed as Responsive 6=TCP */
    int responsive_indicator = 1;

    rv = bcm_ecn_responsive_protocol_set(unit, tcp_protocol_resp, responsive_indicator);
    if (rv != BCM_E_NONE) { 
        printf("bcm_ecn_responsive_protocol_set~Resp-protocol returned '%s'\n", bcm_errmsg(rv));
    }

    if (debug){
        bshell(0, "bsh -c 'lt ECN_PROTOCOL traverse -l'");
    }

    return BCM_E_NONE;
}

bcm_error_t ip_to_int_cn_mapping(int unit)
{
    bcm_ecn_map_profile_t map_profile_info;
    bcm_ecn_traffic_map_info_t map_info;
    uint32 flags;
    int profile_id, rv;

    /* Select mapping profile 1 for internal priority. */
    bcm_ecn_map_profile_t_init(&map_profile_info);
    map_profile_info.type = bcmEcnMapProfileIntCnPostForward;
    map_profile_info.int_pri = 0;
    map_profile_info.profile_id = 1;
    rv = bcm_ecn_map_profile_set(unit, &map_profile_info);
    if (rv != BCM_E_NONE) {
        printf("bcm_ecn_map_profile_set returned '%s'\n", bcm_errmsg(rv));
        return rv;
    }
    /* For packets with ECN, set internal congestion notification. */
    bcm_ecn_traffic_map_info_t_init(&map_info);
    map_info.flags = BCM_ECN_TRAFFIC_MAP_RESPONSIVE;
    map_info.type = bcmEcnTrafficMapTypeIngressPostForward;
    map_info.profile_id = 1;
    map_info.ecn = iphdr_ecn;
    map_info.int_cn = resp_mapped_int_cn;
    rv = bcm_ecn_traffic_map_set(unit, &map_info);
    if (rv != BCM_E_NONE) {
        printf("bcm_ecn_traffic_map_set returned '%s'\n", bcm_errmsg(rv));
        return rv;
    }

    if (debug) {
        bshell(0, "bsh -c 'lt ECN_INT_PRI_TO_CNG_POST traverse -l'");
        bshell(0, "bsh -c 'lt ECN_IP_TO_CNG_POST traverse -l'");
    }

    return BCM_E_NONE;
}

bcm_error_t int_cn_to_mmuif_mapping(int unit)
{
    bcm_ecn_traffic_action_config_t config;

    bcm_ecn_traffic_action_config_t_init(&config);
    config.action_type = BCM_ECN_TRAFFIC_ACTION_TYPE_ENQUEUE;
    config.int_cn = resp_mapped_int_cn;  /* Responsive */
    config.action_flags = BCM_ECN_TRAFFIC_ACTION_ENQUEUE_WRED_RESPONSIVE |
                          BCM_ECN_TRAFFIC_ACTION_ENQUEUE_MARK_ELIGIBLE;
    /* For the above INT_CN value, enable both Responsive drop and ECN Marking */
    rv = bcm_ecn_traffic_action_config_set(unit, &config);
    if (rv != BCM_E_NONE) {
        printf("bcm_ecn_traffic_action_config_set:Enqueue~Resp-protocol returned '%s'\n", bcm_errmsg(rv));
        return rv;
    }
    if (debug){
        bshell(0, "bsh -c 'lt ECN_CNG_TO_WRED traverse -l'");
    }

    return BCM_E_NONE;
}

bcm_error_t mmu_wred_ecn_setup(int unit) 
{
    bcm_cos_t cos = 0; /* COS Queue of Egress port on which WRED is configured */
    bcm_cosq_gport_discard_t discard_cfg; 
    int CELL_SIZE = 254;

    bcm_cosq_gport_discard_t_init(&discard_cfg);
    discard_cfg.flags =
         BCM_COSQ_DISCARD_ENABLE |          /* WRED_EN=1 */
         BCM_COSQ_DISCARD_MARK_CONGESTION | /* ECN_MARKING_EN=1 */
         BCM_COSQ_DISCARD_BYTES |
         BCM_COSQ_DISCARD_COLOR_GREEN |     /* Modify only GREEN Drop curve Profiles-0 or 3 or 6*/
         BCM_COSQ_DISCARD_RESPONSIVE_DROP | /* Responsive drop profile - 0 */
         BCM_COSQ_DISCARD_ECT_MARKED;       /* Marking profile - 6 */
    discard_cfg.min_thresh = 1* CELL_SIZE;  /* Queue depth in bytes to begin dropping. Test ONLY */
    discard_cfg.max_thresh = 4 * CELL_SIZE; /* Queue depth in bytes to drop all packets Test ONLY */
    discard_cfg.drop_probability = 90;      /* Drop probability at max threshold */
    rv = bcm_cosq_gport_discard_set(unit, egr_port, cos, &discard_cfg);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_discard_set~Resp-protocol returned '%s'\n", bcm_errmsg(rv));
        return rv;
    }
    if (debug){
        bshell(0, "bsh -c 'lt TM_WRED_UC_Q traverse -l'");
        bshell(0, "bsh -c 'lt TM_WRED_DROP_CURVE_SET_PROFILE traverse -l'");
        bshell(0, "bsh -c 'lt TM_WRED_TIME_PROFILE traverse -l'");
    }

    return BCM_E_NONE;
}

bcm_error_t setup_latency_ecn_profile_port_queue(int unit)
{
    uint32 flags;
    int l_ecn_map_id, rv;
    bcm_ecn_map_t l_ecn_map;
    bcm_ecn_port_map_t l_ecn_port_map;

    flags = BCM_ECN_MAP_EGRESS | BCM_ECN_MAP_LATENCY_ECN;
    rv = bcm_ecn_map_create(unit, flags, &l_ecn_map_id);
    if (rv != BCM_E_NONE) {
        printf("bcm_ecn_map_create returned '%s'\n", bcm_errmsg(rv));
        return rv;
    }

    /* Configure the Profile */
    bcm_ecn_map_t_init(&l_ecn_map);

    l_ecn_map.mmu_queue_id = 0;
    l_ecn_map.latency_ecn_en = 1;
    l_ecn_map.latency_ecn_threshold = 0;
    rv = bcm_ecn_map_set(unit, 0, l_ecn_map_id, &l_ecn_map);
    if (rv != BCM_E_NONE) {
        printf("bcm_ecn_map_set returned '%s'\n", bcm_errmsg(rv));
        return rv;
    }

    /* Map the profile to port */
    bcm_ecn_port_map_t_init(&l_ecn_port_map);

    l_ecn_port_map.flags = BCM_ECN_EGRESS_PORT_LATENCY_ECN_MAP;
    l_ecn_port_map.ecn_map_id = l_ecn_map_id;
    rv = bcm_ecn_port_map_set(unit, egr_port, l_ecn_port_map);
    if (rv != BCM_E_NONE) {
        printf("bcm_ecn_port_map_set returned '%s'\n", bcm_errmsg(rv));
        return rv;
    }
    if (debug){
        bshell(0, "bsh -c 'lt ECN_LATENCY_PROFILE traverse -l'");
        bshell(0, "bsh -c 'lt PORT_ECN traverse -l'");
    }

    return BCM_E_NONE;
}

bcm_error_t egr_int_cn_latency_cn_update(int unit)
{
    bcm_ecn_traffic_action_config_t l_ecn_act_cfg;

    bcm_ecn_traffic_action_config_t_init(&l_ecn_act_cfg);

    l_ecn_act_cfg.action_type = BCM_ECN_TRAFFIC_ACTION_TYPE_LATENCY_ECN_DEQUEUE;
    l_ecn_act_cfg.int_cn = resp_mapped_int_cn; 
    l_ecn_act_cfg.color = bcmColorGreen;
    l_ecn_act_cfg.buffer_high_thd_exceeded = 0; /* Test Purpose ONLY*/
    l_ecn_act_cfg.buffer_low_thd_exceeded = 0;/* Test Purpose ONLY */
    l_ecn_act_cfg.latency_thd_exceeded = 1;    
    l_ecn_act_cfg.action_flags = BCM_ECN_TRAFFIC_ACTION_DEQUEUE_LATENCY_INT_CN_UPDATE;
    l_ecn_act_cfg.latency_int_cn = cong_int_cn;
    l_ecn_act_cfg.congestion_marked = 1;
    l_ecn_act_cfg.responsive = 1;
    rv = bcm_ecn_traffic_action_config_set(unit, &l_ecn_act_cfg);
    if (rv != BCM_E_NONE) { 
        printf("[bcm_ecn_traffic_action_config_set:Dequeue]~Resp-protocol ERR '%s'\n", bcm_errmsg(rv));
        return rv;
    }

    if (debug){
        bshell(0, "bsh -c 'lt ECN_LATENCY_WRED_UPDATE traverse -l'");
    }

    return BCM_E_NONE;
}

bcm_error_t egr_ecn_mark(int unit)
{
    bcm_ecn_traffic_action_config_t config;

    bcm_ecn_traffic_action_config_t_init(&config);
    config.action_type  = BCM_ECN_TRAFFIC_ACTION_TYPE_EGRESS;
    config.int_cn       = cong_int_cn; /* INT_CN corresponding to congestion */
    config.ecn          = iphdr_ecn;
    config.action_flags = BCM_ECN_TRAFFIC_ACTION_EGRESS_ECN_MARKING;
    config.new_ecn = new_outgoing_cong_ecn; /* ECN bits in outgoing packets if congestion is present */
    rv = bcm_ecn_traffic_action_config_set(unit, &config);
    if (rv != BCM_E_NONE) { 
        printf("bcm_ecn_traffic_action_config_set:Egress~Congession returned '%s'\n", bcm_errmsg(rv));
        return rv;
    }

    bcm_ecn_traffic_action_config_t_init(&config);
    config.action_type  = BCM_ECN_TRAFFIC_ACTION_TYPE_EGRESS;
    config.int_cn       = non_cong_int_cn; /* INT_CN corresponding to 'no congestion' */
    config.ecn          = iphdr_ecn;
    config.action_flags = BCM_ECN_TRAFFIC_ACTION_EGRESS_ECN_MARKING;
    config.new_ecn      = new_outgoing_noncong_ecn; /* ECN bits in outgoing packets if congestion
                             is NOT present. (may retain original ECN bits here too for simplicity) */
    rv = bcm_ecn_traffic_action_config_set(unit, &config);
    if (rv != BCM_E_NONE) {
        printf("bcm_ecn_traffic_action_config_set:Egress~NonCongession returned '%s'\n", bcm_errmsg(rv));
        return rv;
    }
    if (debug){
        bshell(0, "bsh -c 'lt ECN_CNG_TO_IP_ECN traverse -l'");
    }

    return BCM_E_NONE;
}

bcm_error_t wred_ecn_config(int unit)
{
    bcm_error_t rv;

    if (BCM_FAILURE((rv = set_responsive_protocol_indication(unit)))) {
        printf("set_responsive_protocol_indication() failed. : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    if (BCM_FAILURE((rv = ip_to_int_cn_mapping(unit)))) {
        printf("ip_to_int_cn_mapping() failed. : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    if (BCM_FAILURE((rv = int_cn_to_mmuif_mapping(unit)))) {
        printf("int_cn_to_mmuif_mapping() failed. : %s\n", bcm_errmsg(rv));
        return rv;
    }

    if (BCM_FAILURE((rv = mmu_wred_ecn_setup(unit)))) {
        printf("wred_ecn_setup() failed. : %s\n", bcm_errmsg(rv));
        return rv;
    }

    if (BCM_FAILURE((rv = setup_latency_ecn_profile_port_queue(unit)))) {
        printf("setup_latency_ecn_profile_port_queue() failed. : %s\n", bcm_errmsg(rv));
        return rv;
    }

    if (BCM_FAILURE((rv = egr_int_cn_latency_cn_update(unit)))) {
        printf("egr_int_cn_latency_cn_update() failed. : %s\n", bcm_errmsg(rv));
        return rv;
    }

    if (BCM_FAILURE((rv = egr_ecn_mark(unit)))) {
        printf("bcm_mark_ecn() failed. : %s\n", bcm_errmsg(rv));
        return rv; 
    }

    return BCM_E_NONE;
}

/*Verify function call*/
bcm_error_t
test_verify(int unit)
{
    uint64 pkt_count;
    char str[512];
    int rv;

    bshell(unit, "clear c");
    printf("\nSending unicast packet to ingress port:%d\n", ing_port);
    /* DA=0xbb:bb:bb, VID=3, DIP=0xc0a80064, IP Prot=TCP */
    snprintf(str, 512, "tx %d pbm=%d data=0x000000BBBBBB0000000000028100000308004502002E000040000A06EDAFC0A80164C0A8006481815151B1B2B3B4A1A2A3A4500F01F543CE00001112131415161718191A; sleep quiet 1", 1, ing_port);
    bshell(unit, str);
    bshell(unit, "sleep quiet 1");

    printf("\n------------------------------------- \n");
    printf("Result of packet verification : ");
    printf("[%s]", (test_failed == 0)?"PASS":"FAIL");
    printf("\n------------------------------------- \n");

    /* Check the snmpBcmTxEcnPkts count */
    /*
    BCM_IF_ERROR_RETURN(bcm_stat_sync_get(unit, egr_port, snmpBcmTxEcnPkts, &pkt_count));
    printf("Counter snmpBcmTxEcnPkts : %d-%d\n", COMPILER_64_HI(pkt_count), COMPILER_64_LO(pkt_count));
    */

    /* Check flex counter */
    BCM_IF_ERROR_RETURN(flex_ctr_check(unit));
    printf("\n------------------------------------- \n");
    printf("Result of FlexCTR verification : PASS");
    printf("\n------------------------------------- \n");

    if(test_failed ==0) {
       return BCM_E_NONE;
    } else {
       return BCM_E_FAIL;
    }
}

/*
 * Clean up pre-test setup.
 */
int test_cleanup(int unit)
{
    printf("==>Cleaning up...\n");

    unregisterPktioRxCallback(unit);

    /* Remove the per queue flex counter */
    if (BCM_E_NONE != flex_ctr_cleanup(unit)) {
        printf("flex_ctr_cleanup() failed.\n");
        return -1;
    }

    return BCM_E_NONE;
}

bcm_error_t execute()
{
    bcm_error_t rv;

    bshell(unit, "config show; attach; version");

    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return -1;
    }
 
    if (BCM_FAILURE((rv = wred_ecn_config(unit)))) {
        printf("wred_ecn_config() failed.\n");
        return -1;
    }

    if (BCM_FAILURE((rv = test_verify(unit)))) {
        printf("test_verify() failed.\n");
        return -1;
    }
 
    if (BCM_FAILURE((rv = test_cleanup(unit)))) {
        printf("test_cleanup() failed.\n");
        return -1;
    }
 
    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
    print execute();
}

