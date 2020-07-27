/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenSDK/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : HR4 Broadscan 2.0 track and export
 *
 * Usage    : BCM.0> cint bs2_track_and_export.c
 *
 * config   : BCM56275_A1-PORT-FLEXFLOW_BS2_0.bcm
 *
 * Log file : bs2_track_and_export_log.txt
 *
 * Test Topology :
 *
 *                                  +----------------------+
 *                                  |                      |          
 *                                  |                      |  egress_port      
 *                                  |                      +--------------------
 *            VLAN-30(broacast flow)|                      |          
 *                 -----------------+      Hurricane-4     |                    
 *                     ingress_port |                      |
 *                                  |                      |
 *                                  |                      |
 *                                  |                      |   cpu(collector)
 *                                  |                      +--------------------
 *                                  |                      |
 *                                  |                      |
 *                                  +----------------------+
 *
 * Summary:
 * ========
 * Cint example to demonstrate Broadscan 2.0 track and export
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup : Done in test_setup()
 *   =============================================
 *     a) In bstest_before_actual_config(), Dump all broadscan2.0 related config variable and 
 *        start ftrmon thread to monitor ipfix packet with "decode" option enabled. 
 *
 *     b) In bstest_traffic_path_setup(), select two ports to setup traffic path .
 *
 *
 *   2) Step2 - Configuration : Done in bs2_track_export_setup()
 *   =========================================================
 *     a) In bs2_track_export_setup(), configure Broadscan 2.0 track and export feature.
 *           Key: [Src/DstIpv4, Src/DstL4Port, IpProtocol], Data:[(IFT) - PktCnt, L4Srcport CheckEvent1(tsSrcLegacy)]
 *           Check: [Primary - Ttl,Min(1),Max(200),Range, TS_CHECK_EVENT_1], IFT Trigger
 *           CheckAction: (None, CounterIncr), Export: (Threshold-{1-pkt traffic_pkt_num}, GreaterEqual)
 *           Collector Port-CPU directly, ExportTrigger: Triggered by IFT Check on sending 1 packet
 *
 *           Resource Taken - PktCnt    (1 IFT-ALU32)
 *                            L4Srcport (1 IFT-LOAD16)
 *                            Check     (1 IFT-ALU32)
 *                            Timestamp (1 IFT-TS)
 *
 *   3) Step3 - Verification : Done in verify()
 *   ===========================================
 *     a) In bstest_tx_traffic(), send one below Input packet(TCP) and this packet 
 *        will be broadcast in the vlan.
 *
 *     b) Wait for ipfix export packet to reach ftrmon task and being printed.  
 *
 *     c) Expected Result:
 *     ===================
 *       We can see the ipfix packet will be printed by ftrmon task with all the export information listed and decoded.  
 */

/*
Input Packet :
======
*********************************************************************************************************
 * Packet Used for Testing
 * PktType:         IPv4 TCP
 * PktSize:         88B
 * DstMac:          0x22:33:44:55:66:77
 * SrcMac:          0x10:20:30:40:50:60
 * OuterVlan:       0x1E
 * InnerVlan:       0x2E
 * SrcIp:           0x0a.0a.02.02
 * DstIp:           0x0a.0a.02.03
 * TCP SrcPort:     0x3344
 * TCP DstPort:     0x5566
 * TCP Window Size: 0x11F5
   22 33 44 55 66 77 10 20 30 40 50 60 81 00 00 1E
   81 00 00 2E 08 00 45 08 00 38 00 00 40 00 40 06
   49 98 0A 0A 02 02 0A 0A 02 03 33 44 55 66 00 A1
   A2 A3 00 B1 B2 B3 50 02 11 F5 F8 C3 00 00 F5 F5
   F5 F5 F5 F5 F5 F5 F5 F5 F5 F5 F5 F5 F5 F5 F5 F5
   F5 F5 F5 F5 F5 F5 F5 F5
*********************************************************************************************************

Received Ipfix Packet:
=======
[bcmFtRMon.0]00000000   00 00 00 00 00 c0 00 00
[bcmFtRMon.0]00000008   00 00 00 f0 81 00 06 00
[bcmFtRMon.0]00000010   08 00 45 84 00 ac 00 00
[bcmFtRMon.0]00000018   00 00 09 11 95 85 0a 82
[bcmFtRMon.0]00000020   56 2a 0a 82 b0 0a 1f 91
[bcmFtRMon.0]00000028   08 07 00 98 91 47 00 0a
[bcmFtRMon.0]00000030   00 90 01 02 03 04 00 00
[bcmFtRMon.0]00000038   00 02 00 00 00 01 80 01
[bcmFtRMon.0]00000040   00 80 01 30 00 00 00 20
[bcmFtRMon.0]00000048   82 03 04 11 22 33 44 00
[bcmFtRMon.0]00000050   00 00 00 00 00 00 00 00
[bcmFtRMon.0]00000058   00 00 00 00 00 00 00 00
[bcmFtRMon.0]00000060   00 00 00 00 00 00 00 00
[bcmFtRMon.0]00000068   00 00 00 00 00 00 00 00
[bcmFtRMon.0]00000070   55 66 33 44 0a 0a 02 03
[bcmFtRMon.0]00000078   0a 0a 02 02 06 01 00 0b
[bcmFtRMon.0]00000080   00 2c 56 7e 87 29 00 00
[bcmFtRMon.0]00000088   00 01 00 00 00 01 33 44
[bcmFtRMon.0]00000090   00 00 00 00 00 00 00 00
[bcmFtRMon.0]00000098   00 00 00 00 00 00 00 00
[bcmFtRMon.0]000000a0   00 00 00 00 00 00 00 00
[bcmFtRMon.0]000000a8   00 00 00 00 00 00 00 00
[bcmFtRMon.0]000000b0   00 00 00 00 00 00 00 00
[bcmFtRMon.0]000000b8   00 00 00 00 00 00 00 00
[bcmFtRMon.0]000000c0   00 00 ee ee ee ee ee ee

*/


cint_reset();
/* Global variables */
int unit = 0;
/* Set this to 1, to run CMIC counter in device */
int enable_cmic_counter = 0;

/* Set this to 1, if running on bcmsim. On device set it to 0 */
int bcmsim_run = 0;
/* Set this to 1 to skip cleanup after test */
int skip_cleanup = 1;

const uint8 ipv4_tcp_sync_pkt1[] = {
0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x81, 0x00, 0x00, 0x1E,
0x81, 0x00, 0x00, 0x2E, 0x08, 0x00, 0x45, 0x08, 0x00, 0x38, 0x00, 0x00, 0x40, 0x00, 0x40, 0x06,
0x49, 0x98, 0x0A, 0x0A, 0x02, 0x02, 0x0A, 0x0A, 0x02, 0x03, 0x33, 0x44, 0x55, 0x66, 0x00, 0xA1,
0xA2, 0xA3, 0x00, 0xB1, 0xB2, 0xB3, 0x50, 0x02, 0x11, 0xF5, 0xF8, 0xC3, 0x00, 0x00, 0xF5, 0xF5,
0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5,
0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5
};

/* Packet Params below as per packet sent above */
/* Packet Vlan */
int pkt_ingress_vlan = 30;
/* Packet Vlan Formats - Untagged 0x0, OuterTagged 0x1, InnerTagged 0x2, Double Tagged 0x3 */
uint8 pkt_vlan_format = 0x3;
/* Packet Source IPv4 */
uint32 pkt_src_ipv4 = 0x0a0a0202;
/* Packet Destination IPv4 */
uint32 pkt_dst_ipv4 = 0x0a0a0203;
/* Packet L4 Source Port */
uint16 pkt_l4_src_port = 0x3344;
/* Packet L4 Destination Port */
uint16 pkt_l4_dst_port = 0x5566;
/* Packet IP Protocol - TCP - 6, UDP - 17 */
uint8 pkt_protocol = 6;
/* Packet TTL */
uint8 pkt_ttl = 0x40;
/* Packet TCP Window Size */
uint16 pkt_tcp_window_size = 0x11F5;

