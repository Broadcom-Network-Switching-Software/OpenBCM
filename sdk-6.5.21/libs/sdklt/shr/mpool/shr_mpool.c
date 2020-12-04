/*! \file shr_mpool.c
 *
 * Simply memory allocator designed to manage SDK DMA memory from a
 * large pre-allocated block of physically contiguous memory.
 *
 * The allocator relies on a linked list of allocation descriptors, so
 * it does not scale well.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_spinlock.h>
#include <sal/sal_libc.h>
#include <shr/shr_mpool.h>

/*******************************************************************************
 * Local definitions
 */

/* Allow external override for system cache line size */
#ifndef MPOOL_CACHE_LINE_SIZE
#ifdef L1_CACHE_BYTES
#define MPOOL_CACHE_LINE_SIZE   L1_CACHE_BYTES
#else
#define MPOOL_CACHE_LINE_SIZE   128 /* Should be fine on most platforms */
#endif
#endif

/*! Allocation chunk size should be aligned with the cache-line size. */
#define MPOOL_CHUNK_SIZE        (1 * MPOOL_CACHE_LINE_SIZE)

/*! Minimum pool size for meaningful operation. */
#define MPOOL_SIZE_MIN          MPOOL_CHUNK_SIZE

typedef struct shr_mpool_mem_s {
    char sig[8];
    sal_spinlock_t lock;
    uint8_t *addr;
    size_t size;
    struct shr_mpool_mem_s *next;
} shr_mpool_mem_t;

/*******************************************************************************
 * Public functions
 */

shr_mpool_handle_t
shr_mpool_create(void *base_ptr, size_t size, size_t chunk_size)
{
    shr_mpool_mem_t *head, *tail;
    unsigned long mod;
    uint8_t *mpool_base = base_ptr;

    if (mpool_base == NULL) {
        return NULL;
    }

    if (size < MPOOL_SIZE_MIN) {
        return NULL;
    }

    /* First chunk needs to contain head, tail and alignment */
    if ((3 * sizeof(shr_mpool_mem_t)) > MPOOL_CHUNK_SIZE) {
        return NULL;
    }

    /* Align mpool_base to chunk size if needed. */
    mod = (unsigned long)mpool_base & (MPOOL_CHUNK_SIZE - 1);
    if (mod) {
        mpool_base = mpool_base + (MPOOL_CHUNK_SIZE - mod);
        size -= (MPOOL_CHUNK_SIZE - mod);
    }

    /* Align pool size to chunk size if needed. */
    size &= ~(MPOOL_CHUNK_SIZE - 1);

    head = (shr_mpool_mem_t *)mpool_base;
    tail = &head[1];

    head->size = tail->size = 0;
    head->addr = mpool_base + MPOOL_CHUNK_SIZE;
    tail->addr = head->addr + size - MPOOL_CHUNK_SIZE;
    head->next = tail;
    tail->next = NULL;

    /* For debug only */
    sal_strncpy(head->sig, "head", sizeof(head->sig));
    sal_strncpy(tail->sig, "tail", sizeof(tail->sig));

    head->lock = sal_spinlock_create("mpool_lock");
    if (head->lock == NULL) {
        return NULL;
    }

    return head;
}

/*
 * Function: shr_mpool_destroy
 *
 * Purpose:
 *    Free mpool control structures.
 * Parameters:
 *    pool - mpool handle (from mpool_create)
 * Returns:
 *    Always 0
 */
int
shr_mpool_destroy(shr_mpool_handle_t pool)
{
    if (pool == NULL) {
        return -1;
    }

    sal_spinlock_destroy(pool->lock);

    return 0;
}

/*
 * Function: shr_mpool_alloc
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
shr_mpool_alloc(shr_mpool_handle_t pool, size_t size)
{
    shr_mpool_mem_t *mptr = pool, *new_mptr = NULL;
    size_t free_gap;
    uint8_t *new_addr;
    int mod;

    if (pool == NULL) {
        return NULL;
    }

    sal_spinlock_lock(pool->lock);

    size += sizeof(shr_mpool_mem_t);

    mod = size & (MPOOL_CHUNK_SIZE - 1);
    if (mod != 0 ) {
        size += (MPOOL_CHUNK_SIZE - mod);
    }
    while (mptr && mptr->next) {
        free_gap = mptr->next->addr - (mptr->addr + mptr->size);
        if (free_gap >= size) {
            break;
        }
        mptr = mptr->next;
    }

    if (!(mptr && mptr->next)) {
        sal_spinlock_unlock(pool->lock);
        return NULL;
    }

    new_addr = mptr->addr + mptr->size;
    new_mptr = (shr_mpool_mem_t *)(new_addr - sizeof(shr_mpool_mem_t));
    new_mptr->addr = new_addr;
    new_mptr->size = size;
    new_mptr->next = mptr->next;
    mptr->next = new_mptr;

    /* For debug only */
    sal_strncpy(new_mptr->sig, "alloc", sizeof(new_mptr->sig));

    sal_spinlock_unlock(pool->lock);

    return new_mptr->addr;
}


/*
 * Function: shr_mpool_free
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
shr_mpool_free(shr_mpool_handle_t pool, void *ptr)
{
    uint8_t *addr = (uint8_t *)ptr;
    shr_mpool_mem_t *mptr = pool, *prev = NULL;

    if (pool == NULL) {
        return;
    }

    sal_spinlock_lock(pool->lock);

    while (mptr && mptr->next) {
        if (mptr->next->addr == addr) {
            break;
        }
        mptr = mptr->next;
    }

    if (mptr && mptr->next) {
        prev = mptr;
        mptr = mptr->next;
        prev->next = mptr->next;
        mptr = (shr_mpool_mem_t *)(addr - sizeof(shr_mpool_mem_t));
        sal_memset(mptr, 0, sizeof(shr_mpool_mem_t));
    }

    sal_spinlock_unlock(pool->lock);
}

/*
 * Function: shr_mpool_usage
 *
 * Purpose:
 *    Report total sum of allocated mpool memory.
 * Parameters:
 *    pool - mpool handle (from mpool_create)
 * Returns:
 *    Number of bytes currently allocated using mpool_alloc.
 */
int
shr_mpool_usage(shr_mpool_handle_t pool)
{
    int usage = 0;
    shr_mpool_mem_t *mptr;

    if (pool == NULL) {
        return -1;
    }

    sal_spinlock_lock(pool->lock);

    for (mptr = pool; mptr; mptr = mptr->next) {
	usage += mptr->size;
    }

    sal_spinlock_unlock(pool->lock);

    return usage;
}

size_t
shr_mpool_chunk_size(shr_mpool_handle_t pool)
{
    return MPOOL_CHUNK_SIZE;
}
