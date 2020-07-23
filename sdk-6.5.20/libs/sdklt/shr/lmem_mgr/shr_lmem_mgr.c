/*! \file shr_lmem_mgr.c
 *
 * Local memory manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <sal/sal_mutex.h>
#include <sal/sal_assert.h>
#include <bsl/bsl.h>
#include <shr/shr_error.h>
#include <shr/shr_lmem_mgr.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_SHR_LMEM

#define LMEM_MGR_SIGNATURE   0x75f2a5dc
#define ROBUST_ENABLE       1

/*
 * When ROBUST_ENABLE is 1 we allocate an extra control byte at the end of each
 * element. The control byte uses 1 bit to indicate if the element is allocated
 * or not and 6 bits as signature that the element is valid.
 */
#if ROBUST_ENABLE
#define ELEMENT_FREE 0x0
#define ELEMENT_ALLOC 0x1
#define ELEMENT_SIG 0xCC
#define CONTROL_SET_FREE(_elem,_lm) \
do {                                                \
    uint8_t *control;                               \
    control = _elem + _lm->element_size - 1;        \
    *control = ELEMENT_SIG | ELEMENT_FREE;          \
} while (0)

#define CONTROL_ALLOC(_elem,_lm)    \
do {                                                    \
    uint8_t *control;                                   \
    control = _elem + _lm->element_size - 1;            \
    if (*control != (ELEMENT_SIG | ELEMENT_FREE)) {     \
        LOG_ERROR(BSL_LOG_MODULE,                       \
            (BSL_META("Corrupt free element list, control=0x%x\n"), \
                    *control));                         \
        assert (*control == (ELEMENT_SIG | ELEMENT_FREE));  \
    }                                                   \
    *control = ELEMENT_SIG | ELEMENT_ALLOC;             \
} while (0)

#define CONTROL_FREE(_elem,_lm)     \
do {                                                        \
    uint8_t *control;                                       \
    control = _elem + _lm->element_size - 1;                \
    /* Don't continue if the element control portion is wrong */ \
    if (*control != (ELEMENT_SIG | ELEMENT_ALLOC)) {        \
        LOG_ERROR(BSL_LOG_MODULE,                           \
            (BSL_META("Freeing already freed element control=0x%x\n"), \
                    *control));                             \
        return;                                             \
    }                                                       \
    *control = ELEMENT_SIG | ELEMENT_FREE;                  \
} while (0)

#else
#define CONTROL_SET_FREE(_elem,_lm)
#define CONTROL_ALLOC(_elem,_lm)
#define CONTROL_FREE(_elem,_lm)
#endif

typedef struct local_mem_chunks_s {
    void *mem;
    struct local_mem_chunks_s *next;
} local_mem_chunks_t;

typedef struct local_mem_mgr_s {
    uint32_t signature;
    size_t element_size; /* The size of each element (in bytes) */
    size_t chunk_size;   /* How many elements are in a chunk */
    /* The location of the pointer field within the entry */
    uint32_t pointer_offset;
    bool mt;             /* Multi-thread safe */
    uint32_t max_count;  /* Maximum allocated entries */
    uint32_t in_use;     /* How many elements are currently in use */
    uint32_t allocated;  /* How many elements being allocated (free+in_use) */
    uint32_t high_wtr_mark;
    sal_mutex_t mtx;     /* Mutex valid when in multi-thread safe mode */
    uint8_t *free_list;  /* Allocating from the beginning of the list */
    uint8_t *last_elem;  /* Releasing elements to the end of the list */
    local_mem_chunks_t *chunk_list;
    const char *label;
    struct local_mem_mgr_s *next;
} local_mem_mgr_t;

static local_mem_mgr_t *lmm_global_list;
static sal_mutex_t lmm_global_sync;

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Build the chunk linked list
 *
 * This function chains a linked list of all the elements in a given memory
 * chunk were the first element points to the second which points to the
 * third element and so on.
 * The last element is pointing to NULL (end of the list) and the function
 * sets the LM instance last element to point to that element.
 *
 * \param [in] lm Local memory control structure.
 * \param [in] element The first element in the chunk.
 * \param [in] elements_cnt The number of elements in the chunk.
 */
