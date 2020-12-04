/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Module: Indexed resource management, using banked lists
 */
#ifndef _SHR_IDXRES_AFL_
#define _SHR_IDXRES_AFL_

#include <sal/types.h>

typedef uint32 shr_aidxres_element_t;

struct _shr_aidxres_list_s;

typedef struct _shr_aidxres_list_s *shr_aidxres_list_handle_t;

/*
 *   Function
 *      shr_aidxres_list_create
 *   Purpose
 *      Create an aligned/contiguous_blocked banked free list
 *   Parameters
 *      (out) shr_aidxres_list_handle_t *list = place to put list handle
 *      (in) shr_aidxres_element_t first = number of first entry to manage
 *      (in) shr_aidxres_element_t last = number of last entry to manage
 *      (in) shr_aidxres_element_t validLow = low valid entry value
 *      (in) shr_aidxres_element_t validHigh = high valid entry value
 *      (in) shr_aidxres_element_t block_factor = max block power of two
 *      (in) char *name = name for the list (used for sal_alloc)
 *   Returns
 *      bcm_error_t = BCM_E_NONE if list created successfully
 *                    BCM_E_* as appropriate otherwise
 *   Notes
 *      The validLow and validHigh values are used to specify the valid range
 *      of entries for querying 'free/used' status of an entry; any value not
 *      in this range is considered an invalid argument, but values that are
 *      not between first and last will be permanently 'used' and not allowed
 *      by the free operation nor ever provided by the allocate operation.
 *      The blocking factor is the actual power of two that is to be used when
 *      computing maximum block size.  Blocks will be able to be manipulated up
 *      to 2^blocking_factor, but note that blocking_factor must be less than
 *      or equal to the number of bits used for bank index (so in 8b mode, this
 *      must be 7 or less, in 16b mode it must be 15 or less, and in 32b mode
 *      it must be 31 or less).
 */
extern int
shr_aidxres_list_create(shr_aidxres_list_handle_t *list,
                        shr_aidxres_element_t first,
                        shr_aidxres_element_t last,
                        shr_aidxres_element_t valid_low,
                        shr_aidxres_element_t valid_high,
                        shr_aidxres_element_t block_factor,
                        char *name);

/*
 *   Function
 *      shr_aidxres_list_destroy
 *   Purpose
 *      Destroy a list
 *   Parameters
 *      (in) shr_aidxres_list_handle_t list = the list handle
 *   Returns
 *      bcm_error_t = BCM_E_NONE if list created successfully
 *                    BCM_E_* as appropriate otherwise
 *   Notes
 *      This destroys the list, but does not claim the semaphore first, so the
 *      caller must take care not to destroy the list while it's being used.
 *      It is possible that some OSes will not permit the destruction of a lock
 *      that is in use, so maybe that at least helps.  It is also willing to
 *      destroy the list even if there are still allocated entries.
 */
extern int
shr_aidxres_list_destroy(shr_aidxres_list_handle_t list);

/*
 *   Function
 *      shr_aidxres_list_alloc
 *   Purpose
 *      Allocate the next available single element from a list
 *   Parameters
 *      (in) shr_aidxres_list_handle_t list = list from which to allocate
 *      (out) shr_aidxres_element_t *element = where to put alloced elem num
 *   Returns
 *      bcm_error_t = BCM_E_NONE if element allocated successfully
 *                    BCM_E_* as appropriate otherwise
 *   Notes
 *      As for the idxres list alloc call, this returns *one* element.
 */
extern int
shr_aidxres_list_alloc(shr_aidxres_list_handle_t list,
                       shr_aidxres_element_t *element);

/*
 *   Function
 *      shr_aidxres_list_alloc_set
 *   Purpose
 *      Allocate a set of the next available single elements from a list
 *   Parameters
 *      (in) shr_aidxres_list_handle_t list = list from which to allocate
 *      (in) shr_aidxres_element_t count = number of elements to allocate
 *      (out) shr_aidxres_element_t *elements = ptr to array for alloced elems
 *      (out) shr_aidxres_element_t *done = ptr to number of successful allocs
 *   Returns
 *      bcm_error_t = BCM_E_NONE if element allocated successfully
 *                    BCM_E_* as appropriate otherwise
 *   Notes
 *      This uses the same function as shr_idxres_list_alloc, except that it
 *      verifies that there are enough elements free to fulfill the request
 *      before it tries to allocate any of them.  It is still possible that an
 *      error prevents completion, however, so if the result is not success,
 *      the done value must be verified (and any elements that were done that
 *      can not be used must be freed).
 *      The set is NOT guaranteed to be contiguous.
 *      The set consists of count *one* element blocks.
 */
