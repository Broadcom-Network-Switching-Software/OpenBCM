/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_STAT_H__
#define __BCM_STAT_H__

#include <bcm/types.h>

/* 
 * Statistics counters.
 * 
 * Note: the following RFC 2465 MIBs are not supported:
 * 
 *   snmpIpv6IfStatsInTooBigErrors,
 *   snmpIpv6IfStatsInNoRoutes,
 *   snmpIpv6IfStatsInUnknownProtos,
 *   snmpIpv6IfStatsInTruncatedPkts,
 *   snmpIpv6IfStatsInDelivers,
 *   snmpIpv6IfStatsOutRequests,
 *   snmpIpv6IfStatsOutFragOKs,
 *   snmpIpv6IfStatsOutFragFails,
 *   snmpIpv6IfStatsOutFragCreates,
 *   snmpIpv6IfStatsReasmReqds,
 *   snmpIpv6IfStatsReasmOKs,
 *   snmpIpv6IfStatsReasmFails,
 */
typedef enum bcm_stat_val_e {
    snmpIfInOctets = 0,                 /* RFC 1213 */
    snmpIfInUcastPkts = 1,              /* RFC 1213 */
    snmpIfInNUcastPkts = 2,             /* RFC 1213 */
    snmpIfInDiscards = 3,               /* RFC 1213 */
    snmpIfInErrors = 4,                 /* RFC 1213 */
    snmpIfInUnknownProtos = 5,          /* RFC 1213 */
    snmpIfOutOctets = 6,                /* RFC 1213 */
    snmpIfOutUcastPkts = 7,             /* RFC 1213 */
    snmpIfOutNUcastPkts = 8,            /* RFC 1213 */
    snmpIfOutDiscards = 9,              /* RFC 1213 */
    snmpIfOutErrors = 10,               /* RFC 1213 */
    snmpIfOutQLen = 11,                 /* RFC 1213 */
    snmpIpInReceives = 12,              /* RFC 1213 */
    snmpIpInHdrErrors = 13,             /* RFC 1213 */
    snmpIpForwDatagrams = 14,           /* RFC 1213 */
    snmpIpInDiscards = 15,              /* RFC 1213 */
    snmpDot1dBasePortDelayExceededDiscards = 16, /* RFC 1493 */
    snmpDot1dBasePortMtuExceededDiscards = 17, /* RFC 1493 */
    snmpDot1dTpPortInFrames = 18,       /* RFC 1493 */
    snmpDot1dTpPortOutFrames = 19,      /* RFC 1493 */
    snmpDot1dPortInDiscards = 20,       /* RFC 1493 */
    snmpEtherStatsDropEvents = 21,      /* RFC 1757 (EtherStat) */
    snmpEtherStatsMulticastPkts = 22,   /* RFC 1757 (EtherStat) */
    snmpEtherStatsBroadcastPkts = 23,   /* RFC 1757 (EtherStat) */
    snmpEtherStatsUndersizePkts = 24,   /* RFC 1757 (EtherStat) */
    snmpEtherStatsFragments = 25,       /* RFC 1757 (EtherStat) */
    snmpEtherStatsPkts64Octets = 26,    /* RFC 1757 (EtherStat) */
    snmpEtherStatsPkts65to127Octets = 27, /* RFC 1757 (EtherStat) */
    snmpEtherStatsPkts128to255Octets = 28, /* RFC 1757 (EtherStat) */
    snmpEtherStatsPkts256to511Octets = 29, /* RFC 1757 (EtherStat) */
    snmpEtherStatsPkts512to1023Octets = 30, /* RFC 1757 (EtherStat) */
    snmpEtherStatsPkts1024to1518Octets = 31, /* RFC 1757 (EtherStat) */
    snmpEtherStatsOversizePkts = 32,    /* RFC 1757 (EtherStat) */
    snmpEtherRxOversizePkts = 33,       /* Rx component of
                                           snmpEtherStatsOversizePkts */
    snmpEtherTxOversizePkts = 34,       /* Tx component of
                                           snmpEtherStatsOversizePkts */
    snmpEtherStatsJabbers = 35,         /* RFC 1757 (EtherStat) */
    snmpEtherStatsOctets = 36,          /* RFC 1757 (EtherStat) */
    snmpEtherStatsPkts = 37,            /* RFC 1757 (EtherStat) */
    snmpEtherStatsCollisions = 38,      /* RFC 1757 (EtherStat) */
    snmpEtherStatsCRCAlignErrors = 39,  /* RFC 1757 (EtherStat) */
    snmpEtherStatsTXNoErrors = 40,      /* RFC 1757 (EtherStat) */
    snmpEtherStatsRXNoErrors = 41,      /* RFC 1757 (EtherStat) */
    snmpDot3StatsAlignmentErrors = 42,  /* RFC 2665 */
    snmpDot3StatsFCSErrors = 43,        /* RFC 2665 */
    snmpDot3StatsSingleCollisionFrames = 44, /* RFC 2665 */
    snmpDot3StatsMultipleCollisionFrames = 45, /* RFC 2665 */
    snmpDot3StatsSQETTestErrors = 46,   /* RFC 2665 */
    snmpDot3StatsDeferredTransmissions = 47, /* RFC 2665 */
    snmpDot3StatsLateCollisions = 48,   /* RFC 2665 */
    snmpDot3StatsExcessiveCollisions = 49, /* RFC 2665 */
    snmpDot3StatsInternalMacTransmitErrors = 50, /* RFC 2665 */
    snmpDot3StatsCarrierSenseErrors = 51, /* RFC 2665 */
    snmpDot3StatsFrameTooLongs = 52,    /* RFC 2665 */
    snmpDot3StatsInternalMacReceiveErrors = 53, /* RFC 2665 */
    snmpDot3StatsSymbolErrors = 54,     /* RFC 2665 */
    snmpDot3ControlInUnknownOpcodes = 55, /* RFC 2665 */
    snmpDot3InPauseFrames = 56,         /* RFC 2665 */
    snmpDot3OutPauseFrames = 57,        /* RFC 2665 */
    snmpIfHCInOctets = 58,              /* RFC 2233 */
    snmpIfHCInUcastPkts = 59,           /* RFC 2233 */
    snmpIfHCInMulticastPkts = 60,       /* RFC 2233 */
    snmpIfHCInBroadcastPkts = 61,       /* RFC 2233 */
    snmpIfHCOutOctets = 62,             /* RFC 2233 */
    snmpIfHCOutUcastPkts = 63,          /* RFC 2233 */
    snmpIfHCOutMulticastPkts = 64,      /* RFC 2233 */
    snmpIfHCOutBroadcastPckts = 65,     /* RFC 2233 */
    snmpIpv6IfStatsInReceives = 66,     /* RFC 2465 */
    snmpIpv6IfStatsInHdrErrors = 67,    /* RFC 2465 */
    snmpIpv6IfStatsInAddrErrors = 68,   /* RFC 2465 */
    snmpIpv6IfStatsInDiscards = 69,     /* RFC 2465 */
    snmpIpv6IfStatsOutForwDatagrams = 70, /* RFC 2465 */
    snmpIpv6IfStatsOutDiscards = 71,    /* RFC 2465 */
    snmpIpv6IfStatsInMcastPkts = 72,    /* RFC 2465 */
    snmpIpv6IfStatsOutMcastPkts = 73,   /* RFC 2465 */
    snmpIfInBroadcastPkts = 74,         /* RFC 1573 */
    snmpIfInMulticastPkts = 75,         /* RFC 1573 */
    snmpIfOutBroadcastPkts = 76,        /* RFC 1573 */
    snmpIfOutMulticastPkts = 77,        /* RFC 1573 */
    snmpIeee8021PfcRequests = 78,       /* IEEE 802.1bb */
    snmpIeee8021PfcIndications = 79,    /* IEEE 802.1bb */
    snmpBcmReceivedUndersizePkts = 80,  /* Broadcom-specific */
    snmpBcmTransmittedUndersizePkts = 81, /* Broadcom-specific */
    snmpBcmIPMCBridgedPckts = 82,       /* Broadcom-specific */
    snmpBcmIPMCRoutedPckts = 83,        /* Broadcom-specific */
    snmpBcmIPMCInDroppedPckts = 84,     /* Broadcom-specific */
    snmpBcmIPMCOutDroppedPckts = 85,    /* Broadcom-specific */
    snmpBcmEtherStatsPkts1519to1522Octets = 86, /* Broadcom-specific */
    snmpBcmEtherStatsPkts1522to2047Octets = 87, /* Broadcom-specific */
    snmpBcmEtherStatsPkts2048to4095Octets = 88, /* Broadcom-specific */
    snmpBcmEtherStatsPkts4095to9216Octets = 89, /* Broadcom-specific */
    snmpBcmReceivedPkts64Octets = 90,   /* Broadcom-specific */
    snmpBcmReceivedPkts65to127Octets = 91, /* Broadcom-specific */
    snmpBcmReceivedPkts128to255Octets = 92, /* Broadcom-specific */
    snmpBcmReceivedPkts256to511Octets = 93, /* Broadcom-specific */
    snmpBcmReceivedPkts512to1023Octets = 94, /* Broadcom-specific */
    snmpBcmReceivedPkts1024to1518Octets = 95, /* Broadcom-specific */
    snmpBcmReceivedPkts1519to2047Octets = 96, /* Broadcom-specific */
    snmpBcmReceivedPkts2048to4095Octets = 97, /* Broadcom-specific */
    snmpBcmReceivedPkts4095to9216Octets = 98, /* Broadcom-specific */
    snmpBcmTransmittedPkts64Octets = 99, /* Broadcom-specific */
    snmpBcmTransmittedPkts65to127Octets = 100, /* Broadcom-specific */
    snmpBcmTransmittedPkts128to255Octets = 101, /* Broadcom-specific */
    snmpBcmTransmittedPkts256to511Octets = 102, /* Broadcom-specific */
    snmpBcmTransmittedPkts512to1023Octets = 103, /* Broadcom-specific */
    snmpBcmTransmittedPkts1024to1518Octets = 104, /* Broadcom-specific */
    snmpBcmTransmittedPkts1519to2047Octets = 105, /* Broadcom-specific */
    snmpBcmTransmittedPkts2048to4095Octets = 106, /* Broadcom-specific */
    snmpBcmTransmittedPkts4095to9216Octets = 107, /* Broadcom-specific */
    snmpBcmTxControlCells = 108,        /* Fabric TX Control cells counter.
                                           (Broadcom-specific) */
    snmpBcmTxDataCells = 109,           /* Fabric TX Data cell counter.
                                           (Broadcom-specific) */
    snmpBcmTxDataBytes = 110,           /* Fabric TX Data byte counter.
                                           (Broadcom-specific) */
    snmpBcmRxCrcErrors = 111,           /* Fabric RX CRC errors counter.
                                           (Broadcom-specific) */
    snmpBcmRxFecCorrectable = 112,      /* Fabric RX LFEC/FEC correctable error.
                                           (Broadcom-specific) */
    snmpBcmRxBecCrcErrors = 113,        /* Fabric RX BEC crc error.
                                           (Broadcom-specific) */
    snmpBcmRxDisparityErrors = 114,     /* Fabric RX 8b/10b disparity errors.
                                           (Broadcom-specific) */
    snmpBcmRxControlCells = 115,        /* Fabric RX Control cells counter.
                                           (Broadcom-specific) */
    snmpBcmRxDataCells = 116,           /* Fabric RX Data cell counter.
                                           (Broadcom-specific) */
    snmpBcmRxDataBytes = 117,           /* Fabric RX Data byte counter.
                                           (Broadcom-specific) */
    snmpBcmRxDroppedRetransmittedControl = 118, /* Fabric RX dropped retransmitted
                                           control. (Broadcom-specific) */
    snmpBcmTxBecRetransmit = 119,       /* Fabric TX BEC retransmit.
                                           (Broadcom-specific) */
    snmpBcmRxBecRetransmit = 120,       /* Fabric RX BEC retransmit.
                                           (Broadcom-specific) */
    snmpBcmTxAsynFifoRate = 121,        /* Fabric TX Asyn FIFO rate at units of
                                           40 bits. (Broadcom-specific) */
    snmpBcmRxAsynFifoRate = 122,        /* Fabric RX Asyn FIFO rate at units of
                                           40 bits. (Broadcom-specific) */
    snmpBcmRxFecUncorrectable = 123,    /* Fabric RX LFEC/FEC uncorrectable
                                           erros. (Broadcom-specific) */
    snmpBcmRxBecRxFault = 124,          /* Fabric RX BEC RX fault.
                                           (Broadcom-specific) */
    snmpBcmRxCodeErrors = 125,          /* Fabric RX 8b/10b code errors.
                                           (Broadcom-specific) */
    snmpBcmRxRsFecBitError = 126,       /* Fabric RX RS-FEC bit error counter.
                                           (Broadcom-specific) */
    snmpBcmRxRsFecSymbolError = 127,    /* RX RS-FEC symbol error counter.
                                           (Broadcom-specific) */
    snmpBcmRxLlfcPrimary = 128,         /* Fabric RX LLFC Primary.
                                           (Broadcom-specific) */
    snmpBcmRxLlfcSecondary = 129,       /* Fabric RX LLFC Secondary.
                                           (Broadcom-specific) */
    snmpBcmCustomReceive0 = 130,        /* Broadcom-specific */
    snmpBcmCustomReceive1 = 131,        /* Broadcom-specific */
    snmpBcmCustomReceive2 = 132,        /* Broadcom-specific */
    snmpBcmCustomReceive3 = 133,        /* Broadcom-specific */
    snmpBcmCustomReceive4 = 134,        /* Broadcom-specific */
    snmpBcmCustomReceive5 = 135,        /* Broadcom-specific */
    snmpBcmCustomReceive6 = 136,        /* Broadcom-specific */
    snmpBcmCustomReceive7 = 137,        /* Broadcom-specific */
    snmpBcmCustomReceive8 = 138,        /* Broadcom-specific */
    snmpBcmCustomTransmit0 = 139,       /* Broadcom-specific */
    snmpBcmCustomTransmit1 = 140,       /* Broadcom-specific */
    snmpBcmCustomTransmit2 = 141,       /* Broadcom-specific */
    snmpBcmCustomTransmit3 = 142,       /* Broadcom-specific */
    snmpBcmCustomTransmit4 = 143,       /* Broadcom-specific */
    snmpBcmCustomTransmit5 = 144,       /* Broadcom-specific */
    snmpBcmCustomTransmit6 = 145,       /* Broadcom-specific */
    snmpBcmCustomTransmit7 = 146,       /* Broadcom-specific */
    snmpBcmCustomTransmit8 = 147,       /* Broadcom-specific */
    snmpBcmCustomTransmit9 = 148,       /* Broadcom-specific */
    snmpBcmCustomTransmit10 = 149,      /* Broadcom-specific */
    snmpBcmCustomTransmit11 = 150,      /* Broadcom-specific */
    snmpBcmCustomTransmit12 = 151,      /* Broadcom-specific */
    snmpBcmCustomTransmit13 = 152,      /* Broadcom-specific */
    snmpBcmCustomTransmit14 = 153,      /* Broadcom-specific */
    snmpDot3StatsInRangeLengthError = 154, /* RFC 1284 */
    snmpDot3OmpEmulationCRC8Errors = 155, /* RFC 4837 */
    snmpDot3MpcpRxGate = 156,           /* RFC 4837 */
    snmpDot3MpcpRxRegister = 157,       /* RFC 4837 */
    snmpDot3MpcpTxRegRequest = 158,     /* RFC 4837 */
    snmpDot3MpcpTxRegAck = 159,         /* RFC 4837 */
    snmpDot3MpcpTxReport = 160,         /* RFC 4837 */
    snmpDot3EponFecCorrectedBlocks = 161, /* RFC 4837 */
    snmpDot3EponFecUncorrectableBlocks = 162, /* RFC 4837 */
    snmpBcmPonInDroppedOctets = 163,    /* Broadcom-specific */
    snmpBcmPonOutDroppedOctets = 164,   /* Broadcom-specific */
    snmpBcmPonInDelayedOctets = 165,    /* Broadcom-specific */
    snmpBcmPonOutDelayedOctets = 166,   /* Broadcom-specific */
    snmpBcmPonInDelayedHundredUs = 167, /* Broadcom-specific */
    snmpBcmPonOutDelayedHundredUs = 168, /* Broadcom-specific */
    snmpBcmPonInFrameErrors = 169,      /* Broadcom-specific */
    snmpBcmPonInOamFrames = 170,        /* Broadcom-specific */
    snmpBcmPonOutOamFrames = 171,       /* Broadcom-specific */
    snmpBcmPonOutUnusedOctets = 172,    /* Broadcom-specific */
    snmpBcmEtherStatsPkts9217to16383Octets = 173, /* Broadcom-specific */
    snmpBcmReceivedPkts9217to16383Octets = 174, /* Broadcom-specific */
    snmpBcmTransmittedPkts9217to16383Octets = 175, /* Broadcom-specific */
    snmpBcmRxVlanTagFrame = 176,        /* Broadcom-specific */
    snmpBcmRxDoubleVlanTagFrame = 177,  /* Broadcom-specific */
    snmpBcmTxVlanTagFrame = 178,        /* Broadcom-specific */
    snmpBcmTxDoubleVlanTagFrame = 179,  /* Broadcom-specific */
    snmpBcmRxPFCControlFrame = 180,     /* PFC Receive Control Frame */
    snmpBcmTxPFCControlFrame = 181,     /* PFC Transmit Control Frame */
    snmpBcmRxPFCFrameXonPriority0 = 182, /* Receive PFC Frame Priority 0 XON to
                                           XOFF */
    snmpBcmRxPFCFrameXonPriority1 = 183, /* Receive PFC Frame Priority 1 XON to
                                           XOFF */
    snmpBcmRxPFCFrameXonPriority2 = 184, /* Receive PFC Frame Priority 2 XON to
                                           XOFF */
    snmpBcmRxPFCFrameXonPriority3 = 185, /* Receive PFC Frame Priority 3 XON to
                                           XOFF */
    snmpBcmRxPFCFrameXonPriority4 = 186, /* Receive PFC Frame Priority 4 XON to
                                           XOFF */
    snmpBcmRxPFCFrameXonPriority5 = 187, /* Receive PFC Frame Priority 5 XON to
                                           XOFF */
    snmpBcmRxPFCFrameXonPriority6 = 188, /* Receive PFC Frame Priority 6 XON to
                                           XOFF */
    snmpBcmRxPFCFrameXonPriority7 = 189, /* Receive PFC Frame Priority 7 XON to
                                           XOFF */
    snmpBcmRxPFCFramePriority0 = 190,   /* Receive PFC Frame Priority 0 */
    snmpBcmRxPFCFramePriority1 = 191,   /* Receive PFC Frame Priority 1 */
    snmpBcmRxPFCFramePriority2 = 192,   /* Receive PFC Frame Priority 2 */
    snmpBcmRxPFCFramePriority3 = 193,   /* Receive PFC Frame Priority 3 */
    snmpBcmRxPFCFramePriority4 = 194,   /* Receive PFC Frame Priority 4 */
    snmpBcmRxPFCFramePriority5 = 195,   /* Receive PFC Frame Priority 5 */
    snmpBcmRxPFCFramePriority6 = 196,   /* Receive PFC Frame Priority 6 */
    snmpBcmRxPFCFramePriority7 = 197,   /* Receive PFC Frame Priority 7 */
    snmpBcmTxPFCFramePriority0 = 198,   /* Transmit PFC Frame Priority 0 */
    snmpBcmTxPFCFramePriority1 = 199,   /* Transmit PFC Frame Priority 1 */
    snmpBcmTxPFCFramePriority2 = 200,   /* Transmit PFC Frame Priority 2 */
    snmpBcmTxPFCFramePriority3 = 201,   /* Transmit PFC Frame Priority 3 */
    snmpBcmTxPFCFramePriority4 = 202,   /* Transmit PFC Frame Priority 4 */
    snmpBcmTxPFCFramePriority5 = 203,   /* Transmit PFC Frame Priority 5 */
    snmpBcmTxPFCFramePriority6 = 204,   /* Transmit PFC Frame Priority 6 */
    snmpBcmTxPFCFramePriority7 = 205,   /* Transmit PFC Frame Priority 7 */
    snmpFcmPortClass3RxFrames = 206,    /* RFC 4044 */
    snmpFcmPortClass3TxFrames = 207,    /* RFC 4044 */
    snmpFcmPortClass3Discards = 208,    /* RFC 4044 */
    snmpFcmPortClass2RxFrames = 209,    /* RFC 4044 */
    snmpFcmPortClass2TxFrames = 210,    /* RFC 4044 */
    snmpFcmPortClass2Discards = 211,    /* RFC 4044 */
    snmpFcmPortInvalidCRCs = 212,       /* RFC 4044 */
    snmpFcmPortDelimiterErrors = 213,   /* RFC 4044 */
    snmpBcmSampleIngressPkts = 214,     /* Total number of packets eligible to
                                           be sampled by Ingress Sampler.
                                           (Broadcom-specific) */
    snmpBcmSampleIngressSnapshotPkts = 215, /* Total number of eligible packets when
                                           the last packet was sampled by
                                           Ingress sampler. (Broadcom-specific) */
    snmpBcmSampleIngressSampledPkts = 216, /* Total number of packets sampled by
                                           Ingress sampler. (Broadcom-specific) */
    snmpBcmSampleFlexPkts = 217,        /* Total number of packets eligible to
                                           be sampled by Flex sampler enabled by
                                           IFP ingress sampling policy action.
                                           (Broadcom-specific) */
    snmpBcmSampleFlexSnapshotPkts = 218, /* Total number of eligible packets when
                                           the last packet was sampled by Flex
                                           sampler enabled by IFP ingress
                                           sampling policy action.
                                           (Broadcom-specific) */
    snmpBcmSampleFlexSampledPkts = 219, /* Total number of packets sampled by
                                           Flex sampler enabled by IFP ingress
                                           sampling policy action.
                                           (Broadcom-specific) */
    snmpBcmEgressProtectionDataDrop = 220, /* Egress Protection data drop in EPIPE.
                                           (Broadcom-specific) */
    snmpBcmTxE2ECCControlFrames = 221,  /* Total number of End to End Congestion
                                           Control(E2E-CC) Head of line(HOL)
                                           packets or DMVoQ flow control packets
                                           transmitted on each of the ports.
                                           (Broadcom-specific) */
    snmpBcmE2EHOLDropPkts = 222,        /* Total number of packets dropped due
                                           to E2EHOL (End to End Head of Line)
                                           drop status. (Broadcom-specific) */
    snmpEtherStatsTxCRCAlignErrors = 223, /* RFC 1757 (EtherStat) */
    snmpEtherStatsTxJabbers = 224,      /* RFC 1757 (EtherStat) */
    snmpBcmMacMergeTxFrag = 225,        /* Number of additional mPackets
                                           transmitted due to preemption. */
    snmpBcmMacMergeTxVerifyFrame = 226, /* Number of transmitted verify
                                           mPackets. */
    snmpBcmMacMergeTxReplyFrame = 227,  /* Number of transmitted reply mPackets. */
    snmpBcmMacMergeRxFrameAssErrors = 228, /* Number of packets with reassembly
                                           errors in MAC receive direction. */
    snmpBcmMacMergeRxFrameSmdErrors = 229, /* Number of received packets rejected
                                           due to unknown SMD value or arriving
                                           with an SMD-C when no packet is in
                                           progress. */
    snmpBcmMacMergeRxFrameAss = 230,    /* Number of received packets that were
                                           successfully reassembled. */
    snmpBcmMacMergeRxFrag = 231,        /* Number of additional mPackets
                                           received due to preemption. */
    snmpBcmMacMergeRxVerifyFrame = 232, /* Number of received verify mPackets. */
    snmpBcmMacMergeRxReplyFrame = 233,  /* Number of received reply mPackets. */
    snmpBcmMacMergeRxFinalFragSizeError = 234, /* Number of the received mPackets which
                                           are less than non-final fragments
                                           size. */
    snmpBcmMacMergeRxFragSizeError = 235, /* Number of received mPackets which are
                                           less than final fragments size. */
    snmpBcmMacMergeRxDiscard = 236,     /* Number of all discarded mPackets
                                           because of error conditions. */
    snmpBcmMacMergeHoldCount = 237,     /* Number of hold request primitive
                                           events. */
    snmpBcmRxBipErrorCount = 238,       /* RX BER and BIP counter. */
    snmpBcmTxEcnErrors = 239,           /* Egress ECN error counter. */
    snmpBcmRxDot3LengthMismatches = 240, /* Number of the received good frames in
                                           which the 802.3 length field is valid
                                           (<= 0x600) and does not match the
                                           number of data bytes actually
                                           received. */
    snmpBcmReceivedTruncatedPkts = 241, /* Number of the received truncated
                                           frame counter. */
    snmpBcmReceivedPreemptPkts64Octets = 242, /* Broadcom-specific */
    snmpBcmReceivedPreemptPkts65to127Octets = 243, /* Broadcom-specific */
    snmpBcmReceivedPreemptPkts128to255Octets = 244, /* Broadcom-specific */
    snmpBcmReceivedPreemptPkts256to511Octets = 245, /* Broadcom-specific */
    snmpBcmReceivedPreemptPkts512to1023Octets = 246, /* Broadcom-specific */
    snmpBcmReceivedPreemptPkts1024to1518Octets = 247, /* Broadcom-specific */
    snmpBcmReceivedPreemptPkts1519to2047Octets = 248, /* Broadcom-specific */
    snmpBcmReceivedPreemptPkts2048to4095Octets = 249, /* Broadcom-specific */
    snmpBcmReceivedPreemptPkts4095to9216Octets = 250, /* Broadcom-specific */
    snmpBcmReceivedPreemptPkts9217to16383Octets = 251, /* Broadcom-specific */
    snmpBcmTransmittedPreemptPkts64Octets = 252, /* Broadcom-specific */
    snmpBcmTransmittedPreemptPkts65to127Octets = 253, /* Broadcom-specific */
    snmpBcmTransmittedPreemptPkts128to255Octets = 254, /* Broadcom-specific */
    snmpBcmTransmittedPreemptPkts256to511Octets = 255, /* Broadcom-specific */
    snmpBcmTransmittedPreemptPkts512to1023Octets = 256, /* Broadcom-specific */
    snmpBcmTransmittedPreemptPkts1024to1518Octets = 257, /* Broadcom-specific */
    snmpBcmTransmittedPreemptPkts1519to2047Octets = 258, /* Broadcom-specific */
    snmpBcmTransmittedPreemptPkts2048to4095Octets = 259, /* Broadcom-specific */
    snmpBcmTransmittedPreemptPkts4095to9216Octets = 260, /* Broadcom-specific */
    snmpBcmTransmittedPreemptPkts9217to16383Octets = 261, /* Broadcom-specific */
    snmpBcmIfInUcastPreemptPkts = 262,  /* Broadcom-specific */
    snmpBcmIfInPreemptOctets = 263,     /* Broadcom-specific */
    snmpBcmIfInPreemptErrors = 264,     /* Broadcom-specific */
    snmpBcmIfInMulticastPreemptPkts = 265, /* Broadcom-specific */
    snmpBcmIfInBroadcastPreemptPkts = 266, /* Broadcom-specific */
    snmpBcmIfOutPreemptOctets = 267,    /* Broadcom-specific */
    snmpBcmIfOutUcastPreemptPkts = 268, /* Broadcom-specific */
    snmpBcmIfOutBroadcastPreemptPkts = 269, /* Broadcom-specific */
    snmpBcmIfOutMulticastPreemptPkts = 270, /* Broadcom-specific */
    snmpBcmEtherStatsBroadcastPreemptPkts = 271, /* Broadcom-specific */
    snmpBcmEtherStatsMulticastPreemptPkts = 272, /* Broadcom-specific */
    snmpBcmEtherStatsCRCAlignPreemptErrors = 273, /* Broadcom-specific */
    snmpBcmEtherStatsUndersizePreemptPkts = 274, /* Broadcom-specific */
    snmpBcmEtherStatsOversizePreemptPkts = 275, /* Broadcom-specific */
    snmpBcmEtherStatsPreemptFragments = 276, /* Broadcom-specific */
    snmpBcmEtherStatsPreemptJabbers = 277, /* Broadcom-specific */
    snmpBcmEtherStatsPreemptPkts = 278, /* Broadcom-specific */
    snmpBcmEtherStatsPreemptOctets = 279, /* Broadcom-specific */
    snmpBcmIfOutPreemptErrors = 280,    /* Broadcom-specific */
    snmpBcmCustomReceive9 = 281,        /* Broadcom-specific */
    snmpBcmCustomReceive10 = 282,       /* Broadcom-specific */
    snmpBcmCustomReceive11 = 283,       /* Broadcom-specific */
    snmpBcmCustomReceive12 = 284,       /* Broadcom-specific */
    snmpBcmCustomReceive13 = 285,       /* Broadcom-specific */
    snmpBcmCustomReceive14 = 286,       /* Broadcom-specific */
    snmpBcmCustomReceive15 = 287,       /* Broadcom-specific */
    snmpBcmRxTunnelDecapEcnPkts = 288,  /* Number of ECN marked packets received
                                           on a terminating tunnel. */
    snmpBcmTxEcnPkts = 289,             /* Number of the transmitted ECN marked
                                           packets. */
    snmpValCount = 290 
} bcm_stat_val_t;

