/*! \file shr_itbm.h
 *
 * Indexed table block management library.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SHR_ITBM_H
#define SHR_ITBM_H

#include <sal/sal_libc.h>

/*!
 * Indicates invalid index or value, this constant is used for variable
 * initialization.
 */
#ifndef SHR_ITBM_INVALID
#define SHR_ITBM_INVALID ((uint32_t)(-1))
#endif

/*!
 * \brief Type definition for indexed table list handle.
 */
typedef struct shr_itbm_list_s *shr_itbm_list_hdl_t;

/*!
 * \brief Indexed table parameters from the component for creating and
 *        managing the list.
 */
typedef struct shr_itbm_list_params_s {
    /*! Unit number. */
    int unit;
    /*! Physical table ID. */
    uint32_t table_id;
    /*! Minimum entry number in the indexed table. */
    uint32_t min;
    /*! Maximum entry number in the indexed table. */
    uint32_t max;
    /*! Number of the first entry in the list to manage. */
    uint32_t first;
    /*! Number of the last entry in the list to manage. */
    uint32_t last;
    /*! Component ID. */
    uint32_t comp_id;
    /*!
     * Enable to maintain allocated and free elements statistics of the
     * list in blocks format \ref shr_itbm_blk_t and also to support
     * defragmentation operation on the list.
     */
    bool block_stat;
    /*!
     * Enable to create an indexed bucket type list for managing indexed tables
     * that support more than one entry per physical table entry. When enabled,
     * each element from the regular ITBM list is treated as a bucket and
     * supports \ref entries_per_bucket count of elements in it. This parameter
     * can be enabled only for \ref block_stat enabled lists.
     */
    bool buckets;
    /*!
     * Indicates the number of entries supported per bucket for indexed bucket
     * type lists. Both \ref buckets and \ref block_stat parameters must be
     * enabled to configure and use this parameter for a list.
     */
    uint8_t entries_per_bucket;
    /*!
     * Enable to reverse the default list blocks management direction of ITBM.
     * By default this parameter is disabled and block elements are allocated
     * from the first element to the last list element direction and the list
     * of free blocks are moved to the bottom of the list on defragmentation.
     * But when \ref reverse parameter is enabled for a list, then block
     * elements are allocated from the last to first element direction and the
     * free blocks are moved to the top of the list on defragmentation.
     *
     * This parameter should be enabled for the secondary shared indexed
     * buckets type lists, when a physical table is mapped to two ITBM lists
     * (primary and secondary) and each mapped list must support a different
     * \ref entries_per_bucket count value in the mapped lists. To use this
     * parameter, both \ref block_stat and \ref buckets parameters must be
     * enabled for the list.
     */
    bool reverse;
} shr_itbm_list_params_t;

/*!
 * \brief Indexed table block location in the list and the block size.
 */
typedef struct shr_itbm_blk_s {
    /*!
     * Indicates the number of entries supported per physical table entry,
     * referred to as a bucket.
     */
    uint8_t entries_per_bucket;
    /*!
     * The base bucket number which contains the first element of this block.
     * This parameter is valid and applicable only to indexed bucket type
     * lists.
     */
    uint32_t bucket;

    /*! First element number of this block. */
    uint32_t first_elem;

    /*! Total elements in this block. */
    uint32_t ecount;

    /*! Component ID. */
    uint32_t comp_id;
} shr_itbm_blk_t;

/*!
 * \brief Indexed table defragmentation block move sequence.
 */
typedef struct shr_itbm_defrag_blk_mseq_s {
    /*! Total number of moves required for this block. */
    uint32_t mcount;

    /*!
     * The destination base bucket number to which the \c first_elem of the
     * block has to be moved, this parameter is valid and applicable only to
     * indexed bucket type lists.
     *
     * For blocks that undergo multiple moves, the previous destination
     * bucket location in the list is the new source location for retrieving
     * the block and moving it to the next destination location provided in
     * this list. The last array element value in this list is the final
     * destination location for the block.
     */
    uint32_t *bucket;

    /*!
     * The list of move locations to be used for moving this block for
     * defragmenting the list. For blocks that undergo multiple moves, the
     * previous destination location in this list is the new source
     * location for retrieving the block and moving it to the next
     * destination location provided in this list. The last array element
     * value in this list is the final destination location for the block.
     */
    uint32_t *first_elem;

    /*! Total elements in this block, which is being moved. */
    uint32_t ecount;
} shr_itbm_defrag_blk_mseq_t;

