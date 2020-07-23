/*! \file shr_lmem_mgr.h
 *
 * Local memory manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SHR_LMEM_MGR_H
#define SHR_LMEM_MGR_H

#include <sal/sal_libc.h>

/*!
 * \brief Forward declaration of local memory handle
 */
typedef struct shr_lmm_hdl_s *shr_lmm_hdl_t;

/*!
 * \brief Initializes local memory instance.
 *
 * This macro initializes an instance of the local memory manager. It is
 * assume that each managed element data type contains a pointer field
 * named 'next'. The pointer field is of type (T *).
 *
 * \param [in] T is the type of the memory that should be managed
 * \param [out] hdl is the application handler that represent
 *              this instance of the local memory manager
 * \param [in] chunk_size is the desired chunk size allocation. For
 *              applications that manages many elements this number
 *              should be larger. In reality, this number represent the
 *              ratio between allocating individual elements or chunk of
 *              elements.
 * \param [in]  multi_thread is a boolean indicating true if multi threads
 *              can allocate/free elements from this memory and false otherwise
 * \param [out] rv is the return value. 0 for success, otherwise failure
 *
 * \return void None.
 */
#define LMEM_MGR_INIT(T, hdl, chunk_size, multi_thread, rv)  \
{                                       \
    rv = shr_lmm_init(chunk_size,       \
            sizeof(T),                  \
            (uintptr_t) ((char *)&(((T *)(0))->next)), \
            multi_thread,               \
            #T,                         \
            &hdl);                      \
}

/*!
 * \brief Initializes local memory instance with memory restriction.
 *
 * This macro initializes an instance of the local memory manager similar to
 * \ref LMEM_MGR_INIT() with an additional option that enables the caller to
 * place a limit on the number of elements that can be allocated.
 *
 * \param [in] T is the type of the memory that should be managed
 * \param [out] hdl is the application handler that represent
 *              this instance of the local memory manager
 * \param [in] chunk_size is the desired chunk size allocation. For
 *              applications that manages many elements this number
 *              should be larger. In reality, this number represent the
 *              ratio between allocating individual elements or chunk of
 *              elements.
 * \param [in]  multi_thread is a boolean indicating true if multi threads
 *              can allocate/free elements from this memory and false otherwise
 * \param [in] max Is the maximal number of elements that can be allocated. This
 *              parameter determine that the maximal memory size that will be
 *              allocated for this element is the size of \c T * max.
 * \param [out] rv is the return value. 0 for success, otherwise failure
 *
 * \return None.
 */
