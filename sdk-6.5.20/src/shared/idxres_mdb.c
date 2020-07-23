/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Module: Indexed Resource management, MDB model
 */

#include <shared/bsl.h>

#include <sal/types.h>
#include <shared/error.h>
#include <shared/idxres_mdb.h>

/* FIXME: No SOC headers in SHR !!! */
#include <soc/cm.h>
#include <soc/drv.h>

/*
 *  This resource manager divides an indexed resource into elements, collecting
 *  those elements into banks (to reduce the space required to represent a
 *  single element).  It tracks lists of free elements and user defined lists
 *  of elements.
 *
 *  The bank size is configurable in powers of two elements, up to 32768
 *  elements per bank.  Blocks can not cross bank boundaries.  Useful bank
 *  sizes that are supported are 128, 256, 512, 1024, 2048, 4096, 8192, 16384,
 *  and 32768; there may be good argument for smaller banks (such as finer
 *  grains when using the 'low' and 'high' alloc methods), but there is a
 *  certain amount of base overhead per bank (plus a certain amount per list
 *  per bank, whether the list is a user list or a free list), so it seems that
 *  banks should be as large as reasonable for the purpose.  Also, there is
 *  likely to be a bank size that seems 'most optimal' or, at least, 'least
 *  pessimal' for a given maximum block size -- for example, 4096 element
 *  banks provides the least waste for the case of largest block = 132 elems.
 *
 *  The free lists include a required single-element block list, and arbitrary
 *  element count block lists (up to the bank size).  These are arbitrary
 *  instead of power of two in order to improve efficiency in cases where
 *  element counts are likely (for example) to be a bit larger than the nearest
 *  power of two.  Again, using 132 element largest blocks as an example, and
 *  knowing (or just assuming) that some other likely sizes are 1, 2, 3, 4, 5,
 *  16, 28, 50, and 98, you might choose those as the free list block sizes.
 *  It should be pointed out though, that if using the join on alloc feature,
 *  higher efficiency can be obtained by also having some free lists for block
 *  sizes in excess of the largest expected block (so maybe add 256, 512, 1024,
 *  and 2048 element block free lists if using 2048 or 4096 element banks),
 *  though this may have minimal impact if not using the join on alloc feature.
 *
 *  The user lists can contain blocks of any size that is desired by the user
 *  (within the constraint that any single block must exist entirely within a
 *  single bank), but the user lists are NOT sorted, NOR are they ordered.  It
 *  is absolutely NOT guaranteed that blocks will appear in a traversal of a
 *  user list in anything even vaguely resembling the order in which those
 *  blocks were added to the user list (most likely they will not).  This is
 *  due to the way the lists are stored (the compressed form of the elements).
 *
 *  Blocks are not required to be in a list.  It is perfectly reasonable that
 *  the user lists have special meaning and the general case is that allocated
 *  blocks are floating (as long as the user knows about them).  However, there
 *  is no 'non-list', so blocks that are not in a list will not have
 *  predecessors or successors, nor will any list information be associated
 *  with the block.
 */

/*****************************************************************************
 *
 *  Implementation
 *
 *  All of this stuff is private to this module; most of it has been written to
 *  assume everything is perfect and acceptable (to improve performance).
 *
 *  Functions in this section are generally very simple in their actual nature,
 *  and mostly commit entire operations (no partial updates or leakage).
 *
 *  Block base is zero-based for these functions.
 */


#define _MDB_BLOCK_END 0xFFFF           /* last block this bank this list */
#define _MDB_BLOCK_NOT_HEAD 0xFFFE      /* element is not head of a block */
#define _MDB_BLOCK_NOT_IN_LIST 0xFFFD   /* block is not in a list */
#define _MDB_BLOCK_INVALID 0xFFF0       /* first invalid offset */
#define _MDB_BANK_END 0xFFFF            /* last bank in list */
#define _MDB_BANK_NOT_PRESENT 0xFFFE    /* bank not present in list */

#define _MDB_VALID_CHECK(_mdb) \
    if (!(_mdb)) { \
        LOG_ERROR(BSL_LS_SOC_COMMON, \
                  (BSL_META("NULL is not a valid handle\n"))); \
        return _SHR_E_PARAM; \
    }
#define _MDB_LOCK_TAKE(_mdb) \
    if ((_mdb)->lock) { \
        if (sal_mutex_take((_mdb)->lock,sal_mutex_FOREVER)) { \
            LOG_ERROR(BSL_LS_SOC_COMMON, \
                      (BSL_META("unable to take mdb %08X lock\n"), \
                       PTR_TO_INT(_mdb)));                         \
            return _SHR_E_INTERNAL; \
        } \
    }
#define _MDB_LOCK_GIVE(_mdb) \
    if ((_mdb)->lock) { \
        if (sal_mutex_give((_mdb)->lock)) { \
            LOG_ERROR(BSL_LS_SOC_COMMON, \
                      (BSL_META("unable to release mdb %08X lock\n"), \
                       PTR_TO_INT(_mdb)));                            \
            return _SHR_E_INTERNAL; \
        } \
    }

/*
 *  This describes a single element.  The fields have different meanings
 *  depending upon whether the element is the head of a block or one of the
 *  other elements in a block (blocks of one element only have a head).
 *
 *  For the head element:
 *    List indicates the list to which the block belongs.
 *    Count indicates the number of elements in the block.
 *    Next points to the next block in the list on this page.
 *    Prev points to the previous block in the list on this page.
 *
 *  For non-head and non-tail elements:
 *    List indicates the element is not a head element.
 *    Count is zero.
 *    Next is reserved.
 *    Prev is reserved.
 *
 *  For the tail element:
 *    List indicates the element is not a head element.
 *    Count indicates the number of elements in the block.
 *    Next is reserved.
 *    Prev is reserved.
 *
 *  This layout basically means that manipulation of blocks is most efficient
 *  when it is done with the head element; any other element means that a
 *  second read (or more generally a search) must be performed to find the head
 *  element of the block.  This behaviour reduces the updates necessary by not
 *  requiring blocks to be aligned to a power of two elements that is equal to
 *  or larger than the number of elements in the block, and should not impact
 *  performance in the general case, as the head element in a block is the
 *  element which serves as the handle for that block.
 *
 *  The extension of the tail element containing accurate count information is
 *  to improve performance of the 'join downward' alloc/free features.
 */
/* Alignment is 64b */
typedef struct shr_mdb_elem_desc_s {
    shr_mdb_elem_bank_index_t list;       /* list of which block is a member */
    shr_mdb_elem_bank_index_t count;      /* elements in this block */
    shr_mdb_elem_bank_index_t next;       /* next block this list-bank */
    shr_mdb_elem_bank_index_t prev;       /* prev block this list-bank */
} shr_mdb_elem_desc_t;

/*
 *  This describes a page within a single list.  Lists are divivded into
 *  sublists per bank, so that allocation can be reasonably fast when it is
 *  requested that the allocation be performed preferring low or high element
 *  rather than fastest available fit.
 *
 *  For a bank that is in a list:
 *    Head is the head of the first block in this bank on the list.
 *    Tail is the head of the last block in this bank on the list.
 *    Elems is the number of elements in this bank on the list.
 *    Blocks is the number of blocks in this bank on the list.
 *    Next is the next bank participating in this list.
 *    Prev is the previous bank participating in this list.
 *
 *  For a bank that is not in a list:
 *    Head is reserved.
 *    Tail is reserved.
 *    Elems is zero.
 *    Blocks is zero.
 *    Next is reserved.
 *    Prev is reserved.
 */
/* Alignment is 96b */
typedef struct shr_mdb_list_bank_desc_s {
    shr_mdb_elem_bank_index_t head;            /* first block this list-bank */
    shr_mdb_elem_bank_index_t tail;            /* last block this list-bank */
    shr_mdb_elem_bank_index_t elems;           /* elements this list-bank */
    shr_mdb_elem_bank_index_t blocks;          /* blocks this list-bank */
    shr_mdb_bank_index_t next;                 /* next bank this list */
    shr_mdb_bank_index_t prev;                 /* previous bank this list */
} shr_mdb_list_bank_desc_t;

/*
 *  This describes a list, as it covers the whole space.  Each list is always
 *  assumed to exist at this level, even if it contains no elements.
 *
 *  For a free list:
 *    Elems is the number of elements in the list.
 *    Blocks is the number of blocks in the list.
 *    Head is the first bank in the list.
 *    Tail is the last bank in the list.
 *    ElemsBlock is the number of elements per block on this list.
 *    Reserved is reserved (used for alignment).
 *
 *  For a user list:
 *    Elems is the number of elements in the list.
 *    Blocks is the number of blocks in the list.
 *    Head is the first bank in the list.
 *    Tail is the last bank in the list.
 *    ElemsBlock is reserved.
 *    Reserved is reserved (used for alignment).
 */
/* Alignment is 96b */
typedef struct shr_mdb_list_desc_s {
    shr_mdb_elem_index_t elems;              /* elements in this list */
    shr_mdb_elem_index_t blocks;             /* blocks in this list */
    shr_mdb_bank_index_t head;               /* first bank this list */
    shr_mdb_bank_index_t tail;               /* last bank this list */
    shr_mdb_elem_bank_index_t elemsBlock;    /* elements per block this list */
    shr_mdb_elem_bank_index_t reserved;      /* for alignment */
} shr_mdb_list_desc_t;

/*
 *  This describes the entire resource.
 *
 *  Lock is the handle of the lock for the whole thing.  If this is NULL, it
 *  means that the caller asked that it be constructed without a lock, and is
 *  therefore responsible for its protection.  Otherwise, the specified lock
 *  will be taken before any access or manipulation, and freed immediately
 *  afterward.
 *
 *  Low is the minimum element ID in this resource.  Internally, element
 *  numbers start with zero, but this allows an external bias to be applied.
 *
 *  Count is the number of elements in the resource.
 *
 *  FreeLists is the number of free lists being used for allocation.  Free list
 *  zero contains blocks of exactly one single element; other free lists
 *  contain only blocks of exactly the number of elements specified for that
 *  free list.
 *
 *  UserLists is the number of user lists in this resource.  These can be used
 *  for any purpose the user wants, and can contain blocks of any size that is
 *  supported by the resource.
 *
 *  Lists is the total number of lists -- it is merely FreeLists and UserLists
 *  added together.  This is kept for performance.
 *
 *  Banks is the number of banks in this resource.  This will be
 *  ceil(count/(2^bankShift)).
 *
 *  BankShift is the shift distance for the number of elements in a bank (which
 *  will always be an integral power of two, except the last bank).
 *
 *  BankMask is ~(1<<bankShift)-1, used as a mask for the bank number part of
 *  an element ID (after rebiasing), or inverted for use as a mask for the
 *  element offset within the bank.  This is kept for performance.
 *
 *  LastBankSize if the number of elements left over after all of the whole
 *  banks.  If zero, it means that all of the elements fit in an integral
 *  number of banks; if nonzero, the final bank has the specified number of
 *  elements (will always be < 2^bankShift).
 *
 *  AllocPref is the current alloc/free preferences.
 *
 *  List is a pointer to the information for the lists (see above).
 *
 *  ListBank is a pointer to the information for the banks within the lists
 *  (see above).
 *
 *  Elem is a pointer to the element descriptors (see above).
 *
 *  Note that the list descriptors, list-bank descriptors, and element
 *  descriptors follow the rest of the mdb_list_t, in the same alloc cell.
 *  This means it's a fairly large alloc cell.
 */
typedef struct shr_mdb_list_s {
    sal_mutex_t lock;                    /* lock for this resource map */
    shr_mdb_elem_index_t low;            /* lowest elem number as exposed */
    shr_mdb_elem_index_t count;          /* number of elements */
    shr_mdb_elem_index_t freeLists;      /* number of free lists */
    shr_mdb_elem_index_t userLists;      /* number of user lists */
    shr_mdb_elem_index_t lists;          /* total number of lists */
    shr_mdb_elem_index_t banks;          /* number of banks */
    shr_mdb_elem_index_t bankShift;      /* bank shift distance */
    shr_mdb_elem_index_t bankMask;       /* bank bits maxk */
    shr_mdb_elem_index_t lastBankSize;   /* elements in last bank */
    shr_mdb_alloc_pref_t allocPref;      /* alloc/free preferences */
    shr_mdb_list_desc_t *list;           /* pointer to list descriptors */
    shr_mdb_list_bank_desc_t *listBank;  /* pointer to list-bank descriptors */
    shr_mdb_elem_desc_t *elem;           /* pointer to element descriptors */
    /* list descriptors follow */
    /* list-bank descriptors follow */
    /* element descriptors follow */
} shr_mdb_list_t;

#define _MDB_BANK_FROM_ELEM(_res,_elem) \
    (((_elem) & (_res).bankMask) >> (_res).bankShift)

#define _MDB_OFFS_FROM_ELEM(_res,_elem) \
    ((_elem) & (~((_res).bankMask)))

#define _MDB_ELEM_FROM_BANK_OFFS(_res,_bank,_offs) \
    ((_offs) | ((_bank) << (_res).bankShift))

#define _MDB_LIST_FROM_USERLIST(_res,_userlist) \
    ((_userlist) + (_res).freeLists)

#define _MDB_LIST_BANK_INDEX(_res,_list,_bank) \
    ((_list) + ((_bank) * (_res).lists))

#define _MDB_ELEM_BANK_DESC(_res,_bank,_offs) \
    ((_res).elem[_MDB_ELEM_FROM_BANK_OFFS(_res, _bank, _offs)])

#define _MDB_LIST_BANK_DESC(_res,_list,_bank) \
    ((_res).listBank[_MDB_LIST_BANK_INDEX(_res, _list, _bank)])

/*
 *  Function
 *    _mdb_block_prep
 *  Purpose
 *    Prepare a block of elements.  This rewrites either the head or the entire
 *    block so that the block is ready for use or list insertion.  The block
 *    must not be a member of a list.
 *  Arguments
 *    (in) shr_mdb_list_handle_t list = pointer to the list info (modified)
 *    (in) shr_mdb_elem_index_t head = head element of the block
 *    (in) shr_mdb_elem_bank_index_t count = elements in this block
 *  Return
 *    void
 *  Notes
 *    Assumes all arguments are valid.
 */
static void
_mdb_block_prep(shr_mdb_list_handle_t res,
                shr_mdb_elem_index_t head,
                shr_mdb_elem_bank_index_t count)
{
    shr_mdb_elem_desc_t elem;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,%08X,%d)\n"),
               PTR_TO_INT(res),
               head,
               count));

    /* prepare head element */
    elem.list = _MDB_BLOCK_NOT_IN_LIST;
    elem.count = count;
    elem.prev = _MDB_BLOCK_END;
    elem.next = _MDB_BLOCK_END;
    res->elem[head] = elem;
    if (count > 1) {
        /* prepare tail element */
        elem.list = _MDB_BLOCK_NOT_HEAD;
        res->elem[head + count - 1] = elem;
        /* prepare any middle elements */
        for (elem.count = 0, head++;
             count > 2;
             head++, count--) {
            res->elem[head] = elem;
        }
    } /* if (count) */
}

