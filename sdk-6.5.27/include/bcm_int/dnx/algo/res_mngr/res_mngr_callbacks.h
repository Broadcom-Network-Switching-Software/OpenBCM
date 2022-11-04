/** \file res_mngr_callbacks.h
 * 
 * Internal DNX resource manager APIs 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */
#ifndef ALGO_RES_MNGR_CALLBACKS_INCLUDED
/*
 * { 
 */
#define ALGO_RES_MNGR_CALLBACKS_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#include <bcm_int/dnx/algo/res_mngr/res_mngr_types.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr.h>
#include <bcm_int/dnx/algo/res_mngr/resource_tag_bitmap.h>

/** 
 * Callbacks for dnx_algo_res_create.
 * 
 * Note that these callbacks don't take core_id as an argument.
 *   If the advanced algorithm requires core as an argument, it must be provided in the extra arguments structure.
 * 
 * \see
 * dnx_algo_res_create
 */

/**
* \brief
*   Callback to create a new instance of the advanced algorithm.
*
*  \par DIRECT INPUT:
*    \param [in] unit -
*      Relevant unit.
*    \param [in] create_data -
*      Pointed memory contains setup parameters required for the
*      creation of the resource. See \ref dnx_algo_res_create_data_t
*    \param [in] extra_arguments
*      Pointed memory to hold extra arguments required for creating this resource. \n
*      \b As \b input - \n
*       The content of this pointer depends on the algorithm. It's the algorithm's \n
*       responsibility to verify it.
*       If the algorithm doesn't require extra arguments, this can be set to NULL.
*      \b As \b output - \n
*        Holds the created instance id.
*  \par DIRECT OUTPUT:
*    \retval Zero if no error was detected
*    \retval Negative if error was detected. See \ref shr_error_e
*  \remark
*    None
*  \see
*    dnx_algo_res_create
*    shr_error_e
*/
typedef shr_error_e(
    *dnx_algo_res_create_cb) (
    int unit,
    uint32 node_id,
    void *res_mngr,
    dnx_algo_res_create_data_t * create_data,
    void *extra_arguments,
    uint32 alloc_flags);

/**
* \brief
*   Callback to destroy an instance of the advanced algorithm.
*
*  \par DIRECT INPUT:
*    \param [in] unit -
*      Relevant unit.
*    \param [in] extra_arguments
*      Pointed memory to hold extra arguments required for creating this resource. \n
*      \b As \b input - \n
*       The content of this pointer depends on the algorithm. It's the algorithm's \n
*       responsibility to verify it.
*       If the algorithm doesn't require extra arguments, this can be set to NULL.
*  \par DIRECT OUTPUT:
*    \retval Zero if no error was detected
*    \retval Negative if error was detected. See \ref shr_error_e
*  \par INDIRECT OUTPUT:
*    Depends on the advanced algorithm, the specified advanced algorithm's sw state
*       will be destroyed.
*  \remark
*    None
*  \see
*    dnx_algo_res_destroy
*    shr_error_e
*/
typedef shr_error_e(
    *dnx_algo_res_destroy_cb) (
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t * res_mngr,
    void *extra_arguments);

/**
* \brief
*   Callback to allocate a free element of an advanced algorithm.
*
*  \par DIRECT INPUT:
*    \param [in] unit -
*      Relevant unit.
*    \param [in] flags -
*      DNX_ALGO_RES_ALLOCATE_* flags
*    \param [in, out] extra_arguments
*      Pointed memory to hold extra arguments required for allocating this element. \n
*      \b As \b input - \n
*       The content of this pointer depends on the algorithm. It's the algorithm's \n
*       responsibility to verify it.
*       If the algorithm doesn't require extra arguments, this can be set to NULL.
*    \param [in] element -
*      Pointer to place the allocated element.
*      \b As \b output - \n
*        Holds the allocated element.
*      \b As \b input - \n
*        If flag \ref DNX_ALGO_RES_ALLOCATE_WITH_ID is set, pass the element to be allocated here.
*  \par INDIRECT OUTPUT:
*    *element -
*      Value of the allocated element.
*  \par DIRECT OUTPUT:
*    shr_error_e - 
*      Error return value
*  \remark
*    None
*  \see
*    dnx_algo_res_allocate
*    shr_error_e
*****************************************************/
typedef shr_error_e(
    *dnx_algo_res_allocate_cb) (
    int unit,
    uint32 node_id,
    void *res_mngr,
    uint32 flags,
    void *extra_arguments,
    int *element);

