/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : L3 MPLS LSR label Swap scenario
 *
 * Usage    : BCM.0> cint th4_hsdk_l3mpls_lsr_swap_trunk.c
 *
 * config   : bcm56990-generic-l3mpls.config.yml
 *
 * Log file : th4_hsdk_l3mpls_lsr_swap_trunk_log.txt
 *
 * Test Topology :
 *
 *                  +------------------------------+
 *                  |                              |
 *                  |                              | trunk member port 0
 *                  |                              +---------------------+
 *     ingress_port |                              |
 * +----------------+          SWITCH              |
 *                  |                              |
 *                  |                              | trunk member port 1
 *                  |                              +---------------------+
 *                  |                              |
 *                  |                              |
 *                  +------------------------------+
 * Summary:
 * ========
 *   This Cint example to show configuration of L3 MPLS tunnel LSR to trunk group
 *   using BCM APIs.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects three ports and configure them in Loopback mode. Out of these
 *        thress ports, one port is used as ingress_port and the others as
 *        trunk group member egress ports.
 *
 *     b) Install an IFP rule to copy incoming packets to CPU
 *
 *     Note: IFP rule is meant for a testing purpose only (Internal) and it is
 *           nothing to do with an actual functional test.
 *
 *   2) Step2 - Configuration (Done in config_lsr_swap_trunk()):
 *   =========================================================
 *     a) Configure a basic L3 MPLS Tunnel swap/switching functional scenario and
 *        does the necessary configurations of vlan, l3 interface and next hop,
 *        trunk group.
 *
 *   3) Step3 - Verification (Done in test_verify()):
 *   ===========================================
 *     a) Register pktio call back funciton,
 *
 *     b) Transmit the MPLS packet with two labels into ingress port.
 *       {000000001111} {000000000202} 8100 000b
 *       8847 0011 1002 000a a140 4500 003c 6762 // TWO labels-0x111, 0xAA, TTL-2, TTL-64
 *       0000 fe01 be76 0a3a 4001 0a3a 4273 0000
 *       550a 0001 0051 6162 6364 6566 6768 696a
 *       6b6c 6d6e 6f70 7172 7374 7576 7761 6263
 *       6465 6667 6869
 *
 *     c) Expected Result:
 *     ===================
 *       In Pktio call back function, checking the received pkt data, packet length, and
 *       the received port as expected ports.
 *       In function check_flexctr, checking the flexctr packet counter and bytes counter
 *       as expected values.
 *       {000000aa2222} {000000aa1212} 8100 0016
 *       8847 0022 2001 000a a140 4500 003c 6762 // TWO labels-0x222, 0xAA, TTL-1, TTL-64
 *       0000 fe01 be76 0a3a 4001 0a3a 4273 0000
 *       550a 0001 0051 6162 6364 6566 6768 696a
 *       6b6c 6d6e 6f70 7172 7374 7576 7761 6263
 *       6465 6667 6869
 */

/* Reset C interpreter */
cint_reset();

bcm_port_t ingress_port;
bcm_port_t egress_port[2];
bcm_vlan_t in_vlan = 11;
bcm_vlan_t out_vlan = 22;
bcm_mac_t local_mac_in = {0x00, 0x00, 0x00, 0x00, 0x11, 0x11};
bcm_mac_t local_mac = {0x00, 0x00, 0x00, 0xAA, 0x12, 0x12};
bcm_mac_t nh_mac    = {0x00, 0x00, 0x00, 0xAA, 0x22, 0x22};
bcm_mpls_label_t in_tunnel_label = 0x111;  /* 273 */
bcm_mpls_label_t out_tunnel_label = 0x222;  /* 546 */
bcm_trunk_t trunk_id = 1;

