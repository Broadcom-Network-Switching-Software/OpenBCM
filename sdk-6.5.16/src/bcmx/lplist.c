/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        lplist.c
 * Purpose:     Logical port list functions
 * Requires:
 *
 * Notes:       The following is intended:
 *
 *     bcmx_lplist_t alist;
 *
 *     ... list is built up with some elements ....
 *
 *     if (!bcmx_lplist_init(alist, 0, 0))  {
 *
 *         ... list is built up with some elements, then ....
 *
 *         BCMX_LPLIST_ITER(alist, lport, count) {
 *             .... do stuff with lport ....
 *         }
 *     }
 */    

#include <sdk_config.h>
#include <sal/core/libc.h>
#include <shared/alloc.h>

#include <bcm/error.h>

#include <bcmx/bcmx.h>
#include <bcmx/lplist.h>

/* This is min size of a list when allocated. */
#ifndef BCMX_LPLIST_MIN_COUNT
#define BCMX_LPLIST_MIN_COUNT 16
#endif

/* This is the increment when reallocating */
#ifndef BCMX_LPLIST_MIN_INCREMENT
#define BCMX_LPLIST_MIN_INCREMENT 16
#endif

/* Is list empty? */
#define LP_EMPTY(list)  ((list)->lp_last < 0)

/* Number of elements currently in list */
#define LP_COUNT(list)  ((list)->lp_last + 1)

/* Size of an 'n' list elements */
#define LP_ELSIZE(n) ((n) * sizeof(bcmx_lport_t))

/* Next free cell on the list.  No updates to list. */
#define LP_FIRST_FREE(list)    ((list)->lp_ports[(list)->lp_last + 1])

/*
 * Function:
 *      _bcmx_lplist_realloc
 * Purpose:
 *      Re-allocate a port list
 * Parameters:
 *      list       -  The port list to change
 *      new_count  -  The new count of ports
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcmx_lplist_realloc(bcmx_lplist_t *list, int new_count)
{
    bcmx_lport_t *old_ports;

    if (list == NULL || list->lp_ports == NULL) {
        return BCM_E_INTERNAL;
    }

    old_ports = list->lp_ports;

    list->lp_ports = (bcmx_lport_t *)
        sal_alloc(LP_ELSIZE(new_count), "lplist");

    if (!list->lp_ports) {
        list->lp_ports = old_ports;
        return BCM_E_MEMORY;
    }

    sal_memcpy(list->lp_ports, old_ports, LP_ELSIZE(LP_COUNT(list)));

    list->lp_alloc = new_count;

    sal_free(old_ports);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmx_lplist_init
 * Purpose:
 *      Allocate and initialize a port list
 * Parameters:
 *      list        - The port list to initialize
 *      init_count  - The initial count to use
 *      flags       - List flags - deprecated and ignored
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If successful, then list->lp_max != 0.
 *      At least BCMX_LPLIST_MIN_COUNT ports will be allocated.
 *      The port list structure itself must be allocated already
 *      (that is, list != NULL).
 */