/* Helper API - Retreive export element and its data size from provided tracking param */
void fte_export_element_maps(bcm_flowtracker_tracking_param_type_t tparam,
                             bcm_flowtracker_export_element_type_t *exp_ele,
                             uint32 *data_size)
{
    switch(tparam) {
        /* 0-4 */
        case bcmFlowtrackerTrackingParamTypeSrcIPv4:
            *exp_ele = bcmFlowtrackerExportElementTypeSrcIPv4;
            *data_size = 4;
            break;
        case bcmFlowtrackerTrackingParamTypeDstIPv4:
            *exp_ele = bcmFlowtrackerExportElementTypeDstIPv4;
            *data_size = 4;
            break;
        case bcmFlowtrackerTrackingParamTypeSrcIPv6:
            *exp_ele = bcmFlowtrackerExportElementTypeSrcIPv6;
            *data_size = 16;
            break;
        case bcmFlowtrackerTrackingParamTypeDstIPv6:
            *exp_ele = bcmFlowtrackerExportElementTypeDstIPv6;
            *data_size = 16;
            break;
            /* 5-9 */
        case bcmFlowtrackerTrackingParamTypeL4SrcPort:
            *exp_ele = bcmFlowtrackerExportElementTypeL4SrcPort;
            *data_size = 2;
            break;
        case bcmFlowtrackerTrackingParamTypeL4DstPort:
            *exp_ele = bcmFlowtrackerExportElementTypeL4DstPort;
            *data_size = 2;
            break;
        case bcmFlowtrackerTrackingParamTypeIPProtocol:
            *exp_ele = bcmFlowtrackerExportElementTypeIPProtocol;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypePktCount:
            *exp_ele = bcmFlowtrackerExportElementTypePktCount;
            *data_size = 4;
            break;
        case bcmFlowtrackerTrackingParamTypeByteCount:
            *exp_ele = bcmFlowtrackerExportElementTypeByteCount;
            *data_size = 4;
            break;
            /* 10-14 */
        case bcmFlowtrackerTrackingParamTypeVRF:
            *exp_ele = bcmFlowtrackerExportElementTypeVRF;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeTTL:
            *exp_ele = bcmFlowtrackerExportElementTypeTTL;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeIPLength:
            *exp_ele = bcmFlowtrackerExportElementTypeIPLength;
            *data_size = 2;
            break;
        case bcmFlowtrackerTrackingParamTypeTcpWindowSize:
            *exp_ele = bcmFlowtrackerExportElementTypeTcpWindowSize;
            *data_size = 2;
            break;
        case bcmFlowtrackerTrackingParamTypeDosAttack:
            *exp_ele = bcmFlowtrackerExportElementTypeDosAttack;
            *data_size = 4;
            break;
            /* 15-19 */
        case bcmFlowtrackerTrackingParamTypeVxlanNetworkId:
            *exp_ele = bcmFlowtrackerExportElementTypeVxlanNetworkId;
            *data_size = 3;
            break;
        case bcmFlowtrackerTrackingParamTypeNextHeader:
            *exp_ele = bcmFlowtrackerExportElementTypeNextHeader;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeHopLimit:
            *exp_ele = bcmFlowtrackerExportElementTypeHopLimit;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerSrcIPv4:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerSrcIPv4;
            *data_size = 4;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerDstIPv4:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerDstIPv4;
            *data_size = 4;
            break;
            /* 20-24 */
        case bcmFlowtrackerTrackingParamTypeInnerSrcIPv6:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerSrcIPv6;
            *data_size = 16;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerDstIPv6:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerDstIPv6;
            *data_size = 16;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerIPProtocol:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerIPProtocol;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerTTL:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerTTL;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerNextHeader:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerNextHeader;
            *data_size = 1;
            break;
            /* 25-29 */
        case bcmFlowtrackerTrackingParamTypeInnerHopLimit:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerHopLimit;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerL4SrcPort:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerL4SrcPort;
            *data_size = 2;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerL4DstPort:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerL4DstPort;
            *data_size = 2;
            break;
        case bcmFlowtrackerTrackingParamTypeTcpFlags:
            *exp_ele = bcmFlowtrackerExportElementTypeTcpFlags;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeOuterVlanTag:
            *exp_ele = bcmFlowtrackerExportElementTypeOuterVlanTag;
            *data_size = 2;
            break;
            /* 30-34 */
        case bcmFlowtrackerTrackingParamTypeIP6Length:
            *exp_ele = bcmFlowtrackerExportElementTypeIP6Length;
            *data_size = 2;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerIPLength:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerIPLength;
            *data_size = 2;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerIP6Length:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerIP6Length;
            *data_size = 2;
            break;
        case bcmFlowtrackerTrackingParamTypeTimestampNewLearn:
            *exp_ele = bcmFlowtrackerExportElementTypeTimestampNewLearn;
            *data_size = 6;
            break;
        case bcmFlowtrackerTrackingParamTypeTimestampFlowStart:
            *exp_ele = bcmFlowtrackerExportElementTypeTimestampFlowStart;
            *data_size = 6;
            break;
            /* 35-39 */
        case bcmFlowtrackerTrackingParamTypeTimestampFlowEnd:
            *exp_ele = bcmFlowtrackerExportElementTypeTimestampFlowEnd;
            *data_size = 6;
            break;
        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1:
            *exp_ele = bcmFlowtrackerExportElementTypeTimestampCheckEvent1;
            *data_size = 6;
            break;
        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent2:
            *exp_ele = bcmFlowtrackerExportElementTypeTimestampCheckEvent2;
            *data_size = 6;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerDosAttack:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerDosAttack;
            *data_size = 4;
            break;
        case bcmFlowtrackerTrackingParamTypeTunnelClass:
            *exp_ele = bcmFlowtrackerExportElementTypeTunnelClass;
            *data_size = 3;
            break;
            /* 40-44 */
        case bcmFlowtrackerTrackingParamTypeFlowtrackerCheck:
            *exp_ele = bcmFlowtrackerExportElementTypeFlowtrackerCheck;
            *data_size = 4;
            break;
        case bcmFlowtrackerTrackingParamTypeCustom:
            *exp_ele = bcmFlowtrackerExportElementTypeCustom;
            *data_size = 4;
            break;
        case bcmFlowtrackerTrackingParamTypeIngPort:
            *exp_ele = bcmFlowtrackerExportElementTypeIngPort;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeChipDelay:
            *exp_ele = bcmFlowtrackerExportElementTypeChipDelay;
            *data_size = 4;
            break;
        case bcmFlowtrackerTrackingParamTypeE2EDelay:
            *exp_ele = bcmFlowtrackerExportElementTypeE2EDelay;
            *data_size = 4;
            break;
            /* 45-49 */
        case bcmFlowtrackerTrackingParamTypeIPATDelay:
            *exp_ele = bcmFlowtrackerExportElementTypeIPATDelay;
            *data_size = 4;
            break;
        case bcmFlowtrackerTrackingParamTypeIPDTDelay:
            *exp_ele = bcmFlowtrackerExportElementTypeIPDTDelay;
            *data_size = 4;
            break;
        case bcmFlowtrackerTrackingParamTypeIngPortPGMinCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeIngPortPGMinCongestionLevel;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeIngPortPGSharedCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeIngPortPGSharedCongestionLevel;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeIngPortPoolMinCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeIngPortPoolMinCongestionLevel;
            *data_size = 1;
            break;
            /* 50-54 */
        case bcmFlowtrackerTrackingParamTypeIngPortPoolSharedCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeIngPortPoolSharedCongestionLevel;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeIngPoolSharedCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeIngPoolSharedCongestionLevel;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeIngPortPGHeadroomCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeIngPortPGHeadroomCongestionLevel;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeIngPortPoolHeadroomCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeIngPortPoolHeadroomCongestionLevel;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeIngGlobalHeadroomCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeIngGlobalHeadroomCongestionLevel;
            *data_size = 1;
            break;
            /* 55-59 */
        case bcmFlowtrackerTrackingParamTypeEgrPoolSharedCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeEgrPoolSharedCongestionLevel;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeEgrQueueMinCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeEgrQueueMinCongestionLevel;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeEgrQueueSharedCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeEgrQueueSharedCongestionLevel;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeEgrQueueGroupMinCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeEgrQueueGroupMinCongestionLevel;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeEgrQueueGroupSharedCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeEgrQueueGroupSharedCongestionLevel;
            *data_size = 1;
            break;
            /* 60-64 */
        case bcmFlowtrackerTrackingParamTypeEgrPortPoolSharedCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeEgrPortPoolSharedCongestionLevel;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeEgrRQEPoolSharedCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeEgrRQEPoolSharedCongestionLevel;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeCFAPCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeCFAPCongestionLevel;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeMMUCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeMMUCongestionLevel;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeMMUQueueId:
            *exp_ele = bcmFlowtrackerExportElementTypeMMUQueueId;
            *data_size = 2;
            break;
            /* 65-69 */
        case bcmFlowtrackerTrackingParamTypeECMPGroupIdLevel1:
            *exp_ele = bcmFlowtrackerExportElementTypeECMPGroupIdLevel1;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeECMPGroupIdLevel2:
            *exp_ele = bcmFlowtrackerExportElementTypeECMPGroupIdLevel2;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeTrunkMemberId:
            *exp_ele = bcmFlowtrackerExportElementTypeTrunkMemberId;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeTrunkGroupId:
            *exp_ele = bcmFlowtrackerExportElementTypeTrunkGroupId;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeIngDropReasonGroupIdVector:
            *exp_ele = bcmFlowtrackerExportElementTypeIngDropReasonGroupIdVector;
            *data_size = 2;
            break;
            /* 70-74 */
        case bcmFlowtrackerTrackingParamTypeNextHopB:
            *exp_ele = bcmFlowtrackerExportElementTypeNextHopB;
            *data_size = 2;
            break;
        case bcmFlowtrackerTrackingParamTypeNextHopA:
            *exp_ele = bcmFlowtrackerExportElementTypeNextHopA;
            *data_size = 2;
            break;
        case bcmFlowtrackerTrackingParamTypeIPPayloadLength:
            *exp_ele = bcmFlowtrackerExportElementTypeIPPayloadLength;
            *data_size = 2;
            break;
        case bcmFlowtrackerTrackingParamTypeQosAttr:
            *exp_ele = bcmFlowtrackerExportElementTypeQosAttr;
            *data_size = 2;
            break;
        case bcmFlowtrackerTrackingParamTypeEgrPort:
            *exp_ele = bcmFlowtrackerExportElementTypeEgrPort;
            *data_size = 1;
            break;
            /* 75-79 */
        case bcmFlowtrackerTrackingParamTypeMMUCos:
            *exp_ele = bcmFlowtrackerExportElementTypeMMUCos;
            *data_size = 2;
            break;
        case bcmFlowtrackerTrackingParamTypeEgrDropReasonGroupIdVector:
            *exp_ele = bcmFlowtrackerExportElementTypeEgrDropReasonGroupIdVector;
            *data_size = 2;
            break;
        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent3:
            *exp_ele = bcmFlowtrackerExportElementTypeTimestampCheckEvent3;
            *data_size = 6;
            break;
        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent4:
            *exp_ele = bcmFlowtrackerExportElementTypeTimestampCheckEvent4;
            *data_size = 6;
            break;
        case bcmFlowtrackerTrackingParamTypeIngressDelay:
            *exp_ele = bcmFlowtrackerExportElementTypeIngressDelay;
            *data_size = 4;
            break;
            /* 80-84 */
        case bcmFlowtrackerTrackingParamTypeEgressDelay:
            *exp_ele = bcmFlowtrackerExportElementTypeEgressDelay;
            *data_size = 4;
            break;
        case bcmFlowtrackerTrackingParamTypeMMUQueueGroupId:
            *exp_ele = bcmFlowtrackerExportElementTypeMMUQueueGroupId;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeMMUPacketTos:
            *exp_ele = bcmFlowtrackerExportElementTypeMMUPacketTos;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeEgrPacketTos:
            *exp_ele = bcmFlowtrackerExportElementTypeEgrPacketTos;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeIngPacketTos:
            *exp_ele = bcmFlowtrackerExportElementTypeIngPacketTos;
            *data_size = 1;
            break;
            /* 85-89 */
        case bcmFlowtrackerTrackingParamTypeL2ForwardingField:
            *exp_ele = bcmFlowtrackerExportElementTypeL2ForwardingField;
            *data_size = 2;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerTcpFlags:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerTcpFlags;
            *data_size = 1;
            break;

        default:
            *data_size = 0;
    }
}

