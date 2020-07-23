/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_FLEXCTR_H__
#define __BCM_FLEXCTR_H__

#include <bcm/types.h>
#include <bcm/port.h>

/* 
 * Enumeration of flex counter sources.
 * 
 * Flex counter source indicates the sources to generates a flex counter
 * action.
 */
typedef enum bcm_flexctr_source_e {
    bcmFlexctrSourceIngPort = 0,        /* Ingress system port. */
    bcmFlexctrSourceEgrPort = 1,        /* Egress system port. */
    bcmFlexctrSourceIngVlan = 2,        /* Ingress VLAN. */
    bcmFlexctrSourceEgrVlan = 3,        /* Egress VLAN. */
    bcmFlexctrSourceIngVpn = 4,         /* Ingress VPN. */
    bcmFlexctrSourceEgrVpn = 5,         /* Egress VPN. */
    bcmFlexctrSourceSvp = 6,            /* Source VP. */
    bcmFlexctrSourceDvp = 7,            /* Destination VP. */
    bcmFlexctrSourceIngL3Intf = 8,      /* Ingress L3 interface. */
    bcmFlexctrSourceIngVrf = 9,         /* Ingress VRF. */
    bcmFlexctrSourceIfp = 10,           /* Ingress filter processor. */
    bcmFlexctrSourceVfp = 11,           /* Ingress VLAN filter processor. */
    bcmFlexctrSourceEfp = 12,           /* Egress filter processor. */
    bcmFlexctrSourceL3EgressOverlay = 13, /* Overlay L3 egress. */
    bcmFlexctrSourceL3EgressUnderlay = 14, /* Underlay L3 egress. */
    bcmFlexctrSourceExactMatch = 15,    /* ExactMatch filter processor. */
    bcmFlexctrSourceIpmc = 16,          /* Ipmc. */
    bcmFlexctrSourceFieldDestination = 17, /* Destination field. */
    bcmFlexctrSourceIngSamplePool = 18, /* Packets eligible to be sampled by
                                           ingress sampler. */
    bcmFlexctrSourceIngSampleCount = 19, /* Packets sampled by ingress sampler. */
    bcmFlexctrSourceFlexSamplePool = 20, /* Packets eligible to be sampled by
                                           flex sampler. */
    bcmFlexctrSourceFlexSampleCount = 21, /* Packets sampled by flex sampler. */
    bcmFlexctrSourceL3Route = 22,       /* L3 route. */
    bcmFlexctrSourceMplsLabel = 23,     /* MPLS termination. */
    bcmFlexctrSourceIpTunnelTerminator = 24, /* IP tunnel termination. */
    bcmFlexctrSourceEcmp = 25,          /* ECMP group. */
    bcmFlexctrSourceEgrL3Intf = 26,     /* Egress L3 interface. */
    bcmFlexctrSourceTrunk = 27,         /* TRUNK group. */
    bcmFlexctrSourceLatencyMonitor = 28, /* Latency monitor. */
    bcmFlexctrSourceEsFilter = 29,      /* Ethernet Segment filter. */
    bcmFlexctrSourceL2TunnelInitiator = 30, /* L2 tunnel initiator. */
    bcmFlexctrSourceL2TunnelVnidAssignment = 31, /* L2 tunnel VNID assignment. */
    bcmFlexctrSourceL2TunnelTerminator = 32, /* L2 tunnel termination. */
    bcmFlexctrSourceL2TunnelVpnAssignment = 33, /* L2 tunnel VPN assignment. */
    bcmFlexctrSourceSvpAssignment = 34, /* Source VP assignment. */
    bcmFlexctrSourceIngVlanTranslate = 35, /* Ingress VLAN translation. */
    bcmFlexctrSourceEgrVlanTranslate = 36, /* Egress VLAN translation. */
    bcmFlexctrSourceCount = 37          /* Always last one. */
} bcm_flexctr_source_t;

#define BCM_FLEXCTR_SOURCE_NAME_INITIALIZER \
{ \
    "bcmFlexctrSourceIngPort", \
    "bcmFlexctrSourceEgrPort", \
    "bcmFlexctrSourceIngVlan", \
    "bcmFlexctrSourceEgrVlan", \
    "bcmFlexctrSourceIngVpn", \
    "bcmFlexctrSourceEgrVpn", \
    "bcmFlexctrSourceSvp", \
    "bcmFlexctrSourceDvp", \
    "bcmFlexctrSourceIngL3Intf", \
    "bcmFlexctrSourceIngVrf", \
    "bcmFlexctrSourceIfp", \
    "bcmFlexctrSourceVfp", \
    "bcmFlexctrSourceEfp", \
    "bcmFlexctrSourceL3EgressOverlay", \
    "bcmFlexctrSourceL3EgressUnderlay", \
    "bcmFlexctrSourceExactMatch", \
    "bcmFlexctrSourceIpmc", \
    "bcmFlexctrSourceFieldDestination", \
    "bcmFlexctrSourceIngSamplePool", \
    "bcmFlexctrSourceIngSampleCount", \
    "bcmFlexctrSourceFlexSamplePool", \
    "bcmFlexctrSourceFlexSampleCount", \
    "bcmFlexctrSourceL3Route", \
    "bcmFlexctrSourceMplsLabel", \
    "bcmFlexctrSourceIpTunnelTerminator", \
    "bcmFlexctrSourceEcmp", \
    "bcmFlexctrSourceEgrL3Intf", \
    "bcmFlexctrSourceTrunk", \
    "bcmFlexctrSourceLatencyMonitor", \
    "bcmFlexctrSourceEsFilter", \
    "bcmFlexctrSourceL2TunnelInitiator", \
    "bcmFlexctrSourceL2TunnelVnidAssignment", \
    "bcmFlexctrSourceL2TunnelTerminator", \
    "bcmFlexctrSourceL2TunnelVpnAssignment", \
    "bcmFlexctrSourceSvpAssignment", \
    "bcmFlexctrSourceIngVlanTranslate", \
    "bcmFlexctrSourceEgrVlanTranslate", \
    "bcmFlexctrSourceCount"  \
}

/* 
 * Enumeration of flex counter objects.
 * 
 * Flex counter object could be used to generate counter index and
 * counter value.
 * Some objects could also be used for conditional trigger and
 * quantization.
 */
