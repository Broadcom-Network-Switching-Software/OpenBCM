/*! \file inmem_issu.c
 *
 *  ISSU Upgrade of IMM LTs containing enum fields.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_mutex.h>
#include <sal/sal_sem.h>
#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <sal/sal_assert.h>
#include <sal/sal_rwlock.h>
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <shr/shr_fmm.h>
#include <shr/shr_ha.h>
#include <bcmdrd_config.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_enum.h>
#include <bcmissu/issu_api.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmcfg/comp/bcmcfg_feature_ctl.h>
#include <bcmimm/bcmimm_backend.h>
#include <bcmimm/generated/front_e_ha.h>
#include <bcmimm/bcmimm_basic.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmimm/bcmimm_internal.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMIMM_FRONTEND

static bcmltd_fields_t field_buf[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Upgrade a particular enum field with its new value.
 *
 * \param [in] unit The unit associated with the LT.
 * \param [in] tbl LT information.
 * \param [in] fid Field ID to identify the field to process.
 * \param [in] enum_type_name The enum type of the field.
 * \param [in] count The number of fields in the entry.
 *
 * \return SHR_E_NONE Success, error code otherwise.
 */
static int update_entry_enum_field(int unit,
                                   table_info_t *tbl,
                                   uint32_t fid,
                                   const char *enum_type_name,
                                   size_t count)
{
    size_t j, k;
    bcmltd_field_t *fld;
    uint32_t current_val;
    int rv;
    int rc = SHR_E_NONE;

    for (j = 0; j < count; j++) {
        fld = field_buf[unit].field[j];
        if (fld->id == fid) {
            /*
             * Once we found the field go through all its instances and break.
             */
            for (k = j; k < count; k++) {
                fld = field_buf[unit].field[k];
                if (fld->id != fid) {
                    break;
                }
                rv = bcmissu_enum_sym_to_current(unit, enum_type_name,
                                                (uint32_t)fld->data, &current_val);
                if (SHR_FAILURE(rv)) {
                    /* Mark the field to be deleted */
                    fld->flags = SHR_FMM_FIELD_DEL;
                    rc = rv;
                    continue;
                }
                fld->data = current_val;
            }
            break;
        }
    }
    return rc;
}

/*!
 * \brief Prepare working buffers.
 *
 * This function prepares the 'in' and 'out' buffers that can be used
 * to communicate with the back end.
 *
 * \param [in] tbl The associated front end table.
 * \param [in] data_buf The data buffer used by the 'in' and 'out' buffers.
 * \param [out] be_out The out buffer.
 * \param [out] be_in The 'in' buffer.
 * \param [out] data_field_cnt Indicating the number of data fields within the
 * entry.
 */
static void prep_buffers(table_info_t *tbl,
                         uint8_t *data_buf,
                         bcmimm_be_fields_t *be_out,
                         bcmimm_be_fields_upd_t *be_in,
                         uint32_t *data_field_cnt)
{
    uint32_t data_f_size;

    data_f_size = BCMIMM_BE_FID_SIZE + tbl->f_data_size + sizeof(bool);
    *data_field_cnt = tbl->data_bits_len / data_f_size;
    be_out->count = *data_field_cnt;
    if (tbl->type == IMM_BASIC) {
        be_out->fdata = data_buf + BCMIMM_BE_FID_SIZE * (*data_field_cnt);
        be_out->fid = (uint32_t *)data_buf;
    } else {
        be_out->fdata = data_buf;
        be_out->fid = NULL;
    }
    be_in->del = (bool *)
        ((char *)be_out->fdata + tbl->f_data_size * (*data_field_cnt));
    if ((*data_field_cnt) > 0) {
        sal_memset(be_in->del, 0, sizeof(bool) * (*data_field_cnt));
    }
    be_in->fdata = be_out->fdata;
    be_in->fid = be_out->fid;
}

/*!
 * \brief Drains a complete IMM table onto a memory block.
 *
 * This function allocates a memory block that will contain all the content of
 * an IMM table. The data will be kept in IMM BE format.
 * After reading an entry the function will copy its content into the buffer
 * and then will delete the entry from the IMM.
 * This action is necessary when a table key is of enum type and the enum had
 * changed between the versions.
 * The caller is responsible to free the returned memory block by calling
 * sal_free().
 *
 * \param [in] tbl LT information.
 * \param [in] ent_cnt the number of existing entries in the table.
 * \param [in] entry_size The size of each row in the memory block.
 * \param [in] be_out Back-end formatted buffer to extract the entries.
 * \param [in] in_key_buf Buffer to store the input key.
 * \param [in] out_key_buf Buffer to store the output key.
 * \param [in] data_field_cnt The maximal number of field in the entry.
 *
 * \return Pointer to the memory block - Success. NULL failure.
 */