/*!
 * \brief Indexed table list elements usage statistics.
 *
 * This structure contains the current usage statistics of a list. The list
 * elements in the \c first and \c last entry number range are checked
 * to determine the current \c in_use_count and \c free_count statistics.
 */
typedef struct shr_itbm_list_stat_s {
    /*! Number of elements in use. */
    uint32_t in_use_count;

    /*! Number of free elements. */
    uint32_t free_count;
} shr_itbm_list_stat_t;

/*!
 * \brief Initializes a list parameters type structure.
 *
 * This function initializes a list parameters type structure \ref
 * shr_itbm_list_params_t members to default values.
 *
 * \param [in] params Pointer to \ref shr_itbm_list_stat_t structure.
 *
 * \return None.
 */
extern void
shr_itbm_list_params_t_init(shr_itbm_list_params_t *params);

/*!
 * \brief Initializes a list statistics type structure.
 *
 * This function initializes a list statistics type structure \ref
 * shr_itbm_list_stat_t members to default values.
 *
 * \param [in] stat Pointer to \ref shr_itbm_list_stat_t structure.
 *
 * \return None.
 */
extern void
shr_itbm_list_stat_t_init(shr_itbm_list_stat_t *stat);

/*!
 * \brief Initializes a list block type structure.
 *
 * This function initializes a list block type structure \ref shr_itbm_blk_t
 * members to default values.
 *
 * \param [in] blk Pointer to \ref shr_itbm_blk_t structure.
 *
 * \return None.
 */
extern void
shr_itbm_blk_t_init(shr_itbm_blk_t *blk);

/*!
 * \brief Create an indexed table elements free list.
 *
 * This function creates a list of free elements for an indexed table.
 *
 * \param [in] params The indexed table parameter details from the
 *                    component, for creating and managing the list.
 * \param [in] name Name of the list.
 * \param [out] hdl Handle to the list.
 *
 * \retval SHR_E_NONE List created successfully.
 * \retval SHR_E_RESOURCE The \c min or \c max entry range specified in
 *                        \c params is already managed by another list.
 * \retval SHR_E_EXITS A list already exists with the same table \c params.
 * \retval SHR_E_MEMORY On memory allocation failure.
 * \retval SHR_E_PARAM Invalid \c first or \c last parameter value. The
 *                     \c first parameter value in \c params input parameter
 *                     must be greater than or equal to \c min parameter
 *                     value. The \c last parameter value must be less than
 *                     or equal to \c max parameter value. For indexed
 *                     buckets  type lists, \c buckets parameter must be
 *                     enabled to configure non-default \c entries_per_bucket
 *                     value i.e. 1, 2 or 4.
 */
extern int
shr_itbm_list_create(shr_itbm_list_params_t params,
                     char *name,
                     shr_itbm_list_hdl_t *hdl);

/*!
 * \brief Destroy an indexed table list.
 *
 * This function destroys an indexed table list.
 *
 * \param [in] hdl Handle to the list to be destroyed.
 *
 * \retval SHR_E_NONE List destroyed successfully.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_BUSY List elements still in-use, free the elements before
 *                    destroying the list (or) the list defragmentation is
 *                    in-progress.
 * \retval SHR_E_NOT_FOUND List not found to destroy.
 */
extern int
shr_itbm_list_destroy(shr_itbm_list_hdl_t hdl);

/*!
 * \brief Allocate the next available block of contiguous elements from a
 *        list.
 *
 * This function allocates the next available block of contiguous elements
 * from a list with a block size of \c count elements.
 *
 * \param [in] hdl Handle to the list for allocation of the block.
 * \param [in] count Number of elements in the block.
 * \param [out] bucket Applicable only to indexed bucket type lists,
 *                     pointer to the base bucket that contains the first
 *                     element of the allocated block. The allocated block
 *                     can span additional adjoining buckets besides the
 *                     base bucket, if the number of elements specified
 *                     in the \c count input parameter exceeds the free
 *                     elements count available in the base bucket.
 * \param [out] first Pointer to the first element of the block. For indexed
 *                    buckets type lists, pointer to the first element of
 *                    the block in the block's base bucket.
 *
 * \retval SHR_E_NONE Block allocation successful.
 * \retval SHR_E_NOT_FOUND The given list is not found.
 * \retval SHR_E_RESOURCE Not enough free elements to complete the block
 *                        allocation.
 * \retval SHR_E_FULL No free elements available in this list.
 * \retval SHR_E_PARAM Invalid \c count value.
 * \retval SHR_E_BUSY List defragmentation in-progress.
 */
