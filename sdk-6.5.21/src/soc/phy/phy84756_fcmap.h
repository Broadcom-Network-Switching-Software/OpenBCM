/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef   _bfcmap_phy84756_H_
#define   _bfcmap_phy84756_H_


#define BFCMAP_PHY84756_BRCM_OUI3   0x1be9        /* Broadcom OUI */
#define BFCMAP_PHY84756_BRCM_OUI1   0x1018        /* Broadcom OUI */

/*
 * Supported PHYs
 */

#if 0
#define BFCMAP_PHY_BCM84756_MODEL          0x3d
#endif
#define BFCMAP_PHY_BCM84756_MODEL          0x27

typedef enum bfcmap_phy84756_speed_s {
    BFCMAP_PHY84756_SPEED_10 = 0,
    BFCMAP_PHY84756_SPEED_100,
    BFCMAP_PHY84756_SPEED_1000,
    BFCMAP_PHY84756_SPEED_10000
}bfcmap_phy84756_speed_t;

typedef enum bfcmap_phy84756_duplex_s {
    BFCMAP_PHY84756_HALF_DUPLEX = 0,
    BFCMAP_PHY84756_FULL_DUPLEX
}bfcmap_phy84756_duplex_t;

typedef enum bfcmap_phy84756_intf_s {
    BFCMAP_PHY84756_INTF_SGMII = 0,
    BFCMAP_PHY84756_INTF_1000X,
    BFCMAP_PHY84756_INTF_SFI,
    BFCMAP_PHY84756_INTF_XFI,
    BFCMAP_PHY84756_INTF_XAUI
}bfcmap_phy84756_intf_t;

/*
 * Switch MAC speed and duplex policies
 */
typedef enum bfcmap_phy84756_mac_policy_s {
    BFCMAP_BCM84756_SWITCH_FIXED = 0,
    BFCMAP_BCM84756_SWITCH_FOLLOWS_LINE
}bfcmap_phy84756_mac_policy_t;

 
typedef enum bfcmap_phy84756_port_mdix_s {
    BFCMAP_PHY84756_PORT_MDIX_AUTO,
    BFCMAP_PHY84756_PORT_MDIX_FORCE_AUTO,
    BFCMAP_PHY84756_PORT_MDIX_NORMAL,
    BFCMAP_PHY84756_PORT_MDIX_XOVER
}bfcmap_phy84756_port_mdix_t;

typedef enum bfcmap_phy84756_port_mdix_status_s {
    BFCMAP_PHY84756_PORT_MDIX_STATUS_NORMAL,
    BFCMAP_PHY84756_PORT_MDIX_STATUS_XOVER
}bfcmap_phy84756_port_mdix_status_t;


typedef enum bfcmap_phy84756_master_s {
    BFCMAP_PHY84756_PORT_MS_SLAVE =0,
    BFCMAP_PHY84756_PORT_MS_MASTER,
    BFCMAP_PHY84756_PORT_MS_AUTO,
    BFCMAP_PHY84756_PORT_MS_NONE
}bfcmap_phy84756_master_t;

typedef enum bfcmap_phy84756_fcmap_dir_s {
    BFCMAP_PHY84756_FCMAP_DIR_EGRESS = 0,
    BFCMAP_PHY84756_FCMAP_DIR_INGRESS
} bfcmap_phy84756_fcmap_dir_t;

typedef enum bfcmap_phy84756_fcmap_port_mode_s {
    BFCMAP_PHY84756_FCMAP_FCOE_TO_FC_MODE = 0,
    BFCMAP_PHY84756_FCMAP_FCOE_TO_FCOE_MODE
} bfcmap_phy84756_fcmap_port_mode_t;

typedef enum bfcmap_phy84756_fcmap_port_speed_s {
    BFCMAP_PHY84756_FCMAP_PORT_SPEED_2GBPS = 0,
    BFCMAP_PHY84756_FCMAP_PORT_SPEED_4GBPS = 1,
    BFCMAP_PHY84756_FCMAP_PORT_SPEED_8GBPS = 2,
    BFCMAP_PHY84756_FCMAP_PORT_SPEED_16GBPS = 3,
    BFCMAP_PHY84756_FCMAP_PORT_SPEED_MAX_COUNT
} bfcmap_phy84756_fcmap_port_speed_t;

