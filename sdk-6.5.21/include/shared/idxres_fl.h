/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Module: Indexed resource management, using banked lists
 */
#ifndef _SHR_IDXRES_FL_
#define _SHR_IDXRES_FL_

#include <sal/types.h>

typedef uint32 shr_idxres_element_t;

struct _shr_idxres_list_s;

typedef struct _shr_idxres_list_s *shr_idxres_list_handle_t;

/*
 *   Function
 *      shr_idxres_list_create_scaled
 *   Purpose
 *      Create a banked free list (with element scaling)
 *   Parameters
 *      (out) shr_idxres_list_handle_t *list = place to put list handle
 *      (in) shr_idxres_element_t first = number of first entry to manage
 *      (in) shr_idxres_element_t last = number of last entry to manage
 *      (in) shr_idxres_element_t validLow = low valid entry value
 *      (in) shr_idxres_element_t validHigh = high valid entry value
 *      (in) shr_idxres_element_t scale = scaling factor
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
 *      Zero for scale disables scaling function.  Scaling factor applies to
 *      all parameters if it is enabled.  Caller must ensure the size of the
 *      range is an integral multiple of scaling factor.
 */
extern int
shr_idxres_list_create_scaled(shr_idxres_list_handle_t *list,
                              shr_idxres_element_t first,
                              shr_idxres_element_t last,
                              shr_idxres_element_t valid_low,
                              shr_idxres_element_t valid_high,
                              shr_idxres_element_t scale,
                              char *name);

/*
 *   Function
 *      shr_idxres_list_create
 *   Purpose
 *      Create a banked free list
 *   Parameters
 *      (out) shr_idxres_list_handle_t *list = place to put list handle
 *      (in) shr_idxres_element_t first = number of first entry to manage
 *      (in) shr_idxres_element_t last = number of last entry to manage
 *      (in) shr_idxres_element_t validLow = low valid entry value
 *      (in) shr_idxres_element_t validHigh = high valid entry value
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
 */
extern int
shr_idxres_list_create(shr_idxres_list_handle_t *list,
                       shr_idxres_element_t first,
                       shr_idxres_element_t last,
                       shr_idxres_element_t valid_low,
                       shr_idxres_element_t valid_high,
                       char *name);

/*
 *   Function
 *      shr_idxres_list_destroy
 *   Purpose
 *      Destroy a list
 *   Parameters
 *      (in) shr_idxres_list_handle_t list = the list handle
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
shr_idxres_list_destroy(shr_idxres_list_handle_t list);

/*
 *   Function
 *      shr_idxres_list_alloc
 *   Purpose
 *      Allocate the next available element from a list
 *   Parameters
 *      (in) shr_idxres_list_handle_t list = list from which to allocate
 *      (out) shr_idxres_element_t *element = where to put alloced elem num
 *   Returns
 *      bcm_error_t = BCM_E_NONE if element allocated successfully
 *                    BCM_E_* as appropriate otherwise
 *   Notes
 */
extern int
shr_idxres_list_alloc(shr_idxres_list_handle_t list,
                      shr_idxres_element_t *element);

/*
 *   Function
 *      shr_idxres_list_alloc_set
 *   Purpose
 *      Allocate the next available element from a list
 *   Parameters
 *      (in) shr_idxres_list_handle_t list = list from which to allocate
 *      (in) shr_idxres_element_t count = number of elements to allocate
 *      (out) shr_idxres_element_t *elements = ptr to array for alloced elems
 *      (out) shr_idxres_element_t *done = ptr for number of successful allocs
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
 */
extern int
shr_idxres_list_alloc_set(shr_idxres_list_handle_t list,
                          shr_idxres_element_t count,
                          shr_idxres_element_t *elements,
                          shr_idxres_element_t *done);

/*
 *   Function
 *      shr_idxres_list_free
 *   Purpose
 *      Free an element back to a list
 *   Parameters
 *      (in) shr_idxres_list_handle_t list = list from which elem was alloced
 *      (in) shr_idxres_element_t entry = element number to free
 *   Returns
 *      bcm_error_t = BCM_E_NONE if element freed successfully
 *                    BCM_E_* as appropriate otherwise
 *   Notes
 *      Freeing an entry already in the list is checked, as well as freeing an
 *      entry outside of the list-managed range.
 */
extern int
shr_idxres_list_free(shr_idxres_list_handle_t list,
                     shr_idxres_element_t element);