static void build_chunk_linked_list(local_mem_mgr_t *lm,
                                    uint8_t *element,
                                    uint32_t elements_cnt)
{
    uint32_t j;
    uint8_t **next;

    for (j = 0; j < elements_cnt; j++) {
        next = (uint8_t **)(element + lm->pointer_offset);
        CONTROL_SET_FREE(element, lm);
        if (j == elements_cnt - 1) {
            lm->last_elem = element;
            *next = NULL;
        } else {
            element += lm->element_size;
            *next = element;
        }
    }
}

/*!
 * \brief Allocate new chunk of elements
 *
 * This function allocates new chunk of elements and build the free list of
 * all the allocated elements.
 * It is also adds the chunk into the chunk list.
 *
 * \param [in] lm Local memory control structure.
 * \param [in] cnt The number of elements to add. If this is 0 use the
 * control structure 'chunk_size', otherwise use the requested count.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_MEMORY Failed to allocate memory.
 */
static int arrange_new_chunk(local_mem_mgr_t *lm, uint32_t cnt)
{
    uint32_t elements_cnt;
    local_mem_chunks_t *chunk;

    if (cnt != 0) {
        elements_cnt = cnt;
    } else {
        elements_cnt = lm->chunk_size;
    }
    lm->free_list = sal_alloc(lm->element_size * elements_cnt, "shrLmmFree");
    if (!lm->free_list) {
        return SHR_E_MEMORY;
    }
    chunk = sal_alloc(sizeof(local_mem_chunks_t), "shrLmmChunk");
    if (!chunk) {
        sal_free (lm->free_list);
        return SHR_E_MEMORY;
    }
    chunk->mem = lm->free_list;
    /* Add the new chunk to the beginning of the list */
    chunk->next = lm->chunk_list;
    lm->chunk_list = chunk;
    lm->allocated += elements_cnt;

    build_chunk_linked_list(lm, lm->free_list, elements_cnt);
    return SHR_E_NONE;
}

/*******************************************************************************
 * Public functions
 */
int shr_lmm_global_init(void)
{
    if (!lmm_global_sync) {
        lmm_global_sync = sal_mutex_create("shrLmmMgrGlobal");
        if (!lmm_global_sync) {
            return SHR_E_MEMORY;
        }
        lmm_global_list = NULL;
    }
    return SHR_E_NONE;
}

int shr_lmm_global_shutdown(void)
{
    if (lmm_global_list) {
        local_mem_mgr_t *tmp = lmm_global_list;
        while (tmp) {
            LOG_WARN(BSL_LOG_MODULE,
                    (BSL_META("element %s has not been freed\n"),
                     (tmp->label ? tmp->label : "unlabled")));
            tmp = tmp->next;
        }
        return SHR_E_BUSY;
    }
    if (lmm_global_sync) {
        sal_mutex_destroy(lmm_global_sync);
        lmm_global_sync = NULL;
    }
    return SHR_E_NONE;
}

int shr_lmm_init(size_t chunk_size,
                 size_t element_size,
                 uint32_t pointer_offset,
                 bool multi_thread,
                 const char *label,
                 shr_lmm_hdl_t *hdl)
{
    return shr_lmm_restrict_init(chunk_size, element_size, pointer_offset,
                                 multi_thread, 0xFFFFFFFF, label, hdl);
}

int shr_lmm_restrict_init(size_t chunk_size,
                          size_t element_size,
                          uint32_t pointer_offset,
                          bool multi_thread,
                          uint32_t max_elements,
                          const char *label,
                          shr_lmm_hdl_t *hdl)
{
    local_mem_mgr_t *lm;
    int rv;

    lm = sal_alloc(sizeof(local_mem_mgr_t), "shrLmmMgr");
    if (!lm) {
        return -1;
    }
    lm->chunk_size = chunk_size;
#if ROBUST_ENABLE
    element_size += sizeof(uint32_t); /* increment size in multiples of 4 */
#endif
    lm->element_size = element_size;
    lm->pointer_offset = pointer_offset;
    lm->signature = LMEM_MGR_SIGNATURE;
    lm->mt = multi_thread;
    lm->max_count = max_elements;
    lm->in_use = 0;
    lm->high_wtr_mark = 0;
    lm->allocated = 0;
    lm->label = label;
    lm->chunk_list = NULL;
    if (arrange_new_chunk(lm, 0) != SHR_E_NONE) {
        sal_free(lm);
        return SHR_E_MEMORY;
    }
    if (multi_thread) {
        lm->mtx = sal_mutex_create("shrLmmMgr");
        if (!lm->mtx) {
            lm->mt = false;
            shr_lmm_delete((shr_lmm_hdl_t)lm);
            return -1;
        }
    }
    *hdl = (shr_lmm_hdl_t)lm;
    rv = sal_mutex_take(lmm_global_sync, SAL_MUTEX_FOREVER);
    if (rv != SHR_E_NONE) {
        return SHR_E_INIT;
    }
    lm->next = lmm_global_list;
    lmm_global_list = lm;
    sal_mutex_give(lmm_global_sync);

    return SHR_E_NONE;
}

