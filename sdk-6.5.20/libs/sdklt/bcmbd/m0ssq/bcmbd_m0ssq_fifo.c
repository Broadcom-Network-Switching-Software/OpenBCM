/*! \file bcmbd_m0ssq_fifo.c
 *
 * M0SSQ FIFO driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <shr/shr_debug.h>

#include <sal/sal_types.h>
#include <sal/sal_assert.h>
#include <sal/sal_mutex.h>

#include <bcmbd/bcmbd_m0ssq_internal.h>
#include <bcmbd/bcmbd_m0ssq_fifo_internal.h>

/*******************************************************************************
 * Local definition
 */
/* Debug log level. */
#define BSL_LOG_MODULE BSL_LS_BCMBD_DEV

/* Macro for FIFO operations. */
#define FIFO_PTR_SIZE       sizeof(bcmbd_m0ssq_fifo_ptr_t)
#define FIFO_PTR_POS        (0)
#define FIFO_PTR_WP_POS     offsetof(bcmbd_m0ssq_fifo_ptr_t, wp)
#define FIFO_PTR_RP_POS     offsetof(bcmbd_m0ssq_fifo_ptr_t, rp)
#define FIFO_WRITE(fifo, pos, data_ptr, len) \
            fifo->mem.write32(&fifo->mem, pos, (uint32_t *) (data_ptr), len)
#define FIFO_READ(fifo, pos, data_ptr, len) \
            fifo->mem.read32(&fifo->mem, pos, (uint32_t *) (data_ptr), len)
#define FIFO_MEM_SIZE(fifo) (fifo->mem.size)
#define FIFO_SIZE(fifo)     (fifo->size)

/*******************************************************************************
 * M0SSQ FIFO driver.
 */
int
bcmbd_m0ssq_fifo_init(bcmbd_m0ssq_fifo_t *fifo,
                      bcmbd_m0ssq_mem_t *mem,
                      uint32_t ent_size)
{

   bcmbd_m0ssq_fifo_ptr_t ptr;
   char mutex_name[64];

   sal_memcpy(&fifo->mem, mem, sizeof(bcmbd_m0ssq_mem_t));
   fifo->per_entry_size = ent_size;
   ptr.wp = FIFO_PTR_SIZE / fifo->per_entry_size;
   ptr.rp = FIFO_PTR_SIZE / fifo->per_entry_size;
   FIFO_WRITE(fifo, FIFO_PTR_POS, &ptr, FIFO_PTR_SIZE);
   fifo->size = mem->size - FIFO_PTR_SIZE;
   sal_snprintf(mutex_name, sizeof(mutex_name), "bcmbdM0ssqFIFO");
   fifo->lock = sal_mutex_create(mutex_name);
   if (fifo->lock == NULL) {
       return SHR_E_FAIL;
   }

   return SHR_E_NONE;
}

int
bcmbd_m0ssq_fifo_cleanup(bcmbd_m0ssq_fifo_t *fifo)
{
    if (fifo->lock) {
        sal_mutex_destroy(fifo->lock);
    }

    return SHR_E_NONE;
}

static uint32_t
bcmbd_m0ssq_fifo_used_size(bcmbd_m0ssq_fifo_t *fifo)
{
   uint32_t used_size;
   bcmbd_m0ssq_fifo_ptr_t ptr;

   FIFO_READ(fifo, FIFO_PTR_POS, &ptr, FIFO_PTR_SIZE);

   ptr.wp *= fifo->per_entry_size;
   ptr.rp *= fifo->per_entry_size;

   if (ptr.wp >= ptr.rp) {
       used_size = (ptr.wp - ptr.rp);
   } else {
       used_size = (FIFO_MEM_SIZE(fifo) - FIFO_PTR_SIZE) - (ptr.rp - ptr.wp);
   }
   return used_size;
}

static uint32_t
bcmbd_m0ssq_fifo_free_size(bcmbd_m0ssq_fifo_t *fifo)
{
   uint32_t free_size;

   free_size = (FIFO_MEM_SIZE(fifo) - FIFO_PTR_SIZE) -
                bcmbd_m0ssq_fifo_used_size(fifo) -
                fifo->per_entry_size;
   return free_size;
}

