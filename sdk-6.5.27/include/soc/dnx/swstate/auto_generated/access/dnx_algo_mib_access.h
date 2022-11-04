
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_ALGO_MIB_ACCESS_H__
#define __DNX_ALGO_MIB_ACCESS_H__

#include <soc/dnx/swstate/auto_generated/types/dnx_algo_mib_types.h>
#include <soc/dnxc/swstate/auto_generated/types/dnxc_types.h>


typedef int (*dnx_algo_mib_db_is_init_cb)(
    int unit, uint8 *is_init);

typedef int (*dnx_algo_mib_db_init_cb)(
    int unit);

typedef int (*dnx_algo_mib_db_mib_counter_pbmp_set_cb)(
    int unit, bcm_pbmp_t counter_pbmp);

typedef int (*dnx_algo_mib_db_mib_counter_pbmp_get_cb)(
    int unit, bcm_pbmp_t *counter_pbmp);

typedef int (*dnx_algo_mib_db_mib_counter_pbmp_pbmp_neq_cb)(
    int unit, _shr_pbmp_t input_pbmp, uint8 *result);

typedef int (*dnx_algo_mib_db_mib_counter_pbmp_pbmp_eq_cb)(
    int unit, _shr_pbmp_t input_pbmp, uint8 *result);

typedef int (*dnx_algo_mib_db_mib_counter_pbmp_pbmp_member_cb)(
    int unit, uint32 _input_port, uint8 *result);

typedef int (*dnx_algo_mib_db_mib_counter_pbmp_pbmp_not_null_cb)(
    int unit, uint8 *result);

typedef int (*dnx_algo_mib_db_mib_counter_pbmp_pbmp_is_null_cb)(
    int unit, uint8 *result);

typedef int (*dnx_algo_mib_db_mib_counter_pbmp_pbmp_count_cb)(
    int unit, int *result);

typedef int (*dnx_algo_mib_db_mib_counter_pbmp_pbmp_xor_cb)(
    int unit, _shr_pbmp_t input_pbmp);

typedef int (*dnx_algo_mib_db_mib_counter_pbmp_pbmp_remove_cb)(
    int unit, _shr_pbmp_t input_pbmp);

typedef int (*dnx_algo_mib_db_mib_counter_pbmp_pbmp_assign_cb)(
    int unit, _shr_pbmp_t input_pbmp);

typedef int (*dnx_algo_mib_db_mib_counter_pbmp_pbmp_get_cb)(
    int unit, _shr_pbmp_t *output_pbmp);

typedef int (*dnx_algo_mib_db_mib_counter_pbmp_pbmp_and_cb)(
    int unit, _shr_pbmp_t input_pbmp);

typedef int (*dnx_algo_mib_db_mib_counter_pbmp_pbmp_negate_cb)(
    int unit, _shr_pbmp_t input_pbmp);

typedef int (*dnx_algo_mib_db_mib_counter_pbmp_pbmp_or_cb)(
    int unit, _shr_pbmp_t input_pbmp);

typedef int (*dnx_algo_mib_db_mib_counter_pbmp_pbmp_clear_cb)(
    int unit);

typedef int (*dnx_algo_mib_db_mib_counter_pbmp_pbmp_port_add_cb)(
    int unit, uint32 _input_port);

typedef int (*dnx_algo_mib_db_mib_counter_pbmp_pbmp_port_flip_cb)(
    int unit, uint32 _input_port);

typedef int (*dnx_algo_mib_db_mib_counter_pbmp_pbmp_port_remove_cb)(
    int unit, uint32 _input_port);

typedef int (*dnx_algo_mib_db_mib_counter_pbmp_pbmp_port_set_cb)(
    int unit, uint32 _input_port);

typedef int (*dnx_algo_mib_db_mib_counter_pbmp_pbmp_ports_range_add_cb)(
    int unit, uint32 _first_port, uint32 _range);

typedef int (*dnx_algo_mib_db_mib_counter_pbmp_pbmp_fmt_cb)(
    int unit, char* _buffer);

typedef int (*dnx_algo_mib_db_mib_interval_set_cb)(
    int unit, int interval);

typedef int (*dnx_algo_mib_db_mib_interval_get_cb)(
    int unit, int *interval);



typedef struct {
    dnx_algo_mib_db_mib_counter_pbmp_set_cb set;
    dnx_algo_mib_db_mib_counter_pbmp_get_cb get;
    dnx_algo_mib_db_mib_counter_pbmp_pbmp_neq_cb pbmp_neq;
    dnx_algo_mib_db_mib_counter_pbmp_pbmp_eq_cb pbmp_eq;
    dnx_algo_mib_db_mib_counter_pbmp_pbmp_member_cb pbmp_member;
    dnx_algo_mib_db_mib_counter_pbmp_pbmp_not_null_cb pbmp_not_null;
    dnx_algo_mib_db_mib_counter_pbmp_pbmp_is_null_cb pbmp_is_null;
    dnx_algo_mib_db_mib_counter_pbmp_pbmp_count_cb pbmp_count;
    dnx_algo_mib_db_mib_counter_pbmp_pbmp_xor_cb pbmp_xor;
    dnx_algo_mib_db_mib_counter_pbmp_pbmp_remove_cb pbmp_remove;
    dnx_algo_mib_db_mib_counter_pbmp_pbmp_assign_cb pbmp_assign;
    dnx_algo_mib_db_mib_counter_pbmp_pbmp_get_cb pbmp_get;
    dnx_algo_mib_db_mib_counter_pbmp_pbmp_and_cb pbmp_and;
    dnx_algo_mib_db_mib_counter_pbmp_pbmp_negate_cb pbmp_negate;
    dnx_algo_mib_db_mib_counter_pbmp_pbmp_or_cb pbmp_or;
    dnx_algo_mib_db_mib_counter_pbmp_pbmp_clear_cb pbmp_clear;
    dnx_algo_mib_db_mib_counter_pbmp_pbmp_port_add_cb pbmp_port_add;
    dnx_algo_mib_db_mib_counter_pbmp_pbmp_port_flip_cb pbmp_port_flip;
    dnx_algo_mib_db_mib_counter_pbmp_pbmp_port_remove_cb pbmp_port_remove;
    dnx_algo_mib_db_mib_counter_pbmp_pbmp_port_set_cb pbmp_port_set;
    dnx_algo_mib_db_mib_counter_pbmp_pbmp_ports_range_add_cb pbmp_ports_range_add;
    dnx_algo_mib_db_mib_counter_pbmp_pbmp_fmt_cb pbmp_fmt;
} dnx_algo_mib_db_mib_counter_pbmp_cbs;

typedef struct {
    dnx_algo_mib_db_mib_interval_set_cb set;
    dnx_algo_mib_db_mib_interval_get_cb get;
} dnx_algo_mib_db_mib_interval_cbs;

typedef struct {
    dnx_algo_mib_db_mib_counter_pbmp_cbs counter_pbmp;
    dnx_algo_mib_db_mib_interval_cbs interval;
} dnx_algo_mib_db_mib_cbs;

typedef struct {
    dnx_algo_mib_db_is_init_cb is_init;
    dnx_algo_mib_db_init_cb init;
    dnx_algo_mib_db_mib_cbs mib;
} dnx_algo_mib_db_cbs;





extern dnx_algo_mib_db_cbs dnx_algo_mib_db;

#endif 