static uint8_t *copy_be_tbl_to_local_buf(table_info_t *tbl,
                                         size_t ent_cnt,
                                         uint32_t entry_size,
                                         bcmimm_be_fields_t *be_out,
                                         uint8_t *in_key_buf,
                                         uint8_t *out_key_buf,
                                         uint32_t data_field_cnt)
{
    uint8_t *tbl_buf;
    uint32_t idx = 0;
    uint32_t *fld_cnt;
    uint32_t key_size;
    uint8_t *data;
    int rv;

    tbl_buf = sal_alloc(ent_cnt * entry_size, "bcmimmIssu");
    if (!tbl_buf) {
        return NULL;
    }
    key_size = BCMIMM_BITS_TO_BYTES(tbl->key_arry_size);
    rv = bcmimm_be_table_get_first(tbl->blk_hdl, false, out_key_buf, be_out);
    if (SHR_FAILURE(rv)) {
        sal_free(tbl_buf);
        return NULL;
    }
    do {
        /* Each entry has the following format:
         *  - Field count (32 bits)
         *  - Key value (key_size bytes).
         *  - Field IDs (each of BCMIMM_BE_FID_SIZE bytes) total of field count
         * elements.
         *  - Field data (each tbl->f_data_size long) total of field count
         *  elements.
         */
        fld_cnt = (uint32_t *)(tbl_buf + idx * entry_size);
        *fld_cnt = be_out->count; /* Set the field count */
        fld_cnt++;
        data = (uint8_t *)fld_cnt;
        sal_memcpy(data, out_key_buf, key_size); /* Copy the key */
        data += key_size;
        /* Copy all the field IDs */
        sal_memcpy(data, be_out->fid, BCMIMM_BE_FID_SIZE * be_out->count);
        data += BCMIMM_BE_FID_SIZE * be_out->count;
        sal_memcpy(data, be_out->fdata, tbl->f_data_size * be_out->count);
        idx++;
        be_out->count = data_field_cnt;
        sal_memcpy(in_key_buf, out_key_buf, key_size);
        rv = bcmimm_be_table_get_next(tbl->blk_hdl, false, in_key_buf,
                                      out_key_buf, be_out);
        /* Delete the previous entry */
        bcmimm_be_entry_delete(tbl->blk_hdl, in_key_buf);
    } while (SHR_SUCCESS(rv) && idx < ent_cnt);

    if (idx != ent_cnt) {
        sal_free(tbl_buf);
        return NULL;
    }
    return tbl_buf;
}

/*!
 * \brief Update enum values for key field.
 *
 * This function works on key fields that are of type enum and their enum
 * definition had changed. Since a key field changed the entry should be
 * removed and re-entered using its new key.
 * Also the table traverse require snapshot, otherwise the same entry might
 * show up again after its key was upgraded.
 *
 * \param [in] unit The unit associated with the LT.
 * \param [in] tbl LT information.
 * \param [in] fid Field ID to identify the field to process.
 * \param [in] enum_type_name The enum type of the field.
 * \param [in] in_key_buf Buffer to store the input key.
 * \param [in] out_key_buf Buffer to store the output key.
 * \param [in] data_buf Buffer to store the entry content.
 *
 * \return SHR_E_NONE Success, error code otherwise.
 */