typedef enum bfcmap_phy84756_fcmap_port_state_s {
    BFCMAP_PHY84756_FCMAP_PORT_STATE_INIT = 0,
    BFCMAP_PHY84756_FCMAP_PORT_STATE_RESET = 1,
    BFCMAP_PHY84756_FCMAP_PORT_STATE_ACTIVE = 2,
    BFCMAP_PHY84756_FCMAP_PORT_STATE_LINKDOWN = 3,
    BFCMAP_PHY84756_FCMAP_PORT_STATE_DISABLE = 4,
    BFCMAP_PHY84756_FCMAP_PORT_STATE_MAX_COUNT
} bfcmap_phy84756_fcmap_port_state_t;

typedef unsigned int bfcmap_phy84756_port_mode_t;
typedef unsigned int bfcmap_phy84756_port_ability_t;

/*
 * Port modes.
 */

#define BFCMAP_PHY84756_PM_10MB_HD         (1 << 0)
#define BFCMAP_PHY84756_PM_10MB_FD         (1 << 1)
#define BFCMAP_PHY84756_PM_100MB_HD        (1 << 2)
#define BFCMAP_PHY84756_PM_100MB_FD        (1 << 3)
#define BFCMAP_PHY84756_PM_1000MB_HD       (1 << 4)
#define BFCMAP_PHY84756_PM_1000MB_FD       (1 << 5)
#define BFCMAP_PHY84756_PM_PAUSE_TX        (1 << 6)
#define BFCMAP_PHY84756_PM_PAUSE_RX        (1 << 7)
#define BFCMAP_PHY84756_PM_PAUSE_ASYMM     (1 << 8)
#define BFCMAP_PHY84756_PM_TBI             (1 << 9)
#define BFCMAP_PHY84756_PM_MII             (1 << 10)
#define BFCMAP_PHY84756_PM_GMII            (1 << 11)
#define BFCMAP_PHY84756_PM_SGMII           (1 << 12)
#define BFCMAP_PHY84756_PM_LB_MAC          (1 << 13)
#define BFCMAP_PHY84756_PM_LB_NONE         (1 << 14)
#define BFCMAP_PHY84756_PM_LB_PHY          (1 << 15)
#define BFCMAP_PHY84756_PM_AN              (1 << 16)

#define BFCMAP_PHY84756_PM_PAUSE           (BFCMAP_PHY84756_PM_PAUSE_TX  | \
                                            BFCMAP_PHY84756_PM_PAUSE_RX)
#define BFCMAP_PHY84756_PM_10MB            (BFCMAP_PHY84756_PM_10MB_HD   | \
                                            BFCMAP_PHY84756_PM_10MB_FD)
#define BFCMAP_PHY84756_PM_100MB           (BFCMAP_PHY84756_PM_100MB_HD  | \
                                            BFCMAP_PHY84756_PM_100MB_FD)
#define BFCMAP_PHY84756_PM_1000MB          (BFCMAP_PHY84756_PM_1000MB_HD | \
                                            BFCMAP_PHY84756_PM_1000MB_FD)

#define BFCMAP_PHY84756_PM_SPEED_ALL       (BFCMAP_PHY84756_PM_1000MB |    \
                                            BFCMAP_PHY84756_PM_100MB |     \
                                            BFCMAP_PHY84756_PM_10MB)

#define BFCMAP_PHY84756_PM_FD              (BFCMAP_PHY84756_PM_1000MB_FD |   \
                                            BFCMAP_PHY84756_PM_100MB_FD  |   \
                                            BFCMAP_PHY84756_PM_10MB_FD)

#define BFCMAP_PHY84756_PM_HD              (BFCMAP_PHY84756_PM_1000MB_HD |   \
                                            BFCMAP_PHY84756_PM_100MB_HD  |   \
                                            BFCMAP_PHY84756_PM_10MB_HD)

/*
 * Port ability
 */
