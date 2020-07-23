/*! \file shr_ha.h
 *
 * SHR HA API header file
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SHR_HA_H
#define SHR_HA_H

#include <shr/shr_ha_types.h>

/*!
 * \brief Callback for allocating a HA memory pool.
 *
 * This function signature is used by the HA manager to request the
 * application to allocate a new HA memory pool.
 *
 * \param [in] ctx Opaque pointer provided in \ref shr_ha_unit_init()
 * \param [in] size The desired size of the new HA memory pool.
 * \param [out] actual_addition The actual size of the HA memory pool.
 *
 * \return Pointer newly allocated HA memory pool or NULL if error.
 */
typedef void *(*shr_ha_pool_alloc_f)(void *ctx, uint32_t size,
                                     uint32_t *actual_size);

/*!
 * \brief Callback for freeing a HA memory pool.
 *
 * This function signature is is used by the HA manager to release HA
 * memory pools.
 *
 * The function is called for both the initial HA memory pool (if
 * provided) and any pool allocated via the \ref shr_ha_pool_alloc_f
 * callback.
 *
 * \param [in] ctx Opaque pointer provided in \ref shr_ha_unit_init()
 * \param [in] pool Pointer to HA memory pool to be freed.
 * \param [in] size Size of the HA memory pool to be freed.
 *
 * \return none.
 */
typedef void (*shr_ha_pool_free_f)(void *cts, void *pool, uint32_t size);

/*!
 * \brief Initialize unit HA memory.
 *
 * This function being called for any newly discovered unit. The function must
 * be called by the application prior of calling \c bcmlt_device_attach().
 * The application must open the shared file associated with the unit and that
 * create memory map into the shared file. The memory map pointer \c mmap is
 * passed as input for this function. When opening the shared file the
 * application need to determine if the file already exist or not. The input
 * parameter new_file should indicate if the file is new or already existed.
 * The main reason to differentiate between existing file to new file is the
 * size of the file and the shared memory. If the file was already exist, the
 * application must memory map the entire space of the file. That is since
 * the HA manager stored valuable information inside the file. Note that file
 * created at a specific size might be extended during run-time using the
 * application provided function \c alloc_f (see \ref shr_ha_pool_alloc_f()).
 *
 * \param [in] unit The unit number associated with this HA instance.
 * \param [in] mmap Pointer to HA memory. For cold boot this parameter
 *                  should be NULL. For warm boot this should be the
 *                  memory that was used in the previous run. Note
 *                  that this memory can be memory map of shared file
 *                  or any other storage device.
 * \param [in] size The size of the shared memory segment.
 * \param [in] alloc_f Application callback function for resizing HA
 *                     memory. The memory manager calls this function
 *                     when it needs more HA memory. To use heap
 *                     memory set this value to NULL.
 * \param [in] free_f Application callback callback for freeing HA
 *                    memory allocated via the \c resize_f
 *                    function. To use heap memory set this value to
 *                    NULL.
 * \param [in] ctx Application context used during the callback to \c
 *                 resize_f and \c free_f.
 *
 * \retval SHR_E_NONE success, otherwise failure in adding the field to the
 */
extern int shr_ha_unit_init(int unit,
                            void *mmap,
                            int size,
                            shr_ha_pool_alloc_f alloc_f,
                            shr_ha_pool_free_f free_f,
                            void *ctx);

/*!
 * \brief Allocate memory block with string ID.
 *
 * This function being used to allocate a block of HA memory and associate it
 * with a particular unit. The component and sub-component IDs (\c mod_id,
 * \c sub_id respectively) are in essence non volatile pointers. When the
 * application starts warm, the component and sub-component IDs can be used
 * to retrieve the same memory content that was held prior to the boot.
 * To control the numbers, each software component will have pre-allocated
 * component ID (in some header file). The sub-component IDs will b managed
 * internally by the component.
 * In case of software upgrade, the HA will used the auto generated structure
 * signature (\c struct_sig), that describes the data structure of the HA
 * memory, to perform automatic upgrade to the data structure.
 *
 * \param [in] unit The unit associated with the HA memory.
 * \param [in] comp_id Pre-allocated component ID.
 * \param [in] sub_id Internal ID managed by the component.
 * \param [in] blk_id Given string ID that is associated with the block. This
 * ID makes it easier to determine the owner of an HA block. If this parameter
 * is NULL the original string (in case of warm boot) remains unchanged.
 * \param [out] length This parameter used as input and output. For input it
 * specifies the desired memory size. For output it is the actual memory block
 * that was allocated. The output value
 * has meaning during warm boot where the requested size might be different
 * from the actual size of the block that was allocated in previous run.
 *
 * \return If success the function returns pointer to the desired memory.
 * Otherwise it returns NULL.
 */