/*
 *  Function
 *    _mdb_block_join
 *  Purpose
 *    Join two blocks into one.  The blocks must not be members of any list.
 *    The blocks must be strictly adjacent.
 *  Arguments
 *    (in) shr_mdb_list_handle_t list = pointer to the list info (modified)
 *    (in) shr_mdb_elem_index_t headLow = head element of the low block
 *    (in) shr_mdb_elem_index_t headHigh = head element of the high block
 *  Return
 *    void
 *  Notes
 *    Assumes all arguments are valid.
 */
static void
_mdb_block_join(shr_mdb_list_handle_t res,
                shr_mdb_elem_index_t headLow,
                shr_mdb_elem_index_t headHigh)
{
    shr_mdb_elem_bank_index_t count = res->elem[headLow].count;
    shr_mdb_elem_bank_index_t newCount;
    shr_mdb_elem_index_t headTemp;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,%08X,%08X)\n"),
               PTR_TO_INT(res),
               headLow,
               headHigh));

    /* ensure joined blocks are in the expected order */
    if (headLow > headHigh) {
        headTemp = headLow;
        headLow = headHigh;
        headHigh = headTemp;
    }
    /* compute new joined block length & update head element */
    newCount = res->elem[headLow].count + res->elem[headHigh].count;
    /* remove old first block tail element */
    res->elem[headLow + count - 1].count = 0;
    /* remove old second block head element */
    res->elem[headHigh].list = _MDB_BLOCK_NOT_HEAD;
    res->elem[headHigh].count = 0;
    /* update new joined block head element */
    res->elem[headLow].count = newCount;
    /* update new joined block tail element */
    res->elem[headLow + newCount - 1].count = newCount;
}

/*
 *  Function
 *    _mdb_block_split_size
 *  Purpose
 *    Split a block into two blocks, one of them a specified size.  The block
 *    must not be a member of a list.
 *  Arguments
 *    (in) shr_mdb_list_handle_t list = pointer to the list info (modified)
 *    (in) shr_mdb_elem_index_t head = head element of the original block
 *    (in) shr_mdb_elem_bank_index_t count = number of elements to split off
 *    (in) int high = TRUE to split off from high end, FALSE for low end
 *    (out) shr_mdb_elem_index_t *newBlock = where to put split block head
 *    (out) shr_mdb_elem_index_t *extra = where to put remainder block head
 *  Return
 *    void
 *  Notes
 *    Assumes all arguments are valid.
 */
static void
_mdb_block_split_size(shr_mdb_list_handle_t res,
                      shr_mdb_elem_index_t head,
                      shr_mdb_elem_bank_index_t count,
                      int high,
                      shr_mdb_elem_index_t *newBlock,
                      shr_mdb_elem_index_t *extra)
{
    shr_mdb_elem_bank_index_t oldCount = res->elem[head].count;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,%08X,%d,%s,*,*) enter\n"),
               PTR_TO_INT(res),
               head,
               count,
               high?"High":"Low"));

    /* split the block based upon high or low becoming the requested size */
    if (high) {
        /* high end of block becomes the requested size */
        *newBlock = head + oldCount - count;
        *extra = head;
    } else {
        /* low end of block becomes the requested size */
        *newBlock = head;
        *extra = head + count;
    }
    /* set new block head and tail */
    res->elem[*newBlock].list = _MDB_BLOCK_NOT_IN_LIST;
    res->elem[*newBlock].count = count;
    res->elem[*newBlock + count - 1].count = count;
    /* set leftover block head and tail */
    res->elem[*extra].list = _MDB_BLOCK_NOT_IN_LIST;
    res->elem[*extra].count = oldCount - count;
    res->elem[*extra + (oldCount - count) - 1].count = oldCount - count;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,%08X,%d,%s,&(%08X),&(%08X)) leave\n"),
               PTR_TO_INT(res),
               head,
               count,
               high?"High":"Low",
               *newBlock,
               *extra));
}

/*
 *  Function
 *    _mdb_block_split_point
 *  Purpose
 *    Split a block into two blocks, at a specific point.  The block must not
 *    be a member of a list, and must include elements below AND at the split
 *    point (so 'second' must be strictly greater than 'head' and still fall
 *    within the block at 'head').  The block will be split so the original
 *    head is the head element of the lower block and second will be the head
 *    of the upper block, after the call.
 *  Arguments
 *    (in) shr_mdb_list_handle_t list = pointer to the list info (modified)
 *    (in) shr_mdb_elem_index_t head = head element of the original block
 *    (in) shr_mdb_elem_index_t second = head element of second sub block
 *  Return
 *    void
 *  Notes
 *    Assumes all arguments are valid.
 */
static void
_mdb_block_split_point(shr_mdb_list_handle_t res,
                       shr_mdb_elem_index_t head,
                       shr_mdb_elem_index_t second)
{
    shr_mdb_elem_bank_index_t oldCount = res->elem[head].count;
    shr_mdb_elem_bank_index_t newCount = second - head;
    shr_mdb_elem_bank_index_t auxCount = oldCount - newCount;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,%08X,%08X) enter\n"),
               PTR_TO_INT(res),
               head,
               second));

    /* set lower block head and tail */
    res->elem[head].count = newCount;
    res->elem[second - 1].count = newCount;
    /* set upper block head and tail */
    res->elem[second].list = _MDB_BLOCK_NOT_IN_LIST;
    res->elem[second].count = oldCount - newCount;
    res->elem[second + auxCount - 1].count = oldCount - newCount;
}

/*
 *  Function
 *    _mdb_list_insert
 *  Purpose
 *    Insert a block of elements to a list.  This assumes the block has already
 *    been prepared, and merely updates the list membership information for the
 *    bank in which the block resides and for the block in question.  It will
 *    add the bank to those in the list if the bank is not already
 *    participating in the list.
 *  Arguments
 *    (in) shr_mdb_list_handle_t list = pointer to the list info (modified)
 *    (in) shr_mdb_elem_index_t head = head element of the block
 *    (in) shr_mdb_elem_index_t list = the list on which the block is to be put
 *  Return
 *    void
 *  Notes
 *    Assumes all arguments are valid.
 */
static void
_mdb_list_insert(shr_mdb_list_handle_t res,
                 shr_mdb_elem_index_t head,
                 shr_mdb_elem_bank_index_t list)
{
    shr_mdb_list_bank_desc_t *listBank;
    shr_mdb_elem_desc_t *thisElem;
    shr_mdb_elem_bank_index_t bank;
    shr_mdb_elem_bank_index_t offs;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,%08X,%04X)\n"),
               PTR_TO_INT(res),
               head,
               list));

    /* resolve some things about the block's head element */
    bank = _MDB_BANK_FROM_ELEM(*res, head);
    offs = _MDB_OFFS_FROM_ELEM(*res, head);
    thisElem = &(res->elem[head]);
    listBank = &_MDB_LIST_BANK_DESC(*res, list, bank);
    /* set the block as being on this list */
    thisElem->list = list;
    /* link this block into this bank of this list */
    thisElem->prev = _MDB_BLOCK_END;
    thisElem->next = listBank->head;
    listBank->head = offs;
    if (listBank->blocks) {
        /* not the first block this list this bank */
        /* set up backlink */
        _MDB_ELEM_BANK_DESC(*res, bank, thisElem->next).prev = offs;
    } else {
        /* the first block this list this bank */
        /* set up tail */
        listBank->tail = offs;
        /* add this bank to the list */
        listBank->next = res->list[list].head;
        listBank->prev = _MDB_BANK_END;
        res->list[list].head = bank;
        if (res->list[list].blocks) {
            /* not the first block this list */
            /* set up backlink */
            _MDB_LIST_BANK_DESC(*res, list, listBank->next).prev = bank;
        } else {
            /* first block this list; set up tail */
            res->list[list].tail = bank;
        }
    }
    /* account for this block on this list (bank and total) */
    listBank->elems += thisElem->count;
    res->list[list].elems += thisElem->count;
    listBank->blocks++;
    res->list[list].blocks++;
}

/*
 *  Function
 *    _mdb_block_list_remove
 *  Purpose
 *    Remove a block of elements from a list.  This will merely update the
 *    block's membership information and delink if from its list.  It will also
 *    remove the affected bank from the list if there are no more blocks in the
 *    list on that page.  The block must be a member of some list.
 *  Arguments
 *    (in) shr_mdb_list_handle_t list = pointer to the list info (modified)
 *    (in) shr_mdb_elem_index_t head = head element of the block
 *  Return
 *    void
 *  Notes
 *    Assumes all arguments are valid.
 */
static void
_mdb_list_remove(shr_mdb_list_handle_t res,
                 shr_mdb_elem_index_t head)
{
    shr_mdb_list_bank_desc_t *listBank;
    shr_mdb_elem_desc_t *thisElem;
    shr_mdb_elem_bank_index_t bank;
    shr_mdb_elem_bank_index_t list;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,%08X)\n"),
               PTR_TO_INT(res),
               head));

    /* resolve some things about the block's head element */
    bank = _MDB_BANK_FROM_ELEM(*res, head);
    thisElem = &(res->elem[head]);
    list = thisElem->list;
    listBank = &_MDB_LIST_BANK_DESC(*res, list, bank);
    /* set the block as not being on a list */
    thisElem->list = _MDB_BLOCK_NOT_IN_LIST;
    /* account for this block on this list (bank and total) */
    listBank->elems -= thisElem->count;
    res->list[list].elems -= thisElem->count;
    listBank->blocks--;
    res->list[list].blocks--;
    /* unlink this block from this bank of this list */
    if (_MDB_BLOCK_INVALID > thisElem->next) {
        /* not the last block in this bank of the list */
        _MDB_ELEM_BANK_DESC(*res, bank, thisElem->next).prev = thisElem->prev;
    } else {
        /* the last block in this bank of the list */
        listBank->tail = thisElem->prev;
    }
    if (_MDB_BLOCK_INVALID > thisElem->prev) {
        /* not the first block in this bank of the list */
        _MDB_ELEM_BANK_DESC(*res, bank, thisElem->prev).next = thisElem->next;
    } else {
        /* the first block in this bank of the list */
        listBank->head = thisElem->next;
    }
    thisElem->prev = _MDB_BLOCK_END;
    thisElem->next = _MDB_BLOCK_END;
    /* unlink this bank from the list if appropriate */
    if (!listBank->blocks) {
        /* this bank contains no more blocks; unlink it */
        if (_MDB_BLOCK_INVALID > listBank->next) {
            /* not the last bank in this list */
            _MDB_LIST_BANK_DESC(*res, list, listBank->next).prev = listBank->prev;
        } else {
            /* the last bank in this list */
            res->list[list].tail = listBank->prev;
        }
        if (_MDB_BLOCK_INVALID > listBank->prev) {
            /* not the first bank in this list */
            _MDB_LIST_BANK_DESC(*res, list, listBank->prev).next = listBank->next;
        } else {
            /* the first bank in this list */
            res->list[list].head = listBank->next;
        }
        listBank->prev = _MDB_BANK_END;
        listBank->next = _MDB_BANK_END;
    }
}

/*
 *  Function
 *    _mdb_block_frag_and_free
 *  Purpose
 *    Fragment a block into the largest possible fragments (up to the size of
 *    the block as provided) and place those subblocks on their respective free
 *    lists.  The block must not be a member of any list (but the subblocks
 *    will all be members of the proper free lists afterward).
 *  Arguments
 *    (in) shr_mdb_list_handle_t list = pointer to the list info (modified)
 *    (in) shr_mdb_elem_index_t head = head element of block to free
 *  Return
 *    void
 *  Notes
 *    Assumes all arguments are valid.
 */
static void
_mdb_block_frag_and_free(shr_mdb_list_handle_t res,
                         shr_mdb_elem_index_t head)
{
    shr_mdb_elem_bank_index_t freeList = res->freeLists - 1;
    shr_mdb_elem_bank_index_t count = res->elem[head].count;
    shr_mdb_elem_bank_index_t elemsBlock = res->list[freeList].elemsBlock;
    shr_mdb_elem_index_t newFree;
    shr_mdb_elem_index_t oldHead = head;
    shr_mdb_elem_index_t newHead;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,%08X)\n"),
               PTR_TO_INT(res),
               head));

    /* split the block into largest possible free subblocks */
    while (count) {
        /* while we have elements to free */
        LOG_DEBUG(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: block at %08X has %d elements;"
                            " freelist = %d\n"),
                   PTR_TO_INT(res),
                   oldHead,
                   count,
                   freeList));
        if (count > elemsBlock) {
            /* block size > list block size; fragment it */
            _mdb_block_split_size(res,
                                  oldHead,
                                  elemsBlock,
                                  res->allocPref & shr_mdb_free_block_high,
                                  &newFree,
                                  &newHead);
            /* insert the proper fragment to this free list */
            _mdb_list_insert(res, newFree, freeList);
            /* compute size of remaining fragment */
            count -= elemsBlock;
            /* adjust to use remaining fragment next iteration */
            oldHead = newHead;
        } else if (count == elemsBlock) {
            /* block size == list block size; finish up here */
            _mdb_list_insert(res, oldHead, freeList);
            count = 0;
        } else {
            /* block size < list block size; move to next list down */
            freeList--;
            elemsBlock = res->list[freeList].elemsBlock;
        }
    } /* while ((count) && (_SHR_E_NONE == result)) */
}

/*
 *  Function
 *    _mdb_block_defrag
 *  Purpose
 *    Join a block with adjacent blocks that are on free lists to form the
 *    largest possible contiguous free block within the bank.
 *  Arguments
 *    (in) shr_mdb_list_handle_t list = pointer to the list info (modified)
 *    (in/out) shr_mdb_elem_index_t *head = (in) ptr to head elem of orig blk
 *                                          (out) updated to block's head elem
 *    (in) shr_mdb_alloc_pref_t override = override join preferences
 *  Return
 *    void
 *  Notes
 *    Assumes all arguments are valid.
 *    The value at head is updated to reflect the actual head element of the
 *    final coalesced block.
 *    Will not expand the block in such a way as to cause it to cross banks.
 *    There is an override to force defrag to apply; this is because there are
 *    some places where it is assumed that defrag works in a certain way.
 */