void shr_lmm_delete(shr_lmm_hdl_t hdl)
{
    local_mem_mgr_t *lm = (local_mem_mgr_t *)hdl;
    local_mem_chunks_t *p_chunk;
    local_mem_mgr_t *tmp;

    if (!lm || (lm->signature != LMEM_MGR_SIGNATURE)) {
        return;
    }
    if (lm->mt) {
        sal_mutex_destroy(lm->mtx);
    }
    lm->signature = 0;  /* So consecutive call will fail */
    /* sal_free  all the chunks */
    while (lm->chunk_list) {
        p_chunk = lm->chunk_list;
        lm->chunk_list = p_chunk->next;
        sal_free  (p_chunk->mem);
        sal_free  (p_chunk);
    }
    /* Disconnect the element from the list */
    sal_mutex_take(lmm_global_sync, SAL_MUTEX_FOREVER);
    if (lm == lmm_global_list) {
        lmm_global_list = lm->next;
    } else {
        tmp = lmm_global_list;
        while (tmp->next) {
            if (tmp->next == lm) {
                tmp->next = lm->next;
                break;
            }
            tmp = tmp->next;
        }
    }
    sal_mutex_give(lmm_global_sync);
    sal_free  (lm);
}

void *shr_lmm_alloc(shr_lmm_hdl_t hdl)
{
    local_mem_mgr_t *lm = (local_mem_mgr_t *)hdl;
    void *rv;
    uint8_t **next;

    if (!lm || (lm->signature != LMEM_MGR_SIGNATURE)) {
        return NULL;
    }
    if (lm->mt) {
        sal_mutex_take(lm->mtx, SAL_MUTEX_FOREVER);
    }
    if (lm->in_use >= lm->max_count) {
        if (lm->mt) {
            sal_mutex_give(lm->mtx);
        }
        return NULL;
    }
    if (lm->free_list == NULL) {
        if (arrange_new_chunk(lm, 0) != SHR_E_NONE) {
            if (lm->mt) {
                sal_mutex_give(lm->mtx);
            }
            return NULL;
        }
    }

    rv = (void *)lm->free_list;
    CONTROL_ALLOC(lm->free_list, lm);

    next = (uint8_t **)(lm->free_list + lm->pointer_offset);
    lm->free_list = *next;
    if (!lm->free_list) {
        lm->last_elem = NULL;  /* We just gave up the last element */
    }
    lm->in_use++;
    if (lm->in_use > lm->high_wtr_mark) {
        lm->high_wtr_mark = lm->in_use;
    }

    if (lm->mt) {
        sal_mutex_give(lm->mtx);
    }
    return rv;
}

