/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _PM4X25TD_INT_H_
#define _PM4X25TD_INT_H_

#include <soc/portmod/portmod.h>

int _pm4x25td_pm_enable(int unit, int port, pm_info_t pm_info, int port_index, const portmod_port_add_info_t* add_info, int enable);
int _pm4x25td_pm_port_init(int unit, int port, int internal_port, const portmod_port_add_info_t* add_info, int enable);
int pm4x25td_egr_1588_timestamp_config_set(int unit, int port, pm_info_t pm_info, portmod_egr_1588_timestamp_config_t timestamp_config);
int pm4x25td_egr_1588_timestamp_config_get(int unit, int port, pm_info_t pm_info, portmod_egr_1588_timestamp_config_t* timestamp_config);
#endif /*_PM4X25TD_INT_H_*/
