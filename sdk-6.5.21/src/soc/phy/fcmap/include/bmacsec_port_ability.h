/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/* Common definitions for port_ability (BMACSEC) */

#ifndef BMACSEC_PORT_ABILITY_H
#define BMACSEC_PORT_ABILITY_H

typedef enum bmacsec_phy_speed_s {
    BMACSEC_SPEED_10 = 0,
    BMACSEC_SPEED_100,
    BMACSEC_SPEED_1000,
    BMACSEC_SPEED_2500
}bmacsec_phy_speed_t;

typedef enum bmacsec_phy_duplex_s {
    BMACSEC_HALF_DUPLEX = 0,
    BMACSEC_FULL_DUPLEX
}bmacsec_phy_duplex_t;

typedef enum bmacsec_phy_port_mode_s {
    BMACSEC_PHY_FIBER = 0,
    BMACSEC_PHY_COPPER
}bmacsec_phy_port_mode_t;

typedef enum bmacsec_phy_port_medium_s {
    BMACSEC_PHY_MEDIUM_COPPER = 0,
    BMACSEC_PHY_MEDIUM_FIBER,
    BMACSEC_PHY_MEDIUM_BOTH
}bmacsec_phy_port_medium_t;


/*
 * Switch MAC speed and duplex policies
 */
typedef enum bmacsec_phy_mac_policy_s {
    BMACSEC_SWITCH_FIXED = 0,
    BMACSEC_SWITCH_FOLLOWS_LINE
}bmacsec_phy_mac_policy_t;

 
typedef enum bmacsec_port_mdix_s {
    BMACSEC_PORT_MDIX_AUTO,
    BMACSEC_PORT_MDIX_FORCE_AUTO,
    BMACSEC_PORT_MDIX_NORMAL,
    BMACSEC_PORT_MDIX_XOVER
}bmacsec_port_mdix_t;

typedef enum bmacsec_port_mdix_status_s {
    BMACSEC_PORT_MDIX_STATUS_NORMAL,
    BMACSEC_PORT_MDIX_STATUS_XOVER
}bmacsec_port_mdix_status_t;


typedef enum bmacsec_phy_master_s {
    BMACSEC_PORT_MS_SLAVE =0,
    BMACSEC_PORT_MS_MASTER,
    BMACSEC_PORT_MS_AUTO,
    BMACSEC_PORT_MS_NONE
}bmacsec_phy_master_t;

typedef unsigned int bmacsec_port_mode_t;
typedef unsigned int bmacsec_pa_encap_t;

typedef struct bmacsec_port_ability_s {
    bmacsec_port_mode_t speed_half_duplex;
    bmacsec_port_mode_t speed_full_duplex;
    bmacsec_port_mode_t pause;
    bmacsec_port_mode_t interface;
    bmacsec_port_mode_t medium;
    bmacsec_port_mode_t loopback;
    bmacsec_port_mode_t flags;
    bmacsec_port_mode_t eee;
    bmacsec_pa_encap_t  encap;
} bmacsec_port_ability_t;


/*
 * Defines:
 *      BMACSEC_PA_SPEED*
 * Purpose:
 *      Defines for port ability speed.
 * 
 * Note: The defines are matched with SDK defines
 */

#define BMACSEC_PA_SPEED_10MB         (1 << 0)
#define BMACSEC_PA_SPEED_100MB        (1 << 5)
#define BMACSEC_PA_SPEED_1000MB       (1 << 6)

/*
 * Defines:
 *      _BMACSEC_PA_PAUSE_*
 * Purpose:
 *      Defines for flow control abilities.
 *
 * Note: The defines are matched with SDK defines
 */
#define BMACSEC_PA_PAUSE_TX        (1 << 0)       /* TX pause capable */
#define BMACSEC_PA_PAUSE_RX        (1 << 1)       /* RX pause capable */
#define BMACSEC_PA_PAUSE_ASYMM     (1 << 2)       /* Asymm pause capable (R/O) */
#define BMACSEC_PA_PAUSE           (BMACSEC_PA_PAUSE_TX  | BMACSEC_PA_PAUSE_RX)



/*
 * Defines:
 *      BMACSEC_PA_INTF_*
 * Purpose:
 *      Defines for port interfaces supported.
    */
#define BMACSEC_PA_INTF_MII        (1 << 1)       /* MII mode supported */
#define BMACSEC_PA_INTF_GMII       (1 << 2)       /* GMII mode supported */
#define BMACSEC_PA_INTF_SGMII      (1 << 4)       /* SGMII mode supported */
#define BMACSEC_PA_INTF_XGMII      (1 << 5)       /* XGMII mode supported */
#define BMACSEC_PA_INTF_QSGMII     (1 << 6)       /* QSGMII mode supported */


/*
 * Defines:
 *      BMACSEC_PA_LOOPBACK_*
 * Purpose:
 *      Defines for port loopback modes.
 */
#define BMACSEC_PA_LB_NONE         (1 << 0)       /* Useful for automated test */
#define BMACSEC_PA_LB_MAC          (1 << 1)       /* MAC loopback supported */
#define BMACSEC_PA_LB_PHY          (1 << 2)       /* PHY loopback supported */
#define BMACSEC_PA_LB_LINE         (1 << 3)       /* PHY lineside loopback */

/*
 * Defines:
 *      BMACSEC_PA_FLAGS_*
 * Purpose:
 *      Defines for the reest of port ability flags.
 */
#define BMACSEC_PA_AUTONEG         (1 << 0)       /* Auto-negotiation */
#define BMACSEC_PA_COMBO           (1 << 1)       /* COMBO ports support both
                                                * copper and fiber interfaces */




#define BMACSEC_PA_SPEED_ALL     (BMACSEC_PA_1000MB |      \
                                 BMACSEC_PA_100MB |        \
                                 BMACSEC_PA_10MB)

#define BMACSEC_PA_SPEED_MAX(m)  (((m) & BMACSEC_PA_1000MB) ? 1000 : \
                                 ((m) & BMACSEC_PA_100MB)  ? 100 :   \
                                 ((m) & BMACSEC_PA_10MB)   ? 10 : 0)

#define BMACSEC_PA_SPEED(s)      ((1000  == (s)) ? BMACSEC_PA_1000MB : \
                                 (100    == (s)) ? BMACSEC_PA_100MB :  \
                                 (10     == (s)) ? BMACSEC_PA_10MB : 0)

#define BMACSEC_PA_FD            (BMACSEC_PA_1000MB |    \
                                 BMACSEC_PA_100MB   |    \
                                 BMACSEC_PA_10MB)

#define BMACSEC_PA_HD            (BMACSEC_PA_1000MB |    \
                                 BMACSEC_PA_100MB |      \
                                 BMACSEC_PA_10MB)


/* EEE abilities */
#define BMACSEC_PA_EEE_100MB_BASETX       (1 << 0)    /* EEE for 100M-BaseTX */
#define BMACSEC_PA_EEE_1GB_BASET          (1 << 1)    /* EEE for 1G-BaseT */
#define BMACSEC_PA_EEE_10GB_BASET         (1 << 2)    /* EEE for 10G-BaseT */
#define BMACSEC_PA_EEE_10GB_KX            (1 << 3)    /* EEE for 10G-KX */
#define BMACSEC_PA_EEE_10GB_KX4           (1 << 4)    /* EEE for 10G-KX4 */
#define BMACSEC_PA_EEE_10GB_KR            (1 << 5)    /* EEE for 10G-KR */

#endif
