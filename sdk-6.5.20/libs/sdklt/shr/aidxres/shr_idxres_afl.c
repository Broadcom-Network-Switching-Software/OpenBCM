/*! \file shr_idxres_afl.c
 *
 * Module: Aligned Indexed resource management, using banked lists
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 *  Older versions of this code imposed a lock on created lists.  This seems
 *  wasteful because list management should only occur at times where the
 *  applicable resources (including the list) are protected by a lock at a
 *  higher level in the code.
 *
 *  Set SHR_AIDXRES_SELF_LOCKING to TRUE to impose a lock on created lists.
 *  Doing this was the default behaviour for a long time, but it uses more
 *  resources and takes longer to manipulate lists due to the locking overhead.
 *
 *  Set SHR_AIDXRES_SELF_LOCKING to FALSE to not do this. Not doing it will
 *  avoid setting up a lock, and skip the overhead of tracking the lock,
 *  reducing resource usage marginally.
 */
#define SHR_AIDXRES_SELF_LOCKING FALSE

/*
 *   Please see the idxres_fl.c file for a description of the banked lists
 *   concept and implementation.  This module enhances that to provide some
 *   additional features.  Concepts that are applicable to both models are not
 *   discussed here -- only the enhancements.
 *
 *   This is an implementation of a high-speed doubly-linked-list based free
 *   list manager. This implmentation, while a little more expensive in both
 *   memory and time than the one in idxres_fl, adds certain additional
 *   features that are necessary in some places:
 *
 *   * Allocation of contiguous blocks.  Blocks of elements can be allocated
 *     contiguously rather than sparsely (as the set of elements operation both
 *     here and in the other module implements).
 *
 *   * Alignment of blocks.  Blocks of elements are aligned to either the power
 *     of two elements that is the size of the block, or the next power of two
 *     up if the block is not an even power of two elements in size.
 *
 *   The overall performance drops slightly (from all case O(1) to a typical
 *   case of O(p), where p is the power to which two must be raised that is
 *   equal to the largest block size supported by the list; if you prefer, it
 *   can be expressed as O(log2(maxBlockSize)) instead), and the memory
 *   requirements increase substantially (4 times the size of the other module
 *   for the main list plus sublist overhead (about 2 times the internal list
 *   overhead of the other module) plus some minor additional overhead for the
 *   entire list).  What this means is that you probably don't want to use this
 *   sort of list to manage a resource that neither needs contiguous blocks nor
 *   needs aligned blocks (use the other module in this case).
 *
 *   Variable block size is supported and managed without massive time overhead
 *   by the use of sublists to keep track of the various sized free blocks.  In
 *   order to keep memory use down, free block lists are only provided for
 *   blocks that contain an integral power of two elements and are properly
 *   aligned to their power of two (this also maintains alignment).  Since free
 *   blocks are always sized as aligned power-of-two elements, there are p (as
 *   defined above) sublists. This has the upshot that larger maximum block
 *   sizes will increase the total size in memory of the list.
 *
 *   Also, the 'scaling' feature is removed; it was believed to be useful in
 *   the other module because if you always allocated blocks of some constant
 *   size, you could just use the other module with scaling enabled.  Here, the
 *   caller specifies the exact block size and non-power-of-two block sizes are
 *   supported just as well as power-of-two block sizes.  It is therefore not
 *   considered helpful for this module (actually, it's somewhat confusing in
 *   this case), and not supported here.  The other module supported using any
 *   value that was covered by a scaled element when freeing it; this module
 *   requires that the first element of a block be provided when freeing (this
 *   could be changed but it would make the free operation an O(max_blocksize)
 *   instead of O(log2(max_blocksize)) operation.
 *
 *   Blocks are allocated using a method approximating best-fit: they are
 *   placed in a smallest necessary free block.  Here, necessary is defined as
 *   being the exact size of the block if it is an integral power of two, or
 *   the next integral power of two up if it is not.  If there are no free
 *   blocks of the necessary size, a larger one will be split so to generate at
 *   least one block of the necessary size.  When splitting blocks to obtain
 *   blocks of the necessary size, the lower portion is returned to the proper
 *   free sublist(s).  Any elements left over from the necessary size (this
 *   only happens if the request is for a block that is not an integral power
 *   of two elements) will be returned to the appropriate free sublist(s), and
 *   these will be the higher numbered elements of the necessary block.
 *
 *   Blocks are freed by combining them with appropriate neighbouring blocks,
 *   breaking the resulting block into the largest possible blocks (if the
 *   resulting block is not an integral power of two elements in size), and
 *   returning the result of that to the appropriate free sublist(s).
 *   Appropriate in this context is used to indicate that a block would combine
 *   with the to-be-freed block in such a way as to preserve alignment of the
 *   combined block, to increase the size of the blocks into which the result
 *   will be broken down, and to not cross bank boundaries.
 *
 *   The maximum block power-of-two is specified at list creation time, and can
 *   be up to the number of bits used to represent elements within a bank (that
 *   is, 7 if in byte mode, 15 in doublebyte mode, and 31 in quadbyte mode).
 *
 *   Memory overhead, as mentioned above, is somewhat higher, and performance
 *   is a little lower, than the single element allocator.  You have to trade
 *   something for the ability to manage contiguous blocks and maintain
 *   alignment for blocks...
 *
 *                           8              16             32
 *        ----------------   -------------  -------------  -------------
 *        Create             Linear(n+p+b)  Linear(n+p+b)  Linear(n+p+b)
 *        Destroy            Linear(n+p+b)  Linear(n+p+b)  Linear(n+p+b)
 *        Allocate           Linear(p+e)    Linear(p+e)    Linear(p+e)
 *        Free               Linear(p+e)    Linear(p+e)    Linear(p+e)
 *        Status             Constant(1)    Constant(1)    Constant(1)
 *        Reserve            Linear(p+e)    Linear(p+e)    Linear(p+e)
 *        BlockReserve       Linear(p+e)    Linear(p+e)    Linear(p+e)
 *        ----------------   -------------  -------------  -------------
 *        Size of element    4              8              16
 *        Needed banks       (n/128)        (n/32768)      1
 *        Max 'p' value      7              15             31
 *        List overhead      52             52             52
 *        Sublist overhead   8+8*b+2*b      8+8*b+4*b      8+8*b+8*b
 *        ----------------   -------------  -------------  -------------
 *
 *   In the table above, 'n' is the number of elements total in the list, 'p'
 *   is log2(maxBlockSize), 'b' is the number of banks, and 'e' is the number
 *   of elements in the block being manipulated.
 *
 *   Note that 'linear' is true for a given list on the performance side: the
 *   values for 'n', 'p', 'b' are set at list creation time, so they will not
 *   vary within a list, but they can vary between lists, and the value 'e' is
 *   specific to a particular call.
 *
 *   The size data are expressed in bytes, assuming the compiler and platform
 *   pack arrays and records.  The list overhead does not include the actual
 *   mutex: it assumes the mutex handle is a pointer to some indetermintate
 *   size object somewhere else.
 *
 *   Reserve is still somewhat more expensive in terms of time and complexity
 *   than alloc/free, but this is because it has to touch each element in
 *   question multiple times and potentially has to insert and remove blocks to
 *   multiple sublists on each side of the reserved blocks.  Blockreserve is
 *   marginally better on time than reserve because it is limited to valid
 *   blocks (so it inherently has a limit for 'e').  It is also useful in that
 *   it reserves the elements as blocks rather than individual.
 */

#include <bsl/bsl.h>
#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <shr/shr_idxres_afl.h>


#ifndef AIDXLIST_BASE

#define AIDXLIST_BASE 32

#endif

#undef AIDXRES_BITS_ENTRY
#if (32 == AIDXLIST_BASE)
typedef uint32_t _aidxres_list_entry_t;         /* an element in a list bank */
#define AIDXRES_FINAL_ENTRY (0xFFFFFFFF)        /* final used entry */
#define AIDXRES_USED_ENTRY (0xFFFFFFFE)         /* other used entries */
#define AIDXRES_FIRST_ENTRY (0xFFFFFFF1)        /* first used entry */
#define AIDXRES_MAX_ENTRY (0x7FFFFFFF)          /* max entry value */
#define AIDXRES_BITS_ENTRY (31)                 /* usable bits per entry */
#define AIDXRES_MAX_BANK (0)                    /* max bank value */
#define AIDXRES_FORMAT_ENTRY "%08X"             /* format for dumping entry */
#define AIDXRES_FORMAT_MASK (0x00)              /* mask for entries per line */
#define AIDXRES_BFORMAT_MASK (0x01)             /* mask for bnk ent per line */
#endif
#if (16 == AIDXLIST_BASE)
typedef uint16_t _aidxres_list_entry_t;         /* an element in a list bank */
#define AIDXRES_FINAL_ENTRY (0xFFFF)            /* final used entry */
#define AIDXRES_USED_ENTRY (0xFFFE)             /* other used entries */
#define AIDXRES_FIRST_ENTRY (0xFFF1)            /* first used entry */
#define AIDXRES_MAX_ENTRY (0x7FFF)              /* max entry value */
#define AIDXRES_BITS_ENTRY (15)                 /* usable bits per entry */
#define AIDXRES_MAX_BANK (0xFFFFul)             /* max bank value */
#define AIDXRES_FORMAT_ENTRY "%04X"             /* format for dumping entry */
#define AIDXRES_FORMAT_MASK (0x00)              /* mask for entries per line */
#define AIDXRES_BFORMAT_MASK (0x01)             /* mask for bnk ent per line */
#endif
#if (8 == AIDXLIST_BASE)
typedef uint8_t _aidxres_list_entry_t;          /* an element in a list bank */
#define AIDXRES_FINAL_ENTRY (0xFF)              /* final used entry */
#define AIDXRES_USED_ENTRY (0xFE)               /* other used entries */
#define AIDXRES_FIRST_ENTRY (0xF1)              /* first used entry */
#define AIDXRES_MAX_ENTRY (0x7F)                /* max entry value */
#define AIDXRES_BITS_ENTRY (7)                  /* usable bits per entry */
#define AIDXRES_MAX_BANK (0xFFFFFFul)           /* max bank value */
#define AIDXRES_FORMAT_ENTRY "%02X"             /* format for dumping entry */
#define AIDXRES_FORMAT_MASK (0x01)              /* mask for entries per line */
#define AIDXRES_BFORMAT_MASK (0x01)             /* mask for bnk ent per line */
#endif
/*
 *  Note 4 bits per entry is meant for debugging and testing only; it is more
 *  expensive than 8 bits per entry for total memory allocation and also more
 *  expensive in terms of total processor time, plus it limits the block size
 *  to a maximum of eight elements, but it does make it easier to visualise the
 *  list when debugging.
 */
#if (4 == AIDXLIST_BASE)
typedef uint8_t _aidxres_list_entry_t;            /* an element in a list bank */
#define AIDXRES_FINAL_ENTRY (0xF)               /* final used entry */
#define AIDXRES_USED_ENTRY (0xE)                /* other used entries */
#define AIDXRES_FIRST_ENTRY (0xA)               /* first used entry */
#define AIDXRES_MAX_ENTRY (0x7)                 /* max entry value */
#define AIDXRES_BITS_ENTRY (3)                  /* usable bits per entry */
#define AIDXRES_MAX_BANK (0xFFFFFFFul)          /* max bank value */
#define AIDXRES_FORMAT_ENTRY "%01X"             /* format for dumping entry */
#define AIDXRES_FORMAT_MASK (0x01)              /* mask for entries per line */
#define AIDXRES_BFORMAT_MASK (0x01)             /* mask for bnk ent per line */
#endif
/*
 *  This just makes sure a valid IDXLIST_BASE is selected above.
 */
#ifndef AIDXRES_BITS_ENTRY
#error AIDXLIST_BASE must be one of: 8; 16; 32.
#endif
/*
 *  Additional macros using the constants defined in the sections above.  These
 *  are not unique per storage size of list entry.
 */
#define AIDXRES_FINAL_BANK (0xFFFFFFFF)
#define AIDXRES_USED_BANK (0xFFFFFFFE)
#define AIDXRES_FIRST_BANK (0xFFFFFFF1)

/*
 *  There are n sublists, where 2^n is the largest single block of elements
 *  that can be allocated by that list.  Each sublist contains lists of blocks
 *  that are 2^m elements long, where m is the number of that sublist.
 *
 *  There are e elements per block, where e is some power of two, so that 2^b
 *  is equal to e.  The value n, above, must be equal to or less than the
 *  value of b here.  In short, the largest allocation unit must be equal to
 *  or smaller than a single block.
 *
 *  Each block contains n element sublists.  Each of these sublists contains
 *  lists of blocks of elements (first element of a block only) that are of
 *  length and alignment 2^n (as above, where n is the sublist number).
 */

