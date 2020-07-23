/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       tomahawk3_cdmac_flexport.h
 */



#ifndef _SOC_TOMAHAWK3_CDMAC_FLEXPORT_H_
#define _SOC_TOMAHAWK3_CDMAC_FLEXPORT_H_

#include <soc/init/tomahawk3_flexport_defines.h>
#include <soc/init/tomahawk3_set_tdm.h>


/*** START SDK API COMMON CODE ***/

extern int soc_tomahawk3_flex_mac_rx_port_down(
    int unit, soc_port_schedule_state_t *port_schedule_state);

extern int soc_tomahawk3_flex_mac_tx_port_down(
    int unit, soc_port_schedule_state_t *port_schedule_state);

extern int soc_tomahawk3_flex_mac_port_up(
    int unit, soc_port_schedule_state_t *port_schedule_state);

extern int soc_tomahawk3_flex_mac_enable(
    int unit, soc_port_schedule_state_t *port_schedule_state);

extern int soc_tomahawk3_flex_get_pm_state(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    int pm_num,
    int *pm_is_up_before_flex,
    int *pm_is_up_after_flex);

extern int soc_tomahawk3_flex_tsc(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    int is_down);

extern int soc_tomahawk3_flex_tsc_down(
    int unit,
    soc_port_schedule_state_t *port_schedule_state);

extern int soc_tomahawk3_flex_tsc_up(
    int unit,
    soc_port_schedule_state_t *port_schedule_state);

/*** END SDK API COMMON CODE ***/

#endif
