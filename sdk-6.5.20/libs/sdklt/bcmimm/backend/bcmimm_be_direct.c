/*! \file be_direct_tbl.c
 *
 * In-Memory back-end implementation.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
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
 * Private functions
 */
/*!
 *\brief Insert an entry into direct table with array fields.
 *
 * This function inserts an entry into direct table that has one or more array
 * fields.
 *
 * \param [in] t_info Information related to the direct table.
 * \param [in] key The entry key.
 * \param [in] fld_cnt Indicates how many fields are in the buffer \c buf.
 * \param [in] buf Buffer contained the entry fields information.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM The key value exceeds the table size.
 * \return SHR_E_EXISTS Entry already exist.
 */
static int array_tbl_ent_insert(bcmimm_tbl_info_t *t_info,
                                uint32_t key,
                                uint32_t fld_cnt,
                                void *buf)
{
    direct_tbl_array_t *tbl = (direct_tbl_array_t *)t_info->tbl;
    direct_tbl_entry_array_t *entry;
    bcmimm_be_direct_tbl_field_array_t *in_fld = buf;
    uint32_t j;
    direct_tbl_array_field_t *fld;
    uint32_t entry_size = tbl->hdr.ent_len;
    uint8_t *ent_p = tbl->body;

    if (key < tbl->start_idx) {
        return SHR_E_PARAM;
    } else {
        key -= tbl->start_idx;
    }
    if (key >= tbl->rows) {
        return SHR_E_PARAM;
    }
    ent_p += entry_size * key;
    entry = (direct_tbl_entry_array_t *)ent_p;
    if (entry->valid) {
        return SHR_E_EXISTS;
    }
    fld = entry->fields;
    for (j = 0; j < fld_cnt; j++, fld++, in_fld++) {
        fld->fid = in_fld->id;
        fld->data = in_fld->data;
        fld->idx = in_fld->idx;
    }
    entry->count = fld_cnt;
    entry->valid = true;
    return SHR_E_NONE;
}

/*!
 *\brief Insert an entry into direct table without array fields.
 *
 * This function inserts an entry into direct table that has no array fields.
 *
 * \param [in] t_info Information related to the direct table.
 * \param [in] key The entry key.
 * \param [in] fld_cnt Indicates how many fields are in the buffer \c buf.
 * \param [in] buf Buffer contained the entry fields information.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM The key value exceeds the table size.
 * \return SHR_E_EXISTS Entry already exist.
 */
static int basic_tbl_ent_insert(bcmimm_tbl_info_t *t_info,
                                uint32_t key,
                                uint32_t fld_cnt,
                                void *buf)
{
    direct_tbl_basic_t *tbl = (direct_tbl_basic_t *)t_info->tbl;
    direct_tbl_entry_basic_t *entry;
    bcmimm_be_direct_tbl_field_basic_t *in_fld = buf;
    uint32_t j;
    direct_tbl_basic_field_t *fld;
    uint32_t entry_size = tbl->hdr.ent_len;
    uint8_t *ent_p = tbl->body;

    if (key < tbl->start_idx) {
        return SHR_E_PARAM;
    } else {
        key -= tbl->start_idx;
    }
    if (key >= tbl->rows) {
        return SHR_E_PARAM;
    }
    ent_p += entry_size * key;
    entry = (direct_tbl_entry_basic_t *)ent_p;
    if (entry->valid) {
        return SHR_E_EXISTS;
    }
    fld = entry->fields;
    for (j = 0; j < fld_cnt; j++, fld++, in_fld++) {
        fld->fid = in_fld->id;
        fld->data = in_fld->data;
    }
    entry->count = fld_cnt;
    entry->valid = true;
    return SHR_E_NONE;
}

/*!
 *\brief Update an entry of a direct table with array fields.
 *
 * This function updates an entry of a direct table that has one or more array
 * fields. The function deletes fields that their corresponding value in the
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
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM The key value exceeds the table size.
 * \return SHR_E_NOT_FOUND The entry doesn't exist.
 */
static int array_tbl_ent_update(bcmimm_tbl_info_t *t_info,
                                uint32_t key,
                                uint32_t fld_cnt,
                                bool *del,
                                void *buf)
{
    direct_tbl_array_t *tbl = (direct_tbl_array_t *)t_info->tbl;
    direct_tbl_entry_array_t *entry;
    uint32_t in_idx, k;
    direct_tbl_array_field_t *fld;
    bcmimm_be_direct_tbl_field_array_t *in_fld;
    bool insert;
    uint32_t entry_size = tbl->hdr.ent_len;
    uint8_t *ent_p = tbl->body;

    if (key < tbl->start_idx) {
        return SHR_E_PARAM;
    } else {
        key -= tbl->start_idx;
    }
    if (key >= tbl->rows) {
        return SHR_E_PARAM;
    }
    ent_p += entry_size * key;
    entry = (direct_tbl_entry_array_t *)ent_p;
    if (!entry->valid) {
        return SHR_E_NOT_FOUND;
    }
    if (entry->lock > 0) {
        return SHR_E_ACCESS;
    }
    in_fld = (bcmimm_be_direct_tbl_field_array_t *)buf;
    for (in_idx = 0; in_idx < fld_cnt; in_idx++, in_fld++) {
        fld = entry->fields;
        insert = false;
        for (k = 0; k < entry->count; k++, fld++) {
            if (fld->fid == in_fld->id) {
                if (fld->idx > in_fld->idx) {
                    if (entry->count == tbl->hdr.data_len) {
                        return SHR_E_INTERNAL;
                    }
                    /* Need to prepare space in the middle */
                    sal_memmove(fld + 1, fld, (entry->count - k) *
                                sizeof(direct_tbl_array_field_t));
                    entry->count++;
                    fld->idx = in_fld->idx;
                    fld->data = in_fld->data;
                    insert = true;
                    break;
                } else if (fld->idx == in_fld->idx) {
                    break;
                }
            }
        }
        if (insert) {
            continue;
        }
        if (k < entry->count) {    /* Update the existing field */
            if (del && del[in_idx]) { /* Delete the field */
                if (entry->count > k + 1) {
                    sal_memmove(fld, fld + 1, (entry->count - (k + 1)) *
                                sizeof(direct_tbl_array_field_t));
                }
                entry->count--;
            } else {
                fld->data = in_fld->data;
            }
        } else if (entry->count == tbl->hdr.data_len) {
            return SHR_E_INTERNAL;
        } else {    /* Add a new field at the end */
            fld = &entry->fields[k];
            fld->fid = in_fld->id;
            fld->data = in_fld->data;
            fld->idx = in_fld->idx;
            entry->count++;
        }
    }
    return SHR_E_NONE;
}

