/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_FLEXDIGEST_H__
#define __BCM_FLEXDIGEST_H__

#include <bcm/types.h>

#ifndef BCM_HIDE_DISPATCHABLE

/* Initialize the flex digest module. */
extern int bcm_flexdigest_init(
    int unit);

/* Detach the flex-digest module. */
extern int bcm_flexdigest_detach(
    int unit);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Options to control flex digest profiles. */
#define BCM_FLEXDIGEST_PROFILE_CREATE_OPTIONS_WITH_ID (1 << 0)   /* Use input profile Id
                                                          instead of allocating
                                                          one. */

/* The control type for flex digest normalization seed profile. */
typedef enum bcm_flexdigest_norm_seed_control_e {
    bcmFlexDigestNormSeedControlBinASeed = 0, /* The Seed A in normalization seed
                                           profile. */
    bcmFlexDigestNormSeedControlBinBSeed = 1, /* The Seed B in normalization seed
                                           profile. */
    bcmFlexDigestNormSeedControlBinCSeed = 2, /* The Seed C in normalization seed
                                           profile. */
    bcmFlexDigestNormSeedControlCount = 3 /* Always last. Not a usable value. */
} bcm_flexdigest_norm_seed_control_t;

#define BCM_FLEXDIGEST_NORM_SEED_CONTROL_STRINGS \
{ \
    "BinASeed", \
    "BinBSeed", \
    "BinCSeed"  \
}

#ifndef BCM_HIDE_DISPATCHABLE

/* Create a flex digest normalization seed profile. */
extern int bcm_flexdigest_norm_seed_profile_create(
    int unit, 
    uint32 options, 
    int *seed_profile_id);

/* Destroy a flex digest normalization seed profile. */
extern int bcm_flexdigest_norm_seed_profile_destroy(
    int unit, 
    int seed_profile_id);

/* Destroy all flex digest normalization seed profiles. */
extern int bcm_flexdigest_norm_seed_profile_destroy_all(
    int unit);

/* Configure the flex digest normalization seed profile. */
extern int bcm_flexdigest_norm_seed_profile_set(
    int unit, 
    int seed_profile_id, 
    bcm_flexdigest_norm_seed_control_t type, 
    int value);

/* Get the flex digest normalization seed profile configuration. */
extern int bcm_flexdigest_norm_seed_profile_get(
    int unit, 
    int seed_profile_id, 
    bcm_flexdigest_norm_seed_control_t type, 
    int *value);

#endif /* BCM_HIDE_DISPATCHABLE */

/* The control type for flex digest normalization profile. */
typedef enum bcm_flexdigest_norm_profile_control_e {
    bcmFlexDigestNormProfileControlNorm = 0, /* Configure this to enable or disable
                                           normalization. */
    bcmFlexDigestNormProfileControlSeedId = 1, /* The seed profile identifier in
                                           normalization profile. */
    bcmFlexDigestNormProfileControlCount = 2 /* Always last. Not a usable value. */
} bcm_flexdigest_norm_profile_control_t;

#define BCM_FLEXDIGEST_NORM_PROFILE_CONTROL_STRINGS \
{ \
    "Norm", \
    "SeedId"  \
}

#ifndef BCM_HIDE_DISPATCHABLE

/* Create a flex digest normalization profile. */
extern int bcm_flexdigest_norm_profile_create(
    int unit, 
    uint32 options, 
    int *norm_profile_id);

/* Destroy a flex digest normalization profile. */
extern int bcm_flexdigest_norm_profile_destroy(
    int unit, 
    int norm_profile_id);

/* Destroy all flex digest normalization profiles. */
extern int bcm_flexdigest_norm_profile_destroy_all(
    int unit);

/* Configure flex digest normalization profile. */
extern int bcm_flexdigest_norm_profile_set(
    int unit, 
    int norm_profile_id, 
    bcm_flexdigest_norm_profile_control_t type, 
    int value);

/* Get flex digest normalization profile configuration. */
extern int bcm_flexdigest_norm_profile_get(
    int unit, 
    int norm_profile_id, 
    bcm_flexdigest_norm_profile_control_t type, 
    int *value);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * List of flex digest qualifiers.
 * 
 * Flex digest lookup qualifiers are collected into qualifier sets
 * (qsets), which are used to allocate flex digest lookup groups.
 */
typedef enum bcm_flexdigest_qualify_e {
    bcmFlexDigestQualifyMatchId = 0,    /* Qualifies on match identifier. */
    bcmFlexDigestQualifyL4Valid = 1,    /* Qualifies on L4 valid. */
    bcmFlexDigestQualifyMyStation1Hit = 2, /* Qualifies on my station 1 hit. */
    bcmFlexDigestQualifyTunnelProcessingResults1 = 3, /* Qualifies on tunnel processing
                                           results 1. */
    bcmFlexDigestQualifyTunnelProcessingResultsRaw = 4, /* Qualifies on tunnel processing
                                           results raw. */
    bcmFlexDigestQualifyFlexHve2ResultSet1 = 5, /* Qualifies on flex HVE2 result set 1. */
    bcmFlexDigestQualifyFixedHve2ResultSet0 = 6, /* Qualifies on fixed HVE2 result Set 0. */
    bcmFlexDigestQualifyFlexHve1ResultSet1 = 7, /* Qualifies on flex HVE1 result set 1. */
    bcmFlexDigestQualifyFixedHve1ResultSet0 = 8, /* Qualifies on fixed HVE1 result Set 0. */
    bcmFlexDigestQualifyL2IifFlexDigestCtrlIdB = 9, /* Qualifies on l2 iif flex digest ctrl
                                           id B. */
    bcmFlexDigestQualifyL2IifFlexDigestCtrlIdA = 10, /* Qualifies on l2 iif flex digest ctrl
                                           id A. */
    bcmFlexDigestQualifyProtocolPktOpaqueCtrlId = 11, /* Qualifies on protocol pkt opaque ctrl
                                           id. */
    bcmFlexDigestQualifyVfiOpaqueCtrlId = 12, /* Qualifies on vfi opaque ctrl id. */
    bcmFlexDigestQualifySvpOpaqueCtrlId = 13, /* Qualifies on svp opaque ctrl id */
    bcmFlexDigestQualifyL3IifOpaqueCtrlId = 14, /* Qualifies on l3 iif opaque ctrl id. */
    bcmFlexDigestQualifyL2IifOpaqueCtrlId = 15, /* Qualifies on l2 iif opaque ctrl id. */
    bcmFlexDigestQualifyCount = 16      /* Always last. Not a usable value. */
} bcm_flexdigest_qualify_t;

#define BCM_FLEXDIGEST_QUALIFY_STRINGS \
{ \
    "MatchId", \
    "L4Valid", \
    "MyStation1Hit", \
    "TunnelProcessingResults1", \
    "TunnelProcessingResultsRaw", \
    "FlexHve2ResultSet1", \
    "FixedHve2ResultSet0", \
    "FlexHve1ResultSet1", \
    "FixedHve1ResultSet0", \
    "L2IifFlexDigestCtrlIdB", \
    "L2IifFlexDigestCtrlIdA", \
    "ProtocolPktOpaqueCtrlId", \
    "VfiOpaqueCtrlId", \
    "SvpOpaqueCtrlId", \
    "L3IifOpaqueCtrlId", \
    "L2IifOpaqueCtrlId"  \
}

/* 
 * The maximum value of Flex Digest qualifiers.
 * 
 * Use this definition to indicate the maximum value of qualifiers. It is
 * used to .
 */
#define BCM_FLEXDIGEST_QUALIFY_MAX  (bcmFlexDigestQualifyCount) /* Must be >=
                                                  bcmFlexDigestQualifyCount. */

/* 
 * List of flex digest matches.
 * 
 * flex digest lookup matching criteria are associated into logical match
 * identifier, which are used to be qualified in flex digest lookup
 * group.
 */
