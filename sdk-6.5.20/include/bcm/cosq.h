/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_COSQ_H__
#define __BCM_COSQ_H__

#include <bcm/types.h>
#include <bcm/fabric.h>
#include <bcm/field.h>
#include <bcm/switch.h>
#include <shared/port.h>

#ifndef BCM_HIDE_DISPATCHABLE

/* Initialize the CoSQ subsystem. */
extern int bcm_cosq_init(
    int unit);

/* De-initialize the CoSQ subsystem. */
extern int bcm_cosq_detach(
    int unit);

/* Configure the number of Class of Service Queues (COSQs). */
extern int bcm_cosq_config_set(
    int unit, 
    int numq);

/* Configure the number of Class of Service Queues (COSQs). */
extern int bcm_cosq_config_get(
    int unit, 
    int *numq);

/* Get or set the mapping from internal priority to CoS queue. */
extern int bcm_cosq_mapping_set(
    int unit, 
    bcm_cos_t priority, 
    bcm_cos_queue_t cosq);

/* Get or set the mapping from internal priority to CoS queue. */
extern int bcm_cosq_mapping_get(
    int unit, 
    bcm_cos_t priority, 
    bcm_cos_queue_t *cosq);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 1st dimension. */
typedef struct bcm_cosq_fadt_threshold_s {
    uint32 thresh_min;          /* minimum value of the FADT threshold */
    uint32 thresh_max;          /* maximum value of the FADT threshold */
    int alpha;                  /* alpha parameter of the FADT threshold */
    uint32 resource_range_min;  /* minimum limit of resource range */
    uint32 resource_range_max;  /* maximum limit of resource range */
} bcm_cosq_fadt_threshold_t;

/* Config parameters for Priority flow control */
typedef struct bcm_cosq_pfc_config_s {
    int xoff_threshold;                 /* Assert backpressure when number of
                                           buffers used is at or above this
                                           threshold. */
    int xon_threshold;                  /* Remove backpressure when number of
                                           buffers used drops below this
                                           threshold. */
    bcm_cosq_fadt_threshold_t xoff_fadt_threshold; /* Assert backpressure when number of
                                           resources used is at or above this
                                           FADT threshold. */
    int xon_fadt_offset;                /* Remove backpressure when number of
                                           resources used drops below FADT
                                           threshold defined by
                                           xoff_fadt_threshold - fadt_offset. */
    int xon_fadt_floor;                 /* Remove backpressure in FADT mode when
                                           number of resources used drops below
                                           this threshold. */
    int xoff_threshold_bd;              /* Assert backpressure when number of
                                           buffer descriptors used is at or
                                           above this threshold. */
    int xon_threshold_bd;               /* Remove backpressure when number of
                                           buffer descriptors used drops below
                                           this threshold. */
    int drop_threshold;                 /* Drop packets when number of buffers
                                           used hit this threshold. */
    int reserved_buffers;               /* Number of buffers reserved for the
                                           specified priority group. */
    int lossless;                       /* lossless PFC enable for the specified
                                           priority group. */
} bcm_cosq_pfc_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Configure Priority-Based Flow Control (PFC). */
extern int bcm_cosq_pfc_config_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    uint32 flags, 
    bcm_cosq_pfc_config_t *config);

/* Configure Priority-Based Flow Control (PFC). */
extern int bcm_cosq_pfc_config_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    uint32 flags, 
    bcm_cosq_pfc_config_t *config);

/* Get or set the mapping from internal priority to CoS queue. */
extern int bcm_cosq_port_mapping_set(
    int unit, 
    bcm_port_t port, 
    bcm_cos_t priority, 
    bcm_cos_queue_t cosq);

/* Get or set the mapping from internal priority to CoS queue. */
extern int bcm_cosq_port_mapping_get(
    int unit, 
    bcm_port_t port, 
    bcm_cos_t priority, 
    bcm_cos_queue_t *cosq);

/* 
 * Get or set the mapping  of multiple internal priorities to their
 * corresponding CoS queues.
 */
extern int bcm_cosq_port_mapping_multi_set(
    int unit, 
    bcm_port_t port, 
    int count, 
    bcm_cos_t *priority_array, 
    bcm_cos_queue_t *cosq_array);

/* 
 * Get or set the mapping  of multiple internal priorities to their
 * corresponding CoS queues.
 */
extern int bcm_cosq_port_mapping_multi_get(
    int unit, 
    bcm_port_t port, 
    int count, 
    bcm_cos_t *priority_array, 
    bcm_cos_queue_t *cosq_array);

#endif /* BCM_HIDE_DISPATCHABLE */

/* parameters for congestion mapping */
typedef struct bcm_cosq_congestion_mapping_info_s {
    bcm_cos_t flow_control_bits[BCM_COS_COUNT]; /* Valid flow control bits in Fabric
                                           0(Invalid), 1(Valid), -1(ignore). */
} bcm_cosq_congestion_mapping_info_t;

/* Initialize a CoSQ congestion mapping information structure. */
extern void bcm_cosq_congestion_mapping_info_t_init(
    bcm_cosq_congestion_mapping_info_t *config);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Set/Get flow control bits as per valid cos values for the connected
 * fabric module.
 */
extern int bcm_cosq_congestion_mapping_set(
    int unit, 
    int fabric_modid, 
    bcm_cosq_congestion_mapping_info_t *mapping_info);

/* 
 * Set/Get flow control bits as per valid cos values for the connected
 * fabric module.
 */
