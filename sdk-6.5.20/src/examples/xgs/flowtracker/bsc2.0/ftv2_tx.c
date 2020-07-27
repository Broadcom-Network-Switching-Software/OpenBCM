/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * C file to send traffic
 */

/* Packet configuration global variables */
uint8 ftv2_tx_test_config = 0;

/* delay after sending packet */
uint8 sleep_after_tx = 120;

/********** Packet Parameteres **********/
/* Packet Ingress Port */
int traffic_ingress_port = -1;
/* Packet Egress Port */
int traffic_egress_port = -1;
/* No. of packets sents */
uint32 traffic_pkt_num = 0;
/* No. of packets received */
uint32 traffic_pkt_num_received = 0;


/* Packet Vlan */
int pkt_ingress_vlan = -1;
/* Packet Vlan Formats
 * Untagged 0x0, OuterTagged 0x1, InnerTagged 0x2, Double Tagged 0x3 */
uint8 pkt_vlan_format = 0;
/* Packet Source IPv4 */
uint32 pkt_src_ipv4 = 0;
/* Packet Destination IPv4 */
uint32 pkt_dst_ipv4 = 0;
/* Packet L4 Source Port */
uint16 pkt_l4_src_port = 0;
/* Packet L4 Destination Port */
uint16 pkt_l4_dst_port = 0;
/* Packet IP Protocol
 * TCP - 6, UDP - 17 */
uint8 pkt_protocol = 0;
/* Packet TTL */
uint8 pkt_ttl = 0;
/* TCP Window Size */
uint16 pkt_tcp_window_size = 0;
/* VNID for vxlan packet */
uint32 pkt_in_vnid = 0;

/* Switch controls to save */
int sc_mac_sa_da_def = -1;

/* EFP rule to drop packet at egress */
bcm_field_entry_t efp_entry = -1;
bcm_field_group_t efp_group = -1;

/* Packet Used for Testing */
/*
PktType:         IPv4 TCP
PktSize:         88B
DstMac:          0x22:33:44:55:66:77
SrcMac:          0x10:20:30:40:50:60
OuterVlan:       0x1E
InnerVlan:       0x2E
SrcIp:           0x0a.0a.02.02
DstIp:           0x0a.0a.02.03
TCP SrcPort:     0x3344
TCP DstPort:     0x5566
TCP Window Size: 0x11F5
22 33 44 55 66 77 10 20 30 40 50 60 81 00 00 1E
81 00 00 2E 08 00 45 08 00 38 00 00 40 00 40 06
49 98 0A 0A 02 02 0A 0A 02 03 33 44 55 66 00 A1
A2 A3 00 B1 B2 B3 50 02 11 F5 F8 C3 00 00 F5 F5
F5 F5 F5 F5 F5 F5 F5 F5 F5 F5 F5 F5 F5 F5 F5 F5
F5 F5 F5 F5 F5 F5 F5 F5
*/
const uint8 ipv4_tcp_sync_pkt1[] = {
0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x81, 0x00, 0x00, 0x1E,
0x81, 0x00, 0x00, 0x2E, 0x08, 0x00, 0x45, 0x08, 0x00, 0x38, 0x00, 0x00, 0x40, 0x00, 0x40, 0x06,
0x49, 0x98, 0x0A, 0x0A, 0x02, 0x02, 0x0A, 0x0A, 0x02, 0x03, 0x33, 0x44, 0x55, 0x66, 0x00, 0xA1,
0xA2, 0xA3, 0x00, 0xB1, 0xB2, 0xB3, 0x50, 0x02, 0x11, 0xF5, 0xF8, 0xC3, 0x00, 0x00, 0xF5, 0xF5,
0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5,
0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5
};

/* Modified above packet ipv4_tcp_sync_pkt1 with MACSA = MACDA , drop bit test */
const uint8 ipv4_tcp_sync_pkt2[] = {
0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x81, 0x00, 0x00, 0x1E,
0x81, 0x00, 0x00, 0x2E, 0x08, 0x00, 0x45, 0x08, 0x00, 0x38, 0x00, 0x00, 0x40, 0x00, 0x40, 0x06,
0x49, 0x98, 0x0A, 0x0A, 0x02, 0x02, 0x0A, 0x0A, 0x02, 0x03, 0x33, 0x44, 0x55, 0x66, 0x00, 0xA1,
0xA2, 0xA3, 0x00, 0xB1, 0xB2, 0xB3, 0x50, 0x02, 0x11, 0xF5, 0xF8, 0xC3, 0x00, 0x00, 0xF5, 0xF5,
0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5,
0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5
};

/*
PktType:         IPv6 TCP
PktSize:         120B
DstMac:          0x20:21:22:23:24:22
SrcMac:          0x10:11:12:13:14:12
SrcIp:           0x0:1:2:3:4::2
DstIp:           0x0:2:3:4:5:6:7:2
TCP SrcPort:     0x5566
TCP DstPort:     0x7788
FlowLabel:       0x00001
NextHeader:      0x6 (TCP)
HopLimit:        0x20
10 11 12 13 14 12 20 21 22 23 24 22 86 DD 65 00
00 01 00 42 06 14 00 00 00 01 00 02 00 03 00 04
00 00 00 00 00 02 00 00 00 02 00 03 00 04 00 05
00 06 00 07 00 02 55 66 77 88 00 00 00 00 40 06
E2 3B 90 02 00 02 A0 A8 78 02 10 00 20 00 00 00
00 01 00 00 00 00 50 12 22 33 D9 EA 66 00 00 01
02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11
12 13 14 15 16 00 01 03
*/
const uint8 ipv6_tcp_sync_pkt1[] = {
0x10, 0x11, 0x12, 0x13, 0x14, 0x12, 0x20, 0x21, 0x22, 0x23, 0x24, 0x22, 0x86, 0xDD, 0x65, 0x00,
0x00, 0x01, 0x00, 0x42, 0x06, 0x14, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04,
0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04, 0x00, 0x05,
0x00, 0x06, 0x00, 0x07, 0x00, 0x02, 0x55, 0x66, 0x77, 0x88, 0x00, 0x00, 0x00, 0x00, 0x40, 0x06,
0xE2, 0x3B, 0x90, 0x02, 0x00, 0x02, 0xA0, 0xA8, 0x78, 0x02, 0x10, 0x00, 0x20, 0x00, 0x00, 0x00,
0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x50, 0x12, 0x22, 0x33, 0xD9, 0xEA, 0x66, 0x00, 0x00, 0x01,
0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11,
0x12, 0x13, 0x14, 0x15, 0x16, 0x00, 0x01, 0x03
};

