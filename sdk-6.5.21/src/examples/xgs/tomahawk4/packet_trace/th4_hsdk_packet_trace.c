/*  Feature  : packet trace
 *
 *  Usage    : BCM.0> cint th4_hsdk_packet_trace.c
 *
 *  config   : bcm56990-generic-l3mpls.config.yml
 *
 *  Log file : th4_hsdk_packet_trace_log.txt
 *
 *  Test Topology :
 *                  +--------------------------+
 *                  |                          |
 *                  |                          | ecmp member 0 +---> trunk G1 member 0
 *                  |                          +---------------|
 *     ingress_port |                          |               +---> trunk G1 member 1
 * +----------------+          SWITCH          |
 *                  |                          |
 *                  |                          | ecmp member 1 +---> trunk G2 member 0
 *                  |                          +---------------|
 *                  |                          |               +---> trunk G2 member 1
 *                  |                          |
 *                  +--------------------------+
 *
 *  This script can verify below L3 APIs:
 *  ========
 *   bcm_l3_route_add()
 *   bcm_l3_ecmp_create()
 *   bcm_trunk_create()/bcm_trunk_set()
 *   bcm_mpls_tunnel_initiator_set()
 *   bcm_pktio_trace_data_collect()
 *   bcm_pktio_trace_field_get()
 *   bcm_pktio_trace_drop_reasons_get()
 *   bcm_pktio_trace_counter_get()
 *
 *
 *
 *  Summary:
 *  ========
 *  This cint example demonstrate packet trace feature by leveraging L3 MPLS tunnel initiation to ECMP group
 *  with trunk group as the data path.
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Create vlan and add port to vlan. Enable L3 feature per port.
 *
 *    2) Step2 - Configuration (Done in config_l3_mpls_init_ecmp_trunk())
 *    ======================================================
 *      a) Create egress objects used for unicast route destionation.
 *      b) Create an ecmp grpup.
 *      b) Create a l3 route to that ecmp group.
 *
 *    3) Step3 - Verification (Done in test_verify())
 *    ==========================================
 *      a) Verify the result after send a packet.
 *         ingress_port:
 *         {000000000001} {002a10777700} 8100 000b
 *         0800 4500 003c 6762 0000 ff01 bd76 0a3a
 *         4001 0a3a 4273 0000 550a 0001 0051 6162
 *         6364 6566 6768 696a 6b6c 6d6e 6f70 7172
 *         7374 7576 7761 6263 6465 6667 6869
 *      b) Expected Result:
 *         ecmp member[0], trunk group 0:
 *         Packet raw data (86):
 *         00000000fe02} {000000000002} 8100 000c
 *         847 0567 82fe 0020 01fe 4500 003c 6762 // two mpls labels: 0x5678, 0x200
 *         000 fe01 be76 0a3a 4001 0a3a 4273 0000
 *         50a 0001 0051 6162 6364 6566 6768 696a
 *         b6c 6d6e 6f70 7172 7374 7576 7761 6263
 *         465 6667 6869
 *
 *         ecmp member[1], trunk group 1:
 *         {00000000fe02} {000000000202} 8100 0016
 *         8847 0567 92fe 0020 11fe 4500 003c 6762 // two mpls labels: 0x5679, 0x201
 *         0000 fe01 be76 0a3a 4001 0a3a 4273 0000
 *         550a 0001 0051 6162 6364 6566 6768 696a
 *         6b6c 6d6e 6f70 7172 7374 7576 7761 6263
 *         6465 6667 6869
 *      c) Configuring packet trace, sending the test packet to ingress_port and
 *         collecting the packet trace data.
 *      d) Decoding the packet trace data fields, print it out which match the test
 *         packet data path, and dumping the raw packet trace data.
 *         FIELDS:
 *         	MY_STATION_HIT                  :1
 *         	FP_AND_PBM_ACTION_VALID         :0
 *         	FP_REPLACE_PBM_ACTION_VALID     :0
 *         	FP_OR_PBM_ACTION_VALID          :0
 *         	POST_VXLT_TAG_STATUS            :2
 *         	INCOMING_TAG_STATUS             :TAGGED
 *         	FORWARDING_FIELD                :1
 *         	FORWARDING_TYPE                 :VRF
 *         	EGRESS_MASK_TABLE_INDEX         :1
 *         	VLAN_MASK_INDEX                 :0
 *         	MAC_BLOCK_INDEX                 :0
 *         	NONUC_TRUNK_BLOCK_MASK_INDEX    :192
 *         	DGPP                            :17
 *         	NHOP                            :2
 *         	LAG_OFFSET                      :1
 *         	HASH_VALUE_LAG                  :445
 *         	LAG_ID                          :2
 *         	LAG_RESOLUTION_DONE             :1
 *         	ECMP_HASH_VALUE2                :50393
 *         	ECMP_OFFSET2                    :1
 *         	ECMP_GROUP2                     :1
 *         	ECMP_RESOLUTION_DONE2           :1
 *         	ECMP_HASH_VALUE1                :0
 *         	ECMP_OFFSET1                    :0
 *         	ECMP_GROUP1                     :0
 *         	ECMP_RESOLUTION_DONE1           :0
 *         	FWD_DESTINATION_FIELD           :1
 *         	FWD_DESTINATION_TYPE            :ECMP
 *         	PKT_RESOLUTION_VECTOR           :KNOWN_L3UC_PKT
 *         	VALID_VLAN_ID                   :1
 *         	INGRESS_STG_STATE               :4
 *         	DOS_ATTACK                      :0
 *         	L3_TUNNEL_HIT                   :0
 *         	MPLS_ENTRY_TABLE_LOOKUP_0_HIT   :0
 *         	MPLS_ENTRY_TABLE_LOOKUP_1_HIT   :0
 *         	MPLS_BOS_TERMINATED             :0
 *         	L2LU_SRC_INDEX                  :1988
 *         	L2LU_SRC_KEY_TYPE               :0
 *         	L2LU_SRC_HIT                    :1
 *         	L2LU_DST_IDX                    :0
 *         	L2LU_DST_KEY_TYPE               :0
 *         	L2LU_DST_HIT                    :0
 *         	MPLS_ENTRY_INDEX_2              :0
 *         	MPLS_ENTRY_KEY_TYPE_2           :0
 *         	MPLS_ENTRY_INDEX_1              :0
 *         	MPLS_ENTRY_KEY_TYPE_1           :0
 *         	MPLS_ENTRY_HIT_1                :0
 *         	MPLS_ENTRY_HIT_2                :0
 *         	MY_STATION_INDEX                :0
 *         	ENTROPY                         :0
 *         	L3_TUNNEL_HIT_1                 :0
 *         	L3_TUNNEL_INDEX_1               :0
 *         	L3_TUNNEL_KEY_TYPE_1            :0
 *         	COMP_DST_HIT                    :0
 *         	COMP_DST_HIT_INDEX              :0
 *         	COMP_DST_HIT_TABLE              :0
 *         	COMP_DST_SUB_DB_PRIORITY        :0
 *         	COMP_SRC_HIT                    :0
 *         	COMP_SRC_HIT_INDEX              :0
 *         	COMP_SRC_HIT_TABLE              :0
 *         	COMP_SRC_SUB_DB_PRIORITY        :0
 *         	LPM_DST_HIT                     :1
 *         	LPM_DST_HIT_INDEX               :24575
 *         	LPM_DST_HIT_TABLE               :2
 *         	LPM_DST_SUB_DB_PRIORITY         :1
 *         	LPM_SRC_HIT                     :0
 *         	LPM_SRC_HIT_INDEX               :0
 *         	LPM_SRC_HIT_TABLE               :0
 *         	LPM_SRC_SUB_DB_PRIORITY         :0
 *         	L3_TUNNEL_TCAM_HIT              :0
 *         	L3_TUNNEL_TCAM_INDEX            :0
 *         	DVP                             :0
 *         	SVP                             :0
 *         	INNER_L2_OUTER_TAGGED           :0
 *         	O_NEXT_HOP                      :0
 *         	VFI                             :11
 *         	MY_STATION_2_HIT                :1
 *         	MY_STATION_2_INDEX              :1
 *
 *         DROP_REASON:
 *
 *         COUNTER:
 *         	RIPC4
 *         	RUC
 *
 *         DATA:
 *         [0000]: 00 00 00 00 30 10 00 00 - 00 00 00 00 00 00 00 00
 *         [0010]: 00 00 00 00 00 00 00 00 - 07 00 00 00 00 00 80 c4
 *         [0020]: 00 00 00 00 ff bf 80 01 - 00 00 00 00 00 00 00 00
 *         [0030]: 00 00 00 00 00 00 2c 00 - 00 80 20 00 00 00 00 40
 *         [0040]: 00 00 00 01 00 10 00 00 - de 04 82 00 d9 c4 01 80
 *         [0050]: 00 30 00 00 11 02 00 01 - c0 05 00 00 01 a0 10 00
 *         [0060]: 00 00 00 00 00 00 00 00 - 80 00 00 00 00 40 00 00
 *
 *
 *    4) Step4 - The l3 mpls init teardown configuration(Done in test_cleanup())
 *    ==========================================
 *      a) Mostly reverse the configuration, done in clear_l3_mpls_init_ecmp_trunk()
 */
cint_reset();

int unit = 0;
bcm_error_t rv = BCM_E_NONE;
bcm_port_t ingress_port;
bcm_port_t egress_port[4];
bcm_vlan_t in_vlan = 11, out_vlan = 12, out_vlan2 = 22;
bcm_vrf_t vrf = 1;
bcm_trunk_t trunk_id = 1, trunk_id2 = 2;
bcm_if_t ecmp_group_id;
int my_station_id;
bcm_mac_t router_mac_in = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
bcm_mac_t router_mac_out = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
bcm_mac_t router_mac_out2 = {0x00, 0x00, 0x00, 0x00, 0x02, 0x02};
bcm_mac_t dst_mac = {0x00, 0x00, 0x00, 0x00, 0xfe, 0x02};
bcm_ip_t dip = 0x0a3a4273;
bcm_ip_t mask = 0xffffff00;
bcm_field_group_t ifp_group;
bcm_field_entry_t ifp_entry[4];
uint32_t stat_counter_id;


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
 * Configures the port in loopback mode
 */
bcm_error_t
ingress_port_setup(int unit, bcm_port_t port)
{
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_NONE));

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
egress_port_setup(int unit, int port_count, bcm_port_t *port)
{
	int i=0;
    bcm_field_entry_t entry;
    bcm_field_group_config_t group_config;

    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDrop);
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));
	ifp_group = group_config.group;
	print ifp_group;

    for(i=0;i<port_count; i++){
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));
	ifp_entry[i] = entry;
	print ifp_entry[i];
    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, *port, BCM_FIELD_EXACT_MATCH_MASK));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));
    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, *port, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, *port, BCM_PORT_DISCARD_ALL));
	port++;
	}

    return BCM_E_NONE;
}

