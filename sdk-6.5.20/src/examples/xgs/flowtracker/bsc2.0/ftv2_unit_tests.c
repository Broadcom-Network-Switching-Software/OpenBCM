/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/* Reset CINT */
cint_reset();

/* Device specific variables */
/* HR4 */
//int ift_key_double_entry_count = 4607;
//uint8 ft_broadscan_version = 1;
/* FB6 */
int ift_key_double_entry_count = 131071;
uint8 ft_broadscan_version = 2;


/* Include all relevant cint files here */
#include "ftv2_resource_config.c"
#include "ftv2_param_sanity.c"
#include "ftv2_ftfp_presel.c"
#include "ftv2_ftfp_group.c"
#include "ftv2_collector.c"
#include "ftv2_group.c"
#include "ftv2_drop.c"
#include "ftv2_check.c"
#include "ftv2_export.c"
#include "ftv2_timestamp.c"
#include "ftv2_tx.c"
#include "ftv2_rx.c"
#include "ftv2_validate.c"

/* Global Variables all cint files */
int unit = 0;
uint16 debug_flags = 0;
uint8 dump_mem_reg = 0;
uint8 dump_diag_shell = 0;
uint8 use_ftrmon_thread = 0;
uint8 skip_cleanup = 0;
uint8 skip_log = 0;
uint8 tc_in_progress = 0;
uint8 bcmsim_ftv2_enabled = 1;

