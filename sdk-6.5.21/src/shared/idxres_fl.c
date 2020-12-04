/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Module: Indexed resource management, using banked lists
 */

/*
 *  This is an implementation of a high-speed linked-list based freelist
 *  manager for indexed resources.  This particular implementation is designed
 *  to work for sets of resources that are up to 2^31 elements in total, while
 *  allowing as many as four bytes per element (flat mode) or few as one byte
 *  per element (banked mode, also for two bytes per element).  Banked mode
 *  adds a linear time overhead to alloc, but other operations should maintain
 *  their performance ('reserve' actualy gets faster with smaller banks!).
 *
 *  There is no dynamic memory allocation involved in the allocate or free
 *  functions; the only places that use dynamic memory are the create list and
 *  destroy list functions.
 *
 *  When in banked mode, each bank only tracks elements within that bank.  A
 *  list of banks with free elements is also kept.  This list is updated as
 *  elements are allocated and freed so that it always contains only banks with
 *  at least one free element.
 *
 *  Also, because we're using banks and want to use bit shifting operations
 *  instead of multiply and divide, we need to steal a bit from the number of
 *  elements per bank to indicate special cases (the high bit set indicates a
 *  member of the allocated list).
 *
 *  Please note that the lists used here are free lists -- that is, an element
 *  is in the list if it is *not* in use, and is marked in use otherwise.  A
 *  list of the 'in use' elements is not maintained but could be obtained by a
 *  linear traversal of the structures, collecting those elements whose slot in
 *  the list marks them as 'in use'.  The primary reason for not maintaining an
 *  in-use list as well as a free list is simple: in-use elements could be
 *  freed out-of-order and this would make list management an O(n) task;
 *  without that, we can alloc from head of free list and free to tail of free
 *  list in a pure O(1) manner.
 *
 *  Note that this resource manager also allows the list to be biased -- for
 *  example, if a specific resource starts at index 8192, the list can be
 *  created so that 8192 is, in fact, the initial entry internally.  This saves
 *  the caller the effort of translating inputs and outputs for simple mappings
 *  of that nature.  It is possible that more complex mappings (resources with
 *  holes in their indices, for example) can be managed just as easily, though
 *  at the cost of wasting space in the list, by using the reserve function
 *  right after list creation to reserve the holes so they will never be
 *  allocated.
 */

#include <shared/bsl.h>
#include <soc/drv.h>
#include <bcm/error.h>
#include <shared/idxres_fl.h>

/*
 *  Older versions of this code imposed a lock on created lists.  This seems
 *  wasteful because list management should only occur at times where the
 *  applicable resources (including the list) are protected by a lock at a
 *  higher level in the code.
 *
 *  Set _SHR_IDXRES_SELF_LOCKING to TRUE to impose a lock on created lists.
 *  Doing this was the default behaviour for a long time, but it uses more
 *  resources and takes longer to manipulate lists due to the locking overhead.
 *
 *  Set _SHR_IDXRES_SELF_LOCKING to FALSE to not do this. Not doing it will
 *  avoid setting up a lock, and skip the overhead of tracking the lock,
 *  reducing resource usage marginally.
 */
#define _SHR_IDXRES_SELF_LOCKING FALSE

/*
 *  Definitions of list entries within a bank
 *
 *  Must be sure the #defines describe the type accurately.
 *
 *  These may need to be reoptimised for 64b cores; also the calculations are
 *  for 32b cores and they'll probably be wrong (nearly a factor of two for
 *  bytes, but time should not be affected) on 64b cores.
 *
 *  The supported IDXLIST_BASE values are 8, 16, 32.  The value indicates the
 *  number of bits used to represent a single element of the free list, but
 *  there is a tradeoff...
 *
 *                           8            16           32
 *        ----------------   -----------  -----------  -----------
 *        Create             Linear(n)    Linear(n)    Linear(n)
 *        Allocate           Constant(2)  Constant(2)  Constant(2)
 *        Free               Constant(2)  Constant(2)  Constant(2)
 *        Status             Constant(1)  Constant(1)  Constant(1)
 *        Reserve            Linear(k)    Linear(k)    Linear(k)
 *        ----------------   -----------  -----------  -----------
 *        Size of element    1            2            4
 *        Needed banks       (n/128)      (n/32768)    1
 *        Bank overhead      8*m          12*m         20
 *        List overhead      48           48           48
 *        ----------------   -----------  -----------  -----------
 *
 *  For performance: linear() indicates strictly linear performance (some
 *  constant times the parameter; Constant() indicates constant performance
 *  (the parameter indicates relative complexity of the operation).  The
 *  performance of 'reserve' is a bit more complex -- it is linear for the
 *  number of elements to reserve PLUS linear for the number of elements not
 *  already allocated in the list, so it's actually worse unless there are
 *  limited free elements other than those to be reserved.  However, reserve is
 *  primarily intended to be called at initialisation, so this should not be
 *  such a big problem.
 *
 *  For arguments: (n) indicates the number of elements in the list; (m)
 *  indicates the number of banks in the list; (e) indicates the number of
 *  elements used by the call, on calls where it can be nonunit.
 *
 *  For sizes, all are in bytes.  Any divide operation that results in a
 *  nonzero remainder must be rounded up to the next interval (so the result of
 *  (32769 / 32768) must be rounded to 2 -- it may not be truncated to 1).  If
 *  the number of elements in the final bank does not fill the bank, only
 *  enough memory is allocated for the actual elements plus the bank overhead
 *  (it does not allocate the entire worst case bank data, but the entire bank
 *  overhead is required).  Memory use for a list is fixed at time of creation
 *  for that list -- there is no alloc/free activity except by the create and
 *  destroy functions, and they do so with a single (though potentially quite
 *  large) block.  Also, all size calculations assume a compiler that will
 *  operate in 'packed' mode on records and arrays; on some architectures this
 *  may cost dearly in terms of processor time to use anything below 32; also
 *  on some architectures the default mode is 'unpacked', so there may be no
 *  memory savings by going below 32.  Best to examine on a platform basis.
 *
 *  Alocations will either fail immediately (no free elements in any banks) or
 *  succeed in constant time, despite the number of banks.  This is accomplised
 *  at small memory cost (four extra bytes per bank plus eight extra bytes
 *  overall) and minor time during the allocate (if a bank has no more free
 *  elements, it is removed from the list) and free (if a bank is not already
 *  in the list of banks with free elements, it is added to the list).  Both
 *  maintenance operations on the banks-with-free-elements list are, as for the
 *  elements list, constant time operations.
 *
 *  Thrashing will be maintained to a single bank if the number of elements
 *  being thrashed is at least one less than the number of elements in a bank.
 *  This has the happy consequence of improving locality and thence potentially
 *  improving cacheability of the thrashed elements.  If, however, the number
 *  of elements being thrashed is not at least one less than the number of
 *  elements in a bank, the thrashing set will 'creep' through the entire free
 *  set given enough time.
 */