typedef enum bcm_flexdigest_match_e {
    bcmFlexDigestMatchSysHdrLoopBack = 0, /* Zone0: System Header: Loopback. */
    bcmFlexDigestMatchOuterL2HdrVntag = 1, /* Zone1: Outer L2 Header: Vntag. */
    bcmFlexDigestMatchOuterL2HdrSnapOrLlc = 2, /* Zone1: Outer L2 Header: Snap or LLC. */
    bcmFlexDigestMatchOuterL2HdrOtag = 3, /* Zone1: Outer L2 Header: Otag. */
    bcmFlexDigestMatchOuterL2HdrL2 = 4, /* Zone1: Outer L2 Header: L2. */
    bcmFlexDigestMatchOuterL2HdrItag = 5, /* Zone1: Outer L2 Header: Itag. */
    bcmFlexDigestMatchOuterL2HdrHg3Ext0 = 6, /* Zone1: Outer L2 Header: Highg3 ext 0. */
    bcmFlexDigestMatchOuterL2HdrHg3Base = 7, /* Zone1: Outer L2 Header: Highg3 base. */
    bcmFlexDigestMatchOuterL2HdrEtag = 8, /* Zone1: Outer L2 Header: Etag. */
    bcmFlexDigestMatchOuterL3L4HdrVxlan = 9, /* Zone2: Outer L3 L4 Header: Vxlan. */
    bcmFlexDigestMatchOuterL3L4HdrUnknownL5 = 10, /* Zone2: Outer L3 L4 Header: UnknownL5. */
    bcmFlexDigestMatchOuterL3L4HdrUnknownL4 = 11, /* Zone2: Outer L3 L4 Header: UnknownL4. */
    bcmFlexDigestMatchOuterL3L4HdrUnknownL3 = 12, /* Zone2: Outer L3 L4 Header: UnknownL3. */
    bcmFlexDigestMatchOuterL3L4HdrUdp = 13, /* Zone2: Outer L3 L4 Header: Udp. */
    bcmFlexDigestMatchOuterL3L4HdrTcpLast16Bytes = 14, /* Zone2: Outer L3 L4 Header:
                                           TcpLast16Bytes. */
    bcmFlexDigestMatchOuterL3L4HdrTcpFirst4Bytes = 15, /* Zone2: Outer L3 L4 Header:
                                           TcpFirst4Bytes. */
    bcmFlexDigestMatchOuterL3L4HdrRarp = 16, /* Zone2: Outer L3 L4 Header: Rarp. */
    bcmFlexDigestMatchOuterL3L4HdrP1588 = 17, /* Zone2: Outer L3 L4 Header: P1588. */
    bcmFlexDigestMatchOuterL3L4HdrMplsCw = 18, /* Zone2: Outer L3 L4 Header: MplsCw. */
    bcmFlexDigestMatchOuterL3L4HdrMplsAch = 19, /* Zone2: Outer L3 L4 Header: MplsAch. */
    bcmFlexDigestMatchOuterL3L4HdrMpls6 = 20, /* Zone2: Outer L3 L4 Header: Mpls6. */
    bcmFlexDigestMatchOuterL3L4HdrMpls5 = 21, /* Zone2: Outer L3 L4 Header: Mpls5. */
    bcmFlexDigestMatchOuterL3L4HdrMpls4 = 22, /* Zone2: Outer L3 L4 Header: Mpls4. */
    bcmFlexDigestMatchOuterL3L4HdrMpls3 = 23, /* Zone2: Outer L3 L4 Header: Mpls3. */
    bcmFlexDigestMatchOuterL3L4HdrMpls2 = 24, /* Zone2: Outer L3 L4 Header: Mpls2. */
    bcmFlexDigestMatchOuterL3L4HdrMpls1 = 25, /* Zone2: Outer L3 L4 Header: Mpls1. */
    bcmFlexDigestMatchOuterL3L4HdrMpls0 = 26, /* Zone2: Outer L3 L4 Header: Mpls0. */
    bcmFlexDigestMatchOuterL3L4HdrMacInMac = 27, /* Zone2: Outer L3 L4 Header: MacInMac. */
    bcmFlexDigestMatchOuterL3L4HdrIpv6 = 28, /* Zone2: Outer L3 L4 Header: Ipv6. */
    bcmFlexDigestMatchOuterL3L4HdrIpv4 = 29, /* Zone2: Outer L3 L4 Header: Ipv4. */
    bcmFlexDigestMatchOuterL3L4HdrIgmp = 30, /* Zone2: Outer L3 L4 Header: Igmp. */
    bcmFlexDigestMatchOuterL3L4HdrIcmp = 31, /* Zone2: Outer L3 L4 Header: Icmp. */
    bcmFlexDigestMatchOuterL3L4HdrGreSeq = 32, /* Zone2: Outer L3 L4 Header: GreSeq. */
    bcmFlexDigestMatchOuterL3L4HdrGreRout = 33, /* Zone2: Outer L3 L4 Header: GreRout. */
    bcmFlexDigestMatchOuterL3L4HdrGreKey = 34, /* Zone2: Outer L3 L4 Header: GreKey. */
    bcmFlexDigestMatchOuterL3L4HdrGreChksum = 35, /* Zone2: Outer L3 L4 Header: GreChksum. */
    bcmFlexDigestMatchOuterL3L4HdrGre = 36, /* Zone2: Outer L3 L4 Header: Gre. */
    bcmFlexDigestMatchOuterL3L4HdrGpe = 37, /* Zone2: Outer L3 L4 Header: Gpe. */
    bcmFlexDigestMatchOuterL3L4HdrFragExt2 = 38, /* Zone2: Outer L3 L4 Header: FragExt2. */
    bcmFlexDigestMatchOuterL3L4HdrFragExt1 = 39, /* Zone2: Outer L3 L4 Header: FragExt1. */
    bcmFlexDigestMatchOuterL3L4HdrEtherType = 40, /* Zone2: Outer L3 L4 Header: EtherType. */
    bcmFlexDigestMatchOuterL3L4HdrEspExt = 41, /* Zone2: Outer L3 L4 Header: EspExt. */
    bcmFlexDigestMatchOuterL3L4HdrBfd = 42, /* Zone2: Outer L3 L4 Header: Bfd. */
    bcmFlexDigestMatchOuterL3L4HdrAuthExt2 = 43, /* Zone2: Outer L3 L4 Header: AuthExt2. */
    bcmFlexDigestMatchOuterL3L4HdrAuthExt1 = 44, /* Zone2: Outer L3 L4 Header: AuthExt1. */
    bcmFlexDigestMatchOuterL3L4HdrArp = 45, /* Zone2: Outer L3 L4 Header: Arp. */
    bcmFlexDigestMatchOuterL3L4HdrIfaMetadata = 46, /* Zone2: Outer L3 L4 Header: IFA
                                           metadata. */
    bcmFlexDigestMatchInnerL2HdrVntag = 47, /* Zone3: Inner L2 Header: Vntag. */
    bcmFlexDigestMatchInnerL2HdrSnapOrLlc = 48, /* Zone3: Inner L2 Header: Snap or LLC. */
    bcmFlexDigestMatchInnerL2HdrOtag = 49, /* Zone3: Inner L2 Header: Otag. */
    bcmFlexDigestMatchInnerL2HdrL2 = 50, /* Zone3: Inner L2 Header: L2. */
    bcmFlexDigestMatchInnerL2HdrItag = 51, /* Zone3: Inner L2 Header: Itag. */
    bcmFlexDigestMatchInnerL2HdrEtag = 52, /* Zone3: Inner L2 Header: Etag. */
    bcmFlexDigestMatchInnerL3L4HdrUnknownL5 = 53, /* Zone4: Inner L3 L4 Header: UnknownL5. */
    bcmFlexDigestMatchInnerL3L4HdrUnknownL4 = 54, /* Zone4: Inner L3 L4 Header: UnknownL4. */
    bcmFlexDigestMatchInnerL3L4HdrUnknownL3 = 55, /* Zone4: Inner L3 L4 Header: UnknownL3. */
    bcmFlexDigestMatchInnerL3L4HdrUdp = 56, /* Zone4: Inner L3 L4 Header: Udp. */
    bcmFlexDigestMatchInnerL3L4HdrTcpLast16Bytes = 57, /* Zone4: Inner L3 L4 Header:
                                           TcpLast16Bytes. */
    bcmFlexDigestMatchInnerL3L4HdrTcpFirst4Bytes = 58, /* Zone4: Inner L3 L4 Header:
                                           TcpFirst4Bytes. */
    bcmFlexDigestMatchInnerL3L4HdrRarp = 59, /* Zone4: Inner L3 L4 Header: Rarp. */
    bcmFlexDigestMatchInnerL3L4HdrP1588 = 60, /* Zone4: Inner L3 L4 Header: P1588. */
    bcmFlexDigestMatchInnerL3L4HdrIpv6 = 61, /* Zone4: Inner L3 L4 Header: Ipv6. */
    bcmFlexDigestMatchInnerL3L4HdrIpv4 = 62, /* Zone4: Inner L3 L4 Header: Ipv4. */
    bcmFlexDigestMatchInnerL3L4HdrIgmp = 63, /* Zone4: Inner L3 L4 Header: Igmp. */
    bcmFlexDigestMatchInnerL3L4HdrIcmp = 64, /* Zone4: Inner L3 L4 Header: Icmp. */
    bcmFlexDigestMatchInnerL3L4HdrFragExt2 = 65, /* Zone4: Inner L3 L4 Header: FragExt2. */
    bcmFlexDigestMatchInnerL3L4HdrFragExt1 = 66, /* Zone4: Inner L3 L4 Header: FragExt1. */
    bcmFlexDigestMatchInnerL3L4HdrEtherType = 67, /* Zone4: Inner L3 L4 Header: EtherType. */
    bcmFlexDigestMatchInnerL3L4HdrEspExt = 68, /* Zone4: Inner L3 L4 Header: EspExt. */
    bcmFlexDigestMatchInnerL3L4HdrBfd = 69, /* Zone4: Inner L3 L4 Header: Bfd. */
    bcmFlexDigestMatchInnerL3L4HdrAuthExt2 = 70, /* Zone4: Inner L3 L4 Header: AuthExt2.. */
    bcmFlexDigestMatchInnerL3L4HdrAuthExt1 = 71, /* Zone4: Inner L3 L4 Header: AuthExt1. */
    bcmFlexDigestMatchInnerL3L4HdrArp = 72, /* Zone4: Inner L3 L4 Header: Arp. */
    bcmFlexDigestMatchSysHdrEpNih = 73, /* Zone0: System Header: EP NIH. */
    bcmFlexDigestMatchOuterL2HdrSvtag = 74, /* Zone1: Outer L2 Header: SVTAG. */
    bcmFlexDigestMatchOuterL3L4HdrIfaHeader = 75, /* Zone2: Outer L3 L4 Header: IFA
                                           header. */
    bcmFlexDigestMatchCount = 76        /* Always last. Not a usable value. */
} bcm_flexdigest_match_t;

