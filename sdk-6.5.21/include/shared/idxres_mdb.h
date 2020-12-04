/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Module: Aligned Indexed Resource Multilist management, using banked lists
 */

#ifndef _SHARED_IDXRES_MDB_H_INCLUDED
#define _SHARED_IDXRES_MDB_H_INCLUDED

typedef uint32 shr_mdb_elem_index_t;
typedef uint16 shr_mdb_elem_bank_index_t;
typedef uint16 shr_mdb_bank_index_t;

/*
 *  This defines the type that is used as a handle for an mdb object.
 */
struct shr_mdb_list_s;
typedef struct shr_mdb_list_s *shr_mdb_list_handle_t;

/*
 *  When querying information about a block within an mdb object, this
 *  structure is used for results.
 *
 *  head is the actual head element of the block.
 *
 *  size is the number of elements in the block.
 *
 *  list indicates the list to which the block belongs, or any number 0xFFF0 or
 *  higher indicates it is not in a list.
 */
typedef struct shr_mdb_block_info_s {
    shr_mdb_elem_index_t head;
    shr_mdb_elem_bank_index_t size;
    shr_mdb_elem_bank_index_t list;
} shr_mdb_block_info_t;

/*
 *  When querying information about a list within an mdb object, this structure
 *  is used for results.
 *
 *  block_size only applies to free lists, and specifies the size in elements
 *  of blocks on that list.  User lists do not have a 'block size' (so the
 *  field is zero) since they can have any size blocks.
 *
 *  blocks indicates how many blocks are on the list.
 *
 *  elements indicates how many elements are on the list.
 */
typedef struct shr_mdb_list_info_s {
    shr_mdb_elem_bank_index_t block_size;
    shr_mdb_elem_index_t      blocks;
    shr_mdb_elem_index_t      elements;
} shr_mdb_list_info_t;

/*
 *  When querying information about the a whole mdb object, this structure is
 *  used for results.
 *
 *  free_elems and free blocks indicate the overall quantity and makeup of the
 *  free space in the list.  The free lists may themselves be queried for
 *  further information.
 *
 *  first and last are the first valid and last valid element numbers.
 *
 *  bank_size indicates how many elements are in a complete bank.  It is
 *  possible that the final bank might be incomplete.
 *
 *  free_lists indicates how many free lists there are.
 *
 *  user_lists indicates how many user lists there are.
 *
 *  lock indicates whether the mdb object protects itself.
 */
typedef struct shr_mdb_info_s {
    shr_mdb_elem_index_t      free_elems;
    shr_mdb_elem_index_t      free_blocks;
    shr_mdb_elem_index_t      first;
    shr_mdb_elem_index_t      last;
    shr_mdb_elem_bank_index_t bank_size;
    shr_mdb_elem_bank_index_t free_lists;
    shr_mdb_elem_bank_index_t user_lists;
    int                       lock;
} shr_mdb_info_t;

/*
 *  This defines the bitmap that is used to control allocation and freeing
 *  behaviour of an mdb object.  Note that there are several bitfields here,
 *  not just a few bits or a set of values, and that the setting is specific to
 *  a single mdb object (many can exist with different settings).
 *
 *  The alloc_bank set chooses the initial mechanism used to find the free
 *  space as the first part of the alloc process.  The 'first' setting does not
 *  mean 'low numbers', it basically means pick the space of the appropriate
 *  size which is fastest to locate, and is the fastest mechanism, plus it has
 *  the best cache use in case of high-speed thrashing that can fall within a
 *  single bank.  The 'high' setting means search for space from the highest
 *  bank toward the lowest; the 'low' setting means search for space from the
 *  lowest bank toward the highest.  Both of these settings are much slower but
 *  can be helpful if trying to collect allocated blocks toward one end of the
 *  space.  Only pick one from this set of options.
 *
 *  The alloc_block setting chooses whether to bias an alloc block toward the
 *  high end or low end of a free block that is too large (which it will be
 *  unless the alloc block is exactly one of the free lists sizes, and it may
 *  still be in that case if there were no exact size free blocks).  This
 *  setting takes effect AFTER the alloc join setting.  Only pick one from this
 *  set of options.
 *
 *  The free_block setting chooses whether to bias a freed block that is not an
 *  exact number of elements for any free list so that the largest block is on
 *  the high or low end of the block.  This setting takes effect AFTER the free
 *  join setting.  Only pick one from this set of options.
 *
 *  The join_alloc and join_free options indicate whether adjacent free blocks
 *  are combined on alloc and free.  If neither is chosen, free is assumed.
 *  Selecting free tends to coalesce larger free blocks, making larger alloc
 *  requests possible (can't alloc a block that is bigger than the largest free
 *  block); selecting alloc tends to reduce free space fragmentation (by
 *  coalescing the allocated blocks in a given bank); selecting both gains both
 *  features, at a slight performance cost compared to only one or the other.
 *
 *  The join directions indicate which direction will be searched for adjacent
 *  free blocks when joining.  It should be reasonably efficient to allow both
 *  directions.  If neither is chosen, high is assumed.
 */
