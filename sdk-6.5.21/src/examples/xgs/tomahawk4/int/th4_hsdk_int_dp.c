/*
  Feature  : In-Band Network Telemetry (INT) transit node processing

  Usage    : BCM.0> cint th4_hsdk_int_dp.c

  config   : th4_hsdk_int_config.yml

  Log file : th4_hsdk_int_dp_log.txt

  Test Topology :

               +-----------------+
               |                 |
               |                 |
  ingress port |                 | Egress port
  ------------>>       TH4       >>-----------
               |                 |
               |                 |
               |                 |
               +-----------------+

  This script can verify below INT INT-DP APIs:
  ========
   bcm_int_metadata_profile_create
   bcm_int_metadata_field_add
   bcm_int_dp_vector_match_add
   bcm_int_dp_action_profile_create
   bcm_int_opaque_data_profile_create
   bcm_int_cosq_stat_config_set

  Summary:
  ========
  This cint example demonstrate INT-DP transit node configuration.

  Metadata format:
   0                   1                   2                   3
   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                           DEVICE ID                           |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |    RX_TIME_STAMP_SEC_upper    |    RX_TIME_STAMP_SEC_lower    |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |    RX_TIME_STAMP_SEC_lower    |     RX_TIME_STAMP_NANOSEC     |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |     RX_TIME_STAMP_NANOSEC     |         Transit Delay         |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                         Transit Delay                         |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |         ingress port          |         egress port           |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |       0       |A|    QUEUE_ID |                             |B|
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                            OPAQUE                             |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  A: CUT_THOUGH (rotate right 1)
  B: CONGESTION

  Detailed steps done in the CINT script:
  =======================================
  1) Step1 - Test Setup (Done in test_setup())
  ============================================
    a) Pick two ports as ingress port and egress port
    b) Put ports in loopback, and configure FP to copy packet to CPU on egress port

  2) Step2 - Configuration (Done in config_int_dp())
  ======================================================
    a) Create metadata profile.
    b) Create INT-DP vector match.
    c) Create INT-DP action profile.
    d) Create INT opaque data profile.
    e) Create L3 route so packet coming to ingress port will
       be routed out of egress port, and have INT header and
       meta data inserted

  3) Step3 - Verification (Done in test_verify())
  ==========================================
    a) Verify the result after send IP packet that hit the route installed
    b) Verification using flex counter
    c) Verification using Packet RX callback

     Expected Result:
       - Incoming packet (136 bytes)
      [0000]: D201 3911 05D8 8280 2851 E77B 8100 0015
      [0010]: 0800 4500 0072 0000 0000 4011 5B66 0A0A  DIP 0x0A0A0A01
      [0020]: 0101 0A0A 0A01 04B0 1A6F 005E 0000 AAAA  Dport 1A6F
      [0030]: AAAA BBBB BBBB 0101 0000 FFFF FFFF FF00
      [0040]: 0000 0400 0000 0000 0001 0001 0203 0405
      [0050]: 0607 0809 0A0B 0C0D 0E0F 1011 1213 1415
      [0060]: 1617 1819 1A1B 1C1D 1E1F 2021 2223 2425
      [0070]: 2627 2829 2A2B 2C2D 2E2F 3031 3233 0001
      [0080]: AF25 D21E XXXX XXXX

       - Expected packet raw data (168 bytes):
      [0000]: c0be 9e49 7d03 e002 bb0d 8665 8100 0016
      [0010]: 0800 4500 0072 0000 0000 3f11 5c66 0a0a  DIP 0x0A0A0A01
      [0020]: 0101 0a0a 0a01 04b0 1a6f 005e 0000 aaaa  Dport 1A6F
      [0030]: aaaa bbbb bbbb 0101 0000 ffff ffff ff01
      [0040]: 0000 0400 0020 0000 0001 5555 5555 0000  Dev_ID 0x55555555
      [0050]: 0000 0000 0000 0000 0000 0000 000a 0001
      [0060]: 0002 0080 0000 eeee eeee 0001 0203 0405
      [0070]: 0607 0809 0a0b 0c0d 0e0f 1011 1213 1415
      [0080]: 1617 1819 1a1b 1c1d 1e1f 2021 2223 2425
      [0090]: 2627 2829 2a2b 2c2d 2e2f 3031 3233 0001
      [00a0]: af25 d21e XXXX XXXX
 */

