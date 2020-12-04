
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNXC_SW_STATE_DISPATCHER_H

#define _DNXC_SW_STATE_DISPATCHER_H

#include <sal/core/sync.h>
#include <soc/dnxc/swstate/auto_generated/types/dnxc_module_ids_types.h>
#include <soc/dnxc/swstate/types/sw_state_mutex.h>
#include <soc/dnxc/swstate/types/sw_state_sem.h>

typedef enum
{
    DNXC_SW_STATE_IMPLEMENTATION_PLAIN,
    DNXC_SW_STATE_IMPLEMENTATION_WB
} dnxc_sw_state_implementation_e;

typedef int (
    *dnxc_sw_state_init_cb) (
    int unit,
    uint32 flags);

typedef int (
    *dnxc_sw_state_deinit_cb) (
    int unit,
    uint32 flags);

typedef int (
    *dnxc_sw_state_module_init_cb) (
    int unit,
    uint32 module_id,
    uint32 size,
    uint32 flags,
    char *dbg_string);

typedef int (
    *dnxc_sw_state_module_deinit_cb) (
    int unit,
    uint32 module_id,
    uint32 flags,
    char *dbg_string);

typedef int (
    *dnxc_sw_state_alloc_cb) (
    int unit,
    uint32 module_id,
    uint8 **ptr_to_ptr,
    uint32 nof_elements,
    uint32 element_size,
    uint32 flags,
    char *dbg_string);

typedef int (
    *dnxc_sw_state_free_cb) (
    int unit,
    uint32 module_id,
    uint8 **ptr_to_ptr,
    uint32 flags,
    char *dbg_string);

typedef int (
    *dnxc_sw_state_memcpy_cb) (
    int unit,
    uint32 module_id,
    uint32 size,
    uint8 *dest,
    uint8 *src,
    uint32 flags,
    char *dbg_string);

typedef int (
    *dnxc_sw_state_memset_cb) (
    int unit,
    uint32 module_id,
    uint8 *dest,
    uint32 value,
    uint32 size,
    uint32 flags,
    char *dbg_string);

typedef int (
    *dnxc_sw_state_alloc_size_cb) (
    int unit,
    uint32 module_id,
    uint8 *ptr,
    uint32 *nof_elements,
    uint32 *element_size);

typedef int (
    *dnx_sw_state_counter_inc_cb) (
    int unit,
    uint32 module_id,
    uint8 *ptr_location,
    uint32 inc_value,
    uint32 type_size,
    uint32 flags,
    char *dbg_string);

typedef int (
    *dnx_sw_state_counter_dec_cb) (
    int unit,
    uint32 module_id,
    uint8 *ptr_location,
    uint32 dec_value,
    uint32 type_size,
    uint32 flags,
    char *dbg_string);

typedef int (
    *dnxc_sw_state_mutex_create_cb) (
    int unit,
    uint32 module_id,
    sw_state_mutex_t * mtx,
    char *desc,
    uint32 flags);

typedef int (
    *dnxc_sw_state_mutex_destroy_cb) (
    int unit,
    uint32 module_id,
    sw_state_mutex_t * ptr_mtx,
    uint32 flags);

typedef int (
    *dnxc_sw_state_mutex_take_cb) (
    int unit,
    uint32 module_id,
    sw_state_mutex_t * ptr_mtx,
    int usec,
    uint32 flags);

typedef int (
    *dnxc_sw_state_mutex_give_cb) (
    int unit,
    uint32 module_id,
    sw_state_mutex_t * ptr_mtx,
    uint32 flags);

typedef int (
    *dnxc_sw_state_sem_create_cb) (
    int unit,
    uint32 module_id,
    sw_state_sem_t * ptr_sem,
    int is_binary,
    int initial_count,
    char *desc,
    uint32 flags);

typedef int (
    *dnxc_sw_state_sem_destroy_cb) (
    int unit,
    uint32 module_id,
    sw_state_sem_t * ptr_sem,
    uint32 flags);

typedef int (
    *dnxc_sw_state_sem_take_cb) (
    int unit,
    uint32 module_id,
    sw_state_sem_t * ptr_sem,
    int usec,
    uint32 flags);

typedef int (
    *dnxc_sw_state_sem_give_cb) (
    int unit,
    uint32 module_id,
    sw_state_sem_t * ptr_sem,
    uint32 flags);

typedef int (
    *dnxc_sw_state_start_size_measurement_point_cb) (
    int unit);

typedef uint32 (
    *dnxc_sw_state_size_measurement_get_cb) (
    int unit);

typedef uint8 *(
    *dnxc_sw_state_calc_pointer) (
    int unit,
    sal_vaddr_t offset);

typedef uint8 **(
    *dnxc_sw_state_calc_dpointer) (
    int unit,
    sal_vaddr_t offset);

typedef sal_vaddr_t(
    *dnxc_sw_state_calc_offset) (
    int unit,
    uint8 *ptr);

typedef sal_vaddr_t(
    *dnxc_sw_state_calc_offset_from_dptr) (
    int unit,
    uint8 **ptr);

typedef struct
{
    dnxc_sw_state_module_init_cb module_init;
    dnxc_sw_state_module_deinit_cb module_deinit;
    dnxc_sw_state_alloc_cb alloc;
    dnxc_sw_state_free_cb free;
    dnxc_sw_state_alloc_size_cb alloc_size;
    dnxc_sw_state_memcpy_cb memcpy;
    dnxc_sw_state_memset_cb memset;
    dnxc_sw_state_mutex_create_cb mutex_create;
    dnxc_sw_state_mutex_destroy_cb mutex_destroy;
    dnxc_sw_state_mutex_take_cb mutex_take;
    dnxc_sw_state_mutex_give_cb mutex_give;
    dnxc_sw_state_sem_create_cb sem_create;
    dnxc_sw_state_sem_destroy_cb sem_destroy;
    dnxc_sw_state_sem_take_cb sem_take;
    dnxc_sw_state_sem_give_cb sem_give;
    dnx_sw_state_counter_inc_cb counter_inc;
    dnx_sw_state_counter_dec_cb counter_dec;
    dnxc_sw_state_start_size_measurement_point_cb start_measurement_point;
    dnxc_sw_state_size_measurement_get_cb size_measurement_get;
    dnxc_sw_state_calc_pointer calc_pointer;
    dnxc_sw_state_calc_dpointer calc_dpointer;
    dnxc_sw_state_calc_offset calc_offset;
    dnxc_sw_state_calc_offset_from_dptr calc_offset_from_dptr;

} dnxc_sw_state_dispatcher_cb;

extern dnxc_sw_state_dispatcher_cb dnxc_sw_state_dispatcher[][NOF_MODULE_ID];

shr_error_e dnxc_sw_state_dispatcher_module_init(
    int unit,
    uint32 module_id,
    dnxc_sw_state_implementation_e implementation);

shr_error_e dnxc_sw_state_dispatcher_module_deinit(
    int unit);

#endif
