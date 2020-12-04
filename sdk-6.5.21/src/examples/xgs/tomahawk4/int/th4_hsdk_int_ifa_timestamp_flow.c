/*
  Feature  : Demonstrate flow tail timestamp function.

  Usage    : BCM.0> cint th4_hsdk_int_ifa_timestamp_flow.c

  config   : th4_hsdk_int_timestamp_config.yml
             l3_route_int_action_valid: 1

  Log file : th4_hsdk_int_ifa_timestamp_flow_log.txt

  Test Topology :
                        ^
                        |
                        | CPU
                +---------------------------+
                |          BCM56996         |
                |       ^                   |
                |VLAN 12|                   |
                |packet |                   |
           +--->| ------!            VLAN 11|
           |  2 |                    packet |
           !----|<--------------------------|<------------------
                |                           | 1
                |   VLAN 11: Ports 1, 2     |
                |   VLAN 12: Ports CPU, 2   |
                |                           |
                +---------------------------+

  This script can verify below APIs:
  ========
   bcm_field_group_config_create
   bcm_field_action_config_info_add
   bcm_field_entry_install

  Summary:
  ========
  The setup has VLAN 11 with member ports 1 and 2, and VLAN 12 with
  member ports 2 and CPU port. The packets ingressing on port 1 with outer VLAN
  id 11 will be sent to port 2 and the packets ingressing on port 2 will be sent
  to CPU port. Port 1,2 have egress port action to delete outer VLAN tag so
  single tagged packets received on port 1 should egress untagged on port 2.
  The default ingress action entry of port 2 will add outer TAG for
  incoming untagged packets so CPU should receive flooded packets with
  outer VLAN 12.
  Timestamp is inserted at ingress port 1 and egress port 2.

  Ingress port(s): 1.

  Egress port(s): 2 and CPU.

    Detailed steps done in the CINT script:
    =======================================
    1) Step1 - Test Setup (Done in test_setup())
    ============================================
      a) Create vlan and add port to vlan.

    2) Step2 - Configuration (Done in flow_ts_config())
    ======================================================
      a) Configure FP to insert timestamp per flow.
      b) Enable ToD auto-update.

    3) Step3 - Verification (Done in verify())
    ==========================================
      a) Verify the result after send a packet.

      Expected Result:
      - Incoming packet (154 bytes)
      0000: 0000 0000 BBBB 0000 0000 AAAA 8100 000B
      0010: 0800 4500 008A 0000 0000 4011 A473 0A0A
      0020: 0A2A C0A8 0114 003F 1A6F 0076 0000 AAAA
      0030: AAAA BBBB BBBB 0101 0000 FFFF FFFF 0301
      0040: 0000 0060 0020 0000 0000 DDDD DDDD 0000
      0050: 4000 0000 0000 0002 76C1 6A62 76C1 6E20
      0060: 0000 001A 0019 0000 0000 5555 5555 5555
      0070: 5555 5555 5555 5555 5555 5555 5555 5555
      0080: 5555 5555 5555 5555 5555 5555 5555 5555
      0090: 5555 5555 5555

      - Expected packet raw data (182 bytes)
      0000: 0000 0000 bbbb 0000 0000 aaaa 8100 000c
      0010: 0800 4500 008a 0000 0000 4011 a473 0a0a
      0020: 0a2a c0a8 0114 003f 1a6f 0076 0000 aaaa
      0030: aaaa bbbb bbbb 0101 0000 ffff ffff 0301
      0040: 0000 0060 0020 0000 0000 dddd dddd 0000
      0050: 4000 0000 0000 0002 76c1 6a62 76c1 6e20
      0060: 0000 001a 0019 0000 0000 5555 5555 5555
      0070: 5555 5555 5555 5555 5555 5555 5555 5555
      0080: 5555 5555 5555 5555 5555 5555 5555 5555
      0090: 5555 5555 5555 0000 0000 0000 0000 0000
      00a0: 0000 0000 0000 0000 0000 0000 0000 5000
      00b0: 0001
 */

cint_reset();

int unit = 0;
bcm_port_t port_in, port_out;
bcm_vlan_t vlan_in = 11, vlan_out = 12;

uint32_t stat_counter_id;
bcm_flexctr_counter_value_t counter_value;
int test_failed = 0; /* Final result will be stored here */

/*
 * This function is written so that hardcoding of port
 * numbers in Cint scripts is removed. This function gives
 * required number of ports
 */