/* Display TestMenu */
int
testMenu()
{
    printf("[CINT] :: Displaying All the test cases currently available for Broadscan2.0\n");
    printf("[CINT] :: -----------------------------------------------------------------------------------------------------------------------\n");
    printf("[CINT] :: TC  1: Check params sanity - Drop Reason, Check , Group\n");

    /* Test Case 2 - Total IFT data (14B)
     * 0000000000000000 (b3) - EFT
     * 0000000000000000 (b2) - MFT
     * 00000002000000b0 (b1) - IFT - PktCount(4B)(ALU32_0), ByteCount(4B)(ALU32_1)
     * 3344000206400000 (b0) - IFT - L4SrcPort(2B)(LOAD16_0), Checker(2B)(ALU16_0), Protocol(1B)(LOAD8_0), TTL(1B)(LOAD8_1)
     */
    printf("[CINT] :: TC  2: { => Resource Config 1} {Key - AT_IT_IPV4 5Tuple (IpProtocol + SrcIpv4 + DstIpv4 + L4SrcPort + L4DstPort}\n");
    printf("[CINT] :: TC  2: {Data - IpProtocol(LOAD8) + PktCount(ALU32) + ByteCount(ALU32) + TTL(LOAD8) + L4Srcport(LOAD16)}\n");
    printf("[CINT] :: TC  2: {PCheck - TTL, Range: 1-200} {Action - CounterIncr} {Export - PktCnt} {Flags - ExportModeThreshold}\n");
    printf("[CINT] :: TC  2: {ALL IFT data, IFT trigger, Single Wide Mode, ALU16 Export Triggered, PktCount Sent: xxx}\n");

    /* Test Case 3 - Total IFT data (20B)
     * 0000000000000000 (b7) - EFT
     * 0000000000000000 (b6) - MFT
     * 0003064000000000 (b5) - IFT - Checker(2B)(ALU16_0), Protocol(1B)(LOAD8_0), TTL(1B)(LOAD8_1)
     * 0000000000000000 (b4) - IFT
     * 000000000000000) (b3) - EFT
     * 0000000000000000 (b2) - MFT
     * 11220a0b0c0d0000 (b1) - IFT - Timestamp(6B)(TS0), PktCount(4B)(ALU32_0)(First 2 bytes out of 4)
     * 0003000001083344 (b0) - IFT - PktCount(4B)(ALU32_0)(Last 2 bytes out of 4), ByteCount(4B)(ALU32_1), L4SrcPort(2B)(LOAD16_0)
     */
    printf("[CINT] :: TC  3: { => Resource Config 1} {Key - AT_IT_IPV4 5Tuple (IpProtocol + SrcIpv4 + DstIpv4 + L4SrcPort + L4DstPort}\n");
    printf("[CINT] :: TC  3: {Data - IpProtocol(LOAD8) + PktCount(ALU32) + ByteCount(ALU32) + TTL(LOAD8) + L4Srcport(LOAD16) + CheckEvent1-Legacy(TS)}\n");
    printf("[CINT] :: TC  3: {PCheck - IpProtocol, Range: 1-20} {Action - CounterIncr} {Export - PktCnt} {Flags - ExportModeThreshold, CheckEvent1}\n");
    printf("[CINT] :: TC  3: {ALL IFT data, IFT trigger, Double Wide Mode, ALU16 Export Triggered, PktCount Sent: xxx}\n");

    /* Test Case 4 - Total IFT data (12B), MFT data(9B), EFT data(2B)
     * 0000000000000000 (b7) - EFT
     * 2f00000000000000 (b6) - MFT - IngressPort(1B)(LOAD8_4)
     * 0000000000000000 (b5) - IFT
     * 0000000000000000 (b4) - IFT
     * 3200000000000000 (b3) - EFT - EgressPort(1B)(LOAD8_6)
     * 0a0b112233440003 (b2) - MFT - Timestamp(6B)(TS2),  Checker(2B)(ALU16_5)
     * 0000000300000108 (b1) - IFT - PktCount(4B)(ALU32_0), ByteCount(4B)(ALU32_1)
     * 3344064000000000 (b0) - IFT - L4SrcPort(2B)(LOAD16_0), Protocol(1B)(LOAD8_0), TTL(1B)(LOAD8_1)
     */
    printf("[CINT] :: TC  4: { => Resource Config 1} {Key - AT_IT_IPV4 5Tuple (IpProtocol + SrcIpv4 + DstIpv4 + L4SrcPort + L4DstPort}\n");
    printf("[CINT] :: TC  4: {Data - IpProtocol(LOAD8) + PktCount(ALU32) + ByteCount(ALU32) + TTL(LOAD8) + L4Srcport(LOAD16) + \n");
    printf("[CINT] :: TC  4: {     - IngressPort(LOAD8) + EgressPort(LOAD8) + CheckEvent1-Legacy(TS)}\n");
    printf("[CINT] :: TC  4: {PCheck - IngressPort, Equal, Min-yyy} (Action - CounterIncr} {Export - PktCount} {Flags - ExportModeThreshold, CheckEvent1)}\n");
    printf("[CINT] :: TC  4: {IFT, MFT & EFT  data, MFT trigger, Double Wide Mode, ALU16 Export Triggered, PktCount Sent: xxx}\n");

    /* Test Case 5 - Total IFT data (12B), MFT data(1B), EFT data(9B)
     * 3200000000000000 (b7) - EFT - EgressPort(1B)(LOAD8_6)
     * 0000000000000000 (b6) - MFT
     * 0000000000000000 (b5) - IFT
     * 0000000000000000 (b4) - IFT
     * ffff010203040003 (b3) - EFT - Timestamp(6B)(TS3), Checker(2B)(ALU16_6)
     * 2f00000000000000 (b2) - MFT - IngressPort(1B)(LOAD8_4)
     * 0000000300000108 (b1) - IFT - PktCount(4B)(ALU32_0), ByteCount(4B)(ALU32_1)
     * 3344064000000000 (b0) - IFT - L4SrcPort(2B)(LOAD16_0), Protocol(1B)(LOAD8_0), TTL(1B)(LOAD8_1)
     */
    printf("[CINT] :: TC  5: { => Resource Config 1} {Key - AT_IT_IPV4 5Tuple (IpProtocol + SrcIpv4 + DstIpv4 + L4SrcPort + L4DstPort}\n");
    printf("[CINT] :: TC  5: {Data - IpProtocol(LOAD8) + PktCount(ALU32) + ByteCount(ALU32) + TTL(LOAD8) + L4Srcport(LOAD16) + \n");
    printf("[CINT] :: TC  5: {IngressPort(LOAD8) + EgressPort(LOAD8) + CheckEvent1-Legacy(TS)}\n");
    printf("[CINT] :: TC  5: {PCheck - EgressPort, Equal, Min-yyy} {Action - CounterIncr} {Export - PktCount} {Flags - ExportModeThreshold, CheckEvent1}\n");
    printf("[CINT] :: TC  5: {IFT, MFT & EFT  data, EFT trigger, Double Wide Mode, ALU16 Export Triggered, PktCount Sent: xxx}\n");

    /* Test Case 6 - Total IFT data (14B)
     * 0000000000000000 (b3) - EFT
     * 0000000000000000 (b2) - MFT
     * 0000000100000058 (b1) - IFT - PktCount(4B)(ALU32_0), ByteCount(4B)(ALU32_1)
     * 3344003306400000 (b0) - IFT - L4SrcPort(2B)(LOAD16_0), Checker(2B)(ALU16_0), Protocol(1B)(LOAD8_0), TTL(1B)(LOAD8_1)
     */
    printf("[CINT] :: TC  6: { => Resource Config 1} {Key - AT_IT_IPV4 5Tuple (IpProtocol + SrcIpv4 + DstIpv4 + L4SrcPort + L4DstPort}\n");
    printf("[CINT] :: TC  6: {Data - IpProtocol(LOAD8) + PktCount(ALU32) + ByteCount(ALU32) + TTL(LOAD8) + L4Srcport(LOAD16)}\n");
    printf("[CINT] :: TC  6: {PCheck - IPProtocol, Equal, ParamMask & Shift} {SCheck - TTL, Equal, ParamMask & Shift}\n");
    printf("[CINT] :: TC  6: {         Action - UpdateValue(L4SrcPort), ParamMask & Shift} {Export - xxx} {Flags - ExportModeThreshold}\n");
    printf("[CINT] :: TC  6: {ALL IFT data , IFT trigger, Single Wide Mode, ALU16 Export Triggered, PktCount Sent: 1}\n");

    /* Test Case 7 - Total IFT data (12B), MFT data(1B), EFT data(14B)
     * e404000332000000 (b7) - EFT - CheckDelay(4B)(ALU32_10)(Last 2 bytes out of 4), EgressPort(LOAD8_6), Checker(2B)(ALU16_6)
     * 0000000000000000 (b6) - MFT
     * 0000000000000000 (b5) - IFT
     * 0000000000000000 (b4) - IFT
     * 0000070707070005 (b3) - EFT - Timestamp(6B)(TS3), CheckDelay(4B)(ALU32_10)(First 2 bytes out of 4)
     * 2f00000000000000 (b2) - MFT - IngressPort(1B)(LOAD8_4)
     * 0000000300000108 (b1) - IFT - PktCount(4B)(ALU32_0), ByteCount(4B)(ALU32_1)
     * 3344064000000000 (b0) - IFT - L4SrcPort(2B)(LOAD16_0), Protocol(1B)(LOAD8_0), TTL(1B)(LOAD8_1)
     */
    printf("[CINT] :: TC  7: { => Resource Config 1} {Key - AT_IT_IPV4 5Tuple (IpProtocol + SrcIpv4 + DstIpv4 + L4SrcPort + L4DstPort}\n");
    printf("[CINT] :: TC  7: {Data - IpProtocol(LOAD8) + PktCount(ALU32) + ByteCount(ALU32) + TTL(LOAD8) + L4Srcport(LOAD16) + \n");
    printf("[CINT] :: TC  7: {       IngressPort(LOAD8) + EgressPort(LOAD8) + CheckEvent1-CMIC(TS) + ChipDelay(ALU32)}\n");
    printf("[CINT] :: TC  7: {PCheck - EgressPort, Equal, Min-yyy} {Action - CounterIncr} {Export - PktCnt} {Flags - ExportModeThreshold, CheckEvent1}\n");
    printf("[CINT] :: TC  7: {IFT, MFT & EFT  data, EFT trigger, Double Wide Mode, ALU16 Export Triggered, PktCount Sent: xxx}\n");

    /* Test Case 8 - Total IFT data (30B), MFT data(1B), EFT data(1B)
     * 0000000000000000 (b7) - EFT
     * 0000000000000000 (b6) - MFT
     * 0000010801010108 (b5) - IFT - ByteCount(4B)(ALU32_1) + IPATDelay(4B)(ALU32_2)
     * 3344000306400000 (b4) - IFT - L4SrcPort(2B)(LOAD16_0) + Checker(2B)(ALU16_0) + Protocol(1B)(LOAD8_0), TTL(1B)(LOAD8_1)
     * 3200000000000000 (b3) - EFT - EgressPort(1B)(LOAD8_6)
     * 2f00000000000000 (b2) - MFT - IngressPort(1B)(LOAD8_4)
     * 0000090808060000 (b1) - IFT - Timestamp(6B)(TS0), Internal Timestamp(6B)(TS1)(First 2Bytes)
     * 0908080600000003 (b0) - IFT - Internal Timestamp(6B)(TS1)(Last 4Bytes) + PktCount(4B)(ALU32_0)
     */
    printf("[CINT] :: TC  8: { => Resource Config 1} {Key - AT_IT_IPV4 5Tuple (IpProtocol + SrcIpv4 + DstIpv4 + L4SrcPort + L4DstPort}\n");
    printf("[CINT] :: TC  8: {Data - IpProtocol(LOAD8) + PktCount(ALU32) + ByteCount(ALU32) + TTL(LOAD8) + L4Srcport(LOAD16) + \n");
    printf("[CINT] :: TC  8: {       IngressPort(LOAD8) + IPATDelay(ALU32) + EgressPort(LOAD8) + CheckEvent1-CMIC(TS)\n");
    printf("[CINT] :: TC  8: {PCheck - EgressPort, Equal, Min-yyy} {Action - CounterIncr} {Export - PktCnt} {Flags - ExportModeThreshold, CheckEvent1}\n");
    printf("[CINT] :: TC  8: {IFT, MFT & EFT  data, EFT trigger, Double Wide Mode, ALU16 Export Triggered, PktCount Sent: xxx}\n");

    /* Test Case 9 - Total IFT data (12B), MFT data(1B), EFT data(5B)
     * 008e707032000000 (b3) - EFT - Checker(4B)(ALU32_10) + EgressPort(1B)(LOAD8_6)
     * 2f00000000000000 (b2) - MFT - IngressPort(1B)(LOAD8_4)
     * 0000000100000058 (b1) - IFT - PktCount(4B)(ALU32_0) + ByteCount(4B)(ALU32_1)
     * 3344064000000000 (b0) - IFT - L4SrcPort(2B)(LOAD16_0) + Protocol(1B)(LOAD8_0), TTL(1B)(LOAD8_1)
     */
    printf("[CINT] :: TC  9: { => Resource Config 1} {Key - AT_IT_IPV4 5Tuple (IpProtocol + SrcIpv4 + DstIpv4 + L4SrcPort + L4DstPort}\n");
    printf("[CINT] :: TC  9: {Data - IpProtocol(LOAD8) + PktCount(ALU32) + ByteCount(ALU32) + TTL(LOAD8) + L4Srcport(LOAD16) + \n");
    printf("[CINT] :: TC  9: {       IngressPort(LOAD8) + EgressPort(LOAD8)\n");
    printf("[CINT] :: TC  9: {PCheck - EgressPort, Equal, Min-yyy} {Action - UpdateValue(EgressDelay)} {Export - xxx} {Flags - ExportModeThreshold}\n");
    printf("[CINT] :: TC  9: {IFT, MFT & EFT  data, EFT trigger, Single Wide Mode, ALU32 Export Triggered, PktCount Sent: xxx}\n");

    /* Test Case 10 - Total IFT data (22B)
     * 0000000000000000 (b7) - EFT
     * 0000000000000000 (b6) - MFT
     * 0058556606400000 (b5) - IFT - ByteCount(4B)(ALU32_2)(Last 2B) + L4DstPort(2B)(LOAD16_0) + Protocol(1B)(LOAD8_0), TTL(1B)(LOAD8_1)
     * 0000000000000000 (b4) - IFT
     * 0000000000000000 (b3) - EFT
     * 0000000000000000 (b2) - MFT
     * 0000010a020b0a0a (b1) - IFT - Timestamp(6B)(TS0), Checker(4B)(ALU32_0) (First 2B)
     * 0202000000010000 (b0) - IFT - Checker(4B)(ALU32_0)(Last 2B) + PktCount(4B)(ALU32_1) + ByteCount(4B)(ALU32_2) (First 2B)
     */
    printf("[CINT] :: TC 10: { => Resource Config 1} {Key - AT_IT_IPV4 5Tuple (IpProtocol + SrcIpv4 + DstIpv4 + L4SrcPort + L4DstPort}\n");
    printf("[CINT] :: TC 10: {Data - IpProtocol(LOAD8) + PktCount(ALU32) + ByteCount(ALU32) + TTL(LOAD8) + L4Dstport(LOAD16) + CheckEvent2-PTP(TS)\n");
    printf("[CINT] :: TC 10: {PCheck - IpProtocol, Equal, Min-y} {SCheck - Ttl, Equal, Min-z}\n");
    printf("[CINT] :: TC 10: {         Action - UpdateValue(SrcIpv4)} {Export - w} {Flags - ExportModeThreshold, CheckEvent2}\n");
    printf("[CINT] :: TC 10: {IFT, MFT & EFT  data, IFT trigger, Single Wide Mode, ALU32 Export Triggered, CollectorSrc - PTP, PktCount Sent: x}\n");

    /* Test Case 11 - Total IFT data (19B)
     *  0000000000000000 (b7) - EFT
     *  0000000000000000 (b6) - MFT
     *  5566060000000000 (b5) - IFT - L4DstPort(2B)(ALU16_0), Protocol(1B)(LOAD8_0)
     *  0000000000000000 (b4) - IFT
     *  0000000000000000 (b3) - EFT
     *  0000000000000000 (b2) - MFT
     *  1122334567890000 (b1) - IFT - Timestamp(6B)(TS0), PktCount(4B)(ALU32_0) (First 2B)
     *  0001000000580038 (b0) - IFT - PktCount(4B)(ALU32_0) (Last 2B), ByteCount(4B)(ALU32_1) + IpLength(2B)(LOAD16_0)
     */
    printf("[CINT] :: TC 11: { => Resource Config 1} {Key - AT_IT_IPV4 5Tuple (IpProtocol + SrcIpv4 + DstIpv4 + L4SrcPort + L4DstPort}\n");
    printf("[CINT] :: TC 11: {Data - IpProtocol(LOAD8) + PktCount(ALU32) + ByteCount(ALU32) + IPLength(LOAD16) + CheckEvent3-NTP(TS))\n");
    printf("[CINT] :: TC 11: {PCheck -TcpWindowSize, Equal, Min-y} {SCheck -Ttl, Equal, Min-z}\n");
    printf("[CINT] :: TC 11: {        Action - UpdateValue(L4DstPort)} {Export - w} {Flags - ExportModeThreshold, CheckEvent3}\n");
    printf("[CINT] :: TC 11: {IFT, MFT & EFT  data, IFT trigger, Single Wide Mode, ALU32 Export Triggered, CollectorSrc - NTP, PktCount Sent: x}\n");

    /* Test Case 12 - Total IFT data (30B), MFT data(1B), EFT data(1B)
     * 0101000332000000 (b7) - EFT - IPDT Delay(4B) Last 1B, Checker(ALU16_6) + IngressPort(LOAD8_6)
     * 0000000000000000 (b6) - MFT
     * 0000000000000000 (b5) - IFT
     * 0000000000000000 (b4) - IFT
     * 0000090808060001 (b3) - EFT - Internal TimeStamp(6B)(TS_3) + IPDT Delay(4B) First 1B
     * 2f00000000000000 (b2) - MFT - IngressPort(1B)(LOAD8_4)
     * 0000000300000108 (b1) - IFT - PktCount(4B)(ALU32_0), ByteCount(4B)(ALU32_1)
     * 3344064000000000 (b0) - IFT - L4SrcPort(2B)(ALU16_0), Protocol(1B)(LOAD8_0), TTL(1B)(LOAD8_1)
     */
    printf("[CINT] :: TC 12: { => Resource Config 1} {Key - AT_IT_IPV4 5Tuple (IpProtocol + SrcIpv4 + DstIpv4 + L4SrcPort + L4DstPort}\n");
    printf("[CINT] :: TC 12: {Data - IpProtocol(LOAD8) + PktCount(ALU32) + ByteCount(ALU32) + TTL(LOAD8) + L4Srcport(LOAD16) + \n");
    printf("[CINT] :: TC 12: {       IngressPort(LOAD8) + IPDTDelay(ALU32) + EgressPort(LOAD8)\n");
    printf("[CINT] :: TC 12: {PCheck - EgressPort, Equal, Min-yyy} {Action - CounterIncr} {Export - xxx} {Flags - ExportModeThreshold}\n");
    printf("[CINT] :: TC 12: {IFT, MFT & EFT  data, EFT trigger, Double Wide Mode, ALU16 Export Triggered, PktCount Sent: xxx}\n");

    /* Test Case 13 - Total IFT data (16B), MFT(10B)
     *  0000000000000000 (b7) - EFT
     *  0001000000000000 (b6) - MFT - Check(2B)(ALU16_5)
     *  0000000000000000 (b5) - IFT
     *  0000000000000000 (b4) - IFT
     *  0000000000000000 (b3) - EFT
     *  1002034068890003 (b2) - MFT - Internal TimeStamp(6B)(TS_2), IngressDrop(2B)(LOAD16_8)
     *  0000000100000058 (b1) - IFT - PktCount(4B)(ALU32_0), ByteCount(4B)(ALU32_1)
     *  0038060000000000 (b0) - IFT - IpLength(2B)(LOAD16_0), Protocol(1B)(LOAD8_0)
     */
    printf("[CINT] :: TC 13: { => Resource Config 1} {Key - AT_IT_IPV4 5Tuple (IpProtocol + SrcIpv4 + DstIpv4 + L4SrcPort + L4DstPort}\n");
    printf("[CINT] :: TC 13: {Data - IpProtocol(LOAD8) + PktCount(ALU32) + ByteCount(ALU32) + IPLength(LOAD16) + IngressDropVector(LOAD16) + CheckEvent1-NTP(TS)\n");
    printf("[CINT] :: TC 13: {PCheck - IngressPort, Equal, Min-y} {Action - CounterIncr} {Export - PktCnt} {Flags - ExportModeThreshold, CheckEvent1}\n");
    printf("[CINT] :: TC 13: {IFT, MFT data, MFT trigger, Double Wide Mode, ALU16 Export Triggered, PktCount Sent: x}\n");

    /* Test Case 14 - Total IFT data (16B), MFT(10B)
     *  0000000000000000 (b7) - EFT
     *  0001000000000000 (b6) - MFT - Check(2B)(ALU16_5)
     *  0000000000000000 (b5) - IFT
     *  0000000000000000 (b4) - IFT
     *  0000000000000000 (b3) - EFT
     *  1111222233330003 (b2) - MFT - Internal TimeStamp(6B)(TS_2), IngressDrop(2B)(LOAD16_8)
     *  0000000100000058 (b1) - IFT - PktCount(4B)(ALU32_0), ByteCount(4B)(ALU32_1)
     *  0038060000000000 (b0) - IFT - IpLength(2B)(LOAD16_0), Protocol(1B)(LOAD8_0)
     */
    printf("[CINT] :: TC 14: { => Resource Config 1} {Key - AT_IT_IPV4 5Tuple (IpProtocol + SrcIpv4 + DstIpv4 + L4SrcPort + L4DstPort}\n");
    printf("[CINT] :: TC 14: {Data - IpProtocol(LOAD8) + PktCount(ALU32) + ByteCount(ALU32) + IPLength(LOAD16) + IngressDropVector(LOAD16) + CheckEvent1-NTP(TS)\n");
    printf("[CINT] :: TC 14: {PCheck - IngressDropVector, Mask, Min-y} {Action - CounterIncr} {Export - w} {Flags - ExportModeThreshold, CheckEvent1}\n");
    printf("[CINT] :: TC 14: {IFT, MFT data, MFT trigger, Double Wide Mode, ALU16 Export Triggered, PktCount Sent: x}\n");

    /* Test Case 15 - Total IFT data (21B)
     *  0000000000000000 (b7) - EFT
     *  0000000000000000 (b6) - MFT
     *  0058003806000000 (b5) - IFT -  ByteCount(4B)(ALU32_2) Last 2B, IpLength(2B)(LOAD16_0), Protocol(1B)(LOAD8_0)
     *  0000000000000000 (b4) - IFT
     *  0000000000000000 (b3) - EFT
     *  0000000000000000 (b2) - MFT
     *  0000020203110000 (b1) - IFT - Internal TimeStamp(6B)(TS_0), Check(4B)(ALU32_0) First 2B
     *  0556000000010000 (b0) - IFT - Check(4B)(ALU32_0) Last 2B, PktCount(4B)(ALU32_1), ByteCount(4B)(ALU32_2) First 2B
     */
    printf("[CINT] :: TC 15: { => Resource Config 1} {Key - AT_IT_IPV4 5Tuple (IpProtocol + SrcIpv4 + DstIpv4 + L4SrcPort + L4DstPort}\n");
    printf("[CINT] :: TC 15: {Data - IpProtocol(LOAD8) + PktCount(ALU32) + ByteCount(ALU32) + IPLength(LOAD16) + CheckEvent1-CMIC(TS)\n");
    printf("[CINT] :: TC 15: {PCheck - TcpWindowSize, Equal, ParamMask & Shift, Min-y} {SCheck - DstIpv4, Equal, ParamMask & Shift, Min-x\n");
    printf("[CINT] :: TC 15: {         Action - UpdateValue(L4DstPort), ParamMask & Shift} {Export - xxx} {Flags - ExportModeThreshold, CheckEvent1}\n");
    printf("[CINT] :: TC 15: {IFT data, IFT trigger, Single Wide Mode, ALU32 Export Triggered, PktCount Sent: x}\n");

    /* Test Case 16 - Total IFT data (16B), EFT (10B)
     *  0001000000000000 (b7) - EFT - Check(2B)(ALU16_6)
     *  0000000000000000 (b6) - MFT
     *  0000000000000000 (b5) - IFT
     *  0000000000000000 (b4) - IFT
     *  0505030304040001 (b3) - EFT - Internal TimeStamp(6B)(TS_3), EgressDrop(2B)(LOAD16_12)
     *  0000000000000000 (b2) - MFT
     *  0000000100000058 (b1) - IFT - PktCount(4B)(ALU32_0), ByteCount(4B)(ALU32_1)
     *  0038060000000000 (b0) - IFT - IpLength(2B)(LOAD16_0), Protocol(1B)(LOAD8_0)
     */
    printf("[CINT] :: TC 16: { => Resource Config 1} {Key - AT_IT_IPV4 5Tuple (IpProtocol + SrcIpv4 + DstIpv4 + L4SrcPort + L4DstPort}\n");
    printf("[CINT] :: TC 16: {Data - IpProtocol(LOAD8) + PktCount(ALU32) + ByteCount(ALU32) + IPLength(LOAD16) + EgressDropVector(LOAD16) + CheckEvent1-PTP(TS)\n");
    printf("[CINT] :: TC 16: {PCheck - EgressPort, Equal, Min-y} {Action - CounterIncr} {Export - PktCnt} {Flags - ExportModeThreshold, CheckEvent1}\n");
    printf("[CINT] :: TC 16: {IFT, MFT & EFT data, EFT trigger, Double Wide Mode, ALU16 Export Triggered, PktCount Sent: x}\n");

    /* Test Case 17 - Total IFT data (16B), EFT (10B)
     *  0001000000000000 (b7) - EFT - Check(2B)(ALU16_6)
     *  0000000000000000 (b6) - MFT
     *  0000000000000000 (b5) - IFT
     *  0000000000000000 (b4) - IFT
     *  0550300340040001 (b3) - EFT - Internal TimeStamp(6B)(TS_3), EgressDrop(2B)(LOAD16_12)
     *  0000000000000000 (b2) - MFT
     *  0000000100000058 (b1) - IFT - PktCount(4B)(ALU32_0), ByteCount(4B)(ALU32_1)
     *  0038060000000000 (b0) - IFT - IpLength(2B)(LOAD16_0), Protocol(1B)(LOAD8_0)
     */
    printf("[CINT] :: TC 17: { => Resource Config 1} {Key - AT_IT_IPV4 5Tuple (IpProtocol + SrcIpv4 + DstIpv4 + L4SrcPort + L4DstPort}\n");
    printf("[CINT] :: TC 17: {Data - IpProtocol(LOAD8) + PktCount(ALU32) + ByteCount(ALU32) + IPLength(LOAD16) + IngressDropVector(LOAD16) + CheckEvent1-PTP(TS)\n");
    printf("[CINT] :: TC 17: {PCheck - EgressDropVector, Mask, Min-y} {Action - CounterIncr} {Export - w} {Flags - ExportModeThreshold, CheckEvent1}\n");
    printf("[CINT] :: TC 17: {IFT, MFT & EFT data, EFT trigger, Double Wide Mode, ALU16 Export Triggered, PktCount Sent: x}\n");

    printf("[CINT] :: -----------------------------------------------------------------------------------------------------------------------\n");
    printf("[CINT] :: Make sure resource config variables are set in device as per config mentioned in testcase description.\n");
    printf("[CINT] :: Enable/Disable sim for test run by setting bcmsim_ftv2_enabled. By default it is set to 1, means sim run is enabled\n");
    printf("[CINT] :: Use test_run(<tc_id>) to execute relevant test case [eg. cint->test_run(1)]\n");
    printf("[CINT] :: Use test_cleanup(<tc_id>) to cleanup configurations done in test case [eg. cint->test_cleanup(1)]\n");
    printf("[CINT] :: Use test_run_range(<start_tc_id>, <end_tc_id>) to execute relevant test cases [eg. cint->test_run_range(1,5)]\n");
    printf("[CINT] :: Use debug_set to set debug level [eg. cint->debug_set(0xFF) ]\n");
    printf("[CINT] :: Debug Levels 0x0001 - Dump Relevant Memory & Registers after every step execution.\n");
    printf("[CINT] ::              0x0002 - Dump Diag Shell commands.\n");
    printf("[CINT] ::              0x0004 - Use RxMon Task to see packet, instead of inbuilt rx callback utility.\n");
    printf("[CINT] ::              0x0008 - Dont do cleanup after test execution.\n");
    printf("[CINT] ::              0x0010 - Ignore all detailed success logs and print only summarised PASS or FAIL.\n");
    return 0;
}