#ifndef IDXLIST_BASE
#define IDXLIST_BASE 8
#endif
#undef IDXRES_ENTRY_IS_FREE
#if (32 == IDXLIST_BASE)
typedef uint32 _idxres_list_entry_t;           /* an element in a list bank */
#define IDXRES_LAST_ENTRY (0xFFFFFFFF)         /* last entry in this list */
#define IDXRES_USED_ENTRY (0xFFFFFFFE)         /* used entry */
#define IDXRES_MAX_ENTRY (0x7FFFFFFF)          /* max entry value */
#define IDXRES_BITS_ENTRY (31)                 /* usable bits per entry */
#define IDXRES_MAX_BANK (0)                    /* max bank value */
#define IDXRES_FORMAT_ENTRY " %08X"            /* format for dumping entry */
#define IDXRES_FORMAT_MASK (0x03)              /* mask for entries per line */
#define IDXRES_ENTRY_IS_FREE(entry) ((0==((entry)&0x80000000))||(IDXRES_LAST_ENTRY==(entry)))
#endif
#if (16 == IDXLIST_BASE)
typedef uint16 _idxres_list_entry_t;           /* an element in a list bank */
#define IDXRES_LAST_ENTRY (0xFFFF)             /* last entry in this list */
#define IDXRES_USED_ENTRY (0xFFFE)             /* used entry */
#define IDXRES_MAX_ENTRY (0x7FFF)              /* max entry value */
#define IDXRES_BITS_ENTRY (15)                 /* usable bits per entry */
#define IDXRES_MAX_BANK (0xFFFFul)             /* max bank value */
#define IDXRES_FORMAT_ENTRY " %04X"            /* format for dumping entry */
#define IDXRES_FORMAT_MASK (0x07)              /* mask for entries per line */
#define IDXRES_ENTRY_IS_FREE(entry) ((0==((entry)&0x8000))||(IDXRES_LAST_ENTRY==(entry)))
#endif
#if (8 == IDXLIST_BASE)
typedef uint8 _idxres_list_entry_t;            /* an element in a list bank */
#define IDXRES_LAST_ENTRY (0xFF)               /* last entry in this list */
#define IDXRES_USED_ENTRY (0xFE)               /* used entry */
#define IDXRES_MAX_ENTRY (0x7F)                /* max entry value */
#define IDXRES_BITS_ENTRY (7)                  /* usable bits per entry */
#define IDXRES_MAX_BANK (0xFFFFFFul)           /* max bank value */
#define IDXRES_FORMAT_ENTRY " %02X"            /* format for dumping entry */
#define IDXRES_FORMAT_MASK (0x0F)              /* mask for entries per line */
#define IDXRES_ENTRY_IS_FREE(entry) ((0==((entry)&0x80))||(IDXRES_LAST_ENTRY==(entry)))
#endif
/*
 *  Note 4 bits per entry is meant for debugging and testing only; it is more
 *  expensive than 8 bits per entry for total memory allocation and also more
 *  expensive in terms of total processor time.
 */
#if (4 == IDXLIST_BASE)
typedef uint8 _idxres_list_entry_t;            /* an element in a list bank */
#define IDXRES_LAST_ENTRY (0xF)                /* last entry in this list */
#define IDXRES_USED_ENTRY (0xE)                /* used entry */
#define IDXRES_MAX_ENTRY (0x7)                 /* max entry value */
#define IDXRES_BITS_ENTRY (3)                  /* usable bits per entry */
#define IDXRES_MAX_BANK (0xFFFFFFFul)          /* max bank value */
#define IDXRES_FORMAT_ENTRY " %01X"            /* format for dumping entry */
#define IDXRES_FORMAT_MASK (0x0F)              /* mask for entries per line */
#define IDXRES_ENTRY_IS_FREE(entry) ((0==((entry)&0x8))||(IDXRES_LAST_ENTRY==(entry)))
#endif
/*
 *  This just makes sure a valid IDXLIST_BASE is selected above.
 */
#ifndef IDXRES_ENTRY_IS_FREE
#error IDXLIST_BASE must be one of: 8; 16; 32.
#endif
/*
 *  Additional macros using the constants defined in the sections above.  These
 *  are not unique per storage size of list entry.
 */
#define IDXRES_ENTRY_IS_USED(entry) (IDXRES_USED_ENTRY==(entry))
#define IDXRES_ENTRY_IS_VALID(entry) (IDXRES_ENTRY_IS_FREE(entry)||IDXRES_ENTRY_IS_USED(entry))
#define IDXRES_LAST_BANK (0xFFFFFFFF)
#define IDXRES_USED_BANK (0xFFFFFFFE)

/*
 *  This structure describes a single bank within the resource free list.  It
 *  contains all that is needed to locate the next free element in that bank,
 *  to return freed elements to the bank, and to track the active number of
 *  free and allocated elements in this bank.
 */
typedef struct _idxres_list_bank_s {
    _idxres_list_entry_t       free_head;    /* head of free list */
    _idxres_list_entry_t       free_tail;    /* tail of free list */
    _idxres_list_entry_t       free_count;   /* number of free elements */
    _idxres_list_entry_t       count;        /* number of elements */
} _idxres_list_bank_t;

