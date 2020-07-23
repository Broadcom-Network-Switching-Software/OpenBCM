/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_FABRIC_H__
#define __BCM_FABRIC_H__

#include <bcm/types.h>
#include <bcm/port.h>
#include <shared/fabric.h>

#define BCM_FABRIC_PORT_EGRESS_MULTICAST    (1U<<0)    
#define BCM_FABRIC_PORT_INGRESS_MULTICAST   (1U<<1)    

#ifndef BCM_HIDE_DISPATCHABLE

/* Create a fabric port on the parent physical port */
extern int bcm_fabric_port_create(
    int unit, 
    bcm_gport_t parent_port, 
    int offset, 
    uint32 flags, 
    bcm_gport_t *port);

/* Destroy a fabric port */
extern int bcm_fabric_port_destroy(
    int unit, 
    bcm_gport_t parent_port);

/* Given a child port retrieve the parent port */
extern int bcm_fabric_port_get(
    int unit, 
    bcm_gport_t child_port, 
    uint32 flags, 
    bcm_gport_t *parent_port);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_fabric_control_t */
typedef enum bcm_fabric_control_e {
    bcmFabricArbiterId = 0,             /* Arbiter Identifier */
    bcmFabricActiveArbiterId = 1,       /* Active Arbiter Identifier */
    bcmFabricArbiterConfig = 2,         /* Arbiter NULL Grant Mechanism */
    bcmFabricMaximumFailedLinks = 3,    /* Maximum Failed Links for load shared
                                           data plane redundancy mode */
    bcmFabricActiveId = 4,              /* Retrieves actual(versus provisioned)
                                           Active Arbiter Identifier */
    bcmFabricRedundancyMode = 5,        /* Fabric Redundancy Mode */
    bcmFabricMode = 6,                  /* Fabric Mode (QE fabric, QE TME) */
    bcmFabricManager = 7,               /* Fabric Manager Mode (XBAR only,
                                           ARB/XBAR, ARB capable/XBAR) */
    bcmFabricMcGroupSourceKnockout = 8, /* Multicast Group Source Knockout
                                           Configuration. For get operation the
                                           arg parameter direction In/Out */
    bcmFabricEgressQueueMin = 9,        /* Local QID start index */
    bcmFabricEgressQueueMax = 10,       /* Local QID end index */
    bcmFabricQueueMin = 11,             /* FIC QID start index */
    bcmFabricQueueMax = 12,             /* FIC QID end index */
    bcmFabricArbitrationMapFabric = 13, /* Arbitration Map for Fabric Queues */
    bcmFabricArbitrationMapSubscriber = 14, /* Arbitration Map for Subscriber Queues */
    bcmFabricArbitrationMapHierarchicalSubscriber = 15, /* Arbitration Map for higher level
                                           schedulers/scheduling nodes */
    bcmFabricSubscriberCosLevels = 16,  /* Maximum COS levels for subscriber
                                           queues */
    bcmFabricSubscriberCosLevelAllocation = 17, /* Internal COS allocation for
                                           subscriber queues */
    bcmFabricShaperQueueMin = 18,       /* FIC QID start shaping index */
    bcmFabricShaperQueueMax = 19,       /* FIC QID end shaping index */
    bcmFabricShaperQueueIncrement = 20, /* FIC QID shaping increment. 1
                                           indicates shaping can be configured
                                           on every queue, 4 indicates shaping
                                           can be configured on every 4th queue,
                                           0 indicates shaping on entire queue
                                           group */
    bcmFabricShaperEgressQueueMin = 21, /* Local QID start shaping index */
    bcmFabricShaperEgressQueueMax = 22, /* Local QID end shaping index */
    bcmFabricShaperEgressQueueIncrement = 23, /* Local QID shaping increment. 1
                                           indicates shaping can be configured
                                           on every queue, 4 indicates shaping
                                           can be configured on every 4th queue,
                                           0 indicates shaping on entire queue
                                           group */
    bcmFabricMulticastQueueMin = 24,    /* Multicast QID start index */
    bcmFabricMulticastQueueMax = 25,    /* Multicast QID end index */
    bcmFabricRecycleQueueMin = 26,      /* Recycle QID start index */
    bcmFabricRecycleQueueMax = 27,      /* Recycle QID end index */
    bcmFabricWatchdogQueueMin = 28,     /* Watchdog QID start index */
    bcmFabricWatchdogQueueMax = 29,     /* Watchdog QID end index */
    bcmFabricWatchdogQueueEnable = 30,  /* Queue Watchdog Functionality
                                           Enable/Disable setting (True/False) */
    bcmFabricTrafficManagementCosMode = 31, /* traffic management cos mode. Only
                                           retreiving this configuration is
                                           supported */
    bcmFabricCellSizeFixed = 32,        /* fixed cell size configuration. TRUE
                                           => fixed cell size, FALSE => variable
                                           cell size */
    bcmFabricCellSizeMin = 33,          /* minimum cell size configuration.
                                           Configuration units are in bytes */
    bcmFabricCellSizeMax = 34,          /* maximum cell size configuration.
                                           Configuration units are in bytes */
    bcmFabricCreditSize = 35,           /* credit worth size. Configuration
                                           units are in bytes */
    bcmFabricCreditSizeRemoteDefault = 36, /* Default remote credit size */
    bcmFabricVsqCategory = 37,          /* VSQ Catagory Mode */
    bcmFabricMulticastSchedulerMode = 38, /* 0 => Trafic Class only mode, 1 =>
                                           {Traffic class, group} mode */
    bcmFabricMcLowPrioMin = 39,         /* Start index of low priority multicast
                                           groups */
    bcmFabricMcLowPrioMax = 40,         /* End index of low priority multicast
                                           groups */
    bcmFabricMcMidPrioMin = 41,         /* Start index of medium priority
                                           multicast groups */
    bcmFabricMcMidPrioMax = 42,         /* End index of medium priority
                                           multicast groups */
    bcmFabricEnableMcLowPrioDrop = 43,  /*  Enable dropping low priority
                                           mulicast */
    bcmFabricMcLowPrioDropThUp = 44,    /* Dropping low priority multicast high
                                           threshold */
    bcmFabricMcLowPrioDropThDown = 45,  /* Dropping low priority multicast low
                                           threshold */
    bcmFabricMcLowPrioDropSelect = 46,  /* Select the highet priority to be
                                           treated as low */
    bcmFabricTsApplicationHierachySetup = 47, /* Application manage ingress node
                                           resources. 1 indicates application
                                           manage, 0 indicate SDK auto manage */
    bcmFabricMaxPorts = 48,             /* Maximum fabric ports to be supported
                                           by SDK */
    bcmFabricEgressDropLimitBytes = 49, /* Total system-wide buffer drop limit */
    bcmFabricEgressRedDropLimitBytes = 50, /* Total system-wide Red drop limit */
    bcmFabricEgressYellowDropLimitBytes = 51, /* Total system-wide Yellow drop limit */
    bcmFabricDemandCalculationEnable = 52, /* Enable the demand calculation (on QE) */
    bcmFabricOperatingIntervalEnable = 53, /* Enable the start of epoch/operating
                                           interval (on standby arbiter) */
    bcmFabricIngressLevel1NumSchedulers = 54, /* Number of ingress schedulers on level
                                           1 */
    bcmFabricIngressLevel2NumSchedulers = 55, /* Number of ingress schedulers on level
                                           2 */
    bcmFabricIngressLevel3NumSchedulers = 56, /* Number of ingress schedulers on level
                                           3 */
    bcmFabricIngressLevel4NumSchedulers = 57, /* Number of ingress schedulers on level
                                           4 */
    bcmFabricIngressLevel5NumSchedulers = 58, /* Number of ingress schedulers on level
                                           5 */
    bcmFabricIngressLevel6NumSchedulers = 59, /* Number of ingress schedulers on level
                                           6 */
    bcmFabricIngressLevel7NumSchedulers = 60, /* Number of ingress schedulers on level
                                           7 */
    bcmFabricIngressLevel1SchedulerUpdateCycles = 61, /* Ingress scheduler update cycles on
                                           level 1 */
    bcmFabricIngressLevel2SchedulerUpdateCycles = 62, /* Ingress scheduler update cycles on
                                           level 2 */
    bcmFabricIngressLevel3SchedulerUpdateCycles = 63, /* Ingress scheduler update cycles on
                                           level 3 */
    bcmFabricIngressLevel4SchedulerUpdateCycles = 64, /* Ingress scheduler update cycles on
                                           level 4 */
    bcmFabricIngressLevel5SchedulerUpdateCycles = 65, /* Ingress scheduler update cycles on
                                           level 5 */
    bcmFabricIngressLevel6SchedulerUpdateCycles = 66, /* Ingress scheduler update cycles on
                                           level 6 */
    bcmFabricIngressLevel7SchedulerUpdateCycles = 67, /* Ingress scheduler update cycles on
                                           level 7 */
    bcmFabricIsolate = 68,              /* ArgValue: 1: Isolate the fabric
                                           element from the system 0: Undo the
                                           Isolation action */
    bcmFabricRCIControlSource = 69,     /* In Dual switch mode on links that
                                           emit 128B cells, it determines the
                                           sources for RCI bits. Whether from
                                           the Primary queue, from the Secondary
                                           queue, the OR of both, or none */
    bcmFabricGCIControlSource = 70,     /* In Dual switch mode on links that
                                           emit 128B cells, it determines the
                                           source for GCI bits. Whether from the
                                           Primary queue, from the Secondary
                                           queue, the MAX of both, or none */
    bcmFabricControlECNEnable = 71,     /* Enable the support of ECN */
    bcmFabricOcbMulticastId1Min = 72,   /* define the 1st Multicast range
                                           minimum. If the Multicast Packet is
                                           in this range, the start-of-packet is
                                           offloaded to the OCB (On-Chip Buffer) */
    bcmFabricOcbMulticastId1Max = 73,   /* define the 1st Multicast range
                                           maximum */
    bcmFabricOcbMulticastId2Min = 74,   /* define the 2nd Multicast range
                                           minimum. */
    bcmFabricOcbMulticastId2Max = 75,   /* define the 2nd Multicast range
                                           maximum */
    bcmFabricShutdown = 76,             /* performs graceful shutdown of the
                                           device */
    bcmFabricRCIIncrementValue = 77,    /* The value that will be added to RCI
                                           bucket whenever a cell carrying RCI
                                           indication arrives */
    bcmFabricGciLeakyBucketEnable = 78, /* Enable/Disable GCI leaky bucket
                                           mechanism */
    bcmFabricGciBackoffEnable = 79,     /* Enable/Disable GCI random backoff
                                           mechanism */
    bcmFabricMinimalLinksToDestination = 80, /* Define minimum number of links to
                                           destination */
    bcmFabricControlCellsEnable = 81,   /* Enable/Disable fabric control cells */
    bcmFabricForceTdmBypassTrafficToFabric = 82, /* Force TDM bypass traffic to fabric or
                                           not */
    bcmFabricControlRciThrottlingMode = 83, /* Used to set RCI Throttling mode, see */
    bcmFabricDelaySingleCellInFabricRx = 84, /* Enable/Disable delay of single cell
                                           in fabric rx, to improve bus
                                           utilization */
    bcmFabricControlDirectMcModidOffset = 85, /* The offset from which the modids of
                                           the FAPs connected to the FE start.
                                           Relevant for direct multicast mode
                                           only */
    bcmFabricLocalRouteControlCellsEnable = 86 /* Enable/disable FE1 control traffic
                                           directly to FAPs without going
                                           through FE2 */
} bcm_fabric_control_t;

/* bcm_fabric_mode_t */
typedef enum bcm_fabric_mode_e {
    bcmFabricModeTME = 0,               /* QE Standalone/TME */
    bcmFabricModeFabric = 1,            /* QE Uses external fabric device */
    bcmFabricModeArbiterCrossbar = 2,   /* BM configured as both and arbiter and
                                           a crossbar */
    bcmFabricModeCrossbar = 3,          /* BM configured as a crossbar without
                                           arbiter functionality */
    bcmFabricModeArbiterCapableCrossbar = 4 /* BM configured as xbar without arbiter
                                           functionality, but capable of to
                                           switching arbiter */
} bcm_fabric_mode_t;

typedef enum bcm_fabric_pipe_e {
    bcmFabricPipe0 = 0,     /* pipe 0 */
    bcmFabricPipe1 = 1,     /* pipe 1 */
    bcmFabricPipe2 = 2,     /* pipe 2 */
    bcmFabricPipeAll = -1   /* all pipes */
} bcm_fabric_pipe_t;

/* bcm_fabric_connection_mode_t */
typedef enum bcm_fabric_connection_mode_e {
    bcmFabricXbarConnectionModeA = 1,   /* Plane A */
    bcmFabricXbarConnectionModeB = 2,   /* Plane B */
    bcmFabricXbarConnectionModeC = 3    /* Worst Case Plane A/Plane B */
} bcm_fabric_connection_mode_t;

