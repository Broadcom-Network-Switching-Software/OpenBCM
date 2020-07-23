/*! \file cmicx_customer_led.c
 *
 * Customer CMICx LED bit pattern composer.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/******************************************************************************
 *
 * The CMICx LED interface has two RAM Banks as shown below, Bank0
 * (Accumulation RAM) for accumulation of status from ports and Bank1
 * (Pattern RAM) for writing LED pattern. Both Bank0 and Bank1 are of
 * 1024x16-bit, each row representing one port.
 *
 *           Accumulation RAM (Bank 0)              Pattern RAM (Bank1)
 *          15                       0           15                      0
 *         ----------------------------         ---------------------------
 * Row 0   |  Port1 status            |         |  Port1 LED Pattern      |
 *         ----------------------------         ---------------------------
 * Row 1   |  Port2 status            |         |  Port2 LED Pattern      |
 *         ----------------------------         ---------------------------
 *         |                          |         |                         |
 *         ----------------------------         ---------------------------
 *         |                          |         |                         |
 *         ----------------------------         ---------------------------
 *         |                          |         |                         |
 *         ----------------------------         ---------------------------
 *         |                          |         |                         |
 *         ----------------------------         ---------------------------
 *         |                          |         |                         |
 *         ----------------------------         ---------------------------
 * Row 127 |  Port128 status          |         |  Port128 LED Pattern    |
 *         ----------------------------         ---------------------------
 * Row 128 |                          |         |                         |
 *         ----------------------------         ---------------------------
 *         |                          |         |                         |
 *         ----------------------------         ---------------------------
 *         |                          |         |                         |
 *         ----------------------------         ---------------------------
 * Row x   |  Port(x+1) status        |         |  Port(x+1) LED Pattern  |
 *         ----------------------------         ---------------------------
 *         |                          |         |                         |
 *         ----------------------------         ---------------------------
 *         |                          |         |                         |
 *         ----------------------------         ---------------------------
 * Row 1022|  Port1023 status         |         |  Port1023 LED Pattern   |
 *         ----------------------------         ---------------------------
 * Row 1023|  Port1024 status         |         |  Port1024 LED Pattern   |
 *         ----------------------------         ---------------------------
 *
 * Format of Accumulation RAM:
 *
 * Bits   15:9       8        7         6        5      4:3     2    1    0
 *    ------------------------------------------------------------------------
 *    | Reserved | Link  | Link-up |  Flow  | Duplex | Speed | Col | Tx | Rx |
 *    |          | Enable| Status  | Control|        |       |     |    |    |
 *    ------------------------------------------------------------------------
 *
 * Where Speed 00 - 10 Mbps
 *             01 - 100 Mbps
 *             10 - 1 Gbps
 *             11 - Above 1 Gbps
 *
 * The customer handler in this file should read the port status from
 * the HW Accumulation RAM or "swdata" array, then form the required LED bit
 * pattern in the Pattern RAM at the corrsponding location.
 *
 * The "swdata" is a 1024 bytes array, application user can use BCMBD-LED API
 * to write software port infomation to this array. Then customer LED FW can
 * read it from \c ctrl->swdata to know more port information.
 *
 * Note that the physical port numbers may differ from the row number
 * of the LED RAM banks. Typically the first front-panel port uses
 * physical port 1 and this would correspond to LED row 0. Physical
 * port 2 would then map to LED row 1, and so forth.
 *
 * There are five LED interfaces in CMICx-based devices, and although
 * a single interface can be used to output LED patterns for all
 * ports, it is possible to use more than one interface, e.g. the LEDs
 * for some ports are connected to LED interface-0, while the rest of
 * the ports are connected to LED interface-1. Accordingly, the custom
 * handler MUST fill in start-port, end-port and pattern-width in the
 * soc_led_custom_handler_ctrl_t structure passsed to the custom
 * handler.
 *
 * The example custom handler provided in this file has reference code
 * for forming two different LED patterns. Please refer to these
 * patterns before writing your own custom handler code.
 *
 * The led_customer_t structure definition is available in
 * cmicx_customer_led.h.
 */

#include "cmicx_customer_led.h"

/*! Macro to calculate LED RAM address. */
#define LED_HW_RAM_ADDR(base, port) \
    (base + ((port - 1) * sizeof(uint32_t)))

/*! Macro to read LED RAM. */
#define LED_HW_RAM_READ16(base, port) \
    *((uint16_t *) LED_HW_RAM_ADDR(base, port))

/*! Macro to write LED RAM. */
#define LED_HW_RAM_WRITE16(base, port, val)  \
    *((uint16_t *) LED_HW_RAM_ADDR(base, port)) = (val)

/*! The time window of activity LED diplaying on. */
#define ACT_TICKS 2

/*! Customer defined software flag. */
#define LED_SW_LINK_UP   0x1

/*!
 * \brief Function for LED bit pattern generator.
 *
 * Customer can compose the LED bit pattern to control serial LED
 * according to link/traffic information.
 *
 * \param [in,out] ctrl Data structure indicating the locations of the
 *                      port status and serial LED bit pattern RAM.
 * \param [in] cnt 30Hz counter.
 */
void
customer_led_handler(led_customer_t *ctrl,
                     uint32_t cnt)
{
    uint16_t accu_val = 0, pattern = 0;
    uint16_t pport, intf;
    uint8_t idx;

    /*
     * Assume that board configuration is as follows.
     *  - Front-panel ports are 128x25G.
     *  - Ports 1-64 are connected to LED interface 0.
     *  - Ports 65-128 are connected to LED interface 1.
     *  - Each port has two LEDs to display link status and activity.
     *  - LED1 pattern: b[1-0] - Link status: 10 - Up, 00 - Down
     *    LED2 pattern: b[3-2] - Activity: 01 - Tx/Rx activity
     */

    /* Physical port numbers to be used */
    for (pport = 1; pport <= 128; pport++) {

        /* Read value from accumulation RAM */
        accu_val = LED_HW_RAM_READ16(ctrl->accu_ram_base, pport);

        /* Read value from pattern RAM */
        pattern = LED_HW_RAM_READ16(ctrl->pat_ram_base, pport);

        /* Clear Link Status and Activity bits */
        pattern &= ~0xf;

        /* Check software link up flag */
        if (ctrl->swdata[pport - 1] & LED_SW_LINK_UP) {

            /* Set Link Status in bits [1:0] according to board configuration */
            pattern |= 0x2;

            /* Check software link up flag and port activity */
            if ((accu_val & (LED_HW_RX | LED_HW_TX)) &&
                (cnt & ACT_TICKS)) {

                /* Set Activity in bits [3:2] according to board configuration */
                pattern |= (0x1 << 2);
            }
        }

        /* Write value to pattern RAM */
        LED_HW_RAM_WRITE16(ctrl->pat_ram_base, pport, pattern);
    }

    /* Configure LED HW interfaces based on board configuration */
    for (idx = 0; idx < LED_HW_MAX_NUM_INTF; idx++) {
        led_intf_ctrl_t *lic = &ctrl->intf_ctrl[idx];
        switch (idx) {
        case 0:

            /* Send the pattern over LED interface 0 for ports 1 - 64 */
            lic->valid = 1;
            lic->start_row = 0;
            lic->end_row = 63;
            lic->pat_width = 4;
            break;
        case 1:

            /* Send the pattern over LED interface 1 for ports 65 - 128 */
            lic->valid = 1;
            lic->start_row = 64;
            lic->end_row = 127;
            lic->pat_width = 4;
            break;
        default:

            /* Invalidate rest of the interfaces */
            lic->valid = 0;
            break;
        }
    }

    return;
}