/* Set Debug level of system */
void
debug_set(uint32 debug_value)
{
    debug_flags = debug_value;

    if (debug_flags & 0x0001) {
        dump_mem_reg = 1;
    } else {
        dump_mem_reg = 0;
    }

    if (debug_flags & 0x0002) {
        dump_diag_shell = 1;
    } else {
        dump_diag_shell = 0;
    }

    if (debug_flags & 0x0004) {
        use_ftrmon_thread = 1;
    } else {
        use_ftrmon_thread = 0;
    }

    if (debug_flags & 0x0008) {
        skip_cleanup = 1;
    } else {
        skip_cleanup = 0;
    }

    if (debug_flags & 0x0010) {
        skip_log = 1;
    } else {
        skip_log = 0;
    }

    printf("[DEBUG_SET]: Debug Value Set to: 0x%04x\n",debug_value);
}

/* Valid Test Case Function Check */
int
test_valid(int tc_id)
{
    /* Disable all configs by default */
    ftv2_param_sanity_test_config = 0;
    ftv2_ftfp_presel_test_config = 0;
    ftv2_ftfp_group_test_config = 0;
    ftv2_collector_test_config = 0;
    ftv2_group_test_config = 0;
    ftv2_group_flow_limit_test_config = 0;
    ftv2_group_ageing_timer_test_config = 0;
    ftv2_group_collector_copy_test_config = 0;
    ftv2_group_cpu_notification_test_config = 0;
    ftv2_group_control_test_config = 0;
    ftv2_group_meter_test_config = 0;
    ftv2_drop_test_config = 0;
    ftv2_group_flow_checker_test_config = 0;
    ftv2_group_tracking_param_test_config = 0;
    ftv2_group_export_template_test_config = 0;
    ftv2_group_export_trigger_test_config = 0;
    ftv2_ftfp_group_entry_test_config = 0;
    ftv2_system_time_test_config = 0;
    ftv2_tx_test_config = 0;
    ftv2_validate_test_config = 0;

    /* Enable per test case as per requirement */
    /* Remember Me - Add new test case here */
    switch(tc_id) {
        case 1:
            /* Test Case 1 */
            ftv2_param_sanity_test_config = 1;
            break;
        case 2:
            /* Test Case 2 */
            ftv2_ftfp_presel_test_config = 1;
            ftv2_ftfp_group_test_config = 1;
            ftv2_collector_test_config = 1;
            ftv2_group_test_config = 1;
            ftv2_group_flow_limit_test_config = 1;
            ftv2_group_flow_checker_test_config = 2;
            ftv2_group_tracking_param_test_config = 2;
            ftv2_group_export_template_test_config = 1;
            ftv2_ftfp_group_entry_test_config = 1;
            ftv2_tx_test_config = 2;
            ftv2_validate_test_config = 1;
            break;
        case 3:
            /* Test Case 3 */
            ftv2_ftfp_presel_test_config = 1;
            ftv2_ftfp_group_test_config = 1;
            ftv2_collector_test_config = 1;
            ftv2_group_test_config = 1;
            ftv2_group_flow_limit_test_config = 1;
            ftv2_group_flow_checker_test_config = 3;
            ftv2_group_tracking_param_test_config = 3;
            ftv2_group_export_template_test_config = 1;
            ftv2_ftfp_group_entry_test_config = 1;
            ftv2_system_time_test_config = 3;
            ftv2_tx_test_config = 3;
            ftv2_validate_test_config = 1;
            break;
        case 4:
            /* Test Case 4 */
            ftv2_ftfp_presel_test_config = 1;
            ftv2_ftfp_group_test_config = 1;
            ftv2_collector_test_config = 1;
            ftv2_group_test_config = 1;
            ftv2_group_flow_limit_test_config = 1;
            ftv2_group_flow_checker_test_config = 4;
            ftv2_group_tracking_param_test_config = 4;
            ftv2_group_export_template_test_config = 1;
            ftv2_ftfp_group_entry_test_config = 1;
            ftv2_system_time_test_config = 4;
            ftv2_tx_test_config = 4;
            ftv2_validate_test_config = 1;
            break;
        case 5:
            /* Test Case 5 */
            ftv2_ftfp_presel_test_config = 1;
            ftv2_ftfp_group_test_config = 1;
            ftv2_collector_test_config = 1;
            ftv2_group_test_config = 1;
            ftv2_group_flow_limit_test_config = 1;
            ftv2_group_flow_checker_test_config = 5;
            ftv2_group_tracking_param_test_config = 5;
            ftv2_group_export_template_test_config = 1;
            ftv2_ftfp_group_entry_test_config = 1;
            ftv2_system_time_test_config = 5;
            ftv2_tx_test_config = 5;
            ftv2_validate_test_config = 1;
            break;
        case 6:
            /* Test Case 6 */
            ftv2_ftfp_presel_test_config = 1;
            ftv2_ftfp_group_test_config = 1;
            ftv2_collector_test_config = 1;
            ftv2_group_test_config = 1;
            ftv2_group_flow_limit_test_config = 1;
            ftv2_group_flow_checker_test_config = 6;
            ftv2_group_tracking_param_test_config = 6;
            ftv2_group_export_template_test_config = 1;
            ftv2_ftfp_group_entry_test_config = 1;
            ftv2_tx_test_config = 6;
            ftv2_validate_test_config = 1;
            break;
        case 7:
            /* Test Case 7 */
            ftv2_ftfp_presel_test_config = 1;
            ftv2_ftfp_group_test_config = 1;
            ftv2_collector_test_config = 1;
            ftv2_group_test_config = 1;
            ftv2_group_flow_limit_test_config = 1;
            ftv2_group_flow_checker_test_config = 7;
            ftv2_group_tracking_param_test_config = 7;
            ftv2_group_export_template_test_config = 1;
            ftv2_ftfp_group_entry_test_config = 1;
            ftv2_system_time_test_config = 7;
            ftv2_tx_test_config = 7;
            ftv2_validate_test_config = 1;
            break;
        case 8:
            /* Test Case 8 */
            ftv2_ftfp_presel_test_config = 1;
            ftv2_ftfp_group_test_config = 1;
            ftv2_collector_test_config = 1;
            ftv2_group_test_config = 1;
            ftv2_group_flow_limit_test_config = 1;
            ftv2_group_flow_checker_test_config = 8;
            ftv2_group_tracking_param_test_config = 8;
            ftv2_group_export_template_test_config = 1;
            ftv2_ftfp_group_entry_test_config = 1;
            ftv2_system_time_test_config = 8;
            ftv2_tx_test_config = 8;
            ftv2_validate_test_config = 1;
            break;
        case 9:
            /* Test Case 9 */
            ftv2_ftfp_presel_test_config = 1;
            ftv2_ftfp_group_test_config = 1;
            ftv2_collector_test_config = 1;
            ftv2_group_test_config = 1;
            ftv2_group_flow_limit_test_config = 1;
            ftv2_group_flow_checker_test_config = 9;
            ftv2_group_tracking_param_test_config = 9;
            ftv2_group_export_template_test_config = 1;
            ftv2_ftfp_group_entry_test_config = 1;
            ftv2_system_time_test_config = 9;
            ftv2_tx_test_config = 9;
            ftv2_validate_test_config = 1;
            break;
        case 10:
            /* Test Case 10 */
            ftv2_ftfp_presel_test_config = 1;
            ftv2_ftfp_group_test_config = 1;
            ftv2_collector_test_config = 10;
            ftv2_group_test_config = 1;
            ftv2_group_flow_limit_test_config = 1;
            ftv2_group_flow_checker_test_config = 10;
            ftv2_group_tracking_param_test_config = 10;
            ftv2_group_export_template_test_config = 1;
            ftv2_ftfp_group_entry_test_config = 1;
            ftv2_system_time_test_config = 10;
            ftv2_tx_test_config = 10;
            ftv2_validate_test_config = 1;
            break;
        case 11:
            /* Test Case 11 */
            ftv2_ftfp_presel_test_config = 1;
            ftv2_ftfp_group_test_config = 1;
            ftv2_collector_test_config = 11;
            ftv2_group_test_config = 1;
            ftv2_group_flow_limit_test_config = 1;
            ftv2_group_flow_checker_test_config = 11;
            ftv2_group_tracking_param_test_config = 11;
            ftv2_group_export_template_test_config = 1;
            ftv2_ftfp_group_entry_test_config = 1;
            ftv2_system_time_test_config = 11;
            ftv2_tx_test_config = 11;
            ftv2_validate_test_config = 1;
            break;
        case 12:
            /* Test Case 12 */
            ftv2_ftfp_presel_test_config = 1;
            ftv2_ftfp_group_test_config = 1;
            ftv2_collector_test_config = 1;
            ftv2_group_test_config = 1;
            ftv2_group_flow_limit_test_config = 1;
            ftv2_group_flow_checker_test_config = 12;
            ftv2_group_tracking_param_test_config = 12;
            ftv2_group_export_template_test_config = 1;
            ftv2_ftfp_group_entry_test_config = 1;
            ftv2_system_time_test_config = 12;
            ftv2_tx_test_config = 12;
            ftv2_validate_test_config = 1;
            break;
        case 13:
            /* Test Case 13 */
            ftv2_ftfp_presel_test_config = 1;
            ftv2_ftfp_group_test_config = 1;
            ftv2_collector_test_config = 1;
            ftv2_group_test_config = 1;
            ftv2_group_flow_limit_test_config = 1;
            ftv2_drop_test_config = 13;
            ftv2_group_flow_checker_test_config = 13;
            ftv2_group_tracking_param_test_config = 13;
            ftv2_group_export_template_test_config = 1;
            ftv2_ftfp_group_entry_test_config = 1;
            ftv2_system_time_test_config = 13;
            ftv2_tx_test_config = 13;
            ftv2_validate_test_config = 1;
            break;
        case 14:
            /* Test Case 14 */
            ftv2_ftfp_presel_test_config = 1;
            ftv2_ftfp_group_test_config = 1;
            ftv2_collector_test_config = 1;
            ftv2_group_test_config = 1;
            ftv2_group_flow_limit_test_config = 1;
            ftv2_drop_test_config = 14;
            ftv2_group_flow_checker_test_config = 14;
            ftv2_group_tracking_param_test_config = 14;
            ftv2_group_export_template_test_config = 1;
            ftv2_ftfp_group_entry_test_config = 1;
            ftv2_system_time_test_config = 14;
            ftv2_tx_test_config = 14;
            ftv2_validate_test_config = 1;
            break;
        case 15:
            /* Test Case 15 */
            ftv2_ftfp_presel_test_config = 1;
            ftv2_ftfp_group_test_config = 1;
            ftv2_collector_test_config = 1;
            ftv2_group_test_config = 1;
            ftv2_group_flow_limit_test_config = 1;
            ftv2_group_flow_checker_test_config = 15;
            ftv2_group_tracking_param_test_config = 15;
            ftv2_group_export_template_test_config = 1;
            ftv2_ftfp_group_entry_test_config = 1;
            ftv2_system_time_test_config = 15;
            ftv2_tx_test_config = 15;
            ftv2_validate_test_config = 1;
            break;
        case 16:
            /* Test Case 16 */
            ftv2_ftfp_presel_test_config = 1;
            ftv2_ftfp_group_test_config = 1;
            ftv2_collector_test_config = 1;
            ftv2_group_test_config = 1;
            ftv2_group_flow_limit_test_config = 1;
            ftv2_drop_test_config = 16;
            ftv2_group_flow_checker_test_config = 16;
            ftv2_group_tracking_param_test_config = 16;
            ftv2_group_export_template_test_config = 1;
            ftv2_ftfp_group_entry_test_config = 1;
            ftv2_system_time_test_config = 16;
            ftv2_tx_test_config = 16;
            ftv2_validate_test_config = 1;
            break;
        case 17:
            /* Test Case 17 */
            ftv2_ftfp_presel_test_config = 1;
            ftv2_ftfp_group_test_config = 1;
            ftv2_collector_test_config = 1;
            ftv2_group_test_config = 1;
            ftv2_group_flow_limit_test_config = 1;
            ftv2_drop_test_config = 17;
            ftv2_group_flow_checker_test_config = 17;
            ftv2_group_tracking_param_test_config = 17;
            ftv2_group_export_template_test_config = 1;
            ftv2_ftfp_group_entry_test_config = 1;
            ftv2_system_time_test_config = 17;
            ftv2_tx_test_config = 17;
            ftv2_validate_test_config = 1;
            break;
        default:
            return FALSE;
    }

    return TRUE;
}