static int update_lt_enum_key_field(int unit,
                                    table_info_t *tbl,
                                    uint32_t fid,
                                    const char *enum_type_name,
                                    uint8_t *in_key_buf,
                                    uint8_t *out_key_buf,
                                    uint8_t *data_buf)
{
    bcmimm_be_fields_t be_out;
    bcmimm_be_fields_upd_t be_in;
    int rv;
    uint32_t data_field_cnt;
    bcmimm_table_dynamic_info_t *dyn_info;
    size_t rows;
    uint32_t entry_size;
    uint8_t *tbl_buf;
    uint32_t j;
    uint32_t *fld_cnt;
    uint8_t *data;
    uint32_t key_val = 0, new_key_val;
    uint32_t key_size = BCMIMM_BITS_TO_BYTES(tbl->key_arry_size);


    prep_buffers(tbl, data_buf, &be_out, &be_in, &data_field_cnt);
    /*
     * Allocate memory to host the entire LT in back-end format.
     * That is needed since modified key may clash with a key of an entry
     * that is already in the table.
     */
    rows = bcmimm_be_table_size_get(tbl->blk_hdl);
    if (rows == 0) {
        return SHR_E_NONE;
    }
    entry_size = (BCMIMM_BE_FID_SIZE + tbl->f_data_size) * data_field_cnt;
    /* Every entry will start with the field count followed by the key value */
    entry_size += sizeof(uint32_t) + key_size;
    /* Round the entry size to multiple of 4. */
    entry_size = ((entry_size + 3) / 4) * 4;

    tbl_buf = copy_be_tbl_to_local_buf(tbl, rows, entry_size, &be_out,
                                       in_key_buf, out_key_buf,
                                       data_field_cnt);
    if (!tbl_buf) {
        return SHR_E_INTERNAL;
    }

    for (j = 0; j < rows; j++) {
        fld_cnt = (uint32_t *)(tbl_buf + j * entry_size);
        be_out.count = *fld_cnt;
        fld_cnt++;
        sal_memcpy(&key_val, fld_cnt, key_size);
        /* Convert the enum value to its new value */
        rv = bcmissu_enum_sym_to_current(unit, enum_type_name,
                                         key_val, &new_key_val);
        if (SHR_FAILURE(rv)) {
            continue;  /* Enum value may have been deleted */
        }
        sal_memcpy(in_key_buf, &new_key_val, key_size);
        data = (uint8_t *)fld_cnt;
        data += key_size;
        sal_memcpy(be_out.fid, data, BCMIMM_BE_FID_SIZE * be_out.count);
        data += BCMIMM_BE_FID_SIZE * be_out.count;
        sal_memcpy(be_out.fdata, data, tbl->f_data_size * be_out.count);
        rv = bcmimm_be_entry_insert(tbl->blk_hdl, in_key_buf, &be_out);
        if (SHR_FAILURE(rv)) {
            dyn_info = &bcmimm_dynamic_info[unit][tbl->fields_idx];
            LOG_WARN(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                              "Failed to insert an entry to LT=%s\n"),
                       dyn_info->tbl_name));
        }
    }
    sal_free(tbl_buf);

    return SHR_E_NONE;
}

/*!
 * \brief Update specific enum field throughout the entire LT.
 *
 * This function traverse each entry in the LT and upgrade a specific field
 * of type enum to its new value (if mapping had changed). This function is
 * only being called for enum types that had changed.
 *
 * \param [in] unit The unit associated with the LT.
 * \param [in] tbl LT information.
 * \param [in] fid Field ID to identify the field to process.
 * \param [in] enum_type_name The enum type of the field.
 * \param [in] in_key_buf Buffer to store the input key.
 * \param [in] out_key_buf Buffer to store the output key.
 * \param [in] data_buf Buffer to store the entry content.
 *
 * \return SHR_E_NONE Success, error code otherwise.
 */
static int update_lt_enum_field(int unit,
                                table_info_t *tbl,
                                uint32_t fid,
                                const char *enum_type_name,
                                uint8_t *in_key_buf,
                                uint8_t *out_key_buf,
                                uint8_t *data_buf)
{
    bcmimm_be_fields_t be_out;
    bcmimm_be_fields_upd_t be_in;
    int rv;
    uint32_t data_field_cnt;
    size_t field_buf_size = field_buf[unit].count;
    bcmimm_table_dynamic_info_t *dyn_info;

    prep_buffers(tbl, data_buf, &be_out, &be_in, &data_field_cnt);
    /* Traverse every entry and upgrade the field enum. */
    rv = bcmimm_be_table_get_first(tbl->blk_hdl, false, out_key_buf, &be_out);
    if (SHR_FAILURE(rv)) {
        return SHR_E_NONE;   /* Table is empty */
    }
    do {
        if (tbl->type == IMM_BASIC) {
            bcmimm_extract_buf_into_flds(&be_out, tbl, &field_buf[unit]);
        } else {
            bcmimm_direct_extract_buf_into_flds(&be_out, tbl,
                                                &field_buf[unit]);
        }
        rv = update_entry_enum_field(unit, tbl, fid,
                                     enum_type_name, be_out.count);
        if (SHR_SUCCESS(rv) || rv == SHR_E_NOT_FOUND) {
            /* Prepare to update the entry with the new field value */
            field_buf[unit].count = be_out.count;
            sal_memset(data_buf, 0, tbl->data_bits_len);
            if (tbl->type == IMM_BASIC) {
                dyn_info = &bcmimm_dynamic_info[unit][tbl->fields_idx];
                be_in.count = bcmimm_compress_fields_into_buffer
                        (&field_buf[unit], NULL, tbl, dyn_info,
                         NULL, be_in.fdata, be_in.del, be_in.fid);
            } else {
                be_in.count = bcmimm_direct_compress_fields
                        (unit, tbl, &field_buf[unit], NULL,
                         NULL, be_in.fdata, be_in.del);
            }

            /*
             * be_in.fdata & fid points to the same buffer as be_out.fdata
             * & fid.
             */
            rv = bcmimm_be_entry_update(tbl->blk_hdl, out_key_buf, &be_in);
            if (SHR_FAILURE(rv)) {
                field_buf[unit].count = field_buf_size;
                return rv;
            }
            sal_memset(be_in.del, 0, sizeof(bool) * data_field_cnt);
        }
        sal_memcpy(in_key_buf, out_key_buf,
                   BCMIMM_BITS_TO_BYTES(tbl->key_arry_size));
        be_out.count = data_field_cnt;
        rv = bcmimm_be_table_get_next(tbl->blk_hdl, false, in_key_buf,
                                      out_key_buf, &be_out);
    } while (SHR_SUCCESS(rv));
    /* Restore the field_buff count */
    field_buf[unit].count = field_buf_size;
    return SHR_E_NONE;
}

