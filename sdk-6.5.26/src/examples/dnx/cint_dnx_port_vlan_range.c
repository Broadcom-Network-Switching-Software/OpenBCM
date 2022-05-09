/* $Id: cint_dnx_port_vlan_range.c
 * $
 *
 * File: cint_dnx_port_vlan_range.c
 * Purpose: An example of how to match incoming VLAN using VLAN range
 *          The following CINT provides a calling sequence example to set VLAN range of port to match incoming VLAN,
 *          and bring up one main services 1:1 Model.
 *
 * Calling sequence:
 *
 * Initialization:
 *  1. Initialize service models.
 *  2. Set ports VLAN domain.
 *  3. Set all ports to recognize single stag, single ctag and s_c_tag frames.
 *
 * 1:1 Service:
 * Set up sequence:
 *  1. Add VLAN range info ports. Need to add VLAN range info before creating LIF.
 *        - Call bcm_vlan_translate_action_range_add() with action bcmVlanActionCompressed.
 *  2. Create LIFs
 *        - Call bcm_vlan_port_create() with following criterias:
 *          BCM_VLAN_PORT_MATCH_PORT_VLAN, BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED
 *  3. Cross connect the 2 LIFs
 *        - Call bcm_vswitch_cross_connect_add()
 * Clean up sequence:
 *  1. Delete the cross connected LIFs.
 *        - Call bcm_vswitch_cross_connect_delete()
 *  2. Delete VLAN range info from ports
 *        - Call bcm_vlan_translate_action_range_delete()
 *  3. Delete LIFs.
 *        - Call bcm_vlan_port_destroy()
 *
 * Service Model:
 * 1:1 Service:
 *    Port_1  <-----------------------------  CrossConnect  ------------>  Port_2
 *    SVLAN range 10~20  <----------------------------------------------> SVLAN range 30~40
 *    SVLAN range 50~60 CVLAN range 70~80 <-----------------------------> SVLAN range 90~100 CVLAN range 110~120
 *
 * Traffic within VLAN range:
 *  1. Port_1 SVLAN range 10~20 <-CrossConnect-> Port_2 SVLAN range 30~40
 *        - From Port_1:
 *              -   ethernet header with any DA, SA
 *              -   VLAN tag: VLAN tag type 0x9100, VID = 10
 *        - From Port_2:
 *              -   ethernet header with any DA, SA
 *              -   VLAN tag: VLAN tag type 0x9100, VID = 30
 *        - From Port_1:
 *              -   ethernet header with any DA, SA
 *              -   VLAN tag: VLAN tag type 0x9100, VID = 15
 *        - From Port_2:
 *              -   ethernet header with any DA, SA
 *              -   VLAN tag: VLAN tag type 0x9100, VID = 35
 *        - From Port_1:
 *              -   ethernet header with any DA, SA
 *              -   VLAN tag: VLAN tag type 0x9100, VID = 20
 *        - From Port_2:
 *              -   ethernet header with any DA, SA
 *              -   VLAN tag: VLAN tag type 0x9100, VID = 40
 *
 *  2. Port_1 SVLAN range 50~60 CVLAN range 70~80 <-CrossConnect-> Port_2 SVLAN range 90~100 CVLAN range 110~120
 *        - From Port_1:
 *              -   ethernet header with any DA, SA
 *              -   VLAN tag: VLAN tag type 0x9100, VID = 50, VLAN tag type 0x8100, VID = 70
 *        - From Port_2:
 *              -   ethernet header with any DA, SA
 *              -   VLAN tag: VLAN tag type 0x9100, VID = 90, VLAN tag type 0x8100, VID = 110
 *        - From Port_1:
 *              -   ethernet header with any DA, SA
 *              -   VLAN tag: VLAN tag type 0x9100, VID = 55, VLAN tag type 0x8100, VID = 75
 *        - From Port_2:
 *              -   ethernet header with any DA, SA
 *              -   VLAN tag: VLAN tag type 0x9100, VID = 95, VLAN tag type 0x8100, VID = 115
 *        - From Port_1:
 *              -   ethernet header with any DA, SA
 *              -   VLAN tag: VLAN tag type 0x9100, VID = 60, VLAN tag type 0x8100, VID = 80
 *        - From Port_2:
 *              -   ethernet header with any DA, SA
 *              -   VLAN tag: VLAN tag type 0x9100, VID = 100,VLAN tag type 0x8100, VID = 120
 *
 * Traffic out of VLAN range, can't be matched. All are dropped:
 *  1. Port_1 SVLAN range 10~20 <-CrossConnect-> Port_2 SVLAN range 30~40
 *        - From Port_1:
 *              -   ethernet header with any DA, SA
 *              -   VLAN tag: VLAN tag type 0x9100, VID = 5
 *        - From Port_2:
 *              -   ethernet header with any DA, SA
 *              -   VLAN tag: VLAN tag type 0x9100, VID = 25
 *        - From Port_1:
 *              -   ethernet header with any DA, SA
 *              -   VLAN tag: VLAN tag type 0x9100, VID = 25
 *        - From Port_2:
 *              -   ethernet header with any DA, SA
 *              -   VLAN tag: VLAN tag type 0x9100, VID = 45
 *
 *  2. Port_1 SVLAN range 50~60 CVLAN range 70~80 <-CrossConnect-> Port_2 SVLAN range 90~100 CVLAN range 110~120
 *        - From Port_1:
 *              -   ethernet header with any DA, SA
 *              -   VLAN tag: VLAN tag type 0x9100, VID = 45, VLAN tag type 0x8100, VID = 70
 *        - From Port_2:
 *              -   ethernet header with any DA, SA
 *              -   VLAN tag: VLAN tag type 0x9100, VID = 85, VLAN tag type 0x8100, VID = 110
 *        - From Port_1:
 *              -   ethernet header with any DA, SA
 *              -   VLAN tag: VLAN tag type 0x9100, VID = 50, VLAN tag type 0x8100, VID = 65
 *        - From Port_2:
 *              -   ethernet header with any DA, SA
 *              -   VLAN tag: VLAN tag type 0x9100, VID = 90, VLAN tag type 0x8100, VID = 105
 *        - From Port_1:
 *              -   ethernet header with any DA, SA
 *              -   VLAN tag: VLAN tag type 0x9100, VID = 45, VLAN tag type 0x8100, VID = 65
 *        - From Port_2:
 *              -   ethernet header with any DA, SA
 *              -   VLAN tag: VLAN tag type 0x9100, VID = 85, VLAN tag type 0x8100, VID = 105
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_global.c
 * cint ../../src/./examples/dnx/cint_dnx_port_vlan_range.c
 * cint
 * vlan_range_init(0,200,201);
 * vlan_range_1_1_service(0);
 * exit;
 *
 * send pkt with svlan 10 from port1 to port2
 * tx 1 psrc=200 data=0x0000d64f1d3b00007d26e15a9100000affff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x0000d64f1d3b00007d26e15a9100000affff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * send pkt with svlan 30 from port2 to port1
 * tx 1 psrc=201 data=0x00007d26e15a0000d64f1d3b9100001effff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x00007d26e15a0000d64f1d3b9100001effff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * send pkt with svlan 11 from port1 to port2
 * tx 1 psrc=200 data=0x00006c033da30000e86c99d09100000bffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x00006c033da30000e86c99d09100000bffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * send pkt with svlan 31 from port2 to port1
 * tx 1 psrc=201 data=0x0000e86c99d000006c033da39100001fffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x0000e86c99d000006c033da39100001fffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * send pkt with svlan 12 from port1 to port2
 * tx 1 psrc=200 data=0x0000410a077e0000808020609100000cffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x0000410a077e0000808020609100000cffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * send pkt with svlan 32 from port2 to port1
 * tx 1 psrc=201 data=0x0000808020600000410a077e91000020ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x0000808020600000410a077e91000020ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * send pkt with svlan 13 from port1 to port2
 * tx 1 psrc=200 data=0x00000245d0a20000c12337bc9100000dffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x00000245d0a20000c12337bc9100000dffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * send pkt with svlan 33 from port2 to port1
 * tx 1 psrc=201 data=0x0000c12337bc00000245d0a291000021ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x0000c12337bc00000245d0a291000021ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * send pkt with svlan 14 from port1 to port2
 * tx 1 psrc=200 data=0x000016358f1000002b50acad9100000effff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x000016358f1000002b50acad9100000effff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * send pkt with svlan 34 from port2 to port1
 * tx 1 psrc=201 data=0x00002b50acad000016358f1091000022ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x00002b50acad000016358f1091000022ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * send pkt with svlan 15 from port1 to port2
 * tx 1 psrc=200 data=0x00003cf397210000a03ed84b9100000fffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x00003cf397210000a03ed84b9100000fffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * send pkt with svlan 35 from port2 to port1
 * tx 1 psrc=201 data=0x0000a03ed84b00003cf3972191000023ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x0000a03ed84b00003cf3972191000023ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * send pkt with svlan 16 from port1 to port2
 * tx 1 psrc=200 data=0x00003cf87d260000039f590f91000010ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x00003cf87d260000039f590f91000010ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * send pkt with svlan 36 from port2 to port1
 * tx 1 psrc=201 data=0x0000039f590f00003cf87d2691000024ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x0000039f590f00003cf87d2691000024ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * send pkt with svlan 17 from port1 to port2
 * tx 1 psrc=200 data=0x000059ee310e00009021784e91000011ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x000059ee310e00009021784e91000011ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * send pkt with svlan 37 from port2 to port1
 * tx 1 psrc=201 data=0x00009021784e000059ee310e91000025ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x00009021784e000059ee310e91000025ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * send pkt with svlan 18 from port1 to port2
 * tx 1 psrc=200 data=0x0000ab8839fb00001af3a5f191000012ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x0000ab8839fb00001af3a5f191000012ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * send pkt with svlan 38 from port2 to port1
 * tx 1 psrc=201 data=0x00001af3a5f10000ab8839fb91000026ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x00001af3a5f10000ab8839fb91000026ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * send pkt with svlan 19 from port1 to port2
 * tx 1 psrc=200 data=0x0000239019c500003a3b2d6c91000013ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x0000239019c500003a3b2d6c91000013ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * send pkt with svlan 39 from port2 to port1
 * tx 1 psrc=201 data=0x00003a3b2d6c0000239019c591000027ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x00003a3b2d6c0000239019c591000027ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * send pkt with svlan 20 from port1 to port2
 * tx 1 psrc=200 data=0x00002409904000008ce1de6191000014ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x00002409904000008ce1de6191000014ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * send pkt with svlan 40 from port2 to port1
 * tx 1 psrc=201 data=0x00008ce1de6100002409904091000028ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x00008ce1de6100002409904091000028ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * send pkt with svlan 50 cvlan 70 from port1 to port2
 * tx 1 psrc=200 data=0x00009d9a38520000b8bfdaee9100003281000046ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x00009d9a38520000b8bfdaee9100003281000046ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2000000000000000
 *
 * send pkt with svlan 90 cvlan 110 from port2 to port1
 * tx 1 psrc=201 data=0x0000b8bfdaee00009d9a38529100005a8100006effff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x0000b8bfdaee00009d9a38529100005a8100006effff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2000000000000000
 *
 * send pkt with svlan 51 cvlan 71 from port1 to port2
 * tx 1 psrc=200 data=0x00005408d56c0000825f01589100003381000047ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x00005408d56c0000825f01589100003381000047ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2000000000000000
 *
 * send pkt with svlan 91 cvlan 111 from port2 to port1
 * tx 1 psrc=201 data=0x0000825f015800005408d56c9100005b8100006fffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x0000825f015800005408d56c9100005b8100006fffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2000000000000000
 *
 * send pkt with svlan 52 cvlan 72 from port1 to port2
 * tx 1 psrc=200 data=0x000009618d230000d26245c59100003481000048ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x000009618d230000d26245c59100003481000048ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2000000000000000
 *
 * send pkt with svlan 92 cvlan 112 from port2 to port1
 * tx 1 psrc=201 data=0x0000d26245c5000009618d239100005c81000070ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x0000d26245c5000009618d239100005c81000070ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2000000000000000
 *
 * send pkt with svlan 53 cvlan 73 from port1 to port2
 * tx 1 psrc=200 data=0x0000d662f75600004569f82c9100003581000049ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x0000d662f75600004569f82c9100003581000049ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2000000000000000
 *
 * send pkt with svlan 93 cvlan 113 from port2 to port1
 * tx 1 psrc=201 data=0x00004569f82c0000d662f7569100005d81000071ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x00004569f82c0000d662f7569100005d81000071ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2000000000000000
 *
 * send pkt with svlan 54 cvlan 74 from port1 to port2
 * tx 1 psrc=200 data=0x000056cc77540000b46513f4910000368100004affff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x000056cc77540000b46513f4910000368100004affff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2000000000000000
 *
 * send pkt with svlan 94 cvlan 114 from port2 to port1
 * tx 1 psrc=201 data=0x0000b46513f4000056cc77549100005e81000072ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x0000b46513f4000056cc77549100005e81000072ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2000000000000000
 *
 * send pkt with svlan 55 cvlan 75 from port1 to port2
 * tx 1 psrc=200 data=0x000033c3068100009855f6cd910000378100004bffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x000033c3068100009855f6cd910000378100004bffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2000000000000000
 *
 * send pkt with svlan 95 cvlan 115 from port2 to port1
 * tx 1 psrc=201 data=0x00009855f6cd000033c306819100005f81000073ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x00009855f6cd000033c306819100005f81000073ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2000000000000000
 *
 * send pkt with svlan 56 cvlan 76 from port1 to port2
 * tx 1 psrc=200 data=0x000018ac12ea00006feba52e910000388100004cffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x000018ac12ea00006feba52e910000388100004cffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2000000000000000
 *
 * send pkt with svlan 96 cvlan 116 from port2 to port1
 * tx 1 psrc=201 data=0x00006feba52e000018ac12ea9100006081000074ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x00006feba52e000018ac12ea9100006081000074ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2000000000000000
 *
 * send pkt with svlan 57 cvlan 77 from port1 to port2
 * tx 1 psrc=200 data=0x000073725191000075f0638e910000398100004dffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x000073725191000075f0638e910000398100004dffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2000000000000000
 *
 * send pkt with svlan 97 cvlan 117 from port2 to port1
 * tx 1 psrc=201 data=0x000075f0638e0000737251919100006181000075ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x000075f0638e0000737251919100006181000075ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2000000000000000
 *
 * send pkt with svlan 58 cvlan 78 from port1 to port2
 * tx 1 psrc=200 data=0x0000bbf4081e0000f712c3889100003a8100004effff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x0000bbf4081e0000f712c3889100003a8100004effff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2000000000000000
 *
 * send pkt with svlan 98 cvlan 118 from port2 to port1
 * tx 1 psrc=201 data=0x0000f712c3880000bbf4081e9100006281000076ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x0000f712c3880000bbf4081e9100006281000076ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2000000000000000
 *
 * send pkt with svlan 59 cvlan 79 from port1 to port2
 * tx 1 psrc=200 data=0x0000434d0a0600000973bfd69100003b8100004fffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x0000434d0a0600000973bfd69100003b8100004fffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2000000000000000
 *
 * send pkt with svlan 99 cvlan 119 from port2 to port1
 * tx 1 psrc=201 data=0x00000973bfd60000434d0a069100006381000077ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x00000973bfd60000434d0a069100006381000077ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2000000000000000
 *
 * send pkt with svlan 60 cvlan 80 from port1 to port2
 * tx 1 psrc=200 data=0x000078717bcf00007cfc42d39100003c81000050ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x000078717bcf00007cfc42d39100003c81000050ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2000000000000000
 *
 * send pkt with svlan 100 cvlan 120 from port2 to port1
 * tx 1 psrc=201 data=0x00007cfc42d3000078717bcf9100006481000078ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x00007cfc42d3000078717bcf9100006481000078ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2000000000000000
 */

