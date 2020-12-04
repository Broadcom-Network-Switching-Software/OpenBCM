/*! \file bcmpkt_bpool.c
 *
 * BCMPKT buffer pool library is to provide simple packet buffer pool solution
 * for user to get efficiency. Each pool contains same size buffers. To create a
 * buffer pool, the user need to assign buffer size and total buffer number
 * supported in this pool.
 *
 * The user may create one buffer pool for each device, which is indexed by
 * \c unit. The \c unit = BCMPKT_BPOOL_SHARED_ID means shared buffer pool.
 *
 * BCMPKT buffer pool API supports create, destroy or cleanup buffer pool(s).
 * After a pool created, the user may alloc buffer from the pool or free an
 * allocated buffer into it.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef PKTIO_IMPL
#include <pktio_dep.h>
#else
#include <sal/sal_spinlock.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_timeout.h>
#include <sal/sal_sleep.h>
#include <bsl/bsl.h>
#endif
#include <bcmpkt/bcmpkt_buf.h>
#include <bcmpkt/bcmpkt_internal.h>
#include <bcmpkt/bcmpkt_net.h>
#include <bcmdrd/bcmdrd_dev.h>
#include "bcmpkt_buf_internal.h"

#define BSL_LOG_MODULE              BSL_LS_BCMPKT_PACKET

#ifndef BCMPKT_CACHE_LINE_SIZE
#ifdef L1_CACHE_BYTES
#define BCMPKT_CACHE_LINE_SIZE      L1_CACHE_BYTES
#else
#define BCMPKT_CACHE_LINE_SIZE      128
#endif
#endif

/*! Shared plus all devices. */
#define BCMPKT_BPOOL_COUNT          BCMDRD_CONFIG_MAX_UNITS + 1

/* 0 - shared pool, 1 - unit 0 pool... */
#define PIDX(_u)                    (_u + 1)

/*! For a free buffer, the first word is used as a pointer to the next free. */
#define BP_BUF_NEXT(_buf)           (((uint8_t **)(_buf))[0])

/*! Align to cache edge. */
#define BCMPKT_BPOOL_ALIGN(_a) \
    ((_a + (BCMPKT_CACHE_LINE_SIZE - 1)) & ~(BCMPKT_CACHE_LINE_SIZE - 1))

/*! Packet buffer pool structure. */
typedef struct bcmpkt_bpool_s {
    /* Next pool in linked list. */
    struct bcmpkt_bpool_s *next;
    /* Active means may allocate buffer from it. */
    bool active;
    /* Allocated pool memory pointer. */
    uint8_t *mem;
    /* Next byte to the end, for valid check. */
    uint8_t *mem_end;
    /* Pool merory size. */
    uint32_t mem_size;
    /* Each buffer size. */
    uint32_t buf_size;
    /* Buffer number. */
    uint32_t buf_count;
    /* Increase for each free from buf_count. */
    uint32_t free_count;
    /* Point to the first buffer in free list. */
    uint8_t *free_list;
    /* Buffer pool protection lock. */
    sal_spinlock_t lock;
    /* DMA address. */
    uint64_t dma_addr;
} bcmpkt_bpool_t;

/*! Per device buffer pools. 0 for shared, 1 for unit 0... */
static bcmpkt_bpool_t bps[BCMPKT_BPOOL_COUNT];

