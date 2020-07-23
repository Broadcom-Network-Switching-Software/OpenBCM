/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _SOC_LED_H
#define _SOC_LED_H

#include <sal/types.h>

/*
 *   SOC LED APIs.
 */

/* Initialize LED driver. */
extern int
soc_led_driver_init(int unit);

/* Read control data of LED firmware. */
extern int
soc_led_control_data_read(int unit, int led_uc, int offset, uint8 *data, int len);

/* Write control data of LED firmware. */
extern int
soc_led_control_data_write(int unit, int led_uc, int offset, const uint8 *data, int len);

/* Load LED firmware binary. */
extern int
soc_led_fw_load(int unit, int led_uc, const uint8 *data, int led);

/* Get if LED firmware is started or not. */
extern int
soc_led_fw_start_get(int unit, int led_uc, int *start);

/* Start/stop LED firmware. */
extern int
soc_led_fw_start_set(int unit, int led_uc, int start);

/* Map physical port to LED microcontroller number and port index. */
extern int
soc_led_port_to_uc_port_get(int unit, int port, int *led_uc, int *led_uc_port);

/* Configure the mapping from physical port to LED microcontroller number and port index. */
extern int
soc_led_port_to_uc_port_set(int unit, int port, int led_uc, int led_uc_port);

/* Get number of LED microcontrollers. */
extern int
soc_led_uc_num_get(int unit, int *led_uc_num);

/* Restore LED as warmboot on. */
extern int
soc_led_wb_data_restore(int unit);

/*
 * Driver internal definition.
 */
#define LED_DRIVER_CALL(_f, _a) ((SOC_LED_DRIVER(unit) == NULL) || \
                                 (SOC_LED_DRIVER(unit)->_f == NULL) ) \
                                 ? SOC_E_UNAVAIL : (SOC_LED_DRIVER(unit)->_f _a)

#define SOC_LED_DRIVER_INIT(unit) soc_led_driver_init(unit)

#define SOC_LED_CONTROL_DATA_READ(unit, led_uc, offset, pdata, len)  \
        LED_DRIVER_CALL(soc_led_control_data_read, (unit, led_uc, offset, pdata, len));

#define SOC_LED_CONTROL_DATA_WRITE(unit, led_uc, offset, pdata, len)  \
        LED_DRIVER_CALL(soc_led_control_data_write, (unit, led_uc, offset, pdata, len));

#define SOC_LED_FW_LOAD(unit, led_uc, pdata, len) \
        LED_DRIVER_CALL(soc_led_fw_load, (unit, led_uc, pdata, len)) \

#define SOC_LED_FW_START_GET(unit, led_uc, pdata) \
        LED_DRIVER_CALL(soc_led_fw_start_get, (unit, led_uc, pdata))

#define SOC_LED_FW_START_SET(unit, led_uc, data) \
        LED_DRIVER_CALL(soc_led_fw_start_set, (unit, led_uc, data))

#define SOC_LED_PORT_TO_UC_PORT_GET(unit, port, pled_uc, pled_uc_port) \
        LED_DRIVER_CALL(soc_led_port_to_uc_port_get, (unit, port, pled_uc, pled_uc_port))

#define SOC_LED_PORT_TO_UC_PORT_SET(unit, port, led_uc, led_uc_port) \
        LED_DRIVER_CALL(soc_led_port_to_uc_port_set, (unit, port, led_uc, led_uc_port))

#define SOC_LED_UC_NUM_GET(unit, led_uc_num) \
        LED_DRIVER_CALL(soc_led_uc_num_get, (unit, led_uc_num))

typedef int (*soc_led_uc_num_get_f) (int, int *);
typedef int (*soc_led_fw_load_f) (int, int,const uint8 *, int);
typedef int (*soc_led_fw_start_get_f) (int, int, int *);
typedef int (*soc_led_fw_start_set_f) (int, int, int);
typedef int (*soc_led_control_data_write_f) (int, int, int, const uint8 *, int);
typedef int (*soc_led_control_data_read_f) (int, int, int, uint8 *, int);
typedef int (*soc_led_port_to_uc_port_get_f) (int, int, int *, int *);
typedef int (*soc_led_port_to_uc_port_set_f) (int, int, int, int);

/*
 * Typedef: soc_led_driver_t
 * Purpose: LED driver model.
 */
typedef struct soc_led_driver_s {
    soc_led_uc_num_get_f               soc_led_uc_num_get;
    soc_led_fw_load_f                  soc_led_fw_load;
    soc_led_fw_start_get_f             soc_led_fw_start_get;
    soc_led_fw_start_set_f             soc_led_fw_start_set;
    soc_led_control_data_write_f       soc_led_control_data_write;
    soc_led_control_data_read_f        soc_led_control_data_read;
    soc_led_port_to_uc_port_get_f      soc_led_port_to_uc_port_get;
    soc_led_port_to_uc_port_set_f      soc_led_port_to_uc_port_set;
} soc_led_driver_t;

/* To check if led driver is supported. */
#define SOC_LED_DRIVER_SUPPORT(unit)       (SOC_CONTROL(unit)->soc_led_drver != NULL)

#endif  /* _SOC_LED_H */