void *shr_lmm_alloc_bulk(shr_lmm_hdl_t hdl, uint32_t cnt)
{
    local_mem_mgr_t *lm = (local_mem_mgr_t *)hdl;
    void *rv = NULL;
    uint8_t **next = NULL;
    uint32_t j;
    uint8_t *elem;
    uint8_t *last_elem;
    uint32_t elements_cnt;

    if (!lm || (lm->signature != LMEM_MGR_SIGNATURE) || cnt == 0) {
        return NULL;
    }
    if (lm->mt) {
        sal_mutex_take(lm->mtx, SAL_MUTEX_FOREVER);
    }
    if (lm->in_use + cnt > lm->max_count) {
        if (lm->mt) {
            sal_mutex_give(lm->mtx);
        }
        return NULL;
    }

    /* Go though the elements to see that there are enough in the free list */
    elem = lm->free_list;
    last_elem = NULL;
    for (j = 0; j < cnt && elem; j++) {
        next = (uint8_t **)(elem + lm->pointer_offset);
        CONTROL_ALLOC(elem, lm);
        last_elem = elem;
        elem = *next;
    }
    if (j < cnt) {  /* Not enough elements */
        if (last_elem) {
            rv = (void *)lm->free_list;
            next = (uint8_t **)(last_elem + lm->pointer_offset);
        }
        if (cnt -j > lm->chunk_size) {
            elements_cnt = cnt - j;
        } else {
            elements_cnt = 0;
        }
        if (arrange_new_chunk(lm, elements_cnt) != SHR_E_NONE) {
            if (lm->mt) {
                sal_mutex_give(lm->mtx);
            }
            return NULL;
        }

        /*
         * If there were free elements before add the free list at the end of
         * these elements.
         */
        if (last_elem) {
            *next = lm->free_list;
        } else {
            rv = lm->free_list;
        }

        elem = lm->free_list;
        for (;j < cnt && elem; j++) {
            next = (uint8_t **)(elem + lm->pointer_offset);
            CONTROL_ALLOC(elem, lm);
            last_elem = elem;
            elem = *next;
        }
     }  else {
         rv = (void *)lm->free_list;
     }
     assert (j >= cnt);
     lm->free_list = elem;
     if (!lm->free_list) {
         lm->last_elem = NULL;  /* We just gave up the last element */
     }

     /* Next is the 'next' pointer of the last element */
    next = (uint8_t **)(last_elem + lm->pointer_offset);
    *next = NULL;
    lm->in_use += cnt;
    if (lm->in_use > lm->high_wtr_mark) {
        lm->high_wtr_mark = lm->in_use;
    }

    if (lm->mt) {
        sal_mutex_give(lm->mtx);
    }
    return rv;
}

void shr_lmm_free(shr_lmm_hdl_t hdl, void *element)
{
    local_mem_mgr_t *lm = (local_mem_mgr_t *)hdl;
    uint8_t **next;

    if (!lm || (lm->signature != LMEM_MGR_SIGNATURE) || !element) {
        return;
    }
    CONTROL_FREE((uint8_t *)element, lm);

    next = (uint8_t **)((uint8_t *)element + lm->pointer_offset);
    if (lm->mt) {
        sal_mutex_take(lm->mtx, SAL_MUTEX_FOREVER);
    }
    assert (lm->in_use > 0);
    *next = NULL;  /* Terminates the list */
    /* Add the element to the end of the list */
    if (lm->last_elem) {
        next = (uint8_t **)(lm->last_elem + lm->pointer_offset);
        *next = element;
    } else {
        lm->free_list = element;
    }
    lm->last_elem = element;
    lm->in_use--;

    if (lm->mt) {
        sal_mutex_give(lm->mtx);
    }
}

void shr_lmm_free_bulk(shr_lmm_hdl_t hdl, void *element)
{
     local_mem_mgr_t *lm = (local_mem_mgr_t *)hdl;
     uint8_t **next;
     uint8_t *elem;
     uint8_t *last_elem;
     uint32_t elem_cnt = 0;

     if (!lm || (lm->signature != LMEM_MGR_SIGNATURE) || !element) {
         return;
     }
     elem = element;

     do {
         CONTROL_FREE(elem, lm);
         next = (uint8_t **)(elem + lm->pointer_offset);
         last_elem = elem;
         elem = *next;
         elem_cnt++;
     } while (elem);

     if (lm->mt) {
         sal_mutex_take(lm->mtx, SAL_MUTEX_FOREVER);
     }
     assert (lm->in_use >= elem_cnt);
      lm->in_use -= elem_cnt;
     *next = NULL;  /* Terminate the list */
     /* Add the list to the end */
     if (lm->last_elem) {
         next = (uint8_t **)(lm->last_elem + lm->pointer_offset);
         *next = element;
     } else {  /* The free list and last were empty */
         lm->free_list = element;
     }
     lm->last_elem = last_elem;

     if (lm->mt) {
         sal_mutex_give(lm->mtx);
     }
}

