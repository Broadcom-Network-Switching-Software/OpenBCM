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
#include <shr/shr_ha_types.h>

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

/*!
 * \brief Allocate memory block with string ID.
 *
 * This function is used to allocate a block of HA memory and
 * associate it with a particular unit. The component and
 * sub-component IDs (\c mod_id, \c sub_id respectively) are in
 * essence pointers to non-volatile memory. When the application warm
 * boots, the component and sub-component IDs can be used to retrieve
 * the same memory content that was used in the previous run. Each
 * software component should use a pre-allocated component ID from a
 * shared header file, while the sub-component IDs should be managed
 * internally by the component.
 *
 * In case of a software upgrade (ISSU), the HA memory manager will
 * use the auto-generated structure signature (\c struct_sig) to
 * perform an automatic upgrade of the data structure.
 *
 * For generic (non-unit) HA memory allocations, use \ref
 * BCMHA_GEN_UNIT as the unit number.
 *
 * \param [in] unit Unit number to be associated with this HA memory.
 * \param [in] comp_id Pre-allocated component ID.
 * \param [in] sub_id Internal ID managed by the component.
 * \param [in] id_str ID string to be associated with the allocated
 *                    memory block. This ID makes it easier to
 *                    determine the owner of an HA block. If this
 *                    parameter is NULL, the original string (in case
 *                    of warm boot) remains unchanged.
 * \param [in,out] length This parameter used both as input and
 *                        output. For input it specifies the required
 *                        memory size. For output it is the actual
 *                        size of the allocated memory block. The
 *                        output value has meaning during warm boot
 *                        where the requested size might be different
 *                        from the actual size of the block that was
 *                        allocated in the previous run.
 *
 * \return On success, the function returns pointer to the desired
 *         memory, otherwise it returns NULL.
 */
extern void *
bcmha_mem_alloc(int unit,
                shr_ha_mod_id comp_id, shr_ha_sub_id sub_id,
                const char *id_str, uint32_t *length);

/*!
 * \brief Extend an existing HA memory block.
 *
 * This function extends an already allocated HA memory block to a
 * desired length.
 *
 * \param [in] unit Unit number associated with this HA memory.
 * \param [in] mem Pointer to the HA memory block to extend.
 * \param [in] length Desired new length of the HA memory block.
 *
 * \return On success, the function returns pointer to the externded
 *         memory, otherwise it returns NULL.
 */
extern void *
bcmha_mem_realloc(int unit, void *mem, uint32_t length);

/*!
 * \brief Free previously allocated HA memory block.
 *
 * This function should rarely be used, and it is mainly here for
 * completeness of the API.
 *
 * The function returns a HA memory block back into the HA memory
 * pool. Once the block has been returned, it can be reused by the HA
 * manager for future requests of the same or smaller size. Since this
 * can potentially cause memory fragmentation, it is recommendeded to
 * avoid continuous allocation and freeing of HA memory during
 * run-time.
 *
 * Since the application is allocating the HA memory, there is no good
 * reason to free the HA memory upon unit shutdown. If necessary, the
 * application may release the entire HA memory pool back to the
 * operating system. Otherwise, the HA memory may be reused in a
 * subsequent following warm boot.
 *
 * \param [in] unit Unit number associated with this HA memory.
 * \param [in] mem HA memory block to free.
 *
 * \return SHR_E_NONE on success, otherwise failure freeing the memory.
 */
extern int
bcmha_mem_free(int unit, void *mem);

#endif /* BCMHA_MEM_H */
