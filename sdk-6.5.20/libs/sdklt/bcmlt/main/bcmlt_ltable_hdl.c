/*! \file bcmlt_ltable_hdl.c
 *
 *  Logical table handle manager.
 *  This file convert handle to pointer and pointer to handle.
 *  This module operates under the following assumptions:
 *  1. Upon init the maximal number of handles is determined and will not be
 *  changed.
 *  2. Creating and freeing a handle is not thread safe. It is the
 *  responsibility of the caller to make sure there is only single thread that
 *  allocates and frees handles.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <sal/sal_assert.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>
#include <bcmlt/bcmlt.h>
#include "bcmlt_internal.h"

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMLT_ENTRY

/*
 * Determine the number of handles that will be allocate with every extension.
 */
#define HANDLE_ARRAY_CHUNK  100

/*
 * Rather that starting from 0 start for this number so random handle
 * numbers will be invalid.
 */
#define HANDLE_BASE     1000

/* The maximal number of handle chunks that can be allocated */
#define CHUNK_ARRAY_SIZE      1024

/*
 * The handles will be slot of pointers where each slot expressed by the tuple
 * (chunk, offset). The handle resource manager contains multiple vectors, each
 * containing an array of handle slots. By allowing multiple vectors the system
 * can increase its handle number when the handle resource had been depleted.
 * The maximal number of vectors is CHUNK_ARRAY_SIZE. This array being created
 * during initialization and can not grow further. Each element of this array is
 * a pointer to a vector of slots. When the system needs more handles it simply
 * allocates another vector that contains HANDLE_ARRAY_CHUNK of handle slots. It
 * then set an empty slot in the array to point to this vector. That empty slot
 * consider to be the chunk number of all the handles within this vector.
 *
 * Internally each element will point to the next element (within the vector)
 * while also carrying its own origin chunk.
 * The value stored in each handle slot looks as follows:
 * -----------------------------------------
 * | origin chunk | chunk ptr | offset ptr |
 * -----------------------------------------
 * Where:
 * Origin chunk is the chunk (vector) that this slot belongs to.
 * Chunk ptr is the chunk ID of the handle point by this handle.
 * Offset ptr is the slot offset within the chunk (vector) of the handle
 * pointed by this handle.
 */
#define CHUNK_PTR_SHIFT 10
#define OFFSET_MASK 0x3FF
#define CHUNK_IDX_MASK 0x7FF

typedef union hdl_struct_u {
    struct {
        unsigned int ptr_offset: CHUNK_PTR_SHIFT;
        unsigned int ptr_chunk: 11;
        unsigned int orig_chunk: 11;
    } intern_addr;  /* This is the decoding of internal addresses */
    uint8_t *data_ptr;
} hdl_struct_t;


/*!
 * \brief Resource manager structure for handles
 */
typedef struct {
    hdl_struct_t **array;   /*!< Pointer to array of data arrays. */
    uint32_t **occ_array; /*< Bitmap indicating is a slot is occupied */
    hdl_struct_t *free_list;  /*!< Point to the beginning of the free list. */
    hdl_struct_t *last_elem;  /*!< Point to the last element of the free list. */
    uint32_t next_slot;  /*!< The next available slot for new chunk */
    uint32_t array_size;  /*!< Maintains the array size. */
} hdl_resource_t;

static hdl_resource_t hdl_resource;

/*******************************************************************************
 * Private functions
 */
/*!
 *\brief Initializes pointer array free list.
 *
 * This function Initializes the input array to be a link list of elements.
 * It then sets the field \c last_elem of res (resource) to point to the
 * last element.
 *
 * \param [in] array Is the begining of the array of pointers.
 * \param [in] res Is the resource control structure. It maintains the
 * \c array as well as the pointers to the first and last free elements.
 * \param [in] size Is the number of pointers available in the array.
 *
 * \return None.
 */