/* Helper API - convert input tracking param into equivalent export element */
int fte_tracking_param_xlate(int unit,
                             bcm_flowtracker_tracking_param_type_t tparam,
                             uint32 *size,
                             bcm_flowtracker_export_element_type_t *exp_elem)
{
    fte_export_element_maps(tparam, exp_elem, size);

    if (*size == 0) {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/* Helper API - Return string for provided export element */
char* ft_convert_export_element_to_string(bcm_flowtracker_export_element_type_t export_element)
{
    switch(export_element) {
        /* 0-4 */
        case bcmFlowtrackerExportElementTypeSrcIPv4:
            return "SrcIPv4";
        case bcmFlowtrackerExportElementTypeDstIPv4:
            return "DstIPv4";
        case bcmFlowtrackerExportElementTypeSrcIPv6:
            return "SrcIPv6";
        case bcmFlowtrackerExportElementTypeDstIPv6:
            return "DstIPv6";
        case bcmFlowtrackerExportElementTypeL4SrcPort:
            return "L4SrcPort";
            /* 5-9 */
        case bcmFlowtrackerExportElementTypeL4DstPort:
            return "L4DstPort";
        case bcmFlowtrackerExportElementTypeIPProtocol:
            return "IPProtocol";
        case bcmFlowtrackerExportElementTypePktCount:
            return "PktCount";
        case bcmFlowtrackerExportElementTypeByteCount:
            return "ByteCount";
        case bcmFlowtrackerExportElementTypePktDeltaCount:
            return "PktDeltaCount";
            /* 10-14 */
        case bcmFlowtrackerExportElementTypeByteDeltaCount:
            return "ByteDeltaCount";
        case bcmFlowtrackerExportElementTypeVRF:
            return "VRF";
        case bcmFlowtrackerExportElementTypeObservationTimeMsecs:
            return "ObservationTimeMsecs";
        case bcmFlowtrackerExportElementTypeFlowStartTimeMsecs:
            return "FlowStartTimeMsecs";
        case bcmFlowtrackerExportElementTypeFlowtrackerGroup:
            return "FlowtrackerGroup";
            /* 15-19 */
        case bcmFlowtrackerExportElementTypeReserved:
            return "Reserved";
        case bcmFlowtrackerExportElementTypeTTL:
            return "TTL";
        case bcmFlowtrackerExportElementTypeIPLength:
            return "IPLength";
        case bcmFlowtrackerExportElementTypeTcpWindowSize:
            return "TcpWindowSize";
        case bcmFlowtrackerExportElementTypeDosAttack:
            return "DosAttack";
            /* 20-24 */
        case bcmFlowtrackerExportElementTypeVxlanNetworkId:
            return "VxlanNetworkId";
        case bcmFlowtrackerExportElementTypeNextHeader:
            return "NextHeader";
        case bcmFlowtrackerExportElementTypeHopLimit:
            return "HopLimit";
        case bcmFlowtrackerExportElementTypeInnerSrcIPv4:
            return "InnerSrcIPv4";
        case bcmFlowtrackerExportElementTypeInnerDstIPv4:
            return "InnerDstIPv4";
            /* 25-29 */
        case bcmFlowtrackerExportElementTypeInnerSrcIPv6:
            return "InnerSrcIPv6";
        case bcmFlowtrackerExportElementTypeInnerDstIPv6:
            return "InnerDstIPv6";
        case bcmFlowtrackerExportElementTypeInnerIPProtocol:
            return "InnerIPProtocol";
        case bcmFlowtrackerExportElementTypeInnerTTL:
            return "InnerTTL";
        case bcmFlowtrackerExportElementTypeInnerNextHeader:
            return "InnerNextHeader";
            /* 30-34 */
        case bcmFlowtrackerExportElementTypeInnerHopLimit:
            return "InnerHopLimit";
        case bcmFlowtrackerExportElementTypeInnerL4SrcPort:
            return "InnerL4SrcPort";
        case bcmFlowtrackerExportElementTypeInnerL4DstPort:
            return "InnerL4DstPort";
        case bcmFlowtrackerExportElementTypeExportReasons:
            return "ExportReasons";
        case bcmFlowtrackerExportElementTypeExportFlags:
            return "ExportFlags";
            /* 35-39 */
        case bcmFlowtrackerExportElementTypeTcpFlags:
            return "TcpFlags";
        case bcmFlowtrackerExportElementTypeOuterVlanTag:
            return "OuterVlanTag";
        case bcmFlowtrackerExportElementTypeIP6Length:
            return "IP6Length";
        case bcmFlowtrackerExportElementTypeInnerIPLength:
            return "InnerIPLength";
        case bcmFlowtrackerExportElementTypeInnerIP6Length:
            return "InnerIP6Length";
            /* 40-44 */
        case bcmFlowtrackerExportElementTypeTimestampNewLearn:
            return "TimestampNewLearn";
        case bcmFlowtrackerExportElementTypeTimestampFlowStart:
            return "FlowStart";
        case bcmFlowtrackerExportElementTypeTimestampFlowEnd:
            return "FlowEnd";
        case bcmFlowtrackerExportElementTypeTimestampCheckEvent1:
            return "Timestamp Check Event1";
        case bcmFlowtrackerExportElementTypeTimestampCheckEvent2:
            return "Timestamp Check Event2";
            /* 45-49 */
        case bcmFlowtrackerExportElementTypeInnerDosAttack:
            return "InnerDosAttack";
        case bcmFlowtrackerExportElementTypeTunnelClass:
            return "TunnelClass";
        case bcmFlowtrackerExportElementTypeFlowtrackerCheck:
            return "FlowtrackerCheck";
        case bcmFlowtrackerExportElementTypeCustom:
            return "Custom";
        case bcmFlowtrackerExportElementTypeFlowViewID:
            return "FlowViewID";
            /* 50-54 */
        case bcmFlowtrackerExportElementTypeFlowViewSampleCount:
            return "FlowViewSampleCount";
        case bcmFlowtrackerExportElementTypeFlowCount:
            return "FlowCount";
        case bcmFlowtrackerExportElementTypeDropPktCount:
            return "DropPktCount";
        case bcmFlowtrackerExportElementTypeDropByteCount:
            return "DropByteCount";
        case bcmFlowtrackerExportElementTypeEgrPort:
            return "EgrPort";
            /* 55-59 */
        case bcmFlowtrackerExportElementTypeQueueID:
            return "QueueID";
        case bcmFlowtrackerExportElementTypeQueueBufferUsage:
            return "QueueBufferUsage";
        case bcmFlowtrackerExportElementTypeChipDelay:
            return "ChipDelay";
        case bcmFlowtrackerExportElementTypeE2EDelay:
            return "E2EDelay";
        case bcmFlowtrackerExportElementTypeIPATDelay:
            return "IPATDelay";
            /* 60-64 */
        case bcmFlowtrackerExportElementTypeIPDTDelay:
            return "IPDTDelay";
        case bcmFlowtrackerExportElementTypeIngPortPGMinCongestionLevel:
            return "IngPortPGMinCongestionLevel";
        case bcmFlowtrackerExportElementTypeIngPortPGSharedCongestionLevel:
            return "IngPortPGSharedCongestionLevel";
        case bcmFlowtrackerExportElementTypeIngPortPoolMinCongestionLevel:
            return "IngPortPoolMinCongestionLevel";
        case bcmFlowtrackerExportElementTypeIngPortPoolSharedCongestionLevel:
            return "IngPortPoolSharedCongestionLevel";
            /* 65-69 */
        case bcmFlowtrackerExportElementTypeIngPoolSharedCongestionLevel:
            return "IngPoolSharedCongestionLevel";
        case bcmFlowtrackerExportElementTypeIngPortPGHeadroomCongestionLevel:
            return "IngPortPGHeadroomCongestionLevel";
        case bcmFlowtrackerExportElementTypeIngPortPoolHeadroomCongestionLevel:
            return "IngPortPoolHeadroomCongestionLevel";
        case bcmFlowtrackerExportElementTypeIngGlobalHeadroomCongestionLevel:
            return "IngGlobalHeadroomCongestionLevel";
        case bcmFlowtrackerExportElementTypeEgrPoolSharedCongestionLevel:
            return "EgrPoolSharedCongestionLevel";
            /* 70-74 */
        case bcmFlowtrackerExportElementTypeEgrQueueMinCongestionLevel:
            return "EgrQueueMinCongestionLevel";
        case bcmFlowtrackerExportElementTypeEgrQueueSharedCongestionLevel:
            return "EgrQueueSharedCongestionLevel";
        case bcmFlowtrackerExportElementTypeEgrQueueGroupMinCongestionLevel:
            return "EgrQueueGroupMinCongestionLevel";
        case bcmFlowtrackerExportElementTypeEgrQueueGroupSharedCongestionLevel:
            return "EgrQueueGroupSharedCongestionLevel";
        case bcmFlowtrackerExportElementTypeEgrPortPoolSharedCongestionLevel:
            return "EgrPortPoolSharedCongestionLevel";
            /* 75-79 */
        case bcmFlowtrackerExportElementTypeEgrRQEPoolSharedCongestionLevel:
            return "EgrRQEPoolSharedCongestionLevel";
        case bcmFlowtrackerExportElementTypeCFAPCongestionLevel:
            return "CFAPCongestionLevel";
        case bcmFlowtrackerExportElementTypeMMUCongestionLevel:
            return "MMUCongestionLevel";
        case bcmFlowtrackerExportElementTypeMMUQueueId:
            return "MMU Queue Id";
        case bcmFlowtrackerExportElementTypeECMPGroupIdLevel1:
            return "ECMPGroupIdLevel1";
            /* 80-84 */
        case bcmFlowtrackerExportElementTypeECMPGroupIdLevel2:
            return "ECMPGroupIdLevel2";
        case bcmFlowtrackerExportElementTypeTrunkMemberId:
            return "TrunkMemberId";
        case bcmFlowtrackerExportElementTypeTrunkGroupId:
            return "TrunkGroupId";
        case bcmFlowtrackerExportElementTypeIngPort:
            return "IngPort";
        case bcmFlowtrackerExportElementTypeIngDropReasonGroupIdVector:
            return "IngDropReasonGroupIdVector";
            /* 85-89 */
        case bcmFlowtrackerExportElementTypeNextHopB:
            return "NextHopB";
        case bcmFlowtrackerExportElementTypeNextHopA:
            return "NextHopA";
        case bcmFlowtrackerExportElementTypeIPPayloadLength:
            return "IPPayloadLength";
        case bcmFlowtrackerExportElementTypeQosAttr:
            return "QosAttr";
        case bcmFlowtrackerExportElementTypeMMUCos:
            return "MMUCos";
            /* 90-94 */
        case bcmFlowtrackerExportElementTypeEgrDropReasonGroupIdVector:
            return "EgrDropReasonGroupIdVector";
        case bcmFlowtrackerExportElementTypeTimestampCheckEvent3:
            return "TimestampCheckEvent3";
        case bcmFlowtrackerExportElementTypeTimestampCheckEvent4:
            return "TimestampCheckEvent4";
        case bcmFlowtrackerExportElementTypeQueueRxPktCount:
            return "QueueRxPktCount";
        case bcmFlowtrackerExportElementTypeFlowIdentifier:
            return "FlowIdentifier";
            /* 95-99 */
        case bcmFlowtrackerExportElementTypeMMUQueueGroupId:
            return "MmuGroupId";
        case bcmFlowtrackerExportElementTypeMMUPacketTos:
            return "MmuPacketTos";
        case bcmFlowtrackerExportElementTypeEgrPacketTos:
            return "EgrPacketTos";
        case bcmFlowtrackerExportElementTypeQueueRxByteCount:
            return "QueueRxByteCount";
        case bcmFlowtrackerExportElementTypeIPVersion:
            return "IPVersion";
            /* 100-101 */
        case bcmFlowtrackerExportElementTypeBufferUsage:
            return "BufferUsage";
        case bcmFlowtrackerExportElementTypeIngPacketTos:
            return "IngPacketTos";
        case bcmFlowtrackerExportElementTypeL2ForwardingField:
            return "L2ForwardingField";
        case bcmFlowtrackerExportElementTypeInnerTcpFlags:
            return "InnerTcpFlags";

        default:
            return "Unknown";
    }
}

/* Helper API - Send traffic for provided number of packets */
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

/* Helper API - Send packet of provided length from given port */
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

/*
 * Helper API - Set time in system.
 * source stage - 0:Legacy, 1:NTP, 2:PTP, 3:CMIC 4: MAC
 */
int cmic_time_intfid = -1;
int
ft_time_set(int unit, uint8 source_stage, uint32 sec_upper, uint32 sec_lower, uint32 nano_sec, uint32 nano_sec_adjust_upper, uint32 nano_sec_adjust_lower)
{
    int rv = BCM_E_NONE;
    bcm_time_tod_t tod;
    unsigned int stages;
    bcm_time_format_t format = 0;
    bcm_time_interface_t ts_cmic_intf;
    const char *s_stage[] = {
        "Legacy",
        "NTP",
        "PTP",
        "CMIC",
    };
    const char *s_time_format[] = {
        "PTP",
        "NTP",
    };
    char str[200];

    /*
     * BSC_TS_UTC_CONVERSION is                    - 48b sec, 32b nanosec & 48b adjustmenet in nanosec
     * NTP time (NS_NTP_TOD_VALUE_0/1r) is 64b     - 32b sec & 32b nanosec
     * PTP time (NS_PTP_TOD_A_VALUE_0/1/2r is 80b  - 48b sec & 32b nsec
     * CMIC time counter (NS_TIMESYNC_TS0_TIMESTAMP_UPPER_STATUS/NS_TIMESYNC_TS0_TIMESTAMP_LOWER_STATUS)
     *                   (NS_TIMESYNC_TS1_TIMESTAMP_UPPER_STATUS/NS_TIMESYNC_TS1_TIMESTAMP_UPPER_STATUS)
     */
    if (source_stage == 0) {
        /* Legacy time can be be set in sim & svk both */
        stages = BCM_TIME_STAGE_INGRESS_FLOWTRACKER;
        format = bcmTimeFormatPTP;
    } else if ((source_stage == 1) && (bcmsim)) {
        /* NTP time cannot be set in svk, can be
         * set as dummy register in bcmsim */
        stages = BCM_TIME_STAGE_LIVE_WIRE_NTP;
        format = bcmTimeFormatNTP;
    } else if ((source_stage == 2) && (bcmsim)) {
        /* PTP time cannot be set in svk, can be
         * set as dummy register in bcmsim */
        stages = BCM_TIME_STAGE_LIVE_WIRE_PTP;
        format = bcmTimeFormatPTP;
    } else if (source_stage == 3) {
        /* CMIC timer can be started in sim & svk both */
    } else if ((source_stage == 4) && (bcmsim)) {
        /* MAC time cannot be set in svk, can be
         * set as dummy register in bcmsim. It is
         * a internal time captured by pipeline
         * at ingress, used to calculated chip delay */
    } else {
        return BCM_E_PARAM;
    }

    if (source_stage == 3) {

        /* CMIC is a free running counter */
        if (enable_cmic_counter) {

            if (bcmsim_run) {

                sprintf(str, "s NS_TIMESYNC_INPUT_TIME_FIFO1_TS_0 TS0_L=%d", nano_sec);
                bshell(unit, str);
                sprintf(str, "s NS_TIMESYNC_INPUT_TIME_FIFO1_TS_1 TS0_U=%d", sec_lower);
                bshell(unit, str);

                printf("[CINT] - SIM CMIC time <%6s> format <%s> set: sec %08x, nanosec 0x%08x\n",
                        s_stage[source_stage], s_time_format[format], sec_lower, nano_sec);

            } else {

                bcm_time_interface_t_init(&ts_cmic_intf);
                ts_cmic_intf.id = 0;
                ts_cmic_intf.flags = BCM_TIME_ENABLE | BCM_TIME_WITH_ID;
                rv = bcm_time_interface_add(unit, &ts_cmic_intf);
                if (BCM_FAILURE(rv)) {
                    printf("[CINT] bcm_time_interface_add => rv %d(%s)\n", rv, bcm_errmsg(rv));
                    return rv;
                }
                cmic_time_intfid = 0;
                printf("[CINT] - CMIC time interface added, cmic counter has started !\n");
            }
        } else {

            printf("[CINT] - CMIC time interface is not enabled !\n");
        }

    } else {

        if ((bcmsim_run) &&
           ((source_stage == 1) || (source_stage == 2) || (source_stage == 4))) {

            /* in BCMSIM run, NTP/PTP/MAC time has to be set directly in reg not using tod api */
            if (source_stage == 1) {
                sprintf(str, "s NS_NTP_TOD_VALUE_0 VAL=%d", nano_sec);
                bshell(unit, str);
                sprintf(str, "s NS_NTP_TOD_VALUE_1 VAL=%d", sec_lower);
                bshell(unit, str);
            } else if (source_stage == 2) {
                sprintf(str, "s NS_PTP_TOD_A_VALUE_0 VAL=%d", nano_sec);
                bshell(unit, str);
                sprintf(str, "s NS_PTP_TOD_A_VALUE_1 VAL=%d", sec_lower);
                bshell(unit, str);
                sprintf(str, "s NS_PTP_TOD_A_VALUE_2 VAL=%d", sec_upper);
                bshell(unit, str);
            } else if (source_stage == 4) {
                sprintf(str, "s NS_TIMESYNC_GPIO_0_PHASE_ADJUST_LOWER PHASE_ADJUST=%d", nano_sec);
                bshell(unit, str);
                sprintf(str, "s NS_TIMESYNC_INPUT_TIME_FIFO1_TS_1 TS0_U=%d", sec_lower);
                bshell(unit, str);
            } else {
                return BCM_E_PARAM;
            }

        } else {

            /* in SVK, NTP/PTP time cannot be set as it is a read only register */
            /* Time Setting - Using bcm_time_tod_set */
            bcm_time_tod_t_init(&tod);
            tod.time_format = format;
            COMPILER_64_SET(tod.ts.seconds, sec_upper, sec_lower);
            tod.ts.nanoseconds = nano_sec;
            COMPILER_64_SET(tod.ts_adjustment_counter_ns, nano_sec_adjust_upper, nano_sec_adjust_lower);
            rv = bcm_time_tod_set(unit, stages, &tod);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] bcm_time_tod_set => rv %d(%s)\n", rv, bcm_errmsg(rv));
                return rv;
            }
        }

        if (bcmsim_run) {
            printf("[CINT] - System BCMSIM time <%6s> format <%s> set: sec 0x%08x-%08x, nanosec 0x%08x, adjust_nanosec 0x%08x-%08x\n",
                    s_stage[source_stage], s_time_format[format], sec_upper, sec_lower,
                    nano_sec, nano_sec_adjust_upper, nano_sec_adjust_lower);
        } else {
            printf("[CINT] - System SVK time <%6s> format <%s> set: sec 0x%08x-%08x, nanosec 0x%08x, adjust_nanosec 0x%08x-%08x\n",
                    s_stage[source_stage], s_time_format[format], sec_upper, sec_lower,
                    nano_sec, nano_sec_adjust_upper, nano_sec_adjust_lower);
        }
    }

    return BCM_E_NONE;
}

