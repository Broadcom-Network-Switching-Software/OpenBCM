/** \file resource_tag_bitmap.h
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * \brief
 * Overview
 *
 * The resource tag bitmap is used to allocate resources in the system according to preset
 * criteria.
 *
 * Glossary:
 * bitmap / resource / tagged bitmap - one instance of resource_tag_bitmap.
 * element - a single bit in the bitmap, usually mapped to a table index (outside of the bitmap scope).
 * block - one or more elements that are allocated in a single allocation. Usually consecutive, but in the
 *          case of sparse allocation (allocation using the \ref RESOURCE_TAG_BITMAP_ALLOC_SPARSE
 *          flag) , could be non-consecutive.
 * pool - the set of all elements in the resource.
 * grain - a set of consecutive elements that have a common property represented by tag.
 * tag - an identifier of a property common to all elements in a grain.
 *
 * During allocation, the bitmap will look for a free block of elements as defined by the user,
 * in a grain which fits the given tag.
 * If a matching free block was found, then the first element of that block will be returned.
 * If no grain with the require tag existed, then the grain will search for a free block
 * in a grain that doesn't have a tag, and will assign the tag to the grain.
 *
 * If no tag is used, then blocks can be allocated anywhere in the pool.
 *
 * Some exceptions can be made to the basic algorithm, by using the
 * RESOURCE_TAG_BITMAP_ALLOC_* flags.
 */
#ifndef RESOURCE_TAG_BITMAP_H_INCLUDED
/*
 * {
 */
#define RESOURCE_TAG_BITMAP_H_INCLUDED

/** { */
/*************
* INCLUDES  *
*************/
/** { */
#include <bcm_int/dnx/algo/res_mngr/resource_tag_bitmap_utils.h>

/*
 * Internal struct of resource tag bitmap.
 */
typedef struct
{
    /*
     * struct that represent resource tag bitmap utils, contains fields that are shared with other algorithm that used tagged bitmap
     * */
    resource_tag_bitmap_utils_t resource;
    /**
     * array of lowest element/s that was/were free by tag/s
     */
    int *lowest_free;
    /**
     * array of next element/s after last alloc block by tag/s
     */
    int *next_alloc;
}  *resource_tag_bitmap_t;

/** } */

/*************
* FUNCTIONS *
*************/

/**
 * \brief - Create a tagged bitmap resource.
 *
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] node_id - Node ID.
 * \param [in] res_tag_bitmap - Resource tag bitmap to be destroyed.
 * \param [in] create_info - Relevant create information.
 * \param [in] extra_arguments - Placeholder to fill prototype requirement for advanced algorithms.
 * \param [in] alloc_flags - SW state allocation flags.
 *       This flags value is determend by the variable type (dnx data for example) of the additional variables specified in the xml
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   There are count / grain_size grains, and each one has a tag associated.
 * \see
 *   * None
 */
shr_error_e resource_tag_bitmap_create(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap,
    dnx_algo_res_create_data_t * create_info,
    void *extra_arguments,
    uint32 alloc_flags);

/**
 * \brief
 *   Allocate an element or block of elements of a particular resource
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] node_id - Node ID.
 *   \param [in] res_tag_bitmap - Resource tag bitmap to be allocated into.
 *   \param [in,out] alloc_info - All relevant data needed during the allocation.
 *                                  alloc_info.first_references is an output that
 *                                  indicate whether it's the first reference for a tagged grain.   
 *   \param [in,out] elem - Pointer to place the allocated element.
 *       \b As \b output - \n
 *       Holds the first *allocated* element in the pattern.
 *       \b As \b input - \n
 *       If flag \ref DNX_ALGO_RES_ALLOCATE_WITH_ID is set, pass the first element to be *allocated* here.
 *   \return
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   \remark
 *      This will allocate a single block of the requested number of elements
 *      of this resource (each of which may be a number of elements taken from
 *      the underlying pool).
 *
 *   \see
 *   * None
*****************************************************/
shr_error_e resource_tag_bitmap_alloc(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    resource_tag_bitmap_utils_alloc_info_t * alloc_info,
    int *elem);