/*
 *   This structure describes the entire list.  Note that the data pointer does
 *   not point to another memory block; the entire structure is part of the one
 *   large block of memory -- the index, the bank descriptors, and the bank
 *   data are all in the single alloc cell.
 *
 *   This is here instead of the .h file because we don't want it manipulated
 *   by functions that should be calling the API provided here.
 */
typedef struct _shr_idxres_list_s {
#if _SHR_IDXRES_SELF_LOCKING
    sal_mutex_t                lock;         /* lock for this list */
#endif /* _SHR_IDXRES_SELF_LOCKING */
    shr_idxres_element_t       first;        /* lowest elem managed by list */
    shr_idxres_element_t       last;         /* highest elem managed by list */
    shr_idxres_element_t       valid_low;    /* lowest valid element number */
    shr_idxres_element_t       valid_high;   /* highest valid element number */
    shr_idxres_element_t       free_count;   /* number of free elements */
    shr_idxres_element_t       alloc_count;  /* number of allocated elements */
    shr_idxres_element_t       scale;        /* scaling factor for elements */
    shr_idxres_element_t       bank_max;     /* highest bank number */
    shr_idxres_element_t       bank_head;    /* first bank with free elems */
    shr_idxres_element_t       bank_tail;    /* last bank with free elems */
    shr_idxres_element_t       *bank_list;   /* pointer to free bank list */
    _idxres_list_entry_t       *data;        /* pointer to start of data */
    _idxres_list_bank_t        bank[1];      /* bank descriptors */
    /* a bank descriptor exists for each bank */
    /* actual free lists follow the bank descriptors */
} _idxres_list_t;
#define ELEMENT_BNK(element) ((element) >> IDXRES_BITS_ENTRY)
#define ELEMENT_IDX(element) ((element) & IDXRES_MAX_ENTRY)
#define ELEMENT_NUM(bank, entry) (((bank) << IDXRES_BITS_ENTRY)+(entry))
#define BANK_IS_LEGAL(list, bank) ((bank) <= (list)->bank_max)
#define INDEX_IS_LEGAL(list, bank, index) ((index) < (list)->bank[bank].count)

/*
 *  Generally, IDXRES_DEBUG should NOT be defined.  Then it won't try to do
 *  things like printing status.
 *
 *  The lists are not traversed by the dump function; I didn't want them to be
 *  traversed (that can be done by hand in the output, but you can also check
 *  for crosslinks and other problems this way).
 */
#undef IDXRES_DEBUG
#ifdef IDXRES_DEBUG
#define IDXRES_DUMP(stuff) bsl_printf stuff
#define IDXRES_DUMP_LIST(list) _shr_idxres_dump_list(list)
static void
_shr_idxres_dump_list(const shr_idxres_list_handle_t list){
    unsigned int _bank;
    unsigned int _index;
    IDXRES_DUMP(("Resource list attributes\n"));
    IDXRES_DUMP(("   first = %08X   last = %08X   low = %08X   high = %08X\n",
                 list->first,
                 list->last,
                 list->valid_low,
                 list->valid_high));
    IDXRES_DUMP(("    free = %08X  alloc = %08X  bMax = %08X\n",
                 list->free_count,
                 list->alloc_count,
                 list->bank_max));
    IDXRES_DUMP(("Free banks list\n"));
    IDXRES_DUMP(("    head = %08X   tail = %08X",
                 list->bank_head,
                 list->bank_tail));
    for (_bank = 0; _bank <= list->bank_max; _bank++) {
        if (0 == (_bank & 0x03)) {
            IDXRES_DUMP(("\n    bank # %08X : ",_bank));
        }
        IDXRES_DUMP((" %08X", list->bank_list[_bank]));
    }
    for (_bank = 0; _bank <= list->bank_max; _bank++) {
        IDXRES_DUMP(("\nBank %08X\n",_bank));
        IDXRES_DUMP(("    head = %08X   tail = %08X  free = %08X  count = %08X",
                     list->bank[_bank].free_head,
                     list->bank[_bank].free_tail,
                     list->bank[_bank].free_count,
                     list->bank[_bank].count));
        for (_index = 0; _index < list->bank[_bank].count; _index++) {
            if (0 == (_index & IDXRES_FORMAT_MASK)) {
                IDXRES_DUMP(("\n    elem # %08X : ",_index));
            }
            IDXRES_DUMP((IDXRES_FORMAT_ENTRY,list->data[ELEMENT_NUM(_bank,_index)]));
        }
    }
    IDXRES_DUMP(("\n"));
}
#else
#define IDXRES_DUMP(stuff)
#define IDXRES_DUMP_LIST(list)
#endif

/*
 *   Function
 *      _shr_idxres_list_alloc
 *   Purpose
 *      Allocate the next available element from a list
 *   Parameters
 *      (in) shr_idxres_list_handle_t list = list from which to allocate
 *      (out) shr_idxres_element_t *element = where to put alloced elem num
 *   Returns
 *      BCM_E_NONE if element allocated successfully
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      No locking or parameter checking is performed.  This is used internally
 *      for alloc and alloc_set operations.
 */