static void
_mdb_block_defrag(shr_mdb_list_handle_t res,
                  shr_mdb_elem_index_t *head,
                  shr_mdb_alloc_pref_t override)
{
    shr_mdb_elem_index_t nextBlock;
    shr_mdb_elem_bank_index_t count;
    shr_mdb_elem_bank_index_t offs;
    shr_mdb_alloc_pref_t pref = res->allocPref | override;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,&(%08X)) enter\n"),
               PTR_TO_INT(res),
               *head));

    offs = _MDB_OFFS_FROM_ELEM(*res, *head);
    if (pref & shr_mdb_join_low) {
        /* try to collect free blocks into one by going downward in bank */
        while (offs) {
            count = res->elem[(*head) - 1].count;
            nextBlock = (*head) - count;
            if (res->elem[nextBlock].list >= res->freeLists) {
                /* the next block isn't on a free list; finsihed going down */
                break;
            }
            LOG_DEBUG(BSL_LS_SOC_COMMON,
                      (BSL_META("join downward to %08X (%d elements)\n"),
                       nextBlock,
                       count));
            offs -= count;
            /* get the block from its free list */
            _mdb_list_remove(res, nextBlock);
            /* join the block to the existing block */
            _mdb_block_join(res, nextBlock, *head);
            /* update start of new bigger block */
            *head = nextBlock;
        } /* while (offs) */
    } /* if (res->allocPref & _mdb_join_low) */
    if ((pref & shr_mdb_join_high) ||
        (!(pref & (shr_mdb_join_low | shr_mdb_join_high)))) {
        /* try to collect free blocks into one by going upward in bank */
        count = res->elem[*head].count;
        LOG_DEBUG(BSL_LS_SOC_COMMON,
                  (BSL_META("current %08X count %d next %08X mask %08X\n"),
                   *head,
                   count,
                   *head + count,
                   res->bankMask));
        while (((offs + count) <= (~(res->bankMask))) &&
               ((*head + count) < res->count)) {
            nextBlock = *head + count;
            if (res->elem[nextBlock].list >= res->freeLists) {
                /* the next block isn't on a free list; finished going up */
                break;
            }
            LOG_DEBUG(BSL_LS_SOC_COMMON,
                      (BSL_META("join upward to %08X (%d elements)\n"),
                       nextBlock,
                       res->elem[nextBlock].count));
            /* get the block from its free list */
            _mdb_list_remove(res, nextBlock);
            /* join the block to the existing block */
            _mdb_block_join(res, *head, nextBlock);
            /* update number of elements in the block */
            count = res->elem[*head].count;
        } /* while ((offs + count) <= res->bankMask) */
    } /* if (alloc_join_high mode or neither that nor alloc_join_low mode) */

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,&(%08X)) leave\n"),
               PTR_TO_INT(res),
               *head));
}

/*
 *  Function
 *    _mdb_block_head_get
 *  Purpose
 *    Given an arbitrary element in a block, find the element that is the head
 *    of that block.
 *  Arguments
 *    (in) shr_mdb_list_handle_t list = pointer to the list info (modified)
 *    (in/out) shr_mdb_elem_index_t *head = (in) ptr to some elem of a block
 *                                          (out) updated to block's head elem
 *  Return
 *    void
 *  Notes
 *    Assumes all arguments are valid.
 *    The value at head is updated to reflect the actual head element.
 */
static void
_mdb_block_head_get(shr_mdb_list_handle_t res,
                    shr_mdb_elem_index_t *head)
{
    shr_mdb_elem_index_t currBlock;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,&(%08X)) enter\n"),
               PTR_TO_INT(res),
               *head));

    if (_MDB_BLOCK_NOT_HEAD != res->elem[*head].list) {
        /* element is the head, so just return now */
        LOG_DEBUG(BSL_LS_SOC_COMMON,
                  (BSL_META("(%08X,&(%08X)) early leave (head, %d)\n"),
                   PTR_TO_INT(res),
                   *head,
                   res->elem[*head].count));
        return;
    }
    if (0 != res->elem[*head].count) {
        /* element isn't head, but count is nonzero, so must be tail */
        *head -= (res->elem[*head].count - 1);
        LOG_DEBUG(BSL_LS_SOC_COMMON,
                  (BSL_META("(%08X,&(%08X)) early leave (tail, %d)\n"),
                   PTR_TO_INT(res),
                   *head,
                   res->elem[*head].count));
        return;
    }
    /*
     *  Instead of starting with the current element and searching backwards
     *  until we reach the block's head, we'll just look for a block that
     *  contains the specified element, and return its head, starting at the
     *  beginning of the bank containing the element.
     *
     *  The hope here is that the typical block is many elements.
     */
    for (currBlock = ((*head) & (res->bankMask));
         ((currBlock < res->count) &&
          ((currBlock + res->elem[currBlock].count) < res->count) &&
          (currBlock < (*head)) &&
          ((currBlock + res->elem[currBlock].count) < (*head)));
         currBlock += res->elem[currBlock].count) {
        /* just search; don't do anything */
        LOG_DEBUG(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: block at %08X length %d\n"),
                   PTR_TO_INT(res),
                   currBlock,
                   res->elem[currBlock].count));
    }
    *head = currBlock;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,&(%08X)) leave\n"),
               PTR_TO_INT(res),
               *head));
}


/*****************************************************************************
 *
 *  Support
 *
 *  A lot of functions that are used when implementing the API calls.  These
 *  functions verify input arguments, and use the implementation calls to
 *  actually do most of the work.
 *
 *  These calls may do early returns, but will avoid leaks and partial update
 *  conditions that could cause problems.
 *
 *  Block base is as exposed to the caller for these functions, but may be used
 *  as internally.
 */

/*
 *  Function
 *    _shr_mdb_block_alloc
 *  Purpose
 *    Allocate a contiguous block of the specified number of elements.
 *  Arguments
 *    (in) shr_mdb_list_handle_t res = pointer to the list info (modified)
 *    (out) shr_mdb_elem_index_t *head = where to put the head of the block
 *    (in) shr_mdb_elem_bank_index_t count = number of elements in block
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *  Notes
 */
static int
_shr_mdb_block_alloc(shr_mdb_list_handle_t res,
                     shr_mdb_elem_index_t *head,
                     shr_mdb_elem_bank_index_t count)
{
    shr_mdb_list_bank_desc_t *listBank = NULL;
    shr_mdb_elem_bank_index_t baseList;
    shr_mdb_elem_bank_index_t list;
    shr_mdb_elem_bank_index_t offs;
    shr_mdb_elem_index_t bank;
    shr_mdb_elem_index_t block = ~0;
    shr_mdb_elem_index_t extra = ~0;
    int result = _SHR_E_RESOURCE;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,*,%d) enter\n"),
               PTR_TO_INT(res),
               count));

    /* make sure we can actually alloc the requested block */
    if (count > res->list[res->freeLists - 1].elemsBlock) {
        /* the request is larger than the largest supported block */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: count %d exceeds largest block %d\n"),
                   PTR_TO_INT(res),
                   count,
                   res->list[res->freeLists - 1].elemsBlock));
        return _SHR_E_PARAM;
    }
    /* find the free list with the smallest acceptable blocks */
    for (baseList = 0;
         (baseList < res->freeLists) &&
         (res->list[baseList].elemsBlock < count);
         baseList++) {
        /* don't do anything but iterate for the desired list */
    }
    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("mdb %08X: looking for free block >= %d elements\n"),
               PTR_TO_INT(res),
               res->list[baseList].elemsBlock));
    /* allocate an initial block */
    switch (res->allocPref & shr_mdb_alloc_bank_mask) {
    case shr_mdb_alloc_bank_first:
        /* using first method; get the first block that's big enough */
        LOG_DEBUG(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: using '%s' alloc mode\n"),
                   PTR_TO_INT(res),
                   "first"));
        /* this method effectively searches all banks at the same time */
        /* find a list with some available blocks */
        for (list = baseList;
             list < res->freeLists;
             list++) {
            if (res->list[list].blocks) {
                /* this list has some blocks */
                result = _SHR_E_NONE;
                break;
            }
        }
        if (_SHR_E_NONE == result) {
            /* found a list with some large enough blocks; get first block */
            bank = res->list[list].head;
            offs = _MDB_LIST_BANK_DESC(*res, list, bank).head;
            block = (bank << res->bankShift) | offs;
            /* take the block out of its list */
            _mdb_list_remove(res, block);
        }
        break;
    case shr_mdb_alloc_bank_low:
        /* using low method; search banks low to high */
        LOG_DEBUG(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: using '%s' alloc mode\n"),
                   PTR_TO_INT(res),
                   "low"));
        for (bank = 0;
             bank < res->banks;
             bank++) {
            /* check for any list with acceptable blocks in this bank */
            for (list = baseList;
                 list < res->freeLists;
                 list++) {
                listBank = &_MDB_LIST_BANK_DESC(*res, list, bank);
                if (listBank->blocks) {
                    /* this list has some blocks */
                    result = _SHR_E_NONE;
                    break;
                }
            }
            if (_SHR_E_NONE == result) {
                /* we found a bank and list with an acceptable block */
                offs = listBank->head;
                block = (bank << res->bankShift) | offs;
                /* take the block out of its list */
                _mdb_list_remove(res, block);
                break;
            }
        }
        break;
    case shr_mdb_alloc_bank_high:
        /* using high method; search banks high to low */
        LOG_DEBUG(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: using '%s' alloc mode\n"),
                   PTR_TO_INT(res),
                   "high"));
        bank = res->banks;
        do {
            bank--;
            /* check for any list with acceptable blocks in this bank */
            for (list = baseList;
                 list < res->freeLists;
                 list++) {
                listBank = &_MDB_LIST_BANK_DESC(*res, list, bank);
                if (listBank->blocks) {
                    /* this list has some blocks */
                    result = _SHR_E_NONE;
                    break;
                }
            }
            if (_SHR_E_NONE == result) {
                /* we found a bank and list with an acceptable block */
                offs = listBank->head;
                block = (bank << res->bankShift) | offs;
                /* take the block out of its list */
                _mdb_list_remove(res, block);
                break;
            }
        } while (bank > 0);
        break;
    default:
        /* should never see this; it's not valid */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: unknown alloc mode: %d\n"),
                   PTR_TO_INT(res),
                   res->allocPref & shr_mdb_alloc_bank_mask));
        result = _SHR_E_INTERNAL;
    }
    if (_SHR_E_NONE == result) {
        /* was able to find an acceptable block somewhere */
        LOG_DEBUG(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: found suitable block at %08X (%d elements)\n"),
                   PTR_TO_INT(res),
                   block + res->low,
                   res->elem[block].count));
        if (res->allocPref & shr_mdb_join_alloc) {
            /* need to join adjacent free blocks on alloc */
            _mdb_block_defrag(res, &block, 0);
        }
        if (res->elem[block].count > count) {
            /* need to break the user request off this block & free the rest */
            _mdb_block_split_size(res,
                             block,
                             count,
                             res->allocPref & shr_mdb_alloc_block_high,
                             &block,
                             &extra);
            /* dispose of the 'extra' part */
            _mdb_block_frag_and_free(res, extra);
        }
        *head = block + res->low;
    }

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,%d,&(%08X)) return %d (%s)\n"),
               PTR_TO_INT(res),
               count,
               *head,
               result,
               _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Function
 *    _shr_mdb_block_free
 *  Purpose
 *    Free a block of elements that is not in a list.
 *  Arguments
 *    (in) shr_mdb_list_handle_t res = pointer to the list info (modified)
 *    (in) shr_mdb_elem_index_t head = the head element of the block to free
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *  Notes
 */
static int
_shr_mdb_block_free(shr_mdb_list_handle_t res,
                    shr_mdb_elem_index_t head)
{
    shr_mdb_elem_index_t block;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,%08X)\n"),
               PTR_TO_INT(res),
               head));

    /* make sure the arguments are acceptable */
    block = head - res->low;
    if ((head < res->low) || (block >= res->count)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: invalid block at %08X can not be freed\n"),
                   PTR_TO_INT(res),
                   head));
        /* don't ask me why, but BCM likes NOT_FOUND for INVALID_ID */
        return _SHR_E_NOT_FOUND;
    }
    if (res->elem[block].list < res->freeLists) {
        /* this block is already free */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: block at %08X is already free\n"),
                   PTR_TO_INT(res),
                   head));
        return _SHR_E_NOT_FOUND;
    }
    if (_MDB_BLOCK_NOT_IN_LIST != res->elem[block].list) {
        /* this block is in a user list */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: block at %08X is in a list\n"),
                   PTR_TO_INT(res),
                   head));
        return _SHR_E_BUSY;
    }
    if ((res->allocPref & shr_mdb_join_free) ||
        (!(res->allocPref & (shr_mdb_join_alloc | shr_mdb_join_free)))) {
        /* need to join adjacent free blocks on free */
        _mdb_block_defrag(res, &block, 0);
    }
    /* free the block */
    _mdb_block_frag_and_free(res, block);

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,%08X) return %d (%s)\n"),
               PTR_TO_INT(res),
               head,
               _SHR_E_NONE,
               _SHR_ERRMSG(_SHR_E_NONE)));
    return _SHR_E_NONE;
}

/*
 *  Function
 *    _mdb_block_size_get
 *  Purpose
 *    Get number of elements in a block
 *  Arguments
 *    (in) shr_mdb_list_handle_t res = pointer to the list information
 *    (in) shr_mdb_elem_index_t block = head element of block
 *    (out) shr_mdb_elem_bank_index_t *count = where to put the count
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *  Notes
 */
static int
_mdb_block_size_get(shr_mdb_list_handle_t res,
                    shr_mdb_elem_index_t head,
                    shr_mdb_elem_bank_index_t *count)
{
    shr_mdb_elem_index_t block;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,%08X,*) enter\n"),
               PTR_TO_INT(res),
               head));

    /* bias the block to internal value */
    block = head - res->low;
    /* make sure the block is valid */
    if ((head < res->low) || (block >= res->count)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: element %08X is not valid\n"),
                   PTR_TO_INT(res),
                   head));
        /* don't ask me why, but BCM likes NOT_FOUND for INVALID_ID */
        return _SHR_E_NOT_FOUND;
    }
    /* make sure we have the block's head */
    _mdb_block_head_get(res, &block);
    /* make sure the block is not free */
    if (res->freeLists > res->elem[block].list) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: block at %08X..%08X is free\n"),
                   PTR_TO_INT(res),
                   block + res->low,
                   block + res->elem[block].count + res->low - 1));
        return _SHR_E_NOT_FOUND;
    }
    /* okay, return the count on which the block belongs, biased */
    *count = res->elem[block].count;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,%08X,&(%d)) return %d (%s)\n"),
               PTR_TO_INT(res),
               head,
               *count,
               _SHR_E_NONE,
               _SHR_ERRMSG(_SHR_E_NONE)));
    return _SHR_E_NONE;
}

/*
 *  Function
 *    _shr_mdb_elems_reserve
 *  Purpose
 *    Reserve specific elements.  These elements will be allocated as single
 *    element, and can be freed one at a time by using the free call above.
 *    The specified elements can span banks.  Will ensure all of the requested
 *    elements are free before reserving any of them.
 *  Arguments
 *    (in) shr_mdb_list_handle_t res = pointer to the list info (modified)
 *    (in) shr_mdb_elem_index_t head = the lowest element to reserve
 *    (in) shr_mdb_elem_index_t count = the number of elements to reserve
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *  Notes
 */