#define BFCMAP_PHY84756_PA_10MB_HD         (1 << 0)
#define BFCMAP_PHY84756_PA_10MB_FD         (1 << 1)
#define BFCMAP_PHY84756_PA_100MB_HD        (1 << 2)
#define BFCMAP_PHY84756_PA_100MB_FD        (1 << 3)
#define BFCMAP_PHY84756_PA_1000MB_HD       (1 << 4)
#define BFCMAP_PHY84756_PA_1000MB_FD       (1 << 5)
#define BFCMAP_PHY84756_PA_10000MB_FD      (1 << 6)
#define BFCMAP_PHY84756_PA_PAUSE_TX        (1 << 7)
#define BFCMAP_PHY84756_PA_PAUSE_RX        (1 << 8)
#define BFCMAP_PHY84756_PA_PAUSE_ASYMM     (1 << 9)
#define BFCMAP_PHY84756_PA_SGMII           (1 << 10)
#define BFCMAP_PHY84756_PA_XSGMII          (1 << 11)
#define BFCMAP_PHY84756_PA_LB_MAC          (1 << 12)
#define BFCMAP_PHY84756_PA_LB_NONE         (1 << 13)
#define BFCMAP_PHY84756_PA_LB_PHY          (1 << 14)
#define BFCMAP_PHY84756_PA_AN              (1 << 15)

#define BFCMAP_PHY84756_PA_PAUSE      (BFCMAP_PHY84756_PA_PAUSE_TX  | \
                                       BFCMAP_PHY84756_PA_PAUSE_RX)
#define BFCMAP_PHY84756_PA_10MB       (BFCMAP_PHY84756_PA_10MB_HD   | \
                                       BFCMAP_PHY84756_PA_10MB_FD)
#define BFCMAP_PHY84756_PA_100MB      (BFCMAP_PHY84756_PA_100MB_HD  | \
                                       BFCMAP_PHY84756_PA_100MB_FD)
#define BFCMAP_PHY84756_PA_1000MB     (BFCMAP_PHY84756_PA_1000MB_HD | \
                                       BFCMAP_PHY84756_PA_1000MB_FD)

#define BFCMAP_PHY84756_PA_SPEED_ALL  (BFCMAP_PHY84756_PA_1000MB |    \
                                       BFCMAP_PHY84756_PA_100MB |     \
                                       BFCMAP_PHY84756_PA_10MB)

#define BFCMAP_PHY84756_PA_FD         (BFCMAP_PA_PHY84756_1000MB_FD | \
                                       BFCMAP_PHY84756_PA_100MB_FD  | \
                                       BFCMAPPHY84756__PA_10MB_FD)

#define BFCMAP_PHY84756_PA_HD         (BFCMAP_PHY84756_PA_1000MB_HD | \
                                       BFCMAP_PHY84756_PA_100MB_HD |  \
                                       BFCMAP_PHY84756_PA_10MB_HD)

#define BFCMAP_PHY84756_PA_FCMAP                  (1 << 0)
#define BFCMAP_PHY84756_PA_FCMAP_FCMAC_LOOPBACK   (1 << 1)
#define BFCMAP_PHY84756_PA_FCMAP_AUTONEG          (1 << 2)
#define BFCMAP_PHY84756_PA_FCMAP_2GB              (1 << 3)
#define BFCMAP_PHY84756_PA_FCMAP_4GB              (1 << 4)
#define BFCMAP_PHY84756_PA_FCMAP_8GB              (1 << 5)
#define BFCMAP_PHY84756_PA_FCMAP_16GB             (1 << 6)

/*
 * Port mode
 */
#define BFCMAP_PHY84756_MODE_FCMAP         (1 << 0)

/* 
 * Standalone PHY driver functions 
 */
extern int
bfcmap_phy84756_phy_mac_driver_attach(phy_ctrl_t *pc,
                                 blmi_dev_addr_t fcmap_dev_addr,
                                 int dev_port, blmi_dev_io_f mmi_cbf);
extern int
bfcmap_phy84756_phy_mac_driver_detach(phy_ctrl_t *pc);


/* Init PHY with MDIO address phy_id */
extern int 
bfcmap_phy84756_init(phy_ctrl_t     *pc, buint8_t reset, int line_mode, 
                     int fcmap_enable, int phy_ext_boot);

/* Reset PHY with MDIO address phy_id */
extern int 
bfcmap_phy84756_reset(phy_ctrl_t     *pc);

/* Post Reset PHY setup with MDIO address phy_id */
extern int
bfcmap_phy84756_no_reset_setup(phy_ctrl_t     *pc, int line_mode,
                               int phy_ext_boot);

/* Enable/Disable PHY */
extern int
bfcmap_phy84756_enable_set(phy_ctrl_t     *pc, int enable);

