/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_INT_H__
#define __BCM_INT_H__

#if defined(INCLUDE_INT)

#include <bcm/types.h>

/* INT Turnaround configuration information data. */
typedef struct bcm_int_turnaround_config_s {
    bcm_mac_t src_mac; 
} bcm_int_turnaround_config_t;

/* INT Turnaround statistics information data. */
typedef struct bcm_int_turnaround_stat_s {
    uint64 rx_pkt_cnt;                  /* Number of packet recieved in FW. */
    uint64 tx_pkt_cnt;                  /* Number of packet transmitted from FW. */
    uint64 drop_pkt_cnt;                /* Number of packet dropped in FW. */
    uint64 int_init_config_drop;        /* Number of pkt dropped due to missing
                                           configuration. */
    uint64 int_hop_cnt_invalid_drop;    /* Number of pkt dropped due to Hop
                                           count exceeded Hop limit. */
    uint64 int_hdr_len_invalid_drop;    /* Number of pkt dropped due to current
                                           length exceeded maximum length. */
    uint64 int_pkt_size_invalid_drop;   /* Number of pkt dropped due to invalid
                                           packet size. */
} bcm_int_turnaround_stat_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Initializes int subsystem. */
extern int bcm_int_init(
    int unit);

/* Detaches int subsystem. */
extern int bcm_int_detach(
    int unit);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize INT Turnaround configuration information structure. */
extern void bcm_int_turnaround_config_t_init(
    bcm_int_turnaround_config_t *config_info);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set INT Turnaround - configuration information */
extern int bcm_int_turnaround_config_set(
    int unit, 
    bcm_int_turnaround_config_t *config_data);

/* Get INT Turnaround - configuration information */
extern int bcm_int_turnaround_config_get(
    int unit, 
    bcm_int_turnaround_config_t *config_data);

