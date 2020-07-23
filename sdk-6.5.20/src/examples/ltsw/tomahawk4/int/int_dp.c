/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*  Feature  : In-band Flow Analyzer transit processing
 *
 *  Usage    : BCM.0> cint int_dp.c
 *
 *  config   : int_dp_config.bcm
 *
 *  Log file : int_dp_log.txt
 *
 *  Test Topology : None
 *
 *  This script can verify below INT INT-DP APIs:
 *  ========
 *   bcm_int_metadata_profile_create
 *   bcm_int_metadata_field_add
 *   bcm_int_dp_vector_match_add
 *   bcm_int_dp_action_profile_create
 *   bcm_int_opaque_data_profile_create
 *   bcm_int_cosq_stat_config_set
 *
 *  Summary:
 *  ========
 *  This cint example demonstrate INT-DP transit node configuration.
 *
 *  Metadata format:
 *   0                   1                   2                   3
 *   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |                           DEVICE ID                           |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |    RX_TIME_STAMP_SEC_upper    |    RX_TIME_STAMP_SEC_lower    |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |    RX_TIME_STAMP_SEC_lower    |     RX_TIME_STAMP_NANOSEC     |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |     RX_TIME_STAMP_NANOSEC     |         Transit Delay         |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |                         Transit Delay                         |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |         ingress port          |         egress port           |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |       0       |A|    QUEUE_ID |                             |B|
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |                            OPAQUE                             |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  A: CUT_THOUGH (rotate right 1)
 *  B: CONGESTION
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Create vlan and add port to vlan. Enable L3 feature per port.
 *
 *    2) Step2 - Configuration (Done in int_dp_create())
 *    ======================================================
 *      a) Create ingress and egress objects.
 *      b) Create metadata profile.
 *      c) Create INT-DP vector match.
 *      d) Create INT-DP action profile.
 *      e) Create INT opaque data profile.
 *      f) Set COSQ stat values.
 *      g) Create routes.
 *      h) Add L2 MAC into l2 station table.
 *
 *    3) Step3 - Verification (Done in verify())
 *    ==========================================
 *      a) Verify the result after send a packet.
 *      b) Expected Result:
 *         Packet raw data (168):
 *      0000: c0be 9e49 7d03 e002 bb0d 8665 8100 0016
 *      0010: 0800 453a 006e 0000 0000 3e11 6531 0a0a
 *      0020: 0101 0a0a 0200 003e 1a6f 005a 0000 aaaa
 *      0030: aaaa bbbb bbbb 0101 0000 ffff ffff ff01
 *      0040: 0000 0400 0020 0000 0001 ffff ffff 0000
 *      0050: 0000 0000 0000 0000 0000 0000 000a 0002
 *      0060: 0003 0080 0000 eeee eeee 0001 0203 0405
 *      0070: 0607 0809 0a0b 0c0d 0e0f 1011 1213 1415
 *      0080: 1617 1819 1a1b 1c1d 1e1f 2021 2223 2425
 *      0090: 2627 2829 2a2b 2c2d 2e2f 3031 3233 0001
 *      00a0: af25 d21e
 */
    cint_reset();

    int unit = 0;
    int i = 0;
    int metadata_profile_id = 0;
    int action_profile_id = 0;
    int opaque_profile_id = 0;

    bcm_ip_t dip = 0x0A0A0200; /* 10.10.2.0 */

    bcm_mac_t local_mac_1 = {0xD2, 0x01, 0x39, 0x11, 0x05, 0xD8};
    bcm_mac_t local_mac_2 = {0xE0, 0x02, 0xBB, 0x0D, 0x86, 0x65};
    bcm_mac_t remote_mac_1 = {0x82, 0x80, 0x28, 0x51, 0xE7, 0x7B};
    bcm_mac_t remote_mac_2 = {0xC0, 0xBE, 0x9E, 0x49, 0x7D, 0x03};

    bcm_mac_t pkt_in_dst_mac = {0x00, 0x00, 0x00, 0x00, 0xbb, 0xbb};
    bcm_mac_t local_mac_network = {0x00, 0x00, 0x00, 0x00, 0x22, 0x22};
    bcm_vlan_t vid_acc = 21;
    bcm_vlan_t vid_network = 22;

    bcm_vrf_t   vrf = 0;
    int intf_id_acc = 1;
    int intf_id_network = 2;
    bcm_if_t ingress_if;

    bcm_port_t      port_1 = 2;  /* customer port */
    bcm_port_t      port_2 = 3;  /* provider port */

    bcm_gport_t     gport_1, gport_2;

/*
 * This function is pre-test setup.
 */