cint_reset();

int unit = 0;
int metadata_profile_id = 0;
int action_profile_id = 0;
int opaque_profile_id = 0;

bcm_mac_t local_mac_in = {0xD2, 0x01, 0x39, 0x11, 0x05, 0xD8};
bcm_mac_t local_mac_out = {0xE0, 0x02, 0xBB, 0x0D, 0x86, 0x65};
bcm_mac_t remote_mac_out = {0xC0, 0xBE, 0x9E, 0x49, 0x7D, 0x03};
bcm_ip_t dip = 0x0A0A0A01; /* 10.10.10.1 */
bcm_ip_t mask = 0xffffffff;
bcm_vlan_t vid_in = 21;
bcm_vlan_t vid_out = 22;
bcm_vrf_t vrf = 0;
int intf_id_in = 1;
int intf_id_out = 2;
bcm_if_t ingress_if;

bcm_port_t port_in;  /* ingress port */
bcm_port_t port_out;  /* egress port */
bcm_gport_t gport_in, gport_out;

uint32_t stat_counter_id;
bcm_field_entry_t ifp_entry;
bcm_field_group_t ifp_group;

int rv;

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
egress_port_setup(int unit, bcm_port_t port)
{
    bcm_field_entry_t entry;
    bcm_field_group_config_t group_config;

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_ALL));

    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDrop);
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));
    ifp_group = group_config.group;
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));
    ifp_entry = entry;
    BCM_IF_ERROR_RETURN(
        bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));
    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

    return BCM_E_NONE;
}

/*
 * This function is pre-test setup.
 */
bcm_error_t test_setup(int unit)
{
    int port_list[2];

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 2)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    port_in = port_list[0];
    port_out = port_list[1];
    printf("port_in: %d, port_out: %d\n", port_in, port_out);

    bcm_port_gport_get(unit, port_in, &gport_in);
    bcm_port_gport_get(unit, port_out, &gport_out);

    /* Enable L3 Egress Mode */
    bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1);

    printf("setting ingress port:%d MAC loopback\n", port_in);
    rv = ingress_port_setup(unit, port_in);
    if (BCM_FAILURE(rv)) {
        printf("ingress_port_setup() failed for port %d\n", port_in);
        return rv;
    }

    printf("setting egress port:%d MAC loopback\n", port_out);
    rv = egress_port_setup(unit, port_out);
    if (BCM_FAILURE(rv)) {
        printf("egress_port_setup() failed for port %d\n", port_out);
        return rv;
    }

    return BCM_E_NONE;
}

/*Expected Egress packets */
unsigned char expected_egress_packet[168] = {
0xc0, 0xbe, 0x9e, 0x49, 0x7d, 0x03, 0xe0, 0x02,
0xbb, 0x0d, 0x86, 0x65, 0x81, 0x00, 0x00, 0x16,
0x08, 0x00, 0x45, 0x00, 0x00, 0x72, 0x00, 0x00,
0x00, 0x00, 0x3f, 0x11, 0x5c, 0x66, 0x0a, 0x0a,
0x01, 0x01, 0x0a, 0x0a, 0x0a, 0x01, 0x04, 0xb0,
0x1a, 0x6f, 0x00, 0x5e, 0x00, 0x00, 0xaa, 0xaa,
0xaa, 0xaa, 0xbb, 0xbb, 0xbb, 0xbb, 0x01, 0x01,
0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01,
0x00, 0x00, 0x04, 0x00, 0x00, 0x20, 0x00, 0x00,
0x00, 0x01, 0x55, 0x55, 0x55, 0x55, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x01,
0x00, 0x02, 0x00, 0x80, 0x00, 0x00, 0xee, 0xee,
0xee, 0xee, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d,
0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d,
0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25,
0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d,
0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x00, 0x01,
0xaf, 0x25, 0xd2, 0x1e
};

