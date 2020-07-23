/*! \file bcm56990_b0_tm_idb_flexport.h
 *
 * File contains IDB init and flexport related defines for bcm56990_b0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56990_A0_TM_IDB_FLEXPORT_H
#define BCM56990_A0_TM_IDB_FLEXPORT_H

#include <bcmtm/bcmtm_types.h>


/*** START SDKLT API COMMON CODE ***/


#define TH4_B0_CA_BUFFER_SIZE             64
#define SOC_SWITCH_BYPASS_MODE_LOW 2
#define TH4_B0_OBM_PARAMS_LOSSY_INDEX                     0
#define TH4_B0_OBM_PARAMS_LOSSLESS_INDEX                  1
#define TH4_B0_OBM_PARAMS_LOSSY_PLUS_LOSSLESS_INDEX       2
#define TH4_B0_OBM_PARAMS_DROP_COUNT_SELECT_LOSSLESS0     2
#define TH4_B0_OBM_PARAMS_DROP_COUNT_SELECT_LOSSY_LO      0
#define TH4_B0_IDB_CA_PEEK_DEPTH_FRONT_PANEL_PORT         6
#define TH4_B0_IDB_CA_PEEK_DEPTH_CPU_LPBK_PORT            2
#define TH4_B0_OBM_PRIORITY_LOSSY_LO      0
#define TH4_B0_OBM_PRIORITY_LOSSY_HI      1
#define TH4_B0_OBM_PRIORITY_LOSSLESS0     2
#define TH4_B0_OBM_PRIORITY_LOSSLESS1     3

#define TH4_B0_TDM_LENGTH     33
#define TH4_B0_TDM_SCHED_SPECIAL_SLOT      16


#define TH4_B0_PHY_PORT_CPU               0
#define TH4_B0_PHY_PORT_MNG0              257
#define TH4_B0_PHY_PORT_MNG1              258
#define TH4_B0_PHY_PORT_LPBK0             259
#define TH4_B0_PHY_PORT_LPBK1             260
#define TH4_B0_PHY_PORT_LPBK2             261
#define TH4_B0_PHY_PORT_LPBK3             262
#define TH4_B0_PHY_PORT_LPBK4             263
#define TH4_B0_PHY_PORT_LPBK5             264
#define TH4_B0_PHY_PORT_LPBK6             265
#define TH4_B0_PHY_PORT_LPBK7             266

typedef enum {
    TDM_IDB_PORT_SCHED,
    TDM_IDB_PKT_SCHED,
} idb_sch_inst_name_e;

#define TH4_PIPE_HAS_CPU(p) (p == 0)
#define TH4_PIPE_HAS_LB(p) (p == 1 || p == 3 || p == 5 || p == 7 || p == 9 ||\
                            p == 11 || p == 13 || p == 15)
#define TH4_PIPE_HAS_MGMT(p) (p == 2 || p == 8)

extern int
bcm56990_b0_tm_idb_general_init(int unit);

extern int
bcm56990_b0_tm_port_idb_add(int unit, bcmtm_pport_t pport);

extern int
bcm56990_b0_tm_port_idb_up(int unit, bcmtm_pport_t pport);

extern int
bcm56990_b0_tm_port_idb_down(int unit, bcmtm_pport_t pport);

extern int
bcm56990_b0_tm_port_idb_delete(int unit, bcmtm_pport_t pport);

/*** END SDKLT API COMMON CODE ***/

#endif /* BCM56990_A0_TM_IDB_FLEXPORT_H */