int
bcmpkt_bpool_create(int unit, int buf_size, int buf_count)
{
    bcmpkt_bpool_t *bp = NULL;
    uint8_t *buf, *next_buf;
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    if (!(unit == BCMPKT_BPOOL_SHARED_ID || bcmdrd_dev_exists(unit))) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    if (buf_size < (int)BCMPKT_BPOOL_BSIZE_MIN) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_PARAM,
             (BSL_META_U(unit, "bpool size %d too small - must be >= %d\n"),
              buf_size, BCMPKT_BPOOL_BSIZE_MIN));
    }
    buf_size += BCMPKT_BUF_SIZE_OVERHEAD;

    if (buf_count <= 0) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_PARAM,
             (BSL_META_U(unit, "Invalid buffer count %d\n"),
              buf_count));
    }

    if (bps[PIDX(unit)].mem) {
        SHR_RETURN_VAL_EXIT(SHR_E_EXISTS);
    }
    bp = &bps[PIDX(unit)];

    sal_memset(bp, 0, sizeof(*bp));
    bp->buf_size = BCMPKT_BPOOL_ALIGN(buf_size);
    bp->buf_count = (uint32_t)buf_count;
    bp->free_count = (uint32_t)buf_count;
    bp->mem_size = bp->buf_size * bp->buf_count;
    /* Overflow. */
    if (bp->mem_size / bp->buf_count != bp->buf_size ) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    if (unit == BCMPKT_BPOOL_SHARED_ID) {
        bp->mem = sal_alloc(bp->mem_size, "bcmpktBufPoolCreate");
    } else {
        bp->mem = bcmdrd_hal_dma_alloc(unit, bp->mem_size,
                                       "bcmpktBufPoolCreate", &bp->dma_addr);
    }
    SHR_NULL_CHECK(bp->mem, SHR_E_MEMORY);
    sal_memset(bp->mem, 0, bp->mem_size);
    bp->mem_end = bp->mem + bp->mem_size;

    buf = bp->mem;
    bp->free_list = buf;
    for (i = 0; i < bp->buf_count - 1; i++) {
        next_buf = buf + bp->buf_size;
        BP_BUF_NEXT(buf) = next_buf;
        buf = next_buf;
    }
    BP_BUF_NEXT(buf) = NULL;

    bp->active = true;
    bp->lock = sal_spinlock_create("bcmpktBufPoolCreate");
    SHR_NULL_CHECK(bp->lock, SHR_E_MEMORY);

exit:
    if (SHR_FUNC_ERR() && (bp)) {
        if (bp->mem) {
            if (unit == BCMPKT_BPOOL_SHARED_ID) {
                sal_free(bp->mem);
            } else {
                bcmdrd_hal_dma_free(unit, bp->mem_size, bp->mem, bp->dma_addr);
            }
        }
        sal_memset(bp, 0, sizeof(*bp));
    }
    SHR_FUNC_EXIT();
}

int
bcmpkt_bpool_destroy(int unit)
{
    bcmpkt_bpool_t *bp = NULL;
    shr_timeout_t to;
    sal_usecs_t timeout_usec = 1000000;
    int min_polls = 10;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (!(unit == BCMPKT_BPOOL_SHARED_ID || bcmdrd_dev_exists(unit))) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    bp = &bps[PIDX(unit)];
    if (!bp->mem) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    sal_spinlock_lock(bp->lock);
    bp->active = false;
    sal_spinlock_unlock(bp->lock);

    /*
     * Waiting for all buffer released.
     */
    shr_timeout_init(&to, timeout_usec, min_polls);
    while (true) {
        if (bp->free_count == bp->buf_count) {
            break;
        }
        if (shr_timeout_check(&to)) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "Operation timed out\n")));
            break;
        }
    }

    if (bp->lock) {
        sal_spinlock_destroy(bp->lock);
        bp->lock = NULL;
    }

    /* If timeout, delay 100ms for protect abnormal free after destroyed. */
    if (shr_timeout_check(&to)) {
        sal_usleep(100000);
    }

    if (unit == BCMPKT_BPOOL_SHARED_ID) {
        sal_free(bp->mem);
    } else {
        bcmdrd_hal_dma_free(unit, bp->mem_size, bp->mem, bp->dma_addr);
    }

    sal_memset(bp, 0, sizeof(*bp));
exit:
    SHR_FUNC_EXIT();

}

int
bcmpkt_bpool_cleanup(void)
{
    int i;

    for (i = 0; i < BCMPKT_BPOOL_COUNT; i++) {
        bcmpkt_bpool_destroy(i - 1);
    }

    return SHR_E_NONE;
}

/* This is internal function, 'unit' parameter should be checked before
 * calling it and buf_size should not be NULL.
 */
