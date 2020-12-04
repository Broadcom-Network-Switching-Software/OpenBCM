/*! \file ha.h
 *
 * ltsw HA definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM_INT_COMMON_HA_H
#define BCM_INT_COMMON_HA_H

#include <sal/sal_types.h>
#include <shr/shr_ha.h>

typedef shr_ha_mod_id soc_ltsw_ha_mod_id_t;
typedef shr_ha_sub_id soc_ltsw_ha_sub_id_t;
typedef shr_ha_usage_report_t soc_ltsw_ha_usage_report_t;
typedef shr_ha_pool_alloc_f soc_ltsw_ha_pool_alloc_f;
typedef shr_ha_pool_free_f  soc_ltsw_ha_pool_free_f;

#define SOC_LTSW_HA_GEN_UNIT -1
#define SOC_LTSW_HA_SUB_IDS SHR_HA_SUB_IDS

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
soc_ltsw_ha_mem_init(
    int unit,
    soc_ltsw_ha_pool_alloc_f pool_alloc,
    soc_ltsw_ha_pool_free_f pool_free,
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
soc_ltsw_ha_mem_cleanup(int unit);

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
soc_ltsw_ha_mem_reset(int unit);

/*!
 * \brief Dump the contents of an HA memory instance.
 *
 * This function traverses all active HA memory blocks associated with
 * a given unit and writes the contents using the provided write
 * function.
 *
 * \param [in] unit The unit number associated with this HA instance
 *                  or -1 for the generic instance.
 * \param [in] write_f I/O write function that receives a pointer to a
 *                     buffer and a buffer length. The I/O function
 *                     saves the content of the buffer so it can later
 *                     be retrieved.
 * \param [in] ctx Application provided context for \c write_f callback
 *             function.
 *             Typically this will be a file handle or a structure that
 *             contains file information. This parameter provides context to
 *             the \c write_f function and allows multiple threads to call this
 *             function in parallel.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO The I/O function failed.
 */
extern int
soc_ltsw_ha_mem_content_dump(
    int unit,
    int (*write_f)(void *ctx, void *buf, size_t len),
    void *ctx);

/*!
 * \brief Compare HA memory block contents with saved contents.
 *
 * This function reads saved HA memory blocks via a provided read
 * function and compares the contents to the current HA memory blocks
 * with the same component and sub-component.
 *
 * \param [in] unit The unit number associated with this HA instance
 *                  or -1 for the generic instance.
 * \param [in] read_f I/O read function that receives a pointer to a
 *                    buffer, a buffer length and opaque pointer as input.
 *                    The I/O function reads \c len bytes into the provided
 *                    buffer.
 * \param [in] ctx Application provided context for \c read_f callback
 *             function.
 *             Typically this will be a file handle or a structure that
 *             contains file information. This parameter provides context to
 *             the \c read_f function and allows multiple threads to call this
 *             function in parallel.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO The I/O function failed.
 * \retval SHR_E_MEMORY Failed to allocate working buffer.
 * \retval SHR_E_FAIL The content comparison failed.
 */
extern int
soc_ltsw_ha_mem_content_comp(
    int unit,
    int (*read_f)(void *ctx, void *buf, size_t len),
    void *ctx);

/*!
 * \brief Retrive per-component HA memory usage.
 *
 * For a given HA memory instance, this function provides a report on
 * the HA memory size allocated by each component. The report is
 * provided for each sub-component ID where the sub-component ID used
 * as the index in the \c reports array.
 *
 * \param [in] unit The unit number associated with this HA instance
 *                  or -1 for the generic instance.
 * \param [in] comp_id The memory usage will be provided for this component.
 * \param [in] report_size Indicates the \c reports array size. The
 *                         maximum size of the array can be \c
 *                         SHR_HA_SUB_IDS. If it is smaller the HA
 *                         will only report for sub-id that is smaller
 *                         then the report_size.
 * \param [out] reports Array containing the number of bytes allocated per
 *                      sub-component ID.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_NOT_FOUND When the component is not allocating any HA
 * resources.
 */
extern int
soc_ltsw_ha_mem_usage(
    int unit,
    soc_ltsw_ha_mod_id_t comp_id,
    uint32_t report_size,
    soc_ltsw_ha_usage_report_t *reports);

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
soc_ltsw_ha_mem_alloc(
    int unit,
    soc_ltsw_ha_mod_id_t comp_id,
    soc_ltsw_ha_sub_id_t sub_id,
    const char *blk_id,
    uint32_t *length);

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
soc_ltsw_ha_mem_realloc(int unit, void *mem, uint32_t length);

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
extern int
soc_ltsw_ha_mem_free(int unit, void *mem);

typedef struct soc_ltsw_ha_info_s {
    int comp_count;
    const char *comp_name[];
} soc_ltsw_ha_info_t;

/*!
 * \brief Initialize the HA module with needed information.
 *
 * This function is used to import the component information for a given unit.
 *
 * \param [in] unit Unit number associated with this HA memory.
 * \param [in] ha_info Pointer to input the component information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
soc_ltsw_ha_init(int unit, soc_ltsw_ha_info_t *ha_info);

/*!
 * \brief Cleanup the HA module.
 *
 * This function is used to clean up the HA module during detaching process.
 *
 * \param [in] unit Unit number associated with this HA memory.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
soc_ltsw_ha_cleanup(int unit);

/*!
 * \brief Get the HA module information.
 *
 * This function is used to get the HA component information for a given unit.
 *
 * \param [in] unit Unit number associated with this HA memory.
 * \param [in] ha_info Pointer to input the component information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
soc_ltsw_ha_info_get(int unit, soc_ltsw_ha_info_t **ha_info);

#endif /* BCM_INT_COMMON_HA_H */