extern int bcm_cosq_congestion_mapping_get(
    int unit, 
    int fabric_modid, 
    bcm_cosq_congestion_mapping_info_t *mapping_info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Class of Service policies. */
#define BCM_COSQ_NONE                       0x0        /* Pass through. */
#define BCM_COSQ_STRICT                     0x01       /* Weights are ignored. */
#define BCM_COSQ_ROUND_ROBIN                0x02       /* Weights are ignored. */
#define BCM_COSQ_WEIGHTED_ROUND_ROBIN       0x03       
#define BCM_COSQ_WEIGHTED_FAIR_QUEUING      0x04       
#define BCM_COSQ_DEFICIT_ROUND_ROBIN        0x05       /* XGS3 only. */
#define BCM_COSQ_BOUNDED_DELAY              0x06       /* Strata only. */
#define BCM_COSQ_SP                         BCM_COSQ_STRICT /* Strict priority. */
#define BCM_COSQ_EF                         0x07       /* Expedited forwarding. */
#define BCM_COSQ_AF                         0x08       /* Assured forwarding. */
#define BCM_COSQ_SP_GLOBAL                  0x09       
#define BCM_COSQ_BE                         0x0A       /* Best effort. */
#define BCM_COSQ_CALENDAR                   0x0B       /* Calendar based
                                                          scheduling discipline. */
#define BCM_COSQ_SP0                        0x0C       /* Strict priority level
                                                          0. */
#define BCM_COSQ_SP1                        0x0D       /* Strict priority level
                                                          1. */
#define BCM_COSQ_SP2                        0x0E       /* Strict priority level
                                                          2. */
#define BCM_COSQ_SP3                        0x0F       /* Strict priority level
                                                          3. */
#define BCM_COSQ_SP4                        0x10       /* Strict priority level
                                                          4. */
#define BCM_COSQ_SP5                        0x11       /* Strict priority level
                                                          5. */
#define BCM_COSQ_SP6                        0x12       /* Strict priority level
                                                          6. */
#define BCM_COSQ_SP7                        0x13       /* Strict priority level
                                                          7. */
#define BCM_COSQ_SP8                        0x14       /* Strict priority level
                                                          8. */
#define BCM_COSQ_SP9                        0x15       /* Strict priority level
                                                          9. */
#define BCM_COSQ_SP10                       0x16       /* Strict priority level
                                                          10. */
#define BCM_COSQ_AF0                        0x17       /* Assured forwarding
                                                          level 0. */
#define BCM_COSQ_AF1                        0x18       /* Assured forwarding
                                                          level 1. */
#define BCM_COSQ_AF2                        0x19       /* Assured forwarding
                                                          level 2. */
#define BCM_COSQ_AF3                        0x1A       /* Assured forwarding
                                                          level 3. */
#define BCM_COSQ_AF4                        0x1B       /* Assured forwarding
                                                          level 4. */
#define BCM_COSQ_AF5                        0x1C       /* Assured forwarding
                                                          level 5. */
#define BCM_COSQ_AF6                        0x1D       /* Assured forwarding
                                                          level 6. */
#define BCM_COSQ_AF7                        0x1E       /* Assured forwarding
                                                          level 7. */
#define BCM_COSQ_GSP0                       0x1F       /* Global Strict priority
                                                          level 0. */
#define BCM_COSQ_GSP1                       0x20       /* Global Strict priority
                                                          level 1. */
#define BCM_COSQ_GSP2                       0x21       /* Global Strict priority
                                                          level 2. */
#define BCM_COSQ_GSP3                       0x22       /* Global Strict priority
                                                          level 3. */
#define BCM_COSQ_GSP4                       0x23       /* Global Strict priority
                                                          level 4. */
#define BCM_COSQ_GSP5                       0x24       /* Global Strict priority
                                                          level 5. */
#define BCM_COSQ_GSP6                       0x25       /* Global Strict priority
                                                          level 6. */
#define BCM_COSQ_GSP7                       0x26       /* Global Strict priority
                                                          level 7. */
#define BCM_COSQ_GSP8                       0x27       /* Global Strict priority
                                                          level 8. */
#define BCM_COSQ_GSP9                       0x28       /* Global Strict priority
                                                          level 9. */
#define BCM_COSQ_GSP10                      0x29       /* Global Strict priority
                                                          level 10. */
#define BCM_COSQ_DELAY_TOLERANCE_NORMAL     0x2A       /* Delay tolerance is
                                                          normal */
#define BCM_COSQ_DELAY_TOLERANCE_LOW_DELAY  0x2B       /* Delay tolerance is low
                                                          delay */
#define BCM_COSQ_DELAY_TOLERANCE_02         0x2C       /* Flexible delay
                                                          tolerance level.
                                                          Mainly needed for
                                                          interfaces with rates
                                                          higher than 10G (e.g.
                                                          Interlaken) */
#define BCM_COSQ_DELAY_TOLERANCE_03         0x2D       /* Flexible delay
                                                          tolerance level.
                                                          Mainly needed for
                                                          interfaces with rates
                                                          higher than 10G (e.g.
                                                          Interlaken) */
#define BCM_COSQ_DELAY_TOLERANCE_04         0x2E       /* Flexible delay
                                                          tolerance level.
                                                          Mainly needed for
                                                          interfaces with rates
                                                          higher than 10G (e.g.
                                                          Interlaken) */
#define BCM_COSQ_DELAY_TOLERANCE_05         0x2F       /* Flexible delay
                                                          tolerance level.
                                                          Mainly needed for
                                                          interfaces with rates
                                                          higher than 10G (e.g.
                                                          Interlaken) */
#define BCM_COSQ_DELAY_TOLERANCE_06         0x30       /* Flexible delay
                                                          tolerance level.
                                                          Mainly needed for
                                                          interfaces with rates
                                                          higher than 10G (e.g.
                                                          Interlaken) */
#define BCM_COSQ_DELAY_TOLERANCE_07         0x31       /* Flexible delay
                                                          tolerance level.
                                                          Mainly needed for
                                                          interfaces with rates
                                                          higher than 10G (e.g.
                                                          Interlaken) */
#define BCM_COSQ_DELAY_TOLERANCE_08         0x32       /* Flexible delay
                                                          tolerance level. */
#define BCM_COSQ_DELAY_TOLERANCE_09         0x33       /* Flexible delay
                                                          tolerance level. */
#define BCM_COSQ_DELAY_TOLERANCE_10         0x34       /* Flexible delay
                                                          tolerance level */
#define BCM_COSQ_DELAY_TOLERANCE_11         0x35       /* Flexible delay
                                                          tolerance level */
#define BCM_COSQ_DELAY_TOLERANCE_12         0x36       /* Flexible delay
                                                          tolerance level */
#define BCM_COSQ_DELAY_TOLERANCE_13         0x37       /* Flexible delay
                                                          tolerance level */
#define BCM_COSQ_DELAY_TOLERANCE_14         0x38       /* Flexible delay
                                                          tolerance level */
#define BCM_COSQ_DELAY_TOLERANCE_15         0x39       /* Flexible delay
                                                          tolerance level */
#define BCM_COSQ_PON_DROPDOWN               0x3A       /* Weights for EPON DBA
                                                          Dropdown. */
#define BCM_COSQ_DELAY_TOLERANCE_10G_SLOW_ENABLED 0x3B       /* Adjusted for slow
                                                          enabled 10Gb ports */
#define BCM_COSQ_DELAY_TOLERANCE_10G_LOW_DELAY 0x3C       /* Adjusted for low delay
                                                          10Gb ports */
#define BCM_COSQ_DELAY_TOLERANCE_1G         0x3D       /* Adjusted for 1Gb ports */
#define BCM_COSQ_DELAY_TOLERANCE_40G_SLOW_ENABLED 0x3E       /* Adjusted for slow
                                                          enabled 40Gb ports */
#define BCM_COSQ_DELAY_TOLERANCE_40G_LOW_DELAY 0x3F       /* Adjusted for low delay
                                                          40Gb ports */
#define BCM_COSQ_DELAY_TOLERANCE_100G_SLOW_ENABLED 0x40       /* Adjusted for slow
                                                          enabled 100Gb ports */
#define BCM_COSQ_DELAY_TOLERANCE_100G_LOW_DELAY 0x41       /* Adjusted for low delay
                                                          100Gb ports */
#define BCM_COSQ_DELAY_TOLERANCE_200G_SLOW_ENABLED 0x42       /* Adjusted for slow
                                                          enabled 200Gb ports */
#define BCM_COSQ_DELAY_TOLERANCE_200G_LOW_DELAY 0x43       /* Adjusted for low delay
                                                          200Gb ports */
#define BCM_COSQ_DELAY_TOLERANCE_SET_COMMON_STATUS_MSG -2         /* sets the status
                                                          message generation
                                                          period, in the common
                                                          status message mode */
#define BCM_COSQ_MAX                        0x43       
#define BCM_COSQ_DELAY_TOLERANCE_16         0x44       /* Flexible delay
                                                          tolerance level */
#define BCM_COSQ_DELAY_TOLERANCE_17         0x45       /* Flexible delay
                                                          tolerance level */
#define BCM_COSQ_DELAY_TOLERANCE_18         0x46       /* Flexible delay
                                                          tolerance level */
#define BCM_COSQ_DELAY_TOLERANCE_19         0x47       /* Flexible delay
                                                          tolerance level */
#define BCM_COSQ_DELAY_TOLERANCE_20         0x48       /* Flexible delay
                                                          tolerance level */
#define BCM_COSQ_DELAY_TOLERANCE_21         0x49       /* Flexible delay
                                                          tolerance level */
#define BCM_COSQ_DELAY_TOLERANCE_22         0x4A       /* Flexible delay
                                                          tolerance level */
#define BCM_COSQ_DELAY_TOLERANCE_23         0x4B       /* Flexible delay
                                                          tolerance level */

#define BCM_COSQ_WEIGHT_UNLIMITED   -1         
#define BCM_COSQ_WEIGHT_STRICT      0          
#define BCM_COSQ_WEIGHT_MIN         1          

#define BCM_COSQ_PRIO_VALID(prio)  ((prio) >= 0 && (prio < 8)) 

#define BCM_COSQ_QUEUE_VALID(unit, numq)  \
    ((numq) >= 0 && (numq) < NUM_COS(unit)) 

#ifndef BCM_HIDE_DISPATCHABLE

/* Get or set Class of Service policy, weights and delay. */
extern int bcm_cosq_sched_set(
    int unit, 
    int mode, 
    const int weights[BCM_COS_COUNT], 
    int delay);

/* Get or set Class of Service policy, weights and delay. */
extern int bcm_cosq_port_sched_set(
    int unit, 
    bcm_pbmp_t pbm, 
    int mode, 
    const int weights[BCM_COS_COUNT], 
    int delay);

/* Get or set Class of Service policy, weights and delay. */
extern int bcm_cosq_sched_get(
    int unit, 
    int *mode, 
    int weights[BCM_COS_COUNT], 
    int *delay);

/* Get or set Class of Service policy, weights and delay. */
extern int bcm_cosq_port_sched_get(
    int unit, 
    bcm_pbmp_t pbm, 
    int *mode, 
    int weights[BCM_COS_COUNT], 
    int *delay);

/* Get or set Class of Service policy, weights and delay. */
extern int bcm_cosq_sched_config_set(
    int unit, 
    int mode, 
    int weight_arr_size, 
    int *weights);

/* Get or set Class of Service policy, weights and delay. */
extern int bcm_cosq_port_sched_config_set(
    int unit, 
    bcm_pbmp_t pbm, 
    int mode, 
    int weight_arr_size, 
    int *weights);

/* Get or set Class of Service policy, weights and delay. */
extern int bcm_cosq_sched_config_get(
    int unit, 
    int weight_arr_size, 
    int *weights, 
    int *weight_arr_count, 
    int *mode);

/* Get or set Class of Service policy, weights and delay. */
extern int bcm_cosq_port_sched_config_get(
    int unit, 
    bcm_pbmp_t pbm, 
    int weight_arr_size, 
    int *weights, 
    int *weight_arr_count, 
    int *mode);

/* Retrieve maximum weights for given CoS policy. */
extern int bcm_cosq_sched_weight_max_get(
    int unit, 
    int mode, 
    int *weight_max);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_COSQ_ALL                        0x00000001 
#define BCM_COSQ_BW_EAV_MODE                0x00000008 /* Prioritize selected
                                                          queue(s) for bandwidth
                                                          before non-selected
                                                          queue(s). */
#define BCM_COSQ_BW_LLFC                    0x00000010 /* Link Level Flow
                                                          Control setting */
#define BCM_COSQ_BW_PACKET_MODE             0x00000020 /* The bandwidth
                                                          specified is in
                                                          packets per second
                                                          instead of kbits per
                                                          second */
#define BCM_COSQ_BW_NOT_COMMIT              0x00000040 /* Set rate to SW DB
                                                          only, commit to HW is
                                                          done when calling the
                                                          API with the same
                                                          GPort type and without
                                                          NOT_COMMIT flag. */
#define BCM_COSQ_BW_EAV_TAS_MODE            0x00000080 /* Avoid burstiness when
                                                          gate is opening or
                                                          after queue is empty. */
#define BCM_COSQ_BW_TAS_TOKEN_FREEZE        0x00000100 /* Freeze shaper token
                                                          when gate is closed to
                                                          avoid burstiness. */
#define BCM_COSQ_BW_ALT_CALC                0x00000200 /* Use alternative
                                                          building algorithm for
                                                          bandwidth calculation. */
#define BCM_COSQ_BW_FABRIC_ADJUST_CONFIG_SHARED 0x00000400 /* Indicates we want to
                                                          config shared DRM. */
#define BCM_COSQ_BW_BURST_CALCULATE         0x00000800 /* Re-calculate burst
                                                          based on configured
                                                          rate. */
#define BCM_COSQ_BW_ROUND_TO_CLOSEST        0x00001000 /* Round up or down to
                                                          the closest number. */

#ifndef BCM_HIDE_DISPATCHABLE

/* Configure a port's bandwidth distribution among CoS queues. */
extern int bcm_cosq_port_bandwidth_set(
    int unit, 
    bcm_port_t port, 
    bcm_cos_queue_t cosq, 
    uint32 kbits_sec_min, 
    uint32 kbits_sec_max, 
    uint32 flags);

/* Configure a port's bandwidth distribution among CoS queues. */
extern int bcm_cosq_port_bandwidth_get(
    int unit, 
    bcm_port_t port, 
    bcm_cos_queue_t cosq, 
    uint32 *kbits_sec_min, 
    uint32 *kbits_sec_max, 
    uint32 *flags);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Discard (WRED) related flags. */
#define BCM_COSQ_DISCARD_ENABLE             0x0001     
#define BCM_COSQ_DISCARD_CAP_AVERAGE        0x0002     
#define BCM_COSQ_DISCARD_NONTCP             0x0004     
#define BCM_COSQ_DISCARD_GRANULARITY_1K     0x0008     /* Threshold values are
                                                          expected in 1K
                                                          granularity */
#define BCM_COSQ_DISCARD_COLOR_GREEN        0x0100     
#define BCM_COSQ_DISCARD_COLOR_YELLOW       0x0200     
#define BCM_COSQ_DISCARD_COLOR_RED          0x0400     
#define BCM_COSQ_DISCARD_COLOR_BLACK        0x0800     
#define BCM_COSQ_DISCARD_COLOR_ALL          0x0F00     
#define BCM_COSQ_DISCARD_DROP_FIRST         BCM_COSQ_DISCARD_COLOR_RED 
#define BCM_COSQ_DISCARD_PACKETS            0x1000     
#define BCM_COSQ_DISCARD_BYTES              0x2000     
#define BCM_COSQ_DISCARD_MARK_CONGESTION    0x4000     
#define BCM_COSQ_DISCARD_PORT               0x8000     
#define BCM_COSQ_DISCARD_DEVICE             0x10000    
#define BCM_COSQ_DISCARD_BUFFER_DESC        0x20000    
#define BCM_COSQ_DISCARD_TCP                0x40000    
#define BCM_COSQ_DISCARD_SYSTEM             0x80000    
#define BCM_COSQ_DISCARD_PROBABILITY1       0x100000   
#define BCM_COSQ_DISCARD_PROBABILITY2       0x200000   
#define BCM_COSQ_DISCARD_IFP                0x400000   
#define BCM_COSQ_DISCARD_OUTER_CFI          0x800000   
#define BCM_COSQ_DISCARD_ECT_MARKED         0x2000000  /* WCS for ECT Marked
                                                          packets */
#define BCM_COSQ_DISCARD_RESPONSIVE_DROP    0x4000000  /* WCS for Responsive
                                                          Drop packets */
#define BCM_COSQ_DISCARD_NON_RESPONSIVE_DROP 0x8000000  /* WCS for Non-Responsive
                                                          Drop packets */
#define BCM_COSQ_DISCARD_EGRESS             0x10000000 
#define BCM_COSQ_DISCARD_POOL_0             0x20000000 /* Apply on pool 0 */
#define BCM_COSQ_DISCARD_POOL_1             0x40000000 /* Apply on pool 1 */

#ifndef BCM_HIDE_DISPATCHABLE

/* Configure Weighted Random Early Discard (WRED). */
extern int bcm_cosq_discard_set(
    int unit, 
    uint32 flags);

/* Configure Weighted Random Early Discard (WRED). */
extern int bcm_cosq_discard_get(
    int unit, 
    uint32 *flags);

/* Configure a port's Weighted Random Early Discard (WRED) parameters. */
extern int bcm_cosq_discard_port_set(
    int unit, 
    bcm_port_t port, 
    bcm_cos_queue_t cosq, 
    uint32 color, 
    int drop_start, 
    int drop_slope, 
    int average_time);

/* Configure a port's Weighted Random Early Discard (WRED) parameters. */
extern int bcm_cosq_discard_port_get(
    int unit, 
    bcm_port_t port, 
    bcm_cos_queue_t cosq, 
    uint32 color, 
    int *drop_start, 
    int *drop_slope, 
    int *average_time);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Definitions for queue priorities. Used for end-to-end scheduler flow
 * control configuration and for threshold setting
 */
#define BCM_COSQ_HIGH_PRIORITY  -4         
#define BCM_COSQ_LOW_PRIORITY   -5         

/* flags for various admission tests */
#define BCM_COSQ_CONTROL_ADMISSION_CT   0x0001     /* Category */
#define BCM_COSQ_CONTROL_ADMISSION_CTTC 0x0002     /* category, traffic class */
#define BCM_COSQ_CONTROL_ADMISSION_CTCC 0x0004     /* category, connection class */
#define BCM_COSQ_CONTROL_ADMISSION_ST   0x0008     /* statistics tag */
#define BCM_COSQ_CONTROL_ADMISSION_LLFC 0x0010     
#define BCM_COSQ_CONTROL_ADMISSION_PFC  0x0020     

/* 
 * Definitions used to select a specific configuration knob for OBM
 * buffer thresholds configuration
 */
#define BCM_COSQ_OBM_CLASS_LOSSLESS0    0          /* used for configuring
                                                      thresholds for traffic
                                                      class lossless0. */
#define BCM_COSQ_OBM_CLASS_LOSSLESS1    1          /* used for configuring
                                                      thresholds for traffic
                                                      class lossless1. */
#define BCM_COSQ_OBM_CLASS_LOSSY_LOW    2          /* used for configuring
                                                      thresholds for traffic
                                                      class lossy low. */
#define BCM_COSQ_OBM_CLASS_LOSSY        3          /* used for configuring
                                                      combined thresholds for
                                                      traffic classes lossy low
                                                      and lossy high. */
#define BCM_COSQ_OBM_CLASS_EXPRESS      4          /* used for configuring
                                                      thresholds for traffic
                                                      class express. */
#define BCM_COSQ_OBM_CLASS_ALL          5          /* used for configuring
                                                      combined thresholds for
                                                      all traffic classes. */

/* Definitions for ADT group priority */
#define BCM_COSQ_ADT_PRI_GROUP_MIDDLE   0          /* Queues that neither
                                                      trigger nor are subject to
                                                      adaptive alpha change. */
#define BCM_COSQ_ADT_PRI_GROUP_LOW      1          /* Queues that their burst
                                                      absorption (alpha) is
                                                      adaptively changed to
                                                      provide space for high
                                                      priority queues. */
#define BCM_COSQ_ADT_PRI_GROUP_HIGH     2          /* Queues that get precedence
                                                      in buffer usage by
                                                      triggering changes in low
                                                      priority queues alpha. */

/* Features that can be controlled on a gport/cosq  basis. */
typedef enum bcm_cosq_control_e {
    bcmCosqControlFabricConnectMinUtilization = 0, /* percentage of minimum utilization
                                           before connection is established. */
    bcmCosqControlFabricConnectMaxTime = 1, /* timeout configuration for minimum
                                           utilization connection setting. */
    bcmCosqControlBandwidthBurstMax = 2, /* kbits burst for maximum rate
                                           restriction if different from default
                                           burst size. */
    bcmCosqControlBandwidthBurstMin = 3, /* Burst for maximum rate restriction in
                                           kbits (for DNX [MZ] devices, check
                                           documentation for burst max units). */
    bcmCosqControlFabricConnectMax = 4, /* length a queue retains its fabric
                                           connection */
    bcmCosqControlSchedulable = 14,     /* Allow TX scheduling of packets in the
                                           queue. */
    bcmCosqControlPacketLengthAdjust = 16, /* positive or negative queue packet
                                           length adjustment in bytes */
    bcmCosqControlCongestionManagedQueue = 17, /* Congestion managed queue identifier */
    bcmCosqControlCongestionFeedbackWeight = 18, /* Congestion feedback weight */
    bcmCosqControlCongestionSetPoint = 19, /* Congestion point queue size set point */
    bcmCosqControlCongestionSampleBytesMin = 20, /* Minimum number of bytes to enqueue
                                           between congestion status sampling */
    bcmCosqControlCongestionSampleBytesMax = 21, /* Maximum number of bytes to enqueue
                                           between congestion status sampling */
    bcmCosqControlFabricPortIngressScheduler = 22, /* Associating ingress scheduler node
                                           with Fabric/Child gport */
    bcmCosqControlFlowControlPriority = 23, /* Flow Control Mapping to port level HR
                                           end-to-end scheduler. For end-to-end
                                           port scheduler only
                                           BCM_COSQ_HIGH_PRIORITY value is
                                           meaningful for arg parameter. */
    bcmCosqControlSpLevelMax = 24,      /* Retreive Max SP level of a scheduler
                                           Element */
    bcmCosqControlSp0WeightMax = 25,    /* For a Scheduler Element, retreive the
                                           Max weight support for SP level 0 */
    bcmCosqControlSp1WeightMax = 26,    /* For a Scheduler Element, retreive the
                                           Max weight support for SP level 1 */
    bcmCosqControlSp2WeightMax = 27,    /* For a Scheduler Element, retreive the
                                           Max weight support for SP level 2 */
    bcmCosqControlSp3WeightMax = 28,    /* For a Scheduler Element, retreive the
                                           Max weight support for SP level 3 */
    bcmCosqControlSp4WeightMax = 29,    /* For a Scheduler Element, retreive the
                                           Max weight support for SP level 4 */
    bcmCosqControlSp5WeightMax = 30,    /* For a Scheduler Element, retreive the
                                           Max weight support for SP level 5 */
    bcmCosqControlSp6WeightMax = 31,    /* For a Scheduler Element, retreive the
                                           Max weight support for SP level 6 */
    bcmCosqControlSp7WeightMax = 32,    /* For a Scheduler Element, retreive the
                                           Max weight support for SP level 7 */
    bcmCosqControlSp8WeightMax = 33,    /* For a Scheduler Element, retreive the
                                           Max weight support for SP level 8 */
    bcmCosqControlSp9WeightMax = 34,    /* For a Scheduler Element, retreive the
                                           Max weight support for SP level 9 */
    bcmCosqControlSp10WeightMax = 35,   /* For a Scheduler Element, retreive the
                                           Max weight support for SP level 10 */
    bcmCosqControlDropLimitAlpha = 36,  /* Congestion drop limit alpha */
    bcmCosqControlDropLimitBytes = 37,  /* Congestion drop limit bytes */
    bcmCosqControlResumeLimitBytes = 38, /* Resume traffic flow from Congestion
                                           drop */
    bcmCosqControlYellowDropLimitBytes = 39, /* Congestion yellow drop limit bytes */
    bcmCosqControlRedDropLimitBytes = 40, /* Congestion red drop limit bytes */
    bcmCosqControlQselOffset = 41,      /* associate qsel_offset_id */
    bcmCosqControlEgressFlowControlThreshold0 = 42, /* Boundary before which all traffic
                                           flows */
    bcmCosqControlEgressFlowControlThreshold1 = 43, /* Boundary for some traffic throttling */
    bcmCosqControlEgressFlowControlThreshold2 = 44, /* Boundary for higher traffic
                                           throttling */
    bcmCosqControlEgressFlowControlThreshold3 = 45, /* Boundary after which all traffic
                                           drops */
    bcmCosqControlFabricPortScheduler = 46, /* Associating scheduler node with
                                           Fabric/Child gport */
    bcmCosqControlClassMap = 47,        /* Associate an Egress Cos to a specific
                                           resource */
    bcmCosqControlSchedulerAdoptAllPriority = 48, /* Scheduler node priority adoption
                                           scheme, 1 - adopt all priorities, 0 -
                                           standard behaviour */
    bcmCosqControlEgressPool = 49,      /* Associate an Egress Cos to a specific
                                           service pool under both Hierarchical
                                           and non-Hierarchical mode.In
                                           Non-Hierarchical Mode, egress service
                                           pool for MC queue should be set. */
    bcmCosqControlEgressPoolLimitBytes = 50, /* Shared pool limit setting */
    bcmCosqControlEgressPoolYellowLimitBytes = 51, /* Shared pool limit setting for yellow
                                           packets */
    bcmCosqControlEgressPoolRedLimitBytes = 52, /* Shared pool limit setting for red
                                           packets */
    bcmCosqControlEgressPoolLimitEnable = 53, /* Enable or disable using egress shared
                                           pool */
    bcmCosqControlEfPropagation = 54,   /* Enable or disable EF propagation */
    bcmCosqControlFlowControlState = 55, /* Set flow control state for the
                                           specified gport */
    bcmCosqControlHeaderUpdateField = 56, /* updating header with queue specific
                                           information/signature */
    bcmCosqControlFlowSlowRate1 = 57,   /* configure the device wide slow rate
                                           one */
    bcmCosqControlFlowSlowRate2 = 58,   /* configure the device wide slow rate
                                           two */
    bcmCosqControlFlowSlowRate = 59,    /* associate slow rate with a connector
                                           gport or scheduling element gport. In
                                           case of a composite element this
                                           configuration should be done with
                                           gport created when the element was
                                           added. the valid values are 0 =>
                                           disable, 1 => slowRate one, 2 => slow
                                           Rate two */
    bcmCosqControlDiscreteWeightLevel0 = 60, /* weight associated with level 0 of
                                           scheduler element having discrete
                                           weight attribute */
    bcmCosqControlDiscreteWeightLevel1 = 61, /* weight associated with level 1 of
                                           scheduler element having discrete
                                           weight attribute */
    bcmCosqControlDiscreteWeightLevel2 = 62, /* weight associated with level 2 of
                                           scheduler element having discrete
                                           weight attribute */
    bcmCosqControlDiscreteWeightLevel3 = 63, /* weight associated with level 3 of
                                           scheduler element having discrete
                                           weight attribute */
    bcmCosqControlAdmissionTestProfileA = 64, /* admission test profile A */
    bcmCosqControlAdmissionTestProfileB = 65, /* admission test profile B */
    bcmCosqControlFlowControlEnable = 66, /* Enables / Disables device-level flow
                                           control functionality */
    bcmCosqControlEgressBandwidthEnable = 67, /* Enable / Disables Egress bandwidth
                                           functionality */
    bcmCosqControlCongestionFeedbackMax = 68, /* Maximum value of the Congestion
                                           Feedback. Units: bytes */
    bcmCosqControlCongestionManagedQueueMin = 69, /* It defines the range of VOQs mapped
                                           to CP-Id */
    bcmCosqControlCongestionManagedQueueMax = 70, /* It defines the range of VOQs mapped
                                           to CP-Id */
    bcmCosqControlCongestionThresholdRandom = 71, /* Randomize the Congestion Sample
                                           Threshold */
    bcmCosqControlIgnoreAnemic = 72,    /* Ignore anemic priority */
    bcmCosqControlCongestionProxy = 73, /* Enable/disable QCN Proxy */
    bcmCosqControlMulticastPriorityIngressScheduling = 78, /* Multicast packets traffic class
                                           mapping to high/low strict priority */
    bcmCosqControlEgressUCQueueSharedLimitBytes = 79, /* Egress UC Shared Queue limit setting */
    bcmCosqControlEgressUCQueueMinLimitBytes = 80, /* UC Min Queue limit setting */
    bcmCosqControlEgressUCSharedDynamicEnable = 81, /* Enable Dynamic threshold limit for
                                           Egress UC Queue */
    bcmCosqControlEgressUCQueueLimitEnable = 82, /* Enable Discards based on configured
                                           threshold for Egress UC Queue */
    bcmCosqControlEgressMCQueueSharedLimitBytes = 83, /* Egress MC Shared Queue limit setting */
    bcmCosqControlEgressMCQueueMinLimitBytes = 84, /* MC Min Queue limit setting */
    bcmCosqControlEgressMCSharedDynamicEnable = 85, /* Enable Dynamic threshold limit for
                                           Egress MC Queue */
    bcmCosqControlEgressMCQueueLimitEnable = 86, /* Enable Discards based on configured
                                           threshold for Egress UC Queue */
    bcmCosqControlIngressPortPGSharedLimitBytes = 87, /* Ingress Port PG Shared limit setting */
    bcmCosqControlIngressPortPGMinLimitBytes = 88, /* Ingress Port PG Min limit setting */
    bcmCosqControlIngressPortPGSharedDynamicEnable = 89, /* Enable Dynamic threshold limit for
                                           Ingress Port PG */
    bcmCosqControlIngressPortPoolMaxLimitBytes = 90, /* Ingress Port SP Max limit setting */
    bcmCosqControlIngressPortPoolMinLimitBytes = 91, /* Ingress Port SP Min limit setting */
    bcmCosqControlBandwidthBurstMaxEmptyQueue = 92, /* Sets the maximum amount of credits
                                           that an empty queue can get */
    bcmCosqControlBandwidthBurstMaxFlowControlledQueue = 93, /* Sets the maximum amount of credits
                                           for a flow controlled queue */
    bcmCosqControlSPPortLimitState = 94, /* For each port , retrieve limit state
                                           of the corresponding shared pool of
                                           the port */
    bcmCosqControlPGPortLimitState = 95, /* For each port , retrieve limit state
                                           of the corresponding priority group
                                           of the port */
    bcmCosqControlPGPortXoffState = 96, /* For each port , retrieve xoff state
                                           of the corresponding priority group
                                           of the port */
    bcmCosqControlPoolRedDropState = 97, /* For each pool , retrieve drop state
                                           of the red packets for the pool */
    bcmCosqControlPoolYellowDropState = 98, /* For each pool , retrieve drop state
                                           of the yellow packets for the pool */
    bcmCosqControlPoolGreenDropState = 99, /* For each pool , retrieve drop state
                                           of the green packets for the pool */
    bcmCosqControlIngressPool = 100,    /* Associate an Ingress Cos to a
                                           specific service pool */
    bcmCosqControlEEEQueueThresholdProfileSelect = 101, /* Select queue depth threshold as the
                                           condition to exit LPI state */
    bcmCosqControlEEEPacketLatencyProfileSelect = 102, /* Select packet max latency as the
                                           condition to exit LPI state */
    bcmCosqControlPortQueueUcast = 103, /* retrieve port, cosq queue number of
                                           PBSMH UC packet headers */
    bcmCosqControlPortQueueMcast = 104, /* retrieve port, cosq queue number of
                                           PBSMH MC packet headers */
    bcmCosqControlEgressPortPoolYellowLimitBytes = 105, /* Egress Port Service Pool limit
                                           setting for yellow packets */
    bcmCosqControlEgressPortPoolRedLimitBytes = 106, /* Egress Port Service Pool limit
                                           setting for red packets */
    bcmCosqControlSingleCalendarMode = 107, /* Set single calendar mode for an
                                           interface */
    bcmCosqControlPriorityPropagationEnable = 108, /* Enable priority propagation */
    bcmCosqControlPrioritySelect = 109, /* Set high/low port priority for
                                           propagation */
    bcmCosqControlFlowControlErrWatchdog = 110, /* Watchdog period */
    bcmCosqControlFlowControlErrHandle = 111, /* Fc indication that will be sent when
                                           watch-dog error is reported */
    bcmCosqControlUCEgressPool = 112,   /* Set Egress service pool for UC queue
                                           in Non-Hierarchical mode only */
    bcmCosqControlMCEgressPool = 113,   /* Set Egress service pool for MC queue
                                           in Non-Hierarchical mode only */
    bcmCosqControlRedirectQueueSharedLimitBytes = 114, /*  Redirected Queue Shared Limited in
                                           Bytes */
    bcmCosqControlRedirectQueueMinLimitBytes = 115, /*  Redirected Queue Min. Limit in Bytes */
    bcmCosqControlRedirectSharedDynamicEnable = 116, /*  Redirected Queue Enable Dynamic
                                           Shared */
    bcmCosqControlRedirectQueueLimitEnable = 117, /*  Redirected Queue Limit Enable */
    bcmCosqControlRedirectColorDynamicEnable = 118, /*  Redirected Queue Color Dynamic
                                           Enable */
    bcmCosqControlRedirectColorEnable = 119, /*  Redirected Queue Color Enable */
    bcmCosqControlUCDropLimitAlpha = 120, /*  UC Congestion drop limit alpha */
    bcmCosqControlMCDropLimitAlpha = 121, /*  MC Congestion drop limit alpha */
    bcmCosqControlIngressPoolLimitBytes = 122, /*  Ingress Shared Pool Limit in Bytes */
    bcmCosqControlIngressHeadroomPoolLimitBytes = 123, /*  Ingress Headroom Pool Limit in Bytes */
    bcmCosqControlIngressHeadroomPool = 124, /*  Associate an Ingress [Port, Prio] to
                                           a specific Headroom pool */
    bcmCosqControlPFCBackpressureEnable = 125, /*  Selection of being backpressured by
                                           PFC message received */
    bcmCosqControlE2ECCTransmitEnable = 126, /*  Enabling Transmission of E2ECC
                                           message */
    bcmCosqControlIngressPortPGHeadroomLimitBytes = 127, /*  Ingress port PG Headroom limit
                                           setting */
    bcmCosqControlIngressPortPGResetFloorBytes = 128, /*  Ingress port PG reset floor setting */
    bcmCosqControlEgressPoolSharedLimitBytes = 129, /*  Shared limit setting for per chip
                                           based egress Service Pool */
    bcmCosqControlEgressPoolResumeLimitBytes = 130, /*  Resume limit setting for per chip
                                           based egress Service Pool */
    bcmCosqControlEgressPoolYellowSharedLimitBytes = 131, /*  Shared limit setting for per chip
                                           based egress Service Pool for yellow
                                           packets */
    bcmCosqControlEgressPoolYellowResumeLimitBytes = 132, /*  Resume limit setting for per chip
                                           based egress Service Pool for yellow
                                           packets */
    bcmCosqControlEgressPoolRedSharedLimitBytes = 133, /*  Shared limit setting for per chip
                                           based egress Service Pool for red
                                           packets */
    bcmCosqControlEgressPoolRedResumeLimitBytes = 134, /*  Resume limit setting for per chip
                                           based egress Service Pool for red
                                           packets */
    bcmCosqControlOCBOnly = 135,        /*  Configures a queue to be OCB only */
    bcmCosqControlDefaultInvalidQueue = 136, /*  Configures the default queue that
                                           traffic will be redirected to in case
                                           it's destined for invalid queue */
    bcmCosqControlIngressPortDropTpid1 = 137, /* cosq ingress port drop global TPID 1. */
    bcmCosqControlIngressPortDropTpid2 = 138, /* cosq ingress port drop global TPID 2. */
    bcmCosqControlIngressPortDropUntaggedPCP = 139, /* cosq ingress port drop untagged PCP. */
    bcmCosqControlIngressLatencyEnable = 140, /* enable latency measurements between
                                           IRPP and ITPP */
    bcmCosqControlIngressPortDropTpid3 = 141, /* cosq ingress port drop global TPID 3. */
    bcmCosqControlIngressPortDropTpid4 = 142, /* cosq ingress port drop global TPID 4. */
    bcmCosqControlIngressPortDropIgnoreIpDscp = 143, /* cosq ingress port drop ignore IP
                                           DSCP. */
    bcmCosqControlIngressPortDropIgnoreMplsExp = 144, /* cosq ingress port drop ignore MPLS
                                           EXP. */
    bcmCosqControlIngressPortDropIgnoreInnerTag = 145, /* cosq ingress port drop ignore inner
                                           VLAN tag. */
    bcmCosqControlIngressPortDropIgonreOuterTag = 146, /* cosq ingress port drop ignore outer
                                           VLAN tag. */
    bcmCosqControlEgressUCQueueGroupMinEnable = 147, /* Specify to use queue guarantee or
                                           qgroup guarantee fro Unicast queue */
    bcmCosqControlEgressUCQueueGroupSharedLimitEnable = 148, /* Enable qgroup limit for the Unicast
                                           Queue */
    bcmCosqControlEgressUCQueueGroupMinLimitBytes = 149, /* Min Guarantee setting for Egress
                                           Qgroup */
    bcmCosqControlEgressUCQueueGroupSharedLimitBytes = 150, /* Shared Limit setting for Egress
                                           Qgroup */
    bcmCosqControlEgressUCQueueGroupSharedDynamicEnable = 151, /* Enable Dynamic Threshold for Qgroup
                                           shared limit */
    bcmCosqControlAlternateStoreForward = 152, /* configure ASF mode */
    bcmCosqControlEgressUCQueueGroupDropLimitAlpha = 153, /* Egress Qgroup Congestion drop limit
                                           alpha */
    bcmCosqControlOcbFadtDropEnable = 154, /* to allow activating the Ocb FADT by:
                                           bcm_cosq_control_set(unit, 0, 0,
                                           bcmCosqControlOcbFadtDropEnable, 1 /
                                           0); */
    bcmCosqControlEgressPriorityOverCast = 155, /* To allow activating SP before WFQ
                                           mode (per OTM port) by:
                                           bcm_cosq_control_set(unit,
                                           bcmCosqGportTypeLocalPort, 0,
                                           bcmCosqControlEgressPriorityOverCast,
                                           1 / 0); */
    bcmCosqControlEgressMCQueueGroupMinLimitBytes = 156, /* Min Guarantee setting for Egress
                                           Qgroup for Multicast */
    bcmCosqControlEgressMCQueueGroupMinEnable = 157, /* Specify to use queue guarantee or
                                           qgroup guarantee fro Multicast queue */
    bcmCosqControlIsHighPriority = 158, /* Flow can be high or low Priority,
                                           used for Rci Throttling. */
    bcmCosqControlIsFabric = 159,       /* Flow can be fabric(remote) or local
                                           flow, used for Rci Throttling */
    bcmCosqControlEgressPoolHighCongestionLimitBytes = 160, /* Egress pool high congestion
                                           threshold, for WRED */
    bcmCosqControlEgressPoolLowCongestionLimitBytes = 161, /* Egress pool low congestion threshold,
                                           for WRED */
    bcmCosqControlMulticastQueueToPdSpMap = 160, /* Map Multicast egress queue to PDs SP. */
    bcmCosqControlSourcePortSchedCompensationEnable = 161, 
    bcmCosqControlIngressPortPGResetOffsetBytes = 162, /*  Ingress port PG reset Offset setting */
    bcmCosqControlIngressPublicPoolLimitBytes = 163, /*  Public Service Pool Limit Bytes */
    bcmCosqControlIngressPoolResetOffsetLimitBytes = 164, /* Specify the Per pool reset offset
                                           limit */
    bcmCosqControlEgressUCQueueColorLimitDynamicEnable = 165, /* Specify if Color limit settings is
                                           static or dynamic for unicast queue */
    bcmCosqControlEgressUCQueueRedLimit = 166, /* Specify the Per queue red limit for
                                           unicast */
    bcmCosqControlEgressUCQueueYellowLimit = 167, /* Specify the Per queue yellow limit
                                           for unicast */
    bcmCosqControlEgressMCQueueColorLimitDynamicEnable = 168, /* Specify if Color limit settings is
                                           static or dynamic for multicast queue */
    bcmCosqControlEgressMCQueueRedLimit = 169, /* Specify the Per queue red limit for
                                           multicast */
    bcmCosqControlEgressMCQueueYellowLimit = 170, /* Specify the Per queue yellow limit
                                           for multicast */
    bcmCosqControlEgressPortPoolSharedLimitBytes = 171, /* Egress port service pool limit
                                           setting for shared */
    bcmCosqControlEgressPortPoolRedResumeBytes = 172, /* Resume limit setting for port egress
                                           Service Pool for red packets */
    bcmCosqControlEgressPortPoolYellowResumeBytes = 173, /* Resume limit setting for port egress
                                           Service Pool for yellow packets */
    bcmCosqControlEgressPortPoolSharedResumeBytes = 174, /* Resume limit setting for port egress
                                           Service pool drop state */
    bcmCosqControlIngressPortPoolResumeLimitBytes = 175, /* Ingress Port SP resume limit setting */
    bcmCosqControlEgressMCQueueColorLimitEnable = 176, /* Enable Discards based on configured
                                           threshold for Egress MC Queue color
                                           discard thresholds */
    bcmCosqControlEgressUCQueueColorLimitEnable = 177, /* Enable Discards based on configured
                                           threshold for Egress UC Queue color
                                           discard thresholds */
    bcmCosqControlObmDiscardLimit = 178, /* Configure discard limit in Bytes.
                                           Cosq value used in API
                                           bcm_cosq_control_set and
                                           bcm_cosq_control_get must be set
                                           using BCM_COSQ_OBM_CLASS_XXX. */
    bcmCosqControlObmMinLimit = 179,    /* Configure guaranteed limit in Bytes.
                                           Cosq value used in API
                                           bcm_cosq_control_set and
                                           bcm_cosq_control_get must be set to
                                           BCM_COSQ_OBM_CLASS_LOSSY. */
    bcmCosqControlPgVsqFadtFcEnable = 180, /* Enable FADT Flow Control on PG-VSQ */
    bcmCosqControlIngressPortPGGlobalHeadroomEnable = 181, /* Enable ingress port PG to make use of
                                           global Headroom */
    bcmCosqControlEgressTCGCirSpEnable = 182, /* Sets the TCG CIR SP per code */
    bcmCosqControlLatencyCounterEnable = 183, /* Enable latency counting */
    bcmCosqControlLatencyDropEnable = 184, /* Enable drop based on latency */
    bcmCosqControlLatencyEcnEnable = 185, /* Enable Ecn (cni) mark based on
                                           latency */
    bcmCosqControlLatencyDropThreshold = 186, /* Latency drop threshold */
    bcmCosqControlLatencyEcnThreshold = 187, /* Latency ecn (mark cni) threshold */
    bcmCosqControlLatencyTrackDropPacket = 188, /* Enable tracking packets which dropped
                                           due to latency */
    bcmCosqControlLatencyEgressCounterCommand = 189, /* Latency egress counter command */
    bcmCosqControlLatencyBinThreshold = 190, /* Latency bin threshold. use the cosq
                                           to select the bin index you refer to */
    bcmCosqControlLatencyTrack = 191,   /* Enable tracking latency packets in
                                           dedicated table. */
    bcmCosqControlIsInDram = 192,       /* Returns if VOQ is in DRAM or not */
    bcmCosqControlGroup = 193,          /* Control group value of an element */
    bcmCosqControlIngressPortPoolSharedLimitBytes = 194, /* Ingress Port SP Shared limit setting */
    bcmCosqControlEgressUCQueueResumeOffsetBytes = 195, /* Egress UC Queue Resume Offset limit */
    bcmCosqControlEgressMCQueueResumeOffsetBytes = 196, /* Egress MC Queue Resume Offset limit */
    bcmCosqControlLatencyVoqRejectBinThreshold = 197, /* Latency bin threshold. use the cosq
                                           to select the bin index you refer to */
    bcmCosqControlLatencyVoqRejectFilterMulticast = 198, /* Configure MC packets to update VOQ
                                           latency */
    bcmCosqControlLatencyVoqRejectFilterSniff = 199, /* Configure SNIFF packets to update VOQ
                                           latency */
    bcmCosqControlMirrorOnDropAgingGlobal = 200, /* Mirror on Drop aging time for global
                                           type */
    bcmCosqControlMirrorOnDropAgingVoq = 201, /* Mirror on Drop aging time for voq
                                           type */
    bcmCosqControlMirrorOnDropAgingVsq = 202, /* Mirror on Drop aging time for vsq
                                           type */
    bcmCosqControlFlush = 203,          /* Flush the element specified by Gport */
    bcmCosqControlPhantomQueueThreshold = 204, /* Control threshold for phantom queues */
    bcmCosqControlEgrRateMeasurementInterval = 205, /* Control Interval [usec] for Egress
                                           rate measurement */
    bcmCosqControlEgrRateMeasurementWeight = 206, /* Control rate weight for Egress rate
                                           measurements. see
                                           BCM_COSQ_RATE_WEIGHT_XXX */
    bcmCosqControlShaperPacketMode = 207, /* Control shaper resolution mode:
                                           'packet' or 'bit'. If set to '1'
                                           ('packet') then shaping is per full
                                           packets. */
    bcmCosqControlEgressPoolMCSharedLimitEntries = 208, /* Egress MC Port Pool Shared Limit */
    bcmCosqControlEgressMCQueueSharedLimitEntries = 209, /* Egress MC Queue Shared Limit Entries */
    bcmCosqControlEgressRQESharedLimitBytes = 210, /* Egress RQE Threshold */
    bcmCosqControlEgressMCQueueDataBufferSharedLimitBytes = 211, /* Egress MC Shared Queue limit setting
                                           of data buffers in bytes */
    bcmCosqControlEgressMCQueueDataBufferRedLimit = 212, /* Specify the Per queue red limit of
                                           data buffers for multicast */
    bcmCosqControlEgressMCQueueEntryRedLimit = 213, /* Specify the Per queue red limit of
                                           egress queue entries for multicast */
    bcmCosqControlEgressMCQueueDataBufferYellowLimit = 214, /* Specify the Per queue yellow limit of
                                           data buffers for multicast */
    bcmCosqControlEgressMCQueueEntryYellowLimit = 215, /* Specify the Per queue yellow limit of
                                           egress queue entries for multicast */
    bcmCosqControlOcbRangeSelectThreshold = 216, /* Controls the OCB range in selected
                                           threshold. */
    bcmCosqLowRateVoqConnectorFactor = 217, /* Low rate VOQ connector factor. */
    bcmCosqControlIngressPortDropPortProfileMap = 218, /* Map port to prd profile. */
    bcmCosqControlE2ETCGCirSpEnable = 219, /* enable Strict Priority between E2E
                                           TCGs. */
    bcmCosqControlLatencyDropProbEnable = 220, /* Enable drop probability based on
                                           latency */
    bcmCosqControlLatencyDropProbBaseThreshold = 221, /* Define the drop base threshold -
                                           relevant if drop probability enabled */
    bcmCosqControlLatencyEcnProbEnable = 222, /* Enable ECN (cni) marking probability
                                           based on latency */
    bcmCosqControlLatencyEcnProbBaseThreshold = 223, /* Latency ECN(mark cni) probability
                                           base threshold */
    bcmCosqControlLatencyEcnProbConvertExponent = 224, /* Latency ECN Probability Convert
                                           Exponent - define the converted Ecn
                                           marking probability function for
                                           classic TCP packets in coupled mode */
    bcmCosqControlLatencyEcnProbConvertCoeff = 225, /* Latency ECN Probability Convert
                                           Coefficient - define the converted
                                           Ecn marking probability function for
                                           classic TCP packets in coupled mode */
    bcmCosqControlLatencyEgressAqmFlowId = 226, /* Egress latency AQM - map port and tc
                                           to flow id gport */
    bcmCosqControlLatencyEgressAqmFlowIdToProfile = 227, /* Egress latency AQM - Map AQM flow id
                                           to AQM profile */
    bcmCosqControlLatencyEgressCoupledAqmMode = 228, /* Egress latency AQM - define per Aqm
                                           e2e profile if working in coupled
                                           mode or not */
    bcmCosqControlLatencyEgressAqmL4sEcnClassificationThresholdMax = 229, /* Egress latency AQM - define the max
                                           latency value (in nanoseconds), in
                                           which packet will be define L4S,
                                           relevant only to
                                           LATENCY_END_TO_END_AQM profile. */
    bcmCosqControlLatencyEgressAqmAverageWeight = 230, /* Egress latency AQM - define the
                                           weight of the current latency sample,
                                           in order to calculate average
                                           latency. relevant only to
                                           LATENCY_END_TO_END_AQM profile. */
    bcmCosqControlLatencyEcnInstantThreshold = 231, /*  Latency-based ECN instantaneous mode
                                           threshold. */
    bcmCosqControlIngressMinLowPriority = 232, 
    bcmCosqControlAdtPriGroup = 233,    /* Set ADT group for a UC queue. */
    bcmCosqControlEgressPoolAdtHighPriAlpha = 234, /* Set ADT high priority queue alpha
                                           used to compute margin value of
                                           shared service pool. */
    bcmCosqControlEgressPoolAdtMargin0 = 235, /* Set ADT margin level for margin 0 in
                                           bytes of shared service pool. */
    bcmCosqControlEgressPoolAdtMargin1 = 236, /* Set ADT margin level for margin 1 in
                                           bytes of service pool. */
    bcmCosqControlEgressPoolAdtMargin2 = 237, /* Set ADT margin level for margin 2 in
                                           bytes of service pool. */
    bcmCosqControlEgressPoolAdtMargin3 = 238, /* Set ADT margin level for margin 3 in
                                           bytes of shared service pool. */
    bcmCosqControlEgressPoolAdtMargin4 = 239, /* Set ADT margin level for margin 4 in
                                           bytes of service pool. */
    bcmCosqControlEgressPoolAdtMargin5 = 240, /* Set ADT margin level for margin 5 in
                                           bytes of service pool. */
    bcmCosqControlEgressPoolAdtMargin6 = 241, /* Set ADT margin level for margin 6 in
                                           bytes of shared service pool. */
    bcmCosqControlEgressPoolAdtMargin7 = 242, /* Set ADT margin level for margin 7 in
                                           bytes of service pool. */
    bcmCosqControlEgressPoolAdtMargin8 = 243, /* Set ADT margin level for margin 8 in
                                           bytes of service pool. */
    bcmCosqControlEgressPoolAdtMargin9 = 244, /* Set ADT margin level for margin 9 in
                                           bytes of shared service pool. */
    bcmCosqControlAdtLowPriMonitorPool = 245, /* Set egress pool on which ADT margin
                                           levels are monitored. */
    bcmCosqControlEgressPoolToIngressPoolMap = 246, /*  Associate egress service pool to
                                           ingress service pools. Bit [3..0] -
                                           ingress pool [3..0]. */
    bcmCosqControlEgressPoolMCSharedEntryLimit = 247, /* Egress MC Pool Shared Limit Entries */
    bcmCosqControlPolicerTCSMThresh = 248 /* Set Policer TCSM thresholds in % of
                                           the meter bucket size. */
} bcm_cosq_control_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set various features at the gport/cosq level. */
extern int bcm_cosq_control_set(
    int unit, 
    bcm_gport_t port, 
    bcm_cos_queue_t cosq, 
    bcm_cosq_control_t type, 
    int arg);

#endif /* BCM_HIDE_DISPATCHABLE */

typedef enum bcm_cosq_priority_mapping_e {
    bcmCosqPriorityGroup = 0,   /* Index is priority group. */
    bcmCosqIngressPool = 1      /* Index is service pool. */
} bcm_cosq_priority_mapping_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Get priority list attched to a priority group or service pool on a
 * port.
 */
extern int bcm_cosq_priority_mapping_get_all(
    int unit, 
    bcm_gport_t gport, 
    int index, 
    bcm_cosq_priority_mapping_t type, 
    int pri_max, 
    int *pri_array, 
    int *pri_count);

/* Set various features at the gport/cosq level. */
extern int bcm_cosq_control_get(
    int unit, 
    bcm_gport_t port, 
    bcm_cos_queue_t cosq, 
    bcm_cosq_control_t type, 
    int *arg);

#endif /* BCM_HIDE_DISPATCHABLE */

/* max number of buffers */
#define BCM_COSQ_MAX_BUFFER     4          

/* integrated cosq indexes for cosq APIs */
typedef struct bcm_cosq_object_id_s {
    bcm_gport_t port;               /* gport */
    bcm_cos_queue_t cosq;           /* cosq or priority */
    bcm_cosq_buffer_id_t buffer;    /* buffer id */
} bcm_cosq_object_id_t;

/* Initialize COSQ compound index structure. */
extern void bcm_cosq_object_id_t_init(
    bcm_cosq_object_id_t *id);

/* Integrated cosq feature data. */
typedef struct bcm_cosq_control_data_s {
    bcm_cosq_control_t type;    /* cosq feature. */
    int arg;                    /* cosq feature value. */
} bcm_cosq_control_data_t;

/* Initialize COSQ control data structure. */
extern void bcm_cosq_control_data_t_init(
    bcm_cosq_control_data_t *control);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/get various features at the gport, cosq, and/or other level. */
extern int bcm_cosq_control_extended_set(
    int unit, 
    bcm_cosq_object_id_t *id, 
    bcm_cosq_control_data_t *control);

/* Set/get various features at the gport, cosq, and/or other level. */
extern int bcm_cosq_control_extended_get(
    int unit, 
    bcm_cosq_object_id_t *id, 
    bcm_cosq_control_data_t *control);

#endif /* BCM_HIDE_DISPATCHABLE */

/* For Virtual output queues (system ports) */
typedef struct bcm_cosq_gport_discard_s {
    uint32 flags; 
    int min_thresh;         /* Queue depth in bytes to begin dropping. */
    int max_thresh;         /* Queue depth in bytes to drop all packets. */
    int drop_probability;   /* Drop probability at max threshold. */
    int gain;               /* Determines the smoothing that should be applied. */
    int ecn_thresh;         /* Queue depth in bytes to stop marking and start
                               dropping. */
    int refresh_time;       /* Actual average refresh time. */
    uint32 use_queue_group; /* Use QGroupMin instead of QMin for WRED
                               resolution. */
    uint32 profile_id;      /* Profile of discard rules. */
} bcm_cosq_gport_discard_t;

/* For Virtual output queues (system ports) */
typedef enum bcm_cosq_gport_stats_e {
    bcmCosqGportGreenAcceptedPkts = 0,  /* Green/DP0, accepted packet count. */
    bcmCosqGportGreenAcceptedBytes = 1, /* Green/DP0, accepted byte count. */
    bcmCosqGportNotGreenAcceptedPkts = 2, /* Not Green/DP1-3, accepted packet
                                           count. */
    bcmCosqGportNotGreenAcceptedBytes = 3, /* Not Green/DP1-3, accepted byte count. */
    bcmCosqGportGreenCongestionMarkedPkts = 4, /* Green/DP0, ECN Marked packets. */
    bcmCosqGportGreenCongestionMarkedBytes = 5, /* Green/DP0, ECN Marked bytes. */
    bcmCosqGportGreenDiscardDroppedPkts = 6, /* Green/DP0, WRED dropped packets. */
    bcmCosqGportGreenDiscardDroppedBytes = 7, /* Green/DP0, WRED dropped bytes. */
    bcmCosqGportYellowAcceptedPkts = 8, /* Yellow/DP1, accepted packet count. */
    bcmCosqGportYellowAcceptedBytes = 9, /* Yellow/DP1, accepted byte count. */
    bcmCosqGportYellowCongestionMarkedPkts = 10, /* Yellow/DP1, ECN Marked packets. */
    bcmCosqGportYellowCongestionMarkedBytes = 11, /* Yellow/DP1, ECN nMarked bytes. */
    bcmCosqGportYellowDiscardDroppedPkts = 12, /* Yellow/DP1, WRED dropped packets. */
    bcmCosqGportYellowDiscardDroppedBytes = 13, /* Yellow/DP1, WRED dropped bytes. */
    bcmCosqGportRedAcceptedPkts = 14,   /* Red/DP2, accepted packet count. */
    bcmCosqGportRedAcceptedBytes = 15,  /* Red/DP2, accepted byte count. */
    bcmCosqGportRedCongestionMarkedPkts = 16, /* Red/DP2, ECN Marked packets. */
    bcmCosqGportRedCongestionMarkedBytes = 17, /* Red/DP2, ECN Marked bytes. */
    bcmCosqGportRedDiscardDroppedPkts = 18, /* Red/DP2, WRED dropped packets. */
    bcmCosqGportRedDiscardDroppedBytes = 19, /* Red/DP2, WRED dropped bytes. */
    bcmCosqGportBlackAcceptedPkts = 20, /* Black/DP3, accepted packet count. */
    bcmCosqGportBlackAcceptedBytes = 21, /* Black/DP3, accepted byte count. */
    bcmCosqGportNonWredDroppedPkts = 22, /* NON-WRED dropped packet count. */
    bcmCosqGportNonWredDroppedBytes = 23, /* NON-WRED dropped byte count. */
    bcmCosqGportDequeuedPkts = 24,      /* dequeued packets. */
    bcmCosqGportDequeuedBytes = 25,     /* dequeued bytes. */
    bcmCosqGportGreenDroppedPkts = 26,  /* Green/DP0, non-WRED dropped pkts. */
    bcmCosqGportGreenDroppedBytes = 27, /* Green/DP0, non-WRED dropped bytes. */
    bcmCosqGportNotGreenDroppedPkts = 28, /* Not Green/DP1-3 dropped pkts. */
    bcmCosqGportNotGreenDroppedBytes = 29, /* Not Green/DP1-3 dropped bytes. */
    bcmCosqGportYellowDroppedPkts = 30, /* Yellow/DP1, non-WRED dropped pkts. */
    bcmCosqGportYellowDroppedBytes = 31, /* Yellow/DP1, non-WRED dropped bytes. */
    bcmCosqGportRedDroppedPkts = 32,    /* Red/DP2, non-WRED dropped pkts. */
    bcmCosqGportRedDroppedBytes = 33,   /* Red/DP2, non-WRED dropped bytes. */
    bcmCosqGportBlackCongestionMarkedPkts = 34, /* Black/DP3, ECN Marked packets. */
    bcmCosqGportBlackCongestionMarkedBytes = 35, /* Black/DP3, ECN Marked bytes. */
    bcmCosqGportBlackDiscardDroppedPkts = 36, /* Black/DP3, WRED dropped at packets. */
    bcmCosqGportBlackDiscardDroppedBytes = 37, /* Black/DP3, WRED dropped bytes. */
    bcmCosqGportBlackDroppedPkts = 38,  /* Black/DP3, non-WRED dropped at
                                           packets. */
    bcmCosqGportBlackDroppedBytes = 39, /* Black/DP3, non-WRED dropped bytes. */
    bcmCosqGportOverSubscribeTotelDroppedPkts = 40, /* Free list underflow, pkts dropped. */
    bcmCosqGportOverSubscribeTotalDroppedBytes = 41, /* Free list underflow, bytes dropped. */
    bcmCosqGportOverSubscribeGuaranteeDroppedPkts = 42, /* Gbl buf thres guarantee, pkts
                                           dropped. */
    bcmCosqGportOverSubscribeGuaranteedDroppedBytes = 43, /* Gbl buf thres guarantee, bytes
                                           dropped. */
    bcmCosqGportYellowRedDiscardDroppedPkts = 46, /* Yellow and Red dropped packets */
    bcmCosqGportYellowRedDiscardDroppedBytes = 47, /* Yellow and Red dropped bytes */
    bcmCosqGportEnqueuedPkts = 48,      /* enqueued packets */
    bcmCosqGportEnqueuedBytes = 49,     /* enqueued bytes */
    bcmCosqGportOutLimitDroppedPkts = 50, /* Egress Admission Control (EAC)
                                           dropped pkts */
    bcmCosqGportOutLimitDroppedBytes = 51, /* EAC dropped bytes */
    bcmCosqGportOutLimitDroppedGreenPkts = 52, /* EAC dropped green pkts */
    bcmCosqGportOutLimitDroppedGreenBytes = 53, /* EAC dropped green bytes */
    bcmCosqGportOutLimitDroppedYellowPkts = 54, /* EAC dropped yellow pkts */
    bcmCosqGportOutLimitDroppedYellowBytes = 55, /* EAC dropped yellow bytes */
    bcmCosqGportOutLimitDroppedRedPkts = 56, /* EAC dropped red pkts */
    bcmCosqGportOutLimitDroppedRedBytes = 57, /* EAC dropped red bytes */
    bcmCosqGportOutLimitDroppedMulticastPkts = 58, /* EAC dropped multicast pkts */
    bcmCosqGportOutLimitDroppedMulticastBytes = 59, /* EAC dropped multicast bytes */
    bcmCosqGportOutPkts = 60,           /* Egress Interface. */
    bcmCosqGportOutBytes = 61,          /* Egress Interface. */
    bcmCosqGportReceivedBytes = 62,     /* Bytes received into queue. */
    bcmCosqGportReceivedPkts = 63,      /* Pkts received into queue. */
    bcmCosqGportDroppedBytes = 64,      /* Bytes dropped in queue. */
    bcmCosqGportDroppedPkts = 65,       /* Pkts dropped in queue. */
    bcmCosqGportDelayedBytes = 66,      /* Delayed bytes. */
    bcmCosqGportDelayedHundredUs = 67,  /* Maximum Pkts delay. */
    bcmCosqGportGreenPkts = 68,         /* Green/DP0 packet count. */
    bcmCosqGportGreenBytes = 69,        /* Green/DP0 byte count. */
    bcmCosqGportNotGreenPkts = 70,      /* Not-Green/DP0 packet count. */
    bcmCosqGportNotGreenBytes = 71,     /* Not-Green/DP0 byte count. */
    bcmCosqGportOffset0Bytes = 72,      /* Byte count at configurable offset 0. */
    bcmCosqGportOffset0Packets = 73,    /* Packet count at configurable offset
                                           0. */
    bcmCosqGportOffset1Bytes = 74,      /* Byte count at configurable offset 1. */
    bcmCosqGportOffset1Packets = 75,    /* Packet count at configurable offset
                                           1. */
    bcmCosqGportOffset2Bytes = 76,      /* Byte count at configurable offset 2. */
    bcmCosqGportOffset2Packets = 77,    /* Packet count at configurable offset
                                           2. */
    bcmCosqGportOffset3Bytes = 78,      /* Byte count at configurable offset 3. */
    bcmCosqGportOffset3Packets = 79,    /* Packet count at configurable offset
                                           3. */
    bcmCosqGportOffset4Bytes = 80,      /* Byte count at configurable offset 4. */
    bcmCosqGportOffset4Packets = 81,    /* Packet count at configurable offset
                                           4. */
    bcmCosqGportOffset5Bytes = 82,      /* Byte count at configurable offset 5. */
    bcmCosqGportOffset5Packets = 83,    /* Packet count at configurable offset
                                           5. */
    bcmCosqGportOffset6Bytes = 84,      /* Byte count at configurable offset 6. */
    bcmCosqGportOffset6Packets = 85,    /* Packet count at configurable offset
                                           6. */
    bcmCosqGportOffset7Bytes = 86,      /* Byte count at configurable offset 7. */
    bcmCosqGportOffset7Packets = 87,    /* Packet count at configurable offset
                                           7. */
    bcmCosqGportStatCount = 88          /* Must be the last entry ! */
} bcm_cosq_gport_stats_t;

/* Profile stat event-related flags. */
#define BCM_COSQ_GPORT_STATS_SHARED 0x00000001 

/* egress mapping redirection cos related flags. */
#define BCM_COSQ_GPORT_EGRESS_MAPPING_REDIRECTION_COS 0x00000001 

/* Statistics Profile */
typedef struct bcm_cosq_gport_stat_profile_s {
    SHR_BITDCL stats[_SHR_BITDCLSIZE(bcmCosqGportOutBytes)]; /* Use Gport Stats Enumeration */
} bcm_cosq_gport_stat_profile_t;

#define BCM_COSQ_GPORT_STAT_PROFILE_INIT(_profile_set)  SHR_BITCLR_RANGE(_profile_set.stats, 0, (sizeof(_profile_set)*8)) 
#define BCM_COSQ_GPORT_STAT_PROFILE_ADD(_profile_set, _stat)  SHR_BITSET(_profile_set.stats, _stat) 
#define BCM_COSQ_GPORT_STAT_PROFILE_REMOVE(_profile_set, _stat)  SHR_BITCLR(_profile_set.stats, _stat) 
#define BCM_COSQ_GPORT_STAT_PROFILE_TEST(_profile_set, _stat)  SHR_BITGET(_profile_set.stats, _stat) 

#define BCM_COSQ_PRIORITY_PROFILE_MAX_SIZE  64         /* Maximum priority
                                                          profile size across
                                                          all devices. */

/* Priority Profile */
typedef struct bcm_cosq_gport_priority_profile_s {
    SHR_BITDCL pri[_SHR_BITDCLSIZE(BCM_COSQ_PRIORITY_PROFILE_MAX_SIZE)]; /* Use Gport priority macros Enumeration */
} bcm_cosq_gport_priority_profile_t;

#define BCM_COSQ_PRI_PROFILE_SET(_profile, _offset)  SHR_BITSET(_profile.pri, _offset) 
#define BCM_COSQ_PRI_PROFILE_GET(_profile, _offset)  SHR_BITGET(_profile.pri, _offset) 
#define BCM_COSQ_PRI_PROFILE_CLEAR(_profile, _offset)  SHR_BITCLR(_profile.pri, (sizeof(_profile))) 
#define BCM_COSQ_PRI_PROFILE_CLEAR_ALL(_profile)  SHR_BITCLR_RANGE(_profile.pri, 0, BCM_COSQ_PRIORITY_PROFILE_MAX_SIZE) 
#define BCM_COSQ_PRI_PROFILE_SET_ALL(_profile)  SHR_BITSET_RANGE(_profile.pri, 0, BCM_COSQ_PRIORITY_PROFILE_MAX_SIZE) 

#ifndef BCM_HIDE_DISPATCHABLE

/* Get a profile of hi/lo priorities used for ingress queuing */
extern int bcm_cosq_priority_profile_get(
    int unit, 
    int pri_profile_id, 
    int *count, 
    bcm_cosq_gport_priority_profile_t *pri_profile);

/* Set up a profile of hi/lo priorities used for ingress queuing */
extern int bcm_cosq_priority_profile_set(
    int unit, 
    int pri_profile_id, 
    int count, 
    bcm_cosq_gport_priority_profile_t *pri_profile);

/* Associate a priority profile with a queue range. */
extern int bcm_cosq_priority_set(
    int unit, 
    int start_queue, 
    int end_queue, 
    int pri_profile_id);

/* Get the priority profile associated with a queue range. */
extern int bcm_cosq_priority_get(
    int unit, 
    int start_queue, 
    int end_queue, 
    int *pri_profile_id);

/* 
 * Set/Get a node to be flow controlled when a flow control packet is
 * recevied.
 */
extern int bcm_cosq_subport_flow_control_set(
    int unit, 
    bcm_gport_t subport, 
    bcm_gport_t sched_port);

/* 
 * Set/Get a node to be flow controlled when a flow control packet is
 * recevied.
 */
extern int bcm_cosq_subport_flow_control_get(
    int unit, 
    bcm_gport_t subport, 
    bcm_gport_t *sched_port);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a CoSQ gport discard structure. */
extern void bcm_cosq_gport_discard_t_init(
    bcm_cosq_gport_discard_t *discard);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Allocate/Retrieve unicast queue group and scheduler queue group
 * configuration.
 */
extern int bcm_cosq_gport_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_gport_t *physical_port, 
    int *num_cos_levels, 
    uint32 *flags);

#endif /* BCM_HIDE_DISPATCHABLE */

typedef int (*bcm_cosq_gport_traverse_cb)(
    int unit, 
    bcm_gport_t port, 
    int numq, 
    uint32 flags, 
    bcm_gport_t gport, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Traverse all added CoS queue GPORTs and call the supplied callback
 * routine for each one.
 */
extern int bcm_cosq_gport_traverse(
    int unit, 
    bcm_cosq_gport_traverse_cb cb, 
    void *user_data);

/* 
 * Traverse all added CoS queue GPORTs belong to a port and call the
 * supplied callback
 * routine for each one.
 */
extern int bcm_cosq_gport_traverse_by_port(
    int unit, 
    bcm_gport_t port, 
    bcm_cosq_gport_traverse_cb cb, 
    void *user_data);

/* Configure/retrieve minimum and maximum queue depth size setting */
extern int bcm_cosq_gport_size_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    uint32 bytes_min, 
    uint32 bytes_max);

/* Configure/retrieve minimum and maximum queue depth size setting */
extern int bcm_cosq_gport_size_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    uint32 *bytes_min, 
    uint32 *bytes_max);

#endif /* BCM_HIDE_DISPATCHABLE */

/* color size related flags. */
#define BCM_COSQ_GPORT_SIZE_BYTES           0x0001     
#define BCM_COSQ_GPORT_SIZE_BUFFER_DESC     0x0002     
#define BCM_COSQ_GPORT_SIZE_COLOR_BLIND     0x0004     
#define BCM_COSQ_GPORT_SIZE_COLOR_SYSTEM_RED 0x0008     
#define BCM_COSQ_GPORT_SIZE_OCB             0x0010     /* OCB configuration */
#define BCM_COSQ_GPORT_SIZE_SRC_VSQ_POOL1   0x0020     /* Configure the pool-1
                                                          source VSQ, else
                                                          configure the pool-0
                                                          source VSQ. */
#define BCM_COSQ_GPORT_SIZE_PACKET_DESC     0x0040     /* Configure packet
                                                          description */
#define BCM_COSQ_GPORT_SIZE_BUFFERS         0x0080     /* Configure buffers size */

#define BCM_COSQ_GPORT_SIZE_SRAM    BCM_COSQ_GPORT_SIZE_OCB /* For backward compatibility
                                                  support. */

/* color size configuration */
typedef struct bcm_cosq_gport_size_s {
    uint32 size_min;        /* minimum size */
    uint32 size_max;        /* maximum size */
    int32 size_alpha_max;   /* additional maximum size limit: free size * 2 ^
                               size_alpha_max */
    uint32 size_fadt_min;   /* additional minimal size (used mostly in FADT) */
} bcm_cosq_gport_size_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Configure/retrieve minimum and maximum queue depth color based size
 * setting
 */
extern int bcm_cosq_gport_color_size_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_color_t color, 
    uint32 flags, 
    bcm_cosq_gport_size_t *gport_size);

/* 
 * Configure/retrieve minimum and maximum queue depth color based size
 * setting
 */
extern int bcm_cosq_gport_color_size_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_color_t color, 
    uint32 flags, 
    bcm_cosq_gport_size_t *gport_size);

/* 
 * Configure/Retrieve enable/disable state of a cos level in a queue
 * group.
 *  BCM88230 only: Also configure/Retrieve enable/disable state of a FIFO
 * in an egress group.
 */
extern int bcm_cosq_gport_enable_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    int enable);

/* Force an immediate counter update and retrieve statistics. */
extern int bcm_cosq_gport_stat_sync_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_cosq_gport_stats_t stat, 
    uint64 *value);

/* 
 * Configure/Retrieve enable/disable state of a cos level in a queue
 * group.
 *  BCM88230 only: Also configure/Retrieve enable/disable state of a FIFO
 * in an egress group.
 */
extern int bcm_cosq_gport_enable_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    int *enable);

/* Enable/Disable statistics for queue group. */
extern int bcm_cosq_gport_stat_enable_set(
    int unit, 
    bcm_gport_t gport, 
    int enable);

/* Retrieve queue group statistic setting. */
extern int bcm_cosq_gport_stat_enable_get(
    int unit, 
    bcm_gport_t gport, 
    int *enable);

