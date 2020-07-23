
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Generic Memory Manager using AVL Tree
 *
 * NOTE: These routines are NOT thread-safe.  If multiple threads could
 * access an AVL tree, external locks must be used to mutually exclude
 * each call to the shr_avl routines, including shr_avl_traverse().
 */
#include <shared/bsl.h>

#include <sal/types.h>
#include <shared/alloc.h>
#include <sal/core/libc.h>
#include <shared/mem_avl.h>
#include <assert.h>

#define MEM_AVL_DEBUG
#ifdef MEM_AVL_DEBUG
#include <soc/cm.h>
#endif
/*
 *
 *     The memory management scheme works as follows:
 *
 *     Structure:
 *         AVL Tree:
 *           Each node in the tree describes a free block of memory.
 *           Said node has the number of memory units contained in the block,
 *           as well as the starting address (within the block) of the
 *           block.
 *         Doubly Linked List:
 *           The nodes within the aforementioned tree have previous and next
 *           pointers to allow for a DLL.  This DLL is used to coalesce
 *           adjacent free blocks, reducing fragmentation, and allowing for
 *           larger blocks to be allocated.
 *
 *     Initialization:
 *           We start with a single node avl tree.  This is deemed the 'free'
 *           block tree.  The node has the entire memory to be managed and is
 *           the head of our doubly linked list.
 *
 *     Allocation:
 *           We execute a best fit search of the free tree.
 *           The node returned to us is removed from the tree.  If the node
 *           returned to us is larger than our need, we split off the
 *           unused portion and add said portion back into the tree.
 *           We mark the allocated portion used, insert the remainder
 *           into the DLL (the allocated node was already in the DLL).
 *           We then return the memory address to the user.
 *
 *     ReAlloc:
 *           If the realloc size is identical, we simply return the same
 *           address to the user.
 *           Otherwise, we free the associated block, (with possible
 *           coalescence), and allocate a fresh block.
 *           One item of note, this implementation of realloc does not
 *           copy the original contents of the memory to the new location.
 *
 *     Free:
 *           Find the node (within the DLL) associated with this block of
 *           memory.  If the previous block is free also, coalesce the two
 *           blocks into one.  If the next block is free, coalesce that block
 *           into this one.  Mark the memory as free in the DLL, and add it to
 *           the free tree.
 *
 * LANGUAGE:
 *
 *     C
 *
 * AUTHORS:
 *
 *     Travis B. Sawyer, Sean Campbell
 *
 * CREATION DATE:
 *
 *     21-June-2005
 *     Rewritten to use AVL free tree on:
 *     27-July-2005
 */

/*
 * Internally used functions
 */
static void shr_mem_avl_list_tree(shr_avl_entry_t *a, int m);
static int _shr_mem_avl_remove(shr_mem_avl_t *mem_avl,
                               shr_mem_avl_entry_pt entry);
static int _shr_mem_avl_compare(void *user_data,
                                shr_avl_datum_t *a,
                                shr_avl_datum_t *b);
static int _shr_mem_avl_dll_insert(shr_mem_avl_t* mem_avl,
                                   shr_mem_avl_entry_pt entry);
static int _shr_mem_avl_dll_remove(shr_mem_avl_t *mem_avl,
                                   shr_mem_avl_entry_pt entry);
static shr_mem_avl_entry_pt _shr_mem_avl_dll_search(shr_mem_avl_t *mem_avl, 
                                                    unsigned int addr,
                                                    unsigned int loose);

static void _shr_mem_avl_find_free_count(shr_avl_entry_t* entry,
                                        int size,
                                        int *count);