bcm_error_t portNumbersGet(int unit, int *port_list, int num_ports)
{
    int i=0, port=0;
    bcm_port_config_t configP;
    bcm_pbmp_t ports_pbmp;
    int rv;

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


/* Create vlan and add port to vlan */
bcm_error_t
vlan_create_add_ports(int unit, int vid, int p1, int p2)
{
    bcm_pbmp_t pbmp, upbmp;

    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, p1);
    BCM_PBMP_PORT_ADD(pbmp, p2);
    return bcm_vlan_port_add(unit, vid, pbmp, upbmp);
}

int setup_egr_port_flexctr(int unit, int egr_port)
{
    bcm_flexctr_action_t action;
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_a_op = NULL;
    bcm_flexctr_value_operation_t *value_b_op = NULL;
    int options = 0;
    bcm_error_t rv;

    bcm_flexctr_action_t_init (&action);
    action.flags = 0;
    action.source = bcmFlexctrSourceEgrPort;
    action.mode = bcmFlexctrCounterModeNormal;
    /* Total number of counters */
    action.index_num = 16;
    action.hint = 0;

    /* Counter index is always zero. */
    index_op = &action.index_operation;
    index_op->object[0] = bcmFlexctrObjectStaticEgrEgressPort;
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

    rv = bcm_flexctr_action_create(unit, options, &action, &stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_action_create %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Attach counter action to egress port */
    rv = bcm_port_stat_attach(unit, egr_port, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("ERROR: bcm_port_stat_attach() -  %s.\n", bcm_errmsg(rv));
    }

    return rv;
}

bcm_error_t
cleanup_egr_port_flexctr(int unit, bcm_gport_t egr_port)
{
    bcm_error_t rv = BCM_E_NONE;

    /* Detach flexctr from port */
    rv = bcm_port_stat_detach_with_id(unit, egr_port, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_port_stat_detach_with_id: %s\n", bcm_errmsg(rv));
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

bcm_error_t
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
        test_failed = 1;
        printf("flex_ctr_check: failed\n");
    } else {
        printf("flex_ctr_check: Successful\n");
    }

    return BCM_E_NONE;
}

/*
 * This function is pre-test setup.
 */
bcm_error_t test_setup(int unit)
{
    int port_list[2];
    int rv;

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 2)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    port_in = port_list[0];
    port_out = port_list[1];
    printf("port_in: %d, port_out: %d\n", port_in, port_out);

    /* Create vlan_in and add ports port_in & port_out to vlan */
    rv = vlan_create_add_ports(unit, vlan_in, port_in, port_out);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vlan_create_add_ports(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create vlan_out and add ports port_out & CPU to vlan */
    rv = vlan_create_add_ports(unit, vlan_out, port_out, 0);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vlan_create_add_ports(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Set default egress TAG action to remove outer and inner VLAN tags of egress packets */
    bcm_vlan_action_set_t action;

    bcm_vlan_action_set_t_init(&action);
    action.ot_outer = bcmVlanActionDelete;
    BCM_IF_ERROR_RETURN(
        bcm_vlan_port_egress_default_action_set(unit, port_in, &action));
    BCM_IF_ERROR_RETURN(
        bcm_vlan_port_egress_default_action_set(unit, port_out, &action));
    BCM_IF_ERROR_RETURN(
        bcm_port_untagged_vlan_set(unit, port_in, vlan_in));
    BCM_IF_ERROR_RETURN(
        bcm_port_untagged_vlan_set(unit, port_out, vlan_out));

    /* Set ports to mac loopback */
    BCM_IF_ERROR_RETURN(
        bcm_port_loopback_set(unit, port_in, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(
        bcm_port_loopback_set(unit, port_out, BCM_PORT_LOOPBACK_MAC));

    /* Setup flex counter on CPU */
    BCM_IF_ERROR_RETURN(
        setup_egr_port_flexctr(unit, 0));

    return BCM_E_NONE;
}

/* Expected Egress packets */
unsigned char expected_egress_packet[182] = {
0x00, 0x00, 0x00, 0x00, 0xbb, 0xbb, 0x00, 0x00,
0x00, 0x00, 0xaa, 0xaa, 0x81, 0x00, 0x00, 0x0c,
0x08, 0x00, 0x45, 0x00, 0x00, 0x8a, 0x00, 0x00,
0x00, 0x00, 0x40, 0x11, 0xa4, 0x73, 0x0a, 0x0a,
0x0a, 0x2a, 0xc0, 0xa8, 0x01, 0x14, 0x00, 0x3f,
0x1a, 0x6f, 0x00, 0x76, 0x00, 0x00, 0xaa, 0xaa,
0xaa, 0xaa, 0xbb, 0xbb, 0xbb, 0xbb, 0x01, 0x01,
0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x03, 0x01,
0x00, 0x00, 0x00, 0x60, 0x00, 0x20, 0x00, 0x00,
0x00, 0x00, 0xdd, 0xdd, 0xdd, 0xdd, 0x00, 0x00,
0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
0x76, 0xc1, 0x6a, 0x62, 0x76, 0xc1, 0x6e, 0x20,
0x00, 0x00, 0x00, 0x1a, 0x00, 0x19, 0x00, 0x00,
0x00, 0x00, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0xfd, 0xc0,
0x4e, 0xcb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x00,
0x00, 0x01
};

unsigned char packet_filter[28] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00
};

int rx_count;   /* Global received packet count */
uint32 test_pkt_num = 1;
unsigned char *expected_packets[test_pkt_num];
const uint8 priority = 100;

/* Rx callback function for applications using the HSDK PKTIO */
bcm_pktio_rx_t
pktioRxCallback(int unit, bcm_pktio_pkt_t * packet, void *cookie)
{
    int    *count = (auto) cookie;
    void   *buffer;
    uint32 length;
    uint32 port;
    unsigned char *bufcopy;
    int i, filter_start = 150;

    (*count)++; /* Bump received packet count */

    /* Get basic packet info */
    if (BCM_FAILURE(bcm_pktio_pkt_data_get(unit, packet, (void *) &buffer, &length))) {
        return BCM_PKTIO_RX_NOT_HANDLED;
    }
    /* Get source port metadata */
    BCM_IF_ERROR_RETURN(
        bcm_pktio_pmd_field_get(unit, packet, bcmPktioPmdTypeRx,
                                BCM_PKTIO_RXPMD_SRC_PORT_NUM, &port));

    printf("pktioRxCallback: packet received #%d from port %d, length %d\n",
           *count, port, length);
    if (port_out != port){
        test_failed = 1;
        printf("pktioRxCback: packet received from wrong port %d, expected from port %d\n",
               port, port_out);
    } else {
        printf("pktioRxCallback: packet received from correct expected port %d\n", port);
    }

    /* Filter some fields like timestamp, MMU counter of the meta data out
     * because these fields vary */
    bufcopy = sal_alloc(length+1, "bufcopy");
    sal_memcpy(bufcopy, buffer, length);
    for (i = 0; i < 28; i++) {
        bufcopy[filter_start+i] &= packet_filter[i];
        expected_egress_packet[filter_start+i] &= packet_filter[i];
    }

    if (sal_memcmp(bufcopy, expected_packets[0], length) != 0){
        test_failed = 1;
        printf("pktioRxCallback: received packet data comparision failed\n");
    } else {
        printf("pktioRxCallback: packet received data comparision pass\n");
    }

    return BCM_PKTIO_RX_NOT_HANDLED;
}

/* Register callback function for received packets */
bcm_error_t
registerPktioRxCallback(int unit)
{
    const uint32  flags = 0;      /* Flags only used on match (for now) */

    return bcm_pktio_rx_register(unit, "rx_cb", pktioRxCallback, priority, &rx_count, flags);
}

bcm_error_t
unregisterPktioRxCallback(int unit)
{
    return bcm_pktio_rx_unregister(unit, pktioRxCallback, priority);
}

/*
 * Enable time stamp insertion via FP
 */
bcm_error_t flow_ts_config(int unit)
{
    bcm_udf_t udf_info;
    bcm_udf_id_t udf_id;
    bcm_udf_alloc_hints_t hints;
    bcm_udf_pkt_format_info_t pkt_fmt_info;
    bcm_udf_pkt_format_id_t pkt_fmt_id;

    bcm_udf_t_init(&udf_info);
    bcm_udf_alloc_hints_t_init(&hints);

    udf_info.start = 64;
    udf_info.width = 64;
    udf_info.layer = bcmUdfLayerL4InnerHeader;
    BCM_IF_ERROR_RETURN(bcm_udf_create(unit, &hints, &udf_info, &udf_id));

    bcm_udf_pkt_format_info_t_init(&pkt_fmt_info);
    pkt_fmt_info.ethertype = 0x0800;
    pkt_fmt_info.ethertype_mask = 0xffff;
    pkt_fmt_info.vlan_tag = BCM_PKT_FORMAT_VLAN_TAG_ANY;
    pkt_fmt_info.l4_dst_port = 0x1a6f;
    pkt_fmt_info.l4_dst_port_mask = 0xffff;
    BCM_IF_ERROR_RETURN(bcm_udf_pkt_format_create(unit, 0, &pkt_fmt_info, &pkt_fmt_id));

    BCM_IF_ERROR_RETURN(bcm_udf_pkt_format_add(unit, udf_id, pkt_fmt_id));

    int num_object = 1;
    int obj_list[1];
    int length = 8;
    /* Probe Marker 1: 0xAAAAAAAA, Probe Marker 2: 0xBBBBBBBB. */
    uint8 d[] = {0xAA, 0xAA, 0xAA, 0xAA, 0XBB, 0xBB, 0xBB, 0xBB};
    uint8 m[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    obj_list[0] = udf_id;
    bcm_field_qset_t qual;
    BCM_FIELD_QSET_INIT(qual);

    BCM_IF_ERROR_RETURN(
        bcm_field_qset_id_multi_set(unit, bcmFieldQualifyUdf, num_object, &obj_list, &qual));

    BCM_FIELD_QSET_ADD(qual, bcmFieldQualifySrcPort);
    BCM_FIELD_QSET_ADD(qual, bcmFieldQualifyStageIngress);

    bcm_field_aset_t fp_action;
    BCM_FIELD_ASET_INIT(fp_action);
    BCM_FIELD_ASET_ADD(fp_action, bcmFieldActionIngressTimeStampInsert);
    BCM_FIELD_ASET_ADD(fp_action, bcmFieldActionEgressTimeStampInsert);

    bcm_field_group_config_t gcfg1;
    bcm_field_group_config_t_init(gcfg1);
    gcfg1.flags= BCM_FIELD_GROUP_CREATE_WITH_ASET;
    gcfg1.qset= qual;
    gcfg1.aset= fp_action;
    gcfg1.priority= 2;
    gcfg1.group = 1;

    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &gcfg1));
    print gcfg1.group;

    bcm_field_action_params_t params;
    bcm_field_entry_t entry_id = 1;
    BCM_IF_ERROR_RETURN(bcm_field_entry_create_id(unit, gcfg1.group, entry_id));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_SrcPort(unit, entry_id, 0, 0, port_in, 0x1ff));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_udf(unit, entry_id, udf_id, length, d, m));
    BCM_IF_ERROR_RETURN(
        bcm_field_action_config_info_add(unit, entry_id, bcmFieldActionIngressTimeStampInsert, &params, NULL));
    BCM_IF_ERROR_RETURN(
        bcm_field_action_config_info_add(unit, entry_id, bcmFieldActionEgressTimeStampInsert, &params, NULL));
    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry_id));

    return BCM_E_NONE;
}