/*
 * Helper API - Get time in system.
 * source stage - 0:Legacy, 1:NTP, 2:PTP, 3:CMIC
 */
int
ft_time_get(int unit, uint8 source_stage, uint32 *sec_upper, uint32 *sec_lower, uint32 *nano_sec, uint32 *nano_sec_adjust_upper, uint32 *nano_sec_adjust_lower)
{
    int rv = BCM_E_NONE;
    bcm_time_tod_t tod;
    unsigned int stages = 0;
    bcm_time_ts_counter_t ts_cmic_counter;
    const char *s_stage[] = {
        "Legacy",
        "NTP",
        "PTP",
        "CMIC",
    };
    const char *s_time_format[] = {
        "PTP",
        "NTP",
    };

    if (source_stage == 0) {
        stages = BCM_TIME_STAGE_INGRESS_FLOWTRACKER;
    } else if (source_stage == 1) {
        stages = BCM_TIME_STAGE_LIVE_WIRE_NTP;
    } else if (source_stage == 2) {
        stages = BCM_TIME_STAGE_LIVE_WIRE_PTP;
    } else if (source_stage == 3) {
    } else {
        return BCM_E_PARAM;
    }

    if (source_stage == 3) {

        if (enable_cmic_counter) {
            bcm_time_ts_counter_t_init(&ts_cmic_counter);
            rv = bcm_time_ts_counter_get(unit, &ts_cmic_counter);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] bcm_time_ts_counter_get => rv %d(%s)\n", rv, bcm_errmsg(rv));
                return rv;
            }

            printf("[CINT] - CMIC time format <%s> get: en %d, ts_counter TS%d, counter 0x%08x-%08x\n",
                    s_time_format[ts_cmic_counter.time_format],
                    ts_cmic_counter.enable, ts_cmic_counter.ts_counter,
                    COMPILER_64_HI(ts_cmic_counter.ts_counter_ns),
                    COMPILER_64_LO(ts_cmic_counter.ts_counter_ns));
        } else {
            printf("[CINT] - CMIC time interface is not enabled !\n");
        }

    } else {

        bcm_time_tod_t_init(&tod);
        rv = bcm_time_tod_get(unit, stages, &tod);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] bcm_time_tod_get => rv %d(%s)\n", rv, bcm_errmsg(rv));
            return rv;
        }

        *sec_upper = COMPILER_64_HI(tod.ts.seconds);
        *sec_lower = COMPILER_64_LO(tod.ts.seconds);
        *nano_sec = tod.ts.nanoseconds;
        *nano_sec_adjust_upper = COMPILER_64_HI(tod.ts_adjustment_counter_ns);
        *nano_sec_adjust_lower = COMPILER_64_LO(tod.ts_adjustment_counter_ns);

        printf("[CINT] - System time <%6s> format <%s> get: sec 0x%08x-%08x, nanosec 0x%08x, adjust_nanosec 0x%08x-%08x\n",
                s_stage[source_stage], s_time_format[tod.time_format], *sec_upper,
                *sec_lower, *nano_sec, *nano_sec_adjust_upper, *nano_sec_adjust_lower);
    }

    return BCM_E_NONE;
}

/* Helper api - print current time in system */
int
ft_time_get_print(int unit, uint8 source_stage)
{
    int rv = BCM_E_NONE;
    uint32 secu = 0, secl = 0;
    uint32 nano_sec = 0;
    uint32 nano_secu =0, nano_secl = 0;

    if (source_stage != 0xFF) {

        rv = ft_time_get(unit, source_stage, &secu, &secl, &nano_sec, &nano_secu, &nano_secl);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] - Failed to retreive time for source stage %d - rv %d(%s)!\n",
                    source_stage, rv, bcm_errmsg(rv));
        }

    } else {

         source_stage = 0;
         rv = ft_time_get(unit, source_stage, &secu, &secl, &nano_sec, &nano_secu, &nano_secl);
         if (BCM_FAILURE(rv)) {
             printf("[CINT] - Failed to retreive time for source stage %d - rv %d(%s)!\n",
                     source_stage, rv, bcm_errmsg(rv));
         }
         source_stage = 1;
         rv = ft_time_get(unit, source_stage, &secu, &secl, &nano_sec, &nano_secu, &nano_secl);
         if (BCM_FAILURE(rv)) {
             printf("[CINT] - Failed to retreive time for source stage %d - rv %d(%s)!\n",
                     source_stage, rv, bcm_errmsg(rv));
         }
         source_stage = 2;
         rv = ft_time_get(unit, source_stage, &secu, &secl, &nano_sec, &nano_secu, &nano_secl);
         if (BCM_FAILURE(rv)) {
             printf("[CINT] - Failed to retreive time for source stage %d - rv %d(%s)!\n",
                     source_stage, rv, bcm_errmsg(rv));
         }
         if (enable_cmic_counter) {
             source_stage = 3;
             rv = ft_time_get(unit, source_stage, &secu, &secl, &nano_sec, &nano_secu, &nano_secl);
             if (BCM_FAILURE(rv)) {
                 printf("[CINT] - Failed to retreive time for source stage %d - rv %d(%s)!\n",
                         source_stage, rv, bcm_errmsg(rv));
             }
         }
    }

    return rv;
}