#define BCM_FLEXDIGEST_MATCH_STRINGS \
{ \
    "SysHdrLoopBack", \
    "OuterL2HdrVntag", \
    "OuterL2HdrSnapOrLlc", \
    "OuterL2HdrOtag", \
    "OuterL2HdrL2", \
    "OuterL2HdrItag", \
    "OuterL2HdrHg3Ext0", \
    "OuterL2HdrHg3Base", \
    "OuterL2HdrEtag", \
    "OuterL3L4HdrVxlan", \
    "OuterL3L4HdrUnknownL5", \
    "OuterL3L4HdrUnknownL4", \
    "OuterL3L4HdrUnknownL3", \
    "OuterL3L4HdrUdp", \
    "OuterL3L4HdrTcpLast16Bytes", \
    "OuterL3L4HdrTcpFirst4Bytes", \
    "OuterL3L4HdrRarp", \
    "OuterL3L4HdrP1588", \
    "OuterL3L4HdrMplsCw", \
    "OuterL3L4HdrMplsAch", \
    "OuterL3L4HdrMpls6", \
    "OuterL3L4HdrMpls5", \
    "OuterL3L4HdrMpls4", \
    "OuterL3L4HdrMpls3", \
    "OuterL3L4HdrMpls2", \
    "OuterL3L4HdrMpls1", \
    "OuterL3L4HdrMpls0", \
    "OuterL3L4HdrMacInMac", \
    "OuterL3L4HdrIpv6", \
    "OuterL3L4HdrIpv4", \
    "OuterL3L4HdrIgmp", \
    "OuterL3L4HdrIcmp", \
    "OuterL3L4HdrGreSeq", \
    "OuterL3L4HdrGreRout", \
    "OuterL3L4HdrGreKey", \
    "OuterL3L4HdrGreChksum", \
    "OuterL3L4HdrGre", \
    "OuterL3L4HdrGpe", \
    "OuterL3L4HdrFragExt2", \
    "OuterL3L4HdrFragExt1", \
    "OuterL3L4HdrEtherType", \
    "OuterL3L4HdrEspExt", \
    "OuterL3L4HdrBfd", \
    "OuterL3L4HdrAuthExt2", \
    "OuterL3L4HdrAuthExt1", \
    "OuterL3L4HdrArp", \
    "OuterL3L4HdrIfaMetadata", \
    "InnerL2HdrVntag", \
    "InnerL2HdrSnapOrLlc", \
    "InnerL2HdrOtag", \
    "InnerL2HdrL2", \
    "InnerL2HdrItag", \
    "InnerL2HdrEtag", \
    "InnerL3L4HdrUnknownL5", \
    "InnerL3L4HdrUnknownL4", \
    "InnerL3L4HdrUnknownL3", \
    "InnerL3L4HdrUdp", \
    "InnerL3L4HdrTcpLast16Bytes", \
    "InnerL3L4HdrTcpFirst4Bytes", \
    "InnerL3L4HdrRarp", \
    "InnerL3L4HdrP1588", \
    "InnerL3L4HdrIpv6", \
    "InnerL3L4HdrIpv4", \
    "InnerL3L4HdrIgmp", \
    "InnerL3L4HdrIcmp", \
    "InnerL3L4HdrFragExt2", \
    "InnerL3L4HdrFragExt1", \
    "InnerL3L4HdrEtherType", \
    "InnerL3L4HdrEspExt", \
    "InnerL3L4HdrBfd", \
    "InnerL3L4HdrAuthExt2", \
    "InnerL3L4HdrAuthExt1", \
    "InnerL3L4HdrArp", \
    "SysHdrEpNih", \
    "OuterL2HdrSvtag", \
    "OuterL3L4HdrIfaHeader"  \
}

/* 
 * List of flex digest actions.
 * 
 * flex digest policy table actions.
 */
typedef enum bcm_flexdigest_action_e {
    bcmFlexDigestActionExtractBinSetACmds0 = 0, /* Extract bin set A commands for bin 0. */
    bcmFlexDigestActionExtractBinSetACmds1 = 1, /* Extract bin set A commands for bin 1. */
    bcmFlexDigestActionExtractBinSetACmds2 = 2, /* Extract bin set A commands for bin 2. */
    bcmFlexDigestActionExtractBinSetACmds3 = 3, /* Extract bin set A commands for bin 3. */
    bcmFlexDigestActionExtractBinSetACmds4 = 4, /* Extract bin set A commands for bin 4. */
    bcmFlexDigestActionExtractBinSetACmds5 = 5, /* Extract bin set A commands for bin 5. */
    bcmFlexDigestActionExtractBinSetACmds6 = 6, /* Extract bin set A commands for bin 6. */
    bcmFlexDigestActionExtractBinSetACmds7 = 7, /* Extract bin set A commands for bin 7. */
    bcmFlexDigestActionExtractBinSetACmds8 = 8, /* Extract bin set A commands for bin 8. */
    bcmFlexDigestActionExtractBinSetACmds9 = 9, /* Extract bin set A commands for bin 9. */
    bcmFlexDigestActionExtractBinSetACmds10 = 10, /* Extract bin set A commands for bin
                                           10. */
    bcmFlexDigestActionExtractBinSetACmds11 = 11, /* Extract bin set A commands for bin
                                           11. */
    bcmFlexDigestActionExtractBinSetACmds12 = 12, /* Extract bin set A commands for bin
                                           12. */
    bcmFlexDigestActionExtractBinSetACmds13 = 13, /* Extract bin set A commands for bin
                                           13. */
    bcmFlexDigestActionExtractBinSetACmds14 = 14, /* Extract bin set A commands for bin
                                           14. */
    bcmFlexDigestActionExtractBinSetACmds15 = 15, /* Extract bin set A commands for bin
                                           15. */
    bcmFlexDigestActionExtractBinSetBCmds0 = 16, /* Extract bin set B commands for bin 0. */
    bcmFlexDigestActionExtractBinSetBCmds1 = 17, /* Extract bin set B commands for bin 1. */
    bcmFlexDigestActionExtractBinSetBCmds2 = 18, /* Extract bin set B commands for bin 2. */
    bcmFlexDigestActionExtractBinSetBCmds3 = 19, /* Extract bin set B commands for bin 3. */
    bcmFlexDigestActionExtractBinSetBCmds4 = 20, /* Extract bin set B commands for bin 4. */
    bcmFlexDigestActionExtractBinSetBCmds5 = 21, /* Extract bin set B commands for bin 5. */
    bcmFlexDigestActionExtractBinSetBCmds6 = 22, /* Extract bin set B commands for bin 6. */
    bcmFlexDigestActionExtractBinSetBCmds7 = 23, /* Extract bin set B commands for bin 7. */
    bcmFlexDigestActionExtractBinSetBCmds8 = 24, /* Extract bin set B commands for bin 8. */
    bcmFlexDigestActionExtractBinSetBCmds9 = 25, /* Extract bin set B commands for bin 9. */
    bcmFlexDigestActionExtractBinSetBCmds10 = 26, /* Extract bin set B commands for bin
                                           10. */
    bcmFlexDigestActionExtractBinSetBCmds11 = 27, /* Extract bin set B commands for bin
                                           11. */
    bcmFlexDigestActionExtractBinSetBCmds12 = 28, /* Extract bin set B commands for bin
                                           12. */
    bcmFlexDigestActionExtractBinSetBCmds13 = 29, /* Extract bin set B commands for bin
                                           13. */
    bcmFlexDigestActionExtractBinSetBCmds14 = 30, /* Extract bin set B commands for bin
                                           14. */
    bcmFlexDigestActionExtractBinSetBCmds15 = 31, /* Extract bin set B commands for bin
                                           15. */
    bcmFlexDigestActionExtractBinSetCCmds0 = 32, /* Extract bin set C commands for bin 0. */
    bcmFlexDigestActionExtractBinSetCCmds1 = 33, /* Extract bin set C commands for bin 1. */
    bcmFlexDigestActionExtractBinSetCCmds2 = 34, /* Extract bin set C commands for bin 2. */
    bcmFlexDigestActionExtractBinSetCCmds3 = 35, /* Extract bin set C commands for bin 3. */
    bcmFlexDigestActionExtractBinSetCCmds4 = 36, /* Extract bin set C commands for bin 4. */
    bcmFlexDigestActionExtractBinSetCCmds5 = 37, /* Extract bin set C commands for bin 5. */
    bcmFlexDigestActionExtractBinSetCCmds6 = 38, /* Extract bin set C commands for bin 6. */
    bcmFlexDigestActionExtractBinSetCCmds7 = 39, /* Extract bin set C commands for bin 7. */
    bcmFlexDigestActionExtractBinSetCCmds8 = 40, /* Extract bin set C commands for bin 8. */
    bcmFlexDigestActionExtractBinSetCCmds9 = 41, /* Extract bin set C commands for bin 9. */
    bcmFlexDigestActionExtractBinSetCCmds10 = 42, /* Extract bin set C commands for bin
                                           10. */
    bcmFlexDigestActionExtractBinSetCCmds11 = 43, /* Extract bin set C commands for bin
                                           11. */
    bcmFlexDigestActionExtractBinSetCCmds12 = 44, /* Extract bin set C commands for bin
                                           12. */
    bcmFlexDigestActionExtractBinSetCCmds13 = 45, /* Extract bin set C commands for bin
                                           13. */
    bcmFlexDigestActionExtractBinSetCCmds14 = 46, /* Extract bin set C commands for bin
                                           14. */
    bcmFlexDigestActionExtractBinSetCCmds15 = 47, /* Extract bin set C commands for bin
                                           15. */
    bcmFlexDigestActionCount = 48       /* Always last. Not a usable value. */
} bcm_flexdigest_action_t;

