/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Indexed resource management -- simple bitmap
 */

#ifndef _SHR_RES_BITMAP_H_
#define _SHR_RES_BITMAP_H_

#include <sal/types.h>
#include <shared/bitop.h>

/*
 *  This is a fairly brute-force implementation of bitmap, with minimal
 *  optimisations or improvements.  It could probably be enhanced somewhat by
 *  some subtleties, such as checking whether a SHR_BITDCL is all ones before
 *  scanning individual bits when looking for free space.
 */



/*
 *  This controls certain optimisations that try to more quickly find available
 *  blocks.  These optimisations tend to improve allocation performance in many
 *  cases, but they also tend to reduce resource packing efficiency.
 *
 *  SHR_RES_BITMAP_SEARCH_RESUME: If TRUE, this module will track the first
 *  element of the last freed block and the next element after the last
 *  successful allocation, first trying a new alloc in the place of the last
 *  free, then starting its exhaustive search for available elements after the
 *  last successful alloc, wrapping around if needed.  If FALSE, this module
 *  will not check the last freed location for suitability and will always
 *  start the exhaustive search from the low element.
 */
#define SHR_RES_BITMAP_SEARCH_RESUME TRUE

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
#define SHR_RES_BITMAP_ALLOC_WITH_ID      0x00000001
#define SHR_RES_BITMAP_ALLOC_ALIGN_ZERO   0x00000002
#define SHR_RES_BITMAP_ALLOC_REPLACE      0x00000004

typedef struct _shr_res_bitmap_list_s {
    int low;
    int count;
    int used;
#if SHR_RES_BITMAP_SEARCH_RESUME
    int lastFree;
    int nextAlloc;
#endif /* SHR_RES_BITMAP_SEARCH_RESUME */
    SHR_BITDCL data[1];
} _shr_res_bitmap_list_t;

typedef _shr_res_bitmap_list_t *shr_res_bitmap_handle_t;

/*
 *   Function
 *      shr_res_bitmap_create
 *   Purpose
 *      Create a tagged bitmap resource
 *   Parameters
 *      (OUT) handle    : where to put the handle
 *      (IN) low_id     : minimum valid element ID
 *      (IN) count      : number of elements total
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 */
extern int
shr_res_bitmap_create(shr_res_bitmap_handle_t *handle,
                      int low_id,
                      int count);

/*
 *   Function
 *      shr_res_tag_bitmap_destroy
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
shr_res_bitmap_destroy(shr_res_bitmap_handle_t handle);

/*
 *   Function
 *      shr_res_bitmap_alloc
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
 *      of this resource.
 *
 *      Partial blocks will not be allocated.
 *
 *      The caller must track how many elements were requested and provide that
 *      number when freeing the block.
 */
extern int
shr_res_bitmap_alloc(shr_res_bitmap_handle_t handle,
                     uint32 flags,
                     int count,
                     int *elem);

/*
 *   Function
 *      shr_res_bitmap_alloc_align
 *   Purpose
 *      Allocate an element or block of elements of a particular resource,
 *      using a base alignment and an offset.
 *   Parameters
 *      (IN) handle   : handle for the instance to access
 *      (IN) flags    : flags providing specifics of what/how to allocate
 *      (IN) align    : base alignment
 *      (IN) offs     : offest from base alignment for first element
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
 *      of this resource.
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
shr_res_bitmap_alloc_align(shr_res_bitmap_handle_t handle,
                           uint32 flags,
                           int align,
                           int offs,
                           int count,
                           int *elem);

/*
 *   Function
 *      shr_res_bitmap_alloc_align_sparse
 *   Purpose
 *      Allocate an element or sparse block of elements of a particular
 *      resource, using a base alignment, an offset, a pattern.
 *   Parameters
 *      (IN) handle   : handle for the instance to access
 *      (IN) flags    : flags providing specifics of what/how to allocate
 *      (IN) align    : base alignment
 *      (IN) offs     : offest from base alignment for first element
 *      (IN) pattern  : bitmapped pattern of elements
 *      (IN) length   : length of pattern
 *      (IN) repeats  : number of iterations of pattern
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
 *      of this resource.
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
 *      The pattern argument is a bitmap of the elements that are of interest
 *      in a single iteration of the pattern (and only the least significant
 *      'length' bits are used; higher bits are ignored).  The bit with value
 *      (1 << k) set indicates the element at (elem + k) must be in the block;
 *      clear it indicates the element at (elem + k) is not in the block.  This
 *      repeats for as many iterations as indicated by 'repeats'.
 *
 *      For example:
 *        align = 4, offs = 0, pattern = 0x7, length = 8, repeats = 2 would
 *        request a block of three elements, a gap of one element, and then
 *        another block of three elements, with the first allocated element
 *        aligned to a multiple of four.
 *
 *        align = 8, offs = 1, pattern = 0x1, length = 2, repeats = 4 would
 *        request four oddly numbered elements with the first one allocated at
 *        (8 * n) + 1 where n is some arbitrary number.
 *
 *      Note the bitmap is considered based at the requested alignment+offset,
 *      even if the least significant bit is not set, so it is probably
 *      simplest to ensure the LSb of pattern is set.
 *
 *      For example:
 *        align = 4, offs = 0, pattern = 0xA, length = 4, repeats = 1 would
 *        request two elements, but the returned base element number would be
 *        (4 * n), while the actually allocated elements would be (4 * n) + 1
 *        and (4 * n) + 3.
 *
 *      The caller must track the pattern, length and repeats values and
 *      provide these values along with the elem value when freeing the block.
 *
 *      Any allocation made through this function must be freed using the
 *      shr_res_bitmap_free_sparse function.
 */