static int
_shr_idxres_list_alloc(shr_idxres_list_handle_t list,
                       shr_idxres_element_t *element)
{
    shr_idxres_element_t   bank;     /* working bank during scan */
    shr_idxres_element_t   addr;     /* working current element address */
    _idxres_list_entry_t   curr;     /* working current entry in list */
    _idxres_list_entry_t   next;     /* working next entry in list */
    int            result;   /* value to be returned to caller */

    /* be optimistic about results */
    result = BCM_E_NONE;

    /* see if there are any entries; allocate if so */
    if (list->free_count) {
        /* start with current bank */
        bank = list->bank_head;
        /* okay; this bank has free elements; get head of list */
        curr = list->bank[bank].free_head;
        /* validate the head of the list */
        if (INDEX_IS_LEGAL(list, bank, curr)) {
            /* get the head of the list's address */
            addr = ELEMENT_NUM(bank, curr);
            /* read and verify the head of the list */
            next = list->data[addr];
            if (IDXRES_ENTRY_IS_FREE(next)) {
                /* head entry is valid */
                /* advance head to next element in list */
                list->bank[bank].free_head = next;
                /* mark the former head as in-use */
                list->data[addr] = IDXRES_USED_ENTRY;
                /* return the overall element number of the former head */
                /* note this is biased against the first managed element */
                if (list->scale) {
                    (*element) = list->first + (addr * list->scale);
                } else {
                    (*element) = list->first + addr;
                }
                /* account for allocating this element */
                list->free_count--;
                list->alloc_count++;
                list->bank[bank].free_count--;
                if (0 == list->bank[bank].free_count) {
                    /* no more free entries in this bank; remove from list */
                    list->bank_head = list->bank_list[bank];
                    list->bank_list[bank] = IDXRES_USED_BANK;
                }
            } else { /* if (ENTRY_IS_FREE(entry)) */
                /* should have had valid head pointer; internal error */
                result = BCM_E_INTERNAL;
            } /* if (!ENTRY_IS_FREE(entry)) */
        } else { /* if (INDEX_IS_LEGAL(list, bank, index)) */
            /* should have had a valid head pointer; internal error */
            result = BCM_E_INTERNAL;
        } /* if (INDEX_IS_LEGAL(list, bank, index)) */
    } else { /* if (list->free_count) */
        /* no entries available; indicate resource shortage */
        result = BCM_E_RESOURCE;
    } /* if (list->free_count) */

    /* return the actual result */
    return result;
}

/*
 *   Function
 *      _shr_idxres_list_free
 *   Purpose
 *      Free an element back to a list
 *   Parameters
 *      (in) shr_idxres_list_handle_t list = list from which elem was alloced
 *      (in) shr_idxres_element_t element = element number to free
 *   Returns
 *      BCM_E_NONE if element freed successfully
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Freeing an entry already in the list is checked, as well as freeing an
 *      entry outside of the list-managed range.
 *      No locking and limited parameter checking is performed.  This is used
 *      internally for alloc and alloc_set operations.
 */