/* Set Speed of PHY and (Line and Switch) UNIMACs */
extern int
bfcmap_phy84756_speed_set(phy_ctrl_t     *pc, int speed);

/* Get Speed of PHY and (Line and Switch) UNIMACs */
extern int
bfcmap_phy84756_speed_get(phy_ctrl_t     *pc, int *speed);

/* Set duplex of PHY and (Line and Switch) UNIMACs */
extern int
bfcmap_phy84756_duplex_set(phy_ctrl_t     *pc, bfcmap_phy84756_duplex_t duplex);

/* Get duplex of PHY and (Line and Switch) UNIMACs */
extern int
bfcmap_phy84756_duplex_get(phy_ctrl_t *pc, bfcmap_phy84756_duplex_t *duplex);

/* Set AUTO negotiation Attributes */
extern int
bfcmap_phy84756_an_set(phy_ctrl_t *pc, int an, int autoneg_advert);

/* Get AUTO negotiation Attributes */
extern int
bfcmap_phy84756_an_get(phy_ctrl_t *pc, int *an, int *an_done);

/* Set MDIX */
extern int
bfcmap_phy84756_mdix_set(phy_ctrl_t *pc, bfcmap_phy84756_port_mdix_t mdix_mode);

/* MDIX Status Get */
extern int
bfcmap_phy84756_mdix_status_get(phy_ctrl_t *pc, 
                                bfcmap_phy84756_port_mdix_status_t *status);
/* Set Loopback */
extern int
bfcmap_phy84756_loopback_set(phy_ctrl_t     *pc, int loopback);

/* Get loopback settings of the PHY */
extern int
bfcmap_phy84756_loopback_get(phy_ctrl_t     *pc, int *loopback);

/* Set master */
extern int
bfcmap_phy84756_master_set(phy_ctrl_t     *pc, bfcmap_phy84756_master_t master);

/* Get Master */
extern int
bfcmap_phy84756_master_get(phy_ctrl_t     *pc, int *master);

/* Set local autoneg advertising parameters */
extern int
bfcmap_phy84756_ability_advert_set(phy_ctrl_t     *pc, 
                               bfcmap_phy84756_port_ability_t ability);

/* Get local autoneg advertising parameters */
extern int
bfcmap_phy84756_ability_advert_get(phy_ctrl_t     *pc, 
                               bfcmap_phy84756_port_ability_t *ability);

/* Get Remote autoneg advertising parameters */
extern int
bfcmap_phy84756_remote_ability_advert_get(phy_ctrl_t     *pc, 
                                bfcmap_phy84756_port_ability_t *ability);

/* Get Link status */
extern int
bfcmap_phy84756_link_get(phy_ctrl_t     *pc, int *link);

/* Get Link fault status */
extern int
bfcmap_phy84756_linkfault_get(phy_ctrl_t     *pc, int *fault);

/* Get Link fault trigger and reason code */
extern int
bfcmap_phy84756_linkfault_trigger_rc_get(phy_ctrl_t *pc, uint32 *lf_trigger, uint32 *lf_rc);


/* Gets the Ability of the PHY */
extern int
bfcmap_phy84756_ability_local_get(phy_ctrl_t     *pc, 
                             bfcmap_phy84756_port_ability_t *ability);

/* Download the firmware */
extern int
bfcmap_phy84756_mdio_firmware_download(phy_ctrl_t     *pc,
                                  buint8_t *new_fw, buint32_t fw_length);
/* Firmware Update */
extern int
bfcmap_phy84756_spi_firmware_update(phy_ctrl_t     *pc,
                                  buint8_t *array, buint32_t datalen);
/* Get Interface of side */
extern int
bfcmap_phy84756_line_intf_get(phy_ctrl_t     *pc, int dev_port, 
                              bfcmap_phy84756_intf_t *mode);
/* Set Interface of side */
extern int
bfcmap_phy84756_line_intf_set(phy_ctrl_t     *pc, int dev_port, 
                              bfcmap_phy84756_intf_t mode);


/* 1588 related */

#define BFCMAP_PORT_PHY_TIMESYNC_ETHERNET_CAPABLE                     (1U<<0)
#define BFCMAP_PORT_PHY_TIMESYNC_MPLS_CAPABLE                         (1U<<1)
#define BFCMAP_PORT_PHY_TIMESYNC_BEH_CAPABLE                          (1U<<2)

