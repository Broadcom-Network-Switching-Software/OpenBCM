/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef __CMICX_LED_H__
#define __CMICX_LED_H__

#include <bcm/types.h>

#if defined(BCM_CMICX_SUPPORT) || defined(CMICX_FW_BUILD)
#include <shared/cmicfw/iproc_mbox.h>
#include <shared/cmicfw/cmicx_led_defs.h>
#include <shared/cmicfw/cmicx_led_public.h>

/* CMICX LED control data total size. */
#define CMICX_LED_CONTROL_DATA_SIZE          (1024)

/* Flag to disable hardware link override. */
#define CMICX_LED_HW_LINK_OVERRIDE_DISABLE   (0x2)

/* Customer LED location. */
#define CUSTOM_HANDLER_ADDR                  (0x3800)

/* CMICX LED signature. */
#define CMICX_LED_SIGNATURE                  (0x584C4544)

/* CMICX LED UC. */
#define CMICX_LED_UC                         (0)

/*! CMICX LED FW MAX SIZE. */
#define CMICX_LED_FW_SIZE_MAX                (1024)

/* Maximum number of ports supported by device */
#define LED_MAX_PORT 128

#define CMICX_LED_SHMEM_SIZE            (2048 + 64)
#define CMICX_LED_SHMEM_OFFSET_DEFAULT  (2048 + 512)

#define LED_STOP 0
#define LED_START 1

typedef enum soc_led_msg_id_e {
    LED_MSG_LNK_STS = 0,
    LED_MSG_STATUS,
    LED_MSG_ENABLE,
    LED_MSG_STATUS_SET,
    LED_MSG_SPEED,
    LED_MSG_CONTROL_DATA_WRITE,
    LED_MSG_CONTROL_DATA_READ,
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

typedef struct soc_led_control_data_s {
    uint32 offset;
    uint32 data;
}__attribute__ ((packed)) soc_led_control_data_t;

#ifndef CMICX_FW_BUILD
extern int soc_cmicx_led_uc_num_get(int unit, int *led_uc_num);
extern int soc_cmicx_led_fw_load(int unit, int led_uc,const uint8 *data, int len);
extern int soc_cmicx_led_fw_start_get(int unit, int led_uc, int *start);
extern int soc_cmicx_led_fw_start_set(int unit, int led_uc, int start);
extern int soc_cmicx_led_control_data_write(int unit, int led_uc, int offset,const uint8 *data, int len);
extern int soc_cmicx_led_control_data_read(int unit, int led_uc, int offset, uint8 *data, int len);
extern int soc_cmicx_led_port_to_uc_port_get(int unit, int port, int *led_uc, int *led_uc_port);
extern int soc_cmicx_led_port_to_uc_port_set(int unit, int port, int led_uc, int led_uc_port);
extern void soc_cmicx_led_fw_dump(int unit, int led_uc, int len);
extern int soc_cmicx_led_init(int unit);
extern int soc_cmicx_led_deinit(int unit);
extern int soc_cmicx_led_link_status(int unit, soc_led_link_status_t *link_status);
extern int soc_cmicx_led_status_get(int unit, soc_led_port_status_t *led_port_status);
extern int soc_cmicx_led_status_set(int unit, soc_led_port_status_t *led_port_status);
extern int soc_cmicx_led_enable(int unit, uint32 enable);
extern int soc_cmicx_led_speed(int unit, soc_led_port_speed_t *led_port_speed);
extern int soc_cmicx_led_linkscan_default_fw_load(int unit);
#endif /* !(CMICX_FW_BUILD) */
#endif /* defined(BCM_CMICX_SUPPORT) || defined(CMICX_FW_BUILD) */
#endif /* __CMICX_LED_H__ */
