/*! \file cmicx_customer_led.h
 *
 * Customer CMICx LED bit pattern composer.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef CMICX_CUSTOMER_LED_H
#define CMICX_CUSTOMER_LED_H

typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;


/*! Flags for each entry of LED accumulation RAM. */
#define LED_HW_RX               (1 << 0)
#define LED_HW_TX               (1 << 1)
#define LED_HW_COLLISION        (1 << 2)
#define LED_HW_SPEED            (1 << 3)
#define LED_HW_DUPLEX           (1 << 5)
#define LED_HW_FLOW_CTRL        (1 << 6)
#define LED_HW_LINK_UP          (1 << 7)
#define LED_HW_LINK_ENABLE      (1 << 8)

/*! Max number of LED interfaces. */
#define LED_HW_MAX_NUM_INTF (5)

/*! Max number of bit per port. */
#define LED_HW_MAX_NUM_BITS_PER_PORT_OUTPUT 16

/*! \brief LED interface control structure.
 *
 *   This struct can define
 *       - Interface valid or not.
 *       - The range of LED pattern RAM for
 *         output bit pattern of a LED interface.
 */
typedef struct led_intf_ctrl_s {

    /*! Interface valid, default to 0. */
    uint32_t valid;

    /*! Start row of output pattern. */
    uint32_t start_row;

    /*! Stop row of output pattern. */
    uint32_t end_row;

    /*! Pattern width of each row. */
    uint32_t pat_width;

} led_intf_ctrl_t;

/*
 *  The interface between LED customer firmware and SDK
 */
typedef struct led_customer_s {

    /*! Base address of acculation RAM */
    uint32_t accu_ram_base;

    /*! Base address of Pattern RAM */
    uint32_t pat_ram_base;

    /*! Base address of "swdata" space */
    uint8_t *swdata;

    /*! Setting for LED interfaces. */
    led_intf_ctrl_t intf_ctrl[LED_HW_MAX_NUM_INTF];

} led_customer_t;

#endif /* CMICX_CUSTOMER_LED_H */