static int
_shr_mdb_elems_reserve(shr_mdb_list_handle_t res,
                       shr_mdb_elem_index_t head,
                       shr_mdb_elem_index_t count)
{
    shr_mdb_elem_index_t currElem;
    shr_mdb_elem_index_t currCount = count;
    shr_mdb_elem_index_t baseElem;
    shr_mdb_elem_index_t tempElem;
    shr_mdb_elem_index_t block;
    shr_mdb_elem_desc_t elem;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,%08X,%d) enter\n"),
               PTR_TO_INT(res),
               head,
               count));

    /* make sure the arguments are acceptable */
    block = head - res->low;
    if ((head < res->low) ||
        ((head + count - res->low) > res->count)) {
        /* head or count places part or all of the block outside range */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: can not reserve %d elements at %08X since"
                   " the range contains invalid elements\n"),
                   PTR_TO_INT(res),
                   count,
                   head));
        /* don't ask me why, but BCM likes NOT_FOUND for INVALID_ID */
        return _SHR_E_NOT_FOUND;
    }
    if (!count) {
        /* do nothing? */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: can not reserve zero element range\n"),
                   PTR_TO_INT(res)));
        return _SHR_E_PARAM;
    }
    /* find the head of the first block in the range */
    currElem = block;
    _mdb_block_head_get(res, &currElem);
    baseElem = currElem;
    /* make sure each block touched by the range is free */
    while (currElem < block + count) {
        if (res->elem[currElem].list >= res->freeLists) {
            /* this block is not free */
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("mdb %08X: can't reserve in-use block"
                       " at %08X\n"),
                       PTR_TO_INT(res),
                       currElem));
            return _SHR_E_RESOURCE;
        }
        currElem += res->elem[currElem].count;
    } /* while (currElem < head + count) */
    /* set up a singly-allocated not-in-list element description */
    elem.list = _MDB_BLOCK_NOT_IN_LIST;
    elem.count = 1;
    elem.next = _MDB_BLOCK_END;
    elem.prev = _MDB_BLOCK_END;
    /* start with the first block touched */
    currElem = baseElem;
    /* singly-allocate all participating elements */
    while (currElem < (block + count)) {
        /* keep this block's size */
        currCount = res->elem[currElem].count;
        /* remove this block from its free list */
        _mdb_list_remove(res, currElem);
        if (block > currElem) {
            LOG_DEBUG(BSL_LS_SOC_COMMON,
                      (BSL_META("mdb %08X: break off low elements"
                                " %08X..%08X\n"),
                       PTR_TO_INT(res),
                       currElem + res->low,
                       head - 1));
            /* break off unwanted low elements */
            _mdb_block_split_point(res, currElem, block);
            /* adjust the current block size accordingly */
            currCount = res->elem[block].count;
            /* join unwanted low elements with neighbours */
            _mdb_block_defrag(res, &currElem, 0);
            /* return unwanted low elements to the free pool */
            _mdb_block_frag_and_free(res, currElem);
            /* now point to the elements we kept */
            currElem = block;
        }
        if ((currElem + currCount) > (block + count)) {
            LOG_DEBUG(BSL_LS_SOC_COMMON,
                      (BSL_META("mdb %08X: break off high elements"
                                " %08X..%08X\n"),
                       PTR_TO_INT(res),
                       head + count,
                       currElem + currCount - 1 + res->low));
            /* break off unwanted high elements */
            tempElem = block + count;
            _mdb_block_split_point(res, currElem, tempElem);
            /* adjust the current block size accordingly */
            currCount = res->elem[currElem].count;
            /* join unwanted high elements with neighbours */
            _mdb_block_defrag(res, &tempElem, 0);
            /* return unwanted high elements to the free pool */
            _mdb_block_frag_and_free(res, tempElem);
        }
        while (currCount > 0) {
            /* set this element as being allocated singly */
            res->elem[currElem] = elem;
            currCount--;
            currElem++;
        }
    } /* while (currElem < head + count) */

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,%08X,%d) return %d (%s)\n"),
               PTR_TO_INT(res),
               head,
               count,
               _SHR_E_NONE,
               _SHR_ERRMSG(_SHR_E_NONE)));
    return _SHR_E_NONE;
}

/*
 *  Function
 *    _shr_mdb_elems_collect
 *  Purpose
 *    Collect a set of single-element blocks into a large single block.
 *  Arguments
 *    (in) shr_mdb_list_handle_t res = pointer to the list info (modified)
 *    (in) shr_mdb_elem_index_t head = the lowest element to reserve
 *    (in) shr_mdb_elem_bank_index_t count = the number of elements to reserve
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *  Notes
 */
static int
_shr_mdb_elems_collect(shr_mdb_list_handle_t res,
                       shr_mdb_elem_index_t head,
                       shr_mdb_elem_bank_index_t count)
{
    shr_mdb_elem_index_t currElem;
    shr_mdb_elem_index_t block;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,%08X,%d) enter\n"),
               PTR_TO_INT(res),
               head,
               count));

    /* make sure the arguments are acceptable */
    block = head - res->low;
    if ((head < res->low) || ((head + count - res->low) > res->count)) {
        /* head or count places part or all of the block outside range */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: can not collect %d elements at %08X since"
                   " the range contains invalid elements\n"),
                   PTR_TO_INT(res),
                   count,
                   head));
        /* don't ask me why, but BCM likes NOT_FOUND for INVALID_ID */
        return _SHR_E_NOT_FOUND;
    }
    if (!count) {
        /* do nothing? */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: can not reserve zero element range\n"),
                   PTR_TO_INT(res)));
        return _SHR_E_PARAM;
    }
    /* make sure all elements are in the same bank */
    if ((_MDB_OFFS_FROM_ELEM(*res, block) + count) > ((~(res->bankMask)) + 1)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: %d element block at %08X"
                   " would span banks\n"),
                   PTR_TO_INT(res),
                   count,
                   head));
        /* don't ask me why, but BCM likes NOT_FOUND for INVALID_ID */
        return _SHR_E_PARAM;
    }
    /* make sure each element is free and is a single-element block */
    for (currElem = block;
         currElem < block + count;
         currElem++) {
        if (res->elem[currElem].list == _MDB_BLOCK_NOT_HEAD) {
            /* non-head elements do not exist in one-element blocks */
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("mdb %08X: can't collect non-single-element"
                       " block at %08X\n"),
                       PTR_TO_INT(res),
                       currElem + res->low));
            return _SHR_E_PARAM;
        }
        if (res->elem[currElem].list < res->freeLists) {
            /* this block is already free */
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("mdb %08X: can't collect free"
                       " block at %08X\n"),
                       PTR_TO_INT(res),
                       currElem + res->low));
            return _SHR_E_NOT_FOUND;
        }
        if (res->elem[currElem].list != _MDB_BLOCK_NOT_IN_LIST) {
            /* the block is in a list */
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("mdb %08X: can't collect block at %08X since"
                       " it is in a list\n"),
                       PTR_TO_INT(res),
                       currElem + res->low));
            return _SHR_E_BUSY;
        }
        if (res->elem[currElem].count != 1) {
            /* the block size is not one */
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("mdb %08X: can't collect %d-element"
                       " block at %08X\n"),
                       PTR_TO_INT(res),
                       res->elem[currElem].count,
                       currElem + res->low));
            return _SHR_E_PARAM;
        }
    }
    /* prepare the newly gathered block */
    _mdb_block_prep(res, block, count);

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,%08X,%d) return %d (%s)\n"),
               PTR_TO_INT(res),
               head,
               count,
               _SHR_E_NONE,
               _SHR_ERRMSG(_SHR_E_NONE)));
    return _SHR_E_NONE;
}

/*
 *  Function
 *    _shr_mdb_elems_unreserve
 *  Purpose
 *    Unreserve specific elements.  These elements will be gathered into blocks
 *    and then freed.  Can be used on any group of contiguous single-element
 *    blocks, but will not work against larger blocks (use free instead).  This
 *    was meant specifically as a faster way to undo a reserve call, though
 *    free could be called instead against every element in the reserved set.
 *    Makes sure the element set looks as expected before freeing any elements.
 *  Arguments
 *    (in) shr_mdb_list_handle_t res = pointer to the list info (modified)
 *    (in) shr_mdb_elem_index_t head = the lowest element to reserve
 *    (in) shr_mdb_elem_index_t count = the number of elements to reserve
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *  Notes
 */
static int
_shr_mdb_elems_unreserve(shr_mdb_list_handle_t res,
                         shr_mdb_elem_index_t head,
                         shr_mdb_elem_index_t count)
{
    shr_mdb_elem_index_t currElem;
    shr_mdb_elem_index_t currCount = count;
    shr_mdb_elem_index_t block;
    shr_mdb_elem_bank_index_t offs;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,%08X,%d) enter\n"),
               PTR_TO_INT(res),
               head,
               count));

    /* make sure the arguments are acceptable */
    block = head - res->low;
    if ((head < res->low) || ((head + count - res->low) > res->count)) {
        /* head or count places part or all of the block outside range */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: can not unreserve %d elements at"
                   " %08X since the range contains invalid elements\n"),
                   PTR_TO_INT(res),
                   count,
                   head));
        /* don't ask me why, but BCM likes NOT_FOUND for INVALID_ID */
        return _SHR_E_NOT_FOUND;
    }
    if (!count) {
        /* do nothing? */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: can not reserve zero element range\n"),
                   PTR_TO_INT(res)));
        return _SHR_E_PARAM;
    }
    /* make sure each element is free and is a single-element block */
    for (currElem = block;
         currElem < block + count;
         currElem++) {
        if (res->elem[currElem].list == _MDB_BLOCK_NOT_HEAD) {
            /* non-head elements do not exist in one-element blocks */
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("mdb %08X: can't unreserve non-single-element"
                       " block at %08X\n"),
                       PTR_TO_INT(res),
                       currElem + res->low));
            return _SHR_E_PARAM;
        }
        if (res->elem[currElem].list < res->freeLists) {
            /* this block is already free */
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("mdb %08X: can't unreserve free"
                       " block at %08X\n"),
                       PTR_TO_INT(res),
                       currElem + res->low));
            return _SHR_E_NOT_FOUND;
        }
        if (res->elem[currElem].list != _MDB_BLOCK_NOT_IN_LIST) {
            /* the block is in a list */
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("mdb %08X: can't unreserve block at %08X since"
                       " it is in a list\n"),
                       PTR_TO_INT(res),
                       currElem + res->low));
            return _SHR_E_BUSY;
        }
        if (res->elem[currElem].count != 1) {
            /* the block size is not one */
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("mdb %08X: can't unreserve %d-element"
                       " block at %08X\n"),
                       PTR_TO_INT(res),
                       res->elem[currElem].count,
                       currElem + res->low));
            return _SHR_E_PARAM;
        }
    }
    /* gather the elements into largest blocks possible and free them */
    while (count > 0) {
        currElem = block;
        offs = _MDB_OFFS_FROM_ELEM(*res, currElem);
        currCount = (res->bankMask + 1) - offs;
        if (currCount > count) {
            currCount = count;
        }
        /* prepare the newly gathered block */
        _mdb_block_prep(res, currElem, currCount);
        /* defragment the block into neighbours */
        if ((res->allocPref & shr_mdb_join_free) ||
            (!(res->allocPref & (shr_mdb_join_alloc | shr_mdb_join_free)))) {
            /* need to join adjacent free blocks on free */
            _mdb_block_defrag(res, &currElem, 0);
        }
        /* free the block */
        _mdb_block_frag_and_free(res, currElem);
        /* go to the next bank */
        block += currCount;
        count -= currCount;
    }

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,%08X,%d) return %d (%s)\n"),
               PTR_TO_INT(res),
               head,
               count,
               _SHR_E_NONE,
               _SHR_ERRMSG(_SHR_E_NONE)));
    return _SHR_E_NONE;
}

/*
 *  Function
 *    _shr_mdb_block_alloc_id
 *  Purpose
 *    Allocate a block at a specific starting point.
 *  Arguments
 *    (in) shr_mdb_list_handle_t res = pointer to the list info (modified)
 *    (in) shr_mdb_elem_index_t head = the lowest element to allocate
 *    (in) shr_mdb_elem_bank_index_t count = the number of elements to allocate
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *  Notes
 */
static int
_shr_mdb_block_alloc_id(shr_mdb_list_handle_t res,
                        shr_mdb_elem_index_t head,
                        shr_mdb_elem_bank_index_t count)
{
    shr_mdb_elem_index_t currElem;
    shr_mdb_elem_index_t block;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,%08X,%d) enter\n"),
               PTR_TO_INT(res),
               head,
               count));

    /* make sure the arguments are acceptable */
    block = head - res->low;
    if ((head < res->low) ||
        ((head + count - res->low) > res->count)) {
        /* head or count places part or all of the block outside range */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: can not alloc %d elements at %08X since"
                   " the range contains invalid elements\n"),
                   PTR_TO_INT(res),
                   count,
                   head));
        /* don't ask me why, but BCM likes NOT_FOUND for INVALID_ID */
        return _SHR_E_NOT_FOUND;
    }
    if (!count) {
        /* do nothing? */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: can not reserve zero element range\n"),
                   PTR_TO_INT(res)));
        return _SHR_E_PARAM;
    }
    /* find the head of the first block in the range */
    currElem = block;
    _mdb_block_head_get(res, &currElem);
    /* make sure it's free */
    if (res->freeLists <= res->elem[currElem].list) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: can not allocate %d elements at %08X since"
                   " the range does not begin within a free block\n"),
                   PTR_TO_INT(res),
                   count,
                   head));
        return _SHR_E_RESOURCE;
    }
    /* take it off its free list */
    _mdb_list_remove(res, currElem);
    /* defragment around this block to get it as large as possible */
    /* override to force defrag upward because that is assumed here */
    _mdb_block_defrag(res, &currElem, shr_mdb_join_high);
    /* make sure the free block is big enough */
    if ((currElem > block) ||
        ((currElem + res->elem[currElem].count) < (block + count))) {
        /* the block starts too high or ends too low; put it back & give up */
        _mdb_block_frag_and_free(res, currElem);
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: unable to alloc %d elements at %08X"
                   " because at least some part of it was not free\n"),
                   PTR_TO_INT(res),
                   count,
                   head));
        return _SHR_E_RESOURCE;
    }
    /* trim off any unwanted space below the desired block */
    if (block > currElem) {
        /* there are elements before the desired start; get rid of them */
        _mdb_block_split_point(res, currElem, block);
        _mdb_block_frag_and_free(res, currElem);
    }
    /* trim off any unwanted space above the desired block */
    if (res->elem[block].count > count) {
        /* there are elements after the desired end; get rid of them */
        _mdb_block_split_point(res, block, block + count);
        _mdb_block_frag_and_free(res, block + count);
    }

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,%08X,%d) return %d (%s)\n"),
               PTR_TO_INT(res),
               head,
               count,
               _SHR_E_NONE,
               _SHR_ERRMSG(_SHR_E_NONE)));
    return _SHR_E_NONE;
}

/*
 *  Function
 *    _mdb_user_list_insert
 *  Purpose
 *    Insert a block to a user list.
 *  Arguments
 *    (in) shr_mdb_list_handle_t res = pointer to the list information
 *    (in) shr_mdb_elem_bank_index_t list = the list on which to insert
 *    (in) shr_mdb_elem_index_t head = head element of block
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *  Notes
 */
