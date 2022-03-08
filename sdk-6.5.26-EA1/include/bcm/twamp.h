/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_TWAMP_H__
#define __BCM_TWAMP_H__

#include <bcm/types.h>

/*  TWAMP flags */
#define BCM_TWAMP_SENDER_REPLACE            0x01       /* Replace an existing
                                                          TWAMP Sender */
#define BCM_TWAMP_SENDER_WITH_ID            0x02       /* Use the specified
                                                          TWAMP sender node ID */
#define BCM_TWAMP_SENDER_WITH_REMOTE_ID     0x04       /* Use the specified
                                                          remote TWAMP Sender ID */
#define BCM_TWAMP_SENDER_LIVENESS_ENABLE    0x08       /* Enable Liveness
                                                          functionality */
#define BCM_TWAMP_SENDER_IPV6               0x10       /* Use IPV6 Encapsulation */
#define BCM_TWAMP_SENDER_REMOTE_LIVENESS_UPDATE_STATE_DISABLE 0x20       /* Disable updating the
                                                          fault state but
                                                          generate an event. */
#define BCM_TWAMP_SENDER_REMOTE_LIVENESS_EVENT_DISABLE 0x40       /* Do not generate event
                                                          but update fault
                                                          state. */
#define BCM_TWAMP_SENDER_REMOTE_LIVENESS_AUTO_UPDATE 0x80       /* Auto update rmep on
                                                          timeout event even if
                                                          event FIFO is full. */

/*  TWAMP fault flags. */
#define BCM_TWAMP_SENDER_FAILURE_REMOTE_LIVENESS 0x00000001 /* Liveness failure for
                                                          TWAMP sender. */

/* TWAMP Defines */
#define BCM_TWAMP_MAX_TLV_NUM           2          /* Maximum supported number
                                                      of TLVs in a twamp packet. */
#define BCM_TWAMP_MAX_NUM_OF_DELAY_BINS 6          /* Maximum number of delay
                                                      histogram bins. */
#define BCM_TWAMP_MAX_TLV_VALUE_SIZE    16         /* Maximum array size of TLV
                                                      value. */

/* TWAMP Sender ID type. */
typedef int bcm_twamp_sender_t;

/* Types of TWAMP sender nodes */
typedef enum bcm_twamp_sender_type_e {
    bcmTwampSenderTypeTwamp = 0,    /* TWAMP DM */
    bcmTwampSenderTypeStamp = 1,    /* STAMP DM */
    bcmTwampSenderTypeTwampLM = 2,  /* TWAMP LM */
    bcmTwampSenderTypeStampLM = 3   /* STAMP LM */
} bcm_twamp_sender_type_t;

/* TWAMP TLV object. */
typedef struct bcm_twamp_tlv_s {
    uint8 tlv_flags;                    /* STAMP TLV Flags */
    uint8 tlv_type;                     /* STAMP TLV Type */
    uint32 tlv_value[BCM_TWAMP_MAX_TLV_VALUE_SIZE]; /* Raw TLV Value */
    uint16 tlv_length;                  /* STAMP TLV Length */
} bcm_twamp_tlv_t;

/* TWAMP Sender Node object. */
typedef struct bcm_twamp_sender_info_s {
    uint32 flags;                       /* BCM_TWAMP_SENDER_XXX */
    bcm_twamp_sender_t id;              /* TWAMP ID. */
    bcm_twamp_sender_type_t type;       /* TWAMP sender node type. */
    uint8 ip_tos;                       /* IPv4 Tos / IPv6 Traffic Class. */
    uint8 ip_ttl;                       /* IPv4 TTL / IPv6 Hop Count. */
    bcm_ip_t dst_ip_addr;               /* Destination IPv4 address. */
    bcm_ip_t src_ip_addr;               /* Destination IPv4 address. */
    bcm_ip6_t dst_ip6_addr;             /* Destination IPv6 address. */
    bcm_ip6_t src_ip6_addr;             /* Source IPv6 address. */
    uint32 udp_src_port;                /* UDP source port for Ipv4, Ipv6. */
    uint32 udp_dest_port;               /* UDP destination port for Ipv4, Ipv6. */
    uint16 session_id;                  /* STAMP Session Identifier */
    uint8 sender_control_code;          /* Sender Control Code. */
    uint8 block_number;                 /* Block Number on Loss Measurement
                                           Query Messages. */
    uint16 error_estimate;              /* TWAMP DM error estimate value. */
    bcm_cos_t int_pri;                  /* Egress queuing for outgoing TWAMP
                                           sender messages. */
    bcm_twamp_tlv_t tlv[BCM_TWAMP_MAX_TLV_NUM]; /* Associated STAMP TLVs. */
    int twamp_period;                   /* Transmission period in ms. */
    bcm_gport_t tx_gport;               /* TX gport associated with this node. */
    bcm_if_t egress_if;                 /* Egress interface. */
    bcm_twamp_sender_t remote_entry_id; /* ID of remote entry. */
    int lm_counter_base_id;             /* Counter id associated with the TWAMP
                                           LM session. */
    uint8 number_of_delay_bins;         /* Number of delay histogram bins. */
    bcm_time_spec_t expected_delay;     /* Expected frame delay for histogram. */
    uint8 histogram_bin_variance;       /* Histogram bin's size relative to
                                           expected delay in percentage. */
    uint8 liveness_failure_clear_threshold; /* Number of packets required to reset
                                           the liveness-failure status. */
    uint32 faults;                      /* Fault flags.
                                           BCM_TWAMP_SENDER_FAILURE_XXX */
    int missed_probe_message_count;     /* Number of missed packets until
                                           liveness failure is notified. */
} bcm_twamp_sender_info_t;