/*
PktType:         IPv4
PktSize:         118B
DstMac:          0x00:00:00:00:22:22
SrcMac:          0x00:00:00:00:00:02
SrcIp:           0x0a:0a:0a:01
DstIp:           0xc0:a8:01:01
TCP SrcPort:     0xFFFF
TCP DstPort:     0x2118
VNID:            0x345667
00 00 00 00 00 02 00 00 00 00 22 22 81 00 00 16
08 00 45 00 00 64 00 01 00 00 10 11 D4 D4 0A 0A
0A 01 C0 A8 01 01 FF FF 21 18 00 50 00 00 39 74
31 01 34 56 67 17 00 00 00 00 11 BB 00 00 00 00
11 AA 81 00 00 15 08 00 45 00 00 2E 00 00 00 00
40 FF AF CB 0A 00 00 01 C0 00 00 05 00 01 02 03
04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13
14 15 16 17 18 19
*/
const uint8 vxlan_vnid_pkt1[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x22, 0x22, 0x81, 0x00, 0x00, 0x16,
0x08, 0x00, 0x45, 0x00, 0x00, 0x64, 0x00, 0x01, 0x00, 0x00, 0x10, 0x11, 0xD4, 0xD4, 0x0A, 0x0A,
0x0A, 0x01, 0xC0, 0xA8, 0x01, 0x01, 0xFF, 0xFF, 0x21, 0x18, 0x00, 0x50, 0x00, 0x00, 0x39, 0x74,
0x31, 0x01, 0x34, 0x56, 0x67, 0x17, 0x00, 0x00, 0x00, 0x00, 0x11, 0xBB, 0x00, 0x00, 0x00, 0x00,
0x11, 0xAA, 0x81, 0x00, 0x00, 0x15, 0x08, 0x00, 0x45, 0x00, 0x00, 0x2E, 0x00, 0x00, 0x00, 0x00,
0x40, 0xFF, 0xAF, 0xCB, 0x0A, 0x00, 0x00, 0x01, 0xC0, 0x00, 0x00, 0x05, 0x00, 0x01, 0x02, 0x03,
0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13,
0x14, 0x15, 0x16, 0x17, 0x18, 0x19
};

int
tx_broadscan_test_packet(int unit, uint8 *pktData, int length, int target_port, int num_packets)
{
    int rv = BCM_E_NONE, idx = 0;

    /* Transmit all packets across target port */
    for (idx = 0; idx < num_packets; idx++) {
        rv = tx_pkt_data(unit, pktData, length, target_port);
        if(BCM_FAILURE(rv)) {
            printf("[CINT] Traffic - tx_pkt_data failed with rv %d\n",rv,bcm_errmsg(rv));
            return rv;
        }
    }

    return BCM_E_NONE;
}

