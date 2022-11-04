/** \file resource_tag_bitmap_fast.h
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * \brief
 * Overview
 *
 * The resource tag bitmap fast is used to allocate resources in the system according to preset
 * criteria. It's resource tag bitmap fast that has faster performance but it has higher memory consumption
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
#ifndef RESOURCE_TAG_BITMAP_FAST_H_INCLUDED
/*
 * {
 */
#define RESOURCE_TAG_BITMAP_FAST_H_INCLUDED

/** { */
/*************
* INCLUDES  *
*************/
#include <bcm_int/dnx/algo/res_mngr/resource_tag_bitmap_utils.h>
#include <soc/dnxc/swstate/types/sw_state_linked_list.h>

/*
 * Struct of resource tag bitmap fast.
 */
typedef struct
{
    /*
     * struct that represent resource tag bitmap utils, contains fields that are shared with other algorithm that used tagged bitmap
     * */
    resource_tag_bitmap_utils_t resource;
    /*
     * holding multihead linked list of the free grains per tags
     * */
    sw_state_sorted_multihead_ll_t free_grains;
    /*
     * linked list head index of the empty groups - (linked list that hold the grains that were not tagged)
     */
    int empty_groups_ll_head_idx;
}  *resource_tag_bitmap_fast_t;

/*************
* GLOBALS   *
*************/
/*
 * {
 */
/*
 * }
 */

/**
 * \brief
 * FOR INTERNAL USE ONLY:
 * struct that used for alloc info grain node in the linked list free grains
 */
typedef struct
{
    /*
     * the tags of the grain node in the linked list
     */
    uint32 tags[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS];
    /*
     *the allocated grain node in the linked list
     */
    sw_state_ll_node_t grain_node;
    /*
     * Starting ID of range of the grain
     */
    int grain_start;
    /*
     * ending ID of range of the grain
     * I.e., elements in the range {3,6}, set range_start to 3 and range_end to 7.
     */
    int grain_end;
    /*
     * Boolean indication of whether any of the grains involved were unused before this allocation.
     */
    uint8 first_references[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS];
} resource_tag_bitmap_grained_alloc_info_t;

/*************
* FUNCTIONS *
*************/