/*!
 *\brief Update an entry of a direct table without array fields.
 *
 * This function updates an entry of a direct table that has no array
 * fields. The function deletes fields that their corresponding value in the
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
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM The key value exceeds the table size.
 * \return SHR_E_NOT_FOUND The entry doesn't exist.
 */
static int basic_tbl_ent_update(bcmimm_tbl_info_t *t_info,
                                uint32_t key,
                                uint32_t fld_cnt,
                                bool *del,
                                void *buf)
{
    direct_tbl_basic_t *tbl = (direct_tbl_basic_t *)t_info->tbl;
    direct_tbl_entry_basic_t *entry;
    uint32_t in_idx, k;
    direct_tbl_basic_field_t *fld;
    bcmimm_be_direct_tbl_field_basic_t *in_fld;
    uint32_t entry_size = tbl->hdr.ent_len;
    uint8_t *ent_p = tbl->body;

    if (key < tbl->start_idx) {
        return SHR_E_PARAM;
    } else {
        key -= tbl->start_idx;
    }

    if (key >= tbl->rows) {
        return SHR_E_PARAM;
    }
    ent_p += entry_size * key;
    entry = (direct_tbl_entry_basic_t *)ent_p;
    if (!entry->valid) {
        return SHR_E_NOT_FOUND;
    }
    if (entry->lock > 0) {
        return SHR_E_ACCESS;
    }
    in_fld = (bcmimm_be_direct_tbl_field_basic_t *)buf;
    for (in_idx = 0; in_idx < fld_cnt; in_idx++, in_fld++) {
        fld = entry->fields;
        for (k = 0; k < entry->count; k++, fld++) {
            if (fld->fid == in_fld->id) {
                break;
            }
        }
        if (k < entry->count) {    /* Update the existing field */
            if (del && del[in_idx]) { /* Delete the field */
                if (entry->count > k + 1) {
                    *fld = entry->fields[entry->count - 1];
                }
                entry->count--;
            } else {
                fld->data = in_fld->data;
            }
        } else if (entry->count == tbl->hdr.data_len) {
            return SHR_E_INTERNAL;
        } else {    /* Add a new field at the end */
            fld = &entry->fields[k];
            fld->fid = in_fld->id;
            fld->data = in_fld->data;
            entry->count++;
        }
    }
    return SHR_E_NONE;
}

/*!
 *\brief Lookup an entry of a direct table with array fields.
 *
 * This function lookup an entry of a direct table that has one or more array
 * fields.
 *
 * \param [in] t_info Information related to the direct table.
 * \param [in] key The entry key.
 * \param [in,out] fld_cnt Indicates how many fields are in the buffer \c buf.
 * \param [out] buf Buffer to place the entry fields information.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM The key value exceeds the table size.
 * \return SHR_E_NOT_FOUND Entry not found.
 */
static int array_tbl_ent_lookup(bcmimm_tbl_info_t *t_info,
                                uint32_t key,
                                size_t *fld_cnt,
                                void *buf)
{
    direct_tbl_array_t *tbl = (direct_tbl_array_t *)t_info->tbl;
    bcmimm_be_direct_tbl_field_array_t *out_fld = buf;
    direct_tbl_entry_array_t *entry;
    direct_tbl_array_field_t *fld;
    uint32_t j;
    uint32_t entry_size = tbl->hdr.ent_len;
    uint8_t *ent_p = tbl->body;

    if (key < tbl->start_idx) {
        return SHR_E_PARAM;
    } else {
        key -= tbl->start_idx;
    }

    if (key >= tbl->rows) {
        return SHR_E_PARAM;
    }
    ent_p += entry_size * key;
    entry = (direct_tbl_entry_array_t *)ent_p;
    if (entry->count > *fld_cnt) {
        return SHR_E_PARAM;
    }
    if (!entry->valid) {
        return SHR_E_NOT_FOUND;
    }
    fld = entry->fields;
    for (j = 0; j < entry->count; j++, fld++, out_fld++) {
        out_fld->id = fld->fid;
        out_fld->data = fld->data;
        out_fld->idx = fld->idx;
    }
    *fld_cnt = entry->count;
    return SHR_E_NONE;
}

/*!
 *\brief Lookup an entry of a direct table without array fields.
 *
 * This function lookup an entry of a direct table that has no array fields.
 *
 * \param [in] t_info Information related to the direct table.
 * \param [in] key The entry key.
 * \param [in,out] fld_cnt Indicates how many fields are in the buffer \c buf.
 * \param [out] buf Buffer to place the entry fields information.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM The key value exceeds the table size.
 * \return SHR_E_NOT_FOUND Entry not found.
 */