static int
_shr_idxres_list_free(shr_idxres_list_handle_t list,
                      shr_idxres_element_t element)
{
    shr_idxres_element_t   bank;     /* working bank */
    _idxres_list_entry_t   tail;     /* working tail entry */
    _idxres_list_entry_t   index;    /* working element in list */
    int            result;   /* value to be returned to caller */

    /* validate parameters */
    if ((element < list->valid_low) ||
        (element > list->valid_high)) {
        /* completely invalid parameters */
        return BCM_E_PARAM;
    }

    /* further validation */
    if ((element < list->first) ||
        (element > list->last)) {
        /* trying to free elments not managed by the list */
        return BCM_E_RESOURCE;
    }

    /* remove bias on entry number */
    if (list->scale) {
        element = (element - list->first) / list->scale;
    } else {
        element = element - list->first;
    }

    /* decode into bank and entry */
    bank = ELEMENT_BNK(element);
    index = ELEMENT_IDX(element);
    tail = list->data[element];

    /* hope for the best */
    result = BCM_E_NONE;

    /* make sure the element is currenyly in use */
    if (IDXRES_ENTRY_IS_USED(tail)) {
        /* this element is in use; put it back in the free list */
        list->data[element] = IDXRES_LAST_ENTRY;
        if (list->bank[bank].free_count) {
            /* not creating a new list; append to existing one */
            tail = list->bank[bank].free_tail;
            if (INDEX_IS_LEGAL(list,bank,tail)) {
                /* old tail pointer was good */
                list->data[ELEMENT_NUM(bank, tail)] = index;
            } else {
                /* but the old tail pointer was invalid */
                result = BCM_E_INTERNAL;
            }
        } else { /* if (list->bank[bank].free_count) */
            /* creating a new list; set head pointer */
            list->bank[bank].free_head = index;
        } /* if (list->bank[bank].free_count) */
        list->bank[bank].free_tail = index;
        /* adjust accounting for freeing the element */
        if (0 == list->bank[bank].free_count) {
            /* the bank is not in the free list; add it */
            list->bank_list[bank] = IDXRES_LAST_BANK;
            if (IDXRES_LAST_BANK != list->bank_head) {
                /* not the first in the list; just append */
                list->bank_list[list->bank_tail] = bank;
            } else {
                /* first in the list; create new list */
                list->bank_head = bank;
            }
            list->bank_tail = bank;
        }
        list->bank[bank].free_count++;
        list->free_count++;
        list->alloc_count--;
    } else if (IDXRES_ENTRY_IS_FREE(tail)) {
        /* this element is not in use */
        result = BCM_E_RESOURCE;
    } else {
        /* something is wrong with this element */
        result = BCM_E_INTERNAL;
    }

    /* return the actual result */
    return result;
}

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
 *      BCM_E_NONE if list created successfully
 *      BCM_E_* as appropriate otherwise
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
int
shr_idxres_list_create_scaled(shr_idxres_list_handle_t *list,
                              shr_idxres_element_t first,
                              shr_idxres_element_t last,
                              shr_idxres_element_t valid_low,
                              shr_idxres_element_t valid_high,
                              shr_idxres_element_t scale,
                              char *name)
{
    shr_idxres_list_handle_t work_list; /* working list */
    shr_idxres_element_t     banks;     /* banks in this list */
    shr_idxres_element_t     count;     /* elements in this list */
    shr_idxres_element_t     bank;      /* working bank number */
    shr_idxres_element_t     addr;      /* working element address */
    _idxres_list_entry_t     entry;     /* working entry number */
    _idxres_list_entry_t     fcount;    /* working free count number */

    /* check parameter validity */
    if ((valid_low > first) ||
        (valid_high < last) ||
        (first > last) ||
#if IDXRES_MAX_BANK
        ((((IDXRES_MAX_BANK + 1) << IDXRES_BITS_ENTRY) - 1) < (valid_high - valid_low))
#else
        (IDXRES_MAX_ENTRY < (valid_high - valid_low))
#endif
        ) {
        /* something's not valid on input */
        return BCM_E_PARAM;
    }

    /* compute the parameters for the memory block */
    if (scale) {
        count = ((last - first) + scale) / scale;
    } else {
        count = last - first + 1;
    }
#if IDXRES_MAX_BANK
    banks = (count + IDXRES_MAX_ENTRY) >> IDXRES_BITS_ENTRY;
    if ((IDXRES_MAX_BANK+1) < banks) {
        /* it's too big still */
        return BCM_E_PARAM;
    }
#else
    banks = 1;
#endif

    /* try to allocate enough space for the list */
    IDXRES_DUMP(("Allocate %d byte cell for %s.\n",
                 ((sizeof(_idxres_list_t) - sizeof(_idxres_list_bank_t)) +
                  (banks * (sizeof(_idxres_list_bank_t) + sizeof(shr_idxres_element_t))) +
                  (count * sizeof(_idxres_list_entry_t))),
                 name));
    work_list = sal_alloc((sizeof(_idxres_list_t) - sizeof(_idxres_list_bank_t)) +
                           (banks * (sizeof(_idxres_list_bank_t) + sizeof(shr_idxres_element_t))) +
                           (count * sizeof(_idxres_list_entry_t)),
                          name);
    if (!work_list) {
        /* unable to allocate the needed memory */
        (*list) = NULL;
        return BCM_E_MEMORY;
    }

#if _SHR_IDXRES_SELF_LOCKING
    /* create and then take the mutex */
    IDXRES_DUMP(("Creating mutex/lock for %s\n",name));
    work_list->lock = sal_mutex_create(name);
    if (!(work_list->lock)) {
        /* unable to create the lock */
        sal_free(work_list);
        (*list) = NULL;
        return BCM_E_RESOURCE;
    }
    IDXRES_DUMP(("Taking lock for %s\n",name));
    if (sal_mutex_take(work_list->lock, sal_mutex_FOREVER)) {
        /* Cound not obtain lock */
        sal_mutex_destroy(work_list->lock);
        sal_free(work_list);
        (*list) = NULL;
        return BCM_E_INTERNAL;
    }
#endif /* _SHR_IDXRES_SELF_LOCKING */

    /* initialise the list */
    work_list->first = first;
    work_list->last = last;
    work_list->valid_low = valid_low;
    work_list->valid_high = valid_high;
    work_list->scale = scale;
    work_list->free_count = count;
    work_list->alloc_count = 0;
    work_list->bank_list = (shr_idxres_element_t*)
                           (((uint8*)(work_list)) +
                            sizeof(_idxres_list_t) +
                            ((banks - 1) * sizeof(_idxres_list_bank_t)));
    work_list->data = (_idxres_list_entry_t*)
                      ((uint8*)(work_list->bank_list) +
                       (banks * sizeof(shr_idxres_element_t)));
    work_list->bank_max = banks - 1;
    work_list->bank_head = 0;
    work_list->bank_tail = banks - 1;
    /* initialise each bank of the list */
    for (bank = 0, addr = 0; bank < banks; bank++) {
        /* initialise this bank */
        work_list->bank_list[bank] = bank + 1;
        work_list->bank[bank].free_head = 0;
#if IDXRES_MAX_BANK
        if (IDXRES_MAX_ENTRY < count) {
            fcount = IDXRES_MAX_ENTRY + 1;
            count -= fcount;
        } else {
#endif
            fcount = count;
#if IDXRES_MAX_BANK
        }
#endif
        work_list->bank[bank].count = fcount;
        work_list->bank[bank].free_count = fcount;
        fcount--;
        work_list->bank[bank].free_tail = fcount;
        /* initialise each entry of this bank */
        for (entry = 1;
             entry <= fcount;
             entry++, addr++) {
            work_list->data[addr] = entry;
        }
        work_list->data[addr] = IDXRES_LAST_ENTRY;
        addr++;
    }
    work_list->bank_list[banks - 1] = IDXRES_LAST_BANK;

    /* debugging */
    IDXRES_DUMP_LIST(work_list);

#if _SHR_IDXRES_SELF_LOCKING
    /* release the lock now */
    if (sal_mutex_give(work_list->lock)) {
        /* could not release lock */
        sal_mutex_destroy(work_list->lock);
        sal_free(work_list);
        (*list) = NULL;
        return BCM_E_INTERNAL;
    }
#endif /* _SHR_IDXRES_SELF_LOCKING */

    /* all done */
    (*list) = work_list;
    return BCM_E_NONE;
}

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
 *      BCM_E_NONE if list created successfully
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      The validLow and validHigh values are used to specify the valid range
 *      of entries for querying 'free/used' status of an entry; any value not
 *      in this range is considered an invalid argument, but values that are
 *      not between first and last will be permanently 'used' and not allowed
 *      by the free operation nor ever provided by the allocate operation.
 *      This function assumes scaling disabled.
 */
int
shr_idxres_list_create(shr_idxres_list_handle_t *list,
                       shr_idxres_element_t first,
                       shr_idxres_element_t last,
                       shr_idxres_element_t valid_low,
                       shr_idxres_element_t valid_high,
                       char *name)
{
    return shr_idxres_list_create_scaled(list,
                                         first,
                                         last,
                                         valid_low,
                                         valid_high,
                                         0,
                                         name);
}

/*
 *   Function
 *      shr_idxres_list_destroy
 *   Purpose
 *      Destroy a list
 *   Parameters
 *      (in) shr_idxres_list_handle_t list = the list handle
 *   Returns
 *      BCM_E_NONE if list created successfully
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      This destroys the list, but does not claim the semaphore first, so the
 *      caller must take care not to destroy the list while it's being used.
 *      It is possible that some OSes will not permit the destruction of a lock
 *      that is in use, so maybe that at least helps.  It is also willing to
 *      destroy the list even if there are still allocated entries.
 */