/*!
 * \brief Upgrade an LT enum fields with changed definition.
 *
 * This function go through all the fields of a LT to search for enum type
 * fields. For enum fields it checks if the enum type definition had changed.
 * If it did, it update this field throughout the LT entries.
 *
 * \param [in] unit Unit associated with the LT.
 * \param [in] tbl The IMM structure associated with the LT.
 * \param [in] in_key_buf Buffer to store the input key.
 * \param [in] out_key_buf Buffer to store the output key.
 * \param [in] data_buf Buffer to store the entry content.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_MEMORY - Failure due to memory constraints.
 * \return SHR_E_UNAVAIL - The table could not be found.
 */
static int process_lt_enums(int unit,
                              table_info_t *tbl,
                              uint8_t *in_key_buf,
                              uint8_t *out_key_buf,
                              uint8_t *data_buf)
{
    size_t num_of_fields;
    bcmlrd_fid_t *field_array = NULL;
    unsigned int j;
    bcmlrd_field_def_t field_def;
    const char *enum_type_name;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmlrd_field_count_get(unit, tbl->sid, &num_of_fields));
    SHR_ALLOC(field_array, sizeof(bcmlrd_fid_t) * num_of_fields, "bcmimmFldEnum");
    SHR_NULL_CHECK(field_array, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (bcmlrd_field_list_get(unit, tbl->sid, num_of_fields,
                               field_array, &num_of_fields));

    for (j = 0; j < num_of_fields; j++) {
        SHR_IF_ERR_EXIT
            (bcmlrd_table_field_def_get(unit, tbl->sid,
                                        field_array[j], &field_def));
        if (field_def.symbol) {
            SHR_IF_ERR_EXIT
                (bcmlrd_enum_symbol_name(unit, tbl->sid,
                                         field_array[j], &enum_type_name));
            if (bcmissu_enum_has_changed(unit, enum_type_name)) {
                if (field_def.key) {
                    update_lt_enum_key_field(unit, tbl, field_array[j],
                                             enum_type_name,
                                             in_key_buf, out_key_buf,
                                             data_buf);
                } else {
                    update_lt_enum_field(unit, tbl, field_array[j],
                                         enum_type_name,
                                         in_key_buf, out_key_buf, data_buf);
                }
            }
        }
    }

exit:
    SHR_FREE(field_array);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update all the fields that are of type enum.
 *
 * This function goes through all the IMM LTs and upgrade their enum fields
 * if the enum definition had changed.
 * This function pre-allocate the necessary buffers to enable the processing of
 * each LT.
 *
 * \param [in] unit The unit to update its IMM LT enums.
 * \param [in] max_key_len Length of the longest key within the IMM tables.
 * \param [in] max_data_len Total data field length of the longest entry within
 * all the IMM tables.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int update_enums(int unit,
                        size_t max_key_len,
                        size_t max_data_len)
{
    uint32_t idx;
    table_info_t *fe_tbl;
    uint8_t *in_key_buf = NULL;
    uint8_t *out_key_buf = NULL;
    uint8_t *data_buf = NULL;
    bcmimm_table_dynamic_info_t *dyn_info;

    SHR_FUNC_ENTER(unit);

    SHR_ALLOC(in_key_buf, max_key_len * 2, "bcmimmIssuKey");
    SHR_NULL_CHECK(in_key_buf, SHR_E_MEMORY);
    out_key_buf = in_key_buf + max_key_len;
    SHR_ALLOC(data_buf, max_data_len, "bcmimmIssuData");
    SHR_NULL_CHECK(data_buf, SHR_E_MEMORY);
    for (idx = 0; idx < bcmimm_tables_list[unit]->allocated; idx++) {
        fe_tbl = &bcmimm_tables_list[unit]->tbl[idx];

        if (process_lt_enums(unit, fe_tbl, in_key_buf, out_key_buf, data_buf)) {
            dyn_info = &bcmimm_dynamic_info[unit][fe_tbl->fields_idx];
            LOG_WARN(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                              "ISSU enum conversion error LT=%s\n"),
                       dyn_info->tbl_name));
             return SHR_E_INTERNAL;
        }
    }

