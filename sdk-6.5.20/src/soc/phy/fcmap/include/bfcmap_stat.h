/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BFCMAP_STATS_H
#define BFCMAP_STATS_H

struct bfcmap_port_ctrl_s;
struct bfcmap_dev_ctrl_s;
struct bfcmap_api_table_s;
struct bfcmap_counter_info_s;
struct bfcmap_counter_cb_s;

typedef struct bfcmap_counter_info_s {
    bfcmap_stat_t    stat;
    buint32_t           address;
    buint16_t           delta;

#define BFCMAP_COUNTER_F_PORT           0x01
#define BFCMAP_COUNTER_F_SECURE_CHAN    0x02
#define BFCMAP_COUNTER_F_SECURE_ASSOC   0x04
#define BFCMAP_COUNTER_F_FLOW           0x08

#define BFCMAP_COUNTER_F_MEM            0x10
#define BFCMAP_COUNTER_F_REG            0x20

#define BFCMAP_COUNTER_F_SIZE32         0x100
#define BFCMAP_COUNTER_F_SIZE64         0x200

#define BFCMAP_COUNTER_F_SW             0x1000
    buint16_t  flags;
} bfcmap_counter_info_t;

#define BFCMAP_COUNTER_F_NOT_IMPLEMENTED    0x8000

/*
 * The following structure contains helper functions which are provided
 * by the chip implementation to common counter framework to comute the
 * correct counter address.
 */
typedef int (*counter_get_assoc_idx)(struct bfcmap_port_ctrl_s*, int assocId);
typedef int (*counter_get_chan_idx)(struct bfcmap_port_ctrl_s*, int chanId);

typedef struct bfcmap_counter_cb_s {
    int                     num_chan;
    counter_get_chan_idx    get_chan_idx;
    int                     num_assoc;
    counter_get_assoc_idx   get_assoc_idx;
    int      (*hw_clear_port_counter)(struct bfcmap_port_ctrl_s*);
    int      (*sw_clear_port_counter)(struct bfcmap_port_ctrl_s*);
    int      (*sw_get_counter)(struct bfcmap_port_ctrl_s*, bfcmap_counter_info_t*,
                               int, int, buint32_t*, buint64_t*);
    int      (*sw_set_counter)(struct bfcmap_port_ctrl_s*, bfcmap_counter_info_t*,
                               int, int, buint32_t*, buint64_t*);
} bfcmap_counter_cb_t;


extern int 
bfcmap_int_stat_clear(struct bfcmap_port_ctrl_s *mpc);

extern int 
bfcmap_int_stat_get(struct bfcmap_port_ctrl_s *mpc, bfcmap_stat_t stat, 
                        int chanId, int assocId, buint64_t *val);

extern int 
bfcmap_int_stat_get32(struct bfcmap_port_ctrl_s *mpc, bfcmap_stat_t stat, 
                          int chanId, int assocId, buint32_t *val);

extern int 
bfcmap_int_stat_set(struct bfcmap_port_ctrl_s *mpc, bfcmap_stat_t stat, 
                            int chanId, int assocId,  buint64_t val);

extern int 
bfcmap_int_stat_set32(struct bfcmap_port_ctrl_s *mpc,bfcmap_stat_t stat, 
                             int chanId, int assocId, buint32_t val);

extern bfcmap_counter_info_t *
bfcmap_int_stat_tbl_create(struct bfcmap_dev_ctrl_s*, 
                                 bfcmap_counter_info_t *stats_tbl);

#endif /* BFCMAP_STATS_H */

