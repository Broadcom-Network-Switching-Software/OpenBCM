/*! \file bcmbd_m0ssq_fifo_internal.h
 *
 *      M0SSQ FIFO driver provides FIFO methods for MBOX driver.
 *  A FIFO is built on top of memory block which is accessible
 *  for host processor and M0 uC. The read/write pointer of FIFO
 *  are kept in the memory block.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_M0SSQ_FIFO_INTERNAL_H
#define BCMBD_M0SSQ_FIFO_INTERNAL_H

#include <sal/sal_types.h>
#include <sal/sal_mutex.h>

#include <bsl/bsl.h>

#include <shr/shr_debug.h>

#include <bcmbd/bcmbd_m0ssq_internal.h>

/*******************************************************************************
 * M0SSQ FIFO driver.
 */

/*!
 * \brief FIFO driver data.
 */
typedef struct bcmbd_m0ssq_fifo_s {

    /*! FIFO's memory block. */
    bcmbd_m0ssq_mem_t mem;

    /*! Per FIFO entry size in byte. */
    uint32_t per_entry_size;

    /*! Size of available data space. */
    uint32_t size;

    /*! Mutex lock for FIFO usage. */
    sal_mutex_t  lock;

} bcmbd_m0ssq_fifo_t;

/*!
 * \brief FIFO control pointer.
 */
typedef struct bcmbd_m0ssq_fifo_ptr_s {

    /*! head of fifo. */
    uint32_t rp;

    /*! tail of fifo. */
    uint32_t wp;

} bcmbd_m0ssq_fifo_ptr_t;

/*!
 * \brief Initialize a FIFO instance.
 *
 * \param [in] fifo FIFO instance.
 * \param [in] mem Memory block for FIFO usage.
 * \param [in] ent_size Size of each FIFO entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL if FIFO initialization failed.
 */
extern int
bcmbd_m0ssq_fifo_init(bcmbd_m0ssq_fifo_t *fifo,
                      bcmbd_m0ssq_mem_t *mem,
                      uint32_t ent_size);

/*!
 * \brief Cleanup resource of a FIFO instance.
 *
 * \param [in] fifo FIFO instance.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL if FIFO cleanup failed.
 */
extern int
bcmbd_m0ssq_fifo_cleanup(bcmbd_m0ssq_fifo_t *fifo);

/*!
 * \brief Acquire lock for a FIFO.
 *
 * \param [in] fifo FIFO instance.
 */
extern void
bcmbd_m0ssq_fifo_lock(bcmbd_m0ssq_fifo_t *fifo);

/*!
 * \brief Release lock for a FIFO.
 *
 * \param [in] fifo FIFO instance.
 */
extern void
bcmbd_m0ssq_fifo_unlock(bcmbd_m0ssq_fifo_t *fifo);


/*!
 * \brief Get FIFO write pointer.
 *
 * \param [in] fifo FIFO instance.
 *
 * \retval Value of write pointer.
 */
extern uint32_t
bcmbd_m0ssq_fifo_wp_get(bcmbd_m0ssq_fifo_t *fifo);


/*!
 * \brief Get FIFO read pointer.
 *
 * \param [in] fifo FIFO instance.
 *
 * \retval Read pointer.
 */
extern uint32_t
bcmbd_m0ssq_fifo_rp_get(bcmbd_m0ssq_fifo_t *fifo);

/*!
 * \brief Check if there's enough free space for write.
 *
 * \param [in] fifo FIFO instance.
 * \param [in] size Size of write data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL if no enough free space for write.
 */
extern int
bcmbd_m0ssq_fifo_write_check(bcmbd_m0ssq_fifo_t *fifo,
                             uint32_t size);


/*!
 * \brief  Check if there's valid data for read.
 *
 * \param [in] fifo FIFO instance.
 * \param [in] size Size of read data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL If no enough valid data for read.
 */
extern int
bcmbd_m0ssq_fifo_read_check(bcmbd_m0ssq_fifo_t *fifo,
                            uint32_t size);

/*!
 * \brief Move FIFO's read pointer forward.
 *
 * \param [in] fifo FIFO instance.
 * \param [in] size Forward step size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed.
 */
extern int
bcmbd_m0ssq_fifo_rp_forward(bcmbd_m0ssq_fifo_t *fifo,
                            uint32_t size);

/*!
 * \brief Move FIFO's write pointer forward.
 *
 * \param [in] fifo FIFO instance.
 * \param [in] size Forward step size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed.
 */
extern int
bcmbd_m0ssq_fifo_wp_forward(bcmbd_m0ssq_fifo_t *fifo,
                            uint32_t size);

/*!
 * \brief Write data from specific position of FIFO.
 *
 *
 * \param [in] fifo FIFO instance.
 * \param [in] pos  Starting write position within FIFO.
 * \param [in] data Buffer for write data.
 * \param [in] len  Data length for data write. It must be
 *                  multiple of per_entry_size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL if FIFO initialization failed.
 */
extern int
bcmbd_m0ssq_fifo_pos_write(bcmbd_m0ssq_fifo_t *fifo,
                           uint32_t pos,
                           void* data, uint32_t len);


/*!
 * \brief Read data from specific position of FIFO.
 *
 *
 * \param [in]  fifo FIFO instance.
 * \param [in]  pos  Starting write position within FIFO.
 * \param [out] data Buffer for read data.
 * \param [in]  len  Data length of read data. It must be
 *                   multiple of per_entry_size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL if FIFO initialization failed.
 */
extern int
bcmbd_m0ssq_fifo_pos_read(bcmbd_m0ssq_fifo_t *fifo,
                          uint32_t pos,
                          void* data, uint32_t len);

/*!
 * \brief Write data from write pointer of FIFO.
 *
 * This function
 *   - Write data from write pointer of FIFO.
 *   - Move write pointer forward with size.
 *
 * \param [in]  fifo FIFO instance.
 * \param [in]  data Buffer for write data.
 * \param [in]  len  Data length of write data. It must be
 *                   multiple of per_entry_size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL if FIFO initialization failed.
 */
extern int
bcmbd_m0ssq_fifo_write(bcmbd_m0ssq_fifo_t *fifo,
                       void* data, uint32_t len);

/*!
 * \brief Peek data from read pointer of FIFO.
 *
 * This function only read data from write pointer of FIFO.
 * But the read pointer will not be moved forward.
 *
 * \param [in]  fifo FIFO instance.
 * \param [in]  data Buffer for read data.
 * \param [in]  len  Data length for read data. It must be
 *                   multiple of per_entry_size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL if FIFO initialization failed.
 */
extern int
bcmbd_m0ssq_fifo_peek_read(bcmbd_m0ssq_fifo_t *fifo,
                           void* data, uint32_t len);

/*!
 * \brief Read data from read pointer of FIFO.
 *
 * This function
 *     - Read data from write pointer of FIFO.
 *     - Move write pointer forward with size.
 *
 * \param [in]  fifo FIFO instance.
 * \param [in]  data Buffer for read data.
 * \param [in]  len  Data length for read data. It must be
 *                   multiple of per_entry_size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL if FIFO initialization failed.
 */
extern int
bcmbd_m0ssq_fifo_read(bcmbd_m0ssq_fifo_t *fifo,
                      void* data, uint32_t len);


#endif /* BCMBD_M0SSQ_FIFO_INTERNAL_H */