/*!
* \brief
*   Allocate a free element.
*
*    \param [in] unit - Relevant unit.
*    \param [in] node_id - Node ID.
*    \param [in] res_tag_bitmap - Resource tag bitmap.
*    \param [in] flags - DNX_ALGO_RES_ALLOCATE_* flags
*    \param [in] extra_arguments - Placeholder to fill prototype requirement for advanced algorithms.
*    \param [in] element - Pointer to place the allocated element.
*      \b As \b output - \n
*        Holds the allocated element.
*      \b As \b input - \n
*        If flag \ref DNX_ALGO_RES_ALLOCATE_WITH_ID is set, pass the element to be allocated here.
*   \return
*    shr_error_e - Error return value
*  \remark
*    None
*  \see
*    dnx_algo_res_allocate
*    sw_state_res_tag_bitmap_alloc
*    shr_error_e
*****************************************************/
shr_error_e resource_tag_bitmap_allocate_single(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap,
    uint32 flags,
    void *extra_arguments,
    int *element);

/**
* \brief
*   Allocate several elements.
*
*   \param [in] unit - Relevant unit.
*   \param [in] node_id - Node ID.
*   \param [in] res_tag_bitmap - Resource tag bitmap.
*   \param [in] flags - DNX_ALGO_RES_ALLOCATE_* flags
*   \param [in] nof_elements - Number of elements.
*   \param [in] extra_arguments - Placeholder to fill prototype requirement for advanced algorithms.
*   \param [in,out] element -Pointer to place the allocated element.
*       \b As \b output - \n
*       Holds the allocated element.
*       \b As \b input - \n
*       If flag \ref DNX_ALGO_RES_ALLOCATE_WITH_ID is set, pass the element to be allocated here.
*   \return
*       shr_error_e - Error return value
*   \remark
*       None
*   \see
*       dnx_algo_res_allocate_several
*       sw_state_res_tag_bitmap_allocate_several
*       shr_error_e
*****************************************************/
shr_error_e resource_tag_bitmap_allocate_several(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap,
    uint32 flags,
    uint32 nof_elements,
    void *extra_arguments,
    int *element);

/**
 * \brief
 *   Free a single element of a particular bitmap.
 *   \param [in] unit - Relevant unit.
 *   \param [in] node_id - Node ID.
 *   \param [in] res_tag_bitmap - Resource tag bitmap to be freed from.
 *   \param [in] elem - The element to free.
 *   \param [out] extra_argument - In case it is not NULL, will be filled with boolean indication per tag level (uint8*)
 *                                 of whether it's the last reference to that tagged grain.
 *   \return
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   \remark
 *
 *   \see
 *     * None
*****************************************************/
shr_error_e resource_tag_bitmap_free(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap,
    int elem,
    void *extra_argument);

/**
 * \brief
 *   Free an element or block of elements of a particular bitmap.
 *   The only fields used in the free info are count for freeing one or more consecutive elements.
 *   \param [in] unit - Relevant unit.
 *   \param [in] node_id - Node ID.
 *   \param [in] res_tag_bitmap - Resource tag bitmap to be freed from.
 *   \param [in] nof_elements - Number of elements to br freed.
 *   \param [in] elem - The element to free(used a starting point).
 *   \param [out] extra_argument - In case it is not NULL, will be filled with boolean indication per tag level (uint8*)
 *                                 of whether it's the last reference to that tagged grain.
 *   \return
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   \remark
 *
 *   \see
 *     * None
*****************************************************/
shr_error_e resource_tag_bitmap_free_several(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap,
    uint32 nof_elements,
    int elem,
    void *extra_argument);

/**
* \brief
*   Prints resource data includes tags and grains
*
*   \param [in] unit -  Relevant unit.
*   \param [in] node_id - Node ID.
*   \param [in] res_tag_bitmap - Resource tag bitmap to print.
*   \param [in] data - relevant data to be printed.
*  \return
*    \retval Zero if no error was detected
*    \retval Negative if error was detected. See \ref shr_error_e
*  \remark
*    None
*  \see
*    None
*****************************************************/
int resource_tag_bitmap_print(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap,
    dnx_algo_res_dump_data_t * data);

/**
 * \brief - Force set a tag to a range of elements..
 *
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] node_id - Node ID.
 * \param [in] res_tag_bitmap - Resource tag bitmap to set tag on.
 * \param [in,out] tag_info - Tag info.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e resource_tag_bitmap_tag_set(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap,
    void *tag_info);

/**
 * \brief - Get the tag for a specific element.
 *
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] node_id - Node ID.
 * \param [in] res_tag_bitmap - Resource tag bitmap to get the tag from.
 * \param [in] tag_info - Tag info.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e resource_tag_bitmap_tag_get(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap,
    void *tag_info);

/**
 * \brief
 *   Clear all the elements of the bitmap.
 *
 *   \param [in] unit -
 *       Relevant unit.
 *   \param [in] node_id - Node ID.
 *   \param [in] res_tag_bitmap - Resource tag bitmap to be cleared.
 *   \return
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   \remark
 *      None
 *   \see
 *       dnx_algo_res_simple_bitmap_clear
*****************************************************/
shr_error_e resource_tag_bitmap_clear(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap);