extern int 
bfcmap_port_phy_timesync_capability_get(blmi_dev_addr_t phy_id, buint32_t *cap);

#define BFCMAP_PORT_PHY_TIMESYNC_VALID_FLAGS                         (1U <<  0)
#define BFCMAP_PORT_PHY_TIMESYNC_VALID_ITPID                         (1U <<  1)
#define BFCMAP_PORT_PHY_TIMESYNC_VALID_OTPID                         (1U <<  2)
#define BFCMAP_PORT_PHY_TIMESYNC_VALID_OTPID2                        (1U <<  3)
#define BFCMAP_PORT_PHY_TIMESYNC_VALID_GMODE                         (1U <<  4)
#define BFCMAP_PORT_PHY_TIMESYNC_VALID_FRAMESYNC_MODE                (1U <<  5)
#define BFCMAP_PORT_PHY_TIMESYNC_VALID_SYNCOUT_MODE                  (1U <<  6)
#define BFCMAP_PORT_PHY_TIMESYNC_VALID_TS_DIVIDER                    (1U <<  7)
#define BFCMAP_PORT_PHY_TIMESYNC_VALID_ORIGINAL_TIMECODE             (1U <<  8)
#define BFCMAP_PORT_PHY_TIMESYNC_VALID_TX_TIMESTAMP_OFFSET           (1U <<  9)
#define BFCMAP_PORT_PHY_TIMESYNC_VALID_RX_TIMESTAMP_OFFSET           (1U << 10)
#define BFCMAP_PORT_PHY_TIMESYNC_VALID_TX_SYNC_MODE                  (1U << 11)
#define BFCMAP_PORT_PHY_TIMESYNC_VALID_TX_DELAY_REQUEST_MODE         (1U << 12)
#define BFCMAP_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_REQUEST_MODE        (1U << 13)
#define BFCMAP_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_RESPONSE_MODE       (1U << 14)
#define BFCMAP_PORT_PHY_TIMESYNC_VALID_RX_SYNC_MODE                  (1U << 15)
#define BFCMAP_PORT_PHY_TIMESYNC_VALID_RX_DELAY_REQUEST_MODE         (1U << 16)
#define BFCMAP_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_REQUEST_MODE        (1U << 17)
#define BFCMAP_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_RESPONSE_MODE       (1U << 18)
#define BFCMAP_PORT_PHY_TIMESYNC_VALID_MPLS_CONTROL                  (1U << 19)
#define BFCMAP_PORT_PHY_TIMESYNC_VALID_RX_LINK_DELAY                 (1U << 20)
#define BFCMAP_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K1              (1U << 21)
#define BFCMAP_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K2              (1U << 22)
#define BFCMAP_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K3              (1U << 23)
#define BFCMAP_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_LOOP_FILTER     (1U << 24)
#define BFCMAP_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE       (1U << 25)
#define BFCMAP_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE_DELTA (1U << 26)

#define BFCMAP_PORT_PHY_TIMESYNC_ENABLE                               (1U<<0)
#define BFCMAP_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE                    (1U<<1) /* per port */
#define BFCMAP_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE                  (1U<<2)
#define BFCMAP_PORT_PHY_TIMESYNC_RX_CRC_ENABLE                        (1U<<3)
#define BFCMAP_PORT_PHY_TIMESYNC_8021AS_ENABLE                        (1U<<4)
#define BFCMAP_PORT_PHY_TIMESYNC_L2_ENABLE                            (1U<<5)
#define BFCMAP_PORT_PHY_TIMESYNC_IP4_ENABLE                           (1U<<6)
#define BFCMAP_PORT_PHY_TIMESYNC_IP6_ENABLE                           (1U<<7)
#define BFCMAP_PORT_PHY_TIMESYNC_CLOCK_SRC_EXT                        (1U<<8) /* 0 - NCO FREQ CTRL REG, 1 - DPLL */
#define BFCMAP_PORT_PHY_TIMESYNC_CLOCK_SRC_EXT_MODE                   (1U<<9) /* DPLL 0 - phase, 1 - freq lock */