extern int
shr_itbm_list_block_alloc(shr_itbm_list_hdl_t hdl,
                          uint32_t count,
                          uint32_t *bucket,
                          uint32_t *first);

/*!
 * \brief Allocate a block of contiguous elements within a specified range
 *        of a list.
 *
 * This function allocates a block of contiguous elements within a specified
 * range of a list. The caller provides the block's first element number
 * \c first, the size of the block to be allocated \c count and the block's
 * base \c bucket (applicable only to indexed bucket type lists) as input
 * to this function.
 *
 * \param [in] hdl Handle to the list for allocating the block.
 * \param [in] count Number of elements in the block.
 * \param [in] bucket Applicable only to indexed bucket type lists,
 *                    use specified input bucket as the base bucket while
 *                    allocating the block elements. The allocated block
 *                    can span additional buckets adjoining the base
 *                    bucket, when the number of elements specified in the
 *                    \c count input parameter exceeds the free elements
 *                    count in the input base \c bucket.
 * \param [in] first  List offset index at which the first element of the
 *                    block must be allocated. For indexed bucket type
 *                    lists, this parameter specifies the block's first
 *                    element's offset index value in the block's base
 *                    bucket.
 *
 * \retval SHR_E_NONE Specified block allocation successful.
 * \retval SHR_E_NOT_FOUND The given list is not found.
 * \retval SHR_E_PARAM Invalid \c first or \c count value.
 * \retval SHR_E_EXISTS The element or elements in the specified range are
 *                      already in-use.
 * \retval SHR_E_BUSY List defragmentation in-progress.
 */
extern int
shr_itbm_list_block_alloc_id(shr_itbm_list_hdl_t hdl,
                             uint32_t count,
                             uint32_t bucket,
                             uint32_t first);

/*!
 * \brief Free a block of contiguous elements back to a list.
 *
 * This function frees a block of elements back to a list.
 *
 * \param [in] hdl Handle to the list for freeing the block.
 * \param [in] count Number of elements in the block.
 * \param [in] bucket Applicable only to indexed bucket type lists,
 *                    block's base bucket that contains the first element
 *                    of the block that is to be freed.
 * \param [in] first  List offset index value of the first element of the
 *                    block. For indexed bucket type lists, this parameter
 *                    indicates the block's first element's offset index in
 *                    the block's base bucket.
 *
 * \retval SHR_E_NONE Block free successful.
 * \retval SHR_E_NOT_FOUND The given list is not found.
 * \retval SHR_E_PARAM Invalid \c first or \c count value.
 * \retval SHR_E_BUSY List defragmentation in-progress.
 */
extern int
shr_itbm_list_block_free(shr_itbm_list_hdl_t hdl,
                         uint32_t count,
                         uint32_t bucket,
                         uint32_t first);

/*!
 * \brief Resize an existing block in the list.
 *
 * This function performs an in-place resize operation on an existing block.
 * An existing block's size can be increased or decreased by specifying a new
 * block size value in \c new_count input parameter.
 *
 * When a block's size is reduced, elements are freed back to the list. When
 * a block size is increased, free elements adjoining the current block are
 * claimed from the free pool and allocated to this existing block, allowing
 * the block to grow to a new block size of \c new_count elements requested
 * by the caller.
 *
 * \param [in] hdl Handle to the list.
 * \param [in] count Current number of elements in the block.
 * \param [in] bucket Applicable only to indexed bucket type lists,
 *                    block's base bucket that contains the first element
 *                    of the block.
 * \param [in] first  List offset index value of the first element of the
 *                    block. For indexed bucket type lists, this parameter
 *                    indicates the block's first element's offset index in
 *                    the block's base bucket.
 * \param [in] new_count New number of elements for this block.
 *
 * \retval SHR_E_NONE Block resize successful.
 * \retval SHR_E_NOT_FOUND The given list is not found.
 * \retval SHR_E_PARAM Invalid \c count or \c first or \c new_count value.
 * \retval SHR_E_RESOURCE Not enough adjoining free elements to complete the
 *                        block resize operation.
 * \retval SHR_E_FULL No free elements available in this list.
 * \retval SHR_E_BUSY List defragmentation in-progress.
 */