/**
* \brief
*   Callback to check whether an element was allocated in an advanced algorithm.
*
*  \par DIRECT INPUT:
*    \param [in] unit -
*      Relevant unit.
*    \param [in] element -
*      Element to be checked.
*    \param [in] is_allocated -
*      Pointer to be filled with allocated / free indication.
*  \par INDIRECT OUTPUT:
*    *is_allocated -
*       boolean indication of the element's existence.
*  \par DIRECT OUTPUT:
*    shr_error_e - 
*      Error return value
*  \remark
*    None
*  \see
*    dnx_algo_res_allocate
*    shr_error_e
*****************************************************/
typedef shr_error_e(
    *dnx_algo_res_is_allocated_cb) (
    int unit,
    uint32 node_id,
    void *res_mngr,
    int element,
    uint8 *is_allocated);

/**
* \brief
*   Callback to free an allocated element in an advanced algorithm.
*
*  \par DIRECT INPUT:
*    \param [in] unit -
*      Relevant unit.
*    \param [in] element -
*      Element to be freed.
*    \param [in, out] extra_arguments -
*      Pointed memory to hold extra arguments returning from freeing this element.
*  \par DIRECT OUTPUT:
*    shr_error_e - 
*      Error return value
*  \remark
*    None
*  \see
*    dnx_algo_res_free
*    shr_error_e
*****************************************************/
typedef shr_error_e(
    *dnx_algo_res_free_cb) (
    int unit,
    uint32 node_id,
    void *res_mngr,
    int element,
    void *extra_argument);

/**
 * \brief
 *   Callback to get the number of currently free elements.
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit -
 *      Relevant unit.
 *    \param [in] nof_free_elements -
 *      Int pointer to memory to load output into. \n
 *      \b As \b output - \n
 *        This procedure loads the memory with the number of free elements in this resource.
 *  \par DIRECT OUTPUT:
 *    \retval Zero if no error was detected
 *    \retval Negative if error was detected. See \ref shr_error_e
 *  \par INDIRECT INPUT:
 *    The sw state used by the advanced algorithm.
 *  \par INDIRECT OUTPUT:
 *   \b *nof_free_elements \n
 *     See DIRECT INPUT above
 *  \remark
 *    None
 *  \see
 *    dnx_algo_res_nof_free_elements_get
 *    dnx_algo_res_advanced_alogrithm_cb_t
 *    shr_error_e
 *****************************************************/
typedef shr_error_e(
    *dnx_algo_res_nof_free_elements_get_cb) (
    int unit,
    uint32 node_id,
    void *res_mngr,
    int *nof_free_elements);

/**
 * \brief
 *   Callback to clear allocated elements in this resource.
 *
 *  \param [in] unit -
 *      Relevant unit.
 *   \return
 *       \retval Zero if no error was detected
 *       \retval Negative if error was detected. See \ref shr_error_e
 *  \remark
 *    None
 *  \see
 *    dnx_algo_res_clear
 *    dnx_algo_res_advanced_alogrithm_cb_t
 *    shr_error_e
 *****************************************************/
typedef shr_error_e(
    *dnx_algo_res_clear_cb) (
    int unit,
    uint32 node_id,
    void *res_mngr);

/**
 * \brief
 *      Given a resource name and element,
 *      returns the next allocated element for this resource.
 *      The returned element will be equal to or greater than
 *      the given element. It is the user's responsibility to
 *      update it between iterations. When there are no more
 *      allocated elements, DNX_ALGO_RES_ILLEGAL_ELEMENT will be
 *      returned.
 *
 *  \param [in] unit -
 *      Relevant unit.
 *  \param [in,out] element -
 *       \b As \b input - \n
 *       The element to start searching for the next allocated element from.
 *       \b As \b output - \n
 *       he next allocated element. It will be >= from the element given as input.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *      It is the user's responsibility to update the element between iterations. This usually means
 *       giving it a ++.
 * \see
 *      None
 *****************************************************/
