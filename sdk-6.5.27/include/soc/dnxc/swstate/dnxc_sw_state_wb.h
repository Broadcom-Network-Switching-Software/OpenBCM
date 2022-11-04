
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _DNXC_SW_STATE_WB_H

#define _DNXC_SW_STATE_WB_H

#include <soc/types.h>
#include <sal/core/sync.h>
#include <sal/core/alloc.h>
#include <soc/error.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/scache.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <soc/dnxc/swstate/auto_generated/types/dnxc_module_ids_types.h>
#include <soc/dnxc/swstate/dnxc_sw_state_layout.h>

typedef enum
{
    DNXC_SW_STATE_SCACHE_HANDLE_DATA_BLOCK
} DNXC_SW_STATE_SCACHE_HANDLES;

typedef enum dnxc_sw_state_data_block_init_mode_e
{
    socDnxcSwStateDataBlockRegularInit,
    socDnxcSwStateDataBlockRestoreAndOveride
} dnxc_sw_state_init_mode_t;

typedef enum
{
    SW_STATE_ALLOC_ELEMENT_POINTER,
    SW_STATE_ALLOC_ELEMENT_MUTEX,
    SW_STATE_ALLOC_ELEMENT_SEM,
    SW_STATE_ALLOC_EXTERNAL,
} dnxc_sw_state_alloc_element_type_e;

typedef struct dnxc_sw_state_alloc_element_s
{
    uint32 ptr_offset;
    uint8 *ptr_value;
    dnxc_sw_state_alloc_element_type_e type;

} dnxc_sw_state_alloc_element_t;

typedef struct dnxc_sw_state_data_block_header_s
{
    uint8 is_init;
    uint32 total_buffer_size;
    uint32 data_size;
    uint32 size_left;
    uint32 module_sizes[NOF_MODULE_ID];

    void *data_ptr;
    void *next_free_data_slot;
    dnxc_sw_state_alloc_element_t *ptr_offsets_sp;
    dnxc_sw_state_alloc_element_t *ptr_offsets_stack_base;
    sal_mutex_t alloc_mtx;
    void **roots_array;
    dnxc_sw_state_layout_t **layout_array;
} dnxc_sw_state_data_block_header_t;

uint8 dnxc_sw_state_data_block_header_is_init(
    int unit);

uint8 dnxc_sw_state_data_block_is_in_range(
    int unit,
    uint8 *ptr);

uint8 dnxc_sw_state_data_block_is_enough_space(
    int unit,
    uint32 size);

int dnxc_sw_state_wb_sizes_get(
    int unit,
    uint32 *in_use,
    uint32 *left);

int dnxc_sw_state_data_block_pointers_stack_push(
    int unit,
    uint8 **ptr_location,
    dnxc_sw_state_alloc_element_type_e type);

int dnxc_sw_state_init_wb(
    int unit,
    uint32 flags,
    uint32 sw_state_max_size);

int dnxc_sw_state_deinit_wb(
    int unit,
    uint32 flags);

int dnxc_sw_state_module_init_wb(
    int unit,
    uint32 node_id,
    uint32 size,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_module_deinit_wb(
    int unit,
    uint32 node_id,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_alloc_wb(
    int unit,
    uint32 node_id,
    uint8 **ptr_location,
    uint32 nof_elements,
    uint32 element_size,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_free_wb(
    int unit,
    uint32 node_id,
    uint8 **ptr_location,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_alloc_size_wb(
    int unit,
    uint32 node_id,
    uint8 *ptr,
    uint32 *nof_elements,
    uint32 *element_size);

int dnxc_sw_state_wb_start_size_measurement_point(
    int unit);

uint32 dnxc_sw_state_wb_size_measurement_get(
    int unit);

uint8 *dnxc_sw_state_wb_calc_pointer(
    int unit,
    sal_vaddr_t offset);

uint8 **dnxc_sw_state_wb_calc_double_pointer(
    int unit,
    sal_vaddr_t offset);

sal_vaddr_t dnxc_sw_state_wb_calc_offset(
    int unit,
    uint8 *ptr);

sal_vaddr_t dnxc_sw_state_wb_calc_offset_from_dptr(
    int unit,
    uint8 **ptr);

void dnxc_sw_state_wb_module_size_update(
    int unit,
    uint32 node_id,
    uint32 allocated_size,
    uint32 increment);

uint32 dnxc_sw_state_wb_module_size_get(
    int unit,
    uint32 node_id);

int dnxc_sw_state_layout_init_wb(
    int unit,
    uint32 node_id,
    uint32 size,
    uint32 nof_params,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_layout_deinit_wb(
    int unit,
    uint32 node_id,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_memcpy_wb(
    int unit,
    uint32 node_id,
    uint32 size,
    uint8 *dest,
    uint8 *src,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_memset_wb(
    int unit,
    uint32 node_id,
    uint8 *dest,
    uint32 value,
    uint32 size,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_counter_inc_wb(
    int unit,
    uint32 node_id,
    uint8 *ptr_location,
    uint32 inc_value,
    uint32 type_size,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_counter_dec_wb(
    int unit,
    uint32 node_id,
    uint8 *ptr_location,
    uint32 dec_value,
    uint32 type_size,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_auto_sync(
    int unit,
    uint8 *current_pointer_in_sw_state,
    uint32 size);

#endif