/*
 *   Function
 *      shr_idxres_list_free_set
 *   Purpose
 *      Free a set of elements back to a list
 *   Parameters
 *      (in) shr_idxres_list_handle_t list = list to which to free
 *      (in) shr_idxres_element_t count = number of elements to free
 *      (in) shr_idxres_element_t *elements = ptr to array for elems to free
 *      (out) shr_idxres_element_t *done = ptr for number of successful frees
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
shr_idxres_list_free_set(shr_idxres_list_handle_t list,
                          shr_idxres_element_t count,
                          shr_idxres_element_t *elements,
                          shr_idxres_element_t *done);

/*
 *   Function
 *      shr_idxres_list_state
 *   Purpose
 *      Get status of the list itself
 *   Parameters
 *      (in) shr_idxres_list_handle_t list = list to check
 *      (out) shr_idxres_element_t *first = buffer for first value
 *      (out) shr_idxres_element_t *last = buffer for last value
 *      (out) shr_idxres_element_t *valid_low = buffer for valid_low value
 *      (out) shr_idxres_element_t *valid_high = buffer for valid_high value
 *      (out) shr_idxres_element_t *free_count = buffer for free_count value
 *      (out) shr_idxres_element_t *alloc_count = buffer for alloc_count value
 *   Returns
 *      bcm_error_t = BCM_E_NONE if successful
 *                    BCM_E_* as appropriate otherwise
 *   Notes
 *      If you don't want to fetch a specific attribute of the list, pass
 *      NULL for the pointer to that attribute's location.
 *      There is no set function for these items; most are set at creation of
 *      list and the others are current state of list.
 */
extern int
shr_idxres_list_state(shr_idxres_list_handle_t list,
                      shr_idxres_element_t *first,
                      shr_idxres_element_t *last,
                      shr_idxres_element_t *valid_low,
                      shr_idxres_element_t *valid_high,
                      shr_idxres_element_t *free_count,
                      shr_idxres_element_t *alloc_count);

/*
 *   Function
 *      shr_idxres_list_state_scaled
 *   Purpose
 *      Get status of the list itself
 *   Parameters
 *      (in) shr_idxres_list_handle_t list = list to check
 *      (out) shr_idxres_element_t *first = buffer for first value
 *      (out) shr_idxres_element_t *last = buffer for last value
 *      (out) shr_idxres_element_t *valid_low = buffer for valid_low value
 *      (out) shr_idxres_element_t *valid_high = buffer for valid_high value
 *      (out) shr_idxres_element_t *free_count = buffer for free_count value
 *      (out) shr_idxres_element_t *alloc_count = buffer for alloc_count value
 *      (out) shr_idxres_element_t *scale = buffer for scale value
 *   Returns
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      If you don't want to fetch a specific attribute of the list, pass
 *      NULL for the pointer to that attribute's location.
 *      There is no set function for these items; most are set at creation of
 *      list and the others are current state of list.
 */
extern int
shr_idxres_list_state_scaled(shr_idxres_list_handle_t list,
                             shr_idxres_element_t *first,
                             shr_idxres_element_t *last,
                             shr_idxres_element_t *valid_low,
                             shr_idxres_element_t *valid_high,
                             shr_idxres_element_t *free_count,
                             shr_idxres_element_t *alloc_count,
                             shr_idxres_element_t *scale);

/*
 *   Function
 *      shr_idxres_list_elem_state
 *   Purpose
 *      See if an element is currently in use
 *   Parameters
 *      (in) shr_idxres_list_handle_t list = list to check
 *      (in) shr_idxres_element_t entry = element number to check
 *   Returns
 *      bcm_error_t = BCM_E_EXISTS if element is in use
 *                    BCM_E_NOT_FOUND if element is not in use
 *                    BCM_E_* as appropriate otherwise
 *   Notes
 *      This function ALWAYS returns an error (never BCM_E_NONE).
 */
extern int
shr_idxres_list_elem_state(shr_idxres_list_handle_t list,
                           shr_idxres_element_t element);

/*
 *   Function
 *      shr_idxres_list_reserve
 *   Purpose
 *      Reserve a range of elements in a list
 *   Parameters
 *      (in) shr_idxres_list_handle_t list = list handle
 *      (in) shr_idxres_element_t first = first entry to reserve
 *      (in) shr_idxres_element_t last = last entry to reserve
 *   Returns
 *      bcm_error_t = BCM_E_NONE if elements reserved successfully
 *                    BCM_E_* as appropriate otherwise
 *   Notes
 *      This is truly an inefficient way to manage top and bottom reservations
 *      unless they are not known at list creation time, as this does not do
 *      anything to adjust the physical size of the list's workspace; it merely
 *      takes the requested range out of the available elements.
 *      Elements reserved in this manner can be returned using free.
 */
extern int
shr_idxres_list_reserve(shr_idxres_list_handle_t list,
                        shr_idxres_element_t first,
                        shr_idxres_element_t last);

#endif /* ndef _SHR_IDXRES_FL_ */

