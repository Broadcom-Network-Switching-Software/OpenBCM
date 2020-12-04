/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


/* Common place to put all the timesync IEEE1588 common utilities */

#ifndef BMACSEC_TIMESYNC_DEFS_H
#define BMACSEC_TIMESYNC_DEFS_H

#define BMACSEC_PORT_PHY_TIMESYNC_ETHERNET_CAPABLE                     (1U<<0)
#define BMACSEC_PORT_PHY_TIMESYNC_MPLS_CAPABLE                         (1U<<1)

#define BMACSEC_PORT_PHY_TIMESYNC_VALID_FLAGS                         (1U <<  0)
#define BMACSEC_PORT_PHY_TIMESYNC_VALID_ITPID                         (1U <<  1)
#define BMACSEC_PORT_PHY_TIMESYNC_VALID_OTPID                         (1U <<  2)
#define BMACSEC_PORT_PHY_TIMESYNC_VALID_OTPID2                        (1U <<  3)
#define BMACSEC_PORT_PHY_TIMESYNC_VALID_GMODE                         (1U <<  4)
#define BMACSEC_PORT_PHY_TIMESYNC_VALID_FRAMESYNC_MODE                (1U <<  5)
#define BMACSEC_PORT_PHY_TIMESYNC_VALID_SYNCOUT_MODE                  (1U <<  6)
#define BMACSEC_PORT_PHY_TIMESYNC_VALID_TS_DIVIDER                    (1U <<  7)
#define BMACSEC_PORT_PHY_TIMESYNC_VALID_ORIGINAL_TIMECODE             (1U <<  8)
#define BMACSEC_PORT_PHY_TIMESYNC_VALID_TX_TIMESTAMP_OFFSET           (1U <<  9)
#define BMACSEC_PORT_PHY_TIMESYNC_VALID_RX_TIMESTAMP_OFFSET           (1U << 10)
#define BMACSEC_PORT_PHY_TIMESYNC_VALID_TX_SYNC_MODE                  (1U << 11)
#define BMACSEC_PORT_PHY_TIMESYNC_VALID_TX_DELAY_REQUEST_MODE         (1U << 12)
#define BMACSEC_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_REQUEST_MODE        (1U << 13)
#define BMACSEC_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_RESPONSE_MODE       (1U << 14)
#define BMACSEC_PORT_PHY_TIMESYNC_VALID_RX_SYNC_MODE                  (1U << 15)
#define BMACSEC_PORT_PHY_TIMESYNC_VALID_RX_DELAY_REQUEST_MODE         (1U << 16)
#define BMACSEC_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_REQUEST_MODE        (1U << 17)
#define BMACSEC_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_RESPONSE_MODE       (1U << 18)
#define BMACSEC_PORT_PHY_TIMESYNC_VALID_MPLS_CONTROL                  (1U << 19)
#define BMACSEC_PORT_PHY_TIMESYNC_VALID_RX_LINK_DELAY                 (1U << 20)
#define BMACSEC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K1              (1U << 21)
#define BMACSEC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K2              (1U << 22)
#define BMACSEC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K3              (1U << 23)
#define BMACSEC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_LOOP_FILTER     (1U << 24)
#define BMACSEC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE       (1U << 25)
#define BMACSEC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE_DELTA (1U << 26)

#define BMACSEC_PORT_PHY_TIMESYNC_ENABLE                               (1U << 0)
#define BMACSEC_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE                    (1U << 1)
#define BMACSEC_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE                  (1U << 2)
#define BMACSEC_PORT_PHY_TIMESYNC_RX_CRC_ENABLE                        (1U << 3)
#define BMACSEC_PORT_PHY_TIMESYNC_8021AS_ENABLE                        (1U << 4)
#define BMACSEC_PORT_PHY_TIMESYNC_L2_ENABLE                            (1U << 5)
#define BMACSEC_PORT_PHY_TIMESYNC_IP4_ENABLE                           (1U << 6)
#define BMACSEC_PORT_PHY_TIMESYNC_IP6_ENABLE                           (1U << 7)
#define BMACSEC_PORT_PHY_TIMESYNC_CLOCK_SRC_EXT                        (1U << 8)  /* 0 - NCO FREQ CTRL REG, 1 - DPLL */
#define BMACSEC_PORT_PHY_TIMESYNC_CLOCK_EXT_FREQ_LOCK                  (1U << 9)  /* DPLL 0 - phase, 1 - freq lock */