/*
 * This function does the following.
 * Transmit the below packet on port_1 and verify
 */
bcm_error_t test_verify(int unit)
{
    char str[512];
    int rv;

    expected_packets[0] = expected_egress_packet;

    rv = registerPktioRxCallback(unit);
    if (BCM_FAILURE(rv)) {
        printf("registerPktioRxCallback() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Start traffic. */
    bshell(unit, "pw start");
    bshell(unit, "pw report all");

    /* Send pacekt to port_1 */
    snprintf(str, 512, "tx 1 pbm=%d data=00000000BBBB00000000AAAA8100000B08004500008A000000004011A4730A0A0A2AC0A80114003F1A6F00760000AAAAAAAABBBBBBBB01010000FFFFFFFF030100000060002000000000DDDDDDDD0000400000000000000276C16A6276C16E200000001A0019000000005555555555555555555555555555555555555555555555555555555555555555555555555555555555555555;sleep 5", port_in);
    printf("%s\n", str);
    bshell(unit, str);
    bshell(unit, "show c");

    flex_ctr_check(unit);

    printf("\n------------------------------------------- \n");
    printf("    RESULT OF IFA TRANSIT Node VERIFICATION  ");
    printf("\n------------------------------------------- \n");
    printf("Test = [%s]", !test_failed? "PASS":"FAIL");
    printf("\n------------------------------------- \n");

    if (test_failed) {
        return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}

bcm_error_t test_cleanup(int unit)
{
    /* unregister Pktio callback */
    unregisterPktioRxCallback(unit);

    /* Cleanup flex counter */
    cleanup_egr_port_flexctr(unit, 0);

    /* Remove the port loopback configiuration */
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port_in, BCM_PORT_LOOPBACK_NONE));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port_out, BCM_PORT_LOOPBACK_NONE));

    return BCM_E_NONE;
}

bcm_error_t execute()
{
    int rv;

    bshell(unit, "config show; a ; version");

    print "~~~ #1) test_setup(): ** start **";
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return rv;
    }
    print "~~~ #1) test_setup(): ** end **";

    print "~~~ #2) flow_ts_config(): ** start **";
    if (BCM_FAILURE((rv = flow_ts_config(unit)))) {
        printf("flow_ts_config() failed.\n");
        return rv;
    }
    print "~~~ #2) flow_ts_config(): ** end **";

    print "~~~ #3) test_verify(): ** start **";
    if (BCM_FAILURE((rv = test_verify(unit)))) {
        printf("test_verify() failed.\n");
        return rv;
    }
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
  print execute();
}

