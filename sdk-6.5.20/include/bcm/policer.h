/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_POLICER_H__
#define __BCM_POLICER_H__

#include <bcm/types.h>

/* Policer Modes. */
typedef enum bcm_policer_mode_e {
    bcmPolicerModeSrTcm = 0,            /* RFC 2697 */
    bcmPolicerModeCommitted = 1, 
    bcmPolicerModePeak = 2, 
    bcmPolicerModeTrTcm = 3,            /* RFC 2698 */
    bcmPolicerModeTrTcmDs = 4,          /* RFC 4115, TrTcmModified */
    bcmPolicerModeGreen = 5, 
    bcmPolicerModePassThrough = 6, 
    bcmPolicerModeSrTcmModified = 7, 
    bcmPolicerModeCoupledTrTcmDs = 8, 
    bcmPolicerModeCascade = 9, 
    bcmPolicerModeCoupledCascade = 10, 
    bcmPolicerModeSrTcmTsn = 11,        /* IEEE 802.1Qci TSN Modified srTCM */
    bcmPolicerModeCount = 12 
} bcm_policer_mode_t;

/* Modes for creating a group of policers */
typedef enum bcm_policer_group_mode_e {
    bcmPolicerGroupModeSingle = 0,      /* A single policer used for all traffic
                                           types */
    bcmPolicerGroupModeTrafficType = 1, /* A dedicated policer per traffic type
                                           unicast, multicast, broadcast */
    bcmPolicerGroupModeDlfAll = 2,      /* A pair of policers where the base
                                           policer is used for dlf and the other
                                           policer is used for all traffic types */
    bcmPolicerGroupModeDlfIntPri = 3,   /* N+1 policers where the base policer
                                           is used for dlf and next N are used
                                           per Cos */
    bcmPolicerGroupModeTyped = 4,       /* A dedicated policer for unknown
                                           unicast, known unicast, multicast,
                                           broadcast */
    bcmPolicerGroupModeTypedAll = 5,    /* A dedicated policer for unknown
                                           unicast, known unicast, multicast,
                                           broadcast and one for all traffic
                                           (not already policed) */
    bcmPolicerGroupModeTypedIntPri = 6, /* A dedicated policer for unknown
                                           unicast, known unicast, multicast,
                                           broadcast and N internal priority
                                           policers for traffic (not already
                                           policed) */
    bcmPolicerGroupModeSingleWithControl = 7, /* A single policer used for all traffic
                                           types with an additional policer for
                                           control traffic */
    bcmPolicerGroupModeTrafficTypeWithControl = 8, /* A dedicated policer per traffic type
                                           unicast, multicast, broadcast with an
                                           additional policer for control
                                           traffic */
    bcmPolicerGroupModeDlfAllWithControl = 9, /* A pair of policers where the base
                                           policer is used for control, the next
                                           one for dlf and the other policer is
                                           used for all traffic types */
    bcmPolicerGroupModeDlfIntPriWithControl = 10, /* N+2 policers where the base policer
                                           is used for control, the next one for
                                           dlf and next N are used per Cos */
    bcmPolicerGroupModeTypedWithControl = 11, /* A dedicated policer for control,
                                           unknown unicast, known unicast,
                                           multicast, broadcast */
    bcmPolicerGroupModeTypedAllWithControl = 12, /* A dedicated policer for control,
                                           unknown unicast, known unicast,
                                           multicast, broadcast and one for all
                                           traffic (not already policed) */
    bcmPolicerGroupModeTypedIntPriWithControl = 13, /* A dedicated policer for control,
                                           unknown unicast, known unicast,
                                           multicast, broadcast and N internal
                                           priority policers for traffic (not
                                           already policed) */
    bcmPolicerGroupModeDot1P = 14,      /* A Set of 8 policers based on outer
                                           Vlan priority */
    bcmPolicerGroupModeIntPri = 15,     /* A Set of 16 policers based on
                                           internal priority */
    bcmPolicerGroupModeIntPriCng = 16,  /* A Set of 64 policers based on
                                           internal priority and cng */
    bcmPolicerGroupModeSvpType = 17,    /* A Set of 2 policers based on SVP type */
    bcmPolicerGroupModeDscp = 18,       /* A Set of 64 policers based on IP DSCP
                                           bits */
    bcmPolicerGroupModeCascade = 19,    /*  User specified set of policers(max
                                           8), wherein excess bandwidth can
                                           overflow from high priority bucket to
                                           low priority bucket */
    bcmPolicerGroupModeCascadeWithCoupling = 20, /*  User specified set of policers(max
                                           4), wherein excess bandwidth can
                                           overflow from high priority bucket to
                                           low priority bucket and from low
                                           priority green bucket to high
                                           priority yellow bucket */
    bcmPolicerGroupModeInnerDot1P = 21, /* A Set of 8 policers based on inner
                                           Vlan priority */
    bcmPolicerGroupModeEgressSingle = 22, /* A dedicated policer for all egress
                                           traffic */
    bcmPolicerGroupModeEgressIntPri = 23, /* A dedicated policer for egress
                                           traffic for each cos level */
    bcmPolicerGroupModeShortIntPri = 24, /* A Set of 8 policers based on internal
                                           priority */
    bcmPolicerGroupModeIntPriCascade = 25, /* Set of policers(max 8), wherein
                                           excess bandwidth can overflow from
                                           high priority bucket to low priority
                                           bucket, qualified on internal
                                           priority */
    bcmPolicerGroupModeIntPriCascadeWithCoupling = 26, /* Set of policers(max 4), wherein
                                           excess bandwidth can overflow from
                                           high priority bucket to low priority
                                           bucket and from low priority green
                                           bucket to high priority yellow */
    bcmPolicerGroupModePreemptable = 27, /* A set of 2 policers based on whether
                                           the packet is preemptable or not */
    bcmPolicerGroupModeCascadeDroppedPktExclude = 28, /* Set of policers(max 8), wherein
                                           excess bandwidth can overflow from
                                           high priority bucket to low priority
                                           bucket, with dropped packets ignored
                                           for metering */
    bcmPolicerGroupModeCount = 29       /* Last value. Not to be used */
} bcm_policer_group_mode_t;

