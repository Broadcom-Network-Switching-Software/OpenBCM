/*! \file be_internals.c
 *
 * In-Memory back-end internal implementation.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <sal/sal_alloc.h>
#include <sal/sal_rwlock.h>
#include <sal/sal_mutex.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>
#include <shr/shr_ha.h>
#include <shr/shr_hash.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmdrd_config.h>
#include <bcmissu/issu_api.h>
#include <bcmimm/bcmimm_backend.h>
#include <bcmimm/generated/back_e_ha.h>
#include "be_internals.h"

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMIMM_BACKEND

#define ELEM_CHUNK_SIZE    40000

/*!
 * \brief Quick sort stack data structure.
 */
typedef struct {
    int blk_size;     /*!< The depth of the stack */
    uint32_t *blk;    /*!< The stack memory */
    sal_mutex_t mtx;  /*1< Protect access to the structure */
} quicksort_stack_t;

/*!
 * \brief Helper array maps between HA block sub ID to physical memory.
 */
static element_list_head_t *fix_element_blk[BCMDRD_CONFIG_MAX_UNITS][SHR_HA_SUB_IDS];

static sal_mutex_t sort_mutex;
static size_t sort_key_len;

/*
 * This stack is used by the quicksort function to avoid using recursive sort.
 */
static quicksort_stack_t quicksort_stack[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Calculate progressive element count for free block.
 *
 * This function uses the sub value to calculate the expected element count for
 * the free elements block. As the sub value increases the function allows
 * larger free blocks before the sub reaches its max value of 255.
 *
 * \param [in] sub The sub value.
 *
 * \return The number of elements that should be allocated.
 */
static uint32_t element_cnt_calc(shr_ha_sub_id sub) {
    uint32_t elem_cnt;

    if (sub < 100) {
        elem_cnt = ELEM_CHUNK_SIZE;
    } else if (sub < 150) {
        elem_cnt = 2 * ELEM_CHUNK_SIZE;
    } else if (sub < 200) {
        elem_cnt = 4 * ELEM_CHUNK_SIZE;
    } else {
        elem_cnt = 8 * ELEM_CHUNK_SIZE;
    }
    return elem_cnt;
}

/*!
 * \brief Add block element and its associated chain to the block elements pool.
 *
 * This function is called to recover a complete block chain for an element of
 * a specific size (warm boot only).
 *
 * \param [in] unit This is the unit associated with this block elements.
 * \param [in] f_blk This is the first free block in the chain. A chain
 * composes of multiple blocks containing the same element size. If this is
 * not the last block the field blk_ext points (HA pointer) to the next block
 * in the chain. f_blk is always valid pointer (not NULL).
 * \param [out] f_list This is the free list of elements of all sizes. This
 * list will be updated to contain the newlly recovered chain.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int chain_blk_add(int unit,
                         element_list_head_t *f_blk,
                         bcmimm_free_list_t **f_list)
{
    bcmimm_free_list_t *new_list = NULL;
    shr_ha_sub_id sub;
    element_list_head_t *blk = f_blk;
    uint32_t len = ELEM_CHUNK_SIZE * f_blk->data_len +
                    sizeof(element_list_head_t);
    char ha_id[MAX_BLK_ID_STR_LEN];
    char block_desc[20];

    SHR_FUNC_ENTER(unit);
    SHR_ALLOC(new_list, sizeof(bcmimm_free_list_t), "bcmimmBackend");
    SHR_NULL_CHECK(new_list, SHR_E_MEMORY);
    new_list->lock = sal_mutex_create("bcmimmBackend");
    SHR_NULL_CHECK(new_list->lock, SHR_E_MEMORY);
    fix_element_blk[unit][f_blk->sub] = blk;
    new_list->master_blk = blk;
    new_list->unit = unit;
    new_list->data_len = blk->data_len;
    new_list->next = *f_list;

    while (true) {
        if (blk->blk_ext == 0) {
            break;
        }
        sub = BCMIMM_SUB_GET(blk->blk_ext);
        sal_snprintf(block_desc, 20, "be_data_len_%d", (int)blk->data_len);
        SHR_IF_ERR_EXIT
            (shr_ha_str_to_id(block_desc,
                              SHR_HA_BLK_USAGE_OTHER,
                              MAX_BLK_ID_STR_LEN,
                              ha_id));
        blk = shr_ha_mem_alloc(unit,
                               bcmimm_be_comp_id,
                               sub,
                               ha_id,
                               &len);
        SHR_NULL_CHECK(blk, SHR_E_MEMORY);
        if ((blk->signature != BCMIMM_BLK_SIGNATURE) || (blk->sub != sub)) {
            /*
             * Free the block and break. There is nothing in previous run
             * that can be recovered.
             */
            shr_ha_mem_free(unit, blk);
            break;
        }
        /* The new field must have valid value. */
        if (blk->elem_count == 0) {
            blk->elem_count =
                (len - sizeof(element_list_head_t)) / blk->data_len;
        }
        fix_element_blk[unit][sub] = blk;
        if (blk->sub >= bcmimm_available_subs[unit]) {
            bcmimm_available_subs[unit] = blk->sub + 1;
        }
    }
    *f_list = new_list;

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(new_list);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize an empty free block.
 *
 * This function initializes element free list blocks. The initialization
 * includes building a linked list for all the elements (where each element
 * points to the element that its physically after). In the case that this
 * is not the first block of this size (there is a first block) we set the
 * free list pointer to the first element in this new block.
 *
 * \param [in] blk This is the block to initialize.
 * \param [in] first_blk Points to the first block on the chain (if this call
 * was made for non-first block).
 * \param [in] elem_cnt How many elements are in this block.
 *
 * \return None.
 */
static void free_blk_init(element_list_head_t *blk,
                          element_list_head_t *first_blk,
                          uint32_t elem_cnt)
{
    uint32_t idx;
    shr_ha_sub_id sub = blk->sub;
    uint8_t *elem = ((uint8_t *)blk) + sizeof(element_list_head_t);
    uint32_t *ptr;
    uint32_t basic_ptr_val = 0;
    uint32_t data_len = blk->data_len;
    size_t ptr_offset = data_len - BCMIMM_ELEM_PTR_SIZE;

    /*
     * Set the pointer value to the first element in this block. HA pointer
     * composed of the sub module of this block, the array index of this block
     * in the chain and the offset. Since this is the first block offset = 0.
     */
    BCMIMM_SET_PTR(basic_ptr_val, sub, 0);
    blk->free_list = basic_ptr_val;

    for (idx = 1; idx < elem_cnt; idx++) {
        ptr = (uint32_t *)(elem + ptr_offset);
        *ptr = basic_ptr_val + idx;
        elem += data_len;
    }

    *ptr = 0;   /* Override the last ptr to terminate the linked list */
    if (first_blk) {
        first_blk->free_list = basic_ptr_val;
    }
}