#define BCM_STAT_NAME_INITIALIZER \
{ \
    "snmpIfInOctets", \
    "snmpIfInUcastPkts", \
    "snmpIfInNUcastPkts", \
    "snmpIfInDiscards", \
    "snmpIfInErrors", \
    "snmpIfInUnknownProtos", \
    "snmpIfOutOctets", \
    "snmpIfOutUcastPkts", \
    "snmpIfOutNUcastPkts", \
    "snmpIfOutDiscards", \
    "snmpIfOutErrors", \
    "snmpIfOutQLen", \
    "snmpIpInReceives", \
    "snmpIpInHdrErrors", \
    "snmpIpForwDatagrams", \
    "snmpIpInDiscards", \
    "snmpDot1dBasePortDelayExceededDiscards", \
    "snmpDot1dBasePortMtuExceededDiscards", \
    "snmpDot1dTpPortInFrames", \
    "snmpDot1dTpPortOutFrames", \
    "snmpDot1dPortInDiscards", \
    "snmpEtherStatsDropEvents", \
    "snmpEtherStatsMulticastPkts", \
    "snmpEtherStatsBroadcastPkts", \
    "snmpEtherStatsUndersizePkts", \
    "snmpEtherStatsFragments", \
    "snmpEtherStatsPkts64Octets", \
    "snmpEtherStatsPkts65to127Octets", \
    "snmpEtherStatsPkts128to255Octets", \
    "snmpEtherStatsPkts256to511Octets", \
    "snmpEtherStatsPkts512to1023Octets", \
    "snmpEtherStatsPkts1024to1518Octets", \
    "snmpEtherStatsOversizePkts", \
    "snmpEtherRxOversizePkts", \
    "snmpEtherTxOversizePkts", \
    "snmpEtherStatsJabbers", \
    "snmpEtherStatsOctets", \
    "snmpEtherStatsPkts", \
    "snmpEtherStatsCollisions", \
    "snmpEtherStatsCRCAlignErrors", \
    "snmpEtherStatsTXNoErrors", \
    "snmpEtherStatsRXNoErrors", \
    "snmpDot3StatsAlignmentErrors", \
    "snmpDot3StatsFCSErrors", \
    "snmpDot3StatsSingleCollisionFrames", \
    "snmpDot3StatsMultipleCollisionFrames", \
    "snmpDot3StatsSQETTestErrors", \
    "snmpDot3StatsDeferredTransmissions", \
    "snmpDot3StatsLateCollisions", \
    "snmpDot3StatsExcessiveCollisions", \
    "snmpDot3StatsInternalMacTransmitErrors", \
    "snmpDot3StatsCarrierSenseErrors", \
    "snmpDot3StatsFrameTooLongs", \
    "snmpDot3StatsInternalMacReceiveErrors", \
    "snmpDot3StatsSymbolErrors", \
    "snmpDot3ControlInUnknownOpcodes", \
    "snmpDot3InPauseFrames", \
    "snmpDot3OutPauseFrames", \
    "snmpIfHCInOctets", \
    "snmpIfHCInUcastPkts", \
    "snmpIfHCInMulticastPkts", \
    "snmpIfHCInBroadcastPkts", \
    "snmpIfHCOutOctets", \
    "snmpIfHCOutUcastPkts", \
    "snmpIfHCOutMulticastPkts", \
    "snmpIfHCOutBroadcastPckts", \
    "snmpIpv6IfStatsInReceives", \
    "snmpIpv6IfStatsInHdrErrors", \
    "snmpIpv6IfStatsInAddrErrors", \
    "snmpIpv6IfStatsInDiscards", \
    "snmpIpv6IfStatsOutForwDatagrams", \
    "snmpIpv6IfStatsOutDiscards", \
    "snmpIpv6IfStatsInMcastPkts", \
    "snmpIpv6IfStatsOutMcastPkts", \
    "snmpIfInBroadcastPkts", \
    "snmpIfInMulticastPkts", \
    "snmpIfOutBroadcastPkts", \
    "snmpIfOutMulticastPkts", \
    "snmpIeee8021PfcRequests", \
    "snmpIeee8021PfcIndications", \
    "snmpBcmReceivedUndersizePkts", \
    "snmpBcmTransmittedUndersizePkts", \
    "snmpBcmIPMCBridgedPckts", \
    "snmpBcmIPMCRoutedPckts", \
    "snmpBcmIPMCInDroppedPckts", \
    "snmpBcmIPMCOutDroppedPckts", \
    "snmpBcmEtherStatsPkts1519to1522Octets", \
    "snmpBcmEtherStatsPkts1522to2047Octets", \
    "snmpBcmEtherStatsPkts2048to4095Octets", \
    "snmpBcmEtherStatsPkts4095to9216Octets", \
    "snmpBcmReceivedPkts64Octets", \
    "snmpBcmReceivedPkts65to127Octets", \
    "snmpBcmReceivedPkts128to255Octets", \
    "snmpBcmReceivedPkts256to511Octets", \
    "snmpBcmReceivedPkts512to1023Octets", \
    "snmpBcmReceivedPkts1024to1518Octets", \
    "snmpBcmReceivedPkts1519to2047Octets", \
    "snmpBcmReceivedPkts2048to4095Octets", \
    "snmpBcmReceivedPkts4095to9216Octets", \
    "snmpBcmTransmittedPkts64Octets", \
    "snmpBcmTransmittedPkts65to127Octets", \
    "snmpBcmTransmittedPkts128to255Octets", \
    "snmpBcmTransmittedPkts256to511Octets", \
    "snmpBcmTransmittedPkts512to1023Octets", \
    "snmpBcmTransmittedPkts1024to1518Octets", \
    "snmpBcmTransmittedPkts1519to2047Octets", \
    "snmpBcmTransmittedPkts2048to4095Octets", \
    "snmpBcmTransmittedPkts4095to9216Octets", \
    "snmpBcmTxControlCells", \
    "snmpBcmTxDataCells", \
    "snmpBcmTxDataBytes", \
    "snmpBcmRxCrcErrors", \
    "snmpBcmRxFecCorrectable", \
    "snmpBcmRxBecCrcErrors", \
    "snmpBcmRxDisparityErrors", \
    "snmpBcmRxControlCells", \
    "snmpBcmRxDataCells", \
    "snmpBcmRxDataBytes", \
    "snmpBcmRxDroppedRetransmittedControl", \
    "snmpBcmTxBecRetransmit", \
    "snmpBcmRxBecRetransmit", \
    "snmpBcmTxAsynFifoRate", \
    "snmpBcmRxAsynFifoRate", \
    "snmpBcmRxFecUncorrectable", \
    "snmpBcmRxBecRxFault", \
    "snmpBcmRxCodeErrors", \
    "snmpBcmRxRsFecBitError", \
    "snmpBcmRxRsFecSymbolError", \
    "snmpBcmRxLlfcPrimary", \
    "snmpBcmRxLlfcSecondary", \
    "snmpBcmCustomReceive0", \
    "snmpBcmCustomReceive1", \
    "snmpBcmCustomReceive2", \
    "snmpBcmCustomReceive3", \
    "snmpBcmCustomReceive4", \
    "snmpBcmCustomReceive5", \
    "snmpBcmCustomReceive6", \
    "snmpBcmCustomReceive7", \
    "snmpBcmCustomReceive8", \
    "snmpBcmCustomTransmit0", \
    "snmpBcmCustomTransmit1", \
    "snmpBcmCustomTransmit2", \
    "snmpBcmCustomTransmit3", \
    "snmpBcmCustomTransmit4", \
    "snmpBcmCustomTransmit5", \
    "snmpBcmCustomTransmit6", \
    "snmpBcmCustomTransmit7", \
    "snmpBcmCustomTransmit8", \
    "snmpBcmCustomTransmit9", \
    "snmpBcmCustomTransmit10", \
    "snmpBcmCustomTransmit11", \
    "snmpBcmCustomTransmit12", \
    "snmpBcmCustomTransmit13", \
    "snmpBcmCustomTransmit14", \
    "snmpDot3StatsInRangeLengthError", \
    "snmpDot3OmpEmulationCRC8Errors", \
    "snmpDot3MpcpRxGate", \
    "snmpDot3MpcpRxRegister", \
    "snmpDot3MpcpTxRegRequest", \
    "snmpDot3MpcpTxRegAck", \
    "snmpDot3MpcpTxReport", \
    "snmpDot3EponFecCorrectedBlocks", \
    "snmpDot3EponFecUncorrectableBlocks", \
    "snmpBcmPonInDroppedOctets", \
    "snmpBcmPonOutDroppedOctets", \
    "snmpBcmPonInDelayedOctets", \
    "snmpBcmPonOutDelayedOctets", \
    "snmpBcmPonInDelayedHundredUs", \
    "snmpBcmPonOutDelayedHundredUs", \
    "snmpBcmPonInFrameErrors", \
    "snmpBcmPonInOamFrames", \
    "snmpBcmPonOutOamFrames", \
    "snmpBcmPonOutUnusedOctets", \
    "snmpBcmEtherStatsPkts9217to16383Octets", \
    "snmpBcmReceivedPkts9217to16383Octets", \
    "snmpBcmTransmittedPkts9217to16383Octets", \
    "snmpBcmRxVlanTagFrame", \
    "snmpBcmRxDoubleVlanTagFrame", \
    "snmpBcmTxVlanTagFrame", \
    "snmpBcmTxDoubleVlanTagFrame", \
    "snmpBcmRxPFCControlFrame", \
    "snmpBcmTxPFCControlFrame", \
    "snmpBcmRxPFCFrameXonPriority0", \
    "snmpBcmRxPFCFrameXonPriority1", \
    "snmpBcmRxPFCFrameXonPriority2", \
    "snmpBcmRxPFCFrameXonPriority3", \
    "snmpBcmRxPFCFrameXonPriority4", \
    "snmpBcmRxPFCFrameXonPriority5", \
    "snmpBcmRxPFCFrameXonPriority6", \
    "snmpBcmRxPFCFrameXonPriority7", \
    "snmpBcmRxPFCFramePriority0", \
    "snmpBcmRxPFCFramePriority1", \
    "snmpBcmRxPFCFramePriority2", \
    "snmpBcmRxPFCFramePriority3", \
    "snmpBcmRxPFCFramePriority4", \
    "snmpBcmRxPFCFramePriority5", \
    "snmpBcmRxPFCFramePriority6", \
    "snmpBcmRxPFCFramePriority7", \
    "snmpBcmTxPFCFramePriority0", \
    "snmpBcmTxPFCFramePriority1", \
    "snmpBcmTxPFCFramePriority2", \
    "snmpBcmTxPFCFramePriority3", \
    "snmpBcmTxPFCFramePriority4", \
    "snmpBcmTxPFCFramePriority5", \
    "snmpBcmTxPFCFramePriority6", \
    "snmpBcmTxPFCFramePriority7", \
    "snmpFcmPortClass3RxFrames", \
    "snmpFcmPortClass3TxFrames", \
    "snmpFcmPortClass3Discards", \
    "snmpFcmPortClass2RxFrames", \
    "snmpFcmPortClass2TxFrames", \
    "snmpFcmPortClass2Discards", \
    "snmpFcmPortInvalidCRCs", \
    "snmpFcmPortDelimiterErrors", \
    "snmpBcmSampleIngressPkts", \
    "snmpBcmSampleIngressSnapshotPkts", \
    "snmpBcmSampleIngressSampledPkts", \
    "snmpBcmSampleFlexPkts", \
    "snmpBcmSampleFlexSnapshotPkts", \
    "snmpBcmSampleFlexSampledPkts", \
    "snmpBcmEgressProtectionDataDrop", \
    "snmpBcmTxE2ECCControlFrames", \
    "snmpBcmE2EHOLDropPkts", \
    "snmpEtherStatsTxCRCAlignErrors", \
    "snmpEtherStatsTxJabbers", \
    "snmpBcmMacMergeTxFrag", \
    "snmpBcmMacMergeTxVerifyFrame", \
    "snmpBcmMacMergeTxReplyFrame", \
    "snmpBcmMacMergeRxFrameAssErrors", \
    "snmpBcmMacMergeRxFrameSmdErrors", \
    "snmpBcmMacMergeRxFrameAss", \
    "snmpBcmMacMergeRxFrag", \
    "snmpBcmMacMergeRxVerifyFrame", \
    "snmpBcmMacMergeRxReplyFrame", \
    "snmpBcmMacMergeRxFinalFragSizeError", \
    "snmpBcmMacMergeRxFragSizeError", \
    "snmpBcmMacMergeRxDiscard", \
    "snmpBcmMacMergeHoldCount", \
    "snmpBcmRxBipErrorCount", \
    "snmpBcmTxEcnErrors", \
    "snmpBcmRxDot3LengthMismatches", \
    "snmpBcmReceivedTruncatedPkts", \
    "snmpBcmReceivedPreemptPkts64Octets", \
    "snmpBcmReceivedPreemptPkts65to127Octets", \
    "snmpBcmReceivedPreemptPkts128to255Octets", \
    "snmpBcmReceivedPreemptPkts256to511Octets", \
    "snmpBcmReceivedPreemptPkts512to1023Octets", \
    "snmpBcmReceivedPreemptPkts1024to1518Octets", \
    "snmpBcmReceivedPreemptPkts1519to2047Octets", \
    "snmpBcmReceivedPreemptPkts2048to4095Octets", \
    "snmpBcmReceivedPreemptPkts4095to9216Octets", \
    "snmpBcmReceivedPreemptPkts9217to16383Octets", \
    "snmpBcmTransmittedPreemptPkts64Octets", \
    "snmpBcmTransmittedPreemptPkts65to127Octets", \
    "snmpBcmTransmittedPreemptPkts128to255Octets", \
    "snmpBcmTransmittedPreemptPkts256to511Octets", \
    "snmpBcmTransmittedPreemptPkts512to1023Octets", \
    "snmpBcmTransmittedPreemptPkts1024to1518Octets", \
    "snmpBcmTransmittedPreemptPkts1519to2047Octets", \
    "snmpBcmTransmittedPreemptPkts2048to4095Octets", \
    "snmpBcmTransmittedPreemptPkts4095to9216Octets", \
    "snmpBcmTransmittedPreemptPkts9217to16383Octets", \
    "snmpBcmIfInUcastPreemptPkts", \
    "snmpBcmIfInPreemptOctets", \
    "snmpBcmIfInPreemptErrors", \
    "snmpBcmIfInMulticastPreemptPkts", \
    "snmpBcmIfInBroadcastPreemptPkts", \
    "snmpBcmIfOutPreemptOctets", \
    "snmpBcmIfOutUcastPreemptPkts", \
    "snmpBcmIfOutBroadcastPreemptPkts", \
    "snmpBcmIfOutMulticastPreemptPkts", \
    "snmpBcmEtherStatsBroadcastPreemptPkts", \
    "snmpBcmEtherStatsMulticastPreemptPkts", \
    "snmpBcmEtherStatsCRCAlignPreemptErrors", \
    "snmpBcmEtherStatsUndersizePreemptPkts", \
    "snmpBcmEtherStatsOversizePreemptPkts", \
    "snmpBcmEtherStatsPreemptFragments", \
    "snmpBcmEtherStatsPreemptJabbers", \
    "snmpBcmEtherStatsPreemptPkts", \
    "snmpBcmEtherStatsPreemptOctets", \
    "snmpBcmIfOutPreemptErrors", \
    "snmpBcmCustomReceive9", \
    "snmpBcmCustomReceive10", \
    "snmpBcmCustomReceive11", \
    "snmpBcmCustomReceive12", \
    "snmpBcmCustomReceive13", \
    "snmpBcmCustomReceive14", \
    "snmpBcmCustomReceive15", \
    "snmpBcmRxTunnelDecapEcnPkts", \
    "snmpBcmTxEcnPkts"  \
}

