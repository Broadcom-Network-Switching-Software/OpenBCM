/*
* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*
* $File:  trident3_tdm_flexport.h
*/


#ifndef _SOC_TRIDENT3_TDM_FLEXPORT_H_
#define _SOC_TRIDENT3_TDM_FLEXPORT_H_


extern int soc_trident3_tdm_calculation_flexport(int unit, 
    soc_port_schedule_state_t *port_schedule_state);

extern int
soc_trident3_tdm_flexport_remove_ports(int unit, soc_port_schedule_state_t *port_schedule_state);

extern int
soc_trident3_tdm_flexport_ovs_consolidate(int unit, soc_port_schedule_state_t *port_schedule_state);

extern int
soc_trident3_tdm_flexport_add_ports(int unit, soc_port_schedule_state_t *port_schedule_state);

extern int
tdm_td3_alloc_prev_chip_data(tdm_mod_t *_tdm_pkg);

extern int
tdm_td3_free_prev_chip_data(tdm_mod_t *_tdm_pkg);

extern int
soc_trident3_tdm_flexport_idb(int unit, soc_port_schedule_state_t *port_schedule_state);

#endif