#define BCM_FLEXDIGEST_ACTION_STRINGS \
{ \
    "ExtractBinSetACmds0", \
    "ExtractBinSetACmds1", \
    "ExtractBinSetACmds2", \
    "ExtractBinSetACmds3", \
    "ExtractBinSetACmds4", \
    "ExtractBinSetACmds5", \
    "ExtractBinSetACmds6", \
    "ExtractBinSetACmds7", \
    "ExtractBinSetACmds8", \
    "ExtractBinSetACmds9", \
    "ExtractBinSetACmds10", \
    "ExtractBinSetACmds11", \
    "ExtractBinSetACmds12", \
    "ExtractBinSetACmds13", \
    "ExtractBinSetACmds14", \
    "ExtractBinSetACmds15", \
    "ExtractBinSetBCmds0", \
    "ExtractBinSetBCmds1", \
    "ExtractBinSetBCmds2", \
    "ExtractBinSetBCmds3", \
    "ExtractBinSetBCmds4", \
    "ExtractBinSetBCmds5", \
    "ExtractBinSetBCmds6", \
    "ExtractBinSetBCmds7", \
    "ExtractBinSetBCmds8", \
    "ExtractBinSetBCmds9", \
    "ExtractBinSetBCmds10", \
    "ExtractBinSetBCmds11", \
    "ExtractBinSetBCmds12", \
    "ExtractBinSetBCmds13", \
    "ExtractBinSetBCmds14", \
    "ExtractBinSetBCmds15", \
    "ExtractBinSetCCmds0", \
    "ExtractBinSetCCmds1", \
    "ExtractBinSetCCmds2", \
    "ExtractBinSetCCmds3", \
    "ExtractBinSetCCmds4", \
    "ExtractBinSetCCmds5", \
    "ExtractBinSetCCmds6", \
    "ExtractBinSetCCmds7", \
    "ExtractBinSetCCmds8", \
    "ExtractBinSetCCmds9", \
    "ExtractBinSetCCmds10", \
    "ExtractBinSetCCmds11", \
    "ExtractBinSetCCmds12", \
    "ExtractBinSetCCmds13", \
    "ExtractBinSetCCmds14", \
    "ExtractBinSetCCmds15"  \
}

/* Flex digest Policy table Command Field Selector. */
#define BCM_FLEXDIGEST_FIELD_OUTER_MACSA_0_15 1          /* The outer MACSA 0-15
                                                          bits. */
#define BCM_FLEXDIGEST_FIELD_OUTER_MACSA_16_31 2          /* The outer MACSA 16-31
                                                          bits. */
#define BCM_FLEXDIGEST_FIELD_OUTER_MACSA_32_47 3          /* The outer MACSA 32-47
                                                          bits. */
#define BCM_FLEXDIGEST_FIELD_OUTER_MACDA_0_15 4          /* The outer MACDA 0-15
                                                          bits. */
#define BCM_FLEXDIGEST_FIELD_OUTER_MACDA_16_31 5          /* The outer MACDA 16-31
                                                          bits. */
#define BCM_FLEXDIGEST_FIELD_OUTER_MACDA_32_47 6          /* The outer MACDA 32-47
                                                          bits. */
#define BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_0_15 7          /* The outer IP HDR SIP
                                                          0-15 bits. */
#define BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_16_31 8          /* The outer IP HDR SIP
                                                          16-31 bits. */
#define BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_32_47 9          /* The outer IP HDR SIP
                                                          32-47 bits. */
#define BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_48_63 10         /* The outer IP HDR SIP
                                                          48-63 bits. */
#define BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_64_79 11         /* The outer IP HDR SIP
                                                          64-79 bits. */
#define BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_80_95 12         /* The outer IP HDR SIP
                                                          80-95 bits. */
#define BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_96_111 13         /* The outer IP HDR SIP
                                                          96-111 bits. */
#define BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_112_127 14         /* The outer IP HDR SIP
                                                          112-127 bits. */
#define BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_0_15 15         /* The outer IP HDR DIP
                                                          0-15 bits. */
#define BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_16_31 16         /* The outer IP HDR DIP
                                                          16-31 bits. */
#define BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_32_47 17         /* The outer IP HDR DIP
                                                          32-47 bits. */
#define BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_48_63 18         /* The outer IP HDR DIP
                                                          48-63 bits. */
#define BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_64_79 19         /* The outer IP HDR DIP
                                                          64-79 bits. */
#define BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_80_95 20         /* The outer IP HDR DIP
                                                          80-95 bits. */
#define BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_96_111 21         /* The outer IP HDR DIP
                                                          96-111 bits. */
#define BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_112_127 22         /* The outer IP HDR DIP
                                                          112-127 bits. */
#define BCM_FLEXDIGEST_FIELD_OUTER_ITAG_0_15 23         /* The outer ITAG 0-15
                                                          bits. */
#define BCM_FLEXDIGEST_FIELD_OUTER_ITAG_16_31 24         /* The outer ITAG 16-31
                                                          bits. */
#define BCM_FLEXDIGEST_FIELD_OUTER_VXLAN_VN_ID_RESERVED_2_0_15 25         /* The outer VXLAN VN ID
                                                          RESERVED 2 0-15 bits. */
#define BCM_FLEXDIGEST_FIELD_OUTER_VXLAN_VN_ID_RESERVED_2_16_31 26         /* The outer VXLAN VN ID
                                                          RESERVED 2 16-31 bits. */
#define BCM_FLEXDIGEST_FIELD_OUTER_OTAG_0_15 27         /* The outer OTAG 0-15
                                                          bits. */
#define BCM_FLEXDIGEST_FIELD_OUTER_OTAG_16_31 28         /* The outer OTAG 16-31
                                                          bits. */
#define BCM_FLEXDIGEST_FIELD_OUTER_L4_DST_PORT 29         /* The outer L4 DST PORT. */
#define BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_PROTOCOL 30         /* The outer IP HDR
                                                          PROTOCOL. */
#define BCM_FLEXDIGEST_FIELD_OUTER_ETHERTYPE 31         /* The outer ETHERTYPE. */
#define BCM_FLEXDIGEST_FIELD_OUTER_L4_SRC_PORT 32         /* The outer L4 SRC PORT. */
#define BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_FIRST_TWO_BYTES 33         /* The outer IP HDR FIRST
                                                          TWO BYTES. */
#define BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_FLOW_LABEL 34         /* The outer IP HDR FLOW
                                                          LABEL. */
#define BCM_FLEXDIGEST_FIELD_OUTER_IP_EXT_HDR1_PROTOCOL 35         /* The outer IP EXT HDR1
                                                          PROTOCOL. */
#define BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_TTL 36         /* The outer IP HDR TTL. */
#define BCM_FLEXDIGEST_FIELD_OUTER_IP_EXT_HDR2_PROTOCOL 37         /* The outer IP EXT HDR2
                                                          PROTOCOL. */
#define BCM_FLEXDIGEST_FIELD_OUTER_L5_BYTES_4_7_0_15 38         /* The outer L5 BYTES 4
                                                          to 7 0-15 bits. */
#define BCM_FLEXDIGEST_FIELD_OUTER_L5_BYTES_4_7_16_31 39         /* The outer L5 BYTES 4
                                                          to 7 16-31 bits. */
#define BCM_FLEXDIGEST_FIELD_OUTER_IP_LAST_PROTOCOL 40         /* The outer IP last
                                                          protocol. */
#define BCM_FLEXDIGEST_FIELD_INNER_MACSA_0_15 41         /* The inner MACSA 0-15
                                                          bits. */
#define BCM_FLEXDIGEST_FIELD_INNER_MACSA_16_31 42         /* The inner MACSA 16-31
                                                          bits. */
#define BCM_FLEXDIGEST_FIELD_INNER_MACSA_32_47 43         /* The inner MACSA 32-47
                                                          bits. */
#define BCM_FLEXDIGEST_FIELD_INNER_MACDA_0_15 44         /* The inner MACDA 0-15
                                                          bits. */
#define BCM_FLEXDIGEST_FIELD_INNER_MACDA_16_31 45         /* The inner MACDA 16-31
                                                          bits. */
#define BCM_FLEXDIGEST_FIELD_INNER_MACDA_32_47 46         /* The inner MACDA 32-47
                                                          bits. */
#define BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_SIP_0_15 47         /* The inner IP HDR SIP
                                                          0-15 bits. */
