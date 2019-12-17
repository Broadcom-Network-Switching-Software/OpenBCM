/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 *
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __CMICX_LED_PUBLIC_H__
#define __CMICX_LED_PUBLIC_H__

#include <bcm/types.h>

#define CUSTOM_HANDLER_ADDR (0x3800)

/* LED Output pattern defines */
#define LED_OUTPUT_RX               (1 << 0)
#define LED_OUTPUT_TX               (1 << 1)
#define LED_OUTPUT_COLLISION 	    (1 << 2)
#define LED_OUTPUT_SPEED            (1 << 3)
#define LED_OUTPUT_DUPLEX     	    (1 << 5)
#define LED_OUTPUT_FLOW_CTRL        (1 << 6)
#define LED_OUTPUT_LINK_UP          (1 << 7)
#define LED_OUTPUT_LINK_ENABLE      (1 << 8)


/* Macros and structure for LED interface */
#define LED_MAX_NUM_INTF (5) /* Max number of LED interfaces */
#define LED_INVALID_PORT 0xFFFF
#define LED_MAX_NUM_BITS_PER_PORT_OUTPUT 16



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
    LED_SPD_INVALID /* Should ALWAYS be last */
} soc_led_speed_t;

typedef struct soc_led_intf_ctrl_s {
    uint32 valid; /* interface valid, default to 0 */
    uint32 start_row;
    uint32 end_row;
    uint32 pat_width;
} soc_led_intf_ctrl_t;

typedef struct soc_led_custom_handler_ctrl_s{
    uint32 accu_ram_base;
    uint32 pat_ram_base;
    uint8 *port_speed; /* Pointer to port_speed[] */
                       /* as defined in */
                       /* $sdk/include/shared/cmicfw/cmicx_led.h */
    soc_led_intf_ctrl_t intf_ctrl[LED_MAX_NUM_INTF];

} soc_led_custom_handler_ctrl_t;

typedef void (custom_handler_t)(soc_led_custom_handler_ctrl_t *ctrl, uint32 activity_count);

#endif /* __CMICX_LED_PUBLIC_H__ */