/* Flow tracker test setup */
int
ftv2_test_setup(uint32 tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;

    /* Check param sanity of api's */
    rv = ftv2_param_sanity_test_main(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d failed at presetup step of param sanity, terminating now ....\n",tc_id);
        return rv;
    }

    /* Step 1 - Create Presel FTFP entries. */
    rv = ftv2_fp_presel_create_main(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d failed at presel create step, terminating now ....\n",tc_id);
        return rv;
    }

    /* Step 2 - Create FTFP Group with qualifying criteria. */
    rv = ftv2_fp_group_create_main(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d failed at FTFP Group create step, terminating now ....\n",tc_id);
        return rv;
    }

    /* Step 3 - Create FT Collector. */
    rv = ftv2_collector_create_main(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d failed at collector create step, terminating now ....\n",tc_id);
        return rv;
    }

    /* Step 4 - Create FT Group */
    rv = ftv2_group_create_main(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d failed at ft group create step, terminating now ....\n",tc_id);
        return rv;
    }

    /* Step 5 - Set Flow Limit */
    rv = ftv2_group_flow_limit_set(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d failed at ft group flow limit set step, terminating now ....\n",tc_id);
        return rv;
    }

    /* Step 6 - Set FT Group Age Timer */
    rv = ftv2_group_age_timer_set(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d failed at ft group age timer step, terminating now ....\n",tc_id);
        return rv;
    }

    /* Step 7 - Set Collector Copy Info */
    rv = ftv2_group_collector_copy_info_set(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d failed at ft group collector copy info step, terminating now ....\n",tc_id);
        return rv;
    }

    /* Step 8 - Set CPU Notification */
    rv = ftv2_group_cpu_notification_set(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d failed at ft group cpu notification step, terminating now ....\n",tc_id);
        return rv;
    }

    /* Step 9 - Set Group Control */
    rv = ftv2_group_control_set(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d failed at ft group control step, terminating now ....\n",tc_id);
        return rv;
    }

    /* Step 10 - Set Meter Profile */
    rv = ftv2_group_meter_set(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d failed at ft group meter step, terminating now ....\n",tc_id);
        return rv;
    }

    /* Step 11 - Set Drop Reason for checker */
    rv = ftv2_drop_reason_group_create_main(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d failed at drop reason group create step, terminating now ....\n",tc_id);
        return rv;
    }

    /* Step 13 - Set Check Info */
    rv = ftv2_check_create_main(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d failed at check create step, terminating now ....\n",tc_id);
        return rv;
    }

    /* Step 14 - Set Tracking params */
    rv = ftv2_group_tracking_params_set(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d failed at ft group tracking param set step, terminating now ....\n",tc_id);
        return rv;
    }

    /* Step 15 - Create Export Template and add collector, template to group */
    rv = ftv2_group_export_template_collector_add(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d failed at template & collector create & add step, terminating now ....\n",tc_id);
        return rv;
    }

    /* Step 16 - Set Export Trigger */
    rv = ftv2_export_trigger_main(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d failed at export trigger step, terminating now ....\n",tc_id);
        return rv;
    }

    /* Step 17 - Install FTFP Group Entry */
    rv = ftv2_fp_group_entry_install_main(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d failed at ftfp entry install step, terminating now ....\n",tc_id);
        return rv;
    }

    /* Step 18 - Set Time in system */
    rv = ftv2_system_time_set_main(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d failed at system time set step, terminating now ....\n",tc_id);
        return rv;
    }

    if (!skip_log) {
        printf("[CINT] Config - TestCase Config Setup Completed Succesfully !!!!\n");
    }

    return BCM_E_NONE;
}