enum vlan_range_service_mode_e {
    stag_to_stag2,         /*SVLAN <-> SVLAN'*/
    s_c_tag_to_s2_c_tag    /*SVLAN + CVLAN <-> SVLAN' + CVLAN*/
};

struct port_info_s {
    bcm_gport_t port;
    bcm_gport_t lif_gport;
    bcm_vlan_t  svlan_low;
    bcm_vlan_t  svlan_high;
    bcm_vlan_t  cvlan_low;
    bcm_vlan_t  cvlan_high;
};

struct vlan_range_service_info_s {
    int service_mode;
    port_info_s port_info[2];
};

vlan_range_service_info_s vlan_range_service_info[2];
uint32 num_of_vlan_range_service;

/*
 *Initialize the service models and set up PON application configurations.
 */
int vlan_range_init(int unit, bcm_port_t port_1, bcm_port_t port_2)
{
    int index;

    /* port_1 SVLAN range 10~20 <-CrossConnect-> port_2 SVLAN range 30~40 */
    index = 0;
    vlan_range_service_info[index].service_mode = stag_to_stag2;
    vlan_range_service_info[index].port_info[0].port = port_1;
    vlan_range_service_info[index].port_info[0].svlan_low = 10;
    vlan_range_service_info[index].port_info[0].svlan_high = 20;
    vlan_range_service_info[index].port_info[0].cvlan_low = BCM_VLAN_INVALID;
    vlan_range_service_info[index].port_info[0].cvlan_high = BCM_VLAN_INVALID;

    vlan_range_service_info[index].port_info[1].port = port_2;
    vlan_range_service_info[index].port_info[1].svlan_low = 30;
    vlan_range_service_info[index].port_info[1].svlan_high = 40;
    vlan_range_service_info[index].port_info[1].cvlan_low = BCM_VLAN_INVALID;
    vlan_range_service_info[index].port_info[1].cvlan_high = BCM_VLAN_INVALID;

    /* port_1 SVLAN range 50~60 CVLAN range 70~80 <-CrossConnect-> port_2 SVLAN range 90~100 CVLAN range 110~120 */
    index++;
    vlan_range_service_info[index].service_mode = s_c_tag_to_s2_c_tag;

    vlan_range_service_info[index].port_info[0].port = port_1;
    vlan_range_service_info[index].port_info[0].svlan_low = 50;
    vlan_range_service_info[index].port_info[0].svlan_high = 60;
    vlan_range_service_info[index].port_info[0].cvlan_low = 70;
    vlan_range_service_info[index].port_info[0].cvlan_high = 80;

    vlan_range_service_info[index].port_info[1].port = port_2;
    vlan_range_service_info[index].port_info[1].svlan_low = 90;
    vlan_range_service_info[index].port_info[1].svlan_high = 100;
    vlan_range_service_info[index].port_info[1].cvlan_low = 110;
    vlan_range_service_info[index].port_info[1].cvlan_high = 120;

    num_of_vlan_range_service = index+1;

    return BCM_E_NONE;
}