int
bcmx_lplist_init(bcmx_lplist_t *list, int init_count, uint32 flags)
{
    int ports = BCMX_LPLIST_MIN_COUNT;

    if (!list) {
        return BCM_E_PARAM;
    }

    if (init_count > ports) {
        ports = init_count;
    }

    list->lp_ports = sal_alloc(LP_ELSIZE(ports), "lpports");
    if (!list->lp_ports) {
        return BCM_E_MEMORY;
    }

    list->lp_alloc = ports;
    list->lp_last = -1;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmx_lplist_t_init
 * Purpose:
 *      Initialize the bcmx_lplist_t structure.
 *      Allocate and initialize a port list.
 * Parameters:
 *      list       -  Pointer to port list structure to initialize
 * Returns:
 *      None
 * Note:
 *      Although the functionality of this routine is also provided by
 *      bcmx_lplist_init, this has an API interface consistent
 *      with other structure initialization/free routines.
 */
void
bcmx_lplist_t_init(bcmx_lplist_t *list)
{
    bcmx_lplist_init(list, 0, 0);
}

/*
 * Function:
 *      bcmx_lplist_free
 * Purpose:
 *      De-allocate a port list
 * Parameters:
 *      list        - The port list to de-allocate
 * Returns:
 *      BCM_E_XXX
 */

int
bcmx_lplist_free(bcmx_lplist_t *list)
{
    int rv = BCM_E_PARAM;

    if (!bcmx_lplist_is_null(list)) {
        if (list->lp_ports) {
            sal_free(list->lp_ports);
        }
        list->lp_ports = 0;
        rv = BCM_E_NONE;
    }

    return rv;
}

/*
 * Function:
 *      bcmx_lplist_t_free
 * Purpose:
 *      De-allocate a port list
 * Parameters:
 *      list        - The port list to de-allocate
 * Returns:
 *      None
 * Note:
 *      Although the functionality of this routine is also provided by
 *      bcmx_lplist_free, this has an API interface consistent
 *      with other structure initialization/free routines.
 */
void
bcmx_lplist_t_free(bcmx_lplist_t *list)
{
    bcmx_lplist_free(list);
}

/*
 * Function:
 *      bcmx_lplist_clear
 * Purpose:
 *      Make a port list empty
 * Parameters:
 *      list       -  The port list to clear
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int
bcmx_lplist_clear(bcmx_lplist_t *list)
{
    if (!bcmx_lplist_is_null(list)) {
        list->lp_last = -1;
    }

    return BCM_E_NONE;
}


/****************************************************************
 *
 * The base Logical Port List functions.
 *
 * See also the macros BCMX_LPLIST_REMOVE, etc.
 */


/*
 * Function:
 *      bcmx_lplist_index_get
 * Purpose:
 *      Get the index of the first occurrance of a port on the list
 * Parameters:
 *      list       -- The list to search
 * Returns:
 *      Index >= 0 if found; otherwise BCM_E_NOT_FOUND
 */

int
bcmx_lplist_index_get(bcmx_lplist_t *list, bcmx_lport_t port)
{
    return bcmx_lplist_index_get_from(list, 0, port);
}


/*
 * Function:
 *      bcmx_lplist_index_get_from
 * Purpose:
 *      Get the index of the first occurrance of a port on the list
 *      starting at position
 * Parameters:
 *      list       -- The list to search
 *      position   -- The position to start the search
 * Returns:
 *      BCM_E_PARAM if position is out of range
 *      Index >= 0 if found; otherwise BCM_E_NOT_FOUND
 */

int
bcmx_lplist_index_get_from(bcmx_lplist_t *list, int position, bcmx_lport_t port)
{
    int i;

    if (bcmx_lplist_is_empty(list)) {
        return BCM_E_NOT_FOUND;
    }

    if (position < 0 || position > list->lp_last) {
        return BCM_E_PARAM;
    }

    for (i = position; i <= list->lp_last; i++) {
        if (list->lp_ports[i] == port) {
            return i;
        }
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *      bcmx_lplist_index
 * Purpose:
 *      Get the lport at position
 * Parameters:
 *      list       -- The list to index
 *      position   -- The position to retrieve lport
 * Returns:
 *      BCMX_NO_SUCH_LPORT if position is out of range
 *      otherwise lport
 */

bcmx_lport_t
bcmx_lplist_index(bcmx_lplist_t *list, int position)
{
    bcmx_lport_t lport = BCMX_NO_SUCH_LPORT;

    if (!bcmx_lplist_is_null(list) &&
        position >= 0 && position <= list->lp_last) {
        lport = list->lp_ports[position];
    }

    return lport;
}

/*
 * Function:
 *      bcmx_lplist_add
 * Purpose:
 *      Add a port to the end of a lplist
 * Parameters:
 *      list       -- The list to update
 *      lport      -- The port to add
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Will initialize the list if it is NULL.
 *      Reallocates if not enough space for list.
 */

int
bcmx_lplist_add(bcmx_lplist_t *list, bcmx_lport_t lport)
{
    if (bcmx_lplist_is_null(list)) {
        BCM_IF_ERROR_RETURN(bcmx_lplist_init(list, -1, 0));
    }

    if (BCMX_LPLIST_COUNT(list) + 1 >= list->lp_alloc) {
        if (_bcmx_lplist_realloc(list, list->lp_alloc +
                                 BCMX_LPLIST_MIN_INCREMENT)
            != BCM_E_NONE) {
            return BCM_E_MEMORY;
        }
    }

    (list->lp_last)++;
    list->lp_ports[list->lp_last] = lport;

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcmx_lplist_pbmp_add
 * Purpose:
 *      
 * Parameters:
 *      list       -  The port list to access
 *      unit
 *      pbm
 * Returns:
 * Notes:
 */


int
_bcmx_lplist_pbmp_add(bcmx_lplist_t *list, int unit, bcm_pbmp_t pbm)
{
    bcmx_lport_t lport;
    bcm_port_t port;
    int rv = BCM_E_NONE;

    BCM_PBMP_ITER(pbm, port) {
	lport = bcmx_unit_port_to_lport(unit, port);
	rv = bcmx_lplist_add(list, lport);
	if (rv < 0) {
	    break;
	}
    }

    return rv;
}

/*
 * Function:
 *      bcmx_lplist_port_remove
 * Purpose:
 *      Remove a port from the list.  
 * Parameters:
 *      list     - The list to act on
 *      lport    - The logical port to look for and remove
 *      all      - If true, remove all occurances; otherwise just first.
 * Returns:
 *      BCM_E_PARAM - parameter error
 *      BCM_E_NONE  - port removed
 * Notes:
 */

int
bcmx_lplist_port_remove(bcmx_lplist_t *list, bcmx_lport_t lport, int all)
{
    int rv = BCM_E_NONE;
    int count = 0;
    int idx = 0;

    while ((idx=bcmx_lplist_index_get_from(list, idx, lport)) >= 0) {
        count++;
        bcmx_lplist_idx_remove(list, idx);
        if (!all || (idx > list->lp_last)) {
            break;
        }
    }

    if ((idx < 0 && idx != BCM_E_NOT_FOUND) || count == 0) {
        rv = idx;
    }
   

    return rv;
}

/*
 * Function:
 *      bcmx_lplist_eq
 * Purpose:
 *      Compare two port lists
 * Parameters:
 *      list1   - List to examine
 *      list2   - List to examine
 * Returns:
 *      1 if equal, 0 if not
 * Notes:
 *     Just a simple check that every port in one list is in the other.
 *     Thus it doesn't detect differences in order or multiplicity.
 */

int
bcmx_lplist_eq(bcmx_lplist_t *list1, bcmx_lplist_t *list2)
{
    bcmx_lport_t lport;
    int count;
    int list1_empty, list2_empty;

    list1_empty = bcmx_lplist_is_empty(list1);
    list2_empty = bcmx_lplist_is_empty(list2);

    if (list1_empty && list2_empty) {
        /* Both lists are empty */
        return 1;
    } else if (list1_empty != list2_empty) {
        /* One list is empty, the other not empty */
        return 0;
    } else {
        /* Both lists not empty */
        BCMX_LPLIST_IDX_ITER(list1, lport, count) {
            if (bcmx_lplist_index_get(list2, lport) < 0) {
                return 0;
            }
        }

        BCMX_LPLIST_IDX_ITER(list2, lport, count) {
            if (bcmx_lplist_index_get(list1, lport) < 0) {
                return 0;
            }
        }
    }
    return 1;
}

/*
 * Function:
 *      bcmx_lplist_append
 * Purpose:
 *      Copy list2 to end of list1
 * Parameters:
 *      list1     - List to change
 *      list2     - List copied onto the end of list1
 *
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Does not support sorted lists.
 *      Does not support extra flags for port types
 */

int
bcmx_lplist_append(bcmx_lplist_t *list1, bcmx_lplist_t *list2)
{
    int total;

    if (bcmx_lplist_is_null(list1)) {
        BCM_IF_ERROR_RETURN(bcmx_lplist_init(list1, -1, 0));
    }

    if (bcmx_lplist_is_empty(list2)) {
        return BCM_E_NONE;
    }

    total = LP_COUNT(list1) + LP_COUNT(list2);
    if (list1->lp_alloc <= total) {
        BCM_IF_ERROR_RETURN(_bcmx_lplist_realloc(list1,
                total + BCMX_LPLIST_MIN_INCREMENT));
    }

    sal_memcpy(&(LP_FIRST_FREE(list1)), list2->lp_ports,
               LP_ELSIZE(LP_COUNT(list2)));

    list1->lp_last += LP_COUNT(list2);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmx_lplist_copy
 * Purpose:
 *      Copy src to dest, overwriting dest
 * Parameters:
 *      dest    - Destination list
 *      src     - Source list
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int
bcmx_lplist_copy(bcmx_lplist_t *dest, bcmx_lplist_t *src)
{
    if (bcmx_lplist_is_null(dest)) {
        BCM_IF_ERROR_RETURN(bcmx_lplist_init(dest, -1, 0));
    }

    if (bcmx_lplist_is_empty(src)) {
        return bcmx_lplist_clear(dest);
    }

    dest->lp_last = 0;
    if (dest->lp_alloc < src->lp_alloc) {
        BCM_IF_ERROR_RETURN(_bcmx_lplist_realloc(dest, src->lp_alloc));
    }

    sal_memcpy(dest->lp_ports, src->lp_ports, LP_ELSIZE(LP_COUNT(src)));
    dest->lp_last = src->lp_last;

    return BCM_E_NONE;
}



/* Current absolute max for number of logical ports in a list */
#ifndef BCMX_LPLIST_ABSOLUTE_MAX_LEN
#define BCMX_LPLIST_ABSOLUTE_MAX_LEN 3000  
#endif

/*
 * Function:
 *      bcmx_lplist_check
 * Purpose:
 *      Debug function.  Check the consistency of a list
 * Parameters:
 *      list   - List to examine
 * Returns:
 *      0 if okay, < 0 if not consistent
 */

int
bcmx_lplist_check(bcmx_lplist_t *list)
{
    if (!list) {
        return -1;
    }

    if (!list->lp_ports) {
        return -2;
    }

    if (list->lp_alloc < 0) {
        return -3;
    }

    if (list->lp_alloc > BCMX_LPLIST_ABSOLUTE_MAX_LEN) {
        return -4;
    }

    if (list->lp_last >= list->lp_alloc) {
        return -5;
    }

    return 0;
}

/*
 * Function:
 *      bcmx_lplist_range
 * Purpose:
 *      Add a range of ports to a port list
 * Parameters:
 *      list     - list to add ports to
 *      start    - Low port to add
 *      end      - Last port to add
 * Returns:
 *      BCM_E_XXX
 */

int
bcmx_lplist_range(bcmx_lplist_t *list, bcmx_lport_t start, bcmx_lport_t end)
{
    bcmx_lport_t curport = start;
    int rv;

    while ((curport != end) && (curport != BCMX_NO_SUCH_LPORT)) {
        if ((rv = bcmx_lplist_add(list, curport)) < 0) {
            return rv;
        }
        curport = BCMX_LPORT_NEXT(curport);
    }
    if (curport == end) {
        if ((rv = bcmx_lplist_add(list, curport)) < 0) {
            return rv;
        }
    }

    return BCM_E_NONE;
}

/* Functional version of lplist macros */


/*
 * Function:
 *      bcmx_lplist_is_null
 * Purpose:
 *      
 * Parameters:
 *      list
 *
 * Returns:
 */

int
bcmx_lplist_is_null(bcmx_lplist_t *list)
{
    return BCMX_LPLIST_IS_NULL(list);
}


/*
 * Function:
 *      bcmx_lplist_is_empty
 * Purpose:
 *      
 * Parameters:
 *      list
 *
 * Returns:
 */

int
bcmx_lplist_is_empty(bcmx_lplist_t *list)
{
    return BCMX_LPLIST_IS_EMPTY(list);
}


/*
 * Function:
 *      bcmx_lplist_count
 * Purpose:
 *      
 * Parameters:
 *      list
 *
 * Returns:
 */

int
bcmx_lplist_count(bcmx_lplist_t *list)
{
    return BCMX_LPLIST_COUNT(list);
}


/*
 * Function:
 *      bcmx_lplist_remove
 * Purpose:
 *      
 * Parameters:
 *      list
 *      lport
 *
 * Returns:
 */

void
bcmx_lplist_remove(bcmx_lplist_t *list, int lport)
{
    if (!bcmx_lplist_is_empty(list)) {
        BCMX_LPLIST_REMOVE(list, lport);
    }
}


/*
 * Function:
 *      bcmx_lplist_idx_remove
 * Purpose:
 *      
 * Parameters:
 *      list
 *      idx
 *
 * Returns:
 */

void
bcmx_lplist_idx_remove(bcmx_lplist_t *list, int idx)
{
    if (!bcmx_lplist_is_empty(list)) {
        BCMX_LPLIST_IDX_REMOVE(list, idx);
    }
}


/*
 * Function:
 *      bcmx_lplist_pbmp_add
 * Purpose:
 *      
 * Parameters:
 *      list
 *      unit
 *      pbm
 *
 * Returns:
 */

int
bcmx_lplist_pbmp_add(bcmx_lplist_t *list, int unit, bcm_pbmp_t *pbm)
{
    int rv = BCM_E_PARAM;

    if (list) {
        rv = _bcmx_lplist_pbmp_add(list, unit, *pbm);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_lplist_to_pbmp
 * Purpose:
 *      
 * Parameters:
 *      list
 *      unit
 *      pbm
 *
 * Returns:
 *      void
 */

void
bcmx_lplist_to_pbmp(bcmx_lplist_t *list, int unit, bcm_pbmp_t *pbm)
{
    bcm_pbmp_t p;

    BCM_PBMP_CLEAR(p);
    if (!bcmx_lplist_is_empty(list)) {
        BCMX_LPLISTPTR_TO_PBMP(list, unit, p);
    }
    *pbm = p;
}


/*
 * Function:
 *      _bcmx_lplist_lport_compare
 * Purpose:
 *      lport sort compare function
 * Parameters:
 *      a - pointer to 1st element
 *      b - pointer to 2nd element
 * Returns:
 *      -1 if *a < *b
 *       0 if *a = *b
 *      +1 if *a > *b
 */


STATIC int
_bcmx_lplist_lport_compare(void *a, void *b)
{
    bcmx_lport_t        ap, bp;
    int rv;

    ap = *(bcmx_lport_t *)a;
    bp = *(bcmx_lport_t *)b;

    if (ap < bp) {
	rv = -1;
    } else if (ap > bp) {
	rv = 1;
    } else {
        rv = 0;
    }

    return rv;
}


/*
 * Function:
 *      bcmx_lplist_sort
 * Purpose:
 *      Sort lplist in ascending order
 * Parameters:
 *      list   - List to sort
 * Returns:
 *      BCM_E_NONE  - list sorted
 */

int
bcmx_lplist_sort(bcmx_lplist_t *list)
{
    if (!bcmx_lplist_is_empty(list)) {
        _shr_sort(list->lp_ports, LP_COUNT(list),
                  sizeof(bcmx_lport_t), _bcmx_lplist_lport_compare);
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmx_lplist_uniq
 * Purpose:
 *      Delete duplicate elements in list
 * Parameters:
 *      list   - List to uniqueify
 * Returns:
 *      BCM_E_NONE
 */

int
bcmx_lplist_uniq(bcmx_lplist_t *list)
{
    int idx, dup;
    bcmx_lport_t lport;

    if (!bcmx_lplist_is_empty(list)) {
        for ( idx = 0; idx < list->lp_last; idx++ ) {
            lport = list->lp_ports[idx];
            while ((dup = bcmx_lplist_index_get_from(list,
                                                     idx+1, lport)) >= 0) {
                bcmx_lplist_idx_remove(list, dup);
            }
        }
    }

    return BCM_E_NONE;
}
