/*! \file field_arr_mem_mgr.c
 *
 * Field array memory manager API implementation
 *
 * Interface to the field array memory manager
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_alloc.h>
#include <sal/sal_mutex.h>
#include <shr/shr_error.h>
#include <shr/shr_lmem_mgr.h>
#include <shr/shr_fmm.h>

#define FIELD_ARR_ALLOC_CHUNKS  2

typedef struct famm_hdl_s {
    uint32_t size;
    uint32_t ref_count;
    shr_lmm_hdl_t hdl;
    struct famm_hdl_s *next;
} famm_hdl_t;

static famm_hdl_t *famm_hdl_list;
static sal_mutex_t famm_mutex;

#define FAMM_LOCK() sal_mutex_take(famm_mutex, SAL_MUTEX_FOREVER)
#define FAMM_UNLOCK() sal_mutex_give(famm_mutex)

int shr_famm_init(void)
{
    shr_famm_cleanup();

    famm_mutex = sal_mutex_create("fld_arr_mem_mgr");
    if (!famm_mutex) {
        return SHR_E_MEMORY;
    }

    return SHR_E_NONE;
}

void shr_famm_cleanup(void)
{
    famm_hdl_t *fh;

    if (famm_mutex) {
        sal_mutex_destroy(famm_mutex);
        famm_mutex = 0;
    }

    while (famm_hdl_list) {
        fh = famm_hdl_list;
        famm_hdl_list = famm_hdl_list->next;
        shr_lmm_delete(fh->hdl);
        sal_free(fh);
    }
}

int shr_famm_hdl_init(uint32_t num_of_fld, shr_famm_hdl_t *hdl)
{
    int rv;
    uint32_t element_size, pointer_offset;
    famm_hdl_t *fh;

    FAMM_LOCK();
    fh = famm_hdl_list;
    while (fh) {
        if (fh->size == num_of_fld) {
            fh->ref_count++;
            break;
        }
        fh = fh->next;
    }

    if (!fh) {
        fh = sal_alloc(sizeof(*fh), "shrFmmFldArrHdl");
        if (!fh) {
            FAMM_UNLOCK();
            return SHR_E_MEMORY;
        }
        fh->size = num_of_fld;
        fh->ref_count = 1;

        /*
         * Initialize local memory instance for field array.
         * The basic element is composed of:
         *  1. Array of fmm pointers (array_size is specified by num_of_fld)
         *  2. Pointer to the next element in the list
         */
        element_size = (num_of_fld + 1) * sizeof(shr_fmm_t *);
        pointer_offset = num_of_fld * sizeof(shr_fmm_t *);

        rv = shr_lmm_init(FIELD_ARR_ALLOC_CHUNKS,
                          element_size,
                          pointer_offset,
                          true,
                          "shr_famm",
                          &fh->hdl);

        if (0 != rv) {
            sal_free(fh);
            FAMM_UNLOCK();
            return SHR_E_MEMORY;
        }

        fh->next = famm_hdl_list;
        famm_hdl_list = fh;
    }
    FAMM_UNLOCK();

    *hdl = (shr_famm_hdl_t)fh;

    return SHR_E_NONE;
}

void shr_famm_hdl_delete(shr_famm_hdl_t hdl)
{
    famm_hdl_t *fh = (famm_hdl_t *)hdl;

    if (!fh) {
        return;
    }

    FAMM_LOCK();
    if (--fh->ref_count == 0) {
        famm_hdl_t *tmp;
        shr_lmm_delete(fh->hdl);
        /* delete the element from the linked list */
        if (fh == famm_hdl_list) {
            famm_hdl_list = fh->next;
        } else {
            tmp = famm_hdl_list;
            while (tmp->next) {
                if (tmp->next == fh) {
                    tmp->next = fh->next;
                    break;
                }
                tmp = tmp->next;
            }
        }
        sal_free(fh);
    }
    FAMM_UNLOCK();
}

shr_fmm_t** shr_famm_alloc(shr_famm_hdl_t hdl, uint32_t num_of_fld)
{
    famm_hdl_t *fh = (famm_hdl_t *)hdl;
    shr_fmm_t **field_arr;
    uint32_t i;

    if (!fh || fh->size < num_of_fld)  {
        return NULL;
    }

    field_arr = shr_lmm_alloc(fh->hdl);
    if (!field_arr) {
        return NULL;
    }

    for (i = 0; i < num_of_fld; ) {
        field_arr[i] = shr_fmm_alloc();
        if (!field_arr[i]) {
            shr_famm_free(hdl, field_arr, i);
            return NULL;
        }
        sal_memset(field_arr[i], 0, sizeof(shr_fmm_t));
        i++;
    }

    return field_arr;
}

void shr_famm_free(shr_famm_hdl_t hdl,
                   shr_fmm_t ** field_arr,
                   uint32_t num_of_fld)
{
    famm_hdl_t *fh = (famm_hdl_t *)hdl;
    uint32_t i;

    if (fh->size < num_of_fld)  {
        num_of_fld = fh->size;
    }

    for (i = 0; i < num_of_fld; i++) {
        shr_fmm_free(field_arr[i]);
    }

    shr_lmm_free(fh->hdl, (void *)field_arr);
}