/* Retrieve/Set Statistics */
extern int bcm_cosq_gport_stat_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_cosq_gport_stats_t stat, 
    uint64 *value);

/* Retrieve/Set Statistics */
extern int bcm_cosq_gport_stat_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_cosq_gport_stats_t stat, 
    uint64 value);

/* Get the values associated with a set of Central Statistics Indices. */
extern int bcm_cosq_gport_statistic_multi_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_gport_t lgl_gport, 
    bcm_cos_queue_t cosq, 
    int stat_count, 
    bcm_cosq_gport_stats_t *stats_array, 
    int value_count, 
    uint64 *value_array);

extern int bcm_cosq_gport_statistic_multi_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_gport_t lgl_gport, 
    bcm_cos_queue_t cosq, 
    int stat_count, 
    bcm_cosq_gport_stats_t *stats_array, 
    uint64 value);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_COSQ_GPORT_WITH_ID              0x00000001 
#define BCM_COSQ_GPORT_SCHEDULER            0x00000002 
#define BCM_COSQ_GPORT_OVERLAY              0x00000004 
#define BCM_COSQ_GPORT_LATENCY_AQM_FLOW_ID  0x00000004 /* For DNX devices - AQM
                                                          flow id gport */
#define BCM_COSQ_GPORT_UCAST_QUEUE_GROUP    0x00000008 
#define BCM_COSQ_GPORT_DESTMOD_UCAST_QUEUE_GROUP 0x00000010 
#define BCM_COSQ_GPORT_MCAST_QUEUE          0x00000010 
#define BCM_COSQ_GPORT_MCAST_QUEUE_GROUP    0x00000020 
#define BCM_COSQ_GPORT_SUBSCRIBER           0x00000040 
#define BCM_COSQ_GPORT_TM_FLOW_ID           0x00000040 
#define BCM_COSQ_GPORT_EGRESS_GROUP         0x00000080 
#define BCM_COSQ_GPORT_DISABLE              0x00000100 /* Disable queue, used
                                                          for
                                                          BCM_COSQ_GPORT_CALENDAR
                                                          scheduling discipline */
#define BCM_COSQ_GPORT_CALENDAR             0x00000200 /* Indicates that this
                                                          gport is of scheduling
                                                          type CALENDAR */
#define BCM_COSQ_GPORT_ELEPHANT_UCAST_QUEUE_GROUP 0x00000200 /* Unicast queue for
                                                          elephant flows */
#define BCM_COSQ_GPORT_QSEL_ENTRY_DISABLE   0x00000400 /* Inhibits automatic
                                                          update of queue
                                                          mapping table based
                                                          upon the GPORT
                                                          parameters */
#define BCM_COSQ_GPORT_VOQ_CONNECTOR        0x00000800 /* Indicates a VoQ
                                                          connector resource has
                                                          to be allocated */
#define BCM_COSQ_GPORT_VSQ                  0x00001000 /* Indicates a VSQ
                                                          resource has to be
                                                          allocated */
#define BCM_COSQ_GPORT_ISQ                  0x00002000 /* indicates that an ISQ
                                                          resource has to be
                                                          allocated */
#define BCM_COSQ_GPORT_REPLACE              0x00004000 /* modify configuration */
#define BCM_COSQ_GPORT_RECYCLE              0x00008000 /* Indicate a recycle
                                                          queue space */
#define BCM_COSQ_GPORT_SCHEDULER_PFC_ALIGN  0x00008000 /* Indicate aligned
                                                          allocation of
                                                          scheduler node */
#define BCM_COSQ_GPORT_SCHEDULER_HR_DUAL_WFQ 0x00010000 /* The scheduler works in
                                                          Dual HR mode */
#define BCM_COSQ_GPORT_SCHEDULER_HR_ENHANCED 0x00020000 /* The scheduler works in
                                                          Enhanced HR mode */
#define BCM_COSQ_GPORT_SCHEDULER_HR_SINGLE_WFQ 0x00030000 /* The scheduler works in
                                                          Single HR mode */
#define BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE1_4SP 0x00040000 /* The scheduler works in
                                                          4 SP levels CL mode */
#define BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE2_3SP_WFQ 0x00080000 /* The scheduler works in
                                                          3 SP levels, the last
                                                          being WFQ */
#define BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE3_WFQ_2SP 0x000C0000 /* The scheduler works in
                                                          2 SP levels, the first
                                                          one being WFQ */
#define BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE4_2SP_WFQ 0x00100000 /* The scheduler works in
                                                          2 SP levels, the last
                                                          one being WFQ */
#define BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE5_1SP_WFQ 0x00140000 /* The scheduler works in
                                                          1 SP level and that is
                                                          WFQ mode */
#define BCM_COSQ_GPORT_SCHEDULER_FQ         0x00200000 /* The scheduler works in
                                                          Fair Queue mode */
#define BCM_COSQ_GPORT_SCHEDULER_CLASS_ENHANCED_LOWPRI_FQ 0x00400000 /* The CL scheduler
                                                          element is in enhanced
                                                          CL mode, when the
                                                          spouse FQ is the
                                                          lowest priority */
#define BCM_COSQ_GPORT_SCHEDULER_CLASS_ENHANCED_HIGHPRI_FQ 0x00800000 /* The CL scheduler
                                                          element is in enhanced
                                                          CL mode, when the
                                                          spouse FQ is the
                                                          highest priority */
#define BCM_COSQ_GPORT_SCHEDULER_CLASS_WFQ_MODE_DISCREET 0x01000000 /* WFQ mode is Discreet */
#define BCM_COSQ_GPORT_SCHEDULER_CLASS_WFQ_MODE_CLASS 0x02000000 /* WFQ mode is class
                                                          based */
#define BCM_COSQ_GPORT_SCHEDULER_WFQ        0x02000000 /* The scheduler works in
                                                          generic WFQ mode */
#define BCM_COSQ_GPORT_SCHEDULER_CLASS_WFQ_MODE_INDEPENDENT 0x03000000 /* WFQ mode is
                                                          independent (inverse) */
#define BCM_COSQ_GPORT_SCHEDULER_CLASS_WFQ_MODE_INDEPENDENT_PROPORTIONAL 0x08000000 /* WFQ mode is
                                                          independent
                                                          (proportinal) */
#define BCM_COSQ_GPORT_SCHEDULER_DUAL_SHAPER 0x04000000 /* The scheduler element
                                                          is in Dual shaper
                                                          mode. This scheduler
                                                          element is composed of
                                                          CL scheduler element
                                                          for CIR and a FQ
                                                          scheduler element for
                                                          EIR. */
#define BCM_COSQ_GPORT_COMPOSITE            0x10000000 /* composite mode for
                                                          CIR/PIR functionality */
#define BCM_COSQ_GPORT_NON_CONTIGUOUS_VOQ_CONNECTOR 0x00010000 /* queue, connector
                                                          resource associated
                                                          with non-contiguous
                                                          connector region */
#define BCM_COSQ_GPORT_WITH_SHAPING         0x04000000 
#define BCM_COSQ_GPORT_WITHOUT_SHAPING      0x08000000 
#define BCM_COSQ_GPORT_MULTIPATH            0x10000000 
#define BCM_COSQ_GPORT_VLAN_UCAST_QUEUE_GROUP 0x20000000 
#define BCM_COSQ_GPORT_VIRTUAL_PORT         0x40000000 
#define BCM_COSQ_GPORT_ADD_COS              0x80000000 
#define BCM_COSQ_GPORT_CONF_SCOPE_CORE0     0x20000000 
#define BCM_COSQ_GPORT_CONF_SCOPE_CORE1     0x40000000 
#define BCM_COSQ_GPORT_SW_ONLY              0x80000000 

#define BCM_COSQ_GPORT_PRIORITY_PROFILE_SET(_flags, _profile)  \
    _SHR_COSQ_GPORT_PRIORITY_PROFILE_SET(_flags, _profile) 

#define BCM_COSQ_GPORT_LOCAL    BCM_COSQ_GPORT_SUBSCRIBER /* for compatibility */

#define BCM_COSQ_SUBSCRIBER_MAP_PORT_VLAN   0x00000001 
#define BCM_COSQ_SUBSCRIBER_MAP_ENCAP_ID    0x00000002 

typedef struct bcm_cosq_subscriber_map_s {
    uint32 flags; 
    bcm_port_t port; 
    bcm_vlan_t vlan; 
    bcm_if_t encap_id; 
    bcm_gport_t queue_id; 
} bcm_cosq_subscriber_map_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Hierarchical queuing support */
extern int bcm_cosq_subscriber_map_add(
    int unit, 
    bcm_cosq_subscriber_map_t *map);

/* Hierarchical queuing support */
extern int bcm_cosq_subscriber_map_delete(
    int unit, 
    bcm_cosq_subscriber_map_t *map);

/* Hierarchical queuing support */
extern int bcm_cosq_subscriber_map_delete_all(
    int unit);

/* Hierarchical queuing support */
extern int bcm_cosq_subscriber_map_get(
    int unit, 
    bcm_cosq_subscriber_map_t *map);

#endif /* BCM_HIDE_DISPATCHABLE */

typedef int (*bcm_cosq_subscriber_map_traverse_cb)(
    int unit, 
    bcm_cosq_subscriber_map_t *map, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Hierarchical queuing support */
extern int bcm_cosq_subscriber_traverse(
    int unit, 
    bcm_cosq_subscriber_map_traverse_cb cb, 
    void *user_data);

/* 
 * Allocate/Retrieve unicast queue group and scheduler queue group
 * configuration.
 */
extern int bcm_cosq_gport_add(
    int unit, 
    bcm_gport_t port, 
    int numq, 
    uint32 flags, 
    bcm_gport_t *gport);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Config parameters for ingress queue bundle creation. */
typedef struct bcm_cosq_voq_connector_gport_s {
    uint32 flags; 
    bcm_gport_t port;           /* A gport that the created voq connector will
                                   be connected to at the to of the scheduling
                                   scheme. */
    int numq;                   /* number of cos levels to be created/number of
                                   voq connectors in the created bundle. */
    bcm_module_t remote_modid;  /* Ingress module id */
    uint32 nof_remote_cores;    /*  indicates how many connectors should be
                                   allocated */
} bcm_cosq_voq_connector_gport_t;

/* New flags for bcm_cosq_voq_connector_gport_add. */
#define BCM_COSQ_VOQ_CONNECTOR_GPORT_F_COMPOSITE 0x10000000 /* composite VOQ
                                                          connector */
#define BCM_COSQ_VOQ_CONNECTOR_GPORT_F_NON_CONTIGUOUS 0x00010000 /* Create on
                                                          non-contiguous
                                                          connector region */
#define BCM_COSQ_VOQ_CONNECTOR_GPORT_F_WITH_ID 0x00000001 /* VOQ connector ID */
#define BCM_COSQ_VOQ_CONNECTOR_GPORT_F_LOW_RATE 0x00000002 /* Low rate VOQ connector */
#define BCM_COSQ_VOQ_CONNECTOR_GPORT_F_RESERVED 0x00000800 /* Place holder for
                                                          legacy
                                                          BCM_COSQ_GPORT_VOQ_CONNECTOR
                                                          flag */

#ifndef BCM_HIDE_DISPATCHABLE

/* Allocate voq connectors */
extern int bcm_cosq_voq_connector_gport_add(
    int unit, 
    bcm_cosq_voq_connector_gport_t *config, 
    bcm_gport_t *gport);

extern int bcm_cosq_voq_connector_gport_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_cosq_voq_connector_gport_t *config);

/* Deletes queue group */
extern int bcm_cosq_gport_delete(
    int unit, 
    bcm_gport_t gport);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Config parameters for rate class create. */
typedef struct bcm_cosq_rate_class_create_info_s {
    int rate;           /* port rate in Gbps. */
    uint32 attributes;  /* additinal attributes for the rate class. see
                           COSQ_RATE_CLASS_CREATE_ATTR_XXX */
} bcm_cosq_rate_class_create_info_t;

#define BCM_COSQ_RATE_CLASS_CREATE_ATTR_SLOW_ENABLED 0x2        /* slow mechanism enabled */
#define BCM_COSQ_RATE_CLASS_CREATE_ATTR_OCB_ONLY 0x4        /* rate class to be used
                                                          on OCB only */
#define BCM_COSQ_RATE_CLASS_CREATE_ATTR_MULTICAST 0x8        /* rate class for FMQs
                                                          (Fabric Multicast
                                                          Queues) */

#ifndef BCM_HIDE_DISPATCHABLE

/* create a rate class with the specified attributes. */
extern int bcm_cosq_gport_rate_class_create(
    int unit, 
    bcm_gport_t gport, 
    uint32 flags, 
    bcm_cosq_rate_class_create_info_t *create_info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Config parameters for ingress queue attributes. */
typedef struct bcm_cosq_queue_attributes_s {
    int delay_tolerance_level;  /* Credit request profile of each queue in the
                                   bundle. */
    int rate_class;             /* Rate class profile of each queue in the
                                   bundle. */
} bcm_cosq_queue_attributes_t;

/* Config parameters for ingress queue bundle creation. */
typedef struct bcm_cosq_ingress_queue_bundle_gport_config_s {
    uint32 flags; 
    bcm_gport_t port;                   /* A gport that the created queue will
                                           be connected to, usually a system
                                           port. */
    bcm_core_t local_core_id;           /* Used to select the core of the
                                           created gport when
                                           BCM_COSQ_GPORT_WITH_ID is not used.
                                           Only relevant when asymmetrical mode. */
    int numq;                           /* number of cos levels to be
                                           created/number of queues in the
                                           created bundle. */
    bcm_cosq_queue_attributes_t queue_atrributes[BCM_COS_COUNT]; /* Config parameters for ingress queue
                                           attributes. */
} bcm_cosq_ingress_queue_bundle_gport_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Create an ingress queue bundle. */
extern int bcm_cosq_ingress_queue_bundle_gport_add(
    int unit, 
    bcm_cosq_ingress_queue_bundle_gport_config_t *config, 
    bcm_gport_t *gport);

/* Get or set the mapping from internal priority to CoS queue. */
extern int bcm_cosq_gport_mapping_set(
    int unit, 
    bcm_port_t ing_port, 
    bcm_cos_t int_pri, 
    uint32 flags, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq);

/* Get or set the mapping from internal priority to CoS queue. */
extern int bcm_cosq_gport_mapping_get(
    int unit, 
    bcm_port_t ing_port, 
    bcm_cos_t int_pri, 
    uint32 flags, 
    bcm_gport_t *gport, 
    bcm_cos_queue_t *cosq);

/* Configure/Retrieve bandwidth setting. */
extern int bcm_cosq_gport_bandwidth_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    uint32 kbits_sec_min, 
    uint32 kbits_sec_max, 
    uint32 flags);

/* Configure/Retrieve bandwidth setting. */
extern int bcm_cosq_gport_bandwidth_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    uint32 *kbits_sec_min, 
    uint32 *kbits_sec_max, 
    uint32 *flags);

/* Configure/retrieve scheduling policy */
extern int bcm_cosq_gport_sched_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    int mode, 
    int weight);

/* Configure/retrieve scheduling policy */
extern int bcm_cosq_gport_sched_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    int *mode, 
    int *weight);

#endif /* BCM_HIDE_DISPATCHABLE */

/* dynamic weights types. */
typedef enum bcm_cosq_dynamic_state_e {
    bcmCosqDynamicStateNormal = 0,      /* Regular weight */
    bcmCosqDynamicStateQueue0Congested = 1, /* Pipe 0 is congested, Pipe 1 not
                                           congested */
    bcmCosqDynamicStateQueue1Congested = 2, /* Pipe 1 is congested, Pipe 0 not
                                           congested */
    bcmCosqDynamicStateCongested = 3    /* Weight for congested pipe */
} bcm_cosq_dynamic_state_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* set/get weights dynamic or regular weights */
extern int bcm_cosq_gport_dynamic_sched_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_cosq_dynamic_state_t state, 
    int weight);

/* set/get weights dynamic or regular weights */
extern int bcm_cosq_gport_dynamic_sched_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_cosq_dynamic_state_t state, 
    int *weight);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Discard rules. */
typedef struct bcm_cosq_discard_rule_s {
    uint32 egress_pool_congestion_state; /* Pool congestion state. */
    uint32 queue_min_congestion_state;  /* Q min use state. */
    uint32 action;                      /* Discard action. */
} bcm_cosq_discard_rule_t;

#define BCM_COSQ_DISCARD_CONGESTION_LOW     0          
#define BCM_COSQ_DISCARD_CONGESTION_MEDIUM  1          
#define BCM_COSQ_DISCARD_CONGESTION_HIGH    2          

#define BCM_COSQ_DISCARD_ACTION_WRED_DROP   0          
#define BCM_COSQ_DISCARD_ACTION_FORCE_DROP  1          
#define BCM_COSQ_DISCARD_ACTION_FORCE_NO_DROP 2          

/* Initialize a discard rule structure. */
extern void bcm_cosq_discard_rule_t_init(
    bcm_cosq_discard_rule_t *rule);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create an empty discard rule profile. */
extern int bcm_cosq_discard_profile_create(
    int unit, 
    int *profile_id);

/* Destroy a discard rule profile. */
extern int bcm_cosq_discard_profile_destroy(
    int unit, 
    int profile_id);

/* Update one rule in a profile. */
extern int bcm_cosq_discard_profile_update(
    int unit, 
    int profile_id, 
    bcm_cosq_discard_rule_t *rule);

extern int bcm_cosq_discard_profile_get(
    int unit, 
    int profile_id, 
    int max, 
    bcm_cosq_discard_rule_t *rule_array, 
    int *rule_count);

/* Configure/Retrieve discard setting. */
extern int bcm_cosq_gport_discard_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_cosq_gport_discard_t *discard);

/* Configure/Retrieve discard setting. */
extern int bcm_cosq_gport_discard_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_cosq_gport_discard_t *discard);

/* Configure/Retrieve discard setting. */
extern int bcm_cosq_gport_discard_extended_set(
    int unit, 
    bcm_cosq_object_id_t *id, 
    bcm_cosq_gport_discard_t *discard);

/* Configure/Retrieve discard setting. */
extern int bcm_cosq_gport_discard_extended_get(
    int unit, 
    bcm_cosq_object_id_t *id, 
    bcm_cosq_gport_discard_t *discard);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_COSQ_SCHED_CONFIG_EXPEDITE  0x00000001 

#ifndef BCM_HIDE_DISPATCHABLE

/* Mapping/redirecting scheduling discipline to Egress FIFO. */
extern int bcm_cosq_gport_sched_config_set(
    int unit, 
    bcm_gport_t gport, 
    int sched_mode, 
    int int_pri, 
    uint32 flags);

/* Mapping/redirecting scheduling discipline to Egress FIFO. */
extern int bcm_cosq_gport_sched_config_get(
    int unit, 
    bcm_gport_t gport, 
    int sched_mode, 
    int int_pri, 
    uint32 *flags);

/* 
 * Configure/retrieve mapping of fabric egress cos level to front panel
 * cos level.
 */
extern int bcm_cosq_gport_flow_control_set(
    int unit, 
    bcm_gport_t port, 
    bcm_cos_t int_pri, 
    uint32 flow_control_mask);

/* 
 * Configure/retrieve mapping of fabric egress cos level to front panel
 * cos level.
 */
extern int bcm_cosq_gport_flow_control_get(
    int unit, 
    bcm_gport_t port, 
    bcm_cos_t int_pri, 
    uint32 *flow_control_mask);

/* Attach/Detach the output of a GPORT to the input of a scheduler GPORT. */
extern int bcm_cosq_gport_attach(
    int unit, 
    bcm_gport_t sched_port, 
    bcm_gport_t input_port, 
    bcm_cos_queue_t cosq);

/* Attach/Detach the output of a GPORT to the input of a scheduler GPORT. */
extern int bcm_cosq_gport_detach(
    int unit, 
    bcm_gport_t sched_port, 
    bcm_gport_t input_port, 
    bcm_cos_queue_t cosq);

/* Attach/Detach the output of a GPORT to the input of a scheduler GPORT. */
extern int bcm_cosq_gport_attach_get(
    int unit, 
    bcm_gport_t sched_port, 
    bcm_gport_t *input_port, 
    bcm_cos_queue_t *cosq);

/* 
 * Get the child node GPORT atatched to N-th index (cosq) of the
 * scheduler GPORT.
 */
extern int bcm_cosq_gport_child_get(
    int unit, 
    bcm_gport_t in_gport, 
    bcm_cos_queue_t cosq, 
    bcm_gport_t *out_gport);

/* retrieve parent scheduler for a child gport */
extern int bcm_cosq_gport_parent_get(
    int unit, 
    bcm_gport_t child_port, 
    bcm_cos_queue_t cos, 
    bcm_gport_t *parent_port);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_COSQ_GPORT_QUEUE_ATTACH_WITH_ID 0x00000001 
#define BCM_COS_MULTICAST_EF                -2         
#define BCM_COS_MULTICAST_NON_EF            -3         

#define BCM_COSQ_GPORT_ATTACH_ID_SYSPORT_SET(_attach_id, _sysport)  \
    _SHR_COSQ_GPORT_ATTACH_ID_SYSPORT_SET(_attach_id,_sysport) 

#ifndef BCM_HIDE_DISPATCHABLE

extern int bcm_cosq_gport_queue_attach(
    int unit, 
    uint32 flags, 
    bcm_gport_t ingress_queue, 
    bcm_cos_t ingress_int_pri, 
    bcm_gport_t egress_queue, 
    bcm_cos_t egress_int_pri, 
    int *attach_id);

extern int bcm_cosq_gport_queue_detach(
    int unit, 
    bcm_gport_t ingress_queue, 
    bcm_cos_t ingress_int_pri, 
    int attach_id);

extern int bcm_cosq_gport_queue_attach_get(
    int unit, 
    bcm_gport_t ingress_queue, 
    bcm_cos_t ingress_int_pri, 
    bcm_gport_t *egress_queue, 
    bcm_cos_t *egress_int_pri, 
    int attach_id);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_COSQ_GPORT_CONNECTION_INGRESS   0x01       /* establishing the
                                                          connection on ingress */
#define BCM_COSQ_GPORT_CONNECTION_EGRESS    0x02       /* establishing the
                                                          connection on egress */
#define BCM_COSQ_GPORT_CONNECTION_INVALID   0x04       /* removing the
                                                          connection */
#define BCM_COSQ_GPORT_CONNECTION_MODID_OVERRIDE 0x08       

/* connection information */
typedef struct bcm_cosq_gport_connection_s {
    uint32 flags; 
    bcm_module_t remote_modid;  /* remote module id. */
    bcm_gport_t voq;            /* voq gport */
    bcm_gport_t voq_connector;  /* voq connector gport */
} bcm_cosq_gport_connection_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* attaches VoQ connector to the VoQ */
extern int bcm_cosq_gport_connection_set(
    int unit, 
    bcm_cosq_gport_connection_t *gport_connect);

/* attaches VoQ connector to the VoQ */
extern int bcm_cosq_gport_connection_get(
    int unit, 
    bcm_cosq_gport_connection_t *gport_connect);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_COSQ_DELAY_TOLERANCE_SLOW_LEVELS 7          /* Number of slow
                                                          threshold levels. */

/* delay tolerance configuration */
typedef struct bcm_cosq_delay_tolerance_s {
    int credit_request_watchdog_status_msg_gen; /* Watch Dog Status Message Generation
                                           Period. When the Watch Dog Status
                                           Message Generation Period time passed
                                           since the last credit for an active
                                           Queue, the Queue will generate flow
                                           status message again. Range depends
                                           on mode. ARAD A supports 1-15 in
                                           aggressive status message credit
                                           watchdog mode, and 33-500 in the
                                           normal mode. (0 disables TH).
                                           Resolution: WD full cycle units. */
    int credit_request_watchdog_delete_queue_thresh; /* Watch Dog Delete Queue Threshold.
                                           When the Watch Dog Delete Queue
                                           Threshold time passed since the last
                                           credit for an active Queue, the Queue
                                           will be in Delete State. Range:
                                           100-500. (0 disables TH).  Not
                                           supported in aggressive status
                                           message credit watchdog mode of ARAD
                                           A. Units: milliseconds. Resolution:
                                           WD full cycle units. */
    int credit_request_hungry_off_to_slow_thresh; /* When (QsizeCrBal) >
                                           off_to_slow_thresh, and CRS==OFF, CRS
                                           may be changed to SLOW. Resolution in
                                           bytes. Range: (-229376) - (229376).
                                           Resolution: 1 Byte. */
    int credit_request_hungry_off_to_normal_thresh; /* When (QsizeCrBal) >
                                           off_to_normal_thresh, and CRS==OFF,
                                           CRS may be changed to NORM.
                                           Resolution in bytes. Range: (-229376)
                                           - (229376). Resolution: 1 Byte. */
    int credit_request_hungry_slow_to_normal_thresh; /* When (QsizeCrBal) >
                                           slow_to_normal_thresh, and CRS==SLOW,
                                           CRS may be changed to NORM.
                                           Resolution in bytes.  Range:
                                           (-229376) - (229376). Resolution: 1
                                           Byte. */
    int credit_request_hungry_normal_to_slow_thresh; /* When (QsizeCrBal) <
                                           normal_to_slow_th, and CRS==NORM, CRS
                                           may be changed to SLOW. Resolution in
                                           bytes. Range: (-229376) - (229376).
                                           Resolution: 1 Byte. */
    int credit_request_hungry_multiplier; /* When queue size crosses a multiply of
                                           multiplier a new FlowStatus Cell is
                                           generated. Resolution in bytes.
                                           Range: 2^5  2^12. (2^4  disables
                                           mechanism). */
    int credit_request_satisfied_backoff_enter_thresh; /* When the Credit Balance is bigger
                                           than the threshold, the device sends
                                           OFF message to the scheduler. This is
                                           to prevent credit accumulation from a
                                           certain threshold. Resolution in
                                           bytes. Range: 0  491520. Resolution:
                                           1 Byte. */
    int credit_request_satisfied_backoff_exit_thresh; /* Hysteresis value for to the Backoff
                                           Enter. Resolution in bytes. Range: 0 
                                           491520. Resolution: 1 Byte. */
    int credit_request_satisfied_backlog_enter_thresh; /* When the Credit Balance is bigger
                                           Backlog Enter threshold bytes than
                                           the queue size, the device sends OFF
                                           message to the scheduler. This
                                           threshold acts as Slow/Norm to off
                                           threshold. Resolution in bytes.
                                           Range: 0  491520. Resolution: 1 Byte. */
    int credit_request_satisfied_backlog_exit_thresh; /* Hysteresis value for to the Backlog
                                           Exit. Resolution in bytes. Range: 0 
                                           491520. Resolution: 1 Byte. */
    int credit_request_satisfied_backslow_enter_thresh; /* Hysteresis value for to the Backslow
                                           Enter. Resolution in bytes. Range: 0 
                                           491520. Resolution: 1 Byte. */
    int credit_request_satisfied_backslow_exit_thresh; /* Hysteresis value for to the Backslow
                                           Exit. Resolution in bytes. Range: 0 
                                           491520. Resolution: 1 Byte. */
    int credit_request_satisfied_empty_queue_thresh; /* When Qsize==0 and CrBal >=
                                           satisfied_empty_q_th, the CRS is
                                           changed to OFF. This threshold is a
                                           specific case of
                                           PETRA_ITM_CR_REQUEST_CR_BALANCE_TH.
                                           backlog_enter_th threshold.
                                           Resolution in bytes. Range: 32768 
                                           32767. */
    int credit_request_satisfied_empty_queue_max_balance_thresh; /* Max Empty Queue Credit Balance. This
                                           value is the maximum credits an empty
                                           queue can accumulate. Resolution in
                                           bytes. Range: 32768  32767. */
    int credit_request_satisfied_empty_queue_exceed_thresh; /* Exceed Max Empty Queue Credit
                                           Balance. This indication permits the
                                           credit balance of an empty queue to
                                           exceed configured Exceed Max Empty
                                           Queue Credit Balance up to (Credit
                                           Value minus 1), when a credit is
                                           received. This is used to prevent the
                                           deletion of partial credits. */
    uint32 flags;                       /* flags for the API */
    int slow_level_thresh_up[BCM_COSQ_DELAY_TOLERANCE_SLOW_LEVELS]; /* Slow level thresholds upwards */
    int slow_level_thresh_down[BCM_COSQ_DELAY_TOLERANCE_SLOW_LEVELS]; /* Slow level thresholds downwards */
    int bandwidth_level;                /* Level of rate (port rate) */
} bcm_cosq_delay_tolerance_t;

/* 
 * tolerance level related flags specified in the flags field of
 * bcm_cosq_delay_tolerance_t.
 */
#define BCM_COSQ_DELAY_TOLERANCE_REMOTE_CREDIT_VALUE 0x1        /* Specifies that the
                                                          profile works with the
                                                          remote credit value,
                                                          works only in specific
                                                          device types */
#define BCM_COSQ_DELAY_TOLERANCE_IS_LOW_LATENCY 0x2        /* Specifies that the
                                                          profile is for low
                                                          latency queues
                                                          enabling multiple
                                                          packet dequeues */
#define BCM_COSQ_DELAY_TOLERANCE_HIGH_Q_PRIORITY 0x4        /* Specifies that the
                                                          profile is for high
                                                          priority queue. */
#define BCM_COSQ_DELAY_TOLERANCE_TOLERANCE_OCB_ONLY 0x8        /* Specifies that the
                                                          profile is for
                                                          OCB-only queues. */
#define BCM_COSQ_DELAY_TOLERANCE_BANDWIDTH_PROFILE_HIGH 0x10       /* Set a queue MAX
                                                          command credits
                                                          configurations for
                                                          high bandwidth
                                                          traffic. */
#define BCM_COSQ_DELAY_TOLERANCE_PUSH_QUEUE 0x20       /* Specifies that the
                                                          profile is for push
                                                          queue. */

#ifndef BCM_HIDE_DISPATCHABLE

/* Configures delay tolerance scheduling disciplines */
extern int bcm_cosq_delay_tolerance_level_set(
    int unit, 
    int delay_tolerance_level, 
    bcm_cosq_delay_tolerance_t *delay_tolerance);

/* Configures delay tolerance scheduling disciplines */
extern int bcm_cosq_delay_tolerance_level_get(
    int unit, 
    int delay_tolerance_level, 
    bcm_cosq_delay_tolerance_t *delay_tolerance);

#endif /* BCM_HIDE_DISPATCHABLE */

/* attributes use to get a set of recommended delay tolerance values */
typedef struct bcm_cosq_delay_tolerance_preset_attr_s {
    int rate;           /* approximately the desired credit rate in Gbps */
    int credit_size;    /* egress device credit size in bytes */
    uint32 flags;       /* additional attributes -
                           BCM_COSQ_DELAY_TOLERANCE_PRESET_ATTR_F_XXX */
} bcm_cosq_delay_tolerance_preset_attr_t;

/* attributes used by bcm_cosq_delay_tolerance_preset_attr_t.flags */
#define BCM_COSQ_DELAY_TOLERANCE_PRESET_ATTR_F_HIGH_PRIORITY 0x1        /* used to get
                                                          recommended values for
                                                          high priority queues */
#define BCM_COSQ_DELAY_TOLERANCE_PRESET_ATTR_F_SLOW_ENABLED 0x2        /* used to get
                                                          recommended values
                                                          with slow mechanism
                                                          enabled */
#define BCM_COSQ_DELAY_TOLERANCE_PRESET_ATTR_F_OCB_ONLY 0x4        /* used to get
                                                          recommended values for
                                                          OCB only queues. */
#define BCM_COSQ_DELAY_TOLERANCE_PRESET_ATTR_F_MULTICAST 0x8        /* used to get
                                                          recommended values for
                                                          FMQs (Fabric Multicast
                                                          Queues). */
#define BCM_COSQ_DELAY_TOLERANCE_PRESET_ATTR_F_PUSH_QUEUE 0x10       /* Used to get parameters
                                                          for push queue
                                                          profile. */

#ifndef BCM_HIDE_DISPATCHABLE

/* Get recommended set of delay tolerance thresholds */
extern int bcm_cosq_delay_tolerance_preset_get(
    int unit, 
    bcm_cosq_delay_tolerance_preset_attr_t *preset_attr, 
    bcm_cosq_delay_tolerance_t *delay_tolerance);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a preset attributes structure. */
extern void bcm_cosq_delay_tolerance_preset_attr_t_init(
    bcm_cosq_delay_tolerance_preset_attr_t *preset_attr);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Configure/retrieve {traffic class, dp} mapping to egress queue on a
 * port.
 */
extern int bcm_cosq_gport_egress_map_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_t ingress_pri, 
    bcm_color_t ingress_dp, 
    bcm_cos_queue_t offset);

/* 
 * Configure/retrieve {traffic class, dp} mapping to egress queue on a
 * port.
 */
extern int bcm_cosq_gport_egress_map_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_t ingress_pri, 
    bcm_color_t ingress_dp, 
    bcm_cos_queue_t *offset);

#endif /* BCM_HIDE_DISPATCHABLE */

/* flag values for multicast config setting */
#define BCM_COSQ_MULTICAST_SCHEDULED    0x00000001 /* multicast scheduled values
                                                      are valid */
#define BCM_COSQ_MULTICAST_UNSCHEDULED  0x00000002 /* multicast unscheduled
                                                      values are valid */

/* multicast configuration settings */
typedef enum bcm_cosq_egress_multicast_service_pools_e {
    bcmCosqEgressMulticastServicePool0 = 0, /* Service pool 0 identifier */
    bcmCosqEgressMulticastServicePool1 = 1 /* Service pool 1 identifier */
} bcm_cosq_egress_multicast_service_pools_t;

/* multicast configuration settings */
typedef struct bcm_cosq_egress_multicast_config_s {
    bcm_color_t scheduled_dp;   /* drop precedence */
    bcm_cos_t priority;         /* TC */
    int unscheduled_se;         /* shared resource eligibility */
    int unscheduled_sp;         /* service pool ID */
} bcm_cosq_egress_multicast_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Configure/retrieve {traffic class, dp} mapping to multicast egress
 * configuration on a port.
 */
extern int bcm_cosq_gport_egress_multicast_config_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_t ingress_pri, 
    bcm_color_t ingress_dp, 
    uint32 flags, 
    bcm_cosq_egress_multicast_config_t *config);

/* 
 * Configure/retrieve {traffic class, dp} mapping to multicast egress
 * configuration on a port.
 */
extern int bcm_cosq_gport_egress_multicast_config_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_t ingress_pri, 
    bcm_color_t ingress_dp, 
    uint32 flags, 
    bcm_cosq_egress_multicast_config_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* flag values for VSQ creation */
#define BCM_COSQ_VSQ_GL         1          /* create 1 global VSQ */
#define BCM_COSQ_VSQ_CT         2          /* create according to Category */
#define BCM_COSQ_VSQ_CTTC       3          /* create according to Category *
                                              Traffic-Class */
#define BCM_COSQ_VSQ_CTCC       4          /* create according to Category *
                                              Connection-Class */
#define BCM_COSQ_VSQ_PP         5          /* create according to
                                              ST-VSQ-Pointer, arrived from the
                                              field processor or external PP */
#define BCM_COSQ_VSQ_LLFC       6          /* create according to Source Network
                                              Port */
#define BCM_COSQ_VSQ_PFC        7          /* create according to Packet
                                              Priority */

/* VSQ info settings */
typedef struct bcm_cosq_vsq_info_s {
    uint32 flags;           /* flags for vsq info */
    int ct_id;              /* Category ID. Relevant when flag is CT/CTTC/CTCC */
    int cc_id;              /* Connection Class ID. Relevant when flag is CTCC */
    int traffic_class;      /* VSQ Traffic Class ID. The VSQ traffic class ID
                               only relevant for the VSQ mapping and doesn't
                               remap the system TC. Relevant when flag is CTTC */
    bcm_gport_t src_port;   /* Source Physical port ID. The network interface ID
                               that the packet arrived from. Relevant when flag
                               is LLFC, PFC */
    int pp_vsq_id;          /* The ID of the VSQ, in the PP space. Relevant when
                               flag PP is set. In this case, the user specifies
                               the ST-VSQ-Pointer in the range 0-255 from the
                               FP/PP that needs to be mapped to the VSQ */
    bcm_core_t core_id;     /* The core ID of the VSQ */
} bcm_cosq_vsq_info_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Create the VSQ. */
extern int bcm_cosq_gport_vsq_create(
    int unit, 
    bcm_cosq_vsq_info_t *vsq_info, 
    bcm_gport_t *vsq_gport);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_COSQ_VSQ_NOF_PG     8          /* The Maximum number of PGs in
                                              bundle. */

/* Config parameters for PG VSQs attributes. */
typedef struct bcm_cosq_pg_vsq_attributes_s {
    uint8 cosq_bitmap;  /* a bitmap of the priorities/TCs (0-7) that are
                           included in the PG */
    uint8 ocb_only;     /* is OCB-only (otherwise it is DRAM-mix. */
    uint8 pool_id;      /* pool ID, can be is 0 or 1 */
    uint8 is_lossles;   /* is it lossless (eligible to headroom) */
} bcm_cosq_pg_vsq_attributes_t;

/* Config parameters for src based VSQs attributes. */
typedef struct bcm_cosq_src_vsqs_gport_config_s {
    uint32 flags; 
    int numq;                           /* the PG size bundle. */
    bcm_cosq_pg_vsq_attributes_t pg_attributes[BCM_COSQ_VSQ_NOF_PG]; /* Config parameters for src based VSQs
                                           attributes. */
} bcm_cosq_src_vsqs_gport_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Create the source based VSQ. */
extern int bcm_cosq_src_vsqs_gport_add(
    int unit, 
    bcm_gport_t port, 
    bcm_cosq_src_vsqs_gport_config_t *config, 
    bcm_gport_t *src_port_vsq_gport, 
    bcm_gport_t *pg_base_vsq_gport);

/* Create the source based VSQ. */
extern int bcm_cosq_src_vsqs_gport_get(
    int unit, 
    bcm_gport_t port, 
    bcm_cosq_src_vsqs_gport_config_t *config, 
    bcm_gport_t *src_port_vsq_gport, 
    bcm_gport_t *pg_base_vsq_gport);

/* Destroy the VSQ. */
extern int bcm_cosq_gport_vsq_destroy(
    int unit, 
    bcm_gport_t vsq_gport);

/* adding/deleting queue to vsq */
extern int bcm_cosq_gport_vsq_add(
    int unit, 
    bcm_gport_t vsq, 
    bcm_gport_t queue, 
    bcm_cos_queue_t cosq);

/* adding/deleting queue to vsq */
extern int bcm_cosq_gport_vsq_delete(
    int unit, 
    bcm_gport_t vsq, 
    bcm_gport_t queue, 
    bcm_cos_queue_t cosq);