static void ptr_array_init(void *array, hdl_resource_t *res, uint32_t size)
{
    uint32_t j;
    hdl_struct_t *elem = (hdl_struct_t *)array;
    uint32_t chunk_num = res->next_slot;

    for (j = 0; j < size - 1; j++, elem++) {
        /* Set the pointer chunk and offset */
        elem->intern_addr.ptr_offset = j + 1;
        elem->intern_addr.ptr_chunk = chunk_num;
        elem->intern_addr.orig_chunk = chunk_num;
    }
    /* Last element pointing to NULL */
    elem->intern_addr.ptr_offset = 0;
    elem->intern_addr.ptr_chunk = 0;
    elem->intern_addr.orig_chunk = chunk_num;
    res->last_elem = elem;
}

/*!
 *\brief Extend pointer array free list.
 *
 * This function extends the pointers array by extending the original array
 * and initializing the new extension.
 *
 * \param [in] res Is the resource control structure. It maintains the
 * \c array as well as the pointers to the first and last free elements.
 * \param [in] size Is the number of pointers needed to be added.
 *
 * \return SHR_E_NONE on success.
 * \return SHR_E_FULL The handle count can't grow anymore.
 * \return SHR_E_MEMORY No memory available.
 */
static int ptr_array_extend(hdl_resource_t *res, uint32_t size)
{
    void *ptr;
    int word_size;

    if (res->next_slot + 1 >= CHUNK_ARRAY_SIZE) {
        return SHR_E_FULL;
    }
    /* Allocate the chunk */
    ptr = sal_alloc(size * sizeof(hdl_struct_t), "bcmltHdlArray");
    if (!ptr) {
        return SHR_E_MEMORY;
    }
    res->array[res->next_slot] = ptr;

    /* Set ptr to point to the uninitialized section of the array */
    ptr_array_init(ptr, res, size);
    res->free_list = ptr;

    /*
     * Calculate the number of bytes to allocate where the elements are
     * 32 bits words. Therefore we need (size+31) / 32 words and then
     * we need to multiple by 4 since sal_alloc expect size by bytes.
     */
    word_size = (size + 31) / 8;
    ptr = sal_alloc(word_size, "bcmltHdlOccArray");
    if (!ptr) {
        return SHR_E_MEMORY;
    }
    sal_memset (ptr, 0, word_size);
    res->occ_array[res->next_slot] = ptr;

    res->next_slot++;
    return SHR_E_NONE;
}


/*******************************************************************************
 * External functions local to this component
 */
int bcmlt_hdl_init(void)
{
    hdl_resource_t *res = &hdl_resource;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_ALLOC(res->array, CHUNK_ARRAY_SIZE * sizeof(uint8_t *), "bcmltHdl");
    SHR_NULL_CHECK(res->array, SHR_E_MEMORY);
    SHR_ALLOC(res->occ_array, CHUNK_ARRAY_SIZE * sizeof(uint8_t *), "bcmltHdl");
    SHR_NULL_CHECK(res->occ_array, SHR_E_MEMORY);
    res->array_size = HANDLE_ARRAY_CHUNK;
    res->next_slot = 0;

    /* Initialize the free list */
    ptr_array_extend(res, HANDLE_ARRAY_CHUNK);

exit:
    if (SHR_FUNC_ERR()) {
        if (res->array) {
            sal_free(res->array);
            res->array = NULL;
            res->array_size = 0;
        }
    }
    SHR_FUNC_EXIT();
}

void bcmlt_hdl_shutdown(void)
{
    uint32_t j;
    hdl_resource_t *res = &hdl_resource;

    if (res->array) {
        for (j = 0; j < res->next_slot; j++) {
            sal_free(res->array[j]);
            sal_free(res->occ_array[j]);
        }
        SHR_FREE(res->array);
        SHR_FREE(res->occ_array);
    }
    sal_memset (res, 0, sizeof(hdl_resource_t));
}

