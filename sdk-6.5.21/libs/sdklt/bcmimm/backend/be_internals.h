/*! \file be_internals.h
 *
 * In-Memory back-end internal API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BE_INTERNALS_H
#define BE_INTERNALS_H

/*!
 * \brief Element pointer size.
 *
 * sub component code (bits 22-31) - That is the HA block identifier.
 * occupied bit (bit 21) - (only for entry) indicates if the entry is in use.
 * locked bit (bit 20) - (only for entry) indicates if the entry is locked.
 * offset (bits 0-19) - That is the offset of the element in the HA block.
 * The offset size allows for up to 1M entries per HA block.
 */
#define BCMIMM_ELEM_PTR_SIZE      4
/*! Sub offset in HA pointer */
#define BCMIMM_SUB_OFFST        SHR_HA_PTR_SUB_SHIFT
/*! Lock bit number in an entry pointer */
#define BCMIMM_LOCK_BIT         SHR_HA_PTR_PRIV_SHIFT
/*! Occupy bit in an entry pointer */
#define BCMIMM_OCCUPY_BIT       (BCMIMM_LOCK_BIT+1)
/*! mask for the offset bits */
#define BCMIMM_OFFSET_MASK      SHR_HA_PTR_OFFSET_MASK
#define BCMIMM_PTR_CTRL_MASK    SHR_HA_PTR_PRIV_BITS

/*!
 * \brief Reference count size
 *
 * This indicates the size of the reference count (in bytes). The
 * reference count is an unsigned number that can reach 64K
 */
#define BCMIMM_ELEM_REF_CNT_SIZE    2

/*!
 * \brief HA block signature.
 *
 * Every HA block, once assigned and set, will have a signature field. The
 * in-mem back-end validate the integrity of an HA block using the signature.
 * An unsigned HA block is considered new block while signed block considered
 * blocks that recovered during warm boot.
 */
#define BCMIMM_BLK_SIGNATURE   0xe68a24bb

/*!
 * \brief HA pointer constructor macro.
 *
 * \param [out] x This is the pointer value.
 * \param [in] sub This is the sub module ID.
 * \param [in] offset This is the offset of the element from the begining
 * of the block.
 */
#define BCMIMM_SET_PTR(_x, _sub, _offset) \
    _x |= SHR_HA_PTR_CONSTRUCT(_sub, _offset)

/*!
 * \brief HA pointer assignment macro.
 *
 * It is important to notice that the control portion is beeing preserved at
 * the destination.
 *
 * \param [in,out] ptr_d This is the destination pointer to copy to.
 * \param [in] ptr_s This is the source pointer to copy from.
 */
#define BCMIMM_ASSIGN_PTR(_ptr_d,_ptr_s) \
    _ptr_d = ((_ptr_s) & (~SHR_HA_PTR_PRIV_BITS)) | \
              (_ptr_d & SHR_HA_PTR_PRIV_BITS)

/*!
 * \brief Set occupied flag macro.
 *
 * \param [out] x This is the pointer value.
 */
#define BCMIMM_OCCUPIED_SET(_x) _x |= (1 << BCMIMM_OCCUPY_BIT)

/*!
 * \brief Clear occupied flag macro.
 *
 * \param [out] x This is the pointer value.
 */
#define BCMIMM_OCCUPIED_CLEAR(_x) _x &= (~(1 << BCMIMM_OCCUPY_BIT))

/*!
 * \brief Is occupied flag set macro.
 *
 * \param [out] x This is the pointer value.
 */
#define BCMIMM_IS_OCCUPIED(_x) (_x & (1 << BCMIMM_OCCUPY_BIT))

/*!
 * \brief Is the pointer 'NULL' macro.
 *
 * Ignore occupied and lock bits
 *
 * \param [in] x This is the pointer value.
 */
#define BCMIMM_IS_NULL_PTR(_x) (((_x) & (~BCMIMM_PTR_CTRL_MASK)) == 0)

/*!
 * \brief Obtain the element offset from a pointer.
 *
 * \param [in] x This is the pointer value.
 */
#define BCMIMM_OFFSET_GET(_x) (_x) & BCMIMM_OFFSET_MASK

/*!
 * \brief Obtain the sub module value from a pointer.
 *
 * \param [in] x This is the pointer value.
 */