int
tx_pkt_data(int unit, uint8 *pktData, int length, int target_port)
{
    int rv = BCM_E_NONE;
    bcm_pkt_t  *pkt;

    /* Allocate a packet structure and associated DMA packet data buffer. */
    rv = bcm_pkt_alloc(unit, length, BCM_TX_CRC_REGEN, &pkt);
    if(BCM_FAILURE(rv)) {
        printf("[CINT] Traffic - Error in bcm_pkt_alloc(): %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Copy packet data from caller into DMA packet data buffer */
    bcm_pkt_memcpy(pkt, 0, pktData, length);
    BCM_PBMP_PORT_SET(pkt->tx_pbmp, target_port);

    /* Transmit packet, wait for DMA to complete before returning */
    rv = bcm_tx(unit, pkt, NULL);
    if(BCM_FAILURE(rv)) {
        printf("[CINT] Traffic - Error in bcm_tx(): %s.\n",bcm_errmsg(rv));
    }

    /* Free packet resources */
    bcm_pkt_free(unit, pkt);

    return rv;
}

int
ftv2_pkt_path_setup(int tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;
    bcm_pbmp_t pbmp, upbmp;

    /* Skip if no test config set */
    if (!ftv2_tx_test_config) {
        if (!skip_log) {
            printf("[CINT] Traffic - Skip setting up path for packet ......\n");
        }
        return rv;
    }

    /* Remember Me - Setup up traffic path here as per config */
    /* Send Traffic as per test config set */
    switch(ftv2_tx_test_config) {

        case 2:
            /* ftv2_tx_test_config = 2 */
            traffic_ingress_port = 19;
            traffic_egress_port = 21;
            traffic_pkt_num = 2;
            traffic_pkt_num_received = traffic_pkt_num;
            pkt_ingress_vlan = 30;
            pkt_vlan_format = 3;
            pkt_src_ipv4 = 0x0a0a0202;
            pkt_dst_ipv4 = 0x0a0a0203;
            pkt_l4_src_port = 0x3344;
            pkt_l4_dst_port = 0x5566;
            pkt_protocol = 6;
            pkt_ttl = 0x40;
            pkt_in_vnid = 0;

            /* Create vlan to send packets */
            rv = bcm_vlan_create(unit, pkt_ingress_vlan);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - Packet path: bcm_vlan_create => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
                return rv;
            }

            /* Add ports in vlan */
            BCM_PBMP_CLEAR(pbmp);
            BCM_PBMP_CLEAR(upbmp);
            BCM_PBMP_PORT_ADD(pbmp, traffic_ingress_port);
            BCM_PBMP_PORT_ADD(pbmp, traffic_egress_port);;
            rv = bcm_vlan_port_add(unit, pkt_ingress_vlan, pbmp, upbmp);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - Packet Path: bcm_vlan_port_add => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            break;

        case 2:
            /* ftv2_tx_test_config = 2 */
            traffic_ingress_port = 17;
            traffic_egress_port = 20;
            traffic_pkt_num = 3;
            pkt_ingress_vlan = 30;
            pkt_vlan_format = 3;
            pkt_src_ipv4 = 0x0a0a0202;
            pkt_dst_ipv4 = 0x0a0a0203;
            pkt_l4_src_port = 0x3344;
            pkt_l4_dst_port = 0x5566;
            pkt_protocol = 6;
            pkt_ttl = 0x40;
            pkt_in_vnid = 0;

            /* Create vlan to send packets */
            rv = bcm_vlan_create(unit, pkt_ingress_vlan);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - Packet path: bcm_vlan_create => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
                return rv;
            }

            /* Add ports in vlan */
            BCM_PBMP_CLEAR(pbmp);
            BCM_PBMP_CLEAR(upbmp);
            BCM_PBMP_PORT_ADD(pbmp, traffic_ingress_port);
            BCM_PBMP_PORT_ADD(pbmp, traffic_egress_port);;
            rv = bcm_vlan_port_add(unit, pkt_ingress_vlan, pbmp, upbmp);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - Packet Path: bcm_vlan_port_add => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            break;

        case 3:
            /* ftv2_tx_test_config = 3 */
            traffic_ingress_port = 19;
            traffic_egress_port = 21;
            traffic_pkt_num = 3;
            traffic_pkt_num_received = traffic_pkt_num;
            pkt_ingress_vlan = 30;
            pkt_vlan_format = 3;
            pkt_src_ipv4 = 0x0a0a0202;
            pkt_dst_ipv4 = 0x0a0a0203;
            pkt_l4_src_port = 0x3344;
            pkt_l4_dst_port = 0x5566;
            pkt_protocol = 6;
            pkt_ttl = 0x40;
            pkt_in_vnid = 0;
            break;

        case 4:
            /* ftv2_tx_test_config = 4 */
            traffic_ingress_port = 17;
            traffic_egress_port = 20;
            traffic_pkt_num = 3;
            traffic_pkt_num_received = traffic_pkt_num;
            pkt_ingress_vlan = 30;
            pkt_vlan_format = 3;
            pkt_src_ipv4 = 0x0a0a0202;
            pkt_dst_ipv4 = 0x0a0a0203;
            pkt_l4_src_port = 0x3344;
            pkt_l4_dst_port = 0x5566;
            pkt_protocol = 6;
            pkt_ttl = 0x40;
            pkt_in_vnid = 0;
            break;

        case 5:
            /* ftv2_tx_test_config = 5 */
            traffic_ingress_port = 17;
            traffic_egress_port = 20;
            traffic_pkt_num = 3;
            traffic_pkt_num_received = traffic_pkt_num;
            pkt_ingress_vlan = 30;
            pkt_vlan_format = 3;
            pkt_src_ipv4 = 0x0a0a0202;
            pkt_dst_ipv4 = 0x0a0a0203;
            pkt_l4_src_port = 0x3344;
            pkt_l4_dst_port = 0x5566;
            pkt_protocol = 6;
            pkt_ttl = 0x40;
            pkt_in_vnid = 0;
            break;

        case 6:
            /* ftv2_tx_test_config = 6 */
            traffic_ingress_port = 19;
            traffic_egress_port = 21;
            /* TC expects this value, ALU Triggers export on update on receiving first packet and hence should not be changed */
            traffic_pkt_num = 1;
            traffic_pkt_num_received = traffic_pkt_num;
            pkt_ingress_vlan = 30;
            pkt_vlan_format = 3;
            pkt_src_ipv4 = 0x0a0a0202;
            pkt_dst_ipv4 = 0x0a0a0203;
            /* TC expects this value, should not be changed */
            pkt_l4_src_port = 0x3344;
            pkt_l4_dst_port = 0x5566;
            /* TC expects this value, should not be changed */
            pkt_protocol = 6;
            /* TC expects this value, should not be changed */
            pkt_ttl = 0x40;
            pkt_in_vnid = 0;
            break;

        case 7:
            /* ftv2_tx_test_config = 7 */
            traffic_ingress_port = 17;
            traffic_egress_port = 20;
            traffic_pkt_num = 3;
            traffic_pkt_num_received = traffic_pkt_num;
            pkt_ingress_vlan = 30;
            pkt_vlan_format = 3;
            pkt_src_ipv4 = 0x0a0a0202;
            pkt_dst_ipv4 = 0x0a0a0203;
            pkt_l4_src_port = 0x3344;
            pkt_l4_dst_port = 0x5566;
            pkt_protocol = 6;
            pkt_ttl = 0x40;
            pkt_in_vnid = 0;
            break;

        case 8:
            /* ftv2_tx_test_config = 8 */
            traffic_ingress_port = 17;
            traffic_egress_port = 20;
            traffic_pkt_num = 3;
            traffic_pkt_num_received = traffic_pkt_num;
            pkt_ingress_vlan = 30;
            pkt_vlan_format = 3;
            pkt_src_ipv4 = 0x0a0a0202;
            pkt_dst_ipv4 = 0x0a0a0203;
            pkt_l4_src_port = 0x3344;
            pkt_l4_dst_port = 0x5566;
            pkt_protocol = 6;
            pkt_ttl = 0x40;
            pkt_in_vnid = 0;
            break;

        case 9:
            /* ftv2_tx_test_config = 9 */
            traffic_ingress_port = 47;
            traffic_egress_port = 50;
            traffic_pkt_num = 1;
            traffic_pkt_num_received = traffic_pkt_num;
            pkt_ingress_vlan = 30;
            pkt_vlan_format = 3;
            pkt_src_ipv4 = 0x0a0a0202;
            pkt_dst_ipv4 = 0x0a0a0203;
            pkt_l4_src_port = 0x3344;
            pkt_l4_dst_port = 0x5566;
            pkt_protocol = 6;
            pkt_ttl = 0x40;
            pkt_in_vnid = 0;
            break;

        case 10:
            /* ftv2_tx_test_config = 10 */
            traffic_ingress_port = 28;
            traffic_egress_port = 32;
            traffic_pkt_num = 1;
            traffic_pkt_num_received = traffic_pkt_num;
            pkt_ingress_vlan = 30;
            pkt_vlan_format = 3;
            pkt_src_ipv4 = 0x0a0a0202;
            pkt_dst_ipv4 = 0x0a0a0203;
            pkt_l4_src_port = 0x3344;
            pkt_l4_dst_port = 0x5566;
            pkt_protocol = 6;
            pkt_ttl = 0x40;
            pkt_in_vnid = 0;
            break;

        case 11:
            /* ftv2_tx_test_config = 11 */
            traffic_ingress_port = 28;
            traffic_egress_port = 32;
            traffic_pkt_num = 1;
            traffic_pkt_num_received = traffic_pkt_num;
            pkt_ingress_vlan = 30;
            pkt_vlan_format = 3;
            pkt_src_ipv4 = 0x0a0a0202;
            pkt_dst_ipv4 = 0x0a0a0203;
            pkt_l4_src_port = 0x3344;
            pkt_l4_dst_port = 0x5566;
            pkt_protocol = 6;
            pkt_ttl = 0x40;
            pkt_tcp_window_size = 0x11F5;
            pkt_in_vnid = 0;
            break;

        case 12:
            /* ftv2_tx_test_config = 12 */
            traffic_ingress_port = 47;
            traffic_egress_port = 50;
            traffic_pkt_num = 3;
            traffic_pkt_num_received = traffic_pkt_num;
            pkt_ingress_vlan = 30;
            pkt_vlan_format = 3;
            pkt_src_ipv4 = 0x0a0a0202;
            pkt_dst_ipv4 = 0x0a0a0203;
            pkt_l4_src_port = 0x3344;
            pkt_l4_dst_port = 0x5566;
            pkt_protocol = 6;
            pkt_ttl = 0x40;
            pkt_in_vnid = 0;
            break;

        case 13:
            /* ftv2_tx_test_config = 13 */
            traffic_ingress_port = 29;
            traffic_egress_port = 32;
            traffic_pkt_num = 1;
            /* drop test, received at egress port should be 0 */
            traffic_pkt_num_received = 0;
            pkt_ingress_vlan = 30;
            pkt_vlan_format = 3;
            pkt_src_ipv4 = 0x0a0a0202;
            pkt_dst_ipv4 = 0x0a0a0203;
            pkt_l4_src_port = 0x3344;
            pkt_l4_dst_port = 0x5566;
            pkt_protocol = 6;
            pkt_ttl = 0x40;
            pkt_tcp_window_size = 0x11F5;
            pkt_in_vnid = 0;
            break;

        case 14:
            /* ftv2_tx_test_config = 14 */
            traffic_ingress_port = 29;
            traffic_egress_port = 32;
            traffic_pkt_num = 1;
            /* drop test, received at egress port should be 0 */
            traffic_pkt_num_received = 0;
            pkt_ingress_vlan = 30;
            pkt_vlan_format = 3;
            pkt_src_ipv4 = 0x0a0a0202;
            pkt_dst_ipv4 = 0x0a0a0203;
            pkt_l4_src_port = 0x3344;
            pkt_l4_dst_port = 0x5566;
            pkt_protocol = 6;
            pkt_ttl = 0x40;
            pkt_tcp_window_size = 0x11F5;
            pkt_in_vnid = 0;
            break;

        case 15:
            /* ftv2_tx_test_config = 15 */
            traffic_ingress_port = 47;
            traffic_egress_port = 50;
            traffic_pkt_num = 1;
            traffic_pkt_num_received = traffic_pkt_num;
            pkt_ingress_vlan = 30;
            pkt_vlan_format = 3;
            pkt_src_ipv4 = 0x0a0a0202;
            pkt_dst_ipv4 = 0x0a0a0203;
            pkt_l4_src_port = 0x3344;
            pkt_l4_dst_port = 0x5566;
            pkt_protocol = 6;
            pkt_ttl = 0x40;
            pkt_tcp_window_size = 0x11F5;
            pkt_in_vnid = 0;
            break;

        case 16:
            /* ftv2_tx_test_config = 16 */
            traffic_ingress_port = 29;
            traffic_egress_port = 32;
            traffic_pkt_num = 1;
            /* drop test, received at egress port should be 0 */
            traffic_pkt_num_received = 0;
            pkt_ingress_vlan = 30;
            pkt_vlan_format = 3;
            pkt_src_ipv4 = 0x0a0a0202;
            pkt_dst_ipv4 = 0x0a0a0203;
            pkt_l4_src_port = 0x3344;
            pkt_l4_dst_port = 0x5566;
            pkt_protocol = 6;
            pkt_ttl = 0x40;
            pkt_tcp_window_size = 0x11F5;
            pkt_in_vnid = 0;
            break;

        case 17:
            /* ftv2_tx_test_config = 17 */
            traffic_ingress_port = 29;
            traffic_egress_port = 32;
            traffic_pkt_num = 1;
            /* drop test, received at egress port should be 0 */
            traffic_pkt_num_received = 0;
            pkt_ingress_vlan = 30;
            pkt_vlan_format = 3;
            pkt_src_ipv4 = 0x0a0a0202;
            pkt_dst_ipv4 = 0x0a0a0203;
            pkt_l4_src_port = 0x3344;
            pkt_l4_dst_port = 0x5566;
            pkt_protocol = 6;
            pkt_ttl = 0x40;
            pkt_tcp_window_size = 0x11F5;
            pkt_in_vnid = 0;
            break;

        case 80:
            /* ftv2_tx_test_config = 80 */
            /* IPv6 Traffic */
            traffic_ingress_port = 19;
            traffic_egress_port = 21;
            traffic_pkt_num = 2;
            traffic_pkt_num_received = traffic_pkt_num;
            pkt_ingress_vlan = 30;
            pkt_vlan_format = 0;
            pkt_src_ipv4 = 0;
            pkt_dst_ipv4 = 0;
            pkt_l4_src_port = 0;
            pkt_l4_dst_port = 0;
            pkt_protocol = 6;
            pkt_ttl = 0;
            pkt_in_vnid = 0;
            break;

        case 90:
            /* ftv2_tx_test_config = 90 */
            /* Vxlan Traffic */
            traffic_ingress_port = 19;
            traffic_egress_port = 21;
            traffic_pkt_num = 2;
            traffic_pkt_num_received = traffic_pkt_num;
            pkt_ingress_vlan = 30;
            pkt_vlan_format = 0;
            pkt_src_ipv4 = 0;
            pkt_dst_ipv4 = 0;
            pkt_l4_src_port = 0;
            pkt_l4_dst_port = 0;
            pkt_protocol = 17;
            pkt_ttl = 0;
            pkt_in_vnid = 0x345667;
            break;

        default:
            printf("[CINT] TC %2d - invalid tx test config %d\n",tc_id, ftv2_tx_test_config);
            return BCM_E_FAIL;
    }

    /* Create vlan to send packets */
    rv = bcm_vlan_create(unit, pkt_ingress_vlan);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - Packet path: bcm_vlan_create => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
        return rv;
    }

    /* Add ports in vlan */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, traffic_ingress_port);
    BCM_PBMP_PORT_ADD(pbmp, traffic_egress_port);
    rv = bcm_vlan_port_add(unit, pkt_ingress_vlan, pbmp, upbmp);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - Packet Path: bcm_vlan_port_add => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
        return rv;
    }

    /* Final Step Log */
    if (!skip_log) {
        printf("[CINT] Traffic - Packet path setup Succesfully.\n");
    }

    return BCM_E_NONE;
}

