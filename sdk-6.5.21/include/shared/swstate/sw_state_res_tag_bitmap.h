/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Indexed resource management -- tagged bitmap
 */

#ifndef _SW_STATE_RES_TAG_BITMAP_H_
#define _SW_STATE_RES_TAG_BITMAP_H_

#include <sal/types.h>
#include <shared/bitop.h>
#include <shared/swstate/sw_state.h>


/*
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
#define SW_STATE_RES_TAG_BITMAP_ALLOC_WITH_ID      0x00000001
#define SW_STATE_RES_TAG_BITMAP_ALLOC_ALIGN_ZERO   0x00000002
#define SW_STATE_RES_TAG_BITMAP_ALLOC_REPLACE      0x00000004
#define SW_STATE_RES_TAG_BITMAP_ALWAYS_CHECK_TAG   0x00000008
#define SW_STATE_RES_TAG_BITMAP_ALLOC_CHECK_ONLY   0x00000010

typedef struct _sw_state_res_tag_bitmap_list_s {
    int low;            /* lowest element ID */
    int count;          /* number of elements */
    int used;           /* number of elements in use */
    int grainSize;      /* elements per tag grain */
    int tagSize;        /* bytes per tag */
    int lastFree;       /* first element of last freed block */
    int nextAlloc;      /* next element after last alloc block */
    PARSER_HINT_ARR     uint8 *tagData;     /* pointer to base of tag data (after data) */
    PARSER_HINT_ARR     SHR_BITDCL *data;
} _sw_state_res_tag_bitmap_list_t;

typedef uint32 sw_state_res_tag_bitmap_handle_t;

typedef struct sw_state_res_tag_bitmap_info_s {
                        uint32                              max_nof_bmps;
                        uint32                              nof_in_use;
    PARSER_HINT_ARR     SHR_BITDCL                          *occupied_ids;
    PARSER_HINT_ARR_PTR _sw_state_res_tag_bitmap_list_t     **bitmap_lists;
} sw_state_res_tag_bitmap_info_t;

/*
 *   Function
 *      sw_state_res_tag_bitmap_init
 *   Purpose
 *      Initialize the resource tag bitmap module.
 *   Parameters
 *      (IN) unit       : ID of the device to be used with this bitmap.
 *      (IN) nof_bitmaps: How many bitmaps will be used in this device's run.
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 */
int 
sw_state_res_tag_bitmap_init(int unit, 
                             int nof_bitmaps);


/*
 *   Function
 *      sw_state_res_tag_bitmap_create
 *   Purpose
 *      Create a tagged bitmap resource
 *   Parameters
 *      (OUT) handle    : where to put the handle
 *      (IN) low_id     : minimum valid element ID
 *      (IN) count      : number of elements total
 *      (IN) grain_size : number of elements per grain
 *      (IN) tag_size   : number of bytes in each grain's tag
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      This will destroy all of the resource pools, then tear down the rest of
 *      the resource management for the instance.
 *
 *      There are count / grain_size grains, and each one has a tag associated.
 */
extern int
sw_state_res_tag_bitmap_create(int unit, 
                              sw_state_res_tag_bitmap_handle_t *handle,
                              int low_id,
                              int count,
                              int grain_size,
                              int tag_size);

/*
 *   Function
 *      sw_state_res_tag_bitmap_destroy
 *   Purpose
 *      Destroy a tagged bitmap resource
 *   Parameters
 *      (IN) handle : handle for the instance to access
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 */
extern int
sw_state_res_tag_bitmap_destroy(int unit, 
                                sw_state_res_tag_bitmap_handle_t handle);


/*
 *  Ensure that all grains in an alloc are tagged accordingly.
 */
int
sw_state_res_tag_bitmap_tag_set(int unit,
                                sw_state_res_tag_bitmap_handle_t handle,
                                const void* tag,
                                int elemIndex,
                                int elemCount);
int
sw_state_res_tag_bitmap_tag_get(int unit,
                                sw_state_res_tag_bitmap_handle_t handle,
                                int elemIndex,
                                const void* tag);
