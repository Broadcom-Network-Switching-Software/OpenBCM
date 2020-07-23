/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef   _bsdk_phy84728_H_
#define   _bsdk_phy84728_H_


#define BSDK_PHY84728_BRCM_OUI3   0x1be9        /* Broadcom OUI */
#define BSDK_PHY84728_BRCM_OUI1   0x1018        /* Broadcom OUI */

/*
 * Supported PHYs
 */

#if 0
#define BSDK_PHY_BCM84728_MODEL          0x3d
#endif
#define BSDK_PHY_BCM84728_MODEL          0x27

typedef enum bsdk_phy84728_speed_s {
    BSDK_PHY84728_SPEED_10 = 0,
    BSDK_PHY84728_SPEED_100,
    BSDK_PHY84728_SPEED_1000,
    BSDK_PHY84728_SPEED_10000
}bsdk_phy84728_speed_t;

typedef enum bsdk_phy84728_duplex_s {
    BSDK_PHY84728_HALF_DUPLEX = 0,
    BSDK_PHY84728_FULL_DUPLEX
}bsdk_phy84728_duplex_t;

typedef enum bsdk_phy84728_intf_s {
    BSDK_PHY84728_INTF_SGMII = 0,
    BSDK_PHY84728_INTF_1000X,
    BSDK_PHY84728_INTF_SFI,
    BSDK_PHY84728_INTF_XFI,
    BSDK_PHY84728_INTF_XAUI
}bsdk_phy84728_intf_t;

/*
 * Switch MAC speed and duplex policies
 */
typedef enum bsdk_phy84728_mac_policy_s {
    BSDK_BCM84728_SWITCH_FIXED = 0,
    BSDK_BCM84728_SWITCH_FOLLOWS_LINE
}bsdk_phy84728_mac_policy_t;

 
typedef enum bsdk_phy84728_port_mdix_s {
    BSDK_PHY84728_PORT_MDIX_AUTO,
    BSDK_PHY84728_PORT_MDIX_FORCE_AUTO,
    BSDK_PHY84728_PORT_MDIX_NORMAL,
    BSDK_PHY84728_PORT_MDIX_XOVER
}bsdk_phy84728_port_mdix_t;

typedef enum bsdk_phy84728_port_mdix_status_s {
    BSDK_PHY84728_PORT_MDIX_STATUS_NORMAL,
    BSDK_PHY84728_PORT_MDIX_STATUS_XOVER
}bsdk_phy84728_port_mdix_status_t;


typedef enum bsdk_phy84728_master_s {
    BSDK_PHY84728_PORT_MS_SLAVE =0,
    BSDK_PHY84728_PORT_MS_MASTER,
    BSDK_PHY84728_PORT_MS_AUTO,
    BSDK_PHY84728_PORT_MS_NONE
}bsdk_phy84728_master_t;

typedef enum bsdk_phy84728_fcmap_dir_s {
    BSDK_PHY84728_FCMAP_DIR_EGRESS = 0,
    BSDK_PHY84728_FCMAP_DIR_INGRESS
} bsdk_phy84728_fcmap_dir_t;

typedef enum bsdk_phy84728_fcmap_port_mode_s {
    BSDK_PHY84728_FCMAP_FCOE_TO_FC_MODE = 0,
    BSDK_PHY84728_FCMAP_FCOE_TO_FCOE_MODE
} bsdk_phy84728_fcmap_port_mode_t;

typedef enum bsdk_phy84728_fcmap_port_speed_s {
    BSDK_PHY84728_FCMAP_PORT_SPEED_2GBPS = 0,
    BSDK_PHY84728_FCMAP_PORT_SPEED_4GBPS = 1,
    BSDK_PHY84728_FCMAP_PORT_SPEED_8GBPS = 2,
    BSDK_PHY84728_FCMAP_PORT_SPEED_16GBPS = 3,
    BSDK_PHY84728_FCMAP_PORT_SPEED_MAX_COUNT
} bsdk_phy84728_fcmap_port_speed_t;