typedef enum bcm_flexctr_object_e {
    bcmFlexctrObjectConstZero = 0,      /* Const value 0 object. */
    bcmFlexctrObjectConstOne = 1,       /* Const value 1 object. */
    bcmFlexctrObjectTriggerInterval = 2, /* Trigger interval object. */
    bcmFlexctrObjectQuant = 3,          /* Quantization result object. */
    bcmFlexctrObjectStaticIngFieldStageLookup = 4, /* Ingress: VFP object. */
    bcmFlexctrObjectStaticIngFieldStageIngress = 5, /* Ingress: IFP object. */
    bcmFlexctrObjectStaticIngExactMatch = 6, /* Ingress: exact match FP Object. */
    bcmFlexctrObjectStaticIngMplsSingleTable = 7, /* Ingress: MPLS single table object. */
    bcmFlexctrObjectStaticIngMplsDoubleTable = 8, /* Ingress: MPLS double table object. */
    bcmFlexctrObjectStaticIngL3Tunnel = 9, /* Ingress: L3 tunnel object. */
    bcmFlexctrObjectStaticIngFlowCountPrefixLength = 10, /* Ingress: flow count prefix length
                                           object. */
    bcmFlexctrObjectStaticIngFlowCountPortContext = 11, /* Ingress: flow count port context
                                           object. */
    bcmFlexctrObjectStaticIngFlowCountFlowSizeLower = 12, /* Ingress: flow count flow size lower
                                           byte object. */
    bcmFlexctrObjectStaticIngFlowCountFlowSizeUpper = 13, /* Ingress: flow count flow size upper
                                           byte object. */
    bcmFlexctrObjectStaticIngAlpmSrcLookup = 14, /* Ingress: ALPM source lookup object. */
    bcmFlexctrObjectStaticIngAlpmDstLookup = 15, /* Ingress: ALPM destination lookup
                                           object. */
    bcmFlexctrObjectStaticIngLookupHitStatus = 16, /* Ingress: lookup hit status object. */
    bcmFlexctrObjectStaticIngIngressPort = 17, /* Ingress: ingress port object. */
    bcmFlexctrObjectStaticIngEgressPort = 18, /* Ingress: egress port object. */
    bcmFlexctrObjectStaticIngVlan = 19, /* Ingress: VLAN object. */
    bcmFlexctrObjectStaticIngL3Intf = 20, /* Ingress: L3 ingress interface
                                           (L3_IIF) object. */
    bcmFlexctrObjectStaticIngVrf = 21,  /* Ingress: VRF object. */
    bcmFlexctrObjectStaticIngPortClassId = 22, /* Ingress: port class ID object. */
    bcmFlexctrObjectStaticIngEcmpGroup = 23, /* Ingress: ECMP group object. */
    bcmFlexctrObjectStaticIngEcmpMember = 24, /* Ingress: ECMP member object. */
    bcmFlexctrObjectStaticIngNextHopOrMcIndex = 25, /* Ingress: next hop or multicast index
                                           object. */
    bcmFlexctrObjectStaticIngRtag7Hash0Instance0 = 26, /* Ingress: RTAG7 subselection hash 0
                                           and instance 0 object. */
    bcmFlexctrObjectStaticIngRtag7Hash0Instance1 = 27, /* Ingress: RTAG7 subselection hash 0
                                           and instance 1 object. */
    bcmFlexctrObjectStaticIngRtag7Hash1Instance0 = 28, /* Ingress: RTAG7 subselection hash 1
                                           and instance 0 object. */
    bcmFlexctrObjectStaticIngRtag7Hash1Instance1 = 29, /* Ingress: RTAG7 subselection hash 1
                                           and instance 1 object. */
    bcmFlexctrObjectStaticIngIngressTimestampBitRange0 = 30, /* Ingress: ingress timestamp object for
                                           bit 0 to 15. */
    bcmFlexctrObjectStaticIngIngressTimestampBitRange1 = 31, /* Ingress: ingress timestamp object for
                                           bit 16 to 31. */
    bcmFlexctrObjectStaticIngIngressTimestampBitRange2 = 32, /* Ingress: ingress timestamp object for
                                           bit 32 to 47. */
    bcmFlexctrObjectStaticIngIpTotalLength = 33, /* Ingress: packet length in IP header
                                           object. */
    bcmFlexctrObjectStaticIngL4Flags = 34, /* Ingress: L4 flags object. */
    bcmFlexctrObjectStaticIngUdf1 = 35, /* Ingress: UDF1 object. */
    bcmFlexctrObjectStaticIngUdf2 = 36, /* Ingress: UDF2 object. */
    bcmFlexctrObjectStaticIngIntPri = 37, /* Ingress: internal priority object. */
    bcmFlexctrObjectStaticIngIntCn = 38, /* Ingress: internal congestion
                                           notification object. */
    bcmFlexctrObjectStaticIngModEvent = 39, /* Ingress: mirror-on-drop event object. */
    bcmFlexctrObjectStaticIngTrunkGroup = 40, /* Ingress: trunk group object. */
    bcmFlexctrObjectStaticIngTrunkMember = 41, /* Ingress: trunk member object. */
    bcmFlexctrObjectStaticIngIpProtocol = 42, /* Ingress: protocol field in IP header
                                           object. */
    bcmFlexctrObjectStaticEgrAdapt = 43, /* Egress: egress adapt table object. */
    bcmFlexctrObjectStaticEgrFieldStageEgress = 44, /* Egress: EFP object. */
    bcmFlexctrObjectStaticEgrIngressPort = 45, /* Egress: ingress port object. */
    bcmFlexctrObjectStaticEgrEgressPort = 46, /* Egress: egress port object. */
    bcmFlexctrObjectStaticEgrVlan = 47, /* Egress: VLAN object. */
    bcmFlexctrObjectStaticEgrVrf = 48,  /* Egress: VRF object. */
    bcmFlexctrObjectStaticEgrL3Oif = 49, /* Egress: L3 outgoing interface object. */
    bcmFlexctrObjectStaticEgrL3TunnelIndex = 50, /* Egress: L3 tunnel index object. */
    bcmFlexctrObjectStaticEgrL3NextHop = 51, /* Egress: L3 next hop object. */
    bcmFlexctrObjectStaticEgrL3NextHopMetadata = 52, /* Egress: L3 next hop metadata object. */
    bcmFlexctrObjectStaticEgrI2eClassId = 53, /* Egress: I2E class ID object. */
    bcmFlexctrObjectStaticEgrI2eClassIdType = 54, /* Egress: I2E class ID type object. */
    bcmFlexctrObjectStaticEgrQueueNum = 55, /* Egress: queue number object. */
    bcmFlexctrObjectStaticEgrQueueOccupancy = 56, /* Egress: queue occupancy object. */
    bcmFlexctrObjectStaticEgrResidenceTimestampLower = 57, /* Egress: residency timestamp lower
                                           object. */
    bcmFlexctrObjectStaticEgrResidenceTimestampUpper = 58, /* Egress: residency timestamp upper
                                           object. */
    bcmFlexctrObjectStaticEgrEgressTimestampBitRange0 = 59, /* Egress: egress timestamp object for
                                           bit 0 to 15. */
    bcmFlexctrObjectStaticEgrEgressTimestampBitRange1 = 60, /* Egress: egress timestamp object for
                                           bit 16 to 31. */
    bcmFlexctrObjectStaticEgrEgressTimestampBitRange2 = 61, /* Egress: egress timestamp object for
                                           bit 32 to 47. */
    bcmFlexctrObjectStaticEgrIngressTimestampBitRange0 = 62, /* Egress: ingress timestamp object for
                                           bit 0 to 15. */
    bcmFlexctrObjectStaticEgrIngressTimestampBitRange1 = 63, /* Egress: ingress timestamp object for
                                           bit 16 to 31. */
    bcmFlexctrObjectStaticEgrIngressTimestampBitRange2 = 64, /* Egress: ingress timestamp object for
                                           bit 32 to 47. */
    bcmFlexctrObjectStaticEgrEntropyLabelBitRange0 = 65, /* Egress: entropy label object for bit
                                           0 to 15. */
    bcmFlexctrObjectStaticEgrEntropyLabelBitRange1 = 66, /* Egress: entropy label object for bit
                                           16 to 25. */
    bcmFlexctrObjectStaticEgrModEvent = 67, /* Egress: mirror-on-drop event object. */
    bcmFlexctrObjectStaticEgrIntPri = 68, /* Egress: internal priority object. */
    bcmFlexctrObjectStaticEgrIntCn = 69, /* Egress: internal congestion
                                           notification object. */
    bcmFlexctrObjectStaticEgrCng = 70,  /* Egress: congestion color object. */
    bcmFlexctrObjectStaticEgrPortAndQueueNum = 71, /* Egress: port and queue number object. */
    bcmFlexctrObjectStaticIngPktAttribute = 72, /* Ingress: packet attribute offset
                                           object. */
    bcmFlexctrObjectStaticEgrPktAttribute = 73, /* Egress: packet attribute offset
                                           object. */
    bcmFlexctrObjectStaticIngVfi = 74,  /* Ingress: VFI (virtual forwarding
                                           instance). */
    bcmFlexctrObjectStaticIngDvp = 75,  /* Ingress: destination VP (virtual
                                           port). */
    bcmFlexctrObjectStaticIngSvp = 76,  /* Ingress: source VP (virtual port). */
    bcmFlexctrObjectStaticIngOverlayEcmpGroup = 77, /* Ingress: overlay ECMP group object. */
    bcmFlexctrObjectStaticIngOverlayEcmpMember = 78, /* Ingress: overlay ECMP member object. */
    bcmFlexctrObjectStaticIngOverlayL3NextHop = 79, /* Ingress: overlay L3 next hop object. */
    bcmFlexctrObjectStaticEgrDvp = 80,  /* Egress: destination VP (virtual
                                           port). */
    bcmFlexctrObjectStaticEgrSvp = 81,  /* Egress: source VP (virtual port). */
    bcmFlexctrObjectStaticEgrVfi = 82,  /* Egress: VFI (virtual forwarding
                                           instance). */
    bcmFlexctrObjectStaticEgrNetworkGroupSvp = 83, /* Egress: network group for source VP
                                           (virtual port). */
    bcmFlexctrObjectStaticEgrNetworkGroupDvp = 84, /* Egress: network group for destination
                                           VP (virtual port). */
    bcmFlexctrObjectStaticEgrOverlayL3NextHop = 85, /* Egress: overlay L3 next hop object. */
    bcmFlexctrObjectStaticEgrOverlayL3Oif = 86, /* Egress: overlay L3 outgoing interface
                                           object. */
    bcmFlexctrObjectStaticCount = 87,   /* Always last for static objects. Not a
                                           usable value. */
    bcmFlexctrObjectIngSystemSource = 1000, /* Ingress system source object. */
    bcmFlexctrObjectIngIngressPpPort = 1001, /* Ingress pp port object. */
    bcmFlexctrObjectIngL2Iif = 1002,    /* Ingress l2 iif object. */
    bcmFlexctrObjectIngL3Iif = 1003,    /* Ingress l3 iif object. */
    bcmFlexctrObjectIngL2SrcLookupHitIndex0 = 1004, /* Ingress l2 src lookup hit index 0
                                           object. */
    bcmFlexctrObjectIngL2SrcLookupHitIndex1 = 1005, /* Ingress l2 src lookup hit index 1
                                           object. */
    bcmFlexctrObjectIngL2DstLookupHitIndex0 = 1006, /* Ingress l2 dst lookup hit index 0
                                           object. */
    bcmFlexctrObjectIngL2DstLookupHitIndex1 = 1007, /* Ingress l2 dst lookup hit index 1
                                           object. */
    bcmFlexctrObjectIngSvp = 1008,      /* Ingress svp object. */
    bcmFlexctrObjectIngVfi = 1009,      /* Ingress vfi object. */
    bcmFlexctrObjectIngDstFpFlexCtrIndex = 1010, /* Ingress dst fp flex ctr index object. */
    bcmFlexctrObjectIngIfpOpaqueObj1 = 1011, /* Ingress ifp opaque obj 1 object. */
    bcmFlexctrObjectIngIfpOpaqueObj0 = 1012, /* Ingress ifp opaque obj 0 object. */
    bcmFlexctrObjectEgrVfi = 1013,      /* Egress vfi object. */
    bcmFlexctrObjectEgrDvp = 1014,      /* Egress dvp object. */
    bcmFlexctrObjectEgrL2Oif = 1015,    /* Egress l2 oif object. */
    bcmFlexctrObjectEgrL3Oif1 = 1016,   /* Egress l3 oif 1 object. */
    bcmFlexctrObjectEgrTxPpPort = 1017, /* Egress tx pp port object. */
    bcmFlexctrObjectEgrL3Oif2 = 1018,   /* Egress l3 oif 2 object. */
    bcmFlexctrObjectEgrL3NextHop2FlexCtrIndex = 1019, /* Egress l3 next hop 2 flex ctr index
                                           object. */
    bcmFlexctrObjectEgrL3NextHop1FlexCtrIndex = 1020, /* Egress l3 next hop 1 flex ctr index
                                           object. */
    bcmFlexctrObjectEgrPipePortQueueNum = 1021, /* Egress pipe port queue num object. */
    bcmFlexctrObjectEgrEfpOpaqueObj2 = 1022, /* Egress efp opaque obj 2 object. */
    bcmFlexctrObjectEgrEfpOpaqueObj1 = 1023, /* Egress efp opaque obj 1 object. */
    bcmFlexctrObjectEgrResidenceTimeShifted0 = 1024, /* Egress residence time shifted 0
                                           object. */
    bcmFlexctrObjectEgrResidenceTimeShifted = 1025, /* Egress residence time shifted object. */
    bcmFlexctrObjectIngVfpOpaqueObj0 = 1026, /* Ingress vfp opaque obj 0 object. */
    bcmFlexctrObjectIngNhop2OrEcmpGroupIndex1 = 1027, /* Ingress nhop 2 or ecmp group index 1
                                           object. */
    bcmFlexctrObjectIngNhopIndex1 = 1028, /* Ingress nhop index 1 object. */
    bcmFlexctrObjectIngEmFtOpaqueObj1 = 1029, /* Ingress em ft opaque obj 1 object. */
    bcmFlexctrObjectIngEmFtOpaqueObj0 = 1030, /* Ingress em ft opaque obj 0 object. */
    bcmFlexctrObjectEgrEcnMapInputVector = 1031, /* Egress ecn map input vector object. */
    bcmFlexctrObjectIngEmFtHitIndex0 = 1032, /* Ingress em ft hit index 0 object. */
    bcmFlexctrObjectIngEmFtHitIndex1 = 1033, /* Ingress em ft hit index 1 object. */
    bcmFlexctrObjectIngL3DstClassId = 1034, /* Ingress l3 dst class id object. */
    bcmFlexctrObjectEgrResidenceTimeShifted1 = 1035, /* Egress residence time shifted 1
                                           object. */
    bcmFlexctrObjectEgrEcnMapInputVector0 = 1036, /* Egress ecn map input vector 0 object. */
    bcmFlexctrObjectIngIntPri = 1037,   /* Ingress int pri object. */
    bcmFlexctrObjectIngIfpOpaqueObj2 = 1038, /* Ingress ifp opaque obj 2 object. */
    bcmFlexctrObjectEgrIntcnIntpriCng0 = 1039, /* Egress intcn intpri cng 0 object. */
    bcmFlexctrObjectEgrIntcnIntpriCng = 1040, /* Egress intcn intpri cng object. */
    bcmFlexctrObjectEgrL3Oif216VfiClassid = 1041, /* Egress l3 oif 2 16 vfi classid
                                           object. */
    bcmFlexctrObjectIngIngressQoSRemapValue = 1042, /* Ingress QoS Remap value object. */
    bcmFlexctrObjectIngFlexDigestA0 = 1043, /* Ingress flex digest a0 object. */
    bcmFlexctrObjectIngFlexDigestA1 = 1044, /* Ingress flex digest a1 object. */
    bcmFlexctrObjectIngFlexDigestB0 = 1045, /* Ingress flex digest b0 object. */
    bcmFlexctrObjectIngFlexDigestB1 = 1046, /* Ingress flex digest b1 object. */
    bcmFlexctrObjectIngFlexDigestC0 = 1047, /* Ingress flex digest c0 object. */
    bcmFlexctrObjectIngFlexDigestC1 = 1048, /* Ingress flex digest c1 object. */
    bcmFlexctrObjectIngIpHdrTotalLength = 1049, /* Ingress ip header total length
                                           object. */
    bcmFlexctrObjectIngI2eClassId = 1050, /* Ingress i2e class id object. */
    bcmFlexctrObjectIngVlanXlate = 1051, /* Ingress vlan xlate object. */
    bcmFlexctrObjectIngIfpLagDgmActionSet = 1052, /* Ingress ifp lag dgm action set
                                           object. */
    bcmFlexctrObjectIngIfpSamplerIndex = 1053, /* Ingress ifp sampler index object. */
    bcmFlexctrObjectIngEmFtOpaqueObj = 1054, /* Ingress em ft opaque obj object. */
    bcmFlexctrObjectEgrEvxltFlexCtrIndex = 1055, /* Egress evxlt flex ctr index object. */
    bcmFlexctrObjectIngIfpOpaqueObj1_0 = 1056, /* Ingress ifp opaque obj 1 0 object. */
    bcmFlexctrObjectIngIfpOpaqueObj1_1 = 1057, /* Ingress ifp opaque obj 1 1 object. */
    bcmFlexctrObjectIngIfpOpaqueObj1_2 = 1058, /* Ingress ifp opaque obj 1 2 object. */
    bcmFlexctrObjectIngIfpOpaqueObj1_3 = 1059, /* Ingress ifp opaque obj 1 3 object. */
    bcmFlexctrObjectIngIfpOpaqueObj3 = 1060, /* Ingress ifp opaque obj 3 object. */
    bcmFlexctrObjectIngIfpOpaqueObj4 = 1061, /* Ingress ifp opaque obj 4 object. */
    bcmFlexctrObjectIngIfpOpaqueObj5 = 1062, /* Ingress ifp opaque obj 5 object. */
    bcmFlexctrObjectIngIfpOpaqueObj6 = 1063, /* Ingress ifp opaque obj 6 object. */
    bcmFlexctrObjectIngIfpOpaqueObj7 = 1064, /* Ingress ifp opaque obj 7 object. */
    bcmFlexctrObjectIngIfpOpaqueObj8 = 1065, /* Ingress ifp opaque obj 8 object. */
    bcmFlexctrObjectIngIfpOpaqueObj9 = 1066, /* Ingress ifp opaque obj 9 object. */
    bcmFlexctrObjectIngVfpOpaqueObj1 = 1067, /* Ingress vfp opaque obj 1 object. */
    bcmFlexctrObjectIngEpNihHdrDropCode = 1068, /* Ingress EP NIH header drop code
                                           object. */
    bcmFlexctrObjectEgrEpRecircProfile = 1069, /* Egress EP Recirculate profile object. */
    bcmFlexctrObjectCount = 1070        /* Always last one. */
} bcm_flexctr_object_t;