int
bcmbd_m0ssq_fifo_write_check(bcmbd_m0ssq_fifo_t *fifo,
                             uint32_t size)
{
   if (bcmbd_m0ssq_fifo_free_size(fifo) <= size) {
       /* There is no enough free space. */
       return SHR_E_FAIL;
   }

   /* There is enough free space. */
   return SHR_E_NONE;
}

int
bcmbd_m0ssq_fifo_read_check(bcmbd_m0ssq_fifo_t *fifo,
                            uint32_t size)
{
   if (bcmbd_m0ssq_fifo_used_size(fifo) < size) {
       /* There is no enough data. */
       return SHR_E_EMPTY;
   }

   /* There is enough data in fifo. */
   return SHR_E_NONE;
}

int
bcmbd_m0ssq_fifo_rp_forward(bcmbd_m0ssq_fifo_t *fifo,
                            uint32_t len)
{

   uint32_t rp;

   /* Check whether the length excess valid data. */
   if (bcmbd_m0ssq_fifo_read_check(fifo, len)) {
       return SHR_E_PARAM;
   }

   /* Get current rp. */
   rp = bcmbd_m0ssq_fifo_rp_get(fifo);

   /* Caculate next rp. */
   if ((rp + len) >= FIFO_MEM_SIZE(fifo)) {
       rp = ((rp + len) % FIFO_MEM_SIZE(fifo)) + FIFO_PTR_SIZE;
   } else {
       rp = (rp + len);
   }

   /* Convert point from byte number to entry number. */
   rp /= fifo->per_entry_size;

   /* Update next rp */
   return FIFO_WRITE(fifo, FIFO_PTR_RP_POS, &rp, sizeof(rp));
}

int
bcmbd_m0ssq_fifo_wp_forward(bcmbd_m0ssq_fifo_t *fifo, uint32_t len)
{
   uint32_t wp;

   /* Check whether request length excess valid data amount. */
   if (bcmbd_m0ssq_fifo_write_check(fifo, len)) {
       return SHR_E_PARAM;
   }

   /* Get current wp. */
   wp = bcmbd_m0ssq_fifo_wp_get(fifo);

   /* Caculate next wp. */
   if ((wp + len) >= FIFO_MEM_SIZE(fifo)) {
       wp = ((wp + len) % FIFO_MEM_SIZE(fifo)) + FIFO_PTR_SIZE;
   } else {
       wp = (wp + len);
   }

   /* Convert point from byte number to entry number. */
   wp /= fifo->per_entry_size;

   /* Update next wp */
   return FIFO_WRITE(fifo, FIFO_PTR_WP_POS, &wp, sizeof(wp));
}

/*******************************************************************************
 * M0SSQ FIFO public APIs.
 */
void
bcmbd_m0ssq_fifo_unlock(bcmbd_m0ssq_fifo_t *fifo)
{
    sal_mutex_give(fifo->lock);
}

void
bcmbd_m0ssq_fifo_lock(bcmbd_m0ssq_fifo_t *fifo)
{
    sal_mutex_take(fifo->lock, SAL_MUTEX_FOREVER);
}


uint32_t
bcmbd_m0ssq_fifo_wp_get(bcmbd_m0ssq_fifo_t *fifo)
{
    uint32_t wp;

    assert(fifo);

    FIFO_READ(fifo, FIFO_PTR_WP_POS, &wp, sizeof(wp));

    return (wp * fifo->per_entry_size);
}

uint32_t
bcmbd_m0ssq_fifo_rp_get(bcmbd_m0ssq_fifo_t *fifo)
{
     uint32_t rp;

     assert(fifo);

     FIFO_READ(fifo, FIFO_PTR_RP_POS, &rp, sizeof(rp));

     return (rp * fifo->per_entry_size);
}