/* deleting all queues from vsq */
extern int bcm_cosq_gport_vsq_delete_all(
    int unit, 
    bcm_gport_t vsq);

/* retrieving queues that are part of vsq */
extern int bcm_cosq_gport_vsq_get(
    int unit, 
    bcm_gport_t vsq, 
    int queue_max, 
    bcm_gport_t *queue_array, 
    bcm_cos_queue_t *cosq_array, 
    int *queue_count);

#endif /* BCM_HIDE_DISPATCHABLE */

/* flag values for threshold setting */
#define BCM_COSQ_THRESHOLD_PER_DP           0x00000001 /* dp field meaningful */
#define BCM_COSQ_THRESHOLD_INGRESS          0x00000002 /* device level ingress
                                                          thresholds */
#define BCM_COSQ_THRESHOLD_EGRESS           0x00000004 /* device level egress
                                                          thresholds */
#define BCM_COSQ_THRESHOLD_UNICAST          0x00000008 /* device level unicast
                                                          thresholds */
#define BCM_COSQ_THRESHOLD_MULTICAST        0x00000010 /* device level multicast
                                                          thresholds */
#define BCM_COSQ_THRESHOLD_SET              0x00000020 /* threshold for assert
                                                          condition (where
                                                          applicable) */
#define BCM_COSQ_THRESHOLD_CLEAR            0x00000040 /* threshold for
                                                          de-assert (where
                                                          applicable) */
#define BCM_COSQ_THRESHOLD_NOT_COMMIT       0x00000080 /* to aid in template
                                                          management */
#define BCM_COSQ_THRESHOLD_FLOW_CONTROL     0x00000100 /* flow control */
#define BCM_COSQ_THRESHOLD_DROP             0x00000200 /* Admission Control */
#define BCM_COSQ_THRESHOLD_MULTICAST_SP0    0x00000400 /* Multicast Service-Pool
                                                          0 */
#define BCM_COSQ_THRESHOLD_MULTICAST_SP1    0x00000800 /* Multicast Service-Pool
                                                          1 */
#define BCM_COSQ_THRESHOLD_COLOR_SYSTEM_RED 0x00001000 /* Set Color System red
                                                          format */
#define BCM_COSQ_THRESHOLD_RANGE_0          0x00002000 /* Set Range 0 threshold,
                                                          where threshold value
                                                          indicates the max size
                                                          of the specific range */
#define BCM_COSQ_THRESHOLD_RANGE_1          0x00004000 /* Set Range 1 threshold,
                                                          where threshold value
                                                          indicates the max size
                                                          of the specific range */
#define BCM_COSQ_THRESHOLD_RANGE_2          0x00008000 /* Set Range 2 threshold,
                                                          where threshold value
                                                          indicates the max size
                                                          of the specific range */
#define BCM_COSQ_THRESHOLD_RANGE_3          0x00010000 /* Set Range 3 threshold,
                                                          where threshold value
                                                          indicates the max size
                                                          of the specific range */
#define BCM_COSQ_THRESHOLD_OCB              0x00020000 /* On-Chip buffer */
#define BCM_COSQ_THRESHOLD_RCI              0x00040000 /* Set threshold value
                                                          for RCI */
#define BCM_COSQ_THRESHOLD_ETH_PORT_LLFC    0x00080000 /* Set LLFC threshold
                                                          value for Ethernet
                                                          port */
#define BCM_COSQ_THRESHOLD_ETH_PORT_PFC     0x00100000 /* Set PFC threshold
                                                          value for Ethernet
                                                          port */
#define BCM_COSQ_THRESHOLD_POOL0            0x00200000 /* Set threshold value
                                                          for shared Pool 0
                                                          resource */
#define BCM_COSQ_THRESHOLD_POOL1            0x00400000 /* Set threshold value
                                                          for shared Pool 1
                                                          resource */
#define BCM_COSQ_THRESHOLD_QSIZE_RECOVERY   0x00800000 /* Queue size recovery
                                                          threshold settings */
#define BCM_COSQ_THRESHOLD_HEADROOM         0x01000000 /* Set threshold value
                                                          for sram headroom
                                                          resource */
#define BCM_COSQ_THRESHOLD_ETH_PORT_HIGH_FIFO 0x02000000 /* Set threshold value
                                                          for High priority RMC */
#define BCM_COSQ_THRESHOLD_ETH_PORT_LOW_FIFO 0x04000000 /* Set threshold value
                                                          for Low priority RMC */
#define BCM_COSQ_THRESHOLD_ETH_PORT_TDM_FIFO 0x08000000 /* Set threshold value
                                                          for TDM priority RMC */

/* threshold type enumerartions */
typedef enum bcm_cosq_threshold_type_e {
    bcmCosqThresholdBytes = _SHR_COSQ_THRESHOLD_BYTES, /* consumed bytes. Egress Drop
                                           threshold, Unicast, multicast, dp
                                           (multicast) at port and device level.
                                           Egress Flow control threshold,
                                           unicast , multicast, port, channel
                                           and device level. */
    bcmCosqThresholdPacketDescriptors = _SHR_COSQ_THRESHOLD_PACKET_DESCRIPTORS, /* consumed packet descriptors. Egress
                                           drop threshold, unicast, multicast,
                                           dp (multicast) at port level. Egress
                                           Flow control threshold, unicast,
                                           multicast device level. */
    bcmCosqThresholdPackets = _SHR_COSQ_THRESHOLD_PACKETS, /* consumed packets. Egress Drop
                                           threshold, Unicast, multicast,
                                           dp(multicast) at device level. Egress
                                           Flow control threshold, multicast at
                                           device level. */
    bcmCosqThresholdDataBuffers = _SHR_COSQ_THRESHOLD_DATA_BUFFERS, /* consumed data buffers. Egress Drop
                                           threshold, multicast, dp at device
                                           level Egress Flow control threshold,
                                           unicast, multicast at device level. */
    bcmCosqThresholdAvailablePacketDescriptors = _SHR_COSQ_THRESHOLD_AVAILABLE_PACKET_DESCRIPTORS, /* available Packet Descriptors. Egress
                                           Drop threshold, Unicast, multicast,
                                           dp (multicast) at port and device
                                           level. */
    bcmCosqThresholdAvailableDataBuffers = _SHR_COSQ_THRESHOLD_AVAILABLE_DATA_BUFFERS, /* available data buffers. Egress Drop
                                           threshold, multicast, dp at device
                                           level. */
    bcmCosqThresholdBufferDescriptorBuffers = _SHR_COSQ_THRESHOLD_BUFFER_DESCRIPTOR_BUFFERS, /* consumed Buffer descriptor Buffers.
                                           Ingress Drop threshold, DP at device
                                           level. Ingress Flow control threshold
                                           at device level. */
    bcmCosqThresholdBufferDescriptors = _SHR_COSQ_THRESHOLD_BUFFER_DESCRIPTORS, /* consumed buffer descriptors. Ingress
                                           Drop threshold, DP at device level. */
    bcmCosqThresholdDbuffs = _SHR_COSQ_THRESHOLD_DBUFFS, /* consumed Dbuffs. Ingress Drop
                                           threshold, DP, unicast at device
                                           level. Ingress flow control
                                           threshold, unicast, multicast at
                                           device level. */
    bcmCosqThresholdFullDbuffs = _SHR_COSQ_THRESHOLD_FULL_DBUFFS, /* consumed Full Dbuffs. Ingress Drop
                                           threshold, DP, multicast at device
                                           level. */
    bcmCosqThresholdMiniDbuffs = _SHR_COSQ_THRESHOLD_MINI_DBUFFS, /* consumed mini Dbuffs. Ingress Drop
                                           threshold, DP, multicast at device
                                           level. */
    bcmCosqThresholdDynamicWeight = _SHR_COSQ_THRESHOLD_DYNAMIC_WEIGHT, /* Dynamic scheduling threshold. */
    bcmCosqThresholdPacketDescriptorsMin = _SHR_COSQ_THRESHOLD_PACKET_DESCRIPTORS_MIN, /* Fair Adaptive Thresholds support for
                                           bcmCosqThresholdPacketDescriptors. */
    bcmCosqThresholdPacketDescriptorsMax = _SHR_COSQ_THRESHOLD_PACKET_DESCRIPTORS_MAX, /* Fair Adaptive Thresholds support for
                                           bcmCosqThresholdPacketDescriptors. */
    bcmCosqThresholdPacketDescriptorsAlpha = _SHR_COSQ_THRESHOLD_PACKET_DESCRIPTORS_ALPHA, /* Fair Adaptive Thresholds support for
                                           bcmCosqThresholdPacketDescriptors. */
    bcmCosqThresholdDataBuffersMin = _SHR_COSQ_THRESHOLD_DATA_BUFFERS_MIN, /* Fair Adaptive Thresholds support for
                                           bcmCosqThresholdDataBuffers. */
    bcmCosqThresholdDataBuffersMax = _SHR_COSQ_THRESHOLD_DATA_BUFFERS_MAX, /* Fair Adaptive Thresholds support for
                                           bcmCosqThresholdDataBuffers. */
    bcmCosqThresholdDataBuffersAlpha = _SHR_COSQ_THERESHOLD_DATA_BUFFERS_ALPHA, /* Fair Adaptive Thresholds support for
                                           bcmCosqThresholdDataBuffers. */
    bcmCosqThresholdOcbDbuffs = _SHR_COSQ_THRESHOLD_OCB_DBUFFS, /* consumed OCB Dbuffs. Ingress Drop
                                           threshold, DP, at device level. */
    bcmCosqThresholdPacketDescriptorBuffers = _SHR_COSQ_THRESHOLD_PACKET_DESCRIPTOR_BUFFERS, /* consumed PDBs. Ingress Drop
                                           threshold, DP, at device level. */
    bcmCosqThresholdBundleDescriptorBuffers = _SHR_COSQ_THRESHOLD_BUNDLE_DESCRIPTOR_BUFFERS, /* consumed Bundle Descriptor Buffers.
                                           Ingress Drop threshold, DP, at device
                                           level. */
    bcmCosqThresholdDataBuffersAlpha1 = _SHR_COSQ_THERESHOLD_DATA_BUFFERS_ALPHA1, /* Additional Alpha factor for Fair
                                           Adaptive Thresholds for
                                           bcmCosqThresholdDataBuffers. */
    bcmCosqThresholdOcbPacketDescriptorBuffers = _SHR_COSQ_THRESHOLD_OCB_PACKET_DESCRIPTOR_BUFFERS /* OCB packet descriptor buffers. */
} bcm_cosq_threshold_type_t;

/* flag values for threshold setting valid bitmap */
#define BCM_COSQ_THRESHOLD_VALID_DP         0x00000001 /* dp field should be be
                                                          used */
#define BCM_COSQ_THRESHOLD_VALID_VALUE      0x00000002 /* value field should be
                                                          be used */
#define BCM_COSQ_THRESHOLD_VALID_TC         0x00000004 /* tc field should be be
                                                          used */
#define BCM_COSQ_THRESHOLD_VALID_VSQ_CATEGORY 0x00000008 /* vsq category field
                                                          should be be used */

/* flow control / addmission settings */
typedef struct bcm_cosq_threshold_s {
    uint32 flags;                   /* BCM_COSQ_THRESHOLD_* flag values */
    bcm_color_t dp;                 /* drop precedence. relevant for
                                       BCM_COSQ_THRESHOLD_PER_DP flag value */
    bcm_cosq_threshold_type_t type; /* threshold type */
    int value;                      /* threshold value for drop/flow control */
    int tc;                         /* traffic class for which the thresholds
                                       are to be configured */
    int vsq_category;               /* VSQ category for which the thresholds are
                                       to be configured */
    uint32 valid;                   /* a bitmap representing enabled fields in
                                       the struct. */
    int priority;                   /* priority */
} bcm_cosq_threshold_t;

/* Initialize a COSQ threshold structure. */
extern void bcm_cosq_threshold_t_init(
    bcm_cosq_threshold_t *threshold);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Configuring/retrieving Ingress / egress Thresholds for Flow control
 * and Admission Control
 */
extern int bcm_cosq_gport_threshold_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_cosq_threshold_t *threshold);

/* 
 * Configuring/retrieving Ingress / egress Thresholds for Flow control
 * and Admission Control
 */
extern int bcm_cosq_gport_threshold_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_cosq_threshold_t *threshold);

#endif /* BCM_HIDE_DISPATCHABLE */

/* cosq gport type eumerations */
typedef enum bcm_cosq_gport_type_e {
    bcmCosqGportTypeE2EPort = 0,        /* Ports in E2E hierarchy */
    bcmCosqGportTypeUnicastEgress = 1,  /* Unicast Egress Queues - Port
                                           hierarchy */
    bcmCosqGportTypeMulticastEgress = 2, /* Multicast Egress Queues - Port
                                           Hierarchy */
    bcmCosqGportTypeCompositeFlow2 = 3, /* 2nd  flow of either composite
                                           connector or composite SE */
    bcmCosqGportTypeSchedCIR = 4,       /* SE(scheduling Element) of CIR type.
                                           Applicable for Dual Shaper SE */
    bcmCosqGportTypeSchedPIR = 5,       /* SE(scheduling Element) of CIR type.
                                           Applicable for Dual Shaper SE */
    bcmCosqGportTypeLocalPort = 6,      /* Ports in Egress transmit hierarchy */
    bcmCosqGportTypeLocalPortTC = 7,    /* Port TC level in Egress transmit
                                           hierarchy */
    bcmCosqGportTypeLocalPortTCG = 8,   /* Port TC Group level in Egress
                                           transmit hierarchy */
    bcmCosqGportTypeE2EPortTC = 9,      /* Port TC level in End-to-End hierarchy */
    bcmCosqGportTypeE2EPortTCG = 10,    /* Port TC Group level in End-to-End
                                           hierarchy */
    bcmCosqGportTypeGlobalFmqRoot = 11, /* FMQ Root gport. TypeGlobal indicates
                                           single instance */
    bcmCosqGportTypeGlobalFmqGuaranteed = 12, /* FMQ Guaranteed gport */
    bcmCosqGportTypeGlobalFmqBestEffortAggregate = 13, /* FMQ Best Effort aggregate gport */
    bcmCosqGportTypeGlobalFmqBestEffort0 = 14, /* FMQ Best Effort 0 gport */
    bcmCosqGportTypeGlobalFmqBestEffort1 = 15, /* FMQ Best Effort 1 gport */
    bcmCosqGportTypeGlobalFmqBestEffort2 = 16, /* FMQ Best Effort 2 gport */
    bcmCosqGportTypeGlobalFmqClass1 = 17, /* FMQ Class 1 gport */
    bcmCosqGportTypeGlobalFmqClass2 = 18, /* FMQ Class 2 gport */
    bcmCosqGportTypeGlobalFmqClass3 = 19, /* FMQ Class 3 gport */
    bcmCosqGportTypeGlobalFmqClass4 = 20, /* FMQ Class 4 gport */
    bcmCosqGportTypeGlobalIsqRoot = 21, /* ISQ Root gport */
    bcmCosqGportTypeGlobalFabricMeshLocal = 22, /* Ingress Mesh Scheduler - Local */
    bcmCosqGportTypeGlobalFabricMeshDev1 = 23, /* Ingress Mesh Scheduler - Destination
                                           Device 1 */
    bcmCosqGportTypeGlobalFabricMeshDev2 = 24, /* Ingress Mesh Scheduler - Destination
                                           Device 2 */
    bcmCosqGportTypeGlobalFabricMeshDev3 = 25, /* Ingress Mesh Scheduler - Destination
                                           Device 3 */
    bcmCosqGportTypeGlobalFabricMeshDev4 = 26, /* Ingress Mesh Scheduler - Destination
                                           Device 4 */
    bcmCosqGportTypeGlobalFabricMeshDev5 = 27, /* Ingress Mesh Scheduler - Destination
                                           Device 5 */
    bcmCosqGportTypeGlobalFabricMeshDev6 = 28, /* Ingress Mesh Scheduler - Destination
                                           Device 6 */
    bcmCosqGportTypeGlobalFabricMeshDev7 = 29, /* Ingress Mesh Scheduler - Destination
                                           Device 7 */
    bcmCosqGportTypeGlobalFabricClosUnicastLocal = 30, /* Ingress Clos Fabric scheduler -
                                           shaper e (Ref Arch Spec) */
    bcmCosqGportTypeGlobalFabricClosUnicastLocalLow = 31, /* Ingress Clos Fabric scheduler - WFQ 3
                                           (Ref Arch Spec) */
    bcmCosqGportTypeGlobalFabricClosUnicastLocalHigh = 32, /* Ingress Clos Fabric scheduler -
                                           shaper a and WFQ 2 (Ref Arch Spec) */
    bcmCosqGportTypeGlobalFabricClosFabric = 33, /* Ingress Clos Fabric scheduler -
                                           shaper f (Ref Arch Spec) */
    bcmCosqGportTypeGlobalFabricClosFabricHigh = 34, /* Ingress Clos Fabric scheduler - WFQ 2
                                           (Ref Arch Spec) */
    bcmCosqGportTypeGlobalFabricClosFabricLow = 35, /* Ingress Clos Fabric scheduler - WFQ 3
                                           (Ref Arch Spec) */
    bcmCosqGportTypeGlobalFabricClosUnicastFabricHigh = 36, /* Ingress Clos Fabric scheduler -
                                           shaper b and WFQ 4 (Ref Arch Spec) */
    bcmCosqGportTypeGlobalFabricClosUnicastFabricLow = 37, /* Ingress Clos Fabric scheduler - WFQ 5
                                           (Ref Arch Spec) */
    bcmCosqGportTypeGlobalFabricClosFmqGuaranteed = 38, /* Ingress Clos Fabric scheduler -
                                           shaper c and WFQ 4 (Ref Arch Spec) */
    bcmCosqGportTypeGlobalFabricClosFmqBestEffort = 39, /* Ingress Clos Fabric scheduler - WFQ 5
                                           (Ref Arch Spec) */
    bcmCosqGportTypeSched = 40,         /* SE(scheduling Element) */
    bcmCosqGportTypeGlobalSystemRed = 41, /* System RED gport */
    bcmCosqGportTypeFabricPipe = 42,    /* Fabric all pipes */
    bcmCosqGportTypeFabricPipeEgress = 43, /* Fabric egress pipes */
    bcmCosqGportTypeFabricPipeIngress = 44, /* Fabric ingress pipes */
    bcmCosqGportTypeFabricPipeMiddle = 45, /* Fabric middle pipes */
    bcmCosqGportTypeGlobalFabricMeshCommonLocal0 = 46, /* Mesh common local 0 */
    bcmCosqGportTypeGlobalFabricMeshCommonLocal1 = 47, /* Mesh common local 1 */
    bcmCosqGportTypeGlobalFabricMeshCommonDev1 = 48, /* Mesh common dev 1 */
    bcmCosqGportTypeGlobalFabricMeshCommonDev2 = 49, /* Mesh common dev 2 */
    bcmCosqGportTypeGlobalFabricMeshCommonDev3 = 50, /* Mesh common dev 3 */
    bcmCosqGportTypeGlobalFabricMeshCommonMc = 51, /* Mesh common multicast */
    bcmCosqGportTypeGlobalFabricClosCommonLocal0 = 52, /* Clos common local 0 */
    bcmCosqGportTypeGlobalFabricClosCommonLocal1 = 53, /* Clos common local 1 */
    bcmCosqGportTypeGlobalFabricClosCommonFabric = 54, /* Clos common fabric */
    bcmCosqGportTypeGlobalFabricClosCommonUnicastFabric = 55, /* Clos common unicast fabric */
    bcmCosqGportTypeGlobalFabricClosCommonMulticastFabric = 56, /* Clos common multicast fabric */
    bcmCosqGportTypeGlobalFabricMeshLocal0OcbHigh = 57, /* Mesh local 0 ocb high */
    bcmCosqGportTypeGlobalFabricMeshLocal0OcbLow = 58, /* Mesh local 0 ocb low */
    bcmCosqGportTypeGlobalFabricMeshLocal0MixHigh = 59, /* Mesh local 0 mix high */
    bcmCosqGportTypeGlobalFabricMeshLocal0MixLow = 60, /* Mesh local 0 mix low */
    bcmCosqGportTypeGlobalFabricMeshLocal1OcbHigh = 61, /* Mesh local 1 ocb high */
    bcmCosqGportTypeGlobalFabricMeshLocal1OcbLow = 62, /* Mesh local 1 ocb low */
    bcmCosqGportTypeGlobalFabricMeshLocal1MixHigh = 63, /* Mesh local 1 mix high */
    bcmCosqGportTypeGlobalFabricMeshLocal1MixLow = 64, /* Mesh local 1 mix low */
    bcmCosqGportTypeGlobalFabricMeshDev1OcbHigh = 65, /* Mesh device 1 ocb high */
    bcmCosqGportTypeGlobalFabricMeshDev1OcbLow = 66, /* Mesh device 1 ocb low */
    bcmCosqGportTypeGlobalFabricMeshDev1MixHigh = 67, /* Mesh device 1 mix high */
    bcmCosqGportTypeGlobalFabricMeshDev1MixLow = 68, /* Mesh device 1 mix low */
    bcmCosqGportTypeGlobalFabricMeshDev2OcbHigh = 69, /* Mesh device 2 ocb high */
    bcmCosqGportTypeGlobalFabricMeshDev2OcbLow = 70, /* Mesh device 2 ocb low */
    bcmCosqGportTypeGlobalFabricMeshDev2MixHigh = 71, /* Mesh device 2 mix high */
    bcmCosqGportTypeGlobalFabricMeshDev2MixLow = 72, /* Mesh device 2 mix low */
    bcmCosqGportTypeGlobalFabricMeshDev3OcbHigh = 73, /* Mesh device 3 ocb high */
    bcmCosqGportTypeGlobalFabricMeshDev3OcbLow = 74, /* Mesh device 3 ocb low */
    bcmCosqGportTypeGlobalFabricMeshDev3MixHigh = 75, /* Mesh device 3 mix high */
    bcmCosqGportTypeGlobalFabricMeshDev3MixLow = 76, /* Mesh device 3 mix low */
    bcmCosqGportTypeGlobalFabricMeshMcOcbHigh = 77, /* Mesh multicast ocb high */
    bcmCosqGportTypeGlobalFabricMeshMcOcbLow = 78, /* Mesh multicast ocb low */
    bcmCosqGportTypeGlobalFabricMeshMcMixHigh = 79, /* Mesh multicast mix high */
    bcmCosqGportTypeGlobalFabricMeshMcMixLow = 80, /* Mesh multicast mix low */
    bcmCosqGportTypeGlobalFabricMeshLocal0 = 81, /* Mesh local 0 */
    bcmCosqGportTypeGlobalFabricMeshLocal1 = 82, /* Mesh local 1 */
    bcmCosqGportTypeGlobalFabricMeshMc = 83, /* Mesh multicast */
    bcmCosqGportTypeGlobalFabricMesh = 84, /* Mesh */
    bcmCosqGportTypeGlobalFabricClosLocal0OcbHigh = 85, /* Clos local 0 ocb high */
    bcmCosqGportTypeGlobalFabricClosLocal0OcbLow = 86, /* Clos local 0 ocb low */
    bcmCosqGportTypeGlobalFabricClosLocal0MixHigh = 87, /* Clos local 0 mix high */
    bcmCosqGportTypeGlobalFabricClosLocal0MixLow = 88, /* Clos local 0 mix low */
    bcmCosqGportTypeGlobalFabricClosLocal1OcbHigh = 89, /* Clos local 1 ocb high */
    bcmCosqGportTypeGlobalFabricClosLocal1OcbLow = 90, /* Clos local 1 ocb low */
    bcmCosqGportTypeGlobalFabricClosLocal1MixHigh = 91, /* Clos local 1 mix high */
    bcmCosqGportTypeGlobalFabricClosLocal1MixLow = 92, /* Clos local 1 mix low */
    bcmCosqGportTypeGlobalFabricClosUnicastFabricOcbHigh = 93, /* Clos unicast ocb high */
    bcmCosqGportTypeGlobalFabricClosUnicastFabricOcbLow = 94, /* Clos unicast ocb low */
    bcmCosqGportTypeGlobalFabricClosUnicastFabricMixHigh = 95, /* Clos unicast mix high */
    bcmCosqGportTypeGlobalFabricClosUnicastFabricMixLow = 96, /* Clos unicast mix low */
    bcmCosqGportTypeGlobalFabricClosFmqGuaranteedOcb = 97, /* Clos guaranteed multicast ocb */
    bcmCosqGportTypeGlobalFabricClosFmqBestEffortOcb = 98, /* Clos best effort multicast ocb */
    bcmCosqGportTypeGlobalFabricClosFmqGuaranteedMix = 99, /* Clos guaranteed multicast mix */
    bcmCosqGportTypeGlobalFabricClosFmqBestEffortMix = 100, /* Clos best effort multicast mix */
    bcmCosqGportTypeGlobalFabricClosLocal0 = 101, /* Clos local 0 */
    bcmCosqGportTypeGlobalFabricClosLocal1 = 102, /* Clos local 1 */
    bcmCosqGportTypeGlobalFabricClosLocal0High = 103, /* Clos local 0 high */
    bcmCosqGportTypeGlobalFabricClosLocal0Low = 104, /* Clos local 0 low */
    bcmCosqGportTypeGlobalFabricClosLocal1High = 105, /* Clos local 1 high */
    bcmCosqGportTypeGlobalFabricClosLocal1Low = 106, /* Clos local 1 low */
    bcmCosqGportTypeGlobalFabricClosOcbHigh = 107, /* Clos ocb high */
    bcmCosqGportTypeGlobalFabricClosMixHigh = 108, /* Clos mix high */
    bcmCosqGportTypeGlobalFabricClosOcbLow = 109, /* Clos ocb low */
    bcmCosqGportTypeGlobalFabricClosMixLow = 110, /* Clos mix low */
    bcmCosqGportTypeGlobalFabricClos = 111, /* Clos */
    bcmCosqGportTypeFabricRxUnicast = 112, /* Fabric unicast FIFO */
    bcmCosqGportTypeFabricRxMulticast = 113, /* Fabric multicast FIFO */
    bcmCosqGportTypeFabricRxTdm = 114,  /* Fabric TDM FIFO */
    bcmCosqGportTypeFabricRxLocalUnicast = 115, /* Local unicast FIFO */
    bcmCosqGportTypeFabricRxLocalMulticast = 116, /* MESH-MC FIFO */
    bcmCosqGportTypeFabricRxLocalTdm = 117, /* Local TDM bypass FIFO */
    bcmCosqGportTypeGlobalFabricMeshDev0 = 118, /* Ingress Mesh Scheduler - Destination
                                           Device 0 */
    bcmCosqGportTypeCore = 119,         /* Gport Type Core */
    bcmCosqGportTypeSchedSharedShaper0 = 120, /* Element at index 0 inside shared
                                           shaper */
    bcmCosqGportTypeSchedSharedShaper1 = 121, /* Element at index 1 inside shared
                                           shaper */
    bcmCosqGportTypeSchedSharedShaper2 = 122, /* Element at index 2 inside shared
                                           shaper */
    bcmCosqGportTypeSchedSharedShaper3 = 123, /* Element at index 3 inside shared
                                           shaper */
    bcmCosqGportTypeSchedSharedShaper4 = 124, /* Element at index 4 inside shared
                                           shaper */
    bcmCosqGportTypeSchedSharedShaper5 = 125, /* Element at index 5 inside shared
                                           shaper */
    bcmCosqGportTypeSchedSharedShaper6 = 126, /* Element at index 6 inside shared
                                           shaper */
    bcmCosqGportTypeSchedSharedShaper7 = 127 /* Element at index 7 inside shared
                                           shaper */
} bcm_cosq_gport_type_t;

/* gport information related to getting a handle */
typedef struct bcm_cosq_gport_info_s {
    bcm_gport_t in_gport;   /* gport to be converted. NULL for global gports
                               (i.e. single instance on a device) */
    bcm_cos_queue_t cosq;   /* Num of cosq levels */
    bcm_gport_t out_gport;  /* retreived gport */
} bcm_cosq_gport_info_t;

/* gport information related to getting a handle */
typedef struct bcm_cosq_gport_info_core_s {
    bcm_cosq_gport_type_t gport_type;   /* cosq gport type enumeration */
    bcm_gport_t in_gport;               /* gport to be converted. NULL for
                                           global gports (i.e. single instance
                                           on a device) */
    bcm_cos_queue_t cosq;               /* Num of cosq levels */
    uint32 flags;                       /* relevant flags */
} bcm_cosq_gport_info_core_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Getting a gport handle */
extern int bcm_cosq_gport_handle_get(
    int unit, 
    bcm_cosq_gport_type_t gport_type, 
    bcm_cosq_gport_info_t *gport_info);

/* Getting a gport handle */
extern int bcm_cosq_gport_handle_core_get(
    int unit, 
    bcm_core_t core, 
    bcm_cosq_gport_info_core_t *gport_info, 
    bcm_gport_t *out_gport);

#endif /* BCM_HIDE_DISPATCHABLE */

/* flow control direction type eumerations */
typedef enum bcm_cosq_fc_direction_type_e {
    bcmCosqFlowControlGeneration = 0,   /* flow control generation */
    bcmCosqFlowControlReception = 1,    /* flow control reception */
    bcmCosqFlowControlInternal = 2      /* internal flow control */
} bcm_cosq_fc_direction_type_t;

/* flag values for flow control path configuration */
#define BCM_COSQ_FC_ENDPOINT_PORT_ALL       0x00000001 /* all ports as endpoint,
                                                          e.g. generate flow
                                                          control on all
                                                          interfaces */
#define BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT  0x00000002 /* endpoint not
                                                          specified. It is
                                                          implicitly specified.
                                                          e.g. for flow control
                                                          generation based on
                                                          in-band VSQ the target
                                                          endpoint is determined
                                                          by the VSQ interface
                                                          attribute */
#define BCM_COSQ_FC_INHERIT_UP              0x00000004 /* also applies flow
                                                          control to all cos
                                                          levels greater then
                                                          that specified via
                                                          flow control endpoint */
#define BCM_COSQ_FC_INHERIT_DOWN            0x00000008 /* also applies flow
                                                          control to all cos
                                                          levels lower then that
                                                          specified via flow
                                                          control endpoint */
#define BCM_COSQ_FC_RETRANSMIT              0x00000010 /* ILKN Retransmit -
                                                          relevant for both FC
                                                          directions */
#define BCM_COSQ_FC_HCFC_BITMAP             0x00000020 /* HCFC Bitmap */
#define BCM_COSQ_FC_PORT_OVER_PFC           0x00000040 /* mapping PFC source to
                                                          port target for FC
                                                          reception */
#define BCM_COSQ_FC_INTF_COSQ_PFC           0x00000080 /* mapping PFC source to
                                                          relevant priorioty in
                                                          all ports on same
                                                          interface for FC
                                                          reception */
#define BCM_COSQ_FC_ETH_PORT                0x00000100 /* FC triggered by RX
                                                          Port FIFO */
#define BCM_COSQ_FC_BDB                     0x00000200 /* FC triggered by BDB
                                                          resource */
#define BCM_COSQ_FC_MINI_DB                 0x00000400 /* FC triggered by
                                                          Mini-Multicast Dbuffs */
#define BCM_COSQ_FC_FULL_DB                 0x00000800 /* FC triggered by
                                                          Full-Multicast Dbuffs */
#define BCM_COSQ_FC_OCB_DB                  0x00001000 /* FC triggered by OCB
                                                          Dbuffs */
#define BCM_COSQ_FC_IS_OCB_ONLY             0x00002000 /* Specifies that FC
                                                          triggered by OCB-only
                                                          Resouce */
#define BCM_COSQ_FC_MUB                     0x00004000 /* FC receive or generate
                                                          via Multi-Use Bit */
#define BCM_COSQ_FC_MASK_POOL_INDICATION    0x00008000 /* Ignore Pool related
                                                          threshold */
#define BCM_COSQ_FC_HEADROOM                0x00010000 /* Specifies that FC
                                                          triggered by Headroom
                                                          Resouce */

/* Endpoint(source or reaction/target) definition */
typedef struct bcm_cosq_fc_endpoint_s {
    uint32 flags;           /* flags to qualify a endpoint */
    bcm_gport_t port;       /* gport identifing the underlying resource */
    bcm_cos_queue_t cosq;   /* cosq/class */
    int calender_index;     /* calender entry */
    int priority;           /* priority. Example, E2E Scheduler Element priority
                               level - for mapping flow control to E2E SE
                               (Scheduler Element) */
} bcm_cosq_fc_endpoint_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* configure flow control path */
extern int bcm_cosq_fc_path_add(
    int unit, 
    bcm_cosq_fc_direction_type_t fc_direction, 
    bcm_cosq_fc_endpoint_t *source, 
    bcm_cosq_fc_endpoint_t *target);

/* retrieve flow control path information */
extern int bcm_cosq_fc_path_get(
    int unit, 
    bcm_cosq_fc_direction_type_t fc_direction, 
    bcm_cosq_fc_endpoint_t *source, 
    int target_max, 
    bcm_cosq_fc_endpoint_t *target, 
    int *target_count);

/* delete flow control path configuration */
extern int bcm_cosq_fc_path_delete(
    int unit, 
    bcm_cosq_fc_direction_type_t fc_direction, 
    bcm_cosq_fc_endpoint_t *source, 
    bcm_cosq_fc_endpoint_t *target);

/* 
 * Set/Unset the traffic from ingress port going to destination module
 * to use virtual destination queues
 */
extern int bcm_cosq_gport_destmod_attach(
    int unit, 
    bcm_gport_t gport, 
    bcm_port_t ingress_port, 
    bcm_module_t dest_modid, 
    int fabric_egress_port);

/* 
 * Set/Unset the traffic from ingress port going to destination module
 * to use virtual destination queues
 */
extern int bcm_cosq_gport_destmod_detach(
    int unit, 
    bcm_gport_t gport, 
    bcm_port_t ingress_port, 
    bcm_module_t dest_modid, 
    int fabric_egress_port);

extern int bcm_cosq_gport_multipath_add(
    int unit, 
    bcm_gport_t multipath_gport, 
    bcm_gport_t member_gport, 
    bcm_cos_queue_t member_gport_cosq);

extern int bcm_cosq_gport_multipath_get(
    int unit, 
    bcm_gport_t multipath_gport, 
    int member_gport_max, 
    bcm_gport_t *member_gport_array, 
    bcm_cos_queue_t *member_gport_cosq_array, 
    int *member_gport_count);

extern int bcm_cosq_gport_multipath_delete(
    int unit, 
    bcm_gport_t multipath_gport, 
    bcm_gport_t member_gport, 
    bcm_cos_queue_t member_gport_cosq);

#endif /* BCM_HIDE_DISPATCHABLE */

/* For queue report thresholds on EPON */
typedef struct bcm_cosq_report_threshold_s {
    int threshold0; /* Queue report threshold0. */
    int threshold1; /* Queue report threshold1. */
    int threshold2; /* Queue report threshold2. */
    int threshold3; /* Queue report threshold3. */
} bcm_cosq_report_threshold_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get/set queue report thresholds */
extern int bcm_cosq_gport_report_threshold_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_cosq_report_threshold_t *threshold);

/* Get/set queue report thresholds */
extern int bcm_cosq_gport_report_threshold_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_cosq_report_threshold_t *threshold);

/* Get or set the mapping from internal priority to CoS egress queue. */
extern int bcm_cosq_gport_egress_mapping_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_t int_pri, 
    bcm_cos_t cos, 
    uint32 flags);

/* Get or set the mapping from internal priority to CoS egress queue. */
extern int bcm_cosq_gport_egress_mapping_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_t *int_pri, 
    bcm_cos_t *cos, 
    uint32 flags);

#endif /* BCM_HIDE_DISPATCHABLE */