bcm_field_group_t ifp_group;
bcm_field_entry_t ifp_entry[2];
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
    int port_list[3];

    rv = portNumbersGet(unit, port_list, 3);
    if (BCM_FAILURE(rv)) {
        printf("portNumbersGet() failed\n");
        return rv;
    }

    ingress_port = port_list[0];
	print ingress_port;
    egress_port[0] = port_list[1];
    print egress_port[0];
    egress_port[1] = port_list[2];
    print egress_port[1];

    printf("setting ingress port:%d MAC loopback\n", ingress_port);
    rv = ingress_port_setup(unit, ingress_port);
    if (BCM_FAILURE(rv)) {
        printf("ingress_port_setup() failed for port %d\n", ingress_port);
        return rv;
    }

    printf("setting egress ports:%d, %d MAC loopback\n", egress_port[0], egress_port[1]);
    rv = egress_port_setup(unit, 2, egress_port);
    if (BCM_FAILURE(rv)) {
        printf("egress_port_setup() failed for port %d, %d\n", egress_port[0], egress_port[1]);
        return rv;
    }

    /*bshell(unit, "pw start report +raw +decode");*/

	return BCM_E_NONE;
}

/*Expected Egress packets */
unsigned char expected_egress_packet[86] = {
0x00, 0x00, 0x00, 0xaa, 0x22, 0x22, 0x00, 0x00, 0x00, 0xaa, 0x12, 0x12, 0x81, 0x00, 0x00, 0x16,
0x88, 0x47, 0x00, 0x22, 0x20, 0x01, 0x00, 0x0a, 0xa1, 0x40, 0x45, 0x00, 0x00, 0x3c, 0x67, 0x62,
0x00, 0x00, 0xfe, 0x01, 0xbe, 0x76, 0x0a, 0x3a, 0x40, 0x01, 0x0a, 0x3a, 0x42, 0x73, 0x00, 0x00,
0x55, 0x0a, 0x00, 0x01, 0x00, 0x51, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a,
0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x61, 0x62, 0x63,
0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
};