/*
 * Create LIF according to the LIF type.
 */
int vlan_range_lif_create(int unit, int service_index, int port_index)
{
    int rv;
    int service_mode;
    bcm_vlan_port_t vlan_port;

    service_mode = vlan_range_service_info[service_index].service_mode;

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.port = vlan_range_service_info[service_index].port_info[port_index].port;

    switch(service_mode) {
        case stag_to_stag2:
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
            vlan_port.match_vlan  = vlan_range_service_info[service_index].port_info[port_index].svlan_low;
            break;
        case s_c_tag_to_s2_c_tag:
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
            vlan_port.match_vlan  = vlan_range_service_info[service_index].port_info[port_index].svlan_low;
            vlan_port.match_inner_vlan  = vlan_range_service_info[service_index].port_info[port_index].cvlan_low;
            break;
        default:
            printf("ERR: nni_lif_create INVALID PARAMETER lif_type %d\n", service_mode);
            return BCM_E_PARAM;
    }

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("error, bcm_vlan_port_create!\n");
        return rv;
    }

    vlan_range_service_info[service_index].port_info[port_index].lif_gport = vlan_port.vlan_port_id;
    return rv;
}

/*
 * Add VLAN range info to port.
 */
int vlan_range_action_add(int unit, int service_index, int port_index)
{
    int rv;
    bcm_port_t port;
    int service_mode;
    bcm_vlan_t outer_vlan_low;
    bcm_vlan_t outer_vlan_high;
    bcm_vlan_t inner_vlan_low;
    bcm_vlan_t inner_vlan_high;
    bcm_vlan_action_set_t action;

    service_mode = vlan_range_service_info[service_index].service_mode;
    port = vlan_range_service_info[service_index].port_info[port_index].port;
    outer_vlan_low = vlan_range_service_info[service_index].port_info[port_index].svlan_low;
    outer_vlan_high= vlan_range_service_info[service_index].port_info[port_index].svlan_high;
    inner_vlan_low = vlan_range_service_info[service_index].port_info[port_index].cvlan_low;
    inner_vlan_high= vlan_range_service_info[service_index].port_info[port_index].cvlan_high;

    bcm_vlan_action_set_t_init(&action);
    if (service_mode == stag_to_stag2) {
        action.ot_outer = bcmVlanActionCompressed;
        action.new_outer_vlan = outer_vlan_low;
    } else if (service_mode == s_c_tag_to_s2_c_tag) {
        action.dt_outer = bcmVlanActionCompressed;
        action.new_outer_vlan = outer_vlan_low;
        action.dt_inner = bcmVlanActionCompressed;
        action.new_inner_vlan = inner_vlan_low;
    }

    rv = bcm_vlan_translate_action_range_add(unit, port, outer_vlan_low, outer_vlan_high, inner_vlan_low, inner_vlan_high, &action);
    if (rv != BCM_E_NONE)
    {
        printf("error, bcm_vlan_translate_action_range_add!\n");
        return rv;
    }

    return rv;
}

