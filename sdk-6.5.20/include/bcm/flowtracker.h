/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_FLOWTRACKER_H__
#define __BCM_FLOWTRACKER_H__

#include <bcm/port.h>
#include <bcm/collector.h>
#include <bcm/pkt.h>

/* Flowtracker IPFIX observation domain. */
typedef uint32 bcm_flowtracker_ipfix_observation_domain_t;

/* The different timestamp sources in flowtracker to compute time delay */
typedef enum bcm_flowtracker_timestamp_source_e {
    bcmFlowtrackerTimestampSourceLegacy = 0, /* Legacy source */
    bcmFlowtrackerTimestampSourceNTP = 1, /* Network Time Protocol source */
    bcmFlowtrackerTimestampSourcePTP = 2, /* Precision Time Protocol source */
    bcmFlowtrackerTimestampSourceCMIC = 3, /* CMIC source */
    bcmFlowtrackerTimestampSourceIngressFlowtrackerStage = 4, /* Timestamp from logical table select
                                           table */
    bcmFlowtrackerTimestampSourcePacket = 5, /* Timestamp captured from incoming
                                           packet */
    bcmFlowtrackerTimestampSourceIngress = 6, /* Timestamp captured when packet
                                           ingressed in pipeline */
    bcmFlowtrackerTimestampSourceNewLearn = 7, /* Timestamp captured when flow got
                                           learnt */
    bcmFlowtrackerTimestampSourceFlowStart = 8, /* Timestamp captured when flow started */
    bcmFlowtrackerTimestampSourceFlowEnd = 9, /* Timestamp captured when flow ended */
    bcmFlowtrackerTimestampSourceCheckEvent1 = 10, /* Timestamp captured when event1
                                           happened in checker */
    bcmFlowtrackerTimestampSourceCheckEvent2 = 11, /* Timestamp captured when event2
                                           happened in checker */
    bcmFlowtrackerTimestampSourceCheckEvent3 = 12, /* Timestamp captured when event3
                                           happened in checker */
    bcmFlowtrackerTimestampSourceCheckEvent4 = 13, /* Timestamp captured when event4
                                           happened in checker */
    bcmFlowtrackerTimestampSourceCMICTxStart = 14, /* Timestamp captured at egress when
                                           packet tx start */
    bcmFlowtrackerTimestampSourceCMICTxEnd = 15, /* Timestamp captured at egress when
                                           packet tx end */
    bcmFlowtrackerTimestampSourceCount = 16 /* Always Last. Not a usable value */
} bcm_flowtracker_timestamp_source_t;

#define BCM_FLOWTRACKER_TIMESTAMP_SOURCE_STRINGS \
{ \
    "Legacy", \
    "NTP", \
    "PTP", \
    "CMIC", \
    "IngressFlowtrackerStage", \
    "Packet", \
    "Ingress", \
    "NewLearn", \
    "FlowStart", \
    "FlowEnd", \
    "CheckEvent1", \
    "CheckEvent2", \
    "CheckEvent3", \
    "CheckEvent4", \
    "CMICTxStart", \
    "CMICTxEnd"  \
}

/* Flowtracker Drop Reason in pipeline. */
typedef enum bcm_flowtracker_drop_reason_e {
    bcmFlowtrackerDropReasonInvalid = 0, /* Invalid packet drop reason */
    bcmFlowtrackerDropReasonMmuIngressPortPG = 1, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuIngressPortPool = 2, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuIngressPortGreen = 3, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuIngressPortYellow = 4, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuIngressPortRed = 5, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuIngressHeadroom = 6, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressUCQueueGreen = 7, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressUCQueueYellow = 8, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressUCQueueRed = 9, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressUCQueueGroupGreen = 10, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressUCQueueGroupYellow = 11, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressUCQueueGroupRed = 12, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressUCPortPoolGreen = 13, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressUCPortPoolYellow = 14, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressUCPortPoolRed = 15, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressUCPoolGreen = 16, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressUCPoolYellow = 17, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressUCPoolRed = 18, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressWREDUCQueueGreen = 19, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressWREDUCQueueYellow = 20, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressWREDUCQueueRed = 21, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressWREDUCQueueGroupGreen = 22, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressWREDUCQueueGroupYellow = 23, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressWREDUCQueueGroupRed = 24, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressWREDUCPortPoolGreen = 25, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressWREDUCPortPoolYellow = 26, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressWREDUCPortPoolRed = 27, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressWREDUCMin = 28, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressWREDUCPool = 29, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressRQEQueueGreen = 30, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressRQEQueueYellow = 31, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressRQEQueueRed = 32, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressRQEPoolGreen = 33, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressRQEPoolYellow = 34, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressRQEPoolRed = 35, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressMCQueueGreen = 36, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressMCQueueYellow = 37, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressMCQueueRed = 38, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressMCPortPoolGreen = 39, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressMCPortPoolYellow = 40, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressMCPortPoolRed = 41, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressMCPoolGreen = 42, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressMCPoolYellow = 43, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressMCPoolRed = 44, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuCFAP = 45, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuIngressWRED = 46, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressUC = 47, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuIngress = 48, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressMC = 49, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmuEgressRQE = 50, /* Packet dropped at Mmu due to */
    bcmFlowtrackerDropReasonMmu = 51,   /* Packet dropped by MMU. */
    bcmFlowtrackerDropReasonMmuIpMtu = 52, /* Packet dropped at Mmu due to IP MTU
                                           check fail. */
    bcmFlowtrackerDropReasonIngControlFrame = 53, /* Packet dropped at ingress due to
                                           control frame. */
    bcmFlowtrackerDropReasonIngL3DosAttack = 54, /* Packet dropped at ingress due to L3
                                           Dos attack. */
    bcmFlowtrackerDropReasonIngHigigLoopback = 55, /* Packet dropped at ingress due to
                                           HiGig loopback. */
    bcmFlowtrackerDropReasonIngiHigigUnknownOpcode = 56, /* Packet dropped at ingress due to
                                           HiGig unknown opcode. */
    bcmFlowtrackerDropReasonIngHigigHdrError = 57, /* Packet dropped at ingress due to
                                           HiGig header error. */
    bcmFlowtrackerDropReasonIngLagFailLoopback = 58, /* Packet dropped at ingress due to link
                                           aggregation loopback fail. */
    bcmFlowtrackerDropReasonIngMacSAEqualsDA = 59, /* Packet dropped at ingress due to MAC
                                           source address equals destination
                                           address. */
    bcmFlowtrackerDropReasonIngL4DosAttack = 60, /* Packet dropped at ingress due to L4
                                           Dos attack. */
    bcmFlowtrackerDropReasonIngNIVDot1p = 61, /* Packet dropped at ingress due to NIV
                                           DOT1P. */
    bcmFlowtrackerDropReasonIngNIVVntagPresent = 62, /* Packet dropped at ingress due to NIV
                                           VNTAG present. */
    bcmFlowtrackerDropReasonIngNIVVntagNotPresent = 63, /* Packet dropped at ingress due to NIV
                                           VNTAG not present. */
    bcmFlowtrackerDropReasonIngNIVVntagFormat = 64, /* Packet dropped at ingress due to NIV
                                           VNTAG format. */
    bcmFlowtrackerDropReasonIngUnknownSubtendingPort = 65, /* Packet dropped at ingress due to
                                           unknown subtending port. */
    bcmFlowtrackerDropReasonIngIPinIPTunnelProcess = 66, /* Packet dropped at ingress due to IP
                                           in IP tunnel process failure. */
    bcmFlowtrackerDropReasonIngMiMTunnelProcess = 67, /* Packet dropped at ingress due to MiM
                                           tunnel process failure. */
    bcmFlowtrackerDropReasonIngTunnelInterfaceCheck = 68, /* Packet dropped at ingress due to
                                           tunnel interface check failure. */
    bcmFlowtrackerDropReasonIngMplsInvalidControlWord = 69, /* Packet dropped at ingress due to MPLS
                                           invalid control word. */
    bcmFlowtrackerDropReasonIngMplsGalNotBos = 70, /* Packet dropped at ingress due to MPLS
                                           Generic Label is not BOS. */
    bcmFlowtrackerDropReasonIngMplsInvalidPayload = 71, /* Packet dropped at ingress due to MPLS
                                           invalid payload. */
    bcmFlowtrackerDropReasonIngMplsEntropyLabel = 72, /* Packet dropped at ingress due to MPLS
                                           entropy label in unallowed range. */
    bcmFlowtrackerDropReasonIngMplsLabelLookupMiss = 73, /* Packet dropped at ingress due to MPLS
                                           label loopkup miss. */
    bcmFlowtrackerDropReasonIngMplsInvalidAction = 74, /* Packet dropped at ingress due to MPLS
                                           invalid action. */
    bcmFlowtrackerDropReasonIngTunnelTTLCheck = 75, /* Packet dropped at ingress due to
                                           tunnel ttl check failure. */
    bcmFlowtrackerDropReasonIngTunnelSHIMHDRError = 76, /* Packet dropped at ingress due to
                                           tunnel shim header error. */
    bcmFlowtrackerDropReasonIngObjValidation = 77, /* Packet dropped at ingress due to
                                           tunnel object validation failure. */
    bcmFlowtrackerDropReasonIngInvalidVlan = 78, /* Packet dropped at ingress due to VLAN
                                           not valid. */
    bcmFlowtrackerDropReasonIngPortNotInVlanMember = 79, /* Packet dropped at ingress due to port
                                           not in VLAN membership. */
    bcmFlowtrackerDropReasonIngInvalidTpid = 80, /* Packet dropped at ingress due to TPID
                                           mismatch. */
    bcmFlowtrackerDropReasonIngPvlanMismatch = 81, /* Packet dropped at ingress due to
                                           PVLAN VID mismatch. */
    bcmFlowtrackerDropReasonIngMACIPBindCheck = 82, /* Packet dropped at ingress due to mac
                                           ip bind check failure. */
    bcmFlowtrackerDropReasonIngTunnelAdapt4LookupMiss = 83, /* Packet dropped at ingress due to
                                           tunnel adaptation table 4 lookup
                                           miss. */
    bcmFlowtrackerDropReasonIngTunnelAdapt3LookupMiss = 84, /* Packet dropped at ingress due to
                                           tunnel adaptation table 3 lookup
                                           miss. */
    bcmFlowtrackerDropReasonIngTunnelAdapt2LookupMiss = 85, /* Packet dropped at ingress due to
                                           tunnel adaptation table 2 lookup
                                           miss. */
    bcmFlowtrackerDropReasonIngTunnelAdapt1LookupMiss = 86, /* Packet dropped at ingress due to
                                           tunnel adaptation table 1 lookup
                                           miss. */
    bcmFlowtrackerDropReasonIngFieldProcessorLookup = 87, /* Packet dropped at ingress due to
                                           field processor lookup stage. */
    bcmFlowtrackerDropReasonIngFcoeVft = 88, /* Packet dropped at ingress due to Fcoe
                                           VFT drop. */
    bcmFlowtrackerDropReasonIngFcoeSrcBindCheck = 89, /* Packet dropped at ingress due to FCOE
                                           source bind check failure. */
    bcmFlowtrackerDropReasonIngFcoeSrcFpmaCheck = 90, /* Packet dropped at ingress due to
                                           source FPMA prefix check failure. */
    bcmFlowtrackerDropReasonIngVfiPt2Pt = 91, /* Packet dropped at ingress due to
                                           arrival from a vp that is not
                                           programmed in PT2PT connection. */
    bcmFlowtrackerDropReasonIngSpanningTreeNotForwarding = 92, /* Packet dropped at ingress due to
                                           Spanning tree not forwarding. */
    bcmFlowtrackerDropReasonIngMcastIndexError = 93, /* Packet dropped at ingress due to
                                           multicast index error. */
    bcmFlowtrackerDropReasonIngCompositeError = 94, /* Packet dropped at ingress due to HW
                                           related error like lookup table with
                                           parity error. */
    bcmFlowtrackerDropReasonIngBpdu = 95, /* BPDU packet drop at ingress. */
    bcmFlowtrackerDropReasonIngProtocol = 96, /* Protocol packet drop at ingress. */
    bcmFlowtrackerDropReasonIngCml = 97, /* Packet dropped at ingress due to CML
                                           bit. */
    bcmFlowtrackerDropReasonIngSrcRoute = 98, /* Packet dropped at ingress due to
                                           source route. */
    bcmFlowtrackerDropReasonIngL2SrcDiscard = 99, /* Packet dropped at ingress due to L2
                                           source discard. */
    bcmFlowtrackerDropReasonIngL2SrcStaticMove = 100, /* Packet dropped at ingress due to L2
                                           source static move. */
    bcmFlowtrackerDropReasonIngL2DstDiscard = 101, /* Packet dropped at ingress due to L2
                                           destination discard. */
    bcmFlowtrackerDropReasonIngL3Disable = 102, /* Packet dropped at ingress due to L3
                                           disabled. */
    bcmFlowtrackerDropReasonIngSrcMacZero = 103, /* Packet dropped at ingress due to
                                           source mac zero. */
    bcmFlowtrackerDropReasonIngVLANCC = 104, /* Packet dropped at ingress due to vlan
                                           cc or pbt failure. */
    bcmFlowtrackerDropReasonIngClassBasedSM = 105, /* Packet dropped at ingress due to
                                           class based SM failure. */
    bcmFlowtrackerDropReasonIngTimeSyncPkt = 106, /* Network time sync packet drop at
                                           ingress. */
    bcmFlowtrackerDropReasonIngIPMCProcess = 107, /* Packet dropped at ingress due to ipmc
                                           process failure. */
    bcmFlowtrackerDropReasonIngMyStation = 108, /* Packet dropped at ingress due to my
                                           station. */
    bcmFlowtrackerDropReasonIngPvlanVpFilter = 109, /* Packet dropped at ingress due to
                                           pvlan vp filer. */
    bcmFlowtrackerDropReasonIngPktFlowSelectMiss = 110, /* Packet dropped at ingress due to flow
                                           select miss. */
    bcmFlowtrackerDropReasonIngL3DstDiscard = 111, /* Packet dropped at Mmu due to L3
                                           destination discard at ingress. */
    bcmFlowtrackerDropReasonIngTunnelDecapECN = 112, /* Packet dropped at ingress due to
                                           tunnel decap ECN. */
    bcmFlowtrackerDropReasonIngPktFlowSelectAction = 113, /* Packet dropped at ingress due to flow
                                           select action. */
    bcmFlowtrackerDropReasonIngL3SrcDiscard = 114, /* Packet dropped at ingress due to L3
                                           source discard. */
    bcmFlowtrackerDropReasonIngFcoeZoneLookupMiss = 115, /* Packet dropped at ingress due to fcoe
                                           zone lookup miss. */
    bcmFlowtrackerDropReasonIngL3Ttl = 116, /* Packet dropped at ingress due to l3
                                           ttl error. */
    bcmFlowtrackerDropReasonIngL3Hdr = 117, /* Packet dropped at ingress due to l3
                                           header error. */
    bcmFlowtrackerDropReasonIngL2Hdr = 118, /* Packet dropped at ingress due to l2
                                           header error. */
    bcmFlowtrackerDropReasonIngL3SrcLookupMiss = 119, /* Packet dropped at ingress due to l3
                                           source lookup miss. */
    bcmFlowtrackerDropReasonIngL3DstLookupMiss = 120, /* Packet dropped at ingress due to l3
                                           destination lookup miss. */
    bcmFlowtrackerDropReasonIngL2TtlError = 121, /* Packet dropped at ingress due to l2
                                           ttl error. */
    bcmFlowtrackerDropReasonIngL2RpfCheck = 122, /* Packet dropped at ingress due to l2
                                           rpf check. */
    bcmFlowtrackerDropReasonIngPfm = 123, /* Packet dropped at ingress due to port
                                           filtering mode. */
    bcmFlowtrackerDropReasonIngTagUntagDiscard = 124, /* Packet dropped at ingress due to port
                                           untag discard. */
    bcmFlowtrackerDropReasonIngEcmpResolution = 125, /* Packet dropped at ingress due to ecmp
                                           resolution. */
    bcmFlowtrackerDropReasonIngStormControl = 126, /* Packet dropped at ingress due to
                                           storm control metering. */
    bcmFlowtrackerDropReasonIngFcoeZoneCheck = 127, /* Packet dropped at ingress due to FCOE
                                           zone check is a miss */
    bcmFlowtrackerDropReasonIngFPChangeL2FieldNoRedirectDrop = 128, /* Packet dropped at ingress due to l2
                                           field no direct drop. */
    bcmFlowtrackerDropReasonIngMulticast = 129, /* Multicast Packet dropped at ingress. */
    bcmFlowtrackerDropReasonIngTunnelError = 130, /* Packet dropped at ingress due to
                                           tunnel error. */
    bcmFlowtrackerDropReasonIngNextHop = 131, /* Next Hop drop at ingress. */
    bcmFlowtrackerDropReasonIngUrpfCheck = 132, /* Packet dropped at ingress due to urpf
                                           check failure. */
    bcmFlowtrackerDropReasonIngNat = 133, /* NAT process failure eg. destination
                                           lookup miss or Hairpin drop at
                                           ingress. */
    bcmFlowtrackerDropReasonIngProtectionData = 134, /* Packet dropped at ingress due to
                                           protection data. */
    bcmFlowtrackerDropReasonIngStageIngress = 135, /* Packet dropped at ingress due to
                                           field ingress stage. */
    bcmFlowtrackerDropReasonEgrAdapt1Miss = 136, /* Packet dropped at egress due to
                                           ADAPT_1 miss drop condition. */
    bcmFlowtrackerDropReasonEgrAdapt2Miss = 137, /* Packet dropped at egress due to
                                           ADAPT_2 miss drop condition. */
    bcmFlowtrackerDropReasonEgrTtl = 138, /* Packet dropped at egress due to TTL. */
    bcmFlowtrackerDropReasonEgrFwdDomainNotFound = 139, /* Packet dropped at egress due to
                                           FWD_DOMAIN not found error. */
    bcmFlowtrackerDropReasonEgrFwdDomainNotMember = 140, /* Packet dropped at egress due to
                                           packet not member of FWD_DOMAIN. */
    bcmFlowtrackerDropReasonEgrStgBlock = 141, /* Packet dropped at egress due to
                                           spanning tree. */
    bcmFlowtrackerDropReasonEgrIpmcL2Self = 142, /* Packet dropped at egress due to IPMC
                                           L2 self port. */
    bcmFlowtrackerDropReasonEgrNonUCastPrune = 143, /* Packet dropped at egress due to non
                                           unicast prune drop. */
    bcmFlowtrackerDropReasonEgrSrcVirtualPort = 144, /* Packet dropped at egress due to
                                           source virtual port removal. */
    bcmFlowtrackerDropReasonEgrVplagPrune = 145, /* Packet dropped at egress due to VPLAG
                                           packet pruning. */
    bcmFlowtrackerDropReasonEgrSplitHorizon = 146, /* Packet dropped at egress due to split
                                           horizon. */
    bcmFlowtrackerDropReasonEgrIpmcL3SelfIntf = 147, /* Packet dropped at egress due to IPMC
                                           L3 Self Interface. */
    bcmFlowtrackerDropReasonEgrPurgePkt = 148, /* Packet dropped at egress due to
                                           purge. */
    bcmFlowtrackerDropReasonEgrStgDisable = 149, /* Packet dropped at egress due to
                                           spanning tree disabled. */
    bcmFlowtrackerDropReasonEgrAgedPkt = 150, /* Packet dropped at egress due to
                                           packet aging. */
    bcmFlowtrackerDropReasonEgrL2MtuFail = 151, /* Packet dropped at egress due to L2
                                           MTU fail. */
    bcmFlowtrackerDropReasonEgrParity = 152, /* Packet dropped at egress due to
                                           parity error. */
    bcmFlowtrackerDropReasonEgrPortEgress = 153, /* Packet dropped at egress due to
                                           egress port. */
    bcmFlowtrackerDropReasonEgrFieldProcessor = 154, /* Packet dropped at egress due to
                                           egress field processor. */
    bcmFlowtrackerDropReasonEgrEPAdditionTooLarge = 155, /* Packet dropped at egress due to EP
                                           additions are too large. */
    bcmFlowtrackerDropReasonEgrNivPrune = 156, /* Packet dropped at egress due to
                                           tunnel id not valid. */
    bcmFlowtrackerDropReasonEgrPacketFlowSelect = 157, /* Packet dropped at egress due to
                                           PKT_FLOW_SELECT policy. */
    bcmFlowtrackerDropReasonEgrPacketTooSmall = 158, /* Packet dropped at egress due to small
                                           size. */
    bcmFlowtrackerDropReasonEgrVisibilityPacket = 159, /* Visibility packet drop at egress. */
    bcmFlowtrackerDropReasonEgrInvalid1588Packet = 160, /* Packet dropped at egress as it is
                                           invalid 1588 packet. */
    bcmFlowtrackerDropReasonEgrEcnTable = 161, /* Packet dropped at egress due to ECN
                                           table. */
    bcmFlowtrackerDropReasonEgrHiGigReserved = 162, /* Packet dropped at egress due to HiGig
                                           reserved drop. */
    bcmFlowtrackerDropReasonEgrHiGig2Reserved = 163, /* Packet dropped at egress due to
                                           HiGig2 reserved drop. */
    bcmFlowtrackerDropReasonEgrBadTunnelDip = 164, /* Packet dropped at egress due to bad
                                           tunnel DIP. */
    bcmFlowtrackerDropReasonEgrEsipLinkLocal = 165, /* Packet dropped at egress due to ESIP
                                           Link local. */
    bcmFlowtrackerDropReasonEgrCfi = 166, /* CFI packet drop at egress. */
    bcmFlowtrackerDropReasonEgrFlowSelectMiss = 167, /* Packet dropped at egress due to look
                                           up miss in PKT_FLOW_SELECT and
                                           EGR_PKT_FLOW_SELECT tcams. */
    bcmFlowtrackerDropReasonEgrProtection = 168, /* Packet dropped at egress due to
                                           protection from
                                           EGR_L3_NEXT_HOP_1.DROP or
                                           EGR_TX_PROT_GROUP_TABLE.DROP_BITMAP. */
    bcmFlowtrackerDropReasonEgrFlexEditor = 169, /* Packet dropped at egress due to
                                           resource conflict or wrong
                                           configuration in flex editor. */
    bcmFlowtrackerDropReasonEgrQosCtrl = 170, /* Packet dropped at egress due to
                                           egress cosq control. */
    bcmFlowtrackerDropReasonCount = 171 /* Last Value. Not Usable. */
} bcm_flowtracker_drop_reason_t;