/*
 *  This is a single entry in the list.  Since we're keeping alignment and
 *  supporting contiguous allocations larger than one element, we need to know
 *  more about a given element than we need for the simpler case (idxres_fl).
 *
 *  elem_count indicates the number of elements in a block, but is only
 *  guaranteed to be correct for the first element in a block.  For allocated
 *  blocks (not in a sublist), the final element also has the count.  Other
 *  elements in a block will have garbage for count.
 *
 *  sublist indicates which sublist if the element is in a sublist, otherwise
 *  it is AIDXRES_FIRST_ENTRY for the first element in a block,
 *  AIDXRES_USED_ENTRY for the intervening elements in a blocks, and
 *  AIDXRES_FINAL_ENTRY for the final element in a block.
 *
 *  pref and next are pointers to the previous and next elements in the
 *  sublist, or are garbage for elements not in a sublist.
 */
typedef struct aidxres_list_elemdesc_s {
    _aidxres_list_entry_t           elem_count;     /* elems in this block */
    _aidxres_list_entry_t           sublist;        /* which sublist */
    _aidxres_list_entry_t           prev;           /* previous element */
    _aidxres_list_entry_t           next;           /* next element */
} _aidxres_list_elemdesc_t;

/*
 *  There are several lists now -- one for each possible size of block.  This
 *  structure contains the data needed to check the list and get/put a block of
 *  elements from/to the list.  The data for a single bank is made up of a
 *  bunch of these (one for each power of two less than or equal to the power
 *  of two needed to represent the max block size for the list, rounding up).
 */
typedef struct aidxres_list_bank_sublist_s {
    _aidxres_list_entry_t           free_count;     /* elements this list */
    _aidxres_list_entry_t           head;           /* head of this list */
} _aidxres_list_bank_sublist_t;

/*
 *  This structure is how banks participate in sublists.  One of these exists
 *  per bank per sublist.
 */
typedef struct aidxres_list_bank_s {
    shr_aidxres_element_t           prev;           /* previous bank in list */
    shr_aidxres_element_t           next;           /* next bank in list */
} _aidxres_list_bank_t;

/*
 *  This specifies how [bank,sublist] is converted to [index].
 */
#define AIDXRES_BANK_SUBLIST(list,bank,sublist) (((bank)*((list)->sublist_count))+(sublist))

/*
 *  This structure describes the list of banks participating in a specific list
 *  of blocks of a specific number of elements.  The head is actually the bank
 *  number of the head, not the whole element number.
 */
typedef struct aidxres_list_sublist_s {
    shr_aidxres_element_t           free_count;     /* elements this list*/
    shr_aidxres_element_t           head;           /* head of this list */
} _aidxres_list_sublist_t;

/*
 *   This structure describes the entire list.  Note that the data pointer does
 *   not point to another memory block; the entire structure is part of the one
 *   large block of memory -- the index, the bank descriptors, and the bank
 *   data are all in the single alloc cell.
 *
 *   This is here instead of the .h file because we don't want it manipulated
 *   by functions that should be calling the API provided here.
 */
typedef struct shr_aidxres_list_s {
#if SHR_AIDXRES_SELF_LOCKING
    sal_mutex_t                     lock;           /* lock for this list */
#endif /* SHR_AIDXRES_SELF_LOCKING */
    shr_aidxres_element_t           first;          /* low elem mgd by list */
    shr_aidxres_element_t           last;           /* high elem mgd by list */
    shr_aidxres_element_t           valid_low;      /* low valid element */
    shr_aidxres_element_t           valid_high;     /* high valid element */
    shr_aidxres_element_t           free_count;     /* free elements */
    shr_aidxres_element_t           alloc_count;    /* allocated elements */
    shr_aidxres_element_t           sublist_count;  /* number of sublists */
    shr_aidxres_element_t           bank_max;       /* maximum bank */
    shr_aidxres_element_t           bank_rem_max;   /* final bank max elem */
    _aidxres_list_bank_t            *bank_list;     /* bank list pointer */
    _aidxres_list_bank_sublist_t    *bank_sublist;  /* bank sublist pointer */
    _aidxres_list_elemdesc_t        *element;       /* element pointer */
    _aidxres_list_sublist_t         sublist[1];     /* sublist information */
    /* a sublist descriptor exists for each sublist */
    /* a bank descriptor exists for each bank */
    /* a bank sublist descriptor exists for each sublist in each bank */
    /* an element exists for each element in the list */
} _aidxres_list_t;
#define AIDXRES_ELEMENT_BNK(element) ((element) >> AIDXRES_BITS_ENTRY)
#define AIDXRES_ELEMENT_IDX(element) ((element) & AIDXRES_MAX_ENTRY)
#define AIDXRES_ELEMENT_NUM(bank, entry) (((bank) << AIDXRES_BITS_ENTRY)+(entry))

/*
 *  Generally, AIDXRES_DEBUG and AIDXRES_DEBUG_VERBOSE should NOT be defined.
 *  These switch on some debuging information (with AIDXRES_DEBUG_VERBOSE
 *  including state dumps very often).
 *
 *  The lists are not traversed by the dump function; I didn't want them to be
 *  traversed (that can be done by hand in the output, but you can also check
 *  for crosslinks and other problems this way).
 */
#undef AIDXRES_DEBUG
#undef AIDXRES_DEBUG_VERBOSE

/*
 *  Define AIDXRES_SANITY_CHECKING to enable deep sanity checks of lists.
 *  If it is not defined, the checks will never be performed.  If it is
 *  defined, the checks will only be performed as below (minimal additional
 *  overhead under normal conditions in this case).
 *
 *  The sanity check checks the list to be sure things are intact.  The bank
 *  sublists are first checked for loops (this involves a traversal of each
 *  of the bank sublists).  Each bank then has its element sublists checked for
 *  loops and element count accuracy (an accumulator is kept for overall
 *  element count accuracy checking), and then any blocks that do not appear on
 *  the bank sublists will be checked to be sure they look correct.  Finally,
 *  the overall element counts will be verified.
 *
 *  Even if AIDXRES_SANITY_CHECKING is defined, the value of the variable
 *  aidxres_sanity_settings, defined below (and also see the defines in the
 *  header file for AIDXRES_SANITY_POINT_* and AIDXRES_SANITY_FUNC_*) will
 *  control when the checks are actually performed.  Futhermore, a dump can
 *  be performed of a list should the checks fail.  Without the checks enabled
 *  the various points where the checks would be performed will only suffer
 *  the penalty of a test+branch.  AIDXRES_DEFAULT_SANITY here sets the
 *  default value of aidxres_sanity_settings, which can be manipulated by
 *  other code, such as the example 'afl paranoia' command on SBX devices.
 */
#undef AIDXRES_SANITY_CHECKING
#if 0
#define AIDXRES_DEFAULT_SANITY \
    (AIDXRES_SANITY_POINT_ENTRY | \
     AIDXRES_SANITY_POINT_RETURN | \
     AIDXRES_SANITY_FUNC_ALLOC | \
     AIDXRES_SANITY_FUNC_FREE | \
     AIDXRES_SANITY_FUNC_CREATE | \
     AIDXRES_SANITY_DUMP_FAULTS)
#else
#define AIDXRES_DEFAULT_SANITY 0
#endif

/*
 *  Implementation of the diagnostic and sanity checking features
 */