static int basic_tbl_ent_lookup(bcmimm_tbl_info_t *t_info,
                                uint32_t key,
                                size_t *fld_cnt,
                                void *buf)
{
    direct_tbl_basic_t *tbl = (direct_tbl_basic_t *)t_info->tbl;
    bcmimm_be_direct_tbl_field_basic_t *out_fld = buf;
    direct_tbl_entry_basic_t *entry;
    direct_tbl_basic_field_t *fld;
    uint32_t j;
    uint32_t entry_size = tbl->hdr.ent_len;
    uint8_t *ent_p = tbl->body;

    if (key < tbl->start_idx) {
        return SHR_E_PARAM;
    } else {
        key -= tbl->start_idx;
    }

    if (key >= tbl->rows) {
        return SHR_E_PARAM;
    }
    ent_p += entry_size * key;
    entry = (direct_tbl_entry_basic_t *)ent_p;
    if (entry->count > *fld_cnt) {
        return SHR_E_PARAM;
    }
    if (!entry->valid) {
        return SHR_E_NOT_FOUND;
    }
    fld = entry->fields;
    for (j = 0; j < entry->count; j++, fld++, out_fld++) {
        out_fld->id = fld->fid;
        out_fld->data = fld->data;
    }
    *fld_cnt = entry->count;
    return SHR_E_NONE;
}

/*!
 *\brief Lookup for the next entry of a direct table with array fields.
 *
 * This function lookup for the next entry of a direct table that has one or
 * more array fields. The search is starting based on the value of the key.
 *
 * \param [in] t_info Information related to the direct table.
 * \param [in,out] fld_cnt Indicates how many fields are in the buffer \c buf.
 * For output it indicates how many fields were written into the buffer.
 * \param [in,out] key entry key. The initial value being used to start the
 * search for an entry. Once an entry being found its key will be placed within
 * this parameter.
 * \param [out] buf Buffer to place the entry fields information.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM The key value exceeds the table size.
 * \return SHR_E_NOT_FOUND Entry not found.
 */
static int array_tbl_ent_next(bcmimm_tbl_info_t *t_info,
                              size_t *fld_cnt,
                              uint32_t *key,
                              void *buf)
{
    direct_tbl_array_t *tbl = (direct_tbl_array_t *)t_info->tbl;
    direct_tbl_entry_array_t *entry;
    bcmimm_be_direct_tbl_field_array_t *out_fld = buf;
    direct_tbl_array_field_t *fld;
    uint32_t j;
    uint32_t entry_size = tbl->hdr.ent_len;
    uint8_t *ent_p = tbl->body;

    if (*key >= tbl->start_idx) {
        *key -= tbl->start_idx;
    }
    if (*key >= tbl->rows) {
        return SHR_E_NOT_FOUND;
    }
    ent_p += entry_size * (*key);
    entry = (direct_tbl_entry_array_t *)ent_p;
    for (j = *key; j < tbl->rows; j++) {
        if (entry->valid) {
            break;
        }
        ent_p += entry_size;
        entry = (direct_tbl_entry_array_t *)ent_p;
    }
    if (j == tbl->rows) {
        return SHR_E_NOT_FOUND;
    }
    if (entry->count > *fld_cnt) {
        return SHR_E_INTERNAL;
    }
    *key = j + tbl->start_idx;
    fld = entry->fields;
    for (j = 0; j < entry->count; j++, fld++, out_fld++) {
        out_fld->id = fld->fid;
        out_fld->data = fld->data;
        out_fld->idx = fld->idx;
    }
    *fld_cnt = entry->count;
    return SHR_E_NONE;
}

/*!
 *\brief Lookup for the next entry of a direct table without array fields.
 *
 * This function lookup for the next entry of a direct table that has no
 * array fields. The search is starting based on the value of the key.
 *
 * \param [in] t_info Information related to the direct table.
 * \param [in,out] fld_cnt Indicates how many fields are in the buffer \c buf.
 * For output it indicates how many fields were written into the buffer.
 * \param [in,out] key entry key. The initial value being used to start the
 * search for an entry. Once an entry being found its key will be placed within
 * this parameter.
 * \param [out] buf Buffer to place the entry fields information.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM The key value exceeds the table size.
 * \return SHR_E_NOT_FOUND Entry not found.
 */
static int basic_tbl_ent_next(bcmimm_tbl_info_t *t_info,
                              size_t *fld_cnt,
                              uint32_t *key,
                              void *buf)
{
    direct_tbl_basic_t *tbl = (direct_tbl_basic_t *)t_info->tbl;
    direct_tbl_entry_basic_t *entry;
    bcmimm_be_direct_tbl_field_basic_t *out_fld = buf;
    direct_tbl_basic_field_t *fld;
    uint32_t j;
    uint32_t entry_size = tbl->hdr.ent_len;
    uint8_t *ent_p = tbl->body;

    if (*key >= tbl->start_idx) {
        *key -= tbl->start_idx;
    }
    if (*key >= tbl->rows) {
        return SHR_E_NOT_FOUND;
    }
    ent_p += entry_size * (*key);
    entry = (direct_tbl_entry_basic_t *)ent_p;
    for (j = *key; j < tbl->rows; j++) {
        if (entry->valid) {
            break;
        }
        ent_p += entry_size;
        entry = (direct_tbl_entry_basic_t *)ent_p;
    }
    if (j == tbl->rows) {
        return SHR_E_NOT_FOUND;
    }
    if (entry->count > *fld_cnt) {
        return SHR_E_INTERNAL;
    }
    *key = j + tbl->start_idx;
    fld = entry->fields;
    for (j = 0; j < entry->count; j++, fld++, out_fld++) {
        out_fld->id = fld->fid;
        out_fld->data = fld->data;
    }
    *fld_cnt = entry->count;
    return SHR_E_NONE;
}

/*!
 *\brief Delete an entry of a direct table with array fields.
 *
 * This function lookup for the next entry of a direct table that has one or
 * more array fields.
 *
 * \param [in] t_info Information related to the direct table.
 * \param [in] key entry key.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM The key value exceeds the table size.
 * \return SHR_E_ACCESS Entry is locked.
 * \return SHR_E_NOT_FOUND Entry was not found.
 */