bcm_error_t clear_field_resource(int unit, bcm_field_group_t group, bcm_field_entry_t *entry, int entry_num)
{
	bcm_error_t rv = BCM_E_NONE;
	int i;

	for(i=0; i<entry_num; i++){
    /* Remove field entry from hardware table */
    rv = bcm_field_entry_remove(unit, entry[i]);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_entry_remove() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Destroy the field entry */
    rv = bcm_field_entry_destroy(unit, entry[i]);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_entry_destroy() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }
	}

    /* Destroy the field group */
    rv = bcm_field_group_destroy(unit, group);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_group_destroy() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/*
 * Test Setup:
 * This functions gets the port numbers and sets up ingress and
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
bcm_error_t test_setup(int unit)
{
	bcm_error_t rv = BCM_E_NONE;
    int port_list[5], i;

    rv = portNumbersGet(unit, port_list, 5);
    if (BCM_FAILURE(rv)) {
        printf("portNumbersGet() failed\n");
        return rv;
    }

    ingress_port = port_list[0];
	print ingress_port;
	for (i=0;i<4;i++){
        egress_port[i] = port_list[i+1];
        print egress_port[i];
	}

    printf("setting ingress port:%d MAC loopback\n", ingress_port);
    rv = ingress_port_setup(unit, ingress_port);
    if (BCM_FAILURE(rv)) {
        printf("ingress_port_setup() failed for port %d\n", ingress_port);
        return rv;
    }

    printf("setting egress ports:%d,%d,%d,%d MAC loopback\n",
			egress_port[0], egress_port[1],
			egress_port[2], egress_port[3]);
    rv = egress_port_setup(unit, 4, egress_port);
    if (BCM_FAILURE(rv)) {
        printf("egress_port_setup() failed for port %d,%d,%d,%d\n",
			egress_port[0], egress_port[1],
			egress_port[2], egress_port[3]);
        return rv;
    }

    /* Enable IPv4 on ingress port */
    print bcm_port_control_set(unit, ingress_port, bcmPortControlIP4, TRUE);

    /*bshell(unit, "pw start report +raw +decode");*/

	return BCM_E_NONE;
}

/*Expected Egress packets */
unsigned char expected_egress_packet[86] = {
0x00, 0x00, 0x00, 0x00, 0xfe, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x81, 0x00, 0x00, 0x0c,
0x88, 0x47, 0x05, 0x67, 0x82, 0xfe, 0x00, 0x20, 0x01, 0xfe, 0x45, 0x00, 0x00, 0x3c, 0x67, 0x62,
0x00, 0x00, 0xfe, 0x01, 0xbe, 0x76, 0x0a, 0x3a, 0x40, 0x01, 0x0a, 0x3a, 0x42, 0x73, 0x00, 0x00,
0x55, 0x0a, 0x00, 0x01, 0x00, 0x51, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a,
0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x61, 0x62, 0x63,
0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
};

unsigned char expected_egress_packet2[86] = {
0x00, 0x00, 0x00, 0x00, 0xfe, 0x02, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x81, 0x00, 0x00, 0x16,
0x88, 0x47, 0x05, 0x67, 0x92, 0xfe, 0x00, 0x20, 0x11, 0xfe, 0x45, 0x00, 0x00, 0x3c, 0x67, 0x62,
0x00, 0x00, 0xfe, 0x01, 0xbe, 0x76, 0x0a, 0x3a, 0x40, 0x01, 0x0a, 0x3a, 0x42, 0x73, 0x00, 0x00,
0x55, 0x0a, 0x00, 0x01, 0x00, 0x51, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a,
0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x61, 0x62, 0x63,
0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
};

int test_failed = 0; /*Final result will be stored here */
int rx_count;   /* Global received packet count */
uint32 test_pkt_num = 2;
unsigned char *expected_packets[test_pkt_num];
const uint8 priority = 100;

/* Rx callback function for applications using the HSDK PKTIO */
bcm_pktio_rx_t
pktioRxCallback(int unit, bcm_pktio_pkt_t * packet, void *cookie)
{
    int                *count = (auto) cookie;
    void               *buffer;
    uint32              length;
    uint32              port;

	(*count)++; /* Bump received packet count */

    /* Get basic packet info */
    if (BCM_FAILURE(bcm_pktio_pkt_data_get(unit, packet, (void *) &buffer, &length))) {
        return BCM_PKTIO_RX_NOT_HANDLED;
    }
    /* Get source port metadata */
    BCM_IF_ERROR_RETURN(bcm_pktio_pmd_field_get
                        (unit, packet, bcmPktioPmdTypeRx,
                         BCM_PKTIO_RXPMD_SRC_PORT_NUM, &port));

	if ((egress_port[0] != port) && (egress_port[1] != port) && (egress_port[2] != port) && (egress_port[3] != port)){
   	    test_failed = 1;
        printf("pktioRxCallback: packet received from wrong port %d, expected to received from port %d, %d, %d, %d.\n",
				port, egress_port[0], egress_port[1], egress_port[2], egress_port[3]);
	} else if ((egress_port[0] == port)&&(sal_memcmp(buffer, expected_packets[0], length) != 0)){
   	    test_failed = 1;
        printf("pktioRxCallback: packet received from port %d as expected, but packet data not match\n", port);
	} else if ((egress_port[1] == port)&&(sal_memcmp(buffer, expected_packets[0], length) != 0)){
   	    test_failed = 1;
        printf("pktioRxCallback: packet received from port %d as expected, but packet data not match\n", port);
	} else if ((egress_port[2] == port)&&(sal_memcmp(buffer, expected_packets[1], length) != 0)){
   	    test_failed = 1;
        printf("pktioRxCallback: packet received from port %d as expected, but packet data not match\n", port);
	} else if ((egress_port[3] == port)&&(sal_memcmp(buffer, expected_packets[1], length) != 0)){
   	    test_failed = 1;
        printf("pktioRxCallback: packet received from port %d as expected, but packet data not match\n", port);
	}
    return BCM_PKTIO_RX_NOT_HANDLED;
}

/* Register callback function for received packets */
bcm_error_t
registerPktioRxCallback(int unit)
{
    const uint32        flags = 0;      /* Flags only used on match (for now) */

    return bcm_pktio_rx_register
      (unit, "rx_cb", pktioRxCallback, priority, &rx_count, flags);
}

bcm_error_t
unregisterPktioRxCallback(int unit)
{
    return bcm_pktio_rx_unregister(unit, pktioRxCallback, priority);
}

/* Create vlan and add port to vlan */
bcm_error_t
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
}

/* Destroy vlan and remove port from vlan */
bcm_error_t
vlan_destroy_remove_port(int unit, int vid, int port)
{
    bcm_pbmp_t pbmp;

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    BCM_IF_ERROR_RETURN(bcm_vlan_port_remove(unit, vid, pbmp));
    return bcm_vlan_destroy(unit, vid);
}

/* Destroy vlan and remove ports from vlan */
bcm_error_t
vlan_destroy_remove_ports(int unit, int vid, int count, int *ports)
{
    bcm_pbmp_t pbmp;
    int i;

    BCM_PBMP_CLEAR(pbmp);
    for (i = 0; i < count; i++) {
        BCM_PBMP_PORT_ADD(pbmp, ports[i]);
	}

    BCM_IF_ERROR_RETURN(bcm_vlan_port_remove(unit, vid, pbmp));
    return bcm_vlan_destroy(unit, vid);
}

/* Create vlan and add array of ports to vlan */
bcm_error_t
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
}

bcm_error_t
create_l3_ingress_interface(int unit, bcm_vrf_t vrf, bcm_if_t *ingress_if)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_l3_ingress_t l3_ingress;

    bcm_l3_ingress_t_init(&l3_ingress);
    if(*ingress_if)
        l3_ingress.flags = BCM_L3_INGRESS_WITH_ID;
    l3_ingress.vrf = vrf;
    rv = bcm_l3_ingress_create(unit, &l3_ingress, ingress_if);

    return rv;
}

bcm_error_t
clear_l3_ingress_interface(int unit, bcm_if_t intf_id)
{
    bcm_error_t rv = BCM_E_NONE;

	rv = bcm_l3_ingress_destroy(unit, intf_id);
    return rv;
}

bcm_error_t
create_l3_interface(int unit, int flags, bcm_mac_t local_mac, int vid, bcm_if_t *intf_id)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_l3_intf_t l3_intf;

    /* L3 interface and egress object for access */
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_flags |= flags;
    sal_memcpy(l3_intf.l3a_mac_addr, local_mac, sizeof(local_mac));
    l3_intf.l3a_vid = vid;
    l3_intf.l3a_intf_id = *intf_id;
    rv = bcm_l3_intf_create(unit, &l3_intf);
    if (BCM_FAILURE(rv)) {
        printf("bcm_l3_intf_create()%s\n", bcm_errmsg(rv));
        return rv;
    }
    *intf_id = l3_intf.l3a_intf_id;

    return rv;
}

