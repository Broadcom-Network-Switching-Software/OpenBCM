/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        board_int/support.h
 * Purpose:     board support internal library interfaces
 */

#ifndef   _BOARD_INT_SUPPORT_H_
#define   _BOARD_INT_SUPPORT_H_

#include <sal/core/alloc.h>
#include <bcm/types.h>
#include <bcm/init.h>
#include <bcm/trunk.h>

#include <board/board.h>
#include <board/support.h>

#include <bcm_int/common/trunk.h>

/* Board tree */

typedef struct board_tree_driver_s {
    int num_connection;
    board_connection_t *connection;
    void *user_data;
    int (*info)(int unit, int *modid, bcm_trunk_chip_info_t *ti,
                void *user_data);
    int (*edge)(int unit, bcm_trunk_chip_info_t *ti,
                 bcm_trunk_add_info_t *ta, void *user_data);
    int (*vertex)(int unit, int modid, bcm_gport_t port, void *user_data);
} board_tree_driver_t;

typedef int (*board_connection_sp_cb_t)(board_connection_t *conn,
                                        void *user_data);

extern int board_tree_connect(board_tree_driver_t *tree_driver);

extern int board_connection_stack_ports(int max_num,
                                        board_connection_t *connection,
                                        int *actual);
extern int board_connection_add_internal_stack_ports(int count,
                                           board_connection_t *connection);

extern int board_connection_connected_stack_port_traverse(int count,
                                               board_connection_t *connection,
                                               board_connection_sp_cb_t func,
                                               void *user_data);

/* LED */
extern int board_led_program_write(int unit, int len, uint8 *pgm);
extern int board_led_program_read(int unit, int maxlen,
                                  uint8 *pgm, int *actual);
extern int board_led_data_write(int unit, int len, uint8 *data);
extern int board_led_data_read(int unit, int maxlen, uint8 *data, int *actual);
extern int board_led_enable_set(int unit, int value);
extern int board_led_enable_get(int unit, int *value);
extern int board_led_status_get(int unit, int ctl, int *value);

extern int board_led_init(int unit, int offset, int len, uint8 *pgm);
extern int board_led_deinit(int unit);


extern int board_device_reset(int unit);
extern int board_device_info(int unit, bcm_info_t *info);
extern int board_num_devices(void);
extern int board_device_modid_max(int unit);

extern int board_connections_alloc(board_driver_t *driver,
                                   board_connection_t **connection,
                                   int *actual);

extern int board_connection_gport_stackable(bcm_gport_t gport);

extern int board_probe_get(int max_num, bcm_info_t *info, int *actual);


/* Conveniences */
#define ALLOC(size) sal_alloc(size, (char *)FUNCTION_NAME())
#define FREE(ptr)   sal_free(ptr)


#endif /* _BOARD_INT_SUPPORT_H_ */
