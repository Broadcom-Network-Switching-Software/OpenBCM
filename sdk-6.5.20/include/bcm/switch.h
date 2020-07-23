/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_SWITCH_H__
#define __BCM_SWITCH_H__

#include <bcm/port.h>
#include <bcm/error.h>
#include <bcm/udf.h>
#include <bcm/field.h>
#include <bcm/stg.h>
#include <bcm/types.h>
#include <shared/switch.h>

/* Switch controls. */
typedef enum bcm_switch_control_e {
    bcmSwitchHgHdrErrToCpu = 0,         /* HiGig Header Error to CPU. */
    bcmSwitchClassTagPacketsToCpu = 1,  /* HG2 packets with a PPD header of 1. */
    bcmSwitchIpmcTunnelToCpu = 2,       /* IPMC tunnel packet to CPU. */
    bcmSwitchStationMoveOverLearnLimitToCpu = 3, /* Station movement over MAC learn limit
                                           to CPU. */
    bcmSwitchMcastUnknownErrToCpu = 4,  /* MC unknown Index Error to CPU. */
    bcmSwitchLearnLimitPriority = 5,    /* Priority for MAC limit exceeds to
                                           CPU. */
    bcmSwitchControlOpcodePriority = 6, /* Control Op Code priority. */
    bcmSwitchCpuProtoBpduPriority = 7,  /* Priority for BPDUs to CPU. */
    bcmSwitchCpuProtoArpPriority = 8,   /* Priority for ARP/ND packets to CPU. */
    bcmSwitchCpuProtoIgmpPriority = 9,  /* Priority for IGMP/MLD packets to CPU. */
    bcmSwitchCpuProtoDhcpPriority = 10, /* Priority for DHCP packets to CPU. */
    bcmSwitchCpuProtoIpmcReservedPriority = 11, /* Priority for IPv4/IPv6 multicast
                                           packets to CPU. */
    bcmSwitchCpuProtoIpOptionsPriority = 12, /* Priority for IPv4/IPv6 packets with
                                           options or IPv6 packets with 0 len.
                                           Payload/jumbo option to CPU. */
    bcmSwitchCpuProtoExceptionsPriority = 13, /* Priority for IPv4/IPv6 packets with
                                           TTL=1 to CPU. */
    bcmSwitchCpuProtocolPrio = 14,      /* Priority used for BPDUs to CPU. */
    bcmSwitchCpuUnknownPrio = 15,       /* Priority for unknown SA or DA to CPU. */
    bcmSwitchCpuSamplePrio = 16,        /* Priority for sFlow sample to CPU. */
    bcmSwitchCpuDefaultPrio = 17,       /* Priority for all other packets to
                                           CPU. */
    bcmSwitchL2StaticMoveToCpu = 18,    /* Port movement of static L2 addr. */
    bcmSwitchUnknownIpmcToCpu = 19,     /* Send unknown IP multicasts to CPU. */
    bcmSwitchUnknownMcastToCpu = 20,    /* Send unknown multicasts to CPU. */
    bcmSwitchUnknownUcastToCpu = 21,    /* Send DLF packets to CPU. */
    bcmSwitchNonIpL3ErrToCpu = 22,      /* Non-IP packets w/DA of my router MAC
                                           to CPU. */
    bcmSwitchL3HeaderErrToCpu = 23,     /* Invalid checksum, TTL 0, etc. */
    bcmSwitchUnknownVlanToCpu = 24,     /* Packet VLAN not in VLAN table. */
    bcmSwitchUnknownL3SrcToCpu = 25,    /* SIP not found in L3 table. */
    bcmSwitchUnknownL3DestToCpu = 26,   /* DIP not found in L3/LPM tables. */
    bcmSwitchIpmcPortMissToCpu = 27,    /* Incoming port not in IPMC entry. */
    bcmSwitchIpmcErrorToCpu = 28,       /* IPMC pkt has header err or TTL=0. */
    bcmSwitchVCLabelMissToCpu = 29,     /* Send VC mismatch packet to CPU. */
    bcmSwitchSourceRouteToCpu = 30,     /* Send multicast SA packet to CPU. */
    bcmSwitchSampleIngressRandomSeed = 31, /* SFlow ingress RNG seed. */
    bcmSwitchSampleEgressRandomSeed = 32, /* SFlow egress RNG seed. */
    bcmSwitchCpuFpCopyPrio = 33,        /* Priority used for FP copy to CPU. */
    bcmSwitchCpuIcmpRedirectPrio = 34,  /* Priority for ICMP to CPU. */
    bcmSwitchCpuMtuFailPrio = 35,       /* Priority for MTU failure to CPU. */
    bcmSwitchCpuMirrorPrio = 36,        /* Priority for packets mirrored to CPU. */
    bcmSwitchCpuLookupFpCopyPrio = 37,  /* Priority for Lookup FP copy to CPU. */
    bcmSwitchL2NonStaticMoveToCpu = 38, /* L2 w/non-static addr to CPU. */
    bcmSwitchV6L3ErrToCpu = 39,         /* Invalid IPv6 packets to CPU. */
    bcmSwitchV6L3DstMissToCpu = 40,     /* IPv6 unknown dest packets to CPU. */
    bcmSwitchV6L3LocalLinkDrop = 41,    /* Drop IPv6 with link local sources. */
    bcmSwitchV4L3ErrToCpu = 42,         /* Invalid IPv6 packets to CPU. */
    bcmSwitchV4L3DstMissToCpu = 43,     /* IPv6 unknown dest packets to CPU. */
    bcmSwitchTunnelErrToCpu = 44,       /* Invalid tunnel packets to CPU. */
    bcmSwitchMartianAddrToCpu = 45,     /* Martian packets to CPU. */
    bcmSwitchMartianAddr = 46,          /* Enable/Disable Martian packet check. */
    bcmSwitchL3UcTtlErrToCpu = 47,      /* L3 unicast w/invalid TTL to CPU. */
    bcmSwitchL3SlowpathToCpu = 48,      /* Slowpath packets to CPU. */
    bcmSwitchIpmcTtlErrToCpu = 49,      /* IPMC packets w/invalid TTL to CPU. */
    bcmSwitchDosAttackToCpu = 50,       /* Denial of Service attacks to CPU. */
    bcmSwitchDosAttackSipEqualDip = 51, /* Enable SIP=DIP checking. */
    bcmSwitchDosAttackMinTcpHdrSize = 52, /* Header length for min TCP header
                                           checking. */
    bcmSwitchDosAttackV4FirstFrag = 53, /* Enable IPv4 First Frag checking. */
    bcmSwitchDosAttackTcpFlags = 54,    /* Enable TCP flag checking. */
    bcmSwitchDosAttackL4Port = 55,      /* Enable L4 port number checking. */
    bcmSwitchDosAttackTcpFrag = 56,     /* Enable TCP fragment checking. */
    bcmSwitchDosAttackIcmp = 57,        /* Enable ICMP size checK for v4 and v6. */
    bcmSwitchDosAttackIcmpV4 = 58,      /* Enable ICMPv4 size check. */
    bcmSwitchDosAttackIcmpV6 = 59,      /* Enable ICMPv6 size check. */
    bcmSwitchDosAttackIcmpPktOversize = 60, /* Max pkt size for ICMP size check. */
    bcmSwitchDosAttackMACSAEqualMACDA = 61, /* MACSA == MACDA check. */
    bcmSwitchDosAttackIcmpV6PingSize = 62, /* Max pkt size for v6 ping check. */
    bcmSwitchDosAttackIcmpFragments = 63, /* Fragmented ICMP packets check. */
    bcmSwitchDosAttackTcpOffset = 64,   /* TCP Header offset equals to 1 check. */
    bcmSwitchDosAttackUdpPortsEqual = 65, /* UDP packets where SPORT = DROP. */
    bcmSwitchDosAttackTcpPortsEqual = 66, /* TCP packets where SPORT = DROP. */
    bcmSwitchDosAttackTcpFlagsSF = 67,  /* TCP packets with SYN & FIN bits
                                           enabled. */
    bcmSwitchDosAttackTcpFlagsFUP = 68, /* TCP packets with FIN, URG, PSH bits
                                           set and sequence number equals 0. */
    bcmSwitchDosAttackTcpHdrPartial = 69, /* TCP packets with not full TCP header. */
    bcmSwitchDosAttackPingFlood = 70,   /* Enable ping flood checking. */
    bcmSwitchDosAttackSynFlood = 71,    /* Enable SYN flood checking. */
    bcmSwitchDosAttackTcpSmurf = 72,    /* Enable TCP smurf checking. */
    bcmSwitchDosAttackTcpXMas = 73,     /* SeqNum, Fin,Urg,Psh equal 0. */
    bcmSwitchDosAttackL3Header = 74,    /* Ipv4/6 header length checks. */
    bcmSwitchIcmpRedirectToCpu = 75,    /* ICMP redirect To CPU. */
    bcmSwitchMplsSequenceErrToCpu = 76, /* MPLS Sequence number check failure to
                                           CPU. */
    bcmSwitchMplsLabelMissToCpu = 77,   /* MPLS label lookup miss to CPU. */
    bcmSwitchMplsTtlErrToCpu = 78,      /* MPLS Header TTL used value is 0 or 1. */
    bcmSwitchMplsInvalidL3PayloadToCpu = 79, /* MPLS L3 payload invalid. */
    bcmSwitchMplsInvalidActionToCpu = 80, /* Invalid action / comnination of
                                           actions. */
    bcmSwitchSharedVlanMismatchToCpu = 81, /* Private VLAN VID mismatch. */
    bcmSwitchL3SrcUrpfErrToCpu = 82,    /* Invalid L3 uRPF SIP to CPU. */
    bcmSwitchGreKeyToVlan = 83,         /* Enable system configuration for GRE
                                           enhancement (including GRE.key). */
    bcmSwitchArpReplyToCpu = 84,        /* ARP replies to CPU. */
    bcmSwitchArpReplyDrop = 85,         /* ARP replies dropped. */
    bcmSwitchArpRequestToCpu = 86,      /* ARP requests to CPU. */
    bcmSwitchArpRequestDrop = 87,       /* ARP requests dropped. */
    bcmSwitchArpReplyMyStationL2ToCPU = 88, /* Only APR reply packets with My MAC
                                           should be sent to CPU */
    bcmSwitchNdPktToCpu = 89,           /* ND packets to CPU. */
    bcmSwitchNdPktDrop = 90,            /* ND packets dropped. */
    bcmSwitchMcastFloodBlocking = 91,   /* Current PFM rule is applied when
                                           IGMP/MLD/MC/ReservedMC packet flooded
                                           to VLAN. */
    bcmSwitchIgmpPktToCpu = 92,         /* IGMP packets to CPU. */
    bcmSwitchIgmpPktDrop = 93,          /* IGMP packets dropped. */
    bcmSwitchDhcpPktToCpu = 94,         /* DHCP packets to CPU. */
    bcmSwitchDhcpPktDrop = 95,          /* DHCP packets dropped. */
    bcmSwitchMldPktToCpu = 96,          /* MLD packets to CPU. */
    bcmSwitchMldPktDrop = 97,           /* MLD packets dropped. */
    bcmSwitchV4ResvdMcPktToCpu = 98,    /* IPv4 reserved MC packets to CPU. */
    bcmSwitchV4ResvdMcPktDrop = 99,     /* IPv4 reserved MC packets dropped. */
    bcmSwitchV4ResvdMcPktFlood = 100,   /* IPv4 reserved MC packets flooded to
                                           VLAN. */
    bcmSwitchV6ResvdMcPktToCpu = 101,   /* IPv6 reserved MC packets to CPU. */
    bcmSwitchV6ResvdMcPktDrop = 102,    /* IPv6 reserved MC packets dropped. */
    bcmSwitchV6ResvdMcPktFlood = 103,   /* IPv6 reserved MC packets flooded to
                                           VLAN. */
    bcmSwitchIgmpReportLeaveToCpu = 104, /* IGMP report/leave messages to CPU. */
    bcmSwitchIgmpReportLeaveDrop = 105, /* IGMP report/leave messages dropped. */
    bcmSwitchIgmpReportLeaveFlood = 106, /* IGMP report/leave messages flooded to
                                           VLAN. */
    bcmSwitchIgmpQueryToCpu = 107,      /* IGMP query messages to CPU. */
    bcmSwitchIgmpQueryDrop = 108,       /* IGMP query messages dropped. */
    bcmSwitchIgmpQueryFlood = 109,      /* IGMP query messages flooded to VLAN. */
    bcmSwitchIgmpUnknownToCpu = 110,    /* IGMP unknown messages to CPU. */
    bcmSwitchIgmpUnknownDrop = 111,     /* IGMP unknown messages dropped. */
    bcmSwitchIgmpUnknownFlood = 112,    /* IGMP unknown messages flooded to
                                           VLAN. */
    bcmSwitchMldReportDoneToCpu = 113,  /* MLD report/done messages to CPU. */
    bcmSwitchMldReportDoneDrop = 114,   /* MLD report/done messages dropped. */
    bcmSwitchMldReportDoneFlood = 115,  /* MLD report/done messages flooded to
                                           VLAN. */
    bcmSwitchMldQueryToCpu = 116,       /* MLD query messages to CPU. */
    bcmSwitchMldQueryDrop = 117,        /* MLD query messages dropped. */
    bcmSwitchMldQueryFlood = 118,       /* MLD query messages flooded to VLAN. */
    bcmSwitchIpmcV4RouterDiscoveryToCpu = 119, /* IPv4 Router discovery messages to
                                           CPU. */
    bcmSwitchIpmcV4RouterDiscoveryDrop = 120, /* IPv4 Router discovery messages
                                           dropped. */
    bcmSwitchIpmcV4RouterDiscoveryFlood = 121, /* IPv4 Router discovery messages
                                           flooded to VLAN. */
    bcmSwitchIpmcV6RouterDiscoveryToCpu = 122, /* IPv6 Router discovery messages to
                                           CPU. */
    bcmSwitchIpmcV6RouterDiscoveryDrop = 123, /* IPv6 Router discovery messages
                                           dropped. */
    bcmSwitchIpmcV6RouterDiscoveryFlood = 124, /* IPv6 Router discovery messages
                                           flooded to VLAN. */
    bcmSwitchDirectedMirroring = 125,   /* Enable directed mirroring mode. */
    bcmSwitchPktAge = 126,              /* Chip-wide packet age time, msec. */
    bcmSwitchMcastFloodDefault = 127,   /* VLAN Multicast flood control. */
    bcmSwitchParityErrorToCpu = 128,    /* Lookup parity error. */
    bcmSwitchL3MtuFailToCpu = 129,      /* L3 MTU check failure. */
    bcmSwitchMeterAdjust = 130,         /* Meter/Rate control with overhead on
                                           wire accounted for per packet. */
    bcmSwitchCounterAdjust = 131,       /* Counter compensation. */
    bcmSwitchMeterAdjustInterframeGap = 132, /* Indicate the size in Bytes of the IFG
                                           in Header-compensation for policer. */
    bcmSwitchHashL2 = 133,              /* Hash Select for L2. */
    bcmSwitchHashL3 = 134,              /* Hash Select for L3. */
    bcmSwitchHashMultipath = 135,       /* Hash Select for Multipath. */
    bcmSwitchHashControl = 136,         /* Hash Control of fields. */
    bcmSwitchFieldMultipathHashSelect = 137, /* Hash Select for FP
                                           bcmFieldActionL3Switch. */
    bcmSwitchFieldMultipathHashSeed = 138, /* Hash Seed for FP
                                           bcmFieldActionL3Switch. */
    bcmSwitchMirrorStackMode = 139,     /* Select BCM56504, 5670 or 5675-A0
                                           style stacking. */
    bcmSwitchMirrorSrcModCheck = 140,   /* Source modid check in BCM56504 style
                                           of mirroring. */
    bcmSwitchMirrorUnmarked = 141,      /* Send mirror packets as simple
                                           unicast. */
    bcmSwitchDestPortHGTrunk = 142,     /* Enable to use dest port in HiGig
                                           trunking. */
    bcmSwitchColorSelect = 143,         /* Choose color selection source. */
    bcmSwitchModuleLoopback = 144,      /* Allow local module ingress. */
    bcmSwitchSrcModBlockMirrorCopy = 145, /* Source modid blocking for BCM5670
                                           compatibility. */
    bcmSwitchSrcModBlockMirrorOnly = 146, /* Source modid blocking for BCM5675-A0
                                           compatibility. */
    bcmSwitchHashSeed0 = 147,           /* BCM56700/56800/56580 hash seeds. */
    bcmSwitchHashSeed1 = 148,           /* For enhanced hashing algoithm. */
    bcmSwitchHashField0PreProcessEnable = 149, /* Enable pre-processing for enhanced
                                           hash mode 0. */
    bcmSwitchHashField1PreProcessEnable = 150, /* Enable pre-processing for enhanced
                                           hash mode 1. */
    bcmSwitchHashField0Config = 151,    /* BCM56700/56800/56580 enhanced hash
                                           mode 0 config. */
    bcmSwitchHashField0Config1 = 152,   /* BCM56840 enhanced hash mode 0 config
                                           1. */
    bcmSwitchHashField1Config = 153,    /* BCM56700/56800/56580 enhanced hash
                                           mode 1 config. */
    bcmSwitchHashField1Config1 = 154,   /* BCM56840 enhanced hash mode 1 config
                                           1. */
    bcmSwitchMacroFlowHashFieldConfig = 155, /* Enhanced hash macro flow mode config. */
    bcmSwitchHashSelectControl = 156,   /* BCM56700/56800/56580 field selection
                                           control. */
    bcmSwitchHashIP4Field0 = 157,       /* BCM56700/56800/56580 enhanced hash
                                           field. */
    bcmSwitchHashIP4Field1 = 158,       /* Selections for IPv4 packets. */
    bcmSwitchHashIP4TcpUdpField0 = 159, /* BCM56840 enhanced hash field. */
    bcmSwitchHashIP4TcpUdpField1 = 160, /* Selections for IPv4 TCP/UDP packets. */
    bcmSwitchHashIP4TcpUdpPortsEqualField0 = 161, /* BCM56840 enhanced hash field. */
    bcmSwitchHashIP4TcpUdpPortsEqualField1 = 162, /* Selections for IPv4 TCP/UDP packets
                                           with source L4 port equals to
                                           destination L4 port. */
    bcmSwitchHashIP6Field0 = 163,       /* BCM56700/56800/56580 enhanced hash
                                           field. */
    bcmSwitchHashIP6Field1 = 164,       /* Selections for IPv6 packets. */
    bcmSwitchHashIP6TcpUdpField0 = 165, /* BCM56840 enhanced hash field. */
    bcmSwitchHashIP6TcpUdpField1 = 166, /* Selections for IPv6 TCP/UDP packets. */
    bcmSwitchHashIP6TcpUdpPortsEqualField0 = 167, /* BCM56840 enhanced hash field. */
    bcmSwitchHashIP6TcpUdpPortsEqualField1 = 168, /* Selections for IPv6 TCP/UDP packets
                                           with source L4 port equals to
                                           destination L4 port. */
    bcmSwitchHashL2Field0 = 169,        /* BCM56700/56800/56580 enhanced hash
                                           field. */
    bcmSwitchHashL2Field1 = 170,        /* Selections for L2 packets. */
    bcmSwitchHashMPLSField0 = 171,      /* BCM56700/56800/56580 enhanced hash
                                           field. */
    bcmSwitchHashMPLSField1 = 172,      /* Selections for MPLS packets. */
    bcmSwitchHashFCOEField0 = 173,      /* BCM56840 enhanced hash field. */
    bcmSwitchHashFCOEField1 = 174,      /* Selections for FCOE packets. */
    bcmSwitchHashL2TrillField0 = 175,   /* BCM56840 enhanced hash field. */
    bcmSwitchHashL2TrillField1 = 176,   /* Selections for L2 payload Trill
                                           packets. */
    bcmSwitchHashL3TrillField0 = 177,   /* BCM56840 enhanced hash field. */
    bcmSwitchHashL3TrillField1 = 178,   /* Selections for L3 payload Trill
                                           packets. */
    bcmSwitchHashTrillTunnelField0 = 179, /* BCM56840 enhanced hash field. */
    bcmSwitchHashTrillTunnelField1 = 180, /* Selections Trill tunnel packets. */
    bcmSwitchHashHG2UnknownField0 = 181, /* BCM56700/56800/56580 enhanced hash
                                           field. */
    bcmSwitchHashHG2UnknownField1 = 182, /* Selections for HiGig2 packets. */
    bcmSwitchHashField0OverlayCntagRpidEnable = 183, /* Selections for CNTAG, RPID fields. */
    bcmSwitchHashField1OverlayCntagRpidEnable = 184, /* Selections for CNTAG, RPID fields. */
    bcmSwitchHashHg2PktFieldsEnable = 185, /* Selections for HiGig2 packets. */
    bcmSwitchHashField0Ip6FlowLabel = 186, /* Selections for IPV6 packets. */
    bcmSwitchHashField1Ip6FlowLabel = 187, /* Selections for IPV6 packets. */
    bcmSwitchHashUseFlowSelTrunkUc = 188, /* Enable/Disable flow based hashing for
                                           Trunk. */
    bcmSwitchHashUseFlowSelEcmp = 189,  /* Enable/Disable flow based hashing for
                                           ECMP. */
    bcmSwitchTrunkHashSet0UnicastOffset = 190, /* BCM56700/56800/56580 enhanced hash
                                           bits. */
    bcmSwitchTrunkHashSet1UnicastOffset = 191, /* Selections for unicast trunking. */
    bcmSwitchTrunkHashSet0NonUnicastOffset = 192, /* BCM56700/56800/56580 enhanced hash
                                           bits. */
    bcmSwitchTrunkHashSet1NonUnicastOffset = 193, /* Selections for non-unicast trunking. */
    bcmSwitchTrunkFailoverHashOffset = 194, /* Enhanced hash selections for trunk
                                           failover. */
    bcmSwitchFabricTrunkHashSet0UnicastOffset = 195, /* BCM56700/56800/56580 enhanced hash
                                           bits. */
    bcmSwitchFabricTrunkHashSet1UnicastOffset = 196, /* Selections for unicast fabric
                                           trunking. */
    bcmSwitchFabricTrunkHashSet0NonUnicastOffset = 197, /* BCM56700/56800/56580 enhanced hash
                                           bits. */
    bcmSwitchFabricTrunkHashSet1NonUnicastOffset = 198, /* Selections for non-unicast fabric
                                           trunking. */
    bcmSwitchFabricTrunkFailoverHashOffset = 199, /* Enhanced hash selections for fabric
                                           trunk failover. */
    bcmSwitchFabricTrunkDynamicHashOffset = 200, /* Enhanced hash selections for fabric
                                           trunk dynamic load balancing. */
    bcmSwitchLoadBalanceHashSet0UnicastOffset = 201, /* BCM56700/56800/56580 enhanced hash
                                           bits. */
    bcmSwitchLoadBalanceHashSet1UnicastOffset = 202, /* Selections for unicast load
                                           balancing. */
    bcmSwitchLoadBalanceHashSet0NonUnicastOffset = 203, /* BCM56700/56800/56580 enhanced hash
                                           bits. */
    bcmSwitchLoadBalanceHashSet1NonUnicastOffset = 204, /* Selections for non-unicast load
                                           balancing. */
    bcmSwitchMacroFlowHashMinOffset = 205, /* Minimum offset value to be used in
                                           offset table. */
    bcmSwitchMacroFlowHashMaxOffset = 206, /* Maximum offset value to be used in
                                           offset table. */
    bcmSwitchMacroFlowHashStrideOffset = 207, /* Offset increment to be used for
                                           populating offset table. */
    bcmSwitchUniformUcastTrunkDistribution = 208, /* Uniform unicast trunk distribution on
                                           trunk with non power of 2 trunk
                                           ports. */
    bcmSwitchUniformFabricTrunkDistribution = 209, /* Uniform HiGig trunk distribution on
                                           trunk with non power of 2 trunk
                                           ports. */
    bcmSwitchECMPHashSet0Offset = 210,  /* BCM56700/56800/56580 enhanced hash
                                           bits. */
    bcmSwitchECMPHashSet1Offset = 211,  /* Selections for ECMP. */
    bcmSwitchCpuToCpuEnable = 212,      /* Enable copying packets from CMIC to
                                           CPU. */
    bcmSwitchIgmpUcastEnable = 213,     /* Enable unicast packets with IGMP
                                           payload. */
    bcmSwitchMldUcastEnable = 214,      /* Enable unicast packets with MLD
                                           payload. */
    bcmSwitchIgmpReservedMcastEnable = 215, /* IGMP packets with reserved MC address
                                           threated as IGMP. */
    bcmSwitchMldReservedMcastEnable = 216, /* MLD packets with reserved MC address
                                           threated as MLD. */
    bcmSwitchMldDirectAttachedOnly = 217, /* Stricter checks for MLD packets are
                                           enable. */
    bcmSwitchPortEgressBlockL2 = 218,   /* Apply egress mask port blocking on L2
                                           traffic. */
    bcmSwitchPortEgressBlockL3 = 219,   /* Apply egress mask port blocking on L3
                                           traffic. */
    bcmSwitchBpduInvalidVlanDrop = 220, /* BPDU packets are dropped upon invalid
                                           VLAN. */
    bcmSwitchMirrorInvalidVlanDrop = 221, /* Mirror-to-port packets are dropped
                                           upon invalid VLAN. */
    bcmSwitchMirrorPktChecksEnable = 222, /* Enable/Disable all packet checks for
                                           mirror packets. */
    bcmSwitchRemoteLearnTrust = 223,    /* Honor the DONOT_LEARN bit in HiGig
                                           header. */
    bcmSwitchSourceMacZeroDrop = 224,   /* Packets with MACSA of all zeros are
                                           dropped. */
    bcmSwitchIpmcGroupMtu = 225,        /* For ingress MTU check for all IPMC
                                           groups. */
    bcmSwitchModuleType = 226,          /* Set Switch module type. */
    bcmSwitchDeprecated227 = 227,       /* Deprecated. Do not use. */
    bcmSwitchDeprecated228 = 228,       /* Deprecated. Do not use. */
    bcmSwitchDeprecated229 = 229,       /* Deprecated. Do not use. */
    bcmSwitchL3EgressMode = 230,        /* Enable advanced egress management. */
    bcmSwitchL3DefipMultipathCountUpdate = 231, /* Enable control to update L3-DEFIP
                                           ECMP-COUNT */
    bcmSwitchL3IngressMode = 232,       /* Enable advanced Ingress-interface
                                           management. */
    bcmSwitchL3HostAsRouteReturnValue = 233, /* Enable control to return value, when
                                           prefix added to Route table during
                                           L3HostAdd. */
    bcmSwitchSourceModBlockUcast = 234, /* Apply source modid block on unicast
                                           packets only. */
    bcmSwitchSourceModBlockControlOpcode = 235, /* Apply source modid block on system
                                           control packets. */
    bcmSwitchEgressBlockUcast = 236,    /* Apply egress mask on unicast packets
                                           only. */
    bcmSwitchAlternateStoreForward = 237, /* Alternate Store and Forward. */
    bcmSwitchWarmBoot = 238,            /* Set Warm boot state. */
    bcmSwitchStableSelect = 239,        /* Select the storage (or stable)
                                           location for Level 2 Warm Boot. */
    bcmSwitchStableSize = 240,          /* Select the storage (or stable) size
                                           (bytes) for Level 2 Warm Boot. */
    bcmSwitchStableUsed = 241,          /* Query the storage (or stable) usage
                                           (bytes) for Level 2 Warm Boot. */
    bcmSwitchStableConsistent = 242,    /* Query the storage (or stable) for
                                           whether state is believed consistent
                                           with hardware (API completed prior to
                                           warmboot). */
    bcmSwitchControlSync = 243,         /* Force a sync of the Level 2 warm boot
                                           state on demand. */
    bcmSwitchControlAutoSync = 244,     /* Perform a sync of the Level 2 warm
                                           boot state after every API. */
    bcmSwitchCallbackAbortOnError = 245, /* Abort if a callback returns an error
                                           code. */
    bcmSwitchUnknownIpmcAsMcast = 246,  /* Threat unknown IPMC as L2 Mcast. */
    bcmSwitchTunnelUnknownIpmcDrop = 247, /* Drop unknown IPMC for incomming
                                           tunnel. */
    bcmSwitchDhcpLearn = 248,           /* Do not learn MACSA of DHCP packet. */
    bcmSwitchIpmcTtl1ToCpu = 249,       /* Copy L3 Mcast with TTL 1 to CPU. */
    bcmSwitchL3UcastTtl1ToCpu = 250,    /* Copy L3 Ucast with TTL 1 to CPU. */
    bcmSwitchDosAttackFlagZeroSeqZero = 251, /* Null scan. */
    bcmSwitchHgHdrExtLengthEnable = 252, /* Use HiGig+ HDR_EXT_LEN field. */
    bcmSwitchDosAttackSynFrag = 253,    /* Drop TCP SYN packet with source port
                                           0-1023 for the first segment. */
    bcmSwitchIp4McastL2DestCheck = 254, /* Check MACDA and IPv4 DIP address
                                           match. */
    bcmSwitchIp6McastL2DestCheck = 255, /* Check MACDA and IPv6 DIP address
                                           match. */
    bcmSwitchL3InterfaceUrpfEnable = 256, /* Enable URPF per VLAN/L3-IIF for
                                           unicast packets. */
    bcmSwitchL3TunnelUrpfMode = 257,    /* RPF mode for unicast tunneled
                                           packets. */
    bcmSwitchL3TunnelUrpfDefaultRoute = 258, /* Enable URPF default gateway check for
                                           unicast tunneled packets. */
    bcmSwitchL3UrpfFailToCpu = 259,     /* Enable trap URPF fail packets to CPU. */
    bcmSwitchL3UrpfRouteEnable = 260,   /* Enable route lookup for URPF check. */
    bcmSwitchL3UrpfRouteEnableExternal = 261, /* Enable External(only) route lookup
                                           for URPF check. */
    bcmSwitchL3UrpfMode = 262,          /* Port rpf mode for unicast packets. */
    bcmSwitchL3UrpfDefaultRoute = 263,  /* Enable URPF default gateway check. */
    bcmSwitchIngressRateLimitIncludeIFG = 264, /* Add bytes to ingress rate limiting
                                           for IFG. */
    bcmSwitchRateLimitLinear = 265,     /* Use linear limits for shaping and
                                           egress metering. */
    bcmSwitchHashL2Dual = 266,          /* Secondary Hash Select for L2. */
    bcmSwitchHashL3Dual = 267,          /* Secondary Hash Select for L3. */
    bcmSwitchHashDualMoveDepth = 268,   /* Maximum moves in dual hash mode. */
    bcmSwitchHashDualMoveDepthL2 = 269, /* Maximum moves in dual hash mode for
                                           L2 tables. */
    bcmSwitchHashDualMoveDepthL3 = 270, /* Maximum moves in dual hash mode for
                                           L3 tables. */
    bcmSwitchHashDualMoveDepthVlan = 271, /* Maximum moves in dual hash mode for
                                           vlan tables. */
    bcmSwitchHashDualMoveDepthMpls = 272, /* Maximum moves in dual hash mode for
                                           mpls tables. */
    bcmSwitchHashDualMoveDepthEgressVlan = 273, /* Maximum moves in dual hash mode for
                                           egress vlan tables. */
    bcmSwitchHashDualMoveDepthWlanPort = 274, /* Maximum moves in dual hash mode for
                                           WLAN port table. */
    bcmSwitchHashDualMoveDepthWlanClient = 275, /* Maximum moves in dual hash mode for
                                           WLAN client table. */
    bcmSwitchHashMultiMoveDepth = 276,  /* Maximum moves in multi hash mode. */
    bcmSwitchHashMultiMoveDepthL2 = 277, /* Maximum moves in multi hash mode for
                                           L2 tables. */
    bcmSwitchHashMultiMoveDepthL3 = 278, /* Maximum moves in multi hash mode for
                                           L3 tables. */
    bcmSwitchHashMultiMoveDepthVlan = 279, /* Maximum moves in multi hash mode for
                                           vlan tables. */
    bcmSwitchHashMultiMoveDepthMpls = 280, /* Maximum moves in multi hash mode for
                                           mpls tables. */
    bcmSwitchHashMultiMoveDepthEgressVlan = 281, /* Maximum moves in multi hash mode for
                                           egress vlan tables. */
    bcmSwitchHashL2DualLeastFull = 282, /* Use least full bank in dual hash L2
                                           tables. */
    bcmSwitchHashL3DualLeastFull = 283, /* Use least full bank in dual hash L3
                                           tables. */
    bcmSwitchHashMPLSDualLeastFull = 284, /* Use least full bank in dual hash MPLS
                                           tables. */
    bcmSwitchSharedVlanEnable = 285,    /* Enable shared vlan. */
    bcmSwitchSharedVlanL2McastEnable = 286, /* Use shared vlan for L2 mcast pkt. */
    bcmSwitchIpmcReplicationSharing = 287, /* IPMC replication uses lists. */
    bcmSwitchIpmcReplicationAvailabilityThreshold = 288, /* IPMC replication table available
                                           space threshold. */
    bcmSwitchVrfMax = 289,              /* Maximum VRF ID for the device. */
    bcmSwitchFailoverStackTrunk = 290,  /* Trunk failover support for Stack
                                           ports. */
    bcmSwitchFailoverEtherTrunk = 291,  /* Trunk failover support for Ether
                                           ports. */
    bcmSwitchClassBasedMoveFailPktToCpu = 292, /* Class based check fail pkt to CPU. */
    bcmSwitchClassBasedMoveFailPktDrop = 293, /* Drop class based check fail pkt. */
    bcmSwitchHgHdrMcastFlood = 294,     /* Flood control value in HiGig Header. */
    bcmSwitchHgHdrMcastFloodOverride = 295, /* Use flood control value in HiGig
                                           header. */
    bcmSwitchHgHdrIpMcastFlood = 296,   /* IPMC flood control value in HiGig
                                           header. */
    bcmSwitchHgHdrIpMcastFloodOverride = 297, /* Use IPMC flood ctrl value in HiGig
                                           header. */
    bcmSwitchSTPBlockedFieldBypass = 298, /* Disable FP when STP state is blocked. */
    bcmSwitchL2PortBlocking = 299,      /* Enable selective port blocking per
                                           MAC address. */
    bcmSwitchHashIpfixIngress = 300,    /* Hash Select for Ingress IPFIX. */
    bcmSwitchHashIpfixIngressDual = 301, /* Secondary Hash Select for Ingress
                                           IPFIX. */
    bcmSwitchHashIpfixEgress = 302,     /* Hash Select for Egress IPFIX. */
    bcmSwitchHashIpfixEgressDual = 303, /* Secondary Hash Select for Egress
                                           IPFIX. */
    bcmSwitchHashMPLS = 304,            /* Hash Select for MPLS entry. */
    bcmSwitchHashMPLSDual = 305,        /* Secondary Hash Select for MPLS entry. */
    bcmSwitchForceForwardFabricTrunk = 306, /* Fabric trunk resolution in force
                                           forward mode. */
    bcmSwitchUseGport = 307,            /* Enable GPORT format for
                                           destination/source representation. */
    bcmSwitchHgHdrMcastVlanRange = 308, /* HIGIG2 broadcast index range */
    bcmSwitchHgHdrMcastL2Range = 309,   /* HIGIG2 multicast index range */
    bcmSwitchHgHdrMcastL3Range = 310,   /* HIGIG2 IPMC index range */
    bcmSwitchMcastL2Range = 311,        /* Multicast index range */
    bcmSwitchMcastL3Range = 312,        /* IPMC index range */
    bcmSwitchHashWlanPort = 313,        /* Hash Select for Wlan SVP entry. */
    bcmSwitchHashWlanPortDual = 314,    /* Secondary hash Select for Wlan SVP
                                           entry. */
    bcmSwitchHashWlanClient = 315,      /* Hash select for Wlan client database
                                           entry */
    bcmSwitchHashWlanClientDual = 316,  /* Secondary hash select for Wlan client
                                           database entry */
    bcmSwitchWlanClientAuthorizeAll = 317, /* Authorize all wlan clients, disable
                                           wlan client lookup */
    bcmSwitchWlanClientUnauthToCpu = 318, /* Unauthorized WLAN client's packets to
                                           CPU. */
    bcmSwitchWlanClientRoamedOutErrorToCpu = 319, /* Roamed out WLAN client's packets to
                                           CPU. */
    bcmSwitchWlanClientSrcMacMissToCpu = 320, /* WLAN client with source MAC miss:
                                           packets to CPU. */
    bcmSwitchWlanClientDstMacMissToCpu = 321, /* WLAN client with destination MAC
                                           miss: packets to CPU. */
    bcmSwitchOamHeaderErrorToCpu = 322, /* Packets with error in the OAM header
                                           to CPU. */
    bcmSwitchOamUnknownVersionToCpu = 323, /* Packets with unknown OAM version to
                                           CPU. */
    bcmSwitchOamUnknownVersionDrop = 324, /* Packets with unknown OAM version
                                           Dropped. */
    bcmSwitchOamUnexpectedPktToCpu = 325, /* OAM Packets with unexpected MDL to
                                           CPU. */
    bcmSwitchOamCcmToCpu = 326,         /* CCM with unexpected MEPID or Interval
                                           to CPU. */
    bcmSwitchOamXconCcmToCpu = 327,     /* CCM with low MDL or unexpected MAID
                                           to CPU. */
    bcmSwitchOamXconOtherToCpu = 328,   /* Other than CCM OAM PDUs with low MDL
                                           to CPU. */
    bcmSwitchL3SrcBindFailToCpu = 329,  /* Packets with L3 source bind table
                                           miss to CPU. */
    bcmSwitchTunnelIp4IdShared = 330,   /* Share the IPv4 ID space across
                                           tunnels. */
    bcmSwitchHashRegexAction = 331,     /* Hash Select for Regex Action entries. */
    bcmSwitchHashRegexActionDual = 332, /* Secondary hash Select for Regex
                                           Action entries. */
    bcmSwitchDeprecated333 = 333,       /* Deprecated. Do not use. */
    bcmSwitchCpuCopyDestination = 334,  /* Destination for CPU bound packets. */
    bcmSwitchCpuProtoTimeSyncPrio = 335, /* Priority for Time-Sync packets to
                                           CPU. */
    bcmSwitchCpuProtoMmrpPrio = 336,    /* Priority for MMRP packets to CPU. */
    bcmSwitchCpuProtoSrpPrio = 337,     /* Priority for SRP packets to CPU. */
    bcmSwitchTimeSyncPktToCpu = 338,    /* Time-Sync protocol packets to CPU. */
    bcmSwitchTimeSyncPktDrop = 339,     /* Time-Sync protocol packets dropped. */
    bcmSwitchTimeSyncPktFlood = 340,    /* Time-Sync protocol packets flooded to
                                           VLAN. */
    bcmSwitchMmrpPktToCpu = 341,        /* MMRP protocol packets to CPU. */
    bcmSwitchMmrpPktDrop = 342,         /* MMRP protocol packets dropped. */
    bcmSwitchMmrpPktFlood = 343,        /* MMRP protocol packets flooded to
                                           VLAN. */
    bcmSwitchSrpPktToCpu = 344,         /* SRP protocol packets to CPU. */
    bcmSwitchSrpPktDrop = 345,          /* SRP protocol packets dropped. */
    bcmSwitchSrpPktFlood = 346,         /* SRP protocol packets flooded to VLAN. */
    bcmSwitchSRPEthertype = 347,        /* Set SRP protocol EtherType to match. */
    bcmSwitchMMRPEthertype = 348,       /* Set MMRP EtherType to match. */
    bcmSwitchTimeSyncEthertype = 349,   /* Set Timing and Sync protocol
                                           EtherType to match. */
    bcmSwitchFcoeEtherType = 350,       /* Set FCOE protocol EtherType to match. */
    bcmSwitchSRPDestMacOui = 351,       /* Set SRP protocol MAC upper 3 bytes to
                                           match. */
    bcmSwitchMMRPDestMacOui = 352,      /* Set MMRP protocol MAC upper 3 bytes
                                           to match. */
    bcmSwitchTimeSyncDestMacOui = 353,  /* Set Timnig and Sync protocol MAC
                                           upper 3 bytes to match. */
    bcmSwitchSRPDestMacNonOui = 354,    /* Set SRP protocol MAC lower 3 bytes to
                                           match. */
    bcmSwitchMMRPDestMacNonOui = 355,   /* Set MMRP protocol MAC lower 3 bytes
                                           to match. */
    bcmSwitchTimeSyncDestMacNonOui = 356, /* Set Timnig and Sync protocol MAC lowe
                                           3 bytes to match. */
    bcmSwitchTimeSyncMessageTypeBitmap = 357, /* Set Timnig and Sync protocol message
                                           bitmap to match. */
    bcmSwitchTimeSyncClassAPktPrio = 358, /* Set Priority for the Class A. */
    bcmSwitchTimeSyncClassBPktPrio = 359, /* Set Priority for the Class B. */
    bcmSwitchTimeSyncClassAExeptionPktPrio = 360, /* Set Remapping Priority for exeption
                                           packets in the Class
                                           A. */
    bcmSwitchTimeSyncClassBExeptionPktPrio = 361, /* Set Remapping Priority for exeption
                                           packets in the Class B. */
    bcmSwitchL2McastAllRouterPortsAddEnable = 362, /* Add all-router ports to every L2
                                           multicast entry. */
    bcmSwitchBypassMode = 363,          /* Omit some switching features to
                                           decrease traffic latency. */
    bcmSwitchIpmcSameVlanL3Route = 364, /* Enable forwarding of known IPMC
                                           packets to the incoming vlan . */
    bcmSwitchDeprecated365 = 365,       /* Deprecated. Do not use. */
    bcmSwitchFieldCache = 366,          /* Enable caching of Rule table or field
                                           entries */
    bcmSwitchFieldCommit = 367,         /* Flush the cached Rule table or field
                                           entries to hardware */
    bcmSwitchIpmcCache = 368,           /* Enable caching of IP Multicast S/G
                                           entries */
    bcmSwitchIpmcCommit = 369,          /* Flush the cached IP Multicast table
                                           entries to hardware */
    bcmSwitchL2Cache = 370,             /* Enable caching of MAC address entries */
    bcmSwitchL2Commit = 371,            /* Flush the cached MAC address table to
                                           hardware */
    bcmSwitchL2AgeDelete = 372,         /* Enables deletition of MAC addresses
                                           during aging scan */
    bcmSwitchL2AgeScan = 373,           /* Scan all MAC addresses and decrement
                                           age-status */
    bcmSwitchL3HostCache = 374,         /* Enable caching of L3 Host table
                                           entries */
    bcmSwitchL3HostCommit = 375,        /* Flush the cached L3 Host table to
                                           hardware */
    bcmSwitchL3RouteCache = 376,        /* Enable caching of L3 Route table
                                           entries */
    bcmSwitchL3RouteCommit = 377,       /* Flush the cached L3 Route table to
                                           hardware */
    bcmSwitchL2InvalidCtlToCpu = 378,   /* Packets with invalid L2 control
                                           fields to CPU. */
    bcmSwitchInvalidGreToCpu = 379,     /* Packets with invalid GRE fields to
                                           CPU */
    bcmSwitchHashL2MPLSField0 = 380,    /* L2 MPLS enhanced hash field */
    bcmSwitchHashL2MPLSField1 = 381,    /* L2 MPLS enhanced hash field */
    bcmSwitchHashL3MPLSField0 = 382,    /* L3 MPLS enhanced hash field */
    bcmSwitchHashL3MPLSField1 = 383,    /* L3 MPLS enhanced hash field */
    bcmSwitchHashMPLSTunnelField0 = 384, /* MPLS tunnel enhanced hash field */
    bcmSwitchHashMPLSTunnelField1 = 385, /* MPLS tunnel enhanced hash field */
    bcmSwitchHashMIMTunnelField0 = 386, /* MIM tunnel enhanced hash field */
    bcmSwitchHashMIMTunnelField1 = 387, /* MIM tunnel enhanced hash field */
    bcmSwitchHashMIMField0 = 388,       /* MIM enhanced hash field */
    bcmSwitchHashMIMField1 = 389,       /* MIM enhanced hash field */
    bcmSwitchStgInvalidToCpu = 390,     /* Packets with STG state of egress port
                                           is not in forwarding state to CPU */
    bcmSwitchVlanTranslateEgressMissToCpu = 391, /* Packets with VLAN translation talbe
                                           missed when it is expected to hit to
                                           CPU. */
    bcmSwitchL3PktErrToCpu = 392,       /* Packets with IP length field is
                                           longer than the actual packet length
                                           to CPU. */
    bcmSwitchMtuFailureToCpu = 393,     /* Packets with L2 MTU Failure to CPU. */
    bcmSwitchSrcKnockoutToCpu = 394,    /* Packets with Split horizon or SVP
                                           pruning (SVP==DVP) failure to CPU. */
    bcmSwitchWlanTunnelMismatchToCpu = 395, /* Packets with incoming WLAN SVP did
                                           not arrive on the expected tunnel to
                                           CPU. */
    bcmSwitchWlanTunnelMismatchDrop = 396, /* Drop packets with incoming WLAN SVP
                                           did not arrive on the expected
                                           tunnel. */
    bcmSwitchWlanPortMissToCpu = 397,   /* WLAN packets that have a SVP miss to
                                           CPU. */
    bcmSwitchUnknownVlanToCpuCosq = 398, /* VLAN copy to CPU COS. */
    bcmSwitchStgInvalidToCpuCosq = 399, /* STG copy to CPU COS. */
    bcmSwitchVlanTranslateEgressMissToCpuCosq = 400, /* VLAN translate miss copy to CPU COS. */
    bcmSwitchTunnelErrToCpuCosq = 401,  /* Tunnel Error to CPU COS. */
    bcmSwitchL3HeaderErrToCpuCosq = 402, /* L3 Header Error to CPU COS. */
    bcmSwitchL3PktErrToCpuCosq = 403,   /* L3 Pkt Error to CPU COS. */
    bcmSwitchIpmcTtlErrToCpuCosq = 404, /* TTL drop copy to CPU COS. */
    bcmSwitchMtuFailureToCpuCosq = 405, /* MTU failure copy to CPU COS. */
    bcmSwitchHgHdrErrToCpuCosq = 406,   /* HIGIG error copy to CPU COS. */
    bcmSwitchSrcKnockoutToCpuCosq = 407, /* Split horizon or SVP failure to CPU
                                           COS. */
    bcmSwitchWlanTunnelMismatchToCpuCosq = 408, /* WLAN move to CPU COS. */
    bcmSwitchWlanPortMissToCpuCosq = 409, /* WLAN SVP miss to CPU COS. */
    bcmSwitchIpfixRateViolationDataInsert = 410, /* Allow insert when rate violation is
                                           detected. */
    bcmSwitchIpfixRateViolationPersistent = 411, /* Keep rate violation status
                                           persistent. */
    bcmSwitchHashVlanTranslate = 412,   /* Hash Select for VLAN translate. */
    bcmSwitchHashVlanTranslateDual = 413, /* Secondary Hash Select for VLAN
                                           translate. */
    bcmSwitchHashEgressVlanTranslate = 414, /* Hash Select for Egress VLAN
                                           translate. */
    bcmSwitchHashEgressVlanTranslateDual = 415, /* Secondary Hash Select for Egress VLAN
                                           translate. */
    bcmSwitchLayeredQoSResolution = 416, /* Enable layered QoS resolution mode. */
    bcmSwitchCustomerQueuing = 417,     /* Enable customer domain queuing. */
    bcmSwitchCosqStatThreshold = 418,   /* Sets Statistics Threshold to ACE-TCE
                                           Mode */
    bcmSwitchCosqStatInterval = 419,    /* Sets background ejection Statistics
                                           Interval */
    bcmSwitchEncapErrorToCpu = 420,     /* Encapsulation Error to CPU */
    bcmSwitchMplsPortIndependentLowerRange1 = 421, /* Port Independent Lower MPLS Label for
                                           Range-1 */
    bcmSwitchMplsPortIndependentUpperRange1 = 422, /* Port Independent Upper MPLS Label for
                                           Range-1 */
    bcmSwitchMplsPortIndependentLowerRange2 = 423, /* Port Independent Lower MPLS Label for
                                           Range-2 */
    bcmSwitchMplsPortIndependentUpperRange2 = 424, /* Port Independent Upper MPLS Label for
                                           Range-2 */
    bcmSwitchMirrorEgressTrueColorSelect = 425, /* Color override selection for egress
                                           true mirroring */
    bcmSwitchMirrorEgressTruePriority = 426, /* Priority override selection for
                                           egress true mirroring */
    bcmSwitchPFCClass0Queue = 427,      /* Queue to suspend when a Priority Flow
                                           Control class 0 packet is received. */
    bcmSwitchPFCClass1Queue = 428,      /* Queue to suspend when a Priority Flow
                                           Control class 1 packet is received. */
    bcmSwitchPFCClass2Queue = 429,      /* Queue to suspend when a Priority Flow
                                           Control class 2 packet is received. */
    bcmSwitchPFCClass3Queue = 430,      /* Queue to suspend when a Priority Flow
                                           Control class 3 packet is received. */
    bcmSwitchPFCClass4Queue = 431,      /* Queue to suspend when a Priority Flow
                                           Control class 4 packet is received. */
    bcmSwitchPFCClass5Queue = 432,      /* Queue to suspend when a Priority Flow
                                           Control class 5 packet is received. */
    bcmSwitchPFCClass6Queue = 433,      /* Queue to suspend when a Priority Flow
                                           Control class 6 packet is received. */
    bcmSwitchPFCClass7Queue = 434,      /* Queue to suspend when a Priority Flow
                                           Control class 7 packet is received. */
    bcmSwitchPFCClass0McastQueue = 435, /* Multicast queue to suspend when a
                                           Priority Flow Control class 0 packet
                                           is received. */
    bcmSwitchPFCClass1McastQueue = 436, /* Multicast queue to suspend when a
                                           Priority Flow Control class 1 packet
                                           is received. */
    bcmSwitchPFCClass2McastQueue = 437, /* Multicast queue to suspend when a
                                           Priority Flow Control class 2 packet
                                           is received. */
    bcmSwitchPFCClass3McastQueue = 438, /* Multicast queue to suspend when a
                                           Priority Flow Control class 3 packet
                                           is received. */
    bcmSwitchPFCClass4McastQueue = 439, /* Multicast queue to suspend when a
                                           Priority Flow Control class 4 packet
                                           is received. */
    bcmSwitchPFCClass5McastQueue = 440, /* Multicast queue to suspend when a
                                           Priority Flow Control class 5 packet
                                           is received. */
    bcmSwitchPFCClass6McastQueue = 441, /* Multicast queue to suspend when a
                                           Priority Flow Control class 6 packet
                                           is received. */
    bcmSwitchPFCClass7McastQueue = 442, /* Multicast queue to suspend when a
                                           Priority Flow Control class 7 packet
                                           is received. */
    bcmSwitchPFCClass0DestmodQueue = 443, /* Destination module queue to suspend
                                           when a Priority Flow Control class 0
                                           packet is received. */
    bcmSwitchPFCClass1DestmodQueue = 444, /* Destination module queue to suspend
                                           when a Priority Flow Control class 1
                                           packet is received. */
    bcmSwitchPFCClass2DestmodQueue = 445, /* Destination module queue to suspend
                                           when a Priority Flow Control class 2
                                           packet is received. */
    bcmSwitchPFCClass3DestmodQueue = 446, /* Destination module queue to suspend
                                           when a Priority Flow Control class 3
                                           packet is received. */
    bcmSwitchPFCClass4DestmodQueue = 447, /* Destination module queue to suspend
                                           when a Priority Flow Control class 4
                                           packet is received. */
    bcmSwitchPFCClass5DestmodQueue = 448, /* Destination module queue to suspend
                                           when a Priority Flow Control class 5
                                           packet is received. */
    bcmSwitchPFCClass6DestmodQueue = 449, /* Destination module queue to suspend
                                           when a Priority Flow Control class 6
                                           packet is received. */
    bcmSwitchPFCClass7DestmodQueue = 450, /* Destination module queue to suspend
                                           when a Priority Flow Control class 7
                                           packet is received. */
    bcmSwitchPFCQueue0Class = 451,      /* Priority Flow Control class for COS
                                           queue 0. */
    bcmSwitchPFCQueue1Class = 452,      /* Priority Flow Control class for COS
                                           queue 1. */
    bcmSwitchPFCQueue2Class = 453,      /* Priority Flow Control class for COS
                                           queue 2. */
    bcmSwitchPFCQueue3Class = 454,      /* Priority Flow Control class for COS
                                           queue 3. */
    bcmSwitchPFCQueue4Class = 455,      /* Priority Flow Control class for COS
                                           queue 4. */
    bcmSwitchPFCQueue5Class = 456,      /* Priority Flow Control class for COS
                                           queue 5. */
    bcmSwitchPFCQueue6Class = 457,      /* Priority Flow Control class for COS
                                           queue 6. */
    bcmSwitchPFCQueue7Class = 458,      /* Priority Flow Control class for COS
                                           queue 7. */
    bcmSwitchReserveLowL3InterfaceId = 459, /* Set the lower inclusive bounds of the
                                           L3 Interfaces.  Get will return the
                                           lowest value supported by the device.
                                            Setting 0 will remove the reserved
                                           range. */
    bcmSwitchReserveHighL3InterfaceId = 460, /* Set the higher inclusive bounds of
                                           the L3 Interfaces.  Get will return
                                           the lowest value supported by the
                                           device.  Setting 0 will remove the
                                           reserved range. */
    bcmSwitchReserveLowL3EgressId = 461, /* Set the lower inclusive bounds of the
                                           L3 Egress object.  Get will return
                                           the lowest value supported by the
                                           device.  Setting 0 will remove the
                                           reserved range. */
    bcmSwitchReserveHighL3EgressId = 462, /* Set the higher inclusive bounds of
                                           the L3 Egress object.  Get will
                                           return the lowest value supported by
                                           the device.  Setting 0 will remove
                                           the reserved range. */
    bcmSwitchReserveLowVlanPort = 463,  /* Set the lower inclusive bounds of the
                                           vlan gport resources.  Get will
                                           return the lowest value supported by
                                           the device.  Setting 0 will remove
                                           the reserved range */
    bcmSwitchReserveHighVlanPort = 464, /* Set the highest inclusive bounds of
                                           the vlan gport resources.  Get will
                                           return the highest value supported by
                                           the device.  Setting 0 will remove
                                           the reserved range */
    bcmSwitchReserveLowMplsPort = 465,  /* Set the lower inclusive bounds of the
                                           mpls gport resources.  Get will
                                           return the lowest value supported by
                                           the device.  Setting 0 will remove
                                           the reserved range */
    bcmSwitchReserveHighMplsPort = 466, /* Set the highest inclusive bounds of
                                           the mpls gport resources.  Get will
                                           return the highest value supported by
                                           the device.  Setting 0 will remove
                                           the reserved range */
    bcmSwitchReserveLowMimPort = 467,   /* Set the lower inclusive bounds of the
                                           mim gport resources.  Get will return
                                           the lowest value supported by the
                                           device.  Setting 0 will remove the
                                           reserved range */
    bcmSwitchReserveHighMimPort = 468,  /* Set the highest inclusive bounds of
                                           the mim gport resources.  Get will
                                           return the highest value supported by
                                           the device.  Setting 0 will remove
                                           the reserved range */
    bcmSwitchReserveLowEncap = 469,     /* Set the lower inclusive bounds of the
                                           Encap ID resources used internally by
                                           gports.  Get will return the lowest
                                           value supported by the device. 
                                           Setting 0 will remove the reserved
                                           range */
    bcmSwitchReserveHighEncap = 470,    /* Set the highest inclusive bounds of
                                           the Encap ID resources used
                                           internally by gports.  Get will
                                           return the highest value supported by
                                           the device.  Setting 0 will remove
                                           the reserved range */
    bcmSwitchReserveLowL2Egress = 471,  /* Set the lower inclusive bounds of the
                                           L2 Encap ID resources.  Get will
                                           return the lowest value supported by
                                           the device.  Setting 0 will remove
                                           the reserved range */
    bcmSwitchReserveHighL2Egress = 472, /* Set the highest inclusive bounds of
                                           the L2 Encap ID resources.  Get will
                                           return the highest value supported by
                                           the device.  Setting 0 will remove
                                           the reserved range */
    bcmSwitchReserveLowVpn = 473,       /* Set the lower inclusive bounds of the
                                           VPN ID resources.  Get will return
                                           the lowest value supported by the
                                           device.  Setting 0 will remove the
                                           reserved range */
    bcmSwitchReserveHighVpn = 474,      /* Set the highest inclusive bounds of
                                           the VPN ID resources.  Get will
                                           return the highest value supported by
                                           the device.  Setting 0 will remove
                                           the reserved range */
    bcmSwitchReserveLowFailoverId = 475, /* Set the lower inclusive bounds of
                                           failover object.  Get will return the
                                           lowest value supported by the device.
                                            Setting 0 will remove the reserved
                                           range */
    bcmSwitchReserveHighFailoverId = 476, /* Set the higher inclusive bounds of
                                           failover object.  Get will return the
                                           lowest value supported by the device.
                                            Setting 0 will remove the reserved
                                           range */
    bcmSwitchReserveLowOamEndPointId = 477, /* Set the lower inclusive bounds of OAM
                                           endpoint.  Get will return the lowest
                                           value supported by the device. 
                                           Setting 0 will remove the reserved
                                           range */
    bcmSwitchReserveHighOamEndPointId = 478, /* Set the higher inclusive bounds of
                                           OAM endpoin.  Get will return the
                                           lowest value supported by the device.
                                            Setting 0 will remove the reserved
                                           range */
    bcmSwitchReserveLowOamGroupId = 479, /* Set the lower inclusive bounds of OAM
                                           Group.  Get will return the lowest
                                           value supported by the device. 
                                           Setting 0 will remove the reserved
                                           range */
    bcmSwitchReserveHighOamGroupId = 480, /* Set the higher inclusive bounds of
                                           OAM Group.  Get will return the
                                           lowest value supported by the device.
                                            Setting 0 will remove the reserved
                                           range */
    bcmSwitchSnapNonZeroOui = 481,      /* Decode SNAP packets with non-zero OUI
                                           for protocol-based VLAN */
    bcmSwitchSynchronousPortClockSource = 482, /* Set ITU-T G.8261/8262 synchronous
                                           Ethernet recovered clock source. 
                                           Setting -1 selects in free-run mode
                                           (no recovered clock). Get returns the
                                           current value. */
    bcmSwitchSynchronousPortClockSourceBkup = 483, /* Set ITU-T G.8261/8262 synchronous
                                           Ethernet recovered backup clock
                                           source.  Get returns the current
                                           value. */
    bcmSwitchSynchronousPortClockSourceDivCtrl = 484, /* Set ITU-T G.8261/8262 synchronous
                                           Ethernet recovered clock source. 
                                           Setting -1 selects the divide by five
                                           logic. Get returns the current value. */
    bcmSwitchSynchronousPortClockSourceBkupDivCtrl = 485, /* Set ITU-T G.8261/8262 synchronous
                                           Ethernet recovered backup clock
                                           source.  Setting -1 selects the
                                           divide by five logic. Get returns the
                                           current value. */
    bcmSwitchL2HitClear = 486,          /* Clear L2 source and destination hit
                                           bits when set to 1 */
    bcmSwitchL2SrcHitClear = 487,       /* Clear L2 source hit bits when set to
                                           1 */
    bcmSwitchL2DstHitClear = 488,       /* Clear L2 destination hit bits when
                                           set to 1 */
    bcmSwitchL3HostHitClear = 489,      /* Clear L3 host hit bits when set to 1 */
    bcmSwitchL3RouteHitClear = 490,     /* Clear L3 route hit bits when set to 1 */
    bcmSwitchIpmcSameVlanPruning = 491, /* Allow both L2 and L3 copies of a
                                           packet to go out on a physical port
                                           if it happens to be a member of both
                                           the L2 and L3 bitmaps of the
                                           replication group. */
    bcmSwitchDeprecated492 = 492,       /* Deprecated. Do not use. */
    bcmSwitchDeprecated493 = 493,       /* Deprecated. Do not use. */
    bcmSwitchL2McIdxRetType = 494,      /* When set to 1, L2 multicast API will
                                           return multicast index as
                                           bcm_multicast_t instead of hardware
                                           multicast index. */
    bcmSwitchL3McIdxRetType = 495,      /* When set to 1, IP multicast API will
                                           return multicast index as
                                           bcm_multicast_t instead of hardware
                                           multicast index. */
    bcmSwitchDeprecated496 = 496,       /* Deprecated. Do not use. */
    bcmSwitchDeprecated497 = 497,       /* Deprecated. Do not use. */
    bcmSwitchDeprecated498 = 498,       /* Deprecated. Do not use. */
    bcmSwitchDeprecated499 = 499,       /* Deprecated. Do not use. */
    bcmSwitchDeprecated500 = 500,       /* Deprecated. Do not use. */
    bcmSwitchArpDhcpToCpu = 501,        /* Set both ARP and DHCP to CPU. */
    bcmSwitchIgmpMldToCpu = 502,        /* Set the snooping mode on both IGMP
                                           and MLD together. */
    bcmSwitchLinkDownInfoSkip = 503,    /* Skip port information gathering
                                           during linkdown event
                                           notification */
    bcmSwitchDeprecated504 = 504,       /* Deprecated. Do not use. */
    bcmSwitchL2SourceDiscardMoveToCpu = 505, /* SRC MAC discard configuration to CPU */
    bcmSwitchNivEthertype = 506,        /* Set EtherType field in VNTAG */
    bcmSwitchNivPrioDropToCpu = 507,    /* NIV priority admittance drop to CPU */
    bcmSwitchNivInterfaceMissToCpu = 508, /* NIV forwarding lookup miss to CPU */
    bcmSwitchNivRpfFailToCpu = 509,     /* NIV RPF check fail to CPU */
    bcmSwitchNivTagInvalidToCpu = 510,  /* Invalid VNTAG to CPU */
    bcmSwitchNivTagDropToCpu = 511,     /* VNTAG present to CPU */
    bcmSwitchNivUntagDropToCpu = 512,   /* VNTAG not present to CPU */
    bcmSwitchEEEPipelineTime = 513,     /* Time (in system clock cycles) for MAC
                                           to ensure that the egress pipeline is
                                           empty prior to the assertion of the
                                           LPI control signal. */
    bcmSwitchEEEGlobalCongestionThreshold = 514, /* If this threshold is exceeded, EEE is
                                           disabled for all ports. */
    bcmSwitchDeprecated515 = 515,       /* Deprecated. Do not use. */
    bcmSwitchDeprecated516 = 516,       /* Deprecated. Do not use. */
    bcmSwitchDeprecated517 = 517,       /* Deprecated. Do not use. */
    bcmSwitchDeprecated518 = 518,       /* Deprecated. Do not use. */
    bcmSwitchDeprecated519 = 519,       /* Deprecated. Do not use. */
    bcmSwitchDeprecated520 = 520,       /* Deprecated. Do not use. */
    bcmSwitchDeprecated521 = 521,       /* Deprecated. Do not use. */
    bcmSwitchRemoteCpuSchanEnable = 522, /* Enables SCHAN requests for Remote CPU
                                           feature */
    bcmSwitchRemoteCpuFromCpuEnable = 523, /* Enables recieving packets from CPU
                                           for Remote CPU feature */
    bcmSwitchRemoteCpuToCpuEnable = 524, /* Enables sending packet to CPU for
                                           Remote CPU feature */
    bcmSwitchRemoteCpuCmicEnable = 525, /* Enables CMIC requests for Remote CPU
                                           feature */
    bcmSwitchRemoteCpuMatchLocalMac = 526, /* Local MAC addresses matching for
                                           Remote CPU feature */
    bcmSwitchRemoteCpuMatchVlan = 527,  /* VLAN matching for Remote CPU feature */
    bcmSwitchRemoteCpuVlan = 528,       /* VLAN tag to be inserted into Remote
                                           CPU packets */
    bcmSwitchRemoteCpuTpid = 529,       /* TPID tag to be inserted into Remote
                                           CPU packets */
    bcmSwitchRemoteCpuSignature = 530,  /* Unique Signature for Remote CPU
                                           packets */
    bcmSwitchRemoteCpuForceScheduling = 531, /* Remote CPU packets are forced to have
                                           COS value */
    bcmSwitchRemoteCpuToCpuDestPortAllReasons = 532, /* Remote CPU packets to be forwarded
                                           for all directed reasons */
    bcmSwitchRemoteCpuToCpuDestMacAllReasons = 533, /* Remote CPU packets to be forwarded
                                           for all reasons */
    bcmSwitchRxRedirectPktReasons = 534, /* All forwarding reasons for normal
                                           packets */
    bcmSwitchRxRedirectPktReasonsExtended = 535, /* All extended forwarding reasons for
                                           normal packets */
    bcmSwitchRxRedirectHigigPktReasons = 536, /* All forwarding reasons for higig
                                           packets */
    bcmSwitchRxRedirectHigigPktReasonsExtended = 537, /* All extended forwarding reasons for
                                           higig packets */
    bcmSwitchRxRedirectTruncatedPktReasons = 538, /* All forwarding reasons for truncated
                                           packets */
    bcmSwitchRxRedirectTruncatedPktReasonsExtended = 539, /* All extended forwarding reasons for
                                           truncated packets */
    bcmSwitchRxRedirectPktCos = 540,    /* All forwarding COS queue bitmap for
                                           normal packets */
    bcmSwitchRxRedirectPktCosExtended = 541, /* All extended forwarding COS queue
                                           bitmap for normal packets */
    bcmSwitchRemoteCpuLocalMacOui = 542, /* Higher 3 bytes of Local MAC address
                                           used for Remote CPU */
    bcmSwitchRemoteCpuLocalMacNonOui = 543, /* Lower 3 bytes of Local MAC address
                                           used for Remote CPU */
    bcmSwitchRemoteCpuDestMacOui = 544, /* Higher 3 bytes of Destination MAC
                                           address used for Remote CPU */
    bcmSwitchRemoteCpuDestMacNonOui = 545, /* Lower 3 bytes of Destination MAC
                                           address used for Remote CPU */
    bcmSwitchRemoteCpuEthertype = 546,  /* Remote CPU EtherType */
    bcmSwitchRemoteCpuDestPort = 547,   /* Remote CPU will direct raw data
                                           packets with SOBMH header to a given
                                           port */
    bcmSwitchRemoteCpuHigigDestPort = 548, /* Remote CPU will direct raw data
                                           packets with HiGig header to a given
                                           port */
    bcmSwitchRemoteCpuTcMappingMacOui = 549, /* Higher 3 bytes of Destination MAC
                                           address used to match received ToCPU
                                           packets */
    bcmSwitchRemoteCpuTcMappingMacNonOui = 550, /* Lower 3 bytes of Destination MAC
                                           address used to match received ToCPU
                                           packets */
    bcmSwitchRemoteCpuTcMappingEthertype = 551, /* EtherType used to match received
                                           ToCPU packets */
    bcmSwitchServiceTpidReplace = 552,  /* Replace TPID in Service Delimiting
                                           SD-TAG */
    bcmSwitchEgressPktAge = 553,        /* Chip-wide egress packet age time,
                                           msec. */
    bcmSwitchFabricTrunkDynamicSampleRate = 554, /* Number of times average port egressed
                                           bytes and queued bytes are computed
                                           in a second */
    bcmSwitchFabricTrunkDynamicEgressBytesExponent = 555, /* The exponent used in the calculation
                                           of exponentially weighted moving
                                           average of port egressed bytes */
    bcmSwitchFabricTrunkDynamicQueuedBytesExponent = 556, /* The exponent used in the calculation
                                           of exponentially weighted moving
                                           average of port queued bytes */
    bcmSwitchFabricTrunkDynamicEgressBytesDecreaseReset = 557, /* If set, the average port egressed
                                           bytes is updated with the sampled
                                           value if the sampled value is less
                                           than the average */
    bcmSwitchFabricTrunkDynamicQueuedBytesDecreaseReset = 558, /* If set, the average port queued bytes
                                           is updated with the sampled value if
                                           the sampled value is less than the
                                           average */
    bcmSwitchFabricTrunkDynamicEgressBytesMinThreshold = 559, /* The minimum threshold, in mbps, used
                                           to quantize average port egressed
                                           bytes */
    bcmSwitchFabricTrunkDynamicEgressBytesMaxThreshold = 560, /* The maximum threshold, in mbps, used
                                           to quantize average port egressed
                                           bytes */
    bcmSwitchFabricTrunkDynamicQueuedBytesMinThreshold = 561, /* The minimum threshold, in bytes, used
                                           to quantize average port queued bytes */
    bcmSwitchFabricTrunkDynamicQueuedBytesMaxThreshold = 562, /* The maximum threshold, in bytes, used
                                           to quantize average port queued bytes */
    bcmSwitchMacLowPower = 563,         /* Enable switch low power mode and set
                                           the low power frequency, Khz */
    bcmSwitchCongestionCntag = 564,     /* Enable/Disable CN-TAG processing */
    bcmSwitchCongestionCntagEthertype = 565, /* EtherType for CN-TAG */
    bcmSwitchCongestionCnm = 566,       /* Enable/Disable congestion
                                           notification message handling */
    bcmSwitchCongestionCnmEthertype = 567, /* EtherType for congestion notification
                                           message */
    bcmSwitchCongestionNotificationIdHigh = 568, /* Device identifier for contructing
                                           congestion point identifier */
    bcmSwitchCongestionNotificationIdLow = 569, /* Port identifier for constructing
                                           congestion point identifier */
    bcmSwitchCongestionNotificationIdQueue = 570, /* Queue mode of the congestion point
                                           identifier. When set to 0 use
                                           congestion managed queue id,
                                           otherwise use queue number */
    bcmSwitchCongestionUseOuterTpid = 571, /* Outer TPID to be used for congestion
                                           notification message */
    bcmSwitchCongestionUseOuterVlan = 572, /* Outer vlan to be used for congestion
                                           notification message */
    bcmSwitchCongestionUseOuterPktPri = 573, /* Outer priority to be used for
                                           congestion notification message */
    bcmSwitchCongestionUseOuterCfi = 574, /* Outer CFI to be used for congestion
                                           notification message */
    bcmSwitchCongestionUseInnerPktPri = 575, /* Inner priority to be used for
                                           congestion notification message */
    bcmSwitchCongestionUseInnerCfi = 576, /* Inner CFI to be used for congestion
                                           notification message */
    bcmSwitchCongestionMissingCntag = 577, /* Choose the CNM generation behavior
                                           when congestion notification
                                           threshold is reached but the incoming
                                           sampled packet does not have CN-TAG */
    bcmSwitchCongestionExcludeReplications = 578, /* Take the replicated packets in
                                           account during the congestion test */
    bcmSwitchMiMDefaultSVP = 579,       /* Enable MiM termination lookup miss
                                           packets to be forwarded using default
                                           SVP */
    bcmSwitchMiMDefaultSVPValue = 580,  /* Default SVP  for Mac-in-Mac */
    bcmSwitchEgressDroppedReportZeroLength = 581, /* Enable egress pipeline to report zero
                                           length for dropped packets */
    bcmSwitchLoadBalanceHashSelect = 582, /* Selects which hash function to use
                                           for Load Balance computation */
    bcmSwitchTrillEthertype = 583,      /* Set TRILL EtherType field. Value of
                                           uint16 type */
    bcmSwitchTrillISISEthertype = 584,  /* Set TRILL ISIS EtherType field. Value
                                           of uint16 type */
    bcmSwitchTrillISISDestMacOui = 585, /* Set TRILL All_ISIS_Rbridge MAC upper
                                           3 bytes to match. */
    bcmSwitchTrillBroadcastDestMacOui = 586, /* Set TRILL ALL_Rbridge MAC upper 3
                                           bytes to match. */
    bcmSwitchTrillEndStationDestMacOui = 587, /* Set TRILL ALL_ESADI MAC upper 3 bytes
                                           to match. */
    bcmSwitchTrillISISDestMacNonOui = 588, /* Set TRILL ALL_ISIS_Rbridge MAC lower
                                           3 bytes to match. */
    bcmSwitchTrillBroadcastDestMacNonOui = 589, /* Set TRILL ALL_Rbridge MAC lower 3
                                           bytes to match. */
    bcmSwitchTrillEndStationDestMacNonOui = 590, /* Set TRILL ALL_ESADI MAC lower 3 bytes
                                           to match. */
    bcmSwitchTrillMinTtl = 591,         /* Set minimum TTL for Egress and
                                           Transit TRILL Rbridge. */
    bcmSwitchTrillTtlCheckEnable = 592, /* Enable TTL check for Egress TRILL
                                           Rbridge. */
    bcmSwitchTrillTtlErrToCpu = 593,    /* Copy TRILL TTL failure packets to
                                           CPU. */
    bcmSwitchTrillHeaderErrToCpu = 594, /* Copy TRILL Header Error frame to CPU. */
    bcmSwitchTrillMismatchToCpu = 595,  /* Copy TRILL mismatch error frame to
                                           CPU. */
    bcmSwitchTrillNameMissToCpu = 596,  /* Copy TRILL Rbridge lookup failure to
                                           CPU. */
    bcmSwitchTrillRpfFailToCpu = 597,   /* Copy TRILL RPF check fail to CPU. */
    bcmSwitchTrillOptionsToCpu = 598,   /* Copy header Options to CPU. */
    bcmSwitchTrillNameErrDrop = 599,    /* Set Drop setting for TRILL packets
                                           with Ingress-name equal to
                                           Egress-name. */
    bcmSwitchTrillRpfFailDrop = 600,    /* Set Drop setting for TRILL multicast
                                           packets that fail RPF check. */
    bcmSwitchTrillHeaderVersionErrDrop = 601, /* Set Drop Setting for TRILL frames
                                           received with a non-zero Version
                                           field. */
    bcmSwitchTrillNameMissDrop = 602,   /* Set Drop setting for TRILL Ingress
                                           RBridge nickname lookup failure. */
    bcmSwitchTrillAdjacencyFailDrop = 603, /* Set Drop setting for TRILL adjacency
                                           failure. */
    bcmSwitchTrillHeaderErrDrop = 604,  /* Set Drop Setting for TRILL header
                                           error. */
    bcmSwitchUdpPktActionEnable = 605,  /* Enable additional actions for UDP
                                           packets */
    bcmSwitchUdpPktRedirectPbmp = 606,  /* Replace the destination port bitmap
                                           of UDP packets */
    bcmSwitchUdpPktCopyToPbmp = 607,    /* Add the destination port bitmap of
                                           UDP packets */
    bcmSwitchUdpPktNewTc = 608,         /* Change the Traffic Class value of UDP
                                           packets */
    bcmSwitchL3TunnelIpV4ModeOnly = 609, /* Set L3 Tunnel to IPV4 mode only */
    bcmSwitchL3SrcHitEnable = 610,      /* Enables/Disables source hit bit
                                           lookup update in the L3 table */
    bcmSwitchL2DstHitEnable = 611,      /* Enables/Disables destination hit bit
                                           lookup update in the L2 table */
    bcmSwitchWESPProtocolEnable = 612,  /* Enables/Disables parsing into WESP
                                           payload */
    bcmSwitchWESPProtocol = 613,        /* WESP protocol number */
    bcmSwitchDeprecated614 = 614,       /* Deprecated. Do not use. */
    bcmSwitchIp6CompressEnable = 615,   /* Enables/Disables IPv6 compression
                                           support in the VFP table */
    bcmSwitchIp6CompressDefaultOffset = 616, /* Set the offset value used to extract
                                           26-bits from the IPv6 address field
                                           of IPv6 packet */
    bcmSwitchStableSaveLongIds = 617,   /* When saving FP information to storage
                                           for level-2 warm boot, save full
                                           entity ids (i.e. not compact) */
    bcmSwitchGportAnyDefaultL2Learn = 618, /* Set default cml_value for Any Gport
                                           type. */
    bcmSwitchGportAnyDefaultL2Move = 619, /* Set default cml_station_move value
                                           for Any Gport type */
    bcmSwitchIngParseL2TunnelTermDipSipSelect = 620, /* Controls the values used to populate
                                           the DIP+SIP containers for
                                           L2-tunnel-terminated packets */
    bcmSwitchIngParseL3L4IPv4 = 621,    /* Enable parsing of IPv4 L3/L4 headers */
    bcmSwitchIngParseL3L4IPv6 = 622,    /* Enable parsing of IPv6 L3/L4 headers */
    bcmSwitchL2LearnMode = 623,         /* Learning Mode */
    bcmSwitchSetWredJitterMask = 624,   /* Set WRED Jitter value */
    bcmSwitchSetMplsEntropyLabelTtl = 625, /* Set MPLS Entropy-Label TTL value */
    bcmSwitchSetMplsEntropyLabelPri = 626, /* Set MPLS Entropy-Label PRI value */
    bcmSwitchSetMplsEntropyLabelOffset = 627, /* Set MPLS Entropy-Label Hash Offset */
    bcmSwitchPonOamFailsafeState = 628, /* Enables/Disables MPCP fail safe */
    bcmSwitchEquipmentAlarmState = 629, /* Set equipment alarm report state */
    bcmSwitchPowerAlarmState = 630,     /* Set power alarm report state */
    bcmSwitchBatteryMissingAlarmState = 631, /* Set battery missing alarm report
                                           state */
    bcmSwitchBatteryFailureAlarmState = 632, /* Set battery failure alarm report
                                           state */
    bcmSwitchBatteryVoltLowAlarmState = 633, /* Set battery volt low alarm report
                                           state */
    bcmSwitchBatteryVoltLowAlarmReportThreshold = 634, /* Set battery volt low alarm report
                                           threshold */
    bcmSwitchBatteryVoltLowAlarmClearThreshold = 635, /* Set battery volt low alarm clear
                                           threshold */
    bcmSwitchPhysicalIntrusionAlarmState = 636, /* Set physical instrusion alarm report
                                           state */
    bcmSwitchSelfTestFailureAlarmState = 637, /* Set self test failure alarm report
                                           state */
    bcmSwitchPonIfSwitchAlarmState = 638, /* Set PON switchover alarm report state */
    bcmSwitchL3IngressInterfaceMapSet = 639, /* Enable mode to configure
                                           ingress-mapping of Vlan and L3
                                           Ingress Interface */
    bcmSwitchMplsPWControlWordToCpu = 640, /* Enable MPLS packets with PWACH
                                           control word, to be copied to CPU */
    bcmSwitchMplsPWControlTypeToCpu = 641, /* Enable MPLS packets with other
                                           control word, to be copied to CPU */
    bcmSwitchMplsPWControlWord = 642,   /* Set Control Word value */
    bcmSwitchMplsPWCountPktsAll = 643,  /* Enable count-all MPLS packets
                                           arriving on PWs */
    bcmSwitchEntropyHashSet0Offset = 644, /* Selection for MPLS-Entropy-Label hash
                                           bits. */
    bcmSwitchEntropyHashSet1Offset = 645, /* Selection for MPLS-Entropy-Label hash
                                           bits. */
    bcmSwitchArpRequestMyStationIPToCPU = 646, /* APR reply packets with My IP address
                                           should be sent to CPU. */
    bcmSwitchArpRequestMyStationIPDrop = 647, /* APR reply packets with My IP address
                                           should be dropped. */
    bcmSwitchArpRequestMyStationIPFwd = 648, /* APR reply packets with given IP
                                           address should not trapped/dropped. */
    bcmSwitchL3SIPMoveToCpu = 649, 
    bcmSwitchTrunkHashSrcPortEnable = 650, /* BCM886XX: Make Source port part of
                                           the hash. */
    bcmSwitchTrunkHashOffset = 651,     /* BCM886XX: Hash key is barrel shifted
                                           by this value */
    bcmSwitchTrunkHashMPLSLabelBOS = 652, /* In an mpls label stack, look for
                                           label with bottom of stack bit set.
                                           Valid values see
                                           BCM_HASH_MPLS_LABEL_BOS_ */
    bcmSwitchTrunkHashMPLSPWControlWord = 653, /* BCM886XX: Indicates control word is
                                           present at end of MPLS headers. */
    bcmSwitchTrunkHashSeed = 654,       /* BCM886XX: Initial seed value for
                                           trunk hashing. */
    bcmSwitchTrunkHashConfig = 655,     /* BCM886XX: Selects hashing polynomial. */
    bcmSwitchTrunkHashPktHeaderCount = 656, /* BCM886XX: Number of headers to
                                           consider in hashing. */
    bcmSwitchTrunkHashPktHeaderSelect = 657, /* BCM886XX: Selects which starting
                                           header to use for hashing. */
    bcmSwitchECMPHashSrcPortEnable = 658, /* BCM886XX: Make Source port part of
                                           the ECMP hash. */
    bcmSwitchECMPHashOffset = 659,      /* BCM886XX: ECMP Hash key is barrel
                                           shifted by this value. */
    bcmSwitchECMPHashSeed = 660,        /* BCM886XX: Initial seed value for ECMP
                                           hashing. */
    bcmSwitchECMPHashConfig = 661,      /* BCM886XX: Selects hashing polynomial
                                           for ECMP. */
    bcmSwitchECMPHashPktHeaderCount = 662, /* BCM886XX: Number of headers to
                                           consider in ECMP hashing. */
    bcmSwitchTrunkDynamicHashOffset = 663, /* Enhanced hash selections for trunk
                                           dynamic load balancing. */
    bcmSwitchTrunkDynamicSampleRate = 664, /* Number of times historical member
                                           load and queued bytes are computed in
                                           a second */
    bcmSwitchTrunkDynamicAccountingSelect = 665, /* 0 = physical link accounting, 1 =
                                           virtual link accounting */
    bcmSwitchTrunkDynamicEgressBytesExponent = 666, /* The exponent used in the calculation
                                           of exponentially weighted moving
                                           average of historical member load.
                                           Applicable to physical link
                                           accounting. */
    bcmSwitchTrunkDynamicQueuedBytesExponent = 667, /* The exponent used in the calculation
                                           of exponentially weighted moving
                                           average of historical member queued
                                           bytes. Applicable to physical link
                                           accounting. */
    bcmSwitchTrunkDynamicEgressBytesDecreaseReset = 668, /* If set, the historical member load is
                                           reset to the instantaneous value if
                                           the latter is smaller. Applicable to
                                           physical link accounting. */
    bcmSwitchTrunkDynamicQueuedBytesDecreaseReset = 669, /* If set, the historical member queued
                                           bytes is updated with the
                                           instantaneous value if the latter is
                                           smaller. Applicable to physical link
                                           accounting. */
    bcmSwitchTrunkDynamicEgressBytesMinThreshold = 670, /* The minimum threshold, in mbps, used
                                           to quantize historical member load.
                                           Applicable to physical link
                                           accounting. */
    bcmSwitchTrunkDynamicEgressBytesMaxThreshold = 671, /* The maximum threshold, in mbps, used
                                           to quantize historical member load.
                                           Applicable to physical link
                                           accounting. */
    bcmSwitchTrunkDynamicQueuedBytesMinThreshold = 672, /* The minimum threshold, in bytes, used
                                           to quantize historical member queued
                                           bytes. Applicable to physical link
                                           accounting. */
    bcmSwitchTrunkDynamicQueuedBytesMaxThreshold = 673, /* The maximum threshold, in bytes, used
                                           to quantize historical member queued
                                           bytes. Applicable to physical link
                                           accounting. */
    bcmSwitchTrunkDynamicExpectedLoadMinThreshold = 674, /* The minimum threshold, in mbps, used
                                           to quantize historical and
                                           instantaneous expected member load.
                                           Applicable to virtual link
                                           accounting. */
    bcmSwitchTrunkDynamicExpectedLoadMaxThreshold = 675, /* The maximum threshold, in mbps, used
                                           to quantize historical and
                                           instantaneous expected member load.
                                           Applicable to virtual link
                                           accounting. */
    bcmSwitchTrunkDynamicImbalanceMinThreshold = 676, /* The minimum threshold, in percentage,
                                           used to quantize historical and
                                           instantaneous member load imbalance.
                                           Applicable to virtual link
                                           accounting. */
    bcmSwitchTrunkDynamicImbalanceMaxThreshold = 677, /* The maximum threshold, in percentage,
                                           used to quantize historical and
                                           instantaneous member load imbalance.
                                           Applicable to virtual link
                                           accounting. */
    bcmSwitchEcmpDynamicHashOffset = 678, /* Enhanced hash selections for L3 ECMP
                                           dynamic load balancing. */
    bcmSwitchEcmpDynamicSampleRate = 679, /* Number of times historical member
                                           load and queued bytes are computed in
                                           a second */
    bcmSwitchEcmpDynamicAccountingSelect = 680, /* 0 = physical link accounting, 1 =
                                           virtual link accounting */
    bcmSwitchEcmpDynamicEgressBytesExponent = 681, /* The exponent used in the calculation
                                           of exponentially weighted moving
                                           average of historical member load.
                                           Applicable to physical link
                                           accounting. */
    bcmSwitchEcmpDynamicQueuedBytesExponent = 682, /* The exponent used in the calculation
                                           of exponentially weighted moving
                                           average of historical member queued
                                           bytes. Applicable to physical link
                                           accounting. */
    bcmSwitchEcmpDynamicEgressBytesDecreaseReset = 683, /* If set, the historical member load is
                                           reset to the instantaneous value if
                                           the latter is smaller. Applicable to
                                           physical link accounting. */
    bcmSwitchEcmpDynamicQueuedBytesDecreaseReset = 684, /* If set, the historical member queued
                                           bytes is updated with the
                                           instantaneous value if the latter is
                                           smaller. Applicable to physical link
                                           accounting. */
    bcmSwitchEcmpDynamicEgressBytesMinThreshold = 685, /* The minimum threshold, in mbps, used
                                           to quantize historical member load.
                                           Applicable to physical link
                                           accounting. */
    bcmSwitchEcmpDynamicEgressBytesMaxThreshold = 686, /* The maximum threshold, in mbps, used
                                           to quantize historical member load.
                                           Applicable to physical link
                                           accounting. */
    bcmSwitchEcmpDynamicQueuedBytesMinThreshold = 687, /* The minimum threshold, in bytes, used
                                           to quantize historical member queued
                                           bytes. Applicable to physical link
                                           accounting. */
    bcmSwitchEcmpDynamicQueuedBytesMaxThreshold = 688, /* The maximum threshold, in bytes, used
                                           to quantize historical member queued
                                           bytes. Applicable to physical link
                                           accounting. */
    bcmSwitchEcmpDynamicExpectedLoadMinThreshold = 689, /* The minimum threshold, in mbps, used
                                           to quantize historical and
                                           instantaneous expected member load.
                                           Applicable to virtual link
                                           accounting. */
    bcmSwitchEcmpDynamicExpectedLoadMaxThreshold = 690, /* The maximum threshold, in mbps, used
                                           to quantize historical and
                                           instantaneous expected member load.
                                           Applicable to virtual link
                                           accounting. */
    bcmSwitchEcmpDynamicImbalanceMinThreshold = 691, /* The minimum threshold, in percentage,
                                           used to quantize historical and
                                           instantaneous member load imbalance.
                                           Applicable to virtual link
                                           accounting. */
    bcmSwitchEcmpDynamicImbalanceMaxThreshold = 692, /* The maximum threshold, in percentage,
                                           used to quantize historical and
                                           instantaneous member load imbalance.
                                           Applicable to virtual link
                                           accounting. */
    bcmSwitchFailoverDropToCpu = 693,   /* Protection data drop to CPU enable. */
    bcmSwitchMplsReservedEntropyLabelToCpu = 694, /* Packet is copied to the CPU if the
                                           entropy label falls between the
                                           reserved range (0-15). */
    bcmSwitchL3SrcBindMissToCpu = 695,  /* Send a copy of L3 packet to CPU if
                                           SIP lookup fails in HPAE mode. */
    bcmSwitchMplsGalAlertLabelToCpu = 696, /* When a GAL label is exposed in a
                                           packet, such a packet will be
                                           dropped. This control determines
                                           whether such a packet gets copied to
                                           CPU. */
    bcmSwitchMplsRalAlertLabelToCpu = 697, /* When a RAL label is exposed in a
                                           packet, such a packet will be
                                           dropped. This control determines
                                           whether such a packet gets copied to
                                           CPU. */
    bcmSwitchMplsIllegalReservedLabelToCpu = 698, /* Exposed an illegal reserved label to
                                           CPU. For example, exposed GAL Label
                                           has BOS=0. */
    bcmSwitchMplsLookupsExceededToCpu = 699, /* Expose out of lookups to CPU */
    bcmSwitchMplsUnknownAchTypeToCpu = 700, /* Packet has an unknown ACH Type. */
    bcmSwitchMplsUnknownAchVersionToCpu = 701, /* The version number of ACH is not 1. */
    bcmSwitchTimesyncIngressVersion = 702, /* Ingress 1588 version 0:V2 1:>V2. */
    bcmSwitchTimesyncEgressVersion = 703, /* Egress 1588 version 0:V2 1:>V2. */
    bcmSwitchTimesyncUnknownVersionToCpu = 704, /* The version number is not recognized. */
    bcmSwitchCongestionCnmToCpu = 705,  /* Send ICNM packets to the CPU.
                                           (Deprecated) */
    bcmSwtichCongestionCnmProxyErrorToCpu = 706, /* Enable forwarding to CPU. */
    bcmSwtichCongestionCnmProxyToCpu = 707, /* This is to copy the dropped CNM
                                           packet to CPU if a network
                                           administrator needs to look at them. */
    bcmSwitchWlanTunnelErrorDrop = 708, /* WLAN Decryption offload enable. */
    bcmSwitchMplsReservedEntropyLabelDrop = 709, /* When the value of the received
                                           entropy label falls in the reserved
                                           range(0-15), this control will
                                           determine if the packet is dropped or
                                           not. */
    bcmSwitchRemoteProtectionTrust = 710, /* If set, use the Protection Status bit
                                           in the received HiGig Header. */
    bcmSwitchL2GreProtocolType = 711,   /* Set protocol-type for L2-GRE */
    bcmSwitchL2GreVpnIdSizeSet = 712,   /* Set bit-size of VPNID within L2-GRE
                                           key */
    bcmSwitchL2GreVpnIdMissToCpu = 713, /* If set, send a copy of L2GRE packet
                                           to CPU if VPNID lookup fails. */
    bcmSwitchL2GreTunnelMissToCpu = 714, /* If set, send a copy of L2GRE packet
                                           to CPU if SIP lookup fails. */
    bcmSwitchMultipathCompressBuffer = 715, /* Set the size of the L3 ECMP table
                                           defragmentation buffer. */
    bcmSwitchMultipathCompress = 716,   /* Defragment L3 ECMP table. */
    bcmSwitchBstEnable = 717,           /* Enable BST tracking. */
    bcmSwitchBstTrackingMode = 718,     /* BST resource usage tracking mode. */
    bcmSwitchL3McastL2 = 719,           /* if set to 0 then l2 key is <VID, DA>,
                                           if set to 1 then l2 key is <RIF/VID,
                                           DIP>. */
    bcmSwitchMiMTeBvidLowerRange = 720, /* Configure the lower limit for Traffic
                                           engineered BVID */
    bcmSwitchMiMTeBvidUpperRange = 721, /* Configure the upper limit for Traffic
                                           engineered BVID */
    bcmSwitchRemoteEncapsulationMode = 722, /* If set, all packet replication is
                                           performed on a remote device. 
                                           Required egress encapsulation is
                                           indicated by the HiGig2 header. */
    bcmSwitchFabricTrunkAutoIncludeDisable = 723, /* If set, disable automatic inclusion
                                           of other members of a HiGig trunk
                                           group when one member is specified as
                                           the steering destination for a remote
                                           module. */
    bcmSwitchTrunkHashNormalize = 724,  /* Enables using same hash in both
                                           directions of a flow. Applies to both
                                           MAC/IP addresses */
    bcmSwitchMplsLabelCache = 725,      /* Enable Mpls Label Cache */
    bcmSwitchMplsLabelCommit = 726,     /* Commit the MPLS Label Database to
                                           hardware */
    bcmSwitchTrillHopCount = 727,       /* Trill hop count */
    bcmSwitchPolicerFairness = 728,     /* if set, packet is considered in
                                           profile if bucket-level plus random
                                           value are above packet-size. If not
                                           set, packet is considered in profile
                                           if bucket-level is above packet-size. */
    bcmSwitchVxlanUdpDestPortSet = 729, /* Set UDP Destination port for VXLAN */
    bcmSwitchVxlanEntropyEnable = 730,  /* Set UDP Source port for VXLAN Tunnel
                                           Entropy(Value = TRUE/FALSE) */
    bcmSwitchVxlanVnIdMissToCpu = 731,  /* Set VXLAN VN_ID lookup failure Copy
                                           to CPU */
    bcmSwitchVxlanTunnelMissToCpu = 732, /* If set, send a copy of VXLAN packet
                                           to CPU if VXLAN Tunnel lookup fails. */
    bcmSwitchECMPVxlanHashOffset = 733, /* Enhanced hash select for VxLAN ECMP
                                           hashing . */
    bcmSwitchECMPL2GreHashOffset = 734, /* Enhanced hash select for L2Gre ECMP
                                           hashing . */
    bcmSwitchECMPTrillHashOffset = 735, /* Enhanced hash select for Trill ECMP
                                           hashing . */
    bcmSwitchECMPMplsHashOffset = 736,  /* Enhanced hash select for Mpls ECMP
                                           hashing . */
    bcmSwitchVirtualPortDynamicHashOffset = 737, /* Enhanced hash select for Virtual Port
                                           Aggregation. */
    bcmSwitchTMDomain = 738,            /* Device TM-Domain. */
    bcmSwitchPortHeaderType = 739,      /* Sets TM Port header type. */
    bcmSwitchL2OverflowEvent = 740,     /* Enable/Disable L2 overflow event
                                           processing. */
    bcmSwitchLinkProvClear = 741,       /* Automatically clear provision for a
                                           link when it is de-registered. */
    bcmSwitchEtagEthertype = 742,       /* Set EtherType field in ETAG */
    bcmSwitchEtagInvalidToCpu = 743,    /* Invalid ETAG to CPU */
    bcmSwitchEtagDropToCpu = 744,       /* Packets dropped due to ETAG present
                                           to CPU. Enable/disable or set
                                           specific system port gport. */
    bcmSwitchNonEtagDropToCpu = 745,    /* Packets dropped due to ETAG not
                                           present to CPU */
    bcmSwitchExtenderRpfFailToCpu = 746, /* Extender RPF check fail to CPU */
    bcmSwitchExtenderInterfaceMissToCpu = 747, /* Extender forwarding lookup miss to
                                           CPU */
    bcmSwitchPrioDropToCpu = 748,       /* Priority admittance drop to CPU */
    bcmSwitchExtenderMulticastLowerThreshold = 749, /* Lower threshold of multicast extended
                                           port VID */
    bcmSwitchExtenderMulticastHigherThreshold = 750, /* Higher threshold of multicast
                                           extended port VID */
    bcmSwitchL3NATEnable = 751,         /* Enable/Disable NAT on the switch. */
    bcmSwitchL3DNATHairpinToCpu = 752,  /* If set send copy of DNAT hairpin
                                           packets to CPU. */
    bcmSwitchL3DNATMissToCpu = 753,     /* If set send copy of DNAT lookup miss
                                           packets to CPU. */
    bcmSwitchL3SNATMissToCpu = 754,     /* If set send copy of SNAT lookup miss
                                           packets to CPU. */
    bcmSwitchL3NatOtherToCpu = 755,     /* If set send copy of realm crossing
                                           non-TCP, UDP or ICMP packets to CPU. */
    bcmSwitchL3NatRealmCrossingIcmpToCpu = 756, /* If set send copy of realm crossing
                                           ICMP packets to CPU. */
    bcmSwitchL3NatFragmentsToCpu = 757, /* If set send copy of NAT fragment
                                           packets to CPU. */
    bcmSwitchHashL3DNATPool = 758,      /* Hash Select for DNAT address type
                                           entry. */
    bcmSwitchHashL3DNATPoolDual = 759,  /* Secondary Hash Select for DNAT
                                           address type entry. */
    bcmSwitchMcastTrunkHashMin = 760,   /* Minimal hushing value for a copy with
                                           a given port as destination. Copies
                                           with lower Hash values will be
                                           dropped. */
    bcmSwitchMcastTrunkHashMax = 761,   /* Maximal hushing value for a copy with
                                           a given port as destination. Copies
                                           with higher Hash values will be
                                           dropped. */
    bcmSwitchIp4Compress = 762,         /* compress IPv4 routing table. */
    bcmSwitchEcmpResilientHashOffset = 763, /* Enhanced hash select for ECMP
                                           resilient hashing . */
    bcmSwitchTrunkResilientHashOffset = 764, /* Barrel shift the resilient hashing
                                           LAG key. */
    bcmSwitchFabricTrunkResilientHashOffset = 765, /* Enhanced hash select for HiGig trunk
                                           resilient hashing . */
    bcmSwitchShaperAdjust = 766,        /* Meter/Rate control with overhead on
                                           wire accounted for per packet for
                                           EFP. */
    bcmSwitchHashVpVlanMemberIngress = 767, /* Select Hash algorithm for ingress vp
                                           vlan membership check. */
    bcmSwitchHashVpVlanMemberIngressDual = 768, /* Select secondary Hash algorithm for
                                           ingress vp vlan membership check. */
    bcmSwitchHashVpVlanMemberEgress = 769, /* Select Hash algorithm for Egress vp
                                           vlan membership check. */
    bcmSwitchHashVpVlanMemberEgressDual = 770, /* Select secondary Hash algorithm for
                                           Egress vp vlan membership check. */
    bcmSwitchHashL2Endpoint = 771,      /* Select hash algorithm for L2 Endpoint
                                           table. */
    bcmSwitchHashL2EndpointDual = 772,  /* Select secondary hash algorithm for
                                           L2 Endpoint table. */
    bcmSwitchHashEndpointQueueMap = 773, /* Select hash algorithm for Endpoint
                                           Queue Map table. */
    bcmSwitchHashEndpointQueueMapDual = 774, /* Select secondary hash algorithm for
                                           Endpoint Queue Map table. */
    bcmSwitchStackRouteHistoryBitmap1 = 775, /* Configures Stacking route history
                                           bitmap stamping according to port
                                           profile number 1. Uses for packet
                                           pruning in Stacking system. */
    bcmSwitchStackRouteHistoryBitmap2 = 776, /* Configures Stacking route history
                                           bitmap stamping according to port
                                           profile number 2. Uses for packet
                                           pruning in Stacking system. */
    bcmSwitchArpIPcheck = 777,          /* For ARP request, check if TPA is
                                           known. */
    bcmSwitchDeprecated778 = 778,       /* Deprecated. Do not use. */
    bcmSwitchDeprecated779 = 779,       /* Deprecated. Do not use. */
    bcmSwitchL2LearnLimitToCpu = 780,   /* Copy the frame to CPU once the learn
                                           limitation occurred due to the hash
                                           is full in L2 Table. */
    bcmSwitchFlexibleMirrorDestinations = 781, /* Enable configuration of the maximum
                                           number of mirror destinations. */
    bcmSwitchEcmpMacroFlowHashEnable = 782, /* Enable ECMP macro-flow hashing. */
    bcmSwitchOamVersionCheckDisable = 783, /* Disable version check for OAM packets
                                           in pipeline. */
    bcmSwitchOamOlpChipEtherType = 784, /* Ether Type to be used for OLP Chip
                                           communication. */
    bcmSwitchOamOlpChipTpid = 785,      /* TPID to be used for OLP Chip
                                           communication. */
    bcmSwitchOamOlpChipVlan = 786,      /* VLAN ID to be used for OLP Chip
                                           communication. */
    bcmSwitchDeprecated787 = 787,       /* Deprecated. Do not use. */
    bcmSwitchEgressKeepSystemHeader = 788, /* Keep system headers on Tx packets for
                                           a given port. */
    bcmSwitchMacroFlowEcmpHashConcatEnable = 789, /* Enable hash concatenation for ecmp
                                           macro flow. */
    bcmSwitchMacroFlowLoadBalanceHashConcatEnable = 790, /* Enable hash concatenation for Load
                                           balance macro flow. */
    bcmSwitchMacroFlowTrunkHashConcatEnable = 791, /* Enable hash concatenation for Trunk
                                           macro flow. */
    bcmSwitchMacroFlowHigigTrunkHashConcatEnable = 792, /* Enable hash concatenation for HiGig
                                           Trunk macro flow. */
    bcmSwitchMacroFlowECMPHashSeed = 793, /* Set Hash seed for ecmp macro flow. */
    bcmSwitchMacroFlowLoadBalanceHashSeed = 794, /* Set Hash Seed for Load balance macro
                                           flow. */
    bcmSwitchMacroFlowTrunkHashSeed = 795, /* Set Hash Seed for Trunk macro flow. */
    bcmSwitchMacroFlowHigigTrunkHashSeed = 796, /* Set Hash Seed for HiGig Trunk macro
                                           flow. */
    bcmSwitchHashTrillPayloadSelect0 = 797, /* Set hash control to select TRILL
                                           payload L2/L3 fields for Hash Block
                                           A. */
    bcmSwitchHashTrillPayloadSelect1 = 798, /* Set hash control to select TRILL
                                           payload L2/L3 fields for Hash Block
                                           B. */
    bcmSwitchHashTrillTunnelSelect0 = 799, /* Set hash control to select TRILL
                                           Tunnel L2/L3 fields for Hash Block A. */
    bcmSwitchHashTrillTunnelSelect1 = 800, /* Set hash control to select TRILL
                                           Tunnel L2/L3 fields for Hash Block B. */
    bcmSwitchHashIP6AddrCollapseSelect0 = 801, /* Hash Control to collapse IPV6
                                           addresses for hash block A. */
    bcmSwitchHashIP6AddrCollapseSelect1 = 802, /* Hash Control to collapse IPV6
                                           addresses for hash block B. */
    bcmSwitchPrependTagEnable = 803,    /* Enable Prepend tag per Port. */
    bcmSwitchHashL2VxlanField0 = 804,   /* Hash A Selection of L2 Payload for
                                           Vxlan Packets */
    bcmSwitchHashL2VxlanField1 = 805,   /* Hash B Selection of L2 Payload for
                                           Vxlan Packets */
    bcmSwitchHashL3VxlanField0 = 806,   /* Hash A Selection of L3 Payload for
                                           Vxlan Packets */
    bcmSwitchHashL3VxlanField1 = 807,   /* Hash B Selection of L3 Payload for
                                           Vxlan Packets */
    bcmSwitchHashL2L2GreField0 = 808,   /* Hash A Selection of L2 Payload for
                                           L2Gre Packets */
    bcmSwitchHashL2L2GreField1 = 809,   /* Hash B Selection of L2 Payload for
                                           L2Gre Packets */
    bcmSwitchHashL3L2GreField0 = 810,   /* Hash A Selection of L3 Payload for
                                           L2Gre Packets */
    bcmSwitchHashL3L2GreField1 = 811,   /* Hash B Selection of L3 Payload for
                                           L2Gre Packets */
    bcmSwitchMcastTrunkIngressCommit = 812, /* If set, the Multicast LAG packets use
                                           the new LAG table definition for the
                                           member resolution. */
    bcmSwitchMcastTrunkEgressCommit = 813, /* If set, the new Multicast LAG table
                                           is copied to the old one. It assumes
                                           that all the devices are using the
                                           new LAG table, after calling
                                           bcmSwitchMcastTrunkIngressCommit. */
    bcmSwitchMplsShortPipe = 814,       /* If Set device supports Short pipe
                                           only. Otherwise Pipe only. */
    bcmSwitchTraverseMode = 815,        /* Control the traverse mode, choose
                                           mode from
                                           bcm_switch_table_update_mode_t */
    bcmSwitchTrunkResilientHashConfig = 816, /* Stateful LAG hashing function */
    bcmSwitchTrunkResilientHashSeed = 817, /* Stateful LAG hashing seed */
    bcmSwitchECMPResilientHashSeed = 818, /* Stateful ECMP hashing seed */
    bcmSwitchResilientHashAgeTimer = 819, /* Resilient hash entry deletion time.
                                           Units: seconds */
    bcmSwitchFieldStageEgressToCpu = 820, /* Control to enable egress copy to CPU
                                           action */
    bcmSwitchHashOamEgress = 821,       /* Hash Select for egress OAM. */
    bcmSwitchHashOamEgressDual = 822,   /* Secondary Hash Select for egress OAM. */
    bcmSwitchColorL3L2Marking = 823,    /* Set the DP->Resolved-DP map (COS
                                           marking) */
    bcmSwitchWredForAllPkts = 824,      /* Apply WRED per color profiles on all
                                           packets */
    bcmSwitchFcoeNpvModeEnable = 825,   /* NPV mode */
    bcmSwitchFcoeDomainRoutePrefixLength = 826, /* D-ID Bits to use for domain route
                                           search */
    bcmSwitchFcoeCutThroughEnable = 827, /* Enables Cut-Through mode for FCOE
                                           packets */
    bcmSwitchFcoeSourceBindCheckAction = 828, /* Sets the action to take when
                                           Source-bind-check fails */
    bcmSwitchFcoeSourceFpmaPrefixCheckAction = 829, /* Sets the action to take when
                                           Source-fpma-prefix-check fails */
    bcmSwitchFcoeVftHopCountExpiryToCpu = 830, /* If set, copy hopcount expired packets
                                           to CPU */
    bcmSwitchFcoeVftHopCountExpiryAction = 831, /* If set, send hopcount expired packets
                                           with EOFni in FC trailer. Applicable
                                           at egress. */
    bcmSwitchFcoeFcEofT1Value = 832,    /* Sets the numerical value for EOFt */
    bcmSwitchFcoeFcEofT2Value = 833,    /* Sets the numerical value for EOFt
                                           (alternate) */
    bcmSwitchFcoeFcEofA1Value = 834,    /* Sets the numerical value for EOFa */
    bcmSwitchFcoeFcEofA2Value = 835,    /* Sets the numerical value for EOFa
                                           (alternate) */
    bcmSwitchFcoeFcEofN1Value = 836,    /* Sets the numerical value for EOFn */
    bcmSwitchFcoeFcEofN2Value = 837,    /* Sets the numerical value for EOFn
                                           (alternate) */
    bcmSwitchFcoeFcEofNI1Value = 838,   /* Sets the numerical value for EOFni */
    bcmSwitchFcoeFcEofNI2Value = 839,   /* Sets the numerical value for EOFni
                                           (alternate) */
    bcmSwitchFcoeZoneCheckFailToCpu = 840, /* Action to take when FCOE zone check
                                           fails, 0 : no-op(default), 1 :
                                           Copy-to-CPU */
    bcmSwitchFcoeZoneCheckMissDrop = 841, /* Action to take when FCOE zone check
                                           is a miss, 0 : no-op, 1 :
                                           Drop(default) */
    bcmSwitchHashUseL2GreTunnelGreKey0 = 842, /* Place the GRE key extracted from
                                           L2GRE pkt header in Hash A buckets */
    bcmSwitchHashUseL2GreTunnelGreKey1 = 843, /* Place the GRE key extracted from
                                           L2GRE pkt header in Hash B buckets */
    bcmSwitchHashL2GrePayloadSelect0 = 844, /* Select inner L2/L3 fields to derive
                                           the Hash A buckets when an L2GRE pkt
                                           is received and the L2GRE tunnel is
                                           being terminated */
    bcmSwitchHashL2GrePayloadSelect1 = 845, /* Select inner L2/L3 fields to derive
                                           the Hash B buckets when an L2GRE pkt
                                           is received and the L2GRE tunnel is
                                           being terminated */
    bcmSwitchHashL2GreNetworkPortPayloadDisable0 = 846, /* Disable using the l2gre payload for
                                           hashing, for L2Gre pkts */
    bcmSwitchHashL2GreNetworkPortPayloadDisable1 = 847, /* Disable using the l2gre payload for
                                           hashing, for L2Gre pkts */
    bcmSwitchSystemReservedVlan = 848,  /* System Reserved VLAN */
    bcmSwitchProxySrcKnockout = 849,    /* If set, remove the source port during
                                           a proxy lookup operation of a packet
                                           which originated on this module. */
    bcmSwitchNetworkGroupDepth = 850,   /* Number of split horizon groups
                                           supported by the device */
    bcmSwitchUnknownSubportPktTagToCpu = 851, /* If set, send packet to CPU on
                                           following error conditions a. missing
                                           subport pkt tag and b. unknown
                                           subport pkt tag (lookup miss). */
    bcmSwitchL3RoutedLearn = 852,       /* enable L2 learning for routed
                                           packets. */
    bcmSwitchSubportPktTagEthertype = 853, /* Set EtherType field in LLTAG */
    bcmSwitchSubportPktTagToCpu = 854,  /* LLTAG present to CPU */
    bcmSwitchNonSubportPktTagToCpu = 855, /* LLTAG not present to CPU */
    bcmSwitchHashL2GreKeyMask0 = 856,   /* Set the mask which is AND-ed with
                                           hash L2GRE key before hash generation
                                           for hash A */
    bcmSwitchHashL2GreKeyMask1 = 857,   /* Set the mask which is AND-ed with
                                           hash L2GRE key before hash generation
                                           for hash B */
    bcmSwitchL3Max128BV6Entries = 858,  /* Max number of Number of 128B LPM
                                           entries. */
    bcmSwitchEEEQueueThresholdProfile0 = 859, /* Set EEE BB mode queue depth threshold
                                           Profile 0 */
    bcmSwitchEEEQueueThresholdProfile1 = 860, /* Set EEE BB mode queue depth threshold
                                           Profile 1 */
    bcmSwitchEEEQueueThresholdProfile2 = 861, /* Set EEE BB mode queue depth threshold
                                           Profile 2 */
    bcmSwitchEEEQueueThresholdProfile3 = 862, /* Set EEE BB mode queue depth threshold
                                           Profile 3 */
    bcmSwitchEEEPacketLatencyProfile0 = 863, /* Set EEE BB mode packet max latency
                                           Profile 0 */
    bcmSwitchEEEPacketLatencyProfile1 = 864, /* Set EEE BB mode packet max latency
                                           Profile 1 */
    bcmSwitchEEEPacketLatencyProfile2 = 865, /* Set EEE BB mode packet max latency
                                           Profile 2 */
    bcmSwitchEEEPacketLatencyProfile3 = 866, /* Set EEE BB mode packet max latency
                                           Profile 3 */
    bcmSwitchDosAttackV6MinFragEnable = 867, /* Enable Min pkt size check for
                                           fragmented IPv6. */
    bcmSwitchDosAttackV6MinFragSize = 868, /* Min pkt size for fragmented IPv6
                                           check. */
    bcmSwitchMirrorUnicastCosq = 869,   /* Unicast mirror COS. */
    bcmSwitchMirrorMulticastCosq = 870, /* Multicast mirror COS. */
    bcmSwitchMcQueueSchedMode = 871,    /* Schedule mode select for multicast
                                           queues. */
    bcmSwitchReserved872 = 872,         /* Reserved. */
    bcmSwitchCacheTableUpdateAll = 873, /* Update all cached memories. */
    bcmSwitchHashNivSrcIfEtagSvidSelect0 = 874, /* Select VN-TAG.src_vif or E-Tag.svid
                                           for hashing computation in Hash A
                                           buckets. */
    bcmSwitchHashNivSrcIfEtagSvidSelect1 = 875, /* Select VN-TAG.src_vif or E-Tag.svid
                                           for hashing computation in Hash B
                                           buckets. */
    bcmSwitchHashNivDstIfEtagVidSelect0 = 876, /* Select VN-TAG.dst_vif or E-Tag.vid
                                           for hashing computation in Hash A
                                           buckets. */
    bcmSwitchHashNivDstIfEtagVidSelect1 = 877, /* Select VN-TAG.dst_vif or E-Tag.vid
                                           for hashing computation in Hash B
                                           buckets. */
    bcmSwitchBstSnapshotEnable = 878,   /* BST resource usage SnapShot mode. */
    bcmSwitchSymmetricHashControl = 879, /* Enable/Disable symmetric hash, for
                                           Ip4, Ip6, FCoE packet, etc. */
    bcmSwitchL2ExtLearn = 880,          /* Learn L2 addresses on EXT_L2_ENTRY */
    bcmSwitchReserveHighTunnelId = 881, /* Set the highest inclusive bounds of
                                           the Tunnel ID resources.  Get will
                                           return the highest value supported by
                                           the device.  Setting 0 will remove
                                           the reserved range */
    bcmSwitchReserveLowTunnelId = 882,  /* Set the lower inclusive bounds of the
                                           Tunnel ID resources.  Get will return
                                           the lowest value supported by the
                                           device.  Setting 0 will remove the
                                           reserved range */
    bcmSwitchHigigCongestionClassEnable = 883, /* Enable the Congestion_Class field in
                                           HIGIG2 header to store the int_cn
                                           value in it. */
    bcmSwitchEcnNonIpResponsive = 884,  /* Set the default responsive indication
                                           value for NON-IP packets. */
    bcmSwitchEcnNonIpIntCongestionNotification = 885, /* Set the internal congestion
                                           notification (int_cn) value for
                                           NON-IP packets. */
    bcmSwitchMimPayloadTpidEnable = 886, /* Indicates which of MPLS_TPID[3:0] are
                                           allowed to match the TPID of a
                                           transit MinM frame's payload L2
                                           header's outer VLAN tag. */
    bcmSwitchHashMimUseTunnelHeader0 = 887, /* Indicates whether the tunnel or
                                           payload fields must be used for
                                           hashing for MIM traffic in field
                                           selection block A. */
    bcmSwitchHashMimUseTunnelHeader1 = 888, /* Indicates whether the tunnel or
                                           payload fields must be used for
                                           hashing for MIM traffic in field
                                           selection block B. */
    bcmSwitchMplsPipeTunnelLabelExpSet = 889, /* If set, MPLS pipe modes is supported
                                           with EXP field set. In this case exp
                                           is taken from the exp field in
                                           bcm_mpls_egress_label_s struct in the
                                           bcm_mpls_port_add api. Otherwise EXP
                                           field will be copied from previous
                                           label. Default is 0. */
    bcmSwitchControlPortConfigInstall = 890, /* Port config install. */
    bcmSwitchLoopbackMtuSize = 891,     /* MTU size in bytes check for packets
                                           ingressed on loopback port. */
    bcmSwitchHashVxlanPayloadSelect0 = 892, /* Set hash control to select VXLAN
                                           payload L2/L3 fields for Hash Block
                                           A. */
    bcmSwitchHashVxlanPayloadSelect1 = 893, /* Set hash control to select VXLAN
                                           payload L2/L3 fields for Hash Block
                                           B. */
    bcmSwitchCongestionCnmSrcMacNonOui = 894, /* Source MAC address in Congestion
                                           Notification Message Lower 3 bytes of
                                           MAC address to match */
    bcmSwitchCongestionCnmSrcMacOui = 895, /* Source MAC address in Congestion
                                           Notification Message Upper 3 bytes of
                                           MAC address to match */
    bcmSwitchHashL2MPLSPayloadSelect0 = 896, /* Set hash control to select L2 MPLS
                                           payload L2/L3 fields for Hash Block
                                           A. */
    bcmSwitchHashL2MPLSPayloadSelect1 = 897, /* Set hash control to select L2 MPLS
                                           payload L2/L3 fields for Hash Block
                                           B. */
    bcmSwitchHashL3L2MPLSField0 = 898,  /* Hash A Selection of L3 Payload for L2
                                           MPLS Packets */
    bcmSwitchHashL3L2MPLSField1 = 899,  /* Hash B Selection of L3 Payload for L2
                                           MPLS Packets */
    bcmSwitchHashIPSecSelect0 = 900,    /* Set hash control to select outer
                                           L4_SRC/DST_PORT or IPSec SPI for hash
                                           block A. */
    bcmSwitchHashIPSecSelect1 = 901,    /* Set hash control to select outer
                                           L4_SRC/DST_PORT or IPSec SPI for hash
                                           block B. */
    bcmSwitchECMPLevel1RandomSeed = 902, /* Seed for RNG used in ECMP Randomized
                                           Load Balancing (at Level1 in Two
                                           Level ECMP) */
    bcmSwitchECMPLevel2RandomSeed = 903, /* Seed for RNG used in ECMP Randomized
                                           Load Balancing at Level2 in Two Level
                                           ECMP */
    bcmSwitchTimesyncEgressTimestampingMode = 904, /* Selects 32bit or 48bit timestamping
                                           mode for 1588 packets. */
    bcmSwitchSubportCoEEtherType = 905, /* Set COE protocol ingress EtherType to
                                           match */
    bcmSwitchSubportEgressTpid = 906,   /* The TPID to use when the
                                           bcmPortControlSubportTagSize is 4
                                           Bytes */
    bcmSwitchSubportEgressWideTpid = 907, /* The TPID to use when the
                                           bcmPortControlSubportTagSize is 8
                                           Bytes */
    bcmSwitchIpmcSameVlanPruningOverride = 908, /* Set to override the IPMC same vlan
                                           pruning */
    bcmSwitchSynchronousPortClockSourceValid = 909, /* Returns whether synchronous clock can
                                           be sucessfully recovered from the
                                           port, configured via
                                           bcmSwitchSynchronousPortClockSource */
    bcmSwitchSynchronousPortClockSourceBkupValid = 910, /* Returns whether synchronous clock is
                                           successfully recovered from the back
                                           up port, configured via
                                           bcmSwitchSynchronousPortClockSourceBkup */
    bcmSwitchRemoteProtectionEnable = 911, /* Set the Protection Status bit in
                                           HiGig Header */
    bcmSwitchMirrorExclusive = 912,     /* Set Mirror exclusive behaviour
                                           between FP and Port */
    bcmSwitchEcnDelayMeasurementThreshold = 913, /* Set Ecn delay measurement threshold */
    bcmSwitchMacroFlowHashUseMSB = 914, /* If enable, use MSB for Macroflow
                                           based hashing */
    bcmSwitchMplsDefaultTtlCopy = 915,  /* Default value that is used when
                                           sending a packet from AC to PWE and
                                           the push-profile is copy */
    bcmSwitchOamStackingSupport = 916,  /* Use VC label field to carry OAM
                                           endpoint context to egress XGS switch */
    bcmSwitchHashOam = 917,             /* Hash select for OAM tables */
    bcmSwitchHashOamDual = 918,         /* Secondary hash select for OAM tables */
    bcmSwitchHashOamLeastFull = 919,    /* Use least full bank in dual hash OAM
                                           tables */
    bcmSwitchHashDualMoveDepthOam = 920, /* Maximum moves in dual hash mode for
                                           OAM tables */
    bcmSwitchPortVlanTranslationAdvanced = 921, /* Enable Port Advanced default vlan
                                           Edit mode. 1 - enable, 0 - disable */
    bcmSwitchForwardingLayerMtuFilter = 922, /* EnableDisable Forwarding Layer
                                           filtering per header format */
    bcmSwitchLinkLayerMtuFilter = 923,  /* EnableDisable Link Layer Filtering
                                           per header format */
    bcmSwitchForwardingLayerMtuSize = 924, /* Mtu size for Forwarding Layer
                                           Filtering */
    bcmSwitchLinkLayerMtuSize = 925,    /* MTU size for Link Layer Filtering */
    bcmSwitchMimBvidInsertionControl = 926, /* If 0, BVID is inserted into the MiM
                                           packet if the destination virtual
                                           port(bcm_mim_port_t object) is a
                                           Network port (legacy behavior). If
                                           set to 1, BVID is inserted into the
                                           MiM packet if the egress_tunnel_vlan
                                           field of the corresponding
                                           bcm_mim_port_t object contains a
                                           valid vlan id (1 .. 4095) */
    bcmSwitchL2CpuDeleteEvent = 927,    /* If enable, when SW initiates a DELETE
                                           operation to l2 table, SDK can get
                                           relative DELETE events from L2 Mode
                                           FIFO. */
    bcmSwitchL2LearnEvent = 928,        /* If enable, when HW does a LEARN
                                           operation to l2 table, SDK can get
                                           relative LEARN events from L2 Mode
                                           FIFO. */
    bcmSwitchL2CpuAddEvent = 929,       /* If enable, when SW initiates a INSERT
                                           operation to l2 table, SDK can get
                                           ADD events from L2 Mode FIFO. */
    bcmSwitchL2AgingEvent = 930,        /* If enable, when HW does a AGE
                                           operation to l2 table , SDK can get
                                           AGE events from L2 Mode FIFO. */
    bcmSwitchUdfHashEnable = 931,       /* Enable/Disable UDF hashing */
    bcmSwitchOlpMatchRule = 932,        /* Set Match rule value that identifies
                                           OLP packets when copied to CPU. */
    bcmSwitchMplsExtendedLabelTtl = 933, /* Set the TTL of the new MPLS label
                                           created when using extended label
                                           feature. */
    bcmSwitchMacroFlowHashOverlayMaxOffset = 934, /* Maximum offset value to be used in
                                           overlay ECMP offset table */
    bcmSwitchMacroFlowHashOverlayStrideOffset = 935, /* Offset increment to be used for
                                           populating overlay ECMP offset table. */
    bcmSwitchMacroFlowHashOverlayMinOffset = 936, /* Minimum offset value to be used in
                                           overlay ECMP offset table */
    bcmSwitchMacroFlowEcmpHashOverlayConcatEnable = 937, /* Enable hash concatenation for overlay
                                           layer ecmp macro flow. */
    bcmSwitchMacroFlowECMPOverlayHashSeed = 938, /* Hash Seed value for the overlay ECMP
                                           calculation */
    bcmSwitchECMPOverlayHashOffset = 939, /* Offset selections for enhanced
                                           overlay L3 ECMP. */
    bcmSwitchMacroFlowEcmpUnderlayHashConcatEnable = 940, /* 56960 Enable hash concatenation for
                                           ecmp level 2 macro flow. */
    bcmSwitchMacroFlowECMPUnderlayHashSeed = 941, /* 56960 Set Hash seed for ecmp level 2
                                           macro flow. */
    bcmSwitchECMPUnderlayVxlanHashOffset = 942, /* Enhanced hash select for VxLAN ECMP
                                           Level 2 hashing . */
    bcmSwitchECMPUnderlayL2GreHashOffset = 943, /* Enhanced hash select for L2Gre ECMP
                                           Level 2 hashing . */
    bcmSwitchECMPUnderlayTrillHashOffset = 944, /* Enhanced hash select for Trill ECMP
                                           Level 2 hashing . */
    bcmSwitchVirtualPortUnderlayDynamicHashOffset = 945, /* Enhanced hash select for Virtual Port
                                           Aggregation at Level 2. */
    bcmSwitchECMPUnderlayHashSet0Offset = 946, /* 56960 enhanced hash bits for ECMP
                                           Level 2. */
    bcmSwitchSampleFlexRandomSeed = 947, /* Set sFlow Flex RNG seed */
    bcmSwitchNonUcVlanShapingEnable = 948, /*  Enable/disable the ability of
                                           assigning single destination
                                           non-unicast traffic to service-based
                                           queues */
    bcmSwitchCustomHeaderEncapPriorityOffset = 949, /* Set the offset of the priority value
                                           in the encapsulated custom header */
    bcmSwitchIngCapwapEnable = 950,     /* Enable or disable ingress CAPWAP
                                           parser. */
    bcmSwitchCapwapDataUdpDstPort = 951, /* UDP dest port number of CAPWAP data
                                           packet. */
    bcmSwitchCountCapwapPayloadBytesOnly = 952, /* If set, count only the CAPWAP's
                                           wireless payload bytes for CAPWAP
                                           data packets. */
    bcmSwitchOamOlpStationMacOui = 953, /* Specify OUI bytes of BCM device MAC
                                           address used during OLP communication
                                           and OLP PKT sig match */
    bcmSwitchOamOlpStationMacNonOui = 954, /* Specify NON OUI bytes of BCM device
                                           MAC address used during OLP
                                           communication and OLP PKT sig match */
    bcmSwitchReserved955 = 955,         /* Reserved955 */
    bcmSwitchPFCDeadlockDetectionTimeInterval = 956, /* Set and Get the PFC detection time
                                           interval (0-100ms[default], 1-10ms). */
    bcmSwitchPFCDeadlockRecoveryAction = 957, /* Set and Get Recovery action to be
                                           performed in PFC deadlock recovery
                                           state. 0-Transmit Packets (Default),
                                           1-Drop. */
    bcmSwitchRangeCheckersAPIType = 958, /* Setting this switch control to 0
                                           allows to use only field APIs
                                           (bcm_field_range_XXX) and setting it
                                           to 1 allows to use only new range
                                           module APIs (bcm_range_XXX). Default
                                           value will be set to 0. */
    bcmSwitchReserved958 = 959,         /* Reserved958 */
    bcmSwitchEcmpGroupMemberMode = 960, /* Set/get ECMP group mode */
    bcmSwitchMimlEncapInnerEthertype = 961, /* Set MIML-ETYPE (inner EtherType)
                                           field in MiML for encapsulation.
                                           Value of unit16 type. */
    bcmSwitchHashMiMPayloadSelect0 = 962, /* Set hash control to select MiM L2/L3
                                           payload fields for Hash Block A */
    bcmSwitchHashMiMPayloadSelect1 = 963, /* Set hash control to select MiM L2/L3
                                           payload fields for Hash Block B */
    bcmSwitchHashL3MiMField0 = 964,     /* L3 MiM enhanced hash field for Hash
                                           Block A */
    bcmSwitchHashL3MiMField1 = 965,     /* L3 MiM enhanced hash field for Hash
                                           Block B */
    bcmSwitchHashMPLSUseLabelStack = 966, /* If there are more than 3 labels,
                                           label stack (up to 5 labels) is used
                                           for hashing else Top 3 MPLS labels +
                                           payload IPV4/IPv6 fields will be
                                           selected for hashing */
    bcmSwitchHashMPLSExcludeGAL = 967,  /* To exclude GAL label from hash
                                           computation */
    bcmSwitchRedirectBufferThresholdPriorityLow = 968, /* Set the buffer threshold limit for
                                           low priority redirected packets */
    bcmSwitchRedirectBufferThresholdPriorityMedium = 969, /* Set the buffer threshold limit for
                                           medium priority redirected packets */
    bcmSwitchRedirectBufferThresholdPriorityHigh = 970, /* Set the buffer threshold limit for
                                           high priority redirected packets */
    bcmSwitchFlexIP6ExtHdr = 971,       /* Programmable value for user defined
                                           extension header 1 & 2. */
    bcmSwitchFlexIP6ExtHdrEgress = 972, /* Egress programmable value for user
                                           defined extension header 1 & 2. */
    bcmSwitchHashUseFlowSelMplsEcmp = 973, /* Enable/Disable flow based hashing for
                                           MPLS ECMP. */
    bcmSwitchV6L3SrcDstLocalLinkDropCancel = 974, /* Do not drop IPv6. It will override
                                           drop control
                                           bcmSwitchV6L3LocalLinkDrop if source
                                           and destination are local link. */
    bcmSwitchL3ClassIdForL2Enable = 975, /* Enable/Disable L3 Lookup to get
                                           classids based on DIP & SIP for L2
                                           Switched packets. */
    bcmSwitchDropSobmhOnLinkDown = 976, /* Enable/Disable SOBMH blocking on link
                                           down egress port */
    bcmSwitchResilientHashCRC0ByteMask = 977, /* Set mask for CRC0 stateful hashing
                                           function in 2Byte resolution */
    bcmSwitchResilientHashCRC1ByteMask = 978, /* Set mask for CRC1 stateful hashing
                                           function in 2Byte resolution */
    bcmSwitchResilientHashCRC2ByteMask = 979, /* Set mask for CRC2 stateful hashing
                                           function in 2Byte resolution */
    bcmSwitchResilientHashCRC3ByteMask = 980, /* Set mask for CRC3 stateful hashing
                                           function in 2Byte resolution */
    bcmSwitchResilientHashCRCSeed = 981, /* Set seed for stateful CRC hashing
                                           functions */
    bcmSwitchECMPResilientHashKey0Shift = 982, /* Shift left the
                                           Stateful-Hashing-Lb-Key(47 bit) to
                                           create ECMP key-0 */
    bcmSwitchECMPResilientHashKey1Shift = 983, /* Shift right the
                                           Stateful-Hashing-Lb-Key(47 bit) to
                                           create ECMP key-1 */
    bcmSwitchVpnDetachPorts = 984,      /* Set VPN destroy mode to either delete
                                           ports that are attached to the VSI or
                                           ignore them. Default: 1 - Destroy
                                           attached ports. */
    bcmSwitchClassL3L2Marking = 985,    /* Set the
                                           In-Lif-Profile->Resolved-In-Lif-Profile
                                           map (In-Lif-Profile mapping */
    bcmSwitchL3L2MarkingMode = 986,     /* Set the DP map mode */
    bcmSwitchSTPDisabledFieldBypass = 987, /* Disable FP when STP state is
                                           disabled. */
    bcmSwitchHashSctpL4Port = 988,      /* To set that RTAG7 hashing can be
                                           performed on SCTP. */
    bcmSwitchReserved988 = 989,         /* Reserved988. */
    bcmSwitchGlobalTpidEnable = 990,    /* Enable/Disable global TPIDs. */
    bcmSwitchECMPResilientHashCombine = 991, /* Combine both regular and resilient
                                           hashing for ECMP key. */
    bcmSwitchHashSetTflowMode = 992,    /* enable/disable using hash offset
                                           b0/b1 regardless of th_tflow_enabled */
    bcmSwitchDowngrade = 993,           /* set the switch for a downgrade in
                                           Warmboot */
    bcmSwitchHashELISearch = 994,       /* Enable ELI search and the use of the
                                           EL label for LAG and ECMP load
                                           balancing. */
    bcmSwitchCrashRecoveryMode = 995,   /* Determine the Crash recovery mode
                                           0:off 1:API 2:on demand. */
    bcmSwitchCrCommit = 996,            /* If woekinging in on demand CR, use
                                           this to trigger a commit. */
    bcmSwitchCrLastTransactionStatus = 997, /* Query after a crash if last
                                           transaction went through. */
    bcmSwitchCrCouldNotRecover = 998,   /* Query after A crash if program
                                           succesfully recovered. */
    bcmSwitchHWL2Freeze = 999,          /* Freeze/thaw L2 H/W activity. */
    bcmSwitchHashIP6NextHeaderUseExtensionHeader0 = 1000, /* Hash Control to select the IPv6 Next
                                           Header field for hash Block A. If
                                           set, use Extension Header, else use
                                           current Next Header field. */
    bcmSwitchHashIP6NextHeaderUseExtensionHeader1 = 1001, /* Hash Control to select the IPv6 Next
                                           Header field for hash Block B. If
                                           set, use Extension Header, else use
                                           current Next Header field. */
    bcmSwitchHashUseFlowSelLbidNonUnicast = 1002, /* Enable/Disable flow based hashing for
                                           non-unicast LBID. */
    bcmSwitchHashUseFlowSelLbidUnicast = 1003, /* Enable/Disable flow based hashing for
                                           unicast LBID. */
    bcmSwitchHashUseFlowSelHigigTrunkNonUnicast = 1004, /* Enable/Disable flow based hashing for
                                           non-unicast HiGig Trunk. */
    bcmSwitchHashUseFlowSelHigigTrunkUnicast = 1005, /* Enable/Disable flow based hashing for
                                           unicast HiGig Trunk. */
    bcmSwitchHashUseFlowSelHigigTrunkFailover = 1006, /* Enable/Disable flow based hashing for
                                           HiGig Trunk Failover. */
    bcmSwitchHashUseFlowSelTrunkNonUnicast = 1007, /* Enable/Disable flow based hashing for
                                           non-unicast Trunk. */
    bcmSwitchHashUseFlowSelEntropy = 1008, /* Enable/Disable flow based hashing for
                                           Entropy. */
    bcmSwitchHashUseFlowSelVxlanEcmp = 1009, /* Enable/Disable flow based hashing for
                                           VXLAN ECMP. */
    bcmSwitchDefaultNativeOutVlanPort = 1010, /* Default native out VLAN Port. */
    bcmSwitchDefaultEgressVlanEditClassId = 1011, /* Default Vlan Edit Class Id for Egress
                                           VLAN translation. */
    bcmSwitchGtpDetectEnable = 1012,    /* Enable/disable the capability for GTP
                                           packet detection. */
    bcmSwitchGtpHdrFirstByte = 1013,    /* Setup the value for GTP Header First
                                           Byte. */
    bcmSwitchGtpHdrFirstByteMask = 1014, /* Setup the mask for GTP Header First
                                           Byte. */
    bcmSwitchHashGtpTeidEnable0 = 1015, /* Enable using the Tunnel endpoint id
                                           for hashing, for GTP pkts. */
    bcmSwitchHashGtpTeidEnable1 = 1016, /* Enable using the Tunnel endpoint id
                                           for hashing, for GTP pkts. */
    bcmSwitchEccSingleBitErrorEvent = 1017, /* Enable/Disable 1bit ECC error
                                           reporting for all the memories which
                                           are protected by ECC logic and whose
                                           Parity/ECC error could only be
                                           corrected by H/W. */
    bcmSwitchHeaderCompensationPerPacket = 1018, /* Set the header compensation of
                                           packets according to the MSBs of its
                                           CUD. */
    bcmSwitch3rdTagStampMode = 1019,    /* Set the 3rd tag stamping mode,
                                           0:Dsiable, 1: Enable. */
    bcmSwitchMacroFlowHigigTrunkHashMinOffset = 1020, /* Minumum offset value to be used in
                                           RTAG7 macroflow for HiGig Trunk */
    bcmSwitchMacroFlowHigigTrunkHashMaxOffset = 1021, /* Maximum offset value to be used in
                                           RTAG7 macroflow for HiGig Trunk */
    bcmSwitchMacroFlowHigigTrunkHashStrideOffset = 1022, /* Offset increment value to be used in
                                           RTAG7 macroflow for HiGig Trunk */
    bcmSwitchMacroFlowTrunkHashMinOffset = 1023, /* Minumum offset value to be used in
                                           RTAG7 macroflow for Trunk */
    bcmSwitchMacroFlowTrunkHashMaxOffset = 1024, /* Maximum offset value to be used in
                                           RTAG7 macroflow for Trunk */
    bcmSwitchMacroFlowTrunkHashStrideOffset = 1025, /* Offset increment value to be used in
                                           RTAG7 macroflow for Trunk */
    bcmSwitchMacroFlowLoadBalanceEntropyHashMinOffset = 1026, /* Minumum offset value to be used in
                                           RTAG7 macroflow for Load Balance
                                           ID/Entropy */
    bcmSwitchMacroFlowLoadBalanceEntropyHashMaxOffset = 1027, /* Maximum offset value to be used in
                                           RTAG7 macroflow for Load Balance
                                           ID/Entropy */
    bcmSwitchMacroFlowLoadBalanceEntropyHashStrideOffset = 1028, /* Offset increment value to be used in
                                           RTAG7 macroflow for Load Balance
                                           ID/Entropy */
    bcmSwitchCrTransactionStart = 1029, /* If working in on demand CR, use this
                                           to start a new transaction. */
    bcmSwitchUdpTunnelIPv4DstPort = 1030, /* UDP destination port for IPv4
                                           encapsulation in UDP tunnel. */
    bcmSwitchUdpTunnelIPv6DstPort = 1031, /* UDP destination port for IPv6
                                           encapsulation in UDP tunnel. */
    bcmSwitchUdpTunnelMplsDstPort = 1032, /* UDP destination port for MPLS
                                           encapsulation in UDP tunnel. */
    bcmSwitchECMPSecondHierHashOffset = 1033, /* BCM88680: ECMP second hierarchy Hash
                                           key is barrel shifted by this value. */
    bcmSwitchECMPSecondHierHashConfig = 1034, /* BCM88680: Selects hashing polynomial
                                           for the second hierarchy of the ECMP. */
    bcmSwitchIngressRateLimitMpps = 1035, /* Limit ingress rate in Mega Packets
                                           Per Second. */
    bcmSwitchMirrorSnoopForwardOriginalWhenDropped = 1036, /* When dropping the mirror and/or snoop
                                           packet copies due to FIFOs being
                                           full, do not drop the original
                                           (forward) copy */
    bcmSwitchFabricTrunkDynamicPhysicalQueuedBytesExponent = 1037, /* The exponent used in the calculation
                                           of exponentially weighted moving
                                           average of historical member bytes in
                                           physical queue. */
    bcmSwitchFabricTrunkDynamicPhysicalQueuedBytesDecreaseReset = 1038, /* If set, the historical member bytes
                                           in physical queue is updated with the
                                           instantaneous value if the latter is
                                           smaller. */
    bcmSwitchFabricTrunkDynamicPhysicalQueuedBytesMinThreshold = 1039, /* The minimum threshold, in bytes, used
                                           to quantize historical member bytes
                                           in physical queue. */
    bcmSwitchFabricTrunkDynamicPhysicalQueuedBytesMaxThreshold = 1040, /* The maximum threshold, in bytes, used
                                           to quantize historical member bytes
                                           in physical queue. */
    bcmSwitchTrunkDynamicPhysicalQueuedBytesExponent = 1041, /* The exponent used in the calculation
                                           of exponentially weighted moving
                                           average of historical member bytes in
                                           physical queue. */
    bcmSwitchTrunkDynamicPhysicalQueuedBytesDecreaseReset = 1042, /* If set, the historical member bytes
                                           in physical queue is updated with the
                                           instantaneous value if the latter is
                                           smaller. */
    bcmSwitchTrunkDynamicPhysicalQueuedBytesMinThreshold = 1043, /* The minimum threshold, in bytes, used
                                           to quantize historical member bytes
                                           in physical queue. */
    bcmSwitchTrunkDynamicPhysicalQueuedBytesMaxThreshold = 1044, /* The maximum threshold, in bytes, used
                                           to quantize historical member bytes
                                           in physical queue. */
    bcmSwitchEcmpDynamicPhysicalQueuedBytesExponent = 1045, /* The exponent used in the calculation
                                           of exponentially weighted moving
                                           average of historical member bytes in
                                           physical queue. */
    bcmSwitchEcmpDynamicPhysicalQueuedBytesDecreaseReset = 1046, /* If set, the historical member bytes
                                           in physical queue is updated with the
                                           instantaneous value if the latter is
                                           smaller. */
    bcmSwitchEcmpDynamicPhysicalQueuedBytesMinThreshold = 1047, /* The minimum threshold, in bytes, used
                                           to quantize historical member bytes
                                           in physical queue. */
    bcmSwitchEcmpDynamicPhysicalQueuedBytesMaxThreshold = 1048, /* The maximum threshold, in bytes, used
                                           to quantize historical member bytes
                                           in physical queue. */
    bcmSwitchFabricTrunkDynamicRandomSeed = 1049, /* Seed for RNG used in HiGig Trunk
                                           Dynamic Load Balancing */
    bcmSwitchTrunkDynamicRandomSeed = 1050, /* Seed for RNG used in Trunk Dynamic
                                           Load Balancing */
    bcmSwitchEcmpDynamicRandomSeed = 1051, /* Seed for RNG used in ECMP Dynamic
                                           Load Balancing */
    bcmSwitchHashMPLSExcludeReservedLabel = 1052, /* To exclude all reserved labels from
                                           hash computation */
    bcmSwitchMplsNonIpPayloadDefaultEcn = 1053, /* To set the default ECN for L2 MPLS
                                           termination flows for non-IP payloads */
    bcmSwitchECMPHashBitCountSelect = 1054, /* To select the most significant bits
                                           of the 16-bit ECMP hash value before
                                           the modulo function */
    bcmSwitchEcnIpPayloadResponsive = 1055, /* To set to use payload IP header's IP
                                           protocol field to determine
                                           responsive attribute of the packet
                                           for transiting tunnels */
    bcmSwitchHashMultiMoveDepthExactMatch = 1056, /* Maximum moves in multi hash mode for
                                           FPEM tables. */
    bcmSwitchCrAbort = 1057,            /* If working in on demand CR, use this
                                           to abort the current transaction. */
    bcmSwitchLIUdpSrcPortSet = 1058,    /* Set UDP source port for Lawful
                                           Interception tunnel. */
    bcmSwitchRoce1Ethertype = 1059,     /* Set l2 EtherType to match RoCEv1
                                           packets. */
    bcmSwitchRoce2UdpDstPort = 1060,    /* Set udp dest port to match RoCEv2
                                           packets. */
    bcmSwitchHashUseFlowSelRoce2Ecmp = 1061, /* Enable/Disable flow based hashing for
                                           RoCEv2 ECMP. */
    bcmSwitchHashSelectControlExtended = 1062, /* Extended hash field selection control */
    bcmSwitchHashField0Roce1FlowLabel = 1063, /* Enable RoCEv1 GRH(Global Route
                                           Header) flow label for Block A Hash
                                           Field Selection. */
    bcmSwitchHashField1Roce1FlowLabel = 1064, /* Enable RoCEv1 GRH(Global Route
                                           Header) flow label for Block B Hash
                                           Field Selection. */
    bcmSwitchHashField0Roce2FlowLabel = 1065, /* Enable RoCEv2 IPv6 flow label for
                                           Block A Hash Field Selection. */
    bcmSwitchHashField1Roce2FlowLabel = 1066, /* Enable RoCEv2 IPv6 flow label for
                                           Block B Hash Field Selection. */
    bcmSwitchHashRoce1Field0 = 1067,    /* Hash block A field selection for
                                           RoCEv1 packets. */
    bcmSwitchHashRoce1Field1 = 1068,    /* Hash block B field selection for
                                           RoCEv1 packets. */
    bcmSwitchHashRoce2IP4Field0 = 1069, /* Hash block A field selection for
                                           RoCEv2 IPv4 packets. */
    bcmSwitchHashRoce2IP4Field1 = 1070, /* Hash block B field selection for
                                           RoCEv2 IPv4 packets. */
    bcmSwitchHashRoce2IP6Field0 = 1071, /* Hash block A field selection for
                                           RoCEv2 IPv6 packets. */
    bcmSwitchHashRoce2IP6Field1 = 1072, /* Hash block B field selection for
                                           RoCEv2 IPv6 packets. */
    bcmSwitchEcmpRoce2HashOffset = 1073, /* Enhanced hash selection for RoCEv2
                                           ECMP port based hashing. */
    bcmSwitchVxlanUdpDestPortSet1 = 1074, /* Set UDP Destination port for VXLAN
                                           (alternate). */
    bcmSwitchVxlanEntropyEnableIP6 = 1075, /* Set IPv6 Flow Label for VXLAN Tunnel
                                           Entropy (Value = TRUE/FALSE). */
    bcmSwitchE2EFCReceiveDestMacOui = 1076, /* E2EFC message MAC upper 3 bytes to
                                           match, only for E2EFC RX. */
    bcmSwitchE2EFCReceiveDestMacNonOui = 1077, /* E2EFC message MAC lower 3 bytes to
                                           match, only for E2EFC RX. */
    bcmSwitchE2EFCReceiveEtherType = 1078, /* E2EFC message EtherType/length to
                                           match, only for E2EFC RX. */
    bcmSwitchE2EFCReceiveOpcode = 1079, /* E2EFC message opcode to match, only
                                           for E2EFC RX. */
    bcmSwitchHashUseFlowSelEcmpOverlay = 1080, /* Enable/Disable flow based hashing for
                                           ECMP LEVEL 1. */
    bcmSwitchMplsSpeculativeNibbleMap = 1081, /* Map first nibble after MPLS header to
                                           speculative next protocol. */
    bcmSwitchIpmcSameFwdDmnThreshold = 1082, /* Set the threshold for 1:1 mapped
                                           VLANs and VPNs for IPMC packet flow
                                           for RIOT */
    bcmSwitchHashUseFlowSelVpTrunk = 1083, /* Enable/Disable flow based hashing for
                                           VP Trunk. */
    bcmSwitchHashUseFlowSelTrunkResilientHash = 1084, /* Enable/Disable flow based hashing for
                                           Trunk RH. */
    bcmSwitchMeterMef10dot3Enable = 1085, /* Enable meter mef 10.3 mode. */
    bcmSwitchHashMultiMoveDepthVlanTranslate1 = 1086, /* Maximum moves in multi hash mode for
                                           vlan translate1 tables. */
    bcmSwitchHashMultiMoveDepthVlanTranslate2 = 1087, /* Maximum moves in multi hash mode for
                                           vlan translate2 tables. */
    bcmSwitchHashMultiMoveDepthEgressVlanTranslate1 = 1088, /* Maximum moves in multi hash mode for
                                           vlan egress translate1 tables. */
    bcmSwitchHashMultiMoveDepthEgressVlanTranslate2 = 1089, /* Maximum moves in multi hash mode for
                                           vlan egress translate2 tables. */
    bcmSwitchIntL4DestPort1 = 1090,     /* INT(Inband Network Telemetry) packet
                                           UDP destination port1. */
    bcmSwitchIntL4DestPort2 = 1091,     /* INT packet UDP destination port2. */
    bcmSwitchIntProbeMarker1 = 1092,    /* INT header ProbeMarker1. */
    bcmSwitchIntProbeMarker2 = 1093,    /* INT header ProbeMarker2. */
    bcmSwitchIntVersion = 1094,         /* INT header version. */
    bcmSwitchIntRequestVectorValue = 1095, /* INT header request vector. */
    bcmSwitchIntRequestVectorMask = 1096, /* INT header request vector mask. */
    bcmSwitchIntSwitchId = 1097,        /* INT payload switch ID. */
    bcmSwitchIntEgressTimeDelta = 1098, /* Time in nano-second that is added to
                                           current time stamp to adjust latency
                                           from EP to port. */
    bcmSwitchIntHopLimitCpuEnable = 1099, /* Enable copy to CPU on hop limit
                                           condition. */
    bcmSwitchIntTurnAroundCpuEnable = 1100, /* Enable copy to CPU on egress port as
                                           turnaround port. */
    bcmSwitchIntMaxPayloadLength = 1101, /* INT max payload length. Only apply to
                                           devices using global value */
    bcmSwitchForwardLookupNotFoundTrap = 1102, /* Set foward lookup not found Trap ID. */
    bcmSwitchEtherTypeCustom1 = 1103,   /* Set Custom_1 Ethernet type. */
    bcmSwitchEtherTypeCustom2 = 1104,   /* Set Custom_2 Ethernet type. */
    bcmSwitchL3LpmHitbitEnable = 1105,  /* Enable hit bit lookup update in the
                                           LPM table. */
    bcmSwitchMplsCtrlPktToCpu = 1106,   /* MPLS control packets to CPU. */
    bcmSwitchInvalidTpidToCpu = 1107,   /* Invalid tag protocol ID packets to
                                           CPU. */
    bcmSwitchFlowSelectMissToCpu = 1108, /* Packets that miss flow selection to
                                           CPU. */
    bcmSwitchL3HdrMismatchToCpu = 1109, /* L3 header mismatch packets to CPU. */
    bcmSwitchMeterTrafficClass = 1110,  /* Meter Traffic Class value mapped from
                                           internal Traffic Class */
    bcmSwitchRoeEthertype = 1111,       /* EtherType value for ROE frames after
                                           tunnel decap. */
    bcmSwitchRoeCustomEthertype = 1112, /* EtherType value for ROE Custom frames
                                           after tunnel decap. */
    bcmSwitchRoeUdpDestPort = 1113,     /* Set UDP Destination port for ROE
                                           frames. */
    bcmSwitchRoeCustomUdpDestPort = 1114, /* Set UDP Destination port for ROE
                                           Custom frames. */
    bcmSwitchOamOlpHeaderAdd = 1115,    /* Enable or disable OLP header addition
                                           to OAM packets copied to CPU. OLP
                                           header addition is enabled by default
                                           for OAM packets. Please note that
                                           Embedded applications such as
                                           Ethernet LM/DM and MPLS LM/DM will
                                           not work correctly if OLP header
                                           addition is disabled. */
    bcmSwitchL2FreezeMode = 1116,       /* Use global forward mode for higher
                                           efficiency or per port no learn mode
                                           if some ports shouldn't forward. */
    bcmSwitchL3McRpfDefaultRoute = 1117, /* Enable SIP base MC RPF default route
                                           check. */
    bcmSwitchIngParseL2TunnelTermArp = 1118, /* Enable parsing of payload ARP header
                                           for L2 tunnel terminated packets. */
    bcmSwitchIngParseL2TunnelTermRarp = 1119, /* Enable parsing of payload RARP header
                                           for L2 tunnel terminated packets. */
    bcmSwitchNoneEthernetPacketMeterPtr = 1120, /* Ethernet meter pointer for none
                                           Ethernet packets. */
    bcmSwitchEtherDscpRemark = 1121,    /* Enable copying IP header.TOS to
                                           In-DSCP-EXP. */
    bcmSwitchDosAttackTcpFirstOption = 1122, /* Configure TCP option number for
                                           switch control
                                           bcmSwitchDosAttackTcpFlagsSynOption.
                                           Only first option in the TCP option
                                           can be checked and compared to with
                                           this value. */
    bcmSwitchDosAttackInnerTcpFirstOption = 1123, /* Configure Inner TCP option number for
                                           switch control
                                           bcmSwitchDosAttackTcpFlagsSynOption.
                                           Only first option in the TCP option
                                           can be checked and compared to with
                                           this value. */
    bcmSwitchDosAttackTcpFlagsMask = 1124, /* Configure TCP control flags valid
                                           bits for Dos Attack. This value is
                                           used for generating Dos Attack for
                                           bcmSwitchDosAttackTcpFlagsZero and/or
                                           bcmSwitchDosAttackTcpFlagsResvdNonZero. */
    bcmSwitchDosAttackInnerTcpFlagsMask = 1125, /* Configure Inner TCP control flags
                                           valid bits for Dos Attack. This value
                                           is used for generating Dos Attack for
                                           bcmSwitchDosAttackTcpFlagsZero and/or
                                           bcmSwitchDosAttackTcpFlagsResvdNonZero. */
    bcmSwitchDosAttackTcpFlagsSR = 1126, /* Enable DOS attack checking for TCP
                                           packet with SYN and RST Flags set. */
    bcmSwitchDosAttackTcpFlagsZero = 1127, /* Enable DOS attack checking for TCP
                                           packet in which TCP Control Flag is
                                           Zero. Switch Control
                                           bcmSwitchDosAttackTcpFlagsMask and/or
                                           bcmSwitchDosAttackInnerTcpFlagsMask
                                           must be configured to determine valid
                                           TCP Flag bits. */
    bcmSwitchDosAttackTcpFlagsSynOption = 1128, /* Enable DOS attack checking for TCP
                                           packet with SYN Flag set and option
                                           number in payload. Switch Control
                                           bcmSwitchDosAttackTcpFirstOption
                                           and/or
                                           bcmSwitchDosAttackInnerTcpFirstOption
                                           must be set required option number
                                           for matching. */
    bcmSwitchDosAttackTcpFlagsSynFirstFrag = 1129, /* Enable DOS attack checking for TCP
                                           packet with SYN Flags set in first
                                           fragment. */
    bcmSwitchDosAttackTcpFlagsFinNoAck = 1130, /* Enable DOS attack checking for TCP
                                           packet with flags FIN set and ACK not
                                           set. */
    bcmSwitchDosAttackTcpFlagsRstNoAck = 1131, /* Enable DOS attack checking for TCP
                                           packet with flags RST set and ACK not
                                           set. */
    bcmSwitchDosAttackTcpFlagsNoSFRA = 1132, /* Enable DOS attack checking for TCP
                                           packet with no SYN, FIN, RST, ACK
                                           Flag set. */
    bcmSwitchDosAttackTcpFlagsUrgNoUrgPtr = 1133, /* Enable DOS attack checking for TCP
                                           packet with URG flag set and Null
                                           Urgent Pointer. */
    bcmSwitchDosAttackTcpFlagsNoUrgWithUrgPtr = 1134, /* Enable DOS attack checking for TCP
                                           packet with URG flag not set and
                                           non-null Urgent Pointer. */
    bcmSwitchDosAttackTcpDstPortZero = 1135, /* Enable DOS attack checking for TCP
                                           packet with Destination port as zero. */
    bcmSwitchDosAttackTcpSrcPortZero = 1136, /* Enable DOS attack checking for TCP
                                           packet with Source Port as zero. */
    bcmSwitchDosAttackTcpFlagsResvdNonZero = 1137, /* Enable DOS attack checking for TCP
                                           packet with Reserved flags non-zero.
                                           Switch Control
                                           bcmSwitchDosAttackTcpFlagsMask and/or
                                           bcmSwitchDosAttackInnerTcpFlagsMask
                                           must be configured to determine
                                           reserved flag bits. */
    bcmSwitchDosAttackTcpV4DstBcast = 1138, /* Enable DOS attack checking for TCP
                                           packet with Ipv4 Broadcast DIP
                                           address. */
    bcmSwitchDosAttackTcpFlagsAck = 1139, /* Enable DOS attack checking for TCP
                                           packet with ACK flag set and
                                           Acknowledgement number is zero. */
    bcmSwitchMirrorPsampFormat2Epoch = 1140, /* Set EPOCH value in PSAMP format2
                                           header */
    bcmSwitchL2StationExtendedMode = 1141, /* Set L2 station extended memory global
                                           mode. */
    bcmSwitchHashSeed = 1142,           /* Set the seed of the selected hash
                                           function. */
    bcmSwitchLayerRecordModeSelection = 1143, /* Select the working mode of the layer
                                           records. */
    bcmSwitchECMPThirdHierHashConfig = 1144, /* Selects hashing polynomial for the
                                           third hierarchy of the ECMP. */
    bcmSwitchNwkHashConfig = 1145,      /* Selects hashing polynomial for the
                                           network load balancing key
                                           generation. */
    bcmSwitchMplsStack0HashSeed = 1146, /* Set the seed of the first MPLS stack
                                           hash. */
    bcmSwitchMplsStack1HashSeed = 1147, /* Set the seed of the second MPLS stack
                                           hash. */
    bcmSwitchParserHashSeed = 1148,     /* Set the seed of the parser stage
                                           hashing. */
    bcmSwitchMplsSpeculativeParse = 1149, /* Enable/disable the MPLS speculative
                                           parsing. */
    bcmSwitchExternalTcamSync = 1150,   /* Enum for external TCAM sync. */
    bcmSwitchArpMyIp1 = 1151,           /* My-ARP trap configuration of IP 1. */
    bcmSwitchArpMyIp2 = 1152,           /* My-ARP trap configuration of IP 2. */
    bcmSwitchArpIgnoreDa = 1153,        /* ARP trap configuration of Ignore DA. */
    bcmSwitchNdpMyIp1 = 1154,           /* My-NDP trap configuration of IP 1. */
    bcmSwitchNdpMyIp2 = 1155,           /* My-NDP trap configuration of IP 2. */
    bcmSwitchIcmpIgnoreDa = 1156,       /* ICMP trap configuration of Ignore DA. */
    bcmSwitchVxlanEvpnEnable = 1157,    /* enable/disable VXLAN EVPN. */
    bcmSwitchHashDualMoveDepthIngVpVlanMember = 1158, /* Maximum moves in dual hash mode for
                                           ING_VP_VLAN_MEMBERSHIP table. */
    bcmSwitchHashDualMoveDepthEgrVpVlanMember = 1159, /* Maximum moves in dual hash mode for
                                           EGR_VP_VLAN_MEMBERSHIP table. */
    bcmSwitchGpeEntropyEnable = 1160,   /* Set UDP Source port for GPE Tunnel
                                           Entropy(Value = TRUE/FALSE). */
    bcmSwitchGpeUdpDestPortSet = 1161,  /* Set UDP Destination port for GPE. */
    bcmSwitchGeneveEntropyEnable = 1162, /* Set UDP Source port for GENEVE Tunnel
                                           Entropy(Value = TRUE/FALSE). */
    bcmSwitchGeneveUdpDestPortSet = 1163, /* Set UDP Destination port for GENEVE. */
    bcmSwitchBfdRxDisable = 1164,       /* Disable BFD Rx packet processing. */
    bcmSwitchDescCommit = 1165,         /* Commit descriptor DMA chain to HW. */
    bcmSwitchDlbMonitorIngressRandomSeed = 1166, /* Set RNG seed for DLB flow monitoring */
    bcmSwitchCounterAdjustEgressTm = 1167, /* Counter compensation specifically for
                                           ETM. */
    bcmSwitchCounterAdjustEgressPp = 1168, /* Counter compensation specifically for
                                           ERPP. */
    bcmSwitchTagPcpDeiSrc = 1169,       /* Set Source of PCP_DEi for SIT */
    bcmSwitchBfdMyDipDestination = 1170, /* Set destination for BFD IP address
                                           forwarding. */
    bcmSwitchEndToEndLatencyPerDestinationPort = 1171, /* Define if end to end latency is made
                                           per destination port or not. */
    bcmSwitchExternalTcamInit = 1172,   /* Init/De-init External TCAM. */
    bcmSwitchHashMultiMoveDepthL3Tunnel = 1173, /* Maximum moves in multi hash mode for
                                           L3 Tunnel tables. */
    bcmSwitchGbpEtherType = 1174,       /* Specify the EtherType to be used for
                                           GBP protocol */
    bcmSwitchVxlanUdpSrcPortSelect = 1175, /* Select UDP source port for VxLAN
                                           tunnel */
    bcmSwitchGpeUdpSrcPortSelect = 1176, /* Select UDP source port for GPE tunnel */
    bcmSwitchGeneveUdpSrcPortSelect = 1177, /* Select UDP source port for GENEVE
                                           tunnel */
    bcmSwitchPreserveOtagBeforeItag = 1178, /* Control for preserved OTAG order wrt.
                                           ITAG */
    bcmSwitchMirrorCpuCosq = 1179,      /* Mirror COS for CPU port as MTP */
    bcmSwitchIntProbeMarkerEnable = 1180, /* INT packet ProbeMarker(UDF) parser
                                           Enable. */
    bcmSwitchIntL4DestPortEnable = 1181, /* INT packet UDP destination port
                                           parser Enable. */
    bcmSwitchFtmhEtherType = 1182,      /* Set FTMH header EtherType to match */
    bcmSwitchGbpSidDidMcastLookupMissClassid = 1183, /* Default class id to be used when
                                           there is a miss in GBP SID_DID lookup
                                           for multicast flows. */
    bcmSwitchCustomOuterEthertype1 = 1184, /* Outer EtherType 1 used for extracting
                                           packet fields in custom applications. */
    bcmSwitchCustomOuterEthertype2 = 1185, /* Outer EtherType 2 used for extracting
                                           packet fields in custom applications. */
    bcmSwitchCustomInnerEthertype1 = 1186, /* Inner EtherType 1 used for extracting
                                           packet fields in custom applications. */
    bcmSwitchCustomInnerEthertype2 = 1187, /* Inner EtherType 2 used for extracting
                                           packet fields in custom applications. */
    bcmSwitchCustomOuterProtocolId1 = 1188, /* Outer Protocol ID 1 used for
                                           extracting packet fields in custom
                                           applications. */
    bcmSwitchCustomOuterProtocolId2 = 1189, /* Outer Protocol ID 2 used for
                                           extracting packet fields in custom
                                           applications. */
    bcmSwitchCustomInnerProtocolId1 = 1190, /* Inner Protocol ID 1 used for
                                           extracting packet fields in custom
                                           applications. */
    bcmSwitchCustomInnerProtocolId2 = 1191, /* Inner Protocol ID 2 used for
                                           extracting packet fields in custom
                                           applications. */
    bcmSwitchCustomOuterUdpDstPort1 = 1192, /* Outer UDP Dest Port 1 used for
                                           extracting packet fields in custom
                                           applications. */
    bcmSwitchCustomOuterUdpDstPort2 = 1193, /* Outer UDP Dest Port 2 used for
                                           extracting packet fields in custom
                                           applications. */
    bcmSwitchCustomInnerUdpDstPort1 = 1194, /* Inner UDP Dest Port 1 used for
                                           extracting packet fields in custom
                                           applications. */
    bcmSwitchCustomInnerUdpDstPort2 = 1195, /* Inner UDP Dest Port 2 used for
                                           extracting packet fields in custom
                                           applications. */
    bcmSwitchReservedEgressNextHop = 1196, /* Set reserved egress next hop, which
                                           is used to generate L2 header for
                                           IPMC packet that does not have L2
                                           header after tunnel termination. */
    bcmSwitchConcatAppHeader = 1197,    /* Concat app header to master key in
                                           ELK. */
    bcmSwitchVxlanGpeUdpDestPortSet = 1198, /* Set UDP Destination port for
                                           VXLAN-GPE. */
    bcmSwitchSystemPortOverride = 1199, /* When set, port group id used for 
                                           egress VLAN translation is replaced 
                                           with destination global logical port
                                           (mod, port). */
    bcmSwitchSRV6EgressPSPEnable = 1200, /* When set, device supports SRv6 Egress
                                           PSP mode. */
    bcmSwitchSRV6ReducedModeEnable = 1201, /* When set, device supports SRv6
                                           Reduced encapsulation mode. */
    bcmSwitchHashL4Field = 1202,        /* Hash selections for L4 headers. */
    bcmSwitchL3UcLargeNATEnable = 1203, /* Requires BROADCOM_PREMIUM license */
    bcmSwitchL3UcLargeDNAT = 1204,      /* Requires BROADCOM_PREMIUM license */
    bcmSwitchL3UcLargeSNAT = 1205,      /* Requires BROADCOM_PREMIUM license */
    bcmSwitchL3UcLargeDNATSNAT = 1206,  /* Requires BROADCOM_PREMIUM license */
    bcmSwitchL3UcLargeDNAPT = 1207,     /* Requires BROADCOM_PREMIUM license */
    bcmSwitchL3UcLargeSNAPT = 1208,     /* Requires BROADCOM_PREMIUM license */
    bcmSwitchL3UcLargeDNAPTSNAPT = 1209, /* Requires BROADCOM_PREMIUM license */
    bcmSwitchL3McLargeNATEnable = 1210, /* Requires BROADCOM_PREMIUM license */
    bcmSwitchL3McLargeNATRewriteType0 = 1211, /* Requires BROADCOM_PREMIUM license */
    bcmSwitchL3McLargeNATRewriteType1 = 1212, /* Requires BROADCOM_PREMIUM license */
    bcmSwitchL3McLargeNATRewriteType2 = 1213, /* Requires BROADCOM_PREMIUM license */
    bcmSwitchL2ChangeFieldsEnable = 1214, /* Requires BROADCOM_PREMIUM license */
    bcmSwitchModuleVerifyEnable = 1215, /* Enable/disable input verification for
                                           a particular module from the list
                                           bcm_module_verify_e. */
    bcmSwitchTraverseCommitDone = 1216, /* Return 0 if the flush machine is
                                           running. Return 1 if the flush
                                           machine is idle. */
    bcmSwitchHashIP4InnerField = 1217,  /* Hash selections for inner IPv4
                                           headers. */
    bcmSwitchHashIP4OuterField = 1218,  /* Hash selections for outer IPv4
                                           headers. */
    bcmSwitchHashIP6InnerField = 1219,  /* Hash selections for inner IPv6
                                           headers. */
    bcmSwitchHashIP6OuterField = 1220,  /* Hash selections for outer IPv6
                                           headers. */
    bcmSwitchHashL2InnerField = 1221,   /* Hash selections for inner L2 headers. */
    bcmSwitchHashL2OuterField = 1222,   /* Hash selections for outer L2 headers. */
    bcmSwitchMirrorPsampIpfixVersion = 1223, /* Set packet sampling (PSAMP) IP flow
                                           information export (IPFIX) version
                                           number. */
    bcmSwitchLlcHeaderControlMode = 1224, /* When set vlaue=1 LLC header control
                                           bits located in 1:0 selected, value=0
                                           bits 7:6 selected. */
    bcmSwitchPcieHotSwapDisable = 1225, /* When set vlaue=1 PCIE Hot swap
                                           manager is disabled. Applicable for
                                           CMICX based devices only */
    bcmSwitchTrunkDynamicMonitorIngressRandomSeed = 1226, /* Set RNG seed for DLB trunk flow
                                           monitoring. */
    bcmSwitchL2TunnelTpid = 1227,       /* TPID used for outer L2 header VLAN
                                           tag insertion during L2 tunnel
                                           encapsulation. */
    bcmSwitchEcnNonTcpResponsive = 1228, /* Set default responsive indication
                                           value for NON-TCP packets, including
                                           NON-IP packets. */
    bcmSwitchHigig3Ethertype = 1229,    /* Set High 10 bits of HiGig3 EtherType. */
    bcmSwitchIPv4DoNotVerifyIncomingChecksum = 1230, /* Requires BROADCOM_PREMIUM license */
    bcmSwitchHashUseFlowSelTrunkFailover = 1231, /* Enable/Disable flow based hashing for
                                           Trunk failover. */
    bcmSwitchHashUseFlowSelHigigTrunk = 1232, /* Enable/Disable flow based hashing for
                                           HiGig Trunk. */
    bcmSwitchHashUseFlowSelTrunkDynamic = 1233, /* Enable/Disable flow based hashing for
                                           Trunk dynamic load balance. */
    bcmSwitchHashUseHigig3EntropyTrunkNonUnicast = 1234, /* Enable/Disable HiGig3 Entropy based
                                           hashing for non-unicast Trunk. */
    bcmSwitchTrunkUnicastHashOffset = 1235, /* Enhanced hash offset selections for
                                           trunk unicast packet. */
    bcmSwitchTrunkNonUnicastHashOffset = 1236, /* Enhanced hash offset selections for
                                           trunk non-unicast packet. */
    bcmSwitchHigigTrunkHashOffset = 1237, /* Enhanced hash offset selections for
                                           higig trunk. */
    bcmSwitchVpTrunkHashOffset = 1238,  /* Enhanced hash offset selections for
                                           higig trunk. */
    bcmSwitchMacroFlowTrunkUnicastHashConcatEnable = 1239, /* Enable hash concatenation for HiGig
                                           Trunk unicast macro flow. */
    bcmSwitchMacroFlowTrunkNonUnicastHashConcatEnable = 1240, /* Enable hash concatenation for HiGig
                                           Trunk non-unicast macro flow. */
    bcmSwitchMacroFlowTrunkFailoverHashConcatEnable = 1241, /* Enable hash concatenation for Trunk
                                           failover macro flow. */
    bcmSwitchMacroFlowHigigTrunkFailoverHashConcatEnable = 1242, /* Enable hash concatenation for HiGig
                                           Trunk failover macro flow. */
    bcmSwitchMacroFlowTrunkDynamicHashConcatEnable = 1243, /* Enable hash concatenation for trunk
                                           dynamic load balance macro flow. */
    bcmSwitchMacroFlowVpTrunkHashConcatEnable = 1244, /* Enable hash concatenation for virtual
                                           port trunk group. */
    bcmSwitchMacroFlowTrunkUnicastHashSeed = 1245, /* Set Hash Seed for Trunk unicast macro
                                           flow. */
    bcmSwitchMacroFlowTrunkNonUnicastHashSeed = 1246, /* Set Hash Seed for Trunk non-unicast
                                           macro flow. */
    bcmSwitchMacroFlowTrunkFailoverHashSeed = 1247, /* Set Hash Seed for Trunk failover
                                           macro flow. */
    bcmSwitchMacroFlowHigigTrunkFailoverHashSeed = 1248, /* Set Hash Seed for HiGig Trunk
                                           failover macro flow. */
    bcmSwitchMacroFlowTrunkDynamicHashSeed = 1249, /* Set Hash Seed for Trunk dynamic load
                                           balance macro flow. */
    bcmSwitchMacroFlowVpTrunkHashSeed = 1250, /* Set Hash Seed for virtual port Trunk
                                           group macro flow. */
    bcmSwitchMacroFlowTrunkUnicastHashMinOffset = 1251, /* Minimum offset value to be used in
                                           trunk unicast offset table. */
    bcmSwitchMacroFlowTrunkUnicastHashMaxOffset = 1252, /* Maximum offset value to be used in
                                           trunk unicast offset table. */
    bcmSwitchMacroFlowTrunkUnicastHashStrideOffset = 1253, /* Offset increment to be used for
                                           populating trunk unicast offset
                                           table. */
    bcmSwitchMacroFlowTrunkNonUnicastHashMinOffset = 1254, /* Minimum offset value to be used in
                                           trunk non-unicast offset table. */
    bcmSwitchMacroFlowTrunkNonUnicastHashMaxOffset = 1255, /* Maximum offset value to be used in
                                           trunk non-unicast offset table. */
    bcmSwitchMacroFlowTrunkNonUnicastHashStrideOffset = 1256, /* Offset increment to be used for
                                           populating trunk non-unicast offset
                                           table. */
    bcmSwitchMacroFlowTrunkFailoverHashMinOffset = 1257, /* Minimum offset value to be used in
                                           trunk failover offset table. */
    bcmSwitchMacroFlowTrunkFailoverHashMaxOffset = 1258, /* Maximum offset value to be used in
                                           trunk failover offset table. */
    bcmSwitchMacroFlowTrunkFailoverHashStrideOffset = 1259, /* Offset increment to be used for
                                           populating trunk failover offset
                                           table. */
    bcmSwitchMacroFlowHigigTrunkFailoverHashMinOffset = 1260, /* Minimum offset value to be used in
                                           higig trunk failover offset table. */
    bcmSwitchMacroFlowHigigTrunkFailoverHashMaxOffset = 1261, /* Maximum offset value to be used in
                                           higig trunk failover offset table. */
    bcmSwitchMacroFlowHigigTrunkFailoverHashStrideOffset = 1262, /* Offset increment to be used for
                                           populating higig trunk failover
                                           offset table. */
    bcmSwitchMacroFlowTrunkDynamicHashMinOffset = 1263, /* Minimum offset value to be used in
                                           trunk dynamic load balance offset
                                           table. */
    bcmSwitchMacroFlowTrunkDynamicHashMaxOffset = 1264, /* Maximum offset value to be used in
                                           trunk dynamic load balance offset
                                           table. */
    bcmSwitchMacroFlowTrunkDynamicHashStrideOffset = 1265, /* Offset increment to be used for trunk
                                           populating dynamic load balance
                                           offset table. */
    bcmSwitchMacroFlowVpTrunkHashMinOffset = 1266, /* Minimum offset value to be used in
                                           virtual port trunk group offset
                                           table. */
    bcmSwitchMacroFlowVpTrunkHashMaxOffset = 1267, /* Maximum offset value to be used in
                                           virtual port trunk group offset
                                           table. */
    bcmSwitchMacroFlowVpTrunkHashStrideOffset = 1268, /* Offset increment to be used for
                                           populating virtual port trunk group
                                           offset table. */
    bcmSwitchTrunkRandomSeed = 1269,    /* Seed for RNG used in Trunk hashing. */
    bcmSwitchHigigTrunkRandomSeed = 1270, /* Seed for RNG used in HiGig Trunk
                                           hashing. */
    bcmSwitchVPTrunkRandomSeed = 1271,  /* Seed for RNG used in virtal port
                                           Trunk hashing. */
    bcmSwitchModuleErrorRecoveryEnable = 1272, /* Flag for switch module error recovery
                                           enable. */
    bcmSwitchHashMultiMoveDepthVlanTranslate3 = 1273, /* Maximum moves in multi hash mode for
                                           vlan vranslate 3 tables. */
    bcmSwitchHashMultiMoveDepthL2Tunnel = 1274, /* Maximum moves in multi hash mode for
                                           L2 tunnel termination tables. */
    bcmSwitchHashMultiMoveDepthL2TunnelVnid = 1275, /* Maximum moves in multi hash mode for
                                           L2 tunnel VNID tables. */
    bcmSwitchHashMultiMoveDepthFlowTracker = 1276, /* Maximum moves in multi hash mode for
                                           flow tracker tables. */
    bcmSwitchHashMultiMoveDepthIpmcGroup = 1277, /* Maximum moves in multi hash mode for
                                           L3 IPv4/IPv6 multicast (*,G) tables. */
    bcmSwitchHashMultiMoveDepthIpmcSourceGroup = 1278, /* Maximum moves in multi hash mode for
                                           L3 IPv4/IPv6 multicast (S,G) tables. */
    bcmSwitchHashMultiMoveDepthL2mc = 1279, /* Maximum moves in multi hash mode for
                                           L2 IPv4/IPv6 multicast tables. */
    bcmSwitchMactAgeRefreshMode = 1280, /* Select the hit bit mode */
    bcmSwitchDosAttackMaxIpPayloadEnable = 1281, /* Enable outer IP payload maximum size
                                           check. 1: enable, 0(default):
                                           disable. */
    bcmSwitchDosAttackMaxIpPayloadSize = 1282, /* Maximum outer IP payload size value
                                           for outer IP payload maximum size
                                           check. */
    bcmSwitchDosAttackMinIpPayloadEnable = 1283, /* Enable outer IP payload minimum size
                                           check. 1: enable, 0(default):
                                           disable. */
    bcmSwitchDosAttackMinIpPayloadSize = 1284, /* Minimum outer IP payload size value
                                           for outer IP payload minimum size
                                           check. */
    bcmSwitchDosAttackMaxInnerIpPayloadEnable = 1285, /* Enable inner IP payload maximum size
                                           check. 1: enable, 0(default):
                                           disable. */
    bcmSwitchDosAttackMaxInnerIpPayloadSize = 1286, /* Maximum inner IP payload size value
                                           for inner IP payload maximum size
                                           check. */
    bcmSwitchDosAttackMinInnerIpPayloadEnable = 1287, /* Enable inner IP payload minimum size
                                           check. 1: enable, 0(default):
                                           disable. */
    bcmSwitchDosAttackMinInnerIpPayloadSize = 1288, /* Minimum inner IP payload size value
                                           for inner IP payload minimum size
                                           check. */
    bcmSwitchEccSingleBitBusBufferErrorEvent = 1289, /* Enable/Disable 1bit ECC error
                                           reporting for internal bus and buffer
                                           which are protected by ECC logic. */
    bcmSwitchHashLayersDisable = 1290,  /* disable selected load balancing hash
                                           layers using the
                                           BCM_HASH_LAYER_XXX_DISABLE flags */
    bcmSwitchMplsAlternateMarkingSpecialLabel = 1291, /* Special Label Indicator value for
                                           Alternate Marking */
    bcmSwitchSvtagSciProfile = 1292,    /* SVTag Secutre Context Index Profile */
    bcmSwitchSvtagErrCodeProfile = 1293, /* SVTag Error Code Profile */
    bcmSwitchLatencyEcnMarkMode = 1294, /* Latency-based ECN mark mode, choose
                                           mode from
                                           bcm_switch_latency_ecn_mark_mode_t. */
    bcmSwitchGlobalTodMode = 1295,      /* Define the global TOD mode. See
                                           bcm_switch_tod_mode_t for details. */
    bcmSwitchTimesyncLiveWireToD = 1296, /* Enable live wire simultaneous NTP/PTP
                                           Time of Day in pipeline. */
    bcmSwitchL3TunnelCollapseDisable = 1297, /* Disables UDP Tunnel Collapsing for
                                           the specified protocol */
    bcmSwitchControlOamBfdFailoverStateIgnore = 1298, /* Enable/Disable ignoring protection
                                           state for OAMP inject packets. */
    bcmSwitchEntropyHashOffset = 1299,  /* Enhanced hash select for Entropy
                                           hashing. */
    bcmSwitchMacroFlowEntropyHashConcatEnable = 1300, /* Enable hash concatenation for Entropy
                                           hashing flow. */
    bcmSwitchMacroFlowEntropyHashSeed = 1301, /* Set Hash Seed for Entropy hashing
                                           flow. */
    bcmSwitchSubportLowPriPfcSel = 1302, /* Select the Low priority PFC class for
                                           the Subports. */
    bcmSwitchSubportHighPriPfcSel = 1303, /* Select the High priority PFC class
                                           for the Subports. */
    bcmSwitchExactMatchInit = 1304,     /* Initialize Exact Match databases */
    bcmSwitchRangeTypeSelect = 1305,    /* Select Range Type for L4 OPS Extended
                                           (Indexed) */
    bcmSwitchRangeOperator = 1306,      /* Select Operator between range types
                                           for L4 OPS Extended */
    bcmSwitchRangeResultMap = 1307,     /* Select result mapping for L4 OPS
                                           Extended's Encoders (Indexed) */
    bcmSwitchIFA2Enable = 1308,         /* Requires BROADCOM_PREMIUM license */
    bcmSwitchIFA2HeaderType = 1309,     /* Requires BROADCOM_PREMIUM license */
    bcmSwitchIFA2MetaDataFirstLookupType = 1310, /* Requires BROADCOM_PREMIUM license */
    bcmSwitchIFA2MetaDataSecondLookupType = 1311, /* Requires BROADCOM_PREMIUM license */
    bcmSwitchMacroFlowEcmpDynamicHashConcatEnable = 1312, /* Enable hash concatenation for ECMP
                                           dynamic load balance macro flow. */
    bcmSwitchMacroFlowEcmpDynamicHashSeed = 1313, /* Set Hash Seed for ECMP dynamic load
                                           balance macro flow. */
    bcmSwitchMacroFlowEcmpDynamicHashMinOffset = 1314, /* Minimum offset value to be used in
                                           ECMP dynamic load balance offset
                                           table. */
    bcmSwitchMacroFlowEcmpDynamicHashMaxOffset = 1315, /* Maximum offset value to be used in
                                           ECMP dynamic load balance offset
                                           table. */
    bcmSwitchMacroFlowEcmpDynamicHashStrideOffset = 1316, /* Offset increment to be used for ECMP
                                           populating dynamic load balance
                                           offset table. */
    bcmSwitchTrunkUnicastHashBitCountSelect = 1317, /* To specify the number of bits
                                           required in the final hash result for
                                           trunk unicast hash before the modulo
                                           function */
    bcmSwitchTrunkNonunicastHashBitCountSelect = 1318, /* To specify the number of bits
                                           required in the final hash result for
                                           trunk non-unicast hash before the
                                           modulo function */
    bcmSwitchTrunkFailoverHashBitCountSelect = 1319, /* To specify the number of bits
                                           required in the final hash result for
                                           trunk failover hash before the modulo
                                           function */
    bcmSwitchHigigTrunkHashBitCountSelect = 1320, /* To specify the number of bits
                                           required in the final hash result for
                                           higig trunk hash before the modulo
                                           function */
    bcmSwitchHigigTrunkFailoverHashBitCountSelect = 1321, /* To specify the number of bits
                                           required in the final hash result for
                                           higig trunk failover hash before the
                                           modulo function */
    bcmSwitchTrunkDynamicHashBitCountSelect = 1322, /* To specify the number of bits
                                           required in the final hash result for
                                           DLB trunk hash before the modulo
                                           function */
    bcmSwitchVpTrunkHashBitCountSelect = 1323, /* To specify the number of bits
                                           required in the final hash result for
                                           VPLAG hash before the modulo function */
    bcmSwitchEntropyHashBitCountSelect = 1324, /* To specify the number of bits
                                           required in the final hash result for
                                           entropy hash before the modulo
                                           function */
    bcmSwitchEcmpOverlayHashBitCountSelect = 1325, /* To specify the number of bits
                                           required in the final hash result for
                                           ECMP overlay hash before the modulo
                                           function */
    bcmSwitchEcmpDynamicHashBitCountSelect = 1326, /* To specify the number of bits
                                           required in the final hash result for
                                           DLB ECMP hash before the modulo
                                           function */
    bcmSwitchGpeNextProtoGbp = 1327,    /* GPE header next protocol value to
                                           parse/insert GBP header. */
    bcmSwitchIoamMaxPayloadLength = 1328, /* Maximum Length of IOAM payload. */
    bcmSwitchIoamNameSpaceId = 1329,    /* Value of NAMESPACE_ID field in IOAM
                                           payload to match. */
    bcmSwitchGpeNextProtoIoam = 1330,   /* GPE header next protocol value to
                                           parse IOAM header. */
    bcmSwitchGpeNextProtoIoamE2e = 1331, /* GPE header next protocol value to
                                           parse/insert IOAM E2E header. */
    bcmSwitchPFCStopInterfaceBitmap = 1332, /* Stop the whole interface when
                                           received configured PFC priorities. */
    bcmSwitchIFAMetaDataAdaptTable = 1333, /* Requires BROADCOM_PREMIUM license */
    bcmSwitchL3UcSameInterfaceDrop = 1334, /* Enable dropping of L3 unicast packets
                                           to the same L3 incoming interface. */
    bcmSwitchL2ApplCallbackSuppress = 1335, /* BCM56980 Enable/Disable suppression
                                           of application callbacks for L2
                                           entries added/deleted through
                                           bcm_l2_addr_add/bcm_l2_addr_delete
                                           APIs. This feature is disabled by
                                           default */
    bcmSwitchOtpAvsValue = 1336,        /* AVS value from OTP. */
    bcmSwitchSampleVersion = 1337,      /* Set sFlow version number. */
    bcmSwitchHashUseFlowSelEcmpWeighted = 1338, /* Enable/Disable flow based hashing for
                                           weighted ECMP. */
    bcmSwitchEcmpWeightedHashOffset = 1339, /* Weighted ECMP Hash key is barrel
                                           shifted by this value. */
    bcmSwitchMacroFlowEcmpWeightedHashMinOffset = 1340, /* Minumum offset value to be used in
                                           RTAG7 macroflow for weighted ECMP. */
    bcmSwitchMacroFlowEcmpWeightedHashMaxOffset = 1341, /* Maximum offset value to be used in
                                           RTAG7 macroflow for weighted ECMP. */
    bcmSwitchMacroFlowEcmpWeightedHashStrideOffset = 1342, /* Offset increment value to be used in
                                           RTAG7 macroflow for weighted ECMP. */
    bcmSwitchMacroFlowEcmpWeightedHashConcatEnable = 1343, /* Enable hash concatenation for
                                           weighted ECMP macro flow. */
    bcmSwitchMplsPwControlWordUpperClear = 1344, /* Clear the upper 2 bytes of the
                                           control word. */
    bcmSwitchMplsPwControlWordLowerClear = 1345, /* Clear the lower 2 bytes of the
                                           control word. */
    bcmSwitchMplsGal = 1346,            /* Specify the GAL value for GAL label
                                           match. */
    bcmSwitchMplsEli = 1347,            /* Specify the ELI value for ELI label
                                           match. */
    bcmSwitchMplsPwTermSeqNumberRange = 1348, /* Specify the PW decapsulation sequence
                                           number range. */
    bcmSwitchEgressVlanTranslate2Enable = 1349, /* Enable egress vlan translation table
                                           2 for fixed flows. */
    bcmSwitchPointToPointAccessVlanCheckEnable = 1350, /* Enable access vlan check for point to
                                           point service. */
    bcmSwitchNshEgressAdaptationKeySelect = 1351, /* Select egress adaptation key type for
                                           NSH encapsulation. Valid values are
                                           BCM_SWITCH_NSH_EGR_ADAPT_KEY_xxx */
    bcmSwitchNshMetaDataSourceInterfaceSelect = 1352, /* Select the source of NSH metadata
                                           source interface. Valid values are
                                           BCM_SWITCH_NSH_METADATA_SRC_INTF_xxx */
    bcmSwitchNshMetaDataTenantIdSelect = 1353, /* Select the source of NSH metadata
                                           tenant ID. Valid values are
                                           BCM_SWITCH_NSH_METADATA_TENANT_ID_xxx */
    bcmSwitchNshTenantIdMappingMissDropEnable = 1354, /* Enable NSH tenant ID mapping miss
                                           drop */
    bcmSwitchNshEthertype = 1355,       /* Set the NSH protocol etherType */
    bcmSwitchNshEthertypeEnable = 1356, /* Enable to match the NSH protocol
                                           etherType */
    bcmSwitchL3IngTagActionsEnable = 1357, /* Requires BROADCOM_PREMIUM license */
    bcmSwitchECMPRandomSeed = 1358,     /* Seed for RNG used in random load
                                           balancing for single level ECMP. */
    bcmSwitchHashVersatileInitValue0 = 1359, /* Initial value 0 for both versatile
                                           hash block A and B. */
    bcmSwitchHashVersatileInitValue1 = 1360, /* Initial value 1 for both versatile
                                           hash block A and B. */
    bcmSwitchHash0VersatileInitValue0 = 1361, /* Initial value 0 for versatile hash
                                           block A. */
    bcmSwitchHash0VersatileInitValue1 = 1362, /* Initial value 1 for versatile hash
                                           block A. */
    bcmSwitchHash1VersatileInitValue0 = 1363, /* Initial value 0 for versatile hash
                                           block B. */
    bcmSwitchHash1VersatileInitValue1 = 1364, /* Initial value 1 for versatile hash
                                           block B. */
    bcmSwitchIoamE2eNextProtoGbp = 1365, /* IAOM E2E header (GPE -> IOAM_E2E ->
                                           GBP) next protocol value to
                                           parse/insert GBP header. */
    bcmSwitchMirrorUnicastCosqEnable = 1366, /* Enable to use multicast COS from
                                           mirroring for unicast mirror. */
    bcmSwitchFlowCountHashShift = 1367, /* Specify the right shift amount
                                           applied to the hash value generated
                                           by RTAG7 hash for prefix length
                                           calculation. */
    bcmSwitchFlowCountHashMask = 1368,  /* Specify the mask ORed to shifted hash
                                           value for prefix length calculation. */
    bcmSwitchFlowCountFlexCtrIngAction = 1369, /* Specify the ingress enhanced flex
                                           counter action by using
                                           bcmFlexctrObjectStaticIngFlowCountPrefixLength. */
    bcmSwitchSLLBDestinationNotFound = 1370, /* Default destination applied for SLLB
                                           routing lookups miss. */
    bcmSwitchVIPEcmpTableSize = 1371,   /* SLLB ECMP Consistent Hash table size. */
    bcmSwitchVIPL3EgressIDMin = 1372,   /* Minumum VIP ECMP FEC Id in the
                                           reserved FEC range. */
    bcmSwitchVIPL3EgressIDMax = 1373,   /* Maximum VIP ECMP FEC Id in the
                                           reserved range. */
    bcmSwitchPCCAgeTimer = 1374,        /* LEM PCC State Table age timer in
                                           seconds. */
    bcmSwitchPCCLearnLimit = 1375,      /* LEM PCC State Table learn limit. */
    bcmSwitchFlexeOamAlmCollectionTimerStep = 1376, /* The Timer step for collecting FlexE
                                           OAM alarms. */
    bcmSwitchFlexeOamAlmCollectionStepCount = 1377, /* The number of steps for FlexE OAM
                                           alarm collection period. The final
                                           period should be timer_step *
                                           step_count * 1024 / 833Mhz */
    bcmSwitchIfaVersion = 1378,         /* IFA version number. */
    bcmSwitchIfaProtocol = 1379,        /* Protocol value to be compared against
                                           the packet IP protocol or next header
                                           field in order to identify and parse
                                           IFA packets. */
    bcmSwitchIoamGreProtocolParse = 1380, /* Enable to parse IOAM packet with GRE
                                           encapsulation. */
    bcmSwitchIoamGreProtocol = 1381,    /* Set the protocol value to be compared
                                           against the IP protocol field in the
                                           GRE header in order to identify and
                                           parse IOAM packets. */
    bcmSwitchIoamOptionIncrementalParse = 1382, /* Enable to parse IOAM packet with IPV6
                                           encapsulation. */
    bcmSwitchIoamOptionIncremental = 1383, /* Set the option type value to be
                                           compared against the option type in
                                           the IPv6 Hop-by-Hop or destination
                                           extension header in order to identify
                                           and parse IOAM packets. */
    bcmSwitchIoamType = 1384,           /* Set IOAM type value to be compared
                                           against the IOAM type in the GRE
                                           header or the IPv6 extension header
                                           in order to identify and parse IOAM
                                           packets. */
    bcmSwitchIntVectorMatchMissAction = 1385, /* Action for inband telemetry packets
                                           with no vector match. 0: Drop, 1:
                                           Forward as non-INT packet. */
    bcmSwitch1588ClockAddressOui = 1386, /* OUI portion (first 3 octets) of the
                                           IEEE 1588 clock address representing
                                           this system. */
    bcmSwitch1588ClockAddressNonOui = 1387, /* Non-OUI portion (last 3 octets) of
                                           the IEEE 1588 clock address
                                           representing this system. */
    bcmSwitchHashUseFlowSelL2Ecmp = 1388, /* Enable/Disable flow based hashing for
                                           L2 ECMP. */
    bcmSwitchHashUseFlowSelL2EcmpOverlay = 1389, /* Enable/Disable flow based hashing for
                                           overlay L2 ECMP. */
    bcmSwitchHashUseFlowSelEcmpResilientHash = 1390, /* Enable/Disable flow based hashing for
                                           ECMP resilient hashing. */
    bcmSwitchHashUseFlowSelEcmpOverlayResilientHash = 1391, /* Enable/Disable flow based hashing for
                                           overlay ECMP resilient hashing. */
    bcmSwitchHashUseFlowSelEcmpWeightedOverlay = 1392, /* Enable/Disable flow based hashing for
                                           overlay weighted ECMP. */
    bcmSwitchEcmpOverlayResilientHashOffset = 1393, /* Enhanced hash select for overlay ECMP
                                           resilient hashing. */
    bcmSwitchEcmpOverlayMplsHashOffset = 1394, /* Enhanced hash select for overlay MPLS
                                           ECMP hashing. */
    bcmSwitchEcmpOverlayHashOffset = 1395, /* Enhanced hash select for overlay ECMP
                                           hashing. */
    bcmSwitchEcmpOverlayWeightedHashOffset = 1396, /* Enhanced hash select for overlay
                                           weighted ECMP hashing. */
    bcmSwitchL2EcmpHashOffset = 1397,   /* Enhanced hash select for L2 ECMP
                                           hashing. */
    bcmSwitchL2EcmpOverlayHashOffset = 1398, /* Enhanced hash select for overlay L2
                                           ECMP hashing. */
    bcmSwitchMacroFlowEcmpResilientHashConcatEnable = 1399, /* Enable hash concatenation for ECMP
                                           resilient hashing macroflow. */
    bcmSwitchMacroFlowEcmpResilientHashMinOffset = 1400, /* Minimum offset value to be used in
                                           RTAG7 macroflow for ECMP resilient
                                           hashing. */
    bcmSwitchMacroFlowEcmpResilientHashMaxOffset = 1401, /* Maximum Offset value to be used in
                                           RTAG7 macroflow for ECMP resilient
                                           hashing. */
    bcmSwitchMacroFlowEcmpResilientHashStrideOffset = 1402, /* Offset increment value to be used in
                                           RTAG7 macroflow for ECMP resilient
                                           hashing. */
    bcmSwitchMacroFlowEcmpOverlayResilientHashConcatEnable = 1403, /* Enable hash concatenation for overlay
                                           ECMP resilient hashing macroflow. */
    bcmSwitchMacroFlowEcmpOverlayResilientHashMinOffset = 1404, /* Minimum offset value to be used in
                                           RTAG7 macroflow for overlay ECMP
                                           resilient hashing. */
    bcmSwitchMacroFlowEcmpOverlayResilientHashMaxOffset = 1405, /* Maximum offset value to be used in
                                           RTAG7 macroflow for overlay ECMP
                                           resilient hashing. */
    bcmSwitchMacroFlowEcmpOverlayResilientHashStrideOffset = 1406, /* Offset increment value to be used in
                                           RTAG7 macroflow for overlay ECMP
                                           resilient hashing. */
    bcmSwitchMacroFlowEcmpMplsHashConcatEnable = 1407, /* Enable hash concatenation for MPLS
                                           ECMP macroflow. */
    bcmSwitchMacroFlowEcmpMplsHashMinOffset = 1408, /* Minimum offset value to be used in
                                           RTAG7 macroflow for MPLS ECMP. */
    bcmSwitchMacroFlowEcmpMplsHashMaxOffset = 1409, /* Maximum Offset value to be used in
                                           RTAG7 macroflow for MPLS ecmp. */
    bcmSwitchMacroFlowEcmpMplsHashStrideOffset = 1410, /* Offset increment value to be used in
                                           RTAG7 macroflow for MPLS ecmp. */
    bcmSwitchMacroFlowEcmpOverlayMplsHashConcatEnable = 1411, /* Enable hash concatenation for overlay
                                           MPLS ECMP macroflow. */
    bcmSwitchMacroFlowEcmpOverlayMplsHashMinOffset = 1412, /* Minimum offset value to be used in
                                           RTAG7 macroflow for overlay MPLS
                                           ECMP. */
    bcmSwitchMacroFlowEcmpOverlayMplsHashMaxOffset = 1413, /* Maximum Offset value to be used in
                                           RTAG7 macroflow for overlay MPLS
                                           ECMP. */
    bcmSwitchMacroFlowEcmpOverlayMplsHashStrideOffset = 1414, /* Offset increment value to be used in
                                           RTAG7 macroflow for overlay MPLS
                                           ECMP. */
    bcmSwitchMacroFlowL2EcmpHashConcatEnable = 1415, /* Enable hash concatenation for L2 ECMP
                                           macroflow. */
    bcmSwitchMacroFlowL2EcmpHashMinOffset = 1416, /* Minimum offset value to be used in
                                           RTAG7 macroflow for L2 ECMP. */
    bcmSwitchMacroFlowL2EcmpHashMaxOffset = 1417, /* Maximum offset value to be used in
                                           RTAG7 macroflow for L2 ECMP. */
    bcmSwitchMacroFlowL2EcmpHashStrideOffset = 1418, /* Offset increment value to be used in
                                           RTAG7 macroflow for L2 ECMP. */
    bcmSwitchMacroFlowL2EcmpOverlayHashConcatEnable = 1419, /* Enable hash concatenation for overlay
                                           L2 ECMP macroflow. */
    bcmSwitchMacroFlowL2EcmpOverlayHashMinOffset = 1420, /* Minimum offset value to be used in
                                           RTAG7 macroflow for overlay L2 ECMP. */
    bcmSwitchMacroFlowL2EcmpOverlayHashMaxOffset = 1421, /* Maximum offset value to be used in
                                           RTAG7 macroflow for overlay L2 ECMP. */
    bcmSwitchMacroFlowL2EcmpOverlayHashStrideOffset = 1422, /* Offset increment value to be used in
                                           RTAG7 macroflow for overlay L2 ECMP. */
    bcmSwitchMacroFlowEcmpWeightedOverlayHashConcatEnable = 1423, /* Enable hash concatenation for overlay
                                           weighted ECMP macroflow. */
    bcmSwitchMacroFlowEcmpWeightedOverlayHashMinOffset = 1424, /* Minimum offset value to be used in
                                           RTAG7 macroflow for overlay weighted
                                           ECMP. */
    bcmSwitchMacroFlowEcmpWeightedOverlayHashMaxOffset = 1425, /* Maximum offset value to be used in
                                           RTAG7 macroflow for overlay weighted
                                           ECMP. */
    bcmSwitchMacroFlowEcmpWeightedOverlayHashStrideOffset = 1426, /* Offset increment value to be used in
                                           RTAG7 macroflow for overlay weighted
                                           ECMP. */
    bcmSwitchHashUseFlowSelMplsEcmpOverlay = 1427, /* Enable/Disable flow based hashing for
                                           MPLS Overlay ECMP. */
    bcmSwitchExemScanPeriod = 1428,     /* EXEM flush-machine scan period in
                                           uSec. */
    bcmSwitchVxlanHeaderFlags = 1429,   /* Set the flags value to fill into the
                                           flags field in the VXLAN header
                                           during VXLAN tunnel initiation. */
    bcmSwitchVxlanHeaderMatchFlags = 1430, /* Set the flags value to match against
                                           the flags field in the VXLAN header
                                           during VXLAN tunnel termination. */
    bcmSwitchVxlanHeaderFlagsMask = 1431, /* Set the mask to be applied to the
                                           flags field in the VXLAN header
                                           before matching with
                                           bcmSwitchVxlanHeaderMatchFlags. */
    bcmSwitchVxlanHeaderFlagsMismatchDrop = 1432, /* Drop VXLAN packets if the flags field
                                           in the VXLAN header is not matched
                                           with bcmSwitchVxlanHeaderMatchFlags
                                           after masked by
                                           bcmSwitchVxlanHeaderFlagsMask. */
    bcmSwitchVxlanHeaderReserved1 = 1433, /* Set the value to fill into the
                                           RESERVED_1 field in the VXLAN header
                                           during VXLAN tunnel initiation. */
    bcmSwitchVxlanHeaderReserved2 = 1434, /* Set the value to fill into the
                                           RESERVED_2 field in the VXLAN header
                                           during VXLAN tunnel initiation. */
    bcmSwitchFlexeSarCellMode = 1435,   /* FlexE SAR cell mode, see
                                           bcm_switch_flexe_sar_cell_mode_t */
    bcmSwitchMplsSpecialLabelAutoTerminateDisable = 1436, /* Disable the auto termination for MPLS
                                           special label */
    bcmSwitchExternalTableCapacity = 1437, /* Capacity for external tables */
    bcmSwitchExemFlushProfilePayloadOffset = 1438, /* EXEM flush profile payload offset */
    bcmSwitchNtpL4DestPort = 1439,      /* UDP destination port number to
                                           identify NTP packets. */
    bcmSwitchNtpResidenceTimeUpdate = 1440, /* Enable to update the timestamp field
                                           in NTP packet based on the internal
                                           residence time. */
    bcmSwitchV6L3LocalLinkSrcDisposition = 1441, /* Select drop behavior for packets with
                                           IPv6 link-local addresses */
    bcmSwitchMirrorAllowDuplicateMirroredPackets = 1442, /* Enable to allow duplicate mirror
                                           packets with the same encapsulation
                                           format sent to the same destination
                                           port. */
    bcmSwitchEgressStgCheckIngressEnable = 1443, /* Enable egress port's spanning tree
                                           check before enqueuing the packet to
                                           the MMU for L2 forwarding. */
    bcmSwitchVxlanIPv4TunnelVlanTranslateEnable = 1444, /* Requires BROADCOM_PREMIUM license */
    bcmSwitchVxlanIPv6TunnelVlanTranslateEnable = 1445, /* Requires BROADCOM_PREMIUM license */
    bcmSwitchEgressFlexCounterPresel = 1446, /* Pre-select to count mirror packets or
                                           copy to CPU packets in egress flex
                                           counter. */
    bcmSwitch__Count = 1447             /* Must be last.  Not a usable value. */
} bcm_switch_control_t;

#define BCM_SWITCH_CONTROL_STR \
    "HgHdrErrToCpu", \
    "ClassTagPacketsToCpu", \
    "IpmcTunnelToCpu", \
    "StationMoveOverLearnLimitToCpu", \
    "McastUnknownErrToCpu", \
    "LearnLimitPriority", \
    "ControlOpcodePriority", \
    "CpuProtoBpduPriority", \
    "CpuProtoArpPriority", \
    "CpuProtoIgmpPriority", \
    "CpuProtoDhcpPriority", \
    "CpuProtoIpmcReservedPriority", \
    "CpuProtoIpOptionsPriority", \
    "CpuProtoExceptionsPriority", \
    "CpuProtocolPrio", \
    "CpuUnknownPrio", \
    "CpuSamplePrio", \
    "CpuDefaultPrio", \
    "L2StaticMoveToCpu", \
    "UnknownIpmcToCpu", \
    "UnknownMcastToCpu", \
    "UnknownUcastToCpu", \
    "NonIpL3ErrToCpu", \
    "L3HeaderErrToCpu", \
    "UnknownVlanToCpu", \
    "UnknownL3SrcToCpu", \
    "UnknownL3DestToCpu", \
    "IpmcPortMissToCpu", \
    "IpmcErrorToCpu", \
    "VCLabelMissToCpu", \
    "SourceRouteToCpu", \
    "SampleIngressRandomSeed", \
    "SampleEgressRandomSeed", \
    "CpuFpCopyPrio", \
    "CpuIcmpRedirectPrio", \
    "CpuMtuFailPrio", \
    "CpuMirrorPrio", \
    "CpuLookupFpCopyPrio", \
    "L2NonStaticMoveToCpu", \
    "V6L3ErrToCpu", \
    "V6L3DstMissToCpu", \
    "V6L3LocalLinkDrop", \
    "V4L3ErrToCpu", \
    "V4L3DstMissToCpu", \
    "TunnelErrToCpu", \
    "MartianAddrToCpu", \
    "MartianAddr", \
    "L3UcTtlErrToCpu", \
    "L3SlowpathToCpu", \
    "IpmcTtlErrToCpu", \
    "DosAttackToCpu", \
    "DosAttackSipEqualDip", \
    "DosAttackMinTcpHdrSize", \
    "DosAttackV4FirstFrag", \
    "DosAttackTcpFlags", \
    "DosAttackL4Port", \
    "DosAttackTcpFrag", \
    "DosAttackIcmp", \
    "DosAttackIcmpV4", \
    "DosAttackIcmpV6", \
    "DosAttackIcmpPktOversize", \
    "DosAttackMACSAEqualMACDA", \
    "DosAttackIcmpV6PingSize", \
    "DosAttackIcmpFragments", \
    "DosAttackTcpOffset", \
    "DosAttackUdpPortsEqual", \
    "DosAttackTcpPortsEqual", \
    "DosAttackTcpFlagsSF", \
    "DosAttackTcpFlagsFUP", \
    "DosAttackTcpHdrPartial", \
    "DosAttackPingFlood", \
    "DosAttackSynFlood", \
    "DosAttackTcpSmurf", \
    "DosAttackTcpXMas", \
    "DosAttackL3Header", \
    "IcmpRedirectToCpu", \
    "MplsSequenceErrToCpu", \
    "MplsLabelMissToCpu", \
    "MplsTtlErrToCpu", \
    "MplsInvalidL3PayloadToCpu", \
    "MplsInvalidActionToCpu", \
    "SharedVlanMismatchToCpu", \
    "L3SrcUrpfErrToCpu", \
    "GreKeyToVlan", \
    "ArpReplyToCpu", \
    "ArpReplyDrop", \
    "ArpRequestToCpu", \
    "ArpRequestDrop", \
    "ArpReplyMyStationL2ToCPU", \
    "NdPktToCpu", \
    "NdPktDrop", \
    "McastFloodBlocking", \
    "IgmpPktToCpu", \
    "IgmpPktDrop", \
    "DhcpPktToCpu", \
    "DhcpPktDrop", \
    "MldPktToCpu", \
    "MldPktDrop", \
    "V4ResvdMcPktToCpu", \
    "V4ResvdMcPktDrop", \
    "V4ResvdMcPktFlood", \
    "V6ResvdMcPktToCpu", \
    "V6ResvdMcPktDrop", \
    "V6ResvdMcPktFlood", \
    "IgmpReportLeaveToCpu", \
    "IgmpReportLeaveDrop", \
    "IgmpReportLeaveFlood", \
    "IgmpQueryToCpu", \
    "IgmpQueryDrop", \
    "IgmpQueryFlood", \
    "IgmpUnknownToCpu", \
    "IgmpUnknownDrop", \
    "IgmpUnknownFlood", \
    "MldReportDoneToCpu", \
    "MldReportDoneDrop", \
    "MldReportDoneFlood", \
    "MldQueryToCpu", \
    "MldQueryDrop", \
    "MldQueryFlood", \
    "IpmcV4RouterDiscoveryToCpu", \
    "IpmcV4RouterDiscoveryDrop", \
    "IpmcV4RouterDiscoveryFlood", \
    "IpmcV6RouterDiscoveryToCpu", \
    "IpmcV6RouterDiscoveryDrop", \
    "IpmcV6RouterDiscoveryFlood", \
    "DirectedMirroring", \
    "PktAge", \
    "McastFloodDefault", \
    "ParityErrorToCpu", \
    "L3MtuFailToCpu", \
    "MeterAdjust", \
    "CounterAdjust", \
    "MeterAdjustInterframeGap", \
    "HashL2", \
    "HashL3", \
    "HashMultipath", \
    "HashControl", \
    "FieldMultipathHashSelect", \
    "FieldMultipathHashSeed", \
    "MirrorStackMode", \
    "MirrorSrcModCheck", \
    "MirrorUnmarked", \
    "DestPortHGTrunk", \
    "ColorSelect", \
    "ModuleLoopback", \
    "SrcModBlockMirrorCopy", \
    "SrcModBlockMirrorOnly", \
    "HashSeed0", \
    "HashSeed1", \
    "HashField0PreProcessEnable", \
    "HashField1PreProcessEnable", \
    "HashField0Config", \
    "HashField0Config1", \
    "HashField1Config", \
    "HashField1Config1", \
    "MacroFlowHashFieldConfig", \
    "HashSelectControl", \
    "HashIP4Field0", \
    "HashIP4Field1", \
    "HashIP4TcpUdpField0", \
    "HashIP4TcpUdpField1", \
    "HashIP4TcpUdpPortsEqualField0", \
    "HashIP4TcpUdpPortsEqualField1", \
    "HashIP6Field0", \
    "HashIP6Field1", \
    "HashIP6TcpUdpField0", \
    "HashIP6TcpUdpField1", \
    "HashIP6TcpUdpPortsEqualField0", \
    "HashIP6TcpUdpPortsEqualField1", \
    "HashL2Field0", \
    "HashL2Field1", \
    "HashMPLSField0", \
    "HashMPLSField1", \
    "HashFCOEField0", \
    "HashFCOEField1", \
    "HashL2TrillField0", \
    "HashL2TrillField1", \
    "HashL3TrillField0", \
    "HashL3TrillField1", \
    "HashTrillTunnelField0", \
    "HashTrillTunnelField1", \
    "HashHG2UnknownField0", \
    "HashHG2UnknownField1", \
    "HashField0OverlayCntagRpidEnable", \
    "HashField1OverlayCntagRpidEnable", \
    "HashHg2PktFieldsEnable", \
    "HashField0Ip6FlowLabel", \
    "HashField1Ip6FlowLabel", \
    "HashUseFlowSelTrunkUc", \
    "HashUseFlowSelEcmp", \
    "TrunkHashSet0UnicastOffset", \
    "TrunkHashSet1UnicastOffset", \
    "TrunkHashSet0NonUnicastOffset", \
    "TrunkHashSet1NonUnicastOffset", \
    "TrunkFailoverHashOffset", \
    "FabricTrunkHashSet0UnicastOffset", \
    "FabricTrunkHashSet1UnicastOffset", \
    "FabricTrunkHashSet0NonUnicastOffset", \
    "FabricTrunkHashSet1NonUnicastOffset", \
    "FabricTrunkFailoverHashOffset", \
    "FabricTrunkDynamicHashOffset", \
    "LoadBalanceHashSet0UnicastOffset", \
    "LoadBalanceHashSet1UnicastOffset", \
    "LoadBalanceHashSet0NonUnicastOffset", \
    "LoadBalanceHashSet1NonUnicastOffset", \
    "MacroFlowHashMinOffset", \
    "MacroFlowHashMaxOffset", \
    "MacroFlowHashStrideOffset", \
    "UniformUcastTrunkDistribution", \
    "UniformFabricTrunkDistribution", \
    "ECMPHashSet0Offset", \
    "ECMPHashSet1Offset", \
    "CpuToCpuEnable", \
    "IgmpUcastEnable", \
    "MldUcastEnable", \
    "IgmpReservedMcastEnable", \
    "MldReservedMcastEnable", \
    "MldDirectAttachedOnly", \
    "PortEgressBlockL2", \
    "PortEgressBlockL3", \
    "BpduInvalidVlanDrop", \
    "MirrorInvalidVlanDrop", \
    "MirrorPktChecksEnable", \
    "RemoteLearnTrust", \
    "SourceMacZeroDrop", \
    "IpmcGroupMtu", \
    "ModuleType", \
    "Deprecated227", \
    "Deprecated228", \
    "Deprecated229", \
    "L3EgressMode", \
    "L3DefipMultipathCountUpdate", \
    "L3IngressMode", \
    "L3HostAsRouteReturnValue", \
    "SourceModBlockUcast", \
    "SourceModBlockControlOpcode", \
    "EgressBlockUcast", \
    "AlternateStoreForward", \
    "WarmBoot", \
    "StableSelect", \
    "StableSize", \
    "StableUsed", \
    "StableConsistent", \
    "ControlSync", \
    "ControlAutoSync", \
    "CallbackAbortOnError", \
    "UnknownIpmcAsMcast", \
    "TunnelUnknownIpmcDrop", \
    "DhcpLearn", \
    "IpmcTtl1ToCpu", \
    "L3UcastTtl1ToCpu", \
    "DosAttackFlagZeroSeqZero", \
    "HgHdrExtLengthEnable", \
    "DosAttackSynFrag", \
    "Ip4McastL2DestCheck", \
    "Ip6McastL2DestCheck", \
    "L3InterfaceUrpfEnable", \
    "L3TunnelUrpfMode", \
    "L3TunnelUrpfDefaultRoute", \
    "L3UrpfFailToCpu", \
    "L3UrpfRouteEnable", \
    "L3UrpfRouteEnableExternal", \
    "L3UrpfMode", \
    "L3UrpfDefaultRoute", \
    "IngressRateLimitIncludeIFG", \
    "RateLimitLinear", \
    "HashL2Dual", \
    "HashL3Dual", \
    "HashDualMoveDepth", \
    "HashDualMoveDepthL2", \
    "HashDualMoveDepthL3", \
    "HashDualMoveDepthVlan", \
    "HashDualMoveDepthMpls", \
    "HashDualMoveDepthEgressVlan", \
    "HashDualMoveDepthWlanPort", \
    "HashDualMoveDepthWlanClient", \
    "HashMultiMoveDepth", \
    "HashMultiMoveDepthL2", \
    "HashMultiMoveDepthL3", \
    "HashMultiMoveDepthVlan", \
    "HashMultiMoveDepthMpls", \
    "HashMultiMoveDepthEgressVlan", \
    "HashL2DualLeastFull", \
    "HashL3DualLeastFull", \
    "HashMPLSDualLeastFull", \
    "SharedVlanEnable", \
    "SharedVlanL2McastEnable", \
    "IpmcReplicationSharing", \
    "IpmcReplicationAvailabilityThreshold", \
    "VrfMax", \
    "FailoverStackTrunk", \
    "FailoverEtherTrunk", \
    "ClassBasedMoveFailPktToCpu", \
    "ClassBasedMoveFailPktDrop", \
    "HgHdrMcastFlood", \
    "HgHdrMcastFloodOverride", \
    "HgHdrIpMcastFlood", \
    "HgHdrIpMcastFloodOverride", \
    "STPBlockedFieldBypass", \
    "L2PortBlocking", \
    "HashIpfixIngress", \
    "HashIpfixIngressDual", \
    "HashIpfixEgress", \
    "HashIpfixEgressDual", \
    "HashMPLS", \
    "HashMPLSDual", \
    "ForceForwardFabricTrunk", \
    "UseGport", \
    "HgHdrMcastVlanRange", \
    "HgHdrMcastL2Range", \
    "HgHdrMcastL3Range", \
    "McastL2Range", \
    "McastL3Range", \
    "HashWlanPort", \
    "HashWlanPortDual", \
    "HashWlanClient", \
    "HashWlanClientDual", \
    "WlanClientAuthorizeAll", \
    "WlanClientUnauthToCpu", \
    "WlanClientRoamedOutErrorToCpu", \
    "WlanClientSrcMacMissToCpu", \
    "WlanClientDstMacMissToCpu", \
    "OamHeaderErrorToCpu", \
    "OamUnknownVersionToCpu", \
    "OamUnknownVersionDrop", \
    "OamUnexpectedPktToCpu", \
    "OamCcmToCpu", \
    "OamXconCcmToCpu", \
    "OamXconOtherToCpu", \
    "L3SrcBindFailToCpu", \
    "TunnelIp4IdShared", \
    "HashRegexAction", \
    "HashRegexActionDual", \
    "Deprecated333", \
    "CpuCopyDestination", \
    "CpuProtoTimeSyncPrio", \
    "CpuProtoMmrpPrio", \
    "CpuProtoSrpPrio", \
    "TimeSyncPktToCpu", \
    "TimeSyncPktDrop", \
    "TimeSyncPktFlood", \
    "MmrpPktToCpu", \
    "MmrpPktDrop", \
    "MmrpPktFlood", \
    "SrpPktToCpu", \
    "SrpPktDrop", \
    "SrpPktFlood", \
    "SRPEthertype", \
    "MMRPEthertype", \
    "TimeSyncEthertype", \
    "FcoeEtherType", \
    "SRPDestMacOui", \
    "MMRPDestMacOui", \
    "TimeSyncDestMacOui", \
    "SRPDestMacNonOui", \
    "MMRPDestMacNonOui", \
    "TimeSyncDestMacNonOui", \
    "TimeSyncMessageTypeBitmap", \
    "TimeSyncClassAPktPrio", \
    "TimeSyncClassBPktPrio", \
    "TimeSyncClassAExeptionPktPrio", \
    "TimeSyncClassBExeptionPktPrio", \
    "L2McastAllRouterPortsAddEnable", \
    "BypassMode", \
    "IpmcSameVlanL3Route", \
    "Deprecated365", \
    "FieldCache", \
    "FieldCommit", \
    "IpmcCache", \
    "IpmcCommit", \
    "L2Cache", \
    "L2Commit", \
    "L2AgeDelete", \
    "L2AgeScan", \
    "L3HostCache", \
    "L3HostCommit", \
    "L3RouteCache", \
    "L3RouteCommit", \
    "L2InvalidCtlToCpu", \
    "InvalidGreToCpu", \
    "HashL2MPLSField0", \
    "HashL2MPLSField1", \
    "HashL3MPLSField0", \
    "HashL3MPLSField1", \
    "HashMPLSTunnelField0", \
    "HashMPLSTunnelField1", \
    "HashMIMTunnelField0", \
    "HashMIMTunnelField1", \
    "HashMIMField0", \
    "HashMIMField1", \
    "StgInvalidToCpu", \
    "VlanTranslateEgressMissToCpu", \
    "L3PktErrToCpu", \
    "MtuFailureToCpu", \
    "SrcKnockoutToCpu", \
    "WlanTunnelMismatchToCpu", \
    "WlanTunnelMismatchDrop", \
    "WlanPortMissToCpu", \
    "UnknownVlanToCpuCosq", \
    "StgInvalidToCpuCosq", \
    "VlanTranslateEgressMissToCpuCosq", \
    "TunnelErrToCpuCosq", \
    "L3HeaderErrToCpuCosq", \
    "L3PktErrToCpuCosq", \
    "IpmcTtlErrToCpuCosq", \
    "MtuFailureToCpuCosq", \
    "HgHdrErrToCpuCosq", \
    "SrcKnockoutToCpuCosq", \
    "WlanTunnelMismatchToCpuCosq", \
    "WlanPortMissToCpuCosq", \
    "IpfixRateViolationDataInsert", \
    "IpfixRateViolationPersistent", \
    "HashVlanTranslate", \
    "HashVlanTranslateDual", \
    "HashEgressVlanTranslate", \
    "HashEgressVlanTranslateDual", \
    "LayeredQoSResolution", \
    "CustomerQueuing", \
    "CosqStatThreshold", \
    "CosqStatInterval", \
    "EncapErrorToCpu", \
    "MplsPortIndependentLowerRange1", \
    "MplsPortIndependentUpperRange1", \
    "MplsPortIndependentLowerRange2", \
    "MplsPortIndependentUpperRange2", \
    "MirrorEgressTrueColorSelect", \
    "MirrorEgressTruePriority", \
    "PFCClass0Queue", \
    "PFCClass1Queue", \
    "PFCClass2Queue", \
    "PFCClass3Queue", \
    "PFCClass4Queue", \
    "PFCClass5Queue", \
    "PFCClass6Queue", \
    "PFCClass7Queue", \
    "PFCClass0McastQueue", \
    "PFCClass1McastQueue", \
    "PFCClass2McastQueue", \
    "PFCClass3McastQueue", \
    "PFCClass4McastQueue", \
    "PFCClass5McastQueue", \
    "PFCClass6McastQueue", \
    "PFCClass7McastQueue", \
    "PFCClass0DestmodQueue", \
    "PFCClass1DestmodQueue", \
    "PFCClass2DestmodQueue", \
    "PFCClass3DestmodQueue", \
    "PFCClass4DestmodQueue", \
    "PFCClass5DestmodQueue", \
    "PFCClass6DestmodQueue", \
    "PFCClass7DestmodQueue", \
    "PFCQueue0Class", \
    "PFCQueue1Class", \
    "PFCQueue2Class", \
    "PFCQueue3Class", \
    "PFCQueue4Class", \
    "PFCQueue5Class", \
    "PFCQueue6Class", \
    "PFCQueue7Class", \
    "ReserveLowL3InterfaceId", \
    "ReserveHighL3InterfaceId", \
    "ReserveLowL3EgressId", \
    "ReserveHighL3EgressId", \
    "ReserveLowVlanPort", \
    "ReserveHighVlanPort", \
    "ReserveLowMplsPort", \
    "ReserveHighMplsPort", \
    "ReserveLowMimPort", \
    "ReserveHighMimPort", \
    "ReserveLowEncap", \
    "ReserveHighEncap", \
    "ReserveLowL2Egress", \
    "ReserveHighL2Egress", \
    "ReserveLowVpn", \
    "ReserveHighVpn", \
    "ReserveLowFailoverId", \
    "ReserveHighFailoverId", \
    "ReserveLowOamEndPointId", \
    "ReserveHighOamEndPointId", \
    "ReserveLowOamGroupId", \
    "ReserveHighOamGroupId", \
    "SnapNonZeroOui", \
    "SynchronousPortClockSource", \
    "SynchronousPortClockSourceBkup", \
    "SynchronousPortClockSourceDivCtrl", \
    "SynchronousPortClockSourceBkupDivCtrl", \
    "L2HitClear", \
    "L2SrcHitClear", \
    "L2DstHitClear", \
    "L3HostHitClear", \
    "L3RouteHitClear", \
    "IpmcSameVlanPruning", \
    "Deprecated492", \
    "Deprecated493", \
    "L2McIdxRetType", \
    "L3McIdxRetType", \
    "Deprecated496", \
    "Deprecated497", \
    "Deprecated498", \
    "Deprecated499", \
    "Deprecated500", \
    "ArpDhcpToCpu", \
    "IgmpMldToCpu", \
    "LinkDownInfoSkip", \
    "Deprecated504", \
    "L2SourceDiscardMoveToCpu", \
    "NivEthertype", \
    "NivPrioDropToCpu", \
    "NivInterfaceMissToCpu", \
    "NivRpfFailToCpu", \
    "NivTagInvalidToCpu", \
    "NivTagDropToCpu", \
    "NivUntagDropToCpu", \
    "EEEPipelineTime", \
    "EEEGlobalCongestionThreshold", \
    "Deprecated515", \
    "Deprecated516", \
    "Deprecated517", \
    "Deprecated518", \
    "Deprecated519", \
    "Deprecated520", \
    "Deprecated521", \
    "RemoteCpuSchanEnable", \
    "RemoteCpuFromCpuEnable", \
    "RemoteCpuToCpuEnable", \
    "RemoteCpuCmicEnable", \
    "RemoteCpuMatchLocalMac", \
    "RemoteCpuMatchVlan", \
    "RemoteCpuVlan", \
    "RemoteCpuTpid", \
    "RemoteCpuSignature", \
    "RemoteCpuForceScheduling", \
    "RemoteCpuToCpuDestPortAllReasons", \
    "RemoteCpuToCpuDestMacAllReasons", \
    "RxRedirectPktReasons", \
    "RxRedirectPktReasonsExtended", \
    "RxRedirectHigigPktReasons", \
    "RxRedirectHigigPktReasonsExtended", \
    "RxRedirectTruncatedPktReasons", \
    "RxRedirectTruncatedPktReasonsExtended", \
    "RxRedirectPktCos", \
    "RxRedirectPktCosExtended", \
    "RemoteCpuLocalMacOui", \
    "RemoteCpuLocalMacNonOui", \
    "RemoteCpuDestMacOui", \
    "RemoteCpuDestMacNonOui", \
    "RemoteCpuEthertype", \
    "RemoteCpuDestPort", \
    "RemoteCpuHigigDestPort", \
    "RemoteCpuTcMappingMacOui", \
    "RemoteCpuTcMappingMacNonOui", \
    "RemoteCpuTcMappingEthertype", \
    "ServiceTpidReplace", \
    "EgressPktAge", \
    "FabricTrunkDynamicSampleRate", \
    "FabricTrunkDynamicEgressBytesExponent", \
    "FabricTrunkDynamicQueuedBytesExponent", \
    "FabricTrunkDynamicEgressBytesDecreaseReset", \
    "FabricTrunkDynamicQueuedBytesDecreaseReset", \
    "FabricTrunkDynamicEgressBytesMinThreshold", \
    "FabricTrunkDynamicEgressBytesMaxThreshold", \
    "FabricTrunkDynamicQueuedBytesMinThreshold", \
    "FabricTrunkDynamicQueuedBytesMaxThreshold", \
    "MacLowPower", \
    "CongestionCntag", \
    "CongestionCntagEthertype", \
    "CongestionCnm", \
    "CongestionCnmEthertype", \
    "CongestionNotificationIdHigh", \
    "CongestionNotificationIdLow", \
    "CongestionNotificationIdQueue", \
    "CongestionUseOuterTpid", \
    "CongestionUseOuterVlan", \
    "CongestionUseOuterPktPri", \
    "CongestionUseOuterCfi", \
    "CongestionUseInnerPktPri", \
    "CongestionUseInnerCfi", \
    "CongestionMissingCntag", \
    "CongestionExcludeReplications", \
    "MiMDefaultSVP", \
    "MiMDefaultSVPValue", \
    "EgressDroppedReportZeroLength", \
    "LoadBalanceHashSelect", \
    "TrillEthertype", \
    "TrillISISEthertype", \
    "TrillISISDestMacOui", \
    "TrillBroadcastDestMacOui", \
    "TrillEndStationDestMacOui", \
    "TrillISISDestMacNonOui", \
    "TrillBroadcastDestMacNonOui", \
    "TrillEndStationDestMacNonOui", \
    "TrillMinTtl", \
    "TrillTtlCheckEnable", \
    "TrillTtlErrToCpu", \
    "TrillHeaderErrToCpu", \
    "TrillMismatchToCpu", \
    "TrillNameMissToCpu", \
    "TrillRpfFailToCpu", \
    "TrillOptionsToCpu", \
    "TrillNameErrDrop", \
    "TrillRpfFailDrop", \
    "TrillHeaderVersionErrDrop", \
    "TrillNameMissDrop", \
    "TrillAdjacencyFailDrop", \
    "TrillHeaderErrDrop", \
    "UdpPktActionEnable", \
    "UdpPktRedirectPbmp", \
    "UdpPktCopyToPbmp", \
    "UdpPktNewTc", \
    "L3TunnelIpV4ModeOnly", \
    "L3SrcHitEnable", \
    "L2DstHitEnable", \
    "WESPProtocolEnable", \
    "WESPProtocol", \
    "Deprecated614", \
    "Ip6CompressEnable", \
    "Ip6CompressDefaultOffset", \
    "StableSaveLongIds", \
    "GportAnyDefaultL2Learn", \
    "GportAnyDefaultL2Move", \
    "IngParseL2TunnelTermDipSipSelect", \
    "IngParseL3L4IPv4", \
    "IngParseL3L4IPv6", \
    "L2LearnMode", \
    "SetWredJitterMask", \
    "SetMplsEntropyLabelTtl", \
    "SetMplsEntropyLabelPri", \
    "SetMplsEntropyLabelOffset", \
    "PonOamFailsafeState", \
    "EquipmentAlarmState", \
    "PowerAlarmState", \
    "BatteryMissingAlarmState", \
    "BatteryFailureAlarmState", \
    "BatteryVoltLowAlarmState", \
    "BatteryVoltLowAlarmReportThreshold", \
    "BatteryVoltLowAlarmClearThreshold", \
    "PhysicalIntrusionAlarmState", \
    "SelfTestFailureAlarmState", \
    "PonIfSwitchAlarmState", \
    "L3IngressInterfaceMapSet", \
    "MplsPWControlWordToCpu", \
    "MplsPWControlTypeToCpu", \
    "MplsPWControlWord", \
    "MplsPWCountPktsAll", \
    "EntropyHashSet0Offset", \
    "EntropyHashSet1Offset", \
    "ArpRequestMyStationIPToCPU", \
    "ArpRequestMyStationIPDrop", \
    "ArpRequestMyStationIPFwd", \
    "L3SIPMoveToCpu", \
    "TrunkHashSrcPortEnable", \
    "TrunkHashOffset", \
    "TrunkHashMPLSLabelBOS", \
    "TrunkHashMPLSPWControlWord", \
    "TrunkHashSeed", \
    "TrunkHashConfig", \
    "TrunkHashPktHeaderCount", \
    "TrunkHashPktHeaderSelect", \
    "ECMPHashSrcPortEnable", \
    "ECMPHashOffset", \
    "ECMPHashSeed", \
    "ECMPHashConfig", \
    "ECMPHashPktHeaderCount", \
    "TrunkDynamicHashOffset", \
    "TrunkDynamicSampleRate", \
    "TrunkDynamicAccountingSelect", \
    "TrunkDynamicEgressBytesExponent", \
    "TrunkDynamicQueuedBytesExponent", \
    "TrunkDynamicEgressBytesDecreaseReset", \
    "TrunkDynamicQueuedBytesDecreaseReset", \
    "TrunkDynamicEgressBytesMinThreshold", \
    "TrunkDynamicEgressBytesMaxThreshold", \
    "TrunkDynamicQueuedBytesMinThreshold", \
    "TrunkDynamicQueuedBytesMaxThreshold", \
    "TrunkDynamicExpectedLoadMinThreshold", \
    "TrunkDynamicExpectedLoadMaxThreshold", \
    "TrunkDynamicImbalanceMinThreshold", \
    "TrunkDynamicImbalanceMaxThreshold", \
    "EcmpDynamicHashOffset", \
    "EcmpDynamicSampleRate", \
    "EcmpDynamicAccountingSelect", \
    "EcmpDynamicEgressBytesExponent", \
    "EcmpDynamicQueuedBytesExponent", \
    "EcmpDynamicEgressBytesDecreaseReset", \
    "EcmpDynamicQueuedBytesDecreaseReset", \
    "EcmpDynamicEgressBytesMinThreshold", \
    "EcmpDynamicEgressBytesMaxThreshold", \
    "EcmpDynamicQueuedBytesMinThreshold", \
    "EcmpDynamicQueuedBytesMaxThreshold", \
    "EcmpDynamicExpectedLoadMinThreshold", \
    "EcmpDynamicExpectedLoadMaxThreshold", \
    "EcmpDynamicImbalanceMinThreshold", \
    "EcmpDynamicImbalanceMaxThreshold", \
    "FailoverDropToCpu", \
    "MplsReservedEntropyLabelToCpu", \
    "L3SrcBindMissToCpu", \
    "MplsGalAlertLabelToCpu", \
    "MplsRalAlertLabelToCpu", \
    "MplsIllegalReservedLabelToCpu", \
    "MplsLookupsExceededToCpu", \
    "MplsUnknownAchTypeToCpu", \
    "MplsUnknownAchVersionToCpu", \
    "TimesyncIngressVersion", \
    "TimesyncEgressVersion", \
    "TimesyncUnknownVersionToCpu", \
    "CongestionCnmToCpu", \
    "bcmSwtichCongestionCnmProxyErrorToCpu", \
    "bcmSwtichCongestionCnmProxyToCpu", \
    "WlanTunnelErrorDrop", \
    "MplsReservedEntropyLabelDrop", \
    "RemoteProtectionTrust", \
    "L2GreProtocolType", \
    "L2GreVpnIdSizeSet", \
    "L2GreVpnIdMissToCpu", \
    "L2GreTunnelMissToCpu", \
    "MultipathCompressBuffer", \
    "MultipathCompress", \
    "BstEnable", \
    "BstTrackingMode", \
    "L3McastL2", \
    "MiMTeBvidLowerRange", \
    "MiMTeBvidUpperRange", \
    "RemoteEncapsulationMode", \
    "FabricTrunkAutoIncludeDisable", \
    "TrunkHashNormalize", \
    "MplsLabelCache", \
    "MplsLabelCommit", \
    "TrillHopCount", \
    "PolicerFairness", \
    "VxlanUdpDestPortSet", \
    "VxlanEntropyEnable", \
    "VxlanVnIdMissToCpu", \
    "VxlanTunnelMissToCpu", \
    "ECMPVxlanHashOffset", \
    "ECMPL2GreHashOffset", \
    "ECMPTrillHashOffset", \
    "ECMPMplsHashOffset", \
    "VirtualPortDynamicHashOffset", \
    "TMDomain", \
    "PortHeaderType", \
    "L2OverflowEvent", \
    "LinkProvClear", \
    "EtagEthertype", \
    "EtagInvalidToCpu", \
    "EtagDropToCpu", \
    "NonEtagDropToCpu", \
    "ExtenderRpfFailToCpu", \
    "ExtenderInterfaceMissToCpu", \
    "PrioDropToCpu", \
    "ExtenderMulticastLowerThreshold", \
    "ExtenderMulticastHigherThreshold", \
    "L3NATEnable", \
    "L3DNATHairpinToCpu", \
    "L3DNATMissToCpu", \
    "L3SNATMissToCpu", \
    "L3NatOtherToCpu", \
    "L3NatRealmCrossingIcmpToCpu", \
    "L3NatFragmentsToCpu", \
    "HashL3DNATPool", \
    "HashL3DNATPoolDual", \
    "McastTrunkHashMin", \
    "McastTrunkHashMax", \
    "Ip4Compress", \
    "EcmpResilientHashOffset", \
    "TrunkResilientHashOffset", \
    "FabricTrunkResilientHashOffset", \
    "ShaperAdjust", \
    "HashVpVlanMemberIngress", \
    "HashVpVlanMemberIngressDual", \
    "HashVpVlanMemberEgress", \
    "HashVpVlanMemberEgressDual", \
    "HashL2Endpoint", \
    "HashL2EndpointDual", \
    "HashEndpointQueueMap", \
    "HashEndpointQueueMapDual", \
    "StackRouteHistoryBitmap1", \
    "StackRouteHistoryBitmap2", \
    "ArpIPcheck", \
    "Deprecated778", \
    "Deprecated779", \
    "L2LearnLimitToCpu", \
    "FlexibleMirrorDestinations", \
    "EcmpMacroFlowHashEnable", \
    "OamVersionCheckDisable", \
    "OamOlpChipEtherType", \
    "OamOlpChipTpid", \
    "OamOlpChipVlan", \
    "Deprecated787", \
    "EgressKeepSystemHeader", \
    "MacroFlowEcmpHashConcatEnable", \
    "MacroFlowLoadBalanceHashConcatEnable", \
    "MacroFlowTrunkHashConcatEnable", \
    "MacroFlowHigigTrunkHashConcatEnable", \
    "MacroFlowECMPHashSeed", \
    "MacroFlowLoadBalanceHashSeed", \
    "MacroFlowTrunkHashSeed", \
    "MacroFlowHigigTrunkHashSeed", \
    "HashTrillPayloadSelect0", \
    "HashTrillPayloadSelect1", \
    "HashTrillTunnelSelect0", \
    "HashTrillTunnelSelect1", \
    "HashIP6AddrCollapseSelect0", \
    "HashIP6AddrCollapseSelect1", \
    "PrependTagEnable", \
    "HashL2VxlanField0", \
    "HashL2VxlanField1", \
    "HashL3VxlanField0", \
    "HashL3VxlanField1", \
    "HashL2L2GreField0", \
    "HashL2L2GreField1", \
    "HashL3L2GreField0", \
    "HashL3L2GreField1", \
    "McastTrunkIngressCommit", \
    "McastTrunkEgressCommit", \
    "MplsShortPipe", \
    "TraverseMode", \
    "TrunkResilientHashConfig", \
    "TrunkResilientHashSeed", \
    "ECMPResilientHashSeed", \
    "ResilientHashAgeTimer", \
    "FieldStageEgressToCpu", \
    "HashOamEgress", \
    "HashOamEgressDual", \
    "ColorL3L2Marking", \
    "WredForAllPkts", \
    "FcoeNpvModeEnable", \
    "FcoeDomainRoutePrefixLength", \
    "FcoeCutThroughEnable", \
    "FcoeSourceBindCheckAction", \
    "FcoeSourceFpmaPrefixCheckAction", \
    "FcoeVftHopCountExpiryToCpu", \
    "FcoeVftHopCountExpiryAction", \
    "FcoeFcEofT1Value", \
    "FcoeFcEofT2Value", \
    "FcoeFcEofA1Value", \
    "FcoeFcEofA2Value", \
    "FcoeFcEofN1Value", \
    "FcoeFcEofN2Value", \
    "FcoeFcEofNI1Value", \
    "FcoeFcEofNI2Value", \
    "FcoeZoneCheckFailToCpu", \
    "FcoeZoneCheckMissDrop", \
    "HashUseL2GreTunnelGreKey0", \
    "HashUseL2GreTunnelGreKey1", \
    "HashL2GrePayloadSelect0", \
    "HashL2GrePayloadSelect1", \
    "HashL2GreNetworkPortPayloadDisable0", \
    "HashL2GreNetworkPortPayloadDisable1", \
    "SystemReservedVlan", \
    "ProxySrcKnockout", \
    "NetworkGroupDepth", \
    "UnknownSubportPktTagToCpu", \
    "L3RoutedLearn", \
    "SubportPktTagEthertype", \
    "SubportPktTagToCpu", \
    "NonSubportPktTagToCpu", \
    "HashL2GreKeyMask0", \
    "HashL2GreKeyMask1", \
    "L3Max128BV6Entries", \
    "EEEQueueThresholdProfile0", \
    "EEEQueueThresholdProfile1", \
    "EEEQueueThresholdProfile2", \
    "EEEQueueThresholdProfile3", \
    "EEEPacketLatencyProfile0", \
    "EEEPacketLatencyProfile1", \
    "EEEPacketLatencyProfile2", \
    "EEEPacketLatencyProfile3", \
    "DosAttackV6MinFragEnable", \
    "DosAttackV6MinFragSize", \
    "MirrorUnicastCosq", \
    "MirrorMulticastCosq", \
    "McQueueSchedMode", \
    "Reserved872", \
    "CacheTableUpdateAll", \
    "HashNivSrcIfEtagSvidSelect0", \
    "HashNivSrcIfEtagSvidSelect1", \
    "HashNivDstIfEtagVidSelect0", \
    "HashNivDstIfEtagVidSelect1", \
    "BstSnapshotEnable", \
    "SymmetricHashControl", \
    "L2ExtLearn", \
    "ReserveHighTunnelId", \
    "ReserveLowTunnelId", \
    "HigigCongestionClassEnable", \
    "EcnNonIpResponsive", \
    "EcnNonIpIntCongestionNotification", \
    "MimPayloadTpidEnable", \
    "HashMimUseTunnelHeader0", \
    "HashMimUseTunnelHeader1", \
    "MplsPipeTunnelLabelExpSet", \
    "ControlPortConfigInstall", \
    "LoopbackMtuSize", \
    "HashVxlanPayloadSelect0", \
    "HashVxlanPayloadSelect1", \
    "CongestionCnmSrcMacNonOui", \
    "CongestionCnmSrcMacOui", \
    "HashL2MPLSPayloadSelect0", \
    "HashL2MPLSPayloadSelect1", \
    "HashL3L2MPLSField0", \
    "HashL3L2MPLSField1", \
    "HashIPSecSelect0", \
    "HashIPSecSelect1", \
    "ECMPLevel1RandomSeed", \
    "ECMPLevel2RandomSeed", \
    "TimesyncEgressTimestampingMode", \
    "SubportCoEEtherType", \
    "SubportEgressTpid", \
    "SubportEgressWideTpid", \
    "IpmcSameVlanPruningOverride", \
    "SynchronousPortClockSourceValid", \
    "SynchronousPortClockSourceBkupValid", \
    "RemoteProtectionEnable", \
    "MirrorExclusive", \
    "EcnDelayMeasurementThreshold", \
    "MacroFlowHashUseMSB", \
    "MplsDefaultTtlCopy", \
    "OamStackingSupport", \
    "HashOam", \
    "HashOamDual", \
    "HashOamLeastFull", \
    "HashDualMoveDepthOam", \
    "PortVlanTranslationAdvanced", \
    "ForwardingLayerMtuFilter", \
    "LinkLayerMtuFilter", \
    "ForwardingLayerMtuSize", \
    "LinkLayerMtuSize", \
    "MimBvidInsertionControl", \
    "L2CpuDeleteEvent", \
    "L2LearnEvent", \
    "L2CpuAddEvent", \
    "L2AgingEvent", \
    "UdfHashEnable", \
    "OlpMatchRule", \
    "MplsExtendedLabelTtl", \
    "MacroFlowHashOverlayMaxOffset", \
    "MacroFlowHashOverlayStrideOffset", \
    "MacroFlowHashOverlayMinOffset", \
    "MacroFlowEcmpHashOverlayConcatEnable", \
    "MacroFlowECMPOverlayHashSeed", \
    "ECMPOverlayHashOffset", \
    "MacroFlowEcmpUnderlayHashConcatEnable", \
    "MacroFlowECMPUnderlayHashSeed", \
    "ECMPUnderlayVxlanHashOffset", \
    "ECMPUnderlayL2GreHashOffset", \
    "ECMPUnderlayTrillHashOffset", \
    "VirtualPortUnderlayDynamicHashOffset", \
    "ECMPUnderlayHashSet0Offset", \
    "SampleFlexRandomSeed", \
    "NonUcVlanShapingEnable", \
    "CustomHeaderEncapPriorityOffset", \
    "IngCapwapEnable", \
    "CapwapDataUdpDstPort", \
    "CountCapwapPayloadBytesOnly", \
    "OamOlpStationMacOui", \
    "OamOlpStationMacNonOui", \
    "Reserved955", \
    "PFCDeadlockDetectionTimeInterval", \
    "PFCDeadlockRecoveryAction", \
    "RangeCheckersAPIType", \
    "Reserved958", \
    "EcmpGroupMemberMode", \
    "MimlEncapInnerEthertype", \
    "HashMiMPayloadSelect0", \
    "HashMiMPayloadSelect1", \
    "HashL3MiMField0", \
    "HashL3MiMField1", \
    "HashMPLSUseLabelStack", \
    "HashMPLSExcludeGAL", \
    "RedirectBufferThresholdPriorityLow", \
    "RedirectBufferThresholdPriorityMedium", \
    "RedirectBufferThresholdPriorityHigh", \
    "FlexIP6ExtHdr", \
    "FlexIP6ExtHdrEgress", \
    "HashUseFlowSelMplsEcmp", \
    "V6L3SrcDstLocalLinkDropCancel", \
    "L3ClassIdForL2Enable", \
    "DropSobmhOnLinkDown", \
    "ResilientHashCRC0ByteMask", \
    "ResilientHashCRC1ByteMask", \
    "ResilientHashCRC2ByteMask", \
    "ResilientHashCRC3ByteMask", \
    "ResilientHashCRCSeed", \
    "ECMPResilientHashKey0Shift", \
    "ECMPResilientHashKey1Shift", \
    "VpnDetachPorts", \
    "ClassL3L2Marking", \
    "L3L2MarkingMode", \
    "STPDisabledFieldBypass", \
    "HashSctpL4Port", \
    "Reserved988", \
    "GlobalTpidEnable", \
    "ECMPResilientHashCombine", \
    "HashSetTflowMode", \
    "Downgrade", \
    "HashELISearch", \
    "CrashRecoveryMode", \
    "CrCommit", \
    "CrLastTransactionStatus", \
    "CrCouldNotRecover", \
    "HWL2Freeze", \
    "HashIP6NextHeaderUseExtensionHeader0", \
    "HashIP6NextHeaderUseExtensionHeader1", \
    "HashUseFlowSelLbidNonUnicast", \
    "HashUseFlowSelLbidUnicast", \
    "HashUseFlowSelHigigTrunkNonUnicast", \
    "HashUseFlowSelHigigTrunkUnicast", \
    "HashUseFlowSelHigigTrunkFailover", \
    "HashUseFlowSelTrunkNonUnicast", \
    "HashUseFlowSelEntropy", \
    "HashUseFlowSelVxlanEcmp", \
    "DefaultNativeOutVlanPort", \
    "DefaultEgressVlanEditClassId", \
    "GtpDetectEnable", \
    "GtpHdrFirstByte", \
    "GtpHdrFirstByteMask", \
    "HashGtpTeidEnable0", \
    "HashGtpTeidEnable1", \
    "EccSingleBitErrorEvent", \
    "HeaderCompensationPerPacket", \
    "3rdTagStampMode", \
    "MacroFlowHigigTrunkHashMinOffset", \
    "MacroFlowHigigTrunkHashMaxOffset", \
    "MacroFlowHigigTrunkHashStrideOffset", \
    "MacroFlowTrunkHashMinOffset", \
    "MacroFlowTrunkHashMaxOffset", \
    "MacroFlowTrunkHashStrideOffset", \
    "MacroFlowLoadBalanceEntropyHashMinOffset", \
    "MacroFlowLoadBalanceEntropyHashMaxOffset", \
    "MacroFlowLoadBalanceEntropyHashStrideOffset", \
    "CrTransactionStart", \
    "UdpTunnelIPv4DstPort", \
    "UdpTunnelIPv6DstPort", \
    "UdpTunnelMplsDstPort", \
    "ECMPSecondHierHashOffset", \
    "ECMPSecondHierHashConfig", \
    "IngressRateLimitMpps", \
    "MirrorSnoopForwardOriginalWhenDropped", \
    "FabricTrunkDynamicPhysicalQueuedBytesExponent", \
    "FabricTrunkDynamicPhysicalQueuedBytesDecreaseReset", \
    "FabricTrunkDynamicPhysicalQueuedBytesMinThreshold", \
    "FabricTrunkDynamicPhysicalQueuedBytesMaxThreshold", \
    "TrunkDynamicPhysicalQueuedBytesExponent", \
    "TrunkDynamicPhysicalQueuedBytesDecreaseReset", \
    "TrunkDynamicPhysicalQueuedBytesMinThreshold", \
    "TrunkDynamicPhysicalQueuedBytesMaxThreshold", \
    "EcmpDynamicPhysicalQueuedBytesExponent", \
    "EcmpDynamicPhysicalQueuedBytesDecreaseReset", \
    "EcmpDynamicPhysicalQueuedBytesMinThreshold", \
    "EcmpDynamicPhysicalQueuedBytesMaxThreshold", \
    "FabricTrunkDynamicRandomSeed", \
    "TrunkDynamicRandomSeed", \
    "EcmpDynamicRandomSeed", \
    "HashMPLSExcludeReservedLabel", \
    "MplsNonIpPayloadDefaultEcn", \
    "ECMPHashBitCountSelect", \
    "EcnIpPayloadResponsive", \
    "HashMultiMoveDepthExactMatch", \
    "CrAbort", \
    "LIUdpSrcPortSet", \
    "Roce1Ethertype", \
    "Roce2UdpDstPort", \
    "HashUseFlowSelRoce2Ecmp", \
    "HashSelectControlExtended", \
    "HashField0Roce1FlowLabel", \
    "HashField1Roce1FlowLabel", \
    "HashField0Roce2FlowLabel", \
    "HashField1Roce2FlowLabel", \
    "HashRoce1Field0", \
    "HashRoce1Field1", \
    "HashRoce2IP4Field0", \
    "HashRoce2IP4Field1", \
    "HashRoce2IP6Field0", \
    "HashRoce2IP6Field1", \
    "EcmpRoce2HashOffset", \
    "VxlanUdpDestPortSet1", \
    "VxlanEntropyEnableIP6", \
    "E2EFCReceiveDestMacOui", \
    "E2EFCReceiveDestMacNonOui", \
    "E2EFCReceiveEtherType", \
    "E2EFCReceiveOpcode", \
    "HashUseFlowSelEcmpOverlay", \
    "MplsSpeculativeNibbleMap", \
    "IpmcSameFwdDmnThreshold", \
    "HashUseFlowSelVpTrunk", \
    "HashUseFlowSelTrunkResilientHash", \
    "MeterMef10dot3Enable", \
    "HashMultiMoveDepthVlanTranslate1", \
    "HashMultiMoveDepthVlanTranslate2", \
    "HashMultiMoveDepthEgressVlanTranslate1", \
    "HashMultiMoveDepthEgressVlanTranslate2", \
    "IntL4DestPort1", \
    "IntL4DestPort2", \
    "IntProbeMarker1", \
    "IntProbeMarker2", \
    "IntVersion", \
    "IntRequestVectorValue", \
    "IntRequestVectorMask", \
    "IntSwitchId", \
    "IntEgressTimeDelta", \
    "IntHopLimitCpuEnable", \
    "IntTurnAroundCpuEnable", \
    "IntMaxPayloadLength", \
    "ForwardLookupNotFoundTrap", \
    "EtherTypeCustom1", \
    "EtherTypeCustom2", \
    "L3LpmHitbitEnable", \
    "MplsCtrlPktToCpu", \
    "InvalidTpidToCpu", \
    "FlowSelectMissToCpu", \
    "L3HdrMismatchToCpu", \
    "MeterTrafficClass", \
    "RoeEthertype", \
    "RoeCustomEthertype", \
    "RoeUdpDestPort", \
    "RoeCustomUdpDestPort", \
    "OamOlpHeaderAdd", \
    "L2FreezeMode", \
    "L3McRpfDefaultRoute", \
    "IngParseL2TunnelTermArp", \
    "IngParseL2TunnelTermRarp", \
    "NoneEthernetPacketMeterPtr", \
    "EtherDscpRemark", \
    "DosAttackTcpFirstOption", \
    "DosAttackInnerTcpFirstOption", \
    "DosAttackTcpFlagsMask", \
    "DosAttackInnerTcpFlagsMask", \
    "DosAttackTcpFlagsSR", \
    "DosAttackTcpFlagsZero", \
    "DosAttackTcpFlagsSynOption", \
    "DosAttackTcpFlagsSynFirstFrag", \
    "DosAttackTcpFlagsFinNoAck", \
    "DosAttackTcpFlagsRstNoAck", \
    "DosAttackTcpFlagsNoSFRA", \
    "DosAttackTcpFlagsUrgNoUrgPtr", \
    "DosAttackTcpFlagsNoUrgWithUrgPtr", \
    "DosAttackTcpDstPortZero", \
    "DosAttackTcpSrcPortZero", \
    "DosAttackTcpFlagsResvdNonZero", \
    "DosAttackTcpV4DstBcast", \
    "DosAttackTcpFlagsAck", \
    "MirrorPsampFormat2Epoch", \
    "L2StationExtendedMode", \
    "HashSeed", \
    "LayerRecordModeSelection", \
    "ECMPThirdHierHashConfig", \
    "NwkHashConfig", \
    "MplsStack0HashSeed", \
    "MplsStack1HashSeed", \
    "ParserHashSeed", \
    "MplsSpeculativeParse", \
    "ExternalTcamSync", \
    "ArpMyIp1", \
    "ArpMyIp2", \
    "ArpIgnoreDa", \
    "NdpMyIp1", \
    "NdpMyIp2", \
    "IcmpIgnoreDa", \
    "VxlanEvpnEnable", \
    "HashDualMoveDepthIngVpVlanMember", \
    "HashDualMoveDepthEgrVpVlanMember", \
    "GpeEntropyEnable", \
    "GpeUdpDestPortSet", \
    "GeneveEntropyEnable", \
    "GeneveUdpDestPortSet", \
    "BfdRxDisable", \
    "DescCommit", \
    "DlbMonitorIngressRandomSeed", \
    "CounterAdjustEgressTm", \
    "CounterAdjustEgressPp", \
    "TagPcpDeiSrc", \
    "BfdMyDipDestination", \
    "EndToEndLatencyPerDestinationPort", \
    "ExternalTcamInit", \
    "HashMultiMoveDepthL3Tunnel", \
    "GbpEtherType", \
    "VxlanUdpSrcPortSelect", \
    "GpeUdpSrcPortSelect", \
    "GeneveUdpSrcPortSelect", \
    "PreserveOtagBeforeItag", \
    "MirrorCpuCosq", \
    "IntProbeMarkerEnable", \
    "IntL4DestPortEnable", \
    "FtmhEtherType", \
    "GbpSidDidMcastLookupMissClassid", \
    "CustomOuterEthertype1", \
    "CustomOuterEthertype2", \
    "CustomInnerEthertype1", \
    "CustomInnerEthertype2", \
    "CustomOuterProtocolId1", \
    "CustomOuterProtocolId2", \
    "CustomInnerProtocolId1", \
    "CustomInnerProtocolId2", \
    "CustomOuterUdpDstPort1", \
    "CustomOuterUdpDstPort2", \
    "CustomInnerUdpDstPort1", \
    "CustomInnerUdpDstPort2", \
    "ReservedEgressNextHop", \
    "ConcatAppHeader", \
    "VxlanGpeUdpDestPortSet", \
    "SystemPortOverride", \
    "SRV6EgressPSPEnable", \
    "SRV6ReducedModeEnable", \
    "HashL4Field", \
    "L3UcLargeNATEnable", \
    "L3UcLargeDNAT", \
    "L3UcLargeSNAT", \
    "L3UcLargeDNATSNAT", \
    "L3UcLargeDNAPT", \
    "L3UcLargeSNAPT", \
    "L3UcLargeDNAPTSNAPT", \
    "L3McLargeNATEnable", \
    "L3McLargeNATRewriteType0", \
    "L3McLargeNATRewriteType1", \
    "L3McLargeNATRewriteType2", \
    "L2ChangeFieldsEnable", \
    "ModuleVerifyEnable", \
    "TraverseCommitDone", \
    "HashIP4InnerField", \
    "HashIP4OuterField", \
    "HashIP6InnerField", \
    "HashIP6OuterField", \
    "HashL2InnerField", \
    "HashL2OuterField", \
    "MirrorPsampIpfixVersion", \
    "LlcHeaderControlMode", \
    "PcieHotSwapDisable", \
    "TrunkDynamicMonitorIngressRandomSeed", \
    "L2TunnelTpid", \
    "EcnNonTcpResponsive", \
    "Higig3Ethertype", \
    "IPv4DoNotVerifyIncomingChecksum", \
    "HashUseFlowSelTrunkFailover", \
    "HashUseFlowSelHigigTrunk", \
    "HashUseFlowSelTrunkDynamic", \
    "HashUseHigig3EntropyTrunkNonUnicast", \
    "TrunkUnicastHashOffset", \
    "TrunkNonUnicastHashOffset", \
    "HigigTrunkHashOffset", \
    "VpTrunkHashOffset", \
    "MacroFlowTrunkUnicastHashConcatEnable", \
    "MacroFlowTrunkNonUnicastHashConcatEnable", \
    "MacroFlowTrunkFailoverHashConcatEnable", \
    "MacroFlowHigigTrunkFailoverHashConcatEnable", \
    "MacroFlowTrunkDynamicHashConcatEnable", \
    "MacroFlowVpTrunkHashConcatEnable", \
    "MacroFlowTrunkUnicastHashSeed", \
    "MacroFlowTrunkNonUnicastHashSeed", \
    "MacroFlowTrunkFailoverHashSeed", \
    "MacroFlowHigigTrunkFailoverHashSeed", \
    "MacroFlowTrunkDynamicHashSeed", \
    "MacroFlowVpTrunkHashSeed", \
    "MacroFlowTrunkUnicastHashMinOffset", \
    "MacroFlowTrunkUnicastHashMaxOffset", \
    "MacroFlowTrunkUnicastHashStrideOffset", \
    "MacroFlowTrunkNonUnicastHashMinOffset", \
    "MacroFlowTrunkNonUnicastHashMaxOffset", \
    "MacroFlowTrunkNonUnicastHashStrideOffset", \
    "MacroFlowTrunkFailoverHashMinOffset", \
    "MacroFlowTrunkFailoverHashMaxOffset", \
    "MacroFlowTrunkFailoverHashStrideOffset", \
    "MacroFlowHigigTrunkFailoverHashMinOffset", \
    "MacroFlowHigigTrunkFailoverHashMaxOffset", \
    "MacroFlowHigigTrunkFailoverHashStrideOffset", \
    "MacroFlowTrunkDynamicHashMinOffset", \
    "MacroFlowTrunkDynamicHashMaxOffset", \
    "MacroFlowTrunkDynamicHashStrideOffset", \
    "MacroFlowVpTrunkHashMinOffset", \
    "MacroFlowVpTrunkHashMaxOffset", \
    "MacroFlowVpTrunkHashStrideOffset", \
    "TrunkRandomSeed", \
    "HigigTrunkRandomSeed", \
    "VPTrunkRandomSeed", \
    "ModuleErrorRecoveryEnable", \
    "HashMultiMoveDepthVlanTranslate3", \
    "HashMultiMoveDepthL2Tunnel", \
    "HashMultiMoveDepthL2TunnelVnid", \
    "HashMultiMoveDepthFlowTracker", \
    "HashMultiMoveDepthIpmcGroup", \
    "HashMultiMoveDepthIpmcSourceGroup", \
    "HashMultiMoveDepthL2mc", \
    "MactAgeRefreshMode", \
    "DosAttackMaxIpPayloadEnable", \
    "DosAttackMaxIpPayloadSize", \
    "DosAttackMinIpPayloadEnable", \
    "DosAttackMinIpPayloadSize", \
    "DosAttackMaxInnerIpPayloadEnable", \
    "DosAttackMaxInnerIpPayloadSize", \
    "DosAttackMinInnerIpPayloadEnable", \
    "DosAttackMinInnerIpPayloadSize", \
    "EccSingleBitBusBufferErrorEvent", \
    "HashLayersDisable", \
    "MplsAlternateMarkingSpecialLabel", \
    "SvtagSciProfile", \
    "SvtagErrCodeProfile", \
    "LatencyEcnMarkMode", \
    "GlobalTodMode", \
    "TimesyncLiveWireToD", \
    "L3TunnelCollapseDisable", \
    "ControlOamBfdFailoverStateIgnore", \
    "EntropyHashOffset", \
    "MacroFlowEntropyHashConcatEnable", \
    "MacroFlowEntropyHashSeed", \
    "SubportLowPriPfcSel", \
    "SubportHighPriPfcSel", \
    "ExactMatchInit", \
    "RangeTypeSelect", \
    "RangeOperator", \
    "RangeResultMap", \
    "IFA2Enable", \
    "IFA2HeaderType", \
    "IFA2MetaDataFirstLookupType", \
    "IFA2MetaDataSecondLookupType", \
    "MacroFlowEcmpDynamicHashConcatEnable", \
    "MacroFlowEcmpDynamicHashSeed", \
    "MacroFlowEcmpDynamicHashMinOffset", \
    "MacroFlowEcmpDynamicHashMaxOffset", \
    "MacroFlowEcmpDynamicHashStrideOffset", \
    "TrunkUnicastHashBitCountSelect", \
    "TrunkNonunicastHashBitCountSelect", \
    "TrunkFailoverHashBitCountSelect", \
    "HigigTrunkHashBitCountSelect", \
    "HigigTrunkFailoverHashBitCountSelect", \
    "TrunkDynamicHashBitCountSelect", \
    "VpTrunkHashBitCountSelect", \
    "EntropyHashBitCountSelect", \
    "EcmpOverlayHashBitCountSelect", \
    "EcmpDynamicHashBitCountSelect", \
    "GpeNextProtoGbp", \
    "IoamMaxPayloadLength", \
    "IoamNameSpaceId", \
    "GpeNextProtoIoam", \
    "GpeNextProtoIoamE2e", \
    "PFCStopInterfaceBitmap", \
    "IFAMetaDataAdaptTable", \
    "L3UcSameInterfaceDrop", \
    "L2ApplCallbackSuppress", \
    "OtpAvsValue", \
    "SampleVersion", \
    "HashUseFlowSelEcmpWeighted", \
    "EcmpWeightedHashOffset", \
    "MacroFlowEcmpWeightedHashMinOffset", \
    "MacroFlowEcmpWeightedHashMaxOffset", \
    "MacroFlowEcmpWeightedHashStrideOffset", \
    "MacroFlowEcmpWeightedHashConcatEnable", \
    "MplsPwControlWordUpperClear", \
    "MplsPwControlWordLowerClear", \
    "MplsGal", \
    "MplsEli", \
    "MplsPwTermSeqNumberRange", \
    "EgressVlanTranslate2Enable", \
    "PointToPointAccessVlanCheckEnable", \
    "NshEgressAdaptationKeySelect", \
    "NshMetaDataSourceInterfaceSelect", \
    "NshMetaDataTenantIdSelect", \
    "NshTenantIdMappingMissDropEnable", \
    "NshEthertype", \
    "NshEthertypeEnable", \
    "L3IngTagActionsEnable", \
    "ECMPRandomSeed", \
    "HashVersatileInitValue0", \
    "HashVersatileInitValue1", \
    "Hash0VersatileInitValue0", \
    "Hash0VersatileInitValue1", \
    "Hash1VersatileInitValue0", \
    "Hash1VersatileInitValue1", \
    "IoamE2eNextProtoGbp", \
    "MirrorUnicastCosqEnable", \
    "FlowCountHashShift", \
    "FlowCountHashMask", \
    "FlowCountFlexCtrIngAction", \
    "SLLBDestinationNotFound", \
    "VIPEcmpTableSize", \
    "VIPL3EgressIDMin", \
    "VIPL3EgressIDMax", \
    "PCCAgeTimer", \
    "PCCLearnLimit", \
    "FlexeOamAlmCollectionTimerStep", \
    "FlexeOamAlmCollectionStepCount", \
    "IfaVersion", \
    "IfaProtocol", \
    "IoamGreProtocolParse", \
    "IoamGreProtocol", \
    "IoamOptionIncrementalParse", \
    "IoamOptionIncremental", \
    "IoamType", \
    "IntVectorMatchMissAction", \
    "1588ClockAddressOui", \
    "1588ClockAddressNonOui", \
    "HashUseFlowSelL2Ecmp", \
    "HashUseFlowSelL2EcmpOverlay", \
    "HashUseFlowSelEcmpResilientHash", \
    "HashUseFlowSelEcmpOverlayResilientHash", \
    "HashUseFlowSelEcmpWeightedOverlay", \
    "EcmpOverlayResilientHashOffset", \
    "EcmpOverlayMplsHashOffset", \
    "EcmpOverlayHashOffset", \
    "EcmpOverlayWeightedHashOffset", \
    "L2EcmpHashOffset", \
    "L2EcmpOverlayHashOffset", \
    "MacroFlowEcmpResilientHashConcatEnable", \
    "MacroFlowEcmpResilientHashMinOffset", \
    "MacroFlowEcmpResilientHashMaxOffset", \
    "MacroFlowEcmpResilientHashStrideOffset", \
    "MacroFlowEcmpOverlayResilientHashConcatEnable", \
    "MacroFlowEcmpOverlayResilientHashMinOffset", \
    "MacroFlowEcmpOverlayResilientHashMaxOffset", \
    "MacroFlowEcmpOverlayResilientHashStrideOffset", \
    "MacroFlowEcmpMplsHashConcatEnable", \
    "MacroFlowEcmpMplsHashMinOffset", \
    "MacroFlowEcmpMplsHashMaxOffset", \
    "MacroFlowEcmpMplsHashStrideOffset", \
    "MacroFlowEcmpOverlayMplsHashConcatEnable", \
    "MacroFlowEcmpOverlayMplsHashMinOffset", \
    "MacroFlowEcmpOverlayMplsHashMaxOffset", \
    "MacroFlowEcmpOverlayMplsHashStrideOffset", \
    "MacroFlowL2EcmpHashConcatEnable", \
    "MacroFlowL2EcmpHashMinOffset", \
    "MacroFlowL2EcmpHashMaxOffset", \
    "MacroFlowL2EcmpHashStrideOffset", \
    "MacroFlowL2EcmpOverlayHashConcatEnable", \
    "MacroFlowL2EcmpOverlayHashMinOffset", \
    "MacroFlowL2EcmpOverlayHashMaxOffset", \
    "MacroFlowL2EcmpOverlayHashStrideOffset", \
    "MacroFlowEcmpWeightedOverlayHashConcatEnable", \
    "MacroFlowEcmpWeightedOverlayHashMinOffset", \
    "MacroFlowEcmpWeightedOverlayHashMaxOffset", \
    "MacroFlowEcmpWeightedOverlayHashStrideOffset", \
    "HashUseFlowSelMplsEcmpOverlay", \
    "ExemScanPeriod", \
    "VxlanHeaderFlags", \
    "VxlanHeaderMatchFlags", \
    "VxlanHeaderFlagsMask", \
    "VxlanHeaderFlagsMismatchDrop", \
    "VxlanHeaderReserved1", \
    "VxlanHeaderReserved2", \
    "FlexeSarCellMode", \
    "MplsSpecialLabelAutoTerminateDisable", \
    "ExternalTableCapacity", \
    "ExemFlushProfilePayloadOffset", \
    "NtpL4DestPort", \
    "NtpResidenceTimeUpdate", \
    "V6L3LocalLinkSrcDisposition", \
    "MirrorAllowDuplicateMirroredPackets", \
    "EgressStgCheckIngressEnable", \
    "VxlanIPv4TunnelVlanTranslateEnable", \
    "VxlanIPv6TunnelVlanTranslateEnable", \
    "EgressFlexCounterPresel" 

/* 
 * NSH Egress adaptation key types used by
 * bcmSwitchNshEgressAdaptationKeySelect.
 */
#define BCM_SWITCH_NSH_EGR_ADAPT_KEY_OPAQUE_OBJECT (0)        /* NSH egress adaptation
                                                          key is opaque object. */
#define BCM_SWITCH_NSH_EGR_ADAPT_KEY_OPAQUE_OBJECT_VRF (1)        /* NSH egress adaptation
                                                          key is opaque object
                                                          and VRF ID. */

/* 
 * NSH metadata source interface types used by
 * bcmSwitchNshMetaDataSourceInterfaceSelect.
 */
#define BCM_SWITCH_NSH_METADATA_SRC_INTF_SGPP (0)        /* NSH metadata source
                                                          interface is SGPP. */
#define BCM_SWITCH_NSH_METADATA_SRC_INTF_FROM_EGR_ADAPT (1)        /* NSH metadata source
                                                          interface is from
                                                          egress adaptation
                                                          lookup. */

/* 
 * NSH metadata tenant ID source used by
 * bcmSwitchNshMetaDataTenantIdSelect.
 */
#define BCM_SWITCH_NSH_METADATA_TENANT_ID_VRF (0)        /* NSH metadata tenant ID
                                                          is VRF ID. */
#define BCM_SWITCH_NSH_METADATA_TENANT_ID_FROM_EGR_ADAPT (1)        /* NSH metadata tenant ID
                                                          is from egress
                                                          adaptation lookup. */

/* Requires BROADCOM_PREMIUM license */
#define BCM_SWITCH_IFA_METADATA_ADAPT1_SINGLE 0          /* Select single wide
                                                          adaptation table 1 for
                                                          IFA metadata. */
#define BCM_SWITCH_IFA_METADATA_ADAPT1_DOUBLE 1          /* Select double wide
                                                          adaptation table 1 for
                                                          IFA metadata. */
#define BCM_SWITCH_IFA_METADATA_ADAPT2_SINGLE 2          /* Select single wide
                                                          adaptation table 2 for
                                                          IFA metadata. */
#define BCM_SWITCH_IFA_METADATA_ADAPT2_DOUBLE 3          /* Select double wide
                                                          adaptation table 2 for
                                                          IFA metadata. */

/* ecmp hash bit count enums */
typedef enum bcm_switch_ecmp_hash_bit_count_e {
    BCM_SWITCH_ECMP_HASH_BIT_COUNT_LOWER_10 = 0, 
    BCM_SWITCH_ECMP_HASH_BIT_COUNT_LOWER_11 = 1, 
    BCM_SWITCH_ECMP_HASH_BIT_COUNT_LOWER_12 = 2, 
    BCM_SWITCH_ECMP_HASH_BIT_COUNT_LOWER_13 = 3, 
    BCM_SWITCH_ECMP_HASH_BIT_COUNT_LOWER_14 = 4, 
    BCM_SWITCH_ECMP_HASH_BIT_COUNT_LOWER_15 = 5, 
    BCM_SWITCH_ECMP_HASH_BIT_COUNT_LOWER_16 = 6 
} bcm_switch_ecmp_hash_bit_count_t;

/* 
 * Values passed for setting switch control
 * bcmSwitchPFCDeadlockRecoveryAction.
 */
typedef enum bcm_switch_pfc_deadlock_action_e {
    bcmSwitchPFCDeadlockActionTransmit = 0, /* Transmit packets in Recovery state
                                           (ignoring the PFC state)-Default
                                           Action */
    bcmSwitchPFCDeadlockActionDrop = 1, /* Drop packets in Recovery state. */
    bcmSwitchPFCDeadlockActionMaxCount = 2 /* Maximum PFC Deadlock Recovery action
                                           type count */
} bcm_switch_pfc_deadlock_action_t;

/* 
 * Values passed for setting switch control
 * bcmSwitchPFCDeadlockDetectionTimeInterval..
 */
typedef enum bcm_switch_pfc_deadlock_detection_interval_e {
    bcmSwitchPFCDeadlockDetectionInterval100MiliSecond = 0, /* 100 ms interval for Detection
                                           interval (default) */
    bcmSwitchPFCDeadlockDetectionInterval10MiliSecond = 1, /* 10ms interval for Detection interval */
    bcmSwitchPFCDeadlockDetectionIntervalCount = 2 /* Maximum PFC deadlock detecion type
                                           count */
} bcm_switch_pfc_deadlock_detection_interval_t;

/* Values passed for setting switch control bcmSwitchOamStackingSupport */
typedef enum bcm_switch_oam_stacking_support_e {
    bcmSwitchOamStackingSupportNone = 0, /* Do not carry OAM endpoint context in
                                           place of VC label in stacking header */
    bcmSwitchOamStackingSupportOamContextInStackingHeader = 1 /* Carry OAM endpoint context in place
                                           of VC label in stacking header */
} bcm_switch_oam_stacking_support_t;

/* Values passed for setting switch control bcmSwitchGlobalTodMode */
typedef enum bcm_switch_tod_mode_e {
    bcmSwitchTodModeNone = 0,       /* No TOD mode */
    bcmSwitchTodModeOam = 1,        /* The TOD mode is OAM */
    bcmSwitchTodModeNtp = 2,        /* The TOD mode is NTP */
    bcmSwitchTodModeOamAndNtp = 3,  /* The TOD mode is OAM and NTP */
    bcmSwitchTodModeCount = 4       /* Maximum TOD mode count */
} bcm_switch_tod_mode_t;

/* Values passed for setting switch control bcmSwitchFlexeSarCellMode */
typedef enum bcm_switch_flexe_sar_cell_mode_e {
    bcmSwitchFlexeSarCellMode28x66b = 0, /* The SAR cell mode is 28x66b, the
                                           actual cell size is 236B(including
                                           SAR OH) */
    bcmSwitchFlexeSarCellMode29x66b = 1, /* The SAR cell mode is 29x66b, the
                                           actual cell size is 244B(including
                                           SAR OH) */
    bcmSwitchFlexeSarCellModeCount = 2  /* Maximum SAR cell mode count */
} bcm_switch_flexe_sar_cell_mode_t;

typedef enum bcm_switch_external_table_e {
    bcmSwitchExternalTableIp4Unicast = 0, 
    bcmSwitchExternalTableIp4UnicastOptimized = 1, 
    bcmSwitchExternalTableIp4Rpf = 2, 
    bcmSwitchExternalTableIp6Unicast = 3, 
    bcmSwitchExternalTableIp6UnicastOptimized = 4, 
    bcmSwitchExternalTableIp6Rpf = 5, 
    bcmSwitchExternalTableIp4Multicast = 6, 
    bcmSwitchExternalTableIp6Multicast = 7, 
    bcmSwitchExternalTableNat = 8 
} bcm_switch_external_table_t;

/* Requires BROADCOM_PREMIUM license */
#define BCM_L3UC_NAT_NONE       0          /* Disable the specified NAT update. */
#define BCM_L3UC_NAT_TCP        1          /* Enable Unicast NAT updates for TCP
                                              packets only. */
#define BCM_L3UC_NAT_UDP        2          /* Enable Unicast NAT updates for UDP
                                              packets only. */
#define BCM_L3UC_NAT            3          /* Enable Unicast NAT updates for any
                                              IPV4 packet. */

/* Requires BROADCOM_PREMIUM license */
#define BCM_L3MC_NAT_NONE                   0          /* Disable the specified
                                                          NAT update. */
#define BCM_L3MC_DNAT                       1          /* Enable Multicast DNAT
                                                          updates. */
#define BCM_L3MC_DNAT_WITH_MACDA_UPDATE     2          /* Enable Multicast DNAT
                                                          updates and MACDA
                                                          update. */
#define BCM_L3MC_SNAT                       3          /* Enable Multicast SNAT
                                                          updates. */
#define BCM_L3MC_DNAT_SNAT                  4          /* Enable both Multicast
                                                          DNAT and SNAT updates. */
#define BCM_L3MC_DNAT_SNAT_WITH_MACDA_UPDATE 5          /* Enable both Multicast
                                                          DNAT and SNAT updates
                                                          and MACDA update. */
#define BCM_L3MC_DNAPT                      6          /* Enable Multicast DNAPT
                                                          updates. */
#define BCM_L3MC_DNAPT_WITH_MACDA_UPDATE    7          /* Enable Multicast DNAPT
                                                          updates and MACDA
                                                          update. */
#define BCM_L3MC_SNAPT                      8          /* Enable Multicast SNAPT
                                                          updates. */
#define BCM_L3MC_DNAPT_SNAPT                9          /* Enable both Multicast
                                                          DNAPT and SNAPT
                                                          updates. */
#define BCM_L3MC_DNAPT_SNAPT_WITH_MACDA_UPDATE 10         /* Enable both Multicast
                                                          DNAPT and SNAPT
                                                          updates and MACDA
                                                          update. */

#define BCM_SWITCH_CONTROL_L3_LEARN_NONE    0          /* Disable L2 learning
                                                          for all routed
                                                          packets. */
#define BCM_SWITCH_CONTROL_L3_LEARN_IPV4_UC 0x1        /* Enable L2 learning for
                                                          IPv4 Unicast packets. */
#define BCM_SWITCH_CONTROL_L3_LEARN_IPV4_MC 0x2        /* Enable L2 learning for
                                                          IPv4 Multicast
                                                          packets. */
#define BCM_SWITCH_CONTROL_L3_LEARN_IPV6_UC 0x4        /* Enable L2 learning for
                                                          IPv6 Unicast packets. */
#define BCM_SWITCH_CONTROL_L3_LEARN_IPV6_MC 0x8        /* Enable L2 learning for
                                                          IPv6 Multicast
                                                          packets. */
#define BCM_SWITCH_CONTROL_L3_LEARN_MPLS    0x10       /* Enable L2 learning for
                                                          MPLS packets. */
#define BCM_SWITCH_CONTROL_L3_LEARN_ALL     0xffffffff /* Enable L2 learning for
                                                          all routed packets. */

/* Module types for bcmSwitchModuleType. */
#define BCM_SWITCH_MODULE_UNKNOWN   0          
#define BCM_SWITCH_MODULE_XGS1      1          
#define BCM_SWITCH_MODULE_XGS2      2          
#define BCM_SWITCH_MODULE_XGS3      3          

/* Unicast RPF lookup modes. */
#define BCM_SWITCH_URPF_DISABLE (0)        
#define BCM_SWITCH_URPF_LOOSE   (1)        
#define BCM_SWITCH_URPF_STRICT  (2)        

/* Port header type. */
#define BCM_SWITCH_PORT_HEADER_TYPE_NONE    (0)        
#define BCM_SWITCH_PORT_HEADER_TYPE_ETH     (1)        
#define BCM_SWITCH_PORT_HEADER_TYPE_RAW     (2)        
#define BCM_SWITCH_PORT_HEADER_TYPE_TM      (3)        
#define BCM_SWITCH_PORT_HEADER_TYPE_PROG    (4)        
#define BCM_SWITCH_PORT_HEADER_TYPE_CPU     (5)        
#define BCM_SWITCH_PORT_HEADER_TYPE_STACKING (6)        
#define BCM_SWITCH_PORT_HEADER_TYPE_TDM     (7)        
#define BCM_SWITCH_PORT_HEADER_TYPE_TDM_RAW (8)        
#define BCM_SWITCH_PORT_HEADER_TYPE_MPLS_RAW (9)        
#define BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP (10)       
#define BCM_SWITCH_PORT_HEADER_TYPE_TM_SSP  (11)       
#define BCM_SWITCH_PORT_HEADER_TYPE_TM_ENCAP (12)       
#define BCM_SWITCH_PORT_HEADER_TYPE_TM_SSP_ENCAP (13)       
#define BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2 (14)       
#define BCM_SWITCH_PORT_HEADER_TYPE_RCH_0   (15)       
#define BCM_SWITCH_PORT_HEADER_TYPE_RCH_1   (16)       
#define BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP_JR1_MODE (17)       
#define BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_JR1_MODE (18)       
#define BCM_SWITCH_PORT_HEADER_TYPE_ENCAP_EXTERNAL_CPU (19)       
#define BCM_SWITCH_PORT_HEADER_TYPE_TDM_ETH (20)       
#define BCM_SWITCH_PORT_HEADER_TYPE_INJECTED (21)       

/* ECMP group mode. */
#define BCM_SWITCH_ECMP_GROUP_MEMBER_MODE_256 (0)        
#define BCM_SWITCH_ECMP_GROUP_MEMBER_MODE_1K (1)        

/* Argument formatting macro for bcmSwitchModuleType. */
#define BCM_SWITCH_CONTROL_MOD_TYPE_PACK(modid, mod_type)  \
    (((modid) << 16) | (mod_type & 0xFFFF)) 
#define BCM_SWITCH_CONTROL_MODID_UNPACK(arg)  \
    (((arg) >> 16) & 0xFFFF) 
#define BCM_SWITCH_CONTROL_MODTYPE_UNPACK(arg)  \
    (((arg) >> 0) & 0xFFFF) 

/* Stable locations for bcmSwitchStableSelect */
#define BCM_SWITCH_STABLE_NONE              (_SHR_SWITCH_STABLE_NONE) 
#define BCM_SWITCH_STABLE_DEVICE_NEXT_HOP   (_SHR_SWITCH_STABLE_DEVICE_NEXT_HOP) 
#define BCM_SWITCH_STABLE_DEVICE_EXT_MEM    (_SHR_SWITCH_STABLE_DEVICE_EXT_MEM) 
#define BCM_SWITCH_STABLE_APPLICATION       (_SHR_SWITCH_STABLE_APPLICATION) 
#define BCM_SWITCH_STABLE_SHARED_MEM        (_SHR_SWITCH_STABLE_SHARED_MEM) 

/* Flags for automatic link provision clear. */
#define BCM_SWITCH_LINK_PROV_AUTO_CLEAR     0x00000001 /* Clear link provision
                                                          automatically. */
#define BCM_SWITCH_LINK_PROV_ALLOW_LIST_EXCLUDE 0x00000002 /* Retain allow list when
                                                          clearing link
                                                          provision. */
#define BCM_SWITCH_LINK_PROV_BLOCK_LIST_EXCLUDE 0x00000004 /* Retain block list when
                                                          clearing link
                                                          provision. */

/* 
 * Egress flex counter preselect, used for
 * bcmSwitchEgressFlexCounterPresel.
 */
typedef enum bcm_switch_egress_flex_counter_presel_e {
    bcmSwitchEgressFlexCounterPreselMirror = 0, /* Pre-select to determine if counting
                                           mirror packets or not. */
    bcmSwitchEgressFlexCounterPreselCopyToCpu = 1, /* Pre-select to determine if counting
                                           copy to CPU packets or not. */
    bcmSwitchEgressFlexCounterPreselMirrorAndCopyToCpu = 2, /* Pre-select to determine if counting
                                           both mirror and copy to CPU packets
                                           or not. */
    bcmSwitchEgressFlexCounterPreselCount = 3 /* Must be last. Not a usable value. */
} bcm_switch_egress_flex_counter_presel_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Configure port-specific and device-wide operating modes. Device wide
 * operating
 * modes are configured on all ports, except the stack ports.
 */
extern int bcm_switch_control_get(
    int unit, 
    bcm_switch_control_t type, 
    int *arg);

/* 
 * Configure port-specific and device-wide operating modes. Device wide
 * operating
 * modes are configured on all ports, except the stack ports.
 */
extern int bcm_switch_control_set(
    int unit, 
    bcm_switch_control_t type, 
    int arg);

/* 
 * Configure port-specific and device-wide operating modes. Device wide
 * operating
 * modes are configured on all ports, except the stack ports.
 */
extern int bcm_switch_control_port_get(
    int unit, 
    bcm_port_t port, 
    bcm_switch_control_t type, 
    int *arg);

/* 
 * Configure port-specific and device-wide operating modes. Device wide
 * operating
 * modes are configured on all ports, except the stack ports.
 */
extern int bcm_switch_control_port_set(
    int unit, 
    bcm_port_t port, 
    bcm_switch_control_t type, 
    int arg);

/* Configure encapsulation priority for the internal priority queue. */
extern int bcm_switch_rcpu_encap_priority_map_set(
    int unit, 
    uint32 flags, 
    int internal_cpu_pri, 
    int encap_pri);

/* Retrieve encapsulation priority of the internal priority queue. */
extern int bcm_switch_rcpu_encap_priority_map_get(
    int unit, 
    uint32 flags, 
    int internal_cpu_pri, 
    int *encap_pri);

/* Configure decapsulation priority for the internal priority queue. */
extern int bcm_switch_rcpu_decap_priority_map_set(
    int unit, 
    int decap_pri, 
    int internal_cpu_pri);

/* Retrieve decapsulation priority of the internal priority queue. */
extern int bcm_switch_rcpu_decap_priority_map_get(
    int unit, 
    int decap_pri, 
    int *internal_cpu_pri);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Hash tables. */
typedef enum bcm_switch_hash_table_e {
    bcmHashTableL2 = 0,                 /* L2 hash table. */
    bcmHashTableL3 = 1,                 /* L3 hash table. */
    bcmHashTableVlanTranslate = 2,      /* VLAN Translate hash table. */
    bcmHashTableEgressVlanTranslate = 3, /* Egress VLAN Translate hash table. */
    bcmHashTableMPLS = 4,               /* MPLS hash table. */
    bcmHashTableFlowtrackerEntry = 5,   /* Flowtracker Entry hash table. */
    bcmHashTableL2Tunnel = 6,           /* L2 tunnel termination hash table. */
    bcmHashTableL2TunnelVnid = 7,       /* L2 tunnel VNID hash table. */
    bcmHashTableL3Tunnel = 8,           /* L3 tunnel termination hash table. */
    bcmHashTableExactMatch = 9,         /* FP exact match hash table. */
    bcmHashTableIpmcGroup = 10,         /* L3 IPv4/IPv6 multicast (*,G) hash
                                           table. */
    bcmHashTableIpmcSourceGroup = 11,   /* L3 IPv4/IPv6 multicast (S,G) hash
                                           table. */
    bcmHashTableL2mc = 12,              /* L2 IPv4/IPv6 multicast hash table. */
    bcmHashTable__Num = 13 
} bcm_switch_hash_table_t;

/* L2 address learning modes. */
typedef enum bcm_switch_l2_location_e {
    bcmSwitchL2LocInternal = 0, /* L2 addresses will be learnt on Internal TCAM */
    bcmSwitchL2LocExternal = 1, /* L2 addresses will be learnt on External TCAM */
    bcmSwitchL2LocIntAndExt = 2 /* L2 addresses are learnt either on internal or
                                   external Tcam */
} bcm_switch_l2_location_t;

/* Switch table update mode tables. */
typedef enum bcm_switch_table_update_mode_e {
    bcmSwitchTableUpdateNormal = 0,     /* Calling traverse API e.g.
                                           l2_replace() will act in normal mode,
                                           i.e. set match rule and action and
                                           perform the replace */
    bcmSwitchTableUpdateRuleAdd = 1,    /* Calling traverse API e.g.
                                           l2_replace() will add rule  without
                                           perform the replace */
    bcmSwitchTableUpdateRuleClear = 2,  /* clear all rules e.g. l2_replace()
                                           Calling traverse API */
    bcmSwitchTableUpdateRuleCommit = 3, /* Run traverse API e.g. l2_replace()
                                           with configured rules */
    bcmSwitchTableUpdateRuleCommitNonBlocking = 4 /* Start the flush machine and don't
                                           wait for it to finish. */
} bcm_switch_table_update_mode_t;

/* Opaque handle to a hash entry. */
typedef int bcm_hash_entry_t;

/* Use config structure to setup the offset and mask. */
#define BCM_SWITCH_HASH_USE_CONFIG  (~0U)      

/* mask length in config struct. */
#define BCM_SWITCH_HASH_ENTRY_CONFIG_MASK_LENGTH 4          

/* field selection in flexible hash RAM table */
typedef enum bcm_switch_hash_field_e {
    bcmSwitchFlexHashField0 = 0,        /* field 0 in flexible hash RAM table */
    bcmSwitchFlexHashField1 = 1,        /* field 1 in flexible hash RAM table */
    bcmSwitchFlexHashField2 = 2,        /* field 2 in flexible hash RAM table,
                                           fed to RTAG7 bin 4, support only UDF
                                           type field */
    bcmSwitchFlexHashField3 = 3,        /* field 3 in flexible hash RAM table,
                                           fed to RTAG7 bin 5, support only UDF
                                           type field */
    bcmSwitchFlexHashField4 = 4,        /* field 4 in flexible hash RAM table,
                                           fed to RTAG7 bin 6, support only UDF
                                           type field */
    bcmSwitchFlexHashField5 = 5,        /* field 5 in flexible hash RAM table,
                                           fed to RTAG7 bin 7, support only UDF
                                           type field */
    bcmSwitchFlexHashField6 = 6,        /* field 6 in flexible hash RAM table,
                                           fed to RTAG7 bin 8, support only UDF
                                           type field */
    bcmSwitchFlexHashField7 = 7,        /* field 7 in flexible hash RAM table,
                                           fed to RTAG7 bin 9, support only UDF
                                           type field */
    bcmSwitchFlexHashField8 = 8,        /* field 8 in flexible hash RAM table,
                                           fed to RTAG7 bin 10, support only UDF
                                           type field */
    bcmSwitchFlexHashField9 = 9,        /* field 9 in flexible hash RAM table,
                                           fed to RTAG7 bin 11, support only UDF
                                           type field */
    bcmSwitchFlexHashField10 = 10,      /* field 10 in flexible hash RAM table,
                                           fed to RTAG7 bin 12, support only UDF
                                           type field */
    bcmSwitchFlexHashField11 = 11,      /* field 11 in flexible hash RAM table,
                                           fed to RTAG7 bin 13, support only UDF
                                           type field */
    bcmSwitchFlexHashField12 = 12,      /* field 12 in flexible hash RAM table,
                                           fed to RTAG7 bin 1, support only UDF
                                           type field */
    bcmSwitchFlexHashFieldCount = 13    /* field count in flexible hash RAM
                                           table */
} bcm_switch_hash_field_t;

/* Values passed for setting switch control bcmSwitchLatencyEcnMode. */
typedef enum bcm_switch_latency_ecn_mark_mode_e {
    bcmSwitchLatencyEcnMarkInstant = 0, /*  Instantaneous mode. */
    bcmSwitchLatencyEcnMarkAverage = 1, /*  Average mode. */
    bcmSwitchLatencyEcnMarkCount = 2    /*  Must be last. Not a usable value. */
} bcm_switch_latency_ecn_mark_mode_t;

/* Flags for flex hash entry configration. */
#define BCM_SWITCH_HASH_FIELD_TYPE_UDF  1          

/* flexible hash entry configuration struct. */
typedef struct bcm_switch_hash_entry_config_s {
    uint32 flags;                       /* control flags, default value
                                           indicates offset type flex hash */
    bcm_switch_hash_field_t field;      /* field selector, currently there are
                                           two fields in flex hash ram table */
    uint32 offset;                      /* hash offset when field type is
                                           offset, relative offset in bits from
                                           start in udf_id_t when field type is
                                           udf. The value of (start + offset)
                                           should be a multiple of (8*2) to
                                           ensure the interested field starts at
                                           even byte in a packet */
    int mask_length;                    /* number of bytes in the mask */
    uint8 mask[BCM_SWITCH_HASH_ENTRY_CONFIG_MASK_LENGTH]; /* hash mask */
    bcm_udf_layer_t base_layer;         /* this is unsupported for now */
    bcm_udf_id_t udf_id;                /* udf id when field type is udf chunk */
} bcm_switch_hash_entry_config_t;

/* Initialize a bcm_switch_hash_entry_config_t structure. */
extern void bcm_switch_hash_entry_config_t_init(
    bcm_switch_hash_entry_config_t *config);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set a flex hash entry configuration information. */
extern int bcm_switch_hash_entry_set(
    int unit, 
    bcm_hash_entry_t entry, 
    bcm_switch_hash_entry_config_t *config);

/* Get a flex hash entry configuration information. */
extern int bcm_switch_hash_entry_get(
    int unit, 
    bcm_hash_entry_t entry, 
    bcm_switch_hash_entry_config_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* flexible hash CAM/RAM table traverse callback */
typedef int (*bcm_switch_hash_entry_traverse_cb)(
    int unit, 
    int flags, 
    bcm_hash_entry_t entry, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse all configured flex hash entry to callback. */
extern int bcm_switch_hash_entry_traverse(
    int unit, 
    int flags, 
    bcm_switch_hash_entry_traverse_cb cb_fn, 
    void *user_data);

/* Set a flex hash entry configuration information. */
extern int bcm_switch_hash_qualify_udf(
    int unit, 
    bcm_hash_entry_t entry, 
    bcm_udf_id_t udf_id, 
    int length, 
    uint8 *data, 
    uint8 *mask);

/* Get a flex hash entry configuration information. */
extern int bcm_switch_hash_qualify_udf_get(
    int unit, 
    bcm_hash_entry_t entry, 
    bcm_udf_id_t udf_id, 
    int max_length, 
    uint8 *data, 
    uint8 *mask, 
    int *actual_length);

/* Installs a flex hash entry into the hardware tables. */
extern int bcm_switch_hash_entry_install(
    int unit, 
    bcm_hash_entry_t entry, 
    uint32 offset);

/* Re-Install a flex hash entry into the hardware tables. */
extern int bcm_switch_hash_entry_reinstall(
    int unit, 
    bcm_hash_entry_t entry, 
    uint32 offset);

/* Remove a flex hash entry from the hardware tables. */
extern int bcm_switch_hash_entry_remove(
    int unit, 
    bcm_hash_entry_t entry);

/* Create a blank flex hash entry. */
extern int bcm_switch_hash_entry_create(
    int unit, 
    bcm_field_group_t group, 
    bcm_hash_entry_t *entry);

/* Create a blank flex hash entry. */
extern int bcm_switch_hash_entry_create_qset(
    int unit, 
    bcm_field_qset_t qset, 
    bcm_hash_entry_t *entry);

/* Destroy a blank flex hash entry. */
extern int bcm_switch_hash_entry_destroy(
    int unit, 
    bcm_hash_entry_t entry);

/* Add flexible hashing data field qualifiers */
extern int bcm_switch_hash_qualify_data(
    int unit, 
    bcm_hash_entry_t entry, 
    int qual_id, 
    uint32 data, 
    uint32 mask);

/* Get number of banks for a hash table */
extern int bcm_switch_hash_banks_max_get(
    int unit, 
    bcm_switch_hash_table_t hash_table, 
    uint32 *bank_count);

/* Set/Get hash type for a multi bank hash table */
extern int bcm_switch_hash_banks_config_set(
    int unit, 
    bcm_switch_hash_table_t hash_table, 
    uint32 bank_num, 
    int hash_type, 
    uint32 hash_offset);

/* Set/Get hash type for a multi bank hash table */
extern int bcm_switch_hash_banks_config_get(
    int unit, 
    bcm_switch_hash_table_t hash_table, 
    uint32 bank_num, 
    int *hash_type, 
    uint32 *hash_offset);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Entry type for retrieving temperature values. */
typedef _shr_switch_temperature_monitor_t bcm_switch_temperature_monitor_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * There are temperature monitors embedded in the various points of some
 * switch chips for the purpose of monitoring the health of the chip.
 * This API retrieves each temperature monitor's current value and
 * peak value. The value unit is 0.1 celsius degree. Table \ref
 * bcm_switch_temperature_monitor_t
 * describes the meaning of each field of the structure
 * bcm_switch_temperature_monitor_t.
 */
extern int bcm_switch_temperature_monitor_get(
    int unit, 
    int temperature_max, 
    bcm_switch_temperature_monitor_t *temperature_array, 
    int *temperature_count);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Switch packet info flags. */
#define BCM_SWITCH_PKT_INFO_SRC_GPORT       (1 << 0)   /* Source gport (module,
                                                          port) field valid. */
#define BCM_SWITCH_PKT_INFO_VLAN            (1 << 1)   /* VLAN identifier field
                                                          valid. */
#define BCM_SWITCH_PKT_INFO_ETHERTYPE       (1 << 2)   /* EtherType field valid. */
#define BCM_SWITCH_PKT_INFO_SRC_MAC         (1 << 3)   /* Source MAC address
                                                          field valid. */
#define BCM_SWITCH_PKT_INFO_DST_MAC         (1 << 4)   /* Destination MAC
                                                          address field valid. */
#define BCM_SWITCH_PKT_INFO_SRC_IP          (1 << 5)   /* Source IPv4 address
                                                          field valid. */
#define BCM_SWITCH_PKT_INFO_DST_IP          (1 << 6)   /* Destination IPv4
                                                          address field valid. */
#define BCM_SWITCH_PKT_INFO_SRC_IPV6        (1 << 7)   /* Source IPv6 address
                                                          field valid. */
#define BCM_SWITCH_PKT_INFO_DST_IPV6        (1 << 8)   /* Destination IPv6
                                                          address field valid. */
#define BCM_SWITCH_PKT_INFO_PROTOCOL        (1 << 9)   /* IP protocol field
                                                          valid. */
#define BCM_SWITCH_PKT_INFO_SRC_L4_PORT     (1 << 10)  /* Source L4 port field
                                                          valid. */
#define BCM_SWITCH_PKT_INFO_DST_L4_PORT     (1 << 11)  /* Destination L4 port
                                                          field valid. */
#define BCM_SWITCH_PKT_INFO_HASH_TRUNK      (1 << 12)  /* Retrieve trunk hash
                                                          result. */
#define BCM_SWITCH_PKT_INFO_HASH_MULTIPATH  (1 << 13)  /* Retrieve ECMP hash
                                                          result. */
#define BCM_SWITCH_PKT_INFO_HASH_UDP_SOURCE_PORT (1 << 14)  /* Retrieve Entropy Value
                                                          of VXLan packet. */
#define BCM_SWITCH_PKT_INFO_HASH_LBID       (1 << 15)  /* Retrieve LBID. */
#define BCM_SWITCH_PKT_INFO_HASH_OVERLAY    (1 << 16)  /* Retrieve ECMP hash
                                                          result for overlay
                                                          layer. */
#define BCM_SWITCH_PKT_INFO_TOP_LABEL       (1 << 17)  /* First label in MPLS
                                                          stack. */
#define BCM_SWITCH_PKT_INFO_2ND_LABEL       (1 << 18)  /* Secondt label in MPLS
                                                          stack. */
#define BCM_SWITCH_PKT_INFO_3RD_LABEL       (1 << 19)  /* Third label in MPLS
                                                          stack. */
#define BCM_SWITCH_PKT_INFO_LABELS_4MSB     (1 << 20)  /* 4MSB of top, second
                                                          and third label in
                                                          MPLS stack. */

/* Packet forward reason for hash calculation. */
typedef enum bcm_switch_pkt_hash_info_fwd_reason_e {
    bcmSwitchPktHashInfoFwdReasonUnicast = 0, /* Known unicast packet */
    bcmSwitchPktHashInfoFwdReasonIpmc = 1, /* L3 multicast packet */
    bcmSwitchPktHashInfoFwdReasonL2mc = 2, /* L2 multicast packet */
    bcmSwitchPktHashInfoFwdReasonBcast = 3, /* Broadcast packet */
    bcmSwitchPktHashInfoFwdReasonDlf = 4 /* Destination lookup failed packet */
} bcm_switch_pkt_hash_info_fwd_reason_t;

#define BCM_SWITCH_PKT_HASH_INFO_FWD_REASON_STRINGS \
    "Unicast", \
    "Ipmc", \
    "L2mc", \
    "Bcast", \
    "Dlf" 

/* 
 * Switch packet info structure.
 * 
 * Contains packet fields for calculating aggregate link hash values.
 */
typedef struct bcm_switch_pkt_info_s {
    uint32 flags;                       /* BCM_SWITCH_PKT_INFO_xxx flags for
                                           packet parameter hashing. */
    bcm_gport_t src_gport;              /* Source module and port,
                                           BCM_GPORT_INVALID to wildcard. */
    bcm_vlan_t vid;                     /* VLAN ID. */
    bcm_ethertype_t ethertype;          /* EtherType */
    bcm_mac_t src_mac;                  /* Source MAC address. */
    bcm_mac_t dst_mac;                  /* Destination MAC address. */
    bcm_ip_t sip;                       /* Source host IP address (IPv4). */
    bcm_ip_t dip;                       /* Destination host IP address (IPv4). */
    bcm_ip6_t sip6;                     /* Source host IP address (IPv6). */
    bcm_ip6_t dip6;                     /* Destination host IP address (IPv6). */
    uint8 protocol;                     /* IP protocol */
    uint32 src_l4_port;                 /* Source L4 port */
    uint32 dst_l4_port;                 /* Destination L4 port. */
    bcm_gport_t trunk_gport;            /* Gport of trunk type on which packet
                                           will egress. */
    bcm_if_t mpintf;                    /* Multipath egress forwarding object. */
    bcm_switch_pkt_hash_info_fwd_reason_t fwd_reason; /* Packet forward reason. */
    bcm_mpls_label_t top_label;         /* First label in MPLS stack. */
    bcm_mpls_label_t second_label;      /* Second label in MPLS stack. */
    bcm_mpls_label_t third_label;       /* Third label in MPLS stack. */
} bcm_switch_pkt_info_t;

/* Initialize a bcm_switch_pkt_info_t structure. */
extern void bcm_switch_pkt_info_t_init(
    bcm_switch_pkt_info_t *pkt_info);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Gets the hash result for the specified link aggregation method using
 * provided packet parameters and device configuration.
 */
extern int bcm_switch_pkt_info_hash_get(
    int unit, 
    bcm_switch_pkt_info_t *pkt_info, 
    bcm_gport_t *dst_gport, 
    bcm_if_t *dst_intf);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Read function signature to register for the application provided
 * stable for Level 2 Warm Boot
 */
typedef int (*bcm_switch_read_func_t)(
    int unit, 
    uint32 *buf, 
    int offset, 
    int nbytes);

/* 
 * Write function signature to register for the application provided
 * stable for Level 2 Warm Boot
 */
typedef int (*bcm_switch_write_func_t)(
    int unit, 
    uint32 *buf, 
    int offset, 
    int nbytes);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Register read/write functions for the application provided stable for
 * Level 2 Warm Boot
 */
extern int bcm_switch_stable_register(
    int unit, 
    bcm_switch_read_func_t rf, 
    bcm_switch_write_func_t wf);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Call back function signature to register */
typedef void (*bcm_switch_event_cb_t)(
    int unit, 
    bcm_switch_event_t event, 
    uint32 arg1, 
    uint32 arg2, 
    uint32 arg3, 
    void *userdata);

#ifndef BCM_HIDE_DISPATCHABLE

/* Registers/Unregisters a callback function for switch critical events */
extern int bcm_switch_event_register(
    int unit, 
    bcm_switch_event_cb_t cb, 
    void *userdata);

/* Registers/Unregisters a callback function for switch critical events */
extern int bcm_switch_event_unregister(
    int unit, 
    bcm_switch_event_cb_t cb, 
    void *userdata);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Backward compatibility. */
#define bcm_switch_port_control_get bcm_switch_control_port_set 
#define bcm_switch_port_control_set bcm_switch_control_port_set 

#define BCM_HASH_ZERO           0x00000001 /* Always return 0 (debug only). */
#define BCM_HASH_LSB            0x00000002 /* Return LSB of key. */
#define BCM_HASH_CRC16L         0x00000003 /* Lower bits of CRC-16. */
#define BCM_HASH_CRC16U         0x00000004 /* Upper bits of CRC-16. */
#define BCM_HASH_CRC32L         0x00000005 /* Lower bits of CRC-32. */
#define BCM_HASH_CRC32U         0x00000006 /* Upper bits of CRC-32. */
#define BCM_HASH_XORU           0x00000007 /* Upper bits of byte by byte XOR. */
#define BCM_HASH_XORL           0x00000008 /* Lower bits of byte by byte XOR. */
#define BCM_HASH_OFFSET         0x10000000 /* Use supplied offset value into the
                                              hash value. */

/* Multipath (ECMP/WCMP) hashing. */
#define BCM_HASH_CONTROL_MULTIPATH_L4PORTS  0x0001     /* Use TCP/UDP ports. */
#define BCM_HASH_CONTROL_MULTIPATH_DIP      0x0002     /* Use destination IP
                                                          address. */
#define BCM_HASH_CONTROL_MULTIPATH_USERDEF(_val)  \
    (((_val) & 0xff) << 24) 
#define BCM_HASH_CONTROL_MULTIPATH_USERDEF_VAL(c)  \
    ((c) >> 24 & 0xff) 

/* Unicast trunk hash controls. */
#define BCM_HASH_CONTROL_TRUNK_UC_XGS2      0x0004     /* 5695 compatible hash. */
#define BCM_HASH_CONTROL_TRUNK_UC_SRCPORT   0x0008     /* Use source port. */

/* Non-unicast trunk block mask hash controls. */
#define BCM_HASH_CONTROL_TRUNK_NUC_DST      0x0010     /* Use destination MAC or
                                                          IP address. */
#define BCM_HASH_CONTROL_TRUNK_NUC_SRC      0x0020     /* Use source MAC or IP
                                                          address. */
#define BCM_HASH_CONTROL_TRUNK_NUC_MODPORT  0x0040     /* Use module ID and
                                                          port. */

/* Use enhanced (aka. RTAG7) hashing algorithm. */
#define BCM_HASH_CONTROL_ECMP_ENHANCE       0x0080     /* Use enhanced ECMP
                                                          hashing. */
#define BCM_HASH_CONTROL_TRUNK_NUC_ENHANCE  0x0100     /* Use enhanced
                                                          non-unicast trunk
                                                          hashing. */
#define BCM_HASH_CONTROL_ECMP_OVERLAY_ENHANCE 0x0200     /* Use enhanced ECMP
                                                          hashing for overlay
                                                          layer. */

/* Enhanced hash field config control. */
#define BCM_HASH_FIELD_CONFIG_CRC16XOR8     0x00000001 /* 8-bit CRC16 + 8-bit
                                                          XOR8. */
#define BCM_HASH_FIELD_CONFIG_CRC16XOR4     0x00000002 /* 8-bit CRC16 + 8-bit
                                                          XOR4. */
#define BCM_HASH_FIELD_CONFIG_CRC16XOR2     0x00000003 /* 8-bit CRC16 + 8-bit
                                                          XOR2. */
#define BCM_HASH_FIELD_CONFIG_CRC16XOR1     0x00000004 /* 8-bit CRC16 + 8-bit
                                                          XOR1. */
#define BCM_HASH_FIELD_CONFIG_CRC16         0x00000005 /* 16-bit CRC16. */
#define BCM_HASH_FIELD_CONFIG_XOR16         0x00000006 /* 16-bit XOR. */
#define BCM_HASH_FIELD_CONFIG_CRC16CCITT    0x00000007 /* 16-bit CRC using CCITT
                                                          polynomial. */
#define BCM_HASH_FIELD_CONFIG_CRC32LO       0x00000008 /* Lower 16-bit of CRC32. */
#define BCM_HASH_FIELD_CONFIG_CRC32HI       0x00000009 /* Higher 16-bit of
                                                          CRC32. */
#define BCM_HASH_FIELD_CONFIG_CRC32_ETH_LO  0x0000000a /* Lower 16-bit of
                                                          Ethernet CRC32. */
#define BCM_HASH_FIELD_CONFIG_CRC32_ETH_HI  0x0000000b /* Higher 16-bit of
                                                          Ethernet CRC32. */
#define BCM_HASH_FIELD_CONFIG_CRC32_KOOPMAN_LO 0x0000000c /* Lower 16-bit of
                                                          Koopman CRC32. */
#define BCM_HASH_FIELD_CONFIG_CRC32_KOOPMAN_HI 0x0000000d /* Higher 16-bit of
                                                          Koopman CRC32. */
#define BCM_HASH_FIELD_CONFIG_VERSATILE_HASH_LO 0x0000000e /* Lower 16-bit of
                                                          Versatile Hash. */
#define BCM_HASH_FIELD_CONFIG_VERSATILE_HASH_HI 0x0000000f /* Higher 16-bit of
                                                          Versatile Hash. */

/* Enhanced hash field selection disable control. */
#define BCM_HASH_FIELD0_DISABLE_IP4         0x00000001 /* Selection 0 for IPv4
                                                          packets. */
#define BCM_HASH_FIELD1_DISABLE_IP4         0x00000002 /* Selection 1 for IPv4
                                                          packets. */
#define BCM_HASH_FIELD0_DISABLE_IP6         0x00000004 /* Selection 0 for IPv6
                                                          packets. */
#define BCM_HASH_FIELD1_DISABLE_IP6         0x00000008 /* Selection 1 for IPv6
                                                          packets. */
#define BCM_HASH_FIELD0_DISABLE_MPLS        0x00000010 /* Selection 0 for MPLS
                                                          packets. */
#define BCM_HASH_FIELD1_DISABLE_MPLS        0x00000020 /* Selection 1 for MPLS
                                                          packets. */
#define BCM_HASH_FIELD0_DISABLE_TUNNEL_IP4_IP 0x00000040 /* Selection 0 for tunnel
                                                          IPv4 over IPv4
                                                          packets. */
#define BCM_HASH_FIELD1_DISABLE_TUNNEL_IP4_IP 0x00000080 /* Selection 1 for tunnel
                                                          IPv4 over IPv4
                                                          packets. */
#define BCM_HASH_FIELD0_DISABLE_TUNNEL_IP6_IP 0x00000100 /* Selection 0 for tunnel
                                                          IPv6 over IPv4
                                                          packets. */
#define BCM_HASH_FIELD1_DISABLE_TUNNEL_IP6_IP 0x00000200 /* Selection 1 for tunnel
                                                          IPv6 over IPv4
                                                          packets. */
#define BCM_HASH_FIELD0_DISABLE_TUNNEL_IP4_GRE 0x00000400 /* Selection 0 for tunnel
                                                          IPv4 over GRE packets. */
#define BCM_HASH_FIELD1_DISABLE_TUNNEL_IP4_GRE 0x00000800 /* Selection 1 for tunnel
                                                          IPv4 over GRE packets. */
#define BCM_HASH_FIELD0_DISABLE_TUNNEL_IP6_GRE 0x00001000 /* Selection 0 for tunnel
                                                          IPv6 over GRE packets. */
#define BCM_HASH_FIELD1_DISABLE_TUNNEL_IP6_GRE 0x00002000 /* Selection 1 for tunnel
                                                          IPv6 over GRE packets. */
#define BCM_HASH_FIELD0_DISABLE_TUNNEL_IP4_IP6 0x00004000 /* Selection 0 for tunnel
                                                          IPv4 over IPv6
                                                          packets. */
#define BCM_HASH_FIELD1_DISABLE_TUNNEL_IP4_IP6 0x00008000 /* Selection 1 for tunnel
                                                          IPv4 over IPv6
                                                          packets. */
#define BCM_HASH_FIELD0_DISABLE_TUNNEL_IP6_IP6 0x00010000 /* Selection 0 for tunnel
                                                          IPv6 over IPv6
                                                          packets. */
#define BCM_HASH_FIELD1_DISABLE_TUNNEL_IP6_IP6 0x00020000 /* Selection 1 for tunnel
                                                          IPv6 over IPv6
                                                          packets. */
#define BCM_HASH_FIELD0_DISABLE_MIM         0x00040000 /* Selection 0 for MIM
                                                          packets. */
#define BCM_HASH_FIELD1_DISABLE_MIM         0x00080000 /* Selection 1 for MIM
                                                          packets. */
#define BCM_HASH_FIELD0_DISABLE_FCOE        0x00100000 /* Selection 0 for FCoE
                                                          packets. */
#define BCM_HASH_FIELD1_DISABLE_FCOE        0x00200000 /* Selection 1 for FCoE
                                                          packets. */
#define BCM_HASH_FIELD0_DISABLE_TUNNEL_IP4_GRE_IP4 0x00400000 /* Selection 0 for tunnel
                                                          IPv4 over GRE IPv4
                                                          packets. */
#define BCM_HASH_FIELD0_DISABLE_TUNNEL_IP4_GRE_IP6 0x00800000 /* Selection 0 for tunnel
                                                          IPv4 over GRE IPv6
                                                          packets. */
#define BCM_HASH_FIELD1_DISABLE_TUNNEL_IP4_GRE_IP4 0x01000000 /* Selection 1 for tunnel
                                                          IPv4 over GRE IPv4
                                                          packets. */
#define BCM_HASH_FIELD1_DISABLE_TUNNEL_IP4_GRE_IP6 0x02000000 /* Selection 1 for tunnel
                                                          IPv4 over GRE IPv6
                                                          packets. */
#define BCM_HASH_FIELD0_DISABLE_TUNNEL_IP6_GRE_IP4 0x04000000 /* Selection 0 for tunnel
                                                          IPv6 over GRE IPv4
                                                          packets. */
#define BCM_HASH_FIELD0_DISABLE_TUNNEL_IP6_GRE_IP6 0x08000000 /* Selection 0 for tunnel
                                                          IPv6 over GRE IPv6
                                                          packets. */
#define BCM_HASH_FIELD1_DISABLE_TUNNEL_IP6_GRE_IP4 0x10000000 /* Selection 1 for tunnel
                                                          IPv6 over GRE IPv4
                                                          packets. */
#define BCM_HASH_FIELD1_DISABLE_TUNNEL_IP6_GRE_IP6 0x20000000 /* Selection 1 for tunnel
                                                          IPv6 over GRE IPv6
                                                          packets. */
#define BCM_HASH_FIELD0_DISABLE_VXLAN       0x40000000 /* Selection 0 for tunnel
                                                          VXLAN packets. */
#define BCM_HASH_FIELD1_DISABLE_VXLAN       0x80000000 /* Selection 1 for tunnel
                                                          VXLAN packets. */

/* 
 * Extended flags for bcmSwitchHashSelectControlExtended switch control
 * type to disable enhanced hash field selection control.
 */
#define BCM_HASH_FIELD0_DISABLE_ROCE1       0x00000001 /* Hash block A field
                                                          selection disable for
                                                          RoCEv1 packet. */
#define BCM_HASH_FIELD1_DISABLE_ROCE1       0x00000002 /* Hash block B field
                                                          selection disable for
                                                          RoCEv1 packet. */
#define BCM_HASH_FIELD0_DISABLE_ROCE2_IP4   0x00000004 /* Hash block A field
                                                          selection disable for
                                                          RoCEv2 IPv4 packet. */
#define BCM_HASH_FIELD1_DISABLE_ROCE2_IP4   0x00000008 /* Hash block B field
                                                          selection disable for
                                                          RoCEv2 IPv4 packet. */
#define BCM_HASH_FIELD0_DISABLE_ROCE2_IP6   0x00000010 /* Hash block A field
                                                          selection disable for
                                                          RoCEv2 IPv6 packet. */
#define BCM_HASH_FIELD1_DISABLE_ROCE2_IP6   0x00000020 /* Hash block B field
                                                          selection disable for
                                                          RoCEv2 IPv6 packet. */

/* UDF hashing select. */
#define BCM_HASH_FIELD0_ENABLE_UDFHASH  0x00000001 /* UDF hashing enable on
                                                      engine 0. */
#define BCM_HASH_FIELD1_ENABLE_UDFHASH  0x00000002 /* UDF hashing enable on
                                                      engine 1. */

/* Enhanced hash field selection. */
#define BCM_HASH_FIELD_DSTMOD           0x00000001 /* Destination module ID. */
#define BCM_HASH_NON_SYMMETRICAL        BCM_HASH_FIELD_DSTMOD /* Non symmetrical hashing. */
#define BCM_HASH_FIELD_DSTPORT          0x00000002 /* Destination port ID. */
#define BCM_HASH_FIELD_SRCMOD           0x00000004 /* Source modle ID. */
#define BCM_HASH_FIELD_SRCPORT          0x00000008 /* Souce port ID. */
#define BCM_HASH_FIELD_PROTOCOL         0x00000010 /* Protocol ID. */
#define BCM_HASH_FIELD_DSTL4            0x00000020 /* Destination L4 port. */
#define BCM_HASH_FIELD_SRCL4            0x00000040 /* Source L4 port. */
#define BCM_HASH_FIELD_L4               (BCM_HASH_FIELD_SRCL4 | BCM_HASH_FIELD_DSTL4) /* The entire L4 field. */
#define BCM_HASH_FIELD_VLAN             0x00000080 /* VLAN ID. */
#define BCM_HASH_FIELD_IP4DST_LO        0x00000100 /* IPv4 destination address
                                                      lower 16 bits. */
#define BCM_HASH_FIELD_IP4DST_HI        0x00000200 /* IPv4 destination address
                                                      upper 16 bits. */
#define BCM_HASH_FIELD_IP4SRC_LO        0x00000400 /* IPv4 source address lower
                                                      16 bits. */
#define BCM_HASH_FIELD_IP4SRC_HI        0x00000800 /* IPv4 source address upper
                                                      16 bits. */
#define BCM_HASH_FIELD_IPV4_ADDRESS     (BCM_HASH_FIELD_IP4DST_LO | BCM_HASH_FIELD_IP4DST_HI | BCM_HASH_FIELD_IP4SRC_LO | BCM_HASH_FIELD_IP4SRC_HI) /* The entire IPv4 address
                                                      field including the source
                                                      and the destination
                                                      addresses. */
#define BCM_HASH_FIELD_IP6DST_LO        0x00001000 /* IPv6 collapsed destination
                                                      address lower 16 bits. */
#define BCM_HASH_FIELD_IP6DST_HI        0x00002000 /* IPv6 collapsed destination
                                                      address upper 16 bits. */
#define BCM_HASH_FIELD_IP6SRC_LO        0x00004000 /* IPv6 collapsed source
                                                      address lower 16 bits. */
#define BCM_HASH_FIELD_IP6SRC_HI        0x00008000 /* IPv6 collapsed source
                                                      address upper 16 bits. */
#define BCM_HASH_FIELD_IPV6_ADDRESS     (BCM_HASH_FIELD_IP6DST_LO | BCM_HASH_FIELD_IP6DST_HI | BCM_HASH_FIELD_IP6SRC_LO | BCM_HASH_FIELD_IP6SRC_HI) /* The entire IPv6 address
                                                      field including the source
                                                      and the destination
                                                      addresses. */
#define BCM_HASH_FIELD_FLOWLABEL_LO     BCM_HASH_FIELD_DSTMOD /* IPv6 flow label lower 16
                                                      bits. */
#define BCM_HASH_FIELD_FLOWLABEL_HI     BCM_HASH_FIELD_DSTPORT /* IPv6 flow label upper 4
                                                      bits. */
#define BCM_HASH_FIELD_FLOW_LABEL       (BCM_HASH_FIELD_FLOWLABEL_LO | BCM_HASH_FIELD_FLOWLABEL_HI) /* IPv6 flow label. */
#define BCM_HASH_FIELD_NXT_HDR          0x00010000 /* Next header. */
#define BCM_HASH_FIELD_ETHER_TYPE       0x00020000 /* Ether type. */
#define BCM_HASH_FIELD_MACDA_LO         0x00040000 /* MAC destination address
                                                      lower 16 bits. */
#define BCM_HASH_FIELD_MACDA_MI         0x00080000 /* MAC destination address
                                                      middle 16 bits. */
#define BCM_HASH_FIELD_MACDA_HI         0x00100000 /* MAC destination address
                                                      upper 16 bits. */
#define BCM_HASH_FIELD_MACSA_LO         0x00200000 /* MAC source address lower
                                                      16 bits. */
#define BCM_HASH_FIELD_MACSA_MI         0x00400000 /* MAC source address middle
                                                      16 bits. */
#define BCM_HASH_FIELD_MACSA_HI         0x00800000 /* MAC source address upper
                                                      16 bits. */
#define BCM_HASH_FIELD_MAC_ADDRESS      (BCM_HASH_FIELD_MACDA_LO | BCM_HASH_FIELD_MACDA_MI | BCM_HASH_FIELD_MACDA_HI | BCM_HASH_FIELD_MACSA_LO | BCM_HASH_FIELD_MACSA_MI | BCM_HASH_FIELD_MACSA_HI) /* MAC source and destination
                                                      address. */
#define BCM_HASH_FIELD_LOAD_BALANCE     0x01000000 /* Load balance ID - HiGig2
                                                      only. */
#define BCM_HASH_FIELD_RSVD_LABELS      0x02000000 /* Reserved second label, top
                                                      label. */
#define BCM_HASH_FIELD_2ND_LABEL        0x04000000 /* Second label. */
#define BCM_HASH_FIELD_TOP_LABEL        0x08000000 /* Top label. */
#define BCM_HASH_FIELD_LOOKUP_ID_LO     0x10000000 /* Used for ISID[15:0]. */
#define BCM_HASH_FIELD_LOOKUP_ID_HI     0x20000000 /* Used for ISID[23:16]. */
#define BCM_HASH_FIELD_CNTAG_FLOW_ID    0x40000000 /* CNTAG flow ID. */
#define BCM_HASH_MPLS_ALL_LABELS        0x80000000 /* All the MPLS labels. */

/* Enhanced field selection bit map for RoCE hash field selection. */
#define BCM_HASH_ROCE_FIELD_DSTMOD          BCM_HASH_FIELD_DSTMOD /* Destination module ID. */
#define BCM_HASH_ROCE_FIELD_DSTPORT         BCM_HASH_FIELD_DSTPORT /* Destination port ID. */
#define BCM_HASH_ROCE_FIELD_SRCMOD          BCM_HASH_FIELD_SRCMOD /* Source modle ID. */
#define BCM_HASH_ROCE_FIELD_SRCPORT         BCM_HASH_FIELD_SRCPORT /* Souce port ID. */
#define BCM_HASH_ROCE_FIELD_FLOWLABEL_LO    BCM_HASH_FIELD_DSTMOD /* RoCE flow label lower
                                                          16 bits. */
#define BCM_HASH_ROCE_FIELD_FLOWLABEL_HI    BCM_HASH_FIELD_DSTPORT /* RoCE flow label upper
                                                          4 bits. */
#define BCM_HASH_ROCE_FIELD_DSTL4           BCM_HASH_FIELD_DSTL4 /* RoCEv2 L4 destination
                                                          port. */
#define BCM_HASH_ROCE_FIELD_SRCL4           BCM_HASH_FIELD_SRCL4 /* RoCEv2 L4 source port. */
#define BCM_HASH_ROCE_FIELD_VLAN            BCM_HASH_FIELD_VLAN /* VLAN ID. */
#define BCM_HASH_ROCE_FIELD_MACDA_LO        BCM_HASH_FIELD_MACDA_LO /* RoCEv1 MAC destination
                                                          address lower 16 bits. */
#define BCM_HASH_ROCE_FIELD_MACDA_MI        BCM_HASH_FIELD_MACDA_MI /* RoCEv1 MAC destination
                                                          address middle 16
                                                          bits. */
#define BCM_HASH_ROCE_FIELD_MACDA_HI        BCM_HASH_FIELD_MACDA_HI /* RoCEv1 MAC destination
                                                          address upper 16 bits. */
#define BCM_HASH_ROCE_FIELD_MACSA_LO        BCM_HASH_FIELD_MACSA_LO /* RoCEv1 MAC source
                                                          address lower 16 bits. */
#define BCM_HASH_ROCE_FIELD_MACSA_MI        BCM_HASH_FIELD_MACSA_MI /* RoCEv1 MAC source
                                                          address middle 16
                                                          bits. */
#define BCM_HASH_ROCE_FIELD_MACSA_HI        BCM_HASH_FIELD_MACSA_HI /* RoCEv1 MAC source
                                                          address upper16 bits. */
#define BCM_HASH_ROCE_FIELD_IP4DST_LO       BCM_HASH_FIELD_IP4DST_LO /* RoCEv2 IPv4
                                                          destination address
                                                          lower 16 bits. */
#define BCM_HASH_ROCE_FIELD_IP4DST_HI       BCM_HASH_FIELD_IP4DST_HI /* RoCEv2 IPv4
                                                          destination address
                                                          upper 16 bits. */
#define BCM_HASH_ROCE_FIELD_IP4SRC_LO       BCM_HASH_FIELD_IP4SRC_LO /* RoCEv2 IPv4 source
                                                          address lower 16 bits. */
#define BCM_HASH_ROCE_FIELD_IP4SRC_HI       BCM_HASH_FIELD_IP4SRC_HI /* RoCEv2 IPv4 source
                                                          address upper 16 bits. */
#define BCM_HASH_ROCE_FIELD_IP6DST_LO       BCM_HASH_FIELD_IP6DST_LO /* RoCEv2 IPv6 collapsed
                                                          destination address
                                                          lower 16 bits. */
#define BCM_HASH_ROCE_FIELD_IP6DST_HI       BCM_HASH_FIELD_IP6DST_HI /* RoCEv2 IPv6 collapsed
                                                          destination address
                                                          upper 16 bits. */
#define BCM_HASH_ROCE_FIELD_IP6SRC_LO       BCM_HASH_FIELD_IP6SRC_LO /* RoCEv2 IPv6 collapsed
                                                          source address lower
                                                          16 bits. */
#define BCM_HASH_ROCE_FIELD_IP6SRC_HI       BCM_HASH_FIELD_IP6SRC_HI /* RoCEv2 IPv6 collapsed
                                                          source address upper
                                                          16 bits. */
#define BCM_HASH_ROCE_FIELD_BTH_DSTQP_COLLAPSED 0x00010000 /* RoCE Collapsed
                                                          Destination Queue Pair
                                                          in BTH (Base Transport
                                                          Header). */
#define BCM_HASH_ROCE_FIELD_BTH_PARTITION_KEY 0x00020000 /* RoCE Partition Key in
                                                          BTH (Base Transport
                                                          Header). */

/* Enhanced hash MPLS field selection. */
#define BCM_HASH_MPLS_FIELD_DSTMOD          BCM_HASH_FIELD_DSTMOD /* Destination module ID. */
#define BCM_HASH_MPLS_FIELD_DSTPORT         BCM_HASH_FIELD_DSTPORT /* Destination port ID. */
#define BCM_HASH_MPLS_FIELD_SRCMOD          BCM_HASH_FIELD_SRCMOD /* Source modle ID. */
#define BCM_HASH_MPLS_FIELD_SRCPORT         BCM_HASH_FIELD_SRCPORT /* Souce port ID. */
#define BCM_HASH_MPLS_FIELD_3RD_LABEL       0x00000010 /* Third label. */
#define BCM_HASH_MPLS_FIELD_LABELS_4MSB     0x00000020 /* Most significant 4
                                                          bits of top, second,
                                                          and third label. */
#define BCM_HASH_MPLS_FIELD_IP4DST_LO       BCM_HASH_FIELD_IP4DST_LO /* IPv4 destination
                                                          address lower 16 bits. */
#define BCM_HASH_MPLS_FIELD_IP4DST_HI       BCM_HASH_FIELD_IP4DST_HI /* IPv4 destination
                                                          address upper 16 bits. */
#define BCM_HASH_MPLS_FIELD_IP4SRC_LO       BCM_HASH_FIELD_IP4SRC_LO /* IPv4 source address
                                                          lower 16 bits. */
#define BCM_HASH_MPLS_FIELD_IP4SRC_HI       BCM_HASH_FIELD_IP4SRC_HI /* IPv4 source address
                                                          upper 16 bits. */
#define BCM_HASH_MPLS_FIELD_3RD_LABEL_4MSB_5TH_LABEL BCM_HASH_FIELD_IP6DST_LO /* Third label or 4 most
                                                          significant bits of
                                                          5th label. */
#define BCM_HASH_MPLS_FIELD_LABELS_4MSB_3RD_LABEL BCM_HASH_FIELD_IP6DST_HI /* Most significant 4
                                                          bits of top, second,
                                                          and third label or 3rd
                                                          label alone. */
#define BCM_HASH_MPLS_FIELD_IP4DST_IP6DST_LO_5TH_LABEL BCM_HASH_FIELD_IP6SRC_LO /* IPv4 destination
                                                          address lower 16 bits
                                                          or IPv6 collapsed
                                                          destination address
                                                          lower 16 bits or 5th
                                                          label. */
#define BCM_HASH_MPLS_FIELD_IP4DST_IP6DST_HI_4TH_LABEL BCM_HASH_FIELD_IP6SRC_HI /* IPv4 destination
                                                          address upper 16 bits
                                                          or IPv6 collapsed
                                                          destination address
                                                          upper 16 bits or 4th
                                                          label. */
#define BCM_HASH_MPLS_FIELD_IP4SRC_IP6SRC_LO_4MSB_4TH_LABEL BCM_HASH_FIELD_NXT_HDR /* IPv4 source address
                                                          lower 16 bits or IPv6
                                                          collapsed source
                                                          address lower 16 bits
                                                          or 4 most significant
                                                          bits of 4th label. */
#define BCM_HASH_MPLS_FIELD_IP4SRC_IP6SRC_HI_4MSB BCM_HASH_FIELD_ETHER_TYPE /* IPv4 source address
                                                          upper 16 bits or IPv6
                                                          collapsed source
                                                          address upper 16 bits
                                                          or 4 most significant
                                                          bits of 1st,2nd,3rd
                                                          labels. */
#define BCM_HASH_MPLS_FIELD_2ND_LABEL       BCM_HASH_FIELD_2ND_LABEL /* Second label. */
#define BCM_HASH_MPLS_FIELD_TOP_LABEL       BCM_HASH_FIELD_TOP_LABEL /* Top label. */
#define BCM_HASH_MPLS_FIELD_CNTAG_FLOW_ID   BCM_HASH_FIELD_CNTAG_FLOW_ID /* CNTAG flow ID. */

/* Enhanced hash FCOE field selection. */
#define BCM_HASH_FCOE_FIELD_DSTMOD          BCM_HASH_FIELD_DSTMOD /* Destination module ID. */
#define BCM_HASH_FCOE_FIELD_DSTPORT         BCM_HASH_FIELD_DSTPORT /* Destination port ID. */
#define BCM_HASH_FCOE_FIELD_SRCMOD          BCM_HASH_FIELD_SRCMOD /* Source modle ID. */
#define BCM_HASH_FCOE_FIELD_SRCPORT         BCM_HASH_FIELD_SRCPORT /* Souce port ID. */
#define BCM_HASH_FCOE_FIELD_VLAN            0x00000010 /* VLAN ID. */
#define BCM_HASH_FCOE_FIELD_VIRTUAL_FABRIC_ID 0x00000020 /* Virtual Fabric ID. */
#define BCM_HASH_FCOE_FIELD_RESPONDER_EXCHANGE_ID 0x00000040 /* Responder Exchange_ID. */
#define BCM_HASH_FCOE_FIELD_ORIGINATOR_EXCHANGE_ID 0x00000080 /* Originator
                                                          Exchange_ID. */
#define BCM_HASH_FCOE_FIELD_DST_ID_LO       0x00000100 /* Destination_ID lower
                                                          16 bits. */
#define BCM_HASH_FCOE_FIELD_DST_ID_HI       0x00000200 /* Destination_ID upper
                                                          16 bits. */
#define BCM_HASH_FCOE_FIELD_SRC_ID_LO       0x00000400 /* Source_ID lower 16
                                                          bits. */
#define BCM_HASH_FCOE_FIELD_SRC_ID_HI       0x00000800 /* Source_ID upper 16
                                                          bits. */
#define BCM_HASH_FCOE_FIELD_CNTAG_FLOW_ID   BCM_HASH_FIELD_CNTAG_FLOW_ID /* CNTAG flow ID. */

/* Enhanced hash Trill field selection. */
#define BCM_HASH_TRILL_FIELD_DSTMOD         BCM_HASH_FIELD_DSTMOD /* Destination module ID. */
#define BCM_HASH_TRILL_FIELD_DSTPORT        BCM_HASH_FIELD_DSTPORT /* Destination port ID. */
#define BCM_HASH_TRILL_FIELD_SRCMOD         BCM_HASH_FIELD_SRCMOD /* Source modle ID. */
#define BCM_HASH_TRILL_FIELD_SRCPORT        BCM_HASH_FIELD_SRCPORT /* Souce port ID. */
#define BCM_HASH_TRILL_FIELD_VLAN           0x00000010 /* VLAN ID. */
#define BCM_HASH_TRILL_FIELD_ETHER_TYPE     0x00000020 /* Ether type. */
#define BCM_HASH_TRILL_FIELD_MACDA_LO       0x00000040 /* MAC destination
                                                          address lower 16 bits. */
#define BCM_HASH_TRILL_FIELD_MACDA_MI       0x00000080 /* MAC destination
                                                          address middle 16
                                                          bits. */
#define BCM_HASH_TRILL_FIELD_MACSA_LO       0x00000100 /* MAC source address
                                                          lower 16 bits. */
#define BCM_HASH_TRILL_FIELD_MACSA_MI       0x00000200 /* MAC source address
                                                          middle 16 bits. */
#define BCM_HASH_TRILL_FIELD_ING_RBRIDGE_NAME 0x00000400 /* Ingress RBridge
                                                          Nickname. */
#define BCM_HASH_TRILL_FIELD_EGR_RBRIDGE_NAME 0x00000800 /* Egress RBridge
                                                          Nickname. */

/* Stacking mode select. */
#define BCM_MIRROR_STACK_MODE_XGS3  0          /* Stacking with BCM5650x
                                                  (BCM5610x, BCM5630x). */
#define BCM_MIRROR_STACK_MODE_XGS1  1          /* Stacking with BCM5670. */
#define BCM_MIRROR_STACK_MODE_XGS2  2          /* Stacking with BCM5675-A0. */

#define BCM_COLOR_PRIORITY      0          /* Priority->COLOR mapping. */
#define BCM_COLOR_OUTER_CFI     1          /* SPVLAN CFI->COLOR mapping. */
#define BCM_COLOR_INNER_CFI     2          /* CVLAN CFI->COLOR mapping. */

#define BCM_HASH_MPLS_LABEL_NONE        0          /* Normal processing for MPLS
                                                      label stack, i.e. hash
                                                      according to forwarding
                                                      header configuration. */
#define BCM_HASH_MPLS_LABEL_BOS_START   1          /* Hashing from BOS label,
                                                      skipping all other label. */
#define BCM_HASH_MPLS_LABEL_BOS_SKIP    2          /* Hashing from header next
                                                      to BOS label, skipping all
                                                      MPLS labels. */

/* IGM/MLD Action select. */
#define BCM_SWITCH_FORWARD      0x00000001 
#define BCM_SWITCH_FLOOD        0x00000002 
#define BCM_SWITCH_DROP         0x00000004 
#define BCM_SWITCH_CPU          0x00000008 

/* Switch features bypass mode */
#define BCM_SWITCH_BYPASS_NONE      0          /* No switch logic bypassed. */
#define BCM_SWITCH_BYPASS_L3_ONLY   1          /* L3 switch logic bypassed. */
#define BCM_SWITCH_BYPASS_L3_AND_FP 2          /* L3 and FP switch logic
                                                  bypassed. */

/* Switch Remote CPU packet encapsulation priority mapping options. */
#define BCM_SWITCH_REMOTE_CPU_ENCAP_IEEE    0x00000001 /* Use 802.1P Priority
                                                          mapping for Remote CPU
                                                          packet encapsulation. */
#define BCM_SWITCH_REMOTE_CPU_ENCAP_HIGIG2  0x00000002 /* Use module header
                                                          traffic class mapping
                                                          for Remote CPU packet
                                                          encapsulation. */
#define BCM_SWITCH_REMOTE_CPU_ENCAP_IEEE_CPU_TC 0x00000004 /* Use status header
                                                          traffic class mapping
                                                          for Remote CPU packet
                                                          encapsulation. */

/* Switch features filter bypass control modes */
#define BCM_FILTER_NONE             _SHR_SWITCH_FILTER_NONE /* No switch filter bypass. */
#define BCM_FILTER_RX_CRCCHK        _SHR_SWITCH_FILTER_RX_CRCCHK /* RX filter bypass CRC check. */
#define BCM_FILTER_DOS_TOCPU        _SHR_SWITCH_FILTER_DOS_TOCPU /* DOS filter bypass at the frame
                                                  to CPU. */
#define BCM_FILTER_STP_USERADDR     _SHR_SWITCH_FILTER_STP_USERADDR /* STP filter bypass L2 User
                                                  address. */
#define BCM_FILTER_STP_MAC0X        _SHR_SWITCH_FILTER_STP_MAC0X /* STP filter bypass the frame at
                                                  MAC=0180c2-00000x. */
#define BCM_FILTER_VLAN_IGMP_MLD    _SHR_SWITCH_FILTER_VLAN_IGMP_MLD /* VLAN filter bypass IGMP and
                                                  MLD. */
#define BCM_FILTER_VLAN_ARP_DHCP    _SHR_SWITCH_FILTER_VLAN_ARP_DHCP /* VLAN filter bypass ARP and
                                                  DHCP */
#define BCM_FILTER_VLAN_MIIM        _SHR_SWITCH_FILTER_VLAN_MIIM /* VLAN filter bypass IMP ingress
                                                  frame. */
#define BCM_FILTER_VLAN_MCAST       _SHR_SWITCH_FILTER_VLAN_MCAST /* VLAN filter bypass MCAST
                                                  frame. */
#define BCM_FILTER_VLAN_RSV_MCAST   _SHR_SWITCH_FILTER_VLAN_RSV_MCAST /* VLAN filter bypass Reserved
                                                  MCAST frame. */
#define BCM_FILTER_VLAN_USERADDR    _SHR_SWITCH_FILTER_VLAN_USERADDR /* VLAN filter bypass L2 User
                                                  address. */
#define BCM_FILTER_EAP_USERADDR     _SHR_SWITCH_FILTER_EAP_USERADDR /* EAP filter bypass L2 User
                                                  address. */
#define BCM_FILTER_EAP_DHCP         _SHR_SWITCH_FILTER_EAP_DHCP /* EAP filter bypass DHCP. */
#define BCM_FILTER_EAP_ARP          _SHR_SWITCH_FILTER_EAP_ARP /* EAP filter bypass EAP and ARP. */
#define BCM_FILTER_EAP_MAC_22_2F    _SHR_SWITCH_FILTER_EAP_MAC_22_2F /* EAP filter bypass the frame at
                                                  MAC=0180c2-000021 to
                                                  0180c2-00002F. */
#define BCM_FILTER_EAP_MAC_21       _SHR_SWITCH_FILTER_EAP_MAC_21 /* EAP filter bypass the frame at
                                                  MAC=0180c2-000021. */
#define BCM_FILTER_EAP_MAC_20       _SHR_SWITCH_FILTER_EAP_MAC_20 /* EAP filter bypass the frame at
                                                  MAC=0180c2-000020. */
#define BCM_FILTER_EAP_MAC_11_1F    _SHR_SWITCH_FILTER_EAP_MAC_11_1F /* EAP filter bypass the frame at
                                                  MAC=0180c2-000011 to
                                                  0180c2-00001F. */
#define BCM_FILTER_EAP_MAC_10       _SHR_SWITCH_FILTER_EAP_MAC_10 /* EAP filter bypass the frame at
                                                  MAC=0180c2-000010. */
#define BCM_FILTER_EAP_MAC_0X       _SHR_SWITCH_FILTER_EAP_MAC_0X /* EAP filter bypass the frame at
                                                  MAC=0180c2-00000x. */
#define BCM_FILTER_EAP_BPDU         _SHR_SWITCH_FILTER_EAP_BPDU /* EAP filter bypass BPDU frame. */
#define BCM_FILTER_CONTROL_COUNT    _SHR_SWITCH_FILTER_CONTROL_COUNT /* The count of filter bypass
                                                  control items. */

/* LED functions group */
#define BCM_LED_FUNCGRP_0       _SHR_SWITCH_LED_FUNCGRP_0 
#define BCM_LED_FUNCGRP_1       _SHR_SWITCH_LED_FUNCGRP_1 

/* LED function items */
#define BCM_LED_FUNC_NONE           _SHR_SWITCH_LED_FUNC_NONE 
#define BCM_LED_FUNC_PHYLED4        _SHR_SWITCH_LED_FUNC_PHYLED4 
#define BCM_LED_FUNC_LNK            _SHR_SWITCH_LED_FUNC_LNK 
#define BCM_LED_FUNC_DPX            _SHR_SWITCH_LED_FUNC_DPX 
#define BCM_LED_FUNC_ACT            _SHR_SWITCH_LED_FUNC_ACT 
#define BCM_LED_FUNC_COL            _SHR_SWITCH_LED_FUNC_COL 
#define BCM_LED_FUNC_LINK_ACT       _SHR_SWITCH_LED_FUNC_LINK_ACT 
#define BCM_LED_FUNC_DPX_COL        _SHR_SWITCH_LED_FUNC_DPX_COL 
#define BCM_LED_FUNC_SP_10          _SHR_SWITCH_LED_FUNC_SP_10 
#define BCM_LED_FUNC_SP_100         _SHR_SWITCH_LED_FUNC_SP_100 
#define BCM_LED_FUNC_SP_1G          _SHR_SWITCH_LED_FUNC_SP_1G 
#define BCM_LED_FUNC_10_ACT         _SHR_SWITCH_LED_FUNC_10_ACT 
#define BCM_LED_FUNC_100_ACT        _SHR_SWITCH_FILTER_EAP_DHCP 
#define BCM_LED_FUNC_10_100_ACT     _SHR_SWITCH_LED_FUNC_10_100_ACT 
#define BCM_LED_FUNC_1G_ACT         _SHR_SWITCH_LED_FUNC_1G_ACT 
#define BCM_LED_FUNC_EAV_LINK       _SHR_SWITCH_LED_FUNC_EAV_LINK 
#define BCM_LED_FUNC_PHYLED3        _SHR_SWITCH_LED_FUNC_PHYLED3 
#define BCM_LED_FUNC_SP_100_200     _SHR_SWITCH_LED_FUNC_SP_100_200 
#define BCM_LED_FUNC_100_200_ACT    _SHR_SWITCH_LED_FUNC_100_200_ACT 
#define BCM_LED_FUNC_SP_LNK_ACT_SP  _SHR_SWITCH_LED_FUNC_LNK_ACT_SP 

/* LED working mode */
#define BCM_LED_MODE_OFF        _SHR_SWITCH_LED_MODE_OFF 
#define BCM_LED_MODE_ON         _SHR_SWITCH_LED_MODE_ON 
#define BCM_LED_MODE_BLINK      _SHR_SWITCH_LED_MODE_BLINK 
#define BCM_LED_MODE_AUTO       _SHR_SWITCH_LED_MODE_AUTO 

/* types for bcmSwitchL2LearnMode */
#define BCM_L2_INGRESS_CENT         0x1        /* Ingress and Centralized
                                                  Learning */
#define BCM_L2_INGRESS_DIST         0x2        /* Ingress and Centralized
                                                  Learning */
#define BCM_L2_EGRESS_DIST          0x4        /* Egress and Distributed
                                                  Learning */
#define BCM_L2_EGRESS_CENT          0x8        /* Egress and Centralized
                                                  Learning */
#define BCM_L2_EGRESS_INDEPENDENT   0x10       /* Egress and Independent
                                                  Learning */
#define BCM_L2_LEARN_CPU            0x20       /* Learning events handled by CPU */
#define BCM_L2_LEARN_DISABLE        0x40       /* Disabled learning */

/* Hashing function enumeration. */
typedef enum bcm_switch_hash_config_e {
    BCM_HASH_CONFIG_NONE = 1,           /* Indication to not hash */
    BCM_HASH_CONFIG_CRC8_0x14D = 2,     /* CRC8 with 0x14D polynomial */
    BCM_HASH_CONFIG_CRC8_0x1C3 = 3,     /* CRC8 with 0x1C3 polynomial */
    BCM_HASH_CONFIG_CRC8_0x1CF = 4,     /* CRC8 with 0x1CF polynomial */
    BCM_HASH_CONFIG_CRC16_0x17111 = 5,  /* CRC16 with 0x17111 polynomial */
    BCM_HASH_CONFIG_CRC16_0x10491 = 6,  /* CRC16 with 0x10491 polynomial */
    BCM_HASH_CONFIG_CRC16_0x155F5 = 7,  /* CRC16 with 0x155F5 polynomial */
    BCM_HASH_CONFIG_CRC16_0x1013D = 8,  /* CRC16 with 0x1013D polynomial */
    BCM_HASH_CONFIG_CRC16_0x13965 = 9,  /* CRC16 with 0x13965 polynomial */
    BCM_HASH_CONFIG_CRC16_0x1698D = 10, /* CRC16 with 0x1698D polynomial */
    BCM_HASH_CONFIG_CRC16_0x1105D = 11, /* CRC16 with 0x1105D polynomial */
    BCM_HASH_CONFIG_CRC16_0x10861 = 12, /* CRC16 with 0x10861 polynomial */
    BCM_HASH_CONFIG_CRC16_0x10285 = 13, /* CRC16 with 0x10285 polynomial */
    BCM_HASH_CONFIG_CRC16_0x101a1 = 14, /* CRC16 with 0x101a1 polynomial */
    BCM_HASH_CONFIG_CRC16_0x12499 = 15, /* CRC16 with 0x12499 polynomial */
    BCM_HASH_CONFIG_CRC16_0x1f801 = 16, /* CRC16 with 0x1f801 polynomial */
    BCM_HASH_CONFIG_CRC16_0x172e1 = 17, /* CRC16 with 0x172e1 polynomial */
    BCM_HASH_CONFIG_CRC16_0x1eb21 = 18, /* CRC16 with 0x1eb21 polynomial */
    BCM_HASH_CONFIG_FP_DATA = 19,       /* Use only data calculated by FP */
    BCM_HASH_CONFIG_ROUND_ROBIN = 20,   /* Round robin between ports per packet */
    BCM_HASH_CONFIG_CLOCK_BASED = 21,   /* Result based in clock (random) */
    BCM_HASH_CONFIG_CRC16_0x8003 = 22,  /* CRC16 with 0x8003 polynomial */
    BCM_HASH_CONFIG_CRC16_0x8011 = 23,  /* CRC16 with 0x8011 polynomial */
    BCM_HASH_CONFIG_CRC16_0x8423 = 24,  /* CRC16 with 0x8423 polynomial */
    BCM_HASH_CONFIG_CRC16_0x8101 = 25,  /* CRC16 with 0x8101 polynomial */
    BCM_HASH_CONFIG_CRC16_0x84a1 = 26,  /* CRC16 with 0x84a1 polynomial */
    BCM_HASH_CONFIG_CRC16_0x9019 = 27,  /* CRC16 with 0x9019 polynomial */
    BCM_HASH_CONFIG_CRC16_0x1015d = 28, /* CRC16 with 0x1015d polynomial */
    BCM_HASH_CONFIG_CRC16_0x100d7 = 29, /* CRC16 with 0x100d7 polynomial */
    BCM_HASH_CONFIG_CRC16_0x10039 = 30, /* CRC16 with 0x10039 polynomial */
    BCM_HASH_CONFIG_CRC16_0x10ac5 = 31, /* CRC16 with 0x10ac5 polynomial */
    BCM_HASH_CONFIG_CRC16_0x109e7 = 32, /* CRC16 with 0x109e7 polynomial */
    BCM_HASH_CONFIG_CRC16_0x10939 = 33, /* CRC16 with 0x10939 polynomial */
    BCM_HASH_CONFIG_CRC16_0x12105 = 34, /* CRC16 with 0x12105 polynomial */
    BCM_HASH_CONFIG_CRC16_0x1203d = 35, /* CRC16 with 0x1203d polynomial */
    BCM_HASH_CONFIG_COUNT               /* Must be last */
} bcm_switch_hash_config_t;

/* RTAG7 Hash Field Control selection of payload and tunnel fields. */
#define BCM_HASH_SELECT_INNER_L2        0x1        /* Select the inner L2 fields
                                                      for hash fields selection. */
#define BCM_HASH_SELECT_OUTER_L2        0x2        /* Select the outer L2 fields
                                                      for hash fields selection. */
#define BCM_HASH_SELECT_INNER_L3        0x3        /* Select the inner L3 fields
                                                      for hash fields selection. */
#define BCM_HASH_SELECT_OUTER_L3        0x4        /* Select the outer L3 fields
                                                      for hash fields selection. */
#define BCM_HASH_SELECT_TUNNEL_INNER_L2 0x5        /* Select tunnel header and
                                                      inner L2 fields for hash
                                                      fields selection. */

/* IPv6 Collapse Methods for RTAG7 Hashing */
#define BCM_HASH_IP6_COLLAPSE_XOR   0x1        /* Select XOR method to collapse
                                                  IPv6 Addresses for RTAG7
                                                  hashing in field selection
                                                  block */
#define BCM_HASH_IP6_COLLAPSE_LSB   0x2        /* Select LSB method to collapse
                                                  IPv6 Addresses for RTAG7
                                                  hashing in field selection
                                                  block */

/* IPSec selection for RTAG7 Hashing */
#define BCM_HASH_IPSEC_SELECT_OUTER_L4_PORT 0x1        /* Select OUTER
                                                          L4_SRC/DST_PORT for
                                                          RTAG7 hashing in field
                                                          selection block */
#define BCM_HASH_IPSEC_SELECT_SPI           0x2        /* Select IPv6 SPI for
                                                          RTAG7 hashing in field
                                                          selection block */

/* header selection for trunk hashing */
#define BCM_HASH_HEADER_FORWARD     0x00000001 /* Select the forward header as
                                                  starting header for hashing */
#define BCM_HASH_HEADER_TERMINATED  0x00000002 /* Select the last terminated
                                                  header as starting header for
                                                  hashing */

/* Symmetric hash field selection. */
#define BCM_SYMMETRIC_HASH_0_IP4_ENABLE     0x00000001 /* Enable symmetric key
                                                          hashing on Hash A for
                                                          IPV4 packets. */
#define BCM_SYMMETRIC_HASH_1_IP4_ENABLE     0x00000002 /* Enable symmetric key
                                                          hashing on Hash B for
                                                          IPV4 packets. */
#define BCM_SYMMETRIC_HASH_0_IP6_ENABLE     0x00000004 /* Enable symmetric key
                                                          hashing on Hash A for
                                                          IPV6 packets. */
#define BCM_SYMMETRIC_HASH_1_IP6_ENABLE     0x00000008 /* Enable symmetric key
                                                          hashing on Hash B for
                                                          IPV6 packets. */
#define BCM_SYMMETRIC_HASH_0_SUPPRESS_UNIDIR_FIELD_ENABLE 0x00000010 /* Enable setting SPI,
                                                          TEID, L2 GRE Key to
                                                          zero for Hash A
                                                          calculation. */
#define BCM_SYMMETRIC_HASH_1_SUPPRESS_UNIDIR_FIELD_ENABLE 0x00000020 /* Enable setting SPI,
                                                          TEID, L2 GRE Key to
                                                          zero for Hash B
                                                          calculation. */
#define BCM_SYMMETRIC_HASH_0_FCOE_ENABLE    0x00000040 /* Enable symmetric key
                                                          hashing on Hash A for
                                                          FCoE packets. */
#define BCM_SYMMETRIC_HASH_1_FCOE_ENABLE    0x00000080 /* Enable symmetric key
                                                          hashing on Hash B for
                                                          FCoE packets. */

/* Switch Event Control for event selection */
#define BCM_SWITCH_EVENT_CONTROL_ALL    (-1)       /* Selects All
                                                      interrupts-causes. */

/* Switch event action controls. */
typedef enum bcm_switch_event_control_action_e {
    bcmSwitchEventMask = _SHR_SWITCH_EVENT_MASK, /* Mask (disable) or unmask (enable) the
                                           event */
    bcmSwitchEventClear = _SHR_SWITCH_EVENT_CLEAR, /* Clear the event */
    bcmSwitchEventRead = _SHR_SWITCH_EVENT_READ, /* read the event */
    bcmSwitchEventForce = _SHR_SWITCH_EVENT_FORCE, /* Activate/diactivate forcing the event */
    bcmSwitchEventStormTimedCount = _SHR_SWITCH_EVENT_STORM_TIMED_COUNT, /* If more then
                                           bcmSwitchEventStormTimedCount
                                           interrupts occur in
                                           bcmSwitchEventStormTimedPeriod
                                           period, an interrupt storm is
                                           indicated (default hanling same as
                                           for bcmSwitchEventStormNominal
                                           threshold) */
    bcmSwitchEventStormTimedPeriod = _SHR_SWITCH_EVENT_STORM_TIMED_PERIOD, /* If more then
                                           bcmSwitchEventStormTimedCount
                                           interrupts occur in
                                           bcmSwitchEventStormTimedPeriod
                                           period, an interrupt storm is
                                           indicated (default hanling same as
                                           for bcmSwitchEventStormNominal
                                           threshold) */
    bcmSwitchEventStormNominal = _SHR_SWITCH_EVENT_STORM_NOMINAL, /* Nominal threshold for interrupt storm
                                           detection. If an interrupt occurs
                                           more times then the threshold, an
                                           interrupt storm is indicated. The
                                           default handling for such an
                                           interrupt is holding it in masked
                                           condition */
    bcmSwitchEventPriority = _SHR_SWITCH_EVENT_PRIORITY, /* Change handling priority in case of
                                           multiple interrupts */
    bcmSwitchEventLog = _SHR_SWITCH_EVENT_LOG, /* Activate/Diactivate log prints on
                                           event */
    bcmSwitchEventStat = _SHR_SWITCH_EVENT_STAT, /* Enable to Clear/Read event statistics */
    bcmSwitchEventCorrActOverride = _SHR_SWITCH_EVENT_CORR_ACT_OVERRIDE, /* Enable to override application
                                           callback */
    bcmSwitchEventUnmaskAndClearDisable = _SHR_SWITCH_EVENT_UNMASK_AND_CLEAR_DISABLE, /* Skip clearing and enabling the
                                           asserted interrupt at the end of
                                           event corrective action */
    bcmSwitchEventForceUnmask = _SHR_SWITCH_EVENT_FORCE_UNMASK /* Skip enable interrupt action for
                                           asserted interrupt at the end of
                                           event corrective action, if the
                                           interrupt is not cleared */
} bcm_switch_event_control_action_t;

/* Interrupt event operation */
typedef struct bcm_switch_event_control_s {
    int event_id;                       /* Event identifier, e.g. for interrupt
                                           event, device-specific enumerator per
                                           interrupt-cause. */
    int index;                          /* If multi-instance blocks represent
                                           the specific block index. */
    bcm_switch_event_control_action_t action; /* The action to perform on the
                                           indicated event. */
} bcm_switch_event_control_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set event control. */
extern int bcm_switch_event_control_set(
    int unit, 
    bcm_switch_event_t event, 
    bcm_switch_event_control_t type, 
    uint32 value);

/* Get event control. */
extern int bcm_switch_event_control_get(
    int unit, 
    bcm_switch_event_t event, 
    bcm_switch_event_control_t type, 
    uint32 *value);

#endif /* BCM_HIDE_DISPATCHABLE */

typedef enum bcm_switch_bst_tracking_mode_e {
    bcmSwitchBstTrackCurrent = 0,   /* Track the current usage. */
    bcmSwitchBstTrackPeak = 1       /* Track the peak usage. */
} bcm_switch_bst_tracking_mode_t;

/* Split Horizon network group configuration flags */
#define BCM_SWITCH_NETWORK_GROUP_MCAST_REMAP_ENABLE 0x00000001 
#define BCM_SWITCH_NETWORK_GROUP_INGRESS_PRUNE_ENABLE 0x00000002 
#define BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE 0x00000004 

/* Split Horizon destination network group configuration information */
typedef struct bcm_switch_network_group_config_s {
    bcm_switch_network_group_t dest_network_group_id; /* Split Horizon Destination Network
                                           Group Identifier */
    uint32 config_flags;                /* BCM_SWITCH_NETWORK_GROUP_XXX flags */
} bcm_switch_network_group_config_t;

/* Initialize a bcm_switch_network_group_config_t structure. */
extern void bcm_switch_network_group_config_t_init(
    bcm_switch_network_group_config_t *config);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Configures the split horizon network group configuration for a given
 * pair of source and destination network group.
 */
extern int bcm_switch_network_group_config_set(
    int unit, 
    bcm_switch_network_group_t source_network_group_id, 
    bcm_switch_network_group_config_t *config);

/* 
 * Gets the split horizon network group configuration for a given pair of
 * source and destination network group.
 */
extern int bcm_switch_network_group_config_get(
    int unit, 
    bcm_switch_network_group_t source_network_group_id, 
    bcm_switch_network_group_config_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* User buffer type. */
typedef enum bcm_switch_user_buffer_type_e {
    bcmSwitchUserBufferTypeDram = 0 /* Dram User Buffer. */
} bcm_switch_user_buffer_type_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Writes data to the user buffer */
extern int bcm_switch_user_buffer_write(
    int unit, 
    uint32 flags, 
    bcm_switch_user_buffer_type_t buff_type, 
    uint8 *buf, 
    int offset, 
    int nbytes);

/* Reads data from the user buffer */
extern int bcm_switch_user_buffer_read(
    int unit, 
    uint32 flags, 
    bcm_switch_user_buffer_type_t buff_type, 
    uint8 *buf, 
    int offset, 
    int nbytes);

#endif /* BCM_HIDE_DISPATCHABLE */

/* header selection for trunk hashing */
#define BCM_SWITCH_USER_BUFFER_LOGICAL2PHY_TRANS 0x00000001 /* User buffer flag for
                                                          using logical to
                                                          physical translation,
                                                          instead of using
                                                          logical adresses */

#ifndef BCM_HIDE_DISPATCHABLE

/* Sets Ipv6 reserved multicast address and mask of the device. */
extern int bcm_switch_ipv6_reserved_multicast_addr_set(
    int unit, 
    bcm_ip6_t ip6_addr, 
    bcm_ip6_t ip6_mask);

/* Gets Ipv6 reserved multicast address and mask of the device. */
extern int bcm_switch_ipv6_reserved_multicast_addr_get(
    int unit, 
    bcm_ip6_t *ip6_addr, 
    bcm_ip6_t *ip6_mask);

/* Set/get multiple ipv6 reserved multicast address and mask. */
extern int bcm_switch_ipv6_reserved_multicast_addr_multi_set(
    int unit, 
    int inner, 
    int num, 
    bcm_ip6_t *ip6_addr, 
    bcm_ip6_t *ip6_mask);

/* Set/get multiple ipv6 reserved multicast address and mask. */
extern int bcm_switch_ipv6_reserved_multicast_addr_multi_get(
    int unit, 
    int inner, 
    int num, 
    bcm_ip6_t *ip6_addr, 
    bcm_ip6_t *ip6_mask);

#endif /* BCM_HIDE_DISPATCHABLE */

typedef enum bcm_switch_object_e {
    bcmSwitchObjectL2EntryCurrent = 0,  /* Statistics of current L2 Entry. */
    bcmSwitchObjectVlanCurrent = 1,     /* Statistics of current Vlan Entry. */
    bcmSwitchObjectL3HostCurrent = 2,   /* Statistics of current L3 Hosts. */
    bcmSwitchObjectL3RouteCurrent = 3,  /* Statistics of current L3 Routes. */
    bcmSwitchObjectL3EgressCurrent = 4, /* Statistics of current L3 Egress
                                           objects. */
    bcmSwitchObjectIpmcCurrent = 5,     /* Statistics of current IP Multicast
                                           Objects. */
    bcmSwitchObjectEcmpCurrent = 6,     /* Statistics of current ECMP objects. */
    bcmSwitchObjectL3RouteV4RoutesMax = 7, /* Maximum number of v4 routes possible */
    bcmSwitchObjectL3RouteV4RoutesFree = 8, /* Maximum number of v4 routes that can
                                           be added in current state */
    bcmSwitchObjectL3RouteV4RoutesUsed = 9, /* Used count of v4 routes */
    bcmSwitchObjectL3RouteV6Routes64bMax = 10, /* Maximum number of 64bv6 routes
                                           possible */
    bcmSwitchObjectL3RouteV6Routes64bFree = 11, /* Maximum number of 64bV6 routes that
                                           can be added in current state */
    bcmSwitchObjectL3RouteV6Routes64bUsed = 12, /* Used count of 64bv6 routes */
    bcmSwitchObjectL3RouteV6Routes128bMax = 13, /* Maximum number of 128bV6 routes
                                           possible */
    bcmSwitchObjectL3RouteV6Routes128bFree = 14, /* Maximum number of 128bV6 routes that
                                           can be added in current state */
    bcmSwitchObjectL3RouteV6Routes128bUsed = 15, /* Used count of 128bv6 routes */
    bcmSwitchObjectL3RouteTotalUsedRoutes = 16, /* Sum of 44 + 64bv6 + 128bv6 routes */
    bcmSwitchObjectIpmcHeadTableFree = 17, /* Number of free entries in the
                                           replication head table */
    bcmSwitchObjectL3HostV4Used = 18,   /* Statistics of L3 Hosts used by IPv4 */
    bcmSwitchObjectL3HostV6Used = 19,   /* Statistics of L3 Hosts used by IPv6 */
    bcmSwitchObjectEcmpMax = 20,        /* Maximum number of Ecmp groups
                                           possible */
    bcmSwitchObjectPFCDeadlockCosMax = 21, /* Get the Max COS supported for PFC
                                           Deadlock detection and recovery. */
    bcmSwitchObjectL3HostV4Max = 22,    /* Maximum number of IPv4 routes
                                           possible in L3 host table. */
    bcmSwitchObjectL3HostV6Max = 23,    /* Maximum number of IPv6 routes
                                           possible in L3 host table. */
    bcmSwitchObjectL3RouteV4RoutesMinGuaranteed = 24, /* Guaranteed number of IPv4 routes in
                                           ALPM mode. */
    bcmSwitchObjectL3RouteV6Routes64bMinGuaranteed = 25, /* Guaranteed number of IPv6 64bits
                                           routes in ALPM mode. */
    bcmSwitchObjectL3RouteV6Routes128bMinGuaranteed = 26, /* Guaranteed number of IPv6 128bits
                                           routes in ALPM mode. */
    bcmSwitchObjectL3EgressMax = 27,    /* Maximum number of L3 Egress objects
                                           possible. */
    bcmSwitchObjectIpmcV4Used = 28,     /* Statistics of used IPv4 Multicast
                                           entries. */
    bcmSwitchObjectIpmcV6Used = 29,     /* Statistics of used IPv6 Multicast
                                           entries. */
    bcmSwitchObjectIpmcV4Max = 30,      /* Maximum number of IPv4 Multicast
                                           entries possible. */
    bcmSwitchObjectIpmcV6Max = 31,      /* Maximum number of IPv6 Multicast
                                           entries possible. */
    bcmSwitchObjectL2EntryMax = 32,     /* Maximum number of L2 entries
                                           possible. */
    bcmSwitchObjectL3HostUcV4Max = 33,  /* Maximum number of L3 IPv4 unicast
                                           host entries possible. */
    bcmSwitchObjectL3HostUcV6Max = 34,  /* Maximum number of L3 IPv6 unicast
                                           host entries possible. */
    bcmSwitchObjectL3HostUcV4Used = 35, /* Statistics number of used L3 IPv4
                                           unicast host entries. */
    bcmSwitchObjectL3HostUcV6Used = 36, /* Statistics number of used L3 IPv6
                                           unicast host entries. */
    bcmSwitchObjectIpmcSrcGroupV4Max = 37, /* Maximum number of L3 IPv4 (S, G)
                                           Multicast host entries possible. */
    bcmSwitchObjectIpmcSrcGroupV6Max = 38, /* Maximum number of L3 IPv6 (S, G)
                                           Multicast host entries possible. */
    bcmSwitchObjectIpmcSrcGroupV4Used = 39, /* Statistics number of used L3 IPv4 (S,
                                           G) Multicast host entries. */
    bcmSwitchObjectIpmcSrcGroupV6Used = 40, /* Statistics number of used L3 IPv6 (S,
                                           G) Multicast host entries. */
    bcmSwitchObjectIpmcGroupV4Max = 41, /* Maximum number of L3 IPv4 (*, G)
                                           Multicast host entries possible. */
    bcmSwitchObjectIpmcGroupV6Max = 42, /* Maximum number of L3 IPv6 (*, G)
                                           Multicast host entries possible. */
    bcmSwitchObjectIpmcGroupV4Used = 43, /* Statistics number of used L3 IPv4 (*,
                                           G) Multicast host entries. */
    bcmSwitchObjectIpmcGroupV6Used = 44, /* Statistics number of used L3 IPv6 (*,
                                           G) Multicast host entries. */
    bcmSwitchObjectL3RoutePrivateV4RouteProjection = 45, /* Projected maximum IPv4 private VRF
                                           routes in ALPM mode. */
    bcmSwitchObjectL3RouteGlobalV4RouteProjection = 46, /* Projected maximum IPv4 global low
                                           routes in ALPM mode. */
    bcmSwitchObjectL3RouteOverrideV4RouteProjection = 47, /* Projected maximum IPv4 override
                                           (global high) routes in ALPM mode. */
    bcmSwitchObjectL3RoutePrivateV6RouteProjection = 48, /* Projected maximum IPv6 private VRF
                                           routes in ALPM mode. */
    bcmSwitchObjectL3RouteGlobalV6RouteProjection = 49, /* Projected maximum IPv6 global low
                                           routes in ALPM mode. */
    bcmSwitchObjectL3RouteOverrideV6RouteProjection = 50, /* Projected maximum IPv6 override
                                           (global high) routes in ALPM mode. */
    bcmSwitchObjectCount                /* Maximum object count. This is not an
                                           object and should always be in the
                                           last */
} bcm_switch_object_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Retrieving the statistics on the number of API objects. */
extern int bcm_switch_object_count_get(
    int unit, 
    bcm_switch_object_t object, 
    int *entries);

/* 
 * Retrieving the statistics on the number of API objects. This retrieves
 * for multiple objects.
 */
extern int bcm_switch_object_count_multi_get(
    int unit, 
    int object_size, 
    bcm_switch_object_t *object_array, 
    int *entries);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Services types. */
typedef enum bcm_switch_service_e {
    bcmServiceCounterCollection = 0,    /* Counters collection service. */
    bcmServiceRxLOS = 1,                /* RX LOS service. */
    bcmServiceCount                     /* Maximum object count. This is not an
                                           object and should always be in the
                                           last */
} bcm_switch_service_t;

/* Service enable configuration options. */
typedef enum bcm_switch_service_enabled_e {
    bcmServiceStateDisabled = 0,    /* Service disabled. */
    bcmServiceStateEnabled = 1,     /* Service enabled. */
    bcmServiceEnabledCount          /* Maximum object count. This is not an
                                       object and should always be in the last */
} bcm_switch_service_enabled_t;

/* Service configuration structure. */
typedef struct bcm_switch_service_config_s {
    uint32 flags;                       /* Option flags. */
    bcm_switch_service_enabled_t enabled; /* Service enable configuration. */
} bcm_switch_service_config_t;

/* Initialize a bcm_switch_service_config_t_init structure. */
extern void bcm_switch_service_config_t_init(
    bcm_switch_service_config_t *config);

#ifndef BCM_HIDE_DISPATCHABLE

/* Change/read configuration of specific service. */
extern int bcm_switch_service_set(
    int unit, 
    bcm_switch_service_t service, 
    bcm_switch_service_config_t *config);

/* Change/read configuration of specific service. */
extern int bcm_switch_service_get(
    int unit, 
    bcm_switch_service_t service, 
    bcm_switch_service_config_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* MC queue group mode. */
typedef enum bcm_switch_control_mc_queue_config_mode_e {
    bcmSwitchMcUcPairGroupMode = 0, /* Each MC queue is paired with one UC queue */
    bcmSwitchAllMcGroupMode = 1     /* All MC queues are grouped together */
} bcm_switch_control_mc_queue_config_mode_t;

/* Flags for bcm_switch_l3_protocol_group_set / get */
#define BCM_SWITCH_L3_PROTOCOL_GROUP_NONE   0x0000     /* No protocols in this
                                                          group */
#define BCM_SWITCH_L3_PROTOCOL_GROUP_IPV4   0x0001     /* Add IPv4 to the group. */
#define BCM_SWITCH_L3_PROTOCOL_GROUP_IPV6   0x0002     /* Add IPv6 to the group. */
#define BCM_SWITCH_L3_PROTOCOL_GROUP_ARP    0x0004     /* Add ARP to the group. */
#define BCM_SWITCH_L3_PROTOCOL_GROUP_MPLS   0x0008     /* Add MPLS to the group. */
#define BCM_SWITCH_L3_PROTOCOL_GROUP_MIM    0x0010     /* Add MiM to the group. */
#define BCM_SWITCH_L3_PROTOCOL_GROUP_TRILL  0x0020     /* Add trill to the
                                                          group. */
#define BCM_SWITCH_L3_PROTOCOL_GROUP_FCOE   0x0040     /* Add FCOE to the group. */

/* 
 * Protocol group to which l3 protocols will be mapped for multiple mymac
 * termination
 */
typedef uint32 bcm_l3_protocol_group_id_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Assign/get protocol groups for multiple mymac termination */
extern int bcm_switch_l3_protocol_group_set(
    int unit, 
    uint32 group_members, 
    bcm_l3_protocol_group_id_t group_id);

/* Assign/get protocol groups for multiple mymac termination */
extern int bcm_switch_l3_protocol_group_get(
    int unit, 
    uint32 *group_members, 
    bcm_l3_protocol_group_id_t *group_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* profile mapping type. */
typedef enum bcm_switch_profile_mapping_type_e {
    bcmCosqIngressQueueToRateClass = 0, /* ingress queue to rate class mapping */
    bcmCosqIngressQueuesToLatencyProfile = 1, /* ingress queues to ingress latency
                                           profile mapping */
    bcmCosqPortToEndToEndLatencyProfile = 2 /* destination local port to end to end
                                           latency profile mapping */
} bcm_switch_profile_mapping_type_t;

/* profile mapping struct. */
typedef struct bcm_switch_profile_mapping_s {
    bcm_switch_profile_mapping_type_t profile_type; /* The type of profile to be mapped to. */
    bcm_gport_t mapped_profile;         /* mapped profile handle */
} bcm_switch_profile_mapping_t;

/* Initialize a bcm_switch_profile_mapping_t structure */
extern void bcm_switch_profile_mapping_t_init(
    bcm_switch_profile_mapping_t *profile_mapping);

/* Global TPID type. */
typedef enum bcm_switch_tpid_type_e {
    bcmTpidTypeOuter = 0,               /* Global Outer TPID */
    bcmTpidTypeInner = 1,               /* Global Inner TPID */
    bcmTpidTypeVntag = 2,               /* Global TPID for Vntag */
    bcmTpidTypeEtag = 3,                /* Global TPID for Etag */
    bcmTpidTypeCapwapPayloadOuter = 4,  /* Global Outer TPID of CAPWAP wireless
                                           payload */
    bcmTpidTypeCapwapPayloadInner = 5,  /* Global Inner TPID of CAPWAP wireless
                                           payload */
    bcmTpidTypeL2TunnelPayloadInner = 6, /* Global Inner TPID of L2 Tunnel
                                           payload */
    bcmTpidTypeMimPayloadOuter = 7,     /* Matched TPID of Transit MIM Payload
                                           for Parser */
    bcmTpidTypeVxlanPayloadOuter = 8,   /* Matched TPID of Transit VXLAN Payload
                                           for Parser */
    bcmTpidTypeL2grePayloadOuter = 9,   /* Matched TPID of Transit L2GRE Payload
                                           for Parser */
    bcmTpidTypeVxlanPayloadInner = 10,  /* Matched inner TPID of Transit VXLAN
                                           Payload for Parser */
    bcmTpidTypeItag = 11,               /* Global TPID value for I-Tag */
    bcmTpidTypeCount = 12               /* Always Last, not a usable value */
} bcm_switch_tpid_type_t;

/* Global TPID information */
typedef struct bcm_switch_tpid_info_s {
    bcm_switch_tpid_type_t tpid_type;   /* The type of TPID to be set */
    uint16 tpid_value;                  /* The value of TPID to be set */
    uint32 flags;                       /* Future expansion, BCM_SWITCH_TPID_XXX
                                           flags */
    int color;                          /* Color mode for TPID */
} bcm_switch_tpid_info_t;

/* Global TPID information flags */
#define BCM_SWITCH_TPID_VALUE_INVALID   0x0000     /* TPID value 0x0000 is not
                                                      valid. See the Programming
                                                      Guide PP document for full
                                                      details */

#ifndef BCM_HIDE_DISPATCHABLE

/* Add a global TPID. */
extern int bcm_switch_tpid_add(
    int unit, 
    uint32 options, 
    bcm_switch_tpid_info_t *tpid_info);

/* Delete a global TPID. */
extern int bcm_switch_tpid_delete(
    int unit, 
    bcm_switch_tpid_info_t *tpid_info);

/* Delete all global TPIDs. */
extern int bcm_switch_tpid_delete_all(
    int unit);

/* Get a global TPID. */
extern int bcm_switch_tpid_get(
    int unit, 
    bcm_switch_tpid_info_t *tpid_info);

/* Get global TPID count or TPIDs. */
extern int bcm_switch_tpid_get_all(
    int unit, 
    int size, 
    bcm_switch_tpid_info_t *tpid_info_array, 
    int *count);

#endif /* BCM_HIDE_DISPATCHABLE */

/* TPID class match type. */
typedef enum bcm_switch_tpid_class_match_e {
    bcmSwitchTpidClassMatchPort = 0,    /* tpid_class_id from port. */
    bcmSwitchTpidClassMatchIngVlanActionId = 1, /* tpid_class_id from Ingress vlan
                                           translation action id */
    bcmSwitchTpidClassMatch_Count = 2   /* Number of match criterion */
} bcm_switch_tpid_class_match_t;

/* TPID class retrieval information */
typedef struct bcm_switch_tpid_class_s {
    uint32 flags;                       /* BCM_SWITCH_TPID_CLASS_XXX, unused
                                           now. */
    bcm_switch_tpid_class_match_t match; /* Match criteria for tpid_class_id */
    bcm_gport_t port;                   /* Port ID */
    int vlan_translation_action_id;     /* Vlan Translation action ID */
    uint32 tpid_class_id;               /* TPID-profile */
} bcm_switch_tpid_class_t;

/* Initialize a bcm_switch_tpid_class_t structure. */
extern void bcm_switch_tpid_class_t_init(
    bcm_switch_tpid_class_t *tpid_class);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Get the tpid profile from a given port or ingress vlan translation
 * action ID.
 */
extern int bcm_switch_tpid_class_get(
    int unit, 
    bcm_switch_tpid_class_t *tpid_class);

#endif /* BCM_HIDE_DISPATCHABLE */

/* packet trace lookup result enums */
typedef enum bcm_switch_pkt_trace_lookup_e {
    bcmSwitchPktTraceLookupInvalid = 0, 
    bcmSwitchPktTraceLookupFirstVlanTranslationHit = 1, 
    bcmSwitchPktTraceLookupSecondVlanTranslationHit = 2, 
    bcmSwitchPktTraceLookupForwardingVlanValid = 3, 
    bcmSwitchPktTraceLookupL2SrcHit = 6, 
    bcmSwitchPktTraceLookupL2SrcStatic = 7, 
    bcmSwitchPktTraceLookupL2DstHit = 8, 
    bcmSwitchPktTraceLookupL2CacheHit = 9, 
    bcmSwitchPktTraceLookupL3SrcHostHit = 10, 
    bcmSwitchPktTraceLookupL3DestHostHit = 11, 
    bcmSwitchPktTraceLookupL3DestRouteHit = 12, 
    bcmSwitchPktTraceLookupL2SrcMiss = 13, 
    bcmSwitchPktTraceLookupDosAttack = 14, 
    bcmSwitchPktTraceLookupIpTunnelHit = 15, 
    bcmSwitchPktTraceLookupMplsLabel1Hit = 16, 
    bcmSwitchPktTraceLookupMplsLabel2Hit = 17, 
    bcmSwitchPktTraceLookupMplsTerminated = 18, 
    bcmSwitchPktTraceLookupMystationHit = 19, 
    bcmSwitchPktTraceLookupCount = 20 
} bcm_switch_pkt_trace_lookup_t;

/* lookup result set bit map */
typedef struct bcm_switch_pkt_trace_lookup_result_s {
    SHR_BITDCL pkt_trace_status_bitmap[_SHR_BITDCLSIZE(bcmSwitchPktTraceLookupCount)]; /* bit map for packet trace lookup
                                           result set */
} bcm_switch_pkt_trace_lookup_result_t;

/* packet trace resolution enums */
typedef enum bcm_switch_pkt_trace_resolution_e {
    bcmSwitchPktTraceResolutionUnkown = 0, 
    bcmSwitchPktTraceResolutionControlPkt = 1, /* 1 */
    bcmSwitchPktTraceResolutionOamPkt = 2, 
    bcmSwitchPktTraceResolutionBfdPkt = 3, 
    bcmSwitchPktTraceResolutionBpduPkt = 4, 
    bcmSwitchPktTraceResolution1588Pkt = 6, 
    bcmSwitchPktTraceResolutionKnownL2UcPkt = 8, 
    bcmSwitchPktTraceResolutionUnknownL2UcPkt = 9, 
    bcmSwitchPktTraceResolutionKnownL2McPkt = 10, 
    bcmSwitchPktTraceResolutionUnknownL2McPkt = 11, 
    bcmSwitchPktTraceResolutionL2BcPkt = 12, 
    bcmSwitchPktTraceResolutionKnownL3UcPkt = 16, 
    bcmSwitchPktTraceResolutionUnknownL3UcPkt = 17, 
    bcmSwitchPktTraceResolutionKnownIpmcPkt = 18, 
    bcmSwitchPktTraceResolutionUnknownIpmcPkt = 19, 
    bcmSwitchPktTraceResolutionKnownMplsL2Pkt = 24, 
    bcmSwitchPktTraceResolutionUnknownMplsPkt = 25, 
    bcmSwitchPktTraceResolutionKnownMplsL3Pkt = 26, 
    bcmSwitchPktTraceResolutionKnownMplsPkt = 28, 
    bcmSwitchPktTraceResolutionKnownMimPkt = 32, 
    bcmSwitchPktTraceResolutionUnknownMimPkt = 33, 
    bcmSwitchPktTraceResolutionKnownTrillPkt = 40, 
    bcmSwitchPktTraceResolutionUnknownTrillPkt = 41, 
    bcmSwitchPktTraceResolutionKnownNivPkt = 48, 
    bcmSwitchPktTraceResolutionUnknownNivPkt = 49, 
    bcmSwitchPktTraceResolutionKnownL2GrePkt = 50, 
    bcmSwitchPktTraceResolutionKnownVxlanPkt = 51, 
    bcmSwitchPktTraceResolutionKnownFCOEPkt = 52, 
    bcmSwitchPktTraceResolutionUnknownFCOEPkt = 53, 
    bcmSwitchPktTraceResolutionCount = 54 
} bcm_switch_pkt_trace_resolution_t;

#define BCM_SWITCH_PKT_TRACE_ECMP_1         0x0001     /* level 1 ecmp hashing
                                                          resolution done */
#define BCM_SWITCH_PKT_TRACE_ECMP_2         0x0002     /* level 2 ecmp hashing
                                                          resolution done */
#define BCM_SWITCH_PKT_TRACE_TRUNK          0x0004     /* trunk hashing
                                                          resolution done */
#define BCM_SWITCH_PKT_TRACE_FABRIC_TRUNK   0x0008     /* hg trunk hashing
                                                          resolution done */

/* packet hashing resolution information */
typedef struct bcm_switch_pkt_trace_hashing_info_s {
    uint32 flags;                       /* flag containing
                                           BCM_SWITCH_PKT_TRACE_ECMP/TRUNK/FABRIC_TRUNK,
                                           0 == no hashing resolution done */
    bcm_if_t ecmp_1_group;              /* multipath egress forwarding object. */
    bcm_if_t ecmp_1_egress;             /* ecmp destination interface */
    bcm_if_t ecmp_2_group;              /* multipath egress forwarding object. */
    bcm_if_t ecmp_2_egress;             /* ecmp destination interface */
    bcm_gport_t trunk;                  /* destination trunk group */
    bcm_gport_t trunk_member;           /* destination member port which packet
                                           egress. */
    bcm_gport_t fabric_trunk;           /* destination hg trunk group */
    bcm_gport_t fabric_trunk_member;    /* destination member hg port which
                                           packet will egress. */
} bcm_switch_pkt_trace_hashing_info_t;

/* packet forward destination enums */
#define BCM_SWITCH_PKT_TRACE_FWD_DST_INVALID 0          /* Forward destination
                                                          invalid. */
#define BCM_SWITCH_PKT_TRACE_FWD_DST_NH     0x0001     /* Forward destination
                                                          Next hop egress
                                                          object. */
#define BCM_SWITCH_PKT_TRACE_FWD_DST_L2MC   0x0002     /* Forward destination
                                                          L2MC. */
#define BCM_SWITCH_PKT_TRACE_FWD_DST_IPMC   0x0004     /* Forward destination
                                                          IPMC. */
#define BCM_SWITCH_PKT_TRACE_FWD_DST_DGPP   0x0008     /* Forward destination
                                                          DGPP */
#define BCM_SWITCH_PKT_TRACE_FWD_DST_VLAN   0x0010     /* Forward destination
                                                          VLAN */
#define BCM_SWITCH_PKT_TRACE_FWD_DST_DVP    0x0020     /* Forward destination
                                                          DVP */

/* packet forward destination information */
typedef struct bcm_switch_pkt_trace_fwd_dst_info_s {
    uint32 flags;               /* flag containing
                                   BCM_SWITCH_PKT_TRACE_FWD_DST_NH/L2MC/IPMC/DGPP/DVP/VLAN/INVALID,
                                   0 == no forward destination resolution done */
    bcm_if_t egress_intf;       /* egress forwarding destination interface. */
    bcm_multicast_t mc_group;   /* Multicast group. */
    bcm_gport_t port;           /* DGPP - destination port. */
    bcm_gport_t dvp;            /* DVP - destination virtual port. */
    bcm_vlan_t fwd_vlan;        /* Destination forward vlan. */
} bcm_switch_pkt_trace_fwd_dst_info_t;

#define BCM_SWITCH_PKT_TRACE_DROP_REASON_MAX 4          /* drop reason bitmap
                                                          length */

/* packet trace drop reason enums */
typedef enum bcm_switch_pkt_trace_drop_reason_e {
    bcmSwitchPktTraceNoDrop = 0,        /* No packet drop */
    bcmSwitchPktTraceDropReasonInternal = 1, /* Packet dropped due to internal reason */
    bcmSwitchPktTraceDropReasonMplsLabelLookupMiss = 2, /* Packet dropped due to MPLS label
                                           lookup miss */
    bcmSwitchPktTraceDropReasonMplsInvalidAction = 3, /* Packet dropped due to MPLS invalid
                                           action */
    bcmSwitchPktTraceDropReasonMplsInvalidPayload = 4, /* Packet dropped due to MPLS invalid
                                           payload */
    bcmSwitchPktTraceDropReasonMplsTtlCheckFail = 5, /* Packet dropped due to MPLS TTL check
                                           fail */
    bcmSwitchPktTraceDropReasonMplsInvalidControlWord = 6, /* Packet dropped due to MPLS invalid
                                           control word */
    bcmSwitchPktTraceDropReasonL2greSipLookupMiss = 7, /* Packet dropped due to L2GRE SIP
                                           lookup miss */
    bcmSwitchPktTraceDropReasonL2greVpnLookupMiss = 8, /* Packet dropped due to L2GRE VPNID
                                           lookup miss */
    bcmSwitchPktTraceDropReasonL2greTunnelError = 9, /* Packet dropped due to L2GRE Tunnel
                                           Error */
    bcmSwitchPktTraceDropReasonVxlanSipLookupMiss = 10, /* Packet dropped due to VXLAN SIP
                                           lookup miss */
    bcmSwitchPktTraceDropReasonVxlanVnidLookupMiss = 11, /* Packet dropped due to VXLAN VN_ID
                                           lookup miss */
    bcmSwitchPktTraceDropReasonVxlanTunnelError = 12, /* Packet dropped due to VXLAN Tunnel
                                           Error */
    bcmSwitchPktTraceDropReasonVlanNotValid = 13, /* Packet dropped due to VLAN not valid */
    bcmSwitchPktTraceDropReasonIngressPortNotInVlanMember = 14, /* Packet dropped due to ingress port
                                           not in VLAN member */
    bcmSwitchPktTraceDropReasonTpidMismatch = 15, /* Packet dropped due to TPID mismatch */
    bcmSwitchPktTraceDropReasonIpv4ProtocolError = 16, /* IPV4 protocol error drop */
    bcmSwitchPktTraceDropReasonHigigLoopback = 17, /* HiGig loopback drop */
    bcmSwitchPktTraceDropReasonHigigMirrorOnly = 18, /* HiGig mirror only drop */
    bcmSwitchPktTraceDropReasonHigigUnknownHeader = 19, /* HiGig unknown header drop */
    bcmSwitchPktTraceDropReasonHigigUnknownOpcode = 20, /* HiGig unknown opcode drop */
    bcmSwitchPktTraceDropReasonLagFailLoopback = 21, /* LAG fail loopback drop */
    bcmSwitchPktTraceDropReasonL2SrcEqualL2Dst = 22, /* Src MAC equals Dst MAC drop */
    bcmSwitchPktTraceDropReasonIpv6ProtocolError = 23, /* IPV6 protocol error drop */
    bcmSwitchPktTraceDropReasonNivVntagPresent = 24, /* NIV VNTAG present drop */
    bcmSwitchPktTraceDropReasonNivVntagNotPresent = 25, /* NIV VNTAG not present drop */
    bcmSwitchPktTraceDropReasonNivVntagFormat = 26, /* NIV VNTAG format drop */
    bcmSwitchPktTraceDropReasonTrillErrorFrame = 27, /* Trill error frame drop */
    bcmSwitchPktTraceDropReasonBpdu = 28, /* BPDU drop */
    bcmSwitchPktTraceDropReasonBadUdpChecksum = 29, /* Bad udp checksum drop */
    bcmSwitchPktTraceDropReasonTunnlDecapEcnError = 30, /* Tunnel Decap ECN error drop */
    bcmSwitchPktTraceDropReasonIpv4HeaderError = 31, /* Packet dropped due to IPv4 Header
                                           error */
    bcmSwitchPktTraceDropReasonIpv6HeaderError = 32, /* Packet dropped due to IPv6 Header
                                           error */
    bcmSwitchPktTraceDropReasonParityError = 33, /* Packet dropped due to parity error */
    bcmSwitchPktTraceDropReasonRpfCheckFail = 34, /* Packet dropped due to unicast RPF
                                           check fail */
    bcmSwitchPktTraceDropReasonStageLookup = 35, /* Packet dropped on field lookup stage */
    bcmSwitchPktTraceDropReasonStageIngress = 36, /* Packet dropped on field ingress stage */
    bcmSwitchPktTraceDropReasonTunnelObjectValidationFail = 37, /* Packet dropped due to Tunnel
                                           termination can't be done as not all
                                           packet forwarding objects got
                                           assigned. */
    bcmSwitchPktTraceDropReasonTunnelShimHeaderError = 38, /* Packet dropped due to tunnel shim
                                           header error. */
    bcmSwitchPktTraceDropReasonTunnelTTLError = 39, /* Packet dropped due to tunnel TTL
                                           check fail. */
    bcmSwitchPktTraceDropReasonTunnelInterfaceCheckFail = 40, /* Packet dropped due to tunnel
                                           interface check failure. */
    bcmSwitchPktTraceDropReasonTunnelError = 41, /* Packet dropped due to tunnel error. */
    bcmSwitchPktTraceDropReasonTunnelAdapt1LookupMiss = 42, /* Packcet dropped due to tunnel
                                           adaptation table 1 lookup miss. */
    bcmSwitchPktTraceDropReasonTunnelAdapt2LookupMiss = 43, /* Packcet dropped due to tunnel
                                           adaptation table 2 lookup miss. */
    bcmSwitchPktTraceDropReasonTunnelAdapt3LookupMiss = 44, /* Packcet dropped due to tunnel
                                           adaptation table 3 lookup miss. */
    bcmSwitchPktTraceDropReasonTunnelAdapt4LookupMiss = 45, /* Packcet dropped due to tunnel
                                           adaptation table 4 lookup miss. */
    bcmSwitchPktTraceDropReasonControlFrame = 46, /* Packcet dropped due to control frame
                                           drop. */
    bcmSwitchPktTraceDropReasonHigigHdrError = 47, /* Packcet dropped due to higig header
                                           error. */
    bcmSwitchPktTraceDropReasonNivDot1p = 48, /* Packcet dropped due to niv dot1p. */
    bcmSwitchPktTraceDropReasonMplsGalLabel = 49, /* Packcet dropped due to mpls gal
                                           label. */
    bcmSwitchPktTraceDropReasonStpNotForwarding = 50, /* Packcet dropped due to stp not
                                           forwarding. */
    bcmSwitchPktTraceDropReasonMcastIndexError = 51, /* Packcet dropped due to multicast
                                           index error. */
    bcmSwitchPktTraceDropReasonProtocolPkt = 52, /* Packcet dropped due to protocol pkt
                                           drop. */
    bcmSwitchPktTraceDropReasonUnknownSrcMac = 53, /* Packcet dropped due to unknown src
                                           mac. */
    bcmSwitchPktTraceDropReasonSrcRoute = 54, /* Packcet dropped due to source route. */
    bcmSwitchPktTraceDropReasonL2SrcDiscard = 55, /* Packcet dropped due to L2 source
                                           discard. */
    bcmSwitchPktTraceDropReasonL2SrcStaticMove = 56, /* Packcet dropped due to L2 source
                                           static move. */
    bcmSwitchPktTraceDropReasonL2DstDiscard = 57, /* Packcet dropped due to L2 dest
                                           discard. */
    bcmSwitchPktTraceDropReasonSrcMacZero = 58, /* Packcet dropped due to source mac
                                           zero. */
    bcmSwitchPktTraceDropReasonL2DstMissOrPbt = 59, /* Packcet dropped due to L2 dest miss
                                           or pbt. */
    bcmSwitchPktTraceDropReasonClassBasedStationMove = 60, /* Packcet dropped due to class based
                                           station move. */
    bcmSwitchPktTraceDropReasonTimeSyncPkt = 61, /* Packcet dropped due to time sync pkt
                                           drop. */
    bcmSwitchPktTraceDropReasonMyStation = 62, /* Packcet dropped due to my station
                                           drop. */
    bcmSwitchPktTraceDropReasonL3DstDiscard = 63, /* Packcet dropped due to L3 dest
                                           discard. */
    bcmSwitchPktTraceDropReasonMcastLookupMiss = 64, /* Packcet dropped due to multicast
                                           lookup miss. */
    bcmSwitchPktTraceDropReasonPortTagPresentError = 65, /* Packcet dropped due to port tag
                                           present error. */
    bcmSwitchPktTraceDropReasonPVlanVpEgressFilter = 66, /* Packcet dropped due to pvlan vp
                                           egress filter. */
    bcmSwitchPktTraceDropReasonPortBitmapZero = 67, /* Packcet dropped due to port bitmap
                                           zero. */
    bcmSwitchPktTraceDropReasonCount = 68 
} bcm_switch_pkt_trace_drop_reason_t;

#define BCM_SWITCH_PKT_TRACE_RAW_DATA_MAX   112        /* max bytes of
                                                          pkt_trace_info.raw_data */

/* packet trace ingress process data */
typedef struct bcm_switch_pkt_trace_info_s {
    bcm_switch_pkt_trace_lookup_result_t pkt_trace_lookup_status; /* packet trace lookup status set */
    bcm_switch_pkt_trace_resolution_t pkt_trace_resolution; /* packet trace resolution result */
    bcm_switch_pkt_trace_hashing_info_t pkt_trace_hash_info; /* packet trace hashing information */
    bcm_stg_stp_t pkt_trace_stp_state;  /* packet trace ingress stp state */
    uint32 pkt_trace_drop_reason[BCM_SWITCH_PKT_TRACE_DROP_REASON_MAX]; /* packet trace drop reason */
    uint32 dest_pipe_num;               /* ingress pipeline number of the pkt
                                           trace packet */
    uint32 raw_data_length;             /* number of bytes returned as raw_data */
    uint8 raw_data[BCM_SWITCH_PKT_TRACE_RAW_DATA_MAX]; /* packet trace process result data in
                                           raw format */
    bcm_switch_pkt_trace_fwd_dst_info_t pkt_trace_fwd_dst_info; /* packet trace forward destination
                                           information */
} bcm_switch_pkt_trace_info_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Clear the visibility SW and HW configuration and states */
extern int bcm_switch_pkt_trace_info_clear(
    int unit, 
    bcm_port_t port);

/* 
 * Generate a visibility trace packet and/or read the result of the
 * packets ingress processing information
 */
extern int bcm_switch_pkt_trace_info_get(
    int unit, 
    uint32 options, 
    uint8 port, 
    int len, 
    uint8 *data, 
    bcm_switch_pkt_trace_info_t *pkt_trace_info);

/* 
 * Generate a visibility trace packet and then read the visibility packet
 * debug information as raw data.
 */
extern int bcm_switch_pkt_trace_raw_data_get(
    int unit, 
    uint32 options, 
    uint8 port, 
    int len, 
    uint8 *data, 
    uint32 raw_data_buf_size, 
    uint8 *raw_data, 
    uint32 *raw_data_size);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 32bit or 48bit timestamping mode for 1588 packets */
typedef enum bcm_switch_timesync_timestamping_mode_e {
    bcmTimesyncTimestampingMode32bit = 0, /* 32bit timestamping mode for 1588
                                           packets */
    bcmTimesyncTimestampingMode48bit = 1 /* 48bit timestamping mode for 1588
                                           packets. */
} bcm_switch_timesync_timestamping_mode_t;

/* 
 * Flags for bcm_switch_lif_property_set / get - Correlates with
 * bcmLifPropertyOutGlobalLifRangeType
 */
#define BCM_SWITCH_LIF_PROPERTY_ID_RANGE_TYPE_MAPPED 0x0        /* global outlif mapped. */
#define BCM_SWITCH_LIF_PROPERTY_ID_RANGE_TYPE_DIRECT 0x1        /* global outlif direct. */

/* 
 * Flags for bcm_switch_lif_property_set / get - Correlates with
 * bcmLifPropertyEEDBBankPhase
 */
#define BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_DYNAMIC 0x0        /* EEDB bank dynamic
                                                          phase. */
#define BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_1 0x1        /* EEDB bank phase 1. */
#define BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_2 0x2        /* EEDB bank phase 2. */
#define BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_3 0x4        /* EEDB bank phase 3. */
#define BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_4 0x8        /* EEDB bank phase 4. */
#define BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_5 0x10       /* EEDB bank phase 5. */
#define BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_6 0x20       /* EEDB bank phase 6. */

/* 
 * Flags for bcm_switch_lif_property_set / get - Correlates with
 * bcmLifPropertyEEDBBankExtension
 */
#define BCM_SWITCH_LIF_PROPERTY_EEDB_BANK_EXT_ENABLE 0x0        /* EEDB bank has
                                                          extension. */
#define BCM_SWITCH_LIF_PROPERTY_EEDB_BANK_EXT_DISABLE 0x1        /* EEDB bank doesn't have
                                                          extension. */
#define BCM_SWITCH_LIF_PROPERTY_EEDB_BANK_EXT_DYNAMIC 0x2        /* EEDB bank extension
                                                          dynamic. */

/* 
 * Flags for bcm_switch_lif_property_set / get - Correlates with
 * bcmLifPropertyEEDBBankExtensionType
 */
#define BCM_SWITCH_LIF_PROPERTY_EEDB_BANK_EXT_TYPE_PROTECTION 0x0        /* EEDB bank extension
                                                          type protection
                                                          pointer. */
#define BCM_SWITCH_LIF_PROPERTY_EEDB_BANK_EXT_TYPE_DATA 0x1        /* EEDB bank extension
                                                          type Data. */

/* 
 * lif property configuration parameters for
 * bcm_switch_lif_property_set/get.
 */
typedef enum bcm_switch_lif_property_e {
    bcmLifPropertyOutGlobalLifRangeType = 0, /* per LIF range of 4K entries, specify
                                           if it's Direct or Mapped. */
    bcmLifPropertyEEDBBankPhase = 1,    /* per EEDB bank (0-23), specify if it's
                                           Phase. */
    bcmLifPropertyEEDBBankExtension = 2, /* per EEDB bank (0-21), specify if it
                                           has an extension */
    bcmLifPropertyEEDBBankExtensionType = 3 /* per EEDB bank (0-21), specify its
                                           extension type: protection-ptr or
                                           Data */
} bcm_switch_lif_property_t;

/* 
 * This structure describes the fields that need to be supplies for the
 * configuration along with the property type.
 */
typedef struct bcm_switch_lif_property_config_s {
    uint32 key;     /* property key : 0-47  in case of
                       bcmLifPropertyOutGlobalLifRangeType or 0-23 in case of
                                              bcmLifPropertyEEDBBankPhase and
                       bcmLifPropertyEEDBBankExtension */
    uint32 value;   /* property value */
} bcm_switch_lif_property_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Setup the LIF property along with the desired property and the
 * configuration values
 */
extern int bcm_switch_lif_property_set(
    int unit, 
    bcm_switch_lif_property_t lif_property, 
    bcm_switch_lif_property_config_t *lif_config);

/* 
 * Get the LIF property along with the desired property and the
 * configuration values.
 */
extern int bcm_switch_lif_property_get(
    int unit, 
    bcm_switch_lif_property_t lif_property, 
    bcm_switch_lif_property_config_t *lif_config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Key to be used for switch control indexed setget. */
typedef struct bcm_switch_control_key_s {
    bcm_switch_control_t type;  /* Switch control type */
    int index;                  /* Switch control index */
} bcm_switch_control_key_t;

/* Info to be used for switch control indexed setget. */
typedef struct bcm_switch_control_info_s {
    int value;  /* Switch control value */
} bcm_switch_control_info_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Configure port-specific and device-wide operating modes. Device wide
 * operating
 * modes are configured on all ports, except the stack ports.
 */
extern int bcm_switch_control_indexed_set(
    int unit, 
    bcm_switch_control_key_t key, 
    bcm_switch_control_info_t value);

/* 
 * Configure port-specific and device-wide operating modes. Device wide
 * operating
 * modes are configured on all ports, except the stack ports.
 */
extern int bcm_switch_control_indexed_get(
    int unit, 
    bcm_switch_control_key_t key, 
    bcm_switch_control_info_t *value);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Specified MAC address is used as destination MAC address when BCM
 * SWITCH device sends OLP encapsulated pkt from specified port towards
 * offload processor
 */
typedef struct bcm_switch_olp_l2_addr_s {
    bcm_port_t port;    /* ingress port for this entry */
    bcm_mac_t mac;      /* MAC address for this entry */
} bcm_switch_olp_l2_addr_t;

/* Initialize a bcm_switch_olp_l2_addr_t structure. */
extern void bcm_switch_olp_l2_addr_t_init(
    bcm_switch_olp_l2_addr_t *l2_addr);

/* Replace the matching entry */
#define BCM_SWITCH_OLP_L2_ADDR_REPLACE  0x1        

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/Replace MAC address assigned to Offload Processor connecting port. */
extern int bcm_switch_olp_l2_addr_add(
    int unit, 
    uint32 options, 
    bcm_switch_olp_l2_addr_t *l2_addr);

/* Delete OLP MAC address. */
extern int bcm_switch_olp_l2_addr_delete(
    int unit, 
    bcm_switch_olp_l2_addr_t *l2_addr);

/* Delete all previously configured OLP mac addresses. */
extern int bcm_switch_olp_l2_addr_delete_all(
    int unit);

/* Get MAC address assigned to Offload Processor connecting port. */
extern int bcm_switch_olp_l2_addr_get(
    int unit, 
    bcm_switch_olp_l2_addr_t *l2_addr);

#endif /* BCM_HIDE_DISPATCHABLE */

/* OLP MAC address traverse callback */
typedef int (*bcm_switch_olp_l2_addr_traverse_cb)(
    int unit, 
    bcm_switch_olp_l2_addr_t *l2_addr, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Traverse previously configured OLP MAC addresses and call callback
 * function for each valid OLP MAC.
 */
extern int bcm_switch_olp_l2_addr_traverse(
    int unit, 
    bcm_switch_olp_l2_addr_traverse_cb cb_fn, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* AGM Forwarding group type */
typedef enum bcm_switch_agm_type_e {
    bcmSwitchAgmTypeTrunk = 0,          /* Forwarding group is a Trunk group */
    bcmSwitchAgmTypeFabricTrunk = 1,    /* Forwarding group is a HiGig Trunk
                                           group */
    bcmSwitchAgmTypeL3Ecmp = 2,         /* Forwarding group is an Ecmp Trunk
                                           group */
    bcmSwitchAgmTypeL3EcmpOverlay = 3,  /* Forwarding group is an Overlay Ecmp
                                           Trunk group */
    bcmSwitchAgmTypeCount = 4           /* Maximum AGM type count */
} bcm_switch_agm_type_t;

/* AGM period interval type */
typedef enum bcm_switch_agm_period_interval_e {
    bcmSwitchAgmInterval1Second = 0,    /* 1s interval for each monitor period */
    bcmSwitchAgmInterval100MiliSecond = 1, /* 100 ms interval for each monitor
                                           period */
    bcmSwtichAgmInterval10MiliSecond = 2, /* 10ms interval for each monitor period */
    bcmSwitchAgmInterval1MiliSecond = 3, /* 1ms interval for each monitor period */
    bcmSwitchAgmInterval100MacroSecond = 4, /* 100us interval for each monitor
                                           period */
    bcmSwitchAgmIntervalCount = 5       /* Maximum AGM period interval type
                                           count */
} bcm_switch_agm_period_interval_t;

/* AGM monitor info */
typedef struct bcm_switch_agm_info_s {
    bcm_switch_agm_id_t agm_id;         /* AGM monitor id */
    bcm_switch_agm_type_t agm_type;     /* Indicates forwarding group type using
                                           this monitor */
    int num_members;                    /* number of members for the forwarding
                                           group */
    int period_num;                     /* AGM period number to be monitored, 0
                                           means continue counting traffic
                                           indefinitely */
    bcm_switch_agm_period_interval_t period_interval; /* interval for each period */
} bcm_switch_agm_info_t;

/* AGM stat info */
typedef struct bcm_switch_agm_stat_s {
    uint64 packets;         /* accumulated packets value */
    uint64 bytes;           /* accumulated bytes value */
    int period;             /* period number used by current stat */
    bcm_gport_t gport_id;   /* If this AGM is for a Trunk/HiGigTrunk, indicates
                               which trunk member using the counter */
    bcm_if_t intf_id;       /* If this AGM is for an ECMP group, indicates which
                               group member using the counter */
} bcm_switch_agm_stat_t;

/* bcm_switch_agm_traverse_cb */
typedef int (*bcm_switch_agm_traverse_cb)(
    int unit, 
    bcm_switch_agm_info_t *agm_info, 
    void *user_data);

/* Initialize a bcm_switch_agm_info_t structure. */
extern void bcm_switch_agm_info_t_init(
    bcm_switch_agm_info_t *agm_info);

/* Initialize a bcm_switch_agm_stat_t structure. */
extern void bcm_switch_agm_stat_t_init(
    bcm_switch_agm_stat_t *agm_stat);

#ifndef BCM_HIDE_DISPATCHABLE

/* Creates an aggregation monitoring entry. */
extern int bcm_switch_agm_create(
    int unit, 
    uint32 options, 
    bcm_switch_agm_info_t *agm_info);

/* Enable or disable the aggregation group monitoring on a given entry. */
extern int bcm_switch_agm_enable_set(
    int unit, 
    bcm_switch_agm_id_t agm_id, 
    int enable);

/* 
 * Retrieve bandwidth and packet distribution information that were
 * collected via aggregation group monitoring.
 */
extern int bcm_switch_agm_stat_get(
    int unit, 
    bcm_switch_agm_id_t agm_id, 
    int nstat, 
    bcm_switch_agm_stat_t *stat_arr);

/* 
 * Clear stats of counter entries attached to the aggregation monitor
 * (id).
 */
extern int bcm_switch_agm_stat_clear(
    int unit, 
    bcm_switch_agm_id_t agm_id);

/* Destroy an existing aggregation monitoring entry. */
extern int bcm_switch_agm_destroy(
    int unit, 
    bcm_switch_agm_id_t agm_id);

/* Retrieve a (fabric) trunk_id where the monitor entry is attached. */
extern int bcm_switch_agm_trunk_attach_get(
    int unit, 
    bcm_switch_agm_id_t agm_id, 
    bcm_trunk_t *trunk_id);

/* Retrieve a l3 ecmp group id where the monitor entry is attached. */
extern int bcm_switch_agm_l3_ecmp_attach_get(
    int unit, 
    bcm_switch_agm_id_t agm_id, 
    bcm_if_t *l3_ecmp_id);

/* Retrieve an AGM monitor info if it's created. */
extern int bcm_switch_agm_get(
    int unit, 
    bcm_switch_agm_info_t *agm_info);

/* Return a monitor entry's enable status. */
extern int bcm_switch_agm_enable_get(
    int unit, 
    bcm_switch_agm_id_t agm_id, 
    int *enable);

/* 
 * Traverse all aggregation monitor entries and call callback function
 * for each allocated entry.
 */
extern int bcm_switch_agm_traverse(
    int unit, 
    int flags, 
    bcm_switch_agm_traverse_cb trav_fn, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Service type for Match */
typedef enum bcm_switch_match_service_e {
    bcmSwitchMatchServiceMiml = 0,      /* Mac-in-Mac Lite(MiML). Required
                                           fields: inner_ethtype, outer_ethtype. */
    bcmSwitchMatchServiceCustomHeader = 1, /* Custom Header. Required fields:
                                           value32, int_pri, color. */
    bcmSwitchMatchServiceGtp = 2,       /* GPRS Tunnel Protocol packets.
                                           Required fields: match_criteria,
                                           src_port, dst_port. */
    bcmSwitchMatchServicePrpHsrSupervision = 3, /* Seamless Redundancy PRP/HSR
                                           supervision protocol. Required
                                           fields: ethtype1 (for PRP), ethtype2
                                           (for HSR), dst_mac, dst_mac_mask */
    bcmSwitchMatchServiceDot1cbSupervision = 4, /* Seamless Redundancy 802.1CB
                                           supervision protocol. Required
                                           fields: ethtype1, dst_mac,
                                           dst_mac_mask */
    bcmSwitchMatchServiceLinkLocal = 5, /* Link-local MAC addresses. Required
                                           fields: dst_mac, dst_mac_mask */
    bcmSwitchMatchService__Count = 6    /* Must be last. */
} bcm_switch_match_service_t;

/* Types for bcm_switch_match_config_info_t. */
#define BCM_SWITCH_GTP_MATCH_DST_PORT       0x1        /* Match only destination
                                                          port of GTP packets */
#define BCM_SWITCH_GTP_MATCH_SRC_PORT       0x2        /* Match only source port
                                                          of GTP packets */
#define BCM_SWITCH_GTP_MATCH_SRC_OR_DST_PORT 0x3        /* Match source port or
                                                          destination port of
                                                          GTP packets */
#define BCM_SWITCH_GTP_MATCH_SRC_AND_DST_PORT 0x4        /* Match source port and
                                                          destination port of
                                                          GTP packets */

/* 
 * This structure describes the fields that need to be supplies for the
 * configuration along with the property type.
 */
typedef struct bcm_switch_match_config_info_s {
    uint32 value32;         /* 32-bit data value to match */
    uint16 inner_ethtype;   /* 16-bit Inner EtherType to match */
    uint16 outer_ethtype;   /* 16-bit Outer EtherType to match */
    uint8 int_pri;          /* Internal packet priority to set when matched */
    bcm_color_t color;      /* Packet Color to set when matched */
    uint8 match_criteria;   /* Match criteria for GPRS Tunnel Protocol packets */
    uint16 dst_port;        /* Matched destination port */
    uint16 src_port;        /* Matched source port */
    uint16 ethtype1;        /* 16-bit EtherType to match */
    uint16 ethtype2;        /* 16-bit EtherType (alternate) to match */
    bcm_mac_t dst_mac;      /* Destination MAC address to match */
    bcm_mac_t dst_mac_mask; /* Mask for destination MAC address */
} bcm_switch_match_config_info_t;

/* Initialize a bcm_switch_match_config_info_t structure */
extern void bcm_switch_match_config_info_t_init(
    bcm_switch_match_config_info_t *config_info);

#ifndef BCM_HIDE_DISPATCHABLE

/* Add a match config. */
extern int bcm_switch_match_config_add(
    int unit, 
    bcm_switch_match_service_t match_service, 
    bcm_switch_match_config_info_t *config_info, 
    int *match_id);

/* Delete a created match config. */
extern int bcm_switch_match_config_delete(
    int unit, 
    bcm_switch_match_service_t match_service, 
    int match_id);

/* Delete all created match objects. */
extern int bcm_switch_match_config_delete_all(
    int unit, 
    bcm_switch_match_service_t match_service);

/* Set/Get the match config info. */
extern int bcm_switch_match_config_set(
    int unit, 
    bcm_switch_match_service_t match_service, 
    int match_id, 
    bcm_switch_match_config_info_t *config_info);

/* Set/Get the match config info. */
extern int bcm_switch_match_config_get(
    int unit, 
    bcm_switch_match_service_t match_service, 
    int match_id, 
    bcm_switch_match_config_info_t *config_info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* match table traverse callback. */
typedef int (*bcm_switch_match_config_traverse_cb)(
    int unit, 
    int match_id, 
    bcm_switch_match_config_info_t *config_info, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Traverse all created match objects and call callback function for each
 * allocated entry.
 */
extern int bcm_switch_match_config_traverse(
    int unit, 
    bcm_switch_match_service_t match_service, 
    bcm_switch_match_config_traverse_cb cb_fn, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Control type for match */
typedef enum bcm_switch_match_control_type_e {
    bcmSwitchMatchControlPortEnable = 0, /* Enable/disable the capability for
                                           match detection on a port. */
    bcmSwitchMatchControlMatchMask = 1, /* Setup the mask for matching lookup. */
    bcmSwitchMatchControl__Count        /* Must be last. */
} bcm_switch_match_control_type_t;

/* 
 * This structure describes the fields that need to be supplies for the
 * configuration along with the property type.
 */
typedef struct bcm_switch_match_control_info_s {
    int port_enable;    /* Enable or disable (per-port) */
    uint32 mask32;      /* Mask value for matching (32 bits) */
} bcm_switch_match_control_info_t;

/* Initialize a bcm_switch_match_control_info_t structure */
extern void bcm_switch_match_control_info_t_init(
    bcm_switch_match_control_info_t *control_info);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/Get the match control info. */
extern int bcm_switch_match_control_set(
    int unit, 
    bcm_switch_match_service_t match_service, 
    bcm_switch_match_control_type_t control_type, 
    bcm_gport_t gport, 
    bcm_switch_match_control_info_t *control_info);

/* Set/Get the match control info. */
extern int bcm_switch_match_control_get(
    int unit, 
    bcm_switch_match_service_t match_service, 
    bcm_switch_match_control_type_t control_type, 
    bcm_gport_t gport, 
    bcm_switch_match_control_info_t *control_info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* match control traverse callback. */
typedef int (*bcm_switch_match_control_traverse_cb)(
    int unit, 
    bcm_switch_match_control_type_t control_type, 
    bcm_gport_t gport, 
    bcm_switch_match_control_info_t *control_info, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Traverse all match control info and call callback function for each
 * allocated entry.
 */
extern int bcm_switch_match_control_traverse(
    int unit, 
    bcm_switch_match_service_t match_service, 
    bcm_switch_match_control_traverse_cb cb_fn, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Encap service type. */
typedef enum bcm_switch_encap_service_e {
    BCM_SWITCH_ENCAP_SERVICE_MIML = 0,  /* Mac-in-Mac Lite(MiML). Required
                                           fields: egress_vlan, egress_srcmac,
                                           egress_dstmac, pkt_pri, pkt_cfi,
                                           pkt_type, iid and length. */
    BCM_SWITCH_ENCAP_SERVICE_CUSTOM_HEADER = 1, /* Custom Header. Required fields:
                                           value32. */
    BCM_SWITCH_ENCAP_SERVICE_COUNT      /* Must be last */
} bcm_switch_encap_service_t;

/* 
 * Encap info structure.
 * 
 * The following parameters are required when the encap type is MIML:
 * egress_vlan, egress_srcmac, egress_dstmac, pkt_pri, pkt_cfi, pkt_type,
 * iid, length and port.
 * 
 * The following parameters are required when the encap type is
 * CUSTOM_HEADER: value32.
 * 
 * The other optional configurations depend on "flags" definition per
 * service: e.g. qos_map_id , higig_vlan.
 */
typedef struct bcm_switch_encap_info_s {
    bcm_switch_encap_service_t encap_service; /* Encapsulation Service Type. */
    uint32 flags;                       /* Configuration flags. */
    uint32 value32;                     /* 32-bit data value for encapsulation. */
    int qos_map_id;                     /* QoS Mapped priority. */
    bcm_gport_t port;                   /* Physical port/trunk, which is must
                                           set for MiML to indicate the
                                           destination port. */
    bcm_vlan_t egress_vlan;             /* VID. */
    bcm_mac_t egress_srcmac;            /* Source MAC address. */
    bcm_mac_t egress_dstmac;            /* Dest MAC address. */
    uint8 pkt_pri;                      /* Packet priority. */
    uint8 pkt_cfi;                      /* Packet cfi. */
    uint8 pkt_type;                     /* MiML Packet Type. */
    uint16 iid;                         /* MiML IID. */
    uint16 length;                      /* MiML Length. */
    bcm_vlan_t higig_vlan;              /* MiML HiGig header OVID. */
} bcm_switch_encap_info_t;

/* Switch encap flags for Custom Header. */
#define BCM_SWITCH_ENCAP_CUSTOM_HEADER_QOS_MAP_INT_PRI (1 << 0)   /* Encapsulation priority
                                                          control per entry, map
                                                          priority. Required
                                                          fields: qos_map_id. */

/* Switch encap flags for MiML. */
#define BCM_SWITCH_ENCAP_MIML_QOS_MAP_INT_PRI (1 << 0)   /* Encapsulation priority
                                                          control per entry, map
                                                          priority. Required
                                                          fields: qos_map_id. */
#define BCM_SWITCH_ENCAP_MIML_OVERRIDE_HG_HEADER_VID (1 << 1)   /* Use higig_vlan value
                                                          to override OVID field
                                                          in HiGig header.
                                                          Required fields:
                                                          higig_vlan. */

/* Initialize a bcm_switch_encap_info_t structure */
extern void bcm_switch_encap_info_t_init(
    bcm_switch_encap_info_t *encap_info);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create an encapuslation object. */
extern int bcm_switch_encap_create(
    int unit, 
    bcm_switch_encap_info_t *encap_info, 
    bcm_if_t *encap_id);

/* Destroy previously created encapuslation object. */
extern int bcm_switch_encap_destroy(
    int unit, 
    bcm_if_t encap_id);

/* Destroy all encap objects. */
extern int bcm_switch_encap_destroy_all(
    int unit);

/* Modify the configuration of the encap object with the given encap_id. */
extern int bcm_switch_encap_set(
    int unit, 
    bcm_if_t encap_id, 
    bcm_switch_encap_info_t *encap_info);

/* Get the configuration of the encap object with the given encap_id. */
extern int bcm_switch_encap_get(
    int unit, 
    bcm_if_t encap_id, 
    bcm_switch_encap_info_t *encap_info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Switch encap traverse callback. */
typedef int (*bcm_switch_encap_traverse_cb)(
    int unit, 
    bcm_if_t encap_id, 
    bcm_switch_encap_info_t *encap_info, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Traverse all created encapsulation objects and call callback function
 * for each allocated entry.
 */
extern int bcm_switch_encap_traverse(
    int unit, 
    bcm_switch_encap_traverse_cb cb_fn, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Vendor Info Struct */
typedef struct bcm_switch_dram_vendor_info_s {
    uint32 dram_density;    /* Dram Density */
    uint32 fifo_depth;      /* FIFO Depth */
    uint32 revision_id;     /* Revision Id */
    uint32 manufacture_id;  /* Manufacture Id */
} bcm_switch_dram_vendor_info_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get dram vendor info */
extern int bcm_switch_dram_vendor_info_get(
    int unit, 
    bcm_switch_dram_vendor_info_t *info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Thermo sensors enum */
typedef enum bcm_switch_thermo_sensor_type_e {
    bcmSwitchThermoSensorInvalid = 0, 
    bcmSwitchThermoSensorDram = 1, 
    bcmSwitchThermoSensorSpare1 = 2, 
    bcmSwitchThermoSensorSpare2 = 3, 
    bcmSwitchThermoSensorCount = 4 
} bcm_switch_thermo_sensor_type_t;

/* Thermo Sensor Struct */
typedef struct bcm_switch_thermo_sensor_s {
    int thermo_sensor_min;      /* thermo sensor minimal read */
    int thermo_sensor_max;      /* thermo sensor maximal read */
    int thermo_sensor_current;  /* thermo sensor current read */
} bcm_switch_thermo_sensor_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get switch thermo sensor data */
extern int bcm_switch_thermo_sensor_read(
    int unit, 
    bcm_switch_thermo_sensor_type_t sensor_type, 
    int interface_id, 
    bcm_switch_thermo_sensor_t *sensor_data);

/* An API to get the Recommended Operational Value. */
extern int bcm_switch_rov_get(
    int unit, 
    uint32 flags, 
    uint32 *rov);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_SWITCH_SDK_TO_WARMBOOT_VERSION_GET(_major_release, _minor_release, _patch_level, _warmboot_ver)  \
    (_warmboot_ver) = ((((_major_release) & 0xFF) << 16 ) | (((_minor_release) & 0xFF) << 8 ) | (((_patch_level) & 0xFF))) 

/* Nibble speculation. */
#define BCM_SWITCH_DEFAULT_NIBBLE_INDEX 16         /* Used to configure the
                                                      nibbles default speculated
                                                      protocol. */

/* 
 * Enumeration values that can be passed to API in order to set expected
 * protocol after MPLS bos label based on nibble value.
 */
typedef enum bcm_switch_mpls_next_protocol_e {
    bcmSwitchMplsNextProtocolDefault = 0, /* Use the default speculation protocol. */
    bcmSwitchMplsNextProtocolNone = 1,  /* Do not speculate any protocol. */
    bcmSwitchMplsNextProtocolEthernet = 2, /* Speculate Ethernet after the MPLS
                                           stack. */
    bcmSwitchMplsNextProtocolIpv4 = 3,  /* Speculate IPv4 protocol after the
                                           MPLS stack. */
    bcmSwitchMplsNextProtocolIpv6 = 4,  /* Speculate IPv6 protocol after the
                                           MPLS stack. */
    bcmSwitchMplsNextProtocolControlWord = 5, /* Speculate CW protocol aftre the MPLS
                                           stack. */
    bcmSwitchMplsNextProtocolBierMpls = 6, /* Speculate BIER MPLS protocol after
                                           the MPLS stack */
    bcmSwitchMplsNextProtocolBierTi = 7, /* Speculate TI MPLS protocol after the
                                           MPLS stack */
    bcmSwitchMplsNextProtocolCount = 8  /* Should be last element in enum. */
} bcm_switch_mpls_next_protocol_t;

/* 
 * LPM hit bit table ids to be used with
 * bcmSwitchL3LpmHitbitEnable/Disable.
 */
typedef enum bcm_switch_l3_lpm_hitbit_tables_e {
    bcmL3LpmHitbitTableIPv4UC = 0,      /* IPv4 UC table */
    bcmL3LpmHitbitTableIPv4MC = 1,      /* IPv4 MC table. */
    bcmL3LpmHitbitTableIPv6UC = 2,      /* IPv6 UC table */
    bcmL3LpmHitbitTableIPv6MC = 3,      /* IPv6 MC table */
    bcmL3LpmHitbitTableIPv4UCScale = 4, /* IPv4 UC scale table */
    bcmL3LpmHitbitTableIPv6UCScale = 5, /* IPv6 UC scale table */
    bcmL3LpmHitbitTableCount = 6        /* Always Last, not a usable value */
} bcm_switch_l3_lpm_hitbit_tables_t;

/* Switch module IDs */
typedef enum bcm_switch_module_e {
    bcmModuleL3EgressFec = 0,   /* FEC module */
    bcmModuleL3EgressEcmp = 1,  /* ECMP module */
    bcmModuleCount = 2          /* Always Last, not a usable value */
} bcm_switch_module_t;

#define BCM_SWITCH_L2_FREEZE_MODE_PORT      0          /* Reconfigure CML for
                                                          each port to stop
                                                          hardware learning. */
#define BCM_SWITCH_L2_FREEZE_MODE_GLOBAL    0x1        /* Use global CML
                                                          override setting to
                                                          put all ports in
                                                          forward. */

/* Chip info types */
typedef enum bcm_switch_chip_info_e {
    bcmSwitchChipInfoIcid = 0,  /* ICID - IC identification number. */
    bcmSwitchChipInfoCount      /* Must be last.  Not a usable value. */
} bcm_switch_chip_info_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get the specified chip info. */
extern int bcm_switch_chip_info_get(
    int unit, 
    bcm_switch_chip_info_t info_type, 
    int max_size, 
    void *data_buf, 
    int *actual_size);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Block type */
typedef enum bcm_switch_block_type_e {
    bcmSwitchBlockTypeAll = -1,     /* All block types */
    bcmSwitchBlockTypeMmu = 0,      /* Memory Management Unit */
    bcmSwitchBlockTypeMmuGlb = 1,   /* MMU(Global) */
    bcmSwitchBlockTypeMmuXpe = 2,   /* MMU(Crosspoint Element) */
    bcmSwitchBlockTypeMmuSc = 3,    /* MMU(Slice Control) */
    bcmSwitchBlockTypeMmuSed = 4,   /* MMU(Slice Enqueue Dequeue) */
    bcmSwitchBlockTypeIpipe = 5,    /* Ingress pipeline */
    bcmSwitchBlockTypeEpipe = 6,    /* Egress pipeline */
    bcmSwitchBlockTypePgw = 7,      /* Packet GateWay */
    bcmSwitchBlockTypeClport = 8,   /* CLPORT */
    bcmSwitchBlockTypeXlport = 9,   /* XLPORT */
    bcmSwitchBlockTypeMacsec = 10,  /* MACSEC */
    bcmSwitchBlockTypePort = 11,    /* PORT */
    bcmSwitchBlockTypeCount = 12    /* Must be last. */
} bcm_switch_block_type_t;

/* SER error type */
typedef enum bcm_switch_error_type_e {
    bcmSwitchErrorTypeAll = -1,         /* All error types */
    bcmSwitchErrorTypeUnknown = 0,      /* Unknown error type */
    bcmSwitchErrorTypeParity = 1,       /* Parity error */
    bcmSwitchErrorTypeEccSingleBit = 2, /* Ecc single bit error */
    bcmSwitchErrorTypeEccDoubleBit = 3, /* Ecc double bit error */
    bcmSwitchErrorTypeCount = 4         /* Must be last. */
} bcm_switch_error_type_t;

/* SER error correction type */
typedef enum bcm_switch_correction_type_e {
    bcmSwitchCorrectTypeAll = -1,       /* All correction types */
    bcmSwitchCorrectTypeNoAction = 0,   /* S/W takes no action when error
                                           happens (Like some working memories) */
    bcmSwitchCorrectTypeFailToCorrect = 1, /* S/W tries to correct error but fails */
    bcmSwitchCorrectTypeEntryClear = 2, /* S/W writes NULL entry to clear the
                                           error */
    bcmSwitchCorrectTypeCacheRestore = 3, /* Restore entry from a valid S/W cache */
    bcmSwitchCorrectTypeHwCacheRestore = 4, /* Restore entry from another pipe */
    bcmSwitchCorrectTypeSpecial = 5,    /* Memory needs special correction
                                           handling */
    bcmSwitchCorrectTypeCount = 6       /* Must be last. */
} bcm_switch_correction_type_t;

/* SER log info flags */
#define BCM_SWITCH_SER_LOG_MEM          0x00000001 /* Error happens on memory */
#define BCM_SWITCH_SER_LOG_REG          0x00000002 /* Error happens on register */
#define BCM_SWITCH_SER_LOG_MULTI        0x00000004 /* Parity errors detected
                                                      more than once */
#define BCM_SWITCH_SER_LOG_CORRECTED    0x00000008 /* Error be corrected by S/W */
#define BCM_SWITCH_SER_LOG_ENTRY        0x00000010 /* Corrupt entry data is
                                                      valid */
#define BCM_SWITCH_SER_LOG_CACHE        0x00000020 /* Cache data is valid */

/* This structure describes SER log info. */
typedef struct bcm_switch_ser_log_info_s {
    uint32 time;                        /* Error detected time */
    uint32 addr;                        /* Error happened address */
    uint32 flags;                       /* SER log info flags */
    int mem_reg;                        /* Error happened memory or register id */
    uint8 name[BCM_MAX_MEM_REG_NAME];   /* Error happened memory or register
                                           name */
    int index;                          /* Error happened memory/register index */
    int port;                           /* Error happened port id */
    int pipe;                           /* Pipe num */
    int instance;                       /* Instance id (MMU) */
    bcm_switch_block_type_t block_type; /* Block type */
    bcm_switch_error_type_t error_type; /* Error type */
    bcm_switch_correction_type_t correction_type; /* Correction type */
    uint32 entry_data[BCM_MAX_MEM_WORDS]; /* Corrupted entry data */
    uint32 cache_data[BCM_MAX_MEM_WORDS]; /* Cache entry data */
} bcm_switch_ser_log_info_t;

/* An API to Initialize a bcm_switch_ser_log_info_t structure. */
extern void bcm_switch_ser_log_info_t_init(
    bcm_switch_ser_log_info_t *info);

#ifndef BCM_HIDE_DISPATCHABLE

/* An API to get ser log info. */
extern int bcm_switch_ser_log_info_get(
    int unit, 
    int id, 
    bcm_switch_ser_log_info_t *info);

extern int bcm_switch_control_indexed_port_set(
    int unit, 
    bcm_port_t port, 
    bcm_switch_control_key_t key, 
    bcm_switch_control_info_t value);

extern int bcm_switch_control_indexed_port_get(
    int unit, 
    bcm_port_t port, 
    bcm_switch_control_key_t key, 
    bcm_switch_control_info_t *value);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_SWITCH_CONTROL_SYNC_FULL    0xFFFFFFFF /* Used as arg value in API
                                                      bcm_switch_control_set,
                                                      type=bcmSwitchControlSync.
                                                      indicates of doing full
                                                      sequence of sw state sync.
                                                      meaning stop activities
                                                      that may write into the sw
                                                      state before the sync is
                                                      made */

/* SER error statistics flags */
#define BCM_SWITCH_SER_STAT_TCAM    0x00000001 /* Error happens on TCAM memories */

/* This structure describes SER error statistics type. */
typedef struct bcm_switch_ser_error_stat_type_s {
    uint32 flags;                       /* SER error statistics flags */
    bcm_switch_block_type_t block_type; /* Block type */
    bcm_switch_error_type_t error_type; /* SER error type */
    bcm_switch_correction_type_t correction_type; /* SER error correction type */
} bcm_switch_ser_error_stat_type_t;

/* An API to initialize a bcm_switch_ser_error_stat_type_t structure. */
extern void bcm_switch_ser_error_stat_type_t_init(
    bcm_switch_ser_error_stat_type_t *stat_type);

#ifndef BCM_HIDE_DISPATCHABLE

/* API to get SER error statistics for specified statistics type. */
extern int bcm_switch_ser_error_stat_get(
    int unit, 
    bcm_switch_ser_error_stat_type_t stat_type, 
    uint32 *value);

/* API to clear SER error statistics for specified statistics type. */
extern int bcm_switch_ser_error_stat_clear(
    int unit, 
    bcm_switch_ser_error_stat_type_t stat_type);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_SWITCH_MAX_NUM_OPAQUE_TAGS      2          /* Maximum number of
                                                          opaque tags supported
                                                          by the device. */
#define BCM_SWITCH_OPAQUE_TAG_SIZE_4_BYTES  0x1        /* Tag size of 4 bytes */
#define BCM_SWITCH_OPAQUE_TAG_SIZE_8_BYTES  0x2        /* Tag size of 8 bytes */

/* Opaque tag parameters. */
typedef struct bcm_switch_opaque_tag_params_s {
    uint16 ethertype;   /* EtherType to match when parsing for opaque tag */
    uint8 tag_size;     /* Size of opaque tag (Only 1, 2 are valid values) */
    uint8 tag_type;     /* User specified 2-bit value (0 is invalid value) */
    uint8 valid;        /* Make config active(1) or not active in h/w(0) */
} bcm_switch_opaque_tag_params_t;

/* Initialize bcm_switch_opaque_tag_params_t structure */
extern void bcm_switch_opaque_tag_config_t_init(
    bcm_switch_opaque_tag_params_t *info);

#ifndef BCM_HIDE_DISPATCHABLE

/* Configure opaque tag parameters. */
extern int bcm_switch_opaque_tag_config_set(
    int unit, 
    int index, 
    bcm_switch_opaque_tag_params_t *opaque_tag_config);

/* Configure payload opaque tag parameters. */
extern int bcm_switch_payload_opaque_tag_config_set(
    int unit, 
    int index, 
    bcm_switch_opaque_tag_params_t *opaque_tag_config);

/* Read opaque tag parameters configured for a given set. */
extern int bcm_switch_opaque_tag_config_get(
    int unit, 
    int index, 
    bcm_switch_opaque_tag_params_t *opaque_tag_config);

/* Get payload opaque tag parameters. */
extern int bcm_switch_payload_opaque_tag_config_get(
    int unit, 
    int index, 
    bcm_switch_opaque_tag_params_t *opaque_tag_config);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_SWITCH_OBM_PRIORITY_LOSSY_LOW   0          /* This obm_priority is
                                                          mapped to OBM buffer
                                                          partition Lossy Low */
#define BCM_SWITCH_OBM_PRIORITY_LOSSY_HIGH  1          /* This obm_priority is
                                                          mapped to OBM buffer
                                                          partition Lossy High */
#define BCM_SWITCH_OBM_PRIORITY_LOSSLESS0   2          /* This obm_priority is
                                                          mapped to OBM buffer
                                                          partition Lossless0 */
#define BCM_SWITCH_OBM_PRIORITY_LOSSLESS1   3          /* This obm_priority is
                                                          mapped to OBM buffer
                                                          partition Lossless1 */
#define BCM_SWITCH_OBM_PRIORITY_EXPRESS     4          /* This obm_priority is
                                                          mapped to OBM buffer
                                                          partition Express */

/* OBM classifier code point */
typedef int bcm_switch_obm_code_point_t;

/* OBM classifier ob_priority */
typedef int bcm_switch_obm_priority_t;

/* OBM classifier types */
typedef enum bcm_switch_obm_classifier_type_e {
    bcmSwitchObmClassifierDscp = 0,     /* OBM classifier of type DSCP */
    bcmSwitchObmClassifierMpls = 1,     /* OBM classifier of type MPLS */
    bcmSwitchObmClassifierEtag = 2,     /* OBM classifier of type ETAG */
    bcmSwitchObmClassifierVlan = 3,     /* OBM classifier of type VLAN */
    bcmSwitchObmClassifierHigig2 = 4,   /* OBM classifier of type HiGig2 */
    bcmSwitchObmClassifierField = 5,    /* OBM classifier of type FIELD */
    bcmSwitchObmClassifierHigig3 = 6,   /* OBM classifier of type Higig3 */
    bcmSwitchObmClassifierCount = 7     /* OBM classifier count */
} bcm_switch_obm_classifier_type_t;

/* OBM classifier parameters */
typedef struct bcm_switch_obm_classifier_s {
    uint32 flags;                       /* For future use */
    bcm_mac_t obm_destination_mac;      /* Destination MAC address */
    bcm_mac_t obm_destination_mac_mask; /* Destination MAC address mask */
    uint16 obm_ethertype;               /* EtherType to be matched */
    uint16 obm_ethertype_mask;          /* Mask used in matching EtherType */
    bcm_switch_obm_code_point_t obm_code_point; /*  Oversubscription Buffer Manager
                                           classifier's code point.This field if
                                           overloaded for providing DSCP, MPLS
                                           traffic class, ETAG priority code
                                           point, VLAN priority code point or
                                           HiGig2 traffic class depending on the
                                           type of classifier. This  will be
                                           mapped to obm_priority value. */
    bcm_switch_obm_priority_t obm_priority; /* Oversubscription Buffer Manager
                                           classifier's priority for given
                                           obm_code_point. */
} bcm_switch_obm_classifier_t;

/* Initialize an OBM classifier structure. */
extern void bcm_switch_obm_classifier_t_init(
    bcm_switch_obm_classifier_t *switch_obm_classifier);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Set the mapping from code_point, Dest MAC and EtherType to a
 * obm_priority based on OBM classifier type.For FIELD classifier always
 * first entry is used.
 */
extern int bcm_switch_obm_classifier_mapping_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_switch_obm_classifier_type_t switch_obm_classifier_type, 
    bcm_switch_obm_classifier_t *switch_obm_classifier);

/* 
 * Get the mapping from code_point, Dest MAC and EtherType to a
 * obm_priority based on OBM classifier type.
 */
extern int bcm_switch_obm_classifier_mapping_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_switch_obm_classifier_type_t switch_obm_classifier_type, 
    bcm_switch_obm_classifier_t *switch_obm_classifier);

/* 
 * Set multiple mapping from code_point, Dest MAC and EtherType to a
 * obm_priority based on OBM classifier type.
 */
extern int bcm_switch_obm_classifier_mapping_multi_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_switch_obm_classifier_type_t switch_obm_classifier_type, 
    int array_count, 
    bcm_switch_obm_classifier_t *switch_obm_classifier);

/* 
 * Get multiple mapping from port, Dest MAC and EtherType to a
 * obm_priority based on OBM classifier type.
 */
extern int bcm_switch_obm_classifier_mapping_multi_get(
    int unit, 
    bcm_gport_t port, 
    bcm_switch_obm_classifier_type_t switch_obm_classifier_type, 
    int array_max, 
    bcm_switch_obm_classifier_t *switch_obm_classifier, 
    int *array_count);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Switch Dos Attack Events. */
typedef enum bcm_switch_dosattack_event_e {
    bcmSwitchDosAttackEventMACSAEqualMACDA = 0, /* Dos Attack Event when Source MAC is
                                           same as Destination MAC. */
    bcmSwitchDosAttackEventIpFragOffset = 1, /* Dos Attack Event when Protocol field
                                           Offset in IPv4 or IPv6 header is
                                           equal to 1. */
    bcmSwitchDosAttackEventV6Type0RoutingHdr = 2, /* Dos Attack Event when Type0 Routing
                                           Extension header is present in IPv6. */
    bcmSwitchDosAttackEventV6NonLastFragLT1280 = 3, /* Dos Attack Event when non-last
                                           fragment of IPv6 Packet is less than
                                           1280 bytes. */
    bcmSwitchDosAttackEventTcpIpFirstFragSmall = 4, /* Dos Attack Event when First Fragment
                                           of IP payload with TCP is smaller
                                           than configured value. */
    bcmSwitchDosAttackEventTcpOffset = 5, /* Dos Attack Event when L4 is TCP and
                                           IP protocol offset is equal to 1. */
    bcmSwitchDosAttackEventSynFrag = 6, /* Dos Attack Event when TCP SYN Packet
                                           with Source Port 0-1023 for non-first
                                           fragments tcp/udp port values are
                                           always zero. */
    bcmSwitchDosAttackEventFlagZeroSeqZero = 7, /* Dos Attack Event when TCP Packets
                                           with control flags == 0 and sequence
                                           number == 0. */
    bcmSwitchDosAttackEventTcpFlagsFUP = 8, /* Dos Attack Event when TCP Packets
                                           with FIN,URG & PSH bits set &
                                           sequence number == 0. */
    bcmSwitchDosAttackEventTcpFlagsSF = 9, /* Dos Attack Event when TCP packets
                                           with SYN and FIN set. */
    bcmSwitchDosAttackEventTcpPortsEqual = 10, /* Dos Attack Event when TCP Source and
                                           Destination ports are same. */
    bcmSwitchDosAttackEventUdpPortsEqual = 11, /* Dos Attack Event when UDP Source and
                                           Destination ports are same. */
    bcmSwitchDosAttackEventIcmp = 12,   /* Dos Attack Event when ICMP V4 and V6
                                           ping packet with payload bigger than
                                           configured value. */
    bcmSwitchDosAttackEventIcmpFragments = 13, /* Dos Attack Event when IPv4/IPv6
                                           fragmented ICMP packets. */
    bcmSwitchDosAttackEventTcpFlagsSR = 14, /* Dos Attack Event when TCP packets
                                           with SYN and RST set. */
    bcmSwitchDosAttackEventTcpFlagsSynFirstFrag = 15, /* Dos Attack Event when TCP with SYN
                                           set and first fragment of fragmented
                                           IP packet. */
    bcmSwitchDosAttackEventTcpFlagsFinNoAck = 16, /* Dos Attack Event when TCP packet with
                                           FIN set but ACK not set. */
    bcmSwitchDosAttackEventTcpFlagsRstNoAck = 17, /* Dos Attack Event when TCP packet with
                                           RST set but ACK not set. */
    bcmSwitchDosAttackEventTcpFlagsNoSFRA = 18, /* Dos Attack Event when TCP packet with
                                           SYN, FIN, RST and ACK not set. */
    bcmSwitchDosAttackEventTcpDstPortZero = 19, /* Dos Attack Event when TCP packet with
                                           Destination port equals to 0. */
    bcmSwitchDosAttackEventTcpSrcPortZero = 20, /* Dos Attack Event when TCP packet with
                                           Source port equals to 0. */
    bcmSwitchDosAttackEventTcpIpDstBcast = 21, /* Dos Attack Event when TCP Packet with
                                           Destination IP Broadcast. */
    bcmSwitchDosAttackEventTcpFlagsZero = 22, /* Dos Attack Event when configured TCP
                                           flags is zero. */
    bcmSwitchDosAttackEventTcpFlagsResvdNonZero = 23, /* Dos Attack Event when configured
                                           Reserved flags is non-zero. */
    bcmSwitchDosAttackEventTcpFlagsAck = 24, /* Dos Attack Event when TCP flag ACK is
                                           set and Acknowledge number is zero. */
    bcmSwitchDosAttackEventTcpFlagsUrgNoUrgPtr = 25, /* Dos Attack Event when TCP Flag URG
                                           set and Urgent pointer is zero. */
    bcmSwitchDosAttackEventTcpFlagsNoUrgWithUrgPtr = 26, /* Dos Attack Event when TCP Flag URG
                                           not set and Urgent pointer is not
                                           zero. */
    bcmSwitchDosAttackEventTcpFlagsSynOption = 27, /* Dos Attack Event when TCP Flag SYN
                                           set and Option same as configured
                                           value. */
    bcmSwitchDosAttackEventCount = 28   /* Must be last. Not a usable value. */
} bcm_switch_dosattack_event_t;

#define BCM_SWITCH_DOSATTACK_EVENT_STR \
{ \
    "MACSAEqualMACDA", \
    "IpFragOffset", \
    "V6Type0RoutingHdr", \
    "V6NonLastFragLT1280", \
    "TcpIpFirstFragSmall", \
    "TcpOffset", \
    "SynFrag", \
    "FlagZeroSeqZero", \
    "TcpFlagsFUP", \
    "TcpFlagsSF", \
    "TcpPortsEqual", \
    "UdpPortsEqual", \
    "Icmp", \
    "IcmpFragments", \
    "TcpFlagsSR", \
    "TcpFlagsSynFirstFrag", \
    "TcpFlagsFinNoAck", \
    "TcpFlagsRstNoAck", \
    "TcpFlagsNoSFRA", \
    "TcpDstPortZero", \
    "TcpSrcPortZero", \
    "TcpIpDstBcast", \
    "TcpFlagsZero", \
    "TcpFlagsResvdNonZero", \
    "TcpFlagsAck", \
    "TcpFlagsUrgNoUrgPtr", \
    "TcpFlagsNoUrgWithUrgPtr", \
    "TcpFlagsSynOption"  \
}

/* Switch Dos Attack Events Flags. */
#define BCM_SWITCH_DOSATTACK_OUTER_HDR      (1 << 0)   /* Dos Attack events
                                                          origin in outer header
                                                          of packet. */
#define BCM_SWITCH_DOSATTACK_INNER_HDR      (1 << 1)   /* Dos Attack events
                                                          origin in inner header
                                                          of packet. */
#define BCM_SWITCH_DOSATTACK_FLOWTRACKER_PRESEL (1 << 2)   /* Dos Attack events used
                                                          for qualifying in
                                                          presel of flowtracker
                                                          FP stage. */

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * These API set/get dos attack event triggers for tracking or qualifying
 * purposes.
 */
extern int bcm_switch_dosattack_event_get(
    int unit, 
    uint32 flags, 
    bcm_switch_dosattack_event_t event, 
    int *arg);

/* 
 * These API set/get dos attack event triggers for tracking or qualifying
 * purposes.
 */
extern int bcm_switch_dosattack_event_set(
    int unit, 
    uint32 flags, 
    bcm_switch_dosattack_event_t event, 
    int arg);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Tunnel UDP src port select */
typedef enum bcm_switch_tunnel_udp_src_port_select_e {
    bcmSwitchTunnelUdpSrcPortStatic = 0, /* Use static value configured in tunnel
                                           initiator */
    bcmSwitchTunnelUdpSrcPortHashEntropy = 1, /* Calculate hash entropy to range [0,
                                           65535] as src port */
    bcmSwitchTunnelUdpSrcPortHashEntropyPrivate = 2, /* Calculate hash entropy to private
                                           range [49152, 65535] as src port */
    bcmSwitchTunnelUdpSrcPortCount = 3  /* Must be last */
} bcm_switch_tunnel_udp_src_port_select_t;

#define BCM_MAX_NOF_SEGMENTS    16         /* Max number of segment for kbp
                                              master key. */
#define BCM_ALIGN_MASTER_KEY    80         /* Align kbp master key to 80b. */
#define BCM_SEGMENT_NAME_SIZE   20         /* Segment name size. */

/* Kbp master key segment info. */
typedef struct bcm_switch_kbp_master_key_segment_info_s {
    uint8 *segment_name;    /* segment name */
    int segment_size_bits;  /* segment size in bits */
    int segment_type;       /* segment type */
} bcm_switch_kbp_master_key_segment_info_t;

/* Switch kbp app type info structure. */
typedef struct bcm_switch_kbp_apptype_info_s {
    int *inst_p;                        /* kbp_instruction_t* instruction
                                           pointer */
    int *fwd_db;                        /* struct kbp_db* fwd db */
    int *rpf_db;                        /* struct kbp_db* rpf db */
    int *dummy_db;                      /* struct kbp_db* dummy db */
    int *fwd_ad_db;                     /* struct kbp_ad_db* ad fwd db */
    int *rpf_ad_db;                     /* struct kbp_ad_db* ad rpf db */
    int *dummy_ad_db;                   /* struct kbp_ad_db* ad dummy db */
    bcm_switch_kbp_master_key_segment_info_t master_key_segment[BCM_MAX_NOF_SEGMENTS]; /* master key fields segments */
    int total_segments_size;            /* total segments size in bits */
    int opcode;                         /* opcode */
    int ltr_index;                      /* ltr_index */
    int master_key_nof_segments;        /* master key fields number of segments */
    int is_valid;                       /* is apptype valid */
} bcm_switch_kbp_apptype_info_t;

#define BCM_KBP_MAX_NOF_THREADS 2          /* KBP MAX NOF threads. */

/* Switch kbp info structure. */
typedef struct bcm_switch_kbp_info_s {
    int *device_p;                      /* struct kbp_device* device_p */
    int *thread_p[BCM_KBP_MAX_NOF_THREADS]; /* kbp thread pointers */
} bcm_switch_kbp_info_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get kbp pointer from SDK. */
extern int bcm_switch_kbp_info_get(
    int unit, 
    bcm_core_t core, 
    bcm_switch_kbp_info_t *kbp_info);

/* Get apptype info per apptype. */
extern int bcm_switch_kbp_apptype_info_get(
    int unit, 
    bcm_core_t core, 
    bcm_field_AppType_t apptype, 
    bcm_switch_kbp_apptype_info_t *apptype_info);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_SWITCH_RECORD_MODE_LSB  0          /* Use the LSB bits of the layer
                                                  record for load balancing key
                                                  generation. */
#define BCM_SWITCH_RECORD_MODE_MSB  1          /* Use the MSB bits of the layer
                                                  record for load balancing key
                                                  generation. */

/* Callback function used to power down the DRAM */
typedef void (*bcm_switch_dram_power_down_callback_t)(
    int unit, 
    uint32 flags, 
    void *userdata);

#ifndef BCM_HIDE_DISPATCHABLE

/* Registers/Unregisters a callback function for switch critical events */
extern int bcm_switch_dram_power_down_cb_register(
    int unit, 
    uint32 flags, 
    bcm_switch_dram_power_down_callback_t callback, 
    void *userdata);

/* Registers/Unregisters a callback function for switch critical events */
extern int bcm_switch_dram_power_down_cb_unregister(
    int unit, 
    bcm_switch_dram_power_down_callback_t callback, 
    void *userdata);

/* API activates DRAM power down procedure */
extern int bcm_switch_dram_power_down(
    int unit, 
    uint32 flags);

/* 
 * DRAM partial init (after device power up) when powering back up the
 * DRAM after it was powered down
 */
extern int bcm_switch_dram_init(
    int unit, 
    uint32 flags);

/* API enable/disable redirecting traffic to DRAM */
extern int bcm_switch_dram_traffic_enable_set(
    int unit, 
    uint32 flags, 
    uint32 enable);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Allocate encap for reflector with ID */
#define BCM_SWITCH_REFELCTOR_WITH_ID    0x1        

/* Replace reflector encap data */
#define BCM_SWITCH_REFELCTOR_REPLACE    0x2        

/* Reflector types */
typedef enum bcm_switch_reflector_type_e {
    bcmSwitchReflectorUc = 0,           /* L2(UC external) or L3 (UC internal)
                                           reflector */
    bcmSwitchReflectorL2UcInternal = 1, /* L2 internal unicast reflector */
    bcmSwitchReflectorL2McExternal = 2, /* L2 external multicast reflector */
    bcmSwitchReflectorL2McInternal = 3, /* L2 external multicast reflector */
    bcmSwitchReflectorTwampIp4 = 4,     /* TWAMP reflector over IPv4 */
    bcmSwitchReflectorTwampIp6 = 5,     /* TWAMP reflector over IPv6 */
    bcmSwitchReflectorSbfdIp4 = 6,      /* SBFD reflector over IPv4 */
    bcmSwitchReflectorSbfdIp6 = 7,      /* SBFD reflector over IPv6 */
    bcmSwitchReflectorSbfdMpls = 8      /* SBFD reflector over MPLS */
} bcm_switch_reflector_type_t;

/* Reflector encap data */
typedef struct bcm_switch_reflector_data_s {
    bcm_switch_reflector_type_t type;   /* reflector type */
    bcm_mac_t mc_reflector_my_mac;      /* Source MAC address stamped on the
                                           response packet. Relevant for MC
                                           reflector only. */
    uint32 error_estimate;              /* ERROR_ESTIMATE */
    bcm_if_t next_encap_id;             /* Next pointer encap id for the RCH
                                           Ethernet encapsulation */
} bcm_switch_reflector_data_t;

/* data bcm_switch_reflector_data_t initialization */
extern void bcm_switch_reflector_data_t_init(
    bcm_switch_reflector_data_t *data);

/* Callback function for reflector traverse */
typedef void (*bcm_switch_reflector_traverse_cb)(
    int unit, 
    bcm_if_t encap_id, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Create/Destroy/Get encap ID for L2/L3 relfectors (both unicast and
 * multicast) .
 */
extern int bcm_switch_reflector_create(
    int unit, 
    uint32 flags, 
    bcm_if_t *encap_id, 
    bcm_switch_reflector_data_t *data);

/* 
 * Create/Destroy/Get encap ID for L2/L3 relfectors (both unicast and
 * multicast) .
 */
extern int bcm_switch_reflector_get(
    int unit, 
    uint32 flags, 
    bcm_if_t encap_id, 
    bcm_switch_reflector_data_t *data);

/* 
 * Create/Destroy/Get encap ID for L2/L3 relfectors (both unicast and
 * multicast) .
 */
extern int bcm_switch_reflector_destroy(
    int unit, 
    uint32 flags, 
    bcm_if_t encap_id);

/* 
 * Create/Destroy/Get encap ID for L2/L3 relfectors (both unicast and
 * multicast) .
 */
extern int bcm_switch_reflector_traverse(
    int unit, 
    uint32 flags, 
    bcm_switch_reflector_traverse_cb traverse_cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Packet control options flags, common for integrity check API and
 * protocol control API.
 */
#define BCM_SWITCH_PKT_CONTROL_OPTIONS_REPLACE 0x00000001 /* Replace a packet
                                                          control entry. */

/* 
 * Packet control action structure, common for integrity check API and
 * protocol control API.
 */
typedef struct bcm_switch_pkt_control_action_s {
    uint8 discard;          /* Packet is discarded. */
    uint8 copy_to_cpu;      /* Packet is copied to CPU. */
    uint8 flood;            /* Packet is flooded to the forwarding domain. */
    uint8 bpdu;             /* Packet is BPDU. */
    uint8 opaque_ctrl_id;   /* Opaque control ID. */
    uint32 class_id;        /* Class ID. */
} bcm_switch_pkt_control_action_t;

/* An API to initialize a packet control action structure. */
extern void bcm_switch_pkt_control_action_t_init(
    bcm_switch_pkt_control_action_t *action);

/* Packet protocol match structure. */
typedef struct bcm_switch_pkt_protocol_match_s {
    uint8 l2_iif_opaque_ctrl_id;        /* L2 iif based control id. */
    uint8 l2_iif_opaque_ctrl_id_mask;   /* L2 iif based control id mask. */
    bcm_vlan_t vfi;                     /* Virtual forwarding instance. */
    bcm_vlan_t vfi_mask;                /* VFI mask. */
    bcm_mac_t macda;                    /* Destination MAC address. */
    bcm_mac_t macda_mask;               /* Destination MAC address mask. */
    uint16 ethertype;                   /* Ethernet type. */
    uint16 ethertype_mask;              /* Ethernet type mask. */
    uint16 arp_rarp_opcode;             /* ARP/RARP packet operation code. */
    uint16 arp_rarp_opcode_mask;        /* ARP/RARP packet operation code mask. */
    uint8 icmp_type;                    /* ICMP type. */
    uint8 icmp_type_mask;               /* ICMP type mask. */
    uint8 igmp_type;                    /* IGMP type. */
    uint8 igmp_type_mask;               /* IGMP type mask. */
    uint8 l4_valid;                     /* If set, layer 4 header present. */
    uint8 l4_valid_mask;                /* L4_valid mask. */
    uint16 l4_src_port;                 /* Layer 4 source port. */
    uint16 l4_src_port_mask;            /* Layer 4 source port mask. */
    uint16 l4_dst_port;                 /* Layer 4 destination port. */
    uint16 l4_dst_port_mask;            /* Layer 4 destination port mask. */
    uint16 l5_bytes_0_1;                /* First 2 bytes of layer 5 header. */
    uint16 l5_bytes_0_1_mask;           /* L5_bytes_0_1 mask. */
    uint16 outer_l5_bytes_0_1;          /* Outer l5 bytes 0 1. */
    uint16 outer_l5_bytes_0_1_mask;     /* Outer l5 bytes 0 1 mask. */
    uint8 ip_last_protocol;             /* Protocol from IP extension header if
                                           present, otherwise from IP header. */
    uint8 ip_last_protocol_mask;        /* Ip_last_protocol mask. */
    uint8 fixed_hve_result_set_1;       /* Fixed HVE result 1. */
    uint8 fixed_hve_result_set_1_mask;  /* Fixed HVE result 1 mask. */
    uint8 fixed_hve_result_set_5;       /* Fixed HVE result 5. */
    uint8 fixed_hve_result_set_5_mask;  /* Fixed HVE result 5 mask. */
    uint8 flex_hve_result_set_1;        /* Flex HVE result 1. */
    uint8 flex_hve_result_set_1_mask;   /* Flex HVE result 1 mask. */
    uint8 tunnel_processing_results_1;  /* Tunnel processing results 1. */
    uint8 tunnel_processing_results_1_mask; /* Tunnel processing results 1 mask. */
    uint8 vfp_opaque_ctrl_id;           /* VFP opaque ctrl id. */
    uint8 vfp_opaque_ctrl_id_mask;      /* VFP opaque ctrl id mask. */
    uint8 vlan_xlate_opaque_ctrl_id;    /* VLAN_XLATE opaque ctrl id: Bit 0 :
                                           VLAN_XLATE drop; Bit 1 : VLAN_XLATE
                                           copy to CPU; Bits 2-3: The highest 2
                                           bits of VLAN_XLATE opaque control id. */
    uint8 vlan_xlate_opaque_ctrl_id_mask; /* vlan_xlate_opaque_ctrl_id - Mask. */
    uint8 vlan_xlate_opaque_ctrl_id_1;  /* VLAN_XLATE opaque ctrl id_1: Bits
                                           0-3: The lowest 4 bits of VLAN_XLATE
                                           opaque control id. */
    uint8 vlan_xlate_opaque_ctrl_id_1_mask; /* vlan_xlate_opaque_ctrl_id_1 - Mask. */
    uint8 l2_iif_opaque_ctrl_id_1;      /* L2 iif based control id_1. */
    uint8 l2_iif_opaque_ctrl_id_1_mask; /* L2 iif based control id_1 mask. */
    uint8 fixed_hve_result_set_4;       /* Fixed HVE result 4. */
    uint8 fixed_hve_result_set_4_mask;  /* Fixed HVE result 4 mask. */
} bcm_switch_pkt_protocol_match_t;

/* An API to initialize a protocol match structure. */
extern void bcm_switch_pkt_protocol_match_t_init(
    bcm_switch_pkt_protocol_match_t *match);

#ifndef BCM_HIDE_DISPATCHABLE

/* An API to add a protocol control entry. */
extern int bcm_switch_pkt_protocol_control_add(
    int unit, 
    uint32 options, 
    bcm_switch_pkt_protocol_match_t *match, 
    bcm_switch_pkt_control_action_t *action, 
    int priority);

/* An API to get action and priority for a protocol control entry. */
extern int bcm_switch_pkt_protocol_control_get(
    int unit, 
    bcm_switch_pkt_protocol_match_t *match, 
    bcm_switch_pkt_control_action_t *action, 
    int *priority);

/* An API to delete a protocol control entry. */
extern int bcm_switch_pkt_protocol_control_delete(
    int unit, 
    bcm_switch_pkt_protocol_match_t *match);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Protocol control entry callback function prototype. */
typedef int (*bcm_switch_pkt_protocol_control_traverse_cb)(
    int unit, 
    bcm_switch_pkt_protocol_match_t *match, 
    bcm_switch_pkt_control_action_t *action, 
    int *priority, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse all installed protocol control entries. */
extern int bcm_switch_pkt_protocol_control_traverse(
    int unit, 
    bcm_switch_pkt_protocol_control_traverse_cb cb, 
    void *user_data);

/* Delete all installed protocol control entries. */
extern int bcm_switch_pkt_protocol_control_delete_all(
    int unit);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Packet integrity match attribute structure. */
typedef struct bcm_switch_pkt_integrity_match_s {
    uint8 tunnel_processing_results_1;  /* Tunnel processing results 1. */
    uint8 tunnel_processing_results_1_mask; /* Tunnel processing results 1 mask. */
    uint8 mpls_flow_type;               /* Mpls flow type. */
    uint8 mpls_flow_type_mask;          /* Mpls flow type mask. */
    uint8 my_station_1_hit;             /* My station 1 hit. */
    uint8 my_station_1_hit_mask;        /* My station 1 hit mask. */
    uint8 l4_valid;                     /* L4 valid. */
    uint8 l4_valid_mask;                /* L4 valid mask. */
    uint8 icmp_type;                    /* Icmp type. */
    uint8 icmp_type_mask;               /* Icmp type mask. */
    uint8 ip_last_protocol;             /* Ip last protocol. */
    uint8 ip_last_protocol_mask;        /* Ip last protocol mask. */
    uint16 tcp_hdr_len_and_flags;       /* Tcp hdr len and flags. */
    uint16 tcp_hdr_len_and_flags_mask;  /* Tcp hdr len and flags mask. */
    uint8 fixed_hve_result_0;           /* Fixed hve result 0. */
    uint8 fixed_hve_result_0_mask;      /* Fixed hve result 0 mask. */
    uint8 fixed_hve_result_1;           /* Fixed hve result 1. */
    uint8 fixed_hve_result_1_mask;      /* Fixed hve result 1 mask. */
    uint8 fixed_hve_result_2;           /* Fixed hve result 2. */
    uint8 fixed_hve_result_2_mask;      /* Fixed hve result 2 mask. */
    uint8 fixed_hve_result_3;           /* Fixed hve result 3. */
    uint8 fixed_hve_result_3_mask;      /* Fixed hve result 3 mask. */
    uint8 fixed_hve_result_4;           /* Fixed hve result 4. */
    uint8 fixed_hve_result_4_mask;      /* Fixed hve result 4 mask. */
    uint8 fixed_hve_result_5;           /* Fixed hve result 5. */
    uint8 fixed_hve_result_5_mask;      /* Fixed hve result 5 mask. */
    uint8 flex_hve_result_0;            /* Flex hve result 0. */
    uint8 flex_hve_result_0_mask;       /* Flex hve result 0 mask. */
    uint8 flex_hve_result_1;            /* Flex hve result 1. */
    uint8 flex_hve_result_1_mask;       /* Flex hve result 1 mask. */
    uint8 flex_hve_result_2;            /* Flex hve result 2. */
    uint8 flex_hve_result_2_mask;       /* Flex hve result 2 mask. */
} bcm_switch_pkt_integrity_match_t;

/* Initialize a packet integrity match attribute structure. */
extern void bcm_switch_pkt_integrity_match_t_init(
    bcm_switch_pkt_integrity_match_t *match);

#ifndef BCM_HIDE_DISPATCHABLE

/* Add an entry into packet integrity check table. */
extern int bcm_switch_pkt_integrity_check_add(
    int unit, 
    uint32 options, 
    bcm_switch_pkt_integrity_match_t *match, 
    bcm_switch_pkt_control_action_t *action, 
    int priority);

/* Delete an entry from packet integrity check table. */
extern int bcm_switch_pkt_integrity_check_delete(
    int unit, 
    bcm_switch_pkt_integrity_match_t *match);

/* Get an entry from packet integrity check table. */
extern int bcm_switch_pkt_integrity_check_get(
    int unit, 
    bcm_switch_pkt_integrity_match_t *match, 
    bcm_switch_pkt_control_action_t *action, 
    int *priority);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Monitor action definitions which are common to swith drop events and
 * switch trace events.
 */
#define BCM_SWITCH_MON_ACTION_NONE          0          /* Action none. */
#define BCM_SWITCH_MON_ACTION_COPY_TO_CPU   0x1        /* Enable to copy packets
                                                          to CPU. */
#define BCM_SWITCH_MON_ACTION_LOOPBACK_DROP 0x2        /* Enable to drop
                                                          loopback copy. */

/* Switch drop event monitor structure. */
typedef struct bcm_switch_drop_event_mon_s {
    bcm_pkt_drop_event_t drop_event;    /* Drop event. */
    uint32 actions;                     /* BCM_SWITCH_MON_ACTION_XXX actions
                                           definitions. */
} bcm_switch_drop_event_mon_t;

/* Initialize a bcm_switch_drop_event_mon_t structure. */
extern void bcm_switch_drop_event_mon_t_init(
    bcm_switch_drop_event_mon_t *monitor);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * These APIs set/get monitor configuration for drop events defined in
 * <bcm/types.h>.
 */
extern int bcm_switch_drop_event_mon_set(
    int unit, 
    bcm_switch_drop_event_mon_t *monitor);

/* 
 * These APIs set/get monitor configuration for drop events defined in
 * <bcm/types.h>.
 */
extern int bcm_switch_drop_event_mon_get(
    int unit, 
    bcm_switch_drop_event_mon_t *monitor);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Switch trace event monitor structure. */
typedef struct bcm_switch_trace_event_mon_s {
    bcm_pkt_trace_event_t trace_event;  /* Trace event. */
    uint32 actions;                     /* BCM_SWITCH_MON_ACTION_XXX actions
                                           definitions. */
} bcm_switch_trace_event_mon_t;

/* Initialize a bcm_switch_trace_event_mon_t structure. */
extern void bcm_switch_trace_event_mon_t_init(
    bcm_switch_trace_event_mon_t *monitor);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * These APIs set/get monitor configuration for trace events defined in
 * <bcm/types.h>.
 */
extern int bcm_switch_trace_event_mon_set(
    int unit, 
    bcm_switch_trace_event_mon_t *monitor);

/* 
 * These APIs set/get monitor configuration for trace events defined in
 * <bcm/types.h>.
 */
extern int bcm_switch_trace_event_mon_get(
    int unit, 
    bcm_switch_trace_event_mon_t *monitor);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Hash banks. */
typedef enum bcm_switch_hash_bank_e {
    bcmSwitchHashBank0 = 0,     /* Hash bank 0. */
    bcmSwitchHashBank1 = 1,     /* Hash bank 1. */
    bcmSwitchHashBank2 = 2,     /* Hash bank 2. */
    bcmSwitchHashBank3 = 3,     /* Hash bank 3. */
    bcmSwitchHashBank4 = 4,     /* Hash bank 4. */
    bcmSwitchHashBank5 = 5,     /* Hash bank 5. */
    bcmSwitchHashBank6 = 6,     /* Hash bank 6. */
    bcmSwitchHashBank7 = 7,     /* Hash bank 7. */
    bcmSwitchHashBank8 = 8,     /* Hash bank 8. */
    bcmSwitchHashBank9 = 9,     /* Hash bank 9. */
    bcmSwitchHashBank10 = 10,   /* Hash bank 10. */
    bcmSwitchHashBank11 = 11,   /* Hash bank 11. */
    bcmSwitchHashBank12 = 12,   /* Hash bank 12. */
    bcmSwitchHashBank13 = 13,   /* Hash bank 13. */
    bcmSwitchHashBank14 = 14,   /* Hash bank 14. */
    bcmSwitchHashBank15 = 15,   /* Hash bank 15. */
    bcmSwitchHashBank16 = 16,   /* Hash bank 16. */
    bcmSwitchHashBank17 = 17,   /* Hash bank 17. */
    bcmSwitchHashBank18 = 18,   /* Hash bank 18. */
    bcmSwitchHashBank19 = 19,   /* Hash bank 19. */
    bcmSwitchHashBank20 = 20,   /* Hash bank 20. */
    bcmSwitchHashBank21 = 21,   /* Hash bank 21. */
    bcmSwitchHashBank22 = 22,   /* Hash bank 22. */
    bcmSwitchHashBank23 = 23,   /* Hash bank 23. */
    bcmSwitchHashBank24 = 24,   /* Hash bank 24. */
    bcmSwitchHashBank25 = 25,   /* Hash bank 25. */
    bcmSwitchHashBank26 = 26,   /* Hash bank 26. */
    bcmSwitchHashBank27 = 27,   /* Hash bank 27. */
    bcmSwitchHashBank28 = 28,   /* Hash bank 28. */
    bcmSwitchHashBank29 = 29,   /* Hash bank 29. */
    bcmSwitchHashBank30 = 30,   /* Hash bank 30. */
    bcmSwitchHashBank31 = 31,   /* Hash bank 31. */
    bcmSwitchHashBankCount = 32 /* Must Be Last. Not A Valid Hash Bank. */
} bcm_switch_hash_bank_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get the hash banks assigned to a hash table. */
extern int bcm_switch_hash_bank_multi_get(
    int unit, 
    bcm_switch_hash_table_t hash_table, 
    int array_size, 
    bcm_switch_hash_bank_t *bank_array, 
    int *num_banks);

/* Get a set of hash tables to which the given hash bank is assigned. */
extern int bcm_switch_hash_table_multi_get(
    int unit, 
    bcm_switch_hash_bank_t bank_num, 
    int array_size, 
    bcm_switch_hash_table_t *table_array, 
    int *num_tables);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Hash bank information structure. */
typedef struct bcm_switch_hash_bank_info_s {
    bcm_switch_hash_bank_t bank_num;    /* Hash bank number. */
    uint32 base_entry_width;            /* Width of base entry in bits. */
    uint32 num_base_entries;            /* Number of base entries. */
} bcm_switch_hash_bank_info_t;

/* Initialize a hash bank information structure. */
extern void bcm_switch_hash_bank_info_init(
    bcm_switch_hash_bank_info_t *bank_info);

#ifndef BCM_HIDE_DISPATCHABLE

/* Get information about a hash bank. */
extern int bcm_switch_hash_bank_info_get(
    int unit, 
    bcm_switch_hash_bank_info_t *bank_info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Hash bank control valid fields. Each flag indicates whether a member
 * field in the bcm_switch_hash_bank_control_t structure is valid for the
 * hash bank control get or set function.
 */
#define BCM_SWITCH_HASH_BANK_CONTROL_ROBUST_VALID (1 << 0)   /* Robust field valid. */
#define BCM_SWITCH_HASH_BANK_CONTROL_HASH_TYPE_VALID (1 << 1)   /* Hash type field valid. */
#define BCM_SWITCH_HASH_BANK_CONTROL_OFFSET_VALID (1 << 2)   /* Offset field valid. */
#define BCM_SWITCH_HASH_BANK_CONTROL_SEED_VALID (1 << 3)   /* Robust hash seed field
                                                          valid. */

/* Hash bank control structure. */
typedef struct bcm_switch_hash_bank_control_s {
    bcm_switch_hash_bank_t bank_num;    /* Hash bank number. */
    uint32 valid_fields;                /* Bitmap of valid fields. */
    int robust;                         /* 0 to disable robust hash or 1 to
                                           enable robust hash. */
    int hash_type;                      /* BCM_HASH_XXX definitions. */
    uint32 offset;                      /* Hash offset. */
    uint32 seed;                        /* Random seed for robust hash. */
} bcm_switch_hash_bank_control_t;

/* Initialize a hash bank control structure. */
extern void bcm_switch_hash_bank_control_init(
    bcm_switch_hash_bank_control_t *bank_control);

#ifndef BCM_HIDE_DISPATCHABLE

/* Hash bank control set/get function. */
extern int bcm_switch_hash_bank_control_set(
    int unit, 
    bcm_switch_hash_bank_control_t *bank_control);

/* Hash bank control set/get function. */
extern int bcm_switch_hash_bank_control_get(
    int unit, 
    bcm_switch_hash_bank_control_t *bank_control);

#endif /* BCM_HIDE_DISPATCHABLE */

/* ISSU upgrade start. */
extern int bcm_switch_issu_upgrade_start(
    const char *from_ver, 
    const char *to_ver);

/* ISSU upgrade done. */
extern int bcm_switch_issu_upgrade_done(void);

/* Concat header type to master key in ELK */
typedef enum bcm_switch_concat_header_e {
    bcmSwitchConcatHeaderNone = 0,      /* No extra header concate to master key
                                           in ELK. */
    bcmSwitchConcatHeaderForwarding = 1, /* Concate forwarding header to master
                                           key in ELK. */
    bcmSwitchConcatHeaderNextForwarding = 2, /* Concate header following the
                                           forwarding header to master key in
                                           ELK. */
    bcmSwitchConcatHeader__Count        /* Must be last. */
} bcm_switch_concat_header_t;

/* Callback function used for device hard reset */
typedef void (*bcm_switch_hard_reset_callback_t)(
    int unit, 
    uint32 flags, 
    void *userdata);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Registers/Unregisters a callback function for switch critical events -
 * hard reset
 */
extern int bcm_switch_hard_reset_cb_register(
    int unit, 
    uint32 flags, 
    bcm_switch_hard_reset_callback_t callback, 
    void *userdata);

/* 
 * Registers/Unregisters a callback function for switch critical events -
 * hard reset
 */
extern int bcm_switch_hard_reset_cb_unregister(
    int unit, 
    bcm_switch_hard_reset_callback_t callback, 
    void *userdata);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_SWITCH_FEC_PROPERTY_1ST_HIERARCHY 0x1        /* First FEC Hierarchy. */
#define BCM_SWITCH_FEC_PROPERTY_2ND_HIERARCHY 0x2        /* Second FEC Hierarchy. */
#define BCM_SWITCH_FEC_PROPERTY_3RD_HIERARCHY 0x4        /* Third FEC Hierarchy. */

/* 
 * This structure contains the first and last FEC ID in the FEC IDs range
 * of the given hierarchy.
 */
typedef struct bcm_switch_fec_property_config_s {
    uint32 flags;   /* BCM_SWITCH_FEC_PROPERTY_XXX */
    int start;      /* Start ID */
    int end;        /* End ID */
} bcm_switch_fec_property_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get the available FEC IDs for each FEC hierarchy. */
extern int bcm_switch_fec_property_get(
    int unit, 
    bcm_switch_fec_property_config_t *fec_config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * An enum that represent a resource type to be used with
 * bcm_dnx_switch_resource_utilization_get
 */
typedef enum bcm_switch_resource_type_e {
    bcmResourceTypeEM = 0, 
    bcmResourceTypeLPM = 1, 
    bcmResourceTypeDirect = 2, 
    bcmResourceTypeNOF = 3      /* should always be last */
} bcm_switch_resource_type_t;

/* 
 * A struct that represent resource utilization Is used as output for
 * bcm_switch_resource_utilization_get.
 *                 Is used as output for
 * bcm_switch_resource_utilization_get.
 *                 Can be expanded in the future to represent more
 * complex resources.
 */
typedef struct bcm_switch_resource_utilization_s {
    uint32 guaranteed_free; /* the maximum number of resources that are free for
                               use in the worst case configuration sequence */
} bcm_switch_resource_utilization_t;

/* 
 * A struct that represent resource properties and can be used as input
 * for bcm_switch_resource_utilization_get in order to get more accurate
 * result
 */
typedef struct bcm_switch_resource_utilization_query_s {
    bcm_switch_resource_type_t type;    /* This field represent the type of the
                                           resource */
    uint32 id;                          /* This field can serve as a resource ID
                                           if such is applicable for the
                                           resource type */
} bcm_switch_resource_utilization_query_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * return the utilization for a specific resource the free field in the
 * utilization output is represented as worst case and not average case.
 */
extern int bcm_switch_resource_utilization_get(
    int unit, 
    bcm_switch_resource_utilization_query_t *resource, 
    bcm_switch_resource_utilization_t *utilization);

/* Get number of LED microcontrollers. */
extern int bcm_switch_led_uc_num_get(
    int unit, 
    int *led_uc_num);

/* Load LED firmware binary. */
extern int bcm_switch_led_fw_load(
    int unit, 
    int led_uc, 
    const uint8 *data, 
    int len);

/* Start/stop LED firmware. */
extern int bcm_switch_led_fw_start_set(
    int unit, 
    int led_uc, 
    int data);

/* Get if LED firmware is started or not. */
extern int bcm_switch_led_fw_start_get(
    int unit, 
    int led_uc, 
    int *data);

/* Write control data of LED firmware. */
extern int bcm_switch_led_control_data_write(
    int unit, 
    int led_uc, 
    int offset, 
    const uint8 *data, 
    int len);

/* Read control data of LED firmware. */
extern int bcm_switch_led_control_data_read(
    int unit, 
    int led_uc, 
    int offset, 
    uint8 *data, 
    int len);

/* Map physical port to LED microcontroller number and port index. */
extern int bcm_switch_led_port_to_uc_port_get(
    int unit, 
    int port, 
    int *led_uc, 
    int *led_uc_port);

/* 
 * Configure the mapping from physical port to LED microcontroller number
 * and port index.
 */
extern int bcm_switch_led_port_to_uc_port_set(
    int unit, 
    int port, 
    int led_uc, 
    int led_uc_port);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Replace the existing encap id to destination mapping entry */
#define BCM_SWITCH_ENCAP_DEST_REPLACE   0x00000001 

/* Encap destination struct */
typedef struct bcm_switch_encap_dest_s {
    uint32 flags;       /* BCM_SWITCH_ENCAP_DEST_xxx flags. */
    bcm_if_t encap_id;  /* Encap-ID. */
    bcm_gport_t gport;  /* Gport for Destination. */
} bcm_switch_encap_dest_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Add an encap id to destination mapping entry. */
extern int bcm_switch_encap_dest_map_add(
    int unit, 
    bcm_switch_encap_dest_t *encap_info);

/* 
 * Delete the encap id to destination mapping entry with the given
 * encap_id.
 */
extern int bcm_switch_encap_dest_map_delete(
    int unit, 
    bcm_switch_encap_dest_t *encap_info);

/* Get the destination with the given encap_id. */
extern int bcm_switch_encap_dest_map_get(
    int unit, 
    bcm_switch_encap_dest_t *encap_info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Switch encap dest map traverse callback. */
typedef int (*bcm_switch_encap_dest_map_traverse_cb)(
    int unit, 
    bcm_switch_encap_dest_t *encap_info, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse the all added encap id to destination mapping entries. */
extern int bcm_switch_encap_dest_map_traverse(
    int unit, 
    bcm_switch_encap_dest_map_traverse_cb cb_fn, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Values passed for setting switch control bcmSwitchMactAgeRefreshMode. */
typedef enum bcm_switch_age_refresh_mode_e {
    bcmSwitchAgeRefreshModeSrc = 0,     /* Aging hit bit mode set to source
                                           address */
    bcmSwitchAgeRefreshModeDest = 1,    /* Aging hit bit mode set to destination
                                           address */
    bcmSwitchAgeRefreshModeSrcAndDest = 2, /* Aging hit bit mode set to source and
                                           destination addresses */
    bcmSwitchAgeRefreshModeSrcOrDest = 3 /* Aging hit bit mode set to source or
                                           destination addresses */
} bcm_switch_age_refresh_mode_t;

/* SVTAG flags. */
#define BCM_SWITCH_SVTAG_EGRESS_REPLACE     0x1        /* Replace an SVTAG
                                                          entry. */
#define BCM_SWITCH_SVTAG_EGRESS_DEFAULT_ENTRY 0x2        /* Update the default
                                                          entry values, used in
                                                          case of not found. */

/* SVTAG traverse flags. */
#define BCM_SWITCH_SVTAG_EGRESS_TRAVERSE_DELETE_ALL 0x1        /* Delete all the SVTAG
                                                          entries. */

/* SVTAG Egress info structure */
typedef struct bcm_switch_svtag_egress_info_s {
    uint32 sci;                     /* secure context index. */
    bcm_svtag_pkt_type_t pkt_type;  /* The packet type. */
    uint32 offset_addr;             /* The offset addition to place the security
                                       tag. */
    uint32 signature;               /* The SVTAG signature value. */
    uint8 ipv6_indication;          /* Indication of an IPv6 tunneling. */
    uint8 accumulation;             /* accumulate the offset to the security
                                       tag. */
} bcm_switch_svtag_egress_info_t;

/* switch SVTAG egress traverse callback signature */
typedef int (*bcm_switch_svtag_egress_traverse_cb)(
    int unit, 
    uint32 flags, 
    bcm_gport_t gport, 
    bcm_switch_svtag_egress_info_t *svtag_info, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Adds an SVTAG entry the EM database. */
extern int bcm_switch_svtag_egress_entry_add(
    int unit, 
    uint32 flags, 
    bcm_gport_t gport, 
    bcm_switch_svtag_egress_info_t *svtag_info);

/* Gets an SVTAG entry. */
extern int bcm_switch_svtag_egress_entry_get(
    int unit, 
    uint32 flags, 
    bcm_gport_t gport, 
    bcm_switch_svtag_egress_info_t *svtag_info);

/* Delete an SVTAG entry. */
extern int bcm_switch_svtag_egress_entry_delete(
    int unit, 
    uint32 flags, 
    bcm_gport_t gport);

/* Traverse through the SVTAG egress entries and run callback. */
extern int bcm_switch_svtag_egress_entry_traverse(
    int unit, 
    uint32 flags, 
    bcm_switch_svtag_egress_traverse_cb trav_fn, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Values of tunnel collapsing protocols that can be disabled by
 * bcmSwitchL3TunnelCollapseDisable switch control
 */
typedef enum bcm_switch_l3_tunnel_e {
    bcmSwitchL3TunnelL2TP = 0,  /* L2TP */
    bcmSwitchL3TunnelVxLan = 1, /* VxLan */
    bcmSwitchL3TunnelGtpU = 2,  /* GTP-U */
    bcmSwitchL3TunnelCount = 3  /* Count */
} bcm_switch_l3_tunnel_t;

/* Application signature Identifier. */
typedef uint32 bcm_switch_appl_signature_t;

#define BCM_SWITCH_APPL_SIGNATURE_L4_PAYLOAD_MATCH_LEN 32         /* Number of bytes that
                                                          can be matched in L4
                                                          payload. */

#define BCM_SWITCH_APPL_SIGNATURE_TYPE_TCP  (1 << 0)   /* Application signature
                                                          for TCP flow */
#define BCM_SWITCH_APPL_SIGNATURE_TYPE_UDP  (1 << 1)   /* Application signature
                                                          for UDP flow */
#define BCM_SWITCH_APPL_SIGNATURE_TYPE_RESPONSE (1 << 2)   /* Application signature
                                                          for Response packets */

#define BCM_SWITCH_APPL_SIGNATURE_WITH_ID   (1 << 0)   /* Create Application
                                                          signature with Id */
#define BCM_SWITCH_APPL_SIGNATURE_INNER_PAYLOAD (1 << 1)   /* Application signature
                                                          for matching in inner
                                                          payload */

/* Application signature information */
typedef struct bcm_switch_appl_signature_info_s {
    uint32 flags;                       /* control flags, default values
                                           indicate tcp flow */
    uint32 priority;                    /* priority of applicate signature entry */
    uint16 l4protocol_port;             /* L4 Protocol port */
    uint8 l4_payload[BCM_SWITCH_APPL_SIGNATURE_L4_PAYLOAD_MATCH_LEN]; /* L4 payload data array */
    uint8 l4_payload_mask[BCM_SWITCH_APPL_SIGNATURE_L4_PAYLOAD_MATCH_LEN]; /* L4 payload data array */
} bcm_switch_appl_signature_info_t;

/* Initialize a Application signature structure. */
extern void bcm_switch_appl_signature_info_t_init(
    bcm_switch_appl_signature_info_t *appl_signature_info);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create application signature entry. */
extern int bcm_switch_appl_signature_create(
    int unit, 
    uint32 options, 
    bcm_switch_appl_signature_info_t *appl_signature_info, 
    bcm_switch_appl_signature_t *appl_signature);

/* Get application signature entry info. */
extern int bcm_switch_appl_signature_get(
    int unit, 
    uint32 options, 
    bcm_switch_appl_signature_t appl_signature, 
    bcm_switch_appl_signature_info_t *appl_signature_info);

/* Get all application signature entries configured in the system. */
extern int bcm_switch_appl_signature_get_all(
    int unit, 
    uint32 options, 
    int size, 
    bcm_switch_appl_signature_t *appl_signature_array, 
    int *count);

/* Set priority of application signature entry. */
extern int bcm_switch_appl_signature_prio_set(
    int unit, 
    uint32 options, 
    bcm_switch_appl_signature_t appl_signature, 
    uint32 priority);

/* Destroy application signature entry. */
extern int bcm_switch_appl_signature_destroy(
    int unit, 
    uint32 options, 
    bcm_switch_appl_signature_t appl_signature);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Values of the IPv6 address 32b word index */
typedef enum bcm_switch_ipv6_word_index_e {
    bcmSwitchIPv6LowLsb = 0,    /* LSB Low */
    bcmSwitchIPv6LowMsb = 1,    /* LSB Low */
    bcmSwitchIPv6HighLsb = 2,   /* LSB Low */
    bcmSwitchIPv6HighMsb = 3    /* LSB Low */
} bcm_switch_ipv6_word_index_t;

/* Values of the SVTag Error Codes */
typedef enum bcm_switch_svtag_err_code_e {
    bcmSwitchSvtagErrCodeNone = 0,      /* Error Code None */
    bcmSwitchSvtagErrCodeSpTcamMiss = 1, 
    bcmSwitchSvtagErrCodeTagCntrlPortDisabledPkt = 2, 
    bcmSwitchSvtagErrCodeUntagCtrlPortDisabledPkt = 3, 
    bcmSwitchSvtagErrCodeIpv4ChecksumMismatchOrMplsBosNotFound = 4, 
    bcmSwitchSvtagErrCodeInvalidSectag = 5, 
    bcmSwitchSvtagErrCodeScTcamMiss = 6, 
    bcmSwitchSvtagErrCodeNoSa = 7, 
    bcmSwitchSvtagErrCodeReplayFailure = 8, 
    bcmSwitchSvtagErrCodeCount = 9 
} bcm_switch_svtag_err_code_t;

/* 
 * This enum contains values of operators that can be used on L4 OPS
 * Extended Range Types
 */
typedef enum bcm_switch_range_operator_e {
    bcmSwitchRangeOperatorInvalid = -1, /* Param Invalid */
    bcmSwitchRangeOperatorFirst = 0,    /* First Param (used for indexing) */
    bcmSwitchRangeOperatorNone = bcmSwitchRangeOperatorFirst, /* Operator None (in3,in2,in1,in0) */
    bcmSwitchRangeOperatorFirstPairAnd = 1, /* Operator First Pair And
                                           (in3,in2,in1,in1&in0) */
    bcmSwitchRangeOperatorSecondPairAnd = 2, /* Operator Second Pair And
                                           (in3,in3&in2,in1,in0) */
    bcmSwitchRangeOperatorBothPairAnd = 3, /* Operator Both Pair And
                                           (in3,in3&in2,in1,in1&in0) */
    bcmSwitchRangeOperator64bPairAndLow = 4, /* Operator 64b Pair And Low
                                           (in3,in2,{in3,in2}&{in1,in0}) */
    bcmSwitchRangeOperator64bPairAndHigh = 5, /* Operator 64b Pair And High
                                           ({in3,in2}&{in1,in0},in1,in0) */
    bcmSwitchRangeOperatorFirstPairOr = 6, /* Operator First Pair Or
                                           (in3,in2,in1,in1|in0) */
    bcmSwitchRangeOperatorSecondPairOr = 7, /* Operator Second Pair Or
                                           (in3,in3|in2,in1,in0) */
    bcmSwitchRangeOperatorBothPairOr = 8, /* Operator Both Pair Or
                                           (in3,in3|in2,in1,in1|in0) */
    bcmSwitchRangeOperator64bPairOrLow = 9, /* Operator 64b Pair Or Low
                                           (in3,in2,{in3,in2}|{in1,in0}) */
    bcmSwitchRangeOperator64bPairOrHigh = 10, /* Operator 64b Pair Or High
                                           ({in3,in2}|{in1,in0},in1,in0) */
    bcmSwitchRangeOperatorCount = 11    /* Count */
} bcm_switch_range_operator_t;

#define BCM_SWITCH_RANGE_OPERATOR_STR \
{ \
    "None", \
    "FirstPairAnd", \
    "SecondPairAnd", \
    "BothPairAnd", \
    "64bPairAndLow", \
    "64bPairAndHigh", \
    "FirstPairOr", \
    "SecondPairOr", \
    "BothPairOr", \
    "64bPairOrLow", \
    "64bPairOrHigh"  \
}

/* 
 * This enum contains values of result map selection for L4 OPS Extended
 * Encoders
 */
typedef enum bcm_switch_range_result_map_e {
    bcmSwitchRangeResultMapInvalid = -1, /* Param Invalid */
    bcmSwitchRangeResultMapFirst = 0,   /* First Param (used for indexing) */
    bcmSwitchRangeResultMapNone = bcmSwitchRangeResultMapFirst, /* Result None */
    bcmSwitchRangeResultMapFirstOnly = 1, /* Result [First] (out0) */
    bcmSwitchRangeResultMapSecondOnly = 2, /* Result [Second] (out1) */
    bcmSwitchRangeResultMapThirdOnly = 3, /* Result [Third] (out2) */
    bcmSwitchRangeResultMapForthOnly = 4, /* Result [Forth] (out3) */
    bcmSwitchRangeResultMapFirstnSecond = 5, /* Result [First,Second] (out1,out0) */
    bcmSwitchRangeResultMapFirstnThird = 6, /* Result {First,Third} (out2,out0) */
    bcmSwitchRangeResultMapFirstnForth = 7, /* Result [First,Forth] (out3,out0) */
    bcmSwitchRangeResultMapSecondnThird = 8, /* Result [Second,Third] (out2,out1) */
    bcmSwitchRangeResultMapSecondnForth = 9, /* Result [Second,Forth] (out3,out1) */
    bcmSwitchRangeResultMapThirdnForth = 10, /* Result [Third,Forth] (out3,out2) */
    bcmSwitchRangeResultMapFirstnSecondnThird = 11, /* Result [First,Second,Third]
                                           (out2,out1,out0) */
    bcmSwitchRangeResultMapFirstnSecondnForth = 12, /* Result [First,Second,Forth]
                                           (out3,out1,out0) */
    bcmSwitchRangeResultMapFirstnThirdnForth = 13, /* Result [First,Third,Forth]
                                           (out3,out2,out0) */
    bcmSwitchRangeResultMapSecondnThirdnForth = 14, /* Result [Second,Third,Forth]
                                           (out3,out2,out1) */
    bcmSwitchRangeResultMapFirstnSecondnThirdnForth = 15, /* Result [First,Second,Third,Forth]
                                           (out3,out2,out1,out0) */
    bcmSwitchRangeResultMapCount = 16   /* Count */
} bcm_switch_range_result_map_t;

#define BCM_SWITCH_RANGE_RESULT_MAP_STR \
{ \
    "None", \
    "FirstOnly", \
    "SecondOnly", \
    "ThirdOnly", \
    "ForthOnly", \
    "FirstnSecond", \
    "FirstnThird", \
    "FirstnForth", \
    "SecondnThird", \
    "SecondnForth", \
    "ThirdnForth", \
    "FirstnSecondnThird", \
    "FirstnSecondnForth", \
    "FirstnThirdnForth", \
    "SecondnThirdnForth", \
    "FirstnSecondnThirdnForth"  \
}

/* LIF hit type. */
#define BCM_SWITCH_LIF_HIT_INGRESS  0x1        /* Ingress Logical Interface. */
#define BCM_SWITCH_LIF_HIT_EGRESS   0x2        /* Egress Logical Interface */
#define BCM_SWITCH_LIF_HIT_L3_INTF  0x4        /* In case set, indicate L3
                                                  interface is used, otherwise
                                                  gport */
#define BCM_SWITCH_LIF_HIT_GET      0x8        /* Get LIF HIT indication */
#define BCM_SWITCH_LIF_HIT_CLEAR    0x10       /* Clear LIF HIT indication */

/* 
 * This structure describes the fields that need to be supplies for the
 * LIF hit.
 */
typedef struct bcm_switch_lif_hit_s {
    uint32 flags;       /* LIF hit flags */
    bcm_gport_t gport;  /* Gport ID */
    bcm_if_t l3_intf;   /* L3 interface ID */
    uint32 val;         /* HIT value */
} bcm_switch_lif_hit_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get the LIF hit values */
extern int bcm_switch_lif_hit_get(
    int unit, 
    bcm_switch_lif_hit_t *lif_hit);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Values passed for setting switch control bcmSwitchIoamType. */
typedef enum bcm_switch_ioam_type_e {
    bcmSwitchIoamTypeIncrementalTrace = 0, /* IOAM incremental trace option type */
    bcmSwitchIoamTypeCount = 1          /* Always last */
} bcm_switch_ioam_type_t;

/* Action selection for bcmSwitchIntVectorMatchMissAction. */
#define BCM_SWITCH_INT_VECTOR_MATCH_MISS_DROP 0          /* Drop. */
#define BCM_SWITCH_INT_VECTOR_MATCH_MISS_FORWARD 1          /* Forward. */

/* Enums to choose IPV6 L3 Local Link Src disposition. */
typedef enum bcm_switch_v6l3_local_link_select_e {
    bcmSwitchV6L3LocalLinkSrcDrop = 2,  /*  Always drop packets with Link Local
                                           SIP */
    bcmSwitchV6L3LocalLinkSrcDstNotLocalDrop = 3, /* Drop packets with Link Local SIP only
                                           if DIP is not local link.If DIP is
                                           also Local Link, do not drop the
                                           packets */
    bcmSwitchV6L3LocalLinkSelectCount = 4 /* Always last */
} bcm_switch_v6l3_local_link_select_t;

/* Specify the type of packets to be redirected or copied to CPU. */
typedef enum bcm_switch_ep_recirc_pkt_copy_type_e {
    bcmSwitchEpRecircPktCopyTypeNoCopy = 0, /* No copy. */
    bcmSwitchEpRecircPktCopyTypeDropped = 1, /* Copy only if the incoming packet is
                                           dropped. */
    bcmSwitchEpRecircPktCopyTypeNotDropped = 2, /* Copy only if the incoming packet is
                                           not dropped. */
    bcmSwitchEpRecircPktCopyTypeAll = 3, /* Copy all packets. */
    bcmSwitchEpRecircPktCopyTypeCount = 4 /* Must be last. Not a usable value. */
} bcm_switch_ep_recirc_pkt_copy_type_t;

/* 
 * Specify how the residence time field is constructed in NIH or CPU DMA
 * header.
 */
typedef enum bcm_switch_ep_recirc_residence_time_field_select_e {
    bcmSwitchEpRecircResidenceTimeFieldSelectTime = 0, /* Select residence time. */
    bcmSwitchEpRecircResidenceTimeFieldSelectOpaqueObjBC = 1, /* Select opaque object B and C. */
    bcmSwitchEpRecircResidenceTimeFieldSelectCount = 2 /* Must be last. Not a usable value. */
} bcm_switch_ep_recirc_residence_time_field_select_t;

/* 
 * Specify how the timestamp field is constructed in NIH or CPU DMA
 * header.
 */
typedef enum bcm_switch_ep_recirc_timestamp_select_e {
    bcmSwitchEpRecircTimestampSelectIngress = 0, /* Select ingress timestamp. */
    bcmSwitchEpRecircTimestampSelectEgress = 1, /* Select egress timestamp. */
    bcmSwitchEpRecircTimestampSelectCount = 2 /* Must be last. Not a usable value. */
} bcm_switch_ep_recirc_timestamp_select_t;

/* 
 * Specify the set of contiguous trace events to be copied to the NIH
 * header.
 */
typedef enum bcm_switch_ep_recirc_nih_trace_event_select_e {
    bcmSwitchEpRecircNihTraceEventDisable = 0, /* Disable the trace events from being
                                           copied into the NIH header. */
    bcmSwitchEpRecircNihTraceEvent_0_15 = 1, /* Select trace events 0-15. */
    bcmSwitchEpRecircNihTraceEvent_16_31 = 2, /* Select trace events 16-31. */
    bcmSwitchEpRecircNihTraceEvent_32_47 = 3, /* Select trace events 32-47. */
    bcmSwitchEpRecircNihTraceEventCount = 4 /* Must be last. Not a usable value. */
} bcm_switch_ep_recirc_nih_trace_event_select_t;

/* Specify the destination type for the redirected packets. */
typedef enum bcm_switch_ep_recirc_nih_dest_type_e {
    bcmSwitchEpRecircNihDestTypeNone = 0, /* No destination specified. */
    bcmSwitchEpRecircNihDestTypeFromOpaqueA = 1, /* The two fields destination_type and
                                           destination in the NIH header will
                                           carry values of the opaque control A
                                           and opaque object A from the internal
                                           bus. */
    bcmSwitchEpRecircNihDestTypeEgressL2Interface = 2, /* Destination type is Egress L2
                                           interface (L2 OIF). */
    bcmSwitchEpRecircNihDestTypeCount = 3 /* Must be last. Not a usable value. */
} bcm_switch_ep_recirc_nih_dest_type_t;

/* Flags for bcm_switch_ep_recirc_profile_t. */
#define BCM_SWITCH_EP_RECIRC_PROFILE_REPLACE (1U << 0)  /* Update a profile
                                                          entry. */
#define BCM_SWITCH_EP_RECIRC_PROFILE_WITH_ID (1U << 1)  /* Create or update a
                                                          profile with id. */
#define BCM_SWITCH_EP_RECIRC_DROP_SWITCHED_PACKET (1U << 2)  /* Drop the switched
                                                          packet in the first
                                                          pass. */

/* 
 * This structure configures the data that will be carried in the
 * residence time field on the NIH header.
 */
typedef struct bcm_switch_ep_recirc_nih_residence_time_field_config_s {
    bcm_switch_ep_recirc_residence_time_field_select_t res_time_field_select; /* NIH header residence time field
                                           selection. */
    int nih_drop_code_en;               /* Enable to copy the highest and first
                                           drop code to the NIH header. This
                                           will overwrite the upper 16 bit of
                                           residence time field. */
    bcm_switch_ep_recirc_nih_trace_event_select_t nih_trace_event_select; /* Select one of the sets of contiguous
                                           trace events to be copied to the NIH
                                           header. This will overwrite the lower
                                           16 bit of residence time field. */
} bcm_switch_ep_recirc_nih_residence_time_field_config_t;

/* 
 * EP Recirculate config structure for NIH construction for redirected
 * packets.
 */
typedef struct bcm_switch_ep_recirc_nih_config_s {
    bcm_switch_ep_recirc_nih_dest_type_t destination_type; /* Destination type used for the
                                           redirected packets. */
    int destination;                    /* Destination of the redirected
                                           packets. The meaning of the value
                                           depends on the destination type. */
    bcm_switch_ep_recirc_timestamp_select_t timestamp_select; /* Timestamp field selection. */
    bcm_switch_ep_recirc_nih_residence_time_field_config_t res_time_field_cfg; /* Residence time field construction. */
} bcm_switch_ep_recirc_nih_config_t;

/* 
 * This structure configures the data that will be carried in the
 * timestamp field on the CPU DMA header for packets copied to CPU.
 */
typedef struct bcm_switch_ep_recirc_cpu_dma_timestamp_field_config_s {
    bcm_switch_ep_recirc_timestamp_select_t timestamp_select; /* Timestamp field selection. */
    int opaque_obj_a_en;                /* Enable to carry opaque object A. This
                                           will overwrite the upper 16 bit of
                                           timestamp field. */
} bcm_switch_ep_recirc_cpu_dma_timestamp_field_config_t;

/* EP Recirculate config structure for CPU DMA header construction. */
typedef struct bcm_switch_ep_recirc_cpu_dma_config_s {
    bcm_switch_ep_recirc_cpu_dma_timestamp_field_config_t timestamp_field_cfg; /* Timestamp field construction. */
    bcm_switch_ep_recirc_residence_time_field_select_t res_time_field_select; /* Residence time field selection. */
    uint32 cpu_dma_header_flex_word_bitmap; /* Bitmap to select which 32-bit words
                                           from the internal bus to be copied to
                                           the CPU DMA header flex portion for
                                           packets copied to CPU. Users need to
                                           refer to related documents for the
                                           meaning of the words and the number
                                           of words to choose from. */
} bcm_switch_ep_recirc_cpu_dma_config_t;

/* EP Recirculate profile structure. */
typedef struct bcm_switch_ep_recirc_profile_s {
    int recirc_profile_id;              /* Profile id. */
    uint32 flags;                       /* See BCM_XXX_EP_RECIRC_PROFILE_XXX
                                           flag definitions. */
    bcm_switch_ep_recirc_pkt_copy_type_t redirect_type; /* Specify types of packets to be
                                           redirected. */
    bcm_switch_ep_recirc_pkt_copy_type_t tocpu_type; /* Specify types of packets to be copied
                                           to CPU. */
    int truncate_cell_length;           /* Truncated size (in cells) for the
                                           recirc packet. 0 means no truncation. */
    int buffer_priority;                /* Enqueue priority specified for the
                                           redirect data buffer (RDB). */
    bcm_switch_ep_recirc_nih_config_t nih_cfg; /* Config for NIH header construction
                                           for redirected packets. */
    bcm_switch_ep_recirc_cpu_dma_config_t cpu_dma_cfg; /* Config for CPU DMA header
                                           construction for copy-to-CPU packets. */
} bcm_switch_ep_recirc_profile_t;

/* 
 * Initialize an EP Recirculate NIH residence time field config
 * structure.
 */
extern void bcm_switch_ep_recirc_nih_residence_time_field_config_t_init(
    bcm_switch_ep_recirc_nih_residence_time_field_config_t *nih_residence_time_field);

/* Initialize an EP Recirculate NIH config structure. */
extern void bcm_switch_ep_recirc_nih_config_t_init(
    bcm_switch_ep_recirc_nih_config_t *nih_config);

/* 
 * Initialize an EP Recirculate CPU DMA header timestamp field config
 * structure.
 */
extern void bcm_switch_ep_recirc_cpu_dma_timestamp_field_config_t_init(
    bcm_switch_ep_recirc_cpu_dma_timestamp_field_config_t *cpu_dma_timestamp_field);

/* Initialize an EP Recirculate CPU DMA header config structure. */
extern void bcm_switch_ep_recirc_cpu_dma_config_t_init(
    bcm_switch_ep_recirc_cpu_dma_config_t *cpu_dma_config);

/* Initialize an EP Recirculate profile structure. */
extern void bcm_switch_ep_recirc_profile_t_init(
    bcm_switch_ep_recirc_profile_t *recirc_profile);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create an EP Recirculate profile. */
extern int bcm_switch_ep_recirc_profile_create(
    int unit, 
    bcm_switch_ep_recirc_profile_t *recirc_profile, 
    int *recirc_profile_id);

/* Destroy an EP Recirculate profile. */
extern int bcm_switch_ep_recirc_profile_destroy(
    int unit, 
    int recirc_profile_id);

/* Get all EP Recirculate profile. */
extern int bcm_switch_ep_recirc_profile_get_all(
    int unit, 
    int *recirc_profile_id_array, 
    int *count);

/* Get an EP Recirculate profile. */
extern int bcm_switch_ep_recirc_profile_get(
    int unit, 
    int recirc_profile_id, 
    bcm_switch_ep_recirc_profile_t *recirc_profile);

#endif /* BCM_HIDE_DISPATCHABLE */

/* EP Recirculate drop event control structure. */
typedef struct bcm_switch_ep_recirc_drop_event_control_s {
    int recirc_en;          /* Enable for drop event to trigger EP Recirculate
                               based on the configs of this structure. */
    int threshold;          /* If highest drop code of a dropped packet is
                               larger or equal to the threshold, trigger EP
                               Recirculate based on the calculated profile index
                               entry. Users need to refer to related documents
                               for the list of drop codes. */
    int profile_id_mask;    /* This mask is used to calculate the EP Recirculate
                               profile id for the drop events. Final profile id
                               = ((ing_profile_id & profile_id_mask) |
                               profile_id_offset). Where ing_profile_id is the
                               original id derived from the ingress pipeline. */
    int profile_id_offset;  /* This offset is used to calculate the EP
                               Recirculate profile id for the drop events. See
                               above for how the final profile id is calculated. */
} bcm_switch_ep_recirc_drop_event_control_t;

/* Initialize an EP Recirculate drop event control structure. */
extern void bcm_switch_ep_recirc_drop_event_control_t_init(
    bcm_switch_ep_recirc_drop_event_control_t *drop_event_ctrl);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set drop event based EP Recirculate control. */
extern int bcm_switch_ep_recirc_drop_event_control_set(
    int unit, 
    bcm_switch_ep_recirc_drop_event_control_t *drop_event_ctrl);

/* Get drop event based EP Recirculate control. */
extern int bcm_switch_ep_recirc_drop_event_control_get(
    int unit, 
    bcm_switch_ep_recirc_drop_event_control_t *drop_event_ctrl);

#endif /* BCM_HIDE_DISPATCHABLE */

/* EP Recirculate trace event control structure. */
typedef struct bcm_switch_ep_recirc_trace_event_control_s {
    int recirc_en;          /* Enable for trace event to trigger EP Recirculate
                               based on the configs of this structure. */
    int profile_id_mask;    /* This mask is used to calculate the EP Recirculate
                               profile id for the trace events. Final profile id
                               = ((ing_profile_id & profile_id_mask) |
                               profile_id_offset). Where ing_profile_id is the
                               original id derived from the ingress pipeline. */
    int profile_id_offset;  /* This offset is used to calculate the EP
                               Recirculate profile id for the trace events. See
                               above for how the final profile id is calculated. */
} bcm_switch_ep_recirc_trace_event_control_t;

/* Initialize an EP Recirculate trace event control structure. */
extern void bcm_switch_ep_recirc_trace_event_control_t_init(
    bcm_switch_ep_recirc_trace_event_control_t *trace_event_ctrl);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set trace event based EP Recirculate control. */
extern int bcm_switch_ep_recirc_trace_event_control_set(
    int unit, 
    bcm_switch_ep_recirc_trace_event_control_t *trace_event_ctrl);

/* Get trace event based EP Recirculate control. */
extern int bcm_switch_ep_recirc_trace_event_control_get(
    int unit, 
    bcm_switch_ep_recirc_trace_event_control_t *trace_event_ctrl);

/* Enable/disable a trace event to trigger EP Recirculate. */
extern int bcm_switch_ep_recirc_trace_event_enable_set(
    int unit, 
    bcm_pkt_trace_event_t trace_event, 
    int enable);

/* Get all enabled trace events for EP Recirculate. */
extern int bcm_switch_ep_recirc_trace_event_enable_get_all(
    int unit, 
    bcm_pkt_trace_event_t *trace_event_array, 
    int *count);

/* Get enabled status of a trace event for EP Recirculate. */
extern int bcm_switch_ep_recirc_trace_event_enable_get(
    int unit, 
    bcm_pkt_trace_event_t trace_event, 
    int *enable);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_SWITCH_WIDE_DATA_EXTENSION_REPLACE 0x1        

typedef struct bcm_switch_wide_data_extension_info_s {
    uint32 wide_data_key; 
    uint64 wide_data_result; 
} bcm_switch_wide_data_extension_info_t;

#ifndef BCM_HIDE_DISPATCHABLE

extern int bcm_switch_wide_data_extension_add(
    int unit, 
    uint32 flags, 
    bcm_switch_wide_data_extension_info_t *info);

extern int bcm_switch_wide_data_extension_get(
    int unit, 
    uint32 flags, 
    bcm_switch_wide_data_extension_info_t *info);

extern int bcm_switch_wide_data_extension_delete(
    int unit, 
    uint32 flags, 
    bcm_switch_wide_data_extension_info_t *info);

#endif /* BCM_HIDE_DISPATCHABLE */

typedef int (*bcm_switch_wide_data_extension_traverse_cb)(
    int unit, 
    uint32 flags, 
    bcm_switch_wide_data_extension_info_t *info, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

extern int bcm_switch_wide_data_extension_traverse(
    int unit, 
    bcm_switch_wide_data_extension_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_SWITCH_H__ */