exit:
    SHR_FREE(in_key_buf);
    SHR_FREE(data_buf);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Free resources that were used during the IMM upgrade process.
 *
 * \param [in] field_buf Field buffer to free its resources.
 *
 * \return None.
 */
static void free_field_buf(bcmltd_fields_t *fld_buf)
{
    unsigned int j;

    if (!fld_buf->field) {
        return;
    }
    for (j = 0; j < fld_buf->count; j++) {
        if (fld_buf->field[j]) {
            shr_fmm_free(fld_buf->field[j]);
        }
    }
    SHR_FREE(fld_buf->field);
}

/*!
 * \brief Delete the table from the BE and set the table type.
 *
 * This function is being called when a table that was of one type
 * had changed to a different type.
 * It is assumed that the table keys remains the same, otherwise
 * the table is non issu-able and should be cleaned out.
 *
 * \param [in] unit Unit associated with the LT.
 * \param [in] tbl The LT management block.
 *
 * \return None.
 */
static void handle_table_handler_change(int unit, table_info_t *tbl)
{
    int rv;
    size_t key_size;

    if (tbl->key_fld_cnt == 0) {
        key_size = 1;  /* Key size must be minimum 1 byte */
    } else {
        key_size = BCMIMM_BITS_TO_BYTES(tbl->key_arry_size);
    }
    /* Let see which table we used to have and recreate it */
    if (tbl->type == IMM_BASIC) {
        rv = bcmimm_be_table_create
            (unit,
             BCMMGMT_IMM_FE_COMP_ID,
             tbl->sub,
             key_size,
             tbl->f_data_size,
             1,
             true,  /* warm = true */
             NULL,
             bcmimm_big_endian,
             (bcmimm_be_tbl_hdl_t *)&tbl->blk_hdl);
    } else {
        rv = bcmimm_be_direct_table_create
            (unit,
             BCMMGMT_IMM_FE_COMP_ID,
             tbl->sub,
             tbl->max_num_field,
             1,
             0,
             tbl->array_idx_size > 0,
             true,
             NULL,
             (bcmimm_be_tbl_hdl_t *)&tbl->blk_hdl);
    }
    if (SHR_SUCCESS(rv)) {
        bcmimm_be_table_free(tbl->blk_hdl);
    }
    tbl->blk_hdl = BCMIMM_BE_INVALID_TBL_HDL;
    if (tbl->type == IMM_BASIC) {
        tbl->type = IMM_DIRECT;
    } else {
        tbl->type = IMM_BASIC;
    }
}

/*!
 * \brief Get the current table dimensions.
 *
 * \param [in] unit The unit associated with the LT.
 * \param [in,out] tbl The LT table structure.
 * \param [in sid] The table ID.
 * \param [out] rows The maximal number of rows in the LT.
 *
 * \return SHR_E_NONE - Success, error code otherwise.
 */
static int get_current_tbl_dimension(int unit,
                                     table_info_t *tbl,
                                     uint32_t sid,
                                     size_t *rows)
{
    bcmlrd_client_field_info_t *f_info = NULL;
    const bcmlrd_table_rep_t *lt_info;
    size_t num_fid;
    bcmlrd_map_t const *map;
    uint32_t k;

    SHR_FUNC_ENTER(unit);
    lt_info = bcmlrd_table_get(sid);
    SHR_NULL_CHECK(lt_info, SHR_E_BADID);
    num_fid = lt_info->fields;
    SHR_ALLOC(f_info, sizeof(bcmlrd_client_field_info_t) * num_fid,
              "bcmimmInit");
    SHR_NULL_CHECK(f_info, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT
        (bcmlrd_table_fields_def_get(unit, lt_info->name,
                                     num_fid, f_info, &num_fid));

    tbl->key_fld_cnt = 0;
    tbl->key_arry_size = 0;
    tbl->data_fld_cnt = 0;
    SHR_IF_ERR_EXIT(bcmimm_table_dimension_find(unit, tbl, num_fid, f_info));

    *rows = 0;
    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &map));
    for (k = 0; k < map->table_attr->attributes; k++) {
        if (map->table_attr->attr[k].key ==
                BCMLRD_MAP_TABLE_ATTRIBUTE_ENTRY_LIMIT) {
            *rows = map->table_attr->attr[k].value;
            break;
        }
    }

exit:
    SHR_FREE(f_info);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Restore BE tables.
 *
 * Go through all the LT and restore those that are still exist. The
 * main purpose of this function is to enable further processing of field ID
 * and enum values upgrade.
 * Direct table get special treatment as their entry size is being tested and
 * adjusted if necessary.
 *
 * \param [in] unit The unit associated with this action.
 * \param [in] tables The current list of IMM LT IDs.
 * \param [in] imm_lts The number of IMM LTs (the size of \c tables] array.
 * \param [in] imm_direct The number of IMM direct tables.
 * \param [in,out] fe_sub_id The maximal sub-ID value.
 */
