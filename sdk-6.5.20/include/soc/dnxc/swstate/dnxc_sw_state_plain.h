
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNXC_SW_STATE_PLAIN_H

#define _DNXC_SW_STATE_PLAIN_H

#include <soc/types.h>
#include <sal/core/sync.h>
#include <sal/core/alloc.h>
#include <soc/error.h>
#include <shared/shrextend/shrextend_debug.h>

#define DNXC_SW_STATE_START_SIZE_MEASURMENT(unit)\
    SHR_IF_ERR_EXIT(dnxc_sw_state_plain_start_size_measurement_point(unit))

#define DNXC_SW_STATE_SIZE_MEASURMENT_GET(unit)\
    dnxc_sw_state_plain_size_measurement_get(unit)

typedef enum
{
    DNXC_SW_STATE_DLL_POINTER,
    DNXC_SW_STATE_DLL_MUTEX,
    DNXC_SW_STATE_DLL_SEM
} dnxc_sw_state_dll_entry_type_e;

typedef struct dnxc_sw_state_allocation_dll_entry_s
{
    uint32 start_santinel;
    struct dnxc_sw_state_allocation_dll_entry_s *prev;
    struct dnxc_sw_state_allocation_dll_entry_s *next;
    uint8 *ptr;
    uint8 **ptr_location;
    dnxc_sw_state_dll_entry_type_e type;
    uint32 end_santinel;
} dnxc_sw_state_allocation_dll_entry_t;

typedef struct dnxc_sw_state_allocation_dll_s
{
    uint32 entry_counter;
    dnxc_sw_state_allocation_dll_entry_t *head;
    dnxc_sw_state_allocation_dll_entry_t *tail;
} dnxc_sw_state_allocation_dll_t;

typedef struct dnxc_sw_state_allocation_data_prefix_s
{
    uint32 start_santinel;
    dnxc_sw_state_allocation_dll_entry_t *dll_entry;
    uint32 nof_elements;
    uint32 element_size;
} dnxc_sw_state_allocation_data_prefix_t;

typedef struct dnxc_sw_state_allocation_data_suffix_s
{
    uint32 end_santinel;
} dnxc_sw_state_allocation_data_suffix_t;

int dnxc_sw_state_dll_entry_free_by_ptr_location(
    int unit,
    uint8 **ptr_location);

int dnxc_sw_state_dll_entry_add(
    int unit,
    uint8 **ptr_location,
    dnxc_sw_state_allocation_data_prefix_t * ptr_prefix,
    dnxc_sw_state_dll_entry_type_e type);

int dnxc_sw_state_init_plain(
    int unit,
    uint32 flags);

int dnxc_sw_state_deinit_plain(
    int unit,
    uint32 flags);

int dnxc_sw_state_module_init_plain(
    int unit,
    uint32 module_id,
    uint32 size,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_module_deinit_plain(
    int unit,
    uint32 module_id,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_alloc_plain(
    int unit,
    uint32 module_id,
    uint8 **ptr_location,
    uint32 nof_elements,
    uint32 element_size,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_free_plain(
    int unit,
    uint32 module_id,
    uint8 **ptr_location,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_alloc_size_plain(
    int unit,
    uint32 module,
    uint8 *ptr,
    uint32 *nof_elements,
    uint32 *element_size);

int dnxc_sw_state_plain_start_size_measurement_point(
    int unit);

uint32 dnxc_sw_state_plain_size_measurement_get(
    int unit);

int dnxc_sw_state_total_size_get_plain(
    int unit,
    uint32 *size);

uint8 *dnxc_sw_state_plain_calc_pointer(
    int unit,
    sal_vaddr_t offset);

uint8 **dnxc_sw_state_plain_calc_double_pointer(
    int unit,
    sal_vaddr_t offset);

sal_vaddr_t dnxc_sw_state_plain_calc_offset(
    int unit,
    uint8 *ptr);

sal_vaddr_t dnxc_sw_state_plain_calc_offset_from_dptr(
    int unit,
    uint8 **ptr);

#endif