/* Step1 - Presel Entry Create */
bcm_field_presel_t ftfp_presel_id1 = -1;
int bcmft_presel_entry_create(int unit)
{
    int rv = BCM_E_NONE;
    bcm_field_entry_t ft_presel_eid1 = -1;;
    BCM_IF_ERROR_RETURN
        (bcm_field_presel_create(unit, &ftfp_presel_id1));
    ft_presel_eid1 = ftfp_presel_id1 | BCM_FIELD_QUALIFY_PRESEL;

    BCM_IF_ERROR_RETURN
        (bcm_field_qualify_Stage(unit, ft_presel_eid1, bcmFieldStageIngressFlowtracker));
    BCM_IF_ERROR_RETURN
        (bcm_field_qualify_L4Ports(unit, ft_presel_eid1, 1, 1));
    BCM_IF_ERROR_RETURN
        (bcm_field_qualify_IpType(unit, ft_presel_eid1, bcmFieldIpTypeIpv4NoOpts));
    BCM_IF_ERROR_RETURN
        (bcm_field_qualify_PktDstAddrType(unit, ft_presel_eid1, BCM_FIELD_PKT_DST_ADDR_TYPE_UCAST_MAC | BCM_FIELD_PKT_DST_ADDR_TYPE_UCAST_IP));

    printf("[CINT] Setup: BS Config Step1 - Created presel entries presel_eid1 0x%x.\n",ft_presel_eid1);
    return rv;
}

/* Step2 - Create FTFP Group */
int ftfp_group1 = -1;
int bcmft_ftfp_group_create(int unit)
{
    int rv = BCM_E_NONE;
    bcm_field_group_config_t ftfp_group_config1;
    bcm_field_group_config_t_init(&ftfp_group_config1);
    ftfp_group_config1.flags |= BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    BCM_FIELD_PRESEL_ADD(ftfp_group_config1.preselset, ftfp_presel_id1);
    BCM_FIELD_QSET_INIT(ftfp_group_config1.qset);
    BCM_FIELD_QSET_ADD(ftfp_group_config1.qset, bcmFieldQualifyStageIngressFlowtracker);
    BCM_FIELD_QSET_ADD(ftfp_group_config1.qset, bcmFieldQualifyIpProtocol);
    BCM_FIELD_QSET_ADD(ftfp_group_config1.qset, bcmFieldQualifyOuterVlanId);
    BCM_FIELD_QSET_ADD(ftfp_group_config1.qset, bcmFieldQualifyInnerVlanId);
    BCM_FIELD_QSET_ADD(ftfp_group_config1.qset, bcmFieldQualifyVlanFormat);
    BCM_FIELD_QSET_ADD(ftfp_group_config1.qset, bcmFieldQualifyInPort);
    ftfp_group_config1.mode = bcmFieldGroupModeAuto;
    BCM_IF_ERROR_RETURN
        (bcm_field_group_config_create(unit, &ftfp_group_config1));
    ftfp_group1 = ftfp_group_config1.group;

    printf("[CINT] Setup: BS Config Step2 - Created FTFP group 0x%x.\n",ftfp_group1);
    return rv;
}

/* Step3 - Create Collector Port, collector vlan & export profile */
bcm_vlan_t    fte_coll_vlan        = 0x600;
bcm_mac_t fte_coll_dst_mac         = {0x00, 0x00, 0x00, 0x00, 0x00, 0xc0};
int ft_collector_id1        = -1;
int ft_export_profile_id1   = -1;
int bcmft_collector_create(int unit) {

    int rv = BCM_E_NONE;
    bcm_l2_addr_t l2_addr;
    bcm_pbmp_t cpbmp, cupbmp;
    bcm_mac_t fte_coll_src_mac         = {0x00, 0x00, 0x00, 0x00, 0x00, 0xf0};
    uint16        fte_coll_tpid        = 0x8100;
    bcm_ip_t      fte_coll_src_ip      = 0x0a82562a;
    bcm_ip_t      fte_coll_dst_ip      = 0x0a82b00a;
    uint8         fte_coll_dscp        = 0xE1;
    uint8         fte_coll_ttl         = 9;
    bcm_l4_port_t fte_coll_l4_src_port = 0x1F91;
    bcm_l4_port_t fte_coll_l4_dst_port = 0x0807;
    uint16        fte_coll_mtu         = 1500;
    /* Adding CPU port as collector as want to send ipfix
     * export packet to CPU for ftrmon task to analyse */
    bcm_port_t fte_coll_port = 0;
    uint32 coptions = 0;
    bcm_collector_info_t collector_info;
    bcm_collector_export_profile_t profile_info;

    /* first set collector port and vlan */
    BCM_IF_ERROR_RETURN
        (bcm_vlan_create(unit, fte_coll_vlan));
    BCM_PBMP_CLEAR(cpbmp);
    BCM_PBMP_CLEAR(cupbmp);
    BCM_PBMP_PORT_ADD(cpbmp, fte_coll_port);
    BCM_IF_ERROR_RETURN
        (bcm_vlan_port_add(unit, fte_coll_vlan, cpbmp, cupbmp));
    bcm_l2_addr_t_init(&l2_addr, fte_coll_dst_mac, fte_coll_vlan);
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port  = fte_coll_port;
    BCM_IF_ERROR_RETURN
        (bcm_l2_addr_add(unit, &l2_addr));
    BCM_IF_ERROR_RETURN
        (bcm_switch_control_set(unit, bcmSwitchL2StaticMoveToCpu, TRUE));

    /* now set collector profile */
    bcm_collector_info_t_init(&collector_info);
    bcm_collector_export_profile_t_init(&profile_info);
    sal_memcpy(collector_info.eth.dst_mac, fte_coll_dst_mac, 6);
    sal_memcpy(collector_info.eth.src_mac, fte_coll_src_mac, 6);
    collector_info.eth.vlan_tag_structure =
        BCM_FLOWTRACKER_COLLECTOR_ETH_HDR_SINGLE_TAGGED;
    collector_info.eth.outer_vlan_tag = fte_coll_vlan;
    collector_info.eth.outer_tpid     = fte_coll_tpid;
    collector_info.ipv4.src_ip        = fte_coll_src_ip;
    collector_info.ipv4.dst_ip        = fte_coll_dst_ip;
    collector_info.ipv4.dscp          = fte_coll_dscp;
    collector_info.ipv4.ttl           = fte_coll_ttl;
    collector_info.udp.src_port       = fte_coll_l4_src_port;
    collector_info.udp.dst_port       = fte_coll_l4_dst_port;
    collector_info.ipfix.version = 0xA;
    collector_info.ipfix.initial_sequence_num = 0x2;
    collector_info.transport_type     = bcmCollectorTransportTypeIpv4Udp;
    /* BS2.0 Option - Update timestamp source for collector */
    /*
       collector_info.src_ts = bcmCollectorTimestampSourceNTP;
     */
    ft_collector_id1 = 0;
    BCM_IF_ERROR_RETURN
        (bcm_collector_create(unit, coptions, &ft_collector_id1, collector_info));
    profile_info.wire_format = bcmCollectorWireFormatIpfix;
    profile_info.max_packet_length = fte_coll_mtu;
    BCM_IF_ERROR_RETURN
        (bcm_collector_export_profile_create(unit, 0, &ft_export_profile_id1, &profile_info));

    printf("[CINT] Setup: BS Config Step3 - Created Collector id 0x%x, export_profile id 0x%x on port %d, vlan %d\n",
            ft_collector_id1, ft_export_profile_id1, fte_coll_port, fte_coll_vlan);
    return rv;
}

/* Step4 - Create Flowtracker Group */
bcm_flowtracker_group_t ft_group_id1 = -1;
int bcmft_group_create(int unit)
{
    int rv = BCM_E_NONE;
    bcm_flowtracker_group_info_t flow_group_info;
    bcm_flowtracker_group_info_t_init(&flow_group_info);
    flow_group_info.observation_domain_id = 0;
    BCM_IF_ERROR_RETURN
        (bcm_flowtracker_group_create(unit, 0, &ft_group_id1, &flow_group_info));

    printf("[CINT] Setup: BS Config Step4 - Created flowtracker group 0x%x.\n",ft_group_id1);
    return rv;
}

/* Step5 - Setup flow limit on group */
int bcmft_group_flow_limit_set(int unit)
{
    int rv = BCM_E_NONE;
    uint32 ft_flow_limit1 = 2;
    BCM_IF_ERROR_RETURN
        (bcm_flowtracker_group_flow_limit_set(unit, ft_group_id1, ft_flow_limit1));

    printf("[CINT] Setup: BS Config Step5 - Setup flow limit %d on flowtracker group.\n", ft_flow_limit1);
    return rv;
}

/* Step6 - Setup ageing time on group */
int bcmft_group_ageing_time_set(int unit)
{
    int rv = BCM_E_NONE;
    uint32 ft_ageing_internal_ms = 10000;
    BCM_IF_ERROR_RETURN
        (bcm_flowtracker_group_age_timer_set(unit, ft_group_id1, ft_ageing_internal_ms));

    printf("[CINT] Setup: BS Config Step6 - Setup ageing time %d ms on flowtracker group.\n", ft_ageing_internal_ms);
    return rv;
}

/* Step7 - Setup collector copy info */
int bcmft_group_collector_copy_info_set(int unit)
{
    int rv = BCM_E_NONE;
    uint32 ft_initial_samples = 3;
    bcm_flowtracker_collector_copy_info_t ft_cinfo1;
    bcm_flowtracker_collector_copy_info_t_init(&ft_cinfo1);
    ft_cinfo1.num_pkts_initial_samples = ft_initial_samples;
    BCM_IF_ERROR_RETURN
        (bcm_flowtracker_group_collector_copy_info_set(unit, ft_group_id1, ft_cinfo1));

    printf("[CINT] Setup: BS Config Step7 - Setup collector copy info on flowtracker group.\n");
    return rv;
}

/* Comment out Step 8  CPU notification since it is not supported */
/*
int bcmft_group_cpu_notification_set(int unit)
{
    int rv = BCM_E_NONE;
    bcm_flowtracker_cpu_notification_info_t ft_cpu_notification_info1;
    bcm_flowtracker_cpu_notification_info_t_init(&ft_cpu_notification_info1);
    BCM_FLOWTRACKER_CPU_NOTIFICATION_SET(ft_cpu_notification_info1, bcmFlowtrackerCpuNotificationFlowExceeded);
    BCM_IF_ERROR_RETURN
        (bcm_flowtracker_group_cpu_notification_set(unit, ft_group_id1, &ft_cpu_notification_info1));

    printf("[CINT] Setup: BS Config Step8 - Setup CPU notification on flowtracker group.\n");
    return rv;
}
*/