extern int
shr_res_bitmap_alloc_align_sparse(shr_res_bitmap_handle_t handle,
                                  uint32 flags,
                                  int align,
                                  int offs,
                                  uint32 pattern,
                                  int length,
                                  int repeats,
                                  int *elem);

/*
 *   Function
 *      shr_res_bitmap_free
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
 *      starting at the specified element.
 *
 *      This should only be called with valid data (base element and element
 *      count) against known allocated blocks.  Trying to free a block that is
 *      not in use or trying to free something that spans multiple allocated
 *      blocks may not work.
 *
 *      This function must not be used to free any allocation made through the
 *      shr_res_bitmap_alloc_align_sparse function.
 */
extern int
shr_res_bitmap_free(shr_res_bitmap_handle_t handle,
                    int count,
                    int elem);

/*
 *   Function
 *      shr_res_bitmap_free_sparse
 *   Purpose
 *      Free a sparse block of elements of a particular resource
 *   Parameters
 *      (IN) handle : handle for the instance to access
 *      (IN) pattern: pattern of elements
 *      (IN) length : length of pattern
 *      (IN) repeats: number of iterations of the pattern
 *      (IN) elem   : the element to free (or base of the block to free)
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      This will free a sparse block of elements, starting at the specified
 *      element and proceeding as specified.
 *
 *      This should only be called with valid data (base element, pattern,
 *      length, repeats).  Trying to free a block that is not in use or trying
 *      to free something that spans multiple allocated blocks may not work.
 *
 *      Anything allocated with shr_res_bitmap_alloc_align_sparse must be freed
 *      using this function.  While it is possible to use this function to free
 *      blocks allocated with other functions, it is not advised.
 */
extern int
shr_res_bitmap_free_sparse(shr_res_bitmap_handle_t handle,
                           uint32 pattern,
                           int length,
                           int repeats,
                           int elem);

/*
 *   Function
 *      shr_res_bitmap_check
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
 *      is thought to exist or in preparation for allocating it WITH_ID.
 */
extern int
shr_res_bitmap_check(shr_res_bitmap_handle_t handle,
                     int count,
                     int elem);

/*
 *   Function
 *      shr_res_bitmap_check_all
 *   Purpose
 *      Check the status of a specific block of elements
 *   Parameters
 *      (IN) handle : handle for the instance to access
 *      (IN) count  : elements in the block to check
 *      (IN) elem   : the base of the block to check
 *   Returns
 *      BCM_E_EMPTY if none of the elements are in use
 *      BCM_E_FULL if all of the elements are in use
 *      BCM_E_CONFIG if elements are in use but block(s) do not match
 *      BCM_E_EXISTS if some of the elements are in use but not all of them
 *      BCM_E_PARAM if any of the elements is not valid
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      This will check whether the requested block of the resource is
 *      allocated.  This returns BCM_E_FULL if the entire specified block is
 *      allocated, BCM_E_EMPTY if no element in the entire specified block is
 *      allocated, but if some elements are allocated and some not, it will
 *      return BCM_E_EXISTS.
 *
 *      Normally this should be called to check on a specific block (one that
 *      is thought to exist or in preparation for allocating it WITH_ID.
 *
 *      WARNING: The bitmap allocator does not track blocks internally and so
 *      it is possible that if there are two adjacent blocks both allocated and
 *      this is called to check whether safe to 'reallocate', will falsely
 *      indicate that it can be done.  Also, 'reallocate' in a similar manner
 *      of a large block to a smaller one could leak underlying resources.
 */
extern int
shr_res_bitmap_check_all(shr_res_bitmap_handle_t handle,
                         int count,
                         int elem);

/*
 *   Function
 *      shr_res_bitmap_check_all_sparse
 *   Purpose
 *      Check the status of a specific sparse block of elements
 *   Parameters
 *      (IN) handle : handle for the instance to access
 *      (IN) pattern: pattern of elements to check
 *      (IN) length : length of pattern
 *      (IN) repeats: number of time pattern repeats
 *      (IN) elem   : the base of the first pattern
 *   Returns
 *      BCM_E_EMPTY if none of the elements are in use
 *      BCM_E_FULL if all of the elements are in use
 *      BCM_E_CONFIG if elements are in use but block(s) do not match
 *      BCM_E_EXISTS if some of the elements are in use but not all of them
 *      BCM_E_PARAM if any of the elements is not valid
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      This will check whether the requested sparse block of the resource is
 *      allocated.  If all of the specified elements are allocated, it will
 *      return BCM_E_FULL; if all of the specified elements are free, it will
 *      return BCM_E_EMPTY; if some of the specified elements are allocated and
 *      some are free, it will return BCM_E_EXISTS.
 *
 *      See shr_res_bitmap_sparse_alloc_align_sparse for information about
 *      how sparse patterns are specified.
 *
 *      Normally this should be called to check on a specific block (one that
 *      is thought to exist or in preparation for allocating it WITH_ID.
 *
 *      WARNING: The bitmap allocator does not track blocks internally and so
 *      it is possible that if there are two adjacent blocks both allocated and
 *      this is called to check whether safe to 'reallocate', will falsely
 *      indicate that it can be done.  Also, 'reallocate' in a similar manner
 *      of a large block to a smaller one could leak underlying resources.
 */
extern int
shr_res_bitmap_check_all_sparse(shr_res_bitmap_handle_t handle,
                                uint32 pattern,
                                int length,
                                int repeats,
                                int elem);

/*
 *   Function
 *      shr_res_bitmap_dump
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
shr_res_bitmap_dump(const shr_res_bitmap_handle_t handle);

#endif /* ndef _SHR_RES_BITMAP_H_ */