int
shr_idxres_list_destroy(shr_idxres_list_handle_t list)
{
    shr_idxres_element_t     count;     /* elements in this list */
    shr_idxres_element_t     banks;     /* banks in this list */

    /* check parameter validity */
    if (!list) {
        return BCM_E_PARAM;
    }

    /* debugging */
    IDXRES_DUMP_LIST(list);

#if _SHR_IDXRES_SELF_LOCKING
    /* destroy the semaphore */
    sal_mutex_destroy(list->lock);
#endif /* _SHR_IDXRES_SELF_LOCKING */

    /* compute the parameters for the memory block */
    if (list->scale) {
        count = ((list->last - list->first) + list->scale) / list->scale;
    } else {
        count = list->last - list->first + 1;
    }
    banks = list->bank_max + 1;

    /* poison the list */
    
    sal_memset(list,
               0x00,
               ((sizeof(_idxres_list_t) - sizeof(_idxres_list_bank_t)) +
                (banks * (sizeof(_idxres_list_bank_t) + sizeof(shr_idxres_element_t))) +
                (count * sizeof(_idxres_list_entry_t))));

    /* now free the list */
    sal_free(list);

    /* all done */
    return BCM_E_NONE;
}

/*
 *   Function
 *      shr_idxres_list_alloc
 *   Purpose
 *      Allocate the next available element from a list
 *   Parameters
 *      (in) shr_idxres_list_handle_t list = list from which to allocate
 *      (out) shr_idxres_element_t *element = where to put alloced elem num
 *   Returns
 *      BCM_E_NONE if element allocated successfully
 *      BCM_E_* as appropriate otherwise
 *   Notes
 */
int
shr_idxres_list_alloc(shr_idxres_list_handle_t list,
                      shr_idxres_element_t *element)
{
    int            result;   /* value to be returned to caller */

    /* validate parameters */
    if (!list) {
        return BCM_E_PARAM;
    }

#if _SHR_IDXRES_SELF_LOCKING
    /* claim the lock for the list */
    if (sal_mutex_take(list->lock, sal_mutex_FOREVER)) {
        /* Cound not obtain lock  */
        return BCM_E_INTERNAL;
    }
#endif /* _SHR_IDXRES_SELF_LOCKING */

    /* allocate an element */
    result = _shr_idxres_list_alloc(list, element);

    /* debugging */
    IDXRES_DUMP_LIST(list);

#if _SHR_IDXRES_SELF_LOCKING
    /* release the lock for the list */
    if (sal_mutex_give(list->lock)) {
        /* could not release lock */
        return BCM_E_INTERNAL;
    }
#endif /* _SHR_IDXRES_SELF_LOCKING */

    /* return the actual result */
    return result;
}

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
 *      BCM_E_NONE if element allocated successfully
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      This uses the same function as shr_idxres_list_alloc, except that it
 *      verifies that there are enough elements free to fulfill the request
 *      before it tries to allocate any of them.  It is still possible that an
 *      error prevents completion, however, so if the result is not success,
 *      the done value must be verified (and any elements that were done that
 *      can not be used must be freed).
 *      The set is NOT guaranteed to be contiguous.
 */
int
shr_idxres_list_alloc_set(shr_idxres_list_handle_t list,
                          shr_idxres_element_t count,
                          shr_idxres_element_t *elements,
                          shr_idxres_element_t *done)
{
    int            result;   /* value to be returned to caller */

    /* validate parameters */
    if ((!list) || (!elements) || (!done)) {
        return BCM_E_PARAM;
    }

    /* set initial conditions */
    (*done) = 0;
    result = BCM_E_NONE;

#if _SHR_IDXRES_SELF_LOCKING
    /* claim the lock for the list */
    if (sal_mutex_take(list->lock, sal_mutex_FOREVER)) {
        /* Cound not obtain lock  */
        return BCM_E_INTERNAL;
    }
#endif /* _SHR_IDXRES_SELF_LOCKING */

    /* make sure we have enough free elements */
    if (list->free_count < count) {
        /* not enough free elements to comply */
        result = BCM_E_RESOURCE;
    }

    /* allocate elements */
    while ((0 < count) && (BCM_E_NONE == result)) {
        /* allocate this element */
        result = _shr_idxres_list_alloc(list, elements);
        if (BCM_E_NONE == result) {
            /* success; update accounting */
            elements++;
            count--;
            (*done)++;
        }
    }

    /* debugging */
    IDXRES_DUMP_LIST(list);

#if _SHR_IDXRES_SELF_LOCKING
    /* release the lock for the list */
    if (sal_mutex_give(list->lock)) {
        /* could not release lock */
        return BCM_E_INTERNAL;
    }
#endif /* _SHR_IDXRES_SELF_LOCKING */

    /* return the actual result */
    return result;
}

/*
 *   Function
 *      shr_idxres_list_free
 *   Purpose
 *      Free an element back to a list
 *   Parameters
 *      (in) shr_idxres_list_handle_t list = list from which elem was alloced
 *      (in) shr_idxres_element_t entry = element number to free
 *   Returns
 *      BCM_E_NONE if element freed successfully
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      Freeing an entry already in the list is checked, as well as freeing an
 *      entry outside of the list-managed range.  Elements can be freed using
 *      either free call, no matter which alloc call was used to obtain them.
 */