int shr_mem_avl_create(shr_mem_avl_t **mem_avl_ptr,
                       int mem_size,
                       int mem_base,
                       int max_blocks)
{
    int status = 0;
    shr_mem_avl_entry_t *entry;
    shr_mem_avl_t *mem_avl;

    mem_avl = sal_alloc(sizeof(shr_mem_avl_t), "mem_avl");
    if (mem_avl == NULL) {
        return -1;
    }
    
    sal_memset(mem_avl, 0, sizeof(*mem_avl));

    *mem_avl_ptr = mem_avl;

    /* create the AVL tree.
       Datum is shr_mem_avl_entry_s */
    status = shr_avl_create(&mem_avl->tree, NULL, 
                             sizeof(shr_mem_avl_entry_t), max_blocks);
    if (status != 0) {
        return status;
    }

    /* Initialize and create a single entry with the whole mem */
    entry = sal_alloc(sizeof(shr_mem_avl_entry_t), "mem_avl");

    if (entry == NULL) {
        return -1;
    }

    sal_memset(entry, 0, sizeof(*entry));

    /* Initialize doubly linked list (for block coalesence) */
    entry->size = mem_size;
    entry->addr = mem_base;
    entry->next = NULL;
    entry->prev = NULL;
    entry->self = entry;

    mem_avl->mem_list = entry;

    /* insert the entry into the avl tree */
    status = shr_avl_insert(mem_avl->tree, _shr_mem_avl_compare, 
                   (shr_avl_datum_t*)((mem_avl->mem_list)));

    return( status );
}

static int _shr_mem_avl_compare(void *user_data, shr_avl_datum_t *a, 
                                    shr_avl_datum_t *b)
{
    int size_diff;

    COMPILER_REFERENCE(user_data);

    size_diff = (((shr_mem_avl_entry_pt)(a))->size - 
                 ((shr_mem_avl_entry_pt)(b))->size);

    if (!size_diff) {
        /* both entries are of same size...check the address. If both have
         * same address (which means its the same entry) return 0 else 1 
         */
        if (((shr_mem_avl_entry_pt)(a))->addr ==
              ((shr_mem_avl_entry_pt)(b))->addr) {
            return 0;
        }
        if (((shr_mem_avl_entry_pt)(a))->addr >
              ((shr_mem_avl_entry_pt)(b))->addr) {
            return 1;
        }
            return -1;
    }

    return size_diff;
}
int shr_mem_avl_destroy(shr_mem_avl_t *mem_avl)
{
    int status, rv = 0;
    shr_mem_avl_entry_pt entry;
    shr_mem_avl_entry_pt next;

    /* destroy the avl tree. Frees the space for datum's also. */
    status = shr_avl_destroy(mem_avl->tree);
    if (status != 0) {
        rv = -1;
    }
    mem_avl->tree = NULL;

    /*
     * Now walk the dll and free each entry
     */
    entry = mem_avl->mem_list;

    while (NULL != entry) {
        next = entry->next;
        sal_free(entry);
        entry = next;
    }
    mem_avl->mem_list = NULL;

    return rv;
}

static int _shr_mem_avl_avl_search_compare(shr_avl_datum_t *a, 
                                          shr_avl_datum_t *b)
{
    return (((shr_mem_avl_entry_pt)(a))->size - 
            ((shr_mem_avl_entry_pt)(b))->size);
}

/* performs bestfit search of the avl tree to find the free block */
static shr_avl_datum_t* _shr_mem_avl_avl_search(shr_avl_t *tree, 
                                                shr_avl_datum_t *datum)
{
    int                 cmp_rv;
    shr_avl_datum_t     *result = NULL;
    shr_avl_t           sub_tree;

    if ((!tree) || (!tree->root)) {
        return NULL;
    }

    cmp_rv = _shr_mem_avl_avl_search_compare(&tree->root->datum, datum);

    if (cmp_rv > 0) {
        /* search left sub-tree */
        if (tree->root->left != NULL) {
            sub_tree.root = tree->root->left;
            result = _shr_mem_avl_avl_search(&sub_tree, datum);
            if (result != NULL) {
                cmp_rv = _shr_mem_avl_avl_search_compare(result, datum);
                if (cmp_rv < 0) {
                    result = &tree->root->datum;
                }
            }
        } else {
            result = &tree->root->datum;
        }
    }

    if (cmp_rv == 0) {
        result = &tree->root->datum;
    }

    if (cmp_rv < 0) {
        /* search right sub-tree */
        if (tree->root->right != NULL) {
            sub_tree.root = tree->root->right;
            result = _shr_mem_avl_avl_search(&sub_tree, datum);
            if (result != NULL) {
                cmp_rv = _shr_mem_avl_avl_search_compare(result, datum);
                if (cmp_rv < 0) {
                    result = &tree->root->datum;
                }
            }
        } else {
            result = &tree->root->datum;
        }
    }

    return result;
}

