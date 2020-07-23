/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        firelight_tdm.h
 */

#ifndef _SOC_FIRELIGHT_TDM_H_
#define _SOC_FIRELIGHT_TDM_H_

#include <soc/esw/port.h>

/*** START SDK API COMMON CODE ***/

/* FL specific defines */
#define FIRELIGHT_TDM_PHY_PORTS_PER_DEV 90
#define FIRELIGHT_TDM_LOG_PORTS_PER_DEV 66


/*** START SDK API COMMON CODE ***/
extern void _soc_fl_tdm_print_port_map(int unit,
                                        soc_port_map_type_t *port_map);
extern void _soc_fl_tdm_print_tdm_info(int unit, soc_tdm_schedule_t *tdm_info);
extern void _soc_fl_tdm_print_schedule_state(int unit,
                                 soc_port_schedule_state_t *sch_info);
extern int _soc_fl_tdm_calculation(int unit,
                                 soc_port_schedule_state_t *sch_info);
extern int _soc_fl_tdm_set_idb_calendar(int unit,
                                 soc_port_schedule_state_t *sch_info);

extern int soc_fl_tdm_init(int unit,
                            soc_port_schedule_state_t *schedule_state);

extern void soc_fl_port_schedule_speed_remap(
                int unit,
                soc_port_schedule_state_t *port_schedule_state);

/*** END SDK API COMMON CODE ***/

#endif /* !_SOC_FIRELIGHT_TDM_H_ */