static int array_tbl_ent_delete(bcmimm_tbl_info_t *t_info, uint32_t key)
{
    direct_tbl_array_t *tbl = (direct_tbl_array_t *)t_info->tbl;
    direct_tbl_entry_array_t *entry;
    uint32_t entry_size = tbl->hdr.ent_len;
    uint8_t *ent_p = tbl->body;

    if (key < tbl->start_idx) {
        return SHR_E_PARAM;
    } else {
        key -= tbl->start_idx;
    }
    if (key >= tbl->rows) {
        return SHR_E_PARAM;
    }
    ent_p += entry_size * key;
    entry = (direct_tbl_entry_array_t *)ent_p;
    if (entry->lock > 0) {
        return SHR_E_ACCESS;
    }
    if (!entry->valid) {
        return SHR_E_NOT_FOUND;
    }
    sal_memset(entry->fields, 0, entry_size - sizeof(*entry));
    entry->count = 0;
    entry->valid = false;
    return SHR_E_NONE;
}

/*!
 *\brief Delete an entry of a direct table without array fields.
 *
 * This function lookup for the next entry of a direct table that has no
 * array fields.
 *
 * \param [in] t_info Information related to the direct table.
 * \param [in] key entry key.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM The key value exceeds the table size.
 * \return SHR_E_ACCESS Entry is locked.
 * \return SHR_E_NOT_FOUND Entry was not found.
 */
static int basic_tbl_ent_delete(bcmimm_tbl_info_t *t_info, uint32_t key)
{
    direct_tbl_basic_t *tbl = (direct_tbl_basic_t *)t_info->tbl;
    direct_tbl_entry_basic_t *entry;
    uint32_t entry_size = tbl->hdr.ent_len;
    uint8_t *ent_p = tbl->body;

    if (key < tbl->start_idx) {
        return SHR_E_PARAM;
    } else {
        key -= tbl->start_idx;
    }
    if (key >= tbl->rows) {
        return SHR_E_PARAM;
    }
    ent_p += entry_size * key;
    entry = (direct_tbl_entry_basic_t *)ent_p;
    if (entry->lock > 0) {
        return SHR_E_ACCESS;
    }
    if (!entry->valid) {
        return SHR_E_NOT_FOUND;
    }
    sal_memset(entry->fields, 0, entry_size - sizeof(*entry));
    entry->count = 0;
    entry->valid = false;
    return SHR_E_NONE;
}

/*!
 *\brief Take a content snapshot of a direct table with array fields.
 *
 * This function take a content snapshot of a direct table that has one or more
 * array fields. The snapshot only contains the index of the entries that have
 * content.
 * It is assumed that the traverse buffer is clean.
 *
 * \param [in] t_info Information related to the direct table.
 *
 * \return None.
 */
static void array_tbl_ctx_prep(bcmimm_tbl_info_t *t_info)
{
    direct_tbl_array_t *tbl = (direct_tbl_array_t *)t_info->tbl;
    direct_tbl_entry_array_t *entry;
    uint32_t j;
    uint32_t entry_size = tbl->hdr.ent_len;
    uint8_t *ent_p = tbl->body;

    for (j = 0; j < tbl->rows; j++) {
        entry = (direct_tbl_entry_array_t *)ent_p;
        if (entry->valid) {
            t_info->traverse_buf[j] = 1;
        }
        ent_p += entry_size;
    }
}

/*!
 *\brief Take a content snapshot of a direct table without array fields.
 *
 * This function take a content snapshot of a direct table that has no
 * array fields. The snapshot only contains the index of the entries that have
 * content.
 * It is assumed that the traverse buffer is clean.
 *
 * \param [in] t_info Information related to the direct table.
 *
 * \return None.
 */
static void basic_tbl_ctx_prep(bcmimm_tbl_info_t *t_info)
{
    direct_tbl_basic_t *tbl = (direct_tbl_basic_t *)t_info->tbl;
    direct_tbl_entry_basic_t *entry;
    uint32_t j;
    uint32_t entry_size = tbl->hdr.ent_len;
    uint8_t *ent_p = tbl->body;

    for (j = 0; j < tbl->rows; j++) {
        entry = (direct_tbl_entry_basic_t *)ent_p;
        if (entry->valid) {
            t_info->traverse_buf[j] = 1;
        }
        ent_p += entry_size;
    }
}

/*!
 *\brief Lookup for the next entry of a direct table with array fields.
 *
 * This function lookup for the next entry of a direct table that has one or
 * more array fields. The search is starting based on the value of the key.
 * This search is only done on a snapshot that was taken previously by the
 * function \ref array_tbl_ctx_prep().
 *
 * \param [in] t_info Information related to the direct table.
 * \param [in] fld_cnt Indicates how many fields are in the buffer \c buf.
 * \param [in,out] key entry key. The initial value being used to start the
 * search for an entry. Once an entry being found its key will be placed within
 * this parameter.
 * \param [out] buf Buffer to place the entry fields information.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_NOT_FOUND Entry not found.
 */
static int array_tbl_ctx_next(bcmimm_tbl_info_t *t_info,
                              size_t *fld_cnt,
                              uint32_t *key,
                              void *buf)
{
    direct_tbl_array_t *tbl = (direct_tbl_array_t *)t_info->tbl;
    direct_tbl_entry_array_t *entry;
    bcmimm_be_direct_tbl_field_array_t *out_fld = buf;
    direct_tbl_array_field_t *fld;
    uint32_t j, k;
    uint32_t entry_size = tbl->hdr.ent_len;
    uint8_t *ent_p;

    if (*key >= tbl->start_idx) {
        *key -= tbl->start_idx;
    }
    if (*key >= tbl->rows) {
        return SHR_E_NOT_FOUND;
    }

    ent_p = tbl->body + entry_size * (*key);
    for (j = *key; j < tbl->rows; j++) {
        entry = (direct_tbl_entry_array_t *)ent_p;
        if (t_info->traverse_buf[j] && entry->valid) {
            if (entry->count > *fld_cnt) {
                return SHR_E_INTERNAL;
            }
            *key = j + tbl->start_idx;
            fld = entry->fields;
            for (k = 0; k < entry->count; k++, fld++, out_fld++) {
                out_fld->id = fld->fid;
                out_fld->data = fld->data;
                out_fld->idx = fld->idx;
            }
            *fld_cnt = entry->count;
            return SHR_E_NONE;
        }
        ent_p += entry_size;
    }
    return SHR_E_NOT_FOUND;
}

