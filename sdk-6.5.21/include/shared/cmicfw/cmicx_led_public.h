/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cmicx_led_public.h
 * Purpose:     Customer CMICx LED bit pattern composer.
 * Requires:
 */

#ifndef CMICX_PUBLIC_LED_H
#define CMICX_PUBLIC_LED_H

#include <bcm/types.h>

/* LED color of Broadcom LED box.
 * Customer should define their own LED state.
 */
#define LED_OFF                 (0)
#define LED_ORANGE              (1)
#define LED_GREEN               (2)

/*! Flags for each hardware entry of LED accumulation RAM. */
#define LED_HW_RX               (1 << 0)
#define LED_HW_TX               (1 << 1)
#define LED_HW_COLLISION        (1 << 2)
#define LED_HW_SPEED            (1 << 3)
#define LED_HW_DUPLEX           (1 << 5)
#define LED_HW_FLOW_CTRL        (1 << 6)
#define LED_HW_LINK_UP          (1 << 7)
#define LED_HW_LINK_ENABLE      (1 << 8)

/*! Max number of LED interfaces. */
#define LED_HW_INTF_MAX_NUM     (5)

/*! Max number of bit per port. */
#define LED_HW_MAX_NUM_BITS_PER_PORT_OUTPUT 16

/*! Lagacy port speed enumeration which is not recommend to use. */
typedef enum soc_led_speed_e {
    LED_SPD_10M = 1,    /* 10 Mbps  */
    LED_SPD_100M,       /* 100 Mbps */
    LED_SPD_1000M,      /* 1 Gbps   */
    LED_SPD_2500M,      /* 2.5 Gbps */
    LED_SPD_10G,        /* 10 Gbps  */
    LED_SPD_25G,        /* 25 Gbps  */
    LED_SPD_40G,        /* 40 Gbps  */
    LED_SPD_50G,        /* 50 Gbps  */
    LED_SPD_100G,       /* 100 Gbps */
    LED_SPD_200G,       /* 200 Gbps */
    LED_SPD_400G,       /* 400 Gbps */
    LED_SPD_INVALID     /* Should ALWAYS be last */
} soc_led_speed_t;

/*! \brief LED interface control structure.
 *
 *   This struct can define
 *       - Interface valid or not.
 *       - The range of LED pattern RAM for
 *         output bit pattern of a LED interface.
 */
typedef struct led_intf_ctrl_s {

    /*! Interface valid, default to 0. */
    uint32 valid;

    /*! Start row of output pattern. */
    uint32 start_row;

    /*! Stop row of output pattern. */
    uint32 end_row;

    /*! Pattern width of each row. */
    uint32 pat_width;

} soc_led_intf_ctrl_t;

/*
 *  The interface between LED customer firmware and SDK
 */
typedef struct led_customer_s {

    /*! Base address of acculation RAM. */
    uint32 accu_ram_base;

    /*! Base address of Pattern RAM. */
    uint32 pat_ram_base;

    /*! Base address of "led_control_data" space. */
    uint8 *led_control_data;

    /*! Setting for LED interfaces. */
    soc_led_intf_ctrl_t intf_ctrl[LED_HW_INTF_MAX_NUM];

} soc_led_custom_handler_ctrl_t;

typedef void (custom_handler_t)(soc_led_custom_handler_ctrl_t *ctrl, uint32 activity_count);

#endif /* CMICX_LED_PUBLIC_H */