/* XGS3 cosq counters */
typedef enum bcm_cosq_stat_e {
    bcmCosqStatDroppedPackets = 0,      /* Dropped packet count. */
    bcmCosqStatDroppedBytes = 1,        /* Dropped byte count. */
    bcmCosqStatYellowCongestionDroppedPackets = 2, /* Dropped yellow packets due to
                                           congestion. */
    bcmCosqStatRedCongestionDroppedPackets = 3, /* Dropped red packets due to
                                           congestion. */
    bcmCosqStatGreenDiscardDroppedPackets = 4, /* WRED green packets. */
    bcmCosqStatYellowDiscardDroppedPackets = 5, /* WRED yellow packets. */
    bcmCosqStatRedDiscardDroppedPackets = 6, /* WRED red packets. */
    bcmCosqStatOutPackets = 7,          /* Transmit packet count. */
    bcmCosqStatOutBytes = 8,            /* Transmit byte count. */
    bcmCosqStatIeee8021CnCpTransmittedCnms = 9, /* QCN CNM Transmit packet count. */
    bcmCosqStatHighPriDroppedPackets = 10, /* PGW OBM high priority dropped packet
                                           count. */
    bcmCosqStatLowPriDroppedPackets = 11, /* PGW OBM low priority dropped packet
                                           count. */
    bcmCosqStatHighPriDroppedBytes = 12, /* PGW OBM high priority dropped bytes
                                           count. */
    bcmCosqStatLowPriDroppedBytes = 13, /* PGW OBM low priority dropped byte
                                           count. */
    bcmCosqStatGlobalHeadroomBytes0Peak = 14, /* Total Bytes used Global Headroom by X
                                           pipe Peak */
    bcmCosqStatGlobalHeadroomBytes0Current = 15, /* Total Bytes used Global Headroom by X
                                           pipe Current */
    bcmCosqStatGlobalHeadroomBytes1Peak = 16, /* Total Bytes used Global Headroom by Y
                                           pipe Peak */
    bcmCosqStatGlobalHeadroomBytes1Current = 17, /* Total Bytes used Global Headroom by Y
                                           pipe Current */
    bcmCosqStatIngressPortPoolSharedBytesPeak = 18, /* Peak value of shared buffers in use
                                           by all ports for each service pool in
                                           bytes. */
    bcmCosqStatIngressPortPoolSharedBytesCurrent = 19, /* Current value buffers in use by all
                                           ports for each service pool in bytes. */
    bcmCosqStatIngressPortPGMinBytesPeak = 20, /* Peak value of buffers in use from PG
                                           min space per port per PG in bytes. */
    bcmCosqStatIngressPortPGMinBytesCurrent = 21, /* Current value of buffers in use from
                                           PG min space per port per PG in
                                           bytes. */
    bcmCosqStatIngressPortPGSharedBytesPeak = 22, /* Peak value of buffers in use from
                                           shared space per port per PG in
                                           bytes. */
    bcmCosqStatIngressPortPGSharedBytesCurrent = 23, /* Current value of buffers in use from
                                           shared space per port per PG in
                                           bytes. */
    bcmCosqStatIngressPortPGHeadroomBytesPeak = 24, /* Peak value of  buffers in use from PG
                                           headroom space in bytes. */
    bcmCosqStatIngressPortPGHeadroomBytesCurrent = 25, /* Current value of  buffers in use from
                                           PG headroom space in bytes. */
    bcmCosqStatEgressMCQueueBytesPeak = 26, /* Peak value of  multicast queue
                                           buffers in use in bytes. */
    bcmCosqStatEgressMCQueueBytesCurrent = 27, /* Current value of  multicast queue
                                           buffers in use in bytes. */
    bcmCosqStatEgressUCQueueBytesPeak = 28, /* Peak value of unicast queue buffers
                                           in use in bytes. */
    bcmCosqStatEgressUCQueueBytesCurrent = 29, /* Current value of unicast queue
                                           buffers in use in bytes. */
    bcmCosqStatEgressUCQueuePortBytesPeak = 30, /* Peak value of  unicast queue shared
                                           buffer in use in bytes. */
    bcmCosqStatEgressUCQueuePortBytesCurrent = 31, /* Current value of unicast queue shared
                                           buffer in use in bytes. */
    bcmCosqStatDiscardDroppedPackets = 32, /* WRED dropped packets. */
    bcmCosqStatUCDroppedPackets = 33,   /* Dropped unicast packet count. */
    bcmCosqStatMCDroppedPackets = 34,   /* Dropped multicast packet count. */
    bcmCosqStatUCDroppedBytes = 35,     /* Dropped unicast byte count. */
    bcmCosqStatMCDroppedBytes = 36,     /* Dropped multicast byte count. */
    bcmCosqStatUCOutPackets = 37,       /* Transmit unicast packet count. */
    bcmCosqStatMCOutPackets = 38,       /* Transmit multicast packet count. */
    bcmCosqStatUCOutBytes = 39,         /* Transmit unicast byte count. */
    bcmCosqStatMCOutBytes = 40,         /* Transmit multicast byte count. */
    bcmCosqStatOBMLossless0DroppedPackets = 41, /* OBM Lossless Class 0 dropped packets
                                           count. */
    bcmCosqStatOBMLossless0DroppedBytes = 42, /* OBM Lossless Class 0 dropped bytes
                                           count. */
    bcmCosqStatOBMLossless1DroppedPackets = 43, /* OBM Lossless Class 1 dropped packets
                                           count. */
    bcmCosqStatOBMLossless1DroppedBytes = 44, /* OBM Lossless Class 1 dropped bytes
                                           count. */
    bcmCosqStatOBMLossyHighPriDroppedPackets = 45, /* OBM high priority dropped packets
                                           count. */
    bcmCosqStatOBMLossyHighPriDroppedBytes = 46, /* OBM high priority dropped bytes
                                           count. */
    bcmCosqStatOBMLossyLowPriDroppedPackets = 47, /* OBM low priority dropped packets
                                           count. */
    bcmCosqStatOBMLossyLowPriDroppedBytes = 48, /* OBM low priority dropped bytes count. */
    bcmCosqStatYellowCongestionDroppedBytes = 49, /* Yellow congestion dropped bytes. */
    bcmCosqStatRedCongestionDroppedBytes = 50, /* Red congestion dropped bytes. */
    bcmCosqStatDiscardDroppedBytes = 51, /* WRED drop bytes. */
    bcmCosqStatGreenDiscardDroppedBytes = 52, /* WRED Green drop bytes. */
    bcmCosqStatYellowDiscardDroppedBytes = 53, /* WRED Yellow drop bytes. */
    bcmCosqStatRedDiscardDroppedBytes = 54, /* WRED Red drop bytes. */
    bcmCosqStatHOLDropPackets = 55,     /* per COSQ HOL drop packet count. */
    bcmCosqStatOBMHighWatermark = 56,   /* per port OBM cell max usage. */
    bcmCosqStatOBMBufferBytes = 57,     /* per port OBM cell buffer use count. */
    bcmCosqStatOBMFlowControlEvents = 58, /* per port OBM generated flow control
                                           events. */
    bcmCosqStatTASTxOverrunPackets = 59, /* Number of transmission overrun events
                                           where a packet is still being
                                           transmitted at the time when the
                                           transmission gate of the queue
                                           closed. */
    bcmCosqStatEgressPortPoolTotalBytesCurrent = 60, /* Per-port Per Service pool total
                                           Buffer Use Count. */
    bcmCosqStatEgressPortPoolSharedBytesCurrent = 61, /* Per-port Per Service pool shared
                                           Buffer Use Count. */
    bcmCosqStatEgressMCQueueMinBytesCurrent = 62, /* Current value of  multicast queue
                                           minimum guarantee buffers in use in
                                           bytes. */
    bcmCosqStatEgressQgroupMinBytesCurrent = 63, /* Current value of queue group minimum
                                           guarantee buffers in use in bytes. */
    bcmCosqStatEgressQgroupBytesCurrent = 64, /* Current value of queue group shared
                                           buffers in use in bytes. */
    bcmCosqStatEgressUCQueueMinBytesCurrent = 65, /* Current value of unicast queue
                                           minimum guarantee buffers in use in
                                           bytes. */
    bcmCosqStatOBMLossless0EnqueuedPackets = 66, /* OBM Lossless Class 0 enqueued packets
                                           count. */
    bcmCosqStatOBMLossless0EnqueuedBytes = 67, /* OBM Lossless Class 0 enqueued bytes
                                           count. */
    bcmCosqStatOBMLossless1EnqueuedPackets = 68, /* OBM Lossless Class 1 enqueued packets
                                           count. */
    bcmCosqStatOBMLossless1EnqueuedBytes = 69, /* OBM Lossless Class 1 enqueued bytes
                                           count. */
    bcmCosqStatOBMLossyHighPriEnqueuedPackets = 70, /* OBM high priority enqueued packets
                                           count. */
    bcmCosqStatOBMLossyHighPriEnqueuedBytes = 71, /* OBM high priority enqueued bytes
                                           count. */
    bcmCosqStatOBMLossyLowPriEnqueuedPackets = 72, /* OBM low priority enqueued packets
                                           count. */
    bcmCosqStatOBMLossyLowPriEnqueuedBytes = 73, /* OBM low priority enqueued bytes
                                           count. */
    bcmCosqStatOBMExpressEnqueuedPackets = 74, /* OBM express enqueued packets count. */
    bcmCosqStatOBMExpressEnqueuedBytes = 75, /* OBM express enqueued bytes count. */
    bcmCosqStatOBMExpressDroppedPackets = 76, /* OBM express dropped packets count. */
    bcmCosqStatOBMExpressDroppedBytes = 77, /* OBM express dropped bytes count. */
    bcmCosqStatHdrmPoolDroppedPackets = 78, /* Number of packets dropped per
                                           Headroom Pool */
    bcmCosqStatSourcePortDroppedPackets = 79, /* Number of packets dropped per Source
                                           Port */
    bcmCosqStatEgressCongestionDroppedPackets = 80, /* Number of packets dropped by Egress
                                           admission excuding the WRED drops */
    bcmCosqStatRQETotalDroppedPackets = 81, /* Total packets dropped per RQE queue */
    bcmCosqStatRQETotalDroppedBytes = 82, /* Total bytes dropped per RQE queue */
    bcmCosqStatRQEGreenDroppedPackets = 83, /* Green packets dropped per RQE queue */
    bcmCosqStatRQEYellowDroppedPackets = 84, /* Yellow packets dropped per RQE queue */
    bcmCosqStatOBMLossyBufferBytes = 85, /* Current buffer usage for obm Lossy
                                           traffic class */
    bcmCosqStatOBMLossless0BufferBytes = 86, /* Current buffer usage for obm
                                           Lossless0 traffic class */
    bcmCosqStatOBMLossless1BufferBytes = 87, /* Current buffer usage for obm
                                           Lossless1 traffic class */
    bcmCosqStatRQERedDroppedPackets = 88, /* Red packets dropped per RQE queue */
    bcmCosqStatDiscardUCDroppedPackets = 89, /* Per port per queue WRED dropped
                                           packets. */
    bcmCosqStatTotalDiscardDroppedPackets = 90, /* Total WRED dropped packets. */
    bcmCosqStatAdtAlpha0AdmittedLowPriPackets = 91, /* Low priority packets of the selected
                                           egress pool that are admitted using
                                           limit_alpha 0 */
    bcmCosqStatAdtAlpha1AdmittedLowPriPackets = 92, /* Low priority packets of the selected
                                           egress pool that are admitted using
                                           limit_alpha 1 */
    bcmCosqStatAdtAlpha2AdmittedLowPriPackets = 93, /* Low priority packets of the selected
                                           egress pool that are admitted using
                                           limit_alpha 2 */
    bcmCosqStatAdtAlpha3AdmittedLowPriPackets = 94, /* Low priority packets of the selected
                                           egress pool that are admitted using
                                           limit_alpha 3 */
    bcmCosqStatAdtAlpha4AdmittedLowPriPackets = 95, /* Low priority packets of the selected
                                           egress pool that are admitted using
                                           limit_alpha 4 */
    bcmCosqStatAdtAlpha5AdmittedLowPriPackets = 96, /* Low priority packets of the selected
                                           egress pool that are admitted using
                                           limit_alpha 5 */
    bcmCosqStatAdtAlpha6AdmittedLowPriPackets = 97, /* Low priority packets of the selected
                                           egress pool that are admitted using
                                           limit_alpha 6 */
    bcmCosqStatAdtAlpha7AdmittedLowPriPackets = 98, /* Low priority packets of the selected
                                           egress pool that are admitted using
                                           limit_alpha 7 */
    bcmCosqStatAdtAlpha8AdmittedLowPriPackets = 99, /* Low priority packets of the selected
                                           egress pool that are admitted using
                                           limit_alpha 8 */
    bcmCosqStatAdtAlpha9AdmittedLowPriPackets = 100, /* Low priority packets of the selected
                                           egress pool that are admitted using
                                           limit_alpha 9 */
    bcmCosqStatAdtAlpha10AdmittedLowPriPackets = 101, /* Low priority packets of the selected
                                           egress pool that are admitted using
                                           limit_alpha 10 */
    bcmCosqStatEgressPoolAdtAdmittedHighPriBytes = 102, /* High priority bytes admitted of an
                                           egress service pool */
    bcmCosqStatDebugSaf0Packets = 103,  /* Debug counter 0 to track the number
                                           of SAF (Store-and-Forwarding) packets
                                           of a traffic flow forwarded from an
                                           ITM */
    bcmCosqStatDebugSaf1Packets = 104,  /* Debug counter 1 to track the number
                                           of SAF (Store-and-Forwarding) packets
                                           of a traffic flow forwarded from an
                                           ITM */
    bcmCosqStatDebugSaf2Packets = 105,  /* Debug counter 2 to track the number
                                           of SAF (Store-and-Forwarding) packets
                                           of a traffic flow forwarded from an
                                           ITM */
    bcmCosqStatDebugSaf3Packets = 106,  /* Debug counter 3 to track the number
                                           of SAF (Store-and-Forwarding) packets
                                           of a traffic flow forwarded from an
                                           ITM */
    bcmCosqStatDebugSaf4Packets = 107,  /* Debug counter 4 to track the number
                                           of SAF (Store-and-Forwarding) packets
                                           of a traffic flow forwarded from an
                                           ITM */
    bcmCosqStatDebugSaf5Packets = 108,  /* Debug counter 5 to track the number
                                           of SAF (Store-and-Forwarding) packets
                                           of a traffic flow forwarded from an
                                           ITM */
    bcmCosqStatDebugSaf6Packets = 109,  /* Debug counter 6 to track the number
                                           of SAF (Store-and-Forwarding) packets
                                           of a traffic flow forwarded from an
                                           ITM */
    bcmCosqStatDebugSaf7Packets = 110,  /* Debug counter 7 to track the number
                                           of SAF (Store-and-Forwarding) packets
                                           of a traffic flow forwarded from an
                                           ITM */
    bcmCosqStatDebugCt0Packets = 111,   /* Debug counter 0 to track the number
                                           of CT (Cut-Through)) packets of a
                                           traffic flow forwarded from an ITM */
    bcmCosqStatDebugCt1Packets = 112,   /* Debug counter 1 to track the number
                                           of CT (Cut-Through)) packets of a
                                           traffic flow forwarded from an ITM */
    bcmCosqStatDebugCt2Packets = 113,   /* Debug counter 2 to track the number
                                           of CT (Cut-Through)) packets of a
                                           traffic flow forwarded from an ITM */
    bcmCosqStatDebugCt3Packets = 114,   /* Debug counter 3 to track the number
                                           of CT (Cut-Through)) packets of a
                                           traffic flow forwarded from an ITM */
    bcmCosqStatDebugCt4Packets = 115,   /* Debug counter 4 to track the number
                                           of CT (Cut-Through)) packets of a
                                           traffic flow forwarded from an ITM */
    bcmCosqStatDebugCt5Packets = 116,   /* Debug counter 5 to track the number
                                           of CT (Cut-Through)) packets of a
                                           traffic flow forwarded from an ITM */
    bcmCosqStatDebugCt6Packets = 117,   /* Debug counter 6 to track the number
                                           of CT (Cut-Through)) packets of a
                                           traffic flow forwarded from an ITM */
    bcmCosqStatDebugCt7Packets = 118,   /* Debug counter 7 to track the number
                                           of CT (Cut-Through)) packets of a
                                           traffic flow forwarded from an ITM */
    bcmCosqStatEgressPoolSharedBytesCurrent = 119, /* Current value of buffers used in each
                                           service pool in bytes. */
    bcmCosqStatEgressPortUCPacketsCurrent = 120, /* Current unicast packet count per port
                                           for all SPs in bytes. */
    bcmCosqStatEgressPortMCPacketsCurrent = 121, /*  Current multicast packet count per
                                           port for all SPs in bytes. */
    bcmCosqStatEgressPoolSharedUCBytesCurrent = 122, /* Current value of egress service pool
                                           shared buffer in use by UC in bytes. */
    bcmCosqStatEgressPoolSharedMCBytesCurrent = 123, /* Current value of egress service pool
                                           shared buffer in use by MC in bytes. */
    bcmCosqStatHeadroomPoolBytesCurrent = 124, /* Current value of headroom pool in use
                                           all Priority Groups in bytes. */
    bcmCosqStatIngressPoolSharedBytesCurrent = 125, /* Current value of ingress service pool
                                           shared buffer in use by all the ports
                                           in bytes. */
    bcmCosqStatRQEQueueMinBytesCurrent = 126, /* Current minimum guarantee usage per
                                           RQE queue in bytes. */
    bcmCosqStatRQEQueueSharedBytesCurrent = 127, /* Current shared usage per RQE queue in
                                           bytes. */
    bcmCosqStatRQEQueueTotalBytesCurrent = 128, /* Current total usage per RQE queue in
                                           bytes. */
    bcmCosqStatRQEPoolSharedBytesCurrent = 129, /* Current value of RQE pool in use by
                                           all RQE queues in bytes. */
    bcmCosqStatSourcePortDroppedBytes = 130, /* Number of bytes dropped per Source
                                           Port */
    bcmCosqStatMaxCount = 131           /* this should be the last one. */
} bcm_cosq_stat_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Retrieve/set various MMU statistics based on stat type */
extern int bcm_cosq_stat_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_cosq_stat_t stat, 
    uint64 *value);

/* Retrieve/set various MMU statistics based on stat type */
extern int bcm_cosq_stat_get32(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_cosq_stat_t stat, 
    uint32 *value);

/* Force an immediate counter update and retrieve various MMU statistics. */
extern int bcm_cosq_stat_sync_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_cosq_stat_t stat, 
    uint64 *value);

/* 
 * Force an immediate counter update and retrieve multiple MMU
 * statistics.
 */
extern int bcm_cosq_stat_sync_multi_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    int nstat, 
    bcm_cosq_stat_t *stat_arr, 
    uint64 *value_arr);

/* 
 * Force an immediate counter update and retrieve multiple MMU
 * statistics.
 */
extern int bcm_cosq_stat_sync_multi_get32(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    int nstat, 
    bcm_cosq_stat_t *stat_arr, 
    uint32 *value_arr);

/* Force an immediate counter update and retrieve various MMU statistics. */
extern int bcm_cosq_stat_sync_get32(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_cosq_stat_t stat, 
    uint32 *value);

/* Retrieve/set various MMU statistics based on stat type */
extern int bcm_cosq_stat_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_cosq_stat_t stat, 
    uint64 value);

/* Retrieve/set various MMU statistics based on stat type */
extern int bcm_cosq_stat_set32(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_cosq_stat_t stat, 
    uint32 value);

#endif /* BCM_HIDE_DISPATCHABLE */

/* parameters for VOQ gport */
typedef struct bcm_cosq_congestion_info_s {
    bcm_module_t fabric_modid;  /* Fabric module ID. */
    bcm_port_t fabric_port;     /* Fabric module ID. */
    bcm_module_t dest_modid;    /* destination module ID. */
    bcm_port_t dest_port;       /* destination port ID. */
} bcm_cosq_congestion_info_t;

/* Initialize a CoSQ congestion information structure. */
extern void bcm_cosq_congestion_info_t_init(
    bcm_cosq_congestion_info_t *config);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/Get properties for the queue gport. */
extern int bcm_cosq_gport_congestion_config_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_cosq_congestion_info_t *config);

/* Set/Get properties for the queue gport. */
extern int bcm_cosq_gport_congestion_config_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_cosq_congestion_info_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

typedef struct bcm_cos_pon_link_sla_s {
    int option;                 /* option flag(force report, etc). */
    int polling_level;          /* polling level 0 - 7. */
    uint32 tdm_rate;            /* TDM rate in kbps unit */
    uint32 tdm_grant_length;    /* TDM grant length in bytes */
} bcm_cos_pon_link_sla_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/Get EPON Link SLA Misc. parameters. */
extern int bcm_cosq_gport_pon_link_sla_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_pon_link_sla_t *link_sla);

/* Set/Get EPON Link SLA Misc. parameters. */
extern int bcm_cosq_gport_pon_link_sla_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_pon_link_sla_t *link_sla);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Attribute flags for resource identifier. */
#define BCM_COSQ_DEVICE                 0x10000    /* device resource */
#define BCM_COSQ_DEVICE_EGRESS_POOL     0x20000    /* device egress service
                                                      pool. */
#define BCM_COSQ_DEVICE_INGRESS_POOL    0x40000    /* device ingress service
                                                      pool. */
#define BCM_COSQ_PORT_POOL              0x80000    /* port service pool. */
#define BCM_COSQ_PRIGROUP               0x100000   /* priority group. */
#define BCM_COSQ_HEADROOM               0x200000   /* headroom. */
#define BCM_COSQ_QUEUE                  0x400000   /* headroom. */
#define BCM_COSQ_STAT_CLEAR             0x800000   /* perform with clear-on-read */

/* BST Tracing statistics resources type eumerations. */
typedef enum bcm_bst_stat_id_e {
    bcmBstStatIdInvalid = -1,           /* Invalid BST Id and used for sync all
                                           the Hardware stats to the Software
                                           copy */
    bcmBstStatIdDevice = 0,             /* Per device BST tracing resource */
    bcmBstStatIdEgrPool = 1,            /* Per Egress Pool BST tracing resource */
    bcmBstStatIdEgrMCastPool = 2,       /* Per Egress Pool BST tracing
                                           resource(Multicast) */
    bcmBstStatIdIngPool = 3,            /* Per Ingress Pool BST tracing resource */
    bcmBstStatIdPortPool = 4,           /* Per Port Pool BST tracing resource */
    bcmBstStatIdPriGroupShared = 5,     /* Per Shared Priority Group Pool BST
                                           tracing resource */
    bcmBstStatIdPriGroupHeadroom = 6,   /* Per Priority Group Headroom BST
                                           tracing resource */
    bcmBstStatIdUcast = 7,              /* BST Tracing resource for unicast */
    bcmBstStatIdMcast = 8,              /* BST Tracing resource for multicast */
    bcmBstStatIdHeadroomPool = 9,       /* BST Tracing the Headroom Pool Usage
                                           Count */
    bcmBstStatIdEgrPortPoolSharedUcast = 10, /* Per Port Pool unicast BST Tracing
                                           resource */
    bcmBstStatIdEgrPortPoolSharedMcast = 11, /* Per Port Pool multicast BST Tracing
                                           resource */
    bcmBstStatIdQueueTotal = 12,        /* Per Queue total use counts for all
                                           packets */
    bcmBstStatIdRQEQueue = 13,          /* BST Tracing resource for RQE queue */
    bcmBstStatIdRQEPool = 14,           /* BST Tracing resource for RQE pool */
    bcmBstStatIdUCQueueGroup = 15,      /* BST Tracing resource for unicast
                                           queue group */
    bcmBstStatIdMaxCount = 16           /* Must be the last. Not a usable value. */
} bcm_bst_stat_id_t;

typedef struct bcm_cosq_bst_profile_s {
    uint32 byte; 
} bcm_cosq_bst_profile_t;

/* Initialize a CoSQ BST profile structure. */
extern void bcm_cosq_bst_profile_t_init(
    bcm_cosq_bst_profile_t *profile);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/Get the BST profile for CosQ objects */
extern int bcm_cosq_bst_profile_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_bst_stat_id_t bid, 
    bcm_cosq_bst_profile_t *profile);

/* Set/Get the BST profile for CosQ objects */
extern int bcm_cosq_bst_profile_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_bst_stat_id_t bid, 
    bcm_cosq_bst_profile_t *profile);

/* Set/Get the BST profile for CosQ objects */
extern int bcm_cosq_bst_multi_profile_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_bst_stat_id_t bid, 
    int array_size, 
    bcm_cosq_bst_profile_t *profile_array);

/* Set/Get the BST profile for CosQ objects */
extern int bcm_cosq_bst_multi_profile_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_bst_stat_id_t bid, 
    int array_size, 
    bcm_cosq_bst_profile_t *profile_array, 
    int *count);

/* Sync the HW stats value to SW copy for all or given BST resource. */
extern int bcm_cosq_bst_stat_sync(
    int unit, 
    bcm_bst_stat_id_t bid);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_COSQ_WEIGHT_DISCRETE_WEIGHT_LEVEL0 -1         /* weight associated with
                                                          level 0 of scheduler
                                                          element having
                                                          discrete weight
                                                          attribute. */
#define BCM_COSQ_WEIGHT_DISCRETE_WEIGHT_LEVEL1 -2         /* weight associated with
                                                          level 1 of scheduler
                                                          element having
                                                          discrete weight
                                                          attribute. */
#define BCM_COSQ_WEIGHT_DISCRETE_WEIGHT_LEVEL2 -3         /* weight associated with
                                                          level 2 of scheduler
                                                          element having
                                                          discrete weight
                                                          attribute. */
#define BCM_COSQ_WEIGHT_DISCRETE_WEIGHT_LEVEL3 -4         /* weight associated with
                                                          level 3 of scheduler
                                                          element having
                                                          discrete weight
                                                          attribute. */

/* Various types of cosq classifiers */
typedef struct bcm_cosq_classifier_s {
    uint32 flags;       /* BCM_COSQ_CLASSIFIER_xxx flags */
    bcm_vlan_t vlan;    /* VLAN */
    bcm_mac_t mac;      /* MAC address */
    bcm_vrf_t vrf;      /* Virtual Router Instance */
    bcm_ip_t ip_addr;   /* IPv4 address */
    bcm_ip6_t ip6_addr; /* IPv6 address */
    bcm_gport_t gport;  /* GPORT ID */
    uint32 vfi_index;   /* VFI index */
    bcm_if_t egress_if; /* Egress object interface */
} bcm_cosq_classifier_t;

/* Flags indicating classifier type */
#define BCM_COSQ_CLASSIFIER_WITH_ID     0x00000001 /* Classifier ID */
#define BCM_COSQ_CLASSIFIER_VLAN        0x00000002 /* Classifier consists of
                                                      VLAN */
#define BCM_COSQ_CLASSIFIER_L2          0x00000004 /* Classifier consists of
                                                      VLAN and MAC address */
#define BCM_COSQ_CLASSIFIER_L3          0x00000008 /* Classifier consists of VRF
                                                      and IP address */
#define BCM_COSQ_CLASSIFIER_IP6         0x00000010 /* The IP address in L3
                                                      classifier is IPv6 */
#define BCM_COSQ_CLASSIFIER_GPORT       0x00000020 /* Classifier consists of
                                                      GPORT */
#define BCM_COSQ_CLASSIFIER_FIELD       0x00000040 /* Classifier used in a field
                                                      processor rule. */
#define BCM_COSQ_CLASSIFIER_VFI         0x00000080 /* Classifier consists of VFI
                                                      index */
#define BCM_COSQ_CLASSIFIER_L3_EGRESS   0x00000100 /* Classifier consists of L3
                                                      egress object */
#define BCM_COSQ_CLASSIFIER_NO_INT_PRI  0x00000200 /* Classifier used without
                                                      internal priority */

/* Initialize a bcm_cosq_classifier_t structure. */
extern void bcm_cosq_classifier_t_init(
    bcm_cosq_classifier_t *classifier);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create a queue classifier. */
extern int bcm_cosq_classifier_create(
    int unit, 
    bcm_cosq_classifier_t *classifier, 
    int *classifier_id);

/* Destroy a queue classifier. */
extern int bcm_cosq_classifier_destroy(
    int unit, 
    int classifier_id);

/* Get information about a queue classifier. */
extern int bcm_cosq_classifier_get(
    int unit, 
    int classifier_id, 
    bcm_cosq_classifier_t *classifier);

/* Set a classifier's queue mapping. */
extern int bcm_cosq_classifier_mapping_set(
    int unit, 
    bcm_gport_t port, 
    int classifier_id, 
    bcm_gport_t queue_group, 
    bcm_cos_t priority, 
    bcm_cos_queue_t cosq);

/* Set multiple queue mappings for a classifier. */
extern int bcm_cosq_classifier_mapping_multi_set(
    int unit, 
    bcm_gport_t port, 
    int classifier_id, 
    bcm_gport_t queue_group, 
    int array_count, 
    bcm_cos_t *priority_array, 
    bcm_cos_queue_t *cosq_array);

/* Get a classifier's queue mapping. */
extern int bcm_cosq_classifier_mapping_get(
    int unit, 
    bcm_gport_t port, 
    int classifier_id, 
    bcm_gport_t *queue_group, 
    bcm_cos_t priority, 
    bcm_cos_queue_t *cosq);

/* Get multiple queue mappings for a classifier. */
extern int bcm_cosq_classifier_mapping_multi_get(
    int unit, 
    bcm_gport_t port, 
    int classifier_id, 
    bcm_gport_t *queue_group, 
    int array_max, 
    bcm_cos_t *priority_array, 
    bcm_cos_queue_t *cosq_array, 
    int *array_count);

/* Clear a classifier's queue mapping. */
extern int bcm_cosq_classifier_mapping_clear(
    int unit, 
    bcm_gport_t port, 
    int classifier_id);

/* Gets the classifier id for the given profile. */
extern int bcm_cosq_classifier_id_get(
    int unit, 
    bcm_cosq_classifier_t *classifier, 
    int array_count, 
    bcm_cos_t *priority_array, 
    bcm_cos_queue_t *cosq_array, 
    int *classifier_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Config parameters for Priority flow control */
typedef struct bcm_cosq_qcn_config_s {
    int size;                           /* Set point for queue. In Bytes. */
    int weight;                         /* Weight to be given to change in queue
                                           length. */
    int nbr_samples;                    /* Number of valid samples. */
    int sample_bytes[BCM_COS_COUNT]; 
    int max_feedback;                   /* Maximum value of congestion feedback. */
    int quantization;                   /* Quantization value. */
    int randomize_sample_threshold;     /* Random sample threshold - true/false. */
} bcm_cosq_qcn_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set the configuration of a congestion managed queue parameters */
extern int bcm_cosq_qcn_config_set(
    int unit, 
    bcm_gport_t port, 
    bcm_cos_queue_t cosq, 
    uint32 flags, 
    bcm_cosq_qcn_config_t *config);

/* Set the configuration of a congestion managed queue parameters */
extern int bcm_cosq_qcn_config_get(
    int unit, 
    bcm_gport_t port, 
    bcm_cos_queue_t cosq, 
    uint32 flags, 
    bcm_cosq_qcn_config_t *config);

/* Set the credit worth size per module-ID */
extern int bcm_cosq_dest_credit_size_set(
    int unit, 
    bcm_module_t dest_modid, 
    uint32 credit_size);

/* Set the credit worth size per module-ID */
extern int bcm_cosq_dest_credit_size_get(
    int unit, 
    bcm_module_t dest_modid, 
    uint32 *credit_size);

/* To enable/disable Rx of packets on the specified CPU cosq. */
extern int bcm_cosq_cpu_cosq_enable_set(
    int unit, 
    bcm_cos_queue_t cosq, 
    int enable);

/* Get status of RX for a given CPU queue */
extern int bcm_cosq_cpu_cosq_enable_get(
    int unit, 
    bcm_cos_queue_t cosq, 
    int *enable);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Alpha value for dynamic threshold */
typedef enum bcm_cosq_control_drop_limit_alpha_value_e {
    bcmCosqControlDropLimitAlpha_1_64 = 0, /* Use 1/64 as the alpha value for
                                           dynamic threshold */
    bcmCosqControlDropLimitAlpha_1_32 = 1, /* Use 1/32 as the alpha value for
                                           dynamic threshold */
    bcmCosqControlDropLimitAlpha_1_16 = 2, /* Use 1/16 as the alpha value for
                                           dynamic threshold */
    bcmCosqControlDropLimitAlpha_1_8 = 3, /* Use 1/8 as the alpha value for
                                           dynamic threshold */
    bcmCosqControlDropLimitAlpha_1_4 = 4, /* Use 1/4 as the alpha value for
                                           dynamic threshold */
    bcmCosqControlDropLimitAlpha_1_2 = 5, /* Use 1/2 as the alpha value for
                                           dynamic threshold */
    bcmCosqControlDropLimitAlpha_1 = 6, /* Use 1 as the alpha value for dynamic
                                           threshold */
    bcmCosqControlDropLimitAlpha_2 = 7, /* Use 2 as the alpha value for dynamic
                                           threshold */
    bcmCosqControlDropLimitAlpha_4 = 8, /* Use 4 as the alpha value for dynamic
                                           threshold */
    bcmCosqControlDropLimitAlpha_1_128 = 9, /* Use 1/128 as the alpha value for
                                           dynamic threshold */
    bcmCosqControlDropLimitAlpha_8 = 10, /* Use 8 as the alpha value for dynamic
                                           threshold */
    bcmCosqControlDropLimitAlphaCount = 11 /* Must be the last entry! */
} bcm_cosq_control_drop_limit_alpha_value_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Clear the current statistic/count of specified BST profile */
extern int bcm_cosq_bst_stat_clear(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_bst_stat_id_t bid);

/* Clear the current statistic/count of specified BST profile */
extern int bcm_cosq_bst_stat_extended_clear(
    int unit, 
    bcm_cosq_object_id_t *id, 
    bcm_bst_stat_id_t bid);

/* Get the current statistic/count of specified BST profile */
extern int bcm_cosq_bst_stat_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_bst_stat_id_t bid, 
    uint32 options, 
    uint64 *value);

/* Get the current statistic/count of specified BST profile */
extern int bcm_cosq_bst_stat_get32(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_bst_stat_id_t bid, 
    uint32 options, 
    uint32 *value);

/* Get the current statistic/count of specified BST profile */
extern int bcm_cosq_bst_stat_extended_get(
    int unit, 
    bcm_cosq_object_id_t *id, 
    bcm_bst_stat_id_t bid, 
    uint32 options, 
    uint64 *value);

/* Get the current statistic/count of specified BST profile */
extern int bcm_cosq_bst_stat_extended_get32(
    int unit, 
    bcm_cosq_object_id_t *id, 
    bcm_bst_stat_id_t bid, 
    uint32 options, 
    uint32 *value);

/* Get the current statistic/count of multiple specified BST profile */
extern int bcm_cosq_bst_stat_multi_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    uint32 options, 
    int max_values, 
    bcm_bst_stat_id_t *id_list, 
    uint64 *values);

/* Get the current statistic/count of multiple specified BST profile */
extern int bcm_cosq_bst_stat_multi_get32(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    uint32 options, 
    int max_values, 
    bcm_bst_stat_id_t *id_list, 
    uint32 *values);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Service pool types. */
typedef enum bcm_cosq_service_pool_type_e {
    bcmCosqServicePoolPortColorAware = 0, /* Port service pool level color aware */
    bcmCosqServicePoolColorAware = 1,   /* Pool level color aware */
    bcmCosqServicePoolQueueCongestionAware = 2, /* Pool level queue congeston status
                                           aware */
    bcmCosqServicePoolTypeCount = 3     /* Must be the last entry! */
} bcm_cosq_service_pool_type_t;

/* This structure has everything about service pool. */
typedef struct bcm_cosq_service_pool_s {
    bcm_cosq_service_pool_type_t type;  /* Service pool type. */
    int enabled;                        /* Enable or disable. */
} bcm_cosq_service_pool_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * To set/get enable or disable status of service pool for a given pool
 * id
 */
extern int bcm_cosq_service_pool_set(
    int unit, 
    bcm_service_pool_id_t id, 
    bcm_cosq_service_pool_t cosq_service_pool);

/* 
 * To set/get enable or disable status of service pool for a given pool
 * id
 */
extern int bcm_cosq_service_pool_get(
    int unit, 
    bcm_service_pool_id_t id, 
    bcm_cosq_service_pool_t *cosq_service_pool);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a COSQ service pool structure. */
extern void bcm_cosq_service_pool_t_init(
    bcm_cosq_service_pool_t *service_pool);

#ifndef BCM_HIDE_DISPATCHABLE

/* Reattach the output of a GPORT to the input of a scheduler GPORT. */
extern int bcm_cosq_gport_reattach(
    int unit, 
    bcm_gport_t sched_port, 
    bcm_gport_t input_port, 
    bcm_cos_queue_t cosq);

/* Maps a gport to a profile */
extern int bcm_cosq_profile_mapping_set(
    int unit, 
    bcm_gport_t gport_to_map, 
    bcm_cos_queue_t cosq, 
    uint32 flags, 
    bcm_switch_profile_mapping_t *profile_mapping);

extern int bcm_cosq_profile_mapping_get(
    int unit, 
    bcm_gport_t gport_to_map, 
    bcm_cos_queue_t cosq, 
    uint32 flags, 
    bcm_switch_profile_mapping_t *profile_mapping);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Range configuration type. */
typedef enum bcm_cosq_control_range_type_e {
    bcmCosqRangeMulticastQueue = 0,     /* Multicast QID range configuration. */
    bcmCosqRangeShaperQueue = 1,        /* FIC QID shaping range configuration. */
    bcmCosqRangeFabricQueue = 2,        /* FIC QID range configuration. */
    bcmCosqRecycleQueue = 3,            /* Recycle QID range configuration. */
    bcmCosqWatchdogQueue = 4,           /* Watchdog QID range configuration. */
    bcmCosq1Plus1PortProtection = 5,    /* One plus one port protection:
                                           configuring which DPs participate are
                                           used. */
    bcmCosqThresholdDramMixDbuff = 6,   /* How many OCB buffers can be given to
                                           dram-mix queues. */
    bcmCosqOcbCommittedMulticast_1 = 7, /* A range of MC IDs that are guaranteed
                                           to use OCB buffers If available.
                                           Range number 1. */
    bcmCosqOcbCommittedMulticast_2 = 8, /* A range of MC IDs that are guaranteed
                                           to use OCB buffers If available.
                                           Range number 2. */
    bcmCosqOcbEligibleMulticast_1 = 9,  /* A range of MC IDs that are allowed to
                                           use OCB buffers If available. Range
                                           number 1. */
    bcmCosqOcbEligibleMulticast_2 = 10, /* A range of MC IDs that are allowed to
                                           use OCB buffers If available. Range
                                           number 2. */
    bcmCosqStatIfQueues = 11,           /* A range of queues that produce
                                           statistics reports when working in
                                           queue size mode. */
    bcmCosqStatIfScrubberQueues = 12,   /* A range of queues that produce
                                           statistics scrubber reports when
                                           working in queue size mode. */
    bcmCosqRangeLowRateVoqConnector = 13 /* Low range low rate VOQ connectors. */
} bcm_cosq_control_range_type_t;

/* Range configuration. */
typedef struct bcm_cosq_range_s {
    uint8 is_enabled;   /* Enable range ccnfiguration. */
    int range_start;    /* Minimum size. */
    int range_end;      /* Maximus size. */
} bcm_cosq_range_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/Get a range configuration. */
extern int bcm_cosq_control_range_set(
    int unit, 
    bcm_gport_t port, 
    uint32 flags, 
    bcm_cosq_control_range_type_t type, 
    bcm_cosq_range_t *range);

/* Set/Get a range configuration. */
extern int bcm_cosq_control_range_get(
    int unit, 
    bcm_gport_t port, 
    uint32 flags, 
    bcm_cosq_control_range_type_t type, 
    bcm_cosq_range_t *range);

#endif /* BCM_HIDE_DISPATCHABLE */

/* resource allocation related flags. */
#define BCM_COSQ_RSRC_ALLOC_COLOR_BLIND 0x0001     

/* Type of resource. */
typedef enum bcm_cosq_resource_e {
    bcmReservationResourceBufferDescriptors = 0, /* Resource reservation of buffer
                                           descriptors. */
    bcmResourceOcbBuffers = 1,          /* OCB resource in buffers. */
    bcmReservationResourceOcbBuffers = bcmResourceOcbBuffers, /* OCB resource reservation in buffers. */
    bcmReservationResourceBufferDescriptorBuffers = 2, /* Resource reservation of buffer
                                           descriptor buffers. */
    bcmResourceBytes = 3,               /* Resource in bytes. */
    bcmReservationResourceBytes = bcmResourceBytes, /* Resource reservation in bytes. */
    bcmResourceOcbBytes = 4,            /* OCB resource in bytes. */
    bcmResourceOcbPacketDescriptors = 5, /* OCB Resource in packet descriptors. */
    bcmResourceOcbPacketDescriptorsBuffers = 6, 
    bcmResourceDramBundleDescriptorsBuffers = 7 
} bcm_cosq_resource_t;

/* Backward compatibility. */
typedef bcm_cosq_resource_t bcm_cosq_allocation_resource_t;

/* An entity for which we allocate. */
typedef struct bcm_cosq_allocation_entity_s {
    bcm_gport_t gport;  /* VSQ for which reservation will be done. */
    bcm_color_t color;  /* drop precedence. */
    uint8 pool_id;      /* pool for which to reserve, used for src port VSQs. */
    uint8 is_ocb_only;  /* PG type (OCB-only/DRAM-mix) for which to reserve,
                           used for src port VSQs. */
} bcm_cosq_allocation_entity_t;