unsigned char packet_filter[32] = {
0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff
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
    int  *count = (auto) cookie;
    unsigned char *buffer;
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
    for (i = 0; i < 32; i++) {
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
    const uint32        flags = 0;      /* Flags only used on match (for now) */

    return bcm_pktio_rx_register
      (unit, "rx_cb", pktioRxCallback, priority, &rx_count, flags);
}

bcm_error_t
unregisterPktioRxCallback(int unit)
{
    return bcm_pktio_rx_unregister(unit, pktioRxCallback, priority);
}

/*
 * Configure INT-DP and create INT-DP metadata profile
 */
bcm_error_t int_dp_create(int unit)
{
    /* Adjust Egress Time Stamp */
    BCM_IF_ERROR_RETURN(
        bcm_switch_control_set(unit, bcmSwitchIntEgressTimeDelta, 10));

    /* Enable INT-DP on port_in */
    BCM_IF_ERROR_RETURN(
        bcm_port_control_set(unit, port_in, bcmPortControlIntEnable, 1));

    /* Construct MD format */
    BCM_IF_ERROR_RETURN(
        bcm_int_metadata_profile_create(unit, 0, &metadata_profile_id));

    /* DEVICE ID */
    bcm_int_metadata_field_entry_t field_entry;
    bcm_int_metadata_field_entry_t_init(&field_entry);
    field_entry.select.field = bcmIntMetadataSwitchId;
    field_entry.select.start = 0;
    field_entry.select.size = 32;
    field_entry.select.shift = 0;
    field_entry.construct.offset = 0;
    BCM_IF_ERROR_RETURN(
         bcm_int_metadata_field_add(unit, metadata_profile_id, &field_entry));

    /* RX_TIME_STAMP_SEC_upper */
    bcm_int_metadata_field_entry_t_init(&field_entry);
    field_entry.select.field = bcmIntMetadataIngressTimestampSecondsUpper;
    field_entry.select.start = 0;
    field_entry.select.size = 16;
    field_entry.select.shift = 0;
    field_entry.construct.offset = 4;
    BCM_IF_ERROR_RETURN(
         bcm_int_metadata_field_add(unit, metadata_profile_id, &field_entry));

    /* RX_TIME_STAMP_SEC_lower */
    bcm_int_metadata_field_entry_t_init(&field_entry);
    field_entry.select.field = bcmIntMetadataIngressTimestampSecondsLower;
    field_entry.select.start = 0;
    field_entry.select.size = 32;
    field_entry.select.shift = 0;
    field_entry.construct.offset = 6;
    BCM_IF_ERROR_RETURN(
         bcm_int_metadata_field_add(unit, metadata_profile_id, &field_entry));

    /* RX_TIME_STAMP_NANOSEC */
    bcm_int_metadata_field_entry_t_init(&field_entry);
    field_entry.select.field = bcmIntMetadataIngressTimestampSubSeconds;
    field_entry.select.start =0;
    field_entry.select.size = 32;
    field_entry.select.shift = 0;
    field_entry.construct.offset = 10;
    BCM_IF_ERROR_RETURN(
         bcm_int_metadata_field_add(unit, metadata_profile_id, &field_entry));

    /* Transit Delay */
    bcm_int_metadata_field_entry_t_init(&field_entry);
    field_entry.select.field = bcmIntMetadataResidenceTimeSeconds;
    field_entry.select.start =0;
    field_entry.select.size = 16;
    field_entry.select.shift = 0;
    field_entry.construct.offset = 14;
    BCM_IF_ERROR_RETURN(
         bcm_int_metadata_field_add(unit, metadata_profile_id, &field_entry));

    bcm_int_metadata_field_entry_t_init(&field_entry);
    field_entry.select.field = bcmIntMetadataResidenceTimeSubSeconds;
    field_entry.select.start =0;
    field_entry.select.size = 32;
    field_entry.select.shift = 0;
    field_entry.construct.offset = 16;
    BCM_IF_ERROR_RETURN(
         bcm_int_metadata_field_add(unit, metadata_profile_id, &field_entry));

    /* ingress port */
    bcm_int_metadata_field_entry_t_init(&field_entry);
    field_entry.select.field = bcmIntMetadataIngPortId;
    field_entry.select.start =0;
    field_entry.select.size = 16;
    field_entry.select.shift = 0;
    field_entry.construct.offset = 20;
    BCM_IF_ERROR_RETURN(
         bcm_int_metadata_field_add(unit, metadata_profile_id, &field_entry));

    /* egress port */
    bcm_int_metadata_field_entry_t_init(&field_entry);
    field_entry.select.field = bcmIntMetadataEgrPortId;
    field_entry.select.start =0;
    field_entry.select.size = 16;
    field_entry.select.shift = 0;
    field_entry.construct.offset = 22;
    BCM_IF_ERROR_RETURN(
         bcm_int_metadata_field_add(unit, metadata_profile_id, &field_entry));

    /* QUEUE_ID */
    bcm_int_metadata_field_entry_t_init(&field_entry);
    field_entry.select.field = bcmIntMetadataQueueId;
    field_entry.select.start =6;
    field_entry.select.size = 6;
    field_entry.select.shift = 4;
    field_entry.construct.offset = 24;
    BCM_IF_ERROR_RETURN(
         bcm_int_metadata_field_add(unit, metadata_profile_id, &field_entry));

    /* 0 */
    bcm_int_metadata_field_entry_t_init(&field_entry);
    field_entry.select.field = bcmIntMetadataPadZero;
    field_entry.select.start =0;
    field_entry.select.size = 8;
    field_entry.select.shift = 0;
    field_entry.construct.offset = 24;
    BCM_IF_ERROR_RETURN(
         bcm_int_metadata_field_add(unit, metadata_profile_id, &field_entry));

    /* CUT_THOUGH (rotate right 1) */
    bcm_int_metadata_field_entry_t_init(&field_entry);
    field_entry.select.field = bcmIntMetadataCutThrough;
    field_entry.select.start =15;
    field_entry.select.size = 1;
    field_entry.select.shift = 1;
    field_entry.construct.offset = 25;
    BCM_IF_ERROR_RETURN(
         bcm_int_metadata_field_add(unit, metadata_profile_id, &field_entry));

    /* CONGESTION */
    bcm_int_metadata_field_entry_t_init(&field_entry);
    field_entry.select.field = bcmIntMetadataCongestion;
    field_entry.select.start = 15;
    field_entry.select.size = 1;
    field_entry.select.shift = 0;
    field_entry.construct.offset = 26;
    BCM_IF_ERROR_RETURN(
         bcm_int_metadata_field_add(unit, metadata_profile_id, &field_entry));

    /* META DATA (OPAQUE) */
    bcm_int_metadata_field_entry_t_init(&field_entry);
    field_entry.select.field = bcmIntMetadataOpaqueData1;
    field_entry.select.start =0;
    field_entry.select.size = 32;
    field_entry.select.shift = 0;
    field_entry.construct.offset = 28;
    BCM_IF_ERROR_RETURN(
         bcm_int_metadata_field_add(unit, metadata_profile_id, &field_entry));

    /* Matching incoming INT-DP */
    bcm_int_dp_vector_match_t vector_match;
    bcm_int_dp_vector_match_t_init(&vector_match);
    vector_match.request_vector = 0Xffffffff;
    vector_match.request_vector_mask = 0Xffffffff;
    vector_match.metadata_profile_id = 2;
    BCM_IF_ERROR_RETURN(
        bcm_int_dp_vector_match_add(unit, 0, &vector_match));

    BCM_IF_ERROR_RETURN(
        bcm_switch_control_set(unit, bcmSwitchIntVersion, 1));
    BCM_IF_ERROR_RETURN(
        bcm_switch_control_set(unit, bcmSwitchIntL4DestPort1, 0x1a6f));
    BCM_IF_ERROR_RETURN(
        bcm_switch_control_set(unit, bcmSwitchIntProbeMarker1, 0xaaaaaaaa));
    BCM_IF_ERROR_RETURN(
        bcm_switch_control_set(unit, bcmSwitchIntProbeMarker2, 0xbbbbbbbb));

    /* Create INT-DP action profile */
    bcm_int_dp_action_profile_t action_profile;
    bcm_int_dp_action_profile_t_init(&action_profile);
    action_profile.timestamp_mode = bcmIntTimestampPtp;
    action_profile.residence_time_format = bcmIntResidenceTime48Bits;
    action_profile.metadata_profile_id = metadata_profile_id;
    action_profile.header_operation_mode = bcmIntHeaderOperationMetadataInsert;
    BCM_IF_ERROR_RETURN(
        bcm_int_dp_action_profile_create(unit, 0, &action_profile_id, &action_profile));

    /* Set device ID */
    BCM_IF_ERROR_RETURN(
        bcm_switch_control_set(unit, bcmSwitchIntSwitchId, 0x55555555));

    /* Create opaque data profile */
    bcm_int_opaque_data_profile_t opaque_profile;
    bcm_int_opaque_data_profile_t_init(&opaque_profile);
    opaque_profile.opaque_data[0] = 0xEEEEEEEE;
    BCM_IF_ERROR_RETURN(
        bcm_int_opaque_data_profile_create(unit, 0, &opaque_profile_id, &opaque_profile));

    return BCM_E_NONE;
}

/*
 * Create flex counter and attach to L3 egress object
 */
bcm_error_t config_l3_egress_flexctr(int unit, bcm_if_t egr_obj, uint32 *stat_counter_id)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_flexctr_action_t action = {0};
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_a_op = NULL;
    bcm_flexctr_value_operation_t *value_b_op = NULL;
    int options = 0;

    action.index_num = 1;
    action.source = bcmFlexctrSourceL3Egress;
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

    rv = bcm_l3_egress_stat_attach(unit, egr_obj, *stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("ERROR: bcm_l3_egress_stat_attach() -  %s.\n", bcm_errmsg(rv));
    }
    printf("Flexctr stat_counter_id:0x%x attached to l3 egress obj %d\n",
           *stat_counter_id, egr_obj);

    return rv;
}