extern int
shr_itbm_list_block_resize(shr_itbm_list_hdl_t hdl,
                           uint32_t count,
                           uint32_t bucket,
                           uint32_t first,
                           uint32_t new_count);

/*!
 * \brief Start defragmentation of an indexed list.
 *
 * This function performs defragmentation operation on a \c block_stat
 * enabled indexed list. After running the defragmentation function on the
 * list blocks, this function returns the total number of component blocks
 * that must be moved to complete the list defragmentation operation in the
 * \c count output parameter. For each block that is being moved, this
 * function also returns the block's current location and the total number
 * of elements in each block that was allocated by this component in
 * \c blk_cur_loc array output parameter. The elements in this array are
 * sorted by the \c first_elem number value of the blocks in ascending
 * order or descending order if \c reserve parameter is enabled for the list.
 * For each block in \c blk_cur_loc array, this function also returns
 * the total number of moves required for each block and the exact move
 * sequence to be used for the block in \c blk_mov_seq output parameter.
 *
 * The total number of array elements in \c blk_mov_seq and \c blk_cur_loc
 * arrays match the \c count output parameter value returned by this
 * function. The caller must execute the move sequence for each block
 * exactly as provided in the \c blk_mov_seq array, starting from the first
 * element in this array and continuing until the end of this array for
 * each block. While moving the blocks in the device indexed table that is
 * associated with this list, caller must follow make-before-break sequence
 * for each block move operation.
 *
 * It should be noted that the final destination location for all blocks in
 * \c blk_mov_seq array is considered not active until it is confirmed by
 * the caller via a \c shr_itbm_list_defrag_end function call. If the
 * list defragmentation is successful, caller indicates it by setting
 * \c success to TRUE in the \c shr_itbm_list_defrag_end function call and
 * the defragmented list is used as the new active list for management
 * by ITBM. But if an unexpected error is observed by the caller during
 * the defragmentation blocks move sequence in the device table, then
 * defragmentation operation must be aborted by calling the
 * \c shr_itbm_list_defrag_end function with \c success set to
 * FALSE. In the defragmentation operation abort case, the current active
 * list state is retained and used for further managing the list.
 *
 * During defragmentation, the list is locked by the caller after the
 * \c shr_itbm_list_defrag_start function call and the list remains locked
 * for normal block operations until it is unlocked again by the caller
 * via the \c shr_itbm_list_defrag_end function call. So, a
 * \c shr_itbm_list_defrag_start function call must always be followed by a
 * call to \c shr_itbm_list_defrag_end function, with appropriate
 * defragmentation status set by caller in \c success input parameter to
 * the end function.
 *
 * To get the list of free blocks and the largest free block available in
 * the defragmented list, the callee function must pass valid \c free_count,
 * \c free_blk and \c largest_free_blk pointers to this function. The API will
 * check if these pointers are valid and return the details whenever
 * defragmentation is completed successfully. If the callee is not interested
 * in the free block details, then pass null pointer value to these parameters
 * in the function call, as these pointers are not valid, this API will not set
 * these details up on function return.
 *
 * \param [in] hdl Handle to the list.
 * \param [out] count Total number of blocks to be moved, to complete the
 *                    list defragmentation operation.
 * \param [out] blk_cur_loc The current location and size of the blocks to be
 *                          moved for defragmentation. The array elements are
 *                          sorted by their \c first_elem value in ascending
 *                          order.
 * \param [out] blk_mov_seq The move sequence to be followed by the caller
 *                          for each block element present in
 *                          \c blk_cur_loc array.
 * \param [out] free_count Pointer to set and return the \c free_blk array size.
 * \param [out] free_blk Pointer to return the free blocks array which contains
 *                       the list of free blocks in the list after
 *                       defragmentation is completed successfully. The array
 *                       elements are sorted in ascending order by their bucket
 *                       number for indexed bucket type list and by the first
 *                       element number for regular indexed list.
 * \param [out] largest_free_blk Pointer to set and return the largest free
 *                               block present in the \c free_blk array, this
 *                               block detail is valid only when the \c
 *                               free_count parameter value returned by this
 *                               API is non-zero.
 *
 * \retval SHR_E_NONE Operation successful.
 * \retval SHR_E_NOT_FOUND The given list is not found.
 * \retval SHR_E_MEMORY On memory allocation failure.
 * \retval SHR_E_RESOURCE Free blocks not available to perform
 *                        defragmentation operation on the list.
 * \retval SHR_E_PARAM When defragmentation is performed on a \c block_stat
 *                     disabled list.
 * \retval SHR_E_BUSY List defragmentation already started and in-progress.
 */