bcm_error_t
clear_l3_interface(int unit, bcm_if_t intf_id)
{
    bcm_l3_intf_t l3_intf;
    bcm_error_t rv = BCM_E_NONE;

    bcm_l3_intf_t_init(&l3_intf);
	l3_intf.l3a_intf_id = intf_id;
	rv = bcm_l3_intf_get(unit, &l3_intf);
    if (BCM_FAILURE(rv)) {
        printf("bcm_l3_intf_get %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_l3_intf_delete(unit, &l3_intf);
    if (BCM_FAILURE(rv)) {
        printf("bcm_l3_intf_delete %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/* Create L3 egress object */
bcm_error_t
create_egr_obj_with_route_label(int unit, int l3_if, bcm_mac_t nh_mac, bcm_port_t port,
               bcm_mpls_label_t route_label, bcm_if_t *egr_obj_id)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_l3_egress_t l3_egress;

    bcm_l3_egress_t_init(&l3_egress);
    sal_memcpy(l3_egress.mac_addr,  nh_mac, sizeof(nh_mac));
    l3_egress.intf  = l3_if;
    l3_egress.port = port;
    l3_egress.flags = BCM_L3_ROUTE_LABEL;
    l3_egress.mpls_label = route_label;
    rv = bcm_l3_egress_create(unit, 0, &l3_egress, egr_obj_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_l3_egress_create() %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/* Clear L3 egress object */
bcm_error_t
clear_egr_obj(int unit, bcm_if_t egr_obj_id)
{
    return bcm_l3_egress_destroy(unit, egr_obj_id);
}

int config_l3_ecmp_flexctr(int unit, bcm_if_t ecmp_group_id, uint32 *stat_counter_id)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_flexctr_action_t action = {0};
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_a_op = NULL;
    bcm_flexctr_value_operation_t *value_b_op = NULL;
    int options = 0;

    action.index_num = 1;
    action.source = bcmFlexctrSourceEcmpUnderlay;
    action.hint = 0;
    action.mode = bcmFlexctrCounterModeNormal;

    /* Counter index is always zero. */
    index_op = &action.index_operation;
    index_op->object[0] = bcmFlexctrObjectConstZero;
    index_op->mask_size[0] = 16;
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

    rv = bcm_flexctr_action_create(unit, options, &action, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_action_create %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf("Created flexctr stat_counter_id = 0x%x.\n", *stat_counter_id);

    rv = bcm_l3_ecmp_stat_attach(unit, ecmp_group_id, *stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("ERROR: bcm_l3_ecmp_stat_attach() -  %s.\n", bcm_errmsg(rv));
    }
    printf("Flexctr stat_counter_id:0x%x attached to l3 ecmp group %d\n", *stat_counter_id, ecmp_group_id);

    return rv;
}

/* Config my_station_tcam to enable l3 lookup */
bcm_error_t enable_l3_lookup(int unit, bcm_mac_t router_mac_in, bcm_vlan_t in_vlan, int *station_id)
{
	bcm_error_t rv = BCM_E_NONE;
    bcm_l2_station_t l2_station;

    bcm_l2_station_t_init(&l2_station);
	l2_station.vlan = in_vlan;
	l2_station.vlan_mask = BCM_VLAN_MAX;
    sal_memcpy(l2_station.dst_mac, router_mac_in, sizeof(router_mac_in));
    l2_station.dst_mac_mask = "ff:ff:ff:ff:ff:ff";
    l2_station.flags = BCM_L2_STATION_IPV4 ;
    rv = bcm_l2_station_add(unit, station_id, &l2_station);
    if (BCM_FAILURE(rv)) {
        printf("bcm_l2_station_add() %s\n", bcm_errmsg(rv));
        return rv;
    }

	return rv;
}

/* Clear my_station_tcam entry to disable l3 looup*/
bcm_error_t disable_l3_lookup(int unit, int station_id)
{
	bcm_error_t rv = BCM_E_NONE;

    rv = bcm_l2_station_delete(unit, station_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_l2_station_delete() %s\n", bcm_errmsg(rv));
        return rv;
    }

	return rv;
}

bcm_error_t
check_flexctr(int unit, uint32 stat_counter_id, int *flexctr_check_failed)
{
    bcm_error_t rv = BCM_E_NONE;
    uint32 num_entries = 1;
    uint32 counter_index = 0;
    bcm_flexctr_counter_value_t counter_value;
    int expected_packet_length = 82*10;

    /* Get counter value. */
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

    printf("Flexctr Get : %d packets / %d bytes\n",
           COMPILER_64_LO(counter_value.value[0]),
           COMPILER_64_LO(counter_value.value[1]));

    if (COMPILER_64_LO(counter_value.value[0]) != 10) {
        printf("Flexctr packets verify failed. Expected packet %d but get %d\n", rx_count, COMPILER_64_LO(counter_value.value[0]));
        *flexctr_check_failed = 1;
        return BCM_E_FAIL;
    };

    if (COMPILER_64_LO(counter_value.value[1]) != expected_packet_length) {
        printf("Flexctr bytes verify failed. Expected bytes %d but get %d\n", expected_packet_length, COMPILER_64_LO(counter_value.value[1]));
        *flexctr_check_failed = 1;
        return BCM_E_FAIL;
    };

    return BCM_E_NONE;
}

/*
 * This function creates a trunk group and adds the member ports
 */
bcm_error_t trunk_create(int unit, bcm_trunk_t *tid, int count, bcm_port_t *member_ports)
{
    bcm_error_t rv = BCM_E_NONE;
    int i = 0;
    bcm_trunk_info_t trunk_info;
    bcm_trunk_member_t *member_info = NULL;
    bcm_trunk_member_t info;

    bcm_trunk_info_t_init(&trunk_info);
    /* Use randomized load balancing */
    trunk_info.psc = BCM_TRUNK_PSC_RANDOMIZED;
    trunk_info.dlf_index = BCM_TRUNK_UNSPEC_INDEX;
    trunk_info.mc_index = BCM_TRUNK_UNSPEC_INDEX;
    trunk_info.ipmc_index = BCM_TRUNK_UNSPEC_INDEX;
    rv = bcm_trunk_create(unit, BCM_TRUNK_FLAG_WITH_ID, tid);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_trunk_create %s.\n", bcm_errmsg (rv));
        return rv;
    }else {
        printf ("bcm_trunk_create trunk_id = %d.\n", *tid);
	}

    member_info = sal_alloc((sizeof(info) * count), "trunk members");
    for (i = 0 ; i < count ; i++) {
        bcm_trunk_member_t_init(&member_info[i]);
        BCM_GPORT_MODPORT_SET(member_info[i].gport, 0, member_ports[i]);
    }

    rv = bcm_trunk_set(unit, *tid, &trunk_info, count, member_info);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_trunk_set %s.\n", bcm_errmsg (rv));
        return rv;
    }

    return rv;
}

bcm_error_t
config_ecmp_spray(int unit, bcm_port_t port)
{
    bcm_error_t rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchECMPLevel1RandomSeed, 0xff));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchECMPLevel2RandomSeed, 0xf011));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, port,
                bcmPortControlECMPLevel1LoadBalancingRandomizer, 8));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, port,
                bcmPortControlECMPLevel2LoadBalancingRandomizer, 5));

    return rv;
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

    trunk_info.psc= BCM_TRUNK_PSC_RANDOMIZED;
    trunk_info.dlf_index= BCM_TRUNK_UNSPEC_INDEX;
    trunk_info.mc_index= BCM_TRUNK_UNSPEC_INDEX;
    trunk_info.ipmc_index= BCM_TRUNK_UNSPEC_INDEX;

    print *tid;
    print bcm_trunk_create(unit, BCM_TRUNK_FLAG_WITH_ID, *tid);
    print bcm_trunk_set(unit, *tid, &trunk_info, count, member_info);

    return BCM_E_NONE;
}