int shr_mem_avl_malloc(shr_mem_avl_t *mem_avl, int size, unsigned int *addr)
{
    shr_mem_avl_entry_t entry;
    shr_mem_avl_entry_pt curr_entry;
    shr_mem_avl_entry_pt split_entry;
    void *pTmpVoid;
#ifndef NDEBUG
    int nStatus;
#endif /* !NDEBUG*/
    shr_avl_datum_t *pDatumSearch, *pDatum;

    assert(mem_avl != 0);
    assert(addr);

    if (0 == size) {
        /*
         * What does malloc do here?
         */
        return( -1 );
    }

    /*
     * Need to search the tree to find the best fit.
     */
    sal_memset(&entry, 0, sizeof(entry));
    entry.size = size;
    pDatumSearch = (shr_avl_datum_t *) &entry;

    pDatum = _shr_mem_avl_avl_search(mem_avl->tree, pDatumSearch);
    if (NULL == pDatum) {
        /* OOM */
        return( -1 );
    }
    curr_entry = ((shr_mem_avl_entry_pt)(pDatum))->self;

    if (size > curr_entry->size) {
        /* OOM */
        return( -1 );
    }

    /*
     * Remove the node from the free tree
     */
    /*
     * ENHANCEME: do something with the status from avl_remove
     */
#ifndef NDEBUG
    nStatus = 
#endif /* !NDEBUG*/
    _shr_mem_avl_remove(mem_avl, curr_entry);

    assert(nStatus >= 0);

    /*
     * We have a 'free' block of memory.  It may be larger than we need,
     * if it is, we need to split off the extra and put that back into
     * the free tree
     */
    if (size < curr_entry->size) {
        /*
         * The search returned a free block that is greater than
         * what we need.  We need to split off the unused lines,
         * and add that back to the free tree.
         */
        pTmpVoid = NULL;
        pTmpVoid = sal_alloc(sizeof(*split_entry), "mem_avl");
        if (NULL == pTmpVoid) {
            /* OOSM */
            return( -1 );
        }

        split_entry = (shr_mem_avl_entry_pt)pTmpVoid;

        sal_memset(split_entry, 0, sizeof(*split_entry));
        split_entry->size = curr_entry->size - size;
        split_entry->addr = curr_entry->addr + entry.size;
        split_entry->next = NULL;
        split_entry->prev = NULL;
        split_entry->self = split_entry;

        /*
         * Since we're splitting, we have to update the 'allocated' node's
         * size appropriately
         */
        curr_entry->size = size;


        /*
         * ENHANCEME: do something with the status from avl_insert
         */
#ifndef NDEBUG
        nStatus = 
#endif /*!NDEBUG*/
        shr_avl_insert(mem_avl->tree, _shr_mem_avl_compare, 
                                 (shr_avl_datum_t*)split_entry);
        assert(nStatus >= 0);
        _shr_mem_avl_dll_insert(mem_avl, split_entry);
    }

    /*
     * Now take care of the block we're interested in
     */
    *addr = curr_entry->addr;
    curr_entry->used = 1;

    return( 0 );
}