/* This function is not reentrant! */
int bcmlt_hdl_alloc(void *data, uint32_t *hdl)
{
    hdl_resource_t *res = &hdl_resource;
    hdl_struct_t *hdl_slot;
    int rv;
    uint32_t chunk;
    uint32_t offset;
    uint32_t mask;
    uint32_t slot;
    uint32_t *occupy;

    if (res->free_list == NULL) {
        rv = ptr_array_extend(res, HANDLE_ARRAY_CHUNK);
        if (rv != SHR_E_NONE) {
            return rv;
        }
    }
    hdl_slot = res->free_list;
    /* Obtain the next element in the list */
    chunk = hdl_slot->intern_addr.ptr_chunk;
    offset = hdl_slot->intern_addr.ptr_offset;
    assert(chunk < res->next_slot);
    assert(offset < res->array_size);
    if (res->free_list != res->last_elem) {
        res->free_list = &res->array[chunk][offset];
    } else {
        res->free_list = NULL;
        res->last_elem = NULL;
    }

    /* Get the origin chunk */
    chunk = hdl_slot->intern_addr.orig_chunk;
    hdl_slot->data_ptr = data;
    /*
     * Calculate the actual offset of the handle from its chunk start. This
     * indicates the slot where the handle is at.
     */
    offset = hdl_slot - res->array[chunk];

    /* Set the occupy array */
    mask = 1 << (offset % 32);
    slot = offset / 32;
    occupy = &res->occ_array[chunk][slot];
    *occupy |= mask;

    *hdl = (uint32_t)((chunk << CHUNK_PTR_SHIFT) | offset) + HANDLE_BASE;

    return SHR_E_NONE;
}

/* This function is not reentrant! */
int bcmlt_hdl_free(uint32_t hdl)
{
    hdl_struct_t *hdl_slot;
    hdl_resource_t *res = &hdl_resource;
    uint32_t chunk;
    uint32_t offset;
    uint32_t mask;
    uint32_t slot;
    uint32_t *occupy;

    hdl -= HANDLE_BASE;
    chunk = (hdl >> CHUNK_PTR_SHIFT) & CHUNK_IDX_MASK;
    offset = hdl & OFFSET_MASK;
    if (chunk >= res->next_slot || offset >= res->array_size) {
        return SHR_E_PARAM;
    }

    /*
     * Verify that this slot is indeed occupied so we do not free the same
     * handle multiple times.
     */
    mask = 1 << (offset % 32);
    slot = offset / 32;
    occupy = &res->occ_array[chunk][slot];
    if (!(*occupy & mask)) {
        return SHR_E_PARAM;
    }

    /* Clear the occupy bit. */
    *occupy ^= mask;

    /* Get the actual handle pointer */
    hdl_slot = &res->array[chunk][offset];

    if (res->last_elem) {
        /* Set the last element to point to the handle */
        res->last_elem->intern_addr.ptr_offset = offset;
        res->last_elem->intern_addr.ptr_chunk = chunk;
    } else if (!res->free_list) {
        res->free_list = hdl_slot;
    }
    /*
     * The last element points to NULL. We only need to preserve the
     * original chunk number.
     */
    hdl_slot->data_ptr = NULL;
    hdl_slot->intern_addr.orig_chunk = chunk;
    res->last_elem = hdl_slot;   /* Update the last element pointer */

    return SHR_E_NONE;
}

void *bcmlt_hdl_data_get(uint32_t hdl)
{
    hdl_struct_t *hdl_slot;
    hdl_resource_t *res = &hdl_resource;
    uint32_t chunk;
    uint32_t offset;
    uint32_t mask;
    uint32_t slot;
    uint32_t *occupy;

    hdl -= HANDLE_BASE;
    chunk = (hdl >> CHUNK_PTR_SHIFT) & CHUNK_IDX_MASK;
    offset = hdl & OFFSET_MASK;
    if (chunk >= res->next_slot || offset >= res->array_size) {
        return NULL;
    }
    hdl_slot = &res->array[chunk][offset];

    /*
     * Verify that this slot is indeed occupied so we do not free the same
     * handle multiple times.
     */
    mask = 1 << (offset % 32);
    slot = offset / 32;
    occupy = &res->occ_array[chunk][slot];
    if (!(*occupy & mask)) {
        return NULL;
    }

    return hdl_slot->data_ptr;
}