static int
_mdb_user_list_insert(shr_mdb_list_handle_t res,
                      shr_mdb_elem_bank_index_t list,
                      shr_mdb_elem_index_t head)
{
    shr_mdb_elem_index_t block;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,%d,%08X) enter\n"),
               PTR_TO_INT(res),
               list,
               head));

    /* bias the block to internal value */
    block = head - res->low;
    /* make sure the block and list are valid */
    if (list >= res->userLists) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: list %d is not valid\n"),
                   PTR_TO_INT(res),
                   list));
        /* don't ask me why, but BCM likes NOT_FOUND for INVALID_ID */
        return _SHR_E_NOT_FOUND;
    }
    if ((head < res->low) || (block >= res->count)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: element %08X is not valid\n"),
                   PTR_TO_INT(res),
                   head));
        /* don't ask me why, but BCM likes NOT_FOUND for INVALID_ID */
        return _SHR_E_NOT_FOUND;
    }
    /* make sure we have the block's head */
    _mdb_block_head_get(res, &block);
    /* make sure the block is not free */
    if (res->freeLists > res->elem[block].list) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: block at %08X..%08X is free\n"),
                   PTR_TO_INT(res),
                   block + res->low,
                   block + res->elem[block].count + res->low - 1));
        return _SHR_E_NOT_FOUND;
    }
    /* make sure the block isn't already in a list */
    if (_MDB_BLOCK_NOT_IN_LIST != res->elem[block].list) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: block at %08X..%08X in list %d\n"),
                   PTR_TO_INT(res),
                   block + res->low,
                   block + res->elem[block].count + res->low - 1,
                   res->elem[block].list - res->freeLists));
        return _SHR_E_BUSY;
    }
    /* looks okay; insert the block to the list */
    _mdb_list_insert(res, block, list + res->freeLists);

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,%d,%08X) return %d (%s)\n"),
               PTR_TO_INT(res),
               list,
               head,
               _SHR_E_NONE,
               _SHR_ERRMSG(_SHR_E_NONE)));
    return _SHR_E_NONE;
}

/*
 *  Function
 *    _mdb_user_list_remove
 *  Purpose
 *    Remove a block from its list
 *  Arguments
 *    (in) shr_mdb_list_handle_t res = pointer to the list information
 *    (in) shr_mdb_elem_index_t head = head element of block
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *  Notes
 */
static int
_mdb_user_list_remove(shr_mdb_list_handle_t res,
                      shr_mdb_elem_index_t head)
{
    shr_mdb_elem_index_t block;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,%08X) enter\n"),
               PTR_TO_INT(res),
               head));

    /* bias the block to internal value */
    block = head - res->low;
    /* make sure the block is valid */
    if ((head < res->low) || (block >= res->count)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: element %08X is not valid\n"),
                   PTR_TO_INT(res),
                   head));
        /* don't ask me why, but BCM likes NOT_FOUND for INVALID_ID */
        return _SHR_E_NOT_FOUND;
    }
    /* make sure we have the block's head */
    _mdb_block_head_get(res, &block);
    /* make sure the block is not free */
    if (res->freeLists > res->elem[block].list) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: block at %08X..%08X is free\n"),
                   PTR_TO_INT(res),
                   block + res->low,
                   block + res->elem[block].count + res->low - 1));
        return _SHR_E_NOT_FOUND;
    }
    /* make sure the block is already in a list */
    if (_MDB_BLOCK_NOT_IN_LIST == res->elem[block].list) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: block at %08X..%08X not in a list\n"),
                   PTR_TO_INT(res),
                   block + res->low,
                   block + res->elem[block].count + res->low - 1));
        return _SHR_E_EMPTY;
    }
    /* looks okay; insert the block to the list */
    _mdb_list_remove(res, block);

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,%08X) return %d (%s)\n"),
               PTR_TO_INT(res),
               head,
               _SHR_E_NONE,
               _SHR_ERRMSG(_SHR_E_NONE)));
    return _SHR_E_NONE;
}

/*
 *  Function
 *    _mdb_user_list_get
 *  Purpose
 *    Get user list of which a block is a member.
 *  Arguments
 *    (in) shr_mdb_list_handle_t res = pointer to the list information
 *    (in) shr_mdb_elem_index_t block = head element of block
 *    (out) shr_mdb_elem_bank_index_t *list = where to put the list
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *  Notes
 */
static int
_mdb_user_list_get(shr_mdb_list_handle_t res,
                   shr_mdb_elem_index_t head,
                   shr_mdb_elem_bank_index_t *list)
{
    shr_mdb_elem_index_t block;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,%08X,*) enter\n"),
               PTR_TO_INT(res),
               head));

    /* bias the block to internal value */
    block = head - res->low;
    /* make sure the block is valid */
    if ((head < res->low) || (block >= res->count)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: element %08X is not valid\n"),
                   PTR_TO_INT(res),
                   head));
        /* don't ask me why, but BCM likes NOT_FOUND for INVALID_ID */
        return _SHR_E_NOT_FOUND;
    }
    /* make sure we have the block's head */
    _mdb_block_head_get(res, &block);
    /* make sure the block is not free */
    if (res->freeLists > res->elem[block].list) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: block at %08X..%08X is free\n"),
                   PTR_TO_INT(res),
                   block + res->low,
                   block + res->elem[block].count + res->low - 1));
        return _SHR_E_NOT_FOUND;
    }
    /* make sure the block is already in a list */
    if (_MDB_BLOCK_NOT_IN_LIST == res->elem[block].list) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: block at %08X..%08X not in a list\n"),
                   PTR_TO_INT(res),
                   block + res->low,
                   block + res->elem[block].count + res->low - 1));
        return _SHR_E_EMPTY;
    }
    /* okay, return the list on which the block belongs, biased */
    *list = res->elem[block].list - res->freeLists;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,%08X,&(%d)) return %d (%s)\n"),
               PTR_TO_INT(res),
               head,
               *list,
               _SHR_E_NONE,
               _SHR_ERRMSG(_SHR_E_NONE)));
    return _SHR_E_NONE;
}

/*
 *  Function
 *    _mdb_user_list_head
 *  Purpose
 *    Get first block in specified user list.
 *  Arguments
 *    (in) shr_mdb_list_handle_t res = pointer to the list information
 *    (in) shr_mdb_elem_bank_index_t list = the list whose head is to be fetched
 *    (in) shr_mdb_elem_index_t *head = where to put head element of head block
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *  Notes
 */
static int
_mdb_user_list_head(shr_mdb_list_handle_t res,
                    shr_mdb_elem_bank_index_t list,
                    shr_mdb_elem_index_t *head)
{
    shr_mdb_elem_bank_index_t bank;
    shr_mdb_elem_bank_index_t offs;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,%d,*) enter\n"),
               PTR_TO_INT(res),
               list));

    if (list >= res->userLists) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: there is no list %d\n"),
                   PTR_TO_INT(res),
                   list));
        /* don't ask me why, but BCM likes NOT_FOUND for INVALID_ID */
        return _SHR_E_NOT_FOUND;
    }
    bank = res->list[list + res->freeLists].head;
    if (_MDB_BANK_END == bank) {
        /* there are no banks on this list */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: list %d has no member blocks\n"),
                   PTR_TO_INT(res),
                   list));
        return _SHR_E_EMPTY;
    }
    offs = _MDB_LIST_BANK_DESC(*res, list + res->freeLists, bank).head;
    *head = _MDB_ELEM_FROM_BANK_OFFS(*res, bank, offs);

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,%d,&(%08X)) return %d (%s)\n"),
               PTR_TO_INT(res),
               list,
               *head,
               _SHR_E_NONE,
               _SHR_ERRMSG(_SHR_E_NONE)));
    return _SHR_E_NONE;
}

/*
 *  Function
 *    _mdb_user_list_tail
 *  Purpose
 *    Get last block in specified list.
 *  Arguments
 *    (in) shr_mdb_list_handle_t res = pointer to the list information
 *    (in) shr_mdb_elem_bank_index_t list = the list whose tail is to be fetched
 *    (in) shr_mdb_elem_index_t *tail = where to put head element of tail block
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *  Notes
 */
static int
_mdb_user_list_tail(shr_mdb_list_handle_t res,
                    shr_mdb_elem_bank_index_t list,
                    shr_mdb_elem_index_t *tail)
{
    shr_mdb_elem_bank_index_t bank;
    shr_mdb_elem_bank_index_t offs;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,%d,*) enter\n"),
               PTR_TO_INT(res),
               list));

    if (list >= res->userLists) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: there is no list %d\n"),
                   PTR_TO_INT(res),
                   list));
        /* don't ask me why, but BCM likes NOT_FOUND for INVALID_ID */
        return _SHR_E_NOT_FOUND;
    }
    bank = res->list[list + res->freeLists].tail;
    if (_MDB_BANK_END == bank) {
        /* there are no banks on this list */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: list %d has no member blocks\n"),
                   PTR_TO_INT(res),
                   list));
        return _SHR_E_EMPTY;
    }
    offs = _MDB_LIST_BANK_DESC(*res, list + res->freeLists, bank).tail;
    *tail = _MDB_ELEM_FROM_BANK_OFFS(*res, bank, offs);

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,%d,&(%08X)) return %d (%s)\n"),
               PTR_TO_INT(res),
               list,
               *tail,
               _SHR_E_NONE,
               _SHR_ERRMSG(_SHR_E_NONE)));
    return _SHR_E_NONE;
}

/*
 *  Function
 *    _mdb_list_pred
 *  Purpose
 *    Get the block that is the predecessor to the provided one, on the same
 *    list.
 *  Arguments
 *    (in) shr_mdb_list_handle_t res = pointer to the list information
 *    (in) shr_mdb_elem_index_t current = block whose predecessor is to be found
 *    (in) shr_mdb_elem_index_t *pred = where to put predecessor block
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *  Notes
 */
static int
_mdb_list_pred(shr_mdb_list_handle_t res,
               shr_mdb_elem_index_t current,
               shr_mdb_elem_index_t *pred)
{
    shr_mdb_elem_index_t block;
    shr_mdb_elem_bank_index_t bank;
    shr_mdb_elem_desc_t *thisElem;
    shr_mdb_elem_bank_index_t offs;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,%08X,*) enter\n"),
               PTR_TO_INT(res),
               current));

    /* bias the block to internal value */
    block = current - res->low;
    /* make sure the block is valid */
    if ((current < res->low) || (block >= res->count)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: element %08X is not valid\n"),
                   PTR_TO_INT(res),
                   current));
        /* don't ask me why, but BCM likes NOT_FOUND for INVALID_ID */
        return _SHR_E_NOT_FOUND;
    }
    /* make sure we have the block's head */
    _mdb_block_head_get(res, &block);
    thisElem = &(res->elem[block]);
#if 0 
    /* make sure the block is not free */
    if (res->freeLists > thisElem->list) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: block at %08X..%08X is free\n"),
                   PTR_TO_INT(res),
                   block + res->low,
                   block + thisElem->count + res->low - 1));
        return _SHR_E_NOT_FOUND;
    }
#endif 
    /* make sure the block is already in a list */
    if (_MDB_BLOCK_NOT_IN_LIST == thisElem->list) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: block at %08X..%08X not in a list\n"),
                   PTR_TO_INT(res),
                   block + res->low,
                   block + thisElem->count + res->low - 1));
        return _SHR_E_EMPTY;
    }
    bank = _MDB_BANK_FROM_ELEM(*res, current);
    offs = thisElem->prev;

    if (_MDB_BLOCK_END == offs) {
        /* need to go to previous bank in this list */
        bank = _MDB_LIST_BANK_DESC(*res, thisElem->list, bank).prev;
        if (_MDB_BANK_END == bank) {
            /* can't go past beginning of list */
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("mdb %08X: block at %08X is head of list %d\n"),
                       PTR_TO_INT(res),
                       current,
                       thisElem->list));
            return _SHR_E_NOT_FOUND;
        }
        offs = _MDB_LIST_BANK_DESC(*res, thisElem->list, bank).tail;
    }
    *pred = _MDB_ELEM_FROM_BANK_OFFS(*res, bank, offs);

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,%08X,&(%08X)) return %d (%s)\n"),
               PTR_TO_INT(res),
               current,
               *pred,
               _SHR_E_NONE,
               _SHR_ERRMSG(_SHR_E_NONE)));
    return _SHR_E_NONE;
}

/*
 *  Function
 *    _mdb_list_succ
 *  Purpose
 *    Get the block that is the successor to the provided one, on the same
 *    list.
 *  Arguments
 *    (in) shr_mdb_list_handle_t res = pointer to the list information
 *    (in) shr_mdb_elem_index_t current = block whose successor is to be found
 *    (in) shr_mdb_elem_index_t *succ = where to put successor block
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *  Notes
 */
static int
_mdb_list_succ(shr_mdb_list_handle_t res,
               shr_mdb_elem_index_t current,
               shr_mdb_elem_index_t *succ)
{
    shr_mdb_elem_index_t block;
    shr_mdb_elem_bank_index_t bank;
    shr_mdb_elem_desc_t *thisElem;
    shr_mdb_elem_bank_index_t offs;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,%08X,*) enter\n"),
               PTR_TO_INT(res),
               current));

    /* bias the block to internal value */
    block = current - res->low;
    /* make sure the block is valid */
    if ((current < res->low) || (block >= res->count)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: element %08X is not valid\n"),
                   PTR_TO_INT(res),
                   current));
        /* don't ask me why, but BCM likes NOT_FOUND for INVALID_ID */
        return _SHR_E_NOT_FOUND;
    }
    /* make sure we have the block's head */
    _mdb_block_head_get(res, &block);
    thisElem = &(res->elem[block]);
#if 0 
    /* make sure the block is not free */
    if (res->freeLists > thisElem->list) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: block at %08X..%08X is free\n"),
                   PTR_TO_INT(res),
                   block + res->low,
                   block + thisElem->count + res->low - 1));
        return _SHR_E_NOT_FOUND;
    }
#endif 
    /* make sure the block is already in a list */
    if (_MDB_BLOCK_NOT_IN_LIST == thisElem->list) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: block at %08X..%08X not in a list\n"),
                   PTR_TO_INT(res),
                   block + res->low,
                   block + thisElem->count + res->low - 1));
        return _SHR_E_EMPTY;
    }
    bank = _MDB_BANK_FROM_ELEM(*res, current);
    offs = thisElem->next;

    if (_MDB_BLOCK_END == offs) {
        /* need to go to previous bank in this list */
        bank = _MDB_LIST_BANK_DESC(*res, thisElem->list, bank).next;
        if (_MDB_BANK_END == bank) {
            /* can't go past beginning of list */
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("mdb %08X: block at %08X is head of list %d\n"),
                       PTR_TO_INT(res),
                       current,
                       thisElem->list));
            return _SHR_E_NOT_FOUND;
        }
        offs = _MDB_LIST_BANK_DESC(*res, thisElem->list, bank).head;
    }
    *succ = _MDB_ELEM_FROM_BANK_OFFS(*res, bank, offs);

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,%08X,&(%08X)) return %d (%s)\n"),
               PTR_TO_INT(res),
               current,
               *succ,
               _SHR_E_NONE,
               _SHR_ERRMSG(_SHR_E_NONE)));
    return _SHR_E_NONE;
}

/*
 *  Function
 *    _mdb_user_list_purge
 *  Purpose
 *    Free all blocks in a user list
 *  Arguments
 *    (in) shr_mdb_list_handle_t res = pointer to the list information
 *    (in) shr_mdb_elem_index_t list = the list to be freed
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *  Notes
 */
