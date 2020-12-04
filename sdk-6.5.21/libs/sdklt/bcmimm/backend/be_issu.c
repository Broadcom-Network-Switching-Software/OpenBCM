/*! \file be_issu.c
 *
 * In-Memory back-end ISSU related implementation.
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

/*******************************************************************************
 * Private functions
 */
/*!
 * Finds the free list element associated with a particular sub ID.
 *
 * This function obtains the free block associated with the sub ID. It then
 * searches in the free element list for a list with the same data size
 * as the block data size.
 *
 * \param [in] unit The unit associated with the search.
 * \param [in] sub The search criteria.
 * \param [out] list The free element list associated with the same block size.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_INTERNAL Failure.
 */
static int find_blk_d_len(int unit,
                          shr_ha_sub_id sub,
                          bcmimm_free_list_t **list)
{
    element_list_head_t *blk = bcmimm_get_ha_blk(unit, sub);
    bcmimm_free_list_t *f_list = bcmimm_free_elem[unit];
    uint32_t data_len;

    if (!blk) {
        return SHR_E_INTERNAL;
    }
    data_len = blk->data_len;
    while (f_list) {
        if (f_list->data_len == data_len) {
            *list = f_list;
            return SHR_E_NONE;
        }
        f_list = f_list->next;
    }
    return SHR_E_INTERNAL;
}

/*!
 * \brief Replace every field within an entry with similar field of different
 * size.
 *
 * This function replicates the values maintained in an entry with a fields
 * of equal values but different size. The old list is then being freed.
 *
 * \param [in] unit The unit associated with the entry.
 * \param [in] t_info Table info of the table associated with the entry.
 * \param [in] old_f_list The old free list where the current fields were
 * taken. This parameter is required to returning the old field list.
 * \param [in] big_endian Indicating if the integers are big or little endian.
 * \param [out] ptr Is the address of the entry field pointer. This value is
 * being replaced by the address of the first element in the new list.
 *
 * \return SHR_E_NONE Success
 * \return SHR_E_MEMORY Failure to allocate new field.
 */