typedef enum bsdk_phy84728_fcmap_port_state_s {
    BSDK_PHY84728_FCMAP_PORT_STATE_INIT = 0,
    BSDK_PHY84728_FCMAP_PORT_STATE_RESET = 1,
    BSDK_PHY84728_FCMAP_PORT_STATE_ACTIVE = 2,
    BSDK_PHY84728_FCMAP_PORT_STATE_LINKDOWN = 3,
    BSDK_PHY84728_FCMAP_PORT_STATE_DISABLE = 4,
    BSDK_PHY84728_FCMAP_PORT_STATE_MAX_COUNT
} bsdk_phy84728_fcmap_port_state_t;

typedef unsigned int bsdk_phy84728_port_mode_t;
typedef unsigned int bsdk_phy84728_port_ability_t;

/*
 * Port modes.
 */

#define BSDK_PHY84728_PM_10MB_HD         (1 << 0)
#define BSDK_PHY84728_PM_10MB_FD         (1 << 1)
#define BSDK_PHY84728_PM_100MB_HD        (1 << 2)
#define BSDK_PHY84728_PM_100MB_FD        (1 << 3)
#define BSDK_PHY84728_PM_1000MB_HD       (1 << 4)
#define BSDK_PHY84728_PM_1000MB_FD       (1 << 5)
#define BSDK_PHY84728_PM_PAUSE_TX        (1 << 6)
#define BSDK_PHY84728_PM_PAUSE_RX        (1 << 7)
#define BSDK_PHY84728_PM_PAUSE_ASYMM     (1 << 8)
#define BSDK_PHY84728_PM_TBI             (1 << 9)
#define BSDK_PHY84728_PM_MII             (1 << 10)
#define BSDK_PHY84728_PM_GMII            (1 << 11)
#define BSDK_PHY84728_PM_SGMII           (1 << 12)
#define BSDK_PHY84728_PM_LB_MAC          (1 << 13)
#define BSDK_PHY84728_PM_LB_NONE         (1 << 14)
#define BSDK_PHY84728_PM_LB_PHY          (1 << 15)
#define BSDK_PHY84728_PM_AN              (1 << 16)

#define BSDK_PHY84728_PM_PAUSE           (BSDK_PHY84728_PM_PAUSE_TX  | \
                                            BSDK_PHY84728_PM_PAUSE_RX)
#define BSDK_PHY84728_PM_10MB            (BSDK_PHY84728_PM_10MB_HD   | \
                                            BSDK_PHY84728_PM_10MB_FD)
#define BSDK_PHY84728_PM_100MB           (BSDK_PHY84728_PM_100MB_HD  | \
                                            BSDK_PHY84728_PM_100MB_FD)
#define BSDK_PHY84728_PM_1000MB          (BSDK_PHY84728_PM_1000MB_HD | \
                                            BSDK_PHY84728_PM_1000MB_FD)

#define BSDK_PHY84728_PM_SPEED_ALL       (BSDK_PHY84728_PM_1000MB |    \
                                            BSDK_PHY84728_PM_100MB |     \
                                            BSDK_PHY84728_PM_10MB)

#define BSDK_PHY84728_PM_FD              (BSDK_PHY84728_PM_1000MB_FD |   \
                                            BSDK_PHY84728_PM_100MB_FD  |   \
                                            BSDK_PHY84728_PM_10MB_FD)

#define BSDK_PHY84728_PM_HD              (BSDK_PHY84728_PM_1000MB_HD |   \
                                            BSDK_PHY84728_PM_100MB_HD  |   \
                                            BSDK_PHY84728_PM_10MB_HD)

/*
 * Port ability
 */