extern int
shr_aidxres_list_alloc_set(shr_aidxres_list_handle_t list,
                           shr_aidxres_element_t count,
                           shr_aidxres_element_t *elements,
                           shr_aidxres_element_t *done);

/*
 *   Function
 *      shr_aidxres_list_alloc_block
 *   Purpose
 *      Allocate a block (of specified count) of elements from a list
 *   Parameters
 *      (in) shr_aidxres_list_handle_t list = list from which to allocate
 *      (in) shr_aidxres_element_t count = size of block, in elements
 *      (out) shr_aidxres_element_t *element = where to put alloced elem num
 *   Returns
 *      bcm_error_t = BCM_E_NONE if element allocated successfully
 *                    BCM_E_* as appropriate otherwise
 *   Notes
 *      The block is guaranteed to be aligned to the next power of two into
 *      which it fits, and will be contiguous.
 */
extern int
shr_aidxres_list_alloc_block(shr_aidxres_list_handle_t list,
                             shr_aidxres_element_t count,
                             shr_aidxres_element_t *element);

/*
 *   Function
 *      shr_aidxres_list_free
 *   Purpose
 *      Free an element or block of elements back to a list
 *   Parameters
 *      (in) shr_aidxres_list_handle_t list = list from which elem was alloced
 *      (in) shr_aidxres_element_t element = element number to free
 *                                           (or first element in block)
 *   Returns
 *      bcm_error_t = BCM_E_NONE if element freed successfully
 *                    BCM_E_* as appropriate otherwise
 *   Notes
 *      Freeing an entry already in the list is checked, as well as freeing an
 *      entry outside of the list-managed range.  Elements can be freed using
 *      either free call, no matter which alloc call was used to obtain them.
 */
extern int
shr_aidxres_list_free(shr_aidxres_list_handle_t list,
                      shr_aidxres_element_t element);

/*
 *   Function
 *      shr_aidxres_list_free_set
 *   Purpose
 *      Free a set of elements back to a list
 *   Parameters
 *      (in) shr_aidxres_list_handle_t list = list to which to free
 *      (in) shr_aidxres_element_t count = number of elements to free
 *      (in) shr_aidxres_element_t *elements = ptr to array for elems to free
 *      (out) shr_aidxres_element_t *done = ptr to number of successful frees
 *   Returns
 *      bcm_error_t = BCM_E_NONE if element allocated successfully
 *                    BCM_E_* as appropriate otherwise
 *   Notes
 *      This uses the same function as shr_idxres_list_free.  It is possible
 *      that an error prevents completion, so if the result is not success, the
 *      done value must be verified (and any elements that were not done that
 *      can not be reused must still be freed).  Elements can be freed using
 *      either free call, no matter which alloc method was used to obtain them.
 */
extern int
shr_aidxres_list_free_set(shr_aidxres_list_handle_t list,
                          shr_aidxres_element_t count,
                          shr_aidxres_element_t *elements,
                          shr_aidxres_element_t *done);

/*
 *   Function
 *      shr_idxres_list_state
 *   Purpose
 *      Get status of the list itself
 *   Parameters
 *      (in) shr_aidxres_list_handle_t list = list to check
 *      (out) shr_aidxres_element_t *first = buffer for first value
 *      (out) shr_aidxres_element_t *last = buffer for last value
 *      (out) shr_aidxres_element_t *valid_low = buffer for valid_low value
 *      (out) shr_aidxres_element_t *valid_high = buffer for valid_high value
 *      (out) shr_aidxres_element_t *free_count = buffer for free_count value
 *      (out) shr_aidxres_element_t *alloc_count = buffer for alloc_count value
 *      (out) shr_aidxres_element_t *largest_free = buff for largest free value
 *      (out) shr_aidxres_element_t *block_factor = buffer for block factor val
 *   Returns
 *      bcm_error_t = BCM_E_NONE if successful
 *                    BCM_E_* as appropriate otherwise
 *   Notes
 *      If you don't want to fetch a specific attribute of the list, pass
 *      NULL for the pointer to that attribute's location.
 *      There is no set function for these items; most are set at creation of
 *      list and the others are current state of list.
 *      Largest free is the largest number of elements that a block can contain
 *      and still have the alloc request fulfilled on this list.
 */
extern int
shr_aidxres_list_state(shr_aidxres_list_handle_t list,
                       shr_aidxres_element_t *first,
                       shr_aidxres_element_t *last,
                       shr_aidxres_element_t *valid_low,
                       shr_aidxres_element_t *valid_high,
                       shr_aidxres_element_t *free_count,
                       shr_aidxres_element_t *alloc_count,
                       shr_aidxres_element_t *largest_free,
                       shr_aidxres_element_t *block_factor);

