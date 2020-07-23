/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tomahawk3_set_tdm.h
 */



#ifndef _SOC_TOMAHAWK3_SET_TDM_H_
#define _SOC_TOMAHAWK3_SET_TDM_H_

#include <soc/esw/port.h>
#include <soc/init/tomahawk3_flexport_defines.h>


/*** START SDK API COMMON CODE ***/

extern int soc_tomahawk3_set_tdm_tbl(
    int unit,
    soc_port_schedule_state_t *port_schedule_state);

extern void soc_tomahawk3_set_tdm_tbl_per_pipe(
    int pipe_num,
    soc_port_schedule_state_t *port_schedule_state);

extern int soc_tomahawk3_set_tdm_slot_pos_tbl(
    int phy_port, 
    int speed, 
    int *slot_pos_tbl);

/*** END SDK API COMMON CODE ***/

#endif