extern int
shr_itbm_list_defrag_start(shr_itbm_list_hdl_t hdl,
                           uint32_t *count,
                           shr_itbm_blk_t **blk_cur_loc,
                           shr_itbm_defrag_blk_mseq_t **blk_mov_seq,
                           uint32_t *free_count,
                           shr_itbm_blk_t **free_blk,
                           shr_itbm_blk_t *largest_free_blk);

/*!
 * \brief End defragmentation of an indexed list.
 *
 * This function is called to end defragmentation operation on an indexed
 * list. The final defragmentation status is indicated by the caller in
 * \c success input parameter to this function. If the defragmentation
 * block moves were completed successfully by the caller, then \c success
 * is set to TRUE in the defragmentation end function call by the caller,
 * the defragmented list state becomes the new active list state to be
 * managed by ITBM.
 *
 * But if an unexpected error is observed by the caller, while performing
 * the defragmentation move operations, then \c success is set to FALSE by
 * the caller in defragmentation end function call. The current active list
 * state is retained and used for managing the list. All resources allocated
 * by ITBM for performing the list defragmentation operation are freed and
 * the list is also unlocked for normal block operations after the end call
 * is completed successfully.
 *
 * \param [in] hdl Handle to the list.
 * \param [in] success Final list defragmentation status from the caller.
 *
 * \retval SHR_E_NONE Operation successful.
 * \retval SHR_E_NOT_FOUND The given list is not found.
 * \retval SHR_E_PARAM When this function has been called without starting
 *                     the defragmentation operation on the list (or) when
 *                     the list handle passed in the end call is not the
 *                     list handle with which defragmentation was started.
 */
extern int
shr_itbm_list_defrag_end(shr_itbm_list_hdl_t hdl,
                         bool success);

/*!
 * \brief Returns the largest free block info per shared indexed bucket type
 *        list that might become available in the lists after defragmenting
 *        them.
 *
 * This function can be called to get the size of largest free block that
 * might be available in a shared indexed bucket type list pair after
 * defragmenting the lists. To get this information, this API must be called
 * with both the forward and reverse direction list handles of the two
 * shared indexed bucket type lists previously created using the
 * \ref shr_itbm_list_create API.
 *
 * As the shared indexed bucket type lists could have been created with
 * different \c entries_per_bucket count list parameter values, this API
 * will return the free block size info for the two lists in separate output
 * parameters \c largest and \c largest_rev, which are of \ref shr_itbm_blk_t
 * structure type. The free elements count in these blocks will be based on
 * their list \c entries_per_bucket count config value.
 *
 * \param [in] hdl List handle of the shared indexed bucket list that was
 *                 created with \c reverse parameter disabled -
 *                 \ref shr_itbm_list_params_t.
 * \param [in] hdl_rev List handle of the shared indexed bucket list that
 *                     was created with \c reverse parameter enabled -
 *                     \ref shr_itbm_list_params_t.
 * \param [out] largest Largest free block available in the list pointed by
 *                      \c hdl list handle after defragmentation.
 * \param [out] largest_rev Largest free block available in the list pointed
 *                          by \c hdl_rev list handle after defragmentation.
 *
 * \retval SHR_E_NONE Operation successful.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_MEMORY On memory allocation failure.
 * \retval SHR_E_NOT_FOUND No free blocks in the lists.
 */
extern int
shr_itbm_list_defrag_free_block_get(shr_itbm_list_hdl_t hdl,
                                    shr_itbm_list_hdl_t hdl_rev,
                                    shr_itbm_blk_t *largest,
                                    shr_itbm_blk_t *largest_rev);