/* Resource amounts used for allocation. */
typedef struct bcm_cosq_resource_amounts_s {
    uint32 flags; 
    uint32 reserved;                    /* reserved amount of the resource. */
    uint32 max_shared_pool;             /* maximum amount of the resource in the
                                           shared pool. */
    bcm_cosq_fadt_threshold_t shared_pool_fadt; /* FADT threshold on the amount of the
                                           resource in the shared pool. */
    uint32 max_headroom;                /* maximum amount of the resource in the
                                           lossless headroom. */
    uint32 nominal_headroom;            /* nominal amount of the resource in the
                                           lossless headroom. */
} bcm_cosq_resource_amounts_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/Get the resource allocation for a target/entity */
extern int bcm_cosq_resource_allocation_set(
    int unit, 
    uint32 flags, 
    bcm_cosq_resource_t resource, 
    bcm_cosq_allocation_entity_t *target, 
    bcm_cosq_resource_amounts_t *amounts);

/* Set/Get the resource allocation for a target/entity */
extern int bcm_cosq_resource_allocation_get(
    int unit, 
    uint32 flags, 
    bcm_cosq_resource_t resource, 
    bcm_cosq_allocation_entity_t *target, 
    bcm_cosq_resource_amounts_t *amounts);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Struct for configuring a slow factor. */
typedef struct bcm_cosq_slow_level_s {
    int core;       /* 0, 1, BCM_CORE_ALL */
    int profile; 
    uint32 level; 
    uint32 flags; 
} bcm_cosq_slow_level_t;

typedef struct bcm_cosq_slow_profile_attributes_s {
    uint32 max_rate; 
} bcm_cosq_slow_profile_attributes_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/Get the slow factor per profile. */
extern int bcm_cosq_slow_profile_set(
    int unit, 
    bcm_cosq_slow_level_t *slow_level, 
    bcm_cosq_slow_profile_attributes_t *attr);

/* Set/Get the slow factor per profile. */
extern int bcm_cosq_slow_profile_get(
    int unit, 
    bcm_cosq_slow_level_t *slow_level, 
    bcm_cosq_slow_profile_attributes_t *attr);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Total number of PFC controlled COSqs and scheduling nodes on a port. */
#define BCM_COSQ_PFC_GPORT_COUNT    26         

/* PFC class to COSq / scheduling node mapping structure. */
typedef struct bcm_cosq_pfc_class_mapping_s {
    int class_id;                       /* PFC class. */
    bcm_gport_t gport_list[BCM_COSQ_PFC_GPORT_COUNT]; /* gport group (COSqs or scheduling
                                           nodes) that mapped to a PFC class. */
} bcm_cosq_pfc_class_mapping_t;

/* Initialize a COSq PFC class mapping structure. */
extern void bcm_cosq_pfc_class_mapping_t_init(
    bcm_cosq_pfc_class_mapping_t *mapping);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Get / set mapping of PFC class to COSQs and / or scheduling nodes for
 * a port.
 */
extern int bcm_cosq_pfc_class_mapping_set(
    int unit, 
    bcm_gport_t port, 
    int array_count, 
    bcm_cosq_pfc_class_mapping_t *mapping_array);

/* 
 * Get / set mapping of PFC class to COSQs and / or scheduling nodes for
 * a port.
 */
extern int bcm_cosq_pfc_class_mapping_get(
    int unit, 
    bcm_gport_t port, 
    int array_max, 
    bcm_cosq_pfc_class_mapping_t *mapping_array, 
    int *array_count);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Total number of SAFC controlled COSqs and scheduling nodes on a port. */
#define BCM_COSQ_SAFC_GPORT_COUNT   20         

#define BCM_COSQ_INGRESS_PORT_DROP_FLAGS_THRESHOLD_PRECENT (1<<0)     

#define BCM_COSQ_INGRESS_PORT_DROP_ETH_MAP_KEY(pcp, dei)  ( ( ((pcp << _SHR_PORT_PRD_ETH_DEI_KEY_SIZE) | dei) & (_SHR_PORT_PRD_ETH_KEY_MASK) ) | _SHR_PORT_PRD_ETH_MAP_INDICATION ) 
#define BCM_COSQ_INGRESS_PORT_DROP_TM_MAP_KEY(tc, dp)  ( ( ((tc << _SHR_PORT_PRD_TM_DP_KEY_SIZE) | dp) & (_SHR_PORT_PRD_TM_KEY_MASK) ) | _SHR_PORT_PRD_TM_MAP_INDICATION ) 
#define BCM_COSQ_INGRESS_PORT_DROP_IP_MAP_KEY(dscp)   ( ((dscp) & _SHR_PORT_PRD_IP_KEY_MASK) | _SHR_PORT_PRD_IP_MAP_INDICATION ) 
#define BCM_COSQ_INGRESS_PORT_DROP_MPLS_MAP_KEY(exp)  ( ((exp) & _SHR_PORT_PRD_MPLS_KEY_MASK) | _SHR_PORT_PRD_MPLS_MAP_INDICATION ) 

/* SAFC class to COSq / scheduling node mapping structure. */
typedef struct bcm_cosq_safc_class_mapping_s {
    int class_id;                       /* SAFC class. */
    bcm_gport_t gport_list[BCM_COSQ_SAFC_GPORT_COUNT]; /* gport group (COSqs or scheduling
                                           nodes) that mapped to a SAFC class. */
} bcm_cosq_safc_class_mapping_t;

/* Initialize a SAFC class mapping structure. */
extern void bcm_cosq_safc_class_mapping_t_init(
    bcm_cosq_safc_class_mapping_t *mapping);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Get / set mapping of SAFC class to COSQs and / or scheduling nodes for
 * a port.
 */
extern int bcm_cosq_safc_class_mapping_set(
    int unit, 
    bcm_gport_t port, 
    int array_count, 
    bcm_cosq_safc_class_mapping_t *mapping_array);

/* 
 * Get / set mapping of SAFC class to COSQs and / or scheduling nodes for
 * a port.
 */
extern int bcm_cosq_safc_class_mapping_get(
    int unit, 
    bcm_gport_t port, 
    int array_max, 
    bcm_cosq_safc_class_mapping_t *mapping_array, 
    int *array_count);

#endif /* BCM_HIDE_DISPATCHABLE */

/* queue deadlock recovery state */
typedef enum bcm_cosq_pfc_deadlock_recovery_event_e {
    bcmCosqPfcDeadlockRecoveryEventBegin = 0, /* queue deadlock recovery begin */
    bcmCosqPfcDeadlockRecoveryEventEnd = 1, /* queue deadlock recovery end */
    bcmCosqPfcDeadlockRecoveryEventCount = 2 /* queue deadlock recovery count num */
} bcm_cosq_pfc_deadlock_recovery_event_t;

typedef int (*bcm_cosq_pfc_deadlock_recovery_event_cb_t)(
    int unit, 
    bcm_port_t port, 
    bcm_cos_queue_t cosq, 
    bcm_cosq_pfc_deadlock_recovery_event_t recovery_state, 
    void *userdata);

/* PFC Deadlock config structure for a given Priority. */
typedef struct bcm_cosq_pfc_deadlock_config_s {
    uint32 detection_timer; /* PFC Deadlock Detection Timer. */
    uint32 recovery_timer;  /* PFC Deadlock Recovery Timer. */
} bcm_cosq_pfc_deadlock_config_t;

/* PFC Deadlock config structure for UC Queue Gport. */
typedef struct bcm_cosq_pfc_deadlock_queue_config_s {
    int enable; /* PFC Deadlock config for a Queue. */
} bcm_cosq_pfc_deadlock_queue_config_t;

/* PFC Deadlock config structure for a given Priority. */
typedef struct bcm_cosq_pfc_deadlock_info_s {
    bcm_pbmp_t enabled_pbmp;    /* Bitmap of ports enabled for a given priority. */
    bcm_pbmp_t deadlock_pbmp;   /* Bitmap of ports currently in deadlock status
                                   for a given priority. */
} bcm_cosq_pfc_deadlock_info_t;

/* Initialize a COSq PFC deadlock config structure. */
extern void bcm_cosq_pfc_deadlock_config_t_init(
    bcm_cosq_pfc_deadlock_config_t *config);

/* Initialize a COSq PFC deadlock queue config structure. */
extern void bcm_cosq_pfc_deadlock_queue_config_t_init(
    bcm_cosq_pfc_deadlock_queue_config_t *q_config);

/* Initialize a COSq PFC deadlock info structure. */
extern void bcm_cosq_pfc_deadlock_info_t_init(
    bcm_cosq_pfc_deadlock_info_t *pfc_deadlock_info);

#ifndef BCM_HIDE_DISPATCHABLE

/* Start/Exit PFC Deadlock recovery process */
extern int bcm_cosq_pfc_deadlock_recovery_start(
    int unit, 
    bcm_port_t port, 
    bcm_cos_t cosq);

/* Start/Exit PFC Deadlock recovery process */
extern int bcm_cosq_pfc_deadlock_recovery_exit(
    int unit, 
    bcm_port_t port, 
    bcm_cos_t cosq);

/* Set/Get config for the given priority for the PFC Deadlock feature */
extern int bcm_cosq_pfc_deadlock_config_set(
    int unit, 
    int priority, 
    bcm_cosq_pfc_deadlock_config_t *config);

/* Set/Get config for the given priority for the PFC Deadlock feature */
extern int bcm_cosq_pfc_deadlock_config_get(
    int unit, 
    int priority, 
    bcm_cosq_pfc_deadlock_config_t *config);

/* Get bitmap of Enabled(Admin) and Current Deadlock ports status */
extern int bcm_cosq_pfc_deadlock_info_get(
    int unit, 
    int priority, 
    bcm_cosq_pfc_deadlock_info_t *pfc_deadlock_info);

/* 
 * Set/Get Config status for the given UC Queue Gport for the PFC
 * Deadlock feature
 */
extern int bcm_cosq_pfc_deadlock_queue_config_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_cosq_pfc_deadlock_queue_config_t *q_config);

/* 
 * Set/Get Config status for the given UC Queue Gport for the PFC
 * Deadlock feature
 */
extern int bcm_cosq_pfc_deadlock_queue_config_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_cosq_pfc_deadlock_queue_config_t *q_config);

/* Get the current Deadlock status for the given UC Queue Gport. */
extern int bcm_cosq_pfc_deadlock_queue_status_get(
    int unit, 
    bcm_gport_t gport, 
    uint8 *deadlock_status);

/* 
 * Register/Unregister pfc deadlock recovery callback in PFC deadlock
 * feature
 */
extern int bcm_cosq_pfc_deadlock_recovery_event_register(
    int unit, 
    bcm_cosq_pfc_deadlock_recovery_event_cb_t callback, 
    void *userdata);

/* 
 * Register/Unregister pfc deadlock recovery callback in PFC deadlock
 * feature
 */
extern int bcm_cosq_pfc_deadlock_recovery_event_unregister(
    int unit, 
    bcm_cosq_pfc_deadlock_recovery_event_cb_t callback, 
    void *userdata);

/* Configure the stat types for counters */
extern int bcm_cosq_stat_config_set(
    int unit, 
    int stat_count, 
    bcm_cosq_stat_t *stat_array);

/* Configure the stat types for counters */
extern int bcm_cosq_stat_config_get(
    int unit, 
    int stat_count, 
    bcm_cosq_stat_t *stat_array);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Cosq Direction */
typedef enum bcm_cosq_dir_e {
    bcmCosqIngress = 0,             /* Ingress Port */
    bcmCosqEgress = 1,              /* Egress Port */
    bcmCosqDirectionMaxCount = 2 
} bcm_cosq_dir_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get buffer id associated with a port. */
extern int bcm_cosq_buffer_id_multi_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_cosq_dir_t direction, 
    int array_max, 
    bcm_cosq_buffer_id_t *buf_id_array, 
    int *array_count);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Parameter for bcmCosqControlAlternateStoreForward */
typedef enum bcm_cosq_asf_mode_e {
    bcmCosqAsfModeDisabled = 0,         /* No cut-through, i.e.
                                           store-and-forward mode */
    bcmCosqAsfModeStoreAndForward = bcmCosqAsfModeDisabled, /* Store-and-forward mode */
    bcmCosqAsfModeAlternateStoreForward = 1, /* Alternate-store-forward (cut-through)
                                           mode */
    bcmCosqAsfModeCount = 2             /* Must be last. Not a usable value */
} bcm_cosq_asf_mode_t;

#if defined(INCLUDE_TCB)
/* TCB Scope type */
typedef enum bcm_cosq_tcb_scope_e {
    bcmCosqTcbScopeUcastQueue = 0,  /* Set scope to a unicast queue */
    bcmCosqTcbScopeIngressPort = 1, /* Set scope to ingress port */
    bcmCosqTcbScopeEgressPort = 2,  /* Set scope to egress port */
    bcmCosqTcbScopeIngressPipe = 3, /* Set scope to ingress pipe */
    bcmCosqTcbScopeEgressPipe = 4,  /* Set scope to egress pipe */
    bcmCosqTcbScopeMaxCount = 5 
} bcm_cosq_tcb_scope_t;
#endif

#if defined(INCLUDE_TCB)
/* TCB Config Structure. */
typedef struct bcm_cosq_tcb_config_s {
    bcm_cosq_tcb_scope_t scope_type;    /* TCB monitor scope type */
    bcm_gport_t gport;                  /* TCB monitor entity, can be a Unicast
                                           queue gport or a physical port gport */
    bcm_cos_queue_t cosq;               /* Reserved field */
    uint32 trigger_reason;              /* Define the event which could trigger
                                           the capture */
    uint32 pre_freeze_capture_num;      /* Define the capture number after TCB
                                           enter freeze status */
    uint32 pre_freeze_capture_time;     /* Define the capture time after TCB
                                           enter freeze status. Unit is usec */
    uint32 post_sample_probability;     /* The sample probability in
                                           post-trigger phase. Step is 1/16,
                                           valid configure range is 1 to 16, 1
                                           means 1/16 sample, 16 means sample
                                           all */
    uint32 pre_sample_probability;      /* The sample probability in pre-trigger
                                           phase. Step is 1/16, valid configure
                                           range is 1 to 16, 1 means 1/16
                                           sample, 16 means sample all */
    int pipe_id;                        /* Pipe id */
} bcm_cosq_tcb_config_t;
#endif

#if defined(INCLUDE_TCB)
/* Initialize a tcb config structure. */
extern void bcm_cosq_tcb_config_t_init(
    bcm_cosq_tcb_config_t *config);
#endif

#ifndef BCM_HIDE_DISPATCHABLE

#if defined(INCLUDE_TCB)
/* Set/Get the tcb configuration. */
extern int bcm_cosq_tcb_config_set(
    int unit, 
    bcm_cosq_buffer_id_t buffer_id, 
    bcm_cosq_tcb_config_t *config);
#endif

#if defined(INCLUDE_TCB)
/* Set/Get the tcb configuration. */
extern int bcm_cosq_tcb_config_get(
    int unit, 
    bcm_cosq_buffer_id_t buffer_id, 
    bcm_cosq_tcb_config_t *config);
#endif

#endif /* BCM_HIDE_DISPATCHABLE */

#if defined(INCLUDE_TCB)
/* TCB threshold profile. */
typedef struct bcm_cosq_tcb_threshold_profile_s {
    uint32 start_threshold_bytes;   /* The start threshold above which queue
                                       start the capture */
    uint32 stop_threshold_bytes;    /* The stop threshold below which queue stop
                                       the capture */
} bcm_cosq_tcb_threshold_profile_t;
#endif

#if defined(INCLUDE_TCB)
/* Initialize a tcb threshold profile structure. */
extern void bcm_cosq_tcb_threshold_profile_t_init(
    bcm_cosq_tcb_threshold_profile_t *threshold);
#endif

#ifndef BCM_HIDE_DISPATCHABLE

#if defined(INCLUDE_TCB)
/* Create tcb threshold profile. */
extern int bcm_cosq_tcb_threshold_profile_create(
    int unit, 
    int flags, 
    bcm_cosq_buffer_id_t buffer_id, 
    bcm_cosq_tcb_threshold_profile_t *threshold, 
    int *profile_index);
#endif

#if defined(INCLUDE_TCB)
/* Get a tcb threshold profile. */
extern int bcm_cosq_tcb_threshold_profile_get(
    int unit, 
    bcm_cosq_buffer_id_t buffer_id, 
    int profile_index, 
    bcm_cosq_tcb_threshold_profile_t *threshold);
#endif

#if defined(INCLUDE_TCB)
/* Destroy a tcb threshold profile. */
extern int bcm_cosq_tcb_threshold_profile_destroy(
    int unit, 
    bcm_cosq_buffer_id_t buffer_id, 
    int profile_index);
#endif

#if defined(INCLUDE_TCB)
/* Set/Get threshold profile for UCQ. */
extern int bcm_cosq_tcb_gport_threshold_mapping_set(
    int unit, 
    bcm_cosq_object_id_t *id, 
    int profile_index);
#endif

#if defined(INCLUDE_TCB)
/* Set/Get threshold profile for UCQ. */
extern int bcm_cosq_tcb_gport_threshold_mapping_get(
    int unit, 
    bcm_cosq_object_id_t *id, 
    int *profile_index);
#endif

#endif /* BCM_HIDE_DISPATCHABLE */

#if defined(INCLUDE_TCB)
#define BCM_COSQ_TCB_MAX_BUF_SIZE   80         /* TCB buffer record size */
#endif

#if defined(INCLUDE_TCB)
/* TCB Pkt Record Structure. */
typedef struct bcm_cosq_tcb_buffer_s {
    uint64 timestamp;                   /* timestamp */
    bcm_gport_t ingress_gport;          /* ingress port gport. */
    bcm_gport_t egress_gport;           /* egress port gport. */
    bcm_gport_t queue;                  /* Egress UC queue gport */
    uint32 queue_size;                  /* Queue depth, unit is bytes */
    uint32 service_pool_size;           /* Service pool depth, unit is bytes */
    uint8 buf_data[BCM_COSQ_TCB_MAX_BUF_SIZE]; /* The first 80 bytes of captured packet */
} bcm_cosq_tcb_buffer_t;
#endif

#ifndef BCM_HIDE_DISPATCHABLE

#if defined(INCLUDE_TCB)
/* Get packet records in TCB buffer. */
extern int bcm_cosq_tcb_buffer_multi_get(
    int unit, 
    bcm_cosq_buffer_id_t buffer_id, 
    int array_max, 
    bcm_cosq_tcb_buffer_t *buffer_array, 
    int *array_count);
#endif

#endif /* BCM_HIDE_DISPATCHABLE */

#if defined(INCLUDE_TCB)
/* TCB Drop Event Record Structure. */
typedef struct bcm_cosq_tcb_event_s {
    uint64 timestamp;           /* timestamp */
    bcm_gport_t ingress_gport;  /* ingress port gport. */
    bcm_gport_t egress_gport;   /* egress port gport. */
    bcm_gport_t queue;          /* Egress UC queue gport */
    uint32 queue_size;          /* Queue depth, unit is bytes */
    uint32 service_pool_size;   /* Service pool depth, unit is bytes */
    uint32 drop_reason;         /* Drop reason vector */
    bcm_color_t color;          /* Packet color */
} bcm_cosq_tcb_event_t;
#endif

#ifndef BCM_HIDE_DISPATCHABLE

#if defined(INCLUDE_TCB)
/* Get drop event records in TCB buffer. */
extern int bcm_cosq_tcb_event_multi_get(
    int unit, 
    bcm_cosq_buffer_id_t buffer_id, 
    int array_max, 
    bcm_cosq_tcb_event_t *event_array, 
    int *array_count, 
    int *overflow_count);
#endif

#endif /* BCM_HIDE_DISPATCHABLE */

#if defined(INCLUDE_TCB)
/* TCB control type */
typedef enum bcm_cosq_tcb_control_e {
    bcmCosqTcbControlEnable = 0,    /* Enable or disable TCB instance */
    bcmCosqTcbControlFreeze = 1,    /* Freeze or restart TCB instance */
    bcmCosqTcbControlMaxCount = 2 
} bcm_cosq_tcb_control_t;
#endif

#ifndef BCM_HIDE_DISPATCHABLE

#if defined(INCLUDE_TCB)
/* Set/get TCB working status. */
extern int bcm_cosq_tcb_control_set(
    int unit, 
    bcm_cosq_buffer_id_t buffer_id, 
    bcm_cosq_tcb_control_t type, 
    int arg);
#endif

#if defined(INCLUDE_TCB)
/* Set/get TCB working status. */
extern int bcm_cosq_tcb_control_get(
    int unit, 
    int buffer_id, 
    bcm_cosq_tcb_control_t type, 
    int *arg);
#endif

#endif /* BCM_HIDE_DISPATCHABLE */

/* Callback function type for tcb event handling. */
#if defined(INCLUDE_TCB)
typedef void (*bcm_cosq_tcb_callback_t)(
    int unit, 
    bcm_cosq_buffer_id_t buffer_id, 
    void *user_data);
#endif

#ifndef BCM_HIDE_DISPATCHABLE

#if defined(INCLUDE_TCB)
/* 
 * Register/unregister a callback function to be called when TCB buffer
 * enter freeze status.
 */
extern int bcm_cosq_tcb_cb_register(
    int unit, 
    bcm_cosq_tcb_callback_t fn, 
    void *user_data);
#endif

#if defined(INCLUDE_TCB)
/* 
 * Register/unregister a callback function to be called when TCB buffer
 * enter freeze status.
 */
extern int bcm_cosq_tcb_cb_unregister(
    int unit, 
    bcm_cosq_tcb_callback_t fn);
#endif

#endif /* BCM_HIDE_DISPATCHABLE */

#if defined(INCLUDE_TCB)
/* TCB Threshold profile configure flags */
#define BCM_COSQ_TCB_THRESHOLD_PROFILE_WITH_ID 0x0001     /* TCB threshold profile
                                                          configure with a
                                                          profile index */
#define BCM_COSQ_TCB_THRESHOLD_PROFILE_REPLACE 0x0002     /* TCB threshold profile
                                                          replace, must be used
                                                          with
                                                          COSQ_TCB_THRESHOLD_PROFILE_WITH_ID */
#endif

#if defined(INCLUDE_TCB)
/* TCB Track Trigger Vector */
#define BCM_COSQ_TCB_INGRESS_ADMIN_DROP 0x0001     /* Ingress admission drop can
                                                      cause a trigger */
#define BCM_COSQ_TCB_EGRESS_ADMIN_DROP  0x0002     /* Egress admission drop can
                                                      cause a trigger */
#define BCM_COSQ_TCB_WRED_DROP          0x0004     /* Weighted random drop can
                                                      cause a trigger */
#define BCM_COSQ_TCB_CFAP_DROP          0x0008     /* Cell free allocations pool
                                                      drop can cause a trigger */
#endif

#define BCM_COSQ_INVALID_BUF_ID -1         /* Invalid Buffer ID */

/* cosq_pkt_size_adjust_source */
typedef enum bcm_cosq_pkt_size_adjust_source_e {
    bcmCosqPktSizeAdjustSourceScheduler = 0, /* source is Scheduler */
    bcmCosqPktSizeAdjustSourceCrpsInPP = 1, /* source is CRPS in PP */
    bcmCosqPktSizeAdjustSourceCrpsInTM = 2, /* source is CRPS in TM */
    bcmCosqPktSizeAdjustSourceStatReportIn = 3, /* source is Stat Report In */
    bcmCosqPktSizeAdjustSourceStatReportOut = 4, /* source is Stat Report out */
    bcmCosqPktSizeAdjustSourceGlobal = 5 /* For Scheduler and CRPS */
} bcm_cosq_pkt_size_adjust_source_t;

/* cosq ingress port drop priority maps */
typedef enum bcm_cosq_ingress_port_drop_map_e {
    bcmCosqIngressPortDropTmTcDpPriorityTable = 0, 
    bcmCosqIngressPortDropIpDscpToPriorityTable = 1, 
    bcmCosqIngressPortDropEthPcpDeiToPriorityTable = 2, 
    bcmCosqIngressPortDropMplsExpToPriorityTable = 3 
} bcm_cosq_ingress_port_drop_map_t;

/* cosq ingress congestion resource statistics type */
typedef enum bcm_cosq_icgm_resource_stat_type_e {
    bcmCosqICgmMinFreeBDB = 0,          /* Minimum free BDB watermark */
    bcmCosqICgmMinFreeOcbBuffers = 1,   /* Minimum free OCB buffers watermark */
    bcmCosqICgmMinFreeMiniMcBuffers = 2, /* Minimum free Mini-Multicast buffers
                                           watermark */
    bcmCosqICgmMinFreeFullMcBuffers = 3, /* Minimum free Full-Multicast buffers
                                           watermark */
    bcmCosqICgmMinFreeVoqDramBDB = 4,   /* Minimum free VOQ DRAM BDB watermark */
    bcmCosqICgmMinFreeVoqOcbBDB = 5,    /* Minimum free VOQ OCB BDB watermark */
    bcmCosqICgmMinFreeSramBuffers = 6,  /* Minimum free SRAM buffers watermark */
    bcmCosqICgmMinFreeSramPDB = 7,      /* Minimum free SRAM PDB watermark */
    bcmCosqICgmCurrSharedBytes = 8,     /* Current bytes size in shared space */
    bcmCosqICgmCurrSharedOcbBuffers = 9, /* Current OCB-Buffers size in shared
                                           space */
    bcmCosqICgmCurrSharedOcbPDs = 10,   /* Current OCB-PDs size in shared space */
    bcmCosqICgmCurrGrntdBytes = 11,     /* Current bytes size in guaranteed
                                           space */
    bcmCosqICgmCurrGrntdOcbBuffers = 12, /* Current OCB-Buffers size in
                                           guaranteed space */
    bcmCosqICgmCurrGrntdOcbPDs = 13,    /* Current OCB-PDs size in guaranteed
                                           space */
    bcmCosqICgmCurrHdrmBytes = 14,      /* Current bytes size in headroom space */
    bcmCosqICgmCurrHdrmOcbBuffers = 15, /* Current OCB-Buffers size in headroom
                                           space */
    bcmCosqICgmCurrHdrmOcbPDs = 16,     /* Current OCB-PDs size in headroom
                                           space */
    bcmCosqICgmMaxSharedBytes = 17      /* Maximal bytes size in shared space */
} bcm_cosq_icgm_resource_stat_type_t;

/* cosq_pkt_size_adjust_source_info structure */
typedef struct bcm_cosq_pkt_size_adjust_source_info_s {
    bcm_cosq_pkt_size_adjust_source_t source_type; /* compensation source */
    uint32 source_id;                   /* command id: 0,1 */
} bcm_cosq_pkt_size_adjust_source_info_t;

/* cosq_pkt_size_adjust_info structure */
typedef struct bcm_cosq_pkt_size_adjust_info_s {
    uint32 flags;                       /* flags */
    bcm_gport_t gport;                  /* gport value */
    bcm_cos_queue_t cosq;               /* bcm_cos_queue_t */
    bcm_cosq_pkt_size_adjust_source_info_t source_info; /* adjust source */
} bcm_cosq_pkt_size_adjust_info_t;

/* cosq_icgm_resource_stat_key structure */
typedef struct bcm_cosq_icgm_resource_stat_key_s {
    bcm_gport_t gport;                  /* gport value */
    uint32 flags;                       /* flags */
    bcm_cosq_icgm_resource_stat_type_t stat_type; /* statistics type */
} bcm_cosq_icgm_resource_stat_key_t;

/* 
 * This structure contain the latency data that was measured in the
 * ingress for a packet
 */
typedef struct bcm_cosq_max_latency_pkts_s {
    bcm_gport_t dest_gport; /* will return modport gport */
    bcm_cos_queue_t cosq;   /* cosq */
    uint64 latency;         /* latency measured */
    uint32 latency_flow;    /* latency flow */
    uint32 is_mc;           /* is MC */
} bcm_cosq_max_latency_pkts_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* get cosq ingress congestion resource statistics */
extern int bcm_cosq_icgm_resource_stat_get(
    int unit, 
    bcm_cosq_icgm_resource_stat_key_t *stat_key, 
    uint64 *value);

/* 
 * The purpose of this API is to configure a delta value for a source or
 * enable/disable a certain compensation type.
 */
extern int bcm_cosq_gport_pkt_size_adjust_set(
    int unit, 
    bcm_cosq_pkt_size_adjust_info_t *adjust_info, 
    int delta);

/* 
 * The purpose of this API is to get the delta value configured for a
 * certain compensation type
 */
extern int bcm_cosq_gport_pkt_size_adjust_get(
    int unit, 
    bcm_cosq_pkt_size_adjust_info_t *adjust_info, 
    int *delta);

/* Scheduler final delta mapping API */
extern int bcm_cosq_pkt_size_adjust_delta_map_set(
    int unit, 
    int delta, 
    int final_delta);

/* get the final delta mapping */
extern int bcm_cosq_pkt_size_adjust_delta_map_get(
    int unit, 
    int delta, 
    int *final_delta);

#endif /* BCM_HIDE_DISPATCHABLE */

/* cosq ingress port drop enable modes */
typedef enum bcm_cosq_ingress_port_drop_enable_mode_e {
    bcmCosqIngressPortDropDisable = 0, 
    bcmCosqIngressPortDropEnableHardStage = 1, 
    bcmCosqIngressPortDropEnableHardAndSoftStage = 2 
} bcm_cosq_ingress_port_drop_enable_mode_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* set/get enable/disable cosq ingress port drop */
extern int bcm_cosq_ingress_port_drop_enable_set(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    int enable_mode);

/* set/get enable/disable cosq ingress port drop */
extern int bcm_cosq_ingress_port_drop_enable_get(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    int *enable_mode);

/* set/get cosq ingress port drop thresholds */
extern int bcm_cosq_ingress_port_drop_threshold_set(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    int priority, 
    uint32 value);

/* set/get cosq ingress port drop thresholds */
extern int bcm_cosq_ingress_port_drop_threshold_get(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    int priority, 
    uint32 *value);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Definitions used to select priority in API
 * bcm_cosq_ingress_port_drop_map_set()
 */
#define BCM_COSQ_INGRESS_PORT_DROP_PRIORITY_0 0          /* user defined priority
                                                          - 0 */
#define BCM_COSQ_INGRESS_PORT_DROP_PRIORITY_1 1          /* user defined priority
                                                          - 1 */
#define BCM_COSQ_INGRESS_PORT_DROP_PRIORITY_2 2          /* user defined priority
                                                          - 2 */
#define BCM_COSQ_INGRESS_PORT_DROP_PRIORITY_3 3          /* user defined priority
                                                          - 3 */
#define BCM_COSQ_INGRESS_PORT_DROP_PRIORITY_TDM 15         /* user defined priority
                                                          - TDM */

#ifndef BCM_HIDE_DISPATCHABLE

/* set/get cosq ingress port drop map value */
extern int bcm_cosq_ingress_port_drop_map_set(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    bcm_cosq_ingress_port_drop_map_t map, 
    uint32 key, 
    int priority);

/* set/get cosq ingress port drop map value */
extern int bcm_cosq_ingress_port_drop_map_get(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    bcm_cosq_ingress_port_drop_map_t map, 
    uint32 key, 
    int *priority);

/* get cosq ingress congestion resource statistics */
extern int bcm_cosq_max_latency_pkts_get(
    int unit, 
    bcm_gport_t gport, 
    uint32 flags, 
    int max_count, 
    bcm_cosq_max_latency_pkts_t *max_latency_pkts, 
    int *actual_count);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * User defined properties to be recgnized as control frames by the
 * ingress port drop.
 */
typedef struct bcm_cosq_ingress_drop_control_frame_config_s {
    uint64 mac_da_val;              /* MAC DA to recognize as Control Plane
                                       packet */
    uint64 mac_da_mask;             /* MAC DA mask */
    uint32 ether_type_code;         /* Eth type to recognize as Control Plane
                                       packet */
    uint32 ether_type_code_mask;    /* Eth type code mask */
} bcm_cosq_ingress_drop_control_frame_config_t;

/* Initialize a CoSQ ingress drop control frame configuration structure. */
extern void bcm_cosq_ingress_drop_control_frame_config_t_init(
    bcm_cosq_ingress_drop_control_frame_config_t *config);

#ifndef BCM_HIDE_DISPATCHABLE

/* set cosq ingress port drop control frame properties. */
extern int bcm_cosq_ingress_port_drop_control_frame_set(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    uint32 control_frame_index, 
    bcm_cosq_ingress_drop_control_frame_config_t *control_frame_config);

/* get cosq ingress port drop control frame properties. */
extern int bcm_cosq_ingress_port_drop_control_frame_get(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    uint32 control_frame_index, 
    bcm_cosq_ingress_drop_control_frame_config_t *control_frame_config);

/* 
 * configure cosq ingress port drop configurable Ether type to Ether type
 * code.
 */
extern int bcm_cosq_ingress_port_drop_custom_ether_type_set(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    uint32 ether_type_code, 
    uint32 ether_type_val);

/* 
 * configure cosq ingress port drop configurable Ether type to Ether type
 * code.
 */
extern int bcm_cosq_ingress_port_drop_custom_ether_type_get(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    uint32 ether_type_code, 
    uint32 *ether_type_val);

/* configure cosq ingress port drop default priority. */
extern int bcm_cosq_ingress_port_drop_default_priority_set(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    uint32 default_priority);

/* configure cosq ingress port drop default priority. */
extern int bcm_cosq_ingress_port_drop_default_priority_get(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    uint32 *default_priority);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Cosq ingress port drop max number of fields used to build a flex key. */
#define BCM_COSQ_INGRESS_PORT_DROP_MAX_FLEX_KEY_FIELDS 4          

/* 
 * Cosq ingress port drop unique information used when building a flex
 * key.
 */
typedef struct bcm_cosq_ingress_drop_flex_key_construct_id_s {
    bcm_port_t port;        /* port */
    uint32 ether_type_code; /* Ether type code. Key is constructed per ether
                               type. */
} bcm_cosq_ingress_drop_flex_key_construct_id_t;

/* Initialize a CoSQ ingress drop flex key construct id structure. */
extern void bcm_cosq_ingress_drop_flex_key_construct_id_t_init(
    bcm_cosq_ingress_drop_flex_key_construct_id_t *config);

typedef enum bcm_cosq_ingress_port_drop_flex_key_base_e {
    bcmCosqIngressPortDropFlexKeyBasePacketStart = 0, 
    bcmCosqIngressPortDropFlexKeyBaseEndOfEthHeader = 1, 
    bcmCosqIngressPortDropFlexKeyBaseEndOfHeaderAfterEthHeader = 2 
} bcm_cosq_ingress_port_drop_flex_key_base_t;

/* Cosq ingress port drop flex key construction information. */
typedef struct bcm_cosq_ingress_drop_flex_key_construct_s {
    uint32 array_size;                  /* number of elements in the offset
                                           array */
    uint32 offset_array[BCM_COSQ_INGRESS_PORT_DROP_MAX_FLEX_KEY_FIELDS]; /* array of offsets from which the flex
                                           key will be created */
    bcm_cosq_ingress_port_drop_flex_key_base_t offset_base; /* The offset array to compose the key
                                           can start from several base points:
                                           start of packet, end of eth header,
                                           end of header after eth header. */
    uint32 ether_type_header_size;      /* When the key is composed for a
                                           configurable ether type (ether type
                                           code 1-6), and the offset base is end
                                           of header after eth header, the size
                                           of the header after the eth must be
                                           provided (in bytes). In all other
                                           cases this value is expected to be
                                           -1. */
} bcm_cosq_ingress_drop_flex_key_construct_t;

/* Initialize a CoSQ ingress drop flex key construct structure. */
extern void bcm_cosq_ingress_drop_flex_key_construct_t_init(
    bcm_cosq_ingress_drop_flex_key_construct_t *config);

#ifndef BCM_HIDE_DISPATCHABLE

/* set/get cosq ingress port drop flex key construct properties. */
extern int bcm_cosq_ingress_port_drop_flex_key_construct_set(
    int unit, 
    bcm_cosq_ingress_drop_flex_key_construct_id_t *key_id, 
    uint32 flags, 
    bcm_cosq_ingress_drop_flex_key_construct_t *flex_key_config);

