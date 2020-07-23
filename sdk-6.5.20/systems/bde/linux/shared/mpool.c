/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <mpool.h>

#ifdef __KERNEL__

/* 
 * Abstractions used when compiling for Linux kernel mode. 
 */

#include <lkm.h>

/*
 * We cannot use the linux kernel SAL for MALLOC/FREE because 
 * the current implementation of sal_alloc() allocates memory 
 * out of an mpool created by this module...
 */
#define MALLOC(x) kmalloc(x, GFP_ATOMIC)
#define FREE(x) kfree(x)

static spinlock_t _mpool_lock;
#define MPOOL_LOCK_INIT() spin_lock_init(&_mpool_lock)
#define MPOOL_LOCK() unsigned long flags; spin_lock_irqsave(&_mpool_lock, flags)
#define MPOOL_UNLOCK() spin_unlock_irqrestore(&_mpool_lock, flags)

#else /* !__KERNEL__*/

/* 
 * Abstractions used when compiling for Linux user mode. 
 */

#include <stdlib.h>
#include <sal/core/sync.h>

#define MALLOC(x) malloc(x)
#define FREE(x) free(x)

static sal_sem_t _mpool_lock;
#define MPOOL_LOCK_INIT() _mpool_lock = sal_sem_create("mpool_lock", 1, 1)
#define MPOOL_LOCK() sal_sem_take(_mpool_lock, sal_sem_FOREVER)
#define MPOOL_UNLOCK() sal_sem_give(_mpool_lock)

#endif /* __KERNEL__ */

/* Allow external override for system cache line size */
#ifndef BCM_CACHE_LINE_BYTES
#ifdef L1_CACHE_BYTES
#define BCM_CACHE_LINE_BYTES L1_CACHE_BYTES
#else
#define BCM_CACHE_LINE_BYTES 128 /* Should be fine on most platforms */
#endif
#endif

#define MPOOL_BUF_SIZE               1024
#define MPOOL_BUF_ALLOC_COUNT_MAX      16

typedef struct mpool_mem_s {
    unsigned char *address;
    int size;
    struct mpool_mem_s *prev;
    struct mpool_mem_s *next;
} mpool_mem_t;

static int _buf_alloc_count;
static mpool_mem_t *mpool_buf[MPOOL_BUF_ALLOC_COUNT_MAX];
static mpool_mem_t *free_list;

#define ALLOC_INIT_MPOOL_BUF(ptr) \
        ptr = MALLOC((sizeof(mpool_mem_t) * MPOOL_BUF_SIZE)); \
        if (ptr) { \
            int i; \
            for (i = 0; i < MPOOL_BUF_SIZE - 1; i++) { \
                ptr[i].next = &ptr[i+1]; \
            } \
            ptr[MPOOL_BUF_SIZE - 1].next = NULL; \
            free_list = &ptr[0]; \
        }

/*
 * Function: mpool_init
 *
 * Purpose:
 *    Initialize mpool lock.
 * Parameters:
 *    None
 * Returns:
 *    Always 0
 */
int 
mpool_init(void)
{
    MPOOL_LOCK_INIT();
    return 0;
}

#ifdef TRACK_DMA_USAGE
static int _dma_mem_used = 0;
#endif

/*
 * Function: mpool_alloc
 *
 * Purpose:
 *    Allocate memory block from mpool.
 * Parameters:
 *    pool - mpool handle (from mpool_create)
 *    size - size of memory block to allocate
 * Returns:
 *    Pointer to allocated memory block or NULL if allocation fails.
 */
void *
mpool_alloc(mpool_handle_t pool, int size)
{
    mpool_mem_t *ptr = pool, *newptr = NULL;
    int mod;

    MPOOL_LOCK();

    if (size < BCM_CACHE_LINE_BYTES) {
        size = BCM_CACHE_LINE_BYTES;
    }

    mod = size & (BCM_CACHE_LINE_BYTES - 1);
    if (mod != 0 ) {
        size += (BCM_CACHE_LINE_BYTES - mod);
    }
    while (ptr && ptr->next) {
        if (ptr->next->address - (ptr->address + ptr->size) >= size) {
            break;
        }
        ptr = ptr->next;
    }
  
    if (!(ptr && ptr->next)) {
        MPOOL_UNLOCK();
        return NULL;
    }

    if (!free_list) {
        if (_buf_alloc_count == MPOOL_BUF_ALLOC_COUNT_MAX) {
            MPOOL_UNLOCK();
            return NULL;
        }

        ALLOC_INIT_MPOOL_BUF(mpool_buf[_buf_alloc_count]);

        if (mpool_buf[_buf_alloc_count] == NULL) {
            MPOOL_UNLOCK();
            return NULL;
        }

        _buf_alloc_count++;
    }

    newptr = free_list;
    free_list = free_list->next;
  
    newptr->address = ptr->address + ptr->size;
    newptr->size = size;
    newptr->next = ptr->next;
    newptr->prev = ptr;
    ptr->next->prev = newptr;
    ptr->next = newptr;
#ifdef TRACK_DMA_USAGE
    _dma_mem_used += size;
#endif

    MPOOL_UNLOCK();
    return newptr->address;
}