/*
 *   Function
 *      shr_aidxres_list_elem_state
 *   Purpose
 *      See if an element is currently in use
 *   Parameters
 *      (in) shr_aidxres_list_handle_t list = list to check
 *      (in) shr_aidxres_element_t element = element number to check
 *   Returns
 *      bcm_error_t = BCM_E_EXISTS if element is in use
 *                    BCM_E_NOT_FOUND if element is not in use
 *                    BCM_E_* as appropriate otherwise
 *   Notes
 *      This function ALWAYS returns an error (never BCM_E_NONE).
 */
extern int
shr_aidxres_list_elem_state(shr_aidxres_list_handle_t list,
                            shr_aidxres_element_t element);

/*
 *   Function
 *      shr_aidxres_list_block_state
 *   Purpose
 *      See if an element is currently in use
 *   Parameters
 *      (in) shr_aidxres_list_handle_t list = list to check
 *      (in) shr_aidxres_element_t element = element number to check
 *      (in) shr_aidxres_element_t size = elements in expected block
 *   Returns
 *      BCM_E_EMPTY if none of the elements are in use
 *      BCM_E_FULL if all of the elements are in use
 *      BCM_E_CONFIG if elements are in use but block(s) do not match
 *      BCM_E_EXISTS if some of the elements are in use but not all of them
 *      BCM_E_PARAM if any of the elements is not valid
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      This function ALWAYS returns an error (never BCM_E_NONE).
 */
int
shr_aidxres_list_block_state(shr_aidxres_list_handle_t list,
                             shr_aidxres_element_t element,
                             shr_aidxres_element_t size);

/*
 *   Function
 *      shr_aidxres_list_reserve
 *   Purpose
 *      Reserve a range of elements in a list
 *   Parameters
 *      (in) shr_aidxres_list_handle_t list = list handle
 *      (in) shr_aidxres_element_t first = first entry to reserve
 *      (in) shr_aidxres_element_t last = last entry to reserve
 *   Returns
 *      bcm_error_t = BCM_E_NONE if elements reserved successfully
 *                    BCM_E_* as appropriate otherwise
 *   Notes
 *      This is truly an inefficient way to manage top and bottom reservations
 *      unless they are not known at list creation time, as this does not do
 *      anything to adjust the physical size of the list's workspace; it merely
 *      takes the requested range out of the available elements.
 *      Elements reserved in this manner can be returned using free; they are
 *      allocated as elements instead of blocks.
 */
extern int
shr_aidxres_list_reserve(shr_aidxres_list_handle_t list,
                         shr_aidxres_element_t first,
                         shr_aidxres_element_t last);

/*
 *   Function
 *      shr_aidxres_list_reserve_block
 *   Purpose
 *      Reserve a block in a list
 *   Parameters
 *      (in) shr_aidxres_list_handle_t list = list handle
 *      (in) shr_aidxres_element_t first = first element in block to reserve
 *      (in) shr_aidxres_element_t count = number of elements in block
 *   Returns
 *      bcm_error_t = BCM_E_NONE if elements reserved successfully
 *                    BCM_E_* as appropriate otherwise
 *   Notes
 *      This is truly an inefficient way to manage top and bottom reservations
 *      unless they are not known at list creation time, as this does not do
 *      anything to adjust the physical size of the list's workspace; it merely
 *      takes the requested range out of the available elements.
 *      Elements reserved in this manner can be returned using free; they are
 *      allocated a a single block.
 *      The block to be reserved must satisfy all allocation rules for blocks
 *      (for example, alignment and size) that apply to the list.
 */
extern int
shr_aidxres_list_reserve_block(shr_aidxres_list_handle_t list,
                               shr_aidxres_element_t first,
                               shr_aidxres_element_t count);

/*
 *  This variable controls sanity checking behaviour, assuming the feature is
 *  enabled in the idxres_afl.c file.
 *
 *  Note that CREATE only checks on RETURN; there is no check to perform for
 *  CREATE on ENTRY.  There is no point in doing checks before DESTROY.
 */
extern uint32 _aidxres_sanity_settings;
#define _AIDXRES_SANITY_POINT_ENTRY  0x00000001
#define _AIDXRES_SANITY_POINT_RETURN 0x00000002
#define _AIDXRES_SANITY_FUNC_ALLOC   0x00000010
#define _AIDXRES_SANITY_FUNC_FREE    0x00000020
#define _AIDXRES_SANITY_FUNC_CREATE  0x00000040
#define _AIDXRES_SANITY_DUMP_FAULTS  0x00010000

#endif /* ndef _SHR_IDXRES_AFL_ */

