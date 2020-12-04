/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        custom_led.c
 * Purpose:     Customer CMICx LED bit pattern composer.
 * Requires:
 */

/******************************************************************************
 *
 * The CMICx LED interface has two RAM Banks as shown below, Bank0
 * (Accumulation RAM) for accumulation of status from ports and Bank1
 * (Pattern RAM) for writing LED pattern. Both Bank0 and Bank1 are of
 * 1024x16-bit, each row representing one port.
 *
 *           Accumulation RAM (Bank 0)        Pattern RAM (Bank1)
 *          15                       0     15                          0
 *         ----------------------------   ------------------------------
 * Row 0   |  led_uc_port 0 status    |   | led_uc_port 0 LED Pattern   |
 *         ----------------------------   ------------------------------
 * Row 1   |  led_uc_port 1 status    |   | led_uc_port 1 LED Pattern   |
 *         ----------------------------   ------------------------------
 *         |                          |   |                             |
 *         ----------------------------   ------------------------------
 *         |                          |   |                             |
 *         ----------------------------   ------------------------------
 *         |                          |   |                             |
 *         ----------------------------   ------------------------------
 *         |                          |   |                             |
 *         ----------------------------   ------------------------------
 *         |                          |   |                             |
 *         ----------------------------   ------------------------------
 * Row 127 |  led_uc_port 128 status  |   | led_uc_port 128 LED Pattern |
 *         ----------------------------   ------------------------------
 * Row 128 |                          |   |                             |
 *         ----------------------------   ------------------------------
 *         |                          |   |                             |
 *         ----------------------------   ------------------------------
 *         |                          |   |                             |
 *         ----------------------------   ------------------------------
 * Row x   |  led_uc_port (x+1) status|   | led_uc_port(x+1) LED Pattern|
 *         ----------------------------   ------------------------------
 *         |                          |   |                             |
 *         ----------------------------   ------------------------------
 *         |                          |   |                             |
 *         ----------------------------   ------------------------------
 * Row 1022|  led_uc_port 1022 status |   | led_uc_port 1022 LED Pattern|
 *         ----------------------------   ------------------------------
 * Row 1023|  led_uc_port 1023 status |   | led_uc_port 1023 LED Pattern|
 *         ----------------------------   ------------------------------
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
 * the HW Accumulation RAM or "led_control_data" array, then form the required
 * LED bit pattern in the Pattern RAM at the corrsponding location.
 *
 * The "led_control_data" is a 1024 bytes array, application user can use BCM LED API
 * to exchange port infomation with LED FW.
 *
 * Note that, for Firelight led_uc_port = physical port number - 2.
 * For ESW chip, led_uc_port = physical port number - 1.
 * For DNX/DNXF chip, led_uc_port = physical port number.
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
 * include/shared/cmicfw/cmicx_led_public.h.
 */

#include <shared/cmicfw/cmicx_led_public.h>

/*! Macro to calculate LED RAM address. */
#define LED_HW_RAM_ADDR(base, port) \
    (base + (port * sizeof(uint32)))

/*! Macro to read LED RAM. */
#define LED_HW_RAM_READ16(base, port) \
    *((uint16 *) LED_HW_RAM_ADDR(base, port))

/*! Macro to write LED RAM. */
#define LED_HW_RAM_WRITE16(base, port, val)  \
    *((uint16 *) LED_HW_RAM_ADDR(base, port)) = (val)

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
customer_led_handler(soc_led_custom_handler_ctrl_t *ctrl, uint32 cnt)
{
    uint16 led_uc_port;
    uint16 intf;
    uint16 accu_val = 0, pattern = 0;
    uint8 idx;

    /*
     * Assume that board configuration is as follows.
     *  - Front-panel ports are 128x25G.
     *  - led_uc_port 0-63 are connected to LED interface 0.
     *  - led_uc_port 64-127 are connected to LED interface 1.
     *  - Each port has two LEDs to display link status and activity.
     *  - LED1 pattern: b[1-0] - Link status: Grern - Up, Off - Down
     *    LED2 pattern: b[3-2] - Tx/Tx Activity: Green/Orange.
     */

    /* Process all fornt ports. */
    for (led_uc_port = 0; led_uc_port < 384; led_uc_port ++) {

        /* Read value from accumulation RAM */
        accu_val = LED_HW_RAM_READ16(ctrl->accu_ram_base, led_uc_port);

        /* Read value from pattern RAM */
        pattern = LED_HW_RAM_READ16(ctrl->pat_ram_base, led_uc_port);

        /* Check software link up flag */
        if (ctrl->led_control_data[led_uc_port] & LED_SW_LINK_UP) {

            /* Set Link Status in bits [1:0]. */
            pattern &= ~(0x3);
            pattern |= (LED_GREEN);

            /* Check software link up flag and port activity */
            if (cnt & ACT_TICKS) {
                if (accu_val & (LED_HW_RX | LED_HW_TX)) {

                    /* Clear activity output.*/
                    pattern &= ~(0xC);

                    if ((ctrl->led_control_data[led_uc_port] & 0x6) == (LED_ORANGE << 1))
                    {
                        /* Set Activity in bits [3:2] to orange. */
                        pattern |= (LED_ORANGE << 2);
                    } else {

                        /* Set Activity in bits [3:2] to green. */
                        pattern |= (LED_GREEN << 2);
                    }
                 }
            } else {

                /* Clear activity output.*/
                pattern &= ~(0xC);
            }
        } else {

            /* Clear Link Status and Activity bits */
            pattern &= ~0xf;
        }

        /* Write value to pattern RAM */
        LED_HW_RAM_WRITE16(ctrl->pat_ram_base, led_uc_port, pattern);
    }

    /* Configure LED HW interfaces based on board configuration */
    for (idx = 0; idx < LED_HW_INTF_MAX_NUM; idx++) {
        soc_led_intf_ctrl_t *lic = &ctrl->intf_ctrl[idx];
        switch (idx) {
        case 0:

            /* Send the pattern over LED interface 0 for ports 0 - 63 */
            lic->valid = 1;
            lic->start_row = 0;
            lic->end_row = 63;
            lic->pat_width = 4;
            break;
        case 1:

            /* Send the pattern over LED interface 1 for ports 64 - 127 */
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