static int update_entry_fields_size(int unit,
                                    bcmimm_tbl_info_t *t_info,
                                    bcmimm_free_list_t *old_f_list,
                                    bool big_endian,
                                    uint32_t *ptr)
{
    uint32_t field_len = old_f_list->data_len;
    size_t ptr_offset = field_len - BCMIMM_ELEM_PTR_SIZE;
    uint32_t field_ptr = *ptr;
    uint32_t new_fld_ptr = 0;
    shr_ha_sub_id sub;
    element_list_head_t *blk;
    uint32_t offset;
    uint8_t *src_elem_section;
    uint8_t *src_field;
    uint8_t *new_field;
    size_t ptr_new_offset =
            t_info->field_free_list->data_len - BCMIMM_ELEM_PTR_SIZE;
    bool first_field = true;
    uint32_t *new_ptr = NULL;
    uint32_t field_list = *ptr;

    SHR_FUNC_ENTER(unit);
    while (field_ptr) {
        /* Obtain the source data entry */
        sub = BCMIMM_SUB_GET(field_ptr);
        blk = bcmimm_get_ha_blk(unit, sub);
        offset = BCMIMM_OFFSET_GET(field_ptr);
        assert(offset < blk->elem_count);
        assert(sub == blk->sub);
        src_elem_section = ((uint8_t *)blk) + sizeof(element_list_head_t);
        src_field = src_elem_section + field_len * offset;
        field_ptr = *(uint32_t *)(src_field + ptr_offset);

        /* Allocate the new entry */
        new_field = bcmimm_free_list_elem_get(t_info->unit,
                                              t_info->field_free_list,
                                              &new_fld_ptr);
        SHR_NULL_CHECK(new_field, SHR_E_MEMORY);
        if (first_field) {
            *ptr = new_fld_ptr;
            first_field = false;
        } else {
            /* Set the previous pointer to point to the new field */
            *new_ptr = new_fld_ptr;
        }
        new_ptr = (uint32_t *)(new_field + ptr_new_offset);

        /* Copy the FID and data portion */
        *(uint32_t *)new_field = *(uint32_t *)src_field;
        src_field += BCMIMM_BE_FID_SIZE;
        new_field += BCMIMM_BE_FID_SIZE;
        if (big_endian) {
            new_field += t_info->field_free_list->data_len - field_len;
        }
        sal_memcpy(new_field, src_field, t_info->tbl->data_len);
    }
    /* Null terminate the new list */
    *new_ptr = 0;

    /* Free the old field list */
    bcmimm_free_list_elem_put(old_f_list, field_list);

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Caches all the entries content (excluding the fields) into a buffer.
 *
 * This function go through all the entries within the table and caches each
 * entry content (excluding the actual entry fields) into a provided buffer.
 * Entries that were dynamically allocated will be freed back to their
 * corresponding field list.
 *
 * \param [in] t_info Table info.
 * \param [out] buff The memory to copy the content of each entry.
 */
static int cache_current_entries(bcmimm_tbl_info_t *t_info,
                                   uint8_t *buff)
{
    uint8_t *base;
    uint32_t j;
    tbl_header_t *tbl = t_info->tbl;
    uint32_t field_cnt = 0;
    uint8_t *ent;
    uint32_t *ptr;
    element_list_head_t *blk;
    shr_ha_sub_id sub;
    uint8_t *ent_dest;
    uint32_t offset;
    size_t ptr_offset = tbl->ent_len - BCMIMM_ELEM_PTR_SIZE;
    int unit = t_info->unit;
    uint32_t ptr_val;
    uint32_t prev_ptr_val;
    bool free_entry;
    bcmimm_free_list_t *f_list = bcmimm_free_elem[unit];

    /* Find the old entry list */
    while (f_list) {
        if (f_list->data_len == tbl->ent_len) {
            break;
        }
        f_list = f_list->next;
    }
    if (!f_list) {
        return SHR_E_INTERNAL;
    }

    base = ((uint8_t *)tbl) + sizeof(tbl_header_t);
    for (j = 0; j < tbl->num_of_rows && field_cnt < tbl->num_of_ent; j++) {
        ent = base + tbl->ent_len * j;
        ptr = (uint32_t *)(ent + ptr_offset);
        free_entry = false;
        ptr_val = 0;
        while (BCMIMM_IS_OCCUPIED(*ptr)) {
            ent_dest = buff + field_cnt * tbl->ent_len;
            sal_memcpy(ent_dest, ent, tbl->ent_len);
            field_cnt++;
            prev_ptr_val = ptr_val;
            ptr_val = *ptr;
            if (free_entry) {
                *ptr = 0;
                bcmimm_free_list_elem_put(f_list, prev_ptr_val);
            }
            if (BCMIMM_IS_NULL_PTR(ptr_val)) {
                break;
            }
            free_entry = true;
            sub = BCMIMM_SUB_GET(ptr_val);
            blk = bcmimm_get_ha_blk(unit, sub);
            offset = BCMIMM_OFFSET_GET(ptr_val);
            assert(offset < blk->elem_count);
            ent = ((uint8_t *)blk) + sizeof(element_list_head_t);
            ent += offset * tbl->ent_len;
            ptr = (uint32_t *)(ent + ptr_offset);
        }
    }
    return SHR_E_NONE;
}

/*!
 * \brief Restore the cached entries onto the new table.
 *
 * This function restore every cached entry into a new entry within the new key
 * field length.
 *
 * \param [in] t_info Table info.
 * \param [in] buff memory where the old entries cached.
 * \param [in] key_len New key length.
 * \param [in] entry_len New entry length.
 * \param [in] big_endian Indicates if the CPU operates in big or little endian
 * mode.
 *
 * \return SHR_E_NONE Success
 * \return SHR_E_MEMORY Failed to allocate memory.
 */
static int restore_cached_entries(bcmimm_tbl_info_t *t_info,
                                  uint8_t *buff,
                                  uint32_t key_len,
                                  uint32_t entry_len,
                                  bool big_endian)
{
    tbl_header_t *tbl = t_info->tbl;
    uint32_t j;
    uint8_t *ent;
    uint32_t *ptr;
    uint32_t new_ent_ptr = 0;
    uint8_t *inp_ent = buff;
    uint32_t idx;
    uint32_t ptr_val;
    uint32_t key_delta;
    uint64_t tmp_key;

    SHR_FUNC_ENTER(t_info->unit);
    if (key_len > tbl->key_len) {
        key_delta = key_len - tbl->key_len;
    } else {
        key_delta = 0;
    }
    for (j = 0; j < tbl->num_of_ent; j++, inp_ent += tbl->ent_len) {
        tmp_key = 0;
        if (big_endian) {
            sal_memcpy(((uint8_t *)&tmp_key) + key_delta, inp_ent, tbl->key_len);
        } else {
            sal_memcpy((uint8_t *)&tmp_key, inp_ent, tbl->key_len);
        }
        /* Calculate the index using the new key length */
        idx = shr_hash_elf((uint8_t *)&tmp_key, key_len) % tbl->num_of_rows;
        ent = (uint8_t *)tbl + sizeof(tbl_header_t);
        ent += idx * entry_len;
        ptr = BCMIMM_ENTRY_PTR_GET(ent, entry_len);
        if (BCMIMM_IS_OCCUPIED(*ptr)) {
            /* Enter a new dynamic entry into the linked list */
            ent = bcmimm_free_list_elem_get(t_info->unit,
                                            t_info->entry_free_list,
                                            &new_ent_ptr);
            SHR_NULL_CHECK(ent, SHR_E_MEMORY);
            sal_memset(ent, 0, entry_len);
            /* Assigned the new entry pointer to 'ent->next' */
            ptr_val = *ptr;
            BCMIMM_OCCUPIED_SET(new_ent_ptr);
            *ptr = new_ent_ptr;
            ptr = BCMIMM_ENTRY_PTR_GET(ent, entry_len);
            *ptr = ptr_val;
        } else {
            BCMIMM_OCCUPIED_SET(*ptr);
        }
        /* So now ent points to a clean entry */
        sal_memcpy(ent, (uint8_t *)&tmp_key, key_len);

        /*
         * Copy the field ptr by obtaining its value from previous entry and
         * assigning the value onto the new entry.
         */
        ptr = BCMIMM_ENTRY_FLD_PTR_GET(inp_ent, tbl->ent_len);
        ptr_val = *ptr;
        ptr = BCMIMM_ENTRY_FLD_PTR_GET(ent, entry_len);
        *ptr = ptr_val;
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * External functions (private for this component)
 */
int bcmimm_be_update_data_len(int unit,
                              bcmimm_tbl_info_t *t_info,
                              uint32_t data_len,
                              bool big_endian)
{
    uint8_t *ent;
    uint32_t *ptr;
    uint32_t field_ptr;
    uint8_t *base;
    uint32_t j;
    uint32_t offset;
    tbl_header_t *tbl = t_info->tbl;
    size_t ptr_offset = tbl->ent_len - BCMIMM_ELEM_PTR_SIZE;
    uint32_t field_cnt = tbl->num_of_ent;
    element_list_head_t *blk;
    shr_ha_sub_id sub;
    bcmimm_free_list_t *ha_blk_list = NULL;

    SHR_FUNC_ENTER(unit);
    base = ((uint8_t *)t_info->tbl) + sizeof(tbl_header_t);
    for (j = 0; j < t_info->tbl->num_of_rows && field_cnt > 0; j++) {
        ent = base + t_info->tbl->ent_len * j;
        ptr = (uint32_t *)(ent + ptr_offset);
        while (BCMIMM_IS_OCCUPIED(*ptr)) {
            field_ptr = *(ptr-1);
            if (ha_blk_list == NULL) {
                find_blk_d_len(unit,
                               BCMIMM_SUB_GET(field_ptr),
                               &ha_blk_list);
            }
            SHR_IF_ERR_EXIT(update_entry_fields_size(unit, t_info, ha_blk_list,
                                                     big_endian, ptr-1));
            field_cnt--;
            /* Find the next entry */
            if (BCMIMM_IS_NULL_PTR(*ptr)) {
                break;
            }
            sub = BCMIMM_SUB_GET(*ptr);
            blk = bcmimm_get_ha_blk(t_info->unit, sub);
            offset = BCMIMM_OFFSET_GET(*ptr);
            assert(offset < blk->elem_count);
            ent = ((uint8_t *)blk) + sizeof(element_list_head_t);
            ent += offset * t_info->entry_free_list->data_len;
            ptr = (uint32_t *)(ent + ptr_offset);
        }
    }

exit:
    SHR_FUNC_EXIT();
}


int bcmimm_be_update_key_len(int unit,
                             bcmimm_tbl_info_t *t_info,
                             uint32_t key_len,
                             uint32_t entry_len,
                             bool big_endian)
{
    tbl_header_t *tbl = t_info->tbl;
    uint8_t *buff = NULL;
    uint32_t ha_blk_len;
    uint32_t ha_req_blk_len;
    uint8_t *ptr;

    SHR_FUNC_ENTER(unit);
    /* Step 1: Copy all the entries into temporary buffer */
    if (tbl->num_of_ent > 0) {
        buff = sal_alloc(tbl->ent_len * tbl->num_of_ent, "bcmimmBeKeyConvert");
        SHR_NULL_CHECK(buff, SHR_E_MEMORY);
        sal_memset(buff, 0, tbl->data_len * tbl->key_len);
        SHR_IF_ERR_EXIT(cache_current_entries(t_info, buff));
    }

    /*
     * Step 2: Increase the HA block size if necessary. First find the actual
     * HA block size.
     */
    ha_req_blk_len = entry_len * tbl->num_of_rows + sizeof(*tbl);
    ha_blk_len = ha_req_blk_len;
    ptr = shr_ha_mem_alloc(unit, tbl->comp_id, tbl->sub_id, NULL, &ha_blk_len);
    SHR_NULL_CHECK(ptr, SHR_E_MEMORY);
    if (ha_req_blk_len > ha_blk_len) {
        tbl = shr_ha_mem_realloc(unit, tbl, ha_req_blk_len);
        SHR_NULL_CHECK(tbl, SHR_E_MEMORY);
        t_info->tbl = tbl;
        sal_memset(tbl+1, 0, ha_req_blk_len - sizeof(*tbl));
    }

    /*
     * Step 3: Insert all the cached entries from buff into their new table.
     */
    if (tbl->num_of_ent > 0) {
        SHR_IF_ERR_EXIT(restore_cached_entries(t_info, buff, key_len,
                                               entry_len, big_endian));
    }

exit:
    if (buff) {
        sal_free(buff);
    }
    SHR_FUNC_EXIT();
}