#define BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_SIP_16_31 48         /* The inner IP HDR SIP
                                                          16-31 bits. */
#define BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_SIP_32_47 49         /* The inner IP HDR SIP
                                                          32-47 bits. */
#define BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_SIP_48_63 50         /* The inner IP HDR SIP
                                                          48-63 bits. */
#define BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_SIP_64_79 51         /* The inner IP HDR SIP
                                                          64-79 bits. */
#define BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_SIP_80_95 52         /* The inner IP HDR SIP
                                                          80-95 bits. */
#define BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_SIP_96_111 53         /* The inner IP HDR SIP
                                                          96-111 bits. */
#define BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_SIP_112_127 54         /* The inner IP HDR SIP
                                                          112-127 bits. */
#define BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_DIP_0_15 55         /* The inner IP HDR DIP
                                                          0-15 bits. */
#define BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_DIP_16_31 56         /* The inner IP HDR DIP
                                                          16-31 bits. */
#define BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_DIP_32_47 57         /* The inner IP HDR DIP
                                                          32-47 bits. */
#define BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_DIP_48_63 58         /* The inner IP HDR DIP
                                                          48-63 bits. */
#define BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_DIP_64_79 59         /* The inner IP HDR DIP
                                                          64-79 bits. */
#define BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_DIP_80_95 60         /* The inner IP HDR DIP
                                                          80-95 bits. */
#define BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_DIP_96_111 61         /* The inner IP HDR DIP
                                                          96-111 bits. */
#define BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_DIP_112_127 62         /* The inner IP HDR DIP
                                                          112-127 bits. */
#define BCM_FLEXDIGEST_FIELD_INNER_ITAG_0_15 63         /* The inner ITAG 0-15
                                                          bits. */
#define BCM_FLEXDIGEST_FIELD_INNER_ITAG_16_31 64         /* The inner ITAG 16-31
                                                          bits. */
#define BCM_FLEXDIGEST_FIELD_INNER_OTAG_0_15 65         /* The inner OTAG 0-15
                                                          bits. */
#define BCM_FLEXDIGEST_FIELD_INNER_OTAG_16_31 66         /* The inner OTAG 16-31
                                                          bits. */
#define BCM_FLEXDIGEST_FIELD_INNER_L4_DST_PORT 67         /* The inner L4 DST PORT. */
#define BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_PROTOCOL 68         /* The inner IP HDR
                                                          PROTOCOL. */
#define BCM_FLEXDIGEST_FIELD_INNER_ETHERTYPE 69         /* The inner ETHERTYPE. */
#define BCM_FLEXDIGEST_FIELD_INNER_L4_SRC_PORT 70         /* The inner L4 SRC PORT. */
#define BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_FIRST_TWO_BYTES 71         /* The inner IP HDR FIRST
                                                          TWO BYTES. */
#define BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_FLOW_LABEL 72         /* The inner IP HDR FLOW
                                                          LABEL. */
#define BCM_FLEXDIGEST_FIELD_INNER_IP_EXT_HDR1_PROTOCOL 73         /* The inner IP EXT HDR1
                                                          PROTOCOL. */
#define BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_TTL 74         /* The inner IP HDR TTL. */
#define BCM_FLEXDIGEST_FIELD_INNER_IP_EXT_HDR2_PROTOCOL 75         /* The inner IP EXT HDR2
                                                          PROTOCOL. */
#define BCM_FLEXDIGEST_FIELD_INNER_L5_BYTES_4_7_0_15 76         /* The inner L5 BYTES 4
                                                          to 7 0-15 bits. */
#define BCM_FLEXDIGEST_FIELD_INNER_L5_BYTES_4_7_16_31 77         /* The inner L5 BYTES 4
                                                          to 7 16-31 bits. */
#define BCM_FLEXDIGEST_FIELD_INNER_IP_LAST_PROTOCOL 78         /* The inner IP last
                                                          protocol. */
#define BCM_FLEXDIGEST_FIELD_SYSTEM_SOURCE  79         /* The system source. */
#define BCM_FLEXDIGEST_FIELD_INGRESS_PP_PORT 80         /* The ingress pp port. */
#define BCM_FLEXDIGEST_FIELD_L2_IIF         81         /* The L2 IIF. */
#define BCM_FLEXDIGEST_FIELD_L3_IIF         82         /* The L3 IIF. */
#define BCM_FLEXDIGEST_FIELD_SVP            83         /* The SVP. */
#define BCM_FLEXDIGEST_FIELD_VFI            84         /* The VFI. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_OUTER_XOR_SIP_0_15 85         /* The normalized outer
                                                          xor SIP 0-15 bits. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_OUTER_XOR_SIP_16_31 86         /* The normalized outer
                                                          xor SIP 16-31 bits. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_OUTER_XOR_DIP_0_15 87         /* The normalized outer
                                                          xor DIP 0-15 bits. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_OUTER_XOR_DIP_16_31 88         /* The normalized outer
                                                          xor DIP 16-31 bits. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_INNER_XOR_SIP_0_15 89         /* The normalized inner
                                                          xor SIP 0-15 bits. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_INNER_XOR_SIP_16_31 90         /* The normalized inner
                                                          xor SIP 16-31 bits. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_INNER_XOR_DIP_0_15 91         /* The normalized inner
                                                          xor DIP 0-15 bits. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_INNER_XOR_DIP_16_31 92         /* The normalized inner
                                                          xor DIP 16-31 bits. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_OUTER_IP_ADDR_XOR_HI_0_15 93         /* The normalized outer
                                                          IP addr xor high 0-15
                                                          bits. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_OUTER_IP_ADDR_XOR_HI_16_31 94         /* The normalized outer
                                                          IP addr xor high 16-31
                                                          bits. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_OUTER_IP_ADDR_XOR_LO_0_15 95         /* The normalized outer
                                                          IP addr xor low 0-15
                                                          bits. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_OUTER_IP_ADDR_XOR_LO_16_31 96         /* The normalized outer
                                                          IP addr xor low 16-31
                                                          bits. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_INNER_IP_ADDR_XOR_HI_0_15 97         /* The normalized inner
                                                          IP addr xor high 0-15
                                                          bits. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_INNER_IP_ADDR_XOR_HI_16_31 98         /* The normalized inner
                                                          IP addr xor high 16-31
                                                          bits. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_INNER_IP_ADDR_XOR_LO_0_15 99         /* The normalized inner
                                                          IP addr xor low 0-15
                                                          bits. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_INNER_IP_ADDR_XOR_LO_16_31 100        /* The normalized inner
                                                          IP addr xor low 16-31
                                                          bits. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_OUTER_IP_ADDR_LSB32_HI_0_15 101        /* The normalized outer
                                                          IP addr lsb32 high
                                                          0-15 bits. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_OUTER_IP_ADDR_LSB32_HI_16_31 102        /* The normalized outer
                                                          IP addr lsb32 high
                                                          16-31 bits. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_OUTER_IP_ADDR_LSB32_LO_0_15 103        /* The normalized outer
                                                          IP addr lsb32 low 0-15
                                                          bits. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_OUTER_IP_ADDR_LSB32_LO_16_31 104        /* The normalized outer
                                                          IP addr lsb32 low
                                                          16-31 bits. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_INNER_IP_ADDR_LSB32_HI_0_15 105        /* The normalized inner
                                                          IP addr lsb32 high
                                                          0-15 bits. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_INNER_IP_ADDR_LSB32_HI_16_31 106        /* The normalized inner
                                                          IP addr lsb32 high
                                                          16-31 bits. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_INNER_IP_ADDR_LSB32_LO_0_15 107        /* The normalized inner
                                                          IP addr lsb32 low 0-15
                                                          bits. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_INNER_IP_ADDR_LSB32_LO_16_31 108        /* The normalized inner
                                                          IP addr lsb32 low
                                                          16-31 bits. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_OUTER_L4_PORT_XOR_HI 109        /* The normalized outer
                                                          L4 port xor high. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_OUTER_L4_PORT_XOR_LO 110        /* The normalized outer
                                                          L4 port xor low. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_INNER_L4_PORT_XOR_HI 111        /* The normalized inner
                                                          L4 port xor high. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_INNER_L4_PORT_XOR_LO 112        /* The normalized inner
                                                          L4 port xor low. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_OUTER_L4_PORT_LSB32_HI 113        /* The normalized outer
                                                          L4 port lsb high. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_OUTER_L4_PORT_LSB32_LO 114        /* The normalized outer
                                                          L4 port lsb low. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_INNER_L4_PORT_LSB32_HI 115        /* The normalized inner
                                                          L4 port lsb high. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_INNER_L4_PORT_LSB32_LO 116        /* The normalized inner
                                                          L4 port lsb low. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_SEED_A_0_15 117        /* The normalized see A
                                                          0-15 bits. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_SEED_A_16_31 118        /* The normalized see A
                                                          16-31 bits. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_SEED_B_0_15 119        /* The normalized see B
                                                          0-15 bits. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_SEED_B_16_31 120        /* The normalized see B
                                                          16-31 bits. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_SEED_C_0_15 121        /* The normalized see C
                                                          0-15 bits. */
#define BCM_FLEXDIGEST_FIELD_NORMALIZED_SEED_C_16_31 122        /* The normalized see C
                                                          16-31 bits. */