typedef shr_error_e(
    *dnx_algo_res_get_next_cb) (
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_mngr,
    int *element);

/**
* \brief
*   Callback to allocate a free block of an advanced algorithm,\n
*   which is used to allocate several continuous values.
*
*   \param [in] unit -
*       Relevant unit.
*   \param [in] flags -
*       DNX_ALGO_RES_ALLOCATE_* flags
*   \param [in] nof_elements -
*       Number of elements used to create this resource.
*   \param [in, out] extra_arguments
*       Pointed memory to hold extra arguments required for allocating this element. \n
*       \b As \b input - \n
*       The content of this pointer depends on the algorithm. It's the algorithm's \n
*       responsibility to verify it.
*       If the algorithm doesn't require extra arguments, this can be set to NULL.
*   \param [in,out] element -
*       Pointer to place the allocated element.
*       \b As \b output - \n
*       Holds the allocated element.
*       \b As \b input - \n
*       If flag \ref DNX_ALGO_RES_ALLOCATE_WITH_ID is set, pass the element to be allocated here.
*   \return
*   shr_error_e -
*       Error return value
*   \remark
*       None
*   \see
*       dnx_algo_res_allocate_several
*       shr_error_e
*****************************************************/
typedef shr_error_e(
    *dnx_algo_res_allocate_several_cb) (
    int unit,
    uint32 node_id,
    void *res_mngr,
    uint32 flags,
    uint32 nof_elements,
    void *extra_arguments,
    int *element);

/**
* \brief
*   Callback to free several allocated elements in an advanced algorithm.
*
*    \param [in] unit -
*      Relevant unit.
*    \param [in] nof_elements -
*      Number of elements to be freed.
*    \param [in] element -
*      Element to be freed.
*    \param [out] extra_arguments -
*      Pointed memory to hold extra arguments returning from freeing this element.
*  \return
*    shr_error_e -
*      Error return value
*  \remark
*    None
*  \see
*    dnx_algo_res_free
*    shr_error_e
*****************************************************/
typedef shr_error_e(
    *dnx_algo_res_free_several_cb) (
    int unit,
    uint32 node_id,
    void *res_mngr,
    uint32 nof_elements,
    int element,
    void *extra_argument);

/**
* \brief
*   Callback to prints resource data
*   \param [in] unit -  Relevant unit.
*   \param [in] node_id - Node ID.
*   \param [in] algo_res - Resource.
*  \return
*    shr_error_e
*  \remark
*    None
*  \see
*    None
*****************************************************/
typedef shr_error_e(
    *dnx_algo_res_print_cb) (
    int unit,
    uint32 node_id,
    void *res_mngr,
    dnx_algo_res_dump_data_t * data);

/**
* \brief
*   Callback to get number of used elements in range.
*
*    \param [in] unit - Relevant unit.
*    \param [in] node_id - Node ID.
*    \param [in] res_mngr - Relevant resource manager.
*    \param [in] range_start - Range start.
*    \param [in] nof_elements_in_range - Range end.
*    \param [out] nof_allocated_elements - Pointer to the memory where the return value will be saved.
*  \return
*    shr_error_e - Error return value
*  \remark
*    None
*  \see
*    shr_error_e
*****************************************************/
typedef shr_error_e(
    *dnx_algo_res_nof_allocated_elements_in_range_get_cb) (
    int unit,
    uint32 node_id,
    void *res_mngr,
    int range_start,
    int nof_elements_in_range,
    int *nof_allocated_elements);

/**
* \brief
*   Callback to set the algorithm_info.
*
*    \param [in] unit - Relevant unit.
*    \param [in] node_id - Node ID.
*    \param [in] res_mngr - Relevant resource manager.
*    \param [in] algorithm_info - Algorithm info to be set.
*  \return
*    shr_error_e - Error return value
*  \remark
*    None
*  \see
*    shr_error_e
*****************************************************/
typedef shr_error_e(
    *dnx_algo_res_advanced_algorithm_info_set_cb) (
    int unit,
    uint32 node_id,
    void *res_mngr,
    void *algorithm_info);