int
shr_idxres_list_free(shr_idxres_list_handle_t list,
                     shr_idxres_element_t element)
{
    int            result;   /* value to be returned to caller */

    /* validate parameters */
    if (!list) {
        /* completely invalid parameters */
        return BCM_E_PARAM;
    }

#if _SHR_IDXRES_SELF_LOCKING
    /* claim the lock for the list */
    if (sal_mutex_take(list->lock, sal_mutex_FOREVER)) {
        /* Cound not obtain lock  */
        return BCM_E_INTERNAL;
    }
#endif /* _SHR_IDXRES_SELF_LOCKING */

    result = _shr_idxres_list_free(list, element);

    /* debugging */
    IDXRES_DUMP_LIST(list);

#if _SHR_IDXRES_SELF_LOCKING
    /* release the lock for the list */
    if (sal_mutex_give(list->lock)) {
        /* could not release lock */
        return BCM_E_INTERNAL;
    }
#endif /* _SHR_IDXRES_SELF_LOCKING */

    /* return the actual result */
    return result;
}

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
 *      BCM_E_NONE if element allocated successfully
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      This uses the same function as shr_idxres_list_free.  It is possible
 *      that an error prevents completion, so if the result is not success, the
 *      done value must be verified (and any elements that were not done that
 *      can not be reused must still be freed).  Elements can be freed using
 *      either free call, no matter which alloc method was used to obtain them.
 */
int
shr_idxres_list_free_set(shr_idxres_list_handle_t list,
                          shr_idxres_element_t count,
                          shr_idxres_element_t *elements,
                          shr_idxres_element_t *done)
{
    int            result;   /* value to be returned to caller */

    /* validate parameters */
    if ((!list) || (!elements) || (!done)) {
        return BCM_E_PARAM;
    }

    /* set initial conditions */
    (*done) = 0;
    result = BCM_E_NONE;

#if _SHR_IDXRES_SELF_LOCKING
    /* claim the lock for the list */
    if (sal_mutex_take(list->lock, sal_mutex_FOREVER)) {
        /* Cound not obtain lock  */
        return BCM_E_INTERNAL;
    }
#endif /* _SHR_IDXRES_SELF_LOCKING */

    /* free elements */
    while ((0 < count) && (BCM_E_NONE == result)) {
        /* free this element */
        result = _shr_idxres_list_free(list, *elements);
        if (BCM_E_NONE == result) {
            /* success; update accounting */
            elements++;
            count--;
            (*done)++;
        }
    }

    /* debugging */
    IDXRES_DUMP_LIST(list);

#if _SHR_IDXRES_SELF_LOCKING
    /* release the lock for the list */
    if (sal_mutex_give(list->lock)) {
        /* could not release lock */
        return BCM_E_INTERNAL;
    }
#endif /* _SHR_IDXRES_SELF_LOCKING */

    /* return the actual result */
    return result;
}

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
int
shr_idxres_list_state_scaled(shr_idxres_list_handle_t list,
                             shr_idxres_element_t *first,
                             shr_idxres_element_t *last,
                             shr_idxres_element_t *valid_low,
                             shr_idxres_element_t *valid_high,
                             shr_idxres_element_t *free_count,
                             shr_idxres_element_t *alloc_count,
                             shr_idxres_element_t *scale)
{
    if (!list) {
        /* the list has to be valid */
        return BCM_E_PARAM;
    }

    /* Return the values requested by the caller */
    if (first) {
        (*first) = list->first;
    }
    if (last) {
        (*last) = list->last;
    }
    if (valid_low) {
        (*valid_low) = list->valid_low;
    }
    if (valid_high) {
        (*valid_high) = list->valid_high;
    }
    if (free_count) {
        if (list->scale) {
            (*free_count) = list->free_count * list->scale;
        } else {
            (*free_count) = list->free_count;
        }
    }
    if (alloc_count) {
        if (list->scale) {
            (*alloc_count) = list->alloc_count * list->scale;
        } else {
            (*alloc_count) = list->alloc_count;
        }
    }
    if (scale) {
        (*scale) = list->scale;
    }
    return BCM_E_NONE;
}

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
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      If you don't want to fetch a specific attribute of the list, pass
 *      NULL for the pointer to that attribute's location.
 *      There is no set function for these items; most are set at creation of
 *      list and the others are current state of list.
 */
int
shr_idxres_list_state(shr_idxres_list_handle_t list,
                      shr_idxres_element_t *first,
                      shr_idxres_element_t *last,
                      shr_idxres_element_t *valid_low,
                      shr_idxres_element_t *valid_high,
                      shr_idxres_element_t *free_count,
                      shr_idxres_element_t *alloc_count)
{
    return shr_idxres_list_state_scaled(list,
                                        first,
                                        last,
                                        valid_low,
                                        valid_high,
                                        free_count,
                                        alloc_count,
                                        NULL);
}

/*
 *   Function
 *      shr_idxres_list_elem_state
 *   Purpose
 *      See if an element is currently in use
 *   Parameters
 *      (in) shr_idxres_list_handle_t list = list to check
 *      (in) shr_idxres_element_t entry = element number to check
 *   Returns
 *      BCM_E_EXISTS if element is in use
 *      BCM_E_NOT_FOUND if element is not in use
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      This function ALWAYS returns an error (never BCM_E_NONE).
 */
int
shr_idxres_list_elem_state(shr_idxres_list_handle_t list,
                           shr_idxres_element_t element)
{
    _idxres_list_entry_t entry;    /* working entry in list */

    /* validate parameters */
    if ((!list) ||
        (element < list->valid_low) ||
        (element > list->valid_high)) {
        /* completely invalid parameters */
        return BCM_E_PARAM;
    }

    /* further validation */
    if ((element < list->first) ||
        (element > list->last)) {
        /* getting state for elments not managed by the list (in use) */
        return BCM_E_EXISTS;
    }

    /* remove bias on entry number */
    if (list->scale) {
        element = (element - list->first) / list->scale;
    } else {
        element = element - list->first;
    }

    /* get the element state and parse it */
    entry = list->data[element];
    if (IDXRES_ENTRY_IS_USED(entry)) {
        /* the entry is in use */
        return BCM_E_EXISTS;
    } else if (IDXRES_ENTRY_IS_FREE(entry)) {
        /* the entry is free */
        return BCM_E_NOT_FOUND;
    } else {
        /* something is wrong with the entry */
        return BCM_E_INTERNAL;
    }
}

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
 *      BCM_E_NONE if elements reserved successfully
 *      BCM_E_* as appropriate otherwise
 *   Notes
 *      This is truly an inefficient way to manage top and bottom reservations
 *      unless they are not known at list creation time, as this does not do
 *      anything to adjust the physical size of the list's workspace; it merely
 *      takes the requested range out of the available elements.
 *      Elements reserved in this manner can be returned using free.
 */