#define BCM_FLEXDIGEST_FIELD_OUTER_UDF_TWO_BYTE_0 123        /* The outer UDF two
                                                          bytes container 0. */
#define BCM_FLEXDIGEST_FIELD_OUTER_UDF_TWO_BYTE_1 124        /* The outer UDF two
                                                          bytes container 1. */
#define BCM_FLEXDIGEST_FIELD_OUTER_UDF_TWO_BYTE_2 125        /* The outer UDF two
                                                          bytes container 2. */
#define BCM_FLEXDIGEST_FIELD_OUTER_UDF_TWO_BYTE_3 126        /* The outer UDF two
                                                          bytes container 3. */
#define BCM_FLEXDIGEST_FIELD_OUTER_UDF_TWO_BYTE_4 127        /* The outer UDF two
                                                          bytes container 4. */
#define BCM_FLEXDIGEST_FIELD_OUTER_UDF_TWO_BYTE_5 128        /* The outer UDF two
                                                          bytes container 5. */
#define BCM_FLEXDIGEST_FIELD_OUTER_UDF_TWO_BYTE_6 129        /* The outer UDF two
                                                          bytes container 6. */
#define BCM_FLEXDIGEST_FIELD_OUTER_UDF_TWO_BYTE_7 130        /* The outer UDF two
                                                          bytes container 7. */
#define BCM_FLEXDIGEST_FIELD_OUTER_UDF_TWO_BYTE_8 131        /* The outer UDF two
                                                          bytes container 8. */
#define BCM_FLEXDIGEST_FIELD_OUTER_UDF_TWO_BYTE_9 132        /* The outer UDF two
                                                          bytes container 9. */
#define BCM_FLEXDIGEST_FIELD_OUTER_UDF_ONE_BYTE_0 133        /* The outer UDF one byte
                                                          container 0. */
#define BCM_FLEXDIGEST_FIELD_OUTER_UDF_ONE_BYTE_1 134        /* The outer UDF one byte
                                                          container 1. */
#define BCM_FLEXDIGEST_FIELD_OUTER_UDF_ONE_BYTE_2 135        /* The outer UDF one byte
                                                          container 2. */
#define BCM_FLEXDIGEST_FIELD_OUTER_UDF_ONE_BYTE_3 136        /* The outer UDF one byte
                                                          container 3. */
#define BCM_FLEXDIGEST_FIELD_OUTER_UDF_ONE_BYTE_4 137        /* The outer UDF one byte
                                                          container 4. */
#define BCM_FLEXDIGEST_FIELD_INNER_UDF_TWO_BYTE_0 138        /* The inner UDF two
                                                          bytes container 0. */
#define BCM_FLEXDIGEST_FIELD_INNER_UDF_TWO_BYTE_1 139        /* The inner UDF two
                                                          bytes container 1. */
#define BCM_FLEXDIGEST_FIELD_INNER_UDF_TWO_BYTE_2 140        /* The inner UDF two
                                                          bytes container 2. */
#define BCM_FLEXDIGEST_FIELD_INNER_UDF_TWO_BYTE_3 141        /* The inner UDF two
                                                          bytes container 3. */
#define BCM_FLEXDIGEST_FIELD_INNER_UDF_TWO_BYTE_4 142        /* The inner UDF two
                                                          bytes container 4. */
#define BCM_FLEXDIGEST_FIELD_INNER_UDF_TWO_BYTE_5 143        /* The inner UDF two
                                                          bytes container 5. */
#define BCM_FLEXDIGEST_FIELD_INNER_UDF_TWO_BYTE_6 144        /* The inner UDF two
                                                          bytes container 6. */
#define BCM_FLEXDIGEST_FIELD_INNER_UDF_TWO_BYTE_7 145        /* The inner UDF two
                                                          bytes container 7. */
#define BCM_FLEXDIGEST_FIELD_INNER_UDF_TWO_BYTE_8 146        /* The inner UDF two
                                                          bytes container 8. */
#define BCM_FLEXDIGEST_FIELD_INNER_UDF_TWO_BYTE_9 147        /* The inner UDF two
                                                          bytes container 9. */
#define BCM_FLEXDIGEST_FIELD_INNER_UDF_ONE_BYTE_0 148        /* The inner UDF one byte
                                                          container 0. */
#define BCM_FLEXDIGEST_FIELD_INNER_UDF_ONE_BYTE_1 149        /* The inner UDF one byte
                                                          container 1. */
#define BCM_FLEXDIGEST_FIELD_INNER_UDF_ONE_BYTE_2 150        /* The inner UDF one byte
                                                          container 2. */
#define BCM_FLEXDIGEST_FIELD_INNER_UDF_ONE_BYTE_3 151        /* The inner UDF one byte
                                                          container 3. */
#define BCM_FLEXDIGEST_FIELD_INNER_UDF_ONE_BYTE_4 152        /* The inner UDF one byte
                                                          container 4. */

/* Flex digest qset defines the set of fields used for lookup. */
typedef struct bcm_flexdigest_qset_s {
    SHR_BITDCL w[_SHR_BITDCLSIZE(BCM_FLEXDIGEST_QUALIFY_MAX)]; 
} bcm_flexdigest_qset_t;

/* 
 * The flex digest qualifier set operations.
 * 
 * The flex digest qualifiers are collected into qualifier sets (qsets)
 * which are used to allocate flex digest groups. The type
 * bcm_flexdigest_qset_t is an opaque collection of these requested
 * group capabilities.
 * 
 * The following macros are used to initialize a qualifier set and add,
 * remove or test individual qualifier bits in the qualifier set.
 */
#define BCM_FLEXDIGEST_QSET_INIT(qset)  \
    sal_memset(&(qset), 0, sizeof(bcm_flexdigest_qset_t)) /* Clear a bcm_flexdigest_qset_t. */
#define BCM_FLEXDIGEST_QSET_ADD(qset, q)  \
    SHR_BITSET(((qset).w), (q)) /* Add a qualifier into qualifier
                                                  set. */
#define BCM_FLEXDIGEST_QSET_REMOVE(qset, q)  \
    SHR_BITCLR(((qset).w), (q)) /* Remove a qualifier from
                                                  qualifier set. */
#define BCM_FLEXDIGEST_QSET_TEST(qset, q)  \
    SHR_BITGET(((qset).w), (q)) /* To test whether a qualifier
                                                  exists in qualifier set or
                                                  not. */

/* 
 * Group Priority.
 * 
 * Numerically higher values are higher priority. Each priority value may
 * be used only once. BCM_FLEXDIGEST_GROUP_PRIO_ANY can be used to have a
 * priority assigned automatically.
 */
#define BCM_FLEXDIGEST_GROUP_PRIO_ANY   (-1)       /* To indicate that the
                                                      priority is assigned
                                                      automatically. */

/* Initialize the flex digest Qualifier Set structure. */
extern void bcm_flexdigest_qset_t_init(
    bcm_flexdigest_qset_t *qset);

/* Opaque handle to a flex digest group. */
typedef int bcm_flexdigest_group_t;

/* Opaque handle to a flex digest entry. */
typedef int bcm_flexdigest_entry_t;

/* Opaque handle to a flex digest logical match identifier. */
typedef uint16 bcm_flexdigest_match_id_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Create a flex digest group. */
extern int bcm_flexdigest_group_create(
    int unit, 
    bcm_flexdigest_qset_t qset, 
    int pri, 
    bcm_flexdigest_group_t *group);

/* Create a flex digest group. */
extern int bcm_flexdigest_group_create_id(
    int unit, 
    bcm_flexdigest_qset_t qset, 
    int pri, 
    bcm_flexdigest_group_t group);

/* Get the flex digest group configuration. */
extern int bcm_flexdigest_group_get(
    int unit, 
    bcm_flexdigest_group_t group, 
    bcm_flexdigest_qset_t *qset, 
    int *pri);

/* Destroy a flex digest group. */
extern int bcm_flexdigest_group_destroy(
    int unit, 
    bcm_flexdigest_group_t group);

/* Destroy all flex digest groups. */
extern int bcm_flexdigest_group_destroy_all(
    int unit);

/* Create a flex digest lookup entry for a given groups. */
extern int bcm_flexdigest_entry_create(
    int unit, 
    bcm_flexdigest_group_t group, 
    bcm_flexdigest_entry_t *entry);

/* Create a flex digest lookup entry with entry Identifier. */
extern int bcm_flexdigest_entry_create_id(
    int unit, 
    bcm_flexdigest_group_t group, 
    bcm_flexdigest_entry_t entry);

/* Destroy a flex digest lookup entry. */
extern int bcm_flexdigest_entry_destroy(
    int unit, 
    bcm_flexdigest_entry_t entry);

/* Destroy all flex digest lookup entries in a group. */
extern int bcm_flexdigest_entry_destroy_all(
    int unit, 
    bcm_flexdigest_group_t group);

/* Configure a flex digest lookup entry priority. */
extern int bcm_flexdigest_entry_priority_set(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    int pri);

/* Get the flex digest lookup entry priority. */
extern int bcm_flexdigest_entry_priority_get(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    int *pri);

/* Install a flex digest lookup entry into hardware. */
extern int bcm_flexdigest_entry_install(
    int unit, 
    bcm_flexdigest_entry_t entry);

