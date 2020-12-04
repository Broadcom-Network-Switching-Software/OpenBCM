/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       tomahawk3_port_init.h
 */



#ifndef _SOC_TOMAHAWK3_PORT_INIT_H_
#define _SOC_TOMAHAWK3_PORT_INIT_H_

#include <soc/init/tomahawk3_flexport_defines.h>
#include <soc/init/tomahawk3_set_tdm.h>


/*** START SDK API COMMON CODE ***/

extern int soc_tomahawk3_tscbh_reset(
    int unit, int pmnum, int reset);

extern int soc_tomahawk3_cdmac_reset(
    int unit, soc_port_map_type_t *port_map, int pmnum, int reset);

extern int soc_tomahawk3_set_cdmac_port_mode(
    int unit, soc_port_map_type_t *port_map, int pmnum);

extern int soc_tomahawk3_cdmac_enable(
    int unit, int phy_port);

extern void soc_tomahawk3_port_mode_get(soc_port_map_type_t *port_map,
    int pmnum, int *cdmac0_port_mode,
    int *cdmac1_port_mode, int *speed_400g);

extern int soc_tomahawk3_tsc_clock_enable(int unit,
    soc_port_schedule_state_t *port_schedule_state);

extern int soc_tomahawk3_pm8x50_port_init(
    int unit, soc_port_schedule_state_t *port_schedule_state);

/*** END SDK API COMMON CODE ***/

#endif


