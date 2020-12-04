/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SHARED CYCLIC BUFFER
 */
 

#include <shared/error.h>
#include <shared/cyclic_buffer.h>
#include <sal/core/alloc.h>
#include <sal/core/libc.h>
#include <soc/error.h>


int 
cyclic_buffer_create(int unit, cyclic_buffer_t* buffer, int max_entry_size, int max_buffered_elements, char* buffer_name)
{
    if(NULL == buffer) {
        return _SHR_E_PARAM;
    }

    buffer->elements = (uint8*)sal_alloc(max_buffered_elements*max_entry_size,buffer_name);
    if(NULL == buffer->elements) {
        return _SHR_E_MEMORY;
    }

    buffer->oldest = 0;
    buffer->count = 0;
    buffer->max_allowed = max_buffered_elements;
    buffer->entry_size = max_entry_size;

    return _SHR_E_NONE;
}

int 
cyclic_buffer_destroy(int unit, cyclic_buffer_t* buffer)
{
    if(NULL == buffer) {
        return _SHR_E_PARAM;
    }

    if(NULL != buffer->elements) {
        SOC_FREE(buffer->elements);
    }
    buffer->elements = NULL;
    buffer->oldest = 0;
    buffer->count = 0;
    buffer->max_allowed = 0;
    buffer->entry_size = 0;

    return _SHR_E_NONE;
}

int 
cyclic_buffer_add(int unit, cyclic_buffer_t* buffer, const void* new_element)
{
    int rc, is_full;
    int free_pos;
    
    if(NULL == buffer || NULL == new_element) {
        return _SHR_E_PARAM;
    }

    if(NULL == buffer->elements) {
        return _SHR_E_INIT;
    }

    rc = cyclic_buffer_is_full(unit, buffer, &is_full);
    _SHR_E_IF_ERROR_RETURN(rc);
    if(is_full) {
        return _SHR_E_FULL;
    }

    free_pos = (buffer->oldest + buffer->count) % buffer->max_allowed;
    sal_memcpy(&(buffer->elements[free_pos*buffer->entry_size]), (const uint8*)new_element, buffer->entry_size);
    buffer->count++;
    
    return _SHR_E_NONE;
}

int 
cyclic_buffer_get(int unit, cyclic_buffer_t* buffer, void* received_element)
{
    int rc, is_empty;

    if(NULL == buffer || NULL == received_element) {
        return _SHR_E_PARAM;
    }

    if(NULL == buffer->elements) {
        return _SHR_E_INIT;
    }

    rc = cyclic_buffer_is_empty(unit, buffer, &is_empty);
    _SHR_E_IF_ERROR_RETURN(rc);
    if(is_empty) {
        return _SHR_E_EMPTY;
    }

    sal_memcpy((uint8*)received_element, &(buffer->elements[buffer->oldest*buffer->entry_size]), buffer->entry_size);
    buffer->oldest = (buffer->oldest+1) % buffer->max_allowed;
    buffer->count--;

    return _SHR_E_NONE;
}

int 
cyclic_buffer_is_empty(int unit, const cyclic_buffer_t* buffer, int* is_empty)
{
    if(NULL == buffer || NULL == is_empty) {
        return _SHR_E_PARAM;
    }

    if(NULL == buffer->elements) {
        return _SHR_E_INIT;
    }

    *is_empty = (0 == buffer->count ? 1 : 0);

    return _SHR_E_NONE;
}

int 
cyclic_buffer_is_full(int unit, const cyclic_buffer_t* buffer, int* is_full)
{

    if(NULL == buffer || NULL == is_full) {
        return _SHR_E_PARAM;
    }

    if(NULL == buffer->elements) {
        return _SHR_E_INIT;
    }

    *is_full = (buffer->max_allowed == buffer->count ? 1 : 0);

    return _SHR_E_NONE;
}

int
cyclic_buffer_cells_count(int unit, const cyclic_buffer_t* buffer, int* count)
{
    if(NULL == buffer || NULL == count) {
        return _SHR_E_PARAM;
    }

    if(NULL == buffer->elements) {
        return _SHR_E_INIT;
    }

    *count = buffer->count;

    return _SHR_E_NONE;
}