/*!
 * \brief Allocate block to serve new element size.
 *
 * This function is called to allocate a free block (HA) containing elements
 * of size that is not current available. The newly allocated block will
 * be at the top of the chain of free blocks serving the same element size.
 *
 * \param [in] unit This is the unit associated with this block elements.
 * \param [in] size The desired size of the element.
 * \param [out] list This is the free list of elements of all sizes.
 *
 * \return pointer to the new element list if success and NULL otherwise.
 */
static bcmimm_free_list_t *new_elem_size_alloc(int unit,
                                        size_t size,
                                        bcmimm_free_list_t **list)
{
    uint32_t len, actual_len;
    element_list_head_t *blk = NULL;
    element_list_head_t *next_blk;
    shr_ha_sub_id sub = bcmimm_available_subs[unit]++;
    bcmimm_free_list_t *new_list;
    char ha_id[MAX_BLK_ID_STR_LEN];
    char block_desc[20];

    /* Need to allocate a new list */
    /*
     * Calculate the length.
     * New allocation length is always the default length.
     */
    len = ELEM_CHUNK_SIZE * size + sizeof(element_list_head_t);
    actual_len = len;
    sal_snprintf(block_desc, 20, "be_data_len_%d", (int)size);
    shr_ha_str_to_id(block_desc,
                     SHR_HA_BLK_USAGE_OTHER,
                     MAX_BLK_ID_STR_LEN,
                     ha_id);
    blk = shr_ha_mem_alloc(unit,
                           bcmimm_be_comp_id,
                           sub,
                           ha_id,
                           &actual_len);
    if (blk == NULL) {
        return NULL;
    }
    if (actual_len < len) {
        shr_ha_mem_free(unit, blk);
        return NULL;
    }
    sal_memset(blk, 0, actual_len);
    /* Add new element size into the list of elements */
    new_list = sal_alloc(sizeof(*new_list), "bcmimmBackend");
    if (new_list == NULL) {
        return NULL;
    }

    bcmissu_struct_info_report(unit, bcmimm_be_comp_id,
                               sub, 0, sizeof(element_list_head_t), 1,
                               ELEMENT_LIST_HEAD_T_ID);

    new_list->lock = sal_mutex_create("bcmimmBackend");
    fix_element_blk[unit][sub] = blk;
    new_list->data_len = size;
    new_list->next = *list;
    *list = new_list;
    blk->data_len = size;
    blk->sub = sub;
    blk->blk_ext = 0;
    blk->elem_count = ELEM_CHUNK_SIZE;
    new_list->master_blk = blk;
    new_list->unit = unit;

    if (new_list->next != NULL) {
        next_blk = new_list->next->master_blk;
        /*
         * Make the previous block point to the sub of this block. Upon
         * recovery we need to recover this block based on its sub. The recovery
         * starts from a known sub IDs (1 and 2) for entries and fields.
         */
        BCMIMM_SUB_SET(next_blk->next_size_blk, sub);
    }

    blk->next_size_blk = 0;
    free_blk_init(blk, NULL, ELEM_CHUNK_SIZE);
    blk->signature = BCMIMM_BLK_SIGNATURE;
    return new_list;
}

/*!
 * \brief Allocate new block to serve existing element size.
 *
 * This function is called to allocate a free block (HA) in the chain of free
 * element blocks. Note that the size of elements is already exist.
 *
 * \param [in] unit This is the unit associated with this block elements.
 * \param [in] prev_blk This is the previous block in the chain. Note that this
 * function is a chain extension for block containing the same element size.
 * Hence previous block should not be NULL.
 * \param [in] f_list This is the element free list which manages
 * the chain of blocks of same element size.
 *
 * \return 0 on success and -1 otherwise.
 */
static int new_elem_blk_add(int unit,
                            element_list_head_t *prev_blk,
                            bcmimm_free_list_t *f_list)
{
    uint32_t len;
    element_list_head_t *blk;
    shr_ha_sub_id sub = bcmimm_available_subs[unit]++;
    uint32_t elem_cnt;
    char ha_id[MAX_BLK_ID_STR_LEN];
    char block_desc[20];

    elem_cnt = element_cnt_calc(sub);
    len = elem_cnt * f_list->data_len +
           sizeof(element_list_head_t);
    sal_snprintf(block_desc, 20, "be_data_len_%d", f_list->data_len);
    shr_ha_str_to_id(block_desc,
                     SHR_HA_BLK_USAGE_OTHER,
                     MAX_BLK_ID_STR_LEN,
                     ha_id);
    blk = shr_ha_mem_alloc(unit,
                           bcmimm_be_comp_id,
                           sub,
                           ha_id,
                           &len);
    if (blk == NULL) {
        return -1;
    }

    sal_memset(blk, 0, len); /* Prevent false block mismatch errors */
    blk->data_len = f_list->data_len;
    blk->blk_ext = 0;
    blk->sub = sub;
    blk->free_list = 0;
    blk->next_size_blk = 0;
    blk->elem_count = elem_cnt;
    /* Inform the ISSU about the block header */
    bcmissu_struct_info_report(unit, bcmimm_be_comp_id,
                               sub, 0, sizeof(element_list_head_t), 1,
                               ELEMENT_LIST_HEAD_T_ID);

    fix_element_blk[unit][sub] = blk;
    free_blk_init(blk, f_list->master_blk, elem_cnt);
    /* Link the previous block to this one (in HA) */
    BCMIMM_SUB_SET(prev_blk->blk_ext, sub);
    blk->signature = BCMIMM_BLK_SIGNATURE;     /* Now the block is ready */
    return 0;
}