typedef enum bcm_fabric_level_e {
    bcmFabricLevelAll = -1, /* all levels */
    bcmFabricLevel0 = 0,    /* level 0 */
    bcmFabricLevel1 = 1,    /* level 1 */
    bcmFabricLevel2 = 2,    /* level 2 */
    bcmFabricLevel3 = 3,    /* level 3 */
    bcmFabricLevel4 = 4,    /* level 4 */
    bcmFabricLevel5 = 5,    /* level 5 */
    bcmFabricLevel6 = 6,    /* level 6 */
    bcmFabricLevelNof = 7 
} bcm_fabric_level_t;

typedef enum bcm_fabric_leaky_bucket_e {
    bcmFabricLeakyBucketAll = -1,   /* all Leaky Buckets */
    bcmFabricLeakyBucket0 = 0,      /* Leaky Bucket 0 */
    bcmFabricLeakyBucket1 = 1,      /* Leaky Bucket 1 */
    bcmFabricLeakyBucket2 = 2,      /* Leaky Bucket 2 */
    bcmFabricLeakyBucket3 = 3,      /* Leaky Bucket 3 */
    bcmFabricLeakyBucketNof = 4 
} bcm_fabric_leaky_bucket_t;

typedef enum bcm_fabric_backoff_timer_e {
    bcmFabricBackoffTimerAll = -1,  /* all Backoff timers */
    bcmFabricBackoffTimer0 = 0,     /* Backoff timer 0 */
    bcmFabricBackoffTimer1 = 1,     /* Backoff timer 1 */
    bcmFabricBackoffTimer2 = 2,     /* Backoff timer 2 */
    bcmFabricBackoffTimerNof = 3 
} bcm_fabric_backoff_timer_t;

typedef enum bcm_fabric_phase_e {
    bcmFabricPhaseAll = -1, /* all Phases */
    bcmFabricPhase0 = 0,    /* Phase 0 */
    bcmFabricPhase1 = 1,    /* Phase 1 */
    bcmFabricPhaseNof = 2 
} bcm_fabric_phase_t;

typedef enum bcm_fabric_mc_shaper_e {
    bcmFabricMcShaperAll = -1,      /* all FMC shapers */
    bcmFabricMcShaperGfmc = 0,      /* FMC-HP Shaper */
    bcmFabricMcShaperBfmc = 1,      /* FMC-LP Shaper */
    bcmFabricMcShaperTotalFmc = 2,  /* FMC-Total shaper */
    bcmFabricMcShaperNof = 3 
} bcm_fabric_mc_shaper_t;

typedef enum bcm_fabric_cast_e {
    bcmFabricCastAll = -1,  /* all casts */
    bcmFabricCastUC = 0,    /* Unicast */
    bcmFabricCastMC = 1,    /* Multicast */
    bcmFabricCastNof = 2 
} bcm_fabric_cast_t;

typedef enum bcm_fabric_priority_e {
    bcmFabricPriorityAll = -1,  /* All priorities */
    bcmFabricPriority0 = 0,     /* Priority 0 */
    bcmFabricPriority1 = 1,     /* Priority 1 */
    bcmFabricPriority2 = 2,     /* Priority 2 */
    bcmFabricPriority3 = 3,     /* Priority 3 */
    bcmFabricPriorityNof = 4 
} bcm_fabric_priority_t;

/* BCM_FABRIC_XBAR_ABILITY_ flags */
#define BCM_FABRIC_XBAR_ABILITY_PROTOCOL_A  1          /* Plane A */
#define BCM_FABRIC_XBAR_ABILITY_PROTOCOL_B  2          /* Plane B */
#define BCM_FABRIC_XBAR_ABILITY_PROTOCOL_C  3          /* Worst Case Plane
                                                          A/Plane B */

/* bcm_fabric_redundancy_mode_t */
typedef enum bcm_fabric_redundancy_mode_e {
    bcmFabricRedManual = 0,     /* Manual Mode */
    bcmFabricRed1Plus1Both = 1, /* 1+1 Control Plane and 1+1 Data Plane */
    bcmFabricRed1Plus1LS = 2,   /* 1+1 Control Plane and Load Sharing Data Plane */
    bcmFabricRed1Plus1ELS = 3,  /* 1+1 Control Planeand Enhanced Load Sharing
                                   Data Plane */
    bcmFabricRedLS = 4,         /* Load Sharing Data Plane */
    bcmFabricRedELS = 5         /* Enhanced Load Sharing Data Plane */
} bcm_fabric_redundancy_mode_t;

/* fabric_traffic_management_cos_mode_t */
typedef enum bcm_fabric_traffic_management_cos_mode_e {
    bcmFabricTrafficManagementCosModeSimple = 0, /* Simple Mode. Hierarchy setup by SDK */
    bcmFabricTrafficManagementCosModeFlexible = 1, /* Flexible Mode. Hierarchy setup by
                                           application */
    bcmFabricTrafficManagementCosModeHybrid = 2 /* Hybrid Mode. Hierarchy setup by SDK
                                           and application can add to it */
} bcm_fabric_traffic_management_cos_mode_t;

/* vsq catagory modes */
typedef enum bcm_fabric_vsq_category_mode_e {
    bcmFabricVsqCatagoryModeNone = 0,   /* None. By default, all queues are
                                           assigned to category 2 */
    bcmFabricVsqCatagoryMode1 = 1,      /* Category 0:  Spatial Multicast
                                           Queues, Category 1:  Ingress Shaping
                                           Queues, Category 2:  VoQ destination
                                           based queues, Category 3:  Egress TM
                                           queues */
    bcmFabricVsqCatagoryMode2 = 2       /* Category 2: Unicast Queues, Category
                                           0: all other queues */
} bcm_fabric_vsq_category_mode_t;

/* Information passed back during callback of redundancy state change */
typedef struct bcm_fabric_control_redundancy_info_s {
    int active_arbiter_id;  /* Active Arbiter Identifier. */
    uint64 xbars;           /* xbar links that are being used. */
} bcm_fabric_control_redundancy_info_t;

/* BCM_FABRIC_DISTRIBUTION_* flags. */
#define BCM_FABRIC_DISTRIBUTION_WITH_ID 0x00000001 /* flag to create
                                                      distributionId that is
                                                      passed */

/* Reference bcm_fabric_control_t, bcmFabricMcGroupSourceKnockout type. */
#define BCM_FABRIC_MC_GROUP_SOURCE_KNOCKOUT_ENABLE 0x40000000 /* Indicates if source
                                                          knockout is enabled on
                                                          a multicast group */
#define BCM_FABRIC_MC_GROUP_MASK            0x3FFFFFFF /* multicast group mask */

/* 
 * Allows more specific control in bcm_fabric_packet_adjust_get/set. 
 * With neither flag, get will fetch the ingress (or egress if the chip
 * does not support ingress) value, and set will set both values (of
 * whichever the chip supports if it supports only one).  ORing the
 * INGRESS flag with the selector ID will cause ingress to be set or
 * read.  ORing the EGRESS flag with the selector will cause egress to be
 * set or read.  ORing both INGRESS and EGRESS flags with the selector ID
 * will specifically set both, but is not valid for get.  The GLOBAL flag
 * indicates that the global adjust value (can be combined with ingress
 * and egress as appropriate to the device) is to be accessed, rather
 * than one of the indexed adjust values.
 */
#define BCM_FABRIC_PACKET_ADJUST_INGRESS    0x40000000 
#define BCM_FABRIC_PACKET_ADJUST_EGRESS     0x20000000 
#define BCM_FABRIC_PACKET_ADJUST_GLOBAL     0x10000000 
#define BCM_FABRIC_PACKET_ADJUST_SELECTOR_MASK 0x0FFFFFFF 

#ifndef BCM_HIDE_DISPATCHABLE

/* Initialize the BCM Fabric subsystem. */
extern int bcm_fabric_init(
    int unit);

/* Set fabric control attributes. */
extern int bcm_fabric_control_set(
    int unit, 
    bcm_fabric_control_t type, 
    int arg);

/* Set fabric control attributes. */
extern int bcm_fabric_control_get(
    int unit, 
    bcm_fabric_control_t type, 
    int *arg);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_FABRIC_DISTRIBUTION_SCHED_ALL   0x00000000 
#define BCM_FABRIC_DISTRIBUTION_SCHED_ANY   0x00000001 

/* Multicast Fabric distribution feature control. */
typedef enum bcm_fabric_distribution_control_e {
    bcmFabricDistributionSched = 0  /* Multicast fabric forwarding - all nodes
                                       must be available or any. */
} bcm_fabric_distribution_control_t;

/* Specifies the bit position and length of a field in packets */
typedef struct bcm_fabric_action_field_s {
    uint16 offset;  /* Bit offset of field in a packet */
    uint16 length;  /* Bit length of field in a packet */
} bcm_fabric_action_field_t;

/* Specifies where the predicate obtains its information for comparison */
typedef enum bcm_fabric_predicate_type_e {
    bcmFabricPredicateTypePacket = 0,   /* Predicate is based upon packet
                                           contents (field indicates which bits) */
    bcmFabricPredicateTypeInterface = 1, /* Predicate is based upon 'interface'
                                           metadatum (field is ignored) */
    bcmFabricPredicateTypeSysport = 2,  /* Predicate is based upon 'sysport'
                                           metadatum (field is ignored) */
    bcmFabricPredicateTypeChannel = 3,  /* Predicate is based upon 'channel'
                                           metadatum (field is ignored) */
    bcmFabricPredicateTypeFifo = 4,     /* Predicate is based upon 'fifo'
                                           metadatum (field is ignored) */
    bcmFabricPredicateTypeSubport = 5,  /* Predicate is based upon 'subport'
                                           metadatum (field is ignored) */
    bcmFabricPredicateTypePacketClass = 6, /* Predicate is based upon 'packet
                                           class' metadatum (field is ignored) */
    bcmFabricPredicateTypeFlags = 7     /* Predicate is based upon 'packet
                                           flags' metadata (field is ignored) */
} bcm_fabric_predicate_type_t;

#define BCM_FABRIC_PREDICATE_INFO_INGRESS   0x00000001 /* Predicate will be used
                                                          for ingress path
                                                          packets */
#define BCM_FABRIC_PREDICATE_INFO_EGRESS    0x00000002 /* Predicate will be used
                                                          for egress path
                                                          packets */
#define BCM_FABRIC_PREDICATE_INFO_WITH_ID   0x00000004 /* Predicate needs to be
                                                          created/replaced with
                                                          a specific ID */
#define BCM_FABRIC_PREDICATE_INFO_REPLACE   0x00000008 /* Predicate needs to be
                                                          replaced instead of
                                                          created */
#define BCM_FABRIC_PREDICATE_INFO_RANGE     0x00000010 /* Predicate comparison
                                                          is range instead of
                                                          equality-under-mask */

typedef struct bcm_fabric_predicate_info_s {
    uint32 flags;                       /* Flags controlling the creation and
                                           the operation of the parser */
    bcm_fabric_predicate_type_t source; /* Indicates from where the predicate
                                           will draw its comparison data */
    bcm_fabric_action_field_t field;    /* Indicates the bit field location in
                                           packets for packet type predicates;
                                           is ignored for metadata type
                                           predicates */
    uint32 data;                        /* Compare data for equality-under-mask
                                           comparisons (ignored for range) */
    uint32 mask;                        /* Masking value used for
                                           equality-under-mask comparisons
                                           (ignored for range) */
    uint32 range_low;                   /* Lowest included value for range
                                           comparisons (ignored for non-range) */
    uint32 range_high;                  /* Highest included value for range
                                           comparisons (ignored for non-range) */
} bcm_fabric_predicate_info_t;

/* Fabric predicate ID */
typedef int bcm_fabric_predicate_t;

/* Callback used by bcm_fabric_predicate_traverse */
typedef int (*bcm_fabric_predicate_traverse_cb)(
    int unit, 
    bcm_fabric_predicate_t pred_id, 
    bcm_fabric_predicate_info_t *pred_info, 
    void *user_data);

/* Initialize a bcm_fabric_predicate_info_t to a 'safe' default value. */
extern void bcm_fabric_predicate_info_t_init(
    int unit, 
    bcm_fabric_predicate_info_t *pred_info);

#ifndef BCM_HIDE_DISPATCHABLE

/* Creates or replaces a fabric predicate */
extern int bcm_fabric_predicate_create(
    int unit, 
    bcm_fabric_predicate_info_t *pred_info, 
    bcm_fabric_predicate_t *pred_id);

/* Destroys a fabric predicate */
extern int bcm_fabric_predicate_destroy(
    int unit, 
    bcm_fabric_predicate_t pred_id);

/* Destroys all fabric predicates on a unit */
extern int bcm_fabric_predicate_destroy_all(
    int unit);

/* Gets a fabric predicate */
extern int bcm_fabric_predicate_get(
    int unit, 
    bcm_fabric_predicate_t pred_id, 
    bcm_fabric_predicate_info_t *pred_info);