/* bcm_custom_stat_trigger_e */
typedef enum bcm_custom_stat_trigger_e {
    bcmDbgCntRIPD4 = 0,                 /* Rx: IPv4 L3 discard packets. */
    bcmDbgCntRIPC4 = 1,                 /* Rx: Good L3 (V4 packets) includes
                                           tunneled. */
    bcmDbgCntRIPHE4 = 2,                /* Rx: IPv4 header error packets. */
    bcmDbgCntIMRP4 = 3,                 /* Rx: Routed IPv4 multicast packets. */
    bcmDbgCntRIPD6 = 4,                 /* Rx: IPv6 L3 discard packets. */
    bcmDbgCntRIPC6 = 5,                 /* Rx: Good L3 (V6 packets) includes
                                           tunneled. */
    bcmDbgCntRIPHE6 = 6,                /* Rx: IPv6 header error packets. */
    bcmDbgCntIMRP6 = 7,                 /* Rx: Routed IPv6 multicast packets. */
    bcmDbgCntRDISC = 8,                 /* Rx: IBP discard and CBP full. */
    bcmDbgCntRUC = 9,                   /* Rx: Good unicast (L2+L3) packets. */
    bcmDbgCntRPORTD = 10,               /* Rx: Packets droppped when ingress
                                           port is not in forwarding state. */
    bcmDbgCntPDISC = 11,                /* Rx: policy discard - DST_DISCARD,
                                           SRC_DISCARD, RATE_CONTROL... */
    bcmDbgCntIMBP = 12,                 /* Rx: Bridged multicast packets. */
    bcmDbgCntRFILDR = 13,               /* Rx: Packets dropped by FP. */
    bcmDbgCntRIMDR = 14,                /* Rx: Multicast (L2+L3) packets that
                                           are dropped. */
    bcmDbgCntRDROP = 15,                /* Rx: Port bitmap zero drop condition. */
    bcmDbgCntIRPSE = 16,                /* Rx: HiGig IPIC pause packets. */
    bcmDbgCntIRHOL = 17,                /* Rx: HiGig End-to-End HOL packets
                                           (BCM56504/56700/56800/56580). */
    bcmDbgCntIRIBP = 18,                /* Rx: HiGig End-to-End IBP packets
                                           (BCM56504/56700/56800/56580). */
    bcmDbgCntDSL3HE = 19,               /* Rx: DOS L3 header error packets. */
    bcmDbgCntIUNKHDR = 20,              /* Rx: Unknown HiGig header type packet
                                           (BCM56700/56800/56580). */
    bcmDbgCntDSL4HE = 21,               /* Rx: DOS L4 header error packets. */
    bcmDbgCntIMIRROR = 22,              /* Rx: HiGig mirror packet
                                           (BCM56700/56800/56580). */
    bcmDbgCntDSICMP = 23,               /* Rx: DOS ICMP error packets. */
    bcmDbgCntDSFRAG = 24,               /* Rx: DOS fragment error packets. */
    bcmDbgCntMTUERR = 25,               /* Rx: Packets trapped to CPU due to
                                           egress L3 MTU violation. */
    bcmDbgCntRTUN = 26,                 /* Rx: Tunnel packets. */
    bcmDbgCntRTUNE = 27,                /* Rx: tunnel error packets. */
    bcmDbgCntVLANDR = 28,               /* Rx: VLAN drops. */
    bcmDbgCntRHGUC = 29,                /* Rx: HiGig lookup UC cases (BCM56504
                                           B0). */
    bcmDbgCntRHGMC = 30,                /* Rx: HiGig lookup MC cases (BCM56504
                                           B0). */
    bcmDbgCntMPLS = 31,                 /* Rx: MPLS packets (BCM5660x only). */
    bcmDbgCntMACLMT = 32,               /* Rx: packets dropped due to MAC Limit
                                           is hit. */
    bcmDbgCntMPLSERR = 33,              /* Rx: MPLS packets with error (BCM5660x
                                           only). */
    bcmDbgCntURPFERR = 34,              /* Rx: L3 source URPF check Fail
                                           (BCM5660x only). */
    bcmDbgCntHGHDRE = 35,               /* Rx: HiGig Header error packets
                                           (BCM56700/56800/56580/56514). */
    bcmDbgCntMCIDXE = 36,               /* Rx: Multicast Index error packets
                                           (BCM56700/56800/56580). */
    bcmDbgCntLAGLUP = 37,               /* Rx: LAG failover loopback packets
                                           (BCM56700/56800/56580). */
    bcmDbgCntLAGLUPD = 38,              /* Rx: LAG backup port down
                                           (BCM56700/56800/56580). */
    bcmDbgCntPARITYD = 39,              /* Rx: Parity error packets
                                           (BCM56700/56800/56580). */
    bcmDbgCntVFPDR = 40,                /* Rx: VLAN FP drop case (BCM56514). */
    bcmDbgCntURPF = 41,                 /* Rx: Unicast RPF drop case (BCM56514). */
    bcmDbgCntDSTDISCARDDROP = 42,       /* Rx: L2/L3 lookup DST_DISCARD drop. */
    bcmDbgCntCLASSBASEDMOVEDROP = 43,   /* Rx: Class based learning drop. */
    bcmDbgCntMACLMT_NODROP = 44,        /* Rx: Mac limit exceeded and packet not
                                           dropped. */
    bcmDbgCntMACSAEQUALMACDA = 45,      /* Rx: Dos Attack L2 Packets MACSA
                                           equals to MACDA. */
    bcmDbgCntMACLMT_DROP = 46,          /* Rx: Mac limit exceeded and packet
                                           dropped. */
    bcmDbgCntTGIP4 = 47,                /* Tx: Good IPv4 L3 UC packets. */
    bcmDbgCntTIPD4 = 48,                /* Tx: IPv4 L3 UC Aged and Drop packets. */
    bcmDbgCntTGIPMC4 = 49,              /* Tx: Good IPv4 IPMC packets. */
    bcmDbgCntTIPMCD4 = 50,              /* Tx: IPv4 IPMC Aged and Drop packets. */
    bcmDbgCntTGIP6 = 51,                /* Tx: Good IPv6 L3 UC packets. */
    bcmDbgCntTIPD6 = 52,                /* Tx: IPv6 L3 UC Aged and Drop Packets. */
    bcmDbgCntTGIPMC6 = 53,              /* Tx: Good IPv6 IPMC packets. */
    bcmDbgCntTIPMCD6 = 54,              /* Tx: IPv6 IPMC Aged and Drop packets. */
    bcmDbgCntTTNL = 55,                 /* Tx: Tunnel packets. */
    bcmDbgCntTTNLE = 56,                /* Tx: Tunnel error packets. */
    bcmDbgCntTTTLD = 57,                /* Tx: Packets dropped due to TTL
                                           threshold counter. */
    bcmDbgCntTCFID = 58,                /* Tx: Packets dropped when CFI set &
                                           pket is untagged or L3 switched for
                                           IPMC. */
    bcmDbgCntTVLAN = 59,                /* Tx: VLAN tagged packets. */
    bcmDbgCntTVLAND = 60,               /* Tx: Packets dropped due to invalid
                                           VLAN counter. */
    bcmDbgCntTVXLTMD = 61,              /* Tx: Packets dropped due to miss in
                                           VXLT table counter. */
    bcmDbgCntTSTGD = 62,                /* Tx: Packets dropped due to Spanning
                                           Tree State not in forwarding state. */
    bcmDbgCntTAGED = 63,                /* Tx: Packets dropped due to packet
                                           aged counter. */
    bcmDbgCntTL2MCD = 64,               /* Tx: L2 MC packets. */
    bcmDbgCntTPKTD = 65,                /* Tx: Packets dropped due to any
                                           condition. */
    bcmDbgCntTMIRR = 66,                /* Tx: Packets with mirroring flag. */
    bcmDbgCntTSIPL = 67,                /* Tx: Packets with SIP Link Local Drop
                                           flag (BCM56504/56700/56800/56580). */
    bcmDbgCntTHGUC = 68,                /* Tx: HiGig Looked-up L3UC packets
                                           (BCM56504_B0/56700/56800/56580). */
    bcmDbgCntTHGMC = 69,                /* Tx: HiGig Looked-up L3MC packets
                                           (BCM56504_B0/56700/56800/56580). */
    bcmDbgCntTHIGIG2 = 70,              /* Tx: Unknown HiGig2 Drops
                                           (BCM56700/56800/56580). */
    bcmDbgCntTHGI = 71,                 /* Tx: Unknown HiGig drops
                                           (BCM56700/56800/56580). */
    bcmDbgCntTL2_MTU = 72,              /* Tx: L2 MTU fail drops
                                           (BCM56700/56800/56580). */
    bcmDbgCntTPARITY_ERR = 73,          /* Tx: Parity error drops
                                           (BCM56700/56800/56580). */
    bcmDbgCntTIP_LEN_FAIL = 74,         /* Tx: IP Length check fail drops
                                           (BCM56700/56800/56580). */
    bcmDbgCntTMTUD = 75,                /* Tx: EBAD_MTU_drops (BCM5660x only). */
    bcmDbgCntTSLLD = 76,                /* Tx: ESIP_LINK_LOCAL (BCM5660x only). */
    bcmDbgCntTL2MPLS = 77,              /* Tx: L2_MPLS_ENCAP_TX (BCM5660x only). */
    bcmDbgCntTL3MPLS = 78,              /* Tx: L3_MPLS_ENCAP_TX (BCM5660x only). */
    bcmDbgCntTMPLS = 79,                /* Tx: MPLS_TX (BCM5660x only). */
    bcmDbgCntTMODIDTOOLARGEDROP = 80,   /* Tx: MODID greater than 31 drop
                                           counter. */
    bcmDbgCntPKTMODTOOLARGEDROP = 81,   /* Tx: Byte additions too large drop
                                           counter. */
    bcmDbgCntRX_SYMBOL = 82,            /* Fabric I/F => Rx Errors (e.g.8B10B) */
    bcmDbgCntTIME_ALIGNMENT_EVEN = 83,  /* Fabric I/F => Time Alignment (even) */
    bcmDbgCntTIME_ALIGNMENT_ODD = 84,   /* Fabric I/F => Time Alignment (odd) */
    bcmDbgCntBIT_INTERLEAVED_PARITY_EVEN = 85, /* Fabric I/F => BIP (even) */
    bcmDbgCntBIT_INTERLEAVED_PARITY_ODD = 86, /* Fabric I/F => BIP (odd) */
    bcmDbgCntFcmPortClass3RxFrames = 87, /* FCOE L3 RX frames */
    bcmDbgCntFcmPortClass3RxDiscards = 88, /* FCOE L3 RX discarded frames */
    bcmDbgCntFcmPortClass3TxFrames = 89, /* FCOE L3 TX frames */
    bcmDbgCntFcmPortClass2RxFrames = 90, /* FCOE L2 RX frames */
    bcmDbgCntFcmPortClass2RxDiscards = 91, /* FCOE L2 RX discarded frames */
    bcmDbgCntFcmPortClass2TxFrames = 92, /* FCOE L2 TX frames */
    bcmDbgCntNonHGoE = 93,              /* Non HGoE Rx frames on an HGoE port */
    bcmDbgCntHGoE = 94,                 /* HGoE Rx frames */
    bcmDbgCntEgressProtectionDataDrop = 95, /* Egress Protection data drop in EPIPE */
    bcmDbgCntVntagRxError = 96,         /* Rx: VNTAG error packets */
    bcmDbgCntNivRxForwardingError = 97, /* Rx: NIV/PE packets dropped due to
                                           forwarding error */
    bcmDbgCntIfpTriggerPfcRxDrop = 98,  /* Rx: Original ingress packets dropped
                                           due to IFP PfcTx action */
    bcmDbgCntEcnTxError = 99,           /* Tx: Egress packets dropped due to ECN
                                           error */
    bcmDbgCntNivTxPrune = 100,          /* Tx: Egress NIV/PE packets dropped due
                                           to pruning check */
    bcmDbgCntRxHigigLoopbackDrop = 101, /* Rx: HiGig loopback packets dropped. */
    bcmDbgCntRxHigigUnknownOpcodeDrop = 102, /* Rx: HiGig packets dropped due to
                                           unknown opcode. */
    bcmDbgCntRxIpInIpDrop = 103,        /* Rx: IPinIP tunnel process failure. */
    bcmDbgCntRxMimDrop = 104,           /* Rx: MiM tunnel process failure. */
    bcmDbgCntRxTunnelInterfaceError = 105, /* Rx: IP multicast tunnel incoming
                                           interface is incorrect. */
    bcmDbgCntRxMplsControlWordInvalid = 106, /* Rx: MPLS invalid control word. */
    bcmDbgCntRxMplsGalNotBosDrop = 107, /* Rx: MPLS Generic Label is not BOS. */
    bcmDbgCntRxMplsPayloadInvalid = 108, /* Rx: MPLS invalid payload. */
    bcmDbgCntRxMplsEntropyDrop = 109,   /* Rx: MPLS entropy label in unallowed
                                           range. */
    bcmDbgCntRxMplsLabelLookupMiss = 110, /* Rx: MPLS label lookup miss. */
    bcmDbgCntRxMplsActionInvalid = 111, /* Rx: MPLS invalid action. */
    bcmDbgCntRxTunnelTtlError = 112,    /* Rx: Tunnel TTL check failure. */
    bcmDbgCntRxTunnelShimError = 113,   /* Rx: Tunnel shim header error. */
    bcmDbgCntRxTunnelObjectValidationFail = 114, /* Rx: Tunnel termination failure as not
                                           all packet forwarding objects got
                                           assigned. */
    bcmDbgCntRxVlanMismatch = 115,      /* Rx: VLAN ID invalid. */
    bcmDbgCntRxVlanMemberMismatch = 116, /* Rx: Ingress port not in VLAN
                                           membership. */
    bcmDbgCntRxTpidMismatch = 117,      /* Rx: VLAN TPID mismatch. */
    bcmDbgCntRxPrivateVlanMismatch = 118, /* Rx: PVLAN VID mismatch. */
    bcmDbgCntRxMacIpBindError = 119,    /* Rx: MAC to IP bind check failure. */
    bcmDbgCntRxVlanTranslate2LookupMiss = 120, /* Rx: VLAN Translate2 lookup miss. */
    bcmDbgCntRxL3TunnelLookupMiss = 121, /* Rx: L3 Tunnel lookup miss. */
    bcmDbgCntRxMplsLookupMiss = 122,    /* Rx: MPLS lookup miss. */
    bcmDbgCntRxVlanTranslate1LookupMiss = 123, /* Rx: VLAN Translate1 lookup miss. */
    bcmDbgCntRxFcoeVftDrop = 124,       /* Rx: FVT header error. */
    bcmDbgCntRxFcoeSrcBindError = 125,  /* Rx: FCOE source bind check failure. */
    bcmDbgCntRxFcoeSrcFpmaError = 126,  /* Rx: Source FPMA prefix check failure. */
    bcmDbgCntRxVfiP2pDrop = 127,        /* Rx: PT2PT service but packets arrived
                                           from a VP that is not programmed in
                                           PT2PT connection. */
    bcmDbgCntRxStgDrop = 128,           /* Rx: Spanning tree state not in
                                           forwarding state. */
    bcmDbgCntRxCompositeError = 129,    /* Rx: HW related error like lookup
                                           table with parity error. */
    bcmDbgCntRxBpduDrop = 130,          /* Rx: BPUD packets dropped due to
                                           PORT_TABLE.DROP_BPDU. */
    bcmDbgCntRxProtocolDrop = 131,      /* Rx: Protocol packets dropped due to
                                           PROTOCOL_PKT_CONTROL config. */
    bcmDbgCntRxUnknownMacSaDrop = 132,  /* Rx: Unknown source MAC packets
                                           dropped due to CML_OFFSET.DROP. */
    bcmDbgCntRxSrcRouteDrop = 133,      /* Rx: Packets dropped due to MACSA is
                                           multicast(bit 40 == 1). */
    bcmDbgCntRxL2SrcDiscardDrop = 134,  /* Rx: L2 SRC_DISCARD drop. */
    bcmDbgCntRxL2SrcStaticMoveDrop = 135, /* Rx: Port movement of static L2 addr. */
    bcmDbgCntRxL2DstDiscardDrop = 136,  /* Rx: L2 DST_DISCARD drop. */
    bcmDbgCntRxL3DisableDrop = 137,     /* Rx: Packets dropped due to
                                           PORT_TABLE.V4L3_ENABLE/V6L3_ENABLE
                                           unset. */
    bcmDbgCntRxMacSaEqual0Drop = 138,   /* Rx: Packets dropped due to MACSA ==
                                           0. */
    bcmDbgCntRxVlanCrossConnectOrNonUnicastDrop = 139, /* Rx: L2 forwarding lookup miss or PBT
                                           non unicast packets drop. */
    bcmDbgCntRxTimeSyncDrop = 140,      /* Rx: Network time sync packets
                                           dropped. */
    bcmDbgCntRxIpmcDrop = 141,          /* Rx: IPMC process failure. */
    bcmDbgCntRxMyStationDrop = 142,     /* Rx: MY_STATION.DISCARD drop. */
    bcmDbgCntRxPrivateVlanVpFilterDrop = 143, /* Rx: Private vlan drop based on
                                           src/dst virtual port type. */
    bcmDbgCntRxL3DstDiscardDrop = 144,  /* Rx: L3 DST_DISCARD drop. */
    bcmDbgCntRxTunnelDecapEcnError = 145, /* Rx: Tunnel decap ECN error. */
    bcmDbgCntRxL3SrcDiscardDrop = 146,  /* Rx: L3 SRC_DISCARD drop. */
    bcmDbgCntRxFcoeZoneLookupMiss = 147, /* Rx: FCOE_ZONE lookup miss. */
    bcmDbgCntRxL3TtlError = 148,        /* Rx: L3 TTL check failure. */
    bcmDbgCntRxL3HeaderError = 149,     /* Rx: L3 header error. */
    bcmDbgCntRxL2HeaderError = 150,     /* Rx: L2 header error. */
    bcmDbgCntRxL3DstLookupDrop = 151,   /* Rx: IPMC processing drop, or
                                           L3_DESTINATION == 0 for L3UC packets. */
    bcmDbgCntRxL2TtlError = 152,        /* Rx: L2 ZONE TTL check failure. */
    bcmDbgCntRxL2RpfError = 153,        /* Rx: Incoming port/lag/svp check
                                           failure enforced L2 entry. */
    bcmDbgCntRxTagUntagDiscardDrop = 154, /* Rx: Packets dropped due to
                                           PORT_DIS_UNTAG/PORT_DIS_TAG. */
    bcmDbgCntRxStormControlDrop = 155,  /* Rx: Storm control metering drop. */
    bcmDbgCntRxFcoeZoneError = 156,     /* Rx: FCOE_ZONE check failure. */
    bcmDbgCntRxFieldChangeL2NoRedirectDrop = 157, /* Rx: FP Changes L2 Fields packets is
                                           not eligible for routing. Redirect
                                           action must be deployed to select
                                           packet destination. */
    bcmDbgCntRxNextHopDrop = 158,       /* Rx: NEXT_HOP drop. */
    bcmDbgCntRxNatDrop = 159,           /* Rx: NAT process failure, e.g.
                                           destination lookup miss or Hairpin
                                           drop. */
    bcmDbgCntIngressProtectionDataDrop = 160, /* Rx: Protection data drop. */
    bcmDbgCntRxSrcKnockoutDrop = 161,   /* Rx: SGPP == DGPP or SVP == DVP, etc. */
    bcmDbgCntRxMplsSeqNumberError = 162, /* Rx: MPLS control word sequence number
                                           error. */
    bcmDbgCntRxBlockMaskDrop = 163,     /* Rx: Packets dropped due to bitmap of
                                           ports to be blocked. */
    bcmDbgCntRxDlbRhResolutionError = 164, /* Rx: DLB or RH resolution error. */
    bcmDbgCntTxFwdDomainNotFoundDrop = 165, /* Tx: Forwarding domain not found. */
    bcmDbgCntTxNotFwdDomainMemberDrop = 166, /* Tx: Not forwarding domain member. */
    bcmDbgCntTxIpmcL2SrcPortPruneDrop = 167, /* Tx: IPMC L2 self port drop, VLAN ID
                                           in the packet is the same as the VLAN
                                           ID in EGR_L3_INTF. */
    bcmDbgCntTxNonUnicastPruneDrop = 168, /* Tx: Non unicast pruning. */
    bcmDbgCntTxSvpRemoveDrop = 169,     /* Tx: Virtual port pruning. */
    bcmDbgCntTxVplagPruneDrop = 170,    /* Tx: VPLAG pruning. */
    bcmDbgCntTxSplitHorizonDrop = 171,  /* Tx: Packets dropped due to split
                                           horizon check. This is done by
                                           enabling pruning for DVP network
                                           group. */
    bcmDbgCntTxMmuPurgeDrop = 172,      /* Tx: Packets dropped due to MMU purge. */
    bcmDbgCntTxStgDisableDrop = 173,    /* Tx: Packets dropped due to STG
                                           disabled. */
    bcmDbgCntTxEgressPortDrop = 174,    /* Tx: Packets dropped due to
                                           EGR_LPORT_PROFILE.DROP. */
    bcmDbgCntTxEgressFilterDrop = 175,  /* Tx: Packets dropped by EFP. */
    bcmDbgCntTxVisibilityDrop = 176,    /* Tx: Visibility packets dropped. */
    bcmDbgCntRxMacControlDrop = 177,    /* Rx: Packets dropped due to MAC
                                           control frame. */
    bcmDbgCntRxProtocolErrorIP4 = 178,  /* Rx: Packets dropped due to IPv4
                                           protocol error. */
    bcmDbgCntRxProtocolErrorIP6 = 179,  /* Rx: Packets dropped due to IPv6
                                           protocol error. */
    bcmDbgCntRxLAGFailLoopback = 180,   /* Rx: LAG fail loopback packets in
                                           packet discard stage. */
    bcmDbgCntRxEcmpResolutionError = 181, /* Rx: Packets dropped due to ECMP
                                           resolution error. */
    bcmDbgCntRxPfmDrop = 182,           /* Rx: Packets dropped due to PFM. */
    bcmDbgCntRxTunnelError = 183,       /* Rx: Packets dropped due to tunnel
                                           errors. */
    bcmDbgCntRxBFDUnknownAchError = 184, /* Rx: BFD packets with non-zero version
                                           in the ACH header or the ACH channel
                                           type is unknown. */
    bcmDbgCntRxBFDUnknownControlFrames = 185, /* Rx: Unrecognized BFD control packets
                                           received from the PW VCCM type 1/2/3
                                           or MPLS-TP control channel. */
    bcmDbgCntRxBFDUnknownVersionDiscards = 186, /* Rx: Unknown BFD version or discard
                                           dropped. */
    bcmDbgCntRxDSL2HE = 187,            /* Rx: Dos Attack L2 Packets. */
    bcmDbgCntRxEgressMaskDrop = 188,    /* Rx: Packets dropped due to egress
                                           mask. */
    bcmDbgCntRxFieldRedirectMaskDrop = 189, /* Rx: Packets dropped due to FP
                                           redirection mask. */
    bcmDbgCntRxFieldDelayDrop = 190,    /* Rx: Packets dropped due to ingress FP
                                           delayed action. */
    bcmDbgCntRxEgressMaskSrcPortDrop = 191, /* Rx: Packets dropped due to block
                                           traffic from egress based on the
                                           ingress port. */
    bcmDbgCntRxMacBlockMaskDrop = 192,  /* Rx: Packets dropped due to MAC block
                                           mask. */
    bcmDbgCntRxMCError = 193,           /* Rx: Packets dropped due to multicast
                                           errors. */
    bcmDbgCntRxNonUnicastMaskDrop = 194, /* Rx: Packets dropped due to unknown
                                           unicast, unknown multicast, known
                                           multicast, and broadcast block masks. */
    bcmDbgCntRxNonUnicastLAGMaskDrop = 195, /* Rx: Packets dropped due to multicast
                                           and broadcast trunk block mask. */
    bcmDbgCntRxStgMaskDrop = 196,       /* Rx: Packets dropped due to spanning
                                           tree group state. */
    bcmDbgCntRxVlanBlockMaskDrop = 197, /* Rx: Packets dropped due to VLAN
                                           blocked ports. */
    bcmDbgCntRxEgressVlanMemberMismatch = 198, /* Rx: Packets dropped due to mismatched
                                           egress port for the VLAN. */
    bcmDbgCntTxCellTooSmall = 199,      /* Tx: Multi-cell packets whose SOP cell
                                           size is smaller than 64 bytes after
                                           decap. */
    bcmDbgCntTxLoopbackToLoopbackDrop = 200, /* Tx: Packets dropped due to loopback
                                           packet destination is also loopback
                                           port. */
    bcmDbgCntRxINTDataplaneProbeDrop = 201, /* Rx: INT (Inband Network Telemetry)
                                           dataplane probe packets dropped due
                                           to exception, including turn around
                                           and hop limit exceeded. */
    bcmDbgCntRxINTVectorMismatch = 202, /* Rx: INT (Inband Network Telemetry)
                                           packets dropped due to request vector
                                           match failure. */
    bcmDbgCntRxVfiAdaptLookupMiss = 203, /* Rx: Packets dropped due to the first
                                           lookup miss drop or both the first
                                           and the second lookup miss drop
                                           during ingress VFI adaption. */
    bcmDbgCntRxNextHopOrL3EifRangeError = 204, /* Rx: Packets dropped due to overlay
                                           and underlay next hop or L3 egress
                                           interface ID range error. */
    bcmDbgCntRxVxlanDecapSrcIpLookupMiss = 205, /* Rx: VXLAN packet dropped due to
                                           source IP lookup miss during tunnel
                                           decapsulation. */
    bcmDbgCntNum = 206                  /* Must be last one. */
} bcm_custom_stat_trigger_t;

