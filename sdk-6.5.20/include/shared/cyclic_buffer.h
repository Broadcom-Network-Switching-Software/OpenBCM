/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DFE CELLS BUFFER H
 */
 
#ifndef _SHR_CYCLIC_BUFFER_H_
#define _SHR_CYCLIC_BUFFER_H_

#include <sal/types.h>

typedef struct cyclic_buffer_s {
    uint8* elements;
    int oldest;
    int count;
    int max_allowed;
    int entry_size;
} cyclic_buffer_t;

int cyclic_buffer_create(int unit, cyclic_buffer_t* buffer, int max_entry_size, int max_buffered_elements, char* buffer_name); 
int cyclic_buffer_destroy(int unit, cyclic_buffer_t* buffer);
int cyclic_buffer_add(int unit, cyclic_buffer_t* buffer, const void* new_element);
int cyclic_buffer_get(int unit, cyclic_buffer_t* buffer, void* received_element);
int cyclic_buffer_is_empty(int unit, const cyclic_buffer_t* buffer, int* is_empty);
int cyclic_buffer_is_full(int unit, const cyclic_buffer_t* buffer, int* is_full);
int cyclic_buffer_cells_count(int unit, const cyclic_buffer_t* buffer, int* count);

#endif /*_SHR_CYCLIC_BUFFER_H_*/