/*!
 * \brief Quick sort helper function.
 *
 * This function perform pivot partition where all the number smaller or equal
 * to the pivot value will be set at the index that is incrementing from the
 * \c start_idx value and all the numbers, larger then the pivot value, will be
 * placed at a decrementing index starting at \c end_idx value.
 * For performance reasons this function is an inline function.
 *
 * \param [out] idx_array This is an array containing indexes to all the
 * elements in the \c ref_array. Sorting will be done on this array based
 * on the value of the \c ref_array elements.
 * \param [in] ref_array This is a constant array that contains the values
 * that should be sorted.
 * \param [in] start_idx This is the first index of the array to start the
 * sorting.
 * \param [in] end_idx This is the last index of the array to start the
 * sorting.
 *
 * \return The index where all the numbers at higher index are larger then the
 * the number of the returned index and all the numbers at a lower index are
 * smaller or equalin value.
 */
static inline int pivot_partition(uint32_t *idx_array,
                                  const uint32_t *ref_array,
                                  int start_idx,
                                  int end_idx)
{
    int tmp;
    int index1, index2;
    uint32_t pivot_val = ref_array[idx_array[start_idx]];

    index1 = start_idx;
    index2 = end_idx;

    /* Sorting in ascending order with quick sort */
    while (index1 < index2) {
        while ((index1 < end_idx) &&
               (ref_array[idx_array[index1]] <= pivot_val)) {
            index1++;
        }
        while (ref_array[idx_array[index2]] > pivot_val) {
            index2--;
        }

        if (index1 < index2) {
            /* Swap */
            tmp = idx_array[index1];
            idx_array[index1] = idx_array[index2];
            idx_array[index2] = tmp;
        }
    }

    /*
     * At the end of first iteration, swap pivot element with index2
     * element.
     */
    if (start_idx != index2) {
        tmp = idx_array[start_idx];
        idx_array[start_idx] = idx_array[index2];
        idx_array[index2] = tmp;
    }
    return index2;
}


/*!
 * \brief Quick sort function.
 *
 * This function runs the quick sort algorithem on the ref_array. However,
 * instead of switching the refrence array elements the function replaces
 * the indexes in the \c idx_array. This is non-recursive function but it
 * uses pre-allocated stack.
 *
 * \param [out] idx_array This is an array containing indexes to all the
 * elements in the \c ref_array. Sorting will be done on this array based
 * on the value of the \c ref_array elements.
 * \param [in] ref_array This is a constant array that contains the values
 * that should be sorted.
 * \param [in] start_idx This is the first index of the array to start the
 * sorting.
 * \param [in] end_idx This is the last index of the array to start the
 * sorting.
 *
 * \return None
 */
static void quicksort(int unit,
                      uint32_t *idx_array,
                      const uint32_t *ref_array,
                      int start_idx,
                      int end_idx)
{
    uint32_t *stack;
    int top = 0;
    int s, e, p;

    if (end_idx - start_idx < 1) {
        return;  /* Nothing to do */
    }
    sal_mutex_take(quicksort_stack[unit].mtx, SAL_MUTEX_FOREVER);
    /* First make sure that there is a stack */
    if (quicksort_stack[unit].blk_size < end_idx - start_idx + 4) {
        if (quicksort_stack[unit].blk) {
            SHR_FREE(quicksort_stack[unit].blk);
        }
        quicksort_stack[unit].blk = sal_alloc
            ((end_idx - start_idx + 4) * sizeof(uint32_t),
             "bcmimmBeInternQSrt");
        assert(quicksort_stack[unit].blk);
        quicksort_stack[unit].blk_size = end_idx - start_idx + 4;
    }
    stack = quicksort_stack[unit].blk;
    stack[top++] = start_idx;
    stack[top++] = end_idx;

    while (top > 0 && top < quicksort_stack[unit].blk_size - 2) {
        e = stack[--top];
        s = stack[--top];
        p = pivot_partition(idx_array, ref_array, s, e);
        if (p - 1 > s) {
            stack[top++] = s;
            stack[top++] = p - 1;
        }
        if (p + 1 < e) {
            stack[top++] = p + 1;
            stack[top++] = e;
        }
    }
    assert (top == 0);
    sal_mutex_give(quicksort_stack[unit].mtx);
}

/*!
 * \brief Sort field IDs.
 *
 * This function sorts the fid (field ID) array. However, the elements in
 * the fid array can't move. Therefore, this function creates a reference
 * array where each element in the reference array contains an index into
 * the fid array. The sorting will be done by moving the indexes in the
 * reference array. This function allocates the reference array
 * (\c sorted_fid), if needed, initialize it to the identity values and
 * call \ref quicksort().
 *
 * \param [in] t_info This is the table info
 * \param [in] fid This is the field ID list to sort.
 * \param [in] count This is the number of elements in the \c fid array.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int fid_sort(bcmimm_tbl_info_t *t_info, uint32_t *fid, size_t count)
{
    uint32_t j;

    /* Make sure the buffer for the sorted fid is sufficiently large */
    if (t_info->fid_array_size < count) {
        t_info->sorted_fid =
            shr_util_realloc(t_info->sorted_fid,
                             t_info->fid_array_size * BCMIMM_BE_FID_SIZE,
                             count * BCMIMM_BE_FID_SIZE,
                             "bcmimmBeSrtFid");
        if (t_info->sorted_fid == NULL) {
            return SHR_E_MEMORY;
        }
        t_info->fid_array_size = count;
    }

    for (j = 0; j < (uint32_t)count; j++) {
        t_info->sorted_fid[j] = j;
    }
    quicksort(t_info->unit, t_info->sorted_fid, fid, 0, (int)(count - 1));
    return SHR_E_NONE;
}