/* Enables the Layer4 IP6 address check when 1588 detection in transmission mode  */
#define BMACSEC_PORT_PHY_TIMESYNC_L4_IPV6_ADDR_ENABLE                  (1U << 10) 
/* Enables the Layer4 IP address check when 1588 detection in transmission mode  */
#define BMACSEC_PORT_PHY_TIMESYNC_L4_IP_ADDR_ENABLE                    (1U << 11)
/* Enables the Layer2 MAC DS check when 1588 detection in transmission mode  */
#define BMACSEC_PORT_PHY_TIMESYNC_L2_DS_ENABLE                         (1U << 12)
/* Enables the 802.1as MAC DS check when 1588 detection in transmission mode  */
#define BMACSEC_PORT_PHY_TIMESYNC_AS_DS_ENABLE                         (1U << 13)



/* Base time structure. */
typedef struct bmacsec_time_spec_s {
    buint8_t isnegative;   /* Sign identifier. */
    buint64_t seconds;     /* Seconds absolute value. */
    buint32_t nanoseconds; /* Nanoseconds absolute value. */
} bmacsec_time_spec_t;

/* Actions on Egress event messages */
typedef enum bmacsec_port_phy_timesync_event_message_egress_mode_e {
    BMACSEC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_NONE,
    BMACSEC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_UPDATE_CORRECTIONFIELD,
    BMACSEC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_REPLACE_CORRECTIONFIELD_ORIGIN,
    BMACSEC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_CAPTURE_TIMESTAMP,
    BMACSEC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_RESERVED
} bmacsec_port_phy_timesync_event_message_egress_mode_t;

/* Actions on ingress event messages */
typedef enum bmacsec_port_phy_timesync_event_message_ingress_mode_e {
    BMACSEC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_NONE,
    BMACSEC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_UPDATE_CORRECTIONFIELD,
    BMACSEC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_INSERT_TIMESTAMP,
    BMACSEC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_INSERT_TIMESTAMP_TN,
    BMACSEC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_INSERT_DELAYTIME
} bmacsec_port_phy_timesync_event_message_ingress_mode_t;

/* Global mode actions */
typedef enum bmacsec_port_phy_timesync_global_mode_e {
    BMACSEC_PORT_PHY_TIMESYNC_MODE_FREE,
    BMACSEC_PORT_PHY_TIMESYNC_MODE_SYNCIN,
    BMACSEC_PORT_PHY_TIMESYNC_MODE_CPU
} bmacsec_port_phy_timesync_global_mode_t;

typedef enum bmacsec_port_phy_timesync_framesync_mode_s
{
  BMACSEC_PORT_PHY_TIMESYNC_FRAMESYNC_NONE,
  BMACSEC_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCIN0,
  BMACSEC_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCIN1,
  BMACSEC_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCOUT,
  BMACSEC_PORT_PHY_TIMESYNC_FRAMESYNC_CPU
} bmacsec_port_phy_timesync_framesync_mode_t;

typedef struct bmacsec_port_phy_framesync_s {
    bmacsec_port_phy_timesync_framesync_mode_t mode ;
    buint16_t length_threshold;
    buint16_t event_offset;
} bmacsec_port_phy_timesync_framesync_t;

typedef enum bmacsec_port_phy_timesync_syncout_mode_s
{
  BMACSEC_PORT_PHY_TIMESYNC_SYNCOUT_DISABLE,
  BMACSEC_PORT_PHY_TIMESYNC_SYNCOUT_ONE_TIME,
  BMACSEC_PORT_PHY_TIMESYNC_SYNCOUT_PULSE_TRAIN,
  BMACSEC_PORT_PHY_TIMESYNC_SYNCOUT_PULSE_TRAIN_WITH_SYNC
} bmacsec_port_phy_timesync_syncout_mode_t;

typedef struct bmacsec_port_phy_timesync_syncout_s {

    bmacsec_port_phy_timesync_syncout_mode_t mode;
    buint16_t pulse_1_length; /* in nanoseconds */
    buint16_t pulse_2_length; /* in nanoseconds */
    buint16_t interval; /* in nanoseconds */
    buint64_t syncout_ts;

} bmacsec_port_phy_timesync_syncout_t;

    /* mpls_label_flags */
#define BMACSEC_PORT_PHY_TIMESYNC_MPLS_LABEL_OUT                        (1U<<1)
#define BMACSEC_PORT_PHY_TIMESYNC_MPLS_LABEL_IN                         (1U<<0)