#define BCM_FLEXCTR_OBJECT_NAME_INITIALIZER \
{ \
    "bcmFlexctrObjectConstZero", \
    "bcmFlexctrObjectConstOne", \
    "bcmFlexctrObjectTriggerInterval", \
    "bcmFlexctrObjectQuant", \
    "bcmFlexctrObjectStaticIngFieldStageLookup", \
    "bcmFlexctrObjectStaticIngFieldStageIngress", \
    "bcmFlexctrObjectStaticIngExactMatch", \
    "bcmFlexctrObjectStaticIngMplsSingleTable", \
    "bcmFlexctrObjectStaticIngMplsDoubleTable", \
    "bcmFlexctrObjectStaticIngL3Tunnel", \
    "bcmFlexctrObjectStaticIngFlowCountPrefixLength", \
    "bcmFlexctrObjectStaticIngFlowCountPortContext", \
    "bcmFlexctrObjectStaticIngFlowCountFlowSizeLower", \
    "bcmFlexctrObjectStaticIngFlowCountFlowSizeUpper", \
    "bcmFlexctrObjectStaticIngAlpmSrcLookup", \
    "bcmFlexctrObjectStaticIngAlpmDstLookup", \
    "bcmFlexctrObjectStaticIngLookupHitStatus", \
    "bcmFlexctrObjectStaticIngIngressPort", \
    "bcmFlexctrObjectStaticIngEgressPort", \
    "bcmFlexctrObjectStaticIngVlan", \
    "bcmFlexctrObjectStaticIngL3Intf", \
    "bcmFlexctrObjectStaticIngVrf", \
    "bcmFlexctrObjectStaticIngPortClassId", \
    "bcmFlexctrObjectStaticIngEcmpGroup", \
    "bcmFlexctrObjectStaticIngEcmpMember", \
    "bcmFlexctrObjectStaticIngNextHopOrMcIndex", \
    "bcmFlexctrObjectStaticIngRtag7Hash0Instance0", \
    "bcmFlexctrObjectStaticIngRtag7Hash0Instance1", \
    "bcmFlexctrObjectStaticIngRtag7Hash1Instance0", \
    "bcmFlexctrObjectStaticIngRtag7Hash1Instance1", \
    "bcmFlexctrObjectStaticIngIngressTimestampBitRange0", \
    "bcmFlexctrObjectStaticIngIngressTimestampBitRange1", \
    "bcmFlexctrObjectStaticIngIngressTimestampBitRange2", \
    "bcmFlexctrObjectStaticIngIpTotalLength", \
    "bcmFlexctrObjectStaticIngL4Flags", \
    "bcmFlexctrObjectStaticIngUdf1", \
    "bcmFlexctrObjectStaticIngUdf2", \
    "bcmFlexctrObjectStaticIngIntPri", \
    "bcmFlexctrObjectStaticIngIntCn", \
    "bcmFlexctrObjectStaticIngModEvent", \
    "bcmFlexctrObjectStaticIngTrunkGroup", \
    "bcmFlexctrObjectStaticIngTrunkMember", \
    "bcmFlexctrObjectStaticIngIpProtocol", \
    "bcmFlexctrObjectStaticEgrAdapt", \
    "bcmFlexctrObjectStaticEgrFieldStageEgress", \
    "bcmFlexctrObjectStaticEgrIngressPort", \
    "bcmFlexctrObjectStaticEgrEgressPort", \
    "bcmFlexctrObjectStaticEgrVlan", \
    "bcmFlexctrObjectStaticEgrVrf", \
    "bcmFlexctrObjectStaticEgrL3Oif", \
    "bcmFlexctrObjectStaticEgrL3TunnelIndex", \
    "bcmFlexctrObjectStaticEgrL3NextHop", \
    "bcmFlexctrObjectStaticEgrL3NextHopMetadata", \
    "bcmFlexctrObjectStaticEgrI2eClassId", \
    "bcmFlexctrObjectStaticEgrI2eClassIdType", \
    "bcmFlexctrObjectStaticEgrQueueNum", \
    "bcmFlexctrObjectStaticEgrQueueOccupancy", \
    "bcmFlexctrObjectStaticEgrResidenceTimestampLower", \
    "bcmFlexctrObjectStaticEgrResidenceTimestampUpper", \
    "bcmFlexctrObjectStaticEgrEgressTimestampBitRange0", \
    "bcmFlexctrObjectStaticEgrEgressTimestampBitRange1", \
    "bcmFlexctrObjectStaticEgrEgressTimestampBitRange2", \
    "bcmFlexctrObjectStaticEgrIngressTimestampBitRange0", \
    "bcmFlexctrObjectStaticEgrIngressTimestampBitRange1", \
    "bcmFlexctrObjectStaticEgrIngressTimestampBitRange2", \
    "bcmFlexctrObjectStaticEgrEntropyLabelBitRange0", \
    "bcmFlexctrObjectStaticEgrEntropyLabelBitRange1", \
    "bcmFlexctrObjectStaticEgrModEvent", \
    "bcmFlexctrObjectStaticEgrIntPri", \
    "bcmFlexctrObjectStaticEgrIntCn", \
    "bcmFlexctrObjectStaticEgrCng", \
    "bcmFlexctrObjectStaticEgrPortAndQueueNum", \
    "bcmFlexctrObjectStaticIngPktAttribute", \
    "bcmFlexctrObjectStaticEgrPktAttribute", \
    "bcmFlexctrObjectStaticIngVfi", \
    "bcmFlexctrObjectStaticIngDvp", \
    "bcmFlexctrObjectStaticIngSvp", \
    "bcmFlexctrObjectStaticIngOverlayEcmpGroup", \
    "bcmFlexctrObjectStaticIngOverlayEcmpMember", \
    "bcmFlexctrObjectStaticIngOverlayL3NextHop", \
    "bcmFlexctrObjectStaticEgrDvp", \
    "bcmFlexctrObjectStaticEgrSvp", \
    "bcmFlexctrObjectStaticEgrVfi", \
    "bcmFlexctrObjectStaticEgrNetworkGroupSvp", \
    "bcmFlexctrObjectStaticEgrNetworkGroupDvp", \
    "bcmFlexctrObjectStaticEgrOverlayL3NextHop", \
    "bcmFlexctrObjectStaticEgrOverlayL3Oif", \
    "bcmFlexctrObjectStaticCount", \
    "bcmFlexctrObjectIngSystemSource", \
    "bcmFlexctrObjectIngIngressPpPort", \
    "bcmFlexctrObjectIngL2Iif", \
    "bcmFlexctrObjectIngL3Iif", \
    "bcmFlexctrObjectIngL2SrcLookupHitIndex0", \
    "bcmFlexctrObjectIngL2SrcLookupHitIndex1", \
    "bcmFlexctrObjectIngL2DstLookupHitIndex0", \
    "bcmFlexctrObjectIngL2DstLookupHitIndex1", \
    "bcmFlexctrObjectIngSvp", \
    "bcmFlexctrObjectIngVfi", \
    "bcmFlexctrObjectIngDstFpFlexCtrIndex", \
    "bcmFlexctrObjectIngIfpOpaqueObj1", \
    "bcmFlexctrObjectIngIfpOpaqueObj0", \
    "bcmFlexctrObjectEgrVfi", \
    "bcmFlexctrObjectEgrDvp", \
    "bcmFlexctrObjectEgrL2Oif", \
    "bcmFlexctrObjectEgrL3Oif1", \
    "bcmFlexctrObjectEgrTxPpPort", \
    "bcmFlexctrObjectEgrL3Oif2", \
    "bcmFlexctrObjectEgrL3NextHop2FlexCtrIndex", \
    "bcmFlexctrObjectEgrL3NextHop1FlexCtrIndex", \
    "bcmFlexctrObjectEgrPipePortQueueNum", \
    "bcmFlexctrObjectEgrEfpOpaqueObj2", \
    "bcmFlexctrObjectEgrEfpOpaqueObj1", \
    "bcmFlexctrObjectEgrResidenceTimeShifted0", \
    "bcmFlexctrObjectEgrResidenceTimeShifted", \
    "bcmFlexctrObjectIngVfpOpaqueObj0", \
    "bcmFlexctrObjectIngNhop2OrEcmpGroupIndex1", \
    "bcmFlexctrObjectIngNhopIndex1", \
    "bcmFlexctrObjectIngEmFtOpaqueObj1", \
    "bcmFlexctrObjectIngEmFtOpaqueObj0", \
    "bcmFlexctrObjectEgrEcnMapInputVector", \
    "bcmFlexctrObjectIngEmFtHitIndex0", \
    "bcmFlexctrObjectIngEmFtHitIndex1", \
    "bcmFlexctrObjectIngL3DstClassId", \
    "bcmFlexctrObjectEgrResidenceTimeShifted1", \
    "bcmFlexctrObjectEgrEcnMapInputVector0", \
    "bcmFlexctrObjectIngIntPri", \
    "bcmFlexctrObjectIngIfpOpaqueObj2", \
    "bcmFlexctrObjectEgrIntcnIntpriCng0", \
    "bcmFlexctrObjectEgrIntcnIntpriCng", \
    "bcmFlexctrObjectEgrL3Oif216VfiClassid", \
    "bcmFlexctrObjectIngIngressQoSRemapValue", \
    "bcmFlexctrObjectIngFlexDigestA0", \
    "bcmFlexctrObjectIngFlexDigestA1", \
    "bcmFlexctrObjectIngFlexDigestB0", \
    "bcmFlexctrObjectIngFlexDigestB1", \
    "bcmFlexctrObjectIngFlexDigestC0", \
    "bcmFlexctrObjectIngFlexDigestC1", \
    "bcmFlexctrObjectIngIpHdrTotalLength", \
    "bcmFlexctrObjectIngI2eClassId", \
    "bcmFlexctrObjectIngVlanXlate", \
    "bcmFlexctrObjectIngIfpLagDgmActionSet", \
    "bcmFlexctrObjectIngIfpSamplerIndex", \
    "bcmFlexctrObjectIngEmFtOpaqueObj", \
    "bcmFlexctrObjectEgrEvxltFlexCtrIndex", \
    "bcmFlexctrObjectIngIfpOpaqueObj1_0", \
    "bcmFlexctrObjectIngIfpOpaqueObj1_1", \
    "bcmFlexctrObjectIngIfpOpaqueObj1_2", \
    "bcmFlexctrObjectIngIfpOpaqueObj1_3", \
    "bcmFlexctrObjectIngIfpOpaqueObj3", \
    "bcmFlexctrObjectIngIfpOpaqueObj4", \
    "bcmFlexctrObjectIngIfpOpaqueObj5", \
    "bcmFlexctrObjectIngIfpOpaqueObj6", \
    "bcmFlexctrObjectIngIfpOpaqueObj7", \
    "bcmFlexctrObjectIngIfpOpaqueObj8", \
    "bcmFlexctrObjectIngIfpOpaqueObj9", \
    "bcmFlexctrObjectIngVfpOpaqueObj1", \
    "bcmFlexctrObjectIngEpNihHdrDropCode", \
    "bcmFlexctrObjectEgrEpRecircProfile", \
    "bcmFlexctrObjectCount"  \
}