bcm_error_t
config_l3_mpls_init_ecmp_trunk(int unit)
{
	bcm_error_t rv = BCM_E_NONE;
    bcm_if_t ingress_if = 0x1b2;
	int flags = 0;
    bcm_if_t l3_intf_id, l3_intf_id2;
    bcm_if_t egr_obj_id, egr_obj_id2;
    int num_labels = 1;
    bcm_mpls_egress_label_t label_array[2], label_array2[2];
	bcm_mpls_label_t tunnel_label_init = 0x5678;
	bcm_mpls_label_t tunnel_label_init2 = 0x5679;
	bcm_mpls_label_t vc_label_init = 0x200;
	bcm_mpls_label_t vc_label_init2 = 0x201;
	uint8 exp = 1;
	uint8 ttl = 254;
    bcm_gport_t trunk_gp_id, trunk_gp2_id;
    bcm_port_t trunk_gp_ports[2], trunk_gp2_ports[2];


    /* Create and add ingress port to ingress VLAN */
    rv = vlan_create_add_port(unit, in_vlan, ingress_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vlan_create_add_port(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    trunk_gp_ports[0] = egress_port[0];
    trunk_gp_ports[1] = egress_port[1];
    /* Create and add egress ports to egress VLAN */
    rv = vlan_create_add_ports(unit, out_vlan, 2, trunk_gp_ports);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vlan_create_add_ports(): %s.\n",bcm_errmsg(rv));
        return rv;
    }
    rv = create_trunk(unit, &trunk_id, 2, trunk_gp_ports);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing create_trunk(): %s.\n", bcm_errmsg (rv));
        return rv;
    }
    printf("\nCreate trunk id:%d with member ports %d and %d\n", trunk_id, trunk_gp_ports[0], trunk_gp_ports[1]);
    BCM_GPORT_TRUNK_SET(trunk_gp_id, trunk_id);
    print trunk_gp_id;

    trunk_gp2_ports[0] = egress_port[2];
    trunk_gp2_ports[1] = egress_port[3];
    /* Create and add egress ports to egress VLAN */
    rv = vlan_create_add_ports(unit, out_vlan2, 2, trunk_gp2_ports);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vlan_create_add_ports(): %s.\n",bcm_errmsg(rv));
        return rv;
    }
    rv =create_trunk(unit, &trunk_id2, 2, trunk_gp2_ports);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing create_trunk(): %s.\n", bcm_errmsg (rv));
        return rv;
    }
    printf("\nCreate trunk id:%d with member ports %d and %d\n", trunk_id2, trunk_gp2_ports[0], trunk_gp2_ports[1]);
    BCM_GPORT_TRUNK_SET(trunk_gp2_id, trunk_id2);
    print trunk_gp2_id;

	/* Enable L3 Lookup for incoming l3 packet */
    rv = enable_l3_lookup(unit, router_mac_in, in_vlan, &my_station_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in enable_l3_lookup(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create L3 ingress interface */
    rv = create_l3_ingress_interface(unit, vrf, &ingress_if);
    if (BCM_FAILURE(rv)) {
        printf("\nError in create_l3_ingress_interface(): %s.\n",bcm_errmsg(rv));
        return rv;
    }
	print ingress_if;

    /* Config vlan_id to l3_iif mapping */
    bcm_vlan_control_vlan_t vlan_ctrl;
    bcm_vlan_control_vlan_t_init(&vlan_ctrl);
    bcm_vlan_control_vlan_get(unit, in_vlan, &vlan_ctrl);
    vlan_ctrl.ingress_if = ingress_if;
    print bcm_vlan_control_vlan_set(unit, in_vlan, vlan_ctrl);

    /* Create egress L3 interface */
    rv = create_l3_interface(unit, flags, router_mac_out, out_vlan, &l3_intf_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in create_l3_interface: %s.\n",bcm_errmsg(rv));
        return rv;
    }
    printf("l3 egr intf = %d\n", l3_intf_id);

    /* Create egress L3 interface */
    rv = create_l3_interface(unit, flags, router_mac_out2, out_vlan2, &l3_intf_id2);
    if (BCM_FAILURE(rv)) {
        printf("\nError in create_l3_interface: %s.\n",bcm_errmsg(rv));
        return rv;
    }
    printf("l3 egr intf2 = %d\n", l3_intf_id2);

    /* Create L3 egress object */
    rv = create_egr_obj_with_route_label(unit, l3_intf_id, dst_mac, trunk_gp_id, vc_label_init, &egr_obj_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in create_egr_obj_with_route_label(): %s.\n", bcm_errmsg(rv));
        return rv;
    }
    printf("l3 egr obj = %d\n", egr_obj_id);

    rv = create_egr_obj_with_route_label(unit, l3_intf_id2, dst_mac, trunk_gp2_id, vc_label_init2, &egr_obj_id2);
    if (BCM_FAILURE(rv)) {
        printf("\nError in create_egr_obj_with_route_label(): %s.\n", bcm_errmsg(rv));
        return rv;
    }
    printf("l3 egr obj2 = %d\n", egr_obj_id2);

    /* ECMP group */
    bcm_l3_egress_ecmp_t ecmp_info;
    bcm_l3_ecmp_member_t ecmp_member_array[2];

    /* Level 2 ECMP group for Tunnel 1 */
    bcm_l3_egress_ecmp_t_init(&ecmp_info);
    ecmp_info.ecmp_group_flags = BCM_L3_ECMP_UNDERLAY;
    ecmp_info.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RANDOM;
    ecmp_info.dynamic_size = 64;
    ecmp_info.max_paths = 64;

    bcm_l3_ecmp_member_t_init(&ecmp_member_array[0]);
    ecmp_member_array[0].egress_if = egr_obj_id;
    bcm_l3_ecmp_member_t_init(&ecmp_member_array[1]);
    ecmp_member_array[1].egress_if = egr_obj_id2;

    rv = bcm_l3_ecmp_create(unit, 0, &ecmp_info, 2, ecmp_member_array);
    if (BCM_FAILURE(rv)) {
       printf("Error executing bcm_l3_ecmp_create(): %s.\n", bcm_errmsg(rv));
       return rv;
    }
    ecmp_group_id = ecmp_info.ecmp_intf;
    print ecmp_group_id;

    /* Configure ECMP random number generator seed */
    rv = config_ecmp_spray(unit, ingress_port);
    if(BCM_FAILURE(rv)) {
        printf("\nError in config_ecmp_spray(): %s.\n",bcm_errmsg(rv));
        return rv;
    }

    bcm_l3_route_t route_info;
    /* Install the route for DIP */
    bcm_l3_route_t_init(&route_info);
	route_info.l3a_flags |= BCM_L3_MULTIPATH;
    route_info.l3a_intf = ecmp_group_id;
    route_info.l3a_subnet = dip;
    route_info.l3a_ip_mask = mask;
    route_info.l3a_vrf = vrf;
    rv = bcm_l3_route_add(unit, &route_info);
    if (BCM_FAILURE(rv)) {
        printf("Error executing bcm_l3_route_add(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

	bcm_mpls_egress_label_t_init(&label_array[0]);
	label_array[0].flags = BCM_MPLS_EGRESS_LABEL_TTL_SET | BCM_MPLS_EGRESS_LABEL_EXP_SET;
    label_array[0].label = tunnel_label_init;
	label_array[0].exp   = exp;
    label_array[0].ttl   = ttl;

    rv = bcm_mpls_tunnel_initiator_set(unit, l3_intf_id, num_labels, label_array);
    if (BCM_FAILURE(rv)) {
        printf("ERROR: bcm_mpls_tunnel_initiator_set() : %s.\n", bcm_errmsg(rv));
    }
	print tunnel_label_init;
	print exp;
	print ttl;

	bcm_mpls_egress_label_t_init(&label_array2[0]);
	label_array2[0].flags = BCM_MPLS_EGRESS_LABEL_TTL_SET | BCM_MPLS_EGRESS_LABEL_EXP_SET;
    label_array2[0].label = tunnel_label_init2;
	label_array2[0].exp   = exp;
    label_array2[0].ttl   = ttl;

    rv = bcm_mpls_tunnel_initiator_set(unit, l3_intf_id2, num_labels, label_array2);
    if (BCM_FAILURE(rv)) {
        printf("ERROR: bcm_mpls_tunnel_initiator_set() : %s.\n", bcm_errmsg(rv));
    }
	print tunnel_label_init2;
	print exp;
	print ttl;

	/* create flex ctr and attach to ECMP group */
	rv = config_l3_ecmp_flexctr(unit, ecmp_group_id, &stat_counter_id);
    if (BCM_FAILURE(rv)) {
       printf("ERROR: config_l3_ecmp_flexctr() : %s.\n", bcm_errmsg(rv));
       return rv;
    }

	return rv;
}

bcm_error_t clear_l3_mpls_init_ecmp_trunk(int unit)
{
	bcm_error_t rv = BCM_E_NONE;
    bcm_l3_route_t route_info;
	bcm_l3_egress_ecmp_t ecmp_info;
    bcm_l3_ecmp_member_t ecmp_member_array[2];
	int ecmp_member_count = 0;
	bcm_l3_egress_t egr;
    bcm_vlan_control_vlan_t vlan_ctrl;
	int i=0;

    bcm_l3_route_t_init(&route_info);
    route_info.l3a_subnet = dip;
    route_info.l3a_ip_mask = mask;
    route_info.l3a_vrf = vrf;
    rv = bcm_l3_route_get(unit, &route_info);
    if (BCM_FAILURE(rv)) {
       printf("Error in bcm_l3_route_get(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

    bcm_l3_egress_ecmp_t_init(&ecmp_info);
	ecmp_info.ecmp_intf = route_info.l3a_intf;
	bcm_l3_ecmp_member_t_init(&ecmp_member_array[1]);
    bcm_l3_ecmp_member_t_init(&ecmp_member_array[0]);

	rv = bcm_l3_ecmp_get(unit, &ecmp_info,
		sizeof(ecmp_member_array)/sizeof(ecmp_member_array[0]),
		ecmp_member_array, &ecmp_member_count);
    if (BCM_FAILURE(rv)) {
       printf("Error in bcm_l3_ecmp_get(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

	for(i=0;i<2;i++){
	rv = bcm_l3_egress_get(unit, ecmp_member_array[i].egress_if, &egr);
    if (BCM_FAILURE(rv)) {
       printf("Error in bcm_l3_egress_get(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

	rv = bcm_trunk_destroy(unit, egr.trunk);
    if (BCM_FAILURE(rv)) {
       printf("Error in bcm_trunk_destroy(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

    rv = bcm_mpls_tunnel_initiator_clear(unit, egr.intf);
    if (BCM_FAILURE(rv)) {
       printf("Error in bcm_mpls_tunnel_initiator_clear(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

	rv = clear_l3_interface(unit, egr.intf);
    if (BCM_FAILURE(rv)) {
       printf("Error in clear_l3_interface(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

	rv = clear_egr_obj(unit, ecmp_member_array[i].egress_if);
    if (BCM_FAILURE(rv)) {
       printf("Error in clear_egr_obj(): %s.\n", bcm_errmsg(rv));
       return rv;
    }
	}

	rv = bcm_l3_ecmp_destroy(unit, ecmp_info.ecmp_intf);
    if (BCM_FAILURE(rv)) {
       printf("Error in bcm_l3_ecmp_destroy(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

    /* Config vlan_id to l3_iif mapping */
    bcm_vlan_control_vlan_t_init(&vlan_ctrl);
    bcm_vlan_control_vlan_get(unit, in_vlan, &vlan_ctrl);
	rv = clear_l3_ingress_interface(unit, vlan_ctrl.ingress_if);
    if (BCM_FAILURE(rv)) {
       printf("Error in clear_l3_ingress_interface(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

    vlan_ctrl.ingress_if = 0;
    rv = bcm_vlan_control_vlan_set(unit, in_vlan, vlan_ctrl);
    if (BCM_FAILURE(rv)) {
       printf("Error in bcm_vlan_control_vlan_set(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

	rv = bcm_l3_route_delete(unit, &route_info);
    if (BCM_FAILURE(rv)) {
       printf("Error in bcm_l3_route_delete(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

    rv = disable_l3_lookup(unit, my_station_id);
    if (BCM_FAILURE(rv)) {
       printf("Error in bcm_l3_route_delete(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

	BCM_IF_ERROR_RETURN(vlan_destroy_remove_ports(unit, out_vlan, 2, egress_port));
	BCM_IF_ERROR_RETURN(vlan_destroy_remove_port(unit, in_vlan, ingress_port));

    return rv;
}

bcm_error_t
clear_l3_ecmp_flexctr(int unit,  bcm_if_t ecmp_group_id, uint32 stat_counter_id)
{
    bcm_error_t rv = BCM_E_NONE;

    /* Detach flexctr from l3 ecmp */
	rv = bcm_l3_ecmp_stat_detach(unit, ecmp_group_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_l3_ecmp_stat_detach: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Destroy flexctr action */
    rv = bcm_flexctr_action_destroy(unit, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_action_destroy %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}


struct pkttrace_enum_map_t {
	char *name;
	int val;
};

pkttrace_enum_map_t field_names[] =
{
    {"MY_STATION_HIT", BCM_PKTIO_TRACE_MY_STATION_HIT},
    {"FWD_VLAN", BCM_PKTIO_TRACE_FWD_VLAN},
    {"FP_AND_PBM_ACTION_VALID", BCM_PKTIO_TRACE_FP_AND_PBM_ACTION_VALID},
    {"FP_REPLACE_PBM_ACTION_VALID", BCM_PKTIO_TRACE_FP_REPLACE_PBM_ACTION_VALID},
    {"FP_OR_PBM_ACTION_VALID", BCM_PKTIO_TRACE_FP_OR_PBM_ACTION_VALID},
    {"INNER_PAYLOAD_TAG_STATUS", BCM_PKTIO_TRACE_INNER_PAYLOAD_TAG_STATUS},
    {"POST_VXLT_TAG_STATUS", BCM_PKTIO_TRACE_POST_VXLT_TAG_STATUS},
    {"INCOMING_TAG_STATUS", BCM_PKTIO_TRACE_INCOMING_TAG_STATUS},
    {"FORWARDING_FIELD", BCM_PKTIO_TRACE_FORWARDING_FIELD},
    {"FORWARDING_TYPE", BCM_PKTIO_TRACE_FORWARDING_TYPE},
    {"EGRESS_MASK_TABLE_INDEX", BCM_PKTIO_TRACE_EGRESS_MASK_TABLE_INDEX},
    {"EGRESS_BLOCK_MASK_INDEX", BCM_PKTIO_TRACE_EGRESS_BLOCK_MASK_INDEX},
    {"VLAN_MASK_INDEX", BCM_PKTIO_TRACE_VLAN_MASK_INDEX},
    {"MAC_BLOCK_INDEX", BCM_PKTIO_TRACE_MAC_BLOCK_INDEX},
    {"NONUC_TRUNK_BLOCK_MASK_INDEX", BCM_PKTIO_TRACE_NONUC_TRUNK_BLOCK_MASK_INDEX},
    {"HASH_VALUE_HG", BCM_PKTIO_TRACE_HASH_VALUE_HG},
    {"HG_OFFSET", BCM_PKTIO_TRACE_HG_OFFSET},
    {"HG_PORT", BCM_PKTIO_TRACE_HG_PORT},
    {"HG_TRUNK_ID", BCM_PKTIO_TRACE_HG_TRUNK_ID},
    {"HG_TRUNK_RESOLUTION_DONE", BCM_PKTIO_TRACE_HG_TRUNK_RESOLUTION_DONE},
    {"DGPP", BCM_PKTIO_TRACE_DGPP},
    {"NHOP", BCM_PKTIO_TRACE_NHOP},
    {"LAG_OFFSET", BCM_PKTIO_TRACE_LAG_OFFSET},
    {"HASH_VALUE_LAG", BCM_PKTIO_TRACE_HASH_VALUE_LAG},
    {"LAG_ID", BCM_PKTIO_TRACE_LAG_ID},
    {"LAG_RESOLUTION_DONE", BCM_PKTIO_TRACE_LAG_RESOLUTION_DONE},
    {"RESOLVED_NETWORK_DVP", BCM_PKTIO_TRACE_RESOLVED_NETWORK_DVP},
    {"NETWORK_VP_LAG_VALID", BCM_PKTIO_TRACE_NETWORK_VP_LAG_VALID},
    {"ECMP_HASH_VALUE2", BCM_PKTIO_TRACE_ECMP_HASH_VALUE2},
    {"ECMP_OFFSET2", BCM_PKTIO_TRACE_ECMP_OFFSET2},
    {"ECMP_GROUP2", BCM_PKTIO_TRACE_ECMP_GROUP2},
    {"ECMP_RESOLUTION_DONE2", BCM_PKTIO_TRACE_ECMP_RESOLUTION_DONE2},
    {"ECMP_HASH_VALUE1", BCM_PKTIO_TRACE_ECMP_HASH_VALUE1},
    {"ECMP_OFFSET1", BCM_PKTIO_TRACE_ECMP_OFFSET1},
    {"ECMP_GROUP1", BCM_PKTIO_TRACE_ECMP_GROUP1},
    {"ECMP_RESOLUTION_DONE1", BCM_PKTIO_TRACE_ECMP_RESOLUTION_DONE1},
    {"FWD_DESTINATION_FIELD", BCM_PKTIO_TRACE_FWD_DESTINATION_FIELD},
    {"FWD_DESTINATION_TYPE", BCM_PKTIO_TRACE_FWD_DESTINATION_TYPE},
    {"PKT_RESOLUTION_VECTOR", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR},
    {"VXLT_HIT", BCM_PKTIO_TRACE_VXLT_HIT},
    {"VALID_VLAN_ID", BCM_PKTIO_TRACE_VALID_VLAN_ID},
    {"INGRESS_STG_STATE", BCM_PKTIO_TRACE_INGRESS_STG_STATE},
    {"L2_SRC_HIT", BCM_PKTIO_TRACE_L2_SRC_HIT},
    {"L2_SRC_STATIC", BCM_PKTIO_TRACE_L2_SRC_STATIC},
    {"L2_DST_HIT", BCM_PKTIO_TRACE_L2_DST_HIT},
    {"L2_USER_ENTRY_HIT", BCM_PKTIO_TRACE_L2_USER_ENTRY_HIT},
    {"L3_ENTRY_SOURCE_HIT", BCM_PKTIO_TRACE_L3_ENTRY_SOURCE_HIT},
    {"L3_ENTRY_DESTINATION_HIT", BCM_PKTIO_TRACE_L3_ENTRY_DESTINATION_HIT},
    {"LPM_HIT", BCM_PKTIO_TRACE_LPM_HIT},
    {"UNRESOLVED_SA", BCM_PKTIO_TRACE_UNRESOLVED_SA},
    {"DOS_ATTACK", BCM_PKTIO_TRACE_DOS_ATTACK},
    {"L3_TUNNEL_HIT", BCM_PKTIO_TRACE_L3_TUNNEL_HIT},
    {"MPLS_ENTRY_TABLE_LOOKUP_0_HIT", BCM_PKTIO_TRACE_MPLS_ENTRY_TABLE_LOOKUP_0_HIT},
    {"MPLS_ENTRY_TABLE_LOOKUP_1_HIT", BCM_PKTIO_TRACE_MPLS_ENTRY_TABLE_LOOKUP_1_HIT},
    {"MPLS_BOS_TERMINATED", BCM_PKTIO_TRACE_MPLS_BOS_TERMINATED},
    {"SRC_TBL_HALF_ENTRY_NUM", BCM_PKTIO_TRACE_SRC_TBL_HALF_ENTRY_NUM},
    {"SRC_TBL_INDEX", BCM_PKTIO_TRACE_SRC_TBL_INDEX},
    {"TBL_HALF_ENTRY_NUM", BCM_PKTIO_TRACE_TBL_HALF_ENTRY_NUM},
    {"TBL_INDEX", BCM_PKTIO_TRACE_TBL_INDEX},
    {"DEF64_SRC_PUBLIC_HIT", BCM_PKTIO_TRACE_DEF64_SRC_PUBLIC_HIT},
    {"DEF64_SRC_PRIVATE_HIT", BCM_PKTIO_TRACE_DEF64_SRC_PRIVATE_HIT},
    {"DEF64_PUBLIC_HIT", BCM_PKTIO_TRACE_DEF64_PUBLIC_HIT},
    {"DEF64_PRIVATE_HIT", BCM_PKTIO_TRACE_DEF64_PRIVATE_HIT},
    {"DEF128_SRC_PUBLIC_HIT", BCM_PKTIO_TRACE_DEF128_SRC_PUBLIC_HIT},
    {"DEF128_SRC_PRIVATE_HIT", BCM_PKTIO_TRACE_DEF128_SRC_PRIVATE_HIT},
    {"DEF128_PUBLIC_HIT", BCM_PKTIO_TRACE_DEF128_PUBLIC_HIT},
    {"DEF128_PRIVATE_HIT", BCM_PKTIO_TRACE_DEF128_PRIVATE_HIT},
    {"L3_LKUP2_KEY_TYPE", BCM_PKTIO_TRACE_L3_LKUP2_KEY_TYPE},
    {"L3_LKUP1_KEY_TYPE", BCM_PKTIO_TRACE_L3_LKUP1_KEY_TYPE},
    {"SRC_L3_ENTRY_BITMAP", BCM_PKTIO_TRACE_SRC_L3_ENTRY_BITMAP},
    {"SRC_L3_BUCKET_INDEX", BCM_PKTIO_TRACE_SRC_L3_BUCKET_INDEX},
    {"SRC_PRIVATE_HIT", BCM_PKTIO_TRACE_SRC_PRIVATE_HIT},
    {"SRC_PUBLIC_HIT", BCM_PKTIO_TRACE_SRC_PUBLIC_HIT},
    {"DST_L3_ENTRY_BITMAP", BCM_PKTIO_TRACE_DST_L3_ENTRY_BITMAP},
    {"DST_L3_BUCKET_INDEX", BCM_PKTIO_TRACE_DST_L3_BUCKET_INDEX},
    {"DST_PRIVATE_HIT", BCM_PKTIO_TRACE_DST_PRIVATE_HIT},
    {"DST_PUBLIC_HIT", BCM_PKTIO_TRACE_DST_PUBLIC_HIT},
    {"L2LU_SRC_INDEX", BCM_PKTIO_TRACE_L2LU_SRC_INDEX},
    {"L2LU_SRC_KEY_TYPE", BCM_PKTIO_TRACE_L2LU_SRC_KEY_TYPE},
    {"L2LU_SRC_HIT", BCM_PKTIO_TRACE_L2LU_SRC_HIT},
    {"L2LU_DST_IDX", BCM_PKTIO_TRACE_L2LU_DST_IDX},
    {"L2LU_DST_KEY_TYPE", BCM_PKTIO_TRACE_L2LU_DST_KEY_TYPE},
    {"L2LU_DST_HIT", BCM_PKTIO_TRACE_L2LU_DST_HIT},
    {"MPLS_ENTRY_INDEX_2", BCM_PKTIO_TRACE_MPLS_ENTRY_INDEX_2},
    {"MPLS_ENTRY_KEY_TYPE_2", BCM_PKTIO_TRACE_MPLS_ENTRY_KEY_TYPE_2},
    {"MPLS_ENTRY_INDEX_1", BCM_PKTIO_TRACE_MPLS_ENTRY_INDEX_1},
    {"MPLS_ENTRY_KEY_TYPE_1", BCM_PKTIO_TRACE_MPLS_ENTRY_KEY_TYPE_1},
    {"VT_INDEX_2", BCM_PKTIO_TRACE_VT_INDEX_2},
    {"VT_INDEX_1", BCM_PKTIO_TRACE_VT_INDEX_1},
    {"MPLS_ENTRY_HIT_1", BCM_PKTIO_TRACE_MPLS_ENTRY_HIT_1},
    {"MPLS_ENTRY_HIT_2", BCM_PKTIO_TRACE_MPLS_ENTRY_HIT_2},
    {"VT_KEY_TYPE_2", BCM_PKTIO_TRACE_VT_KEY_TYPE_2},
    {"VT_HIT_2", BCM_PKTIO_TRACE_VT_HIT_2},
    {"VT_KEY_TYPE_1", BCM_PKTIO_TRACE_VT_KEY_TYPE_1},
    {"VT_HIT_1", BCM_PKTIO_TRACE_VT_HIT_1},
    {"MY_STATION_INDEX", BCM_PKTIO_TRACE_MY_STATION_INDEX},
    {"ENTROPY", BCM_PKTIO_TRACE_ENTROPY},
    {"HIT_BIT_INDEX", BCM_PKTIO_TRACE_HIT_BIT_INDEX},
    {"HIT_BIT_TABLE", BCM_PKTIO_TRACE_HIT_BIT_TABLE},
    {"SUB_DB_PRIORITY", BCM_PKTIO_TRACE_SUB_DB_PRIORITY},
    {"L3_ENTRY_DST_HIT", BCM_PKTIO_TRACE_L3_ENTRY_DST_HIT},
    {"L3_ENTRY_SRC_HIT", BCM_PKTIO_TRACE_L3_ENTRY_SRC_HIT},
    {"L3_TUNNEL_HIT_1", BCM_PKTIO_TRACE_L3_TUNNEL_HIT_1},
    {"L3_TUNNEL_INDEX_1", BCM_PKTIO_TRACE_L3_TUNNEL_INDEX_1},
    {"L3_TUNNEL_KEY_TYPE_1", BCM_PKTIO_TRACE_L3_TUNNEL_KEY_TYPE_1},
    {"COMP_DST_HIT", BCM_PKTIO_TRACE_COMP_DST_HIT},
    {"COMP_DST_HIT_INDEX", BCM_PKTIO_TRACE_COMP_DST_HIT_INDEX},
    {"COMP_DST_HIT_TABLE", BCM_PKTIO_TRACE_COMP_DST_HIT_TABLE},
    {"COMP_DST_SUB_DB_PRIORITY", BCM_PKTIO_TRACE_COMP_DST_SUB_DB_PRIORITY},
    {"COMP_SRC_HIT", BCM_PKTIO_TRACE_COMP_SRC_HIT},
    {"COMP_SRC_HIT_INDEX", BCM_PKTIO_TRACE_COMP_SRC_HIT_INDEX},
    {"COMP_SRC_HIT_TABLE", BCM_PKTIO_TRACE_COMP_SRC_HIT_TABLE},
    {"COMP_SRC_SUB_DB_PRIORITY", BCM_PKTIO_TRACE_COMP_SRC_SUB_DB_PRIORITY},
    {"LPM_DST_HIT", BCM_PKTIO_TRACE_LPM_DST_HIT},
    {"LPM_DST_HIT_INDEX", BCM_PKTIO_TRACE_LPM_DST_HIT_INDEX},
    {"LPM_DST_HIT_TABLE", BCM_PKTIO_TRACE_LPM_DST_HIT_TABLE},
    {"LPM_DST_SUB_DB_PRIORITY", BCM_PKTIO_TRACE_LPM_DST_SUB_DB_PRIORITY},
    {"LPM_SRC_HIT", BCM_PKTIO_TRACE_LPM_SRC_HIT},
    {"LPM_SRC_HIT_INDEX", BCM_PKTIO_TRACE_LPM_SRC_HIT_INDEX},
    {"LPM_SRC_HIT_TABLE", BCM_PKTIO_TRACE_LPM_SRC_HIT_TABLE},
    {"LPM_SRC_SUB_DB_PRIORITY", BCM_PKTIO_TRACE_LPM_SRC_SUB_DB_PRIORITY},
    {"L3_TUNNEL_TCAM_HIT", BCM_PKTIO_TRACE_L3_TUNNEL_TCAM_HIT},
    {"L3_TUNNEL_TCAM_INDEX", BCM_PKTIO_TRACE_L3_TUNNEL_TCAM_INDEX},
    {"DVP", BCM_PKTIO_TRACE_DVP},
    {"SVP", BCM_PKTIO_TRACE_SVP},
    {"INNER_L2_OUTER_TAGGED", BCM_PKTIO_TRACE_INNER_L2_OUTER_TAGGED},
    {"O_NEXT_HOP", BCM_PKTIO_TRACE_O_NEXT_HOP},
    {"VFI", BCM_PKTIO_TRACE_VFI},
    {"MY_STATION_2_HIT", BCM_PKTIO_TRACE_MY_STATION_2_HIT},
    {"MY_STATION_2_INDEX", BCM_PKTIO_TRACE_MY_STATION_2_INDEX},
    {"LAG_INDICATOR", BCM_PKTIO_TRACE_LAG_INDICATOR},
    {"fid count", BCM_PKTIO_TRACE_FID_COUNT}
};

pkttrace_enum_map_t fwd_type_names[] =
{
    {"VID", BCM_PKTIO_TRACE_FORWARDING_T_VID},
    {"FID", BCM_PKTIO_TRACE_FORWARDING_T_FID},
    {"RESERVED_COUNTER", 2},
    {"RESERVED_COUNTER", 3},
    {"L3_MPLS", BCM_PKTIO_TRACE_FORWARDING_T_L3_MPLS},
    {"VRF", BCM_PKTIO_TRACE_FORWARDING_T_VRF},
    {"RESERVED_COUNTER", 6},
    {"MPLS", BCM_PKTIO_TRACE_FORWARDING_T_MPLS},
};

pkttrace_enum_map_t fwd_dest_type_names[] =
{
    {"DGLP", BCM_PKTIO_TRACE_FWD_DESTINATION_T_DGLP},
    {"NHI", BCM_PKTIO_TRACE_FWD_DESTINATION_T_NHI},
    {"ECMP", BCM_PKTIO_TRACE_FWD_DESTINATION_T_ECMP},
    {"ECMP_MEMBER", BCM_PKTIO_TRACE_FWD_DESTINATION_T_ECMP_MEMBER},
    {"IPMC", BCM_PKTIO_TRACE_FWD_DESTINATION_T_IPMC},
    {"L2MC", BCM_PKTIO_TRACE_FWD_DESTINATION_T_L2MC},
    {"VLAN_FLOOD", BCM_PKTIO_TRACE_FWD_DESTINATION_T_VLAN_FLOOD},
};

pkttrace_enum_map_t pkt_resolution_names[] =
{
    {"UNKNOWN_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_UNKNOWN_PKT},
    {"CONTROL_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_CONTROL_PKT},
    {"OAM_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_OAM_PKT},
    {"BFD_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_BFD_PKT},
    {"BPDU_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_BPDU_PKT},
    {"RESERVED_COUNTER", 5},
    {"PKT_IS_1588", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_PKT_IS_1588},
    {"RESERVED_COUNTER", 7},
    {"KNOWN_L2UC_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_L2UC_PKT},
    {"UNKNOWN_L2UC_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_UNKNOWN_L2UC_PKT},
    {"KNOWN_L2MC_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_L2MC_PKT},
    {"UNKNOWN_L2MC_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_UNKNOWN_L2MC_PKT},
    {"L2BC_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_L2BC_PKT},
    {"RESERVED_COUNTER", 13},
    {"RESERVED_COUNTER", 14},
    {"RESERVED_COUNTER", 15},
    {"KNOWN_L3UC_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_L3UC_PKT},
    {"UNKNOWN_L3UC_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_UNKNOWN_L3UC_PKT},
    {"KNOWN_IPMC_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_IPMC_PKT},
    {"UNKNOWN_IPMC_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_UNKNOWN_IPMC_PKT},
    {"RESERVED_COUNTER", 20},
    {"RESERVED_COUNTER", 21},
    {"RESERVED_COUNTER", 22},
    {"RESERVED_COUNTER", 23},
    {"KNOWN_MPLS_L2_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_MPLS_L2_PKT},
    {"UNKNOWN_MPLS_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_UNKNOWN_MPLS_PKT},
    {"KNOWN_MPLS_L3_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_MPLS_L3_PKT},
    {"RESERVED_COUNTER", 27},
    {"KNOWN_MPLS_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_MPLS_PKT},
    {"RESERVED_COUNTER", 29},
    {"RESERVED_COUNTER", 30},
    {"RESERVED_COUNTER", 31},
    {"KNOWN_MIM_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_MIM_PKT},
    {"UNKNOWN_MIM_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_UNKNOWN_MIM_PKT},
    {"RESERVED_COUNTER", 34},
    {"RESERVED_COUNTER", 35},
    {"RESERVED_COUNTER", 36},
    {"RESERVED_COUNTER", 37},
    {"RESERVED_COUNTER", 38},
    {"RESERVED_COUNTER", 39},
    {"KNOWN_TRILL_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_TRILL_PKT},
    {"UNKNOWN_TRILL_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_UNKNOWN_TRILL_PKT},
    {"RESERVED_COUNTER", 42},
    {"RESERVED_COUNTER", 43},
    {"RESERVED_COUNTER", 44},
    {"RESERVED_COUNTER", 45},
    {"RESERVED_COUNTER", 46},
    {"RESERVED_COUNTER", 47},
    {"KNOWN_NIV_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_NIV_PKT},
    {"UNKNOWN_NIV_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_UNKNOWN_NIV_PKT},
    {"KNOWN_L2GRE_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_L2GRE_PKT},
    {"KNOWN_VXLAN_PKT", BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_VXLAN_PKT},
};

pkttrace_enum_map_t drop_reason_names[] =
{
    {"BFD_TERMINATED_DROP", BCM_PKTIO_TRACE_DROP_REASON_BFD_TERMINATED_DROP},
    {"BPDU", BCM_PKTIO_TRACE_DROP_REASON_BPDU},
    {"CFI_OR_L3DISABLE", BCM_PKTIO_TRACE_DROP_REASON_CFI_OR_L3DISABLE},
    {"CML", BCM_PKTIO_TRACE_DROP_REASON_CML},
    {"COMPOSITE_ERROR", BCM_PKTIO_TRACE_DROP_REASON_COMPOSITE_ERROR},
    {"CONTROL_FRAME", BCM_PKTIO_TRACE_DROP_REASON_CONTROL_FRAME},
    {"IPV4_PROTOCOL_ERROR", BCM_PKTIO_TRACE_DROP_REASON_IPV4_PROTOCOL_ERROR},
    {"IPV6_PROTOCOL_ERROR", BCM_PKTIO_TRACE_DROP_REASON_IPV6_PROTOCOL_ERROR},
    {"L3_DOS_ATTACK", BCM_PKTIO_TRACE_DROP_REASON_L3_DOS_ATTACK},
    {"L4_DOS_ATTACK", BCM_PKTIO_TRACE_DROP_REASON_L4_DOS_ATTACK},
    {"LAG_FAIL_LOOPBACK", BCM_PKTIO_TRACE_DROP_REASON_LAG_FAIL_LOOPBACK},
    {"MACSA_EQUALS_DA", BCM_PKTIO_TRACE_DROP_REASON_MACSA_EQUALS_DA},
    {"IPMC_PROC", BCM_PKTIO_TRACE_DROP_REASON_IPMC_PROC},
    {"L2DST_DISCARD", BCM_PKTIO_TRACE_DROP_REASON_L2DST_DISCARD},
    {"L2SRC_DISCARD", BCM_PKTIO_TRACE_DROP_REASON_L2SRC_DISCARD},
    {"L2SRC_STATIC_MOVE", BCM_PKTIO_TRACE_DROP_REASON_L2SRC_STATIC_MOVE},
    {"L3DST_DISCARD", BCM_PKTIO_TRACE_DROP_REASON_L3DST_DISCARD},
    {"L3SRC_DISCARD", BCM_PKTIO_TRACE_DROP_REASON_L3SRC_DISCARD},
    {"L3_DST_LOOKUP_MISS", BCM_PKTIO_TRACE_DROP_REASON_L3_DST_LOOKUP_MISS},
    {"L3_HDR_ERROR", BCM_PKTIO_TRACE_DROP_REASON_L3_HDR_ERROR},
    {"L3_TTL_ERROR", BCM_PKTIO_TRACE_DROP_REASON_L3_TTL_ERROR},
    {"MACSA0", BCM_PKTIO_TRACE_DROP_REASON_MACSA0},
    {"MPLS_STAGE", BCM_PKTIO_TRACE_DROP_REASON_MPLS_STAGE},
    {"MULTICAST_INDEX_ERROR", BCM_PKTIO_TRACE_DROP_REASON_MULTICAST_INDEX_ERROR},
    {"MY_STATION", BCM_PKTIO_TRACE_DROP_REASON_MY_STATION},
    {"PFM", BCM_PKTIO_TRACE_DROP_REASON_PFM},
    {"PROTCOL_PKT", BCM_PKTIO_TRACE_DROP_REASON_PROTCOL_PKT},
    {"PVLAN_VP_EFILTER", BCM_PKTIO_TRACE_DROP_REASON_PVLAN_VP_EFILTER},
    {"ECMP_RESOLUTION_ERROR", BCM_PKTIO_TRACE_DROP_REASON_ECMP_RESOLUTION_ERROR},
    {"FP_CHANGE_L2_FIELDS_NO_REDIRECT_DROP", BCM_PKTIO_TRACE_DROP_REASON_FP_CHANGE_L2_FIELDS_NO_REDIRECT_DROP},
    {"IFP_DROP", BCM_PKTIO_TRACE_DROP_REASON_IFP_DROP},
    {"NEXT_HOP_DROP", BCM_PKTIO_TRACE_DROP_REASON_NEXT_HOP_DROP},
    {"PROTECTION_DATA_DROP", BCM_PKTIO_TRACE_DROP_REASON_PROTECTION_DATA_DROP},
    {"TUNNEL_ERROR", BCM_PKTIO_TRACE_DROP_REASON_TUNNEL_ERROR},
    {"SPANNING_TREE_STATE", BCM_PKTIO_TRACE_DROP_REASON_SPANNING_TREE_STATE},
    {"SRC_ROUTE", BCM_PKTIO_TRACE_DROP_REASON_SRC_ROUTE},
    {"TAG_UNTAG_DISCARD", BCM_PKTIO_TRACE_DROP_REASON_TAG_UNTAG_DISCARD},
    {"TIME_SYNC_PKT", BCM_PKTIO_TRACE_DROP_REASON_TIME_SYNC_PKT},
    {"TUNNEL_DECAP_ECN", BCM_PKTIO_TRACE_DROP_REASON_TUNNEL_DECAP_ECN},
    {"MPLS_GAL_LABEL", BCM_PKTIO_TRACE_DROP_REASON_MPLS_GAL_LABEL},
    {"MPLS_INVALID_ACTION", BCM_PKTIO_TRACE_DROP_REASON_MPLS_INVALID_ACTION},
    {"MPLS_INVALID_CW", BCM_PKTIO_TRACE_DROP_REASON_MPLS_INVALID_CW},
    {"MPLS_INVALID_PAYLOAD", BCM_PKTIO_TRACE_DROP_REASON_MPLS_INVALID_PAYLOAD},
    {"MPLS_LABEL_MISS", BCM_PKTIO_TRACE_DROP_REASON_MPLS_LABEL_MISS},
    {"MPLS_TTL_CHECK_FAIL", BCM_PKTIO_TRACE_DROP_REASON_MPLS_TTL_CHECK_FAIL},
    {"VFP", BCM_PKTIO_TRACE_DROP_REASON_VFP},
    {"VLAN_CC_OR_PBT", BCM_PKTIO_TRACE_DROP_REASON_VLAN_CC_OR_PBT},
    {"ENIFILTER", BCM_PKTIO_TRACE_DROP_REASON_ENIFILTER},
    {"INVALID_TPID", BCM_PKTIO_TRACE_DROP_REASON_INVALID_TPID},
    {"INVALID_VLAN", BCM_PKTIO_TRACE_DROP_REASON_INVALID_VLAN},
    {"PVLAN_VID_MISMATCH", BCM_PKTIO_TRACE_DROP_REASON_PVLAN_VID_MISMATCH},
    {"VXLT_MISS", BCM_PKTIO_TRACE_DROP_REASON_VXLT_MISS},
    {"HIGIG_MH_TYPE1", BCM_PKTIO_TRACE_DROP_REASON_HIGIG_MH_TYPE1},
    {"DISC_STAGE", BCM_PKTIO_TRACE_DROP_REASON_DISC_STAGE},
    {"SW1_INVALID_VLAN", BCM_PKTIO_TRACE_DROP_REASON_SW1_INVALID_VLAN},
    {"HIGIG_HDR_ERROR", BCM_PKTIO_TRACE_DROP_REASON_HIGIG_HDR_ERROR},
    {"LAG_FAILOVER", BCM_PKTIO_TRACE_DROP_REASON_LAG_FAILOVER},
    {"CLASS_BASED_SM", BCM_PKTIO_TRACE_DROP_REASON_CLASS_BASED_SM},
    {"BAD_UDP_CHECKSUM", BCM_PKTIO_TRACE_DROP_REASON_BAD_UDP_CHECKSUM},
    {"NIV_FORWARDING", BCM_PKTIO_TRACE_DROP_REASON_NIV_FORWARDING},
    {"NIV_RPF_CHECK_FAIL", BCM_PKTIO_TRACE_DROP_REASON_NIV_RPF_CHECK_FAIL},
    {"TRILL_HEADER_VERSION_NONZERO", BCM_PKTIO_TRACE_DROP_REASON_TRILL_HEADER_VERSION_NONZERO},
    {"TRILL_ADJACENCY_CHECK_FAIL", BCM_PKTIO_TRACE_DROP_REASON_TRILL_ADJACENCY_CHECK_FAIL},
    {"TRILL_RBRIDGE_LOOKUP_MISS", BCM_PKTIO_TRACE_DROP_REASON_TRILL_RBRIDGE_LOOKUP_MISS},
    {"TRILL_UC_HDR_MC_MACDA", BCM_PKTIO_TRACE_DROP_REASON_TRILL_UC_HDR_MC_MACDA},
    {"TRILL_SLOWPATH", BCM_PKTIO_TRACE_DROP_REASON_TRILL_SLOWPATH},
    {"CORE_IS_IS_PKT", BCM_PKTIO_TRACE_DROP_REASON_CORE_IS_IS_PKT},
    {"TRILL_RPF_CHECK_FAIL", BCM_PKTIO_TRACE_DROP_REASON_TRILL_RPF_CHECK_FAIL},
    {"TRILL_PKT_WITH_SNAP_ENCAP", BCM_PKTIO_TRACE_DROP_REASON_TRILL_PKT_WITH_SNAP_ENCAP},
    {"TRILL_ING_RBRIDGE_EQ_EGR_RBRIDGE", BCM_PKTIO_TRACE_DROP_REASON_TRILL_ING_RBRIDGE_EQ_EGR_RBRIDGE},
    {"TRILL_HOPCOUNT_CHECK_FAIL", BCM_PKTIO_TRACE_DROP_REASON_TRILL_HOPCOUNT_CHECK_FAIL},
    {"INT_DATAPLANE_PROBE", BCM_PKTIO_TRACE_DROP_REASON_INT_DATAPLANE_PROBE},
    {"INT_ERROR", BCM_PKTIO_TRACE_DROP_REASON_INT_ERROR},
    {"INVALID_GSH_DROP", BCM_PKTIO_TRACE_DROP_REASON_INVALID_GSH_DROP},
    {"INVALID_NON_GSH_DROP", BCM_PKTIO_TRACE_DROP_REASON_INVALID_NON_GSH_DROP},
    {"SER_DROP", BCM_PKTIO_TRACE_DROP_REASON_SER_DROP},
    {"SRV6_PROC_DROP", BCM_PKTIO_TRACE_DROP_REASON_SRV6_PROC_DROP},
    {"SRV6_VALIDATION_DROP", BCM_PKTIO_TRACE_DROP_REASON_SRV6_VALIDATION_DROP},
    {"VXLAN_TUNNEL_ERROR_DROP", BCM_PKTIO_TRACE_DROP_REASON_VXLAN_TUNNEL_ERROR_DROP},
    {"VXLAN_VN_ID_LOOKUP_MISS", BCM_PKTIO_TRACE_DROP_REASON_VXLAN_VN_ID_LOOKUP_MISS},
    {"UNKNOWN_RH_WITH_NONZERO_SL_DROP", BCM_PKTIO_TRACE_DROP_REASON_UNKNOWN_RH_WITH_NONZERO_SL_DROP},
    {"trace drop reason count", BCM_PKTIO_TRACE_DROP_REASON_COUNT}
};

pkttrace_enum_map_t counter_names[] =
{
    {"RIPD4", BCM_PKTIO_TRACE_COUNTER_RIPD4},
    {"RIPC4", BCM_PKTIO_TRACE_COUNTER_RIPC4},
    {"RIPHE4", BCM_PKTIO_TRACE_COUNTER_RIPHE4},
    {"IMRP4", BCM_PKTIO_TRACE_COUNTER_IMRP4},
    {"RIPD6", BCM_PKTIO_TRACE_COUNTER_RIPD6},
    {"RIPC6", BCM_PKTIO_TRACE_COUNTER_RIPC6},
    {"RIPHE6", BCM_PKTIO_TRACE_COUNTER_RIPHE6},
    {"IMRP6", BCM_PKTIO_TRACE_COUNTER_IMRP6},
    {"BFD_UNKNOWN_ACH_ERROR", BCM_PKTIO_TRACE_COUNTER_BFD_UNKNOWN_ACH_ERROR},
    {"BFD_UNKNOWN_CONTROL_PACKET", BCM_PKTIO_TRACE_COUNTER_BFD_UNKNOWN_CONTROL_PACKET},
    {"BFD_UNKNOWN_VER_OR_DISCR", BCM_PKTIO_TRACE_COUNTER_BFD_UNKNOWN_VER_OR_DISCR},
    {"BLOCK_MASK_DROP", BCM_PKTIO_TRACE_COUNTER_BLOCK_MASK_DROP},
    {"DSFRAG", BCM_PKTIO_TRACE_COUNTER_DSFRAG},
    {"DSICMP", BCM_PKTIO_TRACE_COUNTER_DSICMP},
    {"DSL2HE", BCM_PKTIO_TRACE_COUNTER_DSL2HE},
    {"IMBP", BCM_PKTIO_TRACE_COUNTER_IMBP},
    {"LAGLUP", BCM_PKTIO_TRACE_COUNTER_LAGLUP},
    {"LAGLUPD", BCM_PKTIO_TRACE_COUNTER_LAGLUPD},
    {"MTU_CHECK_FAIL", BCM_PKTIO_TRACE_COUNTER_MTU_CHECK_FAIL},
    {"PARITYD", BCM_PKTIO_TRACE_COUNTER_PARITYD},
    {"PDISC", BCM_PKTIO_TRACE_COUNTER_PDISC},
    {"RDROP", BCM_PKTIO_TRACE_COUNTER_RDROP},
    {"RIMDR", BCM_PKTIO_TRACE_COUNTER_RIMDR},
    {"RPORTD", BCM_PKTIO_TRACE_COUNTER_RPORTD},
    {"RTUN", BCM_PKTIO_TRACE_COUNTER_RTUN},
    {"RTUNE", BCM_PKTIO_TRACE_COUNTER_RTUNE},
    {"RUC", BCM_PKTIO_TRACE_COUNTER_RUC},
    {"SRC_PORT_KNOCKOUT_DROP", BCM_PKTIO_TRACE_COUNTER_SRC_PORT_KNOCKOUT_DROP},
    {"RDISC", BCM_PKTIO_TRACE_COUNTER_RDISC},
    {"RFILDR", BCM_PKTIO_TRACE_COUNTER_RFILDR},
    {"IRPSE", BCM_PKTIO_TRACE_COUNTER_IRPSE},
    {"IRHOL", BCM_PKTIO_TRACE_COUNTER_IRHOL},
    {"IRIBP", BCM_PKTIO_TRACE_COUNTER_IRIBP},
    {"DSL3HE", BCM_PKTIO_TRACE_COUNTER_DSL3HE},
    {"IUNKHDR", BCM_PKTIO_TRACE_COUNTER_IUNKHDR},
    {"DSL4HE", BCM_PKTIO_TRACE_COUNTER_DSL4HE},
    {"IMIRROR", BCM_PKTIO_TRACE_COUNTER_IMIRROR},
    {"MTUERR", BCM_PKTIO_TRACE_COUNTER_MTUERR},
    {"VLANDR", BCM_PKTIO_TRACE_COUNTER_VLANDR},
    {"HGHDRE", BCM_PKTIO_TRACE_COUNTER_HGHDRE},
    {"MCIDXE", BCM_PKTIO_TRACE_COUNTER_MCIDXE},
    {"RHGUC", BCM_PKTIO_TRACE_COUNTER_RHGUC},
    {"RHGMC", BCM_PKTIO_TRACE_COUNTER_RHGMC},
    {"URPF", BCM_PKTIO_TRACE_COUNTER_URPF},
    {"VFPDR", BCM_PKTIO_TRACE_COUNTER_VFPDR},
    {"DSTDISC", BCM_PKTIO_TRACE_COUNTER_DSTDISC},
    {"CBLDROP", BCM_PKTIO_TRACE_COUNTER_CBLDROP},
    {"MACLMT_NODROP", BCM_PKTIO_TRACE_COUNTER_MACLMT_NODROP},
    {"MACLMT_DROP", BCM_PKTIO_TRACE_COUNTER_MACLMT_DROP},
    {"VNTAG_ERROR", BCM_PKTIO_TRACE_COUNTER_VNTAG_ERROR},
    {"NIV_FORWARDING_DROP", BCM_PKTIO_TRACE_COUNTER_NIV_FORWARDING_DROP},
    {"OFFSET_NONTRILL_FRAME_ON_NW_PORT_DROP", BCM_PKTIO_TRACE_COUNTER_OFFSET_NONTRILL_FRAME_ON_NW_PORT_DROP},
    {"OFFSET_TRILL_FRAME_ON_ACCESS_PORT_DROP", BCM_PKTIO_TRACE_COUNTER_OFFSET_TRILL_FRAME_ON_ACCESS_PORT_DROP},
    {"OFFSET_TRILL_ERRORS_DROP", BCM_PKTIO_TRACE_COUNTER_OFFSET_TRILL_ERRORS_DROP},
    {"OFFSET_TRILL_RBRIDGE_LOOKUP_MISS_DROP", BCM_PKTIO_TRACE_COUNTER_OFFSET_TRILL_RBRIDGE_LOOKUP_MISS_DROP},
    {"OFFSET_TRILL_HOPCOUNT_CHECK_FAIL", BCM_PKTIO_TRACE_COUNTER_OFFSET_TRILL_HOPCOUNT_CHECK_FAIL},
    {"OFFSET_TRILL_RPF_CHECK_FAIL_DROP", BCM_PKTIO_TRACE_COUNTER_OFFSET_TRILL_RPF_CHECK_FAIL_DROP},
    {"IRVOQFC", BCM_PKTIO_TRACE_COUNTER_IRVOQFC},
    {"ECMP_NOT_RESOLVED", BCM_PKTIO_TRACE_COUNTER_ECMP_NOT_RESOLVED},
    {"trace counter count", BCM_PKTIO_TRACE_COUNTER_COUNT}
};

pkttrace_enum_map_t tag_status_names[] =
{
    {"UNTAGGED", BCM_PKTIO_TRACE_INCOMING_TAG_STATUS_UNTAGGED},
    {"TAGGED", BCM_PKTIO_TRACE_INCOMING_TAG_STATUS_TAGGED},
};

/* Parsing string given by user as packet payload */
static uint8 *
parse_data_packet_payload(int unit, char *packet_data, int *length)
{
    uint8 *p;
    char tmp, data_iter;
    int data_len, i, j, pkt_len, data_base;

    /* If string data starts with 0x or 0X, skip it */
    if ((packet_data[0] == '0')
        && (packet_data[1] == 'x' || packet_data[1] == 'X')) {
        data_base = 2;
    } else {
        data_base = 0;
    }

    data_len = sal_strlen(packet_data) - data_base;
    pkt_len = (data_len + 1) / 2;
    if (pkt_len < 64) {
        pkt_len = 64;
    }

    p = (auto)sal_alloc(pkt_len, "tx_string_packet");
    if (p == NULL) {
        return NULL;
    }
    sal_memset(p, 0, pkt_len);
    /* Convert char to value */
    i = j = 0;
    while (j < data_len) {
        data_iter = packet_data[data_base + j];
        if (('0' <= data_iter) && (data_iter <= '9')) {
            tmp = data_iter - '0';
        } else if (('a' <= data_iter) && (data_iter <= 'f')) {
            tmp = data_iter - 'a' + 10;
        } else if (('A' <= data_iter) && (data_iter <= 'F')) {
            tmp = data_iter - 'A' + 10;
        } else if (data_iter == ' ') {
            ++j;
            continue;
        } else {
            sal_free((void *)p);
            return NULL;
        }

        /* String input is in 4b unit. Below we're filling in 8b:
           offset is /2, and we shift by 4b if the input char is odd */
        p[i / 2] |= tmp << (((i + 1) % 2) * 4);
        ++i;
        ++j;
    }

    *length = i / 2;
    return p;
}

bcm_pktio_trace_data_t trace_data = {0};
bcm_error_t
config_pkt_trace(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    char data[512];
    bcm_pktio_trace_pkt_t trace_pkt;
    int tx_port = ingress_port;
    uint8 *pkt_data = NULL;
    int pkt_len = 0;

    snprintf(data, 512, "0x000000000001002a107777008100000B08004500003c67620000ff01bd760a3a40010a3a42730000550a000100516162636465666768696a6b6c6d6e6f7071727374757677616263646566676869");
    pkt_data = parse_data_packet_payload(unit, &data, &pkt_len);
    if (pkt_data == NULL) {
        rv = BCM_E_FAIL;
    }

    trace_pkt.port = tx_port;
    trace_pkt.pkt  = pkt_data;
    trace_pkt.len  = pkt_len;
    rv = bcm_pktio_trace_data_collect(unit, &trace_pkt, &trace_data);
    if (BCM_FAILURE(rv)) {
        printf("bcm_pktio_trace_data_collect() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

bcm_error_t
decode_packet_trace_data_fields(int unit, bcm_pktio_trace_data_t *trace_data)
{
    int i=0;
	int rv=0;
	unsigned int val;
    bcm_pktio_bitmap_t drop_bitmap;
    bcm_pktio_bitmap_t counter_bitmap;

    printf("\nFIELDS:\n");
    for (i = 0; i < BCM_PKTIO_TRACE_FID_COUNT; i++) {
			rv = bcm_pktio_trace_field_get(unit, trace_data, i, &val);
			if ( BCM_E_UNAVAIL == rv) continue;
            switch (i) {
                case BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR:
                    printf("\t%-32s:%s\n", field_names[i].name,
                                  pkt_resolution_names[val].name);
                    break;
                case BCM_PKTIO_TRACE_INCOMING_TAG_STATUS:
                    printf("\t%-32s:%s\n", field_names[i].name,
                                  tag_status_names[val].name);
                    break;
                case BCM_PKTIO_TRACE_FORWARDING_TYPE:
                    printf("\t%-32s:%s\n", field_names[i].name,
                                  fwd_type_names[val].name);
                    break;
                case BCM_PKTIO_TRACE_FWD_DESTINATION_TYPE:
                    printf("\t%-32s:%s\n", field_names[i].name,
                                  fwd_dest_type_names[val].name);
                    break;
                default:
                    printf("\t%-32s:%d\n", field_names[i].name, val);
            }
    }

    bcm_pktio_trace_drop_reasons_get(unit, trace_data, &drop_bitmap);
    printf("\nDROP_REASON:\n");
    for (i = 0; i < BCM_PKTIO_TRACE_DROP_REASON_COUNT; i++) {
        if (SHR_BITGET((auto)drop_bitmap.pbits, i)) {
            printf("\t%-32s\n", drop_reason_names[i].name);
        }
    }

    bcm_pktio_trace_counter_get(unit, trace_data, &counter_bitmap);
    printf("\nCOUNTER:\n");
    for (i = 0; i < BCM_PKTIO_TRACE_COUNTER_COUNT; i++) {
        if (SHR_BITGET((auto)counter_bitmap.pbits, i)) {
            printf("\t%-32s\n", counter_names[i].name);
        }
    }

    return BCM_E_NONE;
}

bcm_error_t
decode_pkt_trace_data(int unit, bcm_pktio_trace_data_t *trace_data)
{
	bcm_error_t rv = BCM_E_NONE;
    int idx;

    if ((!trace_data->len) || (NULL == trace_data->buf)){
        printf("Invalid trace data\n");
    }

    rv = decode_packet_trace_data_fields(unit, trace_data);
    if (BCM_FAILURE(rv)) {
        printf("decode_packet_trace_data_fields() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    printf("\nDATA:\n");
    for (idx = 0; idx < trace_data->len; idx++) {
        if ((idx & 0xf) == 0) {
            printf("[%04x]: ", idx);
        }
        if ((idx & 0xf) == 8) {
            printf("- ");
        }
        printf("%02x ", trace_data->buf[idx]);

        if ((idx & 0xf) == 0xf) {
            printf("\n");
        }
    }
    printf("\n");

    return rv;

}

void test_verify(int unit)
{
	bcm_error_t rv = BCM_E_NONE;
    char   str[512];
    expected_packets[0] = expected_egress_packet;
    expected_packets[1] = expected_egress_packet2;

    rv = registerPktioRxCallback(unit);
    if (BCM_FAILURE(rv)) {
        printf("registerPktioRxCallback() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Transmiting l3 packet into ingress_port:%d\n", ingress_port);
    snprintf(str, 512, "tx 10 pbm=%d data=0x000000000001002a107777008100000B08004500003c67620000ff01bd760a3a40010a3a42730000550a000100516162636465666768696a6b6c6d6e6f7071727374757677616263646566676869; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "sleep 1");

    rv = check_flexctr(unit, stat_counter_id, &test_failed);
    if (BCM_FAILURE(rv)) {
        printf("check_flexctr() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }
    rv = unregisterPktioRxCallback(unit);
    if (BCM_FAILURE(rv)) {
        printf("registerPktioRxCallback() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Configuring packet trace\n");
    rv = config_pkt_trace(unit);
    if (BCM_FAILURE(rv)) {
        printf("config_pkt_trace() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Decoding packet trace data fields\n");
	rv = decode_pkt_trace_data(unit, &trace_data);
    if (BCM_FAILURE(rv)) {
        printf("decode_pkt_trace_data() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    printf("\n-------------------------------------------------------------------- \n");
    printf("    RESULT OF PACKET TRACE VERIFICATION  ");
    printf("\n-------------------------------------------------------------------- \n");
    printf("Test = [%s]", !test_failed? "PASS":"FAIL");
    printf("\n------------------------------------- \n");

    if (!test_failed)
        return BCM_E_NONE;
    else
        return BCM_E_FAIL;
}

/*
 * Clean up pre-test setup.
 */
int test_cleanup(int unit)
{
    bcm_error_t rv = BCM_E_NONE;

    /* Remove the flex counter from l3 ecmp group */
    rv = clear_l3_ecmp_flexctr(unit, ecmp_group_id, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("clear_l3_ecmp_flexctr() failed.\n");
        return rv;
    }

    /* Remove the l3 mpls tunnel initiation related configuration */
	rv = clear_l3_mpls_init_ecmp_trunk(unit);
    if (BCM_FAILURE(rv)) {
        printf("clear_l3_mpls_init_ecmp_trunk() failed.\n");
        return rv;
	}

    /* Remove the field configiuration */
	BCM_IF_ERROR_RETURN(clear_field_resource(unit, ifp_group, ifp_entry, 4));

    /* Remove the port loopback configiuration */
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ingress_port, BCM_PORT_LOOPBACK_NONE));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, ingress_port, BCM_PORT_DISCARD_NONE));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port[0], BCM_PORT_LOOPBACK_NONE));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port[1], BCM_PORT_LOOPBACK_NONE));

    return rv;
}


/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) demonstrates the deployment for l3 mpls configure sequence(Done in config_l3_mpls_init_ecmp_trunk())
 *  c) demonstrates the functionality(done in test_verify()).
 *  d) demonstrates the l3 mpls initiation teardown configure sequence(Done in test_cleanup())
 */
bcm_error_t
execute(void)
{
    int unit = 0;
    bcm_error_t rv;

    print "~~~ #1) test_setup(): ** start **";
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return rv;
    }
    print "~~~ #1) test_setup(): ** end **";

    print "~~~ #2) config_l3_mpls_init_ecmp_trunk(): ** start **";
    if (BCM_FAILURE((rv = config_l3_mpls_init_ecmp_trunk(unit)))) {
        printf("config_l3_mpls_init_ecmp_trunk() failed.\n");
        return rv;
    }
    print "~~~ #2) config_l3_mpls_init_ecmp_trunk(): ** end **";

    print "~~~ #3) test_verify(): ** start **";
    test_verify(unit);
    print "~~~ #3) test_verify(): ** end **";

    print "~~~ #4) test_cleanup(): ** start **";
    if (BCM_FAILURE(rv = test_cleanup(unit))) {
         printf("test_cleanup() failed.\n");
         return rv;
    }
    print "~~~ #4) test_cleanup(): ** end **";

    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print "execute(): Start";
  print execute();
  print "execute(): End";
}