extern void *shr_ha_mem_alloc(int unit,
                              shr_ha_mod_id comp_id,
                              shr_ha_sub_id sub_id,
                              const char *blk_id,
                              uint32_t *length);

/*!
 * \brief Reallocate existing memory block.
 *
 * This function extends already allocated memory block to a desired length.
 *
 * \param [in] unit The unit associated with the HA memory.
 * \param [in] mem The current block to extend
 * \param [in] length The desired length of memory to allocate.
 *
 * \return If success the function returns pointer to the extended memory
 * block. Otherwise it returns NULL.
 */
extern void *shr_ha_mem_realloc(int unit, void *mem, uint32_t length);

/*!
 * \brief Free previously allocated memory block.
 *
 * This function should rarely be used and it is here for completion of the API.
 * The function returns the memory block into the memory pool. Once the block
 * had been returned into the free pool it can be reused by the HA manager for
 * future requests of the same or smaller size. This can potentially cause
 * memory fragmentation. It is therefore recommended to avoid continuous
 * allocation and free of HA memory during run-time.
 * Since the application is allocating the HA memory, there is no good reason to
 * free the HA memory upon unit shutdown. If necessary, the application will
 * free this memory. Otherwise, the HA memory may be used for following warm
 * start.
 *
 * \param [in] unit The unit associated with the HA memory.
 * \param [in] mem The block to free.
 *
 * \retval SHR_E_NONE success, otherwise failure freeing the memory.
 */
extern int shr_ha_mem_free(int unit, void *mem);

/*!
 * \brief Close previously initiated unit HA memory.
 *
 * This function is the inverse of \ref shr_ha_unit_init() function. It is
 * expected to be called by the application after the call to
 * \c bcmlt_device_detach().
 * Note that the application is expected to close the shared file associated
 * with this unit. The file can be kept (for future warm boot) or can be
 * removed from its storage (using the function \c shm_unlink()).
 *
 * \param [in] unit The unit associated with the HA memory.
 *
 * \retval SHR_E_NONE success, otherwise error code.
 */
extern int shr_ha_unit_close(int unit);

/*!
 * \brief Reset the content of the HA memory
 *
 * This function goes through all the memory blocks that are allocated within
 * the specified unit and reset the block's content to zero.
 *
 * \param [in] unit The unit associated with the operation.
 *
 * \retval SHR_E_NONE success, otherwise error code.
 */
extern int shr_ha_unit_reset(int unit);

/*!
 * \brief Per component HA memory usage.
 *
 * This function provides a report on the HA memory size allocated by each
 * component. The report is provided for each sub-component ID where the
 * sub-component ID used as the index in the \c reports array.
 *
 * \param [in] unit The unit associated with the HA memory.
 * \param [in] comp_id The memory usage will be given on this component.
 * \param [in] report_size Indicates the \c reports array size. The maximal size
 * of the array can be \c SHR_HA_SUB_IDS. If it is smaller the HA will only
 * report for sub-id that is smaller then the report_size.
 * \param [out] reports Array containing the number of bytes allocated per
 * sub-component ID.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_NOT_FOUND When the component is not allocating any HA
 * resources.
 */
extern int shr_ha_comp_mem_usage(int unit,
                                 shr_ha_mod_id comp_id,
                                 uint32_t report_size,
                                 shr_ha_usage_report_t *reports);

/*!
 * \brief Initialize generic HA memory.
 *
 * This function is similar to the function \ref shr_ha_unit_init with the
 * exception that it is unit independent. The purpose of this API is to provide
 * HA services for generic components that are not associated with any unit.
 *
 * \param [in] mmap Pointer to HA memory. For cold boot this parameter
 *                  should be NULL. For warm boot this should be the
 *                  memory that was used in the previous run. Note
 *                  that this memory can be memory map of shared file
 *                  or any other storage device.
 * \param [in] size The size of the shared memory segment.
 * \param [in] alloc_f Application callback function for resizing HA
 *                     memory. The memory manager calls this function
 *                     when it needs more HA memory. To use heap
 *                     memory set this value to NULL.
 * \param [in] free_f Application callback callback for freeing HA
 *                    memory allocated via the \c alloc_f
 *                    function. To use heap memory, set this value to
 *                    NULL.
 * \param [in] ctx Application context used during the callback to \c
 *                 alloc_f and \c free_f.
 *
 * \retval SHR_E_NONE success, otherwise failure in initializing the generic
 * memory pool.
 */
