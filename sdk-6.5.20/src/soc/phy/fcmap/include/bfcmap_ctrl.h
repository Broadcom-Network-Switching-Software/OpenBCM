/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef __BFCMAP_CTRL_H
#define __BFCMAP_CTRL_H

/*#include <bfcmap_util.h>*/

struct bfcmap_port_ctrl_s;
struct bfcmap_dev_ctrl_s;
struct bfcmap_api_table_s;
struct bfcmap_counter_info_s;
struct bfcmap_counter_cb_s;

typedef struct bfcmap_port_ctrl_s {
    int                         mdev;  /*  bfcmap unit             */
    int                         mport;  /* bfcmap port              */
    bfcmap_port_t              portId;
    bfcmap_port_config_t       cfg;
    bfcmap_dev_io_f            msec_io_f;
    struct bfcmap_dev_ctrl_s   *parent;
    bfcmap_dev_addr_t          dev_addr;
    bfcmap_dev_addr_t          uc_dev_addr;
    struct bfcmap_api_table_s   *api;
    bfcmap_sal_lock_t           plock; /* Port lock */

#define BFCMAP_CTRL_FLAG_ATTACHED           0x00000001
#define BFCMAP_CTRL_FLAG_SHR_MEM_INITED     0x00000002
    unsigned int                f;      /* flags                    */

    struct bfcmap_int_hw_mapper_s *mp;
    void                        *fcmap_port_cfg;

} bfcmap_port_ctrl_t;

#define BFCMAP_PORT(c)                  (c)->mport
#define BFCMAP_UNIT(c)                  (c)->mdev
#define BFCMAP_PORT_DEVICE_CONTROL(c)   (c)->parent
#define BFCMAP_PORT_FLAGS(c)   (c)->f

#define BFCMAP_EGRESS_CHAN_TBL(c)       (c)->e_chans
#define BFCMAP_INGRESS_CHAN_TBL(c)      (c)->i_chans

#define BFCMAP_CHAN_GET_TBL(mpc,d)                           \
    (((d) == BFCMAP_DIR_INGRESS) ? (mpc)->i_chans : (mpc)->e_chans)

#define BFCMAP_PORT_GET_MAX_CHAN(mpc,d)                      \
    (((d) == BFCMAP_DIR_INGRESS) ? (mpc)->max_ingress_chan : (mpc)->max_egress_chan)

#define BFCMAP_CHAN_ENTRY(t, i)     *((t) + (i))

#define BFCMAP_PORT_CONFIG(mpc)          (&(mpc)->cfg)

#define BFCMAP_LOCK_PORT(mpc)         BFCMAP_SAL_LOCK((mpc)->plock)

#define BFCMAP_UNLOCK_PORT(mpc)       BFCMAP_SAL_UNLOCK((mpc)->plock)

#define BFCMAP_CHAN_VEC(mpc)   ((mpc)->chan_vec)


/* BFCMAP unit control structure. This keeps track
 * of all the ports attached to a bfcmap device.*/
typedef struct bfcmap_dev_ctrl_s {
    int                     mdev;      /* bfcmap unit                  */
    int                     num_ports;  /* Number of port on this device*/
    bfcmap_dev_addr_t      dev_addr;
    bfcmap_core_t          core_type;
    bfcmap_port_ctrl_t     *ports;     /* bfcmap_port_ctrl_t port ctrl tbl  */

#define BFCMAP_UNIT_ATTACHED        0x0001
    unsigned int            flags;      /* Device flags                 */
    bfcmap_sal_lock_t       ulock;

    bfcmap_counter_info_t *counter_info;
    struct bfcmap_counter_cb_s   *counter_cb;
    bfcmap_port_t          p; /* bfcmap port containing switch unit and port */
    struct bfcmap_mibs_s   *mibs;      /* FC MIBs structure */
    struct bfcmap_mibs_s   *prev_mibs; /* Previous FC MIBs structure */
} bfcmap_dev_ctrl_t;

extern bfcmap_dev_ctrl_t   *bfcmap_unit_cb[BFCMAP_MAX_UNITS];

#define BFCMAP_DEVICE_CTRL(md)    bfcmap_unit_cb[(md)]

#define BFCMAP_LOCK_DEVICE(mdc)    BFCMAP_SAL_LOCK((mdc)->ulock)

#define BFCMAP_UNLOCK_DEVICE(mdc)  BFCMAP_SAL_UNLOCK((mdc)->ulock)

#define BFCMAP_DEVICE_NUM_PORTS(mdc)    ((mdc)->num_ports)

#define BFCMAP_UNIT_PORT_CONTROL(mdc, p)    ((mdc)->ports + (p))

#endif /* __BFCMAP_CTRL_H */