#define BCMIMM_SUB_GET(_x) ((_x) >> BCMIMM_SUB_OFFST) & 0xFF

/*!
 * \brief Set the sub module value into a pointer.
 *
 * \param [out] x This is the pointer value.
 * \param [in] sub This is the sub module value.
 */
#define BCMIMM_SUB_SET(_x, _sub) _x |= ((_sub) << BCMIMM_SUB_OFFST)

/*!
 * \brief Mark the pointer of an entry as locked.
 *
 * \param [out] x This is the pointer value.
 */
#define BCMIMM_ENTRY_LOCK(_x) _x |= 1 << BCMIMM_LOCK_BIT

/*!
 * \brief Mark the pointer of an entry as unlocked.
 *
 * \param [out] x This is the pointer value.
 */
#define BCMIMM_ENTRY_UNLOCK(_x) _x &= ~(1 << BCMIMM_LOCK_BIT)

/*!
 * \brief Does the pointer of an entry locked.
 *
 * \param [in] x This is the pointer value.
 */
#define BCMIMM_IS_LOCKED(_x) (_x) & (1 << BCMIMM_LOCK_BIT)

/*!
 * \brief Obtain the value of the entry lock reference.
 *
 * The entry structure is that the last two words (4 bytes each) are the
 * entry pointer and field pointers. The two bytes prior are the lock
 * reference count.
 *
 * \param [in] _ent Is the entry to check.
 * \param [in] _ent_len Is the table (and entry) entry length (bytes).
 *
 * \return Pointer to the element reference lock count.
 */
#define BCMIMM_ENTRY_LOCK_CNT_GET(_ent, _ent_len) \
    (uint16_t *)((_ent) + (_ent_len) -              \
    2 * BCMIMM_ELEM_PTR_SIZE - sizeof(uint16_t))

/*!
 * \brief Return a pointer to the entry pointer field.
 *
 * \param [in] ent This is the entry pointer.
 * \param [in] end_len This is the entry length (in bytes).
 */
#define BCMIMM_ENTRY_PTR_GET(_ent, _ent_len) \
    (uint32_t *)((_ent) + (_ent_len) - BCMIMM_ELEM_PTR_SIZE)

/*!
 * \brief Return a pointer to the field's pointer field.
 *
 * \param [in] ent This is the entry pointer.
 * \param [in] data_len This is the data length (in bytes) as it was
 * specified during the table create phase.
 */
#define BCMIMM_ENTRY_FLD_PTR_GET(_ent, _data_len) \
    (uint32_t *)((_ent) + (_data_len) - 2 * BCMIMM_ELEM_PTR_SIZE)

/*!
 * \brief Data structure maintains the element list for particular element size.
 *
 * All the element blocks are HA memory blocks. The in-mem back-end allocates
 * HA blocks to maintain fixed amount of elements per block. These elements
 * are used to store the data fields or the entries in the linked list.
 * If all the elements in the HA blocked had been consumed the in-mem back-end
 * allocates an addition HA block and makes more data elements available.
 * The newly allocated HA block will be pointed by few blocks as follows:
 * a. The previous HA block maintains the sub module ID of this block.
 * b. The pointer to the block will be stored in the \c blk array. The
 * index used in the \c blk array is the block index. The HA pointers
 * maintains this index. Note that the index is an index in the array of
 * a regular memory. Therefore it is important to recreate this structure
 * with the exact way it was in previous run.
 * The following diagram illustrates the way the free blocks are organized.
 *
 *   First block sub=1
 *   |---------------|  |--> |---------------------|  |-->...
 *   | FL | ext |next|--|    | FL  | ext=0|  next--|--|
 *|--|-|----|--------|       |---------------------|
 *|  |<-  |ptr|> |ptr|       |                     |
 *|  |---------------|       |---------------------|
 *|  |      .        |       |                     |
 *|  |      .        |       |                     |
 *|  |---------------|       |---------------------|
 *|
 *|->|---------------|
 *   |   | ext |     |
 *|--|------|--------|
 *|  |      .        |
 *|  |      .        |
 *|  |---------------|
 *|
 *|
 *|->|---------------|
 *   |   |ext=0|     |
 *   |---------------|
 *   |      .        |
 *   |      .        |
 *   |---------------|
 *
 *   Each one of these vertical blocks is an HA memory. The HA blocks on the
 *   same vertical column named chain. All blocks in the same chain provides
 *   free list (FL) elements of the same size. The ext field in every HA block
 *   is the sub component ID of the block underneath it. A pointer
 *   (real pointer) for each of the blocks of a chain is stored in the \c blk
 *   array of the \ref bcmimm_free_list_t. The block pointer retained in the
 *   \c blk array in the location (index) that matches its order apperance in
 *   the chain. The first chain top block uses known sub module number (1).
 *   Therefore, to recreate the structure \ref bcmimm_free_list_t during warm
 *   boot, the in-mem back-end starts to allocate HA blocks using sub=1. The
 *   first block will have two pointers:
 *   a. ext contains the sub of the next block in the chain.
 *   b. next contains the sub of the top first block of the next element size.
 *
 *   Upon allocation of the first HA block, the in-mem back-end allocates a
 *   structure of type \ref bcmimm_free_list_t. It also allocates a fixed
 *   size of the \c blk array. The field \c blk_array_size is set to the
 *   initial size of the \c blk array. The value for the field \c data_len
 *   is taken from the HA block header. The in-mem back-end continues to
 *   follow the entire chain started from block 1 and keep updating
 *   the structure \ref bcmimm_free_list_t. Once the chain had been processed
 *   the in-mem back-end starts to process another chain by using the next
 *   sub ID of the first block. A new data structure of type
 *   \ref bcmimm_free_list_t is allocated and linked to the \c next field
 *   of the first \ref bcmimm_free_list_t structure.
 */