int shr_mem_avl_realloc(shr_mem_avl_t *mem_avl, int size, unsigned int addr)
{
    shr_mem_avl_entry_pt curr_entry;
    shr_mem_avl_entry_pt split_entry;
    void *pTmpVoid;
#ifndef NDEBUG
    int nStatus;
#endif /* !NDEBUG*/

    assert(mem_avl != 0);

    if (0 == size) {
        /*
         * What does realloc do here?
         */
        return( -1 );
    }

    /* use loose search to find target block */
    curr_entry = _shr_mem_avl_dll_search(mem_avl, addr, 1);
    if (NULL == curr_entry) {
        /*
         * This is bad, we couldn't find a
         * block containing this address
         */
        return( -1 );
    }

    /* verify size and availability */
    if ( ((curr_entry->addr + curr_entry->size - 1) < (addr + size - 1)) ||
          (curr_entry->used) ) {
        return( -1 );
    }

    /*
     * Remove the node from the free tree
     */
    /*
     * ENHANCEME: do something with the status from avl_remove
     */
#ifndef NDEBUG
    nStatus =
#endif /* !NDEBUG*/
    _shr_mem_avl_remove(mem_avl, curr_entry);
    assert(nStatus >= 0);

    /*
     * We have a 'free' block of memory.  The starting address of this
     * block may not matched the target.  If not, split off the first
     * portian and place it back into the free tree
     */
    if (addr > curr_entry->addr) {
        /*
         * The search returned a free block that contains the
         * desired block.  The start address is not a match so we
         * need to split off the portion prior to the target
         * address and add it back to the free tree.
         */
        pTmpVoid = NULL;
        pTmpVoid = sal_alloc(sizeof(*split_entry), "mem_avl");
        if (NULL == pTmpVoid) {
            /* OOSM */
            return( -1 );
        }

        split_entry = (shr_mem_avl_entry_pt)pTmpVoid;

        sal_memset(split_entry, 0, sizeof(*split_entry));
        split_entry->size = addr - curr_entry->addr;
        split_entry->addr = curr_entry->addr;
        split_entry->next = NULL;
        split_entry->prev = NULL;
        split_entry->self = split_entry;

        /*
         * Since we're splitting, we have to update the 'allocated' node's
         * size and address appropriately
         */
        curr_entry->addr = addr;
        curr_entry->size = curr_entry->size - split_entry->size;


        /*
         * ENHANCEME: do something with the status from avl_insert
         */
#ifndef NDEBUG
        nStatus =
#endif /* !NDEBUG*/
        shr_avl_insert(mem_avl->tree, _shr_mem_avl_compare, 
                                 (shr_avl_datum_t*)split_entry);
        assert(nStatus >= 0);
        _shr_mem_avl_dll_insert(mem_avl, split_entry);
    }

    if (size < curr_entry->size) {
        /*
         * The search returned a free block that is greater than
         * what we need.  We need to split off the unused lines,
         * and add that back to the free tree.
         */
        pTmpVoid = NULL;
        pTmpVoid = sal_alloc(sizeof(*split_entry), "mem_avl");
        if (NULL == pTmpVoid) {
            /* OOSM */
            return( -1 );
        }

        split_entry = (shr_mem_avl_entry_pt)pTmpVoid;

        sal_memset(split_entry, 0, sizeof(*split_entry));
        split_entry->size = curr_entry->size - size;
        split_entry->addr = curr_entry->addr + size;
        split_entry->next = NULL;
        split_entry->prev = NULL;
        split_entry->self = split_entry;

        /*
         * Since we're splitting, we have to update the 'allocated' node's
         * size appropriately
         */
        curr_entry->size = size;


        /*
         * ENHANCEME: do something with the status from avl_insert
         */
#ifndef NDEBUG
        nStatus = 
#endif /* !NDEBUG*/
        shr_avl_insert(mem_avl->tree, _shr_mem_avl_compare, 
                                 (shr_avl_datum_t*)split_entry);
        assert( nStatus >= 0);
        _shr_mem_avl_dll_insert(mem_avl, split_entry);
    }

    /*
     * Now take care of the block we're interested in
     */
    curr_entry->used = 1;

    return( 0 );
}
/*
 * Doubly Linked List functions
 */
static shr_mem_avl_entry_pt _shr_mem_avl_dll_search(shr_mem_avl_t *mem_avl,
                                                    unsigned int addr,
                                                    unsigned int loose)
{
    shr_mem_avl_entry_pt entry;
    unsigned int curr_addr;

    assert(mem_avl);

    entry = mem_avl->mem_list;

    while (NULL != entry) {
        curr_addr = entry->addr;
        if (curr_addr == addr) {
            return( entry );
        }
        if ( (loose) && 
             (curr_addr < addr) &&
             ((curr_addr + entry->size - 1) > addr) ) {
            return( entry );
        }

        entry = entry->next;
    }

    /*
     * If we got here, we didn't find the appropriate entry
     */
    return( NULL );
}