#define LMEM_MGR_REST_INIT(T, hdl, chunk_size, multi_thread, max, rv)  \
{                                         \
    T x##T;                               \
    rv = shr_lmm_restrict_init(chunk_size,\
            sizeof(T),                    \
            (uint32_t)((uint8_t *)&(x##T.next)-(uint8_t *)&x##T),\
            multi_thread,                 \
            max,                          \
            #T,                           \
            &hdl);                        \
}

/*!
 * \brief Initializes local memory instance with memory restriction.
 *
 * This macro initializes an instance of the local memory manager similar to
 * \ref LMEM_MGR_REST_INIT(). The only difference here is that the data
 * structure is not enforced to contain a next field. This enables a component
 * to use data structure of smaller size.
 *
 * \param [in] T is the type of the memory that should be managed.
 * \param [out] hdl is the application handler that represent
 *              this instance of the local memory manager.
 * \param [in] chunk_size is the desired chunk size allocation. For
 *              applications that manages many elements this number
 *              should be larger. In reality, this number represent the
 *              ratio between allocating individual elements or chunk of
 *              elements.
 * \param [in]  multi_thread is a boolean indicating true if multi threads
 *              can allocate/free elements from this memory and false otherwise
 * \param [in] max Is the maximal number of elements that can be allocated. This
 *              parameter determine that the maximal memory size that will be
 *              allocated for this element is the size of \c T * max.
 * \param [out] rv is the return value. 0 for success, otherwise failure
 *
 * \return None.
 */
#define LMEM_MGR_REST_NO_ID_INIT(T, hdl, chunk_size, multi_thread, max, rv) \
        {                                         \
            rv = shr_lmm_restrict_init(chunk_size,\
                    sizeof(T),                    \
                    0,                            \
                    multi_thread,                 \
                    max,                          \
                    #T,                           \
                    &hdl);                        \
        }

/*!
 * \brief Initializer for the local memory manager.
 *
 * This function must be called before any other LMM function is called.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_MEMORY Failed to allocate resources.
 */
extern int
shr_lmm_global_init(void);

/*!
 * \brief Shutdown the local memory manager.
 *
 * This function should be the last function of the LMM that is called.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_BUSY Not all the resources had been freed.
 */
extern int
shr_lmm_global_shutdown(void);


/*!
 * \brief Initializes local memory instance.
 *
 * The application can call this function directly instead of using the
 * initialization macro. Use this function if the element data type doesn't
 * contains a pointer field named 'next'. Note, however, that the element
 * type must have a different field that is a pointer to the type of the
 * element. This is mandatory to enable link list of this elements.
 *
 * \param [in] chunk_size Is the desired chunk size allocation. For
 *              applications that manages many elements this number
 *              should be larger. In reality, this number represnt the
 *              ratio between allocating individual elements or chunk of
 *              elements.
 * \param [in] element_size Is the size of the basic element (in bytes).
 *              The application may use sizeof() to obtain this value.
 * \param [in] pointer_offset Is the offset (in bytes) of the link list
 *              pointer from the begining of the structure.
 * \param [in]  multi_thread Is a boolean indicating true if multi threads
 *              can allocate/free elements from this memory and false otherwise.
 * \param [in] label String to be associated with the memory handle.
 * \param [out] hdl Is the application handler that represent this instance
 *              of the local memory manager. The caller must use this handle
 *              with every call to allocate or free a memory element.
 *
 * \return 0 for success, -1 for failure.
 */
extern int
shr_lmm_init(size_t chunk_size,
             size_t element_size,
             uint32_t pointer_offset,
             bool multi_thread,
             const char *label,
             shr_lmm_hdl_t *hdl);

/*!
 * \brief Initializes local memory instance with restricted number of elements.
 *
 * The application can call this function directly instead of using the
 * initialization macro. Use this function if the element data type doesn't
 * contains a pointer field named 'next'. Note, however, that the element
 * type must have a different field that is a pointer to the type of the
 * element. This is mandatory to enable link list of this elements.
 * This function different from the function \ref shr_lmm_init() as it allows
 * the caller to specify the maximal number of elements that can be allocated.
 * This enables the caller to control the amount of memory it is willing to
 * dedicate for this particular element.
 *
 * \param [in]  chunk_size Is the desired chunk size allocation. For
 *              applications that manages many elements this number
 *              should be larger. In reality, this number represnt the
 *              ratio between allocating individual elements or chunk of
 *              elements.
 * \param [in]  element_size Is the size of the basic element (in bytes)
 *              The application may use sizeof() to obtain this value.
 * \param [in]  pointer_offset Is the offset (in bytes) of the link list
 *              pointer from the begining of the structure.
 * \param [in]  multi_thread Is a boolean indicating true if multi threads
 *              can allocate/free elements from this memory and false otherwise.
 * \param [in]  max_elements Indicates the maxmimal number of memory elements
 *              that can be allocated for this specific instance.
 * \param [in] label String to be associated with the memory handle.
 * \param [out] hdl Is the application handler that represent this instance
 *              of the local memory manager. The caller must use this handle
 *              with every call to allocate or free a memory element.
 *
 * \return 0 for success, -1 for failure.
 */
extern int
shr_lmm_restrict_init(size_t chunk_size,
                      size_t element_size,
                      uint32_t pointer_offset,
                      bool multi_thread,
                      uint32_t max_elements,
                      const char *label,
                      shr_lmm_hdl_t *hdl);


/*!
 * \brief Deletes pre-allocated memory manager.
 *
 * This function deletes an instance of the local memory manager.
 * As a result, all the resources allocated for this instance will
 * be freed.
 *
 * \param [in] hdl is the local memory handle.
 *
 * \return none
 */
extern void
shr_lmm_delete(shr_lmm_hdl_t hdl);

/*!
 * \brief Allocate an element.
 *
 * This function allocates a single element from the local memory pool.
 * If the memory pool is empty the function will allocate a new chunk
 * of elements (based on chunk_size provided by \ref shr_lmm_init()).
 *
 * \param [in] hdl is the local memory handle.
 *
 * \return pointer to an element on success or NULL on failure.
 */
extern void
*shr_lmm_alloc(shr_lmm_hdl_t hdl);

/*!
 * \brief Allocate a bulk of linked elements.
 *
 * This function allocates a bulk of elements from the local memory pool.
 * If the memory pool is empty the function will allocate a new chunk
 * of elements (based on chunk_size provided by \ref shr_lmm_init()).
 * If the \c cnt parameter is larger than the chunk size the chunk will be
 * of \c cnt size elements.
 *
 * \param [in] hdl is the local memory handle.
 * \param [in] cnt The number of elements to allocate in a single bulk
 * allocation.
 *
 * \return pointer to the first element of the bulk on success or NULL on
 * failure. The element list will be NULL terminated linked list
 */
extern void
*shr_lmm_alloc_bulk(shr_lmm_hdl_t hdl, uint32_t cnt);

/*!
 * \brief Free an element back to the free pool
 *
 * This function frees a single element to the local memory pool.
 *
 * \param [in] hdl is the local memory handle
 * \param [in] element is a pointer to the element to free
 *
 * \return None.
 */
extern void
shr_lmm_free(shr_lmm_hdl_t hdl, void *element);

/*!
 * \brief Free an element list back to the free pool.
 *
 * This function frees a bulk of elements to the local memory pool.
 *
 * \param [in] hdl is the local memory handle
 * \param [in] element is a pointer to the first element of the list to free.
 * The list must be NULL terminated list of elements.
 *
 * \return None.
 */
extern void
shr_lmm_free_bulk(shr_lmm_hdl_t hdl, void *element);

/*!
 * \brief Free all the allocated elements at once.
 *
 * This function frees all the elements that were allocated and restores its
 * free list of elements. Note that the handle is not being freed, and therefore
 * can be reused to allocate new elements.
 * The purpose of this function is to perform fast and efficient release of
 * local memory resources that were allocated by the caller.
 *
 * \param [in] hdl is the local memory handle
 *
 * \return None.
 */
extern void
shr_lmm_free_all(shr_lmm_hdl_t hdl);

/*!
 * \brief Local memory usage statistics.
 */
typedef struct {
    uint32_t in_use;    /*!< Number of elements in use         */
    uint32_t avail;     /*!< Number of available elements      */
    uint32_t allocated; /*!< Number of allocated elements (in_use + free) */
    uint32_t hwtr_mark; /*!< High watermark of elements in use */
    size_t chunk_size;  /*!< The number of elements per chunk */
    size_t element_size;/*!< The size of the basic element (in bytes) */
    const char *label;  /*!< String to identify the specific LMM instance. */
} shr_lmm_stat_t;

/*!
 * \brief Obtain usage statistics for given memory resource.
 *
 * This function obtains the usage statistics for a given memory resource.
 *
 * \param [in] hdl The local memory handle
 * \param [out] stats The usage statistics associated with this memory.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid input parameter.
 * \return SHR_E_INIT When this function is called prior to
 * \ref shr_lmm_global_init().
 */
extern int
shr_lmm_stat_get(shr_lmm_hdl_t hdl, shr_lmm_stat_t *stats);

/*!
 * \brief Obtain usage statistics for given memory resource based on ID.
 *
 * This function obtains the usage statistics for a given memory resource
 * that has its ID matches the requested ID.
 *
 * \param [in] id Unique string to identify the memory instance. Typically
 * this string will be the data type associated with the instance (first
 * parameter for \ref LMEM_MGR_INIT and \ref LMEM_MGR_REST_INIT
 * \param [out] stats The usage statistics associated with this memory.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid input parameter.
 * \return SHR_E_INIT When this function is called prior to
 * \ref shr_lmm_global_init().
 */
extern int
shr_lmm_stat_by_id_get(const char *id, shr_lmm_stat_t *stats);

/*!
 * \brief Obtain usage statistics for all local memory instances.
 *
 * This function obtains the usage statistics for a given memory resource.
 *
 * \param [in] len The size of the stats array.
 * \param [out] stats The usage statistics array where each memory instance
 * will occupy a single array element. The output of this array will be filled
 * up to \c len elements of this array.
 * \param [out] actual Indicats how many memory instances are available in the
 * system. The caller may call this function with len = 0 and stats = NULL for
 * the first time to determine the buffer size required to hold the statistics.
 * Then the caller may pass an allocated stats buffer and its size.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_FULL The provided stats array is too small.
 * \return SHR_E_INIT When this function is called prior to
 * \ref shr_lmm_global_init().
 */
extern int
shr_lmm_stat_get_all(size_t len, shr_lmm_stat_t *stats, size_t *actual);

#endif /* SHR_LMEM_MGR_H */