/* TWAMP DM statistics object. */
typedef struct bcm_twamp_sender_dm_statistics_s {
    bcm_time_spec_t last_near_end_delay; /* Two way frame delay of last received
                                           sender packet */
    bcm_time_spec_t last_far_end_delay; /* Two way frame delay of last received
                                           responder packet */
    bcm_time_spec_t max_near_end_delay; /* Maximum recorded near end delay value */
    bcm_time_spec_t max_far_end_delay;  /* Maximum recorded far end delay value */
    bcm_time_spec_t min_near_end_delay; /* Minimum recorded near end delay value */
    bcm_time_spec_t min_far_end_delay;  /* Minimum recorded far end delay value */
    bcm_time_spec_t acc_near_end_delay; /* Accumulated near end measurements */
    bcm_time_spec_t acc_far_end_delay;  /* Accumulated far end measurements */
    uint16 near_end_delay_histogram[BCM_TWAMP_MAX_NUM_OF_DELAY_BINS]; /* Array of number of packets received
                                           with near end delay in bin's range */
    uint16 far_end_delay_histogram[BCM_TWAMP_MAX_NUM_OF_DELAY_BINS]; /* Array of number of packets received
                                           with far end delay in bin's range */
    uint16 num_samples;                 /* Number of received response packets */
    uint32 sequence_number_tx;          /* Last transmitted packet's sequence
                                           number */
    uint32 sequence_number_rx;          /* Last received packet's sequence
                                           number */
} bcm_twamp_sender_dm_statistics_t;

/* TWAMP LM statistics object. */
typedef struct bcm_twamp_sender_lm_statistics_s {
    uint32 my_tx;               /* Last received sender-counter value */
    uint32 my_rx;               /* Local Counter value */
    uint32 peer_tx;             /* Last received transmit-counter value */
    uint32 peer_rx;             /* Last received receive-counter value */
    int last_lm_near;           /* Near end loss (expressed in 100th of
                                   percent). A value of -1 if not available. */
    int last_lm_far;            /* Far end loss (expressed in 100th of percent).
                                   A value of -1 if not available. */
    int acc_lm_far;             /* Near end accumulated loss. A value of -1 if
                                   not available. */
    int acc_lm_near;            /* Far end accumulated loss. A value of -1 if
                                   not available. */
    int max_lm_far;             /* Far end maximal loss. A value of -1 if not
                                   available. */
    int max_lm_near;            /* Near end maximal loss. A value of -1 if not
                                   available. */
    uint16 min_lm_far;          /* Far end minimal loss. A value of -1 if not
                                   available. */
    uint16 min_lm_near;         /* Near end minimal loss. A value of -1 if not
                                   available. */
    uint16 num_samples;         /* Number of received response packets */
    uint32 sequence_number_tx;  /* Last transmitted packet's sequence number */
    uint32 sequence_number_rx;  /* Last received packet's sequence number */
} bcm_twamp_sender_lm_statistics_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Create or replace a TWAMP sender object. */
extern int bcm_twamp_sender_create(
    int unit, 
    bcm_twamp_sender_info_t *twamp_sender_info);

/* Get an TWAMP sender object. */
extern int bcm_twamp_sender_get(
    int unit, 
    bcm_twamp_sender_t twamp_id, 
    bcm_twamp_sender_info_t *twamp_sender_info);

/* Get delay statistics of a TWAMP sender object. */
extern int bcm_twamp_sender_delay_get(
    int unit, 
    bcm_twamp_sender_t twamp_id, 
    bcm_twamp_sender_dm_statistics_t *twamp_sender_dm_statistics_info);

/* Get loss statistics of a TWAMP sender object. */
extern int bcm_twamp_sender_loss_get(
    int unit, 
    bcm_twamp_sender_t twamp_id, 
    bcm_twamp_sender_lm_statistics_t *twamp_sender_lm_statistics_info);

/* Destroy an TWAMP sender object. */
extern int bcm_twamp_sender_destroy(
    int unit, 
    bcm_twamp_sender_t twamp_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a bcm_twamp_sender_info_t structure. */
extern void bcm_twamp_sender_info_t_init(
    bcm_twamp_sender_info_t *twamp_sender_info);

/* Initialize a bcm_twamp_sender_lm_statistics_t structure. */
extern void bcm_twamp_sender_lm_statistics_t_init(
    bcm_twamp_sender_lm_statistics_t *twamp_sender_lm_statistics_info);

/* Initialize a bcm_twamp_sender_dm_statistics_t structure. */
extern void bcm_twamp_sender_dm_statistics_t_init(
    bcm_twamp_sender_dm_statistics_t *twamp_sender_dm_statistics_info);

/* Callback function type for TWAMP sender traversal */
typedef int (*bcm_twamp_sender_traverse_cb)(
    int unit, 
    bcm_twamp_sender_info_t *twamp_sender_info, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse the TWAMP sender nodes. */
extern int bcm_twamp_sender_traverse(
    int unit, 
    uint32 flags, 
    bcm_twamp_sender_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_TWAMP_H__ */