#define BCM_FLEXCTR_COUNTER_VALUE_SIZE  2          /* Counter array size number. */

/* 
 * Flex counter statistic value.
 * 
 * Normal mode:
 * value[0] is the data of counter value A.
 * value[1] is the data of counter value B.
 * Wide mode:
 * value[0] is the upper part data of counter value A.
 * value[1] is the lower part data of counter value A.
 * Slim mode:
 * value[0] is the data of counter value A.
 */
typedef struct bcm_flexctr_counter_value_s {
    uint64 value[BCM_FLEXCTR_COUNTER_VALUE_SIZE]; /* Flex counter statistic value array. */
} bcm_flexctr_counter_value_t;

/* Maximum flex counter operation object size. */
#define BCM_FLEXCTR_OPERATION_OBJECT_SIZE   8          /* Operation object array
                                                          size number. */

/* 
 * Flex counter index operation structure.
 * 
 * This data structure lists the operation of a flex counter index.
 * Number of supported elements for these member arrays and how the
 * index is generated from these elements may vary depending on device.
 * Flex counter index generated from two elements (if supported):
 *      value0 = (SEL(object0) >> shift0) & ((1 << mask_size0) - 1)).
 *      value1 = (SEL(object1) & ((1 << mask_size1) - 1)) << shift1.
 *      index = (value1 | value0).
 * Flex counter index generated from three elements (if supported):
 *      value0 = (SEL(object0) >> shift0) & ((1 << mask_size0) - 1)).
 *      value1 = ((SEL(object1) >> shift2) & ((1 << mask_size1) - 1)) <<
 * shift1.
 *      value2 = ((SEL(object2) >> shift4) & ((1 << mask_size2) - 1)) <<
 * shift3.
 *      index = (value2 | value1 | value0).
 */
