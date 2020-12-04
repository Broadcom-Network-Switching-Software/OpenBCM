/*! \file shr_mpool.h
 *
 * Simple memory manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SHR_MPOOL_H
#define SHR_MPOOL_H

#include <sal/sal_types.h>

/*! Opaque memory pool handle. */
struct shr_mpool_mem_s;

/*! Handle to be used for all operations on a memory pool. */
typedef struct shr_mpool_mem_s *shr_mpool_handle_t;

/*!
 * \brief Create and initialize mpool control structures.
 *
 * The mpool module is a very simple memory allocator, which can be
 * used to manage a fixed block of memory.
 *
 * The allocated memory will be aligned to a minimum chunk size, in
 * order to minimize fragmentation. No other measures are taken to
 * prevent fragmentation.
 *
 * Although memory can be allocated an freed dynamically, it is
 * recommended that a driver allocates as much memory as it needs
 * up-front. Such a strategy will ensure that any memory constraints
 * are detected during initialization instead of after hours/days of
 * operation.
 *
 * \param [in] base_address Pointer to mpool memory block.
 * \param [in] size Total size of mpool memory block.
 * \param [in] chunk_size Minimum allocation size (currently unsupported).
 *
 * \return mpool handle or NULL on error.
 */
extern shr_mpool_handle_t
shr_mpool_create(void* base_address, size_t size, size_t chunk_size);

/*!
 * \brief Free mpool control structures.
 *
 * \param [in] pool mpool handle (from \ref shr_mpool_create).
 *
 * \retval 0 No errors.
 */
extern int
shr_mpool_destroy(shr_mpool_handle_t pool);

/*!
 * \brief Allocate block of memory.
 *
 * The allocated memory is uninitialized.
 *
 * \param [in] pool mpool handle (from \ref shr_mpool_create).
 * \param [in] size Minimum size of memory to allocate.
 *
 * \return Pointer to allocated memory or NULL on error.
 */
extern void *
shr_mpool_alloc(shr_mpool_handle_t pool, size_t size);

/*!
 * \brief Free block of memory.
 *
 * Free memory block allocated with \ref shr_mpool_alloc.
 *
 * \param [in] pool mpool handle (from \ref shr_mpool_create).
 * \param [in] ptr Pointer to memory to free.
 */
extern void
shr_mpool_free(shr_mpool_handle_t pool, void* ptr);

/*!
 * \brief Return amount of memory currently allocated.
 *
 * \param [in] pool mpool handle (from \ref shr_mpool_create).
 *
 * \return Total amount of allocated memory.
 */
extern int
shr_mpool_usage(shr_mpool_handle_t pool);

/*!
 * \brief Return allocation unit size.
 *
 * This function is mainly for debug and test purposes.
 *
 * Each allocation will be made from chunks of the allocation unit
 * size, e.g. requesting just a single byte of memory will still
 * consume a full allocation unit.
 *
 * \param [in] pool mpool handle (from \ref shr_mpool_create).
 *
 * \return Allocation unit size.
 */
extern size_t
shr_mpool_chunk_size(shr_mpool_handle_t pool);

#endif  /* SHR_MPOOL_H */