#ifdef AIDXRES_DEBUG
#define AIDXRES_DUMP(stuff) bsl_printf stuff
#ifdef AIDXRES_DEBUG_VERBOSE
#define AIDXRES_DUMP_LIST(list) aidxres_dump_list(list)
#else /* def AIDXRES_DEBUG_VERBOSE */
#define AIDXRES_DUMP_LIST(list)
#endif /* def AIDXRES_DEBUG_VERBOSE */
#else /* def AIDXRES_DEBUG */
#define AIDXRES_DUMP(stuff)
#define AIDXRES_DUMP_LIST(list)
#endif /* def AIDXRES_DEBUG */
#if defined(AIDXRES_DEBUG_VERBOSE) || defined(AIDXRES_SANITY_CHECKING)
static void
aidxres_dump_list(const shr_aidxres_list_handle_t list){
    unsigned int _sublist;
    unsigned int _bank;
    unsigned int _offset;
    unsigned int _index;
    unsigned int _max;
    LOG_CLI((BSL_META("Resource list attributes\n")));
    LOG_CLI((BSL_META("   first = %08X   last = %08X   low = %08X   high = %08X\n"),
             list->first,
             list->last,
             list->valid_low,
             list->valid_high));
    LOG_CLI((BSL_META("    free = %08X  alloc = %08X  bMax = %08X  bRMax = %08X\n"),
             list->free_count,
             list->alloc_count,
             list->bank_max,
             list->bank_rem_max));
    LOG_CLI((BSL_META("    sCnt = %08X"),
             list->sublist_count));
    for (_sublist = 0; _sublist < list->sublist_count; _sublist++) {
        LOG_CLI((BSL_META("\nFree banks sublist %02X"), _sublist));
        LOG_CLI((BSL_META("    head=%08X  free=%08X"),
                 list->sublist[_sublist].head,
                 list->sublist[_sublist].free_count));
        for (_bank = 0; _bank <= list->bank_max; _bank++) {
            if (0 == (_bank & AIDXRES_BFORMAT_MASK)) {
                LOG_CLI((BSL_META("\n    bank # %08X :"), _bank));
            }
            _index = AIDXRES_BANK_SUBLIST(list, _bank, _sublist);
            LOG_CLI((BSL_META("   p=%08X n=%08X"),
                     list->bank_list[_index].prev,
                     list->bank_list[_index].next));
        }
    }
    for (_bank = 0; _bank <= list->bank_max; _bank++) {
        LOG_CLI((BSL_META("\nBank %08X\n"),_bank));
        for (_sublist = 0; _sublist < list->sublist_count; _sublist++) {
            _index = AIDXRES_BANK_SUBLIST(list, _bank, _sublist);
            LOG_CLI((BSL_META("    sublist %02X : head="
                              AIDXRES_FORMAT_ENTRY
                              "  free="
                              AIDXRES_FORMAT_ENTRY
                              "\n"),
                     _sublist,
                     list->bank_sublist[_index].head,
                     list->bank_sublist[_index].free_count));
        }
        if (_bank < list->bank_max) {
            _max = AIDXRES_MAX_ENTRY;
        } else {
            _max = list->bank_rem_max;
        }
        for (_offset = 0; _offset <= _max; _offset++) {
            _index = AIDXRES_ELEMENT_NUM(_bank, _offset);
            if (0 == (_offset & AIDXRES_FORMAT_MASK)) {
                LOG_CLI((BSL_META("\n    elem # %08X ["
                                  AIDXRES_FORMAT_ENTRY
                                  "] :"),
                         _index + list->first,
                         AIDXRES_ELEMENT_IDX(_index)));
            }
            LOG_CLI((BSL_META("   e="
                              AIDXRES_FORMAT_ENTRY
                              " s="
                              AIDXRES_FORMAT_ENTRY
                              " p="
                              AIDXRES_FORMAT_ENTRY
                              " n="
                              AIDXRES_FORMAT_ENTRY,
                              list->element[_index].elem_count,
                              list->element[_index].sublist,
                              list->element[_index].prev,
                              list->element[_index].next);
        }
    }
    LOG_CLI((BSL_META("\n")));
}
#endif /* defined(AIDXRES_DEBUG_VERBOSE) || defined(AIDXRES_SANITY_CHECKING) */
/* NOT static */ uint32_t aidxres_sanity_settings = AIDXRES_DEFAULT_SANITY;
#ifdef AIDXRES_SANITY_CHECKING
static int
_aidxres_list_bank_check(const shr_aidxres_list_handle_t list,
                         shr_aidxres_element_t bank,
                         SHR_BITDCL *buffer,
                         shr_aidxres_element_t *totalFree)
{
    int result = SHR_E_NONE;
    int abort = FALSE;
    _aidxres_list_entry_t elems;
    _aidxres_list_entry_t free;
    _aidxres_list_entry_t elem;
    _aidxres_list_entry_t offs;
    _aidxres_list_entry_t last;
    _aidxres_list_entry_t which;
    _aidxres_list_elemdesc_t *elemDesc;
    shr_aidxres_element_t sublist;
    _aidxres_list_bank_sublist_t *bankSublist;

    /* some initial setup work */
    elemDesc = &(list->element[AIDXRES_ELEMENT_NUM(bank, 0)]);
    if (bank < list->bank_max) {
        elems = AIDXRES_MAX_ENTRY + 1;
    } else {
        elems = list->bank_rem_max + 1;
    }
    SHR_BITCLR_RANGE(buffer, 0, elems);
    /* scan each sublist for free elements */
    for (sublist = 0; sublist < list->sublist_count; sublist++) {
        bankSublist = &(list->bank_sublist[AIDXRES_BANK_SUBLIST(list, bank, sublist)]);
        free = 0;
        elem = bankSublist->head;
        last = AIDXRES_FIRST_ENTRY;
        if (elem < elems) {
            if (elemDesc[elem].prev != AIDXRES_FIRST_ENTRY) {
                LOG_CLI((BSL_META("aidxres %p element %08X bank %08X sublist %02X"
                                  " head " AIDXRES_FORMAT_ENTRY " prev invalid: "
                                  AIDXRES_FORMAT_ENTRY "\n"),
                         (void*)list,
                         AIDXRES_ELEMENT_NUM(bank, elem) + list->first,
                         bank,
                         sublist,
                         elem,
                         elemDesc[elem].prev));
                result = SHR_E_INTERNAL;
                abort = TRUE;
            }
        } else if (elem != AIDXRES_FINAL_ENTRY) {
            LOG_CLI((BSL_META("aidxres %p bank %08X sublist %02X head "
                              AIDXRES_FORMAT_ENTRY " should be "
                              AIDXRES_FORMAT_ENTRY " or < "
                              AIDXRES_FORMAT_ENTRY "\n"),
                     (void*)list,
                     bank,
                     sublist,
                     elem,
                     AIDXRES_FINAL_ENTRY,
                     elems));
            elem = AIDXRES_FINAL_ENTRY;
            result = SHR_E_INTERNAL;
            abort = TRUE;
        }
        while ((!abort) && (elem <= AIDXRES_MAX_ENTRY)) {
            if (last != elemDesc[elem].prev) {
                LOG_CLI((BSL_META("aidxres %p element %08X..%08X bank %08X sublist"
                                  " %02X prev " AIDXRES_FORMAT_ENTRY " should be "
                                  AIDXRES_FORMAT_ENTRY "\n"),
                         (void*)list,
                         AIDXRES_ELEMENT_NUM(bank, elem) + list->first,
                         AIDXRES_ELEMENT_NUM(bank, elem) + list->first + elemDesc[elem].elem_count - 1,
                         bank,
                         sublist,
                         elemDesc[elem].prev,
                         last));
                result = SHR_E_INTERNAL;
            }
            if ((1u << sublist) != elemDesc[elem].elem_count) {
                LOG_CLI((BSL_META("aidxres %p element %08X..%08X bank %08X sublist"
                                  " %02X block size %u should be %u\n"),
                         (void*)list,
                         AIDXRES_ELEMENT_NUM(bank, elem) + list->first,
                         AIDXRES_ELEMENT_NUM(bank, elem) + list->first + elemDesc[elem].elem_count - 1,
                         bank,
                         sublist,
                         elemDesc[elem].elem_count,
                         1u << sublist));
                result = SHR_E_INTERNAL;
                elem = AIDXRES_FINAL_ENTRY;
                abort = TRUE;
                break;
            }
            if ((1u << sublist) + elem > elems) {
                LOG_CLI((BSL_META("aidxres %p element %08X..%08X bank %08X sublist"
                                  " %02X extends past end of bank "
                                  AIDXRES_FORMAT_ENTRY "\n"),
                         (void*)list,
                         AIDXRES_ELEMENT_NUM(bank, elem) + list->first,
                         AIDXRES_ELEMENT_NUM(bank, elem) + list->first + elemDesc[elem].elem_count - 1,
                         bank,
                         sublist,
                         elems));
                result = SHR_E_INTERNAL;
            }
            if ((elemDesc[elem].next >= elems) &&
                (elemDesc[elem].next != AIDXRES_FINAL_ENTRY)) {
                LOG_CLI((BSL_META("aidxres %p element %08X..%08X bank %08X sublist"
                                  " %02X next " AIDXRES_FORMAT_ENTRY " should be "
                                  AIDXRES_FORMAT_ENTRY " or < "
                                  AIDXRES_FORMAT_ENTRY "\n"),
                         (void*)list,
                         AIDXRES_ELEMENT_NUM(bank, elem) + list->first,
                         AIDXRES_ELEMENT_NUM(bank, elem) + list->first + elemDesc[elem].elem_count - 1,
                         bank,
                         sublist,
                         elemDesc[elem].next,
                         AIDXRES_FINAL_ENTRY,
                         elems));
                result = SHR_E_INTERNAL;
                elem = AIDXRES_FINAL_ENTRY;
                abort = TRUE;
                break;
            }
            for (offs = 0; offs < elemDesc[elem].elem_count; offs++) {
                if (sublist != elemDesc[elem + offs].sublist) {
                    LOG_CLI((BSL_META("aidxres %p element %08X..%08X bank %08X"
                                      " sublist %02X offset %u sublist "
                                      AIDXRES_FORMAT_ENTRY " should be "
                                      AIDXRES_FORMAT_ENTRY "\n"),
                             (void*)list,
                             AIDXRES_ELEMENT_NUM(bank, elem) + list->first,
                             AIDXRES_ELEMENT_NUM(bank, elem) + list->first + elemDesc[elem].elem_count - 1,
                             bank,
                             sublist,
                             offs,
                             elemDesc[elem + offs].sublist,
                             sublist));
                    result = SHR_E_INTERNAL;
                }
                if (SHR_BITGET(buffer, elem + offs)) {
                    LOG_CLI((BSL_META("aidxres %p element %08X..%08X bank %08X elem "
                                      AIDXRES_FORMAT_ENTRY " already encountered\n"),
                             (void*)list,
                             AIDXRES_ELEMENT_NUM(bank, elem + offs) + list->first,
                             AIDXRES_ELEMENT_NUM(bank, elem + offs) + list->first + (free - 1),
                             bank,
                             elem + offs));
                    result = SHR_E_INTERNAL;
                    abort = TRUE;
                    break;
                }
                SHR_BITSET(buffer, elem + offs);
                if (offs > 0) {
                    if (elemDesc[elem].elem_count - 1 == offs) {
                        which = AIDXRES_FINAL_ENTRY;
                    } else {
                        which = AIDXRES_USED_ENTRY;
                    }
                    if (which != elemDesc[elem + offs].elem_count) {
                        LOG_CLI((BSL_META("aidxres %p element %08X..%08X bank %08X"
                                          " sublist %02X offset %u elem_count "
                                          AIDXRES_FORMAT_ENTRY " should be "
                                          AIDXRES_FORMAT_ENTRY "\n"),
                                 (void*)list,
                                 AIDXRES_ELEMENT_NUM(bank, elem) + list->first,
                                 AIDXRES_ELEMENT_NUM(bank, elem) + list->first + elemDesc[elem].elem_count - 1,
                                 bank,
                                 sublist,
                                 offs,
                                 elemDesc[elem + offs].elem_count,
                                 which));
                        result = SHR_E_INTERNAL;
                    }
                } /* if (offs > 0) */
            } /* for (offs = 0; offs < elemDesc[elem].elem_count; offs++) */
            free += elemDesc[elem].elem_count;
            last = elem;
            elem = elemDesc[elem].next;
        } /* while (elem <= AIDXRES_MAX_ENTRY) */
        if ((elem != AIDXRES_FINAL_ENTRY) && (last != AIDXRES_FINAL_ENTRY)) {
            LOG_CLI((BSL_META("aidxres %p element %08X bank %08X sublist %02X tail "
                              AIDXRES_FORMAT_ENTRY " next invalid: "
                              AIDXRES_FORMAT_ENTRY "\n"),
                     (void*)list,
                     AIDXRES_ELEMENT_NUM(bank, elem) + list->first,
                     bank,
                     sublist,
                     last,
                     elem));
            result = SHR_E_INTERNAL;
        }
        if ((!abort) && (free != bankSublist->free_count)) {
            LOG_CLI((BSL_META("aidxres %p bank %08X sublist %02X count "
                              AIDXRES_FORMAT_ENTRY " incorrect, counted "
                              AIDXRES_FORMAT_ENTRY "\n"),
                     (void*)list,
                     bank,
                     sublist,
                     bankSublist->free_count,
                     free));
            result = SHR_E_INTERNAL;
        }
        totalFree[sublist] += free;
    } /* for (sublist = 0; sublist < list->sublist_count; sublist++) */
    /*
     *  Scan allocated elements for consistency, but we can only do this if the
     *  list consistency check went okay (since we will not otherwise know
     *  which elements are not in lists).
     */
    if (!abort) {
        for (elem = 0; elem < elems; /* incr deliberately */) {
            if (SHR_BITGET(buffer, elem)) {
                /* element was already found in a sublist */
                elem++;
            } else { /* if (SHR_BITGET(buffer, elem)) */
                /* element not checked yet */
                free = elemDesc[elem].elem_count;
                for (offs = 0; offs < free; offs++) {
                    if (SHR_BITGET(buffer, elem + offs)) {
                        LOG_CLI((BSL_META("aidxres %p allocated element %08X..%08X"
                                          " bank %08X elem " AIDXRES_FORMAT_ENTRY
                                          " already encountered\n"),
                                 (void*)list,
                                 AIDXRES_ELEMENT_NUM(bank, elem + offs) + list->first,
                                 AIDXRES_ELEMENT_NUM(bank, elem + offs) + list->first + (free - 1),
                                 bank,
                                 elem + offs));
                        result = SHR_E_INTERNAL;
                    }
                    SHR_BITSET(buffer, elem + offs);
                    if (0 == offs) {
                        last = AIDXRES_FIRST_ENTRY;
                    } else if ((free - 1) == offs) {
                        last = AIDXRES_FINAL_ENTRY;
                        if (free != elemDesc[elem + offs].elem_count) {
                            LOG_CLI((BSL_META("aidxres %p allocated element"
                                              " %08X..%08X bank %08X elem "
                                              AIDXRES_FORMAT_ENTRY " elem_count "
                                              AIDXRES_FORMAT_ENTRY " should be "
                                              AIDXRES_FORMAT_ENTRY "\n"),
                                     (void*)list,
                                     AIDXRES_ELEMENT_NUM(bank, elem + offs) + list->first,
                                     AIDXRES_ELEMENT_NUM(bank, elem + offs) + list->first + (free - 1),
                                     bank,
                                     elem + offs,
                                     elemDesc[elem + offs].sublist,
                                     free));
                            result = SHR_E_INTERNAL;
                        }
                    } else {
                        last = AIDXRES_USED_ENTRY;
                    }
                    if (last != elemDesc[elem + offs].sublist) {
                        LOG_CLI((BSL_META("aidxres %p allocated element %08X..%08X"
                                          " bank %08X elem " AIDXRES_FORMAT_ENTRY
                                          " sublist " AIDXRES_FORMAT_ENTRY
                                          " should be " AIDXRES_FORMAT_ENTRY "\n"),
                                 (void*)list,
                                 AIDXRES_ELEMENT_NUM(bank, elem + offs) + list->first,
                                 AIDXRES_ELEMENT_NUM(bank, elem + offs) + list->first + (free - 1),
                                 bank,
                                 elem + offs,
                                 elemDesc[elem + offs].sublist,
                                 last));
                        result = SHR_E_INTERNAL;
                    }
                } /* for (offs = 0; offs < free; offs++) */
                /* skip to end of this block */
                elem += free;
            } /* if (SHR_BITGET(buffer, elem)) */
        } /* for (elem = 0; elem <= AIDXRES_MAX_ENTRY;) */
    } /* if (!abort) */
    return result;
}
static int
_aidxres_list_check(const shr_aidxres_list_handle_t list)
{
    int result = SHR_E_NONE;
    int tmpRes;
    SHR_BITDCL *buffer;
    shr_aidxres_element_t bslcurr;
    shr_aidxres_element_t bank;
    shr_aidxres_element_t prevBank;
    shr_aidxres_element_t *totalFree;
    shr_aidxres_element_t sublist;

    if (list->bank_max > AIDXRES_MAX_ENTRY) {
        bslcurr = SHR_BITALLOCSIZE(list->bank_max + 1);
    } else {
        /* don't ask me about implied signed division */
        bslcurr = SHR_BITALLOCSIZE(AIDXRES_MAX_ENTRY) + sizeof(SHR_BITDCL);
    }
    buffer = sal_alloc(bslcurr, "aidx list cycle checking");
    if (NULL == buffer) {
        LOG_CLI((BSL_META("aidxres %p unable to validate (c): not enough memory\n"),
                 (void*)list));
        return SHR_E_NONE;
    }
    totalFree = sal_alloc(sizeof(shr_aidxres_element_t) * list->sublist_count,
                          "aidx free count checking");
    if (NULL == totalFree) {
        LOG_CLI((BSL_META("aidxres %p unable to validate (f): not enough memory\n"),
                 (void*)list));
        sal_free(buffer);
        return SHR_E_NONE;
    }
    sal_memset(totalFree, 0x00, sizeof(shr_aidxres_element_t) * list->sublist_count);
    for (sublist = 0; sublist < list->sublist_count; sublist++) {
        SHR_BITCLR_RANGE(buffer, 0, list->bank_max + 1);
        bank = list->sublist[sublist].head;
        if (list->bank_max >= bank) {
            /* the initial bank is valid; traverse the list */
            bslcurr = AIDXRES_BANK_SUBLIST(list, bank, sublist);
            prevBank = AIDXRES_FIRST_BANK;
            while (bank < AIDXRES_FIRST_BANK) {
                if (SHR_BITGET(buffer, bank)) {
                    LOG_CLI((BSL_META("aidxres %p sublist %02X bank %08X has"
                                      " already been encountered\n"),
                             (void*)list,
                             sublist,
                             bank));
                    result = SHR_E_INTERNAL;
                    break;
                }
                if (list->bank_list[bslcurr].prev != prevBank) {
                    LOG_CLI((BSL_META("aidxres %p sublist %02X bank %08X prev %08X"
                                      " should be %08X\n"),
                             (void*)list,
                             sublist,
                             bank,
                             list->bank_list[bslcurr].prev,
                             prevBank));
                    result = SHR_E_INTERNAL;
                }
                SHR_BITSET(buffer, bank);
                prevBank = bank;
                bank = list->bank_list[bslcurr].next;
                if ((bank > list->bank_max) && (AIDXRES_FINAL_BANK != bank)) {
                    LOG_CLI((BSL_META("aidxres %p sublist %02X bank %08X next %08X"
                                      " should be %08X or < %08X\n"),
                             (void*)list,
                             sublist,
                             prevBank,
                             bank,
                             AIDXRES_FINAL_BANK,
                             list->bank_max + 1));
                    result = SHR_E_INTERNAL;
                    break;
                }
                bslcurr = AIDXRES_BANK_SUBLIST(list, bank, sublist);
            } /* (bank < AIDXRES_FIRST_BANK) */
        } else if ((AIDXRES_FINAL_BANK != bank) && (list->bank_max < bank)) {
            LOG_CLI((BSL_META("aidxres %p sublist %02X first bank %08X should be"
                              " either %08X or < %08X\n"),
                     (void*)list,
                     sublist,
                     bank,
                     AIDXRES_FINAL_BANK,
                     list->bank_max + 1));
            result = SHR_E_INTERNAL;
        }
        for (bank = 0; bank <= list->bank_max; bank++) {
            if (!SHR_BITGET(buffer, bank)) {
                /* only check unvisited banks (no free elems this sublist) */
                bslcurr = AIDXRES_BANK_SUBLIST(list, bank, sublist);
                if (AIDXRES_USED_BANK != list->bank_list[bslcurr].prev) {
                    LOG_CLI((BSL_META("aidxres %p sublist %02X empty bank %08X prev"
                                      " %08X should be %08X\n"),
                             (void*)list,
                             sublist,
                             bank,
                             list->bank_list[bslcurr].prev,
                             AIDXRES_USED_BANK));
                }
                if (AIDXRES_USED_BANK != list->bank_list[bslcurr].next) {
                    LOG_CLI((BSL_META("aidxres %p sublist %02X empty bank %08X next"
                                      " %08X should be %08X\n"),
                             (void*)list,
                             sublist,
                             bank,
                             list->bank_list[bslcurr].next,
                             AIDXRES_USED_BANK));
                }
            } /* if (!SHR_BITGET(buffer, bank)) */
        } /* for (bank = 0; bank <= list->bank_mak; bank++) */
    }
    for (bank = 0; bank <= list->bank_max; bank++) {
        tmpRes = _aidxres_list_bank_check(list, bank, buffer, totalFree);
        if (SHR_E_NONE != tmpRes) {
            result = tmpRes;
        }
    }
    for (sublist = 0; sublist < list->sublist_count; sublist++) {
        if (list->sublist[sublist].free_count != totalFree[sublist]) {
            LOG_CLI((BSL_META("aidxres %p sublist %02X free count %08X incorrect;"
                              " counted %08X\n"),
                     (void*)list,
                     sublist,
                     list->sublist[sublist].free_count,
                     totalFree[sublist]));
            result = SHR_E_INTERNAL;
        }
    }
    sal_free(totalFree);
    sal_free(buffer);
    if ((SHR_E_NONE != result) &&
        (aidxres_sanity_settings & AIDXRES_SANITY_DUMP_FAULTS)) {
        LOG_CLI((BSL_META("aidxres %p appears corrupt; dumping it\n"), (void*)list));
        aidxres_dump_list(list);
    }
    return result;
}
#endif /* def AIDXRES_SANITY_CHECKING */

/*!
 * \brief Prepare a block and insert it into the specified sublist, then be sure
 * the bank is also in the sublist.
 *
 * No locking or parameter checking is performed.  This is used internally
 * for alloc class and free class operations.  Does not combine adjacent
 * blocks either across sublists or within a sublist, as this may be
 * called during alloc as well as free.
 *
 * \param [in] shr_aidxres_list_handle_t list = list to manipulate.
 * \param [in] shr_aidxres_element_t first = first element of block.
 * \param [in] shr_aidxres_element_t sublist = sublist for insertion.
 *
 * \return Nothing.
 */
static void
_add_block_to_sublist(shr_aidxres_list_handle_t list,
                      shr_aidxres_element_t first,
                      shr_aidxres_element_t sublist) {
    shr_aidxres_element_t  bslcurr;  /* working [bank,sublist] index */
    shr_aidxres_element_t  bank;     /* working bank */
    shr_aidxres_element_t  count;    /* working elements in block */
    _aidxres_list_entry_t  firstidx; /* working new first element index */
    _aidxres_list_entry_t  previdx;  /* working prev first elem index */

    AIDXRES_DUMP(("_add_block_to_sublist(*,%08X,%02X) called\n",
                  first,
                  sublist));
    /* figure out how big the block is and adjust free size */
    count = 1 << sublist;
    AIDXRES_DUMP(("_add_block_to_sublist: count = %08X\n",count));
    AIDXRES_DUMP(("_add_block_to_sublist: sublist free = %08X\n",
                  list->sublist[sublist].free_count));
    list->sublist[sublist].free_count += count;
    AIDXRES_DUMP(("_add_block_to_sublist: sublist free = %08X\n",
                  list->sublist[sublist].free_count));
    /* figure out which bank */
    bank = AIDXRES_ELEMENT_BNK(first);
    firstidx = AIDXRES_ELEMENT_IDX(first);
    /* compute the index for the bank and sublist information */
    bslcurr = AIDXRES_BANK_SUBLIST(list, bank, sublist);
    /* adjust elements free this bank */
    list->bank_sublist[bslcurr].free_count += count;
    /* Set element count in first element */
    list->element[first].elem_count = count;
    /* Set previous and next first element pointers in first element */
    list->element[first].prev = AIDXRES_FIRST_ENTRY;
    previdx = list->bank_sublist[bslcurr].head;
    list->element[first].next = previdx;
    /* adjust former first element to point to this as new first element */
    if (previdx <= AIDXRES_MAX_ENTRY) {
        list->element[AIDXRES_ELEMENT_NUM(bank,previdx)].prev = firstidx;
    }
    /* Insert this block to the sublist */
    list->bank_sublist[bslcurr].head = firstidx;
    /* Mark the block as not in use */
#ifdef AIDXRES_SANITY_CHECKING
    firstidx = 1;
    previdx = count;
#endif /* def AIDXRES_SANITY_CHECKING */
    while (count) {
        list->element[first].sublist = sublist;
#ifdef AIDXRES_SANITY_CHECKING
        if (1 < firstidx) {
            if (previdx == firstidx) {
                list->element[first].elem_count = AIDXRES_FINAL_ENTRY;
            } else {
                list->element[first].elem_count = AIDXRES_USED_ENTRY;
            }
        }
        firstidx++;
#endif /* def AIDXRES_SANITY_CHECKING */
        first++;
        count--;
    }
    /* Be sure the bank is in the sublist */
    if (AIDXRES_USED_BANK == list->bank_list[bslcurr].next) {
        /* bank is not already in this sublist; add it */
        /* for this bank, next points to prior head & prev is BOL */
        list->bank_list[bslcurr].prev = AIDXRES_FIRST_BANK;
        list->bank_list[bslcurr].next = list->sublist[sublist].head;
#if AIDXRES_MAX_BANK
        if (list->sublist[sublist].head < AIDXRES_MAX_BANK) {
            /* make prior head bank's prev point to this bank */
            list->bank_list[AIDXRES_BANK_SUBLIST(list,
                                                 list->sublist[sublist].head,
                                                 sublist)].prev = bank;
        }
#endif
        /* make this bank head of the list */
        list->sublist[sublist].head = bank;
    }
}

/*!
 * \brief Remove the first block from a sublist in preparation for allocate.
 *
 * No locking or parameter checking is performed.  This is used internally
 * for alloc class operations.  Will remove a bank from the sublist if
 * there are no more appropriate blocks in that bank.  Does not prepare
 * the block for return to the caller, as it may be fragmented first.
 *
 * \param  [in] shr_aidxres_list_handle_t list = list to manipulate..
 * \param  [in] shr_aidxres_element_t sublist = sublist to manipulate.
 * \param  [out] shr_aidxres_element_t *element = place to put grabbed first elem.
 *
 * \return Nothing.
 */
static void
_remove_first_from_sublist(shr_aidxres_list_handle_t list,
                           shr_aidxres_element_t sublist,
                           shr_aidxres_element_t *element)
{
    shr_aidxres_element_t  bslcurr;  /* working [bank,sublist] index */
    shr_aidxres_element_t  bank;     /* working bank number */
    shr_aidxres_element_t  count;    /* working elements in block */

    AIDXRES_DUMP(("_remove_first_from_sublist(*,%02X,*) called\n",sublist));
    /* figure out how big the block is and adjust free size */
    count = 1 << sublist;
    AIDXRES_DUMP(("_remove_first_from_sublist: count = %08X\n",count));
    AIDXRES_DUMP(("_remove_first_from_sublist: sublist free = %08X\n",
                  list->sublist[sublist].free_count));
    list->sublist[sublist].free_count -= count;
    AIDXRES_DUMP(("_remove_first_from_sublist: sublist free = %08X\n",
                  list->sublist[sublist].free_count));
    /* get the first bank that has an element in this list */
    bank = list->sublist[sublist].head;
    /* compute the index for the bank and sublist information */
    bslcurr = AIDXRES_BANK_SUBLIST(list, bank, sublist);
    /* get first element of first block of first bank of the sublist */
    *element = AIDXRES_ELEMENT_NUM(bank, list->bank_sublist[bslcurr].head);
    /* update this bank's sublist */
    list->bank_sublist[bslcurr].head = list->element[*element].next;
    list->bank_sublist[bslcurr].free_count -= count;
    if (list->bank_sublist[bslcurr].head <= AIDXRES_MAX_ENTRY) {
        list->element[AIDXRES_ELEMENT_NUM(bank,list->bank_sublist[bslcurr].head)].prev = AIDXRES_FIRST_ENTRY;
    }
    if (0 == list->bank_sublist[bslcurr].free_count) {
        /* this bank has no more elements in this sublist; remove it */
        /* point head of sublist to next bank in sublist */
        list->sublist[sublist].head = list->bank_list[bslcurr].next;
#if AIDXRES_MAX_BANK
        /* set new head's prev pointer to indicate first bank */
        if (list->sublist[sublist].head < AIDXRES_MAX_BANK) {
            list->bank_list[AIDXRES_BANK_SUBLIST(list,
                                            list->sublist[sublist].head,
                                            sublist)].prev = AIDXRES_FIRST_BANK;
        }
#endif
        /* set this bank's next and prev to indicate it has no blks this lst */
        list->bank_list[bslcurr].prev = AIDXRES_USED_BANK;
        list->bank_list[bslcurr].next = AIDXRES_USED_BANK;
    }
    AIDXRES_DUMP(("_remove_first_from_sublist(*,%02X,&(%08X)) returning\n",
                  sublist,
                  *element));
}

/*!
 * \brief Remove a specific block from a sublist during defragmentation.
 *
 * No locking or parameter checking is performed.  This is used internally
 * for free class operations (specifically during defragmentation).  Will
 * remove a bank from the sublist if there are no more appropriate blocks
 * in that bank.
 *
 * \param [in] shr_aidxres_list_handle_t list = list to manipulate.
 * \param [in] shr_aidxres_element_t first = first element of block.
 * \param [in] shr_aidxres_element_t sublist = sublist for insertion.
 *
 * \return Nothing.
*/
static void
_remove_block_from_sublist(shr_aidxres_list_handle_t list,
                      shr_aidxres_element_t first,
                      shr_aidxres_element_t sublist) {
    shr_aidxres_element_t  bslcurr;  /* working [bank,sublist] index */
    shr_aidxres_element_t  bank;     /* working bank number */
    shr_aidxres_element_t  count;    /* working elements in block */

    AIDXRES_DUMP(("_remove_block_from_sublist(*,%08X,%02X) called\n",
                  first,
                  sublist));
    /* figure out how big the block is and adjust free size */
    count = 1 << sublist;
    list->sublist[sublist].free_count -= count;
    /* compute the index for the bank and sublist information */
    bank = AIDXRES_ELEMENT_BNK(first);
    bslcurr = AIDXRES_BANK_SUBLIST(list, bank, sublist);
    /* remove this block from its sublist */
    if (AIDXRES_FIRST_ENTRY != list->element[first].prev) {
        /* there is something in the bank before this block, point to after */
        list->element[AIDXRES_ELEMENT_NUM(bank, list->element[first].prev)].next =
            list->element[first].next;
    } else {
        /* adjust head of list to point to after this block */
        list->bank_sublist[bslcurr].head = list->element[first].next;
    }
    if (AIDXRES_FINAL_ENTRY != list->element[first].next) {
        /* there is something in the bank after this block, point to before */
        list->element[AIDXRES_ELEMENT_NUM(bank, list->element[first].next)].prev =
            list->element[first].prev;
    }
    /* adjust elements free this bank */
    list->bank_sublist[bslcurr].free_count -= count;
    /* remove this bank from the sublist if appropriate */
    if (0 == list->bank_sublist[bslcurr].free_count) {
        /* this bank has no more blocks on this sublist; remove it */
        if (AIDXRES_FIRST_BANK == list->bank_list[bslcurr].prev) {
            /* this is the first bank; update head to skip it */
            list->sublist[sublist].head = list->bank_list[bslcurr].next;
        } else {
            /* this is not the first bank; update prev to skip it */
            list->bank_list[AIDXRES_BANK_SUBLIST(list,
                                                 list->bank_list[bslcurr].prev,
                                                 sublist)].next =
                list->bank_list[bslcurr].next;
        }
        if (AIDXRES_FINAL_BANK != list->bank_list[bslcurr].next) {
            /* this is not the last bank; udpate the following's prev value */
            list->bank_list[AIDXRES_BANK_SUBLIST(list,
                                                 list->bank_list[bslcurr].next,
                                                 sublist)].prev =
                list->bank_list[bslcurr].prev;
        }
        /* set this bank's next and prev to indicate it has no blks this lst */
        list->bank_list[bslcurr].prev = AIDXRES_USED_BANK;
        list->bank_list[bslcurr].next = AIDXRES_USED_BANK;
    }
    AIDXRES_DUMP(("_remove_block_from_sublist(*,%08X,%02X) returning\n",
                  first,
                  sublist));
}

/*!
 * \brief Prepare a block to be returned to the caller after successful alloc.
 *
 * No locking or parameter checking is performed.  This is used internally
 * for alloc class and free class operations.  Assumes caller removes the
 * block from the sublist.
 *
 * \param [in] shr_aidxres_list_handle_t list = list to manipulate.
 * \param [in] shr_aidxres_element_t first = first element of block.
 * \param [in] shr_aidxres_element_t count = number of elements in block.
 *
 * \return Nothing.
 */
static void
_prep_block_for_alloc(shr_aidxres_list_handle_t list,
                      shr_aidxres_element_t first,
                      shr_aidxres_element_t count) {
    _aidxres_list_entry_t   remain;   /* remaining element count */

    AIDXRES_DUMP(("_prep_block_for_alloc(*,%08X,%08X) called\n",
                  first,
                  count));
    /* Set element count in first element */
    list->element[first].elem_count = count;
    /* Mark the block as in use */
    list->element[first].sublist = AIDXRES_FIRST_ENTRY;
    remain = count - 1;
    first++;
    while (remain > 1) {
        list->element[first].sublist = AIDXRES_USED_ENTRY;
        remain--;
        first++;
    }
    /* specially tag final element if it's not first element */
    if (remain) {
        list->element[first].sublist = AIDXRES_FINAL_ENTRY;
        list->element[first].elem_count = count;
    }
    AIDXRES_DUMP(("_prep_block_for_alloc: returning\n"));
}

/*!
 * \brief Allocate the next available block of appropriate size from a list.
 *
 * No locking or parameter checking is performed.  This is used internally
 * for alloc and alloc_set operations.
 * This will always allocate a block aligned to the size of the request if
 * it is a power of two, or to the next greater power of two elements, and
 * it will return any remaning elements to the appropriate sublists.
 *
 * \param [in] shr_aidxres_list_handle_t list = list from which to allocate.
 * \param [in] shr_aidxres_element_t count = number of elements in block.
 * \param [out] shr_aidxres_element_t *element = where to put alloced block base.
 *
 * \retval SHR_E_NONE Element allocated successfully.
 * \retval SHR_E_PARAM Invalid parameter.
 */
static int
_shr_aidxres_list_alloc(shr_aidxres_list_handle_t list,
                        shr_aidxres_element_t count,
                        shr_aidxres_element_t *element)
{
    int                    result;   /* value to be returned to caller */
#ifdef AIDXRES_SANITY_CHECKING
    int                    auxRes;   /* aux result for sanity checking */
#endif /* def AIDXRES_SANITY_CHECKING */
    shr_aidxres_element_t  blkbase;  /* working block base element number */
    shr_aidxres_element_t  temp;     /* scratchpad element count/number */
    _aidxres_list_entry_t  sublist;  /* working sublist number */
    _aidxres_list_entry_t  bias;     /* working bias of returned block */
    _aidxres_list_entry_t  actual;   /* actual size of acquired block */
    _aidxres_list_entry_t  needed;   /* needed size of block */

    AIDXRES_DUMP(("_shr_aidxres_list_alloc(*,%08X,*) called\n",count));
#ifdef AIDXRES_SANITY_CHECKING
    if ((aidxres_sanity_settings & (AIDXRES_SANITY_POINT_ENTRY |
                                     AIDXRES_SANITY_FUNC_ALLOC)) ==
        (AIDXRES_SANITY_POINT_ENTRY | AIDXRES_SANITY_FUNC_ALLOC)) {
        auxRes = _aidxres_list_check(list);
        if (SHR_E_NONE != auxRes) {
            return auxRes;
        }
    }
#endif /* def AIDXRES_SANITY_CHECKING */

    /* check some argument validity */
    if (!count) {
        /* count is zero; that's not allowed */
        return SHR_E_PARAM;
    }

    /* be optimistic about results */
    result = SHR_E_NONE;

    /* decide which sublist needs to be used for this block */
    AIDXRES_DUMP(("_shr_aidxres_list_alloc: search for best fit sublist\n"));
    for (temp = 1, needed = 1, sublist = 0;
         ((temp < count) && temp);
         temp = temp << 1, needed = needed << 1, sublist++) {
        /* do nothing; the loop iterator and condition do the work here */
    }
    /* at this point, 'needed' indicates the smallest block for the alloc */
    actual = needed;
    /* make sure the decided sublist is valid */
    if (temp && (sublist < list->sublist_count)) {
        /* sublist is valid here, so block size is okay */
        while ((sublist < list->sublist_count) &&
               (!(list->sublist[sublist].free_count))) {
            /* this sublist has no free entries; try the next one up */
            sublist++;
            temp = temp << 1;
            actual = actual << 1;
        }
        if ((!temp) || (sublist >= list->sublist_count)) {
            /* no free blocks large enough */
            result = SHR_E_RESOURCE;
        }
    } else {
        /* block is too big */
        result = SHR_E_PARAM;
    }

    /* if we found a usable sublist, get a block from it */
    if (SHR_E_NONE == result) {
        AIDXRES_DUMP(("_shr_aidxres_list_alloc: get first from sublist %02X\n",
                      sublist));
        /* okay; we have a sublist with large enough elements */
        /* get first block from that sublist */
        _remove_first_from_sublist(list, sublist, &blkbase);
        /* determine how high to bias returned block within actual block */
        AIDXRES_DUMP(("_shr_aidxres_list_alloc: place requested block\n"));
        bias = (actual - count) & (~(needed -1));
        /* set returned element number */
        *element = blkbase + bias + list->first;
        /* check need for fragmentation of the block */
        if (actual != count) {
            /* attach lower subblocks to appropriate lists */
            if (actual != needed) {
                /*
                 *  The block was bigger than the minimum size for the alloc
                 *  request; since the alloc request came out of the top
                 *  portion of the block, we must fragment and return the
                 *  lower portion.
                 */
                AIDXRES_DUMP(("_shr_aidxres_list_alloc: alloc block is too big;"
                              " return lower part to appropriate sublists\n"));
                /* block was bigger than needed */
                /* return first part to free list */
                temp = blkbase;
                do {
                    /* subblock is half the size of the last/original one */
                    /* take largest subblocks and work down */
                    sublist--;
                    actual = actual >> 1;
                    _add_block_to_sublist(list, temp, sublist);
                    temp += actual;
                } while (actual > needed);
            }
            /* move to end of allocated block */
            AIDXRES_DUMP(("_shr_aidxres_list_alloc: return leftover space"
                          " (%08X) at top (%08X) to appropriate sublists\n",
                          needed - count,
                          bias + count));
            temp = blkbase + bias + count;
            /* compute elements remaining above allocated block */
            needed -= count;
            sublist = 0;
            actual = 1;
            while (needed) {
                /*
                 *  The request was for a block that was not an even power of
                 *  two elements long.  We need to fragment the remainder of
                 *  the block and return those fragments to the free list.
                 */
                if (needed & actual) {
                    /* we can slice off this much of it properly */
                    _add_block_to_sublist(list, temp, sublist);
                    temp += actual;
                    needed -= actual;
                } else {
                    actual = actual << 1;
                    sublist++;
                }
            } /* while (needed) */
        } /* if (actual != count) */
        /* adjust final free and alloc counts */
        list->free_count -= count;
        list->alloc_count += count;

        /* prepare returned block */
        _prep_block_for_alloc(list, blkbase + bias, count);
    } /* if (SHR_E_NONE == result) */

    /* debugging */
    AIDXRES_DUMP_LIST(list);
#ifdef AIDXRES_SANITY_CHECKING
    if ((aidxres_sanity_settings & (AIDXRES_SANITY_POINT_RETURN |
                                     AIDXRES_SANITY_FUNC_ALLOC)) ==
        (AIDXRES_SANITY_POINT_RETURN | AIDXRES_SANITY_FUNC_ALLOC)) {
        auxRes = _aidxres_list_check(list);
        if (SHR_E_NONE != auxRes) {
            return auxRes;
        }
    }
#endif /* def AIDXRES_SANITY_CHECKING */

    /* return the actual result */
    return result;
}

/*!
 * \brief Free a block back to a list.
 *
 * Freeing an entry already in the list is checked, as well as freeing an
 * entry outside of the list-managed range.
 * No locking and limited parameter checking is performed.  This is used
 * internally for alloc and alloc_set operations.
 * This will automatically defragment the resource list by combining
 * contiguous blocks into larger ones if possible, at least up to the
 * largest supported block for the list.
 *
 * \param [in] shr_aidxres_list_handle_t list = list from which block was alloced.
 * \param [in] shr_aidxres_element_t = first element in block to free.
 *
 * \retval SHR_E_NONE Element freed successfully.
 * \retval SHR_E_PARAM Invalid parameter.
 * \retval SHR_E_RESOURCE Ilegal member access.
*/
static int
_shr_aidxres_list_free(shr_aidxres_list_handle_t list,
                       shr_aidxres_element_t element)
{
    shr_aidxres_element_t   sublist;  /* working sublist number */
    _aidxres_list_entry_t   submask;  /* working sublist bit value mask */
    shr_aidxres_element_t   currCnt;  /* working count */
    shr_aidxres_element_t   count;    /* number of elements in block */
    shr_aidxres_element_t   min;      /* minimum element number this block */
    _aidxres_list_entry_t   max;      /* maximum element number this block */
    _aidxres_list_entry_t   offset;   /* working offset in bank this block */
    shr_aidxres_element_t   next;     /* working element number next block */
#ifdef AIDXRES_SANITY_CHECKING
    int result;
#endif /* def AIDXRES_SANITY_CHECKING */

    AIDXRES_DUMP(("_shr_aidxres_list_free(*,%08X) called\n",element));
#ifdef AIDXRES_SANITY_CHECKING
    if ((aidxres_sanity_settings & (AIDXRES_SANITY_POINT_ENTRY |
                                     AIDXRES_SANITY_FUNC_FREE)) ==
        (AIDXRES_SANITY_POINT_ENTRY | AIDXRES_SANITY_FUNC_FREE)) {
        result = _aidxres_list_check(list);
        if (SHR_E_NONE != result) {
            return result;
        }
    }
#endif /* def AIDXRES_SANITY_CHECKING */

    /* validate parameters */
    if ((element < list->valid_low) ||
        (element > list->valid_high)) {
        /* completely invalid parameters */
        return SHR_E_PARAM;
    }

    /* further validation */
    if ((element < list->first) ||
        (element > list->last)) {
        /* trying to free elments not managed by the list */
        return SHR_E_RESOURCE;
    }

    /* remove bias on entry number */
    element -= list->first;

    /* validation that we can free the entry */
    AIDXRES_DUMP(("_shr_aidxres_list_free: verify element is allocated\n"));
    if (AIDXRES_FIRST_ENTRY > list->element[element].sublist) {
        /* this element isn't a member of an allocated block */
        return SHR_E_RESOURCE;
    }
    AIDXRES_DUMP(("_shr_aidxres_list_free: verify element is first\n"));
    if (AIDXRES_FIRST_ENTRY != list->element[element].sublist) {
        /* this element is not first element of an allocated block */
        return SHR_E_PARAM;
    }

    /* determine parameters for this block */
    count = list->element[element].elem_count;
    AIDXRES_DUMP(("_shr_aidxres_list_free: block is %08X elements\n",count));

    /* figure out the maximum offset value within a block */
    max = (1 << (list->sublist_count - 1)) - 1;
    /* offset is within the block, not within the bank */
    offset = AIDXRES_ELEMENT_IDX(element) & max;
    /* compute minimum element number to be in this block */
    min = element & (~max);
    /* now adjust max if we're too close to bank end in last bank */
    if (AIDXRES_ELEMENT_BNK(element) == list->bank_max) {
        /* in last bank; allowed elements may be limited due to bank size */
        if ((AIDXRES_ELEMENT_IDX(element) + max) >= list->bank_rem_max) {
            /* we'd touch end of list; strip it down */
            max &= list->bank_rem_max;
        }
    }
    AIDXRES_DUMP(("_shr_aidxres_list_free: offset = %08X; max = %08X;"
                  " min = %08X\n",
                  offset,
                  max,
                  min));

    /* adjust final free and alloc counts */
    list->free_count += count;
    list->alloc_count -= count;

    /*
     *  First we should try to collect free blocks that are downwardly
     *  adjacent, and combine them to make a larger block if the resulting
     *  block would remain correctly aligned for its new size.
     */
    while ((element > min) &&
           (AIDXRES_FIRST_ENTRY >
            (sublist = list->element[element - 1].sublist)) &&
           (sublist < BYTES2BITS(sizeof(currCnt))) &&
           (count <= (currCnt = 1 << sublist)) &&
           (0 == ((offset - currCnt) & currCnt))
          ) {
        AIDXRES_DUMP(("_shr_aidxres_list_free: collect downwardly adjacent"
                      " %08X at %08X ["
                      AIDXRES_FORMAT_ENTRY
                      "]\n",
                      currCnt,
                      element - currCnt,
                      offset - currCnt));
        /* add the block below to this block */
        _remove_block_from_sublist(list, element - currCnt, sublist);
        offset -= currCnt;
        element -= currCnt;
        count += currCnt;
    }

    /*
     *  Now we need to collect upwardly adjacent blocks, as long as the
     *  combined block is still correctly aligned for the new size.
     */
    AIDXRES_DUMP(("next = %08X, element[%08X].sublist = %02X, count = %08X\n",
                  element + count,
                  element + count,
                  list->element[element + count].sublist,
                  count));
    while ((max >= offset + count) &&
           (0 == (offset & count)) &&
           (AIDXRES_FIRST_ENTRY > (sublist = list->element[(next = element + count)].sublist)) &&
           (sublist < BYTES2BITS(sizeof(currCnt)))
          ) {
        currCnt = 1 << sublist;
        AIDXRES_DUMP(("_shr_aidxres_list_free: collect upwardly adjacent"
                      " %08X at %08X ["
                      AIDXRES_FORMAT_ENTRY
                      "]\n",
                      currCnt,
                      next,
                      offset + count));
        /* add the block above to this block */
        _remove_block_from_sublist(list, next, sublist);
        count += currCnt;
    }

    /*
     *  Finally, at this point, we've combined the block to be freed with
     *  anything that makes sense (that is, might end up reducing the number of
     *  fragments that we would have in this bank after we're done freeing this
     *  block).  Refragment this collected block into the proper lists (it's
     *  quite possible that it is only one fragment at this point due to the
     *  size being a power of two and the alignment being proper, but it's also
     *  possible that it's not a power of two in size and therefore needs to be
     *  fragmented again).
     */
    AIDXRES_DUMP(("_shr_aidxres_list_free: add defragged block to sublist\n"));
    sublist = list->sublist_count - 1;
    submask = 1 << sublist;
    while (count) {
        if (count & submask) {
            /* we have a block of this size; put it on the proper list */
            _add_block_to_sublist(list, element, sublist);
            element += submask;
            count -= submask;
        } else {
            sublist--;
            submask = submask >> 1;
        }
    }

    /* debugging */
    AIDXRES_DUMP_LIST(list);
#ifdef AIDXRES_SANITY_CHECKING
    if ((aidxres_sanity_settings & (AIDXRES_SANITY_POINT_RETURN |
                                     AIDXRES_SANITY_FUNC_FREE)) ==
        (AIDXRES_SANITY_POINT_RETURN | AIDXRES_SANITY_FUNC_FREE)) {
        result = _aidxres_list_check(list);
        if (SHR_E_NONE != result) {
            return result;
        }
    }
#endif /* def AIDXRES_SANITY_CHECKING */

    /* return the actual result */
    return SHR_E_NONE;
}

int
shr_aidxres_list_create(shr_aidxres_list_handle_t *list,
                        shr_aidxres_element_t first,
                        shr_aidxres_element_t last,
                        shr_aidxres_element_t valid_low,
                        shr_aidxres_element_t valid_high,
                        shr_aidxres_element_t block_factor,
                        char *name)
{
    shr_aidxres_list_handle_t work_list; /* working list */
    shr_aidxres_element_t     banks;     /* banks in this list */
    shr_aidxres_element_t     count;     /* elements in this list */
    shr_aidxres_element_t     element;   /* working element number */
    shr_aidxres_element_t     block_size;/* max block size in elements */
#ifdef AIDXRES_SANITY_CHECKING
    int result;
#endif /* def AIDXRES_SANITY_CHECKING */

    /*
     *  Constant initialisers are used rather than setting each field
     *  individually during init since this is usually faster and smaller.
     *  They're in this function because this is the only place we use them.
     */
    const _aidxres_list_sublist_t s_init = {0,
                                            AIDXRES_FINAL_BANK};
    const _aidxres_list_bank_t bl_init = {AIDXRES_USED_BANK,
                                          AIDXRES_USED_BANK};
    const _aidxres_list_bank_sublist_t bs_init = {0,
                                                  AIDXRES_FINAL_ENTRY};
    const _aidxres_list_elemdesc_t e_init = {1,
                                             AIDXRES_USED_ENTRY,
                                             AIDXRES_USED_ENTRY,
                                             AIDXRES_USED_ENTRY};

    AIDXRES_DUMP(("shr_aidxres_list_create(*,%08X,%08X,%08X,%08X,%02X,"
                  "(\"%s\") called\n",
                  first,
                  last,
                  valid_low,
                  valid_high,
                  block_factor,
                  name));
    /* check parameter validity */
    if ((valid_low > first) ||
        (valid_high < last) ||
        (first > last) ||
        (block_factor > AIDXRES_BITS_ENTRY) ||
#if AIDXRES_MAX_BANK
        ((((AIDXRES_MAX_BANK + 1) << AIDXRES_BITS_ENTRY) - 1) < (valid_high - valid_low))
#else
        (AIDXRES_MAX_ENTRY < (valid_high - valid_low))
#endif
        ) {
        /* something's not valid on input */
        return SHR_E_PARAM;
    }

    /* compute the parameters for the memory block */
    count = last - first + 1;
#if AIDXRES_MAX_BANK
    banks = (count + AIDXRES_MAX_ENTRY) >> AIDXRES_BITS_ENTRY;
    if ((AIDXRES_MAX_BANK+1) < banks) {
        /* it's too big still */
        return SHR_E_PARAM;
    }
#else
    banks = 1;
#endif

    /* this is zero based index but we need a one based count from here on */
    block_factor++;

    /* try to allocate enough space for the list */
    
    AIDXRES_DUMP(("shr_aidxres_list_create: Allocate %d byte cell for %s.\n",
                 ((sizeof(_aidxres_list_t) - sizeof(_aidxres_list_sublist_t)) +
                  (block_factor * (sizeof(_aidxres_list_sublist_t))) +
                  ((banks * block_factor) * ((sizeof(_aidxres_list_bank_t)) + sizeof(_aidxres_list_bank_sublist_t))) +
                  (count * sizeof(_aidxres_list_elemdesc_t))),
                 name));
    work_list = sal_alloc(((sizeof(_aidxres_list_t) - sizeof(_aidxres_list_sublist_t)) +
                           (block_factor * (sizeof(_aidxres_list_sublist_t))) +
                           ((banks * block_factor) * ((sizeof(_aidxres_list_bank_t)) + sizeof(_aidxres_list_bank_sublist_t))) +
                           (count * sizeof(_aidxres_list_elemdesc_t))),
                          name);
    if (!work_list) {
        /* unable to allocate the needed memory */
        (*list) = NULL;
        return SHR_E_MEMORY;
    }

#if SHR_AIDXRES_SELF_LOCKING
    /* create and then take the mutex */
    AIDXRES_DUMP(("shr_aidxres_list_create: Create mutex/lock for %s\n",name));
    work_list->lock = sal_mutex_create(name);
    if (!(work_list->lock)) {
        /* unable to create the lock */
        sal_free(work_list);
        (*list) = NULL;
        return SHR_E_RESOURCE;
    }
    AIDXRES_DUMP(("shr_aidxres_list_create: take lock for %s\n",name));
    if (sal_mutex_take(work_list->lock, sal_mutex_FOREVER)) {
        /* Cound not obtain lock */
        sal_mutex_destroy(work_list->lock);
        sal_free(work_list);
        (*list) = NULL;
        return SHR_E_INTERNAL;
    }
#endif /* SHR_AIDXRES_SELF_LOCKING */

    /* initialise the list root */
    AIDXRES_DUMP(("shr_aidxres_list_create: Initialise list parameters\n"));
    work_list->first = first;
    work_list->last = last;
    work_list->valid_low = valid_low;
    work_list->valid_high = valid_high;
    work_list->free_count = count;
    work_list->alloc_count = 0;
    work_list->sublist_count = block_factor;
    work_list->bank_max = banks - 1;
    work_list->bank_rem_max = (count - 1) & AIDXRES_MAX_ENTRY;

    /*
     *  There is a set of linked lists of banks; one per sublist.  These lists
     *  link the banks together that have free blocks of the appropriate size
     *  for each sublist (so a bank will be 'in use' on one of these lists if
     *  it has no free blocks of the appropriate size fo the sublist).
     *
     *  This follows the array of sublist descriptors at the end of the fixed
     *  length part of the list descriptor, so it points to the byte after the
     *  final element of the sublist descriptor array.
     */
    work_list->bank_list = (_aidxres_list_bank_t*)
                            (((uint8_t*)work_list) +
                             (sizeof(_aidxres_list_t) - sizeof(_aidxres_list_sublist_t)) +
                             (sizeof(_aidxres_list_sublist_t) * block_factor));

    /*
     *  There is a set of linked list descriptors, one per sublist per bank.
     *  These descriptors specify which blocks in each bank are on each
     *  sublist, and provide access to further such blocks.
     *
     *  This follows the linked lists of banks per sublist, above, so it points
     *  to the byte after the final element in those lists.
     */
    work_list->bank_sublist = (_aidxres_list_bank_sublist_t*)
                              (((uint8_t*)(work_list->bank_list)) +
                               (sizeof(_aidxres_list_bank_t) *
                                (banks * block_factor)));

    /*
     *  There is a set of element descriptors, one per element.  These are
     *  connected in linked lists per sublist within each bank (they do not
     *  cross bank boundaries).
     *
     *  This follows the set of bank sublist descriptors.
     */
    work_list->element = (_aidxres_list_elemdesc_t*)
                         (((uint8_t*)(work_list->bank_sublist)) +
                          (sizeof(_aidxres_list_bank_sublist_t) *
                           (banks * block_factor)));

    /* debugging */
    AIDXRES_DUMP_LIST(work_list);

    /* initialise the sublist information */
    AIDXRES_DUMP(("shr_aidxres_list_create: initialise sublist data (%02X)\n",
                  block_factor));
    for (element = 0; element < block_factor; element++) {
        work_list->sublist[element] = s_init;
    }

    /* initialise the bank sublist information */
    AIDXRES_DUMP(("shr_aidxres_list_create: initialise bank sublist data"
                  " (%08X)\n",
                  banks * block_factor));
    for (element = banks * block_factor; element > 0; /* don't decr here! */) {
        element--; /* decrement must come here */
        work_list->bank_sublist[element] = bs_init;
        work_list->bank_list[element] = bl_init;
    }

    /* initialise the bank internal lists */
    AIDXRES_DUMP(("shr_aidxres_list_create: initialise bank elements data"
                  " (%08X)\n",
                  count));
    for (element = count; element > 0; /* don't decrement here! */) {
        element--; /* decrement must come here */
        work_list->element[element] = e_init;
    }

    /* debugging */
    AIDXRES_DUMP_LIST(work_list);

    /*
     *  Place the elements into blocks, preferring to make the blocks as large
     *  as possible, but collecting remnants into smaller blocks as needed.
     *
     *  In general, all but the last bank of elements should be in largest
     *  possible blocks, and the last bank should have no more than one block
     *  of each size smaller than the largest possible (unless it is also
     *  exactly the largest size, in which case it's same as other banks).
     */
    AIDXRES_DUMP(("shr_aidxres_list_create:"
                  " place elements into free blocks\n"));
    block_factor--; /* need original value here */
    block_size = 1 << block_factor;
    element = 0;
    while (count) {
        /* there are more elements to be assigned to lists */
        if (count >= block_size) {
            /* there's a block of this size; add that block */
            AIDXRES_DUMP(("shr_aidxres_list_create: put %08X of %08X elements"
                          " into block\n",
                          block_size,
                          count));
            _add_block_to_sublist(work_list, element, block_factor);
            element += block_size;
            count -= block_size;
        } else { /* if (count >= block_size) */
            /* no block of this size; try a smaller one */
            block_factor--;
            block_size = block_size >> 1;
        } /* if (count >= block_size) */
    } /* while (count) */

    /* debugging */
    AIDXRES_DUMP_LIST(work_list);
#ifdef AIDXRES_SANITY_CHECKING
    if ((aidxres_sanity_settings & (AIDXRES_SANITY_POINT_RETURN |
                                     AIDXRES_SANITY_FUNC_CREATE)) ==
        (AIDXRES_SANITY_POINT_RETURN | AIDXRES_SANITY_FUNC_CREATE)) {
        result = _aidxres_list_check(work_list);
        if (SHR_E_NONE != result) {
            return result;
        }
    }
#endif /* def AIDXRES_SANITY_CHECKING */

#if SHR_AIDXRES_SELF_LOCKING
    /* release the lock now */
    AIDXRES_DUMP(("shr_aidxres_list_create: release lock\n"));
    if (sal_mutex_give(work_list->lock)) {
        /* could not release lock */
        sal_mutex_destroy(work_list->lock);
        sal_free(work_list);
        (*list) = NULL;
        return SHR_E_INTERNAL;
    }
#endif /* SHR_AIDXRES_SELF_LOCKING */

    /* all done */
    AIDXRES_DUMP(("shr_aidxres_list_create: return\n"));
    (*list) = work_list;
    return SHR_E_NONE;
}

int
shr_aidxres_list_destroy(shr_aidxres_list_handle_t list)
{
    shr_aidxres_element_t block_factor;
    shr_aidxres_element_t banks;
    shr_aidxres_element_t count;

    AIDXRES_DUMP(("shr_aidxres_list_destroy(*) called\n"));
    /* check parameter validity */
    if (!list) {
        return SHR_E_PARAM;
    }

#if SHR_AIDXRES_SELF_LOCKING
    /* destroy the semaphore */
    AIDXRES_DUMP(("shr_aidxres_list_destroy: destroy lock\n"));
    sal_mutex_destroy(list->lock);
#endif /* SHR_AIDXRES_SELF_LOCKING */

    /* cache some data about the list */
    block_factor = list->sublist_count;
    banks = list->bank_max + 1;
    count = (list->last - list->first) + 1;

    /* poison the list */
    
    AIDXRES_DUMP(("shr_aidxres_list_destroy: obliterate list data (%d"
                  " bytes)\n",
                  ((sizeof(_aidxres_list_t) - sizeof(_aidxres_list_sublist_t)) +
                   (block_factor * (sizeof(_aidxres_list_sublist_t))) +
                   ((banks * block_factor) * ((sizeof(_aidxres_list_bank_t)) + sizeof(_aidxres_list_bank_sublist_t))) +
                   (count * sizeof(_aidxres_list_elemdesc_t)))));
    sal_memset(list,
               0x00,
               ((sizeof(_aidxres_list_t) - sizeof(_aidxres_list_sublist_t)) +
                (block_factor * (sizeof(_aidxres_list_sublist_t))) +
                ((banks * block_factor) * ((sizeof(_aidxres_list_bank_t)) + sizeof(_aidxres_list_bank_sublist_t))) +
                (count * sizeof(_aidxres_list_elemdesc_t))));

    /* now free the list */
    AIDXRES_DUMP(("shr_aidxres_list_destroy: free list memory cell\n"));
    sal_free(list);

    /* all done */
    AIDXRES_DUMP(("shr_aidxres_list_destroy: return\n"));
    return SHR_E_NONE;
}

int
shr_aidxres_list_alloc(shr_aidxres_list_handle_t list,
                       shr_aidxres_element_t *element)
{
    int            result;   /* value to be returned to caller */

    /* validate parameters */
    if (!list) {
        return SHR_E_PARAM;
    }

#if SHR_AIDXRES_SELF_LOCKING
    AIDXRES_DUMP(("shr_aidxres_list_alloc: take lock\n"));
    /* claim the lock for the list */
    if (sal_mutex_take(list->lock, sal_mutex_FOREVER)) {
        /* Cound not obtain lock  */
        return SHR_E_INTERNAL;
    }
#endif /* SHR_AIDXRES_SELF_LOCKING */

    /* allocate an element */
    result = _shr_aidxres_list_alloc(list, 1, element);

#if SHR_AIDXRES_SELF_LOCKING
    AIDXRES_DUMP(("shr_aidxres_list_alloc: release lock\n"));
    /* release the lock for the list */
    if (sal_mutex_give(list->lock)) {
        /* could not release lock */
        return SHR_E_INTERNAL;
    }
#endif /* SHR_AIDXRES_SELF_LOCKING */

    /* return the actual result */
    return result;
}

int
shr_aidxres_list_alloc_set(shr_aidxres_list_handle_t list,
                           shr_aidxres_element_t count,
                           shr_aidxres_element_t *elements,
                           shr_aidxres_element_t *done)
{
    int            result;   /* value to be returned to caller */

    /* validate parameters */
    if ((!list) || (!elements) || (!done)) {
        return SHR_E_PARAM;
    }

    /* set initial conditions */
    (*done) = 0;
    result = SHR_E_NONE;

#if SHR_AIDXRES_SELF_LOCKING
    AIDXRES_DUMP(("shr_aidxres_list_alloc_set: take lock\n"));
    /* claim the lock for the list */
    if (sal_mutex_take(list->lock, sal_mutex_FOREVER)) {
        /* Cound not obtain lock  */
        return SHR_E_INTERNAL;
    }
#endif /* SHR_AIDXRES_SELF_LOCKING */

    /* make sure we have enough free elements */
    if (list->free_count < count) {
        /* not enough free elements to comply */
        result = SHR_E_RESOURCE;
    }

    /* allocate elements */
    while ((0 < count) && (SHR_E_NONE == result)) {
        /* allocate this element */
        result = _shr_aidxres_list_alloc(list, 1, elements);
        if (SHR_E_NONE == result) {
            /* success; update accounting */
            elements++;
            count--;
            (*done)++;
        }
    }

#if SHR_AIDXRES_SELF_LOCKING
    AIDXRES_DUMP(("shr_aidxres_list_alloc_set: release lock\n"));
    /* release the lock for the list */
    if (sal_mutex_give(list->lock)) {
        /* could not release lock */
        return SHR_E_INTERNAL;
    }
#endif /* SHR_AIDXRES_SELF_LOCKING */

    /* return the actual result */
    return result;
}

int
shr_aidxres_list_alloc_block(shr_aidxres_list_handle_t list,
                             shr_aidxres_element_t count,
                             shr_aidxres_element_t *element)
{
    int            result;   /* value to be returned to caller */

    /* validate parameters */
    if (!list) {
        return SHR_E_PARAM;
    }

#if SHR_AIDXRES_SELF_LOCKING
    AIDXRES_DUMP(("shr_aidxres_list_alloc_block: take lock\n"));
    /* claim the lock for the list */
    if (sal_mutex_take(list->lock, sal_mutex_FOREVER)) {
        /* Cound not obtain lock  */
        return SHR_E_INTERNAL;
    }
#endif /* SHR_AIDXRES_SELF_LOCKING */

    /* allocate an element */
    result = _shr_aidxres_list_alloc(list, count, element);

#if SHR_AIDXRES_SELF_LOCKING
    AIDXRES_DUMP(("shr_aidxres_list_alloc_block: release lock\n"));
    /* release the lock for the list */
    if (sal_mutex_give(list->lock)) {
        /* could not release lock */
        return SHR_E_INTERNAL;
    }
#endif /* SHR_AIDXRES_SELF_LOCKING */

    /* return the actual result */
    return result;
}

int
shr_aidxres_list_free(shr_aidxres_list_handle_t list,
                      shr_aidxres_element_t element)
{
    int            result;   /* value to be returned to caller */

    /* validate parameters */
    if (!list) {
        /* completely invalid parameters */
        return SHR_E_PARAM;
    }

#if SHR_AIDXRES_SELF_LOCKING
    AIDXRES_DUMP(("shr_aidxres_list_free: take lock\n"));
    /* claim the lock for the list */
    if (sal_mutex_take(list->lock, sal_mutex_FOREVER)) {
        /* Cound not obtain lock  */
        return SHR_E_INTERNAL;
    }
#endif /* SHR_AIDXRES_SELF_LOCKING */

    result = _shr_aidxres_list_free(list, element);

#if SHR_AIDXRES_SELF_LOCKING
    AIDXRES_DUMP(("shr_aidxres_list_free: release lock\n"));
    /* release the lock for the list */
    if (sal_mutex_give(list->lock)) {
        /* could not release lock */
        return SHR_E_INTERNAL;
    }
#endif /* SHR_AIDXRES_SELF_LOCKING */

    /* return the actual result */
    return result;
}

int
shr_aidxres_list_free_set(shr_aidxres_list_handle_t list,
                          shr_aidxres_element_t count,
                          shr_aidxres_element_t *elements,
                          shr_aidxres_element_t *done)
{
    int            result;   /* value to be returned to caller */

    /* validate parameters */
    if ((!list) || (!elements) || (!done)) {
        return SHR_E_PARAM;
    }

    /* set initial conditions */
    (*done) = 0;
    result = SHR_E_NONE;

#if SHR_AIDXRES_SELF_LOCKING
    AIDXRES_DUMP(("shr_aidxres_list_free_set: take lock\n"));
    /* claim the lock for the list */
    if (sal_mutex_take(list->lock, sal_mutex_FOREVER)) {
        /* Cound not obtain lock  */
        return SHR_E_INTERNAL;
    }
#endif /* SHR_AIDXRES_SELF_LOCKING */

    /* free elements */
    while ((0 < count) && (SHR_E_NONE == result)) {
        /* free this element */
        result = _shr_aidxres_list_free(list, *elements);
        if (SHR_E_NONE == result) {
            /* success; update accounting */
            elements++;
            count--;
            (*done)++;
        }
    }

#if SHR_AIDXRES_SELF_LOCKING
    AIDXRES_DUMP(("shr_aidxres_list_free_set: release lock\n"));
    /* release the lock for the list */
    if (sal_mutex_give(list->lock)) {
        /* could not release lock */
        return SHR_E_INTERNAL;
    }
#endif /* SHR_AIDXRES_SELF_LOCKING */

    /* return the actual result */
    return result;
}

int
shr_aidxres_list_state(shr_aidxres_list_handle_t list,
                       shr_aidxres_element_t *first,
                       shr_aidxres_element_t *last,
                       shr_aidxres_element_t *valid_low,
                       shr_aidxres_element_t *valid_high,
                       shr_aidxres_element_t *free_count,
                       shr_aidxres_element_t *alloc_count,
                       shr_aidxres_element_t *largest_free,
                       shr_aidxres_element_t *block_factor)
{
    _aidxres_list_entry_t sublist;

    if (!list) {
        /* the list has to be valid */
        return SHR_E_PARAM;
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
        (*free_count) = list->free_count;
    }
    if (alloc_count) {
        (*alloc_count) = list->alloc_count;
    }
    if (block_factor) {
        (*block_factor) = list->sublist_count - 1;
    }

    /* compute the values requested by the caller & return them */
    if (largest_free) {
        *largest_free = 0;
        if (list->free_count) {
            /* there are free elements; scan the sublists */
            for (sublist = 0; sublist < (list->sublist_count); sublist++) {
                /* check sublists in increasing order of block size */
                if (list->sublist[sublist].free_count) {
                    /* this sublist has at least one free block; keep it */
                    *largest_free = 1 << sublist;
                }
            } /* for (iterate sublists in increasing order) */
        } /* if (list->free_count) */
    } /* if (largest_free) */

    /* indicate success to the caller */
    return SHR_E_NONE;
}

int
shr_aidxres_list_elem_state(shr_aidxres_list_handle_t list,
                            shr_aidxres_element_t element)
{
    /* validate parameters */
    if ((!list) ||
        (element < list->valid_low) ||
        (element > list->valid_high)) {
        /* completely invalid parameters */
        return SHR_E_PARAM;
    }

    /* further validation */
    if ((element < list->first) ||
        (element > list->last)) {
        /* getting state for elments not managed by the list (in use) */
        return SHR_E_EXISTS;
    }

    /* remove bias on entry number */
    element -= list->first;

    /* get the element state and parse it */
    if (AIDXRES_FIRST_ENTRY <= list->element[element].sublist) {
        /* this element is a member of an allocated block */
        return SHR_E_EXISTS;
    }
    if (list->sublist_count <= list->element[element].sublist) {
        /* there's a problem with this element (invalid sublist) */
        return SHR_E_INTERNAL;
    }
    /* it's on a valid sublist, so it's not in use */
    return SHR_E_NOT_FOUND;
}

int
shr_aidxres_list_block_state(shr_aidxres_list_handle_t list,
                             shr_aidxres_element_t element,
                             shr_aidxres_element_t size)
{
    shr_aidxres_element_t index;
    int result = SHR_E_EMPTY;

    /* validate parameters */
    if ((!list) ||
        (element < list->valid_low) ||
        (element + size > list->valid_high)) {
        /* completely invalid parameters */
        return SHR_E_PARAM;
    }
    if ((element + size < list->first) ||
        (element > list->last)) {
        /* entire requested block is not managed by the list (so in use) */
        /* no way to tell anything about blocks in this space */
        return SHR_E_FULL;
    }
    if (((element < list->first) &&
         (element + size >= list->first)) ||
        ((element <= list->last) &&
         (element + size > list->last))) {
        /* block spans a managed boundary; inconsistent block contents */
        return SHR_E_CONFIG;
    }

    /* remove bias on entry number */
    element -= list->first;

#if SHR_AIDXRES_SELF_LOCKING
    /* claim the lock for the list */
    if (sal_mutex_take(list->lock, sal_mutex_FOREVER)) {
        /* Cound not obtain lock  */
        return SHR_E_INTERNAL;
    }
#endif /* SHR_AIDXRES_SELF_LOCKING */

    /* see if the block is as specified */
    if (AIDXRES_FIRST_ENTRY == list->element[element].sublist) {
        /* element specified is first of block */
        if (size == list->element[element].elem_count) {
            /* block is expected size */
            result = SHR_E_FULL;
        } else {
            /* block is not expected size */
            result = SHR_E_CONFIG;
        }
    } else if (AIDXRES_FIRST_ENTRY < list->element[element].sublist) {
        /* element is allocated but not the first element in the block */
        result = SHR_E_CONFIG;
    } else if (list->sublist_count <= list->element[element].sublist) {
        /* corruption */
        result = SHR_E_INTERNAL;
    } else {
        /* checked the first element and it is free; scan the rest */
        for (index = 1; index < size; index++) {
            if (AIDXRES_FIRST_ENTRY <= list->element[element + index].sublist) {
                /* this element is allocated */
                result = SHR_E_EXISTS;
            } else if (list->sublist_count <= list->element[element + index].sublist) {
                /* corruption */
                result = SHR_E_INTERNAL;
                break;
            }
        }
    }

#if SHR_AIDXRES_SELF_LOCKING
    /* release the lock for the list */
    if (sal_mutex_give(list->lock)) {
        /* could not release lock */
        return SHR_E_INTERNAL;
    }
#endif /* SHR_AIDXRES_SELF_LOCKING */
    return result;
}

int
shr_aidxres_list_reserve(shr_aidxres_list_handle_t list,
                         shr_aidxres_element_t first,
                         shr_aidxres_element_t last)
{
    shr_error_t           result;     /* value to be returned to caller */
    shr_aidxres_element_t curr_elem;  /* working current element address */
    shr_aidxres_element_t first_elem; /* first element of current block */
    shr_aidxres_element_t last_elem;  /* last element of current block */
    _aidxres_list_entry_t sublist;    /* sublist number for this block */
    _aidxres_list_entry_t blk_size;   /* how big is this block */
    _aidxres_list_entry_t blk_mask;   /* bits within this block */

    AIDXRES_DUMP(("shr_aidxres_list_reserve(*,%08X,%08X) called\n",
                  first,
                  last));
    /* validate parameters */
    if ((!list) ||
        (first < list->valid_low) ||
        (last > list->valid_high) ||
        (last < first)) {
        return SHR_E_PARAM;
    }

    /* ensure the requrested range is entirely allocatable */
    if ((first < list->first) ||
        (last > list->last)) {
        return SHR_E_RESOURCE;
    }

#if SHR_AIDXRES_SELF_LOCKING
    /* claim the lock for the list */
    AIDXRES_DUMP(("shr_aidxres_list_reserve: take lock\n"));
    if (sal_mutex_take(list->lock, sal_mutex_FOREVER)) {
        /* Cound not obtain lock  */
        return SHR_E_INTERNAL;
    }
#endif /* SHR_AIDXRES_SELF_LOCKING */

    /* remove bias on range */
    first -= list->first;
    last -= list->first;

    /* scan the entire range to ensure availability */
    result = SHR_E_NONE;
    AIDXRES_DUMP(("shr_aidxres_list_reserve: checking that elements %08X..%08X"
                  " are free\n",
                  first,
                  last));
    for (curr_elem = first; curr_elem <= last; curr_elem++) {
        if (AIDXRES_FIRST_ENTRY <= list->element[curr_elem].sublist) {
            /* at least one element is not available */
            result = SHR_E_RESOURCE;
            break;
        }
    }

    /* if all is well so far, reserve the entries in question */
    if (SHR_E_NONE == result) {
        /* need to reserve the entries */
        for (curr_elem = first; curr_elem <= last; /* no incr here */ ) {
            sublist = list->element[curr_elem].sublist;
            blk_size = 1 << sublist;
            blk_mask = blk_size - 1;
            /* claim this block */
            first_elem = curr_elem & (~blk_mask);
            last_elem = first_elem + blk_mask;
            AIDXRES_DUMP(("shr_aidxres_list_reserve: claiming block with"
                          " elements %08X..%08X\n",
                          first_elem,
                          last_elem));
            _remove_block_from_sublist(list, first_elem, sublist);
            /* free any unwanted low elements */
            blk_size = curr_elem - first_elem;
            while (blk_size) {
                sublist--;
                blk_mask = 1 << sublist;
                AIDXRES_DUMP(("shr_aidxres_list_reserve: try to return %08X of"
                              " %08X lead-in elements at %08X\n",
                              blk_mask,
                              blk_size,
                              first_elem));
                if (blk_size & blk_mask) {
                    _add_block_to_sublist(list, first_elem, sublist);
                    blk_size = blk_size - blk_mask;
                    first_elem = first_elem + blk_mask;
                }
            } /* while (blk_size) */
            /* set any wanted elements as in use */
            AIDXRES_DUMP(("shr_aidxres_list_reserve: marking wanted elements"
                          " %08X..%08X as in use\n",
                          curr_elem,
                          (last<last_elem)?last:last_elem));
            while ((curr_elem <= last) && (curr_elem <= last_elem)) {
                list->element[curr_elem].elem_count = 1;
                list->element[curr_elem].sublist = AIDXRES_FIRST_ENTRY;
                curr_elem++;
            } /* while ((curr_elem <= last) && (curr_elem <= last_elem)) */
            /* free any unwanted high elements */
            blk_size = last_elem - curr_elem + 1;
            sublist = 0;
            while (blk_size) {
                blk_mask = 1 << sublist;
                AIDXRES_DUMP(("shr_aidxres_list_reserve: try to return %08X of"
                              " %08X lead-out elements at %08X\n",
                              blk_mask,
                              blk_size,
                              curr_elem));
                if (blk_size & blk_mask) {
                    _add_block_to_sublist(list, curr_elem, sublist);
                    blk_size = blk_size - blk_mask;
                    curr_elem = curr_elem + blk_mask;
                }
                sublist++;
            } /* while (blk_size) */
        } /* for (curr_elem = first; curr_elem <= last;) */
        /* adjust list metadata */
        AIDXRES_DUMP(("shr_aidxres_list_reserve: adjust list allocated and"
                      " free counts\n"));
        list->alloc_count += last - first + 1;
        list->free_count -= last - first + 1;
    } /* if (SHR_E_NONE == result) */

#if SHR_AIDXRES_SELF_LOCKING
    /* release the lock for the list */
    AIDXRES_DUMP(("shr_aidxres_list_reserve: release lock\n"));
    if (sal_mutex_give(list->lock)) {
        /* could not release lock */
        return SHR_E_INTERNAL;
    }
#endif /* SHR_AIDXRES_SELF_LOCKING */

    /* debugging */
    AIDXRES_DUMP_LIST(list);

    /* return the actual result */
    return result;
}

int
shr_aidxres_list_reserve_block(shr_aidxres_list_handle_t list,
                               shr_aidxres_element_t first,
                               shr_aidxres_element_t count)
{
    shr_error_t           result;     /* value to be returned to caller */
    shr_aidxres_element_t curr_elem;  /* working current element address */
    shr_aidxres_element_t first_elem; /* first element of current block */
    shr_aidxres_element_t last_elem;  /* last element of current block */
    shr_aidxres_element_t last;       /* last element of block to reserve */
    _aidxres_list_entry_t sublist;    /* sublist number for this block */
    _aidxres_list_entry_t blk_size;   /* how big is this block */
    _aidxres_list_entry_t blk_mask;   /* bits within this block */

    last = (first + count - 1);
    AIDXRES_DUMP(("shr_aidxres_list_reserve_block(*,%08X,%08X) called\n",
                  first,
                  count));
    /* validate parameters */
    if ((!list) ||
        (first < list->valid_low) ||
        (last > list->valid_high) ||
        (count > ((uint32_t)1 << (list->sublist_count - 1)))) {
        return SHR_E_PARAM;
    }

    /* ensure the requrested range is entirely allocatable */
    if ((first < list->first) ||
        (last > list->last)) {
        return SHR_E_RESOURCE;
    }

    /* remove bias on range */
    first -= list->first;
    last -= list->first;

    /* verify block alignment */
    AIDXRES_DUMP(("shr_aidxres_list_reserve_block: verify block legality\n"));
    sublist = 0;
    blk_mask = 1;
    while (blk_mask < count) {
        sublist++;
        blk_mask = blk_mask << 1;
    }
    AIDXRES_DUMP(("shr_aidxres_list_reserve_block: sublist %02x, mask %08X\n",
                  sublist,
                  blk_mask));
    if (first & (blk_mask - 1)) {
        /* the block is not aligned properly */
        return SHR_E_PARAM;
    }

#if SHR_AIDXRES_SELF_LOCKING
    /* claim the lock for the list */
    AIDXRES_DUMP(("shr_aidxres_list_reserve_block: take lock\n"));
    if (sal_mutex_take(list->lock, sal_mutex_FOREVER)) {
        /* Cound not obtain lock  */
        return SHR_E_INTERNAL;
    }
#endif /* SHR_AIDXRES_SELF_LOCKING */

    /* scan the entire range to ensure availability */
    result = SHR_E_NONE;
    AIDXRES_DUMP(("shr_aidxres_list_reserve_block: checking that elements %08X..%08X"
                  " are free\n",
                  first,
                  last));
    for (curr_elem = first; curr_elem <= last; curr_elem++) {
        if (AIDXRES_FIRST_ENTRY <= list->element[curr_elem].sublist) {
            /* at least one element is not available */
            result = SHR_E_RESOURCE;
            break;
        }
    }

    /* if all is well so far, reserve the entries in question */
    if (SHR_E_NONE == result) {
        /* need to reserve the entries */
        for (curr_elem = first; curr_elem <= last; /* no incr here */ ) {
            sublist = list->element[curr_elem].sublist;
            blk_size = 1 << sublist;
            blk_mask = blk_size - 1;
            /* claim this block */
            first_elem = curr_elem & (~blk_mask);
            last_elem = first_elem + blk_mask;
            AIDXRES_DUMP(("shr_aidxres_list_reserve_block: claiming block with"
                          " elements %08X..%08X\n",
                          first_elem,
                          last_elem));
            _remove_block_from_sublist(list, first_elem, sublist);
            /* free any unwanted low elements */
            blk_size = curr_elem - first_elem;
            while (blk_size) {
                sublist--;
                blk_mask = 1 << sublist;
                AIDXRES_DUMP(("shr_aidxres_list_reserve_block: try to return"
                              " %08X of %08X lead-in elements at %08X\n",
                              blk_mask,
                              blk_size,
                              first_elem));
                if (blk_size & blk_mask) {
                    _add_block_to_sublist(list, first_elem, sublist);
                    blk_size = blk_size - blk_mask;
                    first_elem = first_elem + blk_mask;
                }
            } /* while (blk_size) */
            /* don't bother labelling here; use block label below */
            curr_elem = ((last<last_elem)?last:last_elem) + 1;
            /* free any unwanted high elements */
            blk_size = last_elem - curr_elem + 1;
            sublist = 0;
            while (blk_size) {
                blk_mask = 1 << sublist;
                AIDXRES_DUMP(("shr_aidxres_list_reserve_block: try to return"
                              " %08X of %08X lead-out elements at %08X\n",
                              blk_mask,
                              blk_size,
                              curr_elem));
                if (blk_size & blk_mask) {
                    _add_block_to_sublist(list, curr_elem, sublist);
                    blk_size = blk_size - blk_mask;
                    curr_elem = curr_elem + blk_mask;
                }
                sublist++;
            } /* while (blk_size) */
        } /* for (curr_elem = first; curr_elem <= last;) */
        /* mark the requested block as in use */
        AIDXRES_DUMP(("shr_aidxres_list_reserve_block: marking block containing"
                      " elements %08X..%08X as in use\n",
                      first,
                      last));
        _prep_block_for_alloc(list, first, count);
        /* adjust list metadata */
        AIDXRES_DUMP(("shr_aidxres_list_reserve_block: adjust list allocated"
                      " and free counts\n"));
        list->alloc_count += count;
        list->free_count -= count;
    } /* if (SHR_E_NONE == result) */

#if SHR_AIDXRES_SELF_LOCKING
    /* release the lock for the list */
    AIDXRES_DUMP(("shr_aidxres_list_reserve_block: release lock\n"));
    if (sal_mutex_give(list->lock)) {
        /* could not release lock */
        return SHR_E_INTERNAL;
    }
#endif /* SHR_AIDXRES_SELF_LOCKING */

    /* debugging */
    AIDXRES_DUMP_LIST(list);

    /* return the actual result */
    return result;
}