typedef struct bcmimm_free_list_s {
    uint32_t data_len;       /*!< The element size represented by this chain.*/
    element_list_head_t *master_blk; /*!< The chain master block. */
    int unit;           /*!< The unit involved with this chain. */
    sal_mutex_t lock;     /*!< Multi thread access protection for this list. */
    /*! Point to the next chain of element blocks. */
    struct bcmimm_free_list_s *next;
} bcmimm_free_list_t;

/*!
 * \brief Data structure associated with every table created by the API.
 *
 * Every table associated with the actual table block and two free lists.
 * One list for fields and the other for entries. This data structure being
 * kept in regular memory and being constructed during table create.
 */
typedef struct bcmimm_tbl_info_s {
    /*! Pointer to the table to manage. */
    tbl_header_t *tbl;
    /*! The unit associated with this table. */
    int unit;
    uint32_t fid_array_size;
    /*! Field ID array. Use to sort the fields ID. Contains indexes of fields.*/
    uint32_t *sorted_fid;
    /*! Pointer to the field free list structure. */
    bcmimm_free_list_t *field_free_list;
    /*! Pointer to the entry free list structure. */
    bcmimm_free_list_t *entry_free_list;
    /*! Table read/write lock. */
    sal_rwlock_t lock;
    /*! Traverse buffer contains sorted array of table key */
    uint8_t *traverse_buf;
    /*! Number of elements in traverse buffer */
    uint32_t trav_buf_size;
} bcmimm_tbl_info_t;

/*!
 * \brief The bcmimm back-end component ID.
 */
extern shr_ha_mod_id bcmimm_be_comp_id;

/*!
 * \brief Counter for used sub component values per unit.
 */
extern shr_ha_sub_id bcmimm_available_subs[BCMDRD_CONFIG_MAX_UNITS];

/*!
 * \brief Maintains a link list of all the free fields (of all sizes).
 *
 * This list maintain one one element from each free block size in a list.
 */
extern bcmimm_free_list_t *bcmimm_free_elem[BCMDRD_CONFIG_MAX_UNITS];

/*!
 * \brief Free list element get.
 *
 * This function obtains an element from the free list.
 *
 * \param [in] unit The device number associated with this request.
 * \param [in] elem_fr_lst Pointer to the element free list.
 * \param [out] ptr HA pointer to the newly allocated element.
 *
 * \return Pointer to the element on success and NULL otherwise.
 */
extern uint8_t *bcmimm_free_list_elem_get(int unit,
                                       bcmimm_free_list_t *elem_fr_lst,
                                       uint32_t *ptr);