/* Supported policer stats */
typedef enum bcm_policer_stat_e {
    bcmPolicerStatGreenToGreenPackets = 0, /* Packet count for G-To-G transitions */
    bcmPolicerStatGreenToGreenBytes = 1, /* Byte count for G-To-G transitions */
    bcmPolicerStatGreenToYellowPackets = 2, /* Packet count for G-To-Y transitions */
    bcmPolicerStatGreenToYellowBytes = 3, /* Byte count for G-To-Y transitions */
    bcmPolicerStatGreenToRedPackets = 4, /* Packet count for G-To-R transitions */
    bcmPolicerStatGreenToRedBytes = 5,  /* Byte count for G-To-R transitions */
    bcmPolicerStatGreenToDropPackets = 6, /* Packet count for G-To-D transitions */
    bcmPolicerStatGreenToDropBytes = 7, /* Byte count for G-To-D transitions */
    bcmPolicerStatYellowToGreenPackets = 8, /* Packet count for Y-To-G transitions */
    bcmPolicerStatYellowToGreenBytes = 9, /* Byte count for Y-To-G transitions */
    bcmPolicerStatYellowToYellowPackets = 10, /* Packet count for Y-To-Y transitions */
    bcmPolicerStatYellowToYellowBytes = 11, /* Byte count for Y-To-Y transitions */
    bcmPolicerStatYellowToRedPackets = 12, /* Packet count for Y-To-R transitions */
    bcmPolicerStatYellowToRedBytes = 13, /* Byte count for Y-To-R transitions */
    bcmPolicerStatYellowToDropPackets = 14, /* Packet count for Y-To-D transitions */
    bcmPolicerStatYellowToDropBytes = 15, /* Byte count for Y-To-D transitions */
    bcmPolicerStatRedToGreenPackets = 16, /* Packet count for R-To-G transitions */
    bcmPolicerStatRedToGreenBytes = 17, /* Byte count for R-To-G transitions */
    bcmPolicerStatRedToYellowPackets = 18, /* Packet count for R-To-Y transitions */
    bcmPolicerStatRedToYellowBytes = 19, /* Byte count for R-To-Y transitions */
    bcmPolicerStatRedToRedPackets = 20, /* Packet count for R-To-R transitions */
    bcmPolicerStatRedToRedBytes = 21,   /* Byte count for R-To-R transitions */
    bcmPolicerStatRedToDropPackets = 22, /* Packet count for R-To-D transitions */
    bcmPolicerStatRedToDropBytes = 23,  /* Byte count for R-To-D transitions */
    bcmPolicerStatPackets = 24,         /* Packet count for all types of traffic
                                           policed */
    bcmPolicerStatBytes = 25,           /* Byte count for all types of traffic
                                           policed */
    bcmPolicerStatUnknownUnicastPackets = 26, /* Packet count for policed unknown
                                           unicast traffic */
    bcmPolicerStatUnknownUnicastBytes = 27, /* Byte count for policed unknown
                                           unicast traffic */
    bcmPolicerStatUnicastPackets = 28,  /* Packet count for policed unicast
                                           traffic */
    bcmPolicerStatUnicastBytes = 29,    /* Byte count for policed unicast
                                           traffic */
    bcmPolicerStatMulticastPackets = 30, /* Packet count for policed multicast
                                           traffic */
    bcmPolicerStatMulticastBytes = 31,  /* Byte count for policed multicast
                                           traffic */
    bcmPolicerStatBroadcastPackets = 32, /* Packet count for policed broadcast
                                           traffic */
    bcmPolicerStatBroadcastBytes = 33,  /* Byte count for policed broadcast
                                           traffic */
    bcmPolicerStatDropPackets = 34,     /* Packet count for all types of traffic
                                           dropped */
    bcmPolicerStatDropBytes = 35,       /* Byte count for all types of traffic
                                           dropped */
    bcmPolicerStatDropUnknownUnicastPackets = 36, /* Packet count for dropped unknown
                                           unicast traffic */
    bcmPolicerStatDropUnknownUnicastBytes = 37, /* Byte count for dropped unknown
                                           unicast traffic */
    bcmPolicerStatDropUnicastPackets = 38, /* Packet count for dropped unicast
                                           traffic */
    bcmPolicerStatDropUnicastBytes = 39, /* Byte count for dropped unicast
                                           traffic */
    bcmPolicerStatDropMulticastPackets = 40, /* Packet count for dropped multicast
                                           traffic */
    bcmPolicerStatDropMulticastBytes = 41, /* Byte count for dropped multicast
                                           traffic */
    bcmPolicerStatDropBroadcastPackets = 42, /* Packet count for dropped broadcast
                                           traffic */
    bcmPolicerStatDropBroadcastBytes = 43, /* Byte count for dropped broadcast
                                           traffic */
    bcmPolicerStatGreenPackets = 44,    /* Packet count for traffic marked green */
    bcmPolicerStatGreenBytes = 45,      /* Byte count for traffic marked green */
    bcmPolicerStatYellowPackets = 46,   /* Packet count for traffic marked
                                           yellow */
    bcmPolicerStatYellowBytes = 47,     /* Byte count for traffic marked yellow */
    bcmPolicerStatRedPackets = 48,      /* Packet count for traffic marked red */
    bcmPolicerStatRedBytes = 49,        /* Byte count for traffic marked red */
    bcmPolicerStatGlobalInRedColorPackets = 50, /* Global - packet count incoming
                                           traffic marked as red */
    bcmPolicerStatGlobalInRedColorBytes = 51, /* Global - byte count incoming traffic
                                           marked as red */
    bcmPolicerStatGlobalInBlackColorPackets = 52, /* Global - packet count incoming
                                           traffic marked as black */
    bcmPolicerStatGlobalInBlackColorBytes = 53, /* Global - byte count incoming traffic
                                           marked as black */
    bcmPolicerStatGlobalRejectPackets = 54, /* Global - packet count rejected
                                           traffic */
    bcmPolicerStatGlobalRejectBytes = 55, /* Global - byte count rejected traffic */
    bcmPolicerStatCount = 56            /* Always last */
} bcm_policer_stat_t;

#define BCM_POLICER_WITH_ID                 0x00000001 
#define BCM_POLICER_REPLACE                 0x00000002 
#define BCM_POLICER_COLOR_BLIND             0x00000004 
#define BCM_POLICER_DROP_RED                0x00000008 
#define BCM_POLICER_COLOR_MERGE_MASK        0x00000030 
#define BCM_POLICER_COLOR_MERGE_AND         0x00000000 
#define BCM_POLICER_COLOR_MERGE_OR          0x00000010 
#define BCM_POLICER_COLOR_MERGE_DUAL        0x00000020 
#define BCM_POLICER_MODE_BYTES              0x00000040 
#define BCM_POLICER_MODE_PACKETS            0x00000080 
#define BCM_POLICER_MAX_CKBITS_SEC_VALID    0x00000100 
#define BCM_POLICER_MAX_PKBITS_SEC_VALID    0x00000200 
#define BCM_POLICER_MACRO                   0x00000400 
#define BCM_POLICER_EGRESS                  0x00000800 /* Allocate policer for
                                                          egress usage */
#define BCM_POLICER_REGEX                   0x00001000 /* Allocate policer for
                                                          regex usage */
#define BCM_POLICER_REPLACE_SHARED          0x00002000 /* Replace the profile
                                                          rather than the
                                                          configuration */
#define BCM_POLICER_MIXED_MICRO_MACRO       0x00004000 /* Micro policer not
                                                          under the macro
                                                          envelope */
#define BCM_POLICER_BW_SHARING_GROUP_START  0x00008000 /* Indicates that this
                                                          policer corresponds to
                                                          the start of the
                                                          bandwidth sharing
                                                          group(cascade chain) */
#define BCM_POLICER_BW_SHARING_GROUP_END    0x00010000 /* Indicates that this
                                                          policer corresponds to
                                                          the end of the
                                                          bandwidth sharing
                                                          group(cascade chain) */
#define BCM_POLICER_WITH_POOL_ID            0x00020000 /* Indicates that this
                                                          Policer needs to be
                                                          created with the given
                                                          meter pool id */
#define BCM_POLICER_AS_PACKET_COUNTER       0x00040000 /* Indicates that this
                                                          Policer needs to be
                                                          created as packet
                                                          counter */
#define BCM_POLICER_PKT_ADJ_HEADER_TRUNCATE 0x00080000 /*  Indicates that this
                                                          Policer needs to be
                                                          considerate of header
                                                          truncate adjustment */
#define BCM_POLICER_APPLY_GLOBAL_POOL_ID    0x00100000 /*  Indicates that this
                                                          Policer needs to be
                                                          allocated from the
                                                          given global meter
                                                          pool id. */
#define BCM_POLICER_LARGE_BUCKET_MODE       0x00200000 /*  if set, force policer
                                                          to use large bucket
                                                          mode */

