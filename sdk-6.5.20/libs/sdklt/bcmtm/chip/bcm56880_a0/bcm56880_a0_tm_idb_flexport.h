/*! \file bcm56880_a0_tm_idb_flexport.h
 *
 * File contains IDB init and flexport related defines for bcm56880_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56880_A0_TM_IDB_FLEXPORT_H
#define BCM56880_A0_TM_IDB_FLEXPORT_H

#define TD4_CA_BUFFER_SIZE             64
#define TD4_OBM_PARAMS_LOSSY_INDEX                     0
#define TD4_OBM_PARAMS_LOSSLESS_INDEX                  1
#define TD4_OBM_PARAMS_LOSSY_PLUS_LOSSLESS_INDEX       2
#define TD4_OBM_PARAMS_DROP_COUNT_SELECT_LOSSLESS0     2
#define TD4_OBM_PARAMS_DROP_COUNT_SELECT_LOSSY_LO      0
#define TD4_OBM_PRIORITY_LOSSY_LO      0
#define TD4_OBM_PRIORITY_LOSSY_HI      1
#define TD4_OBM_PRIORITY_LOSSLESS0     2
#define TD4_OBM_PRIORITY_LOSSLESS1     3

#define TD4_TDM_SCHED_SPECIAL_SLOT      18

#define TD4_PIPE_HAS_CPU_MGMT(p) (p==0 || p==1 || p==3 || p==5 || p==7)
#define TD4_PIPE_HAS_LPBK(p)             (p==1 || p==3 || p==5 || p==7)
#define TD4_PHY_PORT_CPU               0
#define TD4_PHY_PORT_MNG0              257
#define TD4_PHY_PORT_MNG1              258
#define TD4_PHY_PORT_MNG2              259
#define TD4_PHY_PORT_MNG3              260
#define TD4_PHY_PORT_LPBK0             261
#define TD4_PHY_PORT_LPBK1             262
#define TD4_PHY_PORT_LPBK2             263
#define TD4_PHY_PORT_LPBK3             264

typedef enum {
    TDM_IDB_PORT_SCHED,
    TDM_IDB_PKT_SCHED,
} idb_sch_inst_name_e;

extern int td4_port_idb_up(int unit, int *lport_bmp);
extern int td4_port_idb_down(int unit, int *lport_bmp);

extern int
td4_reset_idb(int unit, int *reset_bmp, int reset_value);
#endif /* BCM56880_A0_TM_IDB_FLEXPORT_H */