extern int shr_ha_gen_init(void *mmap,
                           int size,
                           shr_ha_pool_alloc_f alloc_f,
                           shr_ha_pool_free_f free_f,
                           void *ctx);

/*!
 * \brief Allocate memory block with ID from the generic pool.
 *
 * This function being used to allocate a block of HA memory and associate it
 * with a generic pool. This function is similar to the function
 * \ref shr_ha_mem_alloc with the exception that this function uses the generic
 * HA memory pool instead of specific unit memory pool.
 *
 * \param [in] comp_id Pre-allocated component ID.
 * \param [in] sub_id Internal ID managed by the component.
 * \param [in] blk_id Given string ID that is associated with the block. This
 * ID makes it easier to determine the owner of an HA block. If this parameter
 * is NULL the original string (in case of warm boot) remains unchanged.
 * \param [out] length This parameter used as input and output. For input it
 * specifies the desired memory size. For output it is the actual memory block
 * that was allocated. The output value
 * has meaning during warm boot where the requested size might be different
 * from the actual size of the block that was allocated in previous run.
 *
 * \return If success the function returns pointer to the desired memory.
 * Otherwise it returns NULL.
 */
extern void *shr_ha_gen_mem_alloc(shr_ha_mod_id comp_id,
                                  shr_ha_sub_id sub_id,
                                  const char *blk_id,
                                  uint32_t *length);

/*!
 * \brief Reallocate existing memory block.
 *
 * This function extends already allocated memory block to a desired length.
 * The function is similar to the function \ref shr_ha_mem_realloc with the
 * exception that it allocates the HA memory from the generic pool.
 *
 * \param [in] mem The current block to extend
 * \param [in] length The desired length of memory to allocate.
 *
 * \return If success the function returns pointer to the extended memory
 * block. Otherwise it returns NULL.
 */
extern void *shr_ha_gen_mem_realloc(void *mem, uint32_t length);

/*!
 * \brief Free previously allocated generic memory block.
 *
 * This function should rarely be used and it is here for completion of the API.
 * The function is very similar to the function \ref shr_ha_mem_free with the
 * exception that the memory returned back to the generic memory pool instead
 * of the unit memory pool.
 *
 * \param [in] mem The block to free.
 *
 * \retval SHR_E_NONE success, otherwise failure freeing the memory.
 */
extern int shr_ha_gen_mem_free(void *mem);

/*!
 * \brief Close previously initiated generic HA memory.
 *
 * This function is the inverse of \ref shr_ha_gen_init() function. It is
 * expected to be called by the application upon shutting down the system.
 *
 * This function operates very closely to the function \ref shr_ha_unit_close
 * with the exception of the fact that this function closes the generic memory
 * pool and not a specific unit pool.
 *
 * \retval SHR_E_NONE success, otherwise error code
 */
extern int shr_ha_gen_close(void);

/*!
 * \brief Reset the content of the HA memory
 *
 * This function goes through all the memory blocks that are allocated within
 * the specified unit and reset the block's content to zero.
 *
 * \retval SHR_E_NONE success, otherwise error code.
 */
extern int shr_ha_gen_reset(void);

/*!
 * \brief Per component generic HA memory usage.
 *
 * This function provides a report on the generic HA memory size allocated by
 * each component. The report is provided for each sub-component ID where the
 * sub-component ID used as the index in the \c reports array.
 *
 * \param [in] comp_id The memory usage will be given on this component.
 * \param [in] report_size Indicates the \c reports array size. The maximal size
 * of the array can be \c SHR_HA_SUB_IDS. If it is smaller the HA will only
 * report for sub-id that is smaller then the report_size.
 * \param [out] reports Array containing the number of bytes allocated per
 * sub-component ID.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_NOT_FOUND When the component is not allocating any HA
 * resources.
 */
extern int shr_ha_gen_comp_mem_usage(shr_ha_mod_id comp_id,
                                     uint32_t report_size,
                                     shr_ha_usage_report_t *reports);