/* INT Turnaround - INT Turnaround statistics information */
extern int bcm_int_turnaround_stat_get(
    int unit, 
    bcm_int_turnaround_stat_t *stat_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* IFA vector match add options. */
#define BCM_INT_IFA_VECTOR_MATCH_OPTIONS_REPLACE (1 << 0)   /* Replace the IFA vector
                                                          match. */

/* IFA vector match criteria. */
typedef struct bcm_int_ifa_vector_match_s {
    int priority;               /* The priority of the vector match. */
    uint16 gns;                 /* Value to match against the global namespace
                                   (GNS) in the IFA base header. */
    uint16 gns_mask;            /* Mask value to be applied to the global
                                   namespace in the IFA packet before matching
                                   with field gns. */
    uint8 lns;                  /* Value to match against the local namespace
                                   (LNS) in the IFA metadata. */
    uint8 lns_mask;             /* Mask value to be applied to the local
                                   namespace in the IFA packet before matching
                                   with field lns. */
    uint32 request_vector;      /* Value to match against the request vector in
                                   the IFA metadata header. */
    uint32 request_vector_mask; /* Mask value to be applied to the request
                                   vector in the IFA packet before matching with
                                   field request_vector. */
    uint8 metadata_profile_id;  /* Profile ID to select and construct IFA
                                   metadata. */
} bcm_int_ifa_vector_match_t;

/* IFA vector match callback function prototype. */
typedef int(*bcm_int_ifa_vector_match_traverse_cb)(
    int unit, 
    bcm_int_ifa_vector_match_t *match, 
    void *user_data);

/* Initialize the bcm_int_ifa_vector_match_t struct. */
extern void bcm_int_ifa_vector_match_t_init(
    bcm_int_ifa_vector_match_t *match);

#ifndef BCM_HIDE_DISPATCHABLE

/* Add the vector match for IFA. */
extern int bcm_int_ifa_vector_match_add(
    int unit, 
    uint32 options, 
    bcm_int_ifa_vector_match_t *match);

/* Get the vector match for IFA. */
extern int bcm_int_ifa_vector_match_get(
    int unit, 
    bcm_int_ifa_vector_match_t *match);

/* Delete the vector match for IFA. */
extern int bcm_int_ifa_vector_match_delete(
    int unit, 
    bcm_int_ifa_vector_match_t *match);

/* Delete all vector matches for IFA. */
extern int bcm_int_ifa_vector_match_delete_all(
    int unit);

/* Traverse all vector matches for IFA. */
extern int bcm_int_ifa_vector_match_traverse(
    int unit, 
    bcm_int_ifa_vector_match_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* IOAM vector match add options. */
#define BCM_INT_IOAM_VECTOR_MATCH_OPTIONS_REPLACE (1 << 0)   /* Replace the IOAM
                                                          vector match. */

/* IOAM vector match criteria. */
typedef struct bcm_int_ioam_vector_match_s {
    int priority;               /* The priority of the vector match. */
    uint16 namespace_id;        /* Value to match against the namespace
                                   identifier in the IOAM  metadata header. */
    uint16 namespace_id_mask;   /* Mask value to be applied to the namespace
                                   identifier in the IOAM packet before matching
                                   with namespace ID. */
    uint32 trace_type;          /* Value to match against the trace type in the
                                   IOAM metadata header. */
    uint32 trace_type_mask;     /* Mask value to be applied to the trace type in
                                   the IOAM packet before matching with trace
                                   type. */
    uint8 metadata_profile_id;  /* Profile ID to select and construct IOAM
                                   metadata. */
} bcm_int_ioam_vector_match_t;

/* IOAM vector match callback function prototype. */
typedef int(*bcm_int_ioam_vector_match_traverse_cb)(
    int unit, 
    bcm_int_ioam_vector_match_t *match, 
    void *user_data);

/* Initialize the bcm_int_ioam_vector_match_t struct. */
extern void bcm_int_ioam_vector_match_t_init(
    bcm_int_ioam_vector_match_t *match);

#ifndef BCM_HIDE_DISPATCHABLE

/* Add the vector match for IOAM. */
extern int bcm_int_ioam_vector_match_add(
    int unit, 
    uint32 options, 
    bcm_int_ioam_vector_match_t *match);

/* Get the vector match for IOAM. */
extern int bcm_int_ioam_vector_match_get(
    int unit, 
    bcm_int_ioam_vector_match_t *match);

/* Delete the vector match for IOAM. */
extern int bcm_int_ioam_vector_match_delete(
    int unit, 
    bcm_int_ioam_vector_match_t *match);

/* Delete all vector matches for IOAM. */
extern int bcm_int_ioam_vector_match_delete_all(
    int unit);

/* Traverse all vector matches for IOAM. */
extern int bcm_int_ioam_vector_match_traverse(
    int unit, 
    bcm_int_ioam_vector_match_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* INT-DP vector match add options. */
#define BCM_INT_DP_VECTOR_MATCH_OPTIONS_REPLACE (1 << 0)   /* Replace the INT-DP
                                                          vector match. */

/* INT-DP vector match criteria. */
typedef struct bcm_int_dp_vector_match_s {
    int priority;               /* The priority of the vector match. */
    uint32 request_vector;      /* Value to match against the telemetry request
                                   vector in the INT-DP header. */
    uint32 request_vector_mask; /* Mask value to be applied to the telemetry
                                   request vector in the INT-DP packet before
                                   matching with request vector. */
    uint8 metadata_profile_id;  /* Profile ID to select and construct INT-DP
                                   metadata. */
} bcm_int_dp_vector_match_t;

/* INT-DP vector match callback function prototype. */
typedef int(*bcm_int_dp_vector_match_traverse_cb)(
    int unit, 
    bcm_int_dp_vector_match_t *match, 
    void *user_data);

/* Initialize the bcm_int_dp_vector_match_t struct. */
extern void bcm_int_dp_vector_match_t_init(
    bcm_int_dp_vector_match_t *match);

#ifndef BCM_HIDE_DISPATCHABLE

/* Add the vector match for INT-DP. */
extern int bcm_int_dp_vector_match_add(
    int unit, 
    uint32 options, 
    bcm_int_dp_vector_match_t *match);

/* Get the vector match for INT-DP. */
extern int bcm_int_dp_vector_match_get(
    int unit, 
    bcm_int_dp_vector_match_t *match);

/* Delete the vector match for INT-DP. */
extern int bcm_int_dp_vector_match_delete(
    int unit, 
    bcm_int_dp_vector_match_t *match);

/* Delete all vector matches for INT-DP. */
extern int bcm_int_dp_vector_match_delete_all(
    int unit);

/* Traverse all vector matches for INT-DP. */
extern int bcm_int_dp_vector_match_traverse(
    int unit, 
    bcm_int_dp_vector_match_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* INT header operation mode. */
typedef enum bcm_int_header_operation_mode_e {
    bcmIntHeaderOperationNone = 0,      /* Do not insert metadata. Applies for
                                           INT-DP termination. */
    bcmIntHeaderOperationMetadataInsert = 1, /* Only insert metadata. */
    bcmIntHeaderOperationCount = 2      /* Always last. Not a usable value. */
} bcm_int_header_operation_mode_t;

/* INT timestamp mode. */
typedef enum bcm_int_timestamp_mode_e {
    bcmIntTimestampPtp = 0,     /* Precision Time Protocol */
    bcmIntTimestampNtp = 1,     /* Network Time Protocol */
    bcmIntTimestampCount = 2    /* Always last. Not a usable value. */
} bcm_int_timestamp_mode_t;

/* INT residence time format. */
typedef enum bcm_int_residence_time_format_e {
    bcmIntResidenceTime48Bits = 0,  /* Nanoseconds up to 48 bits. */
    bcmIntResidenceTime32Bits = 1,  /* IOAM compliant, nanoseconds up to 31
                                       bits. 0x80000000 indicates overflow. */
    bcmIntResidenceTimeCount = 2    /* Always last. Not a usable value. */
} bcm_int_residence_time_format_t;

/* IFA action profile create options. */
#define BCM_INT_IFA_ACTION_PROFILE_OPTIONS_WITH_ID (1 << 0)   /* Create the IFA action
                                                          profile with ID. */
#define BCM_INT_IFA_ACTION_PROFILE_OPTIONS_REPLACE (1 << 1)   /* Replace the IFA action
                                                          profile. */

/* IFA action profile flags. */
#define BCM_INT_IFA_ACTION_PROFILE_FLAGS_IP_LENGTH_UPDATE (1 << 0)   /* Enable to update IP
                                                          length after metadata
                                                          insertion. */

/* IFA action profile. */
typedef struct bcm_int_ifa_action_profile_s {
    uint32 flags;                       /* Action profile flags */
    bcm_int_timestamp_mode_t timestamp_mode; /* Timestamp mode for ingress timestamp,
                                           egress timestamp and residence time. */
    bcm_int_residence_time_format_t residence_time_format; /* Format for residence time. Only
                                           bcmIntResidenceTime48Bits is
                                           applicable. */
    bcm_int_header_operation_mode_t header_operation_mode; /* Header operation mode. */
    uint8 metadata_profile_id;          /* Profile ID to select metadata fields
                                           and construct IFA metadata. This can
                                           be used to overwrite the profile
                                           assigned by IFA vector match. */
} bcm_int_ifa_action_profile_t;

/* IFA action profile callback function prototype. */
typedef int(*bcm_int_ifa_action_profile_traverse_cb)(
    int unit, 
    int *profile_id, 
    bcm_int_ifa_action_profile_t *profile, 
    void *user_data);

/* Initialize the bcm_int_ifa_action_profile_t struct. */
extern void bcm_int_ifa_action_profile_t_init(
    bcm_int_ifa_action_profile_t *profile);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create the IFA action profile. */
extern int bcm_int_ifa_action_profile_create(
    int unit, 
    uint32 options, 
    int *profile_id, 
    bcm_int_ifa_action_profile_t *profile);

/* Get the IFA action profile. */
extern int bcm_int_ifa_action_profile_get(
    int unit, 
    int profile_id, 
    bcm_int_ifa_action_profile_t *profile);

/* Destroy the IFA action profile. */
extern int bcm_int_ifa_action_profile_destroy(
    int unit, 
    int profile_id);

/* Traverse all IFA action profiles. */
extern int bcm_int_ifa_action_profile_traverse(
    int unit, 
    bcm_int_ifa_action_profile_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* IOAM action profile create options. */
#define BCM_INT_IOAM_ACTION_PROFILE_OPTIONS_WITH_ID (1 << 0)   /* Create the IOAM action
                                                          profile with ID. */
#define BCM_INT_IOAM_ACTION_PROFILE_OPTIONS_REPLACE (1 << 1)   /* Replace the IOAM
                                                          action profile. */

/* IOAM action profile flags. */
#define BCM_INT_IOAM_ACTION_PROFILE_FLAGS_IP_LENGTH_UPDATE (1 << 0)   /* Enable to update IP
                                                          length after metadata
                                                          insertion. */

/* IOAM O-bit update mode. */
typedef enum bcm_int_ioam_o_bit_update_mode_e {
    bcmIntIoamObitUpdateSkip = 0,       /* Do not update O-bit. */
    bcmIntIoamObitUpdateOverflow = 1,   /* Update O-bit per max length check.
                                           The O-bit will be set if there is no
                                           enough room for the metadata to be
                                           inserted. */
    bcmIntIoamObitUpdateCount = 2       /* Always last. Not a usable value. */
} bcm_int_ioam_o_bit_update_mode_t;

/* IOAM action profile. */
typedef struct bcm_int_ioam_action_profile_s {
    uint32 flags;                       /* Action profile flags */
    bcm_int_timestamp_mode_t timestamp_mode; /* Timestamp mode for ingress timestamp,
                                           egress timestamp and residence time. */
    bcm_int_residence_time_format_t residence_time_format; /* Format for residence time. */
    bcm_int_header_operation_mode_t header_operation_mode; /* Header operation mode. */
    bcm_int_ioam_o_bit_update_mode_t o_bit_update_mode; /* The O-bit updating mode during
                                           metadata insertion. */
    uint8 metadata_profile_id;          /* Profile ID to select metadata fields
                                           and construct IOAM metadata. This can
                                           be used to overwrite the profile
                                           assigned by IOAM vector match. */
} bcm_int_ioam_action_profile_t;

/* IOAM action profile callback function prototype. */
typedef int(*bcm_int_ioam_action_profile_traverse_cb)(
    int unit, 
    int *profile_id, 
    bcm_int_ioam_action_profile_t *profile, 
    void *user_data);

/* Initialize the bcm_int_ioam_action_profile_t struct. */
extern void bcm_int_ioam_action_profile_t_init(
    bcm_int_ioam_action_profile_t *profile);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create the IOAM action profile. */
extern int bcm_int_ioam_action_profile_create(
    int unit, 
    uint32 options, 
    int *profile_id, 
    bcm_int_ioam_action_profile_t *profile);

/* Get the IOAM action profile. */
extern int bcm_int_ioam_action_profile_get(
    int unit, 
    int profile_id, 
    bcm_int_ioam_action_profile_t *profile);

/* Destroy the IOAM action profile. */
extern int bcm_int_ioam_action_profile_destroy(
    int unit, 
    int profile_id);

/* Traverse all IOAM action profiles. */
extern int bcm_int_ioam_action_profile_traverse(
    int unit, 
    bcm_int_ioam_action_profile_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* INT-DP action profile create options. */
#define BCM_INT_DP_ACTION_PROFILE_OPTIONS_WITH_ID (1 << 0)   /* Create the INT-DP
                                                          action profile with
                                                          ID. */
#define BCM_INT_DP_ACTION_PROFILE_OPTIONS_REPLACE (1 << 1)   /* Replace the INT-DP
                                                          action profile. */

/* INT-DP action profile flags. */
#define BCM_INT_DP_ACTION_PROFILE_FLAGS_IP_LENGTH_UPDATE (1 << 0)   /* Enable to update IP
                                                          length after metadata
                                                          insertion. */
#define BCM_INT_DP_ACTION_PROFILE_FLAGS_UDP_LENGTH_UPDATE (1 << 1)   /* Enable to update UDP
                                                          length after metadata
                                                          insertion. */

/* INT-DP action profile. */
typedef struct bcm_int_dp_action_profile_s {
    uint32 flags;                       /* Action profile flags. */
    bcm_int_timestamp_mode_t timestamp_mode; /* Timestamp mode for ingress timestamp,
                                           egress timestamp and residence time. */
    bcm_int_residence_time_format_t residence_time_format; /* Format for residence time. Only
                                           bcmIntResidenceTime48Bits is
                                           applicable. */
    bcm_int_header_operation_mode_t header_operation_mode; /* Header operation mode. */
    uint8 metadata_profile_id;          /* Profile ID to select metadata fields
                                           and construct INT-DP metadata. This
                                           can be used to overwrite the profile
                                           assigned by INT-DP vector match. */
} bcm_int_dp_action_profile_t;

/* INT-DP action profile callback function prototype. */
typedef int(*bcm_int_dp_action_profile_traverse_cb)(
    int unit, 
    int *profile_id, 
    bcm_int_dp_action_profile_t *profile, 
    void *user_data);

/* Initialize the bcm_int_dp_action_profile_t struct. */
extern void bcm_int_dp_action_profile_t_init(
    bcm_int_dp_action_profile_t *profile);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create the INT-DP action profile. */
extern int bcm_int_dp_action_profile_create(
    int unit, 
    uint32 options, 
    int *profile_id, 
    bcm_int_dp_action_profile_t *profile);

/* Get the INT-DP action profile. */
extern int bcm_int_dp_action_profile_get(
    int unit, 
    int profile_id, 
    bcm_int_dp_action_profile_t *profile);

/* Destroy the INT-DP action profile. */
extern int bcm_int_dp_action_profile_destroy(
    int unit, 
    int profile_id);

/* Traverse all INT-DP action profiles. */
extern int bcm_int_dp_action_profile_traverse(
    int unit, 
    bcm_int_dp_action_profile_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* INT opaque data profile create options. */
#define BCM_INT_OPAQUE_DATA_PROFILE_OPTIONS_WITH_ID (1 << 0)   /* Create the opaque data
                                                          profile for INT
                                                          metadata construction
                                                          with ID. */
#define BCM_INT_OPAQUE_DATA_PROFILE_OPTIONS_REPLACE (1 << 1)   /* Replace the opaque
                                                          data profile for INT
                                                          metadata construction. */

/* INT opaque data field. */
#define BCM_INT_MAX_OPAQUE_DATA 8          /* Max number of INT opaque data. */

/* INT opaque data profile. */
typedef struct bcm_int_opaque_data_profile_s {
    uint32 opaque_data[BCM_INT_MAX_OPAQUE_DATA]; /* Opaque data field. */
} bcm_int_opaque_data_profile_t;

/* INT opaque data profile callback function prototype. */
typedef int(*bcm_int_opaque_data_profile_traverse_cb)(
    int unit, 
    int *profile_id, 
    bcm_int_opaque_data_profile_t *profile, 
    void *user_data);

/* Initialize the bcm_int_opaque_data_profile_t struct. */
extern void bcm_int_opaque_data_profile_t_init(
    bcm_int_opaque_data_profile_t *profile);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create the opaque data profile for INT metadata construction. */
extern int bcm_int_opaque_data_profile_create(
    int unit, 
    uint32 options, 
    int *profile_id, 
    bcm_int_opaque_data_profile_t *profile);

/* Get the opaque data profile for INT metadata construction. */
extern int bcm_int_opaque_data_profile_get(
    int unit, 
    int profile_id, 
    bcm_int_opaque_data_profile_t *profile);

/* Destroy the opaque data profile for INT metadata construction. */
extern int bcm_int_opaque_data_profile_destroy(
    int unit, 
    int profile_id);

/* Traverse all the opaque data profiles for INT metadata construction. */
extern int bcm_int_opaque_data_profile_traverse(
    int unit, 
    bcm_int_opaque_data_profile_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* INT COSQ stat report mode. */
typedef enum bcm_int_cosq_stat_report_mode_e {
    bcmIntCosqStatReportQueueBased = 0, /* Queue based */
    bcmIntCosqStatReportPortBased = 1,  /* Port based */
    bcmIntCosqStatReportCount = 2       /* Always last. Not a usable value. */
} bcm_int_cosq_stat_report_mode_t;

/* INT COSQ stat configuration. */
typedef struct bcm_int_cosq_stat_config_s {
    bcm_int_cosq_stat_report_mode_t report_mode; /* Select to report port or queue based
                                           usage in bytes, including current and
                                           maximum usage. */
    int max_bytes_enable;               /* Enable to use max_bytes field as
                                           maximum usage in bytes when BST is
                                           not enabled. */
    uint32 max_bytes;                   /* Maximum usage in bytes. Applies for
                                           cut-through packets or when BST is
                                           not enabled. */
    uint32 current_bytes;               /* Current usage in bytes. Applies for
                                           cut-through packets. */
    uint32 current_available_bytes;     /* Current available buffer in bytes.
                                           Applies for cut-through packets. */
    uint32 min_available_bytes;         /* Minimum available buffer in bytes.
                                           Applies for cut-through packets or
                                           when BST is not enabled. */
} bcm_int_cosq_stat_config_t;

/* Initialize the bcm_int_cosq_stat_config_t struct. */
extern void bcm_int_cosq_stat_config_t_init(
    bcm_int_cosq_stat_config_t *config);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set INT COSQ stat configuration. */
extern int bcm_int_cosq_stat_config_set(
    int unit, 
    bcm_int_cosq_stat_config_t *config);

/* Get INT COSQ stat configuration. */
extern int bcm_int_cosq_stat_config_get(
    int unit, 
    bcm_int_cosq_stat_config_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* INT metadata profile create options. */
#define BCM_INT_METADATA_PROFILE_OPTIONS_WITH_ID (1 << 0)   /* Create the INT
                                                          metadata profile with
                                                          ID. */

/* INT metadata field. */
typedef enum bcm_int_metadata_field_e {
    bcmIntMetadataPadOne = 0,           /* Pad ones */
    bcmIntMetadataPadZero = 1,          /* Pad zeros */
    bcmIntMetadataOpaqueData1 = 2,      /* Opaque data 1 */
    bcmIntMetadataOpaqueData2 = 3,      /* Opaque data 2 */
    bcmIntMetadataOpaqueData3 = 4,      /* Opaque data 3 */
    bcmIntMetadataSwitchId = 5,         /* Switch ID */
    bcmIntMetadataResidenceTimeSubSeconds = 6, /* Sub-seconds portion of residence time */
    bcmIntMetadataCosqStat0 = 7,        /* Stat 0 generated by traffic manager
                                           (TM) */
    bcmIntMetadataCosqStat1 = 8,        /* Stat 1 generated by traffic manager
                                           (TM) */
    bcmIntMetadataIngressTimestampSubSeconds = 9, /* Sub-seconds portion of ingress
                                           timestamp */
    bcmIntMetadataEgressTimestampSubSeconds = 10, /* Sub-seconds portion of egress
                                           timestamp */
    bcmIntMetadataIngressTimestampSecondsLower = 11, /* Lower seconds portion of ingress
                                           timestamp */
    bcmIntMetadataEgressTimestampSecondsLower = 12, /* Lower seconds portion of egress
                                           timestamp */
    bcmIntMetadataIngressTimestampSecondsUpper = 13, /* Higher seconds portion of ingress
                                           timestamp */
    bcmIntMetadataEgressTimestampSecondsUpper = 14, /* Higher seconds portion of egress
                                           timestamp */
    bcmIntMetadataResidenceTimeSeconds = 15, /* Seconds portion of residence time */
    bcmIntMetadataOverlayL3EgrIntf = 16, /* Overlay L3 egress interface */
    bcmIntMetadataL3EgrIntf = 17,       /* L3 egress interface */
    bcmIntMetadataL3IngIntf = 18,       /* L3 ingress interface */
    bcmIntMetadataIngPortId = 19,       /* Ingress port ID */
    bcmIntMetadataEgrPortId = 20,       /* Egress port ID */
    bcmIntMetadataVrf = 21,             /* VRF */
    bcmIntMetadataVfi = 22,             /* VFI */
    bcmIntMetadataDvp = 23,             /* DVP */
    bcmIntMetadataSvp = 24,             /* SVP */
    bcmIntMetadataTunnelEncapIndex = 25, /* Tunnel encapsulation index */
    bcmIntMetadataCng = 26,             /* Internal color/drop precedence */
    bcmIntMetadataCongestion = 27,      /* Congestion marked status */
    bcmIntMetadataQueueId = 28,         /* Destination queue ID */
    bcmIntMetadataProfileId = 29,       /* Metadata profile ID */
    bcmIntMetadataCutThrough = 30,      /* Cut through status */
    bcmIntMetadataQueueIdProfileIdCutThrough = 31, /* Destination queue ID, metadata
                                           profile ID and cut through status */
    bcmIntMetadataCngQueueIdProfileIdCutThrough = 32, /* Internal color/drop precedence,
                                           destination queue ID, metadata
                                           profile ID and cut through status */
    bcmIntMetadataCount = 33            /* Always last. Not a usable value. */
} bcm_int_metadata_field_t;

/* INT metadata field select. */
typedef struct bcm_int_metadata_field_select_s {
    bcm_int_metadata_field_t field; /* Metadata field. */
    uint8 start;                    /* Starting bit position of the field within
                                       the chunk, from the MSB of the chunk. */
    uint8 size;                     /* Size of the field in bits. */
    uint8 shift;                    /* Number of bits of the field to barrel
                                       shift towards LSB. */
} bcm_int_metadata_field_select_t;

/* INT metadata field construct. */
typedef struct bcm_int_metadata_field_construct_s {
    uint8 offset;   /* The relative offset of the field within metadata,
                       starting from the MSB of metadata, with byte as
                       granularity. */
} bcm_int_metadata_field_construct_t;

/* INT metadata field entry. */
typedef struct bcm_int_metadata_field_entry_s {
    bcm_int_metadata_field_select_t select; /* Metadata field select. */
    bcm_int_metadata_field_construct_t construct; /* Metadata field construct. */
} bcm_int_metadata_field_entry_t;

/* INT metadata field select information. */
typedef struct bcm_int_metadata_field_select_info_s {
    uint8 start;    /* Starting bit position of the field within the chunk, from
                       the MSB of the chunk. */
    uint8 size;     /* Size of the field in bits. */
} bcm_int_metadata_field_select_info_t;

/* INT metadata profile callback function prototype. */
typedef int(*bcm_int_metadata_profile_traverse_cb)(
    int unit, 
    int *profile_id, 
    bcm_int_metadata_field_entry_t *field_entry, 
    void *user_data);

/* Initialize the bcm_int_metadata_field_select_t struct. */
extern void bcm_int_metadata_field_select_t_init(
    bcm_int_metadata_field_select_t *field_select);

/* Initialize the bcm_int_metadata_field_construct_t struct. */
extern void bcm_int_metadata_field_construct_t_init(
    bcm_int_metadata_field_construct_t *field_construct);

/* Initialize the bcm_int_metadata_field_entry_t struct. */
extern void bcm_int_metadata_field_entry_t_init(
    bcm_int_metadata_field_entry_t *field_entry);

/* Initialize the int_metadata_field_select_info_t struct. */
extern void bcm_int_metadata_field_select_info_t_init(
    bcm_int_metadata_field_select_info_t *field_select_info);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create the metadata profile. */
extern int bcm_int_metadata_profile_create(
    int unit, 
    uint32 options, 
    int *profile_id);

/* Get the metadata profile. */
extern int bcm_int_metadata_profile_get(
    int unit, 
    int profile_id, 
    int array_size, 
    bcm_int_metadata_field_entry_t *entry_array, 
    int *field_count);

/* Destroy the metadata profile. */
extern int bcm_int_metadata_profile_destroy(
    int unit, 
    int profile_id);

/* Traverse all metadata profiles. */
extern int bcm_int_metadata_profile_traverse(
    int unit, 
    bcm_int_metadata_profile_traverse_cb cb, 
    void *user_data);

/* Add the metadata field to the metadata profile. */
extern int bcm_int_metadata_field_add(
    int unit, 
    int profile_id, 
    bcm_int_metadata_field_entry_t *field_entry);

/* Delete the metadata field from the metadata profile. */
extern int bcm_int_metadata_field_delete(
    int unit, 
    int profile_id, 
    bcm_int_metadata_field_entry_t *field_entry);

/* Delete all the metadata field from the metadata profile. */
extern int bcm_int_metadata_field_delete_all(
    int unit, 
    int profile_id);

/* Get the metadata field select information. */
extern int bcm_int_metadata_field_select_info_get(
    int unit, 
    bcm_int_metadata_field_t field, 
    bcm_int_metadata_field_select_info_t *field_select_info);

#endif /* defined(INCLUDE_INT) */

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_INT_H__ */