/* Custom receive flags. */
#define BCM_DBG_CNT_RIPD4       0x00000001 /* Rx IPv4 L3 discard packets. */
#define BCM_DBG_CNT_RIPC4       0x00000002 /* Good rx L3 (V4 packets) includes
                                              tunneled. */
#define BCM_DBG_CNT_RIPHE4      0x00000004 /* Rx IPv4 header error packets. */
#define BCM_DBG_CNT_IMRP4       0x00000008 /* Routed IPv4 multicast packets. */
#define BCM_DBG_CNT_RIPD6       0x00000010 /* Rx IPv6 L3 discard packet. */
#define BCM_DBG_CNT_RIPC6       0x00000020 /* Good rx L3 (V6 packets) includes
                                              tunneled. */
#define BCM_DBG_CNT_RIPHE6      0x00000040 /* Rx IPv6 header error packets. */
#define BCM_DBG_CNT_IMRP6       0x00000080 /* Routed IPv6 multicast packets. */
#define BCM_DBG_CNT_RDISC       0x00000100 /* IBP discard and CBP full. */
#define BCM_DBG_CNT_RUC         0x00000200 /* Good rx unicast (L2+L3) packets. */
#define BCM_DBG_CNT_RPORTD      0x00000400 /* Packets droppped when ingress port
                                              is not in forwarding state. */
#define BCM_DBG_CNT_PDISC       0x00000800 /* Rx policy discard - DST_DISCARD,
                                              SRC_DISCARD, RATE_CONTROL... */
#define BCM_DBG_CNT_IMBP        0x00001000 /* Bridged multicast packets. */
#define BCM_DBG_CNT_RFILDR      0x00002000 /* Packets dropped by FP. */
#define BCM_DBG_CNT_RIMDR       0x00004000 /* Multicast (L2+L3) packets that are
                                              dropped. */
#define BCM_DBG_CNT_RDROP       0x00008000 /* Port bitmap zero drop condition. */
#define BCM_DBG_CNT_IRPSE       0x00010000 /* HiGig IPIC pause rx. */
#define BCM_DBG_CNT_IRHOL       0x00020000 /* HiGig End-to-End HOL rx packets
                                              (BCM56504/56700/56800/56580). */
#define BCM_DBG_CNT_IRIBP       0x00040000 /* HiGig End-to-End IBP rx packets
                                              (BCM56504/56700/56800/56580). */
#define BCM_DBG_CNT_DSL3HE      0x00080000 /* DOS L3 header error packets. */
#define BCM_DBG_CNT_IUNKHDR     0x00080000 /* Unknown HiGig header type packet
                                              (BCM56700/56800/56580). */
#define BCM_DBG_CNT_DSL4HE      0x00100000 /* DOS L4 header error packets. */
#define BCM_DBG_CNT_IMIRROR     0x00100000 /* HiGig mirror packet
                                              (BCM56700/56800/56580). */
#define BCM_DBG_CNT_DSICMP      0x00200000 /* DOS ICMP error packets. */
#define BCM_DBG_CNT_DSFRAG      0x00400000 /* DOS fragment error packets. */
#define BCM_DBG_CNT_MTUERR      0x00800000 /* Packets trapped to CPU due to
                                              egress L3 MTU violation. */
#define BCM_DBG_CNT_RTUN        0x01000000 /* Number of tunnel packets received. */
#define BCM_DBG_CNT_RTUNE       0x02000000 /* Rx tunnel error packets. */
#define BCM_DBG_CNT_VLANDR      0x04000000 /* Rx VLAN drops. */
#define BCM_DBG_CNT_RHGUC       0x08000000 /* Rx HiGig lookup UC cases (BCM56504
                                              B0 only). */
#define BCM_DBG_CNT_RHGMC       0x10000000 /* Rx HiGig lookup MC cases (BCM56504
                                              B0 only). */
#define BCM_DBG_CNT_MPLS        0x20000000 /* Received MPLS Packets (BCM5660x
                                              only). */
#define BCM_DBG_CNT_MACLMT      0x20000000 /* packets dropped due to MAC Limit
                                              is hit. */
#define BCM_DBG_CNT_MPLSERR     0x40000000 /* Received MPLS Packets with Error
                                              (BCM5660x only). */
#define BCM_DBG_CNT_URPFERR     0x80000000 /* L3 src URPF check Fail (BCM5660x
                                              only). */
#define BCM_DBG_CNT_HGHDRE      0x08000000 /* HiGig Header error packets
                                              (BCM56700/56800/56580). */
#define BCM_DBG_CNT_MCIDXE      0x10000000 /* Multicast Index error packets
                                              (BCM56700/56800/56580). */
#define BCM_DBG_CNT_LAGLUP      0x20000000 /* LAG failover loopback packets
                                              (BCM56700/56800/56580). */
#define BCM_DBG_CNT_LAGLUPD     0x40000000 /* LAG backup port down
                                              (BCM56700/56800/56580). */
#define BCM_DBG_CNT_PARITYD     0x80000000 /* Parity error packets
                                              (BCM56700/56800/56580). */
#define BCM_FB_DBG_CNT_RMASK    0x07ffffff /* Internal. */
#define BCM_RP_DBG_CNT_RMASK    0x27ffffff /* Internal. */
#define BCM_FB_B0_DBG_CNT_RMASK 0x1fffffff /* Internal. */
#define BCM_HB_DBG_CNT_RMASK    0xffffffff /* Internal. */

/* Custom transmit flags. */
#define BCM_DBG_CNT_TGIP4               0x00000001 /* Tx Good IPv4 L3 UC
                                                      packets. */
#define BCM_DBG_CNT_TIPD4               0x00000002 /* Tx IPv4 L3 UC Aged and
                                                      Drop packets. */
#define BCM_DBG_CNT_TGIPMC4             0x00000004 /* Tx Good IPv4 IPMC packets. */
#define BCM_DBG_CNT_TIPMCD4             0x00000008 /* Tx IPv4 IPMC Aged and Drop
                                                      packets. */
#define BCM_DBG_CNT_TGIP6               0x00000010 /* Tx Good IPv6 L3 UC
                                                      packets. */
#define BCM_DBG_CNT_TIPD6               0x00000020 /* Tx IPv6 L3 UC Aged and
                                                      Drop Packets. */
#define BCM_DBG_CNT_TGIPMC6             0x00000040 /* Tx Good IPv6 IPMC packets. */
#define BCM_DBG_CNT_TIPMCD6             0x00000080 /* Tx IPv6 IPMC Aged and Drop
                                                      packets. */
#define BCM_DBG_CNT_TTNL                0x00000100 /* Tx Tunnel packets. */
#define BCM_DBG_CNT_TTNLE               0x00000200 /* Tx Tunnel error packets. */
#define BCM_DBG_CNT_TTTLD               0x00000400 /* Pkts dropped due to TTL
                                                      threshold counter. */
#define BCM_DBG_CNT_TCFID               0x00000800 /* Pkts dropped when CFI set
                                                      & pket is untagged or L3
                                                      switched for IPMC. */
#define BCM_DBG_CNT_TVLAN               0x00001000 /* Tx VLAN tagged packets. */
#define BCM_DBG_CNT_TVLAND              0x00002000 /* Pkts dropped due to
                                                      invalid VLAN counter. */
#define BCM_DBG_CNT_TVXLTMD             0x00004000 /* Pkts dropped due to miss
                                                      in VXLT table counter. */
#define BCM_DBG_CNT_TSTGD               0x00008000 /* Pkts dropped due to
                                                      Spanning Tree State not in
                                                      forwarding state. */
#define BCM_DBG_CNT_TAGED               0x00010000 /* Pkts dropped due to packet
                                                      aged counter. */
#define BCM_DBG_CNT_TL2MCD              0x00020000 /* L2 MC packet drop counter. */
#define BCM_DBG_CNT_TPKTD               0x00040000 /* Pkts dropped due to any
                                                      condition. */
#define BCM_DBG_CNT_TMIRR               0x00080000 /* mirroring flag. */
#define BCM_DBG_CNT_TSIPL               0x00100000 /* SIP Link Local Drop flag
                                                      (BCM56504/56700/56800/56580). */
#define BCM_DBG_CNT_THGUC               0x00200000 /* HiGig Lookedup L3UC Pkts
                                                      (BCM56504_B0/56700/56800/56580). */
#define BCM_DBG_CNT_THGMC               0x00400000 /* HiGig Lookedup L3MC Pkts
                                                      (BCM56504_B0/56700/56800/56580). */
#define BCM_DBG_CNT_THIGIG2             0x00800000 /* Unknown HiGig2 Drop
                                                      (BCM56700/56800/56580). */
#define BCM_DBG_CNT_THGI                0x01000000 /* Unknown HiGig drop
                                                      (BCM56700/56800/56580). */
#define BCM_DBG_CNT_TL2_MTU             0x02000000 /* L2 MTU fail drop
                                                      (BCM56700/56800/56580). */
#define BCM_DBG_CNT_TPARITY_ERR         0x04000000 /* Parity Error drop
                                                      (BCM56700/56800/56580). */
#define BCM_DBG_CNT_TIP_LEN_FAIL        0x08000000 /* IP Length check fail drop
                                                      (BCM56700/56800/56580). */
#define BCM_DBG_CNT_TMTUD               0x01000000 /* EBAD_MTU_DROP (BCM5660x
                                                      only). */
#define BCM_DBG_CNT_TSLLD               0x10000000 /* ESIP_LINK_LOCAL (BCM5660x
                                                      only). */
#define BCM_DBG_CNT_TL2MPLS             0x20000000 /* L2_MPLS_ENCAP_TX (BCM5660x
                                                      only). */
#define BCM_DBG_CNT_TL3MPLS             0x40000000 /* L3_MPLS_ENCAP_TX (BCM5660x
                                                      only). */
#define BCM_DBG_CNT_TMPLS               0x80000000 /* MPLS_TX (BCM5660x only). */
#define BCM_DBG_CNT_MODIDTOOLARGEDROP   0x10000000 /* MODID greater than 31 drop
                                                      counter. */
#define BCM_DBG_CNT_PKTMODTOOLARGEDROP  0x20000000 /* Byte additions too large
                                                      drop counter. */
#define BCM_FB_DBG_CNT_TMASK            0x001fffff /* Internal. */
#define BCM_RP_DBG_CNT_TMASK            0x001fffff /* Internal. */
#define BCM_FB_B0_DBG_CNT_TMASK         0x007fffff /* Internal. */
#define BCM_HB_DBG_CNT_TMASK            0x0fffffff /* Internal. */
#define BCM_TRX_DBG_CNT_TMASK           0x3fffffff /* Internal. */

#ifndef BCM_HIDE_DISPATCHABLE

/* Initialize the BCM statistics module. */
extern int bcm_stat_init(
    int unit);

#endif /* BCM_HIDE_DISPATCHABLE */

#define bcm_stat_enable_set(unit, port, enable)             /* Gone */
#define bcm_stat_enable_get(unit, port, enable_ptr)             /* Gone */

#ifndef BCM_HIDE_DISPATCHABLE

/* Clear the port-based statistics for the indicated device port. */
extern int bcm_stat_clear(
    int unit, 
    bcm_port_t port);

/* Synchronize software counters with hardware. */
extern int bcm_stat_sync(
    int unit);

/* Get the specified statistics from the device. */
extern int bcm_stat_get(
    int unit, 
    bcm_port_t port, 
    bcm_stat_val_t type, 
    uint64 *value);

/* Get the specified statistics from the device. */
extern int bcm_stat_get32(
    int unit, 
    bcm_port_t port, 
    bcm_stat_val_t type, 
    uint32 *value);

/* Get the specified statistics from the device. */
extern int bcm_stat_multi_get(
    int unit, 
    bcm_port_t port, 
    int nstat, 
    bcm_stat_val_t *stat_arr, 
    uint64 *value_arr);

/* Get the specified statistics from the device. */
extern int bcm_stat_multi_get32(
    int unit, 
    bcm_port_t port, 
    int nstat, 
    bcm_stat_val_t *stat_arr, 
    uint32 *value_arr);

/* 
 * Force an immediate counter update and get statistics for a specified
 * device.
 *  Note: If bcm_stat_sync_get was continuously called for a counter, the
 * corresponding
 * rate calculated with CLI "show counter" will be incorrect.
 */
extern int bcm_stat_sync_get(
    int unit, 
    bcm_port_t port, 
    bcm_stat_val_t type, 
    uint64 *value);

/* 
 * Force an immediate counter update and get statistics for a specified
 * device.
 *  Note: If bcm_stat_sync_get was continuously called for a counter, the
 * corresponding
 * rate calculated with CLI "show counter" will be incorrect.
 */
extern int bcm_stat_sync_get32(
    int unit, 
    bcm_port_t port, 
    bcm_stat_val_t type, 
    uint32 *value);

/* 
 * Force an immediate counter update and get a set of statistics for a
 * specified device.
 */
extern int bcm_stat_sync_multi_get(
    int unit, 
    bcm_port_t port, 
    int nstat, 
    bcm_stat_val_t *stat_arr, 
    uint64 *value_arr);

/* 
 * Force an immediate counter update and get a set of statistics for a
 * specified device.
 */
extern int bcm_stat_sync_multi_get32(
    int unit, 
    bcm_port_t port, 
    int nstat, 
    bcm_stat_val_t *stat_arr, 
    uint32 *value_arr);

/* 
 * Get hardware statistics for a specified device without counter update.
 *  Note: Not all statistics types are supported and implemented.
 * Currently types snmpBcmRxPFCFramePriority0-7 and
 * snmpBcmRxPFCFrameXonPriority0-7
 * are implemented.
 */
extern int bcm_stat_direct_get(
    int unit, 
    bcm_port_t port, 
    bcm_stat_val_t type, 
    uint64 *value);

/* 
 * Get a set of hardware statistics for a specified device without
 * counter update.
 */
extern int bcm_stat_direct_multi_get(
    int unit, 
    bcm_port_t port, 
    int nstat, 
    bcm_stat_val_t *stat_arr, 
    uint64 *value_arr);

/* Set/get debug counter to count certain packet types. */
extern int bcm_stat_custom_set(
    int unit, 
    bcm_port_t port, 
    bcm_stat_val_t type, 
    uint32 flags);

/* Set/get debug counter to count certain packet types. */
extern int bcm_stat_custom_get(
    int unit, 
    bcm_port_t port, 
    bcm_stat_val_t type, 
    uint32 *flags);

/* Add a certain packet type to debug counter to count. */
extern int bcm_stat_custom_add(
    int unit, 
    bcm_port_t port, 
    bcm_stat_val_t type, 
    bcm_custom_stat_trigger_t trigger);

/* Delete the specified packet type from debug counter. */
extern int bcm_stat_custom_delete(
    int unit, 
    bcm_port_t port, 
    bcm_stat_val_t type, 
    bcm_custom_stat_trigger_t trigger);

/* Delete all packet types from debug counter. */
extern int bcm_stat_custom_delete_all(
    int unit, 
    bcm_port_t port, 
    bcm_stat_val_t type);

/* Check if the specified packet type is part of a given debug counter. */
extern int bcm_stat_custom_check(
    int unit, 
    bcm_port_t port, 
    bcm_stat_val_t type, 
    bcm_custom_stat_trigger_t trigger, 
    int *result);

/* Get the dedicated counter associated with a given drop event. */
extern int bcm_stat_drop_event_counter_get(
    int unit, 
    bcm_pkt_drop_event_t type, 
    uint64 *value);

/* 
 * Get the dedicated counter associated with a given drop event with sync
 * mode.
 */
extern int bcm_stat_drop_event_counter_sync_get(
    int unit, 
    bcm_pkt_drop_event_t type, 
    uint64 *value);

/* Clear the dedicated counter associated a the given drop event. */
extern int bcm_stat_drop_event_counter_set(
    int unit, 
    bcm_pkt_drop_event_t type, 
    uint64 value);

/* Get the dedicated counter associated with a given trace event. */
extern int bcm_stat_trace_event_counter_get(
    int unit, 
    bcm_pkt_trace_event_t type, 
    uint64 *value);

/* 
 * Get the dedicated counter associated with a given trace event with
 * sync mode.
 */
extern int bcm_stat_trace_event_counter_sync_get(
    int unit, 
    bcm_pkt_trace_event_t type, 
    uint64 *value);

/* Clear the dedicated counter associated with a given trace event. */
extern int bcm_stat_trace_event_counter_set(
    int unit, 
    bcm_pkt_trace_event_t type, 
    uint64 value);

#endif /* BCM_HIDE_DISPATCHABLE */

/* TCP flags related flex attributes values */
typedef enum bcm_stat_group_mode_attr_tcp_flags_e {
    bcmStatGroupModeAttrTcpFlagsTypeSyn = 0, /* Synchronisation flag */
    bcmStatGroupModeAttrTcpFlagsTypeFin = 1, /* Finish flag */
    bcmStatGroupModeAttrTcpFlagsTypeRst = 2, /* Reset flag */
    bcmStatGroupModeAttrTcpFlagsTypePsh = 3, /* Push flag */
    bcmStatGroupModeAttrTcpFlagsTypeAck = 4, /* Acknowledgement flag */
    bcmStatGroupModeAttrTcpFlagsTypeUrg = 5, /* Urgent flag */
    bcmStatGroupModeAttrTcpFlagsTypeEce = 6, /* ECN capable flag */
    bcmStatGroupModeAttrTcpFlagsTypeCwr = 7 /* Congestion Window Reduced flag */
} bcm_stat_group_mode_attr_tcp_flags_t;

/* FC Type related flex attribute values. */
typedef enum bcm_stat_group_mode_attr_flowcontrolpkt_type_e {
    bcmStatGroupModeAttrFlowControlPktTypeData = 0, /* Data */
    bcmStatGroupModeAttrFlowControlPktTypePause = 1, /* Pause */
    bcmStatGroupModeAttrFlowControlPktTypePfc = 2, /* PFC */
    bcmStatGroupModeAttrFlowControlPktTypeE2ecc = 3 /* E2ECC */
} bcm_stat_group_mode_attr_flowcontrolpkt_type_t;

/* Vlan related flex attributes values */
typedef enum bcm_stat_group_mode_attr_vlan_e {
    bcmStatGroupModeAttrVlanUnTagged = 1, /* UnTagged Vlan */
    bcmStatGroupModeAttrVlanInnerTag = 2, /* Inner Tagged Vlan */
    bcmStatGroupModeAttrVlanOuterTag = 3, /* Outer Tagged Vlan */
    bcmStatGroupModeAttrVlanStackedTag = 4, /* Both Inner and Outer Tagged Vlan */
    bcmStatGroupModeAttrVlanAll = 5     /* UnTagged,Inner and Outer Tagged Vlan */
} bcm_stat_group_mode_attr_vlan_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Clear specific statistics for a port from the device */
extern int bcm_stat_clear_single(
    int unit, 
    bcm_port_t port, 
    bcm_stat_val_t type);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Packet type related flex attributes values */
typedef enum bcm_stat_group_mode_attr_pkt_type_e {
    bcmStatGroupModeAttrPktTypeAll = 1, /* All Packet Types */
    bcmStatGroupModeAttrPktTypeUnknown = 2, /* Unknown Packet */
    bcmStatGroupModeAttrPktTypeControl = 3, /* Control Packet */
    bcmStatGroupModeAttrPktTypeOAM = 4, /* OAM Packet */
    bcmStatGroupModeAttrPktTypeBFD = 5, /* BFD Packet */
    bcmStatGroupModeAttrPktTypeBPDU = 6, /* BPDU Packet */
    bcmStatGroupModeAttrPktTypeICNM = 7, /* ICNM Packet */
    bcmStatGroupModeAttrPktType1588 = 8, /* 1588 Packet */
    bcmStatGroupModeAttrPktTypeKnownL2UC = 9, /* Known L2 Unicast Packet */
    bcmStatGroupModeAttrPktTypeUnknownL2UC = 10, /* Unknown L2 Unicast Packet */
    bcmStatGroupModeAttrPktTypeL2BC = 11, /* L2 Broadcast Packet */
    bcmStatGroupModeAttrPktTypeKnownL2MC = 12, /* Known L2 Multicast Packet */
    bcmStatGroupModeAttrPktTypeUnknownL2MC = 13, /* Unknown L2 Multicast Packet */
    bcmStatGroupModeAttrPktTypeKnownL3UC = 14, /* Known L3 Unicast Packet */
    bcmStatGroupModeAttrPktTypeUnknownL3UC = 15, /* Unknown L3 Unicast Packet */
    bcmStatGroupModeAttrPktTypeKnownIPMC = 16, /* Known IPMC Packet */
    bcmStatGroupModeAttrPktTypeUnknownIPMC = 17, /* Unknown IPMC Packet */
    bcmStatGroupModeAttrPktTypeKnownMplsL2 = 18, /* Known MPLS L2 Packet */
    bcmStatGroupModeAttrPktTypeKnownMplsL3 = 19, /* Known MPLS L3 Packet */
    bcmStatGroupModeAttrPktTypeKnownMpls = 20, /* Known MPLS Packet */
    bcmStatGroupModeAttrPktTypeUnknownMpls = 21, /* Unknown MPLS Packet */
    bcmStatGroupModeAttrPktTypeKnownMplsMulticast = 22, /* Known MPLS Multicast Packet */
    bcmStatGroupModeAttrPktTypeKnownMim = 23, /* Known MiM Packet */
    bcmStatGroupModeAttrPktTypeUnknownMim = 24, /* Unknown MiM Packet */
    bcmStatGroupModeAttrPktTypeKnownTrill = 25, /* Known TRILL Packet */
    bcmStatGroupModeAttrPktTypeUnknownTrill = 26, /* Unknown TRILL Packet */
    bcmStatGroupModeAttrPktTypeKnownNiv = 27, /* Known  NIV Packet */
    bcmStatGroupModeAttrPktTypeUnknownNiv = 28, /* Unknown  NIV Packet */
    bcmStatGroupModeAttrPktTypeL2UC = 29, /* Unicast L2 Packet */
    bcmStatGroupModeAttrPktTypeL2NonUC = 30 /* Non Unicast L2 Packet */
} bcm_stat_group_mode_attr_pkt_type_t;