/* Step9 - Setup Group controls */
int bcmft_group_control_set(int unit)
{
    int rv = BCM_E_NONE;
    BCM_IF_ERROR_RETURN
        (bcm_flowtracker_group_control_set(unit, ft_group_id1, bcmFlowtrackerGroupControlNewLearnEnable, 1));

    printf("[CINT] Setup: BS Config Step9 - Setup controls on flowtracker group.\n");
    return rv;
}

/* Step10 - Setup meters on group */
int bcmft_group_meter_set(int unit)
{
    int rv = BCM_E_NONE;
    bcm_flowtracker_meter_info_t ft_meter_profile_info1;
    bcm_flowtracker_meter_info_t_init(&ft_meter_profile_info1);
    ft_meter_profile_info1.ckbits_sec = 10;
    ft_meter_profile_info1.ckbits_burst = 2;
    BCM_IF_ERROR_RETURN
        (bcm_flowtracker_group_meter_info_set(unit, ft_group_id1, ft_meter_profile_info1));

    printf("[CINT] Setup: BS Config Step10 - Setup meter on flowtracker group.\n");
    return rv;
}

/* BS2.0 Option - Drop Codes can be encoded in groups
 * To be used with tracking params
 * bcmFlowtrackerTrackingParamTypeIngDropReasonGroupIdVector
 * bcmFlowtrackerTrackingParamTypeEgrDropReasonGroupIdVector */
/* Step11 - Create drop reason groups */
bcm_flowtracker_drop_reason_group_t ft_ingress_dr_id1 = -1;
bcm_flowtracker_drop_reason_group_t ft_egress_dr_id1 = -1;
int bcmft_drop_reason_group_create(int unit)
{
    int rv = BCM_E_NONE;
    bcm_flowtracker_drop_reasons_t ft_egress_dr1;
    BCM_FLOWTRACKER_DROP_REASON_CLEAR_ALL(ft_egress_dr1);
    BCM_FLOWTRACKER_DROP_REASON_SET(ft_egress_dr1, bcmFlowtrackerDropReasonEgrFieldProcessor);
    BCM_IF_ERROR_RETURN
        (bcm_flowtracker_drop_reason_group_create(unit, 0, ft_egress_dr1, &ft_egress_dr_id1));

    printf("[CINT] Setup: BS Config Step11 - Created drop reason group with id 0x%x\n",ft_egress_dr_id1);
    return rv;
}

/* Step12 - */

/* Step13 - Create Flow checkers */
bcm_flowtracker_check_t ft_pri_chk_id1 = 0;
bcm_flowtracker_check_t ft_sec_chk_id1 = 0;
int bcmft_flow_check_create_and_group_add(int unit)
{
    int rv = BCM_E_NONE;
    int drp_grp_iter = 0;
    uint32 options_check1 = 0;
    uint32 options_check2 = BCM_FLOWTRACKER_CHECK_SECOND;
    bcm_flowtracker_check_info_t ft_check_info1;
    bcm_flowtracker_check_action_info_t ft_check_action_info1;
    bcm_flowtracker_check_export_info_t ft_check_export_info1;
    bcm_flowtracker_check_delay_info_t ft_check_delay_info1;

    /* Primary Check */
    bcm_flowtracker_check_info_t_init(&ft_check_info1);
    ft_check_info1.param = bcmFlowtrackerTrackingParamTypeTTL;
    ft_check_info1.min_value = 1;
    ft_check_info1.max_value = 200;
    /* BS2.0 Option - Update mask & shift value for attribute shift & mask functionality */
    /*
       ft_check_info1.mask_value = 0xFF;
       ft_check_info1.shift_value = 4;
     */
    ft_check_info1.flags = BCM_FLOWTRACKER_CHECK_EXPORT_MODE_THRESHOLD | BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT1;
    /* BS2.0 Option - New operation bcmFlowtrackerCheckOpMask has been added to do bitwise AND
     * of pkt attrib with max value as mask and then do bitwise OR of resultant. */
    ft_check_info1.operation = bcmFlowtrackerCheckOpInRange;
    /* BS2.0 Option - For drop reason based tracking param - update drop reason group id
     * Applicable for bcmFlowtrackerTrackingParamTypeIngDropReasonGroupIdVector
     * and bcmFlowtrackerTrackingParamTypeEgrDropReasonGroupIdVector */
    /*
       ft_check_info1.num_drop_reason_group_id = 1;
       for (drp_grp_iter = 0; drp_grp_iter < ft_check_info1.num_drop_reason_group_id; drp_grp_iter++) {
       ft_check_info1.drop_reason_group_id[0] = ft_ingress_dr_id1;
       }
     */
    BCM_IF_ERROR_RETURN
        (bcm_flowtracker_check_create(unit, options_check1, ft_check_info1, &ft_pri_chk_id1));
    printf("[CINT] Setup: BS Config Step13 - Primary Check 0x%08x created.\n",ft_pri_chk_id1);

    /* Secondary Check */
    /*
       bcm_flowtracker_check_info_t_init(&ft_check_info1);
       ft_check_info1.param = bcmFlowtrackerTrackingParamTypeIPProtocol;
       ft_check_info1.min_value = 6;
       ft_check_info1.max_value = 0;
       ft_check_info1.flags = 0;
       ft_check_info1.operation = bcmFlowtrackerCheckOpEqual;
       ft_check_info1.primary_check_id = ft_pri_chk_id1;
       BCM_IF_ERROR_RETURN
            (bcm_flowtracker_check_create(unit, options_check2, ft_check_info1, &ft_sec_chk_id1));
       printf("[CINT] Setup: BS Config Step13 - Secondary Check 0x%08x created.\n",ft_sec_chk_id1);
     */

    /* Attach action info to checker */
    bcm_flowtracker_check_action_info_t_init(&ft_check_action_info1);
    ft_check_action_info1.param = bcmFlowtrackerTrackingParamTypeNone;
    ft_check_action_info1.action = bcmFlowtrackerCheckActionCounterIncr;
    /* BS2.0 Option - Update mask & shift value for attribute shift & mask functionality */
    /*
       ft_check_action_info1.mask_value = 0;
       ft_check_action_info1.shift_value = 0;
     */
    BCM_IF_ERROR_RETURN
        (bcm_flowtracker_check_action_info_set(unit, ft_pri_chk_id1, ft_check_action_info1));
    printf("[CINT] Setup: BS Config Step13 - attached action info to checker.\n");

    /* Attach export info to checker */
    bcm_flowtracker_check_export_info_t_init(&ft_check_export_info1);
    ft_check_export_info1.export_check_threshold = traffic_pkt_num;
    ft_check_export_info1.operation = bcmFlowtrackerCheckOpGreaterEqual;
    BCM_IF_ERROR_RETURN
        (bcm_flowtracker_check_export_info_set(unit, ft_pri_chk_id1, ft_check_export_info1));
    printf("[CINT] Setup: BS Config Step13 - attached export info to checker.\n");

    /* BS2.0 Option - Attach delay info to checker for delay computing tracking params
     * bcmFlowtrackerTrackingParamTypeIngressDelay
     * bcmFlowtrackerTrackingParamTypeEgressDelay */
    /*
       bcm_flowtracker_check_delay_info_t_init(&ft_check_delay_info1);
       ft_check_delay_info1.src_ts = bcmFlowtrackerTimestampSourceCMIC;
       ft_check_delay_info1.dst_ts = bcmFlowtrackerTimestampSourceIngress;
       ft_check_delay_info1.gran = 0;
       ft_check_delay_info1.offset = 23;
       BCM_IF_ERROR_RETURN
           (bcm_flowtracker_check_delay_info_set(unit, ft_pri_chk_id1, ft_check_delay_info1));
       printf("[CINT] Setup: BS Config Step13 - attached delay info to checker.\n");
     */

    /* Add Flow Checker In FT Group */
    BCM_IF_ERROR_RETURN
        (bcm_flowtracker_group_check_add(unit, ft_group_id1, ft_pri_chk_id1));

    printf("[CINT] Setup: BS Config Step13 - added checker 0x%08x in flowtracker group 0x%x\n", ft_pri_chk_id1, ft_group_id1);
    return rv;
}

/* Step14 - Set tracking params to flowtracker group */
int max_num_tracking_params = 20;
int num_tracking_param_configured = 0;
int bcmft_group_tracking_param_set(int unit)
{
    int rv = BCM_E_NONE;
    int tparam_iter = 0;
    bcm_flowtracker_tracking_param_info_t ft_tracking_info1[max_num_tracking_params];
    /* Initialise tracking params structure */
    for (tparam_iter = 0; tparam_iter < max_num_tracking_params; tparam_iter++)
    {
        ft_tracking_info1[tparam_iter].flags = 0;
        ft_tracking_info1[tparam_iter].param = 0;
        ft_tracking_info1[tparam_iter].src_ts = 0;
        ft_tracking_info1[tparam_iter].check_id = 0;
    }
    /* Configure all tracking params on a group */
    /* Make sure this variable is set as per number of tracking param filled below */
    num_tracking_param_configured = 9;
    ft_tracking_info1[0].flags = BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY;
    ft_tracking_info1[0].param = bcmFlowtrackerTrackingParamTypeSrcIPv4;
    ft_tracking_info1[1].flags = BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY;
    ft_tracking_info1[1].param = bcmFlowtrackerTrackingParamTypeDstIPv4;
    ft_tracking_info1[2].flags = BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY;
    ft_tracking_info1[2].param = bcmFlowtrackerTrackingParamTypeL4SrcPort;
    ft_tracking_info1[3].flags = BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY;
    ft_tracking_info1[3].param = bcmFlowtrackerTrackingParamTypeL4DstPort;
    ft_tracking_info1[4].flags = BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY;
    ft_tracking_info1[4].param = bcmFlowtrackerTrackingParamTypeIPProtocol;
    ft_tracking_info1[5].flags = 0;
    ft_tracking_info1[5].param = bcmFlowtrackerTrackingParamTypePktCount;
    ft_tracking_info1[6].flags = 0;
    ft_tracking_info1[6].param = bcmFlowtrackerTrackingParamTypeL4SrcPort;
    ft_tracking_info1[7].flags = 0;
    ft_tracking_info1[7].param = bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1;
    /* BS2.0 Option - fill source of timestamp based tracking params */
    ft_tracking_info1[7].src_ts = bcmFlowtrackerTimestampSourceNTP;
    ft_tracking_info1[8].flags = 0;
    ft_tracking_info1[8].param = bcmFlowtrackerTrackingParamTypeFlowtrackerCheck;
    ft_tracking_info1[8].check_id = ft_pri_chk_id1;
    BCM_IF_ERROR_RETURN
        (bcm_flowtracker_group_tracking_params_set(unit, ft_group_id1, num_tracking_param_configured, ft_tracking_info1));

    printf("[CINT] Setup: BS Config Step14 - Setup tracking params on flowtracker group 0x%x\n",ft_group_id1);
    return rv;
}

