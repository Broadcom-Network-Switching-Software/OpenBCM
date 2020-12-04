/*! \file bcm56780_a0_tm_idb_flexport.h
 *
 * File contains IDB init and flexport related defines for bcm56780_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56780_A0_TM_IDB_FLEXPORT_H
#define BCM56780_A0_TM_IDB_FLEXPORT_H

#define TD4_X9_CA_BUFFER_SIZE 64
#define TD4_X9_SWITCH_BYPASS_MODE_LOW 2
#define TD4_X9_OBM_PARAMS_LOSSY_INDEX 0
#define TD4_X9_OBM_PARAMS_LOSSLESS_INDEX 1
#define TD4_X9_OBM_PARAMS_LOSSY_PLUS_LOSSLESS_INDEX 2
#define TD4_X9_OBM_PARAMS_DROP_COUNT_SELECT_LOSSLESS0 2
#define TD4_X9_OBM_PARAMS_DROP_COUNT_SELECT_LOSSY_LO 0
#define TD4_X9_IDB_CA_PEEK_DEPTH_FRONT_PANEL_PORT 6
#define TD4_X9_IDB_CA_PEEK_DEPTH_CPU_LPBK_PORT 2

#define TD4_X9_OBM_PRIORITY_LOSSY_LO 0
#define TD4_X9_OBM_PRIORITY_LOSSY_HI 1
#define TD4_X9_OBM_PRIORITY_LOSSLESS0 2
#define TD4_X9_OBM_PRIORITY_LOSSLESS1 3

#define TD4_X9_TDM_LENGTH 41
#define TD4_X9_TDM_SCHED_SPECIAL_SLOT 18

#define TD4_X9_PIPE_HAS_CPU_MGMT(p) (p==0 || p==1 || p==3)
#define TD4_X9_PIPE_HAS_LPBK(p) (p==1 || p==3)
#define TD4_X9_PIPE_HAS_RDB(p) (p==0  || p==2)
#define TD4_X9_PHY_PORT_CPU 0
#define TD4_X9_PHY_PORT_MNG0 161
#define TD4_X9_PHY_PORT_MNG1 162
#define TD4_X9_PHY_PORT_LPBK0 163
#define TD4_X9_PHY_PORT_LPBK1 164
#define TD4_X9_PHY_PORT_RDB0 165
#define TD4_X9_PHY_PORT_RDB1 166

typedef enum {
    TDM_IDB_PORT_SCHED,
    TDM_IDB_PKT_SCHED,
} idb_sch_inst_name_e;

/*!
 * \brief Get the pipe port macro number from physical port.
 *
 * \param [in] pport Physical port number.
 *
 * \return Return pipe port macro number.
 */
extern int
bcm56780_a0_tm_get_pipe_pm(int pport);

/*!
 * \brief Get the sub port from physical port.
 *
 * \param [in] pport Physical port number.
 *
 * \return Return pipe port macro number.
 */
extern int
bcm56780_a0_tm_get_subport (int pport);

#endif /* BCM56780_A0_TM_IDB_FLEXPORT_H */