int test_failed = 0; /*Final result will be stored here */
int rx_count;   /* Global received packet count */
uint32 test_pkt_num = 1;
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

	if ((egress_port[0] != port) && (egress_port[1] != port)){
   	    test_failed = 1;
        printf("pktioRxCallback: packet received from wrong port %d, expected to received from port %d or %d.\n", port, egress_port[0], egress_port[1]);
	} else if (sal_memcmp(buffer, expected_packets[0], length) != 0){
   	    test_failed = 1;
        printf("pktioRxCallback: packet received from port %d as expected, but packet data not match\n", port);
	} else {
        printf("pktioRxCallback: packet received from correct expected ports\n");
        printf("pktioRxCallback: packet received data comparision pass\n");
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

bcm_error_t
test_verify(int unit)
{
	bcm_error_t rv = BCM_E_NONE;
    char str[512];
    expected_packets[0] = expected_egress_packet;
    int expected_packet_length = 90;

    rv = registerPktioRxCallback(unit);
    if (BCM_FAILURE(rv)) {
        printf("registerPktioRxCallback() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    printf("\na) Sending MPLS packet with TWO labels to ingress_port:%d\n", ingress_port);
    snprintf(str, 512, "tx 10 pbm=%d data=0x0000000011110000000002028100000B884700111002000AA1404500003C67620000FE01BE760A3A40010A3A42730000550A000100516162636465666768696A6B6C6D6E6F7071727374757677616263646566676869; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "sleep quiet 1");

    rv = check_flexctr(unit, stat_counter_id, &test_failed);
    if (BCM_FAILURE(rv)) {
        printf("check_flexctr() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    printf("\n----------------------------------------------------------- \n");
    printf("    RESULT OF L3MPLS TUNNEL LABEL SWAP TO TRUNK VERIFICATION  ");
    printf("\n----------------------------------------------------------- \n");
    printf("Test = [%s]", !test_failed? "PASS":"FAIL");
    printf("\n------------------------------------- \n");

    if (!test_failed)
        return BCM_E_NONE;
    else
        return BCM_E_FAIL;
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

/* Create L3 egress object with outgoing label */
bcm_error_t
create_egr_obj(int unit, int l3_if, bcm_mac_t nh_mac, bcm_port_t port,
               bcm_mpls_label_t out_tunnel_label, bcm_if_t *egr_obj_id)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_l3_egress_t l3_egress;

    bcm_l3_egress_t_init(&l3_egress);
    sal_memcpy(l3_egress.mac_addr,  nh_mac, sizeof(nh_mac));
    l3_egress.intf  = l3_if;
    l3_egress.port = port;
    l3_egress.mpls_label = out_tunnel_label;
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
    bcm_error_t rv = BCM_E_NONE;

    rv = bcm_l3_egress_destroy(unit, egr_obj_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_l3_egress_destroy() %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

int config_trunk_flexctr(int unit, bcm_trunk_t tid, uint32 *stat_counter_id)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_flexctr_action_t action = {0};
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_a_op = NULL;
    bcm_flexctr_value_operation_t *value_b_op = NULL;
    int options = 0;

    action.index_num = 1;
    action.source = bcmFlexctrSourceTrunk;
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

    rv = bcm_trunk_stat_attach(unit, tid, *stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("ERROR: bcm_trunk_stat_attach() -  %s.\n", bcm_errmsg(rv));
    }
    printf("Flexctr stat_counter_id:0x%x attached to trunk %d\n", *stat_counter_id, tid);

    return rv;
}

bcm_error_t
check_flexctr(int unit, uint32 stat_counter_id, int *flexctr_check_failed)
{
    bcm_error_t rv = BCM_E_NONE;
    uint32 num_entries = 1;
    uint32 counter_index = 0;
    bcm_flexctr_counter_value_t counter_value;
    int expected_packet_length = 90*10;

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
        printf("Flexctr packet verify failed. Expected packet %d but get %d\n", rx_count, COMPILER_64_LO(counter_value.value[0]));
		*flexctr_check_failed = 1;
        return BCM_E_FAIL;
    };

    if (COMPILER_64_LO(counter_value.value[1]) != expected_packet_length) {
        printf("Flexctr byte verify failed. Expected bytes %d but get %d\n", expected_packet_length, COMPILER_64_LO(counter_value.value[1]));
        *flexctr_check_failed = 1;
        return BCM_E_FAIL;
    };

    return BCM_E_NONE;
}

bcm_error_t
clear_trunk_flexctr(int unit,  bcm_trunk_t tid, uint32 stat_counter_id)
{
    bcm_error_t rv = BCM_E_NONE;

    /* Detach flexctr from trunk */
	rv = bcm_trunk_stat_detach(unit, tid, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_trunk_stat_detach: %s\n", bcm_errmsg(rv));
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
config_lsr_swap_trunk(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_if_t intf_id;
    bcm_if_t egr_obj;
    bcm_mpls_tunnel_switch_t tunnel_switch;
    bcm_gport_t trunk_gp_id;
    bcm_port_t trunk_member_ports[2];

    /* Create and add ingress port to ingress VLAN */
    rv = vlan_create_add_port(unit, in_vlan, ingress_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vlan_create_add_port(): %s.\n",bcm_errmsg(rv));
        return rv;
    }

	/* Enable MPLS Lookup for incoming packet */
    BCM_IF_ERROR_RETURN(bcm_l2_tunnel_add(unit, local_mac_in, in_vlan));

    /* Create and add egress port to egress VLAN */
    rv = vlan_create_add_ports(unit, out_vlan, 2, egress_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vlan_create_add_port(): %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Create egress L3 interface */
    rv = create_l3_interface(unit, BCM_L3_ADD_TO_ARL, local_mac, out_vlan, &intf_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in create_l3_interface: %s.\n",bcm_errmsg(rv));
        return rv;
    }
    printf("l3 egr intf = %d\n", intf_id);

    trunk_member_ports[0] = egress_port[0];
    trunk_member_ports[1] = egress_port[1];
    rv = trunk_create(unit, &trunk_id, 2, trunk_member_ports);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing create_trunk(): %s.\n", bcm_errmsg (rv));
        return rv;
    }
    printf("\nCreate trunk id:%d with member ports %d and %d\n", trunk_id, trunk_member_ports[0], trunk_member_ports[1]);
    BCM_GPORT_TRUNK_SET (trunk_gp_id, trunk_id);
	print trunk_gp_id;

    /* Create Egress object with outgoing label */
    rv = create_egr_obj(unit, intf_id, nh_mac, trunk_gp_id, out_tunnel_label, &egr_obj);
    if (BCM_FAILURE(rv)) {
        printf("\nError in create_egr_obj: %s.\n",bcm_errmsg(rv));
        return rv;
    }
    printf("l3 egr obj = %d\n", egr_obj);

    /* MPLS label lookup and SWAP operation */
    bcm_mpls_tunnel_switch_t_init (&tunnel_switch);
    tunnel_switch.label   = in_tunnel_label;  /* Incoming MPLS label */
    tunnel_switch.port    = BCM_GPORT_INVALID;
    tunnel_switch.action  = BCM_MPLS_SWITCH_ACTION_SWAP;
    tunnel_switch.egress_if = egr_obj;  /* egress object contains the out label info */
    BCM_IF_ERROR_RETURN(bcm_mpls_tunnel_switch_add(unit, &tunnel_switch));

	/* create flex ctr and attach to trunk group */
	config_trunk_flexctr(unit, trunk_id, &stat_counter_id);

    return BCM_E_NONE;
}

bcm_error_t
clear_lsr_swap_trunk(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
	bcm_l3_egress_t egr;
    bcm_mpls_tunnel_switch_t tunnel_switch;

    bcm_mpls_tunnel_switch_t_init (&tunnel_switch);
    tunnel_switch.label   = in_tunnel_label;
    BCM_IF_ERROR_RETURN(bcm_mpls_tunnel_switch_get(unit, &tunnel_switch));
	BCM_IF_ERROR_RETURN(bcm_mpls_tunnel_switch_delete(unit, &tunnel_switch));
	BCM_IF_ERROR_RETURN(bcm_l3_egress_get(unit, tunnel_switch.egress_if, &egr));
	BCM_IF_ERROR_RETURN(clear_egr_obj(unit, tunnel_switch.egress_if));
	BCM_IF_ERROR_RETURN(bcm_trunk_destroy(unit, trunk_id));
	BCM_IF_ERROR_RETURN(clear_l3_interface(unit, egr.intf));
	BCM_IF_ERROR_RETURN(vlan_destroy_remove_ports(unit, out_vlan, 2, egress_port));
	BCM_IF_ERROR_RETURN(vlan_destroy_remove_port(unit, in_vlan, ingress_port));
    BCM_IF_ERROR_RETURN(bcm_l2_tunnel_delete(unit, local_mac_in, in_vlan));

    return rv;
}

/*
 * Clean up pre-test setup.
 */
bcm_error_t test_cleanup(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_mpls_tunnel_switch_t tunnel_switch;

    bcm_mpls_tunnel_switch_t_init (&tunnel_switch);
    tunnel_switch.label   = in_tunnel_label;  /* Incoming MPLS label */
    BCM_IF_ERROR_RETURN(bcm_mpls_tunnel_switch_get(unit, &tunnel_switch));

	/* unregister Pktio callback */
    unregisterPktioRxCallback(unit);

    /* Remove the flex counter from trunk group */
    if (BCM_FAILURE(rv = clear_trunk_flexctr(unit, trunk_id, stat_counter_id))) {
        printf("clear_trunk_flexctr() failed.\n");
        return rv;
    }

    /* Remove the l3 mpls label swap to trunk group related configuration */
	if (BCM_FAILURE(rv = clear_lsr_swap_trunk(unit))){
        printf("clear_lsr_swap_trunk() failed.\n");
        return rv;
	}

    /* Remove the field configiuration */
	BCM_IF_ERROR_RETURN(clear_field_resource(unit, ifp_group, ifp_entry, 2));

    /* Remove the port loopback configiuration */
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ingress_port, BCM_PORT_LOOPBACK_NONE));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, ingress_port, BCM_PORT_DISCARD_NONE));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port[0], BCM_PORT_LOOPBACK_NONE));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port[1], BCM_PORT_LOOPBACK_NONE));

    return BCM_E_NONE;
}
/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in config_lsr_swap_trunk())
 *  c) demonstrates the functionality(done in verify()).
 *  d) demonstrates the teardown l3 mpls termination configure sequence(Done in test_cleanup())
 */
bcm_error_t
execute(void)
{
    int unit = 0;
    bcm_error_t rv = BCM_E_NONE;

    print "~~~ #1) test_setup(): ** start **";
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return rv;
    }
    print "~~~ #1) test_setup(): ** end **";

    print "~~~ #2) config_lsr_swap_trunk(): ** start **";
    if (BCM_FAILURE((rv = config_lsr_swap_trunk(unit)))) {
        printf("config_lsr_swap_trunk() failed.\n");
        return rv;
    }
    print "~~~ #2) config_lsr_swap_trunk(): ** end **";

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