#define BCM_FT_DROP_REASON_STRINGS \
{ \
    "Invalid", \
    "MmuIngressPortPG", \
    "MmuIngressPortPool", \
    "MmuIngressPortGreen", \
    "MmuIngressPortYellow", \
    "MmuIngressPortRed", \
    "MmuIngressHeadroom", \
    "MmuEgressUCQueueGreen", \
    "MmuEgressUCQueueYellow", \
    "MmuEgressUCQueueRed", \
    "MmuEgressUCQueueGroupGreen", \
    "MmuEgressUCQueueGroupYellow", \
    "MmuEgressUCQueueGroupRed", \
    "MmuEgressUCPortPoolGreen", \
    "MmuEgressUCPortPoolYellow", \
    "MmuEgressUCPortPoolRed", \
    "MmuEgressUCPoolGreen", \
    "MmuEgressUCPoolYellow", \
    "MmuEgressUCPoolRed", \
    "MmuEgressWREDUCQueueGreen", \
    "MmuEgressWREDUCQueueYellow", \
    "MmuEgressWREDUCQueueRed", \
    "MmuEgressWREDUCQueueGroupGreen", \
    "MmuEgressWREDUCQueueGroupYellow", \
    "MmuEgressWREDUCQueueGroupRed", \
    "MmuEgressWREDUCPortPoolGreen", \
    "MmuEgressWREDUCPortPoolYellow", \
    "MmuEgressWREDUCPortPoolRed", \
    "MmuEgressWREDUCMin", \
    "MmuEgressWREDUCPool", \
    "MmuEgressRQEQueueGreen", \
    "MmuEgressRQEQueueYellow", \
    "MmuEgressRQEQueueRed", \
    "MmuEgressRQEPoolGreen", \
    "MmuEgressRQEPoolYellow", \
    "MmuEgressRQEPoolRed", \
    "MmuEgressMCQueueGreen", \
    "MmuEgressMCQueueYellow", \
    "MmuEgressMCQueueRed", \
    "MmuEgressMCPortPoolGreen", \
    "MmuEgressMCPortPoolYellow", \
    "MmuEgressMCPortPoolRed", \
    "MmuEgressMCPoolGreen", \
    "MmuEgressMCPoolYellow", \
    "MmuEgressMCPoolRed", \
    "MmuCFAP", \
    "MmuIngressWRED", \
    "MmuEgressUC", \
    "MmuIngress", \
    "MmuEgressMC", \
    "MmuEgressRQE", \
    "Mmu", \
    "MmuIpMtu", \
    "IngControlFrame", \
    "IngL3DosAttack", \
    "IngHigigLoopback", \
    "IngiHigigUnknownOpcode", \
    "IngHigigHdrError", \
    "IngLagFailLoopback", \
    "IngMacSAEqualsDA", \
    "IngL4DosAttack", \
    "IngNIVDot1p", \
    "IngNIVVntagPresent", \
    "IngNIVVntagNotPresent", \
    "IngNIVVntagFormat", \
    "IngUnknownSubtendingPort", \
    "IngIPinIPTunnelProcess", \
    "IngMiMTunnelProcess", \
    "IngTunnelInterfaceCheck", \
    "IngMplsInvalidControlWord", \
    "IngMplsGalNotBos", \
    "IngMplsInvalidPayload", \
    "IngMplsEntropyLabel", \
    "IngMplsLabelLookupMiss", \
    "IngMplsInvalidAction", \
    "IngTunnelTTLCheck", \
    "IngTunnelSHIMHDRError", \
    "IngObjValidation", \
    "IngInvalidVlan", \
    "IngPortNotInVlanMember", \
    "IngInvalidTpid", \
    "IngPvlanMismatch", \
    "IngMACIPBindCheck", \
    "IngTunnelAdapt4LookupMiss", \
    "IngTunnelAdapt3LookupMiss", \
    "IngTunnelAdapt2LookupMiss", \
    "IngTunnelAdapt1LookupMiss", \
    "IngFieldProcessorLookup", \
    "IngFcoeVft", \
    "IngFcoeSrcBindCheck", \
    "IngFcoeSrcFpmaCheck", \
    "IngVfiPt2Pt", \
    "IngSpanningTreeNotForwarding", \
    "IngMcastIndexError", \
    "IngCompositeError", \
    "IngBpdu", \
    "IngProtocol", \
    "IngCml", \
    "IngSrcRoute", \
    "IngL2SrcDiscard", \
    "IngL2SrcStaticMove", \
    "IngL2DstDiscard", \
    "IngL3Disable", \
    "IngSrcMacZero", \
    "IngVLANCC", \
    "IngClassBasedSM", \
    "IngTimeSyncPkt", \
    "IngIPMCProcess", \
    "IngMyStation", \
    "IngPvlanVpFilter", \
    "IngPktFlowSelectMiss", \
    "IngL3DstDiscard", \
    "IngTunnelDecapECN", \
    "IngPktFlowSelectAction", \
    "IngL3SrcDiscard", \
    "IngFcoeZoneLookupMiss", \
    "IngL3Ttl", \
    "IngL3Hdr", \
    "IngL2Hdr", \
    "IngL3SrcLookupMiss", \
    "IngL3DstLookupMiss", \
    "IngL2TtlError", \
    "IngL2RpfCheck", \
    "IngPfm", \
    "IngTagUntagDiscard", \
    "IngEcmpResolution", \
    "IngStormControl", \
    "IngFcoeZoneCheck", \
    "IngFPChangeL2FieldNoRedirectDrop", \
    "IngMulticast", \
    "IngTunnelError", \
    "IngNextHop", \
    "IngUrpfCheck", \
    "IngNat", \
    "IngProtectionData", \
    "IngStageIngress", \
    "EgrAdapt1Miss", \
    "EgrAdapt2Miss", \
    "EgrTtl", \
    "EgrFwdDomainNotFound", \
    "EgrFwdDomainNotMember", \
    "EgrStgBlock", \
    "EgrIpmcL2Self", \
    "EgrNonUCastPrune", \
    "EgrSrcVirtualPort", \
    "EgrVplagPrune", \
    "EgrSplitHorizon", \
    "EgrIpmcL3SelfIntf", \
    "EgrPurgePkt", \
    "EgrStgDisable", \
    "EgrAgedPkt", \
    "EgrL2MtuFail", \
    "EgrParity", \
    "EgrPortEgress", \
    "EgrFieldProcessor", \
    "EgrEPAdditionTooLarge", \
    "EgrNivPrune", \
    "EgrPacketFlowSelect", \
    "EgrPacketTooSmall", \
    "EgrVisibilityPacket", \
    "EgrInvalid1588Packet", \
    "EgrEcnTable", \
    "EgrHiGigReserved", \
    "EgrHiGig2Reserved", \
    "EgrBadTunnelDip", \
    "EgrEsipLinkLocal", \
    "EgrCfi", \
    "EgrFlowSelectMiss", \
    "EgrProtection", \
    "EgrFlexEditor", \
    "EgrQosCtrl"  \
}

/* Flowtracker packet drop reasons. */
typedef struct bcm_flowtracker_drop_reasons_s {
    SHR_BITDCL rbits[_SHR_BITDCLSIZE(bcmFlowtrackerDropReasonCount)]; /* Bitmap of packet drop reasons in
                                           flowtracker module. */
} bcm_flowtracker_drop_reasons_t;

/* Manipulate Drop Reason Bitmap. */
#define BCM_FLOWTRACKER_DROP_REASON_SET(_ft_drop_reasons, _reason)  SHR_BITSET(((_ft_drop_reasons).rbits), (_reason)) 
#define BCM_FLOWTRACKER_DROP_REASON_ING_MMU_SET_ALL(_ft_drop_reasons)  SHR_BITSET_RANGE(((_ft_drop_reasons).rbits), 1, bcmFlowtrackerDropReasonIngStageIngress) 
#define BCM_FLOWTRACKER_DROP_REASON_EGR_SET_ALL(_ft_drop_reasons)  SHR_BITSET_RANGE(((_ft_drop_reasons).rbits), (bcmFlowtrackerDropReasonIngStageIngress + 1), (bcmFlowtrackerDropReasonCount - (bcmFlowtrackerDropReasonIngStageIngress + 1))) 
#define BCM_FLOWTRACKER_DROP_REASON_GET(_ft_drop_reasons, _reason)  SHR_BITGET(((_ft_drop_reasons).rbits), (_reason)) 
#define BCM_FLOWTRACKER_DROP_REASON_CLEAR(_ft_drop_reasons, _reason)  SHR_BITCLR(((_ft_drop_reasons).rbits), (_reason)) 
#define BCM_FLOWTRACKER_DROP_REASON_CLEAR_ALL(_ft_drop_reasons)  SHR_BITCLR_RANGE(((_ft_drop_reasons).rbits), 0, bcmFlowtrackerDropReasonCount) 
#define BCM_FLOWTRACKER_DROP_REASON_IS_NULL(_ft_drop_reasons)  SHR_BITNULL_RANGE(((_ft_drop_reasons).rbits), 0, bcmFlowtrackerDropReasonCount) 
#define BCM_FLOWTRACKER_DROP_REASON_COUNT(_ft_drop_reasons, count)  SHR_BITCOUNT_RANGE(((_ft_drop_reasons).rbits), count, 0 , bcmFlowtrackerDropReasonCount) 

/* Flowtracker drop reason group identifier. */
typedef uint32 bcm_flowtracker_drop_reason_group_t;

/* Flowtracker drop reason group defines. */
#define BCM_FLOWTRACKER_DROP_REASON_GROUP_ID_MAX 16         /* Maximum number of drop
                                                          reason group id. */

/* Drop Reason Group Creation flags. */
#define BCM_FLOWTRACKER_DROP_REASON_GROUP_AGGREGATE_MMU (1 << 0)   /* Drop Reason Group for
                                                          Aggregate MMU
                                                          Flowtracker Type. */
#define BCM_FLOWTRACKER_DROP_REASON_GROUP_AGGREGATE_EGRESS (1 << 1)   /* Drop Reasong Group for
                                                          Aggregate Egress
                                                          Flowtracker Type. */

#ifndef BCM_HIDE_DISPATCHABLE

/* Create Flowtracker drop reasons group. */
extern int bcm_flowtracker_drop_reason_group_create(
    int unit, 
    uint32 flags, 
    bcm_flowtracker_drop_reasons_t drop_reasons, 
    bcm_flowtracker_drop_reason_group_t *id);

/* Get Flowtracker drop reasons from a group. */
extern int bcm_flowtracker_drop_reason_group_get(
    int unit, 
    bcm_flowtracker_drop_reason_group_t id, 
    bcm_flowtracker_drop_reasons_t *drop_reasons);