/**
* \brief
*   Callback to get the algorithm_info.
*
*    \param [in] unit - Relevant unit.
*    \param [in] node_id - Node ID.
*    \param [in] res_mngr - Relevant resource manager.
*    \param [out] algorithm_info - Pointer to save algorithm info into.
*  \return
*    shr_error_e - Error return value
*  \remark
*    None
*  \see
*    shr_error_e
*****************************************************/
typedef shr_error_e(
    *dnx_algo_res_advanced_algorithm_info_get_cb) (
    int unit,
    uint32 node_id,
    void *res_mngr,
    void *algorithm_info);

/**
* \brief
*   Callback to set the data needed for data_per_entry.
*
*    \param [in] unit - Relevant unit.
*    \param [in] node_id - Node ID.
*    \param [in] res_mngr - Relevant res tag bitmap.
*    \param [in] element - Element to which will be added the data.
*    \param [in] data - data per entry to be set.
*  \return
*    shr_error_e - Error return value
*  \remark
*    None
*  \see
*    shr_error_e
*****************************************************/
typedef shr_error_e(
    *dnx_algo_res_data_per_entry_set_cb) (
    int unit,
    uint32 node_id,
    void *res_mngr,
    int element,
    const void *data);

/**
* \brief
*   Callback to get the data needed for data_per_entry.
*
*    \param [in] unit - Relevant unit.
*    \param [in] node_id - Node ID.
*    \param [in] res_mngr - Relevant resource manager.
*    \param [in] element - Element to which will be added the data.
*    \param [out] data - Pointer to save data per entry into.
*  \return
*    shr_error_e - Error return value
*  \remark
*    None
*  \see
*    shr_error_e
*****************************************************/
typedef shr_error_e(
    *dnx_algo_res_data_per_entry_get_cb) (
    int unit,
    uint32 node_id,
    void *res_mngr,
    int element,
    void *data);

/**
 * \brief Callback struct for dnx_algo_res_create.
 *
 * This structure contains the callbacks that will be called when an advanced resource
 *  is accessed.
 * 
 *  The callbacks for create, allocate, is_allocated and free are mandatory, and the rest are optional.
 *  
 *  The callback for destroy is currently not mandatory, but might be soon,
 *      so it's best to implement it.
 *   
 *  \see 
 * dnx_algo_res_create
 */
typedef struct
{
    char algo_name[DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH];  /* Mandatory */
    dnx_algo_res_create_cb create_cb;   /* Mandatory */
    dnx_algo_res_allocate_cb allocate_cb;       /* Mandatory */
    dnx_algo_res_is_allocated_cb is_allocated_cb;       /* Mandatory */
    dnx_algo_res_free_cb free_cb;       /* Mandatory */
    dnx_algo_res_nof_free_elements_get_cb nof_free_elements_get_cb;     /* Mandatory */
    dnx_algo_res_clear_cb clear_cb;     /* optional */
    dnx_algo_res_allocate_several_cb allocate_several_cb;       /* optional */
    dnx_algo_res_free_several_cb free_several_cb;       /* optional */
    dnx_algo_res_get_next_cb get_next_cb;       /* optional */
    dnx_algo_res_print_cb print_cb;     /* optional */
    dnx_algo_res_nof_allocated_elements_in_range_get_cb nof_allocated_elements_in_range_get_cb; /* optional */
    dnx_algo_res_advanced_algorithm_info_set_cb algorithm_info_set;     /* optional */
    dnx_algo_res_advanced_algorithm_info_get_cb algorithm_info_get;     /* optional */
    dnx_algo_res_data_per_entry_set_cb data_per_entry_set;      /* optional */
    dnx_algo_res_data_per_entry_get_cb data_per_entry_get;      /* optional */
} dnx_algo_res_advanced_alogrithm_cb_t;

/*
 * } 
 */
#endif/*_ALGO_RES_MNGR_CALLBACKS_INCLUDED__*/