/* Traverse all of the fabric predicates */
extern int bcm_fabric_predicate_traverse(
    int unit, 
    bcm_fabric_predicate_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Indicates source for queue index. */
typedef enum bcm_fabric_action_queue_idx_from_e {
    bcmFabricActionQueueIdxFromPacket = 0, /* Take queue index from queue field of
                                           packet */
    bcmFabricActionQueueIdxFromReplicantId = 1, /* Take queue index from multicast
                                           replicant ID */
    bcmFabricActionQueueIdxFromReplicantLookup = 2 /* Take queue index from result of
                                           multicast replicant lookup */
} bcm_fabric_action_queue_idx_from_t;

/* Indicates source data (and size) selection for insert action */
typedef enum bcm_fabric_action_insert_e {
    bcmFabricActionInsertRepId16 = 0,   /* Insert 16-bit replicant ID from
                                           multicast path */
    bcmFabricActionInsertRepId18 = 1,   /* Insert 18-bit replicant ID from
                                           multicast path, overwriting low two
                                           bits of prior byte */
    bcmFabricActionInsertEncapId16 = 2, /* Insert 16-bit encapId from primary
                                           lookup (based upon replicant ID) */
    bcmFabricActionInsertEncapId18 = 3, /* Insert 18-bit encapId from primary
                                           lookup (based upon replicant ID),
                                           overwriting low two bits of prior
                                           byte */
    bcmFabricActionInsertQueueIndex16 = 4, /* Insert 16-bit queue index pulled from
                                           packet by action */
    bcmFabricActionInsertQueueIndex18 = 5, /* Insert 18-bit queue index pulled from
                                           packet by action, overwriting low two
                                           bits of prior byte */
    bcmFabricActionInsertQueueId16 = 6, /* Insert 16-bit queue ID from secondary
                                           lookup (based upon queue index pulled
                                           from packet by action) */
    bcmFabricActionInsertQueueId18 = 7  /* Insert 18-bit queue ID from secondary
                                           lookup (based upon queue index pulled
                                           from packet by action), overwriting
                                           low two bits of prior byte */
} bcm_fabric_action_insert_t;

#define BCM_FABRIC_ACTION_INFO_INGRESS      0x00000001 /* Action will be used
                                                          for ingress path
                                                          packets */
#define BCM_FABRIC_ACTION_INFO_EGRESS       0x00000002 /* Action will be used
                                                          for egress path
                                                          packets */
#define BCM_FABRIC_ACTION_INFO_WITH_ID      0x00000004 /* Action needs to be
                                                          created/replaced with
                                                          a specific ID */
#define BCM_FABRIC_ACTION_INFO_REPLACE      0x00000008 /* Action needs to be
                                                          replaced instead of
                                                          created */
#define BCM_FABRIC_ACTION_INFO_QUEUE_DEFAULT 0x00000010 /* Use global default
                                                          queue instead of
                                                          deriving one from the
                                                          packet */
#define BCM_FABRIC_ACTION_INFO_USE_TAG_OFFSET 0x00000020 /* Include HGX extension
                                                          queue tag in queue
                                                          selection process */
#define BCM_FABRIC_ACTION_INFO_OVERWRITE_DEST 0x00000040 /* Overwrite packet
                                                          destination with
                                                          selected queue */
#define BCM_FABRIC_ACTION_INFO_LENGTH_ADJUST 0x00000080 /* Use packet len_adj_idx
                                                          to select length
                                                          adjustment */
#define BCM_FABRIC_ACTION_INFO_CLEAR_TEST_BIT 0x00000100 /* Clear the 'test' bit
                                                          in the packet header */
#define BCM_FABRIC_ACTION_INFO_LOOKUP_ENCAPID 0x00000200 /* Lookup encapId from
                                                          multicast replicantId */
#define BCM_FABRIC_ACTION_INFO_CLEAR_MC_BIT 0x00000400 /* Clear the 'multicast'
                                                          bit in the packet
                                                          header */
#define BCM_FABRIC_ACTION_INFO_LEN_ADJ_ON_SHAPING 0x00000800 /* Apply length
                                                          adjustment to the data
                                                          sent to shaping */
#define BCM_FABRIC_ACTION_INFO_LOOKUP_QUEUEID 0x00001000 /* Lookup queueId from
                                                          the queue index */
#define BCM_FABRIC_ACTION_INFO_XGS_MODE     0x00002000 /* Overwrite packet
                                                          header destination
                                                          field according to the
                                                          FIFO into which the
                                                          packet is placed */
#define BCM_FABRIC_ACTION_INFO_STAT0_ADJ_PER_PACKET 0x00004000 /* Adjust stat 0 per
                                                          frame */
#define BCM_FABRIC_ACTION_INFO_STAT1_ADJ_PER_PACKET 0x00008000 /* Adjust stat 1 per
                                                          frame */
#define BCM_FABRIC_ACTION_INFO_STAT_COMBINE 0x00010000 /* Combine stat 0 and 1
                                                          so stat 0 is low bits
                                                          and stat 1 is high
                                                          bits */

/* Fabric action ID */
typedef int bcm_fabric_action_t;

/* Fabric qsel ID */
typedef int bcm_fabric_qsel_t;

/* Fabric qsel_offset ID */
typedef int bcm_fabric_qsel_offset_t;

/* Fabric statistics segment ID */
typedef int bcm_fabric_stats_segment_t;

/* Specifies the bit position and length of a field in packets */
typedef struct bcm_fabric_action_info_s {
    uint32 flags;                       /* Flags controlling the creation and
                                           operation of the action */
    bcm_fabric_action_field_t queue_field_low; /* Selects bits from packet to be used
                                           for low bits of queue index */
    bcm_fabric_action_field_t queue_field_high; /* Selects bits from packet to be used
                                           for high bits of queue index */
    bcm_fabric_action_field_t qsel_offset_index_field; /* Selects bits from packet to be used
                                           for qsel_offset index */
    int override_dp;                    /* Override value for packet drop
                                           precedence; set -1 to use value from
                                           packet */
    int override_ecn;                   /* Override value for packet ECN bits;
                                           set -1 to use value from packet */
    bcm_fabric_action_field_t len_adj_index_field; /* Selects bits from packet to be used
                                           for len_adj index */
    bcm_fabric_action_queue_idx_from_t queue_id_source; /* Selects whether to draw initial queue
                                           index from packet or multicast path */
    uint32 queue_index_offset;          /* Offset to apply to queue index during
                                           lookup of queue_id */
    bcm_fabric_qsel_t qsel;             /* Specifies the qsel that is to be used
                                           for queue lookup; if this is zero,
                                           the queue lookup is not performed and
                                           qsel_offset will be chosen according
                                           to the qsel_offset field rather than
                                           read during the lookup */
    bcm_fabric_qsel_offset_t qsel_offset; /* Specifies the qsel_offset to use if
                                           qsel is zero */
    bcm_fabric_action_field_t encap_id_field; /* Which bits in the packet to overwrite
                                           with the encap_id */
    bcm_fabric_action_field_t queue_id_field; /* Which bits in the packet to overwrite
                                           with the queue_id */
    int clear_ehv_bit;                  /* Which bit to clear for extended
                                           header valid bit clear; use -1 to
                                           inhibit */
    int clear_mp_bit;                   /* Which bit to clear for header MP bit
                                           clear; use -1 to inhibit */
    int four_byte_remove_offset;        /* Specifies the bit offset of a four
                                           byte block to remove from the packet;
                                           -1 indicates no removal */
    int header_bytes_remove;            /* Specifies a number of bytes to remove
                                           from the front of the packet; 0
                                           indicates no removal */
    bcm_fabric_action_insert_t insert_dest_select; /* Specifies the source (and size) of
                                           the insertion that will be done by an
                                           interface that is configured to
                                           perform a header insertion; the
                                           configuration of each interface
                                           determines whether the insertion is
                                           actually performed for packets on
                                           that interface */
    bcm_fabric_stats_segment_t stat0_segment; /* The statistics segment used for stat0 */
    bcm_fabric_predicate_type_t stat0_field_low_type; /* Indicate source of stat0 low bits */
    bcm_fabric_action_field_t stat0_field_low; /* Which bits from the packet select the
                                           low bits of stat0 */
    bcm_fabric_predicate_type_t stat0_field_high_type; /* Indicates source of stat0 high bits */
    bcm_fabric_action_field_t stat0_field_high; /* Which bits from the packet select the
                                           high bits of stat0 */
    int stat0_adjust;                   /* Adjustment applied to value for stat0
                                           during statistics update */
    bcm_fabric_stats_segment_t stat1_segment; /* The statistics segment used for stat0 */
    bcm_fabric_predicate_type_t stat1_field_low_type; /* Indicates source of stat1 low bits */
    bcm_fabric_action_field_t stat1_field_low; /* Which bits from the packet select the
                                           low bits of stat1 */
    bcm_fabric_predicate_type_t stat1_field_high_type; /* Indicates source of stat1 high bits */
    bcm_fabric_action_field_t stat1_field_high; /* Which bits from the packet select the
                                           high bits of stat1 */
    int stat1_adjust;                   /* Adjustment applied to value for stat1
                                           during statistics update */
} bcm_fabric_action_info_t;

/* Callback used by bcm_fabric_action_traverse */
typedef int (*bcm_fabric_action_traverse_cb)(
    int unit, 
    bcm_fabric_action_t action_id, 
    bcm_fabric_action_info_t *action_info, 
    void *user_data);

/* Set default values to a bcm_fabric_action_info_t. */
extern void bcm_fabric_action_info_t_init(
    int unit, 
    bcm_fabric_action_info_t *action_info);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create or replace a fabric action */
extern int bcm_fabric_action_create(
    int unit, 
    bcm_fabric_action_info_t *action_info, 
    bcm_fabric_action_t *action_id);

/* Destroy a fabric action */
extern int bcm_fabric_action_destroy(
    int unit, 
    bcm_fabric_action_t action_id);

/* Destroy all fabric actions on a unit */
extern int bcm_fabric_action_destroy_all(
    int unit);

/* Get information about an action */
extern int bcm_fabric_action_get(
    int unit, 
    bcm_fabric_action_t action_id, 
    bcm_fabric_action_info_t *action_info);

/* Traverse all of the fabric actions on a unit */
extern int bcm_fabric_action_traverse(
    int unit, 
    bcm_fabric_action_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_FABRIC_PREDICATE_ACTION_INFO_INGRESS 0x00000001 /* Predicate will be used
                                                          for ingress path
                                                          packets */
#define BCM_FABRIC_PREDICATE_ACTION_INFO_EGRESS 0x00000002 /* Predicate will be used
                                                          for egress path
                                                          packets */
#define BCM_FABRIC_PREDICATE_ACTION_INFO_WITH_ID 0x00000004 /* Predicate needs to be
                                                          created/replaced with
                                                          a specific ID */
#define BCM_FABRIC_PREDICATE_ACTION_INFO_REPLACE 0x00000008 /* Predicate needs to be
                                                          replaced instead of
                                                          created */

#define BCM_FABRIC_PREDICATE_COUNT  32         /* Maximum number of supported
                                                  fabric predicates */

/* 
 * Used to specify mask and data for predicates when connecting them to
 * actions
 */
typedef SHR_BITDCL bcm_fabric_predicate_vector_t[_SHR_BITDCLSIZE(BCM_FABRIC_PREDICATE_COUNT)];

#define BCM_FABRIC_PREDICATE_VECTOR_GET(_vec, _n)  SHR_BITGET((_vec), (_n)) 
#define BCM_FABRIC_PREDICATE_VECTOR_SET(_vec, _n)  SHR_BITSET((_vec), (_n)) 
#define BCM_FABRIC_PREDICATE_VECTOR_CLR(_vec, _n)  SHR_BITCLR((_vec), (_n)) 

/* Fabric predicate action ID */
typedef int bcm_fabric_predicate_action_t;

typedef struct bcm_fabric_predicate_action_info_s {
    uint32 flags;                       /* Flags indicating how this
                                           predicate_action is to be
                                           created/replaced */
    int priority;                       /* The priority for this
                                           predicate_action */
    bcm_fabric_predicate_vector_t data; /* The value of the predicate vector for
                                           this predicate_action */
    bcm_fabric_predicate_vector_t mask; /* Indicates which predicates are
                                           significant in the data field */
    bcm_fabric_action_t action_id;      /* Specifies the action to be taken when
                                           the specified predicate condition is
                                           met */
} bcm_fabric_predicate_action_info_t;

/* Callback used by bcm_fabric_predicate_action_traverse */
typedef int (*bcm_fabric_predicate_action_traverse_cb)(
    int unit, 
    bcm_fabric_predicate_action_t predicate_action_id, 
    bcm_fabric_predicate_action_info_t *predicate_action_info, 
    void *user_data);

/* Set up default values for a bcm_fabric_predicate_action_info_t */
extern void bcm_fabric_predicate_action_info_t_init(
    int unit, 
    bcm_fabric_predicate_action_info_t *predicate_action_info);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create or replace a fabric predicate_action rule. */
extern int bcm_fabric_predicate_action_create(
    int unit, 
    bcm_fabric_predicate_action_info_t *predicate_action, 
    bcm_fabric_predicate_action_t *predicate_action_id);

/* Read a fabric predicate action */
extern int bcm_fabric_predicate_action_get(
    int unit, 
    bcm_fabric_predicate_action_t predicate_action_id, 
    bcm_fabric_predicate_action_info_t *predicate_action_info);

/* Destroy a fabric predicate_action rule */
extern int bcm_fabric_predicate_action_destroy(
    int unit, 
    bcm_fabric_predicate_action_t predicate_action_id);

/* Destroy all predicate_action rules on a unit */
extern int bcm_fabric_predicate_action_destroy_all(
    int unit);

/* Traverse the fabric predicate_action rules on a unit */
extern int bcm_fabric_predicate_action_traverse(
    int unit, 
    bcm_fabric_predicate_action_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_FABRIC_QSEL_INGRESS 0x00000001 /* qsel will be used for ingress path
                                              packets */
#define BCM_FABRIC_QSEL_EGRESS  0x00000002 /* qsel will be used for egress path
                                              packets */
#define BCM_FABRIC_QSEL_WITH_ID 0x00000004 /* qsel needs to be created/replaced
                                              with a specific ID */
#define BCM_FABRIC_QSEL_REPLACE 0x00000008 /* qsel needs to be replaced instead
                                              of created */

/* Callback used by bcm_fabric_qsel_traverse */
typedef int (*bcm_fabric_qsel_traverse_cb)(
    int unit, 
    bcm_fabric_qsel_t qsel_id, 
    uint32 flags, 
    int base, 
    int count, 
    void *user_data);

/* Callback used by bcm_fabric_qsel_entry_traverse */
typedef int (*bcm_fabric_qsel_entry_traverse_cb)(
    int unit, 
    bcm_fabric_qsel_t qsel_id, 
    int offset, 
    bcm_gport_t queue, 
    bcm_fabric_qsel_offset_t qsel_offset, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create a fabric qsel */
extern int bcm_fabric_qsel_create(
    int unit, 
    uint32 flags, 
    int base, 
    int count, 
    bcm_fabric_qsel_t *qsel_id);

/* Destroy a fabric qsel */
extern int bcm_fabric_qsel_destroy(
    int unit, 
    bcm_fabric_qsel_t qsel_id);

/* Destroy all fabric qsels on a unit */
extern int bcm_fabric_qsel_destroy_all(
    int unit);

/* Get information about a particular qsel */
extern int bcm_fabric_qsel_get(
    int unit, 
    bcm_fabric_qsel_t qsel_id, 
    uint32 *flags, 
    int *base, 
    int *count);

/* Traverse the qsels on a unit */
extern int bcm_fabric_qsel_traverse(
    int unit, 
    bcm_fabric_qsel_traverse_cb cb, 
    void *user_data);

/* 
 * Set/get the base queue and qsel_offset for a particular index within a
 * qsel
 */
extern int bcm_fabric_qsel_entry_set(
    int unit, 
    bcm_fabric_qsel_t qsel_id, 
    int offset, 
    bcm_gport_t queue, 
    bcm_fabric_qsel_offset_t qsel_offset_id);

/* 
 * Set/get the base queue and qsel_offset for a particular index within a
 * qsel
 */
extern int bcm_fabric_qsel_entry_get(
    int unit, 
    bcm_fabric_qsel_t qsel_id, 
    int offset, 
    bcm_gport_t *queue, 
    bcm_fabric_qsel_offset_t *qsel_offset_id);

/* Set/get a number of entries within a qsel. */
extern int bcm_fabric_qsel_entry_multi_set(
    int unit, 
    bcm_fabric_qsel_t qsel_id, 
    int offset, 
    int count, 
    bcm_gport_t *queue, 
    bcm_fabric_qsel_offset_t *qsel_offset_id);

/* Set/get a number of entries within a qsel. */
extern int bcm_fabric_qsel_entry_multi_get(
    int unit, 
    bcm_fabric_qsel_t qsel_id, 
    int offset, 
    int count, 
    bcm_gport_t *queue, 
    bcm_fabric_qsel_offset_t *qsel_offset_id);

/* Traverse the valid entries within a qsel */
extern int bcm_fabric_qsel_entry_traverse(
    int unit, 
    bcm_fabric_qsel_t qsel_id, 
    bcm_fabric_qsel_entry_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_FABRIC_QSEL_OFFSET_INGRESS  0x00000001 /* qsel will be used for
                                                      ingress path packets */
#define BCM_FABRIC_QSEL_OFFSET_EGRESS   0x00000002 /* qsel will be used for
                                                      egress path packets */
#define BCM_FABRIC_QSEL_OFFSET_WITH_ID  0x00000004 /* qsel needs to be
                                                      created/replaced with a
                                                      specific ID */
#define BCM_FABRIC_QSEL_OFFSET_REPLACE  0x00000008 /* qsel needs to be replaced
                                                      instead of created */

/* Callback used by bcm_fabric_qsel_offset_traverse */
typedef int (*bcm_fabric_qsel_offset_traverse_cb)(
    int unit, 
    bcm_fabric_qsel_offset_t qsel_offset_id, 
    uint32 flags, 
    void *user_data);

/* Callback used by bcm_fabric_qsel_offset_entry_traverse */
typedef int (*bcm_fabric_qsel_offset_entry_traverse_cb)(
    int unit, 
    bcm_fabric_qsel_offset_t qsel_offset_id, 
    bcm_cos_t int_pri, 
    int offset, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create a fabric qsel_offset */
extern int bcm_fabric_qsel_offset_create(
    int unit, 
    uint32 flags, 
    bcm_fabric_qsel_offset_t *qsel_offset_id);

/* Destroy a fabric qsel_offset */
extern int bcm_fabric_qsel_offset_destroy(
    int unit, 
    bcm_fabric_qsel_offset_t qsel_offset_id);

/* Destroy all qsel_offsets on a unit */
extern int bcm_fabric_qsel_offset_destroy_all(
    int unit);

/* Traverse the qsel_offsets on a unit */
extern int bcm_fabric_qsel_offset_traverse(
    int unit, 
    bcm_fabric_qsel_offset_traverse_cb cb, 
    void *user_data);

/* Set/get an entry in a fabric qsel_offset */
extern int bcm_fabric_qsel_offset_entry_set(
    int unit, 
    bcm_fabric_qsel_offset_t qsel_offset_id, 
    bcm_cos_t int_pri, 
    int offset);

/* Set/get an entry in a fabric qsel_offset */
extern int bcm_fabric_qsel_offset_entry_get(
    int unit, 
    bcm_fabric_qsel_offset_t qsel_offset_id, 
    bcm_cos_t int_pri, 
    int *offset);

/* Traverse the entries of a qsel_offset */
extern int bcm_fabric_qsel_offset_entry_traverse(
    int unit, 
    bcm_fabric_qsel_offset_t qsel_offset_id, 
    bcm_fabric_qsel_offset_entry_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_GROUP_MODID_BIT             \
(1U<<31) /* Group indicator bit */
#define BCM_LOCAL_MODID_BIT             \
(1U<<30) /* Local modid indicator bit */
#define BCM_FABRIC_GROUP_MODID_SET(id)  \
id = id | BCM_GROUP_MODID_BIT; /* The macro will internally
                                                      add group_base to define a
                                                      separate number space
                                                      representing a FAPs group */
#define BCM_FABRIC_LOCAL_MODID_SET(id)  \
        id = id | BCM_LOCAL_MODID_BIT;   /* The macro will internally
                                                      add local_base to define a
                                                      separate number space
                                                      representing a local index
                                                      of a single Module */
#define BCM_FABRIC_MODID_IS_GROUP(modid)  \
        (modid & BCM_GROUP_MODID_BIT ? 1 : 0)  /* True if the modid
                                                      represents a FAPs group */
#define BCM_FABRIC_MODID_IS_LOCAL(modid)  \
        (modid & BCM_LOCAL_MODID_BIT ? 1 : 0)  /* True if the modid
                                                      represents a Module local
                                                      index */
#define BCM_FABRIC_MODID_IS_MODULE(modid)  \
        !BCM_FABRIC_MODID_IS_GROUP(modid) && !BCM_FABRIC_MODID_IS_LOCAL(modid) /* True if the modid
                                                      represents a Module index */
#define BCM_FABRIC_GROUP_MODID_UNSET(id)  \
id & ~BCM_GROUP_MODID_BIT /* Clear group indicator */
#define BCM_FABRIC_LOCAL_MODID_UNSET(id)  \
id & ~BCM_LOCAL_MODID_BIT   /* Clear local modid
                                                      indicator */

#ifndef BCM_HIDE_DISPATCHABLE

/* Set of Modules belong to a group */
extern int bcm_fabric_modid_group_set(
    int unit, 
    bcm_module_t group, 
    int modid_count, 
    bcm_module_t *modid_array);

/* Get of Modules belong to a group */
extern int bcm_fabric_modid_group_get(
    int unit, 
    bcm_module_t group, 
    int modid_max_count, 
    bcm_module_t *modid_array, 
    int *modid_count);

/* Find the relevant group for a given module id */
extern int bcm_fabric_modid_group_find(
    int unit, 
    bcm_module_t modid, 
    bcm_module_t *group);

/* Mapping\Getting a Module ID to local space */
extern int bcm_fabric_modid_local_mapping_set(
    int unit, 
    bcm_module_t local_modid, 
    bcm_module_t modid);

/* Mapping\Getting a Module ID to local space */
extern int bcm_fabric_modid_local_mapping_get(
    int unit, 
    bcm_module_t local_modid, 
    bcm_module_t *modid);

/* Update links topology, set all the links connected to a destination */
extern int bcm_fabric_link_topology_set(
    int unit, 
    bcm_module_t destination, 
    int links_count, 
    bcm_port_t *links_array);

/* Get links topology. */
extern int bcm_fabric_link_topology_get(
    int unit, 
    bcm_module_t destination, 
    int max_links_count, 
    int *links_count, 
    bcm_port_t *links_array);

/* Set the replications of the multicast group in the fabric element */
extern int bcm_fabric_multicast_set(
    int unit, 
    bcm_multicast_t group, 
    uint32 flags, 
    uint32 destid_count, 
    bcm_module_t *destid_array);

/* Get the replications of the multicast group in the fabric element */
extern int bcm_fabric_multicast_get(
    int unit, 
    bcm_multicast_t group, 
    uint32 flags, 
    int destid_count_max, 
    int *destid_count, 
    bcm_module_t *destid_array);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_FABRIC_MAX_MULTICAST_MODULES    (192)      /* Max number of MC
                                                          modules */

#define BCM_FABRIC_MAX_MULTICAST_TABLE_ENTRY_SIZE (194)      /* max MCT entry size */

typedef SHR_BITDCL bcm_fabric_module_vector_t[_SHR_BITDCLSIZE(BCM_FABRIC_MAX_MULTICAST_TABLE_ENTRY_SIZE)];

#define BCM_FABRIC_MODULE_VEC_GET(vec, modid)  SHR_BITGET(vec, modid) 
#define BCM_FABRIC_MODULE_VEC_SET(vec, modid)  SHR_BITSET(vec, modid) 
#define BCM_FABRIC_MODULE_VEC_CLR(vec, modid)  SHR_BITCLR(vec, modid) 
#define BCM_FABRIC_MODULE_VEC_ZERO(vec)  \
    sal_memset(vec, 0, SHR_BITALLOCSIZE(BCM_FABRIC_MAX_MULTICAST_TABLE_ENTRY_SIZE)) 

/* bcm_fabric_multicast_* flags */
#define BCM_FABRIC_MULTICAST_SET_ONLY       _SHR_FABRIC_MULTICAST_SET_ONLY 
#define BCM_FABRIC_MULTICAST_COMMIT_ONLY    _SHR_FABRIC_MULTICAST_COMMIT_ONLY 
#define BCM_FABRIC_MULTICAST_STATUS_ONLY    _SHR_FABRIC_MULTICAST_STATUS_ONLY 

#ifndef BCM_HIDE_DISPATCHABLE

/* Set replications of the multicast groups in the fabric element */
extern int bcm_fabric_multicast_multi_set(
    int unit, 
    uint32 flags, 
    uint32 ngroups, 
    bcm_multicast_t *groups, 
    bcm_fabric_module_vector_t *dest_array);

/* 
 * Get a set of replications of the multicast groups in the fabric
 * element
 */
extern int bcm_fabric_multicast_multi_get(
    int unit, 
    uint32 flags, 
    uint32 ngroups, 
    bcm_multicast_t *groups, 
    bcm_fabric_module_vector_t *dest_array);

#endif /* BCM_HIDE_DISPATCHABLE */

/* FabricLinkCellFormat flags */
#define BCM_FABRIC_LINK_CELL_FORMAT_VSC256_V1 _SHR_FABRIC_LINK_CELL_FORMAT_VSC256_V1 /* VS256_V1 cell format */
#define BCM_FABRIC_LINK_CELL_FORMAT_VSC128  _SHR_FABRIC_LINK_CELL_FORMAT_VSC128 /* VSC128 cell format */
#define BCM_FABRIC_LINK_CELL_FORMAT_VSC256_V2 _SHR_FABRIC_LINK_CELL_FORMAT_VSC256_V2 /* VS256_V2 cell format */

typedef enum bcm_fabric_link_control_e {
    bcmFabricLinkCellFormat = 0,        /* VSC256 or VSC128 */
    bcmFabricLinkCellInterleavingEnable = 1, /* Enable controls and data cells
                                           interleaving (only if link partner is
                                           Arad/Petra-B/FE1600) */
    bcmFabricLinkPrimaryWeight = 2,     /* Sets primary arbitrations weight for
                                           MAC-TX access, in Dual-switch mode.
                                           This mode supports two queues per
                                           link: Primary queue for Data traffic
                                           and Secondary queue for TDM traffic.
                                           The bandwidth allocation between the
                                           two types of traffic can be
                                           controlled by assigning Weights for
                                           each queue type, in the
                                           0:[NUMBER_OF_LINKS-1] range. Lower
                                           weight implies higher priority weight
                                           0 is taken as strict priority. */
    bcmFabricLinkSecondaryWeight = 3,   /* Sets secondary arbitrations weight
                                           for MAC-TX access, in Dual-switch
                                           mode. This mode supports two queues
                                           per link: Primary queue for Data
                                           traffic and Secondary queue for TDM
                                           traffic. The bandwidth allocation
                                           between the two types of traffic can
                                           be controlled by assigning Weights
                                           for each queue type, in the
                                           0:[NUMBER_OF_LINKS-1] range. Lower
                                           weight implies higher priority weight
                                           0 is taken as strict priority. */
    bcmFabricLinkIsSecondaryOnly = 4,   /* Sets the link to support Secondary
                                           (TDM) queue only (relevant only to
                                           VCS128 links) */
    bcmFabricLLFControlSource = 5,      /* In Dual switch mode, on links that
                                           emit 128B cells, it determined the
                                           source for LLFC bits. Whether from
                                           the Primary queue, from the Secondary
                                           queue, the OR of both, or none */
    bcmFabricLinkRepeaterDestinationLink = 6, /* For each input link, defines the
                                           output link that the traffic is
                                           forwarded to. Only relevant when
                                           setting the operation mode to
                                           'repeater mode'. The driver validates
                                           that the mapping is symmetric */
    bcmFabricLinkIsolate = 7,           /* 1: The link may be enabled in the
                                           SerDes level and in the MAC level,
                                           however, it's link partner will see
                                           the link as disabled, and will not
                                           use it for traffic distribution 0:
                                           Undo the isolation operation */
    bcmFabricLinkPcpEnable = 8,         /* Enable/disable Packet Cell Packing */
    bcmFabricLinkTxTrafficDisable = 9,  /* Enable/disable Sending cells over
                                           specific link */
    bcmFabricLinkRepeaterEnable = 10,   /* Enable/disable repeater link - enable
                                           means that the link is connected to
                                           repeater device */
    bcmFabricLinkRetimerConnect = 11,   /* Connect/Disconnect retimer links. */
    bcmFabricLinkRetimerFecEnable = 12  /* Enable FEC on a retimer link. This
                                           configuration can be set only prior
                                           to setting the link into retimer. */
} bcm_fabric_link_control_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set\Get fabric-link attribute per link. \br */
extern int bcm_fabric_link_control_set(
    int unit, 
    bcm_port_t link, 
    bcm_fabric_link_control_t type, 
    int arg);

/* Set\Get fabric-link attribute per link. \br */
extern int bcm_fabric_link_control_get(
    int unit, 
    bcm_port_t link, 
    bcm_fabric_link_control_t type, 
    int *arg);

#endif /* BCM_HIDE_DISPATCHABLE */

typedef struct bcm_fabric_link_remote_pipe_mapping_s {
    uint32 num_of_remote_pipes;         /* Number of pipes supported by the
                                           remote device */
    bcm_fabric_pipe_t *remote_pipe_mapping; /* The mapping from remote pipe
                                           (represented by the index) to the
                                           local pipe (represented by the value
                                           stored in this index) */
    uint32 remote_pipe_mapping_max_size; /* Max size of remote_pipe_mapping */
} bcm_fabric_link_remote_pipe_mapping_t;

/* 
 * Initialize a bcm_fabric_link_remote_pipe_mapping_t to a 'safe' default
 * value.
 */
extern void bcm_fabric_link_remote_pipe_mapping_t_init(
    bcm_fabric_link_remote_pipe_mapping_t *pipe_mapping);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Set/get per link the mapping between the local pipe to the remote
 * pipe.
 */
extern int bcm_fabric_link_remote_pipe_mapping_set(
    int unit, 
    bcm_port_t port, 
    bcm_fabric_link_remote_pipe_mapping_t *mapping_config);

/* 
 * Set/get per link the mapping between the local pipe to the remote
 * pipe.
 */
extern int bcm_fabric_link_remote_pipe_mapping_get(
    int unit, 
    bcm_port_t port, 
    bcm_fabric_link_remote_pipe_mapping_t *mapping_config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* fabric link threshold types */
typedef enum bcm_fabric_link_threshold_type_e {
    bcmFabricLinkRxFifoLLFC = 0,        /* the threshold that initiates link
                                           level flow control when the receiving
                                           FIFO is overloaded */
    bcmFabricLinkFE1TxBypassLLFC = 1,   /* Only relevant for FE1 links. When the
                                           transmitting threshold is overloaded;
                                           the receiving FIFOs of all the FE1
                                           links start generating link level
                                           flow control */
    bcmFabricLinkRciFC = 2,             /* The TX FIFO threshold; that initiates
                                           the Route Congestion Indication flow
                                           control */
    bcmFabricLinkTxGciLvl1FC = 3,       /* The TX FIFO threshold that initiates
                                           the Global Congestion Indication
                                           level 1 Flow Vontrol. */
    bcmFabricLinkTxGciLvl2FC = 4,       /* The TX FIFO threshold that initiates
                                           the Global Congestion Indication
                                           level 2 Flow Vontrol. */
    bcmFabricLinkTxGciLvl3FC = 5,       /* The TX FIFO threshold that initiates
                                           the Global Congestion Indication
                                           level 3 Flow Vontrol. */
    bcmFabricLinkRxGciLvl1FC = 6,       /* The RX FIFO threshold that initiates
                                           the Global Congestion Indication
                                           level 1 Flow Control. */
    bcmFabricLinkRxGciLvl2FC = 7,       /* The RX FIFO threshold that initiates
                                           the Global Congestion Indication
                                           level 2 Flow Control. */
    bcmFabricLinkRxGciLvl3FC = 8,       /* The RX FIFO threshold that initiates
                                           the Global Congestion Indication
                                           level 3 Flow Control. */
    bcmFabricLinkTxPrio0Drop = 9,       /* The TX FIFO threshold; that initiate
                                           the priority 0 packet dropping. */
    bcmFabricLinkTxPrio1Drop = 10,      /* The TX FIFO threshold; that initiate
                                           the priority 1 packet dropping. */
    bcmFabricLinkTxPrio2Drop = 11,      /* The TX FIFO threshold; that initiate
                                           the priority 2 packet dropping. */
    bcmFabricLinkTxPrio3Drop = 12,      /* The TX FIFO threshold; that initiate
                                           the priority 3 packet dropping. */
    bcmFabricLinkRxPrio0Drop = 13,      /*  The RX FIFO threshold that initiate
                                           the priority 0 packet dropping. */
    bcmFabricLinkRxPrio1Drop = 14,      /*  The RX FIFO threshold that initiate
                                           the priority 1 packet dropping. */
    bcmFabricLinkRxPrio2Drop = 15,      /*  The RX FIFO threshold that initiate
                                           the priority 2 packet dropping. */
    bcmFabricLinkRxPrio3Drop = 16,      /*  The RX FIFO threshold that initiate
                                           the priority 3 packet dropping. */
    bcmFabricLinkGciLeakyBucket1Congestion = 17, /* Configure GCI leaky bucket 1
                                           congestion threshold */
    bcmFabricLinkGciLeakyBucket2Congestion = 18, /* Configure GCI leaky bucket 2
                                           congestion threshold */
    bcmFabricLinkGciLeakyBucket3Congestion = 19, /* Configure GCI leaky bucket 3
                                           congestion threshold */
    bcmFabricLinkGciLeakyBucket4Congestion = 20, /* Configure GCI leaky bucket 4
                                           congestion threshold */
    bcmFabricLinkGciLeakyBucket1Full = 21, /* Configure GCI leaky bucket 1 full
                                           value */
    bcmFabricLinkGciLeakyBucket2Full = 22, /* Configure GCI leaky bucket 2 full
                                           value */
    bcmFabricLinkGciLeakyBucket3Full = 23, /* Configure GCI leaky bucket 3 full
                                           value */
    bcmFabricLinkGciLeakyBucket4Full = 24, /* Configure GCI leaky bucket 4 full
                                           value */
    bcmFabricLinkRxRciLvl1FC = 25,      /* The RX FIFO threshold that initiates
                                           the Route Congestion Indication level
                                           1 Flow Control. */
    bcmFabricLinkRxRciLvl2FC = 26,      /* The RX FIFO threshold that initiates
                                           the Route Congestion Indication level
                                           2 Flow Control. */
    bcmFabricLinkRxRciLvl3FC = 27,      /* The RX FIFO threshold that initiates
                                           the Route Congestion Indication level
                                           3 Flow Control. */
    bcmFabricLinkRxFull = 28,           /* Configure threshold for max FIFO
                                           size, beyond this threshold, packets
                                           will be dropped. */
    bcmFabricLinkRxFifoSize = 29,       /* Configure the FIFO size, this is a
                                           static configuration. */
    bcmFabricLinkRxMcLowPrioDrop = 30,  /* Configure the multicast low priority
                                           cells drop thresholds. */
    bcmFabricLinkMidGciLvl1FC = 31,     /* Configure the Mid GCI threshold for
                                           level 1 flow control. */
    bcmFabricLinkMidGciLvl2FC = 32,     /* Configure the Mid GCI threshold for
                                           level 2 flow control. */
    bcmFabricLinkMidGciLvl3FC = 33,     /* Configure the Mid GCI threshold for
                                           level 3 flow control. */
    bcmFabricLinkMidRciLvl1FC = 34,     /* Configure the Mid RCI threshold for
                                           level 1 flow control. */
    bcmFabricLinkMidRciLvl2FC = 35,     /* Configure the Mid RCI threshold for
                                           level 2 flow control. */
    bcmFabricLinkMidRciLvl3FC = 36,     /* Configure the Mid RCI threshold for
                                           level 3 flow control. */
    bcmFabricLinkMidPrio0Drop = 37,     /* Configure the maximum threshold for
                                           DCM priorty 0 drops, above this
                                           threshold the dcm will drop cells
                                           recieved from DCH. */
    bcmFabricLinkMidPrio1Drop = 38,     /* Configure the maximum threshold for
                                           DCM priorty 1 drops, above this
                                           threshold the dcm will drop cells
                                           recieved from DCH. */
    bcmFabricLinkMidPrio2Drop = 39,     /* Configure the maximum threshold for
                                           DCM priorty 2 drops, above this
                                           threshold the dcm will drop cells
                                           recieved from DCH. */
    bcmFabricLinkMidPrio3Drop = 40,     /* Configure the maximum threshold for
                                           DCM priorty 3 drops, above this
                                           threshold the dcm will drop cells
                                           recieved from DCH. */
    bcmFabricLinkMidAlmostFull = 41,    /* Configure threshold for almost full,
                                           beyond this threshold, flow control
                                           will be sent back to the DCH. */
    bcmFabricLinkMidFifoSize = 42,      /* Configure the FIFO depth (size). */
    bcmFabricLinkTxRciLvl1FC = 43,      /* The TX FIFO threshold that initiates
                                           the Route Congestion Indication level
                                           1 Flow Control. */
    bcmFabricLinkTxRciLvl2FC = 44,      /* The TX FIFO threshold that initiates
                                           the Route Congestion Indication level
                                           2 Flow Control. */
    bcmFabricLinkTxRciLvl3FC = 45,      /* The TX FIFO threshold that initiates
                                           the Route Congestion Indication level
                                           3 Flow Control. */
    bcmFabricLinkTxAlmostFull = 46,     /* Configure threshold for almost full,
                                           beyond this threshold, flow control
                                           will be sent back to the DCM. */
    bcmFabricLinkTxFifoSize = 47,       /* Configure DCL FIFO depth (size). */
    bcmFabricLinkMidFull = 48           /* Configure threshold for max FIFO
                                           size, beyond this threshold, packets
                                           will be dropped. */
} bcm_fabric_link_threshold_type_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Add\Delete thresholds set */
extern int bcm_fabric_link_threshold_add(
    int unit, 
    uint32 flags, 
    int *fifo_type);

/* Add\Delete thresholds set */
extern int bcm_fabric_link_threshold_delete(
    int unit, 
    int fifo_type);

/* Set\Get fifo_type thresholds */
extern int bcm_fabric_link_thresholds_set(
    int unit, 
    int fifo_type, 
    uint32 count, 
    bcm_fabric_link_threshold_type_t *type, 
    int *value);

/* Set\Get fifo_type thresholds */
extern int bcm_fabric_link_thresholds_get(
    int unit, 
    int fifo_type, 
    uint32 count, 
    bcm_fabric_link_threshold_type_t *type, 
    int *value);

/* Set\Get fifo_type thresholds per pipe */
extern int bcm_fabric_link_thresholds_pipe_set(
    int unit, 
    int fifo_type, 
    bcm_fabric_pipe_t pipe, 
    uint32 flags, 
    uint32 count, 
    bcm_fabric_link_threshold_type_t *type, 
    int *value);

/* Set\Get fifo_type thresholds per pipe */
extern int bcm_fabric_link_thresholds_pipe_get(
    int unit, 
    int fifo_type, 
    bcm_fabric_pipe_t pipe, 
    uint32 flags, 
    uint32 count, 
    bcm_fabric_link_threshold_type_t *type, 
    int *value);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_FABRIC_LINK_TH_FE1_LINKS_ONLY   0x00000001 /* Only apply the
                                                          settings for the FE1
                                                          links. FE1 and FE3
                                                          links with the same
                                                          type, will have
                                                          different values */
#define BCM_FABRIC_LINK_TH_FE3_LINKS_ONLY   0x00000002 /* Only apply the
                                                          settings for the FE3
                                                          links. FE1 and FE3
                                                          links with the same
                                                          type, will have
                                                          different values */
#define BCM_FABRIC_LINK_TH_PRIM_ONLY        0x00000004 /* Only apply the
                                                          settings to the
                                                          Primary switching
                                                          context */
#define BCM_FABRIC_LINK_TH_SCND_ONLY        0x00000008 /* Only apply the
                                                          settings to the
                                                          Secondary switching
                                                          context */
#define BCM_FABRIC_LINK_THRESHOLD_WITH_ID   0x00000010 
#define BCM_FABRIC_LINK_THRESHOLD_RX_FIFO_ONLY 0x00000020 /* Only apply the
                                                          settings to the RX
                                                          links */
#define BCM_FABRIC_LINK_THRESHOLD_TX_FIFO_ONLY 0x00000040 /* Only apply the
                                                          settings to the TX
                                                          links */

#define BCM_FABRIC_LINK_STATUS_CRC_ERROR    _SHR_FABRIC_LINK_STATUS_CRC_ERROR /* Non-zero CRC rate */
#define BCM_FABRIC_LINK_STATUS_SIZE_ERROR   _SHR_FABRIC_LINK_STATUS_SIZE_ERROR /* Non-zero size
                                                          error-count */
#define BCM_FABRIC_LINK_STATUS_CODE_GROUP_ERROR _SHR_FABRIC_LINK_STATUS_CODE_GROUP_ERROR /* Non-zero code group
                                                          error-count */
#define BCM_FABRIC_LINK_STATUS_MISALIGN     _SHR_FABRIC_LINK_STATUS_MISALIGN /* Link down,
                                                          misalignment error */
#define BCM_FABRIC_LINK_STATUS_NO_SIG_LOCK  _SHR_FABRIC_LINK_STATUS_NO_SIG_LOCK /* Link down, SerDes
                                                          signal lock error */
#define BCM_FABRIC_LINK_STATUS_NO_SIG_ACCEP _SHR_FABRIC_LINK_STATUS_NO_SIG_ACCEP /* Link up, but not
                                                          accepting reachability
                                                          cells */
#define BCM_FABRIC_LINK_STATUS_ERRORED_TOKENS _SHR_FABRIC_LINK_STATUS_ERRORED_TOKENS /* Low value, indicates
                                                          bad link connectivity
                                                          or link down, based on
                                                          reachability cells */

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Attach list of links to a thresholds set (fifo_type) \br
 * Retrieve list of links attached with a thresholds set (fifo_type)
 */
extern int bcm_fabric_link_thresholds_attach(
    int unit, 
    int fifo_type, 
    uint32 links_count, 
    bcm_port_t *links);

/* 
 * Attach list of links to a thresholds set (fifo_type) \br
 * Retrieve list of links attached with a thresholds set (fifo_type)
 */
extern int bcm_fabric_link_thresholds_retrieve(
    int unit, 
    int fifo_type, 
    uint32 links_count_max, 
    bcm_port_t *links, 
    uint32 *links_count);

/* Get the status of the link */
extern int bcm_fabric_link_status_get(
    int unit, 
    bcm_port_t link_id, 
    uint32 *link_status, 
    uint32 *errored_token_count);

#endif /* BCM_HIDE_DISPATCHABLE */

typedef enum bcm_fabric_device_type_e {
    bcmFabricDeviceTypeUnknown = 0, /* Unknown device type */
    bcmFabricDeviceTypeFE13 = 1,    /* FE13 device type */
    bcmFabricDeviceTypeFE2 = 2,     /* FE2 device type */
    bcmFabricDeviceTypeFAP = 3,     /* FAP device type */
    bcmFabricDeviceTypeFE1 = 4,     /* FE1 device type */
    bcmFabricDeviceTypeFE3 = 5,     /* FE3 device type */
    bcmFabricDeviceTypeFIP = 6,     /* FIP device type */
    bcmFabricDeviceTypeFOP = 7      /* FOP device type */
} bcm_fabric_device_type_t;

typedef struct bcm_fabric_link_connectivity_s {
    uint32 module_id;                   /* remote module ID. */
    uint32 link_id;                     /* remote link id. The ID of the link
                                           partner in the remote module.
                                           BCM_FABRIC_LINK_NO_CONNECTIVITY: The
                                           connection is invalid */
    bcm_fabric_device_type_t device_type; /* remote device type */
} bcm_fabric_link_connectivity_t;

#define BCM_FABRIC_LINK_NO_CONNECTIVITY _SHR_FABRIC_LINK_NO_CONNECTIVITY /* FABRIC_LINK_NO_CONNECTIVITY */

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Retrieves the current link-partner information of a link for all
 * existing links
 */
extern int bcm_fabric_link_connectivity_status_get(
    int unit, 
    int link_partner_max, 
    bcm_fabric_link_connectivity_t *link_partner_array, 
    int *link_partner_count);

/* Retrieves the current link-partner information of a single link. */
extern int bcm_fabric_link_connectivity_status_single_get(
    int unit, 
    bcm_port_t link_id, 
    bcm_fabric_link_connectivity_t *link_partner_info);

/* Retrieves the links through which a remote module ID is reachable */
extern int bcm_fabric_reachability_status_get(
    int unit, 
    int moduleid, 
    int links_max, 
    uint32 *links_array, 
    int *links_count);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bandwidth profile configuration */
typedef struct bcm_fabric_bandwidth_profile_s {
    int num_links;      /* number of links/serdes */
    int rci;            /* route congestion indication */
    uint32 max_kbps;    /* maximum bandwidth (in kbps) */
} bcm_fabric_bandwidth_profile_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set retrieve fabric bandwidth profile on unit level */
extern int bcm_fabric_bandwidth_profile_set(
    int unit, 
    int profile_count, 
    bcm_fabric_bandwidth_profile_t *profile_array);

/* Set retrieve fabric bandwidth profile on unit level */
extern int bcm_fabric_bandwidth_profile_get(
    int unit, 
    int profile_count, 
    bcm_fabric_bandwidth_profile_t *profile_array);

/* Set retrieve fabric bandwidth profile per core */
extern int bcm_fabric_bandwidth_core_profile_set(
    int unit, 
    int core, 
    uint32 flags, 
    int profile_count, 
    bcm_fabric_bandwidth_profile_t *profile_array);

/* Set retrieve fabric bandwidth profile per core */
extern int bcm_fabric_bandwidth_core_profile_get(
    int unit, 
    int core, 
    uint32 flags, 
    int profile_count, 
    bcm_fabric_bandwidth_profile_t *profile_array);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_FABRIC_TDM_USER_FIELD_MAX_SIZE  6          /* Maximum size of user
                                                          field */

/* TDM editing flags */
#define BCM_FABRIC_TDM_EDITING_INGRESS      0x0001     /* Ingress direction */
#define BCM_FABRIC_TDM_EDITING_EGRESS       0x0002     /* Egress Direction */
#define BCM_FABRIC_TDM_EDITING_UNICAST      0x0004     /* Unicast */
#define BCM_FABRIC_TDM_EDITING_MULTICAST    0x0008     /* Multicast */
#define BCM_FABRIC_TDM_EDITING_NON_SYMMETRIC_CRC 0x0010     /* asymmetric CRC,
                                                          add/remove CRC based
                                                          on ingress/egress
                                                          flags */

/* bcm_fabric_tdm_editing_type_t */
typedef enum bcm_fabric_tdm_editing_type_e {
    bcmFabricTdmEditingAppend = 0,      /* append internal forwarding header
                                           (FTMH) to all received TDM packets.
                                           Configuration applied to ingress
                                           port. If set destination forwarding
                                           must be configured (egress port). */
    bcmFabricTdmEditingRemove = 1,      /* remove forwarding header to all the
                                           transmitted packets at this egress
                                           port. */
    bcmFabricTdmEditingNoChange = 2,    /* do not change the forwarding header.
                                           For ingress the forwarding header is
                                           not changed (forwarding header added
                                           by upstream device). For egress
                                           forwarding header not changed (i.e
                                           not changed/removed) and is passed to
                                           downstream device. */
    bcmFabricTdmEditingCustomExternal = 3 /* External custom header. In ingress
                                           direction embed external custom
                                           header in the added forwading header.
                                           Embedded fields will replace user
                                           specified fields. In the egress
                                           direction, extract  external custom
                                           overhead from user specified fields
                                           of the removed forwarding header. */
} bcm_fabric_tdm_editing_type_t;

/* tdm editing configuration */
typedef struct bcm_fabric_tdm_editing_s {
    uint32 flags;                       /* flag values. Valid for Ingress and
                                           Egress configuration */
    bcm_fabric_tdm_editing_type_t type; /* editing type. Valid for Ingress and
                                           Egress configuration */
    bcm_gport_t destination_port;       /* destination port. Valid for Ingress
                                           unicast configuration.
                                           (BCM_FABRIC_TDM_EDITING_UNICAST and
                                           TDM_EDITING_INGRESS flag has to be
                                           specified. Also reference type
                                           field). */
    bcm_multicast_t multicast_id;       /* Multicast Id. Valid for Ingress
                                           multicast configuration.
                                           (BCM_FABRIC_TDM_EDITING_MULTICAST and
                                           BCM_FABRIC_TDM_EDITING_INGRESS flag
                                           has to be specified. Also reference
                                           type field. */
    uint8 user_field[BCM_FABRIC_TDM_USER_FIELD_MAX_SIZE]; /* user field values. Valid for Ingress
                                           configuration.
                                           (BCM_FABRIC_TDM_EDITING_INGRESS flag
                                           has to be specified) */
    int user_field_count;               /* count of total number of bits. Valid
                                           for Ingress configuration
                                           (BCM_FABRIC_TDM_EDITING_INGRESS flag
                                           has to be specified) */
    int add_packet_crc;                 /* TRUE =>add CRC, FALSE => do not add
                                           CRC */
} bcm_fabric_tdm_editing_t;

/* Initialize  bcm_fabric_tdm_editing_t to a 'safe' default value. */
extern void bcm_fabric_tdm_editing_t_init(
    bcm_fabric_tdm_editing_t *editing);

#ifndef BCM_HIDE_DISPATCHABLE

/* TDM configuration */
extern int bcm_fabric_tdm_editing_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_fabric_tdm_editing_t *editing);

/* TDM configuration */
extern int bcm_fabric_tdm_editing_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_fabric_tdm_editing_t *editing);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_FABRIC_TDM_MAX_LINKS    64         /* maximum fabric links */

/* tdm editing configuration */
typedef struct bcm_fabric_tdm_direct_routing_s {
    int links_count;                    /* number of links. setting a link count
                                           of zero implies no direct routing */
    bcm_gport_t links[BCM_FABRIC_TDM_MAX_LINKS]; /* link specification */
} bcm_fabric_tdm_direct_routing_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* TDM routing configuration */
extern int bcm_fabric_tdm_direct_routing_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_fabric_tdm_direct_routing_t *routing_info);

/* TDM routing configuration */
extern int bcm_fabric_tdm_direct_routing_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_fabric_tdm_direct_routing_t *routing_info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Discard aging flags */
#define BCM_FABRIC_CONFIG_DISCARD_AGING_ACTION_ONLY 0x0001     /* If set, only aging can
                                                          set discard aging.
                                                          Otherwise, also other
                                                          mechanizems. */

/* bcm_fabric_age_mode_t */
typedef enum bcm_fabric_age_mode_e {
    bcmFabricAgeModeReset = 0,      /* size is reset to 0 */
    bcmFabricAgeModeDecrement = 1   /* size is decremented by 1 */
} bcm_fabric_age_mode_t;

/* bcm_fabric_module_control_t */
typedef enum bcm_fabric_module_control_e {
    bcmFabricModuleControlFSMSeqNumberEnable = 0, /* Fabric Status Message sequence
                                           numbers mechanism enable/disable */
    bcmFabricModuleControlMcTopologySwControlEnable = 1 
} bcm_fabric_module_control_t;

/* Aging configuration */
typedef struct bcm_fabric_config_discard_s {
    uint32 flags;                   /* BCM_FABRIC_CONFIG_DISCARD_XXX flags */
    int enable;                     /* Enable Aging period functionality */
    int age;                        /* Unit in millisecond. Every 'age' outgoing
                                       local port size is monitored. If it was
                                       not updated during parameter age, it is
                                       updated according to age_mode */
    bcm_fabric_age_mode_t age_mode; /* Defines the action if outgoing local port
                                       size was not updated by VoQs after
                                       aging_period */
} bcm_fabric_config_discard_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Set discard aging configuration. Configuration to enable, disable
 * aging and its settings.
 */
extern int bcm_fabric_config_discard_set(
    int unit, 
    bcm_fabric_config_discard_t *discard);

/* 
 * Set discard aging configuration. Configuration to enable, disable
 * aging and its settings.
 */
extern int bcm_fabric_config_discard_get(
    int unit, 
    bcm_fabric_config_discard_t *discard);

/* Per module configuration */
extern int bcm_fabric_module_control_set(
    int unit, 
    uint32 flags, 
    bcm_module_t modid, 
    bcm_fabric_module_control_t control, 
    int value);

/* Per module configuration */
extern int bcm_fabric_module_control_get(
    int unit, 
    uint32 flags, 
    bcm_module_t modid, 
    bcm_fabric_module_control_t control, 
    int *value);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Credit watchdog control, allowed values to:
 * bcm_fabric_control_set(unit, bcmFabricWatchdogQueueEnable, value)
 */
#define BCM_FABRIC_WATCHDOG_QUEUE_DISABLE   0          /* credit watchdog will
                                                          be disabled */
#define BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_NORMAL 1          /* credit watchdog will
                                                          be enabled in normal
                                                          mode */
#define BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_FAST_STATUS_MESSAGE 2          /* credit watchdog will
                                                          be enabled in
                                                          aggressive status
                                                          message mode */
#define BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_COMMON_STATUS_MESSAGE 3          /* credit watchdog will
                                                          be enabled in common
                                                          status message mode */
#define BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_MIN BCM_FABRIC_WATCHDOG_QUEUE_DISABLE 
#define BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_MAX BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_COMMON_STATUS_MESSAGE 

/* bcm_fabric_priority_* flags */
#define BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY _SHR_FABRIC_QUEUE_PRIORITY_HIGH_ONLY 
#define BCM_FABRIC_QUEUE_PRIORITY_LOW_ONLY  _SHR_FABRIC_QUEUE_PRIORITY_LOW_ONLY 
#define BCM_FABRIC_PRIORITY_MULTICAST       _SHR_FABRIC_PRIORITY_MULTICAST 
#define BCM_FABRIC_PRIORITY_TDM             _SHR_FABRIC_PRIORITY_TDM 
#define BCM_FABRIC_PRIORITY_OCB_MIX_ONLY    _SHR_FABRIC_PRIORITY_OCB_MIX_ONLY 
#define BCM_FABRIC_PRIORITY_OCB_ONLY_ONLY   _SHR_FABRIC_PRIORITY_OCB_ONLY_ONLY 

#ifndef BCM_HIDE_DISPATCHABLE

/* Allow configuration of the fabric cell priority */
extern int bcm_fabric_priority_set(
    int unit, 
    uint32 flags, 
    bcm_cos_t ingress_pri, 
    bcm_color_t color, 
    int fabric_priority);

/* Getting fabric cell priority configuration. */
extern int bcm_fabric_priority_get(
    int unit, 
    uint32 flags, 
    bcm_cos_t ingress_pri, 
    bcm_color_t color, 
    int *fabric_priority);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Fabric route */
typedef struct bcm_fabric_route_s {
    uint32 pipe_id;     /* Origin fabric pipe */
    int number_of_hops; /* Corresponds to the number of routing hops (number
                           traversed links) */
    int *hop_ids;       /* Traversed links */
} bcm_fabric_route_t;

/* Initialize a bcm_fabric_route_t to a 'safe' default value. */
extern void bcm_fabric_route_t_init(
    bcm_fabric_route_t *fabric_route);

#ifndef BCM_HIDE_DISPATCHABLE

/* Receive fabric route cells */
extern int bcm_fabric_route_rx(
    int unit, 
    uint32 flags, 
    uint32 data_out_max_size, 
    uint32 *data_out, 
    uint32 *data_out_size);

/* Send fabric route cells */
extern int bcm_fabric_route_tx(
    int unit, 
    uint32 flags, 
    bcm_fabric_route_t *route, 
    uint32 data_in_size, 
    uint32 *data_in);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Packet Cell Packing (pcp) mode. */
typedef enum bcm_fabric_pcp_mode_e {
    bcmFabricPcpModeNoPacking = 0,  /* No packing */
    bcmFabricPcpModeSimple = 1,     /* Simple Packing */
    bcmFabricPcpModeContinuous = 2  /* Continuous Packing */
} bcm_fabric_pcp_mode_t;

/* Fabric pcp mode configuration */
typedef struct bcm_fabric_pcp_mode_config_s {
    bcm_fabric_pcp_mode_t pcp_mode; /* Mode of Packet Cell Packing (PCP) */
} bcm_fabric_pcp_mode_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/Get PCP mode per VOQ/destination device. */
extern int bcm_fabric_pcp_dest_mode_config_set(
    int unit, 
    uint32 flags, 
    bcm_module_t modid, 
    bcm_fabric_pcp_mode_config_t *pcp_config);

/* Set/Get PCP mode per VOQ/destination device. */
extern int bcm_fabric_pcp_dest_mode_config_get(
    int unit, 
    uint32 flags, 
    bcm_module_t modid, 
    bcm_fabric_pcp_mode_config_t *pcp_config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Configure all-reachable minimum number of links. */
#define BCM_FABRIC_DESTINATION_LINK_MIN_ALL_REACHABLE 1          

#ifndef BCM_HIDE_DISPATCHABLE

/* Configure minimum number of links per destination device. */
extern int bcm_fabric_destination_link_min_set(
    int unit, 
    uint32 flags, 
    bcm_module_t module_id, 
    int num_of_links);

/* Configure minimum number of links per destination device. */
extern int bcm_fabric_destination_link_min_get(
    int unit, 
    uint32 flags, 
    bcm_module_t module_id, 
    int *num_of_links);

#endif /* BCM_HIDE_DISPATCHABLE */

/* severity counter to indicate severity level thresholds. */
#define BCM_FABRIC_NUM_OF_RCI_LEVELS    7          

/* RCI indication level factor. */
#define BCM_FABRIC_NUM_OF_RCI_SEVERITIES    3          

/* Fabric RCI configuration */
typedef struct bcm_fabric_rci_config_s {
    uint32 rci_core_level_thresholds[BCM_FABRIC_NUM_OF_RCI_LEVELS]; /* thresholds for severity counter of
                                           RCI - per core */
    uint32 rci_device_level_thresholds[BCM_FABRIC_NUM_OF_RCI_LEVELS]; /* thresholds for severity counter of
                                           RCI - per device */
    uint32 rci_severity_factors[BCM_FABRIC_NUM_OF_RCI_SEVERITIES]; /* RCI severity level factor */
    uint32 rci_high_score_fabric_rx_queue; /* RCI score of fabric FIFO */
    uint32 rci_high_score_fabric_rx_local_queue; /* RCI score of fabric local FIFO */
    uint32 rci_threshold_num_of_congested_links; /* number of congested links to be used
                                           as treshold */
    uint32 rci_high_score_congested_links; /* high score to be used when number of
                                           congested links reaches threshold */
} bcm_fabric_rci_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* configure RCI related information */
extern int bcm_fabric_rci_config_set(
    int unit, 
    bcm_fabric_rci_config_t rci_config);

/* configure RCI related information */
extern int bcm_fabric_rci_config_get(
    int unit, 
    bcm_fabric_rci_config_t *rci_config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a bcm_fabric_rci_config_t to a 'safe' default value. */
extern void bcm_fabric_rci_config_t_init(
    bcm_fabric_rci_config_t *rci_config);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set the replications in the fabric element to external faps */
extern int bcm_fabric_static_replication_set(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    uint32 destid_count, 
    bcm_module_t *destid_array);

#endif /* BCM_HIDE_DISPATCHABLE */

/* fabric_threshold_type */
typedef enum bcm_fabric_threshold_type_e {
    bcmFabricMidTagDropClassTh = _SHR_FABRIC_MID_TAG_DROP_CLASS_TH, /* Ingress Admission Thresholds */
    bcmFabricTxLinkLoadDropPipeTh = _SHR_FABRIC_TX_LINK_LOAD_DROP_PIPE_TH, 
    bcmFabricTxFragGuaranteedPipeTh = _SHR_FABRIC_TX_FRAG_GUARANTEED_PIPE_TH, 
    bcmFabricSharedDropClassTh = _SHR_FABRIC_SHARED_DROP_CLASS_TH, 
    bcmFabricSharedMcCopiesDropPrioTh = _SHR_FABRIC_SHARED_MC_COPIES_DROP_PRIO_TH, 
    bcmFabricMidFragDropClassTh = _SHR_FABRIC_MID_FRAG_DROP_CLASS_TH, 
    bcmFabricSharedBankMcDropPrioTh = _SHR_FABRIC_SHARED_BANK_MC_DROP_PRIO_TH, /* DTL Admission Thresholds */
    bcmFabricTxTagDropClassTh = _SHR_FABRIC_TX_TAG_DROP_CLASS_TH, 
    bcmFabricTxFragDropClassTh = _SHR_FABRIC_TX_FRAG_DROP_CLASS_TH, 
    bcmFabricTxTagGciPipeTh = _SHR_FABRIC_TX_TAG_GCI_PIPE_TH, /* GCI Generation */
    bcmFabricTxFragGciPipeTh = _SHR_FABRIC_TX_FRAG_GCI_PIPE_TH, 
    bcmFabricMidTagGciPipeTh = _SHR_FABRIC_MID_TAG_GCI_PIPE_TH, 
    bcmFabricMidFragGciPipeTh = _SHR_FABRIC_MID_FRAG_GCI_PIPE_TH, 
    bcmFabricSharedGciPipeTh = _SHR_FABRIC_SHARED_GCI_PIPE_TH, 
    bcmFabricSharedFragCopiesGciPipeTh = _SHR_FABRIC_SHARED_FRAG_COPIES_GCI_PIPE_TH, 
    bcmFabricTxFragGuaranteedRciPipeTh = _SHR_FABRIC_TX_FRAG_GUARANTEED_RCI_PIPE_TH, /* RCI Generation */
    bcmFabricTxLinkLoadRciPipeTh = _SHR_FABRIC_TX_LINK_LOAD_RCI_PIPE_TH, 
    bcmFabricTxTagRciPipeTh = _SHR_FABRIC_TX_TAG_RCI_PIPE_TH, 
    bcmFabricTxFragRciPipeTh = _SHR_FABRIC_TX_FRAG_RCI_PIPE_TH, 
    bcmFabricMidTagRciPipeTh = _SHR_FABRIC_MID_TAG_RCI_PIPE_TH, 
    bcmFabricMidFragRciPipeTh = _SHR_FABRIC_MID_FRAG_RCI_PIPE_TH, 
    bcmFabricSharedRciPipeTh = _SHR_FABRIC_SHARED_RCI_PIPE_TH, 
    bcmFabricMidMcCopiesFcPrioTh = _SHR_FABRIC_MID_MC_COPIES_FC_PRIO_TH, /* FC Generation */
    bcmFabricMidTagFcPipeTh = _SHR_FABRIC_MID_TAG_FC_PIPE_TH, 
    bcmFabricMidFragFcPipeTh = _SHR_FABRIC_MID_FRAG_FC_PIPE_TH, 
    bcmFabricSharedFcPipeTh = _SHR_FABRIC_SHARED_FC_PIPE_TH, 
    bcmFabricTxTagFcPipeTh = _SHR_FABRIC_TX_TAG_FC_PIPE_TH, 
    bcmFabricTxFragFcPipeTh = _SHR_FABRIC_TX_FRAG_FC_PIPE_TH, 
    bcmFabricMidMaskFcPipeTh = _SHR_FABRIC_MID_MASK_FC_PIPE_TH, 
    bcmFabricTxFE1BypassLLFCFcPipeTh = _SHR_FABRIC_TX_FE1_BYPASS_LLFCFC_PIPE_TH, 
    bcmFabricRxFragDropPipeTh = _SHR_FABRIC_RX_FRAG_DROP_PIPE_TH, /* DCH Legacy Thresholds */
    bcmFabricRxMcLowPrioDropPipeTh = _SHR_FABRIC_RX_MC_LOW_PRIO_DROP_PIPE_TH, 
    bcmFabricRxFragGciPipeTh = _SHR_FABRIC_RX_FRAG_GCI_PIPE_TH, 
    bcmFabricRxLLFCFcPipeTh = _SHR_FABRIC_RX_LLFCFC_PIPE_TH, 
    bcmFabricRxWfqDynamicWeightPipeTh = _SHR_FABRIC_RX_WFQ_DYNAMIC_WEIGHT_PIPE_TH, /* Dynamic weights */
    bcmFabricMidWfqDynamicWeightPipeTh = _SHR_FABRIC_MID_WFQ_DYNAMIC_WEIGHT_PIPE_TH, 
    bcmFabricTxWfqDynamicWeightPipeTh = _SHR_FABRIC_TX_WFQ_DYNAMIC_WEIGHT_PIPE_TH 
} bcm_fabric_threshold_type_t;

/* Fabric threshold ID */
typedef uint32 bcm_fabric_threshold_id_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Configure all fabric flow control and drop thresholds. */
extern int bcm_fabric_profile_threshold_set(
    int unit, 
    int profile_id, 
    bcm_fabric_threshold_id_t threshold_id, 
    bcm_fabric_threshold_type_t threshold_type, 
    uint32 flags, 
    int value);

/* Configure all fabric flow control and drop thresholds. */
extern int bcm_fabric_profile_threshold_get(
    int unit, 
    int profile_id, 
    bcm_fabric_threshold_id_t threshold_id, 
    bcm_fabric_threshold_type_t threshold_type, 
    uint32 flags, 
    int *value);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Threshold indication flags for modifying only special sets of links. */
#define BCM_FABRIC_LINK_TH_LR_ONLY  _BCM_FABRIC_LINK_TH_LR_ONLY 
#define BCM_FABRIC_LINK_TH_NLR_ONLY _BCM_FABRIC_LINK_TH_NLR_ONLY 

/* Threshold special selectors for all priorities and all levels. */
#define BCM_FABRIC_PRIO_ALL     -1         
#define BCM_FABRIC_LEVEL_ALL    -1         

#define BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(pipe, level)  _SHR_FABRIC_TH_INDEX_PIPE_LEVEL_SET(pipe, level) 
#define BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(cast, prio)  _SHR_FABRIC_TH_INDEX_CAST_PRIO_SET(cast, prio) 
#define BCM_FABRIC_TH_INDEX_FMC_SLOW_START_PHASE_SET(fmc_shaper, slow_start_phase)  _SHR_FABRIC_TH_INDEX_FMC_SLOW_START_PHASE_SET(fmc_shaper, slow_start_phase) 

#ifndef BCM_HIDE_DISPATCHABLE

/* The following API associate links with profile (0,1). */
extern int bcm_fabric_link_profile_set(
    int unit, 
    int profile_id, 
    uint32 flags, 
    uint32 links_count, 
    bcm_port_t *links);

/* The following API retrieves the threshold profile for given links. */
extern int bcm_fabric_link_profile_get(
    int unit, 
    int profile_id, 
    uint32 flags, 
    uint32 links_count_max, 
    uint32 *links_count, 
    bcm_port_t *links);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Selector for configuring all pipes */
#define BCM_FABRIC_PIPE_ALL     _SHR_FABRIC_PIPE_ALL 

#ifndef BCM_HIDE_DISPATCHABLE

/* Enables MC local route for multiple MC groups. */
extern int bcm_fabric_multicast_local_route_set(
    int unit, 
    uint32 flags, 
    uint32 nof_groups, 
    bcm_multicast_t *groups, 
    int *enable);

/* Get MC local route state for multiple MC groups. */
extern int bcm_fabric_multicast_local_route_get(
    int unit, 
    uint32 flags, 
    uint32 nof_groups, 
    bcm_multicast_t *groups, 
    int *enable);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Fabric cell header field types. */
typedef enum bcm_fabric_cell_header_field_type_e {
    bcmFabricCellHeaderFieldTypeSourceDevice = _shr_dnxc_fabric_cell_header_field_type_source_device, /* Fabric cell header field type
                                           fip/source device unit ID. */
    bcmFabricCellHeaderFieldTypeFIPLink = _shr_dnxc_fabric_cell_header_field_type_fip_link, /* Fabric cell header field type fip
                                           link. */
    bcmFabricCellHeaderFieldTypePipe = _shr_dnxc_fabric_cell_header_field_type_pipe, /* Fabric cell header field type pipe. */
    bcmFabricCellHeaderFieldTypeNof = _shr_dnxc_fabric_cell_header_field_type_nof /* Number of fabric cell header field
                                           types exposed. */
} bcm_fabric_cell_header_field_type_t;

/* Allowed flags for bcm_fabric_route_rx API */
#define BCM_FABRIC_CELL_RX_HEADER_PREPEND   _SHR_FABRIC_CELL_RX_HEADER_PREPEND 

/* Fabric topology type. */
typedef enum bcm_fabric_topology_type_e {
    bcmFabricTopologyLB = 0,    /* Load balancing topology. */
    bcmFabricTopologyMC = 1,    /* Multicast topology. */
    bcmFabricTopologyAll = 2    /* All. */
} bcm_fabric_topology_type_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Configure links topology. */
extern int bcm_fabric_link_topology_config_set(
    int unit, 
    uint32 flags, 
    bcm_module_t destination, 
    bcm_fabric_topology_type_t type, 
    int links_count, 
    bcm_port_t *links_array);

/* Get the configured links topology. */
extern int bcm_fabric_link_topology_config_get(
    int unit, 
    uint32 flags, 
    bcm_module_t destination, 
    bcm_fabric_topology_type_t type, 
    int links_count_max, 
    int *links_count, 
    bcm_port_t *links_array);

#endif /* BCM_HIDE_DISPATCHABLE */

typedef struct bcm_fabric_rci_resolution_key_s {
    int pipe; 
    uint32 shared_rci; 
    uint32 guaranteed_rci; 
} bcm_fabric_rci_resolution_key_t;

typedef struct bcm_fabric_rci_resolution_config_s {
    uint32 resolved_rci; 
} bcm_fabric_rci_resolution_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

extern int bcm_fabric_rci_resolution_set(
    int unit, 
    uint32 flags, 
    bcm_fabric_rci_resolution_key_t *key, 
    bcm_fabric_rci_resolution_config_t *config);

extern int bcm_fabric_rci_resolution_get(
    int unit, 
    uint32 flags, 
    bcm_fabric_rci_resolution_key_t *key, 
    bcm_fabric_rci_resolution_config_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* RCI throttling mode */
typedef enum bcm_fabric_control_rci_throttling_mode_e {
    bcmFabricControlRciThrottlingModeGlobal = 0, /* Global(default) mode with no Biasing */
    bcmFabricControlRciThrottlingModeDeviceBiasing = 1, /* Device-Biasing mode */
    bcmFabricControlRciThrottlingModeFlowBiasing = 2 /* Flow-Biasing mode */
} bcm_fabric_control_rci_throttling_mode_t;

/* RCI throttling probability mode */
typedef enum bcm_fabric_rci_biasing_probability_mode_e {
    bcmFabricRciDevBiasingModeDiscard = 0, /* Device-Biasing Discard probability */
    bcmFabricRciDevBiasingModeReplicate = 1, /* Device-Biasing Replicate probability */
    bcmFabricRciFlowBiasingModeUninstall = 2 /* Flow-Biasing Uninstall probability */
} bcm_fabric_rci_biasing_probability_mode_t;

/* Hold the RCI throttling profile info */
typedef struct bcm_fabric_rci_biasing_probability_info_s {
    bcm_gport_t gport;                  /* Target gport, for core and future
                                           use. */
    int is_remote;                      /* For local or fabric (remote) flows. */
    int is_high_prio;                   /* For high or low priority flows. */
    bcm_fabric_rci_biasing_probability_mode_t probability_type; /* Probability type see . */
} bcm_fabric_rci_biasing_probability_info_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* API for managing RCI mechanism probabilities info */
extern int bcm_fabric_rci_biasing_probability_set(
    int unit, 
    uint32 flags, 
    bcm_fabric_rci_biasing_probability_info_t *probability_info, 
    int probability[BCM_FABRIC_NUM_OF_RCI_LEVELS]);

/* API for managing RCI mechanism probabilities info */
extern int bcm_fabric_rci_biasing_probability_get(
    int unit, 
    uint32 flags, 
    bcm_fabric_rci_biasing_probability_info_t *probability_info, 
    int probability[BCM_FABRIC_NUM_OF_RCI_LEVELS]);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * LCI shaper modes, allowed values to: bcm_fabric_cgm_control_set(unit,
 * flags, bcmFabricCgmLciShaperMode, control_id, value)
 */
#define BCM_FABRIC_CGM_LCI_SHAPER_MODE_DYNAMIC 0          
#define BCM_FABRIC_CGM_LCI_SHAPER_MODE_STATIC 1          

/* Fabric CGM controls types */
typedef enum bcm_fabric_cgm_control_type_e {
    bcmFabricCgmRciLinkPipeLevelTh = 0, /* RCI controls */
    bcmFabricCgmRciEgressPipeLevelTh = 1, 
    bcmFabricCgmRciTotalEgressPipeLevelTh = 2, 
    bcmFabricCgmRciLocalLevelTh = 3, 
    bcmFabricCgmRciTotalLocalLevelTh = 4, 
    bcmFabricCgmRciLinkLevelFactor = 5, 
    bcmFabricCgmRciEgressLevelFactor = 6, 
    bcmFabricCgmRciHighSeverityMinLinksParam = 7, 
    bcmFabricCgmRciHighSeverityFactor = 8, 
    bcmFabricCgmRciCoreLevelMappingTh = 9, 
    bcmFabricCgmRciDeviceLevelMappingTh = 10, 
    bcmFabricCgmGciLeakyBucketEn = 11, 
    bcmFabricCgmGciLeakyBucketSizeLeakyBucketTh = 12, 
    bcmFabricCgmGciLeakyBucketFCLeakyBucketTh = 13, 
    bcmFabricCgmGciBackoffEn = 14, 
    bcmFabricCgmGciBackoffLevelTh = 15, 
    bcmFabricCgmSlowStartEn = 16, 
    bcmFabricCgmSlowStartRateShaperPhaseParam = 17, 
    bcmFabricCgmLlfcPipeTh = 18, 
    bcmFabricCgmDropFabricClassTh = 19, 
    bcmFabricCgmDropMeshMcPriorityTh = 20, 
    bcmFabricCgmEgressDropTdmLatencyEn = 21, 
    bcmFabricCgmEgressDropTdmLatencyTh = 22, 
    bcmFabricCgmEgressDropMcLowPrioEn = 23, 
    bcmFabricCgmEgressDropMcLowPrioTh = 24, 
    bcmFabricCgmEgressDropMcLowPrioSelect = 25, 
    bcmFabricCgmFcIngressTh = 26, 
    bcmFabricCgmEgressDynamicWfqTh = 27, 
    bcmFabricCgmLciShaperMode = 28, 
    bcmFabricCgmLciShaperStaticRateInGbps = 29, 
    bcmFabricCgmNof = 30 
} bcm_fabric_cgm_control_type_t;

/* Fabric CGM control index */
typedef uint32 bcm_fabric_cgm_control_id_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Configure Fabric CGM. */
extern int bcm_fabric_cgm_control_set(
    int unit, 
    uint32 flags, 
    bcm_fabric_cgm_control_type_t control_type, 
    bcm_fabric_cgm_control_id_t control_id, 
    int value);

/* Configure Fabric CGM. */
extern int bcm_fabric_cgm_control_get(
    int unit, 
    uint32 flags, 
    bcm_fabric_cgm_control_type_t control_type, 
    bcm_fabric_cgm_control_id_t control_id, 
    int *value);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_FABRIC_STAT_INDEX_DEVICE_SET    _SHR_FABRIC_STAT_INDEX_DEVICE_SET() 
#define BCM_FABRIC_STAT_INDEX_LINK_SET(link)  _SHR_FABRIC_STAT_INDEX_LINK_SET(link) 
#define BCM_FABRIC_STAT_INDEX_LINK_PIPE_SET(link, pipe)  _SHR_FABRIC_STAT_INDEX_LINK_PIPE_SET(link,pipe) 

/* Fabric stat counter types */
typedef enum bcm_fabric_stat_e {
    bcmFabricStatDeviceReachDrop = 0,   /* Device Reachability drop counter */
    bcmFabricStatDeviceGlobalDrop = 1,  /* Device Global drop counter */
    bcmFabricStatQueueLinkPipeCurrOccupancyBytes = 2, /* Current occupancy, per link and pipe
                                           in bytes */
    bcmFabricStatQueueLinkMaxWmkLevel = 3, /* RCI watermark level per link */
    bcmFabricStatDeviceRciWmkLvl = 4,   /* Device Max RCI level */
    bcmFabricStatNof = 5 
} bcm_fabric_stat_t;

/* Fabric stat index */
typedef uint32 bcm_fabric_stat_index_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get SAI Fabric counters. */
extern int bcm_fabric_stat_get(
    int unit, 
    uint32 flags, 
    bcm_fabric_stat_index_t index, 
    bcm_fabric_stat_t stat, 
    uint64 *value);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_FABRIC_H__ */