typedef struct bmacsec_port_phy_timesync_mpls_label_s {

    buint32_t value; /* bits [19:0] */
    buint32_t mask; /* bits [19:0] */
    buint32_t flags; /* label flags */

} bmacsec_port_phy_timesync_mpls_label_t;

    /* mpls_flags */
#define BMACSEC_PORT_PHY_TIMESYNC_MPLS_ENABLE                           (1U<<0)
#define BMACSEC_PORT_PHY_TIMESYNC_MPLS_ENTROPY_ENABLE                   (1U<<1)
#define BMACSEC_PORT_PHY_TIMESYNC_MPLS_CONTROL_WORD_ENABLE              (1U<<3)
#define BMACSEC_PORT_PHY_TIMESYNC_MPLS_SPECIAL_LABEL_ENABLE             (1U<<2)

typedef struct bmacsec_port_phy_timesync_mpls_control_s {

    buint32_t flags;
    buint32_t special_label; /* bits [19:0] */
    bmacsec_port_phy_timesync_mpls_label_t labels[10]; /* label */

} bmacsec_port_phy_timesync_mpls_control_t;


#define BMACSEC_PORT_PHY_TIMESYNC_TN_LOAD                               (1U<<23)
#define BMACSEC_PORT_PHY_TIMESYNC_TN_ALWAYS_LOAD                        (1U<<22)
#define BMACSEC_PORT_PHY_TIMESYNC_TIMECODE_LOAD                         (1U<<21)
#define BMACSEC_PORT_PHY_TIMESYNC_TIMECODE_ALWAYS_LOAD                  (1U<<20)
#define BMACSEC_PORT_PHY_TIMESYNC_SYNCOUT_LOAD                          (1U<<19)
#define BMACSEC_PORT_PHY_TIMESYNC_SYNCOUT_ALWAYS_LOAD                   (1U<<18)
#define BMACSEC_PORT_PHY_TIMESYNC_NCO_DIVIDER_LOAD                      (1U<<17)
#define BMACSEC_PORT_PHY_TIMESYNC_NCO_DIVIDER_ALWAYS_LOAD               (1U<<16)
#define BMACSEC_PORT_PHY_TIMESYNC_LOCAL_TIME_LOAD                       (1U<<15)
#define BMACSEC_PORT_PHY_TIMESYNC_LOCAL_TIME_ALWAYS_LOAD                (1U<<14)
#define BMACSEC_PORT_PHY_TIMESYNC_NCO_ADDEND_LOAD                       (1U<<13)
#define BMACSEC_PORT_PHY_TIMESYNC_NCO_ADDEND_ALWAYS_LOAD                (1U<<12)
#define BMACSEC_PORT_PHY_TIMESYNC_DPLL_LOOP_FILTER_LOAD                 (1U<<11)
#define BMACSEC_PORT_PHY_TIMESYNC_DPLL_LOOP_FILTER_ALWAYS_LOAD          (1U<<10)
#define BMACSEC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_LOAD                   (1U<<9)
#define BMACSEC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_ALWAYS_LOAD            (1U<<8)
#define BMACSEC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_LOAD             (1U<<7)
#define BMACSEC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_ALWAYS_LOAD      (1U<<6)
#define BMACSEC_PORT_PHY_TIMESYNC_DPLL_K3_LOAD                          (1U<<5)
#define BMACSEC_PORT_PHY_TIMESYNC_DPLL_K3_ALWAYS_LOAD                   (1U<<4)
#define BMACSEC_PORT_PHY_TIMESYNC_DPLL_K2_LOAD                          (1U<<3)
#define BMACSEC_PORT_PHY_TIMESYNC_DPLL_K2_ALWAYS_LOAD                   (1U<<2)
#define BMACSEC_PORT_PHY_TIMESYNC_DPLL_K1_LOAD                          (1U<<1)
#define BMACSEC_PORT_PHY_TIMESYNC_DPLL_K1_ALWAYS_LOAD                   (1U<<0)

#define BMACSEC_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT                   (1U<<1)
#define BMACSEC_PORT_PHY_TIMESYNC_FRAMESYNC_INTERRUPT                   (1U<<0)

#define BMACSEC_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_MASK              (1U<<1)
#define BMACSEC_PORT_PHY_TIMESYNC_FRAMESYNC_INTERRUPT_MASK              (1U<<0)