/* Delete Flowtracker drop reason group. */
extern int bcm_flowtracker_drop_reason_group_delete(
    int unit, 
    bcm_flowtracker_drop_reason_group_t id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Flowtracker export trigger types */
typedef enum bcm_flowtracker_export_trigger_e {
    bcmFlowtrackerExportTriggerNone = 0, /* No export trigger. */
    bcmFlowtrackerExportTriggerTimer = 1, /* Flow data will be exported to
                                           collector every
                                           bcm_flowtracker_export_trigger_info_t.interval_usecs. */
    bcmFlowtrackerExportTriggerNewLearn = 2, /* Flow data will be exported to
                                           collector everytime a new flow is
                                           learnt in the group. */
    bcmFlowtrackerExportTriggerAgeOut = 3, /* Flow data will be exported to
                                           collector everytime a flow ages out
                                           in the group. */
    bcmFlowtrackerExportTriggerDrop = 4, /* Drop reason on a flow has changed. */
    bcmFlowtrackerExportTriggerCongestion = 5, /* Queue Congestion Monitor on a flow. */
    bcmFlowtrackerExportTriggerCount = 6 /* Last Value. Not Usable. */
} bcm_flowtracker_export_trigger_t;

/* Flowtracker export trigger flags. */
#define BCM_FLOWTRACKER_EXPORT_TRIGGER_LEARN_GUARANTEE (1 << 0)   /* This flag is
                                                          applicable when
                                                          bcmFlowtrackerExportTriggerNewLearn
                                                          is set. When it is
                                                          set,
                                                          newly learned records
                                                          are always exported.
                                                          If it is not possible
                                                          to export the record,
                                                          flow is
                                                          not learned in the
                                                          hardware. */
#define BCM_FLOWTRACKER_EXPORT_TRIGGER_AGEOUT_GUARANTEE (1 << 1)   /* This flag is
                                                          applicable when
                                                          bcmFlowtrackerExportTriggerNewLearn
                                                          is set. When it is
                                                          set,
                                                          aged out records are
                                                          always exported. */

/* Flowtracker export trigger information. */
typedef struct bcm_flowtracker_export_trigger_info_s {
    SHR_BITDCL trigger_bmp[_SHR_BITDCLSIZE(bcmFlowtrackerExportTriggerCount)]; /* The bitmap of export triggers which
                                           will result in the export of the
                                           IPFIX packets. */
    uint32 interval_usecs;              /* If one of the export triggers is
                                           bcmFlowtrackerExportTriggerTimer,
                                           this provides the interval for the
                                           timer. */
    uint32 flags;                       /* Flag to export triggers. */
} bcm_flowtracker_export_trigger_info_t;

/* Manipulate the export trigger bitmap. */
#define BCM_FLOWTRACKER_TRIGGER_SET(_export_info, _trigger)  SHR_BITSET(((_export_info).trigger_bmp), (_trigger)) 
#define BCM_FLOWTRACKER_TRIGGER_GET(_export_info, _trigger)  SHR_BITGET(((_export_info).trigger_bmp), (_trigger)) 
#define BCM_FLOWTRACKER_TRIGGER_CLEAR(_export_info, _trigger)  SHR_BITCLR(((_export_info).trigger_bmp), (_trigger)) 
#define BCM_FLOWTRACKER_TRIGGER_CLEAR_ALL(_export_info)  SHR_BITCLR_RANGE(((_export_info).trigger_bmp), 0, bcmFlowtrackerExportTriggerCount) 

/* 
 * Macros for bcm_flowtracker_collector_eth_header_t.vlan_tag_structure
 * to indicate the tag structure of vlans.
 */
#define BCM_FLOWTRACKER_COLLECTOR_ETH_HDR_UNTAGGED 0x1        
#define BCM_FLOWTRACKER_COLLECTOR_ETH_HDR_SINGLE_TAGGED 0x2        
#define BCM_FLOWTRACKER_COLLECTOR_ETH_HDR_DOUBLE_TAGGED 0x3        

/* 
 * Ethernet header in the encap used to send the IPFIX packet to the
 * collector.
 */
typedef struct bcm_flowtracker_collector_eth_header_s {
    bcm_mac_t dst_mac; 
    bcm_mac_t src_mac; 
    uint8 vlan_tag_structure; 
    uint16 outer_tpid; 
    uint16 inner_tpid; 
    bcm_vlan_tag_t outer_vlan_tag; 
    bcm_vlan_tag_t inner_vlan_tag; 
} bcm_flowtracker_collector_eth_header_t;

/* 
 * IPv4 header in the encap used to send the IPFIX packet to the
 * collector.
 */
typedef struct bcm_flowtracker_collector_ipv4_header_s {
    bcm_ip_t src_ip; 
    bcm_ip_t dst_ip; 
    uint8 dscp; 
    uint8 ttl; 
} bcm_flowtracker_collector_ipv4_header_t;

/* 
 * IPv6 header in the encap used to send the IPFIX packet to the
 * collector.
 */
typedef struct bcm_flowtracker_collector_ipv6_header_s {
    bcm_ip6_t src_ip; 
    bcm_ip6_t dst_ip; 
    uint8 traffic_class; 
    uint32 flow_label; 
    uint8 hop_limit; 
} bcm_flowtracker_collector_ipv6_header_t;

/* 
 * UDP header in the encap used to send the IPFIX packet to the
 * collector.
 */
typedef struct bcm_flowtracker_collector_udp_header_s {
    bcm_l4_port_t src_port; 
    bcm_l4_port_t dst_port; 
} bcm_flowtracker_collector_udp_header_t;

/* Software ID given for a Flowtracker collector configuration. */
typedef int bcm_flowtracker_collector_t;

/* 
 * Transport types that are supported for exporting flow data to the
 * collector
 */
typedef enum bcm_flowtracker_collector_transport_type_e {
    bcmFlowtrackerCollectorTransportTypeIpfixIpv4Udp = 0, /* The encap will be of the format UDP
                                           over IPv4. */
    bcmFlowtrackerCollectorTransportTypeIpfixIpv6Udp = 1, /* The encap will be of the format UDP
                                           over IPv6. */
    bcmFlowtrackerCollectorTransportTypeRaw = 2, /* Indicates no encap. This will be
                                           useful when the raw IPFIX PDUs need
                                           to be transported to the local CPU. */
    bcmFlowtrackerCollectorTransportTypeCount = 3 /* Last Value. Not Usable. */
} bcm_flowtracker_collector_transport_type_t;

/* Flowtracker Collector types. */
typedef enum bcm_flowtracker_collector_type_e {
    bcmFlowtrackerCollectorRemote = 0,  /* The collector is a remote machine
                                           expecting flow information records
                                           using encap scheme defined through
                                           TransportTypeXxx. */
    bcmFlowtrackerCollectorLocal = 1,   /* The collector is local CPU receiving
                                           flow information records usually
                                           without any encap. TransportTypeRaw. */
    bcmFlowtrackerCollectorCount = 2    /* Last Value. Not Usable. */
} bcm_flowtracker_collector_type_t;

/* Flowtracker collector information. */
typedef struct bcm_flowtracker_collector_info_s {
    bcm_flowtracker_collector_type_t collector_type; /* Flowtracker Collector type. Remote vs
                                           Local */
    bcm_flowtracker_collector_transport_type_t transport_type; /* Transport type used for exporting
                                           flow data to the collector. This
                                           identifies the usable fields within
                                           the encap structure member defined
                                           below. */
    bcm_flowtracker_collector_eth_header_t eth; /* Ethernet encapsulation of the packet
                                           sent to collector. */
    bcm_flowtracker_collector_ipv4_header_t ipv4; /* IPv4 encapsulation of the packet sent
                                           to collector. */
    bcm_flowtracker_collector_ipv6_header_t ipv6; /* IPv6 encapsulation of the packet sent
                                           to collector. */
    bcm_flowtracker_collector_udp_header_t udp; /* UDP encapsulation of the packet sent
                                           to collector. */
    uint16 max_packet_length;           /* The maximum packet length of an
                                           export packet that can be sent to
                                           this collector. */
    bcm_flowtracker_timestamp_source_t src_ts; /* Timestamp source info. */
} bcm_flowtracker_collector_info_t;

/* Software ID given for a Flowtracker flow group configuration. */
typedef int bcm_flowtracker_group_t;

/* Flow check object. */
typedef uint32 bcm_flowtracker_check_t;

/* 
 * Flags (bcm_flowtracker_group_info_t.flags) to be used with group
 * during group create.
 */
#define BCM_FLOWTRACKER_GROUP_USER_ENTRIES_ONLY (1 << 0)   /* Group supports user
                                                          added flow entries
                                                          only. */

/* Default Group. */
#define BCM_FLOWTRACKER_GROUP_DEFAULT   (1 << 1)   /* Indicates that this is a
                                                      default group. */

/* Custom key length. */
#define BCM_FLOWTRACKER_CUSTOM_KEY_MAX_LENGTH 16         /* Maximum length of
                                                          custom key */

/* Flowtracker Group Types */
typedef enum bcm_flowtracker_group_type_e {
    bcmFlowtrackerGroupTypeNone = 0,    /* Default Flowtracker Group */
    bcmFlowtrackerGroupTypeAggregateIngress = 1, /* Aggregate Ingress Flowtracker Group
                                           type. */
    bcmFlowtrackerGroupTypeAggregateMmu = 2, /* Aggregate Mmu Flowtracker Group Type. */
    bcmFlowtrackerGroupTypeAggregateEgress = 3, /* Aggregate Egress Flowtracker Group
                                           Type. */
    bcmFlowtrackerGroupTypeCount = 4    /* Last Value. Not Usable. */
} bcm_flowtracker_group_type_t;

/* Flowtracker flow group information. */
typedef struct bcm_flowtracker_group_info_s {
    bcm_flowtracker_ipfix_observation_domain_t observation_domain_id; /* IPFIX observation domain to which
                                           this flow group belongs to. */
    int group_class;                    /* Flowtracker flow group Class id. */
    uint32 group_flags;                 /* Flowtracker flow group flags. See
                                           BCM_FLOWTRACKER_GROUP_XXX. */
    bcm_flowtracker_group_type_t group_type; /* Flowtracker flow group type. See
                                           bcm_flowtracker_group_type_t. */
    bcm_field_group_t field_group[BCM_PIPES_MAX]; /* Field group corresponding to the flow
                                           group. */
} bcm_flowtracker_group_info_t;

#define BCM_FLOWTRACKER_GROUP_TYPE_SHIFT    (24)       

#define BCM_FLOWTRACKER_GROUP_TYPE_MASK (0xFF)     

#define BCM_FLOWTRACKER_GROUP_IDX_MASK  ((1 << BCM_FLOWTRACKER_GROUP_TYPE_SHIFT) - 1) 

#define BCM_FLOWTRACKER_GROUP_ID_SET(group_id, group_type, group_index)  \
    (group_id) = ((((group_type) & BCM_FLOWTRACKER_GROUP_TYPE_MASK) <<  \
                    (BCM_FLOWTRACKER_GROUP_TYPE_SHIFT)) |               \
                    ((group_index) & BCM_FLOWTRACKER_GROUP_IDX_MASK)) 

#define BCM_FLOWTRACKER_GROUP_INDEX_GET(group_id)  \
    ((group_id) & (BCM_FLOWTRACKER_GROUP_IDX_MASK)) 

#define BCM_FLOWTRACKER_GROUP_TYPE_GET(group_id)  \
    (((group_id) >> BCM_FLOWTRACKER_GROUP_TYPE_SHIFT) & BCM_FLOWTRACKER_GROUP_TYPE_MASK) 

/* 
 * Five tuple can be outer/inner SRC IP, DST IP, L4 SRC PORT, L4 DST PORT
 * and IP PROTOCOL that constitutes a flow.
 */
typedef struct bcm_flowtracker_flow_key_s {
    bcm_ip_addr_t src_ip; 
    bcm_ip_addr_t dst_ip; 
    bcm_l4_port_t l4_src_port; 
    bcm_l4_port_t l4_dst_port; 
    uint8 ip_protocol; 
    bcm_ip_addr_t inner_src_ip; 
    bcm_ip_addr_t inner_dst_ip; 
    bcm_l4_port_t inner_l4_src_port; 
    bcm_l4_port_t inner_l4_dst_port; 
    uint8 inner_ip_protocol; 
    uint32 vxlan_network_id; 
    uint8 custom[BCM_FLOWTRACKER_CUSTOM_KEY_MAX_LENGTH]; 
    bcm_port_t in_port; 
} bcm_flowtracker_flow_key_t;

/* Flowtracker flow group flow level data. */
typedef struct bcm_flowtracker_flow_data_s {
    uint64 packet_count; 
    uint64 byte_count; 
    uint64 flow_start_timestamp_msecs; 
    uint64 observation_timestamp_msecs; 
    bcm_rx_reasons_t pkt_drop_reasons; 
    uint8 is_static; 
} bcm_flowtracker_flow_data_t;

/* Flowtracker Flow direction Types. */
typedef enum bcm_flowtracker_direction_e {
    bcmFlowtrackerFlowDirectionNone = 0, /* No Direction. */
    bcmFlowtrackerFlowDirectionForward = 1, /* The flow is tracked in forward
                                           direction only. Flow direciton is
                                           forward from server to client flow. */
    bcmFlowtrackerFlowDirectionReverse = 2, /* The flow is tracked in reverse
                                           direction only. Flow direciton is
                                           reverse from client to server flow. */
    bcmFlowtrackerFlowDirectionBidirectional = 3, /* The flow is tracked in both the
                                           forward and reverse directions. */
    bcmFlowtrackerFlowDirectionCount = 4 /* Last Value. Not Usable. */
} bcm_flowtracker_direction_t;

/* Software ID given for a Flowtracker export template configuration. */
typedef int bcm_flowtracker_export_template_t;

/* Flowtracker export element flags. */
#define BCM_FLOWTRACKER_EXPORT_ELEMENT_FLAGS_ENTERPRISE (1 << 0)   /* Indicates that the
                                                          element is an
                                                          enterprise specific
                                                          element */
#define BCM_FLOWTRACKER_EXPORT_ELEMENT_FLAGS_STICKYOR_UPDATE (1 << 1)   /* Indicates that the
                                                          element is a sticky
                                                          element */

/* The different export element types that constitute an template. */
typedef enum bcm_flowtracker_export_element_type_e {
    bcmFlowtrackerExportElementTypeSrcIPv4 = 0, 
    bcmFlowtrackerExportElementTypeDstIPv4 = 1, 
    bcmFlowtrackerExportElementTypeSrcIPv6 = 2, 
    bcmFlowtrackerExportElementTypeDstIPv6 = 3, 
    bcmFlowtrackerExportElementTypeL4SrcPort = 4, 
    bcmFlowtrackerExportElementTypeL4DstPort = 5, 
    bcmFlowtrackerExportElementTypeIPProtocol = 6, 
    bcmFlowtrackerExportElementTypePktCount = 7, 
    bcmFlowtrackerExportElementTypeByteCount = 8, 
    bcmFlowtrackerExportElementTypePktDeltaCount = 9, 
    bcmFlowtrackerExportElementTypeByteDeltaCount = 10, 
    bcmFlowtrackerExportElementTypeVRF = 11, 
    bcmFlowtrackerExportElementTypeObservationTimeMsecs = 12, 
    bcmFlowtrackerExportElementTypeFlowStartTimeMsecs = 13, 
    bcmFlowtrackerExportElementTypeFlowtrackerGroup = 14, 
    bcmFlowtrackerExportElementTypeReserved = 15, 
    bcmFlowtrackerExportElementTypeTTL = 16, 
    bcmFlowtrackerExportElementTypeIPLength = 17, 
    bcmFlowtrackerExportElementTypeTcpWindowSize = 18, 
    bcmFlowtrackerExportElementTypeDosAttack = 19, 
    bcmFlowtrackerExportElementTypeVxlanNetworkId = 20, 
    bcmFlowtrackerExportElementTypeNextHeader = 21, 
    bcmFlowtrackerExportElementTypeHopLimit = 22, 
    bcmFlowtrackerExportElementTypeInnerSrcIPv4 = 23, 
    bcmFlowtrackerExportElementTypeInnerDstIPv4 = 24, 
    bcmFlowtrackerExportElementTypeInnerSrcIPv6 = 25, 
    bcmFlowtrackerExportElementTypeInnerDstIPv6 = 26, 
    bcmFlowtrackerExportElementTypeInnerIPProtocol = 27, 
    bcmFlowtrackerExportElementTypeInnerTTL = 28, 
    bcmFlowtrackerExportElementTypeInnerNextHeader = 29, 
    bcmFlowtrackerExportElementTypeInnerHopLimit = 30, 
    bcmFlowtrackerExportElementTypeInnerL4SrcPort = 31, 
    bcmFlowtrackerExportElementTypeInnerL4DstPort = 32, 
    bcmFlowtrackerExportElementTypeExportReasons = 33, 
    bcmFlowtrackerExportElementTypeExportFlags = 34, 
    bcmFlowtrackerExportElementTypeTcpFlags = 35, 
    bcmFlowtrackerExportElementTypeOuterVlanTag = 36, 
    bcmFlowtrackerExportElementTypeIP6Length = 37, 
    bcmFlowtrackerExportElementTypeInnerIPLength = 38, 
    bcmFlowtrackerExportElementTypeInnerIP6Length = 39, 
    bcmFlowtrackerExportElementTypeTimestampNewLearn = 40, 
    bcmFlowtrackerExportElementTypeTimestampFlowStart = 41, 
    bcmFlowtrackerExportElementTypeTimestampFlowEnd = 42, 
    bcmFlowtrackerExportElementTypeTimestampCheckEvent1 = 43, 
    bcmFlowtrackerExportElementTypeTimestampCheckEvent2 = 44, 
    bcmFlowtrackerExportElementTypeInnerDosAttack = 45, 
    bcmFlowtrackerExportElementTypeTunnelClass = 46, 
    bcmFlowtrackerExportElementTypeFlowtrackerCheck = 47, 
    bcmFlowtrackerExportElementTypeCustom = 48, 
    bcmFlowtrackerExportElementTypeFlowViewID = 49, 
    bcmFlowtrackerExportElementTypeFlowViewSampleCount = 50, 
    bcmFlowtrackerExportElementTypeFlowCount = 51, 
    bcmFlowtrackerExportElementTypeDropPktCount = 52, 
    bcmFlowtrackerExportElementTypeDropByteCount = 53, 
    bcmFlowtrackerExportElementTypeEgrPort = 54, 
    bcmFlowtrackerExportElementTypeQueueID = 55, 
    bcmFlowtrackerExportElementTypeQueueBufferUsage = 56, 
    bcmFlowtrackerExportElementTypeChipDelay = 57, 
    bcmFlowtrackerExportElementTypeE2EDelay = 58, 
    bcmFlowtrackerExportElementTypeIPATDelay = 59, 
    bcmFlowtrackerExportElementTypeIPDTDelay = 60, 
    bcmFlowtrackerExportElementTypeIngPortPGMinCongestionLevel = 61, 
    bcmFlowtrackerExportElementTypeIngPortPGSharedCongestionLevel = 62, 
    bcmFlowtrackerExportElementTypeIngPortPoolMinCongestionLevel = 63, 
    bcmFlowtrackerExportElementTypeIngPortPoolSharedCongestionLevel = 64, 
    bcmFlowtrackerExportElementTypeIngPoolSharedCongestionLevel = 65, 
    bcmFlowtrackerExportElementTypeIngPortPGHeadroomCongestionLevel = 66, 
    bcmFlowtrackerExportElementTypeIngPortPoolHeadroomCongestionLevel = 67, 
    bcmFlowtrackerExportElementTypeIngGlobalHeadroomCongestionLevel = 68, 
    bcmFlowtrackerExportElementTypeEgrPoolSharedCongestionLevel = 69, 
    bcmFlowtrackerExportElementTypeEgrQueueMinCongestionLevel = 70, 
    bcmFlowtrackerExportElementTypeEgrQueueSharedCongestionLevel = 71, 
    bcmFlowtrackerExportElementTypeEgrQueueGroupMinCongestionLevel = 72, 
    bcmFlowtrackerExportElementTypeEgrQueueGroupSharedCongestionLevel = 73, 
    bcmFlowtrackerExportElementTypeEgrPortPoolSharedCongestionLevel = 74, 
    bcmFlowtrackerExportElementTypeEgrRQEPoolSharedCongestionLevel = 75, 
    bcmFlowtrackerExportElementTypeCFAPCongestionLevel = 76, 
    bcmFlowtrackerExportElementTypeMMUCongestionLevel = 77, 
    bcmFlowtrackerExportElementTypeMMUQueueId = 78, 
    bcmFlowtrackerExportElementTypeECMPGroupIdLevel1 = 79, 
    bcmFlowtrackerExportElementTypeECMPGroupIdLevel2 = 80, 
    bcmFlowtrackerExportElementTypeTrunkMemberId = 81, 
    bcmFlowtrackerExportElementTypeTrunkGroupId = 82, 
    bcmFlowtrackerExportElementTypeIngPort = 83, 
    bcmFlowtrackerExportElementTypeIngDropReasonGroupIdVector = 84, 
    bcmFlowtrackerExportElementTypeNextHopB = 85, 
    bcmFlowtrackerExportElementTypeNextHopA = 86, 
    bcmFlowtrackerExportElementTypeIPPayloadLength = 87, 
    bcmFlowtrackerExportElementTypeQosAttr = 88, 
    bcmFlowtrackerExportElementTypeMMUCos = 89, 
    bcmFlowtrackerExportElementTypeEgrDropReasonGroupIdVector = 90, 
    bcmFlowtrackerExportElementTypeTimestampCheckEvent3 = 91, 
    bcmFlowtrackerExportElementTypeTimestampCheckEvent4 = 92, 
    bcmFlowtrackerExportElementTypeQueueRxPktCount = 93, 
    bcmFlowtrackerExportElementTypeFlowIdentifier = 94, 
    bcmFlowtrackerExportElementTypeMMUQueueGroupId = 95, 
    bcmFlowtrackerExportElementTypeMMUPacketTos = 96, 
    bcmFlowtrackerExportElementTypeEgrPacketTos = 97, 
    bcmFlowtrackerExportElementTypeQueueRxByteCount = 98, 
    bcmFlowtrackerExportElementTypeIPVersion = 99, 
    bcmFlowtrackerExportElementTypeBufferUsage = 100, 
    bcmFlowtrackerExportElementTypeIngPacketTos = 101, 
    bcmFlowtrackerExportElementTypeL2ForwardingField = 102, 
    bcmFlowtrackerExportElementTypeInnerTcpFlags = 103, 
    bcmFlowtrackerExportElementTypeGbpSrcId = 104, 
    bcmFlowtrackerExportElementTypeTimestampIngress = 105, 
    bcmFlowtrackerExportElementTypeTimestampEgress = 106, 
    bcmFlowtrackerExportElementTypeEgrClassId = 107, 
    bcmFlowtrackerExportElementTypeTcpSeqNumber = 108, 
    bcmFlowtrackerExportElementTypeCount = 109 
} bcm_flowtracker_export_element_type_t;

/* The different tracking parameters that can be tracked in flowtracker. */
typedef enum bcm_flowtracker_tracking_param_type_e {
    bcmFlowtrackerTrackingParamTypeNone = 0, 
    bcmFlowtrackerTrackingParamTypeSrcIPv4 = 1, 
    bcmFlowtrackerTrackingParamTypeDstIPv4 = 2, 
    bcmFlowtrackerTrackingParamTypeSrcIPv6 = 3, 
    bcmFlowtrackerTrackingParamTypeDstIPv6 = 4, 
    bcmFlowtrackerTrackingParamTypeL4SrcPort = 5, 
    bcmFlowtrackerTrackingParamTypeL4DstPort = 6, 
    bcmFlowtrackerTrackingParamTypeIPProtocol = 7, 
    bcmFlowtrackerTrackingParamTypePktCount = 8, 
    bcmFlowtrackerTrackingParamTypeByteCount = 9, 
    bcmFlowtrackerTrackingParamTypeVRF = 10, 
    bcmFlowtrackerTrackingParamTypeTTL = 11, 
    bcmFlowtrackerTrackingParamTypeIPLength = 12, 
    bcmFlowtrackerTrackingParamTypeTcpWindowSize = 13, 
    bcmFlowtrackerTrackingParamTypeDosAttack = 14, 
    bcmFlowtrackerTrackingParamTypeVxlanNetworkId = 15, 
    bcmFlowtrackerTrackingParamTypeNextHeader = 16, 
    bcmFlowtrackerTrackingParamTypeHopLimit = 17, 
    bcmFlowtrackerTrackingParamTypeInnerSrcIPv4 = 18, 
    bcmFlowtrackerTrackingParamTypeInnerDstIPv4 = 19, 
    bcmFlowtrackerTrackingParamTypeInnerSrcIPv6 = 20, 
    bcmFlowtrackerTrackingParamTypeInnerDstIPv6 = 21, 
    bcmFlowtrackerTrackingParamTypeInnerIPProtocol = 22, 
    bcmFlowtrackerTrackingParamTypeInnerTTL = 23, 
    bcmFlowtrackerTrackingParamTypeInnerNextHeader = 24, 
    bcmFlowtrackerTrackingParamTypeInnerHopLimit = 25, 
    bcmFlowtrackerTrackingParamTypeInnerL4SrcPort = 26, 
    bcmFlowtrackerTrackingParamTypeInnerL4DstPort = 27, 
    bcmFlowtrackerTrackingParamTypeTcpFlags = 28, 
    bcmFlowtrackerTrackingParamTypeOuterVlanTag = 29, 
    bcmFlowtrackerTrackingParamTypeIP6Length = 30, 
    bcmFlowtrackerTrackingParamTypeInnerIPLength = 31, 
    bcmFlowtrackerTrackingParamTypeInnerIP6Length = 32, 
    bcmFlowtrackerTrackingParamTypeTimestampNewLearn = 33, 
    bcmFlowtrackerTrackingParamTypeTimestampFlowStart = 34, 
    bcmFlowtrackerTrackingParamTypeTimestampFlowEnd = 35, 
    bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1 = 36, 
    bcmFlowtrackerTrackingParamTypeTimestampCheckEvent2 = 37, 
    bcmFlowtrackerTrackingParamTypeInnerDosAttack = 38, 
    bcmFlowtrackerTrackingParamTypeTunnelClass = 39, 
    bcmFlowtrackerTrackingParamTypeFlowtrackerCheck = 40, 
    bcmFlowtrackerTrackingParamTypeCustom = 41, 
    bcmFlowtrackerTrackingParamTypeIngPort = 42, 
    bcmFlowtrackerTrackingParamTypeChipDelay = 43, 
    bcmFlowtrackerTrackingParamTypeE2EDelay = 44, 
    bcmFlowtrackerTrackingParamTypeIPATDelay = 45, 
    bcmFlowtrackerTrackingParamTypeIPDTDelay = 46, 
    bcmFlowtrackerTrackingParamTypeIngPortPGMinCongestionLevel = 47, 
    bcmFlowtrackerTrackingParamTypeIngPortPGSharedCongestionLevel = 48, 
    bcmFlowtrackerTrackingParamTypeIngPortPoolMinCongestionLevel = 49, 
    bcmFlowtrackerTrackingParamTypeIngPortPoolSharedCongestionLevel = 50, 
    bcmFlowtrackerTrackingParamTypeIngPoolSharedCongestionLevel = 51, 
    bcmFlowtrackerTrackingParamTypeIngPortPGHeadroomCongestionLevel = 52, 
    bcmFlowtrackerTrackingParamTypeIngPortPoolHeadroomCongestionLevel = 53, 
    bcmFlowtrackerTrackingParamTypeIngGlobalHeadroomCongestionLevel = 54, 
    bcmFlowtrackerTrackingParamTypeEgrPoolSharedCongestionLevel = 55, 
    bcmFlowtrackerTrackingParamTypeEgrQueueMinCongestionLevel = 56, 
    bcmFlowtrackerTrackingParamTypeEgrQueueSharedCongestionLevel = 57, 
    bcmFlowtrackerTrackingParamTypeEgrQueueGroupMinCongestionLevel = 58, 
    bcmFlowtrackerTrackingParamTypeEgrQueueGroupSharedCongestionLevel = 59, 
    bcmFlowtrackerTrackingParamTypeEgrPortPoolSharedCongestionLevel = 60, 
    bcmFlowtrackerTrackingParamTypeEgrRQEPoolSharedCongestionLevel = 61, 
    bcmFlowtrackerTrackingParamTypeCFAPCongestionLevel = 62, 
    bcmFlowtrackerTrackingParamTypeMMUCongestionLevel = 63, 
    bcmFlowtrackerTrackingParamTypeMMUQueueId = 64, 
    bcmFlowtrackerTrackingParamTypeECMPGroupIdLevel1 = 65, 
    bcmFlowtrackerTrackingParamTypeECMPGroupIdLevel2 = 66, 
    bcmFlowtrackerTrackingParamTypeTrunkMemberId = 67, 
    bcmFlowtrackerTrackingParamTypeTrunkGroupId = 68, 
    bcmFlowtrackerTrackingParamTypeIngDropReasonGroupIdVector = 69, 
    bcmFlowtrackerTrackingParamTypeNextHopB = 70, 
    bcmFlowtrackerTrackingParamTypeNextHopA = 71, 
    bcmFlowtrackerTrackingParamTypeIPPayloadLength = 72, 
    bcmFlowtrackerTrackingParamTypeQosAttr = 73, 
    bcmFlowtrackerTrackingParamTypeEgrPort = 74, 
    bcmFlowtrackerTrackingParamTypeMMUCos = 75, 
    bcmFlowtrackerTrackingParamTypeEgrDropReasonGroupIdVector = 76, 
    bcmFlowtrackerTrackingParamTypeTimestampCheckEvent3 = 77, 
    bcmFlowtrackerTrackingParamTypeTimestampCheckEvent4 = 78, 
    bcmFlowtrackerTrackingParamTypeIngressDelay = 79, 
    bcmFlowtrackerTrackingParamTypeEgressDelay = 80, 
    bcmFlowtrackerTrackingParamTypeMMUQueueGroupId = 81, 
    bcmFlowtrackerTrackingParamTypeMMUPacketTos = 82, 
    bcmFlowtrackerTrackingParamTypeEgrPacketTos = 83, 
    bcmFlowtrackerTrackingParamTypeIngPacketTos = 84, 
    bcmFlowtrackerTrackingParamTypeL2ForwardingField = 85, 
    bcmFlowtrackerTrackingParamTypeInnerTcpFlags = 86, 
    bcmFlowtrackerTrackingParamTypeGbpSrcId = 87, 
    bcmFlowtrackerTrackingParamTypeTimestampIngress = 88, 
    bcmFlowtrackerTrackingParamTypeTimestampEgress = 89, 
    bcmFlowtrackerTrackingParamTypeEgrClassId = 90, 
    bcmFlowtrackerTrackingParamTypeTcpSeqNumber = 91, 
    bcmFlowtrackerTrackingParamTypeCount = 92 
} bcm_flowtracker_tracking_param_type_t;

#define BCM_FT_TRACKING_PARAM_STRINGS \
{ \
    "None", \
    "SrcIPv4", \
    "DstIPv4", \
    "SrcIPv6", \
    "DstIPv6", \
    "L4SrcPort", \
    "L4DstPort", \
    "IPProtocol", \
    "PktCount", \
    "ByteCount", \
    "VRF", \
    "TTL", \
    "IPLength", \
    "TcpWindowSize", \
    "DosAttack", \
    "VxlanNetworkId", \
    "NextHeader", \
    "HopLimit", \
    "InnerSrcIPv4", \
    "InnerDstIPv4", \
    "InnerSrcIPv6", \
    "InnerDstIPv6", \
    "InnerIPProtocol", \
    "InnerTTL", \
    "InnerNextHeader", \
    "InnerHopLimit", \
    "InnerL4SrcPort", \
    "InnerL4DstPort", \
    "TcpFlags", \
    "OuterVlanTag", \
    "IP6Length", \
    "InnerIPLength", \
    "InnerIP6Length", \
    "TimestampNewLearn", \
    "TimestampFlowStart", \
    "TimestampFlowEnd", \
    "TimestampCheckEvent1", \
    "TimestampCheckEvent2", \
    "InnerDosAttack", \
    "TunnelClass", \
    "FlowtrackerCheck", \
    "Custom", \
    "IngPort", \
    "ChipDelay", \
    "E2EDelay", \
    "IPATDelay", \
    "IPDTDelay", \
    "IngPortPGMinCongestionLevel", \
    "IngPortPGSharedCongestionLevel", \
    "IngPortPoolMinCongestionLevel", \
    "IngPortPoolSharedCongestionLevel", \
    "IngPoolSharedCongestionLevel", \
    "IngPortPGHeadroomCongestionLevel", \
    "IngPortPoolHeadroomCongestionLevel", \
    "IngGlobalHeadroomCongestionLevel", \
    "EgrPoolSharedCongestionLevel", \
    "EgrQueueMinCongestionLevel", \
    "EgrQueueSharedCongestionLevel", \
    "EgrQueueGroupMinCongestionLevel", \
    "EgrQueueGroupSharedCongestionLevel", \
    "EgrPortPoolSharedCongestionLevel", \
    "EgrRQEPoolSharedCongestionLevel", \
    "CFAPCongestionLevel", \
    "MMUCongestionLevel", \
    "MMUQueueId", \
    "ECMPGroupIdLevel1", \
    "ECMPGroupIdLevel2", \
    "TrunkMemberId", \
    "TrunkGroupId", \
    "IngDropReasonGroupIdVector", \
    "NextHopB", \
    "NextHopA", \
    "IPPayloadLength", \
    "QosAttr", \
    "EgrPort", \
    "MMUCos", \
    "EgrDropReasonGroupIdVector", \
    "TimestampCheckEvent3", \
    "TimestampCheckEvent4", \
    "IngressDelay", \
    "EgressDelay", \
    "MMUQueueGroupId", \
    "MMUPacketTos", \
    "EgrPacketTos", \
    "IngPacketTos", \
    "L2ForwardingField", \
    "InnerTcpFlags", \
    "GbpSrcId", \
    "TimestampIngress", \
    "TimestampEgress", \
    "EgrClassId", \
    "TcpSeqNumber", \
    "Count"  \
}

/* Flowtracker export template elements information. */
typedef struct bcm_flowtracker_export_element_info_s {
    uint32 flags;                       /* See
                                           BCM_FLOWTRACKER_EXPORT_ELEMENT_FLAGS_XXX. */
    bcm_flowtracker_export_element_type_t element; /* Type of export element. */
    uint32 data_size;                   /* Size of information element in the
                                           export record in bytes. If the
                                           data_size is given as 0, then the
                                           default RFC size is used. */
    uint16 info_elem_id;                /* Information element to be used, when
                                           the template set is exported. */
    bcm_flowtracker_check_t check_id;   /* Flowtracker Check Id for exporting
                                           check data. */
    bcm_flowtracker_direction_t direction; /* Direction of exporting data element. */
    uint16 custom_id;                   /* Custom Identifier for Flowtracker
                                           Export Element type custom. */
} bcm_flowtracker_export_element_info_t;

/* Flowtracker template set transmit configuration. */
typedef struct bcm_flowtracker_template_transmit_config_s {
    int retransmit_interval_secs;   /* Interval in seconds at which the template
                                       set has to be transmitted. */
    int initial_burst;              /* Number of times the template set needs to
                                       be sent before settling to a periodic
                                       export. */
} bcm_flowtracker_template_transmit_config_t;

/* 
 * Flowtracker information for checker to compute time delay between
 * various packet events across pipeline.
 */
typedef struct bcm_flowtracker_check_delay_info_s {
    bcm_flowtracker_timestamp_source_t src_ts; /* Timestamp source info. */
    bcm_flowtracker_timestamp_source_t dst_ts; /* Timestamp destination info. */
    uint8 gran;                         /* Delay granularity which defines
                                           number of shifts for delay operands. */
    int offset;                         /* Offset added to delay result in
                                           nanoseconds. */
} bcm_flowtracker_check_delay_info_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Initialize flowtracker subsystem. */
extern int bcm_flowtracker_init(
    int unit);

/* Detach flowtracker subsystem. */
extern int bcm_flowtracker_detach(
    int unit);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Options that can be passed via options parameter during export
 * template creation.
 */
#define BCM_FLOWTRACKER_EXPORT_TEMPLATE_WITH_ID (1 << 0)   /* Create an export
                                                          template with ID. */

#ifndef BCM_HIDE_DISPATCHABLE

/* Validate the template and return the list supported by the device. */
extern int bcm_flowtracker_export_template_validate(
    int unit, 
    bcm_flowtracker_group_t flow_group_id, 
    int max_in_export_elements, 
    bcm_flowtracker_export_element_info_t *in_export_elements, 
    int max_out_export_elements, 
    bcm_flowtracker_export_element_info_t *out_export_elements, 
    int *actual_out_export_elements);

/* Create a flowtracker export template. */
extern int bcm_flowtracker_export_template_create(
    int unit, 
    uint32 options, 
    bcm_flowtracker_export_template_t *id, 
    uint16 set_id, 
    int num_export_elements, 
    bcm_flowtracker_export_element_info_t *list_of_export_elements);

/* Get a flowtracker export template. */
extern int bcm_flowtracker_export_template_get(
    int unit, 
    bcm_flowtracker_export_template_t id, 
    uint16 *set_id, 
    int max_size, 
    bcm_flowtracker_export_element_info_t *list_of_export_elements, 
    int *list_size);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize flowtracker template transmit configuration. */
extern void bcm_flowtracker_template_transmit_config_t_init(
    bcm_flowtracker_template_transmit_config_t *config);

#ifndef BCM_HIDE_DISPATCHABLE

/* Destroy a flowtracker export template. */
extern int bcm_flowtracker_export_template_destroy(
    int unit, 
    bcm_flowtracker_export_template_t id);

/* Set the template transmit configuration. */
extern int bcm_flowtracker_template_transmit_config_set(
    int unit, 
    bcm_flowtracker_export_template_t template_id, 
    bcm_collector_t collector_id, 
    bcm_flowtracker_template_transmit_config_t *config);

/* Set the template transmit configuration. */
extern int bcm_flowtracker_template_transmit_config_get(
    int unit, 
    bcm_flowtracker_export_template_t template_id, 
    bcm_collector_t collector_id, 
    bcm_flowtracker_template_transmit_config_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a collector information structure. */
extern void bcm_flowtracker_collector_info_t_init(
    bcm_flowtracker_collector_info_t *collector_info);

/* 
 * Options that can be passed via options parameter during collector
 * creation.
 */
#define BCM_FLOWTRACKER_COLLECTOR_WITH_ID   (1 << 0)   /* Create a collector
                                                          with ID. */
#define BCM_FLOWTRACKER_COLLECTOR_REPLACE   (1 << 1)   /* Replace a collector
                                                          with ID. */

#ifndef BCM_HIDE_DISPATCHABLE

/* Create a flowtracker collector. */
extern int bcm_flowtracker_collector_create(
    int unit, 
    uint32 options, 
    bcm_flowtracker_collector_t *collector_id, 
    bcm_flowtracker_collector_info_t *collector_info);

/* Get a flowtracker collector. */
extern int bcm_flowtracker_collector_get(
    int unit, 
    bcm_flowtracker_collector_t id, 
    bcm_flowtracker_collector_info_t *collector_info);

/* Get all the configured flowtracker collectors. */
extern int bcm_flowtracker_collector_get_all(
    int unit, 
    int max_size, 
    bcm_flowtracker_collector_t *collector_list, 
    int *list_size);

/* Destroy a flowtracker collector. */
extern int bcm_flowtracker_collector_destroy(
    int unit, 
    bcm_flowtracker_collector_t id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a flow group information structure. */
extern void bcm_flowtracker_group_info_t_init(
    bcm_flowtracker_group_info_t *flow_group_info);

/* 
 * Options that can be passed via options parameter during flow group
 * creation.
 */
#define BCM_FLOWTRACKER_GROUP_WITH_ID   (1 << 0)   /* Create a flow group with
                                                      ID. */
#define BCM_FLOWTRACKER_GROUP_REPLACE   (1 << 1)   /* Replace a flow group with
                                                      ID. */

#ifndef BCM_HIDE_DISPATCHABLE

/* Create a flowtracker flow group. */
extern int bcm_flowtracker_group_create(
    int unit, 
    uint32 options, 
    bcm_flowtracker_group_t *flow_group_id, 
    bcm_flowtracker_group_info_t *flow_group_info);

/* Get a flowtracker flow group. */
extern int bcm_flowtracker_group_get(
    int unit, 
    bcm_flowtracker_group_t id, 
    bcm_flowtracker_group_info_t *flow_group_info);

/* Get all the configured flowtracker flow group IDs. */
extern int bcm_flowtracker_group_get_all(
    int unit, 
    int max_size, 
    bcm_flowtracker_group_t *flow_group_list, 
    int *list_size);

/* Set flow limit on the flow group with ID. */
extern int bcm_flowtracker_group_flow_limit_set(
    int unit, 
    bcm_flowtracker_group_t id, 
    uint32 flow_limit);

/* Get flow limit set on the flow group with ID. */
extern int bcm_flowtracker_group_flow_limit_get(
    int unit, 
    bcm_flowtracker_group_t id, 
    uint32 *flow_limit);

/* Set stat mode id to provided flow tracker group. */
extern int bcm_flowtracker_group_stat_modeid_set(
    int unit, 
    bcm_flowtracker_group_t id, 
    uint32 stat_modeid);

/* Get stat modeid attached to provided flow tracker group. */
extern int bcm_flowtracker_group_stat_modeid_get(
    int unit, 
    bcm_flowtracker_group_t id, 
    uint32 *stat_modeid);

/* Set aging timer interval in ms on the flow group with ID */
extern int bcm_flowtracker_group_age_timer_set(
    int unit, 
    bcm_flowtracker_group_t id, 
    uint32 aging_interval_ms);

/* Get aging timer interval in ms set on the flow group with ID */
extern int bcm_flowtracker_group_age_timer_get(
    int unit, 
    bcm_flowtracker_group_t id, 
    uint32 *aging_interval_ms);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a export trigger information structure. */
extern void bcm_flowtracker_export_trigger_info_t_init(
    bcm_flowtracker_export_trigger_info_t *info);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set flow group's export triggers. */
extern int bcm_flowtracker_group_export_trigger_set(
    int unit, 
    bcm_flowtracker_group_t id, 
    bcm_flowtracker_export_trigger_info_t *export_trigger_info);

/* Get flow group's export triggers. */
extern int bcm_flowtracker_group_export_trigger_get(
    int unit, 
    bcm_flowtracker_group_t id, 
    bcm_flowtracker_export_trigger_info_t *export_trigger_info);

/* Get number of flows learnt on this flow group with ID. */
extern int bcm_flowtracker_group_flow_count_get(
    int unit, 
    bcm_flowtracker_group_t id, 
    uint32 *flow_count);

/* Destroy the flow group with ID. */
extern int bcm_flowtracker_group_destroy(
    int unit, 
    bcm_flowtracker_group_t id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Flags that can be passed via flags parameter during flow group clear
 * operation.
 */
#define BCM_FLOWTRACKER_GROUP_CLEAR_ALL     (1 << 0)   /* Clear entire flow
                                                          group flow entries(key
                                                          and data). */
#define BCM_FLOWTRACKER_GROUP_CLEAR_FLOW_DATA_ONLY (1 << 1)   /* Clear only data of the
                                                          flow group flow
                                                          entries. */

#ifndef BCM_HIDE_DISPATCHABLE

/* Clear information associated with a flow group */
extern int bcm_flowtracker_group_clear(
    int unit, 
    bcm_flowtracker_group_t id, 
    uint32 flags);

/* Associate flow group to a collector with an export template. */
extern int bcm_flowtracker_group_collector_add(
    int unit, 
    bcm_flowtracker_group_t flow_group_id, 
    bcm_flowtracker_collector_t collector_id, 
    bcm_flowtracker_export_template_t template_id);

/* Dis Associate flow group from a collector with an export template. */
extern int bcm_flowtracker_group_collector_delete(
    int unit, 
    bcm_flowtracker_group_t flow_group_id, 
    bcm_flowtracker_collector_t collector_id, 
    bcm_flowtracker_export_template_t template_id);

/* 
 * Get the list of associated collectors and export templates for the
 * flow group.
 */
extern int bcm_flowtracker_group_collector_get_all(
    int unit, 
    bcm_flowtracker_group_t flow_group_id, 
    int max_list_size, 
    bcm_flowtracker_collector_t *list_of_collectors, 
    bcm_flowtracker_export_template_t *list_of_templates, 
    int *list_size);

/* 
 * Associate flow group to a collector with an export template and
 * profile.
 */
extern int bcm_flowtracker_group_collector_attach(
    int unit, 
    bcm_flowtracker_group_t flow_group_id, 
    bcm_collector_t collector_id, 
    int export_profile_id, 
    bcm_flowtracker_export_template_t template_id);

/* Dis Associate flow group from a collector with an export template. */
extern int bcm_flowtracker_group_collector_detach(
    int unit, 
    bcm_flowtracker_group_t flow_group_id, 
    bcm_collector_t collector_id, 
    int export_profile_id, 
    bcm_flowtracker_export_template_t template_id);

/* 
 * Get the list of associated collectors and export templates for the
 * flow group.
 */
extern int bcm_flowtracker_group_collector_attach_get_all(
    int unit, 
    bcm_flowtracker_group_t flow_group_id, 
    int max_list_size, 
    bcm_collector_t *list_of_collectors, 
    bcm_flowtracker_export_template_t *list_of_templates, 
    int *export_profile_id_list, 
    int *list_size);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize flow key */
extern void bcm_flowtracker_flow_key_t_init(
    bcm_flowtracker_flow_key_t *flow_key);

#ifndef BCM_HIDE_DISPATCHABLE

/* Given a flow key, get the associated data */
extern int bcm_flowtracker_group_data_get(
    int unit, 
    bcm_flowtracker_group_t flow_group_id, 
    bcm_flowtracker_flow_key_t *flow_key, 
    bcm_flowtracker_flow_data_t *flow_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* State Transition flags. */
#define BCM_FLOWTRACKER_STATE_TRANSITION_TCP_CLOSED_SYN (1 << 0)   /* State Transition for
                                                          TCP from CLOSED on TCP
                                                          Flag SYN. */
#define BCM_FLOWTRACKER_STATE_TRANSITION_TCP_CLOSED_SYNACK (1 << 1)   /* State Transition for
                                                          TCP from CLOSED on TCP
                                                          Flag SYN+ACK. */
#define BCM_FLOWTRACKER_STATE_TRANSITION_TCP_SYNWAIT_ACK (1 << 2)   /* State Transition for
                                                          TCP from SYNWAIT on
                                                          TCP Flag ACK. */
#define BCM_FLOWTRACKER_STATE_TRANSITION_TCP_SYNWAIT_FIN (1 << 3)   /* State Transition for
                                                          TCP from SYNWAIT on
                                                          TCP Flag FIN. */
#define BCM_FLOWTRACKER_STATE_TRANSITION_TCP_SYNWAIT_RST (1 << 4)   /* State Transition for
                                                          TCP from SYNWAIT on
                                                          TCP Flag RST. */
#define BCM_FLOWTRACKER_STATE_TRANSITION_TCP_ESTABLISHED_FIN (1 << 5)   /* State Transition for
                                                          TCP from ESTABLISHED
                                                          on TCP Flag FIN. */
#define BCM_FLOWTRACKER_STATE_TRANSITION_TCP_ESTABLISHED_RST (1 << 6)   /* State Transition for
                                                          TCP from ESTABLISHED
                                                          on TCP Flag RST. */

/* Flow check operation types */
typedef enum bcm_flowtracker_check_operation_e {
    bcmFlowtrackerCheckOpNone = 0,      /* No operation for check. */
    bcmFlowtrackerCheckOpEqual = 1,     /* The packet attribute value should be
                                           equal to minimum value. */
    bcmFlowtrackerCheckOpNotEqual = 2,  /* The packet attribute value should not
                                           be equal to minimum value. */
    bcmFlowtrackerCheckOpSmaller = 3,   /* The packet attribute value should be
                                           smaller than the maximum value. */
    bcmFlowtrackerCheckOpSmallerEqual = 4, /* The packet attribute value should be
                                           smaller or equal to the maximum
                                           value. */
    bcmFlowtrackerCheckOpGreater = 5,   /* The packet attribute value should be
                                           greater than minimum value. */
    bcmFlowtrackerCheckOpGreaterEqual = 6, /* The packet attribute value should be
                                           greater than or equal to minimum
                                           value. */
    bcmFlowtrackerCheckOpPass = 7,      /* The check will always pass. */
    bcmFlowtrackerCheckOpFail = 8,      /* The Check will always fail. */
    bcmFlowtrackerCheckOpInRange = 9,   /* The packet attribute value should be
                                           in Range of minimum and maximum
                                           values. Both values are inclusive. */
    bcmFlowtrackerCheckOpOutofRange = 10, /* The packet attribute value should be
                                           out of Range of minimum and maximum
                                           values. Both values are inclusive. */
    bcmFlowtrackerCheckOpMask = 11,     /* Bitwise AND of packet attribute with
                                           maximum value as mask and then do
                                           bitwise OR of resultant value. */
    bcmFlowtrackerCheckOpCount = 12     /* Last Value. Total number of all
                                           operations. */
} bcm_flowtracker_check_operation_t;

#define BCM_FT_CHECK_OPERATION_STRINGS \
{ \
    "None", \
    "Equal", \
    "NotEqual", \
    "Smaller", \
    "SmallerEqual", \
    "Greater", \
    "GreaterEqual", \
    "Pass", \
    "Fail", \
    "InRange", \
    "OutofRange", \
    "Mask", \
    "Count"  \
}

/* Flags for flowtracker check creation. */
#define BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT1 (1 << 0)   /* Enable trigger for
                                                          check's first event. */
#define BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT2 (1 << 1)   /* Enable trigger for
                                                          check's second event. */
#define BCM_FLOWTRACKER_CHECK_DATA_CLEAR_ON_EXPORT (1 << 2)   /* Clear data associated
                                                          with this check during
                                                          export. */
#define BCM_FLOWTRACKER_CHECK_EXPORT_MODE_THRESHOLD (1 << 3)   /* if set, compare if
                                                          check's updated data
                                                          is greater-equal to
                                                          threshold and old data
                                                          is less than threshold
                                                          for export check else
                                                          compare ALU new data
                                                          to old session data
                                                          and perform export
                                                          check. */
#define BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT3 (1 << 4)   /* Enable trigger for
                                                          check's third event. */
#define BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT4 (1 << 5)   /* Enable trigger for
                                                          check's fourth event. */
#define BCM_FLOWTRACKER_CHECK_DATA_CLEAR_ON_PERIODIC_EXPORT (1 << 6)   /* Clear session data
                                                          after periodic export. */
#define BCM_FLOWTRACKER_CHECK_UNICAST_FLOW_ONLY (1 << 7)   /* Check operates on
                                                          Unicast flows only */
#define BCM_FLOWTRACKER_CHECK_MULTICAST_FLOW_ONLY (1 << 8)   /* Check operates on
                                                          multicast flows only */
#define BCM_FLOWTRACKER_CHECK_MODE_32BIT    (1 << 9)   /* Check operating mode
                                                          is 32 bits */
#define BCM_FLOWTRACKER_CHECK_MODE_16BIT    (1 << 10)  /* Check operating mode
                                                          is 16 bits */

/* Flowtracker check information. */
typedef struct bcm_flowtracker_check_info_s {
    uint32 flags;                       /* Configuration flags for Check
                                           Creation. */
    bcm_flowtracker_tracking_param_type_t param; /* The attribute of flow on which the
                                           check is performed. */
    uint32 min_value;                   /* Element value to do greater than or
                                           equal checks. Minimum value to do
                                           range checks. */
    uint32 max_value;                   /* Element value to do smaller than or
                                           mask checks. Maximum value to do
                                           range checks. */
    uint32 mask_value;                  /* Mask value to be applied on flow
                                           attribute. */
    uint8 shift_value;                  /* Right shift value to be applied on
                                           flow attribute. */
    bcm_flowtracker_check_operation_t operation; /* Operation to be performed for this
                                           check. */
    bcm_flowtracker_check_t primary_check_id; /* primary check id to associate second
                                           check for aggregated checks on same
                                           flow. */
    uint32 state_transition_flags;      /* State Transition flags */
    uint16 num_drop_reason_group_id;    /* Number of drop reason group id. */
    bcm_flowtracker_drop_reason_group_t drop_reason_group_id[BCM_FLOWTRACKER_DROP_REASON_GROUP_ID_MAX]; /* Flowtracker drop reason group ids.
                                           Applicable only if tracking param is
                                           IngDropReasonGroupIdVector or
                                           EgrDropReasonGroupIdVector. */
    uint16 custom_id;                   /* Custom Identifier for Flowtracker
                                           Tracking parameter type custom. */
} bcm_flowtracker_check_info_t;

/* Flow check Action types */
typedef enum bcm_flowtracker_check_action_e {
    bcmFlowtrackerCheckActionNone = 0,  /* No action. */
    bcmFlowtrackerCheckActionCounterIncr = 1, /* Increment the Counter stored in
                                           tracking data table by 1. */
    bcmFlowtrackerCheckActionCounterIncrByValue = 2, /* Increment the attribute value stored
                                           in tracking data with the packet's
                                           attribute value. */
    bcmFlowtrackerCheckActionUpdateLowerValue = 3, /* Compare packet's attribute value with
                                           tracking data value and if lower,
                                           replace tracking data value with
                                           packets attribute value. */
    bcmFlowtrackerCheckActionUpdateHigherValue = 4, /* Compare packet's attribute value with
                                           tracking data value and if higher,
                                           replace tracking data value with
                                           packets attribute value. */
    bcmFlowtrackerCheckActionUpdateValue = 5, /* Replace packet's attribute value with
                                           the tracking data value. */
    bcmFlowtrackerCheckActionUpdateAverageValue = 6, /* Update tracking data with the
                                           Avergare of packet's attribute value
                                           and tracking data value */
    bcmFlowtrackerCheckActionCounterIncrByPktBytes = 7, /* Increment the Counter attached to
                                           this tracking data by number of bytes
                                           in the packet. */
    bcmFlowtrackerCheckActionCount = 8  /* Last Value. Count of Flow check
                                           Actions */
} bcm_flowtracker_check_action_t;

#define BCM_FT_CHECK_ACTION_STRINGS \
{ \
    "None", \
    "CounterIncr", \
    "CounterIncrByValue", \
    "UpdateLowerValue", \
    "UpdateHigherValue", \
    "UpdateValue", \
    "UpdateAverageValue", \
    "CounterIncrByPktBytes", \
    "Count"  \
}

/* Initialize a flow check information structure. */
extern void bcm_flowtracker_check_info_t_init(
    bcm_flowtracker_check_info_t *check_info);

/* 
 * Options that can be passed via options parameter during flow check
 * creation.
 */
#define BCM_FLOWTRACKER_CHECK_WITH_ID   (1 << 0)   /* Create a flow check with
                                                      ID. */
#define BCM_FLOWTRACKER_CHECK_SECOND    (1 << 1)   /* Create a secondary check
                                                      on existing check. */

#ifndef BCM_HIDE_DISPATCHABLE

/* Create a flowtracker flow group. */
extern int bcm_flowtracker_check_create(
    int unit, 
    uint32 options, 
    bcm_flowtracker_check_info_t check_info, 
    bcm_flowtracker_check_t *check_id);

/* Destroy a flowtracker flow check. */
extern int bcm_flowtracker_check_destroy(
    int unit, 
    bcm_flowtracker_check_t check_id);

/* Get a flowtracker flow check. */
extern int bcm_flowtracker_check_get(
    int unit, 
    bcm_flowtracker_check_t check_id, 
    bcm_flowtracker_check_info_t *check_info);

/* Destroys all flowtracker flow checks. */
extern int bcm_flowtracker_check_destroy_all(
    int unit);

#endif /* BCM_HIDE_DISPATCHABLE */

/* User callback for flow check traverse routine. */
typedef int(*bcm_flowtracker_check_traverse_cb)(
    int unit, 
    bcm_flowtracker_check_t check_id, 
    bcm_flowtracker_check_info_t *check_info, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Destroys all flowtracker flow checks. */
extern int bcm_flowtracker_check_traverse(
    int unit, 
    bcm_flowtracker_check_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Percent weightage assigned to tracking param for running average */
typedef enum bcm_flowtracker_tracking_param_weight_e {
    bcmFlowtrackerTrackingParamWeight0 = 0, /* 0 percent Weight assigned to tracking
                                           param. */
    bcmFlowtrackerTrackingParamWeight4 = 4, /* 4 percent Weight assigned to tracking
                                           param. */
    bcmFlowtrackerTrackingParamWeight8 = 8, /* 8 percent Weight assigned to tracking
                                           param. */
    bcmFlowtrackerTrackingParamWeight12 = 12, /* 12 percent Weight assigned to
                                           tracking param. */
    bcmFlowtrackerTrackingParamWeight14 = 14, /* 14 percent Weight assigned to
                                           tracking param. */
    bcmFlowtrackerTrackingParamWeight16 = 16, /* 16 percent Weight assigned to
                                           tracking param. */
    bcmFlowtrackerTrackingParamWeight19 = 19, /* 19 percent Weight assigned to
                                           tracking param. */
    bcmFlowtrackerTrackingParamWeight22 = 22, /* 22 percent Weight assigned to
                                           tracking param. */
    bcmFlowtrackerTrackingParamWeight25 = 25, /* 25 percent Weight assigned to
                                           tracking param. */
    bcmFlowtrackerTrackingParamWeight29 = 29, /* 29 percent Weight assigned to
                                           tracking param. */
    bcmFlowtrackerTrackingParamWeight33 = 33, /* 33 percent Weight assigned to
                                           tracking param. */
    bcmFlowtrackerTrackingParamWeight37 = 37, /* 37 percent Weight assigned to
                                           tracking param. */
    bcmFlowtrackerTrackingParamWeight41 = 41, /* 41 percent Weight assigned to
                                           tracking param. */
    bcmFlowtrackerTrackingParamWeight44 = 44, /* 44 percent Weight assigned to
                                           tracking param. */
    bcmFlowtrackerTrackingParamWeight47 = 47, /* 47 percent Weight assigned to
                                           tracking param. */
    bcmFlowtrackerTrackingParamWeight50 = 50, /* 50 percent Weight assigned to
                                           tracking param. */
    bcmFlowtrackerTrackingParamWeight53 = 53, /* 53 percent Weight assigned to
                                           tracking param. */
    bcmFlowtrackerTrackingParamWeight56 = 56, /* 56 percent Weight assigned to
                                           tracking param. */
    bcmFlowtrackerTrackingParamWeight59 = 59, /* 59 percent Weight assigned to
                                           tracking param. */
    bcmFlowtrackerTrackingParamWeight63 = 63, /* 63 percent Weight assigned to
                                           tracking param. */
    bcmFlowtrackerTrackingParamWeight67 = 67, /* 67 percent Weight assigned to
                                           tracking param. */
    bcmFlowtrackerTrackingParamWeight71 = 71, /* 71 percent Weight assigned to
                                           tracking param. */
    bcmFlowtrackerTrackingParamWeight75 = 75, /* 75 percent Weight assigned to
                                           tracking param. */
    bcmFlowtrackerTrackingParamWeight78 = 78, /* 78 percent Weight assigned to
                                           tracking param. */
    bcmFlowtrackerTrackingParamWeight81 = 81, /* 81 percent Weight assigned to
                                           tracking param. */
    bcmFlowtrackerTrackingParamWeight84 = 84, /* 84 percent Weight assigned to
                                           tracking param. */
    bcmFlowtrackerTrackingParamWeight86 = 86, /* 86 percent Weight assigned to
                                           tracking param. */
    bcmFlowtrackerTrackingParamWeight88 = 88, /* 88 percent Weight assigned to
                                           tracking param. */
    bcmFlowtrackerTrackingParamWeight92 = 92, /* 92 percent Weight assigned to
                                           tracking param. */
    bcmFlowtrackerTrackingParamWeight96 = 96, /* 96 percent Weight assigned to
                                           tracking param. */
    bcmFlowtrackerTrackingParamWeight100 = 100 /* 100 percent Weight assigned to
                                           tracking param. */
} bcm_flowtracker_tracking_param_weight_t;

/* Actions related information for Flow check. */
typedef struct bcm_flowtracker_check_action_info_s {
    uint32 flags;                       /* Action flags. */
    bcm_flowtracker_tracking_param_type_t param; /* The attribute of flow on which the
                                           check is performed. */
    bcm_flowtracker_check_action_t action; /* Action to be performed if check
                                           passes. */
    uint32 mask_value;                  /* Mask value to be applied on flow
                                           attribute. */
    uint8 shift_value;                  /* Right shift value to be applied on
                                           flow attribute. */
    bcm_flowtracker_tracking_param_weight_t weight; /* Weight given to current data over
                                           running average. */
    bcm_flowtracker_direction_t direction; /* Direction of flow from where tracking
                                           paramter is picked. */
    uint16 custom_id;                   /* Custom Identifier for Flowtracker
                                           Tracking parameter type custom. */
} bcm_flowtracker_check_action_info_t;

/* Initialize a flow check action information structure. */
extern void bcm_flowtracker_check_action_info_t_init(
    bcm_flowtracker_check_action_info_t *info);

#ifndef BCM_HIDE_DISPATCHABLE

/* Initialize a flow check action information structure. */
extern int bcm_flowtracker_check_action_info_set(
    int unit, 
    bcm_flowtracker_check_t check_id, 
    bcm_flowtracker_check_action_info_t info);

/* Initialize a flow check action information structure. */
extern int bcm_flowtracker_check_action_info_get(
    int unit, 
    bcm_flowtracker_check_t check_id, 
    bcm_flowtracker_check_action_info_t *info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Export related information for Flow check. */
typedef struct bcm_flowtracker_check_export_info_s {
    uint32 export_check_threshold;      /* Threshold at which the export should
                                           be triggered for this check. At each
                                           incoming matching flow's packet the
                                           tracking data is updated and that
                                           value is checked with threshold
                                           value. If the check mentioned by
                                           operation passes between two values
                                           then export is triggered. */
    bcm_flowtracker_check_operation_t operation; /* Operation to be checked with
                                           threshold value for export check. */
} bcm_flowtracker_check_export_info_t;

/* Initialize a flow check export information structure. */
extern void bcm_flowtracker_check_export_info_t_init(
    bcm_flowtracker_check_export_info_t *info);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set flow check export information to flowtracker check. */
extern int bcm_flowtracker_check_export_info_set(
    int unit, 
    bcm_flowtracker_check_t check_id, 
    bcm_flowtracker_check_export_info_t info);

/* Get flow check export information for a flowtracker check. */
extern int bcm_flowtracker_check_export_info_get(
    int unit, 
    bcm_flowtracker_check_t check_id, 
    bcm_flowtracker_check_export_info_t *info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a flow check delay information structure. */
extern void bcm_flowtracker_check_delay_info_t_init(
    bcm_flowtracker_check_delay_info_t *info);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set a flow check delay information to a flowtracker check. */
extern int bcm_flowtracker_check_delay_info_set(
    int unit, 
    bcm_flowtracker_check_t check_id, 
    bcm_flowtracker_check_delay_info_t info);

/* Get a flow check delay information structure. */
extern int bcm_flowtracker_check_delay_info_get(
    int unit, 
    bcm_flowtracker_check_t check_id, 
    bcm_flowtracker_check_delay_info_t *info);

/* Add a flow check into flow group. */
extern int bcm_flowtracker_group_check_add(
    int unit, 
    bcm_flowtracker_group_t group_id, 
    bcm_flowtracker_check_t check_id);

/* Delete a flow check into flow group. */
extern int bcm_flowtracker_group_check_delete(
    int unit, 
    bcm_flowtracker_group_t group_id, 
    bcm_flowtracker_check_t check_id);

/* Get all the flow checks associated with flow group. */
extern int bcm_flowtracker_group_check_get_all(
    int unit, 
    bcm_flowtracker_group_t group_id, 
    int max_checks, 
    bcm_flowtracker_check_t *list_of_check_ids, 
    int *num_checks);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Collector Copy Flags. */
#define BCM_FLOWTRACKER_COLLECTOR_COPY_SAMPLE_RANDOM 0x00000001 /* This flag is used to
                                                          select the random
                                                          number out of
                                                          num_pkts_skip_for_next_sample
                                                          for random sampling of
                                                          packet. If this flag
                                                          is not used then
                                                          num_pkts_skip_for_next_sample
                                                          is used as sampling
                                                          interval. */

/* Flowtracker Collector copy info. */
typedef struct bcm_flowtracker_collector_copy_info_s {
    uint32 flags;                       /* Flags for Collector copy information */
    uint32 num_pkts_initial_samples;    /*  Number of initial sample copies to
                                           be sent to collector */
    uint32 num_pkts_skip_for_next_sample; /* Number of packets to skip to send
                                           next sample copy to collector. */
} bcm_flowtracker_collector_copy_info_t;

/* Initialize a flow group collector copy information structure. */
extern void bcm_flowtracker_collector_copy_info_t_init(
    bcm_flowtracker_collector_copy_info_t *info);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set flow group's collector copy information. */
extern int bcm_flowtracker_group_collector_copy_info_set(
    int unit, 
    bcm_flowtracker_group_t group_id, 
    bcm_flowtracker_collector_copy_info_t info);

/* Get flow group's collector copy information. */
extern int bcm_flowtracker_group_collector_copy_info_get(
    int unit, 
    bcm_flowtracker_group_t group_id, 
    bcm_flowtracker_collector_copy_info_t *info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Configuration flags for flowtracker groups's meter. */
#define BCM_FLOWTRACKER_METER_MODE_PACKET   (1 << 0)   /* Group metering mode is
                                                          per packet. */

/* Flowtracker meter info. */
typedef struct bcm_flowtracker_meter_info_s {
    uint32 flags;               /* Flags for meter information */
    uint32 ckbits_sec;          /* Committed rate (kbits per sec). */
    uint32 ckbits_burst;        /* Committed burst size (kbits). */
    uint32 actual_ckbits_sec;   /* Granularity adjusted Committed rate (kbits
                                   per sec). */
    uint32 actual_ckbits_burst; /*  Granularity adjusted Committed burst size
                                   (kbits). */
} bcm_flowtracker_meter_info_t;

/* Initialize a flow group meter information structure. */
extern void bcm_flowtracker_meter_info_t_init(
    bcm_flowtracker_meter_info_t *meter_info);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set flow group's metering information. */
extern int bcm_flowtracker_group_meter_info_set(
    int unit, 
    bcm_flowtracker_group_t group_id, 
    bcm_flowtracker_meter_info_t info);

/* Set flow group's metering information. */
extern int bcm_flowtracker_group_meter_info_get(
    int unit, 
    bcm_flowtracker_group_t group_id, 
    bcm_flowtracker_meter_info_t *info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Flowtracker Tracking param defines. */
#define BCM_FLOWTRACKER_TRACKING_PARAM_MASK_LEN 16         /* Mask Length for
                                                          highest bit length
                                                          tracking parameter. */

/* bcm_flowtracker_tracking_param_mask_t */
typedef uint8 bcm_flowtracker_tracking_param_mask_t[BCM_FLOWTRACKER_TRACKING_PARAM_MASK_LEN];

/* bcm_flowtracker_param_user_entry_param_data_t */
typedef uint8 bcm_flowtracker_tracking_param_user_data_t[BCM_FLOWTRACKER_TRACKING_PARAM_MASK_LEN];

/* 
 * Options that can be passed via options parameter during export
 * template creation.
 */
#define BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY (1 << 0)   /* Tracking parameter is
                                                          a key. */
#define BCM_FLOWTRACKER_TRACKING_PARAM_TIMESTAMP_EVENT1 (1 << 1)   /* Tracking parameter use
                                                          timestamp engine 1. */
#define BCM_FLOWTRACKER_TRACKING_PARAM_TIMESTAMP_EVENT2 (1 << 2)   /* Tracking parameter use
                                                          timestamp engine 2. */
#define BCM_FLOWTRACKER_TRACKING_PARAM_TIMESTAMP_EVENT3 (1 << 3)   /* Tracking parameter use
                                                          timestamp engine 3. */
#define BCM_FLOWTRACKER_TRACKING_PARAM_TIMESTAMP_EVENT4 (1 << 4)   /* Tracking parameter use
                                                          timestamp engine 4. */
#define BCM_FLOWTRACKER_TRACKING_PARAM_STICKYOR_UPDATE (1 << 5)   /* Tracking parameter is
                                                          sticky. New value will
                                                          be ORed with old value
                                                          and then updated in
                                                          session data. */

/* Flowtracker tracking parameter information. */
typedef struct bcm_flowtracker_tracking_param_info_s {
    uint32 flags;                       /* Flags for tracking parameters. */
    bcm_flowtracker_tracking_param_type_t param; /* Type of tracking parameter. */
    bcm_flowtracker_tracking_param_user_data_t tracking_data; /* Element data for the tracking param
                                           to be used to add a flow. */
    bcm_flowtracker_tracking_param_mask_t mask; /* Mask to select granular information
                                           from tracking parameter. By default,
                                           mask is set to all enabled. */
    bcm_flowtracker_check_t check_id;   /* Flowtracker check to be used to
                                           tracking flow check data. */
    bcm_flowtracker_timestamp_source_t src_ts; /* Timestamp source info, valid in case
                                           of timestamp and delay related
                                           tracking params. */
    bcm_udf_id_t udf_id;                /* UDF Id associated with the tracking
                                           param. */
    bcm_flowtracker_direction_t direction; /* Direction of flow from where tracking
                                           parameter is picked. */
    uint16 custom_id;                   /* Custom Identifier for Flowtracker
                                           Tracking parameter type custom. */
} bcm_flowtracker_tracking_param_info_t;

/* Initialize flowtracker's tracking parameters information structure. */
extern void bcm_flowtracker_tracking_param_info_t_init(
    bcm_flowtracker_tracking_param_info_t *info);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set tracking parameter for this flowtracker group. */
extern int bcm_flowtracker_group_tracking_params_set(
    int unit, 
    bcm_flowtracker_group_t id, 
    int num_tracking_params, 
    bcm_flowtracker_tracking_param_info_t *list_of_tracking_params);

/* Get flowtracker tracking parameters for this group. */
extern int bcm_flowtracker_group_tracking_params_get(
    int unit, 
    bcm_flowtracker_group_t id, 
    int max_size, 
    bcm_flowtracker_tracking_param_info_t *list_of_tracking_params, 
    int *list_size);

/* Clear information associated with multiple flow groups. */
extern int bcm_flowtracker_group_multi_clear(
    int unit, 
    uint32 flags, 
    int num_groups, 
    bcm_flowtracker_group_t *list_of_group_ids);

#endif /* BCM_HIDE_DISPATCHABLE */

/* State transition types support for the flowtracker group */
typedef enum bcm_flowtracker_state_transition_type_e {
    bcmFlowtrackerStateTransitionTypeTcpControl = 0, /* Use TCP control from outer header of
                                           the packet for state transitions. */
    bcmFlowtrackerStateTransitionTypeInnerTcpControl = 1, /* Use TCP control from inner header of
                                           the packet for state transition. */
    bcmFlowtrackerStateTransitionTypeCount = 2 /* Last. */
} bcm_flowtracker_state_transition_type_t;

/* Flowtracker Group Control types */
typedef enum bcm_flowtracker_group_control_type_e {
    bcmFlowtrackerGroupControlNone = 0, /* No Group Control. */
    bcmFlowtrackerGroupControlNewLearnEnable = 1, /* Control to Enable/disable New flow
                                           learn on this group. */
    bcmFlowtrackerGroupControlFlowtrackerEnable = 2, /* Control to Enable/disable flow
                                           tracking on this group */
    bcmFlowtrackerGroupControlFlowDirection = 3, /* Control to set flow direction for
                                           tracking forward/reverse flow. */
    bcmFlowtrackerGroupControlUdpFlowDirection = 4, /* Control to set flow direction for
                                           tracking forward/reverse UDP flow. */
    bcmFlowtrackerGroupControlStateTransitionType = 5, /* Control to set state transition type
                                           on this flowtracker group. */
    bcmFlowtrackerGroupControlCount = 6 /* Total Count of Group controls. */
} bcm_flowtracker_group_control_type_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set behavior controls of flow group. */
extern int bcm_flowtracker_group_control_set(
    int unit, 
    bcm_flowtracker_group_t group_id, 
    bcm_flowtracker_group_control_type_t type, 
    int arg);

/* Get flow group's controls. */
extern int bcm_flowtracker_group_control_get(
    int unit, 
    bcm_flowtracker_group_t group_id, 
    bcm_flowtracker_group_control_type_t type, 
    int *arg);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Flowtracker Tracking param defines. */
#define BCM_FLOWTRACKER_TRACKING_USER_ENTRY_WIDTH 48         /* Max width of
                                                          flowtracker user entry
                                                          key. */

/* Flowtracker user entry handle information. */
typedef struct bcm_flowtracker_user_entry_handle_s {
    bcm_flowtracker_group_t flow_group_id; /* Flowtracker group Id. */
    int flow_index;                     /* Flow index. */
    int flow_key_mode;                  /* Flow Key mode. */
    int flow_key_type;                  /* Flow Key type. */
    uint8 key_data[BCM_FLOWTRACKER_TRACKING_USER_ENTRY_WIDTH]; /* Flow user entry Key. */
} bcm_flowtracker_user_entry_handle_t;

/* Initialize flowtracker user entry handle information structure. */
extern void bcm_flowtracker_user_entry_handle_t_init(
    bcm_flowtracker_user_entry_handle_t *user_entry_handle);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Add a user flow entry basis user input key elements. API expects that
 * all tracking parametrs of type = 'KEY' in the group are specified.
 */
extern int bcm_flowtracker_user_entry_add(
    int unit, 
    bcm_flowtracker_group_t flow_group_id, 
    uint32 options, 
    int num_user_entry_params, 
    bcm_flowtracker_tracking_param_info_t *user_entry_param_list, 
    bcm_flowtracker_user_entry_handle_t *entry_handle);

/* Fetch user entry info given the entry handle. */
extern int bcm_flowtracker_user_entry_get(
    int unit, 
    bcm_flowtracker_user_entry_handle_t *entry_handle, 
    int num_user_entry_params, 
    bcm_flowtracker_tracking_param_info_t *user_entry_param_list, 
    int *actual_user_entry_params);

/* Fetch all user entries added in a against a given flow group. */
extern int bcm_flowtracker_user_entry_get_all(
    int unit, 
    bcm_flowtracker_group_t flow_group_id, 
    int num_user_entry_handles, 
    bcm_flowtracker_user_entry_handle_t *user_entry_handle_list, 
    int *actual_user_entry_handles);

/* Delete user flow entry that is added earlier. */
extern int bcm_flowtracker_user_entry_delete(
    int unit, 
    bcm_flowtracker_group_t flow_group_id, 
    bcm_flowtracker_user_entry_handle_t *entry_handle);

/* Delete user flow entry that is added earlier. */
extern int bcm_flowtracker_user_entry_delete_all(
    int unit, 
    bcm_flowtracker_group_t flow_group_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Stats of the Flows on the group. */
typedef struct bcm_flowtracker_group_stat_s {
    uint64 flow_exceeded_count;         /* Count of flows not tracked in this
                                           group after maximum number of flows
                                           configured for this group are already
                                           learnt. */
    uint64 flow_missed_count;           /* Count of Aged out flows in the group. */
    uint64 flow_aged_out_count;         /* Count of flows which are Aged out in
                                           the group. */
    uint64 flow_learnt_count;           /* Count of flows learnt on the group. */
    uint64 flow_meter_exceeded_count;   /* Count of flows which exceeded the
                                           metering limits in the group. */
    uint64 flow_start_count;            /* Count of flows for which connection
                                           established trigger is received. */
    uint64 flow_end_count;              /* Count of flows for which connection
                                           closed trigger is received. */
} bcm_flowtracker_group_stat_t;

/* Initialize a flowtracker group statistics structure. */
extern void bcm_flowtracker_group_stat_t_init(
    bcm_flowtracker_group_stat_t *group_stats);

#ifndef BCM_HIDE_DISPATCHABLE

/* Get flow group's statistics. */
extern int bcm_flowtracker_group_stat_get(
    int unit, 
    bcm_flowtracker_group_t group_id, 
    bcm_flowtracker_group_stat_t *group_stats);

/* Set flow group's statistics. */
extern int bcm_flowtracker_group_stat_set(
    int unit, 
    bcm_flowtracker_group_t group_id, 
    bcm_flowtracker_group_stat_t *group_stats);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Max size of the data record received at the CPU through the FIFO DMA
 * reception.
 */
#define BCM_FLOWTRACKER_EXPORT_RECORD_LENGTH 128        /* Maximum length of a
                                                          data record. 1024
                                                          bits. */

/* 
 * Flowtracker Export record structure. Data record info passed to
 * application software by SDK Software. The unprocessed data record will
 * be presented in network byte order.
 */
typedef struct bcm_flowtracker_export_record_s {
    int size;                           /* Number of valid octets in the
                                           data_record array for the device. */
    uint8 data_record[BCM_FLOWTRACKER_EXPORT_RECORD_LENGTH]; /* Raw data record in network byte
                                           order. */
} bcm_flowtracker_export_record_t;

/* bcm_flowtracker_export_record_t_init */
extern void bcm_flowtracker_export_record_t_init(
    bcm_flowtracker_export_record_t *record);

/* 
 * Defines for priorities of registered handlers for Flowtracker export
 * records. Callback handlers are always called from highest priority to
 * lowest priority until one of the handlers returns record_handled or
 * record_handled_owned.
 * 
 * 
 * When callback handling proecedure starts, a default callback handler
 * is installed at BCM_FLOWTRACKER_EXPORT_CB_PRIO_MIN priority that
 * simply discards the record.
 */
#define BCM_FLOWTRACKER_EXPORT_CB_PRIO_MIN  ((uint8)0) 
#define BCM_FLOWTRACKER_EXPORT_CB_PRIO_MAX  ((uint8)255) 

/* Flowtracker Export callback options */
typedef struct bcm_flowtracker_collector_callback_options_s {
    int callback_prio;  /* Priority of the callback function in the list. */
} bcm_flowtracker_collector_callback_options_t;

/* bcm_flowtracker_collector_callback_options_t_init */
extern void bcm_flowtracker_collector_callback_options_t_init(
    bcm_flowtracker_collector_callback_options_t *callback_options);

/* Return values from Flowtracker export record callout routines. */
typedef enum bcm_flowtracker_export_record_handle_e {
    BCM_FLOWTRACKER_EXPORT_RECORD_INVALID = 0, /* Invalid return value. */
    BCM_FLOWTRACKER_EXPORT_RECORD_NOT_HANDLED = 1, /* Export record not processed. */
    BCM_FLOWTRACKER_EXPORT_RECORD_HANDLED = 2, /* Export record handled, not owned. */
    BCM_FLOWTRACKER_EXPORT_RECORD_HANDLED_OWNED = 3 /* Export record handled and owned. */
} bcm_flowtracker_export_record_handle_t;

/* Callback routine for flowtracker record exported to local collector. */
typedef bcm_flowtracker_export_record_handle_t (*bcm_flowtracker_export_record_cb_f)(
    int unit, 
    bcm_flowtracker_collector_t collector_id, 
    bcm_flowtracker_export_record_t *record, 
    void *userdata);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Register callback routine for Local collector Flowtracker record
 * export.
 */
extern int bcm_flowtracker_export_record_register(
    int unit, 
    bcm_flowtracker_collector_t collector_id, 
    bcm_flowtracker_collector_callback_options_t callback_options, 
    bcm_flowtracker_export_record_cb_f callback_fn, 
    void *userdata);

/* 
 * Unregister callback routine for Local collector Flowtracker record
 * export.
 */
extern int bcm_flowtracker_export_record_unregister(
    int unit, 
    bcm_flowtracker_collector_t collector_id, 
    bcm_flowtracker_collector_callback_options_t callback_options, 
    bcm_flowtracker_export_record_cb_f callback_fn);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Group actions set flags. */
#define BCM_FLOWTRACKER_GROUP_ACTIONS_SET_FLAGS_ELEPHANT_FLOWS (1 << 0)   /* Actions apply to only
                                                          elephant flows in the
                                                          group. */

/* 
 * Packet Color, need to be specified in param0 when using
 * bcmFlowtrackerGroupActionXXDropPrecedence.
 */
#define BCM_FLOWTRACKER_GROUP_ACTION_PKT_COLOR_GREEN 1          
#define BCM_FLOWTRACKER_GROUP_ACTION_PKT_COLOR_YELLOW 2          
#define BCM_FLOWTRACKER_GROUP_ACTION_PKT_COLOR_RED 3          

/* Flowtracker group action destination types */
typedef enum bcm_flowtracker_group_action_dest_type_e {
    bcmFlowtrackerGroupActionDestTypeL2EgrIntf = 0, /* L2 Outgoing Interface */
    bcmFlowtrackerGroupActionDestTypeVp = 1, /* Virtual Port */
    bcmFlowtrackerGroupActionDestTypeEcmp = 2, /* ECMP */
    bcmFlowtrackerGroupActionDestTypeL3Egr = 3, /* Next Hop Index */
    bcmFlowtrackerGroupActionDestTypeL2Mcast = 4, /* Layer2 Multicast */
    bcmFlowtrackerGroupActionDestTypeL3Mcast = 5, /* Layer3 Multicast */
    bcmFlowtrackerGroupActionDestTypeCount = 6 /* Always Last. Not a usable value */
} bcm_flowtracker_group_action_dest_type_t;

/* Flowtracker Group action types */
typedef enum bcm_flowtracker_group_action_e {
    bcmFlowtrackerGroupActionRpDropPrecedence = 0, /* Set Red Priority Drop Precedence;
                                           param0:
                                           BCM_FLOWTRACKER_GROUP_ACTION_PKT_COLOR_XXX.. */
    bcmFlowtrackerGroupActionYpDropPrecedence = 1, /* Set Yellow Priority Drop Precedence;
                                           param0:
                                           BCM_FLOWTRACKER_GROUP_ACTION_PKT_COLOR_XXX.. */
    bcmFlowtrackerGroupActionGpDropPrecedence = 2, /* Set Green Priority Drop Precedence;
                                           param0:
                                           BCM_FLOWTRACKER_GROUP_ACTION_PKT_COLOR_XXX.. */
    bcmFlowtrackerGroupActionUcastCosQNew = 3, /* Change CoS Queue for unicast packets;
                                           param0: New CoS Queue. */
    bcmFlowtrackerGroupActionMcastCosQNew = 4, /* Change CoS Queue for multicast
                                           packets; param0: New CoS Queue. */
    bcmFlowtrackerGroupActionPrioIntNew = 5, /* Change internal priority; param0: New
                                           internal priority */
    bcmFlowtrackerGroupActionFspReinject = 6, /* Re-inject the First Seen Packet to
                                           the specified CosQ */
    bcmFlowtrackerGroupActionDropMonitor = 7, /* Monitor Dropped flows */
    bcmFlowtrackerGroupActionCongestion = 8, /* Queue Congestion Monitoring report to
                                           collector */
    bcmFlowtrackerGroupActionOpaqueObject = 9, /* Set the exact match opaque object
                                           value that is used to match in IFP */
    bcmFlowtrackerGroupActionFlexCtrAssign = 10, /* Set the flex counter action profile */
    bcmFlowtrackerGroupActionIfaInsert = 11, /* Insert IFA header (in initiator node)
                                           and metadata */
    bcmFlowtrackerGroupActionIfaDelete = 12, /* Delete IFA header and metadata */
    bcmFlowtrackerGroupActionVxlanGbpEnable = 13, /* Enable VXLAN Group based policy */
    bcmFlowtrackerGroupActionINTResidenceTimeEnable = 14, /* Enable use of residence time in In
                                           band telemetry packets */
    bcmFlowtrackerGroupActionCopyToCpu = 15, /* Copy the packet to CPU */
    bcmFlowtrackerGroupActionDrop = 16, /* Drop the packet */
    bcmFlowtrackerGroupActionDropCancel = 17, /* Override the drop action in another
                                           rule */
    bcmFlowtrackerGroupActionRedirectPort = 18, /* Redirect packet to single port or
                                           virtual port;param0: Gport */
    bcmFlowtrackerGroupActionRedirectIpmc = 19, /* Replace L3 multicast L3 & L2 bitmap;
                                           param0: IP Multicast Index. */
    bcmFlowtrackerGroupActionRedirectMcast = 20, /* Replace L2 multicast dest bitmap;
                                           param0: L2 Multicast Index. */
    bcmFlowtrackerGroupActionL3Switch = 21, /* L3-switch packet; param0: ECMP or
                                           next hop info. */
    bcmFlowtrackerGroupActionRedirectCancel = 22, /* Override the redirect action in
                                           another rule */
    bcmFlowtrackerGroupActionDestinationType = 23, /* Set destination type for the flow */
    bcmFlowtrackerGroupActionFlexStateAssign = 24, /* Set flex state action profile */
    bcmFlowtrackerGroupActionMirrorIndex = 25, /* Set mirror index */
    bcmFlowtrackerGroupActionCount = 26 /* Always Last. Not a usable value. */
} bcm_flowtracker_group_action_t;

/* Flowtracker Group action params */
typedef struct bcm_flowtracker_group_action_param_s {
    uint32 param0; 
    uint32 param1; 
} bcm_flowtracker_group_action_param_t;

/* Flowtracker Group action info */
typedef struct bcm_flowtracker_group_action_info_s {
    bcm_flowtracker_group_action_t action; /* Action */
    bcm_flowtracker_group_action_param_t params; /* Action param */
} bcm_flowtracker_group_action_info_t;

/* Initialize a action information structure. */
extern void bcm_flowtracker_group_action_info_t_init(
    bcm_flowtracker_group_action_info_t *action_info);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set list of actions on a flow group. */
extern int bcm_flowtracker_group_actions_set(
    int unit, 
    bcm_flowtracker_group_t flow_group_id, 
    uint32 flags, 
    int num_actions, 
    bcm_flowtracker_group_action_info_t *action_list);

/* Get list of actions applied on a flow group. */
extern int bcm_flowtracker_group_actions_get(
    int unit, 
    bcm_flowtracker_group_t flow_group_id, 
    uint32 flags, 
    int max_actions, 
    bcm_flowtracker_group_action_info_t *action_list, 
    int *num_actions);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Miscellaneous Flowtracker Elephant defines. */
#define BCM_FLOWTRACKER_ELEPHANT_MAX_PROMOTION_FILTERS 2          /* Maximum number of
                                                          promotion filters in
                                                          an elephant profile. */

/* Elephant filter flags. */
#define BCM_FLOWTRACKER_ELEPHANT_FILTER_FLAGS_INCREASING_RATE (1 << 0)   /* Rate must increase on
                                                          every scan */

/* 
 * Options that can be passed via options parameter during elephant
 * profile creation.
 */
#define BCM_FLOWTRACKER_ELEPHANT_PROFILE_WITH_ID (1 << 0)   /* Create an elephant
                                                          profile  with ID. */

/* Mirror on Drop class ID flag. */
#define BCM_FLOWTRACKER_EXACT_MATCH_CLASS_ID_FLOW_DROP 1          /* Exact Match flow drop
                                                          indicator class ID */

/* Flowtracker Elephant profile Id. */
typedef int bcm_flowtracker_elephant_profile_t;

/* Flowtracker Elephant filter config */
typedef struct bcm_flowtracker_elephant_filter_s {
    uint32 flags;                       /* See
                                           BCM_FLOWTRACKER_ELEPHANT_FILTER_FLAGS_XXX
                                           definitions */
    uint32 monitor_interval_usecs;      /* How long should the flow be monitored */
    uint32 rate_low_threshold_kbits_sec; /* Minimum rate the flow must have to
                                           continue monitoring (kbits_sec = 1000
                                           bits/sec) */
    uint32 rate_high_threshold_kbits_sec; /* Rate that must be observed at least
                                           once during monitoring interval for a
                                           flow to pass the monitoring criteria
                                           (kbits_sec = 1000 bits/sec) */
    uint64 size_threshold_bytes;        /* Total size in bytes that must be
                                           observed after the monitoring
                                           interval for a flow to pass the
                                           monitoring criteria */
} bcm_flowtracker_elephant_filter_t;

/* Flowtracker Elephant profile info */
typedef struct bcm_flowtracker_elephant_profile_info_s {
    uint8 num_promotion_filters;        /* Number of promotion filters */
    bcm_flowtracker_elephant_filter_t promotion_filters[BCM_FLOWTRACKER_ELEPHANT_MAX_PROMOTION_FILTERS]; /* Promotion filters */
    bcm_flowtracker_elephant_filter_t demotion_filter; /* Demotion filter */
} bcm_flowtracker_elephant_profile_info_t;

/* Initialize an elephant profile */
extern void bcm_flowtracker_elephant_profile_info_t_init(
    bcm_flowtracker_elephant_profile_info_t *profile);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create flowtracker elephant profile. */
extern int bcm_flowtracker_elephant_profile_create(
    int unit, 
    uint32 options, 
    bcm_flowtracker_elephant_profile_info_t *profile, 
    bcm_flowtracker_elephant_profile_t *profile_id);

/* Destroy a flowtracker elephant profile */
extern int bcm_flowtracker_elephant_profile_destroy(
    int unit, 
    bcm_flowtracker_elephant_profile_t profile_id);

/* Get flowtracker elephant profile information. */
extern int bcm_flowtracker_elephant_profile_get(
    int unit, 
    bcm_flowtracker_elephant_profile_t profile_id, 
    bcm_flowtracker_elephant_profile_info_t *profile);

/* Get all the configured flowtracker elephant profiles */
extern int bcm_flowtracker_elephant_profile_get_all(
    int unit, 
    int max, 
    bcm_flowtracker_elephant_profile_t *profile_list, 
    int *count);

/* Attach a flow group with an elephant profile. */
extern int bcm_flowtracker_group_elephant_profile_attach(
    int unit, 
    bcm_flowtracker_group_t flow_group_id, 
    bcm_flowtracker_elephant_profile_t profile_id);

/* Get the elephant profile Id attached to a flow group. */
extern int bcm_flowtracker_group_elephant_profile_attach_get(
    int unit, 
    bcm_flowtracker_group_t flow_group_id, 
    bcm_flowtracker_elephant_profile_t *profile_id);

/* Detach a flow group from an elephant profile. */
extern int bcm_flowtracker_group_elephant_profile_detach(
    int unit, 
    bcm_flowtracker_group_t flow_group_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Elephant match types */
#define BCM_FLOWTRACKER_ELEPHANT_MATCH_TYPE_PKT_TYPE_BMP (1 << 0)   /* Packet type bitmap. */
#define BCM_FLOWTRACKER_ELEPHANT_MATCH_TYPE_INT_PRI_BMP (1 << 1)   /* Internal priority
                                                          bitmap. */
#define BCM_FLOWTRACKER_ELEPHANT_MATCH_TYPE_INGRESS_PBMP (1 << 2)   /* Ingress port bitmap. */
#define BCM_FLOWTRACKER_ELEPHANT_MATCH_TYPE_EGRESS_PBMP (1 << 3)   /* Egress port bitmap. */

/* Elephant flow packet types */
#define BCM_FLOWTRACKER_ELEPHANT_PKT_TYPE_IPV4_TCP (1 << 0)   /* IPv4 TCP packets. */
#define BCM_FLOWTRACKER_ELEPHANT_PKT_TYPE_IPV4_UDP (1 << 1)   /* IPv4 UDP packets. */
#define BCM_FLOWTRACKER_ELEPHANT_PKT_TYPE_IPV4_OTHER (1 << 2)   /* IPv4 non TCP, UDP
                                                          packets. */
#define BCM_FLOWTRACKER_ELEPHANT_PKT_TYPE_IPV6_TCP (1 << 3)   /* IPv6 TCP packets. */
#define BCM_FLOWTRACKER_ELEPHANT_PKT_TYPE_IPV6_UDP (1 << 4)   /* IPv6 UDP packets. */
#define BCM_FLOWTRACKER_ELEPHANT_PKT_TYPE_IPV6_OTHER (1 << 5)   /* IPv6 non TCP, UDP
                                                          packets. */
#define BCM_FLOWTRACKER_ELEPHANT_PKT_TYPE_MPLS (1 << 6)   /* MPLS packets. */
#define BCM_FLOWTRACKER_ELEPHANT_PKT_TYPE_OTHER (1 << 7)   /* Non IPv4, IPv6, MPLS
                                                          packets. */

/* Elephant match data. */
typedef struct bcm_flowtracker_elephant_match_data_s {
    bcm_pbmp_t ingress_pbmp;    /* Ingress port bitmap */
    bcm_pbmp_t egress_pbmp;     /* Egress port bitmap */
    uint32 pkt_type_bmp;        /* pkt type bitmap, See
                                   BCM_FLOWTRACKER_ELEPHANT_PKT_TYPE_XXX */
    uint32 int_pri_bmp;         /* Internal priority bitmap */
} bcm_flowtracker_elephant_match_data_t;

/* Initialize elephant match data. */
extern void bcm_flowtracker_elephant_match_data_t_init(
    bcm_flowtracker_elephant_match_data_t *match_data);

typedef enum bcm_flowtracker_elephant_match_action_e {
    bcmFlowtrackerElephantMatchActionLookup = 0, /* Enable elephant lookup */
    bcmFlowtrackerElephantMatchActionQueue = 1, /* Enable queue actions */
    bcmFlowtrackerElephantMatchActionColor = 2, /* Enable color actions */
    bcmFlowtrackerElephantMatchActionCount = 3 /* Always Last. Not a usable value. */
} bcm_flowtracker_elephant_match_action_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set the match criteria for an elephant action. */
extern int bcm_flowtracker_elephant_action_match_set(
    int unit, 
    bcm_flowtracker_elephant_match_action_t action, 
    uint32 match_types, 
    bcm_flowtracker_elephant_match_data_t *match_data);

/* Get the match criteria for an elephant action. */
extern int bcm_flowtracker_elephant_action_match_get(
    int unit, 
    bcm_flowtracker_elephant_match_action_t action, 
    uint32 *match_types, 
    bcm_flowtracker_elephant_match_data_t *match_data);

/* Set the internal priority remap for elephant flows. */
extern int bcm_flowtracker_elephant_int_pri_remap_set(
    int unit, 
    bcm_cos_t int_pri, 
    bcm_cos_t new_int_pri);

/* Get the internal priority remap for elephant flows. */
extern int bcm_flowtracker_elephant_int_pri_remap_get(
    int unit, 
    bcm_cos_t int_pri, 
    bcm_cos_t *new_int_pri);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Elephant controls */
typedef enum bcm_flowtracker_elephant_control_e {
    bcmFlowtrackerElephantControlMonitorIntervalUsecs = 0, /* Elephant monitor interval in
                                           microseconds */
    bcmFlowtrackerElephantControlBloomFilterByteThreshold = 1, /* Bloom filter promotion threshold in
                                           bytes */
    bcmFlowtrackerElephantControlElephantThresholdBytes = 2, /* Elephant promotion threshold in bytes */
    bcmFlowtrackerElephantControlDemotionThresholdBytes = 3, /* Elephant demotion threshold in bytes */
    bcmFlowtrackerElephantControlYellowThreshold = 4, /* Threshold in bytes to mark an
                                           elephant flow as yellow */
    bcmFlowtrackerElephantControlRedThreshold = 5, /* Threshold in bytes to mark an
                                           elephant flow as red */
    bcmFlowtrackerElephantControlPacketRemarkEnable = 6, /* Enable packet remarking */
    bcmFlowtrackerElephantControlSampleRate = 7, /* Set elephant flow sample rate (Every
                                           1 in 'arg' number of packets will be
                                           sampled) */
    bcmFlowtrackerElephantControlSampleSeed = 8, /* Set seed for the random number
                                           generator used for sampling */
    bcmFlowtrackerElephantControlSampleCopyToCpu = 9, /* Copy to CPU sampled packets */
    bcmFlowtrackerElephantControlCount = 10 /* Last Value. Not Usable. */
} bcm_flowtracker_elephant_control_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set elephant controls */
extern int bcm_flowtracker_elephant_control_set(
    int unit, 
    bcm_flowtracker_elephant_control_t  type, 
    int arg);

/* Get elephant controls */
extern int bcm_flowtracker_elephant_control_get(
    int unit, 
    bcm_flowtracker_elephant_control_t  type, 
    int *arg);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Elephant hash tables */
typedef enum bcm_flowtracker_elephant_hash_table_e {
    bcmFlowtrackerElephantHashTableBloomFilter = 0, /* Bloom filter hash table. */
    bcmFlowtrackerElephantHashTableFlowTable = 1, /* Elephant flow table. */
    bcmFlowtrackerElephantHashTableCount = 2 /* Last Value. Not Usable. */
} bcm_flowtracker_elephant_hash_table_t;

/* Elephant hash types */
typedef enum bcm_flowtracker_elephant_hash_type_e {
    bcmFlowtrackerElephantHashTypeField0Function0 = 0, /* Hash field 0, mode 0 configured using
                                           bcmSwitchHashField0Config */
    bcmFlowtrackerElephantHashTypeField0Function1 = 1, /* Hash field 0, mode 1 configured using
                                           bcmSwitchHashField0Config1. */
    bcmFlowtrackerElephantHashTypeField1Function0 = 2, /* Hash field 1, mode 0 configured using
                                           bcmSwitchHashField1Config. */
    bcmFlowtrackerElephantHashTypeField1Function1 = 3, /* Hash field 1, mode 1 configured using
                                           bcmSwitchHashField1Config1. */
    bcmbcmFlowtrackerElephantHashTypeCount = 4 /* Last Value. Not Usable. */
} bcm_flowtracker_elephant_hash_type_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set the hashing configuration */
extern int bcm_flowtracker_elephant_hash_config_set(
    int unit, 
    bcm_flowtracker_elephant_hash_table_t hash_table, 
    int instance_num, 
    int bank_num, 
    bcm_flowtracker_elephant_hash_type_t hash_type, 
    int right_rotate_bits);

/* Get the hashing configuration */
extern int bcm_flowtracker_elephant_hash_config_get(
    int unit, 
    bcm_flowtracker_elephant_hash_table_t hash_table, 
    int instance_num, 
    int bank_num, 
    bcm_flowtracker_elephant_hash_type_t *hash_type, 
    int *right_rotate_bits);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Elephant statistics */
typedef struct bcm_flowtracker_elephant_stats_s {
    uint64 num_candidates_detected;     /* Total number of candidate flows that
                                           were detected */
    uint64 num_flow_table_insert_failures; /* Total number of elephant flow table
                                           insertion failures */
    uint64 num_flow_table_insert_success; /* Total number of elephant flow table
                                           insertion success */
    uint64 num_elephants_detected;      /* Total number of elephants that were
                                           detected */
} bcm_flowtracker_elephant_stats_t;

/* Initialize elephant monitoring statistics structure. */
extern void bcm_flowtracker_elephant_stats_t_init(
    bcm_flowtracker_elephant_stats_t *stats);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set the elephant monitoring statistics */
extern int bcm_flowtracker_elephant_stats_set(
    int unit, 
    bcm_flowtracker_elephant_stats_t *stats);

/* Get the elephant monitoring statistics */
extern int bcm_flowtracker_elephant_stats_get(
    int unit, 
    bcm_flowtracker_elephant_stats_t *stats);

/* Get the elephant monitoring statistics */
extern int bcm_flowtracker_elephant_stats_sync_get(
    int unit, 
    bcm_flowtracker_elephant_stats_t *stats);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Flowtracker Chip Debug configuration. */
typedef struct bcm_flowtracker_chip_debug_info_s {
    bcm_flowtracker_tracking_param_type_t param; /* The attribute of flow on which
                                           debugging is enabled. */
    uint32 count;                       /* Count of the pass events happened for
                                           a particular check. */
} bcm_flowtracker_chip_debug_info_t;

/* Initialize flowtracker Chip Debug information. */
extern void bcm_flowtracker_chip_debug_info_t_init(
    bcm_flowtracker_chip_debug_info_t *info);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set flowtracker chip wide debug tracking parameters info. */
extern int bcm_flowtracker_chip_debug_info_set(
    int unit, 
    int num_debug_info, 
    bcm_flowtracker_chip_debug_info_t *list_of_debug_info);

/* Get flowtracker tracking parameters debug information. */
extern int bcm_flowtracker_chip_debug_info_get(
    int unit, 
    int max_debug_info_size, 
    bcm_flowtracker_chip_debug_info_t *list_of_debug_info, 
    int *actual_debug_info_size);

/* Add a new static flow to the flow group. */
extern int bcm_flowtracker_static_flow_add(
    int unit, 
    bcm_flowtracker_group_t flow_group_id, 
    bcm_flowtracker_flow_key_t *flow_key);

/* Delete an existing static flow from the flow group. */
extern int bcm_flowtracker_static_flow_delete(
    int unit, 
    bcm_flowtracker_group_t flow_group_id, 
    bcm_flowtracker_flow_key_t *flow_key);

/* Delete all existing static flows from the flow group. */
extern int bcm_flowtracker_static_flow_delete_all(
    int unit, 
    bcm_flowtracker_group_t flow_group_id);

/* Get all static flows installed for a given flow group. */
extern int bcm_flowtracker_static_flow_get_all(
    int unit, 
    bcm_flowtracker_group_t flow_group_id, 
    int max_size, 
    bcm_flowtracker_flow_key_t *flow_key_arr, 
    int *list_size);

#endif /* BCM_HIDE_DISPATCHABLE */

/* The different sources for Flowtracker Class */
typedef enum bcm_flowtracker_class_source_type_e {
    bcmFlowtrackerClassSourceTypeNone = 0, /* No Class Id. */
    bcmFlowtrackerClassSourceTypePort = 1, /* Class Id from Port. */
    bcmFlowtrackerClassSourceTypeSvp = 2, /* Class Id from Source Vp. */
    bcmFlowtrackerClassSourceTypeL3Interface = 3, /* Class Id from L3 Interface. */
    bcmFlowtrackerClassSourceTypeFieldSrc = 4, /* Class Id from VFP HI. */
    bcmFlowtrackerClassSourceTypeFieldDst = 5, /* Class Id from VFP LO. */
    bcmFlowtrackerClassSourceTypeL2Src = 6, /* Class Id from L2 Src. */
    bcmFlowtrackerClassSourceTypeL2Dst = 7, /* Class Id from L2 Dst. */
    bcmFlowtrackerClassSourceTypeL3Src = 8, /* Class Id from L3 Src. */
    bcmFlowtrackerClassSourceTypeL3Dst = 9, /* Class Id from L3 Dst. */
    bcmFlowtrackerClassSourceTypeVlan = 10, /* Class Id from Vlan. */
    bcmFlowtrackerClassSourceTypeVrf = 11, /* Class Id from Vrf. */
    bcmFlowtrackerClassSourceTypeFieldExactMatchLookUp0 = 12, /* Class Id from ExactMatch First
                                           Lookup. */
    bcmFlowtrackerClassSourceTypeFieldExactMatchLookUp1 = 13, /* Class Id from ExactMatch Second
                                           Lookup. */
    bcmFlowtrackerClassSourceTypeFieldIngress = 14, /* Class Id from IFP Policy Table. */
    bcmFlowtrackerClassSourceTypeI2E = 15, /* Class Id from Ingress to Egress. */
    bcmFlowtrackerClassSourceTypeNextHopA = 16, /* Class Id from NextHop A. */
    bcmFlowtrackerClassSourceTypeNextHopB = 17, /* Class Id from NextHop B. */
    bcmFlowtrackerClassSourceTypeL3InterfaceA = 18, /* Class Id from L3 Interface A. */
    bcmFlowtrackerClassSourceTypeL3InterfaceB = 19, /* Class Id from L3 Interface B. */
    bcmFlowtrackerClassSourceTypeDstVp = 20, /* Class Id from Destination virtual
                                           port */
    bcmFlowtrackerClassSourceTypeIpTunnel = 21, /* Class Id from Ip Tunnel. */
    bcmFlowtrackerClassSourceTypeXlateNextHopA = 22, /* Class Id from translated NextHop A. */
    bcmFlowtrackerClassSourceTypeXlateNextHopB = 23, /* Class Id from translated NextHop B. */
    bcmFlowtrackerClassSourceTypeXlateL3InterfaceA = 24, /* Class Id from translated L3 Interface
                                           A. */
    bcmFlowtrackerClassSourceTypeXlateL3InterfaceB = 25, /* Class Id from translated L3 Interface
                                           B. */
    bcmFlowtrackerClassSourceTypeXlateDstVp = 26, /* Class Id from translated destination
                                           virtual port */
    bcmFlowtrackerClassSourceTypeXlateIpTunnel = 27, /* Class Id from translated Ip Tunnel. */
    bcmFlowtrackerClassSourceTypeCount = 28 /* Always Last. Not a usable value. */
} bcm_flowtracker_class_source_type_t;

#define BCM_FLOWTRACKER_CLASS_SOURCE_TYPE_STRINGS \
{ \
    "None", \
    "Port", \
    "Svp", \
    "L3Interface", \
    "FieldSrc", \
    "FieldDst", \
    "L2Src", \
    "L2Dst", \
    "L3Src", \
    "L3Dst", \
    "Vlan", \
    "Vrf", \
    "FieldExactMatchLookUp0", \
    "FieldExactMatchLookUp1", \
    "FieldIngress", \
    "I2E", \
    "NextHopA", \
    "NextHopB", \
    "L3InterfaceA", \
    "L3InterfaceB", \
    "DstVp", \
    "IpTunnel", \
    "XlateNextHopA", \
    "XlateNextHopB", \
    "XlateL3InterfaceA", \
    "XlateL3InterfaceB", \
    "XlateDstVp", \
    "XlateIpTunnel", \
    "Count"  \
}

/* Flowtracker Aggregate Class information. */
typedef struct bcm_flowtracker_aggregate_class_info_s {
    bcm_flowtracker_class_source_type_t source_type1; 
    bcm_flowtracker_class_source_type_t source_type2; 
} bcm_flowtracker_aggregate_class_info_t;

/* 
 * Options that can be passed via options parameter during flowtracker
 * aggregate class creation.
 */
#define BCM_FLOWTRACKER_AGGREGATE_CLASS_WITH_ID (1 << 0)   /* Create flowtracker
                                                          aggregate class with
                                                          ID. */

/* Initialize flowtracker aggregate class info. */
extern void bcm_flowtracker_aggregate_class_info_t_init(
    bcm_flowtracker_aggregate_class_info_t *agg_class_info);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create flowtracker aggregate class for given group type. */
extern int bcm_flowtracker_aggregate_class_create(
    int unit, 
    uint32 options, 
    bcm_flowtracker_group_type_t group_type, 
    bcm_flowtracker_aggregate_class_info_t *aggregate_class_info, 
    uint32 *aggregate_class);

/* Get flowtracker aggregate class info. */
extern int bcm_flowtracker_aggregate_class_get(
    int unit, 
    uint32 aggregate_class, 
    bcm_flowtracker_group_type_t group_type, 
    bcm_flowtracker_aggregate_class_info_t *aggregate_class_info);

/* Get all flowtracker aggregate classes configured in the system. */
extern int bcm_flowtracker_aggregate_class_get_all(
    int unit, 
    bcm_flowtracker_group_type_t group_type, 
    int size, 
    uint32 *aggregate_class_array, 
    int *actual_size);

/* Destroy flowtracker aggregate class. */
extern int bcm_flowtracker_aggregate_class_destroy(
    int unit, 
    uint32 aggregate_class, 
    bcm_flowtracker_group_type_t group_type);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Flags for setting up periodic export interval */
#define BCM_FLOWTRACKER_TYPE_MICRO      (1 << 0)   /* Micro Flowtracker type */
#define BCM_FLOWTRACKER_TYPE_AGGREGATE  (1 << 1)   /* Aggregate Flowtracker type */

/* Flags for periodic export configurations */
#define BCM_FLOWTRACKER_MICRO_PERIODIC_EXPORT_GUARANTEE (1 << 0)   /* If this flag is set,
                                                          export of records due
                                                          to periodic export are
                                                          discarded if no
                                                          resources
                                                          are available to
                                                          export the record. */

/* Structure for Flowtracker periodic export config. */
typedef struct bcm_flowtracker_periodic_export_config_s {
    uint32 flags;           /* Flags. */
    uint32 interval_msec;   /* Periodic export interval in milliseconds. */
} bcm_flowtracker_periodic_export_config_t;

/* Initialize a export period config structure. */
extern void bcm_flowtracker_periodic_export_config_t_init(
    bcm_flowtracker_periodic_export_config_t *config);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set periodic export configuration */
extern int bcm_flowtracker_periodic_export_config_set(
    int unit, 
    uint32 ft_type_flags, 
    bcm_flowtracker_periodic_export_config_t *config);

/* Get periodic export configuration */
extern int bcm_flowtracker_periodic_export_config_get(
    int unit, 
    uint32 ft_type_flags, 
    bcm_flowtracker_periodic_export_config_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Types of exact match table indexing modes */
typedef enum bcm_flowtracker_exact_match_idx_mode_e {
    bcmFlowtrackerExactMatchIdxModeSingle = 0, /* Single wide entry */
    bcmFlowtrackerExactMatchIdxModeDouble = 1, /* Double wide entry */
    bcmFlowtrackerExactMatchIdxModeQuad = 2 /* Quad wide entry */
} bcm_flowtracker_exact_match_idx_mode_t;

/* 
 * Information required for performing a delete/modify action on a learnt
 * flow.
 */
typedef struct bcm_flowtracker_group_flow_action_info_s {
    uint8 pipe_idx;                     /* Pipe on which the flow was learnt. */
    bcm_flowtracker_exact_match_idx_mode_t mode; /* Exact match index mode. */
    uint32 exact_match_idx;             /* Exact match index of the learnt flow. */
} bcm_flowtracker_group_flow_action_info_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Delete a flow entry learnt by HW in exact match table. This API is
 * allowed only when flowtracker_hardware_learn_enable = 2.
 */
extern int bcm_flowtracker_group_flow_delete(
    int unit, 
    bcm_flowtracker_group_t id, 
    bcm_flowtracker_group_flow_action_info_t *action_info);

/* 
 * Modify the action data of a flow entry learnt by HW in exact match
 * table. Note that the action types should match whatever is configured
 * for the group already. This allows only changing the values of those
 * actions. This API is allowed only when
 * flowtracker_hardware_learn_enable = 2.
 */
extern int bcm_flowtracker_group_flow_action_update(
    int unit, 
    bcm_flowtracker_group_t id, 
    bcm_flowtracker_group_flow_action_info_t *action_info, 
    int num_actions, 
    bcm_flowtracker_group_action_info_t *action_list);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Enumerations for flowtracker global controls. */
typedef enum bcm_flowtracker_control_type_e {
    bcmFlowtrackerControlTcpBidirEstablishedDoNotLearn = 0, /* Do not learn flows which is already
                                           in an established state before being
                                           seen by BroadScan. */
    bcmFlowtrackerControlCount = 1      /* Not used. */
} bcm_flowtracker_control_type_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* To configure global controls in flowtracker */
extern int bcm_flowtracker_control_set(
    int unit, 
    bcm_flowtracker_control_type_t type, 
    int arg);

/* To get global controls in flowtracker */
extern int bcm_flowtracker_control_get(
    int unit, 
    bcm_flowtracker_control_type_t type, 
    int *arg);

/* To configure Default Egress Class Id source in flowtracker */
extern int bcm_flowtracker_egress_class_source_set(
    int unit, 
    uint32 options, 
    bcm_flowtracker_class_source_type_t source_type);

/* To get configured Default Egress Class Id source in flowtracker. */
extern int bcm_flowtracker_egress_class_source_get(
    int unit, 
    uint32 options, 
    bcm_flowtracker_class_source_type_t *source_type);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Flowtraker group record information contents. */
typedef enum bcm_flowtracker_group_record_type_e {
    bcmFlowtrackerGroupRecordTypeKeyOnly = 0, /* Flowtracker group record with Key
                                           only. */
    bcmFlowtrackerGroupRecordTypeKeyData = 1, /* Flowtracker group record with key and
                                           data. */
    bcmFlowtrackerGroupRecordTypeCount = 2 /* Last Value. Not Usable. */
} bcm_flowtracker_group_record_type_t;

/* Flowtracker Group record. */
typedef struct bcm_flowtracker_group_record_info_s {
    bcm_flowtracker_group_t group_id;   /* Flowtracker Group Id of the record. */
    uint32 index;                       /* Index of flowtracker group record. */
    bcm_flowtracker_group_record_type_t type; /* Flowtracker Group record valid
                                           content type */
    bcm_flowtracker_export_record_t record; /* Flowtracker Group record */
} bcm_flowtracker_group_record_info_t;

/* Initialize flowtracker group record information. */
extern void bcm_flowtracker_group_record_info_t_init(
    bcm_flowtracker_group_record_info_t *info);

/* User callback for flowtracker group record traverse routine. */
typedef int(*bcm_flowtracker_group_record_traverse_cb)(
    int unit, 
    bcm_flowtracker_group_record_info_t *info, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse flowtracker group records. */
extern int bcm_flowtracker_group_record_traverse(
    int unit, 
    bcm_flowtracker_group_t group_id, 
    bcm_flowtracker_group_record_type_t type, 
    bcm_flowtracker_group_record_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Max size of the data in export record. */
#define BCM_FLOWTRACKER_EXPORT_RECORD_DATA_MAX_LENGTH 16         /* Maximum length of a
                                                          data in record. */

/* Flowtracker Export record data information. */
typedef struct bcm_flowtracker_export_record_data_info_s {
    bcm_flowtracker_export_element_info_t export_element_info; /* Flowtracker Export element
                                           information. */
    uint8 data[BCM_FLOWTRACKER_EXPORT_RECORD_DATA_MAX_LENGTH]; /* Raw data in network byte order. */
} bcm_flowtracker_export_record_data_info_t;

/* Initialize flowtracker export record data information. */
extern void bcm_flowtracker_export_record_data_info_t_init(
    bcm_flowtracker_export_record_data_info_t *data_info);

#ifndef BCM_HIDE_DISPATCHABLE

/* To get data from raw export record. */
extern int bcm_flowtracker_export_record_data_info_get(
    int unit, 
    bcm_flowtracker_export_record_t *record, 
    int max_elements, 
    bcm_flowtracker_export_element_info_t *export_element_info, 
    bcm_flowtracker_export_record_data_info_t *data_info, 
    int *count);

#endif /* BCM_HIDE_DISPATCHABLE */

/* UDF Tracking param information. */
typedef struct bcm_flowtracker_udf_info_s {
    bcm_udf_id_t udf_id;    /* UDF object identifier */
    int offset;             /* Relative offset (in bits) in extracted UDF */
    int width;              /* Width of data in bits */
} bcm_flowtracker_udf_info_t;

/* Initialize flowtracker udf information. */
extern void bcm_flowtracker_udf_info_t_init(
    bcm_flowtracker_udf_info_t *info);

#ifndef BCM_HIDE_DISPATCHABLE

/* To set Flowtracker Tracking parameter UDF information. */
extern int bcm_flowtracker_udf_tracking_param_set(
    int unit, 
    uint32 flags, 
    bcm_flowtracker_tracking_param_type_t param, 
    uint16 custom_id, 
    bcm_flowtracker_udf_info_t *udf_info);

/* To get Flowtracker Tracking param UDF information. */
extern int bcm_flowtracker_udf_tracking_param_get(
    int unit, 
    uint32 flags, 
    bcm_flowtracker_tracking_param_type_t param, 
    uint16 custom_id, 
    bcm_flowtracker_udf_info_t *udf_info);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_FLOWTRACKER_H__ */