/*
 * Function: mpool_free
 *
 * Purpose:
 *    Free memory block allocated from mpool..
 * Parameters:
 *    pool - mpool handle (from mpool_create)
 *    addr - address of memory block to free
 * Returns:
 *    Nothing
 */
void 
mpool_free(mpool_handle_t pool, void *addr)
{
    unsigned char *address = (unsigned char *)addr;  
    mpool_mem_t *head = pool, *ptr = NULL;

    MPOOL_LOCK();

    if (!(head && head->prev)) {
        MPOOL_UNLOCK();
        return;
    }

    ptr = head->prev->prev;

    while (ptr && (ptr != head)) {
        if (ptr->address == address) {
#ifdef TRACK_DMA_USAGE
            _dma_mem_used -= ptr->size;
#endif
            ptr->prev->next = ptr->next;
            ptr->next->prev = ptr->prev;
            ptr->next = free_list;
            free_list = ptr;
            break;
        }
        ptr = ptr->prev;
    }

    MPOOL_UNLOCK();
}

/*
 * Function: mpool_create
 *
 * Purpose:
 *    Create and initialize mpool control structures.
 * Parameters:
 *    base_ptr - pointer to mpool memory block
 *    size - total size of mpool memory block
 * Returns:
 *    mpool handle
 * Notes
 *    The mpool handle returned must be used for subsequent
 *    memory allocations from the mpool.
 */
mpool_handle_t
mpool_create(void *base_ptr, int size)
{
    mpool_mem_t *head, *tail;
    int mod = (int)(((unsigned long)base_ptr) & (BCM_CACHE_LINE_BYTES - 1));
    int i;

    MPOOL_LOCK();

    for (i = 0; i < MPOOL_BUF_ALLOC_COUNT_MAX; i++) {
        mpool_buf[i] = NULL;
    }

    _buf_alloc_count = 0;

    ALLOC_INIT_MPOOL_BUF(mpool_buf[_buf_alloc_count]);

    if (mpool_buf[_buf_alloc_count] == NULL) {
        MPOOL_UNLOCK();
        return NULL;
    }

    _buf_alloc_count++;

    if (mod) {
        base_ptr = (char*)base_ptr + (BCM_CACHE_LINE_BYTES - mod);
        size -= (BCM_CACHE_LINE_BYTES - mod);
    }
    size &= ~(BCM_CACHE_LINE_BYTES - 1);

    head = free_list;
    free_list = free_list->next;
    tail = free_list;
    free_list = free_list->next;

    head->size = tail->size = 0;
    head->address = base_ptr;
    tail->address = head->address + size;
    head->prev = tail;
    head->next = tail;
    tail->prev = head;
    tail->next = NULL;

    MPOOL_UNLOCK();
    return head;
}

/*
 * Function: mpool_destroy
 *
 * Purpose:
 *    Free mpool control structures.
 * Parameters:
 *    pool - mpool handle (from mpool_create)
 * Returns:
 *    Always 0
 */
int
mpool_destroy(mpool_handle_t pool)
{
    int i;

    MPOOL_LOCK();

    if ((mpool_mem_t *)pool != mpool_buf[0]) {
        MPOOL_UNLOCK();
        return 0;
    }

    for (i = 0; i < MPOOL_BUF_ALLOC_COUNT_MAX; i++) {
        if (mpool_buf[i]) {
            FREE(mpool_buf[i]);
            mpool_buf[i] = NULL;
        }
    }

    MPOOL_UNLOCK();

    return 0;
}

/*
 * Function: mpool_usage
 *
 * Purpose:
 *    Report total sum of allocated mpool memory.
 * Parameters:
 *    pool - mpool handle (from mpool_create)
 * Returns:
 *    Number of bytes currently allocated using mpool_alloc.
 */
int
mpool_usage(mpool_handle_t pool)
{
    int usage = 0;
    mpool_mem_t *ptr;

    MPOOL_LOCK();

    for (ptr = pool; ptr; ptr = ptr->next) {
        usage += ptr->size;
    }

    MPOOL_UNLOCK();

    return usage;
}