/* Base time structure. */
typedef struct bfcmap_time_spec_s {
    buint8_t isnegative;   /* Sign identifier. */
    buint64_t seconds;     /* Seconds absolute value. */
    buint32_t nanoseconds; /* Nanoseconds absolute value. */
} bfcmap_time_spec_t;

/* Actions on Egress event messages */
typedef enum bfcmap_port_phy_timesync_event_message_egress_mode_e {
    BFCMAP_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_NONE,
    BFCMAP_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_UPDATE_CORRECTIONFIELD,
    BFCMAP_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_REPLACE_CORRECTIONFIELD_ORIGIN,
    BFCMAP_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_CAPTURE_TIMESTAMP
} bfcmap_port_phy_timesync_event_message_egress_mode_t;

/* Actions on ingress event messages */
typedef enum bfcmap_port_phy_timesync_event_message_ingress_mode_e {
    BFCMAP_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_NONE,
    BFCMAP_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_UPDATE_CORRECTIONFIELD,
    BFCMAP_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_INSERT_TIMESTAMP,
    BFCMAP_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_INSERT_DELAYTIME
} bfcmap_port_phy_timesync_event_message_ingress_mode_t;

/* Global mode actions */
typedef enum bfcmap_port_phy_timesync_global_mode_e {
    BFCMAP_PORT_PHY_TIMESYNC_MODE_FREE,
    BFCMAP_PORT_PHY_TIMESYNC_MODE_SYNCIN,
    BFCMAP_PORT_PHY_TIMESYNC_MODE_CPU
} bfcmap_port_phy_timesync_global_mode_t;

/* framesync mode */
typedef enum bfcmap_port_phy_timesync_framesync_mode_e {
    BFCMAP_PORT_PHY_TIMESYNC_FRAMESYNC_NONE, /* Do nothing */
    BFCMAP_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCIN0, /* Use syncin as the source */
    BFCMAP_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCIN1, /* CPU based framesync */
    BFCMAP_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCOUT, /* CPU based framesync */
    BFCMAP_PORT_PHY_TIMESYNC_FRAMESYNC_CPU /* CPU based framesync */
} bfcmap_port_phy_timesync_framesync_mode_t;

/* framesync configuration type. */
typedef struct bfcmap_port_phy_timesync_framesync_s {
    bfcmap_port_phy_timesync_framesync_mode_t mode; /* framesync mode */
    buint16_t length_threshold;            /* syncin threshold for framesync */
    buint16_t event_offset;                /* event offset for framesync */
} bfcmap_port_phy_timesync_framesync_t;

typedef enum bfcmap_port_phy_timesync_syncout_mode_s
{
    BFCMAP_PORT_PHY_TIMESYNC_SYNCOUT_DISABLE,
    BFCMAP_PORT_PHY_TIMESYNC_SYNCOUT_ONE_TIME,
    BFCMAP_PORT_PHY_TIMESYNC_SYNCOUT_PULSE_TRAIN,
    BFCMAP_PORT_PHY_TIMESYNC_SYNCOUT_PULSE_TRAIN_WITH_SYNC
} bfcmap_port_phy_timesync_syncout_mode_t;

typedef struct bfcmap_port_phy_timesync_syncout_s {

    bfcmap_port_phy_timesync_syncout_mode_t mode;
    buint16_t pulse_1_length; /* in nanoseconds */
    buint16_t pulse_2_length; /* in nanoseconds */
    buint16_t interval; /* in nanoseconds */
    buint64_t syncout_ts;

} bfcmap_port_phy_timesync_syncout_t;

    /* mpls_label_flags */
#define BFCMAP_PORT_PHY_TIMESYNC_MPLS_LABEL_OUT                        (1U<<1)
#define BFCMAP_PORT_PHY_TIMESYNC_MPLS_LABEL_IN                         (1U<<0)

typedef struct bfcmap_port_phy_timesync_mpls_label_s {

    buint32_t label_value; /* bits [19:0] */
    buint32_t label_mask; /* bits [19:0] */
    buint32_t label_flags; /* label flags */

} bfcmap_port_phy_timesync_mpls_label_t;

    /* mpls_flags */
#define BFCMAP_PORT_PHY_TIMESYNC_MPLS_CONTROL_WORD_ENABLE              (1U<<3)
#define BFCMAP_PORT_PHY_TIMESYNC_MPLS_SPECIAL_LABEL_ENABLE             (1U<<2)
#define BFCMAP_PORT_PHY_TIMESYNC_MPLS_ENTROPY_ENABLE                   (1U<<1)
#define BFCMAP_PORT_PHY_TIMESYNC_MPLS_ENABLE                           (1U<<0)