/* set/get cosq ingress port drop flex key construct properties. */
extern int bcm_cosq_ingress_port_drop_flex_key_construct_get(
    int unit, 
    bcm_cosq_ingress_drop_flex_key_construct_id_t *key_id, 
    uint32 flags, 
    bcm_cosq_ingress_drop_flex_key_construct_t *flex_key_config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Cosq ingress port drop flex key information. */
typedef struct bcm_cosq_ingress_drop_flex_key_s {
    uint32 value;   /* flex key value */
    uint32 mask;    /* flex key mask */
} bcm_cosq_ingress_drop_flex_key_t;

/* Initialize a CoSQ ingress drop flex key structure. */
extern void bcm_cosq_ingress_drop_flex_key_t_init(
    bcm_cosq_ingress_drop_flex_key_t *config);

/* Cosq ingress port drop flex key entry information. */
typedef struct bcm_cosq_ingress_drop_flex_key_entry_s {
    bcm_cosq_ingress_drop_flex_key_t ether_code; /* ether code information for the flex
                                           key entry */
    uint32 num_key_fields;              /* number of elements in the key_fields
                                           array */
    bcm_cosq_ingress_drop_flex_key_t key_fields[BCM_COSQ_INGRESS_PORT_DROP_MAX_FLEX_KEY_FIELDS]; /* key fields array to build the flex
                                           key entry */
    uint32 priority;                    /* priority to associate with the flex
                                           key entry */
} bcm_cosq_ingress_drop_flex_key_entry_t;

/* Initialize a CoSQ ingress drop flex key entry structure. */
extern void bcm_cosq_ingress_drop_flex_key_entry_t_init(
    bcm_cosq_ingress_drop_flex_key_entry_t *config);

#ifndef BCM_HIDE_DISPATCHABLE

/* set/get cosq ingress port drop flex key entry. */
extern int bcm_cosq_ingress_port_drop_flex_key_entry_set(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    uint32 key_index, 
    bcm_cosq_ingress_drop_flex_key_entry_t *flex_key_info);

/* set/get cosq ingress port drop flex key entry. */
extern int bcm_cosq_ingress_port_drop_flex_key_entry_get(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    uint32 key_index, 
    bcm_cosq_ingress_drop_flex_key_entry_t *flex_key_info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Type of FADT threshold. */
typedef enum bcm_cosq_gport_fadt_threshold_type_e {
    bcmCosqFadtDramBound = 0,           /* DRAM bound settings. */
    bcmCosqFadtDramBoundRecoveryFailure = 1, /* DRAM recovery failure settings. */
    bcmCosqFadtCongestionNotification = 2 /* Congestion notification settings */
} bcm_cosq_gport_fadt_threshold_type_t;

/* Complementary information for FADT threshold. */
typedef struct bcm_cosq_fadt_info_s {
    bcm_gport_t gport;                  /* target gport */
    bcm_cos_queue_t cosq;               /* target cosq */
    bcm_cosq_gport_fadt_threshold_type_t thresh_type; /* threshold type */
    bcm_cosq_resource_t resource_type;  /* resource type */
} bcm_cosq_fadt_info_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* set/get FADT threshold parameters. */
extern int bcm_cosq_gport_fadt_threshold_set(
    int unit, 
    uint32 flags, 
    bcm_cosq_fadt_info_t *fadt_info, 
    bcm_cosq_fadt_threshold_t *threshold);

/* set/get FADT threshold parameters. */
extern int bcm_cosq_gport_fadt_threshold_get(
    int unit, 
    uint32 flags, 
    bcm_cosq_fadt_info_t *fadt_info, 
    bcm_cosq_fadt_threshold_t *threshold);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Init function to the structure "bcm_cosq_fadt_info_t". */
extern void bcm_cosq_fadt_info_t_init(
    bcm_cosq_fadt_info_t *info);

/* Flags for bcm_cosq_pfc_config_set */
#define BCM_COSQ_PFC_BYTES              0x00000001 /* Threshold in bytes */
#define BCM_COSQ_PFC_OCB                0x00000002 /* Threshold for OCB (SRAM)
                                                      resources */
#define BCM_COSQ_PFC_PACKET_DESC        0x00000004 /* Threshold in packet
                                                      descriptors */
#define BCM_COSQ_PFC_BUFFERS            0x00000008 /* Threshold in buffers */
#define BCM_COSQ_PFC_USE_FADT_PARAMS    0x00000010 /* Use FADT parameters */
#define BCM_COSQ_PFC_POOL_0             0x00000020 /* Apply on pool 0 */
#define BCM_COSQ_PFC_POOL_1             0x00000040 /* Apply on pool 1 */

/* TAS control type */
typedef enum bcm_cosq_tas_control_e {
    bcmCosqTASControlTASEnable = 0,     /* Enable the TAS. */
    bcmCosqTASControlUsePTPTime = 1,    /* PTP time is used for TAS synchronous
                                           operation. */
    bcmCosqTASControlInitGateState = 2, /* Gate state when TAS is disabled. Or
                                           the init gate state in the *Restart*
                                           process. */
    bcmCosqTASControlErrGateState = 3,  /* Gate state when error condition
                                           occurs during an old cycle is
                                           running. */
    bcmCosqTASControlGatePurgeEnable = 4, /* Purge express packet when gate is
                                           closed. */
    bcmCosqTASControlGateControlTickInterval = 5, /* Gate control ticks interval in ns. */
    bcmCosqTASControlTASPTPLock = 6,    /* Indicate the PTP time is locked or
                                           out of sync. This type is on system
                                           basis. */
    bcmCosqTASCountrolCount = 7         /* This should be the last one. */
} bcm_cosq_tas_control_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/get TAS control configurations. */
extern int bcm_cosq_tas_control_set(
    int unit, 
    bcm_gport_t port, 
    bcm_cosq_tas_control_t type, 
    int arg);

/* Set/get TAS control configurations. */
extern int bcm_cosq_tas_control_get(
    int unit, 
    bcm_gport_t port, 
    bcm_cosq_tas_control_t type, 
    int *arg);

#endif /* BCM_HIDE_DISPATCHABLE */

/* TAS status type */
typedef enum bcm_cosq_tas_status_e {
    bcmCosqTASStatusGateQueueState = 0, /*  The current gate state of each
                                           traffic class queue. The value
                                           represents all queues' gate state
                                           bitmap, each bit indicates to each
                                           queue. */
    bcmCosqTASStatusGateStateSet = 1,   /* Reflect which control gate source is
                                           selected. The value = 0 indicates the
                                           gate state is from INIT GATE STATE
                                           settings, value = 1 indicates the
                                           gate state is from active control
                                           list table, value = 2 indicates the
                                           gate state is from ERR GATE STATE
                                           settings */
    bcmCosqTASStatusTickGranularity = 2, /*  The granularity of the calendar
                                           table time clock in ns. */
    bcmCosqTASStatusMaxCalendarEntries = 3, /* The max entries could be support in
                                           calendar table. */
    bcmCosqTASStatusCount = 4           /* This should be the last one. */
} bcm_cosq_tas_status_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get the TAS status */
extern int bcm_cosq_tas_status_get(
    int unit, 
    bcm_gport_t port, 
    bcm_cosq_tas_status_t type, 
    int *arg);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Cosq event type */
typedef enum bcm_cosq_event_type_e {
    bcmCosqEventTASProfileChange = 0,   /* TAS: the profile change is completed. */
    bcmCosqEventTASECCErr = 1,          /* TAS: uncorrectable ECC error in
                                           profile calendar table. */
    bcmCosqEventTASNextCycleTimeErr = 2, /* TAS: next cycle start time has
                                           passed. */
    bcmCosqEventTASBaseTimeErr = 3,     /* TAS: base time for the new profile
                                           has passed. */
    bcmCosqEventTASProfilePtrErr = 4,   /* TAS: index to the profile calendar
                                           table is overrun. */
    bcmCosqEventTASPTPOutOfSyncErr = 5, /* TAS: IEEE1588 PTP time is out of
                                           sync. */
    bcmCosqEventCount = 6               /* This should be the last one. */
} bcm_cosq_event_type_t;

typedef struct bcm_cosq_event_types_s {
    SHR_BITDCL w[_SHR_BITDCLSIZE(bcmCosqEventCount)]; 
} bcm_cosq_event_types_t;

/* Callback function type for COSQ event handling */
typedef int (*bcm_cosq_event_cb)(
    int unit, 
    bcm_cosq_event_type_t event_type, 
    bcm_gport_t port, 
    bcm_cos_queue_t cosq, 
    void *user_data);

#define BCM_COSQ_EVENT_TYPE_SET(_event_types, _event_type)  SHR_BITSET(((_event_types).w), (_event_type)) 

#define BCM_COSQ_EVENT_TYPE_GET(_event_types, _event_type)  SHR_BITGET(((_event_types).w), (_event_type)) 

#define BCM_COSQ_EVENT_TYPE_CLEAR(_event_types, _event_type)  SHR_BITCLR(((_event_types).w), (_event_type)) 

#define BCM_COSQ_EVENT_TYPE_SET_ALL(_event_types)  SHR_BITSET_RANGE(((_event_types).w), 0, bcmCosqEventCount) 

#define BCM_COSQ_EVENT_TYPE_CLEAR_ALL(_event_types)  SHR_BITCLR_RANGE(((_event_types).w), 0, bcmCosqEventCount) 

#ifndef BCM_HIDE_DISPATCHABLE

/* Register/unregister a callback to handle the Cosq events */
extern int bcm_cosq_event_register(
    int unit, 
    bcm_cosq_event_types_t event_types, 
    bcm_gport_t port, 
    bcm_cos_queue_t cosq, 
    bcm_cosq_event_cb cb, 
    void *user_data);

/* Register/unregister a callback to handle the Cosq events */
extern int bcm_cosq_event_unregister(
    int unit, 
    bcm_cosq_event_types_t event_types, 
    bcm_gport_t port, 
    bcm_cos_queue_t cosq, 
    bcm_cosq_event_cb cb);

#endif /* BCM_HIDE_DISPATCHABLE */

/* TAS profile id type */
typedef int bcm_cosq_tas_profile_id_t;

/* The value used to assigned in flags field on bcm_cosq_tas_entry_t */
#define BCM_COSQ_TAS_ENTRY_F_PREEMPT    0x00000001 /* Request to hold the
                                                      preempt traffic */

/* 
 * The encoded value used to assigned in ticks field on
 * bcm_cosq_tas_entry_t
 */
#define BCM_COSQ_TAS_ENTRY_TICKS_STAY       (-1)       /* Specify to stay in
                                                          this entry forever in
                                                          non PTP_MODE or until
                                                          the next cycle in
                                                          PTP_MODE. */
#define BCM_COSQ_TAS_ENTRY_TICKS_GO_FIRST   (0)        /* Specify to jump to the
                                                          first entry. The state
                                                          of this entry will be
                                                          ignored. */

/* The TAS entry data structure */
typedef struct bcm_cosq_tas_entry_s {
    uint32 ticks;   /* The scheduling interval in ticks. The tick interval is
                       defined in bcmCosqTASControlGateControlTickInterval. */
    uint32 state;   /* Specify gate state for each traffic class queue in
                       bitmap. (1 bit per queue) */
    uint32 flags;   /* Defined in BCM_COSQ_TAS_ENTRY_F_XXX */
} bcm_cosq_tas_entry_t;

/* The maximum TAS calendar table size. */
#define BCM_COSQ_MAX_TAS_CALENDAR_TABLE_SIZE (128)      

/* The TAS profile data structure */
typedef struct bcm_cosq_tas_profile_s {
    int num_entries;                    /* Specify the number of entries in the
                                           calendar table. */
    bcm_cosq_tas_entry_t entries[BCM_COSQ_MAX_TAS_CALENDAR_TABLE_SIZE]; /* Specify the interval, gate states,
                                           attribute entry sets in the calendar
                                           table. */
    bcm_ptp_timestamp_t ptp_base_time;  /* Specify base time in PTP time. */
    uint32 ptp_cycle_time;              /* Specify cycle time in ns. */
    uint32 ptp_max_cycle_extension;     /* Specify max cycle extension time in
                                           ns. Maximum time allowed for a cycle
                                           to be extended before profile change. */
} bcm_cosq_tas_profile_t;

/* Initialize the bcm_cosq_tas_profile_t structure */
extern void bcm_cosq_tas_profile_t_init(
    bcm_cosq_tas_profile_t *profile);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create the TAS profile */
extern int bcm_cosq_tas_profile_create(
    int unit, 
    bcm_gport_t port, 
    bcm_cosq_tas_profile_t *profile, 
    bcm_cosq_tas_profile_id_t *pid);

/* Destroy the TAS profile */
extern int bcm_cosq_tas_profile_destroy(
    int unit, 
    bcm_gport_t port, 
    bcm_cosq_tas_profile_id_t pid);

/* Destroy TAS profile. */
extern int bcm_cosq_tas_profile_destroy_all(
    int unit, 
    bcm_gport_t port);

/* Get/set TAS profile information */
extern int bcm_cosq_tas_profile_set(
    int unit, 
    bcm_gport_t port, 
    bcm_cosq_tas_profile_id_t pid, 
    bcm_cosq_tas_profile_t *profile);

/* Get/set TAS profile information */
extern int bcm_cosq_tas_profile_get(
    int unit, 
    bcm_gport_t port, 
    bcm_cosq_tas_profile_id_t pid, 
    bcm_cosq_tas_profile_t *profile);

/* Commit the TAS profile */
extern int bcm_cosq_tas_profile_commit(
    int unit, 
    bcm_gport_t port, 
    bcm_cosq_tas_profile_id_t pid);

#endif /* BCM_HIDE_DISPATCHABLE */

/* TAS profile traverse callback */
typedef int (*bcm_cosq_tas_profile_traverse_cb)(
    int unit, 
    bcm_gport_t port, 
    bcm_cosq_tas_profile_id_t pid, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse the TAS profile */
extern int bcm_cosq_tas_profile_traverse(
    int unit, 
    bcm_gport_t port, 
    bcm_cosq_tas_profile_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* TAS profile state */
typedef enum bcm_cosq_tas_profile_state_e {
    bcmCosqTASProfileInit = 0,      /* the state after profile created. */
    bcmCosqTASProfileCommitted = 1, /* the state after profile committed but not
                                       yet written to HW table */
    bcmCosqTASProfilePending = 2,   /*  the state after profile is set to
                                       hardware but not yet effective */
    bcmCosqTASProfileActive = 3,    /* the state indicate the profile is
                                       effective */
    bcmCosqTASProfileExpired = 4,   /* The profile is no longer effective. */
    bcmCosqTASProfileError = 5,     /* the state indicate the profile is error
                                       when any error event happens */
    bcmCosTASProfileCount = 6       /* This should be the last one. */
} bcm_cosq_tas_profile_state_t;

/* The TAS profile status structure */
typedef struct bcm_cosq_tas_profile_status_s {
    bcm_cosq_tas_profile_state_t profile_state; /* profile state */
    bcm_ptp_timestamp_t config_change_time; /* The actual config change time */
    int num_entries;                    /* The actual number of entries in
                                           hardware calendar table. */
    bcm_cosq_tas_entry_t entries[BCM_COSQ_MAX_TAS_CALENDAR_TABLE_SIZE]; /* The actual entries in hardware
                                           calendar table. */
} bcm_cosq_tas_profile_status_t;

/* Initialize the bcm_cosq_tas_profile_status_t structure */
extern void bcm_cosq_tas_profile_status_t_init(
    bcm_cosq_tas_profile_status_t *profile_status);

#ifndef BCM_HIDE_DISPATCHABLE

/* Get the TAS profile status */
extern int bcm_cosq_tas_profile_status_get(
    int unit, 
    bcm_gport_t port, 
    bcm_cosq_tas_profile_id_t pid, 
    bcm_cosq_tas_profile_status_t *status);

#endif /* BCM_HIDE_DISPATCHABLE */

/* flags for sysport to ingress queue mapping set/get */
#define BCM_COSQ_SYSPORT_INGRESS_QUEUE_UNMAP 0x00000001 /* unmap flag */
#define BCM_COSQ_SYSPORT_INGRESS_QUEUE_CORE_FROM_GPORT 0x00000002 /* take local core from
                                                          ingress queue gport.
                                                          supported only for get
                                                          function */

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * bcm_cosq_sysport_ingress_queue_map_get and
 * bcm_cosq_sysport_ingress_queue_map_set
 */
extern int bcm_cosq_sysport_ingress_queue_map_set(
    int unit, 
    uint32 flags, 
    bcm_gport_t sysport, 
    bcm_gport_t ingress_queue);

/* 
 * bcm_cosq_sysport_ingress_queue_map_get and
 * bcm_cosq_sysport_ingress_queue_map_set
 */
extern int bcm_cosq_sysport_ingress_queue_map_get(
    int unit, 
    uint32 flags, 
    bcm_gport_t sysport, 
    bcm_gport_t *ingress_queue);

#endif /* BCM_HIDE_DISPATCHABLE */

/* strucutre holds the key parameters for the object mapping API. */
typedef struct bcm_cosq_obj_map_key_s {
    bcm_gport_t gport; 
} bcm_cosq_obj_map_key_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* API for mapping gport (queue) into object_stat_id. */
extern int bcm_cosq_stat_obj_map_set(
    int unit, 
    int flags, 
    bcm_cosq_obj_map_key_t *key, 
    uint32 object_stat_id);

/* Get the object_stat_id mapping for a given gport queue */
extern int bcm_cosq_stat_obj_map_get(
    int unit, 
    int flags, 
    bcm_cosq_obj_map_key_t *key, 
    uint32 *object_stat_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Strucutre hold the key parameters for cosq statistics configuration
 * API.
 */
typedef struct bcm_cosq_stat_info_key_s {
    int command_id; 
} bcm_cosq_stat_info_key_t;

/* 
 * Strucutre gather the cosq parameters in order to generate the counter
 * pointer toward the counter processor.
 */
typedef struct bcm_cosq_stat_info_s {
    int enable_mapping;         /* Enable/disable using the object stat id
                                   mapping table when generate the counter
                                   pointer */
    int num_queues_per_entry;   /* Defines how many consecutive VOQs are counted
                                   in one counter set. */
    uint32 queue_first;         /* Start range of VOQs which will be counted. */
    uint32 queue_last;          /* End range of VOQs which will be counted. */
} bcm_cosq_stat_info_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * API configure cosq statistics parameters per command_id, in order to
 * generate the counter pointer toward the counter processor.
 */
extern int bcm_cosq_stat_info_set(
    int unit, 
    int flags, 
    bcm_cosq_stat_info_key_t *key, 
    bcm_cosq_stat_info_t *config);

/* Get cosq statistics parameters for given command_id */
extern int bcm_cosq_stat_info_get(
    int unit, 
    int flags, 
    bcm_cosq_stat_info_key_t *key, 
    bcm_cosq_stat_info_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

typedef struct bcm_cosq_static_threshold_s {
    int thresh_set;     /* On threshold. */
    int thresh_clear;   /* Off threshold -- if not hyst, clear = set. */
} bcm_cosq_static_threshold_t;

typedef enum bcm_cosq_ingress_forward_priority_e {
    bcmCosqIngrFwdPriorityNone = -1, 
    bcmCosqIngrFwdPriorityLow = 0, 
    bcmCosqIngrFwdPriorityHigh = 1 
} bcm_cosq_ingress_forward_priority_t;

typedef enum bcm_cosq_forward_decision_type_e {
    bcmCosqFwdTypeFwd = 0, 
    bcmCosqFwdTypeSnoop = 1, 
    bcmCosqFwdTypeMirror = 2, 
    bcmCosqFwdTypeStatSample = 3 
} bcm_cosq_forward_decision_type_t;

typedef uint32 bcm_cosq_threshold_index_t;

/* Threshold Action type */
typedef enum bcm_cosq_threshold_action_type_e {
    bcmCosqThreshActionDrop = 0,        /* Drop threshold */
    bcmCosqThreshActionFc = 1,          /* FC threshold */
    bcmCosqThreshActionMarkCongestion = 2 /* ECN threshold */
} bcm_cosq_threshold_action_type_t;

/* Complementary information for static threshold. */
typedef struct bcm_cosq_static_threshold_info_s {
    bcm_gport_t gport;                  /* Target gport. */
    bcm_cosq_threshold_index_t index; 
    bcm_cosq_threshold_action_type_t threshold_action; 
    bcm_cosq_resource_t resource_type;  /* Resource type. */
    int is_free_resource;               /* If true -  test free resources,
                                           otherwise - test occupied resources. */
} bcm_cosq_static_threshold_info_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set static threshold parameters. */
extern int bcm_cosq_gport_static_threshold_set(
    int unit, 
    uint32 flags, 
    bcm_cosq_static_threshold_info_t *thresh_info, 
    bcm_cosq_static_threshold_t *threshold);

/* get static threshold parameters. */
extern int bcm_cosq_gport_static_threshold_get(
    int unit, 
    uint32 flags, 
    bcm_cosq_static_threshold_info_t *thresh_info, 
    bcm_cosq_static_threshold_t *threshold);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Index creation macros. */
#define BCM_COSQ_TH_INDEX_DP_SET(dp)        _SHR_COSQ_TH_INDEX_DP_SET(dp) 
#define BCM_COSQ_TH_INDEX_TC_SET(tc)        _SHR_COSQ_TH_INDEX_TC_SET(tc) 
#define BCM_COSQ_TH_INDEX_POOL_DP_SET(pool_id, dp)  _SHR_COSQ_TH_INDEX_POOL_DP_SET(pool_id, dp) 
#define BCM_COSQ_TH_INDEX_CAST_PRIO_FWDACT_SET(cast, priority, forward_action)  _SHR_COSQ_TH_INDEX_CAST_PRIO_FWDACT_SET(cast, priority, forward_action) 
#define BCM_COSQ_TH_INDEX_PRIO_SET(prio)    _SHR_COSQ_TH_INDEX_PRIO_SET(prio) 
#define BCM_COSQ_TH_INDEX_POOL_PRIO_SET(prio, pool_id)  _SHR_COSQ_TH_INDEX_POOL_PRIO_SET(prio, pool_id) 
#define BCM_COSQ_TH_INDEX_INVALID           -1         /* Invalid index for the
                                                          thresh_info in
                                                          bcm_cosq_gport_static_threshold_set */

#ifndef BCM_HIDE_DISPATCHABLE

/* Get / set mapping of PFC priority to Priority Group for a profile. */
extern int bcm_cosq_priority_group_pfc_priority_mapping_profile_set(
    int unit, 
    int profile_id, 
    int array_count, 
    int *pg_array);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Per PFC-rx priority config. */
typedef struct bcm_cosq_pfc_class_map_config_s {
    uint8 pfc_enable;       /* Enable/disable PFC rx, 1: enable, 0: disable */
    uint8 pfc_optimized;    /* PFC optimized flag, 1: optimized, 0: otherwise */
    uint32 cos_list_bmp;    /* COS list bitmap */
} bcm_cosq_pfc_class_map_config_t;

typedef struct bcm_cosq_ingress_port_drop_mpls_special_label_config_s {
    uint32 label_value; /* Special label value (0-15) */
    uint32 priority;    /* Priority to take when the special label is
                           recognized. */
    uint32 is_tdm;      /* TDM indication to set when the special label is
                           recognized. */
} bcm_cosq_ingress_port_drop_mpls_special_label_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get / set mapping of PFC priority to Priority Group for a profile. */
extern int bcm_cosq_priority_group_pfc_priority_mapping_profile_get(
    int unit, 
    int profile_id, 
    int array_max, 
    int *pg_array, 
    int *array_count);

/* 
 * Get / set PFC class configurations (e.g. PFC priority to COSQ mapping)
 * for a profile.
 */
extern int bcm_cosq_pfc_class_config_profile_set(
    int unit, 
    int profile_id, 
    int count, 
    bcm_cosq_pfc_class_map_config_t *config_array);

/* 
 * This API allows the user to save MPLS special label properties. If an
 * MPLS label on a packet is recognized as one of these special labels,
 * priority and TDM indication will be taken from the special label
 * properties.
 */
extern int bcm_cosq_ingress_port_drop_mpls_special_label_set(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    uint32 label_index, 
    bcm_cosq_ingress_port_drop_mpls_special_label_config_t *label_config);

/* 
 * Get / set PFC class configurations (e.g. PFC priority to COSQ mapping)
 * for a profile.
 */
extern int bcm_cosq_pfc_class_config_profile_get(
    int unit, 
    int profile_id, 
    int max_count, 
    bcm_cosq_pfc_class_map_config_t *config_array, 
    int *count);

/* 
 * This API allows the user to save MPLS special label properties. If an
 * MPLS label on a packet is recognized as one of these special labels,
 * priority and TDM indication will be taken from the special label
 * properties.
 */
extern int bcm_cosq_ingress_port_drop_mpls_special_label_get(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    uint32 label_index, 
    bcm_cosq_ingress_port_drop_mpls_special_label_config_t *label_config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a PFC class mapping configuration structure. */
extern void bcm_cosq_pfc_class_map_config_t_init(
    bcm_cosq_pfc_class_map_config_t *config);

extern void bcm_cosq_ingress_port_drop_mpls_special_label_config_t_init(
    bcm_cosq_ingress_port_drop_mpls_special_label_config_t *config);

/* MMU Profile Type */
typedef enum bcm_cosq_profile_type_e {
    bcmCosqProfilePFCAndQueueHierarchy = 0, /* Profile for setting Queue Scheduler
                                           Hierarchy and PFC
                                                           optimized classes */
    bcmCosqProfilePGProperties = 1,     /* Profile for Priority Group to Pool
                                           Mapping */
    bcmCosqProfileIntPriToPGMap = 2,    /* Profile for mapping Internal priority
                                           to PG for UC/MC
                                                           packets */
    bcmCosqProfileOobfcEgressQueueMap = 3, /* Profile for Enabling/Disabling MMU
                                           queue reporting and
                                                           mapping queue to bit
                                           positions in OOB message */
    bcmCosqProfileCount = 4             /* This should be the last one */
} bcm_cosq_profile_type_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/get Profile mapped to port for a given profile type */
extern int bcm_cosq_port_profile_set(
    int unit, 
    bcm_port_t port, 
    bcm_cosq_profile_type_t profile_type, 
    int profile_id);

/* Set/get Profile mapped to port for a given profile type */
extern int bcm_cosq_port_profile_get(
    int unit, 
    bcm_port_t port, 
    bcm_cosq_profile_type_t profile_type, 
    int *profile_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* MMU Dynamic Type to indicate modifiability during run time */
typedef enum bcm_cosq_dynamic_setting_type_e {
    bcmCosqDynamicTypeFixed = 0,        /* MMU settings that cannot be changed
                                           dynamically */
    bcmCosqDynamicTypeFlexible = 1,     /* MMU settings that can be changed
                                           dynamically with no restrictions */
    bcmCosqDynamicTypeConditional = 2   /* MMU settings that can be changed as
                                           long as the corresponding
                                                            egress port/ports
                                           dont have any traffic in MMU */
} bcm_cosq_dynamic_setting_type_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Check if a specific property's proile can be modified while there is
 * traffic in
 * ports mapping to the profile.
 */
extern int bcm_cosq_profile_property_dynamic_get(
    int unit, 
    bcm_cosq_profile_type_t profile_type, 
    bcm_cosq_dynamic_setting_type_t *dynamic);

/* Check if a specific property's proile is mapped to any port. */
extern int bcm_cosq_profile_info_inuse_get(
    int unit, 
    bcm_cosq_profile_type_t profile_type, 
    int profile_id, 
    int *in_use);

/* Get the list of ports mapped to a profile. */
extern int bcm_cosq_profile_info_portlist_get(
    int unit, 
    bcm_cosq_profile_type_t profile_type, 
    int profile_id, 
    int max_port_count, 
    bcm_port_t *port_list, 
    int *port_count);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Specify the type of mapping for profile */
typedef enum bcm_cosq_priority_group_mapping_profile_type_e {
    bcmCosqInputPriPriorityGroupUcMapping = 0, 
    bcmCosqInputPriPriorityGroupMcMapping = 1, 
    bcmCosqPriorityGroupServicePoolMapping = 2, 
    bcmCosqPriorityGroupHeadroomPoolMapping = 3 
} bcm_cosq_priority_group_mapping_profile_type_t;

/*  Microburst information */
typedef struct bcm_cosq_burst_output_s {
    uint64 sequence_num;        /* This is a running global number identifying
                                   each burst reported to the host */
    uint64 gport_sequence_num;  /* Total number of bursts observed on gport */
    uint64 traffic_start_time;  /* Traffic start time on gport */
    uint64 burst_start_time;    /* Start time of the burst on gport */
    uint64 burst_end_time;      /* End time of the burst on gport */
    uint64 burst_duration;      /* Burst duration on gport */
    uint64 drop_count;          /* Number of bytes dropped since pervious burst
                                   report on this gport */
    uint64 rx_count;            /* Number of bytes received since pervious burst
                                   report on this gport */
    bcm_gport_t gport_num;      /* User requested gport to be monitored */
    uint32 reserved;            /* User reserved for furture case */
} bcm_cosq_burst_output_t;

/* 
 * Enable to collect actual scan interval information from BTE burst
 * monitor application.
 */
#define BCM_BURST_MONITOR_SCAN_INTERVAL_COLLECTION (1 << 0)   /* Enable to collect
                                                          actual scan interval
                                                          information from BTE
                                                          burst monitor
                                                          application. */

/* Queue congestion monitor Interest Factor computation weights. */
typedef enum bcm_cosq_burst_monitor_weight_e {
    bcmCosqBurstMonitorWeightFlowMaxRxBytes = 0, /* (w1) Maximum of any flow Rx bytes
                                           weight */
    bcmCosqBurstMonitorWeightFlowSumRxBytes = 1, /* (w2) Sum of all flows Rx bytes weight */
    bcmCosqBurstMonitorWeightQueueMaxBufBytes = 2, /* (w3) Maximum usage of any queue
                                           buffer utilization weight */
    bcmCosqBurstMonitorWeightQueueSumBufBytes = 3, /* (w4) Sum of all queues buffer
                                           utilization weight */
    bcmCosqBurstMonitorWeightQueueMaxDropBytes = 4, /* (w5) Maximum of any queues drop bytes
                                           weight */
    bcmCosqBurstMonitorWeightQueueSumDropBytes = 5, /* (w6) Sum of all queues drop bytes
                                           weight */
    bcmCosqBurstMonitorWeightQueueMaxRxBytes = 6, /* (w7) Maximum of any queues Rx bytes
                                           weight */
    bcmCosqBurstMonitorWeightQueueSumRxBytes = 7, /* (w8) Sum of all queues Rx bytes
                                           weight */
    bcmCosqBurstMonitorWeightMaxTMUtil = 8, /* (w9) Maximum XPE Utilization weight */
    bcmCosqBurstMonitorWeightAvgTMUtil = 9, /* (w10) Average XPE utilization weight */
    bcmCosqBurstMonitorWeightRandomNum = 10, /* (w11) random integer weight */
    bcmCosqBurstMonitorWeightViewID = 11, /* (w12) Flow view id weight */
    bcmCosqBurstMonitorWeightMaxViewID = 12, /* (w13) Maximum value of flow view id */
    bcmCosqBurstMonitorWeightViewIDThreshold = 13, /* (w14) Threshold value of flow view id */
    bcmCosqBurstMonitorWeightFlowMaxRxPackets = 14, /* (w15) Maximum of any flow Rx packets
                                           weight */
    bcmCosqBurstMonitorWeightFlowSumRxPackets = 15, /* (w16) Sum of all flows Rx packets
                                           weight */
    bcmCosqBurstMonitorWeightQueueMaxDropPackets = 16, /* (w17) Maximum of any queues drop
                                           packets weight */
    bcmCosqBurstMonitorWeightQueueSumDropPackets = 17, /* (w18) Sum of all queues drop packets
                                           weight */
    bcmCosqBurstMonitorWeightQueueMaxRxPackets = 18, /* (w19) Maximum of any queues received
                                           packets weight */
    bcmCosqBurstMonitorWeightQueueSumRxPackets = 19, /* (w20) Sum of all queues received
                                           packets weight */
    bcmCosqBurstMonitorWeightQueueMaxTxBytes = 20, /* (w21) Maximum of any queues Tx bytes
                                           weight */
    bcmCosqBurstMonitorWeightQueueSumTxBytes = 21, /* (w22) Sum of all queues Tx bytes
                                           weight */
    bcmCosqBurstMonitorWeightQueueMaxTxPackets = 22, /* (w23) Maximum of any queues Tx
                                           packets weight */
    bcmCosqBurstMonitorWeightQueueSumTxPackets = 23, /* (w24) Sum of all queues Tx packets
                                           weight */
    bcmCosqBurstMonitorWeightCount = 24 
} bcm_cosq_burst_monitor_weight_t;

/* 
 * Queue congestion monitor export trigger info. Calculate InterestFactor
 * of each view and export to collector.
 */
typedef struct bcm_cosq_burst_monitor_export_trigger_s {
    uint64 export_threshold;            /* Export threshold */
    uint8 weights[bcmCosqBurstMonitorWeightCount]; /* InterestFactor computation weights. */
} bcm_cosq_burst_monitor_export_trigger_t;

/* Queue congestion monitor flow view Configuration. */
typedef struct bcm_cosq_burst_monitor_flow_view_info_s {
    bcm_cosq_burst_monitor_export_trigger_t export_param; /* Queue congestion monitor export
                                           trigger Info */
    uint64 start_timer_usecs;           /* Start time of Queue congestion
                                           monitor */
    uint64 age_timer_usecs;             /* Replace existing flow view in host
                                           memory based on age timer */
    uint32 scan_interval_usecs;         /* sample/scan Interval */
    uint32 age_threshold;               /* Aging persistent flows if sum of all
                                           sampled rx_byte_count values goes
                                           below threshold */
    uint32 num_samples;                 /* Number of samples captured per view */
    uint32 num_flows;                   /* Number of flows captured per view */
    uint32 num_flows_clear;             /* Clears configured number of flows
                                           with low Rx byte count from total
                                           number of flows, to allow new flows
                                           learning */
    uint32 flags;                       /* Options that can be passed via flags
                                           parameter during flow view
                                           configuration. */
} bcm_cosq_burst_monitor_flow_view_info_t;

/* 
 * Options that can be passed via options parameter during configuration
 * set.
 */
#define BCM_QCM_CONFIG_UPDATE   (1 << 0)   /* Add/Modify configuration to
                                              transfer Queue congestion monitor
                                              information. */
#define BCM_QCM_CONFIG_CLEAR    (1 << 1)   /* Clear configuration information. */

/* 
 * Options that can be passed via flags parameter during flow view stats
 * data get.
 */
#define BCM_QCM_FLOW_VIEW_STATS_DATA_SORT   (1 << 0)   /* Queue congestion
                                                          monitor flow view
                                                          stats information in
                                                          sorted order. */

#ifndef BCM_HIDE_DISPATCHABLE

/* Initialize Queue congestion monitoring */
extern int bcm_cosq_burst_monitor_init(
    int unit);

/* Shut Down Queue congestion monitoring */
extern int bcm_cosq_burst_monitor_detach(
    int unit);

/* Set Queue congestion monitoring flow view configuration. */
extern int bcm_cosq_burst_monitor_flow_view_config_set(
    int unit, 
    uint32 options, 
    bcm_cosq_burst_monitor_flow_view_info_t *flow_view_data);

/* Get Queue congestion monitoring flow view configuration. */
extern int bcm_cosq_burst_monitor_flow_view_config_get(
    int unit, 
    bcm_cosq_burst_monitor_flow_view_info_t *flow_view_data);

/* Queue congestion monitoring clear flow view data. */
extern int bcm_cosq_burst_monitor_flow_view_data_clear(
    int unit);

/* 
 * Customer app provides the buffer address and size to the QCM App. QCM
 * App writes the processed flow view information into this buffer.
 */
extern int bcm_cosq_burst_monitor_flow_view_data_get(
    int unit, 
    uint32 usr_mem_size, 
    uint8 *usr_mem_addr);

/* 
 * Customer app provides the buffer address and size to the QCM App. This
 * is a one time config and no dynamic change in the address or the size
 * is permitted. QCM App writes the processed information into this
 * buffer in a circular fashion and expect customer application to read
 * periodically.
 */
extern int bcm_cosq_burst_monitor_dma_config_set(
    int unit, 
    uint32 host_mem_size, 
    uint32 **host_mem_addr);

/* 
 * Customer provided array of gports that are to be monitored by the QCM
 * App.
 */
extern int bcm_cosq_burst_monitor_set(
    int unit, 
    int num_gports, 
    bcm_gport_t *gport_list);

/* Get array gports that are being monitored by the QCM App. */
extern int bcm_cosq_burst_monitor_get(
    int unit, 
    int max_gports, 
    bcm_gport_t *gport_list, 
    int *num_gports);

/* 
 * Gets the host memory size and address of micro burst data provided by
 * the QCM App
 */
extern int bcm_cosq_burst_monitor_dma_config_get(
    int unit, 
    uint32 *host_mem_size, 
    uint32 **host_mem_addr);

/* Get current QCM FW time. */
extern int bcm_cosq_burst_monitor_get_current_time(
    int unit, 
    uint64 *time_usecs);

/* Get flow views summary data. */
extern int bcm_cosq_burst_monitor_view_summary_get(
    int unit, 
    uint32 mem_size, 
    uint8 *mem_addr, 
    int max_num_views, 
    int *num_views);

/* Get flow view stats data */
extern int bcm_cosq_burst_monitor_flow_view_stats_get(
    int unit, 
    uint32 mem_size, 
    uint8 *mem_addr, 
    int max_num_flows, 
    int view_id, 
    uint32 flags, 
    int *num_flows);

/* Get/Set a profile for the various mappings */
extern int bcm_cosq_priority_group_mapping_profile_set(
    int unit, 
    int profile_index, 
    bcm_cosq_priority_group_mapping_profile_type_t type, 
    int array_count, 
    int *arg);

/* Get/Set a profile for the various mappings */
extern int bcm_cosq_priority_group_mapping_profile_get(
    int unit, 
    int profile_index, 
    bcm_cosq_priority_group_mapping_profile_type_t type, 
    int array_max, 
    int *arg, 
    int *array_count);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Specify the type of service pool override */
typedef enum bcm_cosq_service_pool_override_type_e {
    bcmCosqServicePoolOverrideCpu = 0, 
    bcmCosqServicePoolOverrideMcPkt = 1, 
    bcmCosqServicePoolOverrideMirror = 2 
} bcm_cosq_service_pool_override_type_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Enable or disable service pool override */
extern int bcm_cosq_service_pool_override_set(
    int unit, 
    bcm_cosq_service_pool_override_type_t type, 
    bcm_service_pool_id_t service_pool, 
    int enable);

/* Enable or disable service pool override */
extern int bcm_cosq_service_pool_override_get(
    int unit, 
    bcm_cosq_service_pool_override_type_t type, 
    bcm_service_pool_id_t *service_pool, 
    int *enable);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Specify per port per Priority Group property */
typedef enum bcm_cosq_port_prigroup_control_e {
    bcmCosqPriorityGroupLossless = 0, 
    bcmCosqPauseEnable = 1 
} bcm_cosq_port_prigroup_control_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Setup per port per priority group properties */
extern int bcm_cosq_port_priority_group_property_set(
    int unit, 
    bcm_port_t port, 
    int priority_group_id, 
    bcm_cosq_port_prigroup_control_t type, 
    int arg);

/* Setup per port per priority group properties */
extern int bcm_cosq_port_priority_group_property_get(
    int unit, 
    bcm_port_t port, 
    int priority_group_id, 
    bcm_cosq_port_prigroup_control_t type, 
    int *arg);

/* Check if a cosq_control_type property can be changed dynamically */
extern int bcm_cosq_control_dynamic_get(
    int unit, 
    bcm_cosq_control_t type, 
    bcm_cosq_dynamic_setting_type_t *dynamic);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Cosq to queue mapping for scheduler */
typedef struct bcm_cosq_mapping_s {
    bcm_cos_queue_t cosq;   /* Cos value */
    int num_ucq;            /* Number of unicast queues. Valid values are 0-2 */
    int num_mcq;            /* Number of multicast queues. Valid values are 0-1 */
    int strict_priority;    /* Schedudling priority is strict. 0: Not strict, 1:
                               strict */
    int fc_is_uc_only;      /* Flow control is set for uc only packets */
} bcm_cosq_mapping_t;

/* Initializes the bcm_cosq_mapping_t structure */
extern void bcm_cosq_mapping_t_init(
    bcm_cosq_mapping_t *control);

#ifndef BCM_HIDE_DISPATCHABLE

/* Setup cosq to mmu queue mapping profiles */
extern int bcm_cosq_schedq_mapping_set(
    int unit, 
    int profile_index, 
    int size, 
    bcm_cosq_mapping_t *cosq_mapping_arr);

/* Setup cosq to mmu queue mapping profiles */
extern int bcm_cosq_schedq_mapping_get(
    int unit, 
    int profile_index, 
    int array_max, 
    bcm_cosq_mapping_t *cosq_mapping_arr, 
    int *size);

#endif /* BCM_HIDE_DISPATCHABLE */

/* OBM traffic classes */
typedef enum bcm_obm_traffic_class_e {
    bcmObmClassLossyLow = 0, 
    bcmObmClassLossyHigh = 1, 
    bcmObmClassLossless0 = 2, 
    bcmObmClassLossless1 = 3 
} bcm_obm_traffic_class_t;

/* OBM lookup priority type */
typedef enum bcm_obm_lookup_priority_type_e {
    bcmObmPriorityTypeVLAN = 0, 
    bcmObmPriorityTypeDSCP = 1, 
    bcmObmPriorityTypeUntagged = 2 
} bcm_obm_lookup_priority_type_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/get OBM traffic class mapped to different prioity for a port. */
extern int bcm_obm_port_pri_traffic_class_mapping_set(
    int unit, 
    bcm_port_t port, 
    bcm_obm_lookup_priority_type_t piority_type, 
    int priority, 
    bcm_obm_traffic_class_t obm_tc);

/* Set/get OBM traffic class mapped to different prioity for a port. */
extern int bcm_obm_port_pri_traffic_class_mapping_get(
    int unit, 
    bcm_port_t port, 
    bcm_obm_lookup_priority_type_t piority_type, 
    int priority, 
    bcm_obm_traffic_class_t *obm_tc);

#endif /* BCM_HIDE_DISPATCHABLE */

/* OBM traffic class for max watermark mode selection */
typedef enum bcm_obm_max_watermark_mode_e {
    bcmObmMaxUsageModeAllTraffic = 0, 
    bcmObmMaxUsageModeLossy = 1, 
    bcmObmMaxUsageModeLossless0 = 2, 
    bcmObmMaxUsageModeLossless1 = 3 
} bcm_obm_max_watermark_mode_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/get max watermark mode for a given port. */
extern int bcm_obm_port_max_usage_mode_get(
    int unit, 
    bcm_port_t port, 
    bcm_obm_max_watermark_mode_t *obm_wm_mode);

/* Set/get max watermark mode for a given port. */
extern int bcm_obm_port_max_usage_mode_set(
    int unit, 
    bcm_port_t port, 
    bcm_obm_max_watermark_mode_t obm_wm_mode);

/* Set/get PFC priorities mapped to OBM traffic class for a port. */
extern int bcm_obm_traffic_class_pfc_priority_mapping_set(
    int unit, 
    bcm_port_t port, 
    bcm_obm_traffic_class_t obm_traffic_class, 
    int max_pri_count, 
    int *priority_list);

/* Set/get PFC priorities mapped to OBM traffic class for a port. */
extern int bcm_obm_traffic_class_pfc_priority_mapping_get(
    int unit, 
    bcm_port_t port, 
    bcm_obm_traffic_class_t obm_traffic_class, 
    int max_pri_count, 
    int *priority_list, 
    int *pri_count);

#endif /* BCM_HIDE_DISPATCHABLE */

/* PFC deadlock timer interval selection. */
typedef enum bcm_cosq_pfc_deadlock_timer_interval_e {
    bcmCosqPFCDeadlockTimerInterval1MiliSecond = 0, 
    bcmCosqPFCDeadlockTimerInterval10MiliSecond = 1, 
    bcmCosqPFCDeadlockTimerInterval100MiliSecond = 2, 
    bcmCosqPFCDeadlockTimerIntervalCount = 3 
} bcm_cosq_pfc_deadlock_timer_interval_t;

/* PFC deadlock control type selection. */
typedef enum bcm_cosq_pfc_deadlock_control_e {
    bcmCosqPFCDeadlockDetectionAndRecoveryEnable = 0, 
    bcmCosqPFCDeadlockRecoveryOccurrences = 1, 
    bcmCosqPFCDeadlockDetectionTimer = 2, 
    bcmCosqPFCDeadlockRecoveryTimer = 3, 
    bcmCosqPFCDeadlockTimerGranularity = 4, 
    bcmCosqPFCDeadlockControlTypeCount = 5 
} bcm_cosq_pfc_deadlock_control_t;

/* Bandwidth fabric adjust key info */
typedef struct bcm_cosq_bandwidth_fabric_adjust_key_s {
    bcm_core_t core;    /* core id */
    int num_links;      /* Number of links/serdes. */
    int rci_level;      /* Route congestion indication. */
} bcm_cosq_bandwidth_fabric_adjust_key_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Set/retrieve bandwidth fabric adjust entry (DRM table), Each link has
 * entries corresponding to all the rci levels.
 */
extern int bcm_cosq_bandwidth_fabric_adjust_set(
    int unit, 
    uint32 flags, 
    bcm_cosq_bandwidth_fabric_adjust_key_t *key, 
    uint32 bandwidth);

/* 
 * Get bandwidth fabric adjust entry (DRM table), Each link has entries
 * corresponding to all the rci levels.
 */
extern int bcm_cosq_bandwidth_fabric_adjust_get(
    int unit, 
    uint32 flags, 
    bcm_cosq_bandwidth_fabric_adjust_key_t *key, 
    uint32 *bandwidth);

/* 
 * Set / Get config for the given (PFC priority, port) for the PFC
 * Deadlock feature
 */
extern int bcm_cosq_pfc_deadlock_control_set(
    int unit, 
    bcm_port_t port, 
    int pfc_priority, 
    bcm_cosq_pfc_deadlock_control_t type, 
    int arg);

/* 
 * Set / Get config for the given (PFC priority, port) for the PFC
 * Deadlock feature
 */
extern int bcm_cosq_pfc_deadlock_control_get(
    int unit, 
    bcm_port_t port, 
    int pfc_priority, 
    bcm_cosq_pfc_deadlock_control_t type, 
    int *arg);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Obm interrupt types */
typedef enum bcm_obm_interrupt_type_e {
    bcmObmInterruptTypeExpressOverflow = 0, /* Enable obm  interrupt for express
                                           traffic overflow . */
    bcmObmInterruptTypeExpressDrop = 1, /* Enable obm  interrupt for express
                                           traffic drop . */
    bcmObmInterruptTypePreemptOverflow = 2, /* Enable obm  interrupt for preempt 
                                           traffic overflow. */
    bcmObmInterruptTypePreemptDrop = 3  /* Enable obm  interrupt for preempt
                                           traffic drop. */
} bcm_obm_interrupt_type_t;

#define BCM_COSQ_PFC_DEADLOCK_WIRED_COS     0x10000000 /* Map internal priority
                                                          config to wired cos */
#define BCM_COSQ_PFC_DEADLOCK_WIRELESS_COS  0x20000000 /* Map internal priority
                                                          config to wireless cos */
#define BCM_COSQ_PFC_DEADLOCK_TRANSIT_COS   0x40000000 /* Map internal priority
                                                          config to transit cos */

/* Cosq gport info */
typedef struct bcm_cosq_gport_level_info_s {
    int parent_port_type;   /* Indicates whether the given cosq gport is a
                               member of the scheduling hierarchy belonging to a
                               uplink or downlink port. */
    int level;              /* Cosq gport's scheduler level L0/L1/L2. */
} bcm_cosq_gport_level_info_t;

/* Initialize a COSq gport info structure. */
extern void bcm_cosq_gport_level_info_t_init(
    bcm_cosq_gport_level_info_t *info);

#ifndef BCM_HIDE_DISPATCHABLE

/* Retrieve cosq gport info. */
extern int bcm_cosq_gport_info_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_cosq_gport_level_info_t *info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Defines uplink or downlink, stacking port type. It indicates whether
 * the given cosq gport is a member of the scheduling hierarchy belonging
 * to a uplink, stacking or downlink port
 */
#define BCM_COSQ_DOWNLINK_GPORT 0x0        
#define BCM_COSQ_UPLINK_GPORT   0x1        
#define BCM_COSQ_STACKING_GPORT 0x2        

/* Obm interrupt info passed to the customer callback */
typedef struct bcm_obm_interrupt_info_s {
    int port;                           /* Port on which the obm interrupt has
                                           occured */
    bcm_obm_interrupt_type_t interrupt_type; /* Interrupt type. */
} bcm_obm_interrupt_info_t;

typedef void (*bcm_obm_callback_fn)(
    int unit, 
    bcm_obm_interrupt_info_t *obm_info, 
    void *userdata);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * This is used by the application to register/unregister the callback
 * routine that needs to be called
 *  when we encounter the OBM drop/overflow interrupts.
 */
extern int bcm_obm_cb_unregister(
    int unit, 
    bcm_obm_callback_fn fn, 
    void *user_data);

/* Retrieve the number of PFC optimized groups supported for a port. */
extern int bcm_cosq_port_optimized_pfc_group_num_get(
    int unit, 
    bcm_port_t port, 
    int *arg);

/* 
 * This is used by the application to register/unregister the callback
 * routine that needs to be called
 *  when we encounter the OBM drop/overflow interrupts.
 */
extern int bcm_obm_cb_register(
    int unit, 
    bcm_obm_callback_fn fn, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Scheduler element type */
typedef enum bcm_cosq_scheduler_gport_type_e {
    bcmCosqSchedulerGportTypeClass4Sp = 0, /* CL mode 1 -- 4 strict priorities */
    bcmCosqSchedulerGportTypeClass3SpWfq = 1, /* CL mode 2 -- 3 strict priorities,
                                           last WFQ(2) */
    bcmCosqSchedulerGportTypeClassWfq2Sp = 2, /* CL mode 3 -- 2 strict priorities,
                                           first WFQ(3) or WFQ(1:63) */
    bcmCosqSchedulerGportTypeClass2SpWfq = 3, /* CL mode 4 -- 2 strict priorities,
                                           last WFQ(3) or WFQ(1:63) */
    bcmCosqSchedulerGportTypeClass1SpWfq = 4, /* CL mode 5 -- 1 strict priority WFQ(4)
                                           or WFQ(1:253) */
    bcmCosqSchedulerGportTypeHrSingleWfq = 5, /* HR with single WFQ */
    bcmCosqSchedulerGportTypeHrDualWfq = 6, /* HR with Dual WFQ */
    bcmCosqSchedulerGportTypeHrEnhanced = 7, /* Enhanced HR */
    bcmCosqSchedulerGportTypeFq = 8     /* FQ */
} bcm_cosq_scheduler_gport_type_t;

/* Scheduler element mode */
typedef enum bcm_cosq_scheduler_gport_mode_e {
    bcmCosqSchedulerGportModeSimple = 0, /* Simple element */
    bcmCosqSchedulerGportModeComposite = 1, /* Composite element */
    bcmCosqSchedulerGportModeEnhancedClLowPrioFQ = 2, /* Enhanced CL with FQ at low priority */
    bcmCosqSchedulerGportModeEnhancedClHighPrioFQ = 3, /* Enhanced CL with FQ at high priority */
    bcmCosqSchedulerGportModeSharedDual = 4, /* Shared shaper with 2 elements */
    bcmCosqSchedulerGportModeSharedQuad = 5, /* Shared shaper with 4 elements */
    bcmCosqSchedulerGportModeSharedOcta = 6 /* Shared shaper with 8 elements */
} bcm_cosq_scheduler_gport_mode_t;

/* CL WFQ mode */
typedef enum bcm_cosq_scheduler_gport_cl_wfq_mode_e {
    bcmCosqSchedulerGportClWfqModeNone = 0, /* Not relevant */
    bcmCosqSchedulerGportClWfqModeDiscrete = 1, /* Discrete per flow */
    bcmCosqSchedulerGportClWfqModeClass = 2, /* Discrete per class */
    bcmCosqSchedulerGportClWfqModeIndependent = 3, /* Independent mode */
    bcmCosqSchedulerGportClWfqModeIndependentProportional = 4 /* Independent proportional mode */
} bcm_cosq_scheduler_gport_cl_wfq_mode_t;

#define BCM_COSQ_SCHEDULER_GPORT_WITH_ID    0x00000001 /* Allocate with id */
#define BCM_COSQ_SCHEDULER_GPORT_REPLACE    0x00000002 /* Replace existing
                                                          element */
#define BCM_COSQ_SCHEDULER_GPORT_FMQ_RESERVE 0x00000004 /* Reserve HR to be used
                                                          for FMQ */

/* Scheduling element parameters */
typedef struct bcm_cosq_scheduler_gport_params_s {
    bcm_core_t core;                    /* core id */
    bcm_cosq_scheduler_gport_type_t type; /* type of scheduling element */
    bcm_cosq_scheduler_gport_mode_t mode; /* Mode of scheduling element */
    bcm_cosq_scheduler_gport_cl_wfq_mode_t cl_wfq_mode; /* CL WFQ mode. */
} bcm_cosq_scheduler_gport_params_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Create scheduling element */
extern int bcm_cosq_scheduler_gport_add(
    int unit, 
    uint32 flags, 
    bcm_cosq_scheduler_gport_params_t *params, 
    bcm_gport_t *gport);

/* Set PFC deadlock related configurations. */
extern int bcm_cosq_scheduler_gport_get(
    int unit, 
    uint32 flags, 
    bcm_gport_t gport, 
    bcm_cosq_scheduler_gport_params_t *params);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_COSQ_LATENCY_END_TO_END         0x1        /* refer to end to end
                                                          latency */
#define BCM_COSQ_LATENCY_PROFILE_WITH_ID    0x2        /* create latency profile
                                                          with id */
#define BCM_COSQ_LATENCY_PROFILE_ID_EXTENSION 0x4        /* allow to create
                                                          latency profile belong
                                                          to the extension
                                                          profiles range */
#define BCM_COSQ_LATENCY_PROFILE_BY_FLOW    0x8        /* create latency flow
                                                          profile */
#define BCM_COSQ_LATENCY_END_TO_END_AQM     0x10       /* create end to end AQM
                                                          profile */

#ifndef BCM_HIDE_DISPATCHABLE

/* API create new latency profile. */
extern int bcm_cosq_latency_profile_create(
    int unit, 
    int flags, 
    bcm_gport_t *profile_gport);

/* API destroy new latency profile */
extern int bcm_cosq_latency_profile_destroy(
    int unit, 
    int flags, 
    bcm_gport_t profile_gport);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Cosq packet size and distribution percentage */
typedef struct bcm_cosq_pkt_size_dist_s {
    int pkt_size;   /* Packet size */
    int dist_perc;  /* Percentage distribution of packet size */
} bcm_cosq_pkt_size_dist_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Setup Port PriorityGroup headroom value based on cable length and
 * packet distribution.
 */
extern int bcm_cosq_port_prigrp_hdrm_set(
    int unit, 
    bcm_port_t port, 
    uint32 pri_bmp, 
    int cable_len, 
    int arr_size, 
    bcm_cosq_pkt_size_dist_t *pkt_dist_array);

/* 
 * Calculate and set Headroom Pool limit based on number of lossless
 * classes and
 * packet distribution.
 */
extern int bcm_cosq_hdrm_pool_limit_set(
    int unit, 
    int hdrm_pool, 
    int num_lossless_class, 
    int arr_size, 
    bcm_cosq_pkt_size_dist_t *pkt_dist_array);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_COSQ_GENERIC_KEY_COLOR_SET(key, color)  _SHR_COSQ_GENERIC_KEY_COLOR_SET(key,color) 
#define BCM_COSQ_GENERIC_KEY_COLOR_GET(key)  _SHR_COSQ_GENERIC_KEY_COLOR_GET(key) 
#define BCM_COSQ_GENERIC_KEY_LAT_BIN_COLOR_SET(key, bin, color)  _SHR_COSQ_GENERIC_KEY_LAT_BIN_COLOR_SET(key, bin, color) 
#define BCM_COSQ_GENERIC_KEY_LAT_BIN_COLOR_BIN_GET(key)  _SHR_COSQ_GENERIC_KEY_LAT_BIN_COLOR_BIN_GET(key) 
#define BCM_COSQ_GENERIC_KEY_LAT_BIN_COLOR_COLOR_GET(key)  _SHR_COSQ_GENERIC_KEY_LAT_BIN_COLOR_COLOR_GET(key) 

/* For Generic controls. */
typedef enum bcm_cosq_generic_control_e {
    bcmCosqGenericControlLatencyVoqRejectEnable = 0, /* Generic control latency. */
    bcmCosqGenericControlLatencyVoqRejectProb = 1, /* Generic control latency reject. */
    bcmCosqGenericControlLatencyVoqRejectMinThreshold = 2 /* Generic control latency reject
                                           threshold. */
} bcm_cosq_generic_control_t;

/* mods for bcm_cosq_control_set API, bcmCosqControlLatencyTrack type. */
typedef enum bcm_cosq_control_track_mode_e {
    bcmCosqControlLatencyTrackModInvalid = -1, 
    bcmCosqControlLatencyTrackModFirst = 0, 
    bcmCosqControlLatencyTrackFlowId = bcmCosqControlLatencyTrackModFirst, 
    bcmCosqControlLatencyTrackOutPort = 1, 
    bcmCosqControlLatencyTrackOutPoreTc = 2, 
    bcmCosqControlLatencyTrackOutPortTcMc = 3, 
    bcmCosqControlLatencyTrackModSize = 4 
} bcm_cosq_control_track_mode_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Configuring/retrieving Ingress / egress Generic Control */
extern int bcm_cosq_generic_control_set(
    int unit, 
    uint32 flags, 
    bcm_gport_t gport, 
    uint32 key, 
    bcm_cosq_generic_control_t type, 
    uint32 arg);

/* Configuring/retrieving Ingress / egress Generic Control */
extern int bcm_cosq_generic_control_get(
    int unit, 
    uint32 flags, 
    bcm_gport_t gport, 
    uint32 key, 
    bcm_cosq_generic_control_t type, 
    uint32 *arg);

#endif /* BCM_HIDE_DISPATCHABLE */

/* mirror on drop types */
typedef enum bcm_cosq_mirror_on_drop_type_e {
    bcmCosqMirrorOnDropTypeGlobal = 0,  /* global type */
    bcmCosqMirrorOnDropTypeVoq = 1,     /* voq type */
    bcmCosqMirrorOnDropTypePort = 2,    /* port type */
    bcmCosqMirrorOnDropTypePG = 3       /* pg type */
} bcm_cosq_mirror_on_drop_type_t;

/* Drop group info */
typedef struct bcm_cosq_mirror_on_drop_group_info_s {
    bcm_cosq_drop_reason_t reasons_array[bcmCosqDropReasonRejectCount]; /* drop reasons array */
    int drop_reasons_count;             /* actual amount of drop reasons */
    bcm_cosq_mirror_on_drop_type_t type; /* type of group */
} bcm_cosq_mirror_on_drop_group_info_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/get mirror on drop group info. */
extern int bcm_cosq_mirror_on_drop_group_set(
    int unit, 
    uint32 flags, 
    int group_id, 
    bcm_cosq_mirror_on_drop_group_info_t *group_info);

/* Set/get mirror on drop group info. */
extern int bcm_cosq_mirror_on_drop_group_get(
    int unit, 
    uint32 flags, 
    int group_id, 
    bcm_cosq_mirror_on_drop_group_info_t *group_info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Weights for calculating Egress Rate. */
#define BCM_COSQ_RATE_WEIGHT_FULL       0          /* rate is composed fully of
                                                      measurement of the current
                                                      interval. */
#define BCM_COSQ_RATE_WEIGHT_HALF       1          /* rate is composed 1/2 of
                                                      current interval
                                                      measurement and 1/2 of
                                                      previously measured rate. */
#define BCM_COSQ_RATE_WEIGHT_QUARTER    2          /* rate is composed 3/4 of
                                                      current interval
                                                      measurement and 3/4 of
                                                      previously measured rate. */
#define BCM_COSQ_RATE_WEIGHT_EIGHTH     3          /* rate is composed 1/8 of
                                                      current interval
                                                      measurement and 7/8 of
                                                      previously measured rate. */

/* Queue offset mapping key */
typedef struct bcm_cosq_queue_offset_mapping_key_s {
    int tc; /* incoming TC */
    int dp; /* packet DP */
} bcm_cosq_queue_offset_mapping_key_t;

/* Queue offset */
typedef struct bcm_cosq_queue_offset_s {
    bcm_cos_queue_t cosq;   /* queue offset */
} bcm_cosq_queue_offset_t;

extern void bcm_cosq_queue_offset_mapping_key_t_init(
    bcm_cosq_queue_offset_mapping_key_t *key);

#ifndef BCM_HIDE_DISPATCHABLE

/* Map gport to queue offset for specified keys */
extern int bcm_cosq_gport_queue_offset_mapping_set(
    int unit, 
    uint32 flags, 
    bcm_gport_t gport, 
    int count, 
    bcm_cosq_queue_offset_mapping_key_t *key_array, 
    bcm_cosq_queue_offset_t *offset_array);

/* Get gport to queue offset mapping for specified keys */
extern int bcm_cosq_gport_queue_offset_mapping_get(
    int unit, 
    uint32 flags, 
    bcm_gport_t gport, 
    int count, 
    bcm_cosq_queue_offset_mapping_key_t *key_array, 
    bcm_cosq_queue_offset_t *offset_array);

/* Set/Get EBST enable status. */
extern int bcm_cosq_ebst_enable_set(
    int unit, 
    int enable);

/* Set/Get EBST enable status. */
extern int bcm_cosq_ebst_enable_get(
    int unit, 
    int *enable);

#endif /* BCM_HIDE_DISPATCHABLE */

/* EBST scan mode type eumerations. */
typedef enum bcm_cosq_ebst_scan_mode_e {
    bcmCosqEbstScanModeQueue = 0,       /* Queue mode */
    bcmCosqEbstScanModeUCPortPool = 1,  /* Unicast port service pool mode */
    bcmCosqEbstScanModeMCPortPool = 2,  /* Multicast port service pool mode */
    bcmCosqEbstScanModeCount = 3        /* Must be the last. Not a usable value */
} bcm_cosq_ebst_scan_mode_t;

/* EBST scan mode type eumerations. */
typedef enum bcm_cosq_ebst_control_e {
    bcmCosqEbstControlScanEnable = 0,   /* 1 : Start EBST scan. 0 : Stop EBST
                                           scan */
    bcmCosqEbstControlScanMode = 1,     /* EBST scan mode,
                                           bcm_cosq_ebst_scan_mode_t */
    bcmCosqEbstControlScanInterval = 2, /* Value in time interval (unit 1us)
                                           defining the frequency of an EBST
                                           scan round */
    bcmCosqEbstControlScanThreshold = 3, /* Scan threshold. The scan process will
                                           start if the number of available
                                           entries in the EBST FIFO is greater
                                           than this preconfigured scan
                                           threshold. */
    bcmCosqEbstControlCount = 4         /* Must be the last. Not a usable value */
} bcm_cosq_ebst_control_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/Get EBST scan interval, scan mode and enable mode. */
extern int bcm_cosq_ebst_control_set(
    int unit, 
    bcm_cosq_ebst_control_t type, 
    int arg);

/* Set/Get EBST scan interval, scan mode and enable mode. */
extern int bcm_cosq_ebst_control_get(
    int unit, 
    bcm_cosq_ebst_control_t type, 
    int *arg);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Start and stop thresholds of an event BST (EBST) profile */
typedef struct bcm_cosq_ebst_threshold_profile_s {
    uint32 start_bytes; /* Start threshold value in bytes */
    uint32 stop_bytes;  /* Stop threshold value in bytes */
} bcm_cosq_ebst_threshold_profile_t;

/* Initialize a EBST threshold profile structure. */
extern void bcm_cosq_ebst_threshold_profile_t_init(
    bcm_cosq_ebst_threshold_profile_t *profile);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/Get a EBST threshold profile for a given BST stat Id. */
extern int bcm_cosq_ebst_threshold_profile_set(
    int unit, 
    bcm_cosq_object_id_t *object_id, 
    bcm_bst_stat_id_t bid, 
    bcm_cosq_ebst_threshold_profile_t *profile);

/* Set/Get a EBST threshold profile for a given BST stat Id. */
extern int bcm_cosq_ebst_threshold_profile_get(
    int unit, 
    bcm_cosq_object_id_t *object_id, 
    bcm_bst_stat_id_t bid, 
    bcm_cosq_ebst_threshold_profile_t *profile);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Flags for EBST monitor setting */
#define BCM_COSQ_EBST_MONITOR_ENABLE_VALID  0x00000001 /* Used to enable EBST
                                                          monitoring */
#define BCM_COSQ_EBST_MONITOR_ENTRY_NUM_VALID 0x00000002 /* Used to set number
                                                          entries */

/* EBST monitor info structure */
typedef struct bcm_cosq_ebst_monitor_s {
    uint32 flags;   /* Monitor flag */
    int enable;     /* Enable EBST monitoring */
    int entry_num;  /* Number entries reserved in each EBST data buffer */
} bcm_cosq_ebst_monitor_t;

/* Initialize a port or queue EBST monitor info structure. */
extern void bcm_cosq_ebst_monitor_t_init(
    bcm_cosq_ebst_monitor_t *monitor);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/Get monitor info for a given BST stat Id. */
extern int bcm_cosq_ebst_monitor_set(
    int unit, 
    bcm_cosq_object_id_t *object_id, 
    bcm_bst_stat_id_t bid, 
    bcm_cosq_ebst_monitor_t *monitor);

/* Set/Get monitor info for a given BST stat Id. */
extern int bcm_cosq_ebst_monitor_get(
    int unit, 
    bcm_cosq_object_id_t *object_id, 
    bcm_bst_stat_id_t bid, 
    bcm_cosq_ebst_monitor_t *monitor);

#endif /* BCM_HIDE_DISPATCHABLE */

/* EBST data buffer info structure */
typedef struct bcm_cosq_ebst_data_stat_info_s {
    int entry_max;  /* Max number entries of EBST data buffer */
    int entry_free; /* Available number entries of EBST data buffer */
} bcm_cosq_ebst_data_stat_info_t;

/* Initialize EBST data buffer info structure. */
extern void bcm_cosq_ebst_data_stat_info_t_init(
    bcm_cosq_ebst_data_stat_info_t *info);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Get max number entries and available number entries of EBST data
 * buffer.
 */
extern int bcm_cosq_ebst_data_stat_info_get(
    int unit, 
    bcm_cosq_object_id_t *object_id, 
    bcm_cosq_ebst_data_stat_info_t *info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Color packets drop state enumerations. */
typedef enum bcm_cosq_color_packet_drop_state_e {
    bcmCosqColorPktAccept = 0,  /* Accept packets */
    bcmCosqColorPktDrop = 1     /* Drop packets */
} bcm_cosq_color_packet_drop_state_t;

/* EBST data buffer info structure */
typedef struct bcm_cosq_ebst_data_entry_s {
    uint64 timestamp;                   /* Timestamp */
    uint32 bytes;                       /* Buffer occupancy level in bytes */
    bcm_cosq_color_packet_drop_state_t green_drop_state; /* Green packet drop state */
    bcm_cosq_color_packet_drop_state_t yellow_drop_state; /* Yellow packet drop state */
    bcm_cosq_color_packet_drop_state_t red_drop_state; /* Red packet drop state */
} bcm_cosq_ebst_data_entry_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get EBST stat for a given stat Id. */
extern int bcm_cosq_ebst_data_get(
    int unit, 
    bcm_cosq_object_id_t *object_id, 
    bcm_bst_stat_id_t bid, 
    int array_size, 
    bcm_cosq_ebst_data_entry_t *entry_array, 
    int *count);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Valid fields. */
#define BCM_COSQ_MOD_CONTROL_ENABLE_VALID   0x00000001 /* Enable valid. */
#define BCM_COSQ_MOD_CONTROL_POOL_LIMIT_VALID 0x00000002 /* Pool_limit valid. */

/* Cosq mirror-on-drop control structure. */
typedef struct bcm_cosq_mod_control_s {
    uint32 valid_flags; /* Control flags to indicate which flag is valid. */
    uint8 enable;       /* Mirror enable/disable. */
    uint32 pool_limit;  /* Pool limit in bytes. */
} bcm_cosq_mod_control_t;

/* Initialize a mirror-on-drop control structure. */
extern void bcm_cosq_mod_control_t_init(
    bcm_cosq_mod_control_t *control);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/Get mirror-on-drop control. */
extern int bcm_cosq_mod_control_set(
    int unit, 
    bcm_cosq_mod_control_t *control);

/* Set/Get mirror-on-drop control. */
extern int bcm_cosq_mod_control_get(
    int unit, 
    bcm_cosq_mod_control_t *control);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Mirror-on-drop profile structure. */
typedef struct bcm_cosq_mod_profile_s {
    uint16 percent_0_25;    /* Sampling probability when current filled cells in
                               mirror-on-drop buffer are less than 25% of
                               maximum cell number. A 16-bit random number is
                               compared against this value and if the random
                               number is less than this value, then the packet
                               is sampled. */
    uint16 percent_25_50;   /* Sampling probability when current filled cells in
                               mirror-on-drop buffer are between 25% and 50% of
                               maximum cell number. */
    uint16 percent_50_75;   /* Sampling probability when current filled cells in
                               mirror-on-drop buffer are between 50% and 75% of
                               maximum cell number. */
    uint16 percent_75_100;  /* Sampling probability when current filled cells in
                               mirror-on-drop buffer are greater than 75% of
                               maximum cell number. */
    uint8 enable;           /* MOD enable/disable. */
} bcm_cosq_mod_profile_t;

/* Initialize a mirror-on-drop profile structure. */
extern void bcm_cosq_mod_profile_t_init(
    bcm_cosq_mod_profile_t *profile);

/* Cosq mirror-on-drop profile options flags. */
#define BCM_COSQ_MOD_PROFILE_OPTIONS_WITH_ID 0x00000001 /* Create mirror-on-drop
                                                          profile with id. */

#ifndef BCM_HIDE_DISPATCHABLE

/* Create/Destroy a mirror-on-drop profile. */
extern int bcm_cosq_mod_profile_create(
    int unit, 
    uint32 options, 
    int *profile_id);

/* Create/Destroy a mirror-on-drop profile. */
extern int bcm_cosq_mod_profile_destroy(
    int unit, 
    int profile_id);

/* Set/Get the mirror-on-drop profile entry with given profile id. */
extern int bcm_cosq_mod_profile_set(
    int unit, 
    int profile_id, 
    bcm_cosq_mod_profile_t *profile);

/* Set/Get the mirror-on-drop profile entry with given profile id. */
extern int bcm_cosq_mod_profile_get(
    int unit, 
    int profile_id, 
    bcm_cosq_mod_profile_t *profile);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Cosq mirror-on-drop statistics. */
typedef enum bcm_cosq_mod_stat_e {
    bcmCosqModStatDroppedPackets = 0,   /* Number of mirror-on-drop eligible
                                           packets that are sampled but failed
                                           to be mirrored. */
    bcmCosqModStatEnqueuedPackets = 1,  /* Number of mirror-on-drop eligible
                                           packets that are sampled and mirrored
                                           to queue. */
    bcmCosqModStatPoolBytesCurrent = 2, /* Current value of buffer usage of
                                           mirror-on-drop pool. */
    bcmCosqModStatPoolBytesPeak = 3,    /* Peak value of buffer usage of
                                           mirror-on-drop pool. */
    bcmCosqModStatDestQueueBytesCurrent = 4, /* Current value of buffer usage of
                                           mirror-on-drop destination queue. */
    bcmCosqModStatMaxCount = 5          /* This should be the last one. */
} bcm_cosq_mod_stat_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/Get statistics counter for a statistics object. */
extern int bcm_cosq_mod_stat_set(
    int unit, 
    bcm_cosq_object_id_t *id, 
    bcm_cosq_mod_stat_t stat, 
    uint64 value);

/* Set/Get statistics counter for a statistics object. */
extern int bcm_cosq_mod_stat_get(
    int unit, 
    bcm_cosq_object_id_t *id, 
    bcm_cosq_mod_stat_t stat, 
    uint64 *value);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Types of congestion element. */
typedef enum bcm_cosq_inst_congestion_info_type_e {
    bcmCosqInstCongestionInfoTypePgVsq = 0, /* Info type PgVsq */
    bcmCosqInstCongestionInfoTypeLlfcVsq = 1, /* Info type LlfcVsq */
    bcmCosqInstCongestionInfoTypeVoq = 2 /* Info Type Voq */
} bcm_cosq_inst_congestion_info_type_t;

/* Congestion instrumentation key attributes */
typedef struct bcm_cosq_inst_congestion_info_key_s {
    bcm_cosq_inst_congestion_info_type_t type; /* type of congestion */
    bcm_core_t core_id;                 /* core id */
} bcm_cosq_inst_congestion_info_key_t;

/* Congestion instrumentation output attributes */
typedef struct bcm_cosq_inst_congestion_info_data_s {
    int valid;                      /* Indication whether congestion info is
                                       valid */
    bcm_cosq_resource_t resource;   /* Type of congested resource */
    int pool_id;                    /* Relevant only for llfc vsq type */
    uint32 id;                      /* Id of congested element */
} bcm_cosq_inst_congestion_info_data_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get next congestion element data. */
extern int bcm_cosq_inst_congestion_info_get(
    int unit, 
    uint32 flags, 
    bcm_cosq_inst_congestion_info_key_t *key, 
    bcm_cosq_inst_congestion_info_data_t *data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Congestion/Flow control types. */
typedef enum bcm_cosq_subport_congestion_type_e {
    bcmCosqSubportCongestionTypeNone = 0, 
    bcmCosqSubportCongestionTypePause = 1, 
    bcmCosqSubportCongestionTypePfc = 2, 
    bcmCosqSubportCongestionTypeE2ecc = 3 
} bcm_cosq_subport_congestion_type_t;

/* Subport congestion config information structure. */
typedef struct bcm_cosq_subport_congestion_config_s {
    int subport_speed;              /* Speed of the subport. */
    int cos_to_pfc_map_profile_id;  /* Index of the mapping in
                                       CH_PRIO2COS_PROFILE. */
    uint32 status;                  /* 0 disable 1 enable. */
} bcm_cosq_subport_congestion_config_t;

/* Initialize a subport congestion config structure. */
extern void bcm_cosq_subport_congestion_config_t_init(
    bcm_cosq_subport_congestion_config_t *mapping);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Configure the reception of congestion control (PAUSE, PFC and E2ECC)
 * on subport level
 */
extern int bcm_cosq_subport_congestion_config_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_cosq_subport_congestion_type_t congestion_type, 
    bcm_cosq_subport_congestion_config_t *congestion_config);

/* 
 * Get the configured congestion control (PAUSE, PFC and E2ECC) info on
 * subport level
 */
extern int bcm_cosq_subport_congestion_config_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_cosq_subport_congestion_type_t congestion_type, 
    bcm_cosq_subport_congestion_config_t *congestion_config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Total number of PFC controlled COSqs(L1 nodes) under each sub port. */
#define BCM_COSQ_SUBPORT_COS_COUNT  8          

/* PFC class to subport COSq(L1 nodes)  mapping structure. */
typedef struct bcm_subport_cos_to_pfc_pri_map_s {
    int cos_to_pfc_pri_low[BCM_COSQ_SUBPORT_COS_COUNT]; /* List of COSq(L1 nodes) that mapped to
                                           a Low priority PFC class. */
    int cos_to_pfc_pri_high[BCM_COSQ_SUBPORT_COS_COUNT]; /* List of COSq(L1 nodes) that mapped to
                                           a High priority PFC class. */
} bcm_subport_cos_to_pfc_pri_map_t;

/* Initialize a subport COSq PFC class mapping structure. */
extern void bcm_subport_cos_to_pfc_pri_map_t_init(
    bcm_subport_cos_to_pfc_pri_map_t *cos_to_pfc_map);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Set mapping of PFC class (LO and HIGH) to COSQs L1 nodes under a
 * subport.
 */
extern int bcm_cosq_subport_pfc_class_mapping_set(
    int unit, 
    bcm_gport_t subport_gport, 
    bcm_subport_cos_to_pfc_pri_map_t *cos_to_pfc_map, 
    int *cos_to_pfc_map_profile_id);

/* 
 * Get mapping of PFC class (LO and HIGH) to COSQs L1 nodes under a
 * subport.
 */
extern int bcm_cosq_subport_pfc_class_mapping_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_subport_cos_to_pfc_pri_map_t *cos_to_pfc_map);

#endif /* BCM_HIDE_DISPATCHABLE */

/* PFC deadlock recovery mode */
typedef enum bcm_cosq_pfc_dd_recovery_mode_e {
    bcmCosqPfcDdRecoveryModeTimer = 0, 
    bcmCosqPfcDdRecoveryModePFCState = 1, 
    bcmCosqPfcDdRecoveryModeMax = 2 
} bcm_cosq_pfc_dd_recovery_mode_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set recovery moide in PFC deadlock */
extern int bcm_cosq_pfc_dd_recovery_mode_set(
    int unit, 
    bcm_port_t port, 
    bcm_cosq_pfc_dd_recovery_mode_t recovery_mode);

/* Get recovery moide in PFC deadlock */
extern int bcm_cosq_pfc_dd_recovery_mode_get(
    int unit, 
    bcm_port_t port, 
    bcm_cosq_pfc_dd_recovery_mode_t *recovery_mode);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Match attributes for MMU debug counter */
#define BCM_COSQ_DEBUG_STAT_MATCH_SRC_PORT  0x00000001 /* Select source port as
                                                          a match attribute */
#define BCM_COSQ_DEBUG_STAT_MATCH_SRC_PIPE  0x00000002 /* Select source pipe as
                                                          a match attribute.
                                                          Source pipe number
                                                          will be deduced from
                                                          src_port */
#define BCM_COSQ_DEBUG_STAT_MATCH_DST_PORT  0x00000004 /* Select destination
                                                          port as a match
                                                          attribute */
#define BCM_COSQ_DEBUG_STAT_MATCH_DST_PIPE  0x00000008 /* Select destination
                                                          pipe as a match
                                                          attribute. Destination
                                                          pipe number will be
                                                          deduced from dst_port */
#define BCM_COSQ_DEBUG_STAT_MATCH_QUEUE     0x00000010 /* Select queue as a
                                                          match attribute. UC/MC
                                                          queue type will be
                                                          deduced from queue
                                                          number */
#define BCM_COSQ_DEBUG_STAT_MATCH_PRIORITY_GROUP 0x00000020 /* Select priority group
                                                          as a match attribute */

/* MMU debug counter match criteria */
typedef struct bcm_cosq_debug_stat_match_s {
    uint32 match_flags;     /* Flags to indicate which packet attributes are
                               selected as match for a certain MMU debug counter */
    bcm_port_t src_port;    /* Source port */
    bcm_port_t dst_port;    /* Destination port */
    bcm_cos_queue_t cosq;   /* Queue ID */
    int priority_group_id;  /* Priority group id */
} bcm_cosq_debug_stat_match_t;

/* Initialize MMU debug counter match structure */
extern void bcm_cosq_debug_stat_match_t_init(
    bcm_cosq_debug_stat_match_t *match);

#ifndef BCM_HIDE_DISPATCHABLE

/* Get MMU debug counter match configuration */
extern int bcm_cosq_debug_stat_match_get(
    int unit, 
    bcm_port_t port, 
    bcm_cosq_stat_t stat, 
    bcm_cosq_debug_stat_match_t *match);

/* Set MMU debug counter match configuration */
extern int bcm_cosq_debug_stat_match_set(
    int unit, 
    bcm_port_t port, 
    bcm_cosq_stat_t stat, 
    bcm_cosq_debug_stat_match_t *match);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_COSQ_H__ */