/* Flow tracker Validate */
int
ftv2_test_validate(uint32 tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;
    char command[200];

    /* Skip this validate if no test config set */
    if (!ftv2_validate_test_config) {
        if (!skip_log) {
            printf("[CINT] Validation - Skip validate calls ......\n");
        }
        return rv;
    }

    /* Validate with FT Group Counter Get */
    switch(ftv2_validate_test_config) {
        /* ftv2_validate_test_config = 1 */
        case 1:
            /* Validate with group stat */
            rv = ftv2_group_stat_get(tc_id, debug_flags);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Validation - TC %2d  - ftv2_group_stat_get failed with rv %d(%s)\n",
                        tc_id, rv, bcm_errmsg(rv));
                return rv;
            }

            /* Validate with IPFIX stats */
            rv = ftv2_group_ipfix_stat_get(tc_id, debug_flags);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Validation - TC %2d  - ftv2_group_ipfix_stat_get failed with rv %d(%s)\n",
                        tc_id, rv, bcm_errmsg(rv));
                return rv;
            }

            /* Validate Export Reason */
            rv = ftv2_group_export_reason_get(tc_id, debug_flags);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Validation - TC %2d  - ftv2_group_export_reason_get failed with rv %d(%s)\n",
                        tc_id, rv, bcm_errmsg(rv));
                return rv;
            }

            /* Validate packet watcher parsing */
            rv = ftv2_group_pw_validation(tc_id, debug_flags);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] Validation - TC %2d  - ftv2_group_pw_validation failed with rv %d(%s)\n",
                        tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            break;

        default:
            printf("[CINT] TC %2d - invalid validate test config %d\n",tc_id, ftv2_validate_test_config);
            return BCM_E_FAIL;
    }

    /* Final Step Log */
    if (!skip_log) {
        printf("[CINT] Validation - Test Validations Completed Succesfully !!!!\n");
    }

    return BCM_E_NONE;
}