static int restore_be_tables(int unit, bcmlrd_sid_t *tables,
                             size_t imm_lts, size_t imm_direct,
                             shr_ha_mod_id *fe_sub_id)
{
    uint32_t idx;
    table_info_t *tbl;
    int rv;
    size_t key_size;
    uint32_t sid;
    size_t j, max_idx;
    bcmimm_table_dynamic_info_t *dyn_info;
    const bcmlrd_table_rep_t *lrd_tbl_info;
    bool delete_tbl;
    size_t rows = 0;
    size_t idx_in_tbls;

    for (idx = 0; idx < bcmimm_tables_list[unit]->allocated; idx++) {
        tbl = &bcmimm_tables_list[unit]->tbl[idx];

        /*
         * Find the new SID of the table. If the table is no longer exist
         * then skip to the next table.
         */
        do {
            delete_tbl = false;
            if (bcmissu_ltid_to_current(unit, tbl->sid, &sid) != SHR_E_NONE ||
                sid == BCMLTD_SID_INVALID) {
                delete_tbl = true;
            }
            /*
             * If the table is still available within the new version, verify
             * that the table is still IMM table by matching its sid to the IMM
             * tables sid.
             */
            idx_in_tbls = (size_t)(-1);
            if (!delete_tbl) {
                max_idx = imm_lts + imm_direct;
                for (j = 0; j < max_idx; j++) {
                    if (sid == tables[j]) {
                        idx_in_tbls = j;
                        break;
                    }
                }
                if (j == max_idx) {
                    delete_tbl = true;
                }
            }
            if (delete_tbl) {
                uint32_t last_idx = bcmimm_tables_list[unit]->allocated - 1;
                uint16_t fld_idx;
                handle_table_handler_change(unit, tbl);
                LOG_INFO(BSL_LOG_MODULE,
                         (BSL_META_U(unit, "ISSU old sid=%u not available\n"),
                          tbl->sid));
                bcmimm_tables_list[unit]->allocated--;
                bcmimm_tables_list[unit]->free_idx--;
                if (last_idx == idx) {
                    return SHR_E_NONE;
                }
                fld_idx = tbl->fields_idx;
                sal_memcpy(tbl,
                           &bcmimm_tables_list[unit]->tbl[last_idx],
                           sizeof(table_info_t));
                tbl->fields_idx = fld_idx;
            }
        } while (delete_tbl && idx < bcmimm_tables_list[unit]->allocated);
        /*
         * Since entries might be deleted let makes sure that the index is
         * still within range.
         */
        if (idx >= bcmimm_tables_list[unit]->allocated) {
           break;
        }
        assert(idx_in_tbls < imm_lts + imm_direct);

        dyn_info = &bcmimm_dynamic_info[unit][tbl->fields_idx];
        sal_memset(dyn_info, 0, sizeof(*dyn_info));
        lrd_tbl_info = bcmlrd_table_get(sid);
        if (lrd_tbl_info) {
            dyn_info->tbl_name = lrd_tbl_info->name;
        } else {
            dyn_info->tbl_name = NULL;
            continue;   /* There is nothing to do for this table */
        }
        /* Verify that the table type remains the same */
        if ((tbl->type == IMM_BASIC && idx_in_tbls >= imm_lts) ||
             (tbl->type == IMM_DIRECT && idx_in_tbls < imm_lts)) {
            handle_table_handler_change(unit, tbl);
            continue;   /* Allocation of BE table will happen later. */
        }

        rv = get_current_tbl_dimension(unit, tbl, sid, &rows);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        if (tbl->key_fld_cnt == 0) {
            key_size = 1;  /* Key size must be minimum 1 byte */
        } else {
            key_size = BCMIMM_BITS_TO_BYTES(tbl->key_arry_size);
        }
        if (tbl->type == IMM_BASIC) {
            rv = bcmimm_be_table_create
                (unit,
                 BCMMGMT_IMM_FE_COMP_ID,
                 tbl->sub,
                 key_size,
                 tbl->f_data_size,
                 1,
                 true,  /* warm = true */
                 NULL,
                 bcmimm_big_endian,
                 (bcmimm_be_tbl_hdl_t *)&tbl->blk_hdl);
        } else {
            rv = bcmimm_be_direct_table_create
                (unit,
                 BCMMGMT_IMM_FE_COMP_ID,
                 tbl->sub,
                 tbl->max_num_field,
                 rows,
                 0,
                 tbl->array_idx_size > 0,
                 true,
                 NULL,
                 (bcmimm_be_tbl_hdl_t *)&tbl->blk_hdl);
        }
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        if (*fe_sub_id <= tbl->sub) {
            *fe_sub_id = tbl->sub + 1;
        }
    }
    return SHR_E_NONE;
}