/*!
 *\brief Lookup for the next entry of a direct table without array fields.
 *
 * This function lookup for the next entry of a direct table that has no
 * array fields. The search is starting based on the value of the key.
 * This search is only done on a snapshot that was taken previously by the
 * function \ref basic_tbl_ctx_prep().
 *
 * \param [in] t_info Information related to the direct table.
 * \param [in] fld_cnt Indicates how many fields are in the buffer \c buf.
 * \param [in,out] key entry key. The initial value being used to start the
 * search for an entry. Once an entry being found its key will be placed within
 * this parameter.
 * \param [out] buf Buffer to place the entry fields information.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_NOT_FOUND Entry not found.
 */
static int basic_tbl_ctx_next(bcmimm_tbl_info_t *t_info,
                              size_t *fld_cnt,
                              uint32_t *key,
                              void *buf)
{
    direct_tbl_basic_t *tbl = (direct_tbl_basic_t *)t_info->tbl;
    direct_tbl_entry_basic_t *entry;
    bcmimm_be_direct_tbl_field_basic_t *out_fld = buf;
    direct_tbl_basic_field_t *fld;
    uint32_t j, k;
    uint32_t entry_size = tbl->hdr.ent_len;
    uint8_t *ent_p;

    if (*key >= tbl->start_idx) {
        *key -= tbl->start_idx;
    }
    if (*key >= tbl->rows) {
        return SHR_E_NOT_FOUND;
    }

    ent_p = tbl->body + entry_size * (*key);
    for (j = *key; j < tbl->rows; j++) {
        entry = (direct_tbl_entry_basic_t *)ent_p;
        if (t_info->traverse_buf[j] && entry->valid) {
            if (entry->count > *fld_cnt) {
                return SHR_E_INTERNAL;
            }
            *key = j + tbl->start_idx;
            fld = entry->fields;
            for (k = 0; k < entry->count; k++, fld++, out_fld++) {
                out_fld->id = fld->fid;
                out_fld->data = fld->data;
            }
            *fld_cnt = entry->count;
            return SHR_E_NONE;
        }
        ent_p += entry_size;
    }
    return SHR_E_NOT_FOUND;
}

/*!
 *\brief Clears the content of a direct table with array fields.
 *
 * This function clears the content of a direct table that has one or more
 * array fields.
 *
 * \param [in] t_info Information related to the direct table.
 *
 * \return None.
 */
static void array_tbl_clear(bcmimm_tbl_info_t *t_info)
{
    direct_tbl_array_t *tbl = (direct_tbl_array_t *)t_info->tbl;
    uint32_t entry_size = tbl->hdr.ent_len;

    sal_memset(tbl->body, 0, entry_size * tbl->rows);
    tbl->hdr.num_of_ent = 0;
}

/*!
 *\brief Clears the content of a direct table without array fields.
 *
 * This function clears the content of a direct table that has no array fields.
 *
 * \param [in] t_info Information related to the direct table.
 *
 * \return None.
 */
static void basic_tbl_clear(bcmimm_tbl_info_t *t_info)
{
    direct_tbl_basic_t *tbl = (direct_tbl_basic_t *)t_info->tbl;
    uint32_t entry_size = tbl->hdr.ent_len;

    sal_memset(tbl->body, 0, entry_size * tbl->rows);
    tbl->hdr.num_of_ent = 0;
}

/*!
 *\brief Retrieve the entry count of a direct table with array fields.
 *
 * This function retrieves the entry count of a direct table that has array
 * fields.
 *
 * \param [in] t_info Information related to the direct table.
 *
 * \return The entry count.
 */
static uint32_t array_tbl_entry_count(bcmimm_tbl_info_t *t_info)
{
    direct_tbl_array_t *tbl = (direct_tbl_array_t *)t_info->tbl;

    return tbl->hdr.num_of_ent;
}

/*!
 *\brief Retrieve the entry count of a direct table without array fields.
 *
 * This function retrieves the entry count of a direct table that has no array
 * fields.
 *
 * \param [in] t_info Information related to the direct table.
 *
 * \return The entry count.
 */
static uint32_t basic_tbl_entry_count(bcmimm_tbl_info_t *t_info)
{
    direct_tbl_basic_t *tbl = (direct_tbl_basic_t *)t_info->tbl;

    return tbl->hdr.num_of_ent;
}


/*!
 *\brief Update the locks count of an entry in a direct table with array fields.
 *
 * This function increments/decrement the lock count of an entry in a direct
 * table that has one or more array fields. The lock count increment by the
 * value of the \c val parameter.
 *
 * \param [in] t_info Information related to the direct table.
 * \param [in] key entry key.
 * \param [in] val The value to add into the lock count.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_NOT_FOUND Entry not found.
 * \return SHR_E_UNAVAIL The expected lock count is too large or small.
 */
static int array_entry_lock(bcmimm_tbl_info_t *t_info, uint32_t key, int val)
{
    direct_tbl_array_t *tbl = (direct_tbl_array_t *)t_info->tbl;
    direct_tbl_entry_array_t *entry;
    uint32_t entry_size = tbl->hdr.ent_len;
    uint8_t *ent_p = tbl->body;

    if (key < tbl->start_idx) {
        return SHR_E_PARAM;
    } else {
        key -= tbl->start_idx;
    }
    if (key >= tbl->rows) {
        return SHR_E_PARAM;
    }
    ent_p += entry_size * key;
    entry = (direct_tbl_entry_array_t *)ent_p;
    if (!entry->valid) {
        return SHR_E_NOT_FOUND;
    }
    if (val > 0 && ((uint8_t)-1) - entry->lock < val) {
        return SHR_E_UNAVAIL;
    }
    if (val < 0 && -val > entry->lock) {
        return SHR_E_UNAVAIL;
    }
    entry->lock += val;
    return SHR_E_NONE;
}