int
ftv2_pkt_path_destroy(int tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;

    /* Skip cleaning up packet path */
    if (skip_cleanup) {
        if (!skip_log) {
            printf("[CINT] Traffic - Skip cleaning packet path ......\n");
        }
        return rv;
    }

    /* Destroy vlan used to send packets */
    if (pkt_ingress_vlan != -1) {

        rv = bcm_vlan_destroy(unit, pkt_ingress_vlan);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] TC %2d - Packet path: bcm_vlan_destroy => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
            return rv;
        }
        traffic_ingress_port = -1;
        traffic_egress_port = -1;
        traffic_pkt_num = 0;
        traffic_pkt_num_received = 0;
        pkt_ingress_vlan = -1;
        pkt_vlan_format = 0;
        pkt_src_ipv4 = 0;
        pkt_dst_ipv4 = 0;
        pkt_l4_src_port = 0;
        pkt_l4_dst_port = 0;
        pkt_protocol = 0;
        pkt_ttl = 0;
        pkt_in_vnid = 0;
    }

    /* Final Step Log */
    if (!skip_log) {
        printf("[CINT] Traffic - Packet path cleared Succesfully.\n");
    }

    return BCM_E_NONE;
}

int
ftv2_test_send_traffic(int tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;
    int sc_flags = 0;
    bcm_field_group_config_t efp_group_config;
    uint32 traffic_pkt_num_get = 0, rx_num_packet1_get = 0;
    uint32 pkt_tx_num2_get = 0, pkt_rx_num2_get = 0;

    /* Skip if no test config set */
    if (!ftv2_tx_test_config) {
        if (!skip_log) {
            printf("[CINT] Traffic - Skip Sending Packet ......\n");
        }
        return rv;
    }

    /* First Set ingress port in loopback */
    rv = bcm_port_loopback_set(unit, traffic_ingress_port, BCM_PORT_LOOPBACK_MAC);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] Traffic - MAC Loopback Set failed on ingress port %d, rv %d(%s)\n",
                traffic_ingress_port, rv, bcm_errmsg(rv));
        return rv;
    }

    /* Now Set ingress port in loopback */
    rv = bcm_port_loopback_set(unit, traffic_egress_port, BCM_PORT_LOOPBACK_MAC);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] Traffic - MAC Loopback Set failed on egress port %d, rv %d(%s)\n",
                traffic_egress_port, rv, bcm_errmsg(rv));
        return rv;
    }

    /* Now discard all packets coming through egress port */
    rv = bcm_port_discard_set(unit, traffic_egress_port, BCM_PORT_DISCARD_ALL);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] Traffic - Port discard all failed on egress port %d, rv %d(%s)\n",
                traffic_egress_port, rv, bcm_errmsg(rv));
        return rv;
    }

    /* Clear stats from ingress port */
    rv = bcm_stat_clear(unit, traffic_ingress_port);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] Traffic - Couldn't Clear stat on ingress port %d, rv %d(%s)\n",
                traffic_ingress_port, rv, bcm_errmsg(rv));
        return rv;
    }

    /* Clear stats from egress port */
    rv = bcm_stat_clear(unit, traffic_egress_port);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] Traffic - Couldn't Clear stat on egress port %d, rv %d(%s)\n",
                traffic_egress_port, rv, bcm_errmsg(rv));
        return rv;
    }

    /* Remember Me - Setup up traffic here as per config */
    switch(ftv2_tx_test_config) {

        case 2:
            /* ftv2_tx_test_config = 2 */
            /* Send traffic */
            rv = tx_broadscan_test_packet(unit, ipv4_tcp_sync_pkt1, sizeof(ipv4_tcp_sync_pkt1), traffic_ingress_port, traffic_pkt_num);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Traffic - Test Traffic Tx failed on port %d, rv %d(%s)\n",
                        traffic_ingress_port, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Traffic - sleeping for %d seconds after tx ..\n",sleep_after_tx);
            }
            sal_sleep(sleep_after_tx);

            /* Sent only one kind of packet, hence set expected flow count */
            expected_group_property_flow_count = 1;
            /* Update pktcount expected value if at all added */
            expected_tparam_pkt_count_value = traffic_pkt_num;
            /* Update bytecount expected value if at all added */
            expected_tparam_byte_count_value = traffic_pkt_num * sizeof(ipv4_tcp_sync_pkt1);
            break;

        case 3:
            /* ftv2_tx_test_config = 3 */
            /* Send traffic */
            rv = tx_broadscan_test_packet(unit, ipv4_tcp_sync_pkt1, sizeof(ipv4_tcp_sync_pkt1), traffic_ingress_port, traffic_pkt_num);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Traffic - Test Traffic Tx failed on port %d, rv %d(%s)\n",
                        traffic_ingress_port, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Traffic - sleeping for %d seconds after tx ..\n",sleep_after_tx);
            }
            sal_sleep(sleep_after_tx);

            /* Sent only one kind of packet, hence set expected flow count */
            expected_group_property_flow_count = 1;
            /* Update pktcount expected value if at all added */
            expected_tparam_pkt_count_value = traffic_pkt_num;
            /* Update bytecount expected value if at all added */
            expected_tparam_byte_count_value = traffic_pkt_num * sizeof(ipv4_tcp_sync_pkt1);
            break;

        case 4:
            /* ftv2_tx_test_config = 4 */
            /* Send traffic */
            rv = tx_broadscan_test_packet(unit, ipv4_tcp_sync_pkt1, sizeof(ipv4_tcp_sync_pkt1), traffic_ingress_port, traffic_pkt_num);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Traffic - Test Traffic Tx failed on port %d, rv %d(%s)\n",
                        traffic_ingress_port, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Traffic - sleeping for %d seconds after tx ..\n",sleep_after_tx);
            }
            sal_sleep(sleep_after_tx);

            /* Sent only one kind of packet, hence set expected flow count */
            expected_group_property_flow_count = 1;
            /* Update pktcount expected value if at all added */
            expected_tparam_pkt_count_value = traffic_pkt_num;
            /* Update bytecount expected value if at all added */
            expected_tparam_byte_count_value = traffic_pkt_num * sizeof(ipv4_tcp_sync_pkt1);
            break;

        case 5:
            /* ftv2_tx_test_config = 5 */
            /* Send traffic */
            rv = tx_broadscan_test_packet(unit, ipv4_tcp_sync_pkt1, sizeof(ipv4_tcp_sync_pkt1), traffic_ingress_port, traffic_pkt_num);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Traffic - Test Traffic Tx failed on port %d, rv %d(%s)\n",
                        traffic_ingress_port, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Traffic - sleeping for %d seconds after tx ..\n",sleep_after_tx);
            }
            sal_sleep(sleep_after_tx);

            /* Sent only one kind of packet, hence set expected flow count */
            expected_group_property_flow_count = 1;
            /* Update pktcount expected value if at all added */
            expected_tparam_pkt_count_value = traffic_pkt_num;
            /* Update bytecount expected value if at all added */
            expected_tparam_byte_count_value = traffic_pkt_num * sizeof(ipv4_tcp_sync_pkt1);
            break;

        case 6:
            /* ftv2_tx_test_config = 6 */
            /* Send traffic */
            rv = tx_broadscan_test_packet(unit, ipv4_tcp_sync_pkt1, sizeof(ipv4_tcp_sync_pkt1), traffic_ingress_port, traffic_pkt_num);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Traffic - Test Traffic Tx failed on port %d, rv %d(%s)\n",
                        traffic_ingress_port, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Traffic - sleeping for %d seconds after tx ..\n",sleep_after_tx);
            }
            sal_sleep(sleep_after_tx);

            /* Sent only one kind of packet, hence set expected flow count */
            expected_group_property_flow_count = 1;
            /* Update pktcount expected value if at all added */
            expected_tparam_pkt_count_value = traffic_pkt_num;
            /* Update bytecount expected value if at all added */
            expected_tparam_byte_count_value = traffic_pkt_num * sizeof(ipv4_tcp_sync_pkt1);
            break;

        case 7:
            /* ftv2_tx_test_config = 7 */
            /* Send traffic */
            rv = tx_broadscan_test_packet(unit, ipv4_tcp_sync_pkt1, sizeof(ipv4_tcp_sync_pkt1), traffic_ingress_port, traffic_pkt_num);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Traffic - Test Traffic Tx failed on port %d, rv %d(%s)\n",
                        traffic_ingress_port, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Traffic - sleeping for %d seconds after tx ..\n",sleep_after_tx);
            }
            sal_sleep(sleep_after_tx);

            /* Sent only one kind of packet, hence set expected flow count */
            expected_group_property_flow_count = 1;
            /* Update pktcount expected value if at all added */
            expected_tparam_pkt_count_value = traffic_pkt_num;
            /* Update bytecount expected value if at all added */
            expected_tparam_byte_count_value = traffic_pkt_num * sizeof(ipv4_tcp_sync_pkt1);
            break;

        case 8:
            /* ftv2_tx_test_config = 8 */
            /* Send traffic */
            rv = tx_broadscan_test_packet(unit, ipv4_tcp_sync_pkt1, sizeof(ipv4_tcp_sync_pkt1), traffic_ingress_port, (traffic_pkt_num - 1));
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Traffic - Test Traffic Tx failed on port %d, rv %d(%s)\n",
                                                traffic_ingress_port, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Traffic - sleeping for %d seconds after tx ..\n",sleep_after_tx);
            }
            sal_sleep(sleep_after_tx);

            /* Update time before sending last packet. */
            /* CMIC Time - Current Time */
            rv = ftv2_time_set(tc_id, 3, 0x09080806, 0);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - ftv2_time_set - legacy => rv %d(%s)\n",
                                                    tc_id, rv, bcm_errmsg(rv));
                return rv;
            }

            rv = tx_broadscan_test_packet(unit, ipv4_tcp_sync_pkt1, sizeof(ipv4_tcp_sync_pkt1), traffic_ingress_port, 1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Traffic - Test Traffic Tx failed on port %d, rv %d(%s)\n",
                                                traffic_ingress_port, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Traffic - sleeping for %d seconds after sending last packet ..\n",sleep_after_tx);
            }
            sal_sleep(sleep_after_tx);

            /* Sent only one kind of packet, hence set expected flow count */
            expected_group_property_flow_count = 1;
            /* Update pktcount expected value if at all added */
            expected_tparam_pkt_count_value = traffic_pkt_num;
            /* Update bytecount expected value if at all added */
            expected_tparam_byte_count_value = traffic_pkt_num * sizeof(ipv4_tcp_sync_pkt1);
            break;

        case 9:
            /* ftv2_tx_test_config = 9 */
            /* Send traffic */
            rv = tx_broadscan_test_packet(unit, ipv4_tcp_sync_pkt1, sizeof(ipv4_tcp_sync_pkt1), traffic_ingress_port, traffic_pkt_num);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Traffic - Test Traffic Tx failed on port %d, rv %d(%s)\n",
                        traffic_ingress_port, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Traffic - sleeping for %d seconds after tx ..\n",sleep_after_tx);
            }
            sal_sleep(sleep_after_tx);

            /* Sent only one kind of packet, hence set expected flow count */
            expected_group_property_flow_count = 1;
            /* Update pktcount expected value if at all added */
            expected_tparam_pkt_count_value = traffic_pkt_num;
            /* Update bytecount expected value if at all added */
            expected_tparam_byte_count_value = traffic_pkt_num * sizeof(ipv4_tcp_sync_pkt1);
            break;

        case 10:
            /* ftv2_tx_test_config = 10 */
            /* Send traffic */
            rv = tx_broadscan_test_packet(unit, ipv4_tcp_sync_pkt1, sizeof(ipv4_tcp_sync_pkt1), traffic_ingress_port, traffic_pkt_num);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Traffic - Test Traffic Tx failed on port %d, rv %d(%s)\n",
                        traffic_ingress_port, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Traffic - sleeping for %d seconds after tx ..\n",sleep_after_tx);
            }
            sal_sleep(sleep_after_tx);

            /* Sent only one kind of packet, hence set expected flow count */
            expected_group_property_flow_count = 1;
            /* Update pktcount expected value if at all added */
            expected_tparam_pkt_count_value = traffic_pkt_num;
            /* Update bytecount expected value if at all added */
            expected_tparam_byte_count_value = traffic_pkt_num * sizeof(ipv4_tcp_sync_pkt1);
            break;

        case 11:
            /* ftv2_tx_test_config = 11 */
            /* Send traffic */
            rv = tx_broadscan_test_packet(unit, ipv4_tcp_sync_pkt1, sizeof(ipv4_tcp_sync_pkt1), traffic_ingress_port, traffic_pkt_num);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Traffic - Test Traffic Tx failed on port %d, rv %d(%s)\n",
                        traffic_ingress_port, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Traffic - sleeping for %d seconds after tx ..\n",sleep_after_tx);
            }
            sal_sleep(sleep_after_tx);

            /* Sent only one kind of packet, hence set expected flow count */
            expected_group_property_flow_count = 1;
            /* Update pktcount expected value if at all added */
            expected_tparam_pkt_count_value = traffic_pkt_num;
            /* Update bytecount expected value if at all added */
            expected_tparam_byte_count_value = traffic_pkt_num * sizeof(ipv4_tcp_sync_pkt1);
            break;

        case 12:
            /* ftv2_tx_test_config = 12 */
            /* Send traffic */
            rv = tx_broadscan_test_packet(unit, ipv4_tcp_sync_pkt1, sizeof(ipv4_tcp_sync_pkt1), traffic_ingress_port, (traffic_pkt_num - 1));
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Traffic - Test Traffic Tx failed on port %d, rv %d(%s)\n",
                        traffic_ingress_port, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Traffic - sleeping for %d seconds after tx ..\n",sleep_after_tx);
            }
            sal_sleep(sleep_after_tx);

            /* Update time before sending last packet. */
            /* CMIC Time - Current Time */
            rv = ftv2_time_set(tc_id, 3, 0x09080806, 0);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - ftv2_time_set - legacy => rv %d(%s)\n",
                                                     tc_id, rv, bcm_errmsg(rv));
                return rv;
            }

            rv = tx_broadscan_test_packet(unit, ipv4_tcp_sync_pkt1, sizeof(ipv4_tcp_sync_pkt1), traffic_ingress_port, 1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Traffic - Test Traffic Tx failed on port %d, rv %d(%s)\n",
                        traffic_ingress_port, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Traffic - sleeping for %d seconds after sending last packet ..\n",sleep_after_tx);
            }
            sal_sleep(sleep_after_tx);

            /* Sent only one kind of packet, hence set expected flow count */
            expected_group_property_flow_count = 1;
            /* Update pktcount expected value if at all added */
            expected_tparam_pkt_count_value = traffic_pkt_num;
            /* Update bytecount expected value if at all added */
            expected_tparam_byte_count_value = traffic_pkt_num * sizeof(ipv4_tcp_sync_pkt1);
            break;

        case 13:
            /* ftv2_tx_test_config = 13 */
            rv = bcm_switch_control_get(unit, bcmSwitchDosAttackMACSAEqualMACDA, &sc_mac_sa_da_def);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Traffic - DosAttackMACSAEqualMACDA switch get failed, rv %d(%s)\n",
                        rv, bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_switch_control_set(unit, bcmSwitchDosAttackMACSAEqualMACDA, 1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Traffic - DosAttackMACSAEqualMACDA switch set failed, rv %d(%s)\n",
                        rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Traffic - SwitchControl for MacSA=MacDA set.\n");
            }

            /* Send traffic */
            rv = tx_broadscan_test_packet(unit, ipv4_tcp_sync_pkt2, sizeof(ipv4_tcp_sync_pkt2), traffic_ingress_port, traffic_pkt_num);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Traffic - Test Traffic Tx failed on port %d, rv %d(%s)\n",
                        traffic_ingress_port, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Traffic - sleeping for %d seconds after tx ..\n",sleep_after_tx);
            }
            sal_sleep(sleep_after_tx);

            /* Sent only one kind of packet, hence set expected flow count */
            expected_group_property_flow_count = 1;
            /* Update pktcount expected value if at all added */
            expected_tparam_pkt_count_value = traffic_pkt_num;
            /* Update bytecount expected value if at all added */
            expected_tparam_byte_count_value = traffic_pkt_num * sizeof(ipv4_tcp_sync_pkt2);
            break;

        case 14:
            /* ftv2_tx_test_config = 14 */
            rv = bcm_switch_control_get(unit, bcmSwitchDosAttackMACSAEqualMACDA, &sc_mac_sa_da_def);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Traffic - DosAttackMACSAEqualMACDA switch get failed, rv %d(%s)\n",
                        rv, bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_switch_control_set(unit, bcmSwitchDosAttackMACSAEqualMACDA, 1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Traffic - DosAttackMACSAEqualMACDA switch set failed, rv %d(%s)\n",
                        rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Traffic - SwitchControl for MacSA=MacDA set.\n");
            }

            /* Send traffic */
            rv = tx_broadscan_test_packet(unit, ipv4_tcp_sync_pkt2, sizeof(ipv4_tcp_sync_pkt2), traffic_ingress_port, traffic_pkt_num);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Traffic - Test Traffic Tx failed on port %d, rv %d(%s)\n",
                        traffic_ingress_port, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Traffic - sleeping for %d seconds after tx ..\n",sleep_after_tx);
            }
            sal_sleep(sleep_after_tx);

            /* Sent only one kind of packet, hence set expected flow count */
            expected_group_property_flow_count = 1;
            /* Update pktcount expected value if at all added */
            expected_tparam_pkt_count_value = traffic_pkt_num;
            /* Update bytecount expected value if at all added */
            expected_tparam_byte_count_value = traffic_pkt_num * sizeof(ipv4_tcp_sync_pkt2);
            break;

        case 15:
            /* ftv2_tx_test_config = 15 */
            /* Send traffic */
            rv = tx_broadscan_test_packet(unit, ipv4_tcp_sync_pkt1, sizeof(ipv4_tcp_sync_pkt1), traffic_ingress_port, traffic_pkt_num);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Traffic - Test Traffic Tx failed on port %d, rv %d(%s)\n",
                        traffic_ingress_port, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Traffic - sleeping for %d seconds after tx ..\n",sleep_after_tx);
            }
            sal_sleep(sleep_after_tx);

            /* Sent only one kind of packet, hence set expected flow count */
            expected_group_property_flow_count = 1;
            /* Update pktcount expected value if at all added */
            expected_tparam_pkt_count_value = traffic_pkt_num;
            /* Update bytecount expected value if at all added */
            expected_tparam_byte_count_value = traffic_pkt_num * sizeof(ipv4_tcp_sync_pkt1);
            break;

        case 16:
            /* ftv2_tx_test_config = 16 */
            /* Write FP Rule in EFP to drop packet */
            bcm_field_group_config_t_init(&efp_group_config);
            BCM_FIELD_QSET_INIT(efp_group_config.qset);
            BCM_FIELD_ASET_INIT(efp_group_config.aset);
            BCM_FIELD_QSET_ADD(efp_group_config.qset, bcmFieldQualifyForwardingVlanId);
            BCM_FIELD_QSET_ADD(efp_group_config.qset, bcmFieldQualifyStageEgress);
            BCM_FIELD_ASET_ADD(efp_group_config.aset, bcmFieldActionDrop);
            rv = bcm_field_group_config_create(unit, &efp_group_config);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Traffic - EFP group configure failed, rv %d(%s)\n",
                                                               rv, bcm_errmsg(rv));
                return rv;
            }
            efp_group = efp_group_config.group;
            rv = bcm_field_entry_create(unit, efp_group, &efp_entry);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Traffic - EFP group entry create failed, rv %d(%s)\n",
                                                                  rv, bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_qualify_ForwardingVlanId(unit, efp_entry, pkt_ingress_vlan, -1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Traffic - EFP group configure failed, rv %d(%s)\n",
                        rv, bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_action_add(unit, efp_entry, bcmFieldActionDrop, 0, 0);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Traffic - EFP group configure failed, rv %d(%s)\n",
                        rv, bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_entry_install(unit, efp_entry);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Traffic - EFP group configure failed, rv %d(%s)\n",
                        rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Traffic - Installed EFP rule group 0x%x, entry 0x%x to drop packet in egress pipline.\n", efp_group, efp_entry);
            }

            /* Send traffic */
            rv = tx_broadscan_test_packet(unit, ipv4_tcp_sync_pkt1, sizeof(ipv4_tcp_sync_pkt1), traffic_ingress_port, traffic_pkt_num);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Traffic - Test Traffic Tx failed on port %d, rv %d(%s)\n",
                        traffic_ingress_port, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Traffic - sleeping for %d seconds after tx ..\n",sleep_after_tx);
            }
            sal_sleep(sleep_after_tx);

            /* Sent only one kind of packet, hence set expected flow count */
            expected_group_property_flow_count = 1;
            /* Update pktcount expected value if at all added */
            expected_tparam_pkt_count_value = traffic_pkt_num;
            /* Update bytecount expected value if at all added */
            expected_tparam_byte_count_value = traffic_pkt_num * sizeof(ipv4_tcp_sync_pkt1);
            break;

        case 17:
            /* ftv2_tx_test_config = 17 */
            /* Write FP Rule in EFP to drop packet */
            bcm_field_group_config_t_init(&efp_group_config);
            BCM_FIELD_QSET_INIT(efp_group_config.qset);
            BCM_FIELD_ASET_INIT(efp_group_config.aset);
            BCM_FIELD_QSET_ADD(efp_group_config.qset, bcmFieldQualifyForwardingVlanId);
            BCM_FIELD_QSET_ADD(efp_group_config.qset, bcmFieldQualifyStageEgress);
            BCM_FIELD_ASET_ADD(efp_group_config.aset, bcmFieldActionDrop);
            rv = bcm_field_group_config_create(unit, &efp_group_config);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Traffic - EFP group configure failed, rv %d(%s)\n",
                        rv, bcm_errmsg(rv));
                return rv;
            }
            efp_group = efp_group_config.group;
            rv = bcm_field_entry_create(unit, efp_group, &efp_entry);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Traffic - EFP group entry create failed, rv %d(%s)\n",
                        rv, bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_qualify_ForwardingVlanId(unit, efp_entry, pkt_ingress_vlan, -1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Traffic - EFP group configure failed, rv %d(%s)\n",
                        rv, bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_action_add(unit, efp_entry, bcmFieldActionDrop, 0, 0);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Traffic - EFP group configure failed, rv %d(%s)\n",
                        rv, bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_entry_install(unit, efp_entry);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Traffic - EFP group configure failed, rv %d(%s)\n",
                        rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Traffic - Installed EFP rule group 0x%x, entry 0x%x to drop packet in egress pipline.\n", efp_group, efp_entry);
            }

            /* Send traffic */
            rv = tx_broadscan_test_packet(unit, ipv4_tcp_sync_pkt1, sizeof(ipv4_tcp_sync_pkt1), traffic_ingress_port, traffic_pkt_num);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Traffic - Test Traffic Tx failed on port %d, rv %d(%s)\n",
                                                traffic_ingress_port, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Traffic - sleeping for %d seconds after tx ..\n",sleep_after_tx);
            }
            sal_sleep(sleep_after_tx);

            /* Sent only one kind of packet, hence set expected flow count */
            expected_group_property_flow_count = 1;
            /* Update pktcount expected value if at all added */
            expected_tparam_pkt_count_value = traffic_pkt_num;
            /* Update bytecount expected value if at all added */
            expected_tparam_byte_count_value = traffic_pkt_num * sizeof(ipv4_tcp_sync_pkt1);
            break;

        case 80:
            /* ftv2_tx_test_config = 80 */
            /* IPv6 traffic */
            /* Send traffic */
            rv = tx_broadscan_test_packet(unit, ipv6_tcp_sync_pkt1, sizeof(ipv6_tcp_sync_pkt1), traffic_ingress_port, traffic_pkt_num);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Traffic - Test Traffic Tx failed on port %d, rv %d(%s)\n",
                        traffic_ingress_port, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Traffic - sleeping for %d seconds after tx ..\n",sleep_after_tx);
            }
            sal_sleep(sleep_after_tx);

            /* Sent only one kind of packet, hence set expected flow count */
            expected_group_property_flow_count = 1;
            /* Update pktcount expected value if at all added */
            expected_tparam_pkt_count_value = traffic_pkt_num;
            /* Update bytecount expected value if at all added */
            expected_tparam_byte_count_value = traffic_pkt_num * sizeof(ipv6_tcp_sync_pkt1);
            break;

        case 90:
            /* ftv2_tx_test_config = 90 */
            /* Vxlan Traffic */
            /* Send traffic */
            rv = tx_broadscan_test_packet(unit, vxlan_vnid_pkt1, sizeof(vxlan_vnid_pkt1), traffic_ingress_port, traffic_pkt_num);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Traffic - Test Traffic Tx failed on port %d, rv %d(%s)\n",
                        traffic_ingress_port, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Traffic - sleeping for %d seconds after tx ..\n",sleep_after_tx);
            }
            sal_sleep(sleep_after_tx);

            /* Sent only one kind of packet, hence set expected flow count */
            expected_group_property_flow_count = 1;
            /* Update pktcount expected value if at all added */
            expected_tparam_pkt_count_value = traffic_pkt_num;
            /* Update bytecount expected value if at all added */
            expected_tparam_byte_count_value = traffic_pkt_num * sizeof(vxlan_vnid_pkt1);
            break;

        default:
            printf("[CINT] TC %2d - invalid tx test config %d\n",tc_id, ftv2_tx_test_config);
            return BCM_E_FAIL;
    }

    /* Get Tx Stat From Port & Validate */
    rv = bcm_stat_get32(unit, traffic_ingress_port, snmpEtherStatsTXNoErrors, &traffic_pkt_num_get);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] Traffic - Couldn't get Tx stat from ingress port %d, rv %d(%s)\n",
                traffic_ingress_port, rv, bcm_errmsg(rv));
        return rv;
    }

    if (traffic_pkt_num_get != traffic_pkt_num) {
        printf("[CINT] Traffic - Tx Count does not match with number of packets sent, check loopback on ingress port ....\n");
        return (BCM_E_FAIL);
    }

    /* Get Rx Stat From Port & Validate */
    rv = bcm_stat_get32(unit, traffic_ingress_port, snmpEtherStatsRXNoErrors, &rx_num_packet1_get);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] Traffic - Couldn't get Rx stat from ingress port %d, rv %d(%s)\n",
                traffic_ingress_port, rv, bcm_errmsg(rv));
        return rv;
    }
    if (rx_num_packet1_get != traffic_pkt_num) {
        printf("[CINT] Traffic - Rx Count does not match with number of packets sent, check loopback on ingress port ....\n");
        return (BCM_E_FAIL);
    }

    /* Get Tx Stat From Egress Port & Validate */
    rv = bcm_stat_get32(unit, traffic_egress_port, snmpEtherStatsTXNoErrors, &pkt_tx_num2_get);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] Traffic - Couldn't get Tx stat from port %d, rv %d(%s)\n",
                traffic_egress_port, rv, bcm_errmsg(rv));
        return rv;
    }
    if (pkt_tx_num2_get != traffic_pkt_num_received) {
        printf("[CINT] Traffic - Tx Count does not match with number of packets sent, check egress port ....\n");
        return (BCM_E_FAIL);
    }

    /* Get Rx Stat From Port & Validate */
    rv = bcm_stat_get32(unit, traffic_egress_port, snmpEtherStatsRXNoErrors, &pkt_rx_num2_get);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] Traffic - Couldn't get Rx stat from egress port %d, rv %d(%s)\n",
                traffic_egress_port, rv, bcm_errmsg(rv));
        return rv;
    }
    if (pkt_rx_num2_get != traffic_pkt_num_received) {
        printf("[CINT] Traffic - Rx Count does not match with number of packets sent, check loopback on egress port ....\n");
        return (BCM_E_FAIL);
    }

    /* Final Step Log */
    if (!skip_log) {
        printf("[CINT] Traffic - %d Packets Sent from ingress port %d\n",traffic_pkt_num,traffic_ingress_port);
    }

    return BCM_E_NONE;
}