int
shr_idxres_list_reserve(shr_idxres_list_handle_t list,
                        shr_idxres_element_t first,
                        shr_idxres_element_t last)
{
    int          result;    /* value to be returned to caller */
    shr_idxres_element_t curr_elem; /* working current element address */
    shr_idxres_element_t prev_elem; /* working previous element address*/
    shr_idxres_element_t bank;      /* working bank */
    shr_idxres_element_t prev_bank; /* working previous bank */
    shr_idxres_element_t next_bank; /* working next bank */
    _idxres_list_entry_t curr_ent;  /* working current entry number */
    _idxres_list_entry_t prev_ent;  /* working previous entry number */
    _idxres_list_entry_t next_ent;  /* working next entry number */

    /* validate parameters */
    if ((!list) ||
        (first < list->valid_low) ||
        (last > list->valid_high) ||
        (last < first)) {
        return BCM_E_PARAM;
    }

    /* ensure the requrested range is entirely allocatable */
    if ((first < list->first) ||
        (last > list->last)) {
        return BCM_E_RESOURCE;
    }

#if _SHR_IDXRES_SELF_LOCKING
    /* claim the lock for the list */
    if (sal_mutex_take(list->lock, sal_mutex_FOREVER)) {
        /* Cound not obtain lock  */
        return BCM_E_INTERNAL;
    }
#endif /* _SHR_IDXRES_SELF_LOCKING */

    /* remove bias on range */
    if (list->scale) {
        first = (first - list->first) / list->scale;
        last = (last - list->first) / list->scale;
    } else {
        first -= list->first;
        last -= list->first;
    }

    /* scan the entire range to ensure availability */
    result = BCM_E_NONE;
    for (curr_elem = first; curr_elem <= last; curr_elem++) {
        IDXRES_DUMP(("element %08X =" IDXRES_FORMAT_ENTRY " (%d)\n",
                     curr_elem,
                     list->data[curr_elem],
                     IDXRES_ENTRY_IS_FREE(list->data[curr_elem])));
        if (!IDXRES_ENTRY_IS_FREE(list->data[curr_elem])) {
            /* at least one element is not available */
            result = BCM_E_RESOURCE;
            break;
        }
    }

    /* if all is well so far, reserve the entries in question */
    if (BCM_E_NONE == result) {
        prev_elem = 0; /* this value is overwritten before it is read */
        /* need to reserve the entries */
        for (bank = list->bank_head,
                prev_bank = IDXRES_USED_BANK;
             bank <= list->bank_max;
                bank = next_bank) {
            IDXRES_DUMP(("reserving in bank %08X\n",bank));
            next_bank = list->bank_list[bank];
            if ((ELEMENT_BNK(first) <= bank) ||
                (ELEMENT_BNK(last) >= bank)) {
                prev_ent = IDXRES_LAST_ENTRY;
                curr_ent = list->bank[bank].free_head;
                do { /* while (LAST_ENTRY != curr_ent) */
                    /* while we're on this bank's list */
                    /* compute parameters for this iteration */
                    curr_elem = ELEMENT_NUM(bank, curr_ent);
                    next_ent = list->data[curr_elem];
                    /* see if this element needs to be reserved */
                    if ((first <= curr_elem) && (last >= curr_elem)) {
                        /* this element needs to be reserved */
                        if (IDXRES_LAST_ENTRY == prev_ent) {
                            /* this is head of the list; set head to next  */
                            list->bank[bank].free_head = next_ent;
                        } else {
                            /* this is not head of the list; remove this entry */
                            list->data[prev_elem] = next_ent;
                        }
                        /* mark this entry in use */
                        list->data[curr_elem] = IDXRES_USED_ENTRY;
                        list->free_count--;
                        list->alloc_count++;
                        list->bank[bank].free_count--;
                        /* go to next entry */
                        curr_ent = next_ent;
                    } else { /* if ((first <= curr_elem) && (last >= curr_elem)) */
                        /* this element should not be reserved; move to next */
                        prev_ent = curr_ent;
                        prev_elem = curr_elem;
                        curr_ent = next_ent;
                    } /* if ((first <= curr_elem) && (last >= curr_elem)) */
                } while (IDXRES_LAST_ENTRY != curr_ent);
                /* now, we may need to adjust the tail pointer */
                if (IDXRES_LAST_ENTRY != prev_ent) {
                    /* yes, we need to adjust tail pointer */
                    list->bank[bank].free_tail = prev_ent;
                }
                if (0 == list->bank[bank].free_count) {
                    /* no more free entries in this bank; remove from list */
                    if (IDXRES_USED_BANK == prev_bank) {
                        /* first bank in the list; remove from head */
                        list->bank_head = list->bank_list[bank];
                    } else {
                        /* not first bank in the list */
                        list->bank_list[prev_bank] = list->bank_list[bank];
                    }
                    list->bank_list[bank] = IDXRES_USED_BANK;
                    if (list->bank_tail == bank) {
                        /* last bank in the list; adjust tail */
                        list->bank_tail = prev_bank;
                    }
                } /* if (0 == list->bank[bank].free_count) */
            }
            if (IDXRES_USED_BANK != list->bank_list[bank]) {
                /* update prev_bank only if the current bank was not removed
                 * from the free list and is now empty, otherwise keep the
                 * existing prev_bank for the next update
                 */
                prev_bank = bank;
            }
        } /* for (all banks containing the elements to reserve) */
    } /* if (BCM_E_NONE == result) */

    /* debugging */
    IDXRES_DUMP_LIST(list);

#if _SHR_IDXRES_SELF_LOCKING
    /* release the lock for the list */
    if (sal_mutex_give(list->lock)) {
        /* could not release lock */
        return BCM_E_INTERNAL;
    }
#endif /* _SHR_IDXRES_SELF_LOCKING */

    /* return the actual result */
    return result;
}