#define BSDK_PHY84728_PA_10MB_HD         (1 << 0)
#define BSDK_PHY84728_PA_10MB_FD         (1 << 1)
#define BSDK_PHY84728_PA_100MB_HD        (1 << 2)
#define BSDK_PHY84728_PA_100MB_FD        (1 << 3)
#define BSDK_PHY84728_PA_1000MB_HD       (1 << 4)
#define BSDK_PHY84728_PA_1000MB_FD       (1 << 5)
#define BSDK_PHY84728_PA_10000MB_FD      (1 << 6)
#define BSDK_PHY84728_PA_PAUSE_TX        (1 << 7)
#define BSDK_PHY84728_PA_PAUSE_RX        (1 << 8)
#define BSDK_PHY84728_PA_PAUSE_ASYMM     (1 << 9)
#define BSDK_PHY84728_PA_SGMII           (1 << 10)
#define BSDK_PHY84728_PA_XSGMII          (1 << 11)
#define BSDK_PHY84728_PA_LB_MAC          (1 << 12)
#define BSDK_PHY84728_PA_LB_NONE         (1 << 13)
#define BSDK_PHY84728_PA_LB_PHY          (1 << 14)
#define BSDK_PHY84728_PA_AN              (1 << 15)

#define BSDK_PHY84728_PA_PAUSE      (BSDK_PHY84728_PA_PAUSE_TX  | \
                                       BSDK_PHY84728_PA_PAUSE_RX)
#define BSDK_PHY84728_PA_10MB       (BSDK_PHY84728_PA_10MB_HD   | \
                                       BSDK_PHY84728_PA_10MB_FD)
#define BSDK_PHY84728_PA_100MB      (BSDK_PHY84728_PA_100MB_HD  | \
                                       BSDK_PHY84728_PA_100MB_FD)
#define BSDK_PHY84728_PA_1000MB     (BSDK_PHY84728_PA_1000MB_HD | \
                                       BSDK_PHY84728_PA_1000MB_FD)

#define BSDK_PHY84728_PA_SPEED_ALL  (BSDK_PHY84728_PA_1000MB |    \
                                       BSDK_PHY84728_PA_100MB |     \
                                       BSDK_PHY84728_PA_10MB)

#define BSDK_PHY84728_PA_FD         (BSDK_PA_PHY84728_1000MB_FD | \
                                       BSDK_PHY84728_PA_100MB_FD  | \
                                       BSDKPHY84728__PA_10MB_FD)

#define BSDK_PHY84728_PA_HD         (BSDK_PHY84728_PA_1000MB_HD | \
                                       BSDK_PHY84728_PA_100MB_HD |  \
                                       BSDK_PHY84728_PA_10MB_HD)

#define BSDK_PHY84728_PA_FCMAP                  (1 << 0)
#define BSDK_PHY84728_PA_FCMAP_FCMAC_LOOPBACK   (1 << 1)
#define BSDK_PHY84728_PA_FCMAP_AUTONEG          (1 << 2)
#define BSDK_PHY84728_PA_FCMAP_2GB              (1 << 3)
#define BSDK_PHY84728_PA_FCMAP_4GB              (1 << 4)
#define BSDK_PHY84728_PA_FCMAP_8GB              (1 << 5)
#define BSDK_PHY84728_PA_FCMAP_16GB             (1 << 6)

/*
 * Port mode
 */
#define BSDK_PHY84728_MODE_FCMAP         (1 << 0)

/* 
 * Standalone PHY driver functions 
 */
extern int
bsdk_phy84728_phy_mac_driver_detach(phy_ctrl_t *pc);


/* Init PHY with MDIO address phy_id */
extern int 
bsdk_phy84728_init(phy_ctrl_t     *pc, uint8 reset, int line_mode, 
                     int fcmap_enable, int phy_ext_boot);

/* Reset PHY with MDIO address phy_id */
extern int 
bsdk_phy84728_reset(phy_ctrl_t     *pc);

/* Post Reset PHY setup with MDIO address phy_id */
extern int
bsdk_phy84728_no_reset_setup(phy_ctrl_t     *pc, int line_mode,
                               int phy_ext_boot);

/* Enable/Disable PHY */
extern int
bsdk_phy84728_enable_set(phy_ctrl_t     *pc, int enable);

/* Set Lane Map of PHY */
extern int
bsdk_phy84728_lane_map(phy_ctrl_t     *pc);