/*!
 * \brief Dump the content of an HA file into the write I/O function.
 *
 * This function goes through all the active HA blocks associated with a given
 * unit and writes their content using the provided I/O write function.
 *
 *
 * \param [in] unit The unit associated with the HA file.
 * \param [in] write_f I/O write function that receives a pointer to a
 * buffer and a buffer length. The I/O function saves the content of the buffer
 * so it can later be retrieved.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_IO The I/O function failed.
 */
extern int shr_ha_content_dump(int unit,
                               int (*write_f)(void *buf, size_t len));

/*!
 * \brief Compare the content of the HA blocks with a previously saved content.
 *
 * This function reads the content of every saved HA block and compare its
 * content to the current HA block under the same component and sub-component..
 *
 * \param [in] unit Is the unit associated with the HA file.
 * \param [in] read_f is an I/O read function that receives a pointer to a
 * buffer and a buffer length as input. The I/O function reads the len bytes
 * into the provided buffer.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_IO The I/O function failed.
 * \retval SHR_E_MEMORY Failed to allocate working buffer.
 * \retval SHR_E_FAIL The content comparison failed.
 */
extern int shr_ha_content_comp(int unit,
                               int (*read_f)(void *buf, size_t len));

/*!
 * \brief Dump the content of the generic HA file into the write I/O function.
 *
 * This function go through all the active HA blocks of the generic unit
 * and write their content using the provided I/O write function.
 *
 * \param [in] write_f is an I/O write function that receives a pointer to a
 * buffer and a buffer length. The I/O function saves the content of the buffer
 * so it can later be retrieved.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_IO The I/O function failed.
 */
extern int shr_ha_gen_content_dump(int (*write_f)(void *buf, size_t len));

/*!
 * \brief Compare the content of the generic HA blocks with a previously saved
 * content.
 *
 * This function reads the content of every saved HA block and compare its
 * content to the current generic HA block under the same component and
 * sub-component..
 *
 * \param [in] read_f is an I/O read function that receives a pointer to a
 * buffer and a buffer length as input. The I/O function reads the len bytes
 * into the provided buffer.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_IO The I/O function failed.
 * \retval SHR_E_MEMORY Failed to allocate working buffer.
 * \retval SHR_E_FAIL The content comparison failed.
 */
extern int shr_ha_gen_content_comp(int (*read_f)(void *buf, size_t len));

/*!
 * \brief HA block usage type.
 *
 * This indicates what is the HA block is being used for.
 */
typedef enum {
    SHR_HA_BLK_USAGE_LT = 0,/*!< Block is used to store LT values */
    SHR_HA_BLK_USAGE_TYPE,  /*!< Block is used to store data structure */
    /*! Block is used for other purposes than the above */
    SHR_HA_BLK_USAGE_OTHER
} shr_ha_blk_usage_t;

/*!
 * \brief Convert string that is associated with data type or LT to HA usage
 * string.
 *
 * \param [in] str Input contains the LT name or data type.
 * \param [in] usage What is the block being used for. Based on the input the
 * string prefix will be 'l' for LT, 't' for Data type and 'o' for other.
 * \param [in] len Length of the output string buffer \c id_str
 * \param [out] id_str Contain the converted str into HA block ID.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM invalid input parameter.
 */
extern int shr_ha_str_to_id(const char *str,
                            shr_ha_blk_usage_t usage,
                            size_t len,
                            char *id_str);
/*!
 * \brief Macros to define and extract HA pointers
 *
 * HA pointers are 64 (long) or 32 bit numbers that contains three parts.
 * 1. An eight bit block identifier - the sub component. This is the block from
 *    bit 54 to 63 (long HA pointer) or 22 to 31
 * 2. A two bit for component proprietary usage. These two bits are opaque to
 *    the HA component. The two bits will be located in bits 52, 53 (long HA
 *    pointer) or 20-21 for regular pointer.
 * 3. The offset from the beginning of the block where the pointer points. These
 *    32 bits located from bit 0 to bit 31 (long HA pointer) or 0 to 19 (regular
 *    HA pointer).
 *
 * In the macros below long pointers have the prefix L to the PTR section
 * so \ref SHR_HA_PTR_OFFSET_MASK definition for regular pointer becomes
 * \ref SHR_HA_LPTR_OFFSET_MASK for long pointer definition.
 * Use regular pointer if the HA block size if smaller than 1MB.
 *
 * The above scheme implies that pointers can not be exchanged between
 * components as each component manages its own sub component id value
 * and the component ID is not part of the pointer information.
 */