/* Step15 - Create & Validate Export Template, attach it */
int max_export_elements = 30;
bcm_flowtracker_export_template_t ft_template_id1 = -1;
int bcmft_group_export_template_validate_create_group_attach_it(int unit)
{
    int rv = BCM_E_NONE;
    int elem;
    int exp_elem_iter = 0;
    int tparam_iter = 0;
    int actual_export_elements = 0;
    uint32 options_export = 0x0;
    uint16 fte_set_id = 0x8001;
    char *export_element_string = NULL;
    int actual_tracking_params_configured = 0;
    bcm_flowtracker_export_element_info_t export_elems_ip[max_export_elements];
    bcm_flowtracker_export_element_info_t export_elems_op[max_export_elements];
    bcm_flowtracker_tracking_param_info_t ft_tracking_info1_get[max_num_tracking_params];
    /* Initialise tracking params structure */
    for (tparam_iter = 0; tparam_iter < max_num_tracking_params; tparam_iter++)
    {
        ft_tracking_info1_get[tparam_iter].flags = 0;
        ft_tracking_info1_get[tparam_iter].param = 0;
        ft_tracking_info1_get[tparam_iter].src_ts = 0;
        ft_tracking_info1_get[tparam_iter].check_id = 0;
    }
    /* Initialise all export elements structures */
    for (exp_elem_iter = 0; exp_elem_iter < max_export_elements; exp_elem_iter++) {
        export_elems_ip[exp_elem_iter].data_size = 0;
        export_elems_ip[exp_elem_iter].element = bcmFlowtrackerExportElementTypeReserved;
        export_elems_op[exp_elem_iter].data_size = 0;
        export_elems_op[exp_elem_iter].element = bcmFlowtrackerExportElementTypeReserved;
    }
    /* Get all tracking params configured first */
    BCM_IF_ERROR_RETURN
        (bcm_flowtracker_group_tracking_params_get(unit, ft_group_id1, max_num_tracking_params,
            ft_tracking_info1_get, &actual_tracking_params_configured));
    if (actual_tracking_params_configured != num_tracking_param_configured) {
        printf("[CINT] Setup: BS Config Step15 - Error! Configured tracking params are not same, check cscript !\n");
    }
    /* Convert all tracking parasms into equivalent export element */
    for (tparam_iter = 0; tparam_iter < actual_tracking_params_configured; tparam_iter++) {
        rv = fte_tracking_param_xlate(unit, ft_tracking_info1_get[tparam_iter].param,
                &(export_elems_ip[tparam_iter].data_size),
                &(export_elems_ip[tparam_iter].element));
        if (BCM_FAILURE(rv)) {
            printf("[CINT] Setup: BS Config Step15 - Error! Coverting export element for provided tracking param\n");
        }
        if (ft_tracking_info1_get[tparam_iter].param == bcmFlowtrackerTrackingParamTypeFlowtrackerCheck) {
            export_elems_ip[tparam_iter].check_id = ft_tracking_info1_get[tparam_iter].check_id;
        }
    }
    /* Add FlowGroup to export info elements */
    export_elems_ip[tparam_iter].element = bcmFlowtrackerExportElementTypeFlowtrackerGroup;
    export_elems_ip[tparam_iter].data_size = 2;
    actual_tracking_params_configured++;
    /* Print Input Export Elements */
    printf("*************************************************************************************************\n");
    printf("[CINT] Input information elements (total %d): \n", actual_tracking_params_configured);
    for (exp_elem_iter = 0; exp_elem_iter < actual_tracking_params_configured; exp_elem_iter++) {
        elem = (export_elems_ip[exp_elem_iter].element);
        export_element_string = ft_convert_export_element_to_string(elem);
        printf("\tinput info element (%2d): elem = %3d(%35s) and size = %3d\n", exp_elem_iter, elem,
                export_element_string, export_elems_ip[exp_elem_iter].data_size);
    }
    printf("*************************************************************************************************\n");
    /* Validate export template */
    BCM_IF_ERROR_RETURN
        (bcm_flowtracker_export_template_validate(unit, ft_group_id1, actual_tracking_params_configured,
            export_elems_ip, max_export_elements,
            export_elems_op, &actual_export_elements));
    /* Print Output Export Elements */
    printf("*************************************************************************************************\n");
    printf("[CINT] Output information elements (total %d): \n", actual_export_elements);
    for (exp_elem_iter = 0; exp_elem_iter < actual_export_elements; exp_elem_iter++) {
        elem = (export_elems_op[exp_elem_iter].element);
        export_element_string = ft_convert_export_element_to_string(elem);
        printf("\toutput info element (%2d): elem = %3d(%35s) and size = %3d\n", exp_elem_iter, elem,
                export_element_string, export_elems_op[exp_elem_iter].data_size);
    }
    printf("*************************************************************************************************\n");
    /* Create export template with provided set id */
    BCM_IF_ERROR_RETURN
        (bcm_flowtracker_export_template_create(unit, options_export, &ft_template_id1,
            fte_set_id, actual_export_elements, export_elems_op));
    /* Attach created export template and collector to group */
    BCM_IF_ERROR_RETURN
        (bcm_flowtracker_group_collector_attach(unit, ft_group_id1, ft_collector_id1, ft_export_profile_id1, ft_template_id1));

    printf("[CINT] Setup: BS Config Step15 - Created export template, validate and attached to flowtracker group 0x%x\n",ft_group_id1);
    return rv;
}

/* Step16 - Setup export trigger */
int bcmft_group_export_trigger_set(int unit)
{
    int rv = BCM_E_NONE;
    bcm_flowtracker_export_trigger_info_t trig_info1;
    BCM_FLOWTRACKER_TRIGGER_SET(trig_info1, bcmFlowtrackerExportTriggerNewLearn);
    BCM_IF_ERROR_RETURN
        (bcm_flowtracker_group_export_trigger_set(unit, ft_group_id1, &trig_info1));

    printf("[CINT] Setup: BS Config Step16 - Setup export trigger for group.\n");
    return rv;
}

/* Step17 - Set up FTFP entry */
bcm_field_entry_t ftfp_entry1 = -1;
int bcmft_group_ftfp_entry_create_qualify_install(int unit)
{
    int rv = BCM_E_NONE;
    BCM_IF_ERROR_RETURN
        (bcm_field_entry_create(unit, ftfp_group1, &ftfp_entry1));
    BCM_IF_ERROR_RETURN
        (bcm_field_action_add(unit, ftfp_entry1, bcmFieldActionFlowtrackerGroupId, ft_group_id1, 0));
    BCM_IF_ERROR_RETURN
        (bcm_field_qualify_IpProtocol(unit, ftfp_entry1, pkt_protocol, 0xff));
    BCM_IF_ERROR_RETURN
        (bcm_field_qualify_InPort(unit, ftfp_entry1, traffic_ing_port, 0xff));
    BCM_IF_ERROR_RETURN
        (bcm_field_qualify_OuterVlanId(unit, ftfp_entry1, pkt_ingress_vlan, 0xfff));
    BCM_IF_ERROR_RETURN
        (bcm_field_qualify_VlanFormat(unit, ftfp_entry1, pkt_vlan_format, 0xf));
    BCM_IF_ERROR_RETURN
        (bcm_field_entry_install(unit, ftfp_entry1));

    printf("[CINT] Setup: BS Config Step17 - Installed FTFP entry for group.\n");
    return rv;
}

/* Step18 - Setup Time in system for timestamp related test */
int bcmft_time_set(int unit)
{
    int rv = BCM_E_NONE;
    uint8 src_stage = 0;
    uint32 secu = 0, secl = 0x01020304;
    uint32 nano_sec = 0x11223344;
    uint32 nano_secu = 0, nano_secl = 0;

    /* ft_time_set(unit, source_stage, sec_upper, sec_lower, nano_sec, nano_sec_adjust_upper, nano_sec_adjust_lower) */
    src_stage = 0;
    rv = ft_time_set(unit, 0, secu, secl, nano_sec, nano_secu, nano_secl);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] Setup: BS Config Step18 - Error! time not set.\n");
    } else {
        printf("[CINT] Setup: BS Config Step18 - System Time has been setup.\n");
    }

    /* Start CMIC Counter */
    if (enable_cmic_counter) {
        src_stage = 3;
        rv = ft_time_set(unit, src_stage, 0, 0, 0, 0, 0);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] Setup: BS Config Step18 - Error! CMIC timer not set.\n");
        } else {
            printf("[CINT] Setup: BS Config Step18 - CMIC time has been setup.\n");
        }
    }

    return rv;
}

int traffic_ing_port;
int traffic_egr_port;
int bcmft_configure_vfp(int unit) 
{
    int rv = BCM_E_NONE;
    bcm_field_qset_t qset;
    bcm_field_group_t vfp_group;
    bcm_field_entry_t vfp_entry;

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageLookup);
    BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset, BCM_FIELD_GROUP_PRIO_ANY, &vfp_group));
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, vfp_group, &vfp_entry));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, vfp_entry, traffic_ing_port, -1));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, vfp_entry, bcmFieldActionAssignOpaqueObject2, 0xb, 0));
    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, vfp_entry));
    
    return rv;
}

/* System setup before actual configuration is done */
int bstest_before_actual_config(int unit)
{
    int rv = BCM_E_NONE;
    /* Dump config variables to see if all broadscan2.0 config are loaded. */
    bshell(unit, "config show flow");
    /* Start ftrmon thread to monitor ipfix packet */
    bshell(unit, "ftrmon start");
    bshell(unit, "ftrmon report +decode");

    printf("[CINT] Monitoring: Started ftrmon task for monitoring !\n");
    printf("[CINT] Env: enable_cmic_counter %d, bcmsim_run %d, skip_cleanup %d\n",
                enable_cmic_counter, bcmsim_run, skip_cleanup);
    return rv;
}

/*
 * This function is written so that hardcoding of port
 * numbers in Cint scripts is removed. This function gives
 * required number of ports
 */
bcm_error_t
portNumbersGet(int unit, int *port_list, int num_ports)
{
    int i = 0, port = 0, rv = 0;
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
        return -1;
    }

    return BCM_E_NONE;
}