static int _shr_mem_avl_dll_insert(shr_mem_avl_t* mem_avl,
                                   shr_mem_avl_entry_pt entry)
{
    shr_mem_avl_entry_pt curr_entry;
    unsigned int curr_addr;
    unsigned int insert_addr;

    assert(mem_avl);
    assert(entry);

    curr_entry = mem_avl->mem_list;

    /*
     * Search for the first entry that has an address that is greater
     * than our insert address.  Once found, back track one entry and
     * insert the new one before the found
     * entry.  If we don't find said entry, insert it at the tail.
     * We assume we cannot have the same address twice in the DLL.
     */
    insert_addr = entry->addr;
    while (NULL != curr_entry) {
        curr_addr = curr_entry->addr;

        if (curr_addr > insert_addr) {
            curr_entry = curr_entry->prev;
            break;
        }

        if (NULL == curr_entry->next) {
            break;
        }
        curr_entry = curr_entry->next;
    }

    /*
     * Insert after the entry we found.
     * Since this is an ordered list, and the head is initialized
     * We need not worry about inserting at the head.
     */
    entry->prev = curr_entry;
    entry->next = curr_entry->next;
/*    entry->used = 1; */
    curr_entry->next = entry;
    if (NULL != entry->next) {
        entry->next->prev = entry;
    }

    return( 0 );
}

static int _shr_mem_avl_remove(shr_mem_avl_t *mem_avl,
                               shr_mem_avl_entry_pt entry)
{
    int nStatus;

    /*
     * Calls remove root if necessary
     */
    nStatus = shr_avl_delete(mem_avl->tree, _shr_mem_avl_compare, 
                             (shr_avl_datum_t*)entry);

    return( nStatus );
}

static int _shr_mem_avl_dll_remove(shr_mem_avl_t *mem_avl,
                                   shr_mem_avl_entry_pt entry)
{
    shr_mem_avl_entry_pt prev_entry;
    shr_mem_avl_entry_pt next_entry;

    assert(mem_avl);
    assert(entry);

    prev_entry = entry->prev;
    next_entry = entry->next;

    if (NULL != prev_entry) {
        prev_entry->next = next_entry;
    }

    if (NULL != next_entry) {
        next_entry->prev = prev_entry;
    }

    return( 0 );
}

int shr_mem_avl_free(shr_mem_avl_t *mem_avl, unsigned int addr)
{
    shr_mem_avl_entry_pt entry;
    shr_mem_avl_entry_pt prev_entry;
    shr_mem_avl_entry_pt next_entry;
#ifndef NDEBUG
    int nStatus; 
#endif /* !NDEBUG*/
    assert(mem_avl);

    /*
     * Search for our DLL entry
     */
    entry = _shr_mem_avl_dll_search(mem_avl, addr, 0);
    if (NULL == entry) {
        /*
         * This is bad, we couldn't find our
         * allocated block to free
         */
        return( -1 );
    }

    if ( !(entry->used) ) {
        /*
         * Trying to free a free block?
         */
        return( -1 );
    }

    /*
     * Attempt to coalesce adjacent blocks.
     *
     * We have four scenarios:
     *   1.  Previous block is free
     *   2.  Next block is free
     *   3.  Both Previous & Next blocks are free.
     *   4.  Neither Previous & Next blocks are free.
     */
    prev_entry = entry->prev;
    next_entry = entry->next;

    if (NULL != prev_entry) {
        /* Previous Block is free */
        /*
         * Since we want to ensure that we don't remove the root node
         * of the DLL, we coalesce to to previous node
         */
        if ( !(prev_entry->used) ) {
            _shr_mem_avl_remove(mem_avl, prev_entry);
            prev_entry->size += entry->size;
            _shr_mem_avl_dll_remove(mem_avl, entry);
            sal_free(entry);
            /*
             * Setup our pointer to look at the coalesced block
             */
            entry = prev_entry;
        }
    }

    if (NULL != next_entry) {
        /* Next Block is free */
        if ( !(next_entry->used) ) {
            entry->size += next_entry->size;
            _shr_mem_avl_remove(mem_avl, next_entry);
            _shr_mem_avl_dll_remove(mem_avl, next_entry);
            sal_free(next_entry);
        }
    }

    /*
     * Remove the used flag
     */
    entry->used = 0;

    /*
     * Add to free tree
     */
    /*
     * ENHANCEME: do something with the status from avl_insert
     */
#ifndef NDEBUG
    nStatus =
#endif /* !NDEBUG*/
    shr_avl_insert(mem_avl->tree, _shr_mem_avl_compare, 
                             (shr_avl_datum_t*)entry);
    assert(nStatus >= 0);

    return( 0);
}

