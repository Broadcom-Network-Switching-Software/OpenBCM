/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 */

/******************************************************************************
CMICX LED Interface has two RAM Banks, as shown below, Bank0(ACCUMULATION RAM)
for accumulation of status from ports and Bank1(PATTERN RAM) for writing
LED pattern. Both Bank0 and Bank1 are of 1024x16-bit, each row representing
one port.

         ACCUMULATION RAM (Bank 0)                Pattern RAM (Bank1)
         15                         0          15                         0
        ------------------------------         -----------------------------
Row 0   |  Port1 status              |         |  Port1 LED Pattern        |
        ------------------------------         -----------------------------
Row 1   |  Port2 status              |         |  Port2 LED Pattern        |
        ------------------------------         -----------------------------
        |                            |         |                           |
        ------------------------------         -----------------------------
        |                            |         |                           |
        ------------------------------         -----------------------------
        |                            |         |                           |
        ------------------------------         -----------------------------
        |                            |         |                           |
        ------------------------------         -----------------------------
        |                            |         |                           |
        ------------------------------         -----------------------------
Row 127 |  Port128 status            |         |  Port128 LED Pattern      |
        ------------------------------         -----------------------------
Row 128 |                            |         |                           |
        ------------------------------         -----------------------------
        |                            |         |                           |
        ------------------------------         -----------------------------
        |                            |         |                           |
        ------------------------------         -----------------------------
Row x   |  Port(x+1) status          |         |  Port(x+1) LED Pattern    |
        ------------------------------         -----------------------------
        |                            |         |                           |
        ------------------------------         -----------------------------
        |                            |         |                           |
        ------------------------------         -----------------------------
Row 1022|  Port1023 status           |         |  Port1023 LED Pattern     |
        ------------------------------         -----------------------------
Row 1023|  Port1024 status           |         |  Port1024 LED Pattern     |
        ------------------------------         -----------------------------

Format of Accumulation RAM:


Bits    15:9        8       7         6        5       4:3     2    1    0
     ------------------------------------------------------------------------
     | Reserved | Link  | Link Up |  Flow  | Duplex | Speed | Col | Tx | Rx |
     |          | Enable| Status  | Control|        |       |     |    |    |
     ------------------------------------------------------------------------

The custom handler in this file should read port status, for each port used,
from accumulation ram, and form required LED bit pattern in the Bank1 RAM
(pattern RAM) location corresponding to the port of interest. Note that
physical port numbers may differ from row number of LED RAM Banks. For
Trident3, Physical port numbers spread from 1 to 128 in 128x25G configuration
 and corresponding LED rows spread from Row 0 to Row 127.

There are five LED interfaces in CMICX based devices. Although single
interface can be used to output LED pattern for all ports, it is possible
that more than one interface can be used in the end system, e.g., LEDs for
some ports are connected to one LED interface-0 (i.e LED_CLK and LED_DATA),
while the rest of the ports are connected to LED interface-1. Accordingly,
custom handler MUST fill in start port, end port and width of pattern in the
soc_led_custom_handler_ctrl_t structure passsed to custom handler. The
example custom handler provided in this file has reference code for forming
two different LED patterns. Please refer to these patterns before writing your
own custom handler code.

The soc_led_custom_handler_ctrl_t structure definition is available in
$SDK/include/shared/cmicfw/cmicx_led_public.h

soc_led_custom_handler_ctrl_t structure also carries a point to array
port_speed[] of size equal to maximum ports in the system, e.g 128 in Trident3.
This array would have port speed for each port, as per bit mapping defined in
"soc_led_speed_t" in $SDK/include/shared/cmicfw/cmicx_led_public.h file.

Here is an exception, please keep in mind:
1. For TH3, port status/speed of  xe1 (physical port 258) is located in the 
   accumulation entry/speed array  of physical port 259.
 */
#include <shared/cmicfw/cmicx_led_public.h>

#define ACTIVITY_TICKS 2
#define READ_LED_ACCU_DATA(base, port) (*((uint16 *)(base + ((port - 1) * sizeof(uint32)))))
#define WRITE_LED_SEND_DATA(base, port, val)  (*((uint16 *)(base + ((port - 1) * sizeof(uint32)))) = val)

#define SELECT_BRCM_PATTERN 1

/*
 * Function:
 *      custom_led_handler
 * Purpose:
 *      Timer event handler to accumulate, process and transmit led status
 * Parameters:
 *      param - parameter added while registering the timer event.
 * Returns:
 *      0 on success
 *      Error code on failure
 */