/* Reinstall a flex digest lookup entry into hardware. */
extern int bcm_flexdigest_entry_reinstall(
    int unit, 
    bcm_flexdigest_entry_t entry);

/* Remove a flex digest lookup entry from hardware. */
extern int bcm_flexdigest_entry_remove(
    int unit, 
    bcm_flexdigest_entry_t entry);

/* Create a logical match identifier for flex digest lookup entry key. */
extern int bcm_flexdigest_match_id_create(
    int unit, 
    bcm_flexdigest_match_id_t *match_id);

/* Destroy a logical match identifier. */
extern int bcm_flexdigest_match_id_destroy(
    int unit, 
    bcm_flexdigest_match_id_t match_id);

/* Destroy all logical match identifiers. */
extern int bcm_flexdigest_match_id_destroy_all(
    int unit);

/* Add a matching criteria into a logical match identifiers. */
extern int bcm_flexdigest_match_add(
    int unit, 
    bcm_flexdigest_match_id_t match_id, 
    bcm_flexdigest_match_t match);

/* Delete a matching rule from a logical match identifiers. */
extern int bcm_flexdigest_match_delete(
    int unit, 
    bcm_flexdigest_match_id_t match_id, 
    bcm_flexdigest_match_t match);

/* 
 * Delete all matching criterias associated with a logical match
 * identifiers.
 */
extern int bcm_flexdigest_match_delete_all(
    int unit, 
    bcm_flexdigest_match_id_t match_id);

/* Get the matching criterias from a logical match identifiers. */
extern int bcm_flexdigest_match_get(
    int unit, 
    bcm_flexdigest_match_id_t match_id, 
    int array_size, 
    bcm_flexdigest_match_t *match_array, 
    int *count);

/* Add MatchId qualifier into a flex digest entry. */
extern int bcm_flexdigest_qualify_MatchId(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    bcm_flexdigest_match_id_t match_id);

/* Get the data/mask configuration for MatchId qualifier. */
extern int bcm_flexdigest_qualify_MatchId_get(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    bcm_flexdigest_match_id_t *match_id);

/* Add L4Valid qualifier into a flex digest entry. */
extern int bcm_flexdigest_qualify_L4Valid(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    uint8 data, 
    uint8 mask);

/* Get the data/mask configuration for L4Valid qualifier. */
extern int bcm_flexdigest_qualify_L4Valid_get(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    uint8 *data, 
    uint8 *mask);

/* Add MyStation1Hit qualifier into a flex digest entry. */
extern int bcm_flexdigest_qualify_MyStation1Hit(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    uint8 data, 
    uint8 mask);

/* Get the data/mask configuration for MyStation1Hit qualifier. */
extern int bcm_flexdigest_qualify_MyStation1Hit_get(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    uint8 *data, 
    uint8 *mask);

/* Add TunnelProcessingResults1 qualifier into a flex digest entry. */
extern int bcm_flexdigest_qualify_TunnelProcessingResults1(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    uint8 data, 
    uint8 mask);

/* 
 * Get the data/mask configuration for TunnelProcessingResults1
 * qualifier.
 */
extern int bcm_flexdigest_qualify_TunnelProcessingResults1_get(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    uint8 *data, 
    uint8 *mask);

/* Add TunnelProcessingResultsRaw qualifier into a flex digest entry. */
extern int bcm_flexdigest_qualify_TunnelProcessingResultsRaw(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    uint8 data, 
    uint8 mask);

/* 
 * Get the data/mask configuration for TunnelProcessingResultsRaw
 * qualifier.
 */
extern int bcm_flexdigest_qualify_TunnelProcessingResultsRaw_get(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    uint8 *data, 
    uint8 *mask);

/* Add FlexHve2ResultSet1 qualifier into a flex digest entry. */
extern int bcm_flexdigest_qualify_FlexHve2ResultSet1(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    uint8 data, 
    uint8 mask);

/* Get the data/mask configuration for FlexHve2ResultSet1 qualifier. */
extern int bcm_flexdigest_qualify_FlexHve2ResultSet1_get(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    uint8 *data, 
    uint8 *mask);

/* Add FixedHve2ResultSet0 qualifier into a flex digest entry. */
extern int bcm_flexdigest_qualify_FixedHve2ResultSet0(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    uint8 data, 
    uint8 mask);

/* Get the data/mask configuration for FixedHve2ResultSet0 qualifier. */
extern int bcm_flexdigest_qualify_FixedHve2ResultSet0_get(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    uint8 *data, 
    uint8 *mask);

/* Add FlexHve1ResultSet1 qualifier into a flex digest entry. */
extern int bcm_flexdigest_qualify_FlexHve1ResultSet1(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    uint8 data, 
    uint8 mask);

/* Get the data/mask configuration for FlexHve1ResultSet1 qualifier. */
extern int bcm_flexdigest_qualify_FlexHve1ResultSet1_get(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    uint8 *data, 
    uint8 *mask);

/* Add FixedHve1ResultSet0 qualifier into a flex digest entry. */
extern int bcm_flexdigest_qualify_FixedHve1ResultSet0(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    uint8 data, 
    uint8 mask);

/* Get the data/mask configuration for FixedHve1ResultSet0 qualifier. */
extern int bcm_flexdigest_qualify_FixedHve1ResultSet0_get(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    uint8 *data, 
    uint8 *mask);

/* Add L2IifFlexDigestCtrlIdB qualifier into a flex digest entry. */
extern int bcm_flexdigest_qualify_L2IifFlexDigestCtrlIdB(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    uint8 data, 
    uint8 mask);

/* Get the data/mask configuration for L2IifFlexDigestCtrlIdB qualifier. */
extern int bcm_flexdigest_qualify_L2IifFlexDigestCtrlIdB_get(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    uint8 *data, 
    uint8 *mask);

/* Add L2IifFlexDigestCtrlIdA qualifier into a flex digest entry. */
extern int bcm_flexdigest_qualify_L2IifFlexDigestCtrlIdA(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    uint8 data, 
    uint8 mask);

/* Get the data/mask configuration for L2IifFlexDigestCtrlIdA qualifier. */
extern int bcm_flexdigest_qualify_L2IifFlexDigestCtrlIdA_get(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    uint8 *data, 
    uint8 *mask);

/* Add ProtocolPktOpaqueCtrlId qualifier into a flex digest entry. */
extern int bcm_flexdigest_qualify_ProtocolPktOpaqueCtrlId(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    uint8 data, 
    uint8 mask);

/* Get the data/mask configuration for ProtocolPktOpaqueCtrlId qualifier. */
extern int bcm_flexdigest_qualify_ProtocolPktOpaqueCtrlId_get(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    uint8 *data, 
    uint8 *mask);

/* Add VfiOpaqueCtrlId qualifier into a flex digest entry. */
extern int bcm_flexdigest_qualify_VfiOpaqueCtrlId(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    uint8 data, 
    uint8 mask);

/* Get the data/mask configuration for VfiOpaqueCtrlId qualifier. */
extern int bcm_flexdigest_qualify_VfiOpaqueCtrlId_get(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    uint8 *data, 
    uint8 *mask);

/* Add SvpOpaqueCtrlId qualifier into a flex digest entry. */
extern int bcm_flexdigest_qualify_SvpOpaqueCtrlId(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    uint8 data, 
    uint8 mask);

/* Get the data/mask configuration for SvpOpaqueCtrlId qualifier. */
extern int bcm_flexdigest_qualify_SvpOpaqueCtrlId_get(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    uint8 *data, 
    uint8 *mask);

/* Add L3IifOpaqueCtrlId qualifier into a flex digest entry. */
extern int bcm_flexdigest_qualify_L3IifOpaqueCtrlId(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    uint8 data, 
    uint8 mask);

/* Get the data/mask configuration for L3IifOpaqueCtrlId qualifier. */
extern int bcm_flexdigest_qualify_L3IifOpaqueCtrlId_get(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    uint8 *data, 
    uint8 *mask);

/* Add L2IifOpaqueCtrlId qualifier into a flex digest entry. */
extern int bcm_flexdigest_qualify_L2IifOpaqueCtrlId(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    uint8 data, 
    uint8 mask);

/* Get the data/mask configuration for L2IifOpaqueCtrlId qualifier. */
extern int bcm_flexdigest_qualify_L2IifOpaqueCtrlId_get(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    uint8 *data, 
    uint8 *mask);

/* Remove match criteria from a flex digest processor entry. */
extern int bcm_flexdigest_qualifier_delete(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    bcm_flexdigest_qualify_t qual_id);

/* Delete a qualifier from a flex digest entry. */
extern int bcm_flexdigest_qualifier_delete_all(
    int unit, 
    bcm_flexdigest_entry_t entry);

/* Add an action and parameters into a lookup entry. */
extern int bcm_flexdigest_action_add(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    bcm_flexdigest_action_t action, 
    uint16 param0, 
    uint16 param1);

/* Delete an action and parameters from a lookup entry. */
extern int bcm_flexdigest_action_delete(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    bcm_flexdigest_action_t action);

/* Delete all actions from a lookup entry. */
extern int bcm_flexdigest_action_delete_all(
    int unit, 
    bcm_flexdigest_entry_t entry);