/*!
 *\brief Update the locks count of an entry in a direct table without array
 * fields.
 *
 * This function increments/decrement the lock count of an entry in a direct
 * table that has no array fields. The lock count increment by the value of the
 * \c val parameter.
 *
 * \param [in] t_info Information related to the direct table.
 * \param [in] key entry key.
 * \param [in] val The value to add into the lock count.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_NOT_FOUND Entry not found.
 * \return SHR_E_UNAVAIL The expected lock count is too large or small.
 */
static int basic_entry_lock(bcmimm_tbl_info_t *t_info, uint32_t key, int val)
{
    direct_tbl_basic_t *tbl = (direct_tbl_basic_t *)t_info->tbl;
    direct_tbl_entry_basic_t *entry;
    uint32_t entry_size = tbl->hdr.ent_len;
    uint8_t *ent_p = tbl->body;

    if (key < tbl->start_idx) {
        return SHR_E_PARAM;
    } else {
        key -= tbl->start_idx;
    }
    if (key >= tbl->rows) {
        return SHR_E_PARAM;
    }
    ent_p += entry_size * key;
    entry = (direct_tbl_entry_basic_t *)ent_p;
    if (!entry->valid) {
        return SHR_E_NOT_FOUND;
    }
    if (val > 0 && entry->lock == (uint8_t)-1) {
        return SHR_E_UNAVAIL;
    }
    if (val < 0 && entry->lock == 0) {
        return SHR_E_UNAVAIL;
    }
    entry->lock += val;
    return SHR_E_NONE;
}

/*!
 *\brief Retrieve an entry lock count for a direct table with array fields.
 *
 * This function retrieves the lock count of an entry in a direct table that
 * has one or more array fields.
 *
 * \param [in] t_info Information related to the direct table.
 * \param [in] key entry key.
 * \param [out] val The value to add into the lock count.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid key value.
 * \return SHR_E_NOT_FOUND The entry was not found.
 */
static int array_entry_lock_cnt(bcmimm_tbl_info_t *t_info,
                                uint32_t key,
                                uint32_t *lock_cnt)
{
    direct_tbl_array_t *tbl = (direct_tbl_array_t *)t_info->tbl;
    direct_tbl_entry_array_t *entry;
    uint32_t entry_size = tbl->hdr.ent_len;
    uint8_t *ent_p = tbl->body;

    if (key < tbl->start_idx) {
        return SHR_E_PARAM;
    } else {
        key -= tbl->start_idx;
    }
    if (key >= tbl->rows) {
        return SHR_E_PARAM;
    }
    ent_p += entry_size * key;
    entry = (direct_tbl_entry_array_t *)ent_p;
    if (!entry->valid) {
        return SHR_E_NOT_FOUND;
    }
    *lock_cnt = entry->lock;
    return SHR_E_NONE;
}

/*!
 *\brief Retrieve an entry lock count for a direct table without array fields.
 *
 * This function retrieves the lock count of an entry in a direct table that
 * has no array fields.
 *
 * \param [in] t_info Information related to the direct table.
 * \param [in] key entry key.
 * \param [out] val The value to add into the lock count.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid key value.
 * \return SHR_E_NOT_FOUND The entry was not found.
 */
static int basic_entry_lock_cnt(bcmimm_tbl_info_t *t_info,
                                uint32_t key,
                                uint32_t *lock_cnt)
{
    direct_tbl_basic_t *tbl = (direct_tbl_basic_t *)t_info->tbl;
    direct_tbl_entry_basic_t *entry;
    uint32_t entry_size = tbl->hdr.ent_len;
    uint8_t *ent_p = tbl->body;

    if (key < tbl->start_idx) {
        return SHR_E_PARAM;
    } else {
        key -= tbl->start_idx;
    }
   if (key >= tbl->rows) {
        return SHR_E_PARAM;
    }
    ent_p += entry_size * key;
    entry = (direct_tbl_entry_basic_t *)ent_p;
    if (!entry->valid) {
        return SHR_E_NOT_FOUND;
    }
    *lock_cnt = entry->lock;
    return SHR_E_NONE;
}

/*!
 *\brief Overwrite the field IDs for a direct table with array fields.
 *
 * This function overwrites all the field IDs of all the entries in a direct
 * table that has one or more array fields. This function is used in the case
 * of ISSU where the field ID may change.
 *
 * \param [in] t_info Information related to the direct table.
 * \param [in] key entry key.
 * \param [in] in An array of all the field set in the same order as the
 * original fields. The original array field being retrieved via the
 * \ref array_tbl_ent_next() function.
 *
 * \return SHR_E_NONE Success.
 */
static int array_entry_fid_override(bcmimm_tbl_info_t *t_info,
                                    uint32_t key,
                                    bcmimm_be_fields_t *in)
{
    direct_tbl_array_t *tbl = (direct_tbl_array_t *)t_info->tbl;
    uint32_t entry_size = tbl->hdr.ent_len;
    uint8_t *ent_p = tbl->body + entry_size * key;
    direct_tbl_entry_array_t *entry = (direct_tbl_entry_array_t *)ent_p;
    bcmimm_be_direct_tbl_field_array_t *in_fld = in->fdata;
    uint32_t j;

    if (in->count != entry->count) {
        return SHR_E_INTERNAL;
    }
    /*
     * The assumption is that the order of the fields in the input 'in'
     * matches the fields link list.
     */
    for (j = 0; j < in->count; j++, in_fld++) {
        entry->fields[j].fid = in_fld->id;
    }

    /* Clean fields that are no longer valid */
    j = 0;
    while (j < entry->count) {
        if (entry->fields[j].fid == BCMLTD_SID_INVALID) {
            uint16_t count = entry->count--;
            if (count > j) {
                sal_memcpy(&entry->fields[j], &entry->fields[count-1],
                           sizeof(direct_tbl_array_field_t));
            }
        }
        j++;
    }
    return SHR_E_NONE;
}