#define BCM_POLICER_GLOBAL_METER_SHARING_MODE_MIN_MAX 0x00000000 
#define BCM_POLICER_GLOBAL_METER_SHARING_MODE_MIN 0x00000001 
#define BCM_POLICER_GLOBAL_METER_SHARING_MODE_MAX 0x00000002 

#define BCM_POLICER_GLOBAL_METER_ENVELOP_MICRO_FLOW 0x1        
#define BCM_POLICER_GLOBAL_METER_ENVELOP_MACRO_FLOW 0x2        

#define BCM_POLICER_EXPANSION_GROUPS_TC     0x00000001 /* Set the mapping of TC
                                                          groups for expansion
                                                          per TC */
#define BCM_POLICER_EXPANSION_GROUPS_FWD    0x00000002 /* Set the mapping of
                                                          L2-Fwd Type groups for
                                                          expansion per L2-Fwd
                                                          Type */

typedef struct bcm_policer_config_s {
    uint32 flags;                       /* BCM_POLICER_* Flags. */
    bcm_policer_mode_t mode;            /* Policer mode. */
    uint32 ckbits_sec;                  /* Committed rate (kbits per sec). */
    uint32 cbits_sec_lower;             /* Committed rate (lower digits; 0~999
                                           bits per sec). */
    uint32 max_ckbits_sec;              /* Maximum Committed rate (kbits per
                                           sec), Applicable only for cascade
                                           mode. */
    uint32 max_cbits_sec_lower;         /* Maximum Committed rate (lower digits;
                                           0~999 bits per sec). */
    uint32 ckbits_burst;                /* Committed burst size (kbits). */
    uint32 cbits_burst_lower;           /* Committed burst size (lower digits;
                                           0~999 bits). */
    uint32 pkbits_sec;                  /* Peak rate (kbits per sec). */
    uint32 pbits_sec_lower;             /* Peak rate (lower digits; 0~999 bits
                                           per sec). */
    uint32 max_pkbits_sec;              /* Maximum Peak rate (kbits per sec),
                                           Applicable only for cascade mode. */
    uint32 max_pbits_sec_lower;         /* Maximum Peak rate (lower digits;
                                           0~999 bits per sec). */
    uint32 pkbits_burst;                /* Peak burst size (kbits). */
    uint32 pbits_burst_lower;           /* Peak burst size (lower digits; 0~999
                                           bits). */
    uint32 kbits_current;               /* Current size (kbits). */
    uint32 bits_current_lower;          /* Current size (lower digits; 0~999
                                           bits). */
    uint32 action_id;                   /* Index of the global meter action
                                           entry */
    uint32 sharing_mode;                /* Meter sharing mode in case of Macro
                                           meter of Envelop */
    uint32 entropy_id;                  /* If set, indicate the configuration
                                           group the policer belongs to */
    bcm_policer_pool_t pool_id;         /* Meter Pool Id of the current policer */
    uint32 bucket_width;                /* Bucket width in bits of the current
                                           policer */
    uint32 token_gran;                  /* Token granularity of the current
                                           policer */
    uint32 bucket_init_val;             /* Initial bucket count of the current
                                           policer */
    uint32 bucket_rollover_val;         /* Rollover bucket count of the current
                                           policer */
    bcm_core_t core_id;                 /* Device core on which to allocate the
                                           policer */
    bcm_policer_t ncoflow_policer_id;   /* Next lower priority committed policer
                                           ID to overflow to. Applicable in
                                           Cascaded  policer modes on MEF 10.3+
                                           compliant devices */
    bcm_policer_t npoflow_policer_id;   /* Next lower priority peak policer ID
                                           to overflow to. Applicable in
                                           Cascaded and CoupledCascade policer
                                           modes on MEF 10.3+  compliant devices */
    uint32 actual_ckbits_sec;           /* Granularity adjusted Committed rate
                                           (kbits per sec). */
    uint32 actual_cbits_sec_lower;      /* Granularity adjusted Committed rate
                                           (lower digits; 0~999 bits per sec). */
    uint32 actual_ckbits_burst;         /* Granularity adjusted Committed burst
                                           size (kbits). */
    uint32 actual_cbits_burst_lower;    /* Granularity adjusted Committed burst
                                           size (lower digits; 0~999 bits). */
    uint32 actual_pkbits_sec;           /* Granularity adjusted Peak rate (kbits
                                           per sec). */
    uint32 actual_pbits_sec_lower;      /* Granularity adjusted Peak rate (lower
                                           digits; 0~999 bits per sec). */
    uint32 actual_pkbits_burst;         /* Granularity adjusted Peak burst size
                                           (kbits). */
    uint32 actual_pbits_burst_lower;    /* Granularity adjusted Peak burst size
                                           (lower digits; 0~999 bits). */
    uint32 average_pkt_size;            /* Estimated average Ethernet packet
                                           size (1 to 16383 Bytes) for more
                                           accurate metering. */
    int mark_drop_as_black;             /* For drop decision (no committed or
                                           Excess credits), mark it as black
                                           (otherwise, it will be marked as
                                           red). */
    int color_resolve_profile;          /* Input data for the resolved table. */
} bcm_policer_config_t;

typedef struct bcm_policer_aggregate_group_info_s {
    bcm_policer_group_mode_t mode;  /* Policer mode. */
    bcm_core_t core;                /* Device core on which to allocate the
                                       policer */
    uint32 flags;                   /* BCM_POLICER_* Flags. */
} bcm_policer_aggregate_group_info_t;

typedef struct bcm_policer_map_s {
    int count;          /* Number of offset mappings */
    uint8 *offset_map;  /* policer offset for each index */
} bcm_policer_map_t;

/* Initialize a policer map structure. */
extern void bcm_policer_map_t_init(
    bcm_policer_map_t *offset_map);

/* Initialize a policer configuration structure. */
extern void bcm_policer_config_t_init(
    bcm_policer_config_t *pol_cfg);

#ifndef BCM_HIDE_DISPATCHABLE

/* Initialize the policer module. */
extern int bcm_policer_init(
    int unit);

/* Create a policer entry. */
extern int bcm_policer_create(
    int unit, 
    bcm_policer_config_t *pol_cfg, 
    bcm_policer_t *policer_id);

/* Destroy a policer entry. */
extern int bcm_policer_destroy(
    int unit, 
    bcm_policer_t policer_id);

/* Destroy all policer entries. */
extern int bcm_policer_destroy_all(
    int unit);

/* Set the config settings for a policer entry. */
extern int bcm_policer_set(
    int unit, 
    bcm_policer_t policer_id, 
    bcm_policer_config_t *pol_cfg);

/* Get the config settings for a policer entry. */
extern int bcm_policer_get(
    int unit, 
    bcm_policer_t policer_id, 
    bcm_policer_config_t *pol_cfg);

/* Get the current bucket count for a policer entry. */
extern int bcm_policer_packet_counter_get32(
    int unit, 
    bcm_policer_t policer_id, 
    uint32 *count);

/* Reset the bucket count to initial value for a policer entry. */
extern int bcm_policer_packet_reset_counter_get32(
    int unit, 
    bcm_policer_t policer_id, 
    uint32 *count);

#endif /* BCM_HIDE_DISPATCHABLE */