/*!
 * \brief HA pointer type definition.
 */
typedef uint32_t shr_ha_ptr_t;

/*!
 * \brief HA large pointer type definition.
 */
typedef uint64_t shr_ha_lptr_t;

/*!
 * \brief HA pointer offset mask definition.
 */
#define SHR_HA_PTR_OFFSET_MASK  0xFFFFF

/*!
 * \brief HA large pointer offset mask definition.
 */
#define SHR_HA_LPTR_OFFSET_MASK  0xFFFFFFFF

/*!
 * \brief HA pointer sub module shift definition.
 * Indicates the location of the sub module in the HA pointer.
 */
#define SHR_HA_PTR_SUB_SHIFT    22

/*!
 * \brief HA large pointer sub module shift definition.
 * Indicates the location of the sub module in the HA large pointer.
 */
#define SHR_HA_LPTR_SUB_SHIFT    54

/*!
 * \brief HA pointer component private shift definition.
 * Indicates the location of the component private bits in the HA pointer.
 */
#define SHR_HA_PTR_PRIV_SHIFT   (SHR_HA_PTR_SUB_SHIFT-2)

/*!
 * \brief HA large pointer component private shift definition.
 * Indicates the location of the component private bits in the HA large pointer.
 */
#define SHR_HA_LPTR_PRIV_SHIFT   (SHR_HA_LPTR_SUB_SHIFT-2)

/*!
 * \brief HA pointer sub module size definition.
 */
#define SHR_HA_PTR_SUB_MASK     0x3FF

/*!
 * \brief HA pointer component private bits size definition.
 */
#define SHR_HA_PTR_PRIV_MASK     0x3

/*!
 * \brief HA pointer component private bits mask definition.
 * Specifies the actual bits that are occupied by the component private bits.
 */
#define SHR_HA_PTR_PRIV_BITS    \
    (SHR_HA_PTR_PRIV_MASK << SHR_HA_PTR_PRIV_SHIFT)

/*!
 * \brief HA large pointer component private bits mask definition.
 * Specifies the actual bits that are occupied by the component private bits.
 */
#define SHR_HA_LPTR_PRIV_BITS    \
    ((uint64_t)(SHR_HA_PTR_PRIV_MASK) << SHR_HA_LPTR_PRIV_SHIFT)

/*!
 * \brief HA pointer constructor.
 *
 * \param [in] _sub The sub module of the HA block that this pointer points to.
 * \param [in] _offset The offset (in bytes) from the beginning of the block
 * where the pointer will point to.
 *
 * \return HA pointer.
 */
#define SHR_HA_PTR_CONSTRUCT(_sub, _offset) \
    (((shr_ha_ptr_t)(_sub) & SHR_HA_PTR_SUB_MASK) << SHR_HA_PTR_SUB_SHIFT | \
     ((_offset) & SHR_HA_PTR_OFFSET_MASK))

/*!
 * \brief HA pointer constructor with component private bits.
 *
 * \param [in] _sub The sub module of the HA block that this pointer points to.
 * \param [in] _offset The offset (in bytes) from the beginning of the block
 * where the pointer will point to.
 * \param [in] _priv is private value of up to two bits. This value is opaque
 * to the HA module but can be associated with the pointer and retrieved by
 * the component.
 *
 * \return HA pointer.
 */
#define SHR_HA_PTR_EXT_CONSTRUCT(_sub, _offset, _priv) \
    (((uint64_t)(_sub) & SHR_HA_PTR_SUB_MASK) << SHR_HA_PTR_SUB_SHIFT |   \
    ((uint64_t)(_priv) & SHR_HA_PTR_PRIV_MASK) << SHR_HA_PTR_PRIV_SHIFT | \
    ((_offset) & SHR_HA_PTR_OFFSET_MASK))

/*!
 * \brief Overwrite the HA pointer offset with a new one.
 *
 * \param [in] _ha_ptr The HA pointer.
 * \param [in] _offset The new offset to set.
 *
 * \return The new HA pointer value.
 */
#define SHR_HA_PTR_OFFSET_SET(_ha_ptr, _offset) \
    ((_ha_ptr) & (~(uint64_t)SHR_HA_PTR_OFFSET_MASK)) | \
    ((_offset) & SHR_HA_PTR_OFFSET_MASK)

/*!
 * \brief Extract HA block offset from HA pointer.
 *
 * \param [in] _ha_ptr The HA pointer.
 *
 * \return The offset (in bytes) of the pointer from the beginning of the HA
 * block.
 */
