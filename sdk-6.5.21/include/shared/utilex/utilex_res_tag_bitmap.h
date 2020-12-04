/** \file utilex_res_tag_bitmap.h
 *
 * Indexed resource management -- tagged bitmap
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _UTILEX_RES_TAG_BITMAP_H_
#define _UTILEX_RES_TAG_BITMAP_H_

#include <sal/types.h>
#include <shared/bitop.h>
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/swstate/auto_generated/types/utilex_res_tag_bitmap_types.h>

/**
 *  Provide WITH_ID when allocating a block and you want to specify the initial
 *  element of that block.
 *
 *  Provide ALIGN_ZERO when allocating an aligned block and you want that block
 *  to be aligned against zero rather than against the low_id value used when
 *  creating the resource.
 *
 *  Provide REPLACE when allocating WITH_ID to indicate you want to replace an
 *  existing block.  Note this requires that the existing block be there in its
 *  entirety; it is an error to try to change the size of a block this way.
 */
#define UTILEX_RES_TAG_BITMAP_ALLOC_WITH_ID              0x00000001
#define UTILEX_RES_TAG_BITMAP_ALLOC_ALIGN_ZERO           0x00000002
#define UTILEX_RES_TAG_BITMAP_ALWAYS_CHECK_TAG           0x00000008
#define UTILEX_RES_TAG_BITMAP_ALLOC_CHECK_ONLY           0x00000010

/**
 * \brief - Initialize the resource tag bitmap module.
 * 
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] nof_bitmaps - How many bitmaps will be used in this device's run.
 *   
 * \return
 *   int 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e utilex_res_tag_bitmap_init(
    int unit,
    int nof_bitmaps);

/**
 * \brief - Free the resource tag bitmap module.
 * 
 *      This will destroy all of the resource pools, then tear down the rest of
 *      the resource management for the unit.
 *  
 * \param [in] unit - ID of the device to be used with this bitmap.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e utilex_res_tag_bitmap_deinit(
    int unit);

/**
 * \brief - Create a tagged bitmap resource.
 * 
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [out] handle - Pointer to place the handle for this bitmap.
 * \param [in] low_id - Minimum valid element ID.
 * \param [in] count - Total number of elements.
 * \param [in] grain_size - Number of elements per grain.
 * \param [in] max_tag_value - Maximum tag value.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   There are count / grain_size grains, and each one has a tag associated.
 * \see
 *   * None
 */
shr_error_e utilex_res_tag_bitmap_create(
    int unit,
    utilex_res_tag_bitmap_handle_t * handle,
    int low_id,
    int count,
    int grain_size,
    uint32 max_tag_value);

/**
 * \brief - Destroy a tagged bitmap resource
 * 
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] handle - Handle for the resource to destroy.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e utilex_res_tag_bitmap_destroy(
    int unit,
    utilex_res_tag_bitmap_handle_t handle);

/*
 *  Ensure that all grains in an alloc are tagged accordingly.
 */

/**
 * \brief - Force set a tag to a range of elements..
 * 
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] handle - Handle for the resource instance to access.
 * \param [in] tag - Tag value to use.
 * \param [in] elemIndex - Start of the range to set.
 * \param [in] elemCount - How many elements in the range.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e utilex_res_tag_bitmap_tag_set(
    int unit,
    utilex_res_tag_bitmap_handle_t handle,
    uint32 tag,
    int elemIndex,
    int elemCount);

/**
 * \brief - Get the tag for a specific element.
 * 
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] handle - Handle for the resource instance to access.
 * \param [in] elemIndex - Element index to be checked.
 * \param [out] tag - Tag configured for this element.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e utilex_res_tag_bitmap_tag_get(
    int unit,
    utilex_res_tag_bitmap_handle_t handle,
    int elemIndex,
    uint32 *tag);

/**
 * \brief - Allocate an element or block of elements of a particular resource
 * 
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] handle - Handle for the resource instance to access.
 * \param [in] flags - Flags providing specifics of what/how to allocate
 * \param [in] count - Number of elements to allocate in this block.
 * \param [in,out] elem - Allocated element.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *      The elem argument is IN if the WITH_ID flag is specified; it is OUT if
 *      the WITH_ID flag is not specified.
 *
 *      This will allocate a single block of the requested number of elements
 *      of this resource (each of which may be a number of elements taken from
 *      the underlying pool).
 *
 *      The caller must track how many elements were requested and provide that
 *      number when freeing the block.
 *
 *      Note the tag used will be zero for blocks allocated with this
 *      call (since there is no tag specified here).
 * \see
 *   * None
 */
