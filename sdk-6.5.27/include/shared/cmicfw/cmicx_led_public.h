/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:        cmicx_led_public.h
 * Purpose:     Customer CMICx LED bit pattern composer.
 * Requires:
 */

#ifndef CMICX_PUBLIC_LED_H
#define CMICX_PUBLIC_LED_H

#include <bcm/types.h>

/******************************************************
 *  Special pport to led_uc_port mapping.
 *
 *  For those ports, led_uc_port != pport - constant.
 ******************************************************/
/* TD3 2nd management physical port. */
#define TD3_MGMT2_PPORT             128

/* TD3 2nd management led_uc_port. */
#define TD3_MGMT2_LED_UC_PORT       130

/* TH3 2nd management physical port. */
#define TH3_MGMT2_PPORT             258

/* TH3 2nd management led_uc_port. */
#define TH3_MGMT2_LED_UC_PORT       258

/* TH4 2nd management physical port. */
#define TH4_MGMT2_PPORT             258

/* TH4 2nd management led_uc_port. */
#define TH4_MGMT2_LED_UC_PORT       258

/* TH4G 2nd mangement physical port. */
#define TH4G_MGMT2_PPORT            258

/* TH4G 2nd management led_uc_port. */
#define TH4G_MGMT2_LED_UC_PORT      258

/* TH4GT 2nd mangement physical port. */
#define TH4GT_MGMT2_PPORT           258

/* TH4GT 2nd management led_uc_port. */
#define TH4GT_MGMT2_LED_UC_PORT     258

/******************************************************
 *  LED HW definition
 ******************************************************/
/*! Flags for each hardware entry of LED accumulation RAM. */
#define LED_HW_RX               (1 << 0)
#define LED_HW_TX               (1 << 1)
#define LED_HW_COLLISION        (1 << 2)
#define LED_HW_SPEED            (1 << 3)
#define LED_HW_DUPLEX           (1 << 5)
#define LED_HW_FLOW_CTRL        (1 << 6)
#define LED_HW_LINK_UP          (1 << 7)
#define LED_HW_LINK_ENABLE      (1 << 8)

/*! Max number of LED interfaces.
 *  CMICx devices have up to 5 interfaces.
 *  CMICx-GEN2 devices have up to 8 interfaces.
 */
#ifndef LED_HW_INTF_MAX_NUM
#define LED_HW_INTF_MAX_NUM     (8)
#endif

/*! Max number of bit per port. */
#define LED_HW_MAX_NUM_BITS_PER_PORT_OUTPUT 16

/*****************************************************
 *  LED HW access Macro
 *****************************************************/
/*! Macro to calculate LED RAM address. */
#define LED_HW_RAM_ADDR(base, led_uc_port) \
    (base + (led_uc_port * sizeof(uint32)))

/*! Macro to read LED RAM. */
#define LED_HW_RAM_READ16(base, led_uc_port) \
    *((uint16 *) LED_HW_RAM_ADDR(base, led_uc_port))

/*! Macro to write LED RAM. */
#define LED_HW_RAM_WRITE16(base, led_uc_port, val)  \
    *((uint16 *) LED_HW_RAM_ADDR(base, led_uc_port)) = (val)

/*****************************************************
 *  LED color of Broadcom LED box.
 *****************************************************/
#define LED_OFF                 (0)
#define LED_ORANGE              (1)
#define LED_GREEN               (2)

/*! \brief LED interface control structure.
 *
 *   This structure can define
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

    /*! Base address of accumulation RAM. */
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