#define SHR_HA_PTR_OFFSET(_ha_ptr) \
    ((_ha_ptr) & SHR_HA_PTR_OFFSET_MASK)

/*!
 * \brief Extract HA block sub module from HA pointer.
 *
 * \param [in] _ha_ptr The HA pointer.
 *
 * \return The sub module value.
 */
#define SHR_HA_PTR_SUB(_ha_ptr) \
    (((_ha_ptr) >> SHR_HA_PTR_SUB_SHIFT) & SHR_HA_PTR_SUB_MASK)

/*!
 * \brief Extract HA block private value from HA pointer.
 *
 * \param [in] _ha_ptr The HA pointer.
 *
 * \return The component private value associated with the pointer.
 */
#define SHR_HA_PTR_PRIV(_ha_ptr) \
    (((_ha_ptr) >> SHR_HA_PTR_PRIV_SHIFT) & SHR_HA_PTR_PRIV_MASK)

/*!
 * \brief Set component private value into HA pointer.
 *
 * \param [in] _ha_ptr The HA pointer.
 * \param [in] _priv The component private value.
 *
 * \return None.
 */
#define SHR_HA_PTR_PRIV_SET(_ha_ptr, _priv) \
    ((_ha_ptr) & ~SHR_HA_PTR_PRIV_BITS) |   \
    ((uint64_t)(_priv) & SHR_HA_PTR_PRIV_MASK) << SHR_HA_PTR_PRIV_SHIFT


/*!
 * \brief HA large pointer type definition.
 */
/*!
* \brief HA large pointer constructor.
*
* \param [in] _sub The sub module of the HA block that this pointer points to.
* \param [in] _offset The offset (in bytes) from the beginning of the block
* where the pointer will point to.
*
* \return HA large pointer.
*/
#define SHR_HA_LPTR_CONSTRUCT(_sub, _offset) \
    (((shr_ha_lptr_t)(_sub) & SHR_HA_PTR_SUB_MASK) << SHR_HA_LPTR_SUB_SHIFT | \
     ((_offset) & SHR_HA_LPTR_OFFSET_MASK))

/*!
 * \brief HA large pointer constructor with component private bits.
 *
 * \param [in] _sub The sub module of the HA block that this pointer points to.
 * \param [in] _offset The offset (in bytes) from the beginning of the block
 * where the pointer will point to.
 * \param [in] _priv is private value of up to two bits. This value is opaque
 * to the HA module but can be associated with the pointer and retrieved by
 * the component.
 *
 * \return HA large pointer.
 */
#define SHR_HA_LPTR_EXT_CONSTRUCT(_sub, _offset, _priv) \
    (((uint64_t)(_sub) & SHR_HA_PTR_SUB_MASK) << SHR_HA_LPTR_SUB_SHIFT |   \
    ((uint64_t)(_priv) & SHR_HA_PTR_PRIV_MASK) << SHR_HA_LPTR_PRIV_SHIFT | \
    ((_offset) & SHR_HA_LPTR_OFFSET_MASK))

/*!
 * \brief Overwrite the HA large pointer offset with a new one.
 *
 * \param [in] _ha_ptr The HA pointer.
 * \param [in] _offset The new offset to set.
 *
 * \return The new HA pointer value.
 */
#define SHR_HA_LPTR_OFFSET_SET(_ha_ptr, _offset) \
    ((_ha_ptr) & (~(uint64_t)SHR_HA_LPTR_OFFSET_MASK)) | \
    ((_offset) & SHR_HA_LPTR_OFFSET_MASK)

/*!
 * \brief Extract HA block offset from HA large pointer.
 *
 * \param [in] _ha_ptr The HA pointer.
 *
 * \return The offset (in bytes) of the pointer from the beginning of the HA
 * block.
 */
#define SHR_HA_LPTR_OFFSET(_ha_ptr) \
    (_ha_ptr) & (uint64_t)SHR_HA_LPTR_OFFSET_MASK

/*!
 * \brief Extract HA block sub module from HA large pointer.
 *
 * \param [in] _ha_ptr The HA large pointer.
 *
 * \return The sub module value.
 */
#define SHR_HA_LPTR_SUB(_ha_ptr) \
    (((_ha_ptr) >> SHR_HA_LPTR_SUB_SHIFT) & SHR_HA_PTR_SUB_MASK)