/*!
 * \brief Get the current state of an element.
 *
 * This function obtains the current state of an element in a list.
 * The output parameter \c in_use is TRUE if the element is currently in-use
 * or else it is set to FALSE. This function also returns the \c comp_id of
 * the component that is using this element, if \c in_use is TRUE.
 *
 * \param [in] hdl Handle to the list.
 * \param [in] bucket Applicable only to indexed bucket type lists,
 *                    indicates the bucket which contains the input
 *                    \c element, for which state must be retrieved.
 * \param [in] element  List offset index value of the element to check. For
 *                      indexed bucket type lists, this parameter indicates
 *                      the element's offset index in the input bucket.
 * \param [out] in_use Pointer to the state of an element.
 * \param [out] comp_id Component ID of the component using this element.
 *
 * \retval SHR_E_NOT_FOUND If the element is not found in the list.
 * \retval SHR_E_PARAM Invalid \c element number or list \c hdl.
 * \retval SHR_E_BUSY List defragmentation in-progress.
 */
extern int
shr_itbm_list_elem_state_get(shr_itbm_list_hdl_t hdl,
                             uint32_t bucket,
                             uint32_t element,
                             bool *in_use,
                             uint32_t *comp_id);

/*!
 * \brief Obtain usage statistics of a list.
 *
 * This function obtains the usage statistics of a list.
 *
 * \param [in] hdl Handle to the list.
 * \param [out] stat The usage statistics associated with this list.
 *
 * \retval SHR_E_NONE On success.
 * \retval SHR_E_NOT_FOUND The given list is not found.
 * \retval SHR_E_BUSY List defragmentation in-progress.
 */
extern int
shr_itbm_list_stat_get(shr_itbm_list_hdl_t hdl,
                       shr_itbm_list_stat_t *stat);

/*!
 * \brief Obtain usage statistics of a list in blocks.
 *
 * This function obtains the usage statistics of a list in blocks, provided
 * the \c block_stat parameter in \c shr_itbm_list_params_t was enabled for
 * this list during list create.
 *
 * \param [in] hdl Handle to the list.
 * \param [out] alloc_count Pointer to the count of the total number of
 *                          blocks allocated by the components from this
 *                          list.
 * \param [out] alloc_blk An array of block elements, allocated and in-use
 *                        by the components from this list. The size of
 *                        this array is returned in \c alloc_count output
 *                        parameter and the elements in this array are
 *                        sorted in ascending order by their first
 *                        element \c first_elem number value.
 * \param [out] free_count Pointer to the count of the total number of free
 *                         blocks available in this list.
 * \param [out] free_blk An array of free blocks available in this list.
 *                       The size of this array is returned in \c
 *                       free_count output parameter and the elements in
 *                       this array are sorted in ascending order by their
 *                       first element number value.
 *
 * \retval SHR_E_NONE On success.
 * \retval SHR_E_PARAM If \c block_stat is not enabled for this list.
 * \retval SHR_E_NOT_FOUND The given list is not found.
 * \retval SHR_E_BUSY List defragmentation in-progress.
 */
extern int
shr_itbm_list_block_stat_get(shr_itbm_list_hdl_t hdl,
                             uint32_t *alloc_count,
                             shr_itbm_blk_t **alloc_blk,
                             uint32_t *free_count,
                             shr_itbm_blk_t **free_blk);
/*!
 * \brief Resize an existing list to support a new number of list elements.
 *
 * This API can be used to resize an existing list by modifying its \c first
 * and \c last parameters range. The application must free the in use elements
 * allocated from the range that is being modified prior to a list resize
 * operation. For indexed bucket type lists, the total elements supported by
 * this list and the free elements available for allocation will be recomputed
 * based on the new managed elements range and the \c entries_per_bucket count
 * previously provided in the list create call. The new managed elements
 * range for the list is specified in the \c new_first and \c new_last
 * parameters of this API.
 *
 * \param [in] hdl List handle returned by the list create API.
 * \param [in] new_first New first element range of this list.
 * \param [in] new_last New last element range of this list.
 *
 * \retval SHR_E_NONE List resized successfully.
 * \retval SHR_E_NOT_FOUND Given list is not found to complete resize
 *                         operation.
 * \retval SHR_E_BUSY List elements allocated and in use in the range that
 *                    is being modified, free these list elements and retry
 *                    the list resize operation.
 * \retval SHR_E_PARAM Invalid list handle \c hdl or invalid \c new_first
 *                     or \c new_last parameter value, supplied new elements
 *                     range cannot exceed the list \c min and \c max range
 *                     and new range cannot overlap with an another list
 *                     that has same list config parameters \c reverse and
 *                     \c entries_per_bucket.
 */
extern int
shr_itbm_list_resize(shr_itbm_list_hdl_t hdl,
                     uint32_t new_first,
                     uint32_t new_last);
#endif /* SHR_ITBM_H */
