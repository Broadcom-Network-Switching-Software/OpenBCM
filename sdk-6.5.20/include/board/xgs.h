/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        xgs.h
 * Purpose:     Public XGS board driver delarations
 */

#ifndef   _XGS_H_
#define   _XGS_H_

/* High level XGS board support */

#include <board/board.h>

extern char *board_xgs_description(board_driver_t *driver);
extern int board_xgs_probe(board_driver_t *driver, int num,
                                  bcm_info_t *info);
extern int board_xgs_start(board_driver_t *driver, uint32 flags);
extern int board_xgs_stop(board_driver_t *driver);
extern int board_xgs_modid_get(board_driver_t *driver,
                                      char *name, int *modid);
extern int board_xgs_modid_set(board_driver_t *driver,
                                      char *name, int modid);
extern int board_xgs_num_modid_get(board_driver_t *driver,
                                          char *name, int *num_modid);

#define BOARD_DRIVER_ANY_DEVICE 0xffffffff

#define BOARD_DRIVER_PRIVATE_DATA(driver) \
  ((void *)& _ ## driver ## _private_data)

#define BOARD_DRIVER_STRUCTURE(driver) driver ## _board

/* Cached device data */
typedef struct board_xgs_device_data_s {
    bcm_info_t info;
    int modid;
    int num_modid;
    int modid_mask;
} board_xgs_device_data_t;

typedef struct board_xgs_data_s {
    /* declaration
       populated at compile time
       do not reorder */
    int num_device; /* num_unit? */
    int num_supported_device;
    uint32 *supported_device;
    int num_led_program;
    uint8 *led_program;
    int led_port_offset; /* this is LED program dependent */
    int num_internal_connection;
    board_connection_t *internal_connection;

    /* instantiation
       populated at run time
       no ordering requirements */
    char *description;
    board_xgs_device_data_t *device; /* size: num_devices */
    bcm_gport_t cpu;
} board_xgs_data_t;

#define ARRAY(a) COUNTOF(a), a

#define BOARD_XGS_DRIVER(driver) \
board_driver_t BOARD_DRIVER_STRUCTURE(driver) = { \
  #driver, \
  BOARD_DEFAULT_PRIORITY, \
  BOARD_DRIVER_PRIVATE_DATA(driver),\
  0,\
  NULL,\
  0,\
  NULL,\
  board_xgs_description,\
  board_xgs_probe,\
  board_xgs_start,\
  board_xgs_stop \
}


#endif /* _XGS_H_ */