void custom_led_handler(soc_led_custom_handler_ctrl_t *ctrl, uint32 activity_count)
{
    unsigned short accu_val = 0, send_val = 0;
    unsigned short port, intf;

#if SELECT_BRCM_PATTERN
    /* Pattern for Broadcom SVK with 128x25G configuration */
    /* Port 1 to 64 connected to LED interface 0 */
    /* Port 65 to 128 connected to LED interface 1 */
    /* Pattern: b[1-0] - Link status: 10 - UP, 00 - DOWN */
    /*          b[3-2] - Activity: 01 - Tx/Rx activity */

    /* Physical port numbers to be used */
    for(port = 1; port <= 128; port++) {
        /* Read value from led_ram bank0 */
        accu_val = READ_LED_ACCU_DATA(ctrl->accu_ram_base, port);

        send_val = 0x0; /* Default Off - b'0000 */

        if (accu_val & LED_OUTPUT_LINK_UP) {
            send_val = 0x2;
        }

        if ((accu_val & LED_OUTPUT_LINK_UP) &&
            ((accu_val & LED_OUTPUT_RX) || (accu_val & LED_OUTPUT_TX)) &&
             (activity_count & ACTIVITY_TICKS)) {
            send_val |= (0x1 << 2);
        }

        /* Write value to led_ram bank1 */
        WRITE_LED_SEND_DATA(ctrl->pat_ram_base, port, send_val);
    } /* for */

    /* Send the pattern over LED interface 0 for ports 1 - 64*/
    ctrl->intf_ctrl[0].valid = 1;
    ctrl->intf_ctrl[0].start_row = 0;
    ctrl->intf_ctrl[0].end_row = 63;
    ctrl->intf_ctrl[0].pat_width = 4;

    ctrl->intf_ctrl[1].valid = 1;
    ctrl->intf_ctrl[1].start_row = 64;
    ctrl->intf_ctrl[1].end_row = 127;
    ctrl->intf_ctrl[1].pat_width = 4;

    /* Invalidate rest of the interfaces */
    ctrl->intf_ctrl[2].valid = 0;
    ctrl->intf_ctrl[3].valid = 0;
    ctrl->intf_ctrl[4].valid = 0;

#else
    /* Alternate reference pattern */
    /* For every port toggle between 3'b111 and
       100/011/010/001/000 based on port speed */
    /* Send pattern over single LED interface */
    for(port = 1; port <= 128; port++) {
        /* Read value from led_ram bank0 */
        accu_val = READ_LED_ACCU_DATA(ctrl->accu_ram_base, port);

        send_val = 0x7; /* Default Off - b'111 */

        if (accu_val & LED_OUTPUT_LINK_UP) {

            switch(ctrl->port_speed[port-1]) {
                case LED_SPD_10G:
                    send_val = 0x0; /* b'000 */
                    break;
                case LED_SPD_25G:
                    send_val = 0x1; /* b'001 */
                    break;
                case LED_SPD_40G:
                    send_val = 0x2; /* b'010 */
                    break;
                case LED_SPD_50G:
                    send_val = 0x3; /* b'011 */
                    break;
                case LED_SPD_100G:
                    send_val = 0x4; /* b'100 */
                    break;
                case LED_SPD_200G:
                    send_val = 0x5; /* b'101 */
                    break;
                case LED_SPD_400G:
                    send_val = 0x6; /* b'110 */ 
                    break;
                default:
                    send_val = 0x7; /* b'111 */
                    break;
            }
        }

        if (((accu_val & LED_OUTPUT_RX) || (accu_val & LED_OUTPUT_TX)) &&
            (activity_count & ACTIVITY_TICKS)) {
            send_val = 0x7;
        }

        /* Write value to led_ram bank1 */
        WRITE_LED_SEND_DATA(ctrl->pat_ram_base, port, send_val);
    } /* for */

    /* Send the pattern over LED interface 0 */
    ctrl->intf_ctrl[0].valid = 1;
    ctrl->intf_ctrl[0].start_row = 0;
    ctrl->intf_ctrl[0].end_row = 127;
    ctrl->intf_ctrl[0].pat_width = 3;

    /* Invalidate rest of the interfaces */
    ctrl->intf_ctrl[1].valid = 0;
    ctrl->intf_ctrl[2].valid = 0;
    ctrl->intf_ctrl[3].valid = 0;
    ctrl->intf_ctrl[4].valid = 0;
#endif
    return;
}