/*
 * Read the flex counter and verify
 */
bcm_error_t
check_flexctr(int unit, uint32 stat_counter_id)
{
    bcm_error_t rv = BCM_E_NONE;
    uint32 num_entries = 1;
    uint32 counter_index = 0;
    bcm_flexctr_counter_value_t counter_value;
    int expected_packet_length = 168;

    /* Get counter value. */
    sal_memset(&counter_value, 0, sizeof(counter_value));
    rv = bcm_flexctr_stat_get(unit, stat_counter_id, num_entries,
                              &counter_index, &counter_value);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_stat_get %s\n", bcm_errmsg(rv));
        return rv;
    }
    print counter_value;

    printf("Flexctr Get : %d packets / %d bytes\n",
           COMPILER_64_LO(counter_value.value[0]),
           COMPILER_64_LO(counter_value.value[1]));

    if (COMPILER_64_LO(counter_value.value[0]) != 1) {
        printf("Flexctr packets verify failed. Expected packet %d but get %d\n",
               rx_count, COMPILER_64_LO(counter_value.value[0]));
        return BCM_E_FAIL;
    };

    if (COMPILER_64_LO(counter_value.value[1]) != expected_packet_length) {
        printf("Flexctr bytes verify failed. Expected bytes %d but get %d\n",
               expected_packet_length, COMPILER_64_LO(counter_value.value[1]));
        return BCM_E_FAIL;
    };

    return BCM_E_NONE;
}
/*
 * Delete the flex counter
 */