/*!
 * \brief Delete a field.
 *
 * This function deletes a field from the field linked list of a particular
 * entry. The fields, in the linked list, are sorted based on their fid value.
 *
 * \param [in] t_info This is the table info
 * \param [in] in This is the input field list.
 * \param [in] in_idx This is the index of the relevant field in the \c in
 * field list.
 * \param [out] ent_ptr This is the HA pointer to the last field that was
 * inserted/modified or prior to deleted. It initially starts with the first
 * field in the link list of the entry.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int field_del(bcmimm_tbl_info_t *t_info,
                     const bcmimm_be_fields_upd_t *in,
                     uint32_t in_idx,
                     uint32_t **ent_ptr)
{
    shr_ha_sub_id sub;
    element_list_head_t *blk;
    uint32_t offset;
    uint8_t *elem_section;
    uint8_t *field = NULL;
    bcmimm_free_list_t *f_list = t_info->field_free_list;
    uint32_t elem_size = f_list->data_len;
    uint32_t ptr_offset = elem_size - BCMIMM_ELEM_PTR_SIZE;
    uint32_t *next_ptr = NULL;
    uint32_t fid;
    uint32_t ptr_save;

    /*
     * Traverse the field list (sorted list based on fid), until finding
     * field ID that is equal to the input field ID. Return error if not
     * found.
     */
    while (**ent_ptr) {
        /* Obtain the field from the pointer (ent_ptr) */
        sub = BCMIMM_SUB_GET(**ent_ptr);
        blk = fix_element_blk[t_info->unit][sub];
        offset = BCMIMM_OFFSET_GET(**ent_ptr);
        assert(offset < blk->elem_count);
        assert(sub == blk->sub);
        elem_section = ((uint8_t *)blk) + sizeof(element_list_head_t);
        field = elem_section + elem_size * offset;

        fid = *((uint32_t *)field);   /* The fields are 4 bytes aligned */
        if (fid > in->fid[in_idx]) {
            return SHR_E_PARAM;       /* Field not found. */
        } else if (fid == in->fid[in_idx]) {
            break;
        }
        /* Update the pointer to the next field. */
        *ent_ptr = (uint32_t *)(field + ptr_offset);
    }

    if ((**ent_ptr) == 0) {
        return SHR_E_PARAM;  /* Here if we parsed all the field list */
    }
    /*
     * We break before updating the pointer ent_ptr so ent_ptr
     * actually points to the element we want to delete.
    */
    next_ptr = (uint32_t *)(field + ptr_offset);
    ptr_save = **ent_ptr;
    **ent_ptr = *next_ptr;
    *next_ptr = 0;  /* Null terminate */
    bcmimm_free_list_elem_put(f_list, ptr_save);
    return SHR_E_NONE;
}

/*!
 * \brief Modify field.
 *
 * This function inserts a new field or modified an existing field with a new
 * value. The fields, in the linked list, are sorted based on their fid value.
 *
 * \param [in] t_info This is the table info
 * \param [in] in This is the input field list.
 * \param [in] in_idx This is the index of the relevant field in the \c in
 * field list.
 * \param [out] ent_ptr This is the HA pointer to the last field that was
 * inserted or modified. It initially starts with the first field in the
 * link list of the entry.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int field_mod(bcmimm_tbl_info_t *t_info,
                     const bcmimm_be_fields_upd_t *in,
                     uint32_t in_idx,
                     uint32_t **ent_ptr)
{
    shr_ha_sub_id sub;
    element_list_head_t *blk;
    uint32_t offset;
    uint8_t *elem_section;
    uint8_t *field = NULL;
    bcmimm_free_list_t *f_list = t_info->field_free_list;
    uint32_t data_size = t_info->tbl->data_len;
    uint32_t elem_size = f_list->data_len;
    uint32_t ptr_offset = elem_size - BCMIMM_ELEM_PTR_SIZE;
    uint32_t fid;
    bool insert = true;

    /*
     * Traverse the field list (sorted list based on fid), until finding
     * field ID that is equal or larger than the input field.
     */
    while (**ent_ptr) {
        offset = BCMIMM_OFFSET_GET(**ent_ptr);
        sub = BCMIMM_SUB_GET(**ent_ptr);
        blk = fix_element_blk[t_info->unit][sub];
        assert(sub == blk->sub);
        assert(offset < blk->elem_count);
        elem_section = ((uint8_t *)blk) + sizeof(element_list_head_t);
        field = elem_section + elem_size * offset;
        fid = *((uint32_t *)field);   /* The fields are 4 bytes aligned */
        if (fid > in->fid[in_idx]) {
            break;
        } else if (fid == in->fid[in_idx]) {
            insert = false;  /* Need to update not insert */
            break;
        }

        *ent_ptr = (uint32_t *)(field + ptr_offset);
    }

    if (insert) {
        uint32_t new_ha_ptr = 0;
        uint32_t *ptr;
        uint8_t *new_fld;
        uint32_t *fld_id;

        new_fld = bcmimm_free_list_elem_get(t_info->unit, f_list, &new_ha_ptr);
        if (new_fld == NULL) {
            return SHR_E_MEMORY;
        }
        /* Get the location of the pointer in the new field */
        ptr = (uint32_t *)(new_fld + ptr_offset);

        /* Update the value of this pointer to the value of the prev->next */
        *ptr = **ent_ptr;
        **ent_ptr = new_ha_ptr;
        /* Progress the entry pointer to point the new element */
        *ent_ptr = ptr;
        /* Now set the field with ID and data */
        fld_id = in->fid + in_idx;
        *((uint32_t *)new_fld) = *fld_id;
        new_fld += BCMIMM_BE_FID_SIZE;
        sal_memcpy(new_fld, ((uint8_t *)in->fdata) + data_size * in_idx, data_size);
    } else {
        *ent_ptr = (uint32_t *)(field + ptr_offset);
        field += BCMIMM_BE_FID_SIZE;
        sal_memcpy(field, ((uint8_t *)in->fdata) + data_size * in_idx, data_size);
    }
    return SHR_E_NONE;
}

/*!
 * \brief Binary search of table key in a sorted buffer of keys.
 *
 * This function searches for the index of a given key in a given buffer. Since
 * the keys are sorted, the search is binary search.
 *
 * \param [in] buf The buffer containing the keys.
 * \param [in] buf_size The number of key elements in the buffer.
 * \param [in] key The key to search for.
 * \param [in] key_len The length of every key in the buffer.
 *
 * \return The index of the search key when found or the buffer_size + 1 for
 * failure to find the key.
 */