/*!
 * \brief Extract HA block private value from HA large pointer.
 *
 * \param [in] _ha_ptr The HA large pointer.
 *
 * \return The component private value associated with the pointer.
 */
#define SHR_HA_LPTR_PRIV(_ha_ptr) \
    (((_ha_ptr) >> SHR_HA_LPTR_PRIV_SHIFT) & SHR_HA_PTR_PRIV_MASK)

/*!
 * \brief Set component private value into HA large pointer.
 *
 * \param [in] _ha_ptr The HA large pointer.
 * \param [in] _priv The component private value.
 *
 * \return None.
 */
#define SHR_HA_LPTR_PRIV_SET(_ha_ptr, _priv) \
    ((_ha_ptr) & ~SHR_HA_LPTR_PRIV_BITS) |   \
    ((uint64_t)(_priv) & SHR_HA_PTR_PRIV_MASK) << SHR_HA_LPTR_PRIV_SHIFT


/*!
 * \brief Set HA pointer to HA 'NULL' value.
 *
 * \param [in] _ha_ptr The HA pointer.
 *
 * \return None.
 */
#define SHR_HA_PTR_NULL_SET(_ha_ptr) \
    (_ha_ptr) = 0

/*!
 * \brief Check if HA pointer is HA 'NULL'.
 *
 * \param [in] _ha_ptr The HA pointer.
 *
 * \return True if the pointer is HA 'NULL' and false otherwise.
 */
#define SHR_HA_PTR_IS_NULL(_ha_ptr)  \
    ((_ha_ptr) == 0)

/*!
 * \brief Create the HA memory manager instance.
 *
 * This function allocates and creates the instance of the HA memory manager.
 *
 * \param [in] mmap The HA memory block.
 * \param [in] size The size of the HA memory block.
 * \param [in] alloc_f Application callback function for resizing HA
 *                     memory. The memory manager calls this function
 *                     when it needs more HA memory.
 * \param [in] free_f Application callback callback for freeing HA
 *                    memory allocated via the \c alloc_f function.
 * \param [in] context Application context used during the callback to
 *                     \c alloc_f and \c free_f.
 * \param [out] private Handle to the HA memory instance.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_MEMORY Failed to allocate memory for this instance.
 */
extern int
shr_ha_mm_create(void *mmap,
                 int size,
                 shr_ha_pool_alloc_f alloc_f,
                 shr_ha_pool_free_f free_f,
                 void *context,
                 void **private);

/*!
 * \brief ha_mem_mgr class destructor.
 *
 * Free all resources associated with the HA instance.
 *
 * \param [in] private The context of the HA memory.
 *
 * \retval none
 */
extern void
shr_ha_mm_delete(void *private);

/*!
 * \brief Reset the content of the HA memory for a given instance.
 *
 * This function goes through the entire allocated memory blocks and reset its
 * content to 0.
 *
 * \param [in] private Handle of the HA control block.
 *
 * \retval SHR_E_NONE on success.
 */
extern int
shr_ha_mm_reset(void *private);

/*!
 * \brief Initializes the shared memory.
 *
 * This function initialized the shared memory. The memory is being initialized
 * in the following way:
 * 1. If it is a new file (cold start) the entire memory is marked as a free
 * block with the length of the entire memory.
 * 2. If it is an existing file (warm boot) the function checks the validity of
 * all the existing blocks. It also adjusts their sections and offset since
 * in warm boot there is only single section.
 *
 * \param [in] private The context of the HA memory.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT The memory was already initialized.
 * \retval SHR_E_INTERNAL The shared memory structure is not coherent. This
 * may occur if a memory overwrite took place in the previous run. Typically for
 * this scenario, memory overwrite may occur due to a component writing outside
 * its allocated block size and spilling over to the next block. Since each HA
 * memory block starts with 32 bit signature this can be detected. When detected
 * the HA module will write an error message indicating the last good block
 * owner - typically the suspected module.
 */
extern int
shr_ha_mm_init(void *private);

/*!
 * \brief Allocate HA memory block for the application.
 *
 * This function searches for a block with matches mod and sub ID.
 * If such a block was not found the function allocates a new memory block
 * of the proper length and returns a pointer to the memory.
 * The version is used only in case that a matched block was found. In
 * this case if the versions are different the function will call
 * a method that will upgrade/downgrade the structure to the required
 * version. The structure signature identifies the schema that was used
 * to generate the data structure contained in this block.
 *
 * \param [in] private Is the handle of the HA control block.
 * \param [in] mod_id is the module ID.
 * \param [in] sub_id is the sub module ID.
 * \param [in] blk_str_id String provided by the component that can
 *                        uniquely identify the block.

 * \param [in,out] length A pointer containing the desired length. The
 *                        length also contains the actual allocated
 *                        memory length.
 *
 * \retval pointer to the allocated block
 * \retval NULL if the block was not allocated
 */