bcm_error_t
clear_l3_egress_flexctr(int unit,  bcm_if_t intf_id, uint32 stat_counter_id)
{
    bcm_error_t rv = BCM_E_NONE;

    /* Detach flexctr from l3 egress */
    rv = bcm_l3_egress_stat_detach(unit, intf_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_l3_egress_stat_detach: %s\n", bcm_errmsg(rv));
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

/*
 * Create a L3 route, enable INT-DP for packet hitting the route
 */
bcm_error_t
config_route_with_int_dp(int unit)
{
    /* Create VLAN */
    rv = vlan_create_add_port(unit, vid_in, port_in);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vlan_create_add_port(): %s.\n",bcm_errmsg(rv));
        return rv;
    }

    rv = vlan_create_add_port(unit, vid_out, port_out);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vlan_create_add_port(): %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Create L3 egress interface */
    bcm_l3_intf_t egr_l3_intf;
    bcm_l3_intf_t_init(&egr_l3_intf);
    egr_l3_intf.l3a_flags = BCM_L3_WITH_ID;
    egr_l3_intf.l3a_intf_id = intf_id_out;
    sal_memcpy(egr_l3_intf.l3a_mac_addr, local_mac_out, sizeof(local_mac_out));
    egr_l3_intf.l3a_vid = vid_out;
    egr_l3_intf.l3a_vrf = vrf;
    rv = bcm_l3_intf_create(unit, &egr_l3_intf);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_l3_intf_create(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create L3 ingress interface and enable INT */
    bcm_l3_ingress_t l3_ingress;
    bcm_l3_ingress_t_init(&l3_ingress);
    l3_ingress.flags = BCM_L3_INGRESS_WITH_ID |
                       BCM_L3_INGRESS_INT;
    ingress_if = egr_l3_intf.l3a_intf_id;
    l3_ingress.vrf = vrf;
    l3_ingress.ipmc_intf_id  = ingress_if;
    rv = bcm_l3_ingress_create(unit, &l3_ingress, &ingress_if);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_l3_ingress_create(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* configure VLAN_ID to L3_IIF mapping */
    bcm_vlan_control_vlan_t vlan_ctrl;
    bcm_vlan_control_vlan_t_init(&vlan_ctrl);
    bcm_vlan_control_vlan_get(unit, vid_in, &vlan_ctrl);
    vlan_ctrl.ingress_if = ingress_if;
    rv = bcm_vlan_control_vlan_set(unit, vid_in, vlan_ctrl);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_vlan_control_vlan_set(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create L3 egress object for network port */
    bcm_if_t l3_egr_obj_id = 0;
    bcm_l3_egress_t l3_egr_obj;
    bcm_l3_egress_t_init(&l3_egr_obj);
    l3_egr_obj.intf = intf_id_out;
    sal_memcpy(l3_egr_obj.mac_addr, remote_mac_out, 6);
    l3_egr_obj.port = gport_out;
    rv = bcm_l3_egress_create(unit, 0, &l3_egr_obj, &l3_egr_obj_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_l3_egress_create(): %s.\n", bcm_errmsg(rv));
        return rv;
    }
    print l3_egr_obj_id;

    /* create flex ctr and attach to l3 egress nexthop */
    rv = config_l3_egress_flexctr(unit, l3_egr_obj_id, &stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("ERROR: config_l3_egress_flexctr() : %s.\n", bcm_errmsg(rv));
    }

    /* Add L3 Route */
    bcm_l3_route_t route_info;
    bcm_l3_route_t_init(&route_info);
    route_info.l3a_subnet = dip;
    route_info.l3a_ip_mask = mask;
    route_info.l3a_intf = l3_egr_obj_id;
    route_info.l3a_vrf = vrf;
    route_info.l3a_int_action_profile_id = action_profile_id;
    route_info.l3a_int_opaque_data_profile_id = opaque_profile_id;
    BCM_IF_ERROR_RETURN(bcm_l3_route_add(unit, &route_info));

    /* Add L2 MAC into l2 station table to enable L3 lookup for MAC + VLAN */
    int station_id;
    bcm_l2_station_t station;
    bcm_mac_t mac_mask = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    bcm_l2_station_t_init(&station);
    station.flags = BCM_L2_STATION_IPV4;
    sal_memcpy(station.dst_mac, local_mac_in, sizeof(local_mac_in));
    sal_memcpy(station.dst_mac_mask, mac_mask, sizeof(mac_mask));
    station.vlan = vid_in;
    station.vlan_mask = vid_in ? 0xfff: 0x0;
    BCM_IF_ERROR_RETURN(
        bcm_l2_station_add(unit, &station_id, &station));

    return BCM_E_NONE;
}

/*
 * Configure INT-DP
 */
bcm_error_t config_int_dp(int unit)
{
    rv = int_dp_create(unit);
    if (BCM_FAILURE(rv)) {
        printf("int_dp_create() failed.\n");
        return rv;
    }

    rv = config_route_with_int_dp(unit);
    if (BCM_FAILURE(rv)) {
        printf("config_route_with_int_dp() failed.\n");
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Clear the IFP configuration
 */
bcm_error_t clear_field_resource(int unit, bcm_field_group_t group, bcm_field_entry_t entry)
{
    /* Remove field entry from hardware table */
    rv = bcm_field_entry_remove(unit, entry);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_entry_remove() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Destroy the field entry */
    rv = bcm_field_entry_destroy(unit, entry);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_entry_destroy() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
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
 * This function does the following.
 * Transmit the below packet on port_in and verify
 */
bcm_error_t test_verify(int unit)
{
    char   str[1024];

    expected_packets[0] = expected_egress_packet;

    rv = registerPktioRxCallback(unit);
    if (BCM_FAILURE(rv)) {
        printf("registerPktioRxCallback() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Start traffic. */
    bshell(unit, "pw start");
    bshell(unit, "pw report all");

    /* Send pacekt to port_in (136 bytes) */
    snprintf(str, 1024, "tx 1 pbm=%d data=D201391105D882802851E77B810000150800450000720000000040115B660A0A01010A0A0A0104B01A6F005E0000AAAAAAAABBBBBBBB01010000FFFFFFFFFF0000000400000000000001000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F303132330001AF25D21E;sleep 5", port_in);
    printf("%s\n", str);
    bshell(unit, str);
    bshell(unit, "show c");

    rv = check_flexctr(unit, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("check_flexctr() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    printf("\n------------------------------------------- \n");
    printf("    RESULT OF INT-DP TRANSIT Node VERIFICATION  ");
    printf("\n------------------------------------------- \n");
    printf("Test = [%s]", !test_failed? "PASS":"FAIL");
    printf("\n------------------------------------- \n");

    if (test_failed) {
        return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}

int test_cleanup(int unit)
{
    bcm_l3_route_t route_info;

    /* unregister Pktio callback */
    unregisterPktioRxCallback(unit);

    /* lookup a route given network and netmask */
    bcm_l3_route_t_init(&route_info);
    route_info.l3a_subnet = dip;
    route_info.l3a_ip_mask = mask;
    route_info.l3a_vrf = vrf;
    rv = bcm_l3_route_get(unit, &route_info);
    if (BCM_FAILURE(rv)) {
       printf("Error in bcm_l3_route_add(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

    rv = bcm_l3_route_delete(unit, &route_info);
    if (BCM_FAILURE(rv)) {
       printf("Error in bcm_l3_route_delete(): %s.\n", bcm_errmsg(rv));
       return rv;
    }

    /* Remove the flex counter from l3 egress object */
    rv = clear_l3_egress_flexctr(unit, route_info.l3a_intf, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("clear_l3_egress_flexctr() failed.\n");
        return rv;
    }

    /* Remove the field configiuration */
    BCM_IF_ERROR_RETURN(clear_field_resource(unit, ifp_group, ifp_entry));

    /* Remove the port loopback configiuration */
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port_in, BCM_PORT_LOOPBACK_NONE));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port_in, BCM_PORT_DISCARD_NONE));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port_out, BCM_PORT_LOOPBACK_NONE));

    return BCM_E_NONE;
}

bcm_error_t execute()
{
    bshell(unit, "config show; a ; version");

    print "~~~ #1) test_setup(): ** start **";
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return rv;
    }
    print "~~~ #1) test_setup(): ** end **";

    print "~~~ #2) config_int_dp(): ** start **";
    if (BCM_FAILURE((rv = config_int_dp(unit)))) {
        printf("config_int_dp() failed.\n");
        return rv;
    }
    print "~~~ #2) config_int_dp(): ** end **";

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

