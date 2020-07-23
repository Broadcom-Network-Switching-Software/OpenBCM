/*! \file bcmha_mem.h
 *
 * APIs for managinging HA memory.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMHA_MEM_H
#define BCMHA_MEM_H

#include <sal/sal_types.h>

/*!
 * Convenience macro for specifying the generic HA memory instance,
 * i.e. the instance which is not associated with any specific device.
*/
#define BCMHA_GEN_UNIT  -1

/*!
 * \brief Allocate or grow a HA memory pool.
 *
 * A function of this type must be provided for each HA memory
 * instance in use.
 *
 * When called by the HA memory manager, the function should attempt
 * to allocate a new pool of the requested size.
 *
 * Some pool allocators can only allocate new HA memory in chunks of a
 * certain size. In this case, the value returned in \c actual_size
 * can be greater than the \c requested_size value.
 *
 * \param [in] pool_ctx Context provided during HA memory instance
 *                      initialization.
 * \param [in] requested_size Amount of new HA memory needed.
 * \param [out] actual_size Actual size of added HA memory.
 *
 * \return Pointer to new or existing HA memory pool.
 */
typedef void *(*bcmha_mem_pool_alloc_f)(void *pool_ctx,
                                        uint32_t requested_size,
                                        uint32_t *actual_size);

/*!
 * \brief Free a HA memory pool.
 *
 * A function of this type will be called by \ref bcmha_mem_cleanup
 * for each HA memory pool in use.
 *
 * \param [in] pool_ctx Context provided during HA initialization.
 * \param [in] pool_ptr Pointer to HA pool to be freed.
 * \param [in] pool_size Size of HA pool to be freed.
 *
 * \return Pointer to new or existing HA memory pool.
 */
typedef void (*bcmha_mem_pool_free_f)(void *pool_ctx,
                                      void *pool_ptr,
                                      uint32_t pool_size);

/*!
 * \brief Initialize unit HA memory.
 *
 * This function should be called for any newly discovered unit prior
 * to calling \c bcmlt_device_attach().
 *
 * The HA memory manager will use the \c pool_alloc callback function
 * to allocate HA memory pools from the application. Please see \ref
 * bcmha_mem_pool_alloc_f for details on how to implement this
 * function.
 *
 * The \c pool_free function will be used to free the HA memory blocks
 * when \ref bcmha_mem_cleanup is called. The application will decide
 * what the appropriate action is for the type of HA memory being
 * used.
 *
 * To create a new HA memory block (cold boot), the \c pool_ptr
 * pointer must be set to NULL. To reuse an existing HA memory block
 * (warm boot), the \c pool_ptr must point to this block and \c
 * pool_size must be set accordingly. In this case the HA memory block
 * is expected to be a single contiguous block of memory, even if it
 * was previously created via multiple calls the \c pool_alloc
 * function.
 *
 * If \c pool_ptr is NULL and \c pool_size is non-zero, then \c
 * pool_size will be used as a hint for the initial HA memory block
 * allocation.
 *
 * \param [in] unit The unit number associated with this HA instance
 *                  or -1 for the generic instance.
 * \param [in] pool_alloc Application provided callback function to
 *                          allocate or grow a HA memory pool.
 * \param [in] pool_free Application provided callback function to
 *                       free the HA memory block(s).
 * \param [in] pool_ctx Application-provided context, which will be
 *                      passed back whenever \c resize_func function
 *                      is invoked.
 * \param [in] pool_ptr Pointer to an existing HA memory pool.
 * \param [in] pool_size Size of the existing HA memory pool.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmha_mem_init(int unit,
               bcmha_mem_pool_alloc_f pool_alloc,
               bcmha_mem_pool_free_f pool_free,
               void *pool_ctx,
               void *pool_ptr,
               int pool_size);

/*!
 * \brief Release HA memory resources.
 *
 * This function will release all resources allocated via \ref
 * bcmha_mem_init.
 *
 * For each HA memory pool allocated via the \c pool_alloc callback
 * function, the \c pool_free callback function will also be called.
 *
 * If an initial HA memory pool was provided to \ref bcmha_mem_init
 * (warm boot), then the \c pool_free callback function will be called
 * for that pool as well.
 *
 * \param [in] unit The unit number associated with this HA instance
 *                  or -1 for the generic instance.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmha_mem_cleanup(int unit);

/*!
 * \brief Reset all HA memory instance to its initial state.
 *
 * This function must be called when a device is restarted from within
 * a running application.
 *
 * \param [in] unit The unit number associated with this HA instance
 *                  or -1 for the generic instance.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmha_mem_reset(int unit);

#endif /* BCMHA_MEM_H */