shr_error_e utilex_res_tag_bitmap_alloc(
    int unit,
    utilex_res_tag_bitmap_handle_t handle,
    uint32 flags,
    int count,
    int *elem);

/**
 * \brief - Allocate an element or block of elements of a particular resource,
 *      assuring all of the elements have the same tag.
 * 
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] handle - Handle for the resource instance to access.
 * \param [in] flags - Flags providing specifics of what/how to allocate.
 * \param [in] tag - Tag value to use.
 * \param [in] count - Number of elements to allocate in this block.
 * \param [in,out] elem - Allocated element.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *      As \ref utilex_res_tag_bitmap_alloc , except the tag is provided
 *      rather than assumed to be 0.
 *
 * \see
 *   utilex_res_tag_bitmap_alloc
 */
shr_error_e utilex_res_tag_bitmap_alloc_tag(
    int unit,
    utilex_res_tag_bitmap_handle_t handle,
    uint32 flags,
    uint32 tag,
    int count,
    int *elem);

/**
 * \brief -  Allocate an element or block of elements of a particular resource,
 *      using a base alignment and an offset.
 * 
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] handle - Handle for the resource instance to access.
 * \param [in] flags - Flags providing specifics of what/how to allocate.
 * \param [in] align - Base alignment.
 * \param [in] offs - Offest from base alignment for first element.
 * \param [in] count - Number of elements to allocate in this block.
 * \param [in,out] elem - Allocated element.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 * 
 *   As \ref utilex_res_tag_bitmap_alloc except the first element of the
 *      returned block will be at ((n * align) + offset), where n is some
 *      integer.  If it is not possible to allocate a block with the requested
 *      constraints, the call will fail.  Note that the alignment is within
 *      the specified range of the resource, and not specifically aligned against
 *      the absolute value zero; to request the alignment be against zero,
 *      specify the ALIGN_ZERO flag.
 *
 *      If offset >= align, BCM_E_PARAM.  If align is zero or negative, it will
 *      be treated as if it were 1.
 * 
 *      If WITH_ID is specified, and the requested base element does not
 *      comply with the indicated alignment, BCM_E_PARAM will be returned.
 *  
 * \see
 *   utilex_res_tag_bitmap_alloc
 */
shr_error_e utilex_res_tag_bitmap_alloc_align(
    int unit,
    utilex_res_tag_bitmap_handle_t handle,
    uint32 flags,
    int align,
    int offs,
    int count,
    int *elem);

/**
 * \brief - Allocate an element or block of elements of a particular resource,
 *      using a base alignment and an offset, and assuring the elements all
 *      have the same tag.
 * 
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] handle - Handle for the resource instance to access.
 * \param [in] flags - Flags providing specifics of what/how to allocate.
 * \param [in] align - Base alignment.
 * \param [in] offs - Offest from base alignment for first element.
 * \param [in] tag - Tag value to use.
 * \param [in] count - Number of elements to allocate in this block.
 * \param [in,out] elem - Allocated element.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   As \ref utilex_res_tag_bitmap_alloc_align, but tag is provided rather than assumed to be 0.
 * \see
 *   utilex_res_tag_bitmap_alloc_align
 */
shr_error_e utilex_res_tag_bitmap_alloc_align_tag(
    int unit,
    utilex_res_tag_bitmap_handle_t handle,
    uint32 flags,
    int align,
    int offs,
    uint32 tag,
    int count,
    int *elem);