/*!
 * \brief Return element list to the free list
 *
 * This function returns previously allocated elements (allocated using
 * \ref bcmimm_free_list_elem_get()) into the free list so these elements
 * can be used again. The variable \c ptr is an HA pointer to the first
 * element in the list. The function go through the list pointed by \c ptr
 * and finds the last element. It then set the pointer of the last element
 * to point to the first element in the free list. Last, it sets the free
 * list pointer (obtained from the parameter \c elem_fr_lst) to the begining
 * of the list (the original value of the parameter \c ptr).
 *
 * \param [in] elem_fr_lst This is the element free list control structure.
 * \param [in] ptr This is HA pointer pointing to the first element in the
 * list to free.
 *
 * \return None.
 */
extern void bcmimm_free_list_elem_put(bcmimm_free_list_t *elem_fr_lst,
                                      uint32_t ptr);

/*!
 * \brief Search for the free list to match the required size.
 *
 * This function seaches the master list for a free list that can be used
 * to allocate element of the desired size. To reduce the number of free
 * lists, this function accepts lists that are larger by up to 2 bytes
 * from the desired size. If no list is found to satisfy the required size
 * the function will allocate a new list to match the desired size.
 *
 * \param [in] unit The device number associated with this request.
 * \param [in] size This is the desired (minimal) element size.
 * \param [in] list This is a pointer to the master list. The master
 * list will be updated if the function allocates a new free-list.
 *
 * \return Pointer to the desired free list on success and NULL otherwise.
 */
extern bcmimm_free_list_t *bcmimm_free_list_find(int unit,
                                                size_t size,
                                                bcmimm_free_list_t **list);

/*!
 * \brief Obtains an entry from a free list and HA pointer.
 *
 * This function obtains an entry from a free list using HA pointer value.
 * The HA pointer points to the desired entry within the element list.
 *
 * \param [in] elem_lst This is the element list containing the desired entry.
 * \param [in] ptr This is HA pointer, pointing to the desired entry inside
 * the element list
 *
 * \return Pointer to the entry on success and NULL otherwise.
 */
extern uint8_t *bcmimm_entry_get (bcmimm_free_list_t *elem_lst, uint32_t ptr);

/*!
 * \brief Inserts data field into an entry.
 *
 * This function converts the data array with data_element field into a linked
 * list of fields. Each field has a fixed size determine at table creation.
 * The function allocates memory fields from its associated free list
 * (association was established at table creation). The first element should
 * be pointed by the input parameter ent_ptr. All the rest of the fields being
 * pointed by the previous field. It is important to maintain the same order
 * of the data with the linked list.
 *
 * \param [in] t_info This is the table information containing the entry.
 * \param [in] ent_ptr This is the HA pointer in the entry that points to the
 * list of data fields.
 * \param [in] in This is a structure containing the fields values and IDs.
 * The structure \c in contains a field named \c count that indicates the
 * number of fields in the input. The fields information being partition into
 * two synchronized arrays. One with the field IDs and the other with the
 * field data. The arrays synchronized in a way that the j'th element of the
 * \c fid array correspond to the data of the j'th element of \c fdata array.
 *
 * \return SHR_E_NONE success, SHR_E_MEMORY if failed to allocate internal
 * memory.
 */
extern int bcmimm_data_field_insert(bcmimm_tbl_info_t *t_info,
                                    uint32_t *ent_ptr,
                                    const bcmimm_be_fields_t *in);

/*!
 * \brief Retrieve linked list data fields into data array.
 *
 * This function performs the inverse functionality of the function
 * \ref bcmimm_data_field_insert. It parses the fields linked list and copied the
 * data into the buffer provided by the caller.
 *
 * \param [in] t_info This is the table information containing the entry.
 * \param [in] ent_ptr This is the HA pointer to the first data field of the
 * list.
 * \param [out] out This is a structure where the entry data should be copied
 * into. The \c out structure contains two arrays and a count field indicating
 * the size of each array. Upon return the count field will include the number
 * of elements that were actually copied into the \c out structure.
 *
 * \return SHR_E_NONE success, SHR_E_FULL if there is insufficient space in the
 * output buffer (\c out).
 */
extern int bcmimm_data_field_get(bcmimm_tbl_info_t *t_info,
                                 uint32_t ent_ptr,
                                 bcmimm_be_fields_t *out);