typedef struct bcm_flexctr_index_operation_s {
    bcm_flexctr_object_t object[BCM_FLEXCTR_OPERATION_OBJECT_SIZE]; /* Flex counter object array. */
    uint32 quant_id[BCM_FLEXCTR_OPERATION_OBJECT_SIZE]; /* Quantization ID. */
    uint8 mask_size[BCM_FLEXCTR_OPERATION_OBJECT_SIZE]; /* Object mask size array. */
    uint8 shift[BCM_FLEXCTR_OPERATION_OBJECT_SIZE]; /* Object shift array. */
    uint32 object_id[BCM_FLEXCTR_OPERATION_OBJECT_SIZE]; /* Flex counter object ID. */
} bcm_flexctr_index_operation_t;

/* Enumeration of flex counter value select. */
typedef enum bcm_flexctr_value_select_e {
    bcmFlexctrValueSelectCounterValue = 0, /* Select counter value generated by
                                           operation to update counter. */
    bcmFlexctrValueSelectPacketLength = 1, /* Select packet length to update
                                           counter. */
    bcmFlexctrValueSelectPairedCounter = 2, /* Select paired counter value to update
                                           counter. */
    bcmFlexctrValueSelectCount = 3      /* Always last one. */
} bcm_flexctr_value_select_t;

/* Enumeration of flex counter value operation types. */
typedef enum bcm_flexctr_value_operation_type_e {
    bcmFlexctrValueOperationTypeNoop = 0, /* COUNTER = OLD_COUNTER. */
    bcmFlexctrValueOperationTypeSet = 1, /* COUNTER = CVALUE. */
    bcmFlexctrValueOperationTypeInc = 2, /* COUNTER = OLD_COUNTER + CVALUE. */
    bcmFlexctrValueOperationTypeDec = 3, /* COUNTER = OLD_COUNTER - CVALUE. */
    bcmFlexctrValueOperationTypeMax = 4, /* COUNTER = MAX(OLD_COUNTER, CVALUE). */
    bcmFlexctrValueOperationTypeMin = 5, /* COUNTER = MIN(OLD_COUNTER, CVALUE). */
    bcmFlexctrValueOperationTypeAverage = 6, /* COUNTER = AVERAGE(OLD_COUNTER,
                                           CVALUE). */
    bcmFlexctrValueOperationTypeSetBit = 7, /* COUNTER = SETBIT(OLD_COUNTER,
                                           CVALUE). */
    bcmFlexctrValueOperationTypeRdec = 8, /* COUNTER = CVALUE - OLD_COUNTER. */
    bcmFlexctrValueOperationTypeXor = 9, /* COUNTER = XOR(OLD_COUNTER, CVALUE). */
    bcmFlexctrValueOperationTypeCount = 10 /* Always last one. */
} bcm_flexctr_value_operation_type_t;

/* 
 * Flex counter value operation structure.
 * 
 * This data structure lists the operation of a flex counter update
 * value.
 * Number of supported elements for these member arrays and how the
 * value is generated from these elements may vary depending on device.
 * Flex counter update value generated from two elements (if supported):
 *      value0 = (SEL(object0) >> shift0) & ((1 << mask_size0) - 1)).
 *      value1 = (SEL(object1) & ((1 << mask_size1) - 1)) << shift1.
 *      value = (value1 | value0).
 *      New_counter = UPDATE_TYPE(old_counter, value).
 * Flex counter update value generated from three elements
 * (if supported):
 *      value0 = (SEL(object0) >> shift0) & ((1 << mask_size0) - 1)).
 *      value1 = ((SEL(object1) >> shift2) & ((1 << mask_size1) - 1)) <<
 * shift1.
 *      value2 = ((SEL(object2) >> shift4) & ((1 << mask_size2) - 1)) <<
 * shift3.
 *      value =  (value2 | value1 | value0).
 *      New_counter = UPDATE_TYPE(old_counter, value).
 */
typedef struct bcm_flexctr_value_operation_s {
    bcm_flexctr_value_select_t select;  /* Select counter value. */
    bcm_flexctr_object_t object[BCM_FLEXCTR_OPERATION_OBJECT_SIZE]; /* Flex counter object array. */
    uint32 quant_id[BCM_FLEXCTR_OPERATION_OBJECT_SIZE]; /* Quantization ID. */
    uint8 mask_size[BCM_FLEXCTR_OPERATION_OBJECT_SIZE]; /* Object mask size array. */
    uint8 shift[BCM_FLEXCTR_OPERATION_OBJECT_SIZE]; /* Object shift array. */
    bcm_flexctr_value_operation_type_t type; /* Counter value operation type. */
    uint32 object_id[BCM_FLEXCTR_OPERATION_OBJECT_SIZE]; /* Flex counter object ID. */
} bcm_flexctr_value_operation_t;

#define BCM_FLEXCTR_QUANTIZATION_RANGE_SIZE 8          /* Quantization range
                                                          check array size
                                                          number. */

/* Quantization flags definition. */
#define BCM_FLEXCTR_QUANTIZATION_FLAGS_GLOBAL (1 << 0)   /* Global quantization in
                                                          pipe unique mode. */

#define BCM_FLEXCTR_QUANTIZATION_FLAGS_32BIT_MODE (1 << 1)   /* Specify 32-bit values
                                                          in the range check
                                                          arrays (if supported). */

/* 
 * Flex counter quantization structure.
 * 
 * This data structure lists the properties of a flex counter
 * quantization.
 * Each quantization instance generates a range check result.
 */
typedef struct bcm_flexctr_quantization_s {
    bcm_flexctr_object_t object;        /* Flex counter object. */
    bcm_flexctr_object_t object_upper;  /* Flex counter object for the upper 16
                                           bits when flag
                                           BCM_FLEXCTR_QUANTIZATION_FLAGS_32BIT_MODE
                                           is set. The 'object' field is for the
                                           lower 16 bits. */
    bcm_pbmp_t ports;                   /* Flex counter ports. */
    uint32 range_check_min[BCM_FLEXCTR_QUANTIZATION_RANGE_SIZE]; /* Range check min value array. */
    uint32 range_check_max[BCM_FLEXCTR_QUANTIZATION_RANGE_SIZE]; /* Range check max value array. */
    uint8 range_num;                    /* Total range check number */
    uint32 object_id;                   /* Flex counter object ID. */
    uint32 object_id_upper;             /* Flex counter object ID (0-based
                                           index) for the counter object
                                           specified by 'object_upper' field (if
                                           required). Valid when flag
                                           BCM_FLEXCTR_QUANTIZATION_FLAGS_32BIT_MODE
                                           is set. */
    uint32 flags;                       /* Quantization flags. */
} bcm_flexctr_quantization_t;

/* Enumeration of flex counter trigger type. */
typedef enum bcm_flexctr_trigger_type_e {
    bcmFlexctrTriggerTypeTime = 0,      /* Time based trigger type. */
    bcmFlexctrTriggerTypeCondition = 1, /* Condition based trigger type. */
    bcmFlexctrTriggerTypeConditionStartTimeStop = 2, /* Condition based start and time based
                                           stop type. */
    bcmFlexctrTriggerTypeTimeStartActionCountStop = 3, /* Time based start and action count
                                           based stop type. */
    bcmFlexctrTriggerTypeConditionStartActionCountStop = 4, /* Condition based start and action
                                           count based stop type. */
    bcmFlexctrTriggerTypeCount = 5      /* Always last one. */
} bcm_flexctr_trigger_type_t;

/* Enumeration of flex counter trigger interval scale. */
typedef enum bcm_flexctr_trigger_interval_e {
    bcmFlexctrTriggerIntervalUnlimited = 0, /* Unlimited collection. */
    bcmFlexctrTriggerInterval100ns = 1, /* 100 nanosecond interval. */
    bcmFlexctrTriggerInterval500ns = 2, /* 500 nanosecond interval. */
    bcmFlexctrTriggerInterval1us = 3,   /* 1 microsecond interval. */
    bcmFlexctrTriggerInterval10us = 4,  /* 10 microsecond interval. */
    bcmFlexctrTriggerInterval100us = 5, /* 100 microsecond interval. */
    bcmFlexctrTriggerInterval1ms = 6,   /* 1 millisecond interval. */
    bcmFlexctrTriggerInterval10ms = 7,  /* 10 millisecond interval. */
    bcmFlexctrTriggerInterval100ms = 8, /* 100 millisecond interval. */
    bcmFlexctrTriggerInterval1s = 9,    /* 1 second interval. */
    bcmFlexctrTriggerInterval10s = 10,  /* 10 second interval. */
    bcmFlexctrTriggerIntervalCount = 11 /* Always last one. */
} bcm_flexctr_trigger_interval_t;

/* Trigger flags definition. */
#define BCM_FLEXCTR_TRIGGER_CONDITION_COMPARE_NOT_EQUAL_START (1 << 0)   /* Specify to use 'not
                                                          equal' as the compare
                                                          function to trigger
                                                          start counter update
                                                          for conditional
                                                          trigger. */