/**
 * \brief
 *   Allocate several non consecutive elements.
 *
 *
 *   \param [in] unit -
 *       Relevant unit.
 *   \param [in] handle -
 *       Handle for the instance to access
 *   \param [in] flags -
 *       Flags providing specifics of what/how to allocate.
 *   \param [in] align -
 *       Base alignment.
 *   \param [in] offs -
 *       Offest from base alignment for first element.
 *   \param [in] tag -
 *       Tag value to use.
 *   \param [in] pattern -
 *       Bitmapped pattern of elements.
 *   \param [in] length -
 *       Length of pattern.
 *   \param [in] repeats -
 *       Number of iterations of pattern.
 *   \param [in,out] elem -
 *       Pointer to place the allocated element.
 *       \b As \b output - \n
 *       Holds the first *allocated* element in the pattern.
 *       \b As \b input - \n
 *       If flag \ref DNX_ALGO_RES_ALLOCATE_WITH_ID is set, pass the first element to be *allocated* here.
 *   \return
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   \remark
 *      This works as utilex_res_tag_bitmap_alloc_align_tag, except it also
 *      allocates according to a pattern.
 *
 *      The pattern argument is a bitmap of the elements that are of interest
 *      in a single iteration of the pattern (and only the least significant
 *      'length' bits are used; higher bits are ignored).  The bit with value
 *      (1 << k) set indicates the element at (elem + k) must be in the block;
 *      clear it indicates the element at (elem + k) is not in the block.  This
 *      repeats for as many iterations as indicated by 'repeats'.
 *
 *      The caller must track the pattern, length and repeats values and
 *      provide these values along with the elem value when freeing the block.
 *
 *      Any allocation made through this function must be freed using the
 *      utilex_res_bitmap_free_sparse function.
 *   \see
 *       utilex_res_tag_bitmap_alloc_align_tag
 */
shr_error_e utilex_res_tag_bitmap_alloc_align_tag_sparse(
    int unit,
    utilex_res_tag_bitmap_handle_t handle,
    uint32 flags,
    int align,
    int offs,
    uint32 tag,
    uint32 pattern,
    int length,
    int repeats,
    int *elem);

/**
 * \brief - Free an element or block of elements of a particular resource.
 * 
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] handle - Handle for the resource instance to access.
 * \param [in] count - Number of elements to freed in this block.
 * \param [in] elem - The element to free (or base of the block to free).
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *      This will free a single block of the requested number of elements,
 *      starting at the specified element.
 *  
 *      This should only be called with valid data (base element and element
 *      count) against known allocated blocks.  Trying to free a block that is
 *      not in use or trying to free something that spans multiple allocated
 *      blocks may not work.
 * \see
 *   * None
 */
shr_error_e utilex_res_tag_bitmap_free(
    int unit,
    utilex_res_tag_bitmap_handle_t handle,
    int count,
    int elem);

/**
 * \brief
 *   Free an element or block of elements of a particular
 *   resource
 *
 *
 *   \param [in] unit -
 *       Relevant unit.
 *   \param [in] handle -
 *       Handle for the instance to access
 *   \param [in] pattern -
 *       Bitmapped pattern of elements.
 *   \param [in] length -
 *       Length of pattern.
 *   \param [in] repeats -
 *       Number of iterations of pattern.
 *   \param [in] elem -
 *      The element to free.
 *   \return
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   \remark
 *      This will free a sparse block of elements, starting
 *      at the specified element and proceeding as specified.
 *
 *      This should only be called with valid data (base element, pattern,
 *      length, repeats).  Trying to free a block that is not in use or trying
 *      to free something that spans multiple allocated blocks may not work.
 *
 *      Anything allocated with shr_res_bitmap_alloc_align_sparse must be freed
 *      using this function.  While it is possible to use this function to free
 *      blocks allocated with other functions, it is not advised.
 *   \see
 *       utilex_res_tag_bitmap_free
 */
shr_error_e utilex_res_tag_bitmap_free_sparse(
    int unit,
    utilex_res_tag_bitmap_handle_t handle,
    uint32 pattern,
    int length,
    int repeats,
    int elem);

/**
 * \brief
 *   Clear all elements of a particular resource
 *
 *
 *   \param [in] unit -
 *       Relevant unit.
 *   \param [in] handle -
 *       Handle for the instance to access
 *   \return
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   \remark
 *      None
 *   \see
 *       None
 */
shr_error_e utilex_res_tag_bitmap_clear(
    int unit,
    utilex_res_tag_bitmap_handle_t handle);

