/*
  Feature  : Demonstrate port tail timestamp function.

  Usage    : BCM.0> cint th4_hsdk_int_ifa_timestamp_port.c

  config   : th4_hsdk_int_timestamp_config.yml

             l3_route_int_action_valid: 1

  Log file : th4_hsdk_int_ifa_timestamp_port_log.txt

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
   bcm_port_control_set

  Summary:
  ========
  The setup has VLAN 11 with member ports 1 and 2, and VLAN 12 with
  member ports 2 and CPU port. The packets ingressing on port 1 with outer VLAN
  ID 11 will be sent to port 2 and the packets ingressing on port 2 will be sent
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

    2) Step2 - Configuration (Done in port_ts_config())
    ======================================================
      a) Configure timestamp insertion per port.
      b) Enable ToD auto-update.

    3) Step3 - Verification (Done in test_verify())
    ==========================================
      a) Verify the result after send a packet.

     Expected Result:
      - Incoming packet (78 bytes)
      0000: 0000 00AA AAAA 0000 00BB BBBB 8100 000B
      0010: 1234 1234 1234 1234 1234 1234 1234 1234
      0020: 1234 1234 1234 1234 1234 1234 1234 1234
      0030: 1234 1234 1234 1234 1234 1234 1234 1234
      0040: 1234 1234 1234 1234 1234

      - Expected packet raw data (106 bytes)
      0000: 0000 00aa aaaa 0000 00bb bbbb 8100 000c
      0010: 1234 1234 1234 1234 1234 1234 1234 1234
      0020: 1234 1234 1234 1234 1234 1234 1234 1234
      0030: 1234 1234 1234 1234 1234 1234 1234 1234
      0040: 1234 1234 1234 1234 1234 148f 4ffb 0000
      0050: 0000 0000 0000 2222 0000 0000 0000 0000
      0060: 0000 5001 ffff

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
unsigned char expected_egress_packet[106] = {
0x00, 0x00, 0x00, 0xaa, 0xaa, 0xaa, 0x00, 0x00,
0x00, 0xbb, 0xbb, 0xbb, 0x81, 0x00, 0x00, 0x0c,
0x12, 0x34, 0x12, 0x34, 0x12, 0x34, 0x12, 0x34,
0x12, 0x34, 0x12, 0x34, 0x12, 0x34, 0x12, 0x34,
0x12, 0x34, 0x12, 0x34, 0x12, 0x34, 0x12, 0x34,
0x12, 0x34, 0x12, 0x34, 0x12, 0x34, 0x12, 0x34,
0x12, 0x34, 0x12, 0x34, 0x12, 0x34, 0x12, 0x34,
0x12, 0x34, 0x12, 0x34, 0x12, 0x34, 0x12, 0x34,
0x12, 0x34, 0x12, 0x34, 0x12, 0x34, 0x12, 0x34,
0x12, 0x34, 0x14, 0x8f, 0x4f, 0xfb, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x22,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x50, 0x01, 0xff, 0xff
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
    int i, filter_start = 74;

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
 * Configure port tail timestamp
 */
bcm_error_t port_ts_config(int unit)
{
    uint32 stages;
    bcm_time_tod_t tod_set;

    BCM_IF_ERROR_RETURN(
         bcm_port_control_set(unit, port_in, bcmPortControlPacketTimeStampInsertRx, 1));
    BCM_IF_ERROR_RETURN(
         bcm_port_control_set(unit, port_in, bcmPortControlPacketTimeStampRxId, 0x1111));
    BCM_IF_ERROR_RETURN(
         bcm_port_control_set(unit, port_out, bcmPortControlPacketTimeStampInsertTx, 1));
    BCM_IF_ERROR_RETURN(
         bcm_port_control_set(unit, port_out, bcmPortControlPacketTimeStampTxId, 0xffff));

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
    snprintf(str, 512, "tx 1 pbm=%d data=000000AAAAAA000000BBBBBB8100000B12341234123412341234123412341234123412341234123412341234123412341234123412341234123412341234123412341234123412341234;sleep 5", port_in);
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

    print "~~~ #2) port_ts_config(): ** start **";
    if (BCM_FAILURE((rv = port_ts_config(unit)))) {
        printf("port_ts_config() failed.\n");
        return rv;
    }
    print "~~~ #2) port_ts_config(): ** end **";

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