/*
 * Get VLAN range info from port.
 */
int vlan_range_action_get(int unit, int service_index, int port_index)
{
    int rv;
    bcm_port_t port;
    int service_mode;
    bcm_vlan_t outer_vlan_low;
    bcm_vlan_t outer_vlan_high;
    bcm_vlan_t inner_vlan_low;
    bcm_vlan_t inner_vlan_high;
    bcm_vlan_action_set_t action;

    service_mode = vlan_range_service_info[service_index].service_mode;
    port = vlan_range_service_info[service_index].port_info[port_index].port;
    outer_vlan_low = vlan_range_service_info[service_index].port_info[port_index].svlan_low;
    outer_vlan_high= vlan_range_service_info[service_index].port_info[port_index].svlan_high;
    inner_vlan_low = vlan_range_service_info[service_index].port_info[port_index].cvlan_low;
    inner_vlan_high= vlan_range_service_info[service_index].port_info[port_index].cvlan_high;

    bcm_vlan_action_set_t_init(&action);
    if (service_mode == stag_to_stag2) {
        action.ot_outer = bcmVlanActionCompressed;
        action.new_outer_vlan = outer_vlan_low;
    } else if (service_mode == s_c_tag_to_s2_c_tag) {
        action.dt_outer = bcmVlanActionCompressed;
        action.new_outer_vlan = outer_vlan_low;
        action.dt_inner = bcmVlanActionCompressed;
        action.new_inner_vlan = inner_vlan_low;
    }

    bcm_vlan_action_set_t_init(&action);
    rv = bcm_vlan_translate_action_range_get(unit, port, outer_vlan_low, outer_vlan_high, inner_vlan_low, inner_vlan_high, &action);
    if (rv != BCM_E_NONE)
    {
        printf("error, bcm_vlan_translate_action_range_get!\n");
        return rv;
    }

    if ((service_mode == stag_to_stag2) && ((action.ot_outer != bcmVlanActionCompressed) || (action.new_outer_vlan != outer_vlan_low))) {
        printf("Err, VLAN range action not correct!\n");
        return BCM_E_FAIL;
    } else if ((service_mode == s_c_tag_to_s2_c_tag) &&
              ((action.dt_outer != bcmVlanActionCompressed) ||
              (action.new_outer_vlan != outer_vlan_low) ||
              (action.dt_inner != bcmVlanActionCompressed) ||
              (action.new_inner_vlan != inner_vlan_low))) {
        printf("Err, VLAN range action not correct!\n");
        return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}

/*
 * Delete VLAN range info from port.
 */
int vlan_range_action_delete(int unit, int service_index, int port_index)
{
    int rv;
    bcm_port_t port;
    bcm_vlan_t outer_vlan_low;
    bcm_vlan_t outer_vlan_high;
    bcm_vlan_t inner_vlan_low;
    bcm_vlan_t inner_vlan_high;

    port = vlan_range_service_info[service_index].port_info[port_index].port;
    outer_vlan_low = vlan_range_service_info[service_index].port_info[port_index].svlan_low;
    outer_vlan_high= vlan_range_service_info[service_index].port_info[port_index].svlan_high;
    inner_vlan_low = vlan_range_service_info[service_index].port_info[port_index].cvlan_low;
    inner_vlan_high= vlan_range_service_info[service_index].port_info[port_index].cvlan_high;
    rv = bcm_vlan_translate_action_range_delete(unit, port, outer_vlan_low, outer_vlan_high, inner_vlan_low, inner_vlan_high);
    if (rv != BCM_E_NONE)
    {
        printf("error, bcm_vlan_translate_action_range_delete!\n");
        return rv;
    }

    return rv;
}

/*
 * Delete all VLAN range info from port.
 */
int vlan_range_action_delete_all(int unit)
{
    int rv;

    rv = bcm_vlan_translate_action_range_delete_all(unit);
    if (rv != BCM_E_NONE)
    {
        printf("error, bcm_vlan_translate_action_range_delete_all!\n");
        return rv;
    }

    return rv;
}

/*
 * Set up 1:1 sercies, using port cross connect. Match VLAN using VLAN range.
 */
int vlan_range_1_1_service(int unit)
{
    int rv;
    int service_index, port_index;
    bcm_vswitch_cross_connect_t gports;

    for (service_index = 0; service_index < num_of_vlan_range_service; service_index++)
    {
        for (port_index = 0; port_index < 2; port_index++)
        {
            /* Add port VLAN range action */
            vlan_range_action_add(unit, service_index, port_index);

            /* Create LIF */
            vlan_range_lif_create(unit, service_index, port_index);
        }

        /* Cross connect the 2 LIFs */
        bcm_vswitch_cross_connect_t_init(&gports);
        gports.port1 = vlan_range_service_info[service_index].port_info[0].lif_gport;
        gports.port2 = vlan_range_service_info[service_index].port_info[1].lif_gport;
        rv = bcm_vswitch_cross_connect_add(unit, &gports);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_cross_connect_add\n");
            return rv;
        }
    }

    return rv;
}