int
ftv2_test_cleanup_traffic(int tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;
    int sc_flags = 0;

    /* Skip cleaning up traffic setting */
    if (skip_cleanup) {
        if (!skip_log) {
            printf("[CINT] Traffic - Skip cleaning tx setting ......\n");
        }
        return rv;
    }

    expected_group_property_flow_count = 0;
    expected_tparam_pkt_count_value = 0;
    expected_tparam_byte_count_value = 0;

    /* First Set Port in Loopback */
    if (traffic_ingress_port != -1) {

        rv = bcm_port_loopback_set(unit, traffic_ingress_port, BCM_PORT_LOOPBACK_NONE);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] Traffic - Loopback Set to NONE failed on port %d, rv %d(%s)\n",
                    traffic_ingress_port, rv, bcm_errmsg(rv));
            return rv;
        }
    }

    if (traffic_egress_port != -1) {

        /* remove discard setting for packets on egress port */
        rv = bcm_port_discard_set(unit, traffic_egress_port, BCM_PORT_DISCARD_NONE);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] Traffic - Port discard none failed on egress port %d, rv %d(%s)\n",
                                                      traffic_egress_port, rv, bcm_errmsg(rv));
            return rv;
        }

        rv = bcm_port_loopback_set(unit, traffic_egress_port, BCM_PORT_LOOPBACK_NONE);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] Traffic - Loopback Set to NONE failed on port %d, rv %d(%s)\n",
                                                 traffic_egress_port, rv, bcm_errmsg(rv));
            return rv;
        }
    }

    if (sc_mac_sa_da_def != -1) {
        rv = bcm_switch_control_set(unit, bcmSwitchDosAttackMACSAEqualMACDA, sc_mac_sa_da_def);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] Traffic - DosAttackMACSAEqualMACDA switch set failed, rv %d(%s)\n",
                    rv, bcm_errmsg(rv));
            return rv;
        }
        sc_mac_sa_da_def = -1;
    }

    /* Destroy EFP Group Entries */
    if (efp_entry != -1) {
        rv = bcm_field_entry_destroy(unit, efp_entry);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] TC %2d - bcm_field_entry_destroy => rv %d(%s)\n",
                                                    tc_id,rv,bcm_errmsg(rv));
            return rv;
        }
        if (!skip_log) {
            printf("[CINT] Cleanup - EFP Group Entry Destroyed: 0x%08x\n",efp_entry);
        }
        efp_entry = -1;
    }

    /* Destroy EFP group */
    if (efp_group != -1) {
        rv = bcm_field_group_destroy(unit, efp_group);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] TC %2d - bcm_field_group_destroy => rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
            return rv;
        }
        if (!skip_log) {
            printf("[CINT] Cleanup - EFP Group destroyed: 0x%x\n", efp_group);
        }
        efp_group = -1;
    }

    /* Final Step Log */
    if (!skip_log) {
        printf("[CINT] Traffic - Traffic setting cleaned up succesfully.\n");
    }

    return BCM_E_NONE;
}
