/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BFCMAP_IDENT_H
#define BFCMAP_IDENT_H

struct bfcmap_chip_info_s;
struct bfcmap_port_ctrl_s;
struct bfcmap_dev_ctrl_s;
struct bfcmap_api_table_s;
struct bfcmap_counter_info_s;
struct bfcmap_counter_cb_s;

typedef struct bfcmap_drv_tbl_entry_s {
        char                        *name;
        bfcmap_core_t              id;
        int                         num_port;
        struct bfcmap_api_table_s   *apis;
} bfcmap_drv_tbl_entry_t;
 
/* Returns the bfcmap driver entry corresponding to device core dev_core */
bfcmap_drv_tbl_entry_t* _bfcmap_find_driver(bfcmap_core_t dev_core);

extern int _bfcmap_get_device(bfcmap_dev_addr_t dev_addr);
/*
 * Return mdev for device corresponding to its address.
 */
#define BFCMAP_GET_DEVICE_FROM_DEVADDR(a)  _bfcmap_get_device((a))

/*
 * Adds mapping between bfcmap_port_t and bfcmap_port_ctrl_t */
extern int _bfcmap_add_port(bfcmap_port_t p, struct bfcmap_port_ctrl_s *mpc);

#define BFCMAP_ADD_PORT(p, mpc)      _bfcmap_add_port((p), (mpc))

extern int _bfcmap_remove_port(bfcmap_port_t p);

#define BFCMAP_REMOVE_PORT(p)      _bfcmap_remove_port((p))

/*
 * Return bfcmap_port_ctrl_t corresponding to port.
 */
extern struct bfcmap_port_ctrl_s * _bfcmap_get_port_ctrl(bfcmap_port_t p);

#define BFCMAP_GET_PORT_CONTROL_FROM_PORTID(p)  \
                                    _bfcmap_get_port_ctrl((p))

/*
 * Return bfcmap_port_t corresponding to bfcmap_port_ctrl_t.
 */
extern bfcmap_port_t _bfcmap_get_port_id(struct bfcmap_port_ctrl_s *mpc);

#define BFCMAP_GET_PORT_ID(mpc)  (mpc)->portId

#endif /* BFCMAP_IDENT_H */