extern void *
shr_ha_mm_alloc(void *private,
                shr_ha_mod_id mod_id,
                shr_ha_sub_id sub_id,
                const char *blk_str_id,
                uint32_t *length);

/*!
 * \brief Free memory block back into the pool
 *
 * Free memory tries to find adjacent free memory blocks to concatinate them
 * into a larger memory block
 *
 * \param [in] private The handle of the HA control block.
 * \param [in] mem The old memory handle
 *
 * \retval SHR_E_NONE on success
 * \retval SHR_E_PARAM when one of the input parameters was invalid.
 */
extern int
shr_ha_mm_free(void *private, void *mem);

/*!
 * \brief Reallocate existing memory.
 *
 * If the new desired size is smaller/equal to the current block length
 * do nothing. Otherwise, allocate new memory block using the length
 * parameter, copy the old memory onto the new block and free the old block.
 *
 * \param [in] private The handle of the HA control block.
 * \param [in] mem The old memoy handle.
 * \param [in,out] length A pointer contains the desired length. The
 *        length also contains the actual allocated memory length.
 *
 * \retval pointer to the block if was reallocated
 * \retval NULL otherwise
 */
extern void *
shr_ha_mm_realloc(void *private,
                  void *mem,
                  uint32_t length);

/*!
 * \brief Component HA memory usage report.
 *
 * This function will go through all the allocated HA memory blocks and search
 * for the specific module ID ownership.
 *
 * \param [in] private Handle of the HA control block.
 * \param [in] mod_id Module (or component) ID.
 * \param [in] report_size Indicate the length of the reports array.
 * \param [out] reports The array indexed via sub-component ID where the HA
 * resources allocated by the component for the specific sub component will
 * be written.
 *
 * \return SHR_E_NONE on success.
 * \return SHR_E_NOT_FOUND If the component was not consuming any HA memory.
 */
extern int
shr_ha_mm_usage(void *private,
                shr_ha_mod_id mod_id,
                uint32_t report_size,
                shr_ha_usage_report_t *reports);

/*!
 * \brief Dump the content of the entire HA memory into a write function.
 *
 * This function go through all the HA blocks that have valid content and writes
 * their content using the write IO function provided by the caller.
 *
 * \param [in] private The context of the HA memory.
 * \param [in] write_f A write IO function provided the caller.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO when the caller provided IO function failed.
 */
extern int
shr_ha_mm_dump(void *private,
               int (*write_f)(void *buf, size_t len));

/*!
 * \brief Compare the content read by the provided IO function to the HA
 * content.
 *
 * This function uses the caller provided IO read function to read the content
 * of a previously saved HA memory block and matches it to a current HA memory
 * block with matching block ID.
 * Note that this function only checks that the previous state had been
 * preserved. However, this function doesn't check if the state had been
 * expended (i.e. portion that were not exist before are now exist).
 *
 * \param [in] private The context of the HA memory.
 * \param [in] read_f A read IO function provided the caller. This function
 * is being used to read the content of an HA memory block that was previously
 * saved by the function \ref shr_ha_mm_dump.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO The caller provided read function failed.
 * \retval SHR_E_MEMORY Memory allocation failed.
 * \retval SHR_E_FAIL The block doesn't exist anymore or the block content is
 * different from the content that was previously saved.
 */
extern int
shr_ha_mm_comp(void *private,
               int (*read_f)(void *buf, size_t len));

/*!
 * \brief Restore the content of HA using previously saved content.
 *
 * This function uses the caller provided IO read function to read the content
 * of a previously saved HA memory block and place it into the current HA file.
 *
 * \param [in] private The context of the HA memory.
 * \param [in] read_f A read IO function provided the caller. This function
 * is being used to read the content of an HA memory block that was previously
 * saved by the function \ref shr_ha_mm_dump.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO The caller provided read function failed.
 * \retval SHR_E_MEMORY Memory allocation failed.
 */
extern int
shr_ha_mm_restore(void *private,
                  int (*read_f)(void *buf, size_t len));

#endif /* SHR_HA_H */