int
bcmbd_m0ssq_fifo_pos_write(bcmbd_m0ssq_fifo_t *fifo,
                           uint32_t pos,
                           void* data, uint32_t len)
{
    uint32_t len1, len2;
    int rv;

    /* Parameter sanity check. */
    if ((fifo == NULL) ||
        (data == NULL) ||
        (pos >= FIFO_MEM_SIZE(fifo)) ||
        (pos < FIFO_PTR_SIZE) ||
        (len >= FIFO_SIZE(fifo)) ||
        (len == 0))
    {
        return SHR_E_PARAM;
    }

    /* Write data into FIFO. */
    if ((pos + len) > FIFO_MEM_SIZE(fifo)) {
        len1 = FIFO_MEM_SIZE(fifo) - pos;
        len2 = len - len1;
        rv = FIFO_WRITE(fifo, pos, data, len1);
        if (rv != SHR_E_NONE) {
            return rv;
        }
        rv = FIFO_WRITE(fifo, FIFO_PTR_SIZE,
                       (uint8_t *) data + len1, len2);
    } else {
        rv = FIFO_WRITE(fifo, pos, data, len);
    }

    return rv;
}

int
bcmbd_m0ssq_fifo_pos_read(bcmbd_m0ssq_fifo_t *fifo,
                          uint32_t pos,
                          void* data, uint32_t len)
{
    uint32_t len1, len2;
    int rv;

    /* Parameter sanity check. */
    if ((fifo == NULL) ||
        (data == NULL) ||
        (pos >= FIFO_MEM_SIZE(fifo)) ||
        (pos < FIFO_PTR_SIZE) ||
        (len >= FIFO_SIZE(fifo)) ||
        (len == 0))
    {
        return SHR_E_PARAM;
    }

    /* Read data from FIFO. */
    if ((pos + len) > FIFO_MEM_SIZE(fifo)) {
        len1 = FIFO_MEM_SIZE(fifo) - pos;
        len2 = len - len1;
        rv = FIFO_READ(fifo, pos, data, len1);
        if (rv != SHR_E_NONE) {
            return rv;
        }
        rv = FIFO_READ(fifo, FIFO_PTR_SIZE,
                      (uint8_t *) data + len1, len2);
    } else {
        rv = FIFO_READ(fifo, pos, data, len);
    }

    return rv;
}

int
bcmbd_m0ssq_fifo_write(bcmbd_m0ssq_fifo_t *fifo,
                       void * data, uint32_t len)
{
    uint32_t wp;

    /* Check if there is enough free space to write. */
    if (bcmbd_m0ssq_fifo_write_check(fifo, len)) {
        return SHR_E_UNAVAIL;
    }

    /* Get current write point of FIFO. */
    wp = bcmbd_m0ssq_fifo_wp_get(fifo);

    /* Write data into FIFO. */
    if (bcmbd_m0ssq_fifo_pos_write(fifo, wp, data, len)) {
        return SHR_E_FAIL;
    }

    /* Update write pointer. */
    return bcmbd_m0ssq_fifo_wp_forward(fifo, len);
}

int
bcmbd_m0ssq_fifo_peek_read(bcmbd_m0ssq_fifo_t *fifo,
                           void * data, uint32_t len)
{
    uint32_t rp;
    int rv;

    /* Check if there is enough valid data. */
    rv = bcmbd_m0ssq_fifo_read_check(fifo, len);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Get current read point of FIFO. */
    rp = bcmbd_m0ssq_fifo_rp_get(fifo);

    /* Read data from FIFO. */
    return bcmbd_m0ssq_fifo_pos_read(fifo, rp, data, len);
}

int
bcmbd_m0ssq_fifo_read(bcmbd_m0ssq_fifo_t *fifo,
                      void * data, uint32_t len)
{
    int rv = SHR_E_NONE;

    /* Read data from FIFO. */
    rv = bcmbd_m0ssq_fifo_peek_read(fifo, data, len);

    /* Update read pointer. */
    if (rv == SHR_E_NONE) {
        return bcmbd_m0ssq_fifo_rp_forward(fifo, len);
    };

    return rv;
}