#define BCM_FLEXCTR_TRIGGER_CONDITION_COMPARE_NOT_EQUAL_STOP (1 << 1)   /* Specify to use 'not
                                                          equal' as the compare
                                                          function to trigger
                                                          stop counter update
                                                          for conditional
                                                          trigger. */

/* 
 * Flex counter trigger structure.
 * 
 * This data structure lists the properties of the flex counter trigger.
 */
typedef struct bcm_flexctr_trigger_s {
    bcm_flexctr_trigger_type_t trigger_type; /* Trigger type. */
    uint8 period_num;                   /* Time trigger period number. */
    bcm_flexctr_trigger_interval_t interval; /* Time trigger interval scale. */
    bcm_flexctr_object_t object;        /* Flex counter object. */
    uint16 object_value_start;          /* Conditional trigger start value. */
    uint16 object_value_stop;           /* Conditional trigger stop value. */
    uint16 object_value_mask;           /* Conditional trigger mask. */
    uint16 interval_shift;              /* Time trigger interval shift. */
    uint16 interval_size;               /* Time trigger interval size. */
    uint32 object_id;                   /* Flex counter object ID. */
    uint8 action_count_stop;            /* Number of times a counter action is
                                           hit before stopping. Only valid with
                                           action count based stop type. */
    uint32 flags;                       /* Trigger flags. */
} bcm_flexctr_trigger_t;

/* Enumeration of flex counter counter modes. */
typedef enum bcm_flexctr_counter_mode_e {
    bcmFlexctrCounterModeNormal = 0,    /* Each counter contains two values A
                                           and B.
                                           The update value of value A and B are
                                           separately controlled by
                                           value_a_operation and
                                           value_b_operation in counter action.
                                           All value operation modes except
                                           SetBit are suitable for values A and
                                           B. */
    bcmFlexctrCounterModeWide = 1,      /* Each counter only contains one value
                                           A.
                                           The update value of value A is
                                           controlled by value_a_operation in
                                           counter action.
                                           Only value operation modes noop, set,
                                           inc and setbit are suitable for value
                                           A. */
    bcmFlexctrCounterModeSlim = 2,      /* Each counter only contains one value
                                           A.
                                           The update value of value A is
                                           controlled by value_a_operation in
                                           counter action.
                                           Only value operation modes noop, set
                                           and inc are suitable for value A.
                                           Using slim mode could contain more
                                           counters within same h/w resources. */
    bcmFlexctrCounterModeCount = 3      /* Always last one. */
} bcm_flexctr_counter_mode_t;

/* Enumeration of flex counter drop count modes. */
typedef enum bcm_flexctr_drop_count_mode_e {
    bcmFlexctrDropCountModeNoDrop = 0,  /* Count only if not being dropped. */
    bcmFlexctrDropCountModeDrop = 1,    /* Count only if being dropped. */
    bcmFlexctrDropCountModeAll = 2,     /* Ignore drop status and always count. */
    bcmFlexctrDropCountModeCount = 3    /* Always last one. */
} bcm_flexctr_drop_count_mode_t;

/* Hint type. */
typedef enum bcm_flexctr_hint_type_e {
    bcmFlexctrHintPool = 0,         /* Assign the same pool ID on one hint value
                                       for the given flex counter source. */
    bcmFlexctrHintFieldGroupId = 1, /* Indicate a field group ID as the hint and
                                       assign the pool ID based on the field
                                       group priority. */
    bcmFlexctrHintCount = 2         /* Always last one. */
} bcm_flexctr_hint_type_t;

#define BCM_FLEXCTR_ACTION_FLAGS_TRIGGER    0x1        /* Flex counter trigger
                                                          is attached in the
                                                          action. */
#define BCM_FLEXCTR_ACTION_FLAGS_GLOBAL     0x2        /* Global action in pipe
                                                          unique mode. */

/* 
 * Flex counter action structure.
 * 
 * This data structure lists the properties of a flex counter action.
 */
typedef struct bcm_flexctr_action_s {
    uint32 flags;                       /* Action flags. */
    bcm_flexctr_source_t source;        /* Flex counter source. */
    bcm_pbmp_t ports;                   /* Flex counter ports. */
    int hint;                           /* Flex counter hint. */
    bcm_flexctr_drop_count_mode_t drop_count_mode; /* Counter drop mode for functional
                                           packet drops. */
    int exception_drop_count_enable;    /* Enable to count on hardware exception
                                           drops. */
    int egress_mirror_count_enable;     /* Enable to count egress mirrored
                                           packets also. */
    bcm_flexctr_counter_mode_t mode;    /* Action mode. */
    uint32 index_num;                   /* Total counter index number. */
    bcm_flexctr_index_operation_t index_operation; /* Flex counter index operation
                                           structure. */
    bcm_flexctr_value_operation_t operation_a; /* Flex counter value A operation
                                           structure. */
    bcm_flexctr_value_operation_t operation_b; /* Flex counter value B operation
                                           structure. */
    bcm_flexctr_trigger_t trigger;      /* Flex counter trigger structure. */
    bcm_flexctr_hint_type_t hint_type;  /* Hint type. */
} bcm_flexctr_action_t;

#define BCM_FLEXCTR_GROUP_ACTION_SIZE   16         /* Group action array size
                                                      number. */

/* Group action flags definition. */
#define BCM_FLEXCTR_GROUP_ACTION_FLAGS_GLOBAL (1 << 0)   /* Global group action in
                                                          pipe unique mode. */

/* 
 * Flex counter group action structure.
 * 
 * This data structure lists the properties of a flex counter group
 * action.
 */
typedef struct bcm_flexctr_group_action_s {
    bcm_flexctr_source_t source;        /* Flex counter source. */
    bcm_pbmp_t ports;                   /* Flex counter ports. */
    uint32 stat_counter_id[BCM_FLEXCTR_GROUP_ACTION_SIZE]; /* Flex counter group action array. */
    uint8 action_num;                   /* Total group action array number */
    uint32 flags;                       /* Group action flags. */
} bcm_flexctr_group_action_t;

/* 
 * Enumeration of flex counter packet types: used when packet attribute
 * selector type is:
 * - bcmFlexctrPacketAttributeTypeIngPktType.
 */
typedef enum bcm_flexctr_packet_attribute_pkt_type_e {
    bcmFlexctrPacketAttributePktTypeAll = 0, /* All packet types. */
    bcmFlexctrPacketAttributePktTypeUnknown = 1, /* Unknown packet. */
    bcmFlexctrPacketAttributePktTypeControl = 2, /* Control packet. */
    bcmFlexctrPacketAttributePktTypeBfd = 3, /* BFD (Bidirectional Forwarding
                                           Detection) packet. */
    bcmFlexctrPacketAttributePktTypeBpdu = 4, /* BPDU (Bridge Protocol Data Unit)
                                           packet. */
    bcmFlexctrPacketAttributePktType1588 = 5, /* IEEE 1588 packet. */
    bcmFlexctrPacketAttributePktTypeKnownL2Uc = 6, /* Known L2 unicast packet. */
    bcmFlexctrPacketAttributePktTypeUnknownL2Uc = 7, /* Unknown L2 unicast packet. */
    bcmFlexctrPacketAttributePktTypeKnownL2Mc = 8, /* Known L2 multicast packet. */
    bcmFlexctrPacketAttributePktTypeUnknownL2Mc = 9, /* Unknown L2 multicast packet. */
    bcmFlexctrPacketAttributePktTypeL2Bc = 10, /* L2 broadcast packet. */
    bcmFlexctrPacketAttributePktTypeKnownL3Uc = 11, /* Known L3 unicast packet. */
    bcmFlexctrPacketAttributePktTypeUnknownL3Uc = 12, /* Unknown L3 unicast packet. */
    bcmFlexctrPacketAttributePktTypeKnownIpmc = 13, /* Known IP multicast packet. */
    bcmFlexctrPacketAttributePktTypeUnknownIpmc = 14, /* Unknown IP multicast packet. */
    bcmFlexctrPacketAttributePktTypeKnownMplsL3 = 15, /* Known L3 MPLS packet. */
    bcmFlexctrPacketAttributePktTypeKnownMpls = 16, /* Known MPLS packet. */
    bcmFlexctrPacketAttributePktTypeUnknownMpls = 17, /* Unknown MPLS packet. */
    bcmFlexctrPacketAttributePktTypeCount = 18 /* Always last one. */
} bcm_flexctr_packet_attribute_pkt_type_t;

/* 
 * Enumeration of flex counter VLAN tag information: used when packet
 * attribute selector type is:
 * - bcmFlexctrPacketAttributeTypeIngVlanTag.
 * - bcmFlexctrPacketAttributeTypeEgrVlanTag.
 */
typedef enum bcm_flexctr_packet_attribute_vlan_e {
    bcmFlexctrPacketAttributeVlanUnTagged = 0, /* Untagged VLAN packets. */
    bcmFlexctrPacketAttributeVlanOuterTag = 1, /* Single outer VLAN tagged packets. */
    bcmFlexctrPacketAttributeVlanCount = 2 /* Always last one. */
} bcm_flexctr_packet_attribute_vlan_t;

/* 
 * Enumeration of flex counter packet attribute selector types.
 * 
 * Flex counter packet attribute selectors could be used to generate
 * offset indices and each offset index becomes additional object on
 * object bus. The object could be used to generate counter index and
 * counter value.
 */