/* Stat Group Mode Attribute Selectors */
typedef enum bcm_stat_group_mode_attr_e {
    bcmStatGroupModeAttrColor = 1,      /* Color Selector: Possible
                                           Value:bcmColorGreen|Yellow|REd or
                                           oxFFFFFFFF */
    bcmStatGroupModeAttrFieldIngressColor = 2, /* Field Ingress Color Selector:
                                           Possible
                                           Values:bcmColorGreen|Yellow|REd or
                                           STAT_GROUP_MODE_ATTR_ALL_VALUES for
                                           all */
    bcmStatGroupModeAttrIntPri = 3,     /* Internal Priority Selector: Possible
                                           Values: 0 to 15 or
                                           STAT_GROUP_MODE_ATTR_ALL_VALUES for
                                           all */
    bcmStatGroupModeAttrVlan = 4,       /* Vlan Type Selector: Possible Values:
                                           bcmStatGroupModeVlanAttr */
    bcmStatGroupModeAttrOuterPri = 5,   /* Outer Vlan Priority Selector:
                                           Possible Values: 0 to 7 or
                                           STAT_GROUP_MODE_ATTR_ALL_VALUES for
                                           all */
    bcmStatGroupModeAttrInnerPri = 6,   /* Inner Vlan Priority Selector:
                                           Possible Values: 0 to 7 or
                                           STAT_GROUP_MODE_ATTR_ALL_VALUES for
                                           all */
    bcmStatGroupModeAttrPort = 7,       /* Logical Port Selector: Possible
                                           Values:<MaxLogicalPort> or
                                           STAT_GROUP_MODE_ATTR_ALL_VALUES for
                                           all */
    bcmStatGroupModeAttrTosDscp = 8,    /* Type Of Service Selector(DSCP :
                                           Differentiated services Code Point):
                                           Possible Values:<6b:TOS Val> or
                                           STAT_GROUP_MODE_ATTR_ALL_VALUES for
                                           all */
    bcmStatGroupModeAttrTosEcn = 9,     /* Type Of Service Selector(ECN:
                                           Explicit Congestion Notification):
                                           Possible Values:<2b:TOS Val> or
                                           STAT_GROUP_MODE_ATTR_ALL_VALUES for
                                           all */
    bcmStatGroupModeAttrPktType = 10,   /* Packet Type Selector: Possible
                                           Values:<bcmStatGroupModeAttrPktType*> */
    bcmStatGroupModeAttrIngNetworkGroup = 11, /* Ingress Network Group Selector:
                                           Possible Values:<Value> or
                                           STAT_GROUP_MODE_ATTR_ALL_VALUES for
                                           all */
    bcmStatGroupModeAttrEgrNetworkGroup = 12, /* Egress Network Group Selector:
                                           Possible Values:<Value> or
                                           STAT_GROUP_MODE_ATTR_ALL_VALUES for
                                           all */
    bcmStatGroupModeAttrDrop = 13,      /* Drop Selector: Possible Values:<0 or
                                           1> */
    bcmStatGroupModeAttrPacketTypeIp = 14, /* Ip Packet Selector: Possible
                                           Values:<0 or 1> */
    bcmStatGroupModeAttrColorMplsLabelFirst = 15, /* Color Selector derived from first
                                           Mpls label: Possible
                                           Value:bcmColorGreen|Yellow|REd or
                                           oxFFFFFFFF */
    bcmStatGroupModeAttrIntPriMplsLabelFirst = 16, /* Internal Priority Selector derived
                                           from first Mpls label: Possible
                                           Values: 0 to 15 or
                                           STAT_GROUP_MODE_ATTR_ALL_VALUES for
                                           all */
    bcmStatGroupModeAttrColorMplsLabelSecond = 17, /* Color Selector derived from second
                                           Mpls label: Possible
                                           Value:bcmColorGreen|Yellow|REd or
                                           oxFFFFFFFF */
    bcmStatGroupModeAttrIntPriMplsLabelSecond = 18, /* Internal Priority Selector derived
                                           from second Mpls label: Possible
                                           Values: 0 to 15 or
                                           STAT_GROUP_MODE_ATTR_ALL_VALUES for
                                           all */
    bcmStatGroupModeAttrColorMplsLabelThird = 19, /* Color Selector derived from third
                                           Mpls label: Possible
                                           Value:bcmColorGreen|Yellow|REd or
                                           oxFFFFFFFF */
    bcmStatGroupModeAttrIntPriMplsLabelThird = 20, /* Internal Priority Selector derived
                                           from third Mpls label: Possible
                                           Values: 0 to 15 or
                                           STAT_GROUP_MODE_ATTR_ALL_VALUES for
                                           all */
    bcmStatGroupModeAttrPktCfiPri = 21, /* Vlan tag CFI, PRI Selector: Possible
                                           Values: 0 to 15 or
                                           STAT_GROUP_MODE_ATTR_ALL_VALUES for
                                           all */
    bcmStatGroupModeAttrUdf = 22,       /* UDF. Possible Values:< max 8b valid
                                           bit in UDF Val> or <max 7b valid bit
                                           in UDF Val> when AttrDrop is enabled */
    bcmStatGroupModeAttrCos = 23,       /* MMU cos Selector: Possible Values: 0
                                           to 15 or
                                           STAT_GROUP_MODE_ATTR_ALL_VALUES for
                                           all */
    bcmStatGroupModeAttrQueueType = 24, /* Queue Type Selector: Possible Values:
                                           0 for MC queue, or 1 for UC queue or
                                           STAT_GROUP_MODE_ATTR_ALL_VALUES for
                                           all */
    bcmStatGroupModeAttrCongestionMarked = 25, /* Congestion marked status Selector:
                                           Possible Values: 0 to 1 or
                                           STAT_GROUP_MODE_ATTR_ALL_VALUES for
                                           all */
    bcmStatGroupModeAttrClassType = 26, /* User defined class to identify a set
                                           of attributes combined together */
    bcmStatGroupModeAttrTcpFlagsType = 27, /* TCP flags selector derived from TCP
                                           flags field in the packet */
    bcmStatGroupModeAttrIntCongestion = 28, /* Internal Congestion selector derived
                                           form ECN to internal congestion
                                           mapped value */
    bcmStatGroupModeAttrFlowControlPktType = 29, /* Indicates type of Flow Control Packet */
    bcmStatGroupModePortGroup = 30,     /* User defined group to identify a set
                                           of ports */
    bcmStatGroupModeAttrMaxValue = 31   /* This should be the maximum value
                                           defined in this enum */
} bcm_stat_group_mode_attr_t;

/* Unique constants */
#define BCM_STAT_GROUP_MODE_ATTR_ALL_VALUES 0xFFFFFFFF /* A unique constant for
                                                          using all attr values */

/* Stat Group Mode Flags */
#define BCM_STAT_GROUP_MODE_INGRESS         0x00000001 /* Stat Group Mode
                                                          Ingress */
#define BCM_STAT_GROUP_MODE_EGRESS          0x00000002 /* Stat Group Mode Egress */
#define BCM_STAT_GROUP_MODE_CAPABILITY_OAM  0x00000004 /* Stat Group Mode With
                                                          OAM Capability */
#define BCM_STAT_GROUP_MODE_UDF             0x00000008 /* Stat Group Mode UDF */

/* Ingress and Egress Statistics Accounting Objects */
typedef enum bcm_stat_object_e {
    bcmStatObjectIngPort = 0,           /* Ingress Port Object */
    bcmStatObjectIngVlan = 1,           /* Ingress Vlan Object */
    bcmStatObjectIngVlanXlate = 2,      /* Ingress Vlan Translate Object */
    bcmStatObjectIngVfi = 3,            /* Ingress VFI Object */
    bcmStatObjectIngL3Intf = 4,         /* Ingress L3 Interface Object */
    bcmStatObjectIngVrf = 5,            /* Ingress VRF Object */
    bcmStatObjectIngPolicy = 6,         /* Ingress Policy Object */
    bcmStatObjectIngFieldStageLookup = 6, /* Ingress VFP Object */
    bcmStatObjectIngMplsVcLabel = 7,    /* Ingress MPLS VC Label Object */
    bcmStatObjectIngMplsSwitchLabel = 8, /* Ingress MPLS Switch Label Object */
    bcmStatObjectEgrPort = 9,           /* Egress Port Object */
    bcmStatObjectEgrVlan = 10,          /* Egress Vlan Object */
    bcmStatObjectEgrVlanXlate = 11,     /* Egress Vlan Translate Object */
    bcmStatObjectEgrVfi = 12,           /* Egress VFI Object */
    bcmStatObjectEgrL3Intf = 13,        /* Egress L3 Interface Object */
    bcmStatObjectIngMplsFrrLabel = 14,  /* Ingress MPLS Fast Reroute Label
                                           Object */
    bcmStatObjectIngL3Host = 15,        /* L3 Host without L3 Egress Object */
    bcmStatObjectIngTrill = 16,         /* Ingress Trill Object */
    bcmStatObjectIngMimLookupId = 17,   /* Ingress MiM I-SID Object */
    bcmStatObjectIngL2Gre = 18,         /* Ingress L2 GRE Object */
    bcmStatObjectIngEXTPolicy = 19,     /* Ingress external FP Object */
    bcmStatObjectIngFieldStageExternal = 19, /* Ingress external FP Object */
    bcmStatObjectEgrWlan = 20,          /* Egress WLAN Object */
    bcmStatObjectEgrMim = 21,           /* Egress MiM Object */
    bcmStatObjectEgrMimLookupId = 22,   /* Egress MiM I-SID Object */
    bcmStatObjectEgrL2Gre = 23,         /* Egress L2 GRE Object */
    bcmStatObjectIngVxlan = 24,         /* Ingress Vxlan Object */
    bcmStatObjectIngVsan = 25,          /* Ingress FCOE VSAN Object */
    bcmStatObjectIngFcoe = 26,          /* Ingress FCOE Object */
    bcmStatObjectIngL3Route = 27,       /* Ingress L2 Route Defip Object */
    bcmStatObjectEgrVxlan = 28,         /* Egress Vxlan Object */
    bcmStatObjectEgrL3Nat = 29,         /* Egress L3 NAT Object */
    bcmStatObjectIngNiv = 30,           /* Ingress Niv Object */
    bcmStatObjectIngIpmc = 32,          /* Ingress IPMC Object */
    bcmStatObjectEgrNiv = 31,           /* Egress Niv Object */
    bcmStatObjectIngVxlanDip = 33,      /* Ingress Vxlan Dip Object */
    bcmStatObjectIngFieldStageIngress = 34, /* Ingress FP Object */
    bcmStatObjectEgrFieldStageEgress = 35, /* Egress FP Object */
    bcmStatObjectEgrMplsTunnelLabel = 36, /* Egress MPLS Tunnel Label */
    bcmStatObjectIngExactMatch = 37,    /* Exact Match FP Object */
    bcmStatObjectIngGport = 38,         /* Ingress GPort Object */
    bcmStatObjectIngVlanXlateSecondLookup = 39, /* Ingress Vlan Translate Second Lookup
                                           Object */
    bcmStatObjectEgrVlanXlateSecondLookup = 40, /* Egress Vlan Translate Second Lookup
                                           Object */
    bcmStatObjectIngMplsSwitchSecondLabel = 41, /* Mpls Entry Lookup object for the
                                           Second Label */
    bcmStatObjectEgrMplsTunnelSecondLabel = 42, /* Egress Mpls Tunnel Second Label
                                           Object */
    bcmStatObjectEgrDvpAttr = 43,       /* Egress DVP Attribute table object */
    bcmStatObjectMaxValue = 44          /* This should be the maximum value
                                           defined in this enum */
} bcm_stat_object_t;

#define BCM_STAT_OBJECT \
{ \
    "IngPort", \
    "IngVlan", \
    "IngVlanXlate", \
    "IngVfi", \
    "IngL3Intf", \
    "IngVrf", \
    "IngPolicy", \
    "IngMplsVcLabel", \
    "IngMplsSwitchLabel", \
    "EgrPort", \
    "EgrVlan", \
    "EgrVlanXlate", \
    "EgrVfi", \
    "EgrL3Intf", \
    "IngMplsFrrLabel", \
    "IngL3Host", \
    "IngTrill", \
    "IngMimLookupId", \
    "IngL2Gre", \
    "IngEXTPolicy", \
    "EgrWlan", \
    "EgrMim", \
    "EgrMimLookupId", \
    "EgrL2Gre", \
    "IngVxlan", \
    "IngVsan", \
    "IngFcoe", \
    "IngL3Route", \
    "EgrVxlan", \
    "EgrL3Nat", \
    "IngNiv", \
    "IngIpmc", \
    "EgrNiv", \
    "IngVxlanDip", \
    "IngFieldStageIngress", \
    "EgrFieldStageEgress", \
    "EgrMplsTunnelLabel", \
    "IngExactMatch", \
    "IngGport", \
    "IngVlanXlateSecondLookup", \
    "EgrVlanXlateSecondLookup", \
    "IngMplsSwitchSecondLabel", \
    "EgrMplsTunnelSecondLabel", \
    "EgrDvpAttr", \
    "MaxValue"  \
}

/* Flex stat pool direction */
typedef enum bcm_stat_flex_direction_e {
    bcmStatFlexDirectionIngress = 0,    /* Ingress direction */
    bcmStatFlexDirectionEgress = 1,     /* Egress direction */
    bcmStatFlexDirectionCount = 2       /* Max value defined for this enum */
} bcm_stat_flex_direction_t;

/* BCM Header types */
typedef enum bcm_stat_pp_metadata_header_type_e {
    bcmStatPpMetadataHeaderTypeEth = 0, 
    bcmStatPpMetadataHeaderTypeIpv4 = 1, 
    bcmStatPpMetadataHeaderTypeIpv6 = 2, 
    bcmStatPpMetadataHeaderTypeMpls = 3, 
    bcmStatPpMetadataHeaderTypeArp = 4, 
    bcmStatPpMetadataHeaderTypeFcoe = 5, 
    bcmStatPpMetadataHeaderTypeSrv6 = 6, 
    bcmStatPpMetadataHeaderTypeTcp = 7, 
    bcmStatPpMetadataHeaderTypeUdp = 8, 
    bcmStatPpMetadataHeaderTypeBfd = 9, 
    bcmStatPpMetadataHeaderTypeIcmp = 10, 
    bcmStatPpMetadataHeaderTypeOthers = 11, 
    bcmStatPpMetadataHeaderTypeCount = 12 
} bcm_stat_pp_metadata_header_type_t;

/* Defines the IFP specific parameters. */
typedef enum bcm_stat_group_mode_hint_type_e {
    bcmStatGroupAllocHintIngressFieldGroup = 1, 
    bcmStatGroupAllocHintVlanFieldGroup = 2, 
    bcmStatGroupAllocHintExactMatchFieldGroup = 3, 
    bcmStatGroupAllocHintEgressFieldGroup = 4, 
    bcmStatGroupAllocHintCount = 5      /* Max value defined for this enum */
} bcm_stat_group_mode_hint_type_t;

/* Flex pool statistics information */
typedef struct bcm_stat_flex_pool_stat_info_s {
    uint32 pool_id;                     /* pool ID */
    uint32 total_entries;               /* The size of this pool */
    uint32 used_entries;                /* Entries that are allocated (but not
                                           necessarily assigned) */
    uint32 attached_entries;            /* he number of used_entries that are
                                           actually assigned to an object */
    uint32 free_entries;                /* The result of deducting used_entries
                                           from total_entries */
    SHR_BITDCL used_by_objects[_SHR_BITDCLSIZE(bcmStatObjectMaxValue)]; 
} bcm_stat_flex_pool_stat_info_t;

/* PP Metadata information */
typedef struct bcm_stat_pp_metadata_info_s {
    uint32 flags;                       /* See BCM_STAT_PP_METADATA_XXX */
    bcm_stat_pp_metadata_header_type_t header_type; /* Header type of the packet */
    uint32 address_type_flags;          /* See
                                           BCM_STAT_PP_METADATA_ADDRESS_TYPE_XXX */
    uint32 metadata;                    /* The metadata value to use */
} bcm_stat_pp_metadata_info_t;

/* Stat Flex Group Attribute Selector */
typedef struct bcm_stat_group_mode_attr_selector_s {
    uint32 counter_offset;              /* Counter Offset */
    bcm_stat_group_mode_attr_t attr;    /* Attribute Selector */
    uint32 attr_value;                  /* Attribute Values */
    uint32 udf_bitmap;                  /* valid UDF bits, should be same in one
                                           mode id */
} bcm_stat_group_mode_attr_selector_t;

/* Stat Flex Group Hint */
typedef struct bcm_stat_group_mode_hint_s {
    bcm_stat_group_mode_hint_type_t type; /* Hint type */
    uint32 value;                       /* Hint value */
} bcm_stat_group_mode_hint_t;

/* Stat Flex Group Attribute Selector */
typedef struct bcm_stat_group_mode_id_config_s {
    uint32 flags;                       /* Counter Mode Flags */
    uint32 total_counters;              /* Total Number of Counters */
    bcm_stat_group_mode_hint_t hint;    /* Stat Hint */
} bcm_stat_group_mode_id_config_t;

/* Counter Statistics Values */
typedef struct bcm_stat_value_s {
    uint32 packets;     /* packets value */
    uint64 bytes;       /* bytes value */
    uint64 packets64;   /* 64-bit accumulated packets value */
} bcm_stat_value_t;

/* Statistics Group Modes */
typedef enum bcm_stat_group_mode_e {
    bcmStatGroupModeSingle = 0,         /* A single counter used for all traffic
                                           types */
    bcmStatGroupModeTrafficType = 1,    /* A dedicated counter per traffic type
                                           Unicast, multicast, broadcast */
    bcmStatGroupModeDlfAll = 2,         /* A pair of counters where the base
                                           counter is used for dlf and the other
                                           counter is used for all traffic types */
    bcmStatGroupModeDlfIntPri = 3,      /* N+1 counters where the base counter
                                           is used for dlf and next N are used
                                           per Cos */
    bcmStatGroupModeTyped = 4,          /* A dedicated counter for unknown
                                           unicast, known unicast, multicast,
                                           broadcast */
    bcmStatGroupModeTypedAll = 5,       /* A dedicated counter for unknown
                                           unicast, known unicast, multicast,
                                           broadcast and one for all traffic(not
                                           already counted) */
    bcmStatGroupModeTypedIntPri = 6,    /* A dedicated counter for unknown
                                           unicast, known unicast,
                                           multicast,broadcast and N internal
                                           priority counters for traffic (not
                                           already counted) */
    bcmStatGroupModeSingleWithControl = 7, /* A single counter used for all traffic
                                           types with an additional counter for
                                           control traffic */
    bcmStatGroupModeTrafficTypeWithControl = 8, /* A dedicated counter per traffic type
                                           unicast, multicast, broadcast with an
                                           additional counter for control
                                           traffic */
    bcmStatGroupModeDlfAllWithControl = 9, /* A pair of counters where the base
                                           counter is used for control, the next
                                           one for dlf and the other counter is
                                           used for all traffic types */
    bcmStatGroupModeDlfIntPriWithControl = 10, /* N+2 counters where the base counter
                                           is used for control, the next one for
                                           dlf and next N are used per Cos */
    bcmStatGroupModeTypedWithControl = 11, /* A dedicated counter for control,
                                           unknown unicast, known unicast,
                                           multicast, broadcast */
    bcmStatGroupModeTypedAllWithControl = 12, /* A dedicated counter for control,
                                           unknown unicast, known
                                           unicast,multicast, broadcast and one
                                           for all traffic (not already counted) */
    bcmStatGroupModeTypedIntPriWithControl = 13, /* A dedicated counter for control,
                                           unknown unicast, known unicast,
                                           multicast, broadcast and N internal
                                           priority counters for traffic (not
                                           already counted) */
    bcmStatGroupModeDot1P = 14,         /* A set of 8(2^3) counters selected
                                           based on Vlan priority */
    bcmStatGroupModeIntPri = 15,        /* A set of 16(2^4) counters based on
                                           internal priority */
    bcmStatGroupModeIntPriCng = 16,     /* A set of 64 counters(2^(4+2)) based
                                           on Internal priority+CNG */
    bcmStatGroupModeSvpType = 17,       /* A set of 2 counters(2^1) based on SVP
                                           type */
    bcmStatGroupModeDscp = 18,          /* A set of 64 counters(2^6) based on
                                           DSCP bits */
    bcmStatGroupModeDvpType = 19,       /* A set of 2 counters(2^1) based on DVP
                                           type */
    bcmStatGroupModeCng = 20,           /* A set of 4 counters based on Pre IFP
                                           packet color bits */
    bcmStatGroupModeMplsCngLabelFirst = 21, /* A set of 4 counters based on Pre IFP
                                           packet color bits based on first Mpls
                                           Label */
    bcmStatGroupModeMplsIntPriLabelFirst = 22, /* A set of 16(2^4) counters based on
                                           internal priority based on first Mpls
                                           Label */
    bcmStatGroupModeMplsCngLabelSecond = 23, /* A set of 4 counters based on Pre IFP
                                           packet color bits based on second
                                           Mpls Label */
    bcmStatGroupModeMplsIntPriLabelSecond = 24, /* A set of 16(2^4) counters based on
                                           internal priority based on second
                                           Mpls Label */
    bcmStatGroupModeMplsCngLabelThird = 25, /* A set of 4 counters based on Pre IFP
                                           packet color bits based on third Mpls
                                           Label */
    bcmStatGroupModeMplsIntPriLabelThird = 26, /* A set of 16(2^4) counters based on
                                           internal priority based on third Mpls
                                           Label */
    bcmStatGroupModeCount = 27          /* This should be the maximum value
                                           defined for the enum */
} bcm_stat_group_mode_t;