static void shr_mem_avl_list_tree(shr_avl_entry_t* entry, int m)
{
    shr_mem_avl_entry_pt mem_entry;
    int n=m;
    if (entry == 0) {
        return;
    }
    mem_entry = (shr_mem_avl_entry_pt)(&entry->datum);
    if (entry->right) {
        shr_mem_avl_list_tree(entry->right, m+1);
    }
    while (n--) {
        LOG_CLI((BSL_META("   ")));
    }
    LOG_CLI((BSL_META("0x%x (size:0x%x) (0x%x)\n"),mem_entry->addr, 
             mem_entry->size, entry->balance));
    if (entry->left) {
        shr_mem_avl_list_tree(entry->left, m+1);
    }
}

int shr_mem_avl_list_output( shr_mem_avl_t *mem_avl )
{
    shr_mem_avl_entry_pt entry;
    int index;

    assert(mem_avl);

    entry = mem_avl->mem_list;
    LOG_CLI((BSL_META("Listing Memory Nodes:\n")));
    index = 0;
    while (NULL != entry) {
        LOG_CLI((BSL_META("%i:  Units: 0x%x  Addr: 0x%x Used: %i "),
                 index++,
                 entry->size,
                 entry->addr,
                 entry->used));
        LOG_CLI((BSL_META("This: %p Prev: %p Self: %p Next %p\n"),
                 (void *)entry,
                 (void *)entry->prev,
                 (void *)entry->self,
                 (void *)entry->next));
        entry = entry->next;
    }

    return( 0 );

}

int shr_mem_avl_free_tree_list( shr_mem_avl_t *mem_avl )
{
    assert(mem_avl);

    LOG_CLI((BSL_META("Listing Free Tree:\n")));
    shr_mem_avl_list_tree(mem_avl->tree->root, 0);

    return( 0 );

}

int shr_mem_avl_check_mem( shr_mem_avl_t *mem_avl, 
                           int *edges, 
                           int *patches, 
                           int *usedCount, 
                           int *freeCount)
{
    shr_mem_avl_entry_pt entry;

    assert(mem_avl);

    *edges = 0;
    *patches = 1;
    *usedCount = 0;
    *freeCount = 0;

    /*
     * Search for edges
     */
    entry = mem_avl->mem_list;
    while (NULL != entry) {
        if (NULL != entry->prev) {
            if ((entry->used) !=
                (entry->prev->used) ) {
                (*edges)++;
            }
        }
        entry = entry->next;
    }

    /*
     * Search for patches, and count used & free at the same time
     */
    entry = mem_avl->mem_list;
    while (NULL != entry) {
        if (NULL != entry->next) {
            if ((entry->used) !=
                (entry->next->used)) {
                (*patches)++;
            }
        }

        if (entry->used == 1) {
            (*usedCount)++;
        }
        else {
            (*freeCount)++;
        }

        entry = entry->next;
    }

    return( 0 );
}

int shr_mem_avl_free_count(shr_mem_avl_t *mem_avl, int size, int*count)
{
    assert(mem_avl);
    *count = 0;
    _shr_mem_avl_find_free_count(mem_avl->tree->root, size, count);
    return( 0 );
}

static void _shr_mem_avl_find_free_count(shr_avl_entry_t* entry, int size, int *count)
{
    shr_mem_avl_entry_pt mem_entry;
    if (entry == 0) {
        return;
    }

    mem_entry = (shr_mem_avl_entry_pt)(&entry->datum);
    if (entry->right) {
        _shr_mem_avl_find_free_count(entry->right, size, count);
    }
    *count = *count + (int)(mem_entry->size / size);
    if (entry->left) {
        _shr_mem_avl_find_free_count(entry->left, size, count);
    }
    return;
}