/*!
 *\brief Overwrite the field IDs for a direct table without array fields.
 *
 * This function overwrites all the field IDs of all the entries in a direct
 * table that has no array fields. This function is used in the case
 * of ISSU where the field ID may change.
 *
 * \param [in] t_info Information related to the direct table.
 * \param [in] key entry key.
 * \param [in] in An array of all the field set in the same order as the
 * original fields. The original array field being retrieved via the
 * \ref array_tbl_ent_next() function.
 *
 * \return SHR_E_NONE Success.
 */
static int basic_entry_fid_override(bcmimm_tbl_info_t *t_info,
                                    uint32_t key,
                                    bcmimm_be_fields_t *in)
{
    direct_tbl_basic_t *tbl = (direct_tbl_basic_t *)t_info->tbl;
    uint32_t entry_size = tbl->hdr.ent_len;
    uint8_t *ent_p = tbl->body + entry_size * key;
    direct_tbl_entry_basic_t *entry = (direct_tbl_entry_basic_t *)ent_p;
    bcmimm_be_direct_tbl_field_basic_t *in_fld = in->fdata;
    uint32_t j;

    if (in->count != entry->count) {
        return SHR_E_INTERNAL;
    }
    /*
     * The assumption is that the order of the fields in the input 'in'
     * matches the fields link list.
     */
    for (j = 0; j < in->count; j++, in_fld++) {
        entry->fields[j].fid = in_fld->id;
    }

    /* Clean fields that are no longer valid */
    j = 0;
    while (j < entry->count) {
        if (entry->fields[j].fid == BCMLTD_SID_INVALID) {
            uint16_t count = entry->count--;
            if (count > j) {
                sal_memcpy(&entry->fields[j], &entry->fields[count-1],
                           sizeof(direct_tbl_basic_field_t));
            }
        }
        j++;
    }
    return SHR_E_NONE;
}

/*!
 * \brief Rearrange direct table array entries to larger entry length.
 *
 * This function changes the body content to adjust into new entry length.
 * The function starts to move the last entry to its new location. Since
 * the entry length increases the new location is not contaminated.
 *
 * \param [in] t_info Table info contains all the current data before the
 * change.
 * \param [in] entry_len The new entry length.
 *
 * \retval SHR_E_NONE Success.
 */
static int array_adjust_entry_len(bcmimm_tbl_info_t *t_info,
                                  uint32_t entry_len)
{
    int idx;
    uint8_t *src, *dest;
    direct_tbl_array_t *array_tbl;
    direct_tbl_entry_array_t *array_ent;
    uint32_t ent_left = t_info->tbl->num_of_ent;

    array_tbl = (direct_tbl_array_t *)t_info->tbl;
    for (idx = (int)array_tbl->rows-1; idx >= 0 && ent_left; idx--) {
        src = array_tbl->body + (idx * t_info->tbl->ent_len);
        array_ent = (direct_tbl_entry_array_t *)src;
        if (array_ent->valid) {
            uint32_t len = t_info->tbl->ent_len;
            dest = array_tbl->body + (idx * entry_len);
            sal_memmove(dest, src, len);
            if (dest - src < len) {
               len = dest - src;
            }
            sal_memset(src, 0, len);
            ent_left--;
        }
    }
    return SHR_E_NONE;
}

/*!
 * \brief Rearrange direct table basic entries to larger entry length.
 *
 * This function changes the body content to adjust into new entry length.
 * The function starts to move the last entry to its new location. Since
 * the entry length increases the new location is not contaminated.
 *
 * \param [in] t_info Table info contains all the current data before the
 * change.
 * \param [in] entry_len The new entry length.
 *
 * \retval SHR_E_NONE Success.
 */
static int basic_adjust_entry_len(bcmimm_tbl_info_t *t_info,
                                  uint32_t entry_len)
{
    int idx;
    uint8_t *src, *dest;
    direct_tbl_basic_t *basic_tbl;
    direct_tbl_entry_basic_t *basic_ent;
    uint32_t ent_left = t_info->tbl->num_of_ent;

    basic_tbl = (direct_tbl_basic_t *)t_info->tbl;
    for (idx = (int)basic_tbl->rows-1; idx >= 0 && ent_left > 0; idx--) {
        src = basic_tbl->body + (idx * t_info->tbl->ent_len);
        basic_ent = (direct_tbl_entry_basic_t *)src;
        if (basic_ent->valid) {
            uint32_t len = t_info->tbl->ent_len;
            dest = basic_tbl->body + (idx * entry_len);
            sal_memmove(dest, src, len);
            if (dest - src < len) {
               len = dest - src;
            }
            sal_memset(src, 0, len);
            ent_left--;
        }
    }
    return SHR_E_NONE;
}

/*******************************************************************************
 * Public functions local to this component
 */
int bcmimm_be_direct_entry_insert(bcmimm_tbl_info_t *t_info,
                                  uint32_t key,
                                  uint32_t fld_cnt,
                                  void *buf)
{
    int rv;

    if (fld_cnt > t_info->tbl->data_len) {
        return SHR_E_PARAM;
    }
    if (t_info->tbl->array_tbl) {
        rv = array_tbl_ent_insert(t_info, key, fld_cnt, buf);
    } else {
        rv = basic_tbl_ent_insert(t_info, key, fld_cnt, buf);
    }
    if (rv == SHR_E_NONE) {
        t_info->tbl->num_of_ent++;
    }
    return rv;
}

int bcmimm_be_direct_entry_update(bcmimm_tbl_info_t *t_info,
                                  uint32_t key,
                                  uint32_t fld_cnt,
                                  bool *del,
                                  void *buf)
{
    if (t_info->tbl->array_tbl) {
        return array_tbl_ent_update(t_info, key, fld_cnt, del, buf);
    } else {
        return basic_tbl_ent_update(t_info, key, fld_cnt, del, buf);
    }
}