typedef struct bfcmap_port_phy_timesync_mpls_control_s {

    buint32_t flags;
    buint32_t special_label; /* bits [19:0] */
    bfcmap_port_phy_timesync_mpls_label_t labels[10]; /* label */

} bfcmap_port_phy_timesync_mpls_control_t;

/* Base timesync configuration type. */
typedef struct bfcmap_port_phy_timesync_config_s {
    buint32_t validity_mask;               /* Flags BCM_PORT_PHY_TIMESYNC_VALID_* */
    buint32_t flags;                       /* Flags BFCMAP_PORT_TIMESYNC_* */
    buint16_t itpid;                       /* 1588 inner tag */
    buint16_t otpid;                       /* 1588 outer tag */
    buint16_t otpid2;                      /* 1588 outer tag 2 */
    bfcmap_port_phy_timesync_global_mode_t gmode; /* Global mode */
    bfcmap_port_phy_timesync_framesync_t framesync;
    bfcmap_port_phy_timesync_syncout_t syncout;
    buint16_t ts_divider;                  /* TS divider */
    bfcmap_time_spec_t original_timecode;  /* Original timecode to be inserted */
    buint32_t tx_timestamp_offset;         /* TX AFE delay in ns - per port */
    buint32_t rx_timestamp_offset;         /* RX AFE delay in ns - per port */
    buint32_t rx_link_delay;               /* RX link delay */
    bfcmap_port_phy_timesync_event_message_egress_mode_t tx_sync_mode; /* sync */
    bfcmap_port_phy_timesync_event_message_egress_mode_t tx_delay_request_mode; /* delay request */
    bfcmap_port_phy_timesync_event_message_egress_mode_t tx_pdelay_request_mode; /* pdelay request */
    bfcmap_port_phy_timesync_event_message_egress_mode_t tx_pdelay_response_mode; /* pdelay response */
    bfcmap_port_phy_timesync_event_message_ingress_mode_t rx_sync_mode; /* sync */
    bfcmap_port_phy_timesync_event_message_ingress_mode_t rx_delay_request_mode; /* delay request */
    bfcmap_port_phy_timesync_event_message_ingress_mode_t rx_pdelay_request_mode; /* pdelay request */
    bfcmap_port_phy_timesync_event_message_ingress_mode_t rx_pdelay_response_mode; /* pdelay response */

    bfcmap_port_phy_timesync_mpls_control_t mpls_control;

    buint16_t phy_1588_dpll_k1;
    buint16_t phy_1588_dpll_k2;
    buint16_t phy_1588_dpll_k3;
    buint64_t phy_1588_dpll_loop_filter;   /* DPLL loop filter */
    buint64_t phy_1588_dpll_ref_phase;     /* DPLL ref phase */
    buint32_t phy_1588_dpll_ref_phase_delta; /* DPLL ref phase delta */

} bfcmap_port_phy_timesync_config_t;

/* bfcmap_port_phy_timesync_config_set */
extern int 
bfcmap_port_phy_timesync_config_set(
    blmi_dev_addr_t phy_id, 
    bfcmap_port_phy_timesync_config_t *conf);

/* bfcmap_port_phy_timesync_config_get */
extern int 
bfcmap_port_phy_timesync_config_get(
    blmi_dev_addr_t phy_id, 
    bfcmap_port_phy_timesync_config_t *conf);

/* Fast call actions */
typedef enum bfcmap_port_phy_control_timesync_e {
    BFCMAP_PORT_CONTROL_PHY_TIMESYNC_CAPTURE_TIMESTAMP,
    BFCMAP_PORT_CONTROL_PHY_TIMESYNC_HEARTBEAT_TIMESTAMP,
    BFCMAP_PORT_CONTROL_PHY_TIMESYNC_NCOADDEND,
    BFCMAP_PORT_CONTROL_PHY_TIMESYNC_FRAMESYNC,
    BFCMAP_PORT_CONTROL_PHY_TIMESYNC_LOCAL_TIME,
    BFCMAP_PORT_CONTROL_PHY_TIMESYNC_LOAD_CONTROL,
    BFCMAP_PORT_CONTROL_PHY_TIMESYNC_INTERRUPT,
    BFCMAP_PORT_CONTROL_PHY_TIMESYNC_INTERRUPT_MASK,
    BFCMAP_PORT_CONTROL_PHY_TIMESYNC_TX_TIMESTAMP_OFFSET,
    BFCMAP_PORT_CONTROL_PHY_TIMESYNC_RX_TIMESTAMP_OFFSET
} bfcmap_port_control_phy_timesync_t;