/**
 * \brief - Check the status of a specific element.
 * 
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] handle - Handle for the resource instance to access.
 * \param [in] count - Number of elements to check in this block.
 * \param [in] elem - The element to check (or base of the block to check).
 *   
 * \return
 *   shr_error_e
 *    _SHR_E_NOT_FOUND if the element is not in use.
 *    _SHR_E_EXISTS if the element is in use.
 *    _SHR_E_PARAM if the element is not valid.
 *    _SHR_E_* as appropriate otherwise.
 *   
 * \remark
 *      This will check whether the requested block of the resource is
 *      allocated.  Note that if any element of the resource in the range of
 *      [elem..(elem+count-1)] (inclusive) is not free, it returns
 *      BCM_E_EXISTS; it will only return BCM_E_NOT_FOUND if all elements
 *      within the specified block are free.
 *
 *      Normally this should be called to check on a specific block (one that
 *      is thought to exist or in preparation for allocating it WITH_ID).
 * \see
 *   * None
 */
shr_error_e utilex_res_tag_bitmap_check(
    int unit,
    utilex_res_tag_bitmap_handle_t handle,
    int count,
    int elem);

/**
 * \brief - Check the status of a block of elements.
 * 
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] handle - Handle for the resource instance to access.
 * \param [in] tag - Value of the tag to check.
 * \param [in] count - Number of elements to check in this block.
 * \param [in] elem - The element to check (or base of the block to check).
 *   
 * \return
 *   shr_error_e 
 *      _SHR_E_EMPTY if none of the elements are in use.
 *      _SHR_E_FULL if all of the elements are in use.
 *      _SHR_E_CONFIG if elements are in use but block(s) do not match the tag.
 *      _SHR_E_EXISTS if some of the elements are in use but not all of them.
 *      _SHR_E_PARAM if any of the elements is not valid.
 *      _SHR_E_* as appropriate otherwise.
 * \remark 
 *      As \ref utilex_res_tag_bitmap_check except this:
 *  
 *      WANRING: The tagged bitmap allocator does not track blocks internally
 *      and so it is possible that if there are two adjacent blocks with
 *      identical tags both allocated and this is called to check whether safe
 *      to 'reallocate', will falsely indicate that it can be done.  However,
 *      this will consider different tags to indicate different blocks, so will
 *      not assert false true for the case if the adjacent blocks have
 *      different tags, unless the block size is smaller than the tag size, in
 *      which case it still could falsely claim the operation is valid. Also,
 *      'reallocate' in a similar manner of a large block to a smaller block
 *      could leak underlying resources.
 * \see
 *   utilex_res_tag_bitmap_check
 */
shr_error_e utilex_res_tag_bitmap_check_all_tag(
    int unit,
    utilex_res_tag_bitmap_handle_t handle,
    uint32 tag,
    int count,
    int elem);

/**
 * \brief - Check the status of a block of elements.
 * 
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] handle - Handle for the resource instance to access.
 * \param [in] count - Number of elements to check in this block.
 * \param [in] elem - The element to check (or base of the block to check).
 *   
 * \return
 *   shr_error_e 
 *      _SHR_E_EMPTY if none of the elements are in use.
 *      _SHR_E_FULL if all of the elements are in use.
 *      _SHR_E_CONFIG if elements are in use but block(s) do not match the tag.
 *      _SHR_E_EXISTS if some of the elements are in use but not all of them.
 *      _SHR_E_PARAM if any of the elements is not valid.
 *      _SHR_E_* as appropriate otherwise.
 * \remark 
 *      As \ref utilex_res_tag_bitmap_check_all_tag except without explicit tag.
 *
 *      Unlike the other supported operations that work without explicit tag, this
 *      function assumes the first tag in the block is the desired tag.  If you
 *      want the normal default tag behaviour, provide NULL as the tag argument and
 *      call utilex_res_tag_bitmap_check_all_tag instead.
 * \see
 *   utilex_res_tag_bitmap_check
 */
shr_error_e utilex_res_tag_bitmap_check_all(
    int unit,
    utilex_res_tag_bitmap_handle_t handle,
    int count,
    int elem);

/**
 * \brief - Returns the number of free elements for this resource.
 * 
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] handle - Handle for the resource instance to access.
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
shr_error_e utilex_res_tag_bitmap_nof_free_elements_get(
    int unit,
    utilex_res_tag_bitmap_handle_t handle,
    int *nof_free_elements);
#else
/**
 * This is DUMMY code. It is only for compilers that do not accept empty files
 * and is never to be used.
 */
shr_error_e utilex_res_tag_bitmap_dummy_empty_function_to_make_compiler_happy(
    int unit);

#endif

#endif /* ndef _UTILEX_RES_TAG_BITMAP_H_ */