void shr_lmm_free_all(shr_lmm_hdl_t hdl)
{
    local_mem_mgr_t *lm = (local_mem_mgr_t *)hdl;
    local_mem_chunks_t *chunk;
    uint8_t **next;

    if (!lm || lm->signature != LMEM_MGR_SIGNATURE) {
        return;
    }
    if (lm->mt) {
        sal_mutex_take(lm->mtx, SAL_MUTEX_FOREVER);
    }

    chunk = lm->chunk_list;
    lm->free_list = chunk->mem;
    while (chunk) {
        build_chunk_linked_list(lm, chunk->mem, lm->chunk_size);
        chunk = chunk->next;
        /*
         * If the new chunk is not NULL set the pointer of the last element
         * in the chunk to the next chunk.
         */
        if (chunk) {
            next = (uint8_t **)(lm->last_elem + lm->pointer_offset);
            *next = chunk->mem;
        }
    }
    lm->in_use = 0;
    if (lm->mt) {
        sal_mutex_give(lm->mtx);
    }
}

int shr_lmm_stat_get(shr_lmm_hdl_t hdl, shr_lmm_stat_t *stats)
{
    local_mem_mgr_t *lm = (local_mem_mgr_t *)hdl;

    if (!lm || (lm->signature != LMEM_MGR_SIGNATURE) || !stats) {
        return SHR_E_PARAM;
    }

    if (lm->mt) {
        sal_mutex_take(lm->mtx, SAL_MUTEX_FOREVER);
    }

    stats->in_use = lm->in_use;
    stats->avail = lm->max_count - lm->in_use;
    stats->hwtr_mark = lm->high_wtr_mark;
    stats->allocated = lm->allocated;
    stats->label = lm->label;
    stats->element_size = lm->element_size;
    stats->chunk_size = lm->chunk_size;

    if (lm->mt) {
        sal_mutex_give(lm->mtx);
    }
    return SHR_E_NONE;
}

int shr_lmm_stat_by_id_get(const char *id, shr_lmm_stat_t *stats)
{
    int rv;
    local_mem_mgr_t *tmp;
    size_t len;

    if (!id || !stats) {
        return SHR_E_PARAM;
    }

    len = sal_strlen(id);
    rv = sal_mutex_take(lmm_global_sync, SAL_MUTEX_FOREVER);
    if (rv != SHR_E_NONE) {
        return SHR_E_INIT;
    }
    rv = SHR_E_NOT_FOUND;
    tmp = lmm_global_list;
    while (tmp) {
        if (tmp->label && !sal_strncmp(tmp->label, id, len)) {
            if (tmp->mt) {
                sal_mutex_take(tmp->mtx, SAL_MUTEX_FOREVER);
            }
            stats->in_use = tmp->in_use;
            stats->avail = tmp->max_count - tmp->in_use;
            stats->hwtr_mark = tmp->high_wtr_mark;
            stats->allocated = tmp->allocated;
            stats->label = tmp->label;
            stats->element_size = tmp->element_size;
            stats->chunk_size = tmp->chunk_size;
            if (tmp->mt) {
                sal_mutex_give(tmp->mtx);
            }
            rv = SHR_E_NONE;
            break;
        }
        tmp = tmp->next;
    }
    sal_mutex_give(lmm_global_sync);
    return rv;
}

int shr_lmm_stat_get_all(size_t len, shr_lmm_stat_t *stats, size_t *actual)
{
    int rv;
    local_mem_mgr_t *tmp = lmm_global_list;
    size_t count = 0;

    rv = sal_mutex_take(lmm_global_sync, SAL_MUTEX_FOREVER);
    if (rv != SHR_E_NONE) {
        return SHR_E_INIT;
    }

    while (tmp) {
        if (stats && count < len) {
            if (tmp->mt) {
                sal_mutex_take(tmp->mtx, SAL_MUTEX_FOREVER);
            }
            stats[count].in_use = tmp->in_use;
            stats[count].avail = tmp->max_count - tmp->in_use;
            stats[count].hwtr_mark = tmp->high_wtr_mark;
            stats[count].allocated = tmp->allocated;
            stats[count].label = tmp->label;
            stats[count].element_size = tmp->element_size;
            stats[count].chunk_size = tmp->chunk_size;
            if (tmp->mt) {
                sal_mutex_give(tmp->mtx);
            }
        } else {
            rv = SHR_E_FULL;
        }
        count++;
        tmp = tmp->next;
    }

    if (actual) {
        *actual = count;
    }
    sal_mutex_give(lmm_global_sync);
    return rv;
}