/*
 *   Function
 *      sw_state_res_tag_bitmap_alloc
 *   Purpose
 *      Allocate an element or block of elements of a particular resource
 *   Parameters
 *      (IN) handle   : handle for the instance to access
 *      (IN) flags    : flags providing specifics of what/how to allocate
 *      (IN) count    : elements to allocate in this block
 *      (IN/OUT) elem : where to put the allocated element (block base)
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
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
 *      Note the tag used will be all zero bytes for blocks allocated with this
 *      call (since there is no tag specified here).
 */
extern int
sw_state_res_tag_bitmap_alloc(int unit, 
                             sw_state_res_tag_bitmap_handle_t handle,
                             uint32 flags,
                             int count,
                             int *elem);

/*
 *   Function
 *      sw_state_res_tag_bitmap_alloc_tag
 *   Purpose
 *      Allocate an element or block of elements of a particular resource,
 *      assuring all of the elements have the same tag.
 *   Parameters
 *      (IN) handle   : handle for the instance to access
 *      (IN) flags    : flags providing specifics of what/how to allocate
 *      (IN) tag      : pointer to tag value to use
 *      (IN) count    : elements to allocate in this block
 *      (IN/OUT) elem : where to put the allocated element (block base)
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      The elem argument is IN if the WITH_ID flag is specified; it is OUT if
 *      the WITH_ID flag is not specified.
 *
 *      The tag pointer is a pointer to the value that will be used for tagging
 *      the requested block of elements.  Note that since tags are a number of
 *      bytes starting at the pointer, the tag must already be masked as needed
 *      and should be stored in something no larger than the number of bytes
 *      that was specified as tag size when setting up the pool.
 *
 *      This will allocate a single block of the requested number of elements
 *      of this resource (each of which may be a number of elements taken from
 *      the underlying pool).
 *
 *      If neither this nor the alloc_tag call is used, the tag will be assumed
 *      to be all zeroes.
 *
 *      Partial blocks will not be allocated.
 *
 *      The caller must track how many elements were requested and provide that
 *      number when freeing the block.
 */
extern int
sw_state_res_tag_bitmap_alloc_tag(int unit, 
                                 sw_state_res_tag_bitmap_handle_t handle,
                                 uint32 flags,
                                 const void *tag,
                                 int count,
                                 int *elem);

/*
 *   Function
 *      sw_state_res_tag_bitmap_alloc_align
 *   Purpose
 *      Allocate an element or block of elements of a particular resource,
 *      using a base alignment and an offset.
 *   Parameters
 *      (IN) handle   : handle for the instance to access
 *      (IN) flags    : flags providing specifics of what/how to allocate
 *      (IN) align    : base alignment
 *      (IN) offset   : offest from base alignment for first element
 *      (IN) count    : elements to allocate in this block
 *      (IN/OUT) elem : where to put the allocated element (block base)
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      The elem argument is IN if the WITH_ID flag is specified; it is OUT if
 *      the WITH_ID flag is not specified.  If WITH_ID is specified, and the
 *      requested base element does not comply with the indicated alignment,
 *      BCM_E_PARAM will be returned.
 *
 *      This will allocate a single block of the requested number of elements
 *      of this resource (each of which may be a number of elements taken from
 *      the underlying pool).
 *
 *      The first element of the returned block will be at ((n * align) +
 *      offset), where n is some integer.  If it is not possible to allocate a
 *      block with the requested constraints, the call will fail.  Note that
 *      the alignment is within the specified range of the resource, and not
 *      specifically aligned against the absolute value zero; to request the
 *      alignment be against zero, specify the ALIGN_ZERO flag.
 *
 *      If offset >= align, BCM_E_PARAM.  If align is zero or negative, it will
 *      be treated as if it were 1.
 *
 *      Partial blocks will not be allocated.
 *
 *      The caller must track how many elements were requested and provide that
 *      number when freeing the block.
 *
 *      Note the tag used will be all zero bytes for blocks allocated with this
 *      call (since there is no tag specified here).
 */