void test_setup()
{
    bcm_port_gport_get(unit, port_1, &gport_1);
    bcm_port_gport_get(unit, port_2, &gport_2);

    /* Create vlan vid_acc */
    print bcm_vlan_create(unit, vid_acc);
    print bcm_vlan_gport_add(unit, vid_acc, gport_1, 0);

    /* Create vlan vid_network */
    print bcm_vlan_create(unit, vid_network);
    print bcm_vlan_gport_add(unit, vid_network, gport_2, 0);

    /* Enable L3 Egress Mode */
    bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1);
}

void int_dp_create()
{
    /* Adjust Egress Time Stamp */
    print bcm_switch_control_set(unit, bcmSwitchIntEgressTimeDelta, 10);

    /* Enable INT-DP on port_1 */
    print bcm_port_control_set(unit, port_1, bcmPortControlIntEnable, 1);

    /* Create L3 interface for access port */
    bcm_l3_intf_t ing_l3_intf;
    bcm_l3_intf_t_init(&ing_l3_intf);
    ing_l3_intf.l3a_flags = BCM_L3_WITH_ID;
    ing_l3_intf.l3a_intf_id = intf_id_acc;
    sal_memcpy(ing_l3_intf.l3a_mac_addr, local_mac_1, sizeof(pkt_in_dst_mac));
    ing_l3_intf.l3a_vid = vid_acc;
    ing_l3_intf.l3a_vrf = vrf;
    print bcm_l3_intf_create(unit, &ing_l3_intf);

    /* Use the same ID to allocate the ingress interface (L3_IIF)
     * (This is really not needed for the routing to work, we are dealing with
     *  one direction)
     */
    bcm_l3_ingress_t l3_ingress;
    bcm_l3_ingress_t_init(&l3_ingress);
    ingress_if = ing_l3_intf.l3a_intf_id;
    l3_ingress.flags = BCM_L3_INGRESS_INT;
    l3_ingress.vrf = vrf;
    l3_ingress.ipmc_intf_id  = ingress_if;
    print bcm_l3_ingress_create(unit, &l3_ingress, &ingress_if);

    /* Create L3 interface for network port */
    bcm_l3_intf_t egr_l3_intf;
    bcm_l3_intf_t_init(&egr_l3_intf);
    egr_l3_intf.l3a_flags = BCM_L3_WITH_ID;
    egr_l3_intf.l3a_intf_id = intf_id_network;
    sal_memcpy(egr_l3_intf.l3a_mac_addr, local_mac_2, sizeof(local_mac_network));
    egr_l3_intf.l3a_vid = vid_network;
    egr_l3_intf.l3a_vrf = vrf;
    print bcm_l3_intf_create(unit, &egr_l3_intf);

    /* Create L3 egress object for network port */
    bcm_if_t l3_egr_obj_id = 0;
    bcm_l3_egress_t l3_egr_obj;
    bcm_l3_egress_t_init(&l3_egr_obj);
    l3_egr_obj.flags = 0;
    l3_egr_obj.intf = intf_id_network;
    sal_memcpy(l3_egr_obj.mac_addr, remote_mac_2, 6);
    l3_egr_obj.vlan = vid_network;
    l3_egr_obj.port = gport_2;
    print bcm_l3_egress_create(unit, 0, &l3_egr_obj, &l3_egr_obj_id);
    print l3_egr_obj_id;

    /* Create L3 egress object for access port */
    bcm_if_t l3_egr_obj_id_i = 0;
    bcm_l3_egress_t l3_egr_obj_i;
    bcm_l3_egress_t_init(&l3_egr_obj_i);
    l3_egr_obj_i.flags = 0;
    l3_egr_obj_i.intf = intf_id_acc;
    sal_memcpy(l3_egr_obj_i.mac_addr, remote_mac_1, 6);
    l3_egr_obj_i.vlan = vid_acc;
    l3_egr_obj_i.port = gport_1;
    print bcm_l3_egress_create(unit, 0, &l3_egr_obj_i, &l3_egr_obj_id_i);
    print l3_egr_obj_id_i;

    /* configure VLAN_ID to L3_IIF mapping */
    bcm_vlan_control_vlan_t vlan_control;
    bcm_vlan_control_vlan_t_init(&vlan_control);
    print bcm_vlan_control_vlan_get(unit, vid_acc, &vlan_control);
    vlan_control.ingress_if = ingress_if;
    print bcm_vlan_control_vlan_set(unit, vid_acc, vlan_control);

    /* Construct MD format */
    print bcm_int_metadata_profile_create(unit, 0, &metadata_profile_id);

    /* DEVICE ID */
    bcm_int_metadata_field_entry_t field_entry;
    bcm_int_metadata_field_entry_t_init(&field_entry);
    field_entry.select.field = bcmIntMetadataSwitchId;
    field_entry.select.start = 0;
    field_entry.select.size = 32;
    field_entry.select.shift = 0;
    field_entry.construct.offset = 0;
    print bcm_int_metadata_field_add(unit, metadata_profile_id, &field_entry);

    /* RX_TIME_STAMP_SEC_upper */
    bcm_int_metadata_field_entry_t_init(&field_entry);
    field_entry.select.field = bcmIntMetadataIngressTimestampSecondsUpper;
    field_entry.select.start = 0;
    field_entry.select.size = 16;
    field_entry.select.shift = 0;
    field_entry.construct.offset = 4;
    print bcm_int_metadata_field_add(unit, metadata_profile_id, &field_entry);

    /* RX_TIME_STAMP_SEC_lower */
    bcm_int_metadata_field_entry_t_init(&field_entry);
    field_entry.select.field = bcmIntMetadataIngressTimestampSecondsLower;
    field_entry.select.start = 0;
    field_entry.select.size = 32;
    field_entry.select.shift = 0;
    field_entry.construct.offset = 6;
    print bcm_int_metadata_field_add(unit, metadata_profile_id, &field_entry);

    /* RX_TIME_STAMP_NANOSEC */
    bcm_int_metadata_field_entry_t_init(&field_entry);
    field_entry.select.field = bcmIntMetadataIngressTimestampSubSeconds;
    field_entry.select.start =0;
    field_entry.select.size = 32;
    field_entry.select.shift = 0;
    field_entry.construct.offset = 10;
    print bcm_int_metadata_field_add(unit, metadata_profile_id, &field_entry);

    /* Transit Delay */
    bcm_int_metadata_field_entry_t_init(&field_entry);
    field_entry.select.field = bcmIntMetadataResidenceTimeSeconds;
    field_entry.select.start =0;
    field_entry.select.size = 16;
    field_entry.select.shift = 0;
    field_entry.construct.offset = 14;
    print bcm_int_metadata_field_add(unit, metadata_profile_id, &field_entry);

    bcm_int_metadata_field_entry_t_init(&field_entry);
    field_entry.select.field = bcmIntMetadataResidenceTimeSubSeconds;
    field_entry.select.start =0;
    field_entry.select.size = 32;
    field_entry.select.shift = 0;
    field_entry.construct.offset = 16;
    print bcm_int_metadata_field_add(unit, metadata_profile_id, &field_entry);

    /* ingress port */
    bcm_int_metadata_field_entry_t_init(&field_entry);
    field_entry.select.field = bcmIntMetadataIngPortId;
    field_entry.select.start =0;
    field_entry.select.size = 16;
    field_entry.select.shift = 0;
    field_entry.construct.offset = 20;
    print bcm_int_metadata_field_add(unit, metadata_profile_id, &field_entry);

    /* egress port */
    bcm_int_metadata_field_entry_t_init(&field_entry);
    field_entry.select.field = bcmIntMetadataEgrPortId;
    field_entry.select.start =0;
    field_entry.select.size = 16;
    field_entry.select.shift = 0;
    field_entry.construct.offset = 22;
    print bcm_int_metadata_field_add(unit, metadata_profile_id, &field_entry);

    /* QUEUE_ID */
    bcm_int_metadata_field_entry_t_init(&field_entry);
    field_entry.select.field = bcmIntMetadataQueueId;
    field_entry.select.start =6;
    field_entry.select.size = 6;
    field_entry.select.shift = 4;
    field_entry.construct.offset = 24;
    print bcm_int_metadata_field_add(unit, metadata_profile_id, &field_entry);

    /* 0 */
    bcm_int_metadata_field_entry_t_init(&field_entry);
    field_entry.select.field = bcmIntMetadataPadZero;
    field_entry.select.start =0;
    field_entry.select.size = 8;
    field_entry.select.shift = 0;
    field_entry.construct.offset = 24;
    print bcm_int_metadata_field_add(unit, metadata_profile_id, &field_entry);

    /* CUT_THOUGH (rotate right 1) */
    bcm_int_metadata_field_entry_t_init(&field_entry);
    field_entry.select.field = bcmIntMetadataCutThrough;
    field_entry.select.start =15;
    field_entry.select.size = 1;
    field_entry.select.shift = 1;
    field_entry.construct.offset = 25;
    print bcm_int_metadata_field_add(unit, metadata_profile_id, &field_entry);

    /* CONGESTION */
    bcm_int_metadata_field_entry_t_init(&field_entry);
    field_entry.select.field = bcmIntMetadataCongestion;
    field_entry.select.start = 15;
    field_entry.select.size = 1;
    field_entry.select.shift = 0;
    field_entry.construct.offset = 26;
    print bcm_int_metadata_field_add(unit, metadata_profile_id, &field_entry);

    /* META DATA (OPAQUE) */
    bcm_int_metadata_field_entry_t_init(&field_entry);
    field_entry.select.field = bcmIntMetadataOpaqueData1;
    field_entry.select.start =0;
    field_entry.select.size = 32;
    field_entry.select.shift = 0;
    field_entry.construct.offset = 28;
    print bcm_int_metadata_field_add(unit, metadata_profile_id, &field_entry);


    /* Matching incoming INT-DP */
    bcm_int_dp_vector_match_t vector_match;
    bcm_int_dp_vector_match_t_init(&vector_match);
    vector_match.request_vector = 0Xffffffff;
    vector_match.request_vector_mask = 0Xffffffff;
    vector_match.metadata_profile_id = 2;
    print bcm_int_dp_vector_match_add(unit, 0, &vector_match);

    print bcm_switch_control_set(unit, bcmSwitchIntVersion, 1);
    print bcm_switch_control_set(unit, bcmSwitchIntL4DestPort1, 0x1a6f);
    print bcm_switch_control_set(unit, bcmSwitchIntProbeMarker1, 0xaaaaaaaa);
    print bcm_switch_control_set(unit, bcmSwitchIntProbeMarker2, 0xbbbbbbbb);

    /* Create INT-DP action profile */
    bcm_int_dp_action_profile_t action_profile;
    bcm_int_dp_action_profile_t_init(&action_profile);
    action_profile.timestamp_mode = bcmIntTimestampPtp;
    action_profile.residence_time_format = bcmIntResidenceTime48Bits;
    action_profile.metadata_profile_id = metadata_profile_id;
    action_profile.header_operation_mode = bcmIntHeaderOperationMetadataInsert;
    print bcm_int_dp_action_profile_create(unit, 0, &action_profile_id, &action_profile);

    /* Set device ID */
    print bcm_switch_control_set(unit, bcmSwitchIntSwitchId, 0xFFFFFFFF);

    /* Create opaque data profile */
    bcm_int_opaque_data_profile_t opaque_profile;
    bcm_int_opaque_data_profile_t_init(&opaque_profile);
    opaque_profile.opaque_data[0] = 0xEEEEEEEE;
    print bcm_int_opaque_data_profile_create(unit, 0, &opaque_profile_id, &opaque_profile);

    /* Add L3 Route */
    bcm_l3_route_t route_1;
    bcm_l3_route_t_init(&route_1);
    route_1.l3a_subnet = dip;
    route_1.l3a_ip_mask = bcm_ip_mask_create(32);
    route_1.l3a_intf = l3_egr_obj_id;
    route_1.l3a_vrf = vrf;
    route_1.l3a_int_action_profile_id = action_profile_id;
    route_1.l3a_int_opaque_data_profile_id = opaque_profile_id;
    route_1.l3a_flags3 = BCM_L3_FLAGS3_INT_ACTION;
    print bcm_l3_route_add(unit, &route_1);

    /* Add L2 MAC into l2 station table to enable L3 lookup for MAC + VLAN */
    int station_id;
    bcm_l2_station_t station;
    bcm_mac_t mac_mask = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    bcm_l2_station_t_init(&station);
    station.flags = BCM_L2_STATION_IPV4;
    sal_memcpy(station.dst_mac, local_mac_1, sizeof(local_mac_1));
    sal_memcpy(station.dst_mac_mask, mac_mask, sizeof(mac_mask));
    station.vlan = vid_acc;
    station.vlan_mask = vid_acc ? 0xfff: 0x0;
    print bcm_l2_station_add(unit, &station_id, &station);

    bcm_l2_station_t_init(&station);
    station.flags = BCM_L2_STATION_IPV4 | BCM_L2_STATION_COPY_TO_CPU;
    sal_memcpy(station.dst_mac, remote_mac_2, sizeof(remote_mac_2));
    sal_memcpy(station.dst_mac_mask, mac_mask, sizeof(mac_mask));
    station.vlan = vid_network;
    station.vlan_mask = vid_network ? 0xfff: 0x0;
    print bcm_l2_station_add(unit, &station_id, &station);
}

/*
 * This function does the following.
 * Transmit the below packet on port_1 and verify
 */
void verify()
{
    char   str[1024];
    uint64 in_pkt, out_pkt;

    /* Set ports to mac loopback */
    print bcm_port_loopback_set(unit, port_1, BCM_PORT_LOOPBACK_MAC);
    print bcm_port_loopback_set(unit, port_2, BCM_PORT_LOOPBACK_MAC);

    /* Start traffic. */
    bshell(unit, "pw start");
    bshell(unit, "pw report all");

    /* Send pacekt to port_1 */
    snprintf(str, 1024, "tx 1 pbm=%d data=D201391105D882802851E77B810000150800453A006E000000003F1164310A0A01010A0A0200003E1A6F005A0000AAAAAAAABBBBBBBB01010000FFFFFFFFFF0000000400000000000001000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F303132330001AF25D21E;sleep 5", port_1);
    printf("%s\n", str);
    bshell(unit, str);
    bshell(unit, "show c");
}

test_setup();
int_dp_create();
verify();