static int
_mdb_user_list_purge(shr_mdb_list_handle_t res,
                     shr_mdb_elem_bank_index_t list)
{
    shr_mdb_elem_bank_index_t bank;
    shr_mdb_elem_bank_index_t nextBank;
    shr_mdb_elem_bank_index_t offs;
    shr_mdb_elem_bank_index_t nextOffs;
    shr_mdb_elem_bank_index_t listNum = list + res->freeLists;
    shr_mdb_list_bank_desc_t *thisBank;
    shr_mdb_elem_index_t elem;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,%d) enter\n"),
               PTR_TO_INT(res),
               list));

    /* make sure arguments are valid */
    if (list >= res->userLists) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: there is no list %d\n"),
                   PTR_TO_INT(res),
                   list));
        /* don't ask me why, but BCM likes NOT_FOUND for INVALID_ID */
        return _SHR_E_NOT_FOUND;
    }
    /* make sure there are blocks on the specified list */
    nextBank = res->list[listNum].head;
    if (_MDB_BANK_END == nextBank) {
        /* there are no banks on this list (but it's NOT an error!) */
        LOG_DEBUG(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: user list %d has no member blocks\n"),
                   PTR_TO_INT(res),
                   list));
        return _SHR_E_NONE;
    }
    /* go through all banks in this list */
    while (_MDB_BANK_END != nextBank) {
        bank = nextBank;
        thisBank = &(_MDB_LIST_BANK_DESC(*res, listNum, bank));
        nextBank = thisBank->next;
        nextOffs = thisBank->head;
        LOG_DEBUG(BSL_LS_SOC_COMMON,
                  (BSL_META("mdb %08X: user list %d has blocks in bank %04X,"
                            " head %04X\n"),
                   PTR_TO_INT(res),
                   list,
                   bank,
                   nextOffs));
        /* go through all blocks in this bank of this list */
        while (_MDB_BLOCK_END != nextOffs) {
            offs = nextOffs;
            elem = _MDB_ELEM_FROM_BANK_OFFS(*res, bank, offs);
            nextOffs = res->elem[elem].next;
            LOG_DEBUG(BSL_LS_SOC_COMMON,
                      (BSL_META("mdb %08X: list %d has block to free at"
                                " %04X:%04X (%08X, list %04X, size %04X,"
                                " next %04X)\n"),
                       PTR_TO_INT(res),
                       list + res->freeLists,
                       bank,
                       offs,
                       elem,
                       res->elem[elem].list,
                       res->elem[elem].count,
                       nextOffs));
            /* remove this block from the list */
            _mdb_list_remove(res, elem);
            /* defragment the block into neighbours */
            if ((res->allocPref & shr_mdb_join_free) ||
                (!(res->allocPref & (shr_mdb_join_alloc |
                                     shr_mdb_join_free)))) {
                /* need to join adjacent free blocks on free */
                _mdb_block_defrag(res, &elem, 0);
            }
            /* put the block on appropriate free lists */
            _mdb_block_frag_and_free(res, elem);
        } /* while (_MDB_BLOCK_END != nextOffs) */
    } /* while (_MDB_BANK_END != nextBank) */

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("(%08X,%d) return %d (%s)\n"),
               PTR_TO_INT(res),
               list,
               _SHR_E_NONE,
               _SHR_ERRMSG(_SHR_E_NONE)));
    return _SHR_E_NONE;
}


/*****************************************************************************
 *
 *  Interface
 *
 *  Functions exposed to be called.  These take/release the lock where
 *  appropriate, do some argument filtering, &c.  They rely upon the support or
 *  implementation sections for the real work (except in the case of create,
 *  which does a lot of accounting prep that is required before the internal
 *  and support functions will work.
 *
 *  These just deal with locking and obvious (such as null pointer) validation,
 *  and use the support or implementation functions to do the work (mostly).
 */

/*
 *  Function
 *    shr_mdb_create
 *
 *  Purpose
 *    Create an mdb type indexed resource management object, given the
 *    specified parameters for the object.
 *
 *    Elements per bank will be rounded up to the next integral power of two,
 *    and that must be <=32768.
 *
 *    A free list of block size = 1 is obligatory (and implied); additional
 *    free lists (as specified by freeLists) have their block sizes given in an
 *    array pointed to by freeCnts.
 *
 *    FreeCnts entries must be in strictly increasing order, and the largest
 *    must be less than or equal to bankSize.  The largest freeCnts entry will
 *    be the largest block that can be allocated, so at least one freeCnts
 *    entry must be greater than or equal to the largest block you will need.
 *    Ideally, freeCnts entries will tend to be the most commonly used block
 *    sizes, plus possibly some larger block sizes for more efficient
 *    management of free space.
 *
 *    UserLists specifies the number of user lists that will be included.
 *    These allow in-place use of the existing management memory to track lists
 *    of blocks for whatever purpose is necessary, and allow any size block,
 *    but these lists will be unsorted and unordered.
 *
 *  Arguments
 *    (out) shr_mdb_list_handle_t *handle = where to put the handle
 *    (in) shr_mdb_elem_bank_index_t bankSize = elements per bank
 *    (in) shr_mdb_elem_bank_index_t freeLists = number of additional free lists
 *    (in) shr_mdb_elem_bank_index_t *freeCnts = free lists elements per block
 *    (in) shr_mdb_elem_bank_index_t userLists = number of user lists
 *    (in) shr_mdb_elem_index_t first = lowest element number to manage
 *    (in) shr_mdb_elem_index_t last = highest element number to manage
 *    (in) int lock = TRUE if should be a lock; FALSE if caller will protect
 *
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *
 *  Notes
 *    none
 */
int
shr_mdb_create(shr_mdb_list_handle_t *handle,
               shr_mdb_elem_bank_index_t bankSize,
               shr_mdb_elem_bank_index_t freeLists,
               shr_mdb_elem_bank_index_t *freeCnts,
               shr_mdb_elem_bank_index_t userLists,
               shr_mdb_elem_index_t first,
               shr_mdb_elem_index_t last,
               int lock)
{
    shr_mdb_list_handle_t tempList;
    shr_mdb_list_bank_desc_t bankList;
    unsigned int size;
    unsigned int shift;
    unsigned int banks;
    unsigned int lastBankSize;
    unsigned int elems;
    unsigned int lists;
    unsigned int bank;
    unsigned int list;
    int result;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(*,%d,%d,*,%d,%d,%d,%s) enter\n"),
                 bankSize,
                 freeLists,
                 userLists,
                 first,
                 last,
                 lock?"Lock":"NoLock"));

    /* validate the arguments */
    for (size = 16, shift = 4;
         size && (size < bankSize);
         size <<= 1, shift++) {
        /* find the lowest power of two >= requested bank size */
        /* but don't make it less than 16 */
    }
    if (!size || (32768 < size)) {
        /* the requested bank size is too large */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("requested bank size %d is too large\n"),
                   bankSize));
        return _SHR_E_PARAM;
    }
    if (last <= first) {
        /* there are too few elements (one, zero, or negative) to bother */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("range has negative, zero, or one element\n")));
        return _SHR_E_PARAM;
    }
    if ((freeLists + userLists) > 255) {
        /* there are too many lists */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("there are too many lists (free+user = %d, which is"
                   " > 255)\n"),
                   freeLists + userLists));
        return _SHR_E_PARAM;
    }
    if (!handle) {
        /* there's no place to put the handle */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("NULL pointer for out argument\n")));
        return _SHR_E_PARAM;
    }
    /* keep the decided bank size */
    bankSize = size;
    /* how many elements total */
    elems = (last - first) + 1;
    /* determine how many banks are needed */
    banks = elems / bankSize;
    lastBankSize = elems - (banks * bankSize);
    if (lastBankSize) {
        /* need partial last bank */
        banks++;
    } else {
        /* last bank is full size */
        lastBankSize = bankSize;
    }
    if (banks > 0xFFF0) {
        /* there are too many banks */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("there are too many banks (%d)\n"),
                   banks));
        return _SHR_E_PARAM;
    }
    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("%u elements, bankSize %u -> %u banks with"
               " last bank having %u elements\n"),
               elems,
               bankSize,
               banks,
               lastBankSize));
    /* how many lists total (free + user) */
    lists = userLists + freeLists + 1;
    /* figure out how much memory is needed */
    size = ((elems * sizeof(shr_mdb_elem_desc_t)) +              /* elements */
            (banks * lists * (sizeof(shr_mdb_list_bank_desc_t))) +/* bnk lst */
            (lists * sizeof(shr_mdb_list_desc_t)) +                 /* lists */
            (sizeof(shr_mdb_list_t)));                          /* top level */
    /* try to allocate it */
    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("allocate %d bytes for descriptor\n"),
               size));
    tempList = sal_alloc(size, "mdb_management");
    if (!tempList) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to allocate %d bytes for descriptor\n"),
                   size));
        return _SHR_E_MEMORY;
    }
    /* looks good so far */
    result = _SHR_E_NONE;
    /* prepare it */
    if (lock) {
        LOG_DEBUG(BSL_LS_SOC_COMMON,
                  (BSL_META("create lock\n")));
        tempList->lock = sal_mutex_create("mdb_management_lock");
        if (!(tempList->lock)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("unable to create lock\n")));
            result = _SHR_E_RESOURCE;
        }
    } else {
        tempList->lock = NULL;
    }
    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("prepare top-level descriptor\n")));
    tempList->low = first;
    tempList->count = elems;
    tempList->freeLists = freeLists + 1;
    tempList->userLists = userLists;
    tempList->lists = lists;
    tempList->banks = banks;
    tempList->bankShift = shift;
    tempList->bankMask = ~(bankSize - 1);
    tempList->lastBankSize = lastBankSize;
    tempList->allocPref = (shr_mdb_alloc_bank_first |
                           shr_mdb_alloc_block_high |
                           shr_mdb_free_block_high |
                           shr_mdb_join_alloc_and_free |
                           shr_mdb_join_high_and_low);
    tempList->list = (shr_mdb_list_desc_t*)(&(tempList[1]));
    tempList->listBank = (shr_mdb_list_bank_desc_t*)(&(tempList->list[lists]));
    tempList->elem = (shr_mdb_elem_desc_t*)(&(tempList->listBank[banks * lists]));
    if (_SHR_E_NONE == result) {
        /* prepare list descriptors */
        LOG_DEBUG(BSL_LS_SOC_COMMON,
                  (BSL_META("prepare list descriptors\n")));
        bankList.blocks = 0;
        bankList.elems = 0;
        bankList.head = _MDB_BLOCK_END;
        bankList.tail = _MDB_BLOCK_END;
        bankList.next = _MDB_BANK_END;
        bankList.prev = _MDB_BANK_END;
        for (list = 0; list < lists; list++) {
            tempList->list[list].blocks = 0;
            tempList->list[list].elems = 0;
            tempList->list[list].head = _MDB_BANK_END;
            tempList->list[list].tail = _MDB_BANK_END;
            if (0 == list) {
                LOG_DEBUG(BSL_LS_SOC_COMMON,
                          (BSL_META("obligatory free list %d:"
                           " blocks of 1 element\n"),
                           list));
                tempList->list[list].elemsBlock = 1;
            } else if (list <= freeLists) {
                LOG_DEBUG(BSL_LS_SOC_COMMON,
                          (BSL_META("additional free list %d:"
                           " blocks of %d elements\n"),
                           list,
                           freeCnts[list - 1]));
                tempList->list[list].elemsBlock = freeCnts[list - 1];
                if (tempList->list[list].elemsBlock <=
                    tempList->list[list - 1].elemsBlock) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META("block size in user specified free lists"
                               " is not strictly increasing\n")));
                    result = _SHR_E_PARAM;
                    break;
                }
                if (tempList->list[list].elemsBlock > bankSize) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META("block size %d exceeds bank size %d\n"),
                               tempList->list[list].elemsBlock,
                               bankSize));
                    result = _SHR_E_PARAM;
                    break;
                }
            } else {
                LOG_DEBUG(BSL_LS_SOC_COMMON,
                          (BSL_META("user list %d\n"),
                           list - (freeLists + 1)));
                tempList->list[list].elemsBlock = 0;
            }
            for (bank = 0; bank < banks; bank++) {
                _MDB_LIST_BANK_DESC(*tempList, list, bank) = bankList;
            }
        }
    } /* if (_SHR_E_NONE == result) */
    /* prepare element descriptors */
    if (_SHR_E_NONE == result) {
        LOG_DEBUG(BSL_LS_SOC_COMMON,
                  (BSL_META("prepare element descriptors (%d banks)\n"),
                   tempList->banks));
        for (bank = 0; bank < (banks - 1); bank++) {
            _mdb_block_prep(tempList,
                            _MDB_ELEM_FROM_BANK_OFFS(*tempList, bank, 0),
                            bankSize);
        }
        _mdb_block_prep(tempList,
                        _MDB_ELEM_FROM_BANK_OFFS(*tempList, bank, 0),
                        tempList->lastBankSize);
        /* assign elements to appropriate free lists */
        LOG_DEBUG(BSL_LS_SOC_COMMON,
                  (BSL_META("assign elements to appropriate free lists\n")));
        for (bank = 0; bank < banks; bank++) {
            _mdb_block_frag_and_free(tempList,
                                     _MDB_ELEM_FROM_BANK_OFFS(*tempList,
                                                              bank,
                                                              0));
        }
    } /* if (_SHR_E_NONE == result) */
    if (_SHR_E_NONE == result) {
        *handle = tempList;
    } else {
        /* something went wrong */
        if (tempList) {
            /* but we allocated the memory cell */
            if (tempList->lock) {
                /* and we somehow got a lock */
                sal_mutex_destroy(tempList->lock);
            }
            sal_free(tempList);
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(&(%08X),%d,%d,*,%d,%d,%d,%s) return %d (%s)\n"),
                 PTR_TO_INT(*handle),
                 bankSize,
                 freeLists,
                 userLists,
                 first,
                 last,
                 lock?"Lock":"NoLock",
                 result,
                 _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Function
 *    shr_mdb_destroy
 *
 *  Purpose
 *    Destroy and mdb type indexed resource management object, given the object
 *    handle.
 *
 *  Arguments
 *    (in) shr_mdb_list_handle_t handle = the handle
 *
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *
 *  Notes
 *    Must not be called concurrently with any other operation on same object.
 */
int
shr_mdb_destroy(shr_mdb_list_handle_t handle)
{
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X) enter\n"),
                 PTR_TO_INT(handle)));

    if (!handle) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("NULL handle is not acceptable\n")));
        return _SHR_E_PARAM;
    }
    if (handle->lock) {
        /* we have a lock; get rid of it */
        sal_mutex_destroy(handle->lock);
    }
    /* overwrite the top level information */
    sal_memset(handle, 0, sizeof(shr_mdb_list_t));
    /* free it */
    sal_free(handle);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X) return %d (%s)\n"),
                 PTR_TO_INT(handle),
                 _SHR_E_NONE,
                 _SHR_ERRMSG(_SHR_E_NONE)));
    return _SHR_E_NONE;
}

/*
 *  Function
 *    shr_mdb_reserve
 *
 *  Purpose
 *    Resevere elements directly.
 *
 *  Arguments
 *    (in) shr_mdb_list_handle_t handle = the handle
 *    (in) shr_mdb_elem_index_t first = the first element to reserve
 *    (in) shr_mdb_elem_index_t last = the last element to reserve
 *
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *
 *  Notes
 *    Elements to be reserved must be free, else the call will fail.
 */