typedef int (*bcm_policer_traverse_cb)(
    int unit, 
    bcm_policer_t policer_id, 
    bcm_policer_config_t *info, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Traverse the existing policers and call the user-supplied callback
 * function for each one.
 */
extern int bcm_policer_traverse(
    int unit, 
    bcm_policer_traverse_cb cb, 
    void *user_data);

/* 
 * Allocate a block of policer entries. bcm_policer_set must be
 * called to setup the individual policers.
 */
extern int bcm_policer_group_create(
    int unit, 
    bcm_policer_group_mode_t mode, 
    bcm_policer_t *policer_id, 
    int *npolicers);

/* 
 * Allocate a block of policer entries. bcm_policer_set must be
 * called to setup the individual policers.
 */
extern int bcm_policer_aggregate_group_create(
    int unit, 
    bcm_policer_aggregate_group_info_t *info, 
    bcm_policer_t *policer_id, 
    int *npolicers);

/* 
 * Allocate a block of policer entries with flexible mapping.
 * bcm_policer_set must be called to setup the individual policers.
 */
extern int bcm_policer_group_create_with_map(
    int unit, 
    bcm_policer_group_mode_t mode, 
    bcm_policer_map_t *offset_map, 
    bcm_policer_t *policer_id, 
    int *num_policers);

/* Allocate a micro or a macro meter policer. */
extern int bcm_policer_envelop_create(
    int unit, 
    uint32 flag, 
    bcm_policer_t macro_flow_policer_id, 
    bcm_policer_t *policer_id);

/* 
 * Allocate a group of micro or a macro meter policer. \ref
 * bcm_policer_set must be
 * called to setup the individual policers.
 */
extern int bcm_policer_envelop_group_create(
    int unit, 
    uint32 flag, 
    bcm_policer_group_mode_t mode, 
    bcm_policer_t macro_flow_policer_id, 
    bcm_policer_t *policer_id, 
    int *npolicers);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Policer group mode types */
typedef enum bcm_policer_group_mode_type_e {
    bcmPolicerGroupModeTypeNormal = 0,  /* A group of n policers (max 256) which
                                           do not share bandwidth amongst each
                                           other */
    bcmPolicerGroupModeTypeCascade = 1, /* User specified set of n policers (max
                                           8), wherein excess bandwidth can
                                           overflow from high priority policers
                                           (i.e policer with higher index)
                                           bucket to low priority policers
                                           bucket(policer with next higher index
                                           in the group) */
    bcmPolicerGroupModeTypeCascadeWithCoupling = 2, /* User specified set of policers (max
                                           4),  wherein excess bandwidth can
                                           overflow from high priority (i.e
                                           policer with higher index) policers
                                           bucket to low priority bucket and
                                           from low priority green bucket to
                                           high priority yellow bucket */
    bcmPolicerGroupModeTypeCount = 3    /* Always last, Not to be used */
} bcm_policer_group_mode_type_t;

/* Vlan related policer attributes values */
typedef enum bcm_policer_group_mode_attr_vlan_e {
    bcmPolicerGroupModeAttrVlanUnTagged = 1, /* UnTagged Vlan */
    bcmPolicerGroupModeAttrVlanInnerTag = 2, /* Inner Tagged Vlan */
    bcmPolicerGroupModeAttrVlanOuterTag = 3, /* Outer Tagged Vlan */
    bcmPolicerGroupModeAttrVlanStackedTag = 4, /* Both Inner and Outer Tagged Vlan */
    bcmPolicerGroupModeAttrVlanAll = 5  /* UnTagged,Inner and Outer Tagged Vlan */
} bcm_policer_group_mode_attr_vlan_t;

/* Packet type related policer attributes values */
typedef enum bcm_policer_group_mode_attr_pkt_type_e {
    bcmPolicerGroupModeAttrPktTypeAll = 1, /* All Packet Types */
    bcmPolicerGroupModeAttrPktTypeUnknown = 2, /* Unknown Packet */
    bcmPolicerGroupModeAttrPktTypeControl = 3, /* Control Packet */
    bcmPolicerGroupModeAttrPktTypeOAM = 4, /* OAM Packet */
    bcmPolicerGroupModeAttrPktTypeBFD = 5, /* BFD Packet */
    bcmPolicerGroupModeAttrPktTypeBPDU = 6, /* BPDU Packet */
    bcmPolicerGroupModeAttrPktTypeICNM = 7, /* ICNM Packet */
    bcmPolicerGroupModeAttrPktType1588 = 8, /* 1588 Packet */
    bcmPolicerGroupModeAttrPktTypeKnownL2UC = 9, /* Known L2 Unicast Packet */
    bcmPolicerGroupModeAttrPktTypeUnknownL2UC = 10, /* Unknown L2 Unicast Packet */
    bcmPolicerGroupModeAttrPktTypeL2BC = 11, /* L2 Broadcast Packet */
    bcmPolicerGroupModeAttrPktTypeKnownL2MC = 12, /* Known L2 Multicast Packet */
    bcmPolicerGroupModeAttrPktTypeUnknownL2MC = 13, /* Unknown L2 Multicast Packet */
    bcmPolicerGroupModeAttrPktTypeKnownL3UC = 14, /* Known L3 Unicast Packet */
    bcmPolicerGroupModeAttrPktTypeUnknownL3UC = 15, /* Unknown L3 Unicast Packet */
    bcmPolicerGroupModeAttrPktTypeKnownIPMC = 16, /* Known IPMC Packet */
    bcmPolicerGroupModeAttrPktTypeUnknownIPMC = 17, /* Unknown IPMC Packet */
    bcmPolicerGroupModeAttrPktTypeKnownMplsL2 = 18, /* Known MPLS L2 Packet */
    bcmPolicerGroupModeAttrPktTypeKnownMplsL3 = 19, /* Known MPLS L3 Packet */
    bcmPolicerGroupModeAttrPktTypeKnownMpls = 20, /* Known MPLS Packet */
    bcmPolicerGroupModeAttrPktTypeUnknownMpls = 21, /* Unknown MPLS Packet */
    bcmPolicerGroupModeAttrPktTypeKnownMplsMulticast = 22, /* Known MPLS Multicast Packet */
    bcmPolicerGroupModeAttrPktTypeKnownMim = 23, /* Known MiM Packet */
    bcmPolicerGroupModeAttrPktTypeUnknownMim = 24, /* Unknown MiM Packet */
    bcmPolicerGroupModeAttrPktTypeKnownTrill = 25, /* Known TRILL Packet */
    bcmPolicerGroupModeAttrPktTypeUnknownTrill = 26, /* Unknown TRILL Packet */
    bcmPolicerGroupModeAttrPktTypeKnownNiv = 27, /* Known  NIV Packet */
    bcmPolicerGroupModeAttrPktTypeUnknownNiv = 28 /* Unknown  NIV Packet */
} bcm_policer_group_mode_attr_pkt_type_t;

/* Policer Group Mode Attribute Selectors */
typedef enum bcm_policer_group_mode_attr_e {
    bcmPolicerGroupModeAttrUdf = 1,     /* UDF selector: Possible values:0 to
                                           2^N-1. Value of N may vary depending
                                           on device. */
    bcmPolicerGroupModeAttrFieldIngressColor = 2, /* Field Ingress Color Selector:
                                           Possible
                                           Values:bcmColorGreen|Yellow|REd or
                                           POLICER_GROUP_MODE_ATTR_ALL_VALUES
                                           for all */
    bcmPolicerGroupModeAttrIntPri = 3,  /* Internal Priority Selector: Possible
                                           Values: 0 to 15 or
                                           POLICER_GROUP_MODE_ATTR_ALL_VALUES
                                           for all */
    bcmPolicerGroupModeAttrVlan = 4,    /* Vlan Type Selector: Possible Values:
                                           bcmPolicerGroupModeVlanAttr */
    bcmPolicerGroupModeAttrOuterPri = 5, /* Outer Vlan Priority Selector:
                                           Possible Values: 0 to 7 or
                                           POLICER_GROUP_MODE_ATTR_ALL_VALUES
                                           for all */
    bcmPolicerGroupModeAttrInnerPri = 6, /* Inner Vlan Priority Selector:
                                           Possible Values: 0 to 7 or
                                           POLICER_GROUP_MODE_ATTR_ALL_VALUES
                                           for all */
    bcmPolicerGroupModeAttrPort = 7,    /* Logical Port Selector: Possible
                                           Values:<MaxLogicalPort> or
                                           POLICER_GROUP_MODE_ATTR_ALL_VALUES
                                           for all */
    bcmPolicerGroupModeAttrTosDscp = 8, /* Type Of Service Selector(DSCP :
                                           Differentiated services Code Point):
                                           Possible Values:<6b:TOS Val> or
                                           POLICER_GROUP_MODE_ATTR_ALL_VALUES
                                           for all */
    bcmPolicerGroupModeAttrTosEcn = 9,  /* Type Of Service Selector(ECN:
                                           Explicit Congestion Notification):
                                           Possible Values:<2b:TOS Val> or
                                           POLICER_GROUP_MODE_ATTR_ALL_VALUES
                                           for all */
    bcmPolicerGroupModeAttrPktType = 10, /* Packet Type Selector: Possible
                                           Values:<bcmPolicerGroupModeAttrPktType*> */
    bcmPolicerGroupModeAttrIngNetworkGroup = 11, /* Ingress Network Group Selector:
                                           Possible Values:<Value> or
                                           POLICER_GROUP_MODE_ATTR_ALL_VALUES
                                           for all */
    bcmPolicerGroupModeAttrDrop = 12,   /* Drop Selector: Possible Values:<0 or
                                           1> */
    bcmPolicerGroupModeAttrPacketTypeIp = 13, /* Ip Packet Selector: Possible
                                           Values:<0 or 1> */
    bcmPolicerGroupModeAttrPreemptable = 14, /* Preemptable Packet Selector: Possible
                                           Values:<0 or 1> */
    bcmPolicerGroupModeAttrEtherType = 15, /* Ether Type: Possible
                                           values:0x0-0xFFFF */
    bcmPolicerGroupModeAttrOuterVlanCfi = 16, /* Outer VLAN CFI: Possible Values:<0 or
                                           1> */
    bcmPolicerGroupModeAttrInnerVlanCfi = 17, /* Inner VLAN CFI: Possible Values:<0 or
                                           1> */
    bcmPolicerGroupModeAttrLast = 18,   /* Last value. Not to be used */
    bcmPolicerGroupModeAttrCount = bcmPolicerGroupModeAttrLast /* Last value. Not to be used */
} bcm_policer_group_mode_attr_t;

/* Unique constants */
#define BCM_POLICER_GROUP_MODE_ATTR_ALL_VALUES 0xFFFFFFFF /* A unique constant for
                                                          using all attr values */

/* packet attribute selector flags values */
#define BCM_POLICER_ATTR_WIDTH_OFFSET   0x1        /* Offset and width fields
                                                      are valid for this
                                                      attribute */

/* packet attribute selector flags values */
#define BCM_POLICER_ATTR_VALUE_MASK 0x2        /* Mask is valid for this
                                                  attribute. Flags WIDTH_OFFSET
                                                  and MASK are mutually
                                                  exclusive */

/* packet attribute selector flags values */
#define BCM_POLICER_ATTR_MULTI  0x4        /* Matching multiple attributes in
                                              single selector. The
                                              policer_offset is chosen only if
                                              all attributes match */

/* Policer Group Attribute Selector */
typedef struct bcm_policer_group_mode_attr_selector_s {
    uint32 flags;                       /* packet attribute selector flags */
    uint32 policer_offset;              /* Policer Offset */
    bcm_policer_group_mode_attr_t attr; /* Attribute Selector */
    uint32 attr_value;                  /* Attribute Values */
    uint32 attr_value_mask;             /* Attribute Mask */
    int udf_id;                         /* UDF ID created using bcm_udf_create.
                                           Applicable only if Group mode
                                           attribute is
                                           bcmPolicerGroupModeAttrUdf. */
    uint32 offset;                      /* Offset in bits from the beginning of
                                           the packet attribute */
    uint32 width;                       /* Number of bits in packet attribute
                                           from offset to be used for policer
                                           offset mapping */
    uint32 multi_attr_count;            /* Number of multiple elements specified
                                           in all of the multi_attr_xxx[] array. */
    uint32 multi_attr_flags[bcmPolicerGroupModeAttrCount]; /* Attribute flags */
    bcm_policer_group_mode_attr_t multi_attr[bcmPolicerGroupModeAttrCount]; /* Attribute Selector */
    uint32 multi_attr_value[bcmPolicerGroupModeAttrCount]; /* Attribute Values */
    uint32 multi_attr_value_mask[bcmPolicerGroupModeAttrCount]; /* Attribute Mask */
    int multi_udf_id[bcmPolicerGroupModeAttrCount]; /* UDF ID created using bcm_udf_create.
                                           Applicable only if attribute is
                                           bcmPolicerGroupModeAttrUdf. */
    uint32 multi_offset[bcmPolicerGroupModeAttrCount]; /* Offset in bits from the beginning of
                                           the packet attribute */
    uint32 multi_width[bcmPolicerGroupModeAttrCount]; /* Number of bits in packet attribute
                                           from offset to be used */
} bcm_policer_group_mode_attr_selector_t;

/* Initialize a policer group attribute selector structure. */
extern void bcm_policer_group_mode_attr_selector_t_init(
    bcm_policer_group_mode_attr_selector_t *pol_group_mode_attr_selector);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create Customized Policer Group mode for given Policer Attributes */
extern int bcm_policer_group_mode_id_create(
    int unit, 
    uint32 flags, 
    bcm_policer_group_mode_type_t type, 
    uint32 total_policers, 
    uint32 num_selectors, 
    bcm_policer_group_mode_attr_selector_t *attr_selectors, 
    uint32 *mode_id);

/* Destroys Customized Group mode */
extern int bcm_policer_group_mode_id_destroy(
    int unit, 
    uint32 mode_id);

/* Retrieves Customized Policer Group mode Attributes for given mode_id */
extern int bcm_policer_group_mode_id_get(
    int unit, 
    uint32 mode_id, 
    uint32 num_selectors, 
    uint32 *flags, 
    bcm_policer_group_mode_type_t *type, 
    uint32 *total_policers, 
    bcm_policer_group_mode_attr_selector_t *attr_selectors, 
    uint32 *actual_num_selectors);

/* Create a group of policers belonging to group mode specified */
extern int bcm_policer_custom_group_create(
    int unit, 
    uint32 flags, 
    uint32 mode_id, 
    bcm_policer_t macro_flow_policer_id, 
    bcm_policer_t *policer_id, 
    uint32 *npolicers);

/* 
 * Get the list of policer members for a given policer group represented
 * by its base policer id.
 */
extern int bcm_policer_group_get(
    int unit, 
    bcm_policer_t base_policer_id, 
    int member_max, 
    bcm_policer_t *member_array, 
    int *member_count);

/* 
 * Checks if statistics collection is enabled on the specified policer
 * ID.
 */
extern int bcm_policer_stat_enable_get(
    int unit, 
    bcm_policer_t policer_id, 
    int *enable);

/* Enables statistics collection on the specified policer ID. */
extern int bcm_policer_stat_enable_set(
    int unit, 
    bcm_policer_t policer_id, 
    int enable);

/* Gets the specified stat. */
extern int bcm_policer_stat_get(
    int unit, 
    bcm_policer_t policer_id, 
    bcm_cos_t int_pri, 
    bcm_policer_stat_t stat, 
    uint64 *value);

/* 32-bit version of stat get API */
extern int bcm_policer_stat_get32(
    int unit, 
    bcm_policer_t policer_id, 
    bcm_cos_t int_pri, 
    bcm_policer_stat_t stat, 
    uint32 *value);

/* Sets the specified stat. */
extern int bcm_policer_stat_set(
    int unit, 
    bcm_policer_t policer_id, 
    bcm_cos_t int_pri, 
    bcm_policer_stat_t stat, 
    uint64 value);

/* 32-bit version of stat set API */
extern int bcm_policer_stat_set32(
    int unit, 
    bcm_policer_t policer_id, 
    bcm_cos_t int_pri, 
    bcm_policer_stat_t stat, 
    uint32 value);

/* Associate action with the policer. */
extern int bcm_policer_action_attach(
    int unit, 
    bcm_policer_t policer_id, 
    uint32 action_id);

/* Disassociate action from the policer. */
extern int bcm_policer_action_detach(
    int unit, 
    bcm_policer_t policer_id, 
    uint32 action_id);

/* Get the action ID associated with the policer ID. */
extern int bcm_policer_action_attach_get(
    int unit, 
    bcm_policer_t policer_id, 
    uint32 *action_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Global meter policer actions */
typedef enum bcm_policer_action_e {
    bcmPolicerActionGpDrop = 0,         /* Green priority drop */
    bcmPolicerActionGpDscpNew = 1,      /* Green packet change DSCP value
                                           param0: New DSCP value */
    bcmPolicerActionGpEcnNew = 2,       /* Green packet change ECN value param0:
                                           New ECN value */
    bcmPolicerActionGpPrioIntNew = 3,   /* Green packet internal priority from
                                           an argument. param0: New priority */
    bcmPolicerActionGpCngNew = 4,       /* Green packet change CNG */
    bcmPolicerActionGpVlanPrioNew = 5,  /* Green packet change vlan priority.
                                           param0: New priority */
    bcmPolicerActionYpDrop = 6,         /* Yellow priority drop */
    bcmPolicerActionYpDscpNew = 7,      /* Yellow packet change DSCP value
                                           param0: New DSCP value */
    bcmPolicerActionYpEcnNew = 8,       /* Yellow packet change ECN value
                                           param0: New ECN value */
    bcmPolicerActionYpPrioIntNew = 9,   /* Yellow packet internal priority from
                                           an argument. param0: New priority */
    bcmPolicerActionYpCngNew = 10,      /* Yellow packet change CNG */
    bcmPolicerActionYpVlanPrioNew = 11, /* Yellow packet change vlan priority.
                                           param0: New priority */
    bcmPolicerActionRpDrop = 12,        /* Red priority drop */
    bcmPolicerActionRpDscpNew = 13,     /* Red packet change DSCP value param0:
                                           New DSCP value */
    bcmPolicerActionRpEcnNew = 14,      /* Red packet change ECN value param0:
                                           New ECN value */
    bcmPolicerActionRpPrioIntNew = 15,  /* Red packet internal priority from an
                                           argument. param0: New priority */
    bcmPolicerActionRpCngNew = 16,      /* Red packet change CNG */
    bcmPolicerActionRpVlanPrioNew = 17, /* Red packet change vlan priority.
                                           param0: New priority */
    bcmPolicerActionGpIntCongestionNotificationNew = 18, /* Replace the internal congestion
                                           notification value of green packet.
                                           param0: Internal congestion
                                           notification value. */
    bcmPolicerActionYpIntCongestionNotificationNew = 19, /* Replace the internal congestion
                                           notification value of yellow packet.
                                           param0: Internal congestion
                                           notification value. */
    bcmPolicerActionRpIntCongestionNotificationNew = 20, /* Replace the internal congestion
                                           notification value of red packet.
                                           param0: Internal congestion
                                           notification value. */
    bcmPolicerActionCount = 21          /* Last value. Not to be used */
} bcm_policer_action_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Create a new policer action */
extern int bcm_policer_action_create(
    int unit, 
    uint32 *action_id);

/* Destroy a policer action */
extern int bcm_policer_action_destroy(
    int unit, 
    uint32 action_id);

/* Configure the action for an Action ID . */
extern int bcm_policer_action_add(
    int unit, 
    uint32 action_id, 
    bcm_policer_action_t action, 
    uint32 param0);

/* Get the param value for an action in policer action table. */
extern int bcm_policer_action_get(
    int unit, 
    uint32 action_id, 
    bcm_policer_action_t action, 
    uint32 *param0);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Policer internal color */
typedef enum bcm_policer_color_e {
    bcmPolicerColorGreen = 0,   /* Green color */
    bcmPolicerColorInvalid = 1, /* Invalid color */
    bcmPolicerColorYellow = 2,  /* Yellow color */
    bcmPolicerColorRed = 3      /* Red color */
} bcm_policer_color_t;

/* Policer color decision entry */
typedef struct bcm_policer_color_decision_s {
    uint32 flags;                       /* color decision flags */
    bcm_policer_color_t policer0_color; /* policer-0 color decision */
    uint8 policer0_other_bucket_has_credits; /* if policer0_color is yellow, indicate
                                           if green bucket has credits,
                                           otherwise if yellow bucket has
                                           credits */
    bcm_policer_color_t policer1_color; /* policer-1 color decision */
    uint8 policer1_other_bucket_has_credits; /* if policer1_color is yellow, indicate
                                           if green bucket has credits,
                                           otherwise if yellow bucket has
                                           credits */
    int policer0_mark_drop_as_black;    /* if policer0 decision is drop,
                                           indicate if it is marked black or
                                           red. */
    int policer1_mark_drop_as_black;    /* if policer1 decision is drop,
                                           indicate if it is marked black or
                                           red. */
    bcm_policer_color_t policer0_update_bucket; /* the policer-0 bucket to update */
    bcm_policer_color_t policer1_update_bucket; /* the policer-1 bucket to update */
    bcm_color_t policer_color;          /* policer color decision */
} bcm_policer_color_decision_t;

/* Policer color resolution entry */
typedef struct bcm_policer_color_resolution_s {
    uint32 flags;                       /* color resolution flags */
    uint32 policer_action;              /* policer command */
    bcm_color_t incoming_color;         /* incoming color */
    bcm_color_t ethernet_policer_color; /* Ethernet policer color */
    bcm_color_t policer_color;          /* policer color */
    bcm_color_t ingress_color;          /* ingress color */
    bcm_color_t egress_color;           /* egress color */
} bcm_policer_color_resolution_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set an entry in the Policer Color Decision table */
extern int bcm_policer_color_decision_set(
    int unit, 
    bcm_policer_color_decision_t *policer_color_decision);

/* Get an entry from the Policer Color Decision table */
extern int bcm_policer_color_decision_get(
    int unit, 
    bcm_policer_color_decision_t *policer_color_decision);

/* Set an entry in the Policer Color Resolution table */
extern int bcm_policer_color_resolution_set(
    int unit, 
    bcm_policer_color_resolution_t *policer_color_resolution);

/* Get an entry from the Policer Color Resolution table */
extern int bcm_policer_color_resolution_get(
    int unit, 
    bcm_policer_color_resolution_t *policer_color_resolution);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Sources of global metering. */
typedef enum bcm_policer_global_meter_source_e {
    bcmPolicerGlobalMeterSourceFieldStageLookup = 0, /* VFP - Field Stage Lookup */
    bcmPolicerGlobalMeterSourceDstMac = 1, /* Destination MAC Address Lookup */
    bcmPolicerGlobalMeterSourceSrcMac = 2, /* Source MAC Address Lookup */
    bcmPolicerGlobalMeterSourceVlanTranslateFirst = 3, /* VLAN Translation First Lookup */
    bcmPolicerGlobalMeterSourceVlanTranslateSecond = 4, /* VLAN Translation Second Lookup */
    bcmPolicerGlobalMeterSourceVlan = 5, /* Outer VLAN ID Matched */
    bcmPolicerGlobalMeterSourcePort = 6, /* Ingress Port Matched */
    bcmPolicerGlobalMeterSourceCount = 7 /* Always last */
} bcm_policer_global_meter_source_t;

/* bcm policer global meter configuration structure */
typedef struct bcm_policer_global_meter_config_s {
    uint8 ifg_enable;                   /* Enable Inter Frame Gap(IFG) for
                                           incoming packet length in global
                                           metering. The IFG value is configured
                                           by switch API
                                           bcmSwitchMeterAdjustInterframeGap
                                           (per port). 0 = Disable, 1 = Enable. */
    bcm_policer_global_meter_source_t source_order[bcmPolicerGlobalMeterSourceCount]; /* Specify global metering sources in
                                           the order or precedence. Source at
                                           array index 0 has highest precedence. */
    uint32 source_order_count;          /* Valid index count in source_order
                                           array. */
} bcm_policer_global_meter_config_t;

/* Initialize a policer global meter configuration structure */
extern void bcm_policer_global_meter_config_t_init(
    bcm_policer_global_meter_config_t *config);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set policer global meter configuration. */
extern int bcm_policer_global_meter_config_set(
    int unit, 
    bcm_policer_global_meter_config_t *config);

/* Get policer global meter configuration. */
extern int bcm_policer_global_meter_config_get(
    int unit, 
    bcm_policer_global_meter_config_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Enum to select the scope of the policer. */
typedef enum bcm_policer_scope_e {
    bcmPolicerScopeIngressGlobal = 0, 
    bcmPolicerScopeIngress = 1, 
    bcmPolicerScopeEgress = 2 
} bcm_policer_scope_t;

/* Forwarding types enum. */
typedef enum bcm_policer_fwd_types_e {
    bcmPolicerFwdTypeUc = 0, 
    bcmPolicerFwdTypeUnkownUc = 1, 
    bcmPolicerFwdTypeMc = 2, 
    bcmPolicerFwdTypeUnkownMc = 3, 
    bcmPolicerFwdTypeBc = 4, 
    bcmPolicerFwdTypeMax = 5 
} bcm_policer_fwd_types_t;

/* 
 * Structure hold the for each forwarding type the offset in the
 * expansion group and if to do meter on it or not.
 */
typedef struct bcm_policer_expansion_group_types_config_s {
    int offset; /* Offset in expansion group */
    int valid;  /* If not valid, packet with this type will be filtered. */
} bcm_policer_expansion_group_types_config_t;

/* 
 * Structure holds the policer expansion groups, according the forwarding
 * types.
 */
typedef struct bcm_policer_expansion_group_s {
    bcm_policer_expansion_group_types_config_t config[bcmPolicerFwdTypeMax]; 
    int tc_offset[BCM_COS_COUNT];       /* Per TC offset for expansion groups */
} bcm_policer_expansion_group_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * API Defines groups of L2 Fwd-Types and decide if to count them and in
 * which offset in the expansion to count them.
 */
extern int bcm_policer_expansion_groups_set(
    int unit, 
    int flags, 
    bcm_core_t core_id, 
    bcm_policer_expansion_group_t *expansion_group);

/* API gets the expansion groups of L2 Fwd-Types. */
extern int bcm_policer_expansion_groups_get(
    int unit, 
    int flags, 
    bcm_core_t core_id, 
    bcm_policer_expansion_group_t *expansion_group);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Structure holds configuration per policer database. */
typedef struct bcm_policer_database_config_s {
    int expansion_enable;   /* Allowed to ingress database 0. if enabled,
                               expansion size, filter and offsets will be  made
                               according API bcm_policer_expansion_groups_set. */
    int is_single_bucket;   /* Single or dual bucket mode. */
    int expand_per_tc;      /* If set and expansion is enabled, expansion will
                               be made per TC, else it will be made per L2-Fwd
                               Type */
} bcm_policer_database_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * API create a policer database (sw create). database represent the
 * meter processor belong to one of the meter commands.It gets as input
 * database handler which gather the key parameters: core, scope,
 * database_id
 */
extern int bcm_policer_database_create(
    int unit, 
    int flags, 
    int policer_database_handle, 
    bcm_policer_database_config_t *config);

/* API destroy a policer database */
extern int bcm_policer_database_destroy(
    int unit, 
    int flags, 
    int policer_database_handle);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Enum holds the policer control types. */
typedef enum bcm_policer_control_type_e {
    bcmPolicerControlTypeFilterByDpCommand = 0, /* Filter meter pointers according the
                                           dp command. It is a bitmap. */
    bcmPolicerControlTypeDpResolutionByDpCommand = 1, /* select the input for the resolve
                                           table: policer-profile or dp-command.
                                           This type is per ingress/egress only
                                           and not per database. */
    bcmPolicerControlTypeCountBytes = 2, /* Select if to count bytes or packets
                                           in the internal policer stats.
                                           (default=packets). */
    bcmPolicerFairness = 3,             /*  enable/disable fairness between
                                           multiple flows using the same meter,
                                           by jittering the bucket level. */
    bcmPolicerControlTypeTCSMEnable = 4, /* Enable TC Sensitive metering */
    bcmPolicerControlTypeTCSMUsePPTC = 5 /* Enable using PP TC for determining
                                           the TCSM threshold. If it is not set,
                                           the offset of the expansion group
                                           will be used */
} bcm_policer_control_type_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * API which contain several policer control types that can be
 * configured.
 */
extern int bcm_policer_control_set(
    int unit, 
    int flags, 
    int policer_database_handle, 
    bcm_policer_control_type_t type, 
    uint32 arg);

/* API get the value of one of the control types. */
extern int bcm_policer_control_get(
    int unit, 
    int flags, 
    int policer_database_handle, 
    bcm_policer_control_type_t type, 
    uint32 *arg);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Enum to select the section in the policer-crps shared memory engines. */
typedef enum bcm_policer_engine_section_e {
    bcmPolicerEngineSectionAll = 0, 
    bcmPolicerEngineSectionLow = 1, 
    bcmPolicerEngineSectionHigh = 2 
} bcm_policer_engine_section_t;

/* Structure holds the policer memory engine key configuration. */
typedef struct bcm_policer_engine_s {
    int core_id; 
    int engine_id;                      /* Same units as the crps engines. valid
                                           engines are the ones that are shared
                                           with crps: 0,1,16,17,18,19,20,21. */
    bcm_policer_engine_section_t section; /* When using dual bucket moode, must
                                           use 'All'. */
} bcm_policer_engine_t;

/* 
 * Structure define the database that the engine will attached to and its
 * base stat pointer.
 */
typedef struct bcm_policer_database_attach_config_s {
    int policer_database_handle;    /* Database handle defines the database that
                                       engine belong to. */
    int object_stat_pointer_base;   /* Resolution after expansion. */
} bcm_policer_database_attach_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* API attach one engine (or half engine) to a policer database. */
extern int bcm_policer_engine_database_attach(
    int unit, 
    int flags, 
    bcm_policer_engine_t *engine, 
    bcm_policer_database_attach_config_t *config);

/* API gets per engine (or half engine), the database it attached to. */
extern int bcm_policer_engine_database_get(
    int unit, 
    int flags, 
    bcm_policer_engine_t *engine, 
    bcm_policer_database_attach_config_t *config);

/* 
 * API detach one engine (or half engine) from a policer database. From
 * this point, it can be used to crps or other policer database.
 */
extern int bcm_policer_engine_database_detach(
    int unit, 
    int flags, 
    bcm_policer_engine_t *engine);

/* API enable/disable policer database. */
extern int bcm_policer_database_enable_set(
    int unit, 
    int flags, 
    int policer_database_handle, 
    int enable);

/* API gets if policer database is enabled or disabled. */
extern int bcm_policer_database_enable_get(
    int unit, 
    int flags, 
    int policer_database_handle, 
    int *enable);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_COLOR_RESOLUTION_EGRESS 0x1        /* Refer to egress meter final
                                                  resolve table. (default is
                                                  ingress) */

/* Structure holds the data of each single meter. */
typedef struct bcm_policer_color_data_s {
    bcm_policer_color_t policer_color;  /* Meter color decision. */
    int policer_other_bucket_has_credits; /* If policer0_color is yellow, indicate
                                           if green bucket has credits,otherwise
                                           if yellow bucket has credits. */
} bcm_policer_color_data_t;

/* Structure holds the entire key for the color resolution table. */
typedef struct bcm_policer_primary_color_resolution_key_s {
    bcm_core_t core_id; 
    int is_ingress; 
    int action;                         /* Holds the DP-CMD data or the meter
                                           profile data (depend on
                                           configuration). */
    bcm_policer_color_data_t color_data[3]; /* Color data per meter. For egress,
                                           only 2 are relevant. */
} bcm_policer_primary_color_resolution_key_t;

/* Holds all the outputs of the resolve table. */
typedef struct bcm_policer_primary_color_resolution_config_s {
    int policer_count[3];               /* Is to count the policer. */
    bcm_color_t policer_color[3];       /* The color which will be marked for
                                           the counter (if counted). */
    bcm_policer_color_t policer_update_bucket[3]; /* The policer bucket to update. */
    bcm_color_t resolved_color;         /* The resolved color (or color
                                           decision). */
} bcm_policer_primary_color_resolution_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * API configure one entry of the color resolved table (the first/primary
 * table).
 */
extern int bcm_policer_primary_color_resolution_set(
    int unit, 
    int flags, 
    bcm_policer_primary_color_resolution_key_t *key, 
    bcm_policer_primary_color_resolution_config_t *config);

/* 
 * API get one entry of the color resolved table (the first/primary
 * table).
 */
extern int bcm_policer_primary_color_resolution_get(
    int unit, 
    int flags, 
    bcm_policer_primary_color_resolution_key_t *key, 
    bcm_policer_primary_color_resolution_config_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* init bcm_policer_primary_color_resolution_key_t */
extern void bcm_policer_primary_color_resolution_key_t_init(
    bcm_policer_primary_color_resolution_key_t *policer_primary_color_resolution_key);

/* init bcm_policer_primary_color_resolution_config_t */
extern void bcm_policer_primary_color_resolution_config_t_init(
    bcm_policer_primary_color_resolution_config_t *policer_primary_color_resolution_config);

/* init bcm_policer_engine_t */
extern void bcm_policer_engine_t_init(
    bcm_policer_engine_t *policer_engine);

/* init bcm_policer_database_attach_config_t */
extern void bcm_policer_database_attach_config_t_init(
    bcm_policer_database_attach_config_t *policer_database_attach_config);

/* init bcm_policer_database_config_t */
extern void bcm_policer_database_config_t_init(
    bcm_policer_database_config_t *policer_database_config);

/* init bcm_policer_expansion_group_t */
extern void bcm_policer_expansion_group_t_init(
    bcm_policer_expansion_group_t *policer_expansion_group);

/* init bcm_policer_color_resolution_t */
extern void bcm_policer_color_resolution_t_init(
    bcm_policer_color_resolution_t *policer_color_resolution);

/* Direction of entries allocation in hardware. */
typedef enum bcm_policer_group_direction_e {
    bcmPolicerGroupDirectionVertical = 0, /* Entries allocation within pool */
    bcmPolicerGroupDirectionHorizontal = 1 /* Entries allocation across pool */
} bcm_policer_group_direction_t;

/* Policer Pool numbers (for bcmFieldHintTypePolicerPoolSel). */
typedef enum bcm_policer_pool_num_e {
    bcmPolicerPool0 = 0,    /* Policer pool 0. */
    bcmPolicerPool1 = 1,    /* Policer pool 1. */
    bcmPolicerPool2 = 2,    /* Policer pool 2. */
    bcmPolicerPool3 = 3,    /* Policer pool 3. */
    bcmPolicerPoolCount = 4 /* Always Last. Not a usable value. */
} bcm_policer_pool_num_t;

#define BCM_POLICER_POOL_NUM_STRINGS \
{ \
    "0", \
    "1", \
    "2", \
    "3"  \
}

/* Macros for policer pool overrride */
#define BCM_POLICER_CANCEL_NONE     (0)        /* Do not cancel policer pool
                                                  actions */
#define BCM_POLICER_CANCEL_POOL0    (1 << 0)   /* Cancel policer actions from
                                                  pool 0 */
#define BCM_POLICER_CANCEL_POOL1    (1 << 1)   /* Cancel policer actions from
                                                  pool 1 */
#define BCM_POLICER_CANCEL_POOL2    (1 << 2)   /* Cancel policer actions from
                                                  pool 2 */
#define BCM_POLICER_CANCEL_POOL3    (1 << 3)   /* Cancel policer actions from
                                                  pool 3 */

/* Policer group information */
typedef struct bcm_policer_group_info_s {
    bcm_policer_t policer_id;           /* Policer Identifier. */
    bcm_policer_group_direction_t direction; /* Direction of allocation of entries */
    uint32 mode_id;                     /* Policer group mode identifier. */
    bcm_policer_group_mode_t mode;      /* Policer group mode. */
    uint32 pool_id;                     /* Pool number of allocated entries. If
                                           direction is vertical, it represent
                                           pool in which all entries are
                                           allocated. If direction is
                                           horizontal, it is always 0. */
    uint32 base_offset;                 /* In vertical allocation, it represents
                                           offset of Base policer Index from the
                                           start of pool. In horizontal
                                           direction, it represents offset of
                                           each policers allocated from start of
                                           their respective pool. */
    uint32 num_policers;                /* Number of policers allocated in the
                                           group. */
    uint32 num_offset_reserved;         /* Number of entries reserved in the
                                           hardware. Direction along with this
                                           value provide information of how
                                           entries are allocated */
    bcm_policer_t macro_policer_id;     /* Macro policer of the policer group. */
} bcm_policer_group_info_t;

/* Initializes bcm_policer_group_info_t struct. */
extern void bcm_policer_group_info_t_init(
    bcm_policer_group_info_t *policer_group_info);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Create a group of policers belonging to group mode Id. The output
 * is policer group information required to build database of entries
 * allocations within/across pools.
 */
extern int bcm_policer_custom_group_id_create(
    int unit, 
    uint32 flags, 
    uint32 mode_id, 
    bcm_policer_t macro_flow_policer_id, 
    bcm_policer_group_info_t *policer_group_info);

/* 
 * Copy a policer group from current hardware location to new location as
 * provided.
 */
extern int bcm_policer_custom_group_id_copy(
    int unit, 
    bcm_policer_group_info_t *old_policer_group_info, 
    bcm_policer_group_info_t *new_policer_group_info);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_POLICER_H__ */