extern int
sw_state_res_tag_bitmap_alloc_align(int unit, 
                                   sw_state_res_tag_bitmap_handle_t handle,
                                   uint32 flags,
                                   int align,
                                   int offs,
                                   int count,
                                   int *elem);

/*
 *   Function
 *      sw_state_res_tag_bitmap_alloc_align_tag
 *   Purpose
 *      Allocate an element or block of elements of a particular resource,
 *      using a base alignment and an offset, and assuring the elements all
 *      have the same tag.
 *   Parameters
 *      (IN) handle   : handle for the instance to access
 *      (IN) flags    : flags providing specifics of what/how to allocate
 *      (IN) align    : base alignment
 *      (IN) offset   : offest from base alignment for first element
 *      (IN) tag      : pointer to tag value to use
 *      (IN) count    : elements to allocate in this block
 *      (IN/OUT) elem : where to put the allocated element (block base)
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      The elem argument is IN if the WITH_ID flag is specified; it is OUT if
 *      the WITH_ID flag is not specified.  If WITH_ID is specified, and the
 *      requested base element does not comply with the indicated alignment,
 *      BCM_E_PARAM will be returned.
 *
 *      The tag pointer is a pointer to the value that will be used for tagging
 *      the requested block of elements.  Note that since tags are a number of
 *      bytes starting at the pointer, the tag must already be masked as needed
 *      and should be stored in something no larger than the number of bytes
 *      that was specified as tag size when setting up the pool.
 *
 *      This will allocate a single block of the requested number of elements
 *      of this resource (each of which may be a number of elements taken from
 *      the underlying pool), and ensuring all have the same tag (elements can
 *      be within the same grain as elements from other blocks only if the tag
 *      of the partial grain is equal to the tag for the new elements).
 *
 *      The first element of the returned block will be at ((n * align) +
 *      offset), where n is some integer.  If it is not possible to allocate a
 *      block with the requested constraints, the call will fail.  Note that
 *      the alignment is within the specified range of the resource, and not
 *      specifically aligned against the absolute value zero; to request the
 *      alignment be against zero, specify the ALIGN_ZERO flag.
 *
 *      If offset >= align, BCM_E_PARAM.  If align is zero or negative, it will
 *      be treated as if it were 1.
 *
 *      Partial blocks will not be allocated.
 *
 *      The caller must track how many elements were requested and provide that
 *      number when freeing the block.
 */
extern int
sw_state_res_tag_bitmap_alloc_align_tag(int unit, 
                                       sw_state_res_tag_bitmap_handle_t handle,
                                       uint32 flags,
                                       int align,
                                       int offs,
                                       const void *tag,
                                       int count,
                                       int *elem);

/*
 *   Function
 *      sw_state_res_tag_bitmap_free
 *   Purpose
 *      Free an element or block of elements of a particular resource
 *   Parameters
 *      (IN) handle : handle for the instance to access
 *      (IN) count  : elements in the block to free
 *      (IN) elem   : the element to free (or base of the block to free)
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      This will free a single block of the requested number of elements,
 *      starting at the specified element.  Some of the allocators do not deal
 *      with blocks and so must be told how large a block was, so it is
 *      obligatory that the caller be able to provide such information.
 *
 *      This should only be called with valid data (base element and element
 *      count) against known allocated blocks.  Trying to free a block that is
 *      not in use or trying to free something that spans multiple allocated
 *      blocks may not work.
 */
extern int
sw_state_res_tag_bitmap_free(int unit, 
                            sw_state_res_tag_bitmap_handle_t handle,
                            int count,
                            int elem);

/*
 *   Function
 *      sw_state_res_tag_bitmap_check
 *   Purpose
 *      Check the status of a specific element
 *   Parameters
 *      (IN) handle : handle for the instance to access
 *      (IN) count  : elements in the block to check
 *      (IN) elem   : the element to check (or base of the block to check)
 *   Returns
 *      BCM_E_NOT_FOUND if the element is not in use
 *      BCM_E_EXISTS if the element is in use
 *      BCM_E_PARAM if the element is not valid
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      This will check whether the requested block of the resource is
 *      allocated.  Note that if any element of the resource in the range of
 *      [elem..(elem+count-1)] (inclusive) is not free, it returns
 *      BCM_E_EXISTS; it will only return BCM_E_NOT_FOUND if all elements
 *      within the specified block are free.
 *
 *      Normally this should be called to check on a specific block (one that
 *      is thought to exist or in preparation for allocating it WITH_ID).
 */