/* Flow tracker Cleanup */
int
ftv2_test_cleanup(uint32 tc_id, uint32 debug_flags)
{
    int i = 0;
    int rv = BCM_E_NONE;

    /* Skip this cleanup */
    if (skip_cleanup) {
        if (!skip_log) {
            printf("[CINT] Cleanup - Skip Test Config Cleanup ......\n");
        }
        return rv;
    }

    /* Some extra sleep incase packet is not handled by watcher yet. */
    if (!packet_handled_by_watcher) {
        if (!skip_log) {
            printf("[CINT] Monitoring - Packet not yet handled by watcher !.\n");
        }
        for (i = 0; i < 3; i++) {
            if (!skip_log) {
                printf("[CINT] Monitoring - iter %d => Sleeping for 30 seconds ..\n",(i+1));
            }
            sal_sleep(30);
            if (packet_handled_by_watcher) {
                break;
            }
        }
    } else {
        if (!skip_log) {
            printf("[CINT] Monitoring - Packet handled by watcher !.\n");
        }
    }
    packet_handled_by_watcher = 0;

    /* Destroy FTFP Group Entries */
    rv = ftv2_fp_group_entry_destroy_main(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - ftv2_fp_group_entry_destroy_main failed with rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
        return rv;
    }

    /* Destroy FTFP Group */
    rv = ftv2_fp_group_destroy_main(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - ftv2_fp_group_destroy_main failed with rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
        return rv;
    }

    /* Destroy FTFP Presel Entries */
    rv = ftv2_fp_presel_destroy_main(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - ftv2_fp_presel_destroy_main failed with rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
        return rv;
    }

    /* Clear all flows in FT Group */
    rv = ftv2_group_flows_clear(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - ftv2_group_flows_clear failed with rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
        return rv;
    }

    /* Delete FT Collector */
    rv = ftv2_collector_destroy_main(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - ftv2_collector_destroy_main failed with rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
        return rv;
    }

    /* Delete Export Template */
    rv = ftv2_group_export_template_collector_destroy(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - ftv2_group_export_template_collector_destroy failed with rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
        return rv;
    }

    /* Destroy Flow Checkers */
    rv = ftv2_check_destroy_main(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - ftv2_group_check_destroy failed with rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
        return rv;
    }

    /* Destroy Drop Reason group of checker */
    rv = ftv2_drop_reason_group_destroy_main(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - ftv2_drop_reason_group_destroy_main failed with rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
        return rv;
    }

    /* Destroy FT Group */
    rv = ftv2_group_destroy_main(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - ftv2_group_destroy_main failed with rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
        return rv;
    }

    /* Destroy System time setting */
    rv = ftv2_system_time_destroy_main(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - ftv2_system_time_destroy_main failed with rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
        return rv;
    }

    /* Final Step Log */
    if (!skip_log) {
        printf("[CINT] Cleanup - Config Cleanup Successfully Completed !!!!!\n");
    }

    return BCM_E_NONE;
}