/* Get an action parameters from a lookup entry. */
extern int bcm_flexdigest_action_get(
    int unit, 
    bcm_flexdigest_entry_t entry, 
    bcm_flexdigest_action_t action, 
    uint16 *param0, 
    uint16 *param1);

#if defined(BROADCOM_DEBUG)
/* Show current software state of the flexdigest module. */
extern int bcm_flexdigest_show(
    int unit, 
    const char *pfx);
#endif

#if defined(BROADCOM_DEBUG)
/* Show contents of a flexdigest group. */
extern int bcm_flexdigest_group_dump(
    int unit, 
    bcm_field_group_t group);
#endif

#if defined(BROADCOM_DEBUG)
/* Show contents of a flexdigest entry. */
extern int bcm_flexdigest_entry_dump(
    int unit, 
    bcm_field_entry_t entry);
#endif

#endif /* BCM_HIDE_DISPATCHABLE */

/* List of FlexDigest functions: used only in XFS devices */
#define BCM_FLEXDIGEST_HASH_FUNCTION_CRC16  1          /* 16-bit CRC16. */
#define BCM_FLEXDIGEST_HASH_FUNCTION_CRC16XOR1 2          /* 8-bit CRC16 + 8-bit
                                                          XOR1. */
#define BCM_FLEXDIGEST_HASH_FUNCTION_CRC16XOR2 3          /* 8-bit CRC16 + 8-bit
                                                          XOR2. */
#define BCM_FLEXDIGEST_HASH_FUNCTION_CRC16XOR4 4          /* 8-bit CRC16 + 8-bit
                                                          XOR4. */
#define BCM_FLEXDIGEST_HASH_FUNCTION_CRC16XOR8 5          /* 8-bit CRC16 + 8-bit
                                                          XOR8. */
#define BCM_FLEXDIGEST_HASH_FUNCTION_XOR16  6          /* 16-bit XOR. */
#define BCM_FLEXDIGEST_HASH_FUNCTION_CRC16CCITT 7          /* 16-bit CRC using CCITT
                                                          polynomial. */
#define BCM_FLEXDIGEST_HASH_FUNCTION_CRC32LO 8          /* Lower 16-bit of CRC32. */
#define BCM_FLEXDIGEST_HASH_FUNCTION_CRC32HI 9          /* Higher 16-bit of
                                                          CRC32. */
#define BCM_FLEXDIGEST_HASH_FUNCTION_CRC32_ETH_LO 10         /* Lower 16-bit of
                                                          Ethernet CRC32. */
#define BCM_FLEXDIGEST_HASH_FUNCTION_CRC32_ETH_HI 11         /* Higher 16-bit of
                                                          Ethernet CRC32. */
#define BCM_FLEXDIGEST_HASH_FUNCTION_KOOPMAN_LO 12         /* Lower 16-bit of
                                                          Koopman CRC32. */
#define BCM_FLEXDIGEST_HASH_FUNCTION_KOOPMAN_HI 13         /* Higher 16-bit of
                                                          Koopman CRC32. */
#define BCM_FLEXDIGEST_HASH_FUNCTION_VERSATILE_HASH_0 14         /* Bits[15:0] of
                                                          versatile hash. */
#define BCM_FLEXDIGEST_HASH_FUNCTION_VERSATILE_HASH_1 15         /* Bits[31:16] of
                                                          versatile hash. */

/* The control type for flex digest Hash profile. */
typedef enum bcm_flexdigest_hash_profile_control_e {
    bcmFlexDigestHashProfileControlPreProcessBinA = 0, /* Enable hash pre-processing for bin
                                           set A. */
    bcmFlexDigestHashProfileControlPreProcessBinB = 1, /* Enable hash pre-processing for bin
                                           set B. */
    bcmFlexDigestHashProfileControlPreProcessBinC = 2, /* Enable hash pre-processing for bin
                                           set C. */
    bcmFlexDigestHashProfileControlXorSaltBinA = 3, /* Enable hash salt for bin set A. */
    bcmFlexDigestHashProfileControlXorSaltBinB = 4, /* Enable hash salt for bin set B. */
    bcmFlexDigestHashProfileControlXorSaltBinC = 5, /* Enable hash salt for bin set C. */
    bcmFlexDigestHashProfileControlBinA0FunctionSelection = 6, /* The hash function select for A0
                                           field. */
    bcmFlexDigestHashProfileControlBinA1FunctionSelection = 7, /* The hash function select for A1
                                           field. */
    bcmFlexDigestHashProfileControlBinB0FunctionSelection = 8, /* The hash function select for B0
                                           field. */
    bcmFlexDigestHashProfileControlBinB1FunctionSelection = 9, /* The hash function select for B1
                                           field. */
    bcmFlexDigestHashProfileControlBinC0FunctionSelection = 10, /* The hash function select for C0
                                           field. */
    bcmFlexDigestHashProfileControlBinC1FunctionSelection = 11, /* The hash function select for C1
                                           field. */
    bcmFlexDigestHashProfileControlCount = 12 /* Always last. Not a usable value. */
} bcm_flexdigest_hash_profile_control_t;

#define BCM_FLEXDIGEST_HASH_PROFILE_CONTROL_STRINGS \
{ \
    "PreProcessBinA", \
    "PreProcessBinB", \
    "PreProcessBinC", \
    "XorSaltBinA", \
    "XorSaltBinB", \
    "XorSaltBinC", \
    "BinA0FunctionSelection", \
    "BinA1FunctionSelection", \
    "BinB0FunctionSelection", \
    "BinB1FunctionSelection", \
    "BinC0FunctionSelection", \
    "BinC1FunctionSelection"  \
}

#ifndef BCM_HIDE_DISPATCHABLE

/* Create a flex digest hashing profile. */
extern int bcm_flexdigest_hash_profile_create(
    int unit, 
    uint32 options, 
    int *hash_profile_id);

/* Destroy a flex digest hashing profile. */
extern int bcm_flexdigest_hash_profile_destroy(
    int unit, 
    int hash_profile_id);

/* Destroy all flex digest hashing profiles. */
extern int bcm_flexdigest_hash_profile_destroy_all(
    int unit);

/* Configure a flex digest hashing profiles. */
extern int bcm_flexdigest_hash_profile_set(
    int unit, 
    int hash_profile_id, 
    bcm_flexdigest_hash_profile_control_t type, 
    int value);

/* Get a flex digest hashing profiles. */
extern int bcm_flexdigest_hash_profile_get(
    int unit, 
    int hash_profile_id, 
    bcm_flexdigest_hash_profile_control_t type, 
    int *value);

#endif /* BCM_HIDE_DISPATCHABLE */

/* The bin set for flex digest hashing. */
typedef enum bcm_flexdigest_hash_bin_set_e {
    bcmFlexDigestHashBinA = 0,      /* Flex digest hash Bin Set A. */
    bcmFlexDigestHashBinB = 1,      /* Flex digest hash Bin Set B. */
    bcmFlexDigestHashBinC = 2,      /* Flex digest hash Bin Set C. */
    bcmFlexDigestHashBinCount = 3   /* Always last. Not a usable value. */
} bcm_flexdigest_hash_bin_set_t;

#define BCM_FLEXDIGEST_HASH_BIN_SET_STRINGS \
{ \
    "BinA", \
    "BinB", \
    "BinC"  \
}

#ifndef BCM_HIDE_DISPATCHABLE

/* Configure salt for a specific hashing bin set. */
extern int bcm_flexdigest_hash_salt_set(
    int unit, 
    bcm_flexdigest_hash_bin_set_t bin_set, 
    int array_size, 
    uint16 *salt_array);

/* Get salt configuration for a specific hashing bin set. */
extern int bcm_flexdigest_hash_salt_get(
    int unit, 
    bcm_flexdigest_hash_bin_set_t bin_set, 
    int array_size, 
    uint16 *salt_array, 
    int *count);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Flex digest device-wide control types. */
typedef enum bcm_flexdigest_control_e {
    bcmFlexDigestControlHashVersatileRandomSeedBinA = 0, /* Unsigned 32-bit random seed value of
                                           versatile hash for hashing bin set A. */
    bcmFlexDigestControlHashVersatileRandomSeedBinB = 1, /* Unsigned 32-bit random seed value of
                                           versatile hash for hashing bin set B. */
    bcmFlexDigestControlHashVersatileRandomSeedBinC = 2, /* Unsigned 32-bit random seed value of
                                           versatile hash for hashing bin set C. */
    bcmFlexDigestControlCount = 3       /* Always last. Not a usable value. */
} bcm_flexdigest_control_t;

#define BCM_FLEXDIGEST_CONTROL_STRINGS \
{ \
    "HashVersatileRandomSeedBinA", \
    "HashVersatileRandomSeedBinB", \
    "HashVersatileRandomSeedBinC"  \
}

#ifndef BCM_HIDE_DISPATCHABLE

/* Set a device-wide flex digest control value. */
extern int bcm_flexdigest_control_set(
    int unit, 
    bcm_flexdigest_control_t type, 
    int arg);

/* Get a device-wide flex digest control value. */
extern int bcm_flexdigest_control_get(
    int unit, 
    bcm_flexdigest_control_t type, 
    int *arg);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_FLEXDIGEST_H__ */