extern int
sw_state_res_tag_bitmap_check(int unit, 
                             sw_state_res_tag_bitmap_handle_t handle,
                             int count,
                             int elem);

/*
 *   Function
 *      sw_state_res_tag_bitmap_check_all_tag
 *   Purpose
 *      Check the status of a block of elements
 *   Parameters
 *      (IN) handle : handle for the instance to access
 *      (IN) tag    : the tag for comparison
 *      (IN) count  : elements in the block to check
 *      (IN) elem   : the element to check (or base of the block to check)
 *   Returns
 *      BCM_E_EMPTY if none of the elements are in use
 *      BCM_E_FULL if all of the elements are in use
 *      BCM_E_CONFIG if elements are in use but block(s) do not match
 *      BCM_E_EXISTS if some of the elements are in use but not all of them
 *      BCM_E_PARAM if any of the elements is not valid
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      This will check whether the requested block of the resource is
 *      allocated.  Note that if any element of the resource in the range of
 *      [elem..(elem+count-1)] (inclusive) is not free, it returns
 *      BCM_E_EXISTS; it will only return BCM_E_NOT_FOUND if all elements
 *      within the specified block are free.
 *
 *      Normally this should be called to check on a specific block (one that
 *      is thought to exist or in preparation for allocating it WITH_ID).
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
 */
extern int
sw_state_res_tag_bitmap_check_all_tag(int unit, 
                                     sw_state_res_tag_bitmap_handle_t handle,
                                     const void *tag,
                                     int count,
                                     int elem);

/*
 *  Same as sw_state_res_tag_bitmap_check_all_tag except without explicit tag.
 *
 *  Unlike the other supported operations that work without explicit tag, this
 *  function assumes the first tag in the block is the desired tag.  If you
 *  want the normal default tag behaviour, provide NULL as the tag argument and
 *  call sw_state_res_tag_bitmap_check_all_tag instead.
 */
extern int
sw_state_res_tag_bitmap_check_all(int unit, 
                                 sw_state_res_tag_bitmap_handle_t handle,
                                 int count,
                                 int elem);
/*
 *   Function
 *      sw_state_res_tag_bitmap_nof_free_elements_get
 *   Purpose
 *      Returns the number of free elemnts for this resource.
 *   Parameters
 *      (IN) unit               : ID of the device to be used with this bitmap.
 *      (IN) handle             : handle for instance to dump
 *      (OUT)nof_free_elements  : Int pointer to memory to load output into.
 *   Returns
 *      BCM_E_NONE  success
 *      BCM_E_* otherwise as appropriate
 *   Notes
 */
int
sw_state_res_tag_bitmap_nof_free_elements_get(int unit,
                                              sw_state_res_tag_bitmap_handle_t handle,
                                              int *nof_free_elements);

/*
 *   Function
 *      sw_state_res_tag_bitmap_dump
 *   Purpose
 *      Dump the internal state of a tagged bitmap allocator
 *   Parameters
 *      (IN) handle : handle for instance to dump
 *   Returns
 *      BCM_E_PARAM if handle is clearly bogus
 *      BCM_E_INTERNAL if there is obvious corruption
 *      BCM_E_NONE usually for successful dump
 *      BCM_E_* otherwise as appropriate
 *   Notes
 *      There is very little that can be verified for corruption in the tagged
 *      bitmap allocator, particularly if the search resume feature is off, and
 *      even if it is on, still rather little.
 */
extern int
sw_state_res_tag_bitmap_dump(int unit, 
                             const sw_state_res_tag_bitmap_handle_t handle);

#endif /* ndef _SW_STATE_RES_TAG_BITMAP_H_ */