typedef enum shr_mdb_alloc_pref_e {
    /* alloc mechanism */
    shr_mdb_alloc_bank_first =     0x0000,
    shr_mdb_alloc_bank_high =      0x0001,
    shr_mdb_alloc_bank_low =       0x0002,
    shr_mdb_alloc_bank_reserved =  0x0003,
    shr_mdb_alloc_bank_mask =      0x0003,
    /* where to place new alloc block in the found space */
    shr_mdb_alloc_block_high =     0x0004,
    shr_mdb_alloc_block_low =      0x0000,
    /* where to place largest subblock when freeing */
    shr_mdb_free_block_high =      0x0008,
    shr_mdb_free_block_low =       0x0000,
    /* whether to join adjacent free spaces on alloc or free */
    shr_mdb_join_alloc =           0x0010,
    shr_mdb_join_free =            0x0020,
    shr_mdb_join_alloc_and_free =  0x0030,
    /* join directions */
    shr_mdb_join_high =            0x0040,
    shr_mdb_join_low =             0x0080,
    shr_mdb_join_high_and_low =    0x00C0
} shr_mdb_alloc_pref_t;

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
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *
 *  Notes
 *    none
 */
extern int
shr_mdb_create(shr_mdb_list_handle_t *handle,
               shr_mdb_elem_bank_index_t bankSize,
               shr_mdb_elem_bank_index_t freeLists,
               shr_mdb_elem_bank_index_t *freeCnts,
               shr_mdb_elem_bank_index_t userLists,
               shr_mdb_elem_index_t first,
               shr_mdb_elem_index_t last,
               int lock);

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
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *
 *  Notes
 *    none
 */
extern int
shr_mdb_destroy(shr_mdb_list_handle_t handle);

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
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *
 *  Notes
 *    Elements to be reserved must be free, else the call will fail.
 */
extern int
shr_mdb_reserve(shr_mdb_list_handle_t handle,
                shr_mdb_elem_index_t first,
                shr_mdb_elem_index_t last);

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
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *
 *  Notes
 *    Elements to be unreserved must be singly allocated, else it will fail.
 */
extern int
shr_mdb_unreserve(shr_mdb_list_handle_t handle,
                  shr_mdb_elem_index_t first,
                  shr_mdb_elem_index_t last);

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
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *
 *  Notes
 *    Elements must be singly allocated, else it will fail.
 *    The resulting block's head will be at first if this is successful.
 */
int
shr_mdb_reserve_to_block(shr_mdb_list_handle_t handle,
                         shr_mdb_elem_index_t first,
                         shr_mdb_elem_index_t last);

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
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *
 *  Notes
 *    none
 */
extern int
shr_mdb_alloc(shr_mdb_list_handle_t handle,
              shr_mdb_elem_index_t *block,
              shr_mdb_elem_bank_index_t count);

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
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *
 *  Notes
 *    none
 */
extern int
shr_mdb_alloc_id(shr_mdb_list_handle_t handle,
                 shr_mdb_elem_index_t block,
                 shr_mdb_elem_bank_index_t count);

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
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *
 *  Notes
 *    none
 */
