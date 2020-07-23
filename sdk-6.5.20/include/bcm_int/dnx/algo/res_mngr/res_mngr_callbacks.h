/** \file res_mngr_callbacks.h
 * 
 * Internal DNX resource manager APIs 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef ALGO_RES_MNGR_CALLBACKS_INCLUDED
/*
 * { 
 */
#define ALGO_RES_MNGR_CALLBACKS_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
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
 * Upon creation, the algorithm must create a unique identifier (algo_instance_id) for each instance. This identifier will be used
 *   to manage this resource when the other APIs are called.
 *   If the algorithm is unique (has only one instance), the identifier has no significance.
 *   If the algorithm is unique and uses multi_set_template as internal library, it's recommended to
 *     use the multi_set_template's identifier.
 *
 * \see
 * dnx_algo_res_create
 */

/**
* \brief
*   Callback to create a new instance of the advanced algorithm.
* 
*
*  \par DIRECT INPUT:
*    \param [in] unit -
*      Relevant unit.
*    \param [in] create_data -
*      Pointed memory contains setup parameters required for the
*      creation of the resouce. See \ref dnx_algo_res_create_data_t
*    \param [in] extra_arguments
*      Pointed memory to hold extra arguments required for creating this resource. \n
*      \b As \b input - \n
*       The content of this pointer depends on the algorithm. It's the algorithm's \n
*       responsibility to verify it.
*       If the algorithm doesn't require extra arguments, this can be set to NULL.
*    \param [in] algo_instance_id
*      Pointer to place the Algorithm instance id that was created.
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
    uint32 module_id,
    resource_tag_bitmap_t * res_tag_bitmap,
    dnx_algo_res_create_data_t * create_data,
    void *extra_arguments,
    uint32 nof_elements,
    uint32 alloc_flags);

/**
* \brief
*   Callback to destroy an instance of the advanced algorithm.
* 
*
*  \par DIRECT INPUT:
*    \param [in] unit -
*      Relevant unit.
*    \param [in] algo_instance_id
*      Internal id for the algorithm.
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
    uint32 module_id,
    resource_tag_bitmap_t * res_tag_bitmap,
    void *extra_arguments);

/**
* \brief
*   Callback to allocate a free element of an advanced algorithm.
* 
*
*  \par DIRECT INPUT:
*    \param [in] unit -
*      Relevant unit.
*    \param [in] algo_instance_id -
*      Internal id for the algorithm.
*    \param [in] flags -
*      DNX_ALGO_RES_ALLOCATE_* flags
*    \param [in] extra_arguments
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
 */
typedef shr_error_e(
    *dnx_algo_res_allocate_cb) (
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 flags,
    void *extra_arguments,
    int *element);

/**
* \brief
*   Callback to check whether an element was allocated in an adavnced algorithm.
* 
*
*  \par DIRECT INPUT:
*    \param [in] unit -
*      Relevant unit.
*    \param [in] algo_instance_id -
*      Internal id for the algorithm.
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
 */
typedef shr_error_e(
    *dnx_algo_res_is_allocated_cb) (
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int element,
    uint8 *is_allocated);

/**
* \brief
*   Callback to free an allocated element in an advanced algorithm.
* 
*
*  \par DIRECT INPUT:
*    \param [in] unit -
*      Relevant unit.
*    \param [in] algo_instance_id -
*      Internal id for the algorithm.
*    \param [in] element -
*      Element to be freed.
*  \par DIRECT OUTPUT:
*    shr_error_e - 
*      Error return value
*  \remark
*    None
*  \see
*    dnx_algo_res_free
*    shr_error_e
 */
typedef shr_error_e(
    *dnx_algo_res_free_cb) (
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int element);

/**
 * \brief
 *   Callback to get the number of currently free elements.
 * 
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit -
 *      Relevant unit.
 *    \param [in] algo_instance_id -
 *      Internal id for the algorithm.
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
 */
typedef shr_error_e(
    *dnx_algo_res_nof_free_elements_get_cb) (
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int *nof_free_elements);

/**
 * \brief
 *   Callback to clear allocated elements in this resource.
 *
 *  \param [in] unit -
 *      Relevant unit.
 *  \param [in] algo_instance_id -
 *      Internal id for the algorithm.
 *   \return
 *       \retval Zero if no error was detected
 *       \retval Negative if error was detected. See \ref shr_error_e
 *  \remark
 *    None
 *  \see
 *    dnx_algo_res_clear
 *    dnx_algo_res_advanced_alogrithm_cb_t
 *    shr_error_e
 */
typedef shr_error_e(
    *dnx_algo_res_clear_cb) (
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap);

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
 *  \param [in] algo_instance_id -
 *      Internal id for the algorithm.
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
 */