/* Flex statistics type */
typedef enum bcm_stat_flex_stat_e {
    bcmStatFlexStatPackets = 0, /* Packets statistics */
    bcmStatFlexStatBytes = 1,   /* Bytes statistics */
    bcmStatFlexStatCount = 2    /* Max value defined for this enum */
} bcm_stat_flex_stat_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Retrieves the flex counter details for a given direction */
extern int bcm_stat_flex_pool_info_multi_get(
    int unit, 
    bcm_stat_flex_direction_t direction, 
    uint32 num_pools, 
    bcm_stat_flex_pool_stat_info_t *flex_pool_stat, 
    uint32 *actual_num_pools);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize an attribute selector of Stat Flex Group Mode */
extern void bcm_stat_group_mode_attr_selector_t_init(
    bcm_stat_group_mode_attr_selector_t *attr_selector);

/* Initialize a bcm_stat_value_t data structure */
extern void bcm_stat_value_t_init(
    bcm_stat_value_t *stat_value);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Create Customized Stat Group mode for given Counter Attributes
 * In BCM56860, OAM LM is supported through Flex Counters, When
 * BCM_STAT_GROUP_MODE_CAPABILITY_OAM flag is passed along with
 * BCM_STAT_GROUP_MODE_INGRESS or BCM_STAT_GROUP_MODE_EGRESS, it
 * instructs the API to create a stat group suitable for OAM.
 * In BCM56160, the packet cfi_pri offset map of Vlan Flow Counters
 * is supported through this function to get the mode_id. And use
 * this mode_id to create the customized group mode by
 * bcm_stat_custom_group_create.
 * Counters with same counter_offset and with same attribute
 * type but different attributes values are counted whenever either
 * of the value(s) are true whereas with different attribute
 * types the counter is incremented only when all of the attributes
 * satisfy
 * with their respective values.
 *  The multiple attributes that are being used in the creation of the
 * mode_id
 * should be added for all the counter offsets to ensure proper
 * configuration.
 *  This API will be deprecated in the second half of 2016.
 *  stats mode id should be created using
 * bcm_stat_group_mode_id_config_create API
 */
extern int bcm_stat_group_mode_id_create(
    int unit, 
    uint32 flags, 
    uint32 total_counters, 
    uint32 num_selectors, 
    bcm_stat_group_mode_attr_selector_t *attr_selectors, 
    uint32 *mode_id);

/* 
 * Create Customized Stat class identifiers for combining Counter
 * Attributes.
 * This API is supported on enabling the config property named
 * flex_stat_attributes_class.
 */
extern int bcm_stat_group_mode_attr_class_id_create(
    int unit, 
    bcm_stat_group_mode_attr_t attr, 
    uint32 num_of_attr_values, 
    uint32 *attr_values, 
    bcm_stat_group_mode_attr_class_id_t *attr_class_id);

/* Create group identifiers for combining ports. */
extern int bcm_stat_port_group_create(
    int unit, 
    uint32 num_gports, 
    bcm_gport_t *gport, 
    uint32 *group_id);

/* Retrieves Customized Stat Group mode Attributes for given mode_id */
extern int bcm_stat_group_mode_id_get(
    int unit, 
    uint32 mode_id, 
    uint32 *flags, 
    uint32 *total_counters, 
    uint32 num_selectors, 
    bcm_stat_group_mode_attr_selector_t *attr_selectors, 
    uint32 *actual_num_selectors);

/* Destroys Customized Group mode */
extern int bcm_stat_group_mode_id_destroy(
    int unit, 
    uint32 mode_id);

/* 
 * Destroy Customized Stat class identifiers for combining Counter
 * Attributes.
 * This API is supported on enabling the config property named
 * flex_stat_attributes_class.
 */
extern int bcm_stat_group_mode_attr_class_id_destroy(
    int unit, 
    bcm_stat_group_mode_attr_class_id_t attr_class_id);

/* Destroy group identifiers for combined ports. */
extern int bcm_stat_port_group_destroy(
    int unit, 
    uint32 group_id);

/* Associate an accounting object to customized group mode */
extern int bcm_stat_custom_group_create(
    int unit, 
    uint32 mode_id, 
    bcm_stat_object_t object, 
    uint32 *stat_counter_id, 
    uint32 *num_entries);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize the structure for Stat Flex config */
extern void bcm_stat_group_mode_id_config_t_init(
    bcm_stat_group_mode_id_config_t *stat_config);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Create Customized Stat Group mode for given Counter Attributes
 * In BCM56960, a custom stat group for Ingress Field groups can be
 * created by
 * specifying stat_config->hint.type as
 * bcmStatGroupAllocHintIngressFieldGroup
 * and setting the actual field group id in stat_config->hint.value.
 */
extern int bcm_stat_group_mode_id_config_create(
    int unit, 
    uint32 options, 
    bcm_stat_group_mode_id_config_t *stat_config, 
    uint32 num_selectors, 
    bcm_stat_group_mode_attr_selector_t *attr_sel, 
    uint32 *mode_id);

/* 
 * Retrieves Customized Stat Group mode Attributes for given custom
 * mode_id
 */
extern int bcm_stat_group_mode_id_config_get(
    int unit, 
    uint32 mode_id, 
    bcm_stat_group_mode_id_config_t *config, 
    uint32 num_selectors, 
    bcm_stat_group_mode_attr_selector_t *attr_sel, 
    uint32 *actual_num_selectors);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize the hint structure */
extern void bcm_stat_group_mode_hint_t_init(
    bcm_stat_group_mode_hint_t *stat_hint);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create Counter Entries group for given accounting object */
extern int bcm_stat_group_create(
    int unit, 
    bcm_stat_object_t object, 
    bcm_stat_group_mode_t group_mode, 
    uint32 *stat_counter_id, 
    uint32 *num_entries);

/* Destroy counter Entries group */
extern int bcm_stat_group_destroy(
    int unit, 
    uint32 stat_counter_id);

/* Dump contents of specified group */
extern int bcm_stat_group_dump(
    int unit, 
    bcm_stat_object_t object, 
    bcm_stat_group_mode_t group_mode);

/* Dump contents of all configured groups */
extern int bcm_stat_group_dump_all(
    int unit);

/* Get all Stat Ids attached to flex object. */
extern int bcm_stat_id_get_all(
    int unit, 
    bcm_stat_object_t object, 
    int stat_max, 
    uint32 *stat_array, 
    int *stat_count);

#endif /* BCM_HIDE_DISPATCHABLE */

/* A range of counter pointers. */
typedef struct bcm_stat_counter_pointer_range_s {
    int start; 
    int end; 
} bcm_stat_counter_pointer_range_t;

/* 
 * Define the counting source of a counter engine. That is, what block to
 * count, and what to count in a block
 */
typedef enum bcm_stat_counter_source_type_e {
    bcmStatCounterSourceIngressOam = 0, 
    bcmStatCounterSourceEgressOam = 1, 
    bcmStatCounterSourceIngressVsi = 2, 
    bcmStatCounterSourceIngressInlif = 3, 
    bcmStatCounterSourceIngressCnm = 4, 
    bcmStatCounterSourceIngressField = 5, 
    bcmStatCounterSourceEgressField = 6, 
    bcmStatCounterSourceIngressLatency = 7, 
    bcmStatCounterSourceIngressVoq = 8, 
    bcmStatCounterSourceIngressVsq = 9, 
    bcmStatCounterSourceEgressTransmitVsi = 10, 
    bcmStatCounterSourceEgressReceiveVsi = 11, 
    bcmStatCounterSourceEgressTransmitOutlif = 12, 
    bcmStatCounterSourceEgressReceiveOutlif = 13, 
    bcmStatCounterSourceEgressTransmitQueue = 14, 
    bcmStatCounterSourceEgressTransmitTmPort = 15, 
    bcmStatCounterSourceEgressReceiveQueue = 16, 
    bcmStatCounterSourceEgressReceiveTmPort = 17, 
    bcmStatCounterSourceEgressReceiveTm = 18, 
    bcmStatCounterSourceIngressExtStat = 19, 
    bcmStatCounterSourceCount = 20      /* Must be last one. */
} bcm_stat_counter_source_type_t;

/* Counter Engine Source Configuration */
typedef struct bcm_stat_counter_source_s {
    bcm_core_t core_id;                 /* The counting source core */
    bcm_stat_counter_source_type_t engine_source; /* Counting source */
    bcm_stat_counter_pointer_range_t pointer_range; /* The range of counter  pointers. */
    int command_id;                     /* Defines what counting command to
                                           read. */
    int num_voqs_per_entry;             /* Defines how many consecutive VOQs are
                                           counted in one counter set. Only
                                           relevant for
                                           bcm_stat_counter_engine_source_ingress_voq. */
    int offset;                         /* Constant offset added on top of the
                                           counter pointer value, applicable for
                                           ERPP sources only. */
} bcm_stat_counter_source_t;

/* 
 * An entry counter set offset mapping, consists of color X {forward,
 * drop}
 */
typedef struct bcm_stat_counter_set_entry_s {
    bcm_color_t color;          /* Counter color (Drop Precedence) */
    uint8 is_forward_not_drop;  /* If set, the counter refers to Forward packet
                                   counters. Otherwise, to dropped packet
                                   counters. */
} bcm_stat_counter_set_entry_t;

/* Mapping of an entry to a counter set offset */
typedef struct bcm_stat_counter_set_entry_mapping_s {
    bcm_stat_counter_set_entry_t entry; /* The entrys value, consists of color X
                                           {forward, drop} */
    int offset; 
} bcm_stat_counter_set_entry_mapping_t;

#define BCM_STAT_COUNTER_MAPPING_MAX_SIZE   8          /* The maximum legal size
                                                          of a counter set */

/* Flags for Stat PP metadata */
#define BCM_STAT_PP_METADATA_INGRESS        0x1        /* Configure Ingress side */
#define BCM_STAT_PP_METADATA_EGRESS         0x2        /* Configure Egress side */
#define BCM_STAT_PP_METADATA_TUNNEL         0x4        /* Tunnel termination for
                                                          ingress side, Tunnel
                                                          encapsulation for
                                                          egress side */
#define BCM_STAT_PP_METADATA_FORWARD        0x8        /* Forwarding */
#define BCM_STAT_PP_METADATA_FORWARD_PLUS_ONE 0x10       /* Next layer after
                                                          forwarding */

/* Address type flags */
#define BCM_STAT_PP_METADATA_ADDRESS_TYPE_UCAST 0x1        /* Unicast address flow */
#define BCM_STAT_PP_METADATA_ADDRESS_TYPE_MCAST 0x2        /* Multicast address flow */
#define BCM_STAT_PP_METADATA_ADDRESS_TYPE_BCAST 0x4        /* Broadcast address flow */
#define BCM_STAT_PP_METADATA_ADDRESS_TYPE_UNKNOWN 0x8        /* Unknown address flow */

/* Counter configuration struct */
typedef struct bcm_stat_counter_set_mapping_s {
    int counter_set_size;               /* The size of the counter set, cannot
                                           an entry mapped to a value above it,
                                           will not be counted */
    int num_of_mapping_entries;         /* The number of valid entries in
                                           entry_mapping array */
    bcm_stat_counter_set_entry_mapping_t entry_mapping[BCM_STAT_COUNTER_MAPPING_MAX_SIZE]; /* An array of color X {forward, drop}
                                           mapped to count an offset in the
                                           counter set */
} bcm_stat_counter_set_mapping_t;

/* Defines the width of an entry, and what statistics to gather. */
typedef enum bcm_stat_counter_format_type_e {
    bcmStatCounterFormatPacketsAndBytes = 0, 
    bcmStatCounterFormatPackets = 1, 
    bcmStatCounterFormatBytes = 2, 
    bcmStatCounterFormatDoublePackets = 3, /* doubles that total number of entries
                                           for a counter engine */
    bcmStatCounterFormatMaxVoqSize = 4, /* Not relevant for Jer2 */
    bcmStatCounterFormatMaxSize = 4,    /* Always double mode */
    bcmStatCounterFormatIngressLatency = 5, 
    bcmStatCounterFormatCount = 6       /* Must be last one. */
} bcm_stat_counter_format_type_t;

/* Counter configuration struct */
typedef struct bcm_stat_counter_format_s {
    bcm_stat_counter_format_type_t format_type; /* Counting format: defines the width of
                                           an entry, and what statistics to
                                           gather. */
    bcm_stat_counter_set_mapping_t counter_set_mapping; /* counter set configuration: defines
                                           the size of a counter set, and what
                                           each offset inside a set counts. */
} bcm_stat_counter_format_t;

/* Counter configuration struct */
typedef struct bcm_stat_counter_config_s {
    bcm_stat_counter_format_t format; 
    bcm_stat_counter_source_t source; 
} bcm_stat_counter_config_t;

#define BCM_STAT_COUNTER_CLEAR_CONFIGURATION 0x1        /* Disables the counter
                                                          engine counting clears
                                                          its configuration. */
#define BCM_STAT_COUNTER_CACHE              0x2        /* Clears the counter
                                                          cache. */
#define BCM_STAT_COUNTER_SYMMETRICAL_MULTICORE_ENGINE 0x4        /* If this flag is
                                                          specified then
                                                          consecutive counter
                                                          engines are configured
                                                          for each core, each
                                                          statistics gathering
                                                          is aggregated for
                                                          symmetrical sources.
                                                          config->source.core_id
                                                          must be BCM_CORE_ALL. */
#define BCM_STAT_COUNTER_IGNORE_COLORS      0x8        /* Ignore colors from
                                                          counter set mapping. */
#define BCM_STAT_COUNTER_IGNORE_DISPOSITION 0x10       /* Ignore disposition
                                                          (forward or drop) from
                                                          counter set mapping. */
#define BCM_STAT_COUNTER_MULTIPLE_SOURCES_PER_ENGINE 0x20       /* Connect 2 sources to
                                                          one engine - for QAX
                                                          OAM. */
#define BCM_STAT_COUNTER_OFFSET_VALID       0x40       /* Defines if to consider
                                                          the offset parameter
                                                          in the structure
                                                          bcm_stat_counter_source_t. */

/* Couner engine type */
typedef struct bcm_stat_counter_engine_s {
    uint32 flags; 
    int engine_id; 
} bcm_stat_counter_engine_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Configures a counter engine. */
extern int bcm_stat_counter_config_set(
    int unit, 
    bcm_stat_counter_engine_t *engine, 
    bcm_stat_counter_config_t *config);

/* Configures a counter engine. */
extern int bcm_stat_counter_config_get(
    int unit, 
    bcm_stat_counter_engine_t *engine, 
    bcm_stat_counter_config_t *config);