/* Fast call actions */
typedef enum bmacsec_port_phy_control_timesync_e {
    BMACSEC_PORT_CONTROL_PHY_TIMESYNC_CAPTURE_TIMESTAMP,
    BMACSEC_PORT_CONTROL_PHY_TIMESYNC_HEARTBEAT_TIMESTAMP,
    BMACSEC_PORT_CONTROL_PHY_TIMESYNC_NCOADDEND,
    BMACSEC_PORT_CONTROL_PHY_TIMESYNC_FRAMESYNC,
    BMACSEC_PORT_CONTROL_PHY_TIMESYNC_LOCAL_TIME,
    BMACSEC_PORT_CONTROL_PHY_TIMESYNC_LOAD_CONTROL,
    BMACSEC_PORT_CONTROL_PHY_TIMESYNC_INTERRUPT,
    BMACSEC_PORT_CONTROL_PHY_TIMESYNC_INTERRUPT_MASK,
    BMACSEC_PORT_CONTROL_PHY_TIMESYNC_TX_TIMESTAMP_OFFSET,
    BMACSEC_PORT_CONTROL_PHY_TIMESYNC_RX_TIMESTAMP_OFFSET
} bmacsec_port_phy_control_timesync_t;


/* Base timesync configuration type. */
typedef struct bmacsec_port_phy_timesync_config_s {
    buint32_t validity_mask;
    buint32_t flags;                       /* Flags BMACSEC_PORT_TIMESYNC_* */
    buint16_t itpid;                       /* 1588 inner tag */
    buint16_t otpid;                       /* 1588 outer tag */
    buint16_t otpid2;                      /* 1588 outer tag 2 */
    bmacsec_port_phy_timesync_global_mode_t gmode; /* Global mode */
    bmacsec_port_phy_timesync_framesync_t framesync;
    bmacsec_port_phy_timesync_syncout_t syncout;
    buint16_t ts_divider;

    bmacsec_time_spec_t original_timecode;  /* Original timecode to be inserted */
    buint32_t tx_timestamp_offset;         /* TX AFE delay in ns - per port */
    buint32_t rx_timestamp_offset;         /* RX AFE delay in ns - per port */
    buint32_t rx_link_delay;

    bmacsec_port_phy_timesync_event_message_egress_mode_t tx_sync_mode; /* sync */
    bmacsec_port_phy_timesync_event_message_egress_mode_t tx_delay_request_mode; /* delay request */
    bmacsec_port_phy_timesync_event_message_egress_mode_t tx_pdelay_request_mode; /* pdelay request */
    bmacsec_port_phy_timesync_event_message_egress_mode_t tx_pdelay_response_mode; /* pdelay response */
    bmacsec_port_phy_timesync_event_message_ingress_mode_t rx_sync_mode; /* sync */
    bmacsec_port_phy_timesync_event_message_ingress_mode_t rx_delay_request_mode; /* delay request */
    bmacsec_port_phy_timesync_event_message_ingress_mode_t rx_pdelay_request_mode; /* pdelay request */
    bmacsec_port_phy_timesync_event_message_ingress_mode_t rx_pdelay_response_mode; /* pdelay response */

    bmacsec_port_phy_timesync_mpls_control_t mpls_control;

    buint16_t phy_1588_dpll_k1;
    buint16_t phy_1588_dpll_k2;
    buint16_t phy_1588_dpll_k3;
    buint64_t phy_1588_dpll_loop_filter; /* loop filter */
    buint64_t phy_1588_dpll_ref_phase;   /* ref phase */
    buint32_t phy_1588_dpll_ref_phase_delta;   /* ref phase delta */
} bmacsec_port_phy_timesync_config_t;

/* bmacsec_port_phy_timesync_config_set */
extern int 
bmacsec_port_phy_timesync_config_set(
    bmacsec_dev_addr_t phy_id, 
    bmacsec_port_phy_timesync_config_t *conf);

/* bmacsec_port_phy_timesync_config_get */
extern int 
bmacsec_port_phy_timesync_config_get(
    bmacsec_dev_addr_t phy_id, 
    bmacsec_port_phy_timesync_config_t *conf);

/* bmacsec_port_phy_control_timesync_set */
extern int 
bmacsec_port_phy_control_timesync_set(
    bmacsec_dev_addr_t phy_id, 
    bmacsec_port_phy_control_timesync_t type, 
    buint64_t value);

/* bmacsec_port_phy_control_timesync_get */
extern int 
bmacsec_port_phy_control_timesync_get(
    bmacsec_dev_addr_t phy_id, 
    bmacsec_port_phy_control_timesync_t type, 
    buint64_t *value);

#endif /* BMACSEC_TIMESYNC_DEFS_H */