static uint32_t key_bin_search(uint8_t *buf,
                               uint32_t buf_size,
                               uint8_t *key,
                               uint32_t key_len)
{
    int start = 0;
    int end = (int)buf_size - 1;
    int idx;
    uint8_t *tmp_key;
    int comp;
    uint64_t key_val, tmp_key_val;
    uint64_t mask = 0;

    if (key_len <= sizeof(uint64_t)) {
        if (key_len == sizeof(uint64_t)) {
            mask = (uint64_t)(-1);
        } else {
            mask = ((uint64_t)1 << (key_len * 8)) - 1;
        }
        key_val = *(uint64_t *)key;
        key_val &= mask;
    }
    while (start <= end) {
        idx = (start + end) / 2;
        tmp_key = buf + idx * key_len;
        if (key_len <= sizeof(uint64_t)) {
            tmp_key_val = *(uint64_t *)tmp_key;
            tmp_key_val &= mask;
            if (key_val < tmp_key_val) {
                end = idx - 1;
            } else if (key_val == tmp_key_val) {
                return idx;
            } else {
                start = idx + 1;
            }
        } else { /* For catch all but probably will not be used */
            comp = sal_memcmp(key, tmp_key, key_len);
            if (comp < 0) {
                end = idx - 1;
            } else if (comp > 0) {
                start = idx + 1;
            } else {
                return idx;
            }
        }
    }
    return buf_size + 1;
}

/*!
 * \brief Quick sort compare function.
 *
 * This function helps the quick sort function to compare two elements. Since
 * the key size is variable the function uses the module global value
 * \c sort_key_len to determine the key length.
 *
 * \param [in] key1 Element 1.
 * \param [in] key2 Element 2.
 *
 * \return <0 if key1 is smaller then key2.
 * \return 0 if key1 is equals to key2.
 * \return >0 if key1 is larger then key2.
 */
static int key_comp(const void *key1, const void *key2)
{
    if (sort_key_len <= sizeof(uint64_t)) {
        uint64_t val1, val2, mask;
        if (sort_key_len == sizeof(uint64_t)) {
            mask = (uint64_t)(-1);
        } else {
            mask = ((uint64_t)1 << (8 * sort_key_len)) - 1;
        }
        val1 = *(uint64_t *)key1;
        val2 = *(uint64_t *)key2;
        val1 &= mask;
        val2 &= mask;
        if (val1 < val2) {
            return -1;
        } else if (val1 > val2) {
            return 1;
        } else {
            return 0;
        }
    } else {
        return sal_memcmp(key1,  key2,  sort_key_len);
    }
}

/*******************************************************************************
 * External functions (private for this component)
 */
element_list_head_t *bcmimm_get_ha_blk(int unit, shr_ha_sub_id sub)
{
    return fix_element_blk[unit][sub];
}

int bcmimm_free_lists_retrieve(int unit, shr_ha_sub_id sub)
{
    element_list_head_t *f_blk;
    uint32_t len = ELEM_CHUNK_SIZE * 4; /* Minimal size */

    SHR_FUNC_ENTER(unit);
    sal_memset(fix_element_blk[unit],
               0,
               sizeof(fix_element_blk) / BCMDRD_CONFIG_MAX_UNITS);

    do {
        f_blk = shr_ha_mem_alloc(unit,
                                 bcmimm_be_comp_id,
                                 sub,
                                 NULL,
                                 &len);
        SHR_NULL_CHECK(f_blk, SHR_E_MEMORY);

        if ((f_blk->signature != BCMIMM_BLK_SIGNATURE) || (f_blk->sub != sub)){
            /*
             * This block was not previously allocated so there is nothing to
             * retrieve. Free the block and break.
             */
            shr_ha_mem_free(unit, f_blk);
            break;
        }
        /* New field will have value = 0 */
        if (f_blk->elem_count == 0) {
            f_blk->elem_count =
                (len - sizeof(element_list_head_t)) / f_blk->data_len;
        }

        SHR_IF_ERR_EXIT(chain_blk_add(unit, f_blk, &bcmimm_free_elem[unit]));

        sub = BCMIMM_SUB_GET(f_blk->next_size_blk);
        if (f_blk->sub >= bcmimm_available_subs[unit]) {
            bcmimm_available_subs[unit] = f_blk->sub + 1;
        }
    } while (f_blk->next_size_blk);
exit:
    SHR_FUNC_EXIT();
}

/*
 * This function search for the free list that matches the desired size. If
 * nothing is found the function will create a free list for the new element
 * size.
 */
bcmimm_free_list_t *bcmimm_free_list_find(int unit,
                                         size_t size,
                                         bcmimm_free_list_t **list)
{
    bcmimm_free_list_t *f_list = *list;
    bcmimm_free_list_t *rv = NULL;

    /* Search the list for suitable list */
    while (f_list) {
        if (f_list->data_len == size) {    /* Perfect match */
            rv = f_list;
            break;
        }
        f_list = f_list->next;
    }

    /* If not found any suitable list allocate a new one */
    if (rv == NULL) {
        rv = new_elem_size_alloc(unit, size, list);
    }

    return rv;
}

uint8_t* bcmimm_free_list_elem_get(int unit,
                            bcmimm_free_list_t *elem_fr_lst,
                            uint32_t *ptr)
{
    uint8_t *ent;
    element_list_head_t *blk;
    shr_ha_sub_id sub;
    uint32_t offset;
    uint32_t *new_ptr;
    uint8_t *elem_section;
    int j;

    sal_mutex_take(elem_fr_lst->lock, SAL_MUTEX_FOREVER);

    if (BCMIMM_IS_NULL_PTR(elem_fr_lst->master_blk->free_list)) {
        bool found_prev = false;
        /* Find the last block on the chain. */
        for (j = bcmimm_available_subs[unit]; j >= 0; j--) {
            blk = fix_element_blk[unit][j];
            if (blk && (blk->data_len == elem_fr_lst->data_len) &&
                (blk->blk_ext == 0)) {
                found_prev = true;
                break;
            }
        }
        if (!found_prev || new_elem_blk_add(unit, blk, elem_fr_lst) != 0) {
            sal_mutex_give(elem_fr_lst->lock);
            return NULL;
        }
    }
    /* Get the first element from the list */
    BCMIMM_ASSIGN_PTR(*ptr, elem_fr_lst->master_blk->free_list);
    sub = BCMIMM_SUB_GET(*ptr);
    blk = fix_element_blk[unit][sub];
    offset = BCMIMM_OFFSET_GET(*ptr);
    assert(offset < blk->elem_count);
    elem_section = ((uint8_t *)blk) + sizeof(element_list_head_t);
    ent = elem_section + offset * elem_fr_lst->data_len;
    /* Need to update the free list pointer */
    new_ptr = BCMIMM_ENTRY_PTR_GET(ent, elem_fr_lst->data_len);
    BCMIMM_ASSIGN_PTR(elem_fr_lst->master_blk->free_list, *new_ptr);

    sal_mutex_give(elem_fr_lst->lock);
    return ent;
}