/**
 * \brief - Create a tagged bitmap resource.
 *
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] node_id - Node ID.
 * \param [in] res_tag_bitmap_fast - resource tag bitmap fast to be destroyed.
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
shr_error_e resource_tag_bitmap_fast_create(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap_fast,
    dnx_algo_res_create_data_t * create_info,
    void *extra_arguments,
    uint32 alloc_flags);

/*!
* \brief
*   Allocate a free element.
*
*    \param [in] unit - Relevant unit.
*    \param [in] node_id - Node ID.
*    \param [in] res_tag_bitmap_fast - resource tag bitmap fast.
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
shr_error_e resource_tag_bitmap_fast_allocate_single(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap_fast,
    uint32 flags,
    void *extra_arguments,
    int *element);

/**
* \brief
*   Allocate several elements.
*
*   \param [in] unit - Relevant unit.
*   \param [in] node_id - Node ID.
*   \param [in] res_tag_bitmap_fast - resource tag bitmap fast.
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
shr_error_e resource_tag_bitmap_fast_allocate_several(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap_fast,
    uint32 flags,
    uint32 nof_elements,
    void *extra_arguments,
    int *element);

/**
 * \brief - Force set a tag to a range of elements.
 *
 * \param [in] unit                - ID of the device to be used with this bitmap.
 * \param [in] node_id             - Node ID.
 * \param [in] res_tag_bitmap_fast - Resource tag bitmap fast to set tag on.
 * \param [in,out] tag_info        - Tag info.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e resource_tag_bitmap_fast_tag_set(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap_fast,
    void *tag_info);

/**
 * \brief - Get the tag for a specific element.
 *
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] node_id - Node ID.
 * \param [in] res_tag_bitmap_fast - resource tag bitmap fast to get the tag from.
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
shr_error_e resource_tag_bitmap_fast_tag_get(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap_fast,
    void *tag_info);

/*
* \brief
*   Check whether an element was allocated.
*
*    \param [in] unit - Relevant unit.
*    \param [in] node_id - Node ID.
*    \param [in] res_tag_bitmap_fast - resource tag bitmap fast.
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
shr_error_e resource_tag_bitmap_fast_is_allocated(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap_fast,
    int element,
    uint8 *is_allocated);

/**
 * \brief - Returns the number of free elements for this resource.
 *
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] node_id - Node ID.
 * \param [in] res_tag_bitmap_fast - resource tag bitmap fast.
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
shr_error_e resource_tag_bitmap_fast_nof_free_elements_get(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap_fast,
    int *nof_free_elements);

/**
* \brief
*   Gets number of used elements in the given range.
*
*    \param [in] unit - Relevant unit.
*    \param [in] node_id - Node ID.
*    \param [in] res_tag_bitmap_fast - Relevant res tag bitmap.
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
shr_error_e resource_tag_bitmap_fast_nof_allocated_elements_in_range_get(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap_fast,
    int range_start,
    int nof_elements_in_range,
    int *nof_allocated_elements);

/**
 * \brief
 *   Free a single element of a particular bitmap.
 *   \param [in] unit - Relevant unit.
 *   \param [in] node_id - Node ID.
 *   \param [in] res_tag_bitmap_fast - resource tag bitmap fast to be freed from.
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
shr_error_e resource_tag_bitmap_fast_free(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap_fast,
    int elem,
    void *extra_argument);

/**
 * \brief
 *   Free an element or block of elements of a particular bitmap.
 *   The only fields used in the free info are count for freeing one or more consecutive elements.
 *   \param [in] unit - Relevant unit.
 *   \param [in] node_id - Node ID.
 *   \param [in] res_tag_bitmap_fast - resource tag bitmap fast to be freed from.
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
shr_error_e resource_tag_bitmap_fast_free_several(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap_fast,
    uint32 nof_elements,
    int elem,
    void *extra_argument);

/**
* \brief
*   Prints resource data includes tags and grains
*
*   \param [in] unit -  Relevant unit.
*   \param [in] node_id - Node ID.
*   \param [in] res_tag_bitmap_fast - resource tag bitmap fast to print.
*   \param [in] data - relevant data to be printed.
*  \return
*    \retval Zero if no error was detected
*    \retval Negative if error was detected. See \ref shr_error_e
*  \remark
*    None
*  \see
*    None
*****************************************************/

int resource_tag_bitmap_fast_print(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap_fast,
    dnx_algo_res_dump_data_t * data);

/**
 * \brief
 *   Clear all the elements of the bitmap.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] node_id - Node ID.
 *   \param [in] res_tag_bitmap_fast - resource tag bitmap fast to be cleared.
 *   \return
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   \remark
 *      None
 *   \see
 *       dnx_algo_res_simple_bitmap_clear
*****************************************************/
shr_error_e resource_tag_bitmap_fast_clear(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap_fast);

/*
* \brief
*    The function is setting data value at element index.
*    \param [in] unit - Relevant unit.
*    \param [in] node_id - Node ID.
*    \param [in] res_tag_bitmap_fast - resource tag bitmap fast.
*    \param [in] element - Element to be checked.
*    \param [in] data - Pointer to be with the data that should be set.
*  \return
*    shr_error_e - Error return value
*  \remark
*    None
*/
shr_error_e resource_tag_bitmap_fast_data_per_entry_set(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap_fast,
    int element,
    const void *data);

/*
* \brief
*    The function is returning data value at element index.
*    \param [in] unit - Relevant unit.
*    \param [in] node_id - Node ID.
*    \param [in] res_tag_bitmap_fast - resource tag bitmap fast.
*    \param [in] element - Element to be checked.
*    \param [in] data - Pointer to be with the data that is set.
*  \return
*    shr_error_e - Error return value
*  \remark
*    None
*/
shr_error_e resource_tag_bitmap_fast_data_per_entry_get(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap_fast,
    int element,
    void *data);

/*
 * } RESOURCE_TAG_BITMAP_FAST_H_INCLUDED
 */
#endif
