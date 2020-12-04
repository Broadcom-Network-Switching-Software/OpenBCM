/*
* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*
* $File:  tomahawk2_tdm_flexport.h
*/


#ifndef TOMAHAWK2_TDM_FLEXPORT_H
#define TOMAHAWK2_TDM_FLEXPORT_H


extern int soc_tomahawk2_tdm_calculation_flexport (
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int soc_tomahawk2_tdm_flexport_remove_ports (
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int soc_tomahawk2_tdm_flexport_ovs_consolidate (
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int soc_tomahawk2_tdm_flexport_add_ports (
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int soc_tomahawk2_tdm_set_out_map (
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int
soc_tomahawk2_tdm_flexport_mmu(
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int
soc_tomahawk2_tdm_flexport_idb(
    int unit, soc_port_schedule_state_t *port_schedule_state);


#endif