/*!
 * \brief Determine the buffer size that is sufficient to process every LT.
 *
 * This function goes through all the LTs to determine the buffer size that
 * is sufficient to process every IMM LT.
 * The function also allocates fields buffers.
 *
 * \param [in] unit The unit associated with this action.
 * \param [out] key_len Required key length.
 * \param [out] data_len Required data buffer length.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_MEMORY Insufficient memory for this operation.
 */
static int determine_buf_size(int unit,
                              size_t *key_len,
                              size_t *data_len)
{
    uint32_t idx;
    table_info_t *fe_tbl;
    uint32_t max_fields = 0;

    *key_len = 0;
    *data_len = 0;
    /*
     * Allocate data and key buffers. The buffers should be large enough to
     * contain every entry of every IMM LT.
     * The following loop finds the maximal buffers for key and data.
     */
    for (idx = 0; idx < bcmimm_tables_list[unit]->allocated; idx++) {
        fe_tbl = &bcmimm_tables_list[unit]->tbl[idx];
        if (*data_len < fe_tbl->data_bits_len) {
            *data_len = fe_tbl->data_bits_len;
        }
        if (*key_len < (size_t)BCMIMM_BITS_TO_BYTES(fe_tbl->key_arry_size)) {
            *key_len = BCMIMM_BITS_TO_BYTES(fe_tbl->key_arry_size);
        }
        if (max_fields < fe_tbl->max_num_field) {
            max_fields = fe_tbl->max_num_field;
        }
    }
    field_buf[unit].count = max_fields;
    field_buf[unit].field = sal_alloc(max_fields * sizeof(bcmltd_field_t *),
                                      "bcmimmIssuUpgr");
    if (!field_buf[unit].field) {
        return SHR_E_MEMORY;
    }
    for (idx = 0; idx < max_fields; idx++) {
        field_buf[unit].field[idx] = shr_fmm_alloc();
        if (!field_buf[unit].field[idx]) {
            return SHR_E_MEMORY;
        }
    }
    return SHR_E_NONE;
}

/*!
 * \brief Upgrade the field IDs of a given entry from a basic table.
 *
 * This function parses all the fields of an entry and update their
 * values with the current values. If a field was deleted it will receive
 * an INVALID ID. It is later overwrites the entry IDs with the upgraded
 * values. Those fields that has invalid ID will be deleted.
 *
 * \param [in] unit The unit associated with thie LT.
 * \param [in] tbl LT Data structure.
 * \param [in] count The number of field IDs in the entry.
 *
 * \return None.
 */
static void upgrade_entry_fields(int unit,
                                 table_info_t *tbl,
                                 bcmimm_be_fields_t *be_out,
                                 size_t data_len)
{
    bcmltd_field_t *fld;
    uint32_t j;
    uint32_t id;
    size_t field_buf_size = field_buf[unit].count;

    /* Normalize the BE data structure */
    if (tbl->type == IMM_BASIC) {
        bcmimm_extract_buf_into_flds(be_out, tbl, &field_buf[unit]);
    } else {
        bcmimm_direct_extract_buf_into_flds(be_out, tbl,
                                            &field_buf[unit]);
    }
    for (j = 0; j < be_out->count; j++) {
        fld = field_buf[unit].field[j];
        id = fld->id;
        if (bcmissu_fid_to_current(unit, tbl->sid, id, &id) != SHR_E_NONE) {
            /* The old field is not there anymore, mark it for deletion. */
            fld->id = BCMLTD_SID_INVALID;
        } else {
            fld->id = id;
        }
    }
    field_buf[unit].count = be_out->count;
    if (tbl->type == IMM_BASIC) {
        sal_memset(be_out->fid, 0, data_len);
        bcmimm_compress_fields_into_buffer(&field_buf[unit], NULL,
                                           tbl, NULL,
                                           NULL, be_out->fdata,
                                           NULL, be_out->fid);
    } else {
        sal_memset(be_out->fdata, 0, data_len);
        bcmimm_direct_compress_fields(unit, tbl, &field_buf[unit],
                                      NULL, NULL, be_out->fdata,
                                      NULL);
    }
    field_buf[unit].count = field_buf_size;
}