/**
 * \brief - Returns the number of free elements for this resource.
 *
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] node_id - Node ID.
 * \param [in] res_tag_bitmap - Resource tag bitmap.
 * \param [out] nof_free_elements - Pointer will be filled with the number
 *                                  of free elements in this resource.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e resource_tag_bitmap_nof_free_elements_get(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap,
    int *nof_free_elements);

/**
* \brief
*   Gets number of used elements in the given range.
*
*    \param [in] unit - Relevant unit.
*    \param [in] node_id - Node ID.
*    \param [in] res_tag_bitmap - Relevant res tag bitmap.
*    \param [in] range_start - First element in range.
*    \param [in] nof_elements_in_range - Range size.
*    \param [out] nof_allocated_elements - Pointer to the memory where the return value will be saved.
*  \return
*    shr_error_e - Error return value
*  \remark
*    None
*  \see
*    shr_error_e
*****************************************************/
shr_error_e resource_tag_bitmap_nof_allocated_elements_in_range_get(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap,
    int range_start,
    int nof_elements_in_range,
    int *nof_allocated_elements);

/**
 * \brief - Returns the number of allocated elements per grain.
 *          Note that this API will be active only if _CREATE_GET_NOF_ELEMENTS_PER_GRAIN flag was given
 *          during creation of the resource.
 *
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] node_id - Node ID.
 * \param [in] res_tag_bitmap - Resource tag bitmap.
 * \param [in] grain_index - Relevant grain index.
 * \param [in] tag_level - Relevant tag level.
 * \param [out] nof_allocated_elements - Pointer will be filled with the number
 *                                  of allocated bits in this grain.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e resource_tag_bitmap_nof_used_elements_in_grain_get(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 grain_index,
    int tag_level,
    uint32 *nof_allocated_elements);

/**
 * \brief - Returns the create info used during create.
 *
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] node_id - Node ID.
 * \param [in] res_tag_bitmap - Resource tag bitmap.
 * \param [in] create_info - Pointer to location in which will be saved the create info.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e resource_tag_bitmap_create_info_get(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    resource_tag_bitmap_utils_create_info_t * create_info);

/*
* \brief
*   Check whether an element was allocated.
*
*    \param [in] unit - Relevant unit.
*    \param [in] node_id - Node ID.
*    \param [in] res_tag_bitmap - Resource tag bitmap.
*    \param [in] element - Element to be checked.
*    \param [in] is_allocated - Pointer to be filled with allocated / free indication.
*  \return
*    shr_error_e - Error return value
*  \remark
*    None
*  \see
*    dnx_algo_res_allocate
*    sw_state_res_tag_bitmap_check
*    shr_error_e
*****************************************************/
shr_error_e resource_tag_bitmap_is_allocated(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap,
    int element,
    uint8 *is_allocated);

/*
* \brief
*    The function is setting given data at element index.
*    Can be used only if RESOURCE_TAG_BITMAP_CREATE_DATA_PER_ENTRY flag was given during creation of the resource.
*    \param [in] unit - Relevant unit.
*    \param [in] node_id - Node ID.
*    \param [in] res_tag_bitmap - Resource tag bitmap.
*    \param [in] element - Element to be checked.
*    \param [in] data - Pointer to be with the data that should be set.
*  \return
*    shr_error_e - Error return value
*  \remark
*    None
*/
shr_error_e resource_tag_bitmap_data_per_entry_set(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap,
    int element,
    const void *data);

/*
* \brief
*    The function is returning data value at element index.
*    \param [in] unit - Relevant unit.
*    \param [in] node_id - Node ID.
*    \param [in] res_tag_bitmap - Resource tag bitmap.
*    \param [in] element - Element to be checked.
*    \param [in] data - Pointer to be with the data that should be set.
*  \return
*    shr_error_e - Error return value
*  \remark
*    None
*/
shr_error_e resource_tag_bitmap_data_per_entry_get(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap,
    int element,
    void *data);

/*
 * }
 */

/*
 * } RESOURCE_TAG_BITMAP_H_INCLUDED
 */
#endif