/* Setup traffic path first */
int bstest_traffic_path_setup(int unit)
{


    int port_list[2];

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 2)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    traffic_ing_port = port_list[0];
    traffic_egr_port = port_list[1];

    BCM_IF_ERROR_RETURN
        (bcm_port_loopback_set(unit, traffic_ing_port, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN
        (bcm_port_loopback_set(unit, traffic_egr_port, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN
        (bcm_port_discard_set(unit, traffic_egr_port, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN
        (bcm_stat_clear(unit, traffic_ing_port));
    BCM_IF_ERROR_RETURN
        (bcm_stat_clear(unit, traffic_egr_port));

    int rv = BCM_E_NONE;
    bcm_pbmp_t pbmp, upbmp;
    BCM_IF_ERROR_RETURN
        (bcm_vlan_create(unit, pkt_ingress_vlan));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, traffic_ing_port);
    BCM_PBMP_PORT_ADD(pbmp, traffic_egr_port);;
    BCM_IF_ERROR_RETURN
        (bcm_vlan_port_add(unit, pkt_ingress_vlan, pbmp, upbmp));

    printf("[CINT] Traffic: Created packet path vlan %d: ingr port %d - egr port %d\n",pkt_ingress_vlan, traffic_ing_port, traffic_egr_port);
    return rv;
}

/* Broasdscan configurations */
int bs2_track_export_setup(int unit)
{
    int rv = BCM_E_NONE;

    /* Step1 - Create presel entries */
    BCM_IF_ERROR_RETURN
        (bcmft_presel_entry_create(unit));
    /* Step2 - Create FTFP group */
    BCM_IF_ERROR_RETURN
        (bcmft_ftfp_group_create(unit));
    /* Step3 - Create Collector Port, collector vlan & export profile */
    BCM_IF_ERROR_RETURN
        (bcmft_collector_create(unit));
    /* Step4 - Create Flowtracker Group */
    BCM_IF_ERROR_RETURN
        (bcmft_group_create(unit));
    /* Step5 - Setup flow limit on group */
    BCM_IF_ERROR_RETURN
        (bcmft_group_flow_limit_set(unit));
    /* Step6 - Setup ageing time on group */
    /* BCM_IF_ERROR_RETURN
        (bcmft_group_ageing_time_set(unit)); */
    /* Step7 - Setup collector copy info */
    /* BCM_IF_ERROR_RETURN
        (bcmft_group_collector_copy_info_set(unit)); */
    /* Step8 - Setup CPU notification */
    /* BCM_IF_ERROR_RETURN
        (bcmft_group_cpu_notification_set(unit)); */
    /* Step9 - Setup Group controls */
    /* BCM_IF_ERROR_RETURN
        (bcmft_group_control_set(unit)); */
    /* Step10 - Setup meters on group */
    /* BCM_IF_ERROR_RETURN
        (bcmft_group_meter_set(unit)); */
    /* Step11 - Create drop reason groups */
    /* BCM_IF_ERROR_RETURN
        (bcmft_drop_reason_group_create(unit)); */
    /* Step13 - Create Flow checkers */
    BCM_IF_ERROR_RETURN
        (bcmft_flow_check_create_and_group_add(unit));
    /* Step14 - Set tracking params to flowtracker group */
    BCM_IF_ERROR_RETURN
        (bcmft_group_tracking_param_set(unit));
    /* Step15 - Create & Validate Export Template, attach it */
    BCM_IF_ERROR_RETURN
        (bcmft_group_export_template_validate_create_group_attach_it(unit));
    /* Step16 - Setup export trigger */
    /* BCM_IF_ERROR_RETURN
        (bcmft_group_export_trigger_set(unit)); */
    /* Step17 - Set up FTFP entry */
    BCM_IF_ERROR_RETURN
        (bcmft_group_ftfp_entry_create_qualify_install(unit));
    /* Step18 - Setup Time in system for timestamp related test */
    BCM_IF_ERROR_RETURN
        (bcmft_time_set(unit));
    /* vfp rule */
    printf("[CINT] Configure fp rule \n");
    BCM_IF_ERROR_RETURN
        (bcmft_configure_vfp(unit));

    return rv;
}

/* Send traffic */
int traffic_pkt_num = 1;
int bstest_tx_traffic(int unit)
{
    int rv = BCM_E_NONE;
    BCM_IF_ERROR_RETURN
        (tx_broadscan_test_packet(unit, ipv4_tcp_sync_pkt1, sizeof(ipv4_tcp_sync_pkt1), traffic_ing_port, traffic_pkt_num));

    printf("[CINT] Test Traffic sent, num_packets %d ......\n", traffic_pkt_num);
    return rv;
}

/* After setup & tx, trigger export
 * wait for ipfix export packet to reach ftrmon task */
int bstest_after_tx_traffic(int unit)
{
    int rv = BCM_E_NONE;

    /* Validate export packet in ftRmon task */
    printf("[CINT] Sleeping for 3 seconds for ftRmon to validate.. The time can vary \n");
    sal_sleep(3);

    return rv;
}

/* Cleanup all configurations done */
int bstest_cleanup_main(int unit)
{
    int rv = BCM_E_NONE;
    bshell(unit, "ftrmon stop");
    if (cmic_time_intfid != -1) {
        BCM_IF_ERROR_RETURN
            (bcm_time_interface_delete(unit, cmic_time_intfid));
        cmic_time_intfid = -1;
    }
    if (ftfp_entry1 != -1) {
        BCM_IF_ERROR_RETURN
            (bcm_field_entry_destroy(unit, ftfp_entry1));
        ftfp_entry1 = -1;
    }
    if (ftfp_group1 != -1) {
        BCM_IF_ERROR_RETURN
            (bcm_field_group_destroy(unit, ftfp_group1));
        ftfp_group1 = -1;
    }
    if (ftfp_presel_id1 != -1) {
        BCM_IF_ERROR_RETURN
            (bcm_field_presel_destroy(unit, ftfp_presel_id1));
        ftfp_presel_id1 = -1;
    }
    BCM_IF_ERROR_RETURN
        (bcm_flowtracker_group_clear(unit, ft_group_id1, BCM_FLOWTRACKER_GROUP_CLEAR_ALL));
    BCM_IF_ERROR_RETURN
        (bcm_vlan_destroy(unit, fte_coll_vlan));
    BCM_IF_ERROR_RETURN
        (bcm_l2_addr_delete(unit, fte_coll_dst_mac, fte_coll_vlan));
    BCM_IF_ERROR_RETURN
        (bcm_switch_control_set(unit, bcmSwitchL2StaticMoveToCpu, FALSE));
    BCM_IF_ERROR_RETURN
        (bcm_flowtracker_group_collector_detach(unit, ft_group_id1, ft_collector_id1, ft_export_profile_id1, ft_template_id1));
    if (ft_collector_id1 != -1) {
        BCM_IF_ERROR_RETURN
            (bcm_collector_destroy(unit, ft_collector_id1));
        ft_collector_id1 = -1;
    }
    if (ft_export_profile_id1 != -1) {
        BCM_IF_ERROR_RETURN
            (bcm_collector_export_profile_destroy(unit, ft_export_profile_id1));
        ft_export_profile_id1 = -1;
    }
    if (ft_template_id1 != -1) {
        BCM_IF_ERROR_RETURN
            (bcm_flowtracker_export_template_destroy(unit, ft_template_id1));
        ft_template_id1 = -1;
    }
    BCM_IF_ERROR_RETURN
        (bcm_flowtracker_group_check_delete(unit, ft_group_id1, ft_pri_chk_id1));
    if (ft_sec_chk_id1 != 0) {
        BCM_IF_ERROR_RETURN
            (bcm_flowtracker_check_destroy(unit, ft_sec_chk_id1));
        ft_sec_chk_id1 = 0;
    }
    if (ft_pri_chk_id1 != 0) {
        BCM_IF_ERROR_RETURN
            (bcm_flowtracker_check_destroy(unit, ft_pri_chk_id1));
        ft_pri_chk_id1 = 0;
    }
    if (ft_ingress_dr_id1 != -1) {
        BCM_IF_ERROR_RETURN
            (bcm_flowtracker_drop_reason_group_delete(unit, ft_ingress_dr_id1));
        ft_ingress_dr_id1 = -1;
    }
    if (ft_egress_dr_id1 != -1) {
        BCM_IF_ERROR_RETURN
            (bcm_flowtracker_drop_reason_group_delete(unit, ft_egress_dr_id1));
        ft_egress_dr_id1 = -1;
    }
    BCM_IF_ERROR_RETURN
        (bcm_flowtracker_group_flow_limit_set(unit, ft_group_id1, 0));
    BCM_IF_ERROR_RETURN
        (bcm_flowtracker_group_clear(unit, ft_group_id1, BCM_FLOWTRACKER_GROUP_CLEAR_ALL));
    /* Cleanup in sim cannot be done by group flush functionality present in hardware
     * hence need to be done by writing 0 in all session data table indexes.
     * Index range may vary across chips. */
    if (bcmsim_run) {
        bshell(unit, "w ft_key_double 0 4607 0 0 0 0 0 0 0 0 0 0 0 0");
        bshell(unit, "w bsc_dt_flex_session_data_double 0 4607 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0");
    }
    if (ft_group_id1 != -1) {
        BCM_IF_ERROR_RETURN
            (bcm_flowtracker_group_destroy(unit, ft_group_id1));
        ft_group_id1 = -1;
    }
    BCM_IF_ERROR_RETURN
        (bcm_port_loopback_set(unit, traffic_ing_port, BCM_PORT_LOOPBACK_NONE));
    BCM_IF_ERROR_RETURN
        (bcm_port_discard_set(unit, traffic_egr_port, BCM_PORT_DISCARD_NONE));
    BCM_IF_ERROR_RETURN
        (bcm_port_loopback_set(unit, traffic_egr_port, BCM_PORT_LOOPBACK_NONE));
    BCM_IF_ERROR_RETURN
        (bcm_vlan_destroy(unit, pkt_ingress_vlan));

    printf("[CINT] Test Configurations cleaned up ......\n");
    return rv;
}

/*
 * Test Setup:
 * This function setup the test.
 */
bcm_error_t
test_setup(int unit)
{
    
    bcm_error_t rv;
	
    if (BCM_FAILURE(rv = (bstest_before_actual_config(unit)))) {
        printf("bstest_before_actual_config() failed.\n");
		return rv;
    }

    if (BCM_FAILURE(rv = (bstest_traffic_path_setup(unit)))) {
        printf("bstest_traffic_path_setup() failed.\n");
		return rv;
    }

    return rv;	
}

void verify(int unit)
{

    bcm_error_t rv;
    /*printf("[CINT] Dump timestamps before sending traffic ..\n");
    ft_time_get_print(unit, 0xFF);*/

    if (BCM_FAILURE(rv = (bstest_tx_traffic(unit)))) {
        printf("bstest_tx_traffic() failed.\n");
    }

    if (BCM_FAILURE(rv = (bstest_after_tx_traffic(unit)))) {
        printf("bstest_after_tx_traffic() failed.\n");
    }

    /*printf("[CINT] Dump timestamps after sending traffic ..\n");
    ft_time_get_print(unit, 0xFF);*/

    if (!skip_cleanup) {
        if (BCM_FAILURE(rv = (bstest_cleanup_main(unit)))) {
            printf("bstest_cleanup_main() failed.\n");
		}
    }
}

void execute(void)
{

    bcm_error_t rv;
    int unit = 0;
    print "config show; attach; version";
    bshell(unit, "config show; a ; version");

    print "~~~ #1) test_setup(): ** start **";
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return -1;
    }
    print "~~~ #1) test_setup(): ** end **";

    print "~~~ #2) bs2_track_export_setup(): ** start **";
    if (BCM_FAILURE((rv = bs2_track_export_setup(unit)))) {
        printf("bs2_track_export_setup() failed.\n");
        return -1;
    }
    print "~~~ #2) bs2_track_export_setup(): ** end **";

    print "~~~ #3) verify(): ** start **";
    verify(unit);
    print "~~~ #3) verify(): ** end **";

    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
    print "execute(): Start";
    print execute();
    print "execute(): End";
}