/*!
 * \brief Updates linked list data fields.
 *
 * This function updates the fields provided in the input parameter to their
 * desired value. Fields that are not specified will be kept. The input
 * parameter contains an action for every field. The action can be to add
 * delete or modify the field data.
 * This function uses the fact that the link list is sorted based on field ID
 * to achieve faster performance. The alternative will require search for
 * every input field in the linked list (O(n^2) vs. O(n log n)).
 *
 * \param [in] t_info This is the table information containing the entry.
 * \param [in] ent_ptr This is the HA pointer to the first data field of the
 * list.
 * \param [in] in This is a structure containing the fields values and IDs.
 * The structure \c in contains a field named \c count that indicates the
 * number of entry fields in the input. The entry fields information being
 * partition into three synchronized arrays. One with the field IDs, one
 * with the desired field operation and the last with the entry
 * field data. The arrays synchronized in a way that the j'th element of the
 * \c fid array correspond to the data of the j'th element of \c fdata array.
 *
 * \return SHR_E_NONE success, error code otherwise.
 */
extern int bcmimm_data_field_update(bcmimm_tbl_info_t *t_info,
                                    uint32_t *ent_ptr,
                                    const bcmimm_be_fields_upd_t *in);

/*!
 * \brief Overwrite fields LTIDs.
 *
 * This function overwrites the field ID value (first 32 bits) of every field
 * in a given entry with the values provided in the input \c in.
 * The assumption is that the order of the fields in the input \c in matches
 * the order of the fields in the entry.
 *
 * \param [in] t_info This is the table information containing the entry.
 * \param [in,out] ent_ptr This is the HA pointer to the first data field of the
 * entry. Since this function might delete fields that are no longer valid
 * the value of this pointer might be updated by this function.
 * \param [in] in List of data field values. More important this variable
 * contains the list of field IDs that will overwrite the current field IDs
 * of the entry.
 *
 * \return SHR_E_NONE success, error code otherwise.
 */
extern int bcmimm_field_id_overwrite(bcmimm_tbl_info_t *t_info,
                                     uint32_t *ent_ptr,
                                     bcmimm_be_fields_t *in);

/*!
 * \brief Search for an entry in the table.
 *
 * This function search for the entry with a matching key inside the table
 * pointed by \c t_info. If found, the function will also provide the entry
 * pointer field of the entry. If the parameter \c prev_ent is not NULL, the
 * function will also return a pointer to the previous entry (if one exists)
 * that points to the entry that was searched.
 *
 * \param [in] t_info This is point to the table information.
 * \param [in] key This is the key value to be used during the search.
 * \param [out] ent_ptr This will point to the entry pointer field in
 * the searched entry.
 * \param [out] prev_ent This pointer points to the entry that preceded the
 * searched entry.
 *
 * \return Pointer to the searched entry if entry was found or NULL otherwise.
 */
extern uint8_t *bcmimm_entry_find(bcmimm_tbl_info_t *t_info,
                                  const void *key,
                                  uint32_t **ent_ptr,
                                  uint8_t **prev_ent);

/*!
 * \brief Retrieve the free element blocks in warm boot.
 *
 * This function starts from a given sub module value and retrieve all the
 * element blocks (free and allocated) from the previous run. The function
 * parses the higher row of the element blocks by following the \c next field
 * of the \ref bcmimm_free_list_t structure. For each block found, it allocates
 * the entire chain. See \ref bcmimm_free_list_t for more details.
 *
 * \param [in] unit Device number for the table of interest.
 * \param [in] sub This is the sub module to start with.
 *
 * \return SHR_E_NONE success, SHR_E_MEMORY on error.
 */
extern int bcmimm_free_lists_retrieve(int unit, shr_ha_sub_id sub);

/*!
 * \brief Clear the table from its content.
 *
 * This function free all the entries of the table. The state of the table
 * after the clear will match the state of the table after it had just been
 * created during cold boot.
 *
 * \param [in] t_info This is a pointer to all the relevant information
 * associated with the table.
 *
 * \return None.
 */
extern void bcmimm_table_content_clear(bcmimm_tbl_info_t *t_info);

/*!
 * \brief Finds the next entry in a table.
 *
 * This function searches for the next entry in the table. The search is done
 * on the keys of the traverse buffer. If \c first is set the search of the
 * entry starts from the beginning of the traverse buffer. Otherwise, using
 * binary search the function finds the index of the input key \c in_key. The
 * search will start from the next index.
 *
 * \param [in] t_info This is a pointer to all the relevant information
 * associated with the table.
 * \param [in] first Indicates if this is get first or not.
 * \param [in] in_key If entry is not first then this is the key of the previous
 * entry.
 *
 * \return Pointer to the next entry if found and NULL if no more entries
 * are available.
 */