typedef enum bcm_flexctr_packet_attribute_type_e {
    bcmFlexctrPacketAttributeTypeIngIp = 0, /* Ingress: IP packet. Possible values:
                                           0 or 1. */
    bcmFlexctrPacketAttributeTypeIngDrop = 1, /* Ingress: dropped packet. Possible
                                           values: 0 or 1. */
    bcmFlexctrPacketAttributeTypeIngPktType = 2, /* Ingress: packet type. Possible
                                           values:
                                           <bcmFlexctrPacketAttributePktType*>,
                                           mask: 6-bit (0x3F). */
    bcmFlexctrPacketAttributeTypeIngTosEcn = 3, /* Ingress: TOS for ECN (explicit
                                           congestion notification). Possible
                                           values: 0 to 3, mask: 2-bit (0x3). */
    bcmFlexctrPacketAttributeTypeIngTosDscp = 4, /* Ingress: TOS for DSCP (differentiated
                                           services code point). Possible
                                           values: 0 to 63, mask: 6-bit (0x3F). */
    bcmFlexctrPacketAttributeTypeIngPort = 5, /* Ingress: port number. Possible
                                           values: <MaxLogicalPort>, mask: 6-bit
                                           (0x3F). */
    bcmFlexctrPacketAttributeTypeIngIntCn = 6, /* Ingress: internal congestion
                                           notification. Possible values: 0 to
                                           3, mask: 2-bit (0x3). */
    bcmFlexctrPacketAttributeTypeIngOuterPri = 7, /* Ingress: outer VLAN priority.
                                           Possible values: 0 to 7, mask: 3-bit
                                           (0x7). */
    bcmFlexctrPacketAttributeTypeIngVlanTag = 8, /* Ingress: VLAN tag. Possible values:
                                           <bcmFlexctrPacketAttributeVlan*>,
                                           mask: 1-bit (0x1). */
    bcmFlexctrPacketAttributeTypeIngIntPri = 9, /* Ingress: internal priority. Possible
                                           values: 0 to 15, mask: 4-bit (0xF). */
    bcmFlexctrPacketAttributeTypeIngColor = 10, /* Ingress: packet color. Possible
                                           values: <bcmColorGreen|Yellow|Red>,
                                           mask: 2-bit (0x3). */
    bcmFlexctrPacketAttributeTypeIngFieldIngressColor = 11, /* Ingress: IFP color. Possible values:
                                           <bcmColorGreen|Yellow|Red>, mask:
                                           2-bit (0x3). */
    bcmFlexctrPacketAttributeTypeIngElephant = 12, /* Ingress: elephant packet. Possible
                                           values: 0 or 1. */
    bcmFlexctrPacketAttributeTypeIngTcpFlagsSyn = 13, /* Ingress: TCP SYN flag. Possible
                                           values: 0 or 1. */
    bcmFlexctrPacketAttributeTypeIngTcpFlagsFin = 14, /* Ingress: TCP FIN flag. Possible
                                           values: 0 or 1. */
    bcmFlexctrPacketAttributeTypeIngTcpFlagsRst = 15, /* Ingress: TCP RST flag. Possible
                                           values: 0 or 1. */
    bcmFlexctrPacketAttributeTypeIngTcpFlagsPsh = 16, /* Ingress: TCP PSH flag. Possible
                                           values: 0 or 1. */
    bcmFlexctrPacketAttributeTypeIngTcpFlagsAck = 17, /* Ingress: TCP ACK flag. Possible
                                           values: 0 or 1. */
    bcmFlexctrPacketAttributeTypeIngTcpFlagsUrg = 18, /* Ingress: TCP URG flag. Possible
                                           values: 0 or 1. */
    bcmFlexctrPacketAttributeTypeIngTcpFlagsEce = 19, /* Ingress: TCP ECE flag. Possible
                                           values: 0 or 1. */
    bcmFlexctrPacketAttributeTypeIngTcpFlagsCwr = 20, /* Ingress: TCP CWR flag. Possible
                                           values: 0 or 1. */
    bcmFlexctrPacketAttributeTypeIngMplsExp = 21, /* Ingress: MPLS experimental field.
                                           Possible values: 0 to 7, mask: 3-bit
                                           (0x7). */
    bcmFlexctrPacketAttributeTypeEgrIp = 22, /* Egress: IP packet. Possible values: 0
                                           or 1. */
    bcmFlexctrPacketAttributeTypeEgrDrop = 23, /* Egress: dropped packet. Possible
                                           values: 0 or 1. */
    bcmFlexctrPacketAttributeTypeEgrPktTypeMc = 24, /* Egress: multicast packet type.
                                           Possible values: 0: unicast or 1:
                                           multicast. */
    bcmFlexctrPacketAttributeTypeEgrTosEcn = 25, /* Egress: TOS for ECN (explicit
                                           congestion notification). Possible
                                           values: 0 to 3, mask: 2-bit (0x3). */
    bcmFlexctrPacketAttributeTypeEgrTosDscp = 26, /* Egress: TOS for DSCP (differentiated
                                           services code point). Possible
                                           values: 0 to 63, mask: 6-bit (0x3F). */
    bcmFlexctrPacketAttributeTypeEgrEgressPort = 27, /* Egress: port number. Possible values:
                                           <MaxLogicalPort>, mask: 6-bit (0x3F). */
    bcmFlexctrPacketAttributeTypeEgrIntCn = 28, /* Egress: internal congestion
                                           notification. Possible values: 0 to
                                           3, mask: 2-bit (0x3). */
    bcmFlexctrPacketAttributeTypeEgrOuterPri = 29, /* Egress: outer VLAN priority. Possible
                                           values: 0 to 7, mask: 3-bit (0x7). */
    bcmFlexctrPacketAttributeTypeEgrVlanTag = 30, /* Egress: VLAN tag. Possible values:
                                           <bcmFlexctrPacketAttributeVlan*>,
                                           mask: 1-bit (0x1). */
    bcmFlexctrPacketAttributeTypeEgrIntPri = 31, /* Egress: internal priority. Possible
                                           values: 0 to 15, mask: 4-bit (0xF). */
    bcmFlexctrPacketAttributeTypeEgrColor = 32, /* Egress: packet color. Possible
                                           values: <bcmColorGreen|Yellow|Red>,
                                           mask: 2-bit (0x3). */
    bcmFlexctrPacketAttributeTypeEgrCongestionMarked = 33, /* Egress: congestion marked status due
                                           to MMU congestion. Possible values: 0
                                           or 1. */
    bcmFlexctrPacketAttributeTypeEgrQueueType = 34, /* Egress: queue type. Possible values:
                                           0: multicast or 1: unicast. */
    bcmFlexctrPacketAttributeTypeEgrCos = 35, /* Egress: MMU cos. Possible values: 0
                                           to 15, mask: 4-bit (0xF). */
    bcmFlexctrPacketAttributeTypeEgrMplsExp = 36, /* Egress: MPLS experimental field.
                                           Possible values: 0 to 7, mask: 3-bit
                                           (0x7). */
    bcmFlexctrPacketAttributeTypeCount = 37 /* Always last one. Not a usable value. */
} bcm_flexctr_packet_attribute_type_t;

/* 
 * Enumeration of flex counter direction: used to indicates the direction
 * of a flex counter action.
 */
typedef enum bcm_flexctr_direction_e {
    bcmFlexctrDirectionIngress = 0, /* Ingress flex counter. */
    bcmFlexctrDirectionEgress = 1,  /* Egress flex counter. */
    bcmFlexctrDirectionCount = 2    /* Always last one. */
} bcm_flexctr_direction_t;

/* 
 * Flex counter packet attribute selector structure.
 * 
 * This data structure lists the properties of the flex counter packet
 * attribute selector.
 */
typedef struct bcm_flexctr_packet_attribute_selector_s {
    bcm_flexctr_packet_attribute_type_t attr_type; /* Packet attribute selector type. */
    uint32 attr_mask;                   /* Packet attribute selector mask. The
                                           valid mask range depends on packet
                                           attribute selector type. */
} bcm_flexctr_packet_attribute_selector_t;

/* Flex counter packet attribute selectors. */
#define BCM_FLEXCTR_PACKET_ATTRIBUTE_MAX_SELECTORS 8          /* Maximum number of
                                                          selectors for packet
                                                          attribute. */

/* 
 * Flex counter packet attribute profile structure.
 * 
 * This data structure lists the properties of the flex counter packet
 * attribute profile.
 */
typedef struct bcm_flexctr_packet_attribute_profile_s {
    bcm_pbmp_t ports;                   /* Flex counter ports. */
    uint32 num_selectors;               /* Number of packet attribute selectors
                                           used in this profile. */
    bcm_flexctr_packet_attribute_selector_t attr_selectors[BCM_FLEXCTR_PACKET_ATTRIBUTE_MAX_SELECTORS]; /* Attribute selectors array for flex
                                           counter packet attribute. */
} bcm_flexctr_packet_attribute_profile_t;

/* 
 * Flex counter packet attribute map entry structure.
 * 
 * This data structure lists the properties of the flex counter packet
 * attribute map entry.
 */
typedef struct bcm_flexctr_packet_attribute_map_entry_s {
    bcm_flexctr_packet_attribute_type_t attr_type; /* Packet attribute selector type. */
    uint32 attr_value;                  /* Packet attribute selector value. The
                                           valid value range depends on packet
                                           attribute selector type. */
} bcm_flexctr_packet_attribute_map_entry_t;

/* 
 * Flex counter packet attribute map structure.
 * 
 * This data structure lists the properties of the flex counter packet
 * attribute map.
 */