/* Run TestMenu once when C file is loaded */
testMenu();

/* Run Test case logic */
int
test_run(int tc_id)
{
    int rv = BCM_E_NONE;

    /* Check if test case is valid */
    if (!test_valid(tc_id)) {
        printf("[CINT] Invalid Test Case Id %d\n",tc_id);
        printf("[CINT] Choose from valid test case list ..\n");
        testMenu();
        return rv;
    }

    /* Check if test case run is already in progress. */
    if (tc_in_progress) {
        printf("[CINT] Testcase %d is already in progress, do cleanup before executing next ..\n",tc_in_progress);
        return BCM_E_BUSY;
    }
    tc_in_progress = tc_id;

    /* Flow Tracker Resource Allocation */
    rv = ftv2_test_resource_config_alloc(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d failed while dumping resource config, terminating now ..\n",tc_id);
        return rv;
    }

    /* Start Monitoring Task if needed */
    rv = ftv2_test_start_monitoring(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d failed while starting monitoring, terminating now ..\n",tc_id);
        return rv;
    }

    /* Setup packet path */
    rv = ftv2_pkt_path_setup(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d failed while setting up packet path, terminating now ..\n",tc_id);
        return rv;
    }

    /* Setup Flow Tracker test */
    rv = ftv2_test_setup(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d failed while setting up test, terminating now ..\n",tc_id);
        return rv;
    }

    /* Send Traffic */
    rv = ftv2_test_send_traffic(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d failed while sending traffic, terminating now ..\n",tc_id);
        return rv;
    }

    /* Validate if Flow Tracking has happened */
    rv = ftv2_test_validate(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d failed at validation step, terminating now ..\n",tc_id);
        return rv;
    }

    /* Cleanup Configurations Done */
    rv = ftv2_test_cleanup(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d failed at cleanup step, terminating now ..\n",tc_id);
        return rv;
    }

    /* Cleanup traffix setting done */
    rv = ftv2_test_cleanup_traffic(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d failed at traffic setting cleanup, terminating now ..\n",tc_id);
        return rv;
    }

    /* Destroy packet path setup */
    rv = ftv2_pkt_path_destroy(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d failed at packet path destroy step, terminating now ..\n",tc_id);
        return rv;
    }

    /* Stop Monitoring Task if needed */
    rv = ftv2_test_stop_monitoring(tc_id, debug_flags);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d failed while stopping monitoring, terminating now ..\n",tc_id);
        return rv;
    }

    /* Clear test case progress if cleanup has happened. */
    if (!skip_cleanup) {
        tc_in_progress = 0;
    }

    /* Final Step Log */
    printf("[CINT] ************** TestCase %d has PASSED **************\n",tc_id);

    return BCM_E_NONE;
}