void bcmimm_free_list_elem_put(bcmimm_free_list_t *elem_lst, uint32_t ent_ptr)
{
    uint32_t *ptr = &ent_ptr;
    uint8_t *fld;
    shr_ha_sub_id sub;
    uint32_t offset;
    size_t fld_size = elem_lst->data_len;
    size_t ptr_offset = fld_size - BCMIMM_ELEM_PTR_SIZE;
    element_list_head_t *blk;

    if (BCMIMM_IS_NULL_PTR(ent_ptr)) {
        return;
    }

    /* Find the last element of the list */
    do {
        sub = BCMIMM_SUB_GET(*ptr);
        blk = fix_element_blk[elem_lst->unit][sub];
        offset = BCMIMM_OFFSET_GET(*ptr);
        assert(offset < blk->elem_count);
        fld = ((uint8_t *)blk) + sizeof(element_list_head_t) + offset * fld_size;
        ptr = (uint32_t *)(fld + ptr_offset);
    } while (!BCMIMM_IS_NULL_PTR(*ptr));

    sal_mutex_take(elem_lst->lock, SAL_MUTEX_FOREVER);
    *ptr = elem_lst->master_blk->free_list;
    BCMIMM_ASSIGN_PTR(elem_lst->master_blk->free_list, ent_ptr);
    sal_mutex_give(elem_lst->lock);
}

uint8_t *bcmimm_entry_get (bcmimm_free_list_t *elem_lst, uint32_t ptr)
{
    element_list_head_t *blk;
    shr_ha_sub_id sub;
    uint32_t offset;

    sub = BCMIMM_SUB_GET(ptr);
    blk = fix_element_blk[elem_lst->unit][sub];
    offset = BCMIMM_OFFSET_GET(ptr);
    assert(offset < blk->elem_count);
    return ((uint8_t *)blk) + sizeof(element_list_head_t) +
            offset * elem_lst->data_len;
}