extern uint8_t *bcmimm_next_entry_find(bcmimm_tbl_info_t *t_info,
                                       bool first,
                                       uint8_t *in_key);

/*!
 * \brief Find the next entry in a table - raw search.
 *
 * This function searches for the next entry in the table. The search is done
 * in a sequential way on the raw index table. The search always starts from
 * the start_ptr which is the HA pointer from the previous traverse (or zero
 * for the first traverse).
 * The parameter \c first indicates if the current entry pointed by \c start_ptr
 * should be consider as output (yes if first == true) or not.
 * Entries are organized in bins, where multiple entries might be sharing the
 * same bin. In such a case the entries will be linked in a linked list.
 * This function will traverse the linked list until it reaches its end (NULL).
 * It will then increment to the next bin.
 *
 * \param [in] t_info This is a pointer to all the relevant information
 * associated with the table.
 * \param [in] first Indicates if this is get first or not.
 * \param [in] start_idx Start the search of an entry from this index
 * (slot).
 * \param [in,out] start_ptr Start the search from this pointer HA pointer.
 * This pointer is not 0 in case multiple entries are at the same slot.
 *
 * \return Pointer to the next entry if found and NULL if no more entries
 * are available.
 */
extern uint8_t *bcmimm_next_entry_raw_find(bcmimm_tbl_info_t *t_info,
                                           bool first,
                                           uint32_t start_idx,
                                           uint32_t *start_ptr);

/*!
 * \brief Prepare traverse buffer.
 *
 * This function allocates the traverse buffer to be the size of a key times
 * the number of entries in the table. It then traverses the table and copies
 * the key of each entry into the traverse buffer. Once that is complete, the
 * function sorts all the keys in the traverse buffer.
 *
 * \param [in] t_info This is a pointer to all the relevant information
 * associated with the table.
 *
 * \return SHR_E_NONE - successful.
 * \return SHR_E_MEMORY - failed to allocate the memory required for the
 * traverse buffer.
 */
extern int bcmimm_prep_traverse_buf(bcmimm_tbl_info_t *t_info);

/*!
 *\brief Insert an entry into direct table.
 *
 * This function inserts an entry into direct table.
 *
 * \param [in] t_info Information related to the direct table.
 * \param [in] key The entry key.
 * \param [in] fld_cnt Indicates how many fields are in the buffer \c buf.
 * \param [in] buf Buffer contained the entry fields information.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM The key value exceeds the table size.
 * \retval SHR_E_EXISTS Entry already exist.
 */
extern int bcmimm_be_direct_entry_insert(bcmimm_tbl_info_t *t_info,
                                         uint32_t key,
                                         uint32_t fld_cnt,
                                         void *buf);

/*!
 *\brief Update an entry of a direct table.
 *
 * This function updates an entry of a direct table.
 * The function deletes fields that their corresponding value in the
 * \c del array is true. The function makes sure that all the fields nformation
 * will occupy the entry array from the begining of the entry. The function also
 * updates the field count (entry->count) to reflect the number of fields within
 * the entry.
 *
 * \param [in] t_info Information related to the direct table.
 * \param [in] key The entry key.
 * \param [in] fld_cnt Indicates how many fields are in the buffer \c buf.
 * \param [in] del Array indicating for every field if it should be deleted or
 * not.
 * \param [in] buf Buffer contained the entry fields information.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM The key value exceeds the table size.
 * \retval SHR_E_EMPTY The entry doesn't exist.
 */
extern int bcmimm_be_direct_entry_update(bcmimm_tbl_info_t *t_info,
                                         uint32_t key,
                                         uint32_t fld_cnt,
                                         bool *del,
                                         void *buf);

/*!
 *\brief Lookup an entry of a direct table.
 *
 * This function lookup an entry of a direct table.
 *
 * \param [in] t_info Information related to the direct table.
 * \param [in] key The entry key.
 * \param [in,out] fld_cnt Indicates how many fields are in the buffer \c buf.
 * \param [out] buf Buffer to place the entry fields information.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM The key value exceeds the table size.
 * \retval SHR_E_NOT_FOUND Entry not found.
 */