int
shr_mdb_reserve(shr_mdb_list_handle_t handle,
                shr_mdb_elem_index_t first,
                shr_mdb_elem_index_t last)
{
    int result;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,%08X,%08X) enter\n"),
                 PTR_TO_INT(handle),
                 first,
                 last));

    _MDB_VALID_CHECK(handle);
    if (first > last) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("first %08X is greater than last %08X\n"),
                   first,
                   last));
        return _SHR_E_PARAM;
    }
    _MDB_LOCK_TAKE(handle);
    result = _shr_mdb_elems_reserve(handle, first, (last - first) + 1);
    _MDB_LOCK_GIVE(handle);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,%08X,%08X) return %d (%s)\n"),
                 PTR_TO_INT(handle),
                 first,
                 last,
                 result,
                 _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Function
 *    shr_mdb_unreserve
 *
 *  Purpose
 *    Unresevere elements directly.
 *
 *  Arguments
 *    (in) shr_mdb_list_handle_t handle = the handle
 *    (in) shr_mdb_elem_index_t first = the first element to unreserve
 *    (in) shr_mdb_elem_index_t last = the last element to unreserve
 *
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *
 *  Notes
 *    Elements to be unreserved must be singly allocated, else it will fail.
 */
int
shr_mdb_unreserve(shr_mdb_list_handle_t handle,
                  shr_mdb_elem_index_t first,
                  shr_mdb_elem_index_t last)
{
    int result;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,%08X,%08X) enter\n"),
                 PTR_TO_INT(handle),
                 first,
                 last));

    _MDB_VALID_CHECK(handle);
    if (first > last) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("first %08X is greater than last %08X\n"),
                   first,
                   last));
        return _SHR_E_PARAM;
    }
    _MDB_LOCK_TAKE(handle);
    result = _shr_mdb_elems_unreserve(handle, first, (last - first) + 1);
    _MDB_LOCK_GIVE(handle);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,%08X,%08X) return %d (%s)\n"),
                 PTR_TO_INT(handle),
                 first,
                 last,
                 result,
                 _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Function
 *    shr_mdb_reserve_to_block
 *
 *  Purpose
 *    Convert a set of reserved elements to a usable block.  This is meant to
 *    be used in case a reserved range must be converted into a standard block,
 *    and it must be done so in-place.
 *
 *  Arguments
 *    (in) shr_mdb_list_handle_t handle = the handle
 *    (in) shr_mdb_elem_index_t first = the first element to convert
 *    (in) shr_mdb_elem_index_t last = the last element to convert
 *
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *
 *  Notes
 *    Elements must be singly allocated, else it will fail.
 */
int
shr_mdb_reserve_to_block(shr_mdb_list_handle_t handle,
                         shr_mdb_elem_index_t first,
                         shr_mdb_elem_index_t last)
{
    int result;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,%08X,%08X) enter\n"),
                 PTR_TO_INT(handle),
                 first,
                 last));

    _MDB_VALID_CHECK(handle);
    if (first > last) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("first %08X is greater than last %08X\n"),
                   first,
                   last));
        return _SHR_E_PARAM;
    }
    _MDB_LOCK_TAKE(handle);
    result = _shr_mdb_elems_collect(handle, first, (last - first) + 1);
    _MDB_LOCK_GIVE(handle);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,%08X,%08X) return %d (%s)\n"),
                 PTR_TO_INT(handle),
                 first,
                 last,
                 result,
                 _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Function
 *    shr_mdb_alloc
 *
 *  Purpose
 *    Allocate a block of elements.
 *
 *  Arguments
 *    (in) shr_mdb_list_handle_t handle = the handle
 *    (out) shr_mdb_elem_index_t *block = where to put the block
 *    (in) shr_mdb_elem_bank_index_t count = number of elements
 *
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *
 *  Notes
 *    none
 */
int
shr_mdb_alloc(shr_mdb_list_handle_t handle,
              shr_mdb_elem_index_t *block,
              shr_mdb_elem_bank_index_t count)
{
    int result;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,*,%d) enter\n"),
                 PTR_TO_INT(handle),
                 count));

    _MDB_VALID_CHECK(handle);
    if (!block) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("NULL is unacceptable as block pointer\n")));
        return _SHR_E_PARAM;
    }
    _MDB_LOCK_TAKE(handle);
    result = _shr_mdb_block_alloc(handle, block, count);
    _MDB_LOCK_GIVE(handle);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,&(%08X),%d) return %d (%s)\n"),
                 PTR_TO_INT(handle),
                 *block,
                 count,
                 result,
                 _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Function
 *    shr_mdb_alloc_id
 *
 *  Purpose
 *    Allocate a specific block of elements.
 *
 *  Arguments
 *    (in) shr_mdb_list_handle_t handle = the handle
 *    (in) shr_mdb_elem_index_t block = the block
 *    (in) shr_mdb_elem_bank_index_t count = number of elements
 *
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *
 *  Notes
 *    none
 */
int
shr_mdb_alloc_id(shr_mdb_list_handle_t handle,
                 shr_mdb_elem_index_t block,
                 shr_mdb_elem_bank_index_t count)
{
    int result;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,%08X,%d) enter\n"),
                 PTR_TO_INT(handle),
                 block,
                 count));

    _MDB_VALID_CHECK(handle);
    _MDB_LOCK_TAKE(handle);
    result = _shr_mdb_block_alloc_id(handle, block, count);
    _MDB_LOCK_GIVE(handle);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,%08X,%d) return %d (%s)\n"),
                 PTR_TO_INT(handle),
                 block,
                 count,
                 result,
                 _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Function
 *    shr_mdb_free
 *
 *  Purpose
 *    Free a block of elements.
 *
 *  Arguments
 *    (in) shr_mdb_list_handle_t handle = the handle
 *    (in) shr_mdb_elem_index_t block = the block
 *
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *
 *  Notes
 *    none
 */
int
shr_mdb_free(shr_mdb_list_handle_t handle,
             shr_mdb_elem_index_t block)
{
    int result;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,%08X) enter\n"),
                 PTR_TO_INT(handle),
                 block));

    _MDB_VALID_CHECK(handle);
    _MDB_LOCK_TAKE(handle);
    result = _shr_mdb_block_free(handle, block);
    _MDB_LOCK_GIVE(handle);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,%08X) return %d (%s)\n"),
                 PTR_TO_INT(handle),
                 block,
                 result,
                 _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Function
 *    shr_mdb_block_size_get
 *
 *  Purpose
 *    Get the number of elements in a particular block.
 *
 *  Arguments
 *    (in) shr_mdb_list_handle_t handle = the handle
 *    (in) shr_mdb_elem_index_t *block = the block to check
 *    (out) shr_mdb_elem_bank_index_t *count = where to put the count
 *
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *
 *  Notes
 *    none
 */
int
shr_mdb_block_size_get(shr_mdb_list_handle_t handle,
                       shr_mdb_elem_index_t block,
                       shr_mdb_elem_bank_index_t *count)
{
    int result;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,%08X,*) enter\n"),
                 PTR_TO_INT(handle),
                 block));

    _MDB_VALID_CHECK(handle);
    if (!count) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("NULL is unacceptable as count pointer\n")));
        return _SHR_E_PARAM;
    }
    _MDB_LOCK_TAKE(handle);
    result = _mdb_block_size_get(handle, block, count);
    _MDB_LOCK_GIVE(handle);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,%08X,&(%d)) return %d (%s)\n"),
                 PTR_TO_INT(handle),
                 block,
                 *count,
                 result,
                 _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Function
 *    shr_mdb_list_insert
 *
 *  Purpose
 *    Insert a block of elements to a user list.
 *
 *  Arguments
 *    (in) shr_mdb_list_handle_t handle = the handle
 *    (in) shr_mdb_elem_bank_index_t list = the user list on which to insert
 *    (in) shr_mdb_elem_index_t block = the block
 *
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *
 *  Notes
 *    none
 */
int
shr_mdb_list_insert(shr_mdb_list_handle_t handle,
                    shr_mdb_elem_bank_index_t list,
                    shr_mdb_elem_index_t block)
{
    int result;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,%d,%08X) enter\n"),
                 PTR_TO_INT(handle),
                 list,
                 block));

    _MDB_VALID_CHECK(handle);
    _MDB_LOCK_TAKE(handle);
    result = _mdb_user_list_insert(handle, list, block);
    _MDB_LOCK_GIVE(handle);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,%d,%08X) return %d (%s)\n"),
                 PTR_TO_INT(handle),
                 list,
                 block,
                 result,
                 _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Function
 *    shr_mdb_list_remove
 *
 *  Purpose
 *    Insert a block of elements to a user list.
 *
 *  Arguments
 *    (in) shr_mdb_list_handle_t handle = the handle
 *    (in) shr_mdb_elem_index_t block = the block
 *
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *
 *  Notes
 *    none
 */
int
shr_mdb_list_remove(shr_mdb_list_handle_t handle,
                    shr_mdb_elem_index_t block)
{
    int result;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,%08X) enter\n"),
                 PTR_TO_INT(handle),
                 block));

    _MDB_VALID_CHECK(handle);
    _MDB_LOCK_TAKE(handle);
    result = _mdb_user_list_remove(handle, block);
    _MDB_LOCK_GIVE(handle);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,%08X) return %d (%s)\n"),
                 PTR_TO_INT(handle),
                 block,
                 result,
                 _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Function
 *    shr_mdb_list_get
 *
 *  Purpose
 *    Get the user list to which a block belongs.
 *
 *  Arguments
 *    (in) shr_mdb_list_handle_t handle = the handle
 *    (in) shr_mdb_elem_index_t *block = the block to check
 *    (out) shr_mdb_elem_bank_index_t *list = where to put the user list
 *
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *
 *  Notes
 *    none
 */
int
shr_mdb_list_get(shr_mdb_list_handle_t handle,
                 shr_mdb_elem_index_t block,
                 shr_mdb_elem_bank_index_t *list)
{
    int result;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,%08X,*) enter\n"),
                 PTR_TO_INT(handle),
                 block));

    _MDB_VALID_CHECK(handle);
    if (!list) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("NULL is unacceptable as list pointer\n")));
        return _SHR_E_PARAM;
    }
    _MDB_LOCK_TAKE(handle);
    result = _mdb_user_list_get(handle, block, list);
    _MDB_LOCK_GIVE(handle);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,%08X,&(%d)) return %d (%s)\n"),
                 PTR_TO_INT(handle),
                 block,
                 *list,
                 result,
                 _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Function
 *    shr_mdb_list_head
 *
 *  Purpose
 *    Get the head block of a user list.
 *
 *  Arguments
 *    (in) shr_mdb_list_handle_t handle = the handle
 *    (in) shr_mdb_elem_bank_index_t list = the user list on which to insert
 *    (in) shr_mdb_elem_index_t *head = where to put the head
 *
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *
 *  Notes
 *    none
 */
int
shr_mdb_list_head(shr_mdb_list_handle_t handle,
                  shr_mdb_elem_bank_index_t list,
                  shr_mdb_elem_index_t *head)
{
    int result;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,%d,*) enter\n"),
                 PTR_TO_INT(handle),
                 list));

    _MDB_VALID_CHECK(handle);
    if (!head) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("NULL is unacceptable as head pointer\n")));
        return _SHR_E_PARAM;
    }
    _MDB_LOCK_TAKE(handle);
    result = _mdb_user_list_head(handle, list, head);
    _MDB_LOCK_GIVE(handle);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,%d,&(%08X)) return %d (%s)\n"),
                 PTR_TO_INT(handle),
                 list,
                 *head,
                 result,
                 _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Function
 *    shr_mdb_list_tail
 *
 *  Purpose
 *    Get the tail block of a user list.
 *
 *  Arguments
 *    (in) shr_mdb_list_handle_t handle = the handle
 *    (in) shr_mdb_elem_bank_index_t list = the user list on which to insert
 *    (in) shr_mdb_elem_index_t *tail = where to put the tail
 *
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *
 *  Notes
 *    none
 */
int
shr_mdb_list_tail(shr_mdb_list_handle_t handle,
                  shr_mdb_elem_bank_index_t list,
                  shr_mdb_elem_index_t *tail)
{
    int result;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,%d,*) enter\n"),
                 PTR_TO_INT(handle),
                 list));

    _MDB_VALID_CHECK(handle);
    if (!tail) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("NULL is unacceptable as tail pointer\n")));
        return _SHR_E_PARAM;
    }
    _MDB_LOCK_TAKE(handle);
    result = _mdb_user_list_tail(handle, list, tail);
    _MDB_LOCK_GIVE(handle);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,%d,&(%08X)) return %d (%s)\n"),
                 PTR_TO_INT(handle),
                 list,
                 *tail,
                 result,
                 _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Function
 *    shr_mdb_list_pred
 *
 *  Purpose
 *    Get the predecessor of a block in its list.
 *
 *  Arguments
 *    (in) shr_mdb_list_handle_t handle = the handle
 *    (in) shr_mdb_elem_index_t block = the block
 *    (in) shr_mdb_elem_index_t *pred = where to put the predecessor
 *
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *
 *  Notes
 *    none
 */
int
shr_mdb_list_pred(shr_mdb_list_handle_t handle,
                  shr_mdb_elem_index_t block,
                  shr_mdb_elem_index_t *pred)
{
    int result;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,%08X,*) enter\n"),
                 PTR_TO_INT(handle),
                 block));

    _MDB_VALID_CHECK(handle);
    if (!pred) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("NULL is unacceptable as predecessor pointer\n")));
        return _SHR_E_PARAM;
    }
    _MDB_LOCK_TAKE(handle);
    result = _mdb_list_pred(handle, block, pred);
    _MDB_LOCK_GIVE(handle);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,%08X,&(%08X)) return %d (%s)\n"),
                 PTR_TO_INT(handle),
                 block,
                 *pred,
                 result,
                 _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Function
 *    shr_mdb_list_succ
 *
 *  Purpose
 *    Get the successor of a block in its list.
 *
 *  Arguments
 *    (in) shr_mdb_list_handle_t handle = the handle
 *    (in) shr_mdb_elem_index_t block = the block
 *    (in) shr_mdb_elem_index_t *succ = where to put the successor
 *
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *
 *  Notes
 *    none
 */
int
shr_mdb_list_succ(shr_mdb_list_handle_t handle,
                  shr_mdb_elem_index_t block,
                  shr_mdb_elem_index_t *succ)
{
    int result;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,%08X,*) enter\n"),
                 PTR_TO_INT(handle),
                 block));

    _MDB_VALID_CHECK(handle);
    if (!succ) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("NULL is unacceptable as predecessor pointer\n")));
        return _SHR_E_PARAM;
    }
    _MDB_LOCK_TAKE(handle);
    result = _mdb_list_succ(handle, block, succ);
    _MDB_LOCK_GIVE(handle);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,%08X,&(%08X)) return %d (%s)\n"),
                 PTR_TO_INT(handle),
                 block,
                 *succ,
                 result,
                 _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Function
 *    shr_mdb_list_purge
 *
 *  Purpose
 *    Free all blocks on the user list.
 *
 *  Arguments
 *    (in) shr_mdb_list_handle_t handle = the handle
 *    (in) shr_mdb_elem_bank_index_t list = the list to purge
 *
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *
 *  Notes
 *    none
 */