/*!
 *\brief Update back-end field IDs.
 *
 * This function updates the field IDs of all LT's that has their field ID
 * changed from previous versions. The field ID is being kept at the back end
 * along with other field attributes.
 *
 * \param [in] unit The unit associated with this DB.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int update_be_fid(int unit,
                         size_t max_key_len,
                         size_t max_data_len)
{
    uint32_t idx;
    table_info_t *fe_tbl;
    bcmimm_be_fields_t be_out;
    uint8_t *in_key_buf = NULL;
    uint8_t *out_key_buf = NULL;
    uint8_t *data_buf = NULL;
    uint32_t data_f_size;
    uint32_t max_fields;
    int rv;
    size_t field_buf_size = field_buf[unit].count;

    SHR_FUNC_ENTER(unit);

    SHR_ALLOC(in_key_buf, max_key_len * 2, "bcmimmIssuKey");
    SHR_NULL_CHECK(in_key_buf, SHR_E_MEMORY);
    out_key_buf = in_key_buf + max_key_len;
    SHR_ALLOC(data_buf, max_data_len, "bcmimmIssuData");
    SHR_NULL_CHECK(data_buf, SHR_E_MEMORY);

    be_out.fid = (uint32_t *)data_buf;
    for (idx = 0; idx < bcmimm_tables_list[unit]->allocated; idx++) {
        fe_tbl = &bcmimm_tables_list[unit]->tbl[idx];
        if (!bcmissu_lt_has_changed(unit, fe_tbl->sid)) {
            continue;
        }
        data_f_size = BCMIMM_BE_FID_SIZE + fe_tbl->f_data_size;
        max_fields = max_data_len / data_f_size;
        be_out.count = max_fields;
        if (fe_tbl->type == IMM_BASIC) {
            be_out.fdata = data_buf + BCMIMM_BE_FID_SIZE * be_out.count;
            be_out.fid = (uint32_t *)data_buf;
        } else {
            be_out.fdata = data_buf;
            be_out.fid = NULL;
        }
        /* Traverse every entry and replace its field IDs. */
        rv = bcmimm_be_table_get_first(fe_tbl->blk_hdl, false,
                                       out_key_buf, &be_out);
        if (SHR_FAILURE(rv)) {
            if (rv == SHR_E_NOT_FOUND) {
                continue;   /* Table is empty */
            } else {
                SHR_ERR_EXIT(rv);  /* Error */
            }
        }
        do {
            if (be_out.count > field_buf_size) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            upgrade_entry_fields(unit, fe_tbl, &be_out, max_data_len);

            bcmimm_be_entry_overwrite(fe_tbl->blk_hdl, out_key_buf, &be_out);
            sal_memcpy(in_key_buf, out_key_buf,
                       BCMIMM_BITS_TO_BYTES(fe_tbl->key_arry_size));
            be_out.count = max_fields;
            rv = bcmimm_be_table_get_next(fe_tbl->blk_hdl, false, in_key_buf,
                                          out_key_buf, &be_out);
        } while (SHR_SUCCESS(rv));
    }

exit:
    SHR_FREE(in_key_buf);
    SHR_FREE(data_buf);
    field_buf[unit].count = field_buf_size;
    SHR_FUNC_EXIT();
}

/*!
 *\brief Update logical table IDs.
 *
 * This function updates the LT IDs of all the IMM LT's.
 *
 * \param [in] unit The unit to update its IMM LT IDs.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int update_sids(int unit)
{
    uint32_t idx;
    table_info_t *fe_tbl;
    uint32_t sid;
    bcmimm_table_dynamic_info_t *dyn_info;

    for (idx = 0; idx < bcmimm_tables_list[unit]->allocated; idx++) {
        fe_tbl = &bcmimm_tables_list[unit]->tbl[idx];
        sid = fe_tbl->sid;
        /*
         * The conversion must succeed since we already did this at
         * the first step when restoring the back-end tables.
         */
        if (bcmissu_ltid_to_current(unit, sid, &sid) != SHR_E_NONE) {
            dyn_info = &bcmimm_dynamic_info[unit][fe_tbl->fields_idx];
            LOG_WARN(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                              "ISSU sid conversion error sid=%u LT=%s\n"),
                       sid, dyn_info->tbl_name));
             return SHR_E_INTERNAL;
        }
        fe_tbl->sid = sid;
    }
    return SHR_E_NONE;
}

/*******************************************************************************
 * External functions local to this module
 */
int bcmimm_issu_upgrade(int unit,
                        bcmlrd_sid_t *tables,
                        size_t imm_lts,
                        size_t imm_direct,
                        shr_ha_sub_id *fe_sub_id)
{
    size_t max_key_len, max_data_len;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(restore_be_tables(unit, tables, imm_lts,
                                      imm_direct, fe_sub_id));
    SHR_IF_ERR_EXIT(determine_buf_size(unit, &max_key_len, &max_data_len));
    SHR_IF_ERR_EXIT(update_be_fid(unit, max_key_len, max_data_len));
    SHR_IF_ERR_EXIT(update_sids(unit));
    SHR_IF_ERR_EXIT(update_enums(unit, max_key_len, max_data_len));

exit:
    free_field_buf(&field_buf[unit]);
    SHR_FUNC_EXIT();
}