int
test_cleanup(int tc_id)
{
    int rv = BCM_E_NONE;
    uint8 prev_skip_cleanup = 0;

    /* Store previous cleanup state. */
    prev_skip_cleanup = skip_cleanup;

    /* Clear cleanup skip temporarily. */
    skip_cleanup = 0;

    /* Execute cleanup if test case is in progress. */
    if (tc_in_progress) {

        /* Check if cleanup is done for same test case for which tc_in_progress is set. */
        if (tc_in_progress != tc_id) {
            printf("[CINT] Testcase in progress is %d, it only can be cleaned up.\n",tc_in_progress);
            return BCM_E_UNAVAIL;
        }

        /* Cleanup Configurations Done */
        rv = ftv2_test_cleanup(tc_id, debug_flags);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] TC %2d failed at cleanup step, terminating now ..\n",tc_id);
            return rv;
        }

        /* Cleanup traffix setting done */
        rv = ftv2_test_cleanup_traffic(tc_id, debug_flags);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] TC %2d failed at traffic setting cleanup, terminating now ..\n",tc_id);
            return rv;
        }

        /* Destroy packet path setup */
        rv = ftv2_pkt_path_destroy(tc_id, debug_flags);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] TC %2d failed at packet path destroy step, terminating now ..\n",tc_id);
            return rv;
        }

        /* Stop Monitoring Task if needed */
        rv = ftv2_test_stop_monitoring(tc_id, debug_flags);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] TC %2d failed while stopping monitoring, terminating now ..\n",tc_id);
            return rv;
        }

        /* Final Step Log */
        skip_cleanup = prev_skip_cleanup;
        tc_in_progress = 0;
        printf("[CINT] ************** TestCase %d has been cleanedup **************\n",tc_id);
    }

    return BCM_E_NONE;
}

/* Run Test case Range */
int
test_run_range(int start_tc_id, int end_tc_id)
{
    int rv = BCM_E_NONE, tc_id_tmp = 0;

    /* Validate start and end testcase id. */
    if ((start_tc_id < 0) || (end_tc_id < 0) ||
        (start_tc_id > end_tc_id)) {
        printf("[CINT] Invalid Test Range Input !\n");
        return BCM_E_PARAM;
    }

    /* Validate cleanup settings. */
    if (skip_cleanup) {
        printf("[CINT] Cleanup cannot be skipped in test run range call.\n");
    }

    /* Execute all test cases one by one */
    for (tc_id_tmp = start_tc_id; tc_id_tmp <= end_tc_id; tc_id_tmp++) {

        /* Check if test case is valid */
        if (!test_valid(tc_id_tmp)) {
            continue;
        }

        /* Run testcase */
        rv = test_run(tc_id_tmp);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] TC %2d failed with rv %d(%s), terminating now ..\n",tc_id_tmp,rv,bcm_errmsg(rv));
            return rv;
        }
    }

    return BCM_E_NONE;
}