/*
 * clean up the configurations of 1:1 sercies. Clear VLAN range info.
 */
int vlan_range_1_1_service_cleanup(int unit)
{
    int rv;
    int service_index, port_index;
    bcm_vswitch_cross_connect_t gports;

    for (service_index = 0; service_index < num_of_vlan_range_service; service_index++)
    {
        /* Delete the cross connected LIFs */
        gports.port1 = vlan_range_service_info[service_index].port_info[0].lif_gport;
        gports.port2 = vlan_range_service_info[service_index].port_info[1].lif_gport;
        rv = bcm_vswitch_cross_connect_delete(unit, &gports);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_cross_connect_delete\n");
            print rv;
            return rv;
        }

        for (port_index = 0; port_index < 2; port_index++)
        {
            /* Delete PON LIF VLAN range action */
            vlan_range_action_delete(unit, service_index, port_index);

            /* Delete PON LIF */
            rv = bcm_vlan_port_destroy(unit, vlan_range_service_info[service_index].port_info[port_index].lif_gport);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_vlan_port_destroy \n");
                return rv;
            }
        }
    }

    return rv;
}

void vlan_range_run_with_default()
{
    int unit = 0;
    int port_1 = 1;
    int port_2 = 3;

    vlan_range_init(unit, port_1, port_2);
    vlan_range_1_1_service(unit);
}