typedef shr_error_e(
    *dnx_algo_res_get_next_cb) (
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int *element);

/**
* \brief
*   Callback to allocate a free block of an advanced algorithm,\n
*   which is used to allocate several continuous values.
*
*   \param [in] unit -
*       Relevant unit.
*   \param [in] algo_instance_id -
*       Internal id for the algorithm.
*   \param [in] flags -
*       DNX_ALGO_RES_ALLOCATE_* flags
*   \param [in] nof_elements -
*       Number of elements used to create this resource.
*   \param [in] extra_arguments
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
 */
typedef shr_error_e(
    *dnx_algo_res_allocate_several_cb) (
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 flags,
    uint32 nof_elements,
    void *extra_arguments,
    int *element);

/**
* \brief
*   Callback to free several allocated elements in an advanced algorithm.
*
*
*    \param [in] unit -
*      Relevant unit.
*    \param [in] algo_instance_id -
*      Internal id for the algorithm.
*    \param [in] nof_elements -
*      Number of elements to be freed.
*    \param [in] element -
*      Element to be freed.
*  \return
*    shr_error_e -
*      Error return value
*  \remark
*    None
*  \see
*    dnx_algo_res_free
*    shr_error_e
 */
typedef shr_error_e(
    *dnx_algo_res_free_several_cb) (
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 nof_elements,
    int element);

/**
* \brief
*   Callback to get number of used elements in range.
*
*    \param [in] unit - Relevant unit.
*    \param [in] module_id - Module ID.
*    \param [in] res_tag_bitmap - Relevant res tag bitmap.
*    \param [in] range_start - Range start.
*    \param [in] nof_elements_in_range - Range end.
*    \param [out] nof_allocated_elements - Pointer to the memory where the return value will be saved.
*  \return
*    shr_error_e - Error return value
*  \remark
*    None
*  \see
*    shr_error_e
 */
typedef shr_error_e(
    *dnx_algo_res_nof_allocated_elements_in_range_get_cb) (
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int range_start,
    int nof_elements_in_range,
    int *nof_allocated_elements);

/**
* \brief
*   Callback to set the algorithm_info.
*
*    \param [in] unit - Relevant unit.
*    \param [in] module_id - Module ID.
*    \param [in] res_tag_bitmap - Relevant res tag bitmap.
*    \param [in] algorithm_info - Algorithm info to be set.
*  \return
*    shr_error_e - Error return value
*  \remark
*    None
*  \see
*    shr_error_e
 */
typedef shr_error_e(
    *dnx_algo_res_advanced_algorithm_info_set_cb) (
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    resource_tag_bitmap_tag_info_t * algorithm_info);

/**
* \brief
*   Callback to get the algorithm_info.
*
*    \param [in] unit - Relevant unit.
*    \param [in] module_id - Module ID.
*    \param [in] res_tag_bitmap - Relevant res tag bitmap.
*    \param [out] algorithm_info - Pointer to save algorithm info into.
*  \return
*    shr_error_e - Error return value
*  \remark
*    None
*  \see
*    shr_error_e
 */
typedef shr_error_e(
    *dnx_algo_res_advanced_algorithm_info_get_cb) (
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    resource_tag_bitmap_tag_info_t * algorithm_info);

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
    dnx_algo_res_create_cb create_cb;   /* Mandatory */
    dnx_algo_res_allocate_cb allocate_cb;       /* Mandatory */
    dnx_algo_res_is_allocated_cb is_allocated_cb;       /* Mandatory */
    dnx_algo_res_free_cb free_cb;       /* Mandatory */
    dnx_algo_res_destroy_cb destroy_cb; /* Currently not mandatory,but better be implemented. */
    dnx_algo_res_nof_free_elements_get_cb nof_free_elements_get_cb;     /* Mandatory */
    dnx_algo_res_clear_cb clear_cb;     /* optional */
    dnx_algo_res_allocate_several_cb allocate_several_cb;       /* optional */
    dnx_algo_res_free_several_cb free_several_cb;       /* optional */
    dnx_algo_res_get_next_cb get_next_cb;       /* optional */
    dnx_algo_res_nof_allocated_elements_in_range_get_cb nof_allocated_elements_in_range_get_cb; /* optional */
    dnx_algo_res_advanced_algorithm_info_set_cb algorithm_info_set;     /* optional */
    dnx_algo_res_advanced_algorithm_info_get_cb algorithm_info_get;     /* optional */
} dnx_algo_res_advanced_alogrithm_cb_t;

/*
 * } 
 */
#endif/*_ALGO_RES_MNGR_CALLBACKS_INCLUDED__*/
