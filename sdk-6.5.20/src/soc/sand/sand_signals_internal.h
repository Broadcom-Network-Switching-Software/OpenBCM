
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _SOC_SAND_SIGNALS_INTERNAL_H_
#define _SOC_SAND_SIGNALS_INTERNAL_H_

#include <soc/sand/sand_signals.h>

#define DNX_PEMA_WINDOW_SIZE                    640

typedef struct {
    char *name;
    char *from;
    char *to;
    int  size;
    int  block_id;
    char *addr_str;
    char *expansion;
} static_signal_t;

typedef struct {
    char *name;
    static_signal_t *signals;
} static_block_t;

typedef struct {
    char *name;
    char *bitstr;
    char *expansion;
} static_field_t;

typedef struct {
    char *name;
    int  size;
    static_field_t *fields;
} static_sigstruct_t;

typedef struct {
    char               *name;
    static_block_t     *blocks;
    static_sigstruct_t *sigstructs;
    int                 sigstruct_num;
} static_device_t;

int
sand_signal_handle_get(
    int unit,
    int extra_flags,
    char *block,
    char *from,
    char *to,
    char *name,
    debug_signal_t ** signal_p);

sigstruct_t *
sand_signal_struct_get(
    device_t*   device,
    char*       signal_n,
    char*       from_n,
    char*       to_n,
    char*       block_n);

int
sand_signal_address_parse(
    char *full_address,
    signal_address_t * address);

int
sand_signal_range_parse(
    char *bit_range,
    int *startbit_p,
    int *endbit_p,
    int order,
    int size);

shr_error_e
sand_signal_str_to_value(
        int unit,
        sal_field_type_e type,
        char *str_value,
        int size,
        uint32 *value);

sigparam_t *
sand_signal_resolve_get(
    device_t * device,
    char*       signal_n,
    char*       from_n,
    char*       to_n,
    char*       block_n);

char *
sand_signal_metadata_description(
        device_t * device,
        char *attribute);

shr_error_e
sand_signal_init(
    int        unit,
    device_t * device);

void
sand_signal_deinit(
    int        unit,
    device_t * device);

sigstruct_t *
sand_signal_expand_get(
    int     unit,
    int     core,
    device_t * device,
    int flags,
    sigstruct_t *sigstruct_in,
    char *from_n,
    char *to_n,
    char *block_n,
    uint32 * value,
    expansion_t * expansion);

#if defined(ADAPTER_SERVER_MODE)
shr_error_e
sand_adapter_update_stages(
        int unit,
        device_t * device);

shr_error_e
sand_adapter_get_stage_signals(
    device_t * device,
    int unit,
    char *block_n,
    pp_stage_t *cur_pp_stage,
    int flags);

#endif 

#endif 