extern int
shr_mdb_free(shr_mdb_list_handle_t handle,
             shr_mdb_elem_index_t block);

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
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *
 *  Notes
 *    none
 */
extern int
shr_mdb_block_size_get(shr_mdb_list_handle_t handle,
                       shr_mdb_elem_index_t block,
                       shr_mdb_elem_bank_index_t *count);

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
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *
 *  Notes
 *    none
 */
extern int
shr_mdb_list_insert(shr_mdb_list_handle_t handle,
                    shr_mdb_elem_bank_index_t list,
                    shr_mdb_elem_index_t block);

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
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *
 *  Notes
 *    none
 */
extern int
shr_mdb_list_remove(shr_mdb_list_handle_t handle,
                    shr_mdb_elem_index_t block);

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
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *
 *  Notes
 *    none
 */
extern int
shr_mdb_list_get(shr_mdb_list_handle_t handle,
                 shr_mdb_elem_index_t block,
                 shr_mdb_elem_bank_index_t *list);

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
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *
 *  Notes
 *    none
 */
extern int
shr_mdb_list_head(shr_mdb_list_handle_t handle,
                  shr_mdb_elem_bank_index_t list,
                  shr_mdb_elem_index_t *head);


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
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *
 *  Notes
 *    none
 */
extern int
shr_mdb_list_tail(shr_mdb_list_handle_t handle,
                  shr_mdb_elem_bank_index_t list,
                  shr_mdb_elem_index_t *tail);

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
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *
 *  Notes
 *    none
 */
extern int
shr_mdb_list_pred(shr_mdb_list_handle_t handle,
                  shr_mdb_elem_index_t block,
                  shr_mdb_elem_index_t *pred);

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
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *
 *  Notes
 *    none
 */
extern int
shr_mdb_list_succ(shr_mdb_list_handle_t handle,
                  shr_mdb_elem_index_t block,
                  shr_mdb_elem_index_t *succ);

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
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *
 *  Notes
 *    none
 */
extern int
shr_mdb_list_purge(shr_mdb_list_handle_t handle,
                   shr_mdb_elem_bank_index_t list);

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
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *
 *  Notes
 *    This only supports allocated blocks, not free blocks.
 */
extern int
shr_mdb_block_info(shr_mdb_list_handle_t handle,
                   shr_mdb_elem_index_t block,
                   shr_mdb_block_info_t *blockInfo);

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
 *      BCM_E_EMPTY if none of the elements are in use
 *      BCM_E_FULL if all of the elements are in use
 *      BCM_E_CONFIG if elements are in use but block(s) do not match
 *      BCM_E_EXISTS if some of the elements are in use but not all of them
 *      BCM_E_PARAM if any of the elements is not valid
 *      BCM_E_* otherwise as appropriate
 *
 *  Notes
 */
extern int
shr_mdb_block_check_all(shr_mdb_list_handle_t handle,
                        shr_mdb_elem_index_t first,
                        shr_mdb_elem_bank_index_t count);

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
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *
 *  Notes
 *    none
 */
extern int
shr_mdb_list_info(shr_mdb_list_handle_t handle,
                  shr_mdb_elem_bank_index_t list,
                  int free,
                  shr_mdb_list_info_t *listInfo);

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
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *
 *  Notes
 *    none
 */
extern int
shr_mdb_info(shr_mdb_list_handle_t handle,
             shr_mdb_info_t *mdbInfo);

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
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *
 *  Notes
 *    none
 */
extern int
shr_mdb_allocmode_get(shr_mdb_list_handle_t handle,
                      shr_mdb_alloc_pref_t *allocmode);

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
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *
 *  Notes
 *    none
 */
extern int
shr_mdb_allocmode_set(shr_mdb_list_handle_t handle,
                      shr_mdb_alloc_pref_t allocmode);

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
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *
 *  Notes
 *    none
 */
extern int
shr_mdb_all_free_to_user_list(shr_mdb_list_handle_t handle,
                              shr_mdb_elem_bank_index_t userList);

#endif /* ndef _SHARED_IDXRES_MDB_H_INCLUDED */