#define BFCMAP_PORT_PHY_TIMESYNC_TN_LOAD                               (1U<<23)
#define BFCMAP_PORT_PHY_TIMESYNC_TN_ALWAYS_LOAD                        (1U<<22)
#define BFCMAP_PORT_PHY_TIMESYNC_TIMECODE_LOAD                         (1U<<21)
#define BFCMAP_PORT_PHY_TIMESYNC_TIMECODE_ALWAYS_LOAD                  (1U<<20)
#define BFCMAP_PORT_PHY_TIMESYNC_SYNCOUT_LOAD                          (1U<<19)
#define BFCMAP_PORT_PHY_TIMESYNC_SYNCOUT_ALWAYS_LOAD                   (1U<<18)
#define BFCMAP_PORT_PHY_TIMESYNC_NCO_DIVIDER_LOAD                      (1U<<17)
#define BFCMAP_PORT_PHY_TIMESYNC_NCO_DIVIDER_ALWAYS_LOAD               (1U<<16)
#define BFCMAP_PORT_PHY_TIMESYNC_LOCAL_TIME_LOAD                       (1U<<15)
#define BFCMAP_PORT_PHY_TIMESYNC_LOCAL_TIME_ALWAYS_LOAD                (1U<<14)
#define BFCMAP_PORT_PHY_TIMESYNC_NCO_ADDEND_LOAD                       (1U<<13)
#define BFCMAP_PORT_PHY_TIMESYNC_NCO_ADDEND_ALWAYS_LOAD                (1U<<12)
#define BFCMAP_PORT_PHY_TIMESYNC_DPLL_LOOP_FILTER_LOAD                 (1U<<11)
#define BFCMAP_PORT_PHY_TIMESYNC_DPLL_LOOP_FILTER_ALWAYS_LOAD          (1U<<10)
#define BFCMAP_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_LOAD                   (1U<<9)
#define BFCMAP_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_ALWAYS_LOAD            (1U<<8)
#define BFCMAP_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_LOAD             (1U<<7)
#define BFCMAP_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_ALWAYS_LOAD      (1U<<6)
#define BFCMAP_PORT_PHY_TIMESYNC_DPLL_K3_LOAD                          (1U<<5)
#define BFCMAP_PORT_PHY_TIMESYNC_DPLL_K3_ALWAYS_LOAD                   (1U<<4)
#define BFCMAP_PORT_PHY_TIMESYNC_DPLL_K2_LOAD                          (1U<<3)
#define BFCMAP_PORT_PHY_TIMESYNC_DPLL_K2_ALWAYS_LOAD                   (1U<<2)
#define BFCMAP_PORT_PHY_TIMESYNC_DPLL_K1_LOAD                          (1U<<1)
#define BFCMAP_PORT_PHY_TIMESYNC_DPLL_K1_ALWAYS_LOAD                   (1U<<0)

#define BFCMAP_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT                   (1U<<1)
#define BFCMAP_PORT_PHY_TIMESYNC_FRAMESYNC_INTERRUPT                   (1U<<0)

#define BFCMAP_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_MASK              (1U<<1)
#define BFCMAP_PORT_PHY_TIMESYNC_FRAMESYNC_INTERRUPT_MASK              (1U<<0)

/* bfcmap_port_control_phy_timesync_set */
extern int 
bfcmap_port_control_phy_timesync_set(
    blmi_dev_addr_t phy_id, 
    bfcmap_port_control_phy_timesync_t type, 
    buint64_t value);

/* bfcmap_port_control_phy_timesync_get */
extern int 
bfcmap_port_control_phy_timesync_get(
    blmi_dev_addr_t phy_id, 
    bfcmap_port_control_phy_timesync_t type, 
    buint64_t *value);


#endif /* _bfcmap_phy84756_H_ */