/* Get the specified statistic of a given counter */
extern int bcm_stat_flex_counter_get(
    int unit, 
    uint32 stat_counter_id, 
    bcm_stat_flex_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* Get the synced specified statistic of a given counter */
extern int bcm_stat_flex_counter_sync_get(
    int unit, 
    uint32 stat_counter_id, 
    bcm_stat_flex_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* Set counter value of a given counter */
extern int bcm_stat_flex_counter_set(
    int unit, 
    uint32 stat_counter_id, 
    bcm_stat_flex_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* Set statistics PP metadata value */
extern int bcm_stat_pp_metadata_set(
    int unit, 
    bcm_stat_pp_metadata_info_t *metadata_info);

/* Get statistics PP metadata value */
extern int bcm_stat_pp_metadata_get(
    int unit, 
    bcm_stat_pp_metadata_info_t *metadata_info);

#endif /* BCM_HIDE_DISPATCHABLE */

typedef enum bcm_stat_counter_lif_stack_id_e {
    bcmStatCounterLifStackIdNone = -1,  /* Do not issue a counter command. */
    bcmStatCounterLifStackId0 = 0,      /* Issue a counter command with the
                                           outermost LIF. */
    bcmStatCounterLifStackId1 = 1,      /* Issue a counter command with LIF 1 in
                                           the packet stack. */
    bcmStatCounterLifStackId2 = 2,      /* Issue a counter command with LIF 2 in
                                           the packet stack. */
    bcmStatCounterLifStackId3 = 3       /* Issue a counter command with the
                                           innermost LIF. */
} bcm_stat_counter_lif_stack_id_t;

#define BCM_STAT_COUNT_LIF_NUMBER_OF_STACK_IDS 4          /* The maximal number of
                                                          LIFs in the packet
                                                          stack */

/* 
 * An entry in a mask of range IDs per stack ID level, each value defines
 * a result of LIF value compared with the LIF ranges.
 */
typedef enum bcm_stat_counter_lif_range_id_e {
    bcmStatCounterLifRangeIdLifInvalid = -2, /* The LIF does not exist in the packet
                                           stack. */
    bcmStatCounterLifRangeIdNotInAny = -1, /* The LIF is not in any range. */
    bcmBcmStatCounterLifRangeId0 = 0,   /* The LIF is in any range 0. */
    bcmBcmStatCounterLifRangeId1 = 1    /* The LIF is in any range 1. */
} bcm_stat_counter_lif_range_id_t;

/* 
 * Defines the mask of range IDs per stack ID level, for which the packet
 * will be counted.
 */
typedef struct bcm_stat_counter_lif_mask_s {
    bcm_stat_counter_lif_range_id_t lif_counting_mask[BCM_STAT_COUNT_LIF_NUMBER_OF_STACK_IDS]; /* An entry in a mask of range IDs per
                                           stack ID level, each value defines a
                                           result of LIF value compared with the
                                           LIF ranges. */
} bcm_stat_counter_lif_mask_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Creates a {counting_source, counting_mask} to lif_stack_level mapping.
 * For Issuing a counter command with the the LIF in
 * lif_stack_id_to_count in the packet LIF stack.
 */
extern int bcm_stat_counter_lif_counting_set(
    int unit, 
    uint32 flags, 
    bcm_stat_counter_source_t *source, 
    bcm_stat_counter_lif_mask_t *counting_mask, 
    bcm_stat_counter_lif_stack_id_t lif_stack_id_to_count);

/* 
 * Creates a {counting_source, counting_mask} to lif_stack_level mapping.
 * For Issuing a counter command with the the LIF in
 * lif_stack_id_to_count in the packet LIF stack.
 */
extern int bcm_stat_counter_lif_counting_get(
    int unit, 
    uint32 flags, 
    bcm_stat_counter_source_t *source, 
    bcm_stat_counter_lif_mask_t *counting_mask, 
    bcm_stat_counter_lif_stack_id_t *lif_stack_id_to_count);

#endif /* BCM_HIDE_DISPATCHABLE */

/* A range of LIF IDs. */
typedef struct bcm_stat_counter_lif_counting_range_s {
    int start; 
    int end; 
    int is_double_entry; 
} bcm_stat_counter_lif_counting_range_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set A range of LIF IDs. */
extern int bcm_stat_counter_lif_counting_range_set(
    int unit, 
    uint32 flags, 
    bcm_stat_counter_source_type_t source, 
    bcm_stat_counter_lif_range_id_t range_id, 
    bcm_stat_counter_lif_counting_range_t *lif_range);

/* Set A range of LIF IDs. */
extern int bcm_stat_counter_lif_counting_range_get(
    int unit, 
    uint32 flags, 
    bcm_stat_counter_source_type_t source, 
    bcm_stat_counter_lif_range_id_t range_id, 
    bcm_stat_counter_lif_counting_range_t *lif_range);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Enable parameter 'engine_range_offse' in structure
 * bcm_stat_lif_counting_source_t.
 */
#define BCM_STAT_COUNT_LIF_ENGINE_RANGE_OFFSET_ENABLE 0x1        

/* LIF Counting Source. */
typedef struct bcm_stat_lif_counting_source_s {
    bcm_stat_counter_source_type_t type; 
    int command_id; 
    int stif_counter_id; 
    uint32 offset; 
    int engine_range_offset;            /* Positive or negative offset to align
                                           the profile range to the engine
                                           range. relevant only for out lif
                                           counting in QAX/QUX. */
} bcm_stat_lif_counting_source_t;

/* LIF Counting Configuration. */
typedef struct bcm_stat_lif_counting_s {
    bcm_stat_lif_counting_source_t source; 
    bcm_stat_counter_lif_counting_range_t range; 
} bcm_stat_lif_counting_t;

/* stat counter value structure . */
typedef struct bcm_stat_counter_value_s {
    uint64 value;   /* value of one statistic */
} bcm_stat_counter_value_t;

/* stat counter Input data in order to know which counter to read. */
typedef struct bcm_stat_counter_input_data_s {
    bcm_gport_t counter_source_gport;   /* counter source gport, (for example:
                                           global lif, If the source is Lif) */
    uint32 counter_source_id;           /* Object-stat-id (counter_set id) that
                                           the user wants to read (the
                                           counter_pointer value as sent to the
                                           crps.) */
    bcm_stat_counter_source_type_t counter_source_type; /* type of source to count.not in used
                                           for 88690 device and above */
    int command_id;                     /* command id */
    bcm_core_t core_id;                 /* core id to count from */
    int database_id;                    /* determine the database that user want
                                           to read from. (not in use before
                                           device 88690). */
    int type_id;                        /* determine the source_type_id that
                                           user want to read from. (not in use
                                           before device 88690). */
    int nstat;                          /* size of statistics array to count */
    int *stat_arr;                      /* statistic array to count */
} bcm_stat_counter_input_data_t;

/* stat counter output data. */
typedef struct bcm_stat_counter_output_data_s {
    bcm_stat_counter_value_t *value_arr; /* array with the values read for each
                                           statistic */
} bcm_stat_counter_output_data_t;

#define BCM_STAT_LIF_COUNTING_PROFILE_NONE  -1         /* An indicator for a LIF
                                                          to be uncounted */

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Configure a LIF-counting-profile - define it's range - and what
 * counter-command collects it.
 */
extern int bcm_stat_lif_counting_profile_set(
    int unit, 
    uint32 flags, 
    int lif_counting_profile, 
    bcm_stat_lif_counting_t *lif_counting);

/* 
 * Configure a LIF-counting-profile - define it's range - and what
 * counter-command collects it.
 */
extern int bcm_stat_lif_counting_profile_get(
    int unit, 
    uint32 flags, 
    int lif_counting_profile, 
    bcm_stat_lif_counting_t *lif_counting);

/* Set A priority of a LIF in a stack */
extern int bcm_stat_lif_counting_stack_level_priority_set(
    int unit, 
    uint32 flags, 
    bcm_stat_lif_counting_source_t *source, 
    bcm_stat_counter_lif_stack_id_t lif_stack_level, 
    int priority);

/* Set A priority of a LIF in a stack */
extern int bcm_stat_lif_counting_stack_level_priority_get(
    int unit, 
    uint32 flags, 
    bcm_stat_lif_counting_source_t *source, 
    bcm_stat_counter_lif_stack_id_t lif_stack_level, 
    int *priority);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * different types of filters to be filtered in or out by the counter
 * proccesor
 */
typedef enum bcm_stat_counter_filter_e {
    bcmStatCounterTotalPDsThresholdViolated, /* Total PDs Threshold Violated - for
                                           Egress Receive engine source */
    bcmStatCounterTotalPDsUcPoolSizeThresholdViolated, /* Total PDs Uc Pool Size Threshold
                                           Violated - for Egress Receive engine
                                           source */
    bcmStatCounterPerPortUcPDsThresholdViolated, /* Per Port Uc PDs Threshold Violated -
                                           for Egress Receive engine source */
    bcmStatCounterPerQueueUcPDsThresholdViolated, /* Per Queue Uc PDs Threshold Violated -
                                           for Egress Receive engine source */
    bcmStatCounterPerPortUcDBsThresholdViolated, /* Per Port Uc DBs Threshold Violated -
                                           for Egress Receive engine source */
    bcmStatCounterPerQueueUcDBsThresholdViolated, /* Per Queue Uc DBs Threshold Violated -
                                           for Egress Receive engine source */
    bcmStatCounterPerQueueDisableBit,   /* Stat Counter Per Queue Disable Bit -
                                           for Egress Receive engine source */
    bcmStatCounterTotalPDsMcPoolSizeThresholdViolated, /* Total PDs Mc Pool Size Threshold
                                           Violated - for Egress Receive engine
                                           source */
    bcmStatCounterPerInterfacePDsThreholdViolated, /* Per Interface PDs Threhold Violated -
                                           for Egress Receive engine source */
    bcmStatCounterMcSPThresholdViolated, /* Mc SP Threshold Violated - for Egress
                                           Receive engine source */
    bcmStatCounterPerMcTCThresholdViolated, /* Per Mc TC Threshold Violated - for
                                           Egress Receive engine source */
    bcmStatCounterMcPDsPerPortThresholdViolated, /* Mc PDs Per Port Threshold Violated -
                                           for Egress Receive engine source */
    bcmStatCounterMcPDsPerQueueThresholdViolated, /* Mc PDs Per Queue Threshold Violated -
                                           for Egress Receive engine source */
    bcmStatCounterMcPerPortSizeThresholdViolated, /* Mc Per Port Size Threshold Violated -
                                           for Egress Receive engine source */
    bcmStatCounterMcPerQueueSizeThresholdViolated, /* Mc Per Queue Size Threshold Violated
                                           - for Egress Receive engine source */
    bcmStatCounterGlobalRejectDiscards, /* Global Reject Discards - for Ingress
                                           engine source */
    bcmStatCounterDramRejectDiscards,   /* Dram Reject Discards - for Ingress
                                           engine source */
    bcmStatCounterVoqTailDropDiscards,  /* Voq Tail Drop Discards - for Ingress
                                           engine source */
    bcmStatCounterVoqStatisticsDiscards, /* Voq Statistics Discards - for Ingress
                                           engine source */
    bcmStatCounterVsqTailDropDiscards,  /* Vsq Tail Drop Discards - for Ingress
                                           engine source */
    bcmStatCounterVsqStatisticsDiscards, /* Vsq Statistics Discards - for Ingress
                                           engine source */
    bcmStatCounterQueueNotValidDiscard, /* Queue Not Valid Discard - for Ingress
                                           engine source */
    bcmStatCounterOtherDiscards,        /* Other Discards - for Ingress engine
                                           source */
    bcmStatCounterDropReasonCount       /* Number of Drop Reasons - should
                                           always be last entry */
} bcm_stat_counter_filter_t;

#define BCM_STAT_COUNTER_TM_COMMAND 0xFF       /* flag for dedicated engine
                                                  using TM command */

#ifndef BCM_HIDE_DISPATCHABLE

/* set counter source to filter in or out certain filter criteria. */
extern int bcm_stat_counter_filter_set(
    int unit, 
    bcm_stat_counter_source_t source, 
    bcm_stat_counter_filter_t *filter_array, 
    int filter_count, 
    int is_active);

/* 
 * determine which filter criteria a certain counter source is filtering
 * out.
 */
extern int bcm_stat_counter_filter_get(
    int unit, 
    bcm_stat_counter_source_t source, 
    int filter_max_count, 
    bcm_stat_counter_filter_t *filter_array, 
    int *filter_count);

/* determine if counter is filtering out certain filter criterion. */
extern int bcm_stat_counter_filter_is_active_get(
    int unit, 
    bcm_stat_counter_source_t source, 
    bcm_stat_counter_filter_t filter, 
    int *is_active);

/* 
 * Get 64 bit counter values array for a multiple (or single if array
 * size=1) statistic types belong to one source counter set.
 */
extern int bcm_stat_counter_get(
    int unit, 
    uint32 flags, 
    bcm_stat_counter_input_data_t *stat_counter_input_data, 
    bcm_stat_counter_output_data_t *stat_counter_output_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Init function to the structure "bcm_stat_counter_input_data_t". */
extern void bcm_stat_counter_input_data_t_init(
    bcm_stat_counter_input_data_t *stat_input_data);

/* Initialize a stat_flex_pool_stat_info object struct. */
extern void bcm_stat_flex_pool_stat_info_t_init(
    bcm_stat_flex_pool_stat_info_t *stat_flex_pool_stat_info);

/* Structure bcm_stat_egress_receive_tm_pointer_format_t */
typedef struct bcm_stat_egress_receive_tm_pointer_format_s {
    uint32 queue_pair_mask;     /* Which bits to consider from the queue pair
                                   field when building the counter pointer. */
    uint32 traffic_class_mask;  /* Which bits to consider from the traffic class
                                   field when building the counter pointer. */
    uint32 cast_mask;           /* Consider (or not) the cast bit when building
                                   the counter pointer. */
} bcm_stat_egress_receive_tm_pointer_format_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Set the format of the counter pointer that is sent from egress receive
 * tm.
 */
extern int bcm_stat_egress_receive_tm_pointer_format_set(
    int unit, 
    int flags, 
    bcm_stat_egress_receive_tm_pointer_format_t *pointer_format);

/* 
 * Get the format of the counter pointer that is sent from egress receive
 * tm.
 */
extern int bcm_stat_egress_receive_tm_pointer_format_get(
    int unit, 
    int flags, 
    bcm_stat_egress_receive_tm_pointer_format_t *pointer_format);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Defines the counter database id and core */
typedef struct bcm_stat_counter_database_s {
    int database_id;    /* Database id. */
    bcm_core_t core_id; /* Core id. */
} bcm_stat_counter_database_t;

#define BCM_STAT_DATABASE_CREATE_WITH_ID    0x00000001 /* Create a counter
                                                          database with id */

#ifndef BCM_HIDE_DISPATCHABLE

/* API creates a counter database */
extern int bcm_stat_counter_database_create(
    int unit, 
    uint32 flags, 
    bcm_core_t core_id, 
    int *database_id);

/* API destroy a counter database */
extern int bcm_stat_counter_database_destroy(
    int unit, 
    uint32 flags, 
    bcm_stat_counter_database_t *database);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Defines the counter engine id and core */
typedef struct bcm_stat_engine_s {
    int engine_id;      /* Engine id. */
    bcm_core_t core_id; /* Core id. Must give explicit core. */
} bcm_stat_engine_t;

/* 
 * Enable/disable the engines belong to the database - counting and
 * eviction.
 */
typedef struct bcm_stat_counter_enable_s {
    int enable_counting;    /* Engine input - if set, the engine may receive
                               counter pointers and can count. */
    int enable_eviction;    /* Engine output - if set the engine eviction
                               mechanism (scanner) works and counters evicted to
                               the relevant DMA FIFO. */
} bcm_stat_counter_enable_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * API enable/disable the engines belong to the database - counting and
 * eviction (input and output). Enable should be made after configure all
 * parameters of the database.
 */
extern int bcm_stat_counter_database_enable_set(
    int unit, 
    uint32 flags, 
    bcm_stat_counter_database_t *database, 
    bcm_stat_counter_enable_t *enable);

/* determine the enabler status of the database. */
extern int bcm_stat_counter_database_enable_get(
    int unit, 
    uint32 flags, 
    bcm_stat_counter_database_t *database, 
    bcm_stat_counter_enable_t *enable);

/* API attach new engine to database */
extern int bcm_stat_counter_engine_attach(
    int unit, 
    uint32 flags, 
    bcm_stat_counter_database_t *database, 
    int engine_id);

/* API detach engine from database */
extern int bcm_stat_counter_engine_detach(
    int unit, 
    uint32 flags, 
    bcm_stat_counter_database_t *database, 
    int engine_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Enum which defines the engine control options */
typedef enum bcm_stat_counter_engine_control_e {
    bcmStatCounterClearAll = 0,         /* Clear all counters belong to an
                                           engine. clear HW/SW counters. (depend
                                           on the flags. default is both, but
                                           not simultaneously) */
    bcmStatCounterSequentialSamplingInterval = 1, /* Setting the sequential sampling
                                           interval. micro-seconds units */
    bcmStatCounterEngineEvictionEnable = 2 /* Enable/Disable HW eviction for a
                                           specific engine */
} bcm_stat_counter_engine_control_t;

/* Enum which defines the counter database control options. */
typedef enum bcm_stat_counter_database_control_e {
    bcmStatCounterDatabaseClearAll = 0, /* Clear all counters belong to a
                                           database. clear HW/SW counters.
                                           (depend on the flags. default is
                                           both, but not simultaneously) */
    bcmStatCounterOverflowMode = 1      /* Allow the user to choose the counter
                                           overflow mode */
} bcm_stat_counter_database_control_t;

#define BCM_STAT_COUNTER_CONTROL_CLEAR_SW_ONLY 0x1        /* Refer to control enum
                                                          bcmStatCounterClearAll
                                                          - clear only SW
                                                          counters memory. */
#define BCM_STAT_COUNTER_CONTROL_CLEAR_HW_ONLY 0x2        /* Refer to control enum
                                                          bcmStatCounterClearAll
                                                          - clear only HW
                                                          counters memory. */

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * API which control some counter engine operations on genral crps
 * operations.
 */
extern int bcm_stat_counter_engine_control_set(
    int unit, 
    uint32 flags, 
    bcm_stat_engine_t *engine, 
    bcm_stat_counter_engine_control_t control, 
    uint32 arg);

/* 
 * Get the value of one control element. Can't be used for enum
 * bcmStatCounterClearAll.
 */
extern int bcm_stat_counter_engine_control_get(
    int unit, 
    uint32 flags, 
    bcm_stat_engine_t *engine, 
    bcm_stat_counter_engine_control_t control, 
    uint32 *arg);

/* API which control some counter database operations. */
extern int bcm_stat_counter_database_control_set(
    int unit, 
    uint32 flags, 
    bcm_stat_counter_database_t *database, 
    bcm_stat_counter_database_control_t control, 
    uint32 arg);

/* 
 * Get the value of one control counter database element. Can't be used
 * for enum bcmStatCounterClearAll.
 */
extern int bcm_stat_counter_database_control_get(
    int unit, 
    uint32 flags, 
    bcm_stat_counter_database_t *database, 
    bcm_stat_counter_database_control_t control, 
    uint32 *arg);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES 4          /* Defines the number of
                                                          object types */

/* Define the counting source of a counter engine. */
typedef enum bcm_stat_counter_interface_type_e {
    bcmStatCounterInterfaceIngressReceivePp = 0, 
    bcmStatCounterInterfaceIngressOam = 1, 
    bcmStatCounterInterfaceIngressVoq = 2, 
    bcmStatCounterInterfaceIngressTransmitPp = 3, 
    bcmStatCounterInterfaceEgressReceivePp = 4, 
    bcmStatCounterInterfaceEgressReceiveQueue = 5, 
    bcmStatCounterInterfaceEgressTransmitPp = 6, 
    bcmStatCounterInterfaceEgressOam = 7, 
    bcmStatCounterInterfaceOamp = 8, 
    bcmStatCounterInterfaceCount = 9 
} bcm_stat_counter_interface_type_t;

/* Set of counter engine parameters per type. */
typedef struct bcm_stat_counter_type_config_s {
    uint32 valid;               /* Defines type addmission to an engine. */
    uint32 object_type_offset;  /* Counter offset for the type which is add in
                                   the counter mapping process.
                                   units=object-stat-id */
    int start;                  /* Holds the base counter pointer that the
                                   engine should refer to. units=object-stat-id.
                                   If the engine is concatenate to other engines
                                   (prev or next), the range (start/end) should
                                   be the same for all engines in the
                                   'engines-link-list' and it should refer to
                                   the total range that need to count. (driver
                                   will calculate the specific range for each
                                   engine) */
    int end;                    /* Holds the last object-stat-id pointer that
                                   the engine should refer to.
                                   units=object-stat-id. If the engine is
                                   concatenate to other engines (prev or next),
                                   the range (start/end) should be the same for
                                   all engines in the 'engines-link-list' and it
                                   should refer to the total range that need to
                                   count. (driver will calculate the specific
                                   range for each engine) */
} bcm_stat_counter_type_config_t;

/* Defines the counter engine id and core */
typedef struct bcm_stat_counter_interface_s {
    bcm_stat_counter_interface_type_t source; /* Counting source. */
    int command_id;                     /* Command id (interface id). */
    bcm_stat_counter_type_config_t type_config[BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES]; /* Set of parameters configured per
                                           object type. */
    bcm_stat_counter_format_type_t format_type; /* Format of the counter and it
                                           arithmetic action. */
    int counter_set_size;               /* The size of the counter set. */
} bcm_stat_counter_interface_t;

#define BCM_STAT_COUNTER_MODIFY_RANGE_ONLY  0x00000001 /* Allow to change only
                                                          the range and the
                                                          parameter
                                                          next_consecutive_engine_id
                                                          per type on the fly,
                                                          while engine is
                                                          active. */
#define BCM_STAT_COUNTER_END_TO_END_LATENCY 0x00000002 /* If flag set, driver
                                                          select the latency
                                                          value as an input to
                                                          the engine, otherwise
                                                          select packet size. */

#ifndef BCM_HIDE_DISPATCHABLE

/* API is used to configure the database interface. */
extern int bcm_stat_counter_interface_set(
    int unit, 
    uint32 flags, 
    bcm_stat_counter_database_t *database, 
    bcm_stat_counter_interface_t *config);

/* API is used to get the database interface configuration. */
extern int bcm_stat_counter_interface_get(
    int unit, 
    uint32 flags, 
    bcm_stat_counter_database_t *database, 
    bcm_stat_counter_interface_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Enum of the fields(types) that can be used in the expansion format. */
typedef enum bcm_stat_expansion_types_e {
    bcmStatExpansionTypeTmDropReason = 0, 
    bcmStatExpansionTypeDispositionIsDrop = 1, 
    bcmStatExpansionTypeDropPrecedenceMeter0Valid = 2, 
    bcmStatExpansionTypeDropPrecedenceMeter0Value = 3, 
    bcmStatExpansionTypeDropPrecedenceMeter1Valid = 4, 
    bcmStatExpansionTypDropPrecedenceMeter1Value = 5, 
    bcmStatExpansionTypeDropPrecedenceMeter2Valid = 6, 
    bcmStatExpansionTypeDropPrecedenceMeter2Value = 7, 
    bcmStatExpansionTypeTrafficClass = 8, 
    bcmStatExpansionTypeDropPrecedenceMeterResolved = 9, /* Final dp. */
    bcmStatExpansionTypeDropPrecedenceInput = 10, 
    bcmStatExpansionTypeSystemMultiCast = 11, 
    bcmStatExpansionTypeEcnEligibleAndCni = 12, 
    bcmStatExpansionTypePortMetaData = 13, 
    bcmStatExpansionTypePpDropReason = 14, 
    bcmStatExpansionTypeMetaData = 15, 
    bcmStatExpansionTypeDiscardPp = 16, 
    bcmStatExpansionTypeDiscardTm = 17, 
    bcmStatExpansionTypeEgressTmActionType = 18, 
    bcmStatExpansionTypeLatencyBin = 19, 
    bcmStatExpansionTypeLatencyFlowProfile = 20, 
    bcmStatExpansionTypeTmLastCopy = 21, 
    bcmStatExpansionTypeMax = 22 
} bcm_stat_expansion_types_t;

/* Defines the counter interface key. */
typedef struct bcm_stat_counter_interface_key_s {
    bcm_core_t core_id;                 /* Core id explicit. */
    bcm_stat_counter_interface_type_t interface_source; /* Source. */
    int command_id;                     /* Command id. */
    int type_id;                        /* Object type id [0..3] . */
} bcm_stat_counter_interface_key_t;

/* Define the type and bitmap (if relevant) for each expansion element. */
typedef struct bcm_stat_expansion_element_s {
    bcm_stat_expansion_types_t type; 
    uint32 bitmap;                      /* Select specific bits from the type
                                           for the expansion. relevant only if
                                           user selected metadata or
                                           portMetaData types. */
} bcm_stat_expansion_element_t;

/* Defines the expansion selection elements. */
typedef struct bcm_stat_expansion_select_s {
    int nof_elements;                   /* Nof elements. */
    bcm_stat_expansion_element_t expansion_elements[bcmStatExpansionTypeMax]; /* Array which selects the expansion 
                                           types (and bitmaps) that will
                                           generate the expansion. */
} bcm_stat_expansion_select_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * API is used to configure the expansion selection per interface.It is
 * not allowed to create different expansions for different types belong
 * to the same interface, if the types are access to the same engine.
 */
extern int bcm_stat_counter_expansion_select_set(
    int unit, 
    uint32 flags, 
    bcm_stat_counter_interface_key_t *interface, 
    bcm_stat_expansion_select_t *expansion_select);

/* API is used to get the expansion selection configuration. */
extern int bcm_stat_counter_expansion_select_get(
    int unit, 
    uint32 flags, 
    bcm_stat_counter_interface_key_t *interface, 
    bcm_stat_expansion_select_t *expansion_select);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_STAT_COUNTER_MAPPING_FULL_SPREAD 0x1        /* Used to configure full
                                                          spread for the entire
                                                          expansion. All other
                                                          parameters in the
                                                          strucutre
                                                          bcm_stat_counter_set_map_t
                                                          are ignored.This flag
                                                          is allowed only if the
                                                          expansion is no bigger
                                                          than 5 bits */

/* 
 * Expansion data key. use it to determine the entry to the HW expansion
 * data table.
 */
typedef struct bcm_stat_expansion_data_key_s {
    bcm_stat_expansion_types_t type;    /* Type/filed from the expansion
                                           selection. */
    int value;                          /* Value for the type. -1 refer to all
                                           values. */
} bcm_stat_expansion_data_key_t;

/* Defines the value of the HW expansion data mapping table. */
typedef struct bcm_stat_expansion_data_value_s {
    int counter_set_offset; /* Counter set offset. */
    int valid;              /* Admission for this expansion data(value). */
} bcm_stat_expansion_data_value_t;

#define BCM_STAT_MAX_NOF_EXPANSION_KEY_ELEMENTS 0x7        /* max size of elements
                                                          that create the
                                                          key(entry) for the
                                                          data expansion table. */

/* 
 * Defines the set of conditions that will generate the exact key for the
 * HW data mapping table, and set it's value.
 */
typedef struct bcm_stat_expansion_data_mapping_s {
    int nof_key_conditions;             /* max bcmStatExpansionTypeMax. */
    bcm_stat_expansion_data_key_t key[BCM_STAT_MAX_NOF_EXPANSION_KEY_ELEMENTS]; /* Build the key from number of type
                                           values. */
    bcm_stat_expansion_data_value_t value; /* Set the output value of the table
                                           {admission and counter set offset}. */
} bcm_stat_expansion_data_mapping_t;

/* 
 * Defines the counter set size of the engine and the counter set
 * mapping.
 */
typedef struct bcm_stat_counter_set_map_s {
    int nof_entries;                    /* Nof entries to the expansion data
                                           mapping array. */
    bcm_stat_expansion_data_mapping_t *expansion_data_mapping; /* Data mapping array. max size=1024. */
} bcm_stat_counter_set_map_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* API determine the counter set mapping. */
extern int bcm_stat_counter_set_mapping_set(
    int unit, 
    uint32 flags, 
    bcm_stat_counter_database_t *database, 
    bcm_stat_counter_set_map_t *counter_set_map);

/* API get the counter set mapping. */
extern int bcm_stat_counter_set_mapping_get(
    int unit, 
    uint32 flags, 
    bcm_stat_counter_database_t *database, 
    bcm_stat_counter_set_map_t *counter_set_map);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Destination types for each DMA FIFO. */
typedef enum bcm_eviction_destination_type_e {
    bcmStatEvictionDestinationLocalHost = 0 
} bcm_eviction_destination_type_t;

/* Eviction record format enum. */
typedef enum bcm_eviction_record_format_e {
    bcmStatEvictionRecordFormatPhysical = 0, 
    bcmStatEvictionRecordFormatLogical = 1 
} bcm_eviction_record_format_t;

/* enum selects the source data type for conditional eviction. */
typedef enum bcm_stat_eviction_conditional_source_e {
    bcmStatEvictionConditionalSourceCommandData = 0, 
    bcmStatEvictionConditionalSourceUserData = 1 
} bcm_stat_eviction_conditional_source_t;

/* Enum select the qualifier for conditional eviction. */
typedef enum bcm_stat_eviction_conditional_qual_e {
    bcmStatEvictionConditionalQualAndEqualZero = 0, 
    bcmStatEvictionConditionalQualAndNoneEqualZero = 1, 
    bcmStatEvictionConditionalQualAndEqualAllOnes = 2, 
    bcmStatEvictionConditionalQualAndNoneEqualAllOnes = 3, 
    bcmStatEvictionConditionalQualOrEqualZero = 4, 
    bcmStatEvictionConditionalQualOrNoneEqualZero = 5, 
    bcmStatEvictionConditionalQualOrEqualAllOnes = 6, 
    bcmStatEvictionConditionalQualOrNoneEqualAllOnes = 7, 
    bcmStatEvictionConditionalQualDataBiggerThanSource = 8, 
    bcmStatEvictionConditionalQualSourceBiggerThanData = 9, 
    bcmStatEvictionConditionalQualSourceEqualToData = 10 
} bcm_stat_eviction_conditional_qual_t;

#define BCM_STAT_EVICTION_CONDITIONAL_ACTION_EVICT_COUNTER 0x00000001 /* conditional eviction
                                                          action evict counter */
#define BCM_STAT_EVICTION_CONDITIONAL_ACTION_RESET_COUNTER 0x00000002 /* conditional eviction
                                                          action reset counter */
#define BCM_STAT_EVICTION_CONDITIONAL_ACTION_EVICT_TIME 0x00000004 /* conditional eviction
                                                          action evict time */
#define BCM_STAT_EVICTION_CONDITIONAL_ACTION_INTERRUPT 0x00000008 /* conditional eviction
                                                          action interrupt */
#define BCM_STAT_EVICTION_CONDITIONAL_ACTION_NO_EVICTION 0x00000010 /* conditional eviction
                                                          action no eviction */

/* structure holds the conditional eviction configuration. */
typedef struct bcm_stat_eviction_conditional_s {
    bcm_stat_eviction_conditional_source_t condition_source_select; /* select the source to perform the
                                           condition operation. */
    uint64 condition_user_data;         /* if source is UserData, set the data
                                           value */
    bcm_stat_eviction_conditional_qual_t qualifier; /* conditional qualifier */
    uint32 action_flags;                /* conditional action flags */
} bcm_stat_eviction_conditional_t;

/* Holds the configuration of the eviction per engine. */
typedef struct bcm_stat_eviction_s {
    int dma_fifo_select;                /* DMA FIFO to select. */
    bcm_eviction_destination_type_t type; /* Dest type. */
    bcm_eviction_record_format_t record_format; /* (physical, logical). */
    int eviction_event_id;              /* Relevant only for logical format. */
    int eviction_algorithmic_disable;   /* If set, disable algorithmic (or
                                           probabilistic) scanning */
    bcm_stat_eviction_conditional_t cond; /* Set of parameters configured the
                                           conditional eviction. */
} bcm_stat_eviction_t;

#define BCM_STAT_EVICTION_CONDITIONAL_ENABLE 0x00000001 /* if set, conditional
                                                          eviction is enabled */

#ifndef BCM_HIDE_DISPATCHABLE

/* API to set the engine eviction configuration . */
extern int bcm_stat_counter_eviction_set(
    int unit, 
    uint32 flags, 
    bcm_stat_engine_t *engine, 
    bcm_stat_eviction_t *eviction);

/* API to get the engine eviction configuration. */
extern int bcm_stat_counter_eviction_get(
    int unit, 
    uint32 flags, 
    bcm_stat_engine_t *engine, 
    bcm_stat_eviction_t *eviction);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_STAT_COUNTER_CLEAR_ON_READ  0x1        /* Used to clear on read SW
                                                      counters */

/* 
 * Configuration of statistics to get, from a counter_set and engine
 * which defined explicitly.
 */
typedef struct bcm_stat_counter_explicit_input_data_s {
    bcm_stat_engine_t engine; 
    int object_stat_id;         /* The counter pointer that is being sent to the
                                   counter processor. */
    int type_id; 
    int nstat; 
    int *stat_arr; 
} bcm_stat_counter_explicit_input_data_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * API gets array of statistics for explicit counter_set and engine,
 * given by the user.
 */
extern int bcm_stat_counter_explicit_get(
    int unit, 
    uint32 flags, 
    bcm_stat_counter_explicit_input_data_t *stat_counter_input_data, 
    bcm_stat_counter_output_data_t *stat_counter_output_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* init bcm_stat_engine_t */
extern void bcm_stat_engine_t_init(
    bcm_stat_engine_t *stat_engine);

/* init bcm_stat_counter_database_t */
extern void bcm_stat_counter_database_t_init(
    bcm_stat_counter_database_t *stat_counter_database);

/* init bcm_stat_counter_explicit_input_data_t */
extern void bcm_stat_counter_explicit_input_data_t_init(
    bcm_stat_counter_explicit_input_data_t *stat_counter_explicit_input_data);

/* init bcm_stat_eviction_t */
extern void bcm_stat_eviction_t_init(
    bcm_stat_eviction_t *stat_eviction);

/* init bcm_stat_counter_set_map_t */
extern void bcm_stat_counter_set_map_t_init(
    bcm_stat_counter_set_map_t *stat_counter_set_map);

/* init bcm_stat_expansion_data_mapping_t */
extern void bcm_stat_expansion_data_mapping_t_init(
    bcm_stat_expansion_data_mapping_t *stat_expansion_data_mapping);

/* init bcm_stat_counter_interface_key_t */
extern void bcm_stat_counter_interface_key_t_init(
    bcm_stat_counter_interface_key_t *stat_counter_interface_key);

/* init bcm_stat_expansion_select_t */
extern void bcm_stat_expansion_select_t_init(
    bcm_stat_expansion_select_t *stat_expansion_select);

/* init bcm_stat_counter_interface_t */
extern void bcm_stat_counter_interface_t_init(
    bcm_stat_counter_interface_t *stat_counter_interface);

/* init bcm_stat_counter_enable_t */
extern void bcm_stat_counter_enable_t_init(
    bcm_stat_counter_enable_t *stat_counter_enable);

/* init bcm_stat_counter_output_data_t */
extern void bcm_stat_counter_output_data_t_init(
    bcm_stat_counter_output_data_t *stat_counter_output_data);

typedef struct bcm_stat_eviction_boundaries_s {
    int start; 
    int end; 
} bcm_stat_eviction_boundaries_t;

/* Return sequential eviction boundaries to the default values. */
#define BCM_STAT_EVICTION_RANGE_ALL 0x00000001 

#ifndef BCM_HIDE_DISPATCHABLE

/* Modify sequential boundaries. */
extern int bcm_stat_eviction_boundaries_set(
    int unit, 
    uint32 flags, 
    bcm_stat_engine_t *engine, 
    bcm_stat_eviction_boundaries_t *boundaries);

/* Get the values of the sequential boundaries. */
extern int bcm_stat_eviction_boundaries_get(
    int unit, 
    uint32 flags, 
    bcm_stat_engine_t *engine, 
    bcm_stat_eviction_boundaries_t *boundaries);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Defines the types (that user can select) of packet size adjustment per
 * stat source block.
 */
typedef enum bcm_stat_pkt_size_adjust_select_type_e {
    bcmStatPktSizeSelectCounterIngressHeaderTruncate = 0 
} bcm_stat_pkt_size_adjust_select_type_t;

/* 
 * Defines the key parameters to select a packet size adjustment type
 * (For counter processor, per source and command_id)
 */
typedef struct bcm_stat_counter_command_id_key_s {
    bcm_core_t core_id;                 /* Support specific core or ALL. */
    bcm_stat_counter_interface_type_t source; /* Select source. */
    int command_id;                     /* Select the command id. */
} bcm_stat_counter_command_id_key_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Enable/disable a packet size adjustment type (currently just
 * header-truncate), for ingress packet size adjustment calculation
 * (compensation).
 */
extern int bcm_stat_pkt_size_adjust_select_set(
    int unit, 
    int flags, 
    bcm_stat_counter_command_id_key_t *key, 
    bcm_stat_pkt_size_adjust_select_type_t select_type, 
    int enable);

/* 
 * Get if packet size adjustment type is enabled or disabled according to
 * the given key.
 */
extern int bcm_stat_pkt_size_adjust_select_get(
    int unit, 
    int flags, 
    bcm_stat_counter_command_id_key_t *key, 
    bcm_stat_pkt_size_adjust_select_type_t select_type, 
    int *enable);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_STAT_COUNTER_OVERFLOW_MODE_RESET 0          /* Refer to control enum
                                                          bcmStatCounterOverflowMode
                                                          - reset to 0. */
#define BCM_STAT_COUNTER_OVERFLOW_MODE_WRAP_AROUND 1          /* Refer to control enum
                                                          bcmStatCounterOverflowMode
                                                          - handle overflow for
                                                          source OAM. */
#define BCM_STAT_COUNTER_OVERFLOW_MODE_FIXED_AT_MAX 2          /* Refer to control enum
                                                          bcmStatCounterOverflowMode
                                                          - use mode stuck all
                                                          at ones. */

/* Filter groups */
typedef enum bcm_stat_counter_group_filter_e {
    bcmStatCounterGroupFilterGlobalResourcesDrop = 0, 
    bcmStatCounterGroupFilterVoqResourcesDrop = 1, 
    bcmStatCounterGroupFilterWredDrop = 2, 
    bcmStatCounterGroupFilterOcbResourcesDrop = 3, 
    bcmStatCounterGroupFilterVsqResourcesDrop = 4, 
    bcmStatCounterGroupFilterLatencyDrop = 5, 
    bcmStatCounterGroupFilterMiscDrop = 6, 
    bcmStatCounterGroupFilterUserDefinedGroup = 7, 
    bcmStatCounterGroupFilterCount = 8 
} bcm_stat_counter_group_filter_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set counter source to filter in or out certain filter criteria. */
extern int bcm_stat_counter_filter_group_set(
    int unit, 
    uint32 flags, 
    bcm_stat_counter_command_id_key_t *key, 
    bcm_stat_counter_group_filter_t filter, 
    int is_active);

/* Determine if given filter criteria is active or not. */
extern int bcm_stat_counter_filter_group_get(
    int unit, 
    uint32 flags, 
    bcm_stat_counter_command_id_key_t *key, 
    bcm_stat_counter_group_filter_t filter, 
    int *is_active);

#endif /* BCM_HIDE_DISPATCHABLE */

/* init bcm_stat_counter_command_id_key_t */
extern void bcm_stat_counter_command_id_key_t_init(
    bcm_stat_counter_command_id_key_t *stat_counter_command_id_key);

/* init bcm_stat_eviction_boundaries_t */
extern void bcm_stat_eviction_boundaries_t_init(
    bcm_stat_eviction_boundaries_t *stat_eviction_boundaries);

#define BCM_STAT_EXTERNAL       0x00000001 

#define BCM_STAT_FULL_MASK      0xFFFFFFFF /* mask the whole field */

#define BCM_STAT_INGRESS        0x1        
#define BCM_STAT_EGRESS         0x2        

#define BCM_STAT_SOURCE_MAPPING_FIRST_PORT  0x1        
#define BCM_STAT_SOURCE_MAPPING_SECOND_PORT 0x2        

/* List of elements that build the statistics interface record format */
typedef enum bcm_stat_stif_record_element_type_e {
    bcmStatStifRecordElementObj0 = 0,   /* mask=BCM_STAT_FULL_MASK must be set */
    bcmStatStifRecordElementObj1 = 1,   /* mask=BCM_STAT_FULL_MASK must be set */
    bcmStatStifRecordElementObj2 = 2,   /* mask=BCM_STAT_FULL_MASK must be set */
    bcmStatStifRecordElementObj3 = 3,   /*  mask=BCM_STAT_FULL_MASK must be set */
    bcmStatStifRecordElementPacketSize = 4, /* mask=BCM_STAT_FULL_MASK must be set */
    bcmStatStifRecordElementOpCode = 5, /* mask=BCM_STAT_FULL_MASK must be used.
                                           8 bits represent 4 objs opCode */
    bcmStatStifRecordElementIngressDispositionIsDrop = 6, 
    bcmStatStifRecordElementIngressTmDropReason = 7, 
    bcmStatStifRecordElementIngressTrafficClass = 8, 
    bcmStatStifRecordElementIngressIncomingDropPrecedence = 9, 
    bcmStatStifRecordElementIngressDropPrecedenceMeterResolved = 10, 
    bcmStatStifRecordElementIngressDropPrecedenceMeter0Valid = 11, 
    bcmStatStifRecordElementIngressDropPrecedenceMeter0Value = 12, 
    bcmStatStifRecordElementIngressDropPrecedenceMeter1Valid = 13, 
    bcmStatStifRecordElementIngressDropPrecedenceMeter1Value = 14, 
    bcmStatStifRecordElementIngressDropPrecedenceMeter2Valid = 15, 
    bcmStatStifRecordElementIngressDropPrecedenceMeter2Value = 16, 
    bcmStatStifRecordElementIngressCore = 17, 
    bcmStatStifRecordElementIngressPpMetaData = 18, 
    bcmStatStifRecordElementIngressQueueNumber = 19, 
    bcmStatStifRecordElementEgressMetaDataMultiCast = 20, /* mask=BCM_STAT_FULL_MASK must be set */
    bcmStatStifRecordElementEgressMetaDataPpDropReason = 21, /* mask=BCM_STAT_FULL_MASK must be set */
    bcmStatStifRecordElementEgressMetaDataPort = 22, /* mask=BCM_STAT_FULL_MASK must be set */
    bcmStatStifRecordElementEgressMetaDataEcnEligibleAndCni = 23, /* mask=BCM_STAT_FULL_MASK must be set */
    bcmStatStifRecordElementEgressMetaDataTrafficClass = 24, /* mask=BCM_STAT_FULL_MASK must be set */
    bcmStatStifRecordElementEgressMetaDataDropPrecedence = 25, /* mask=BCM_STAT_FULL_MASK must be set */
    bcmStatStifRecordElementEgressMetaDataObj0 = 26, /* mask=BCM_STAT_FULL_MASK must be set */
    bcmStatStifRecordElementEgressMetaDataObj1 = 27, /* mask=BCM_STAT_FULL_MASK must be set */
    bcmStatStifRecordElementEgressMetaDataObj2 = 28, /* mask=BCM_STAT_FULL_MASK must be set */
    bcmStatStifRecordElementEgressMetaDataObj3 = 29, /* mask=BCM_STAT_FULL_MASK must be set */
    bcmStatStifRecordElementEgressMetaDataCore = 30, /* mask=BCM_STAT_FULL_MASK must be set */
    bcmStatStifRecordElementIngressIsLastCopy = 31 /* Enable last copy for MC */
} bcm_stat_stif_record_element_type_t;

/* 
 * Structure holds one element and its position, in the statistics
 * interface record format
 */
typedef struct bcm_stat_stif_record_format_element_s {
    bcm_stat_stif_record_element_type_t element_type; 
    uint32 mask; 
} bcm_stat_stif_record_format_element_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * API set the statistics interface record format (for billing mode
 * INGRESS or EGRESS)
 */
extern int bcm_stat_stif_record_format_set(
    int unit, 
    int flags, 
    int nof_elements, 
    bcm_stat_stif_record_format_element_t *record_format_elements);

/* 
 * API get the statistics interface record format (for billing mode
 * INGRESS or EGRESS)
 */
extern int bcm_stat_stif_record_format_get(
    int unit, 
    int flags, 
    int max_nof_elements, 
    bcm_stat_stif_record_format_element_t *elements_array, 
    int *nof_elements);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * List of statistics interface sources that generate the statistics
 * records.
 */
typedef enum bcm_stat_stif_source_type_e {
    bcmStatStifSourceIngressEnqueue = 0, 
    bcmStatStifSourceIngressDequeue = 1, 
    bcmStatStifSourceIngressScrubber = 2, 
    bcmStatStifSourceEgressDequeue = 3 
} bcm_stat_stif_source_type_t;

/* 
 * Structure hold one statistic interface source which generate
 * statistics records.
 */
typedef struct bcm_stat_stif_source_s {
    bcm_core_t core; 
    bcm_stat_stif_source_type_t src_type; 
} bcm_stat_stif_source_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * API map statistics source (statistics interface source) to logical
 * port. port=invalid means disconnect the source
 */
extern int bcm_stat_stif_source_mapping_set(
    int unit, 
    int flags, 
    bcm_stat_stif_source_t source, 
    bcm_port_t port);

/* 
 * API get the mapping of statistics source to its logical port.
 * port=invalid means that the source is disconnected
 */
extern int bcm_stat_stif_source_mapping_get(
    int unit, 
    int flags, 
    bcm_stat_stif_source_t source, 
    bcm_port_t *port);

#endif /* BCM_HIDE_DISPATCHABLE */

/* List of stat control types. */
typedef enum bcm_stat_control_e {
    bcmStatControlStifFcEnable = 0, 
    bcmStatControlCounterThreadEnable = 1 /* General enabler for the SW BG thread
                                           of counter collecting. Irelevant for
                                           specific engine. (one thread for all
                                           engines) */
} bcm_stat_control_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* API stat control set. */
extern int bcm_stat_control_set(
    int unit, 
    int flags, 
    bcm_stat_control_t type, 
    int arg);

/* API stat control get. */
extern int bcm_stat_control_get(
    int unit, 
    int flags, 
    bcm_stat_control_t type, 
    int *arg);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Stat Flex Counter Information */
typedef struct bcm_stat_custom_counter_info_s {
    uint32 ctr_tbl_mode_id;         /* Counter table Mode Identifier */
    uint32 ctr_tbl_pool_id;         /* Counter table Pool Id */
    uint32 ctr_tbl_pool_base_idx;   /* Counter table Pool Base Index */
    uint32 offset_tbl_base_idx;     /* Offset table Pool Base Index */
    uint32 num_counters;            /* Number of counter entries created */
    uint32 stat_counter_id;         /* Stat Counter Id */
    bcm_stat_object_t object;       /* Stat Accounting object */
} bcm_stat_custom_counter_info_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Associate an accounting object to customized group mode with user
 * configured pool id.
 */
extern int bcm_stat_custom_group_id_create(
    int unit, 
    uint32 mode_id, 
    bcm_stat_object_t object, 
    uint32 pool_id, 
    uint32 base_idx, 
    bcm_stat_custom_counter_info_t *counter_info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_stat_custom_base_index_action_e */
typedef enum bcm_stat_custom_base_index_action_e {
    bcmStatCustomBaseIdxAlloc = 0,  /* Counter Base Index Alloc */
    bcmStatCustomBaseIdxMove = 1    /* Counter Base Index Move */
} bcm_stat_custom_base_index_action_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * This function is used to move the old set of counters to new free
 * location provided by user.
 */
extern int bcm_stat_custom_counter_id_move(
    int unit, 
    bcm_stat_custom_base_index_action_t idx_action, 
    bcm_stat_custom_counter_info_t counter_info_old, 
    bcm_stat_custom_counter_info_t *counter_info_new);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize flex stat counter information */
extern void bcm_stat_custom_counter_info_t_init(
    bcm_stat_custom_counter_info_t *counter_info);

/* PP statistic profile. */
typedef struct bcm_stat_pp_profile_info_s {
    int counter_command_id;             /* Command id (interface id). */
    int stat_object_type;               /* Statistic object type in range
                                           0-BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES-1 */
    int meter_command_id;               /* Relevant only for egress */
    int meter_qos_map_id;               /* Relevant only for egress */
    uint8 is_meter_enable;              /* Allow metering */
    uint8 is_fp_cs_var;                 /* Enable PMF2 context selection */
    int ingress_tunnel_metadata_size;   /* Ingress metadata size in buffer (in
                                           bits) for terminated headers */
    int ingress_tunnel_metadata_start_bit; /* Ingress metadata start bit in buffer
                                           for terminated headers */
    int ingress_forward_metadata_size;  /* Ingress metadata size in buffer (in
                                           bits) for forwarding header */
    int ingress_forward_metadata_start_bit; /* Ingress metadata start bit in buffer
                                           for forwarding header */
    int ingress_fwd_plus_one_metadata_size; /* Ingress metadata size in buffer (in
                                           bits) for forwarding plus one header */
    int ingress_fwd_plus_one_metadata_start_bit; /* Ingress metadata start bit in buffer
                                           for forwarding plus one header */
    uint8 is_protection_fec;            /* Support protection-FEC */
} bcm_stat_pp_profile_info_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Map {statistic command, statistic profile} to the profile properties */
extern int bcm_stat_pp_profile_create(
    int unit, 
    int flags, 
    bcm_stat_counter_interface_type_t engine_source, 
    int *stat_pp_profile, 
    bcm_stat_pp_profile_info_t *stat_pp_profile_info);

/* Get profile properties according to stat_pp_profile */
extern int bcm_stat_pp_profile_get(
    int unit, 
    int stat_pp_profile, 
    bcm_stat_pp_profile_info_t *stat_pp_profile_info);

/* Delete profile properties according to stat_pp_profile */
extern int bcm_stat_pp_profile_delete(
    int unit, 
    int stat_pp_profile);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Stat PP profile flags */
#define BCM_STAT_PP_PROFILE_WITH_ID 0x0001     /* Add stat profile instance
                                                  using a specific id. */
#define BCM_STAT_PP_PROFILE_REPLACE 0x0002     /* Replace data in an existing
                                                  stat profile instance. */

/* Initialize stat_pp_profile_info_t */
extern void bcm_stat_pp_profile_info_t_init(
    bcm_stat_pp_profile_info_t *pp_profile);

/* PP statistic profile information */
typedef struct bcm_stat_pp_info_s {
    uint32 flags;           /* BCM_STAT_INFO_WITH_ID, BCM_STAT_INFO_REPLACE */
    uint32 stat_id;         /* object statistic id (the counter_pointer value as
                               sent to the crps) */
    int stat_pp_profile;    /* profile */
} bcm_stat_pp_info_t;

/* Initialize stat_pp_info_t structure */
extern void bcm_stat_pp_info_t_init(
    bcm_stat_pp_info_t *stat_pp_info);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Map physical or logical port to stat id and stat profile. In case of
 * DPC LIF the api can accept core parameter to distinguish between the
 * stat id on the two cores.
 */
extern int bcm_gport_stat_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_core_t core_id, 
    bcm_stat_counter_interface_type_t engine_source, 
    bcm_stat_pp_info_t stat_info);

/* 
 * Get mapping of physical or logical port to stat id and stat profile.
 * In case of DPC LIF the api can accept core parameter to distinguish
 * between the stat id on the two cores.
 */
extern int bcm_gport_stat_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_core_t core_id, 
    bcm_stat_counter_interface_type_t engine_source, 
    bcm_stat_pp_info_t *stat_info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Structure holds the key for decoding the stat-id of end to end latency
 * per destination port.
 */
typedef struct bcm_stat_latency_port_stat_id_key_s {
    int is_multicast;   /* Multicast. */
    int traffic_class;  /* Traffic class. */
    bcm_port_t port;    /* Port. */
} bcm_stat_latency_port_stat_id_key_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * For end to end latency per destination port, API give back the decoded
 * latency stat-id.
 */
extern int bcm_stat_latency_port_stat_id_get(
    int unit, 
    int flags, 
    bcm_stat_latency_port_stat_id_key_t *key, 
    int *stat_id);

/* Set drop reasons to be part of certain group. */
extern int bcm_stat_group_drop_reasons_set(
    int unit, 
    uint32 flags, 
    bcm_stat_counter_group_filter_t group, 
    bcm_cosq_drop_reason_t *drop_reasons_array, 
    int drop_reasons_count);

/* Get drop reasons represented by given group. */
extern int bcm_stat_group_drop_reasons_get(
    int unit, 
    uint32 flags, 
    bcm_stat_counter_group_filter_t group, 
    int max_drop_reason_count, 
    bcm_cosq_drop_reason_t *drop_reasons_array, 
    int *drop_reasons_count);

/* Modify sequential boundaries - in stat object id resolution. */
extern int bcm_stat_database_eviction_boundaries_set(
    int unit, 
    uint32 flags, 
    bcm_stat_counter_database_t *database, 
    int type_id, 
    bcm_stat_eviction_boundaries_t *boundaries);

/* 
 * Get the values of the sequential boundaries - in stat object id
 * resolution.
 */
extern int bcm_stat_database_eviction_boundaries_get(
    int unit, 
    uint32 flags, 
    bcm_stat_counter_database_t *database, 
    int type_id, 
    bcm_stat_eviction_boundaries_t *boundaries);

#endif /* BCM_HIDE_DISPATCHABLE */

extern void bcm_stat_pp_metadata_info_t_init(
    bcm_stat_pp_metadata_info_t *metadata_info);

#endif /* __BCM_STAT_H__ */