/* Set Speed of PHY and (Line and Switch) UNIMACs */
extern int
bsdk_phy84728_speed_set(phy_ctrl_t     *pc, int speed);

/* Get Speed of PHY and (Line and Switch) UNIMACs */
extern int
bsdk_phy84728_speed_get(phy_ctrl_t     *pc, int *speed);

/* Set duplex of PHY and (Line and Switch) UNIMACs */
extern int
bsdk_phy84728_duplex_set(phy_ctrl_t     *pc, bsdk_phy84728_duplex_t duplex);

/* Get duplex of PHY and (Line and Switch) UNIMACs */
extern int
bsdk_phy84728_duplex_get(phy_ctrl_t *pc, bsdk_phy84728_duplex_t *duplex);

/* Set AUTO negotiation Attributes */
extern int
bsdk_phy84728_an_set(phy_ctrl_t *pc, int an, int autoneg_advert);

/* Get AUTO negotiation Attributes */
extern int
bsdk_phy84728_an_get(phy_ctrl_t *pc, int *an, int *an_done);

/* Set MDIX */
extern int
bsdk_phy84728_mdix_set(phy_ctrl_t *pc, bsdk_phy84728_port_mdix_t mdix_mode);

/* MDIX Status Get */
extern int
bsdk_phy84728_mdix_status_get(phy_ctrl_t *pc, 
                                bsdk_phy84728_port_mdix_status_t *status);
/* Set Loopback */
extern int
bsdk_phy84728_loopback_set(phy_ctrl_t     *pc, int loopback);

/* Get loopback settings of the PHY */
extern int
bsdk_phy84728_loopback_get(phy_ctrl_t     *pc, int *loopback);

/* Set master */
extern int
bsdk_phy84728_master_set(phy_ctrl_t     *pc, bsdk_phy84728_master_t master);

/* Get Master */
extern int
bsdk_phy84728_master_get(phy_ctrl_t     *pc, int *master);

/* Set local autoneg advertising parameters */
extern int
bsdk_phy84728_ability_advert_set(phy_ctrl_t     *pc, 
                               bsdk_phy84728_port_ability_t ability);

/* Get local autoneg advertising parameters */
extern int
bsdk_phy84728_ability_advert_get(phy_ctrl_t     *pc, 
                               bsdk_phy84728_port_ability_t *ability);

/* Get Remote autoneg advertising parameters */
extern int
bsdk_phy84728_remote_ability_advert_get(phy_ctrl_t     *pc, 
                                bsdk_phy84728_port_ability_t *ability);

/* Get Link status */
extern int
bsdk_phy84728_link_get(phy_ctrl_t     *pc, int *link);

/* Gets the Ability of the PHY */
extern int
bsdk_phy84728_ability_local_get(phy_ctrl_t     *pc, 
                             bsdk_phy84728_port_ability_t *ability);

/* Download the firmware */
extern int
bsdk_phy84728_mdio_firmware_download(phy_ctrl_t     *pc,
                                  uint8 *new_fw, uint32 fw_length);
/* Firmware Update */
extern int
bsdk_phy84728_spi_firmware_update(phy_ctrl_t     *pc,
                                  uint8 *array, uint32 datalen);
/* Get Interface of side */
extern int
bsdk_phy84728_line_intf_get(phy_ctrl_t     *pc, int dev_port, 
                              bsdk_phy84728_intf_t *mode);
/* Set Interface of side */
extern int
bsdk_phy84728_line_intf_set(phy_ctrl_t     *pc, int dev_port, 
                              bsdk_phy84728_intf_t mode);

/* I2C related */
extern int
bsdk_phy_84740_i2cdev_read(phy_ctrl_t     *pc,
                     int dev_addr,
                     int offset,
                     int nbytes,
                     uint8 *read_array);

extern int
bsdk_phy_84740_i2cdev_write(phy_ctrl_t     *pc,
                     int dev_addr,
                     int offset,
                     int nbytes,
                     uint8 *write_array);

#endif /* _bsdk_phy84728_H_ */