void *
bcmpkt_bpool_alloc(int unit, uint32_t size, uint32_t *buf_size)
{
    bcmpkt_bpool_t *bp = NULL;
    void *buf;

    bp = &bps[PIDX(unit)];

    /* This is also for the case that the pool was not created (buf_size = 0). */
    if (size > bp->buf_size) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Size error: request %"PRIu32", buf_size %"PRIu32"\n"),
                   size, bp->buf_size));
        return NULL;
    }

    if (sal_spinlock_lock(bp->lock)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Lock failed\n")));
        sal_spinlock_unlock(bp->lock);
        return NULL;
    }

    if ((!bp->active) || bp->free_count < 1) {
        sal_spinlock_unlock(bp->lock);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Buffer pool is not available\n")));
        return NULL;
    }

    buf = bp->free_list;
    bp->free_list = BP_BUF_NEXT(bp->free_list);
    bp->free_count--;

    sal_spinlock_unlock(bp->lock);

    *buf_size = bp->buf_size;

    return buf;
}

/* This is internal function, 'unit' parameter should be checked before
 * calling it.
 */
int
bcmpkt_bpool_free(int unit, void *buf)
{
    bcmpkt_bpool_t *bp = NULL;
    bcmpkt_data_buf_t *dbuf = (bcmpkt_data_buf_t *)buf;

    SHR_FUNC_ENTER(unit);

    bp = &bps[PIDX(unit)];
    if (!(bp->mem && bp->lock)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }

    if ((uint8_t *)buf < bp->mem ||
        (uint8_t *)buf > bp->mem_end) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_MEMORY,
             (BSL_META_U(unit, "The buffer does not belong to pool %d\n"),
              unit));
    }

    if ((--dbuf->ref_count) > 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /* Avoid double free crashes buffer pool link list. */
    if (dbuf->ref_count < 0) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_PARAM,
             (BSL_META_U(unit, "Unexpected free\n")));
    }

    SHR_IF_ERR_EXIT
        (sal_spinlock_lock(bp->lock));

    BP_BUF_NEXT(buf) = bp->free_list;
    bp->free_list = buf;
    bp->free_count++;

    sal_spinlock_unlock(bp->lock);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_bpool_status_get(int unit, bcmpkt_bpool_status_t *status)
{
    bcmpkt_bpool_t *bp = NULL;

    SHR_FUNC_ENTER(unit);

    if (!(unit == BCMPKT_BPOOL_SHARED_ID || bcmdrd_dev_exists(unit))) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }
    SHR_NULL_CHECK(status, SHR_E_PARAM);

    bp = &bps[PIDX(unit)];
    status->active = bp->active;
    status->buf_size = bp->buf_size;
    status->buf_count = bp->buf_count;
    status->free_count = bp->free_count;

exit:
    SHR_FUNC_EXIT();
}

static void
bpool_info_dump(shr_pb_t *pb, bcmpkt_bpool_t *bp)
{
    uint32_t real_size = bp->buf_size - sizeof(bcmpkt_data_buf_t);
    uint32_t txrx_size = real_size - BCMPKT_TX_HDR_RSV;

    shr_pb_printf(pb, "Active: %s\n", bp->active ? "true" : "false");
    shr_pb_printf(pb, "Memory %p - %p\n", bp->mem, bp->mem_end);
    shr_pb_printf(pb, "Memory size: %"PRIu32"\n", bp->mem_size);
    shr_pb_printf(pb, "Buffer size: %"PRIu32"\n", bp->buf_size);
    shr_pb_printf(pb, "Buffer usable size: %"PRIu32"\n", real_size);
    shr_pb_printf(pb, "Buffer usable size for TX/RX: %"PRIu32"\n", txrx_size);
    shr_pb_printf(pb, "Buffer count: %"PRIu32"\n", bp->buf_count);
    shr_pb_printf(pb, "Free count: %"PRIu32"\n", bp->free_count);
    shr_pb_printf(pb, "First free address: %p\n", bp->free_list);
    shr_pb_printf(pb, "DMA address:  0x%"PRIu64"\n", bp->dma_addr);
}

int
bcmpkt_bpool_info_dump(int unit, shr_pb_t *pb)
{

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pb, SHR_E_PARAM);

    if (!(unit == BCMPKT_BPOOL_SHARED_ID || bcmdrd_dev_exists(unit))) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

   if (bps[PIDX(unit)].mem) {
        shr_pb_printf(pb, "Unit %d pool information:\n", unit);
        bpool_info_dump(pb, &bps[PIDX(unit)]);
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