int bcmimm_be_direct_entry_lookup(bcmimm_tbl_info_t *t_info,
                                  uint32_t key,
                                  size_t *fld_cnt,
                                  void *buf)
{
    if (t_info->tbl->array_tbl) {
        return array_tbl_ent_lookup(t_info, key, fld_cnt, buf);
    } else {
        return basic_tbl_ent_lookup(t_info, key, fld_cnt, buf);
    }
}

int bcmimm_be_direct_snapshot_create(bcmimm_tbl_info_t *t_info)
{
    if (t_info->tbl->num_of_ent == 0) {
        return SHR_E_NOT_FOUND;     /* Table is empty */
    }
    t_info->traverse_buf = sal_alloc(t_info->tbl->num_of_rows,
                                     "bcmImmBeTraverse");
    if (!t_info->traverse_buf) {
        return SHR_E_MEMORY;
    }
    sal_memset(t_info->traverse_buf, 0, t_info->tbl->num_of_rows);
    t_info->trav_buf_size = t_info->tbl->num_of_rows;

    if (t_info->tbl->array_tbl) {
        array_tbl_ctx_prep(t_info);
    } else {
        basic_tbl_ctx_prep(t_info);
    }
    return SHR_E_NONE;
}

int bcmimm_be_direct_entry_get_first(bcmimm_tbl_info_t *t_info,
                                     bool use_snapshot,
                                     uint32_t *key,
                                     size_t *fld_cnt,
                                     void *buf)
{
    if (use_snapshot && !t_info->traverse_buf) {
        return SHR_E_PARAM;
    }
    if (t_info->tbl->num_of_ent == 0) {
        return SHR_E_NOT_FOUND;     /* Table is empty */
    }
    *key = 0;
    if (t_info->tbl->array_tbl) {
        if (use_snapshot) {
            return array_tbl_ctx_next(t_info, fld_cnt, key, buf);
        } else {
            return array_tbl_ent_next(t_info, fld_cnt, key, buf);
        }
    } else {
        if (use_snapshot) {
            return basic_tbl_ctx_next(t_info, fld_cnt, key, buf);
        } else {
            return basic_tbl_ent_next(t_info, fld_cnt, key, buf);
        }
    }
}

int bcmimm_be_direct_entry_get_next(bcmimm_tbl_info_t *t_info,
                                    bool use_snapshot,
                                    uint32_t *key,
                                    size_t *fld_cnt,
                                    void *buf)
{
    if ((use_snapshot && !t_info->traverse_buf) || !key) {
        return SHR_E_PARAM;
    }

    (*key)++;
    if (t_info->tbl->array_tbl) {
        if (use_snapshot) {
            return array_tbl_ctx_next(t_info, fld_cnt, key, buf);
        } else {
            return array_tbl_ent_next(t_info, fld_cnt, key, buf);
        }
    } else {
        if (use_snapshot) {
            return basic_tbl_ctx_next(t_info, fld_cnt, key, buf);
        } else {
            return basic_tbl_ent_next(t_info, fld_cnt, key, buf);
        }
    }
}

int bcmimm_be_direct_entry_delete(bcmimm_tbl_info_t *t_info,
                                  uint32_t key)
{
    int rv;

    if (t_info->tbl->array_tbl) {
        rv = array_tbl_ent_delete(t_info, key);
    } else {
        rv = basic_tbl_ent_delete(t_info, key);
    }
    if (rv == SHR_E_NONE) {
        t_info->tbl->num_of_ent--;
    }
    return rv;
}

void bcmimm_be_direct_tbl_clear(bcmimm_tbl_info_t *t_info)
{
    if (t_info->tbl->array_tbl) {
        array_tbl_clear(t_info);
    } else {
        basic_tbl_clear(t_info);
    }
}

uint32_t bcmimm_be_direct_tbl_cnt_get(bcmimm_tbl_info_t *t_info)
{
    if (t_info->tbl->array_tbl) {
        return array_tbl_entry_count(t_info);
    }  else {
        return basic_tbl_entry_count(t_info);
    }
}

int bcmimm_be_direct_entry_lock(bcmimm_tbl_info_t *t_info, uint32_t *key)
{
    if (t_info->tbl->array_tbl) {
        return array_entry_lock(t_info, *key, 1);
    } else {
        return basic_entry_lock(t_info, *key, 1);
    }
}

int bcmimm_be_direct_entry_unlock(bcmimm_tbl_info_t *t_info, uint32_t *key)
{
    if (t_info->tbl->array_tbl) {
        return array_entry_lock(t_info, *key, -1);
    } else {
        return basic_entry_lock(t_info, *key, -1);
    }
}

int bcmimm_be_direct_entry_lock_cnt(bcmimm_tbl_info_t *t_info,
                                    uint32_t *key,
                                    uint32_t *lock_cnt)
{
    if (t_info->tbl->array_tbl) {
        return array_entry_lock_cnt(t_info, *key, lock_cnt);
    } else {
        return basic_entry_lock_cnt(t_info, *key, lock_cnt);
    }
}
int bcmimm_be_direct_entry_overide(bcmimm_tbl_info_t *t_info,
                                   uint32_t *key,
                                   bcmimm_be_fields_t *in)
{
    if (t_info->tbl->array_tbl) {
        return array_entry_fid_override(t_info, *key, in);
    } else {
        return basic_entry_fid_override(t_info, *key, in);
    }
}

void bcmimm_be_adjust_direct_entry_length(bcmimm_tbl_info_t *t_info,
                                          uint32_t entry_len)
{
    if (t_info->tbl->array_tbl) {
        array_adjust_entry_len(t_info, entry_len);
    } else {
        basic_adjust_entry_len(t_info, entry_len);
    }
}