extern int bcmimm_be_direct_entry_lookup(bcmimm_tbl_info_t *t_info,
                                         uint32_t key,
                                         size_t *fld_cnt,
                                         void *buf);

/*!
 * \brief Create back-end direct table snapshot.
 *
 * This function allocates memory and records the key field(s) of every entry
 * within the newly allocated memory. By doing so the system creates a snapshot
 * of all the entries within the specific LT.
 *
 * \param [in] t_info Information related to the direct table.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_NOT_FOUND The table is empty.
 * \retval SHR_E_MEMORY Memory allocation failed.
 */
extern int bcmimm_be_direct_snapshot_create(bcmimm_tbl_info_t *t_info);

/*!
 *\brief Traverse lookup for the first entry of a direct table.
 *
 * This function lookup for the first entry of a direct table.
 * Based on the value of \c use_snapshot the search will be performed on the
 * snapshot or directly on the raw table by incrementing the entry index until
 * an entry is found.
 *
 * \param [in] t_info Information related to the direct table.
 * \param [in] use_snapshot Indicates if the search should be done on the
 * snapshot buffer or not.
 * \param [in,out] key entry key. The initial value being used to start the
 * search for an entry. Once an entry being found its key will be placed within
 * this parameter.
 * \param [in,out] fld_cnt Indicates how many fields are in the buffer \c buf.
 * \param [out] buf Buffer to place the entry fields information.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_NOT_FOUND Entry not found.
 * \retval SHR_E_PARAM When use_snapshort is true but no snapshot is available
 * (i.e. previous call to \ref bcmimm_be_direct_snapshot_create() has not been
 * made).
 */
extern int bcmimm_be_direct_entry_get_first(bcmimm_tbl_info_t *t_info,
                                            bool use_snapshot,
                                            uint32_t *key,
                                            size_t *fld_cnt,
                                            void *buf);

/*!
 *\brief Traverse lookup for the next entry of a direct table.
 *
 * This function lookup for the first entry of a direct table.
 * The search is starting based on the value of the key as it was previously
 * retrieved by the function \ref bcmimm_be_direct_ctx_get_first() or by this
 * function.
 * This search is only done on a snapshot that was taken previously by the
 * function \ref bcmimm_be_direct_ctx_get_first().
 *
 * \param [in] t_info Information related to the direct table.
 * \param [in] use_snapshot Indicates if the search should be done on the
 * snapshot buffer or not.
 * \param [in,out] key entry key. The initial value being used to start the
 * search for an entry. Once an entry being found its key will be placed within
 * this parameter.
 * \param [in,out] fld_cnt Indicates how many fields are in the buffer \c buf.
 * \param [out] buf Buffer to place the entry fields information.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_NOT_FOUND Entry not found.
 */
extern int bcmimm_be_direct_entry_get_next(bcmimm_tbl_info_t *t_info,
                                           bool use_snapshot,
                                           uint32_t *key,
                                           size_t *fld_cnt,
                                           void *buf);

/*!
 *\brief Delete an entry of a direct table.
 *
 * This function lookup for the next entry of a direct table.
 *
 * \param [in] t_info Information related to the direct table.
 * \param [in] key entry key.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM The key value exceeds the table size.
 * \retval SHR_E_ACCESS Entry is locked.
 */
extern int bcmimm_be_direct_entry_delete(bcmimm_tbl_info_t *t_info,
                                         uint32_t key);

/*!
 *\brief Clears the content of a direct table.
 *
 * This function clears the content of a direct table.
 *
 * \param [in] t_info Information related to the direct table.
 *
 * \retval None.
 */
extern void bcmimm_be_direct_tbl_clear(bcmimm_tbl_info_t *t_info);

/*!
 *\brief Retrieve the table entry count for direct table.
 *
 * This function retrieves the entry count of a direct table.
 *
 * \param [in] t_info Information related to the direct table.
 *
 * \retval The entry count.
 */
extern uint32_t bcmimm_be_direct_tbl_cnt_get(bcmimm_tbl_info_t *t_info);

/*!
 *\brief Increment the locks count of an entry in a direct table.
 *
 * This function increments the lock count of an entry in a direct
 * table. The lock count increment by 1.
 *
 * \param [in] t_info Information related to the direct table.
 * \param [in] key entry key.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_EMPTY Entry not found.
 * \retval SHR_E_UNAVAIL The expected lock count is too large or small.
 */