int
shr_mdb_list_purge(shr_mdb_list_handle_t handle,
                   shr_mdb_elem_bank_index_t list)
{
    int result;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,%d) enter\n"),
                 PTR_TO_INT(handle),
                 list));

    _MDB_VALID_CHECK(handle);
    _MDB_LOCK_TAKE(handle);
    result = _mdb_user_list_purge(handle, list);
    _MDB_LOCK_GIVE(handle);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,%d) return %d (%s)\n"),
                 PTR_TO_INT(handle),
                 list,
                 result,
                 _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Function
 *    shr_mdb_block_info
 *
 *  Purpose
 *    Get information about a block
 *
 *  Arguments
 *    (in) shr_mdb_list_handle_t handle = the handle
 *    (in) shr_mdb_elem_index_t block = an element in the block
 *    (out) shr_mdb_block_info_t *blockInfo = ptr to where to put list info
 *
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *
 *  Notes
 *    This only supports allocated blocks, not free blocks.
 */
int
shr_mdb_block_info(shr_mdb_list_handle_t handle,
                   shr_mdb_elem_index_t block,
                   shr_mdb_block_info_t *blockInfo)
{
    int result = _SHR_E_NONE;
    shr_mdb_elem_index_t head;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,%08X,*) enter\n"),
                 PTR_TO_INT(handle),
                 block));

    if (!blockInfo) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("NULL pointer unacceptable for"
                   " outbound argument\n")));
        return _SHR_E_PARAM;
    }
    _MDB_VALID_CHECK(handle);
    _MDB_LOCK_TAKE(handle);

    head = block - handle->low;
    if ((block < handle->low) ||
        (head >= handle->count)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("element %08X is not valid\n"),
                   block));
        /* don't ask me why, but BCM likes NOT_FOUND for INVALID_ID */
        result = _SHR_E_NOT_FOUND;
    }
    if (_SHR_E_NONE == result) {
        /* find the head of the block */
        _mdb_block_head_get(handle, &head);
        if (handle->elem[head].list < handle->freeLists) {
            /* this thing is free; still an error */
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("element %08X is free\n"),
                       block));
            result = _SHR_E_NOT_FOUND;
        }
    }
    if (_SHR_E_NONE == result) {
        /* fill in the data for the block */
        blockInfo->head = head + handle->low;
        blockInfo->list = handle->elem[head].list;
        blockInfo->size = handle->elem[head].count;
    }

    _MDB_LOCK_GIVE(handle);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,%d,&{%d,%d,%d}) return %d (%s)\n"),
                 PTR_TO_INT(handle),
                 block,
                 blockInfo->head,
                 blockInfo->list,
                 blockInfo->size,
                 result,
                 _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Function
 *    shr_mdb_block_check_all
 *
 *  Purpose
 *    Find out whether a group of elements is free, used by a single block,
 *    used by more than one block, or a combination of free and used.
 *
 *  Arguments
 *    (in) shr_mdb_list_handle_t handle = the handle
 *    (in) shr_mdb_elem_index_t first = first element to check
 *    (in) shr_mdb_elem_bank_index_t count = number of elements to check
 *
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_EMPTY if none of the elements are in use
 *      _SHR_E_FULL if all of the elements are in use
 *      _SHR_E_CONFIG if elements are in use but block(s) do not match
 *      _SHR_E_EXISTS if some of the elements are in use but not all of them
 *      _SHR_E_PARAM if any of the elements is not valid
 *      _SHR_E_* otherwise as appropriate
 *
 *  Notes
 *      _SHR_E_NONE is not returned here.
 */
int
shr_mdb_block_check_all(shr_mdb_list_handle_t handle,
                        shr_mdb_elem_index_t first,
                        shr_mdb_elem_bank_index_t count)
{
    int result = _SHR_E_NONE;
    shr_mdb_elem_index_t head;
    shr_mdb_elem_bank_index_t dist;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,%08X,%d) enter\n"),
                 PTR_TO_INT(handle),
                 first,
                 count));

    _MDB_VALID_CHECK(handle);
    _MDB_LOCK_TAKE(handle);
    head = first - handle->low;
    if ((first < handle->low) ||
        ((head + count) > handle->count)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("element range %08X..%08X is not valid\n"),
                   first,
                   count));
        /* for this function, we specifically want _SHR_E_PARAM */
        result = _SHR_E_PARAM;
    }
    if (_SHR_E_NONE == result) {
        /* find the head of the block */
        _mdb_block_head_get(handle, &head);
        if (handle->elem[head].list < handle->freeLists) {
            /* this block is free */
            result = _SHR_E_EMPTY;
            /* scan to at least end of specified block */
            dist = 0;
            do {
                dist += handle->elem[head].count;
                head += handle->elem[head].count;
                if (handle->elem[head].list >= handle->freeLists) {
                    /* this block is not free */
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META("block including element %08X (%d elems)"
                               " is partially free and partially used\n"),
                               first,
                               count));
                    result = _SHR_E_EXISTS;
                    break;
                }
            } while (dist < count);
        } else { /* if (handle->elem[head].list < handle->freeLists) */
            /* this block is in use */
            if (((head + handle->low) == first) &&
                (count == handle->elem[head].count)) {
                /* block is as caller expected it to be */
                result = _SHR_E_FULL;
            } else {
                /* block exists but not as caller expected it to be */
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META("block including element %08X (%d elems)"
                           " actually starts at %08X with %d elems\n"),
                           first,
                           count,
                           head + handle->low,
                           handle->elem[head].count));
                result = _SHR_E_CONFIG;
            }
        } /* if (handle->elem[head].list < handle->freeLists) */
    } /* if (_SHR_E_NONE == result) */

    _MDB_LOCK_GIVE(handle);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,%08X,%d) return %d (%s)\n"),
                 PTR_TO_INT(handle),
                 first,
                 count,
                 result,
                 _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Function
 *    shr_mdb_list_info
 *
 *  Purpose
 *    Get information about a list (user or free)
 *
 *  Arguments
 *    (in) shr_mdb_list_handle_t handle = the handle
 *    (in) shr_mdb_elem_bank_index_t list = the list to check
 *    (in) int free = TRUE to query a free list, FALSE for a user list
 *    (out) shr_mdb_list_info_t *listInfo = ptr to where to put list info
 *
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *
 *  Notes
 *    none
 */
int
shr_mdb_list_info(shr_mdb_list_handle_t handle,
                  shr_mdb_elem_bank_index_t list,
                  int free,
                  shr_mdb_list_info_t *listInfo)
{
    int result = _SHR_E_NONE;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,%d,%s,*) enter\n"),
                 PTR_TO_INT(handle),
                 list,
                 free?"Free":"User"));

    if (!listInfo) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("NULL pointer unacceptable for"
                   " outbound argument\n")));
        return _SHR_E_PARAM;
    }
    _MDB_VALID_CHECK(handle);
    _MDB_LOCK_TAKE(handle);

    if (free) {
        if (list >= handle->freeLists) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("there are not %d free lists\n"),
                       list));
            /* don't ask me why, but BCM likes NOT_FOUND for INVALID_ID */
            result = _SHR_E_NOT_FOUND;
        }
    } else {
        if (list >= handle->userLists) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("there are not %d user lists\n"),
                       list));
            /* don't ask me why, but BCM likes NOT_FOUND for INVALID_ID */
            result = _SHR_E_NOT_FOUND;
        }
        list += handle->freeLists;
    }
    if (_SHR_E_NONE == result) {
        /* fill in the details about the list */
        listInfo->block_size = handle->list[list].elemsBlock;
        listInfo->blocks = handle->list[list].blocks;
        listInfo->elements = handle->list[list].elems;
    }

    _MDB_LOCK_GIVE(handle);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,%d,%s,*) return %d (%s)\n"),
                 PTR_TO_INT(handle),
                 list,
                 free?"Free":"User",
                 result,
                 _SHR_ERRMSG(result)));
    return result;
}

/*
 *  Function
 *    shr_mdb_info
 *
 *  Purpose
 *    Get information about an mdb object
 *
 *  Arguments
 *    (in) shr_mdb_list_handle_t handle = the handle
 *    (out) shr_mdb_list_info_t *mdbInfo = where to put mdb information
 *
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *
 *  Notes
 *    none
 */
int
shr_mdb_info(shr_mdb_list_handle_t handle,
             shr_mdb_info_t *mdbInfo)
{
    shr_mdb_elem_bank_index_t list;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,*) enter\n"),
                 PTR_TO_INT(handle)));

    if (!mdbInfo) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("NULL pointer unacceptable for"
                   " outbound argument\n")));
        return _SHR_E_PARAM;
    }
    _MDB_VALID_CHECK(handle);
    _MDB_LOCK_TAKE(handle);

    mdbInfo->first = handle->low;
    mdbInfo->last = handle->low + handle->count - 1;
    mdbInfo->free_lists = handle->freeLists;
    mdbInfo->user_lists = handle->userLists;
    mdbInfo->bank_size = 1 << handle->bankShift;
    mdbInfo->lock = !(!(handle->lock));
    mdbInfo->free_blocks = 0;
    mdbInfo->free_elems = 0;
    for (list = 0; list < handle->freeLists; list++) {
        mdbInfo->free_blocks += handle->list[list].blocks;
        mdbInfo->free_elems += handle->list[list].elems;
    }

    _MDB_LOCK_GIVE(handle);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,*) return %d (%s)\n"),
                 PTR_TO_INT(handle),
                 _SHR_E_NONE,
                 _SHR_ERRMSG(_SHR_E_NONE)));
    return _SHR_E_NONE;
}

/*
 *  Function
 *    shr_mdb_allocmode_get
 *
 *  Purpose
 *    Get an mdb object's current allocation mode
 *
 *  Arguments
 *    (in) shr_mdb_list_handle_t handle = the handle
 *    (out) shr_mdb_alloc_pref_t *allocmode = where to put alloc mode
 *
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *
 *  Notes
 *    none
 */
int
shr_mdb_allocmode_get(shr_mdb_list_handle_t handle,
                      shr_mdb_alloc_pref_t *allocmode)
{
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,*) enter\n"),
                 PTR_TO_INT(handle)));

    if (!allocmode) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("NULL pointer unacceptable for"
                   " outbound argument\n")));
        return _SHR_E_PARAM;
    }
    _MDB_VALID_CHECK(handle);
    _MDB_LOCK_TAKE(handle);

    *allocmode = handle->allocPref;

    _MDB_LOCK_GIVE(handle);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,&(%08X)) return %d (%s)\n"),
                 PTR_TO_INT(handle),
                 *allocmode,
                 _SHR_E_NONE,
                 _SHR_ERRMSG(_SHR_E_NONE)));
    return _SHR_E_NONE;
}

/*
 *  Function
 *    shr_mdb_allocmode_set
 *
 *  Purpose
 *    Set an mdb object's current allocation mode
 *
 *  Arguments
 *    (in) shr_mdb_list_handle_t handle = the handle
 *    (in) shr_mdb_alloc_pref_t allocmode = new alloc mode
 *
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *
 *  Notes
 *    none
 */
int
shr_mdb_allocmode_set(shr_mdb_list_handle_t handle,
                      shr_mdb_alloc_pref_t allocmode)
{
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,*) enter\n"),
                 PTR_TO_INT(handle)));

    /* verify the arguments */
    switch (allocmode & shr_mdb_alloc_bank_mask) {
    case shr_mdb_alloc_bank_first:
    case shr_mdb_alloc_bank_high:
    case shr_mdb_alloc_bank_low:
        /* all of these are good */
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("invalid alloc mechanism %d\n"),
                   allocmode & shr_mdb_alloc_bank_mask));
        return _SHR_E_PARAM;
    }
    if (!(allocmode & shr_mdb_join_alloc_and_free)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("must join on free, alloc, or both\n")));
        return _SHR_E_PARAM;
    }
    if (!(allocmode & shr_mdb_join_high_and_low)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("must join high, low, or both\n")));
        return _SHR_E_PARAM;
    }
    if (allocmode & (~(shr_mdb_alloc_bank_mask |
                       shr_mdb_alloc_block_high |
                       shr_mdb_free_block_high |
                       shr_mdb_join_alloc_and_free |
                       shr_mdb_join_high_and_low))) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("invalid bits are set in allocmode (%08X)\n"),
                   allocmode & (~(shr_mdb_alloc_bank_mask |
                   shr_mdb_alloc_block_high |
                   shr_mdb_free_block_high |
                   shr_mdb_join_alloc_and_free |
                   shr_mdb_join_high_and_low))));
        return _SHR_E_PARAM;
    }

    _MDB_VALID_CHECK(handle);
    _MDB_LOCK_TAKE(handle);

    handle->allocPref = allocmode;

    _MDB_LOCK_GIVE(handle);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,&(%08X)) return %d (%s)\n"),
                 PTR_TO_INT(handle),
                 allocmode,
                 _SHR_E_NONE,
                 _SHR_ERRMSG(_SHR_E_NONE)));
    return _SHR_E_NONE;
}

/*
 *  Function
 *    shr_mdb_all_free_to_user_list
 *
 *  Purpose
 *    Collect all free blocks and place them on a specific user list
 *
 *  Arguments
 *    (in) shr_mdb_list_handle_t handle = the handle
 *    (in) shr_mdb_elem_bank_index_t list = the target user list
 *
 *  Return
 *    bcm_error_t cast as int
 *      _SHR_E_NONE if successful
 *      _SHR_E_* otherwise as appropriate
 *
 *  Notes
 *    none
 */
int
shr_mdb_all_free_to_user_list(shr_mdb_list_handle_t handle,
                              shr_mdb_elem_bank_index_t userList)
{
    int result = _SHR_E_NONE;
    shr_mdb_elem_bank_index_t list;
    shr_mdb_elem_bank_index_t offs;
    shr_mdb_elem_index_t bank;
    shr_mdb_elem_index_t block = ~0;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,%d) enter\n"),
                 PTR_TO_INT(handle),
                 userList));

    _MDB_VALID_CHECK(handle);
    _MDB_LOCK_TAKE(handle);

    if (userList < handle->userLists) {
        /* for each free list */
        for (list = 0;
             list < handle->freeLists;
             list++) {
            /* while the free list contains blocks */
            while (handle->list[list].blocks) {
                /* find the first block on this list */
                bank = handle->list[list].head;
                offs = _MDB_LIST_BANK_DESC(*handle, list, bank).head;
                block = (bank << handle->bankShift) | offs;
                /* take the block out of its list */
                _mdb_list_remove(handle, block);
                _mdb_list_insert(handle, block, userList + handle->freeLists);
            } /* while (the free list has blocks) */
        } /* for (each free list) */
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("user list %d invalid for mdb %08X\n"),
                   userList,
                   PTR_TO_INT(handle)));
        result = _SHR_E_PARAM;
    }

    _MDB_LOCK_GIVE(handle);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("(%08X,%d) return %d (%s)\n"),
                 PTR_TO_INT(handle),
                 userList,
                 result,
                 _SHR_ERRMSG(result)));
    return result;
}






