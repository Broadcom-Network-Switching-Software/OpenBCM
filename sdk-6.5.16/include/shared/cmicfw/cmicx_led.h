/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __CMICX_LED_H__
#define __CMICX_LED_H__

#include <bcm/types.h>

#if defined(BCM_CMICX_SUPPORT) || defined(CMICX_FW_BUILD)
#include <shared/cmicfw/iproc_mbox.h>
#include <shared/cmicfw/cmicx_led_public.h>

/* Maximum number of ports supported by device */
#define LED_MAX_PORT 128

#define LED_STOP 0
#define LED_START 1

typedef enum soc_led_msg_id_e {
    LED_MSG_LNK_STS = 0,
    LED_MSG_STATUS,
    LED_MSG_ENABLE,
    LED_MSG_STATUS_SET,
    LED_MSG_SPEED,
    LED_MSG_ID_MAX /* Should ALWAYS be last */
} soc_led_msg_id_t;

typedef enum soc_led_ram_bank_e {
    LED_ACCU_RAM_BANK = 0,
    LED_SEND_RAM_BANK
} soc_led_ram_bank_t;

typedef struct soc_led_port_status_s {
    uint32 port;
    uint32 status;
    uint32 bank;
}__attribute__ ((packed)) soc_led_port_status_t;

typedef struct soc_led_link_status_s {
    uint32 port;
    uint32 link_sts;
}__attribute__ ((packed)) soc_led_link_status_t;

typedef struct soc_led_port_speed_s {
    uint32 port;
    uint32 speed;
}__attribute__ ((packed)) soc_led_port_speed_t;

#ifndef CMICX_FW_BUILD
extern int soc_cmicx_led_mbox_init(int unit);
extern int soc_cmicx_led_deinit(int unit);
extern int soc_cmicx_led_link_status(int unit, soc_led_link_status_t *link_status);
extern int soc_cmicx_led_status_get(int unit, soc_led_port_status_t *led_port_status);
extern int soc_cmicx_led_status_set(int unit, soc_led_port_status_t *led_port_status);
extern int soc_cmicx_led_enable(int unit, uint32 enable);
extern int soc_cmicx_led_speed(int unit, soc_led_port_speed_t *led_port_speed);

#endif /* !(CMICX_FW_BUILD) */
#endif /* defined(BCM_CMICX_SUPPORT) || defined(CMICX_FW_BUILD) */
#endif /* __CMICX_LED_H__ */