int bcmimm_data_field_insert(bcmimm_tbl_info_t *t_info,
                             uint32_t *ent_ptr,
                             const bcmimm_be_fields_t *in)
{
    uint8_t *field;
    uint8_t *prev_field = NULL;
    uint32_t data_size = t_info->tbl->data_len;
    uint32_t ha_ptr = 0;
    uint32_t *prev_ptr;
    uint8_t *curr_data;
    uint32_t ptr_offset =
              t_info->field_free_list->data_len - BCMIMM_ELEM_PTR_SIZE;
    uint32_t *fid;
    size_t count;
    uint32_t idx;

    SHR_FUNC_ENTER(t_info->unit);

    if (in->count == 0) {
        *ent_ptr = 0;
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(fid_sort(t_info, in->fid, in->count));

    for (count = 0; count < in->count; count++) {
        field = bcmimm_free_list_elem_get(t_info->unit,
                                   t_info->field_free_list,
                                   &ha_ptr);
        SHR_NULL_CHECK(field, SHR_E_MEMORY);
        if (prev_field == NULL) {
            *ent_ptr = ha_ptr;
        } else {
            prev_ptr = (uint32_t *)(prev_field + ptr_offset);
            *prev_ptr = ha_ptr;
        }

        prev_field = field;
        idx = t_info->sorted_fid[count];
        fid = in->fid + idx;
        *((uint32_t *)field) = *fid;
        field += BCMIMM_BE_FID_SIZE;
        curr_data = ((uint8_t *)in->fdata) + idx * data_size;
        sal_memcpy(field, curr_data, data_size);
    }

    /* NULL terminate the field list */
    prev_ptr = (uint32_t *)(prev_field + ptr_offset);
    *prev_ptr = 0;
exit:
    SHR_FUNC_EXIT();
}

int bcmimm_data_field_get(bcmimm_tbl_info_t *t_info,
                          uint32_t ent_ptr,
                          bcmimm_be_fields_t *out)
{
    uint8_t *field;
    uint32_t data_size = t_info->tbl->data_len;
    uint8_t *curr_data = (uint8_t *)out->fdata;
    size_t num_of_field = 0;
    element_list_head_t *blk;
    uint32_t offset;
    bcmimm_free_list_t *f_list = t_info->field_free_list;
    uint32_t elem_size = f_list->data_len;
    uint32_t ptr_offset = elem_size - BCMIMM_ELEM_PTR_SIZE;
    uint8_t *elem_section;
    uint32_t *ptr;
    shr_ha_sub_id sub;
    uint32_t *fid = out->fid;

    SHR_FUNC_ENTER(t_info->unit);
    while (ent_ptr) {
        sub = BCMIMM_SUB_GET(ent_ptr);
        blk = fix_element_blk[t_info->unit][sub];
        offset = BCMIMM_OFFSET_GET(ent_ptr);
        assert(offset < blk->elem_count);
        assert(sub == blk->sub);
        elem_section = ((uint8_t *)blk) + sizeof(element_list_head_t);
        field = elem_section + elem_size * offset;
        ptr = (uint32_t *)(field + ptr_offset);
        assert(ent_ptr != *ptr);
        ent_ptr = *ptr;

        if (out->count == 0) {
            if (ent_ptr) {
                out->count = num_of_field;
                SHR_ERR_EXIT(SHR_E_FULL);
            } else {
                break;
            }
        }

        *fid = *((uint32_t *)field);   /* The fields are 4 bytes aligned */
        fid++;
        field += BCMIMM_BE_FID_SIZE;
        sal_memcpy(curr_data, field, data_size);
        curr_data += data_size;
        out->count--;

        num_of_field++;
    }
    out->count = num_of_field;

exit:
    SHR_FUNC_EXIT();
}

int bcmimm_data_field_update(bcmimm_tbl_info_t *t_info,
                             uint32_t *ent_ptr,
                             const bcmimm_be_fields_upd_t *in)
{
    size_t count = 0;
    uint32_t in_idx;

    SHR_FUNC_ENTER(t_info->unit);
    SHR_IF_ERR_EXIT(fid_sort(t_info, in->fid, in->count));

    while (ent_ptr && (count < in->count)) {
        in_idx = t_info->sorted_fid[count];
        if (in->del[in_idx]) {
            /*
             * It is not an error to unset a field that was
             * not explicitly set by previous insert or update previously.
             */
            SHR_IF_ERR_EXIT_EXCEPT_IF(
                field_del(t_info, in, in_idx, &ent_ptr), SHR_E_PARAM);
        } else {
            SHR_IF_ERR_EXIT(
                field_mod(t_info, in, in_idx, &ent_ptr));
        }
        count++;
    }
exit:
    SHR_FUNC_EXIT();
}

int bcmimm_field_id_overwrite(bcmimm_tbl_info_t *t_info,
                              uint32_t *ent_ptr,
                              bcmimm_be_fields_t *in)
{
    shr_ha_sub_id sub;
    element_list_head_t *blk;
    uint32_t offset;
    uint8_t *elem_section;
    uint8_t *field = NULL;
    uint32_t ent_ptr_val = *ent_ptr;
    size_t count;
    bcmimm_free_list_t *f_list = t_info->field_free_list;
    uint32_t elem_size = f_list->data_len;
    uint32_t ptr_offset = elem_size - BCMIMM_ELEM_PTR_SIZE;

    SHR_FUNC_ENTER(t_info->unit);

    if (in->count == 0) {
        SHR_EXIT();
    }

    /*
     * The assumption is that the order of the fields in the input 'in'
     * matches the fields link list.
     */
    for (count = 0; count < in->count; count++) {
        offset = BCMIMM_OFFSET_GET(ent_ptr_val);
        sub = BCMIMM_SUB_GET(ent_ptr_val);
        blk = fix_element_blk[t_info->unit][sub];
        assert(sub == blk->sub);
        assert(offset < blk->elem_count);
        elem_section = ((uint8_t *)blk) + sizeof(element_list_head_t);
        field = elem_section + elem_size * offset;
        ent_ptr_val = *(uint32_t *)(field + ptr_offset);
        if (in->fid[count] == BCMLTD_SID_INVALID) {
            /* The field does no longer exist */
            *(uint32_t *)(field + ptr_offset) = 0; /* NULL terminate */
            /*
             * ent_ptr points the previous ptr, the value is the address of
             * the variable 'field' which we want to free.
             */
            bcmimm_free_list_elem_put(f_list, *ent_ptr);
            /* Set the previous pointer value to point to the next element */
            *ent_ptr = ent_ptr_val;
        } else {
            *((uint32_t *)field) = in->fid[count];/* Set the new field value */
            ent_ptr = (uint32_t *)(field + ptr_offset);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

uint8_t *bcmimm_entry_find(bcmimm_tbl_info_t *t_info,
                           const void *key,
                           uint32_t **ent_ptr,
                           uint8_t **prev_ent)
{
    uint32_t idx;
    uint8_t *entry;
    bool found;
    size_t ptr_offset = t_info->tbl->ent_len - BCMIMM_ELEM_PTR_SIZE;

    idx = shr_hash_elf(key, t_info->tbl->key_len) % t_info->tbl->num_of_rows;
    entry = (uint8_t *)t_info->tbl;
    entry += sizeof(tbl_header_t);
    entry += idx * t_info->tbl->ent_len;
    *ent_ptr = BCMIMM_ENTRY_PTR_GET(entry, t_info->tbl->ent_len);
    if (!BCMIMM_IS_OCCUPIED(**ent_ptr)) {
        return NULL;
    }

    found = false;
    if (prev_ent) {
        *prev_ent = NULL;
    }

    do {
        if (sal_memcmp(key, entry, t_info->tbl->key_len) == 0) {
            found = true;
            break;
        }
        if (BCMIMM_IS_NULL_PTR(**ent_ptr)) {
            break;
        }
        if (prev_ent) {
            *prev_ent = entry;
        }
        entry = bcmimm_entry_get (t_info->entry_free_list, **ent_ptr);
        *ent_ptr = (uint32_t *)(entry + ptr_offset);
    } while (true);

    if (!found) {
        return NULL;
    } else {
        return entry;
    }
}

void bcmimm_table_content_clear(bcmimm_tbl_info_t *t_info)
{
    uint32_t j;
    uint8_t *base;
    uint8_t *ent;
    uint32_t *ptr;
    uint32_t ptr_val;
    uint32_t prev_ptr;
    size_t ptr_offset = t_info->tbl->ent_len - BCMIMM_ELEM_PTR_SIZE;

    /* Free all table entries */
    base = ((uint8_t *)t_info->tbl) + sizeof(tbl_header_t);
    ent = base;
    ptr = (uint32_t *)(ent + ptr_offset);
    ptr_val = *ptr;

    for (j = 0; j < t_info->tbl->num_of_rows; j++) {
        if (BCMIMM_IS_OCCUPIED(ptr_val)) {
            bcmimm_free_list_elem_put(t_info->field_free_list, *(ptr - 1));
            /* Clear the pointer value */
            *(ptr - 1) = 0;
            while (!BCMIMM_IS_NULL_PTR(ptr_val)) {
                ent = bcmimm_entry_get (t_info->entry_free_list, ptr_val);
                prev_ptr = ptr_val; /* prev_ptr pointing to the new entry */
                /* Pointing to the next entry */
                ptr = (uint32_t *)(ent + ptr_offset);
                ptr_val = *ptr;
                /* Free the fields of the entry */
                bcmimm_free_list_elem_put(t_info->field_free_list, *(ptr - 1));
                /* Clear the pointer value */
                *(ptr - 1) = 0;

                /* Free the entry */
                *ptr = 0;   /* Make sure it will only free this entry */
                bcmimm_free_list_elem_put(t_info->entry_free_list, prev_ptr);
            }
        }

        ent = base + (j + 1) * t_info->tbl->ent_len;
        ptr = (uint32_t *)(ent + ptr_offset);
        if (j < t_info->tbl->num_of_rows - 1) {
            ptr_val = *ptr;
        }
    }

    t_info->tbl->num_of_ent = 0;    /* Table has no entries */
}

uint8_t *bcmimm_next_entry_find(bcmimm_tbl_info_t *t_info,
                                bool first,
                                uint8_t *in_key)
{
    uint32_t *ptr;
    uint8_t *ent;
    uint32_t j;
    uint8_t *key;
    uint32_t key_len = t_info->tbl->key_len;

    if (first) {
        j = 0;
        key = t_info->traverse_buf;
        do {
            ent = bcmimm_entry_find(t_info, key, &ptr, NULL);
            j++;
            key += key_len;
        } while (!ent && j < t_info->trav_buf_size);
        return ent;
    }

    j = key_bin_search(t_info->traverse_buf,
                       t_info->trav_buf_size,
                       in_key,
                       key_len);
    if (j >= t_info->trav_buf_size - 1) {
        return NULL;
    }
    j++;
    key = t_info->traverse_buf + j * key_len;
    do {
        ent = bcmimm_entry_find(t_info, key, &ptr, NULL);
        j++;
        key += key_len;
    } while (!ent && j < t_info->trav_buf_size);
    return ent;
}

uint8_t *bcmimm_next_entry_raw_find(bcmimm_tbl_info_t *t_info,
                                    bool first,
                                    uint32_t start_idx,
                                    uint32_t *start_ptr)
{
    uint8_t *ent;
    uint32_t *ptr;
    uint8_t *base;
    uint32_t j;
    element_list_head_t *blk;
    shr_ha_sub_id sub;
    uint32_t offset;
    size_t ptr_offset = t_info->tbl->ent_len - BCMIMM_ELEM_PTR_SIZE;

    base = ((uint8_t *)t_info->tbl) + sizeof(tbl_header_t);

    if (BCMIMM_IS_NULL_PTR(*start_ptr)) {
        /*
         * If the pointer is NULL it means that there is nothing more to do
         * in this slot, and we are therefore incrementing the index (slot).
         * However, the function may be called for the first time with
         * index = 0. So it is possible that in index 0 there is only one entry
         * and we don't want to skip over it. So if index is 0 take the first
         * entry. If there is something in this entry the start index will be
         * incremented at the loop below.
         */
        if (!first) {
            start_idx++;
        }
        /*
         * This loop simply goes through the entry array (bins) and look for
         * occupied entry
         */
        for (j = start_idx; j < t_info->tbl->num_of_rows; j++) {
            ent = base + t_info->tbl->ent_len * j;
            ptr = (uint32_t *)(ent + ptr_offset);
            if (BCMIMM_IS_OCCUPIED(*ptr)) {
                *start_ptr  = *ptr;
                return ent;
            }
        }
        return NULL;
    } else {
        /*
         * Here we continue in the same bin (start index) and looking at the
         * next entry pointed by the current one (since the pointer is
         * not NULL).
         */
        sub = BCMIMM_SUB_GET(*start_ptr);
        blk = fix_element_blk[t_info->unit][sub];
        offset = BCMIMM_OFFSET_GET(*start_ptr);
        assert(offset < blk->elem_count);
        ent = ((uint8_t *)blk) + sizeof(element_list_head_t);
        ent += offset * t_info->entry_free_list->data_len;
        ptr = (uint32_t *)(ent + ptr_offset);
        *start_ptr = *ptr;
        return ent;
    }
}

int bcmimm_prep_traverse_buf(bcmimm_tbl_info_t *t_info)
{
    uint32_t j = t_info->tbl->num_of_ent;
    uint8_t *key;
    uint32_t idx;
    uint8_t *base;
    uint8_t *ent;
    uint32_t *ptr;
    element_list_head_t *blk;
    shr_ha_sub_id sub;
    uint32_t offset;
    uint32_t key_len = t_info->tbl->key_len;
    size_t ptr_offset = t_info->tbl->ent_len - BCMIMM_ELEM_PTR_SIZE;

    if (j == 0) {
        return SHR_E_NOT_FOUND;
    }
    t_info->traverse_buf = sal_alloc(key_len * j, "bcmImmBeTraverse");
    if (!t_info->traverse_buf) {
        return SHR_E_MEMORY;
    }
    t_info->trav_buf_size = j;

    base = ((uint8_t *)t_info->tbl) + sizeof(tbl_header_t);
    /*
     * Traverse the table and copy the keys of each found entry into the
     * traverse buffer.
     */
    key = t_info->traverse_buf;
    for (idx = 0; idx < t_info->tbl->num_of_rows && j > 0; idx++) {
        ent = base + t_info->tbl->ent_len * idx;
        ptr = (uint32_t *)(ent + ptr_offset);
        while (BCMIMM_IS_OCCUPIED(*ptr)) {
            sal_memcpy(key, ent, key_len);
            key += key_len;
            if (j == 0) {
                break;
            }
            j--;
            if (BCMIMM_IS_NULL_PTR(*ptr)) {
                break;
            }
            sub = BCMIMM_SUB_GET(*ptr);
            blk = fix_element_blk[t_info->unit][sub];
            offset = BCMIMM_OFFSET_GET(*ptr);
            assert(offset < blk->elem_count);
            ent = ((uint8_t *)blk) + sizeof(element_list_head_t);
            ent += offset * t_info->entry_free_list->data_len;
            ptr = (uint32_t *)(ent + ptr_offset);
        }
    }

    /* Now sort the keys */
    sal_mutex_take(sort_mutex,  SAL_MUTEX_FOREVER);
    sort_key_len = key_len;
    sal_qsort(t_info->traverse_buf, t_info->tbl->num_of_ent, key_len, key_comp);
    sal_mutex_give(sort_mutex);

    return SHR_E_NONE;
}

/*
 * The sort mutex is global for all units.
 */
int bcmimm_be_generic_init(void)
{
    sort_mutex = sal_mutex_create("bcmImmBeTrvrsMtx");
    if (sort_mutex == NULL) {
        return SHR_E_MEMORY;
    }
    return SHR_E_NONE;
}

void bcmimm_be_generic_shutdown(void)
{
    if (sort_mutex) {
        sal_mutex_destroy(sort_mutex);
        sort_mutex = NULL;
    }
}

int bcmimm_be_unit_init(int unit)
{
    quicksort_stack[unit].mtx = sal_mutex_create("bcmImmBeQS");
    if (quicksort_stack[unit].mtx == NULL) {
        return SHR_E_MEMORY;
    }
    return SHR_E_NONE;
}

void bcmimm_be_unit_shutdown(int unit)
{
    if (quicksort_stack[unit].mtx) {
        sal_mutex_destroy(quicksort_stack[unit].mtx);
        quicksort_stack[unit].mtx = NULL;
    }

    if (quicksort_stack[unit].blk) {
        SHR_FREE(quicksort_stack[unit].blk);
        quicksort_stack[unit].blk_size = 0;
    }
}

