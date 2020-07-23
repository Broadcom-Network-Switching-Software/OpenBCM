/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        support.h
 * Purpose:     Public board support functions
 */

#ifndef   _BOARD_SUPPORT_H_
#define   _BOARD_SUPPORT_H_

#include <board/board.h>

extern int board_connections_discover(int max_num,
                                      board_connection_t *connection,
                                      int *actual);
extern int board_connection_setup(void);
extern int board_device_cpu_port_get(int unit, bcm_gport_t *cpu);
extern int board_device_init(int unit);
extern int board_device_deinit(int unit);
extern int board_port_init(int unit);
extern int board_port_deinit(int unit);
extern const char *board_device_name(int unit);

/* I2C */
#if defined(INCLUDE_I2C)
typedef void (*board_i2c_device_cb)(int unit, int idx,
                                    int devid, char *name, char *desc,
                                    void *user_data);
extern int board_i2c_device_init(int unit);
extern int board_i2c_device_traverse(int unit,
                                     board_i2c_device_cb func,
                                     void *user_data);
#endif

#endif /* _BOARD_SUPPORT_H_ */
