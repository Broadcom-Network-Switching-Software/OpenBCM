
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_TIME_SW_STATE_ACCESS_H__
#define __DNX_TIME_SW_STATE_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_time_sw_state_types.h>
#include <include/bcm/types.h>



typedef int (*dnx_time_interface_db_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*dnx_time_interface_db_init_cb)(
    int unit);


typedef int (*dnx_time_interface_db_flags_set_cb)(
    int unit, uint32 flags);


typedef int (*dnx_time_interface_db_flags_get_cb)(
    int unit, uint32 *flags);


typedef int (*dnx_time_interface_db_id_set_cb)(
    int unit, int id);


typedef int (*dnx_time_interface_db_id_get_cb)(
    int unit, int *id);


typedef int (*dnx_time_interface_db_heartbeat_hz_set_cb)(
    int unit, int heartbeat_hz);


typedef int (*dnx_time_interface_db_heartbeat_hz_get_cb)(
    int unit, int *heartbeat_hz);


typedef int (*dnx_time_interface_db_clk_resolution_set_cb)(
    int unit, int clk_resolution);


typedef int (*dnx_time_interface_db_clk_resolution_get_cb)(
    int unit, int *clk_resolution);


typedef int (*dnx_time_interface_db_bitclock_hz_set_cb)(
    int unit, int bitclock_hz);


typedef int (*dnx_time_interface_db_bitclock_hz_get_cb)(
    int unit, int *bitclock_hz);


typedef int (*dnx_time_interface_db_status_set_cb)(
    int unit, int status);


typedef int (*dnx_time_interface_db_status_get_cb)(
    int unit, int *status);




typedef struct {
    dnx_time_interface_db_flags_set_cb set;
    dnx_time_interface_db_flags_get_cb get;
} dnx_time_interface_db_flags_cbs;


typedef struct {
    dnx_time_interface_db_id_set_cb set;
    dnx_time_interface_db_id_get_cb get;
} dnx_time_interface_db_id_cbs;


typedef struct {
    dnx_time_interface_db_heartbeat_hz_set_cb set;
    dnx_time_interface_db_heartbeat_hz_get_cb get;
} dnx_time_interface_db_heartbeat_hz_cbs;


typedef struct {
    dnx_time_interface_db_clk_resolution_set_cb set;
    dnx_time_interface_db_clk_resolution_get_cb get;
} dnx_time_interface_db_clk_resolution_cbs;


typedef struct {
    dnx_time_interface_db_bitclock_hz_set_cb set;
    dnx_time_interface_db_bitclock_hz_get_cb get;
} dnx_time_interface_db_bitclock_hz_cbs;


typedef struct {
    dnx_time_interface_db_status_set_cb set;
    dnx_time_interface_db_status_get_cb get;
} dnx_time_interface_db_status_cbs;


typedef struct {
    dnx_time_interface_db_is_init_cb is_init;
    dnx_time_interface_db_init_cb init;
    
    dnx_time_interface_db_flags_cbs flags;
    
    dnx_time_interface_db_id_cbs id;
    
    dnx_time_interface_db_heartbeat_hz_cbs heartbeat_hz;
    
    dnx_time_interface_db_clk_resolution_cbs clk_resolution;
    
    dnx_time_interface_db_bitclock_hz_cbs bitclock_hz;
    
    dnx_time_interface_db_status_cbs status;
} dnx_time_interface_db_cbs;





extern dnx_time_interface_db_cbs dnx_time_interface_db;

#endif 
