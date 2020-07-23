/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RX LOS
 */

#ifndef _RX_LOS_DB_H_INCLUDED_
#define _RX_LOS_DB_H_INCLUDED_

#include <sal/core/time.h>
#include <bcm/types.h>
#include <soc/types.h>
#include <bcm/port.h>
#include <appl/dcmn/rx_los/rx_los.h>

/* the next functions require database to be locked when called */

int rx_los_db_state_name_get(rx_los_state_t state, char **name);

uint32 rx_los_db_long_sleep_get(void);
uint32 rx_los_db_short_sleep_get(void);
uint32 rx_los_db_active_sleep_get(void);
uint32 rx_los_db_allowed_retries_get(void);
uint32 rx_los_db_thread_priority_get(void);

int rx_los_db_set_config(uint32 short_sleep_usec, uint32 long_sleep_usec, uint32 allowed_retries, uint32 priority, int link_down_count_max, int link_down_cycle_time);

int rx_los_db_set_active_sleep_config(uint32 active_sleep_usec);

int rx_los_db_units_pbmp_get(pbmp_t *pbmp);

int rx_los_db_move_link_to_support(int unit, int port);
int rx_los_db_remove_link_from_support(int unit, int port);

int rx_los_db_remove_link_from_steady(int unit, int port);
int rx_los_db_move_link_to_steady(int unit, int port);

int rx_los_db_link_support_enable(int unit, int port, int enable);

/* the next functions require database to be locked when called,
   and require the function rx_los_db_unit_null_check to be called and checked before they are used */
int rx_los_db_not_steady_state_pbmp_get(int unit, pbmp_t *pbmp);
int rx_los_db_supported_pbmp_get(int unit, pbmp_t *pbmp);

rx_los_state_t rx_los_db_state_get(int unit, int port);
int rx_los_db_state_set(int unit,int port, rx_los_state_t state);

sal_usecs_t rx_los_db_timestamp_get(int unit, int port);
void rx_los_db_timestamp_update(int unit, int port);

int rx_los_db_tries_get(int unit, int port);

void rx_los_db_tries_initialize(int unit, int port);
void rx_los_db_tries_increment(int unit, int port);

int rx_los_db_unit_null_check(int unit);

/* first init of DB */
void rx_los_db_init(void);

int rx_los_db_unit_init(int unit, soc_pbmp_t links_pbmp);
int rx_los_db_unit_deinit(int unit);

/* user prints */
int rx_los_db_dump(int unit);
int rx_los_db_dump_port(int unit, bcm_port_t port);

/*
 *Recurring detection 
 */
/*link down*/
int rx_los_db_link_down_cycle_get(int unit, bcm_port_t port, sal_usecs_t *cycle_start, uint32 *cycle_count);
int rx_los_db_link_down_cycle_set(int unit, bcm_port_t port, sal_usecs_t cycle_start, uint32 cycle_count);

int rx_los_db_link_down_cycle_config_get(int unit, bcm_port_t port, sal_usecs_t *cycle_start, uint32 *cycle_count);
int rx_los_db_link_down_cycle_config_set(int unit, bcm_port_t port, sal_usecs_t cycle_start, uint32 cycle_count);
/*rx seq change*/
int rx_los_db_rx_seq_change_cycle_get(int unit, bcm_port_t port, sal_usecs_t *cycle_start, uint32 *cycle_count);
int rx_los_db_rx_seq_change_cycle_set(int unit, bcm_port_t port, sal_usecs_t cycle_start, uint32 cycle_count);

int rx_los_db_rx_seq_change_cycle_config_get(int unit, bcm_port_t port, sal_usecs_t *cycle_start, uint32 *cycle_count);
int rx_los_db_rx_seq_change_cycle_config_set(int unit, bcm_port_t port, sal_usecs_t cycle_start, uint32 cycle_count);

int rx_los_db_register(int unit, rx_los_callback_t callback);
int rx_los_db_callback_get(int unit, rx_los_callback_t *callback);
#endif /* _RX_LOS_DB_H_INCLUDED_ */