typedef struct bcm_flexctr_packet_attribute_map_s {
    uint32 num_entries;                 /* Number of packet attribute map
                                           entries used in this map. */
    bcm_flexctr_packet_attribute_map_entry_t attr_map_entries[BCM_FLEXCTR_PACKET_ATTRIBUTE_MAX_SELECTORS]; /* Attribute  map entries array for flex
                                           counter packet attribute. */
} bcm_flexctr_packet_attribute_map_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Flexctr module initialization function.
 * 
 * This function is used to initialize flexctr module.
 * 
 * [in] unit Unit number.
 * 
 * BCM_E_NONE No errors.
 * BCM_E_XXX API errors.
 */
extern int bcm_flexctr_init(
    int unit);

/* 
 * Flexctr module cleanup function.
 * 
 * This function is used to shut down flexctr module.
 * 
 * [in] unit Unit number.
 * 
 * BCM_E_NONE No errors.
 * BCM_E_XXX API errors.
 */
extern int bcm_flexctr_cleanup(
    int unit);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_FLEXCTR_OPTIONS_WITH_ID 0x1        /* Instance id is presented. */
#define BCM_FLEXCTR_OPTIONS_REPLACE 0x2        /* Need to replace an existing
                                                  instance. */

/* Flex counter quantization data structure initialization function. */
extern void bcm_flexctr_quantization_t_init(
    bcm_flexctr_quantization_t *quantization);

#ifndef BCM_HIDE_DISPATCHABLE

/* Flex counter quantization creation function. */
extern int bcm_flexctr_quantization_create(
    int unit, 
    int options, 
    bcm_flexctr_quantization_t *quantization, 
    uint32 *quant_id);

/* Flex counter quantization destroy function. */
extern int bcm_flexctr_quantization_destroy(
    int unit, 
    uint32 quant_id);

/* Flex counter quantization get function. */
extern int bcm_flexctr_quantization_get(
    int unit, 
    uint32 quant_id, 
    bcm_flexctr_quantization_t *quantization);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Flex counter quantization traverse callback. */
typedef int (*bcm_flexctr_quantization_traverse_cb)(
    int unit, 
    uint32 quant_id, 
    bcm_flexctr_quantization_t *quantization, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Flex counter quantization traverse function. */
extern int bcm_flexctr_quantization_traverse(
    int unit, 
    bcm_flexctr_quantization_traverse_cb trav_fn, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Flex counter action data structure initialization function. */
extern void bcm_flexctr_action_t_init(
    bcm_flexctr_action_t *action);

#ifndef BCM_HIDE_DISPATCHABLE

/* Flex counter action creation function. */
extern int bcm_flexctr_action_create(
    int unit, 
    int options, 
    bcm_flexctr_action_t *action, 
    uint32 *stat_counter_id);

/* Flex counter action destroy function. */
extern int bcm_flexctr_action_destroy(
    int unit, 
    uint32 stat_counter_id);

/* Flex counter action get function. */
extern int bcm_flexctr_action_get(
    int unit, 
    uint32 stat_counter_id, 
    bcm_flexctr_action_t *action);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Flex counter action traverse callback. */
typedef int (*bcm_flexctr_action_traverse_cb)(
    int unit, 
    uint32 stat_counter_id, 
    bcm_flexctr_action_t *action, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Flex counter action traverse function. */
extern int bcm_flexctr_action_traverse(
    int unit, 
    bcm_flexctr_action_traverse_cb trav_fn, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Flex counter group action data structure initialization function. */
extern void bcm_flexctr_group_action_t_init(
    bcm_flexctr_group_action_t *group_action);

#ifndef BCM_HIDE_DISPATCHABLE

/* Flex counter group action creation function. */
extern int bcm_flexctr_group_action_create(
    int unit, 
    int options, 
    bcm_flexctr_group_action_t *group_action, 
    uint32 *group_stat_counter_id);

/* Flex counter group action destroy function. */
extern int bcm_flexctr_group_action_destroy(
    int unit, 
    uint32 group_stat_counter_id);

/* Flex counter group action get function. */
extern int bcm_flexctr_group_action_get(
    int unit, 
    uint32 group_stat_counter_id, 
    bcm_flexctr_group_action_t *group_action);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Flex counter group action traverse callback. */
typedef int (*bcm_flexctr_group_action_traverse_cb)(
    int unit, 
    uint32 group_stat_counter_id, 
    bcm_flexctr_group_action_t *group_action, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Flex counter group action traverse function. */
extern int bcm_flexctr_group_action_traverse(
    int unit, 
    bcm_flexctr_group_action_traverse_cb trav_fn, 
    void *user_data);

/* Flex counter trigger enable set function. */
extern int bcm_flexctr_trigger_enable_set(
    int unit, 
    uint32 stat_counter_id, 
    int enable);

/* Flex counter trigger enable get function. */
extern int bcm_flexctr_trigger_enable_get(
    int unit, 
    uint32 stat_counter_id, 
    int *enable);

/* Flex counter statistic get function. */
extern int bcm_flexctr_stat_get(
    int unit, 
    uint32 stat_counter_id, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_flexctr_counter_value_t *counter_values);

/* Flex counter statistic sync get function. */
extern int bcm_flexctr_stat_sync_get(
    int unit, 
    uint32 stat_counter_id, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_flexctr_counter_value_t *counter_values);

/* Flex counter statistic set function. */
extern int bcm_flexctr_stat_set(
    int unit, 
    uint32 stat_counter_id, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_flexctr_counter_value_t *counter_values);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Flex counter packet attribute options for creating a profile. */
#define BCM_FLEXCTR_PACKET_ATTRIBUTE_OPTIONS_WITH_ID 0x1        /* Instance id is
                                                          presented. */
#define BCM_FLEXCTR_PACKET_ATTRIBUTE_OPTIONS_REPLACE 0x2        /* Need to replace an
                                                          existing instance. */

/* 
 * Flex counter packet attribute selector data structure initialization
 * function.
 */
extern void bcm_flexctr_packet_attribute_selector_t_init(
    bcm_flexctr_packet_attribute_selector_t *attr_selector);

/* 
 * Flex counter packet attribute profile data structure initialization
 * function.
 */
extern void bcm_flexctr_packet_attribute_profile_t_init(
    bcm_flexctr_packet_attribute_profile_t *attr_profile);

#ifndef BCM_HIDE_DISPATCHABLE

/* Flex counter packet attribute profile creation function. */
extern int bcm_flexctr_packet_attribute_profile_create(
    int unit, 
    int options, 
    bcm_flexctr_packet_attribute_profile_t *attr_profile, 
    int *profile_id);

/* Flex counter packet attribute profile destroy function. */
extern int bcm_flexctr_packet_attribute_profile_destroy(
    int unit, 
    int profile_id);

/* Flex counter packet attribute profile get function. */
extern int bcm_flexctr_packet_attribute_profile_get(
    int unit, 
    int profile_id, 
    bcm_flexctr_packet_attribute_profile_t *attr_profile);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Flex counter packet attribute profile traverse callback. */
typedef int (*bcm_flexctr_packet_attribute_profile_traverse_cb)(
    int unit, 
    int profile_id, 
    bcm_flexctr_packet_attribute_profile_t *attr_profile, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Flex counter packet attribute profile traverse function. */
extern int bcm_flexctr_packet_attribute_profile_traverse(
    int unit, 
    bcm_flexctr_packet_attribute_profile_traverse_cb trav_fn, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Flex counter packet attribute map entry data structure initialization
 * function.
 */
extern void bcm_flexctr_packet_attribute_map_entry_t_init(
    bcm_flexctr_packet_attribute_map_entry_t *attr_map_entry);

/* 
 * Flex counter packet attribute map data structure initialization
 * function.
 */
extern void bcm_flexctr_packet_attribute_map_t_init(
    bcm_flexctr_packet_attribute_map_t *attr_map);

#ifndef BCM_HIDE_DISPATCHABLE

/* Flex counter packet attribute map entry add function. */
extern int bcm_flexctr_packet_attribute_map_add(
    int unit, 
    int profile_id, 
    bcm_flexctr_packet_attribute_map_t *attr_map, 
    uint32 offset_value);

/* Flex counter packet attribute map entry delete function. */
extern int bcm_flexctr_packet_attribute_map_delete(
    int unit, 
    int profile_id, 
    bcm_flexctr_packet_attribute_map_t *attr_map);

/* Flex counter packet attribute map entry delete all function. */
extern int bcm_flexctr_packet_attribute_map_delete_all(
    int unit, 
    int profile_id);

/* Flex counter packet attribute map entry get function. */
extern int bcm_flexctr_packet_attribute_map_get(
    int unit, 
    int profile_id, 
    bcm_flexctr_packet_attribute_map_t *attr_map, 
    uint32 *offset_value);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Flex counter packet attribute map entry traverse callback. */
typedef int (*bcm_flexctr_packet_attribute_map_traverse_cb)(
    int unit, 
    bcm_flexctr_packet_attribute_map_t *attr_map, 
    uint32 offset_value, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Flex counter packet attribute map entry traverse function. */
extern int bcm_flexctr_packet_attribute_map_traverse(
    int unit, 
    int profile_id, 
    bcm_flexctr_packet_attribute_map_traverse_cb trav_fn, 
    void *user_data);

/* Get overlaid objects for a specified flexctr object. */
extern int bcm_flexctr_object_overlay_multi_get(
    int unit, 
    bcm_flexctr_object_t object, 
    int array_size, 
    bcm_flexctr_object_t *overlay_object_array, 
    int *count);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_FLEXCTR_H__ */
