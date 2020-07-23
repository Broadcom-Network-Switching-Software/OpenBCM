
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __INTERRUPT_ACCESS_H__
#define __INTERRUPT_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/interrupt_types.h>
#include <soc/types.h>



typedef int (*intr_db_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*intr_db_init_cb)(
    int unit);


typedef int (*intr_db_flags_set_cb)(
    int unit, uint32 flags_idx_0, uint32 flags);


typedef int (*intr_db_flags_get_cb)(
    int unit, uint32 flags_idx_0, uint32 *flags);


typedef int (*intr_db_flags_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);


typedef int (*intr_db_storm_timed_count_set_cb)(
    int unit, uint32 storm_timed_count_idx_0, uint32 storm_timed_count);


typedef int (*intr_db_storm_timed_count_get_cb)(
    int unit, uint32 storm_timed_count_idx_0, uint32 *storm_timed_count);


typedef int (*intr_db_storm_timed_count_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);


typedef int (*intr_db_storm_timed_period_set_cb)(
    int unit, uint32 storm_timed_period_idx_0, uint32 storm_timed_period);


typedef int (*intr_db_storm_timed_period_get_cb)(
    int unit, uint32 storm_timed_period_idx_0, uint32 *storm_timed_period);


typedef int (*intr_db_storm_timed_period_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);


typedef int (*intr_db_storm_nominal_set_cb)(
    int unit, uint32 storm_nominal);


typedef int (*intr_db_storm_nominal_get_cb)(
    int unit, uint32 *storm_nominal);




typedef struct {
    intr_db_flags_set_cb set;
    intr_db_flags_get_cb get;
    intr_db_flags_alloc_cb alloc;
} intr_db_flags_cbs;


typedef struct {
    intr_db_storm_timed_count_set_cb set;
    intr_db_storm_timed_count_get_cb get;
    intr_db_storm_timed_count_alloc_cb alloc;
} intr_db_storm_timed_count_cbs;


typedef struct {
    intr_db_storm_timed_period_set_cb set;
    intr_db_storm_timed_period_get_cb get;
    intr_db_storm_timed_period_alloc_cb alloc;
} intr_db_storm_timed_period_cbs;


typedef struct {
    intr_db_storm_nominal_set_cb set;
    intr_db_storm_nominal_get_cb get;
} intr_db_storm_nominal_cbs;


typedef struct {
    intr_db_is_init_cb is_init;
    intr_db_init_cb init;
    
    intr_db_flags_cbs flags;
    
    intr_db_storm_timed_count_cbs storm_timed_count;
    
    intr_db_storm_timed_period_cbs storm_timed_period;
    
    intr_db_storm_nominal_cbs storm_nominal;
} intr_db_cbs;





extern intr_db_cbs intr_db;

#endif 