extern int bcmimm_be_direct_entry_lock(bcmimm_tbl_info_t *t_info,
                                       uint32_t *key);

/*!
 *\brief Decrement the locks count of an entry in a direct table.
 *
 * This function decrement the lock count of an entry in a direct
 * table. The lock count decrement by 1.
 *
 * \param [in] t_info Information related to the direct table.
 * \param [in] key entry key.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_EMPTY Entry not found.
 * \retval SHR_E_UNAVAIL The expected lock count is too large or small.
 */
extern int bcmimm_be_direct_entry_unlock(bcmimm_tbl_info_t *t_info,
                                         uint32_t *key);

/*!
 *\brief Retrieve an entry lock count for a direct table.
 *
 * This function retrieves the lock count of an entry in a direct table.
 *
 * \param [in] t_info Information related to the direct table.
 * \param [in] key entry key.
 * \param [out] lock_cnt The retrieved lock count.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid key value.
 * \retval SHR_E_EMPTY The entry is empty.
 */
extern int bcmimm_be_direct_entry_lock_cnt(bcmimm_tbl_info_t *t_info,
                                           uint32_t *key,
                                           uint32_t *lock_cnt);

/*!
 * \brief Override the field IDs of direct table.
 *
 * This function blindly goes through all the fields in an entry and overwrites
 * each field ID with a new value.
 * This function is only being used for software upgrade where field ID values
 * may change.
 *
 * \param [in] t_info Table info contains all the current data before the
 * change.
 * \param [in] key entry key.
 * \param [in] in Input fields contains new field IDs. The fields are organized
 * at the same order as the existing entry.
 *
 * \retval SHR_E_NONE Success. Error code otherwise.
 */
extern int bcmimm_be_direct_entry_overide(bcmimm_tbl_info_t *t_info,
                                          uint32_t *key,
                                          bcmimm_be_fields_t *in);

/*!
 * \brief Rearrange direct table entries to larger entry length.
 *
 * This function changes the body content to adjust into new entry length.
 * The function starts to move the last entry to its new location. Since
 * the entry length increases the new location is not contaminated.
 *
 * \param [in] t_info Table info contains all the current data before the
 * change.
 * \param [in] entry_len The new entry length.
 *
 * \retval SHR_E_NONE Success. Error code otherwise.
 */
extern void bcmimm_be_adjust_direct_entry_length(bcmimm_tbl_info_t *t_info,
                                                 uint32_t entry_len);

/*!
 * \brief Retrieve the HA block associated with specific unit and sub
 * component ID.
 *
 * This function retrieves a HA block that is managed by the back-end.
 *
 * \param [in] unit Unit associated with the HA block.
 * \param [in] sub Sub component ID associated with the HA block.
 *
 * \return Pointer to the HA block Success.
 * \return NULL Failure.
 */
extern element_list_head_t *bcmimm_get_ha_blk(int unit, shr_ha_sub_id sub);

/*!
 * \brief Update back-end data length onto larger length.
 *
 * This function replaces all the fields in all the entries with fields that
 * has larger length. This occurs when the max field size of an LT got larger.
 *
 * \param [in] unit The unit associated with the LT.
 * \param [in] t_info Table info.
 * \param [in] data_len The new data length.
 * \param [in] big_endian Indicating if the integers are big or little endian.
 *
 * \return SHR_E_NONE Success. Error code otherwise.
 */
extern int bcmimm_be_update_data_len(int unit,
                                     bcmimm_tbl_info_t *t_info,
                                     uint32_t data_len,
                                     bool big_endian);

/*!
 * \brief Update back-end key length.
 *
 * This function replaces all the entries key length with an entry of different
 * length. Note that if the key length is being reduced it is assumed that
 * the actual key value will not be truncated.
 *
 * \param [in] unit The unit associated with the LT.
 * \param [in] t_info Table info.
 * \param [in] key_len The new key length.
 * \param [in] entry_len The new entry length.
 * \param [in] big_endian Indicating if the integers are big or little endian.
 *
 * \return SHR_E_NONE Success. Error code otherwise.
 */
extern int bcmimm_be_update_key_len(int unit,
                                    bcmimm_tbl_info_t *t_info,
                                    uint32_t key_len,
                                    uint32_t entry_len,
                                    bool big_endian);

#endif /* BE_INTERNALS_H */
